// Author(s): Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <QMetaObject>
#include "mcrl2/lps/io.h"
#include "mcrl2/lps/detail/instantiate_global_variables.h"
#include "simulation.h"

void Simulation::init(const QString& filename, bool do_not_use_dummies)
{
  // mcrl2::lps::stochastic_specification spec;
  try
  {
    load_lps(m_stochastic_spec, filename.toStdString());
  }
  catch (mcrl2::runtime_error& e)
  {
    mCRL2log(mcrl2::log::error) << "Error loading LPS: " << e.what() << std::endl;
    return;
  }
  catch (...)
  {
    mCRL2log(mcrl2::log::error) << "Error loading LPS: unknown error" << std::endl;
    return;
  }

  if (!do_not_use_dummies)
  {
    mcrl2::lps::detail::instantiate_global_variables(m_stochastic_spec);
  }
    
  m_simulation = new mcrl2::lps::simulation(m_stochastic_spec, m_strategy);

  for (const mcrl2::data::variable& v: m_stochastic_spec.process().process_parameters())
  {
    m_parameters += QString::fromStdString(mcrl2::data::pp(v));
  }

  updateTrace(0);
  m_initialized = true;
  emit initialisationDone();
}

void Simulation::updateTrace(unsigned long long firstChangedState)
{
  QMutexLocker locker(&m_traceMutex);

  m_trace.erase(m_trace.begin() + firstChangedState, m_trace.end());

  for (unsigned long long i = firstChangedState; i < m_simulation->trace().size(); i++)
  {
    assert(m_simulation->trace()[i].source_state.size() > 0);
    TracePosition position;
    const mcrl2::lps::simulation::simulator_state_t& current_state = m_simulation->trace()[i];
    if (m_simulation->probabilistic_state_must_be_selected(i)) // A probabilistic state must be selected.
    {
      // Set up the selection of a probabilistic state.
      position.is_probabilistic = true;
      for (unsigned long long j = 0; j < m_simulation->trace()[i].source_state.size(); j++)
      {
        Transition outgoing_probabilities;
        outgoing_probabilities.action_or_probability = QString::fromStdString(mcrl2::lps::pp(m_simulation->trace()[i].source_state.probabilities[j]));
        outgoing_probabilities.destination = renderState(current_state.source_state.states[j]);  
        position.transitions += outgoing_probabilities;
        position.state = QStringList(QString::fromStdString("Probabilistic state of size " +
                                                                       std::to_string(current_state.source_state.size())));
      }
    }
    else
    {
      // Set up the selection of a probabilistic transition.
      position.is_probabilistic = false;
      position.state = renderState(current_state.source_state.states[current_state.state_number]);
      position.transitionNumber = m_simulation->trace()[i].transition_number;
      for (unsigned long long j = 0; j < m_simulation->trace()[i].transitions.size(); j++)
      {
        Transition transition;
        if (current_state.transitions[j].state.size()==1)
        {
          transition.destination = renderState(current_state.transitions[j].state.states[0]);
        }
        else
        {
          transition.destination = QStringList(QString::fromStdString("Probabilistic state of size " + 
                                                                       std::to_string(current_state.transitions[j].state.size())));
        }
        transition.action_or_probability = QString::fromStdString(mcrl2::lps::pp(m_simulation->trace()[i].transitions[j].action));
        position.transitions += transition;
      }
    }
    m_trace += position;
  }
}

Simulation::State Simulation::renderState(const mcrl2::lps::state& state)
{
  State output;
  for (unsigned long long i = 0; i < state.size(); i++)
  {
    if (mcrl2::data::is_variable(state[i]))
    {
      output += "_";
    }
    else
    {
      output += QString::fromStdString(mcrl2::data::pp(state[i]));
    }
  }
  return output;
}

void Simulation::select(unsigned long long transitionNumber, unsigned long long selected_state, QSemaphore *semaphore)
{
  assert(selected_state<static_cast<unsigned long long>(m_trace.size()));
  if (m_trace[selected_state].is_probabilistic)
  {
    m_simulation->select_state(transitionNumber);
  }
  else
  {
    m_simulation->truncate(selected_state, false);
    m_simulation->select_transition(transitionNumber);
  }
  updateTrace(m_trace.size() - 1);
  if (semaphore)
  {
    semaphore->release();
  }
  emit finished();
}

void Simulation::auto_select_state_or_probability(unsigned long long selected_state, QSemaphore *semaphore)
{
  if (m_trace[selected_state].is_probabilistic)
  {
    m_simulation->randomly_select_state();
  }
  else
  {
    m_simulation->truncate(selected_state, false);
    m_simulation->randomly_select_transition();
  }
  updateTrace(m_trace.size() - 1);
  if (semaphore)
  {
    semaphore->release();
  }
  emit finished();
}

void Simulation::enable_auto_select_probability(bool enable, QSemaphore *semaphore)
{
  m_simulation->enable_auto_select_probability(enable);
  if (semaphore)
  {
    semaphore->release();
  }
  emit finished();
}

void Simulation::load(QString filename)
{
  try
  {
    m_simulation->load(filename.toStdString());
  }
  catch (mcrl2::runtime_error& e)
  {
    mCRL2log(mcrl2::log::error) << "Error loading trace: " << e.what() << std::endl;
    return;
  }
  catch (...)
  {
    mCRL2log(mcrl2::log::error) << "Error loading trace: unknown error" << std::endl;
    return;
  }
  updateTrace(0);
}

void Simulation::save(QString filename)
{
  try
  {
    m_simulation->save(filename.toStdString());
  }
  catch (mcrl2::runtime_error& e)
  {
    mCRL2log(mcrl2::log::error) << "Error saving trace: " << e.what() << std::endl;
  }
  catch (...)
  {
    mCRL2log(mcrl2::log::error) << "Error saving trace: unknown error" << std::endl;
  }
}
