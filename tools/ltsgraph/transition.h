// Author(s): Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file transition.h
/// \brief Transition class declaration.

#ifndef TRANSITION_H
#define TRANSITION_H

#ifndef STATE_H
  #include "state.h"
#else
  class State;
#endif

#include <string>
#include "utils.h"

class Transition
{
  public:
    Transition(State* from, State* to, std::string label);

    // Setters
    void setControl(const double x, const double y);
    void setControlAlpha(const double alpha);
    void setLabelPos(const double x, const double y);

    // Getters
    State* getFrom() const;
    State* getTo() const;
    std::string getLabel() const;

    void getControl(double &x, double &y);
    double getControlAlpha() const;
    double getControlDist() const;
    void getLabelPos(double &x, double &y);

    void select();
    void deselect();
    bool isSelected();

  private:
    State* fromState;
    State* toState;
    std::string label;

    // The position of the control point relative to fromState
    double controlAlpha;
    double controlDist;
    double labelX;
    double labelY;

    bool selected;

};

#endif //TRANSITION_H
