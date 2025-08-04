// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "cleave.h"

#include "mcrl2/data/substitutions/mutable_map_substitution.h"

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
  bool is_independent = false;
  cleave_condition condition;
  process::action_list action;
};

/// This is the information stored for each summand before splitting.
struct per_summand_information
{
  per_component_information left;
  per_component_information right;
  std::set<data::variable> synchronised;
  std::size_t index = 0UL;
};

struct per_variable_information
{
  bool occurs_unbounded;
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
  std::map<variable, per_variable_information> variable_info;

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
      }

      if (result.right.is_independent)
      {
        result.right.condition.expression = summand.condition();
        result.right.action = summand.multi_action().actions();
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

      // Compute the synchronization vector (the values of h without functions)
      result.synchronised.insert(left_update_dependencies.begin(), left_update_dependencies.end());
      result.synchronised.insert(left_action_dependencies.begin(), left_action_dependencies.end());

      std::set<data::variable> right_synchronised;
      right_synchronised.insert(right_update_dependencies.begin(), right_update_dependencies.end());
      right_synchronised.insert(right_action_dependencies.begin(), right_action_dependencies.end());

      // Remove dependencies on our own parameter because these are not required.
      for (const data::variable& var : left_parameters)
      {
        result.synchronised.erase(var);
      }

      for (const data::variable& var : right_parameters)
      {
        right_synchronised.erase(var);
      }

      /// Handle the condition expression.
      if (enable_split_condition)
      {
        // Merge the dependencies for the splitting procedure.
        std::set<data::variable> synchronised = result.synchronised;
        synchronised.insert(right_synchronised.begin(), right_synchronised.end());
        std::tie(result.left.condition, result.right.condition) = split_condition(summand.condition(), left_parameters, right_parameters, summand.summation_variables(), synchronised);
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
      result.synchronised.insert(left_condition_dependencies.begin(), left_condition_dependencies.end());
      right_synchronised.insert(right_condition_dependencies.begin(), right_condition_dependencies.end());

      // Remove dependencies on our own parameter because these are not required.
      for (const data::variable& var : left_parameters)
      {
        result.synchronised.erase(var);
      }

      for (const data::variable& var : right_parameters)
      {
        right_synchronised.erase(var);
      }

      // Only keep summation variables in the synchronization if they occur in both action or update expressions.
      for (const data::variable& var : summand.summation_variables())
      {
        if (std::find(result.synchronised.begin(), result.synchronised.end(), var) == result.synchronised.end())
        {
          right_synchronised.erase(var);
        }

        if (right_synchronised.find(var) == right_synchronised.end())
        {
          result.synchronised.erase(var);
        }
      }

      // Find problematic parameters.
      for (const data::variable& var : result.synchronised)
      {
        if (left_condition_dependencies.find(var) == left_condition_dependencies.end())
        {
          variable_info[var].occurs_unbounded = true;
        }
      }

      for (const data::variable& var : right_synchronised)
      {
        if (right_condition_dependencies.find(var) == right_condition_dependencies.end())
        {
          variable_info[var].occurs_unbounded = true;
        }
      }

      result.synchronised.insert(right_synchronised.begin(), right_synchronised.end());

      mCRL2log(log::debug) << "Synchronisation vector: ";
      print_names(log::debug, result.synchronised);
      mCRL2log(log::debug) << "\n";
    }
  }

  // Track the number of independent summands, and the number of times that each parameter occurs in a synchronisation vector.
  std::size_t nof_left_independent = 0;
  std::size_t nof_right_independent = 0;
  std::map<variable, std::size_t> synchronised_count;

  for (const variable& var : left_parameters)
  {
    synchronised_count[var] = 0;
  }  
  
  for (const variable& var : right_parameters)
  {
    synchronised_count[var] = 0;
  }

  // Count the number of times a variable occurs in the synchronisation vector (either explicit or implicitly).
  for (const per_summand_information& info : results)
  {
    nof_left_independent += info.left.is_independent;
    nof_right_independent += info.right.is_independent;

    for (const variable& var : info.synchronised)
    {
      synchronised_count[var] += 1;
    }

    for (const data_expression& expr : info.left.condition.implicit)
    {
      for (const variable& var : data::find_free_variables(expr))
      {
        synchronised_count[var] += 1;
      }
    }    
    
    for (const data_expression& expr : info.right.condition.implicit)
    {
      for (const variable& var : data::find_free_variables(expr))
      {
        synchronised_count[var] += 1;
      }
    }
  }

  // Present the information to the user.
  mCRL2log(log::verbose) << "There are " << nof_left_independent << " left independent, and " << nof_right_independent << " right independent summands.\n";
  mCRL2log(log::verbose) << "Parameters synchronised more than once:\n";

  for (const auto& [var, count] : synchronised_count)
  {
    if (count > 0)
    {
      mCRL2log(log::verbose) << "Variable " << var.name() << " occurs " << count << " times.\n";
    }
  }

  for (const auto& [var, info] : variable_info)
  {
    if (info.occurs_unbounded)
    {
       mCRL2log(log::verbose) << "Parameter " << var.name() << " occurs unbounded.\n";
    }
  }

  return results;
}

