// Author(s): Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "mcrl2/lps/simulation.h"
#include "mcrl2/trace/trace.h"

using namespace mcrl2;
using namespace mcrl2::lps;

simulation::simulation(const specification& specification, data::rewrite_strategy strategy)
  : m_specification(specification),
    m_rewriter(m_specification.data(), strategy),
    m_generator(m_specification, m_rewriter),
    m_tau_prioritization(false)
{
  state_t state;
  state.source_state = m_generator.initial_state();
  state.transitions = transitions(state.source_state);
  m_full_trace.push_back(state);
}

void simulation::truncate(size_t state_number)
{
  if (m_tau_prioritization)
  {
    m_prioritized_trace.resize(state_number + 1);
    m_prioritized_originals.resize(state_number + 1);
    m_full_trace.resize(m_prioritized_originals.back() + 1);
  }
  else
  {
    m_full_trace.resize(state_number + 1);
  }
}

void simulation::select(size_t transition_number)
{
  if (m_tau_prioritization)
  {
    m_prioritized_trace.back().transition_number = transition_number;
    state_t prioritized_state;
    prioritized_state.source_state = m_prioritized_trace.back().transitions[transition_number].destination;
    prioritized_state.transitions = prioritize(transitions(prioritized_state.source_state));
    m_prioritized_trace.push_back(prioritized_state);

    m_full_trace.back().transition_number = transition_number;
    state_t full_state;
    full_state.source_state = m_full_trace.back().transitions[transition_number].destination;
    full_state.transitions = transitions(full_state.source_state);
    m_full_trace.push_back(full_state);

    while (true)
    {
      bool found = false;
      std::vector<transition_t> &transitions = m_full_trace.back().transitions;
      for (size_t index = 0; index < transitions.size(); index++)
      {
        if (is_prioritized(transitions[index].action))
        {
          m_full_trace.back().transition_number = index;
          state_t state;
          state.source_state = transitions[index].destination;
          state.transitions = simulation::transitions(state.source_state);
          m_full_trace.push_back(state);
          found = true;
          break;
        }
      }
      if (!found)
      {
        break;
      }
    }

    m_prioritized_originals.push_back(m_full_trace.size() - 1);
  }
  else
  {
    m_full_trace.back().transition_number = transition_number;
    state_t state;
    state.source_state = m_full_trace.back().transitions[transition_number].destination;
    state.transitions = transitions(state.source_state);
    m_full_trace.push_back(state);
  }
}

void simulation::enable_tau_prioritization(bool enable, std::string action)
{
  m_tau_prioritization = enable;
  m_prioritized_action = action;

  m_prioritized_trace.clear();
  m_prioritized_originals.clear();
  if (enable)
  {
    prioritize_trace();
  }
}

void simulation::save(const std::string &filename)
{
  trace::Trace trace;
  trace.setState(m_full_trace[0].source_state);
  for (size_t i = 0; i + 1 < m_full_trace.size(); i++)
  {
    trace.addAction(m_full_trace[i].transitions[m_full_trace[i].transition_number].action);
    trace.setState(m_full_trace[i+1].source_state);
  }
  trace.save(filename);
}

void simulation::load(const std::string &filename)
{
  trace::Trace trace(filename, m_specification.data(), m_specification.action_labels());
  trace.resetPosition();

  state initial_state;
  if (trace.current_state_exists())
  {
    initial_state = trace.currentState();
  }

  std::vector<transition_t> transitions;
  for (size_t i = 0; i < trace.number_of_actions(); i++)
  {
    transition_t transition;
    transition.action = trace.currentAction();
    trace.increasePosition();
    if (trace.current_state_exists())
    {
      transition.destination = trace.currentState();
    }
    transitions.push_back(transition);
  }

  std::deque<state_t> target_trace;
  state_t initial;
  initial.source_state = m_generator.initial_state();
  if (!initial_state.empty())
  {
    if (!match(initial_state, initial.source_state))
    {
      throw mcrl2::runtime_error("The initial state of the trace is not equal to the initial state of this specification");
    }
  }
  initial.transitions = simulation::transitions(initial.source_state);
  target_trace.push_back(initial);

  target_trace = match_trace(target_trace, transitions, 0);
  if (target_trace.size() < transitions.size() + 1)
  {
    std::stringstream ss;
    ss << "could not perform action " << (target_trace.size() - 1) << " (" << pp(transitions[target_trace.size() - 1].action) << ") from trace";
    throw mcrl2::runtime_error(ss.str());
  }

  m_full_trace = target_trace;
  if (m_tau_prioritization)
  {
    m_prioritized_trace.clear();
    m_prioritized_originals.clear();
    prioritize_trace();
  }
}

