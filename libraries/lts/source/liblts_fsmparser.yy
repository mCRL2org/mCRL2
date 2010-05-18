// Author(s): Muck van Weerdenburg
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file fsmparser.yy

%{
#include <vector>
#include <sstream>
#include <cstring>
#include "mcrl2/lts/lts.h"
#include "liblts_fsmparser.h"
#include "liblts_fsmlexer.h"

// Local variables
std::vector<bool> ignore_par; /* Records which parameters will be ignored */
unsigned int num_pars;        /* Number of parameters */
unsigned int par_index;       /* Index of current parameter */

// Function declarations

//external declarations from fsmlexer.ll
void fsmyyerror(const char *s);
int fsmyylex(void);

char* intToCString(int i);

#define safe_assign(lhs, rhs) { ATbool b; ATindexedSetPut(fsm_lexer_obj->protect_table, (ATerm) rhs, &b); lhs = rhs; }
%}

%union {
  ATermAppl aterm;
  int number;
}

//more verbose and specific error messages
%error-verbose

//set name prefix
%name-prefix="fsmyy"

%start fsm_file

%token EOLN SECSEP LPAR RPAR ARROW
%token <number> NUMBER
%token <aterm> ID QUOTED
%type  <aterm> type_name type_name1 action

%%

fsm_file :
    {
      num_pars = 0;
      ignore_par.clear();
    }
  params
    {
      fsm_lexer_obj->valueTable = ATreverse( fsm_lexer_obj->valueTable );
    }
  SECSEP EOLN
  states
  SECSEP EOLN transitions
  ;

// --------- Section containing the state vector ----------

params :
  /* empty */
  |
  params param
    {
      ++num_pars;
    }
  EOLN
  ;

param :
  ID
    {
      fsm_lexer_obj->typeId = $1;
    }
  cardinality type_def
  ;

cardinality :
  LPAR NUMBER RPAR
    {
      ignore_par.push_back($2 == 0);
    }
  ;

type_def :
  type_name
    {
      if (!ignore_par[num_pars])
      {
        fsm_lexer_obj->typeValues = ATempty;
        fsm_lexer_obj->typeId = ATmakeAppl2(fsm_lexer_obj->const_ATtype,(ATerm) fsm_lexer_obj->typeId,(ATerm) $1);
      }
    }
  type_values
    {
      if (!ignore_par[num_pars])
      {
        fsm_lexer_obj->typeValues = ATreverse( fsm_lexer_obj->typeValues );
        fsm_lexer_obj->valueTable = ATinsert( fsm_lexer_obj->valueTable,
            (ATerm)fsm_lexer_obj->typeValues );
      }
    }
  ;

type_name :
  /* empty */
    { safe_assign($$, ATmakeAppl0( ATmakeAFun( "", 0, ATfalse ) )) }
  |
  type_name1
    { safe_assign($$, $1) }
  |
  type_name ARROW type_name1
    {
      std::string result = static_cast<std::string> ( ATwriteToString( (ATerm)$1 ) )
        + "->" + static_cast<std::string> ( ATwriteToString( (ATerm)$3 ) );
      safe_assign($$, ATmakeAppl0( ATmakeAFun( result.c_str(), 0, ATfalse ) ))
    }
  ;

type_name1 :
  ID
    { safe_assign($$, $1) }
  |
  LPAR type_name RPAR
    {
      std::string result = "(" + static_cast<std::string> ( ATwriteToString(
        (ATerm)$2) ) + ")";
      safe_assign($$, ATmakeAppl0( ATmakeAFun( result.c_str(), 0, ATfalse ) ))
    }
  ;


type_values :
  /* empty */
  |
  type_values type_value
  ;

type_value :
  QUOTED
    {
      if (!ignore_par[num_pars])
      {
        fsm_lexer_obj->typeValues = ATinsert( fsm_lexer_obj->typeValues,
            (ATerm)ATmakeAppl2(fsm_lexer_obj->const_ATvalue, (ATerm)$1,
            (ATerm)fsm_lexer_obj->typeId ) );
      }
    }
  ;

// ----------- Section containing the states ---------

states :
  /* empty */
  |
  states
    {
      par_index = 0;
    }
  state
    {
      fsm_lexer_obj->stateVector = ATreverse( fsm_lexer_obj->stateVector );
      unsigned int i = fsm_lexer_obj->fsm_lts->add_state(
          (ATerm) fsm_lexer_obj->stateVector );
      if ( i == 0 )
      {
        fsm_lexer_obj->fsm_lts->set_initial_state( i );
      }
      fsm_lexer_obj->stateVector = ATempty
    }
  EOLN
  ;

state :
  /* empty */
  |
  state NUMBER
    {
      if (!ignore_par[par_index])
      {
        if ( par_index < ATgetLength( fsm_lexer_obj->valueTable ) )
        {
          fsm_lexer_obj->stateVector = ATinsert( fsm_lexer_obj->stateVector,
              ATelementAt( (ATermList)ATelementAt( fsm_lexer_obj->valueTable,
                  par_index ), $2 ) );
        }
      }
      ++par_index;
    }
  ;

// ---------- Section containing the transitions ----------

transitions:
  /* empty */
  |
  transitions transition
  EOLN
  ;

transition:
  NUMBER NUMBER action
    {
      unsigned int frState = $1-1;
      unsigned int toState = $2-1;
      ATerm label = ATtableGet(fsm_lexer_obj->labelTable,(ATerm)$3);
      if ( label == NULL )
      {
        unsigned int i = fsm_lexer_obj->fsm_lts->add_label((ATerm)$3,
            !strcmp("tau",ATgetName(ATgetAFun($3))));
        label = (ATerm) ATmakeInt(i);
        ATtablePut(fsm_lexer_obj->labelTable,(ATerm)$3,label);
      }
      fsm_lexer_obj->fsm_lts->add_transition(mcrl2::lts::transition(frState,
          ATgetInt((ATermInt)label), toState ));
    }
  ;

action :
  /* empty */
    { safe_assign($$, ATmakeAppl0( ATmakeAFun( "", 0, ATfalse ) )) }
  |
  QUOTED
    { safe_assign($$, $1) }
  ;

%%

char* intToCString( int i )
{
    std::ostringstream oss;
    oss << i;
    return (char*)oss.str().c_str();
}
