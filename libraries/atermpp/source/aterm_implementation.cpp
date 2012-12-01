#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <stdexcept>

#include <set>
#include <string.h>
#include <sstream>


#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/detail/memory_utility.h"
#include "mcrl2/atermpp/detail/aterm_implementation.h"
#include "mcrl2/atermpp/detail/aterm_int.h"
#include "mcrl2/atermpp/detail/aterm_appl.h"


#ifdef DMALLOC
#include <dmalloc.h>
#endif

namespace atermpp
{


namespace detail
{

// The hashtables are not vectors to prevent them from being
// destroyed prematurely.

static const size_t INITIAL_TERM_TABLE_SIZE = 1<<17;  // Must be a power of 2.
size_t aterm_table_size;
size_t aterm_table_mask;            // used in memory.h
const _aterm* * aterm_hashtable;  // used in memory.h

aterm static_undefined_aterm;
aterm static_empty_aterm_list(detail::aterm0(detail::function_adm.AS_EMPTY_LIST));

/* The constants below are not static to prevent some compiler warnings */
const size_t INITIAL_MAX_TERM_SIZE = 256;

// The following is not a vector to avoid that it is prematurely destroyed.
size_t terminfo_size=0;
TermInfo *terminfo;

size_t total_nodes = 0;


void resize_aterm_hashtable()
{
  const size_t old_size=aterm_table_size;
  aterm_table_size <<=1; // Double the size.
  const _aterm* * new_hashtable;

  {
    new_hashtable=reinterpret_cast<const _aterm**>(calloc(aterm_table_size,sizeof(_aterm*)));
  }
  
  if (new_hashtable==NULL)
  {
    mCRL2log(mcrl2::log::warning) << "could not resize hashtable to size " << aterm_table_size << ". "; 
    aterm_table_size = old_size;
    return;
  }
  aterm_table_mask = aterm_table_size-1;
  
  /*  Rehash all old elements */
  for (size_t p=0; p<old_size; ++p) 
  {
    const _aterm* aterm_walker=aterm_hashtable[p];

    while (aterm_walker)
    {
      assert(aterm_walker->reference_count()>0);
      const _aterm* next = aterm_walker->next();
      const HashNumber hnr = hash_number(aterm_walker) & aterm_table_mask;
      aterm_walker->set_next(new_hashtable[hnr]);
      new_hashtable[hnr] = aterm_walker;
      assert(aterm_walker->next()!=aterm_walker);
      aterm_walker = next;
    }
  }
  free(aterm_hashtable);
  aterm_hashtable=new_hashtable;
}

#ifndef NDEBUG
static void check_that_all_objects_are_free()
{
  bool result=true;

  for(size_t size=0; size<terminfo_size; ++size)
  {
    TermInfo *ti=&terminfo[size];
    for(Block* b=ti->at_block; b!=NULL; b=b->next_by_size)
    {
      for(_aterm* p=(_aterm*)b->data; p!=NULL && ((b==ti->at_block && p<(_aterm*)ti->top_at_blocks) || p<(_aterm*)b->end); p=p + size)
      {
        if (p->reference_count()!=0 &&
            ((p->function()!=function_adm.AS_DEFAULT && p->function()!=function_adm.AS_EMPTY_LIST) || p->reference_count()>1))
        {
          std::cerr << "CHECK: Non free term " << p << " (size " << size << "). ";
          std::cerr << "Reference count " << p->reference_count() << " nr. " << p->function().number() << ". ";
          std::cerr << "Func: " << p->function().name() << ". Arity: " << p->function().arity() << ".\n";
          result=false;
        }
        
      }
    }
  }

  // Check the function symbols. The first four function symbols
  // can be constructed twice in the same spot (function_symbol_constants.h)
  // and only destroyed once and therefore their reference counts can be 1 at 
  // termination. The function symbols with number 0 and 3 even can have reference
  // count 2, because the terms containing may still exist as they are also constructed
  // in a nonderministic fashion using a placement new. So, they can be constructed
  // without properly being destroyed, increasing the reference count of the function
  // symbols in it by 1.
  for(size_t i=0; i<function_lookup_table_size; ++i) 
  {
    if (!(function_lookup_table[i].reference_count==0 ||
          (i==0 && function_lookup_table[i].reference_count<=2) || //AS_DEFAULT
          (i==1 && function_lookup_table[i].reference_count<=1) || //AS_INT
          (i==2 && function_lookup_table[i].reference_count<=1) || //AS_LIST
          (i==3 && function_lookup_table[i].reference_count<=2)))  //AS_EMPTY_LIST
    {
      std::cerr << "Symbol " << function_lookup_table[i].name << " has positive reference count (nr. " <<
                 i << ", ref.count " << function_lookup_table[i].reference_count << ")\n";
      result=false;
    }

  }
  assert(result);
}
#endif

void initialise_aterm_administration()
{
  // Explict initialisation on first use. This first
  // use is when a function symbol is created for the first time,
  // which may be due to the initialisation of a global variable in
  // a .cpp file, or due to the initialisation of a pre-main initialisation
  // of a static variable, which some compilers do.
  aterm_table_size=INITIAL_TERM_TABLE_SIZE;
  aterm_table_mask=aterm_table_size-1;

  aterm_hashtable=reinterpret_cast<const _aterm**>(calloc(aterm_table_size,sizeof(_aterm*)));
  if (aterm_hashtable==NULL)
  {
    throw std::runtime_error("Out of memory. Cannot create an aterm symbol hashtable.");
  }

  terminfo_size=INITIAL_MAX_TERM_SIZE;
  terminfo=reinterpret_cast<TermInfo*>(malloc(terminfo_size*sizeof(TermInfo)));
  if (terminfo==NULL)
  {
    throw std::runtime_error("Out of memory. Failed to allocate the terminfo array.");
  }

  for(size_t i=0; i<terminfo_size; ++i)
  {
    new (&terminfo[i]) TermInfo();
  }

  new (&detail::static_undefined_aterm) aterm(detail::function_adm.AS_DEFAULT); // Use placement new as static_undefined_aterm
                                                                                // may not have initialised when this is called, 
                                                                                // causing a problem with reference counting.
    
  new (&detail::static_empty_aterm_list) aterm(detail::function_adm.AS_EMPTY_LIST); // Use placement new as static_empty_atermlist
                                                                                 // may not have initialised when this is called, 
                                                                                 // causing a problem with reference counting.
  
  // Check at exit that all function symbols and terms have been cleaned up properly.
// #ifdef this_sanity_check_fails_on_some_machines_and_needs_to_be_investigated_further
  assert(!atexit(check_that_all_objects_are_free)); // zero is returned when registering is successful.
// #endif
  
}


void allocate_block(const size_t size)
{
  Block* newblock = (Block*)calloc(1, sizeof(Block));
  if (newblock == NULL)
  {
    std::runtime_error("Out of memory. Could not allocate a block of memory to store terms.");
  }

  assert(size < terminfo_size);

  TermInfo &ti = terminfo[size];

  newblock->end = (newblock->data) + (BLOCK_SIZE - (BLOCK_SIZE % size));

  newblock->size = size;
#ifndef NDEBUG
  newblock->next_by_size = ti.at_block;
#endif
  ti.at_block = newblock;
  ti.top_at_blocks = newblock->data;
  assert(ti.at_block != NULL);
  assert(ti.at_freelist == NULL);
}

} // namespace detail

} // namespace atermpp

