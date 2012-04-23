// Author(s): Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "simulation.h"
#include <QMetaObject>
#include "mcrl2/utilities/atermthread.h"

Simulation::Simulation(const std::string& filename, mcrl2::data::rewrite_strategy strategy)
  : m_filename(filename),
    m_strategy(strategy)
{
  moveToThread(mcrl2::utilities::qt::get_aterm_thread());
  QMetaObject::invokeMethod(this, "init");
}

void Simulation::init()
{
  // TODO: error handling
  mcrl2::lps::specification specification;
  specification.load(m_filename);

  m_simulation = new mcrl2::lps::simulation(specification, m_strategy);
}

void Simulation::updateTrace(size_t first_changed_state)
{
  QMutexLocker locker(&m_traceMutex);

  m_trace.erase(m_trace.begin() + first_changed_state, m_trace.end());

  for (size_t i = first_changed_state; i < m_simulation->trace().size(); i++)
  {
    State state;
    state.state = QString::fromStdString(mcrl2::lps::pp(m_simulation->trace()[i].source_state));
    state.transition_number = m_simulation->trace()[i].transition_number;
    for (size_t j = 0; j < m_simulation->trace()[i].transitions.size(); j++)
    {
      Transition transition;
      transition.destination = QString::fromStdString(mcrl2::lps::pp(m_simulation->trace()[i].transitions[j].destination));
      transition.action = QString::fromStdString(mcrl2::lps::pp(m_simulation->trace()[i].transitions[j].action));
      state.transitions += transition;
    }
    m_trace += state;
  }

  emit traceChanged();
}

void Simulation::load(QString filename)
{
  try
  {
    m_simulation->load(filename.toStdString());
  }
  catch (mcrl2::runtime_error& e)
  {
    emit error(e.what());
    return;
  }
  catch (...)
  {
    emit error("Unknown error");
  }
}

void Simulation::save(QString filename)
{
  try
  {
    m_simulation->save(filename.toStdString());
  }
  catch (mcrl2::runtime_error& e)
  {
    emit error(e.what());
    return;
  }
  catch (...)
  {
    emit error("Unknown error");
  }
}


