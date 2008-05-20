#ifndef __FSMPARSER_H
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING).
#define __FSMPARSER_H

#include <iostream>
#include <sstream>
#include "mcrl2/lts/liblts.h"

bool parse_fsm( std::istream &stream, mcrl2::lts::lts &lts );

#endif
