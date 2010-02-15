// Author(s): Carst Tankink and Ali Deniz Aladagli
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
    void setControl(const double x, const double y, const double z);
    void setControlAlpha(const double alpha);
	void setcontrolBeta(const double beta);
	void setcontrolGamma(const double gamma);

    void setLabelPos(const double x, const double y, const double z);

    // Getters
    State* getFrom() const;
    State* getTo() const;
    std::string getLabel() const;

    void getControl(double &x, double &y, double &z);
    double getControlAlpha() const;
	double getControlBeta() const;
	double getControlGamma() const;
    double getControlDist() const;
    void getLabelPos(double &x, double &y, double &z);

    void select();
    void deselect();
    bool isSelected();

  private:
    State* fromState;
    State* toState;
    std::string label;

    // The position of the control point relative to fromState
    double controlAlpha;
	double controlBeta;
	double controlGamma;
	double controlDist;
    double labelX;
    double labelY;
	double labelZ;

    bool selected;

};

#endif //TRANSITION_H
