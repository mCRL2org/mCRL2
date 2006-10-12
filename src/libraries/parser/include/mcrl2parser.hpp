/* A Bison parser, made by GNU Bison 2.2.  */

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
     TAG_SPEC = 262,
     TAG_STATE_FRM = 263,
     LMERGE = 264,
     ARROW = 265,
     LTE = 266,
     GTE = 267,
     CONS = 268,
     SNOC = 269,
     CONCAT = 270,
     EQ = 271,
     NEQ = 272,
     AND = 273,
     BARS = 274,
     IMP = 275,
     BINIT = 276,
     ELSE = 277,
     STAR = 278,
     PLUS = 279,
     MINUS = 280,
     EQUALS = 281,
     DOT = 282,
     COMMA = 283,
     COLON = 284,
     SEMICOLON = 285,
     QMARK = 286,
     EXCLAM = 287,
     AT = 288,
     HASH = 289,
     BAR = 290,
     LPAR = 291,
     RPAR = 292,
     PBRACK = 293,
     LBRACK = 294,
     RBRACK = 295,
     LANG = 296,
     RANG = 297,
     PBRACE = 298,
     LBRACE = 299,
     RBRACE = 300,
     KWSORT = 301,
     KWCONS = 302,
     KWMAP = 303,
     KWVAR = 304,
     KWEQN = 305,
     KWACT = 306,
     KWPROC = 307,
     KWINIT = 308,
     KWSTRUCT = 309,
     BOOL = 310,
     POS = 311,
     NAT = 312,
     INT = 313,
     REAL = 314,
     LIST = 315,
     SET = 316,
     BAG = 317,
     CTRUE = 318,
     CFALSE = 319,
     DIV = 320,
     MOD = 321,
     IN = 322,
     LAMBDA = 323,
     FORALL = 324,
     EXISTS = 325,
     WHR = 326,
     END = 327,
     DELTA = 328,
     TAU = 329,
     SUM = 330,
     BLOCK = 331,
     ALLOW = 332,
     HIDE = 333,
     RENAME = 334,
     COMM = 335,
     VAL = 336,
     MU = 337,
     NU = 338,
     DELAY = 339,
     YALED = 340,
     NIL = 341,
     ID = 342,
     NUMBER = 343
   };
#endif


/* Copy the first part of user declarations.  */
#line 10 "../source/mcrl2parser.yy"


#include <stdio.h>
#include <string.h>

#include <aterm2.h>
#include "libstruct.h"
#include "liblowlevel.h"
#include "libprint_c.h"

//Global precondition: the ATerm library has been initialised

//external declarations from mcrl2lexer.l
void mcrl2yyerror(const char *s);
int mcrl2yylex(void);
extern ATermAppl spec_tree;

#ifdef _MSC_VER
#define yyfalse 0
#define yytrue 1
#endif

#define YYMAXDEPTH 40000

//local declarations
ATermAppl gsSpecEltsToSpec(ATermList SpecElts);
//Pre: SpecElts contains one initialisation and zero or more occurrences of
//     sort, constructor, operation, equation, action and process
//     specifications.
//Ret: specification containing one sort, constructor, operation, equation,
//     action and process specification, and one initialisation, in that order.


#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE 
#line 43 "../source/mcrl2parser.yy"
{
  ATermAppl appl;
  ATermList list;
}
/* Line 2609 of glr.c.  */
#line 175 "../source/mcrl2parser.hpp"
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



