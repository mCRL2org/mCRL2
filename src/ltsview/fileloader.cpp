#include "fileloader.h"

FileLoader::FileLoader( Mediator* owner )
{
  mediator = owner;
}

FileLoader::~FileLoader()
{
}

void FileLoader::loadFile( const string fileName, LTS* const lts )
// pre: lts points to an empty LTS object
{
  string ext = fileName.substr( fileName.find_last_of( "." ) + 1, string::npos );
  if ( ext != "fsm" )
    throw "Unknown file type";
  return loadFSMFile( fileName, lts );
}

void FileLoader::loadFSMFile( const string fileName, LTS* const lts )
{
  parseFSMfile( fileName, lts );
}
