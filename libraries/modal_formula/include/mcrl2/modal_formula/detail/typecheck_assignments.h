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

inline
data::assignment typecheck_assignment(const data::assignment& x,
                                      const data::detail::variable_context& variable_context,
                                      data::data_type_checker& typechecker
                                     )
{
  const data::variable& lhs = x.lhs();
  data::data_expression rhs = x.rhs();
  typechecker.check_sort_is_declared(lhs.sort());
  data::sort_expression expected_sort = typechecker.expand_numeric_types_down(lhs.sort());
  rhs = typechecker.typecheck_data_expression(rhs, expected_sort, variable_context);
  if (!typechecker.type_match(lhs.sort(), rhs.sort()))
  {
    rhs = typechecker.upcast_numeric_type(rhs, lhs.sort(), variable_context);
  }
  return data::assignment(x.lhs(), rhs);
}

inline
data::assignment_list typecheck_assignments(const data::assignment_list& assignments,
                                            const data::detail::variable_context& variable_context,
                                            data::data_type_checker& typechecker
                                           )
{
  // check for name clashes
  std::set<core::identifier_string> names;
  for (const data::assignment& a: assignments)
  {
    const core::identifier_string& name = a.lhs().name();
    if (names.find(name) != names.end())
    {
      throw mcrl2::runtime_error("non-unique formal parameter " + core::pp(name) + ")");
    }
    names.insert(name);
  }

  // typecheck the assignments
  data::assignment_vector result;
  for (const data::assignment& a: assignments)
  {
    result.push_back(typecheck_assignment(a, variable_context, typechecker));
  }
  return data::assignment_list(result.begin(), result.end());
}

} // namespace detail

} // namespace state_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_FORMULA_DETAIL_TYPECHECK_ASSIGNMENTS_H
