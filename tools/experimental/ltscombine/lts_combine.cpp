// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "lts_combine.h"

#include "mcrl2/lts/lts_algorithm.h"
#include "mcrl2/lts/lts_io.h"
#include "mcrl2/lts/lts_builder.h"
#include "mcrl2/lts/state_space_generator.h"
#include "mcrl2/atermpp/aterm_list.h"

#include <queue>
#include <chrono>

using state_t = std::size_t;

using namespace mcrl2;

class identifier_string_compare
{
  core::identifier_string m_str;

  public:
    identifier_string_compare(core::identifier_string str) : m_str(str) { }

    bool operator()(core::identifier_string str)
    { 
      return str == m_str;
    }
};

class identifier_string_list_compare
{
  core::identifier_string m_str_list;

public:
  identifier_string_list_compare(core::identifier_string_list str_list)
      : m_str_list(str_list)
  {}

  bool operator()(core::identifier_string_list str_list) { return str_list == m_str_list; }
};

class action_list_to_identifier_list
{
public:
  core::identifier_string operator()(process::action action) { return action.label().name(); }
};

/// <summary>
/// Utility function that returns the index of the synchronisation for which
/// the action_list actions matches the list of strings in syncs.second.
/// </summary>
/// <param name="syncs">The list of synchronisations.</param>
/// <param name="actions">The list of actions to be matched.</param>
/// <returns>The index of the matching synchronisation,
/// or -1 when no matching synchronisation is found.</returns>
size_t get_sync(std::vector<std::pair<core::identifier_string, std::vector<core::identifier_string>>>& syncs,
    process::action_list actions)
{
  // Loop through possible synchronisations
  for (size_t i = 0; i < syncs.size(); i++)
  {
    auto& sync = syncs[i];
    if (actions.size() != sync.second.size())
    {
      // Multi-action must exactly match the list of actions
      continue;
    }
    
    // Boolean vector for each action in the multi-action, and one
    // extra to signal that an action could not be found in the multi-action
    std::vector<bool> synced(sync.second.size() + 1, false);
    synced[0] = true;

    for (const process::action& action : actions)
    {
      // Find action in the multi-action
      std::vector<core::identifier_string>::iterator iter = sync.second.begin();
      while ((iter = std::find_if(iter, sync.second.end(), identifier_string_compare(action.label().name()))) != sync.second.end())
      {
        int index = iter - sync.second.begin() + 1;
        // Check if found action is not already used
        if (!synced[index])
        {
          synced[index] = true;
          break;
        }
        iter++;
      }

      if (iter == sync.second.end())
      {
        // Action could not be found, or all were already in use
        synced[0] = false;
        break;
      }
    }

    if (synced[0])
    {
      // Because all actions are found and the multi-action and
      // action list are of equal length, they must be equal
      return i;
    }
  }

  // No matching synchronisation was found
  return -1;
}

/// <summary>
/// Checks if the given action list contains one of the blocked actions.
/// </summary>
/// <param name="blocks">The list of blocked actions.</param>
/// <param name="actions">The list of actions to be checked.</param>
/// <returns>Whether the list of actions contains a blocked action.</returns>
bool is_blocked(std::vector<core::identifier_string> blocks, process::action_list actions)
{
  // tau actions can not be blocked
  if (actions.empty())
  {
    return false;
  }

  for (const process::action& action : actions)
  {
    if (std::find_if(blocks.begin(), blocks.end(), identifier_string_compare(action.label().name())) != blocks.end())
    {
      return true;
    }
  }

  // No matching blocked action could be found
  return false;
}

core::identifier_string action_name(const process::action& action) {
  return action.label().name();
}

/// <summary>
/// Checks if the given action list matches one of the allowed multi-actions.
/// A match can only occur when the list of actions and allowed multi-action 
/// are equal. If the list is empty, all actions are allowed.
/// </summary>
/// <param name="allowed">The list of allowed multi-actions.</param>
/// <param name="actions">The list of actions to be matched.</param>
/// <returns>Whether the list of actions is matched by an allowed multi-action
/// from the list of allowed multi-actions.</returns>
bool is_allowed(const std::vector<core::identifier_string_list> allowed, process::action_list actions)
{
  // If the list is empty, all actions are allowed
  // tau actions are always allowed
  if (allowed.empty() || actions.empty())
  {
    return true;
  }

  std::vector<core::identifier_string> names(actions.size());
  std::transform(actions.begin(), actions.end(), names.begin(), action_name);
  std::sort(names.begin(), names.end(), [](core::identifier_string a1, core::identifier_string a2) { return a1 < a2; });

  core::identifier_string_list action_names = core::identifier_string_list::term_list(names.begin(), names.end());

  if (std::find_if(allowed.begin(), allowed.end(), identifier_string_list_compare(action_names)) != allowed.end())
  {
    return true;
  }

  return false;
}

