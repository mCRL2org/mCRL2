// Author(s): Willem Rietdijk
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// \file lts_combine.cpp

#include "lts_combine.h"

#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/data/merge_data_specifications.h"
#include "mcrl2/lts/lts_algorithm.h"
#include "mcrl2/lts/lts_builder.h"
#include "mcrl2/lts/lts_io.h"
#include "mcrl2/lts/state_space_generator.h"
#include "mcrl2/process/merge_action_specifications.h"

#include <condition_variable>
#include <chrono>
#include <cstddef>
#include <mutex>
#include <queue>

using state_t = std::size_t;

using namespace mcrl2;

class identifier_string_compare
{
  core::identifier_string m_str;

public:
  identifier_string_compare(core::identifier_string str)
      : m_str(str)
  {}

  bool operator()(core::identifier_string str) { return str == m_str; }
};

/// \brief Utility function that returns the index of the synchronisation for which
///        the action_list actions matches the list of strings in syncs.second.
///
/// \param syncs The list of synchronisations
/// \param actions The list of actions to be matched.
/// \returns The index of the matching synchronisation, or max value when no matching synchronisation is found.
size_t get_sync(const std::vector<core::identifier_string_list>& syncs, const core::identifier_string_list& action_names)
{
  // A synchronising action must consist of two or more action labels
  if (action_names.size() < 2)
  {
    return std::numeric_limits<std::size_t>::max();
  }

  for (auto it = syncs.begin(); it != syncs.end(); it++)
  {
    if (*it == action_names)
    {
      return it - syncs.begin();
    }
  }

  return std::numeric_limits<std::size_t>::max();
}

/// \brief Convert and sort an action_list to an
/// identifier_string_list of names of the actions in the list.
///
/// \param actions The input list of actions.
/// \returns The sorted list of identifier strings.
core::identifier_string_list sorted_action_name_list(const process::action_list& actions)
{
  std::multiset<core::identifier_string> names;
  for (auto& action : actions)
  {
    names.insert(action.label().name());
  }

  return core::identifier_string_list(names.begin(), names.end());
}

/// \brief Checks if the given action list contains one of the blocked actions.
///
/// \param blocks The list of blocked actions.
/// \param actions The list of actions to be checked.
/// \returns Whether the list of actions contains a blocked action.
bool is_blocked(const std::vector<core::identifier_string>& blocks, const process::action_list& actions)
{
  // tau actions can not be blocked
  if (actions.empty() || blocks.empty())
  {
    return false;
  }

  for (const process::action& action : actions)
  {
    core::identifier_string action_name = action.label().name();

    for (const auto& it : blocks)
    {
      if (it == action_name)
      {
        return true;
      }
    }
  }

  // No matching blocked action could be found
  return false;
}

/// \brief Checks if the given action list matches one of the allowed
/// multi-actions. A match can only occur when the list of actions and allowed
/// multi-action are equal. If the list is empty, all actions are allowed.
///
/// \param allowed The list of allowed multi-actions.
/// \param actions The list of actions to be matched.
/// \returns Whether the list of actions is matched by an allowed multi-action
///          from the list of allowed multi-actions.
bool is_allowed(const std::vector<core::identifier_string_list>& allowed, const core::identifier_string_list& action_names)
{
  // If the list is empty, all actions are allowed
  // tau actions are always allowed
  if (allowed.empty() || action_names.empty())
  {
    return true;
  }

  for (const atermpp::term_list<atermpp::aterm_string>& action: allowed)
  {
    if (action == action_names)
    {
      return true;
    }
  }

  return false;
}

/// \brief Returns a new action label for which
/// the given actions are hidden.
///
/// \param tau_actions The action names to be hidden.
/// \param label The existing action label.
void hide_actions(const std::vector<core::identifier_string>& tau_actions, lps::multi_action& label)
{
  process::action_vector new_multi_action;
  for (const process::action& a : label.actions())
  {
    if (std::find_if(tau_actions.begin(),
            tau_actions.end(),
            identifier_string_compare(a.label().name()))
        == tau_actions.end()) // this action must not be hidden.
    {
      new_multi_action.push_back(a);
    }
  }
  
  label = lps::multi_action(process::action_list(new_multi_action.begin(), new_multi_action.end()));
}

