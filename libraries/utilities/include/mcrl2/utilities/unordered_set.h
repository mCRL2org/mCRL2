// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MCRL2_UTILITIES_UNORDERED_SET_H
#define MCRL2_UTILITIES_UNORDERED_SET_H

#include <array>
#include <atomic>
#include <cstddef>
#include <vector>
#include <unordered_set>

#include "mcrl2/utilities/unordered_set_iterator.h"

#include "mcrl2/utilities/detail/bucket_list.h"
#include "mcrl2/utilities/power_of_two.h"
#include "mcrl2/utilities/spinlock.h"

namespace mcrl2
{
namespace utilities
{

/// \brief A class for simple linked list hash table implementation.
template<typename Key,
         typename Hash = std::hash<Key>,
         typename Equals = std::equal_to<Key>,
         typename Allocator = std::allocator<Key>,
         bool ThreadSafe = false>
class unordered_set
{
public:
  /// \brief Combine the bucket list and a lock that locks modifications to the bucket list.
  struct lockable_bucket : public detail::bucket_list<Key, Allocator>
  {};

  using Bucket = lockable_bucket;
  using NodeAllocator = typename Bucket::NodeAllocator;
  using iterator = unordered_set_iterator<Key, Bucket, Allocator, false>;
  using const_iterator = unordered_set_iterator<Key, Bucket, Allocator, true>;

  unordered_set() :
    m_buckets(4)
  {
    m_buckets_mask = m_buckets.size() - 1;
  }

  unordered_set(std::size_t number_of_elements) :
    m_buckets(round_up_to_power_of_two(number_of_elements))
  {
    m_buckets_mask = m_buckets.size() - 1;
  }

  /// \returns An iterator over all keys.
  iterator begin() { return iterator(m_buckets.begin(), m_buckets.end(), typename Bucket::iterator(*m_buckets.begin())); }
  iterator end() { return iterator(m_buckets.end()); }

  /// \returns A const iterator over all keys.
  const_iterator begin() const { return const_iterator(m_buckets.begin(), m_buckets.end(), typename Bucket::const_iterator(*m_buckets.begin())); }
  const_iterator end() const { return const_iterator(m_buckets.end()); }

  /// \brief Constructs an element Key(args...) and inserts it.
  /// \details Assumes that the given element is not already in the set.
  template<typename ...Args>
  Key& emplace(const Args&... args);

  /// \brief Erases the given key from the unordered set.
  /// \details Needs to find the key first.
  void erase(Key& key);

  /// \brief Erases the element pointed to by the iterator.
  /// \returns An iterator to the next key.
  iterator erase(iterator it);

  template<typename...Args>
  iterator find(const Args&... args);

  /// \brief Prints various information about the underlying buckets.
  void print_performance_statistics() const;

  /// \returns The amount of elements stored in this set.
  std::size_t size() const noexcept { return m_number_of_elements; }

  /// \returns The number of elements that can be present in the set before resizing.
  std::size_t capacity() const noexcept { return m_buckets.size(); }

  /// Move and copy constructors.
  unordered_set& operator=(unordered_set&& other) = default;

  /// \returns A reference to the local node allocator.
  const NodeAllocator& allocator() const noexcept { return m_allocator; }
  NodeAllocator& allocator() noexcept { return m_allocator; }

private:

  /// \returns The bucket that might contain the element constructed by the given arguments.
  template<typename ...Args>
  Bucket& find_bucket(const Args&... args)
  {
    std::size_t hash = Hash()(args...);
    /// n mod 2^i is equal to n & (2^i - 1).
    assert(m_buckets_mask == m_buckets.size() - 1);
    std::size_t buffer = hash & m_buckets_mask;
    assert(buffer < m_buckets.size());
    return m_buckets[buffer];
  }

  /// \brief Inserts a bucket node into the hash table.
  /// \details Does not increment the m_number_of_elements.
  void insert(typename Bucket::node* node)
  {
    Bucket& bucket = find_bucket(node->key());
    bucket.push_front(node);
  }

  /// \brief Resizes the hash table if required.
  void resize_if_needed();

  /// \brief Resizes the set to the given number of elements.
  void resize();

  /// \brief The number of elements stored in this set.
  std::size_t m_number_of_elements = 0;

  /// \brief Always equal to m_buckets.size() - 1.
  std::size_t m_buckets_mask;

  std::vector<Bucket> m_buckets;
  NodeAllocator m_allocator;
};

} // namespace utilities
} // namespace mcrl2;

#include "unordered_set_implementation.h"

#endif // MCRL2_UTILITIES_UNORDERED_SET_H
