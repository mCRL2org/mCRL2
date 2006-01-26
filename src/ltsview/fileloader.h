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
    FileLoader( Mediator* owner );
    ~FileLoader();
    void loadFile( const string fileName, LTS* const lts );
  private:
    Mediator* mediator;
    void loadFSMFile( const string fileName, LTS* const lts );
};

#endif // LOADER_H
