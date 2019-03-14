// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lts/state_space_generator.h
/// \brief add your file description here.

#ifndef MCRL2_LTS_STATE_SPACE_GENERATOR_H
#define MCRL2_LTS_STATE_SPACE_GENERATOR_H

#include "mcrl2/lts/lts_builder.h"
#include "mcrl2/trace/trace.h"

namespace mcrl2 {

namespace lts {

struct state_space_generator
{
  const lps::generate_lts_options& options;
  lps::explorer explorer;
  std::map<lps::state, lps::state> m_backpointers;
  std::size_t saved_trace_count = 0;
  std::vector<bool> summand_has_detect_actions;
  std::map<lps::multi_action, lps::state> detect_nondeterminism_map;
  std::vector<lps::explorer::explorer_summand> divergence_summands;
  std::vector<lps::explorer::explorer_summand> divergence_confluent_summands;

  state_space_generator(const lps::specification& lpsspec, const lps::generate_lts_options& options_)
    : options(options_), explorer(lpsspec, options_)
  {
    if (options.detect_action)
    {
      initialize_detected_action_summands(lpsspec);
    }
    if (options.detect_divergence)
    {
      initialize_divergence_summands();
    }
  }

  bool match_action(const lps::action_summand& summand) const
  {
    using utilities::detail::contains;
    for (const process::action& a: summand.multi_action().actions())
    {
      if (contains(options.trace_actions, a.label().name()))
      {
        return true;
      }
    }
    return false;
  }

  void initialize_detected_action_summands(const lps::specification& lpsspec)
  {
    using utilities::detail::contains;
    const auto& summands = lpsspec.process().action_summands();
    summand_has_detect_actions.reserve(summands.size());
    for (const auto& summand: summands)
    {
      summand_has_detect_actions.push_back(match_action(summand));
    }
  }

  void initialize_divergence_summands()
  {
    using utilities::detail::contains;

    auto is_hidden = [&](const lps::explorer::explorer_summand& summand)
    {
      for (const process::action& a: summand.actions.actions())
      {
        if (!contains(options.actions_internal_for_divergencies, a.label().name()))
        {
          return false;
        }
      }
      return true;
    };

    for (const lps::explorer::explorer_summand& summand: explorer.regular_summands())
    {
      if (is_hidden(summand))
      {
        divergence_summands.push_back(summand);
      }
    }

    for (const lps::explorer::explorer_summand& summand: explorer.confluent_summands())
    {
      if (is_hidden(summand))
      {
        divergence_confluent_summands.push_back(summand);
      }
    }
  }

  // Finds a transition s0 --a--> s1, and returns a.
  lps::multi_action find_action(const lps::state& s0, const lps::state& s1)
  {
    for (const std::pair<lps::multi_action, lps::state>& t: explorer.generate_transitions(s0))
    {
      if (t.second == s1)
      {
        return t.first;
      }
    }
    throw mcrl2::runtime_error("no transition found in find_action");
  }

  // Constructs a trace ending in s, using the backpointers map.
  trace::Trace construct_trace(const lps::state& s, const std::map<lps::state, lps::state>& backpointers)
  {
    std::deque<lps::state> states{ s };
    std::deque<lps::multi_action> actions;
    while (true)
    {
      const lps::state& s1 = states.front();
      auto i = backpointers.find(s1);
      if (i == backpointers.end())
      {
        break;
      }
      const lps::state& s0 = i->second;
      states.push_front(s0);
      actions.push_front(find_action(s0, s1));
    }

    trace::Trace tr;
    for (std::size_t i = 0; i < actions.size(); i++)
    {
      tr.setState(states[i]);
      tr.addAction(actions[i]);
    }
    tr.setState(states.back());
    return tr;
  }

  void save_trace(trace::Trace& tr, const std::string& filename, const std::string& message_prefix, const std::string& message_postfix)
  {
    try
    {
      tr.save(filename);
      saved_trace_count++;
      mCRL2log(log::info) << message_prefix << " and saved to '" << filename << "'" << message_postfix;
    }
    catch(...)
    {
      mCRL2log(log::info) << message_prefix << ", but its trace could not be saved to '" << filename << "'" << message_postfix;
    }
  }

  void detect_deadlock(const lps::state& s, std::size_t s_index)
  {
    std::string message_prefix = "Deadlock found";
    std::string message_postfix = " (state index: " + std::to_string(s_index) + ").\n";

    if (options.generate_traces)
    {
      if (saved_trace_count < options.max_traces)
      {
        trace::Trace tr = construct_trace(s, m_backpointers);
        std::string filename = options.trace_prefix + "_dlk_" + std::to_string(saved_trace_count) + ".trc";
        save_trace(tr, filename, message_prefix, message_postfix);
      }
    }
    else
    {
      mCRL2log(log::info) << message_prefix << message_postfix;
    }
  }

  std::string detect_action_filename(const lps::multi_action& a) const
  {
    std::string filename = options.trace_prefix + "_act_" + std::to_string(saved_trace_count);
    if (utilities::detail::contains(options.trace_multiactions, a))
    {
      filename = filename + "_" + lps::pp(a);
    }
    for (const process::action& a_i: a.actions())
    {
      if (utilities::detail::contains(options.trace_actions, a_i.label().name()))
      {
        filename = filename + "_" + core::pp(a_i.label().name());
      }
    }
    filename = filename + ".trc";
    return filename;
  }

