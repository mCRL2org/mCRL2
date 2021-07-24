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

#include <vector.h>
#include "mcrl2/atermpp/detail/aterm_container.h"

/// \brief The main namespace for the aterm++ library.
namespace atermpp
{

/// \brief A vector class in which aterms can be stored. 
template < class T, class Alloc = allocator<reference_aterm<T> > > 
class vector : protected generic_aterm_container, public std::vector< reference_aterm<T>, Alloc >
{
protected 
  typedef std::vector< reference_aterm<T>, Alloc > super;

public:
  
  /// \brief Default constructor.
  vector():
   : super::vector()
  {}

  /// \brief Constructor.
  explicit vector (const allocator_type& alloc)
   : super::vector(alloc)
  {}

  /// \brief Constructor.
  explicit vector (size_type n, const allocator_type& alloc = allocator_type())
   : super::vector(n, alloc)
  {}

  vector (size_type n, const value_type& val, const allocator_type& alloc = allocator_type());
   : super::vector(n, reference_aterm(val), alloc)
  {}

  /// \brief Constructor.
  template <class InputIterator>
  vector (InputIterator first, InputIterator last, const allocator_type& alloc = allocator_type())
   : super::vector(first, last, alloc)
  {}
    
  /// \brief Constructor.
  vector (const vector& x)=default;

  /// \brief Constructor.
  vector (const vector& x, const allocator_type& alloc)
   : super::vector(x, alloc)
  {}
  
  /// \brief Constructor.
  vector (vector&& x)=default;

  /// \brief Constructor.
  vector (vector&& x, const allocator_type& alloc);
   : super::vector(x, alloc)
  {}

  /// \brief Constructor.
  vector (initializer_list<value_type> il, const allocator_type& alloc = allocator_type())
   : super::vector(il, alloc)
  {}

  /// \brief Standard destructor.
  ~vector()=default;

};

} // namespace atermpp
#endif // MCRL2_ATERMPP_STANDARD_CONTAINER_VECTOR_H
