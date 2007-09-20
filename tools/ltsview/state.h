// Author(s): Bas Ploeger and Carst Tankink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file state.h
/// \brief Add your file description here.

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
    void      addParameterValue(int valindex);
    void      addSubordinate( State* s );
    void      addSuperior( State* s );
    void      center();
    void      clearHierarchyInfo();
    void      deselect();
    void      DFSfinish();
    void      DFSclear();
    void      DFSvisit();
    Utils::DFSState  getVisitState() const;

    Cluster*  getCluster() const;
    void      getComrades( std::set< State* > &ss ) const;
    int       getID();
    void      getInTransitions( std::vector< Transition* > &ts ) const;
    void      getOutTransitions( std::vector< Transition* > &ts ) const;
    Transition* getOutTransitioni( int i) const;
    int       getNumOutTransitions() const;
    void      getLoops( std::vector< Transition* > &ls ) const;
    Transition* getLoopi( int i ) const;
    int       getNumberOfLoops() const;

    float     getPositionAngle() const;
    float     getPositionRadius() const;
    Utils::Point3D getPositionAbs() const;
    Utils::Point3D getOutgoingControl() const;
    Utils::Point3D getIncomingControl() const;
    int        getRank() const;
    bool      isSimulated() const;
    std::set<State*>::iterator getSubordinatesBegin() const;
    std::set<State*>::iterator getSubordinatesEnd() const;
    std::set<State*>::iterator getSuperiorsBegin() const;
    std::set<State*>::iterator getSuperiorsEnd() const;
    int       getParameterValue(int parindex);
    bool      isCentered() const;
    bool      isDeadlock() const;
    bool      isMarked() const;
    bool      isSelected() const;
    void      mark();
    void      select();
    void      setCluster( Cluster* c );
    void      setPosition(float r,float a);
    void      setPositionAbs( Utils::Point3D p );
    void      setOutgoingControl( Utils::Point3D p);
    void      setIncomingControl( Utils::Point3D p);
    void      setID(int id);
    void      setRank( int r );
    void      setSimulated(bool simulated);
    void      unmark();
    void      setZoomLevel(const int i);
    int       getZoomLevel() const;
   
  private:
    Cluster*      cluster;
    std::set< State* >    comrades;
    int                    id;
    int           zoomLevel;
    std::vector< Transition* > inTransitions;
    std::vector< Transition* > loops;
    bool      marked;
    std::vector< Transition* > outTransitions;
    float      positionAngle;
    float      positionRadius;
    Utils::Point3D    positionAbs;
    Utils::Point3D        outgoingControl;
    Utils::Point3D        incomingControl;
    int        rank;
    bool                  simulated;
    std::vector< int >    stateVector;
    std::set< State* >    subordinates;
    std::set< State* >    superiors;
    Utils::DFSState       visitState;
    bool                  selected;
};

#endif //STATE_H