  void detect_action(const lps::state& s0, std::size_t s0_index, const lps::multi_action& a, const lps::state& s1)
  {
    using utilities::detail::contains;
    std::string message_prefix = "Action '" + lps::pp(a) + "' found";
    std::string message_postfix = " (state index: " + std::to_string(s0_index) + ").\n";

    if (options.generate_traces)
    {
      if (saved_trace_count < options.max_traces)
      {
        trace::Trace tr = construct_trace(s0, m_backpointers);
        tr.setState(s1);
        tr.addAction(a);
        std::string filename = detect_action_filename(a);
        save_trace(tr, filename, message_prefix, message_postfix);
      }
    }
    else
    {
      mCRL2log(log::info) << message_prefix << message_postfix;
    }
  }

  void detect_nondeterminism(const lps::state& s0, std::size_t s0_index, const lps::multi_action& a, const lps::state& s1)
  {
    auto i = detect_nondeterminism_map.find(a);
    if (i == detect_nondeterminism_map.end())
    {
      detect_nondeterminism_map.insert(std::make_pair(a, s1));
    }
    else if (i->second != s1) // nondeterminism detected
    {
      std::string message_prefix = "Nondeterministic state found";
      std::string message_postfix = " (state index: " + std::to_string(s0_index) + ").\n";

      if (options.generate_traces)
      {
        if (saved_trace_count < options.max_traces)
        {
          trace::Trace tr = construct_trace(s0, m_backpointers);
          tr.setState(s1);
          tr.addAction(a);
          std::string filename = options.trace_prefix + "_nondeterministic_" + std::to_string(saved_trace_count) + ".trc";
          save_trace(tr, filename, message_prefix, message_postfix);
        }
      }
      else
      {
        mCRL2log(log::info) << message_prefix << message_postfix;
      }
    }
  }

  void detect_divergence(const lps::state& s, std::size_t s_index)
  {
    std::unordered_map<lps::state, std::size_t> discovered;
    std::map<lps::state, lps::state> backpointers;
    const lps::state* source = nullptr;
    lps::state last_discovered;

    data::data_expression_list process_parameter_undo = explorer.process_parameter_values();
    explorer.generate_state_space(
      true,
      s,
      divergence_summands,
      divergence_confluent_summands,
      discovered,

      // discover_state
      [&](const lps::state& s, std::size_t /* s_index */)
      {
        last_discovered = s;
        if (options.generate_traces && source)
        {
          backpointers[s] = *source;
        }
      },

      // examine_transition
      [&](std::size_t /* s0_index */, const process::timed_multi_action& a, std::size_t /* s1_index */, const lps::state& s1, std::size_t /* summand_index */)
      {
        if (s1 != last_discovered || s1 == s) // found a loop, hence s is divergent
        {
          std::string message_prefix = "Divergent state found";
          std::string message_postfix = " (state index: " + std::to_string(s_index) + ").\n";

          if (options.generate_traces)
          {
            if (saved_trace_count < options.max_traces)
            {
              trace::Trace tr = construct_trace(s, m_backpointers);
              std::string filename = options.trace_prefix + "_divergence_" + std::to_string(saved_trace_count) + ".trc";
              save_trace(tr, filename, message_prefix, message_postfix);
            }
          }
          else
          {
            mCRL2log(log::info) << message_prefix << message_postfix;
          }
        }
      },

      // start_state
      [&](const lps::state& s, std::size_t /* s_index */)
      {
        source = &s;
      }
    );
    explorer.set_process_parameter_values(process_parameter_undo);
  }

  // Explore the specification passed via the constructor, and put the results in builder.
  void explore(lts_builder& builder)
  {
    bool has_outgoing_transitions;
    const lps::state* source = nullptr;

    explorer.generate_state_space(
      false,

      // discover_state
      [&](const lps::state& s, std::size_t s_index)
      {
        if (options.generate_traces && source)
        {
          m_backpointers[s] = *source;
        }
        if (options.detect_divergence)
        {
          detect_divergence(s, s_index);
        }
      },

      // examine_transition
      [&](std::size_t s0_index, const process::timed_multi_action& a, std::size_t s1_index, const lps::state& s1, std::size_t summand_index)
      {
        builder.add_transition(s0_index, a, s1_index);
        has_outgoing_transitions = true;
        const lps::state& s0 = *source;
        if (options.detect_action && summand_has_detect_actions[summand_index])
        {
          detect_action(s0, s0_index, lps::multi_action(a.actions(), a.time()), s1);
        }
        if (options.detect_nondeterminism)
        {
          detect_nondeterminism(s0, s0_index, lps::multi_action(a.actions(), a.time()), s1);
        }
      },

      // start_state
      [&](const lps::state& s, std::size_t /* s_index */)
      {
        source = &s;
        has_outgoing_transitions = false;
        if (options.detect_nondeterminism)
        {
          detect_nondeterminism_map.clear();
        }
      },

      // finish_state
      [&](const lps::state& s, std::size_t s_index)
      {
        if (options.detect_deadlock && !has_outgoing_transitions)
        {
          detect_deadlock(s, s_index);
        }
      }
    );
    builder.finalize(explorer.state_map());
  }
};

} // namespace lts

} // namespace mcrl2

#endif // MCRL2_LTS_STATE_SPACE_GENERATOR_H
