#include <ostream>
#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>
#include "aterm2.h"
#include "lts.h"
#include "state.h"
#include "transition.h"
#include "liblowlevel.h"
//using namespace std;

void parseFSMfile( std::string fileName, LTS* const lts );
