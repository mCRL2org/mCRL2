%{
#include <stdio.h>
#include <string>
#include "lts.h"
%}

/* Require bison 2.3 or later */
%require "2.3"

%start fsm_file

/* write out a header file containing the token defines */
%defines

/* use newer C++ skeleton file */
%skeleton "lalr1.cc"

/* namespace to enclose parser in */
%name-prefix="ltsview"

/* set the parser's class identifier */
%define "parser_class_name" "LTSViewFSMParser"

/* keep track of the current position within the input */
%locations
%initial-action {
    // initialize the initial location object
    @$.begin.filename = @$.end.filename = &fileloader.filename;
};

/* The fileloader is passed by reference to the parser and to the
 * scanner. This provides a simple but effective pure interface, not
 * relying on global variables. */
%parse-param { class FileLoader& fileloader }

%union {
  std::string* stringVal;
  int integerVal;
}

%token EOFL            0   "end of file" 
%token EOLN                "end of line" 
%token SECSEP              "section separator"
%token LPAR                "opening parenthesis"
%token RPAR                "closing parenthesis"
%token ARROW               "arrow symbol"
%token <stringVal> ID      "string"
%token <stringVal> QUOTED  "quoted string"
%token <integerVal> NUMBER "number"

%type <stringVal> type_name type_name1 action

%destructor { delete $$; } ID QUOTED
%destructor { delete $$; } type_name type_name1 action

%{
#include "fileloader.h"
#include "fsmlexer.h"

/* this "connects" the bison parser in the fileloader to the flex
 * scanner class object. it defines the yylex() function call to pull
 * the next token from the current lexer object of the fileloader
 * context.  */
#undef yylex
#define yylex fileloader.lexer->lex

%}

%%

fsm_file : 
    { fileloader.num_pars = 0;
      fileloader.ignore_par.clear();
    }
  params
  SECSEP EOLN
    { fileloader.state_id = 0; }
  states
  SECSEP EOLN transitions
  ;

// --------- Section containing the state vector ----------

params : 
  /* empty */
  |
  params
    { fileloader.par_values.clear(); }
  param 
    { if (!fileloader.ignore_par[fileloader.num_pars]) {
        fileloader.lts->addParameter(fileloader.par_name,
            fileloader.par_type,fileloader.par_values);
      }
      ++fileloader.num_pars;
    }
  EOLN
  ;

param :
  ID { fileloader.par_name = *$1 }
  cardinality type_def
  ;

cardinality :
  LPAR NUMBER RPAR
    { fileloader.ignore_par.push_back($2 == 0);
      fileloader.par_values.reserve($2);
    }
  ;
  
type_def : 
  type_name
    { fileloader.par_type = *$1; }
  type_values
  ;

type_name :
  /* empty */
    { $$ = new std::string("") }
  |
  type_name1
    { $$ = $1 }
  | 
  type_name ARROW type_name1
    {
      $$ = new std::string(*$1 + "->" + *$3);
    }
  ;

type_name1 :
  ID
    { $$ = $1 }
  |
  LPAR type_name RPAR
    {
      $$ = new std::string("(" + *$2 + ")");
    }
  ;


type_values :
  /* empty */
  |
  type_values type_value
  ;

type_value :
  QUOTED
    { fileloader.par_values.push_back(*$1); }
  ;

// ----------- Section containing the states ---------

states :
  /* empty */
  |
  states
    { 
      fileloader.par_index = 0; 
      fileloader.state_vector.clear();
    }
  state
    { 
      fileloader.lts->addState(fileloader.state_id,fileloader.state_vector);
      ++fileloader.state_id;
    }
  EOLN
  ;

state :
  /* empty */
  |
  state NUMBER
    { 
      if (fileloader.par_index >= fileloader.ignore_par.size())
      {
        fileloader.error("too many state parameter values");
      }
      if (!fileloader.ignore_par[fileloader.par_index])
      {
        if (fileloader.state_vector.size() >= fileloader.lts->getNumParameters())
        {
          fileloader.error("too many state parameter values");
        }
        if ($2 < 0 || $2 >= fileloader.lts->getNumParameterValues(
                fileloader.state_vector.size()))
        {
          fileloader.error("state parameter value out of bounds");
        }
        fileloader.state_vector.push_back($2);
      }
      ++fileloader.par_index;
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
      std::map< std::string,int >::iterator p = fileloader.labels.find(*$3);
      int l;
      if (p == fileloader.labels.end())
      {
        l = fileloader.lts->addLabel(*$3);
        fileloader.labels[*$3] = l;
      }
      else
      {
        l = p->second;
      }
      // State ids in the FSM file are 1-based, but in our administration
      // they are 0-based!
      fileloader.lts->addTransition($1-1,$2-1,l);
    }
  ;

action :
  /* empty */
    { $$ = new std::string("") }
  |
  QUOTED
    { $$ = $1 }
  ;

%%

void ltsview::LTSViewFSMParser::error(const LTSViewFSMParser::location_type &l, 
                               const std::string &m)
{
  fileloader.error(l,m);
}
