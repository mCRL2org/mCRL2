// Author(s): Willem , Jeroen Keiren
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
#include <cstddef>
#include <memory>
#include <mutex>
#include <queue>

using state_t = std::size_t;

using namespace mcrl2;

/// \brief Converts an action list to a mutable vector for manipulation.
inline
std::vector<process::action> make_action_vector(const process::action_list& actions)
{
  return std::vector<process::action>(actions.begin(), actions.end());
}

/// \brief Attempts to match a sorted communication left-hand side for fixed arguments.
/// \details Both actions and lhs_names are assumed to be sorted by action name. The match
/// succeeds if each name in lhs_names occurs in actions with the given arguments.
///
/// \param actions The actions to match against, sorted by action name.
/// \param lhs_names The names of the actions to match, sorted by action name.
/// \param arguments The arguments to match for each action name.
/// \param indices Output parameter containing the indices of the matching actions in the actions vector, if the match is successful.
/// \pre actions and lhs_names are sorted by action name.
/// \pre lhs_names is not empty.
inline
bool match_sorted_lhs(const std::vector<process::action>& actions,
                      const core::identifier_string_list& lhs_names,
                      const data::data_expression_list& arguments,
                      std::vector<std::size_t>& indices)
{
  assert(std::is_sorted(actions.begin(), actions.end(), process::action_compare()));
  assert(std::is_sorted(lhs_names.begin(), lhs_names.end(), process::action_name_compare()));
  assert(!lhs_names.empty());

  indices.clear();

  auto lhs_it = lhs_names.begin();
  for (std::size_t i = 0; i < actions.size() && lhs_it != lhs_names.end(); ++i)
  {
    const core::identifier_string& action_name = actions[i].label().name();

    if (*lhs_it < action_name)
    {
      return false;
    }
    else if (action_name == *lhs_it && actions[i].arguments() == arguments)
    {
      indices.push_back(i);
      ++lhs_it;
    }
    // else (action_name < *lhs_it), continue searching for the same action name.
  }

  /// If we have matched all names in the left-hand side, the match is successful.
  if (lhs_it == lhs_names.end())
  {
    return true;
  }
  else
  {
    indices.clear();
    return false;
  }
}

/// \brief Finds a subset of actions matching a communication rule's left-hand side.
/// \details Tries each candidate argument list for the first action name and then matches the
/// remaining names using a single forward pass over the sorted actions.
/// \param actions The actions to match against, sorted by action name.
/// \param lhs_names The names of the actions to match, sorted by action name.
/// \param indices Output parameter containing the indices of the matching actions in the actions vector, if the match is successful.
/// \pre actions and lhs_names are sorted by action name.
/// \pre lhs_names is not empty.
inline
bool find_matching_indices(const std::vector<process::action>& actions,
                           const core::identifier_string_list& lhs_names,
                           std::vector<std::size_t>& indices)
{
  assert(std::is_sorted(actions.begin(), actions.end(), process::action_compare()));
  assert(std::is_sorted(lhs_names.begin(), lhs_names.end(), process::action_name_compare()));
  assert(!lhs_names.empty());

  const core::identifier_string& first_label = lhs_names.front();

  // Try each candidate argument list for the first action name in the left-hand side,
  // and then match the remaining names using a single forward pass over the sorted actions.
  for (std::size_t i = 0; i < actions.size(); ++i)
  {
    const core::identifier_string& action_name = actions[i].label().name();
    if (first_label < action_name)
    {
      /// Since actions and lhs_names are sorted, if the first label is smaller than the current action name, there is no match.
      break;
    }
    else if (match_sorted_lhs(actions, lhs_names, actions[i].arguments(), indices))
    {
      return true;
    }
    // else (action_name < first_label), continue searching for the first label.
  }

  indices.clear();
  return false;
}

