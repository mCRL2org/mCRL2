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
     TAG_SORT_EXPR = 258,
     TAG_DATA_EXPR = 259,
     TAG_DATA_SPEC = 260,
     TAG_MULT_ACT = 261,
     TAG_PROC_EXPR = 262,
     TAG_PROC_SPEC = 263,
     TAG_PBES_SPEC = 264,
     TAG_STATE_FRM = 265,
     TAG_DATA_VARS = 266,
     TAG_ACTION_RENAME = 267,
     LMERGE = 268,
     ARROW = 269,
     LTE = 270,
     GTE = 271,
     CONS = 272,
     SNOC = 273,
     CONCAT = 274,
     EQ = 275,
     NEQ = 276,
     AND = 277,
     BARS = 278,
     IMP = 279,
     BINIT = 280,
     ELSE = 281,
     STAR = 282,
     PLUS = 283,
     MINUS = 284,
     EQUALS = 285,
     DOT = 286,
     COMMA = 287,
     COLON = 288,
     SEMICOLON = 289,
     QMARK = 290,
     EXCLAM = 291,
     AT = 292,
     HASH = 293,
     BAR = 294,
     LPAR = 295,
     RPAR = 296,
     PBRACK = 297,
     LBRACK = 298,
     RBRACK = 299,
     LANG = 300,
     RANG = 301,
     PBRACE = 302,
     LBRACE = 303,
     RBRACE = 304,
     KWSORT = 305,
     KWCONS = 306,
     KWMAP = 307,
     KWVAR = 308,
     KWEQN = 309,
     KWACT = 310,
     KWPROC = 311,
     KWPBES = 312,
     KWINIT = 313,
     KWSTRUCT = 314,
     BOOL = 315,
     POS = 316,
     NAT = 317,
     INT = 318,
     REAL = 319,
     LIST = 320,
     SET = 321,
     BAG = 322,
     CTRUE = 323,
     CFALSE = 324,
     DIV = 325,
     MOD = 326,
     IN = 327,
     LAMBDA = 328,
     FORALL = 329,
     EXISTS = 330,
     WHR = 331,
     END = 332,
     DELTA = 333,
     TAU = 334,
     SUM = 335,
     BLOCK = 336,
     ALLOW = 337,
     HIDE = 338,
     RENAME = 339,
     COMM = 340,
     VAL = 341,
     MU = 342,
     NU = 343,
     DELAY = 344,
     YALED = 345,
     NIL = 346,
     ID = 347,
     NUMBER = 348
   };
#endif


/* Copy the first part of user declarations.  */
#line 18 "mcrl2parser.yy"


#include <stdio.h>
#include <string.h>

#include <aterm2.h>
#include "mcrl2/utilities/aterm_ext.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/detail/struct.h"

using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;

//Global precondition: the ATerm library has been initialised

//external declarations from mcrl2lexer.ll
void mcrl2yyerror(const char *s);
int mcrl2yylex(void);
extern ATerm spec_tree;
extern ATermIndexedSet parser_protect_table;

#ifdef _MSC_VER
#define yyfalse 0
#define yytrue 1
#endif

#define YYMAXDEPTH 640000

//local declarations
ATermAppl gsDataSpecEltsToSpec(ATermList SpecElts);
//Pre: SpecElts contains zero or more occurrences of sort, constructor,
//     operation and data equation specifications.
//Ret: data specification containing one sort, constructor, operation,
//     and data equation specification, in that order.

ATermAppl gsProcSpecEltsToSpec(ATermList SpecElts);
//Pre: SpecElts contains one process initialisation and zero or more
//     occurrences of sort, constructor, operation, data equation, action and
//     process equation specifications.
//Ret: process specification containing one sort, constructor, operation,
//     data equation, action and process equation specification, and one
//     process initialisation, in that order.

ATermAppl gsActionRenameEltsToActionRename(ATermList SpecElts);
//Pre: ActionRenameElts contains zero or more occurrences of
//     sort, constructor, operation, equation, action and action rename
//     rules.
//Ret: specification containing one sort, constructor, operation, equation,
//     action and action rename rules in that order.

ATermAppl gsPBESSpecEltsToSpec(ATermList SpecElts);
//Pre: SpecElts contains one parameterised boolean initialisation and zero or
//     more occurrences of sort, constructor, operation, data equation, action
//     and parameterised boolean equation specifications.
//Ret: BPES specification containing one sort, constructor, operation,
//     data equation, action and parameterised boolean equation specification,
//     and one parameterised boolean initialisation, in that order.

#define safe_assign(lhs, rhs) { ATbool b; lhs = rhs; ATindexedSetPut(parser_protect_table, (ATerm) lhs, &b); }


#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE 
#line 80 "mcrl2parser.yy"
{
  ATerm term;
  ATermAppl appl;
  ATermList list;
}
/* Line 2604 of glr.c.  */
#line 210 "mcrl2parser.hpp"
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


extern YYSTYPE mcrl2yylval;



