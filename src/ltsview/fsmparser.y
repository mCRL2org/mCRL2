%{
#include "fsmparser.h"
#include "fsmlexer.c"

// Global variables


LTS* fsmparserlts = NULL;

//ATermList parnameList;
//ATermList partypeList;

ATermList stateVector;
ATermList valueTable;
ATermList stateId;
ATermList typeValues;
ATermAppl typeId;
//int nofValues;
//vector< int > offset;

ATermIndexedSet actions;
vector< State* > states;

AFun const_ATtypeid;
AFun const_ATparmid;
AFun const_ATstate;
AFun const_ATparam;
// Function declarations

void fsmerror(const char* c);
char* intToCString(int i);
%}

%union {
  char* str;
  int number;
}

%start fsm_file

%token EOLN SECSEP QUOTE QUOTED LBRACK RBRACK FANIN FANOUT NODENR COMMA
%token <number> NUMBER
%token <str> ID
%token <str> QUOTED
%type  <str> type_name action
//%type  <str> data_expr data_params action_expr

%%

fsm_file : 
	params
	  { /*
	    valueTable = ATreverse( valueTable );
	    parnameList = ATreverse( parnameList );
	    partypeList = ATreverse( partypeList );
	    
	    int nofParams = ATgetLength( partypeList );
	    ATermTable datatypes = ATtableCreate( 2*nofParams, 50 );
	    
	    ATermList rem_partypeList = partypeList;
	    ATerm type;
	    ATermList values;
	    int i = 0;
	    
	    while( !ATisEmpty( rem_partypeList ) )
	    {
	      type = ATgetFirst( rem_partypeList );
	      values = ATgetSlice( valueTable, offset[i], offset[i+1] );
	      ATermList tableValues = (ATermList)ATtableGet( datatypes, type );
	      
	      if ( tableValues == NULL )
	      {
		ATtablePut( datatypes, type, (ATerm)values );
	      }
	      else
	      {
		ATerm value;
		while ( !ATisEmpty( values ) )
		{
		  value = ATgetFirst( values );
		  if ( ATindexOf( tableValues, value, 0 ) == -1 )
		  {
		    tableValues = ATinsert( tableValues, value );
		  }
		  values = ATgetNext( values );
		}
		ATtablePut( datatypes, type, (ATerm)tableValues );
	      }
	      rem_partypeList = ATgetNext( rem_partypeList );
	      ++i;
	    }

	    ATermList stateId = ATempty;
	    AFun const_typeId = ATmakeAFun( "TypeId", 2, ATfalse );
	    AFun const_parmId = ATmakeAFun( "ParamId", 2, ATfalse );
	    
	    cout << ATgetLength( parnameList ) << endl
	      << ATgetLength( partypeList ) << endl;
	    ATfprintf( stderr, "Keys: %t\n", ATtableKeys( datatypes ) );
	    ATfprintf( stderr, "Values: %t\n", ATtableValues( datatypes ) );
	    int j = 0;
	    while ( !ATisEmpty( parnameList ) )
	    {
	      cout << j << endl;
	      ATerm valuelist = ATtableGet( datatypes, ATgetFirst( partypeList ) );
	      assert( valuelist != NULL );
	      stateId = ATinsert( stateId, (ATerm)ATmakeAppl2( const_parmId,
		ATgetFirst( parnameList ), (ATerm)ATmakeAppl2( const_typeId,
		ATgetFirst( partypeList ), valuelist ) ) );
	      parnameList = ATgetNext( parnameList );
	      partypeList = ATgetNext( partypeList );
	      j++;
	    }
	    cout << "ok";
	    fsmparserlts->setStateVectorSpec( stateId );
	    */
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
	  // { parnameList = ATinsert( parnameList, ATmake( "<appl>", $1 ) ) }
	cardinality type_def
	  {
	    stateId = ATinsert( stateId, (ATerm)ATmakeAppl2( const_ATparmid,
	      ATmake( "<appl>", $1 ), (ATerm)typeId ) )
	  }
	|
	FANIN cardinality
	|
	FANOUT cardinality
	|
	NODENR cardinality
	;

cardinality :
	/* empty */
	|
	LBRACK NUMBER RBRACK
	;
	
type_def : 
	type_name
	  { /* 
	    partypeList = ATinsert( partypeList, ATmake( "<appl>", $1 ) );
	    nofValues = 0;
	    */
	    typeValues = ATempty
	  }
	type_values
	  { 
	    /*offset.push_back( nofValues + offset.back() );
	    nofValues = 0;*/
	    typeValues = ATreverse( typeValues );
	    typeId = ATmakeAppl2( const_ATtypeid, ATmake( "<appl>", $1 ),
	      (ATerm) typeValues );
	    valueTable = ATinsert( valueTable, (ATerm)typeValues )
	  }
	;

type_name :
	/* empty */
	  { $$ = "unspecified" }
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
	  {
	    string value = static_cast<string>($1);
	    value = value.substr( 1, value.length() - 2 );
	    ATerm valueAT = ATmake( "<appl>", value.c_str() );
	    typeValues = ATinsert( typeValues, valueAT )
	    //++nofValues;
	  }
	;
/*
data_expr :
	ID 
	  { $$ = (char*)(static_cast<string> $1).c_str() }
	|
	NUMBER
	  { $$ = intToCString( $1 ) }
	|
	ID LBRACK data_params RBRACK
	  { $$ = (char*)(static_cast<string>($1) + "(" + static_cast<string>($3) + ")" ).c_str() }
	;

data_params :
	data_expr
	  { $$ = $1 }
	|
	data_params COMMA data_expr
	  { $$ = (char*)(static_cast<string>($1) + "," + static_cast<string>($3)).c_str() }
	;
*/	
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
	    ATerm action = ATmake( "<appl>", $3 );
	    Transition* t = new Transition( frState, toState, action );
	    ATbool b;
	    ATindexedSetPut( actions, action, &b );
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
	  { $$ = "" }
	|
	QUOTED
	  {
	    string value = static_cast<string>($1);
	    $$ = strdup( value.substr( 1, value.length() - 2 ).c_str() )
	  }
	;
/*	
action_expr :
	ID
	  { 
	    fullAction += static_cast<string>($1);
	    $$ = $1
	  }
	action_arg
	;

action_arg :
	// empty
	|
	LBRACK 
	{ fullAction += "(" }
	action_params RBRACK 
	{ fullAction += ")" }
	;

action_params :
	data_expr
	  { 
	    values.push_back( static_cast<string>($1) );
	    fullAction += static_cast<string>($1)
	  }
	|
	action_params COMMA data_expr
	  { 
	    values.push_back( static_cast<string>($3) );
	    fullAction += "," + static_cast<string>($3)
	  }
	;
*/
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
    ATprotectAFun( const_ATstate );
    ATprotectAFun( const_ATparam );
    ATprotectList( &stateVector );
    ATprotectList( &valueTable );
    ATprotectList( &stateId );
    ATprotectList( &typeValues );
    ATprotectAppl( &typeId );
    //ATprotectList( &parnameList );
    //ATprotectList( &partypeList );
    
    const_ATtypeid = ATmakeAFun( "TypeId", 2, ATfalse );
    const_ATparmid = ATmakeAFun( "ParamId", 2, ATfalse );
    const_ATstate = ATmakeAFun( "State", 2, ATfalse );
    const_ATparam = ATmakeAFun( "Param", 2, ATfalse );
    stateVector = ATempty;
    stateId = ATempty;
    valueTable = ATempty;
    //parnameList = ATempty;
    //partypeList = ATempty;
    actions = ATindexedSetCreate( 100, 50 );

    //offset.clear();
    //offset.push_back( 0 );
    
    // PARSE
    fsmparse();
    
    // CLEAN UP
    ATunprotectAFun( const_ATtypeid );
    ATunprotectAFun( const_ATparmid );
    ATunprotectAFun( const_ATstate );
    ATunprotectAFun( const_ATparam );
    ATunprotectList( &stateVector );
    ATunprotectList( &valueTable );
    //ATunprotectList( &parnameList );
    //ATunprotectList( &partypeList );
    ATunprotectList( &stateId );
    ATunprotectList( &typeValues );
    ATunprotectAppl( &typeId );
    
    ATindexedSetDestroy( actions );
    //offset.clear();
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
