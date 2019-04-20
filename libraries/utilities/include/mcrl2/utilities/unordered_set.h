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

#include "mcrl2/utilities/const.h"
#include "mcrl2/utilities/power_of_two.h"
#include "mcrl2/utilities/spinlock.h"
#include "mcrl2/utilities/detail/bucket_list.h"

namespace mcrl2
{
namespace utilities
{

/// \brief A unordered_set with a subset of the interface of std::unordered_set that only stores a single pointer for each element.
/// \details Only supports input iterators (not bidirectional) compared to std::unordered_set. Furthermore, iterating over all elements
///          in the set is O(n + m), where n is the number of elements in the set and m the number of empty buckets. Also incrementing the
///          iterator is O(m) as opposed to O(1) as the standard mandates.
///          Finally, the unordered_set supports allocators that have a specialized allocate_args(args...) to vary the allocation size based
///          on the arguments used. This is required to store _aterm_appl classes with the function symbol arity determined at runtime.
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

  /// Move and copy constructors.
  unordered_set(const unordered_set& set)
  {
    for (auto& element : set)
    {
      emplace(element);
    }
  }

  unordered_set& operator=(const unordered_set& set)
  {
    clear();
    for (auto& element : set)
    {
      emplace(element);
    }
    return *this;
  }

  unordered_set(unordered_set&& other) = default;
  unordered_set& operator=(unordered_set&& other) = default;

  ~unordered_set() { clear(); }

  /// \returns An iterator over all keys.
  iterator begin() { return iterator(m_buckets.begin(), m_buckets.end(), typename Bucket::iterator(*m_buckets.begin())); }
  iterator end() { return iterator(m_buckets.end()); }

  /// \returns A const iterator over all keys.
  const_iterator begin() const { return const_iterator(m_buckets.begin(), m_buckets.end(), typename Bucket::const_iterator(*m_buckets.begin())); }
  const_iterator end() const { return const_iterator(m_buckets.end()); }

  /// \brief Removes all elements from the set.
  void clear();

  /// \brief Constructs an element Key(args...) and inserts it.
  template<typename ...Args>
  std::pair<iterator, bool> emplace(const Args&... args);

  /// \brief Erases the given key from the unordered set.
  /// \details Needs to find the key first.
  void erase(Key& key);

  /// \brief Erases the element pointed to by the iterator.
  /// \returns An iterator to the next key.
  iterator erase(iterator it);

  /// \brief Searches whether an object Key(args...) occurs in the set.
  /// \returns An iterator to the matching element or the end when this object does not exist.
  template<typename...Args>
  const_iterator find(const Args&... args) const;

  template<typename...Args>
  iterator find(const Args&... args);

  /// \brief Prints various information about the underlying buckets.
  void print_performance_statistics() const;

  /// \returns The amount of elements stored in this set.
  std::size_t size() const noexcept { return m_number_of_elements; }

  /// \returns The number of elements that can be present in the set before resizing.
  std::size_t capacity() const noexcept { return m_buckets.size(); }

  /// \returns A reference to the local node allocator.
  const NodeAllocator& allocator() const noexcept { return m_allocator; }
  NodeAllocator& allocator() noexcept { return m_allocator; }

private:

  /// \brief Inserts T(args...) into the given bucket, assumes that it did not exists before.
  template<typename ...Args>
  std::pair<iterator, bool> emplace_impl(Bucket& bucket, const Args&... args);

  /// \returns The bucket that might contain the element constructed by the given arguments.
  template<typename ...Args>
  const Bucket& find_bucket(const Args&... args) const;

  template<typename ...Args>
  Bucket& find_bucket(const Args&... args)
  {
    // Avoid code duplicate by calling the const version and making the resulting bucket reference non-const.
    return const_cast<Bucket&>(as_const(*this).find_bucket(args...));
  }

  /// \brief Searches for the element in the given bucket.
  template<typename ...Args>
  const_iterator find_impl(const Bucket& bucket, const Args&... args) const;

  template<typename ...Args>
  iterator find_impl(Bucket& bucket, const Args&... args);

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
