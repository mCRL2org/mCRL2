// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_UTILITIES_CACHE_POLICY_H
#define MCRL2_UTILITIES_CACHE_POLICY_H

#include "mcrl2/utilities/unordered_map.h"

namespace mcrl2
{
namespace utilities
{

/// \brief An interface to implement a replacement policy for the fixed_size_cache.
template<typename Key,
  typename T,
  typename Map_ = unordered_map_large<Key, T>>
class replacement_policy
{
public:
  using Map = Map_;

  replacement_policy(Map& map)
    : m_map(map)
  {}

  /// \brief Called whenever a new element has been inserted into the cache.
  virtual void inserted(const Key& key) = 0;

  /// \returns An iterator to the key that should be replaced when the cache is full.
  virtual typename Map::iterator replacement_candidate() = 0;

  /// \brief Called whenever an element was found in the cache.
  virtual void touch(const Key& key) = 0;

protected:
  Map& map() { return m_map; }

private:
  Map& m_map; ///< The map on which the policy operates.
};

/// \brief A policy that replaces an arbitrary (but not random) element.
template<typename Key, typename T>
class no_policy final : public replacement_policy<Key, T>
{
private:
  using super = replacement_policy<Key, T>;

public:
  using Map = typename super::Map;

  no_policy(Map& map)
    : super(map)
  {}

  typename Map::iterator replacement_candidate() override { return super::map().begin(); }

  void inserted(const Key&) override
  {}

  void touch(const Key&) override
  {}
};

template<typename Key, typename T>
class fifo_policy final : public replacement_policy<Key, T>
{
private:
  using super = replacement_policy<Key, T>;

public:
  using Map = typename super::Map;

  fifo_policy(Map& map)
    : super(map)
  {
    m_last_element_it = m_queue.before_begin();
  }

  typename Map::iterator replacement_candidate() override
  {
    assert(!m_queue.empty());
    // Remove the first key (the first one to be inserted into the queue).
    auto it = super::map().find(m_queue.front());
    m_queue.erase_after(m_queue.before_begin());
    assert(it != super::map().end());
    return it;
  }

  void inserted(const Key& key) override
  {
    // A new key was inserted, so it must be the last one to be removed.
    m_last_element_it = m_queue.insert_after(m_last_element_it, key);
  }

  void touch(const Key&) override
  {}

private:
  std::forward_list<Key> m_queue;
  typename std::forward_list<Key>::iterator m_last_element_it;
};

} // namespace utilities
} // namespace mcrl2

#endif // MCRL2_UTILITIES_CACHE_POLICY_H
