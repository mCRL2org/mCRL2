// Author(s): Bas Ploeger and Carst Tankink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file cluster.cpp
/// \brief Add your file description here.

#include "cluster.h"
#include <algorithm>
#include <math.h>
#include "utils.h"
using namespace std;
using namespace Utils;

bool Comp_ClusterVolume::operator()(const Cluster* c1,const Cluster* c2) const {
  return (c1->getVolume() < c2->getVolume());
}

Cluster::Cluster(int r) {
  ancestor = NULL;
  position = 0.0f;
  size = 0.0f;
  baseRadius = 0.0f;
  topRadius = 0.0f;
  deadlock = false;
  markedState = 0;
  visObject = -1;
  markedTransitionCount = 0;
  rank = r;
  selected = false;
  severedDescendantsC = 0;

  for(size_t i = 0; i < descendants.size(); ++i)
  {
    std::vector<bool> dummy;
    severedDescendants.push_back(dummy);
  }
}

Cluster::~Cluster() {
  actionLabelCounts.clear();
  descendants.clear();
  severedDescendants.clear();
  states.clear();
  slots.clear();
}

void Cluster::addState(State* s) {
  states.push_back(s);
}

void Cluster::addActionLabel(int l) {
  if (actionLabelCounts.find(l) == actionLabelCounts.end()) {
    actionLabelCounts[l] = 1;
  } else {
    ++actionLabelCounts[l];
  }
}

void Cluster::setAncestor(Cluster* c) {
  ancestor = c;
}

void Cluster::setPosition(float p) {
  position = p;
}

void Cluster::addDescendant(Cluster* c) {
  descendants.push_back(c);
  std::vector<bool> dummy;
  severedDescendants.push_back(dummy);
}

State* Cluster::getState(int i) const {
  return states[i];
}

int Cluster::getNumStates() const {
  return states.size();
}

Cluster* Cluster::getAncestor() const {
  return ancestor;
}

int Cluster::getNumDescendants() const {
  return descendants.size();
}

Cluster* Cluster::getDescendant(int i) const {
  if (severedDescendants[i].size() > 0)
  {
    return NULL;
  }
  else {
    return descendants[i];
  }
}

bool Cluster::hasDescendants() const {
  return descendants.size() != 0;
}


void Cluster::severDescendant( int i ) 
{
  severedDescendants[i].push_back(true);
  ++severedDescendantsC;
}


void Cluster::healSeverance(int i)
{
  if (severedDescendants[i].size() > 0) 
  {
    severedDescendants[i].pop_back();
    --severedDescendantsC;
  }
}

bool Cluster::hasSeveredDescendants()
{
  return severedDescendantsC > 0;
}


float Cluster::getSize() const {
  return size;
}

float Cluster::getVolume() const {
  return volume;
}

float Cluster::getPosition() const {
  return position;
}

void Cluster::center() {
  position = -1.0f;
}

bool Cluster::isCentered() const {
  return position < -0.9f;
}

int Cluster::getRank() const {
  return rank;
}

int Cluster::getPositionInRank() const 
{
  return positionInRank;
}

void Cluster::setPositionInRank(int p)
{
  positionInRank = p;
}

unsigned int Cluster::getNumSlots(unsigned int ring) const {
  return slots[ring].size();
}

unsigned int Cluster::getTotalNumSlots() const {
  unsigned int result = 0;
  for (unsigned int r = 0; r < NUM_RINGS; ++r) {
    result += getNumSlots(r);
  }
  return result;
}

void Cluster::occupySlot(unsigned int ring,float pos,State* s) {
  unsigned int slot = round_to_int(double(pos) * double(getNumSlots(ring)) /
      360.0);
  if (slot == getNumSlots(ring)) {
    slot = 0;
  }
  slots[ring][slot].push_back(s);
}

void Cluster::occupyCenterSlot(State* s) {
  slots[0][0].push_back(s);
}

void Cluster::addUndecidedState(State *s) {
  undecidedStates.push_back(s);
}

void Cluster::resolveSlots() {
  unsigned int from = 0;
  unsigned int to = 0;
  for (unsigned int r = 0; r < NUM_RINGS; ++r) {
    to = from + round_to_int(undecidedStates.size() * float(r) /
        float(NUM_RINGS));
    if (to > undecidedStates.size() || r == NUM_RINGS-1) {
      to = undecidedStates.size();
    }
    slotUndecided(r,from,to);
    spreadSlots(r);
    from = to;
  }
  undecidedStates.clear();
}

