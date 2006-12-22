%{
#include "lts/liblts.h"
#include "fsmparser.h"
#include "fsmlexer.h"

// Function declarations

//external declarations from mcrl2lexer.l
void fsmyyerror(const char *s);
int fsmyylex(void);

char* intToCString(int i);

#define safe_assign(lhs, rhs) { ATbool b; ATindexedSetPut(fsm_lexer_obj->protect_table, (ATerm) rhs, &b); lhs = rhs; }
%}

%union {
  ATermAppl aterm;
  int number;
}

//set name prefix
%name-prefix="fsmyy"

%start fsm_file

%token EOLN SECSEP LPAR RPAR FANIN FANOUT NODENR ARROW
%token <number> NUMBER
%token <aterm> ID QUOTED
%type  <aterm> type_name type_name1 action

%%

fsm_file : 
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
	params param EOLN
	;

param :
	ID
	cardinality type_def
	|
	FANIN cardinality type_def_ignore
	|
	FANOUT cardinality type_def_ignore
	|
	NODENR cardinality type_def_ignore
	;

cardinality :
	LPAR NUMBER RPAR
	;
	
type_def : 
	type_name
	  { 
	    fsm_lexer_obj->typeValues = ATempty;
	    fsm_lexer_obj->typeId = $1;
	    
	  }
	type_values
	  { 
	    fsm_lexer_obj->typeValues = ATreverse( fsm_lexer_obj->typeValues );
	    fsm_lexer_obj->valueTable = ATinsert( fsm_lexer_obj->valueTable, (ATerm)fsm_lexer_obj->typeValues )
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
	  { fsm_lexer_obj->typeValues = ATinsert( fsm_lexer_obj->typeValues, (ATerm)ATmakeAppl2(
	      fsm_lexer_obj->const_ATvalue, (ATerm)$1, (ATerm)fsm_lexer_obj->typeId ) ) }
	;

type_def_ignore : 
	type_name_ignore
	type_values_ignore
	;

type_name_ignore :
	/* empty */
	| type_name_ignore1
	| type_name_ignore ARROW type_name_ignore1
	;

type_name_ignore1 :
	ID
	| LPAR type_name_ignore RPAR
	;

type_values_ignore :
	/* empty */
	| type_values_ignore type_value_ignore
	;

type_value_ignore :
	QUOTED
	;

// ----------- Section containing the states ---------

states :
	/* empty */
	|
	states
	state
	  {
	    fsm_lexer_obj->stateVector = ATreverse( fsm_lexer_obj->stateVector ); 
	    unsigned int i = fsm_lexer_obj->fsm_lts->add_state( (ATerm) fsm_lexer_obj->stateVector );
	    if ( i == 0 ) 
	      fsm_lexer_obj->fsm_lts->set_initial_state( i );
	    fsm_lexer_obj->stateVector = ATempty
	  }
	EOLN
	;

state :
	/* empty */
	|
	state NUMBER
	  { 
	    unsigned int paramNo = ATgetLength( fsm_lexer_obj->stateVector );
	    if ( paramNo < ATgetLength( fsm_lexer_obj->valueTable ) )
	    {
	      fsm_lexer_obj->stateVector = ATinsert( fsm_lexer_obj->stateVector, ATelementAt(
		(ATermList)ATelementAt( fsm_lexer_obj->valueTable, paramNo ), $2 ) );
	    }
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
	    	unsigned int i = fsm_lexer_obj->fsm_lts->add_label((ATerm)$3);
		label = (ATerm) ATmakeInt(i);
		ATtablePut(fsm_lexer_obj->labelTable,(ATerm)$3,label);
	    }
	    fsm_lexer_obj->fsm_lts->add_transition( frState, ATgetInt((ATermInt)label), toState );
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
