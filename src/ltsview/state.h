#ifndef STATE_H
#define STATE_H

#include <vector>
#include <set>
#include "aterm/aterm1.h"

#ifndef TRANSITION_H
  #include "transition.h"
#else
  class Transition;
#endif

#ifndef CLUSTER_H
  #include "cluster.h"
#else
  class Cluster;
#endif

class State
{
  public:
    State( ATermAppl sv );
    ~State();
    void      addComrade( State* s );
    void      addInTransition( Transition* trans );
    void      addLoop( Transition* trans );
    void      addOutTransition( Transition* trans );
    void      addSubordinate( State* s );
    void      addSuperior( State* s );
    void      clearHierarchyInfo();
    void      deselect();
    Cluster*  getCluster() const;
    void      getComrades( set< State* > &ss ) const;
    void      getInTransitions( vector< Transition* > &ts ) const;
    void      getOutTransitions( vector< Transition* > &ts ) const;
    int	      getRank() const;
    void      getSubordinates( set< State* > &ss ) const;
    void      getSuperiors( set< State* > &ss ) const;
    bool      isMarked() const;
    bool      isSelected() const;
    void      mark();
    void      select();
    void      setCluster( Cluster* c );
    void      setRank( int r );
    void      unmark();

  private:
    Cluster*		  cluster;
    set< State* >	  comrades;
    float		  currentProbability;
    vector< Transition* > inTransitions;
    vector< Transition* > loops;
    bool		  marked;
    float		  probability;
    vector< Transition* > outTransitions;
    int			  rank;
    bool		  selected;
    ATermAppl		  stateVector;
    set< State* >	  subordinates;
    set< State* >	  superiors;
};

#endif //STATE_H
