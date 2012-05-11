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

static size_t total_nodes = 0;

static size_t table_class = INITIAL_TERM_TABLE_CLASS;
static HashNumber table_size    = AT_TABLE_SIZE(INITIAL_TERM_TABLE_CLASS);
HashNumber table_mask    = AT_TABLE_MASK(INITIAL_TERM_TABLE_CLASS);

std::vector <_ATerm*> aterm::hashtable(AT_TABLE_SIZE(INITIAL_TERM_TABLE_CLASS),NULL);

/**
 * Calculate the size (in words) of a term.
 */

static size_t term_size(const _ATerm *t)
{
  if (t->m_function_symbol.number()==AS_INT.number())
  { 
    return TERM_SIZE_INT;
  }
  return TERM_SIZE_APPL(t->m_function_symbol.arity());
}

/*}}}  */

/*{{{  static HashNumber hash_number(aterm t, size_t size) */

static HashNumber hash_number(const _ATerm *t, const size_t size)
{
  HashNumber hnr;

  // hnr = START(t->word[0]);
  hnr = START(t->m_function_symbol.number());

  for (size_t i=ARG_OFFSET; i<size; i++)
  {
    // hnr = COMBINE(hnr, t->word[i]);
    hnr = COMBINE(hnr, *(reinterpret_cast<const MachineWord *>(t) + i));
  }

  return FINISH(hnr);
}


/*}}}  */
/*{{{  HashNumber AT_hashnumber(aterm t) */

HashNumber AT_hashnumber(const _ATerm *t)
{
  return hash_number(t, term_size(t));
}

/*}}}  */

/*{{{  static void resize_hashtable() */

/**
 * Resize the hashtable
 */

static void resize_hashtable()
{
  HashNumber oldsize;

  oldsize = table_size;
  table_class++;
  table_size = ((HashNumber)1)<<table_class;
  table_mask = table_size-1;
  std::vector < _ATerm* > new_hashtable;

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

  for (std::vector < _ATerm*>::const_iterator p=aterm::hashtable.begin(); p !=aterm::hashtable.end(); p++)
  {
    _ATerm* aterm_walker=*p;

    while (aterm_walker)
    {
      assert(aterm_walker->reference_count>0);
      _ATerm* next = aterm_walker->next;
      const HashNumber hnr = hash_number(aterm_walker, term_size(aterm_walker)) & table_mask;
      assert(hnr<new_hashtable.size());
      aterm_walker->next = new_hashtable[hnr];
      new_hashtable[hnr] = aterm_walker;
      assert(aterm_walker->next!=aterm_walker);
      aterm_walker = next;
    }
  }
  new_hashtable.swap(aterm::hashtable);

}


static bool check_that_all_objects_are_free()
{
  return true;
  bool result=true;

  for(size_t size=0; size<terminfo.size(); ++size)
  {
    TermInfo *ti=&terminfo[size];
    for(Block* b=ti->at_block; b!=NULL; b=b->next_by_size)
    {
      for(_ATerm* p=(_ATerm*)b->data; p!=NULL && ((b==ti->at_block && p<(_ATerm*)ti->top_at_blocks) || p<(_ATerm*)b->end); p=p + size)
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
                function_symbol::at_lookup_table[i]->name,function_symbol::at_lookup_table[i]->id,function_symbol::at_lookup_table[i]->reference_count);
    }

  }

  return result;
}

/*}}}  */
/*{{{  void AT_cleanupMemory() */

/**
 * Print hashtable info
 */

/* void AT_cleanupMemory()
//{
  // AT_free(hashtable);
  // AT_free_protected_blocks();
  // check_that_all_objects_are_free();
  // free(terminfo); 
// }

/ *}}}  */

/**
 * Allocate a new block of a particular size class
 */

// header_type* min_heap_address = (header_type*)(~0);
// header_type* max_heap_address = 0;

/*{{{  static void allocate_block(size_t size)  */

