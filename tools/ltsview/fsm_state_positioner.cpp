// Author(s): Bas Ploeger
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>
#include <limits>
#include <queue>
#include <set>
//TODO(sploeger): remove this include once time measurements are finished.
#include <time.h>
#include <vector>
#include "cluster.h"
#include "fsm_state_positioner.h"
#include "lts.h"
#include "mathutils.h"
#include "rtree.h"
#include "state.h"
#include "transition.h"
#include "vectors.h"

using namespace std;
using namespace MathUtils;

class ClusterSlotInfo
{
  public:
    ClusterSlotInfo(Cluster* cluster);
    ~ClusterSlotInfo();
    Vector2D findFarthestFreeSlot(const Vector2D& position);
    Vector2D findNearestFreeSlot(const Vector2D& position);
    void occupySlot(const Vector2D& position);

    bool isCenter(float radius)
    {
      return radius < 0.5f * delta_ring;
    }

  private:
    static const float MIN_DELTA_RING = 0.22f;
    static const float MIN_DELTA_SLOT = 0.22f;

    float delta_ring;
    RTree* slot_rtree;
};

ClusterSlotInfo::ClusterSlotInfo(Cluster* cluster)
  : slot_rtree(NULL)
{
  int num_rings = 1 + static_cast<int>(cluster->getTopRadius() /
      MIN_DELTA_RING);
  if (num_rings > 1)
  {
    delta_ring = cluster->getTopRadius() / static_cast<float>(num_rings - 1);
  }
  else
  {
    delta_ring = 1.0f;
  }
  PackedRTreeBuilder rtree_builder = PackedRTreeBuilder();
  for (int ring = 0; ring < num_rings; ++ring)
  {
    float radius = ring * delta_ring;
    // max with 1 to ensure that ring 0 also has a slot
    int num_slots = max(1, static_cast< int >(2.0f * PI * radius /
          MIN_DELTA_SLOT));
    float delta_deg = 360.0f / static_cast< float >(num_slots);
    for (int slot = 0; slot < num_slots; ++slot)
    {
      float angle = delta_deg * slot;
      Vector2D point;
      point.fromPolar(angle, radius);
      rtree_builder.addPoint(point);
    }
  }
  rtree_builder.buildRTree();
  slot_rtree = rtree_builder.getRTree();
}

ClusterSlotInfo::~ClusterSlotInfo()
{
  if (slot_rtree != NULL)
  {
    delete slot_rtree;
  }
}

void ClusterSlotInfo::occupySlot(const Vector2D& position)
{
  slot_rtree->deletePoint(position);
}

Vector2D ClusterSlotInfo::findNearestFreeSlot(const Vector2D& position)
{
  Vector2D slot_pos = position;
  slot_rtree->findNearestNeighbour(position);
  if (slot_rtree->hasFoundNeighbour())
  {
    slot_pos = slot_rtree->foundNeighbour();
  }
  return slot_pos;
}

Vector2D ClusterSlotInfo::findFarthestFreeSlot(const Vector2D& position)
{
  Vector2D slot_pos = position;
  slot_rtree->findFarthestNeighbour(position);
  if (slot_rtree->hasFoundNeighbour())
  {
    slot_pos = slot_rtree->foundNeighbour();
  }
  return slot_pos;
}


FSMStatePositioner::FSMStatePositioner(LTS *l)
  : StatePositioner(l)
{
  for (Cluster_iterator cluster_it = lts->getClusterIterator();
      ! cluster_it.is_end(); ++cluster_it)
  {
    slot_info[*cluster_it] = new ClusterSlotInfo(*cluster_it);
  }
}

FSMStatePositioner::~FSMStatePositioner()
{
  for (map< Cluster*, ClusterSlotInfo* >::iterator slot_info_it =
      slot_info.begin(); slot_info_it != slot_info.end(); ++slot_info_it)
  {
    delete slot_info_it->second;
  }
}

void FSMStatePositioner::positionStates()
{
  // This algorithm has been based on the one by Frank van Ham, and includes
  // several improvements.
  std::cerr << lts->getNumClusters() << " clusters" << std::endl;
  std::cerr << lts->getNumRanks() << " ranks" << std::endl;
  std::cerr << "bottom up pass" << std::endl;
  bottomUpPass();
  std::cerr << "top down pass" << std::endl;
  topDownPass();
  std::cerr << "resolve unpositioned pass" << std::endl;
  resolveUnpositioned();
}

