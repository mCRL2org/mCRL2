// Author(s): Bas Ploeger
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef FSM_STATE_POSITIONER_H
#define FSM_STATE_POSITIONER_H
#include <vector>
#include "state_positioner.h"
#include "vectors.h"

class LTS;
class State;

class FSMStatePositioner: public StatePositioner
{
  public:
    FSMStatePositioner(LTS *l): StatePositioner(l) {}
    ~FSMStatePositioner() {}
    void positionStates();

  private:
    std::vector< State* > unpositioned_states;
    bool allStatesCentered(std::vector< State* > &states);
    void assignStateToPosition(State* state, Vector2D &position);
    void bottomUpPass();
    void getPredecessors(State* state, std::vector< State* >& predecessors);
    void getSuccessors(State* state, std::vector< State* >& successors);
    void markStateUndecided(State* state);
    void resolveClusterSlots();
    Vector2D sumStateVectorsInMultipleClusters( std::vector< State* > &states,
        float rim_radius);
    Vector2D sumStateVectorsInSingleCluster(std::vector< State* > &states);
    void topDownPass();
};

#endif
