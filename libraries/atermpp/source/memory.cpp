#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <stdexcept>

#include <set>
#include <vector>
#include <string.h>
#include <sstream>


#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/detail/memory_utility.h"
#include "mcrl2/atermpp/detail/memory.h"
#include "mcrl2/atermpp/aterm.h"

#ifdef DMALLOC
#include <dmalloc.h>
#endif

namespace atermpp
{

/*{{{  globals */

namespace detail
{
  static size_t afun_table_class;
  static size_t afun_table_mask;
  static std::vector < size_t > function_symbol_hashtable;

  static size_t aterm_table_class;
  static size_t aterm_table_size;
  size_t aterm_table_mask;  // used in memory.h
  detail::_aterm* * aterm_hashtable;  // used in memory.h

  static const size_t INITIAL_AFUN_TABLE_CLASS = 14;
  static const size_t INITIAL_TERM_TABLE_CLASS = 17;


  static size_t AT_TABLE_SIZE(const size_t table_class)
  {
    return (size_t)1<<(table_class);
  }
  
  static size_t AT_TABLE_MASK(const size_t table_class)
  {
    return AT_TABLE_SIZE(table_class)-1;
  } 

  static size_t first_free=size_t(-1);;
  std::vector < detail::_function_symbol > at_lookup_table;
  
