#ifndef TRANSITION_H
#define TRANSITION_H
#ifndef STATE_H
  #include "state.h"
#else
  class State;
#endif

class Transition
{
  public:
    Transition(State* bs,State* es,int label);
    ~Transition();
    State*	getBeginState() const;
    State*	getEndState() const;
    int			getLabel() const;
    bool		isBackpointer() const;
    bool		isMarked() const;
    void		setBackpointer(bool b);
    void		setMarkedPointer(bool* bp);

  private:
    bool    backpointer;
    State*  beginState;
    State*  endState;
    int   label;
    bool*   marked;
};

#endif
