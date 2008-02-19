#ifndef __FSMPARSER_H
#define __FSMPARSER_H

#include <iostream>
#include <sstream>
#include "mcrl2/lts/liblts.h"

bool parse_fsm( std::istream &stream, mcrl2::lts::lts &lts );

#endif
