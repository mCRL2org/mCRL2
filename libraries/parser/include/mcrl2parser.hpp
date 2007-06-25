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
#define TAG_SORT_EXPR 258
#define TAG_DATA_EXPR 259
#define TAG_MULT_ACT 260
#define TAG_PROC_EXPR 261
#define TAG_SPEC 262
#define TAG_STATE_FRM 263
#define TAG_ACTION_RENAME 264
#define LMERGE 265
#define ARROW 266
#define LTE 267
#define GTE 268
#define CONS 269
#define SNOC 270
#define CONCAT 271
#define EQ 272
#define NEQ 273
#define AND 274
#define BARS 275
#define IMP 276
#define BINIT 277
#define ELSE 278
#define STAR 279
#define PLUS 280
#define MINUS 281
#define EQUALS 282
#define DOT 283
#define COMMA 284
#define COLON 285
#define SEMICOLON 286
#define QMARK 287
#define EXCLAM 288
#define AT 289
#define HASH 290
#define BAR 291
#define LPAR 292
#define RPAR 293
#define PBRACK 294
#define LBRACK 295
#define RBRACK 296
#define LANG 297
#define RANG 298
#define PBRACE 299
#define LBRACE 300
#define RBRACE 301
#define KWSORT 302
#define KWCONS 303
#define KWMAP 304
#define KWVAR 305
#define KWEQN 306
#define KWACT 307
#define KWPROC 308
#define KWINIT 309
#define KWSTRUCT 310
#define BOOL 311
#define POS 312
#define NAT 313
#define INT 314
#define REAL 315
#define LIST 316
#define SET 317
#define BAG 318
#define CTRUE 319
#define CFALSE 320
#define DIV 321
#define MOD 322
#define IN 323
#define LAMBDA 324
#define FORALL 325
#define EXISTS 326
#define WHR 327
#define END 328
#define DELTA 329
#define TAU 330
#define SUM 331
#define BLOCK 332
#define ALLOW 333
#define HIDE 334
#define RENAME 335
#define COMM 336
#define VAL 337
#define MU 338
#define NU 339
#define DELAY 340
#define YALED 341
#define NIL 342
#define ID 343
#define NUMBER 344




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 57 "mcrl2parser.yy"
typedef union YYSTYPE {
  ATermAppl appl;
  ATermList list;
} YYSTYPE;
/* Line 2089 of glr.c.  */
#line 215 "mcrl2parser.hpp"
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



