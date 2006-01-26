#ifndef TRANSITION_H
#define TRANSITION_H

#include "action.h"

#ifndef STATE_H
  #include "state.h"
#else
  class State;
#endif


class Transition
{
  public:
    Transition( State* bs, State* es, Action* a );
    ~Transition();
    Action* getAction() const;
    State*  getBeginState() const;
    State*  getEndState() const;
    bool    isBackpointer() const;
    bool    isMarked() const;
    void    mark();
    void    setBackpointer(bool b);
    void    unmark();
      
  private:
    Action* action;
    bool    backpointer;
    State*  beginState;
    State*  endState;
    bool    marked;
};

#endif
