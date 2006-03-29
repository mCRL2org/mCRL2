#include "state.h"

State::State( ATermList sv )
{
  stateVectorAT = sv;
  ATprotectList( &stateVectorAT );
  while ( !ATisEmpty( sv ) )
  {
    stateVector.push_back( ATgetFirst( sv ) );
    sv = ATgetNext( sv );
  }
  cluster = NULL;
  rank = 0;
  position = -1.0f;
  marked = false;
}

State::~State()
{
  ATunprotectList( &stateVectorAT );
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

bool State::isDeadlock() const
{
  return ( outTransitions.size() == 0 );
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

float State::getPosition() const
{
  return position;
}

void State::setPosition( float p )
{
  position = p;
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

int State::getValueIndexOfParam( int paramIndex )
{
  return ATgetInt((ATermInt)ATgetArgument((ATermAppl)stateVector[paramIndex],1));
}
