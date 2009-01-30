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
#include "aterm2.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/aterm_ext.h"
#include <list>
#include <map>
#include <set>
#include <utility>
#include <cctype>
#include <cstring>


/*extern int yyerror(const char *s);
extern int yylex( void );
extern char* yytext; */

using namespace mcrl2::core;
using namespace std;

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

#define safe_assign(lhs, rhs) { ATbool b; ATindexedSetPut(chi_parser_protect_table, (ATerm) rhs, &b); lhs = rhs; }

void BinTypeCheck(ATermAppl arg1, ATermAppl arg2, std::string type);
void UnaryTypeCheck(ATermAppl arg1, std::string type);
bool ContainerTypeChecking(ATermAppl arg1, ATermAppl arg2);
bool is_number(std::string s);

%}

%union {
  ATermAppl appl;
  ATermList list;
};

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
          gsDebugMsg("%s;%d\n",__FILE__,__LINE__);
    	  gsDebugMsg("inputs contains a valid Chi-specification\n"); 
          safe_assign($$, gsMakeChiSpec($1,ATreverse($2)));
		  chi_spec_tree = $$;
		}
	;

ModelDefinition:
      MODEL Identifier LBRACKET RBRACKET IS ModelBody
        {
      	  safe_assign($$, gsMakeModelDef($2, $6));
      	  gsDebugMsg("parsed Model Def \n  %T\n", $$);
        }
      ;

ModelBody:
      BP ModelStatement ModelCloseScope
      	{ safe_assign($$, gsMakeModelSpec( ATmakeList0(), $2 ));
      	  gsDebugMsg("parsed Model Body  \n  %T\n", $$);	
		}
	| BP LocalVariables_csp PROC_SEP ModelStatement ModelCloseScope
      	{ safe_assign($$, gsMakeModelSpec( $2, $4));
      	  gsDebugMsg("parsed Model Body  \n  %T\n", $$);	
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
      	  gsDebugMsg("parsed Process Definition \n  %T\n", $$);	
		}
     | ProcessDefinitions ProcessDefinition
      	{ safe_assign($$, ATinsert($1, (ATerm) $2));
      	  gsDebugMsg("parsed Process Definition \n  %T\n", $$);	
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
            gsErrorMsg("Duplicate definition for process %T", $2);
            exit(1);
          } else {
            used_process_identifiers.insert($2);
          }
         
      	  safe_assign($$, gsMakeProcDef($2, gsMakeProcDecl(ATmakeList0()) ,$6));
      	  gsDebugMsg("parsed proc Def \n  %T\n", $$);
		}
	| ProcOpenScope Identifier LBRACKET FormalParameter_csp RBRACKET IS ProcessBody
		{ 
      	  safe_assign($$, gsMakeProcDef($2, gsMakeProcDecl( ATreverse($4)), $7));
      	  gsDebugMsg("parsed proc Def\n  %T\n", $$);
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
		  gsDebugMsg("Increase Scope to: %d\n",scope_lvl);
		}
	;

ProcessBody: 
	  BP Statement ProcCloseScope
      	{ safe_assign($$, gsMakeProcSpec( ATmakeList0(), $2 ));
      	  gsDebugMsg("parsed ProcessBody  \n  %T\n", $$);	
		}
	| BP LocalVariables_csp PROC_SEP Statement ProcCloseScope
      	{ safe_assign($$, gsMakeProcSpec( ATreverse($2), $4));
      	  gsDebugMsg("parsed ProcessBody  \n  %T\n", $$);	
		}
	;

ProcCloseScope:
	EP	
		{
		  assert(scope_lvl > 0);
		  scope_lvl--;
		  gsDebugMsg("Decrease Scope to; %d\n",scope_lvl);
          var_type_map.clear();
          chan_type_direction_map.clear();
		}
	;

Identifier: ID
		{ 
 	  	  safe_assign($$, $1 );
      	  gsDebugMsg("parsed id's\n  %T\n", $$);
		}

LocalVariables_csp: 
	  LocalVariables 
      	{ safe_assign($$, ATmakeList1((ATerm) $1));
      	  gsDebugMsg("LocalVariables_csp: parsed \n  %T\n", $$);	
		}
	| LocalVariables_csp COMMA LocalVariables
      	{ safe_assign($$, ATinsert($1, (ATerm) $3));
      	  gsDebugMsg("LocalVariables_csp: parsed \n  %T\n", $$);	
		}
	;

LocalVariables: 
	  VAR IdentifierTypeExpression_csp
		{
		  safe_assign($$, gsMakeVarSpec( $2 ) );
		  gsDebugMsg("LocalVariables: parsed \n %T\n", $$);
		}
	| CHAN ChannelDefinition_csp 
		{
		  //safe_assign($$, gsMakeVarSpec( ATreverse( $2 ) ) );  //<-- gsMakeVarSpec aanpassen
		  gsDebugMsg("LocalVariables: parsed \n %T\n", $$);
		}
    | VAL
//	| RecursionDefinition
	;

IdentifierTypeExpression_csp:
	  IdentifierTypeExpression
      	{ safe_assign($$, $1);
		  gsDebugMsg("IdentifierTypeExpression_csp: parsed \n %T\n", $$);
		}
	| IdentifierTypeExpression_csp COMMA IdentifierTypeExpression 
      	{ 
          ATermList new_list = $3;
          ATermList list = ATreverse($1);
          while (!ATisEmpty(list)) 
          {
             gsDebugMsg("%T",ATgetFirst(list));
             new_list = ATinsert( new_list , ATgetFirst(list));
             list = ATgetNext( list ) ;
          }
          
          safe_assign($$, new_list);
		  gsDebugMsg("IdentifierTypeExpression_csp: parsed \n %T\n", $$);
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
              gsErrorMsg("Expression %T can not be of type \"void\"\n", ATgetFirst(list));
              exit(1);
            } 
            list = ATgetNext( list ) ;
          }

		  safe_assign($$, $1 );
		  gsDebugMsg("IdentifierTypeExpression: parsed \n %T\n", $$);
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
              gsErrorMsg("Expression %T can not be of type \"void\"\n", ATgetFirst(list));
              exit(1);
            } 
            new_list = ATinsert( new_list , (ATerm) gsMakeDataVarExprID( (ATermAppl) ATgetFirst(list), $3));
            list = ATgetNext( list ) ;
          }
          
          safe_assign($$, new_list);

		  //safe_assign($$, gsMakeDataVarExprID ( $1, $3 ) );
		  gsDebugMsg("IdentifierTypeExpression: parsed \n %T\n", $$);
		}
	;

