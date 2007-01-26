#include "fileloader.h"
#include "fsmparser.h"
using namespace std;
using namespace Utils;

void FileLoader::loadFile( const string fileName, LTS* const lts )
// pre: lts points to an empty LTS object
{
  string ext = fileName.substr( fileName.find_last_of( "." ) + 1, string::npos );
  string ext_lc = ext;
  for ( unsigned int i = 0 ; i < ext.size() ; ++i )
  {
    ext_lc[i] = tolower(ext_lc[i]);
  }

  if ( ext_lc != "fsm" )
  {
    throw string( "Unknown file extension: " + ext );
  }
  else    
    loadFSMFile( fileName, lts );
}

void FileLoader::loadFSMFile( const string fileName, LTS* const lts )
{
  parseFSMfile( fileName, lts );
}
