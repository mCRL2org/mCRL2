// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "cleave.h"

#include "lpscleave_utility.h"
#include "split_condition.h"

#include <optional>
#include <vector>

using namespace mcrl2;

/// \brief Construct the summand for the given parameters.
static
lps::stochastic_action_summand make_summand(
  const lps::stochastic_action_summand& summand,
  std::set<data::variable>& synchronized,
  const data::variable_list& right_parameters,
  const data::variable_list& left_parameters, 
  process::action_list left_actions,
  process::action& left_tag, 
  process::action_label& left_sync,
  const cleave_condition& left_condition,
  bool is_independent,
  std::vector<process::action_label>& labels)
{
  // Add a summation variable for every parameter of the other process, and for every summation variable, that we depend on.
  data::variable_list left_variables = summand.summation_variables();
  for (const data::variable& variable : right_parameters)
  {
    if (synchronized.count(variable) > 0 && std::find(left_parameters.begin(), left_parameters.end(), variable) == left_parameters.end())
    {
      left_variables.push_front(variable);
    }
  }    

  data::data_expression_list left_values;
  for (const data::variable& dependency : synchronized)
  {
    left_values.push_front(data::data_expression(dependency));
  }

  for (const data::data_expression& expression : left_condition.implicit)
  {
    left_values.push_front(expression);
  }

  lps::multi_action action(left_actions);
  if (is_independent)
  {
    action.actions().push_front(process::action(left_tag));
  }
  else
  {
    labels.push_back(left_sync);
    action.actions().push_front(process::action(left_sync, left_values));
  }

  // Only update our parameters.
  data::assignment_list left_assignments = project(summand.assignments(), left_parameters);
  return lps::stochastic_action_summand(left_variables, left_condition.expression, action, left_assignments, summand.distribution());
}

/// \returns True iff each assignment is the identity (lhs == lhs) so only trivial updates.
bool is_update_trivial(
  const mcrl2::lps::stochastic_action_summand& summand,
  const mcrl2::data::variable_list& right_parameters)
{
  data::assignment_list left_assignments = project(summand.assignments(), right_parameters);

  return std::find_if(left_assignments.begin(),
    left_assignments.end(),
    [](const data::assignment& assignment)
    {
      return assignment.lhs() != assignment.rhs();
    }) == left_assignments.end();
}

