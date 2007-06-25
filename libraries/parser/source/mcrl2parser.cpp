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

/* This is the parser code for GLR (Generalized LR) parser. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <setjmp.h>

/* Identify Bison output.  */
#define YYBISON 1

/* Skeleton name.  */
#define YYSKELETON_NAME "glr.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0

/* Substitute the variable and function names.  */
#define yyparse mcrl2yyparse
#define yylex   mcrl2yylex
#define yyerror mcrl2yyerror
#define yylval  mcrl2yylval
#define yychar  mcrl2yychar
#define yydebug mcrl2yydebug
#define yynerrs mcrl2yynerrs
#define yylloc mcrl2yylloc

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




/* Copy the first part of user declarations.  */
#line 10 "mcrl2parser.yy"


#include <stdio.h>
#include <string.h>

#include <aterm2.h>
#include "print/messaging.h"
#include "mcrl2/utilities/aterm_ext.h"
#include "libstruct.h"

#ifdef __cplusplus
using namespace mcrl2::utilities;
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
ATermAppl gsSpecEltsToSpec(ATermList SpecElts);
//Pre: SpecElts contains one initialisation and zero or more occurrences of
//     sort, constructor, operation, equation, action and process
//     specifications.
//Ret: specification containing one sort, constructor, operation, equation,
//     action and process specification, and one initialisation, in that order.

ATermAppl gsActionRenameEltsToActionRename(ATermList SpecElts);
//Pre: ActionRenameElts contains zero or more occurrences of
//     sort, constructor, operation, equation, action and action rename
//     rules.
//Ret: specification containing one sort, constructor, operation, equation,
//     action and action rename rules in that order.

#define safe_assign(lhs, rhs) { ATbool b; ATindexedSetPut(parser_protect_table, (ATerm) rhs, &b); lhs = rhs; }


/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 57 "mcrl2parser.yy"
typedef union YYSTYPE {
  ATermAppl appl;
  ATermList list;
} YYSTYPE;
/* Line 186 of glr.c.  */
#line 307 "mcrl2parser.cpp"
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

#if ! defined (YYLTYPE) && ! defined (YYLTYPE_IS_DECLARED)
typedef struct YYLTYPE
{

  char yydummy;

} YYLTYPE;
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif

/* Default (constant) value used for initialization for null
   right-hand sides.  Unlike the standard yacc.c template,
   here we set the default value of $$ to a zeroed-out value.
   Since the default value is undefined, this behavior is
   technically correct. */
static YYSTYPE yyval_default;

/* Copy the second part of user declarations.  */


/* Line 217 of glr.c.  */
#line 334 "mcrl2parser.cpp"

#ifndef YYFREE
# define YYFREE free
#endif
#ifndef YYMALLOC
# define YYMALLOC malloc
#endif
#ifndef YYREALLOC
# define YYREALLOC realloc
#endif

#ifdef __cplusplus
   typedef bool yybool;
#else
   typedef unsigned char yybool;
#endif
#define yytrue 1
#define yyfalse 0

/*-----------------.
| GCC extensions.  |
`-----------------*/

#ifndef __attribute__
/* This feature is available in gcc versions 2.5 and later.  */
# if !defined (__GNUC__) || __GNUC__ < 2 || \
(__GNUC__ == 2 && __GNUC_MINOR__ < 5) || __STRICT_ANSI__
#  define __attribute__(Spec) /* empty */
# endif
#endif

#ifndef YYASSERT
# define YYASSERT(condition) ((void) ((condition) || (abort (), 0)))
#endif

#ifndef ATTRIBUTE_UNUSED
# define ATTRIBUTE_UNUSED __attribute__ ((__unused__))
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  143
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1483

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  90
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  145
/* YYNRULES -- Number of rules. */
#define YYNRULES  351
/* YYNRULES -- Number of states. */
#define YYNSTATES  737
/* YYMAXRHS -- Maximum number of symbols on right-hand side of rule. */
#define YYMAXRHS 6
/* YYMAXLEFT -- Maximum number of symbols to the left of a handle
   accessed by $0, $-1, etc., in any rule. */
#define YYMAXLEFT 0

/* YYTRANSLATE(X) -- Bison symbol number corresponding to X.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   344

#define YYTRANSLATE(YYX)						\
  ((YYX <= 0) ? YYEOF :							\
   (unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short int yyprhs[] =
{
       0,     0,     3,     6,     9,    12,    15,    18,    21,    24,
      26,    28,    32,    34,    36,    40,    42,    44,    47,    49,
      53,    56,    62,    63,    66,    68,    72,    74,    78,    80,
      82,    84,    88,    90,    92,    94,    96,    98,   103,   108,
     113,   115,   117,   122,   124,   128,   132,   134,   139,   144,
     149,   151,   155,   157,   162,   167,   169,   173,   177,   179,
     184,   189,   191,   195,   199,   201,   206,   211,   213,   217,
     221,   225,   229,   233,   235,   239,   241,   245,   247,   251,
     253,   257,   261,   263,   267,   271,   273,   277,   281,   283,
     286,   289,   292,   294,   299,   304,   306,   311,   313,   317,
     319,   321,   323,   325,   329,   331,   333,   335,   337,   339,
     343,   347,   351,   353,   357,   361,   367,   371,   373,   375,
     377,   381,   383,   388,   390,   392,   396,   398,   403,   405,
     409,   413,   415,   420,   422,   426,   428,   433,   435,   439,
     445,   447,   452,   454,   458,   460,   464,   466,   471,   475,
     481,   483,   488,   490,   494,   496,   500,   502,   506,   508,
     512,   514,   519,   523,   529,   531,   536,   538,   540,   542,
     546,   548,   550,   557,   564,   571,   578,   585,   587,   591,
     593,   597,   599,   603,   605,   609,   613,   615,   619,   621,
     625,   627,   631,   635,   637,   639,   643,   645,   649,   651,
     655,   657,   659,   662,   664,   666,   668,   670,   672,   674,
     676,   679,   682,   686,   688,   692,   694,   698,   701,   704,
     707,   711,   715,   717,   720,   725,   728,   732,   736,   742,
     745,   749,   753,   756,   759,   763,   765,   769,   772,   775,
     779,   783,   790,   792,   796,   800,   802,   804,   809,   814,
     820,   826,   827,   831,   833,   837,   843,   845,   849,   851,
     856,   861,   867,   873,   875,   879,   883,   885,   890,   895,
     901,   907,   909,   912,   917,   922,   926,   930,   932,   937,
     942,   948,   954,   959,   961,   963,   965,   967,   969,   973,
     975,   977,   979,   983,   985,   989,   991,   995,   997,  1001,
    1003,  1006,  1009,  1011,  1014,  1017,  1019,  1023,  1025,  1027,
    1031,  1033,  1035,  1040,  1045,  1047,  1051,  1053,  1058,  1063,
    1065,  1069,  1073,  1075,  1080,  1085,  1087,  1091,  1093,  1096,
    1098,  1103,  1108,  1110,  1115,  1117,  1119,  1123,  1125,  1127,
    1130,  1132,  1134,  1136,  1138,  1140,  1142,  1144,  1147,  1152,
    1155,  1159
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short int yyrhs[] =
{
      91,     0,    -1,     3,    92,    -1,     4,   106,    -1,     5,
     135,    -1,     6,   138,    -1,     7,   172,    -1,     8,   197,
      -1,     9,   228,    -1,    93,    -1,    97,    -1,    94,    11,
      93,    -1,    95,    -1,    96,    -1,    95,    35,    96,    -1,
      97,    -1,   103,    -1,    55,    98,    -1,    99,    -1,    98,
      36,    99,    -1,    88,   100,    -1,    88,    37,   101,    38,
     100,    -1,    -1,    32,    88,    -1,   102,    -1,   101,    29,
     102,    -1,    92,    -1,    88,    30,    92,    -1,    88,    -1,
     104,    -1,   105,    -1,    37,    92,    38,    -1,    56,    -1,
      57,    -1,    58,    -1,    59,    -1,    60,    -1,    61,    37,
      92,    38,    -1,    62,    37,    92,    38,    -1,    63,    37,
      92,    38,    -1,   107,    -1,   110,    -1,   107,    72,   108,
      73,    -1,   109,    -1,   108,    29,   109,    -1,    88,    27,
     106,    -1,   111,    -1,    69,   195,    28,   110,    -1,    70,
     195,    28,   110,    -1,    71,   195,    28,   110,    -1,   113,
      -1,   113,    21,   112,    -1,   111,    -1,    70,   195,    28,
     112,    -1,    71,   195,    28,   112,    -1,   115,    -1,   115,
      19,   114,    -1,   115,    20,   114,    -1,   113,    -1,    70,
     195,    28,   114,    -1,    71,   195,    28,   114,    -1,   117,
      -1,   117,    17,   116,    -1,   117,    18,   116,    -1,   115,
      -1,    70,   195,    28,   116,    -1,    71,   195,    28,   116,
      -1,   118,    -1,   118,    13,   118,    -1,   118,    12,   118,
      -1,   118,    43,   118,    -1,   118,    42,   118,    -1,   118,
      68,   118,    -1,   119,    -1,   121,    14,   118,    -1,   120,
      -1,   119,    15,   121,    -1,   121,    -1,   120,    16,   121,
      -1,   122,    -1,   121,    25,   122,    -1,   121,    26,   122,
      -1,   123,    -1,   122,    66,   123,    -1,   122,    67,   123,
      -1,   124,    -1,   123,    24,   124,    -1,   123,    28,   124,
      -1,   126,    -1,    33,   125,    -1,    26,   124,    -1,    35,
     124,    -1,   124,    -1,    70,   195,    28,   125,    -1,    71,
     195,    28,   125,    -1,   128,    -1,   126,    37,   127,    38,
      -1,   106,    -1,   127,    29,   106,    -1,    88,    -1,   129,
      -1,   130,    -1,   133,    -1,    37,   106,    38,    -1,    64,
      -1,    65,    -1,    89,    -1,    39,    -1,    44,    -1,    40,
     127,    41,    -1,    45,   127,    46,    -1,    45,   131,    46,
      -1,   132,    -1,   131,    29,   132,    -1,   106,    30,   106,
      -1,    45,   134,    36,   106,    46,    -1,    88,    30,    92,
      -1,   136,    -1,    75,    -1,   137,    -1,   136,    36,   137,
      -1,    88,    -1,    88,    37,   127,    38,    -1,   139,    -1,
     140,    -1,   140,    25,   139,    -1,   141,    -1,    76,   195,
      28,   140,    -1,   143,    -1,   143,    20,   142,    -1,   143,
      10,   142,    -1,   141,    -1,    76,   195,    28,   142,    -1,
     145,    -1,   143,    22,   144,    -1,   145,    -1,    76,   195,
      28,   144,    -1,   147,    -1,   124,    11,   146,    -1,   124,
      11,   150,    23,   146,    -1,   145,    -1,    76,   195,    28,
     146,    -1,   151,    -1,   151,    28,   149,    -1,   152,    -1,
     151,    28,   150,    -1,   147,    -1,    76,   195,    28,   149,
      -1,   124,    11,   149,    -1,   124,    11,   150,    23,   149,
      -1,   148,    -1,    76,   195,    28,   150,    -1,   153,    -1,
     151,    34,   124,    -1,   154,    -1,   152,    34,   124,    -1,
     157,    -1,   157,    36,   155,    -1,   157,    -1,   157,    36,
     156,    -1,   153,    -1,    76,   195,    28,   155,    -1,   124,
      11,   155,    -1,   124,    11,   156,    23,   155,    -1,   154,
      -1,    76,   195,    28,   156,    -1,   158,    -1,   137,    -1,
     159,    -1,    37,   138,    38,    -1,    74,    -1,    75,    -1,
      77,    37,   160,    29,   138,    38,    -1,    79,    37,   160,
      29,   138,    38,    -1,    80,    37,   162,    29,   138,    38,
      -1,    81,    37,   165,    29,   138,    38,    -1,    78,    37,
     170,    29,   138,    38,    -1,    44,    -1,    45,   161,    46,
      -1,    88,    -1,   161,    29,    88,    -1,    44,    -1,    45,
     163,    46,    -1,   164,    -1,   163,    29,   164,    -1,    88,
      11,    88,    -1,    44,    -1,    45,   166,    46,    -1,   167,
      -1,   166,    29,   167,    -1,   168,    -1,   168,    11,    75,
      -1,   168,    11,    88,    -1,   169,    -1,    88,    -1,   169,
      36,    88,    -1,    44,    -1,    45,   171,    46,    -1,   168,
      -1,   171,    29,   168,    -1,   173,    -1,   174,    -1,   173,
     174,    -1,   175,    -1,   179,    -1,   180,    -1,   183,    -1,
     189,    -1,   192,    -1,   196,    -1,    47,   176,    -1,   177,
      31,    -1,   176,   177,    31,    -1,   161,    -1,    88,    27,
      92,    -1,    94,    -1,    94,    11,    92,    -1,    48,   181,
      -1,    49,   181,    -1,   182,    31,    -1,   181,   182,    31,
      -1,   161,    30,    92,    -1,   184,    -1,    51,   185,    -1,
      50,   187,    51,   185,    -1,   186,    31,    -1,   185,   186,
      31,    -1,   106,    27,   106,    -1,   106,    11,   106,    27,
     106,    -1,   188,    31,    -1,   187,   188,    31,    -1,   161,
      30,    92,    -1,    52,   190,    -1,   191,    31,    -1,   190,
     191,    31,    -1,   161,    -1,   161,    30,   178,    -1,    53,
     193,    -1,   194,    31,    -1,   193,   194,    31,    -1,    88,
      27,   138,    -1,    88,    37,   195,    38,    27,   138,    -1,
     188,    -1,   195,    29,   188,    -1,    54,   138,    31,    -1,
     198,    -1,   202,    -1,    70,   195,    28,   198,    -1,    71,
     195,    28,   198,    -1,    84,    88,   199,    28,   198,    -1,
      83,    88,   199,    28,   198,    -1,    -1,    37,   200,    38,
      -1,   201,    -1,   200,    29,   201,    -1,    88,    30,    92,
      27,   106,    -1,   204,    -1,   204,    21,   203,    -1,   202,
      -1,    70,   195,    28,   203,    -1,    71,   195,    28,   203,
      -1,    84,    88,   199,    28,   203,    -1,    83,    88,   199,
      28,   203,    -1,   206,    -1,   206,    19,   205,    -1,   206,
      20,   205,    -1,   204,    -1,    70,   195,    28,   205,    -1,
      71,   195,    28,   205,    -1,    84,    88,   199,    28,   205,
      -1,    83,    88,   199,    28,   205,    -1,   208,    -1,    33,
     207,    -1,    40,   209,    41,   207,    -1,    42,   209,    43,
     207,    -1,    86,    34,   124,    -1,    85,    34,   124,    -1,
     206,    -1,    70,   195,    28,   207,    -1,    71,   195,    28,
     207,    -1,    84,    88,   199,    28,   207,    -1,    83,    88,
     199,    28,   207,    -1,    82,    37,   106,    38,    -1,   137,
      -1,    64,    -1,    65,    -1,    86,    -1,    85,    -1,    37,
     197,    38,    -1,   218,    -1,   210,    -1,   212,    -1,   213,
      25,   211,    -1,   213,    -1,   213,    25,   211,    -1,   214,
      -1,   215,    28,   213,    -1,   215,    -1,   215,    28,   213,
      -1,   216,    -1,   215,    24,    -1,   215,    25,    -1,   217,
      -1,   215,    24,    -1,   215,    25,    -1,    87,    -1,    37,
     210,    38,    -1,   218,    -1,    87,    -1,    37,   210,    38,
      -1,   219,    -1,   220,    -1,    70,   195,    28,   219,    -1,
      71,   195,    28,   219,    -1,   222,    -1,   222,    21,   221,
      -1,   220,    -1,    70,   195,    28,   221,    -1,    71,   195,
      28,   221,    -1,   224,    -1,   225,    19,   223,    -1,   225,
      20,   223,    -1,   222,    -1,    70,   195,    28,   223,    -1,
      71,   195,    28,   223,    -1,   225,    -1,   224,    34,   124,
      -1,   227,    -1,    33,   226,    -1,   225,    -1,    70,   195,
      28,   226,    -1,    71,   195,    28,   226,    -1,   135,    -1,
      82,    37,   106,    38,    -1,    64,    -1,    65,    -1,    37,
     218,    38,    -1,   229,    -1,   230,    -1,   229,   230,    -1,
     175,    -1,   179,    -1,   180,    -1,   183,    -1,   189,    -1,
     231,    -1,   232,    -1,    80,   233,    -1,    50,   187,    80,
     233,    -1,   234,    31,    -1,   233,   234,    31,    -1,   106,
      11,   137,    21,   138,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,   161,   161,   166,   171,   176,   181,   186,   191,   203,
     212,   216,   225,   234,   239,   248,   257,   261,   270,   275,
     284,   289,   299,   303,   312,   317,   326,   331,   340,   345,
     350,   355,   363,   367,   371,   375,   379,   387,   391,   395,
     406,   415,   419,   428,   433,   442,   451,   455,   460,   465,
     474,   478,   488,   492,   497,   506,   510,   516,   526,   530,
     535,   544,   548,   554,   564,   568,   573,   582,   586,   592,
     598,   604,   610,   620,   624,   634,   638,   648,   652,   662,
     666,   672,   682,   686,   692,   702,   706,   712,   722,   726,
     731,   736,   745,   749,   754,   763,   767,   776,   781,   790,
     795,   799,   803,   807,   815,   820,   825,   830,   835,   844,
     849,   854,   863,   868,   877,   886,   895,   907,   912,   921,
     926,   935,   940,   952,   961,   965,   974,   978,   987,   991,
     996,  1005,  1009,  1018,  1022,  1031,  1035,  1044,  1048,  1053,
    1062,  1066,  1075,  1079,  1088,  1092,  1101,  1105,  1110,  1115,
    1124,  1128,  1137,  1141,  1150,  1154,  1163,  1167,  1176,  1180,
    1189,  1193,  1198,  1203,  1212,  1216,  1225,  1229,  1233,  1237,
    1245,  1250,  1259,  1264,  1269,  1274,  1279,  1288,  1293,  1302,
    1307,  1316,  1321,  1330,  1335,  1344,  1353,  1358,  1367,  1372,
    1381,  1386,  1391,  1400,  1409,  1414,  1423,  1428,  1437,  1442,
    1454,  1463,  1468,  1477,  1482,  1487,  1492,  1497,  1502,  1507,
    1516,  1525,  1530,  1539,  1548,  1557,  1562,  1571,  1580,  1590,
    1595,  1604,  1617,  1626,  1631,  1646,  1651,  1660,  1665,  1675,
    1680,  1689,  1702,  1711,  1716,  1725,  1735,  1748,  1757,  1762,
    1771,  1777,  1793,  1798,  1807,  1819,  1828,  1832,  1837,  1842,
    1847,  1857,  1861,  1871,  1876,  1885,  1894,  1898,  1907,  1911,
    1916,  1921,  1926,  1935,  1939,  1944,  1953,  1957,  1962,  1967,
    1972,  1981,  1985,  1990,  1995,  2000,  2005,  2014,  2018,  2023,
    2028,  2033,  2042,  2047,  2052,  2057,  2062,  2067,  2072,  2080,
    2085,  2094,  2098,  2107,  2111,  2120,  2124,  2133,  2137,  2146,
    2150,  2155,  2164,  2168,  2173,  2182,  2187,  2195,  2200,  2205,
    2213,  2222,  2226,  2231,  2240,  2244,  2253,  2257,  2262,  2271,
    2275,  2280,  2289,  2293,  2298,  2307,  2311,  2320,  2324,  2333,
    2337,  2342,  2351,  2356,  2361,  2366,  2371,  2380,  2389,  2394,
    2403,  2408,  2413,  2418,  2423,  2428,  2437,  2446,  2451,  2466,
    2471,  2480
};
#endif

#if (YYDEBUG) || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "TAG_SORT_EXPR", "TAG_DATA_EXPR",
  "TAG_MULT_ACT", "TAG_PROC_EXPR", "TAG_SPEC", "TAG_STATE_FRM",
  "TAG_ACTION_RENAME", "LMERGE", "ARROW", "LTE", "GTE", "CONS", "SNOC",
  "CONCAT", "EQ", "NEQ", "AND", "BARS", "IMP", "BINIT", "ELSE", "STAR",
  "PLUS", "MINUS", "EQUALS", "DOT", "COMMA", "COLON", "SEMICOLON", "QMARK",
  "EXCLAM", "AT", "HASH", "BAR", "LPAR", "RPAR", "PBRACK", "LBRACK",
  "RBRACK", "LANG", "RANG", "PBRACE", "LBRACE", "RBRACE", "KWSORT",
  "KWCONS", "KWMAP", "KWVAR", "KWEQN", "KWACT", "KWPROC", "KWINIT",
  "KWSTRUCT", "BOOL", "POS", "NAT", "INT", "REAL", "LIST", "SET", "BAG",
  "CTRUE", "CFALSE", "DIV", "MOD", "IN", "LAMBDA", "FORALL", "EXISTS",
  "WHR", "END", "DELTA", "TAU", "SUM", "BLOCK", "ALLOW", "HIDE", "RENAME",
  "COMM", "VAL", "MU", "NU", "DELAY", "YALED", "NIL", "ID", "NUMBER",
  "$accept", "start", "sort_expr", "sort_expr_arrow", "domain_no_arrow",
  "domain_no_arrow_elts_hs", "domain_no_arrow_elt", "sort_expr_struct",
  "struct_constructors_bs", "struct_constructor", "recogniser",
  "struct_projections_cs", "struct_projection", "sort_expr_primary",
  "sort_constant", "sort_constructor", "data_expr", "data_expr_whr",
  "whr_decls_cs", "whr_decl", "data_expr_quant", "data_expr_imp",
  "data_expr_imp_rhs", "data_expr_and", "data_expr_and_rhs",
  "data_expr_eq", "data_expr_eq_rhs", "data_expr_rel", "data_expr_cons",
  "data_expr_snoc", "data_expr_concat", "data_expr_add", "data_expr_div",
  "data_expr_mult", "data_expr_prefix", "data_expr_quant_prefix",
  "data_expr_postfix", "data_exprs_cs", "data_expr_primary",
  "data_constant", "data_enumeration", "bag_enum_elts_cs", "bag_enum_elt",
  "data_comprehension", "data_var_decl", "mult_act", "param_ids_bs",
  "param_id", "proc_expr", "proc_expr_choice", "proc_expr_sum",
  "proc_expr_merge", "proc_expr_merge_rhs", "proc_expr_binit",
  "proc_expr_binit_rhs", "proc_expr_cond", "proc_expr_cond_la",
  "proc_expr_seq", "proc_expr_seq_wo_cond", "proc_expr_seq_rhs",
  "proc_expr_seq_rhs_wo_cond", "proc_expr_at", "proc_expr_at_wo_cond",
  "proc_expr_sync", "proc_expr_sync_wo_cond", "proc_expr_sync_rhs",
  "proc_expr_sync_rhs_wo_cond", "proc_expr_primary", "proc_constant",
  "proc_quant", "act_names_set", "ids_cs", "ren_expr_set", "ren_exprs_cs",
  "ren_expr", "comm_expr_set", "comm_exprs_cs", "comm_expr",
  "mult_act_name", "ids_bs", "mult_act_names_set", "mult_act_names_cs",
  "spec", "spec_elts", "spec_elt", "sort_spec", "sorts_decls_scs",
  "sorts_decl", "domain", "cons_spec", "map_spec", "ops_decls_scs",
  "ops_decl", "eqn_spec", "eqn_sect", "eqn_decls_scs", "eqn_decl",
  "data_vars_decls_scs", "data_vars_decl", "act_spec", "acts_decls_scs",
  "acts_decl", "proc_spec", "proc_decls_scs", "proc_decl",
  "data_vars_decls_cs", "initialisation", "state_frm", "state_frm_quant",
  "fixpoint_params", "data_var_decl_inits_cs", "data_var_decl_init",
  "state_frm_imp", "state_frm_imp_rhs", "state_frm_and",
  "state_frm_and_rhs", "state_frm_prefix", "state_frm_quant_prefix",
  "state_frm_primary", "reg_frm", "reg_frm_alt_naf", "reg_frm_alt",
  "reg_frm_seq_naf", "reg_frm_seq", "reg_frm_postfix_naf",
  "reg_frm_postfix", "reg_frm_primary_naf", "reg_frm_primary", "act_frm",
  "act_frm_quant", "act_frm_imp", "act_frm_imp_rhs", "act_frm_and",
  "act_frm_and_rhs", "act_frm_at", "act_frm_prefix",
  "act_frm_quant_prefix", "act_frm_primary", "action_rename",
  "action_rename_elts", "action_rename_elt", "action_rename_spec",
  "action_rename_sect", "action_rename_rules_scs", "action_rename_rule", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    90,    91,    91,    91,    91,    91,    91,    91,    92,
      93,    93,    94,    95,    95,    96,    97,    97,    98,    98,
      99,    99,   100,   100,   101,   101,   102,   102,   103,   103,
     103,   103,   104,   104,   104,   104,   104,   105,   105,   105,
     106,   107,   107,   108,   108,   109,   110,   110,   110,   110,
     111,   111,   112,   112,   112,   113,   113,   113,   114,   114,
     114,   115,   115,   115,   116,   116,   116,   117,   117,   117,
     117,   117,   117,   118,   118,   119,   119,   120,   120,   121,
     121,   121,   122,   122,   122,   123,   123,   123,   124,   124,
     124,   124,   125,   125,   125,   126,   126,   127,   127,   128,
     128,   128,   128,   128,   129,   129,   129,   129,   129,   130,
     130,   130,   131,   131,   132,   133,   134,   135,   135,   136,
     136,   137,   137,   138,   139,   139,   140,   140,   141,   141,
     141,   142,   142,   143,   143,   144,   144,   145,   145,   145,
     146,   146,   147,   147,   148,   148,   149,   149,   149,   149,
     150,   150,   151,   151,   152,   152,   153,   153,   154,   154,
     155,   155,   155,   155,   156,   156,   157,   157,   157,   157,
     158,   158,   159,   159,   159,   159,   159,   160,   160,   161,
     161,   162,   162,   163,   163,   164,   165,   165,   166,   166,
     167,   167,   167,   168,   169,   169,   170,   170,   171,   171,
     172,   173,   173,   174,   174,   174,   174,   174,   174,   174,
     175,   176,   176,   177,   177,   178,   178,   179,   180,   181,
     181,   182,   183,   184,   184,   185,   185,   186,   186,   187,
     187,   188,   189,   190,   190,   191,   191,   192,   193,   193,
     194,   194,   195,   195,   196,   197,   198,   198,   198,   198,
     198,   199,   199,   200,   200,   201,   202,   202,   203,   203,
     203,   203,   203,   204,   204,   204,   205,   205,   205,   205,
     205,   206,   206,   206,   206,   206,   206,   207,   207,   207,
     207,   207,   208,   208,   208,   208,   208,   208,   208,   209,
     209,   210,   210,   211,   211,   212,   212,   213,   213,   214,
     214,   214,   215,   215,   215,   216,   216,   217,   217,   217,
     218,   219,   219,   219,   220,   220,   221,   221,   221,   222,
     222,   222,   223,   223,   223,   224,   224,   225,   225,   226,
     226,   226,   227,   227,   227,   227,   227,   228,   229,   229,
     230,   230,   230,   230,   230,   230,   231,   232,   232,   233,
     233,   234
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     1,
       1,     3,     1,     1,     3,     1,     1,     2,     1,     3,
       2,     5,     0,     2,     1,     3,     1,     3,     1,     1,
       1,     3,     1,     1,     1,     1,     1,     4,     4,     4,
       1,     1,     4,     1,     3,     3,     1,     4,     4,     4,
       1,     3,     1,     4,     4,     1,     3,     3,     1,     4,
       4,     1,     3,     3,     1,     4,     4,     1,     3,     3,
       3,     3,     3,     1,     3,     1,     3,     1,     3,     1,
       3,     3,     1,     3,     3,     1,     3,     3,     1,     2,
       2,     2,     1,     4,     4,     1,     4,     1,     3,     1,
       1,     1,     1,     3,     1,     1,     1,     1,     1,     3,
       3,     3,     1,     3,     3,     5,     3,     1,     1,     1,
       3,     1,     4,     1,     1,     3,     1,     4,     1,     3,
       3,     1,     4,     1,     3,     1,     4,     1,     3,     5,
       1,     4,     1,     3,     1,     3,     1,     4,     3,     5,
       1,     4,     1,     3,     1,     3,     1,     3,     1,     3,
       1,     4,     3,     5,     1,     4,     1,     1,     1,     3,
       1,     1,     6,     6,     6,     6,     6,     1,     3,     1,
       3,     1,     3,     1,     3,     3,     1,     3,     1,     3,
       1,     3,     3,     1,     1,     3,     1,     3,     1,     3,
       1,     1,     2,     1,     1,     1,     1,     1,     1,     1,
       2,     2,     3,     1,     3,     1,     3,     2,     2,     2,
       3,     3,     1,     2,     4,     2,     3,     3,     5,     2,
       3,     3,     2,     2,     3,     1,     3,     2,     2,     3,
       3,     6,     1,     3,     3,     1,     1,     4,     4,     5,
       5,     0,     3,     1,     3,     5,     1,     3,     1,     4,
       4,     5,     5,     1,     3,     3,     1,     4,     4,     5,
       5,     1,     2,     4,     4,     3,     3,     1,     4,     4,
       5,     5,     4,     1,     1,     1,     1,     1,     3,     1,
       1,     1,     3,     1,     3,     1,     3,     1,     3,     1,
       2,     2,     1,     2,     2,     1,     3,     1,     1,     3,
       1,     1,     4,     4,     1,     3,     1,     4,     4,     1,
       3,     3,     1,     4,     4,     1,     3,     1,     2,     1,
       4,     4,     1,     4,     1,     1,     3,     1,     1,     2,
       1,     1,     1,     1,     1,     1,     1,     2,     4,     2,
       3,     5
};

/* YYDPREC[RULE-NUM] -- Dynamic precedence of rule #RULE-NUM (0 if none). */
static const unsigned char yydprec[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0
};

