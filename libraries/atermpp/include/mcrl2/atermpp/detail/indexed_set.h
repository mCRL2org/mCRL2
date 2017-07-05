// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
// 
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// 
/// \file atermpp/detail/indexed_set.cpp
/// \brief This file contains some constants and functions shared
///        between indexed_sets and tables.

#ifndef MCRL2_ATERMPP_DETAIL_INDEXED_SET_H
#define MCRL2_ATERMPP_DETAIL_INDEXED_SET_H

#include <cstddef>
#include <cassert>
#include "mcrl2/atermpp/indexed_set.h"

namespace atermpp
{
namespace detail
{

static const std::size_t STEP = 1; /* The position on which the next hash entry //searched */


/* in the hashtable we use the following constants to
   indicate designated positions */
static const std::size_t EMPTY(-1);
static const std::size_t DELETED(-2);

inline std::size_t approximatepowerof2(std::size_t n)
{
  std::size_t mask = n;

  while (mask >>= 1)
  {
    n |= mask;
  }

  if (n<127)
  {
    n=127;
  }
  return n;
}

static inline std::size_t calculateNewSize(std::size_t sizeMinus1, std::size_t nr_entries, std::size_t max_load)
{
  if ((nr_entries*200)/max_load < sizeMinus1)
  {
    return sizeMinus1;
  }
  assert(2*sizeMinus1+1>sizeMinus1);
  return 2*sizeMinus1+1;
}


} // namespace detail

template <class ELEMENT>
inline
std::size_t indexed_set<ELEMENT>::put_in_hashtable(const ELEMENT& key, std::size_t n)
{
  /* Find a place to insert key,
     and find whether key already exists */

  std::size_t deleted_position=detail::EMPTY; // This variable recalls a proper deleted position to insert n. EMPTY means not yet found.
  std::size_t start = std::hash<aterm>()(key) & sizeMinus1;
  std::size_t c = start;

  while (true)
  {
    assert(c!=detail::EMPTY);
    std::size_t v = hashtable[c];
    assert(v==detail::EMPTY || v == detail::DELETED || v<m_keys.size());
    if (v == detail::EMPTY)
    {
      /* Found an empty spot, insert a new index belonging to key, 
         preferably at a deleted position, if that has been encountered. */
      if (deleted_position==detail::EMPTY)
      { 
        --nr_of_insertions_until_next_rehash;
        assert(nr_of_insertions_until_next_rehash!=npos);
        hashtable[c] = n;
      }
      else
      { 
        hashtable[deleted_position] = n;
      }
      return n;
    }

    if (v == detail::DELETED)
    {
      /* Recall this position to be used, in case the element is not found. */
      if (deleted_position==detail::EMPTY)
      { 
        deleted_position=c;
      }
    }
    else if (m_keys[v]==key)
    {
      /* key is already in the set, return position of key */
      return v;
    }
    c = (c + detail::STEP) & sizeMinus1;
    assert(c!=start); // In this case the hashtable is full, which should never happen.
  }
  return c;
}


template <class ELEMENT>
inline void indexed_set<ELEMENT>::resize_hashtable()
{
  /* First determine the largest index in use */
  std::size_t largest_used_index=0;
  for (std::size_t i=0; i<m_keys.size(); i++)
  {
    ELEMENT t = m_keys[i];
    if (t.defined() && i>largest_used_index)
    {
      largest_used_index=i;
    }
  }
  m_keys.resize(largest_used_index+1);

  std::size_t newsizeMinus1 = detail::calculateNewSize(sizeMinus1,largest_used_index, max_load);

  hashtable.clear();
  hashtable.resize(newsizeMinus1+1,detail::EMPTY); 

  sizeMinus1=newsizeMinus1;
  nr_of_insertions_until_next_rehash = ((sizeMinus1/100)*max_load);




  free_positions=std::stack < std::size_t >();
  /* rebuild the hashtable again, and put free indices in the free_position stack.
     Count down, such that the lowest indices are highest in the stack, to be 
     re-used first. */
  for (std::size_t i=m_keys.size(); i>0 ; )
  {
     --i;
    ELEMENT t = m_keys[i];
    if (t.defined())
    {
      put_in_hashtable(t, i);
    }
    else
    {
      free_positions.push(i);
    }
  }
}

template <class ELEMENT>
inline indexed_set<ELEMENT>::indexed_set(std::size_t initial_size /* = 100 */, unsigned int max_load_pct /* = 75 */)
      : sizeMinus1(detail::approximatepowerof2(initial_size)),
        max_load(max_load_pct),
        nr_of_insertions_until_next_rehash(((sizeMinus1/100)*max_load)),
        hashtable(std::vector<std::size_t>(1+sizeMinus1,detail::EMPTY))
{
}


template <class ELEMENT>
inline ssize_t indexed_set<ELEMENT>::index(const ELEMENT& elem) const
{
  std::size_t start = std::hash<aterm>()(elem) & sizeMinus1;
  std::size_t c = start;
  do
  {
    std::size_t v=hashtable[c];
    if (v == detail::EMPTY)
    {
      return npos; /* Not found. */
    }
    assert(v == detail::DELETED || v<m_keys.size());
    if (v != detail::DELETED && elem==m_keys[v])
    {
      return v;
    }

    c = (c+detail::STEP) & sizeMinus1;
    assert(c!=start);   // The hashtable is full. This should never happen.
  }
  while (true);

  return npos; /* Not found. */
}

template <class ELEMENT>
bool indexed_set<ELEMENT>::erase(const ELEMENT& key)
{
  std::size_t start = std::hash<aterm>()(key) & sizeMinus1;
  std::size_t c = start;
  std::size_t v;
  while (true)
  {
    v = hashtable[c];
    if (v == detail::EMPTY)
    {
      return false;
    }
    assert(v == detail::DELETED || v<m_keys.size());
    if (v != detail::DELETED && key==m_keys[v])
    {
      break;
    }

    c = (c + detail::STEP) & sizeMinus1;
    if (c == start)
    {
      assert(0);  // The hashtable is full. This should never happen.
      return false;
    }
  }

  hashtable[c] = detail::DELETED;

  assert(m_keys.size()>v);
  assert(!ELEMENT().defined());
  assert(v<m_keys.size());
  m_keys[v]=ELEMENT();
  free_positions.push(v);

  return true;
}


template <class ELEMENT>
inline const ELEMENT& indexed_set<ELEMENT>::get(std::size_t index) const
{
  assert(m_keys.size()>index);
  return m_keys[index];
}

template <class ELEMENT>
inline bool indexed_set<ELEMENT>::defined(std::size_t index) const
{
  return index<m_keys.size() && m_keys[index].defined();
}

template <class ELEMENT>
inline void indexed_set<ELEMENT>::clear()
{
  hashtable.assign(sizeMinus1+1,detail::EMPTY);
  m_keys.clear();
  free_positions=std::stack<std::size_t>();
}


template <class ELEMENT>
inline std::pair<std::size_t, bool> indexed_set<ELEMENT>::put(const ELEMENT& key)
{
  const std::size_t m=(free_positions.empty()? m_keys.size() : free_positions.top());
  const std::size_t n = put_in_hashtable(key,m);
  if (n != m) // Key already exists.
  {
    return std::make_pair(n,false);
  }
  
  if (!free_positions.empty())
  {
    free_positions.pop();
  }
  else if (n>=m_keys.size())
  {
    m_keys.resize(n+1);
  }
  assert(m_keys.size()>n);
  m_keys[n]=key;
  if (nr_of_insertions_until_next_rehash==0)
  {
    resize_hashtable(); 
  }

  return std::make_pair(n, true);
}


} // namespace atermpp 

#endif // MCRL2_ATERMPP_DETAIL_INDEXED_SET_H
