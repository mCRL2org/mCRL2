#ifndef MEMORY_H
#define MEMORY_H

#include "mcrl2/utilities/exception.h"
#include "mcrl2/atermpp/detail/atypes.h"
#include "mcrl2/atermpp/aterm.h"


namespace atermpp
{

namespace detail
{

static const size_t BLOCK_SIZE = 1<<14; 

typedef struct Block
{
  struct Block* next_by_size;
  size_t* end;
  size_t data[];
} Block;

typedef struct TermInfo
{
  Block*       at_block;
  const _aterm*       at_freelist;

  TermInfo():at_block(NULL),at_freelist(NULL)
  {}

} TermInfo;

extern size_t aterm_table_mask;
extern size_t aterm_table_size;
extern const detail::_aterm* * aterm_hashtable;

extern aterm static_undefined_aterm;  // detail/aterm_implementation.h
extern aterm static_empty_aterm_list;

extern size_t terminfo_size;
extern size_t total_nodes;
extern TermInfo *terminfo;

extern size_t garbage_collect_count_down;

void initialise_administration();
void initialise_aterm_administration();

void resize_aterm_hashtable();
void allocate_block(const size_t size);
void collect_terms_with_reference_count_0();

inline size_t SHIFT(const size_t w)
{
#ifdef FUNCTION_SYMBOL_AS_POINTER
  return w>>4;
#else 
  return w;
#endif
}

inline
size_t COMBINE(const HashNumber hnr, const size_t w)
{
  return (hnr>>1) ^ hnr ^ w;
}

inline
size_t COMBINE(const HashNumber hnr, const aterm &w)
{
  return COMBINE(hnr,reinterpret_cast<size_t>(address(w)));
}

inline
void CHECK_TERM(const aterm &
#ifndef NDEBUG 
t
#endif
)
{
  assert(t.defined());
  assert(!address(t)->reference_count_indicates_is_in_freelist());
  assert(!address(t)->reference_count_is_zero());
}

inline HashNumber hash_number(const detail::_aterm *t)
{
  const function_symbol &f=t->function();
  HashNumber hnr = SHIFT(address(f));

  const size_t* begin=reinterpret_cast<const size_t*>(t)+TERM_SIZE;
  const size_t* end=begin+f.arity();
  for (const size_t* i=begin; i!=end; ++i)
  {
    hnr = COMBINE(hnr, *i);
  } 

  return hnr;
}

inline const _aterm* allocate_term(const size_t size)
{
  assert(size>=TERM_SIZE);
  if (size >= terminfo_size) 
  {
    // Resize the size of terminfo to the minimum of twice its old size and size+1;
    const size_t old_term_info_size=terminfo_size;
    terminfo_size <<=1; // Multiply by 2.
    if (size>=terminfo_size)
    {
      terminfo_size=size+1;
    }
    terminfo=reinterpret_cast<TermInfo*>(realloc(terminfo,terminfo_size*sizeof(TermInfo)));
    if (terminfo==NULL)
    {
      throw std::runtime_error("Out of memory. Failed to allocate an extension of terminfo.");
    }
    for(size_t i=old_term_info_size; i<terminfo_size; ++i)
    {
      new (&terminfo[i]) TermInfo();
    }
    assert(size<terminfo_size);
  }

  if (total_nodes>=(aterm_table_size))
  {
    // The hashtable is not big enough to hold nr_of_nodes_for_the_next_garbage_collect. So, resizing
    // is wise (although not necessary, due to the structure of the hastable, which allows is to contain
    // an arbitrary number of element, at some performance penalty.
    resize_aterm_hashtable();
  }

  total_nodes++;
  TermInfo &ti = terminfo[size];
  if (garbage_collect_count_down>0)
  { 
    garbage_collect_count_down--;
  }
  
  if (garbage_collect_count_down==0 && ti.at_freelist==NULL) // It is time to collect free terms, and there are
                                                             // no free terms left.
  {
    collect_terms_with_reference_count_0();
  }
  if (ti.at_freelist==NULL)
  {
    /* there is no more memory of the current size allocate a block */
    allocate_block(size);
    assert(ti.at_block != NULL);
  }
  
  const _aterm *at = ti.at_freelist;
  ti.at_freelist = ti.at_freelist->next();
  assert(at->reference_count_indicates_is_in_freelist());
  at->reset_reference_count();
  assert(ti.at_block != NULL);
  return at;
} 

inline void remove_from_hashtable(const _aterm *t)
{
  /* Remove the node from the aterm_hashtable */
  assert(t->reference_count_indicates_is_in_freelist());
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

  HashNumber hnr = SHIFT(address(sym));

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

inline const _aterm* address(const aterm &t)
{
  return t.m_term;
} 

} //namespace detail

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

} // namespace atermpp

#endif
