#include "transition.h"
using namespace Utils;

// Constructor and Destructor

Transition::Transition(State* bs, State* es, ATerm lbl )
{
  beginState = bs;
  endState = es;
  label = lbl;
  ATprotect( &label );
  backpointer = false;
  marked = NULL;

  // Initialisation of simulation
  selected = false;
  simulationState = UNSEEN;
  visitedAt = 0;
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

// Method implementation of simulation
bool Transition::isSelected() const {
  return selected;
}

SimState Transition::getSimulationState() const {
  return simulationState;
}

int Transition::getVisitedAt() const {
  return visitedAt;
}

void Transition::setSelected(bool s) {
  selected = s;
}

void Transition::setSimulationState(SimState st) {
  simulationState = st;
}

void Transition::setVisitedAt(int va) {
  visitedAt = va;
}
