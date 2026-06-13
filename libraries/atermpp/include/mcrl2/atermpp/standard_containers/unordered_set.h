// Author(s): Jan Friso Groote, Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
//
/// \file mcrl2/atermpp/standard_containers/unordered_set.h
/// \brief This file contains an unordered_set class that behaves
///        exactly as a standard unordered_set. It can only be used
///        to store class instances that derive from aterms.
///        The stored aterms are protected as a whole, i.e.,
///        time and memory is saved as individual protection
///        per element is unnecessary.

#ifndef MCRL2_ATERMPP_STANDARD_CONTAINER_UNORDERED_SET_H
#define MCRL2_ATERMPP_STANDARD_CONTAINER_UNORDERED_SET_H
#pragma once

#include <unordered_set>
#include "mcrl2/atermpp/detail/aterm_container.h"
#include "mcrl2/utilities/shared_mutex.h"

/// \brief The main namespace for the aterm++ library.
namespace atermpp
{

/// \brief An unordered_set class in which aterms can be stored.
template < class Key,
           class Hash = std::hash<detail::reference_aterm<Key>>,
           class Pred = std::equal_to<detail::reference_aterm<Key>>,
           class Alloc = std::allocator<detail::reference_aterm<Key>> >
class unordered_set : public std::unordered_set<detail::reference_aterm<Key>, Hash, Pred, Alloc>
{
protected:
  using super = std::unordered_set<detail::reference_aterm<Key>, Hash, Pred, Alloc>;

  detail::generic_aterm_container<std::unordered_set<detail::reference_aterm<Key>, Hash, Pred, Alloc>> container_wrapper;

public:

  /// Standard typedefs.
  using allocator_type = typename super::allocator_type;
  using value_type = typename super::value_type;
  using size_type = typename super::size_type;
  using node_type = typename super::node_type;
  using reference = typename super::reference;
  using iterator = typename super::iterator;
  using const_iterator = typename super::const_iterator;
  using insert_return_type = typename super::insert_return_type;

  /// \brief Default constructor.
  unordered_set()
    : super(),
      container_wrapper(*this)
  {}

  /// \brief Constructor.
  explicit unordered_set(const allocator_type& alloc)
    : super::unordered_set(alloc),
      container_wrapper(*this)
  {}

  /// \brief Constructor.
  explicit unordered_set(size_type n, const allocator_type& alloc = allocator_type())
    : super::unordered_set(n, alloc),
      container_wrapper(*this)
  {}

  /// \brief Constructor.
  template <class InputIterator>
  unordered_set(InputIterator first, InputIterator last, const allocator_type& alloc = allocator_type())
    : super::unordered_set(first, last, alloc),
      container_wrapper(*this)
  {}

  /// \brief Constructor.
  unordered_set(const unordered_set& x)
    : super::unordered_set(x),
      container_wrapper(*this)
  {}

  /// \brief Constructor.
  unordered_set(const unordered_set& x, const allocator_type& alloc)
    : super::unordered_set(x, alloc),
      container_wrapper(*this)
  {}

  /// \brief Constructor.
  unordered_set(unordered_set&& x) noexcept
    : super::unordered_set(std::move(x)),
      container_wrapper(*this)
  {}

  /// \brief Constructor.
  unordered_set(unordered_set&& x, const allocator_type& alloc)
    : super::unordered_set(std::move(x), alloc),
      container_wrapper(*this)
  {}

  /// \brief Constructor.
  unordered_set(std::initializer_list<value_type> il, const allocator_type& alloc = allocator_type())
    : super::unordered_set(il, alloc),
      container_wrapper(*this)
  {}

  /// \brief Standard assignment.
  unordered_set& operator=(const unordered_set& other) = default;

  /// \brief Standard move assignment.
  unordered_set& operator=(unordered_set&& other) = default;

  /// \brief Standard destructor.
  ~unordered_set() = default;

  void clear() noexcept;

  std::pair<iterator, bool> insert(const value_type& value);

  template<class P>
  std::pair<iterator, bool> insert(P&& value);

  iterator insert(const_iterator hint, const value_type& value);

  template<class P>
  iterator insert(const_iterator hint, P&& value);

  template<class InputIt>
  void insert(InputIt first, InputIt last);

  void insert(std::initializer_list<value_type> ilist);

  insert_return_type insert(node_type&& nh);

  iterator insert(const_iterator hint, node_type&& nh);

  template<class... Args>
  std::pair<iterator, bool> emplace(Args&&... args);

  template<class... Args>
  iterator emplace_hint(const_iterator hint, Args&&... args);

  iterator erase(iterator pos);

