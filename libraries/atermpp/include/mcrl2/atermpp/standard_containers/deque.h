// Author(s): Jan Friso Groote, Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
//
/// \file mcrl2/data/standard_containers/deque.h
/// \brief This file contains a deque class that behaves 
///        exactly as a standard deque. It can only be used
///        to store class instances that derive from aterms.
///        The stored aterms are protected as a whole, i.e.,
///        time and memory is saved as individual protection
///        per element is unnecessary. 

#ifndef MCRL2_ATERMPP_STANDARD_CONTAINER_DEQUE_H
#define MCRL2_ATERMPP_STANDARD_CONTAINER_DEQUE_H

#include <deque>
#include "mcrl2/atermpp/detail/aterm_container.h"

/// \brief The main namespace for the aterm++ library.
namespace atermpp
{

/// \brief A deque class in which aterms can be stored. 
template < class T, class Alloc = std::allocator<detail::reference_aterm<T> > > 
class deque : protected detail::generic_aterm_container<std::deque<detail::reference_aterm<T>, Alloc> >, 
              public std::deque< detail::reference_aterm<T>, Alloc >
{
protected:
  typedef std::deque< detail::reference_aterm<T>, Alloc > super;
  typedef detail::generic_aterm_container<std::deque<detail::reference_aterm<T>, Alloc> > container_wrapper;

public:
  
  /// Standard typedefs.
  typedef Alloc allocator_type;
  typedef T value_type;
  typedef typename super::size_type size_type;
  
  /// \brief Default constructor.
  deque()
   : container_wrapper(*this, true),
     super()
  {}

  /// \brief Constructor.
  explicit deque (const allocator_type& alloc)
   : container_wrapper(*this, true),
     super::deque(alloc)
  {}

  /// \brief Constructor.
  explicit deque (size_type n, const allocator_type& alloc = allocator_type())
   : container_wrapper(*this, true),
     super::deque(n, alloc)
  {}

  /// \brief Constructor.
  deque(size_type n, const value_type& val, const allocator_type& alloc = allocator_type())
   : container_wrapper(*this, true),
     super::deque(n, detail::reference_aterm(val), alloc)
  {}

  /// \brief Constructor.
  template <class InputIterator>
  deque(InputIterator first, InputIterator last, const allocator_type& alloc = allocator_type())
   : container_wrapper(*this, true),
     super::deque(first, last, alloc)
  {}
    
  /// \brief Constructor.
  deque(const deque& x)
   : container_wrapper(*this, true),
     super::deque(x)
  {}

  /// \brief Constructor.
  deque(const deque& x, const allocator_type& alloc)
   : container_wrapper(*this, true),
     super::deque(x, alloc)
  {}
  
  /// \brief Constructor.
  deque(deque&& x)
   : container_wrapper(*this, true),
     super::deque(std::move(x))
  {}

  /// \brief Constructor.
  deque(deque&& x, const allocator_type& alloc)
   : container_wrapper(*this, true),
     super::deque(std::move(x), alloc)
  {}

  /// \brief Constructor. 
  deque(std::initializer_list<value_type> il, const allocator_type& alloc = allocator_type())
    : container_wrapper(*this, true),
      super::deque(il.begin(), il.end(), alloc)
  {}

  /// \brief Copy assignment operator.
  deque& operator=(const deque& other) = default;

  /// \brief Move assignment operator.
  deque& operator=(deque&& other) = default;

  /// \brief Standard destructor.
  ~deque()=default;

};

} // namespace atermpp
#endif // MCRL2_ATERMPP_STANDARD_CONTAINER_DEQUE_H
