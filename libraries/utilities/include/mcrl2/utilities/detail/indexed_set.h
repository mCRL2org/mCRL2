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

#include "mcrl2/utilities/unused.h"
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

static constexpr std::size_t RESERVED(std::numeric_limits<std::size_t>::max()-1);

static constexpr float max_load_factor = 0.75f; ///< The load factor before the hash table is resized.

static constexpr std::size_t PRIME_NUMBER = 999953;

#ifndef NDEBUG  // Numbers are small in debug mode for more intensive checks. 
static constexpr std::size_t minimal_hashtable_size = 8; 
static constexpr std::size_t RESERVATION_SIZE = 8;
#else
static constexpr std::size_t minimal_hashtable_size = 1024;
static constexpr std::size_t RESERVATION_SIZE = 1024;
#endif

static_assert(RESERVATION_SIZE <= minimal_hashtable_size);
static_assert(minimal_hashtable_size>=8);       ///< With a max_load of 0.75 the minimal size of the hashtable must be 8.

} // namespace detail

#define INDEXED_SET_TEMPLATE template <class Key, typename Hash, typename Equals, typename Allocator, bool ThreadSafe, typename KeyTable>
#define INDEXED_SET indexed_set<Key,Hash,Equals,Allocator,ThreadSafe, KeyTable>

INDEXED_SET_TEMPLATE
inline void INDEXED_SET::reserve_indices_for_this_thread(std::size_t thread_index)
{
  lock_exclusive(thread_index);
  if (m_next_index+m_thread_control.size()>=m_keys.size())   // otherwise another process already reserved entries, and nothing needs to be done. 
  {
    assert(thread_index<m_thread_control.size());
    assert(m_next_index<=m_keys.size());
    m_keys.resize(m_keys.size()+detail::RESERVATION_SIZE);

    while ((detail::max_load_factor * m_hashtable.size()) < m_keys.size())
    {
       resize_hashtable();
    }
  }
  unlock_exclusive(thread_index);
}


INDEXED_SET_TEMPLATE
inline void INDEXED_SET::lock_shared(std::size_t thread_index) const
{
  assert(ThreadSafe || thread_index==0);
  if constexpr (ThreadSafe)
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
}

INDEXED_SET_TEMPLATE
inline void INDEXED_SET::unlock_shared(std::size_t thread_index) const
{
  assert(ThreadSafe || thread_index==0);
  if constexpr (ThreadSafe)
  {
    assert(m_thread_control[thread_index].busy_flag);
    m_thread_control[thread_index].busy_flag.store(false, std::memory_order_release);
  }
}

INDEXED_SET_TEMPLATE
inline void INDEXED_SET::lock_exclusive(std::size_t thread_index) const
{
  assert(ThreadSafe || thread_index==0);
  if constexpr (ThreadSafe)
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
}

INDEXED_SET_TEMPLATE
inline void INDEXED_SET::unlock_exclusive(std::size_t thread_index) const
{
  assert(ThreadSafe || thread_index==0);
  if constexpr (ThreadSafe)
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
}

INDEXED_SET_TEMPLATE
inline typename INDEXED_SET::size_type INDEXED_SET::put_in_hashtable(const key_type& key, std::size_t value, std::size_t& new_position)
{
  // Find a place to insert key and find whether key already exists.
  assert(m_hashtable.size()>0);

  new_position = ((m_hasher(key) * detail::PRIME_NUMBER) >> 2) % m_hashtable.size();
  std::size_t start = new_position;
  utilities::mcrl2_unused(start); // suppress warning in release mode. 

  while (true)
  {
    std::size_t index = m_hashtable[new_position];
    assert(index == detail::EMPTY || index < m_keys.size());

    if (index == detail::EMPTY)
    {
      // Found an empty spot, insert a new index belonging to key,
      std::size_t pos=detail::EMPTY;
      if (reinterpret_cast<std::atomic<std::size_t>*>(&m_hashtable[new_position])->compare_exchange_strong(pos,value))
      {
        return value;
      }
      index=pos;             // Insertion failed, but another process put an alternative value "pos"
                             // at this position. 
    }
    // If the index is RESERVED, we go into a busy loop, as another process 
    // will shortly change the RESERVED value into a sensible index. 
    if (index != detail::RESERVED) 
    {
      assert(index!=detail::EMPTY);
      if (m_equals(m_keys[index], key))
      {
        // key is already in the set, return position of key.
        assert(index<m_next_index && m_next_index<=m_keys.size());
        return index;
      }
      assert(m_hashtable.size()>0);
      new_position = (new_position + detail::STEP) % m_hashtable.size();
      assert(new_position != start); // In this case the hashtable is full, which should never happen.
    }
  }

  // not reached. 
  return detail::EMPTY;
}