/// <summary>
/// Returns a new action label for which the given actions are hidden.
/// </summary>
/// <param name="tau_actions">The action names to be hidden.</param>
/// <param name="label">The existing action label.</param>
void hide_actions(const std::vector<core::identifier_string>& tau_actions, lts::action_label_lts* label)
{
  process::action_vector new_multi_action;
  for (const process::action& a : label->actions())
  {
    if (std::find_if(tau_actions.begin(),
            tau_actions.end(),
            identifier_string_compare(a.label().name()))
        == tau_actions.end()) // this action must not be hidden.
    {
      new_multi_action.push_back(a);
    }
  }
  *label = lts::action_label_lts(
      lps::multi_action(process::action_list(new_multi_action.begin(), new_multi_action.end())));
}

/// <summary>
/// Checks whether the arguments of each of the actions of the
/// action_label are equal.
/// </summary>
/// <param name="label">The label to check.</param>
/// <returns>Whether all arguments of each action of the label
/// are equal.</returns>
bool can_sync(const lts::action_label_lts& label)
{
  // Check if each action's arguments are equal to the first action's arguments
  for (auto& action : label.actions())
  {
    if (action.arguments() != label.actions().front().arguments())
    {
      return false;
    }
  }

  return true;
}

class combined_lts_builder : public lts::lts_lts_builder
{
public:
  combined_lts_builder(
    const data::data_specification& dataspec,
    const process::action_label_list& action_labels,
    const data::variable_list& process_parameters)
      : lts_lts_builder(dataspec, action_labels, process_parameters, true)
  { }

  void finalize(size_t states) {
    // add actions
    m_lts.set_num_action_labels(m_actions.size());
    for (const auto& p : m_actions)
    {
      m_lts.set_action_label(p.second, lts::action_label_lts(lps::multi_action(p.first.actions(), p.first.time())));
    }

    m_lts.set_num_states(states, true);
    m_lts.set_initial_state(0);
  }
};

