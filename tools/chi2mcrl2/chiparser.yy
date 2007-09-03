%{
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
//#include "symtab.h"
#include <iostream>
#include "libstruct.h"
#include <aterm2.h>
#include "print/messaging.h"
#include "mcrl2/utilities/aterm_ext.h"

/*extern int yyerror(const char *s);
extern int yylex( void );
extern char* yytext; */

#ifdef __cplusplus
using namespace ::mcrl2::utilities;
#endif

//external declarations from lexer.l
void chiyyerror( const char *s );
int chiyylex( void );
extern ATermAppl spec_tree;
extern ATermIndexedSet parser_protect_table;

#define YYMAXDEPTH 160000

//local declarations
ATermAppl gsSpecEltsToSpec(ATermList SpecElts);
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

%token PROC MODEL_DEF ENUM
%token <appl> VAR CONST CHAN
%token <appl> SKIP BARS ALT
%token <appl> COLON TYPE
%token <appl> ID TIME 
%token <appl> BP EP PROC_SEP SEP COMMA DEFINES ASSIGNMENT MINUS PLUS GG 
%token <appl> LBRACE RBRACE LBRACKET RBRACKET
%token <appl> AND OR GUARD NOT EQUAL OLD
%token <appl> BOOL NUMBER INT REALNUMBER TRUE FALSE DOT DEADLOCK IMPLIES NOTEQUAL GEQ LEQ MAX MIN DIV MOD POWER
%token RECV EXCLAMATION SENDRECV RECVSEND SSEND RRECV STAR GUARD_REP DERIVATIVE

%left '-' '+'
%left '*' '/'       /* order '+','-','*','/' */
%right '^'          /* exponentiation        */
%nonassoc MINUS     /* negation--unary minus */
%start ChiProgram
%glr-parser
%debug
%verbose
/* 
 *  TERMINALS
 *  ---------
 */

%type <appl> Type BasicType
%type <appl> IdentifierType IdentifierTypeExpression
%type <appl> NatIntExpression BasicExpression BooleanExpression Expression 
%type <appl> BoolNatIntExpression 
%type <appl> LocalVariables Identifier// AssignmentStatement 
%type <appl> ProcessBody Statement

%type <list> Identifier_csp //Expression_csp 
%type <list> IdentifierTypeExpression_csp IdentifierType_csp
%type <list> LocalVariables_csp

/* 
 *  GRAMMER 
 *  -------
 */
 
%%

ChiProgram: ProcessDefinition { printf("inputs contains a valid Chi-specification\n"); }
	; 

ProcessDefinition: 
	  PROC Identifier LBRACKET RBRACKET DEFINES ProcessBody
	| PROC Identifier LBRACKET FormalParameter_csp RBRACKET DEFINES ProcessBody
//	| PROC Identifier ExplicitTemplates LBRACKET RBRACKET DEFINES ProcessBody
//	| PROC Identifier ExplicitTemplates LBRACKET FormalParameter_csp RBRACKET DEFINES ProcessBody 
	;

Identifier: ID
		{ 
 	  	  safe_assign($$, $1 );
      	  gsDebugMsg("parsed id's\n  %T\n", $$);
		}

ProcessBody: 
	  BP Statement EP
      	{ //safe_assign($$, gsMakeProcSpec( ATmakeList1((ATerm) gsMakeNil) ,$1));
      	  gsDebugMsg("parsed ProcessBody  \n  %T\n", $$);	
		}
	| BP LocalVariables_csp PROC_SEP Statement EP
      	{ //safe_assign($$, gsMakeProcSpec(ATmakeList1((ATerm) $1)));
      	  gsDebugMsg("parsed ProcessBody  \n  %T\n", $$);	
		}
	;

LocalVariables_csp: 
	  LocalVariables 
      	{ safe_assign($$, ATmakeList1((ATerm) $1));
      	  gsDebugMsg("parsed localvariables  \n  %T\n", $$);	
		}
	| LocalVariables_csp COMMA LocalVariables
      	{ safe_assign($$, ATinsert($1, (ATerm) $3));
      	  gsDebugMsg("parsed localvariables \n  %T\n", $$);	
		}
	;

LocalVariables: 
	  VAR IdentifierTypeExpression_csp
		{
		  safe_assign($$, gsMakeVarSpec( $2 ));
		  gsDebugMsg("parsed VariableList \n %T\n", $$);
		}
/*	| CHAN ChannelDefinition_csp */
//	| RecursionDefinition
	;

IdentifierTypeExpression_csp:
	  IdentifierTypeExpression
      	{ safe_assign($$, ATmakeList1((ATerm) $1));
      	  gsDebugMsg("parsed id-element \n  %T\n", $$);	
		}
	| IdentifierTypeExpression_csp COMMA IdentifierTypeExpression 
      	{ safe_assign($$, ATinsert($1, (ATerm) $3));
      	  gsDebugMsg("parsed id-element \n  %T\n", $$);	
		}
	;

IdentifierTypeExpression:
	  IdentifierType 
	| IdentifierType DEFINES Expression
		{
		  safe_assign($$, gsMakeDataVarExprID ( $1, $3 ) );
		  gsDebugMsg("parsed Identifier Type With Expression \n %T\n", $$);
		}
	;

IdentifierType_csp:
	  IdentifierType
      	{ safe_assign($$, ATmakeList1((ATerm) $1));
      	  gsDebugMsg("parsed id-element \n  %T\n", $$);	
		}
	| IdentifierType_csp COMMA IdentifierType
      	{ safe_assign($$, ATinsert($1, (ATerm) $3));
      	  gsDebugMsg("parsed id-element \n  %T\n", $$);	
		}
	;

FormalParameter_csp: 
	  FormalParameter 
	| FormalParameter_csp COMMA FormalParameter
	;

FormalParameter:
	  VAR IdentifierType_csp
  	| CHAN ChannelDeclaration_csp
	;

IdentifierType:
	  Identifier_csp COLON Type
		{
		  safe_assign($$, gsMakeDataVarID ( $1, $3 ) );
		  gsDebugMsg("parsed IdentifierType\n %T\n", $$);
		}
	;

ChannelDeclaration_csp:
	  ChannelDeclaration
	| ChannelDeclaration_csp COMMA ChannelDeclaration
	;

ChannelDeclaration:
	  IdentifierChannelDeclaration_csp COLON Type
	;
	
IdentifierChannelDeclaration_csp:
	  IdentifierChannelDeclaration
	| IdentifierChannelDeclaration_csp COMMA IdentifierChannelDeclaration
	;

IdentifierChannelDeclaration: 
	  Identifier RECV
	| Identifier EXCLAMATION
	| Identifier SENDRECV
	| Identifier RECVSEND
	;

/* ChannelDefinition_csp:
	  ChannelDefinition
	| ChannelDefinition_csp COMMA ChannelDefinition
	;

ChannelDefinition:
	  IdentifierChannelDefinition_csp COLON Type
	;
			
IdentifierChannelDefinition_csp:
	  IdentifierChannelDefinition
	| IdentifierChannelDefinition_csp COMMA IdentifierChannelDefinition
	;
	  
IdentifierChannelDefinition:
	  Identifier SENDRECV
	| Identifier RECVSEND
	;
*/	
Type: 
	  BasicType
/*	| ContainerType
	| FuntionType
	| DistributionType */
/*	| LBRACKET Type RBRACKET */
	;

BasicType:
 	  TYPE
		{ 
 	  	  safe_assign($$, gsMakeType( $1) );
      	  gsDebugMsg("parsed Type \n  %T\n", $$);
		}
//	| Identifier
//	| Identifier DOT Identier
	;

/**
  * STATEMETNS
  *
  **/

Statement: 
	SKIP
/*	  LBRACKET Statement RBRACKET
	| BasicStatement
	| UnaryStatement
	| BinaryStatement
	; 

BasicStatement:
	  AssignmentStatement
	| CommStatement
//	| DelayStatement
	| Instantiation
//	| HybridStatement
//	| ReturnStatement
//	| FoldStatement
	| AdvancedStatement
	;

AssignmentStatement:
	  OptGuard OptChannel Expression_csp ASSIGNMENT Expression_csp
     	{ //safe_assign($$, gsMakeAssignment((ATerm) $3, (ATerm) $4) );
      	  gsDebugMsg("parsed skip statement \n  %T\n", $$);	
		}
	| OptGuard OptChannel SKIP
      	{ safe_assign($$, gsMakeSkip() );
      	  gsDebugMsg("parsed skip statement \n  %T\n", $$);	
		}
//	| OptGuard LBRACE Expression_csp RBRACE COLON Expression_csp GG Identifier
	;
*/
//OptGuard: /* empty */
/*	| Expression GUARD
	;
*/
//OptChannel: /* empty */
/*	| Expression COLON
	;
*/
Identifier_csp: 
	  Identifier 
		{ /* empty */
		  /* Identifier is propagated */
		}
	| Identifier_csp COMMA Identifier
      	{ safe_assign($$, ATinsert($1, (ATerm) $3));
      	  gsDebugMsg("parsed id's\n  %T\n", $$);	
		}
	;

/*Expression_csp:
	  Expression
      	{ safe_assign($$, ATmakeList1((ATerm) $1));
      	  gsDebugMsg("parsed expression-element \n  %T\n", $$);	
		}
	| Expression_csp COMMA Expression
      	{ safe_assign($$, ATinsert($1, (ATerm) $3));
      	  gsDebugMsg("parsed expression-element\n  %T\n", $$);	
		}
	;
*/
/*CommStatement:
	  OptGuard Expression EXCLAMATION Expression_csp
	| OptGuard Expression SSEND Expression_csp
	| OptGuard Expression EXCLAMATION
	| OptGuard Expression SSEND 
	| OptGuard SSEND Expression_csp 
	| OptGuard Expression RECV Expression_csp
	| OptGuard Expression RRECV Expression_csp
	| OptGuard Expression RECV
	| OptGuard Expression RRECV 
	| OptGuard RRECV Expression_csp 
	;*/
/*	
Instantiation:
	  Identifier LBRACE Expression_csp RBRACE
	| Identifier
	;

BinaryStatement:
	  Statement SEP Statement
	| Statement ALT Statement
	| Statement BARS Statement
	;

UnaryStatement:
	  STAR Statement
	| Expression GUARD_REP Statement
	;

AdvancedStatement:
	  DEADLOCK
	;
*/	
/**
  * EXPRESSIONS
  *
  *
  **/

Expression: //NUMBER
	  LBRACKET Expression RBRACKET
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
//	| FucntionExpression
	;	


BasicExpression:
	 Identifier
		{
			/**  
			  * Lookup Identifier Type
			  *
  			  **/	 
 	  		safe_assign($$, 
				gsMakeExpression( $1, 
					gsMakeType( gsString2ATermAppl("unknown")) 
				)
			);
      		gsDebugMsg("parsed Expression's\n  %T\n", $$);
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
					gsMakeType( gsString2ATermAppl("bool" )) 
				)
			);
      		gsDebugMsg("parsed Expression's\n  %T\n", $$);
		}
	| TRUE
		{ 
 	  		safe_assign($$, 
				gsMakeExpression( $1, 
					gsMakeType( gsString2ATermAppl("bool" )) 
				)
			);
      		gsDebugMsg("parsed Expression's\n  %T\n", $$);
		}
