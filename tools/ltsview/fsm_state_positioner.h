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
    static const unsigned int NUM_RINGS = 2;

    std::map< Cluster*, std::vector< State* > > undecided;
    std::map< Cluster*, std::vector< std::vector< std::vector< State* > > > > slots;
    std::vector< State* > todo_top_down;
    std::vector< State* > todo_resolve_slots;

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

    int getTotalNumSlots(Cluster* cluster);
    int getNumSlots(Cluster* cluster, unsigned int ring);
    void occupySlot(Cluster* cluster, unsigned int ring,float pos,State* s);
    void occupyCenterSlot(Cluster* cluster, State* s);
    void resolveSlots(Cluster* cluster);
    void clearSlots(Cluster* cluster);
    void addUndecidedState(Cluster* cluster, State* s);
    void slotUndecided(Cluster* cluster, unsigned int ring,unsigned int
        from,unsigned int to);
    void spreadSlots(Cluster* cluster, unsigned int ring);
    void createClusterSlots(Cluster* cluster);
};

#endif
