// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MCRL2_UTILITIES_UNORDERED_SET_H
#define MCRL2_UTILITIES_UNORDERED_SET_H

#include "mcrl2/utilities/unordered_set_iterator.h"

#include "mcrl2/utilities/block_allocator.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/detail/bucket_list.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <vector>

namespace mcrl2
{
namespace utilities
{

/// \brief Prints various information for unordered_set like data structures.
template<typename T>
void print_performance_statistics(const T& unordered_set);

/// \brief A unordered_set with a subset of the interface of std::unordered_set that only stores a single pointer for each element.
/// \details Only supports input iterators (not bidirectional) compared to std::unordered_set. Furthermore, iterating over all elements
///          in the set is O(n + m), where n is the number of elements in the set and m the number of empty buckets. Also incrementing the
///          iterator is O(m) as opposed to O(1) as the standard mandates.
///
///          Additionally, the unordered_set supports allocators that have a specialized allocate_args(args...) to vary the allocation size based
///          on the arguments used. This is required to store _aterm_appl classes with the function symbol arity determined at runtime.
///
/// \todo Does not implement std::unordered_map equal_range and swap.
template<typename Key,
         typename Hash = std::hash<Key>,
         typename Equals = std::equal_to<Key>,
         typename Allocator = std::allocator<Key>,
         bool ThreadSafe = false>
class unordered_set
{

private:
  /// \brief Combine the bucket list and a lock that locks modifications to the bucket list.
  using bucket_type = detail::bucket_list<Key, Allocator>;

  using bucket_iterator = typename std::vector<bucket_type>::iterator;
  using const_bucket_iterator = typename std::vector<bucket_type>::const_iterator;

public:
  using key_type = Key;
  using value_type = Key;
  using hasher = Hash;
  using key_equal = Equals;
  using allocator_type = typename bucket_type::NodeAllocator;

  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = typename std::allocator_traits<Allocator>::pointer;
  using const_pointer = typename std::allocator_traits<Allocator>::const_pointer;

  using iterator = unordered_set_iterator<key_type, bucket_type, Allocator, false>;
  using local_iterator = typename bucket_type::iterator;
  using const_iterator = unordered_set_iterator<key_type, bucket_type, Allocator, true>;
  using const_local_iterator = typename bucket_type::const_iterator;

  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  unordered_set() { rehash(0); }

  /// \brief Constructs an unordered_set that contains bucket_count number of buckets.
  explicit unordered_set(size_type bucket_count,
    const hasher& hash = hasher(),
    const key_equal& equals = key_equal())
    : m_hash(hash),
      m_equals(equals)
  {
    rehash(bucket_count);
  }

  // Copy operators.
  unordered_set(const unordered_set& set);
  unordered_set& operator=(const unordered_set& set);

  // Default move operators.
  unordered_set(unordered_set&& other) = default;
  unordered_set& operator=(unordered_set&& other) = default;

  ~unordered_set();

  /// \returns A reference to the local node allocator.
  const allocator_type& get_allocator() const noexcept { return m_allocator; }
  allocator_type& get_allocator() noexcept { return m_allocator; }

  /// \returns An iterator over all keys.
  iterator begin() { return iterator(m_buckets.begin(), m_buckets.end()); }
  iterator end() { return iterator(m_buckets.end()); }

  /// \returns A const iterator over all keys.
  const_iterator begin() const { return const_iterator(m_buckets.begin(), m_buckets.end()); }
  const_iterator end() const { return const_iterator(m_buckets.end()); }

  /// \returns A const iterator over all keys.
  const_iterator cbegin() const { return const_iterator(m_buckets.begin(), m_buckets.end()); }
  const_iterator cend() const { return const_iterator(m_buckets.end()); }

  /// \returns True iff the set is empty.
  bool empty() const noexcept { return size() == 0; }

  /// \returns The amount of elements stored in this set.
  size_type size() const noexcept { return m_number_of_elements; }
  size_type max_size() const noexcept { return m_buckets.max_size(); }

  /// \brief Removes all elements from the set.
  /// \details Does not free the vector of buckets itself.
  void clear();

  /// \brief Inserts an element Key(args...) into the set if it did not already exist.
  /// \returns A pair of the iterator pointing to the element and a boolean that is true iff
  ///         a new element was inserted (as opposed to it already existing in the set).
  template<typename ...Args>
  std::pair<iterator, bool> emplace(Args&&... args);