// Combine two LTSs resulting from the state space exploration of LPSs of lpscleave into a single LTS.
void mcrl2::combine_lts(std::vector<lts::lts_lts_t>& lts,
  std::vector<std::pair<core::identifier_string, std::vector<core::identifier_string>>>& syncs,
  std::vector<core::identifier_string> blocks,
  std::vector<core::identifier_string> hiden,
  std::vector<core::identifier_string_list> allow,
  std::ostream& stream, std::string filename)
{
  // Calculate which states can be reached in a single outgoing step for both LTSs.
  std::vector<lts::outgoing_transitions_per_state_t> outgoing_transitions;
  for (size_t i = 0; i < lts.size(); i++)
  {
    outgoing_transitions.push_back(lts::outgoing_transitions_per_state_t(lts[i].get_transitions(), lts[i].num_states(), true));
  }

  // The parallel composition has pair of states that are stored in an indexed set (to keep track of processed states).
  mcrl2::utilities::indexed_set<std::vector<state_t>> states;
  std::vector<state_t> initial_states;
  for (auto& lts : lts)
  {
    initial_states.push_back(lts.initial_state());
  }
  const auto [initial, found] = states.insert(initial_states);

  // The todo queue containing new found states.
  std::queue<std::size_t> queue;
  queue.push(initial);

  // Progress monitor.
  lts::detail::progress_monitor progress_monitor(lps::exploration_strategy::es_breadth);

  // Start writing the LTS.
  //atermpp::binary_aterm_ostream output(stream);
  //lts::write_lts_header(output, lts[0].data(), lts[0].process_parameters(), lts[0].action_label_declarations());

  combined_lts_builder lts_builder(lts[0].data(), lts[0].action_label_declarations(), lts[0].process_parameters());

  while (!queue.empty())
  {
    // Take the next pair from the queue.
    std::size_t state_index = queue.front();
    std::vector<state_t> state = states[state_index];

    queue.pop();

    // List of new outgoing transitions from this state, combined from the states
    // state[0] to state[i]
    std::vector<std::pair<lts::action_label_lts, std::vector<state_t>>> combos;

    // Loop over the old states contained in the new state
    for (size_t i = 0; i < state.size(); ++i)
    {
      state_t old_state = state[i];

      // Seperate new transitions from this state such that transitions from this state 
      // don't combine with other transitions from this state
      std::vector<std::pair<lts::action_label_lts, std::vector<state_t>>> new_combos;

      // Loop over the outgoing transitions from the old state
      for (state_t t = outgoing_transitions[i].lowerbound(old_state); t < outgoing_transitions[i].upperbound(old_state); ++t)
      {
        const lts::outgoing_pair_t& transition = outgoing_transitions[i].get_transitions()[t];
        const lts::action_label_lts& label = lts[i].action_label(lts::label(transition));

        // Add transition t, from state to [state[0], ..., state[i-1], to(state[i])]
        std::vector<state_t> old_states;
        // Preserve old states for states 0..i-1
        for (size_t j = 0; j < i; ++j)
        {
          old_states.push_back(state[j]);
        }
        // Add new state for state i
        old_states.push_back(lts::to(transition));
        new_combos.push_back(std::make_pair(label, old_states));

        // Add transition t to the existing multi-action from each combo
        for (auto& combo : combos)
        {
          // The new state of the combo already contains this state
          if (combo.second.size() >= i+1)
          {
            continue;
          }

          if (!is_blocked(blocks, label.actions() + combo.first.actions()))
          {
            // Copy states from combo to new state list
            std::vector<state_t> new_states;
            for (state_t s : combo.second)
            {
              new_states.push_back(s);
            }

            // Add new state for state i
            new_states.push_back(lts::to(transition));

            // Create new action label from multi-action of combo and transition t
            lts::action_label_lts new_label(lps::multi_action(label.actions() + combo.first.actions()));

            new_combos.push_back(std::make_pair(new_label, new_states));
          }
        }
      }

      // For each existing multi-action, add the old state of state i
      // to simulate no transition being taken
      for (auto& combo : combos)
      {
        // The new state of the combo already contains this state
        if (combo.second.size() >= i + 1)
        {
          continue;
        }

        // Add current state, thus no transition
        combo.second.push_back(old_state);
      }

      // Finished state i
      for (auto& combo : new_combos)
      {
        combos.push_back(combo);
      }
    }

    // Finished generating all new transitions, add them to the LTS
    for (auto& combo : combos)
    {
      mCRL2log(log::debug) << lts::pp(combo.first) << std::endl;

      size_t sync_index;
      if (can_sync(combo.first) && (sync_index = get_sync(syncs, combo.first.actions())) != -1)
      {
        // Synchronise
        mCRL2log(log::debug) << "Sync: " << syncs[sync_index].first << std::endl;
        
        data::data_expression_list arguments;
        data::sort_expression_list sorts;
        if (!combo.first.actions().empty())
        {
          // Only use arguments and sorts if the action is not a tau action
          arguments = combo.first.actions().front().arguments();
          sorts = combo.first.actions().front().label().sorts();
        }

        // Create new label from the synchronisation
        lts::action_label_lts new_label(
            lps::multi_action(process::action(process::action_label(syncs[sync_index].first, sorts), arguments)));

        // Check if new transition is blocked or not allowed
        if (is_blocked(blocks, new_label.actions()) || !is_allowed(allow, new_label.actions()))
        {
          mCRL2log(log::debug) << "Blocked or not allowed: " << lts::pp(combo.first) << std::endl;
          continue;
        }

        // Hide actions in transition label
        hide_actions(hiden, &new_label);

        // Add new state
        const auto [new_state, inserted] = states.insert(combo.second);
        if (inserted)
        {
          queue.push(new_state);
        }

        // Add the transition with the remaining actions
        progress_monitor.examine_transition();
        
        //lts::write_transition(output, state_index, new_label, new_state);
        lts_builder.add_transition(state_index, new_label, new_state, 1);
      }
      else {
        // Normal multi-action
        mCRL2log(log::debug) << "Multi action" << std::endl;

        // Check if the transition is blocked or not allowed
        if (is_blocked(blocks, combo.first.actions()) || !is_allowed(allow, combo.first.actions()))
        {
          mCRL2log(log::debug) << "Blocked or not allowed: " << lts::pp(combo.first) << std::endl;
          continue;
        }

        // Hide actions in transition label
        hide_actions(hiden, &combo.first);

        // Add new state
        const auto [new_state, inserted] = states.insert(combo.second);
        if (inserted)
        {
          queue.push(new_state);
        }

        // Add the transition with the remaining actions
        progress_monitor.examine_transition();
        
        //lts::write_transition(output, state_index, combo.first, new_state);
        lts_builder.add_transition(state_index, combo.first, new_state, 1);
      }
    }

    progress_monitor.finish_state(states.size(), queue.size(), 1);
  }

  progress_monitor.finish_exploration(states.size(), 1);

  // Write the initial state and the state labels.
  //lts::write_initial_state(output, 0);
  lts_builder.finalize(states.size());
  lts_builder.save(filename);
}
