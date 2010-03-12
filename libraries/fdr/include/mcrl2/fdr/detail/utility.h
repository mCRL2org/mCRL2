// Author(s): Generated code
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/detail/utility.h
/// \brief Utility functions for term classes.

#ifndef MCRL2_FDR_DETAIL_UTILITY_H
#define MCRL2_FDR_DETAIL_UTILITY_H

#include <aterm2.h>
#include <cassert>

namespace mcrl2 {

namespace core {

namespace detail {

//Ret: quoted constant s, if s != NULL
//     unquoted constant Nil, if s == NULL
inline
ATermAppl gsString2ATermAppl(const char *s)
{
  if (s != NULL)
  {
    return ATmakeAppl0(ATmakeAFun(s, 0, ATtrue));
  }
  else
  {
    return gsMakeNil();
  }
}

// checks
// 1) if term t satisfies the predicate f
template <typename Term, typename CheckFunction>
bool check_term_argument(Term t, CheckFunction f)
{
  return f(t);
}

// checks
// 1) if term t is a list,
// 2) if the list has the proper minimum size
// 3) if all elements of the list satisfy the predicate f
template <typename Term, typename CheckFunction>
bool check_list_argument(Term t, CheckFunction f, unsigned int minimum_size)
{
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (t.type() != AT_LIST)
    return false;
  atermpp::aterm_list l(term);
  if (l.size() < minimum_size)
    return false;
  for (atermpp::aterm_list::iterator i = l.begin(); i != l.end(); ++i)
  {
    if (!f(*i))
      return false;
  }
  return true;
}

template <typename Term>
bool check_rule_String(Term t)
{
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (a.size() > 0)
    return false;
  return true;
}

template <typename Term>
bool check_rule_NumberString(Term t)
{
  atermpp::aterm term(atermpp::aterm_traits<Term>::term(t));
  if (term.type() != AT_APPL)
    return false;
  atermpp::aterm_appl a(term);
  if (a.size() > 0)
    return false;
  return true;
}

// String
inline
ATermAppl initConstructString(ATermAppl& t)
{
  t = gsString2ATermAppl("@NoValue");
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructString()
{
  static ATermAppl t = initConstructString(t);
  return t;
}

// List
inline
ATermList constructList()
{
  return ATmakeList0();
}

} // namespace detail

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_DETAIL_UTILITY_H
