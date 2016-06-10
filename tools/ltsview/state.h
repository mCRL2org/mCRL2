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
#include "markmanager.h"
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
    bool isSimulated() const { return simulationCount > 0; }
    bool isCentered() const;
    bool isDeadlock() const;
    bool hasTextures() const;

    bool addMatchedRule(MarkRuleIndex index);
    bool removeMatchedRule(MarkRuleIndex index);
    const std::set<MarkRuleIndex> &getMatchedRules() const { return matchedRules; }
    void clearMatchedRules() { matchedRules.clear(); }

    void setCluster(Cluster* c);
    void setPositionAngle(float a);
    void setPositionRadius(float r);
    void setPositionAbs(const Vector3D& p);
    void setOutgoingControl(const Vector3D& p);
    void setIncomingControl(const Vector3D& p);
    void setLoopControl1(const Vector3D& p);
    void setLoopControl2(const Vector3D& p);
    void setID(int i);
    void setRank(int r);
    void increaseSimulation() { simulationCount++; }
    void decreaseSimulation() { simulationCount--; }

    void setZoomLevel(const int level);
    int getZoomLevel() const;

  private:
    Cluster* cluster;
    int id;
    int zoomLevel;
    std::vector< Transition* > inTransitions;
    std::vector< Transition* > loops;
    std::vector< Transition* > outTransitions;
    std::set< MarkRuleIndex > matchedRules;
    float positionAngle;
    float positionRadius;
    Vector3D positionAbs;
    Vector3D outgoingControl;
    Vector3D incomingControl;
    Vector3D loopControl1;
    Vector3D loopControl2;
    int rank;
    int simulationCount;
};

#endif //STATE_H
