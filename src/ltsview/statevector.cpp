#include "statevector.h"

StateVector::StateVector( vector< string > values )
{
  // The ATerm library provides:
  // * ATappend(l,e): add e to the end of list l (complexity: linear in l)
  // * ATinsert(l,e): insert e at the beginning of list l (complexity: constant)
  // Hence, we run down the vector in reverse order and use ATinsert

  stateVector = ATempty;
  for ( int i = values.size()-1 ; i >= 0 ; --i )
    stateVector = ATinsert( stateVector, ATmake( "<str>", values[i].c_str() ) );
  ATprotect( (ATerm*) &stateVector );
}

StateVector::~StateVector()
{
  ATunprotect( (ATerm*) &stateVector );
}

string StateVector::GetValueOf( int paramIndex ) const
{
  char* result = strdup( ATwriteToString( ATelementAt( stateVector, paramIndex ) ) );
  return static_cast< string > ( result );
}

vector< string > StateVector::GetValues() const
{
  vector< string > result;
  for ( int i = 0 ; i < ATgetLength( stateVector ) ; ++i )
  {
    result.push_back( GetValueOf( i ) );
  }
  return result;
}
