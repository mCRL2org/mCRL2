// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MCRL2_UTILITIES_UNORDERED_SET_IMPLEMENTATION_H
#define MCRL2_UTILITIES_UNORDERED_SET_IMPLEMENTATION_H
#pragma once

#define MCRL2_UNORDERED_SET_TEMPLATES template<typename Key, typename Hash, typename Equals, typename Allocator, bool ThreadSafe>
#define MCRL2_UNORDERED_SET_CLASS unordered_set<Key, Hash, Equals, Allocator, ThreadSafe>

#include "unordered_set.h"

#include "mcrl2/utilities/logger.h"

#include <iostream>

namespace mcrl2
{
namespace utilities
{

MCRL2_UNORDERED_SET_TEMPLATES
typename MCRL2_UNORDERED_SET_CLASS::iterator MCRL2_UNORDERED_SET_CLASS::erase(typename MCRL2_UNORDERED_SET_CLASS::iterator it)
{
  // Find the bucket that is pointed to and remove the key after the before iterator.
  Bucket& bucket = it.bucket();

  // An element was removed from the hash table.
  --m_number_of_elements;

  // Remove the key that is after the before iterator.
  return iterator(it.get_bucket_it(), m_buckets.end(), it.key_before_it(), bucket.erase_after(it.key_before_it(), m_allocator));
}

MCRL2_UNORDERED_SET_TEMPLATES
void MCRL2_UNORDERED_SET_CLASS::erase(Key& key)
{
  Bucket& bucket = find_bucket(key);

  // Loop over the elements in the bucket until the key was found.
  typename Bucket::iterator before_it = bucket.before_begin();
  for (typename Bucket::iterator it = bucket.begin(); it != bucket.end();)
  {
    if (Equals()(*it, key))
    {
      // Erase the current element and stop iterating.
      --m_number_of_elements;
      it = bucket.erase_after(before_it, m_allocator);
      break;
    }
    else
    {
      // Both iterators move one place forward.
      ++before_it;
      ++it;
    }
  }
}

MCRL2_UNORDERED_SET_TEMPLATES
template<typename ...Args>
typename MCRL2_UNORDERED_SET_CLASS::const_iterator MCRL2_UNORDERED_SET_CLASS::find(const Args&... args) const
{
  const Bucket& bucket = find_bucket(args...);

  for(typename Bucket::const_iterator it = bucket.begin(); it != bucket.end(); ++it)
  {
    auto& key = *it;
    if (Equals()(key, args...))
    {
      return const_iterator(it);
    }
  }

  return end();
}

MCRL2_UNORDERED_SET_TEMPLATES
template<typename ...Args>
typename MCRL2_UNORDERED_SET_CLASS::iterator MCRL2_UNORDERED_SET_CLASS::find(const Args&... args)
{
  Bucket& bucket = find_bucket(args...);

  for(typename Bucket::iterator it = bucket.begin(); it != bucket.end(); ++it)
  {
    auto& key = *it;
    if (Equals()(key, args...))
    {
      return iterator(it);
    }
  }

  return end();
}

MCRL2_UNORDERED_SET_TEMPLATES
template<typename ...Args>
Key& MCRL2_UNORDERED_SET_CLASS::emplace(const Args&... args)
{
  // Lock the bucket and search for the key in the bucket list.
  Bucket& bucket = find_bucket(args...);

  /*if (ThreadSafe)
  {
    // Try to find an existing entry in the bucket list.
    bucket.lock();

    auto result = find(args...);
    if (result != end())
    {
      if (ThreadSafe)
      {
        bucket.unlock();
      }
      return *result;
    }
  }*/

  // Construct a new node and put it at the front of the bucket list.
  typename Bucket::node* new_node = m_allocator.allocate_and_construct(args...);

  bucket.push_front(new_node);
  ++m_number_of_elements;
  resize_if_needed();

  // End of critical section.
  /*if (ThreadSafe)
  {
    bucket.unlock();
  }*/
  return new_node->key();
}

inline float bytes_to_megabytes(std::size_t bytes)
{
  return static_cast<float>(bytes) / (1024.0f * 1024.0f);
}

MCRL2_UNORDERED_SET_TEMPLATES
void MCRL2_UNORDERED_SET_CLASS::print_performance_statistics() const
{
  // Calculate a histogram of the bucket lengths.
  std::vector<std::size_t> histogram;

  for (auto& bucket : m_buckets)
  {
    std::size_t bucketLength = 0;
    for (auto& key : bucket)
    {
      (void)key;
      ++bucketLength;
    }

    // Ensure that the current bucket fits within the histogram.
    histogram.resize(std::max(histogram.size(), bucketLength + 1));
    ++histogram[bucketLength];
  }

  mCRL2log(mcrl2::log::debug, "Performance") << "Table stores " << size() << " keys, using approximately "
    << bytes_to_megabytes(m_allocator.capacity() * sizeof(typename Bucket::node)) << " MB for elements, and "
    << bytes_to_megabytes(m_buckets.size() * sizeof(Bucket)) << " MB for buckets.\n";
  for (std::size_t i = 0; i < histogram.size(); ++i)
  {
    mCRL2log(mcrl2::log::debug, "Performance") << "There are " << histogram[i] << " buckets that store " << i << " keys.\n";
  }
}

/// Private functions

MCRL2_UNORDERED_SET_TEMPLATES
void MCRL2_UNORDERED_SET_CLASS::resize_if_needed()
{
  if (m_number_of_elements >= capacity() && !ThreadSafe)
  {
    resize();
  }
}

MCRL2_UNORDERED_SET_TEMPLATES
void MCRL2_UNORDERED_SET_CLASS::resize()
{
  // Create one bucket list for all elements in the hashtable.
  Bucket old_keys;
  for (auto&& bucket : m_buckets)
  {
    for (auto it = bucket.begin(); it != bucket.end();)
    {
      // The insertion will change the current node, which influences the iterator.
      typename Bucket::node* node = it.get_node();
      ++it;
      old_keys.push_front(node);
    }
  }

  // Calculate the new size before clearing the old buckets.
  std::size_t new_size = m_buckets.size() * 2;

  // Recreate the hash table, but don't move or copy the old elements.
  {
    // clear() doesn't actually free the memory used and still results in an 3n peak.
    std::vector<Bucket>().swap(m_buckets);
  }
  m_buckets.resize(new_size);
  m_buckets_mask = m_buckets.size() - 1;

  // Fill the set with all elements of the previous unordered set.
  for (auto it = old_keys.begin(); it != old_keys.end(); )
  {
    // The insertion will change the current node, which influences the iterator.
    typename Bucket::node* node = it.get_node();
    ++it;
    insert(node);
  }

  // The number of elements remain the same, so don't change this counter.
}

#undef MCRL2_UNORDERED_SET_CLASS
#undef MCRL2_UNORDERED_SET_TEMPLATES

} // namespace utilities
} // namespace mcrl2

#endif // MCRL2_UTILITIES_UNORDERED_SET_IMPLEMENTATION_H