std::vector<simulation::transition_t> simulation::transitions(state source_state)
{
  try
  {
    std::vector<simulation::transition_t> output;
    for (next_state_generator::iterator i = m_generator.begin(source_state, &m_substitution); i != m_generator.end(); i++)
    {
      transition_t transition;
      transition.destination = i->state();
      transition.action = i->action();
      output.push_back(transition);
    }
    return output;
  }
  catch (mcrl2::runtime_error& e)
  {
    mCRL2log(mcrl2::log::error) << "an error occurred while calculating the transitions from this state;\n" << e.what() << std::endl;
    return std::vector<simulation::transition_t>();
  }
}

std::vector<simulation::transition_t> simulation::prioritize(const std::vector<simulation::transition_t> &transitions)
{
  std::vector<simulation::transition_t> output;
  for (std::vector<simulation::transition_t>::const_iterator i = transitions.begin(); i != transitions.end(); i++)
  {
    simulation::transition_t transition = *i;
    while (true)
    {
      bool found = false;
      std::vector<transition_t> next_transitions = simulation::transitions(transition.destination);
      for (std::vector<transition_t>::iterator j = next_transitions.begin(); j != next_transitions.end(); j++)
      {
        if (is_prioritized(j->action))
        {
          transition.destination = j->destination;
          found = true;
          break;
        }
      }
      if (!found)
      {
        break;
      }
    }
  }
  return output;
}

bool simulation::is_prioritized(const multi_action &action)
{
  if (m_prioritized_action == "tau")
  {
    return action.actions().size() == 0;
  }
  else
  {
    return action.actions().size() == 1 && (std::string)action.actions().front().label().name() == m_prioritized_action;
  }
}

void simulation::prioritize_trace()
{
  m_prioritized_trace.push_back(m_full_trace.back());
  for (size_t index = 0; index < m_full_trace.size() - 1; index++)
  {
    transition_t transition = m_full_trace[index].transitions[m_full_trace[index].transition_number];
    if (is_prioritized(transition.action))
    {
      m_prioritized_trace.back().source_state = transition.destination;
    }
    else
    {
      m_prioritized_trace.push_back(m_full_trace[index + 1]);
      m_prioritized_originals.push_back(index);
    }
  }
  m_prioritized_originals.push_back(m_full_trace.size() - 1);

  for (std::deque<state_t>::iterator i = m_prioritized_trace.begin(); i != m_prioritized_trace.end(); i++)
  {
    i->transitions = prioritize(transitions(i->source_state));
  }
}

/// Tries to match the transitions from \a transition_number onwards to the end of \a trace. Returns the most complete match found.
/// WARNING: This has exponential worst-case running time when state labels are missing!
std::deque<simulation::state_t> simulation::match_trace(std::deque<simulation::state_t> trace, const std::vector<simulation::transition_t> &transitions, size_t transition_number)
{
  if (transition_number >= transitions.size())
  {
    return trace;
  }

  std::deque<simulation::state_t> best_trace = trace;
  state_t &base_state = trace.back();
  for (size_t i = 0; i < base_state.transitions.size(); i++)
  {
    if (base_state.transitions[i].action == transitions[transition_number].action &&
      (transitions[transition_number].destination.empty() || match(base_state.transitions[i].destination, transitions[transition_number].destination)))
    {
      base_state.transition_number = i;
      state_t s;
      s.source_state = base_state.transitions[i].destination;
      s.transitions = simulation::transitions(s.source_state);
      trace.push_back(s);

      std::deque<simulation::state_t> recursive_trace = match_trace(trace, transitions, transition_number + 1);
      if (recursive_trace.size() > best_trace.size())
      {
        best_trace = recursive_trace;
        // Early exit
        if (best_trace.size() == transitions.size() + 1)
        {
          return best_trace;
        }
      }

      trace.pop_back();
    }
  }
  return best_trace;
}

bool simulation::match(const state &left, const state &right)
{
  assert(left.size() == right.size());
  for (size_t i = 0; i < left.size(); i++)
  {
    if (!is_variable(left[i]) && !is_variable(right[i]) && left[i] != right[i])
    {
      return false;
    }
  }
  return true;
}
