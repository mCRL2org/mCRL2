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

#include <forward_list>

#include <cassert>

namespace mcrl2::utilities
{

/// \brief An interface to implement a replacement policy for the fixed_size_cache.
template<typename Map>
class replacement_policy
{
public:
  virtual ~replacement_policy() = default;
  using key_type = typename Map::key_type;
  using map_type = Map;

protected:
  /// \brief Called whenever the underlying cache is cleared.
  virtual void clear() = 0;

  /// \brief Called whenever a new element has been inserted into the cache.
  virtual void inserted(const key_type& key) = 0;

  /// \returns An iterator to the key that should be replaced when the cache is full.
  virtual typename Map::iterator replacement_candidate(Map& map) = 0;

  /// \brief Called whenever an element was found in the cache.
  virtual void touch(const key_type& key) = 0;
};

/// \brief A policy that replaces an arbitrary (but not random) element.
template<typename Map>
class no_policy final : public replacement_policy<Map>
{
public:
  using key_type = typename Map::key_type;

  typename Map::iterator replacement_candidate(Map& map) override { return map.begin(); }

  // Not implemented.
  void clear() override {}
  void inserted(const key_type&) override {}
  void touch(const key_type&) override {}
};

template<typename Map>
class fifo_policy final : public replacement_policy<Map>
{
public:
  using key_type = typename Map::key_type;

  fifo_policy()
      : m_last_element_it(m_queue.before_begin())
  {}

  fifo_policy(const fifo_policy& other)
    : m_queue(other.m_queue)
  {
    update_last_element_it();
  }

  fifo_policy& operator=(const fifo_policy& other)
  {
    m_queue = other.m_queue;
    update_last_element_it();
    return *this;
  }

  // These moves work when moving m_queue guarantees that m_last_element_it remains valid.
  fifo_policy(fifo_policy&& other) noexcept = default;
  fifo_policy& operator=(fifo_policy&& other) noexcept = default;

  void clear() override
  {
    m_queue.clear();
    update_last_element_it();
  }

  typename Map::iterator replacement_candidate(Map& map) override
  {
    assert(!m_queue.empty());
    // Remove the first key (the first one to be inserted into the queue).
    auto it = map.find(m_queue.front());
    m_queue.erase_after(m_queue.before_begin());
    assert(it != map.end());
    return it;
  }

  void inserted(const key_type& key) override
  {
    // A new key was inserted, so it must be the last one to be removed.
    m_last_element_it = m_queue.insert_after(m_last_element_it, key);
  }

  void touch(const key_type&) override
  {}

private:
  void update_last_element_it()
  {
    // Update the iterator to the last element of the queue.
    for (auto it = m_queue.before_begin(); it != m_queue.end(); ++it)
    {
      m_last_element_it = it;
    }
  }

  std::forward_list<key_type> m_queue;
  typename std::forward_list<key_type>::iterator m_last_element_it;
};

} // namespace mcrl2::utilities

#endif // MCRL2_UTILITIES_CACHE_POLICY_H