/// \brief Removes actions at the specified indices from the action vector.
inline
void erase_indices(std::vector<process::action>& actions,
                   const std::vector<std::size_t>& indices)
{
  std::vector<process::action> remaining;
  remaining.reserve(actions.size() - indices.size());

  auto index_it = indices.begin();
  for (std::size_t i = 0; i < actions.size(); ++i)
  {
    if (index_it != indices.end() && *index_it == i)
    {
      ++index_it;
    }
    else
    {
      remaining.push_back(actions[i]);
    }
  }

  actions = std::move(remaining);
}

/// \brief Applies communication rules to a multi-action label.
/// \details For each communication rule, greedily matches and replaces subsets of actions
/// with their communicated counterpart. Returns the resulting multi-action with unmatched
/// actions preserved.
/// \pre label.actions() is sorted by action name.
/// \pre every communication expression in comm_set has a left-hand side that is sorted by action name.
///      the left hand sides of communication expressions are disjoint.
lps::multi_action mcrl2::apply_communication(const lps::multi_action& label,
                                             const process::communication_expression_list& comm_set)
{
  assert(std::is_sorted(label.actions().begin(), label.actions().end(), process::action_compare()));
  assert(std::all_of(comm_set.begin(), comm_set.end(), [](const process::communication_expression& comm_expr)
  {
    return std::is_sorted(comm_expr.action_name().names().begin(), comm_expr.action_name().names().end(),
                          process::action_name_compare());
  }));

  // vector of actions that have not yet participated in a communication, sorted by action name
  std::vector<process::action> remaining_actions = make_action_vector(label.actions());
  // vector of actions that are the result of a communication
  std::vector<process::action> communicated_actions;
  // temporary vector for storing matching indices when applying a communication rule
  std::vector<std::size_t> matching_indices;

  // apply every communication expression in the communication set until
  // no more matches can be found.
  for (const process::communication_expression& comm_expr : comm_set)
  {
    while (find_matching_indices(remaining_actions, comm_expr.action_name().names(), matching_indices))
    {
      const process::action& representative = remaining_actions[matching_indices.front()];
      communicated_actions.emplace_back(
          process::action_label(comm_expr.name(), representative.label().sorts()),
          representative.arguments());

      erase_indices(remaining_actions, matching_indices);
    }
  }

  remaining_actions.insert(remaining_actions.end(), communicated_actions.begin(), communicated_actions.end());
  std::sort(remaining_actions.begin(), remaining_actions.end(), process::action_compare());

  return lps::multi_action(process::action_list(remaining_actions.begin(), remaining_actions.end()), label.time());
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
  // Shared objects used by all worker threads.
  lts::lts_builder* lts_builder;
  std::queue<std::size_t>& queue;
  lts::detail::progress_monitor& progress_monitor;
  mcrl2::utilities::indexed_set<std::vector<state_t>>& states;
  std::size_t number_of_threads;

  // Mutexes guard the corresponding shared objects.
  std::mutex& lts_builder_mutex;
  std::mutex& queue_mutex;
  std::mutex& progress_mutex;
  std::mutex& states_mutex;

  // Condition variable and scheduler state for queue-based work distribution.
  // Invariant: active_workers and stop are read/written while holding queue_mutex.
  std::condition_variable& queue_cond;
  std::size_t& active_workers;
  bool& stop;
};

/// \brief A worker that computes states of the combined LTS.
class state_thread_worker
{
public:
  state_thread_worker(const combine_lts_static_context& input,
    const std::vector<lts::outgoing_transitions_per_state_t>& outgoing_transitions,
    thread_context_t& context)
    : input(input),
      outgoing_transitions(outgoing_transitions),
      context(context),
      termination_action(process::action(
          process::action_label(core::identifier_string("Terminate"), data::sort_expression_list()),
          data::data_expression_list()))
  {}

