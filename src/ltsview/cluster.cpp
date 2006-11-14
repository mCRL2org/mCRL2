#include "cluster.h"
#include <iostream.h>
using namespace std;
using namespace Utils;

bool Comp_ClusterVolume::operator()(const Cluster* c1, const Cluster* c2) const
{
  return ( c1->getVolume() < c2->getVolume() );
}

Cluster::Cluster()
{
  ancestor = NULL;
  position = 0.0f;
  size = 0.0f;
  baseRadius = 0.0f;
  topRadius = 0.0f;
  deadlock = false;
  markedState = 0;
  markedTransitionCount = 0;

  Slot centerSlot = { -1.0f, 0 };
  slots.push_back(centerSlot);
}

Cluster::~Cluster()
{
  actionLabelCounts.clear();
  descendants.clear();
  states.clear();
}

void Cluster::addState( State* s )
{
  states.push_back( s );
}

void Cluster::addActionLabel( ATerm l )
{
  if ( actionLabelCounts.find( l ) == actionLabelCounts.end() )
    actionLabelCounts[l] = 1;
  else
    ++actionLabelCounts[l];
}

void Cluster::setAncestor( Cluster* c )
{
  ancestor = c;
}

void Cluster::setPosition( float p )
{
  position = p;
}

void Cluster::addDescendant( Cluster* c )
{
  descendants.push_back( c );
}

void Cluster::getStates( vector< State* > &ss ) const
{
  ss = states;
}

int Cluster::getNumberOfStates() const
{
  return states.size();
}

Cluster* Cluster::getAncestor() const
{
  return ancestor;
}

int Cluster::getNumberOfDescendants() const
{
  return descendants.size();
}

Cluster* Cluster::getDescendant( int i ) const
{
  return descendants[i];
}

void Cluster::getDescendants( vector< Cluster* > &cs ) const
{
  cs = descendants;
}

bool Cluster::hasDescendants() const
{
  return ( descendants.size() != 0 );
}

float Cluster::getSize() const
{
  return size;
}

float Cluster::getVolume() const
{
  return volume;
}

float Cluster::getPosition() const
{
  return position;
}

int Cluster::getNumberOfSlots()
{
  return slots.size();
}

Slot Cluster::getSlot(int index)  const
{
  return slots[index];
}

