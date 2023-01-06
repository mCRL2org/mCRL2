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
#include "mcrl2/atermpp/detail/shared_guard.h"

/// \brief The main namespace for the aterm++ library.
namespace atermpp
{

/// \brief A vector class in which aterms can be stored. 
template < class T, class Alloc = std::allocator<detail::reference_aterm<T> > > 
class vector : public std::vector< detail::reference_aterm<T>, Alloc >,
               protected detail::generic_aterm_container<std::vector<detail::reference_aterm<T>, Alloc> >
{
protected:
  typedef std::vector< detail::reference_aterm<T>, Alloc > super;
  typedef detail::generic_aterm_container<std::vector<detail::reference_aterm<T>, Alloc> > container_wrapper;

public: 
  /// Standard typedefs.
  typedef typename super::allocator_type allocator_type;
  typedef typename super::value_type value_type;
  typedef typename super::size_type size_type;
  typedef typename super::reference reference;
  typedef typename super::iterator iterator;
  typedef typename super::const_iterator const_iterator;
  
  /// \brief Default constructor.
  vector()
   : super(),
     container_wrapper(*this, true)
  {}

  /// \brief Constructor.
  explicit vector (const allocator_type& alloc)
   : super::vector(alloc),
     container_wrapper(*this, true)     
  {}

  /// \brief Constructor.
  explicit vector (size_type n, const allocator_type& alloc = allocator_type())
   : super::vector(n, alloc),
     container_wrapper(*this, true)     
  {}

  vector (size_type n, const value_type& val, const allocator_type& alloc = allocator_type())
   : super::vector(n, detail::reference_aterm(val), alloc),
     container_wrapper(*this, true)
  {}

  /// \brief Constructor.
  template <class InputIterator>
  vector (InputIterator first, InputIterator last, const allocator_type& alloc = allocator_type())
   : super::vector(first, last, alloc),
     container_wrapper(*this, true)     
  {}

  /// \brief Constructor.
  vector (const vector& x)
   : super::vector(x),
     container_wrapper(*this, true)     
  {}

  /// \brief Constructor.
  vector (const vector& x, const allocator_type& alloc)
   : super::vector(x, alloc),
     container_wrapper(*this, true)     
  {}
  
  /// \brief Constructor.
  vector (vector&& x)
   : super::vector(std::move(x)),
     container_wrapper(*this, true)     
  {}

  /// \brief Constructor.
  vector (vector&& x, const allocator_type& alloc)
   : super::vector(std::move(x), alloc),
     container_wrapper(*this, true)
  {}

  /// \brief Constructor. To be done later....
  vector (std::initializer_list<value_type> il, const allocator_type& alloc = allocator_type())
    : super::vector(il, alloc),
      container_wrapper(*this, true)      
  {}

  /// \brief Assignment operator.
  vector& operator= (const vector& x) = default;
  
  /// \brief Move assignment operator
  vector& operator=(vector&& x) = default;

  /// \brief Standard destructor.
  ~vector()
  {}
  
  void shrink_to_fit()
  {
    detail::shared_guard _;
    super::shrink_to_fit();
  }

  void clear() noexcept
  {
    detail::shared_guard _;
    super::clear();
  }

  iterator insert( const_iterator pos, const T& value )
  {
    detail::shared_guard _;
    return super::insert(pos, value);
  }

  iterator insert( const_iterator pos, T&& value )
  {
    detail::shared_guard _;
    return super::insert(pos, value);
  }
  
  iterator insert( const_iterator pos, size_type count, const T& value )
  {
    detail::shared_guard _;
    return super::insert(pos, count, value);
  }
    
  template< class InputIt >
  iterator insert( const_iterator pos,
                  InputIt first, InputIt last )
  {
    detail::shared_guard _;
    return super::insert(pos, first, last);  
  }
    
  iterator insert( const_iterator pos, std::initializer_list<T> ilist )
  {
    detail::shared_guard _;
    return super::insert(pos, ilist);
  }
  
  template< class... Args >
  iterator emplace( const_iterator pos, Args&&... args )
  {
    detail::shared_guard _;
    return super::emplace(pos, args...);   
  }

  iterator erase( const_iterator pos )
  {
    detail::shared_guard _;
    return super::erase(pos);
  }

  iterator erase( const_iterator first, const_iterator last )
  {
    detail::shared_guard _;
    return super::erase(first, last);
  }

  void push_back( const T& value )
  {
    detail::shared_guard _;
    super::push_back(value);
  }

  void push_back( T&& value )
  {
    detail::shared_guard _;
    super::push_back(value);
  }

  template< class... Args >
  reference emplace_back( Args&&... args )
  {
    detail::shared_guard _;
    return super::emplace_back(args...);  
  }

  void pop_back()
  {
    detail::shared_guard _;
    super::pop_back();
  }

  void resize( size_type count )
  {
    detail::shared_guard _;
    super::resize(count);
  }

  void resize( size_type count, const value_type& value )
  {
    detail::shared_guard _;
    super::resize(count, value);
  }

  void swap( vector& other ) noexcept
  {
    detail::shared_guard _;
    super::swap(other); // Invalidates end() so must be protected.
  }

  std::size_t size() const override
  {
    return super::size();
  }
};

} // namespace atermpp
#endif // MCRL2_ATERMPP_STANDARD_CONTAINER_VECTOR_H
