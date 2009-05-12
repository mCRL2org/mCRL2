// Author(s): Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file transition.cpp
/// \brief Transition class implementation.

#include "wx.hpp" // precompiled headers

#include "transition.h"
#include <cmath>

#include "workarounds.h" // for M_PI on Windows with MSVC

Transition::Transition(State* from, State* to, std::string lbl)
{
  fromState = from;
  toState = to;
  label = lbl;

  controlAlpha = 0;
  controlDist = 0.5;
  labelX = 0.0;
  labelY = 0.025;
  selected = false;
}


State* Transition::getFrom() const
{
  return fromState;
}

State* Transition::getTo() const
{
  return toState;
}

void Transition::getLabelPos(double& x, double& y) {
  // Get control point of transition
  getControl(x, y);

  // labelX and labelY are relative to the control point
  x = x + labelX;
  y = y + labelY;
}

void Transition::getControl(double& x, double& y)
{
  if(fromState == toState)
  {
    x = fromState->getX() + controlDist * 200.0f * cos(controlAlpha);
    y = fromState->getY() + controlDist * 200.0f * sin(controlAlpha);
  }
  else
  {
    double xFrom = fromState->getX();
    double yFrom = fromState->getY();
    double xTo = toState->getX();
    double yTo = toState->getY();

    double yDiff = yTo - yFrom;
    double xDiff = xTo - xFrom;

    double beta = atan2(yDiff, xDiff);
    double gamma = beta + controlAlpha;
    double dist = sqrt(xDiff * xDiff + yDiff * yDiff);

    x = xFrom + dist * controlDist * cos(gamma);
    y = yFrom + dist * controlDist * sin(gamma);
  }
}

double Transition::getControlAlpha() const
{
  return controlAlpha;
}

double Transition::getControlDist() const
{
  return controlDist;
}

void Transition::setLabelPos(const double x, const double y) {
  // Get transition control points;
  double controlX, controlY;
  getControl(controlX, controlY);
  // Calculate reklative position of label to control points
  labelX = x - controlX;
  labelY = y - controlY;
}
void Transition::setControl(const double x, const double y)
{
  double newX = x;
  double newY = y;

  if(newX > 1000.0)
  {
    newX = 1000.0;
  }
  if(newX < -1000.0)
  {
    newX = -1000.0;
  }

  if(newY > 1000.0)
  {
    newY = 1000.0;
  }
  if(newY < -1000.0)
  {
    newY = -1000.0;
  }

  if(fromState == toState)
  {
    double xDiff = newX - fromState->getX();
    double yDiff = newY - fromState->getY();
    controlDist = sqrt(xDiff * xDiff + yDiff * yDiff)  / 200.0f;
    controlAlpha = atan2(yDiff, xDiff);
  }
  else
  {
    double xFrom = fromState->getX();
    double yFrom = fromState->getY();

    double xTo = toState->getX();
    double yTo = toState->getY();

    double yDiff = yTo - yFrom;
    double xDiff = xTo - xFrom;

    double yDiff1 = newY - yFrom;
    double xDiff1 = newX - xFrom;

    double stateDist = sqrt(xDiff * xDiff + yDiff * yDiff);

    double beta = atan2(yDiff, xDiff);
    double gamma = atan2(yDiff1, xDiff1);

    controlAlpha = gamma - beta;

    controlDist = (newX - xFrom) / (stateDist * cos(gamma));
  }
}

void Transition::setControlAlpha(const double alpha)
{
  controlAlpha = alpha;
}

void Transition::deselect()
{
  selected = false;
}

void Transition::select()
{
  selected = true;
}


bool Transition::isSelected()
{
  return selected;
}

std::string Transition::getLabel() const
{
  return label;
}
