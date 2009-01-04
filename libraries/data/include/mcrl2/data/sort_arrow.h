// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/sort_arrow.h
/// \brief The class sort_arrow.

#ifndef MCRL2_DATA_SORT_ARROW_H
#define MCRL2_DATA_SORT_ARROW_H

#include "mcrl2/data/sort_expression.h"

namespace mcrl2 {

namespace data {

class sort_arrow;

/// \brief Read-only singly linked list of sorts
typedef atermpp::term_list<sort_arrow> sort_arrow_list;

/// \brief sort arrow.
/// Models sorts of shape <tt>A -\> B</tt>.
//<SortExpr>    ::= <SortId>
//                | SortList(<SortExpr>)                                   (- di)
//                | SortSet(<SortExpr>)                                    (- di)
//                | SortBag(<SortExpr>)                                    (- di)
//                | SortStruct(<StructCons>+)                              (- di)
//                | SortArrow(<SortExpr>+, <SortExpr>)                 (- di)
//                | SortArrow(<SortExpr>, <SortExpr>)                      (+ di)
class sort_arrow: public sort_expression
{
  public:
    /// \brief Constructor.
    sort_arrow()
      : sort_expression(core::detail::constructSortArrow())
    {}

    /// \brief Constructor.
    /// \param t A term
    sort_arrow(ATermAppl t)
      : sort_expression(t)
    {
      assert(core::detail::check_term_SortArrow(m_term));
    }

    /// \brief Constructor.
    /// \param t A term
    sort_arrow(atermpp::aterm_appl t)
      : sort_expression(t)
    {
      assert(core::detail::check_term_SortArrow(m_term));
    }

    /// \brief Constructor.
    /// \param arguments A sequence of sort expressions
    /// \param result A sort expression
    sort_arrow(sort_expression_list arguments, sort_expression result)
      : sort_expression(core::detail::gsMakeSortArrow(arguments, result))
    {
      assert(core::detail::check_term_SortArrow(m_term));
    }

    /// \brief FUNCTION_DESCRIPTION
    /// \overload
    /// \return Always returns true.
    bool is_arrow() const
    {
      return true;
    }

    /// \brief Returns the argument sorts.
    /// \return The arguments of the sort arrow.
    sort_expression_list argument_sorts() const
    {
      return atermpp::list_arg1(*this);
    }

    /// \brief Returns the result sort.
    /// \return The target sort of the sort arrow.
    sort_expression result_sort() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief Returns true if the term t is a sort arrow.
/// \param t A term
/// \return True if the term is a sort arrow.
inline
bool is_sort_arrow(atermpp::aterm_appl t)
{
  return core::detail::gsIsSortArrow(t);
}

} // namespace data

} // namespace mcrl2

/// \cond INTERNAL_DOCS
MCRL2_ATERM_TRAITS_SPECIALIZATION(mcrl2::data::sort_arrow)
/// \endcond

#endif // MCRL2_DATA_SORT_ARROW_H
