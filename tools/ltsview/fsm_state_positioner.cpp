// Author(s): Bas Ploeger
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>
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
  // The details of this algorithm can be found in Frank van Ham's master's
  // thesis, pp. 21-29
  for (Cluster_iterator cluster_it = lts->getClusterIterator();
      !cluster_it.is_end(); ++cluster_it)
  {
    createClusterSlots(*cluster_it);
  }
  bottomUpPass();
  topDownPass();
  resolveClusterSlots();
}

void FSMStatePositioner::markStateUndecided(State* state)
{
  state->center();
  if (state->getCluster()->isCentered())
  {
    todo_top_down.push_back(state);
  }
  else
  {
    addUndecidedState(state->getCluster(), state);
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
      float(NUM_RINGS - 1) /
      state->getCluster()->getTopRadius());
  ring = min(ring, NUM_RINGS - 1);
  if (ring == 0)
  {
    state->center();
    occupyCenterSlot(state->getCluster(), state);
  }
  else
  {
    float angle = position.toDegrees();
    state->setPositionAngle(angle);
    state->setPositionRadius(state->getCluster()->getTopRadius() *
        float(ring) / float(NUM_RINGS - 1));
    occupySlot(state->getCluster(), ring, angle, state);
  }
}

void FSMStatePositioner::bottomUpPass()
{
  // Phase 1: Process states bottom-up, keeping edges as short as possible.
  for (Reverse_cluster_iterator ci = lts->getReverseClusterIterator();
      !ci.is_end(); ++ci)
  {
    Cluster* cluster = *ci;
    if (cluster->getNumStates() == 1)
    {
      State* state = cluster->getState(0);
      state->center();
      occupyCenterSlot(cluster, state);
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
  // Phase 2: Process states top-down, keeping edges as short as possible.
  for (vector< State* >::reverse_iterator state_it = todo_top_down.rbegin();
      state_it != todo_top_down.rend(); ++state_it)
  {
    State* state = *state_it;
    vector< State* > predecessors;
    getPredecessors(state, predecessors);
    if (allStatesCentered(predecessors))
    {
      addUndecidedState(state->getCluster(), state);
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
    resolveSlots(*ci);
  }
}

int FSMStatePositioner::getNumSlots(Cluster* cluster, unsigned int
    ring)
{
  return slots[cluster][ring].size();
}

int FSMStatePositioner::getTotalNumSlots(Cluster* cluster)
{
  unsigned int result = 0;
  for (unsigned int r = 0; r < NUM_RINGS; ++r)
  {
    result += getNumSlots(cluster, r);
  }
  return result;
}

void FSMStatePositioner::occupySlot(Cluster* cluster, unsigned int ring, float
    pos, State* state)
{
  int slot = round_to_int(double(pos) * double(getNumSlots(cluster, ring)) /
      360.0);
  if (slot == getNumSlots(cluster, ring))
  {
    slot = 0;
  }
  slots[cluster][ring][slot].push_back(state);
}

void FSMStatePositioner::occupyCenterSlot(Cluster* cluster, State* state)
{
  slots[cluster][0][0].push_back(state);
}

void FSMStatePositioner::addUndecidedState(Cluster* cluster, State *state)
{
  undecided[cluster].push_back(state);
}

void FSMStatePositioner::resolveSlots(Cluster* cluster)
{
  unsigned int from = 0;
  unsigned int to = 0;
  for (unsigned int r = 0; r < NUM_RINGS; ++r)
  {
    to = from + round_to_int(undecided[cluster].size() * float(r) /
        float(NUM_RINGS));
    if (to > undecided[cluster].size() || r == NUM_RINGS - 1)
    {
      to = undecided[cluster].size();
    }
    slotUndecided(cluster, r, from, to);
    spreadSlots(cluster, r);
    from = to;
  }
  undecided[cluster].clear();
}

void FSMStatePositioner::slotUndecided(Cluster* cluster, unsigned int ring, unsigned int
    from, unsigned int to)
{
  unsigned int remainingStates = to - from;
  const int numSlots = getNumSlots(cluster, ring);
  const float radius = cluster->getTopRadius() * float(ring) /
    float(NUM_RINGS - 1);
  int start = 0;
  while (start < numSlots && slots[cluster][ring][start].size() == 0)
  {
    ++start;
  }
  float sp,diff;
  unsigned int i;
  if (start == numSlots)
  {
    // divide all undecided states over the complete ring
    sp = 0.0f;
    diff = 360.0f / remainingStates;
    for (i = from; i < to; ++i)
    {
      undecided[cluster][i]->setPositionRadius(radius);
      undecided[cluster][i]->setPositionAngle(sp);
      occupySlot(cluster, ring, sp, undecided[cluster][i]);
      sp += diff;
    }
  }
  else
  {
    int gapBegin,gapEnd,gapSize,space,lGapBegin,lGapEnd,lGapSize;
    unsigned int M,s;
    float slotDiff = 360.0f / numSlots;
    s = from;
    while (remainingStates > 0)
    {
      // compute the total space and largest gap size
      space = 0;
      lGapBegin = start;
      lGapEnd = start;
      lGapSize = 0;
      gapBegin = start;
      gapEnd = (gapBegin+1) % numSlots;
      while (gapEnd != start)
      {
        gapSize = 1;
        gapEnd = (gapBegin+1) % numSlots;
        while (slots[cluster][ring][gapEnd].size() == 0)
        {
          ++gapSize;
          gapEnd = (gapEnd+1) % numSlots;
        }
        if (gapSize > 1)
        {
          space += gapSize;
          if (gapSize > lGapSize)
          {
            lGapBegin = gapBegin;
            lGapEnd = gapEnd;
            lGapSize = gapSize;
          }
        }
        gapBegin = gapEnd;
      }

      M = int(1.0f + float(remainingStates) * float(lGapSize) / float(space));
      M = min(M, remainingStates);
      // divide M states over the largest gap found
      sp = lGapBegin * slotDiff;
      diff = lGapSize * slotDiff / M;
      for (i = s; i < s+M; ++i)
      {
        undecided[cluster][i]->setPositionRadius(radius);
        undecided[cluster][i]->setPositionAngle(sp);
        occupySlot(cluster, ring, sp, undecided[cluster][i]);
        sp += diff;
        if (sp >= 360.0f)
        {
          sp -= 360.0f;
        }
      }
      s += M;
      remainingStates -= M;
    }
  }
}

void FSMStatePositioner::spreadSlots(Cluster* cluster, unsigned int ring)
{
  if (ring == 0)
  {
    // TODO: spread the states in the center of the cluster
    return;
  }
  const unsigned int numSlots = getNumSlots(cluster, ring);
  const float radius = cluster->getTopRadius() * float(ring) /
    float(NUM_RINGS-1);
  const float slot_diff = 360.0f / numSlots;

  vector< float > slot_space_cw(numSlots, 0.0f);
  vector< float > slot_space_ccw(numSlots, 0.0f);
  unsigned int s,i;
  // compute the amount of space available for each slot
  for (s = 0; s < numSlots; ++s)
  {
    if (slots[cluster][ring][s].size() > 1)
    {
      // space in counter-clockwise direction
      i = 1;
      while (slots[cluster][ring][(s+i)%numSlots].size() == 0)
      {
        ++i;
      }
      slot_space_ccw[s] = i * slot_diff;
      if (slots[cluster][ring][(s+i) % numSlots].size() > 1)
      {
        slot_space_ccw[s] *= 0.5f;
      }
      slot_space_ccw[s] -= rad_to_deg(0.25f / cluster->getTopRadius());

      // space in clockwise direction
      i = 1;
      while (slots[cluster][ring][(s-i) % numSlots].size() == 0)
      {
        ++i;
      }
      slot_space_cw[s] = i * slot_diff;
      if (slots[cluster][ring][(s-i) % numSlots].size() == 1)
      {
        slot_space_cw[s] -= rad_to_deg(0.25f / cluster->getTopRadius());
      }
      else
      {
        slot_space_cw[s] *= 0.5f;
      }
    }
  }

  float r,a,slot_angle;
  unsigned int numStates;
  for (s = 0; s < numSlots; ++s)
  {
    slot_angle = s * slot_diff;
    numStates = slots[cluster][ring][s].size();
    if (numStates == 1)
    {
      slots[cluster][ring][s][0]->setPositionAngle(slot_angle);
      slots[cluster][ring][s][0]->setPositionRadius(radius);
    }
    if (numStates > 1)
    {
      i = 0;
      r = radius;
      while (r > 0.15f && i < numStates)
      {
        a = slot_angle - slot_space_cw[s];
        while ((a < slot_angle + slot_space_ccw[s]) && i < numStates)
        {
          slots[cluster][ring][s][i]->setPositionAngle(a);
          slots[cluster][ring][s][i]->setPositionRadius(r);
          ++i;
          a += rad_to_deg(0.25f/r);
        }
        r -= 0.25f;
      }
      // if i < numStates, then there was not enough room for positioning all
      // states in this slot, so put all of the remaining states in the slot
      // position (this is a panic situation, that should occur very rarely)
      while (i < numStates)
      {
        slots[cluster][ring][s][i]->setPositionAngle(slot_angle);
        slots[cluster][ring][s][i]->setPositionRadius(radius);
        ++i;
      }
    }
  }
}

void FSMStatePositioner::clearSlots(Cluster* cluster)
{
  unsigned int i,j;
  for (i = 0; i < slots[cluster].size(); ++i)
  {
    for (j = 0; j < slots[cluster][i].size(); ++j)
    {
      slots[cluster][i][j].clear();
    }
  }
  undecided[cluster].clear();
}

void FSMStatePositioner::createClusterSlots(Cluster* cluster)
{
  int numSlots;
  if (cluster->getNumDescendants() == 0)
  {
    numSlots = cluster->getNumStates();
  }
  else if (cluster->getNumDescendants() == 1)
  {
    numSlots = min(32, 2 * getTotalNumSlots(cluster->getDescendant(0)));
  }
  else
  { 
    Cluster* centered_descendant = NULL;
    for (int i = 0; i < cluster->getNumDescendants(); ++i)
    {
      if (cluster->getDescendant(i)->isCentered())
      {
        centered_descendant = cluster->getDescendant(i);
        break;
      }
    }
    if (centered_descendant)
    {
      numSlots = 2 * getTotalNumSlots(centered_descendant);
    }
    else
    {
      numSlots = 2 * cluster->getNumDescendants();
    }
    numSlots = min(32, numSlots);
  }
  int S;
  for (unsigned int r = 0; r < NUM_RINGS; ++r)
  {
    if (r == 0)
    {
      S = 1;
    }
    else
    {
      S = round_to_int(numSlots * float(r) / float(NUM_RINGS));
    }
    vector< vector< State* > > slots_r;
    for (int s = 0; s < S; ++s)
    {
      vector< State* > slots_rs;
      slots_r.push_back(slots_rs);
    }
    slots[cluster].push_back(slots_r);
  }
}
