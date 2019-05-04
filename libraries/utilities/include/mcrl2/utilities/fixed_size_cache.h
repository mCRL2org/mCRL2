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

/// \brief An interface to implement a replacement policy for the fixed_size_cache.
template<typename Key, typename Map>
class replacement_policy
{
public:
  replacement_policy(Map& map)
    : m_map(map)
  {}

  /// \brief Called whenever a new element has been inserted into the cache.
  virtual void inserted(const Key& key) = 0;

  /// \returns An iterator to the key that should be replaced when the cache is full.
  virtual typename Map::iterator replacement_candidate() = 0;

  /// \brief Called whenever an element was found in the cache.
  virtual void touch(const Key& key) = 0;

public:
  Map& m_map; ///< The map on which the policy operates.
};

/// \brief A policy that replaces an arbitrary (but not completely random) element.
template<typename Key, typename Map>
class no_policy final : public replacement_policy<Key, Map>
{
private:
  using super = replacement_policy<Key, Map>;

public:

  no_policy(Map& map)
    : super(map)
  {}

  typename Map::iterator replacement_candidate() override { return super::m_map.begin(); }

  void inserted(const Key&) override
  {}

  void touch(const Key&) override
  {}
};

/// \brief A cache keeps track of key-value pairs similar to a map. The difference is that a cache
///        has (an optional) maximum size and a policy that determines what element gets evicted when
///        the cache is full.
template<typename Key,
  typename T,
  typename Map = unordered_map_large<Key, T>,
  typename Policy = no_policy<Key, Map>>
class fixed_size_cache
{
private:
  using Pair = std::pair<Key, T>;

public:
  using iterator = typename Map::iterator;

  fixed_size_cache(std::size_t max_size = 1024)
    : m_map(max_size),
      m_policy(m_map)
  {
    if (max_size == 0)
    {
      m_maximum_size = std::numeric_limits<std::size_t>::max();
    }
    else
    {
      // The reason for this is that the internal mapping might only support powers of two and as such
      // we might as well use the additional capacity.
      m_maximum_size = m_map.capacity();
    }
  }

  iterator begin() { return m_map.begin(); }
  iterator end() { return m_map.end(); }

  iterator find(const Key& key)
  {
    return m_map.find(key);
  }

  /// \brief Stores the given key-value pair in the cache. Depending on the cache policy and capacity an existing elements
  ///        might be removed.
  template<typename ...Args>
  std::pair<iterator, bool> emplace(Args&&... args)
  {
    // If an element was inserted and the current cache is full.
    if (m_map.size() + 1 >= m_maximum_size)
    {
      // Remove an existing element defined by the policy.
      m_map.erase(m_policy.replacement_candidate());
    }
    else
    {
      // An existing element was updated.
      //m_policy.touch((*(result.first)).first);
    }

    assert(m_map.size() <= m_maximum_size);
    return m_map.emplace(std::forward<Args>(args)...);
  }

private:
  Map    m_map;    ///< The underlying mapping from keys to their cached results.
  Policy m_policy; ///< The replacement policy for keys in the cache.

  std::size_t m_maximum_size; ///< The maximum number of elements to cache.
};

} // namespace utilities
} // namespace mcrl2

#endif // MCRL2_UTILITIES_FIXED_SIZE_CACHE_H
