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
    bool isSelected() const;
    void mark();
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
    void unmark();
    void setZoomLevel(const int i);
    int  getZoomLevel() const;

    void addForce(Utils::Point3D f);
    Utils::Point3D getForce();
    void resetForce();
    void addVelocity(Utils::Vect v);
    Utils::Vect getVelocity();
    void resetVelocity();

  private:
    Cluster*                   cluster;
    int                        id;
    int                        zoomLevel;
    std::vector< Transition* > inTransitions;
    std::vector< Transition* > loops;
    std::vector< Transition* > outTransitions;
    bool                       marked;
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
