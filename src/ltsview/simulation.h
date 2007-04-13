#ifndef SIMULATION_H
#define SIMULATION_H

#ifndef TRANSITION_H
  #include "transition.h"
#else
  class Transition;
#endif //TRANSITION_H

#ifndef STATE_H
  #include "state.h"
#else
  class State;
#endif //STATE_H

// Boost signals and slots
#include <boost/signal.hpp>
#include <boost/bind.hpp>

class Simulation {
  public:
    typedef boost::signal<void ()>      simulationSignal;
    typedef boost::signals::connection  simConnection;
  
  public:
    Simulation();
    ~Simulation();
   
    void start(State* initialState);
    void stop();

    std::vector< int > const&   getTransHis()         const;
    std::vector< State*>       const&   getStateHis()         const;
    State*                              getCurrState()        const;
    std::vector< Transition* > const&   getPosTrans()         const;
    int                                 getChosenTrans()      const;
    bool                                getStarted()          const;
    
    
    void                                chooseTrans( int );
    void                                followTrans();
    void                                undoStep();
    void                                resetSim();

    simConnection connect(simulationSignal::slot_function_type subscriber);
    simConnection connectSel(simulationSignal::slot_function_type subscriber);
    void          disconnect(simConnection subscriber);
   
  private:
    bool                        started;
    int                         chosenTrans;
    State*                      currState;
    std::vector< Transition* >  posTrans;
    std::vector< State* >       stateHis;
    std::vector< int >          transHis;
    simulationSignal            signal;  
    simulationSignal            selChangeSignal;
};

#endif //SIMULATION_H
