#ifndef STATE_H
#define STATE_H

#include <string>
#include <vector>
#include "utils.h"

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

    // Getters
    bool isInitialState() const;
    bool isSelected() const;
    bool isLocked() const;
    bool isDragged() const;

    Utils::Vect getPosition() const;
    double getX() const;
    double getY() const;

    std::string getLabel() const;
    size_t getValue() const;

    size_t getNumberOfTransitions() const;
    size_t getNumberOfInTransitions() const;
    size_t getNumberOfSelfLoops() const;
    Transition* getTransition(size_t i) const;
    Transition* getInTransition(size_t i) const;
    Transition* getSelfLoop(size_t i) const;

    // Setters
    void setInitialState(const bool isInitial);

    void setPosition(Utils::Vect p);
    void setX(const double x);
    void setY(const double y);

    Utils::Vect getImpulse() const;
    int getImpulseX() const;
    int getImpulseY() const;

    float getSkew() const;
    
    void setImpulse(const Utils::Vect p);
    void setImpulseX(const int x);
    void setImpulseY(const int y);

    void setSkew(const float s);
    void setTemperature(const float t);


    float getTemperature() const;
   
    void setLabel(std::string label);

    bool hasTransitionTo(State* to);
  private:
    bool isInitial;
    bool selected;
    bool locked;
    bool dragged;
  
    double outCurve;

    unsigned int value;
    Utils::Vect pos;  // Current position, Y
    Utils::Vect p; // X factor of previous movement
    float t;  // Local temperature
    float d;  // Skew gauge
    std::string label;

    std::vector<Transition*> outTransitions;
    std::vector<Transition*> inTransitions;
    std::vector<Transition*> selfLoops;


};



#endif //STATE_H
