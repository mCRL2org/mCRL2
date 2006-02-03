#include "transition.h"

// Constructor and Destructor

Transition::Transition(State* bs, State* es, ATerm lbl )
{
  beginState = bs;
  endState = es;
  label = lbl;
  ATprotect( &label );
  backpointer = false;
}

Transition::~Transition()
{
  ATunprotect( &label );
}

State* Transition::getBeginState() const
{
  return beginState;
}

State* Transition::getEndState() const
{
  return endState;
}

bool Transition::isBackpointer() const
{
  return backpointer;
}

void Transition::setBackpointer( bool b )
{
  backpointer = b;
}

bool Transition::isMarked() const
{
  return marked;
}

void Transition::mark()
{
  marked = true;
}

void Transition::unmark()
{
  marked = false;
}
