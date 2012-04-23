// Author(s): Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef LPSXSIM_SIMULATION_H
#define LPSXSIM_SIMULATION_H

#include <QList>
#include <QMutex>
#include <QObject>
#include <QString>
#include <QVector>

#include "mcrl2/data/rewriter.h"
#include "mcrl2/lps/simulation.h"

class Simulation : public QObject
{
  struct Transition
  {
    QString destination;
    QString action;
  };
  struct State
  {
    QString state;
    QVector<Transition> transitions;
    size_t transition_number;
  };
  typedef QList<State> Trace;

  Q_OBJECT

  public:
  Simulation(const std::string& filename, mcrl2::data::rewrite_strategy strategy);
  ~Simulation() { delete m_simulation; }
  Trace trace() { QMutexLocker locker(&m_traceMutex); return m_trace; }

  private slots:
  void init();
  void updateTrace(size_t first_changed_state);

  public slots:
  void reset(size_t state_number) { m_simulation->truncate(state_number); updateTrace(state_number); }
  void select(size_t transition_number) { m_simulation->select(transition_number); updateTrace(m_simulation->trace().size() - 1); }
  void enable_tau_prioritization(bool enable, QString action = "ctau") { m_simulation->enable_tau_prioritization(enable, action.toStdString()); updateTrace(0); }
  void load(QString filename);
  void save(QString filename);

  signals:
  void traceChanged();
  void error(QString message);

  private:
  std::string m_filename;
  mcrl2::data::rewrite_strategy m_strategy;

  mcrl2::lps::simulation *m_simulation;
  Trace m_trace;
  QMutex m_traceMutex;
};

#endif