/// Determine whether two summands have (syntactically) the same condition, action and assignment expressions.
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
      // The properties should be symmetrical so only only these comparisons are useful. Furthermore, only check if this summand is not already merged.
      if (i < j && info.index == i)
      {
        const lps::stochastic_action_summand& other_summand = summands[j];
        per_summand_information& other_info = results[j];

        if (info.synchronised == other_info.synchronised
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
  const std::set<data::variable>& synchronised,
  const process::action_label& sync,
  const data::variable_list& parameters, 
  const data::variable_list& other_parameters,
  const process::action& tag,
  const data::data_expression& invariant)
{
  // The condition expression.
  data::data_expression condition = info.condition.expression;

  // The action with possible synchronisation or tags.
  data::data_expression_list sync_values;
  for (const data::variable& dependency : synchronised)
  {
    sync_values.push_front(data::data_expression(dependency));
  }

  for (const data::data_expression& expression : info.condition.implicit)
  {
    sync_values.push_front(expression);
  }

  lps::multi_action action(info.action);
  process::action_list al=action.actions();
  if (info.is_independent)
  {
    al.push_front(process::action(tag));
  }
  else
  {
    al.push_front(process::action(sync, sync_values));
  }
  action = multi_action(al,action.time());

  // The update expression.
  data::assignment_list assignments = project(summand.assignments(), parameters);

  // Gather dependencies to determine which sum variables are important.
  std::set<data::variable> dependencies = data::find_free_variables(condition);
  std::set<data::variable> update_dependencies = data::find_free_variables(assignments);

  std::set<data::variable> action_dependencies;
  for (const process::action& action : action.actions())
  {
    auto dependencies = data::find_free_variables(action.arguments());
    action_dependencies.insert(dependencies.begin(), dependencies.end());
  }

  dependencies.insert(action_dependencies.begin(), action_dependencies.end());
  dependencies.insert(update_dependencies.begin(), update_dependencies.end());

  data::variable_list sum_variables;
  for (const data::variable& variable : summand.summation_variables())
  {
    if (dependencies.count(variable) != 0)
    {
      sum_variables.push_front(variable);
    }
  }

  // Add a sum variable for every parameter of the other process that we depend on based on the synchronisation.
  for (const data::variable& variable : other_parameters)
  {
    if (synchronised.count(variable) > 0 && std::find(parameters.begin(), parameters.end(), variable) == parameters.end())
    {
      sum_variables.push_front(variable);
    }
  }

  // Strengthen the condition expression if an invariant is present.
  if (invariant != data::data_expression())
  {
    // There are extra free variables introduced by the invariant, for which we introduce existential quantifiers.
    std::set<data::variable> invariant_dependencies = data::find_free_variables(invariant);
    std::set<data::variable> existential;

    for (const data::variable& variable : invariant_dependencies)
    {
      // If they occur in the synchronisation then these are added as summand variables (and as such not needed here).
      if (std::find(parameters.begin(), parameters.end(), variable) == parameters.end()
           && std::find(sum_variables.begin(), sum_variables.end(), variable) == sum_variables.end())
      {
        existential.insert(variable);
      }
    }

    // Strengthen the condition itself with the invariant.
    condition = data::sort_bool::and_(condition, invariant);

    // Only add if new variables would be introduced.
    if (!existential.empty())
    {
      condition = data::exists(existential, condition);
    }
  }

  return lps::stochastic_action_summand(sum_variables, condition, action, assignments, summand.distribution());
}

std::pair<lps::stochastic_specification, lps::stochastic_specification> mcrl2::cleave(
  const lps::stochastic_specification& spec,
  const data::variable_list& left_parameters,
  const data::variable_list& right_parameters,
  const std::list<std::size_t>& indices,
  const data::data_expression& invariant,
  const std::string& action_prefix,
  bool split_condition,
  bool split_action,
  bool merge_heuristic,
  bool use_next_state
  )
{
  // Check sanity conditions, no timed or stochastic processes.
  const lps::stochastic_linear_process& process = spec.process();

  if (process.has_time())
  {
    throw runtime_error("Cleave does not support timed processes");
  }

  // The 'tag' action is used to tag independent actions.
  process::action_label tag_label(std::string(action_prefix) += "tag", {});

  // The list of action labels that are added to the specification.
  std::vector<process::action_label> labels;
  labels.push_back(tag_label);

  // Perform the static analysis.
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

    for (const data::variable& dependency : result.synchronised)
    {
      sorts.push_front(dependency.sort());
    }

    for (const data::data_expression& expression : result.left.condition.implicit)
    {
      sorts.push_front(expression.sort());
    }

    data_expression updated_invariant = invariant;
    if (use_next_state)
    {
      // Construct a substitution for this update expression.
      data::mutable_map_substitution<std::map<data::variable, data::data_expression>> subst;
      for (const data::assignment& assignment : summand.assignments())
      {
        subst[assignment.lhs()] = assignment.rhs();
      }

      updated_invariant = data::replace_variables(invariant, subst);
    }

    // Add summand to the left specification.
    if (!result.right.is_independent || result.left.is_independent)
    {
      std::string sync_name(action_prefix);
      sync_name += std::string("syncleft_") += std::to_string(result.index);

      process::action_label left_sync_label(sync_name, sorts);
      if (!result.left.is_independent && result.index == index)
      {
        // Add this synchronization label when the sync action is used and we are the summand with its own index (and not merged).
        labels.push_back(left_sync_label);
      }

      left_summands.push_back(create_summand(summand,
        result.left,
        result.synchronised,
        left_sync_label,
        left_parameters,
        right_parameters,
        tag,
        updated_invariant));
    }

    // Add summand to the right specification.
    if (!result.left.is_independent || result.right.is_independent)
    {
      std::string sync_name(action_prefix);
      sync_name += std::string("syncright_") += std::to_string(result.index);

      process::action_label right_sync_label(sync_name, sorts);      
      if (!result.right.is_independent && result.index == index)
      {
        labels.push_back(right_sync_label);
      }

      right_summands.push_back(create_summand(summand,
        result.right,
        result.synchronised,
        right_sync_label,
        right_parameters,
        left_parameters,
        tag,
        updated_invariant));
    }
  }

  // Add the labels to the LPS action specification.
  auto action_labels = spec.action_labels();
  for (const process::action_label& label : labels)
  {
    // Abort when any of these labels already exists.
    if (std::find(action_labels.begin(), action_labels.end(), label) != action_labels.end())
    {
      mCRL2log(log::error) << "The action label " << label << " is already present in the specification.\n";
      mCRL2log(log::error) << "This means that we cannot ensure that this composition is valid.\n";
      throw mcrl2::runtime_error("Aborted decomposition due to an error.");
    }

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
