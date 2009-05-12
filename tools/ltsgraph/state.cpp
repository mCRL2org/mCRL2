// Author(s): Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file state.cpp
/// \brief State class implementation.

#include "wx.hpp" // precompiled headers

#include "state.h"
#include <cmath>
#include <workarounds.h> // for M_PI

State::State(unsigned int _value, bool _isInitialState)
{
  value = _value;
  isInitial = _isInitialState;
  selected = false;
  dragged = false;
  locked = false;
  showStateVector = false;
  outCurve = 0.0;

  pos.x = 0;
  pos.y = 0;

  colour = *wxWHITE;
}

State::~State()
{
}

bool State::isInitialState() const
{
  return isInitial;
}

bool State::getShowStateVector() const
{
  return showStateVector;
}

void State::setShowStateVector(const bool value)
{
  showStateVector = value;
}


void State::addOutTransition(Transition* ot)
{
  State* to = ot->getTo();
  if (hasTransitionTo(to)) {
    outCurve += .025;
    ot->setControlAlpha(outCurve * M_PI);
  }
  // If the target of this transition has a returning transition, we curve it
  else if(to->hasTransitionTo(this)) {
    ot->setControlAlpha(.25 * M_PI);
  }


  outTransitions.push_back(ot);
}

void State::addInTransition(Transition* it)
{
  inTransitions.push_back(it);
}

void State::addSelfLoop(Transition* sl)
{
  selfLoops.push_back(sl);
}

void State::setParameters(std::map<std::string, std::string>& params) {
  parameterValues = params;
}

void State::setColour(const wxColour colour) {
  this->colour = colour;
}

wxColour State::getColour() const {
  return colour;
}

void State::setPosition(const Utils::Vect p)
{
  pos = p;
}

void State::setX(const double _x)
{
  double newX = _x;
  if (newX > 1000.0)
  {
    newX = 1000.0;
  }

  if (newX < -1000.0)
  {
    newX = -1000.0;
  }

  pos.x = newX;
}

void State::setY(const double _y)
{
  double newY = _y;

  if(newY > 1000.0)
  {
    newY = 1000.0;
  }

  if(newY < -1000.0)
  {
    newY = -1000.0;
  }

  pos.y = newY;
}

Utils::Vect State::getPosition() const
{
  return pos;
}

double State::getX() const
{
  return pos.x;
}

double State::getY() const
{
  return pos.y;
}


void State::setLabel(std::string const _label)
{
  label = _label;
}

std::string State::getLabel() const
{
  return label;
}

Transition* State::getTransition(size_t i) const
{
  return outTransitions[i];
}

Transition* State::getInTransition(size_t i) const
{
  return inTransitions[i];
}

Transition* State::getSelfLoop(size_t i) const
{
  return selfLoops[i];
}

size_t State::getNumberOfTransitions() const
{
  return outTransitions.size();
}

size_t State::getNumberOfInTransitions() const
{
  return inTransitions.size();
}

size_t State::getNumberOfSelfLoops() const
{
  return selfLoops.size();
}

size_t State::getValue() const
{
  return value;
}

void State::lock()
{
  locked = !locked;
}

void State::drag()
{
  dragged = true;
}

void State::stopDrag()
{
  dragged = false;
}


void State::select()
{
  selected = true;
}

void State::deselect()
{
  selected = false;
}

bool State::isSelected() const
{
  return selected;
}

bool State::isLocked() const
{
  return locked;
}

bool State::isDragged() const
{
  return dragged;
}

const std::map<std::string, std::string>& State::getParameters() const {
  return parameterValues;
}

bool State::hasTransitionTo(State* to)
{
  bool result = false;

  for(size_t i = 0; i < outTransitions.size() && !result; ++i) {
    result = outTransitions[i]->getTo() == to;
  }

  return result;
}
