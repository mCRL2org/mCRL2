// Author(s): Frank Stappers
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file chiparser.yy

%{

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include "libstruct_core.h"
#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/aterm/aterm2.h"
#include "mcrl2/aterm/aterm_ext.h"
#include "mcrl2/utilities/logger.h"
#include <list>
#include <map>
#include <set>
#include <utility>
#include <cctype>
#include <cstring>


/*extern int yyerror(const char *s);
extern int yylex( void );
extern char* yytext; */

using namespace std;
using namespace mcrl2::log;

//external declarations from lexer.ll
void chiyyerror( const char *s );
extern void chigetposition();
int chiyylex( void );
extern ATermAppl chi_spec_tree;
extern ATermIndexedSet chi_parser_protect_table;
extern int scope_lvl;
extern map<ATerm, ATerm> var_type_map;
extern map<ATerm, pair<ATerm,ATerm> > chan_type_direction_map;
extern set<ATermAppl> used_process_identifiers;
//extern bool processing_models;

enum { UNDEFINEDPARSING, CHANNELPARSING, VARIABLEPARSING };
extern int parsing_mode;

#define YYMAXDEPTH 160000

//local declarations
ATermAppl gsSpecEltsToSpec(ATermAppl SpecElts);
//Pre: SpecElts contains one initialisation and zero or more occurrences of
//     sort, constructor, operation, equation, action and process
//     specifications.
//Ret: specification containing one sort, constructor, operation, equation,
//     action and process specification, and one initialisation, in that order.

#define safe_assign(lhs, rhs) { bool b; ATindexedSetPut(chi_parser_protect_table, (ATerm) rhs, &b); lhs = rhs; }

void BinTypeCheck(ATermAppl arg1, ATermAppl arg2, std::string type);
void UnaryTypeCheck(ATermAppl arg1, std::string type);
bool ContainerTypeChecking(ATermAppl arg1, ATermAppl arg2);
bool is_number(std::string s);

%}

%union {
  ATermAppl appl;
  ATermList list;
};

//more verbose and specific error messages
%error-verbose

//set name prefix
%name-prefix="chiyy"

/*
 *  TERMINALS
 *  ---------
 */

%token <appl> PROC MODEL_DEF ENUM MODEL
%token <appl> VAR CONST CHAN VAL
%token <appl> SKIP BARS ALT
%token <appl> COLON TYPE BOOL NAT REAL VOID
%token <appl> ID TIME
%token <appl> BP EP PROC_SEP SEP COMMA IS ASSIGNMENT MINUS PLUS GG
%token <appl> LBRACE RBRACE LBRACKET RBRACKET
%token <appl> AND OR GUARD NOT OLD
%token <appl> NUMBER INT REALNUMBER TRUE FALSE DOT DEADLOCK IMPLIES NOTEQUAL GEQ LEQ MAX MIN DIV MOD POWER
%token <appl> RECV EXCLAMATION SENDRECV RECVSEND SSEND RRECV STAR GUARD_REP DERIVATIVE
%token <appl> SQLBRACKET SQRBRACKET
%token <appl> LSUBTRACT CONCAT IN
%token <appl> HEAD TAIL RHEAD RTAIL LENGTH TAKE DROP SORT INSERT LESS GREATER HASH
%token <appl> UNION SUB INTERSECTION PICK DELAY


%left  AND
%left  OR
%left  IMPLIES

%nonassoc  GREATER LESS IS NOTEQUAL LEQ GEQ
%left  PLUS MINUS
%left  STAR DIVIDE MOD DIV
%right POWER NOT

%left LSUBTRACT CONCAT IN
%left MIN MAX SUB UNION INTERSECTION HEAD TAKE TAIL

%right SEP
%right GUARD
%right ALT
%right BARS
%right ASSIGNMENT
%right SEND RECV

%left GUARD_REP

%start ChiProgram


//generate a GLR parser
%glr-parser

//Uncomment the line below to enable the bison debug facilities.
//To produce traces, yydebug needs to be set to 1 (see chilexer.ll)
//%debug


/*
 *  TERMINALS
 *  ---------
 */

%type <appl> Type BasicType
%type <appl> NatIntExpression BasicExpression BooleanExpression Expression
%type <appl> EqualityExpression Instantiation ModelStatement
%type <appl> LocalVariables Identifier AssignmentStatement CommStatement
%type <appl> ProcessBody OptGuard BasicStatement OptChannel Statement BinaryStatement UnaryStatement
%type <appl> AdvancedStatement IdentifierChannelDefinition  IdentifierChannelDeclaration
%type <appl> ChiProgram ProcessDefinition FormalParameter ExpressionIdentifier
%type <appl> ModelDefinition ModelBody
%type <appl> ContainerType
%type <appl> ListExpression ListLiteral
%type <appl> Functions
%type <appl> SetExpression MemberTest MinusExpression RecordExpression PlusExpression DelayStatement


%type <list> IdentifierTypeExpression IdentifierType Identifier_csp Expression_csp FormalParameter_csp ProcessDefinitions ChannelDeclaration ChannelDefinition
%type <list> IdentifierTypeExpression_csp IdentifierType_csp ExpressionIdentier_csp
%type <list> LocalVariables_csp ChannelDefinition_csp IdentifierChannelDefinition_csp
%type <list> ChannelDeclaration_csp IdentifierChannelDeclaration_csp
%type <list> Type_csp


/*
 *  GRAMMER
 *  -------
 */

%%

ChiProgram:
    ModelDefinition ProcessDefinitions
		{
          mCRL2log(debug) << __FILE__ << ";" << __LINE__ << std::endl;
    	  mCRL2log(debug) << "inputs contains a valid Chi-specification" << std::endl;
          safe_assign($$, gsMakeChiSpec($1,ATreverse($2)));
		  chi_spec_tree = $$;
		}
	;

ModelDefinition:
      MODEL Identifier LBRACKET RBRACKET IS ModelBody
        {
      	  safe_assign($$, gsMakeModelDef($2, $6));
      	  mCRL2log(debug) << "parsed Model Def \n  " << atermpp::aterm( $$) << "" << std::endl;
        }
      ;

ModelBody:
      BP ModelStatement ModelCloseScope
      	{ safe_assign($$, gsMakeModelSpec( ATmakeList0(), $2 ));
      	  mCRL2log(debug) << "parsed Model Body  \n  " << atermpp::aterm( $$) << "" << std::endl;
		}
	| BP LocalVariables_csp PROC_SEP ModelStatement ModelCloseScope
      	{ safe_assign($$, gsMakeModelSpec( $2, $4));
      	  mCRL2log(debug) << "parsed Model Body  \n  " << atermpp::aterm( $$) << "" << std::endl;
		}
	;

ModelCloseScope:
      EP
       {
          var_type_map.clear();
          chan_type_direction_map.clear();
       }

ProcessDefinitions:
       ProcessDefinition
      	{ safe_assign($$, ATmakeList1((ATerm) $1));
      	  mCRL2log(debug) << "parsed Process Definition \n  " << atermpp::aterm( $$) << "" << std::endl;
		}
     | ProcessDefinitions ProcessDefinition
      	{ safe_assign($$, ATinsert($1, (ATerm) $2));
      	  mCRL2log(debug) << "parsed Process Definition \n  " << atermpp::aterm( $$) << "" << std::endl;
		}
     ;

ProcessDefinition:
	  ProcOpenScope Identifier LBRACKET RBRACKET IS ProcessBody
		{
          /**
            * Check if Identifier is not already used
            *
            **/
          if(used_process_identifiers.find($2) != used_process_identifiers.end())
          {
            mCRL2log(error) << "Duplicate definition for process " << atermpp::aterm($2) << std::endl;
            exit(1);
          } else {
            used_process_identifiers.insert($2);
          }

      	  safe_assign($$, gsMakeProcDef($2, gsMakeProcDecl(ATmakeList0()) ,$6));
      	  mCRL2log(debug) << "parsed proc Def \n  " << atermpp::aterm( $$) << "" << std::endl;
		}
	| ProcOpenScope Identifier LBRACKET FormalParameter_csp RBRACKET IS ProcessBody
		{
      	  safe_assign($$, gsMakeProcDef($2, gsMakeProcDecl( ATreverse($4)), $7));
      	  mCRL2log(debug) << "parsed proc Def\n  " << atermpp::aterm( $$) << "" << std::endl;
		}
//	| PROC Identifier ExplicitTemplates LBRACKET RBRACKET IS ProcessBody
/*     When adding these lines don't forget to:
       + update the aterm_spec.txt and translator function
       + update the translator function with ExplicedTemplates */
//	| PROC Identifier ExplicitTemplates LBRACKET FormalParameter_csp RBRACKET IS ProcessBody
/*     When adding these lines don't forget to:
       + update the aterm_spec.txt and translator function
       + update the translator function with ExplicedTemplates */
	;

ProcOpenScope:
	PROC
		{
		  scope_lvl++;
		  mCRL2log(debug) << "Increase Scope to: " << scope_lvl << "" << std::endl;
		}
	;

ProcessBody:
	  BP Statement ProcCloseScope
      	{ safe_assign($$, gsMakeProcSpec( ATmakeList0(), $2 ));
      	  mCRL2log(debug) << "parsed ProcessBody  \n  " << atermpp::aterm( $$) << "" << std::endl;
		}
	| BP LocalVariables_csp PROC_SEP Statement ProcCloseScope
      	{ safe_assign($$, gsMakeProcSpec( ATreverse($2), $4));
      	  mCRL2log(debug) << "parsed ProcessBody  \n  " << atermpp::aterm( $$) << "" << std::endl;
		}
	;

ProcCloseScope:
	EP
		{
		  assert(scope_lvl > 0);
		  scope_lvl--;
		  mCRL2log(debug) << "Decrease Scope to; " << scope_lvl << "" << std::endl;
          var_type_map.clear();
          chan_type_direction_map.clear();
		}
	;

Identifier: ID
		{
 	  	  safe_assign($$, $1 );
      	  mCRL2log(debug) << "parsed id's\n  " << atermpp::aterm( $$) << "" << std::endl;
		}

LocalVariables_csp:
	  LocalVariables
      	{ safe_assign($$, ATmakeList1((ATerm) $1));
      	  mCRL2log(debug) << "LocalVariables_csp: parsed \n  " << atermpp::aterm( $$) << "" << std::endl;
		}
	| LocalVariables_csp COMMA LocalVariables
      	{ safe_assign($$, ATinsert($1, (ATerm) $3));
      	  mCRL2log(debug) << "LocalVariables_csp: parsed \n  " << atermpp::aterm( $$) << "" << std::endl;
		}
	;

LocalVariables:
	  VAR IdentifierTypeExpression_csp
		{
		  safe_assign($$, gsMakeVarSpec( $2 ) );
		  mCRL2log(debug) << "LocalVariables: parsed \n " << atermpp::aterm( $$) << "" << std::endl;
		}
	| CHAN ChannelDefinition_csp
		{
		  //safe_assign($$, gsMakeVarSpec( ATreverse( $2 ) ) );  //<-- gsMakeVarSpec aanpassen
		  mCRL2log(debug) << "LocalVariables: parsed \n " << atermpp::aterm( $$) << "" << std::endl;
		}
    | VAL
