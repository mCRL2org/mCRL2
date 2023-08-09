// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
//
/// \file mcrl2/atermpp/standard_containers/detail/unordered_map_implementation.h
/// \brief This file contains an implementation of the hash function 
///        to break circular header dependencies. 

#ifndef MCRL2_ATERMPP_STANDARD_CONTAINER_DETAIL_UNORDERED_MAP_IMPLEMENTATION_H
#define MCRL2_ATERMPP_STANDARD_CONTAINER_DETAIL_UNORDERED_MAP_IMPLEMENTATION_H

#include "mcrl2/atermpp/standard_containers/unordered_map.h"
#include "mcrl2/atermpp/detail/thread_aterm_pool.h"

namespace atermpp
{

  template< class Key, class T, class Hash, class Pred, class Alloc >  
  void unordered_map<Key,T,Hash,Pred,Alloc>::clear() noexcept
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    super::clear();
  }

  /// \brief Inserts an element referring to a default value in the map. 
  template< class Key, class T, class Hash, class Pred, class Alloc >
  std::pair<typename unordered_map<Key,T,Hash,Pred,Alloc>::iterator,bool> unordered_map<Key,T,Hash,Pred,Alloc>::insert( const value_type& value )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::insert(value);
  }
    
  template< class Key, class T, class Hash, class Pred, class Alloc >
  template< class P >
  std::pair<typename unordered_map<Key,T,Hash,Pred,Alloc>::iterator,bool> unordered_map<Key,T,Hash,Pred,Alloc>::insert( P&& value )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::insert(value); 
  }

  template< class Key, class T, class Hash, class Pred, class Alloc >
  typename unordered_map<Key,T,Hash,Pred,Alloc>::iterator unordered_map<Key,T,Hash,Pred,Alloc>::insert( const_iterator hint, const value_type& value )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::insert(hint, value);
  }
  
  template< class Key, class T, class Hash, class Pred, class Alloc >
  template< class P >
  typename unordered_map<Key,T,Hash,Pred,Alloc>::iterator unordered_map<Key,T,Hash,Pred,Alloc>::insert( const_iterator hint, P&& value )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::insert(hint, value);
  }
  
  template< class Key, class T, class Hash, class Pred, class Alloc >
  template< class InputIt >
  void unordered_map<Key,T,Hash,Pred,Alloc>::insert( InputIt first, InputIt last )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    super::insert(first, last);
  }

  template< class Key, class T, class Hash, class Pred, class Alloc >
  void unordered_map<Key,T,Hash,Pred,Alloc>::insert( std::initializer_list<value_type> ilist )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    super::insert(ilist);
  }

  template< class Key, class T, class Hash, class Pred, class Alloc >
  typename unordered_map<Key,T,Hash,Pred,Alloc>::insert_return_type unordered_map<Key,T,Hash,Pred,Alloc>::insert( node_type&& nh )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::insert(nh);
  }

  template< class Key, class T, class Hash, class Pred, class Alloc >
  typename unordered_map<Key,T,Hash,Pred,Alloc>::iterator unordered_map<Key,T,Hash,Pred,Alloc>::insert( const_iterator hint, node_type&& nh )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::insert(hint, nh);
  }
  
  template< class Key, class T, class Hash, class Pred, class Alloc >
  template <class M>
  std::pair<typename unordered_map<Key,T,Hash,Pred,Alloc>::iterator, bool> unordered_map<Key,T,Hash,Pred,Alloc>::insert_or_assign( const Key& k, M&& obj )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::insert_or_assign(k, obj);
  }
  
  template< class Key, class T, class Hash, class Pred, class Alloc >
  template <class M>
  std::pair<typename unordered_map<Key,T,Hash,Pred,Alloc>::iterator, bool> unordered_map<Key,T,Hash,Pred,Alloc>::insert_or_assign( Key&& k, M&& obj )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::insert_or_assign(k, obj);
  }
  
  template< class Key, class T, class Hash, class Pred, class Alloc >
  template <class M>
  typename unordered_map<Key,T,Hash,Pred,Alloc>::iterator unordered_map<Key,T,Hash,Pred,Alloc>::insert_or_assign( const_iterator hint, const Key& k, M&& obj )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::insert_or_assign(hint, k, obj); 
  }
  
  template< class Key, class T, class Hash, class Pred, class Alloc >
  template <class M>
  typename unordered_map<Key,T,Hash,Pred,Alloc>::iterator unordered_map<Key,T,Hash,Pred,Alloc>::insert_or_assign( const_iterator hint, Key&& k, M&& obj )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::insert_or_assign(hint, k, obj);   
  }
  
  template< class Key, class T, class Hash, class Pred, class Alloc >
  template< class... Args >
  std::pair<typename unordered_map<Key,T,Hash,Pred,Alloc>::iterator,bool> unordered_map<Key,T,Hash,Pred,Alloc>::emplace( Args&&... args )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::emplace(args...);
  }
  
  template< class Key, class T, class Hash, class Pred, class Alloc >
  template <class... Args>
  typename unordered_map<Key,T,Hash,Pred,Alloc>::iterator unordered_map<Key,T,Hash,Pred,Alloc>::emplace_hint( const_iterator hint, Args&&... args )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::emplace_hint(hint, args...);
  }
  
  template< class Key, class T, class Hash, class Pred, class Alloc >
  template< class... Args >
  std::pair<typename unordered_map<Key,T,Hash,Pred,Alloc>::iterator, bool> unordered_map<Key,T,Hash,Pred,Alloc>::try_emplace( const Key& k, Args&&... args )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::try_emplace(k, args...);
  }
  
  template< class Key, class T, class Hash, class Pred, class Alloc >
  template< class... Args >
  std::pair<typename unordered_map<Key,T,Hash,Pred,Alloc>::iterator, bool> unordered_map<Key,T,Hash,Pred,Alloc>::try_emplace( Key&& k, Args&&... args )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::try_emplace(k, args...);
  }
  
  template< class Key, class T, class Hash, class Pred, class Alloc >
  template< class... Args >
  typename unordered_map<Key,T,Hash,Pred,Alloc>::iterator unordered_map<Key,T,Hash,Pred,Alloc>::try_emplace( const_iterator hint, const Key& k, Args&&... args )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::try_emplace(hint, k, args...);
  }
  
  template< class Key, class T, class Hash, class Pred, class Alloc >
  template< class... Args >
  typename unordered_map<Key,T,Hash,Pred,Alloc>::iterator unordered_map<Key,T,Hash,Pred,Alloc>::try_emplace( const_iterator hint, Key&& k, Args&&... args )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::try_emplace(hint, k, args...);
  }

  template< class Key, class T, class Hash, class Pred, class Alloc >
  typename unordered_map<Key,T,Hash,Pred,Alloc>::iterator unordered_map<Key,T,Hash,Pred,Alloc>::erase( iterator pos )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::erase(pos);
  }

  template< class Key, class T, class Hash, class Pred, class Alloc >
  typename unordered_map<Key,T,Hash,Pred,Alloc>::iterator unordered_map<Key,T,Hash,Pred,Alloc>::erase( const_iterator pos )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::erase(pos);   
  }	
  
  template< class Key, class T, class Hash, class Pred, class Alloc >
  typename unordered_map<Key,T,Hash,Pred,Alloc>::iterator unordered_map<Key,T,Hash,Pred,Alloc>::erase( const_iterator first, const_iterator last )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::erase(first, last);
  }

  template< class Key, class T, class Hash, class Pred, class Alloc >
  typename unordered_map<Key,T,Hash,Pred,Alloc>::size_type unordered_map<Key,T,Hash,Pred,Alloc>::erase( const Key& key )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::erase(key);
  }

  template< class Key, class T, class Hash, class Pred, class Alloc >
  void unordered_map<Key,T,Hash,Pred,Alloc>::swap( unordered_map& other )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    super::swap(other);
  }

  template< class Key, class T, class Hash, class Pred, class Alloc >
  typename unordered_map<Key,T,Hash,Pred,Alloc>::node_type unordered_map<Key,T,Hash,Pred,Alloc>::extract( const_iterator position )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return extract(position);
  }
  
  template< class Key, class T, class Hash, class Pred, class Alloc >
  typename unordered_map<Key,T,Hash,Pred,Alloc>::node_type unordered_map<Key,T,Hash,Pred,Alloc>::extract( const Key& k )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return extract(k);
  }

  template< class Key, class T, class Hash, class Pred, class Alloc >
  template<class H2, class P2>
  void unordered_map<Key,T,Hash,Pred,Alloc>::merge( std::unordered_map<Key, T, H2, P2, allocator_type>& source )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return merge(source);
  }
  
  template< class Key, class T, class Hash, class Pred, class Alloc >
  template<class H2, class P2>
  void unordered_map<Key,T,Hash,Pred,Alloc>::merge( std::unordered_map<Key, T, H2, P2, allocator_type>&& source )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return merge(source);
  }
  
  template< class Key, class T, class Hash, class Pred, class Alloc >
  template<class H2, class P2>
  void unordered_map<Key,T,Hash,Pred,Alloc>::merge( std::unordered_multimap<Key, T, H2, P2, allocator_type>& source )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return merge(source);
  }
  
  template< class Key, class T, class Hash, class Pred, class Alloc >
  template<class H2, class P2>
  void unordered_map<Key,T,Hash,Pred,Alloc>::merge( std::unordered_multimap<Key, T, H2, P2, allocator_type>&& source )
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return merge(source);
  }
} // namespace atermpp