IdentifierType_csp:
	  IdentifierType
      	{ safe_assign($$, $1);
		  gsDebugMsg("IdentifierType_csp: parsed \n %T\n", $$);
		}
	| IdentifierType_csp COMMA IdentifierType
      	{
          ATermList new_list = $1;
          ATermList list = ATreverse($3);
          while (!ATisEmpty(list)) 
          {
             gsDebugMsg("%T",ATgetFirst(list));
             new_list = ATinsert( new_list , ATgetFirst(list));
             list = ATgetNext( list ) ;
          }
          
          safe_assign($$, new_list);
		  gsDebugMsg("IdentifierType_csp: parsed \n %T\n", $$);
		}
	;

FormalParameter_csp: 
	  FormalParameter 
      	{ safe_assign($$, ATmakeList1((ATerm) $1));
		  gsDebugMsg("FormalParameter_csp: parsed \n %T\n", $$);
		}
	| FormalParameter_csp COMMA FormalParameter
      	{ safe_assign($$, ATinsert($1, (ATerm) $3));
		  gsDebugMsg("FormalParameter_csp: parsed \n %T\n", $$);
		}
	;

FormalParameter:
	  VAR IdentifierType_csp
		{
		  safe_assign($$, gsMakeVarDecl( ATreverse($2) ) );
		  gsDebugMsg("FormalParameter: parsed \n %T\n", $$);
		}
  	| CHAN ChannelDeclaration_csp
		{
		  safe_assign($$, gsMakeChanDecl( ATreverse($2) ) );
		  gsDebugMsg("FormalParameter: parsed \n %T\n", $$);
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
		  int n = ATgetLength( list );
		  for(int i = 0; i < n ; ++i )
          {
			if (chan_type_direction_map.end() != chan_type_direction_map.find(ATgetFirst( list )))
			{
              chigetposition();
			  gsErrorMsg("Channel %T is already defined!\n", ATgetFirst( list ));
			  exit(1);
			};
			list = ATgetTail( list, 1 ) ;
		  }	;

		  list = $1;
		  n = ATgetLength( list );
		  for(int i = 0; i < n ; ++i ){
			if (var_type_map.end() != var_type_map.find(ATgetFirst( list )))
			{
              chigetposition();
			  gsErrorMsg("Variable %T is already defined!\n", ATgetFirst( list ));
			  exit(1);
			};
			var_type_map[ATgetFirst( list )]= (ATerm) $3;
			list = ATgetTail( list, 1 ) ;
		  }	;
         
          list = $1;
          ATermList new_list = ATmakeList0();

          while (!ATisEmpty(list)) 
          {
             gsDebugMsg("%T",ATgetFirst(list));
             new_list = ATinsert( new_list , (ATerm) gsMakeDataVarID( (ATermAppl) ATgetFirst(list), $3));
             list = ATgetNext( list ) ;
          }
          
          safe_assign($$, ATreverse(new_list));
		  gsDebugMsg("IdentifierType: parsed \n %T\n", $$);
		  gsDebugMsg("Typecheck Table %d\n", var_type_map.size()); 
  		}
	;

ChannelDeclaration_csp:
	  ChannelDeclaration
      	{ safe_assign($$, $1 );
      	  gsDebugMsg("ChannelDeclaration_csp: parsed formalparameter channel  \n  %T\n", $$);	
		}
	| ChannelDeclaration_csp COMMA ChannelDeclaration
      	{ 
          ATermList new_list = $1;
          ATermList list = ATreverse($3);
          while (!ATisEmpty(list)) 
          {
             gsDebugMsg("%T",ATgetFirst(list));
             new_list = ATinsert( new_list , ATgetFirst(list));
             list = ATgetNext( list ) ;
          }
          safe_assign($$, new_list);
      	  gsDebugMsg("ChannelDeclaration_csp: parsed formalparameter channel \n  %T\n", $$);	
		}
	;