/* YYMERGER[RULE-NUM] -- Index of merging function for rule #RULE-NUM. */
static const unsigned char yymerger[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error.  */
static const unsigned short int yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    32,    33,    34,    35,    36,     0,     0,     0,    28,
       2,     9,     0,    12,    13,    10,    16,    29,    30,     0,
       0,     0,     0,   107,     0,   108,     0,   104,   105,     0,
       0,     0,    99,   106,     3,    40,    41,    46,    50,    55,
      61,    67,    73,    75,    77,    79,    82,    85,    88,    95,
     100,   101,   102,   118,   121,     4,   117,   119,     0,   170,
     171,     0,     0,     0,     0,     0,     0,    99,     0,   167,
       5,   123,   124,   126,   128,   133,   137,   142,   152,   156,
     166,   168,     0,     0,     0,     0,     0,     0,     0,     0,
       6,   200,   201,   203,   204,   205,   206,   222,   207,   208,
     209,     0,     0,     0,     0,   284,   285,     0,     0,     0,
       0,     0,   287,   286,   283,     7,   245,   246,   256,   263,
     271,     0,     0,   340,   341,   342,   343,   344,     8,   337,
     338,   345,   346,     1,     0,    22,    17,    18,     0,     0,
       0,     0,     0,    90,     0,     0,    92,    89,    91,     0,
      97,     0,    99,    97,     0,     0,   112,     0,   179,     0,
     242,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    85,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   179,   213,   210,     0,     0,   217,     0,
     218,     0,     0,     0,   223,     0,   235,   232,     0,     0,
     237,     0,     0,   202,     0,     0,     0,     0,   277,   272,
       0,     0,     0,   334,   335,     0,     0,     0,   305,   332,
       0,   290,   291,     0,   295,     0,   299,   302,   307,   310,
     311,   314,   319,   325,   327,     0,     0,     0,     0,   251,
     251,     0,     0,     0,     0,     0,     0,     0,   347,     0,
     339,    31,     0,     0,    20,     0,     0,     0,     0,    11,
      14,    15,     0,     0,   103,     0,   109,     0,     0,   110,
       0,   111,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    43,     0,     0,    52,    51,     0,     0,    58,    56,
      57,     0,     0,    64,    62,    63,    69,    68,    71,    70,
      72,    76,    78,    74,    80,    81,    83,    84,    86,    87,
       0,     0,   120,   169,     0,   177,     0,     0,   196,     0,
       0,     0,   181,     0,     0,   186,     0,     0,     0,   140,
     138,   150,     0,   142,   144,   154,   156,   125,     0,   131,
     130,   129,     0,   134,   135,     0,     0,   146,   143,   153,
       0,     0,   160,   157,     0,     0,   211,     0,     0,   219,
       0,     0,   229,     0,     0,     0,   225,     0,     0,   233,
       0,     0,     0,   238,   244,     0,     0,   251,   251,   288,
       0,     0,     0,   329,   328,     0,   307,     0,     0,     0,
       0,     0,   300,   301,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   276,   275,     0,     0,
       0,     0,   258,   257,     0,     0,     0,     0,   266,   264,
     265,     0,     0,     0,   349,    23,    28,    26,     0,    24,
      19,    37,    38,    39,     0,     0,    98,   116,   114,     0,
     113,     0,   180,   231,    47,   243,    48,    49,     0,     0,
      42,     0,     0,     0,     0,     0,     0,    96,   122,   127,
       0,     0,   194,   198,   193,     0,     0,     0,     0,     0,
     183,     0,     0,   188,   190,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   214,   212,   221,
     220,   224,   230,     0,   227,   226,   215,   236,   234,   240,
       0,   239,     0,     0,     0,     0,     0,     0,     0,   306,
     336,     0,     0,     0,   273,     0,   308,   292,   293,   297,
     307,   296,     0,     0,   316,   315,   326,     0,     0,   322,
     320,   321,   274,   247,   248,   282,     0,     0,   253,     0,
       0,     0,     0,   251,   251,     0,     0,   251,   251,   348,
       0,   350,     0,     0,    22,    93,    94,   115,    45,    44,
       0,     0,     0,     0,     0,     0,   178,     0,     0,     0,
     197,     0,     0,     0,     0,   182,     0,     0,   187,     0,
       0,     0,     0,   139,     0,   145,   155,     0,   164,   159,
       0,     0,     0,   148,     0,     0,   162,     0,     0,     0,
       0,   278,   279,     0,     0,     0,     0,   312,   313,   333,
       0,     0,   303,   304,     0,     0,     0,     0,     0,     0,
       0,   252,   250,   249,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    27,    25,    21,    53,    54,    59,    60,
      65,    66,   172,   195,   199,   176,   173,   185,   184,   174,
     189,   191,   192,   175,   141,   151,     0,     0,     0,   132,
     136,   147,     0,   161,     0,   228,   216,     0,   281,   280,
     330,   331,   309,   294,   298,     0,     0,     0,     0,     0,
     254,   259,   260,     0,     0,   267,   268,     0,     0,   351,
       0,     0,     0,   149,   163,   241,   317,   318,   323,   324,
       0,   262,   261,   270,   269,   165,   255
};

