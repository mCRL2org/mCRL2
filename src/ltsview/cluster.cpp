#include "cluster.h"
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

void Cluster::getSlots( vector< Slot > &ss ) const 
{
  ss = slots;
}

void Cluster::occupySlot( int slot ) 
{
  slots[slot].free = false;
}
void Cluster::computeSizeAndDescendantPositions()
// pre: size of every descendant is known (and assumed to be correct)
{
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