std::pair<lps::stochastic_specification, lps::stochastic_specification> mcrl2::cleave(
  const lps::stochastic_specification& spec,
  const data::variable_list& left_parameters,
  const data::variable_list& right_parameters,
  std::list<std::size_t>& indices,
  bool enable_split_condition
  )
{
  // Check sanity conditions, no timed or stochastic processes.
  const lps::stochastic_linear_process& process = spec.process();

  if (process.has_time())
  {
    throw runtime_error("Cleave does not support timed processes");
  }

  // Add the tags for the left and right processes
  process::action_label tagright_label("tagright", {});
  process::action_label tagleft_label("tagleft", {});
  process::action_label internal_label("internal", {});

  // Change the summands to include the parameters of the other process and added the sync action.

  // Extend the action specification with an actsync (that is unique) for every summand with the correct sorts, a tag and an intern action.
  std::vector<process::action_label> labels;
  labels.push_back(tagright_label);
  labels.push_back(tagleft_label);
  labels.push_back(internal_label);

  // Create actions to be used within the summands.
  process::action tagleft(tagleft_label, {});
  process::action tagright(tagright_label, {});
  process::action taginternal(internal_label, {});

  process::action_list internal_action;
  internal_action.push_front(taginternal);

  // The summands of the two specifications.
  lps::stochastic_action_summand_vector left_summands;
  lps::stochastic_action_summand_vector right_summands;

  // Add the summands that generate the action label.
  for (std::size_t index = 0; index < process.action_summands().size(); ++index)
  {
    // The original summand.
    const auto& summand = process.action_summands()[index];

    /// 1. Split the multiaction.

    process::action_list left_action;
    process::action_list right_action;

    {
      // Indicates that the multi-action is (for now completely) generated in the left component.
      bool generate_left = (std::find(indices.begin(), indices.end(), index) != indices.end());

      // The resulting multi-action is either alpha or internal.
      process::action_list actions = (summand.multi_action() == lps::multi_action()) ? internal_action : summand.multi_action().actions();
      if (generate_left)
      {
        left_action = actions;
      }
      else
      {
        right_action = actions;
      }
    }

    // The dependencies for the multi-action.
    std::set<data::variable> left_action_dependencies;
    std::set<data::variable> right_action_dependencies;

    for (const process::action& action : left_action)
    {
      auto dependencies = data::find_free_variables(action.arguments());
      left_action_dependencies.insert(dependencies.begin(), dependencies.end());
    }

    for (const process::action& action : right_action)
    {
      auto dependencies = data::find_free_variables(action.arguments());
      right_action_dependencies.insert(dependencies.begin(), dependencies.end());
    }

    /// 2. Handle the condition requirements.

    // Each one stores the condition expression and implicit constraints (expressions h_i in the paper).
    cleave_condition left_condition;
    cleave_condition right_condition;

    if (enable_split_condition)
    {
      std::tie(left_condition, right_condition) = split_condition(summand.condition(), left_parameters, right_parameters, summand.summation_variables());
      mCRL2log(log::verbose) << "Split conditions into " << left_condition.expression << ", and " << right_condition.expression << "\n";
    }
    else
    {
      // Just copy the condition expressions.
      left_condition.expression = summand.condition();
      right_condition.expression = summand.condition();
    }

    std::set<data::variable> left_condition_dependencies = data::find_free_variables(left_condition.expression);
    std::set<data::variable> right_condition_dependencies = data::find_free_variables(right_condition.expression);

    // Add the dependencies of the clauses as well.
    for (const data::data_expression& clause : left_condition.implicit)
    {
      std::set<data::variable> dependencies = data::find_free_variables(clause);
      left_condition_dependencies.insert(dependencies.begin(), dependencies.end());
    }

    for (const data::data_expression& clause : right_condition.implicit)
    {
      std::set<data::variable> dependencies = data::find_free_variables(clause);
      right_condition_dependencies.insert(dependencies.begin(), dependencies.end());
    }

    /// 3. Handle update expressions

    // The dependencies for the update expressions (on other parameters).
    std::set<data::variable> left_update_dependencies;
    std::set<data::variable> right_update_dependencies;

    for (const data::assignment& assignment : summand.assignments())
    {
      auto dependencies = data::find_free_variables(assignment.rhs());
      if (std::find(left_parameters.begin(), left_parameters.end(), assignment.lhs()) != left_parameters.end())
      {
        // This is an assignment for the left component's parameters.
        left_update_dependencies.insert(dependencies.begin(), dependencies.end());
      }

      if (std::find(right_parameters.begin(), right_parameters.end(), assignment.lhs()) != right_parameters.end())
      {
        // This is an assignment for the right component's parameters.
        right_update_dependencies.insert(dependencies.begin(), dependencies.end());
      }
    }

    // Remove dependencies on our own parameter because these are not required.
    for (const data::variable& var : left_parameters)
    {
      left_update_dependencies.erase(var);
      left_condition_dependencies.erase(var);
      left_action_dependencies.erase(var);
    }

    for (const data::variable& var : right_parameters)
    {
      right_update_dependencies.erase(var);
      right_condition_dependencies.erase(var);
      right_action_dependencies.erase(var);
    }

    bool is_left_update_trivial = is_update_trivial(summand, left_parameters);
    bool is_right_update_trivial = is_update_trivial(summand, right_parameters);

    // Compute the synchronization vector (the values of h without functions)
    std::set<data::variable> synchronized;
    synchronized.insert(left_condition_dependencies.begin(), left_condition_dependencies.end());
    synchronized.insert(left_update_dependencies.begin(), left_update_dependencies.end());
    synchronized.insert(left_action_dependencies.begin(), left_action_dependencies.end());

    std::set<data::variable> right_synchronized;
    right_synchronized.insert(right_condition_dependencies.begin(), right_condition_dependencies.end());
    right_synchronized.insert(right_update_dependencies.begin(), right_update_dependencies.end());
    right_synchronized.insert(right_action_dependencies.begin(), right_action_dependencies.end());

    // Only keep summation variables if they occur in both conditions.
    for (const data::variable& var : summand.summation_variables())
    {
      if (std::find(synchronized.begin(), synchronized.end(), var) == synchronized.end())
      {
        right_synchronized.erase(var);
      }

      if (std::find(right_synchronized.begin(), right_synchronized.end(), var) == right_synchronized.end())
      {
        synchronized.erase(var);
      }

      // Remove the summation variables from the action dependencies, which can be done because the action is fully generated in one component.
      left_action_dependencies.erase(var);
      right_action_dependencies.erase(var);
    }

    synchronized.insert(right_synchronized.begin(), right_synchronized.end());

    // Create the actsync(p, e_i) action for our dependencies on p and e_i
    data::sort_expression_list sorts;

    for (const data::variable& dependency : synchronized)
    {
      sorts.push_front(dependency.sort());
    }

    for (const data::data_expression& expression : left_condition.implicit)
    {
      sorts.push_front(expression.sort());
    }

    // Determine which component is independent.
    atermpp::term_list<data::variable> left_all_parameters = left_parameters + summand.summation_variables();
    bool is_left_independent = is_right_update_trivial && is_subset(synchronized, left_all_parameters);

    atermpp::term_list<data::variable> right_all_parameters = right_parameters + summand.summation_variables();
    bool is_right_independent = is_left_update_trivial && is_subset(synchronized, right_all_parameters);

    mCRL2log(log::verbose) << std::boolalpha << "Summand " << index
      << ", is_left_update_trivial: " << is_left_update_trivial
      << ", is_right_update_trivial: " << is_right_update_trivial
      << ", is_left_independent: " << is_left_independent
      << ", is_right_independent: " << is_right_independent << ").\n";
    print_names("Dependencies", synchronized);

    /// 5. Add the necessary summands.

    // Add summand to the left specification.
    if (!is_right_independent)
    {
      process::action_label left_sync_label(std::string("syncleft_") += std::to_string(index), sorts);

      left_summands.push_back(make_summand(summand,
        synchronized,
        right_parameters,
        left_parameters,
        left_action,
        tagleft,
        left_sync_label,
        left_condition,
        is_left_independent,
        labels));
    }

    // Add summand to the right specification.
    if (!is_left_independent)
    {
      process::action_label right_sync_label(std::string("syncright_") += std::to_string(index), sorts);

      right_summands.push_back(make_summand(summand,
        synchronized,
        left_parameters,
        right_parameters,
        right_action,
        tagright,
        right_sync_label,
        right_condition,
        is_right_independent,
        labels));
    }
  }

  // Add the labels to the LPS action specification.
  auto action_labels = spec.action_labels();
  for (const process::action_label& label : labels)
  {
    action_labels.push_front(label);
  }

  lps::deadlock_summand_vector no_deadlock_summands;
  lps::stochastic_linear_process left_process(left_parameters, no_deadlock_summands, left_summands);
  lps::stochastic_linear_process right_process(right_parameters, no_deadlock_summands, right_summands);

  lps::stochastic_process_initializer left_initial(
    project_values(data::make_assignment_list(spec.process().process_parameters(), spec.initial_process().expressions()), left_parameters),
    spec.initial_process().distribution());

  lps::stochastic_process_initializer right_initial(
    project_values(data::make_assignment_list(spec.process().process_parameters(), spec.initial_process().expressions()), right_parameters),
    spec.initial_process().distribution());

  // Create the new LPS and return it.
  return std::make_pair(
    lps::stochastic_specification(spec.data(), action_labels, spec.global_variables(), left_process, left_initial),
    lps::stochastic_specification(spec.data(), action_labels, spec.global_variables(), right_process, right_initial));
}