void FSMStatePositioner::bottomUpPass()
{
  // Process states bottom-up, keeping edges as short as possible.
  for (Reverse_cluster_iterator ci = lts->getReverseClusterIterator();
      !ci.is_end(); ++ci)
  {
    Cluster* cluster = *ci;
    if (cluster->getNumStates() == 1)
    {
      Vector2D position = Vector2D(0, 0);
      requestStatePosition(cluster->getState(0), position);
      continue;
    }
    for (int s = 0; s < cluster->getNumStates(); ++s)
    {
      State* state = cluster->getState(s);
      if (cluster->getNumDescendants() == 0)
      {
        state->center();
        unpositioned_states.push_back(state);
      }
      else if (cluster->getNumDescendants() == 1)
      {
        vector< State* > successors;
        getSuccessors(state, successors);
        if (allStatesCentered(successors))
        {
          state->center();
          unpositioned_states.push_back(state);
        }
        else
        {
          Vector2D position = sumStateVectors(successors,
              cluster->getBaseRadius());
          requestStatePosition(state, position);
        }
      }
      else
      {
        vector< State* > successors;
        getSuccessors(state, successors);
        Vector2D position = sumStateVectors(successors,
            cluster->getBaseRadius());
        requestStatePosition(state, position);
      }
    }
  }
}

void FSMStatePositioner::topDownPass()
{
  // Assumption: unpositioned_states is sorted bottom-up
  vector< State* > unpositioned_temp;
  for (vector< State* >::reverse_iterator state_it =
      unpositioned_states.rbegin(); state_it != unpositioned_states.rend();
      ++state_it)
  {
    State* state = *state_it;
    if (state->getCluster()->isCentered())
    {
      vector< State* > predecessors;
      getPredecessors(state, predecessors);
      if (allStatesCentered(predecessors))
      {
        unpositioned_temp.push_back(state);
      }
      else
      {
        Vector2D position = sumStateVectors(predecessors,
            state->getCluster()->getBaseRadius());
        requestStatePosition(state, position);
      }
    }
    else
    {
      unpositioned_temp.push_back(state);
    }
  }
  unpositioned_states.swap(unpositioned_temp);
}

void FSMStatePositioner::resolveUnpositioned()
{
  for (vector< State* >::iterator state_it = unpositioned_states.begin();
      state_it != unpositioned_states.end(); ++state_it)
  {
    State* state = *state_it;
    ClusterSlotInfo* cs_info = slot_info[state->getCluster()];
    Vector2D position(0, 0);
    position = cs_info->findFarthestFreeSlot(position);
    assignStateToPosition(state, position);
  }
}

Vector2D FSMStatePositioner::sumStateVectors(vector< State* > &states, float
    rim_radius)
{
  Vector2D cluster_vector, state_vector, sum_vector = Vector2D(0, 0);
  for (vector< State* >::iterator state_it = states.begin(); state_it !=
      states.end(); ++state_it)
  {
    State* state = *state_it;
    if (state->getCluster()->isCentered())
    {
      if (!state->isCentered())
      {
        state_vector.fromPolar(state->getPositionAngle(),
            state->getPositionRadius());
        sum_vector += state_vector;
      }
    }
    else
    {
      cluster_vector.fromPolar(state->getCluster()->getPosition(), rim_radius);
      sum_vector += cluster_vector;
      if (!state->isCentered())
      {
        state_vector.fromPolar(state->getPositionAngle() +
            state->getCluster()->getPosition(), state->getPositionRadius());
        sum_vector += state_vector;
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

void FSMStatePositioner::assignStateToPosition(State* state,
    const Vector2D& position)
{
  ClusterSlotInfo* cs_info = slot_info[state->getCluster()];
  cs_info->occupySlot(position);
  float angle, radius;
  position.toPolar(angle, radius);
  if (cs_info->isCenter(radius))
  {
    state->center();
  }
  else
  {
    state->setPositionAngle(angle);
    state->setPositionRadius(radius);
  }
}

void FSMStatePositioner::requestStatePosition(State* state, Vector2D& position)
{
  ClusterSlotInfo* cs_info = slot_info[state->getCluster()];
  std::cerr << "cluster " << state->getCluster()->getPositionInRank() <<
    " at rank " << state->getCluster()->getRank() << ": requested position ("
    << position.x() << ", " << position.y() << ") " << std::endl;
  position = cs_info->findNearestFreeSlot(position);
  std::cerr << "got (" << position.x() << ", " << position.y() << ") " <<
    std::endl;
  assignStateToPosition(state, position);
}
