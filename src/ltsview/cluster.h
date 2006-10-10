#ifndef CLUSTER_H
#define CLUSTER_H
#include <vector>
#include <map>
#include <algorithm>
#include <math.h>
#include "utils.h"
#include "aterm1.h"

#ifndef STATE_H
  #include "state.h"
#else
  class State;
#endif

//using namespace Utils;
//using namespace std;

// forward declaration
class Cluster;

// class for cluster comparison based on cluster volumes
class Comp_ClusterVolume
{
  public:
    bool operator()( const Cluster*, const Cluster* ) const;
};

class Cluster
{
  
  private:

    std::map< ATerm, int >	actionLabelCounts;
    Cluster*			ancestor;
    float			baseRadius;
    bool			deadlock;
    std::vector< Cluster* >	descendants;
    int				markedState;
    int				markedTransitionCount;
    float			position;
    float			size;
    std::vector< State* >	states;
    float			topRadius;
    float			volume;

    std::vector< Utils::Slot > 	slots;

  public:
    // Constructor & destructor.
    Cluster();
    ~Cluster();

    // Functions on descendants and ancestors.
    void      addDescendant( Cluster* c );
    Cluster*  getDescendant( int i ) const;
    void      getDescendants( std::vector< Cluster* > &cs ) const;
    int	      getNumberOfDescendants() const;
    bool      hasDescendants() const;

    void      setAncestor( Cluster* c );
    Cluster*  getAncestor() const;

    // Functions on states
    void      addState( State* s );
    void      getStates( std::vector< State* > &ss ) const;
    bool      hasDeadlock() const;
    bool      hasMarkedState() const;
    void      markState();
    void      setDeadlock( bool b );
    void      unmarkState();
    int       getNumberOfSlots();
    void      getSlots( std::vector< Utils::Slot > &ss) const;
    void      occupySlot( int slot );

    // Functions on transitions
    void      addActionLabel( ATerm l );
    bool      hasMarkedTransition() const;
    int	      markActionLabel( ATerm l );
    int       unmarkActionLabel( ATerm l );
    
    // General cluster information
    void      computeSizeAndDescendantPositions();
    float     getBaseRadius() const;
    float     getPosition() const;
    float     getTopRadius() const;
    float     getSize() const;
    float     getVolume() const;
    void      setPosition( float p );
};

#endif
