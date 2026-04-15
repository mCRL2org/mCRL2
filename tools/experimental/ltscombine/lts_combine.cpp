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
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/data/merge_data_specifications.h"
#include "mcrl2/lps/linearise_allow_block.h"
#include "mcrl2/lps/linearise_hide.h"
#include "mcrl2/lts/lts_algorithm.h"
#include "mcrl2/lts/lts_builder.h"
#include "mcrl2/lts/lts_io.h"
#include "mcrl2/lts/state_space_generator.h"
#include "mcrl2/process/communication_expression.h"
#include "mcrl2/process/merge_action_specifications.h"
#include "mcrl2/process/process_expression.h"

#include <condition_variable>
#include <chrono>
#include <cstddef>
#include <mutex>
#include <queue>

using state_t = std::size_t;

using namespace mcrl2;

inline
core::identifier_string_list get_action_names(const process::action_list& actions)
{
  return core::identifier_string_list(actions.begin(), actions.end(), [](const process::action& a) { return a.label().name(); });
}

/// \brief Utility function that returns the index of the synchronisation for which
///        the action_list actions matches the list of strings in syncs.second.
///
/// \param syncs The list of synchronisations
/// \param actions The list of actions to be matched.
/// \returns The index of the matching synchronisation, or max value when no matching synchronisation is found.
inline
process::communication_expression get_sync(const process::communication_expression_list& comm_set, const core::identifier_string_list& action_names)
{
  // A synchronising action must consist of two or more action labels
  if (action_names.size() < 2)
  {
    return process::communication_expression();
  }

  auto it = std::find_if(
    comm_set.begin(),
    comm_set.end(),
    [&action_names](const process::communication_expression& comm)
  {
    return comm.action_name().names() == action_names;
  });

  if (it != comm_set.end())
  {
    return *it;
  }
  else
  {
    return process::communication_expression();
  }
}

