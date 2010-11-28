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
#include "state.h"
#include "transition.h"
#include "vectors.h"

using namespace std;
using namespace MathUtils;

class ClusterSlotInfo
{
  public:
    ClusterSlotInfo(Cluster* cluster);
    ~ClusterSlotInfo() {}
    void findFarthestFreeSlot(int &ring, int &slot);
    void findNearestSlot(Vector2D &position, int &ring, int &slot);
    void findNearestFreeSlot(int &ring, int &slot);

    int getNumRings()
    {
      return num_slots.size();
    }

    int getNumSlots(int ring)
    {
      return num_slots[ring];
    }

    void getPolarCoordinates(int ring, int slot, float &angle, float &radius)
    {
      radius = delta_ring * ring;
      angle = rad_to_deg(2 * PI * slot / num_slots[ring]);
    }

    void occupySlot(int ring, int slot)
    {
      occupied_slots.insert(Slot(ring, slot));
    }


  private:
    struct Slot
    {
      Slot(int r, int s): ring(r), slot(s) {}
      int ring, slot;
    };

    struct Slot_less
    {
      bool operator()(const Slot &c1, const Slot &c2)
      {
        return (c1.ring == c2.ring) ? c1.slot < c2.slot : c1.ring < c2.ring;
      }
    };

    typedef std::set< Slot, Slot_less > SlotSet;

    static const float MIN_DELTA_RING = 0.22f;
    static const float MIN_DELTA_SLOT = 0.22f;

    float delta_ring;
    std::vector< int > num_slots;
    SlotSet occupied_slots;

    Vector2D getVector(int ring, int slot);
};

ClusterSlotInfo::ClusterSlotInfo(Cluster* cluster)
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

  for (int ring = 0; ring < num_rings; ++ring)
  {
    float circumference = 2 * PI * ring * delta_ring;
    // max with 1 to ensure that ring 0 also has a slot
    int slots = max(1, static_cast<int>(circumference / MIN_DELTA_SLOT));
    num_slots.push_back(slots);
  }
}

Vector2D ClusterSlotInfo::getVector(int ring, int slot)
{
  Vector2D result;
  float angle, radius;
  getPolarCoordinates(ring, slot, angle, radius);
  result.fromPolar(angle, radius);
  return result;
}

void ClusterSlotInfo::findNearestSlot(Vector2D &position, int &ring, int
    &slot)
{
  float angle, radius;
  position.toPolar(angle, radius);
  angle = deg_to_rad(angle);
  ring = min(round_to_int(radius / delta_ring), getNumRings() - 1);
  slot = round_to_int(num_slots[ring] * angle / (2 * PI)) % getNumSlots(ring);
}

void ClusterSlotInfo::findNearestFreeSlot(int &ring, int &slot)
{
  SlotSet visited_slots;
  queue< Slot > to_visit_slots;
  to_visit_slots.push(Slot(ring, slot));
  while (! to_visit_slots.empty())
  {
    Slot slot_coord = to_visit_slots.front();
    ring = slot_coord.ring;
    slot = slot_coord.slot;
    if (occupied_slots.find(slot_coord) == occupied_slots.end())
    {
      return;
    }
    for (int s = slot - 1; s < slot + 3; s += 2)
    {
      int next_slot = s % getNumSlots(ring);
      if (s < 0)
      {
        next_slot += getNumSlots(ring);
      }
      if (visited_slots.find(Slot(ring, next_slot)) == visited_slots.end())
      {
        to_visit_slots.push(Slot(ring, next_slot));
      }
    }
    for (int next_ring = ring - 1; next_ring < ring + 3; next_ring += 2)
    {
      if (0 <= next_ring && next_ring < getNumRings())
      {
        int next_slot = 0;
        if (next_ring > 0)
        {
          next_slot = round_to_int(static_cast<float>(slot) /
              static_cast<float>(getNumSlots(ring)) * getNumSlots(next_ring)) %
            getNumSlots(ring);
        }
        if (visited_slots.find(Slot(next_ring, next_slot)) == visited_slots.end())
        {
          to_visit_slots.push(Slot(next_ring, next_slot));
        }
      }
    }
    visited_slots.insert(Slot(ring, slot));
    to_visit_slots.pop();
  }
}

