/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison GLR parsers in C

   Copyright (C) 2002, 2003, 2004, 2005, 2006 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     PROC = 258,
     MODEL_DEF = 259,
     ENUM = 260,
     MODEL = 261,
     VAR = 262,
     CONST = 263,
     CHAN = 264,
     SKIP = 265,
     BARS = 266,
     ALT = 267,
     COLON = 268,
     TYPE = 269,
     BOOL = 270,
     NAT = 271,
     ID = 272,
     TIME = 273,
     BP = 274,
     EP = 275,
     PROC_SEP = 276,
     SEP = 277,
     COMMA = 278,
     IS = 279,
     ASSIGNMENT = 280,
     MINUS = 281,
     PLUS = 282,
     GG = 283,
     LBRACE = 284,
     RBRACE = 285,
     LBRACKET = 286,
     RBRACKET = 287,
     AND = 288,
     OR = 289,
     GUARD = 290,
     NOT = 291,
     OLD = 292,
     NUMBER = 293,
     INT = 294,
     REALNUMBER = 295,
     TRUE = 296,
     FALSE = 297,
     DOT = 298,
     DEADLOCK = 299,
     IMPLIES = 300,
     NOTEQUAL = 301,
     GEQ = 302,
     LEQ = 303,
     MAX = 304,
     MIN = 305,
     DIV = 306,
     MOD = 307,
     POWER = 308,
     RECV = 309,
     EXCLAMATION = 310,
     SENDRECV = 311,
     RECVSEND = 312,
     SSEND = 313,
     RRECV = 314,
     STAR = 315,
     GUARD_REP = 316,
     DERIVATIVE = 317,
     SQLBRACKET = 318,
     SQRBRACKET = 319,
     LSUBTRACT = 320,
     CONCAT = 321,
     IN = 322,
     HEAD = 323,
     TAIL = 324,
     RHEAD = 325,
     RTAIL = 326,
     LENGTH = 327,
     TAKE = 328,
     DROP = 329,
     SORT = 330,
     INSERT = 331,
     LESS = 332,
     GREATER = 333,
     HASH = 334,
     UNION = 335,
     SUB = 336,
     INTERSECTION = 337,
     PICK = 338,
     DIVIDE = 339
   };
#endif


/* Copy the first part of user declarations.  */
#line 1 "chiparser.yy"


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

void BinTypeCheck(ATermAppl arg1, ATermAppl arg2, std::string type);
void UnaryTypeCheck(ATermAppl arg1, std::string type);
bool ContainerTypeChecking(ATermAppl arg1, ATermAppl arg2);



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE 
#line 59 "chiparser.yy"
{
  ATermAppl appl;
  ATermList list;
}
/* Line 2616 of glr.c.  */
#line 196 "chiparser.hpp"
	YYSTYPE;
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{

  char yydummy;

} YYLTYPE;
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


extern YYSTYPE chiyylval;