//	| RecursionDefinition
	;

IdentifierTypeExpression_csp:
	  IdentifierTypeExpression
      	{ safe_assign($$, $1);
		  mCRL2log(debug) << "IdentifierTypeExpression_csp: parsed \n " << atermpp::aterm( $$) << "" << std::endl;
		}
	| IdentifierTypeExpression_csp COMMA IdentifierTypeExpression
      	{
          ATermList new_list = $3;
          ATermList list = ATreverse($1);
          while (!ATisEmpty(list))
          {
             mCRL2log(debug) << "" << atermpp::aterm(ATgetFirst(list));
             new_list = ATinsert( new_list , ATgetFirst(list));
             list = ATgetNext( list ) ;
          }

          safe_assign($$, new_list);
		  mCRL2log(debug) << "IdentifierTypeExpression_csp: parsed \n " << atermpp::aterm( $$) << "" << std::endl;
		}
	;

IdentifierTypeExpression:
	  IdentifierType
		{
          //An expression cannot be of type "Void"
          ATermList list = $1;
          while (!ATisEmpty(list))
          {
            if (strcmp( ATgetName(ATgetAFun(ATgetArgument(ATgetArgument(ATgetFirst(list),1),0))), "Void" ) == 0)
            {
              chigetposition();
              mCRL2log(error) << "Expression " << atermpp::aterm(ATgetFirst(list)) << " cannot be of type \"void\"" << std::endl;
              exit(1);
            }
            list = ATgetNext( list ) ;
          }

		  safe_assign($$, $1 );
		  mCRL2log(debug) << "IdentifierTypeExpression: parsed \n " << atermpp::aterm( $$) << "" << std::endl;
		}
	| IdentifierType IS Expression
		{
          ATermList list = $1;
          ATermList new_list = ATmakeList0();

          while (!ATisEmpty(list))
          {
            //An expression cannot be of type "void"
            if (strcmp( ATgetName(ATgetAFun(ATgetArgument(ATgetArgument(ATgetFirst(list),1),0))), "Void" ) == 0)
            {
              mCRL2log(error) << "Expression " << atermpp::aterm(ATgetFirst(list)) << " cannot be of type \"void\"" << std::endl;
              exit(1);
            }
            new_list = ATinsert( new_list , (ATerm) gsMakeDataVarExprID( (ATermAppl) ATgetFirst(list), $3));
            list = ATgetNext( list ) ;
          }

          safe_assign($$, new_list);

		  //safe_assign($$, gsMakeDataVarExprID ( $1, $3 ) );
		  mCRL2log(debug) << "IdentifierTypeExpression: parsed \n " << atermpp::aterm( $$) << "" << std::endl;
		}
	;

IdentifierType_csp:
	  IdentifierType
      	{ safe_assign($$, $1);
		  mCRL2log(debug) << "IdentifierType_csp: parsed \n " << atermpp::aterm( $$) << "" << std::endl;
		}
	| IdentifierType_csp COMMA IdentifierType
      	{
          ATermList new_list = $1;
          ATermList list = ATreverse($3);
          while (!ATisEmpty(list))
          {
             mCRL2log(debug) << "" << atermpp::aterm(ATgetFirst(list));
             new_list = ATinsert( new_list , ATgetFirst(list));
             list = ATgetNext( list ) ;
          }

          safe_assign($$, new_list);
		  mCRL2log(debug) << "IdentifierType_csp: parsed \n " << atermpp::aterm( $$) << "" << std::endl;
		}
	;

FormalParameter_csp:
	  FormalParameter
      	{ safe_assign($$, ATmakeList1((ATerm) $1));
		  mCRL2log(debug) << "FormalParameter_csp: parsed \n " << atermpp::aterm( $$) << "" << std::endl;
		}
	| FormalParameter_csp COMMA FormalParameter
      	{ safe_assign($$, ATinsert($1, (ATerm) $3));
		  mCRL2log(debug) << "FormalParameter_csp: parsed \n " << atermpp::aterm( $$) << "" << std::endl;
		}
	;

FormalParameter:
	  VAR IdentifierType_csp
		{
		  safe_assign($$, gsMakeVarDecl( ATreverse($2) ) );
		  mCRL2log(debug) << "FormalParameter: parsed \n " << atermpp::aterm( $$) << "" << std::endl;
		}
  	| CHAN ChannelDeclaration_csp
		{
		  safe_assign($$, gsMakeChanDecl( ATreverse($2) ) );
		  mCRL2log(debug) << "FormalParameter: parsed \n " << atermpp::aterm( $$) << "" << std::endl;
		}
	;

IdentifierType:
	  Identifier_csp COLON Type
		{
		  /**
			* Build TypeCheck table for declared variables
			*
			* TODO: Add scope
			*
			**/
          ATermList list = $1;
		  size_t n = ATgetLength( list );
		  for(size_t i = 0; i < n ; ++i )
          {
			if (chan_type_direction_map.end() != chan_type_direction_map.find(ATgetFirst( list )))
			{
              chigetposition();
			  mCRL2log(error) << "Channel " << atermpp::aterm(ATgetFirst(list)) << " is already defined!" << std::endl;
			  exit(1);
			};
			list = ATgetTail( list, 1 ) ;
		  }	;

		  list = $1;
		  n = ATgetLength( list );
		  for(size_t i = 0; i < n ; ++i ){
			if (var_type_map.end() != var_type_map.find(ATgetFirst( list )))
			{
              chigetposition();
			  mCRL2log(error) << "Variable " << atermpp::aterm(ATgetFirst(list)) << " is already defined!" << std::endl;
			  exit(1);
			};
			var_type_map[ATgetFirst( list )]= (ATerm) $3;
			list = ATgetTail( list, 1 ) ;
		  }	;

          list = $1;
          ATermList new_list = ATmakeList0();

          while (!ATisEmpty(list))
          {
             mCRL2log(debug) << "" << atermpp::aterm(ATgetFirst(list));
             new_list = ATinsert( new_list , (ATerm) gsMakeDataVarID( (ATermAppl) ATgetFirst(list), $3));
             list = ATgetNext( list ) ;
          }

          safe_assign($$, ATreverse(new_list));
		  mCRL2log(debug) << "IdentifierType: parsed \n " << atermpp::aterm( $$) << "" << std::endl;
		  mCRL2log(debug) << "Typecheck Table " <<  var_type_map.size() << "" << std::endl;
  		}
	;

ChannelDeclaration_csp:
	  ChannelDeclaration
      	{ safe_assign($$, $1 );
      	  mCRL2log(debug) << "ChannelDeclaration_csp: parsed formalparameter channel  \n  " << atermpp::aterm( $$) << "" << std::endl;
		}
	| ChannelDeclaration_csp COMMA ChannelDeclaration
      	{
          ATermList new_list = $1;
          ATermList list = ATreverse($3);
          while (!ATisEmpty(list))
          {
             mCRL2log(debug) << "" << atermpp::aterm(ATgetFirst(list));
             new_list = ATinsert( new_list , ATgetFirst(list));
             list = ATgetNext( list ) ;
          }
          safe_assign($$, new_list);
      	  mCRL2log(debug) << "ChannelDeclaration_csp: parsed formalparameter channel \n  " << atermpp::aterm( $$) << "" << std::endl;
		}
	;

ChannelDeclaration:
	  IdentifierChannelDeclaration_csp COLON Type
		{
          mCRL2log(debug) << __FILE__ << ";" << __LINE__ << std::endl;
		  ATermList list = $1;
		  size_t n = ATgetLength( list );
		  for(size_t i = 0; i < n ; ++i ){
			if (var_type_map.end() != var_type_map.find(ATgetArgument( ATgetFirst( list ),0)))
			{
              chigetposition();
			  mCRL2log(error) << "Variable " << atermpp::aterm(ATgetFirst(list)) << " is already defined!" << std::endl;
			  exit(1);
			};
			list = ATgetTail( list, 1 ) ;
		  }	;
          mCRL2log(debug) << "\n" << atermpp::aterm( $1) << "" << std::endl;

          list = $1;
		  n = ATgetLength( list );
		  for(size_t i = 0; i < n ; ++i ){
			if (chan_type_direction_map.end() != chan_type_direction_map.find(ATgetArgument( ATgetFirst( list ),0)))
			{
              chigetposition();
			  mCRL2log(error) << "Channel " << atermpp::aterm(ATgetFirst(list)) << " is already defined!" << std::endl;
			  exit(1);
			};
			chan_type_direction_map[ATgetArgument(ATgetFirst( list ), 0)]=  make_pair( (ATerm) $3, ATgetArgument(ATgetFirst( list ), 1) );
			list = ATgetTail( list, 1 ) ;
		  }	;

		  list = $1;
          ATermList new_list = ATmakeList0();
		  while(!ATisEmpty(list))

          {
             //Hash channel is set to 0 indicating no hash is used
             ATermAppl zero = (ATermAppl) gsMakeExpression(gsString2ATermAppl("0"), (ATermAppl) gsMakeType(gsString2ATermAppl("Nat")));
             new_list = ATinsert(new_list,(ATerm) gsMakeChannelTypedID( (ATermAppl) ATgetFirst(list), $3, zero ) );
			 list = ATgetNext( list ) ;
		  }

		  safe_assign($$, new_list );
		  mCRL2log(debug) << "ChannelDefinition: parsed VariableList \n " << atermpp::aterm( $$) << "" << std::endl;
		}
	| IdentifierChannelDeclaration_csp COLON Expression HASH Type
		{
          mCRL2log(debug) << __FILE__ << ";" << __LINE__ << std::endl;

          //Make sure that Expression is a number
          UnaryTypeCheck( (ATermAppl) ATgetArgument($3,1), "Nat");

		  ATermList list = $1;
		  size_t n = ATgetLength( list );
		  for(size_t i = 0; i < n ; ++i ){
			if (var_type_map.end() != var_type_map.find(ATgetArgument( ATgetFirst( list ),0)))
			{
              chigetposition();
			  mCRL2log(error) << "Variable " << atermpp::aterm(ATgetFirst(list)) << " is already defined!" << std::endl;
			  exit(1);
			};
			list = ATgetTail( list, 1 ) ;
		  }	;

          list = $1;
		  n = ATgetLength( list );
		  for(size_t i = 0; i < n ; ++i ){
			if (chan_type_direction_map.end() != chan_type_direction_map.find(ATgetArgument( ATgetFirst( list ),0)))
			{
              chigetposition();
			  mCRL2log(error) << "Channel " << atermpp::aterm(ATgetFirst(list)) << " is already defined!" << std::endl;
			  exit(1);
			};
			chan_type_direction_map[ATgetArgument(ATgetFirst( list ), 0)]=  make_pair( (ATerm) $5, ATgetArgument(ATgetFirst( list ), 1) );
			list = ATgetTail( list, 1 ) ;
		  }	;

		  list = $1;
          ATermList new_list = ATmakeList0();
		  while(!ATisEmpty(list))
          {
             mCRL2log(debug) << "" << atermpp::aterm(ATgetFirst(list));
             new_list = ATinsert(new_list,(ATerm) gsMakeChannelTypedID( (ATermAppl) ATgetFirst(list), $5, $3 ) );
			 list = ATgetNext( list ) ;
		  }

		  safe_assign($$, new_list );
		  mCRL2log(debug) << "ChannelDefinition: parsed VariableList \n " << atermpp::aterm( $$) << "" << std::endl;
		}
	;

