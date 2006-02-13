#include "cluster.h"

bool Comp_ClusterSize::operator()(const Cluster* c1, const Cluster* c2) const
{
  return ( c1->getSize() < c2->getSize() );
}

Cluster::Cluster()
{
  ancestor = NULL;
  position = 0;
  size = 0.0;
}

Cluster::~Cluster()
{
}

void Cluster::addState( State* s )
{
  states.push_back( s );
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

Cluster* Cluster::getAncestor() const
{
  return ancestor;
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

float Cluster::getPosition() const
{
  return position;
}

void Cluster::computeSizeAndDescendantPositions()
// pre: size of every descendant is known (and assumed to be correct)
{
  topRadius = states.size() / (2 * PI);
  if ( descendants.size() == 0 )
  {
    baseRadius = topRadius;
    size = topRadius;
  }
  else if ( descendants.size() == 1 )
  {
    baseRadius = (**descendants.begin()).getTopRadius();
    size = max( topRadius, (**descendants.begin()).getSize() );
    (**descendants.begin()).setPosition( -1.0 );
  }
  else // descendants.size() > 1
  {
    // sort descendants by size in ascending order
    sort( descendants.begin(), descendants.end(), Comp_ClusterSize() );

    // determine whether a unique smallest descendant exists
    Cluster* smallest = descendants[0];
    Cluster* nextSmallest = descendants[1];
    bool uniqueSmallest = ( nextSmallest->getSize() - smallest->getSize() > 0.1 );
    
    // determine whether a unique largest descendant exists
    Cluster* largest = descendants[ descendants.size()-1 ];
    Cluster* nextLargest = descendants[ descendants.size()-2 ];
    bool uniqueLargest = ( largest->getSize() - nextLargest->getSize() > 0.1 );

    // invariant: descendants in range [noPosBegin, noPosEnd) have not been
    // assigned a position yet
    int noPosBegin = 0;
    int noPosEnd = descendants.size();
    
    float centerSize = 0.0f;	// size of largest descendant in center
    float rimSize = 0.0f;	// size of largest descendant on rim
    if ( uniqueLargest )
    {
      // center the largest descendant
      largest->setPosition( -1.0 );
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
	centerSize = smallest->getSize();
	smallest->setPosition( -1.0 );
	++noPosBegin;
      }
    }
    
    // compute the size of the cluster
    float minRimRadius = (float)( rimSize / sin(PI / (noPosEnd - noPosBegin)) );
    baseRadius = max( centerSize + rimSize + 0.01f, minRimRadius );
    size = max( topRadius, baseRadius );

    // now correct the baseRadius for the case in which a unique largest
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
