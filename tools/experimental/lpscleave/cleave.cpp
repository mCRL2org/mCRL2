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
#include "split_action.h"

#include <optional>
#include <vector>

using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::lps;

struct per_component_information
{
  bool is_independent;
  cleave_condition condition;
  process::action_list action;
};

/// This is the information stored for each summand before splitting.
struct per_summand_information
{
  per_component_information left;
  per_component_information right;
  std::set<data::variable> synchronized;
  std::size_t index;
};

/// \returns True iff each assignment is the identity (lhs := lhs) so only trivial updates.
bool is_update_trivial(
  const assignment_list& assignments,
  const mcrl2::data::variable_list& parameters)
{
  data::assignment_list left_assignments = project(assignments, parameters);

  return std::find_if(left_assignments.begin(),
    left_assignments.end(),
    [](const data::assignment& assignment)
    {
      return assignment.lhs() != assignment.rhs();
    }) == left_assignments.end();
}

/// Computes the conditions, actions and synchronization vector used to split
std::vector<per_summand_information> static_analysis(
  const lps::stochastic_action_summand_vector& summands,
  const data::variable_list& left_parameters,
  const data::variable_list& right_parameters,
  const std::list<std::size_t>& indices,
  bool enable_split_condition,
  bool enable_split_action)
{
  std::vector<per_summand_information> results(summands.size());

  for (std::size_t index = 0; index < summands.size(); ++index)
  {
    mCRL2log(log::debug) << "Analysing summand " << index << ":\n";
    const auto& summand = summands[index]; // The original summand.

    per_summand_information& result = results[index]; // The information for that summand.
    result.index = index; // Keep the original index.

    /// Handle the update expressions
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

    // Determine whether the update expressions are trivial (therefore only copy the current value).
    bool is_left_update_trivial = is_update_trivial(summand.assignments(), left_parameters);
    bool is_right_update_trivial = is_update_trivial(summand.assignments(), right_parameters);

    /// Determine whether the summand is independent in one of the two components.
    if (is_left_update_trivial || is_right_update_trivial)
    {
      std::set<data::variable> action_dependencies;
      for (const process::action& action : summand.multi_action().actions())
      {
        auto dependencies = data::find_free_variables(action.arguments());
        action_dependencies.insert(dependencies.begin(), dependencies.end());
      }

      std::set<data::variable> condition_dependencies = data::find_free_variables(summand.condition());

      // Determine which component is independent.
      atermpp::term_list<data::variable> left_all_parameters = left_parameters + summand.summation_variables();
      result.left.is_independent = is_right_update_trivial
        && is_subset(action_dependencies, left_all_parameters)
        && is_subset(condition_dependencies, left_all_parameters)
        && is_subset(left_update_dependencies, left_all_parameters);

      atermpp::term_list<data::variable> right_all_parameters = right_parameters + summand.summation_variables();
      result.right.is_independent = is_left_update_trivial
        && is_subset(action_dependencies, right_all_parameters)
        && is_subset(condition_dependencies, right_all_parameters)
        && is_subset(right_update_dependencies, right_all_parameters);

      if (result.left.is_independent)
      {
        result.left.condition.expression = summand.condition();
        result.left.action = summand.multi_action().actions();
        mCRL2log(log::verbose) << "Summand " << index << " is left independent\n";
      }

      if (result.right.is_independent)
      {
        result.right.condition.expression = summand.condition();
        result.right.action = summand.multi_action().actions();
        mCRL2log(log::verbose) << "Summand " << index << " is right independent\n";
      }
    }

    if (!result.left.is_independent && !result.right.is_independent)
    {

      /// Handle the action expression
      if (enable_split_action)
      {
        std::tie(result.left.action, result.right.action) = split_action(summand.multi_action().actions(), index, left_parameters, right_parameters, indices);
      }
      else
      {
        // Use the user-defined indices to indicate the choice.
        bool generate_left = (std::find(indices.begin(), indices.end(), index) != indices.end());

        if (generate_left)
        {
          result.left.action = summand.multi_action().actions();
        }
        else
        {
          result.right.action = summand.multi_action().actions();
        }
      }

      // The dependencies for the multi-action.
      std::set<data::variable> left_action_dependencies;
      std::set<data::variable> right_action_dependencies;

      for (const process::action& action : result.left.action)
      {
        auto dependencies = data::find_free_variables(action.arguments());
        left_action_dependencies.insert(dependencies.begin(), dependencies.end());
      }

      for (const process::action& action : result.right.action)
      {
        auto dependencies = data::find_free_variables(action.arguments());
        right_action_dependencies.insert(dependencies.begin(), dependencies.end());
      }

      /// Determine the synchronization vector.

      // Remove dependencies on our own parameter because these are not required.
      for (const data::variable& var : left_parameters)
      {
        left_update_dependencies.erase(var);
        left_action_dependencies.erase(var);
      }

      for (const data::variable& var : right_parameters)
      {
        right_update_dependencies.erase(var);
        right_action_dependencies.erase(var);
      }

      // Compute the synchronization vector (the values of h without functions)
      result.synchronized.insert(left_update_dependencies.begin(), left_update_dependencies.end());
      result.synchronized.insert(left_action_dependencies.begin(), left_action_dependencies.end());

      std::set<data::variable> right_synchronized;
      right_synchronized.insert(right_update_dependencies.begin(), right_update_dependencies.end());
      right_synchronized.insert(right_action_dependencies.begin(), right_action_dependencies.end());

      // Only keep summation variables in the synchronization if they occur in both action or update expressions.
      for (const data::variable& var : summand.summation_variables())
      {
        if (std::find(result.synchronized.begin(), result.synchronized.end(), var) == result.synchronized.end())
        {
          right_synchronized.erase(var);
        }

        if (std::find(right_synchronized.begin(), right_synchronized.end(), var) == right_synchronized.end())
        {
          result.synchronized.erase(var);
        }
      }

      result.synchronized.insert(right_synchronized.begin(), right_synchronized.end());

      /// Handle the condition expression.

      if (enable_split_condition)
      {
        std::tie(result.left.condition, result.right.condition) = split_condition(summand.condition(), left_parameters, right_parameters, summand.summation_variables(), result.synchronized);
      }
      else
      {
        // Just copy the condition expressions.
        result.left.condition.expression = summand.condition();
        result.right.condition.expression = summand.condition();
      }

      std::set<data::variable> left_condition_dependencies = data::find_free_variables(result.left.condition.expression);
      std::set<data::variable> right_condition_dependencies = data::find_free_variables(result.right.condition.expression);

      // Add the dependencies of the clauses as well.
      for (const data::data_expression& clause : result.left.condition.implicit)
      {
        std::set<data::variable> dependencies = data::find_free_variables(clause);
        left_condition_dependencies.insert(dependencies.begin(), dependencies.end());
      }

      for (const data::data_expression& clause : result.right.condition.implicit)
      {
        std::set<data::variable> dependencies = data::find_free_variables(clause);
        right_condition_dependencies.insert(dependencies.begin(), dependencies.end());
      }

      // Update the synchronization vector with the condition dependencies.
      // Remove dependencies on our own parameter because these are not required.
      for (const data::variable& var : left_parameters)
      {
        left_condition_dependencies.erase(var);
      }

      for (const data::variable& var : right_parameters)
      {
        right_condition_dependencies.erase(var);
      }

      result.synchronized.insert(left_condition_dependencies.begin(), left_condition_dependencies.end());
      result.synchronized.insert(right_condition_dependencies.begin(), right_condition_dependencies.end());
    }
  }

  return results;
}

