#include "transition.h"

// Constructor and Destructor

Transition::Transition(State* bs, State* es, Action* a)
{
  beginState = bs;
  endState = es;
  action = a;
  backpointer = false;
}

Transition::~Transition()
{
}

State* Transition::getBeginState() const
{
  return beginState;
}

State* Transition::getEndState() const
{
  return endState;
}

Action* Transition::getAction() const
{
  return action;
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
