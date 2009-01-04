
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison GLR parsers in C
   
      Copyright (C) 2002, 2003, 2004, 2005, 2006 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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


#ifndef YYSTYPE
typedef union YYSTYPE
{

/* Line 2638 of glr.c  */
#line 60 "chiparser.yy"

  ATermAppl appl;
  ATermList list;



/* Line 2638 of glr.c  */
#line 146 "chiparser.hpp"
} YYSTYPE;
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