static void allocate_block(size_t size)
{
  // size_t idx;
  Block* newblock;
  // bool init = false;
  TermInfo* ti;

/*  if (at_freeblocklist != NULL)
  {
    newblock = at_freeblocklist;
    at_freeblocklist = at_freeblocklist->next_by_size;
    at_freeblocklist_size--;
  }
  else 
  { */
    newblock = (Block*)calloc(1, sizeof(Block));
    if (newblock == NULL)
    {
      std::runtime_error("allocate_block: out of memory!");
    }
    // init = true;
    /* min_heap_address = MIN(min_heap_address,(newblock->data));
    max_heap_address = MAX(max_heap_address,(newblock->data+BLOCK_SIZE));
    assert(min_heap_address < max_heap_address); */
  // }

  assert(size >= MIN_TERM_SIZE && size < terminfo.size());

  ti = &terminfo[size];

  newblock->end = (newblock->data) + (BLOCK_SIZE - (BLOCK_SIZE % size));

  newblock->size = size;
  newblock->next_by_size = ti->at_block;
  ti->at_block = newblock;
  ti->top_at_blocks = newblock->data;
  assert(ti->at_block != NULL);
  assert(((size_t)ti->top_at_blocks % MAX(sizeof(double), sizeof(void*))) == 0);

  /* [pem: Feb 14 02] TODO: fast allocation */
  assert(ti->at_freelist == NULL);

  /* if (init)
  {
    / * TODO: optimize * /
    / * Place the new block in the block_table * /
    / *idx = (((MachineWord)newblock) >> (BLOCK_SHIFT+2)) % BLOCK_TABLE_SIZE;* /
    idx = ADDR_TO_BLOCK_IDX(newblock);
    newblock->next_after = block_table[idx].first_after;
    block_table[idx].first_after = newblock;
    idx = (idx+1) % BLOCK_TABLE_SIZE;
    newblock->next_before = block_table[idx].first_before;
    block_table[idx].first_before = newblock;
  } */
}

/*}}}  */


/*{{{  aterm AT_allocate(size_t size)  */

_ATerm* AT_allocate(const size_t size)
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

  _ATerm *at;
  TermInfo *ti = &terminfo[size];
  if (ti->at_block && ti->top_at_blocks < ti->at_block->end)
  {
    /* the first block is not full: allocate a cell */
    at = (_ATerm *)ti->top_at_blocks;
    ti->top_at_blocks += size;
    at->reference_count=0;
    new (&at->m_function_symbol) function_symbol;  // placement new, as the memory calloc'ed.
  }
  else if (ti->at_freelist)
  {
    /* the freelist is not empty: allocate a cell */
    at = ti->at_freelist;
    ti->at_freelist = ti->at_freelist->next;
    assert(ti->at_block != NULL);
    assert(ti->top_at_blocks == ti->at_block->end);
    assert(at->reference_count==0);
  }
  else
  {
    /* there is no more memory of the current size allocate a block */
    allocate_block(size);
    assert(ti->at_block != NULL);
    at = (_ATerm *)ti->top_at_blocks;
    ti->top_at_blocks += size;
    at->reference_count=0;
    new (&at->m_function_symbol) function_symbol;  // placement new, as the memory calloc'ed.
  }

  total_nodes++;
  return at;
} 

/*}}}  */

/*{{{  void AT_freeTerm(size_t size, aterm t) */

/**
 * Free a term of a particular size.
 */

void AT_freeTerm(const size_t size, _ATerm *t)
{
  // fprintf(stderr,"Remove term from hashtable %p\n",t);
  _ATerm *prev=NULL, *cur;

  /* Remove the node from the hashtable */
  const HashNumber hnr = hash_number(t, size) & table_mask;
  cur = &*aterm::hashtable[hnr];

  do
  {
    if (!cur)
    {
        throw std::runtime_error("AT_freeTerm: cannot find term in hashtable at pos " + to_string(hnr) + " function_symbol=" + to_string(t->m_function_symbol.number()));
    }
    if (cur == t)
    {
      if (prev)
      {
        prev->next = cur->next;
      }
      else
      {
        aterm::hashtable[hnr] = cur->next;
      }
      /* Put the node in the appropriate free list */
      total_nodes--;
      return;
    }
  }
  while (((prev=cur), (cur=cur->next)));
  assert(0);
}

/*}}}  */

void at_reduce_reference_count(_ATerm *t);

