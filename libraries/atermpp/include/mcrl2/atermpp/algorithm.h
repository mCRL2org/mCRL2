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

#include <type_traits>

#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/detail/algorithm_impl.h"

namespace atermpp
{
/// \brief Calls op(elem) for subterms of the term t.
/// \param t A term
/// \param op The operation that is applied to subterms
/// \return a copy of the (internally modified) op.
/// The function op must have the signature bool op(aterm_appl t).
/// When op(t) is false, the children of t are skipped.
template <typename UnaryFunction, typename Term>
UnaryFunction for_each(Term t, UnaryFunction op)
{
  return detail::for_each_impl< typename std::add_lvalue_reference< UnaryFunction >::type >(t, op);
}

/// \brief Finds a subterm of t that matches a given predicate.
/// \param t A term
/// \param match The predicate that determines if a subterm is a match
/// \return A subterm that matches the given predicate, or aterm_appl() if none was found.
template <typename Term, typename MatchPredicate>
aterm_appl find_if(const Term &t, MatchPredicate match)
{
  aterm_appl output;
  detail::find_if_impl< typename std::add_lvalue_reference< MatchPredicate >::type >(t, match, output);
  return output;
}

/// \brief Finds a subterm of t that matches a given predicate.
/// The term is only partially traversed. If the stop predicate
/// returns true in a subterm, the recursion is not continued.
/// \param t A term
/// \param match The predicate that determines if a subterm is a match
/// \param stop The predicate that determines if the recursion should not be continued in a subterm
/// \return A subterm that matches the given predicate, or aterm_appl() if none was found.
template <typename Term, typename MatchPredicate, typename StopPredicate>
aterm_appl partial_find_if(Term t, MatchPredicate match, StopPredicate stop)
{
  return detail::partial_find_if_impl<typename std::add_lvalue_reference<MatchPredicate>::type>(t, match, stop);
}

/// \brief Finds all subterms of t that match a given predicate, and writes the found terms
/// to the destination range starting with destBegin.
/// \param t A term
/// \param match The predicate that determines if a subterm is a match
/// \param destBegin The iterator range to which output is written.
template <typename Term, typename MatchPredicate, typename OutputIterator>
void find_all_if(const Term &t, MatchPredicate match, OutputIterator destBegin)
{
  OutputIterator i = destBegin; // we make a copy, since a reference to an iterator is needed
  detail::find_all_if_impl< typename std::add_lvalue_reference< MatchPredicate >::type >(t, match, i);
}

/// \brief Finds all subterms of t that match a given predicate, and writes the found terms
/// to the destination range starting with destBegin.
/// The term is only partially traversed. If the stop predicate
/// returns true in a subterm, the recursion is not continued.
/// \param t A term
/// \param match The predicate that determines if a subterm is a match
/// \param stop The predicate that determines if the recursion should not be continued in a subterm
/// \param destBegin The iterator range to which output is written.
template <typename Term, typename MatchPredicate, typename StopPredicate, typename OutputIterator>
void partial_find_all_if(Term t, MatchPredicate match, StopPredicate stop, OutputIterator destBegin)
{
  OutputIterator i = destBegin; // we make a copy, since a reference to an iterator is needed
  detail::partial_find_all_if_impl< typename std::add_lvalue_reference< MatchPredicate >::type,
         typename std::add_lvalue_reference< StopPredicate >::type >(t, match, stop, i);
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
Term replace(const Term &t, ReplaceFunction r)
{
  return vertical_cast<Term>(detail::replace_impl< typename std::add_lvalue_reference< ReplaceFunction >::type >(t, r));
}

/// \brief Replaces each subterm in t that is equal to old_value with new_value.
/// The replacements are performed in top down order. For example,
/// replace(f(f(x)), f(x), x) returns f(x) and not x.
/// \param t A term
/// \param old_value The subterm that will be replaced.
/// \param new_value The value that will be substituted.
/// \return The result of the replacement.
template <typename Term>
Term replace(const Term &t, const aterm &old_value, const aterm &new_value)
{
  return replace(t, detail::default_replace(old_value, new_value));
}

/// \brief Replaces each subterm x of t by r(x). The ReplaceFunction r has
/// the following signature:
/// aterm_appl x;
/// aterm_appl result = r(x);
/// The replacements are performed in bottom up order. For example,
/// replace(f(f(x)), f(x), x) returns x.
/// \param t A term
/// \param r The replace function that is applied to subterms.
/// \return The result of the replacement.
template <typename Term, typename ReplaceFunction>
Term bottom_up_replace(Term t, ReplaceFunction r)
{
  aterm x = detail::bottom_up_replace_impl< typename std::add_lvalue_reference< ReplaceFunction >::type >(t, r);
  return Term(down_cast<aterm_appl>(x));
}

/// \brief Replaces each subterm in t that is equal to old_value with new_value.
/// The replacements are performed in top down order. For example,
/// replace(f(f(x)), f(x), x) returns f(x) and not x.
/// \param t A term
/// \param old_value The value of the subterm that is replaced.
/// \param new_value The value that is substituted.
/// \return The result of the replacement.
template <typename Term>
Term bottom_up_replace(Term t, aterm_appl old_value, aterm_appl new_value)
{
  return bottom_up_replace(t, detail::default_replace(old_value, new_value));
}

/// \brief Replaces subterms x of t by r(x). The replace function r returns an
/// additional boolean value. This value is used to prevent further recursion.
/// The ReplaceFunction r has the following signature:
/// aterm_appl x;
/// std::pair<aterm_appl, bool> result = r(x);
/// result.first  is the result r(x) of the replacement
/// result.second denotes if the recursion should be continued
/// The replacements are performed in top down order.
/// \param t A term
/// \param r The replace function that is applied to subterms.
/// \return The result of the replacement.
template <typename Term, typename ReplaceFunction>
Term partial_replace(Term t, ReplaceFunction r)
{
  aterm x = detail::partial_replace_impl< typename std::add_lvalue_reference< ReplaceFunction >::type >(t, r);
  return Term(down_cast<aterm_appl>(x));
}

} // namespace atermpp

#endif // MCRL2_ATERMPP_ALGORITHM_H