/// \brief Checks whether the arguments of each of the actions of the
///        action_label are equal.
///
/// \param label The label to check.
/// \returns Whether all arguments of each action of the label
///          are equal.
inline
bool equal_arguments(const lps::multi_action& multi_action)
{
  return multi_action.actions().empty() ||
    std::all_of(
      std::next(multi_action.actions().begin()),
      multi_action.actions().end(),
      [&](const process::action& a) {
        return a.arguments() == multi_action.actions().front().arguments();
      });
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

struct thread_context_t
{
  lts::lts_builder* lts_builder;
  std::queue<std::size_t>& queue;
  lts::detail::progress_monitor& progress_monitor;
  mcrl2::utilities::indexed_set<std::vector<state_t>>& states;
  std::size_t number_of_threads;
  std::mutex& lts_builder_mutex;
  std::mutex& queue_mutex;
  std::mutex& progress_mutex;
  std::mutex& states_mutex;
  std::condition_variable& queue_cond;
  std::size_t& busy;
};

/// \brief A worker that computes states of the combined LTS.
class state_thread_worker
{
public:
  state_thread_worker(const combine_lts_input& input,
    const std::vector<lts::outgoing_transitions_per_state_t>& outgoing_transitions,
    thread_context_t& context)
    : input(input),
      outgoing_transitions(outgoing_transitions),
      context(context)
  {}

  void operator()()
  {
    while (true)
    {
      std::unique_lock<std::mutex> queue_lock(context.queue_mutex);

      // Wait if queue is empty but threads are still working
      while (context.queue.empty())
      {
        context.busy--;
        if (context.busy > 0)
        {
          // Some threads are still busy, wait for them to add work
          context.queue_cond.wait(queue_lock);
        }
        else
        {
          // All threads completed and queue is empty, exit
          return;
        }
      }

      context.busy++;

      const std::size_t state_index = context.queue.front();
      context.queue.pop();
      queue_lock.unlock();

      // Process the state
      visit_state(state_index);
    }
  }

private:
  const combine_lts_input& input;
  const std::vector<lts::outgoing_transitions_per_state_t>& outgoing_transitions;
  thread_context_t& context;

  std::vector<state_t> get_state(std::size_t state_index)
  {
    std::lock_guard<std::mutex> state_lock(context.states_mutex);
    return context.states[state_index];
  }

  std::pair<std::size_t, bool> insert_state(const std::vector<state_t>& state)
  {
    std::lock_guard<std::mutex> state_lock(context.states_mutex);
    return context.states.insert(state);
  }

  void queue_state(std::size_t state_index)
  {
    std::unique_lock<std::mutex> queue_lock(context.queue_mutex);
    context.queue.push(state_index);
    queue_lock.unlock();
    context.queue_cond.notify_one();
  }

  void examine_transition()
  {
    std::lock_guard<std::mutex> progress_lock(context.progress_mutex);
    context.progress_monitor.examine_transition();
  }

  template <typename ActionLabel>
  void add_transition(std::size_t from_state, const ActionLabel& label, std::size_t to_state)
  {
    std::lock_guard<std::mutex> builder_lock(context.lts_builder_mutex);
    context.lts_builder->add_transition(from_state, label, to_state, context.number_of_threads);
  }

  /// \brief Compute the outgoing transitions for the combined state and write them to output.
  void compute_outgoing_transitions(const std::vector<state_t>& state,
    std::vector<std::pair<lps::multi_action, std::vector<state_t>>>& output)
  {
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
        const lts::action_label_lts& label = input.ltss[i].action_label(lts::label(transition));

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
        for (auto& combo: output)
        {
          // The new state of the combo already contains this state
          if (combo.second.size() >= i + 1)
          {
            continue;
          }

          // Copy states from combo to new state list
          std::vector<state_t> new_states;
          for (state_t s: combo.second)
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
      for (auto& combo: output)
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
      for (auto& combo: new_combos)
      {
        output.push_back(combo);
      }
    }
  }

  std::size_t report_state(const std::vector<state_t>& state)
  {
    auto [new_state, inserted] = insert_state(state);
    if (inserted)
    {
      queue_state(new_state);
    }
    return new_state;
  }

  void report_transition(std::size_t from_state, const lps::multi_action& label, std::size_t to_state)
  {
    examine_transition();
    add_transition(from_state, label, to_state);
  }

  lps::multi_action apply_communication(const lps::multi_action& label)
  {
    process::communication_expression comm_expr;
    const core::identifier_string_list action_names = get_action_names(label.actions());

    mCRL2log(log::debug) << core::pp(action_names) << std::endl;

    if (equal_arguments(label)
        && (comm_expr = get_sync(input.comm_set, action_names)) != process::communication_expression())
    {
      // Synchronise
      const core::identifier_string& result_action = comm_expr.name();
      mCRL2log(log::debug) << "Sync: " << result_action << std::endl;

      data::data_expression_list arguments;
      data::sort_expression_list sorts;
      if (!label.actions().empty())
      {
        // Only use arguments and sorts if the action is not a tau action
        arguments = label.actions().front().arguments();
        sorts = label.actions().front().label().sorts();
      }

      // Create new label from the synchronisation
      return lps::multi_action(process::action(process::action_label(result_action, sorts), arguments));
    }
    else
    {
      // No synchronisation, return original label
      return label;
    }
  }

  /// \returns True iff at least one state was computed.
  void visit_state(std::size_t state_index)
  {
    std::vector<state_t> state = get_state(state_index);
    // List of new outgoing transitions from this state, combined from the states
    // state[0] to state[i]
    std::vector<std::pair<lps::multi_action, std::vector<state_t>>> new_outgoing_transitions;
    compute_outgoing_transitions(state, new_outgoing_transitions);


    // Finished generating all new transitions, add them to the LTS
    for (auto& [label, target_state] : new_outgoing_transitions)
    {
      mCRL2log(log::debug) << lps::pp(label) << std::endl;

      label = apply_communication(label);

      // Check if new transition is blocked or not allowed
      if (lps::encap(input.blocks, label.actions()) || !lps::allow_(input.allow_cache,  label.actions(), process::action()))
      {
        mCRL2log(log::debug) << "Blocked or not allowed: " << lps::pp(label) << std::endl;
        continue;
      }

      // Hide actions in transition label
      lps::hide_(input.hiden, label);

      const std::size_t new_state = report_state(target_state);
      report_transition(state_index, label, new_state);
    }
  }
};

void mcrl2::combine_lts(const combine_lts_input& input)
{
  // Calculate which states can be reached in a single outgoing step for both LTSs.
  std::vector<lts::outgoing_transitions_per_state_t> outgoing_transitions;
  for (const lts::lts_lts_t& lts_input: input.ltss)
  {
    outgoing_transitions.emplace_back(lts_input.get_transitions(), lts_input.num_states(), true);
  }

  // The parallel composition has pair of states that are stored in an indexed set (to keep track of processed states).
  mcrl2::utilities::indexed_set<std::vector<state_t>> states;
  std::vector<state_t> initial_states;
  process::action_label_list action_decls;
  data::data_specification data_spec;
  data::variable_list proc_params;
  for (const auto& lts: input.ltss)
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
  if (input.save_at_end)
  {
    combined_lts_lts_builder* lts_lts_builder = new combined_lts_lts_builder(data_spec, action_decls, proc_params);
    lts_builder = lts_lts_builder;
    combined_lts_builder = lts_lts_builder;
  }
  else
  {
    combined_lts_disk_builder* lts_disk_builder
        = new combined_lts_disk_builder(input.filename, data_spec, action_decls, proc_params);
    lts_builder = lts_disk_builder;
    combined_lts_builder = lts_disk_builder;
  }

  std::mutex builder_mutex;
  std::mutex queue_mutex;
  std::mutex progress_mutex;
  std::mutex states_mutex;

  // Used to signal busy and empty queue.
  std::condition_variable queue_cond;
  std::size_t busy = input.nr_of_threads;

  thread_context_t context{
      lts_builder,
      queue,
      progress_monitor,
      states,
      input.nr_of_threads,
      builder_mutex,
      queue_mutex,
      progress_mutex,
      states_mutex,
      queue_cond,
      busy};

  std::vector<std::thread> threads;

  for (size_t i = 0; i < input.nr_of_threads; i++)
  {
    threads.emplace_back(state_thread_worker(input, outgoing_transitions, context));
  }

  for (size_t i = 0; i < input.nr_of_threads; i++)
  {
    threads[i].join();
  }

  progress_monitor.finish_exploration(states.size(), input.nr_of_threads);

  // Write the initial state and the state labels.
  combined_lts_builder->finalize_combined(states.size());
  lts_builder->save(input.filename);
  delete lts_builder;
}
