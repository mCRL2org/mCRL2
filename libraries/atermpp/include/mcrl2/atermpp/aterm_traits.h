// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/aterm_traits.h
/// \brief Traits class for terms.

#ifndef MCRL2_ATERMPP_ATERM_TRAITS_H
#define MCRL2_ATERMPP_ATERM_TRAITS_H

#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_base_of.hpp>

#include <utility>
#include "mcrl2/aterm/aterm2.h"

// XXX Remove
using namespace aterm;

namespace atermpp
{

class aterm;

template < typename T >
struct non_aterm_traits
{
  /// \brief Protects the term t from garbage collection.
  /// \param t A term
  static void protect(const T*)       {}

  /// \brief Unprotects the term t from garbage collection.
  /// \param t A term
  static void unprotect(const T*)     {}

  /// \brief Marks t for garbage collection.
  /// \param t A term
  static void mark(T)           {}

  /// \brief Returns the ATerm that corresponds to the term t.
  /// \param t A term
  /// \return The ATerm that corresponds to the term t.
  static T term(const T& t)
  {
    return t;
  }
};

template < typename T, typename C = void >
struct select_traits_base
{
  typedef non_aterm_traits< T > base_type;
};

/// \brief Traits class for terms. It is used to specify how the term interacts
/// with the garbage collector, and how it can be converted to an ATerm.
template <typename T>
struct aterm_traits: public select_traits_base< T >::base_type
  {};

/// \cond INTERNAL_DOCS
template <>
struct aterm_traits<ATerm>
{
  static void protect(const ATerm* /* t */)
  {
    // ATprotect(t);
  }
  static void unprotect(const ATerm* /* t */)
  {
    // ATunprotect(t);
  }
  static void mark(const ATerm /* t */)
  {
    // ATmarkTerm(t);
  }
  static ATerm term(ATerm t)
  {
    return t;
  }
};

template <>
struct aterm_traits<ATermList>
{
  static void protect(const ATermList* t)
  {
    aterm_traits<ATerm>::protect(reinterpret_cast<const ATerm*>(t));
  }
  static void unprotect(const ATermList* t)
  {
    aterm_traits<ATerm>::unprotect(reinterpret_cast<const ATerm*>(t));
  }
  static void mark(ATermList t)
  {
    aterm_traits<ATerm>::mark(t);
  }
  static ATerm term(ATermList t)
  {
    return t;
  }
};

template <>
struct aterm_traits<ATermAppl>
{
  static void protect(const ATermAppl* t)
  {
    aterm_traits<ATerm>::protect(reinterpret_cast<const ATerm*>(t));
  }
  static void unprotect(const ATermAppl* t)
  {
    aterm_traits<ATerm>::unprotect(reinterpret_cast<const ATerm*>(t));
  }
  static void mark(ATermAppl t)
  {
    aterm_traits<ATerm>::mark(t);
  }
  static ATerm term(ATermAppl t)
  {
    return t;
  }
};

template <>
struct aterm_traits<ATermInt>
{
  static void protect(const ATermInt* t)
  {
    aterm_traits<ATerm>::protect(reinterpret_cast<const ATerm*>(t));
  }
  static void unprotect(const ATermInt* t)
  {
    aterm_traits<ATerm>::unprotect(reinterpret_cast<const ATerm*>(t));
  }
  static void mark(ATermInt t)
  {
    aterm_traits<ATerm>::mark(t);
  }
  static ATerm term(ATermInt t)
  {
    return t;
  }
};

template<typename T1, typename T2>
struct aterm_traits<std::pair<T1, T2> >
{
  static void protect(const std::pair<T1, T2>& t)
  {
    aterm_traits<T1>::protect(t.first);
    aterm_traits<T2>::protect(t.second);
  }
  static void unprotect(const std::pair<T1, T2>& t)
  {
    aterm_traits<T1>::unprotect(t.first);
    aterm_traits<T2>::unprotect(t.second);
  }
  static void mark(const std::pair<T1, T2>& t)
  {
    aterm_traits<T1>::mark(t.first);
    aterm_traits<T2>::mark(t.second);
  }
};
/// \endcond

} // namespace atermpp

#endif // MCRL2_ATERMPP_ATERM_TRAITS_H
