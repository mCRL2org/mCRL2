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

#include "mcrl2/utilities/unordered_set.h"

#include "mcrl2/utilities/power_of_two.h"

#include <memory>

namespace mcrl2::utilities
{

static constexpr std::size_t minimum_size = 4UL;

inline float bytes_to_megabytes(std::size_t bytes)
{
  return static_cast<float>(bytes) / (1024.0f * 1024.0f);
}

MCRL2_UNORDERED_SET_TEMPLATES
MCRL2_UNORDERED_SET_CLASS::unordered_set(const unordered_set& set)
{
  reserve(set.size());

  for (auto& element : set)
  {
    emplace(element);
  }
}

MCRL2_UNORDERED_SET_TEMPLATES
MCRL2_UNORDERED_SET_CLASS& MCRL2_UNORDERED_SET_CLASS::operator=(const unordered_set& set)
{
  clear();
  reserve(set.size());

  for (auto& element : set)
  {
    emplace(element);
  }

  return *this;
}

MCRL2_UNORDERED_SET_TEMPLATES
MCRL2_UNORDERED_SET_CLASS::~unordered_set()
{
  // This unordered_set is not moved-from.
  if (m_buckets.size() > 0)
  {
    clear();
  }
}

MCRL2_UNORDERED_SET_TEMPLATES
void MCRL2_UNORDERED_SET_CLASS::clear()
{
  assert(m_buckets.size() != 0);

  // A naive implementation of erasing all elements.
  for (auto it = begin(); it != end(); )
  {
    it = erase(it);
  }

  assert(std::all_of(m_buckets.begin(), m_buckets.end(), [](const bucket_type& bucket) { return bucket.empty(); }));
  assert(m_number_of_elements == 0);
  assert(m_buckets.size() != 0);
}

MCRL2_UNORDERED_SET_TEMPLATES
template<typename ...Args>
std::pair<typename MCRL2_UNORDERED_SET_CLASS::iterator, bool> MCRL2_UNORDERED_SET_CLASS::emplace(Args&&... args)
{
  auto bucket_it = find_bucket_index(args...);
  auto it = find_impl(bucket_it, args...);

  if (it != end())
  {
    return std::make_pair(it, false);
  }
  else
  {
    return emplace_impl(bucket_it, std::forward<Args>(args)...);
  }
}

MCRL2_UNORDERED_SET_TEMPLATES
typename MCRL2_UNORDERED_SET_CLASS::iterator MCRL2_UNORDERED_SET_CLASS::erase(typename MCRL2_UNORDERED_SET_CLASS::const_iterator const_it)
{
  iterator& it = reinterpret_cast<iterator&>(const_it);

  // Find the bucket that is pointed to and remove the key after the before iterator.
  bucket_type& bucket = *it.get_bucket_it();

  // An element was removed from the hash table.
  --m_number_of_elements;

  // Remove the key that is after the before iterator.
  iterator result_it(it.get_bucket_it(), m_buckets.end(), it.key_before_it(), bucket.erase_after(m_allocator, it.key_before_it()));

  // We must guarantee that the iterator points the a valid key (and this might not be the case after removal).
  result_it.goto_next_bucket();
  return result_it;
}

MCRL2_UNORDERED_SET_TEMPLATES
void MCRL2_UNORDERED_SET_CLASS::erase(const Key& key)
{
  bucket_type& bucket = m_buckets[find_bucket_index(key)];

  // Loop over the elements in the bucket until the key was found.
  typename bucket_type::const_iterator before_it = bucket.before_begin();
  for (typename bucket_type::iterator it = bucket.begin(); it != bucket.end();)
  {
    if (m_equals(*it, key))
    {
      // Erase the current element and stop iterating.
      --m_number_of_elements;
      it = bucket.erase_after(m_allocator, before_it);
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
std::size_t MCRL2_UNORDERED_SET_CLASS::count(const Args&... args) const
{
  return find(args...) != end();
}

MCRL2_UNORDERED_SET_TEMPLATES
template<typename ...Args>
typename MCRL2_UNORDERED_SET_CLASS::const_iterator MCRL2_UNORDERED_SET_CLASS::find(const Args&... args) const
{
  return find_impl(find_bucket_index(args...), args...);
}

MCRL2_UNORDERED_SET_TEMPLATES
template<typename ...Args>
typename MCRL2_UNORDERED_SET_CLASS::iterator MCRL2_UNORDERED_SET_CLASS::find(const Args&... args)
{
  return find_impl(find_bucket_index(args...), args...);
}

MCRL2_UNORDERED_SET_TEMPLATES
void MCRL2_UNORDERED_SET_CLASS::rehash(std::size_t number_of_buckets)
{
  // Ensure that the number of buckets is a power of two greater than the minimum size.
  number_of_buckets = std::max(utilities::round_up_to_power_of_two(number_of_buckets), minimum_size);

  // If n is greater than the current number of buckets in the container (bucket_count), a rehash is forced.
  // The new bucket count can either be equal or greater than n. Otherwise, it has no effect.
  if (number_of_buckets <= bucket_count())
  {
    return;
  }

  // Create one bucket list for all elements in the hashtable.
  bucket_type old_keys;
  for (auto&& bucket : m_buckets)
  {
    old_keys.splice_after(old_keys.before_begin(), bucket);
  }

  // Recreate the hash table, but don't move or copy the old elements.
  {
    // clear() doesn't actually free the memory used and still results in an 3n peak.
    std::vector<bucket_type>().swap(m_buckets);
  }
  m_buckets.resize(number_of_buckets);
  m_buckets_mask = m_buckets.size() - 1;

  // Fill the set with all elements that are stored in old_keys.
  while (!old_keys.empty())
  {
    // Move the current element to this bucket.
    bucket_type& bucket = m_buckets[find_bucket_index(old_keys.front())];
    bucket.splice_front(bucket.begin(), old_keys);
  }

  // The number of elements remain the same, so don't change this counter.
}

template<typename T>
void print_performance_statistics(const T& unordered_set)
{
  // Calculate a histogram of the bucket lengths.
  std::vector<std::size_t> histogram;

  for (std::size_t index = 0; index < unordered_set.bucket_count(); ++index)
  {
    // Ensure that the current bucket fits within the histogram.
    std::size_t bucket_length = unordered_set.bucket_size(index);
    histogram.resize(std::max(histogram.size(), bucket_length + 1));
    ++histogram[bucket_length];
  }

  mCRL2log(mcrl2::log::debug, "Performance") << "Table stores " << unordered_set.size() << " keys, in " << unordered_set.bucket_count() << " buckets.\n";

  for (std::size_t i = 0; i < histogram.size(); ++i)
  {
    mCRL2log(mcrl2::log::debug, "Performance") << "There are " << histogram[i] << " buckets that store " << i << " keys.\n";
  }
}

/// Private functions

MCRL2_UNORDERED_SET_TEMPLATES
template<typename ...Args>
std::pair<typename MCRL2_UNORDERED_SET_CLASS::iterator, bool> MCRL2_UNORDERED_SET_CLASS::emplace_impl(size_type bucket_index, Args&&... args)
{
  auto& bucket = m_buckets[bucket_index];

  // Construct a new node and put it at the front of the bucket list.
  bucket.emplace_front(m_allocator, args...);

  ++m_number_of_elements;
  rehash_if_needed();
  return std::make_pair(iterator(m_buckets.begin() + bucket_index, m_buckets.end(), bucket.before_begin(), bucket.begin()), true);
}

MCRL2_UNORDERED_SET_TEMPLATES
template<typename ...Args>
std::size_t MCRL2_UNORDERED_SET_CLASS::find_bucket_index(const Args&... args) const
{
  std::size_t hash = m_hash(args...);
  /// n mod 2^i is equal to n & (2^i - 1).
  assert(m_buckets_mask == m_buckets.size() - 1);
  std::size_t index = hash & m_buckets_mask;
  assert(index < m_buckets.size());
  return index;
}

MCRL2_UNORDERED_SET_TEMPLATES
template<typename ...Args>
typename MCRL2_UNORDERED_SET_CLASS::const_iterator MCRL2_UNORDERED_SET_CLASS::find_impl(size_type bucket_index, const Args&... args) const
{
  const auto& bucket = m_buckets[bucket_index];

  // Search through the bucket to find an element that is equivalent.
  auto before_it = bucket.before_begin();
  for(auto it = bucket.begin(); it != bucket.end(); ++it)
  {
    auto& key = *it;
    if (m_equals(key, args...))
    {
      return const_iterator(m_buckets.begin() + bucket_index, m_buckets.end(), before_it, it);
    }

    before_it = it;
  }

  return end();
}

MCRL2_UNORDERED_SET_TEMPLATES
template<typename ...Args>
typename MCRL2_UNORDERED_SET_CLASS::iterator MCRL2_UNORDERED_SET_CLASS::find_impl(size_type bucket_index, const Args&... args)
{
  auto& bucket = m_buckets[bucket_index];

  auto before_it = bucket.before_begin();
  for(auto it = bucket.begin(); it != bucket.end(); ++it)
  {
    auto& key = *it;
    if (m_equals(key, args...))
    {
      return iterator(m_buckets.begin() + bucket_index, m_buckets.end(), before_it, it);
    }

    before_it = it;
  }

  return end();
}

MCRL2_UNORDERED_SET_TEMPLATES
void MCRL2_UNORDERED_SET_CLASS::rehash_if_needed()
{
  if (load_factor() >= max_load_factor() && !ThreadSafe)
  {
    rehash(m_buckets.size() * 2);
  }
}

#undef MCRL2_UNORDERED_SET_CLASS
#undef MCRL2_UNORDERED_SET_TEMPLATES

} // namespace mcrl2::utilities

#endif // MCRL2_UTILITIES_UNORDERED_SET_IMPLEMENTATION_H
