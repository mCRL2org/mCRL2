%{

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
//#include "symtab.h"
#include <iostream>
#include "libstruct_core.h"
#include <aterm2.h>
#include "print/messaging.h"
#include "mcrl2/utilities/aterm_ext.h"
#include <list>
#include <map>
#include <set>
#include <utility>

/*extern int yyerror(const char *s);
extern int yylex( void );
extern char* yytext; */

#ifdef __cplusplus
using namespace ::mcrl2::utilities;
using namespace std;
#endif

//external declarations from lexer.l
void chiyyerror( const char *s );
int chiyylex( void );
extern ATermAppl spec_tree;
extern ATermIndexedSet parser_protect_table;
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

#define safe_assign(lhs, rhs) { ATbool b; ATindexedSetPut(parser_protect_table, (ATerm) rhs, &b); lhs = rhs; }

%}

%union {
  ATermAppl appl;
  ATermList list;
}

//set name prefix
%name-prefix="chiyy"

/* 
 *  TERMINALS
 *  ---------
 */

%token <appl> PROC MODEL_DEF ENUM MODEL
%token <appl> VAR CONST CHAN
%token <appl> SKIP BARS ALT
%token <appl> COLON TYPE BOOL NAT
%token <appl> ID TIME 
%token <appl> BP EP PROC_SEP SEP COMMA DEFINES ASSIGNMENT MINUS PLUS GG 
%token <appl> LBRACE RBRACE LBRACKET RBRACKET
%token <appl> AND OR GUARD NOT EQUAL OLD 
%token <appl> NUMBER INT REALNUMBER TRUE FALSE DOT DEADLOCK IMPLIES NOTEQUAL GEQ LEQ MAX MIN DIV MOD POWER
%token <appl> RECV EXCLAMATION SENDRECV RECVSEND SSEND RRECV STAR GUARD_REP DERIVATIVE

%left MINUS PLUS 
%left DIVIDE       /* order '+','-','*','/' */
%right POWER SEP ALT GUARD_REP STAR BARS        /* exponentiation        */
%start ChiProgram
%glr-parser
%debug
%verbose
/* 
 *  TERMINALS
 *  ---------
 */

%type <appl> Type BasicType
%type <appl> NatIntExpression BasicExpression BooleanExpression Expression 
%type <appl> BoolNatIntExpression Instantiation ModelStatement
%type <appl> LocalVariables Identifier AssignmentStatement CommStatement
%type <appl> ProcessBody OptGuard BasicStatement OptChannel Statement BinaryStatement UnaryStatement
%type <appl> AdvancedStatement IdentifierChannelDefinition  IdentifierChannelDeclaration
%type <appl> ChiProgram ProcessDefinition FormalParameter ExpressionIdentifier
%type <appl> ModelDefinition ModelBody 

%type <list> IdentifierTypeExpression IdentifierType Identifier_csp Expression_csp FormalParameter_csp ProcessDefinitions ChannelDeclaration ChannelDefinition
%type <list> IdentifierTypeExpression_csp IdentifierType_csp ExpressionIdentier_csp 
%type <list> LocalVariables_csp ChannelDefinition_csp IdentifierChannelDefinition_csp
%type <list> ChannelDeclaration_csp IdentifierChannelDeclaration_csp


/* 
 *  GRAMMER 
 *  -------
 */
 
%%

ChiProgram: 
    ModelDefinition ProcessDefinitions 
		{ 
    	  gsDebugMsg("inputs contains a valid Chi-specification\n"); 
          safe_assign($$, gsMakeChiSpec($1,ATreverse($2)));
		  spec_tree = $$;
		}
	;

ModelDefinition:
      MODEL Identifier LBRACKET RBRACKET DEFINES ModelBody
        {
      	  safe_assign($$, gsMakeModelDef($2, $6));
      	  gsDebugMsg("parsed Model Def \n  %T\n", $$);
        }
      ;

