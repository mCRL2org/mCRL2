/* A Bison parser, made by GNU Bison 2.0.  */

/* Skeleton parser for GLR parsing with Bison,
   Copyright (C) 2002, 2003, 2004 Free Software Foundation, Inc.

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
#define TAG_SORT_EXPR 258
#define TAG_DATA_EXPR 259
#define TAG_MULT_ACT 260
#define TAG_PROC_EXPR 261
#define TAG_SPEC 262
#define TAG_STATE_FRM 263
#define LMERGE 264
#define ARROW 265
#define LTE 266
#define GTE 267
#define CONS 268
#define SNOC 269
#define CONCAT 270
#define EQ 271
#define NEQ 272
#define AND 273
#define BARS 274
#define IMP 275
#define BINIT 276
#define ELSE 277
#define STAR 278
#define PLUS 279
#define MINUS 280
#define EQUALS 281
#define DOT 282
#define COMMA 283
#define COLON 284
#define SEMICOLON 285
#define QMARK 286
#define EXCLAM 287
#define AT 288
#define HASH 289
#define BAR 290
#define LPAR 291
#define RPAR 292
#define PBRACK 293
#define LBRACK 294
#define RBRACK 295
#define LANG 296
#define RANG 297
#define PBRACE 298
#define LBRACE 299
#define RBRACE 300
#define KWSORT 301
#define KWCONS 302
#define KWMAP 303
#define KWVAR 304
#define KWEQN 305
#define KWACT 306
#define KWPROC 307
#define KWINIT 308
#define KWSTRUCT 309
#define BOOL 310
#define POS 311
#define NAT 312
#define INT 313
#define REAL 314
#define LIST 315
#define SET 316
#define BAG 317
#define CTRUE 318
#define CFALSE 319
#define DIV 320
#define MOD 321
#define IN 322
#define LAMBDA 323
#define FORALL 324
#define EXISTS 325
#define WHR 326
#define END 327
#define DELTA 328
#define TAU 329
#define SUM 330
#define BLOCK 331
#define ALLOW 332
#define HIDE 333
#define RENAME 334
#define COMM 335
#define VAL 336
#define MU 337
#define NU 338
#define DELAY 339
#define YALED 340
#define NIL 341
#define ID 342
#define NUMBER 343




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 46 "mcrl2parser.yy"
typedef union YYSTYPE {
  ATermAppl appl;
  ATermList list;
} YYSTYPE;
/* Line 2089 of glr.c.  */
#line 213 "mcrl2parser.hpp"
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE mcrl2yylval;

#if ! defined (YYLTYPE) && ! defined (YYLTYPE_IS_DECLARED)
typedef struct YYLTYPE
{

  char yydummy;

} YYLTYPE;
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif



