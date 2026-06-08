// Author(s): Jan Friso Groote, Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
//
/// \file mcrl2/atermpp/standard_containers/detail/unordered_set_implementation.h
/// \brief This file contains an implementation of the hash function
///        to break circular header dependencies.

#ifndef MCRL2_ATERMPP_STANDARD_CONTAINER_DETAIL_UNORDERED_SET_IMPLEMENTATION_H
#define MCRL2_ATERMPP_STANDARD_CONTAINER_DETAIL_UNORDERED_SET_IMPLEMENTATION_H

#include "mcrl2/atermpp/standard_containers/unordered_set.h"
#include "mcrl2/atermpp/detail/thread_aterm_pool.h"
#include "mcrl2/utilities/shared_mutex.h"

namespace atermpp
{

  template<class Key, class Hash, class Pred, class Alloc>
  void unordered_set<Key,Hash,Pred,Alloc>::clear() noexcept
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    super::clear();
  }

  template<class Key, class Hash, class Pred, class Alloc>
  std::pair<typename unordered_set<Key,Hash,Pred,Alloc>::iterator, bool>
  unordered_set<Key,Hash,Pred,Alloc>::insert(const value_type& value)
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::insert(value);
  }

  template<class Key, class Hash, class Pred, class Alloc>
  template<class P>
  std::pair<typename unordered_set<Key,Hash,Pred,Alloc>::iterator, bool>
  unordered_set<Key,Hash,Pred,Alloc>::insert(P&& value)
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::insert(std::forward<P>(value));
  }

  template<class Key, class Hash, class Pred, class Alloc>
  typename unordered_set<Key,Hash,Pred,Alloc>::iterator
  unordered_set<Key,Hash,Pred,Alloc>::insert(const_iterator hint, const value_type& value)
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::insert(hint, value);
  }

  template<class Key, class Hash, class Pred, class Alloc>
  template<class P>
  typename unordered_set<Key,Hash,Pred,Alloc>::iterator
  unordered_set<Key,Hash,Pred,Alloc>::insert(const_iterator hint, P&& value)
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::insert(hint, std::forward<P>(value));
  }

  template<class Key, class Hash, class Pred, class Alloc>
  template<class InputIt>
  void unordered_set<Key,Hash,Pred,Alloc>::insert(InputIt first, InputIt last)
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    super::insert(first, last);
  }

  template<class Key, class Hash, class Pred, class Alloc>
  void unordered_set<Key,Hash,Pred,Alloc>::insert(std::initializer_list<value_type> ilist)
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    super::insert(ilist);
  }

  template<class Key, class Hash, class Pred, class Alloc>
  typename unordered_set<Key,Hash,Pred,Alloc>::insert_return_type
  unordered_set<Key,Hash,Pred,Alloc>::insert(node_type&& nh)
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::insert(std::move(nh));
  }

  template<class Key, class Hash, class Pred, class Alloc>
  typename unordered_set<Key,Hash,Pred,Alloc>::iterator
  unordered_set<Key,Hash,Pred,Alloc>::insert(const_iterator hint, node_type&& nh)
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::insert(hint, std::move(nh));
  }

  template<class Key, class Hash, class Pred, class Alloc>
  template<class... Args>
  std::pair<typename unordered_set<Key,Hash,Pred,Alloc>::iterator, bool>
  unordered_set<Key,Hash,Pred,Alloc>::emplace(Args&&... args)
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::emplace(std::forward<Args>(args)...);
  }

  template<class Key, class Hash, class Pred, class Alloc>
  template<class... Args>
  typename unordered_set<Key,Hash,Pred,Alloc>::iterator
  unordered_set<Key,Hash,Pred,Alloc>::emplace_hint(const_iterator hint, Args&&... args)
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::emplace_hint(hint, std::forward<Args>(args)...);
  }

  template<class Key, class Hash, class Pred, class Alloc>
  typename unordered_set<Key,Hash,Pred,Alloc>::iterator
  unordered_set<Key,Hash,Pred,Alloc>::erase(iterator pos)
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::erase(pos);
  }

  template<class Key, class Hash, class Pred, class Alloc>
  typename unordered_set<Key,Hash,Pred,Alloc>::iterator
  unordered_set<Key,Hash,Pred,Alloc>::erase(const_iterator pos)
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::erase(pos);
  }

  template<class Key, class Hash, class Pred, class Alloc>
  typename unordered_set<Key,Hash,Pred,Alloc>::iterator
  unordered_set<Key,Hash,Pred,Alloc>::erase(const_iterator first, const_iterator last)
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::erase(first, last);
  }

  template<class Key, class Hash, class Pred, class Alloc>
  typename unordered_set<Key,Hash,Pred,Alloc>::size_type
  unordered_set<Key,Hash,Pred,Alloc>::erase(const Key& key)
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::erase(key);
  }

  template<class Key, class Hash, class Pred, class Alloc>
  void unordered_set<Key,Hash,Pred,Alloc>::swap(unordered_set& other) noexcept
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    super::swap(other);
  }

  template<class Key, class Hash, class Pred, class Alloc>
  typename unordered_set<Key,Hash,Pred,Alloc>::node_type
  unordered_set<Key,Hash,Pred,Alloc>::extract(const_iterator position)
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::extract(position);
  }

  template<class Key, class Hash, class Pred, class Alloc>
  typename unordered_set<Key,Hash,Pred,Alloc>::node_type
  unordered_set<Key,Hash,Pred,Alloc>::extract(const Key& k)
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::extract(k);
  }

  template<class Key, class Hash, class Pred, class Alloc>
  template<class H2, class P2>
  void unordered_set<Key,Hash,Pred,Alloc>::merge(std::unordered_set<Key, H2, P2, allocator_type>& source)
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    super::merge(source);
  }

  template<class Key, class Hash, class Pred, class Alloc>
  template<class H2, class P2>
  void unordered_set<Key,Hash,Pred,Alloc>::merge(std::unordered_set<Key, H2, P2, allocator_type>&& source)
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    super::merge(std::move(source));
  }

  template<class Key, class Hash, class Pred, class Alloc>
  template<class H2, class P2>
  void unordered_set<Key,Hash,Pred,Alloc>::merge(std::unordered_multiset<Key, H2, P2, allocator_type>& source)
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    super::merge(source);
  }

  template<class Key, class Hash, class Pred, class Alloc>
  template<class H2, class P2>
  void unordered_set<Key,Hash,Pred,Alloc>::merge(std::unordered_multiset<Key, H2, P2, allocator_type>&& source)
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    super::merge(std::move(source));
  }

  template<class Key, class Hash, class Pred, class Alloc>
  typename unordered_set<Key,Hash,Pred,Alloc>::iterator
  unordered_set<Key,Hash,Pred,Alloc>::begin()
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::begin();
  }

  template<class Key, class Hash, class Pred, class Alloc>
  typename unordered_set<Key,Hash,Pred,Alloc>::iterator
  unordered_set<Key,Hash,Pred,Alloc>::end()
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::end();
  }

  template<class Key, class Hash, class Pred, class Alloc>
  typename unordered_set<Key,Hash,Pred,Alloc>::const_iterator
  unordered_set<Key,Hash,Pred,Alloc>::begin() const
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::begin();
  }

  template<class Key, class Hash, class Pred, class Alloc>
  typename unordered_set<Key,Hash,Pred,Alloc>::const_iterator
  unordered_set<Key,Hash,Pred,Alloc>::end() const
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::end();
  }

  template<class Key, class Hash, class Pred, class Alloc>
  typename unordered_set<Key,Hash,Pred,Alloc>::const_iterator
  unordered_set<Key,Hash,Pred,Alloc>::cbegin() const
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::cbegin();
  }

  template<class Key, class Hash, class Pred, class Alloc>
  typename unordered_set<Key,Hash,Pred,Alloc>::const_iterator
  unordered_set<Key,Hash,Pred,Alloc>::cend() const
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::cend();
  }

  template<class Key, class Hash, class Pred, class Alloc>
  bool unordered_set<Key,Hash,Pred,Alloc>::empty() const noexcept
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::empty();
  }

  template<class Key, class Hash, class Pred, class Alloc>
  typename unordered_set<Key,Hash,Pred,Alloc>::size_type
  unordered_set<Key,Hash,Pred,Alloc>::max_size() const noexcept
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::max_size();
  }

} // namespace atermpp

