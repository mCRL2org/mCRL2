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
     TAG_ACTION_RENAME = 266,
     LMERGE = 267,
     ARROW = 268,
     LTE = 269,
     GTE = 270,
     CONS = 271,
     SNOC = 272,
     CONCAT = 273,
     EQ = 274,
     NEQ = 275,
     AND = 276,
     BARS = 277,
     IMP = 278,
     BINIT = 279,
     ELSE = 280,
     STAR = 281,
     PLUS = 282,
     MINUS = 283,
     EQUALS = 284,
     DOT = 285,
     COMMA = 286,
     COLON = 287,
     SEMICOLON = 288,
     QMARK = 289,
     EXCLAM = 290,
     AT = 291,
     HASH = 292,
     BAR = 293,
     LPAR = 294,
     RPAR = 295,
     PBRACK = 296,
     LBRACK = 297,
     RBRACK = 298,
     LANG = 299,
     RANG = 300,
     PBRACE = 301,
     LBRACE = 302,
     RBRACE = 303,
     KWSORT = 304,
     KWCONS = 305,
     KWMAP = 306,
     KWVAR = 307,
     KWEQN = 308,
     KWACT = 309,
     KWPROC = 310,
     KWPBES = 311,
     KWINIT = 312,
     KWSTRUCT = 313,
     BOOL = 314,
     POS = 315,
     NAT = 316,
     INT = 317,
     REAL = 318,
     LIST = 319,
     SET = 320,
     BAG = 321,
     CTRUE = 322,
     CFALSE = 323,
     DIV = 324,
     MOD = 325,
     IN = 326,
     LAMBDA = 327,
     FORALL = 328,
     EXISTS = 329,
     WHR = 330,
     END = 331,
     DELTA = 332,
     TAU = 333,
     SUM = 334,
     BLOCK = 335,
     ALLOW = 336,
     HIDE = 337,
     RENAME = 338,
     COMM = 339,
     VAL = 340,
     MU = 341,
     NU = 342,
     DELAY = 343,
     YALED = 344,
     NIL = 345,
     ID = 346,
     NUMBER = 347
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
  ATermAppl appl;
  ATermList list;
}
/* Line 2604 of glr.c.  */
#line 208 "mcrl2parser.hpp"
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



