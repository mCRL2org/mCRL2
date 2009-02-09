// Author(s): Bas Ploeger and Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file state.h
/// \brief Header file for State class

#ifndef STATE_H
#define STATE_H

#include <vector>
#include <set>
#include "utils.h"

class Transition;
class Cluster;

class State {
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
    Utils::Point3D getPositionAbs() const;
    Utils::Point3D getOutgoingControl() const;
    Utils::Point3D getIncomingControl() const;
    Utils::Point3D getLoopControl1() const;
    Utils::Point3D getLoopControl2() const;
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
    //unsigned int getNumMatchedRules() const;
    //bool isMarked() const;
    //void setMarking(bool b);
    //void setMarkAllEmpty(bool b);
    // Adds mr to the rules matched by this state, and returns the total number
    // of rules matched.
    //int mark(Utils::MarkRule* mr);
    // Removes mr from the rules matched by this state (if any) and returns the
    // total number of rules matched.
    //int unmark(Utils::MarkRule* mr);

    void select();
    void setCluster(Cluster* c);
    void setPositionAngle(float a);
    void setPositionRadius(float r);
    void setPositionAbs(Utils::Point3D &p);
    void setOutgoingControl(Utils::Point3D &p);
    void setIncomingControl(Utils::Point3D &p);
    void setLoopControl1(Utils::Point3D &p);
    void setLoopControl2(Utils::Point3D &p);
    void setID(int id);
    void setRank(int r);
    void setSimulated(bool simulated);

    void setZoomLevel(const int i);
    int getZoomLevel() const;

    void addForce(Utils::Point3D f);
    Utils::Point3D getForce();
    void resetForce();
    Utils::Vect getVelocity();
    void resetVelocity();
    void setVelocity(Utils::Vect v);

  private:
    Cluster* cluster;
    int id;
    int zoomLevel;
    std::vector< Transition* > inTransitions;
    std::vector< Transition* > loops;
    std::vector< Transition* > outTransitions;
    std::set< int > matchedRules;
    /*
    std::vector< Utils::MarkRule* > rulesMatched;
    bool marked;
    bool markAllEmpty;*/

    float positionAngle;
    float positionRadius;
    Utils::Point3D positionAbs;
    Utils::Point3D outgoingControl;
    Utils::Point3D incomingControl;
    Utils::Point3D loopControl1;
    Utils::Point3D loopControl2;
    int rank;
    bool simulated;
    bool selected;
    Utils::Point3D force;
    Utils::Vect velocity;
};

#endif //STATE_H