namespace atermpp::utilities
{

  template<class Key, class Hash, class Pred, class Alloc, bool ThreadSafe>
  inline void unordered_set<Key,Hash,Pred,Alloc,ThreadSafe>::rehash(std::size_t new_size)
  {
    if constexpr (ThreadSafe)
    {
      mcrl2::utilities::lock_guard guard = detail::g_thread_term_pool().lock();
      super::rehash(new_size);
    }
    else
    {
      super::rehash(new_size);
    }
  }

  template<class Key, class Hash, class Pred, class Alloc, bool ThreadSafe>
  inline void unordered_set<Key,Hash,Pred,Alloc,ThreadSafe>::rehash_if_needed()
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    std::size_t count = super::bucket_count();
    if (super::load_factor() >= super::max_load_factor())
    {
      guard.unlock_shared();
      rehash(count * 2);
    }
  }

  template<class Key, class Hash, class Pred, class Alloc, bool ThreadSafe>
  void unordered_set<Key,Hash,Pred,Alloc,ThreadSafe>::clear() noexcept
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    super::clear();
  }

  template<class Key, class Hash, class Pred, class Alloc, bool ThreadSafe>
  template<typename... Args>
  typename unordered_set<Key,Hash,Pred,Alloc,ThreadSafe>::iterator
  unordered_set<Key,Hash,Pred,Alloc,ThreadSafe>::find(const Args&... args)
  {
    if constexpr (ThreadSafe)
    {
      mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
      return super::find(args...);
    }
    return super::find(args...);
  }

  template<class Key, class Hash, class Pred, class Alloc, bool ThreadSafe>
  template<typename... Args>
  typename unordered_set<Key,Hash,Pred,Alloc,ThreadSafe>::const_iterator
  unordered_set<Key,Hash,Pred,Alloc,ThreadSafe>::find(const Args&... args) const
  {
    if constexpr (ThreadSafe)
    {
      mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
      return super::find(args...);
    }
    return super::find(args...);
  }

  template<class Key, class Hash, class Pred, class Alloc, bool ThreadSafe>
  template<typename... Args>
  std::pair<typename unordered_set<Key,Hash,Pred,Alloc,ThreadSafe>::iterator, bool>
  unordered_set<Key,Hash,Pred,Alloc,ThreadSafe>::emplace(Args&&... args)
  {
    rehash_if_needed();
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::emplace(std::forward<Args>(args)...);
  }

  template<class Key, class Hash, class Pred, class Alloc, bool ThreadSafe>
  template<typename... Args>
  void unordered_set<Key,Hash,Pred,Alloc,ThreadSafe>::erase(const Args&... args)
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    super::erase(args...);
  }

  template<class Key, class Hash, class Pred, class Alloc, bool ThreadSafe>
  typename unordered_set<Key,Hash,Pred,Alloc,ThreadSafe>::iterator
  unordered_set<Key,Hash,Pred,Alloc,ThreadSafe>::erase(const_iterator it)
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::erase(it);
  }

  template<class Key, class Hash, class Pred, class Alloc, bool ThreadSafe>
  void unordered_set<Key,Hash,Pred,Alloc,ThreadSafe>::swap(unordered_set& other) noexcept
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    super::swap(other);
  }

  template<class Key, class Hash, class Pred, class Alloc, bool ThreadSafe>
  typename unordered_set<Key,Hash,Pred,Alloc,ThreadSafe>::iterator
  unordered_set<Key,Hash,Pred,Alloc,ThreadSafe>::begin()
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::begin();
  }

  template<class Key, class Hash, class Pred, class Alloc, bool ThreadSafe>
  typename unordered_set<Key,Hash,Pred,Alloc,ThreadSafe>::iterator
  unordered_set<Key,Hash,Pred,Alloc,ThreadSafe>::end()
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::end();
  }

  template<class Key, class Hash, class Pred, class Alloc, bool ThreadSafe>
  typename unordered_set<Key,Hash,Pred,Alloc,ThreadSafe>::const_iterator
  unordered_set<Key,Hash,Pred,Alloc,ThreadSafe>::begin() const
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::begin();
  }

  template<class Key, class Hash, class Pred, class Alloc, bool ThreadSafe>
  typename unordered_set<Key,Hash,Pred,Alloc,ThreadSafe>::const_iterator
  unordered_set<Key,Hash,Pred,Alloc,ThreadSafe>::end() const
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::end();
  }

  template<class Key, class Hash, class Pred, class Alloc, bool ThreadSafe>
  typename unordered_set<Key,Hash,Pred,Alloc,ThreadSafe>::const_iterator
  unordered_set<Key,Hash,Pred,Alloc,ThreadSafe>::cbegin() const
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::cbegin();
  }

  template<class Key, class Hash, class Pred, class Alloc, bool ThreadSafe>
  typename unordered_set<Key,Hash,Pred,Alloc,ThreadSafe>::const_iterator
  unordered_set<Key,Hash,Pred,Alloc,ThreadSafe>::cend() const
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::cend();
  }

  template<class Key, class Hash, class Pred, class Alloc, bool ThreadSafe>
  bool unordered_set<Key,Hash,Pred,Alloc,ThreadSafe>::empty() const noexcept
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::empty();
  }

  template<class Key, class Hash, class Pred, class Alloc, bool ThreadSafe>
  typename unordered_set<Key,Hash,Pred,Alloc,ThreadSafe>::size_type
  unordered_set<Key,Hash,Pred,Alloc,ThreadSafe>::max_size() const noexcept
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::max_size();
  }

} // namespace atermpp::utilities

#endif // MCRL2_ATERMPP_STANDARD_CONTAINER_DETAIL_UNORDERED_SET_IMPLEMENTATION_H
