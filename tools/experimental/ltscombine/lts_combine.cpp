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
#include "mcrl2/utilities/hash_utility.h"

#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <deque>
#include <iterator>
#include <limits>
#include <memory>
#include <mutex>
#include <optional>
#include <thread>
#include <unordered_map>

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
  assert(std::is_sorted(actions.begin(), actions.end()));
  assert(std::is_sorted(lhs_names.begin(), lhs_names.end(), process::action_name_compare()));
  assert(!lhs_names.empty());

  indices.clear();

  auto lhs_it = lhs_names.begin();
  for (std::size_t i = 0; i < actions.size() && lhs_it != lhs_names.end(); ++i)
  {
    const core::identifier_string& action_name = actions[i].label().name();

    if (process::action_name_compare()(*lhs_it, action_name))
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
  assert(std::is_sorted(actions.begin(), actions.end()));
  assert(std::is_sorted(lhs_names.begin(), lhs_names.end(), process::action_name_compare()));
  assert(!lhs_names.empty());

  const core::identifier_string& first_label = lhs_names.front();

  // Try each candidate argument list for the first action name in the left-hand side,
  // and then match the remaining names using a single forward pass over the sorted actions.
  for (std::size_t i = 0; i < actions.size(); ++i)
  {
    const core::identifier_string& action_name = actions[i].label().name();
    if (process::action_name_compare()(first_label, action_name))
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
  assert(std::is_sorted(label.actions().begin(), label.actions().end()));
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

  // multi_action constructor sorts the action list, so no explicit sort needed here.
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

/// \brief The set of states of the combined LTS, which the workers insert into concurrently.
using state_set_t = mcrl2::utilities::indexed_set<std::vector<state_t>, true>;

/// \brief A state of the combined LTS that still has to be explored, together with its index.
using todo_entry_t = std::pair<std::size_t, std::vector<state_t>>;

/// \brief The objects shared by all worker threads.
struct shared_context_t
{
  state_set_t& states;
  lts::lts_builder& lts_builder;
  lts::detail::progress_monitor& progress_monitor;
  std::size_t number_of_threads;

  /// Guards lts_builder.
  std::mutex lts_builder_mutex;

  /// States that any worker can take. Workers explore the states in their own todo buffer, and only move states here
  /// when other workers are idle, so that the shared buffer is rarely accessed.
  std::mutex todo_mutex;
  std::condition_variable todo_cond;
  std::deque<todo_entry_t> todo;                ///< Guarded by todo_mutex.
  std::atomic<std::size_t> todo_size = 0;       ///< The size of todo, written while holding todo_mutex.
  std::atomic<std::size_t> idle_workers = 0;    ///< Written while holding todo_mutex.
  bool stop = false;                            ///< Guarded by todo_mutex. Set once all states have been explored.
};

/// \brief The label index of a component that does not move in a combined transition.
constexpr std::size_t no_transition = std::numeric_limits<std::size_t>::max();

/// \brief A worker only moves states to the shared todo buffer when it has more than this number of states.
constexpr std::size_t share_threshold = 64;

/// \brief A worker writes its transitions to the LTS builder once it has this number of transitions.
constexpr std::size_t flush_threshold = 1024;

/// \brief Maps the label indices of the transitions taken by the components (no_transition for a component that does
/// not move) to the resulting label after communication, block, allow and hide, or to std::nullopt if it is blocked or not
/// allowed. The result only depends on these indices, so it is computed once instead of for every transition.
/// Elements of an unordered_map are not moved on insertion, so references to the labels remain valid.
using label_cache_t = std::unordered_map<std::vector<std::size_t>, std::optional<lps::multi_action>>;

/// \brief A transition of the combined LTS that has not been written to the LTS builder yet. The label is owned by the
/// label cache of the worker.
struct buffered_transition_t
{
  std::size_t from;
  const lps::multi_action* label;
  std::size_t to;
};

/// \brief A worker that computes states of the combined LTS.
class state_thread_worker
{
public:
  state_thread_worker(const combine_lts_static_context& input,
    const std::vector<lts::outgoing_transitions_per_state_t>& outgoing_transitions,
    const std::vector<std::vector<bool>>& allowed_labels,
    shared_context_t& shared,
    std::size_t thread_index)
    : input(input),
      outgoing_transitions(outgoing_transitions),
      allowed_labels(allowed_labels),
      shared(shared),
      thread_index(thread_index)
  {}

  void operator()()
  {
    // The termination action, the label cache and the buffers are locals so that they are constructed and destroyed on
    // this worker thread. An aterm member of this functor would be copy-constructed on the main thread
    // (registering it in the main thread's protection set) but destroyed on the worker thread,
    // because std::thread destroys its copy of the functor on the thread it runs.
    const process::action termination_action(
        process::action_label(core::identifier_string("Terminate"), data::sort_expression_list()),
        data::data_expression_list());
    label_cache_t label_cache;
    std::deque<todo_entry_t> todo;
    std::vector<buffered_transition_t> transitions;

    while (take_shared_states(todo))
    {
      while (!todo.empty())
      {
        const todo_entry_t entry = std::move(todo.front());
        todo.pop_front();

        visit_state(entry.first, entry.second, termination_action, label_cache, todo, transitions);
        share_states(todo);
      }
    }

    // The labels of the buffered transitions are owned by the label cache, so they must be written before it is destroyed.
    flush_transitions(transitions);
  }

private:
  const combine_lts_static_context& input;
  const std::vector<lts::outgoing_transitions_per_state_t>& outgoing_transitions;
  const std::vector<std::vector<bool>>& allowed_labels; ///< Whether a label of a component can occur in an allowed multi-action.
  shared_context_t& shared;
  std::size_t thread_index; ///< The index of this worker for the states set.

  /// \brief Waits until there are shared states, and moves a fair share of them to todo.
  /// \returns false iff all states have been explored, which is the case when all workers are idle and there are no
  ///          shared states.
  bool take_shared_states(std::deque<todo_entry_t>& todo)
  {
    std::unique_lock<std::mutex> todo_lock(shared.todo_mutex);
    ++shared.idle_workers;
    while (shared.todo.empty() && !shared.stop)
    {
      if (shared.idle_workers == shared.number_of_threads)
      {
        shared.stop = true;
        shared.todo_cond.notify_all();
      }
      else
      {
        shared.todo_cond.wait(todo_lock);
      }
    }

    if (shared.stop)
    {
      return false;
    }

    --shared.idle_workers;
    const std::size_t share = std::max<std::size_t>(1, shared.todo.size() / shared.number_of_threads);
    for (std::size_t i = 0; i < share; ++i)
    {
      todo.push_back(std::move(shared.todo.front()));
      shared.todo.pop_front();
    }
    shared.todo_size = shared.todo.size();
    return true;
  }

  /// \brief Moves half of the states in todo to the shared todo buffer when other workers are idle and it is empty.
  void share_states(std::deque<todo_entry_t>& todo)
  {
    if (todo.size() > share_threshold && shared.idle_workers.load(std::memory_order_relaxed) > 0
        && shared.todo_size.load(std::memory_order_relaxed) == 0)
    {
      std::unique_lock<std::mutex> todo_lock(shared.todo_mutex);
      if (shared.todo.empty())
      {
        // Share the most recently found states, so that this worker keeps the ones it is about to explore.
        const std::size_t share = todo.size() / 2;
        std::move(todo.end() - static_cast<std::ptrdiff_t>(share), todo.end(), std::back_inserter(shared.todo));
        todo.erase(todo.end() - static_cast<std::ptrdiff_t>(share), todo.end());
        shared.todo_size = shared.todo.size();
        todo_lock.unlock();
        shared.todo_cond.notify_all();
      }
    }
  }

  /// \brief Writes the buffered transitions to the LTS builder.
  void flush_transitions(std::vector<buffered_transition_t>& transitions)
  {
    std::scoped_lock builder_lock(shared.lts_builder_mutex);
    for (const buffered_transition_t& transition: transitions)
    {
      shared.progress_monitor.examine_transition();
      // The builder lock is already held, so the builder does not have to lock itself.
      shared.lts_builder.add_transition(transition.from, *transition.label, transition.to, 1);
    }
    transitions.clear();
  }

  /// \brief Compute the outgoing transitions for the combined state and report them via callback.
  /// \details Generates all combinations of transitions (including stutters) across components
  /// without explicitly generating the full set. Each candidate (labels, target_state) pair is reported
  /// via callback for early filtering, so we only store the transitions that pass the filter.
  template <typename ReportCandidate>
  void emit_outgoing_transitions(const std::vector<state_t>& state, ReportCandidate report_candidate)
  {
    // Generate all combinations recursively by expanding one component at a time.
    std::vector<state_t> target_state(state.size());
    std::vector<std::size_t> labels(state.size(), no_transition);

    generate_outgoing_transition_combinations(state, 0, false, target_state, labels, report_candidate);
  }

  /// \brief Recursively generate all combinations of transitions across components.
  /// \details For each component, either take no transition (stutter) or one of the available
  /// outgoing transitions. Generates combinations in depth-first order, reporting each complete
  /// transition (reached all components) via the callback.
  template <typename ReportCandidate>
  void generate_outgoing_transition_combinations(const std::vector<state_t>& state,
                           std::size_t component_index,
                           bool has_taken_transition,
                           std::vector<state_t>& target_state,
                           std::vector<std::size_t>& labels,
                           ReportCandidate& report_candidate)
  {
    // Base case: all components have been processed, report only if at least one
    // component contributed a real transition (exclude pure global stutter).
    if (component_index == state.size())
    {
      if (has_taken_transition)
      {
        report_candidate(labels, target_state);
      }
      return;
    }

    const state_t& state_component_index = state[component_index];

    // Option 1: stutter on this component (no transition)
    target_state[component_index] = state_component_index;
    labels[component_index] = no_transition;
    generate_outgoing_transition_combinations(state, component_index + 1, has_taken_transition, target_state, labels, report_candidate);

    // Option 2: take each available outgoing transition from this component
    for (state_t t = outgoing_transitions[component_index].lowerbound(state_component_index);
         t < outgoing_transitions[component_index].upperbound(state_component_index);
         ++t)
    {
      const lts::outgoing_pair_t& transition = outgoing_transitions[component_index].get_transitions()[t];

      // Optimization: if the names in the label of the transition does not occur
      // in the inner allow set, we can skip generating this transition
      // since it will be filtered out by the allow operator later anyway.
      if (allowed_labels[component_index][lts::label(transition)])
      {
        // Extend the labels and target state, then recurse to next component
        target_state[component_index] = lts::to(transition);
        labels[component_index] = lts::label(transition);
        generate_outgoing_transition_combinations(state, component_index + 1, true, target_state, labels, report_candidate);
      }
    }
  }

  /// \returns The label of the combined transition in which component i takes a transition labelled labels[i], or
  ///          std::nullopt if it is blocked or not allowed.
  const std::optional<lps::multi_action>& combined_label(const std::vector<std::size_t>& labels,
    const process::action& termination_action,
    label_cache_t& label_cache)
  {
    if (const auto it = label_cache.find(labels); it != label_cache.end())
    {
      return it->second;
    }

    // Only insert the result once it is computed, so that an exception does not leave a blocked entry behind.
    lps::multi_action label;
    for (std::size_t i = 0; i < labels.size(); ++i)
    {
      if (labels[i] != no_transition)
      {
        label = label + input.ltss[i].action_label(labels[i]);
      }
    }

    mCRL2log(log::log_level_t::debug) << lps::pp(label) << std::endl;

    // Apply communication rules
    label = mcrl2::apply_communication(label, input.comm_set);

    // Check if new transition is blocked or not allowed
    std::optional<lps::multi_action> result;
    if (!lps::encap(input.block_set, label.actions()) && lps::allow_(input.allow_cache, label.actions(), termination_action))
    {
      mCRL2log(log::log_level_t::trace) << "Multi-action is not blocked and allowed:" << lps::pp(label) << std::endl;
      result = lps::hide_(input.hide_set, label);
    }
    else
    {
      mCRL2log(log::log_level_t::trace) << "Multi-action is blocked or not allowed: " << lps::pp(label) << std::endl;
    }
    return label_cache.emplace(labels, std::move(result)).first->second;
  }

  /// \brief Process one state from the combined state space.
  /// \details Emits outgoing transitions via callback, using the label cache to determine whether each candidate is
  /// blocked or allowed and its resulting label. Rejected candidates are not reported. New states are added to todo,
  /// and the transitions to the buffer.
  void visit_state(std::size_t state_index,
    const std::vector<state_t>& state,
    const process::action& termination_action,
    label_cache_t& label_cache,
    std::deque<todo_entry_t>& todo,
    std::vector<buffered_transition_t>& transitions)
  {
    emit_outgoing_transitions(state, [&](const std::vector<std::size_t>& labels, const std::vector<state_t>& target_state)
    {
      const std::optional<lps::multi_action>& label = combined_label(labels, termination_action, label_cache);
      if (label)
      {
        const auto [new_state, inserted] = shared.states.insert(target_state, thread_index);
        if (inserted)
        {
          todo.emplace_back(new_state, target_state);
        }

        transitions.push_back({state_index, &*label, new_state});
        if (transitions.size() >= flush_threshold)
        {
          flush_transitions(transitions);
        }
      }
    });

    shared.progress_monitor.finish_state(shared.states.size(thread_index), todo.size(), input.nr_of_threads);
  }
};

void mcrl2::combine_lts(const combine_lts_static_context& input)
{
  mCRL2log(log::log_level_t::verbose) << "Combining " << input.ltss.size() << " LTSs with " << input.nr_of_threads << " threads." << std::endl;
  // Calculate which states can be reached in a single outgoing step for both LTSs.
  std::vector<lts::outgoing_transitions_per_state_t> outgoing_transitions;
  for (const lts::lts_lts_t& lts_input: input.ltss)
  {
    outgoing_transitions.emplace_back(lts_input.get_transitions(), lts_input.num_states(), true);
  }

  // The parallel composition has pair of states that are stored in an indexed set (to keep track of processed states).
  state_set_t states(input.nr_of_threads);
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

  // Determine for every label of every component whether all its action names occur in the inner allow set.
  std::vector<std::vector<bool>> allowed_labels;
  for (const lts::lts_lts_t& lts: input.ltss)
  {
    std::vector<bool>& allowed = allowed_labels.emplace_back(lts.num_action_labels());
    for (std::size_t i = 0; i < lts.num_action_labels(); ++i)
    {
      const process::action_list& actions = lts.action_label(i).actions();
      allowed[i] = std::all_of(actions.begin(), actions.end(), [&input](const process::action& action)
      {
        return input.inner_allowed_action_names.contains(action.label().name());
      });
    }
  }

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

  shared_context_t shared{
      .states=states,
      .lts_builder=*lts_builder,
      .progress_monitor=progress_monitor,
      .number_of_threads=input.nr_of_threads};
  shared.todo.emplace_back(initial, initial_states);
  shared.todo_size = 1;

  std::vector<std::thread> threads;
  for (std::size_t i = 0; i < input.nr_of_threads; ++i)
  {
    // The states set has one mutex per thread in addition to the main mutex at index zero when there are several threads.
    const std::size_t thread_index = input.nr_of_threads == 1 ? 0 : i + 1;
    threads.emplace_back(state_thread_worker(input, outgoing_transitions, allowed_labels, shared, thread_index));
  }

  for (std::thread& thread: threads)
  {
    thread.join();
  }

  progress_monitor.finish_exploration(states.size(), input.nr_of_threads);

  // Write the initial state and the state labels.
  combined_lts_builder->finalize_combined(states.size());
  lts_builder->save(input.filename);
}
