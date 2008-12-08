// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/sort_expression.h
/// \brief The class sort_expression.

#ifndef MCRL2_DATA_SORT_EXPRESSION_H
#define MCRL2_DATA_SORT_EXPRESSION_H

#include <cassert>
#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_traits.h"
#include "mcrl2/core/print.h"
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/core/detail/soundness_checks.h"
#include "mcrl2/core/detail/constructors.h"

namespace mcrl2 {

namespace data {

class sort_expression;

/// \brief Read-only singly linked list of sort expressions
///
typedef atermpp::term_list<sort_expression> sort_expression_list;

/// \brief Sort expression.
///
/// A sort expression can either be a sort identifier or a sort arrow.
///
//<SortExpr>    ::= <SortId>
//                | SortList(<SortExpr>)                                   (- di)
//                | SortSet(<SortExpr>)                                    (- di)
//                | SortBag(<SortExpr>)                                    (- di)
//                | SortStruct(<StructCons>+)                              (- di)
//                | SortArrow(<SortExpr>+, <SortExpr>)                     (- di)
//                | SortArrow(<SortExpr>, <SortExpr>)                      (+ di)
//
//<SortId>       ::= SortId(<String>)
class sort_expression: public atermpp::aterm_appl
{
  public:
    /// \brief Constructor.
    sort_expression()
      : atermpp::aterm_appl(core::detail::constructSortId())
    {}

    /// \brief Constructor.
    /// \param t A term containing a sort.
    sort_expression(ATermAppl t)
      : atermpp::aterm_appl(t)
    {
      assert(core::detail::check_rule_SortExpr(m_term));
    }

    /// \brief Constructor.
    /// \param t A term containing a sort.
    sort_expression(atermpp::aterm_appl t)
      : atermpp::aterm_appl(t)
    {
      assert(core::detail::check_rule_SortExpr(m_term));
    }

    /// \brief Constructor.
    /// \param s A string representation of a sort arrow.
    sort_expression(std::string s)
      : atermpp::aterm_appl(core::detail::gsMakeSortId(core::detail::gsString2ATermAppl(s.c_str())))
    {}

    /// \brief Returns true if it is a sort_expression of type A -> B.
    /// \return True if the sort is an arrow sort.
    bool is_arrow() const
    {
      return core::detail::gsIsSortArrow(*this);
    }
};

/// \brief Returns true if the term t is a sort_expression
/// \param t A term.
/// \return True if the term is a sort expression.
inline
bool is_sort_expression(atermpp::aterm_appl t)
{
  return core::detail::gsIsSortId(t) || core::detail::gsIsSortArrow(t);
}

/// \brief Returns the sort_expression 'domain -> range'.
/// \param domain A domain sort.
/// \param range A range sort.
/// \return The arrow sort corresponding to the given domain and range.
inline
sort_expression arrow(sort_expression_list domain, sort_expression range)
{
  return core::detail::gsMakeSortArrow(domain, range);
}

/// \brief Returns the domain sorts of s.
/// \deprecated
/// \param s A sort.
/// \return The domain sorts of the given sort.
inline
sort_expression_list domain_sorts(sort_expression s)
{
  return core::detail::gsGetSortExprDomain(s);
}

/// \brief Returns the range sort of s.
/// \deprecated
/// \param s A sort.
/// \return The range sort of the given sort.
inline
sort_expression result_sort(sort_expression s)
{
  return core::detail::gsGetSortExprResult(s);
}

/// \brief Accessor functions and predicates for sort expressions.
namespace sort_expr {

  /// \brief Returns the predefined sort_expression real.
  /// \return The predefined sort Real.
  inline
  sort_expression real()
  {
    return core::detail::gsMakeSortExprReal();
  }

  /// \brief Returns the predefined sort_expression int.
  /// \return The predefined sort Int.
  inline
  sort_expression int_()
  {
    return core::detail::gsMakeSortExprInt();
  }

  /// \brief Returns the predefined sort_expression pos.
  /// \return The predefined sort Pos.
  inline
  sort_expression pos()
  {
    return core::detail::gsMakeSortExprPos();
  }

  /// \brief Returns the predefined sort_expression nat.
  /// \return The predefined sort Nat.
  inline
  sort_expression nat()
  {
    return core::detail::gsMakeSortExprNat();
  }

  /// \brief Returns the predefined sort_expression bool.
  /// \return The predefined sort Bool.
  inline
  sort_expression bool_()
  {
    return core::detail::gsMakeSortExprBool();
  }

  /// \brief Returns true if the term t equals the sort_expression real
  inline bool is_real(atermpp::aterm_appl t) { return t == real(); }

  /// \brief Returns true if the term t equals the sort_expression int
  inline bool is_int (atermpp::aterm_appl t) { return t == int_(); }

  /// \brief Returns true if the term t equals the sort_expression pos
  inline bool is_pos (atermpp::aterm_appl t) { return t == pos(); }

  /// \brief Returns true if the term t equals the sort_expression nat
  inline bool is_nat (atermpp::aterm_appl t) { return t == nat(); }

  /// \brief Returns true if the term t equals the sort_expression bool
  inline bool is_bool(atermpp::aterm_appl t) { return t == bool_(); }

} // namespace sort_expr

} // namespace data

} // namespace mcrl2

/// \cond INTERNAL_DOCS
MCRL2_ATERM_TRAITS_SPECIALIZATION(mcrl2::data::sort_expression)
/// \endcond

#endif // MCRL2_DATA_SORT_EXPRESSION_H
