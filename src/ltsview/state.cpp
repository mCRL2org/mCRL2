#include "state.h"

State::State( ATermAppl sv )
{
  stateVector = sv;
  ATprotect( (ATerm*) &stateVector );
  cluster = NULL;
  rank = 0;
}

State::~State()
{
  ATunprotect( (ATerm*) &stateVector );
}

void State::addSuperior( State* s )
{
  superiors.insert( s );
}

void State::addSubordinate( State* s )
{
  subordinates.insert( s );
}

void State::addComrade( State* s )
{
  comrades.insert( s );
}

void State::addInTransition( Transition* trans )
{
  inTransitions.push_back( trans );
}

void State::addOutTransition( Transition* trans )
{
  outTransitions.push_back( trans );
}

void State::addLoop( Transition* trans )
{
  loops.push_back( trans );
}

bool State::isMarked() const
{
  return marked;
}

void State::mark()
{
  marked = true;
}

void State::unmark()
{
  marked = false;
}

bool State::isSelected() const
{
  return selected;
}

void State::select()
{
  selected = true;
}

void State::deselect()
{
  selected = false;
}

int State::getRank() const
{
  return rank;
}

void State::setRank( int r )
{
  rank = r;
}

Cluster* State::getCluster() const
{
  return cluster;
}

void State::setCluster( Cluster* c )
{
  cluster = c;
}

void State::getSubordinates( set< State* > &ss ) const
{
  ss = subordinates;
}

void State::getSuperiors( set< State* > &ss ) const
{
  ss = superiors;
}

void State::getComrades( set< State* > &ss ) const
{
  ss = comrades;
}

void State::getInTransitions( vector< Transition* > &ts ) const
{
  ts = inTransitions;
}

void State::getOutTransitions( vector< Transition* > &ts ) const
{
  ts = outTransitions;
}

void State::clearHierarchyInfo()
{
  superiors.clear();
  subordinates.clear();
  comrades.clear();
}
