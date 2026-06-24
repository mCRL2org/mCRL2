// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_UTILITIES_DETAIL_HASHTABLE_H
#define MCRL2_UTILITIES_DETAIL_HASHTABLE_H
#pragma once

#include "mcrl2/utilities/power_of_two.h" 
#include "mcrl2/utilities/hashtable.h"    // necessary for header test.
#include "mcrl2/utilities/indexed_set.h"    // necessary for header test.
#include "mcrl2/utilities/exception.h"

namespace mcrl2::utilities
{

template <class Key, typename Hash, typename Equals, typename Allocator>
inline void hashtable<Key, Hash, Equals, Allocator>::rehash(std::size_t size)
{
  // Copy the old hashtable.
  std::vector<Key> old = std::move(m_hashtable);

  m_hashtable = std::vector<Key>(size, nullptr);
  m_buckets_mask = m_hashtable.size() - 1;

  for (const Key& key : old)
  {
    const std::size_t key_index = get_index(key);
    auto it = begin() + key_index;
    // Find the first empty position.
    while (*it != nullptr)
    {
      ++it;
      if (it == end())
      {
        it = begin();
      }

      assert(it != begin() + key_index);
    }

    // Found an empty spot, insert a new index belonging to key,
    *it = key;
  }
}

template <class Key, typename Hash, typename Equals, typename Allocator>
inline hashtable<Key,Hash,Equals,Allocator>::hashtable()
  : hashtable(128)
{
} 

template <class Key, typename Hash, typename Equals, typename Allocator>
inline hashtable<Key,Hash,Equals,Allocator>::hashtable(std::size_t initial_size,
  const hasher& hasher,
  const key_equal& equals)
      : m_hashtable(std::max(initial_size, detail::minimal_hashtable_size), nullptr),
        m_hasher(hasher),
        m_equals(equals)
{
  assert(utilities::is_power_of_two(initial_size));
  m_buckets_mask = m_hashtable.size() - 1;
}

template <class Key, typename Hash, typename Equals, typename Allocator>
inline void hashtable<Key,Hash,Equals,Allocator>::clear()
{
  // Reset every slot to the empty value (nullptr) and the element count to zero,
  // while keeping the current capacity (and therefore m_buckets_mask) valid.
  // Note that m_hashtable.clear() must not be used here: it would shrink the
  // vector to size zero, leaving m_buckets_mask and m_number_of_elements stale
  // and every subsequent get_index() out of bounds.
  m_hashtable.assign(m_hashtable.size(), nullptr);
  m_number_of_elements = 0;
}

template <class Key, typename Hash, typename Equals, typename Allocator>
bool hashtable<Key,Hash,Equals,Allocator>::must_resize()
{
  return (2 * m_number_of_elements >= m_hashtable.size());
}

template <class Key, typename Hash, typename Equals, typename Allocator>
void hashtable<Key,Hash,Equals,Allocator>::resize()
{
  rehash(2 * m_hashtable.size());
}

template <class Key, typename Hash, typename Equals, typename Allocator>
inline std::pair<typename hashtable<Key,Hash,Equals,Allocator>::iterator, bool> hashtable<Key,Hash,Equals,Allocator>::insert(const Key& key)
{
  // Resize hashtable must be done explicitly.
  assert(!must_resize());

  const std::size_t key_index = get_index(key);
  auto it = begin() + key_index;

  // Find either an existing key or the first empty position.
  while (true)
  {
    if (*it == nullptr)
    {
      *it = key;
      ++m_number_of_elements;
      return std::make_pair(it, true);
    }

    if (m_equals(*it, key))
    {
      return std::make_pair(it, false);
    }

    ++it;
    if (it == end())
    {
      it = begin();
    }

    if (it == begin() + key_index)
    {
      assert(false && "hashtable::insert exhausted table without finding a slot");
      throw mcrl2::runtime_error("hashtable::insert exhausted table without finding a slot");
    }
  }
}


template <class Key, typename Hash, typename Equals, typename Allocator>
inline typename hashtable<Key,Hash,Equals,Allocator>::iterator hashtable<Key,Hash,Equals,Allocator>::erase(const Key& key)
{
  const std::size_t key_index = get_index(key);
  auto it = begin() + key_index;

  // Find the key.
  while (!m_equals(*it, key))
  {
    ++it;
    if (it == end())
    {
      it = begin();
    }

    if (it == begin() + key_index)
    {
      assert(false && "hashtable::erase called for a key that is not present");
      throw mcrl2::runtime_error("hashtable::erase called for a key that is not present");
    }
  }

  *it = nullptr;
  --m_number_of_elements;
  return it;
}


template <class Key, typename Hash, typename Equals, typename Allocator>
inline typename hashtable<Key,Hash,Equals,Allocator>::iterator hashtable<Key,Hash,Equals,Allocator>::find(const Key& key)
{
  for (auto it = begin(); it != end(); ++it)
  {
    if (*it != nullptr && m_equals(*it, key))
    {
      return it;
    }
  }

  return end();
}

// PRIVATE FUNCTIONS

template <class Key, typename Hash, typename Equals, typename Allocator>
inline std::size_t hashtable<Key,Hash,Equals,Allocator>::get_index(const Key& key)
{
  return m_hasher(key) & m_buckets_mask;
}


} // namespace mcrl2::utilities



#endif // MCRL2_UTILITIES_DETAIL_HASHTABLE_H
