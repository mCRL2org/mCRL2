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
#include "mcrl2/aterm/protaterm.h"          // IProtectedATerm
#include "mcrl2/atermpp/aterm_traits.h"

namespace atermpp
{

/// \brief Protected list container.
template <class T, class Allocator = std::allocator<T> >
class list: public std::list<T,Allocator>, IProtectedATerm
{
  public:
    /// \brief Constructor.
    list()
      : IProtectedATerm()
    {
      protect_aterms(this);
    }

    /// \brief Constructor.
    /// \param a An allocator.
    explicit list(const Allocator& a)
      : std::list<T,Allocator>(a),
        IProtectedATerm()
    {
      protect_aterms(this);
    }

    /// \brief Constructor.
    /// \param count A positive number.
    explicit list(typename std::list<T,Allocator>::size_type count)
      : std::list<T,Allocator>(count),
        IProtectedATerm()
    {
      protect_aterms(this);
    }

    /// \brief Constructor.
    /// \param count A positive number.
    /// \param val A value.
    list(typename std::list<T,Allocator>::size_type count, const T& val)
      : std::list<T,Allocator>(count, val),
        IProtectedATerm()
    {
      protect_aterms(this);
    }

    /// \brief Constructor.
    /// \param count A positive number.
    /// \param val A value.
    /// \param a An allocator.
    list(typename std::list<T,Allocator>::size_type count, const T& val, const Allocator& a)
      : std::list<T,Allocator>(count, val, a),
        IProtectedATerm()
    {
      protect_aterms(this);
    }

    /// \brief Constructor.
    /// \param right A list.
    list(const list& right)
      : std::list<T,Allocator>(right),
        IProtectedATerm()
    {
      protect_aterms(this);
    }

    /// \brief Constructor.
    /// \param first The start of a range of elements.
    /// \param last The end of a range of elements.
    template<class InIt>
    list(InIt first, InIt last)
      : std::list<T,Allocator>(first, last),
        IProtectedATerm()
    {
      protect_aterms(this);
    }

    /// \brief Constructor.
    /// \param first The start of a range of elements.
    /// \param last The end of a range of elements.
    /// \param a An allocator.
    template<class InIt>
    list(InIt first, InIt last, const Allocator& a)
      : std::list<T,Allocator>(first, last, a),
        IProtectedATerm()
    {
      protect_aterms(this);
    }

    /// Destructor.
    ~list()
    {
    }

    /// \brief Protects the elements from being garbage collected.
    void ATmarkTerms()
    {
#ifdef ATERM_DEBUG_PROTECTION
      std::cout << "atermpp::list.ATmarkTerms() : protecting " << list<T>::size() << " elements" << std::endl;
#endif // ATERM_DEBUG_PROTECTION
      for (typename std::list<T,Allocator>::iterator i = std::list<T,Allocator>::begin(); i != std::list<T,Allocator>::end(); ++i)
      {
        aterm_traits<T>::mark(*i);
      }
    }
};

} // namespace atermpp

#endif // MCRL2_ATERMPP_LIST_H