/// \brief Checks whether the arguments of each of the actions of the
///        action_label are equal.
///
/// \param label The label to check.
/// \returns Whether all arguments of each action of the label
///          are equal.
bool can_sync(const lps::multi_action& label)
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

struct combined_lts_builder
{
  virtual ~combined_lts_builder() = default;
  virtual void finalize_combined(size_t states) = 0;
};

class combined_lts_lts_builder : public lts::lts_lts_builder, public combined_lts_builder
{
public:
  combined_lts_lts_builder(const data::data_specification& dataspec,
      const process::action_label_list& action_labels,
      const data::variable_list& process_parameters)
      : lts_lts_builder(dataspec, action_labels, process_parameters, true)
  {}

  void finalize_combined(size_t states) override
  {
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

class combined_lts_disk_builder : public lts::lts_lts_disk_builder, public combined_lts_builder
{
public:
  combined_lts_disk_builder(const std::string& filename,
      const data::data_specification& dataspec,
      const process::action_label_list& action_labels,
      const data::variable_list& process_parameters)
      : lts_lts_disk_builder(filename, dataspec, action_labels, process_parameters)
  {}

  void finalize_combined(size_t /* states */) override
  {
    // Write the initial state.
    lts::write_initial_state(*stream, 0);
  }
};

/// \brief A thread that computes the states of the LTS.
class state_thread
{
public:
  state_thread(std::vector<lts::lts_lts_t>& lts,
      std::vector<core::identifier_string_list>& syncs,
      std::vector<core::identifier_string>& resulting_actions,
      std::vector<core::identifier_string>& blocks,
      std::vector<core::identifier_string>& hiden,
      std::vector<core::identifier_string_list>& allow,
      std::vector<lts::outgoing_transitions_per_state_t>& outgoing_transitions)
      : lts(lts),
        syncs(syncs),
        resulting_actions(resulting_actions),
        blocks(blocks),
        hiden(hiden),
        allow(allow),
        outgoing_transitions(outgoing_transitions)
  {}

  void operator()(lts::lts_builder* lts_builder,
      std::queue<std::size_t>& queue,
      lts::detail::progress_monitor& progress_monitor,
      mcrl2::utilities::indexed_set<std::vector<state_t>>& states,
      std::size_t number_of_threads,
      std::mutex& lts_builder_mutex,
      std::mutex& queue_mutex,
      std::mutex& progress_mutex,
      std::mutex& states_mutex,
      std::condition_variable& queue_cond,
      std::size_t& busy)
  {
    while (true)
    {
      std::unique_lock<std::mutex> queue_lock(queue_mutex);
      
      // Wait if queue is empty but threads are still working
      while (queue.empty())
      {
        busy--;
        if (busy > 0)
        {
          // Some threads are still busy, wait for them to add work
          queue_cond.wait(queue_lock);
        }
        else
        {
          // All threads completed and queue is empty, exit
          return;
        }
      }
      
      busy++;

      // Process the state
      compute_state(lts_builder,
              queue,
              progress_monitor,
              states,
              number_of_threads,
              lts_builder_mutex,
              queue_lock,
              progress_mutex,
              states_mutex);
    }
  }

private:
  const std::vector<lts::lts_lts_t>& lts;
  const std::vector<core::identifier_string_list>& syncs;
  const std::vector<core::identifier_string>& resulting_actions;
  const std::vector<core::identifier_string>& blocks;
  const std::vector<core::identifier_string>& hiden;
  const std::vector<core::identifier_string_list>& allow;
  const std::vector<lts::outgoing_transitions_per_state_t>& outgoing_transitions;
  
  /// \returns True iff at least one state was computed.
  void compute_state(lts::lts_builder* lts_builder,
      std::queue<std::size_t>& queue,
      lts::detail::progress_monitor& progress_monitor,
      mcrl2::utilities::indexed_set<std::vector<state_t>>& states,
      const std::size_t& number_of_threads,
      std::mutex& lts_builder_mutex,
      std::unique_lock<std::mutex>& queue_lock,
      std::mutex& progress_mutex,
      std::mutex& states_mutex)
  {
    // Take the next pair from the queue.
    std::size_t state_index = queue.front();

    std::unique_lock state_lock(states_mutex);
    std::vector<state_t> state = states[state_index];    
    state_lock.unlock();

    queue.pop();
    queue_lock.unlock();

    // List of new outgoing transitions from this state, combined from the states
    // state[0] to state[i]
    std::vector<std::pair<lps::multi_action, std::vector<state_t>>> combos;

    // Loop over the old states contained in the new state
    for (size_t i = 0; i < state.size(); ++i)
    {
      state_t old_state = state[i];

      // Seperate new transitions from this state such that transitions from this state
      // don't combine with other transitions from this state
      std::vector<std::pair<lps::multi_action, std::vector<state_t>>> new_combos;

      // Loop over the outgoing transitions from the old state
      for (state_t t = outgoing_transitions[i].lowerbound(old_state); t < outgoing_transitions[i].upperbound(old_state);
           ++t)
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
        new_combos.emplace_back(label, old_states);

        // Add transition t to the existing multi-action from each combo
        for (auto& combo : combos)
        {
          // The new state of the combo already contains this state
          if (combo.second.size() >= i + 1)
          {
            continue;
          }

          // Copy states from combo to new state list
          std::vector<state_t> new_states;
          for (state_t s : combo.second)
          {
            new_states.push_back(s);
          }

          // Add new state for state i
          new_states.push_back(lts::to(transition));

          // Create new action label from multi-action of combo and transition t
          // lts::action_label_lts new_label(lps::multi_action(label.actions() + combo.first.actions()));

          new_combos.emplace_back(label + combo.first, new_states);
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
      mCRL2log(log::debug) << lps::pp(combo.first) << std::endl;

      size_t sync_index;
      core::identifier_string_list action_names = sorted_action_name_list(combo.first.actions());

      mCRL2log(log::debug) << core::pp(action_names) << std::endl;

      if (can_sync(combo.first) && (sync_index = get_sync(syncs, action_names)) != std::numeric_limits<std::size_t>::max())
      {
        // Synchronise
        core::identifier_string result_action = resulting_actions[sync_index];
        mCRL2log(log::debug) << "Sync: " << result_action << std::endl;

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
            lps::multi_action(process::action(process::action_label(result_action, sorts), arguments)));

        // Check if new transition is blocked or not allowed
        core::identifier_string_list new_action_names = sorted_action_name_list(new_label.actions());
        if (is_blocked(blocks, new_label.actions()) || !is_allowed(allow, new_action_names))
        {
          mCRL2log(log::debug) << "Blocked or not allowed: " << lps::pp(combo.first) << std::endl;
          continue;
        }

        // Hide actions in transition label
        hide_actions(hiden, new_label);

        std::unique_lock state_lock(states_mutex);
        // Add new state
        const auto [new_state, inserted] = states.insert(combo.second);
        state_lock.unlock();

        if (inserted)
        {
          queue_lock.lock();          
          queue.push(new_state);
          queue_lock.unlock();
        }

        // Add the transition with the remaining actions
        std::unique_lock progress_lock(progress_mutex);
        progress_monitor.examine_transition();
        progress_lock.unlock();

        std::unique_lock builder_lock(lts_builder_mutex);
        lts_builder->add_transition(state_index, new_label, new_state, number_of_threads);
      }
      else
      {
        // Normal multi-action
        mCRL2log(log::debug) << "Multi action" << std::endl;

        // Check if the transition is blocked or not allowed
        if (is_blocked(blocks, combo.first.actions()) || !is_allowed(allow, action_names))
        {
          mCRL2log(log::debug) << "Blocked or not allowed: " << lps::pp(combo.first) << std::endl;
          continue;
        }

        // Hide actions in transition label
        hide_actions(hiden, combo.first);

        // Add new state
        states_mutex.lock();
        const auto [new_state, inserted] = states.insert(combo.second);
        states_mutex.unlock();
        if (inserted)
        {
          queue_lock.lock();
          queue.push(new_state);
          queue_lock.unlock();
        }

        // Add the transition with the remaining actions
        progress_mutex.lock();
        progress_monitor.examine_transition();
        progress_mutex.unlock();

        lts_builder_mutex.lock();
        lts_builder->add_transition(state_index, combo.first, new_state, number_of_threads);
        lts_builder_mutex.unlock();
      }
    }
  }
};

void mcrl2::combine_lts(std::vector<lts::lts_lts_t>& lts,
    std::vector<core::identifier_string_list>& syncs,
    std::vector<core::identifier_string>& resulting_actions,
    std::vector<core::identifier_string>& blocks,
    std::vector<core::identifier_string>& hiden,
    std::vector<core::identifier_string_list>& allow,
    std::string filename,
    bool save_at_end,
    std::size_t nr_of_threads)
{
  // Calculate which states can be reached in a single outgoing step for both LTSs.
  std::vector<lts::outgoing_transitions_per_state_t> outgoing_transitions;
  for (const lts::lts_lts_t& input : lts)
  {
    outgoing_transitions.emplace_back(input.get_transitions(), input.num_states(), true);
  }

  // The parallel composition has pair of states that are stored in an indexed set (to keep track of processed states).
  mcrl2::utilities::indexed_set<std::vector<state_t>> states;
  std::vector<state_t> initial_states;
  process::action_label_list action_decls;
  data::data_specification data_spec;
  data::variable_list proc_params;
  for (auto& lts : lts)
  {
    initial_states.push_back(lts.initial_state());
    action_decls = process::merge_action_specifications(action_decls, lts.action_label_declarations());
    data_spec = data::merge_data_specifications(data_spec, lts.data());
    proc_params = proc_params + lts.process_parameters();
  }
  const auto [initial, found] = states.insert(initial_states);

  // The todo queue containing new found states.
  std::queue<std::size_t> queue;
  queue.push(initial);

  // Progress monitor.
  lts::detail::progress_monitor progress_monitor(lps::exploration_strategy::es_breadth);

  combined_lts_builder* combined_lts_builder;
  lts::lts_builder* lts_builder;
  if (save_at_end)
  {
    combined_lts_lts_builder* lts_lts_builder = new combined_lts_lts_builder(data_spec, action_decls, proc_params);
    lts_builder = lts_lts_builder;
    combined_lts_builder = lts_lts_builder;
  }
  else
  {
    combined_lts_disk_builder* lts_disk_builder
        = new combined_lts_disk_builder(filename, data_spec, action_decls, proc_params);
    lts_builder = lts_disk_builder;
    combined_lts_builder = lts_disk_builder;
  }

  std::mutex builder_mutex;
  std::mutex queue_mutex;
  std::mutex progress_mutex;
  std::mutex states_mutex;

  // Used to signal busy and empty queue.
  std::condition_variable queue_cond;
  std::size_t busy = nr_of_threads;

  std::vector<std::thread> threads;

  for (size_t i = 0; i < nr_of_threads; i++)
  {
    state_thread thread(lts, syncs, resulting_actions, blocks, hiden, allow, outgoing_transitions);
    threads.emplace_back(
        [&]
        {
          thread(lts_builder,
            queue,
            progress_monitor,
            states,
            nr_of_threads,
            builder_mutex,
            queue_mutex,
            progress_mutex,
            states_mutex,
            queue_cond,
            busy);
        });
  }

  for (size_t i = 0; i < nr_of_threads; i++)
  {
    threads[i].join();
  }

  progress_monitor.finish_exploration(states.size(), nr_of_threads);

  // Write the initial state and the state labels.
  combined_lts_builder->finalize_combined(states.size());
  lts_builder->save(filename);
  delete lts_builder;
}
