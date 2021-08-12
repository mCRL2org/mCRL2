// Author(s): Jan Friso Groote, Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
//
/// \file mcrl2/data/standard_containers/unordered_map.h
/// \brief This file contains a unordered_map class that behaves 
///        exactly as a standard unordered_map. It can only be used
///        to store class instances that derive from aterms.
///        The stored aterms are protected as a whole, i.e.,
///        time and memory is saved as individual protection
///        per element is unnecessary. 

#ifndef MCRL2_ATERMPP_STANDARD_CONTAINER_UNORDERED_MAP_H
#define MCRL2_ATERMPP_STANDARD_CONTAINER_UNORDERED_MAP_H

#include <unordered_map>
#include "mcrl2/atermpp/detail/aterm_container.h"

/// \brief The main namespace for the aterm++ library.
namespace atermpp
{

/// \brief A unordered_map class in which aterms can be stored. 
template < class Key, 
           class T,
           class Hash = std::hash<detail::reference_aterm<Key> >,
           class Pred = std::equal_to<detail::reference_aterm<Key> >,
           class Alloc = std::allocator< std::pair<const detail::reference_aterm<Key>, T > > >

class unordered_map : protected detail::generic_aterm_container<
                                     std::unordered_map<detail::reference_aterm<Key>, 
                                     T, Hash, Pred, Alloc> >, 
               public std::unordered_map< detail::reference_aterm<Key>, T, Hash, Pred, Alloc >
{
protected:
  typedef std::unordered_map< detail::reference_aterm<Key>, T, Hash, Pred, Alloc > super;
  typedef detail::generic_aterm_container<std::unordered_map< detail::reference_aterm<Key>, T, Hash, Pred, Alloc > > container_wrapper;

public:
  
  /// Standard typedefs.
  typedef Alloc allocator_type;
  typedef T value_type;
  typedef typename super::size_type size_type;
  
  /// \brief Default constructor.
  unordered_map()
   : container_wrapper(*this, true),
     super()
  {}

  /// \brief Constructor.
  explicit unordered_map (const allocator_type& alloc)
   : container_wrapper(*this, true),
     super::unordered_map(alloc)
  {}

  /// \brief Constructor.
  explicit unordered_map (size_type n, const allocator_type& alloc = allocator_type())
   : container_wrapper(*this, true),
     super::unordered_map(n, alloc)
  {}

  unordered_map (size_type n, const value_type& val, const allocator_type& alloc = allocator_type())
   : container_wrapper(*this, true),
     super::unordered_map(n, detail::reference_aterm(val), alloc)
  {}

  /// \brief Constructor.
  template <class InputIterator>
  unordered_map (InputIterator first, InputIterator last, const allocator_type& alloc = allocator_type())
   : container_wrapper(*this, true),
     super::unordered_map(first, last, alloc)
  {}
    
  /// \brief Constructor.
  unordered_map (const unordered_map& x)
   : container_wrapper(*this, true),
     super::unordered_map(x)
  {}

  /// \brief Constructor.
  unordered_map (const unordered_map& x, const allocator_type& alloc)
   : container_wrapper(*this, true),
     super::unordered_map(x, alloc)
  {}
  
  /// \brief Constructor.
  unordered_map (unordered_map&& x)
   : container_wrapper(*this, true),
     super::unordered_map(std::move(x))
  {}


  /// \brief Constructor.
  unordered_map (unordered_map&& x, const allocator_type& alloc)
   : container_wrapper(*this, true),
     super::unordered_map(std::move(x), alloc)
  {}

  /// \brief Constructor. To be done later....
  unordered_map (std::initializer_list<value_type> il, const allocator_type& alloc = allocator_type())
    : container_wrapper(*this, true),
      super::unordered_map(il, alloc)
  {}

  /// \brief Standard destructor.
  ~unordered_map()=default;

};

} // namespace atermpp
#endif // MCRL2_ATERMPP_STANDARD_CONTAINER_UNORDERED_MAP_H
