#ifndef __FSMLEXER_H
#define __FSMLEXER_H

#include <aterm2.h>
#include "lts/liblts.h"

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
    
    AFun const_ATtypeid;
    AFun const_ATparmid;
    AFun const_ATvalue;
    AFun const_ATstate;
    AFun const_ATparam;

    mcrl2::lts::lts *fsm_lts;
};

extern fsm_lexer *fsm_lexer_obj;

#endif
