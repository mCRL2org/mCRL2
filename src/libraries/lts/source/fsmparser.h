#ifndef __FSMPARSER_H
#define __FSMPARSER_H

#include <iostream>
#include "lts/liblts.h"

bool parse_fsm( std::istream &stream, mcrl2::lts::lts &lts );

#endif
