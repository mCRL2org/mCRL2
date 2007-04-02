#ifndef STATE_H
#define STATE_H

#include <vector>
#include <set>
#include "utils.h"

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

class State {
  public:
    State();
    ~State();
    void      addComrade( State* s );
    void      addInTransition( Transition* trans );
    void      addLoop( Transition* trans );
    void      addOutTransition( Transition* trans );
		void			addParameterValue(int valindex);
    void      addSubordinate( State* s );
    void      addSuperior( State* s );
    void      clearHierarchyInfo();
    //void      deselect();
    void      DFSfinish();
    void      DFSclear();
    void      DFSvisit();
    
    Utils::DFSState  getVisitState() const;
    Cluster*  getCluster() const;
    void      getComrades( std::set< State* > &ss ) const;
		int				getID();
    void      getInTransitions( std::vector< Transition* > &ts ) const;
    void      getOutTransitions( std::vector< Transition* > &ts ) const;
    Transition* getOutTransitioni( int i) const;
    int       getNumberOfOutTransitions() const;
    float     getPosition() const;
    Utils::Point3D getPositionAbs() const;
    Utils::Point3D getOutgoingControl() const;
    Utils::Point3D getIncomingControl() const;
    int	      getRank() const;
    int       getSlot() const;
    void      getSubordinates( std::set< State* > &ss ) const;
    void      getSuperiors( std::set< State* > &ss ) const;
    int	      getParameterValue(int parindex);
    bool      isDeadlock() const;
    bool      isMarked() const;
    //bool      isSelected() const;
    void      mark();
    //void      select();
    void      setCluster( Cluster* c );
    void      setPosition( float p );
    void      setPositionAbs( Utils::Point3D p );
    void      setOutgoingControl( Utils::Point3D p);
    void      setIncomingControl( Utils::Point3D p);
		void			setID(int id);
    void      setRank( int r );
    void      setSlot( int s );
    void      unmark();
   
    // Methods for simulation
    // Getters
    bool      isSelected() const;
    // PRE: True
    // RET: selected
    
    Utils::SimState  getSimulationState() const;
    // PRE: True
    // RET: simulationState

    int       getVisitedAt() const;
    // PRE: True
    // RET: visitedAt

    void simVisit(int va); 
    // Visits this state in simulation
    // PRE:  0 <= va
    // POST: simulationState == NOW && visitedAt = va && 
    //   visitDesc().POST

    void simHistory(); 
    // Makes this state go into the past
    // PRE:  simulationState == NOW
    // POST: simulationState == HISTORY && 
    //   (forall d in subordinates:: d->simUnset())

    void simUnset(); 
    // Makes this state and all of its descendants invisible
    // PRE:  simulationState == FUTURE
    // POST: simulationState == UNSEEN &&
    //   (forall d in subordinates:: d->simUnset.POST)
    
    void setFuture(int va);
    // PRE:  True
    // POST: simulationState == FUTURE && visitedAt = va && 
    //       (forall d in subordinates:: d->setFuture.POST)

  private:
    Cluster*		  cluster;
    std::set< State* >	  comrades;
    //float		  currentProbability;
		int	id;
    std::vector< Transition* > inTransitions;
    std::vector< Transition* > loops;
    bool		  marked;
    //float		  probability;
    std::vector< Transition* > outTransitions;
    float		  position;
    Utils::Point3D	  positionAbs;
    Utils::Point3D        outgoingControl;
    Utils::Point3D        incomingControl;
    int			  rank;
    int                   slot; // The slot this state occupies
    std::vector< int >  stateVector;
    std::set< State* >	  subordinates;
    std::set< State* >	  superiors;
    Utils::DFSState       visitState;


    // Variables used for simulation
    bool		  selected;
    Utils::SimState       simulationState;
    int                   visitedAt; // Used for calculating transparency.
};

#endif //STATE_H
