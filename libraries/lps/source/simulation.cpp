// Author(s): Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "mcrl2/lps/simulation.h"

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

void simulation::reset(size_t state_number)
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
      atermpp::vector<transition_t> &transitions = m_full_trace.back().transitions;
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

atermpp::vector<simulation::transition_t> simulation::transitions(state source_state)
{
  atermpp::vector<simulation::transition_t> output;
  for (next_state_generator::iterator i = m_generator.begin(source_state, &m_substitution); i != m_generator.end(); i++)
  {
    transition_t transition;
    transition.destination = i->state();
    transition.action = i->action();
    output.push_back(transition);
  }
  return output;
}

atermpp::vector<simulation::transition_t> simulation::prioritize(const atermpp::vector<simulation::transition_t> &transitions)
{
  atermpp::vector<simulation::transition_t> output;
  for (atermpp::vector<simulation::transition_t>::const_iterator i = transitions.begin(); i != transitions.end(); i++)
  {
    simulation::transition_t transition = *i;
    while (true)
    {
      bool found = false;
      for (next_state_generator::iterator j = m_generator.begin(transition.destination, &m_substitution); j != m_generator.end(); j++)
      {
        if (is_prioritized(j->action()))
        {
          transition.destination = j->state();
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

  for (atermpp::deque<state_t>::iterator i = m_prioritized_trace.begin(); i != m_prioritized_trace.end(); i++)
  {
    i->transitions = prioritize(transitions(i->source_state));
  }
}
