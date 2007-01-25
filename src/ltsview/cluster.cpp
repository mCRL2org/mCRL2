#include "cluster.h"
#include <algorithm>
#include <math.h>
#include <iostream>
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
  markedTransitionCount = 0;
  rank = r;
  matrix = (float*)malloc(16*sizeof(float));
}

Cluster::~Cluster() {
  free(matrix);
  actionLabelCounts.clear();
  descendants.clear();
  states.clear();
}

void Cluster::addState(State* s) {
  states.push_back(s);
}

void Cluster::addActionLabel(ATerm l) {
  if (actionLabelCounts.find(l) == actionLabelCounts.end())
    actionLabelCounts[l] = 1;
  else
    ++actionLabelCounts[l];
}

void Cluster::setAncestor(Cluster* c) {
  ancestor = c;
}

void Cluster::setPosition(float p) {
  position = p;
}

void Cluster::addDescendant(Cluster* c) {
  descendants.push_back(c);
}

State* Cluster::getState(int i) const {
  return states[i];
}

int Cluster::getNumberOfStates() const {
  return states.size();
}

Cluster* Cluster::getAncestor() const {
  return ancestor;
}

int Cluster::getNumberOfDescendants() const {
  return descendants.size();
}

Cluster* Cluster::getDescendant(int i) const {
  return descendants[i];
}

bool Cluster::hasDescendants() const {
  return (descendants.size() != 0);
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

int Cluster::getRank() const {
  return rank;
}

int Cluster::getNumberOfSlots() {
  return slots.size();
}

Slot* Cluster::getSlot(int index) const {
  return slots[index];
}

int Cluster::occupySlot(float pos) {
  int slot = -1;
  float minDiff = 362.0f; // Difference can't become larger than 360
  unsigned int numberOfSlots = slots.size();

  for (size_t i = 0; i < numberOfSlots; ++i) { 
    // Slot with index i has position i * 360 / (slots.size()
    float slotPosition =  i  * 360 / numberOfSlots;
    float posDif = fabsf(slotPosition - pos);


    if (posDif < minDiff) {
        slot = i;
        minDiff = posDif;
    }
  }
  
  
  ++(slots[slot]->occupying);

  return slot;
}

void Cluster::addUndecidedState(State *s) {
  undecidedStates.push_back(s);
}

void Cluster::resolveSlots() {
  slotUndecided();
  spreadSlots();
}

void Cluster::slotUndecided() {
  // Place undecided nodes in slots so there is as much space as possible 
  // between nodes.
  int remainingNodes = undecidedStates.size();
  int totalSlots = slots.size();

  int gapStart = 0; // Start of checking for gaps 
  int gapBegin = 0; // Initial begin of gap
  int gapEnd   = 0; // Initial end of gap
  int gapSize  = 0; // Initial size of gap (gapEnd - gapBegin);

  int largestGapBegin = 0;  // initial begin of largest gap
  int largestGapEnd = 0;    // initial end of largest gap
  int largestGap = 0;       // initial size of largest gap
  
  while (remainingNodes != 0) {
    // Decide largest gap.
    
    // If the first slot is unoccupied, it is part of a gap, increase by one 
    // until we have found an occupied slot.
    bool gapIsRim = false;

    while (slots[gapBegin]->occupying == 0 && !gapIsRim) {
      gapBegin = (gapBegin + 1) % totalSlots;

      if (gapBegin == gapStart) {
        // Check to see if we have the entire rim to position the node.
        gapIsRim = true;
        largestGap = totalSlots;
      }
    }

    // We need to remember at which slot we started, to make sure we've had all
    // slots.
    gapStart = gapBegin;
    // A gap is at least of size one.
    gapEnd = gapBegin;

    if (!gapIsRim) {
      // Search for the largest gap: Keep gapBegin at the first occupied slot, 
      // and let gapEnd move from gapBegin to the first occupied slot after this
      // then, put gapBegin at the new gapEnd, and continue, until we have gone 
      // round.
      do {
        do  {
          gapEnd = (gapEnd + 1) % totalSlots;
        } while (slots[gapEnd]->occupying == 0);

        gapSize = gapEnd - gapBegin;
        // If gapSize is negative, this means that the end is before the begin.
        gapSize = (gapSize <= 0 ? (totalSlots - gapBegin) + gapEnd
                               : gapSize);

        if (gapSize > largestGap) {
          largestGap = gapSize;
          largestGapBegin = gapBegin;
          largestGapEnd = gapEnd;
        }

        gapBegin = gapEnd;
      } while (gapBegin != gapStart);      

    }
    
    // largestGap is the size of the gap between largestGapBegin and 
    // largestGapEnd.
    // We place (largestGap / totalSlots) * remainingNodes nodes into this gap,
    // rounding the position to the nearest slot.
    int toPlace = (largestGap * remainingNodes + totalSlots - 1) / totalSlots;
    float gapAngle = largestGap * 360.0 / totalSlots;
    float gapBeginAngle = gapBegin * 360.0 / totalSlots;
    toPlace = (toPlace > remainingNodes ? remainingNodes
                                        : toPlace);

    for(int i = 0; i < toPlace; ++i) {
      State* undecided = undecidedStates.back();
      float positionToPlace = gapBeginAngle + (i+1) * gapAngle / (toPlace + 1);
      int slotToPlace = occupySlot(positionToPlace);

      undecided->setPosition(positionToPlace);
      undecided->setSlot(slotToPlace);
      ++(slots[slotToPlace]->occupying);

      undecidedStates.pop_back();
    }

    remainingNodes = undecidedStates.size();
  }
}

void Cluster::spreadSlots() {
  int totalSlots = slots.size();
  
  // Distribute nodes over each slot.
  for(int i = 0; i < totalSlots; ++i) {
    // Calculate free slots neighbouring each slots.
    Slot* toSpread = slots[i];

    if (toSpread->occupying > 1) {
      int free_space = 0;
      int next_slot; 
      int previous_slot;
      
      do {
        ++free_space;
        next_slot = (i + free_space) % totalSlots;
        previous_slot = i - free_space;

        previous_slot = (previous_slot < 0 ? totalSlots + previous_slot
                                         : previous_slot);


      } while ((slots[previous_slot]->occupying == 0) &&
               (slots[next_slot]->occupying == 0) && 
               (previous_slot != i) && (next_slot != i));

      toSpread->total_size = free_space * 360 / totalSlots;

      toSpread->under_consideration = 1;
    }
  }

  // Iterate over the states, placing them according to the space available to 
  // the node.
  for(unsigned int i = 0; i < states.size(); ++i) {
    State* toPlaceState = states[i];

    if (toPlaceState->getPosition() >= -0.9f) {
      int slotOfStateIndex = toPlaceState->getSlot();
      Slot* slotOfState = slots[slotOfStateIndex];
      float statePosition = 0.0f;
    
      if (slotOfState->occupying == 1) {
        statePosition = 360 * slotOfStateIndex / totalSlots;
      }

      else {
        // slots[slotOfState].occupying > 1
        if (slotOfState->total_size > 359.0) {
          // This slot has all the space in the cluster, meaning we can place 
          // the states even more evenly than when it wouldn't.
          statePosition = fmodf(360 * slotOfStateIndex / totalSlots -
                                0.5f * slotOfState->total_size + 
                                slotOfState->under_consideration *
                                (slotOfState->total_size / 
                                 slotOfState->occupying),
                                360.0f);
        }
        else {
          statePosition =  fmodf( 360 * slotOfStateIndex / totalSlots - 
                                 0.5f * slotOfState->total_size + 
                                 slotOfState->under_consideration * 
                            (slotOfState->total_size 
                            / (slotOfState->occupying + 1)),
                          360.0f);
        }
        statePosition = (statePosition < 0 ? 360 + statePosition
                                           : statePosition);

        ++(slotOfState->under_consideration);

      }

      toPlaceState->setPosition(statePosition);
    }
  }
}

void Cluster::computeSizeAndDescendantPositions() {
// pre: size of every descendant and its number of slots are known 
// (and assumed to be correct)
  
  /* We also use this function to calculate the
   * number of slots.
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

  if (descendants.size() == 0) { 
    baseRadius = topRadius;
    size = topRadius;
    volume = 4 * PI/3 * topRadius * topRadius * topRadius;
    //volume = 0.0f;
    
    // Assign as much slots as there are nodes, to provide the best  
    // possible spacing
    // The position of the nodes is by the index as follows:
    // 0 -- getNumberOfStates + 1: The rim slots.
    for(int i = 0; i < getNumberOfStates(); ++i) {
      Slot* toAdd = new Slot;
      toAdd->occupying = 0;
      toAdd->under_consideration = 0;
      toAdd->total_size = 0;
      slots.push_back(toAdd);
    }
  }
  else if (descendants.size() == 1) {
    Cluster* desc = *descendants.begin();
    baseRadius = desc->getTopRadius();
    size = max(topRadius,(**descendants.begin()).getSize());
    volume  = PI / 3 * (baseRadius - topRadius) * (baseRadius - topRadius);
    volume += PI * baseRadius * topRadius;
    volume += desc->getVolume();
    desc->setPosition(-1.0f);

    // The number of slots is double that of the descendant's slots, capped to
    // 32.
    int numSlots = desc->getNumberOfSlots() * 2;

    numSlots = (numSlots > 32 ? 32 
                              : numSlots);

    for (int i = 0; i < numSlots; ++i) {
        Slot* slot = new Slot;
        slot->occupying = 0;
        slot->under_consideration = 0;
        slot->total_size = 0;
        slots.push_back(slot);
    }
  }
  else // descendants.size() > 1
  {
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
    
    float centerSize = 0.0f;	// size of largest descendant in center
    float rimSize = 0.0f;	// size of largest descendant on rim
    if ( uniqueLargest )
    {
      // center the largest descendant
      largest->setPosition( -1.0f );
      --noPosEnd;

      centerSize = largest->getSize();
      rimSize = nextLargest->getSize();
    }
    else
    {
      // no unique largest descendant, so largest will be placed on the rim
      rimSize = largest->getSize();
      
      if ( uniqueSmallest )
      {
	// center the smallest descendant
	smallest->setPosition( -1.0f );
	++noPosBegin;
	centerSize = smallest->getSize();
      }
    }
    
    // compute the radius of the base of the cylinder and the cluster's size
    float minRimRadius = (float)( rimSize / sin(PI / (noPosEnd - noPosBegin)) );
    baseRadius = max( centerSize + rimSize + 0.01f, minRimRadius );
    size = max( topRadius, baseRadius );

    /* now correct the baseRadius for the case in which a unique largest
    // descendant having descendants exists and all other descendants have no
    // descendants
    if ( uniqueLargest && largest->hasDescendants() )
    {
      // unique largest cluster exists (is centered) and has descendants
      int i = noPosBegin;
      while ( i < noPosEnd && ! descendants[i]->hasDescendants() )
	++i;
      if ( i == noPosEnd )
      {
	// none of the clusters on the rim has descendants
	baseRadius = max( largest->getTopRadius() + rimSize + 0.01f,
	    minRimRadius );
      }
    }
    */

    // compute the cluster's volume, which is the volume of the cylinder...
    volume  = PI / 3 * (baseRadius - topRadius) * (baseRadius - topRadius);
    volume += PI * baseRadius * topRadius;
    // ...plus the sum of the volumes of all the descendants
    for ( vector< Cluster* >::iterator descit = descendants.begin() ;
	descit != descendants.end() ; ++descit )
    {
      volume += (**descit).getVolume();
    }
    
    /*
    // divide the remaining descendants over the rim of the circle:
    // suppose the list noPosBegin,...,noPosEnd-1 is: [ 0, 1, 2, 3, 4, 5 ]
    // then the clusters are positioned on the rim in the following order
    // (counter-clockwise): ( 0, 2, 4, 1, 3, 5 )
    int noPosit;
    int i = 0;
    int j = ( noPosEnd - noPosBegin ) / 2 + ( noPosEnd - noPosBegin ) % 2;
    float angle = 360.0 / ( noPosEnd - noPosBegin );
    for ( noPosit = noPosBegin ; noPosit != noPosEnd ; ++noPosit )
    {
      if ( (noPosit - noPosBegin) % 2 == 0 )
      {
	descendants[noPosit]->setPosition( i*angle );
	++i;
      }
      else
      {
	descendants[noPosit]->setPosition( j*angle );
	++j;
      }
    }
    */

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
    while ( noPosBegin != noPosEnd )
    {
      if ( begin )
      {
	if ( noPosBegin+1 != noPosEnd )
	{
	  descendants[noPosBegin]->setPosition( (i++)*angle );
	  volume += descendants[noPosBegin]->getVolume();
	  noPosBegin++;
	  descendants[noPosBegin]->setPosition( (j++)*angle );
	  volume += descendants[noPosBegin]->getVolume();
	  noPosBegin++;
	}
	else
	{
	  descendants[noPosBegin]->setPosition( i*angle );
	  volume += descendants[noPosBegin]->getVolume();
	  noPosBegin++;
	}
      }
      else
      {
	if( noPosEnd-1 != noPosBegin )
	{
	  noPosEnd--;
	  descendants[noPosEnd]->setPosition( (i++)*angle );
	  volume += descendants[noPosEnd]->getVolume();
	  noPosEnd--;
	  descendants[noPosEnd]->setPosition( (j++)*angle );
	  volume += descendants[noPosEnd]->getVolume();
	}
	else
	{
	  noPosEnd--;
	  descendants[noPosEnd]->setPosition( (i++)*angle );
	  volume += descendants[noPosEnd]->getVolume();
	}
      }
      begin = !begin;
    }
    // Now all the positions of the states have been determined, we can 
    // determine the position of the slots
    int numSlots;
    if (uniqueLargest) {
      // There is a unique centered descendant, the largest cluster.
      numSlots = largest->getNumberOfSlots() * 2;
      numSlots = (numSlots > 32 ? 32 
                                : numSlots);

      for (int i = 0; i < numSlots; ++i) {
        Slot* slot = new Slot;
        slot->occupying = 0;
        slot->under_consideration = 0;
        slot->total_size = 0;
        slots.push_back(slot);
      }
    }
    else if (uniqueSmallest) {
      // There is a unique centered descendant, the smallest cluster.
      numSlots = smallest->getNumberOfSlots() * 2;      
      
      numSlots = (numSlots > 32 ? 32 
                                : numSlots);

      for (int i = 0; i < numSlots; ++i) {
        Slot* slot = new Slot;
        slot->occupying = 0;
        slot->under_consideration = 0;
        slot->total_size = 0;
        slots.push_back(slot);
      }
    }
    else {
      // There is no unique, centered descendant. All clusters are placed on
      // the rim.
      numSlots = getNumberOfDescendants() * 2;
      
      numSlots = (numSlots > 32 ? 32
                                : numSlots);

      for (int i = 0; i < numSlots; ++i) {
          Slot* slot = new Slot;
          slot->under_consideration = 0;
          slot->occupying = 0;
          slot->total_size = 0;
          slots.push_back(slot);
      }
    }
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

int Cluster::markActionLabel(ATerm l) {
  if (actionLabelCounts.find(l) != actionLabelCounts.end()) {
    markedTransitionCount += actionLabelCounts[l];
  }
  return markedTransitionCount;
}

int Cluster::unmarkActionLabel(ATerm l) {
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

int Cluster::getPrimitive() const {
  return primitive;
}

void Cluster::setPrimitive(int p) {
  primitive = p;
}

float* Cluster::getMatrix() const {
  return matrix;
}

Point3D Cluster::getCoordinates() const {
  Point3D result = { matrix[12],matrix[13],matrix[14] };
  return result;
}