void Cluster::slotUndecided(unsigned int ring,unsigned int from,unsigned int to) {
  unsigned int remainingStates = to - from;
  const int numSlots = getNumSlots(ring);
  const float radius = getTopRadius() * float(ring) / float(NUM_RINGS-1);
  int start = 0;
  while (start < numSlots && slots[ring][start].size() == 0) {
    ++start;
  }
  float sp,diff;
  unsigned int i;
  if (start == numSlots) {
    // divide all undecided states over the complete ring
    sp = 0.0f;
    diff = 360.0f / remainingStates;
    for (i = from; i < to; ++i) {
      undecidedStates[i]->setPositionRadius(radius);
      undecidedStates[i]->setPositionAngle(sp);
      occupySlot(ring,sp,undecidedStates[i]);
      sp += diff; 
    }
  }
  else {
    int gapBegin,gapEnd,gapSize,space,lGapBegin,lGapEnd,lGapSize;
    unsigned int M,s;
    float slotDiff = 360.0f / numSlots;
    s = from;
    while (remainingStates > 0) {
      // compute the total space and largest gap size
      space = 0;
      lGapBegin = start;
      lGapEnd = start;
      lGapSize = 0;
      gapBegin = start;
      gapEnd = (gapBegin+1) % numSlots;
      while (gapEnd != start) {
        gapSize = 1;
        gapEnd = (gapBegin+1) % numSlots;
        while (slots[ring][gapEnd].size() == 0) {
          ++gapSize;
          gapEnd = (gapEnd+1) % numSlots;
        }
        if (gapSize > 1) {
          space += gapSize;
          if (gapSize > lGapSize) {
            lGapBegin = gapBegin;
            lGapEnd = gapEnd;
            lGapSize = gapSize;
          }
        }
        gapBegin = gapEnd;
      }

      M = int(1.0f + float(remainingStates) * float(lGapSize) / float(space));
      M = min(M,remainingStates);
      // divide M states over the largest gap found
      sp = lGapBegin * slotDiff;
      diff = lGapSize * slotDiff / M;
      for (i = s; i < s+M; ++i) {
        undecidedStates[i]->setPositionRadius(radius);
        undecidedStates[i]->setPositionAngle(sp);
        occupySlot(ring,sp,undecidedStates[i]);
        sp += diff; 
        if (sp >= 360.0f) {
          sp -= 360.0f;
        }
      }
      s += M;
      remainingStates -= M;
    }
  }
}

void Cluster::spreadSlots(unsigned int ring) {
  if (ring == 0) {
    // TODO: spread the states in the center of the cluster
    return;
  }
  const unsigned int numSlots = getNumSlots(ring);
  const float radius = getTopRadius() * float(ring) / float(NUM_RINGS-1);
  const float slot_diff = 360.0f / numSlots;

  vector< float > slot_space_cw(numSlots,0.0f);
  vector< float > slot_space_ccw(numSlots,0.0f);
  unsigned int s,i;
  // compute the amount of space available for each slot
  for (s = 0; s < numSlots; ++s) {
    if (slots[ring][s].size() > 1) {
      // space in counter-clockwise direction
      i = 1;
      while (slots[ring][(s+i)%numSlots].size() == 0) {
        ++i;
      }
      slot_space_ccw[s] = i * slot_diff;
      if (slots[ring][(s+i)%numSlots].size() > 1) {
        slot_space_ccw[s] *= 0.5f;
      }
      slot_space_ccw[s] -= rad_to_deg(0.25f/getTopRadius());

      // space in clockwise direction
      i = 1;
      while (slots[ring][(s-i)%numSlots].size() == 0) {
        ++i;
      }
      slot_space_cw[s] = i * slot_diff;
      if (slots[ring][(s-i)%numSlots].size() == 1) {
        slot_space_cw[s] -= rad_to_deg(0.25f/getTopRadius());
      } else {
        slot_space_cw[s] *= 0.5f;
      }
    }
  }

  float r,a,slot_angle;
  unsigned int numStates;
  for (s = 0; s < numSlots; ++s) {
    slot_angle = s*slot_diff;
    numStates = slots[ring][s].size();
    if (numStates == 1) {
      slots[ring][s][0]->setPositionAngle(slot_angle);
      slots[ring][s][0]->setPositionRadius(radius);
    }
    if (numStates > 1) {
      i = 0;
      r = radius;
      while (r > 0.15f && i < numStates) {
        a = slot_angle - slot_space_cw[s];
        while ((a < slot_angle + slot_space_ccw[s]) && i < numStates) {
          slots[ring][s][i]->setPositionAngle(a);
          slots[ring][s][i]->setPositionRadius(r);
          ++i;
          a += rad_to_deg(0.25f/r);
        }
        r -= 0.25f;
      }
      // if i < numStates, then there was not enough room for positioning all
      // states in this slot, so put all of the remaining states in the slot
      // position (this is a panic situation, that should occur very rarely)
      while (i < numStates) {
        slots[ring][s][i]->setPositionAngle(slot_angle);
        slots[ring][s][i]->setPositionRadius(radius);
        ++i;
      }
    }
  }
}