  void operator()()
  {
    while (true)
    {
      std::unique_lock<std::mutex> queue_lock(context.queue_mutex);

      context.queue_cond.wait(queue_lock, [this]()
      {
        return context.stop || !context.queue.empty();
      });

      if (context.stop && context.queue.empty())
      {
        // Global shutdown: no queued work remains and exploration has ended.
        return;
      }

      const std::size_t state_index = context.queue.front();
      context.queue.pop();
      ++context.active_workers;
      queue_lock.unlock();

      // Process the state
      visit_state(state_index);

      queue_lock.lock();
      --context.active_workers;

      if (context.queue.empty() && context.active_workers == 0)
      {
        // This worker observed that no work is queued, and no other workers are
        // processing states; this means we have explored the entire state space.
        // The worker publishes stop and wakes all
        // waiters so every thread can terminate without depending on new work.
        context.stop = true;
        queue_lock.unlock();
        context.queue_cond.notify_all();
        return;
      }

      queue_lock.unlock();
    }
  }

private:
  const combine_lts_static_context& input;
  const std::vector<lts::outgoing_transitions_per_state_t>& outgoing_transitions;
  thread_context_t& context;
  const process::action termination_action;

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

    // Wake exactly one worker: one new state was enqueued.
    context.queue_cond.notify_one();
  }

  std::size_t states_size()
  {
    std::lock_guard<std::mutex> states_lock(context.states_mutex);
    return context.states.size();
  }

  std::size_t queue_size()
  {
    std::lock_guard<std::mutex> queue_lock(context.queue_mutex);
    return context.queue.size();
  }

  void examine_transition()
  {
    std::lock_guard<std::mutex> progress_lock(context.progress_mutex);
    context.progress_monitor.examine_transition();
  }

  void finish_state()
  {
    // Snapshot sizes under their own locks; report with progress_mutex only.
    // This keeps long progress operations away from queue/states critical sections.
    const std::size_t states_size = this->states_size();
    const std::size_t queue_size = this->queue_size();

    std::lock_guard<std::mutex> progress_lock(context.progress_mutex);
    context.progress_monitor.finish_state(states_size, queue_size, input.nr_of_threads);
  }

  template <typename ActionLabel>
  void add_transition(std::size_t from_state, const ActionLabel& label, std::size_t to_state)
  {
    std::lock_guard<std::mutex> builder_lock(context.lts_builder_mutex);
    context.lts_builder->add_transition(from_state, label, to_state, context.number_of_threads);
  }

  /// \brief Compute the outgoing transitions for the combined state and report them via callback.
  /// \details Generates all combinations of transitions (including stutters) across components
  /// without explicitly generating the full set. Each candidate (label, target_state) pair is reported
  /// via callback for early filtering, so we only store the transitions that pass the filter.
  template <typename ReportCandidate>
  void emit_outgoing_transitions(const std::vector<state_t>& state, ReportCandidate report_candidate)
  {
    // Generate all combinations recursively by expanding one component at a time.
    std::vector<state_t> target_state;
    target_state.reserve(state.size());
    lps::multi_action current_label;

    generate_outgoing_transition_combinations(state, 0, false, target_state, current_label, report_candidate);
  }

