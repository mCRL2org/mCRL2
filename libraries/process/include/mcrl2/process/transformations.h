// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/transformations.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_TRANSFORMATIONS_H
#define MCRL2_PROCESS_TRANSFORMATIONS_H

#include "mcrl2/data/consistency.h"
#include "mcrl2/process/builder.h"

namespace mcrl2 {

namespace process {

namespace detail {

struct remove_nested_if_then_builder: public process_expression_builder<remove_nested_if_then_builder>
{
  typedef process_expression_builder<remove_nested_if_then_builder> super;
  using super::apply;

  process_expression apply(const process::if_then& x)
  {
    process_expression then_case = apply(x.then_case());
    if (is_if_then(then_case))
    {
      const if_then& nested_then_case = atermpp::down_cast<if_then>(then_case);
      data::data_expression condition = data::and_(x.condition(), nested_then_case.condition());
      return if_then(condition, nested_then_case.then_case());
    }
    return if_then(x.condition(), then_case);
  }
};

struct convert_process_instances_builder: public process_expression_builder<convert_process_instances_builder>
{
  typedef process_expression_builder<convert_process_instances_builder> super;
  using super::apply;

  std::map<process_identifier, const process_equation*> equation_index;

  convert_process_instances_builder(process_specification& procspec)
  {
    for (const process_equation& eqn: procspec.equations())
    {
      equation_index[eqn.identifier()] = &eqn;
    }
  }

  process_expression apply(const process::process_instance& x)
  {
    const data::variable_list& d = equation_index.find(x.identifier())->second->formal_parameters();
    const data::data_expression_list& e = x.actual_parameters();
    auto di = d.begin();
    auto ei = e.begin();
    std::vector<data::assignment> assignments;
    for (; di != d.end(); ++di, ++ei)
    {
      if (*di != *ei)
      {
        assignments.emplace_back(*di, *ei);
      }
    }
    return process_instance_assignment(x.identifier(), data::assignment_list(assignments.begin(), assignments.end()));
  }
};

} // namespace detail

inline
void remove_nested_if_then(process_specification& procspec)
{
  detail::remove_nested_if_then_builder f;
  f.update(procspec);
}

inline
void convert_process_instances(process_specification& procspec)
{
  detail::convert_process_instances_builder f(procspec);
  f.update(procspec);
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_TRANSFORMATIONS_H
