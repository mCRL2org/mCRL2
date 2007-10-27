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
     VOID = 272,
     ID = 273,
     TIME = 274,
     BP = 275,
     EP = 276,
     PROC_SEP = 277,
     SEP = 278,
     COMMA = 279,
     IS = 280,
     ASSIGNMENT = 281,
     MINUS = 282,
     PLUS = 283,
     GG = 284,
     LBRACE = 285,
     RBRACE = 286,
     LBRACKET = 287,
     RBRACKET = 288,
     AND = 289,
     OR = 290,
     GUARD = 291,
     NOT = 292,
     OLD = 293,
     NUMBER = 294,
     INT = 295,
     REALNUMBER = 296,
     TRUE = 297,
     FALSE = 298,
     DOT = 299,
     DEADLOCK = 300,
     IMPLIES = 301,
     NOTEQUAL = 302,
     GEQ = 303,
     LEQ = 304,
     MAX = 305,
     MIN = 306,
     DIV = 307,
     MOD = 308,
     POWER = 309,
     RECV = 310,
     EXCLAMATION = 311,
     SENDRECV = 312,
     RECVSEND = 313,
     SSEND = 314,
     RRECV = 315,
     STAR = 316,
     GUARD_REP = 317,
     DERIVATIVE = 318,
     SQLBRACKET = 319,
     SQRBRACKET = 320,
     LSUBTRACT = 321,
     CONCAT = 322,
     IN = 323,
     HEAD = 324,
     TAIL = 325,
     RHEAD = 326,
     RTAIL = 327,
     LENGTH = 328,
     TAKE = 329,
     DROP = 330,
     SORT = 331,
     INSERT = 332,
     LESS = 333,
     GREATER = 334,
     HASH = 335,
     UNION = 336,
     SUB = 337,
     INTERSECTION = 338,
     PICK = 339,
     DELAY = 340,
     DIVIDE = 341,
     SEND = 342
   };
#endif


/* Copy the first part of user declarations.  */
#line 1 "chiparser.yy"


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include "libstruct_core.h"
#include <aterm2.h>
#include "print/messaging.h"
#include "mcrl2/utilities/aterm_ext.h"
#include <list>
#include <map>
#include <set>
#include <utility>
#include <cctype>
#include <string.h>


/*extern int yyerror(const char *s);
extern int yylex( void );
extern char* yytext; */

#ifdef __cplusplus
using namespace ::mcrl2::utilities;
using namespace std;
#endif

//external declarations from lexer.l
void chiyyerror( const char *s );
extern void chigetposition();
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
bool is_number(std::string s);



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE 
#line 62 "chiparser.yy"
{
  ATermAppl appl;
  ATermList list;
}
/* Line 2604 of glr.c.  */
#line 202 "chiparser.hpp"
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