IdentifierChannelDeclaration_csp:
	  IdentifierChannelDeclaration
      	{ safe_assign($$, ATmakeList1((ATerm) $1));
      	  mCRL2log(debug) << "IdentifierChannelDeclaration_csp: parsed formalparameter channel  \n  " << atermpp::aterm( $$) << "" << std::endl;
		}
	| IdentifierChannelDeclaration_csp COMMA IdentifierChannelDeclaration
      	{ safe_assign($$, ATinsert($1, (ATerm) $3));
      	  mCRL2log(debug) << "IdentifierChannelDeclaration_csp: parsed formalparameter channel \n  " << atermpp::aterm( $$) << "" << std::endl;
		}
	;

IdentifierChannelDeclaration:
	  Identifier RECV
        {
          safe_assign($$, gsMakeChannelID($1, gsMakeRecv()));
		  mCRL2log(debug) << "IdentifierChannelDeclaration: parsed Identifier Type With Expression \n " << atermpp::aterm( $$) << "" << std::endl;
        }
	| Identifier EXCLAMATION
        {
          safe_assign($$, gsMakeChannelID($1, gsMakeSend()));
		  mCRL2log(debug) << "IdentifierChannelDeclaration: parsed Identifier Type With Expression \n " << atermpp::aterm( $$) << "" << std::endl;
        }
/*	| Identifier SENDRECV
        {
          safe_assign($$, gsMakeChannelID($1, $2));
		  mCRL2log(debug) << "IdentifierChannelDeclaration: parsed Identifier Type With Expression \n " << atermpp::aterm( $$) << "" << std::endl;
        }
	| Identifier RECVSEND
        {
          safe_assign($$, gsMakeChannelID($1, $2));
		  mCRL2log(debug) << "IdentifierChannelDeclaration: parsed Identifier Type With Expression \n " << atermpp::aterm( $$) << "" << std::endl;
        } */
	;

ChannelDefinition_csp:
	  ChannelDefinition
      	{ //safe_assign($$, ATmakeList1((ATerm) $1));
      	  mCRL2log(debug) << "ChannelDefinition_csp: parsed \n  " << atermpp::aterm( $$) << "" << std::endl;
		}
	| ChannelDefinition_csp COMMA ChannelDefinition
      	{ //safe_assign($$, ATinsert($1, (ATerm) $3));
      	  mCRL2log(debug) << "ChannelDefinition_csp: parsed \n  " << atermpp::aterm( $$) << "" << std::endl;
		}
	;

ChannelDefinition:
	  IdentifierChannelDefinition_csp COLON Type
		{
          mCRL2log(debug) << __FILE__ << ";" << __LINE__ << std::endl;
          mCRL2log(debug) << "ChannelDefinition" << std::endl;
		  ATermList list = $1;
		  size_t n = ATgetLength( list );
		  for(size_t i = 0; i < n ; ++i ){
			if (var_type_map.end() != var_type_map.find(ATgetArgument( ATgetFirst( list ),0)))
			{
              chigetposition();
			  mCRL2log(error) << "Variable " << atermpp::aterm(ATgetFirst(list)) << " is already defined!" << std::endl;
			  exit(1);
			};
			list = ATgetNext( list ) ;
		  }	;

          list = $1;
		  n = ATgetLength( list );
		  for(size_t i = 0; i < n ; ++i ){
			if (chan_type_direction_map.end() != chan_type_direction_map.find(ATgetArgument( ATgetFirst( list ),0)))
			{
              chigetposition();
			  mCRL2log(error) << "Channel " << atermpp::aterm(ATgetFirst(list)) << " is already defined!" << std::endl;
			  exit(1);
			};
			chan_type_direction_map[ATgetArgument(ATgetFirst( list ), 0)]=  make_pair( (ATerm) $3, ATgetArgument(ATgetFirst( list ), 1) );
			list = ATgetNext( list ) ;
		  }	;

		  list = $1;
          ATermList new_list = ATmakeList0();
		  while(!ATisEmpty(list))
          {
             //Hash channel is set to 0 indicating no hash is used
             ATermAppl zero = (ATermAppl) gsMakeExpression(gsString2ATermAppl("0"), (ATermAppl) gsMakeType(gsString2ATermAppl("Nat")));
             new_list = ATinsert(new_list,(ATerm) gsMakeChannelTypedID( (ATermAppl) ATgetFirst(list), $3, zero ) );
			 list = ATgetNext( list ) ;
		  }

		  safe_assign($$, new_list );
		  mCRL2log(debug) << "ChannelDefinition: parsed VariableList \n " << atermpp::aterm( $$) << "" << std::endl;
		}
	| IdentifierChannelDefinition_csp COLON Expression HASH Type
		{
          mCRL2log(debug) << __FILE__ << ";" << __LINE__ << std::endl;

          //Make sure that Expression is a number
          UnaryTypeCheck( (ATermAppl) ATgetArgument($3,1), "Nat");

		  ATermList list = $1;
		  size_t n = ATgetLength( list );
		  for(size_t i = 0; i < n ; ++i ){
			if (var_type_map.end() != var_type_map.find(ATgetArgument( ATgetFirst( list ),0)))
			{
              chigetposition();
			  mCRL2log(error) << "Variable " << atermpp::aterm(ATgetFirst(list)) << " is already defined!" << std::endl;
			  exit(1);
			};
			list = ATgetNext( list ) ;
		  }	;

          list = $1;
		  n = ATgetLength( list );
		  for(size_t i = 0; i < n ; ++i ){
			if (chan_type_direction_map.end() != chan_type_direction_map.find(ATgetArgument( ATgetFirst( list ),0)))
			{
              chigetposition();
			  mCRL2log(error) << "Channel " << atermpp::aterm(ATgetFirst(list)) << " is already defined!" << std::endl;
			  exit(1);
			};
			chan_type_direction_map[ATgetArgument(ATgetFirst( list ), 0)]=  make_pair( (ATerm) $5, ATgetArgument(ATgetFirst( list ), 1) );
			list = ATgetNext( list ) ;
		  }	;

		  list = $1;
          ATermList new_list = ATmakeList0();
		  while(!ATisEmpty(list))
          {
             mCRL2log(debug) << "" << atermpp::aterm(ATgetFirst(list));
             new_list = ATinsert(new_list,(ATerm) gsMakeChannelTypedID( (ATermAppl) ATgetFirst(list), $5, $3 ) );
			 list = ATgetNext( list ) ;
		  }

		  safe_assign($$, new_list );
		  mCRL2log(debug) << "ChannelDefinition: parsed VariableList \n " << atermpp::aterm( $$) << "" << std::endl;
		}
	;

IdentifierChannelDefinition_csp:
	  IdentifierChannelDefinition
      	{ safe_assign($$, ATmakeList1((ATerm) $1));
      	  mCRL2log(debug) << "IdentifierChannelDefinition_csp: parsed \n  " << atermpp::aterm( $$) << "" << std::endl;
		}
	| IdentifierChannelDefinition_csp COMMA IdentifierChannelDefinition
      	{ safe_assign($$, ATinsert($1, (ATerm) $3));
      	  mCRL2log(debug) << "IdentifierChannelDefinition_csp: parsed \n  " << atermpp::aterm( $$) << "" << std::endl;
		}
	;

IdentifierChannelDefinition:
	  Identifier
        {
          safe_assign($$, gsMakeChannelID($1, gsMakeNil()));
		  mCRL2log(debug) << "IdentifierChannelDefinition: parsed \n " << atermpp::aterm( $$) << "" << std::endl;
        }
/*	| Identifier SENDRECV
		{
          safe_assign($$, gsMakeChannelID($1, gsMakeNil));
		  mCRL2log(debug) << "IdentifierChannelDefinition: parsed Identifier Type With Expression \n " << atermpp::aterm( $$) << "" << std::endl;
		}
	| Identifier RECVSEND
		{
          safe_assign($$, gsMakeChannelID($1, gsMakeNil));
		  mCRL2log(debug) << "IdentifierChannelDefinition: parsed Identifier Type With Expression \n " << atermpp::aterm( $$) << "" << std::endl;
		}*/
	;


Type:
	  BasicType
	| LBRACKET Type RBRACKET
		{
          safe_assign($$, $2  );
      	  mCRL2log(debug) << "Type: parsed Type \n  " << atermpp::aterm( $$) << "" << std::endl;
		}

	| ContainerType
/*	| FunctionType
	| DistributionType */
	;

BasicType:
 	  BOOL
		{
          safe_assign($$, gsMakeType( gsString2ATermAppl( "Bool" ) ) );
      	  mCRL2log(debug) << "BasicType: parsed Type \n  " << atermpp::aterm( $$) << "" << std::endl;
		}
 	| NAT
		{
          safe_assign($$, gsMakeType( gsString2ATermAppl("Nat" ) ) );
      	  mCRL2log(debug) << "BasicType: parsed Type \n  " << atermpp::aterm( $$) << "" << std::endl;
		}
 	| VOID
		{
          safe_assign($$, gsMakeType( gsString2ATermAppl("Void" ) ) );
      	  mCRL2log(debug) << "BasicType: parsed Type \n  " << atermpp::aterm( $$) << "" << std::endl;
		}
    | REAL
 	| TYPE
		{
          safe_assign($$, gsMakeType( $1 ) );
      	  mCRL2log(debug) << "BasicType: parsed Type \n  " << atermpp::aterm( $$) << "" << std::endl;
		}
//	| Identifier
//	| Identifier DOT Identier
	;

ContainerType:
      SQLBRACKET Type SQRBRACKET
        {
          safe_assign($$, gsMakeListType($2));
      	  mCRL2log(debug) << "ContainerType: parsed Type \n  " << atermpp::aterm( $$) << "" << std::endl;

        }
    | LBRACE Type RBRACE
        {
          safe_assign($$, gsMakeSetType($2));
      	  mCRL2log(debug) << "ContainerType: parsed Type \n  " << atermpp::aterm( $$) << "" << std::endl;

        }
    | LBRACKET Type_csp COMMA Type RBRACKET
		{
          ATermList list = ATinsert( $2, (ATerm) $4 );
          safe_assign($$, gsMakeTupleType(ATreverse(list)));
      	  mCRL2log(debug) << "ContainerType: parsed Type \n  " << atermpp::aterm( $$) << "" << std::endl;
		}
    ;

Type_csp:
      Type
      	{
          safe_assign($$, ATmakeList1( (ATerm) $1) );
      	  mCRL2log(debug) << "ChannelDeclaration_csp: parsed formalparameter channel  \n  " << atermpp::aterm( $$) << "" << std::endl;
		}
    | Type_csp COMMA Type
		{
          safe_assign($$, ATinsert( $1, (ATerm) $3 ) );
      	  mCRL2log(debug) << "BasicType: parsed Type \n  " << atermpp::aterm( $$) << "" << std::endl;
		}
    ;

