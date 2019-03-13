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

#include "mcrl2/lts/state_space_generator.h"
#include "mcrl2/trace/trace.h"

namespace mcrl2 {

namespace lts {

struct state_space_generator
{
  const lps::generate_lts_options& options;
  lps::explorer explorer;
  std::map<lps::state, lps::state> backpointers;
  std::size_t saved_trace_count = 0;
  std::vector<bool> detect_action_info;

  state_space_generator(const lps::specification& lpsspec, const lps::generate_lts_options& options_)
    : options(options_), explorer(lpsspec, options_)
  {
    initialize_detected_action_summands(lpsspec);
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
    if (options.detect_action)
    {
      const auto& summands = lpsspec.process().action_summands();
      detect_action_info.reserve(summands.size());
      for (const auto& summand: summands)
      {
        detect_action_info.push_back(match_action(summand));
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
  trace::Trace construct_trace(const lps::state& s)
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

  bool save_trace(trace::Trace& tr, const std::string& filename)
  {
    try
    {
      tr.save(filename);
      saved_trace_count++;
      return true;
    }
    catch(...)
    {
      return false;
    }
  }

  void detect_deadlock(const lps::state& s, std::size_t s_index)
  {
    if (options.generate_traces && saved_trace_count < options.max_traces)
    {
      trace::Trace tr = construct_trace(s);
      std::string filename = options.trace_prefix + "_dlk_" + std::to_string(saved_trace_count) + ".trc";
      if (save_trace(tr, filename))
      {
        mCRL2log(log::info) << "deadlock-detect: deadlock found and saved to '" << filename << "' (state index: " << s_index << ").\n";
      }
      else
      {
        mCRL2log(log::info) << "deadlock-detect: deadlock found, but its trace could not be saved to '" << filename << "' (state index: " << s_index << ").\n";
      }
    }
    else
    {
      mCRL2log(log::info) << "deadlock-detect: deadlock found (state index: " << s_index <<  ").\n";
    }
  }

  void detect_action(const lps::state& s0, std::size_t s0_index, const lps::multi_action& a, const lps::state& s1, std::size_t summand_index)
  {
    using utilities::detail::contains;

    if (detect_action_info[summand_index])
    {
      if (options.generate_traces && saved_trace_count < options.max_traces)
      {
        mCRL2log(log::info) << "Detected action '" << a << "' (state index " << s0_index << ")";
        {
          trace::Trace tr = construct_trace(s0);
          tr.setState(s1);
          tr.addAction(a);

          // generate filename
          std::string filename = options.trace_prefix + "_act_" + std::to_string(saved_trace_count);
          if (contains(options.trace_multiactions, a))
          {
            filename = filename + "_" + lps::pp(a);
          }
          for (const process::action& a_i: a.actions())
          {
            if (contains(options.trace_actions, a_i.label().name()))
            {
              filename = filename + "_" + core::pp(a_i.label().name());
            }
          }
          filename = filename + ".trc";

          if (save_trace(tr, filename))
          {
            mCRL2log(log::info) << " and saved to '" << filename << "'.\n";
          }
          else
          {
            mCRL2log(log::info) << " but it could not be saved to '" << filename << "'.\n";
          }
        }
      }
      else
      {
        mCRL2log(log::info) << "Detected action '" << a << "' (state index " << s0_index << ").\n";
      }
    }
  }

  // Explore the specification passed via the constructor, and put the results in builder.
  void explore(lts_builder& builder)
  {
    bool has_outgoing_transitions;
    const lps::state* source = nullptr;

    explorer.generate_state_space(
      // discover_state
      [&](const lps::state& d, std::size_t /* d_index */)
      {
        if (source)
        {
          backpointers[d] = *source;
        }
      },

      // examine_transition
      [&](std::size_t s0_index, const process::timed_multi_action& a, std::size_t s1_index, const lps::state& s1, std::size_t summand_index)
      {
        builder.add_transition(s0_index, a, s1_index);
        has_outgoing_transitions = true;
        if (options.detect_action)
        {
          detect_action(*source, s0_index, lps::multi_action(a.actions(), a.time()), s1, summand_index);
        }
      },

      // start_state
      [&](const lps::state& d, std::size_t /* i */)
      {
        source = &d;
        has_outgoing_transitions = false;
      },

      // finish_state
      [&](const lps::state& d, std::size_t i)
      {
        if (options.detect_deadlock && !has_outgoing_transitions)
        {
          detect_deadlock(d, i);
        }
      }
    );
    builder.finalize(explorer.state_map());
  }
};

} // namespace lts

} // namespace mcrl2

#endif // MCRL2_LTS_STATE_SPACE_GENERATOR_H
