// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file atermpp/detail/indexed_set.cpp
/// \brief This file contains some constants and functions shared
///        between indexed_sets and tables.

#ifndef MCRL2_UTILITIES_DETAIL_INDEXED_SET_H
#define MCRL2_UTILITIES_DETAIL_INDEXED_SET_H
#pragma once

#include <cstddef>
#include <limits>
#include <cassert>
#include <deque>

#include "mcrl2/utilities/indexed_set.h"    // necessary for header test. 

namespace mcrl2
{
namespace utilities
{
namespace detail
{

static const std::size_t STEP = 1; /* The position on which the next hash entry is searched */

/* in the hashtable we use the following constant to indicate free positions */
static const std::size_t EMPTY(std::numeric_limits<std::size_t>::max());
static const float max_load = 0.7f;
static const size_t minimal_hashtable_size = 8;  // With a max_load of 0.75 the minimal size of the hashtable must be 8.
static const std::size_t PRIME_NUMBER = 999953;


} // namespace detail

template <class Key, typename Hash, typename Equals, typename Allocator, bool ThreadSafe>
inline
std::size_t indexed_set<Key,Hash,Equals,Allocator,ThreadSafe>::put_in_hashtable(const Key& key, std::size_t n)
{
  /* Find a place to insert key,
     and find whether key already exists */

  std::size_t start = (m_hasher(key)*detail::PRIME_NUMBER) % m_hashtable.size();
  std::size_t c = start;
  while (true)
  {
    std::size_t v = m_hashtable[c];
    assert(v==detail::EMPTY || v<m_keys.size());
    if (v == detail::EMPTY)
    {
      /* Found an empty spot, insert a new index belonging to key,
         preferably at a deleted position, if that has been encountered. */
      m_hashtable[c] = n;
      return n;
    }

    // if (Equals(m_keys[v],key))
    if (m_keys[v]==key)
    {
      /* key is already in the set, return position of key */
      return v;
    }
    c = (c + detail::STEP) % m_hashtable.size();
    assert(c!=start); // In this case the hashtable is full, which should never happen.
  }
  return c;
}


template <class Key, typename Hash, typename Equals, typename Allocator, bool ThreadSafe>
inline void indexed_set<Key, Hash, Equals, Allocator, ThreadSafe>::resize_hashtable()
{
  m_hashtable=std::vector<std::size_t>(m_hashtable.size()*2,detail::EMPTY);

  size_t index=0;
  for (const Key& k: m_keys)
  {
    put_in_hashtable(k, index);
    ++index;
  }
}

template <class Key, typename Hash, typename Equals, typename Allocator, bool ThreadSafe>
inline indexed_set<Key,Hash,Equals,Allocator,ThreadSafe>::indexed_set()
      : indexed_set(128)
{
} 

template <class Key, typename Hash, typename Equals, typename Allocator, bool ThreadSafe>
inline indexed_set<Key,Hash,Equals,Allocator,ThreadSafe>::indexed_set(std::size_t initial_size,
  const hasher& hasher,
  const key_equal& equals)
      : m_hashtable(std::max(initial_size,detail::minimal_hashtable_size),detail::EMPTY),
        m_hasher(hasher),
        m_equals(equals)
{}

template <class Key, typename Hash, typename Equals, typename Allocator, bool ThreadSafe>
inline typename indexed_set<Key,Hash,Equals,Allocator,ThreadSafe>::size_type indexed_set<Key,Hash,Equals,Allocator,ThreadSafe>::index(const Key& key) const
{
  std::size_t start = (m_hasher(key)*detail::PRIME_NUMBER) % m_hashtable.size();
  std::size_t c = start;
  do
  {
    std::size_t v=m_hashtable[c];
    if (v == detail::EMPTY)
    {
      return npos; /* Not found. */
    }
    assert(v<m_keys.size());
    if (m_equals(key,m_keys[v]))
    {
      return v;
    }

    c = (c+detail::STEP) % m_hashtable.size();
    assert(c!=start);   // The hashtable is full. This should never happen.
  }
  while (true);

  return npos; /* Not found. */
}

template <class Key, typename Hash, typename Equals, typename Allocator, bool ThreadSafe>
inline typename indexed_set<Key,Hash,Equals,Allocator,ThreadSafe>::const_iterator indexed_set<Key,Hash,Equals,Allocator,ThreadSafe>::find(const Key& key) const
{
  const std::size_t i=index(key);
  if (i<m_keys.size())
  {
    return m_keys[i];
  }
  return end();
}


template <class Key, typename Hash, typename Equals, typename Allocator, bool ThreadSafe>
inline const Key& indexed_set<Key,Hash,Equals,Allocator,ThreadSafe>::at(std::size_t index) const
{
  if (index>=m_keys.size())
  {
    throw std::out_of_range("indexed_set: index too large: " + std::to_string(index) + " > " + std::to_string(m_keys.size()) + ".");
  }
  return m_keys[index];
}

template <class Key, typename Hash, typename Equals, typename Allocator, bool ThreadSafe>
inline const Key& indexed_set<Key,Hash,Equals,Allocator,ThreadSafe>::operator[](std::size_t index) const
{
  assert(index<m_keys.size());
  return m_keys[index];
}

template <class Key, typename Hash, typename Equals, typename Allocator, bool ThreadSafe>
inline void indexed_set<Key,Hash,Equals,Allocator,ThreadSafe>::clear()
{
  m_hashtable.assign(m_hashtable.size(),detail::EMPTY);
  m_keys.clear();
}


template <class Key, typename Hash, typename Equals, typename Allocator, bool ThreadSafe>
inline std::pair<std::size_t, bool> indexed_set<Key,Hash,Equals,Allocator,ThreadSafe>::insert(const Key& key)
{
  const std::size_t m= m_keys.size();
  const std::size_t n = put_in_hashtable(key,m);
  if (n != m) // Key already exists.
  {
    return std::make_pair(n,false);
  }

  m_keys.push_back(key);
  if (detail::max_load*m_hashtable.size()<m_keys.size())
  {
    resize_hashtable();
  }

  return std::make_pair(n, true);
}


} // namespace utilities

} // namespace mcrl2

#endif // MCRL2_UTILITIES_DETAIL_INDEXED_SET_H