void at_free_term(_ATerm *t)
{
#ifndef NDEBUG
  if (t->m_function_symbol==AS_EMPTY_LIST) // When destroying ATempty, it appears that all other terms have been removed.
  {
    check_that_all_objects_are_free();
    return;
  }
#endif

  assert(t->reference_count==0);
  const size_t size=term_size(t);
  AT_freeTerm(size,t);  // Remove from hash_table

  // Reduce the reference count of subterms.
  /* if (ATgetType(t)==AT_APPL)
  {
    const size_t afun_number=ATgetAFun(reinterpret_cast<_ATermAppl*>(t));
  */
  for(size_t i=0; i<t->m_function_symbol.arity(); ++i)
  {
    at_reduce_reference_count(reinterpret_cast<_ATermAppl*>(t)->arg[i]);
  }
  t->m_function_symbol=function_symbol(); // AFun::decrease_reference_count(afun_number);

  /* else if (t->m_function_symbol==AS_LIST)
  {
    at_reduce_reference_count(reinterpret_cast<_ATermList*>(t)->head);
    at_reduce_reference_count(reinterpret_cast<_ATermList*>(t)->tail);
  } */
  // t->header = FREE_HEADER;
  TermInfo *ti = &terminfo[size];
  t->next  = ti->at_freelist;
  ti->at_freelist = t; 
}
          
void at_reduce_reference_count(_ATerm *t)
{
  assert(t->reference_count>0);
  if (0== --t->reference_count)
  {
    at_free_term(t);
  }
}


/*{{{  ATermAppl ATmakeAppl0(function_symbol &sym) */

// ATermAppl ATmakeAppl0(const function_symbol &sym)

aterm::aterm(const function_symbol &sym)
{
  _ATerm *cur, *prev, **hashspot;
  HashNumber hnr;


  CHECK_ARITY(sym.arity(), 0);

  hnr = FINISH(START(sym.number()));

  prev = NULL;
  hashspot = &(aterm::hashtable[hnr & table_mask]);

  cur = *hashspot;
  while (cur)
  {
    if (cur->m_function_symbol==sym)
    {
      /* Promote current entry to front of hashtable */
      if (prev!=NULL)
      {
        prev->next = cur->next;
        cur->next = (_ATerm*) &**hashspot;
        *hashspot = cur;
      }

      m_term=cur;
      increase_reference_count<false>(m_term);
      return;
    }
    prev = cur;
    cur = cur->next;
  }

  cur = (_ATermAppl*) &*AT_allocate(TERM_SIZE_APPL(0));
  /* Delay masking until after AT_allocate */
  hnr &= table_mask;
  cur->m_function_symbol = sym;
  // function_symbol::increase_reference_count<true>(sym.number());
  cur->next = &*aterm::hashtable[hnr];
  aterm::hashtable[hnr] = cur;

  m_term=cur;
  increase_reference_count<false>(m_term);
  // return reinterpret_cast<_ATermAppl*>(cur);
}

/*}}}  */

/*{{{  ATermInt ATmakeInt(int val) */

/**
 * Create an ATermInt
 */

// ATermInt ATmakeInt(const int val)
aterm_int::aterm_int(int val)
{
  _ATerm* cur;
  /* The following emulates the encoding trick that is also used in the definition
   * of ATermInt. Not using a union here leads to incorrect hashing results.
   */
  union
  {
    int value;
    MachineWord reserved;
  } _val;

  _val.reserved = 0;
  _val.value = val;

  // header_type header = INT_HEADER;
  HashNumber hnr = START(AS_INT.number());
  hnr = COMBINE(hnr, HN(_val.reserved));
  hnr = FINISH(hnr);

  cur = aterm::hashtable[hnr & table_mask];
  while (cur && (cur->m_function_symbol!=AS_INT || (reinterpret_cast<_ATermInt*>(cur)->value != _val.value)))
  {
    cur = cur->next;
  }

  if (!cur)
  {
    cur = AT_allocate(TERM_SIZE_INT);
    /* Delay masking until after AT_allocate */
    hnr &= table_mask;
    cur->m_function_symbol = AS_INT;
    reinterpret_cast<_ATermInt*>(cur)->reserved = _val.reserved;
    reinterpret_cast<_ATermInt*>(cur)->value = _val.value;

    cur->next = aterm::hashtable[hnr];
    aterm::hashtable[hnr] = cur;
  }

  assert((hnr & table_mask) == (hash_number(cur, TERM_SIZE_INT) & table_mask));
  m_term=cur;
  increase_reference_count<false>(m_term);
}

/*}}}  */

/*{{{  size_t AT_inAnyFreeList(ATerm t) */

/**
 * Check if a term is in any free list.
 */

/* size_t AT_inAnyFreeList(const _ATerm *t)
{
  for (size_t i=MIN_TERM_SIZE; i<terminfo.size(); i++)
  {
    _ATerm* cur = terminfo[i].at_freelist;

    while (cur)
    {
      if (cur == t)
      {
        return i;
      }
      cur = cur->next;
    }
  }
  return 0;
} */

/*}}}  */

} // namespace atermpp

