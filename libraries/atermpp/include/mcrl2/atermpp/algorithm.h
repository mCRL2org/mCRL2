// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/algorithm.h
/// \brief Algorithms for ATerms.

#ifndef MCRL2_ATERMPP_ALGORITHM_H
#define MCRL2_ATERMPP_ALGORITHM_H

#include "boost/type_traits/add_reference.hpp"

#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/detail/algorithm_impl.h"

namespace atermpp
{

/// \brief Finds a subterm of t that matches a given predicate.
/// \param t A term
/// \param match The predicate that determines if a subterm is a match
/// \return A subterm that matches the given predicate, or aterm_appl() if none was found.
template <typename Term, typename MatchPredicate>
aterm_appl find_if(Term t, MatchPredicate match)
{
  aterm_appl output;
  detail::find_if_impl< typename boost::add_reference< MatchPredicate >::type >(t, match, output);
  return output;
}

/// \brief Finds all subterms of t that match a given predicate, and writes the found terms
/// to the destination range starting with destBegin.
/// \param t A term
/// \param match The predicate that determines if a subterm is a match
/// \param destBegin The iterator range to which output is written.
template <typename Term, typename MatchPredicate, typename OutputIterator>
void find_all_if(Term t, MatchPredicate match, OutputIterator destBegin)
{
  OutputIterator i = destBegin; // we make a copy, since a reference to an iterator is needed
  detail::find_all_if_impl< typename boost::add_reference< MatchPredicate >::type >(aterm_traits<Term>::term(t), match, i);
}

/// \brief Replaces each subterm x of t by r(x). The ReplaceFunction r has
/// the following signature:
/// aterm_appl x;
/// aterm_appl result = r(x);
/// The replacements are performed in top down order.
/// \param t A term
/// \param r The replace function that is applied to subterms.
/// \return The result of the replacement.
template <typename Term, typename ReplaceFunction>
Term replace(Term t, ReplaceFunction r)
{
  ATerm x = detail::replace_impl< typename boost::add_reference< ReplaceFunction >::type >(aterm_traits<Term>::term(t), r);
  return Term(reinterpret_cast<ATermAppl>(x));
}

/// \brief Replaces each subterm in t that is equal to old_value with new_value.
/// The replacements are performed in top down order. For example,
/// replace(f(f(x)), f(x), x) returns f(x) and not x.
/// \param t A term
/// \param old_value The subterm that will be replaced.
/// \param new_value The value that will be substituted.
/// \return The result of the replacement.
template <typename Term>
Term replace(Term t, aterm_appl old_value, aterm_appl new_value)
{
  return replace(t, detail::default_replace(old_value, new_value));
}

} // namespace atermpp

#endif // MCRL2_ATERMPP_ALGORITHM_H
