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
#include "mcrl2/pbes/srf_pbes.h"

namespace mcrl2 {

namespace pbes_system {

struct unify_parameters_replace_function
{
  // maps propositional variable names to the corresponding parameters
  const std::map<core::identifier_string, data::variable_list>& propositional_variable_parameters;

  // generates default expressions for sorts
  mutable data::default_expression_generator generator;

  // maps parameters to their new positions
  mutable std::map<data::variable, std::size_t> parameter_positions;

  // store the 'missing' parameters for each parameter list
  mutable std::map<data::variable_list, std::vector<data::variable>> missing_parameters;

  // the new list of parameters
  data::variable_list parameters;

  // reuse this vector for constructing new parameters
  mutable std::vector<data::data_expression> tmp_parameters;

  // If true then the newly introduced parameters will be reset to a default value instead of copying the value.
  bool m_reset = true;

  data::variable_list compute_parameters()
  {
    std::vector<data::variable> parameter_vector;
    for (const auto& p: propositional_variable_parameters)
    {
      const data::variable_list& eqn_parameters = p.second;
      for (const data::variable& v: eqn_parameters)
      {
        auto i = parameter_positions.find(v);
        if (i == parameter_positions.end())
        {
          parameter_positions[v] = parameter_vector.size();
          parameter_vector.push_back(v);
        }
      }
    }
    return data::variable_list(parameter_vector.begin(), parameter_vector.end());
  }

  unify_parameters_replace_function(
    const std::map<core::identifier_string, data::variable_list>& propositional_variable_parameters_,
    const data::data_specification& dataspec,
    bool reset = true
  )
    : propositional_variable_parameters(propositional_variable_parameters_), generator(dataspec), m_reset(reset)
  {
    using utilities::detail::contains;

    parameters = compute_parameters();
    tmp_parameters.resize(parameters.size());

    // Compute missing parameters for each equation
    for (const auto& p: propositional_variable_parameters_)
    {
      const data::variable_list& eqn_parameters = p.second;
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
  }

  propositional_variable_instantiation operator()(const propositional_variable_instantiation& x) const
  {
    const data::variable_list& variables = propositional_variable_parameters.at(x.name());
    const data::data_expression_list& values = x.parameters();
    auto i = variables.begin();
    auto j = values.begin();
    for (; i != variables.end(); ++i, ++j)
    {
      std::size_t pos = parameter_positions[*i];
      tmp_parameters[pos] = *j;
    }
    for (const data::variable& v: missing_parameters[variables])
    {
      std::size_t pos = parameter_positions[v];

      if (m_reset)
      {
        tmp_parameters[pos] = generator(v.sort());
      }
      else
      {
        tmp_parameters[pos] = v;
      }
    }
    return propositional_variable_instantiation(x.name(), data::data_expression_list(tmp_parameters.begin(), tmp_parameters.end()));
  };
};

inline
void unify_parameters(pbes& p)
{
  std::map<core::identifier_string, data::variable_list> propositional_variable_parameters;
  for (const pbes_equation& eqn: p.equations())
  {
    propositional_variable_parameters[eqn.variable().name()] = eqn.variable().parameters();
  }

  unify_parameters_replace_function replace(propositional_variable_parameters, p.data());

  // update the right hand sides of the equations
  replace_propositional_variables(p, replace);

  // update the initial state
  p.initial_state() = replace(p.initial_state());

  // update the left hand sides of the equations
  for (pbes_equation& eqn: p.equations())
  {
    propositional_variable& X = eqn.variable();
    X = propositional_variable(X.name(), replace.parameters);
  }
}

inline
void unify_parameters(srf_pbes& p, bool reset = true)
{
  std::map<core::identifier_string, data::variable_list> propositional_variable_parameters;
  for (const srf_equation& eqn: p.equations())
  {
    propositional_variable_parameters[eqn.variable().name()] = eqn.variable().parameters();
  }

  unify_parameters_replace_function replace(propositional_variable_parameters, p.data(), reset);

  // update the equations
  for (srf_equation& eqn: p.equations())
  {
    for (srf_summand& summand: eqn.summands())
    {
      summand.variable() = replace(summand.variable());
    }
    propositional_variable& X = eqn.variable();
    X = propositional_variable(X.name(), replace.parameters);
  }

  // update the initial state
  p.initial_state() = replace(p.initial_state());
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_UNIFY_PARAMETERS_H
