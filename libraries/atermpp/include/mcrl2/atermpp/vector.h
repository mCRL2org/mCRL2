// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/vector.h
/// \brief Protected vector container.

#ifndef MCRL2_ATERMPP_VECTOR_H
#define MCRL2_ATERMPP_VECTOR_H

#include <memory>
#include <vector>
#include "mcrl2/atermpp/aterm_traits.h"
#include "protaterm.h"          // IProtectedATerm

namespace atermpp {

/// \brief Protected vector container.
template <class T, class Allocator = std::allocator<T> >
class vector: public std::vector<T, Allocator>, IProtectedATerm
{
  public:
    /// \brief Constructor.
    vector()
    {
      ATprotectProtectedATerm(this);
    }

    /// \brief Constructor.
    /// \param a An allocator.
    explicit vector(const Allocator& a)
      : std::vector<T, Allocator>(a)
    {
      ATprotectProtectedATerm(this);
    }

    /// \brief Constructor.
    /// \param count A positive number.
    /// \param std::vector<T PARAM_DESCRIPTION
    explicit vector(typename std::vector<T, Allocator>::size_type count)
      : std::vector<T, Allocator>(count)
    {
      ATprotectProtectedATerm(this);
    }
    
    /// \brief Constructor.
    /// \param count A positive number.
    /// \param val A value.
    /// \param std::vector<T PARAM_DESCRIPTION
    vector(typename std::vector<T, Allocator>::size_type count, const T& val)
      : std::vector<T, Allocator>(count, val)
    {
      ATprotectProtectedATerm(this);
    }

    /// \brief Constructor.
    /// \param count A positive number.
    /// \param val A value.
    /// \param a An allocator.
    /// \param std::vector<T PARAM_DESCRIPTION
    vector(typename std::vector<T, Allocator>::size_type count, const T& val, const Allocator& a)
      : std::vector<T, Allocator>(count, val, a)
    {
      ATprotectProtectedATerm(this);
    }

    /// \brief Constructor.
    /// \param right A vector.
    vector(const vector& right)
      : std::vector<T, Allocator>(right)
    {
      ATprotectProtectedATerm(this);
    }

        /// \brief Constructor.
        /// \param first The start of a range of elements.
        /// \param last The end of a range of elements.
    template<class InIt>
        vector(InIt first, InIt last)
      : std::vector<T, Allocator>(first, last)
    {
      ATprotectProtectedATerm(this);
    }

        /// \brief Constructor.
        /// \param first The start of a range of elements.
        /// \param last The end of a range of elements.
        /// \param a An allocator.
    template<class InIt>
        vector(InIt first, InIt last, const Allocator& a)
      : std::vector<T, Allocator>(first, last, a)
    {
      ATprotectProtectedATerm(this);
    }

    /// Destructor.
    ~vector()
    {
      ATunprotectProtectedATerm(this);
    }

    /// \brief Protects the elements from being garbage collected.
    void ATprotectTerms()
    {
#ifdef ATERM_DEBUG_PROTECTION
std::cout << "atermpp::vector.ATprotectTerms() : protecting " << vector<T>::size() << " elements" << std::endl;
#endif // ATERM_DEBUG_PROTECTION
      for (typename std::vector<T, Allocator>::iterator i = std::vector<T, Allocator>::begin(); i != std::vector<T, Allocator>::end(); ++i)
      {
        aterm_traits<T>::mark(*i);
      }
    }
};

} // namespace atermpp

#endif // MCRL2_ATERMPP_VECTOR_H