ChannelDeclaration:
	  IdentifierChannelDeclaration_csp COLON Type
		{
          gsDebugMsg("%s;%d\n",__FILE__,__LINE__);
		  ATermList list = $1;
		  int n = ATgetLength( list );
		  for(int i = 0; i < n ; ++i ){
			if (var_type_map.end() != var_type_map.find(ATgetArgument( ATgetFirst( list ),0)))
			{
              chigetposition();
			  gsErrorMsg("Variable %T is already defined!\n", ATgetFirst( list ));
			  exit(1);
			};
			list = ATgetTail( list, 1 ) ;
		  }	;
          gsDebugMsg("\n%T\n", $1);
		  
          list = $1;
		  n = ATgetLength( list );
		  for(int i = 0; i < n ; ++i ){
			if (chan_type_direction_map.end() != chan_type_direction_map.find(ATgetArgument( ATgetFirst( list ),0)))
			{
              chigetposition();
			  gsErrorMsg("Channel %T is already defined!\n", ATgetFirst( list ));
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
		  gsDebugMsg("ChannelDefinition: parsed VariableList \n %T\n", $$);
		}
	| IdentifierChannelDeclaration_csp COLON Expression HASH Type
		{
          gsDebugMsg("%s;%d\n",__FILE__,__LINE__);

          //Make sure that Expression is a number 
          UnaryTypeCheck( (ATermAppl) ATgetArgument($3,1), "Nat");
        
		  ATermList list = $1;
		  int n = ATgetLength( list );
		  for(int i = 0; i < n ; ++i ){
			if (var_type_map.end() != var_type_map.find(ATgetArgument( ATgetFirst( list ),0)))
			{
              chigetposition();
			  gsErrorMsg("Variable %T is already defined!\n", ATgetFirst( list ));
			  exit(1);
			};
			list = ATgetTail( list, 1 ) ;
		  }	;
		  
          list = $1;
		  n = ATgetLength( list );
		  for(int i = 0; i < n ; ++i ){
			if (chan_type_direction_map.end() != chan_type_direction_map.find(ATgetArgument( ATgetFirst( list ),0)))
			{
              chigetposition();
			  gsErrorMsg("Channel %T is already defined!\n", ATgetFirst( list ));
			  exit(1);
			};
			chan_type_direction_map[ATgetArgument(ATgetFirst( list ), 0)]=  make_pair( (ATerm) $5, ATgetArgument(ATgetFirst( list ), 1) );
			list = ATgetTail( list, 1 ) ;
		  }	;

		  list = $1;
          ATermList new_list = ATmakeList0();
		  while(!ATisEmpty(list))
          {
             gsDebugMsg("%T",ATgetFirst(list));
             new_list = ATinsert(new_list,(ATerm) gsMakeChannelTypedID( (ATermAppl) ATgetFirst(list), $5, $3 ) );
			 list = ATgetNext( list ) ;
		  }

		  safe_assign($$, new_list );
		  gsDebugMsg("ChannelDefinition: parsed VariableList \n %T\n", $$);
		}
	;
	
IdentifierChannelDeclaration_csp:
	  IdentifierChannelDeclaration
      	{ safe_assign($$, ATmakeList1((ATerm) $1));
      	  gsDebugMsg("IdentifierChannelDeclaration_csp: parsed formalparameter channel  \n  %T\n", $$);	
		}
	| IdentifierChannelDeclaration_csp COMMA IdentifierChannelDeclaration
      	{ safe_assign($$, ATinsert($1, (ATerm) $3));
      	  gsDebugMsg("IdentifierChannelDeclaration_csp: parsed formalparameter channel \n  %T\n", $$);	
		}
	;

IdentifierChannelDeclaration: 
	  Identifier RECV
        {
          safe_assign($$, gsMakeChannelID($1, gsMakeRecv()));
		  gsDebugMsg("IdentifierChannelDeclaration: parsed Identifier Type With Expression \n %T\n", $$);
        } 
	| Identifier EXCLAMATION
        {
          safe_assign($$, gsMakeChannelID($1, gsMakeSend()));
		  gsDebugMsg("IdentifierChannelDeclaration: parsed Identifier Type With Expression \n %T\n", $$);
        } 
/*	| Identifier SENDRECV
        {
          safe_assign($$, gsMakeChannelID($1, $2));
		  gsDebugMsg("IdentifierChannelDeclaration: parsed Identifier Type With Expression \n %T\n", $$);
        } 
	| Identifier RECVSEND
        {
          safe_assign($$, gsMakeChannelID($1, $2));
		  gsDebugMsg("IdentifierChannelDeclaration: parsed Identifier Type With Expression \n %T\n", $$);
        } */
	;

ChannelDefinition_csp:
	  ChannelDefinition
      	{ //safe_assign($$, ATmakeList1((ATerm) $1));
      	  gsDebugMsg("ChannelDefinition_csp: parsed \n  %T\n", $$);	
		}
	| ChannelDefinition_csp COMMA ChannelDefinition
      	{ //safe_assign($$, ATinsert($1, (ATerm) $3));
      	  gsDebugMsg("ChannelDefinition_csp: parsed \n  %T\n", $$);	
		}
	;

ChannelDefinition:
	  IdentifierChannelDefinition_csp COLON Type
		{
          gsDebugMsg("%s;%d\n",__FILE__,__LINE__);
          gsDebugMsg("ChannelDefinition\n");
		  ATermList list = $1;
		  int n = ATgetLength( list );
		  for(int i = 0; i < n ; ++i ){
			if (var_type_map.end() != var_type_map.find(ATgetArgument( ATgetFirst( list ),0)))
			{
              chigetposition();
			  gsErrorMsg("Variable %T is already defined!\n", ATgetFirst( list ));
			  exit(1);
			};
			list = ATgetNext( list ) ;
		  }	;
		  
          list = $1;
		  n = ATgetLength( list );
		  for(int i = 0; i < n ; ++i ){
			if (chan_type_direction_map.end() != chan_type_direction_map.find(ATgetArgument( ATgetFirst( list ),0)))
			{
              chigetposition();
			  gsErrorMsg("Channel %T is already defined!\n", ATgetFirst( list ));
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
		  gsDebugMsg("ChannelDefinition: parsed VariableList \n %T\n", $$);
		}
	| IdentifierChannelDefinition_csp COLON Expression HASH Type
		{
          gsDebugMsg("%s;%d\n",__FILE__,__LINE__);

          //Make sure that Expression is a number 
          UnaryTypeCheck( (ATermAppl) ATgetArgument($3,1), "Nat");
        
		  ATermList list = $1;
		  int n = ATgetLength( list );
		  for(int i = 0; i < n ; ++i ){
			if (var_type_map.end() != var_type_map.find(ATgetArgument( ATgetFirst( list ),0)))
			{
              chigetposition();
			  gsErrorMsg("Variable %T is already defined!\n", ATgetFirst( list ));
			  exit(1);
			};
			list = ATgetNext( list ) ;
		  }	;
		  
          list = $1;
		  n = ATgetLength( list );
		  for(int i = 0; i < n ; ++i ){
			if (chan_type_direction_map.end() != chan_type_direction_map.find(ATgetArgument( ATgetFirst( list ),0)))
			{
              chigetposition();
			  gsErrorMsg("Channel %T is already defined!\n", ATgetFirst( list ));
			  exit(1);
			};
			chan_type_direction_map[ATgetArgument(ATgetFirst( list ), 0)]=  make_pair( (ATerm) $5, ATgetArgument(ATgetFirst( list ), 1) );
			list = ATgetNext( list ) ;
		  }	;

		  list = $1;
          ATermList new_list = ATmakeList0();
		  while(!ATisEmpty(list))
          {
             gsDebugMsg("%T",ATgetFirst(list));
             new_list = ATinsert(new_list,(ATerm) gsMakeChannelTypedID( (ATermAppl) ATgetFirst(list), $5, $3 ) );
			 list = ATgetNext( list ) ;
		  }

		  safe_assign($$, new_list );
		  gsDebugMsg("ChannelDefinition: parsed VariableList \n %T\n", $$);
		}
	;
			
IdentifierChannelDefinition_csp:
	  IdentifierChannelDefinition
      	{ safe_assign($$, ATmakeList1((ATerm) $1));
      	  gsDebugMsg("IdentifierChannelDefinition_csp: parsed \n  %T\n", $$);	
		}
	| IdentifierChannelDefinition_csp COMMA IdentifierChannelDefinition
      	{ safe_assign($$, ATinsert($1, (ATerm) $3));
      	  gsDebugMsg("IdentifierChannelDefinition_csp: parsed \n  %T\n", $$);	
		}
	;
	  
IdentifierChannelDefinition:
	  Identifier
        {
          safe_assign($$, gsMakeChannelID($1, gsMakeNil()));
		  gsDebugMsg("IdentifierChannelDefinition: parsed \n %T\n", $$);
        } 
/*	| Identifier SENDRECV
		{
          safe_assign($$, gsMakeChannelID($1, gsMakeNil));
		  gsDebugMsg("IdentifierChannelDefinition: parsed Identifier Type With Expression \n %T\n", $$);
		}
	| Identifier RECVSEND
		{
          safe_assign($$, gsMakeChannelID($1, gsMakeNil));
		  gsDebugMsg("IdentifierChannelDefinition: parsed Identifier Type With Expression \n %T\n", $$);
		}*/
	;


Type: 
	  BasicType
	| LBRACKET Type RBRACKET
		{ 
          safe_assign($$, $2  );
      	  gsDebugMsg("Type: parsed Type \n  %T\n", $$);
		}
       
	| ContainerType
/*	| FunctionType
	| DistributionType */
	;

BasicType:
 	  BOOL 
		{ 
          safe_assign($$, gsMakeType( gsString2ATermAppl( "Bool" ) ) );
      	  gsDebugMsg("BasicType: parsed Type \n  %T\n", $$);
		}
 	| NAT
		{ 
          safe_assign($$, gsMakeType( gsString2ATermAppl("Nat" ) ) );
      	  gsDebugMsg("BasicType: parsed Type \n  %T\n", $$);
		}
 	| VOID 
		{ 
          safe_assign($$, gsMakeType( gsString2ATermAppl("Void" ) ) );
      	  gsDebugMsg("BasicType: parsed Type \n  %T\n", $$);
		}
    | REAL
 	| TYPE
		{ 
          safe_assign($$, gsMakeType( $1 ) );
      	  gsDebugMsg("BasicType: parsed Type \n  %T\n", $$);
		}
//	| Identifier
//	| Identifier DOT Identier
	;

ContainerType:
      SQLBRACKET Type SQRBRACKET
        {
          safe_assign($$, gsMakeListType($2));
      	  gsDebugMsg("ContainerType: parsed Type \n  %T\n", $$);

        }
    | LBRACE Type RBRACE
        {
          safe_assign($$, gsMakeSetType($2));
      	  gsDebugMsg("ContainerType: parsed Type \n  %T\n", $$);

        }
    | LBRACKET Type_csp COMMA Type RBRACKET
		{ 
          ATermList list = ATinsert( $2, (ATerm) $4 ); 
          safe_assign($$, gsMakeTupleType(ATreverse(list)));
      	  gsDebugMsg("ContainerType: parsed Type \n  %T\n", $$);
		}
    ;

Type_csp:
      Type
      	{ 
          safe_assign($$, ATmakeList1( (ATerm) $1) );
      	  gsDebugMsg("ChannelDeclaration_csp: parsed formalparameter channel  \n  %T\n", $$);	
		}
    | Type_csp COMMA Type
		{ 
          safe_assign($$, ATinsert( $1, (ATerm) $3 ) );
      	  gsDebugMsg("BasicType: parsed Type \n  %T\n", $$);
		}
    ; 

/**
  * STATEMENTS
  *
  **/

Statement: 
	  LBRACKET Statement RBRACKET
      	{ safe_assign($$, gsMakeParenthesisedStat( $2));
      	  gsDebugMsg("Statement: parsed \n  %T\n", $$);	
		}
	| BasicStatement
	| UnaryStatement
	| BinaryStatement
	; 

ModelStatement:
      Instantiation
    | ModelStatement BARS ModelStatement 
      	{ safe_assign($$, gsMakeParStat( $1, $3));
      	  gsDebugMsg("ModelStatement: parsed \n  %T\n", $$);	
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
        gsDebugMsg("AssignmentStatement: parsed \n  %T\n", $$);	
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
              gsErrorMsg("Assignment failed: Incompatible Types Checking failed %T and %T\n", ids, exprs);
		      exit(1);
            }
            ids = ATgetNext(ids);
            exprs = ATgetNext(exprs);
          }  
 
          safe_assign($$, gsMakeAssignmentStat($1, $2, ATreverse($3), ATreverse($5) ) );
      	  gsDebugMsg("AssignmentStatement: parsed \n  %T\n", $$);	
		}
	|
	  OptGuard OptChannel SKIP
      	{ 
          safe_assign($$, gsMakeSkipStat( $1, $2, gsMakeSkip() ));
      	  gsDebugMsg("AssignmentStatement: parsed \n  %T\n", $$);	
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
				  gsErrorMsg("OptGaurd failed: Incompatible Types Checking failed\n");
				  exit(1);
				};


			safe_assign($$, $1 );
      	  	gsDebugMsg("OptGuard: parsed \n  %T\n", $$);	
		}
	;

OptChannel: /* empty */
     	{ safe_assign($$, gsMakeNil() );
		}
	| Expression COLON
      	{ safe_assign($$, $1);
		  gsErrorMsg("OptChannel not yet implemented");
		  assert(false);
      	  gsDebugMsg("OptChannel: parsed \n  %T\n", $$);	
		}
	;

Identifier_csp: 
	  Identifier 
      	{ safe_assign($$, ATmakeList1( (ATerm) $1));
      	  gsDebugMsg("Identifier_csp: parsed \n  %T\n", $$);	
		}
	| Identifier_csp COMMA Identifier
      	{ safe_assign($$, ATinsert($1, (ATerm) $3));
      	  gsDebugMsg("Identifier_csp: parsed \n  %T\n", $$);	
		}
	;

Expression_csp:
	  Expression
      	{ safe_assign($$, ATmakeList1((ATerm) $1));
      	  gsDebugMsg("Expression_csp: parsed \n  %T\n", $$);	
		}
	| Expression_csp COMMA Expression
      	{ safe_assign($$, ATinsert($1, (ATerm) $3));
      	  gsDebugMsg("Expression_csp: parsed \n  %T\n", $$);	
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
          gsDebugMsg("%T\n",$2);
          gsDebugMsg("%T\n",hash);

          safe_assign($$, gsMakeSendStat($1, channel, hash , ATreverse( $4) ) );
      	  gsDebugMsg("CommStatement: parsed %T\n", $$);	
        }
//	| OptGuard Expression SSEND Expression_csp
	| OptGuard Expression EXCLAMATION
        {
          gsDebugMsg("%T",$2);
 
          ATermAppl channel = (ATermAppl) ATgetArgument(ATgetArgument( $2,0),0);   
          ATermAppl hash    = (ATermAppl) ATgetArgument($2,2); 

          safe_assign($$, gsMakeSendStat($1, channel, hash, ATmakeList0() ) );
      	  gsDebugMsg("CommStatement: parsed %T\n", $$);	
        }
//	| OptGuard Expression SSEND 
//	| OptGuard SSEND Expression_csp 
	| OptGuard Expression RECV Expression_csp
        {
          //Check if $2 is properly typed
          //Check if $4 is properly typed
          //Check if $2 can receive

          gsDebugMsg("%T",$2);
 
          ATermAppl channel = (ATermAppl) ATgetArgument(ATgetArgument( $2,0),0);   
          ATermAppl hash    = (ATermAppl) ATgetArgument($2,2); 

          safe_assign($$, gsMakeRecvStat($1, channel, hash, ATreverse( $4) ) );
      	  gsDebugMsg("CommStatement: parsed %T\n", $$);	
        }
//	| OptGuard Expression RRECV Expression_csp
	| OptGuard Expression RECV
        {
          //Check if $2 is properly typed
          //Check if $4 is properly typed
          //Check if $2 can receive

          gsDebugMsg("%T",$2);
 
          ATermAppl channel = (ATermAppl) ATgetArgument(ATgetArgument( $2,0),0);   
          ATermAppl hash    = (ATermAppl) ATgetArgument($2,2); 

          safe_assign($$, gsMakeRecvStat($1, channel, hash, ATmakeList0() ) );
      	  gsDebugMsg("CommStatement: parsed %T\n", $$);	
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
      	  gsDebugMsg("parsed SEP statement \n  %T\n", $$);	
		}
	| Statement ALT Statement
      	{ safe_assign($$, gsMakeAltStat( $1, $3));
      	  gsDebugMsg("parsed ALT statement \n  %T\n", $$);	
		}
/*	| Statement BARS Statement 
      	{ safe_assign($$, gsMakeParStat( $1, $3));
      	  gsDebugMsg("parsed Paralell statement \n  %T\n", $$);	
		}*/
	;

UnaryStatement:
	  STAR Statement
        { 
          gsDebugMsg("%d\n", __LINE__);
      	  safe_assign($$, gsMakeStarStat( $2));
      	  gsDebugMsg("parsed STAR statement \n  %T\n", $$);	
		}
	| Expression GUARD_REP Statement
      	{
          gsDebugMsg("%d\n", __LINE__);
          gsDebugMsg("%T\n",$1);
          gsDebugMsg("%T\n",$3);
          UnaryTypeCheck(ATAgetArgument($1,1), "Bool");
          safe_assign($$, gsMakeGuardedStarStat( $1, $3));
      	  gsDebugMsg("parsed GuardedSTAR statement \n  %T\n", $$);	
		}
	;

AdvancedStatement:
	  OptGuard DEADLOCK
      	{ safe_assign($$, gsMakeDeltaStat($1, gsMakeDelta()));
      	  gsDebugMsg("parsed deadlock statement \n  %T\n", $$);	
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
      		gsDebugMsg("parsed Negation Expression's\n  %T\n", $$);
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
      	  gsDebugMsg("parsed expression-element \n  %T\n", $$);	
		}
	| ExpressionIdentier_csp COMMA ExpressionIdentifier
      	{ safe_assign($$, ATinsert($1, (ATerm) $3));
      	  gsDebugMsg("parsed expression-element\n  %T\n", $$);	
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
		      gsErrorMsg("ExpressionIdentifier: Variable %T is not defined!\n", $1 );
		      exit(1);
		    };
		  
		  //Type the expression
 	  	  safe_assign($$, 
			gsMakeExpression( $1, 
			  (ATermAppl) var_type_map[(ATerm) $1] 
			)
		  );
      	  gsDebugMsg("parsed Identifier's\n  %T\n", $$);
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
              gsErrorMsg("BasicExpression: Variable/Channel %T is not defined!\n", $1 );
              exit(1);
          }
  
          gsDebugMsg("BasicExpression: parsed \n  %T\n", $$);
		}
	| Identifier DOT Expression  
 
		{
		  /**  
		    * Lookup Identifier Type
		    *
		    * TODO: Add scope
		    *
		    **/
          gsDebugMsg("%s:%d\n",__FILE__, __LINE__ );

          UnaryTypeCheck( (ATermAppl) ATgetArgument($3,1), "Nat");
          
          if (var_type_map.find((ATerm) $1) != var_type_map.end())
          {
            ATermAppl tuple_type = (ATermAppl) var_type_map[(ATerm) $1]; 
            gsDebugMsg("%T\n", tuple_type); 

            //Check if $1 is a tuple 
            if( strcmp("TupleType", ATgetName( ATgetAFun( tuple_type ) ) ) != 0 )  
            {
              chigetposition();
              gsErrorMsg("%T is not a Tuple", $1);
              exit(1);
            }
           
            if (!is_number(ATgetName(ATgetAFun(ATgetArgument($3,0)))) )
            {
              chigetposition();
              gsErrorMsg("BasicExpression: Expected a number after the \".\"\n");
              exit(1);
            }
          
            int index = atoi(ATgetName(ATgetAFun(ATgetArgument($3,0))));

            if (index >= (int) ATgetLength( (ATermList) ATgetArgument( tuple_type, 0 ) ) )
            {
              chigetposition();
              gsErrorMsg("BasicExpression: Index value \"%d\" is out of bounds for %T\n", index, $1 );
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
            gsDebugMsg("BasicExpression (Tuple Selection): parsed \n  %T\n", $$);
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
  	 
  	           gsDebugMsg("BasicExpression (Hashed Channel): parsed \n  %T\n", $$);  
          }
          
          if ( (chan_type_direction_map.end() == chan_type_direction_map.find( (ATerm) $1)) &&
               (var_type_map.find((ATerm) $1) == var_type_map.end()) )
          {   
              chigetposition();
              gsErrorMsg("BasicExpression: Variable/Channel %T is not defined!\n", $1 );
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
      		gsDebugMsg("BooleanExpression: parsed \n  %T\n", $$);
		}
	| TRUE
		{ 
 	  		safe_assign($$, 
				gsMakeExpression( $1, 
					gsMakeType( gsString2ATermAppl("Bool" )) 
				)
			);
      		gsDebugMsg("BooleanExpression: parsed \n  %T\n", $$);
		}
	| NOT Expression
		{ 
            UnaryTypeCheck( (ATermAppl) ATgetArgument($2,1), "Bool");

 	  		safe_assign($$, 
				gsMakeUnaryExpression( gsString2ATermAppl("!" ),
				gsMakeType( gsString2ATermAppl("Bool" ) ), 
				$2 ) 
			);
      		gsDebugMsg("parsed Negation Expression's\n  %T\n", $$);
		}
	| EXCLAMATION Expression
		{ 
            UnaryTypeCheck( (ATermAppl) ATgetArgument($2,1), "Bool");

 	  		safe_assign($$, 
				gsMakeUnaryExpression( gsString2ATermAppl("!" ),
				gsMakeType( gsString2ATermAppl("Bool" ) ), 
				$2 ) 
			);
      		gsDebugMsg("parsed Negation Expression's\n  %T\n", $$);
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
      		gsDebugMsg("parsed Binary AND Expression's\n  %T\n", $$);
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
      		gsDebugMsg("parsed Binary AND Expression's\n  %T\n", $$);
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
      		gsDebugMsg("parsed Binary AND Expression's\n  %T\n", $$);
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
      		gsDebugMsg("parsed Expression's\n  %T\n", $$);
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
      		gsDebugMsg("parsed UnaryExpression's\n  %T\n", $$);
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
      		gsDebugMsg("parsed UnaryExpression's\n  %T\n", $$);
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
      		gsDebugMsg("parsed Binary POWER Expression's\n  %T\n", $$);
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
      		gsDebugMsg("parsed Binary STAR Expression's\n  %T\n", $$);
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
      		gsDebugMsg("parsed Binary DIVIDE Expression's\n  %T\n", $$);
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
      		gsDebugMsg("parsed Binary MOD Expression's\n  %T\n", $$);
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
      		gsDebugMsg("parsed Binary DIV Expression's\n  %T\n", $$);
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
      		gsDebugMsg("parsed Binary MIN Expression's\n  %T\n", $$);
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
      		gsDebugMsg("parsed Binary MAX Expression's\n  %T\n", $$);
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
      		gsDebugMsg("parsed Binary LESS Expression's\n  %T\n", $$);
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
      		gsDebugMsg("parsed Binary GREATER Expression's\n  %T\n", $$);
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
      		gsDebugMsg("parsed Binary LEQ Expression's\n  %T\n", $$);
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
      		gsDebugMsg("parsed Binary GEQ Expression's\n  %T\n", $$);
		}
	;

PlusExpression:
	 Expression PLUS Expression
	 { 
       gsDebugMsg("Expression 1: %T\n", $1);
       gsDebugMsg("Expression 2: %T\n", $3);
 
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
            gsDebugMsg("PlusExpression - Nat Expression parsed: \n%T\n", $$);
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
		   gsErrorMsg("Incompatible Types Checking failed\n");
		   exit(1);
		 }
   		 safe_assign($$, gsMakeBinarySetExpression( $2,  
				         ATAgetArgument($1,1), 
                	$1, $3));
         gsDebugMsg("PlusExpression - Set Expression parsed: \n  %T\n", $$);
         processed = true;
       } 

       /**
         *  For all other types on plus 
         *
         **/

       if (!processed)
       {
         chigetposition();
         gsErrorMsg("Expressions %T and %T cannot be used with \"+\"\n", ATAgetArgument($1,0), ATAgetArgument($3,0));
         exit(1);
       }
     }
   ;

