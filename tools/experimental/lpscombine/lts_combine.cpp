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
#include "mcrl2/lts/state_space_generator.h"

#include <queue>

using state_t = std::size_t;

using namespace mcrl2;

/// Sort the action labels such that the first action is the action labelled with 'tag' or 'syncname'.
std::vector<std::pair<process::action, lps::multi_action>> preprocess_labels(const process::action& tag, std::string& syncname, lts::lts_lts_t& lts)
{
  std::vector<std::pair<process::action, lps::multi_action>> labels(lts.num_action_labels());

  for (std::size_t i = 0; i < lts.num_action_labels(); ++i)
  {
    const lts::action_label_lts& label = lts.action_label(i);

    // The new first action label.
    process::action first;

    // Gather all the actions that are not tags or synchronisation actions, and keep track of the tag or sync actions.
    process::action_list actions;
    for (const process::action& action : label.actions())
    {
      const std::string& name = action.label().name();
      if (action == tag || name.find(syncname) == 0)
      {
        assert(first == process::action()); // Can only contain one tag or sync.
        first = action;
      }
      else
      {
        actions.push_front(action);
      }
    }

    // Every transition label that is not tau (i.e. not empty) needs to have a tag or synchronisation action.
    if (label.actions().size() > 0 && first == process::action())
    {
      mCRL2log(log::error) << "Label " << label << " does not contain a tag or synchronisation action.\n";
      throw mcrl2::runtime_error("Unexpected transition.");
    }

    if (label.has_time())
    {
      mCRL2log(log::error) << "Label " << label << " contains time and this tool does not deal properly with time.\n";
      throw mcrl2::runtime_error("Timed transition.");
    }

    assert(label.actions().size() == 0 || first != process::action()); // There should be exactly one.
    labels[i] = std::make_pair(first, lps::multi_action(actions, label.time()));
  }

  return labels;
}