/*	| NOT Expression
	| EXCLAMATION Expression
	| Expression AND Expression 
	| Expression OR Expression 
	| Expression IMPLIES Expression */
	;	
	
NatIntExpression: 
	  NUMBER 
		{ 
 	  		safe_assign($$, 
				gsMakeExpression( $1, 
					gsMakeType( gsString2ATermAppl("nat" )) 
				)
			);
      		gsDebugMsg("parsed Expression's\n  %T\n", $$);
		}
	| PLUS NUMBER
		{ 
 	  		safe_assign($$, 
				gsMakeUnaryExpression( $1, 
					$2, 
					gsMakeType( gsString2ATermAppl("nat" )) 
				)
			);
      		gsDebugMsg("parsed UnaryExpression's\n  %T\n", $$);
		}
	| MINUS NUMBER
		{ 
 	  		safe_assign($$, 
				gsMakeUnaryExpression($1, 
					$2, 
					gsMakeType( gsString2ATermAppl("nat" )) 
				)
			);
      		gsDebugMsg("parsed UnaryExpression's\n  %T\n", $$);
		}
/*	| Expression '^' Expression
	| Expression '*' Expression
	| Expression '/' Expression
	| Expression '+' Expression
	| Expression '-' Expression
	| Expression MOD Expression
	| Expression DIV Expression
	| Expression MIN Expression
	| Expression MAX Expression
	| Expression '<' Expression
	| Expression '>' Expression
	| Expression LEQ Expression
	| Expression GEQ Expression */
	;

