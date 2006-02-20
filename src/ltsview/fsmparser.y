%{
#include "fsmparser.h"
#include "fsmlexer.cpp"

// Global variables

LTS* fsmparserlts = NULL;

ATermList stateVector;
ATermList valueTable;
ATermList stateId;
ATermList typeValues;
ATermAppl typeId;

ATermIndexedSet actions;
vector< State* > states;

AFun const_ATtypeid;
AFun const_ATparmid;
AFun const_ATvalue;
AFun const_ATstate;
AFun const_ATparam;

// Function declarations

void fsmerror(const char* c);
char* intToCString(int i);
%}

%union {
  ATermAppl aterm;
  int number;
}

%start fsm_file

%token EOLN SECSEP QUOTED LBRACK RBRACK FANIN FANOUT NODENR COMMA
%token <number> NUMBER
%token <aterm> ID
%token <aterm> QUOTED
%type  <aterm> type_name action

%%

fsm_file : 
	params
	  { 
	    valueTable = ATreverse( valueTable );
	    stateId = ATreverse( stateId );
	    fsmparserlts->setStateVectorSpec( stateId )
	  }
	SECSEP EOLN
	states
	SECSEP EOLN transitions
	  { fsmparserlts->addTransitionLabels( ATindexedSetElements( actions ) ) }
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
	  {
	    stateId = ATinsert( stateId, (ATerm)ATmakeAppl2( const_ATparmid,
	      (ATerm)$1, (ATerm)typeId ) )
	  }
	|
	FANIN cardinality type_name type_values
	|
	FANOUT cardinality type_name type_values
	|
	NODENR cardinality type_name type_values
	;

cardinality :
	/* empty */
	|
	LBRACK NUMBER RBRACK
	;
	
type_def : 
	type_name
	  { 
	    typeValues = ATempty
	  }
	type_values
	  { 
	    typeValues = ATreverse( typeValues );
	    typeId = ATmakeAppl2( const_ATtypeid, (ATerm)$1, (ATerm)typeValues );
	    valueTable = ATinsert( valueTable, (ATerm)typeValues )
	  }
	;

type_name :
	/* empty */
	  { $$ = ATmakeAppl0( ATmakeAFun( "", 0, ATfalse ) ) }
	|
	ID
	  { $$ = $1 }
	;

type_values :
	/* empty */
	|
	type_values type_value
	;

type_value :
	QUOTED
	  { typeValues = ATinsert( typeValues, (ATerm)ATmakeAppl2(
	      const_ATvalue, (ATerm)$1, (ATerm)ATmakeInt( ATgetLength( typeValues ) ) ) ) }
	;

// ----------- Section containing the states ---------

states :
	/* empty */
	|
	states
	state
	  {
	    stateVector = ATreverse( stateVector ); 
	    State* s = new State( stateVector );
	    fsmparserlts->addState( s );
	    states.push_back( s );
	    if ( states.size() == 1 ) 
	      fsmparserlts->setInitialState( s );
	    stateVector = ATempty
	  }
	EOLN
	;

state :
	/* empty */
	|
	state NUMBER
	  { 
	    int paramNo = ATgetLength( stateVector );
	    if ( paramNo < ATgetLength( valueTable ) )
	    {
	      stateVector = ATinsert( stateVector, ATelementAt(
		(ATermList)ATelementAt( valueTable, paramNo ), $2 ) );
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
	    State* frState = states[$1-1];
	    State* toState = states[$2-1];
	    Transition* t = new Transition( frState, toState, (ATerm)$3 );
	    ATbool b;
	    ATindexedSetPut( actions, (ATerm)$3, &b );
	    fsmparserlts->addTransition( t );
	    if ( $1 != $2 )
	    {
	      frState->addOutTransition( t );
	      toState->addInTransition( t );
	    }
	    else
	    {
	      frState->addLoop( t );
	    }
	  }
	;

action :
	/* empty */
	  { $$ = ATmakeAppl0( ATmakeAFun( "", 0, ATfalse ) ) }
	|
	QUOTED
	  { $$ = $1 }
	;

%%

int fsmwrap()
{
  return 1;
}

void fsmerror(const char *str)
{
  throw "error: " + static_cast<string>(str) + " token \"" +
  static_cast<string>(fsmtext) + "\" at line " +
    static_cast<string>(intToCString( lineNo )) + " position " +
    static_cast<string>(intToCString( posNo ));
}
 
void parseFSMfile( string fileName, LTS* const lts )
{
  FILE* infile = fopen(fileName.c_str(),"r");
  
  // reset the lexer position variables
  lineNo=1;
  posNo=1;
  
  if ( infile == NULL )
    throw "Cannot open file " + fileName + " for reading";
  else
  {
    // INITIALISE
    fsmparserlts = lts;
    fsmrestart( infile );
    
    ATprotectAFun( const_ATtypeid );
    ATprotectAFun( const_ATparmid );
    ATprotectAFun( const_ATvalue );
    ATprotectAFun( const_ATstate );
    ATprotectAFun( const_ATparam );
    ATprotectList( &stateVector );
    ATprotectList( &valueTable );
    ATprotectList( &stateId );
    ATprotectList( &typeValues );
    ATprotectAppl( &typeId );
    
    const_ATtypeid = ATmakeAFun( "TypeId", 2, ATfalse );
    const_ATparmid = ATmakeAFun( "ParamId", 2, ATfalse );
    const_ATvalue = ATmakeAFun( "Value", 2, ATfalse );
    const_ATstate = ATmakeAFun( "State", 2, ATfalse );
    const_ATparam = ATmakeAFun( "Param", 2, ATfalse );
    stateVector = ATempty;
    stateId = ATempty;
    valueTable = ATempty;
    actions = ATindexedSetCreate( 100, 50 );

    // PARSE
    fsmparse();
    
    // CLEAN UP
    ATunprotectAFun( const_ATtypeid );
    ATunprotectAFun( const_ATparmid );
    ATunprotectAFun( const_ATvalue );
    ATunprotectAFun( const_ATstate );
    ATunprotectAFun( const_ATparam );
    ATunprotectList( &stateVector );
    ATunprotectList( &valueTable );
    ATunprotectList( &stateId );
    ATunprotectList( &typeValues );
    ATunprotectAppl( &typeId );
    
    ATindexedSetDestroy( actions );
    states.clear();
    fsmparserlts = NULL;
  }
} 

char* intToCString( int i )
{
    ostringstream oss;
    oss << i;
    return (char*)oss.str().c_str();
}