bool can_be_merged(
  const lps::stochastic_action_summand& summand,
  const lps::stochastic_action_summand& other_summand,
  const per_component_information& info,
  const per_component_information& other_info,
  const data::variable_list& parameters)
{
  // Check if two summands have the same synchronization sorts.
  auto it = other_info.condition.implicit.begin();
  for (const data::data_expression& expression : info.condition.implicit)
  {
    if (it == other_info.condition.implicit.end())
    {
      return false;
    }

    if (expression.sort() != it->sort())
    {
      return false;
    }

    ++it;
  }

  // Check if assignments are the same.
  assignment_list left_assignments = project(summand.assignments(), parameters);
  assignment_list right_assignments = project(other_summand.assignments(), parameters);

  if (left_assignments != right_assignments)
  {
    return false;
  }

  if (info.action != other_info.action)
  {
    return false;
  }

  if (info.condition.expression != other_info.condition.expression)
  {
    return false;
  }

  return true;
}

/// A very expensive procedure to combine synchronization actions.
void merge_summands(const lps::stochastic_action_summand_vector& summands,
  const data::variable_list& left_parameters,
  const data::variable_list& right_parameters,
  std::vector<per_summand_information>& results)
{
  // Merge synchronization actions if possible.
  for (std::size_t i = 0; i < summands.size(); ++i)
  {
    const lps::stochastic_action_summand& summand = summands[i];
    const per_summand_information& info = results[i];

    for (std::size_t j = 0; j < summands.size(); ++j)
    {
      // The properties should be symmetrical so only only these comparisons are useful.
      if (i < j)
      {
        const lps::stochastic_action_summand& other_summand = summands[j];
        per_summand_information& other_info = results[j];

        if (info.synchronized == other_info.synchronized
          && !info.left.is_independent
          && !info.right.is_independent
          && !other_info.left.is_independent
          && !other_info.right.is_independent)
        {
          if (can_be_merged(summand, other_summand, info.left, other_info.left, left_parameters))
          {
            mCRL2log(log::verbose) << "Merged left summands " << i << " and " << j << ".\n";
            other_info.index = i;
          }

          if (can_be_merged(summand, other_summand, info.right, other_info.right, right_parameters))
          {
            mCRL2log(log::verbose) << "Merged right summands " << i << " and " << j << ".\n";
            other_info.index = i;
          }
        }        
      }
    }
  }
}