private:
  /// \brief Recursively generate all combinations of transitions across components.
  /// \details For each component, either take no transition (stutter) or one of the available
  /// outgoing transitions. Generates combinations in depth-first order, reporting each complete
  /// transition (reached all components) via the callback.
  template <typename ReportCandidate>
  void generate_outgoing_transition_combinations(const std::vector<state_t>& state,
                           std::size_t component_index,
                           bool has_taken_transition,
                           std::vector<state_t>& target_state,
                           lps::multi_action& current_label,
                           ReportCandidate& report_candidate)
  {
    // Base case: all components have been processed, report only if at least one
    // component contributed a real transition (exclude pure global stutter).
    if (component_index == state.size())
    {
      if (has_taken_transition)
      {
        report_candidate(current_label, target_state);
      }
      return;
    }

    const state_t& state_component_index = state[component_index];

    // Option 1: stutter on this component (no transition)
    target_state.push_back(state_component_index);
    generate_outgoing_transition_combinations(state, component_index + 1, has_taken_transition, target_state, current_label, report_candidate);
    target_state.pop_back();

    // Option 2: take each available outgoing transition from this component
    for (state_t t = outgoing_transitions[component_index].lowerbound(state_component_index);
         t < outgoing_transitions[component_index].upperbound(state_component_index);
         ++t)
    {
      const lts::outgoing_pair_t& transition = outgoing_transitions[component_index].get_transitions()[t];
      const lts::action_label_lts& label = input.ltss[component_index].action_label(lts::label(transition));

      // Optimization: if the names in the label of the transition does not occur
      // in the inner allow set, we can skip generating this transition
      // since it will be filtered out by the allow operator later anyway.
      const process::action_list& actions = label.actions();
      bool all_allowed = std::all_of(actions.begin(), actions.end(), [this](const process::action& action)
      {
        return input.inner_allowed_action_names.find(action.label().name()) != input.inner_allowed_action_names.end();
      });

      if (all_allowed)
      {
        // Extend the label and target state, then recurse to next component
        target_state.push_back(lts::to(transition));
        const lps::multi_action saved_label = current_label;
        current_label = current_label + label;

      generate_outgoing_transition_combinations(state, component_index + 1, true, target_state, current_label, report_candidate);

        current_label = saved_label;
        target_state.pop_back();
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
    return mcrl2::apply_communication(label, input.comm_set);
  }

  /// \brief Process one state from the combined state space.
  /// \details Emits outgoing transitions via callback, applying communication and filter operators
  /// (allow/block/hide) to each candidate immediately. Rejected candidates are not reported.
  void visit_state(std::size_t state_index)
  {
    const std::vector<state_t> state = get_state(state_index);

    // Callback that filters and reports each candidate transition.
    auto filter_and_report = [this, state_index](const lps::multi_action& candidate_label,
                                                   const std::vector<state_t>& target_state)
    {
      lps::multi_action label = candidate_label;

      mCRL2log(log::debug) << lps::pp(label) << std::endl;

      // Apply communication rules
      label = apply_communication(label);

      // Check if new transition is blocked or not allowed
      if (!lps::encap(input.block_set, label.actions()) && lps::allow_(input.allow_cache, label.actions(), termination_action))
      {
        mCRL2log(log::trace) << "Multi-action is not blocked and allowed:" << lps::pp(label) << std::endl;

        // Hide returns a new label; assign it back.
        label = lps::hide_(input.hide_set, label);

        const std::size_t new_state = report_state(target_state);
        report_transition(state_index, label, new_state);
      }
      else
      {
        mCRL2log(log::trace) << "Multi-action is blocked or not allowed: " << lps::pp(label) << std::endl;
      }
    };

    // Emit transitions via callback; filtering happens inside the lambda above.
    emit_outgoing_transitions(state, filter_and_report);
    finish_state();
  }
};

void mcrl2::combine_lts(const combine_lts_static_context& input)
{
  mCRL2log(log::verbose) << "Combining " << input.ltss.size() << " LTSs with " << input.nr_of_threads << " threads." << std::endl;
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

  std::unique_ptr<combined_lts_builder> combined_lts_builder;
  lts::lts_builder* lts_builder;
  if (input.save_at_end)
  {
    auto lts_lts_builder = std::make_unique<combined_lts_lts_builder>(data_spec, action_decls, proc_params);
    lts_builder = lts_lts_builder.get();
    combined_lts_builder = std::move(lts_lts_builder);
  }
  else
  {
    auto lts_disk_builder = std::make_unique<combined_lts_disk_builder>(input.filename, data_spec, action_decls, proc_params);
    lts_builder = lts_disk_builder.get();
    combined_lts_builder = std::move(lts_disk_builder);
  }

  std::mutex builder_mutex;
  std::mutex queue_mutex;
  std::mutex progress_mutex;
  std::mutex states_mutex;

  // Shared worker scheduling state, protected by queue_mutex.
  // active_workers counts threads that popped work but have not yet completed it.
  // stop becomes true once all states have been processed.
  std::condition_variable queue_cond;
  std::size_t active_workers = 0;
  bool stop = false;

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
      active_workers,
      stop};

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
}