/**
  * STATEMENTS
  *
  **/

Statement:
	  LBRACKET Statement RBRACKET
      	{ safe_assign($$, gsMakeParenthesisedStat( $2));
      	  mCRL2log(debug) << "Statement: parsed \n  " << atermpp::aterm( $$) << "" << std::endl;
		}
	| BasicStatement
	| UnaryStatement
	| BinaryStatement
	;

ModelStatement:
      Instantiation
    | ModelStatement BARS ModelStatement
      	{ safe_assign($$, gsMakeParStat( $1, $3));
      	  mCRL2log(debug) << "ModelStatement: parsed \n  " << atermpp::aterm( $$) << "" << std::endl;
		}

BasicStatement:
//	  Instantiation
	  AssignmentStatement
	| CommStatement
	| DelayStatement
//	| HybridStatement
//	| ReturnStatement
//	| FoldStatement
	| AdvancedStatement
	;

DelayStatement:
      DELAY Expression
      {
        // Ugly Hack :D
        safe_assign($$, gsMakeSkipStat( gsMakeNil(), gsMakeNil(), gsMakeSkip() ));
        mCRL2log(debug) << "AssignmentStatement: parsed \n  " << atermpp::aterm( $$) << "" << std::endl;
      }
    ;


AssignmentStatement:
	  OptGuard OptChannel ExpressionIdentier_csp ASSIGNMENT Expression_csp
     	{
          ATermList ids = $3;
          ATermList exprs = $5;

          while(!ATisEmpty(ids))
          {
            if (!ContainerTypeChecking((ATermAppl) ATgetArgument(ATgetFirst(ids), 1), (ATermAppl) ATgetArgument(ATgetFirst(exprs), 1)))
		    {
              chigetposition();
              mCRL2log(error) << "Assignment failed: Incompatible Types Checking failed " << atermpp::aterm(ids) << " and " << atermpp::aterm(exprs) << std::endl;
		      exit(1);
            }
            ids = ATgetNext(ids);
            exprs = ATgetNext(exprs);
          }

          safe_assign($$, gsMakeAssignmentStat($1, $2, ATreverse($3), ATreverse($5) ) );
      	  mCRL2log(debug) << "AssignmentStatement: parsed \n  " << atermpp::aterm( $$) << "" << std::endl;
		}
	|
	  OptGuard OptChannel SKIP
      	{
          safe_assign($$, gsMakeSkipStat( $1, $2, gsMakeSkip() ));
      	  mCRL2log(debug) << "AssignmentStatement: parsed \n  " << atermpp::aterm( $$) << "" << std::endl;
		}
//	| OptGuard LBRACE Expression_csp RBRACE COLON Expression_csp GG Identifier
	;

OptGuard: /* empty */
     	{ safe_assign($$, gsMakeNil() );
		}
	| Expression GUARD
     	{
			/**
			  * Type Checking
			  *
			  **/
			if(ATAgetArgument($1,1) != gsMakeType(gsString2ATermAppl("Bool")))
				{
				  mCRL2log(error) << "OptGuard failed: Incompatible Types Checking failed" << std::endl;
				  exit(1);
				};


			safe_assign($$, $1 );
      	  	mCRL2log(debug) << "OptGuard: parsed \n  " << atermpp::aterm( $$) << "" << std::endl;
		}
	;

OptChannel: /* empty */
     	{ safe_assign($$, gsMakeNil() );
		}
	| Expression COLON
      	{ safe_assign($$, $1);
		  mCRL2log(error) << "OptChannel not yet implemented" << std::endl;
		  assert(false);
      	  mCRL2log(debug) << "OptChannel: parsed \n  " << atermpp::aterm( $$) << "" << std::endl;
		}
	;

Identifier_csp:
	  Identifier
      	{ safe_assign($$, ATmakeList1( (ATerm) $1));
      	  mCRL2log(debug) << "Identifier_csp: parsed \n  " << atermpp::aterm( $$) << "" << std::endl;
		}
	| Identifier_csp COMMA Identifier
      	{ safe_assign($$, ATinsert($1, (ATerm) $3));
      	  mCRL2log(debug) << "Identifier_csp: parsed \n  " << atermpp::aterm( $$) << "" << std::endl;
		}
	;

Expression_csp:
	  Expression
      	{ safe_assign($$, ATmakeList1((ATerm) $1));
      	  mCRL2log(debug) << "Expression_csp: parsed \n  " << atermpp::aterm( $$) << "" << std::endl;
		}
	| Expression_csp COMMA Expression
      	{ safe_assign($$, ATinsert($1, (ATerm) $3));
      	  mCRL2log(debug) << "Expression_csp: parsed \n  " << atermpp::aterm( $$) << "" << std::endl;
		}
	;

CommStatement:
	  OptGuard Expression EXCLAMATION Expression_csp
        {
          //Check if $2 is properly typed
          //Check if $4 is properly typed
          //Check if $2 can send

          ATermAppl channel = (ATermAppl) ATgetArgument(ATgetArgument($2,0),0);
          ATermAppl hash    = (ATermAppl) ATgetArgument($2,2);
          mCRL2log(debug) << "" << atermpp::aterm($2) << "" << std::endl;
          mCRL2log(debug) << "" << atermpp::aterm(hash) << "" << std::endl;

          safe_assign($$, gsMakeSendStat($1, channel, hash , ATreverse( $4) ) );
      	  mCRL2log(debug) << "CommStatement: parsed " << atermpp::aterm( $$) << "" << std::endl;
        }
//	| OptGuard Expression SSEND Expression_csp
	| OptGuard Expression EXCLAMATION
        {
          mCRL2log(debug) << "" << atermpp::aterm($2);

          ATermAppl channel = (ATermAppl) ATgetArgument(ATgetArgument( $2,0),0);
          ATermAppl hash    = (ATermAppl) ATgetArgument($2,2);

          safe_assign($$, gsMakeSendStat($1, channel, hash, ATmakeList0() ) );
      	  mCRL2log(debug) << "CommStatement: parsed " << atermpp::aterm( $$) << "" << std::endl;
        }
//	| OptGuard Expression SSEND
//	| OptGuard SSEND Expression_csp
	| OptGuard Expression RECV Expression_csp
        {
          //Check if $2 is properly typed
          //Check if $4 is properly typed
          //Check if $2 can receive

          mCRL2log(debug) << "" << atermpp::aterm($2);

          ATermAppl channel = (ATermAppl) ATgetArgument(ATgetArgument( $2,0),0);
          ATermAppl hash    = (ATermAppl) ATgetArgument($2,2);

          safe_assign($$, gsMakeRecvStat($1, channel, hash, ATreverse( $4) ) );
      	  mCRL2log(debug) << "CommStatement: parsed " << atermpp::aterm( $$) << "" << std::endl;
        }
//	| OptGuard Expression RRECV Expression_csp
	| OptGuard Expression RECV
        {
          //Check if $2 is properly typed
          //Check if $4 is properly typed
          //Check if $2 can receive

          mCRL2log(debug) << "" << atermpp::aterm($2);

          ATermAppl channel = (ATermAppl) ATgetArgument(ATgetArgument( $2,0),0);
          ATermAppl hash    = (ATermAppl) ATgetArgument($2,2);

          safe_assign($$, gsMakeRecvStat($1, channel, hash, ATmakeList0() ) );
      	  mCRL2log(debug) << "CommStatement: parsed " << atermpp::aterm( $$) << "" << std::endl;
        }
//	| OptGuard Expression RRECV
//	| OptGuard RRECV Expression_csp
	;

Instantiation:
	  Identifier LBRACKET RBRACKET
      {
        safe_assign($$, gsMakeInstantiation($1, ATmakeList0()));
      }
	| Identifier LBRACKET Expression_csp RBRACKET
      {
        safe_assign($$, gsMakeInstantiation($1, ATreverse($3)));
      }
	;

BinaryStatement:
	  Statement SEP Statement
      	{ safe_assign($$, gsMakeSepStat( $1, $3));
      	  mCRL2log(debug) << "parsed SEP statement \n  " << atermpp::aterm( $$) << "" << std::endl;
		}
	| Statement ALT Statement
      	{ safe_assign($$, gsMakeAltStat( $1, $3));
      	  mCRL2log(debug) << "parsed ALT statement \n  " << atermpp::aterm( $$) << "" << std::endl;
		}
/*	| Statement BARS Statement
      	{ safe_assign($$, gsMakeParStat( $1, $3));
      	  mCRL2log(debug) << "parsed Paralell statement \n  " << atermpp::aterm( $$) << "" << std::endl;
		}*/
	;

UnaryStatement:
	  STAR Statement
        {
          mCRL2log(debug) << "" <<  __LINE__ << "" << std::endl;
      	  safe_assign($$, gsMakeStarStat( $2));
      	  mCRL2log(debug) << "parsed STAR statement \n  " << atermpp::aterm( $$) << "" << std::endl;
		}
	| Expression GUARD_REP Statement
      	{
          mCRL2log(debug) << "" <<  __LINE__ << "" << std::endl;
          mCRL2log(debug) << "" << atermpp::aterm($1) << "" << std::endl;
          mCRL2log(debug) << "" << atermpp::aterm($3) << "" << std::endl;
          UnaryTypeCheck(ATAgetArgument($1,1), "Bool");
          safe_assign($$, gsMakeGuardedStarStat( $1, $3));
      	  mCRL2log(debug) << "parsed GuardedSTAR statement \n  " << atermpp::aterm( $$) << "" << std::endl;
		}
	;

AdvancedStatement:
	  OptGuard DEADLOCK
      	{ safe_assign($$, gsMakeDeltaStat($1, gsMakeDelta()));
      	  mCRL2log(debug) << "parsed deadlock statement \n  " << atermpp::aterm( $$) << "" << std::endl;
		}
	;

/**
  * EXPRESSIONS
  *
  *
  **/

Expression: //NUMBER
	  LBRACKET Expression RBRACKET
		{
			/**
			  * Type Checking inherhit
			  *
			  **/

 	  		safe_assign($$,
				gsMakeUnaryExpression( gsString2ATermAppl("()" ),
				ATAgetArgument($2,1),
				$2 )
			);
      		mCRL2log(debug) << "parsed Negation Expression's\n  " << atermpp::aterm( $$) << "" << std::endl;
		}

	| BasicExpression
	| BooleanExpression
    | MinusExpression
	| PlusExpression
    | NatIntExpression
	| EqualityExpression
	| ListExpression
	| SetExpression
    | MemberTest
	| RecordExpression
/*	| IntExpression
	| RealExpression */
//	| StringExpression
//	| DictExpression
//	| VectorExpression
//	| DistributionExpression
//	| FunctionExpression
	;
ExpressionIdentier_csp: //NUMBER
	  ExpressionIdentifier
      	{ safe_assign($$, ATmakeList1((ATerm) $1));
      	  mCRL2log(debug) << "parsed expression-element \n  " << atermpp::aterm( $$) << "" << std::endl;
		}
	| ExpressionIdentier_csp COMMA ExpressionIdentifier
      	{ safe_assign($$, ATinsert($1, (ATerm) $3));
      	  mCRL2log(debug) << "parsed expression-element\n  " << atermpp::aterm( $$) << "" << std::endl;
		}
	;
