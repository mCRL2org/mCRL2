#ifndef DATATYPE_H
#define DATATYPE_H

#include "aterm/aterm2.h"
#include <string>
#include <set>
using namespace std;

class DataType
{
  private:
    ATermAppl dataType;

  public:
    DataType( string name, set< string > values );
    ~DataType();
};
#endif
