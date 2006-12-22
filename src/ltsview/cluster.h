#ifndef CLUSTER_H
#define CLUSTER_H
#include <vector>
#include <map>
#include "utils.h"
#include "aterm1.h"

#ifndef STATE_H
  #include "state.h"
#else
  class State;
#endif

// forward declaration
class Cluster;

// class for cluster comparison based on cluster volumes
class Comp_ClusterVolume {
  public:
    bool operator()( const Cluster*, const Cluster* ) const;
};

class Cluster {
  private:
    std::map< ATerm, int >	actionLabelCounts;
    Cluster*	ancestor;
    float			baseRadius;
    bool			deadlock;
    std::vector< Cluster* >	descendants;
    int				markedState;
    int				markedTransitionCount;
    float			position;
    int       rank;
    float			size;
    std::vector< State* >	states;
    std::vector< State* > undecidedStates;
    float			topRadius;
    float			volume;
    std::vector< Utils::Slot > slots;

    float *matrix;
    int primitive;

  public:
    // Constructor & destructor.
    Cluster(int r);
    ~Cluster();

    // Functions on descendants and ancestors.
    void      addDescendant(Cluster* c);
    Cluster*  getDescendant(int i) const;
    int	      getNumberOfDescendants() const;
    bool      hasDescendants() const;

    void      setAncestor(Cluster* c);
    Cluster*  getAncestor() const;

    // Functions on states
    void         addState(State* s);
    void         addUndecidedState(State* s);
    State*       getState(int i) const;
    int          getNumberOfStates() const;
    bool         hasDeadlock() const;
    bool         hasMarkedState() const;
    void         markState();
    void         setDeadlock( bool b );
    void         unmarkState();
    int          getNumberOfSlots();
    Utils::Slot  getSlot(int index) const;
    int          occupySlot(float pos);
    void         resolveSlots();

    // Functions on transitions
    void      addActionLabel( ATerm l );
    bool      hasMarkedTransition() const;
    int	      markActionLabel( ATerm l );
    int       unmarkActionLabel( ATerm l );
    
    // General cluster information
    void      computeSizeAndDescendantPositions();
    float     getBaseRadius() const;
    float     getPosition() const;
    int       getRank() const;
    float     getTopRadius() const;
    float     getSize() const;
    float     getVolume() const;
    void      setPosition(float p);
    
    // Functions for visualization
    int    getPrimitive() const;
    void   setPrimitive(int p);
    float* getMatrix() const;
    Utils::Point3D getCoordinates() const;
};

#endif
