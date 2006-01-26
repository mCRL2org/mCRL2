%{
#include "fsmparser.h"
#include "fsmlexer.c"

// Global variables


LTS* fsmparserlts = NULL;

map< string, set< string > > dataTypes;
vector< string > paramNames;
vector< string > paramTypes;
vector< ATermAppl > paramsAT;

vector< ATermAppl > stateVector;
vector< string > values;
vector< vector< string > > valueTable;

string fullAction;
map< string, Action* > actions;
int paramNo, nodeId;
map< int, State* > states;
ATermList stateId;

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
	  {
	    map< string, ATermAppl > dataTypesAT;
	    AFun typeId = ATmakeAFun( "TypeId", 2, ATfalse );
	    for ( map< string, set< string > >::iterator it = dataTypes.begin() ;
		  it != dataTypes.end() ; ++it )
	    {
	      ATermList valuesAT = ATempty;
	      for ( set< string >::reverse_iterator it1 = it->second.rbegin() ;
		it1 != it->second.rend() ; ++it1 )
	      {
		valuesAT = ATinsert(valuesAT, ATmake( "<appl>", it1->c_str() ));
	      }
	      dataTypesAT[ it->first ] = ATmakeAppl2( typeId, ATmake( "<appl>",
	        it->first.c_str() ), (ATerm)valuesAT );
	    }
	    dataTypes.clear();

	    AFun paramId = ATmakeAFun( "ParamId", 2, ATfalse );
	    for ( int i = 0 ; i < paramNames.size() ; ++i )
	    {
	      paramsAT.push_back( ATmakeAppl2( paramId, ATmake( "<appl>",
		paramNames[i].c_str() ), (ATerm)dataTypesAT[ paramTypes[i] ] ) );
	    }
	    paramNames.clear();
	    paramTypes.clear();
	    dataTypesAT.clear();

	    stateId = ATempty;
	    for ( int i = paramsAT.size()-1 ; i >= 0 ; --i )
	    {
	      stateId = ATinsert( stateId, (ATerm)paramsAT[i] );
	    }
	    fsmparserlts->setStateVectorSpec( stateId );
	  }
	SECSEP EOLN
	  { nodeId = 1; }
	states
	  { paramsAT.clear() }
	SECSEP EOLN transitions
	;

// --------- Section containing the state vector ----------

params : 
	/* empty */
	|
	params param EOLN
	;

param :
	param_name cardinality type_def
	;

param_name :
	ID
	  { paramNames.push_back( static_cast<string>($1) ) }
	|
	FANIN
	|
	FANOUT
	|
	NODENR
	;

cardinality :
	/* empty */
	|
	LBRACK NUMBER RBRACK
	;
	
  // Note: it is important that the type name is added to the paramTypes vector
  // BEFORE the type values are parsed! See the type_value entry below.
type_def : 
	type_name
	  { paramTypes.push_back( static_cast<string>($1) ) }
	type_values
	  { 
	    if ( values.size() == 0 )
	    {
	      dataTypes[ paramTypes.back() ].insert( "" );
	      values.push_back( "" );
	    }
	    valueTable.push_back( values );
	    values.clear();
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
	    dataTypes[ paramTypes.back() ].insert( value );
	    values.push_back( value )
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
	  { paramNo = 0 }
	state
	  {
	    ATermList stateVectorAT = ATempty;
	    for ( int i = stateVector.size()-1 ; i >= 0 ; --i )
	    {
	      stateVectorAT = ATinsert( stateVectorAT, (ATerm)stateVector[i] );
	    }
	       
	    AFun state = ATmakeAFun( "State", 2, ATfalse );
	    State* s = new State( ATmakeAppl2( state, (ATerm)stateId, (ATerm)stateVectorAT ) );
	    fsmparserlts->addState( s );
	    states[nodeId] = s;
	    if ( nodeId == 1 ) 
	      fsmparserlts->setInitialState( s );
	    ++nodeId;
	    stateVector.clear()
	  }
	EOLN
	;

state :
	/* empty */
	|
	state NUMBER
	  { 
	    if ( paramNo < paramsAT.size() )
	    {
	      // use range checked access of the valueTable vector of vectors
	      // catch any out-of-range errors
	      string value = "";
	      try
	      {
		value = valueTable.at(paramNo).at($2); 
	      }
	      catch ( out_of_range )
	      {
	      }
	      AFun param = ATmakeAFun( "Param", 2, ATfalse );
	      stateVector.push_back( ATmakeAppl2( param, (ATerm)paramsAT[ paramNo ],
		ATmake( "<appl>", value.c_str() ) ) );
	    }
	    ++paramNo
	  }
	;

// ---------- Section containing the transitions ----------

transitions:
	/* empty */
	|
	transitions transition
//	  { values.clear() }
	EOLN
	;

transition:
	NUMBER NUMBER action
	  {
	    if ( actions.find(fullAction) == actions.end() )
	    {
	      actions[fullAction] = new Action( static_cast<string>($3), values );
	      fsmparserlts->addAction( actions[fullAction] );
	    }
	    State* frState = states[$1];
	    State* toState = states[$2];
	    Transition* t = new Transition( frState, toState, actions[fullAction] );
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

	    fullAction = ""
	  }
	;

action :
	/* empty */
	  { $$ = "" }
	|
	QUOTED
	  {
	    string value = static_cast<string>($1);
	    $$ = strdup( value.substr( 1, value.length() - 2 ).c_str() );
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
    fsmparserlts = lts;
    fsmrestart( infile );
    
    fsmparse();
    
    valueTable.clear();
    states.clear();
    actions.clear();
    fsmparserlts = NULL;
    stateId = ATempty;
  }
} 

char* intToCString( int i )
{
    ostringstream oss;
    oss << i;
    return (char*)oss.str().c_str();
}
