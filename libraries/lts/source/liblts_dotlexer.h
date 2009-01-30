#ifndef __DOTLEXER_H
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#define __DOTLEXER_H

#include <aterm2.h>
#include "mcrl2/lts/lts.h"

class dot_lexer
{
  public:
    ATermIndexedSet protect_table;

    mcrl2::lts::lts *dot_lts;
};

extern dot_lexer *dot_lexer_obj;

#endif
