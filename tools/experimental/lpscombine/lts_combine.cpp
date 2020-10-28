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

// Combine two LTSs resulting from the state space exploration of LPSs of lpscleave into a single LTS.
void mcrl2::combine_lts(const lts::lts_lts_t& left_lts,
  const lts::lts_lts_t& right_lts,
  const std::string& prefix,
  std::ostream& stream)
{
  // Calculate which states can be reached in a single outgoing step for both LTSs.
  const lts::outgoing_transitions_per_state_t left_outgoing(left_lts.get_transitions(), left_lts.num_states(), true);
  const lts::outgoing_transitions_per_state_t right_outgoing(right_lts.get_transitions(), right_lts.num_states(), true);

  // The parallel composition has pair of states that are stored in an indexed set (to keep track of processed states).
  mcrl2::utilities::indexed_set<std::pair<state_t, state_t>> states;
  const auto[initial, found] = states.insert(std::make_pair(left_lts.initial_state(), right_lts.initial_state()));

  // The todo queue containing new found states.
  std::queue<std::size_t> queue;
  queue.push(initial); 

  // The tag action indicates that an independent action.
  process::action tag(process::action_label(std::string(prefix) += "tag", {}), {});

  // Progress monitor.
  lts::detail::progress_monitor progress_monitor(lps::exploration_strategy::es_breadth);

  // Start writing the LTS.
  atermpp::binary_aterm_ostream output(stream);
  lts::write_lts_header(output, left_lts.data(), left_lts.process_parameters(), left_lts.action_label_declarations());

  // The name of a left synchronisation label.
  std::string syncleft(prefix);
  syncleft += "syncleft";

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
      const lts::outgoing_pair_t& transition = left_outgoing.get_transitions()[t];

      // Consider the multi-action label of this transition.
      const lts::action_label_lts& label = left_lts.action_label(lts::label(transition));

      for (const process::action& action : label.actions())
      {
        // If this transition label contains tag then we have an independent transition.
        if (action == tag)
        {
          // The left state can proceed without synchronisation, and the tag must be removed.
          const auto [new_state, inserted] = states.insert(std::make_pair(lts::to(transition), right_state));
          if (inserted)
          {
            queue.push(new_state);
          }

          // Construct label without tag.
          process::action_list actions;
          for (const process::action& action : label.actions())
          {
            if (action != tag)
            {
              actions.push_front(action);
            }
          }

          progress_monitor.examine_transition();
          lts::write_transition(output, state_index, process::timed_multi_action(actions, data::undefined_real()), new_state);
          break;
        }
        else
        {
          // If the action label is a synchronisation action.
          const std::string name = action.label().name();
          if (name.find(syncleft) == 0)
          {
            // This is a synchronisation action, explore all outgoing transitions of the right process to find a corresponding right synchronisation.
            auto index = name.find_last_of("_");
            std::string right_name = std::string(prefix) += std::string("syncright") += name.substr(index);

            // Construct the corresponding right synchronisation action.
            process::action syncright(process::action_label(right_name, action.label().sorts()), action.arguments());

            // Find corresponding synchronisation in the outgoing transitions of the right state.
            for (state_t u = right_outgoing.lowerbound(right_state); u < right_outgoing.upperbound(right_state); ++u)
            {
              const lts::outgoing_pair_t& right_transition = right_outgoing.get_transitions()[u];

              // Consider the multi-action label of this transition.
              const lts::action_label_lts& right_label = right_lts.action_label(lts::label(right_transition));
              for (const process::action& right_action : right_label.actions())
              {
                if (right_action == syncright)
                {
                  // Synchronisation takes place, so construct the resulting action (removing the synchronisation actions).
                  const auto [new_state, inserted] = states.insert(std::make_pair(lts::to(transition), lts::to(right_transition)));
                  if (inserted)
                  {
                    queue.push(new_state);
                  }

                  // Construct label without the left and right synchronisation actions.
                  process::action_list actions;
                  for (const process::action& action2 : label.actions())
                  {
                    if (action2 != action)
                    {
                      actions.push_front(action2);
                    }
                  }

                  for (const process::action& right_action : right_label.actions())
                  {
                    if (right_action != syncright)
                    {
                      actions.push_front(right_action);
                    }
                  }

                  // Add the transition.
                  progress_monitor.examine_transition();
                  lts::write_transition(output, state_index, process::timed_multi_action(actions, data::undefined_real()), new_state);
                  break;
                }
              }
            }

            break;
          }
        }
      }
    }

    // Find independent transitions in the right state.
    for (state_t t = right_outgoing.lowerbound(right_state); t < right_outgoing.upperbound(right_state); ++t)
    {
      const lts::outgoing_pair_t& transition = right_outgoing.get_transitions()[t];

      // Consider the multi-action label of this transition.
      const lts::action_label_lts& label = right_lts.action_label(lts::label(transition));

      for (const process::action& action : label.actions())
      {
        if (action == tag)
        {
          // The right state can proceed without synchronisation, and the tag must be removed.
          const auto [new_state, inserted] = states.insert(std::make_pair(left_state, lts::to(transition)));
          if (inserted)
          {
            queue.push(new_state);
          }

          // Construct label without tag.
          process::action_list actions;
          for (const process::action& action : label.actions())
          {
            if (action != tag)
            {
              actions.push_front(action);
            }
          }

          progress_monitor.examine_transition();
          lts::write_transition(output, state_index, process::timed_multi_action(actions, data::undefined_real()), new_state);
          break;
        }
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