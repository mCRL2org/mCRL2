/*{{{  includes */

#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <stdexcept>

#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/detail/memory_utility.h"
#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/detail/memory.h"
#include "mcrl2/atermpp/detail/util.h"

#ifdef DMALLOC
#include <dmalloc.h>
#endif

namespace atermpp
{

/*{{{  globals */

char memory_id[] = "$Id$";

std::vector<TermInfo> terminfo(INITIAL_MAX_TERM_SIZE);

size_t total_nodes = 0;

static size_t table_class = INITIAL_TERM_TABLE_CLASS;
static HashNumber table_size    = AT_TABLE_SIZE(INITIAL_TERM_TABLE_CLASS);
HashNumber table_mask    = AT_TABLE_MASK(INITIAL_TERM_TABLE_CLASS);

std::vector <detail::_aterm*> detail::hashtable(AT_TABLE_SIZE(INITIAL_TERM_TABLE_CLASS),NULL);

static void resize_hashtable()
{
  HashNumber oldsize;

  oldsize = table_size;
  table_class++;
  table_size = ((HashNumber)1)<<table_class;
  table_mask = table_size-1;
  std::vector < detail::_aterm* > new_hashtable;

  /*  Create new term table */
  try
  {
    new_hashtable.resize(table_size,NULL);
  }
  catch (std::bad_alloc &e)
  {
    mCRL2log(mcrl2::log::warning) << "could not resize hashtable to class " << table_class << ". " << e.what() << std::endl;
    table_class--;
    table_size = ((HashNumber)1)<<table_class;
    table_mask = table_size-1;
    return;
  }
  
  /*  Rehash all old elements */

  for (std::vector < detail::_aterm*>::const_iterator p=detail::hashtable.begin(); p !=detail::hashtable.end(); p++)
  {
    detail::_aterm* aterm_walker=*p;

    while (aterm_walker)
    {
      assert(aterm_walker->reference_count>0);
      detail::_aterm* next = aterm_walker->next;
      const HashNumber hnr = hash_number(aterm_walker, term_size(aterm_walker)) & table_mask;
      assert(hnr<new_hashtable.size());
      aterm_walker->next = new_hashtable[hnr];
      new_hashtable[hnr] = aterm_walker;
      assert(aterm_walker->next!=aterm_walker);
      aterm_walker = next;
    }
  }
  new_hashtable.swap(detail::hashtable);

}

#ifndef NDEBUG
static bool check_that_all_objects_are_free()
{
  return true;
  bool result=true;

  for(size_t size=0; size<terminfo.size(); ++size)
  {
    TermInfo *ti=&terminfo[size];
    for(Block* b=ti->at_block; b!=NULL; b=b->next_by_size)
    {
      for(detail::_aterm* p=(detail::_aterm*)b->data; p!=NULL && ((b==ti->at_block && p<(detail::_aterm*)ti->top_at_blocks) || p<(detail::_aterm*)b->end); p=p + size)
      {
        if (p->reference_count!=0 && p->m_function_symbol!=AS_EMPTY_LIST)
        {
          fprintf(stderr,"CHECK: Non free term %p (size %lu). ",&*p,size);
          fprintf(stderr,"Reference count %ld\n",p->reference_count);
          result=false;
        }
      }
    }
  }

  for(size_t i=0; i<function_symbol::at_lookup_table.size(); ++i)
  {
    if (i!=AS_EMPTY_LIST.number() && function_symbol::at_lookup_table[i]->reference_count>0)  // ATempty is not destroyed, so is AS_EMPTY_LIST.
    {
      result=false;
      fprintf(stderr,"Symbol %s has positive reference count (nr. %ld, ref.count %ld)\n",
                function_symbol::at_lookup_table[i]->name.c_str(),function_symbol::at_lookup_table[i]->id,function_symbol::at_lookup_table[i]->reference_count);
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
  assert(((size_t)ti.top_at_blocks % MAX(sizeof(double), sizeof(void*))) == 0);

  assert(ti.at_freelist == NULL);
}



detail::_aterm* detail::allocate_term(const size_t size)
{
  if (size >= terminfo.size())
  {
    terminfo.resize(size+1);
  }

  if (total_nodes>=table_size)
  {
    // The hashtable is not big enough to hold nr_of_nodes_for_the_next_garbage_collect. So, resizing
    // is wise (although not necessary, due to the structure of the hastable, which allows is to contain
    // an arbitrary number of element, at some performance penalty.
    resize_hashtable();
  }

  detail::_aterm *at;
  TermInfo &ti = terminfo[size];
  if (ti.at_block && ti.top_at_blocks < ti.at_block->end)
  {
    /* the first block is not full: allocate a cell */
    at = (detail::_aterm *)ti.top_at_blocks;
    ti.top_at_blocks += size;
    at->reference_count=0;
    new (&at->m_function_symbol) function_symbol;  // placement new, as the memory calloc'ed.
  }
  else if (ti.at_freelist)
  {
    /* the freelist is not empty: allocate a cell */
    at = ti.at_freelist;
    ti.at_freelist = ti.at_freelist->next;
    assert(ti.at_block != NULL);
    assert(ti.top_at_blocks == ti.at_block->end);
    assert(at->reference_count==0);
  }
  else
  {
    /* there is no more memory of the current size allocate a block */
    allocate_block(size);
    assert(ti.at_block != NULL);
    at = (detail::_aterm *)ti.top_at_blocks;
    ti.top_at_blocks += size;
    at->reference_count=0;
    new (&at->m_function_symbol) function_symbol;  // placement new, as the memory calloc'ed.
  }

  total_nodes++;
  return at;
} 

aterm::aterm(const function_symbol &sym)
{
  detail::_aterm *cur, *prev, **hashspot;
  HashNumber hnr;


  assert(sym.arity()==0);

  hnr = FINISH(START(sym.number()));

  prev = NULL;
  hashspot = &(detail::hashtable[hnr & table_mask]);

  cur = *hashspot;
  while (cur)
  {
    if (cur->m_function_symbol==sym)
    {
      /* Promote current entry to front of hashtable */
      if (prev!=NULL)
      {
        prev->next = cur->next;
        cur->next = (detail::_aterm*) &**hashspot;
        *hashspot = cur;
      }

      m_term=cur;
      increase_reference_count<false>(m_term);
      return;
    }
    prev = cur;
    cur = cur->next;
  }

  cur = detail::allocate_term(TERM_SIZE_APPL(0));
  /* Delay masking until after allocate */
  hnr &= table_mask;
  cur->m_function_symbol = sym;
  cur->next = &*detail::hashtable[hnr];
  detail::hashtable[hnr] = cur;

  m_term=cur;
  increase_reference_count<false>(m_term);
  // return reinterpret_cast<detail::_aterm_appl<Term> *>(cur);
}


/**
 * Create an aterm_int
 */

aterm_int::aterm_int(int val)
{
  detail::_aterm* cur;
  /* The following emulates the encoding trick that is also used in the definition
   * of aterm_int. Not using a union here leads to incorrect hashing results.
   */
  union
  {
    int value;
    size_t reserved;
  } _val;

  _val.reserved = 0;
  _val.value = val;

  // header_type header = INT_HEADER;
  HashNumber hnr = START(AS_INT.number());
  hnr = COMBINE(hnr, _val.reserved);
  hnr = FINISH(hnr);

  cur = detail::hashtable[hnr & table_mask];
  while (cur && (cur->m_function_symbol!=AS_INT || (reinterpret_cast<detail::_aterm_int*>(cur)->value != _val.value)))
  {
    cur = cur->next;
  }

  if (!cur)
  {
    cur = detail::allocate_term(TERM_SIZE_INT);
    /* Delay masking until after allocate */
    hnr &= table_mask;
    cur->m_function_symbol = AS_INT;
    reinterpret_cast<detail::_aterm_int*>(cur)->reserved = _val.reserved;
    // reinterpret_cast<detail::_aterm_int*>(cur)->value = _val.value;

    cur->next = detail::hashtable[hnr];
    detail::hashtable[hnr] = cur;
  }

  assert((hnr & table_mask) == (hash_number(cur, TERM_SIZE_INT) & table_mask));
  m_term=cur;
  increase_reference_count<false>(m_term);
}

/*}}}  */

} // namespace atermpp

