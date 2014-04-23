// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/detail/expand_process_instance_assignments.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_DETAIL_EXPAND_PROCESS_INSTANCE_ASSIGNMENTS_H
#define MCRL2_PROCESS_DETAIL_EXPAND_PROCESS_INSTANCE_ASSIGNMENTS_H

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
  using super::operator();

#if BOOST_MSVC
#include "mcrl2/core/detail/builder_msvc.inc.h"
#endif

  const std::vector<process_equation>& equations;

  expand_process_instance_assignments_builder(const std::vector<process_equation>& equations_)
    : equations(equations_)
  {}

  process_expression operator()(const process::process_instance_assignment& x)
  {
    const process_equation& equation = find_equation(equations, x.identifier());
    data::data_expression_list e = data::make_data_expression_list(equation.formal_parameters());
    e = data::replace_variables(e, data::assignment_sequence_substitution(x.assignments()));
    return process_instance(x.identifier(), e);
  }
};

} // detail

inline
void expand_process_instance_assignments(process_specification& procspec)
{
  detail::expand_process_instance_assignments_builder f(procspec.equations());
  f(procspec);
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_DETAIL_EXPAND_PROCESS_INSTANCE_ASSIGNMENTS_H
