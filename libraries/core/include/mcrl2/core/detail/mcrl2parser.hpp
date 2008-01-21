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
     TAG_MULT_ACT = 260,
     TAG_PROC_EXPR = 261,
     TAG_PROC_SPEC = 262,
     TAG_STATE_FRM = 263,
     TAG_ACTION_RENAME = 264,
     LMERGE = 265,
     ARROW = 266,
     LTE = 267,
     GTE = 268,
     CONS = 269,
     SNOC = 270,
     CONCAT = 271,
     EQ = 272,
     NEQ = 273,
     AND = 274,
     BARS = 275,
     IMP = 276,
     BINIT = 277,
     ELSE = 278,
     STAR = 279,
     PLUS = 280,
     MINUS = 281,
     EQUALS = 282,
     DOT = 283,
     COMMA = 284,
     COLON = 285,
     SEMICOLON = 286,
     QMARK = 287,
     EXCLAM = 288,
     AT = 289,
     HASH = 290,
     BAR = 291,
     LPAR = 292,
     RPAR = 293,
     PBRACK = 294,
     LBRACK = 295,
     RBRACK = 296,
     LANG = 297,
     RANG = 298,
     PBRACE = 299,
     LBRACE = 300,
     RBRACE = 301,
     KWSORT = 302,
     KWCONS = 303,
     KWMAP = 304,
     KWVAR = 305,
     KWEQN = 306,
     KWACT = 307,
     KWPROC = 308,
     KWINIT = 309,
     KWSTRUCT = 310,
     BOOL = 311,
     POS = 312,
     NAT = 313,
     INT = 314,
     REAL = 315,
     LIST = 316,
     SET = 317,
     BAG = 318,
     CTRUE = 319,
     CFALSE = 320,
     DIV = 321,
     MOD = 322,
     IN = 323,
     LAMBDA = 324,
     FORALL = 325,
     EXISTS = 326,
     WHR = 327,
     END = 328,
     DELTA = 329,
     TAU = 330,
     SUM = 331,
     BLOCK = 332,
     ALLOW = 333,
     HIDE = 334,
     RENAME = 335,
     COMM = 336,
     VAL = 337,
     MU = 338,
     NU = 339,
     DELAY = 340,
     YALED = 341,
     NIL = 342,
     ID = 343,
     NUMBER = 344
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

#define YYMAXDEPTH 160000

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

#define safe_assign(lhs, rhs) { ATbool b; ATindexedSetPut(parser_protect_table, (ATerm) rhs, &b); lhs = rhs; }


#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE 
#line 68 "mcrl2parser.yy"
{
  ATermAppl appl;
  ATermList list;
}
/* Line 2616 of glr.c.  */
#line 193 "mcrl2parser.hpp"
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



