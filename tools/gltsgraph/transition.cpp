#include "transition.h"
#include <math.h>
Transition::Transition(State* from, State* to, std::string lbl)
{
  fromState = from;
  toState = to;
  label = lbl;
  controlAlpha = .1 * M_PI;
  controlDist = 0.5;

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
