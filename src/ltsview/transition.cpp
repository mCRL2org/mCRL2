#include "transition.h"

// Constructor and Destructor

Transition::Transition(State* bs, State* es, ATerm lbl )
{
  beginState = bs;
  endState = es;
  label = lbl;
  ATprotect( &label );
  backpointer = false;
  marked = NULL;
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

ATerm Transition::getLabel() const
{
  return label;
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
  return *marked;
}

void Transition::setMarkedPointer( bool* bp )
{
  marked = bp;
}
