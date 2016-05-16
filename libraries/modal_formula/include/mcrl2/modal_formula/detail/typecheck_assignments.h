// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/detail/typecheck_assignments.h
/// \brief add your file description here.

#ifndef MCRL2_MODAL_FORMULA_DETAIL_TYPECHECK_ASSIGNMENTS_H
#define MCRL2_MODAL_FORMULA_DETAIL_TYPECHECK_ASSIGNMENTS_H

#include "mcrl2/data/normalize_sorts.h"
#include "mcrl2/data/typecheck.h"

namespace mcrl2 {

namespace state_formulas {

namespace detail {

// Apparently this approach is too simple
inline
data::assignment_list typecheck_assignments_does_not_work(const data::assignment_list& assignments,
                                                          const std::map<core::identifier_string, data::sort_expression>& variables,
                                                          data::data_type_checker& typechecker
                                                         )
{
  data::assignment_list result;
  for (const data::assignment& a: assignments)
  {
    data::sort_expression expected_sort = typechecker.expand_numeric_types_down(a.lhs().sort());
    data::data_expression rhs = typechecker.typecheck_data_expression1(a.rhs(), expected_sort, variables);
    result.push_front(data::assignment(a.lhs(), rhs));
  }
  result = atermpp::reverse(result);
  assert(data::normalize_sorts(result, typechecker.get_sort_specification()) == result);
  return result;
}

inline
data::assignment_list typecheck_assignments(const data::assignment_list& assignments,
                                            const data::detail::variable_context& variable_context,
                                            data::data_type_checker& typechecker
                                           )
{
  std::map<core::identifier_string, data::sort_expression> sort_map;
  data::assignment_list result;
  data::sort_expression_list sorts;
  for (const data::assignment& a: assignments)
  {
    const core::identifier_string& name = a.lhs().name();
    if (sort_map.count(name) > 0)
    {
      throw mcrl2::runtime_error("non-unique formal parameter " + core::pp(name) + ")");
    }

    data::sort_expression lhs_sort = a.lhs().sort();
    typechecker.check_sort_is_declared(lhs_sort);

    sort_map[name] = lhs_sort;

    data::data_expression rhs = a.rhs();
    data::sort_expression rhs_sort;
    try
    {
      rhs_sort = typechecker.visit_data_expression(variable_context.context(), rhs, typechecker.expand_numeric_types_down(lhs_sort));
    }
    catch (mcrl2::runtime_error& e)
    {
      throw mcrl2::runtime_error(std::string(e.what()) + ".");
    }

    data::sort_expression temp;
    if (!typechecker.type_match(lhs_sort, rhs_sort, temp))
    {
      //upcasting
      try
      {
        std::map<core::identifier_string,data::sort_expression> dummy_table;
        rhs_sort = typechecker.upcast_numeric_type(lhs_sort, rhs_sort, rhs, variable_context.context(), dummy_table, false);
      }
      catch (mcrl2::runtime_error& e)
      {
        throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot (up)cast " + data::pp(rhs) + " to type " + data::pp(lhs_sort));
      }
    }
    result.push_front(data::assignment(a.lhs(), rhs));
    sorts.push_front(lhs_sort);
  }
  result = atermpp::reverse(result);
  result = data::normalize_sorts(result, typechecker.get_sort_specification());
  return result;
}

} // namespace detail

} // namespace state_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_FORMULA_DETAIL_TYPECHECK_ASSIGNMENTS_H
