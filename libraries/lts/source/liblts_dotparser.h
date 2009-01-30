#ifndef __DOTPARSER_H
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#define __DOTPARSER_H

#include <iostream>
#include <sstream>
#include "mcrl2/lts/lts.h"

bool parse_dot( std::istream &stream, mcrl2::lts::lts &lts );

#endif
