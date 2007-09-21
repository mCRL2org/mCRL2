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
     VAR = 261,
     CONST = 262,
     CHAN = 263,
     SKIP = 264,
     BARS = 265,
     ALT = 266,
     COLON = 267,
     TYPE = 268,
     BOOL = 269,
     NAT = 270,
     ID = 271,
     TIME = 272,
     BP = 273,
     EP = 274,
     PROC_SEP = 275,
     SEP = 276,
     COMMA = 277,
     DEFINES = 278,
     ASSIGNMENT = 279,
     MINUS = 280,
     PLUS = 281,
     GG = 282,
     LBRACE = 283,
     RBRACE = 284,
     LBRACKET = 285,
     RBRACKET = 286,
     AND = 287,
     OR = 288,
     GUARD = 289,
     NOT = 290,
     EQUAL = 291,
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
     DIVIDE = 318
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

#define YYMAXDEPTH 160000

//local declarations
ATermAppl gsSpecEltsToSpec(ATermAppl SpecElts);
//Pre: SpecElts contains one initialisation and zero or more occurrences of
//     sort, constructor, operation, equation, action and process
//     specifications.
//Ret: specification containing one sort, constructor, operation, equation,
//     action and process specification, and one initialisation, in that order.

#define safe_assign(lhs, rhs) { ATbool b; ATindexedSetPut(parser_protect_table, (ATerm) rhs, &b); lhs = rhs; }



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE 
#line 47 "chiparser.yy"
{
  ATermAppl appl;
  ATermList list;
}
/* Line 2616 of glr.c.  */
#line 163 "chiparser.hpp"
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



