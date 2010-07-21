// Author(s): Bas Ploeger
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <vector>
#include "cluster.h"
#include "fsm_state_positioner.h"
#include "lts.h"
#include "mathutils.h"
#include "state.h"
#include "transition.h"
#include "vectors.h"

using namespace std;
using namespace MathUtils;

void FSMStatePositioner::positionStates() 
{
  bottomUpPass();
  topDownPass();
  resolveClusterSlots();
}

void FSMStatePositioner::markStateUndecided(State* state)
{
  state->center();
  if (state->getCluster()->isCentered())
  {
    unpositioned_states.push_back(state);
  }
  else
  {
    state->getCluster()->addUndecidedState(state);
  }
}

Vector2D FSMStatePositioner::sumStateVectorsInSingleCluster(
    vector< State* > &states)
{
  Vector2D sum_vector = Vector2D(0, 0);
  for (vector< State* >::iterator state_it = states.begin(); state_it !=
      states.end(); ++state_it)
  {
    State* state = *state_it;
    if ( ! state->isCentered() )
    {
      sum_vector += Vector2D(state->getPositionAngle()) *
          state->getPositionRadius();
    }
  }
  return sum_vector;
}

Vector2D FSMStatePositioner::sumStateVectorsInMultipleClusters(
    vector< State* > &states, float rim_radius)
{
  Vector2D sum_vector = Vector2D(0, 0);
  for (vector< State* >::iterator state_it = states.begin(); state_it !=
      states.end(); ++state_it)
  {
    State* state = *state_it;
    if (state->getCluster()->isCentered())
    {
      if ( ! state->isCentered() )
      {
        sum_vector += Vector2D(state->getPositionAngle()) *
            state->getPositionRadius();
      }
    }
    else
    {
      sum_vector += Vector2D(state->getCluster()->getPosition()) *
        rim_radius;
      if ( ! state->isCentered() )
      {
        sum_vector += Vector2D(state->getPositionAngle() +
            state->getCluster()->getPosition()) *
          state->getPositionRadius();
      }
    }
  }
  return sum_vector;
}

bool FSMStatePositioner::allStatesCentered(vector< State* > &states)
{
  for (vector< State* >::iterator state = states.begin(); state !=
      states.end(); ++state)
  {
    if ( ! (**state).isCentered() )
    {
      return false;
    }
  }
  return true;
}

void FSMStatePositioner::getPredecessors(State* state, vector< State* >&
    predecessors)
{
  for (int t = 0; t < state->getNumInTransitions(); ++t)
  {
    State* predecessor = state->getInTransition(t)->getBeginState();
    if (predecessor->getRank() == state->getRank() - 1)
    {
      predecessors.push_back(predecessor);
    }
  }
}

void FSMStatePositioner::getSuccessors(State* state, vector< State* >&
    successors)
{
  for (int t = 0; t < state->getNumOutTransitions(); ++t)
  {
    State* successor = state->getOutTransition(t)->getEndState();
    if (successor->getRank() == state->getRank() + 1)
    {
      successors.push_back(successor);
    }
  }
}

void FSMStatePositioner::assignStateToPosition(State* state, Vector2D&
    position)
{
  unsigned int ring = round_to_int( position.length() *
      float(Cluster::NUM_RINGS - 1) /
      state->getCluster()->getTopRadius());
  ring = min(ring, Cluster::NUM_RINGS - 1);
  if (ring == 0)
  {
    state->center();
    state->getCluster()->occupyCenterSlot(state);
  }
  else
  {
    float angle = position.toDegrees();
    state->setPositionAngle(angle);
    state->setPositionRadius(state->getCluster()->getTopRadius() *
        float(ring) / float(Cluster::NUM_RINGS - 1));
    state->getCluster()->occupySlot(ring, angle, state);
  }
}

void FSMStatePositioner::bottomUpPass()
{
  //Phase 1: Processes states bottom-up, keeping edges as short as possible.
  //Pre:  clustersInRank is correctly sorted by rank.
  //Post: states are positioned bottom up, keeping edges as
  //      short as possible, if enough information is available.
  //Ret:  states that could not be placed in this phase, sorted bottom-up
  //
  // The details of this algorithm can be found in  Frank van Ham's master's
  // thesis, pp. 21-29

  // Iterate over the ranks in reverse order (bottom-up):
  for (Reverse_cluster_iterator ci = lts->getReverseClusterIterator();
      !ci.is_end(); ++ci)
  {
    Cluster* cluster = *ci;
    if (cluster->getNumStates() == 1)
    {
      State* state = cluster->getState(0);
      state->center();
      cluster->occupyCenterSlot(state);
      continue;
    }
    if (cluster->getNumDescendants() == 0)
    {
      for (int s = 0; s < cluster->getNumStates(); ++s)
      {
        markStateUndecided(cluster->getState(s));
      }
    }
    else if (cluster->getNumDescendants() == 1)
    {
      for (int s = 0; s < cluster->getNumStates(); ++s)
      {
        State* state = cluster->getState(s);
        vector< State* > successors;
        getSuccessors(state, successors);
        if (allStatesCentered(successors))
        {
          markStateUndecided(state);
        }
        else
        {
          Vector2D position = sumStateVectorsInSingleCluster(successors);
          assignStateToPosition(state, position);
        }
      }
    }
    else
    {
      for (int s = 0; s < cluster->getNumStates(); ++s)
      {
        State* state = cluster->getState(s);
        vector< State* > successors;
        getSuccessors(state, successors);
        Vector2D position = sumStateVectorsInMultipleClusters(
            successors, cluster->getBaseRadius());
        assignStateToPosition(state, position);
      }
    }
  }
}

void FSMStatePositioner::topDownPass()
{
  /* Phase 2: Process states top-down, keeping edges as short as
   * possible.
   * Pre:  unpositioned_states is correctly sorted by rank, bottom-up.
   * Post: unpositioned_states contains the states that could not be placed by this
   *       phase, sorted top-down.
   */
  for (vector< State* >::reverse_iterator state_it =
      unpositioned_states.rbegin(); state_it !=
      unpositioned_states.rend(); ++state_it)
  {
    State* state = *state_it;
    vector< State* > predecessors;
    getPredecessors(state, predecessors);
    if (allStatesCentered(predecessors))
    {
      state->getCluster()->addUndecidedState(state);
    }
    else
    {
      Vector2D position = sumStateVectorsInSingleCluster(predecessors);
      assignStateToPosition(state, position);
    }
  }
}

void FSMStatePositioner::resolveClusterSlots()
{
  //Resolves the slots of each cluster, positioning the states within each slot
  //in such a way that they do not overlap.
  for (Cluster_iterator ci = lts->getClusterIterator(); !ci.is_end(); ++ci)
  {
    (**ci).resolveSlots();
  }
}

