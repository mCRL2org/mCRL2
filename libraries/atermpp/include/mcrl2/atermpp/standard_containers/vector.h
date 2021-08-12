// Author(s): Jan Friso Groote, Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
//
/// \file mcrl2/data/standard_containers/vector.h
/// \brief This file contains a vector class that behaves 
///        exactly as a standard vector. It can only be used
///        to store class instances that derive from aterms.
///        The stored aterms are protected as a whole, i.e.,
///        time and memory is saved as individual protection
///        per element is unnecessary. 

#ifndef MCRL2_ATERMPP_STANDARD_CONTAINER_VECTOR_H
#define MCRL2_ATERMPP_STANDARD_CONTAINER_VECTOR_H

#include <vector>
#include "mcrl2/atermpp/detail/aterm_container.h"

/// \brief The main namespace for the aterm++ library.
namespace atermpp
{

/// \brief A vector class in which aterms can be stored. 
template < class T, class Alloc = std::allocator<detail::reference_aterm<T> > > 
class vector : protected detail::generic_aterm_container<std::vector<detail::reference_aterm<T>, Alloc> >, 
               public std::vector< detail::reference_aterm<T>, Alloc >
{
protected:
  typedef std::vector< detail::reference_aterm<T>, Alloc > super;
  typedef detail::generic_aterm_container<std::vector<detail::reference_aterm<T>, Alloc> > container_wrapper;

public:
  
  /// Standard typedefs.
  typedef Alloc allocator_type;
  typedef T value_type;
  typedef typename super::size_type size_type;
  
  /// \brief Default constructor.
  vector()
   : container_wrapper(*this, true),
     super()
  {}

  /// \brief Constructor.
  explicit vector (const allocator_type& alloc)
   : container_wrapper(*this, true),
     super::vector(alloc)
  {}

  /// \brief Constructor.
  explicit vector (size_type n, const allocator_type& alloc = allocator_type())
   : container_wrapper(*this, true),
     super::vector(n, alloc)
  {}

  vector (size_type n, const value_type& val, const allocator_type& alloc = allocator_type())
   : container_wrapper(*this, true),
     super::vector(n, detail::reference_aterm(val), alloc)
  {}

  /// \brief Constructor.
  template <class InputIterator>
  vector (InputIterator first, InputIterator last, const allocator_type& alloc = allocator_type())
   : container_wrapper(*this, true),
     super::vector(first, last, alloc)
  {}
    
  /// \brief Constructor.
  vector (const vector& x)
   : container_wrapper(*this, true),
     super::vector(x)
  {}

  /// \brief Constructor.
  vector (const vector& x, const allocator_type& alloc)
   : container_wrapper(*this, true),
     super::vector(x, alloc)
  {}
  
  /// \brief Constructor.
  vector (vector&& x)
   : container_wrapper(*this, true),
     super::vector(std::move(x))
  {}

  /// \brief Constructor.
  vector (vector&& x, const allocator_type& alloc)
   : container_wrapper(*this, true),
     super::vector(std::move(x), alloc)
  {}

  /// \brief Constructor. To be done later....
  vector (std::initializer_list<value_type> il, const allocator_type& alloc = allocator_type())
    : container_wrapper(*this, true),
      super::vector(il, alloc)
  {}

  /// \brief Assignment operator.
  vector& operator= (const vector& x) = default;
  
  /// \brief Move assignment operator
  vector& operator=(vector&& x) = default;

  /// \brief Standard destructor.
  ~vector()=default;

};

} // namespace atermpp
#endif // MCRL2_ATERMPP_STANDARD_CONTAINER_VECTOR_H