ExpressionIdentifier:
	 Identifier
		{
		  /**
		    * Lookup Identifier Type
		    *
		    * TODO: Add scope
		    *
		    **/

		  // Determine if the expression is defined already
		  if (var_type_map.end() == var_type_map.find( (ATerm) $1))
		    {
              chigetposition();
		      mCRL2log(error) << "ExpressionIdentifier: Variable " << atermpp::aterm($1) << " is not defined!" << std::endl;
		      exit(1);
		    };

		  //Type the expression
 	  	  safe_assign($$,
			gsMakeExpression( $1,
			  (ATermAppl) var_type_map[(ATerm) $1]
			)
		  );
      	  mCRL2log(debug) << "parsed Identifier's\n  " << atermpp::aterm( $$) << "" << std::endl;
		}


BasicExpression:
	 Identifier
		{
		  /**
		    * Lookup Identifier Type
		    *
		    * TODO: Add scope
		    *
		    **/

          bool channel_exists = false;
          bool variable_exists = false;
          if (chan_type_direction_map.end() != chan_type_direction_map.find( (ATerm) $1))
          {
             channel_exists = true;
             //ATermAppl one = (ATermAppl) gsMakeExpression(gsString2ATermAppl("0"), (ATermAppl) gsMakeType(gsString2ATermAppl("Nat")));
              safe_assign($$,
                gsMakeChannelTypedID(
                  gsMakeChannelID($1, gsMakeNil()),
                  (ATermAppl) chan_type_direction_map[(ATerm) $1].first,
                  gsMakeNil()
                )
              );
            }

          // Determine if the expression is defined already
          if (var_type_map.end() != var_type_map.find( (ATerm) $1))
          {
              variable_exists = true;
              //Type the expression
              safe_assign($$,
              gsMakeExpression( $1,
                (ATermAppl) var_type_map[(ATerm) $1]
              )
             );
          }

          if(!channel_exists && !variable_exists)
          {
              chigetposition();
              mCRL2log(error) << "BasicExpression: Variable/Channel " << atermpp::aterm($1) << " is not defined!" << std::endl;
              exit(1);
          }

          mCRL2log(debug) << "BasicExpression: parsed \n  " << atermpp::aterm( $$) << "" << std::endl;
		}
	| Identifier DOT Expression

		{
		  /**
		    * Lookup Identifier Type
		    *
		    * TODO: Add scope
		    *
		    **/
          mCRL2log(debug) <<  __LINE__  << ":" << __FILE__ << std::endl;

          UnaryTypeCheck( (ATermAppl) ATgetArgument($3,1), "Nat");

          if (var_type_map.find((ATerm) $1) != var_type_map.end())
          {
            ATermAppl tuple_type = (ATermAppl) var_type_map[(ATerm) $1];
            mCRL2log(debug) << "" << atermpp::aterm( tuple_type) << "" << std::endl;

            //Check if $1 is a tuple
            if( strcmp("TupleType", ATgetName( ATgetAFun( tuple_type ) ) ) != 0 )
            {
              chigetposition();
              mCRL2log(error) << atermpp::aterm($1) << " is not a Tuple" << std::endl;
              exit(1);
            }

            if (!is_number(ATgetName(ATgetAFun(ATgetArgument($3,0)))) )
            {
              chigetposition();
              mCRL2log(error) << "BasicExpression: Expected a number after the \".\"" << std::endl;
              exit(1);
            }

            int index = atoi(ATgetName(ATgetAFun(ATgetArgument($3,0))));

            if (index >= (int) ATgetLength( (ATermList) ATgetArgument( tuple_type, 0 ) ) )
            {
              chigetposition();
              mCRL2log(error) << "BasicExpression: Index value \"" << index << "\" is out of bounds for " << atermpp::aterm($1) << std::endl;
              exit(1);
            }

            ATermList to_process = (ATermList) ATgetArgument(tuple_type, 0 );
            while (index > 0)
            {
              to_process = ATgetNext(to_process);
              --index;
            }
            ATerm type = ATgetFirst(to_process);

            safe_assign($$,
              gsMakeTupleDot(
			    gsMakeExpression( $1,
			      (ATermAppl) var_type_map[(ATerm) $1]
			    ),
                (ATermAppl) type,
                $3
              )
            );
            mCRL2log(debug) << "BasicExpression (Tuple Selection): parsed \n  " << atermpp::aterm( $$) << "" << std::endl;
          }

          if (chan_type_direction_map.end() != chan_type_direction_map.find( (ATerm) $1))
          {
  	           //Check if # of channel is a Natural number
  	           //UnaryTypeCheck( (ATermAppl) gsMakeType($3), "Nat");

  	           safe_assign($$,
  	             gsMakeChannelTypedID(
  	               gsMakeChannelID($1, gsMakeNil()),
  	               (ATermAppl) chan_type_direction_map[(ATerm) $1].first,
  	               $3
  	             )
  	           );

  	           mCRL2log(debug) << "BasicExpression (Hashed Channel): parsed \n  " << atermpp::aterm( $$) << "" << std::endl;
          }

          if ( (chan_type_direction_map.end() == chan_type_direction_map.find( (ATerm) $1)) &&
               (var_type_map.find((ATerm) $1) == var_type_map.end()) )
          {
              chigetposition();
              mCRL2log(error) << "BasicExpression: Variable/Channel " << atermpp::aterm($1) << " is not defined!" << std::endl;
              exit(1);
          }
		}
//	  OLD LBRACKET Expression RBRACKET
//	| Identifier DERIVATIVE
//	| Expression LANGLE TemplateValue RANGLE
//	| FoldExpression
	;

BooleanExpression:
	  FALSE
		{
 	  		safe_assign($$,
				gsMakeExpression( $1,
					gsMakeType( gsString2ATermAppl("Bool" ))
				)
			);
      		mCRL2log(debug) << "BooleanExpression: parsed \n  " << atermpp::aterm( $$) << "" << std::endl;
		}
	| TRUE
		{
 	  		safe_assign($$,
				gsMakeExpression( $1,
					gsMakeType( gsString2ATermAppl("Bool" ))
				)
			);
      		mCRL2log(debug) << "BooleanExpression: parsed \n  " << atermpp::aterm( $$) << "" << std::endl;
		}
	| NOT Expression
		{
            UnaryTypeCheck( (ATermAppl) ATgetArgument($2,1), "Bool");

 	  		safe_assign($$,
				gsMakeUnaryExpression( gsString2ATermAppl("!" ),
				gsMakeType( gsString2ATermAppl("Bool" ) ),
				$2 )
			);
      		mCRL2log(debug) << "parsed Negation Expression's\n  " << atermpp::aterm( $$) << "" << std::endl;
		}
	| EXCLAMATION Expression
		{
            UnaryTypeCheck( (ATermAppl) ATgetArgument($2,1), "Bool");

 	  		safe_assign($$,
				gsMakeUnaryExpression( gsString2ATermAppl("!" ),
				gsMakeType( gsString2ATermAppl("Bool" ) ),
				$2 )
			);
      		mCRL2log(debug) << "parsed Negation Expression's\n  " << atermpp::aterm( $$) << "" << std::endl;
		}
	| Expression AND Expression
		{
            BinTypeCheck(ATAgetArgument($1,1), ATAgetArgument($3,1), "Bool");

 	  		safe_assign($$,
				gsMakeBinaryExpression( gsString2ATermAppl("&&" ),
				gsMakeType( gsString2ATermAppl("Bool" ) ),
				$1 ,
				$3
				)
			);
      		mCRL2log(debug) << "parsed Binary AND Expression's\n  " << atermpp::aterm( $$) << "" << std::endl;
		}
	| Expression OR Expression
		{
            BinTypeCheck(ATAgetArgument($1,1), ATAgetArgument($3,1), "Bool");

 	  		safe_assign($$,
				gsMakeBinaryExpression( gsString2ATermAppl("||" ),
				gsMakeType( gsString2ATermAppl("Bool" ) ),
				$1 ,
				$3
				)
			);
      		mCRL2log(debug) << "parsed Binary AND Expression's\n  " << atermpp::aterm( $$) << "" << std::endl;
		}
	| Expression IMPLIES Expression
		{
            BinTypeCheck(ATAgetArgument($1,1), ATAgetArgument($3,1), "Bool");

 	  		safe_assign($$,
				gsMakeBinaryExpression( gsString2ATermAppl("->" ),
				gsMakeType( gsString2ATermAppl("Bool" ) ),
				$1 ,
				$3
				)
			);
      		mCRL2log(debug) << "parsed Binary AND Expression's\n  " << atermpp::aterm( $$) << "" << std::endl;
		}
	;

