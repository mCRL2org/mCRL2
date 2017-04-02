// Author(s): Jan Friso Groote. Based on the aterm library by Paul Klint and others.
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/detail/aterm_implementation.h
/// \brief This file contains the code to allocate aterms in tables
///        with sufficient space for its arguments. 

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

struct Block
{
  struct Block* next_by_size;
  size_t* end;
  size_t data[1000]; // This is a block of arbitrary size. The indication data[]
                     // is not accepted by the visual C++ compiler. If a lower
                     // number than 1000 would be selected, the compiler may
                     // warn that there is an index out of bound error.

  private:
    // The copy constructor is made private to indicate that
    // a block is not intended to be copied.
    Block(const Block& /*other*/)
    {
      assert(0);
    }

    // The assignment operator is made private to indicate that
    // assigning a Block is also not allowed.
    Block& operator=(const Block& /*other*/)
    {
      assert(0);
      return *this;
    }
};

struct TermInfo
{
  Block*       at_block;
  _aterm*       at_freelist;

  TermInfo():at_block(nullptr),at_freelist(nullptr)
  {}

};

extern size_t aterm_table_mask;
extern size_t aterm_table_size;
extern detail::_aterm* * aterm_hashtable;

extern size_t terminfo_size;
extern size_t total_nodes_in_hashtable;
extern TermInfo *terminfo;

extern size_t garbage_collect_count_down;

void resize_aterm_hashtable();
void allocate_block(const size_t size);
void collect_terms_with_reference_count_0();

void call_creation_hook(_aterm*);

// Auxiliary function to calculate a hash for _aterm's.
inline
size_t COMBINE(const size_t hnr, const size_t w)
{
  return (w>>3) + (hnr>>1) + (hnr<<1);
}

inline
size_t COMBINE(const size_t hnr, const aterm& w)
{
  return COMBINE(hnr,reinterpret_cast<size_t>(address(w)));
}

inline
void CHECK_TERM(const aterm&
#ifndef NDEBUG
t
#endif
)
{
  assert(t.defined());
  assert(!address(t)->reference_count_indicates_is_in_freelist());
  assert(!address(t)->reference_count_is_zero());
}

inline size_t hash_number(detail::_aterm *t)
{
  const function_symbol& f=t->function();
  const std::hash<function_symbol> function_symbol_hasher;
  size_t hnr = function_symbol_hasher(f);

  const size_t* begin=reinterpret_cast<const size_t*>(t)+TERM_SIZE;
  const size_t* end=begin+f.arity();
  for (const size_t* i=begin; i!=end; ++i)
  {
    hnr = COMBINE(hnr, *i);
  }

  return hnr;
}

inline _aterm* allocate_term(const size_t size)
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
    if (terminfo==nullptr)
    {
      throw std::runtime_error("Out of memory. Failed to allocate an extension of terminfo.");
    }
    for(size_t i=old_term_info_size; i<terminfo_size; ++i)
    {
      new (&terminfo[i]) TermInfo();
    }
    assert(size<terminfo_size);
  }

  if (total_nodes_in_hashtable>=aterm_table_size)
  {
    // The hashtable is not big enough to hold nr_of_nodes_for_the_next_garbage_collect. So, resizing
    // is wise (although not necessary, due to the structure of the hastable, which allows is to contain
    // an arbitrary number of element, at some performance penalty.
    resize_aterm_hashtable();
  }

  TermInfo& ti = terminfo[size];
  if (garbage_collect_count_down>0)
  {
    garbage_collect_count_down--;
  }

  if (garbage_collect_count_down==0 && ti.at_freelist==nullptr) // It is time to collect free terms, and there are
                                                             // no free terms left.
  {
    collect_terms_with_reference_count_0();
  }
  if (ti.at_freelist==nullptr)
  {
    /* there is no more memory of the current size allocate a block */
    allocate_block(size);
    assert(ti.at_block != nullptr);
  }

  _aterm *at = ti.at_freelist;
  ti.at_freelist = ti.at_freelist->next();
  assert(at->reference_count_indicates_is_in_freelist());
  at->reset_reference_count();
  assert(ti.at_block != nullptr);
  return at;
}

inline void remove_from_hashtable(_aterm *t)
{
  /* Remove the node from the aterm_hashtable */
  _aterm *prev=nullptr;
  const size_t hnr = hash_number(t) & aterm_table_mask;
  _aterm *cur = aterm_hashtable[hnr];

  do
  {
    assert(cur!=nullptr); /* This only occurs if the hashtable is in error. */
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
      total_nodes_in_hashtable--;
      return;
    }
  }
  while (((prev=cur), (cur=cur->next())));
  assert(0);
}

inline _aterm* address(const aterm& t)
{
  return t.m_term;
}

} //namespace detail

} // namespace atermpp

#endif
