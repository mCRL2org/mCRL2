#ifndef ACTION_H
#define ACTION_H

#include "aterm/aterm2.h"
#include <string>
#include <vector>
using namespace std;

class Action
{
  private:
    ATermAppl action;
  
  public:
    Action( string actname, vector< string > actparams );
    ~Action();
    string ToString() const;
};

#endif