/* YYPDEFGOTO[NTERM-NUM]. */
static const short int yydefgoto[] =
{
      -1,     8,   457,    21,    22,    23,    24,    25,   146,   147,
     284,   458,   459,    26,    27,    28,   223,    45,   310,   311,
      46,    47,   315,    48,   319,    49,   324,    50,    51,    52,
      53,    54,    55,    56,    57,   157,    58,   161,    59,    60,
      61,   165,   166,    62,   167,   249,    66,    79,    80,    81,
      82,    83,   370,    84,   373,    85,   684,    86,   361,   378,
     685,    87,   364,    88,   365,   383,   619,    89,    90,    91,
     347,   169,   354,   499,   500,   357,   502,   503,   504,   494,
     350,   495,   100,   101,   102,   103,   215,   216,   527,   104,
     105,   218,   219,   106,   107,   224,   225,   221,   170,   108,
     227,   228,   109,   230,   231,   171,   110,   125,   126,   434,
     567,   568,   127,   443,   128,   449,   129,   239,   130,   250,
     251,   547,   252,   253,   254,   255,   256,   257,   550,   259,
     260,   555,   261,   560,   262,   263,   414,   264,   138,   139,
     140,   141,   142,   278,   279
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -581
static const short int yypact[] =
{
     377,   316,    76,    17,   824,   193,   587,   101,    72,   316,
      35,  -581,  -581,  -581,  -581,  -581,   133,   157,   161,  -581,
    -581,  -581,   127,   178,  -581,    68,  -581,  -581,  -581,   325,
     597,   325,    76,  -581,    76,  -581,   509,  -581,  -581,   175,
     175,   175,  -581,  -581,  -581,   199,  -581,  -581,   239,    65,
      90,    58,   268,   281,   118,   184,   107,  -581,   262,  -581,
    -581,  -581,  -581,  -581,   273,  -581,   278,  -581,   754,  -581,
    -581,   175,   296,   298,   302,   315,   320,  1439,   343,  -581,
    -581,  -581,   306,  -581,   164,  -581,  -581,   163,  -581,   327,
    -581,  -581,   280,   175,   175,   175,    76,   175,   308,   824,
    -581,   193,  -581,  -581,  -581,  -581,  -581,  -581,  -581,  -581,
    -581,  1236,   587,  1262,  1262,  -581,  -581,   175,   175,   324,
     311,   322,   374,   378,  -581,  -581,  -581,  -581,   384,   270,
    -581,   175,    76,  -581,  -581,  -581,  -581,  -581,  -581,   101,
    -581,  -581,  -581,  -581,   380,   180,   388,  -581,   316,   316,
     316,   316,   316,  -581,   175,   175,  -581,  -581,  -581,   382,
    -581,   158,   399,   404,    52,    66,  -581,   400,  -581,   272,
    -581,   294,   317,   338,   360,   670,  1201,  1201,  1216,  1216,
     325,   325,   325,   325,   325,   325,   325,   325,   325,   325,
     325,   325,   325,   325,    76,    76,   364,   343,   418,   365,
     371,   396,   371,   398,   435,   846,   824,   905,   905,   964,
     986,   325,  1043,   442,   463,   280,   465,   453,   175,   470,
     175,    29,   472,    67,    76,   479,   455,   175,   481,   188,
     308,   505,   510,  -581,   175,   175,   438,   457,  -581,  -581,
     514,    54,  1262,  -581,  -581,   175,   175,   506,   228,  -581,
     570,  -581,  -581,   547,  -581,   234,  -581,  -581,   130,  -581,
    -581,   526,   549,   467,  -581,   520,   462,   471,    76,   579,
     579,   325,   325,  1275,  1301,  1301,    26,   596,    76,   590,
    -581,  -581,   537,  1395,  -581,    35,   595,   605,   612,  -581,
    -581,  -581,   480,   486,  -581,    76,  -581,   316,    76,  -581,
      76,  -581,    76,   592,   316,    76,   175,    76,    76,   601,
      31,  -581,   175,   175,  -581,  -581,   175,   175,  -581,  -581,
    -581,   175,   175,  -581,  -581,  -581,  -581,  -581,  -581,  -581,
    -581,   491,   491,  -581,   184,   184,   107,   107,  -581,  -581,
      99,   138,  -581,  -581,   824,  -581,   175,   606,  -581,   600,
     645,   654,  -581,   602,   666,  -581,   600,   675,   175,  -581,
    -581,  -581,   683,   221,   674,  -581,    63,  -581,   175,  -581,
    -581,  -581,   175,  -581,  -581,   175,   702,  -581,  -581,  -581,
     175,   705,  -581,  -581,   316,   698,  -581,   316,   699,  -581,
      76,   706,  -581,    76,    76,   707,  -581,   316,   708,  -581,
     824,   175,   711,  -581,  -581,   493,   495,   579,   579,  -581,
     732,   175,   175,  -581,  -581,   695,   709,   501,   503,    76,
    1236,  1331,   249,   254,  1331,  1340,   325,  1360,  1360,  1236,
     587,   587,   714,   648,   715,   716,  -581,  -581,   175,   175,
     657,   658,  -581,  -581,   175,   175,   665,   667,  -581,  -581,
    -581,    76,   364,   723,  -581,  -581,   733,  -581,   172,  -581,
    -581,  -581,  -581,  -581,   597,   597,  -581,  -581,  -581,   404,
    -581,   718,  -581,  -581,  -581,  -581,  -581,  -581,    76,   360,
    -581,   511,   522,   527,   529,   531,   533,  -581,  -581,  -581,
      81,   824,  -581,  -581,   726,    93,   824,   824,   755,   129,
    -581,   824,   137,  -581,   756,   824,   538,  1065,  1122,   325,
    1144,   540,   542,   553,  1122,   558,  1144,  -581,  -581,  -581,
    -581,    76,  -581,   743,  -581,  -581,   760,  -581,  -581,  -581,
     195,  -581,  1236,  1236,   744,   745,   709,   561,   563,   263,
    -581,   732,   732,   736,  -581,  1262,  -581,  -581,   750,   331,
    -581,   751,   175,   175,  -581,  -581,  -581,   175,   175,  -581,
    -581,  -581,  -581,  -581,  -581,  -581,   747,   197,  -581,   587,
     587,   566,   571,   579,   579,   573,   575,   579,   579,    76,
     758,  -581,   316,  1395,   749,  -581,  -581,  -581,  -581,  -581,
     670,   670,  1201,  1201,  1216,  1216,  -581,   746,   700,   600,
    -581,   748,   757,   704,   602,  -581,   762,   600,  -581,    94,
     763,   846,   175,  -581,   175,  -581,  -581,   175,  -581,  -581,
     905,   964,   986,  -581,   781,  1043,  -581,   782,    76,   316,
     779,  -581,  -581,  1236,  1236,    54,    54,  -581,  -581,  -581,
     770,  1331,  -581,  -581,  1331,   577,   581,   586,   610,   316,
     648,  -581,  -581,  -581,  1275,  1275,   784,   785,  1301,  1301,
     787,   788,   824,  -581,  -581,  -581,  -581,  -581,  -581,  -581,
    -581,  -581,  -581,  -581,  -581,  -581,  -581,  -581,  -581,  -581,
    -581,  -581,  -581,  -581,  -581,  -581,   618,   620,   625,  -581,
    -581,  -581,   986,  -581,  1043,  -581,  -581,   824,  -581,  -581,
    -581,  -581,  -581,  -581,  -581,  1340,  1340,  1360,  1360,   783,
    -581,  -581,  -581,  1275,  1275,  -581,  -581,  1301,  1301,  -581,
    1065,  1122,  1144,  -581,  -581,  -581,  -581,  -581,  -581,  -581,
      76,  -581,  -581,  -581,  -581,  -581,  -581
};

/* YYPGOTO[NTERM-NUM].  */
static const short int yypgoto[] =
{
    -581,  -581,    11,   632,   412,  -581,   684,  -133,  -581,   532,
     242,  -581,   244,  -581,  -581,  -581,    -2,  -581,  -581,   358,
     -13,  -153,    73,  -138,  -148,  -137,  -117,  -581,    47,  -581,
    -581,   496,   477,   502,    -3,   235,  -581,    25,  -581,  -581,
    -581,  -581,   539,  -581,  -581,   835,  -581,    43,   -66,   634,
     497,  -194,  -198,  -581,   223,  -200,  -182,  -203,  -581,  -460,
    -187,  -202,  -581,  -197,  -465,  -457,  -468,  -184,  -581,  -581,
     643,    -4,  -581,  -581,   243,  -581,  -581,   245,  -313,  -581,
    -581,  -581,  -581,  -581,   752,    46,  -581,   631,  -581,    49,
      56,   764,    18,    57,  -581,   461,  -193,   725,   -58,    61,
    -581,   633,  -581,  -581,   635,   410,  -581,   759,  -348,  -230,
    -581,   212,  -248,  -580,  -258,  -271,  -100,  -353,  -581,   753,
    -236,   225,  -581,  -374,  -581,  -369,  -581,  -581,   -70,   170,
    -356,    15,  -370,  -404,  -581,  -233,    89,  -581,  -581,  -581,
     731,  -581,  -581,   422,  -243
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -310
static const short int yytable[] =
{
      44,    78,   198,   363,   450,   359,   415,   377,   413,   374,
     371,   238,    20,   369,   369,   382,   448,   448,   362,   291,
     144,   366,   314,   360,   561,   442,   153,   156,   158,   320,
     159,   395,   160,   232,   163,   453,   493,   222,   318,   318,
     435,   323,   323,   258,   258,   618,    67,   548,   627,   124,
     551,   618,   549,   133,   623,   549,   134,   559,   559,   626,
     479,   164,   325,   135,   136,   197,   159,   544,   137,   554,
     180,   181,   143,   222,   711,   712,   562,   390,   393,   -15,
     390,   295,   563,   564,   176,   177,  -158,   241,   214,   217,
     217,   410,    63,   226,   394,   300,    78,  -156,   299,   510,
     182,   183,    29,   -15,   480,    64,   451,   178,   179,    30,
     303,    31,   301,    32,   168,    33,    34,   168,   243,   244,
      35,    36,   599,   145,   411,   412,   184,   596,   295,    63,
     277,   192,   187,   731,   732,   193,   247,   487,   151,   600,
      37,    38,    64,   188,   189,    39,    40,    41,    92,    93,
      94,   131,    96,    97,   124,   124,    67,    67,   604,   286,
     287,   288,   691,   391,    42,    43,   607,   295,   693,   681,
     148,  -289,   416,  -289,   207,   605,   488,   534,   535,   631,
     632,   132,   682,   608,   208,   133,   209,   295,   134,   338,
     339,   210,   160,   160,   149,   135,   136,   211,   150,   296,
     137,   583,    78,    78,    78,    78,    78,   376,   379,   381,
     584,   214,   282,   152,   217,   400,   217,   283,   391,   340,
     341,   652,   653,   226,   306,   401,   650,   326,   327,   328,
     329,   330,   723,   630,   333,   651,   388,   724,   388,   342,
      92,    93,    94,    95,    96,    97,    98,    99,   475,   508,
     190,   191,  -308,  -308,   735,   211,  -308,   618,   422,   423,
     175,   691,   424,   168,   291,   693,   432,   548,   436,   437,
     704,   174,   549,  -303,  -303,   549,   277,  -303,  -304,  -304,
     698,   699,  -304,   185,    67,    67,   674,  -309,  -309,   274,
     275,  -309,   474,   466,   476,   477,   468,   186,   469,   194,
     471,   303,   304,   728,   729,   377,   363,   359,   467,   640,
     195,   377,   363,   382,   196,   473,   124,   124,   124,   382,
     238,   615,   305,   306,   366,   613,   366,   624,   395,   238,
     366,   206,   366,   200,   529,   201,   453,   559,   559,   202,
     536,    78,   490,   656,   657,   307,   306,   660,   661,   554,
     554,    29,   203,     9,   205,   642,   643,   204,    30,   644,
      31,   268,    32,   212,    33,    34,   308,   306,   213,    35,
      36,    10,    11,    12,    13,    14,    15,    16,    17,    18,
       1,     2,     3,     4,     5,     6,     7,   715,   716,    37,
      38,   523,   524,   344,   306,   517,   229,    78,   519,   269,
     448,   448,   413,   413,    19,   273,   442,   442,   271,   363,
     270,   359,   272,    42,    43,   345,   346,   543,   281,   377,
     294,   374,   689,   556,   285,   597,   369,   366,   382,   297,
     601,   602,   238,   238,   298,   606,   302,   314,   314,   610,
     348,   349,   352,   353,   668,   669,   733,   734,   309,   277,
     172,   173,    64,    67,   318,   318,   343,   323,   323,   448,
     448,   156,   156,   124,    67,   442,   442,    67,    67,   384,
      67,    67,   124,   124,   124,   416,   588,   670,   671,   355,
     356,   199,   303,   387,   303,   397,   427,   428,    78,   377,
     430,   306,   303,    78,    78,   580,   386,   382,    78,   431,
     306,   389,    78,   392,    78,   376,   616,   381,   464,   306,
     396,   376,   399,   381,   465,   306,   188,   189,   377,   363,
     359,   532,   306,   533,   306,   382,   407,   266,   267,   541,
     306,   542,   306,   238,   238,    29,   403,   366,   366,   590,
     306,   404,    30,   419,    31,   408,    32,   425,    33,    34,
     591,   306,   409,    35,    36,   592,   306,   593,   306,   594,
     306,   595,   306,   429,   292,   293,   611,   306,   620,   306,
     621,   306,   421,    37,    38,   124,   124,   277,    39,    40,
      41,   622,   306,   426,    67,    67,   625,   306,    67,   635,
     306,   636,   306,   663,   654,   306,   719,   162,    43,   655,
     306,   658,   306,   659,   306,   705,   306,   452,    78,   706,
     306,   420,   124,   124,   707,   306,   433,    78,    78,   376,
     111,   454,   381,    29,   112,   455,   695,   113,   478,   114,
      30,   725,    31,   461,    32,   491,    33,    34,   708,   306,
     696,    35,    36,   462,   405,   406,   720,   306,   721,   306,
     463,   115,   116,   722,   306,   417,   418,   117,   118,    78,
     709,    37,    38,   666,   667,   334,   335,   154,   155,   119,
     120,   121,   122,   123,   496,    64,   124,   124,    67,    67,
     472,   331,   332,   497,    67,    42,    43,    67,   492,   376,
     498,   381,   336,   337,    78,   501,    29,   124,   124,   585,
     586,   124,   124,    30,   505,    31,   507,    32,   509,    33,
      34,   637,   638,   514,    35,    36,   516,    78,   376,   381,
     726,   727,   481,   482,   700,   701,   483,   484,   736,   518,
     520,   485,   486,   539,    37,    38,   566,   522,   525,   528,
     312,   313,   531,   569,   570,   573,   574,   540,    67,    67,
      67,    67,   565,   577,   581,   578,   124,   124,    42,    43,
     124,   124,   598,   582,   587,   241,   603,   609,   506,   410,
     628,   629,   633,   634,   639,   641,  -298,   649,   511,   662,
      29,   282,   512,   289,   672,   513,   675,    30,   673,    31,
     515,    68,   677,    33,    34,   676,   243,   244,    35,    36,
     679,   683,   245,   246,   692,   694,   697,    63,   702,   526,
     730,   530,   713,   714,   247,   717,   718,   460,    37,    38,
      64,   537,   538,    39,    40,    41,   665,   664,    69,    70,
      71,    72,    73,    74,    75,    76,   290,   589,    65,   470,
     367,   489,    77,    43,   690,   351,   385,   678,   571,   572,
      29,   521,   680,   233,   575,   576,   276,    30,   220,    31,
     398,    68,   710,    33,    34,   402,   703,   265,    35,    36,
     280,   240,    29,   579,     0,     0,     0,     0,     0,    30,
       0,    31,     0,    68,     0,    33,    34,     0,    37,    38,
      35,    36,     0,     0,     0,     0,     0,     0,    69,    70,
      71,    72,    73,    74,    75,    76,     0,     0,     0,     0,
      37,    38,    77,    43,     0,     0,     0,     0,     0,     0,
      69,    70,   358,    72,    73,    74,    75,    76,     0,     0,
       0,    29,     0,     0,    77,    43,     0,     0,    30,     0,
      31,     0,    68,     0,    33,    34,     0,     0,     0,    35,
      36,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   645,   646,     0,     0,     0,   647,   648,    37,
      38,     0,     0,     0,     0,     0,     0,     0,     0,    69,
      70,   368,    72,    73,    74,    75,    76,     0,     0,     0,
      29,     0,     0,    77,    43,     0,     0,    30,     0,    31,
       0,    68,     0,    33,    34,     0,     0,     0,    35,    36,
       0,     0,    29,     0,     0,     0,     0,     0,     0,    30,
       0,    31,   686,    68,   687,    33,    34,   688,    37,    38,
      35,    36,     0,     0,     0,     0,     0,     0,    69,    70,
     372,    72,    73,    74,    75,    76,     0,     0,     0,     0,
      37,    38,    77,    43,     0,     0,     0,     0,     0,     0,
      69,    70,   375,    72,    73,    74,    75,    76,     0,    29,
       0,     0,     0,     0,    77,    43,    30,     0,    31,     0,
      68,     0,    33,    34,     0,     0,     0,    35,    36,     0,
       0,    29,     0,     0,     0,     0,     0,     0,    30,     0,
      31,     0,    68,     0,    33,    34,     0,    37,    38,    35,
      36,     0,     0,     0,     0,     0,     0,    69,    70,   380,
      72,    73,    74,    75,    76,     0,     0,     0,     0,    37,
      38,    77,    43,     0,     0,     0,     0,     0,     0,    69,
      70,   612,    72,    73,    74,    75,    76,     0,    29,     0,
       0,     0,     0,    77,    43,    30,     0,    31,     0,    68,
       0,    33,    34,     0,     0,     0,    35,    36,     0,     0,
      29,     0,     0,     0,     0,     0,     0,    30,     0,    31,
       0,    68,     0,    33,    34,     0,    37,    38,    35,    36,
       0,     0,     0,     0,     0,     0,    69,    70,   614,    72,
      73,    74,    75,    76,     0,     0,     0,     0,    37,    38,
      77,    43,     0,     0,     0,     0,     0,     0,    69,    70,
     617,    72,    73,    74,    75,    76,     0,    29,     0,     0,
       0,     0,    77,    43,    30,     0,    31,     0,    32,     0,
      33,    34,    29,     0,     0,    35,    36,     0,     0,    30,
       0,    31,     0,    32,     0,    33,    34,     0,     0,     0,
      35,    36,     0,     0,     0,    37,    38,     0,     0,   111,
       0,   316,   317,   112,     0,     0,   113,     0,   114,     0,
      37,    38,     0,     0,     0,     0,   321,   322,     0,    42,
      43,     0,     0,     0,     0,   241,     0,     0,     0,   242,
     115,   116,     0,     0,    42,    43,   234,   235,   111,     0,
       0,     0,   112,     0,     0,   113,     0,   114,   119,   236,
     237,   122,   123,     0,    64,     0,   243,   244,     0,     0,
       0,     0,   245,   246,   111,     0,     0,    63,   112,   115,
     116,   113,     0,   114,   247,   438,   439,     0,     0,   248,
      64,     0,     0,     0,     0,     0,     0,   119,   440,   441,
     122,   123,     0,    64,   241,   115,   116,     0,   545,     0,
       0,   444,   445,   241,     0,     0,     0,   410,     0,     0,
       0,     0,     0,   119,   446,   447,   122,   123,     0,    64,
       0,     0,     0,   241,     0,   243,   244,   410,     0,     0,
       0,   245,   246,     0,   243,   244,    63,     0,     0,     0,
     552,   553,     0,   247,     0,    63,     0,     0,   546,    64,
       0,     0,   247,     0,   243,   244,     0,     0,    64,     0,
     557,   558,     9,     0,     0,    63,     0,     0,     0,  -121,
       0,     0,   247,     0,     0,     0,     0,     0,    64,  -121,
      10,    11,    12,    13,    14,    15,    16,    17,    18,   -99,
       0,  -121,  -121,     0,   -99,     0,     0,   -99,     0,     0,
    -121,     0,     0,  -121,     0,  -121,   195,   -99,     0,     0,
       0,     0,     0,   456
};

/* YYCONFLP[YYPACT[STATE-NUM]] -- Pointer into YYCONFL of start of
   list of conflicting reductions corresponding to action entry for
   state STATE-NUM in yytable.  0 means no conflicts.  The list in
   yyconfl is terminated by a rule number of 0.  */
static const unsigned char yyconflp[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    17,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    15,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    19,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    11,     0,     0,     0,     0,    13,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     1,
       0,     0,     0,     0,     3,     0,     0,     5,     0,     0,
       0,     0,     0,     0,     0,     0,     7,     9,     0,     0,
       0,     0,     0,     0
};

/* YYCONFL[I] -- lists of conflicting rule numbers, each terminated by
   0, pointed into by YYCONFLP.  */
static const short int yyconfl[] =
{
       0,   121,     0,   121,     0,   121,     0,    99,     0,   121,
       0,    17,     0,    99,     0,   297,     0,   158,     0,   297,
       0
};

static const short int yycheck[] =
{
       2,     4,    68,   205,   275,   205,   242,   210,   241,   209,
     208,   111,     1,   207,   208,   212,   274,   275,   205,   152,
       9,   205,   175,   205,   428,   273,    29,    30,    31,   177,
      32,   224,    34,    99,    36,   278,   349,    95,   176,   177,
     270,   178,   179,   113,   114,   510,     3,   421,   516,     6,
     424,   516,   421,     7,   514,   424,     7,   427,   428,   516,
      29,    36,   179,     7,     7,    68,    68,   420,     7,   425,
      12,    13,     0,   131,   654,   655,   429,    51,    11,    11,
      51,    29,   430,   431,    19,    20,    23,    33,    92,    93,
      94,    37,    75,    97,    27,    29,    99,    34,    46,    36,
      42,    43,    26,    35,    73,    88,    80,    17,    18,    33,
      29,    35,    46,    37,    88,    39,    40,    88,    64,    65,
      44,    45,    29,    88,    70,    71,    68,    46,    29,    75,
     132,    24,    14,   713,   714,    28,    82,    38,    11,    46,
      64,    65,    88,    25,    26,    69,    70,    71,    47,    48,
      49,    50,    51,    52,   111,   112,   113,   114,    29,   148,
     149,   150,   622,   221,    88,    89,    29,    29,   625,    75,
      37,    41,   242,    43,    10,    46,    38,   407,   408,   532,
     533,    80,    88,    46,    20,   139,    22,    29,   139,   192,
     193,    28,   194,   195,    37,   139,   139,    34,    37,    41,
     139,    29,   205,   206,   207,   208,   209,   210,   211,   212,
      38,   215,    32,    35,   218,    27,   220,    37,   276,   194,
     195,   569,   570,   227,    29,    37,    29,   180,   181,   182,
     183,   184,   692,    38,   187,    38,   218,   694,   220,   196,
      47,    48,    49,    50,    51,    52,    53,    54,   306,    28,
      66,    67,    24,    25,   722,    34,    28,   722,    24,    25,
      21,   721,    28,    88,   397,   722,   268,   641,   271,   272,
     644,    72,   641,    24,    25,   644,   278,    28,    24,    25,
     633,   634,    28,    15,   241,   242,   599,    24,    25,    19,
      20,    28,   305,   295,   307,   308,   298,    16,   300,    37,
     302,    29,    30,   707,   708,   508,   508,   507,   297,   545,
      37,   514,   514,   510,    36,   304,   273,   274,   275,   516,
     420,   508,    28,    29,   508,   507,   510,   514,   521,   429,
     514,    25,   516,    37,   400,    37,   579,   707,   708,    37,
     410,   344,   346,   573,   574,    28,    29,   577,   578,   705,
     706,    26,    37,    37,    11,    24,    25,    37,    33,    28,
      35,    37,    37,    36,    39,    40,    28,    29,    88,    44,
      45,    55,    56,    57,    58,    59,    60,    61,    62,    63,
       3,     4,     5,     6,     7,     8,     9,   658,   659,    64,
      65,   393,   394,    28,    29,   384,    88,   400,   387,    88,
     658,   659,   635,   636,    88,    21,   654,   655,    34,   611,
      88,   611,    34,    88,    89,    44,    45,   419,    38,   622,
      38,   621,   620,   426,    36,   491,   620,   611,   625,    30,
     496,   497,   532,   533,    30,   501,    36,   590,   591,   505,
      44,    45,    44,    45,   592,   593,   717,   718,    88,   451,
      40,    41,    88,   410,   592,   593,    38,   594,   595,   717,
     718,   464,   465,   420,   421,   713,   714,   424,   425,    27,
     427,   428,   429,   430,   431,   545,   478,   594,   595,    44,
      45,    71,    29,    30,    29,    30,    19,    20,   491,   692,
      28,    29,    29,   496,   497,   452,    31,   694,   501,    28,
      29,    31,   505,    31,   507,   508,   509,   510,    28,    29,
      31,   514,    31,   516,    28,    29,    25,    26,   721,   721,
     720,    28,    29,    28,    29,   722,    88,   117,   118,    28,
      29,    28,    29,   633,   634,    26,    31,   721,   722,    28,
      29,    31,    33,    37,    35,    88,    37,    21,    39,    40,
      28,    29,    38,    44,    45,    28,    29,    28,    29,    28,
      29,    28,    29,    43,   154,   155,    28,    29,    28,    29,
      28,    29,    25,    64,    65,   532,   533,   579,    69,    70,
      71,    28,    29,    34,   541,   542,    28,    29,   545,    28,
      29,    28,    29,   582,    28,    29,   662,    88,    89,    28,
      29,    28,    29,    28,    29,    28,    29,    11,   611,    28,
      29,    41,   569,   570,    28,    29,    37,   620,   621,   622,
      33,    31,   625,    26,    37,    88,   628,    40,    27,    42,
      33,   697,    35,    38,    37,    29,    39,    40,    28,    29,
     629,    44,    45,    38,   234,   235,    28,    29,    28,    29,
      38,    64,    65,    28,    29,   245,   246,    70,    71,   662,
     649,    64,    65,   590,   591,   188,   189,    70,    71,    82,
      83,    84,    85,    86,    29,    88,   633,   634,   635,   636,
      88,   185,   186,    29,   641,    88,    89,   644,    88,   692,
      88,   694,   190,   191,   697,    29,    26,   654,   655,   464,
     465,   658,   659,    33,    29,    35,    23,    37,    34,    39,
      40,   541,   542,    11,    44,    45,    11,   720,   721,   722,
     705,   706,   312,   313,   635,   636,   316,   317,   730,    31,
      31,   321,   322,    38,    64,    65,    88,    31,    31,    31,
      70,    71,    31,    28,    28,    88,    88,    38,   705,   706,
     707,   708,    38,    88,    31,    88,   713,   714,    88,    89,
     717,   718,    36,    30,    46,    33,    11,    11,   358,    37,
      27,    11,    28,    28,    38,    25,    25,    30,   368,    21,
      26,    32,   372,   151,    38,   375,    38,    33,    88,    35,
     380,    37,    88,    39,    40,    38,    64,    65,    44,    45,
      38,    38,    70,    71,    23,    23,    27,    75,    38,   397,
      27,   401,    28,    28,    82,    28,    28,   285,    64,    65,
      88,   411,   412,    69,    70,    71,   584,   583,    74,    75,
      76,    77,    78,    79,    80,    81,   152,   479,     3,   300,
     206,   344,    88,    89,   621,   202,   215,   604,   438,   439,
      26,   390,   607,   101,   444,   445,   131,    33,    94,    35,
     227,    37,   650,    39,    40,   230,   641,   114,    44,    45,
     139,   112,    26,   451,    -1,    -1,    -1,    -1,    -1,    33,
      -1,    35,    -1,    37,    -1,    39,    40,    -1,    64,    65,
      44,    45,    -1,    -1,    -1,    -1,    -1,    -1,    74,    75,
      76,    77,    78,    79,    80,    81,    -1,    -1,    -1,    -1,
      64,    65,    88,    89,    -1,    -1,    -1,    -1,    -1,    -1,
      74,    75,    76,    77,    78,    79,    80,    81,    -1,    -1,
      -1,    26,    -1,    -1,    88,    89,    -1,    -1,    33,    -1,
      35,    -1,    37,    -1,    39,    40,    -1,    -1,    -1,    44,
      45,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   552,   553,    -1,    -1,    -1,   557,   558,    64,
      65,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    74,
      75,    76,    77,    78,    79,    80,    81,    -1,    -1,    -1,
      26,    -1,    -1,    88,    89,    -1,    -1,    33,    -1,    35,
      -1,    37,    -1,    39,    40,    -1,    -1,    -1,    44,    45,
      -1,    -1,    26,    -1,    -1,    -1,    -1,    -1,    -1,    33,
      -1,    35,   612,    37,   614,    39,    40,   617,    64,    65,
      44,    45,    -1,    -1,    -1,    -1,    -1,    -1,    74,    75,
      76,    77,    78,    79,    80,    81,    -1,    -1,    -1,    -1,
      64,    65,    88,    89,    -1,    -1,    -1,    -1,    -1,    -1,
      74,    75,    76,    77,    78,    79,    80,    81,    -1,    26,
      -1,    -1,    -1,    -1,    88,    89,    33,    -1,    35,    -1,
      37,    -1,    39,    40,    -1,    -1,    -1,    44,    45,    -1,
      -1,    26,    -1,    -1,    -1,    -1,    -1,    -1,    33,    -1,
      35,    -1,    37,    -1,    39,    40,    -1,    64,    65,    44,
      45,    -1,    -1,    -1,    -1,    -1,    -1,    74,    75,    76,
      77,    78,    79,    80,    81,    -1,    -1,    -1,    -1,    64,
      65,    88,    89,    -1,    -1,    -1,    -1,    -1,    -1,    74,
      75,    76,    77,    78,    79,    80,    81,    -1,    26,    -1,
      -1,    -1,    -1,    88,    89,    33,    -1,    35,    -1,    37,
      -1,    39,    40,    -1,    -1,    -1,    44,    45,    -1,    -1,
      26,    -1,    -1,    -1,    -1,    -1,    -1,    33,    -1,    35,
      -1,    37,    -1,    39,    40,    -1,    64,    65,    44,    45,
      -1,    -1,    -1,    -1,    -1,    -1,    74,    75,    76,    77,
      78,    79,    80,    81,    -1,    -1,    -1,    -1,    64,    65,
      88,    89,    -1,    -1,    -1,    -1,    -1,    -1,    74,    75,
      76,    77,    78,    79,    80,    81,    -1,    26,    -1,    -1,
      -1,    -1,    88,    89,    33,    -1,    35,    -1,    37,    -1,
      39,    40,    26,    -1,    -1,    44,    45,    -1,    -1,    33,
      -1,    35,    -1,    37,    -1,    39,    40,    -1,    -1,    -1,
      44,    45,    -1,    -1,    -1,    64,    65,    -1,    -1,    33,
      -1,    70,    71,    37,    -1,    -1,    40,    -1,    42,    -1,
      64,    65,    -1,    -1,    -1,    -1,    70,    71,    -1,    88,
      89,    -1,    -1,    -1,    -1,    33,    -1,    -1,    -1,    37,
      64,    65,    -1,    -1,    88,    89,    70,    71,    33,    -1,
      -1,    -1,    37,    -1,    -1,    40,    -1,    42,    82,    83,
      84,    85,    86,    -1,    88,    -1,    64,    65,    -1,    -1,
      -1,    -1,    70,    71,    33,    -1,    -1,    75,    37,    64,
      65,    40,    -1,    42,    82,    70,    71,    -1,    -1,    87,
      88,    -1,    -1,    -1,    -1,    -1,    -1,    82,    83,    84,
      85,    86,    -1,    88,    33,    64,    65,    -1,    37,    -1,
      -1,    70,    71,    33,    -1,    -1,    -1,    37,    -1,    -1,
      -1,    -1,    -1,    82,    83,    84,    85,    86,    -1,    88,
      -1,    -1,    -1,    33,    -1,    64,    65,    37,    -1,    -1,
      -1,    70,    71,    -1,    64,    65,    75,    -1,    -1,    -1,
      70,    71,    -1,    82,    -1,    75,    -1,    -1,    87,    88,
      -1,    -1,    82,    -1,    64,    65,    -1,    -1,    88,    -1,
      70,    71,    37,    -1,    -1,    75,    -1,    -1,    -1,     0,
      -1,    -1,    82,    -1,    -1,    -1,    -1,    -1,    88,    10,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    20,
      -1,    22,    23,    -1,    25,    -1,    -1,    28,    -1,    -1,
      31,    -1,    -1,    34,    -1,    36,    37,    38,    -1,    -1,
      -1,    -1,    -1,    88
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     3,     4,     5,     6,     7,     8,     9,    91,    37,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    88,
      92,    93,    94,    95,    96,    97,   103,   104,   105,    26,
      33,    35,    37,    39,    40,    44,    45,    64,    65,    69,
      70,    71,    88,    89,   106,   107,   110,   111,   113,   115,
     117,   118,   119,   120,   121,   122,   123,   124,   126,   128,
     129,   130,   133,    75,    88,   135,   136,   137,    37,    74,
      75,    76,    77,    78,    79,    80,    81,    88,   124,   137,
     138,   139,   140,   141,   143,   145,   147,   151,   153,   157,
     158,   159,    47,    48,    49,    50,    51,    52,    53,    54,
     172,   173,   174,   175,   179,   180,   183,   184,   189,   192,
     196,    33,    37,    40,    42,    64,    65,    70,    71,    82,
      83,    84,    85,    86,   137,   197,   198,   202,   204,   206,
     208,    50,    80,   175,   179,   180,   183,   189,   228,   229,
     230,   231,   232,     0,    92,    88,    98,    99,    37,    37,
      37,    11,    35,   124,    70,    71,   124,   125,   124,   106,
     106,   127,    88,   106,   127,   131,   132,   134,    88,   161,
     188,   195,   195,   195,    72,    21,    19,    20,    17,    18,
      12,    13,    42,    43,    68,    15,    16,    14,    25,    26,
      66,    67,    24,    28,    37,    37,    36,   124,   138,   195,
      37,    37,    37,    37,    37,    11,    25,    10,    20,    22,
      28,    34,    36,    88,   161,   176,   177,   161,   181,   182,
     181,   187,   188,   106,   185,   186,   161,   190,   191,    88,
     193,   194,   138,   174,    70,    71,    83,    84,   206,   207,
     197,    33,    37,    64,    65,    70,    71,    82,    87,   135,
     209,   210,   212,   213,   214,   215,   216,   217,   218,   219,
     220,   222,   224,   225,   227,   209,   195,   195,    37,    88,
      88,    34,    34,    21,    19,    20,   187,   106,   233,   234,
     230,    38,    32,    37,   100,    36,    92,    92,    92,    93,
      96,    97,   195,   195,    38,    29,    41,    30,    30,    46,
      29,    46,    36,    29,    30,    28,    29,    28,    28,    88,
     108,   109,    70,    71,   111,   112,    70,    71,   113,   114,
     114,    70,    71,   115,   116,   116,   118,   118,   118,   118,
     118,   121,   121,   118,   122,   122,   123,   123,   124,   124,
     127,   127,   137,    38,    28,    44,    45,   160,    44,    45,
     170,   160,    44,    45,   162,    44,    45,   165,    76,   145,
     146,   148,   150,   151,   152,   154,   157,   139,    76,   141,
     142,   142,    76,   144,   145,    76,   124,   147,   149,   124,
      76,   124,   153,   155,    27,   177,    31,    30,   182,    31,
      51,   188,    31,    11,    27,   186,    31,    30,   191,    31,
      27,    37,   194,    31,    31,   195,   195,    88,    88,    38,
      37,    70,    71,   225,   226,   210,   218,   195,   195,    37,
      41,    25,    24,    25,    28,    21,    34,    19,    20,    43,
      28,    28,   106,    37,   199,   199,   124,   124,    70,    71,
      83,    84,   202,   203,    70,    71,    83,    84,   204,   205,
     205,    80,    11,   234,    31,    88,    88,    92,   101,   102,
      99,    38,    38,    38,    28,    28,   106,    92,   106,   106,
     132,   106,    88,    92,   110,   188,   110,   110,    27,    29,
      73,   195,   195,   195,   195,   195,   195,    38,    38,   140,
     161,    29,    88,   168,   169,   171,    29,    29,    88,   163,
     164,    29,   166,   167,   168,    29,   195,    23,    28,    34,
      36,   195,   195,   195,    11,   195,    11,    92,    31,    92,
      31,   185,    31,   106,   106,    31,    94,   178,    31,   138,
     195,    31,    28,    28,   199,   199,   218,   195,   195,    38,
      38,    28,    28,   106,   207,    37,    87,   211,   213,   215,
     218,   213,    70,    71,   220,   221,   124,    70,    71,   222,
     223,   223,   207,   198,   198,    38,    88,   200,   201,    28,
      28,   195,   195,    88,    88,   195,   195,    88,    88,   233,
     137,    31,    30,    29,    38,   125,   125,    46,   106,   109,
      28,    28,    28,    28,    28,    28,    46,   138,    36,    29,
      46,   138,   138,    11,    29,    46,   138,    29,    46,    11,
     138,    28,    76,   146,    76,   150,   124,    76,   154,   156,
      28,    28,    28,   149,   150,    28,   155,   156,    27,    11,
      38,   207,   207,    28,    28,    28,    28,   219,   219,    38,
     210,    25,    24,    25,    28,   195,   195,   195,   195,    30,
      29,    38,   198,   198,    28,    28,   199,   199,    28,    28,
     199,   199,    21,    92,   102,   100,   112,   112,   114,   114,
     116,   116,    38,    88,   168,    38,    38,    88,   164,    38,
     167,    75,    88,    38,   146,   150,   195,   195,   195,   142,
     144,   149,    23,   155,    23,   106,    92,    27,   207,   207,
     226,   226,    38,   211,   213,    28,    28,    28,    28,    92,
     201,   203,   203,    28,    28,   205,   205,    28,    28,   138,
      28,    28,    28,   149,   155,   138,   221,   221,   223,   223,
      27,   203,   203,   205,   205,   156,   106
};


/* Prevent warning if -Wmissing-prototypes.  */
int yyparse (void);

/* Error token number */
#define YYTERROR 1

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */


#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N) ((void) 0)
#endif


#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */
#define YYLEX yylex ()

YYSTYPE yylval;

YYLTYPE yylloc;

int yynerrs;
int yychar;

static const int YYEOF = 0;
static const int YYEMPTY = -2;

typedef enum { yyok, yyaccept, yyabort, yyerr } YYRESULTTAG;

#define YYCHK(YYE)							     \
   do { YYRESULTTAG yyflag = YYE; if (yyflag != yyok) return yyflag; }	     \
   while (0)

#if YYDEBUG

#if ! defined (YYFPRINTF)
#  define YYFPRINTF fprintf
#endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)

/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);


# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr,					\
                  Type, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;

#else /* !YYDEBUG */

# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)

#endif /* !YYDEBUG */

/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYMAXDEPTH * sizeof (GLRStackItem)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif

/* Minimum number of free items on the stack allowed after an
   allocation.  This is to allow allocation and initialization
   to be completed by functions that call expandGLRStack before the
   stack is expanded, thus insuring that all necessary pointers get
   properly redirected to new data. */
#define YYHEADROOM 2

#if (! defined (YYSTACKEXPANDABLE) \
     && (! defined (__cplusplus) \
	 || (defined (YYSTYPE_IS_TRIVIAL) && YYSTYPE_IS_TRIVIAL)))