NatIntExpression:
	  NUMBER
		{
 	  		safe_assign($$,
				gsMakeExpression( $1,
					gsMakeType( gsString2ATermAppl("Nat" ))
				)
			);
      		mCRL2log(debug) << "parsed Expression's\n  " << atermpp::aterm( $$) << "" << std::endl;
		}
	| PLUS Expression
		{
            UnaryTypeCheck(ATAgetArgument($1,1), "Nat");

 	  		safe_assign($$,
				gsMakeUnaryExpression( $1,
					gsMakeType( gsString2ATermAppl("Nat" ) ),
					$2
				)
			);
      		mCRL2log(debug) << "parsed UnaryExpression's\n  " << atermpp::aterm( $$) << "" << std::endl;
		}
	| MINUS Expression
		{
            UnaryTypeCheck(ATAgetArgument($1,1), "Nat");

 	  		safe_assign($$,
				gsMakeUnaryExpression($1,
					gsMakeType( gsString2ATermAppl("Nat" ) ),
					$2
				)
			);
      		mCRL2log(debug) << "parsed UnaryExpression's\n  " << atermpp::aterm( $$) << "" << std::endl;
		}
	| Expression POWER Expression
		{
            BinTypeCheck(ATAgetArgument($1,1), ATAgetArgument($3,1), "Nat");

 	  		safe_assign($$,
				gsMakeBinaryExpression( gsString2ATermAppl("^" ),
				gsMakeType( gsString2ATermAppl("Nat" ) ),
				$1 ,
				$3
				)
			);
      		mCRL2log(debug) << "parsed Binary POWER Expression's\n  " << atermpp::aterm( $$) << "" << std::endl;
		}
	| Expression STAR Expression
		{
            BinTypeCheck(ATAgetArgument($1,1), ATAgetArgument($3,1), "Nat");

 	  		safe_assign($$,
				gsMakeBinaryExpression( gsString2ATermAppl("*" ),
				gsMakeType( gsString2ATermAppl("Nat" ) ),
				$1 ,
				$3
				)
			);
      		mCRL2log(debug) << "parsed Binary STAR Expression's\n  " << atermpp::aterm( $$) << "" << std::endl;
		}
	| Expression DIVIDE Expression
		{
            BinTypeCheck(ATAgetArgument($1,1), ATAgetArgument($3,1), "Nat");

 	  		safe_assign($$,
				gsMakeBinaryExpression( gsString2ATermAppl("/" ),
				gsMakeType( gsString2ATermAppl("Nat" ) ),
				$1 ,
				$3
				)
			);
      		mCRL2log(debug) << "parsed Binary DIVIDE Expression's\n  " << atermpp::aterm( $$) << "" << std::endl;
		}
	| Expression MOD Expression
		{
            BinTypeCheck(ATAgetArgument($1,1), ATAgetArgument($3,1), "Nat");

 	  		safe_assign($$,
				gsMakeBinaryExpression( gsString2ATermAppl("MOD" ),
				gsMakeType( gsString2ATermAppl("Nat" ) ),
				$1 ,
				$3
				)
			);
      		mCRL2log(debug) << "parsed Binary MOD Expression's\n  " << atermpp::aterm( $$) << "" << std::endl;
		}
	| Expression DIV Expression
		{
            BinTypeCheck(ATAgetArgument($1,1), ATAgetArgument($3,1), "Nat");

 	  		safe_assign($$,
				gsMakeBinaryExpression( gsString2ATermAppl("DIV" ),
				gsMakeType( gsString2ATermAppl("Nat" ) ),
				$1 ,
				$3
				)
			);
      		mCRL2log(debug) << "parsed Binary DIV Expression's\n  " << atermpp::aterm( $$) << "" << std::endl;
		}
	| Expression MIN Expression
		{
            BinTypeCheck(ATAgetArgument($1,1), ATAgetArgument($3,1), "Nat");

 	  		safe_assign($$,
				gsMakeBinaryExpression( gsString2ATermAppl("MIN" ),
				gsMakeType( gsString2ATermAppl("Nat" ) ),
				$1 ,
				$3
				)
			);
      		mCRL2log(debug) << "parsed Binary MIN Expression's\n  " << atermpp::aterm( $$) << "" << std::endl;
		}
	| Expression MAX Expression
		{
            BinTypeCheck(ATAgetArgument($1,1), ATAgetArgument($3,1), "Nat");

 	  		safe_assign($$,
				gsMakeBinaryExpression( gsString2ATermAppl("MAX" ),
				gsMakeType( gsString2ATermAppl("Nat" ) ),
				$1 ,
				$3
				)
			);
      		mCRL2log(debug) << "parsed Binary MAX Expression's\n  " << atermpp::aterm( $$) << "" << std::endl;
		}
	| Expression LESS Expression
		{
            BinTypeCheck(ATAgetArgument($1,1), ATAgetArgument($3,1), "Nat");

 	  		safe_assign($$,
				gsMakeBinaryExpression( gsString2ATermAppl("<" ),
				gsMakeType( gsString2ATermAppl("Bool" ) ),
				$1 ,
				$3
				)
			);
      		mCRL2log(debug) << "parsed Binary LESS Expression's\n  " << atermpp::aterm( $$) << "" << std::endl;
		}
	| Expression GREATER Expression
		{
            BinTypeCheck(ATAgetArgument($1,1), ATAgetArgument($3,1), "Nat");

 	  		safe_assign($$,
				gsMakeBinaryExpression( gsString2ATermAppl(">" ),
				gsMakeType( gsString2ATermAppl("Bool" ) ),
				$1 ,
				$3
				)
			);
      		mCRL2log(debug) << "parsed Binary GREATER Expression's\n  " << atermpp::aterm( $$) << "" << std::endl;
		}
	| Expression LEQ Expression
		{
            BinTypeCheck(ATAgetArgument($1,1), ATAgetArgument($3,1), "Nat");

 	  		safe_assign($$,
				gsMakeBinaryExpression( gsString2ATermAppl("<=" ),
				gsMakeType( gsString2ATermAppl("Bool" ) ),
				$1 ,
				$3
				)
			);
      		mCRL2log(debug) << "parsed Binary LEQ Expression's\n  " << atermpp::aterm( $$) << "" << std::endl;
		}
	| Expression GEQ Expression
		{
            BinTypeCheck(ATAgetArgument($1,1), ATAgetArgument($3,1), "Nat");

 	  		safe_assign($$,
				gsMakeBinaryExpression( gsString2ATermAppl(">=" ),
				gsMakeType( gsString2ATermAppl("Bool" ) ),
				$1 ,
				$3
				)
			);
      		mCRL2log(debug) << "parsed Binary GEQ Expression's\n  " << atermpp::aterm( $$) << "" << std::endl;
		}
	;

PlusExpression:
	 Expression PLUS Expression
	 {
       mCRL2log(debug) << "Expression 1: " << atermpp::aterm( $1) << "" << std::endl;
       mCRL2log(debug) << "Expression 2: " << atermpp::aterm( $3) << "" << std::endl;

       bool processed = false;

       /**
         *  Plus operater on naturals
         *
         **/
       if (strcmp(ATgetName(ATgetAFun(ATAgetArgument(ATAgetArgument($3,1),0))), "Nat") == 0 ||
           strcmp(ATgetName(ATgetAFun(ATAgetArgument(ATAgetArgument($1,1),0))), "Nat") == 0
          )
       {

            BinTypeCheck(ATAgetArgument($1,1), ATAgetArgument($3,1), "Nat");

 	  		safe_assign($$,
				gsMakeBinaryExpression( gsString2ATermAppl("+" ),
				gsMakeType( gsString2ATermAppl("Nat" ) ),
				$1 ,
				$3
				)
			);
            mCRL2log(debug) << "PlusExpression - Nat Expression parsed: \n" << atermpp::aterm( $$) << "" << std::endl;
            processed = true;
		}

       /**
         *  Plus operater on sets
         *
         **/
       if (strcmp(ATgetName(ATgetAFun(ATAgetArgument($3,1))), "SetType") == 0 ||
           strcmp(ATgetName(ATgetAFun(ATAgetArgument($1,1))), "SetType") == 0
          )
       {
         if(!ContainerTypeChecking(ATAgetArgument($1,1),  ATAgetArgument($3,1)))
	     {
           chigetposition();
		   mCRL2log(error) << "Incompatible types checking failed" << std::endl;
		   exit(1);
		 }
   		 safe_assign($$, gsMakeBinarySetExpression( $2,
				         ATAgetArgument($1,1),
                	$1, $3));
         mCRL2log(debug) << "PlusExpression - Set Expression parsed: \n  " << atermpp::aterm( $$) << "" << std::endl;
         processed = true;
       }

       /**
         *  For all other types on plus
         *
         **/

       if (!processed)
       {
         chigetposition();
         mCRL2log(error) << "Expressions " << atermpp::aterm(ATAgetArgument($1,0)) << " and "
                         << atermpp::aterm(ATAgetArgument($3, 0)) << " cannot be used with \"+\"" << std::endl;
   
         exit(1);
       }
     }
   ;

MinusExpression:
	 Expression MINUS Expression
	 {
       mCRL2log(debug) << "Expression 1: " << atermpp::aterm( $1) << "" << std::endl;
       mCRL2log(debug) << "Expression 2: " << atermpp::aterm( $3) << "" << std::endl;

       bool processed = false;
       /**
         *  Minus operater on naturals
         *
         **/
       if (strcmp(ATgetName(ATgetAFun(ATAgetArgument(ATAgetArgument($3,1),0))), "Nat") == 0 ||
           strcmp(ATgetName(ATgetAFun(ATAgetArgument(ATAgetArgument($1,1),0))), "Nat") == 0
          )
       {

 	  		safe_assign($$,
				gsMakeBinaryExpression( gsString2ATermAppl("-" ),
				gsMakeType( gsString2ATermAppl("Nat" ) ),
				$1 ,
				$3
				)
			);
         mCRL2log(debug) << "MinusExpression - Nat Expression parsed: \n" << atermpp::aterm( $$) << "" << std::endl;
         processed = true;
       }
       /**
         *  Minus operater on lists
         *
         **/
       if (strcmp(ATgetName(ATgetAFun(ATAgetArgument($3,1))), "ListType") == 0 ||
           strcmp(ATgetName(ATgetAFun(ATAgetArgument($1,1))), "ListType") == 0
          )
       {
         if(!ContainerTypeChecking(ATAgetArgument($1,1),  ATAgetArgument($3,1)))
	     {
           chigetposition();
		   mCRL2log(error) << "Incompatible types checking failed" << std::endl;
		   exit(1);
		 }
   		 safe_assign($$, gsMakeBinaryListExpression( $2,
				         ATAgetArgument($1,1),
                	$1, $3));
         mCRL2log(debug) << "MinusExpression - Literal Expression parsed: \n  " << atermpp::aterm( $$) << "" << std::endl;
         processed = true;
       }
       /**
         *  Minus operater on sets
         *
         **/
       if (strcmp(ATgetName(ATgetAFun(ATAgetArgument($3,1))), "SetType") == 0 ||
           strcmp(ATgetName(ATgetAFun(ATAgetArgument($1,1))), "SetType") == 0
          )
       {
         if(!ContainerTypeChecking(ATAgetArgument($1,1),  ATAgetArgument($3,1)))
	     {
           chigetposition();
		   mCRL2log(error) << "Incompatible types checking failed" << std::endl;
		   exit(1);
		 }
   		 safe_assign($$, gsMakeBinarySetExpression( $2,
				         ATAgetArgument($1,1),
                	$1, $3));
         mCRL2log(debug) << "MinusExpression - Set Expression parsed: \n  " << atermpp::aterm( $$) << "" << std::endl;
         processed = true;
       }


       if (!processed)
       {
         chigetposition();
         mCRL2log(error) << "Expressions " << atermpp::aterm(ATAgetArgument($1,0))
                         << " and " << atermpp::aterm(ATAgetArgument($3,0))
                         << " cannot be used with \"-\"" << std::endl;
         exit(1);
       }

	 }
   ;

EqualityExpression:
	  Expression IS Expression
		{
			/**
			  * Type Checking
			  *
			  **/
			if(  !(ContainerTypeChecking(ATAgetArgument($1,1),  ATAgetArgument($3,1)))
              )
				{
                  chigetposition();
				  mCRL2log(error) << "EqualityExpression: Incompatible Types Checking failed" << std::endl;
				  exit(1);
				};

            if (strcmp(ATgetName(ATgetAFun(ATAgetArgument($3,1))), "SetType") == 0 )
		    {
			  safe_assign($$, gsMakeBinarySetExpression(
                                  gsString2ATermAppl("=="),
			  		  gsMakeType( gsString2ATermAppl("Bool" )),
			  $1, $3));
      		  mCRL2log(debug) << "EqualityExpression parsed: \n  " << atermpp::aterm( $$) << "" << std::endl;
            } else {
			  safe_assign($$, gsMakeBinaryExpression(
                                  gsString2ATermAppl("=="),
			  		  gsMakeType( gsString2ATermAppl("Bool" )),
			  $1, $3));
      		  mCRL2log(debug) << "EqualityExpression parsed: \n  " << atermpp::aterm( $$) << "" << std::endl;
            }
		}
	| Expression NOTEQUAL Expression
		{
			/**
			  * Type Checking
			  *
			  **/
			if(  !(ContainerTypeChecking(ATAgetArgument($1,1),  ATAgetArgument($3,1)))
              )
				{
                  chigetposition();
				  mCRL2log(error) << "EqualityExpression: Incompatible Types Checking failed" << std::endl;
				  exit(1);
				};

            if (strcmp(ATgetName(ATgetAFun(ATAgetArgument($3,1))), "SetType") == 0 )
		    {
			  safe_assign($$, gsMakeBinarySetExpression(
                                  gsString2ATermAppl("!="),
			  		  gsMakeType( gsString2ATermAppl("Bool" )),
			  $1, $3));
      		  mCRL2log(debug) << "EqualityExpression parsed: \n  " << atermpp::aterm( $$) << "" << std::endl;
            } else {
			  safe_assign($$, gsMakeBinaryExpression( gsString2ATermAppl("!="),
			  		  gsMakeType( gsString2ATermAppl("Bool" )),
			  $1, $3));
      		  mCRL2log(debug) << "EqualityExpression parsed: \n  " << atermpp::aterm( $$) << "" << std::endl;
            }
		}
	;

