// Author(s): Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "simulation.h"
#include <QMetaObject>
#include "mcrl2/utilities/logger.h"
#include "mcrl2/lps/io.h"
#include "mcrl2/lps/detail/instantiate_global_variables.h"

Simulation::Simulation(QString filename, QThread *atermThread, mcrl2::data::rewrite_strategy strategy, const bool do_not_use_dummies)
  : m_strategy(strategy),
    m_initialized(false),
    m_simulation(NULL)
{
  moveToThread(atermThread);
  QMetaObject::invokeMethod(this, "init", Qt::BlockingQueuedConnection, Q_ARG(QString, filename), Q_ARG(bool, do_not_use_dummies));
}

void Simulation::init(const QString& filename, bool do_not_use_dummies)
{
  mcrl2::lps::stochastic_specification spec;
  try
  {
    load_lps(spec, filename.toStdString());
    m_simulation = new mcrl2::lps::simulation(spec, m_strategy);
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
    mcrl2::lps::detail::instantiate_global_variables(spec);
  }

  for (mcrl2::data::variable_list::const_iterator i = spec.process().process_parameters().begin(); i != spec.process().process_parameters().end(); i++)
  {
    m_parameters += QString::fromStdString(mcrl2::data::pp(*i));
  }

  updateTrace(0);
  m_initialized = true;
}

void Simulation::updateTrace(unsigned int firstChangedState)
{
  QMutexLocker locker(&m_traceMutex);

  m_trace.erase(m_trace.begin() + firstChangedState, m_trace.end());

  for (std::size_t i = firstChangedState; i < m_simulation->trace().size(); i++)
  {
    TracePosition position;
    position.state = renderState(m_simulation->trace()[i].source_state);
    position.transitionNumber = m_simulation->trace()[i].transition_number;
    for (std::size_t j = 0; j < m_simulation->trace()[i].transitions.size(); j++)
    {
      Transition transition;
      transition.destination = renderState(m_simulation->trace()[i].transitions[j].destination);
      transition.action = QString::fromStdString(mcrl2::lps::pp(m_simulation->trace()[i].transitions[j].action));
      position.transitions += transition;
    }
    m_trace += position;
  }
}

Simulation::State Simulation::renderState(const mcrl2::lps::state &state)
{
  State output;
  for (std::size_t i = 0; i < state.size(); i++)
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

void Simulation::select(unsigned int transitionNumber, QSemaphore *semaphore)
{
  m_simulation->select(transitionNumber);
  updateTrace(m_trace.size() - 1);
  if (semaphore)
  {
    semaphore->release();
  }
  emit finished();
}

void Simulation::enable_tau_prioritization(bool enable, QSemaphore *semaphore, QString action)
{
  m_simulation->enable_tau_prioritization(enable, action.toStdString());
  updateTrace(0);
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
