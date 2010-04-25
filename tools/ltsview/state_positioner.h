
#ifndef STATE_POSITIONER_H
#define STATE_POSITIONER_H

class LTS;
class State;

class StatePositioner
{
  public:
    StatePositioner(LTS *l): lts(l) {} 
    virtual ~StatePositioner() {}
    virtual void positionStates() = 0;

  protected:
    LTS* lts;
};

#endif
