// Author(s): Bas Ploeger and Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef STATE_H
#define STATE_H

#include <vector>
#include <set>
#include "vectors.h"

class Transition;
class Cluster;

class State
{
  public:
    State(int aid);
    ~State();
    void addInTransition(Transition* trans);
    void addLoop(Transition* trans);
    void addOutTransition(Transition* trans);
    void center();
    void deselect();

    Cluster* getCluster() const;
    int getID();
    Transition* getInTransition(int i) const;
    Transition* getOutTransition(int i) const;
    Transition* getLoop(int i) const;
    int getNumInTransitions() const;
    int getNumOutTransitions() const;
    int getNumLoops() const;

    float getPositionAngle() const;
    float getPositionRadius() const;
    Vector3D getPositionAbs() const;
    Vector3D getOutgoingControl() const;
    Vector3D getIncomingControl() const;
    Vector3D getLoopControl1() const;
    Vector3D getLoopControl2() const;
    int getRank() const;
    bool isSimulated() const;
    bool isCentered() const;
    bool isDeadlock() const;
    bool isSelected() const;
    bool hasTextures() const;

    void addMatchedRule(int mr);
    bool removeMatchedRule(int mr);
    void getMatchedRules(std::vector< int > &mrs);
    int getNumMatchedRules();

    void select();
    void setCluster(Cluster* c);
    void setPositionAngle(float a);
    void setPositionRadius(float r);
    void setPositionAbs(Vector3D p);
    void setOutgoingControl(Vector3D p);
    void setIncomingControl(Vector3D p);
    void setLoopControl1(Vector3D p);
    void setLoopControl2(Vector3D p);
    void setID(int id);
    void setRank(int r);
    void setSimulated(bool simulated);

    void setZoomLevel(const int i);
    int getZoomLevel() const;

  private:
    Cluster* cluster;
    int id;
    int zoomLevel;
    std::vector< Transition* > inTransitions;
    std::vector< Transition* > loops;
    std::vector< Transition* > outTransitions;
    std::set< int > matchedRules;
    float positionAngle;
    float positionRadius;
    Vector3D positionAbs;
    Vector3D outgoingControl;
    Vector3D incomingControl;
    Vector3D loopControl1;
    Vector3D loopControl2;
    int rank;
    bool simulated;
    bool selected;
};

#endif //STATE_H