MinusExpression:
	 Expression MINUS Expression
	 { 
       gsDebugMsg("Expression 1: %T\n", $1);
       gsDebugMsg("Expression 2: %T\n", $3);
 
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
         gsDebugMsg("MinusExpression - Nat Expression parsed: \n%T\n", $$);
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
		   gsErrorMsg("Incompatible Types Checking failed\n");
		   exit(1);
		 }
   		 safe_assign($$, gsMakeBinaryListExpression( $2,  
				         ATAgetArgument($1,1), 
                	$1, $3));
         gsDebugMsg("MinusExpression - Literal Expression parsed: \n  %T\n", $$);
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
		   gsErrorMsg("Incompatible Types Checking failed\n");
		   exit(1);
		 }
   		 safe_assign($$, gsMakeBinarySetExpression( $2,  
				         ATAgetArgument($1,1), 
                	$1, $3));
         gsDebugMsg("MinusExpression - Set Expression parsed: \n  %T\n", $$);
         processed = true;
       } 
       

       if (!processed)
       {
         chigetposition();
         gsErrorMsg("Expressions %T and %T cannot be used with \"-\"\n", ATAgetArgument($1,0), ATAgetArgument($3,0));
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
				  gsErrorMsg("EqualityExpression: Incompatible Types Checking failed\n");
				  exit(1);
				};
            
            if (strcmp(ATgetName(ATgetAFun(ATAgetArgument($3,1))), "SetType") == 0 )
		    {
			  safe_assign($$, gsMakeBinarySetExpression(   
                                  gsString2ATermAppl("=="),  
			  		  gsMakeType( gsString2ATermAppl("Bool" )), 
			  $1, $3));
      		  gsDebugMsg("EqualityExpression parsed: \n  %T\n", $$);
            } else {
			  safe_assign($$, gsMakeBinaryExpression(  
                                  gsString2ATermAppl("=="),  
			  		  gsMakeType( gsString2ATermAppl("Bool" )), 
			  $1, $3));
      		  gsDebugMsg("EqualityExpression parsed: \n  %T\n", $$);
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
				  gsErrorMsg("EqualityExpression: Incompatible Types Checking failed\n");
				  exit(1);
				};
            
            if (strcmp(ATgetName(ATgetAFun(ATAgetArgument($3,1))), "SetType") == 0 )
		    {
			  safe_assign($$, gsMakeBinarySetExpression( 
                                  gsString2ATermAppl("!="),  
			  		  gsMakeType( gsString2ATermAppl("Bool" )), 
			  $1, $3));
      		  gsDebugMsg("EqualityExpression parsed: \n  %T\n", $$);
            } else {
			  safe_assign($$, gsMakeBinaryExpression( gsString2ATermAppl("!="),  
			  		  gsMakeType( gsString2ATermAppl("Bool" )), 
			  $1, $3));
      		  gsDebugMsg("EqualityExpression parsed: \n  %T\n", $$);
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
		   gsErrorMsg("Incompatible Types Checking failed\n");
		   exit(1);
		 }
   		 safe_assign($$, gsMakeBinarySetExpression( $2,  
					gsMakeType( gsString2ATermAppl("Bool" )), 
                	$1, $3));
         gsDebugMsg("MemberTest - SetLiteral parsed: \n  %T\n", $$);
         processed = true;
       } 

       if (strcmp(ATgetName(ATgetAFun(ATAgetArgument($3,1))), "ListType") == 0 )
         {
         if(!ContainerTypeChecking(gsMakeListType(ATAgetArgument($1,1)),  ATAgetArgument($3,1)))
	       {
		     gsErrorMsg("Incompatible Types Checking failed\n");
		     exit(1);
		   }
 	  	 safe_assign($$, gsMakeBinaryListExpression( $2,  
					gsMakeType( gsString2ATermAppl("Bool" )), 
			        $1, $3));
      	 gsDebugMsg("MemberTest - ListExpression parsed: \n  %T\n", $$);
         processed = true;
         }
      
         if (!processed)
         {
           chigetposition();
           gsErrorMsg("Experrsions %T and %T cannot be used with \"in\"", ATAgetArgument($1,0), ATAgetArgument($3,0));
           exit(1); 
        }
    } 
    ;

