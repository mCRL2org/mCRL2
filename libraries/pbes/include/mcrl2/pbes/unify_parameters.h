// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/unify_parameters.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_UNIFY_PARAMETERS_H
#define MCRL2_PBES_UNIFY_PARAMETERS_H

#include "mcrl2/data/default_expression_generator.h"
#include "mcrl2/pbes/pbes_equation_index.h"
#include "mcrl2/pbes/replace.h"

namespace mcrl2 {

namespace pbes_system {

inline
void unify_parameters(pbes& p)
{
  using utilities::detail::contains;

  // a mapping that contains the new positions of variables
  std::map<data::variable_list, std::vector<std::size_t>> variable_positions;

  // compute the new list of parameters, and the parameter positions
  std::vector<data::variable> parameter_vector;
  std::map<data::variable, std::size_t> parameter_positions; // maps parameters to positions

  for (const pbes_equation& eqn: p.equations())
  {
    for (const data::variable& v: eqn.variable().parameters())
    {
      auto i = parameter_positions.find(v);
      if (i == parameter_positions.end())
      {
        parameter_positions[v] = parameter_vector.size();
        parameter_vector.push_back(v);
      }
    }
  }
  data::variable_list parameters(parameter_vector.begin(), parameter_vector.end());

  // Compute missing parameters for each equation
  std::map<data::variable_list, std::vector<data::variable>> missing_parameters;
  for (const pbes_equation& eqn: p.equations())
  {
    const data::variable_list& eqn_parameters = eqn.variable().parameters();
    auto i = missing_parameters.find(eqn_parameters);
    if (i != missing_parameters.end())
    {
      continue;
    }
    std::set<data::variable> eqn_parameter_set(eqn_parameters.begin(), eqn_parameters.end());
    std::vector<data::variable> missing;
    for (const data::variable& v: parameters)
    {
      if (!contains(eqn_parameter_set, v))
      {
        missing.push_back(v);
      }
    }
    missing_parameters[eqn_parameters] = missing;
  }

  data::default_expression_generator generator(p.data());
  pbes_equation_index equation_index(p);

  // reuse this vector for constructing new parameters
  std::vector<data::data_expression> new_parameters(parameters.size());

  auto replace = [&](const propositional_variable_instantiation& x)
    {
      std::size_t k = equation_index.index(x.name());
      const data::variable_list& variables = p.equations()[k].variable().parameters();
      const data::data_expression_list& values = x.parameters();
      auto i = variables.begin();
      auto j = values.begin();
      for (; i != variables.end(); ++i, ++j)
      {
        std::size_t pos = parameter_positions[*i];
        new_parameters[pos] = *j;
      }
      for (const data::variable& v: missing_parameters[variables])
      {
        std::size_t pos = parameter_positions[v];
        new_parameters[pos] = generator(v.sort());
      }
      return propositional_variable_instantiation(x.name(), data::data_expression_list(new_parameters.begin(), new_parameters.end()));
    };

  // update the right hand sides of the equations
  replace_propositional_variables(p, replace);

  // update the initial state
  p.initial_state() = replace(p.initial_state());

  // update the left hand sides
  for (pbes_equation& eqn: p.equations())
  {
    propositional_variable& X = eqn.variable();
    X = propositional_variable(X.name(), parameters);
  }
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_UNIFY_PARAMETERS_H
