// Author(s): Bas Ploeger and Carst Tankink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file cluster.h
/// \brief Add your file description here.

#ifndef CLUSTER_H
#define CLUSTER_H
#include <vector>
#include <map>
#include "utils.h"

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
    bool operator()(const Cluster*,const Cluster*) const;
};

class Cluster {
  private:
    std::map< int, int >	actionLabelCounts;
    Cluster*	ancestor;
    float			baseRadius;
    bool			deadlock;
    std::vector< Cluster* >	descendants;
    int				markedState;
    int				markedTransitionCount;
    float			position;
    int       rank;
    int       positionInRank;
    float			size;
    std::vector< State* >	states;
    std::vector< State* > undecidedStates;
    float			topRadius;
    float			volume;
    std::vector< Utils::Slot* > slots;

    int visObject;
    int visObjectTop;

    bool selected;

  public:
    // Constructor & destructor.
    Cluster(int r);
    ~Cluster();

    // Methods on descendants and ancestors.
    void      addDescendant(Cluster* c);
    Cluster*  getDescendant(int i) const;
    int	      getNumDescendants() const;
    bool      hasDescendants() const;

    void      setAncestor(Cluster* c);
    Cluster*  getAncestor() const;

    // Methods on states
    void         addState(State* s);
    void         addUndecidedState(State* s);
    State*       getState(int i) const;
    int          getNumStates() const;
    bool         hasDeadlock() const;
    bool         hasMarkedState() const;
    void         markState();
    void         setDeadlock( bool b );
    void         unmarkState();
    int          getNumSlots();
    Utils::Slot*  getSlot(int index) const;
    int          occupySlot(float pos);
    void         resolveSlots();
    void         slotUndecided();
    void         spreadSlots();

    // Methods on transitions
    void      addActionLabel(int l);
    bool      hasMarkedTransition() const;
    int	      markActionLabel(int l);
    int       unmarkActionLabel(int l);
    
    // General cluster information
    void      computeSizeAndDescendantPositions();
    float     getBaseRadius() const;
    float     getPosition() const;
    int       getRank() const;
    int       getPositionInRank() const;
    void      setPositionInRank(int p);
    float     getTopRadius() const;
    float     getSize() const;
    float     getVolume() const;
    void      setPosition(float p);

    // Methods for visualization
    int   getVisObject() const;
    int   getVisObjectTop() const;
    bool  hasVisObjectTop() const;
    void  setVisObject(int vo);
    void  setVisObjectTop(int vo);
    
    // Methods for selection
    void  select();
    void  deselect();
    bool  isSelected() const;
};

#endif