BoolNatIntExpression:
	  Expression EQUAL Expression
		{ 
 	  		safe_assign($$, gsMakeBinaryExpression( $2, $1, $3, 
					gsMakeType( gsString2ATermAppl("bool" )) 
			));
      		gsDebugMsg("parsed UnaryExpression's\n  %T\n", $$);
		}
	| Expression NOTEQUAL Expression
		{ 
 	  		safe_assign($$, gsMakeBinaryExpression( $2, $1, $3,  
					gsMakeType( gsString2ATermAppl("bool" )) 
			));
      		gsDebugMsg("parsed UnaryExpression's\n  %T\n", $$);
		}
	;	

/*
RealExpression:
	  TIME
	| REALNUMBER 
	| RealUnaryOperator Expression
	| Expression RealBinaryOperator Expression
	;

RealUnaryOperator:
	  PLUS
	| MINUS 
	;

RealBinaryOperator:
	  '^'
	| '*'
	| '/' 
	| '+'
	| '-'
	| MIN
	| MAX
	| '<'
	| '>' 
	| LEQ
	| GEQ
	| EQUAL
	| NOTEQUAL 
	;


BooleanBinaryOperator:
	  EQUAL
	| NOTEQUAL
	| AND
	| OR
	| IMPLIES
	;
*/

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

