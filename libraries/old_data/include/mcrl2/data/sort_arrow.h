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

/// \brief singly linked list of sorts
///
typedef atermpp::term_list<sort_arrow> sort_arrow_list;

/// \brief sort arrow.
///
/// Models sorts of shape <tt>A -\> B</tt>.
/// 
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
    /// Constructor.
    ///
    sort_arrow()
      : sort_expression(core::detail::constructSortArrow())
    {}

    /// Constructor.
    ///
    sort_arrow(ATermAppl t)
      : sort_expression(t)
    {
      assert(core::detail::check_term_SortArrow(m_term));
    }

    /// Constructor.
    ///
    sort_arrow(atermpp::aterm_appl t)
      : sort_expression(t)
    {
      assert(core::detail::check_term_SortArrow(m_term));
    }
   
    /// Constructor.
    ///
    sort_arrow(sort_expression_list arguments, sort_expression result)
      : sort_expression(core::detail::gsMakeSortArrow(arguments, result))
    {
      assert(core::detail::check_term_SortArrow(m_term));
    }
   
    /// \overload
    ///
    bool is_arrow() const
    {
      return true;
    }

    /// Returns the argument sorts.
    ///
    sort_expression_list argument_sorts() const
    {
      return atermpp::list_arg1(*this);
    }

    /// Returns the result sort.
    ///
    sort_expression result_sort() const
    {
      return atermpp::arg2(*this);
    }   
};

/// \brief Returns true if the term t is a sort arrow.
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
