// ======================================================================
//
// Copyright (c) 2004, 2005, 2006 Wieger Wesselink
//
// ----------------------------------------------------------------------
//
// file          : atermpp/algorithm.h
// date          : 19-09-2006
// version       : 1.0
//
// author(s)     : Wieger Wesselink  <J.W.Wesselink@tue.nl>
//
// ======================================================================

///////////////////////////////////////////////////////////////////////////////
/// \file algorithm.h
/// Contains algorithms on aterm_lists and aterm_appls.

#ifndef ATERM_ALGORITHM_H
#define ATERM_ALGORITHM_H

#include "atermpp/aterm.h"
#include "atermpp/aterm_appl.h"
#include "atermpp/aterm_list.h"
#include "atermpp/detail/algorithm_impl.h"

namespace atermpp
{
  /// Finds a subterm of t that matches the predicate op. If no matching subterm is found,
  /// aterm() is returned.
  template <typename Term, typename UnaryPredicate>
  aterm find_if(Term t, UnaryPredicate op)
  {
    try {
      detail::find_if_impl(aterm_traits<Term>::term(t), op);
    }
    catch (detail::found_term_exception e) {
      return e.t;
    }
    return aterm();
  }

  /// Finds all subterms of t that match the predicate op, and writes the found terms
  /// to the destination range starting with destBegin.
  template <typename Term, typename UnaryPredicate, typename OutputIterator>
  void find_all_if(Term t, UnaryPredicate op, OutputIterator destBegin)
  {
    OutputIterator i = destBegin; // we make a copy, since a reference to an iterator is needed
    detail::find_all_if_impl(aterm_traits<Term>::term(t), op, i);
  }

  /// Replaces each subterm in t that is equal to old_value with new_value.
  /// The replacements are performed in top down order. For example,
  /// replace(f(f(x)), f(x), x) returns f(x) and not x.
  template <typename Term>
  Term replace(Term t, aterm_appl old_value, aterm_appl new_value)
  {
    return replace(t, detail::default_replace(old_value, new_value));
  }

  /// Replaces each subterm x of t by r(x). The ReplaceFunction r has
  /// the following signature:
  ///
  /// aterm_appl x;
  /// aterm_appl result = r(x);
  ///
  /// The replacements are performed in top down order.
  template <typename Term, typename ReplaceFunction>
  Term replace(Term t, ReplaceFunction r)
  {
    ATerm x = detail::replace_impl(aterm_traits<Term>::term(t), r);
    return Term(reinterpret_cast<ATermAppl>(x));   
  }

  /// Replaces each subterm x of t by r(x). The ReplaceFunction r has
  /// the following signature:
  ///
  /// aterm_appl x;
  /// aterm_appl result = r(x);
  ///
  /// The replacements are performed in bottom up order. For example,
  /// replace(f(f(x)), f(x), x) returns x.
  template <typename Term, typename ReplaceFunction>
  Term bottom_up_replace(Term t, ReplaceFunction r)
  {
    ATerm x = detail::bottom_up_replace_impl(aterm_traits<Term>::term(t), r);
    return Term(reinterpret_cast<ATermAppl>(x));   
  }

  /// Replaces each subterm in t that is equal to old_value with new_value.
  /// The replacements are performed in top down order. For example,
  /// replace(f(f(x)), f(x), x) returns f(x) and not x.
  template <typename Term>
  Term bottom_up_replace(Term t, aterm_appl old_value, aterm_appl new_value)
  {
    return bottom_up_replace(t, detail::default_replace(old_value, new_value));
  }

  /// Replaces subterms x of t by r(x). The replace function r returns an
  /// additional boolean value. This value is used to prevent further recursion.
  /// The ReplaceFunction r has the following signature:
  ///
  /// aterm_appl x;
  /// std::pair<aterm_appl, bool> result = r(x);
  ///
  /// result.first  is the result r(x) of the replacement
  /// result.second denotes if the recursion should be continued
  ///
  /// The replacements are performed in top down order.
  template <typename Term, typename ReplaceFunction>
  Term partial_replace(Term t, ReplaceFunction r)
  {
    ATerm x = detail::partial_replace_impl(aterm_traits<Term>::term(t), r);
    return Term(reinterpret_cast<ATermAppl>(x));   
  }

} // namespace atermpp

#endif // ATERM_ALGORITHM_H