  aterm static_undefined_aterm;
  aterm static_empty_aterm_list;
 
}


/* The constants below are not static to prevent some compiler warnings */
const size_t MIN_TERM_SIZE = detail::TERM_SIZE_APPL(0);
const size_t INITIAL_MAX_TERM_SIZE = 256;


/* To change the block size, modify BLOCK_SHIFT only! */
static const size_t BLOCK_SHIFT = 13;

static const size_t BLOCK_SIZE = 1<<BLOCK_SHIFT;

typedef struct Block
{
  /* We need platform alignment for this data block! */
  size_t data[BLOCK_SIZE];

  size_t size;
#ifndef NDEBUG
  struct Block* next_by_size;
#endif

  size_t* end;
} Block;

typedef struct TermInfo
{
  Block*       at_block;
  size_t* top_at_blocks;
  detail::_aterm*       at_freelist;

  TermInfo():at_block(NULL),top_at_blocks(NULL),at_freelist(NULL)
  {}

} TermInfo;

static std::vector<TermInfo> terminfo(INITIAL_MAX_TERM_SIZE);

namespace detail
{
  constant_function_symbols function_adm;

void initialise_administration()
{
  // Explict initialisation on first use. This first
  // use is when a function symbol is created for the first time,
  // which may be due to the initialisation of a global variable in
  // a .cpp file, or due to the initialisation of a pre-main initialisation
  // of a static variable, which some compilers do.

  if (function_symbol_hashtable.size()==0)
  { 
    afun_table_class=detail::INITIAL_AFUN_TABLE_CLASS;
    afun_table_mask=AT_TABLE_MASK(detail::INITIAL_AFUN_TABLE_CLASS);
    function_symbol_hashtable=std::vector < size_t >(AT_TABLE_SIZE(detail::INITIAL_AFUN_TABLE_CLASS),size_t(-1));

    aterm_table_class=detail::INITIAL_TERM_TABLE_CLASS;
    aterm_table_size=detail::AT_TABLE_SIZE(detail::INITIAL_TERM_TABLE_CLASS);
    aterm_table_mask=detail::AT_TABLE_MASK(detail::INITIAL_TERM_TABLE_CLASS);

    aterm_hashtable=reinterpret_cast<detail::_aterm**>(calloc(aterm_table_size,sizeof(detail::_aterm*)));
    at_lookup_table.reserve(128);

    detail::function_adm.initialise_function_symbols();
  }
}

}  // namespace detail

static size_t total_nodes = 0;


static void remove_from_hashtable(const detail::_aterm *t)
{
  /* Remove the node from the aterm_hashtable */
  detail::_aterm *prev=NULL;
  const HashNumber hnr = hash_number(t, term_size(t)) & detail::aterm_table_mask;
  detail::_aterm *cur = detail::aterm_hashtable[hnr];

  do
  {
    if (!cur)
    {
      throw mcrl2::runtime_error("Internal error: cannot find term in hashtable."); // If only occurs if the internal administration is in error.
    }
    if (cur == t)
    {
      if (prev)
      {
        prev->next() = cur->next();
      }
      else
      {
        detail::aterm_hashtable[hnr] = cur->next();
      }
      /* Put the node in the appropriate free list */
      total_nodes--;
      return;
    }
  }
  while (((prev=cur), (cur=cur->next())));
  assert(0);
}

#ifndef NDEBUG
  bool check_that_all_objects_are_free();
#endif


/* Free a term, without removing it from the
   hashtable, and destroying its function symbol */
void detail::simple_free_term(detail::_aterm *t, const size_t arity)
{
  for(size_t i=0; i<arity; ++i)
  {
    reinterpret_cast<detail::_aterm_appl<aterm> *>(t)->arg[i].decrease_reference_count();
  }

  TermInfo &ti = terminfo[TERM_SIZE_APPL(arity)];
  t->next()  = ti.at_freelist;
  ti.at_freelist = t; 
}


void aterm::free_term() const
{
  detail::_aterm* t=this->m_term;
  assert(t->reference_count()==0);
  /* if (function().number()<4) // The default term and the empty list are not removed,
                             // as the datastructures may not exist anymore when this 
                             // happens. */
  if (detail::function_symbol_hashtable.size()==0)
  {
    return;
  } 
  remove_from_hashtable(t);  // Remove from hash_table

  for(size_t i=0; i<function().arity(); ++i)
  {
    reinterpret_cast<detail::_aterm_appl<aterm> *>(t)->arg[i].decrease_reference_count();
  }
#ifndef NDEBUG
  const size_t function_symbol_index=function().number();
  size_t ref_count=detail::at_lookup_table[function_symbol_index].reference_count;
#endif
  const size_t size=term_size(t);

  t->function().~function_symbol(); 

  TermInfo &ti = terminfo[size];
  t->next()  = ti.at_freelist;
  ti.at_freelist = t; 

#ifndef NDEBUG
  if (function_symbol_index==detail::function_adm.AS_INT.number() && ref_count==1) // When destroying ATempty, it appears that all other terms have been removed.
  {
    assert(check_that_all_objects_are_free());
    return;
  }
#endif
}

static void resize_aterm_hashtable()
{
  detail::aterm_table_class++;
  const size_t old_size=detail::aterm_table_size;
  detail::aterm_table_size = ((HashNumber)1)<<detail::aterm_table_class;
  detail::aterm_table_mask = (((HashNumber)1)<<detail::aterm_table_class)-1;
  detail::_aterm* * new_hashtable;

  {
    new_hashtable=reinterpret_cast<detail::_aterm**>(calloc(detail::aterm_table_size,sizeof(detail::_aterm*)));
  }
  
  if (new_hashtable==NULL)
  {
    mCRL2log(mcrl2::log::warning) << "could not resize hashtable to class " << detail::aterm_table_class << ". "; // << e.what() << std::endl;
    detail::aterm_table_class--;
    detail::aterm_table_size = ((HashNumber)1)<<detail::aterm_table_class;
    detail::aterm_table_mask = (((HashNumber)1)<<detail::aterm_table_class)-1;
    return;
  }
  
  /*  Rehash all old elements */
  for (size_t p=0; p<old_size; ++p) 
  {
    detail::_aterm* aterm_walker=detail::aterm_hashtable[p];

    while (aterm_walker)
    {
      assert(aterm_walker->reference_count()>0);
      detail::_aterm* next = aterm_walker->next();
      const HashNumber hnr = hash_number(aterm_walker, term_size(aterm_walker)) & detail::aterm_table_mask;
      aterm_walker->next() = new_hashtable[hnr];
      new_hashtable[hnr] = aterm_walker;
      assert(aterm_walker->next()!=aterm_walker);
      aterm_walker = next;
    }
  }
  free(detail::aterm_hashtable);
  detail::aterm_hashtable=new_hashtable;
}

#ifndef NDEBUG
bool check_that_all_objects_are_free()
{
  bool result=true;

  for(size_t size=0; size<terminfo.size(); ++size)
  {
    TermInfo *ti=&terminfo[size];
    for(Block* b=ti->at_block; b!=NULL; b=b->next_by_size)
    {
      for(detail::_aterm* p=(detail::_aterm*)b->data; p!=NULL && ((b==ti->at_block && p<(detail::_aterm*)ti->top_at_blocks) || p<(detail::_aterm*)b->end); p=p + size)
      {
        if (p->reference_count()!=0 && p->function()!=detail::function_adm.AS_EMPTY_LIST)
        {
          fprintf(stderr,"CHECK: Non free term %p (size %lu). ",&*p,size);
          fprintf(stderr,"Reference count %ld\n",p->reference_count());
          result=false;
        }
      }
    }
  }

  for(size_t i=0; i<detail::at_lookup_table.size(); ++i)
  {
    if (i!=detail::function_adm.AS_EMPTY_LIST.number() && detail::at_lookup_table[i].reference_count>0)  // ATempty is not destroyed, so is AS_EMPTY_LIST.
    {
      result=false;
      fprintf(stderr,"Symbol %s has positive reference count (nr. %ld, ref.count %ld)\n",
                detail::at_lookup_table[i].name.c_str(),i,detail::at_lookup_table[i].reference_count);
    }

  }

  return result;
}
#endif

/*}}}  */
/*{{{  static void allocate_block(size_t size)  */

static void allocate_block(size_t size)
{
  Block* newblock = (Block*)calloc(1, sizeof(Block));
  if (newblock == NULL)
  {
    std::runtime_error("allocate_block: out of memory!");
  }

  assert(size >= MIN_TERM_SIZE && size < terminfo.size());

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



detail::_aterm* detail::allocate_term(const size_t size)
{
  if (size >= terminfo.size())
  {
    terminfo.resize(size+1);
  }

  if (total_nodes>=(detail::aterm_table_size>>1))
  {
    // The hashtable is not big enough to hold nr_of_nodes_for_the_next_garbage_collect. So, resizing
    // is wise (although not necessary, due to the structure of the hastable, which allows is to contain
    // an arbitrary number of element, at some performance penalty.
    resize_aterm_hashtable();
  }

  detail::_aterm *at;
  TermInfo &ti = terminfo[size];
  if (ti.at_block && ti.top_at_blocks < ti.at_block->end)
  {
    /* the first block is not full: allocate a cell */
    at = (detail::_aterm *)ti.top_at_blocks;
    ti.top_at_blocks += size;
    at->reference_count()=0;
  }
  else if (ti.at_freelist)
  {
    /* the freelist is not empty: allocate a cell */
    at = ti.at_freelist;
    ti.at_freelist = ti.at_freelist->next();
    assert(ti.at_block != NULL);
    assert(ti.top_at_blocks == ti.at_block->end);
    assert(at->reference_count()==0);
  }
  else
  {
    /* there is no more memory of the current size allocate a block */
    allocate_block(size);
    assert(ti.at_block != NULL);
    at = (detail::_aterm *)ti.top_at_blocks;
    ti.top_at_blocks += size;
    at->reference_count()=0;
    new (&at->function()) function_symbol;  // placement new, as the memory calloc'ed.
  }

  total_nodes++;
  return at;
} 

aterm::aterm(const function_symbol &sym)
{
  detail::_aterm *cur, *prev, **hashspot;

  assert(sym.arity()==0);

  HashNumber hnr = sym.number();

  prev = NULL;
  hashspot = &(detail::aterm_hashtable[hnr & detail::aterm_table_mask]);

  cur = *hashspot;
  while (cur)
  {
    if (cur->function()==sym)
    {
      /* Promote current entry to front of hashtable */
      if (prev!=NULL)
      {
        prev->next() = cur->next();
        cur->next() = (detail::_aterm*) &**hashspot;
        *hashspot = cur;
      }

      m_term=cur;
      increase_reference_count<false>();
      return;
    }
    prev = cur;
    cur = cur->next();
  }

  cur = detail::allocate_term(detail::TERM_SIZE_APPL(0));
  /* Delay masking until after allocate */
  hnr &= detail::aterm_table_mask;
  new (&cur->function()) function_symbol(sym);
  
  cur->next() = &*detail::aterm_hashtable[hnr];
  detail::aterm_hashtable[hnr] = cur;

  m_term=cur;
  increase_reference_count<false>();
}


/**
 * Create an aterm_int
 */

namespace detail
{

_aterm* aterm_int(size_t val)
{
  HashNumber hnr = COMBINE(detail::function_adm.AS_INT.number(), val);

  detail::_aterm* cur = detail::aterm_hashtable[hnr & detail::aterm_table_mask];
  while (cur && (cur->function()!=detail::function_adm.AS_INT || (reinterpret_cast<detail::_aterm_int*>(cur)->value != val)))
  {
    cur = cur->next();
  }

  if (!cur)
  {
    cur = detail::allocate_term(TERM_SIZE_INT);
    /* Delay masking until after allocate */
    hnr &= detail::aterm_table_mask;
    new (&cur->function()) function_symbol(detail::function_adm.AS_INT);
    reinterpret_cast<detail::_aterm_int*>(cur)->value = val;

    cur->next() = detail::aterm_hashtable[hnr];
    detail::aterm_hashtable[hnr] = cur;
  }

  assert((hnr & detail::aterm_table_mask) == (hash_number(cur, TERM_SIZE_INT) & detail::aterm_table_mask));
  return cur;
}
} //namespace detail

/*{{{  defines */

static const size_t MAGIC_PRIME = 7;

char afun_id[] = "$Id$";

static HashNumber AT_hashAFun(const std::string &name, const size_t arity);

static void resize_function_symbol_hashtable()
{
  ++detail::afun_table_class;

  // detail::afun_table_size  = detail::AT_TABLE_SIZE(detail::afun_table_class);
  detail::afun_table_mask  = detail::AT_TABLE_MASK(detail::afun_table_class);

  detail::function_symbol_hashtable.clear();
  detail::function_symbol_hashtable.resize(detail::AT_TABLE_SIZE(detail::afun_table_class),size_t(-1));

  for (size_t i=0; i<detail::at_lookup_table.size(); i++)
  {
    detail::_function_symbol &entry = detail::at_lookup_table[i];
    assert(entry.reference_count>0);

    HashNumber hnr = AT_hashAFun(entry.name, entry.arity() );
    hnr &= detail::afun_table_mask;
    entry.next = detail::function_symbol_hashtable[hnr];
    detail::function_symbol_hashtable[hnr] = i;
  }
}


/**
 * Calculate the hash value of a symbol.
 */

static HashNumber AT_hashAFun(const std::string &name, const size_t arity)
{
  HashNumber hnr = arity*3;

  for (std::string::const_iterator i=name.begin(); i!=name.end(); i++)
  {
    hnr = 251 * hnr + *i;
  }

  return hnr*MAGIC_PRIME;
}


function_symbol::function_symbol(const std::string &name, const size_t arity)
{
  if (detail::function_symbol_hashtable.size()==0)
  {
    detail::initialise_administration();
  }
  const HashNumber hnr = AT_hashAFun(name, arity) & detail::afun_table_mask;
  /* Find symbol in table */
  size_t cur = detail::function_symbol_hashtable[hnr];
  while (cur!=size_t(-1) && !(detail::at_lookup_table[cur].arity()==arity &&
                              detail::at_lookup_table[cur].name==name))
  {
    cur = detail::at_lookup_table[cur].next;
  }

  if (cur == size_t(-1))
  {
    const size_t free_entry = detail::first_free;
    assert(detail::at_lookup_table.size()<detail::function_symbol_hashtable.size());

    if (free_entry!=size_t(-1)) // There is a free place in at_lookup_table() to store an function_symbol.
    {
      assert(detail::first_free<detail::at_lookup_table.size());
      cur=detail::first_free;
      detail::first_free = detail::at_lookup_table[detail::first_free].next;
      assert(detail::first_free==size_t(-1) || detail::first_free<detail::at_lookup_table.size());
      assert(free_entry<detail::at_lookup_table.size());
      assert(detail::at_lookup_table[cur].reference_count==0);
      detail::at_lookup_table[cur].reference_count=0;
      detail::at_lookup_table[cur].m_arity = arity;
      detail::at_lookup_table[cur].name = name;
    }
    else
    {
      cur = detail::at_lookup_table.size();
      detail::at_lookup_table.push_back(detail::_function_symbol(name,arity));
    }



    detail::at_lookup_table[cur].next = detail::function_symbol_hashtable[hnr];
    detail::function_symbol_hashtable[hnr] = cur;
  }

  m_number=cur;
  increase_reference_count<false>();

  if (detail::at_lookup_table.size()>=detail::function_symbol_hashtable.size())
  {
    resize_function_symbol_hashtable();
  }
}


void function_symbol::free_function_symbol() const
{
  if (detail::function_symbol_hashtable.size()==0)
  {
    // The aterm administration is destroyed. We cannot remove
    // this afun anymore.
    return;
  }
  assert(m_number<detail::at_lookup_table.size());
  const detail::_function_symbol &sym=detail::at_lookup_table[m_number];

  assert(!sym.name.empty());

  /* Calculate hashnumber */
  const HashNumber hnr = AT_hashAFun(sym.name, sym.arity()) & detail::afun_table_mask;

  /* Update hashtable */
  if (detail::function_symbol_hashtable[hnr] == m_number)
  {
    detail::function_symbol_hashtable[hnr] = sym.next;
  }
  else
  {
    size_t cur;
    size_t prev = detail::function_symbol_hashtable[hnr];
    for (cur = detail::at_lookup_table[prev].next; cur != m_number; prev = cur, cur = detail::at_lookup_table[cur].next)
    {
      assert(cur != size_t(-1));
    }
    detail::at_lookup_table[prev].next = detail::at_lookup_table[cur].next;
  }

  assert(m_number<detail::at_lookup_table.size());
  detail::at_lookup_table[m_number].next = detail::first_free;
  detail::first_free = m_number;
}

} // namespace atermpp

