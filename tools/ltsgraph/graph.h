// Author(s): Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file graph.h
/// \brief Graph class.

#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include "state.h"
#include <wx/colour.h>

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
    void colourState(size_t s, wxColour colour);

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
    std::vector<std::string> parameters;
    State* initialState;

    int initial;
    int numStates;
    int numTrans;
    int numLabels;
};

#endif //GRAPH_H
