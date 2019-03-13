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
  lps::lps_explorer explorer;
  std::map<lps::state, lps::state> backpointers;
  std::size_t trace_count = 0; // the number of saved traces

  state_space_generator(const lps::specification& lpsspec, const lps::generate_lts_options& options_)
    : options(options_), explorer(lpsspec, options_)
  {}

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
      return true;
    }
    catch(...)
    {
      return false;
    }
  }

  void report_deadlock(const lps::state& s, std::size_t s_index)
  {
    if (options.generate_traces && trace_count <= options.max_traces)
    {
      trace::Trace tr = construct_trace(s);
      std::string filename = options.trace_prefix + "_dlk_" + std::to_string(trace_count) + ".trc";
      if (save_trace(tr, filename))
      {
        trace_count++;
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
      [&](std::size_t from, const process::timed_multi_action& a, std::size_t to)
      {
        builder.add_transition(from, a, to);
        has_outgoing_transitions = true;
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
        if (!has_outgoing_transitions && options.detect_deadlock)
        {
          report_deadlock(d, i);
        }
      }
    );
    builder.finalize(explorer.state_map());
  }
};

} // namespace lts

} // namespace mcrl2

#endif // MCRL2_LTS_STATE_SPACE_GENERATOR_H
