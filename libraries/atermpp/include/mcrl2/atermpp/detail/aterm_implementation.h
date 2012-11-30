#ifndef MEMORY_H
#define MEMORY_H

#include "mcrl2/utilities/exception.h"
#include "mcrl2/utilities/detail/memory_utility.h"
#include "mcrl2/atermpp/detail/atypes.h"
#include "mcrl2/atermpp/aterm.h"


namespace atermpp
{

namespace detail
{
  extern size_t aterm_table_mask;
  extern size_t aterm_table_size;
  extern const detail::_aterm* * aterm_hashtable;


  extern aterm static_undefined_aterm;  // detail/aterm_implementation.h
  extern aterm static_empty_aterm_list;
 
  void initialise_administration();
  void initialise_aterm_administration();

}
  
inline
const detail::_aterm *aterm::undefined_aterm()
{
  if (detail::static_undefined_aterm.m_term==NULL)
  {
    detail::initialise_administration();
  }
  return detail::static_undefined_aterm.m_term;
} 

inline
const detail::_aterm *aterm::empty_aterm_list()
{
  if (detail::static_empty_aterm_list.m_term==NULL)
  {
    detail::initialise_administration();
  }
  return detail::static_empty_aterm_list.m_term;
} 

namespace detail
{

inline
size_t COMBINE(const HashNumber hnr, const size_t w)
{
  return (hnr<<1) ^ hnr ^ w;
}

inline
size_t COMBINE(const HashNumber hnr, const aterm &w)
{
  return COMBINE(hnr,reinterpret_cast<size_t>(w.address()));
}

inline
void CHECK_TERM(const aterm &
#ifndef NDEBUG 
t
#endif
)
{
  assert(t.defined());
  assert(t.address()->reference_count()>0);
  // assert(aterm_cast<aterm_appl>(t).function().name().size()!=0);
}

inline HashNumber hash_number(const detail::_aterm *t)
{
  const function_symbol &f=t->function();
  HashNumber hnr = f.number();

  const size_t* begin=reinterpret_cast<const size_t*>(t)+TERM_SIZE;
  const size_t* end=begin+f.arity();
  for (const size_t* i=begin; i!=end; ++i)
  {
    hnr = COMBINE(hnr, *i);
  } 

  return hnr;
}

static const size_t BLOCK_SIZE = 1<<13;

typedef struct Block
{
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
  const _aterm*       at_freelist;

  TermInfo():at_block(NULL),top_at_blocks(NULL),at_freelist(NULL)
  {}

} TermInfo;

void resize_aterm_hashtable();
void allocate_block(const size_t size);

extern size_t terminfo_size;
extern size_t total_nodes;
extern TermInfo *terminfo;

inline const _aterm* allocate_term(const size_t size)
{
  if (size >= terminfo_size) 
  {
    // Double the size of terminfo
    size_t old_term_info_size=terminfo_size;
    terminfo_size <<=1; // Multiply by 2.
    terminfo=reinterpret_cast<TermInfo*>(realloc(terminfo,terminfo_size*sizeof(TermInfo)));
    if (terminfo==NULL)
    {
      throw std::runtime_error("Out of memory. Failed to allocate an extension of terminfo.");
    }
    for(size_t i=old_term_info_size; i<terminfo_size; ++i)
    {
      new (&terminfo[i]) TermInfo();
    }
  }

  if (total_nodes>=(aterm_table_size>>1))
  {
    // The hashtable is not big enough to hold nr_of_nodes_for_the_next_garbage_collect. So, resizing
    // is wise (although not necessary, due to the structure of the hastable, which allows is to contain
    // an arbitrary number of element, at some performance penalty.
    resize_aterm_hashtable();
  }

  total_nodes++;
  TermInfo &ti = terminfo[size];
  if (ti.at_block && ti.top_at_blocks < ti.at_block->end)
  {
    /* the first block is not full: allocate a cell */
    _aterm *at = (_aterm *)ti.top_at_blocks;
    ti.top_at_blocks += size;
    at->reset_reference_count();
    return at;
  }
  else if (ti.at_freelist)
  {
    /* the freelist is not empty: allocate a cell */
    const _aterm *at = ti.at_freelist;
    ti.at_freelist = ti.at_freelist->next();
    assert(ti.at_block != NULL);
    assert(ti.top_at_blocks == ti.at_block->end);
    assert(at->reference_count()==0);
    return at;
  }
  else
  {
    /* there is no more memory of the current size allocate a block */
    allocate_block(size);
    assert(ti.at_block != NULL);
    _aterm *at = (_aterm *)ti.top_at_blocks;
    ti.top_at_blocks += size;
    at->reset_reference_count();
    return at;
  }
} 

inline void remove_from_hashtable(const _aterm *t)
{
  /* Remove the node from the aterm_hashtable */
  const _aterm *prev=NULL;
  const HashNumber hnr = hash_number(t) & aterm_table_mask;
  const _aterm *cur = aterm_hashtable[hnr];

  do
  {
    assert(cur!=NULL); /* This only occurs if the hashtable is in error. */
    if (cur == t)
    {
      if (prev)
      {
        prev->set_next(cur->next());
      }
      else
      {
        aterm_hashtable[hnr] = cur->next();
      }
      /* Put the node in the appropriate free list */
      total_nodes--;
      return;
    }
  }
  while (((prev=cur), (cur=cur->next())));
  assert(0);
}


inline const _aterm* aterm0(const function_symbol &sym)
{
  assert(sym.arity()==0);

  HashNumber hnr = sym.number();

  const detail::_aterm *cur = detail::aterm_hashtable[hnr & detail::aterm_table_mask];

  while (cur)
  {
    if (cur->function()==sym)
    {
      return cur;
    }
    cur = cur->next();
  }

  cur = detail::allocate_term(detail::TERM_SIZE);
  /* Delay masking until after allocate */
  hnr &= detail::aterm_table_mask;
  new (&const_cast<detail::_aterm*>(cur)->function()) function_symbol(sym);

  cur->set_next(detail::aterm_hashtable[hnr]);
  detail::aterm_hashtable[hnr] = cur;

  return cur;
}

} //namespace detail

} // namespace atermpp

#endif