#define YYSTACKEXPANDABLE 1
#else
#define YYSTACKEXPANDABLE 0
#endif

/** State numbers, as in LALR(1) machine */
typedef int yyStateNum;

/** Rule numbers, as in LALR(1) machine */
typedef int yyRuleNum;

/** Grammar symbol */
typedef short int yySymbol;

/** Item references, as in LALR(1) machine */
typedef short int yyItemNum;

typedef struct yyGLRState yyGLRState;
typedef struct yySemanticOption yySemanticOption;
typedef union yyGLRStackItem yyGLRStackItem;
typedef struct yyGLRStack yyGLRStack;
typedef struct yyGLRStateSet yyGLRStateSet;

struct yyGLRState {
  yybool yyisState;
  yybool yyresolved;
  yyStateNum yylrState;
  yyGLRState* yypred;
  size_t yyposn;
  union {
    yySemanticOption* yyfirstVal;
    YYSTYPE yysval;
  } yysemantics;
  YYLTYPE yyloc;
};

struct yyGLRStateSet {
  yyGLRState** yystates;
  size_t yysize, yycapacity;
};

struct yySemanticOption {
  yybool yyisState;
  yyRuleNum yyrule;
  yyGLRState* yystate;
  yySemanticOption* yynext;
};

union yyGLRStackItem {
  yyGLRState yystate;
  yySemanticOption yyoption;
};

struct yyGLRStack {
  int yyerrflag;
  int yyerrState;


  yySymbol* yytokenp;
  jmp_buf yyexception_buffer;
  yyGLRStackItem* yyitems;
  yyGLRStackItem* yynextFree;
  int yyspaceLeft;
  yyGLRState* yysplitPoint;
  yyGLRState* yylastDeleted;
  yyGLRStateSet yytops;
};

static void yyinitGLRStack (yyGLRStack* yystack, size_t yysize);
static void yyexpandGLRStack (yyGLRStack* yystack);
static void yyfreeGLRStack (yyGLRStack* yystack);

static void
yyFail (yyGLRStack* yystack, const char* yyformat, ...)
{
  yystack->yyerrflag = 1;
  if (yyformat != NULL)
    {
      char yymsg[256];
      va_list yyap;
      va_start (yyap, yyformat);
      vsprintf (yymsg, yyformat, yyap);
      yyerror (yymsg);
    }
  longjmp (yystack->yyexception_buffer, 1);
}

#if YYDEBUG || YYERROR_VERBOSE
/** A printable representation of TOKEN.  Valid until next call to
 *  tokenName. */
static inline const char*
yytokenName (yySymbol yytoken)
{
  if (yytoken == YYEMPTY)
    return "";

  return yytname[yytoken];
}
#endif

