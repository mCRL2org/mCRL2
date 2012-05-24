// Author(s): Bas Ploeger and Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file simulation.h
/// \brief Header file for Simulation class

#ifndef SIMULATION_H
#define SIMULATION_H

#include <QObject>

#include <vector>

#include "lts.h"

class Transition;
class State;

class Simulation : public QObject
{
  Q_OBJECT

  public:
    Simulation(LTS *lts);
    ~Simulation();
    void operator=(const Simulation &other);

  public slots:
    void start();
    void stop();
    void setInitialState(State* initialState);
    void chooseTrans(int i);
    void followTrans();
    void undoStep();
    void resetSim();

  public:
    LTS *getLTS() const { return lts; }
    std::vector< Transition* > const& getTransHis() const { return transHis; }
    std::vector< State*> const& getStateHis() const { return stateHis; }
    State* getCurrState() const { return currState; }
    std::vector< Transition* > const& getPosTrans() const { return posTrans; }
    Transition* getChosenTrans() const { return chosenTrans < 0 ? NULL : posTrans[chosenTrans]; }
    int getChosenTransi() const { return chosenTrans; }
    bool getStarted() const { return isStarted; }

    // Generates a back trace to initState.
    // Pre: initState is the initial state for the entire (top level) LTS
    void traceBack(State* initState);

  signals:
    void started();
    void changed();
    void selectionChanged();

  private:
    LTS *lts;
    bool isStarted;
    int chosenTrans;
    State* initialState;
    State* currState;
    std::vector< Transition* > posTrans;
    std::vector< State* > stateHis;
    std::vector< Transition* > transHis;
};

#endif //SIMULATION_H
