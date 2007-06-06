#include "transition.h"
using namespace Utils;

// Constructor and Destructor

Transition::Transition(State* bs,State* es,int lbl) {
  beginState = bs;
  endState = es;
  label = lbl;
  backpointer = false;
  marked = NULL;
}

Transition::~Transition() {
}

State* Transition::getBeginState() const
{
  return beginState;
}

State* Transition::getEndState() const
{
  return endState;
}

int Transition::getLabel() const
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

