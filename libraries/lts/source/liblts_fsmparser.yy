// Author(s): Muck van Weerdenburg
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file fsmparser.yy

%{
#define YYSTYPE std::string

#include <vector>
#include <sstream>
#include <cstring>
#include "mcrl2/lts/lts.h"

#include "liblts_fsmparser.h"
#include "liblts_fsmlexer.h"

#include "mcrl2/lts/lts_fsm.h"

// Local variables
std::vector<bool> ignore_par; /* Records which parameters will be ignored */
size_t num_pars;        /* Number of parameters */
size_t par_index;       /* Index of current parameter */

// Function declarations

//external declarations from fsmlexer.ll
void fsmyyerror(const char *s);
int fsmyylex(void);

char* intToCString(size_t i);

%}


//more verbose and specific error messages
%error-verbose

//set name prefix
%name-prefix="fsmyy"

%start fsm_file

%token EOLN SECSEP LPAR RPAR ARROW HASH QMARK COLON COMMA BAG BAR KWSTRUCT SET LIST
%token NUMBER
%token ID QUOTED BOOL POS NAT INT REAL

%%

fsm_file :
    {
      num_pars = 0;
      fsm_lexer_obj->fsm_lts->clear_process_parameters();
      ignore_par.clear();
    }
  params
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
      ignore_par.push_back(atoi($2.c_str()) == 0);
    }
  ;

type_def :
  /* empty; assume type Nat is intended */
    { 
      fsm_lexer_obj->fsm_lts->add_process_parameter(fsm_lexer_obj->typeId,"Nat");
    }
  |
  sort_expr
    {
      if (!ignore_par[num_pars])
      {
        fsm_lexer_obj->fsm_lts->add_process_parameter(fsm_lexer_obj->typeId,$1);
      }
    }
  type_values
    {
    }
  ;

//Sort expressions
//----------------

//sort expression
sort_expr:
  sort_expr_arrow
    {
      $$=$1;
    }
  ;

//arrow sort expression
sort_expr_arrow:
  sort_expr_struct
    {
      $$=$1;
    }
  | domain_no_arrow ARROW sort_expr_arrow
    {
      $$ = $1 + "->" + $3;
    }
  ;

//domain
domain_no_arrow:
  domain_no_arrow_elts_hs
    {
      $$=$1;
    }
  ;

//one or more domain elements, separated by hashes
domain_no_arrow_elts_hs:
  domain_no_arrow_elt
    {
      $$=$1;
    }
  | domain_no_arrow_elts_hs HASH domain_no_arrow_elt
    {
      $$ = $1 + "#" + $3;
    }
  ;

//domain element
domain_no_arrow_elt:
  sort_expr_struct
    {
      $$ = $1;
    }
  ;

//structured sort
sort_expr_struct:
  sort_expr_primary
    {
      $$ = $1;
    }
  | KWSTRUCT struct_constructors_bs
    {
      $$ = "struct " + $2;
    }
  ;

//one ore more structured sort constructors, separated by bars
struct_constructors_bs:
  struct_constructor
    {
      $$ = $1;
    }
  | struct_constructors_bs BAR struct_constructor
    {
      $$ = $1 + "|" + $3;
    }
  ;

//structured sort constructor
struct_constructor:
  ID recogniser
    {
      $$ = $1 + " " + $2;   
    }
  | ID LPAR struct_projections_cs RPAR recogniser
    {
      $$ = $1 + "(" + $3 + ")" + $5;
    }
  ;

//recogniser
recogniser:
  /* empty */
    {
      $$ = "";
    }
  | QMARK ID
    {
      $$ = "?" + $2;
    }
  ;

//one or more structured sort projections, separated by comma's
struct_projections_cs:
  struct_projection
    {
      $$ = $1;
    }
  | struct_projections_cs COMMA struct_projection
    {
      $$ = $1 + "," + $3;
    }
  ;

//structured sort projection
struct_projection:
  sort_expr
    {
      $$ = $1;
    }
  | ID COLON sort_expr
    {
      $$ = $1 + ":" + $3;
    }
  ;

//primary sort expression
sort_expr_primary:
  ID
    {
      $$ = $1; 
    }
  | sort_constant
    {
      $$ = $1; 
    }
  | sort_constructor
    {
      $$ = $1; 
    }
  | LPAR sort_expr RPAR
    { 
      $$ = "(" + $2 + ")";
    }
  ;

//sort constant
sort_constant:
  BOOL
    {
      $$ = "Bool"; 
    }
  | POS
    {
      $$ = "Pos";
    }
  | NAT
    {
      $$ = "Nat";
    }
  | INT
    {
      $$ = "Int";
    }
  | REAL
    {
      $$ = "Real";
    }
  ;

//sort constructor
sort_constructor:
  LIST LPAR sort_expr RPAR
    {
      $$ = "List(" + $3 + ")";
    }
  | SET LPAR sort_expr RPAR
    {
      $$ = "Set(" + $3 + ")";
    }
  | BAG LPAR sort_expr RPAR
    {
      $$ = "Bag(" + $3 + ")";
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
        fsm_lexer_obj->fsm_lts->add_state_element_value(num_pars,$1);
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
      size_t i = fsm_lexer_obj->fsm_lts->add_state(fsm_lexer_obj->stateVector);
      if ( i == 0 )
      {
        fsm_lexer_obj->fsm_lts->set_initial_state( i );
      }
      fsm_lexer_obj->stateVector.clear();
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
        fsm_lexer_obj->stateVector.push_back(atoi($2.c_str()));
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
      using namespace mcrl2::lts;
      size_t frState = atoi($1.c_str())-1;
      size_t toState = atoi($2.c_str())-1;

      std::map < std::string, size_t>::const_iterator label_index=fsm_lexer_obj->labelTable.find($3);
      if (label_index==fsm_lexer_obj->labelTable.end())
      { // Not found. This label does not occur in the fsm.
        const lts_fsm_t::labels_size_type n=fsm_lexer_obj->fsm_lts->add_action($3,$3=="tau");
        fsm_lexer_obj->labelTable[$3]=n;
        fsm_lexer_obj->fsm_lts->add_transition(transition(frState,n,toState));
      }
      else
      { // The label is found. It already exists.
        fsm_lexer_obj->fsm_lts->add_transition(transition(frState,label_index->second,toState));
      }

    }
  ;

action :
  /* empty */
    { $$ = ""; 
    }
  |
  QUOTED
    { 
      $$ = $1;
    }
  ;

%%

char* intToCString( size_t i )
{
    std::ostringstream oss;
    oss << i;
    return (char*)oss.str().c_str();
}
