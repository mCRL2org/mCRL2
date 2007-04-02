#ifndef TRANSITION_H
#define TRANSITION_H
#ifndef STATE_H
  #include "state.h"
#else
  class State;
#endif

class Transition
{
  public:
    Transition(State* bs,State* es,int label);
    ~Transition();
    State*	getBeginState() const;
    State*	getEndState() const;
    int			getLabel() const;
    bool		isBackpointer() const;
    bool		isMarked() const;
    void		setBackpointer(bool b);
    void		setMarkedPointer(bool* bp);

    // Methods for simulation
    // Getters
    bool      isSelected() const;
    // PRE: True
    // RET: selected
    
    Utils::SimState  getSimulationState() const;
    // PRE: True
    // RET: simulationState

    int       getVisitedAt() const;
    // PRE: True
    // RET: visitedAt

    // Setters
    void      setSelected(bool s);
    // PRE: True
    // POST: selected = s

    void      setSimulationState(Utils::SimState st);
    // PRE: True
    // POST: simulationState = st;
    
    void      setVisitedAt(int va);
    // PRE:   0 <= va
    // POST:  visitedAt = va

  private:
    bool    backpointer;
    State*  beginState;
    State*  endState;
    int   label;
    bool*   marked;

    // Variables used for simulation.
    bool      selected;
    Utils::SimState  simulationState;
    int       visitedAt; // Used to calculate transparency
};

#endif