namespace atermpp::utilities {  

  template< class Key, class T, class Hash, class Pred, class Alloc, bool ThreadSafe >
  inline void unordered_map<Key,T,Hash,Pred,Alloc,ThreadSafe>::rehash(std::size_t new_size )
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

  template< class Key, class T, class Hash, class Pred, class Alloc, bool ThreadSafe >
  void unordered_map<Key,T,Hash,Pred,Alloc,ThreadSafe>::clear() noexcept
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    super::clear();
  }
  
  template< class Key, class T, class Hash, class Pred, class Alloc, bool ThreadSafe >
  template<typename ...Args>
  typename unordered_map<Key,T,Hash,Pred,Alloc,ThreadSafe>::iterator unordered_map<Key,T,Hash,Pred,Alloc,ThreadSafe>::find(const Args&... args)
  { 
    if constexpr (ThreadSafe)
    {
      mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
      return super::find(args...); 
    }
    return super::find(args...); 
  }
  
  template< class Key, class T, class Hash, class Pred, class Alloc, bool ThreadSafe >
  template<typename ...Args>
  typename unordered_map<Key,T,Hash,Pred,Alloc,ThreadSafe>::const_iterator unordered_map<Key,T,Hash,Pred,Alloc,ThreadSafe>::find(const Args&... args) const 
  { 
    if (ThreadSafe)
    {
      mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
      return super::find(args...); 
    }
    return super::find(args...); 
  }

  template< class Key, class T, class Hash, class Pred, class Alloc, bool ThreadSafe >
  std::pair<typename unordered_map<Key,T,Hash,Pred,Alloc,ThreadSafe>::iterator, bool> unordered_map<Key,T,Hash,Pred,Alloc,ThreadSafe>::insert(const value_type& value)
  {
    rehash_if_needed();
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::insert(value);
  }
  
  template< class Key, class T, class Hash, class Pred, class Alloc, bool ThreadSafe >
  template< class P >
  std::pair<typename unordered_map<Key,T,Hash,Pred,Alloc,ThreadSafe>::iterator, bool> unordered_map<Key,T,Hash,Pred,Alloc,ThreadSafe>::insert(P&& value)
  {
    rehash_if_needed();
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::insert(value);
  }

  template< class Key, class T, class Hash, class Pred, class Alloc, bool ThreadSafe >
  typename unordered_map<Key,T,Hash,Pred,Alloc,ThreadSafe>::iterator unordered_map<Key,T,Hash,Pred,Alloc,ThreadSafe>::insert(const_iterator hint, value_type&& value)
  {
    rehash_if_needed();
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::insert(hint, value);
  }
  
  template< class Key, class T, class Hash, class Pred, class Alloc, bool ThreadSafe >
  template< class P >
  typename unordered_map<Key,T,Hash,Pred,Alloc,ThreadSafe>::iterator unordered_map<Key,T,Hash,Pred,Alloc,ThreadSafe>::insert(const_iterator hint, P&& value)
  {
    rehash_if_needed();
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::insert(hint, value);
  }
  
  template< class Key, class T, class Hash, class Pred, class Alloc, bool ThreadSafe >
  template< class InputIt >
  void unordered_map<Key,T,Hash,Pred,Alloc,ThreadSafe>::insert(InputIt first, InputIt last)
  {
    rehash_if_needed();
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    super::insert(first, last);
  }

  template< class Key, class T, class Hash, class Pred, class Alloc, bool ThreadSafe >
  void unordered_map<Key,T,Hash,Pred,Alloc,ThreadSafe>::insert(std::initializer_list<value_type> ilist)
  {
    rehash_if_needed();
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    super::insert(ilist);
  }

    /*
  template< class Key, class T, class Hash, class Pred, class Alloc, bool ThreadSafe >
  insert_return_type insert(node_type&& nh)
  {
  mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::insert(nh);
  }

  template< class Key, class T, class Hash, class Pred, class Alloc, bool ThreadSafe >
  iterator insert(const_iterator hint, node_type&& nh)
  {
  mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::insert(hint, nh);
  }
  */
    
  template< class Key, class T, class Hash, class Pred, class Alloc, bool ThreadSafe >
  template <class M>
  std::pair<typename unordered_map<Key,T,Hash,Pred,Alloc,ThreadSafe>::iterator, bool> unordered_map<Key,T,Hash,Pred,Alloc,ThreadSafe>::insert_or_assign(const Key& k, M&& obj)
  {
    rehash_if_needed();
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::insert_or_assign(k, obj);
  }
  
  template< class Key, class T, class Hash, class Pred, class Alloc, bool ThreadSafe >
  template <class M>
  std::pair<typename unordered_map<Key,T,Hash,Pred,Alloc,ThreadSafe>::iterator, bool> unordered_map<Key,T,Hash,Pred,Alloc,ThreadSafe>::insert_or_assign(Key&& k, M&& obj)
  {
    rehash_if_needed();
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::insert_or_assign(k, obj);
  }
  
  template< class Key, class T, class Hash, class Pred, class Alloc, bool ThreadSafe >
  template <class M>
  typename unordered_map<Key,T,Hash,Pred,Alloc,ThreadSafe>::iterator unordered_map<Key,T,Hash,Pred,Alloc,ThreadSafe>::insert_or_assign(const_iterator hint, const Key& k, M&& obj)
  {
    rehash_if_needed();
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::insert_or_assign(hint, k, obj);
  }
  
  template< class Key, class T, class Hash, class Pred, class Alloc, bool ThreadSafe >
  template <class M>
  typename unordered_map<Key,T,Hash,Pred,Alloc,ThreadSafe>::iterator unordered_map<Key,T,Hash,Pred,Alloc,ThreadSafe>::insert_or_assign(const_iterator hint, Key&& k, M&& obj)
  {
    rehash_if_needed();
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::insert_or_assign(hint, k, obj);
  }
  
  template< class Key, class T, class Hash, class Pred, class Alloc, bool ThreadSafe >
  template< class... Args >
  std::pair<typename unordered_map<Key,T,Hash,Pred,Alloc,ThreadSafe>::iterator, bool> unordered_map<Key,T,Hash,Pred,Alloc,ThreadSafe>::emplace(Args&&... args)
  {
    rehash_if_needed();
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::emplace(args...);
  }
  
  template< class Key, class T, class Hash, class Pred, class Alloc, bool ThreadSafe >
  template <class... Args>
  typename unordered_map<Key,T,Hash,Pred,Alloc,ThreadSafe>::iterator unordered_map<Key,T,Hash,Pred,Alloc,ThreadSafe>::emplace_hint(const_iterator hint, Args&&... args)
  {
    rehash_if_needed();
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::emplace(hint, args...);
  }
  
  template< class Key, class T, class Hash, class Pred, class Alloc, bool ThreadSafe >
  template< class... Args >
  std::pair<typename unordered_map<Key,T,Hash,Pred,Alloc,ThreadSafe>::iterator, bool> unordered_map<Key,T,Hash,Pred,Alloc,ThreadSafe>::try_emplace(const Key& k, Args&&... args)
  {
    rehash_if_needed();
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::try_emplace(k, args...);
  }
  
  template< class Key, class T, class Hash, class Pred, class Alloc, bool ThreadSafe >
  template< class... Args >
  std::pair<typename unordered_map<Key,T,Hash,Pred,Alloc,ThreadSafe>::iterator, bool> unordered_map<Key,T,Hash,Pred,Alloc,ThreadSafe>::try_emplace(Key&& k, Args&&... args)
  {
    rehash_if_needed();
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::try_emplace(k, args...);
  }
  
  template< class Key, class T, class Hash, class Pred, class Alloc, bool ThreadSafe >
  template< class... Args >
  typename unordered_map<Key,T,Hash,Pred,Alloc,ThreadSafe>::iterator unordered_map<Key,T,Hash,Pred,Alloc,ThreadSafe>::try_emplace(const_iterator hint, const Key& k, Args&&... args)
  {
    rehash_if_needed();
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::try_emplace(hint, k, args...);
  }
  
  template< class Key, class T, class Hash, class Pred, class Alloc, bool ThreadSafe >
  template< class... Args >
  typename unordered_map<Key,T,Hash,Pred,Alloc,ThreadSafe>::iterator unordered_map<Key,T,Hash,Pred,Alloc,ThreadSafe>::try_emplace(const_iterator hint, Key&& k, Args&&... args)
  {
    rehash_if_needed();
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::try_emplace(hint, k, args...);
  }

  template< class Key, class T, class Hash, class Pred, class Alloc, bool ThreadSafe >
  typename unordered_map<Key,T,Hash,Pred,Alloc,ThreadSafe>::iterator unordered_map<Key,T,Hash,Pred,Alloc,ThreadSafe>::erase(iterator pos)
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::erase(pos);
  }

  template< class Key, class T, class Hash, class Pred, class Alloc, bool ThreadSafe >
  typename unordered_map<Key,T,Hash,Pred,Alloc,ThreadSafe>::iterator unordered_map<Key,T,Hash,Pred,Alloc,ThreadSafe>::erase(const_iterator pos)
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::erase(pos);
  }

  template< class Key, class T, class Hash, class Pred, class Alloc, bool ThreadSafe >
  typename unordered_map<Key,T,Hash,Pred,Alloc,ThreadSafe>::iterator unordered_map<Key,T,Hash,Pred,Alloc,ThreadSafe>::erase(const_iterator first, const_iterator last)
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::erase(first, last);
  }

  template< class Key, class T, class Hash, class Pred, class Alloc, bool ThreadSafe >
  typename unordered_map<Key,T,Hash,Pred,Alloc,ThreadSafe>::size_type unordered_map<Key,T,Hash,Pred,Alloc,ThreadSafe>::erase(const Key& key)
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    return super::erase(key);
  }

  template< class Key, class T, class Hash, class Pred, class Alloc, bool ThreadSafe >
  void unordered_map<Key,T,Hash,Pred,Alloc,ThreadSafe>::swap(unordered_map& other)
  {
    mcrl2::utilities::shared_guard guard = detail::g_thread_term_pool().lock_shared();
    super::swap(other);
  }

} // namespace atermpp::utilities

#endif // MCRL2_ATERMPP_STANDARD_CONTAINER_DETAIL_UNORDERED_MAP_IMPLEMENTATION_H
