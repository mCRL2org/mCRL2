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
    
    // Getters
    State* getFrom() const;
    State* getTo() const;
    std::string getLabel() const;

    void getControl(double &x, double &y);
    double getControlAlpha() const;
    double getControlDist() const;
    
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

    bool selected;

};

#endif //TRANSITION_H