// Combine two LTSs resulting from the state space exploration of LPSs of lpscleave into a single LTS.
void mcrl2::combine_lts(lts::lts_lts_t& left_lts,
  lts::lts_lts_t& right_lts,
  const std::string& prefix,
  std::ostream& stream)
{
  // Calculate which states can be reached in a single outgoing step for both LTSs.
  const lts::outgoing_transitions_per_state_t left_outgoing(left_lts.get_transitions(), left_lts.num_states(), true);
  const lts::outgoing_transitions_per_state_t right_outgoing(right_lts.get_transitions(), right_lts.num_states(), true);

  // The tag action indicates that an independent action.
  process::action tag(process::action_label(std::string(prefix) += "tag", {}), {});

  // The name of a left synchronisation label.
  std::string syncleft(prefix);
  syncleft += "syncleft";

  std::vector<std::pair<process::action, lps::multi_action>> left_labels = preprocess_labels(tag, syncleft, left_lts);

  // The same preprocessing for the right LTS.
  std::string syncright(prefix);
  syncright += "syncright";

  std::vector<std::pair<process::action, lps::multi_action>> right_labels = preprocess_labels(tag, syncright, right_lts);

  // For every syncleft action label we can cache the corresponding syncright label. 
  std::vector<process::action_label> right_synclabel(left_lts.num_action_labels());

  for (std::size_t i = 0; i < left_lts.num_action_labels(); ++i)
  {
    const auto& [sync, label] = left_labels[i];
    
    // Also ignore the tau action.
    if (sync != tag && sync != process::action())
    { 
      // This is a synchronisation action.
      const std::string& name = sync.label().name();

      // Find the synchronisation action's index.
      std::size_t underscore = name.find_last_of("_");

      // Construct the corresponding right synchronisation action.
      right_synclabel[i] = process::action_label(std::string(syncright) += name.substr(underscore), sync.label().sorts());
    }
  }

  // The parallel composition has pair of states that are stored in an indexed set (to keep track of processed states).
  mcrl2::utilities::indexed_set<std::pair<state_t, state_t>> states;
  const auto[initial, found] = states.insert(std::make_pair(left_lts.initial_state(), right_lts.initial_state()));

  // The todo queue containing new found states.
  std::queue<std::size_t> queue;
  queue.push(initial); 

  // Progress monitor.
  lts::detail::progress_monitor progress_monitor(lps::exploration_strategy::es_breadth);

  // Start writing the LTS.
  atermpp::binary_aterm_ostream output(stream);
  lts::write_lts_header(output, left_lts.data(), left_lts.process_parameters(), left_lts.action_label_declarations());

  while (!queue.empty())
  {
    // Take the next pair from the queue.
    std::size_t state_index = queue.front();
    std::pair<state_t, state_t> state = states[state_index];

    queue.pop();

    // Introduce constants for convenience.
    const state_t left_state = state.first;
    const state_t right_state = state.second;

    // Consider all the outgoing transitions for the left state, t is the transition tuple (left_state, label, to).
    for (state_t t = left_outgoing.lowerbound(left_state); t < left_outgoing.upperbound(left_state); ++t)
    {
      const lts::outgoing_pair_t& left_transition = left_outgoing.get_transitions()[t];

      // Consider the multi-action label of this transition.
      const auto& [left_sync, left_label] = left_labels[lts::label(left_transition)];

      // If this transition label contains tag then we have an independent transition.
      if (left_sync == tag)
      {
        // The left state can proceed without synchronisation, and the tag must be removed.
        const auto [new_state, inserted] = states.insert(std::make_pair(lts::to(left_transition), right_state));
        if (inserted)
        {
          queue.push(new_state);
        }

        progress_monitor.examine_transition();
        lts::write_transition(output, state_index, left_label, new_state);
      }
      else
      {
       

        // Find corresponding synchronisation in the outgoing transitions of the right state.
        for (state_t u = right_outgoing.lowerbound(right_state); u < right_outgoing.upperbound(right_state); ++u)
        {
          const lts::outgoing_pair_t& right_transition = right_outgoing.get_transitions()[u];

          // Consider the multi-action label of this transition.
          const auto& [right_sync, right_label] = right_labels[lts::label(right_transition)];

          if (right_sync.label() == right_synclabel[lts::label(left_transition)]
            && right_sync.arguments() == left_sync.arguments())
          {
            // Synchronisation takes place, so construct the resulting action (removing the synchronisation actions).
            const auto [new_state, inserted] = states.insert(std::make_pair(lts::to(left_transition), lts::to(right_transition)));
            if (inserted)
            {
              queue.push(new_state);
            }

            // Add the transition with the remaining actions on each label.
            progress_monitor.examine_transition();
            lts::write_transition(output, state_index,
              lps::multi_action(left_label.actions() + right_label.actions(), data::undefined_real()), new_state);
          }
        }
      }
    }

    // Find independent transitions in the right state.
    for (state_t t = right_outgoing.lowerbound(right_state); t < right_outgoing.upperbound(right_state); ++t)
    {
      const lts::outgoing_pair_t& right_transition = right_outgoing.get_transitions()[t];

      // Consider the multi-action label of this transition.
      const auto& [right_sync, right_label] = right_labels[lts::label(right_transition)];
      if (right_sync == tag)
      {
        // The right state can proceed without synchronisation, and the tag must be removed.
        const auto [new_state, inserted] = states.insert(std::make_pair(left_state, lts::to(right_transition)));
        if (inserted)
        {
          queue.push(new_state);
        }

        progress_monitor.examine_transition();
        lts::write_transition(output, state_index, right_label, new_state);
      }
    }

    progress_monitor.finish_state(states.size(), queue.size());
  }

  progress_monitor.finish_exploration(states.size());
    
  // Write the initial state and the state labels.
  lts::write_initial_state(output, 0);

  for (std::size_t state = 0; state < states.size(); ++state)
  {

  }
}
