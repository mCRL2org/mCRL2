// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "dependency_cleave.h"

#include "lpscleave_utility.h"

#include <optional>

using namespace mcrl2;

struct summand_dependencies
{
  std::set<data::variable> S_i_X; ///< The complete set of dependencies (S_i_X in the report).
  bool in_L_X; ///< Indicates that this summand belongs to the set L_X.
};

/// \brief Computes the dependencies of the given summand w.r.t. the parameters (determining X).
inline
summand_dependencies compute_dependencies(
  const lps::stochastic_specification& spec,
  const lps::stochastic_action_summand& summand,
  const data::variable_list& parameters,
  const data::variable_list& other_parameters)
{
  // Compute the set S^i_X, first add the dependencies of the conditions (Condition 3).
  std::set<data::variable> S_i_X;

  // (Condition 1) Compute the first set, i.e. the positions in N \ X (the other parameters) that our update functions and action expression
  // depends on. We first compute the total set of dependencies and then remove elements from X.
  std::set<data::variable> set_1;
  for (const process::action& action : summand.multi_action().actions())
  {
    auto dependencies = data::find_free_variables(action.arguments());
    set_1.insert(dependencies.begin(), dependencies.end());
  }

  // Find the dependencies of the assignments both processes.
  std::set<data::variable> assignment_dependencies;

  // (Condition 2) parameters that the other process needs for its update expressions (elements of X).
  std::set<data::variable> set_2;
  for (const data::assignment& assignment : summand.assignments())
  {
    auto dependencies = data::find_free_variables(assignment.rhs());
    if (std::find(parameters.begin(), parameters.end(), assignment.lhs()) != parameters.end())
    {
      // This is an assignment for our parameters
      assignment_dependencies.insert(dependencies.begin(), dependencies.end());
    }
    else
    {
      // This is an assignment for the other component's parameters
      set_2.insert(dependencies.begin(), dependencies.end());
    }
  }

  set_1.insert(assignment_dependencies.begin(), assignment_dependencies.end());

  // Remove the indices in X.
  for (const data::variable& param : parameters)
  {
    set_1.erase(param);
  }

  // Remove the other parameters from the second set of positions (from (N \ X)).
  for (const data::variable& param : other_parameters)
  {
    set_2.erase(param);
  }

  std::set<data::variable> set_3 = data::find_free_variables(summand.condition());

  // Gather all the necessary dependencies S^i_X, the condition dependencies were already added.
  S_i_X.insert(set_1.begin(), set_1.end());
  S_i_X.insert(set_2.begin(), set_2.end());
  S_i_X.insert(set_3.begin(), set_3.end());

  // Remove the global variables.

  // This version crashes, but would be nicer/more efficient:
  // dependencies.erase(spec.global_variables().begin(), spec.global_variables().end());
  for (const data::variable& variable : spec.global_variables())
  {
    S_i_X.erase(variable);
  }

  // Indicates that each assignment is the identity (lhs == lhs).
  data::assignment_list other_assignments = project(summand.assignments(), other_parameters);
  bool is_trivial = std::find_if(other_assignments.begin(),
    other_assignments.end(),
    [](const data::assignment& assignment)
    {
      return assignment.lhs() != assignment.rhs();
    }) == other_assignments.end();

  summand_dependencies result;
  result.S_i_X = S_i_X;
  result.in_L_X = is_trivial && set_1.empty() && std::includes(parameters.begin(), parameters.end(), set_3.begin(), set_3.end());
  return result;
}

