/* A Bison parser, made by GNU Bison 2.0.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004 Free Software Foundation, Inc.

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
     EOLN = 258,
     SECSEP = 259,
     LPAR = 260,
     RPAR = 261,
     FANIN = 262,
     FANOUT = 263,
     NODENR = 264,
     ARROW = 265,
     NUMBER = 266,
     ID = 267,
     QUOTED = 268
   };
#endif
#define EOLN 258
#define SECSEP 259
#define LPAR 260
#define RPAR 261
#define FANIN 262
#define FANOUT 263
#define NODENR 264
#define ARROW 265
#define NUMBER 266
#define ID 267
#define QUOTED 268




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 29 "fsmparser.yy"
typedef union YYSTYPE {
  ATermAppl aterm;
  int number;
} YYSTYPE;
/* Line 1318 of yacc.c.  */
#line 68 "fsmparser.hpp"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE fsmlval;



