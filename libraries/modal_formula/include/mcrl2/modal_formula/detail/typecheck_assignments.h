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
  std::set<core::identifier_string> names;
  data::assignment_list result;
  for (const data::assignment& a: assignments)
  {
    const core::identifier_string& name = a.lhs().name();

    if (names.find(name) != names.end())
    {
      throw mcrl2::runtime_error("non-unique formal parameter " + core::pp(name) + ")");
    }
    names.insert(name);

    data::sort_expression lhs_sort = a.lhs().sort();
    typechecker.check_sort_is_declared(lhs_sort);

    data::data_expression rhs = a.rhs();
    data::sort_expression expected_sort = typechecker.expand_numeric_types_down(lhs_sort);
    rhs = typechecker.typecheck_data_expression1(rhs, expected_sort, variable_context.context());
    if (!typechecker.type_match(lhs_sort, rhs.sort()))
    {
      // TODO: is it necessary to use a different expected sort here?
      data::sort_expression expected_sort = lhs_sort;
      rhs = typechecker.upcast_numeric_type(rhs, lhs_sort, variable_context.context());
    }
    result.push_front(data::assignment(a.lhs(), rhs));
  }
  result = atermpp::reverse(result);
  result = data::normalize_sorts(result, typechecker.get_sort_specification());
  return result;
}

} // namespace detail

} // namespace state_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_FORMULA_DETAIL_TYPECHECK_ASSIGNMENTS_H
