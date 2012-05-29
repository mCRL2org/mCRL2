// Author(s): Bas Ploeger and Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <cstddef>

#include "state.h"
#include "transition.h"

using namespace std;

State::State(int aid):
  cluster(NULL),
  id(aid),
  zoomLevel(0),
  positionAngle(-1.0f),
  positionRadius(0.0f),
  rank(0),
  simulated(false),
  selected(false)
{}

State::~State()
{
  unsigned int i;
  for (i = 0; i < outTransitions.size(); ++i)
  {
    delete outTransitions[i];
  }
  for (i = 0; i < loops.size(); ++i)
  {
    delete loops[i];
  }
}

void State::addInTransition(Transition* trans)
{
  inTransitions.push_back(trans);
}

void State::addOutTransition(Transition* trans)
{
  outTransitions.push_back(trans);
}

void State::addLoop(Transition* trans)
{
  loops.push_back(trans);
}

bool State::addMatchedRule(MarkRuleIndex index)
{
  matchedRules.insert(index);
  return matchedRules.size() == 1;
}

bool State::removeMatchedRule(MarkRuleIndex index)
{
  return matchedRules.erase(index) > 0;
}

bool State::isDeadlock() const
{
  return (outTransitions.size() + loops.size() == 0);
}

bool State::isSelected() const
{
  return selected;
}

void State::select()
{
  selected = true;
}

void State::deselect()
{
  selected = false;
}

int State::getID()
{
  return id;
}

void State::setID(int i)
{
  id = i;
}

int State::getRank() const
{
  return rank;
}

void State::setRank(int r)
{
  rank = r;
}

bool State::isCentered() const
{
  return (positionAngle < -0.9f);
}

void State::center()
{
  positionRadius = 0.0f;
  positionAngle = -1.0f;
}

float State::getPositionAngle() const
{
  return positionAngle;
}

float State::getPositionRadius() const
{
  return positionRadius;
}

Vector3D State::getPositionAbs() const
{
  return positionAbs;
}

Vector3D State::getOutgoingControl() const
{
  return outgoingControl;
}

Vector3D State::getIncomingControl() const
{
  return incomingControl;
}

Vector3D State::getLoopControl1() const
{
  return loopControl1;
}

Vector3D State::getLoopControl2() const
{
  return loopControl2;
}

void State::setPositionRadius(float r)
{
  positionRadius = r;
}

void State::setPositionAngle(float a)
{
  positionAngle = a;
}

void State::setPositionAbs(Vector3D p)
{
  positionAbs = p;
}

void State::setOutgoingControl(Vector3D p)
{
  outgoingControl = p;
}

void State::setIncomingControl(Vector3D p)
{
  incomingControl = p;
}

void State::setLoopControl1(Vector3D p)
{
  loopControl1 = p;
}

void State::setLoopControl2(Vector3D p)
{
  loopControl2 = p;
}

Cluster* State::getCluster() const
{
  return cluster;
}

void State::setCluster(Cluster* c)
{
  cluster = c;
}

Transition* State::getInTransition(int i) const
{
  return inTransitions[i];
}

int State::getNumInTransitions() const
{
  return static_cast<int>(inTransitions.size());
}

Transition* State::getOutTransition(int i) const
{
  return outTransitions[i];
}

int State::getNumOutTransitions() const
{
  return static_cast<int>(outTransitions.size());
}

Transition* State::getLoop(int i) const
{
  return loops[i];
}

int State::getNumLoops() const
{
  return static_cast<int>(loops.size());
}

void State::setSimulated(bool simulated)
{
  this->simulated = simulated;
}

bool State::isSimulated() const
{
  return simulated;
}

int State::getZoomLevel() const
{
  return zoomLevel;
}

void State::setZoomLevel(const int level)
{
  zoomLevel = level;
}
