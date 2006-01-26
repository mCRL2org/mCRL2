#ifndef STATEVECTOR_H
#define STATEVECTOR_H

#include "aterm/aterm2.h"
#include <vector>
#include <string>
using namespace std;

class StateVector
{
  private:
    ATermList stateVector;

  public:
    StateVector(vector< string > values);
    ~StateVector();
    
    string GetValueOf( int paramIndex ) const;
    vector< string > GetValues() const;
};

#endif