void Cluster::clearSlots() {
  unsigned int i,j;
  for (i = 0; i < slots.size(); ++i) {
    for (j = 0; j < slots[i].size(); ++j) {
      slots[i][j].clear();
    }
  }
  undecidedStates.clear();
}

void Cluster::computeSizeAndDescendantPositions() {
// pre: size of every descendant and its number of slots are known 
// (and assumed to be correct)
  
  /* We also use this function to calculate the number of slots.
   * This process is described in Frank van Ham's Master's thesis, p. 24
   */
   
  /* Compute the cluster radius r such that all states fit nicely into the
   * cluster. Suppose the cluster has N states and every state is visualized by
   * a circle with radius 0.1. Suppose we want the total area of all states
   * to fill a quarter of the circle that represents the cluster. This way,
   * there is (hopefully) enough room left for transitions and space between the
   * states.
   * So: N * pi * 0.1^2 = 0.25 * pi * r^2
   * Hence: r = sqrt( N * 0.04 )
   */
  topRadius = sqrt(states.size()*0.04);
  int numSlots;

  if (descendants.size() == 0) { 
    baseRadius = topRadius;
    size = topRadius;
    volume = 4 * PI/3 * topRadius * topRadius * topRadius;
    //volume = 0.0f;
    
    // Assign as much slots as there are nodes, to provide the best  
    // possible spacing
    // The position of the nodes is by the index as follows:
    // 0 -- getNumStates + 1: The rim slots.
    numSlots = getNumStates();
  }
  else if (descendants.size() == 1) {
    Cluster* desc = *descendants.begin();
    baseRadius = desc->getTopRadius();
    size = max(topRadius,(**descendants.begin()).getSize());
    volume  = PI / 3 * (baseRadius - topRadius) * (baseRadius - topRadius);
    volume += PI * baseRadius * topRadius;
    volume += desc->getVolume();
    desc->center();

    // The number of slots is double that of the descendant's slots, capped to
    // 32.
    numSlots = desc->getTotalNumSlots() * 2;
    numSlots = min(32,numSlots);
  }
  else { // descendants.size() > 1
    // sort descendants by size in ascending order
    sort( descendants.begin(), descendants.end(), Comp_ClusterVolume() );

    // determine whether a unique smallest descendant exists
    Cluster* smallest = descendants[0];
    Cluster* nextSmallest = descendants[1];
    
    bool uniqueSmallest = ( ( nextSmallest->getVolume() - smallest->getVolume()
    ) / smallest->getVolume()) > 0.01f;
    
    // determine whether a unique largest descendant exists
    Cluster* largest = descendants[ descendants.size()-1 ];
    Cluster* nextLargest = descendants[ descendants.size()-2 ];

    bool uniqueLargest = ( ( nextLargest->getVolume() - largest->getVolume() ) /
        largest->getVolume() ) < -0.01f;

    // invariant: descendants in range [noPosBegin, noPosEnd) have not been
    // assigned a position yet
    int noPosBegin = 0;
    int noPosEnd = descendants.size();
    
    float centerSize = 0.0f;  // size of largest descendant in center
    float rimSize = 0.0f;  // size of largest descendant on rim
    if (uniqueLargest) {
      // center the largest descendant
      largest->center();
      --noPosEnd;

      centerSize = largest->getSize();
      rimSize = nextLargest->getSize();
    }
    else {
      // no unique largest descendant, so largest will be placed on the rim
      rimSize = largest->getSize();
      
      if (uniqueSmallest) {
        // center the smallest descendant
        smallest->center();
        ++noPosBegin;
        centerSize = smallest->getSize();
      }
    }
    
    // compute the radius of the base of the cylinder and the cluster's size
    float minRimRadius = (float)( rimSize / sin(PI / (noPosEnd - noPosBegin)) );
    baseRadius = max( centerSize + rimSize + 0.01f, minRimRadius );
    size = max( topRadius, baseRadius );

    // compute the cluster's volume, which is the volume of the cylinder...
    volume  = PI / 3 * (baseRadius - topRadius) * (baseRadius - topRadius);
    volume += PI * baseRadius * topRadius;
    // ...plus the sum of the volumes of all the descendants
    for (vector< Cluster* >::iterator descit = descendants.begin();
        descit != descendants.end(); ++descit) {
      volume += (**descit).getVolume();
    }

    // Divide the remaining descendants over the rim of the circle. First take
    // the two largest unpositioned clusters and place them opposite to each
    // other, then do the same for the two smallest unpositioned clusters. Keep
    // repeating these steps until all clusters have been positioned. So if the
    // list of clusters (sorted ascending by size) is [ 0, 1, 2, 3, 4, 5 ] then
    // the clusters are placed in the following order (starting at angle 0 and
    // going counter-clockwise): 5, 0, 3, 4, 1, 2

    bool begin = false;
    int i = 0;
    int j = ( noPosEnd - noPosBegin ) / 2 + ( noPosEnd - noPosBegin ) % 2;
    float angle = 360.0 / ( noPosEnd - noPosBegin );
    while (noPosBegin != noPosEnd) {
      if (begin) {
        if (noPosBegin+1 != noPosEnd) {
          descendants[noPosBegin]->setPosition( (i++)*angle );
          volume += descendants[noPosBegin]->getVolume();
          noPosBegin++;
          descendants[noPosBegin]->setPosition( (j++)*angle );
          volume += descendants[noPosBegin]->getVolume();
          noPosBegin++;
        }
        else {
          descendants[noPosBegin]->setPosition( i*angle );
          volume += descendants[noPosBegin]->getVolume();
          noPosBegin++;
        }
      }
      else {
        if(noPosEnd-1 != noPosBegin) {
          noPosEnd--;
          descendants[noPosEnd]->setPosition( (i++)*angle );
          volume += descendants[noPosEnd]->getVolume();
          noPosEnd--;
          descendants[noPosEnd]->setPosition( (j++)*angle );
          volume += descendants[noPosEnd]->getVolume();
        }
        else {
          noPosEnd--;
          descendants[noPosEnd]->setPosition( (i++)*angle );
          volume += descendants[noPosEnd]->getVolume();
        }
      }
      begin = !begin;
    }

    // Now all the positions of the descendants have been determined, we can
    // determine the number of slots
    if (uniqueLargest) {
      // There is a centered descendant, the largest cluster.
      numSlots = largest->getTotalNumSlots() * 2;
    }
    else if (uniqueSmallest) {
      // There is a centered descendant, the smallest cluster.
      numSlots = smallest->getTotalNumSlots() * 2;      
    }
    else {
      // There is no centered descendant. All clusters are placed on the rim.
      numSlots = getNumDescendants() * 2;
    }
    numSlots = min(32,numSlots);
  }
  // create the slots
  int S;
  for (unsigned int r = 0; r < NUM_RINGS; ++r) {
    if (r == 0) {
      S = 1;
    } else {
      S = round_to_int(numSlots * float(r) / float(NUM_RINGS));
    }
    vector< vector< State* > > slots_r;
    for (int s = 0; s < S; ++s) {
      vector< State* > slots_rs;
      slots_r.push_back(slots_rs);
    }
    slots.push_back(slots_r);
  }
}