INDEXED_SET_TEMPLATE
inline void INDEXED_SET::resize_hashtable()
{
  m_hashtable.assign(m_hashtable.size() * 2, detail::EMPTY);

  size_t index = 0;
  for (const Key& k: m_keys)
  {
    if (index<m_next_index)
    {
      std::size_t new_position;  // The resulting new_position is not used here. 
      put_in_hashtable(k, index, new_position);
    }
    else 
    {
      break;
    }
    ++index;
  }
}

INDEXED_SET_TEMPLATE
inline INDEXED_SET::indexed_set(std::size_t number_of_threads)
  : indexed_set(number_of_threads, detail::minimal_hashtable_size)
{
} 

INDEXED_SET_TEMPLATE
inline INDEXED_SET::indexed_set(
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

INDEXED_SET_TEMPLATE
inline typename INDEXED_SET::size_type INDEXED_SET::index(const key_type& key, std::size_t thread_index) const
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
    // If the index is RESERVED, go into a busy loop. Another thread will 
    // change this RESERVED index shortly into a sensible index. 
    if (index != detail::RESERVED)
    {
      assert(index < m_keys.size());
      if (m_equals(key, m_keys[index]))
      {
        unlock_shared(thread_index);
        assert(index<m_next_index && m_next_index<=m_keys.size());
        return index;
      }

      assert(m_hashtable.size()>0);
      position = (position + detail::STEP) % m_hashtable.size();
      assert(position!=start); // The hashtable is full. This should never happen.
    }
  }
  while (true);

  return npos; // Dummy return.
}

INDEXED_SET_TEMPLATE
inline typename INDEXED_SET::const_iterator INDEXED_SET::find(const key_type& key, std::size_t thread_index) const
{
  const std::size_t idx = index(key, thread_index);
  if (idx < m_keys.size())
  {
    return begin() + idx;
  }

  return end();
}


INDEXED_SET_TEMPLATE
inline const Key& INDEXED_SET::at(std::size_t index) const
{
  if (index >= m_next_index)
  {
    throw std::out_of_range("indexed_set: index too large: " + std::to_string(index) + " > " + std::to_string(m_keys.size()) + ".");
  }

  return m_keys[index];
}

INDEXED_SET_TEMPLATE
inline const Key& INDEXED_SET::operator[](std::size_t index) const
{
  assert(index<m_keys.size());
  const Key& key = m_keys[index];
  return key;
}

INDEXED_SET_TEMPLATE
inline void INDEXED_SET::clear(std::size_t thread_index)
{
  lock_exclusive(thread_index);
  m_hashtable.assign(m_hashtable.size(), detail::EMPTY);

  m_keys.clear();
  m_next_index.store(0);
  unlock_exclusive(thread_index);
}


INDEXED_SET_TEMPLATE
inline std::pair<typename INDEXED_SET::size_type, bool> INDEXED_SET::insert(const Key& key, std::size_t thread_index)
{
  lock_shared(thread_index);
  assert(m_next_index<=m_keys.size());
  if (m_next_index+m_thread_control.size()>=m_keys.size())
  {
    unlock_shared(thread_index);
    reserve_indices_for_this_thread(thread_index);
    lock_shared(thread_index);
  }

  std::size_t new_position;
  const std::size_t index = put_in_hashtable(key, detail::RESERVED, new_position);
  
  if (index != detail::RESERVED) // Key already exists.
  {
    unlock_shared(thread_index);
    assert(index<m_next_index && m_next_index<=m_keys.size());
    return std::make_pair(index, false);
  }
  const std::size_t new_index=m_next_index.fetch_add(1);
  assert(new_index<m_keys.size());
  m_keys[new_index]=key; 
  m_hashtable[new_position]=new_index;

  unlock_shared(thread_index);

  assert(new_index<m_next_index && m_next_index<=m_keys.size());
  return std::make_pair(new_index, true);
}

#undef INDEXED_SET_TEMPLATE 
#undef INDEXED_SET 


} // namespace utilities

} // namespace mcrl2

#endif // MCRL2_UTILITIES_DETAIL_INDEXED_SET_H
