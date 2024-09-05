// Author(s): Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
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
#include <QThread>

#ifndef Q_MOC_RUN // Workaround for QTBUG-22829
#include "mcrl2/lts/simulation.h"
#endif // Q_MOC_RUN

class Simulation : public QObject
{
  Q_OBJECT

  public:
    typedef QStringList State;
    struct Transition
    {
      QString action_or_probability;
      State destination;
    };
    struct TracePosition
    {
      bool is_probabilistic = false; // If true the state is probabilistic, the state is not defined and the transitions represent 
                                     // probabilistic transitions with a probability and state to be choosen.
                                     // If false, state is defined and transitions represent outgoing transitions. 
      State state;
      QList<Transition> transitions;
      unsigned long long transitionNumber;
    };
    typedef QList<TracePosition> Trace;

  public:
    Simulation(mcrl2::data::rewrite_strategy strategy)
        : m_strategy(strategy), m_initialized(false), m_simulation(NULL) {}
    ~Simulation() 
    { 
      delete m_simulation; 
    }
    bool initialized() const { return m_initialized; }
    const QStringList& parameters() const { return m_parameters; }
    Trace trace() { QMutexLocker locker(&m_traceMutex); return m_trace; }
    State renderState(const mcrl2::lps::state& state);


  private slots:
    void init(const QString& filename, bool do_not_use_dummies);
    void updateTrace(unsigned long long firstChangedState);

  public slots:
    void reset(unsigned long long stateNumber, bool probabilistic) 
    { 
      m_simulation->truncate(stateNumber, probabilistic); 
      updateTrace(stateNumber); 
    }
    void select(unsigned long long transitionNumber, unsigned long long selected_state, QSemaphore *semaphore);
    void auto_select_state_or_probability(unsigned long long selected_state, QSemaphore *semaphore);
    void enable_auto_select_probability(bool enable, QSemaphore *semaphore);
    void load(QString filename);
    void save(QString filename);

  signals:
    void initialisationDone();
    void finished();

  private:
    mcrl2::data::rewrite_strategy m_strategy;
    bool m_initialized;
    mcrl2::lps::stochastic_specification m_stochastic_spec;

    mcrl2::lps::simulation *m_simulation;
    QStringList m_parameters;
    Trace m_trace;
    QMutex m_traceMutex;
};

#endif
