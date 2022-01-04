// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file utilities/detail/indexed_set.cpp
/// \brief This file contains some constants and functions shared
///        between indexed_sets and tables.

#ifndef MCRL2_UTILITIES_DETAIL_INDEXED_SET_H
#define MCRL2_UTILITIES_DETAIL_INDEXED_SET_H
#pragma once

#include "mcrl2/utilities/indexed_set.h"    // necessary for header test. 

namespace mcrl2
{
namespace utilities
{
namespace detail
{

static const std::size_t STEP = 1; ///< The position on which the next hash entry is searched.

/// in the hashtable we use the following constant to indicate free positions.
static constexpr std::size_t EMPTY(std::numeric_limits<std::size_t>::max());

static constexpr float max_load_factor = 0.75f; ///< The load factor before the hash table is resized.

static constexpr std::size_t PRIME_NUMBER = 999953;

static constexpr std::size_t minimal_hashtable_size = 1024;

static constexpr std::size_t RESERVATION_SIZE=1024;

static_assert(RESERVATION_SIZE <= minimal_hashtable_size);
static_assert(minimal_hashtable_size>=8);       ///< With a max_load of 0.75 the minimal size of the hashtable must be 8.

} // namespace detail

template <class Key, typename Hash, typename Equals, typename Allocator, bool ThreadSafe, typename KeyTable>
inline void indexed_set<Key,Hash,Equals,Allocator,ThreadSafe, KeyTable>::reserve_indices_for_this_thread(std::size_t thread_index)
{
// std::cerr << "RESERVE NEW INDICES " << thread_index << "     " << m_thread_control.size() << "   " << detail::RESERVATION_SIZE << "\n";

  lock_exclusive(thread_index);
  assert(thread_index<m_thread_control.size());
  detail::thread_control& c=m_thread_control[thread_index];
  assert(c.reserved_numbers_begin==c.reserved_numbers_end);
  c.reserved_numbers_begin=m_keys.size();
  c.reserved_numbers_end=c.reserved_numbers_begin+detail::RESERVATION_SIZE;
  m_keys.resize(c.reserved_numbers_end);

  while ((detail::max_load_factor * m_hashtable.size()) < m_keys.size())
  {
     resize_hashtable();
  }
  unlock_exclusive(thread_index);

}


template <class Key, typename Hash, typename Equals, typename Allocator, bool ThreadSafe, typename KeyTable>
inline void indexed_set<Key,Hash,Equals,Allocator,ThreadSafe, KeyTable>::lock_shared(std::size_t thread_index) const
{
  assert(!m_thread_control[thread_index].busy_flag);
  m_thread_control[thread_index].busy_flag.store(true);
  
  // Wait for the forbidden flag to become false.
  while (m_thread_control[thread_index].forbidden_flag.load())
  { 
    m_thread_control[thread_index].busy_flag = false;
    std::unique_lock lock(*m_mutex);
    m_thread_control[thread_index].busy_flag = true;
  }
}

template <class Key, typename Hash, typename Equals, typename Allocator, bool ThreadSafe, typename KeyTable>
inline void indexed_set<Key,Hash,Equals,Allocator,ThreadSafe, KeyTable>::unlock_shared(std::size_t thread_index) const
{
  assert(m_thread_control[thread_index].busy_flag);
  m_thread_control[thread_index].busy_flag.store(false, std::memory_order_release);
}

template <class Key, typename Hash, typename Equals, typename Allocator, bool ThreadSafe, typename KeyTable>
inline void indexed_set<Key,Hash,Equals,Allocator,ThreadSafe, KeyTable>::lock_exclusive(std::size_t thread_index) const
{
   // Only one thread can halt everything.
  m_mutex->lock();

  // Indicate that threads must wait.
  for (std::size_t i=0; i<m_thread_control.size(); ++i)
  {
    if (i != thread_index)
    {
      m_thread_control[i].forbidden_flag=true;
    }
  }

  // Wait for all pools to indicate that they are not busy.
  for (std::size_t i=0; i<m_thread_control.size(); ++i)
  {
    if (i != thread_index)
    {
      // wait for busy 
      while (m_thread_control[i].busy_flag.load());
    }
  }
}

template <class Key, typename Hash, typename Equals, typename Allocator, bool ThreadSafe, typename KeyTable>
inline void indexed_set<Key,Hash,Equals,Allocator,ThreadSafe, KeyTable>::unlock_exclusive(std::size_t thread_index) const
{
  for (std::size_t i=0; i<m_thread_control.size(); ++i)
  {
    if (i != thread_index)
    {
      m_thread_control[i].forbidden_flag=false;
    }
  }

  m_mutex->unlock();
}


template <class Key, typename Hash, typename Equals, typename Allocator, bool ThreadSafe, typename KeyTable>
inline std::size_t indexed_set<Key,Hash,Equals,Allocator,ThreadSafe, KeyTable>::put_in_hashtable(const key_type& key, std::size_t value)
{
  // Find a place to insert key and find whether key already exists.
  assert(m_hashtable.size()>0);
  std::size_t start = ((m_hasher(key) * detail::PRIME_NUMBER) >> 2) % m_hashtable.size();
  std::size_t position = start;

  while (true)
  {
    std::size_t index = m_hashtable[position];
    assert(index == detail::EMPTY || index < m_keys.size());

    if (index == detail::EMPTY)
    {
      // Found an empty spot, insert a new index belonging to key,
      std::size_t pos=detail::EMPTY;
      if (reinterpret_cast<std::atomic<std::size_t>*>(&m_hashtable[position])->compare_exchange_strong(pos,value))
      {
        return value;
      }
      index=pos;             // Insertion failed, but another process put an alternative value "pos"
                             // at this position. 
    }

    assert(index!=detail::EMPTY);
    if (m_equals(m_keys[index], key))
    {
      // key is already in the set, return position of key.
      return index;
    }
    assert(m_hashtable.size()>0);
    position = (position + detail::STEP) % m_hashtable.size();
    assert(position != start); // In this case the hashtable is full, which should never happen.
  }

  return position;
}

template <class Key, typename Hash, typename Equals, typename Allocator, bool ThreadSafe, typename KeyTable>
inline bool indexed_set<Key,Hash,Equals,Allocator,ThreadSafe, KeyTable>::check_index_validity(std::size_t index)
{
  for(detail::thread_control& c: m_thread_control)
  {
    if (c.reserved_numbers_begin <= index && index < c.reserved_numbers_end)
    {
      return false;
    }
  }
  return true;
}


template <class Key, typename Hash, typename Equals, typename Allocator, bool ThreadSafe, typename KeyTable>
inline void indexed_set<Key,Hash,Equals,Allocator,ThreadSafe, KeyTable>::resize_hashtable()
{
  m_hashtable.assign(m_hashtable.size() * 2, detail::EMPTY);

  size_t index = 0;
  for (const Key& k: m_keys)
  {
    if (check_index_validity(index))
    {
      put_in_hashtable(k, index);
    }
    ++index;
  }
}

template <class Key, typename Hash, typename Equals, typename Allocator, bool ThreadSafe, typename KeyTable>
inline indexed_set<Key,Hash,Equals,Allocator,ThreadSafe, KeyTable>::indexed_set(std::size_t number_of_threads)
  : indexed_set(number_of_threads, detail::minimal_hashtable_size)
{
} 

template <class Key, typename Hash, typename Equals, typename Allocator, bool ThreadSafe, typename KeyTable>
inline indexed_set<Key,Hash,Equals,Allocator,ThreadSafe,KeyTable>::indexed_set(
           std::size_t number_of_threads,
           std::size_t initial_size,
           const hasher& hasher,
           const key_equal& equals)
      : m_hashtable(std::max(initial_size, detail::minimal_hashtable_size), detail::EMPTY), 
        m_mutex(new std::mutex()),
        m_thread_control(number_of_threads),
        m_hasher(hasher),
        m_equals(equals)
{
}

template <class Key, typename Hash, typename Equals, typename Allocator, bool ThreadSafe, typename KeyTable>
inline typename indexed_set<Key,Hash,Equals,Allocator,ThreadSafe, KeyTable>::size_type 
                        indexed_set<Key,Hash,Equals,Allocator,ThreadSafe,KeyTable>::index(const key_type& key, std::size_t thread_index) const
{
  lock_shared(thread_index);
  assert(m_hashtable.size()>0);
  std::size_t start = ((m_hasher(key) * detail::PRIME_NUMBER) >> 2) % m_hashtable.size();
  std::size_t position = start;

  do
  {
    std::size_t index = m_hashtable[position];
    if (index == detail::EMPTY)
    {
      unlock_shared(thread_index);
      return npos; // Not found.
    }
    assert(index < m_keys.size());
    if (m_equals(key, m_keys[index]))
    {
      unlock_shared(thread_index);
      return index;
    }

    assert(m_hashtable.size()>0);
    position = (position + detail::STEP) % m_hashtable.size();
    assert(position!=start); // The hashtable is full. This should never happen.
  }
  while (true);

  return npos; // Not found.
}

template <class Key, typename Hash, typename Equals, typename Allocator, bool ThreadSafe, typename KeyTable>
inline typename indexed_set<Key,Hash,Equals,Allocator,ThreadSafe,KeyTable>::const_iterator indexed_set<Key,Hash,Equals,Allocator,ThreadSafe,KeyTable>::find(const key_type& key, std::size_t thread_index) const
{
  const std::size_t idx = index(key, thread_index);
  if (idx < m_keys.size())
  {
    return begin() + idx;
  }

  return end();
}


template <class Key, typename Hash, typename Equals, typename Allocator, bool ThreadSafe, typename KeyTable>
inline const Key& indexed_set<Key,Hash,Equals,Allocator,ThreadSafe,KeyTable>::at(std::size_t index) const
{
  if (index >= m_keys.size())
  {
    throw std::out_of_range("indexed_set: index too large: " + std::to_string(index) + " > " + std::to_string(m_keys.size()) + ".");
  }

  return m_keys[index];
}

template <class Key, typename Hash, typename Equals, typename Allocator, bool ThreadSafe, typename KeyTable>
inline const Key& indexed_set<Key,Hash,Equals,Allocator,ThreadSafe,KeyTable>::operator[](std::size_t index) const
{
  assert(index<m_keys.size());
  const Key& key = m_keys[index];
  return key;
}

template <class Key, typename Hash, typename Equals, typename Allocator, bool ThreadSafe, typename KeyTable>
inline void indexed_set<Key,Hash,Equals,Allocator,ThreadSafe,KeyTable>::clear(std::size_t thread_index)
{
  lock_exclusive(thread_index);
  m_hashtable.assign(m_hashtable.size(), detail::EMPTY);

  m_keys.clear();
  for(detail::thread_control& c: m_thread_control)
  {
    c.reserved_numbers_begin=0;
    c.reserved_numbers_end=0;
  }
  unlock_exclusive(thread_index);
}


template <class Key, typename Hash, typename Equals, typename Allocator, bool ThreadSafe, typename KeyTable>
inline std::pair<std::size_t, bool> indexed_set<Key,Hash,Equals,Allocator,ThreadSafe,KeyTable>::insert(const Key& key, std::size_t thread_index)
{
  detail::thread_control& c=m_thread_control[thread_index];
  if (c.reserved_numbers_begin==c.reserved_numbers_end)
  {
    reserve_indices_for_this_thread(thread_index);
  }

  const std::size_t new_index = c.reserved_numbers_begin;
  m_keys[new_index]=key; 

  lock_shared(thread_index);
  const std::size_t index = put_in_hashtable(key, new_index);
  
  if (index != new_index) // Key already exists.
  {
    m_keys[new_index]=Key(); 
    unlock_shared(thread_index);
    return std::make_pair(index, false);
  }

  c.reserved_numbers_begin++;
  unlock_shared(thread_index);

  return std::make_pair(index, true);
}


} // namespace utilities

} // namespace mcrl2

#endif // MCRL2_UTILITIES_DETAIL_INDEXED_SET_H
