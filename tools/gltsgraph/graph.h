#ifndef GRAPH_H 
#define GRAPH_H

#include <vector>
#include "state.h"

class Graph
{
  public:
    // Constructor
    Graph();

    // Destructor
    ~Graph();

    // Methods
    void addState(State* n);
    size_t getNumberOfStates();
    State* selectState(size_t s);
    Transition* selectTransition(size_t s, size_t t);
    Transition* selectSelfLoop(size_t s, size_t t);

    // Setters
    void setInitialState(State* i);
    void setInfo(int is, int ns, int nt, int nl);
    
    // Getters
    State* getInitialState() const;
    State* getState(size_t i) const;
    int getInitial() const;
    int getNumStates() const;
    int getNumTrans() const;
    int getNumLabels() const;
    


  private:
    std::vector<State*> states;
    State* initialState;

    int initial;
    int numStates;
    int numTrans;
    int numLabels;
};

#endif //GRAPH_H
