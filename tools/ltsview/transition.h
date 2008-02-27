// Author(s): Bas Ploeger and Carst Tankink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file transition.h
/// \brief Add your file description here.

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
    bool                isSelfLoop() const;
    bool		isMarked() const;
    void		setMarkedPointer(bool* bp);

  private:
    State*  beginState;
    State*  endState;
    int     label;
    bool*   marked;
};

#endif