float Cluster::getTopRadius() const {
  return topRadius;
}

float Cluster::getBaseRadius() const {
  return baseRadius;
}

bool Cluster::hasMarkedState() const {
  return (markedState > 0);
}

bool Cluster::hasMarkedTransition() const {
  return (markedTransitionCount > 0);
}

void Cluster::markState() {
  ++markedState;
}

void Cluster::unmarkState() {
  --markedState;
}

int Cluster::markActionLabel(int l) {
  if (actionLabelCounts.find(l) != actionLabelCounts.end()) {
    markedTransitionCount += actionLabelCounts[l];
  }
  return markedTransitionCount;
}

int Cluster::unmarkActionLabel(int l) {
  if (actionLabelCounts.find(l) != actionLabelCounts.end()) {
    markedTransitionCount -= actionLabelCounts[l];
  }
  return markedTransitionCount;
}

bool Cluster::hasDeadlock() const {
  return deadlock;
}

void Cluster::setDeadlock(bool b) {
  deadlock = b;
}

int Cluster::getVisObject() const {
  return visObject;
}

void Cluster::setVisObject(int vo) {
  visObject = vo;
}

int Cluster::getBranchVisObject(int i) const {
  return branchVisObjects[i];
}

int Cluster::getNumBranchVisObjects() const {
  return branchVisObjects.size();
}

void Cluster::addBranchVisObject(int vo) {
  branchVisObjects.push_back(vo);
}

void Cluster::clearBranchVisObjects() {
  branchVisObjects.clear();
}

void Cluster::select()
{
  selected = true;
}

void Cluster::deselect()
{
  selected = false;
}

bool Cluster::isSelected() const
{
  return selected;
}


int Cluster::getMarkedTransitionCount() const
{
  return markedTransitionCount;
}
