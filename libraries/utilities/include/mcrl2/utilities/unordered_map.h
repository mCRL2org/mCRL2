// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MCRL2_UTILITIES_UNORDERED_MAP_H
#define MCRL2_UTILITIES_UNORDERED_MAP_H

#include "mcrl2/utilities/unordered_set.h"
#include "mcrl2/utilities/block_allocator.h"

#include <utility>
#include <functional>

namespace mcrl2
{
namespace utilities
{

/// \brief A class for a map of keys to values in T based using the simple hash table set implementation.
template<typename Key,
         typename T,
         typename Hash = std::hash<Key>,
         typename Equals = std::equal_to<Key>,
         typename Allocator = std::allocator<Key>,
         bool ThreadSafe = false>
class unordered_map
{
private:
  using Pair = std::pair<Key, T>;
  using PairAllocator = typename Allocator::template rebind<Pair>::other;

  // Hashes only the keys of each pair.
  struct PairHash
  {
    /// Clang 3.8: Default initialization of an object of const type requires a user-provided default constructor
    PairHash() {}

    std::size_t operator()(const Pair& pair) const
    {
      return Hash()(pair.first);
    }

    std::size_t operator()(const Key& key) const
    {
      return Hash()(key);
    }
  };

  // Compares only the keys of each pair.
  struct PairEquals
  {
    bool operator()(const Pair& first, const Pair& second) const
    {
      return Equals()(first.first, second.first);
    }

    bool operator()(const Pair& first, const Key& key) const
    {
      return Equals()(first.first, key);
    }
  };

  using Set = unordered_set<Pair, PairHash, PairEquals, PairAllocator, ThreadSafe>;

  Set m_set; ///< The underlying set storing <key, value> pairs.

public:
  using iterator = typename Set::iterator;
  using const_iterator = typename Set::const_iterator;

  /// \brief Provides access to the value associated with the given key, constructs a default
  ///        value whenever the key was undefined.
  T& operator[](const Key& key);

  /// \brief Provides access to the value associated with the given key.
  const T& at(const Key& key) const;

  iterator begin() { return m_set.begin(); }
  iterator end() { return m_set.end(); }

  const_iterator begin() const { return m_set.begin(); }
  const_iterator end() const { return m_set.end(); }

  void clear() { m_set.clear(); }
  int count(const Key& key) { return m_set.count(key); }

  template<typename ...Args>
  std::pair<iterator, bool> emplace(const Args&... args) { return m_set.emplace(args...); }

  void erase(const Key& key) { m_set.erase(key); }
  iterator erase(iterator it) { return m_set.erase(it); }
  iterator find(const Key& key) { return m_set.find(key); }

  std::pair<iterator, bool> insert(const Pair& pair) { return m_set.emplace(pair); }

  std::size_t size() const { return m_set.size(); }
};

/// \brief A specialization for large unordered maps that uses the block_allocator internally by default.
template<typename Key,
         typename T,
         typename Hash = std::hash<Key>,
         typename Equals = std::equal_to<Key>,
         typename Allocator = mcrl2::utilities::block_allocator<Key>,
         bool ThreadSafe = false>
using unordered_map_large = unordered_map<Key, T, Hash, Equals, Allocator, ThreadSafe>;

} // namespace utilities
} // namespace mcrl2

#include "unordered_map_implementation.h"

#endif // MCRL2_UTILITIES_UNORDERED_MAP_H
