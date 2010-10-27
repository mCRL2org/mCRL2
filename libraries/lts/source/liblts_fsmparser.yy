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
unsigned int num_pars;        /* Number of parameters */
unsigned int par_index;       /* Index of current parameter */

// Function declarations

//external declarations from fsmlexer.ll
void fsmyyerror(const char *s);
int fsmyylex(void);

char* intToCString(int i);

%}


/* %union {
  // ATermAppl aterm;
  std::string result_string;
  int number;
} */

//more verbose and specific error messages
%error-verbose

//set name prefix
%name-prefix="fsmyy"

%start fsm_file

%token EOLN SECSEP LPAR RPAR ARROW HASH QMARK COLON COMMA BAG BAR KWSTRUCT SET LIST
%token NUMBER
%token ID QUOTED BOOL POS NAT INT REAL
/* %type  sort_expr sort_expr_arrow domain_no_arrow sort_expr_struct domain_no_arrow_elts_hs struct_constructors_bs struct_constructor recogniser struct_projections_cs struct_projection sort_expr_primary sort_constant sort_constructor domain_no_arrow_elt action
*/

%%

fsm_file :
    {
      num_pars = 0;
      fsm_lexer_obj->fsm_lts->clear_process_parameters();
      ignore_par.clear();
    }
  params
    {
      // fsm_lexer_obj->valueTable = ATreverse( fsm_lexer_obj->valueTable );
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
      ignore_par.push_back(atoi($2.c_str()) == 0);
    }
  ;

type_def :
  sort_expr
    {
      if (!ignore_par[num_pars])
      {
        // fsm_lexer_obj->typeValues = ATempty;
        // fsm_lexer_obj->fsm_lts->add_process_parameter(ATgetName(ATgetAFun(fsm_lexer_obj->typeId)),ATgetName(ATgetAFun($1)));
                                               //mcrl2::data::basic_sort(mcrl2::core::identifier_string($1)))));
        fsm_lexer_obj->fsm_lts->add_process_parameter(fsm_lexer_obj->typeId,$1);
        // NODIG?? fsm_lexer_obj->typeId = ATmakeAppl2(fsm_lexer_obj->const_ATtype,(ATerm) fsm_lexer_obj->typeId,(ATerm) $1);
      }
    }
  type_values
    {
      if (!ignore_par[num_pars])
      {
        // fsm_lexer_obj->typeValues = ATreverse( fsm_lexer_obj->typeValues );
        // fsm_lexer_obj->valueTable = ATinsert( fsm_lexer_obj->valueTable,
        //     (ATerm)fsm_lexer_obj->typeValues );
      }
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
      // std::string result = static_cast<std::string> ( ATwriteToString( (ATerm)$1 ) )
      //   + "->" + static_cast<std::string> ( ATwriteToString( (ATerm)$3 ) );
      // safe_assign($$, ATmakeAppl0( ATmakeAFun( result.c_str(), 0, ATfalse ) ))
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
      // std::string result = static_cast<std::string> ( ATwriteToString( (ATerm)$1 ) )
      //   + "#" + static_cast<std::string> ( ATwriteToString( (ATerm)$3 ) );
      // safe_assign($$, ATmakeAppl0( ATmakeAFun( result.c_str(), 0, ATfalse ) ));
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
      // std::string result = "struct " + static_cast<std::string> ( ATwriteToString( (ATerm)$2 ) );
      // safe_assign($$, ATmakeAppl0( ATmakeAFun( result.c_str(), 0, ATfalse ) ));
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
      // std::string result = static_cast<std::string> ( ATwriteToString( (ATerm)$1 ) )
      //  + "|" + static_cast<std::string> ( ATwriteToString( (ATerm)$3 ) );
      // safe_assign($$, ATmakeAppl0( ATmakeAFun( result.c_str(), 0, ATfalse ) ));
      $$ = $1 + "|" + $3;
    }
  ;

//structured sort constructor
struct_constructor:
  ID recogniser
    {
      // std::string result = static_cast<std::string> ( ATwriteToString( (ATerm)$1 ) )
      //  + " " + static_cast<std::string> ( ATwriteToString( (ATerm)$2 ) );
      // safe_assign($$, ATmakeAppl0( ATmakeAFun( result.c_str(), 0, ATfalse ) ));
      $$ = $1 + " " + $2;   
    }
  | ID LPAR struct_projections_cs RPAR recogniser
    {
      // std::string result = static_cast<std::string> ( ATwriteToString( (ATerm)$1 ) )
      //  + "(" + static_cast<std::string> ( ATwriteToString( (ATerm)$3 ) ) + ")" +
      //        static_cast<std::string> ( ATwriteToString( (ATerm)$5 ) );
      // safe_assign($$, ATmakeAppl0( ATmakeAFun( result.c_str(), 0, ATfalse ) ));
      $$ = $1 + "(" + $3 + ")" + $5;
    }
  ;

//recogniser
recogniser:
  /* empty */
    {
      // std::string result = static_cast<std::string> ( "");
      // safe_assign($$, ATmakeAppl0( ATmakeAFun( result.c_str(), 0, ATfalse ) ));
      $$ = "";
    }
  | QMARK ID
    {
      // std::string result = "?" + static_cast<std::string> ( ATwriteToString( (ATerm)$2 ) );
      // safe_assign($$, ATmakeAppl0( ATmakeAFun( result.c_str(), 0, ATfalse ) ));
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
      // std::string result = static_cast<std::string> ( ATwriteToString( (ATerm)$1 ) )
      //  + "," + static_cast<std::string> ( ATwriteToString( (ATerm)$3 ) );
      // safe_assign($$, ATmakeAppl0( ATmakeAFun( result.c_str(), 0, ATfalse ) ));
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
      // std::string result = static_cast<std::string> ( ATwriteToString( (ATerm)$1 ) )
      //   + ":" + static_cast<std::string> ( ATwriteToString( (ATerm)$3 ) );
      // safe_assign($$, ATmakeAppl0( ATmakeAFun( result.c_str(), 0, ATfalse ) ));
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
      // std::string result = "(" + static_cast<std::string> ( ATwriteToString( (ATerm)$2 ) ) + ")";
      // safe_assign($$, ATmakeAppl0( ATmakeAFun( result.c_str(), 0, ATfalse ) ));
      $$ = "(" + $2 + ")";
    }
  ;

