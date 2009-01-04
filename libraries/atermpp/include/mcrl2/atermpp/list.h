// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/list.h
/// \brief Protected list container.

#ifndef MCRL2_ATERMPP_LIST_H
#define MCRL2_ATERMPP_LIST_H

#include <memory>
#include <list>
#include "mcrl2/atermpp/aterm_traits.h"
#include "protaterm.h"          // IProtectedATerm

namespace atermpp {

/// \brief Protected list container.
template <class T, class Allocator = std::allocator<T> >
class list: public std::list<T, Allocator>, IProtectedATerm
{
  public:
    /// \brief Constructor.
    list()
    {
      ATprotectProtectedATerm(this);
    }

    /// \brief Constructor.
    /// \param a An allocator.
    explicit list(const Allocator& a)
      : std::list<T, Allocator>(a)
    {
      ATprotectProtectedATerm(this);
    }

    /// \brief Constructor.
    /// \param count A positive number.
    /// \param std::list<T PARAM_DESCRIPTION
    explicit list(typename std::list<T, Allocator>::size_type count)
      : std::list<T, Allocator>(count)
    {
      ATprotectProtectedATerm(this);
    }
    
    /// \brief Constructor.
    /// \param count A positive number.
    /// \param val A value.
    /// \param std::list<T PARAM_DESCRIPTION
    list(typename std::list<T, Allocator>::size_type count, const T& val)
      : std::list<T, Allocator>(count, val)
    {
      ATprotectProtectedATerm(this);
    }

    /// \brief Constructor.
    /// \param count A positive number.
    /// \param val A value.
    /// \param a An allocator.
    /// \param std::list<T PARAM_DESCRIPTION
    list(typename std::list<T, Allocator>::size_type count, const T& val, const Allocator& a)
      : std::list<T, Allocator>(count, val, a)
    {
      ATprotectProtectedATerm(this);
    }

    /// \brief Constructor.
    /// \param right A list.
    list(const list& right)
      : std::list<T, Allocator>(right)
    {
      ATprotectProtectedATerm(this);
    }

        /// \brief Constructor.
        /// \param first The start of a range of elements.
        /// \param last The end of a range of elements.
    template<class InIt>
        list(InIt first, InIt last)
      : std::list<T, Allocator>(first, last)
    {
      ATprotectProtectedATerm(this);
    }

        /// \brief Constructor.
        /// \param first The start of a range of elements.
        /// \param last The end of a range of elements.
        /// \param a An allocator.
    template<class InIt>
        list(InIt first, InIt last, const Allocator& a)
      : std::list<T, Allocator>(first, last, a)
    {
      ATprotectProtectedATerm(this);
    }

    /// Destructor.
    ~list()
    {
      ATunprotectProtectedATerm(this);
    }

    /// \brief Protects the elements from being garbage collected.
    void ATprotectTerms()
    {
#ifdef ATERM_DEBUG_PROTECTION
std::cout << "atermpp::list.ATprotectTerms() : protecting " << list<T>::size() << " elements" << std::endl;
#endif // ATERM_DEBUG_PROTECTION
      for (typename std::list<T, Allocator>::iterator i = std::list<T, Allocator>::begin(); i != std::list<T, Allocator>::end(); ++i)
      {
        aterm_traits<T>::mark(*i);
      }
    }
};

} // namespace atermpp

#endif // MCRL2_ATERMPP_LIST_H
