// Author(s): Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file state.h
/// \brief State class declaration.

#ifndef STATE_H
#define STATE_H

#include <string>
#include <vector>
#include <map>
#include "utils.h"
#include <wx/colour.h>
#include <wx/gdicmn.h>

#ifndef TRANSITION_H
  #include "transition.h"
#else
  class Transition;
#endif

class State
{
  public:
    // Constructor & destructor
    State(unsigned int value, bool isInitial);
    ~State();

    // Methods
    void addOutTransition(Transition* ot);
    void addInTransition(Transition* it);
    void addSelfLoop(Transition* sl);
    void select();
    void deselect();
    void lock();
    void drag();
    void stopDrag();
    void setShowStateVector(const bool value);

    // Getters
    bool isInitialState() const;
    bool isSelected() const;
    bool isLocked() const;
    bool isDragged() const;
    bool getShowStateVector() const;

    Utils::Vect getPosition() const;
    double getX() const;
    double getY() const;
	double getZ() const;

    std::string getLabel() const;
    size_t getValue() const;

    size_t getNumberOfTransitions() const;
    size_t getNumberOfInTransitions() const;
    size_t getNumberOfSelfLoops() const;

    Transition* getTransition(size_t i) const;
    Transition* getInTransition(size_t i) const;
    Transition* getSelfLoop(size_t i) const;
    wxColour getColour() const;

    // Setters
    void setInitialState(const bool isInitial);

    void setPosition(Utils::Vect p);
    void setX(const double x);
    void setY(const double y);
	void setZ(const double z);
    void setParameters(std::map<std::string, std::string>& params);

    void setColour(const wxColour colour);

    void setLabel(std::string label);

    bool hasTransitionTo(State* to);
    const std::map<std::string, std::string>& getParameters() const;
  private:
    wxColour colour;
    bool isInitial;
    bool selected;
    bool locked;
    bool dragged;
    bool showStateVector;

    double outCurve;

    unsigned int value;
    Utils::Vect pos;  // Current position, Y
    std::string label;

    std::vector<Transition*> outTransitions;
    std::vector<Transition*> inTransitions;
    std::vector<Transition*> selfLoops;
    std::map<std::string, std::string> parameterValues;

};



#endif //STATE_H
