#ifndef __DOTLEXER_H
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#define __DOTLEXER_H

#include "mcrl2/atermpp/indexed_set.h"
#include "mcrl2/lts/lts_dot.h"

class dot_lexer
{
  public:
    atermpp::indexed_set protect_table;
    std::map < std::string, size_t> labelTable;
    std::map < std::string, size_t> stateTable;
    std::vector < std::string > state_sequence;
    mcrl2::lts::lts_dot_t* dot_lts;
};

extern dot_lexer* dot_lexer_obj;

#endif
