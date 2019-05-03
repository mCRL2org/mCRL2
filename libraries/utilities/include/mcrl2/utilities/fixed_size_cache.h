// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_UTILITIES_FIXED_SIZE_CACHE_H
#define MCRL2_UTILITIES_FIXED_SIZE_CACHE_H

#include "mcrl2/utilities/unordered_map.h"

namespace mcrl2
{
namespace utilities
{

/// \brief A cache keeps track of key-value pairs similar to a map. The difference is that a cache
///        has (an optional) maximum size and a policy that determines what element gets evicted when
///        the cache is full.
template<typename Key, typename T>
class fixed_size_cache
{
private:
  using Map = unordered_map_large<Key, T>;
  using Pair = std::pair<Key, T>;

public:
  using iterator = typename Map::iterator;

  fixed_size_cache(std::size_t max_size)
    : m_map(max_size)
  {
    if (max_size == 0)
    {
      m_maximum_size = std::numeric_limits<std::size_t>::max();
    }
  }

  iterator begin() { return m_map.begin(); }
  iterator end() { return m_map.end(); }

  iterator find(const Key& key) { return m_map.find(key); }

  /// \brief Stores the given key-value pair in the cache. Depending on the cache policy and capacity an existing elements
  ///        might be removed.
  template<typename ...Args>
  std::pair<iterator, bool> emplace(Args&&... args)
  {
    if (m_map.size() +1 >= m_maximum_size)
    {
      // Remove an existing element, for now an arbitrary choice.
      m_map.erase(m_map.begin());
    }

    return m_map.emplace(std::forward<Args>(args)...);
  }

private:
  Map m_map;

  std::size_t m_maximum_size; ///< The maximum number of elements to cache.
};

} // namespace utilities
} // namespace mcrl2

#endif // MCRL2_UTILITIES_FIXED_SIZE_CACHE_H
