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
// Boost signals and slots
#include <boost/signal.hpp>
#include <boost/bind.hpp>

class Transition;
class State;

class Simulation
{
  public:
    typedef boost::signal<void ()> simulationSignal;
    typedef boost::signals::connection simConnection;

  public:
    Simulation();
    ~Simulation();

    void start();
    void stop();
    void setInitialState(State* initialState);

    std::vector< Transition* > const& getTransHis() const;
    std::vector< State*> const& getStateHis() const;
    State* getCurrState() const;
    std::vector< Transition* > const& getPosTrans() const;
    Transition* getChosenTrans() const;
    int getChosenTransi() const;
    bool getStarted() const;

    // Generates a back trace to initState.
    // Pre: initState is the initial state for the entire (top level) LTS
    void traceBack(State* initState);
    void chooseTrans(int i);
    void followTrans();
    void undoStep();
    void resetSim();

    simConnection connect(simulationSignal::slot_function_type subscriber);
    simConnection connectSel(simulationSignal::slot_function_type subscriber);
    void disconnect(simConnection subscriber);

  private:
    bool started;
    int chosenTrans;
    State* initialState;
    State* currState;
    std::vector< Transition* > posTrans;
    std::vector< State* > stateHis;
    std::vector< Transition* > transHis;
    simulationSignal signal;
    simulationSignal selChangeSignal;
};

#endif //SIMULATION_H