//sort constant
sort_constant:
  BOOL
    {
      // safe_assign($$, ATmakeAppl0( ATmakeAFun( "Bool", 0, ATfalse ) ));
      $$ = "Bool"; 
    }
  | POS
    {
      // safe_assign($$, ATmakeAppl0( ATmakeAFun( "Pos", 0, ATfalse ) ));
      $$ = "Pos";
    }
  | NAT
    {
      // safe_assign($$, ATmakeAppl0( ATmakeAFun( "Nat", 0, ATfalse ) ));
      $$ = "Nat";
    }
  | INT
    {
      // safe_assign($$, ATmakeAppl0( ATmakeAFun( "Int", 0, ATfalse ) ));
      $$ = "Int";
    }
  | REAL
    {
      // safe_assign($$, ATmakeAppl0( ATmakeAFun( "Real", 0, ATfalse ) ));
      $$ = "Real";
    }
  ;

//sort constructor
sort_constructor:
  LIST LPAR sort_expr RPAR
    {
      // std::string result = "List(" + static_cast<std::string> ( ATwriteToString( (ATerm)$3 ) ) + ")";
      // safe_assign($$, ATmakeAppl0( ATmakeAFun( result.c_str(), 0, ATfalse ) ));
      $$ = "List(" + $3 + ")";
    }
  | SET LPAR sort_expr RPAR
    {
      // std::string result = "Set(" + static_cast<std::string> ( ATwriteToString( (ATerm)$3 ) ) + ")";
      // safe_assign($$, ATmakeAppl0( ATmakeAFun( result.c_str(), 0, ATfalse ) ));
      $$ = "Set(" + $3 + ")";
    }
  | BAG LPAR sort_expr RPAR
    {
      // std::string result = "Bag(" + static_cast<std::string> ( ATwriteToString( (ATerm)$3 ) ) + ")";
      // safe_assign($$, ATmakeAppl0( ATmakeAFun( result.c_str(), 0, ATfalse ) ));
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
        // fsm_lexer_obj->typeValues = ATinsert( fsm_lexer_obj->typeValues,
        //     (ATerm)ATmakeAppl2(fsm_lexer_obj->const_ATvalue, (ATerm)$1,
        //    (ATerm)fsm_lexer_obj->typeId ) );
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
      // fsm_lexer_obj->stateVector = ATreverse( fsm_lexer_obj->stateVector );
      unsigned int i = fsm_lexer_obj->fsm_lts->add_state(fsm_lexer_obj->stateVector);
                    // mcrl2::lts::detail::state_label(fsm_lexer_obj->stateVecto))_
      if ( i == 0 )
      {
        fsm_lexer_obj->fsm_lts->set_initial_state( i );
      }
      // fsm_lexer_obj->stateVector = ATempty
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
        // if ( par_index < ATgetLength( fsm_lexer_obj->valueTable ) )
        // {
          // fsm_lexer_obj->stateVector = ATinsert( fsm_lexer_obj->stateVector,
          //     ATelementAt( (ATermList)ATelementAt( fsm_lexer_obj->valueTable,
          //         par_index ), $2 ) );
          fsm_lexer_obj->stateVector.push_back(atoi($2.c_str()));
        // }
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
      unsigned int frState = atoi($1.c_str())-1;
      unsigned int toState = atoi($2.c_str())-1;

      std::map < std::string, unsigned int>::const_iterator label_index=fsm_lexer_obj->labelTable.find($3);
      if (label_index==fsm_lexer_obj->labelTable.end())
      { // Not found. This label does not occur in the fsm.
        const lts_fsm_t::labels_size_type n=fsm_lexer_obj->fsm_lts->add_label($3,$3=="tau");
        fsm_lexer_obj->labelTable[$3]=n;
        fsm_lexer_obj->fsm_lts->add_transition(transition(frState,n,toState));
      }
      else
      { // The label is found. It already exists.
        fsm_lexer_obj->fsm_lts->add_transition(transition(frState,label_index->second,toState));
      }

      /* ATerm label = ATtableGet(fsm_lexer_obj->labelTable,(ATerm)$3);
      if ( label == NULL )
      {
        const std::string s(ATgetName(ATgetAFun($3)));
        unsigned int i = fsm_lexer_obj->fsm_lts->add_action(s, s=="tau");
        label = (ATerm) ATmakeInt(i);
        ATtablePut(fsm_lexer_obj->labelTable,(ATerm)$3,label);
      }
      fsm_lexer_obj->fsm_lts->add_transition(mcrl2::lts::transition(frState,
          ATgetInt((ATermInt)label), toState )); 
      */
    }
  ;

action :
  /* empty */
    { $$ = ""; 
      // safe_assign($$, ATmakeAppl0( ATmakeAFun( "", 0, ATfalse ) )) 
    }
  |
  QUOTED
    { 
      $$ = $1;
    }
  ;

%%

char* intToCString( int i )
{
    std::ostringstream oss;
    oss << i;
    return (char*)oss.str().c_str();
}