/* Membertest is used for tests and sets
 */
MemberTest:
    Expression IN Expression
    {
       bool processed = false;
       if (strcmp(ATgetName(ATgetAFun(ATAgetArgument($3,1))), "SetType") == 0 )
       {
         if(!ContainerTypeChecking(gsMakeSetType(ATAgetArgument($1,1)),  ATAgetArgument($3,1)))
	     {
           chigetposition();
		   mCRL2log(error) << "Incompatible types checking failed" << std::endl;
		   exit(1);
		 }
   		 safe_assign($$, gsMakeBinarySetExpression( $2,
					gsMakeType( gsString2ATermAppl("Bool" )),
                	$1, $3));
         mCRL2log(debug) << "MemberTest - SetLiteral parsed: \n  " << atermpp::aterm( $$) << "" << std::endl;
         processed = true;
       }

       if (strcmp(ATgetName(ATgetAFun(ATAgetArgument($3,1))), "ListType") == 0 )
         {
         if(!ContainerTypeChecking(gsMakeListType(ATAgetArgument($1,1)),  ATAgetArgument($3,1)))
	       {
		     mCRL2log(error) << "Incompatible types checking failed" << std::endl;
		     exit(1);
		   }
 	  	 safe_assign($$, gsMakeBinaryListExpression( $2,
					gsMakeType( gsString2ATermAppl("Bool" )),
			        $1, $3));
      	 mCRL2log(debug) << "MemberTest - ListExpression parsed: \n  " << atermpp::aterm( $$) << "" << std::endl;
         processed = true;
         }

         if (!processed)
         {
           chigetposition();
           mCRL2log(error) << "Expressions " << atermpp::aterm(ATAgetArgument($1,0))
                          << " and " << atermpp::aterm(ATAgetArgument($3,0))
                          << " cannot be used with \"in\"" << std::endl;
           exit(1);
        }
    }
    ;

RecordExpression:
     LBRACKET Expression_csp COMMA Expression RBRACKET
     {
      	  mCRL2log(debug) << "R:" << __LINE__;
          ATermList tuple_type = ATmakeList0();
		  ATermList to_process = $2;
          to_process = ATinsert(to_process, (ATerm) $4);
		  while(!ATisEmpty(to_process))
          {
             ATerm elementType = ATgetArgument(ATgetFirst(to_process),1);
             tuple_type= ATinsert(tuple_type, elementType);

			 to_process = ATgetNext( to_process) ;
		  }
          to_process = ATinsert($2, (ATerm) $4 );
          safe_assign($$, gsMakeTupleLiteral( ATreverse( to_process ), gsMakeTupleType( tuple_type ) ) );
      	  mCRL2log(debug) << "RecordExpression parsed: \n  " << atermpp::aterm( $$) << "" << std::endl;

     }
   ;

SetExpression:
      LBRACE RBRACE
      {
          safe_assign($$, gsMakeSetLiteral( ATmakeList0(), gsMakeSetType(gsMakeType(gsMakeNil()))));
      }
    | LBRACE Expression_csp RBRACE
      {
      	  mCRL2log(debug) << "R:" << __LINE__;
          ATerm type = NULL;
		  ATermList to_process = $2;
		  while(!ATisEmpty(to_process))
          {
             ATerm elementType = ATgetArgument(ATgetFirst(to_process),1);
             if (ATgetLength(to_process) == ATgetLength($2))
             {
               type = elementType;
             }
             mCRL2log(debug) << "" << atermpp::aterm(ATgetFirst(to_process)) << "" << std::endl;
             if (type != elementType )
             {
               chigetposition();
               mCRL2log(error) << "SetLiteral contains mixed types " << atermpp::aterm(type)
                               << " and " << atermpp::aterm(elementType) << std::endl;
               exit(1);
             }
			 to_process = ATgetNext( to_process) ;
		  }
          safe_assign($$, gsMakeSetLiteral( ATreverse($2), gsMakeSetType((ATermAppl) type)));
      	  mCRL2log(debug) << "SetLiteral parsed: \n  " << atermpp::aterm( $$) << "" << std::endl;
      }
    | Expression UNION Expression
		{
	   	  /**
		    * Type Checking
		    *
		    **/
		  if(  !((ContainerTypeChecking(ATAgetArgument($1,1),  ATAgetArgument($3,1)))
             && (strcmp(ATgetName(ATgetAFun(ATAgetArgument($3,1))), "SetType") == 0 ))
             )
			{
              chigetposition();
        mCRL2log(error) << "r" << __LINE__ << ": Union failed: Incompatible Types Checking failed:" << std::endl
                        << atermpp::aterm($1) << " and " << atermpp::aterm($3) << std::endl;
			  exit(1);
			};

  		  safe_assign($$, gsMakeBinarySetExpression( $2,
				ATAgetArgument($1,1),
		        $1, $3));
   		  mCRL2log(debug) << "BinarySetExpression parsed: \n  " << atermpp::aterm( $$) << "" << std::endl;
		}
    | Expression INTERSECTION Expression
		{
	   	  /**
		    * Type Checking
		    *
		    **/
		  if(  !((ContainerTypeChecking(ATAgetArgument($1,1),  ATAgetArgument($3,1)))
             && (strcmp(ATgetName(ATgetAFun(ATAgetArgument($3,1))), "SetType") == 0 ))
             )
			{
              chigetposition();
        mCRL2log(error) << "r" << __LINE__ << ": Intersection failed: Incompatible Types Checking failed:" << std::endl
                        << atermpp::aterm($1) << " and " << atermpp::aterm($3) << std::endl;
			  exit(1);
			};

  		  safe_assign($$, gsMakeBinarySetExpression( $2,
				ATAgetArgument($1,1),
		        $1, $3));
   		  mCRL2log(debug) << "BinarySetExpression parsed: \n  " << atermpp::aterm( $$) << "" << std::endl;
		}
    | Expression SUB Expression
		{
	   	  /**
		    * Type Checking
		    *
		    **/
		  if(  !((ContainerTypeChecking(ATAgetArgument($1,1),  ATAgetArgument($3,1)))
             && (strcmp(ATgetName(ATgetAFun(ATAgetArgument($3,1))), "SetType") == 0 ))
             )
			{
              chigetposition();
			  mCRL2log(error) << "r" << __LINE__ << ": Subsection failed: Incompatible Types Checking failed:" << std::endl
                        << atermpp::aterm($1) << " and " << atermpp::aterm($3) << std::endl;
			  exit(1);
			};

  		  safe_assign($$, gsMakeBinarySetExpression( $2,
				gsMakeType( gsString2ATermAppl("Bool" ) ),
		        $1, $3));
   		  mCRL2log(debug) << "BinarySetExpression parsed: \n  " << atermpp::aterm( $$) << "" << std::endl;
		}
    | PICK LBRACKET Expression RBRACKET
      {
            mCRL2log(debug) << "R:" << __LINE__ << "" << std::endl;
			if(!(strcmp(ATgetName(ATgetAFun(ATAgetArgument($3,1))), "ListType") == 0 ))
				{
                  chigetposition();
				  mCRL2log(error) << "functions: " << atermpp::aterm($1) << " cannot be used on " << atermpp::aterm($3) << std::endl;
				  exit(1);
				};

 	  		safe_assign($$, gsMakeFunction( $1,
			     (ATermAppl) ATgetArgument($3,1),
			$3));
      		mCRL2log(debug) << "Functions parsed: \n  " << atermpp::aterm( $$) << "" << std::endl;
      }
    ;

ListExpression:
      ListLiteral
      {
        safe_assign($$, $$);
      }
    | Expression CONCAT Expression
		{
			/**
			  * Type Checking
			  *
			  **/
			if(  !((ContainerTypeChecking(ATAgetArgument($1,1),  ATAgetArgument($3,1)))
              && (strcmp(ATgetName(ATgetAFun(ATAgetArgument($3,1))), "ListType") == 0 ))
              )
				{
                  chigetposition();
          mCRL2log(error) << "r" << __LINE__ << ": Concatenation failed: Incompatible Types Checking failed:" << std::endl
                        << atermpp::aterm($1) << " and " << atermpp::aterm($3) << std::endl;
				  exit(1);
				};

 	  		safe_assign($$, gsMakeBinaryListExpression( $2,
					ATAgetArgument($1,1),
			$1, $3));
      		mCRL2log(debug) << "ListExpression parsed: \n  " << atermpp::aterm( $$) << "" << std::endl;
		}
    | Expression LSUBTRACT Expression
		{
			/**
			  * Type Checking
			  *
			  **/
			if(  !((ContainerTypeChecking(ATAgetArgument($1,1),  ATAgetArgument($3,1)))
              && (strcmp(ATgetName(ATgetAFun(ATAgetArgument($3,1))), "ListType") == 0 ))
              )
				{
                  chigetposition();
				  mCRL2log(error) << "Incompatible types checking failed" << std::endl;
				  exit(1);
				};

 	  		safe_assign($$, gsMakeBinaryListExpression( $2,
					ATAgetArgument($1,1),
			$1, $3));
      		mCRL2log(debug) << "ListExpression parsed: \n  " << atermpp::aterm( $$) << "" << std::endl;
		}
      // Equality and NOTequal handled by BoolNatIntExpression
/*  | ListLiteral IS ListLiteral
    | ListLiteral NOTEQUAL ListLiteral
}*/
    | Functions
    ;