/** Fill in YYVSP[YYLOW1 .. YYLOW0-1] from the chain of states starting
 *  at YYVSP[YYLOW0].yystate.yypred.  Leaves YYVSP[YYLOW1].yystate.yypred
 *  containing the pointer to the next state in the chain. Assumes
 *  YYLOW1 < YYLOW0.  */
static void yyfillin (yyGLRStackItem *, int, int) ATTRIBUTE_UNUSED;
static void
yyfillin (yyGLRStackItem *yyvsp, int yylow0, int yylow1)
{
  yyGLRState* s;
  int i;
  s = yyvsp[yylow0].yystate.yypred;
  for (i = yylow0-1; i >= yylow1; i -= 1)
    {
      YYASSERT (s->yyresolved);
      yyvsp[i].yystate.yyresolved = yytrue;
      yyvsp[i].yystate.yysemantics.yysval = s->yysemantics.yysval;
      yyvsp[i].yystate.yyloc = s->yyloc;
      s = yyvsp[i].yystate.yypred = s->yypred;
    }
}

/* Do nothing if YYNORMAL or if *YYLOW <= YYLOW1.  Otherwise, fill in
   YYVSP[YYLOW1 .. *YYLOW-1] as in yyfillin and set *YYLOW = YYLOW1.
   For convenience, always return YYLOW1.  */
static inline int yyfill (yyGLRStackItem *, int *, int, yybool)
     ATTRIBUTE_UNUSED;
static inline int
yyfill (yyGLRStackItem *yyvsp, int *yylow, int yylow1, yybool yynormal)
{
  if (!yynormal && yylow1 < *yylow)
    {
      yyfillin (yyvsp, *yylow, yylow1);
      *yylow = yylow1;
    }
  return yylow1;
}

/** Perform user action for rule number YYN, with RHS length YYRHSLEN,
 *  and top stack item YYVSP.  YYLVALP points to place to put semantic
 *  value ($$), and yylocp points to place for location information
 *  (@$). Returns yyok for normal return, yyaccept for YYACCEPT,
 *  yyerr for YYERROR, yyabort for YYABORT. */
static YYRESULTTAG
yyuserAction (yyRuleNum yyn, int yyrhslen, yyGLRStackItem* yyvsp,
	      YYSTYPE* yyvalp,
	      YYLTYPE* yylocp ATTRIBUTE_UNUSED,
	      yyGLRStack* yystack
              )
{
  yybool yynormal ATTRIBUTE_UNUSED = (yystack->yysplitPoint == NULL);
  int yylow;

# undef yyerrok
# define yyerrok (yystack->yyerrState = 0)
# undef YYACCEPT
# define YYACCEPT return yyaccept
# undef YYABORT
# define YYABORT return yyabort
# undef YYERROR
# define YYERROR return yyerrok, yyerr
# undef YYRECOVERING
# define YYRECOVERING (yystack->yyerrState != 0)
# undef yyclearin
# define yyclearin (yychar = *(yystack->yytokenp) = YYEMPTY)
# undef YYFILL
# define YYFILL(N) yyfill (yyvsp, &yylow, N, yynormal)
# undef YYBACKUP
# define YYBACKUP(Token, Value)						     \
  return yyerror ("syntax error: cannot back up"),	     \
	 yyerrok, yyerr

  yylow = 1;
  if (yyrhslen == 0)
    *yyvalp = yyval_default;
  else
    *yyvalp = yyvsp[YYFILL (1-yyrhslen)].yystate.yysemantics.yysval;
  YYLLOC_DEFAULT (*yylocp, yyvsp - yyrhslen, yyrhslen);

  switch (yyn)
    {
        case 2:
#line 162 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
      spec_tree = ((*yyvalp).appl);
    ;}
    break;

  case 3:
#line 167 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
      spec_tree = ((*yyvalp).appl);
    ;}
    break;

  case 4:
#line 172 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
      spec_tree = ((*yyvalp).appl);
    ;}
    break;

  case 5:
#line 177 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
      spec_tree = ((*yyvalp).appl);
    ;}
    break;

  case 6:
#line 182 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
      spec_tree = ((*yyvalp).appl);
    ;}
    break;

  case 7:
#line 187 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
      spec_tree = ((*yyvalp).appl);
    ;}
    break;

  case 8:
#line 192 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
      spec_tree = ((*yyvalp).appl);
    ;}
    break;

  case 9:
#line 204 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed sort expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 10:
#line 213 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 11:
#line 217 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSortArrow((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed arrow sort\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 12:
#line 226 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed non-arrow domain\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 13:
#line 235 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed non-arrow domain elements\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 14:
#line 240 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed non-arrow domain elements\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 15:
#line 249 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed non-arrow domain element\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 16:
#line 258 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 17:
#line 262 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSortStruct(ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.list))));
      gsDebugMsg("parsed structured sort\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 18:
#line 271 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed structured sort constructors\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 19:
#line 276 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed structured sort constructors\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 20:
#line 285 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStructCons((((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.appl), ATmakeList0(), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed structured sort constructor\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 21:
#line 290 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStructCons((((yyGLRStackItem const *)yyvsp)[YYFILL (-4)].yystate.yysemantics.yysval.appl), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list)), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed structured sort constructor\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 22:
#line 299 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeNil());
      gsDebugMsg("parsed recogniser\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 23:
#line 304 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed recogniser id\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 24:
#line 313 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed structured sort projections\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 25:
#line 318 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed structured sort projections\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 26:
#line 327 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStructProj(gsMakeNil(), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed structured sort projection\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 27:
#line 332 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStructProj((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed structured sort projection\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 28:
#line 341 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSortId((((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed primary sort\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 29:
#line 346 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed sort constant\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 30:
#line 351 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed sort constructor\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 31:
#line 356 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 32:
#line 364 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSortId((((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 33:
#line 368 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSortId((((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 34:
#line 372 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSortId((((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 35:
#line 376 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSortId((((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 36:
#line 380 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSortId((((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 37:
#line 388 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSortExprList((((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 38:
#line 392 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSortExprSet((((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 39:
#line 396 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSortExprBag((((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 40:
#line 407 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed data expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 41:
#line 416 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 42:
#line 420 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeWhr((((yyGLRStackItem const *)yyvsp)[YYFILL (-3)].yystate.yysemantics.yysval.appl), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.list))));
      gsDebugMsg("parsed where clause\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 43:
#line 429 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed where clause declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 44:
#line 434 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed where clause declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 45:
#line 443 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeWhrDecl((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed where clause declaration\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 46:
#line 452 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 47:
#line 456 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBinder(gsMakeLambda(), (((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed lambda abstraction\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 48:
#line 461 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBinder(gsMakeForall(), (((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 49:
#line 466 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBinder(gsMakeExists(), (((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 50:
#line 475 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 51:
#line 479 "mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed implication\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 52:
#line 489 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 53:
#line 493 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBinder(gsMakeForall(), (((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 54:
#line 498 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBinder(gsMakeExists(), (((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 55:
#line 507 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 56:
#line 511 "mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed conjunction\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 57:
#line 517 "mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed disjunction\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 58:
#line 527 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 59:
#line 531 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBinder(gsMakeForall(), (((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 60:
#line 536 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBinder(gsMakeExists(), (((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 61:
#line 545 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 62:
#line 549 "mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed equality expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 63:
#line 555 "mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed equality expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 64:
#line 565 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 65:
#line 569 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBinder(gsMakeForall(), (((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 66:
#line 574 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBinder(gsMakeExists(), (((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 67:
#line 583 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 68:
#line 587 "mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed relational expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 69:
#line 593 "mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed relational expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 70:
#line 599 "mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed relational expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 71:
#line 605 "mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed relational expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 72:
#line 611 "mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed relational expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 73:
#line 621 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 74:
#line 625 "mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed list cons expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 75:
#line 635 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 76:
#line 639 "mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed list snoc expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 77:
#line 649 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 78:
#line 653 "mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed list concat expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 79:
#line 663 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 80:
#line 667 "mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed addition or set union\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 81:
#line 673 "mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed subtraction or set difference\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 82:
#line 683 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 83:
#line 687 "mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed div expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 84:
#line 693 "mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed mod expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 85:
#line 703 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 86:
#line 707 "mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed multiplication or set intersection\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 87:
#line 713 "mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed list at expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 88:
#line 723 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 89:
#line 727 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.appl)), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl))));
      gsDebugMsg("parsed prefix data expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 90:
#line 732 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.appl)), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl))));
      gsDebugMsg("parsed prefix data expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 91:
#line 737 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.appl)), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl))));
      gsDebugMsg("parsed prefix data expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 92:
#line 746 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 93:
#line 750 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBinder(gsMakeForall(), (((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 94:
#line 755 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBinder(gsMakeExists(), (((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 95:
#line 764 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 96:
#line 768 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeDataAppl((((yyGLRStackItem const *)yyvsp)[YYFILL (-3)].yystate.yysemantics.yysval.appl), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.list))));
      gsDebugMsg("parsed postfix data expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 97:
#line 777 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed data expressions\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 98:
#line 782 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed data expressions\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 99:
#line 791 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed primary data expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 100:
#line 796 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 101:
#line 800 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 102:
#line 804 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 103:
#line 808 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 104:
#line 816 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed data constant\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 105:
#line 821 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed data constant\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 106:
#line 826 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed data constant\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 107:
#line 831 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed data constant\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 108:
#line 836 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed data constant\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 109:
#line 845 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeDataAppl(gsMakeId(gsMakeOpIdNameListEnum()), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.list))));
      gsDebugMsg("parsed data enumeration\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 110:
#line 850 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeDataAppl(gsMakeId(gsMakeOpIdNameSetEnum()), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.list))));
      gsDebugMsg("parsed data enumeration\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 111:
#line 855 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeDataAppl(gsMakeId(gsMakeOpIdNameBagEnum()), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.list))));
      gsDebugMsg("parsed data enumeration\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 112:
#line 864 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.list));
      gsDebugMsg("parsed bag enumeration elements\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 113:
#line 869 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATconcat((((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed bag enumeration elements\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 114:
#line 878 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed bag enumeration element\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 115:
#line 887 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBinder(gsMakeSetBagComp(), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (-3)].yystate.yysemantics.yysval.appl)), (((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed data comprehension\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 116:
#line 896 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeDataVarId((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed data variable declaration\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 117:
#line 908 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeMultAct(ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.list))));
      gsDebugMsg("parsed multi-action\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 118:
#line 913 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeMultAct(ATmakeList0()));
      gsDebugMsg("parsed multi-action\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 119:
#line 922 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed parameterised id's\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 120:
#line 927 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed parameterised id's\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 121:
#line 936 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeParamId((((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl), ATmakeList0()));
      gsDebugMsg("parsed action or process\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 122:
#line 941 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeParamId((((yyGLRStackItem const *)yyvsp)[YYFILL (-3)].yystate.yysemantics.yysval.appl), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.list))));
      gsDebugMsg("parsed action or process\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 123:
#line 953 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed process expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 124:
#line 962 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 125:
#line 966 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeChoice((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed choice expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 126:
#line 975 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 127:
#line 979 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSum((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed summation\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 128:
#line 988 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 129:
#line 992 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeMerge((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed merge expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 130:
#line 997 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeLMerge((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed left merge expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 131:
#line 1006 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 132:
#line 1010 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSum((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed summation\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 133:
#line 1019 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 134:
#line 1023 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBInit((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed bounded initialisation expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 135:
#line 1032 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 136:
#line 1036 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSum((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed summation\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 137:
#line 1045 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 138:
#line 1049 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeIfThen((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed conditional expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 139:
#line 1054 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeIfThenElse((((yyGLRStackItem const *)yyvsp)[YYFILL (-4)].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed conditional expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 140:
#line 1063 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 141:
#line 1067 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSum((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed summation\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 142:
#line 1076 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 143:
#line 1080 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSeq((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed sequential expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 144:
#line 1089 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 145:
#line 1093 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSeq((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed sequential expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 146:
#line 1102 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 147:
#line 1106 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSum((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed summation\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 148:
#line 1111 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeIfThen((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed conditional expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 149:
#line 1116 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeIfThenElse((((yyGLRStackItem const *)yyvsp)[YYFILL (-4)].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed conditional expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 150:
#line 1125 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 151:
#line 1129 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSum((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed summation\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 152:
#line 1138 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 153:
#line 1142 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeAtTime((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed at time expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 154:
#line 1151 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 155:
#line 1155 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeAtTime((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed at time expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 156:
#line 1164 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 157:
#line 1168 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSync((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed sync expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 158:
#line 1177 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 159:
#line 1181 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSync((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed sync expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 160:
#line 1190 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 161:
#line 1194 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSum((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed summation\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 162:
#line 1199 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeIfThen((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed conditional expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 163:
#line 1204 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeIfThenElse((((yyGLRStackItem const *)yyvsp)[YYFILL (-4)].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed conditional expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 164:
#line 1213 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 165:
#line 1217 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSum((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed summation\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 166:
#line 1226 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 167:
#line 1230 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 168:
#line 1234 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 169:
#line 1238 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 170:
#line 1246 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeDelta());
      gsDebugMsg("parsed process constant\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 171:
#line 1251 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeTau());
      gsDebugMsg("parsed process constant\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 172:
#line 1260 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBlock((((yyGLRStackItem const *)yyvsp)[YYFILL (-3)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed process quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 173:
#line 1265 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeHide((((yyGLRStackItem const *)yyvsp)[YYFILL (-3)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed process quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 174:
#line 1270 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeRename((((yyGLRStackItem const *)yyvsp)[YYFILL (-3)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed process quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 175:
#line 1275 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeComm((((yyGLRStackItem const *)yyvsp)[YYFILL (-3)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed process quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 176:
#line 1280 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeAllow((((yyGLRStackItem const *)yyvsp)[YYFILL (-3)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed process quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 177:
#line 1289 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList0());
      gsDebugMsg("parsed action name set\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 178:
#line 1294 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed action name set\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 179:
#line 1303 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed id's\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 180:
#line 1308 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed id's\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 181:
#line 1317 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList0());
      gsDebugMsg("parsed renaming expression set\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 182:
#line 1322 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed renaming expression set\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 183:
#line 1331 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed renaming expressions\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 184:
#line 1336 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed renaming expressions\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 185:
#line 1345 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeRenameExpr((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed renaming expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 186:
#line 1354 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList0());
      gsDebugMsg("parsed communication expression set\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 187:
#line 1359 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed communication expression set\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 188:
#line 1368 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed communication expressions\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 189:
#line 1373 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed communication expressions\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 190:
#line 1382 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeCommExpr((((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl), gsMakeNil()));
      gsDebugMsg("parsed communication expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 191:
#line 1387 "mcrl2parser.yy"
    {      
      safe_assign(((*yyvalp).appl), gsMakeCommExpr((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), gsMakeNil()));
      gsDebugMsg("parsed communication expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 192:
#line 1392 "mcrl2parser.yy"
    {      
      safe_assign(((*yyvalp).appl), gsMakeCommExpr((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed communication expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 193:
#line 1401 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeMultActName(ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.list))));
      gsDebugMsg("parsed multi action name\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 194:
#line 1410 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed id's\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 195:
#line 1415 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed id's\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 196:
#line 1424 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList0());
      gsDebugMsg("parsed multi action name set\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 197:
#line 1429 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed multi action name set\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 198:
#line 1438 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed multi action names\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 199:
#line 1443 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed multi action names\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 200:
#line 1455 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsSpecEltsToSpec(ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.list))));
      gsDebugMsg("parsed specification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 201:
#line 1464 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed specification elements\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 202:
#line 1469 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed specification elements\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 203:
#line 1478 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed specification element\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 204:
#line 1483 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed specification element\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 205:
#line 1488 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed specification element\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 206:
#line 1493 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed specification element\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 207:
#line 1498 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed specification element\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 208:
#line 1503 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed specification element\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 209:
#line 1508 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed specification element\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 210:
#line 1517 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSortSpec((((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed sort specification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 211:
#line 1526 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), (((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.list));
      gsDebugMsg("parsed sort declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 212:
#line 1531 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATconcat((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed sort declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 213:
#line 1540 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList0());
      int n = ATgetLength((((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.list));
      for (int i = 0; i < n; i++) {
        safe_assign(((*yyvalp).list), ATinsert(((*yyvalp).list), (ATerm) gsMakeSortId(ATAelementAt((((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.list), i))));
      }
      gsDebugMsg("parsed standard sort declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 214:
#line 1549 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) gsMakeSortRef((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl))));
      gsDebugMsg("parsed reference sort declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 215:
#line 1558 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.list));
      gsDebugMsg("parsed domain\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 216:
#line 1563 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) gsMakeSortArrow((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl))));
      gsDebugMsg("parsed domain\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 217:
#line 1572 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeConsSpec((((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed constructor operation specification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 218:
#line 1581 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeMapSpec((((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed operation specification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 219:
#line 1591 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), (((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.list));
      gsDebugMsg("parsed operation declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 220:
#line 1596 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATconcat((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed operation declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 221:
#line 1605 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList0());
      int n = ATgetLength((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list));
      for (int i = 0; i < n; i++) {
        safe_assign(((*yyvalp).list), ATinsert(((*yyvalp).list), (ATerm) gsMakeOpId(ATAelementAt((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), i), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl))));
      }
      gsDebugMsg("parsed operation declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 222:
#line 1618 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeDataEqnSpec((((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed equation specification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 223:
#line 1627 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed equation section\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 224:
#line 1632 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList0());
      int n = ATgetLength((((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.list));
      for (int i = 0; i < n; i++) {
        ATermAppl DataEqn = ATAelementAt((((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.list), i);
        ((*yyvalp).list) = ATinsert(((*yyvalp).list), (ATerm) gsMakeDataEqn((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), ATAgetArgument(DataEqn, 1),
          ATAgetArgument(DataEqn, 2), ATAgetArgument(DataEqn, 3)));
      }
      gsDebugMsg("parsed equation section\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 225:
#line 1647 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed equation declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 226:
#line 1652 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed equation declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 227:
#line 1661 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeDataEqn(ATmakeList0(), gsMakeNil(), (((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed equation declaration\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 228:
#line 1666 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeDataEqn(ATmakeList0(), (((yyGLRStackItem const *)yyvsp)[YYFILL (-4)].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed equation declaration\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 229:
#line 1676 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), (((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.list));
      gsDebugMsg("parsed data variable declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 230:
#line 1681 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATconcat((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed data variable declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 231:
#line 1690 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList0());
      int n = ATgetLength((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list));
      for (int i = 0; i < n; i++) {
        safe_assign(((*yyvalp).list), ATinsert(((*yyvalp).list), (ATerm) gsMakeDataVarId(ATAelementAt((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), i), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl))));
      }
      gsDebugMsg("parsed data variable declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 232:
#line 1703 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActSpec((((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed action specification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 233:
#line 1712 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), (((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.list));
      gsDebugMsg("parsed action declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 234:
#line 1717 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATconcat((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed action declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 235:
#line 1726 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList0());
      int n = ATgetLength((((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.list));
      for (int i = 0; i < n; i++) {
        ((*yyvalp).list) = ATinsert(((*yyvalp).list),
          (ATerm) gsMakeActId(ATAelementAt((((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.list), i), ATmakeList0()));
      }
      gsDebugMsg("parsed action declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 236:
#line 1736 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList0());
      int n = ATgetLength((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list));
      for (int i = 0; i < n; i++) {
        safe_assign(((*yyvalp).list), ATinsert(((*yyvalp).list), (ATerm) gsMakeActId(ATAelementAt((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), i), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.list))));
      }
      gsDebugMsg("parsed action declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 237:
#line 1749 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeProcEqnSpec(ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.list))));
      gsDebugMsg("parsed process specification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 238:
#line 1758 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed process declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 239:
#line 1763 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed process declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 240:
#line 1772 "mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeProcEqn(
        ATmakeList0(), gsMakeProcVarId((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), ATmakeList0()), ATmakeList0(), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed process declaration\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 241:
#line 1778 "mcrl2parser.yy"
    {
      ATermList SortExprs = ATmakeList0();
      int n = ATgetLength((((yyGLRStackItem const *)yyvsp)[YYFILL (-3)].yystate.yysemantics.yysval.list));
      for (int i = 0; i < n; i++) {
        SortExprs = ATinsert(SortExprs, ATgetArgument(ATAelementAt((((yyGLRStackItem const *)yyvsp)[YYFILL (-3)].yystate.yysemantics.yysval.list), i), 1));
      }      
      ((*yyvalp).appl) = gsMakeProcEqn(
        ATmakeList0(), gsMakeProcVarId((((yyGLRStackItem const *)yyvsp)[YYFILL (-5)].yystate.yysemantics.yysval.appl), ATreverse(SortExprs)), (((yyGLRStackItem const *)yyvsp)[YYFILL (-3)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed process declaration\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 242:
#line 1794 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.list));
      gsDebugMsg("parsed data variable declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 243:
#line 1799 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATconcat((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed data variable declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 244:
#line 1808 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeProcessInit(ATmakeList0(), (((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed initialisation\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 245:
#line 1820 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed state formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 246:
#line 1829 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 247:
#line 1833 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateForall((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 248:
#line 1838 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateExists((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 249:
#line 1843 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateNu((((yyGLRStackItem const *)yyvsp)[YYFILL (-3)].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 250:
#line 1848 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateMu((((yyGLRStackItem const *)yyvsp)[YYFILL (-3)].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 251:
#line 1857 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList0());
      gsDebugMsg("parsed fixpoint parameters\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 252:
#line 1862 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed fixpoint parameters\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 253:
#line 1872 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed data variable declaration and initialisations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 254:
#line 1877 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed data variable declaration and initialisations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 255:
#line 1886 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeDataVarIdInit(gsMakeDataVarId((((yyGLRStackItem const *)yyvsp)[YYFILL (-4)].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl)), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed data variable declaration and initialisation\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 256:
#line 1895 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 257:
#line 1899 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateImp((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed implication\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 258:
#line 1908 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 259:
#line 1912 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateForall((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 260:
#line 1917 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateExists((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 261:
#line 1922 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateNu((((yyGLRStackItem const *)yyvsp)[YYFILL (-3)].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 262:
#line 1927 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateMu((((yyGLRStackItem const *)yyvsp)[YYFILL (-3)].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 263:
#line 1936 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 264:
#line 1940 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateAnd((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed conjunction\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 265:
#line 1945 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateOr((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed disjunction\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 266:
#line 1954 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 267:
#line 1958 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateForall((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 268:
#line 1963 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateExists((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 269:
#line 1968 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateNu((((yyGLRStackItem const *)yyvsp)[YYFILL (-3)].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 270:
#line 1973 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateMu((((yyGLRStackItem const *)yyvsp)[YYFILL (-3)].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 271:
#line 1982 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 272:
#line 1986 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateNot((((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed prefix state formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 273:
#line 1991 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateMust((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed prefix state formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 274:
#line 1996 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateMay((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed prefix state formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 275:
#line 2001 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateYaledTimed((((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed prefix state formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 276:
#line 2006 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateDelayTimed((((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed prefix state formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 277:
#line 2015 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 278:
#line 2019 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateForall((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 279:
#line 2024 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateExists((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 280:
#line 2029 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateNu((((yyGLRStackItem const *)yyvsp)[YYFILL (-3)].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 281:
#line 2034 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateMu((((yyGLRStackItem const *)yyvsp)[YYFILL (-3)].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 282:
#line 2043 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed postfix state formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 283:
#line 2048 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateVar(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl), 0), ATLgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl), 1)));
      gsDebugMsg("parsed primary state formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 284:
#line 2053 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateTrue());
      gsDebugMsg("parsed primary state formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 285:
#line 2058 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateFalse());
      gsDebugMsg("parsed primary state formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 286:
#line 2063 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateYaled());
      gsDebugMsg("parsed primary state formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 287:
#line 2068 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateDelay());
      gsDebugMsg("parsed primary state formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 288:
#line 2073 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 289:
#line 2081 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed regular formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 290:
#line 2086 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed regular formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 291:
#line 2095 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 292:
#line 2099 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeRegAlt((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed alternative composition\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 293:
#line 2108 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 294:
#line 2112 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeRegAlt((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed alternative composition\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 295:
#line 2121 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 296:
#line 2125 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeRegSeq((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed sequential composition\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 297:
#line 2134 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 298:
#line 2138 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeRegSeq((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed sequential composition\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 299:
#line 2147 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 300:
#line 2151 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeRegTransOrNil((((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed postfix regular formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 301:
#line 2156 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeRegTrans((((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed postfix regular formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 302:
#line 2165 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 303:
#line 2169 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeRegTransOrNil((((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed postfix regular formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 304:
#line 2174 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeRegTrans((((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed postfix regular formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 305:
#line 2183 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeRegNil());
      gsDebugMsg("parsed primary regular formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 306:
#line 2188 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 307:
#line 2196 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed regular formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 308:
#line 2201 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeRegNil());
      gsDebugMsg("parsed primary regular formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 309:
#line 2206 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 310:
#line 2214 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed action formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 311:
#line 2223 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 312:
#line 2227 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActForall((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 313:
#line 2232 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActExists((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 314:
#line 2241 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 315:
#line 2245 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActImp((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed implication\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 316:
#line 2254 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 317:
#line 2258 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActForall((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 318:
#line 2263 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActExists((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 319:
#line 2272 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 320:
#line 2276 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActAnd((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed conjunction\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 321:
#line 2281 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActOr((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed disjunction\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 322:
#line 2290 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 323:
#line 2294 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActForall((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 324:
#line 2299 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActExists((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 325:
#line 2308 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 326:
#line 2312 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActAt((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed conjunction\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 327:
#line 2321 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 328:
#line 2325 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActNot((((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed prefix action formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 329:
#line 2334 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 330:
#line 2338 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActForall((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 331:
#line 2343 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActExists((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 332:
#line 2352 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed primary action formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 333:
#line 2357 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed primary action formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 334:
#line 2362 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActTrue());
      gsDebugMsg("parsed primary action formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 335:
#line 2367 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActFalse());
      gsDebugMsg("parsed primary action formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 336:
#line 2372 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 337:
#line 2381 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsActionRenameEltsToActionRename(ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.list))));
      gsDebugMsg("parsed action rename\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 338:
#line 2390 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed action rename element\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 339:
#line 2395 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed action rename element\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 340:
#line 2404 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed action rename element\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 341:
#line 2409 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed action rename element\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 342:
#line 2414 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed action rename element\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 343:
#line 2419 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed action rename element\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 344:
#line 2424 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed action rename element\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 345:
#line 2429 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed action rename element\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 346:
#line 2438 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActionRenameRules((((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed rename specification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 347:
#line 2447 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.list));
      gsDebugMsg("parsed rename section\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 348:
#line 2452 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList0());
      int n = ATgetLength((((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.list));
      for (int i = 0; i < n; i++) {
        ATermAppl ActionRenameRule = ATAelementAt((((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.list), i);
	((*yyvalp).list) = ATinsert(((*yyvalp).list), (ATerm) gsMakeActionRenameRule((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), ATAgetArgument(ActionRenameRule, 1),
          ATAgetArgument(ActionRenameRule, 2), ATAgetArgument(ActionRenameRule, 3)));
      }
      gsDebugMsg("parsed var and rename sections\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 349:
#line 2467 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed rename rule\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 350:
#line 2472 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed rename rule\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 351:
#line 2481 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActionRenameRule(ATmakeList0(), (((yyGLRStackItem const *)yyvsp)[YYFILL (-4)].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed rename assignement\n %T\n", ((*yyvalp).appl));
    ;}
    break;


    }

  return yyok;
# undef yyerrok
# undef YYABORT
# undef YYACCEPT
# undef YYERROR
# undef YYBACKUP
# undef yyclearin
# undef YYRECOVERING
/* Line 750 of glr.c.  */
#line 4732 "mcrl2parser.cpp"
}


static void
yyuserMerge (int yyn, YYSTYPE* yy0, YYSTYPE* yy1)
{
  /* `Use' the arguments.  */
  (void) yy0;
  (void) yy1;

  switch (yyn)
    {
      
    }
}

			      /* Bison grammar-table manipulation.  */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
        break;
    }
}

/** Number of symbols composing the right hand side of rule #RULE. */
static inline int
yyrhsLength (yyRuleNum yyrule)
{
  return yyr2[yyrule];
}

/** Left-hand-side symbol for rule #RULE. */
static inline yySymbol
yylhsNonterm (yyRuleNum yyrule)
{
  return yyr1[yyrule];
}

#define yyis_pact_ninf(yystate) \
  ((yystate) == YYPACT_NINF)

/** True iff LR state STATE has only a default reduction (regardless
 *  of token). */
static inline yybool
yyisDefaultedState (yyStateNum yystate)
{
  return yyis_pact_ninf (yypact[yystate]);
}

/** The default reduction for STATE, assuming it has one. */
static inline yyRuleNum
yydefaultAction (yyStateNum yystate)
{
  return yydefact[yystate];
}

#define yyis_table_ninf(yytable_value) \
  0

/** Set *YYACTION to the action to take in YYSTATE on seeing YYTOKEN.
 *  Result R means
 *    R < 0:  Reduce on rule -R.
 *    R = 0:  Error.
 *    R > 0:  Shift to state R.
 *  Set *CONFLICTS to a pointer into yyconfl to 0-terminated list of
 *  conflicting reductions.
 */
static inline void
yygetLRActions (yyStateNum yystate, int yytoken,
	        int* yyaction, const short int** yyconflicts)
{
  int yyindex = yypact[yystate] + yytoken;
  if (yyindex < 0 || YYLAST < yyindex || yycheck[yyindex] != yytoken)
    {
      *yyaction = -yydefact[yystate];
      *yyconflicts = yyconfl;
    }
  else if (! yyis_table_ninf (yytable[yyindex]))
    {
      *yyaction = yytable[yyindex];
      *yyconflicts = yyconfl + yyconflp[yyindex];
    }
  else
    {
      *yyaction = 0;
      *yyconflicts = yyconfl + yyconflp[yyindex];
    }
}

static inline yyStateNum
yyLRgotoState (yyStateNum yystate, yySymbol yylhs)
{
  int yyr;
  yyr = yypgoto[yylhs - YYNTOKENS] + yystate;
  if (0 <= yyr && yyr <= YYLAST && yycheck[yyr] == yystate)
    return yytable[yyr];
  else
    return yydefgoto[yylhs - YYNTOKENS];
}

static inline yybool
yyisShiftAction (int yyaction)
{
  return 0 < yyaction;
}

static inline yybool
yyisErrorAction (int yyaction)
{
  return yyaction == 0;
}

				/* GLRStates */

static void
yyaddDeferredAction (yyGLRStack* yystack, yyGLRState* yystate,
		     yyGLRState* rhs, yyRuleNum yyrule)
{
  yySemanticOption* yynewItem;
  yynewItem = &yystack->yynextFree->yyoption;
  yystack->yyspaceLeft -= 1;
  yystack->yynextFree += 1;
  yynewItem->yyisState = yyfalse;
  yynewItem->yystate = rhs;
  yynewItem->yyrule = yyrule;
  yynewItem->yynext = yystate->yysemantics.yyfirstVal;
  yystate->yysemantics.yyfirstVal = yynewItem;
  if (yystack->yyspaceLeft < YYHEADROOM)
    yyexpandGLRStack (yystack);
}

				/* GLRStacks */

/** Initialize SET to a singleton set containing an empty stack. */
static void
yyinitStateSet (yyGLRStateSet* yyset)
{
  yyset->yysize = 1;
  yyset->yycapacity = 16;
  yyset->yystates = (yyGLRState**) YYMALLOC (16 * sizeof yyset->yystates[0]);
  yyset->yystates[0] = NULL;
}

static void yyfreeStateSet (yyGLRStateSet* yyset)
{
  YYFREE (yyset->yystates);
}

/** Initialize STACK to a single empty stack, with total maximum
 *  capacity for all stacks of SIZE. */
static void
yyinitGLRStack (yyGLRStack* yystack, size_t yysize)
{
  yystack->yyerrflag = 0;
  yystack->yyerrState = 0;
  yynerrs = 0;
  yystack->yyspaceLeft = yysize;
  yystack->yynextFree = yystack->yyitems =
    (yyGLRStackItem*) YYMALLOC (yysize * sizeof yystack->yynextFree[0]);
  yystack->yysplitPoint = NULL;
  yystack->yylastDeleted = NULL;
  yyinitStateSet (&yystack->yytops);
}

#define YYRELOC(YYFROMITEMS,YYTOITEMS,YYX,YYTYPE) \
  &((YYTOITEMS) - ((YYFROMITEMS) - (yyGLRStackItem*) (YYX)))->YYTYPE

/** If STACK is expandable, extend it.  WARNING: Pointers into the
    stack from outside should be considered invalid after this call.
    We always expand when there are 1 or fewer items left AFTER an
    allocation, so that we can avoid having external pointers exist
    across an allocation. */
static void
yyexpandGLRStack (yyGLRStack* yystack)
{
#if YYSTACKEXPANDABLE
  yyGLRStack yynewStack;
  yyGLRStackItem* yyp0, *yyp1;
  size_t yysize, yynewSize;
  size_t yyn;
  yysize = yystack->yynextFree - yystack->yyitems;
  if (YYMAXDEPTH <= yysize)
    yyFail (yystack, "parser stack overflow");
  yynewSize = 2*yysize;
  if (YYMAXDEPTH < yynewSize)
    yynewSize = YYMAXDEPTH;
  yyinitGLRStack (&yynewStack, yynewSize);
  for (yyp0 = yystack->yyitems, yyp1 = yynewStack.yyitems, yyn = yysize;
       0 < yyn;
       yyn -= 1, yyp0 += 1, yyp1 += 1)
    {
      *yyp1 = *yyp0;
      if (*(yybool *) yyp0)
	{
	  yyGLRState* yys0 = &yyp0->yystate;
	  yyGLRState* yys1 = &yyp1->yystate;
	  if (yys0->yypred != NULL)
	    yys1->yypred =
	      YYRELOC (yyp0, yyp1, yys0->yypred, yystate);
	  if (! yys0->yyresolved && yys0->yysemantics.yyfirstVal != NULL)
	    yys1->yysemantics.yyfirstVal =
	      YYRELOC(yyp0, yyp1, yys0->yysemantics.yyfirstVal, yyoption);
	}
      else
	{
	  yySemanticOption* yyv0 = &yyp0->yyoption;
	  yySemanticOption* yyv1 = &yyp1->yyoption;
	  if (yyv0->yystate != NULL)
	    yyv1->yystate = YYRELOC (yyp0, yyp1, yyv0->yystate, yystate);
	  if (yyv0->yynext != NULL)
	    yyv1->yynext = YYRELOC (yyp0, yyp1, yyv0->yynext, yyoption);
	}
    }
  if (yystack->yysplitPoint != NULL)
    yystack->yysplitPoint = YYRELOC (yystack->yyitems, yynewStack.yyitems,
				 yystack->yysplitPoint, yystate);

  for (yyn = 0; yyn < yystack->yytops.yysize; yyn += 1)
    if (yystack->yytops.yystates[yyn] != NULL)
      yystack->yytops.yystates[yyn] =
	YYRELOC (yystack->yyitems, yynewStack.yyitems,
		 yystack->yytops.yystates[yyn], yystate);
  YYFREE (yystack->yyitems);
  yystack->yyitems = yynewStack.yyitems;
  yystack->yynextFree = yynewStack.yynextFree + yysize;
  yystack->yyspaceLeft = yynewStack.yyspaceLeft - yysize;

#else

  yyFail (yystack, "parser stack overflow");
#endif
}

static void
yyfreeGLRStack (yyGLRStack* yystack)
{
  YYFREE (yystack->yyitems);
  yyfreeStateSet (&yystack->yytops);
}

/** Assuming that S is a GLRState somewhere on STACK, update the
 *  splitpoint of STACK, if needed, so that it is at least as deep as
 *  S. */
static inline void
yyupdateSplit (yyGLRStack* yystack, yyGLRState* yys)
{
  if (yystack->yysplitPoint != NULL && yystack->yysplitPoint > yys)
    yystack->yysplitPoint = yys;
}

/** Invalidate stack #K in STACK. */
static inline void
yymarkStackDeleted (yyGLRStack* yystack, int yyk)
{
  if (yystack->yytops.yystates[yyk] != NULL)
    yystack->yylastDeleted = yystack->yytops.yystates[yyk];
  yystack->yytops.yystates[yyk] = NULL;
}

/** Undelete the last stack that was marked as deleted.  Can only be
    done once after a deletion, and only when all other stacks have
    been deleted. */
static void
yyundeleteLastStack (yyGLRStack* yystack)
{
  if (yystack->yylastDeleted == NULL || yystack->yytops.yysize != 0)
    return;
  yystack->yytops.yystates[0] = yystack->yylastDeleted;
  yystack->yytops.yysize = 1;
  YYDPRINTF ((stderr, "Restoring last deleted stack as stack #0.\n"));
  yystack->yylastDeleted = NULL;
}

static inline void
yyremoveDeletes (yyGLRStack* yystack)
{
  size_t yyi, yyj;
  yyi = yyj = 0;
  while (yyj < yystack->yytops.yysize)
    {
      if (yystack->yytops.yystates[yyi] == NULL)
	{
	  if (yyi == yyj)
	    {
	      YYDPRINTF ((stderr, "Removing dead stacks.\n"));
	    }
	  yystack->yytops.yysize -= 1;
	}
      else
	{
	  yystack->yytops.yystates[yyj] = yystack->yytops.yystates[yyi];
	  if (yyj != yyi)
	    {
	      YYDPRINTF ((stderr, "Rename stack %lu -> %lu.\n",
			  (unsigned long int) yyi, (unsigned long int) yyj));
	    }
	  yyj += 1;
	}
      yyi += 1;
    }
}

/** Shift to a new state on stack #K of STACK, corresponding to LR state
 * LRSTATE, at input position POSN, with (resolved) semantic value SVAL. */
static inline void
yyglrShift (yyGLRStack* yystack, int yyk, yyStateNum yylrState, size_t yyposn,
	    YYSTYPE yysval, YYLTYPE* yylocp)
{
  yyGLRStackItem* yynewItem;

  yynewItem = yystack->yynextFree;
  yystack->yynextFree += 1;
  yystack->yyspaceLeft -= 1;
  yynewItem->yystate.yyisState = yytrue;
  yynewItem->yystate.yylrState = yylrState;
  yynewItem->yystate.yyposn = yyposn;
  yynewItem->yystate.yyresolved = yytrue;
  yynewItem->yystate.yypred = yystack->yytops.yystates[yyk];
  yystack->yytops.yystates[yyk] = &yynewItem->yystate;
  yynewItem->yystate.yysemantics.yysval = yysval;
  yynewItem->yystate.yyloc = *yylocp;
  if (yystack->yyspaceLeft < YYHEADROOM)
    yyexpandGLRStack (yystack);
}

/** Shift stack #K of YYSTACK, to a new state corresponding to LR
 *  state YYLRSTATE, at input position YYPOSN, with the (unresolved)
 *  semantic value of YYRHS under the action for YYRULE. */
static inline void
yyglrShiftDefer (yyGLRStack* yystack, int yyk, yyStateNum yylrState,
		 size_t yyposn, yyGLRState* rhs, yyRuleNum yyrule)
{
  yyGLRStackItem* yynewItem;

  yynewItem = yystack->yynextFree;
  yynewItem->yystate.yyisState = yytrue;
  yynewItem->yystate.yylrState = yylrState;
  yynewItem->yystate.yyposn = yyposn;
  yynewItem->yystate.yyresolved = yyfalse;
  yynewItem->yystate.yypred = yystack->yytops.yystates[yyk];
  yynewItem->yystate.yysemantics.yyfirstVal = NULL;
  yystack->yytops.yystates[yyk] = &yynewItem->yystate;
  yystack->yynextFree += 1;
  yystack->yyspaceLeft -= 1;
  yyaddDeferredAction (yystack, &yynewItem->yystate, rhs, yyrule);
}

/** Pop the symbols consumed by reduction #RULE from the top of stack
 *  #K of STACK, and perform the appropriate semantic action on their
 *  semantic values.  Assumes that all ambiguities in semantic values
 *  have been previously resolved. Set *VALP to the resulting value,
 *  and *LOCP to the computed location (if any).  Return value is as
 *  for userAction. */
static inline YYRESULTTAG
yydoAction (yyGLRStack* yystack, int yyk, yyRuleNum yyrule,
	    YYSTYPE* yyvalp, YYLTYPE* yylocp)
{
  int yynrhs = yyrhsLength (yyrule);

  if (yystack->yysplitPoint == NULL)
    {
      /* Standard special case: single stack. */
      yyGLRStackItem* rhs = (yyGLRStackItem*) yystack->yytops.yystates[yyk];
      YYASSERT (yyk == 0);
      yystack->yynextFree -= yynrhs;
      yystack->yyspaceLeft += yynrhs;
      yystack->yytops.yystates[0] = & yystack->yynextFree[-1].yystate;
      return yyuserAction (yyrule, yynrhs, rhs,
			   yyvalp, yylocp, yystack);
    }
  else
    {
      int yyi;
      yyGLRState* yys;
      yyGLRStackItem yyrhsVals[YYMAXRHS + YYMAXLEFT + 1];
      yys = yyrhsVals[YYMAXRHS + YYMAXLEFT].yystate.yypred
	= yystack->yytops.yystates[yyk];
      for (yyi = 0; yyi < yynrhs; yyi += 1)
	{
	  yys = yys->yypred;
	  YYASSERT (yys);
	}
      yyupdateSplit (yystack, yys);
      yystack->yytops.yystates[yyk] = yys;
      return yyuserAction (yyrule, yynrhs, yyrhsVals + YYMAXRHS + YYMAXLEFT - 1,
			   yyvalp, yylocp, yystack);
    }
}

#if !YYDEBUG
# define YY_REDUCE_PRINT(K, Rule)
#else
# define YY_REDUCE_PRINT(K, Rule)	\
do {					\
  if (yydebug)				\
    yy_reduce_print (K, Rule);		\
} while (0)

/*----------------------------------------------------------.
| Report that the RULE is going to be reduced on stack #K.  |
`----------------------------------------------------------*/

static inline void
yy_reduce_print (size_t yyk, yyRuleNum yyrule)
{
  int yyi;
  YYFPRINTF (stderr, "Reducing stack %lu by rule %d (line %lu), ",
	     (unsigned long int) yyk, yyrule - 1,
	     (unsigned long int) yyrline[yyrule]);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytokenName (yyrhs[yyi]));
  YYFPRINTF (stderr, "-> %s\n", yytokenName (yyr1[yyrule]));
}
#endif

/** Pop items off stack #K of STACK according to grammar rule RULE,
 *  and push back on the resulting nonterminal symbol.  Perform the
 *  semantic action associated with RULE and store its value with the
 *  newly pushed state, if FORCEEVAL or if STACK is currently
 *  unambiguous.  Otherwise, store the deferred semantic action with
 *  the new state.  If the new state would have an identical input
 *  position, LR state, and predecessor to an existing state on the stack,
 *  it is identified with that existing state, eliminating stack #K from
 *  the STACK. In this case, the (necessarily deferred) semantic value is
 *  added to the options for the existing state's semantic value.
 */
static inline YYRESULTTAG
yyglrReduce (yyGLRStack* yystack, size_t yyk, yyRuleNum yyrule,
             yybool yyforceEval)
{
  size_t yyposn = yystack->yytops.yystates[yyk]->yyposn;

  if (yyforceEval || yystack->yysplitPoint == NULL)
    {
      YYSTYPE yysval;
      YYLTYPE yyloc;

      YY_REDUCE_PRINT (yyk, yyrule);
      YYCHK (yydoAction (yystack, yyk, yyrule, &yysval, &yyloc));
      yyglrShift (yystack, yyk,
		  yyLRgotoState (yystack->yytops.yystates[yyk]->yylrState,
				 yylhsNonterm (yyrule)),
		  yyposn, yysval, &yyloc);
    }
  else
    {
      size_t yyi;
      int yyn;
      yyGLRState* yys, *yys0 = yystack->yytops.yystates[yyk];
      yyStateNum yynewLRState;

      for (yys = yystack->yytops.yystates[yyk], yyn = yyrhsLength (yyrule);
	   0 < yyn; yyn -= 1)
	{
	  yys = yys->yypred;
	  YYASSERT (yys);
	}
      yyupdateSplit (yystack, yys);
      yynewLRState = yyLRgotoState (yys->yylrState, yylhsNonterm (yyrule));
      YYDPRINTF ((stderr,
		  "Reduced stack %lu by rule #%d; action deferred. Now in state %d.\n",
		  (unsigned long int) yyk, yyrule - 1, yynewLRState));
      for (yyi = 0; yyi < yystack->yytops.yysize; yyi += 1)
	if (yyi != yyk && yystack->yytops.yystates[yyi] != NULL)
	  {
	    yyGLRState* yyp, *yysplit = yystack->yysplitPoint;
	    yyp = yystack->yytops.yystates[yyi];
	    while (yyp != yys && yyp != yysplit && yyp->yyposn >= yyposn)
	      {
		if (yyp->yylrState == yynewLRState && yyp->yypred == yys)
		  {
		    yyaddDeferredAction (yystack, yyp, yys0, yyrule);
		    yymarkStackDeleted (yystack, yyk);
		    YYDPRINTF ((stderr, "Merging stack %lu into stack %lu.\n",
				(unsigned long int) yyk,
				(unsigned long int) yyi));
		    return yyok;
		  }
		yyp = yyp->yypred;
	      }
	  }
      yystack->yytops.yystates[yyk] = yys;
      yyglrShiftDefer (yystack, yyk, yynewLRState, yyposn, yys0, yyrule);
    }
  return yyok;
}

static int
yysplitStack (yyGLRStack* yystack, int yyk)
{
  if (yystack->yysplitPoint == NULL)
    {
      YYASSERT (yyk == 0);
      yystack->yysplitPoint = yystack->yytops.yystates[yyk];
    }
  if (yystack->yytops.yysize >= yystack->yytops.yycapacity)
    {
      yystack->yytops.yycapacity *= 2;
      yystack->yytops.yystates =
	(yyGLRState**) YYREALLOC (yystack->yytops.yystates,
				  (yystack->yytops.yycapacity
				   * sizeof yystack->yytops.yystates[0]));
    }
  yystack->yytops.yystates[yystack->yytops.yysize]
    = yystack->yytops.yystates[yyk];
  yystack->yytops.yysize += 1;
  return yystack->yytops.yysize-1;
}

/** True iff Y0 and Y1 represent identical options at the top level.
 *  That is, they represent the same rule applied to RHS symbols
 *  that produce the same terminal symbols. */
static yybool
yyidenticalOptions (yySemanticOption* yyy0, yySemanticOption* yyy1)
{
  if (yyy0->yyrule == yyy1->yyrule)
    {
      yyGLRState *yys0, *yys1;
      int yyn;
      for (yys0 = yyy0->yystate, yys1 = yyy1->yystate,
	   yyn = yyrhsLength (yyy0->yyrule);
	   yyn > 0;
	   yys0 = yys0->yypred, yys1 = yys1->yypred, yyn -= 1)
	if (yys0->yyposn != yys1->yyposn)
	  return yyfalse;
      return yytrue;
    }
  else
    return yyfalse;
}

/** Assuming identicalOptions (Y0,Y1), (destructively) merge the
 *  alternative semantic values for the RHS-symbols of Y1 into the
 *  corresponding semantic value sets of the symbols of Y0. */
static void
yymergeOptionSets (yySemanticOption* yyy0, yySemanticOption* yyy1)
{
  yyGLRState *yys0, *yys1;
  int yyn;
  for (yys0 = yyy0->yystate, yys1 = yyy1->yystate,
       yyn = yyrhsLength (yyy0->yyrule);
       yyn > 0;
       yys0 = yys0->yypred, yys1 = yys1->yypred, yyn -= 1)
    if (yys0 == yys1)
      break;
    else if (! yys0->yyresolved && ! yys1->yyresolved)
      {
	yySemanticOption* yyz;
	for (yyz = yys0->yysemantics.yyfirstVal; yyz->yynext != NULL;
	     yyz = yyz->yynext)
	  continue;
	yyz->yynext = yys1->yysemantics.yyfirstVal;
      }
}

/** Y0 and Y1 represent two possible actions to take in a given
 *  parsing state; return 0 if no combination is possible,
 *  1 if user-mergeable, 2 if Y0 is preferred, 3 if Y1 is preferred. */
static int
yypreference (yySemanticOption* y0, yySemanticOption* y1)
{
  yyRuleNum r0 = y0->yyrule, r1 = y1->yyrule;
  int p0 = yydprec[r0], p1 = yydprec[r1];

  if (p0 == p1)
    {
      if (yymerger[r0] == 0 || yymerger[r0] != yymerger[r1])
	return 0;
      else
	return 1;
    }
  if (p0 == 0 || p1 == 0)
    return 0;
  if (p0 < p1)
    return 3;
  if (p1 < p0)
    return 2;
  return 0;
}

static YYRESULTTAG yyresolveValue (yySemanticOption* yyoptionList,
				   yyGLRStack* yystack, YYSTYPE* yyvalp,
				   YYLTYPE* yylocp);

static YYRESULTTAG
yyresolveStates (yyGLRState* yys, int yyn, yyGLRStack* yystack)
{
  YYRESULTTAG yyflag;
  if (0 < yyn)
    {
      YYASSERT (yys->yypred);
      yyflag = yyresolveStates (yys->yypred, yyn-1, yystack);
      if (yyflag != yyok)
	return yyflag;
      if (! yys->yyresolved)
	{
	  yyflag = yyresolveValue (yys->yysemantics.yyfirstVal, yystack,
				   &yys->yysemantics.yysval, &yys->yyloc
				  );
	  if (yyflag != yyok)
	    return yyflag;
	  yys->yyresolved = yytrue;
	}
    }
  return yyok;
}

static YYRESULTTAG
yyresolveAction (yySemanticOption* yyopt, yyGLRStack* yystack,
	         YYSTYPE* yyvalp, YYLTYPE* yylocp)
{
  yyGLRStackItem yyrhsVals[YYMAXRHS + YYMAXLEFT + 1];
  int yynrhs;

  yynrhs = yyrhsLength (yyopt->yyrule);
  YYCHK (yyresolveStates (yyopt->yystate, yynrhs, yystack));
  yyrhsVals[YYMAXRHS + YYMAXLEFT].yystate.yypred = yyopt->yystate;
  return yyuserAction (yyopt->yyrule, yynrhs,
		       yyrhsVals + YYMAXRHS + YYMAXLEFT - 1,
		       yyvalp, yylocp, yystack);
}

#if YYDEBUG
static void
yyreportTree (yySemanticOption* yyx, int yyindent)
{
  int yynrhs = yyrhsLength (yyx->yyrule);
  int yyi;
  yyGLRState* yys;
  yyGLRState* yystates[YYMAXRHS];
  yyGLRState yyleftmost_state;

  for (yyi = yynrhs, yys = yyx->yystate; 0 < yyi; yyi -= 1, yys = yys->yypred)
    yystates[yyi] = yys;
  if (yys == NULL)
    {
      yyleftmost_state.yyposn = 0;
      yystates[0] = &yyleftmost_state;
    }
  else
    yystates[0] = yys;

  if (yyx->yystate->yyposn < yys->yyposn + 1)
    YYFPRINTF (stderr, "%*s%s -> <Rule %d, empty>\n",
	       yyindent, "", yytokenName (yylhsNonterm (yyx->yyrule)),
	       yyx->yyrule);
  else
    YYFPRINTF (stderr, "%*s%s -> <Rule %d, tokens %lu .. %lu>\n",
	       yyindent, "", yytokenName (yylhsNonterm (yyx->yyrule)),
	       yyx->yyrule, (unsigned long int) (yys->yyposn + 1),
	       (unsigned long int) yyx->yystate->yyposn);
  for (yyi = 1; yyi <= yynrhs; yyi += 1)
    {
      if (yystates[yyi]->yyresolved)
	{
	  if (yystates[yyi-1]->yyposn+1 > yystates[yyi]->yyposn)
	    YYFPRINTF (stderr, "%*s%s <empty>\n", yyindent+2, "",
		       yytokenName (yyrhs[yyprhs[yyx->yyrule]+yyi-1]));
	  else
	    YYFPRINTF (stderr, "%*s%s <tokens %lu .. %lu>\n", yyindent+2, "",
		       yytokenName (yyrhs[yyprhs[yyx->yyrule]+yyi-1]),
		       (unsigned long int) (yystates[yyi - 1]->yyposn + 1),
		       (unsigned long int) yystates[yyi]->yyposn);
	}
      else
	yyreportTree (yystates[yyi]->yysemantics.yyfirstVal, yyindent+2);
    }
}
#endif

static void
yyreportAmbiguity (yySemanticOption* yyx0, yySemanticOption* yyx1,
		   yyGLRStack* yystack)
{
  /* `Unused' warnings.  */
  (void) yyx0;
  (void) yyx1;

#if YYDEBUG
  YYFPRINTF (stderr, "Ambiguity detected.\n");
  YYFPRINTF (stderr, "Option 1,\n");
  yyreportTree (yyx0, 2);
  YYFPRINTF (stderr, "\nOption 2,\n");
  yyreportTree (yyx1, 2);
  YYFPRINTF (stderr, "\n");
#endif
  yyFail (yystack, "ambiguity detected");
}


/** Resolve the ambiguity represented by OPTIONLIST, perform the indicated
 *  actions, and return the result. */
static YYRESULTTAG
yyresolveValue (yySemanticOption* yyoptionList, yyGLRStack* yystack,
		YYSTYPE* yyvalp, YYLTYPE* yylocp)
{
  yySemanticOption* yybest;
  yySemanticOption* yyp;
  int yymerge;

  yybest = yyoptionList;
  yymerge = 0;
  for (yyp = yyoptionList->yynext; yyp != NULL; yyp = yyp->yynext)
    {
      if (yyidenticalOptions (yybest, yyp))
	yymergeOptionSets (yybest, yyp);
      else
	switch (yypreference (yybest, yyp))
	  {
	  case 0:
	    yyreportAmbiguity (yybest, yyp, yystack);
	    break;
	  case 1:
	    yymerge = 1;
	    break;
	  case 2:
	    break;
	  case 3:
	    yybest = yyp;
	    yymerge = 0;
	    break;
	  }
    }

  if (yymerge)
    {
      int yyprec = yydprec[yybest->yyrule];
      YYCHK (yyresolveAction (yybest, yystack, yyvalp, yylocp));
      for (yyp = yybest->yynext; yyp != NULL; yyp = yyp->yynext)
	{
	  if (yyprec == yydprec[yyp->yyrule])
	    {
	      YYSTYPE yyval1;
	      YYLTYPE yydummy;
	      YYCHK (yyresolveAction (yyp, yystack, &yyval1, &yydummy));
	      yyuserMerge (yymerger[yyp->yyrule], yyvalp, &yyval1);
	    }
	}
      return yyok;
    }
  else
    return yyresolveAction (yybest, yystack, yyvalp, yylocp);
}

static YYRESULTTAG
yyresolveStack (yyGLRStack* yystack)
{
  if (yystack->yysplitPoint != NULL)
    {
      yyGLRState* yys;
      int yyn;

      for (yyn = 0, yys = yystack->yytops.yystates[0];
	   yys != yystack->yysplitPoint;
	   yys = yys->yypred, yyn += 1)
	continue;
      YYCHK (yyresolveStates (yystack->yytops.yystates[0], yyn, yystack
			     ));
    }
  return yyok;
}

static void
yycompressStack (yyGLRStack* yystack)
{
  yyGLRState* yyp, *yyq, *yyr;

  if (yystack->yytops.yysize != 1 || yystack->yysplitPoint == NULL)
    return;

  for (yyp = yystack->yytops.yystates[0], yyq = yyp->yypred, yyr = NULL;
       yyp != yystack->yysplitPoint;
       yyr = yyp, yyp = yyq, yyq = yyp->yypred)
    yyp->yypred = yyr;

  yystack->yyspaceLeft += yystack->yynextFree - yystack->yyitems;
  yystack->yynextFree = ((yyGLRStackItem*) yystack->yysplitPoint) + 1;
  yystack->yyspaceLeft -= yystack->yynextFree - yystack->yyitems;
  yystack->yysplitPoint = NULL;
  yystack->yylastDeleted = NULL;

  while (yyr != NULL)
    {
      yystack->yynextFree->yystate = *yyr;
      yyr = yyr->yypred;
      yystack->yynextFree->yystate.yypred = & yystack->yynextFree[-1].yystate;
      yystack->yytops.yystates[0] = &yystack->yynextFree->yystate;
      yystack->yynextFree += 1;
      yystack->yyspaceLeft -= 1;
    }
}

static YYRESULTTAG
yyprocessOneStack (yyGLRStack* yystack, int yyk,
	           size_t yyposn, YYSTYPE* yylvalp, YYLTYPE* yyllocp
		  )
{
  int yyaction;
  const short int* yyconflicts;
  yyRuleNum yyrule;
  yySymbol* const yytokenp = yystack->yytokenp;

  while (yystack->yytops.yystates[yyk] != NULL)
    {
      yyStateNum yystate = yystack->yytops.yystates[yyk]->yylrState;
      YYDPRINTF ((stderr, "Stack %d Entering state %d\n", yyk, yystate));

      YYASSERT (yystate != YYFINAL);

      if (yyisDefaultedState (yystate))
	{
	  yyrule = yydefaultAction (yystate);
	  if (yyrule == 0)
	    {
	      YYDPRINTF ((stderr, "Stack %d dies.\n", yyk));
	      yymarkStackDeleted (yystack, yyk);
	      return yyok;
	    }
	  YYCHK (yyglrReduce (yystack, yyk, yyrule, yyfalse));
	}
      else
	{
	  if (*yytokenp == YYEMPTY)
	    {
	      YYDPRINTF ((stderr, "Reading a token: "));
	      yychar = YYLEX;
	      *yytokenp = YYTRANSLATE (yychar);
	      YY_SYMBOL_PRINT ("Next token is", *yytokenp, yylvalp, yyllocp);
	    }
	  yygetLRActions (yystate, *yytokenp, &yyaction, &yyconflicts);

	  while (*yyconflicts != 0)
	    {
	      int yynewStack = yysplitStack (yystack, yyk);
	      YYDPRINTF ((stderr, "Splitting off stack %d from %d.\n",
			  yynewStack, yyk));
	      YYCHK (yyglrReduce (yystack, yynewStack,
				  *yyconflicts, yyfalse));
	      YYCHK (yyprocessOneStack (yystack, yynewStack, yyposn,
					yylvalp, yyllocp));
	      yyconflicts += 1;
	    }

	  if (yyisShiftAction (yyaction))
	    {
	      YYDPRINTF ((stderr, "On stack %d, ", yyk));
	      YY_SYMBOL_PRINT ("shifting", *yytokenp, yylvalp, yyllocp);
	      yyglrShift (yystack, yyk, yyaction, yyposn+1,
			  *yylvalp, yyllocp);
	      YYDPRINTF ((stderr, ", now in state #%d\n",
			  yystack->yytops.yystates[yyk]->yylrState));
	      break;
	    }
	  else if (yyisErrorAction (yyaction))
	    {
	      YYDPRINTF ((stderr, "Stack %d dies.\n", yyk));
	      yymarkStackDeleted (yystack, yyk);
	      break;
	    }
	  else
	    YYCHK (yyglrReduce (yystack, yyk, -yyaction, yyfalse));
	}
    }
  return yyok;
}

static void
yyreportSyntaxError (yyGLRStack* yystack,
		     YYSTYPE* yylvalp, YYLTYPE* yyllocp)
{
  /* `Unused' warnings. */
  (void) yylvalp;
  (void) yyllocp;

  if (yystack->yyerrState == 0)
    {
#if YYERROR_VERBOSE
      yySymbol* const yytokenp = yystack->yytokenp;
      int yyn;
      yyn = yypact[yystack->yytops.yystates[0]->yylrState];
      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  size_t yysize = 0;
	  const char* yyprefix;
	  char* yymsg;
	  int yyx;

	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  int yyxbegin = yyn < 0 ? -yyn : 0;

	  /* Stay within bounds of both yycheck and yytname.  */
	  int yychecklim = YYLAST - yyn;
	  int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
	  int yycount = 0;

	  yyprefix = ", expecting ";
	  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      {
		yysize += strlen (yyprefix) + strlen (yytokenName (yyx));
		yycount += 1;
		if (yycount == 5)
		  {
		    yysize = 0;
		    break;
		  }
		yyprefix = " or ";
	      }
	  yysize += (sizeof ("syntax error, unexpected ")
		     + strlen (yytokenName (*yytokenp)));
	  yymsg = (char*) YYMALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char* yyp = yymsg;
	      sprintf (yyp, "syntax error%s%s",
		       (*yytokenp == YYEMPTY ? "" : ", unexpected "),
		       yytokenName (*yytokenp));
	      yyp += strlen (yyp);
	      if (yycount < 5)
		{
		  yyprefix = ", expecting ";
		  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			sprintf (yyp, "%s%s", yyprefix, yytokenName (yyx));
			yyp += strlen (yyp);
			yyprefix = " or ";
		      }
		}
	      yyerror (yymsg);
	      YYFREE (yymsg);
	    }
	  else
	    yyerror ("syntax error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("syntax error");
      yynerrs += 1;
    }
}

/* Recover from a syntax error on YYSTACK, assuming that YYTOKENP,
   YYLVALP, and YYLLOCP point to the syntactic category, semantic
   value, and location of the look-ahead.  */
static void
yyrecoverSyntaxError (yyGLRStack* yystack,
		      YYSTYPE* yylvalp,
		      YYLTYPE* yyllocp ATTRIBUTE_UNUSED
		      )
{
  yySymbol* const yytokenp = yystack->yytokenp;
  size_t yyk;
  int yyj;

  if (yystack->yyerrState == 3)
    /* We just shifted the error token and (perhaps) took some
       reductions.  Skip tokens until we can proceed.  */
    while (yytrue)
      {
	if (*yytokenp == YYEOF)
	  {
	    /* Now pop stack until empty and fail. */
	    while (yystack->yytops.yystates[0] != NULL)
	      {
		yyGLRState *yys = yystack->yytops.yystates[0];

		yydestruct ("Error: popping",
                            yystos[yys->yylrState],
			    &yys->yysemantics.yysval);
		yystack->yytops.yystates[0] = yys->yypred;
		yystack->yynextFree -= 1;
		yystack->yyspaceLeft += 1;
	      }
	    yyFail (yystack, NULL);
	  }
	if (*yytokenp != YYEMPTY)
	  {
	    yydestruct ("Error: discarding",
			*yytokenp, yylvalp);
	  }
	YYDPRINTF ((stderr, "Reading a token: "));
	yychar = YYLEX;
	*yytokenp = YYTRANSLATE (yychar);
	YY_SYMBOL_PRINT ("Next token is", *yytokenp, yylvalp, yyllocp);
	yyj = yypact[yystack->yytops.yystates[0]->yylrState];
	if (yyis_pact_ninf (yyj))
	  return;
	yyj += *yytokenp;
	if (yyj < 0 || YYLAST < yyj || yycheck[yyj] != *yytokenp)
	  {
	    if (yydefact[yystack->yytops.yystates[0]->yylrState] != 0)
	      return;
	  }
	else if (yytable[yyj] != 0 && ! yyis_table_ninf (yytable[yyj]))
	  return;
      }

  /* Reduce to one stack.  */
  for (yyk = 0; yyk < yystack->yytops.yysize; yyk += 1)
    if (yystack->yytops.yystates[yyk] != NULL)
      break;
  if (yyk >= yystack->yytops.yysize)
    yyFail (yystack, NULL);
  for (yyk += 1; yyk < yystack->yytops.yysize; yyk += 1)
    yymarkStackDeleted (yystack, yyk);
  yyremoveDeletes (yystack);
  yycompressStack (yystack);

  /* Now pop stack until we find a state that shifts the error token. */
  yystack->yyerrState = 3;
  while (yystack->yytops.yystates[0] != NULL)
    {
      yyGLRState *yys = yystack->yytops.yystates[0];
      yyj = yypact[yys->yylrState];
      if (! yyis_pact_ninf (yyj))
	{
	  yyj += YYTERROR;
	  if (0 <= yyj && yyj <= YYLAST && yycheck[yyj] == YYTERROR
	      && yyisShiftAction (yytable[yyj]))
	    {
	      /* Shift the error token having adjusted its location.  */
	      YYLTYPE yyerrloc;
	      YY_SYMBOL_PRINT ("Shifting", yystos[yytable[yyj]],
			       yylvalp, &yyerrloc);
	      yyglrShift (yystack, 0, yytable[yyj],
			  yys->yyposn, *yylvalp, &yyerrloc);
	      yys = yystack->yytops.yystates[0];
	      break;
	    }
	}

      yydestruct ("Error: popping",
		  yystos[yys->yylrState],
	          &yys->yysemantics.yysval);
      yystack->yytops.yystates[0] = yys->yypred;
      yystack->yynextFree -= 1;
      yystack->yyspaceLeft += 1;
    }
  if (yystack->yytops.yystates[0] == NULL)
    yyFail (yystack, NULL);
}

#define YYCHK1(YYE)							     \
  do {									     \
    switch (YYE) {							     \
    default:								     \
      break;								     \
    case yyabort:							     \
      yystack.yyerrflag = 1;						     \
      goto yyDone;							     \
    case yyaccept:							     \
      yystack.yyerrflag = 0;						     \
      goto yyDone;							     \
    case yyerr:								     \
      goto yyuser_error;						     \
    }									     \
  } while (0)


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
  yySymbol yytoken;
  yyGLRStack yystack;
  size_t yyposn;


  YYSTYPE* const yylvalp = &yylval;
  YYLTYPE* const yyllocp = &yylloc;

  yyinitGLRStack (&yystack, YYINITDEPTH);
  yystack.yytokenp = &yytoken;

  YYDPRINTF ((stderr, "Starting parse\n"));

  if (setjmp (yystack.yyexception_buffer) != 0)
    goto yyDone;

  yylval = yyval_default;


  yyglrShift (&yystack, 0, 0, 0, yylval, &yylloc);
  yytoken = YYEMPTY;
  yyposn = 0;

  while (yytrue)
    {
      /* For efficiency, we have two loops, the first of which is
	 specialized to deterministic operation (single stack, no
	 potential ambiguity).  */
      /* Standard mode */
      while (yytrue)
	{
	  yyRuleNum yyrule;
	  int yyaction;
	  const short int* yyconflicts;

	  yyStateNum yystate = yystack.yytops.yystates[0]->yylrState;
          YYDPRINTF ((stderr, "Entering state %d\n", yystate));
	  if (yystate == YYFINAL)
	    goto yyDone;
	  if (yyisDefaultedState (yystate))
	    {
	      yyrule = yydefaultAction (yystate);
	      if (yyrule == 0)
		{

		  yyreportSyntaxError (&yystack, yylvalp, yyllocp);
		  goto yyuser_error;
		}
	      YYCHK1 (yyglrReduce (&yystack, 0, yyrule, yytrue));
	    }
	  else
	    {
	      if (yytoken == YYEMPTY)
		{
		  YYDPRINTF ((stderr, "Reading a token: "));
		  yychar = YYLEX;
		  yytoken = YYTRANSLATE (yychar);
                  YY_SYMBOL_PRINT ("Next token is", yytoken, yylvalp, yyllocp);
		}
	      yygetLRActions (yystate, yytoken, &yyaction, &yyconflicts);
	      if (*yyconflicts != 0)
		break;
	      if (yyisShiftAction (yyaction))
		{
		  YY_SYMBOL_PRINT ("Shifting", yytoken, yylvalp, yyllocp);
		  if (yytoken != YYEOF)
		    yytoken = YYEMPTY;
		  yyposn += 1;
		  yyglrShift (&yystack, 0, yyaction, yyposn,
		              yylval, yyllocp);
		  if (0 < yystack.yyerrState)
		    yystack.yyerrState -= 1;
		}
	      else if (yyisErrorAction (yyaction))
		{

		  yyreportSyntaxError (&yystack, yylvalp, yyllocp);
		  goto yyuser_error;
		}
	      else
		YYCHK1 (yyglrReduce (&yystack, 0, -yyaction, yytrue));
	    }
	}

      while (yytrue)
	{
	  int yys;
	  int yyn = yystack.yytops.yysize;
	  for (yys = 0; yys < yyn; yys += 1)
	    YYCHK1 (yyprocessOneStack (&yystack, yys, yyposn,
				       yylvalp, yyllocp));
	  yytoken = YYEMPTY;
	  yyposn += 1;
	  yyremoveDeletes (&yystack);
	  if (yystack.yytops.yysize == 0)
	    {
	      yyundeleteLastStack (&yystack);
	      if (yystack.yytops.yysize == 0)
		yyFail (&yystack, "syntax error");
	      YYCHK1 (yyresolveStack (&yystack));
	      YYDPRINTF ((stderr, "Returning to deterministic operation.\n"));

	      yyreportSyntaxError (&yystack, yylvalp, yyllocp);
	      goto yyuser_error;
	    }
	  else if (yystack.yytops.yysize == 1)
	    {
	      YYCHK1 (yyresolveStack (&yystack));
	      YYDPRINTF ((stderr, "Returning to deterministic operation.\n"));
	      yycompressStack (&yystack);
	      break;
	    }
	}
      continue;
    yyuser_error:
      yyrecoverSyntaxError (&yystack, yylvalp, yyllocp);
      yyposn = yystack.yytops.yystates[0]->yyposn;
    }
 yyDone:
  /* On YYABORT, free the lookahead. */
  if (yystack.yyerrflag == 1 && yytoken != YYEMPTY)
    yydestruct ("Error: discarding lookahead",
                yytoken, yylvalp);

  yyfreeGLRStack (&yystack);
  return yystack.yyerrflag;
}

/* DEBUGGING ONLY */
#ifdef YYDEBUG
static void yypstack (yyGLRStack* yystack, int yyk) ATTRIBUTE_UNUSED;
static void yypdumpstack (yyGLRStack* yystack) ATTRIBUTE_UNUSED;

static void
yy_yypstack (yyGLRState* yys)
{
  if (yys->yypred)
    {
      yy_yypstack (yys->yypred);
      fprintf (stderr, " -> ");
    }
  fprintf (stderr, "%d@%lu", yys->yylrState, (unsigned long int) yys->yyposn);
}

static void
yypstates (yyGLRState* yyst)
{
  if (yyst == NULL)
    fprintf (stderr, "<null>");
  else
    yy_yypstack (yyst);
  fprintf (stderr, "\n");
}

static void
yypstack (yyGLRStack* yystack, int yyk)
{
  yypstates (yystack->yytops.yystates[yyk]);
}

#define YYINDEX(YYX)							     \
    ((YYX) == NULL ? -1 : (yyGLRStackItem*) (YYX) - yystack->yyitems)


static void
yypdumpstack (yyGLRStack* yystack)
{
  yyGLRStackItem* yyp;
  size_t yyi;
  for (yyp = yystack->yyitems; yyp < yystack->yynextFree; yyp += 1)
    {
      fprintf (stderr, "%3lu. ", (unsigned long int) (yyp - yystack->yyitems));
      if (*(yybool *) yyp)
	{
	  fprintf (stderr, "Res: %d, LR State: %d, posn: %lu, pred: %ld",
		   yyp->yystate.yyresolved, yyp->yystate.yylrState,
		   (unsigned long int) yyp->yystate.yyposn,
		   (long int) YYINDEX (yyp->yystate.yypred));
	  if (! yyp->yystate.yyresolved)
	    fprintf (stderr, ", firstVal: %ld",
		     (long int) YYINDEX (yyp->yystate.yysemantics.yyfirstVal));
	}
      else
	{
	  fprintf (stderr, "Option. rule: %d, state: %ld, next: %ld",
		   yyp->yyoption.yyrule,
		   (long int) YYINDEX (yyp->yyoption.yystate),
		   (long int) YYINDEX (yyp->yyoption.yynext));
	}
      fprintf (stderr, "\n");
    }
  fprintf (stderr, "Tops:");
  for (yyi = 0; yyi < yystack->yytops.yysize; yyi += 1)
    fprintf (stderr, "%lu: %ld; ", (unsigned long int) yyi,
	     (long int) YYINDEX (yystack->yytops.yystates[yyi]));
  fprintf (stderr, "\n");
}
#endif


#line 2487 "mcrl2parser.yy"
 

ATermAppl gsSpecEltsToSpec(ATermList SpecElts)
{
  ATermAppl Result = NULL;
  ATermList SortDecls = ATmakeList0();
  ATermList ConsDecls = ATmakeList0();
  ATermList MapDecls = ATmakeList0();
  ATermList DataEqnDecls = ATmakeList0();
  ATermList ActDecls = ATmakeList0();
  ATermList ProcEqnDecls = ATmakeList0();
  ATermAppl Init = NULL;
  int n = ATgetLength(SpecElts);
  for (int i = 0; i < n; i++) {
    ATermAppl SpecElt = ATAelementAt(SpecElts, i);
    if (gsIsProcessInit(SpecElt)) {
      if (Init == NULL) {
        Init = SpecElt;
      } else {
        //Init != NULL
        gsErrorMsg("parse error: multiple initialisations\n");
        return NULL;
      }
    } else {
      ATermList SpecEltArg0 = ATLgetArgument(SpecElt, 0);
      if (gsIsSortSpec(SpecElt)) {
        SortDecls = ATconcat(SortDecls, SpecEltArg0);
      } else if (gsIsConsSpec(SpecElt)) {
        ConsDecls = ATconcat(ConsDecls, SpecEltArg0);
      } else if (gsIsMapSpec(SpecElt)) {
        MapDecls = ATconcat(MapDecls, SpecEltArg0);
      } else if (gsIsDataEqnSpec(SpecElt)) {
        DataEqnDecls = ATconcat(DataEqnDecls, SpecEltArg0);
      } else if (gsIsActSpec(SpecElt)) {
        ActDecls = ATconcat(ActDecls, SpecEltArg0);
      } else if (gsIsProcEqnSpec(SpecElt)) {
        ProcEqnDecls = ATconcat(ProcEqnDecls, SpecEltArg0);
      }
    }
  }
  //check whether an initialisation is present
  if (Init == NULL) {
    gsErrorMsg("parse error: missing initialisation\n");
    return NULL;
  }
  Result = gsMakeSpecV1(
    gsMakeDataSpec(
      gsMakeSortSpec(SortDecls),
      gsMakeConsSpec(ConsDecls),
      gsMakeMapSpec(MapDecls),
      gsMakeDataEqnSpec(DataEqnDecls)
    ),
    gsMakeActSpec(ActDecls),
    gsMakeProcEqnSpec(ProcEqnDecls),
    Init
  );
  return Result;
}

ATermAppl gsActionRenameEltsToActionRename(ATermList ActionRenameElts)
{
  ATermAppl Result = NULL;
  ATermList SortDecls = ATmakeList0();
  ATermList ConsDecls = ATmakeList0();
  ATermList MapDecls = ATmakeList0();
  ATermList DataEqnDecls = ATmakeList0();
  ATermList ActDecls = ATmakeList0();
  ATermList ActionRenameRules = ATmakeList0();
  int n = ATgetLength(ActionRenameElts);
  for (int i = 0; i < n; i++) {
    ATermAppl ActionRenameElt = ATAelementAt(ActionRenameElts, i);
    ATermList ActionRenameEltArg0 = ATLgetArgument(ActionRenameElt, 0);
    if (gsIsSortSpec(ActionRenameElt)) {
      SortDecls = ATconcat(SortDecls, ActionRenameEltArg0);
    } else if (gsIsConsSpec(ActionRenameElt)) {
      ConsDecls = ATconcat(ConsDecls, ActionRenameEltArg0);
    } else if (gsIsMapSpec(ActionRenameElt)) {
      MapDecls = ATconcat(MapDecls, ActionRenameEltArg0);
    } else if (gsIsDataEqnSpec(ActionRenameElt)) {
      DataEqnDecls = ATconcat(DataEqnDecls, ActionRenameEltArg0);
    } else if (gsIsActSpec(ActionRenameElt)) {
      ActDecls = ATconcat(ActDecls, ActionRenameEltArg0);
    } else if (gsIsActionRenameRule(ActionRenameElt)) {
      ActionRenameRules = ATconcat(ActionRenameRules, ActionRenameEltArg0);
    }
  }

  Result = gsMakeActionRename(
    gsMakeDataSpec(
      gsMakeSortSpec(SortDecls),
      gsMakeConsSpec(ConsDecls),
      gsMakeMapSpec(MapDecls),
      gsMakeDataEqnSpec(DataEqnDecls)
    ),
    gsMakeActSpec(ActDecls),
    gsMakeActionRenameRules(ActionRenameRules)
  );
  return Result;
}

