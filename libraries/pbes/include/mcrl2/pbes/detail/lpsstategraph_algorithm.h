// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/lpsstategraph_algorithm.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_LPSSTATEGRAPH_ALGORITHM_H
#define MCRL2_PBES_DETAIL_LPSSTATEGRAPH_ALGORITHM_H

#include "mcrl2/data/set_identifier_generator.h"
#include "mcrl2/lps/io.h"
#include "mcrl2/lps/is_well_typed.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/pbes/detail/stategraph_global_reset_variables.h"
#include "mcrl2/pbes/detail/stategraph_local_reset_variables.h"
#include "mcrl2/pbes/tools/pbesstategraph_options.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

inline
data::data_expression_list right_hand_sides(const data::assignment_list& x)
{
  std::vector<data::data_expression> result;
  for (const data::assignment& a: x)
  {
    result.push_back(a.rhs());
  }
  return data::data_expression_list(result.begin(), result.end());
}

inline
pbes construct_stategraph_pbes(const lps::specification& lpsspec)
{
  pbes pbesspec;
  data::data_specification dataspec = lpsspec.data();

  // create 'used' functions for every sort in the free variables of actions
  data::set_identifier_generator generator;
  core::identifier_string used = generator("used");
  std::set<data::sort_expression> action_variable_sorts;
  for (const lps::action_summand& summand: lpsspec.process().action_summands())
  {
    for (const data::variable& v: lps::find_free_variables(summand.multi_action()))
    {
      action_variable_sorts.insert(v.sort());
    }
  }
  std::map<data::sort_expression, data::function_symbol> functions;
  for (const data::sort_expression& s: action_variable_sorts)
  {
    data::function_symbol f(used, data::function_sort({ s }, data::sort_bool::bool_()));
    functions[s] = f;
    dataspec.add_mapping(f);
  }

  // the PBES variable
  core::identifier_string X("X");

  // create a PBES equation that contains a conjunct for each action summand
  std::vector<pbes_expression> conjuncts;
  for (const lps::action_summand& summand: lpsspec.process().action_summands())
  {
    const auto& ei = summand.summation_variables();
    const auto& ci = summand.condition();
    data::data_expression_list gi = summand.next_state(lpsspec.process().process_parameters());

    data::data_expression x = ci;
    for (const data::variable& v: lps::find_free_variables(summand.multi_action()))
    {
      x = data::and_(x, data::application(functions[v.sort()], { v }));
    }
    propositional_variable_instantiation Xi(X, gi);
    conjuncts.push_back(make_forall(ei, imp(x, Xi)));
  }
  // N.B. It is essential that the order in which the conjuncts are traversed in a PBES matches the order of the corresponding summands.
  pbes_expression rhs = conjuncts[0];
  for (std::size_t i = 1; i < conjuncts.size(); i++)
  {
    rhs = and_(conjuncts[i], rhs);
  }
  pbes_equation eqn(fixpoint_symbol::nu(), propositional_variable(X, lpsspec.process().process_parameters()), rhs);

  pbesspec.data() = dataspec;
  pbesspec.initial_state() = propositional_variable_instantiation(X, right_hand_sides(lpsspec.initial_process().assignments()));
  pbesspec.equations().push_back(eqn);
  return pbesspec;
}

inline
void extract_stategraph_lps(lps::specification& lpsspec /* some parameters containing the marking */)
{
}

} // namespace detail

/// \brief Apply the stategraph algorithm
/// \param p A PBES to which the algorithm is applied.
/// \param options The options for the algorithm.
inline
void lpsstategraph(lps::specification& lpsspec, const pbesstategraph_options& options)
{
  pbes p = detail::construct_stategraph_pbes(lpsspec);

  algorithms::normalize(p);
  if (options.use_global_variant)
  {
    detail::global_reset_variables_algorithm algorithm(p, options);
    p = algorithm.run();
    if (options.print_influence_graph)
    {
      detail::stategraph_influence_graph_algorithm ialgo(algorithm.get_pbes());
      ialgo.run();
    }
  }
  else
  {
    detail::local_reset_variables_algorithm algorithm(p, options);
    p = algorithm.run();
    if (options.print_influence_graph)
    {
      detail::stategraph_influence_graph_algorithm ialgo(algorithm.get_pbes());
      ialgo.run();
    }
  }

  detail::extract_stategraph_lps(lpsspec);
}

void lpsstategraph(const std::string& input_filename,
                   const std::string& output_filename,
                   const pbesstategraph_options& options)
{
  lps::specification lpsspec;
  lps::load_lps(lpsspec, input_filename);
  lpsstategraph(lpsspec, options);
  lps::save_lps(lpsspec, output_filename);
  if (!lps::detail::is_well_typed(lpsspec))
  {
    mCRL2log(log::error) << "lpsstategraph error: not well typed!" << std::endl;
    mCRL2log(log::error) << pp(lpsspec) << std::endl;
  }
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_LPSSTATEGRAPH_ALGORITHM_H
