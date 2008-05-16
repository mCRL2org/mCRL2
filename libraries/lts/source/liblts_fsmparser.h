#ifndef __FSMPARSER_H
// Copyright: see the accompanying file COPYING.
#define __FSMPARSER_H

#include <iostream>
#include <sstream>
#include "mcrl2/lts/liblts.h"

bool parse_fsm( std::istream &stream, mcrl2::lts::lts &lts );

#endif
