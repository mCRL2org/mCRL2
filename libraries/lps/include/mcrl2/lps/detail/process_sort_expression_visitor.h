// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/detail/process_sort_expression_visitor.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_DETAIL_PROCESS_SORT_EXPRESSION_VISITOR_H
#define MCRL2_LPS_DETAIL_PROCESS_SORT_EXPRESSION_VISITOR_H

#include <set>
#include "mcrl2/data/sort_expression.h"
#include "mcrl2/lps/process_expression_visitor.h"

namespace mcrl2 {

namespace lps {

namespace detail {

template <typename Term>
struct process_sort_expression_visitor: public process_expression_visitor<Term>
{
  typedef process_expression_visitor<Term> super;

  std::set<data::sort_expression> result;

  /// \brief Visit action node
  /// \return The result of visiting the node
  bool visit_action(const process_expression& x, const action_label& l, const data::data_expression_list& v)
  {
    traverse_sort_expressions(l, std::inserter(result, result.end()));
    traverse_sort_expressions(v, std::inserter(result, result.end()));
    return continue_recursion;
  }

  /// \brief Visit process_assignment node
  /// \return The result of visiting the node
  bool visit_process_assignment(const process_expression& x, const process_identifier& pi, const data::assignment_list& v)
  {
    traverse_sort_expressions(pi, std::inserter(result, result.end()));
    traverse_sort_expressions(v, std::inserter(result, result.end()));
    return continue_recursion;
  }

  /// \brief Visit sum node
  /// \return The result of visiting the node
  bool visit_sum(const process_expression& x, const data::variable_list& v, const process_expression& right)
  {
    traverse_sort_expressions(v, std::inserter(result, result.end()));
    return continue_recursion;
  }

  /// \brief Visit at_time node
  /// \return The result of visiting the node
  bool visit_at_time(const process_expression& x, const process_expression& left, const data::data_expression& d)
  {
    *dest++ = d.sort();
    return continue_recursion;
  }

  /// \brief Visit if_then node
  /// \return The result of visiting the node
  bool visit_if_then(const process_expression& x, const data::data_expression& d, const process_expression& right)
  {
    *dest++ = d.sort();
    return continue_recursion;
  }

  /// \brief Visit if_then_else node
  /// \return The result of visiting the node
  bool visit_if_then_else(const process_expression& x, const data::data_expression& d, const process_expression& left, const process_expression& right)
  {
    *dest++ = d.sort();
    return continue_recursion;
  }
};

} // namespace detail

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_DETAIL_PROCESS_SORT_EXPRESSION_VISITOR_H
