#ifndef CLUSTER_H
#define CLUSTER_H
#include <vector>
#include <map>
#include <algorithm>
#include <math.h>
#include "utils.h"
#include "aterm/aterm1.h"

#ifndef STATE_H
  #include "state.h"
#else
  class State;
#endif

using namespace Utils;
using namespace std;

// forward declaration
class Cluster;

// class for cluster comparison based on cluster sizes
class Comp_ClusterSize
{
  public:
    bool operator()( const Cluster*, const Cluster* ) const;
};

class Cluster
{
  
  private:
    map< ATerm, int >	actionLabelCounts;
    Cluster*		ancestor;
    float		baseRadius;
    bool		deadlock;
    vector< Cluster* >	descendants;
    int			markedState;
    int			markedTransitionCount;
    float		position;
    float		size;
    vector< State* >	states;
    float		topRadius;
  
  public:
    Cluster();
    ~Cluster();
    void      addDescendant( Cluster* c );
    void      addState( State* s );
    void      addActionLabel( ATerm l );
    void      computeSizeAndDescendantPositions();
    Cluster*  getAncestor() const;
    float     getBaseRadius() const;
    void      getDescendants( vector< Cluster* > &cs ) const;
    float     getPosition() const;
    float     getTopRadius() const;
    float     getSize() const;
    void      getStates( vector< State* > &ss ) const;
    bool      hasDescendants() const;
    bool      hasDeadlock() const;
    bool      hasMarkedState() const;
    bool      hasMarkedTransition() const;
    void      markState();
    int	      markActionLabel( ATerm l );
    void      setAncestor( Cluster* c );
    void      setDeadlock( bool b );
    void      setPosition( float p );
    void      unmarkState();
    int       unmarkActionLabel( ATerm l );
};

#endif
