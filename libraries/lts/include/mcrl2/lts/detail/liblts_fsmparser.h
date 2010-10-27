/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

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
     EOLN = 258,
     SECSEP = 259,
     LPAR = 260,
     RPAR = 261,
     ARROW = 262,
     HASH = 263,
     QMARK = 264,
     COLON = 265,
     COMMA = 266,
     BAG = 267,
     BAR = 268,
     KWSTRUCT = 269,
     SET = 270,
     LIST = 271,
     NUMBER = 272,
     ID = 273,
     QUOTED = 274,
     BOOL = 275,
     POS = 276,
     NAT = 277,
     INT = 278,
     REAL = 279
   };
#endif
/* Tokens.  */
#define EOLN 258
#define SECSEP 259
#define LPAR 260
#define RPAR 261
#define ARROW 262
#define HASH 263
#define QMARK 264
#define COLON 265
#define COMMA 266
#define BAG 267
#define BAR 268
#define KWSTRUCT 269
#define SET 270
#define LIST 271
#define NUMBER 272
#define ID 273
#define QUOTED 274
#define BOOL 275
#define POS 276
#define NAT 277
#define INT 278
#define REAL 279




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE fsmyylval;