ModelBody:
      BP ModelStatement EP
      	{ safe_assign($$, gsMakeModelSpec( ATmakeList0(), $2 ));
      	  gsDebugMsg("parsed Model Body  \n  %T\n", $$);	
		}
	| BP LocalVariables_csp PROC_SEP ModelStatement EP
      	{ safe_assign($$, gsMakeModelSpec( $2, $4));
      	  gsDebugMsg("parsed Model Body  \n  %T\n", $$);	
		}
	;

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
	  ProcOpenScope Identifier LBRACKET RBRACKET DEFINES ProcessBody
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
	| ProcOpenScope Identifier LBRACKET FormalParameter_csp RBRACKET DEFINES ProcessBody
		{ 
      	  safe_assign($$, gsMakeProcDef($2, gsMakeProcDecl( ATreverse($4)), $7));
      	  gsDebugMsg("parsed proc Def\n  %T\n", $$);
		}
//	| PROC Identifier ExplicitTemplates LBRACKET RBRACKET DEFINES ProcessBody  
/*     When adding these lines don't forget to:
       + update the aterm_spec.txt and translator function
       + update the translator function with ExplicedTemplates */ 
//	| PROC Identifier ExplicitTemplates LBRACKET FormalParameter_csp RBRACKET DEFINES ProcessBody 
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
		  safe_assign($$, $1 );
		  gsDebugMsg("IdentifierTypeExpression: parsed \n %T\n", $$);
		}
	| IdentifierType DEFINES Expression
		{
          ATermList list = $1;
          ATermList new_list = ATmakeList0();

          while (!ATisEmpty(list)) 
          {
             gsDebugMsg("%T",ATgetFirst(list));
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
		  ATermList list = $1;
		  int n = ATgetLength( list );
		  for(int i = 0; i < n ; ++i ){
			 if (var_type_map.end() != var_type_map.find(ATgetArgument( ATgetFirst( list ),0)))
			 {
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
             new_list = ATinsert(new_list,(ATerm) gsMakeChannelTypedID( (ATermAppl) ATgetFirst(list), $3) );
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
          gsDebugMsg("ChannelDefinition\n");
		  ATermList list = $1;
		  int n = ATgetLength( list );
		  for(int i = 0; i < n ; ++i ){
			 if (var_type_map.end() != var_type_map.find(ATgetArgument( ATgetFirst( list ),0)))
			 {
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
             gsDebugMsg("%T",ATgetFirst(list));
             new_list = ATinsert(new_list,(ATerm) gsMakeChannelTypedID( (ATermAppl) ATgetFirst(list), $3) );
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
/*	| ContainerType
	| FuntionType
	| DistributionType */
/*	| LBRACKET Type RBRACKET */
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
 	| TYPE
		{ 
          safe_assign($$, gsMakeType( $1 ) );
      	  gsDebugMsg("BasicType: parsed Type \n  %T\n", $$);
		}
//	| Identifier
//	| Identifier DOT Identier
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
//	| DelayStatement
//	| HybridStatement
//	| ReturnStatement
//	| FoldStatement 
	| AdvancedStatement
	;

AssignmentStatement:
	  OptGuard OptChannel ExpressionIdentier_csp ASSIGNMENT Expression_csp
     	{
          ATermList ids = $3;
          ATermList exprs = $5;

          while(!ATisEmpty(ids))
          { 
            if (ATgetArgument(ATgetFirst(ids), 1) != ATgetArgument(ATgetFirst(exprs), 1))
		    { 
              gsErrorMsg("Incompatible Types Checking failed\n");
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
      	{ safe_assign($$, gsMakeSkipStat( $1, $2, gsMakeSkip() ));
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
				  gsErrorMsg("Incompatible Types Checking failed\n");
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

          safe_assign($$, gsMakeSendStat($1,  channel, ATreverse( $4) ) );
      	  gsDebugMsg("parsed expression-element \n  %T\n", $$);	
        }
//	| OptGuard Expression SSEND Expression_csp
//	| OptGuard Expression EXCLAMATION
//	| OptGuard Expression SSEND 
//	| OptGuard SSEND Expression_csp 
	| OptGuard Expression RECV Expression_csp
        {
          //Check if $2 is properly typed
          //Check if $4 is properly typed
          //Check if $2 can receive

          gsDebugMsg("%T",$2);
 
          ATermAppl channel = (ATermAppl) ATgetArgument(ATgetArgument( $2,0),0);   

          safe_assign($$, gsMakeRecvStat($1, channel, ATreverse( $4) ) );
      	  gsDebugMsg("parsed expression-element \n  %T\n", $$);	
        }
//	| OptGuard Expression RRECV Expression_csp
//	| OptGuard Expression RECV
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
      	{ safe_assign($$, gsMakeStarStat( $2));
      	  gsDebugMsg("parsed STAR statement \n  %T\n", $$);	
		}
	| Expression GUARD_REP Statement
      	{
			/**
			  * Type Checking
			  *
			  **/	
			if(ATAgetArgument($1,1) != gsMakeType(gsString2ATermAppl("Bool")))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};
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
	| NatIntExpression
	| BoolNatIntExpression 
/*	| IntExpression
	| RealExpression */
//	| StringExpression
//	| ListExpression
//	| SetExpression
//	| DictExpression
//	| VectorExpression
//	| RecordExpression
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
          if (chan_type_direction_map.end() == chan_type_direction_map.find( (ATerm) $1))
            {
                
//              gsErrorMsg("BasicExpression: Channel %T is not defined!\n", $1 );
//              exit(1);
            } else {
              channel_exists = true;
              safe_assign($$, 
                gsMakeChannelTypedID(
                  gsMakeChannelID($1, gsMakeNil()),
                  (ATermAppl) chan_type_direction_map[(ATerm) $1].first
                )
              );
            }
          /* safe_assign($$, 
            gsMakeExpression( $1, 
              (ATermAppl) chan_type_direction_map[(ATerm) $1] 
            )
          ); */
          // Determine if the expression is defined already 
          if (var_type_map.end() == var_type_map.find( (ATerm) $1))
            {
            } else {
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
              gsErrorMsg("BasicExpression: Variable/Channel %T is not defined!\n", $1 );
              exit(1);
          }
  
          gsDebugMsg("BasicExpression: parsed \n  %T\n", $$);
		}
//	  OLD LBRACKET Expression RBRACKET 
//	  Identifier DOT Identifier
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
			/**
			  * Type Checking
			  *
			  **/	
			if(ATAgetArgument($1,1) != gsMakeType(gsString2ATermAppl("Bool")))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};

 	  		safe_assign($$, 
				gsMakeUnaryExpression( gsString2ATermAppl("!" ),
				gsMakeType( gsString2ATermAppl("Bool" ) ), 
				$2 ) 
			);
      		gsDebugMsg("parsed Negation Expression's\n  %T\n", $$);
		}
	| EXCLAMATION Expression
		{ 
			/**
			  * Type Checking
			  *
			  **/	
			if(ATAgetArgument($1,1) != gsMakeType(gsString2ATermAppl("Bool")))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};

 	  		safe_assign($$, 
				gsMakeUnaryExpression( gsString2ATermAppl("!" ),
				gsMakeType( gsString2ATermAppl("Bool" ) ), 
				$2 ) 
			);
      		gsDebugMsg("parsed Negation Expression's\n  %T\n", $$);
		}
	| Expression AND Expression 
		{ 
			/**
			  * Type Checking
			  *
			  **/	
			if(ATAgetArgument($1,1) != ATAgetArgument($3,1))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};
			if(ATAgetArgument($1,1) != gsMakeType(gsString2ATermAppl("Bool")))

				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};

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
			/**
			  * Type Checking
			  *
			  **/	
			if(ATAgetArgument($1,1) != ATAgetArgument($3,1))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};
			if(ATAgetArgument($1,1) != gsMakeType(gsString2ATermAppl("Bool")))

				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};

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
			/**
			  * Type Checking
			  *
			  **/	
			if(ATAgetArgument($1,1) != ATAgetArgument($3,1))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};
			if(ATAgetArgument($1,1) != gsMakeType(gsString2ATermAppl("Bool")))

				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};

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
			/**
			  * Type Checking
			  *
			  **/	
			if(ATAgetArgument($1,1) == gsString2ATermAppl("Nat"))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};

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
			/**
			  * Type Checking
			  *
			  **/	
			if(ATAgetArgument($1,1) == gsMakeType(gsString2ATermAppl("Nat")))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};

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
			/**
			  * Type Checking
			  *
			  **/	
			if(ATAgetArgument($1,1) != ATAgetArgument($3,1))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};
			if(ATAgetArgument($1,1) != gsMakeType(gsString2ATermAppl("Nat")))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};

 	  		safe_assign($$, 
				gsMakeBinaryExpression( gsString2ATermAppl("^" ),
				gsMakeType( gsString2ATermAppl("Nat" ) ),
				$1 , 
				$3  
				) 
			);
      		gsDebugMsg("parsed Binary AND Expression's\n  %T\n", $$);
		}
	| Expression STAR Expression
		{ 
			/**
			  * Type Checking
			  *
			  **/	
			if(ATAgetArgument($1,1) != ATAgetArgument($3,1))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};
			if(ATAgetArgument($1,1) != gsMakeType(gsString2ATermAppl("Nat")))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};

 	  		safe_assign($$, 
				gsMakeBinaryExpression( gsString2ATermAppl("*" ),
				gsMakeType( gsString2ATermAppl("Nat" ) ),
				$1 , 
				$3  
				) 
			);
      		gsDebugMsg("parsed Binary AND Expression's\n  %T\n", $$);
		}
	| Expression DIVIDE Expression
		{ 
			/**
			  * Type Checking
			  *
			  **/	
			if(ATAgetArgument($1,1) != ATAgetArgument($3,1))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};
			if(ATAgetArgument($1,1) != gsMakeType(gsString2ATermAppl("Nat")))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};

 	  		safe_assign($$, 
				gsMakeBinaryExpression( gsString2ATermAppl("/" ),
				gsMakeType( gsString2ATermAppl("Nat" ) ),
				$1 , 
				$3  
				) 
			);
      		gsDebugMsg("parsed Binary AND Expression's\n  %T\n", $$);
		}
	| Expression PLUS Expression
		{ 
			/**
			  * Type Checking
			  *
			  **/	
			if(ATAgetArgument($1,1) != ATAgetArgument($3,1))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};
			if(ATAgetArgument($1,1) != gsMakeType(gsString2ATermAppl("Nat")))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};

 	  		safe_assign($$, 
				gsMakeBinaryExpression( gsString2ATermAppl("+" ),
				gsMakeType( gsString2ATermAppl("Nat" ) ),
				$1 , 
				$3  
				) 
			);
      		gsDebugMsg("parsed Binary AND Expression's\n  %T\n", $$);
		}
	| Expression MINUS Expression
		{ 
			/**
			  * Type Checking
			  *
			  **/	
			if(ATAgetArgument($1,1) != ATAgetArgument($3,1))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};
			if(ATAgetArgument($1,1) != gsMakeType(gsString2ATermAppl("Nat")))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};

 	  		safe_assign($$, 
				gsMakeBinaryExpression( gsString2ATermAppl("-" ),
				gsMakeType( gsString2ATermAppl("Nat" ) ),
				$1 , 
				$3  
				) 
			);
      		gsDebugMsg("parsed Binary AND Expression's\n  %T\n", $$);
		}
	| Expression MOD Expression
		{ 
			/**
			  * Type Checking
			  *
			  **/	
			if(ATAgetArgument($1,1) != ATAgetArgument($3,1))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};
			if(ATAgetArgument($1,1) != gsMakeType(gsString2ATermAppl("Nat")))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};

 	  		safe_assign($$, 
				gsMakeBinaryExpression( gsString2ATermAppl("MOD" ),
				gsMakeType( gsString2ATermAppl("Nat" ) ),
				$1 , 
				$3  
				) 
			);
      		gsDebugMsg("parsed Binary AND Expression's\n  %T\n", $$);
		}
	| Expression DIV Expression
		{ 
			/**
			  * Type Checking
			  *
			  **/	
			if(ATAgetArgument($1,1) != ATAgetArgument($3,1))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};
			if(ATAgetArgument($1,1) != gsMakeType(gsString2ATermAppl("Nat")))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};

 	  		safe_assign($$, 
				gsMakeBinaryExpression( gsString2ATermAppl("DIV" ),
				gsMakeType( gsString2ATermAppl("Nat" ) ),
				$1 , 
				$3  
				) 
			);
      		gsDebugMsg("parsed Binary AND Expression's\n  %T\n", $$);
		}
	| Expression MIN Expression
		{ 
			/**
			  * Type Checking
			  *
			  **/	
			if(ATAgetArgument($1,1) != ATAgetArgument($3,1))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};
			if(ATAgetArgument($1,1) != gsMakeType(gsString2ATermAppl("Nat")))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};

 	  		safe_assign($$, 
				gsMakeBinaryExpression( gsString2ATermAppl("MIN" ),
				gsMakeType( gsString2ATermAppl("Nat" ) ),
				$1 , 
				$3  
				) 
			);
      		gsDebugMsg("parsed Binary AND Expression's\n  %T\n", $$);
		}
	| Expression MAX Expression
		{ 
			/**
			  * Type Checking
			  *
			  **/	
			if(ATAgetArgument($1,1) != ATAgetArgument($3,1))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};
			if(ATAgetArgument($1,1) != gsMakeType(gsString2ATermAppl("Nat")))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};

 	  		safe_assign($$, 
				gsMakeBinaryExpression( gsString2ATermAppl("MAX" ),
				gsMakeType( gsString2ATermAppl("Nat" ) ),
				$1 , 
				$3  
				) 
			);
      		gsDebugMsg("parsed Binary AND Expression's\n  %T\n", $$);
		}
	| Expression '<' Expression
		{ 
			/**
			  * Type Checking
			  *
			  **/	
			if(ATAgetArgument($1,1) != ATAgetArgument($3,1))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};
			if(ATAgetArgument($1,1) != gsMakeType(gsString2ATermAppl("Nat")))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};

 	  		safe_assign($$, 
				gsMakeBinaryExpression( gsString2ATermAppl("<" ),
				gsMakeType( gsString2ATermAppl("Nat" ) ),
				$1 , 
				$3  
				) 
			);
      		gsDebugMsg("parsed Binary AND Expression's\n  %T\n", $$);
		}
	| Expression '>' Expression
		{ 
			/**
			  * Type Checking
			  *
			  **/	
			if(ATAgetArgument($1,1) != ATAgetArgument($3,1))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};
			if(ATAgetArgument($1,1) != gsMakeType(gsString2ATermAppl("Nat")))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};

 	  		safe_assign($$, 
				gsMakeBinaryExpression( gsString2ATermAppl(">" ),
				gsMakeType( gsString2ATermAppl("Nat" ) ),
				$1 , 
				$3  
				) 
			);
      		gsDebugMsg("parsed Binary AND Expression's\n  %T\n", $$);
		}
	| Expression LEQ Expression
		{ 
			/**
			  * Type Checking
			  *
			  **/	
			if(ATAgetArgument($1,1) != ATAgetArgument($3,1))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};
			if(ATAgetArgument($1,1) != gsMakeType(gsString2ATermAppl("Nat")))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};

 	  		safe_assign($$, 
				gsMakeBinaryExpression( gsString2ATermAppl("<=" ),
				gsMakeType( gsString2ATermAppl("Nat" ) ),
				$1 , 
				$3  
				) 
			);
      		gsDebugMsg("parsed Binary AND Expression's\n  %T\n", $$);
		}
	| Expression GEQ Expression 
		{ 
			/**
			  * Type Checking
			  *
			  **/	
			if(ATAgetArgument($1,1) != ATAgetArgument($3,1))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};
			if(ATAgetArgument($1,1) != gsMakeType(gsString2ATermAppl("Nat")))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};

 	  		safe_assign($$, 
				gsMakeBinaryExpression( gsString2ATermAppl(">=" ),
				gsMakeType( gsString2ATermAppl("Nat" ) ),
				$1 , 
				$3  
				) 
			);
      		gsDebugMsg("parsed Binary AND Expression's\n  %T\n", $$);
		}
	;

BoolNatIntExpression:
	  Expression EQUAL Expression
		{
			/**
			  * Type Checking
			  *
			  **/	
			if(ATAgetArgument($1,1) != ATAgetArgument($3,1))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};
		
			safe_assign($$, gsMakeBinaryExpression( $2,  
					gsMakeType( gsString2ATermAppl("Bool" )), 
			$1, $3));
      		gsDebugMsg("parsed UnaryExpression's\n  %T\n", $$);
		}
	| Expression NOTEQUAL Expression
		{ 
			/**
			  * Type Checking
			  *
			  **/	
			if(ATAgetArgument($1,1) != ATAgetArgument($3,1))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};

 	  		safe_assign($$, gsMakeBinaryExpression( $2,  
					gsMakeType( gsString2ATermAppl("Bool" )), 
			$1, $3));
      		gsDebugMsg("parsed UnaryExpression's\n  %T\n", $$);
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
	| exp EQUAL exp			{ }
	| NOT exp				{ }
	| TRUE					{ }
    | FALSE					{ }
; */
%%