Functions:
      LENGTH LBRACKET  Expression RBRACKET
      {
			if(!(strcmp(ATgetName(ATgetAFun(ATAgetArgument($3,1))), "ListType") == 0 ))
				{
                  chigetposition();
				  mCRL2log(error) << "functions: " << atermpp::aterm($1) << " cannot be used on " << atermpp::aterm($3) << std::endl;
				  exit(1);
				};

 	  		safe_assign($$, gsMakeFunction( $1,
					gsMakeType( gsString2ATermAppl("Nat" )),
			$3));
      		mCRL2log(debug) << "Functions parsed: \n  " << atermpp::aterm( $$) << "" << std::endl;
      }
    | HEAD LBRACKET  Expression RBRACKET
      {
			if(!(strcmp(ATgetName(ATgetAFun(ATAgetArgument($3,1))), "ListType") == 0 ))
				{
                  chigetposition();
				  mCRL2log(error) << "functions: " << atermpp::aterm($1) << " cannot be used on " << atermpp::aterm($3) << std::endl;
				  exit(1);
				};

 	  		safe_assign($$, gsMakeFunction( $1,
				  (ATermAppl) ATgetArgument(ATgetArgument($3,1),0),
			$3));
      		mCRL2log(debug) << "Functions parsed: \n  " << atermpp::aterm( $$) << "" << std::endl;
      }
    | TAIL LBRACKET  Expression RBRACKET
      {
            mCRL2log(debug) << "R:" << __LINE__ << "" << std::endl;
			if(!(strcmp(ATgetName(ATgetAFun(ATAgetArgument($3,1))), "ListType") == 0 ))
				{
                  chigetposition();
				  mCRL2log(error) << "functions: " << atermpp::aterm($1) << " cannot be used on " << atermpp::aterm($3) << std::endl;
				  exit(1);
				};

 	  		safe_assign($$, gsMakeFunction( $1,
			     (ATermAppl) ATgetArgument($3,1),
			$3));
      		mCRL2log(debug) << "Functions parsed: \n  " << atermpp::aterm( $$) << "" << std::endl;
      }
    | RHEAD LBRACKET  Expression RBRACKET
      {
            mCRL2log(debug) << "R:" << __LINE__ << "" << std::endl;
			if(!(strcmp(ATgetName(ATgetAFun(ATAgetArgument($3,1))), "ListType") == 0 ))
				{
                  chigetposition();
				  mCRL2log(error) << "functions: " << atermpp::aterm($1) << " cannot be used on " << atermpp::aterm($3) << std::endl;
				  exit(1);
				};

 	  		safe_assign($$, gsMakeFunction( $1,
				  (ATermAppl) ATgetArgument(ATgetArgument($3,1),0),
			$3));
      		mCRL2log(debug) << "Functions parsed: \n  " << atermpp::aterm( $$) << "" << std::endl;
      }
    | RTAIL LBRACKET  Expression RBRACKET
      {
            mCRL2log(debug) << "R:" << __LINE__ << "" << std::endl;
			if(!(strcmp(ATgetName(ATgetAFun(ATAgetArgument($3,1))), "ListType") == 0 ))
				{
                  chigetposition();
				  mCRL2log(error) << "functions: " << atermpp::aterm($1) << " cannot be used on " << atermpp::aterm($3) << std::endl;
				  exit(1);
				};

 	  		safe_assign($$, gsMakeFunction( $1,
			       (ATermAppl) ATgetArgument($3,1),
			$3));
      		mCRL2log(debug) << "Functions parsed: \n  " << atermpp::aterm( $$) << "" << std::endl;
      }
    | TAKE LBRACKET Expression COMMA Expression RBRACKET
      {
            mCRL2log(debug) << "R:" << __LINE__ << "" << std::endl;
			if(!(strcmp(ATgetName(ATgetAFun(ATAgetArgument($3,1))), "ListType") == 0 ))
				{
                  chigetposition();
				  mCRL2log(error) << "Functions: " << atermpp::aterm($1) << " cannot bet used on " << atermpp::aterm($3) << std::endl;
				  exit(1);
				};

			if(! (ATAgetArgument($5,1) == gsMakeType( gsString2ATermAppl("Nat" ) ) ) )
				{
                  chigetposition();
				  mCRL2log(error) << "Functions: " << atermpp::aterm($1) << " cannot be used on 2nd argument " << atermpp::aterm($5) << std::endl;
				  mCRL2log(error) << "Type checking failed" << std::endl;
				  exit(1);
				};

 	  		safe_assign($$, gsMakeFunction2( $1,
			       (ATermAppl) ATgetArgument($3,1),
			       $3,
                   $5));
      		mCRL2log(debug) << "Functions parsed: \n" << atermpp::aterm( $$) << "" << std::endl;
      }
    | DROP LBRACKET  Expression COMMA Expression RBRACKET
      {
            mCRL2log(debug) << "R:" << __LINE__ << "" << std::endl;
			if(!(strcmp(ATgetName(ATgetAFun(ATAgetArgument($3,1))), "ListType") == 0 ))
				{
                  chigetposition();
				  mCRL2log(error) << "Functions: " << atermpp::aterm($1) << " cannot bet used on " << atermpp::aterm($3) << std::endl;
          exit(1);
				};

			if(! (ATAgetArgument($5,1) == gsMakeType( gsString2ATermAppl("Nat" ) ) ) )
				{
                  chigetposition();
				  mCRL2log(error) << "Functions: " << atermpp::aterm($1) << " cannot be used on 2nd argument " << atermpp::aterm($5) << std::endl;
          mCRL2log(error) << "Type checking failed" << std::endl;
          exit(1);
				};

 	  		safe_assign($$, gsMakeFunction2( $1,
			       (ATermAppl) ATgetArgument($3,1),
			       $3,
                   $5));
      		mCRL2log(debug) << "Functions parsed: \n" << atermpp::aterm( $$) << "" << std::endl;
      }
    /* List functions that are not supported */
    | SORT LBRACKET  Expression RBRACKET
      {
            mCRL2log(debug) << "R:" << __LINE__ << "" << std::endl;
			if(!(strcmp(ATgetName(ATgetAFun(ATAgetArgument($3,1))), "ListType") == 0 ))
				{
				  mCRL2log(error) << "functions: " << atermpp::aterm($1) << " cannot be used on " << atermpp::aterm($3) << std::endl;
				  exit(1);
				};

 	  		safe_assign($$, gsMakeFunction( $1,
			       (ATermAppl) ATgetArgument($3,1),
			$3));
      		mCRL2log(debug) << "Functions parsed: \n  " << atermpp::aterm( $$) << "" << std::endl;
      }
    | INSERT LBRACKET  Expression RBRACKET
      {
        chigetposition();
        mCRL2log(error) << atermpp::aterm($1) << " is not supported" << std::endl;
        exit(1);
      }
    ;

ListLiteral:
    SQLBRACKET SQRBRACKET
      {
          safe_assign($$, gsMakeListLiteral( ATmakeList0(), gsMakeListType(gsMakeType(gsMakeNil()))));
      }
  | SQLBRACKET Expression_csp SQRBRACKET
      {
      	  mCRL2log(debug) << "Entering ListLiteral" << std::endl;
          ATerm type = NULL;
		  ATermList to_process = $2;
		  while(!ATisEmpty(to_process))
          {
             ATerm elementType = ATgetArgument(ATgetFirst(to_process),1);
             if (ATgetLength(to_process) == ATgetLength($2))
             {
               type = elementType;
             }
             mCRL2log(debug) << "" << atermpp::aterm(ATgetFirst(to_process)) << "" << std::endl;
             if (type != elementType )
             {
               chigetposition();
               mCRL2log(error) << "ListLiteral contains mixed types " <<
                        atermpp::aterm(type) << " and " << atermpp::aterm(elementType) << std::endl;
               exit(1);
             }
			 to_process = ATgetNext( to_process) ;
		  }
          safe_assign($$, gsMakeListLiteral( ATreverse($2), gsMakeListType((ATermAppl) type)));
      	  mCRL2log(debug) << "ListLiteral parsed: \n  " << atermpp::aterm( $$) << "" << std::endl;
      }
;

/* Precedence for '+','-' vs '*',  '/' is NOT defined here.
 * Instead, the two %left commands above perform that role.
 */
/*
 exp: exp '+' exp        	{ }
    | exp '-' exp        	{ }
    | exp '*' exp        	{ }
    | exp '/' exp        	{ }
    | MINUS exp  %prec NEG 	{ }
    | PLUS exp           	{ }
    | exp '^' exp        	{ }
    | '(' exp ')'        	{ }
    | NUM				 	{ }
    | REAL				 	{ }
	| ID		     	 	{ }
//  Boolean
    | exp AND exp			{ }
	| exp OR exp       		{ }
	| exp BARS exp     		{ }
	| exp GUARD exp 		{ }
	| exp IS exp			{ }
	| NOT exp				{ }
	| TRUE					{ }
    | FALSE					{ }
; */
%%

void BinTypeCheck(ATermAppl arg1, ATermAppl arg2, std::string type)
{
    if(arg1 != arg2)
        {
          chigetposition();
          mCRL2log(error) << "BinTypeCheck: Incompatible Types Checking failed" << std::endl;
          exit(1);
        };
    if(arg1 != gsMakeType(gsString2ATermAppl(type.c_str())))
        {
          chigetposition();
          mCRL2log(error) << "Expected type " << type << std::endl;
          exit(1);
        };
  return;
}

void UnaryTypeCheck(ATermAppl arg1, std::string type)
{
    mCRL2log(debug) << __FILE__ << ";" << __LINE__ << std::endl;
    mCRL2log(debug) << "arg1: " << atermpp::aterm( arg1) << "" << std::endl;
    ATermAppl arg2 = gsMakeType(gsString2ATermAppl(type.c_str()));
    mCRL2log(debug) << "arg2: " << atermpp::aterm( arg2) << "" << std::endl;

    if( arg1 != arg2 )
        {
          chigetposition();
          mCRL2log(error) << "UnaryTypeCheck: Incompatible Type, expected " << type << std::endl;
          exit(1);
        };
  return;
}

bool ContainerTypeChecking(ATermAppl arg1, ATermAppl arg2)
{
  mCRL2log(debug) << __FILE__ << ";" << __LINE__ << std::endl;
  mCRL2log(debug) << "arg1: " << atermpp::aterm( arg1) << "" << std::endl;
  mCRL2log(debug) << "arg2: " << atermpp::aterm( arg2) << "" << std::endl;
  if(arg1 == arg2)
  {
    return true;
  }

  mCRL2log(debug) << "ContainerTypeChecking: " << atermpp::aterm(arg1) << ", " << atermpp::aterm( arg2) << std::endl;
  if((strcmp(ATgetName(ATgetAFun(arg1)), ATgetName(ATgetAFun(arg2)))==0)
     && ( ( strcmp(ATgetName(ATgetAFun(arg1)), "ListType") == 0 ) ||
          ( strcmp(ATgetName(ATgetAFun(arg1)), "SetType") == 0  )
        )

    )
    {
      if(((ATermAppl) ATgetArgument(arg2,0) == gsMakeType(gsMakeNil())) ||
         ((ATermAppl) ATgetArgument(arg1,0) == gsMakeType(gsMakeNil()))
        )
      {
        return true;
      }
      return ContainerTypeChecking((ATermAppl) ATgetArgument(arg1,0), ATermAppl (ATgetArgument(arg2,0)));
    }

  if(  (strcmp(ATgetName(ATgetAFun(arg1)),ATgetName(ATgetAFun(arg2)))==0)
    && (strcmp(ATgetName(ATgetAFun(arg1)), "Type") == 0 )
    )
  {
    if(arg1 == arg2)
    {
      return true;
    } else {
      return false;
    }
  }
  return false;
}

bool is_number(std::string s)
{
   for (size_t i = 0; i < (size_t) s.length(); i++) {
       if (!std::isdigit(s[i]))
           return false;
   }

   return true;
}
