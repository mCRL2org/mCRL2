/* A Bison parser, made by GNU Bison 2.3.  */
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING).

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
     TAG_MULT_ACT = 260,
     TAG_PROC_EXPR = 261,
     TAG_PROC_SPEC = 262,
     TAG_PBES_SPEC = 263,
     TAG_STATE_FRM = 264,
     TAG_ACTION_RENAME = 265,
     LMERGE = 266,
     ARROW = 267,
     LTE = 268,
     GTE = 269,
     CONS = 270,
     SNOC = 271,
     CONCAT = 272,
     EQ = 273,
     NEQ = 274,
     AND = 275,
     BARS = 276,
     IMP = 277,
     BINIT = 278,
     ELSE = 279,
     STAR = 280,
     PLUS = 281,
     MINUS = 282,
     EQUALS = 283,
     DOT = 284,
     COMMA = 285,
     COLON = 286,
     SEMICOLON = 287,
     QMARK = 288,
     EXCLAM = 289,
     AT = 290,
     HASH = 291,
     BAR = 292,
     LPAR = 293,
     RPAR = 294,
     PBRACK = 295,
     LBRACK = 296,
     RBRACK = 297,
     LANG = 298,
     RANG = 299,
     PBRACE = 300,
     LBRACE = 301,
     RBRACE = 302,
     KWSORT = 303,
     KWCONS = 304,
     KWMAP = 305,
     KWVAR = 306,
     KWEQN = 307,
     KWACT = 308,
     KWPROC = 309,
     KWPBES = 310,
     KWINIT = 311,
     KWSTRUCT = 312,
     BOOL = 313,
     POS = 314,
     NAT = 315,
     INT = 316,
     REAL = 317,
     LIST = 318,
     SET = 319,
     BAG = 320,
     CTRUE = 321,
     CFALSE = 322,
     DIV = 323,
     MOD = 324,
     IN = 325,
     LAMBDA = 326,
     FORALL = 327,
     EXISTS = 328,
     WHR = 329,
     END = 330,
     DELTA = 331,
     TAU = 332,
     SUM = 333,
     BLOCK = 334,
     ALLOW = 335,
     HIDE = 336,
     RENAME = 337,
     COMM = 338,
     VAL = 339,
     MU = 340,
     NU = 341,
     DELAY = 342,
     YALED = 343,
     NIL = 344,
     ID = 345,
     NUMBER = 346
   };
#endif


/* Copy the first part of user declarations.  */
#line 18 "mcrl2parser.yy"


#include <stdio.h>
#include <string.h>

#include <aterm2.h>
#include "mcrl2/utilities/aterm_ext.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/struct.h"

#ifdef __cplusplus
using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;
#endif

//Global precondition: the ATerm library has been initialised

//external declarations from mcrl2lexer.l
void mcrl2yyerror(const char *s);
int mcrl2yylex(void);
extern ATermAppl spec_tree;
extern ATermIndexedSet parser_protect_table;

#ifdef _MSC_VER
#define yyfalse 0
#define yytrue 1
#endif

#define YYMAXDEPTH 640000

//local declarations
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
#line 76 "mcrl2parser.yy"
{
  ATermAppl appl;
  ATermList list;
}
/* Line 2604 of glr.c.  */
#line 203 "mcrl2parser.hpp"
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



