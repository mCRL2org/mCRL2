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
#include <QSemaphore>
#include <QString>
#include <QStringList>

#ifndef Q_MOC_RUN // Workaround for QTBUG-22829
#include "mcrl2/data/rewriter.h"
#include "mcrl2/lps/simulation.h"
#endif // Q_MOC_RUN

class Simulation : public QObject
{
  Q_OBJECT

  public:
    typedef QStringList State;
    struct Transition
    {
      State destination;
      QString action;
    };
    struct TracePosition
    {
      State state;
      QList<Transition> transitions;
      size_t transitionNumber;
    };
    typedef QList<TracePosition> Trace;

  public:
    Simulation(QString filename, mcrl2::data::rewrite_strategy strategy, const bool do_not_use_dummies);
    ~Simulation() { delete m_simulation; }
    bool initialized() const { return m_initialized; }
    const QStringList &parameters() const { return m_parameters; }
    Trace trace() { QMutexLocker locker(&m_traceMutex); return m_trace; }

  private slots:
    void init();
    void updateTrace(unsigned int firstChangedState);

  private:
    State renderState(const mcrl2::lps::state &state);

  public slots:
    void reset(unsigned int stateNumber) { m_simulation->truncate(stateNumber); updateTrace(stateNumber); }
    void select(unsigned int transitionNumber, QSemaphore *semaphore);
    void enable_tau_prioritization(bool enable, QSemaphore *semaphore, QString action = "ctau");
    void load(QString filename);
    void save(QString filename);

  signals:
    void finished();

  private:
    QString m_filename;
    mcrl2::data::rewrite_strategy m_strategy;
    bool m_initialized;
    bool m_do_not_use_dummies;

    mcrl2::lps::simulation *m_simulation;
    QStringList m_parameters;
    Trace m_trace;
    QMutex m_traceMutex;
};

#endif