  /// \brief Erases the given key from the unordered set.
  /// \details Needs to find the key first.
  void erase(key_type& key);

  /// \brief Erases the element pointed to by the iterator.
  /// \returns An iterator to the next key.
  iterator erase(iterator it);

  /// \brief Counts the number of occurrences of the given key (1 when it exists and 0 otherwise).
  template<typename ...Args>
  size_type count(const Args&... args) const;

  /// \brief Searches whether an object key_type(args...) occurs in the set.
  /// \returns An iterator to the matching element or the end when this object does not exist.
  template<typename...Args>
  const_iterator find(const Args&... args) const;

  template<typename...Args>
  iterator find(const Args&... args);

  /// \returns An iterator to the elements in the given bucket with index n.
  local_iterator begin(size_type n) { return m_buckets[n].begin(); }
  local_iterator end(size_type n) { return m_buckets[n].end(); }

  const_local_iterator begin(size_type n) const { return m_buckets[n].begin(); }
  const_local_iterator end(size_type n) const { return m_buckets[n].end(); }

  const_local_iterator cbegin(size_type n) const { return m_buckets[n].begin(); }
  const_local_iterator cend(size_type n) const { return m_buckets[n].end(); }

  /// \returns The number of buckets.
  size_type bucket_count() const noexcept { return m_buckets.size(); }
  size_type max_bucket_count() const noexcept { return m_buckets.max_size(); }

  size_type bucket_size(size_type n) const noexcept { return std::distance(m_buckets[n].begin(), m_buckets[n].end()); }
  size_type bucket(const key_type& key) const noexcept { return std::distance(m_buckets.begin(), find_bucket_index(key)); }

  float load_factor() const { return static_cast<float>(size()) / bucket_count(); }
  float max_load_factor() const { return m_max_load_factor; }
  void max_load_factor(float factor) { m_max_load_factor = factor; }

  /// \brief Resize the number buckets to at least number_of_buckets.
  void rehash(size_type number_of_buckets);

  /// \brief Resizes the set to the given number of elements.
  void reserve(size_type count) { rehash(std::ceil(static_cast<float>(count) / max_load_factor())); }

  /// \returns The number of elements that can be present in the set before resizing.
  size_type capacity() const noexcept { return m_buckets.size(); }

  hasher hash_function() const { return m_hash; }
  key_equal key_eq() const { return m_equals; }

private:

  /// \brief Inserts T(args...) into the given bucket, assumes that it did not exists before.
  template<typename ...Args>
  std::pair<iterator, bool> emplace_impl(size_type bucket_index, Args&&... args);

  /// \returns The index of the bucket that might contain the element constructed by the given arguments.
  template<typename ...Args>
  size_type find_bucket_index(const Args&... args) const;

  /// \brief Searches for the element in the given bucket.
  template<typename ...Args>
  const_iterator find_impl(size_type bucket_index, const Args&... args) const;

  template<typename ...Args>
  iterator find_impl(size_type bucket_index, const Args&... args);

  /// \brief Inserts a bucket node into the hash table.
  /// \details Does not increment the m_number_of_elements.
  void insert(typename bucket_type::node* node)
  {
    bucket_type& bucket = m_buckets[find_bucket_index(node->key())];
    bucket.push_front(node);
  }

  /// \brief Resizes the hash table if required.
  void rehash_if_needed();

  /// \brief The number of elements stored in this set.
  size_type m_number_of_elements = 0;

  /// \brief Always equal to m_buckets.size() - 1.
  size_type m_buckets_mask;

  std::vector<bucket_type> m_buckets;

  float m_max_load_factor = 1.0f;

  hasher m_hash = hasher();
  key_equal m_equals = key_equal();
  allocator_type m_allocator;
};

/// \brief A specialization for large unordered sets that uses the block_allocator internally by default.
template<typename Key,
         typename Hash = std::hash<Key>,
         typename Equals = std::equal_to<Key>,
         typename Allocator = mcrl2::utilities::block_allocator<Key>,
         bool ThreadSafe = false>
using unordered_set_large = unordered_set<Key, Hash, Equals, Allocator, ThreadSafe>;

} // namespace utilities
} // namespace mcrl2;

#include "unordered_set_implementation.h"

#endif // MCRL2_UTILITIES_UNORDERED_SET_H
