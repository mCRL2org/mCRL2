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
    void      addInTransition(Transition* trans);
    void      addLoop(Transition* trans);
    void      addOutTransition(Transition* trans);
    void      addParameterValue(int valindex);
    void      center();
    void      deselect();

    Cluster*    getCluster() const;
    int         getID();
    Transition* getInTransition(int i) const;
    int         getNumInTransitions() const;
    Transition* getOutTransition(int i) const;
    int         getNumOutTransitions() const;
    Transition* getLoop(int i) const;
    int         getNumLoops() const;

    float          getPositionAngle() const;
    float          getPositionRadius() const;
    Utils::Point3D getPositionAbs() const;
    Utils::Point3D getOutgoingControl() const;
    Utils::Point3D getIncomingControl() const;
    int            getRank() const;
    bool           isSimulated() const;
    int  getParameterValue(int parindex);
    bool isCentered() const;
    bool isDeadlock() const;
    bool isMarked() const;
    unsigned int  nrRulesMatched() const;
    void setMarking(bool b);
    void setMarkAllEmpty(bool b);
    bool isSelected() const;

    // Adds mr to the rules matched by this state, and returns the total number
    // of rules matched.
    int mark(Utils::MarkRule* mr);
    void select();
    void setCluster(Cluster* c);
    void setPositionAngle(float a);
    void setPositionRadius(float r);
    void setPositionAbs(Utils::Point3D &p);
    void setOutgoingControl(Utils::Point3D &p);
    void setIncomingControl(Utils::Point3D &p);
    void setID(int id);
    void setRank(int r);
    void setSimulated(bool simulated);

    // Removes mr from the rules matched by this state (if any) and returns the 
    // total number of rules matched.
    int unmark(Utils::MarkRule* mr);
    void setZoomLevel(const int i);
    int  getZoomLevel() const;

    void addForce(Utils::Point3D f);
    Utils::Point3D getForce();
    void resetForce();
    Utils::Vect getVelocity();
    void resetVelocity();
    void setVelocity(Utils::Vect v);
  
    void mark();
    void unmark();
    bool isMarkedNoRule();
  private:
    Cluster*                   cluster;
    int                        id;
    int                        zoomLevel;
    std::vector< Transition* > inTransitions;
    std::vector< Transition* > loops;
    std::vector< Transition* > outTransitions;
    std::vector< Utils::MarkRule* >   rulesMatched;
    
    bool  marked;
    bool  markAllEmpty;

    float                      positionAngle;
    float                      positionRadius;
    Utils::Point3D             positionAbs;
    Utils::Point3D             outgoingControl;
    Utils::Point3D             incomingControl;
    int                        rank;
    bool                       simulated;
    std::vector< int >         stateVector;
    bool                       selected;
    Utils::Point3D             force;
    Utils::Vect                velocity;
};

#endif //STATE_H
