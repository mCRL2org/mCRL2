#ifndef __FSMLEXER_H
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#define __FSMLEXER_H

// #include <aterm2.h>
#include "mcrl2/lts/lts_fsm.h"


class fsm_lexer
{
  public:
    mcrl2::lts::detail::state_label_fsm stateVector;
    std::string typeId;
    std::map < std::string, size_t> labelTable;
    mcrl2::lts::lts_fsm_t* fsm_lts;
};

extern fsm_lexer* fsm_lexer_obj;

#endif
