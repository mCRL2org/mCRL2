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
     TAG_IDENTIFIER = 258,
     TAG_SORT_EXPR = 259,
     TAG_DATA_EXPR = 260,
     TAG_DATA_SPEC = 261,
     TAG_MULT_ACT = 262,
     TAG_PROC_EXPR = 263,
     TAG_PROC_SPEC = 264,
     TAG_PBES_SPEC = 265,
     TAG_STATE_FRM = 266,
     TAG_DATA_VARS = 267,
     TAG_ACTION_RENAME = 268,
     LMERGE = 269,
     ARROW = 270,
     LTE = 271,
     GTE = 272,
     CONS = 273,
     SNOC = 274,
     CONCAT = 275,
     EQ = 276,
     NEQ = 277,
     AND = 278,
     BARS = 279,
     IMP = 280,
     BINIT = 281,
     ELSE = 282,
     SLASH = 283,
     STAR = 284,
     PLUS = 285,
     MINUS = 286,
     EQUALS = 287,
     DOT = 288,
     COMMA = 289,
     COLON = 290,
     SEMICOLON = 291,
     QMARK = 292,
     EXCLAM = 293,
     AT = 294,
     HASH = 295,
     BAR = 296,
     LPAR = 297,
     RPAR = 298,
     PBRACK = 299,
     LBRACK = 300,
     RBRACK = 301,
     LANG = 302,
     RANG = 303,
     PBRACE = 304,
     LBRACE = 305,
     RBRACE = 306,
     KWSORT = 307,
     KWCONS = 308,
     KWMAP = 309,
     KWVAR = 310,
     KWEQN = 311,
     KWACT = 312,
     KWPROC = 313,
     KWPBES = 314,
     KWINIT = 315,
     KWSTRUCT = 316,
     BOOL = 317,
     POS = 318,
     NAT = 319,
     INT = 320,
     REAL = 321,
     LIST = 322,
     SET = 323,
     BAG = 324,
     CTRUE = 325,
     CFALSE = 326,
     DIV = 327,
     MOD = 328,
     IN = 329,
     LAMBDA = 330,
     FORALL = 331,
     EXISTS = 332,
     WHR = 333,
     END = 334,
     DELTA = 335,
     TAU = 336,
     SUM = 337,
     BLOCK = 338,
     ALLOW = 339,
     HIDE = 340,
     RENAME = 341,
     COMM = 342,
     VAL = 343,
     MU = 344,
     NU = 345,
     DELAY = 346,
     YALED = 347,
     NIL = 348,
     ID = 349,
     NUMBER = 350
   };
#endif


/* Copy the first part of user declarations.  */
#line 18 "mcrl2parser.yy"


#include <stdio.h>
#include <string.h>

#include <aterm2.h>
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/detail/struct.h"

using namespace mcrl2::core;
using namespace mcrl2::core::detail;

//Global precondition: the ATerm library has been initialised

//external declarations from mcrl2lexer.ll
void mcrl2yyerror(const char *s);
int mcrl2yylex(void);
extern ATerm mcrl2_spec_tree;
extern ATermIndexedSet mcrl2_parser_protect_table;

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

#define safe_assign(lhs, rhs) { ATbool b; lhs = rhs; ATindexedSetPut(mcrl2_parser_protect_table, (ATerm) lhs, &b); }


#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE 
#line 79 "mcrl2parser.yy"
{
  ATerm term;
  ATermAppl appl;
  ATermList list;
}
/* Line 2604 of glr.c.  */
#line 211 "../include/mcrl2/core/detail/mcrl2parser.h"
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