void ClusterSlotInfo::findFarthestFreeSlot(int &ring, int &slot)
{
  if (occupied_slots.empty())
  {
    ring = getNumRings() - 1;
    slot = 0;
    return;
  }
  float max_min_distance = 0.0f;
  for (int r = 0; r < getNumRings(); ++r)
  {
    for (int s = 0; s < getNumSlots(r); ++s)
    {
      Vector2D slot_vector = getVector(r, s);
      float min_distance = numeric_limits< float >::max();
      for (SlotSet::iterator occupied_slot = occupied_slots.begin();
          occupied_slot != occupied_slots.end(); ++occupied_slot)
      {
        Vector2D delta_vector = slot_vector - getVector(occupied_slot->ring,
            occupied_slot->slot);
        min_distance = min(min_distance, delta_vector.length());
      }
      if (min_distance > max_min_distance)
      {
        max_min_distance = min_distance;
        ring = r;
        slot = s;
      }
    }
  }
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
    std::cerr << cluster->getNumStates() << " states in this cluster" << std::endl;
    std::cerr << "  " << cluster->getRank() << " is cluster rank" << std::endl;
    std::cerr << "  " << cluster->getNumDescendants() << " is number of descendants" << std::endl;
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
    int ring, slot;
    cs_info->findFarthestFreeSlot(ring, slot);
    assignStateToSlot(state, ring, slot);
  }
}

Vector2D FSMStatePositioner::sumStateVectors(vector< State* > &states, float
    rim_radius)
{
  clock_t t_begin = clock();
  Vector2D cluster_vector, state_vector, sum_vector = Vector2D(0, 0);
  for (vector< State* >::iterator state_it = states.begin(); state_it !=
      states.end(); ++state_it)
  {
    State* state = *state_it;
    if (state->getCluster()->isCentered())
    {
      if ( ! state->isCentered() )
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
      if ( ! state->isCentered() )
      {
        state_vector.fromPolar(state->getPositionAngle() +
            state->getCluster()->getPosition(), state->getPositionRadius());
        sum_vector += state_vector;
      }
    }
  }
  float duration = static_cast<float>(clock() - t_begin) /
    static_cast<float>(CLOCKS_PER_SEC);
  std::cerr << "sumStateVectors called on " << states.size()
    << " states and took " << duration << " seconds" << std::endl;
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
  clock_t t_begin = clock();
  for (int t = 0; t < state->getNumOutTransitions(); ++t)
  {
    State* successor = state->getOutTransition(t)->getEndState();
    if (successor->getRank() == state->getRank() + 1)
    {
      successors.push_back(successor);
    }
  }
  float duration = static_cast<float>(clock() - t_begin) /
    static_cast<float>(CLOCKS_PER_SEC);
  std::cerr << "getSuccessors has considered " << state->getNumOutTransitions()
    << " outgoing transitions and took " << duration << " seconds" << std::endl;
}

void FSMStatePositioner::assignStateToSlot(State* state, int ring, int slot)
{
  ClusterSlotInfo* cs_info = slot_info[state->getCluster()];
  cs_info->occupySlot(ring, slot);
  if (ring == 0)
  {
    state->center();
  }
  else
  {
    float angle, radius;
    cs_info->getPolarCoordinates(ring, slot, angle, radius);
    state->setPositionAngle(angle);
    state->setPositionRadius(radius);
  }
}

void FSMStatePositioner::requestStatePosition(State* state, Vector2D& position)
{
  clock_t t_begin = clock();
  ClusterSlotInfo* cs_info = slot_info[state->getCluster()];
  int ring, slot;
  cs_info->findNearestSlot(position, ring, slot);
  cs_info->findNearestFreeSlot(ring, slot);
  assignStateToSlot(state, ring, slot);
  float duration = static_cast<float>(clock() - t_begin) /
    static_cast<float>(CLOCKS_PER_SEC);
  std::cerr << "requestStatePosition took " << duration << " seconds" <<
    std::endl;
}