/// \brief Construct the summand for the given parameters.
lps::stochastic_action_summand create_summand(
  const lps::stochastic_action_summand& summand,
  const per_component_information& info,
  const std::set<data::variable>& synchronized,
  const process::action_label& sync,
  const data::variable_list& parameters, 
  const data::variable_list& other_parameters,
  const process::action& tag)
{
  // Add a summation variable for every parameter of the other process, and for every summation variable, that we depend on.
  data::variable_list left_variables = summand.summation_variables();
  for (const data::variable& variable : other_parameters)
  {
    if (synchronized.count(variable) > 0 && std::find(parameters.begin(), parameters.end(), variable) == parameters.end())
    {
      left_variables.push_front(variable);
    }
  }    

  data::data_expression_list left_values;
  for (const data::variable& dependency : synchronized)
  {
    left_values.push_front(data::data_expression(dependency));
  }

  for (const data::data_expression& expression : info.condition.implicit)
  {
    left_values.push_front(expression);
  }

  lps::multi_action action(info.action);
  if (info.is_independent)
  {
    action.actions().push_front(process::action(tag));
  }
  else
  {
    action.actions().push_front(process::action(sync, left_values));
  }

  // Only update our parameters.
  data::assignment_list left_assignments = project(summand.assignments(), parameters);
  return lps::stochastic_action_summand(left_variables, info.condition.expression, action, left_assignments, summand.distribution());
}

std::pair<lps::stochastic_specification, lps::stochastic_specification> mcrl2::cleave(
  const lps::stochastic_specification& spec,
  const data::variable_list& left_parameters,
  const data::variable_list& right_parameters,
  std::list<std::size_t>& indices,
  bool split_condition,
  bool split_action,
  bool merge_heuristic
  )
{
  // Check sanity conditions, no timed or stochastic processes.
  const lps::stochastic_linear_process& process = spec.process();

  if (process.has_time())
  {
    throw runtime_error("Cleave does not support timed processes");
  }

  // The 'tag' action is used to tag independent actions.
  process::action_label tag_label("tag", {});

  // The list of action labels that are added to the specification.
  std::vector<process::action_label> labels;
  labels.push_back(tag_label);

  std::vector<per_summand_information> results = static_analysis(process.action_summands(), left_parameters, right_parameters, indices, split_condition, split_action);

  // Create actions to be used within the summands.
  process::action tag(tag_label, {});

  // The summands of the two LPSs after the cleave.
  lps::stochastic_action_summand_vector left_summands;
  lps::stochastic_action_summand_vector right_summands;

  if (merge_heuristic)
  {
    merge_summands(process.action_summands(), left_parameters, right_parameters, results);
  }

  for (std::size_t index = 0; index < results.size(); ++index)
  {
    const auto& summand = process.action_summands()[index];
    const per_summand_information& result = results[index];

    // Create the actsync(p, e_i) action for our dependencies on p and e_i
    data::sort_expression_list sorts;

    for (const data::variable& dependency : result.synchronized)
    {
      sorts.push_front(dependency.sort());
    }

    for (const data::data_expression& expression : result.left.condition.implicit)
    {
      sorts.push_front(expression.sort());
    }

    // Add summand to the left specification.
    if (!result.right.is_independent || result.left.is_independent)
    {
      process::action_label left_sync_label(std::string("syncleft_") += std::to_string(result.index), sorts);
      if (!result.left.is_independent && result.index == index)
      {
        // Add this synchronization label when the sync action is used and we are the summand with its own index (and not merged).
        labels.push_back(left_sync_label);
      }

      left_summands.push_back(create_summand(summand,
        result.left,
        result.synchronized,
        left_sync_label,
        left_parameters,
        right_parameters,
        tag));
    }

    // Add summand to the right specification.
    if (!result.left.is_independent || result.right.is_independent)
    {
      process::action_label right_sync_label(std::string("syncright_") += std::to_string(result.index), sorts);      
      if (!result.right.is_independent && result.index == index)
      {
        labels.push_back(right_sync_label);
      }

      right_summands.push_back(create_summand(summand,
        result.right,
        result.synchronized,
        right_sync_label,
        right_parameters,
        left_parameters,
        tag));
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