/// \brief Creates a single summand for the dependency cleave process. We are always constructing the
///        left process, i.e. P_V, in the definition.
/// \returns A summand that should be added and else nothing.
template<bool owning = false>
inline
std::optional<lps::stochastic_action_summand> cleave_summand(
  const lps::stochastic_specification& spec,
  std::size_t summand_index,
  const data::variable_list& parameters,
  const data::variable_list& other_parameters,
  std::vector<process::action_label>& sync_labels,
  std::string syncname,
  const process::action& independent_tag,
  const process::action& internal_tag)
{
  lps::stochastic_action_summand summand = spec.process().action_summands()[summand_index];

  // Compute S_i_X depending on whether we are a left or right summand.
  summand_dependencies dependencies =
    owning ?
      compute_dependencies(spec, summand, parameters, other_parameters)
      : compute_dependencies(spec, summand, other_parameters, parameters);

  print_names(std::string("Dependencies of summand ") += std::to_string(summand_index) += std::string(" (S^i_X) := "), dependencies.S_i_X);

  // Add a summation for every parameter of the other process, and for every summation variable, that we depend on.
  data::variable_list variables = summand.summation_variables();
  for (const data::variable& variable : other_parameters)
  {
    if (dependencies.S_i_X.count(variable) > 0)
    {
      variables.push_front(variable);
    }
  }

  // Create the actsync(p, e_i) action for our dependencies on p and e_i
  data::data_expression_list values;
  data::sort_expression_list sorts;

  for (const data::variable& dependency : dependencies.S_i_X)
  {
    values.push_front(data::data_expression(dependency));
    sorts.push_front(dependency.sort());
  }

  sync_labels.emplace_back(syncname += std::string("_") += std::to_string(summand_index), sorts);

  data::assignment_list assignments = project(summand.assignments(), parameters);

  lps::multi_action action;
  if (owning)
  {
    // Here the action is performed by the current process.

    // Convert tau actions to a visible action.
    process::action_list internal;
    internal.push_front(internal_tag);

    process::action_list actions = (summand.multi_action() == lps::multi_action()) ?
      internal : summand.multi_action().actions();

    if (dependencies.in_L_X)
    {
      // This summand belongs to L_V.
      mCRL2log(log::verbose) << "Summand is independent, so " << summand_index << " in L_V.\n";
      actions.push_front(independent_tag);
    }
    else
    {
      // This summand depends on the other process.
      actions.push_front(process::action(sync_labels.back(), values));
    }

    action = lps::multi_action(actions);
  }
  else
  {
    summand_dependencies other_dependencies = compute_dependencies(spec, summand, other_parameters, parameters);
    if (other_dependencies.in_L_X)
    {
      mCRL2log(log::verbose) << "Summand is independent in the other component, so " << summand_index << " in L_W.\n";
      return {};
    }
    else
    {
      // The other process depends on our parameters and we do not perform state updates.
      process::action_list actions;
      actions.push_front(process::action(sync_labels.back(), values));
      action = lps::multi_action(actions);
    }
  }

  return lps::stochastic_action_summand(variables, summand.condition(), action, assignments, summand.distribution());
}

std::list<std::size_t> mcrl2::compute_independent_indices(
  const lps::stochastic_specification& spec,
  const data::variable_list& parameters)
{
  std::list<std::size_t> indices;
  auto other_parameters = get_other_parameters(spec.process(), parameters);

  // For each summand compute whether it is included in the set L_X.
  for (std::size_t index = 0; index < spec.process().action_summands().size(); ++index)
  {
    const lps::stochastic_action_summand& summand = spec.process().action_summands()[index];

    summand_dependencies dependencies = compute_dependencies(spec, summand, parameters, other_parameters);
    if (dependencies.in_L_X)
    {
      // This summand belongs to L_V.
      indices.emplace_back(index);
    }
  }

  return indices;
}

lps::stochastic_specification mcrl2::dependency_cleave(const lps::stochastic_specification& spec,
  const data::variable_list& parameters,
  const std::list<std::size_t>& indices,
  bool right_process)
{
  // Check sanity conditions, no timed or stochastic processes.
  const lps::stochastic_linear_process& process = spec.process();

  if (process.has_time())
  {
    throw runtime_error("Cleave does not support timed processes");
  }

  // The parameters of the "other" component process.
  data::variable_list other_parameters = get_other_parameters(process, parameters);

  // Extend the action specification with an actsync (that is unique) for every summand with the correct sorts, a tag and an internal action.
  std::vector<process::action_label> labels;

  // Add the tags for the left and right processes
  if (right_process)
  {
    labels.emplace_back(process::action_label("tagright", {}));
  }
  else
  {
    labels.emplace_back(process::action_label("tagleft", {}));
  }

  process::action tag(labels.back(), {});

  labels.emplace_back(process::action_label("internal", {}));
  process::action internal_tag(labels.back(), {});

  // Define the synchronization actions for the left and right components.
  std::string synclabel = right_process ? "syncright" : "syncleft";

  // Change the summands to include the parameters of the other process and added the sync action.
  lps::stochastic_action_summand_vector cleave_summands;

  // Add the summands that generate the action label.
  for (std::size_t index : indices)
  {
    if (index < process.action_summands().size())
    {
      auto summand = cleave_summand<true>(spec, index, parameters, other_parameters, labels, synclabel, tag, internal_tag);
      if (summand)
      {
        cleave_summands.emplace_back(summand.value());
      }
    }
    else
    {
      mCRL2log(log::warning) << "Index " << index << " is not a summand in this process.\n";
    }
  }

  for (std::size_t index : get_other_indices(process, indices))
  {
    auto summand = cleave_summand<false>(spec, index, parameters, other_parameters, labels, synclabel, tag, internal_tag);
    if (summand)
    {
      cleave_summands.emplace_back(summand.value());
    }
  }

  // Add the labels to the LPS action specification.
  auto cleave_action_labels = spec.action_labels();
  for (const process::action_label& label : labels)
  {
    cleave_action_labels.push_front(label);
  }

  lps::deadlock_summand_vector no_deadlock_summands;
  lps::stochastic_linear_process cleave_process(parameters, no_deadlock_summands, cleave_summands);

  lps::stochastic_process_initializer cleave_initial(
    project_values(data::make_assignment_list(spec.process().process_parameters(), spec.initial_process().expressions()), parameters),
    spec.initial_process().distribution());

  // Create the new LPS and return it.
  return lps::stochastic_specification(spec.data(), cleave_action_labels, spec.global_variables(), cleave_process, cleave_initial);
}

