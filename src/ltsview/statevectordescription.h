#ifndef STATEVECTORDESCRIPTION_H
#define STATEVECTORDESCRIPTION_H

#include "aterm/aterm2.h"
#include <string>
#include <vector>
using namespace std;

class StateVectorDescription
{
  private:
    ATermList svDescription;
  public:
    StateVectorDescription( vector< string > paramNames, vector< string > paramTypes );
    ~StateVectorDescription();
};
#endif
