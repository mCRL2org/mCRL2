#ifndef TRANSITION_H
#define TRANSITION_H

#include "aterm1.h"

#ifndef STATE_H
  #include "state.h"
#else
  class State;
#endif


class Transition
{
  public:
    Transition( State* bs, State* es, ATerm lbl );
    ~Transition();
    State*  getBeginState() const;
    State*  getEndState() const;
    ATerm   getLabel() const;
    bool    isBackpointer() const;
    bool    isMarked() const;
    void    setBackpointer( bool b );
    void    setMarkedPointer( bool* bp );
      
  private:
    bool    backpointer;
    State*  beginState;
    State*  endState;
    ATerm   label;
    bool*   marked;
};

#endif
