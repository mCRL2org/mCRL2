// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
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

/// \brief Protected set container.
template<class T, class Compare = std::less<T>, class Allocator = std::allocator<T> >
class set: public std::set<T, Compare, Allocator>, IProtectedATerm
{
  public:
    /// \brief Constructor.
    set()
    {
      ATprotectProtectedATerm(this);
    }

    /// \brief Constructor.
    /// \param comp A compare function.
    explicit set(const Compare& comp)
      : std::set<T, Compare, Allocator>(comp)
    {
      ATprotectProtectedATerm(this);
    }

    /// \brief Constructor.
    /// \param comp A compare function.
    /// \param a An allocator.
    set(const Compare& comp, const Allocator& a)
      : std::set<T, Compare, Allocator>(comp, a)
    {
      ATprotectProtectedATerm(this);
    }

    /// \brief Constructor.
    /// \param right A set.
    set(const set& right)
      : std::set<T, Compare, Allocator>(right)
    {
      ATprotectProtectedATerm(this);
    }

    /// \brief Constructor.
    /// \param first The start of a range of set elements.
    /// \param last The end of a range of set elements.
    template<class InIt>
    set(InIt first, InIt last)
      : std::set<T, Compare, Allocator>(first, last)
    {
      ATprotectProtectedATerm(this);
    }

    /// \brief Constructor.
    /// \param first The start of a range of set elements.
    /// \param last The end of a range of set elements.
    /// \param comp A compare function.
    template<class InIt>
    set(InIt first, InIt last, const Compare& comp)
      : std::set<T, Compare, Allocator>(first, last, comp)
    {
      ATprotectProtectedATerm(this);
    }

    /// \brief Constructor.
    /// \param first The start of a range of set elements.
    /// \param last The end of a range of set elements.
    /// \param comp A compare function.
    /// \param a An allocator.
    template<class InIt>
    set(InIt first, InIt last, const Compare& comp, const Allocator& a)
      : std::set<T, Compare, Allocator>(first, last, comp, a)
    {
      ATprotectProtectedATerm(this);
    }
   
    /// Destructor.
    ~set()
    {
      ATunprotectProtectedATerm(this);
    }

    /// Assignment operator.
    /// \param other A set.
    atermpp::set<T, Compare, Allocator>& operator=(const std::set<T, Compare, Allocator>& other)
    {
      std::set<T, Compare, Allocator>::operator=(other);
      return *this;
    }

    /// \brief Protects the elements from being garbage collected.
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

/// \brief Protected multiset container.
template<class T, class Compare = std::less<T>, class Allocator = std::allocator<T> >
class multiset: public std::multiset<T, Compare, Allocator>, IProtectedATerm
{
  public:
    /// \brief Constructor.
    multiset()
    {
      ATprotectProtectedATerm(this);
    }

    /// \brief Constructor.
    /// \param comp A compare function.
    explicit multiset(const Compare& comp)
      : std::multiset<T, Compare, Allocator>(comp)
    {
      ATprotectProtectedATerm(this);
    }

    /// \brief Constructor.
    /// \param comp A compare function.
    /// \param a An allocator.
    multiset(const Compare& comp, const Allocator& a)
      : std::multiset<T, Compare, Allocator>(comp, a)
    {
      ATprotectProtectedATerm(this);
    }

    /// \brief Constructor.
    /// \param right A multiset.
    multiset(const multiset& right)
      : std::multiset<T, Compare, Allocator>(right)
    {
      ATprotectProtectedATerm(this);
    }

    /// \brief Constructor.
    /// \param first The start of a range of multiset elements.
    /// \param last The end of a range of multiset elements.
    template<class InIt>
    multiset(InIt first, InIt last)
      : std::multiset<T, Compare, Allocator>(first, last)
    {
      ATprotectProtectedATerm(this);
    }

    /// \brief Constructor.
    /// \param first The start of a range of multiset elements.
    /// \param last The end of a range of multiset elements.
    /// \param comp A compare function.
    template<class InIt>
    multiset(InIt first, InIt last, const Compare& comp)
      : std::multiset<T, Compare, Allocator>(first, last, comp)
    {
      ATprotectProtectedATerm(this);
    }

    /// \brief Constructor.
    /// \param first The start of a range of multiset elements.
    /// \param last The end of a range of multiset elements.
    /// \param comp A compare function.
    /// \param a An allocator.
    template<class InIt>
    multiset(InIt first, InIt last, const Compare& comp, const Allocator& a)
      : std::multiset<T, Compare, Allocator>(first, last, comp, a)
    {
      ATprotectProtectedATerm(this);
    }
   
    /// Destructor.
    ~multiset()
    {
      ATunprotectProtectedATerm(this);
    }

    /// \brief Protects the elements from being garbage collected.
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
