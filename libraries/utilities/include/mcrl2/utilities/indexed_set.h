// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MCRL2_UTILITIES_INDEXED_SET_H
#define MCRL2_UTILITIES_INDEXED_SET_H

#include "mcrl2/utilities/unordered_map.h"
#include "mcrl2/utilities/block_allocator.h"

namespace mcrl2
{
namespace utilities
{

/// \brief A set that assigns each element an unique index.
template<typename Key,
         typename Hash = std::hash<Key>,
         typename Equals = std::equal_to<Key>,
         typename Allocator = std::allocator<Key>,
         bool ThreadSafe = false>
class indexed_set
{
private:
  using T = std::size_t;
  using Map = unordered_map<Key, T, Hash, Equals, Allocator, ThreadSafe>;

  Map m_map;
public:
  using iterator = typename Map::iterator;
  using const_iterator = typename Map::const_iterator;

  // The interface of an unordered_map without the ability to remove individual elements.

  indexed_set() {}
  indexed_set(std::size_t initial_size) : m_map(initial_size) {}

  const T& at(const Key& key) const { return m_map.at(key); }

  iterator begin() { return m_map.begin(); }
  iterator end() { return m_map.end(); }

  const_iterator begin() const { return m_map.begin(); }
  const_iterator end() const { return m_map.end(); }

  void clear() { m_map.clear(); }
  int count(const Key& key) { return m_map.count(key); }

  template<typename ...Args>
  std::pair<iterator, bool> insert(const Key& key) { return m_map.emplace(key, size()); }

  iterator find(const Key& key) { return m_map.find(key); }
  const_iterator find(const Key& key) const { return m_map.find(key); }

  std::size_t size() const { return m_map.size(); }
};

/// \brief A specialization for large indexed sets that uses the block_allocator internally (by default).
template<typename Key,
         typename Hash = std::hash<Key>,
         typename Equals = std::equal_to<Key>,
         typename Allocator = mcrl2::utilities::block_allocator<Key>,
         bool ThreadSafe = false>
using indexed_set_large = indexed_set<Key, Hash, Equals, Allocator, ThreadSafe>;

}
}


#endif // MCRL2_UTILITIES_INDEXED_SET_H
