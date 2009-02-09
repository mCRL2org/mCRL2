#ifndef __FSMLEXER_H
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#define __FSMLEXER_H

#include <aterm2.h>
#include "mcrl2/lts/lts.h"

class fsm_lexer
{
  public:
    ATermIndexedSet protect_table;

    ATermList stateVector;
    ATermList valueTable;
    ATermList stateId;
    ATermList typeValues;
    ATermAppl typeId;
    ATermTable labelTable;

    AFun const_ATtype;
    AFun const_ATvalue;

    mcrl2::lts::lts *fsm_lts;
};

extern fsm_lexer *fsm_lexer_obj;

#endif
