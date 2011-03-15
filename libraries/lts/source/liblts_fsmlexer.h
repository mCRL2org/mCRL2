#ifndef __FSMLEXER_H
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#define __FSMLEXER_H

// #include <aterm2.h>
#include "mcrl2/lts/lts_fsm.h"


class fsm_lexer
{
  public:
    // ATermIndexedSet protect_table;

    mcrl2::lts::detail::state_label_fsm stateVector;
    // ATermList valueTable;
    // ATermList stateId;
    // ATermList typeValues;
    std::string typeId;
    // ATermTable labelTable;
    std::map < std::string, size_t> labelTable;

    // AFun const_ATtype;
    // AFun const_ATvalue;

    mcrl2::lts::lts_fsm_t* fsm_lts;
};

extern fsm_lexer* fsm_lexer_obj;

#endif
