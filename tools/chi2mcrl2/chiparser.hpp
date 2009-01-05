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
     VAL = 265,
     SKIP = 266,
     BARS = 267,
     ALT = 268,
     COLON = 269,
     TYPE = 270,
     BOOL = 271,
     NAT = 272,
     REAL = 273,
     VOID = 274,
     ID = 275,
     TIME = 276,
     BP = 277,
     EP = 278,
     PROC_SEP = 279,
     SEP = 280,
     COMMA = 281,
     IS = 282,
     ASSIGNMENT = 283,
     MINUS = 284,
     PLUS = 285,
     GG = 286,
     LBRACE = 287,
     RBRACE = 288,
     LBRACKET = 289,
     RBRACKET = 290,
     AND = 291,
     OR = 292,
     GUARD = 293,
     NOT = 294,
     OLD = 295,
     NUMBER = 296,
     INT = 297,
     REALNUMBER = 298,
     TRUE = 299,
     FALSE = 300,
     DOT = 301,
     DEADLOCK = 302,
     IMPLIES = 303,
     NOTEQUAL = 304,
     GEQ = 305,
     LEQ = 306,
     MAX = 307,
     MIN = 308,
     DIV = 309,
     MOD = 310,
     POWER = 311,
     RECV = 312,
     EXCLAMATION = 313,
     SENDRECV = 314,
     RECVSEND = 315,
     SSEND = 316,
     RRECV = 317,
     STAR = 318,
     GUARD_REP = 319,
     DERIVATIVE = 320,
     SQLBRACKET = 321,
     SQRBRACKET = 322,
     LSUBTRACT = 323,
     CONCAT = 324,
     IN = 325,
     HEAD = 326,
     TAIL = 327,
     RHEAD = 328,
     RTAIL = 329,
     LENGTH = 330,
     TAKE = 331,
     DROP = 332,
     SORT = 333,
     INSERT = 334,
     LESS = 335,
     GREATER = 336,
     HASH = 337,
     UNION = 338,
     SUB = 339,
     INTERSECTION = 340,
     PICK = 341,
     DELAY = 342,
     DIVIDE = 343,
     SEND = 344
   };
#endif


/* Copy the first part of user declarations.  */
#line 1 "chiparser.yy"


#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include "libstruct_core.h"
#include "aterm2.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/aterm_ext.h"
#include <list>
#include <map>
#include <set>
#include <utility>
#include <cctype>
#include <cstring>


/*extern int yyerror(const char *s);
extern int yylex( void );
extern char* yytext; */

using namespace mcrl2::core;
using namespace std;

//external declarations from lexer.ll
void chiyyerror( const char *s );
extern void chigetposition();
int chiyylex( void );
extern ATermAppl chi_spec_tree;
extern ATermIndexedSet chi_parser_protect_table;
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

#define safe_assign(lhs, rhs) { ATbool b; ATindexedSetPut(chi_parser_protect_table, (ATerm) rhs, &b); lhs = rhs; }

void BinTypeCheck(ATermAppl arg1, ATermAppl arg2, std::string type);
void UnaryTypeCheck(ATermAppl arg1, std::string type);
bool ContainerTypeChecking(ATermAppl arg1, ATermAppl arg2);
bool is_number(std::string s);



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE 
#line 60 "chiparser.yy"
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



