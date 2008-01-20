%{
#include "fsmparser.h"
#include "fsmlexer.cpp"
#include <map>
#include <ostream>
#include <sstream>
#include "lts.h"

// Global variables
LTS* fsmparserlts = NULL;            /* Points to LTS structure */
int par_index;                       /* Current parameter index */
int num_pars;                        /* Number of specified parameters */
std::string par_name;                /* Name of current parameter */
std::string par_type;                /* Type of current parameter */ 
std::vector<std::string> par_values; /* Value domain of current parameter */
std::vector<bool> ignore_par;        /* Records which parameters are ignored */
int state_id;                        /* Current state index */
std::vector<int> state_vector;       /* State vector of current state */
std::map<std::string,int> labels;    /* Records index for every label string */
char *string_buffer = NULL;          /* Used in the lexer for storing IDs */
unsigned int string_buffer_size = 0; /* Used in the lexer for storing IDs */

// Function declarations
void fsmerror(const char* c);
std::string unquote(char *str);
%}

%start fsm_file

%union {
	char *str;
	int num;
}

%token EOLN SECSEP LPAR RPAR ARROW
%token <str> ID QUOTED
%token <num> NUMBER
%type <str> type_name type_name1 action

%%

fsm_file : 
    { num_pars = 0;
      ignore_par.clear();
    }
	params
	SECSEP EOLN
    { state_id = 0; }
	states
	SECSEP EOLN transitions
	;

// --------- Section containing the state vector ----------

params : 
	/* empty */
	|
	params
    { par_values.clear(); }
  param 
    { if (!ignore_par[num_pars]) {
        fsmparserlts->addParameter(par_name,par_type,par_values);
      }
      ++num_pars;
    }
  EOLN
	;

param :
	ID { par_name = static_cast<std::string>($1) }
	cardinality type_def
	;

cardinality :
	LPAR NUMBER RPAR
    { ignore_par.push_back($2 == 0);
      par_values.reserve($2);
    }
	;
	
type_def : 
	type_name
    { par_type = static_cast<std::string>($1); }
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
    { par_values.push_back(unquote($1)); }
	;

// ----------- Section containing the states ---------

states :
	/* empty */
	|
	states
    { par_index = 0; state_vector.clear(); }
	state
    { fsmparserlts->addState(state_id,state_vector);
      ++state_id; }
	EOLN
	;

state :
	/* empty */
	|
	state NUMBER
	  { 
      if (!ignore_par[par_index]) {
        if (par_index >= fsmparserlts->getNumParameters()) {
          fsmerror("too many state parameter values");
        }
        if ($2 < 0 || $2 >= fsmparserlts->getNumParameterValues(par_index)) {
          fsmerror("state parameter value out of bounds");
        }
        state_vector.push_back($2);
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
			std::string labstr = unquote($3);
			std::map< std::string,int >::iterator p = labels.find(labstr);
			int l;
			if (p == labels.end()) {
				l = fsmparserlts->addLabel(labstr);
				labels[labstr] = l;
			} else {
				l = p->second;
			}
      // State ids in the FSM file are 1-based, but in our administration
      // they are 0-based!
	    fsmparserlts->addTransition($1-1,$2-1,l);
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
    
		labels.clear();
    fsmparserlts = NULL;
    free(string_buffer);
    string_buffer = NULL;
    string_buffer_size = 0;
  }
}

std::string unquote(char *str) {
  std::string result(str);
	return result.substr(1,result.length()-2);
}
