// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/algorithm.h
/// \brief Algorithms for ATerms.

#ifndef MCRL2_ATERMPP_ALGORITHM_H
#define MCRL2_ATERMPP_ALGORITHM_H

#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/detail/algorithm_impl.h"

namespace atermpp
{
  /// \brief Calls op(elem) for subterms of the term t.
  /// \return a copy of the (internally modified) op.
  /// The function op must have the signature bool op(aterm_appl t).
  /// When op(t) is false, the children of t are skipped.
  template <typename UnaryFunction, typename Term>
  UnaryFunction for_each(Term t, UnaryFunction op)
  {
    return detail::for_each_impl(aterm_traits<Term>::term(t), op);
  }

  /// Finds a subterm of t that matches a given predicate. If no matching subterm is found,
  /// aterm_appl() is returned.
  template <typename Term, typename MatchPredicate>
  aterm_appl find_if(Term t, MatchPredicate match)
  {
    try {
      detail::find_if_impl(aterm_traits<Term>::term(t), match);
    }
    catch (detail::found_term_exception e) {
      return e.t;
    }
    return aterm_appl();
  }

  /// Finds a subterm of t that matches a given predicate. If no matching subterm is found,
  /// aterm_appl() is returned. The term is only partially traversed. If the stop predicate
  /// returns true in a subterm, the recursion is not continued.
  /// \param match The predicate that determines if a subterm is a match
  /// \param stop The predicate that determines if the recursion should not be continued in a subterm
  template <typename Term, typename MatchPredicate, typename StopPredicate>
  aterm_appl partial_find_if(Term t, MatchPredicate match, StopPredicate stop)
  {
    try {
      detail::partial_find_if_impl(aterm_traits<Term>::term(t), match, stop);
    }
    catch (detail::found_term_exception e) {
      return e.t;
    }
    return aterm_appl();
  }

  /// Finds all subterms of t that match a given predicate, and writes the found terms
  /// to the destination range starting with destBegin.
  template <typename Term, typename MatchPredicate, typename OutputIterator>
  void find_all_if(Term t, MatchPredicate match, OutputIterator destBegin)
  {
    OutputIterator i = destBegin; // we make a copy, since a reference to an iterator is needed
    detail::find_all_if_impl(aterm_traits<Term>::term(t), match, i);
  }

  /// Finds all subterms of t that match a given predicate, and writes the found terms
  /// to the destination range starting with destBegin.
  /// The term is only partially traversed. If the stop predicate
  /// returns true in a subterm, the recursion is not continued.
  /// \param match The predicate that determines if a subterm is a match
  /// \param stop The predicate that determines if the recursion should not be continued in a subterm
  template <typename Term, typename MatchPredicate, typename StopPredicate, typename OutputIterator>
  void partial_find_all_if(Term t, MatchPredicate match, StopPredicate stop, OutputIterator destBegin)
  {
    OutputIterator i = destBegin; // we make a copy, since a reference to an iterator is needed
    detail::partial_find_all_if_impl(aterm_traits<Term>::term(t), match, stop, i);
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

/*
  /// \cond INTERNAL_DOCS
  template <typename ReplaceFunction, typename CheckFunction>
  struct checked_replace_helper
  {
    const CheckFunction& f_;
    const ReplaceFunction& r_;
    
    checked_replace_helper(const CheckFunction& f, const ReplaceFunction& r)
      : f_(f), r_(r)
    {}
    
    std::pair<aterm_appl, bool> operator()(aterm_appl t) const
    {
      if (f_(t))
      {
        return std::pair<aterm_appl, bool>(r_(t), false); // do not continue the recursion
      }
      else
      {
        return std::pair<aterm_appl, bool>(t, true); // continue the recursion
      }
    }
  };
  /// \endcond

  /// Replaces subterms in the term t. Each subterm for which f(s) returns true
  /// is replaced by r(s), and the recursion is not continued.
  ///
  /// The CheckFunction f and the ReplaceFunction r have the following signature:
  ///
  /// aterm_appl x;
  /// bool b = f(x);
  /// std::pair<aterm_appl, bool> result = r(x);
  template <typename Term, typename ReplaceFunction, typename CheckFunction>
  Term checked_replace(Term t, CheckFunction f, ReplaceFunction r)
  {
    ATerm x = detail::partial_replace_impl(aterm_traits<Term>::term(t), checked_replace_helper<ReplaceFunction, CheckFunction>(f, r));
    return Term(reinterpret_cast<ATermAppl>(x));   
  }
*/

} // namespace atermpp

#endif // MCRL2_ATERMPP_ALGORITHM_H
