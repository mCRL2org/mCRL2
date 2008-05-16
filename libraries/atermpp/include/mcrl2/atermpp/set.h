// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/set.h
/// \brief Protected set and multiset containers.

#ifndef MCRL2_ATERMPP_SET_H
#define MCRL2_ATERMPP_SET_H

#include <functional>
#include <memory>
#include <set>
#include "mcrl2/atermpp/aterm_traits.h"
#include "protaterm.h"          // IProtectedATerm

namespace atermpp {

/// Protected set container.
///
template<class T, class Compare = std::less<T>, class Allocator = std::allocator<T> >
class set: public std::set<T, Compare, Allocator>, IProtectedATerm
{
  public:
    /// Constructor.
    ///
    set()
    {
      ATprotectProtectedATerm(this);
    }

    /// Constructor.
    ///
    explicit set(const Compare& comp)
      : std::set<T, Compare, Allocator>(comp)
    {
      ATprotectProtectedATerm(this);
    }

    /// Constructor.
    ///
    set(const Compare& comp, const Allocator& a)
      : std::set<T, Compare, Allocator>(comp, a)
    {
      ATprotectProtectedATerm(this);
    }

    /// Constructor.
    ///
    set(const set& right)
      : std::set<T, Compare, Allocator>(right)
    {
      ATprotectProtectedATerm(this);
    }

    /// Constructor.
    ///
    template<class InIt>
    set(InIt first, InIt last)
      : std::set<T, Compare, Allocator>(first, last)
    {
      ATprotectProtectedATerm(this);
    }

    /// Constructor.
    ///
    template<class InIt>
    set(InIt first, InIt last, const Compare& comp)
      : std::set<T, Compare, Allocator>(first, last, comp)
    {
      ATprotectProtectedATerm(this);
    }

    /// Constructor.
    ///
    template<class InIt>
    set(InIt first, InIt last, const Compare& comp, const Allocator& a)
      : std::set<T, Compare, Allocator>(first, last, comp, a)
    {
      ATprotectProtectedATerm(this);
    }
   
    /// Destructor.
    ///
    ~set()
    {
      ATunprotectProtectedATerm(this);
    }

    /// Assignment operator.
    ///
    atermpp::set<T, Compare, Allocator>& operator=(const std::set<T, Compare, Allocator>& other)
    {
      std::set<T, Compare, Allocator>::operator=(other);
      return *this;
    }

    /// Protects the elements from being garbage collected.
    ///
    void ATprotectTerms()
    {
#ifdef ATERM_DEBUG_PROTECTION
std::cout << "atermpp::set.ATprotectTerms() : protecting " << set<T>::size() << " elements" << std::endl;
#endif // ATERM_DEBUG_PROTECTION
      for (typename std::set<T, Compare, Allocator>::iterator i = std::set<T, Compare, Allocator>::begin(); i != std::set<T, Compare, Allocator>::end(); ++i)
      {
        aterm_traits<T>::mark(*i);
      }
    }
};

/// Protected multiset container.
///
template<class T, class Compare = std::less<T>, class Allocator = std::allocator<T> >
class multiset: public std::multiset<T, Compare, Allocator>, IProtectedATerm
{
  public:
    /// Constructor.
    ///
    multiset()
    {
      ATprotectProtectedATerm(this);
    }

    /// Constructor.
    ///
    explicit multiset(const Compare& comp)
      : std::multiset<T, Compare, Allocator>(comp)
    {
      ATprotectProtectedATerm(this);
    }

    /// Constructor.
    ///
    multiset(const Compare& comp, const Allocator& a)
      : std::multiset<T, Compare, Allocator>(comp, a)
    {
      ATprotectProtectedATerm(this);
    }

    /// Constructor.
    ///
    multiset(const multiset& right)
      : std::multiset<T, Compare, Allocator>(right)
    {
      ATprotectProtectedATerm(this);
    }

    /// Constructor.
    ///
    template<class InIt>
    multiset(InIt first, InIt last)
      : std::multiset<T, Compare, Allocator>(first, last)
    {
      ATprotectProtectedATerm(this);
    }

    /// Constructor.
    ///
    template<class InIt>
    multiset(InIt first, InIt last, const Compare& comp)
      : std::multiset<T, Compare, Allocator>(first, last, comp)
    {
      ATprotectProtectedATerm(this);
    }

    /// Constructor.
    ///
    template<class InIt>
    multiset(InIt first, InIt last, const Compare& comp, const Allocator& a)
      : std::multiset<T, Compare, Allocator>(first, last, comp, a)
    {
      ATprotectProtectedATerm(this);
    }
   
    /// Destructor.
    ///
    ~multiset()
    {
      ATunprotectProtectedATerm(this);
    }

    /// Protects the elements from being garbage collected.
    ///
    void ATprotectTerms()
    {
#ifdef ATERM_DEBUG_PROTECTION
std::cout << "atermpp::multiset.ATprotectTerms() : protecting " << multiset<T>::size() << " elements" << std::endl;
#endif // ATERM_DEBUG_PROTECTION
      for (typename std::multiset<T, Compare, Allocator>::iterator i = std::multiset<T, Compare, Allocator>::begin(); i != std::multiset<T, Compare, Allocator>::end(); ++i)
      {
        aterm_traits<T>::mark(*i);
      }
    }
};

} // namespace atermpp

#endif // MCRL2_ATERMPP_SET_H
