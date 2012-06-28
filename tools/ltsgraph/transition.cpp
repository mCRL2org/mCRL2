// Author(s): Carst Tankink and Ali Deniz Aladagli
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

#include "mcrl2/utilities/workarounds.h" // for M_PI on Windows with MSVC

Transition::Transition(State* from, State* to, std::string lbl)
{
  fromState = from;
  toState = to;
  label = lbl;
  if (fromState == toState)
  {
    controlAlpha = 0.5 * M_PI;
  }
  else
  {
    controlAlpha = 0;
  }
  controlBeta = 0;
  controlGamma = 0;
  controlDist = 0.5;
  labelX = 0.0;
  labelY = 0.025;
  labelZ = 0.0;
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

void Transition::getLabelPos(double& x, double& y, double& z)
{
  // Get control point of transition
  getControl(x, y, z);

  // labelX and labelY and labelZ are relative to the control point
  x = x + labelX;
  y = y + labelY;
  z = z + labelZ;
}

void Transition::getControl(double& x, double& y, double& z)
{
  if (fromState == toState)
  {
    x = fromState->getX() + controlDist * 200.0f * sin(controlAlpha);
    y = fromState->getY() + controlDist * 200.0f * sin(controlBeta);
    z = fromState->getZ() + controlDist * 200.0f * sin(controlGamma);
  }
  else
  {
    double xFrom = fromState->getX();
    double yFrom = fromState->getY();
    double zFrom = fromState->getZ();
    double xTo = toState->getX();
    double yTo = toState->getY();
    double zTo = toState->getZ();

    double yDiff = yTo - yFrom;
    double xDiff = xTo - xFrom;
    double zDiff = zTo - zFrom;

    double delta = atan2(xDiff, sqrt(yDiff * yDiff + zDiff * zDiff));
    double epsilon = delta + controlAlpha;

    double zeta = atan2(yDiff, sqrt(xDiff * xDiff + zDiff * zDiff));
    double theta = zeta + controlBeta;

    double iota = atan2(zDiff, sqrt(xDiff * xDiff + yDiff * yDiff)) ;
    double kappa = iota + controlGamma;

    double dist = sqrt(xDiff * xDiff + yDiff * yDiff + zDiff * zDiff);

    x = xFrom + dist * controlDist * sin(epsilon);
    y = yFrom + dist * controlDist * sin(theta);
    z = zFrom + dist * controlDist * sin(kappa);

  }
}

double Transition::getControlAlpha() const
{
  return controlAlpha;
}

double Transition::getControlBeta() const
{
  return controlBeta;
}

double Transition::getControlGamma() const
{
  return controlGamma;
}

double Transition::getControlDist() const
{
  return controlDist;
}

void Transition::setLabelPos(const double x, const double y, const double z)
{
  // Get transition control points;
  double controlX, controlY, controlZ;
  getControl(controlX, controlY, controlZ);
  // Calculate reklative position of label to control points
  labelX = x - controlX;
  labelY = y - controlY;
  labelZ = z - controlZ;
}
void Transition::setControl(const double x, const double y, const double z)
{
  double newX = x;
  double newY = y;
  double newZ = z;

  if (fromState == toState)
  {
    double xDiff = newX - fromState->getX();
    double yDiff = newY - fromState->getY();
    double zDiff = newZ - fromState->getZ();
    controlDist = sqrt(xDiff * xDiff + yDiff * yDiff + zDiff * zDiff)  / 200.0f;
    controlAlpha = atan2(xDiff, sqrt(yDiff * yDiff + zDiff * zDiff));
    controlBeta = atan2(yDiff, sqrt(xDiff * xDiff + zDiff * zDiff));
    controlGamma = atan2(zDiff, sqrt(xDiff * xDiff + yDiff * yDiff));
  }
  else
  {
    double xFrom = fromState->getX();
    double yFrom = fromState->getY();
    double zFrom = fromState->getZ();

    double xTo = toState->getX();
    double yTo = toState->getY();
    double zTo = toState->getZ();

    double yDiff = yTo - yFrom;
    double xDiff = xTo - xFrom;
    double zDiff = zTo - zFrom;

    double yDiff1 = newY - yFrom;
    double xDiff1 = newX - xFrom;
    double zDiff1 = newZ - zFrom;

    double stateDist = sqrt(xDiff * xDiff + yDiff * yDiff + zDiff * zDiff);

    double delta = atan2(xDiff, sqrt(yDiff * yDiff + zDiff * zDiff));
    double epsilon = atan2(xDiff1, sqrt(yDiff1 * yDiff1 + zDiff1 * zDiff1)) ;

    double zeta = atan2(yDiff, sqrt(xDiff * xDiff + zDiff * zDiff));
    double theta = atan2(yDiff1, sqrt(xDiff1 * xDiff1 + zDiff1 * zDiff1));

    double iota = atan2(zDiff, sqrt(xDiff * xDiff + yDiff * yDiff));
    double kappa = atan2(zDiff1, sqrt(xDiff1 * xDiff1 + yDiff1 * yDiff1));

    controlAlpha = epsilon - delta;
    controlBeta = theta - zeta;
    controlGamma = kappa - iota;

    controlDist = sqrt(xDiff1 * xDiff1 + yDiff1 * yDiff1 + zDiff1 * zDiff1) / stateDist;
  }
}

void Transition::setControlAlpha(const double alpha)
{
  controlAlpha = alpha;
}

void Transition::setcontrolBeta(const double beta)
{
  controlBeta = beta;
}

void Transition::setcontrolGamma(const double gamma)
{
  controlGamma = gamma;
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