  // iterator erase(const_iterator pos); TODO: Enable this by making the iterator and const_iterator of aterms different. 
  //                                           A const_iterator now behaves as an iterator. 

  iterator erase(const_iterator first, const_iterator last);

  size_type erase(const Key& key);

  void swap(unordered_set& other) noexcept;

  node_type extract(const_iterator position);

  node_type extract(const Key& k);

  template<class H2, class P2>
  void merge(std::unordered_set<Key, H2, P2, allocator_type>& source);

  template<class H2, class P2>
  void merge(std::unordered_set<Key, H2, P2, allocator_type>&& source);

  template<class H2, class P2>
  void merge(std::unordered_multiset<Key, H2, P2, allocator_type>& source);

  template<class H2, class P2>
  void merge(std::unordered_multiset<Key, H2, P2, allocator_type>&& source);

  std::size_t size() const
  {
    return super::size();
  }

  /// \returns An iterator over all keys.
  iterator begin();
  iterator end();

  /// \returns A const iterator over all keys.
  const_iterator begin() const;
  const_iterator end() const;

  /// \returns A const iterator over all keys.
  const_iterator cbegin() const;
  const_iterator cend() const;

  /// \returns True iff the set is empty.
  bool empty() const noexcept;

  /// \returns The amount of elements stored in this set.
  size_type max_size() const noexcept;
};

namespace utilities
{

/// \brief An unordered_set class in which aterms can be stored.
template < class Key,
           class Hash = std::hash<detail::reference_aterm<Key>>,
           class Pred = std::equal_to<detail::reference_aterm<Key>>,
           class Alloc = std::allocator<detail::reference_aterm<Key>>,
           bool ThreadSafe = false >
class unordered_set : public mcrl2::utilities::unordered_set<detail::reference_aterm<Key>, Hash, Pred, Alloc, ThreadSafe, false>
{
protected:
  using super = mcrl2::utilities::unordered_set<detail::reference_aterm<Key>, Hash, Pred, Alloc, ThreadSafe, false>;

  detail::generic_aterm_container<mcrl2::utilities::unordered_set<detail::reference_aterm<Key>, Hash, Pred, Alloc, ThreadSafe, false>> container_wrapper;

public:

  /// Standard typedefs.
  using allocator_type = typename super::allocator_type;
  using value_type = typename super::value_type;
  using size_type = typename super::size_type;
  using reference = typename super::reference;
  using iterator = typename super::iterator;
  using const_iterator = typename super::const_iterator;

  /// \brief Default constructor.
  unordered_set()
    : super(),
      container_wrapper(*this)
  {}

  /// \brief Constructor.
  explicit unordered_set(size_type n)
    : super(n),
      container_wrapper(*this)
  {}

  /// \brief Constructor.
  unordered_set(const unordered_set& x)
    : super(x),
      container_wrapper(*this)
  {}

  /// \brief Constructor.
  unordered_set(unordered_set&& x) noexcept
    : super(std::move(x)),
      container_wrapper(*this)
  {}

  /// \brief Standard assignment.
  unordered_set& operator=(const unordered_set& other) = default;

  /// \brief Standard move assignment.
  unordered_set& operator=(unordered_set&& other) = default;

  /// \brief Standard destructor.
  ~unordered_set() = default;

  void clear() noexcept;

  /// \brief Standard find function in a set.
  /// \returns Element with the specified key.
  template<typename... Args>
  iterator find(const Args&... args);

  /// \brief Standard find function in a set.
  /// \returns Element with the specified key.
  template<typename... Args>
  const_iterator find(const Args&... args) const;

  template<typename... Args>
  std::pair<iterator, bool> emplace(Args&&... args);

  template<typename... Args>
  void erase(const Args&... args);

  iterator erase(const_iterator it);

  void swap(unordered_set& other) noexcept;

  std::size_t size() const
  {
    return super::size();
  }

  /// \returns An iterator over all keys.
  iterator begin();
  iterator end();

  /// \returns A const iterator over all keys.
  const_iterator begin() const;
  const_iterator end() const;

  /// \returns A const iterator over all keys.
  const_iterator cbegin() const;
  const_iterator cend() const;

  /// \returns True iff the set is empty.
  bool empty() const noexcept;

  /// \returns The amount of elements stored in this set.
  size_type max_size() const noexcept;

protected:

  /// Function below is implemented in a .cpp file.
  void rehash(std::size_t new_size);

  void rehash_if_needed();
};

} // namespace utilities

} // namespace atermpp

#include "mcrl2/atermpp/standard_containers/detail/unordered_set_implementation.h"

#endif // MCRL2_ATERMPP_STANDARD_CONTAINER_UNORDERED_SET_H