int Cluster::occupySlot( float pos ) 
{
  //Pre:  (pos ~= -1.0f \/ 0 <= pos < 360) /\ 3 <= slots.size()
  //Post: slots[slot].occupy++, where 
  //  slot = (min s: 1 <= s < slots.size(): abs(slots[s].position - pos) \/
  //  (pos ~= -1.0f /\ slot = 0)
  //Ret:  slot
  // Above Post-condition is currently not fullfilled, since we have not 
  // finished developing the slots system.
  
  int slot = -1;

  if (pos < -0.9f) {
    // Put the state in the center slot.
    slot = 0;
  }  
  else {
    float minDiff = 362.0f; // Difference can't become large than 360
    
    for( size_t i = 0; i < slots.size(); ++i )
    { 
      float posDif = slots[i].position - pos;

      if ( posDif < 0) {
        posDif = -posDif;
      }
      if (posDif < minDiff)
      {
        slot = i;
        minDiff = posDif;
      }
    }
  }
  
  ++slots[slot].occupying;

  return slot;
  
}
void Cluster::computeSizeAndDescendantPositions()
// pre: size of every descendant and its number of slots are known 
// (and assumed to be correct)
{
  /* We also use this function to calculate the
   * number of slots and their positions for each cluster.
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
  topRadius = sqrt( states.size() * 0.04 );

  if ( descendants.size() == 0 )
  { 
    baseRadius = topRadius;
    size = topRadius;
    volume = 4 * PI/3 * topRadius * topRadius * topRadius;
    //volume = 0.0f;
    // Assign as much slots as there are nodes, to provide the best  
    // possible spacing
    // The position of the nodes is by the index as follows:
    // 0: The center slot, this has been assigned already.
    // 1 -- getNumberOfStates + 1: The rim slots, space these evenly, starting 
    // from 0.0
    // This means we have one slots extra, for centered states.

    float step = (2 * PI)/getNumberOfStates();
    float currPos = 0.0f;
  
    for (int i = 1; i < getNumberOfStates() + 1; ++i) 
    {
      Slot slot = {currPos, i};
      slots.push_back(slot);
      currPos += step;
    }
  }
  else if ( descendants.size() == 1 )
  {
    Cluster* desc = *descendants.begin();
    baseRadius = desc->getTopRadius();
    size = max( topRadius, (**descendants.begin()).getSize() );
    volume  = PI / 3 * (baseRadius - topRadius) * (baseRadius - topRadius);
    volume += PI * baseRadius * topRadius;
    volume += desc->getVolume();
    desc->setPosition( -1.0f );

    // The number of slots is double that of the descendant's slots, capped to
    // 32.
    // Please note that the center position is a slot as well, hence we need to
    // substract one, before doubling the number of slots.
    int numSlots = (desc->getNumberOfSlots() - 1) * 2;
   
    // calculate the positions of the slots.
    // Index 0: Already determined to be the center position, do not touch.
    // Index 1 -- numSlots + 1: Rim positions, these need to at the following
    //   positions, if numSlots =/= 32:
    //     -descSlot((i '+' 1) / 2).pos, if i is odd.
    //     -(descSlot( i / 2).pos + descSlot(i /2 ''+'' 1)) / 2
    //       if i is even.
    //     Here, '+' is defined as:
    //       a '+' b = (a + b) mod numSlots
    //     And ''+'' is defined as:
    //       a ''+'' b = a + b, if a + b < desc.numSlots
    //       a ''+'' b = a + b - desc.numSlots - 1, otherwise.
    // If numSlots = 32, space the slots evenly.
    // descSlot(i) is the slot[i] of the desc.
   

    if (numSlots > 32) 
    {
      numSlots = 32;
      // Distribute slots evenly
      float stepSize = (2 * PI) / 32;
      float pos = 0.0f;
      
      for (int i = 1; i < numSlots + 1; ++i) {
        Slot slot = {pos, 0};
        slots.push_back(slot);
        pos += stepSize;
      }
    }
    else 
    {
      for (int i = 1; i < numSlots + 1; ++i) {
        float pos = 0.0f;
      
        if ( i % 2 == 0) {
          // i is even
          int j = (i / 2) + 1;
          if (j > (desc->getNumberOfSlots() - 1)) {
            j = j - (desc->getNumberOfSlots() - 1);
          }
          float descPos1 = desc->getSlot(i / 2).position;
          float descPos2 = desc->getSlot(j).position;

          pos = (descPos1 + descPos2) / 2;
        }
        else {
          // i is odd
          pos = desc->getSlot((i+1) / 2).position;
        }
        Slot slot = {pos, 0};
        slots.push_back(slot);
      }
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
      numSlots = (largest->getNumberOfSlots() - 1) * 2;
      if (numSlots > 32) {
        numSlots = 32;
        float stepSize = (2 * PI) / 32;
        float currPos = 0.0f;

        for (int i = 1; i < numSlots + 1; ++i) {
          Slot slot = {currPos ,0};
          slots.push_back(slot);
          currPos += stepSize;
        }
      }
      else {
        for (int i = 1; i < numSlots + 1; ++i) 
        {
          float position = 0.0f;
          
          if (i % 2 == 0) {
            // i is even
            int j = (i / 2) + 1;
            
            if (j > (largest->getNumberOfSlots() - 1)) 
            {
              j = j - (largest->getNumberOfSlots() - 1);
            }

            float descPos1 = largest->getSlot(i / 2).position;
            float descPos2 = largest->getSlot(j).position;

            position = (descPos1 + descPos2) / 2;
             
          }
          else 
          {
            // i is odd
            position = largest->getSlot((i+1)/2).position;
          }
          Slot slot = {position, 0};
          slots.push_back(slot);
        }
      }
    }
    else if (uniqueSmallest) {
      // There is a unique centered descendant, the smallest cluster.
      numSlots = (smallest->getNumberOfSlots() - 1) * 2;      
      
      if (numSlots > 32) {
        numSlots = 32;
        float stepSize = (2 * PI) / 32;
        float currPos = 0.0f;

        for (int i = 1; i < numSlots + 1; ++i) {
          Slot slot = {currPos ,0};
          slots.push_back(slot);
          currPos += stepSize;
        }
     }
      else {
        for (int i = 1; i < numSlots + 1; ++i) 
        {
          float position = 0.0f;
          
          if (i % 2 == 0) {
            // i is even
            int j = (i / 2) + 1;
            
            if (j > (smallest->getNumberOfSlots() - 1)) 
            {
              j = j - (smallest->getNumberOfSlots() - 1);
            }

            float descPos1 = smallest->getSlot(i / 2).position;
            float descPos2 = smallest->getSlot(j).position;

            position = (descPos1 + descPos2) / 2;
             
          }
          else 
          {
            // i is odd
            position = smallest->getSlot((i+1)/2).position;
          }
          Slot slot = {position, 0};
          slots.push_back(slot);
        }
      }
    }
    else {
      // There is no unique, centered descendant. All clusters are placed on
      // the rim.
      numSlots = getNumberOfDescendants() * 2;
      
      if (numSlots > 32) {
        numSlots = 32;
        float stepSize = (2 * PI) / 32;
        float currPos = 0.0f;

        for (int i = 1; i < numSlots + 1; ++i) {
          Slot slot = {currPos ,0};
          slots.push_back(slot);
          currPos += stepSize;
        }
      }

      else {
        for (int i = 1; i < numSlots + 1; ++i) 
        {
          float position = 0.0f;
          if (i % 2 == 0) {
            // i is even
            // 2 <= i, so 0 <= i - 2.
            int j = (i  - 2)/ 2;
            
            float descPos1 = getDescendant(j)->getPosition();
            float descPos2 = 
              getDescendant((j + 1) % getNumberOfDescendants())->getPosition();
            position = (descPos1 + descPos2) / 2;
             
          }
          else 
          {
            // i is odd
            position = getDescendant((i-1)/2)->getPosition();
          }
          Slot slot = {position, 0};
          slots.push_back(slot);
        }
      }
    }
  }
}

float Cluster::getTopRadius() const
{
  return topRadius;
}

float Cluster::getBaseRadius() const
{
  return baseRadius;
}

bool Cluster::hasMarkedState() const
{
  return ( markedState > 0 );
}

bool Cluster::hasMarkedTransition() const
{
  return ( markedTransitionCount > 0 );
}

void Cluster::markState()
{
  ++markedState;
}

void Cluster::unmarkState()
{
  --markedState;
}

int Cluster::markActionLabel( ATerm l )
{
  if ( actionLabelCounts.find( l ) != actionLabelCounts.end() )
    markedTransitionCount += actionLabelCounts[l];
  return markedTransitionCount;
}

int Cluster::unmarkActionLabel( ATerm l )
{
  if ( actionLabelCounts.find( l ) != actionLabelCounts.end() )
    markedTransitionCount -= actionLabelCounts[l];
  return markedTransitionCount;
}

bool Cluster::hasDeadlock() const
{
  return deadlock;
}

void Cluster::setDeadlock( bool b )
{
  deadlock = b;
}