//struct symtab * symlook(char *s)
//{
//  struct symtab *sp;
// 
//  for(sp = symtab; sp < &symtab[NSYMS]; sp++) {
//    if(sp->name && !strcmp(sp->name,s))
//      return sp;
//
//    if(!sp->name) {
//      sp->name = (char *) strdup(s);
//      return sp;
//    }
//  }
//  yyerror("** Too many symbols check symlook in parser.y **");
//    exit(1);
//}

void resetType()
{
  //count = 0;
}

void reset_parser(void)
{
  //num = 0;
}

ATermAppl gsSpecEltsToSpec(ATermList SpecElts)
{
  ATermAppl Result = NULL;
  //ATermList VarDecl = ATmakeList0();
  //ATermAppl StatementSpec = NULL; 
  //int n = ATgetLength(SpecElts);

/*  
  for (int i = 0; i < n; i++) {
    ATermAppl SpecElt = ATAelementAt(SpecElts, i);
    if (gsIsProcessInit(SpecElt)) {
      if (Init == NULL) {
        Init = SpecElt;
      } else {
        //Init != NULL
        gsErrorMsg("parse error: multiple initialisations\n");
        return NULL;
      }
    } else {
      ATermList SpecEltArg0 = ATLgetArgument(SpecElt, 0);
      if (gsIsSortSpec(SpecElt)) {
        SortDecls = ATconcat(SortDecls, SpecEltArg0);
      } else if (gsIsConsSpec(SpecElt)) {
        ConsDecls = ATconcat(ConsDecls, SpecEltArg0);
      } else if (gsIsMapSpec(SpecElt)) {
        MapDecls = ATconcat(MapDecls, SpecEltArg0);
      } else if (gsIsDataEqnSpec(SpecElt)) {
        DataEqnDecls = ATconcat(DataEqnDecls, SpecEltArg0);
      } else if (gsIsActSpec(SpecElt)) {
        ActDecls = ATconcat(ActDecls, SpecEltArg0);
      } else if (gsIsProcEqnSpec(SpecElt)) {
        ProcEqnDecls = ATconcat(ProcEqnDecls, SpecEltArg0);
      }
    }
  }
  //check whether an initialisation is present
  if (Init == NULL) {
    gsErrorMsg("parse error: missing initialisation\n");
    return NULL;
  }
*/
  Result = NULL;
  /*
  gsMakeProcSpec(
    gsMakeVarSpec(VarDecl),
    StatementSpec
  ); */
  return Result;
}
