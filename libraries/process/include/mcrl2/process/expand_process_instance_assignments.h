// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/expand_process_instance_assignments.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_EXPAND_PROCESS_INSTANCE_ASSIGNMENTS_H
#define MCRL2_PROCESS_EXPAND_PROCESS_INSTANCE_ASSIGNMENTS_H

#include "mcrl2/process/builder.h"
#include "mcrl2/process/utility.h"

namespace mcrl2 {

namespace process {

namespace detail {

struct expand_process_instance_assignments_builder: public process_expression_builder<expand_process_instance_assignments_builder>
{
  typedef process_expression_builder<expand_process_instance_assignments_builder> super;
  using super::enter;
  using super::leave;
  using super::apply;
  using super::update;

  const std::vector<process_equation>& equations;

  expand_process_instance_assignments_builder(const std::vector<process_equation>& equations_)
    : equations(equations_)
  {}

  /// \brief Converts a process instance P(e) into p[d := e], where P(d) = d is the equation
  /// corresponding to P.
  process_expression apply(const process::process_instance& x)
  {
    const process_equation& eqn = find_equation(equations, x.identifier());
    process_expression p = eqn.expression();
    data::mutable_map_substitution<> sigma;
    data::variable_list d = eqn.formal_parameters();
    data::data_expression_list e = x.actual_parameters();
    data::variable_list::iterator di = d.begin();
    data::data_expression_list::iterator ei = e.begin();
    for (; di != d.end(); ++di, ++ei)
    {
      sigma[*di] = *ei;
    }
    process_expression result = process::replace_variables_capture_avoiding(p, sigma, data::substitution_variables(sigma));
    return result;
  }

  /// \brief Converts a process instance assignment P(d = e) into p[d := e], where P(d) = d is the equation
  /// corresponding to P.
  process_expression apply(const process::process_instance_assignment& x)
  {
    const process_equation& eqn = find_equation(equations, x.identifier());
    process_expression p = eqn.expression();
    data::mutable_map_substitution<> sigma;
    for (const auto& a: x.assignments())
    {
      sigma[a.lhs()] = a.rhs();
    }
    process_expression result = process::replace_variables_capture_avoiding(p, sigma, data::substitution_variables(sigma));
    return result;
  }
};

} // detail

/// \brief Replaces embedded process instances by the right hand sides of the corresponding equations
inline
process_expression expand_process_instance_assignments(const process_expression& x, const std::vector<process_equation>& equations)
{
  detail::expand_process_instance_assignments_builder f(equations);
  return f.apply(x);
}

// Converts a process_instance_assignment into a process_instance, by expanding assignments
inline
process_instance expand_assignments(const process::process_instance_assignment& x, const std::vector<process_equation>& equations)
{
  const process_equation& eqn = find_equation(equations, x.identifier());
  data::assignment_sequence_substitution sigma(x.assignments());
  std::vector<data::data_expression> e;
  for (const data::variable& v: eqn.formal_parameters())
  {
    e.push_back(sigma(v));
  }
  return process_instance(x.identifier(), data::data_expression_list(e.begin(), e.end()));
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_EXPAND_PROCESS_INSTANCE_ASSIGNMENTS_H