RecordExpression:
     LBRACKET Expression_csp COMMA Expression RBRACKET
     {
      	  gsDebugMsg("R:%d",__LINE__);
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
      	  gsDebugMsg("RecordExpression parsed: \n  %T\n", $$);

     }
   ; 
	
SetExpression:
      LBRACE RBRACE
      {
          safe_assign($$, gsMakeSetLiteral( ATmakeList0(), gsMakeSetType(gsMakeType(gsMakeNil()))));
      }
    | LBRACE Expression_csp RBRACE
      {
      	  gsDebugMsg("R:%d",__LINE__);
          ATerm type; 
		  ATermList to_process = $2;
		  while(!ATisEmpty(to_process))
          {
             ATerm elementType = ATgetArgument(ATgetFirst(to_process),1);
             if (ATgetLength(to_process) == ATgetLength($2))
             {
               type = elementType;
             }
             gsDebugMsg("%T\n",ATgetFirst(to_process));
             if (type != elementType )
             {
               chigetposition();
               gsErrorMsg("SetLiteral contains mixed types %T and %T\n"
                         , type, elementType);
               exit(1);
             }
			 to_process = ATgetNext( to_process) ;
		  }
          safe_assign($$, gsMakeSetLiteral( ATreverse($2), gsMakeSetType((ATermAppl) type)));
      	  gsDebugMsg("SetLiteral parsed: \n  %T\n", $$);
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
			  gsErrorMsg("r%d: Union failed: Incompatible Types Checking failed:\n %T and %T\n", __LINE__, $1, $3);
			  exit(1);
			};

  		  safe_assign($$, gsMakeBinarySetExpression( $2,  
				ATAgetArgument($1,1), 
		        $1, $3));
   		  gsDebugMsg("BinarySetExpression parsed: \n  %T\n", $$);
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
			  gsErrorMsg("r%d: Intersection failed: Incompatible Types Checking failed:\n %T and %T\n", __LINE__, $1, $3);
			  exit(1);
			};

  		  safe_assign($$, gsMakeBinarySetExpression( $2,  
				ATAgetArgument($1,1), 
		        $1, $3));
   		  gsDebugMsg("BinarySetExpression parsed: \n  %T\n", $$);
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
			  gsErrorMsg("r%d: Subsection failed: Incompatible Types Checking failed:\n %T and %T\n", __LINE__, $1, $3);
			  exit(1);
			};

  		  safe_assign($$, gsMakeBinarySetExpression( $2,  
				gsMakeType( gsString2ATermAppl("Bool" ) ),
		        $1, $3));
   		  gsDebugMsg("BinarySetExpression parsed: \n  %T\n", $$);
		} 
    | PICK LBRACKET Expression RBRACKET 
      {
            gsDebugMsg("R:%d\n",__LINE__);
			if(!(strcmp(ATgetName(ATgetAFun(ATAgetArgument($3,1))), "ListType") == 0 ))
				{
                  chigetposition();
				  gsErrorMsg("Functions: %T cannot used on %T", $1, $3);
				  exit(1);
				};

 	  		safe_assign($$, gsMakeFunction( $1,  
			     (ATermAppl) ATgetArgument($3,1), 
			$3));
      		gsDebugMsg("Functions parsed: \n  %T\n", $$);
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
				  gsErrorMsg("Concatination failed: Incompatible Types Checking failed:\n %T and %T\n", $1, $3);
				  exit(1);
				};

 	  		safe_assign($$, gsMakeBinaryListExpression( $2,  
					ATAgetArgument($1,1), 
			$1, $3));
      		gsDebugMsg("ListExpression parsed: \n  %T\n", $$);
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
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};

 	  		safe_assign($$, gsMakeBinaryListExpression( $2,  
					ATAgetArgument($1,1), 
			$1, $3));
      		gsDebugMsg("ListExpression parsed: \n  %T\n", $$);
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
				  gsErrorMsg("Functions: %T cannot used on %T", $1, $3);
				  exit(1);
				};

 	  		safe_assign($$, gsMakeFunction( $1,  
					gsMakeType( gsString2ATermAppl("Nat" )), 
			$3));
      		gsDebugMsg("Functions parsed: \n  %T\n", $$);
      }
    | HEAD LBRACKET  Expression RBRACKET
      {
			if(!(strcmp(ATgetName(ATgetAFun(ATAgetArgument($3,1))), "ListType") == 0 ))
				{
                  chigetposition();
				  gsErrorMsg("Functions: %T cannot used on %T", $1, $3);
				  exit(1);
				};

 	  		safe_assign($$, gsMakeFunction( $1,  
				  (ATermAppl) ATgetArgument(ATgetArgument($3,1),0), 
			$3));
      		gsDebugMsg("Functions parsed: \n  %T\n", $$);
      }
    | TAIL LBRACKET  Expression RBRACKET
      {
            gsDebugMsg("R:%d\n",__LINE__);
			if(!(strcmp(ATgetName(ATgetAFun(ATAgetArgument($3,1))), "ListType") == 0 ))
				{
                  chigetposition();
				  gsErrorMsg("Functions: %T cannot used on %T", $1, $3);
				  exit(1);
				};

 	  		safe_assign($$, gsMakeFunction( $1,  
			     (ATermAppl) ATgetArgument($3,1), 
			$3));
      		gsDebugMsg("Functions parsed: \n  %T\n", $$);
      }
    | RHEAD LBRACKET  Expression RBRACKET
      {
            gsDebugMsg("R:%d\n",__LINE__);
			if(!(strcmp(ATgetName(ATgetAFun(ATAgetArgument($3,1))), "ListType") == 0 ))
				{
                  chigetposition();
				  gsErrorMsg("Functions: %T cannot used on %T", $1, $3);
				  exit(1);
				};

 	  		safe_assign($$, gsMakeFunction( $1,  
				  (ATermAppl) ATgetArgument(ATgetArgument($3,1),0), 
			$3));
      		gsDebugMsg("Functions parsed: \n  %T\n", $$);
      }
    | RTAIL LBRACKET  Expression RBRACKET
      {
            gsDebugMsg("R:%d\n",__LINE__);
			if(!(strcmp(ATgetName(ATgetAFun(ATAgetArgument($3,1))), "ListType") == 0 ))
				{
                  chigetposition();
				  gsErrorMsg("Functions: %T cannot used on %T", $1, $3);
				  exit(1);
				};

 	  		safe_assign($$, gsMakeFunction( $1,  
			       (ATermAppl) ATgetArgument($3,1), 
			$3));
      		gsDebugMsg("Functions parsed: \n  %T\n", $$);
      }
    | TAKE LBRACKET Expression COMMA Expression RBRACKET
      {
            gsDebugMsg("R:%d\n",__LINE__);
			if(!(strcmp(ATgetName(ATgetAFun(ATAgetArgument($3,1))), "ListType") == 0 ))
				{
                  chigetposition();
				  gsErrorMsg("Functions: %T cannot used on %T\n", $1, $3);
				  exit(1);
				};

			if(! (ATAgetArgument($5,1) == gsMakeType( gsString2ATermAppl("Nat" ) ) ) )
				{
                  chigetposition();
				  gsErrorMsg("Functions: %T cannot used on 2nd argument %T\n", $1, $5);
				  gsErrorMsg("Type checking failed\n");
				  exit(1);
				};

 	  		safe_assign($$, gsMakeFunction2( $1,  
			       (ATermAppl) ATgetArgument($3,1), 
			       $3, 
                   $5));
      		gsDebugMsg("Functions parsed: \n%T\n", $$);
      }
    | DROP LBRACKET  Expression COMMA Expression RBRACKET
      {
            gsDebugMsg("R:%d\n",__LINE__);
			if(!(strcmp(ATgetName(ATgetAFun(ATAgetArgument($3,1))), "ListType") == 0 ))
				{
                  chigetposition();
				  gsErrorMsg("Functions: %T cannot used on %T\n", $1, $3);
				  exit(1);
				};

			if(! (ATAgetArgument($5,1) == gsMakeType( gsString2ATermAppl("Nat" ) ) ) )
				{
                  chigetposition();
				  gsErrorMsg("Functions: %T cannot used on 2nd argument %T\n", $1, $5);
				  gsErrorMsg("Type checking failed\n");
				  exit(1);
				};

 	  		safe_assign($$, gsMakeFunction2( $1,  
			       (ATermAppl) ATgetArgument($3,1), 
			       $3, 
                   $5));
      		gsDebugMsg("Functions parsed: \n%T\n", $$);
      }
    /* List functions that are not supported */
    | SORT LBRACKET  Expression RBRACKET
      {
            gsDebugMsg("R:%d\n",__LINE__);
			if(!(strcmp(ATgetName(ATgetAFun(ATAgetArgument($3,1))), "ListType") == 0 ))
				{
				  gsErrorMsg("Functions: %T cannot used on %T", $1, $3);
				  exit(1);
				};

 	  		safe_assign($$, gsMakeFunction( $1,  
			       (ATermAppl) ATgetArgument($3,1), 
			$3));
      		gsDebugMsg("Functions parsed: \n  %T\n", $$);
      }
    | INSERT LBRACKET  Expression RBRACKET
      {
        chigetposition();
        gsErrorMsg("%T is not supported", $1);
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
      	  gsDebugMsg("Entering ListLiteral\n");
          ATerm type; 
		  ATermList to_process = $2;
		  while(!ATisEmpty(to_process))
          {
             ATerm elementType = ATgetArgument(ATgetFirst(to_process),1);
             if (ATgetLength(to_process) == ATgetLength($2))
             {
               type = elementType;
             }
             gsDebugMsg("%T\n",ATgetFirst(to_process));
             if (type != elementType )
             {
               chigetposition();
               gsErrorMsg("ListLiteral contains mixed types %T and %T\n"
                         , type, elementType);
               exit(1);
             }
			 to_process = ATgetNext( to_process) ;
		  }
          safe_assign($$, gsMakeListLiteral( ATreverse($2), gsMakeListType((ATermAppl) type)));
      	  gsDebugMsg("ListLiteral parsed: \n  %T\n", $$);
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
          gsErrorMsg("BinTypeCheck: Incompatible Types Checking failed\n");
          exit(1);
        };
    if(arg1 != gsMakeType(gsString2ATermAppl(type.c_str())))
        {
          chigetposition();
          gsErrorMsg("Expected type %s\n", type.c_str());
          exit(1);
        };
  return;
}

void UnaryTypeCheck(ATermAppl arg1, std::string type)
{
    gsDebugMsg("%s;%d\n",__FILE__,__LINE__);
    gsDebugMsg("arg1: %T\n", arg1);
    ATermAppl arg2 = gsMakeType(gsString2ATermAppl(type.c_str()));
    gsDebugMsg("arg2: %T\n", arg2);

    if( arg1 != arg2 )
        {
          chigetposition();
          gsErrorMsg("UnaryTypeCheck: Incompatible Type, expected %s\n", type.c_str());
          exit(1);
        };
  return;
}

bool ContainerTypeChecking(ATermAppl arg1, ATermAppl arg2)
{
  gsDebugMsg("%s;%d\n",__FILE__,__LINE__);
  gsDebugMsg("arg1: %T\n", arg1);
  gsDebugMsg("arg2: %T\n", arg2);
  if(arg1 == arg2)
  {
    return true;
  }
  
  gsDebugMsg("ContainerTypeChecking: %T, %T\n",arg1, arg2);
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
   for (int i = 0; i < (int) s.length(); i++) {
       if (!std::isdigit(s[i]))
           return false;
   }

   return true;
}
