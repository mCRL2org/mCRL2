%{
#include "fsmparser.h"
#include "fsmlexer.cpp"
#include <map>
#include <ostream>
#include <sstream>
#include "lts.h"
#include "state.h"
#include "transition.h"

// Global variables
LTS* fsmparserlts = NULL;
std::map< int,State* > states;
std::map< std::string,int > labels;
std::string par_name;
int par_index, nodenr_index, fanin_index, fanout_index, N;
State *state = NULL;
char *string_buffer = NULL;
unsigned int string_buffer_size = 0;

// Function declarations
void fsmerror(const char* c);
std::string unquote(char *str);
%}

%start fsm_file

%union {
	char *str;
	int num;
}

%token EOLN SECSEP LPAR RPAR FANIN FANOUT NODENR ARROW
%token <str> ID QUOTED
%token <num> NUMBER
%type <str> type_name type_name1 action

%%

fsm_file : 
	{ N = 0; nodenr_index = 0; fanin_index = 0; fanout_index = 0 }
	params
	SECSEP EOLN
	states
	SECSEP EOLN transitions
	;

// --------- Section containing the state vector ----------

params : 
	/* empty */
	|
	params { N++ } param EOLN
	;

param :
	ID { par_name = static_cast<std::string>($1) }
	cardinality type_def
	|
	FANIN { fanin_index = N-1 } cardinality type_def_ignore
	|
	FANOUT { fanout_index = N-1 } cardinality type_def_ignore
	|
	NODENR { nodenr_index = N-1 } cardinality type_def_ignore
	;

cardinality :
	LPAR NUMBER RPAR
	;
	
type_def : 
	type_name { par_index = fsmparserlts->addParameter(par_name,static_cast<std::string>($1)) }
	type_values
	;

type_name :
	/* empty */
	  { $$ = "" }
	|
	type_name1
	  { $$ = $1 }
	| 
	type_name ARROW type_name1
	  {
	    std::string result = static_cast<std::string>($1) + "->" + static_cast<std::string>($3);
	    $$ = strdup(result.c_str())
	  }
	;

type_name1 :
	ID
	  { $$ = $1 }
	|
	LPAR type_name RPAR
	  {
	    std::string result = "(" + static_cast<std::string> ($2) + ")";
	    $$ = strdup(result.c_str())
	  }
	;


type_values :
	/* empty */
	|
	type_values type_value
	;

type_value :
	QUOTED
	  { fsmparserlts->addParameterValue(par_index,unquote($1)) }
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
		{ state = new State(); fsmparserlts->addState(state); N = 0 }
	state
	  {
	    //states.push_back( s );
	    //if ( states.size() == 1 ) 
	    //  fsmparserlts->setInitialState( s );
	    //stateVector = ATempty
	  }
	EOLN
	;

state :
	/* empty */
	|
	state NUMBER
	  { 
			if (N != fanin_index && N != fanout_index) {
				if (N == nodenr_index) {
					state->setID($2);
					states[$2] = state;
					if ($2 == 1) {
						fsmparserlts->setInitialState(state);
					}
				} else {
					state->addParameterValue($2);
				}
			}
			N++
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
	    State* s1 = states[$1];
	    State* s2 = states[$2];
			std::string labstr = unquote($3);
			std::map< std::string,int >::iterator p = labels.find(labstr);
			int l;
			if (p == labels.end()) {
				l = fsmparserlts->addLabel(labstr);
				labels[labstr] = l;
			} else {
				l = p->second;
			}
	    Transition* t = new Transition(s1,s2,l);
	    fsmparserlts->addTransition(t);
	    if ($1 != $2) {
	      s1->addOutTransition(t);
	      s2->addInTransition(t);
	    } else {
	      s1->addLoop(t);
	    }
	  }
	;

action :
	/* empty */
	  { $$ = "" }
	|
	QUOTED
	  { $$ = $1 }
	;

%%

int fsmwrap() {
  return 1;
}

void fsmerror(const char *str) {
	std::ostringstream oss;
	oss << "Parse error: " << str << " token \"" << fsmtext << "\" at line " <<
      lineNo << " position " << posNo;
	throw oss.str();
}
 
void parseFSMfile( std::string fileName, LTS* const lts ) {
  FILE* infile = fopen(fileName.c_str(),"r");
  if (infile == NULL) {
		throw std::string("Cannot open file for reading:\n" + fileName);
	}
  else {
    // INITIALISE
    // reset the lexer position variables
    lineNo = 1;
    posNo = 1;
    // initialize the string buffer for read string-valued tokens
    string_buffer_size = 128;
    string_buffer = (char*)malloc(string_buffer_size*sizeof(char));
  
    fsmparserlts = lts;
    fsmrestart(infile);

    fsmparse();
    
    states.clear();
		labels.clear();
    fsmparserlts = NULL;
		state = NULL;
    free(string_buffer);
    string_buffer = NULL;
    string_buffer_size = 0;
  }
}

std::string unquote(char *str) {
  std::string result(str);
	return result.substr(1,result.length()-2);
}
