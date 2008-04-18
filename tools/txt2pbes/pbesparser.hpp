/* A Bison parser, made by GNU Bison 1.875c.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003 Free Software Foundation, Inc.

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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     IDPRED = 258,
     IDPROP = 259,
     IDDATA = 260,
     TRUE = 261,
     FALSE = 262,
     ZERO = 263,
     ONE = 264,
     LESS = 265,
     IS = 266,
     PLUS = 267,
     EN = 268,
     OF = 269,
     NEG = 270,
     COMMA = 271,
     EQ = 272,
     MU = 273,
     NU = 274,
     FORALL = 275,
     EXISTS = 276,
     LPAR = 277,
     RPAR = 278
   };
#endif
#define IDPRED 258
#define IDPROP 259
#define IDDATA 260
#define TRUE 261
#define FALSE 262
#define ZERO 263
#define ONE 264
#define LESS 265
#define IS 266
#define PLUS 267
#define EN 268
#define OF 269
#define NEG 270
#define COMMA 271
#define EQ 272
#define MU 273
#define NU 274
#define FORALL 275
#define EXISTS 276
#define LPAR 277
#define RPAR 278




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 25 "pbesparser.yy"
typedef union YYSTYPE {
	int position; 
	char* string;
} YYSTYPE;
/* Line 1275 of yacc.c.  */
#line 88 "pbesparser.hpp"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE pbeslval;



