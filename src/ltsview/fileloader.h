#ifndef FILELOADER_H
#define FILELOADER_H

#include <vector>
#include <map>
#include <set>
#include <string>
#include "mediator.h"
#include "lts.h"
#include "fsmparser.h"

using namespace std;

class FileLoader
{
  public:
    static void loadFile( const string fileName, LTS* const lts );
  private:
    static void loadFSMFile( const string fileName, LTS* const lts );
};

#endif // LOADER_H
