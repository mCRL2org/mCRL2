
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison GLR parsers in C
   
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

/* C GLR parser skeleton written by Paul Hilfinger.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "glr.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0


/* Substitute the variable and function names.  */
#define yyparse mcrl3yyparse
#define yylex   mcrl3yylex
#define yyerror mcrl3yyerror
#define yylval  mcrl3yylval
#define yychar  mcrl3yychar
#define yydebug mcrl3yydebug
#define yynerrs mcrl3yynerrs
#define yylloc  mcrl3yylloc

/* Copy the first part of user declarations.  */

/* Line 172 of glr.c  */
#line 18 "mcrl2parser.yy"


#include <stdio.h>
#include <string.h>

#include <aterm2.h>
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/core/detail/lexer.h"
#include "mcrl2/data/standard_utility.h"

using namespace mcrl2::core;
using namespace mcrl2::core::detail;

//Global precondition: the ATerm library has been initialised

//external declarations from mcrl2lexer.ll
extern void mcrl3yyerror(const char *s);
extern int mcrl3yylex();
extern ATerm mcrl3_spec_tree;
extern ATermIndexedSet mcrl3_parser_protect_table;

#ifdef _MSC_VER
#define yyfalse 0
#define yytrue 1
#endif

#define YYMAXDEPTH 640000

#define safe_assign(lhs, rhs) { ATbool b; lhs = rhs; ATindexedSetPut(mcrl3_parser_protect_table, (ATerm) lhs, &b); }


/* Line 172 of glr.c  */
#line 101 "mcrl2parser.cpp"



#include "mcrl2/core/detail/mcrl2parser.h"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

/* Default (constant) value used for initialization for null
   right-hand sides.  Unlike the standard yacc.c template,
   here we set the default value of $$ to a zeroed-out value.
   Since the default value is undefined, this behavior is
   technically correct.  */
static YYSTYPE yyval_default;

/* Copy the second part of user declarations.  */


/* Line 243 of glr.c  */
#line 136 "mcrl2parser.cpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#ifndef YYFREE
# define YYFREE free
#endif
#ifndef YYMALLOC
# define YYMALLOC malloc
#endif
#ifndef YYREALLOC
# define YYREALLOC realloc
#endif

#define YYSIZEMAX ((size_t) -1)

#ifdef __cplusplus
   typedef bool yybool;
#else
   typedef unsigned char yybool;
#endif
#define yytrue 1
#define yyfalse 0

#ifndef YYSETJMP
# include <setjmp.h>
# define YYJMP_BUF jmp_buf
# define YYSETJMP(env) setjmp (env)
# define YYLONGJMP(env, val) longjmp (env, val)
#endif

/*-----------------.
| GCC extensions.  |
`-----------------*/

#ifndef __attribute__
/* This feature is available in gcc versions 2.5 and later.  */
# if (! defined __GNUC__ || __GNUC__ < 2 \
      || (__GNUC__ == 2 && __GNUC_MINOR__ < 5) || __STRICT_ANSI__)
#  define __attribute__(Spec) /* empty */
# endif
#endif


#ifdef __cplusplus
# define YYOPTIONAL_LOC(Name) /* empty */
#else
# define YYOPTIONAL_LOC(Name) Name __attribute__ ((__unused__))
#endif

#ifndef YYASSERT
# define YYASSERT(condition) ((void) ((condition) || (abort (), 0)))
#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  167
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1671

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  96
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  174
/* YYNRULES -- Number of rules.  */
#define YYNRULES  413
/* YYNRULES -- Number of states.  */
#define YYNSTATES  860
/* YYMAXRHS -- Maximum number of symbols on right-hand side of rule.  */
#define YYMAXRHS 7
/* YYMAXLEFT -- Maximum number of symbols to the left of a handle
   accessed by $0, $-1, etc., in any rule.  */
#define YYMAXLEFT 0

/* YYTRANSLATE(X) -- Bison symbol number corresponding to X.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   350

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

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
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short int yyprhs[] =
{
       0,     0,     3,     6,     9,    12,    15,    18,    21,    24,
      27,    30,    33,    36,    38,    42,    44,    46,    48,    50,
      54,    56,    59,    61,    63,    65,    69,    73,    74,    78,
      79,    82,    84,    88,    90,    93,    96,    98,   100,   102,
     104,   108,   110,   112,   114,   116,   118,   123,   128,   133,
     135,   137,   142,   144,   148,   152,   154,   159,   164,   169,
     171,   175,   179,   181,   185,   187,   192,   197,   199,   203,
     207,   209,   214,   219,   221,   225,   229,   231,   236,   241,
     246,   248,   252,   256,   260,   264,   268,   270,   274,   276,
     280,   282,   286,   288,   292,   296,   298,   302,   306,   310,
     312,   316,   320,   322,   325,   328,   331,   333,   338,   343,
     345,   350,   352,   356,   358,   360,   362,   364,   368,   370,
     372,   374,   376,   379,   382,   386,   390,   394,   396,   400,
     404,   410,   414,   416,   418,   421,   423,   425,   427,   429,
     432,   435,   439,   441,   445,   447,   451,   453,   457,   460,
     463,   466,   470,   474,   476,   479,   484,   487,   491,   495,
     501,   504,   508,   510,   512,   514,   518,   520,   525,   527,
     529,   533,   535,   540,   542,   546,   550,   552,   557,   559,
     563,   565,   570,   572,   576,   582,   584,   589,   591,   595,
     597,   601,   603,   608,   612,   618,   620,   625,   627,   631,
     633,   637,   639,   643,   645,   649,   651,   656,   660,   666,
     668,   673,   675,   677,   679,   681,   685,   687,   689,   693,
     698,   705,   712,   719,   726,   733,   736,   740,   743,   747,
     749,   753,   757,   760,   764,   766,   770,   772,   776,   780,
     784,   786,   790,   793,   797,   799,   803,   805,   807,   809,
     812,   814,   816,   818,   820,   822,   825,   828,   832,   834,
     838,   841,   844,   847,   851,   855,   862,   866,   868,   870,
     875,   880,   886,   892,   893,   897,   899,   903,   909,   911,
     915,   917,   922,   927,   933,   939,   941,   945,   949,   951,
     956,   961,   967,   973,   975,   978,   983,   988,   992,   996,
     998,  1003,  1008,  1014,  1020,  1025,  1027,  1029,  1031,  1033,
    1035,  1039,  1041,  1043,  1045,  1049,  1051,  1055,  1057,  1061,
    1063,  1067,  1069,  1072,  1075,  1077,  1080,  1083,  1085,  1089,
    1091,  1093,  1097,  1099,  1101,  1106,  1111,  1113,  1117,  1119,
    1124,  1129,  1131,  1135,  1139,  1141,  1146,  1151,  1153,  1157,
    1159,  1162,  1164,  1169,  1174,  1176,  1181,  1183,  1185,  1189,
    1191,  1193,  1196,  1198,  1200,  1202,  1204,  1207,  1212,  1215,
    1219,  1225,  1229,  1231,  1233,  1235,  1237,  1242,  1247,  1249,
    1253,  1255,  1260,  1265,  1267,  1271,  1275,  1277,  1282,  1287,
    1289,  1292,  1294,  1299,  1304,  1309,  1311,  1313,  1315,  1319,
    1321,  1323,  1326,  1328,  1330,  1332,  1334,  1337,  1340,  1344,
    1349,  1357,  1359,  1361
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const short int yyrhs[] =
{
      97,     0,    -1,     3,    94,    -1,     4,    98,    -1,     5,
     117,    -1,     6,   148,    -1,     7,   165,    -1,     8,   168,
      -1,     9,   203,    -1,    10,   214,    -1,    11,   245,    -1,
      12,   262,    -1,    13,   164,    -1,   100,    -1,   101,    15,
     100,    -1,   105,    -1,    99,    -1,   102,    -1,   103,    -1,
     102,    40,   103,    -1,   105,    -1,    60,   106,    -1,   114,
      -1,   104,    -1,   107,    -1,   106,    41,   107,    -1,    94,
     108,   109,    -1,    -1,    42,   110,    43,    -1,    -1,    37,
      94,    -1,   111,    -1,   110,    34,   111,    -1,    98,    -1,
     112,    98,    -1,    94,    35,    -1,    94,    -1,   113,    -1,
     115,    -1,   116,    -1,    42,    98,    43,    -1,    61,    -1,
      62,    -1,    63,    -1,    64,    -1,    65,    -1,    66,    42,
      98,    43,    -1,    67,    42,    98,    43,    -1,    68,    42,
      98,    43,    -1,   118,    -1,   121,    -1,   118,    78,   119,
      79,    -1,   120,    -1,   119,    34,   120,    -1,    94,    32,
     117,    -1,   124,    -1,    75,   122,    33,   121,    -1,    76,
     122,    33,   121,    -1,    77,   122,    33,   121,    -1,   123,
      -1,   122,    34,   123,    -1,   154,    35,    98,    -1,   126,
      -1,   126,    25,   125,    -1,   124,    -1,    76,   122,    33,
     125,    -1,    77,   122,    33,   125,    -1,   128,    -1,   128,
      23,   127,    -1,   128,    24,   127,    -1,   126,    -1,    76,
     122,    33,   127,    -1,    77,   122,    33,   127,    -1,   130,
      -1,   130,    21,   129,    -1,   130,    22,   129,    -1,   128,
      -1,    75,   122,    33,   129,    -1,    76,   122,    33,   129,
      -1,    77,   122,    33,   129,    -1,   131,    -1,   131,    17,
     131,    -1,   131,    16,   131,    -1,   131,    47,   131,    -1,
     131,    46,   131,    -1,   131,    74,   131,    -1,   132,    -1,
     134,    18,   131,    -1,   133,    -1,   132,    19,   134,    -1,
     134,    -1,   133,    20,   134,    -1,   135,    -1,   134,    30,
     135,    -1,   134,    31,   135,    -1,   136,    -1,   135,    72,
     136,    -1,   135,    73,   136,    -1,   135,    28,   136,    -1,
     137,    -1,   136,    29,   137,    -1,   136,    33,   137,    -1,
     139,    -1,    38,   138,    -1,    31,   137,    -1,    40,   137,
      -1,   137,    -1,    76,   122,    33,   138,    -1,    77,   122,
      33,   138,    -1,   141,    -1,   139,    42,   140,    43,    -1,
     117,    -1,   140,    34,   117,    -1,    94,    -1,   142,    -1,
     143,    -1,   146,    -1,    42,   117,    43,    -1,    69,    -1,
      70,    -1,    71,    -1,    95,    -1,    44,    45,    -1,    48,
      49,    -1,    44,   140,    45,    -1,    48,   140,    49,    -1,
      48,   144,    49,    -1,   145,    -1,   144,    34,   145,    -1,
     117,    35,   117,    -1,    48,   147,    41,   117,    49,    -1,
      94,    35,    98,    -1,   149,    -1,   150,    -1,   149,   150,
      -1,   151,    -1,   156,    -1,   157,    -1,   160,    -1,    50,
     152,    -1,   153,    36,    -1,   152,   153,    36,    -1,   154,
      -1,    94,    32,    98,    -1,    94,    -1,   154,    34,    94,
      -1,   101,    -1,   101,    15,    98,    -1,    51,   158,    -1,
      52,   158,    -1,   159,    36,    -1,   158,   159,    36,    -1,
     154,    35,    98,    -1,   161,    -1,    54,   162,    -1,    53,
     164,    54,   162,    -1,   163,    36,    -1,   162,   163,    36,
      -1,   117,    32,   117,    -1,   117,    15,   117,    32,   117,
      -1,   123,    36,    -1,   164,   123,    36,    -1,   166,    -1,
      81,    -1,   167,    -1,   166,    41,   167,    -1,    94,    -1,
      94,    42,   140,    43,    -1,   169,    -1,   170,    -1,   170,
      30,   169,    -1,   171,    -1,    82,   122,    33,   170,    -1,
     173,    -1,   173,    24,   172,    -1,   173,    14,   172,    -1,
     171,    -1,    82,   122,    33,   172,    -1,   175,    -1,   173,
      26,   174,    -1,   175,    -1,    82,   122,    33,   174,    -1,
     177,    -1,   137,    15,   176,    -1,   137,    15,   180,    27,
     176,    -1,   175,    -1,    82,   122,    33,   176,    -1,   181,
      -1,   181,    33,   179,    -1,   182,    -1,   181,    33,   180,
      -1,   177,    -1,    82,   122,    33,   179,    -1,   137,    15,
     179,    -1,   137,    15,   180,    27,   179,    -1,   178,    -1,
      82,   122,    33,   180,    -1,   183,    -1,   181,    39,   137,
      -1,   184,    -1,   182,    39,   137,    -1,   187,    -1,   187,
      41,   185,    -1,   187,    -1,   187,    41,   186,    -1,   183,
      -1,    82,   122,    33,   185,    -1,   137,    15,   185,    -1,
     137,    15,   186,    27,   185,    -1,   184,    -1,    82,   122,
      33,   186,    -1,   188,    -1,   167,    -1,   189,    -1,   190,
      -1,    42,   168,    43,    -1,    80,    -1,    81,    -1,    94,
      42,    43,    -1,    94,    42,   119,    43,    -1,    83,    42,
     191,    34,   168,    43,    -1,    85,    42,   191,    34,   168,
      43,    -1,    86,    42,   192,    34,   168,    43,    -1,    87,
      42,   195,    34,   168,    43,    -1,    84,    42,   200,    34,
     168,    43,    -1,    48,    49,    -1,    48,   154,    49,    -1,
      48,    49,    -1,    48,   193,    49,    -1,   194,    -1,   193,
      34,   194,    -1,    94,    15,    94,    -1,    48,    49,    -1,
      48,   196,    49,    -1,   197,    -1,   196,    34,   197,    -1,
     198,    -1,   198,    15,    81,    -1,   198,    15,    94,    -1,
      94,    41,   199,    -1,    94,    -1,   199,    41,    94,    -1,
      48,    49,    -1,    48,   201,    49,    -1,   202,    -1,   201,
      34,   202,    -1,   199,    -1,   204,    -1,   205,    -1,   204,
     205,    -1,   150,    -1,   206,    -1,   209,    -1,   210,    -1,
     213,    -1,    55,   207,    -1,   208,    36,    -1,   207,   208,
      36,    -1,   154,    -1,   154,    35,   155,    -1,    56,   164,
      -1,    57,   211,    -1,   212,    36,    -1,   211,   212,    36,
      -1,    94,    32,   168,    -1,    94,    42,   122,    43,    32,
     168,    -1,    59,   168,    36,    -1,   215,    -1,   219,    -1,
      76,   122,    33,   215,    -1,    77,   122,    33,   215,    -1,
      90,    94,   216,    33,   215,    -1,    89,    94,   216,    33,
     215,    -1,    -1,    42,   217,    43,    -1,   218,    -1,   217,
      34,   218,    -1,    94,    35,    98,    32,   117,    -1,   221,
      -1,   221,    25,   220,    -1,   219,    -1,    76,   122,    33,
     220,    -1,    77,   122,    33,   220,    -1,    90,    94,   216,
      33,   220,    -1,    89,    94,   216,    33,   220,    -1,   223,
      -1,   223,    23,   222,    -1,   223,    24,   222,    -1,   221,
      -1,    76,   122,    33,   222,    -1,    77,   122,    33,   222,
      -1,    90,    94,   216,    33,   222,    -1,    89,    94,   216,
      33,   222,    -1,   225,    -1,    38,   224,    -1,    44,   226,
      45,   224,    -1,    46,   226,    47,   224,    -1,    92,    39,
     137,    -1,    91,    39,   137,    -1,   223,    -1,    76,   122,
      33,   224,    -1,    77,   122,    33,   224,    -1,    90,    94,
     216,    33,   224,    -1,    89,    94,   216,    33,   224,    -1,
      88,    42,   117,    43,    -1,   167,    -1,    69,    -1,    70,
      -1,    92,    -1,    91,    -1,    42,   214,    43,    -1,   235,
      -1,   227,    -1,   229,    -1,   230,    30,   228,    -1,   230,
      -1,   230,    30,   228,    -1,   231,    -1,   232,    33,   230,
      -1,   232,    -1,   232,    33,   230,    -1,   233,    -1,   232,
      29,    -1,   232,    30,    -1,   234,    -1,   232,    29,    -1,
     232,    30,    -1,    93,    -1,    42,   227,    43,    -1,   235,
      -1,    93,    -1,    42,   227,    43,    -1,   236,    -1,   237,
      -1,    76,   122,    33,   236,    -1,    77,   122,    33,   236,
      -1,   239,    -1,   239,    25,   238,    -1,   237,    -1,    76,
     122,    33,   238,    -1,    77,   122,    33,   238,    -1,   241,
      -1,   242,    23,   240,    -1,   242,    24,   240,    -1,   239,
      -1,    76,   122,    33,   240,    -1,    77,   122,    33,   240,
      -1,   242,    -1,   241,    39,   137,    -1,   244,    -1,    38,
     243,    -1,   242,    -1,    76,   122,    33,   243,    -1,    77,
     122,    33,   243,    -1,   165,    -1,    88,    42,   117,    43,
      -1,    69,    -1,    70,    -1,    42,   235,    43,    -1,   246,
      -1,   247,    -1,   246,   247,    -1,   150,    -1,   206,    -1,
     248,    -1,   249,    -1,    86,   250,    -1,    53,   164,    86,
     250,    -1,   251,    36,    -1,   250,   251,    36,    -1,   117,
      15,   167,    25,   252,    -1,   167,    25,   252,    -1,   167,
      -1,   188,    -1,   254,    -1,   255,    -1,    76,   122,    33,
     254,    -1,    77,   122,    33,   254,    -1,   257,    -1,   257,
      25,   256,    -1,   255,    -1,    76,   122,    33,   256,    -1,
      77,   122,    33,   256,    -1,   259,    -1,   259,    23,   258,
      -1,   259,    24,   258,    -1,   257,    -1,    76,   122,    33,
     258,    -1,    77,   122,    33,   258,    -1,   261,    -1,    38,
     260,    -1,   259,    -1,    76,   122,    33,   260,    -1,    77,
     122,    33,   260,    -1,    88,    42,   117,    43,    -1,   167,
      -1,    69,    -1,    70,    -1,    42,   253,    43,    -1,   263,
      -1,   264,    -1,   263,   264,    -1,   150,    -1,   209,    -1,
     265,    -1,   269,    -1,    58,   266,    -1,   267,    36,    -1,
     266,   267,    36,    -1,   268,    94,    32,   253,    -1,   268,
      94,    42,   122,    43,    32,   253,    -1,    89,    -1,    90,
      -1,    59,   167,    36,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,   360,   360,   365,   370,   375,   380,   385,   390,   395,
     400,   405,   410,   422,   430,   438,   442,   447,   455,   459,
     467,   475,   483,   487,   495,   499,   508,   516,   519,   528,
     531,   539,   543,   551,   555,   561,   569,   577,   581,   585,
     589,   597,   601,   605,   609,   613,   621,   625,   629,   640,
     648,   652,   660,   664,   672,   680,   684,   688,   692,   701,
     705,   713,   725,   729,   738,   742,   746,   754,   758,   763,
     772,   776,   780,   788,   792,   797,   806,   810,   814,   818,
     826,   830,   835,   840,   845,   850,   859,   863,   872,   876,
     885,   889,   898,   902,   907,   916,   920,   925,   930,   939,
     943,   948,   957,   961,   965,   969,   977,   981,   985,   993,
     997,  1010,  1014,  1022,  1026,  1030,  1034,  1038,  1046,  1050,
    1054,  1058,  1062,  1066,  1074,  1078,  1082,  1090,  1094,  1102,
    1110,  1118,  1129,  1137,  1141,  1149,  1153,  1157,  1161,  1169,
    1177,  1181,  1189,  1197,  1205,  1209,  1217,  1221,  1229,  1237,
    1246,  1250,  1258,  1270,  1278,  1282,  1297,  1301,  1309,  1313,
    1322,  1326,  1337,  1341,  1349,  1353,  1361,  1365,  1376,  1384,
    1388,  1396,  1400,  1408,  1412,  1416,  1424,  1428,  1436,  1440,
    1448,  1452,  1460,  1464,  1468,  1476,  1480,  1488,  1492,  1500,
    1504,  1512,  1516,  1520,  1524,  1532,  1536,  1544,  1548,  1556,
    1560,  1568,  1572,  1580,  1584,  1592,  1596,  1600,  1604,  1612,
    1616,  1624,  1628,  1632,  1638,  1642,  1650,  1654,  1662,  1666,
    1674,  1678,  1682,  1686,  1690,  1698,  1702,  1710,  1714,  1722,
    1726,  1734,  1742,  1746,  1754,  1758,  1766,  1770,  1774,  1782,
    1790,  1794,  1802,  1806,  1814,  1818,  1826,  1837,  1845,  1849,
    1857,  1861,  1865,  1869,  1873,  1881,  1889,  1893,  1901,  1910,
    1922,  1930,  1938,  1942,  1950,  1955,  1969,  1980,  1988,  1992,
    1996,  2000,  2004,  2013,  2016,  2025,  2029,  2037,  2045,  2049,
    2057,  2061,  2065,  2069,  2073,  2081,  2085,  2089,  2097,  2101,
    2105,  2109,  2113,  2121,  2125,  2129,  2133,  2137,  2141,  2149,
    2153,  2157,  2161,  2165,  2173,  2177,  2181,  2185,  2189,  2193,
    2197,  2205,  2209,  2217,  2221,  2229,  2233,  2241,  2245,  2253,
    2257,  2265,  2269,  2273,  2281,  2285,  2289,  2297,  2301,  2309,
    2313,  2317,  2325,  2333,  2337,  2341,  2349,  2353,  2361,  2365,
    2369,  2377,  2381,  2385,  2393,  2397,  2401,  2409,  2413,  2421,
    2425,  2433,  2437,  2441,  2449,  2453,  2457,  2461,  2465,  2476,
    2484,  2488,  2496,  2500,  2504,  2512,  2520,  2524,  2541,  2545,
    2553,  2557,  2565,  2569,  2580,  2588,  2592,  2596,  2604,  2608,
    2616,  2620,  2624,  2632,  2636,  2640,  2648,  2652,  2656,  2664,
    2668,  2676,  2680,  2684,  2692,  2696,  2700,  2704,  2708,  2720,
    2728,  2732,  2740,  2744,  2748,  2752,  2760,  2768,  2772,  2780,
    2785,  2794,  2798,  2806
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "TAG_IDENTIFIER", "TAG_SORT_EXPR",
  "TAG_DATA_EXPR", "TAG_DATA_SPEC", "TAG_MULT_ACT", "TAG_PROC_EXPR",
  "TAG_PROC_SPEC", "TAG_STATE_FRM", "TAG_ACTION_RENAME", "TAG_PBES_SPEC",
  "TAG_DATA_VARS", "LMERGE", "ARROW", "LTE", "GTE", "CONS", "SNOC",
  "CONCAT", "EQ", "NEQ", "AND", "BARS", "IMP", "BINIT", "ELSE", "SLASH",
  "STAR", "PLUS", "MINUS", "EQUALS", "DOT", "COMMA", "COLON", "SEMICOLON",
  "QMARK", "EXCLAM", "AT", "HASH", "BAR", "LPAR", "RPAR", "LBRACK",
  "RBRACK", "LANG", "RANG", "LBRACE", "RBRACE", "KWSORT", "KWCONS",
  "KWMAP", "KWVAR", "KWEQN", "KWACT", "KWGLOB", "KWPROC", "KWPBES",
  "KWINIT", "KWSTRUCT", "BOOL", "POS", "NAT", "INT", "REAL", "LIST", "SET",
  "BAG", "CTRUE", "CFALSE", "IF", "DIV", "MOD", "IN", "LAMBDA", "FORALL",
  "EXISTS", "WHR", "END", "DELTA", "TAU", "SUM", "BLOCK", "ALLOW", "HIDE",
  "RENAME", "COMM", "VAL", "MU", "NU", "DELAY", "YALED", "NIL", "ID",
  "NUMBER", "$accept", "start", "SortExpr", "SortArrow", "sort_expr_arrow",
  "domain_no_arrow", "domain_no_arrow_elts_hs", "domain_no_arrow_elt",
  "SortStruct", "sort_expr_struct", "StructCons_list_bar_separated",
  "StructCons", "StructProj_list", "recogniser",
  "StructProj_list_comma_separated", "StructProj", "StructProjLabel",
  "SortId", "sort_expr_primary", "sort_constant", "sort_constructor",
  "data_expr", "data_expr_whr", "id_inits_cs", "id_init",
  "data_expr_quant", "data_vars_decls_cs", "data_vars_decl",
  "data_expr_imp", "data_expr_imp_rhs", "data_expr_and",
  "data_expr_and_rhs", "data_expr_eq", "data_expr_eq_rhs", "data_expr_rel",
  "data_expr_cons", "data_expr_snoc", "data_expr_concat", "data_expr_add",
  "data_expr_div", "data_expr_mult", "data_expr_prefix",
  "data_expr_quant_prefix", "data_expr_postfix", "data_exprs_cs",
  "data_expr_primary", "data_constant", "data_enumeration",
  "bag_enum_elts_cs", "bag_enum_elt", "data_comprehension",
  "data_var_decl", "data_spec", "data_spec_elts", "data_spec_elt",
  "sort_spec", "sorts_decls_scs", "sorts_decl", "ids_cs", "domain",
  "cons_spec", "map_spec", "ops_decls_scs", "ops_decl", "data_eqn_spec",
  "data_eqn_sect", "data_eqn_decls_scs", "data_eqn_decl",
  "data_vars_decls_scs", "mult_act", "param_ids_bs", "param_id",
  "proc_expr", "proc_expr_choice", "proc_expr_sum", "proc_expr_merge",
  "proc_expr_merge_rhs", "proc_expr_binit", "proc_expr_binit_rhs",
  "proc_expr_cond", "proc_expr_cond_la", "proc_expr_seq",
  "proc_expr_seq_wo_cond", "proc_expr_seq_rhs",
  "proc_expr_seq_rhs_wo_cond", "proc_expr_at", "proc_expr_at_wo_cond",
  "proc_expr_sync", "proc_expr_sync_wo_cond", "proc_expr_sync_rhs",
  "proc_expr_sync_rhs_wo_cond", "proc_expr_primary", "proc_constant",
  "id_assignment", "proc_quant", "act_names_set", "ren_expr_set",
  "ren_exprs_cs", "ren_expr", "comm_expr_set", "comm_exprs_cs",
  "comm_expr", "comm_expr_lhs", "ids_bs", "mult_act_names_set",
  "mult_act_names_cs", "mult_act_name", "proc_spec", "proc_spec_elts",
  "proc_spec_elt", "act_spec", "acts_decls_scs", "acts_decl",
  "glob_var_spec", "proc_eqn_spec", "proc_eqn_decls_scs", "proc_eqn_decl",
  "proc_init", "state_frm", "state_frm_quant", "fixpoint_params",
  "data_var_decl_inits_cs", "data_var_decl_init", "state_frm_imp",
  "state_frm_imp_rhs", "state_frm_and", "state_frm_and_rhs",
  "state_frm_prefix", "state_frm_quant_prefix", "state_frm_primary",
  "reg_frm", "reg_frm_alt_naf", "reg_frm_alt", "reg_frm_seq_naf",
  "reg_frm_seq", "reg_frm_postfix_naf", "reg_frm_postfix",
  "reg_frm_primary_naf", "reg_frm_primary", "act_frm", "act_frm_quant",
  "act_frm_imp", "act_frm_imp_rhs", "act_frm_and", "act_frm_and_rhs",
  "act_frm_at", "act_frm_prefix", "act_frm_quant_prefix",
  "act_frm_primary", "action_rename_spec", "action_rename_spec_elts",
  "action_rename_spec_elt", "action_rename_rule_spec",
  "action_rename_rule_sect", "action_rename_rules_scs",
  "action_rename_rule", "action_rename_rule_rhs", "pb_expr",
  "pb_expr_quant", "pb_expr_imp", "pb_expr_imp_rhs", "pb_expr_and",
  "pb_expr_and_rhs", "pb_expr_not", "pb_expr_quant_not", "pb_expr_primary",
  "pbes_spec", "pbes_spec_elts", "pbes_spec_elt", "pb_eqn_spec",
  "pb_eqn_decls_scs", "pb_eqn_decl", "fixpoint", "pb_init", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned short int yyr1[] =
{
       0,    96,    97,    97,    97,    97,    97,    97,    97,    97,
      97,    97,    97,    98,    99,   100,   100,   101,   102,   102,
     103,   104,   105,   105,   106,   106,   107,   108,   108,   109,
     109,   110,   110,   111,   111,   112,   113,   114,   114,   114,
     114,   115,   115,   115,   115,   115,   116,   116,   116,   117,
     118,   118,   119,   119,   120,   121,   121,   121,   121,   122,
     122,   123,   124,   124,   125,   125,   125,   126,   126,   126,
     127,   127,   127,   128,   128,   128,   129,   129,   129,   129,
     130,   130,   130,   130,   130,   130,   131,   131,   132,   132,
     133,   133,   134,   134,   134,   135,   135,   135,   135,   136,
     136,   136,   137,   137,   137,   137,   138,   138,   138,   139,
     139,   140,   140,   141,   141,   141,   141,   141,   142,   142,
     142,   142,   142,   142,   143,   143,   143,   144,   144,   145,
     146,   147,   148,   149,   149,   150,   150,   150,   150,   151,
     152,   152,   153,   153,   154,   154,   155,   155,   156,   157,
     158,   158,   159,   160,   161,   161,   162,   162,   163,   163,
     164,   164,   165,   165,   166,   166,   167,   167,   168,   169,
     169,   170,   170,   171,   171,   171,   172,   172,   173,   173,
     174,   174,   175,   175,   175,   176,   176,   177,   177,   178,
     178,   179,   179,   179,   179,   180,   180,   181,   181,   182,
     182,   183,   183,   184,   184,   185,   185,   185,   185,   186,
     186,   187,   187,   187,   187,   187,   188,   188,   189,   189,
     190,   190,   190,   190,   190,   191,   191,   192,   192,   193,
     193,   194,   195,   195,   196,   196,   197,   197,   197,   198,
     199,   199,   200,   200,   201,   201,   202,   203,   204,   204,
     205,   205,   205,   205,   205,   206,   207,   207,   208,   208,
     209,   210,   211,   211,   212,   212,   213,   214,   215,   215,
     215,   215,   215,   216,   216,   217,   217,   218,   219,   219,
     220,   220,   220,   220,   220,   221,   221,   221,   222,   222,
     222,   222,   222,   223,   223,   223,   223,   223,   223,   224,
     224,   224,   224,   224,   225,   225,   225,   225,   225,   225,
     225,   226,   226,   227,   227,   228,   228,   229,   229,   230,
     230,   231,   231,   231,   232,   232,   232,   233,   233,   234,
     234,   234,   235,   236,   236,   236,   237,   237,   238,   238,
     238,   239,   239,   239,   240,   240,   240,   241,   241,   242,
     242,   243,   243,   243,   244,   244,   244,   244,   244,   245,
     246,   246,   247,   247,   247,   248,   249,   249,   250,   250,
     251,   251,   252,   252,   253,   254,   254,   254,   255,   255,
     256,   256,   256,   257,   257,   257,   258,   258,   258,   259,
     259,   260,   260,   260,   261,   261,   261,   261,   261,   262,
     263,   263,   264,   264,   264,   264,   265,   266,   266,   267,
     267,   268,   268,   269
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     1,     3,     1,     1,     1,     1,     3,
       1,     2,     1,     1,     1,     3,     3,     0,     3,     0,
       2,     1,     3,     1,     2,     2,     1,     1,     1,     1,
       3,     1,     1,     1,     1,     1,     4,     4,     4,     1,
       1,     4,     1,     3,     3,     1,     4,     4,     4,     1,
       3,     3,     1,     3,     1,     4,     4,     1,     3,     3,
       1,     4,     4,     1,     3,     3,     1,     4,     4,     4,
       1,     3,     3,     3,     3,     3,     1,     3,     1,     3,
       1,     3,     1,     3,     3,     1,     3,     3,     3,     1,
       3,     3,     1,     2,     2,     2,     1,     4,     4,     1,
       4,     1,     3,     1,     1,     1,     1,     3,     1,     1,
       1,     1,     2,     2,     3,     3,     3,     1,     3,     3,
       5,     3,     1,     1,     2,     1,     1,     1,     1,     2,
       2,     3,     1,     3,     1,     3,     1,     3,     2,     2,
       2,     3,     3,     1,     2,     4,     2,     3,     3,     5,
       2,     3,     1,     1,     1,     3,     1,     4,     1,     1,
       3,     1,     4,     1,     3,     3,     1,     4,     1,     3,
       1,     4,     1,     3,     5,     1,     4,     1,     3,     1,
       3,     1,     4,     3,     5,     1,     4,     1,     3,     1,
       3,     1,     3,     1,     3,     1,     4,     3,     5,     1,
       4,     1,     1,     1,     1,     3,     1,     1,     3,     4,
       6,     6,     6,     6,     6,     2,     3,     2,     3,     1,
       3,     3,     2,     3,     1,     3,     1,     3,     3,     3,
       1,     3,     2,     3,     1,     3,     1,     1,     1,     2,
       1,     1,     1,     1,     1,     2,     2,     3,     1,     3,
       2,     2,     2,     3,     3,     6,     3,     1,     1,     4,
       4,     5,     5,     0,     3,     1,     3,     5,     1,     3,
       1,     4,     4,     5,     5,     1,     3,     3,     1,     4,
       4,     5,     5,     1,     2,     4,     4,     3,     3,     1,
       4,     4,     5,     5,     4,     1,     1,     1,     1,     1,
       3,     1,     1,     1,     3,     1,     3,     1,     3,     1,
       3,     1,     2,     2,     1,     2,     2,     1,     3,     1,
       1,     3,     1,     1,     4,     4,     1,     3,     1,     4,
       4,     1,     3,     3,     1,     4,     4,     1,     3,     1,
       2,     1,     4,     4,     1,     4,     1,     1,     3,     1,
       1,     2,     1,     1,     1,     1,     2,     4,     2,     3,
       5,     3,     1,     1,     1,     1,     4,     4,     1,     3,
       1,     4,     4,     1,     3,     3,     1,     4,     4,     1,
       2,     1,     4,     4,     4,     1,     1,     1,     3,     1,
       1,     2,     1,     1,     1,     1,     2,     2,     3,     4,
       7,     1,     1,     3
};

/* YYDPREC[RULE-NUM] -- Dynamic precedence of rule #RULE-NUM (0 if none).  */
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
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0
};

/* YYMERGER[RULE-NUM] -- Index of merging function for rule #RULE-NUM.  */
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
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error.  */
static const unsigned short int yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     2,     0,     0,    41,    42,    43,    44,
      45,     0,     0,     0,    36,     3,    16,    13,     0,    17,
      18,    23,    15,    37,    22,    38,    39,     0,     0,     0,
       0,     0,     0,   118,   119,   120,     0,     0,     0,   113,
     121,     4,    49,    50,    55,    62,    67,    73,    80,    86,
      88,    90,    92,    95,    99,   102,   109,   114,   115,   116,
       0,     0,     0,     0,     0,     5,   132,   133,   135,   136,
     137,   138,   153,   163,   166,     6,   162,   164,     0,   216,
     217,     0,     0,     0,     0,     0,     0,   113,     0,   212,
       7,   168,   169,   171,   173,   178,   182,   187,   197,   201,
     211,   213,   214,     0,     0,     0,     0,   250,     8,   247,
     248,   251,   252,   253,   254,     0,     0,     0,     0,   306,
     307,     0,     0,     0,     0,     0,   309,   308,   305,     9,
     267,   268,   278,   285,   293,     0,     0,   362,   363,    10,
     359,   360,   364,   365,     0,     0,   402,   403,    11,   399,
     400,   404,   405,   144,     0,     0,    12,     1,     0,    27,
      21,    24,     0,     0,     0,     0,     0,   104,     0,     0,
     106,   103,   105,     0,   122,   111,     0,   123,   113,   111,
       0,     0,   127,     0,     0,    59,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     144,   139,     0,   142,     0,   148,     0,   149,     0,     0,
     154,     0,   134,     0,     0,    99,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   258,   255,     0,   260,     0,   261,     0,     0,
     249,     0,     0,     0,     0,   299,   294,     0,     0,     0,
     356,   357,     0,     0,     0,   327,   354,     0,   312,   313,
       0,   317,     0,   321,   324,   329,   332,   333,   336,   341,
     347,   349,     0,     0,     0,     0,   273,   273,     0,     0,
       0,     0,     0,     0,   113,     0,     0,   366,     0,   361,
     411,   412,   406,     0,     0,     0,   401,   160,     0,     0,
       0,    40,     0,    29,     0,     0,     0,     0,    14,    19,
      20,     0,     0,   117,     0,   124,     0,     0,   125,     0,
     126,     0,     0,     0,     0,     0,     0,     0,    52,     0,
       0,    64,    63,     0,     0,    70,    68,    69,     0,     0,
       0,    76,    74,    75,    82,    81,    84,    83,    85,    89,
      91,    87,    93,    94,    98,    96,    97,   100,   101,     0,
       0,     0,   140,     0,     0,   150,     0,     0,     0,     0,
     156,     0,   165,   215,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   218,   113,     0,     0,   185,   183,
     195,     0,   187,   189,   199,   201,   170,     0,   176,   175,
     174,     0,   179,   180,     0,     0,   191,   188,   198,     0,
       0,   205,   202,     0,     0,   256,     0,     0,     0,   262,
     266,     0,     0,   273,   273,   310,     0,     0,     0,   351,
     350,     0,   329,     0,     0,     0,     0,     0,   322,   323,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   298,   297,     0,     0,     0,     0,   280,   279,
       0,     0,     0,     0,   288,   286,   287,     0,     0,     0,
       0,   368,     0,   407,     0,   413,   145,    61,   161,    36,
      33,     0,    31,     0,     0,    26,    25,    46,    47,    48,
       0,     0,   112,   131,   129,     0,   128,     0,    56,    60,
      57,    58,     0,     0,    51,     0,     0,     0,     0,     0,
       0,     0,   110,   143,   141,   152,   151,   155,     0,   158,
     157,   167,   172,   225,     0,     0,   242,   240,   246,     0,
     244,     0,     0,   227,     0,     0,   229,     0,   232,     0,
       0,   234,   236,     0,   219,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   146,   259,   257,   264,
       0,   263,     0,     0,     0,     0,     0,     0,     0,   328,
     358,     0,     0,     0,   295,     0,   330,   314,   315,   319,
     329,   318,     0,     0,   338,   337,   348,     0,     0,   344,
     342,   343,   296,   269,   270,   304,     0,     0,   275,     0,
       0,     0,     0,   273,   273,     0,     0,   273,   273,   367,
       0,   372,   373,   371,   369,   408,     0,     0,    35,     0,
      28,    34,    30,   107,   108,   130,    54,    53,     0,     0,
       0,     0,     0,     0,     0,     0,   226,     0,     0,     0,
     243,     0,     0,     0,     0,   228,     0,     0,     0,   233,
       0,     0,     0,     0,   184,     0,   190,   200,     0,   209,
     204,     0,     0,     0,   193,     0,     0,   207,     0,     0,
       0,   300,   301,     0,     0,     0,     0,   334,   335,   355,
       0,     0,   325,   326,     0,     0,     0,     0,     0,     0,
       0,   274,   272,   271,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   396,   397,     0,     0,     0,
     395,   409,   374,   375,   378,   383,   389,     0,    32,    65,
      66,    71,    72,    77,    78,    79,   159,   220,   241,   245,
     224,   221,   231,   230,   222,   239,   235,   237,   238,   223,
     186,   196,     0,     0,     0,   177,   181,   192,     0,   206,
       0,   147,     0,   303,   302,   352,   353,   331,   316,   320,
       0,     0,     0,     0,     0,   276,   281,   282,     0,     0,
     289,   290,     0,     0,   370,     0,     0,   391,   390,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     194,   208,   265,   339,   340,   345,   346,     0,   284,   283,
     292,   291,     0,     0,   398,     0,     0,     0,     0,     0,
     380,   379,     0,     0,   386,   384,   385,     0,   210,   277,
       0,     0,   376,   377,   394,     0,     0,     0,     0,   410,
     392,   393,     0,     0,     0,     0,   381,   382,   387,   388
};

/* YYPDEFGOTO[NTERM-NUM].  */
static const short int yydefgoto[] =
{
      -1,    12,   500,    26,    27,    28,    29,    30,    31,    32,
     170,   171,   323,   505,   501,   502,   503,    33,    34,    35,
      36,   185,    52,   347,   348,    53,   194,   195,    54,   352,
      55,   356,    56,   362,    57,    58,    59,    60,    61,    62,
      63,    64,   181,    65,   391,    66,    67,    68,   191,   192,
      69,   193,    75,    76,   117,    78,   221,   222,   165,   577,
      79,    80,   225,   226,    81,    82,   230,   231,   166,   276,
      86,    99,   100,   101,   102,   103,   419,   104,   422,   105,
     760,   106,   410,   427,   761,   107,   413,   108,   414,   432,
     680,   109,   110,   111,   112,   396,   401,   555,   556,   403,
     560,   561,   562,   548,   398,   549,   550,   118,   119,   120,
     121,   253,   254,   122,   123,   257,   258,   124,   139,   140,
     470,   617,   618,   141,   479,   142,   485,   143,   266,   144,
     277,   278,   597,   279,   280,   281,   282,   283,   284,   600,
     286,   287,   605,   288,   610,   289,   290,   450,   291,   149,
     150,   151,   152,   153,   307,   308,   633,   731,   732,   733,
     831,   734,   835,   735,   798,   736,   158,   159,   160,   161,
     312,   313,   314,   162
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -714
static const short int yypact[] =
{
     664,   -14,    37,  1333,   330,    74,   489,   566,  1461,   198,
     463,    -6,   106,  -714,    37,    79,  -714,  -714,  -714,  -714,
    -714,   151,   232,   252,  -714,  -714,  -714,  -714,   219,   270,
    -714,  -714,    71,  -714,  -714,  -714,  -714,    43,  1406,    43,
    1333,  1260,  1272,  -714,  -714,  -714,    -6,    -6,    -6,  -714,
    -714,  -714,   238,  -714,  -714,   293,   119,   292,    61,   303,
     305,   229,   108,   202,  -714,   285,  -714,  -714,  -714,  -714,
     250,    -6,    -6,    -6,  1333,  -714,   330,  -714,  -714,  -714,
    -714,  -714,  -714,  -714,   308,  -714,   322,  -714,   838,  -714,
    -714,    -6,   323,   325,   335,   345,   361,  1211,   400,  -714,
    -714,  -714,   396,  -714,   216,  -714,  -714,    77,  -714,   407,
    -714,  -714,  -714,    -6,    -6,   356,   489,  -714,  -714,   566,
    -714,  -714,  -714,  -714,  -714,  1490,  1461,   132,   132,  -714,
    -714,    -6,    -6,   412,   363,   370,   439,   455,  -714,  -714,
    -714,  -714,   471,   316,  -714,    -6,  1345,  -714,  -714,  -714,
     198,  -714,  -714,  -714,   302,   416,  -714,  -714,  -714,   463,
    -714,  -714,  -714,  -714,   482,   403,    -6,  -714,   483,   486,
     491,  -714,    37,    37,    37,    37,    37,  -714,    -6,    -6,
    -714,  -714,  -714,   501,  -714,  -714,   231,  -714,   513,   531,
      85,   142,  -714,   522,   453,  -714,   466,   474,   485,  1418,
    1421,  1421,  1348,  1348,    43,    43,    43,    43,    43,    43,
      43,    43,    43,    43,    43,    43,    43,    43,    43,  1333,
     536,   250,   546,   551,   504,    -6,   558,    -6,    55,    60,
    1333,   594,  -714,  1333,   416,   400,   569,   507,   601,   603,
     601,   613,   616,  1275,   710,   489,   909,   909,   928,   993,
      43,  1012,   508,    -6,   630,    -6,   235,   356,   668,   672,
    -714,    -6,    -6,   618,   644,  -714,  -714,   681,    90,   132,
    -714,  -714,    -6,    -6,   705,   360,  -714,   704,  -714,  -714,
     723,  -714,   387,  -714,  -714,   192,  -714,  -714,   749,   743,
     523,  -714,   742,   517,   520,  1333,   759,   759,    43,    43,
    1519,  1548,  1548,   138,   144,   788,   784,  1345,   783,  -714,
    -714,  -714,   302,   801,   750,   810,  -714,  -714,   757,    37,
     820,  -714,   346,   826,    79,   824,   825,   827,  -714,  -714,
    -714,   528,   555,  -714,  1333,  -714,    37,  1333,  -714,  1333,
    -714,  1333,  1333,    -6,  1333,  1333,   840,    50,  -714,    -6,
      -6,  -714,  -714,    -6,    -6,  -714,  -714,  -714,    -6,    -6,
      -6,  -714,  -714,  -714,  -714,  -714,  -714,  -714,  -714,   526,
     526,  -714,   108,   108,   202,   202,   202,  -714,  -714,   256,
      37,   837,  -714,    37,   839,  -714,  1333,  1333,  1333,   841,
    -714,   257,  -714,  -714,   489,    69,   845,    78,   849,   850,
     103,   853,   104,   855,  -714,   840,   258,    -6,  -714,  -714,
    -714,   847,   254,   851,  -714,   178,  -714,    -6,  -714,  -714,
    -714,    -6,  -714,  -714,    -6,   882,  -714,  -714,  -714,    -6,
     885,  -714,  -714,    37,   866,  -714,   489,    -6,   867,  -714,
    -714,   559,   570,   759,   759,  -714,   571,    -6,    -6,  -714,
    -714,   861,   862,   572,   574,  1333,  1490,   185,   392,   401,
     185,   663,    43,   764,   764,  1490,  1461,  1461,   863,   823,
     893,   894,  -714,  -714,    -6,    -6,   834,   835,  -714,  -714,
      -6,    -6,   836,   842,  -714,  -714,  -714,  1345,   416,   176,
     895,  -714,   898,  -714,   240,  -714,  -714,  -714,  -714,   900,
    -714,   264,  -714,    37,   843,  -714,  -714,  -714,  -714,  -714,
    1406,  1406,  -714,  -714,  -714,   531,  -714,   892,  -714,  -714,
    -714,  -714,  1333,   485,  -714,   577,   593,   595,   600,   602,
     612,   624,  -714,  -714,  -714,  -714,  -714,  1333,   910,  -714,
    -714,  -714,  -714,  -714,   165,   489,  -714,  -714,   902,   166,
    -714,   489,   489,  -714,   931,   184,  -714,   489,  -714,   907,
     194,  -714,   935,   489,  -714,   645,  1074,  1132,    43,  1192,
     647,   649,   651,  1132,   653,  1192,   937,  -714,  -714,  -714,
     274,  -714,  1490,  1490,   921,   922,   862,   659,   661,   414,
    -714,   571,   571,   913,  -714,   132,  -714,  -714,   930,   476,
    -714,   932,    -6,    -6,  -714,  -714,  -714,    -6,    -6,  -714,
    -714,  -714,  -714,  -714,  -714,  -714,   923,   277,  -714,  1461,
    1461,   669,   673,   759,   759,   682,   687,   759,   759,  1345,
     936,  -714,  -714,  -714,  -714,  -714,   822,    -6,  -714,   346,
    -714,  -714,  -714,  -714,  -714,  -714,  -714,  -714,  1418,  1418,
    1421,  1421,  1348,  1348,  1348,  1333,  -714,   920,   870,   871,
    -714,   924,   926,   877,   879,  -714,   934,   871,   880,  -714,
     164,   938,   710,    -6,  -714,    -6,  -714,  -714,    -6,  -714,
    -714,   909,   928,   993,  -714,   948,  1012,  -714,   955,    37,
     951,  -714,  -714,  1490,  1490,    90,    90,  -714,  -714,  -714,
     941,   185,  -714,  -714,   185,   689,   694,   696,   701,    37,
     823,  -714,  -714,  -714,  1519,  1519,   952,   953,  1548,  1548,
     954,   967,   176,  1213,   822,  -714,  -714,    -6,    -6,   946,
    -714,  -714,  -714,  -714,   976,   576,  -714,   278,  -714,  -714,
    -714,  -714,  -714,  -714,  -714,  -714,  -714,  -714,  -714,  -714,
    -714,  -714,  -714,  -714,  -714,   902,  -714,  -714,  -714,  -714,
    -714,  -714,   709,   712,   722,  -714,  -714,  -714,   993,  -714,
    1012,  -714,   489,  -714,  -714,  -714,  -714,  -714,  -714,  -714,
     663,   663,   764,   764,   970,  -714,  -714,  -714,  1519,  1519,
    -714,  -714,  1548,  1548,  -714,    -6,    -6,  -714,  -714,   962,
     727,   729,  1333,  1574,  1577,  1577,   974,  1074,  1132,  1192,
    -714,  -714,  -714,  -714,  -714,  -714,  -714,  1333,  -714,  -714,
    -714,  -714,   731,   733,  -714,   822,   822,   964,    -6,    -6,
    -714,  -714,    -6,    -6,  -714,  -714,  -714,   822,  -714,  -714,
    1213,  1213,  -714,  -714,  -714,   738,   752,   774,   778,  -714,
    -714,  -714,  1574,  1574,  1577,  1577,  -714,  -714,  -714,  -714
};

/* YYPGOTO[NTERM-NUM].  */
static const short int yypgoto[] =
{
    -714,  -714,    22,  -714,   844,   583,  -714,   852,  -714,  -153,
    -714,   693,  -714,  -714,  -714,   379,  -714,  -714,  -714,  -714,
    -714,     8,  -714,   777,   498,    91,   464,    16,  -182,    49,
    -179,  -147,  -164,  -178,  -714,   193,  -714,  -714,   606,   614,
     320,    87,   318,  -714,    17,  -714,  -714,  -714,  -714,   686,
    -714,  -714,  -714,  -714,    57,  -714,  -714,   805,   -42,  -714,
    -714,  -714,   957,   -15,  -714,  -714,   641,  -183,     3,  1025,
    -714,    -5,   -72,   787,   640,  -232,  -228,  -714,   354,  -240,
    -210,  -244,  -714,  -527,  -212,  -243,  -714,  -218,  -534,  -524,
    -526,  -207,  -434,  -714,  -714,   798,  -714,  -714,   375,  -714,
    -714,   372,  -714,   377,  -714,  -714,   383,  -714,  -714,   927,
      54,  -714,   792,    47,  -714,  -714,   790,  -714,   925,  -398,
    -254,  -714,   338,  -287,  -642,  -295,  -300,  -113,  -425,  -714,
     929,  -249,   348,  -714,  -415,  -714,  -395,  -714,  -714,  -118,
     239,  -405,    58,  -411,  -404,  -714,  -250,   147,  -714,  -714,
    -714,   903,  -714,  -714,   568,  -281,   336,  -660,    28,  -713,
       9,  -710,  -690,  -653,    25,  -714,  -714,  -714,   906,  -714,
    -714,   747,  -714,  -714
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -332
static const short int yytable[] =
{
      87,   412,   486,   138,   408,   426,   484,   484,   423,   285,
     285,    51,   265,   478,   418,   418,   236,   351,   449,   420,
     451,   355,   355,   330,    25,   363,   490,   164,   223,   224,
     224,   594,   411,   431,   409,   679,   168,   415,   361,   361,
     612,   679,   598,   471,   259,   601,   684,   389,   183,   688,
     189,   687,   609,   609,   357,   632,   604,   157,   186,   190,
     611,    77,   599,   148,   799,   599,   147,   156,   613,   614,
     797,   252,   786,   787,    37,   387,   228,   204,   205,    14,
      13,    38,   229,    39,   523,    40,   -20,    41,   163,   164,
     830,    42,   388,    98,   834,   834,   183,    15,    16,    17,
      18,    19,    20,    21,    22,    23,   167,   206,   207,   386,
     249,   -20,    43,    44,    45,   836,   250,   255,   543,   334,
     138,   138,    87,    87,   177,   180,   182,   546,   268,   524,
     164,    24,   446,   232,   338,   208,   214,    49,    50,   830,
     830,   306,   200,   201,   834,   834,   818,   819,   303,   163,
     315,   452,   553,   558,   305,    83,   767,   691,   692,   270,
     271,   164,   769,   163,   858,   859,   447,   448,    84,  -113,
     268,    83,   547,   169,   269,   235,   339,   849,   274,   223,
     215,   216,   320,   224,    84,   224,   233,   797,   797,   584,
     585,   340,   386,   172,   325,   326,   327,   554,   559,   318,
     659,   270,   271,    98,   148,  -203,   157,   147,   272,   273,
     384,   252,   384,    83,   656,   660,   156,  -201,   664,   569,
     274,   712,   713,   268,   487,   275,    84,   595,   668,   392,
     246,   217,   163,   665,   175,   218,   379,  -311,   229,  -311,
     247,   810,   248,   669,   320,   757,   811,   211,    70,    71,
      72,   145,    74,   113,   270,   271,    89,    90,   758,   212,
     213,   272,   273,    87,    87,   334,    83,   436,   773,   774,
      84,   320,   636,   274,   173,   679,   335,   437,   596,    84,
     330,   767,   637,   838,   146,   769,   598,   567,   632,   779,
     334,   334,   523,   250,   174,   138,   138,   138,   639,   532,
     541,   564,   306,   468,   377,   378,   599,   640,   343,   599,
     176,   710,   343,   202,   203,   305,   198,   690,   199,   320,
     711,   806,   209,   426,   412,   210,   408,   219,   586,   426,
     412,    98,    98,    98,    98,    98,   425,   428,   430,   301,
     302,   497,   512,   265,   220,   514,   700,   515,   490,   517,
     233,   431,   265,   544,   389,   676,   674,   431,   513,   519,
     415,   685,   415,   234,   579,   238,   415,   239,   415,   716,
     717,   609,   609,   720,   721,   604,   604,   240,   815,   816,
      70,    71,    72,    73,    74,   472,   473,   241,    14,  -330,
    -330,   310,   311,  -330,   229,   538,   539,   364,   365,   366,
     367,   368,   533,   242,   371,   535,    15,    16,    17,    18,
      19,    20,    21,    22,    23,   244,   458,   459,   790,   791,
     460,  -325,  -325,   484,   484,  -325,   245,   478,   478,   412,
    -326,  -326,   408,   518,  -326,   520,   521,   318,   319,   426,
     499,    87,   423,  -331,  -331,   449,   449,  -331,   251,   418,
     256,   138,    87,   765,   295,    87,    87,   296,    87,    87,
     138,   138,   138,   593,   297,   415,   351,   351,   431,   265,
     265,   355,   355,   657,   743,   744,   745,   452,   298,   661,
     662,    98,   306,   630,   631,   666,   342,   343,   361,   361,
     361,   671,   820,   821,   299,   305,   300,   484,   484,   344,
     343,   478,   478,   741,   742,   702,   703,   345,   343,   704,
      84,   196,   197,    70,    71,    72,    73,    74,   317,   114,
      37,   154,   155,    98,   426,   641,   321,    38,   322,    39,
     646,    88,   324,    41,   374,   375,   376,    42,   318,   383,
     394,   343,   318,   433,   333,   229,   463,   464,   336,   606,
     466,   343,   431,   467,   343,   237,   212,   213,    43,    44,
      45,   510,   343,   341,   426,   412,   337,   408,   380,    89,
      90,    91,    92,    93,    94,    95,    96,   138,   138,   346,
     265,   265,   382,    97,    50,   318,    87,    87,   511,   343,
      87,   431,   582,   343,   385,   293,   294,   180,   180,   804,
     805,   415,   415,   583,   343,   591,   343,   592,   343,   268,
     648,   343,   393,   446,   138,   138,    70,    71,    72,    73,
      74,   113,   114,   115,   306,   116,   649,   343,   650,   343,
     390,   730,    98,   651,   343,   652,   343,   305,    98,    98,
     270,   271,   331,   332,    98,   653,   343,   272,   273,   395,
      98,   397,    83,    98,   425,   677,   430,   654,   343,   274,
     425,   400,   430,   746,   402,    84,   435,     1,     2,     3,
       4,     5,     6,     7,     8,     9,    10,    11,   672,   343,
     681,   343,   682,   343,   683,   343,   686,   343,   138,   138,
      87,    87,   695,   343,   696,   343,    87,   739,   740,    87,
     812,   268,   714,   343,   439,   446,   715,   343,   440,   138,
     138,   771,   443,   138,   138,   718,   343,   631,   730,   730,
     719,   343,   780,   343,   445,   441,   442,   781,   343,   782,
     343,   784,   270,   271,   783,   343,   453,   454,   444,   602,
     603,    37,   807,   343,    83,   808,   343,   455,    38,   456,
      39,   274,    88,   457,    41,   809,   343,    84,    42,    98,
     825,   343,   826,   343,   840,   343,   841,   343,    98,    98,
     425,   852,   343,   430,   461,    87,    87,    87,    87,    43,
      44,    45,   462,   138,   138,   853,   343,   138,   138,   465,
      89,    90,   407,    92,    93,    94,    95,    96,   730,   730,
     730,   469,   268,   488,    97,    50,   446,   854,   343,   489,
     827,   855,   343,   525,   526,   369,   370,   527,   528,   491,
     730,   730,   529,   530,   531,   839,   372,   373,   643,   644,
     697,   698,   730,   270,   271,   730,   730,   493,   813,   814,
     607,   608,   775,   776,   494,    83,   495,   730,   730,   730,
     730,   496,   274,   842,   843,   425,   498,   430,    84,    98,
     723,   856,   857,   504,   724,   850,   851,   507,   508,    37,
     509,   565,   522,   534,   566,   536,    38,   540,    39,   545,
      88,   570,    41,   551,   552,   571,    42,   557,   572,   563,
     568,   725,   726,   574,    98,   425,   430,   573,   727,   728,
     575,   580,   578,   581,   589,   590,   615,    43,    44,    45,
     729,   587,   588,    46,    47,    48,    84,   616,    89,    90,
      91,    92,    93,    94,    95,    96,   619,   620,   623,   624,
     627,   634,    97,    50,   635,   638,   628,   642,   621,   622,
      37,   645,   655,   658,   625,   626,   663,    38,   667,    39,
     670,    88,   689,    41,   693,   694,   699,    42,   709,    37,
     701,   722,  -320,   747,   748,   547,    38,   750,    39,   751,
      88,   752,    41,   554,   559,   768,    42,   754,    43,    44,
      45,   759,   770,   772,   777,   788,   789,   792,   802,    89,
      90,   417,    92,    93,    94,    95,    96,    43,    44,    45,
     793,   803,   817,    97,    50,   824,   837,   844,    89,    90,
     421,    92,    93,    94,    95,    96,   576,   506,   738,   328,
     406,   647,    97,    50,    37,   516,   381,   537,   329,   227,
      85,    38,   416,    39,   542,    88,   766,    41,   399,   753,
     756,    42,   749,    37,   755,   434,   260,   438,   785,   778,
      38,   267,    39,   309,    88,   629,    41,   292,   794,   492,
      42,     0,    43,    44,    45,   316,   705,   706,     0,     0,
       0,   707,   708,    89,    90,   424,    92,    93,    94,    95,
      96,    43,    44,    45,     0,     0,     0,    97,    50,     0,
       0,     0,    89,    90,   429,    92,    93,    94,    95,    96,
       0,   737,     0,     0,     0,    37,    97,    50,     0,     0,
       0,     0,    38,     0,    39,     0,    88,     0,    41,     0,
       0,     0,    42,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   762,     0,   763,
       0,     0,   764,    43,    44,    45,     0,     0,     0,     0,
       0,     0,     0,     0,    89,    90,   673,    92,    93,    94,
      95,    96,     0,    37,     0,     0,     0,     0,    97,    50,
      38,     0,    39,     0,    88,     0,    41,     0,     0,     0,
      42,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   800,   801,     0,     0,     0,     0,     0,     0,     0,
       0,    43,    44,    45,     0,     0,     0,     0,     0,     0,
       0,  -166,    89,    90,   675,    92,    93,    94,    95,    96,
       0,     0,     0,    37,     0,  -166,    97,    50,     0,     0,
      38,     0,    39,     0,    88,  -113,    41,  -166,  -166,     0,
      42,  -113,     0,     0,  -113,     0,     0,  -166,     0,     0,
    -166,   723,  -166,   243,  -113,   724,     0,     0,     0,   822,
     823,    43,    44,    45,     0,     0,     0,     0,     0,     0,
       0,     0,    89,    90,   678,    92,    93,    94,    95,    96,
       0,     0,   725,   726,     0,     0,    97,    50,     0,   795,
     796,    37,   845,   846,     0,     0,   847,   848,    38,     0,
      39,   729,    40,    37,    41,   184,    37,    84,    42,     0,
      38,     0,    39,    38,    40,    39,    41,    40,   404,    41,
      42,   187,     0,    42,     0,     0,     0,     0,     0,    43,
      44,    45,     0,     0,     0,    46,    47,    48,     0,     0,
       0,    43,    44,    45,    43,    44,    45,    46,    47,    48,
      46,    47,    48,     0,    49,    50,     0,     0,     0,     0,
       0,     0,     0,     0,    37,     0,   188,    50,     0,   405,
      50,    38,     0,    39,     0,    40,    37,    41,     0,    37,
       0,    42,     0,    38,     0,    39,    38,    40,    39,    41,
      40,     0,    41,    42,     0,     0,    42,     0,     0,     0,
       0,     0,    43,    44,    45,     0,     0,     0,    46,    47,
      48,     0,     0,     0,    43,    44,    45,    43,    44,    45,
      46,    47,    48,   358,   359,   360,     0,    49,    50,     0,
       0,     0,     0,     0,     0,     0,     0,    37,     0,   304,
      50,     0,    49,    50,    38,     0,    39,     0,    40,    37,
      41,     0,    37,     0,    42,     0,    38,     0,    39,    38,
      40,    39,    41,    40,     0,    41,    42,     0,     0,    42,
       0,     0,     0,     0,     0,    43,    44,    45,     0,     0,
       0,     0,   178,   179,     0,     0,     0,    43,    44,    45,
      43,    44,    45,     0,   349,   350,     0,   353,   354,   125,
      49,    50,     0,   126,     0,   127,     0,   128,     0,     0,
       0,     0,    49,    50,     0,    49,    50,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   125,     0,
     129,   130,   126,     0,   127,     0,   128,   131,   132,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   133,
     134,   135,   136,   137,     0,    84,     0,   125,     0,   129,
     130,   126,     0,   127,     0,   128,   261,   262,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   133,   263,
     264,   136,   137,     0,    84,     0,   125,     0,   129,   130,
     126,     0,   127,     0,   128,   474,   475,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   133,   476,   477,
     136,   137,   723,    84,     0,   723,   724,   129,   130,   724,
       0,     0,     0,     0,   480,   481,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   133,   482,   483,   136,
     137,     0,    84,   725,   726,     0,   725,   726,     0,     0,
     828,   829,     0,   832,   833,     0,     0,     0,     0,     0,
       0,     0,   729,     0,     0,   729,     0,     0,    84,     0,
       0,    84
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
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    17,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    19,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    21,     0,     0,
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
       0,     0,     0,     0,     0,     0,     0,    15,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    23,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    11,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    13,     0,
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
       0,     0,     0,     0,     0,     1,     0,     0,     0,     0,
       0,     3,     0,     0,     5,     0,     0,     0,     0,     0,
       0,     0,     0,     7,     9,     0,     0,     0,     0,     0,
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
       0,     0
};

/* YYCONFL[I] -- lists of conflicting rule numbers, each terminated by
   0, pointed into by YYCONFLP.  */
static const short int yyconfl[] =
{
       0,   166,     0,   166,     0,   166,     0,   113,     0,   166,
       0,    21,     0,   113,     0,   319,     0,   166,     0,   113,
       0,   203,     0,   319,     0
};

static const short int yycheck[] =
{
       5,   244,   302,     8,   244,   249,   301,   302,   248,   127,
     128,     3,   125,   300,   246,   247,    88,   199,   268,   247,
     269,   200,   201,   176,     2,   203,   307,    11,    70,    71,
      72,   456,   244,   251,   244,   569,    14,   244,   202,   203,
     465,   575,   457,   297,   116,   460,   573,   230,    40,   575,
      42,   575,   463,   464,   201,   489,   461,    10,    41,    42,
     464,     4,   457,     9,   724,   460,     9,    10,   466,   467,
     723,   113,   714,   715,    31,    15,    73,    16,    17,    42,
      94,    38,    74,    40,    34,    42,    15,    44,    94,    73,
     803,    48,    32,     6,   804,   805,    88,    60,    61,    62,
      63,    64,    65,    66,    67,    68,     0,    46,    47,    54,
      33,    40,    69,    70,    71,   805,    39,   114,    49,    34,
     125,   126,   127,   128,    37,    38,    39,    49,    38,    79,
     114,    94,    42,    76,    49,    74,    28,    94,    95,   852,
     853,   146,    23,    24,   854,   855,   788,   789,   145,    94,
     155,   269,    49,    49,   146,    81,   683,   582,   583,    69,
      70,   145,   686,    94,   854,   855,    76,    77,    94,    25,
      38,    81,    94,    94,    42,    88,    34,   837,    88,   221,
      72,    73,   166,   225,    94,   227,    42,   840,   841,   443,
     444,    49,    54,    42,   172,   173,   174,    94,    94,    34,
      34,    69,    70,   116,   150,    27,   159,   150,    76,    77,
     225,   253,   227,    81,    49,    49,   159,    39,    34,    41,
      88,   619,   620,    38,    86,    93,    94,    42,    34,   234,
      14,    29,    94,    49,    15,    33,   219,    45,   230,    47,
      24,   768,    26,    49,   228,    81,   770,    18,    50,    51,
      52,    53,    54,    55,    69,    70,    80,    81,    94,    30,
      31,    76,    77,   268,   269,    34,    81,    32,   693,   694,
      94,   255,    32,    88,    42,   809,    45,    42,    93,    94,
     433,   808,    42,   809,    86,   809,   701,    33,   722,   704,
      34,    34,    34,    39,    42,   300,   301,   302,    34,    43,
      43,    43,   307,   295,   217,   218,   701,    43,    34,   704,
      40,    34,    34,    21,    22,   307,    78,    43,    25,   303,
      43,    43,    19,   567,   567,    20,   566,    42,   446,   573,
     573,   244,   245,   246,   247,   248,   249,   250,   251,    23,
      24,   319,   334,   456,    94,   337,   595,   339,   629,   341,
      42,   569,   465,   395,   537,   567,   566,   575,   336,   343,
     567,   573,   569,    41,   436,    42,   573,    42,   575,   623,
     624,   782,   783,   627,   628,   780,   781,    42,   782,   783,
      50,    51,    52,    53,    54,   298,   299,    42,    42,    29,
      30,    89,    90,    33,   386,   387,   388,   204,   205,   206,
     207,   208,   380,    42,   211,   383,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    15,    29,    30,   718,   719,
      33,    29,    30,   718,   719,    33,    30,   714,   715,   672,
      29,    30,   672,   342,    33,   344,   345,    34,    35,   683,
      94,   446,   682,    29,    30,   695,   696,    33,    41,   681,
      94,   456,   457,   681,    42,   460,   461,    94,   463,   464,
     465,   466,   467,   455,    94,   672,   648,   649,   686,   582,
     583,   650,   651,   545,   652,   653,   654,   595,    39,   551,
     552,   394,   487,   488,   489,   557,    33,    34,   652,   653,
     654,   563,   792,   793,    39,   487,    25,   792,   793,    33,
      34,   788,   789,   650,   651,    29,    30,    33,    34,    33,
      94,    47,    48,    50,    51,    52,    53,    54,    36,    56,
      31,    58,    59,   436,   768,   503,    43,    38,    42,    40,
     522,    42,    41,    44,   214,   215,   216,    48,    34,    35,
      33,    34,    34,    35,    43,   537,    23,    24,    35,   462,
      33,    34,   770,    33,    34,    91,    30,    31,    69,    70,
      71,    33,    34,    41,   808,   808,    35,   807,    32,    80,
      81,    82,    83,    84,    85,    86,    87,   582,   583,    94,
     693,   694,    36,    94,    95,    34,   591,   592,    33,    34,
     595,   809,    33,    34,    36,   131,   132,   510,   511,    23,
      24,   808,   809,    33,    34,    33,    34,    33,    34,    38,
      33,    34,    43,    42,   619,   620,    50,    51,    52,    53,
      54,    55,    56,    57,   629,    59,    33,    34,    33,    34,
      36,   636,   545,    33,    34,    33,    34,   629,   551,   552,
      69,    70,   178,   179,   557,    33,    34,    76,    77,    48,
     563,    48,    81,   566,   567,   568,   569,    33,    34,    88,
     573,    48,   575,   655,    48,    94,    36,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    33,    34,
      33,    34,    33,    34,    33,    34,    33,    34,   693,   694,
     695,   696,    33,    34,    33,    34,   701,   648,   649,   704,
     772,    38,    33,    34,    36,    42,    33,    34,    36,   714,
     715,   689,    94,   718,   719,    33,    34,   722,   723,   724,
      33,    34,    33,    34,    43,   261,   262,    33,    34,    33,
      34,   709,    69,    70,    33,    34,   272,   273,    94,    76,
      77,    31,    33,    34,    81,    33,    34,    42,    38,    45,
      40,    88,    42,    30,    44,    33,    34,    94,    48,   672,
      33,    34,    33,    34,    33,    34,    33,    34,   681,   682,
     683,    33,    34,   686,    25,   780,   781,   782,   783,    69,
      70,    71,    39,   788,   789,    33,    34,   792,   793,    47,
      80,    81,    82,    83,    84,    85,    86,    87,   803,   804,
     805,    42,    38,    15,    94,    95,    42,    33,    34,    25,
     802,    33,    34,   349,   350,   209,   210,   353,   354,    36,
     825,   826,   358,   359,   360,   817,   212,   213,   510,   511,
     591,   592,   837,    69,    70,   840,   841,    36,   780,   781,
      76,    77,   695,   696,    94,    81,    36,   852,   853,   854,
     855,    94,    88,   825,   826,   768,    36,   770,    94,   772,
      38,   852,   853,    37,    42,   840,   841,    43,    43,    31,
      43,   407,    32,    36,    27,    36,    38,    36,    40,    34,
      42,   417,    44,    34,    34,   421,    48,    34,   424,    34,
      39,    69,    70,   429,   807,   808,   809,    15,    76,    77,
      15,   437,    36,    36,    43,    43,    43,    69,    70,    71,
      88,   447,   448,    75,    76,    77,    94,    94,    80,    81,
      82,    83,    84,    85,    86,    87,    33,    33,    94,    94,
      94,    36,    94,    95,    36,    35,    94,    94,   474,   475,
      31,    49,    32,    41,   480,   481,    15,    38,    41,    40,
      15,    42,    15,    44,    33,    33,    43,    48,    35,    31,
      30,    25,    30,    43,    94,    94,    38,    43,    40,    43,
      42,    94,    44,    94,    94,    27,    48,    43,    69,    70,
      71,    43,    27,    32,    43,    33,    33,    33,    42,    80,
      81,    82,    83,    84,    85,    86,    87,    69,    70,    71,
      33,    25,    32,    94,    95,    43,    32,    43,    80,    81,
      82,    83,    84,    85,    86,    87,   433,   324,   639,   175,
     243,   523,    94,    95,    31,   339,   221,   386,   176,    72,
       5,    38,   245,    40,   394,    42,   682,    44,   240,   664,
     668,    48,   659,    31,   667,   253,   119,   257,   710,   701,
      38,   126,    40,   150,    42,   487,    44,   128,   722,   312,
      48,    -1,    69,    70,    71,   159,   602,   603,    -1,    -1,
      -1,   607,   608,    80,    81,    82,    83,    84,    85,    86,
      87,    69,    70,    71,    -1,    -1,    -1,    94,    95,    -1,
      -1,    -1,    80,    81,    82,    83,    84,    85,    86,    87,
      -1,   637,    -1,    -1,    -1,    31,    94,    95,    -1,    -1,
      -1,    -1,    38,    -1,    40,    -1,    42,    -1,    44,    -1,
      -1,    -1,    48,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   673,    -1,   675,
      -1,    -1,   678,    69,    70,    71,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    80,    81,    82,    83,    84,    85,
      86,    87,    -1,    31,    -1,    -1,    -1,    -1,    94,    95,
      38,    -1,    40,    -1,    42,    -1,    44,    -1,    -1,    -1,
      48,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   727,   728,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    69,    70,    71,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     0,    80,    81,    82,    83,    84,    85,    86,    87,
      -1,    -1,    -1,    31,    -1,    14,    94,    95,    -1,    -1,
      38,    -1,    40,    -1,    42,    24,    44,    26,    27,    -1,
      48,    30,    -1,    -1,    33,    -1,    -1,    36,    -1,    -1,
      39,    38,    41,    42,    43,    42,    -1,    -1,    -1,   795,
     796,    69,    70,    71,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    80,    81,    82,    83,    84,    85,    86,    87,
      -1,    -1,    69,    70,    -1,    -1,    94,    95,    -1,    76,
      77,    31,   828,   829,    -1,    -1,   832,   833,    38,    -1,
      40,    88,    42,    31,    44,    45,    31,    94,    48,    -1,
      38,    -1,    40,    38,    42,    40,    44,    42,    43,    44,
      48,    49,    -1,    48,    -1,    -1,    -1,    -1,    -1,    69,
      70,    71,    -1,    -1,    -1,    75,    76,    77,    -1,    -1,
      -1,    69,    70,    71,    69,    70,    71,    75,    76,    77,
      75,    76,    77,    -1,    94,    95,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    31,    -1,    94,    95,    -1,    94,
      95,    38,    -1,    40,    -1,    42,    31,    44,    -1,    31,
      -1,    48,    -1,    38,    -1,    40,    38,    42,    40,    44,
      42,    -1,    44,    48,    -1,    -1,    48,    -1,    -1,    -1,
      -1,    -1,    69,    70,    71,    -1,    -1,    -1,    75,    76,
      77,    -1,    -1,    -1,    69,    70,    71,    69,    70,    71,
      75,    76,    77,    75,    76,    77,    -1,    94,    95,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    31,    -1,    94,
      95,    -1,    94,    95,    38,    -1,    40,    -1,    42,    31,
      44,    -1,    31,    -1,    48,    -1,    38,    -1,    40,    38,
      42,    40,    44,    42,    -1,    44,    48,    -1,    -1,    48,
      -1,    -1,    -1,    -1,    -1,    69,    70,    71,    -1,    -1,
      -1,    -1,    76,    77,    -1,    -1,    -1,    69,    70,    71,
      69,    70,    71,    -1,    76,    77,    -1,    76,    77,    38,
      94,    95,    -1,    42,    -1,    44,    -1,    46,    -1,    -1,
      -1,    -1,    94,    95,    -1,    94,    95,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    38,    -1,
      69,    70,    42,    -1,    44,    -1,    46,    76,    77,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    88,
      89,    90,    91,    92,    -1,    94,    -1,    38,    -1,    69,
      70,    42,    -1,    44,    -1,    46,    76,    77,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    88,    89,
      90,    91,    92,    -1,    94,    -1,    38,    -1,    69,    70,
      42,    -1,    44,    -1,    46,    76,    77,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    88,    89,    90,
      91,    92,    38,    94,    -1,    38,    42,    69,    70,    42,
      -1,    -1,    -1,    -1,    76,    77,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    88,    89,    90,    91,
      92,    -1,    94,    69,    70,    -1,    69,    70,    -1,    -1,
      76,    77,    -1,    76,    77,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    88,    -1,    -1,    88,    -1,    -1,    94,    -1,
      -1,    94
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned short int yystos[] =
{
       0,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    97,    94,    42,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    94,    98,    99,   100,   101,   102,
     103,   104,   105,   113,   114,   115,   116,    31,    38,    40,
      42,    44,    48,    69,    70,    71,    75,    76,    77,    94,
      95,   117,   118,   121,   124,   126,   128,   130,   131,   132,
     133,   134,   135,   136,   137,   139,   141,   142,   143,   146,
      50,    51,    52,    53,    54,   148,   149,   150,   151,   156,
     157,   160,   161,    81,    94,   165,   166,   167,    42,    80,
      81,    82,    83,    84,    85,    86,    87,    94,   137,   167,
     168,   169,   170,   171,   173,   175,   177,   181,   183,   187,
     188,   189,   190,    55,    56,    57,    59,   150,   203,   204,
     205,   206,   209,   210,   213,    38,    42,    44,    46,    69,
      70,    76,    77,    88,    89,    90,    91,    92,   167,   214,
     215,   219,   221,   223,   225,    53,    86,   150,   206,   245,
     246,   247,   248,   249,    58,    59,   150,   209,   262,   263,
     264,   265,   269,    94,   123,   154,   164,     0,    98,    94,
     106,   107,    42,    42,    42,    15,    40,   137,    76,    77,
     137,   138,   137,   117,    45,   117,   140,    49,    94,   117,
     140,   144,   145,   147,   122,   123,   122,   122,    78,    25,
      23,    24,    21,    22,    16,    17,    46,    47,    74,    19,
      20,    18,    30,    31,    28,    72,    73,    29,    33,    42,
      94,   152,   153,   154,   154,   158,   159,   158,   164,   117,
     162,   163,   150,    42,    41,   137,   168,   122,    42,    42,
      42,    42,    42,    42,    15,    30,    14,    24,    26,    33,
      39,    41,   154,   207,   208,   164,    94,   211,   212,   168,
     205,    76,    77,    89,    90,   223,   224,   214,    38,    42,
      69,    70,    76,    77,    88,    93,   165,   226,   227,   229,
     230,   231,   232,   233,   234,   235,   236,   237,   239,   241,
     242,   244,   226,   122,   122,    42,    94,    94,    39,    39,
      25,    23,    24,   164,    94,   117,   167,   250,   251,   247,
      89,    90,   266,   267,   268,   167,   264,    36,    34,    35,
     123,    43,    42,   108,    41,    98,    98,    98,   100,   103,
     105,   122,   122,    43,    34,    45,    35,    35,    49,    34,
      49,    41,    33,    34,    33,    33,    94,   119,   120,    76,
      77,   124,   125,    76,    77,   126,   127,   127,    75,    76,
      77,   128,   129,   129,   131,   131,   131,   131,   131,   134,
     134,   131,   135,   135,   136,   136,   136,   137,   137,   140,
      32,   153,    36,    35,   159,    36,    54,    15,    32,   163,
      36,   140,   167,    43,    33,    48,   191,    48,   200,   191,
      48,   192,    48,   195,    43,    94,   119,    82,   175,   176,
     178,   180,   181,   182,   184,   187,   169,    82,   171,   172,
     172,    82,   174,   175,    82,   137,   177,   179,   137,    82,
     137,   183,   185,    35,   208,    36,    32,    42,   212,    36,
      36,   122,   122,    94,    94,    43,    42,    76,    77,   242,
     243,   227,   235,   122,   122,    42,    45,    30,    29,    30,
      33,    25,    39,    23,    24,    47,    33,    33,   117,    42,
     216,   216,   137,   137,    76,    77,    89,    90,   219,   220,
      76,    77,    89,    90,   221,   222,   222,    86,    15,    25,
     251,    36,   267,    36,    94,    36,    94,    98,    36,    94,
      98,   110,   111,   112,    37,   109,   107,    43,    43,    43,
      33,    33,   117,    98,   117,   117,   145,   117,   121,   123,
     121,   121,    32,    34,    79,   122,   122,   122,   122,   122,
     122,   122,    43,    98,    36,    98,    36,   162,   117,   117,
      36,    43,   170,    49,   154,    34,    49,    94,   199,   201,
     202,    34,    34,    49,    94,   193,   194,    34,    49,    94,
     196,   197,   198,    34,    43,   122,    27,    33,    39,    41,
     122,   122,   122,    15,   122,    15,   101,   155,    36,   168,
     122,    36,    33,    33,   216,   216,   235,   122,   122,    43,
      43,    33,    33,   117,   224,    42,    93,   228,   230,   232,
     235,   230,    76,    77,   237,   238,   137,    76,    77,   239,
     240,   240,   224,   215,   215,    43,    94,   217,   218,    33,
      33,   122,   122,    94,    94,   122,   122,    94,    94,   250,
     167,   167,   188,   252,    36,    36,    32,    42,    35,    34,
      43,    98,    94,   138,   138,    49,   117,   120,    33,    33,
      33,    33,    33,    33,    33,    32,    49,   168,    41,    34,
      49,   168,   168,    15,    34,    49,   168,    41,    34,    49,
      15,   168,    33,    82,   176,    82,   180,   137,    82,   184,
     186,    33,    33,    33,   179,   180,    33,   185,   186,    15,
      43,   224,   224,    33,    33,    33,    33,   236,   236,    43,
     227,    30,    29,    30,    33,   122,   122,   122,   122,    35,
      34,    43,   215,   215,    33,    33,   216,   216,    33,    33,
     216,   216,    25,    38,    42,    69,    70,    76,    77,    88,
     167,   253,   254,   255,   257,   259,   261,   122,   111,   125,
     125,   127,   127,   129,   129,   129,   117,    43,    94,   202,
      43,    43,    94,   194,    43,   199,   197,    81,    94,    43,
     176,   180,   122,   122,   122,   172,   174,   179,    27,   185,
      27,    98,    32,   224,   224,   243,   243,    43,   228,   230,
      33,    33,    33,    33,    98,   218,   220,   220,    33,    33,
     222,   222,    33,    33,   252,    76,    77,   259,   260,   253,
     122,   122,    42,    25,    23,    24,    43,    33,    33,    33,
     179,   185,   168,   238,   238,   240,   240,    32,   220,   220,
     222,   222,   122,   122,    43,    33,    33,   117,    76,    77,
     255,   256,    76,    77,   257,   258,   258,    32,   186,   117,
      33,    33,   254,   254,    43,   122,   122,   122,   122,   253,
     260,   260,    33,    33,    33,    33,   256,   256,   258,   258
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
   while (YYID (0))

#if YYDEBUG

# ifndef YYFPRINTF
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			    \
do {									    \
  if (yydebug)								    \
    {									    \
      YYFPRINTF (stderr, "%s ", Title);					    \
      yy_symbol_print (stderr, Type,					    \
		       Value);  \
      YYFPRINTF (stderr, "\n");						    \
    }									    \
} while (YYID (0))

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
   to be completed by functions that call yyexpandGLRStack before the
   stack is expanded, thus insuring that all necessary pointers get
   properly redirected to new data.  */
#define YYHEADROOM 2

#ifndef YYSTACKEXPANDABLE
# if (! defined __cplusplus \
      || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL))
#  define YYSTACKEXPANDABLE 1
# else
#  define YYSTACKEXPANDABLE 0
# endif
#endif

#if YYSTACKEXPANDABLE
# define YY_RESERVE_GLRSTACK(Yystack)			\
  do {							\
    if (Yystack->yyspaceLeft < YYHEADROOM)		\
      yyexpandGLRStack (Yystack);			\
  } while (YYID (0))
#else
# define YY_RESERVE_GLRSTACK(Yystack)			\
  do {							\
    if (Yystack->yyspaceLeft < YYHEADROOM)		\
      yyMemoryExhausted (Yystack);			\
  } while (YYID (0))
#endif


#if YYERROR_VERBOSE

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static size_t
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      size_t yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return strlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

#endif /* !YYERROR_VERBOSE */

/** State numbers, as in LALR(1) machine */
typedef int yyStateNum;

/** Rule numbers, as in LALR(1) machine */
typedef int yyRuleNum;

/** Grammar symbol */
typedef short int yySymbol;

/** Item references, as in LALR(1) machine */
typedef short int yyItemNum;

typedef struct yyGLRState yyGLRState;
typedef struct yyGLRStateSet yyGLRStateSet;
typedef struct yySemanticOption yySemanticOption;
typedef union yyGLRStackItem yyGLRStackItem;
typedef struct yyGLRStack yyGLRStack;

struct yyGLRState {
  /** Type tag: always true.  */
  yybool yyisState;
  /** Type tag for yysemantics.  If true, yysval applies, otherwise
   *  yyfirstVal applies.  */
  yybool yyresolved;
  /** Number of corresponding LALR(1) machine state.  */
  yyStateNum yylrState;
  /** Preceding state in this stack */
  yyGLRState* yypred;
  /** Source position of the first token produced by my symbol */
  size_t yyposn;
  union {
    /** First in a chain of alternative reductions producing the
     *  non-terminal corresponding to this state, threaded through
     *  yynext.  */
    yySemanticOption* yyfirstVal;
    /** Semantic value for this state.  */
    YYSTYPE yysval;
  } yysemantics;
  /** Source location for this state.  */
  YYLTYPE yyloc;
};

struct yyGLRStateSet {
  yyGLRState** yystates;
  /** During nondeterministic operation, yylookaheadNeeds tracks which
   *  stacks have actually needed the current lookahead.  During deterministic
   *  operation, yylookaheadNeeds[0] is not maintained since it would merely
   *  duplicate yychar != YYEMPTY.  */
  yybool* yylookaheadNeeds;
  size_t yysize, yycapacity;
};

struct yySemanticOption {
  /** Type tag: always false.  */
  yybool yyisState;
  /** Rule number for this reduction */
  yyRuleNum yyrule;
  /** The last RHS state in the list of states to be reduced.  */
  yyGLRState* yystate;
  /** The lookahead for this reduction.  */
  int yyrawchar;
  YYSTYPE yyval;
  YYLTYPE yyloc;
  /** Next sibling in chain of options.  To facilitate merging,
   *  options are chained in decreasing order by address.  */
  yySemanticOption* yynext;
};

/** Type of the items in the GLR stack.  The yyisState field
 *  indicates which item of the union is valid.  */
union yyGLRStackItem {
  yyGLRState yystate;
  yySemanticOption yyoption;
};

struct yyGLRStack {
  int yyerrState;


  YYJMP_BUF yyexception_buffer;
  yyGLRStackItem* yyitems;
  yyGLRStackItem* yynextFree;
  size_t yyspaceLeft;
  yyGLRState* yysplitPoint;
  yyGLRState* yylastDeleted;
  yyGLRStateSet yytops;
};

#if YYSTACKEXPANDABLE
static void yyexpandGLRStack (yyGLRStack* yystackp);
#endif

static void yyFail (yyGLRStack* yystackp, const char* yymsg)
  __attribute__ ((__noreturn__));
static void
yyFail (yyGLRStack* yystackp, const char* yymsg)
{
  if (yymsg != NULL)
    yyerror (yymsg);
  YYLONGJMP (yystackp->yyexception_buffer, 1);
}

static void yyMemoryExhausted (yyGLRStack* yystackp)
  __attribute__ ((__noreturn__));
static void
yyMemoryExhausted (yyGLRStack* yystackp)
{
  YYLONGJMP (yystackp->yyexception_buffer, 2);
}

#if YYDEBUG || YYERROR_VERBOSE
/** A printable representation of TOKEN.  */
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
 *  containing the pointer to the next state in the chain.  */
static void yyfillin (yyGLRStackItem *, int, int) __attribute__ ((__unused__));
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
 * YYVSP[YYLOW1 .. *YYLOW-1] as in yyfillin and set *YYLOW = YYLOW1.
 * For convenience, always return YYLOW1.  */
static inline int yyfill (yyGLRStackItem *, int *, int, yybool)
     __attribute__ ((__unused__));
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
 *  (@$).  Returns yyok for normal return, yyaccept for YYACCEPT,
 *  yyerr for YYERROR, yyabort for YYABORT.  */
/*ARGSUSED*/ static YYRESULTTAG
yyuserAction (yyRuleNum yyn, int yyrhslen, yyGLRStackItem* yyvsp,
	      YYSTYPE* yyvalp,
	      YYLTYPE* YYOPTIONAL_LOC (yylocp),
	      yyGLRStack* yystackp
	      )
{
  yybool yynormal __attribute__ ((__unused__)) =
    (yystackp->yysplitPoint == NULL);
  int yylow;
# undef yyerrok
# define yyerrok (yystackp->yyerrState = 0)
# undef YYACCEPT
# define YYACCEPT return yyaccept
# undef YYABORT
# define YYABORT return yyabort
# undef YYERROR
# define YYERROR return yyerrok, yyerr
# undef YYRECOVERING
# define YYRECOVERING() (yystackp->yyerrState != 0)
# undef yyclearin
# define yyclearin (yychar = YYEMPTY)
# undef YYFILL
# define YYFILL(N) yyfill (yyvsp, &yylow, N, yynormal)
# undef YYBACKUP
# define YYBACKUP(Token, Value)						     \
  return yyerror (YY_("syntax error: cannot back up")),     \
	 yyerrok, yyerr

  yylow = 1;
  if (yyrhslen == 0)
    *yyvalp = yyval_default;
  else
    *yyvalp = yyvsp[YYFILL (1-yyrhslen)].yystate.yysemantics.yysval;
  YYLLOC_DEFAULT ((*yylocp), (yyvsp - yyrhslen), yyrhslen);

  switch (yyn)
    {
        case 2:

/* Line 936 of glr.c  */
#line 361 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).term), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl));
      mcrl3_spec_tree = ((*yyvalp).term);
    ;}
    break;

  case 3:

/* Line 936 of glr.c  */
#line 366 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).term), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl));
      mcrl3_spec_tree = ((*yyvalp).term);
    ;}
    break;

  case 4:

/* Line 936 of glr.c  */
#line 371 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).term), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl));
      mcrl3_spec_tree = ((*yyvalp).term);
    ;}
    break;

  case 5:

/* Line 936 of glr.c  */
#line 376 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).term), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl));
      mcrl3_spec_tree = ((*yyvalp).term);
    ;}
    break;

  case 6:

/* Line 936 of glr.c  */
#line 381 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).term), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl));
      mcrl3_spec_tree = ((*yyvalp).term);
    ;}
    break;

  case 7:

/* Line 936 of glr.c  */
#line 386 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).term), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl));
      mcrl3_spec_tree = ((*yyvalp).term);
    ;}
    break;

  case 8:

/* Line 936 of glr.c  */
#line 391 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).term), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl));
      mcrl3_spec_tree = ((*yyvalp).term);
    ;}
    break;

  case 9:

/* Line 936 of glr.c  */
#line 396 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).term), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl));
      mcrl3_spec_tree = ((*yyvalp).term);
    ;}
    break;

  case 10:

/* Line 936 of glr.c  */
#line 401 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).term), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl));
      mcrl3_spec_tree = ((*yyvalp).term);
    ;}
    break;

  case 11:

/* Line 936 of glr.c  */
#line 406 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).term), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl));
      mcrl3_spec_tree = ((*yyvalp).term);
    ;}
    break;

  case 12:

/* Line 936 of glr.c  */
#line 411 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).term), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.list));
      mcrl3_spec_tree = ((*yyvalp).term);
    ;}
    break;

  case 13:

/* Line 936 of glr.c  */
#line 423 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 14:

/* Line 936 of glr.c  */
#line 431 "mcrl2parser.yy"
    {
    safe_assign(((*yyvalp).appl), gsMakeSortArrow((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
  ;}
    break;

  case 15:

/* Line 936 of glr.c  */
#line 439 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 17:

/* Line 936 of glr.c  */
#line 448 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list)));
    ;}
    break;

  case 18:

/* Line 936 of glr.c  */
#line 456 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 19:

/* Line 936 of glr.c  */
#line 460 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 20:

/* Line 936 of glr.c  */
#line 468 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 21:

/* Line 936 of glr.c  */
#line 476 "mcrl2parser.yy"
    {
    safe_assign(((*yyvalp).appl), gsMakeSortStruct(ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.list))));
  ;}
    break;

  case 22:

/* Line 936 of glr.c  */
#line 484 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 23:

/* Line 936 of glr.c  */
#line 488 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 24:

/* Line 936 of glr.c  */
#line 496 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 25:

/* Line 936 of glr.c  */
#line 500 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 26:

/* Line 936 of glr.c  */
#line 509 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStructCons((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 27:

/* Line 936 of glr.c  */
#line 516 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList0());
    ;}
    break;

  case 28:

/* Line 936 of glr.c  */
#line 520 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.list)));
    ;}
    break;

  case 29:

/* Line 936 of glr.c  */
#line 528 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeNil());
    ;}
    break;

  case 30:

/* Line 936 of glr.c  */
#line 532 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 31:

/* Line 936 of glr.c  */
#line 540 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 32:

/* Line 936 of glr.c  */
#line 544 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 33:

/* Line 936 of glr.c  */
#line 552 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStructProj(gsMakeNil(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 34:

/* Line 936 of glr.c  */
#line 556 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStructProj((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 35:

/* Line 936 of glr.c  */
#line 562 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 36:

/* Line 936 of glr.c  */
#line 570 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSortId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 37:

/* Line 936 of glr.c  */
#line 578 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 38:

/* Line 936 of glr.c  */
#line 582 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 39:

/* Line 936 of glr.c  */
#line 586 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 40:

/* Line 936 of glr.c  */
#line 590 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 41:

/* Line 936 of glr.c  */
#line 598 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSortId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 42:

/* Line 936 of glr.c  */
#line 602 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSortId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 43:

/* Line 936 of glr.c  */
#line 606 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSortId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 44:

/* Line 936 of glr.c  */
#line 610 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSortId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 45:

/* Line 936 of glr.c  */
#line 614 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSortId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 46:

/* Line 936 of glr.c  */
#line 622 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), mcrl2::data::sort_list::list(mcrl2::data::sort_expression((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl))));
    ;}
    break;

  case 47:

/* Line 936 of glr.c  */
#line 626 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), mcrl2::data::sort_set::set_(mcrl2::data::sort_expression((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl))));
    ;}
    break;

  case 48:

/* Line 936 of glr.c  */
#line 630 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), mcrl2::data::sort_bag::bag(mcrl2::data::sort_expression((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl))));
    ;}
    break;

  case 49:

/* Line 936 of glr.c  */
#line 641 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 50:

/* Line 936 of glr.c  */
#line 649 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 51:

/* Line 936 of glr.c  */
#line 653 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeWhr((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.appl), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.list))));
    ;}
    break;

  case 52:

/* Line 936 of glr.c  */
#line 661 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 53:

/* Line 936 of glr.c  */
#line 665 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 54:

/* Line 936 of glr.c  */
#line 673 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeIdInit((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 55:

/* Line 936 of glr.c  */
#line 681 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 56:

/* Line 936 of glr.c  */
#line 685 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBinder(gsMakeLambda(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 57:

/* Line 936 of glr.c  */
#line 689 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBinder(gsMakeForall(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 58:

/* Line 936 of glr.c  */
#line 693 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBinder(gsMakeExists(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 59:

/* Line 936 of glr.c  */
#line 702 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list));
    ;}
    break;

  case 60:

/* Line 936 of glr.c  */
#line 706 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATconcat((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.list)));
    ;}
    break;

  case 61:

/* Line 936 of glr.c  */
#line 714 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList0());
      int n = ATgetLength((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list));
      for (int i = 0; i < n; i++) {
        safe_assign(((*yyvalp).list), ATinsert(((*yyvalp).list), (ATerm) gsMakeDataVarId(ATAelementAt((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), i), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl))));
      }
    ;}
    break;

  case 62:

/* Line 936 of glr.c  */
#line 726 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 63:

/* Line 936 of glr.c  */
#line 730 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl),
        gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)), ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl))));
    ;}
    break;

  case 64:

/* Line 936 of glr.c  */
#line 739 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 65:

/* Line 936 of glr.c  */
#line 743 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBinder(gsMakeForall(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 66:

/* Line 936 of glr.c  */
#line 747 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBinder(gsMakeExists(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 67:

/* Line 936 of glr.c  */
#line 755 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 68:

/* Line 936 of glr.c  */
#line 759 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl),
        gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)), ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl))));
    ;}
    break;

  case 69:

/* Line 936 of glr.c  */
#line 764 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl),
        gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)), ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl))));
    ;}
    break;

  case 70:

/* Line 936 of glr.c  */
#line 773 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 71:

/* Line 936 of glr.c  */
#line 777 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBinder(gsMakeForall(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 72:

/* Line 936 of glr.c  */
#line 781 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBinder(gsMakeExists(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 73:

/* Line 936 of glr.c  */
#line 789 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 74:

/* Line 936 of glr.c  */
#line 793 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl),
        gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)), ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl))));
    ;}
    break;

  case 75:

/* Line 936 of glr.c  */
#line 798 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl),
        gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)), ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl))));
    ;}
    break;

  case 76:

/* Line 936 of glr.c  */
#line 807 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 77:

/* Line 936 of glr.c  */
#line 811 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBinder(gsMakeLambda(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 78:

/* Line 936 of glr.c  */
#line 815 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBinder(gsMakeForall(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 79:

/* Line 936 of glr.c  */
#line 819 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBinder(gsMakeExists(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 80:

/* Line 936 of glr.c  */
#line 827 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 81:

/* Line 936 of glr.c  */
#line 831 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl),
        gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)), ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl))));
    ;}
    break;

  case 82:

/* Line 936 of glr.c  */
#line 836 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl),
        gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)), ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl))));
    ;}
    break;

  case 83:

/* Line 936 of glr.c  */
#line 841 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl),
        gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)), ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl))));
    ;}
    break;

  case 84:

/* Line 936 of glr.c  */
#line 846 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl),
        gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)), ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl))));
    ;}
    break;

  case 85:

/* Line 936 of glr.c  */
#line 851 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl),
        gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)), ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl))));
    ;}
    break;

  case 86:

/* Line 936 of glr.c  */
#line 860 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 87:

/* Line 936 of glr.c  */
#line 864 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl),
        gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)), ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl))));
    ;}
    break;

  case 88:

/* Line 936 of glr.c  */
#line 873 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 89:

/* Line 936 of glr.c  */
#line 877 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl),
        gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)), ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl))));
    ;}
    break;

  case 90:

/* Line 936 of glr.c  */
#line 886 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 91:

/* Line 936 of glr.c  */
#line 890 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl),
        gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)), ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl))));
    ;}
    break;

  case 92:

/* Line 936 of glr.c  */
#line 899 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 93:

/* Line 936 of glr.c  */
#line 903 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl),
        gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)), ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl))));
    ;}
    break;

  case 94:

/* Line 936 of glr.c  */
#line 908 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl),
        gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)), ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl))));
    ;}
    break;

  case 95:

/* Line 936 of glr.c  */
#line 917 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 96:

/* Line 936 of glr.c  */
#line 921 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl),
        gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)), ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl))));
    ;}
    break;

  case 97:

/* Line 936 of glr.c  */
#line 926 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl),
        gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)), ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl))));
    ;}
    break;

  case 98:

/* Line 936 of glr.c  */
#line 931 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl),
        gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)), ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl))));
    ;}
    break;

  case 99:

/* Line 936 of glr.c  */
#line 940 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 100:

/* Line 936 of glr.c  */
#line 944 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl),
        gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)), ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl))));
    ;}
    break;

  case 101:

/* Line 936 of glr.c  */
#line 949 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl),
        gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)), ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl))));
    ;}
    break;

  case 102:

/* Line 936 of glr.c  */
#line 958 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 103:

/* Line 936 of glr.c  */
#line 962 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl)), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl))));
    ;}
    break;

  case 104:

/* Line 936 of glr.c  */
#line 966 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl)), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl))));
    ;}
    break;

  case 105:

/* Line 936 of glr.c  */
#line 970 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl)), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl))));
    ;}
    break;

  case 106:

/* Line 936 of glr.c  */
#line 978 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 107:

/* Line 936 of glr.c  */
#line 982 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBinder(gsMakeForall(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 108:

/* Line 936 of glr.c  */
#line 986 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBinder(gsMakeExists(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 109:

/* Line 936 of glr.c  */
#line 994 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 110:

/* Line 936 of glr.c  */
#line 998 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeDataAppl((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.appl), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.list))));
    ;}
    break;

  case 111:

/* Line 936 of glr.c  */
#line 1011 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 112:

/* Line 936 of glr.c  */
#line 1015 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 113:

/* Line 936 of glr.c  */
#line 1023 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 114:

/* Line 936 of glr.c  */
#line 1027 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 115:

/* Line 936 of glr.c  */
#line 1031 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 116:

/* Line 936 of glr.c  */
#line 1035 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 117:

/* Line 936 of glr.c  */
#line 1039 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 118:

/* Line 936 of glr.c  */
#line 1047 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 119:

/* Line 936 of glr.c  */
#line 1051 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 120:

/* Line 936 of glr.c  */
#line 1055 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 121:

/* Line 936 of glr.c  */
#line 1059 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 122:

/* Line 936 of glr.c  */
#line 1063 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeId(mcrl2::data::sort_list::nil_name()));
    ;}
    break;

  case 123:

/* Line 936 of glr.c  */
#line 1067 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeId(mcrl2::data::sort_set::emptyset_name()));
    ;}
    break;

  case 124:

/* Line 936 of glr.c  */
#line 1075 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeDataAppl(gsMakeId(mcrl2::data::sort_list::list_enumeration_name()), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.list))));
    ;}
    break;

  case 125:

/* Line 936 of glr.c  */
#line 1079 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeDataAppl(gsMakeId(mcrl2::data::sort_set::set_enumeration_name()), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.list))));
    ;}
    break;

  case 126:

/* Line 936 of glr.c  */
#line 1083 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeDataAppl(gsMakeId(mcrl2::data::sort_bag::bag_enumeration_name()), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.list))));
    ;}
    break;

  case 127:

/* Line 936 of glr.c  */
#line 1091 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list));
    ;}
    break;

  case 128:

/* Line 936 of glr.c  */
#line 1095 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATconcat((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list)));
    ;}
    break;

  case 129:

/* Line 936 of glr.c  */
#line 1103 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 130:

/* Line 936 of glr.c  */
#line 1111 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBinder(gsMakeSetBagComp(), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.appl)), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 131:

/* Line 936 of glr.c  */
#line 1119 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeDataVarId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 132:

/* Line 936 of glr.c  */
#line 1130 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsDataSpecEltsToSpec(ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list))));
    ;}
    break;

  case 133:

/* Line 936 of glr.c  */
#line 1138 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 134:

/* Line 936 of glr.c  */
#line 1142 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 135:

/* Line 936 of glr.c  */
#line 1150 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 136:

/* Line 936 of glr.c  */
#line 1154 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 137:

/* Line 936 of glr.c  */
#line 1158 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 138:

/* Line 936 of glr.c  */
#line 1162 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 139:

/* Line 936 of glr.c  */
#line 1170 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSortSpec((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.list)));
    ;}
    break;

  case 140:

/* Line 936 of glr.c  */
#line 1178 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.list));
    ;}
    break;

  case 141:

/* Line 936 of glr.c  */
#line 1182 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATconcat((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.list)));
    ;}
    break;

  case 142:

/* Line 936 of glr.c  */
#line 1190 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList0());
      int n = ATgetLength((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list));
      for (int i = 0; i < n; i++) {
        safe_assign(((*yyvalp).list), ATinsert(((*yyvalp).list), (ATerm) gsMakeSortId(ATAelementAt((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list), i))));
      }
    ;}
    break;

  case 143:

/* Line 936 of glr.c  */
#line 1198 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) gsMakeSortRef((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl))));
    ;}
    break;

  case 144:

/* Line 936 of glr.c  */
#line 1206 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 145:

/* Line 936 of glr.c  */
#line 1210 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 146:

/* Line 936 of glr.c  */
#line 1218 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list));
    ;}
    break;

  case 147:

/* Line 936 of glr.c  */
#line 1222 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) gsMakeSortArrow((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl))));
    ;}
    break;

  case 148:

/* Line 936 of glr.c  */
#line 1230 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeConsSpec((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.list)));
    ;}
    break;

  case 149:

/* Line 936 of glr.c  */
#line 1238 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeMapSpec((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.list)));
    ;}
    break;

  case 150:

/* Line 936 of glr.c  */
#line 1247 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.list));
    ;}
    break;

  case 151:

/* Line 936 of glr.c  */
#line 1251 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATconcat((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.list)));
    ;}
    break;

  case 152:

/* Line 936 of glr.c  */
#line 1259 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList0());
      int n = ATgetLength((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list));
      for (int i = 0; i < n; i++) {
        safe_assign(((*yyvalp).list), ATinsert(((*yyvalp).list), (ATerm) gsMakeOpId(ATAelementAt((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), i), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl))));
      }
    ;}
    break;

  case 153:

/* Line 936 of glr.c  */
#line 1271 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeDataEqnSpec((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list)));
    ;}
    break;

  case 154:

/* Line 936 of glr.c  */
#line 1279 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.list)));
    ;}
    break;

  case 155:

/* Line 936 of glr.c  */
#line 1283 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList0());
      int n = ATgetLength((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.list));
      for (int i = 0; i < n; i++) {
        ATermAppl DataEqn = ATAelementAt((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.list), i);
        safe_assign(((*yyvalp).list),
          ATinsert(((*yyvalp).list), (ATerm) gsMakeDataEqn((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), ATAgetArgument(DataEqn, 1),
            ATAgetArgument(DataEqn, 2), ATAgetArgument(DataEqn, 3))));
      }
    ;}
    break;

  case 156:

/* Line 936 of glr.c  */
#line 1298 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 157:

/* Line 936 of glr.c  */
#line 1302 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 158:

/* Line 936 of glr.c  */
#line 1310 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeDataEqn(ATmakeList0(), mcrl2::data::sort_bool::true_(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 159:

/* Line 936 of glr.c  */
#line 1314 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeDataEqn(ATmakeList0(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 160:

/* Line 936 of glr.c  */
#line 1323 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.list));
    ;}
    break;

  case 161:

/* Line 936 of glr.c  */
#line 1327 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATconcat((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.list)));
    ;}
    break;

  case 162:

/* Line 936 of glr.c  */
#line 1338 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeMultAct(ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list))));
    ;}
    break;

  case 163:

/* Line 936 of glr.c  */
#line 1342 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeMultAct(ATmakeList0()));
    ;}
    break;

  case 164:

/* Line 936 of glr.c  */
#line 1350 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 165:

/* Line 936 of glr.c  */
#line 1354 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 166:

/* Line 936 of glr.c  */
#line 1362 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeParamId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl), ATmakeList0()));
    ;}
    break;

  case 167:

/* Line 936 of glr.c  */
#line 1366 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeParamId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.appl), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.list))));
    ;}
    break;

  case 168:

/* Line 936 of glr.c  */
#line 1377 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 169:

/* Line 936 of glr.c  */
#line 1385 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 170:

/* Line 936 of glr.c  */
#line 1389 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeChoice((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 171:

/* Line 936 of glr.c  */
#line 1397 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 172:

/* Line 936 of glr.c  */
#line 1401 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSum((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 173:

/* Line 936 of glr.c  */
#line 1409 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 174:

/* Line 936 of glr.c  */
#line 1413 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeMerge((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 175:

/* Line 936 of glr.c  */
#line 1417 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeLMerge((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 176:

/* Line 936 of glr.c  */
#line 1425 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 177:

/* Line 936 of glr.c  */
#line 1429 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSum((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 178:

/* Line 936 of glr.c  */
#line 1437 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 179:

/* Line 936 of glr.c  */
#line 1441 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBInit((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 180:

/* Line 936 of glr.c  */
#line 1449 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 181:

/* Line 936 of glr.c  */
#line 1453 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSum((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 182:

/* Line 936 of glr.c  */
#line 1461 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 183:

/* Line 936 of glr.c  */
#line 1465 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeIfThen((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 184:

/* Line 936 of glr.c  */
#line 1469 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeIfThenElse((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 185:

/* Line 936 of glr.c  */
#line 1477 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 186:

/* Line 936 of glr.c  */
#line 1481 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSum((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 187:

/* Line 936 of glr.c  */
#line 1489 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 188:

/* Line 936 of glr.c  */
#line 1493 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSeq((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 189:

/* Line 936 of glr.c  */
#line 1501 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 190:

/* Line 936 of glr.c  */
#line 1505 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSeq((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 191:

/* Line 936 of glr.c  */
#line 1513 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 192:

/* Line 936 of glr.c  */
#line 1517 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSum((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 193:

/* Line 936 of glr.c  */
#line 1521 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeIfThen((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 194:

/* Line 936 of glr.c  */
#line 1525 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeIfThenElse((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 195:

/* Line 936 of glr.c  */
#line 1533 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 196:

/* Line 936 of glr.c  */
#line 1537 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSum((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 197:

/* Line 936 of glr.c  */
#line 1545 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 198:

/* Line 936 of glr.c  */
#line 1549 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeAtTime((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 199:

/* Line 936 of glr.c  */
#line 1557 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 200:

/* Line 936 of glr.c  */
#line 1561 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeAtTime((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 201:

/* Line 936 of glr.c  */
#line 1569 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 202:

/* Line 936 of glr.c  */
#line 1573 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSync((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 203:

/* Line 936 of glr.c  */
#line 1581 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 204:

/* Line 936 of glr.c  */
#line 1585 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSync((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 205:

/* Line 936 of glr.c  */
#line 1593 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 206:

/* Line 936 of glr.c  */
#line 1597 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSum((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 207:

/* Line 936 of glr.c  */
#line 1601 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeIfThen((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 208:

/* Line 936 of glr.c  */
#line 1605 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeIfThenElse((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 209:

/* Line 936 of glr.c  */
#line 1613 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 210:

/* Line 936 of glr.c  */
#line 1617 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSum((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 211:

/* Line 936 of glr.c  */
#line 1625 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 212:

/* Line 936 of glr.c  */
#line 1629 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 213:

/* Line 936 of glr.c  */
#line 1633 "mcrl2parser.yy"
    {
      //mcrl3yyerror("process assignments are not yet supported");
      // YYABORT
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 214:

/* Line 936 of glr.c  */
#line 1639 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 215:

/* Line 936 of glr.c  */
#line 1643 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 216:

/* Line 936 of glr.c  */
#line 1651 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeDelta());
    ;}
    break;

  case 217:

/* Line 936 of glr.c  */
#line 1655 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeTau());
    ;}
    break;

  case 218:

/* Line 936 of glr.c  */
#line 1663 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeIdAssignment((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), ATmakeList0()));
    ;}
    break;

  case 219:

/* Line 936 of glr.c  */
#line 1667 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeIdAssignment((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.appl), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.list))));
    ;}
    break;

  case 220:

/* Line 936 of glr.c  */
#line 1675 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBlock((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (6))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (6))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 221:

/* Line 936 of glr.c  */
#line 1679 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeHide((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (6))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (6))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 222:

/* Line 936 of glr.c  */
#line 1683 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeRename((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (6))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (6))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 223:

/* Line 936 of glr.c  */
#line 1687 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeComm((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (6))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (6))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 224:

/* Line 936 of glr.c  */
#line 1691 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeAllow((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (6))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (6))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 225:

/* Line 936 of glr.c  */
#line 1699 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList0());
    ;}
    break;

  case 226:

/* Line 936 of glr.c  */
#line 1703 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.list)));
    ;}
    break;

  case 227:

/* Line 936 of glr.c  */
#line 1711 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList0());
    ;}
    break;

  case 228:

/* Line 936 of glr.c  */
#line 1715 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.list)));
    ;}
    break;

  case 229:

/* Line 936 of glr.c  */
#line 1723 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 230:

/* Line 936 of glr.c  */
#line 1727 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 231:

/* Line 936 of glr.c  */
#line 1735 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeRenameExpr((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 232:

/* Line 936 of glr.c  */
#line 1743 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList0());
    ;}
    break;

  case 233:

/* Line 936 of glr.c  */
#line 1747 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.list)));
    ;}
    break;

  case 234:

/* Line 936 of glr.c  */
#line 1755 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 235:

/* Line 936 of glr.c  */
#line 1759 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 236:

/* Line 936 of glr.c  */
#line 1767 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeCommExpr((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl), gsMakeNil()));
    ;}
    break;

  case 237:

/* Line 936 of glr.c  */
#line 1771 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeCommExpr((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), gsMakeNil()));
    ;}
    break;

  case 238:

/* Line 936 of glr.c  */
#line 1775 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeCommExpr((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 239:

/* Line 936 of glr.c  */
#line 1783 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeMultActName(ATinsert(ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.list)), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl))));
    ;}
    break;

  case 240:

/* Line 936 of glr.c  */
#line 1791 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 241:

/* Line 936 of glr.c  */
#line 1795 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 242:

/* Line 936 of glr.c  */
#line 1803 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList0());
    ;}
    break;

  case 243:

/* Line 936 of glr.c  */
#line 1807 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.list)));
    ;}
    break;

  case 244:

/* Line 936 of glr.c  */
#line 1815 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 245:

/* Line 936 of glr.c  */
#line 1819 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 246:

/* Line 936 of glr.c  */
#line 1827 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeMultActName(ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list))));
    ;}
    break;

  case 247:

/* Line 936 of glr.c  */
#line 1838 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsProcSpecEltsToSpec(ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list))));
    ;}
    break;

  case 248:

/* Line 936 of glr.c  */
#line 1846 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 249:

/* Line 936 of glr.c  */
#line 1850 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 250:

/* Line 936 of glr.c  */
#line 1858 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 251:

/* Line 936 of glr.c  */
#line 1862 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 252:

/* Line 936 of glr.c  */
#line 1866 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 253:

/* Line 936 of glr.c  */
#line 1870 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 254:

/* Line 936 of glr.c  */
#line 1874 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 255:

/* Line 936 of glr.c  */
#line 1882 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActSpec((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.list)));
    ;}
    break;

  case 256:

/* Line 936 of glr.c  */
#line 1890 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.list));
    ;}
    break;

  case 257:

/* Line 936 of glr.c  */
#line 1894 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATconcat((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.list)));
    ;}
    break;

  case 258:

/* Line 936 of glr.c  */
#line 1902 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList0());
      int n = ATgetLength((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list));
      for (int i = 0; i < n; i++) {
        safe_assign(((*yyvalp).list),
          ATinsert(((*yyvalp).list), (ATerm) gsMakeActId(ATAelementAt((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list), i), ATmakeList0())));
      }
    ;}
    break;

  case 259:

/* Line 936 of glr.c  */
#line 1911 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList0());
      int n = ATgetLength((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list));
      for (int i = 0; i < n; i++) {
        safe_assign(((*yyvalp).list), ATinsert(((*yyvalp).list), (ATerm) gsMakeActId(ATAelementAt((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), i), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.list))));
      }
    ;}
    break;

  case 260:

/* Line 936 of glr.c  */
#line 1923 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeGlobVarSpec((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.list)));
    ;}
    break;

  case 261:

/* Line 936 of glr.c  */
#line 1931 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeProcEqnSpec(ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.list))));
    ;}
    break;

  case 262:

/* Line 936 of glr.c  */
#line 1939 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 263:

/* Line 936 of glr.c  */
#line 1943 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 264:

/* Line 936 of glr.c  */
#line 1951 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeProcEqn(
        gsMakeProcVarId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), ATmakeList0()), ATmakeList0(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 265:

/* Line 936 of glr.c  */
#line 1956 "mcrl2parser.yy"
    {
      ATermList SortExprs = ATmakeList0();
      int n = ATgetLength((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (6))].yystate.yysemantics.yysval.list));
      for (int i = 0; i < n; i++) {
        SortExprs = ATinsert(SortExprs, ATgetArgument(ATAelementAt((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (6))].yystate.yysemantics.yysval.list), i), 1));
      }
      safe_assign(((*yyvalp).appl), gsMakeProcEqn(
        gsMakeProcVarId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (6))].yystate.yysemantics.yysval.appl), ATreverse(SortExprs)), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (6))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((6) - (6))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 266:

/* Line 936 of glr.c  */
#line 1970 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeProcessInit((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 267:

/* Line 936 of glr.c  */
#line 1981 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 268:

/* Line 936 of glr.c  */
#line 1989 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 269:

/* Line 936 of glr.c  */
#line 1993 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateForall((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 270:

/* Line 936 of glr.c  */
#line 1997 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateExists((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 271:

/* Line 936 of glr.c  */
#line 2001 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateNu((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 272:

/* Line 936 of glr.c  */
#line 2005 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateMu((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 273:

/* Line 936 of glr.c  */
#line 2013 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList0());
    ;}
    break;

  case 274:

/* Line 936 of glr.c  */
#line 2017 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.list)));
    ;}
    break;

  case 275:

/* Line 936 of glr.c  */
#line 2026 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 276:

/* Line 936 of glr.c  */
#line 2030 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 277:

/* Line 936 of glr.c  */
#line 2038 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeDataVarIdInit(gsMakeDataVarId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.appl)), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 278:

/* Line 936 of glr.c  */
#line 2046 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 279:

/* Line 936 of glr.c  */
#line 2050 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateImp((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 280:

/* Line 936 of glr.c  */
#line 2058 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 281:

/* Line 936 of glr.c  */
#line 2062 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateForall((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 282:

/* Line 936 of glr.c  */
#line 2066 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateExists((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 283:

/* Line 936 of glr.c  */
#line 2070 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateNu((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 284:

/* Line 936 of glr.c  */
#line 2074 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateMu((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 285:

/* Line 936 of glr.c  */
#line 2082 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 286:

/* Line 936 of glr.c  */
#line 2086 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateAnd((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 287:

/* Line 936 of glr.c  */
#line 2090 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateOr((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 288:

/* Line 936 of glr.c  */
#line 2098 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 289:

/* Line 936 of glr.c  */
#line 2102 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateForall((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 290:

/* Line 936 of glr.c  */
#line 2106 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateExists((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 291:

/* Line 936 of glr.c  */
#line 2110 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateNu((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 292:

/* Line 936 of glr.c  */
#line 2114 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateMu((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 293:

/* Line 936 of glr.c  */
#line 2122 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 294:

/* Line 936 of glr.c  */
#line 2126 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateNot((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 295:

/* Line 936 of glr.c  */
#line 2130 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateMust((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 296:

/* Line 936 of glr.c  */
#line 2134 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateMay((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 297:

/* Line 936 of glr.c  */
#line 2138 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateYaledTimed((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 298:

/* Line 936 of glr.c  */
#line 2142 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateDelayTimed((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 299:

/* Line 936 of glr.c  */
#line 2150 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 300:

/* Line 936 of glr.c  */
#line 2154 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateForall((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 301:

/* Line 936 of glr.c  */
#line 2158 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateExists((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 302:

/* Line 936 of glr.c  */
#line 2162 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateNu((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 303:

/* Line 936 of glr.c  */
#line 2166 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateMu((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 304:

/* Line 936 of glr.c  */
#line 2174 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 305:

/* Line 936 of glr.c  */
#line 2178 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateVar(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl), 0), ATLgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl), 1)));
    ;}
    break;

  case 306:

/* Line 936 of glr.c  */
#line 2182 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateTrue());
    ;}
    break;

  case 307:

/* Line 936 of glr.c  */
#line 2186 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateFalse());
    ;}
    break;

  case 308:

/* Line 936 of glr.c  */
#line 2190 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateYaled());
    ;}
    break;

  case 309:

/* Line 936 of glr.c  */
#line 2194 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateDelay());
    ;}
    break;

  case 310:

/* Line 936 of glr.c  */
#line 2198 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 311:

/* Line 936 of glr.c  */
#line 2206 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 312:

/* Line 936 of glr.c  */
#line 2210 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 313:

/* Line 936 of glr.c  */
#line 2218 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 314:

/* Line 936 of glr.c  */
#line 2222 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeRegAlt((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 315:

/* Line 936 of glr.c  */
#line 2230 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 316:

/* Line 936 of glr.c  */
#line 2234 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeRegAlt((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 317:

/* Line 936 of glr.c  */
#line 2242 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 318:

/* Line 936 of glr.c  */
#line 2246 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeRegSeq((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 319:

/* Line 936 of glr.c  */
#line 2254 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 320:

/* Line 936 of glr.c  */
#line 2258 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeRegSeq((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 321:

/* Line 936 of glr.c  */
#line 2266 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 322:

/* Line 936 of glr.c  */
#line 2270 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeRegTransOrNil((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 323:

/* Line 936 of glr.c  */
#line 2274 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeRegTrans((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 324:

/* Line 936 of glr.c  */
#line 2282 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 325:

/* Line 936 of glr.c  */
#line 2286 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeRegTransOrNil((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 326:

/* Line 936 of glr.c  */
#line 2290 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeRegTrans((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 327:

/* Line 936 of glr.c  */
#line 2298 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeRegNil());
    ;}
    break;

  case 328:

/* Line 936 of glr.c  */
#line 2302 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 329:

/* Line 936 of glr.c  */
#line 2310 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 330:

/* Line 936 of glr.c  */
#line 2314 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeRegNil());
    ;}
    break;

  case 331:

/* Line 936 of glr.c  */
#line 2318 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 332:

/* Line 936 of glr.c  */
#line 2326 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 333:

/* Line 936 of glr.c  */
#line 2334 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 334:

/* Line 936 of glr.c  */
#line 2338 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActForall((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 335:

/* Line 936 of glr.c  */
#line 2342 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActExists((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 336:

/* Line 936 of glr.c  */
#line 2350 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 337:

/* Line 936 of glr.c  */
#line 2354 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActImp((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 338:

/* Line 936 of glr.c  */
#line 2362 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 339:

/* Line 936 of glr.c  */
#line 2366 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActForall((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 340:

/* Line 936 of glr.c  */
#line 2370 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActExists((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 341:

/* Line 936 of glr.c  */
#line 2378 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 342:

/* Line 936 of glr.c  */
#line 2382 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActAnd((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 343:

/* Line 936 of glr.c  */
#line 2386 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActOr((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 344:

/* Line 936 of glr.c  */
#line 2394 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 345:

/* Line 936 of glr.c  */
#line 2398 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActForall((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 346:

/* Line 936 of glr.c  */
#line 2402 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActExists((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 347:

/* Line 936 of glr.c  */
#line 2410 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 348:

/* Line 936 of glr.c  */
#line 2414 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActAt((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 349:

/* Line 936 of glr.c  */
#line 2422 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 350:

/* Line 936 of glr.c  */
#line 2426 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActNot((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 351:

/* Line 936 of glr.c  */
#line 2434 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 352:

/* Line 936 of glr.c  */
#line 2438 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActForall((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 353:

/* Line 936 of glr.c  */
#line 2442 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActExists((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 354:

/* Line 936 of glr.c  */
#line 2450 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 355:

/* Line 936 of glr.c  */
#line 2454 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 356:

/* Line 936 of glr.c  */
#line 2458 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActTrue());
    ;}
    break;

  case 357:

/* Line 936 of glr.c  */
#line 2462 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActFalse());
    ;}
    break;

  case 358:

/* Line 936 of glr.c  */
#line 2466 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 359:

/* Line 936 of glr.c  */
#line 2477 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsActionRenameEltsToActionRename(ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list))));
    ;}
    break;

  case 360:

/* Line 936 of glr.c  */
#line 2485 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 361:

/* Line 936 of glr.c  */
#line 2489 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 362:

/* Line 936 of glr.c  */
#line 2497 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 363:

/* Line 936 of glr.c  */
#line 2501 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 364:

/* Line 936 of glr.c  */
#line 2505 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 365:

/* Line 936 of glr.c  */
#line 2513 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActionRenameRules((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list)));
    ;}
    break;

  case 366:

/* Line 936 of glr.c  */
#line 2521 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.list));
    ;}
    break;

  case 367:

/* Line 936 of glr.c  */
#line 2525 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList0());
      int n = ATgetLength((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.list));
      for (int i = 0; i < n; i++) {
        ATermAppl ActionRenameRule = ATAelementAt((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.list), i);
  safe_assign(((*yyvalp).list), ATinsert(((*yyvalp).list),
          (ATerm) gsMakeActionRenameRule((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list),
            ATAgetArgument(ActionRenameRule, 1),
            ATAgetArgument(ActionRenameRule, 2),
            ATAgetArgument(ActionRenameRule, 3))));
      }
    ;}
    break;

  case 368:

/* Line 936 of glr.c  */
#line 2542 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 369:

/* Line 936 of glr.c  */
#line 2546 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 370:

/* Line 936 of glr.c  */
#line 2554 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActionRenameRule(ATmakeList0(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 371:

/* Line 936 of glr.c  */
#line 2558 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActionRenameRule(ATmakeList0(), mcrl2::data::sort_bool::true_(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 372:

/* Line 936 of glr.c  */
#line 2566 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 373:

/* Line 936 of glr.c  */
#line 2570 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 374:

/* Line 936 of glr.c  */
#line 2581 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 375:

/* Line 936 of glr.c  */
#line 2589 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 376:

/* Line 936 of glr.c  */
#line 2593 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakePBESForall((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 377:

/* Line 936 of glr.c  */
#line 2597 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakePBESExists((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 378:

/* Line 936 of glr.c  */
#line 2605 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 379:

/* Line 936 of glr.c  */
#line 2609 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakePBESImp((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 380:

/* Line 936 of glr.c  */
#line 2617 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 381:

/* Line 936 of glr.c  */
#line 2621 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakePBESForall((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 382:

/* Line 936 of glr.c  */
#line 2625 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakePBESExists((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 383:

/* Line 936 of glr.c  */
#line 2633 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 384:

/* Line 936 of glr.c  */
#line 2637 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakePBESAnd((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 385:

/* Line 936 of glr.c  */
#line 2641 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakePBESOr((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 386:

/* Line 936 of glr.c  */
#line 2649 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 387:

/* Line 936 of glr.c  */
#line 2653 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakePBESForall((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 388:

/* Line 936 of glr.c  */
#line 2657 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakePBESExists((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 389:

/* Line 936 of glr.c  */
#line 2665 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 390:

/* Line 936 of glr.c  */
#line 2669 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakePBESNot((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 391:

/* Line 936 of glr.c  */
#line 2677 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 392:

/* Line 936 of glr.c  */
#line 2681 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakePBESForall((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 393:

/* Line 936 of glr.c  */
#line 2685 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakePBESExists((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 394:

/* Line 936 of glr.c  */
#line 2693 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 395:

/* Line 936 of glr.c  */
#line 2697 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakePropVarInst(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl), 0), ATLgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl), 1)));
    ;}
    break;

  case 396:

/* Line 936 of glr.c  */
#line 2701 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakePBESTrue());
    ;}
    break;

  case 397:

/* Line 936 of glr.c  */
#line 2705 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakePBESFalse());
    ;}
    break;

  case 398:

/* Line 936 of glr.c  */
#line 2709 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 399:

/* Line 936 of glr.c  */
#line 2721 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsPBESSpecEltsToSpec(ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list))));
    ;}
    break;

  case 400:

/* Line 936 of glr.c  */
#line 2729 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 401:

/* Line 936 of glr.c  */
#line 2733 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 402:

/* Line 936 of glr.c  */
#line 2741 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 403:

/* Line 936 of glr.c  */
#line 2745 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 404:

/* Line 936 of glr.c  */
#line 2749 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 405:

/* Line 936 of glr.c  */
#line 2753 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 406:

/* Line 936 of glr.c  */
#line 2761 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakePBEqnSpec(ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.list))));
    ;}
    break;

  case 407:

/* Line 936 of glr.c  */
#line 2769 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 408:

/* Line 936 of glr.c  */
#line 2773 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 409:

/* Line 936 of glr.c  */
#line 2781 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl),
        gsMakePBEqn((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.appl), gsMakePropVarDecl((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.appl), ATmakeList0()), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 410:

/* Line 936 of glr.c  */
#line 2786 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl),
        gsMakePBEqn((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (7))].yystate.yysemantics.yysval.appl), gsMakePropVarDecl((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (7))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (7))].yystate.yysemantics.yysval.list)), (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (7))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 411:

/* Line 936 of glr.c  */
#line 2795 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeMu());
    ;}
    break;

  case 412:

/* Line 936 of glr.c  */
#line 2799 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeNu());
    ;}
    break;

  case 413:

/* Line 936 of glr.c  */
#line 2807 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl),
        gsMakePBInit(gsMakePropVarInst(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl), 0), ATLgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl), 1))));
    ;}
    break;



/* Line 936 of glr.c  */
#line 5870 "mcrl2parser.cpp"
      default: break;
    }

  return yyok;
# undef yyerrok
# undef YYABORT
# undef YYACCEPT
# undef YYERROR
# undef YYBACKUP
# undef yyclearin
# undef YYRECOVERING
}


/*ARGSUSED*/ static void
yyuserMerge (int yyn, YYSTYPE* yy0, YYSTYPE* yy1)
{
  YYUSE (yy0);
  YYUSE (yy1);

  switch (yyn)
    {
      
      default: break;
    }
}

			      /* Bison grammar-table manipulation.  */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/** Number of symbols composing the right hand side of rule #RULE.  */
static inline int
yyrhsLength (yyRuleNum yyrule)
{
  return yyr2[yyrule];
}

static void
yydestroyGLRState (char const *yymsg, yyGLRState *yys)
{
  if (yys->yyresolved)
    yydestruct (yymsg, yystos[yys->yylrState],
		&yys->yysemantics.yysval);
  else
    {
#if YYDEBUG
      if (yydebug)
	{
	  if (yys->yysemantics.yyfirstVal)
	    YYFPRINTF (stderr, "%s unresolved ", yymsg);
	  else
	    YYFPRINTF (stderr, "%s incomplete ", yymsg);
	  yy_symbol_print (stderr, yystos[yys->yylrState],
			   NULL);
	  YYFPRINTF (stderr, "\n");
	}
#endif

      if (yys->yysemantics.yyfirstVal)
	{
	  yySemanticOption *yyoption = yys->yysemantics.yyfirstVal;
	  yyGLRState *yyrh;
	  int yyn;
	  for (yyrh = yyoption->yystate, yyn = yyrhsLength (yyoption->yyrule);
	       yyn > 0;
	       yyrh = yyrh->yypred, yyn -= 1)
	    yydestroyGLRState (yymsg, yyrh);
	}
    }
}

/** Left-hand-side symbol for rule #RULE.  */
static inline yySymbol
yylhsNonterm (yyRuleNum yyrule)
{
  return yyr1[yyrule];
}

#define yyis_pact_ninf(yystate) \
  ((yystate) == YYPACT_NINF)

/** True iff LR state STATE has only a default reduction (regardless
 *  of token).  */
static inline yybool
yyisDefaultedState (yyStateNum yystate)
{
  return yyis_pact_ninf (yypact[yystate]);
}

/** The default reduction for STATE, assuming it has one.  */
static inline yyRuleNum
yydefaultAction (yyStateNum yystate)
{
  return yydefact[yystate];
}

#define yyis_table_ninf(yytable_value) \
  YYID (0)

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

/** Return a fresh GLRStackItem.  Callers should call
 * YY_RESERVE_GLRSTACK afterwards to make sure there is sufficient
 * headroom.  */

static inline yyGLRStackItem*
yynewGLRStackItem (yyGLRStack* yystackp, yybool yyisState)
{
  yyGLRStackItem* yynewItem = yystackp->yynextFree;
  yystackp->yyspaceLeft -= 1;
  yystackp->yynextFree += 1;
  yynewItem->yystate.yyisState = yyisState;
  return yynewItem;
}

/** Add a new semantic action that will execute the action for rule
 *  RULENUM on the semantic values in RHS to the list of
 *  alternative actions for STATE.  Assumes that RHS comes from
 *  stack #K of *STACKP. */
static void
yyaddDeferredAction (yyGLRStack* yystackp, size_t yyk, yyGLRState* yystate,
		     yyGLRState* rhs, yyRuleNum yyrule)
{
  yySemanticOption* yynewOption =
    &yynewGLRStackItem (yystackp, yyfalse)->yyoption;
  yynewOption->yystate = rhs;
  yynewOption->yyrule = yyrule;
  if (yystackp->yytops.yylookaheadNeeds[yyk])
    {
      yynewOption->yyrawchar = yychar;
      yynewOption->yyval = yylval;
      yynewOption->yyloc = yylloc;
    }
  else
    yynewOption->yyrawchar = YYEMPTY;
  yynewOption->yynext = yystate->yysemantics.yyfirstVal;
  yystate->yysemantics.yyfirstVal = yynewOption;

  YY_RESERVE_GLRSTACK (yystackp);
}

				/* GLRStacks */

/** Initialize SET to a singleton set containing an empty stack.  */
static yybool
yyinitStateSet (yyGLRStateSet* yyset)
{
  yyset->yysize = 1;
  yyset->yycapacity = 16;
  yyset->yystates = (yyGLRState**) YYMALLOC (16 * sizeof yyset->yystates[0]);
  if (! yyset->yystates)
    return yyfalse;
  yyset->yystates[0] = NULL;
  yyset->yylookaheadNeeds =
    (yybool*) YYMALLOC (16 * sizeof yyset->yylookaheadNeeds[0]);
  if (! yyset->yylookaheadNeeds)
    {
      YYFREE (yyset->yystates);
      return yyfalse;
    }
  return yytrue;
}

static void yyfreeStateSet (yyGLRStateSet* yyset)
{
  YYFREE (yyset->yystates);
  YYFREE (yyset->yylookaheadNeeds);
}

/** Initialize STACK to a single empty stack, with total maximum
 *  capacity for all stacks of SIZE.  */
static yybool
yyinitGLRStack (yyGLRStack* yystackp, size_t yysize)
{
  yystackp->yyerrState = 0;
  yynerrs = 0;
  yystackp->yyspaceLeft = yysize;
  yystackp->yyitems =
    (yyGLRStackItem*) YYMALLOC (yysize * sizeof yystackp->yynextFree[0]);
  if (!yystackp->yyitems)
    return yyfalse;
  yystackp->yynextFree = yystackp->yyitems;
  yystackp->yysplitPoint = NULL;
  yystackp->yylastDeleted = NULL;
  return yyinitStateSet (&yystackp->yytops);
}


#if YYSTACKEXPANDABLE
# define YYRELOC(YYFROMITEMS,YYTOITEMS,YYX,YYTYPE) \
  &((YYTOITEMS) - ((YYFROMITEMS) - (yyGLRStackItem*) (YYX)))->YYTYPE

/** If STACK is expandable, extend it.  WARNING: Pointers into the
    stack from outside should be considered invalid after this call.
    We always expand when there are 1 or fewer items left AFTER an
    allocation, so that we can avoid having external pointers exist
    across an allocation.  */
static void
yyexpandGLRStack (yyGLRStack* yystackp)
{
  yyGLRStackItem* yynewItems;
  yyGLRStackItem* yyp0, *yyp1;
  size_t yysize, yynewSize;
  size_t yyn;
  yysize = yystackp->yynextFree - yystackp->yyitems;
  if (YYMAXDEPTH - YYHEADROOM < yysize)
    yyMemoryExhausted (yystackp);
  yynewSize = 2*yysize;
  if (YYMAXDEPTH < yynewSize)
    yynewSize = YYMAXDEPTH;
  yynewItems = (yyGLRStackItem*) YYMALLOC (yynewSize * sizeof yynewItems[0]);
  if (! yynewItems)
    yyMemoryExhausted (yystackp);
  for (yyp0 = yystackp->yyitems, yyp1 = yynewItems, yyn = yysize;
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
  if (yystackp->yysplitPoint != NULL)
    yystackp->yysplitPoint = YYRELOC (yystackp->yyitems, yynewItems,
				 yystackp->yysplitPoint, yystate);

  for (yyn = 0; yyn < yystackp->yytops.yysize; yyn += 1)
    if (yystackp->yytops.yystates[yyn] != NULL)
      yystackp->yytops.yystates[yyn] =
	YYRELOC (yystackp->yyitems, yynewItems,
		 yystackp->yytops.yystates[yyn], yystate);
  YYFREE (yystackp->yyitems);
  yystackp->yyitems = yynewItems;
  yystackp->yynextFree = yynewItems + yysize;
  yystackp->yyspaceLeft = yynewSize - yysize;
}
#endif

static void
yyfreeGLRStack (yyGLRStack* yystackp)
{
  YYFREE (yystackp->yyitems);
  yyfreeStateSet (&yystackp->yytops);
}

/** Assuming that S is a GLRState somewhere on STACK, update the
 *  splitpoint of STACK, if needed, so that it is at least as deep as
 *  S.  */
static inline void
yyupdateSplit (yyGLRStack* yystackp, yyGLRState* yys)
{
  if (yystackp->yysplitPoint != NULL && yystackp->yysplitPoint > yys)
    yystackp->yysplitPoint = yys;
}

/** Invalidate stack #K in STACK.  */
static inline void
yymarkStackDeleted (yyGLRStack* yystackp, size_t yyk)
{
  if (yystackp->yytops.yystates[yyk] != NULL)
    yystackp->yylastDeleted = yystackp->yytops.yystates[yyk];
  yystackp->yytops.yystates[yyk] = NULL;
}

/** Undelete the last stack that was marked as deleted.  Can only be
    done once after a deletion, and only when all other stacks have
    been deleted.  */
static void
yyundeleteLastStack (yyGLRStack* yystackp)
{
  if (yystackp->yylastDeleted == NULL || yystackp->yytops.yysize != 0)
    return;
  yystackp->yytops.yystates[0] = yystackp->yylastDeleted;
  yystackp->yytops.yysize = 1;
  YYDPRINTF ((stderr, "Restoring last deleted stack as stack #0.\n"));
  yystackp->yylastDeleted = NULL;
}

static inline void
yyremoveDeletes (yyGLRStack* yystackp)
{
  size_t yyi, yyj;
  yyi = yyj = 0;
  while (yyj < yystackp->yytops.yysize)
    {
      if (yystackp->yytops.yystates[yyi] == NULL)
	{
	  if (yyi == yyj)
	    {
	      YYDPRINTF ((stderr, "Removing dead stacks.\n"));
	    }
	  yystackp->yytops.yysize -= 1;
	}
      else
	{
	  yystackp->yytops.yystates[yyj] = yystackp->yytops.yystates[yyi];
	  /* In the current implementation, it's unnecessary to copy
	     yystackp->yytops.yylookaheadNeeds[yyi] since, after
	     yyremoveDeletes returns, the parser immediately either enters
	     deterministic operation or shifts a token.  However, it doesn't
	     hurt, and the code might evolve to need it.  */
	  yystackp->yytops.yylookaheadNeeds[yyj] =
	    yystackp->yytops.yylookaheadNeeds[yyi];
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
 * LRSTATE, at input position POSN, with (resolved) semantic value SVAL.  */
static inline void
yyglrShift (yyGLRStack* yystackp, size_t yyk, yyStateNum yylrState,
	    size_t yyposn,
	    YYSTYPE* yyvalp, YYLTYPE* yylocp)
{
  yyGLRState* yynewState = &yynewGLRStackItem (yystackp, yytrue)->yystate;

  yynewState->yylrState = yylrState;
  yynewState->yyposn = yyposn;
  yynewState->yyresolved = yytrue;
  yynewState->yypred = yystackp->yytops.yystates[yyk];
  yynewState->yysemantics.yysval = *yyvalp;
  yynewState->yyloc = *yylocp;
  yystackp->yytops.yystates[yyk] = yynewState;

  YY_RESERVE_GLRSTACK (yystackp);
}

/** Shift stack #K of YYSTACK, to a new state corresponding to LR
 *  state YYLRSTATE, at input position YYPOSN, with the (unresolved)
 *  semantic value of YYRHS under the action for YYRULE.  */
static inline void
yyglrShiftDefer (yyGLRStack* yystackp, size_t yyk, yyStateNum yylrState,
		 size_t yyposn, yyGLRState* rhs, yyRuleNum yyrule)
{
  yyGLRState* yynewState = &yynewGLRStackItem (yystackp, yytrue)->yystate;

  yynewState->yylrState = yylrState;
  yynewState->yyposn = yyposn;
  yynewState->yyresolved = yyfalse;
  yynewState->yypred = yystackp->yytops.yystates[yyk];
  yynewState->yysemantics.yyfirstVal = NULL;
  yystackp->yytops.yystates[yyk] = yynewState;

  /* Invokes YY_RESERVE_GLRSTACK.  */
  yyaddDeferredAction (yystackp, yyk, yynewState, rhs, yyrule);
}

/** Pop the symbols consumed by reduction #RULE from the top of stack
 *  #K of STACK, and perform the appropriate semantic action on their
 *  semantic values.  Assumes that all ambiguities in semantic values
 *  have been previously resolved.  Set *VALP to the resulting value,
 *  and *LOCP to the computed location (if any).  Return value is as
 *  for userAction.  */
static inline YYRESULTTAG
yydoAction (yyGLRStack* yystackp, size_t yyk, yyRuleNum yyrule,
	    YYSTYPE* yyvalp, YYLTYPE* yylocp)
{
  int yynrhs = yyrhsLength (yyrule);

  if (yystackp->yysplitPoint == NULL)
    {
      /* Standard special case: single stack.  */
      yyGLRStackItem* rhs = (yyGLRStackItem*) yystackp->yytops.yystates[yyk];
      YYASSERT (yyk == 0);
      yystackp->yynextFree -= yynrhs;
      yystackp->yyspaceLeft += yynrhs;
      yystackp->yytops.yystates[0] = & yystackp->yynextFree[-1].yystate;
      return yyuserAction (yyrule, yynrhs, rhs,
			   yyvalp, yylocp, yystackp);
    }
  else
    {
      /* At present, doAction is never called in nondeterministic
       * mode, so this branch is never taken.  It is here in
       * anticipation of a future feature that will allow immediate
       * evaluation of selected actions in nondeterministic mode.  */
      int yyi;
      yyGLRState* yys;
      yyGLRStackItem yyrhsVals[YYMAXRHS + YYMAXLEFT + 1];
      yys = yyrhsVals[YYMAXRHS + YYMAXLEFT].yystate.yypred
	= yystackp->yytops.yystates[yyk];
      for (yyi = 0; yyi < yynrhs; yyi += 1)
	{
	  yys = yys->yypred;
	  YYASSERT (yys);
	}
      yyupdateSplit (yystackp, yys);
      yystackp->yytops.yystates[yyk] = yys;
      return yyuserAction (yyrule, yynrhs, yyrhsVals + YYMAXRHS + YYMAXLEFT - 1,
			   yyvalp, yylocp, yystackp);
    }
}

#if !YYDEBUG
# define YY_REDUCE_PRINT(Args)
#else
# define YY_REDUCE_PRINT(Args)		\
do {					\
  if (yydebug)				\
    yy_reduce_print Args;		\
} while (YYID (0))

/*----------------------------------------------------------.
| Report that the RULE is going to be reduced on stack #K.  |
`----------------------------------------------------------*/

/*ARGSUSED*/ static inline void
yy_reduce_print (yyGLRStack* yystackp, size_t yyk, yyRuleNum yyrule,
		 YYSTYPE* yyvalp, YYLTYPE* yylocp)
{
  int yynrhs = yyrhsLength (yyrule);
  yybool yynormal __attribute__ ((__unused__)) =
    (yystackp->yysplitPoint == NULL);
  yyGLRStackItem* yyvsp = (yyGLRStackItem*) yystackp->yytops.yystates[yyk];
  int yylow = 1;
  int yyi;
  YYUSE (yyvalp);
  YYUSE (yylocp);
  YYFPRINTF (stderr, "Reducing stack %lu by rule %d (line %lu):\n",
	     (unsigned long int) yyk, yyrule - 1,
	     (unsigned long int) yyrline[yyrule]);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(((yyGLRStackItem const *)yyvsp)[YYFILL ((yyi + 1) - (yynrhs))].yystate.yysemantics.yysval)
		       		       );
      YYFPRINTF (stderr, "\n");
    }
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
 *  the STACK.  In this case, the (necessarily deferred) semantic value is
 *  added to the options for the existing state's semantic value.
 */
static inline YYRESULTTAG
yyglrReduce (yyGLRStack* yystackp, size_t yyk, yyRuleNum yyrule,
	     yybool yyforceEval)
{
  size_t yyposn = yystackp->yytops.yystates[yyk]->yyposn;

  if (yyforceEval || yystackp->yysplitPoint == NULL)
    {
      YYSTYPE yysval;
      YYLTYPE yyloc;

      YY_REDUCE_PRINT ((yystackp, yyk, yyrule, &yysval, &yyloc));
      YYCHK (yydoAction (yystackp, yyk, yyrule, &yysval,
			 &yyloc));
      YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyrule], &yysval, &yyloc);
      yyglrShift (yystackp, yyk,
		  yyLRgotoState (yystackp->yytops.yystates[yyk]->yylrState,
				 yylhsNonterm (yyrule)),
		  yyposn, &yysval, &yyloc);
    }
  else
    {
      size_t yyi;
      int yyn;
      yyGLRState* yys, *yys0 = yystackp->yytops.yystates[yyk];
      yyStateNum yynewLRState;

      for (yys = yystackp->yytops.yystates[yyk], yyn = yyrhsLength (yyrule);
	   0 < yyn; yyn -= 1)
	{
	  yys = yys->yypred;
	  YYASSERT (yys);
	}
      yyupdateSplit (yystackp, yys);
      yynewLRState = yyLRgotoState (yys->yylrState, yylhsNonterm (yyrule));
      YYDPRINTF ((stderr,
		  "Reduced stack %lu by rule #%d; action deferred.  Now in state %d.\n",
		  (unsigned long int) yyk, yyrule - 1, yynewLRState));
      for (yyi = 0; yyi < yystackp->yytops.yysize; yyi += 1)
	if (yyi != yyk && yystackp->yytops.yystates[yyi] != NULL)
	  {
	    yyGLRState* yyp, *yysplit = yystackp->yysplitPoint;
	    yyp = yystackp->yytops.yystates[yyi];
	    while (yyp != yys && yyp != yysplit && yyp->yyposn >= yyposn)
	      {
		if (yyp->yylrState == yynewLRState && yyp->yypred == yys)
		  {
		    yyaddDeferredAction (yystackp, yyk, yyp, yys0, yyrule);
		    yymarkStackDeleted (yystackp, yyk);
		    YYDPRINTF ((stderr, "Merging stack %lu into stack %lu.\n",
				(unsigned long int) yyk,
				(unsigned long int) yyi));
		    return yyok;
		  }
		yyp = yyp->yypred;
	      }
	  }
      yystackp->yytops.yystates[yyk] = yys;
      yyglrShiftDefer (yystackp, yyk, yynewLRState, yyposn, yys0, yyrule);
    }
  return yyok;
}

static size_t
yysplitStack (yyGLRStack* yystackp, size_t yyk)
{
  if (yystackp->yysplitPoint == NULL)
    {
      YYASSERT (yyk == 0);
      yystackp->yysplitPoint = yystackp->yytops.yystates[yyk];
    }
  if (yystackp->yytops.yysize >= yystackp->yytops.yycapacity)
    {
      yyGLRState** yynewStates;
      yybool* yynewLookaheadNeeds;

      yynewStates = NULL;

      if (yystackp->yytops.yycapacity
	  > (YYSIZEMAX / (2 * sizeof yynewStates[0])))
	yyMemoryExhausted (yystackp);
      yystackp->yytops.yycapacity *= 2;

      yynewStates =
	(yyGLRState**) YYREALLOC (yystackp->yytops.yystates,
				  (yystackp->yytops.yycapacity
				   * sizeof yynewStates[0]));
      if (yynewStates == NULL)
	yyMemoryExhausted (yystackp);
      yystackp->yytops.yystates = yynewStates;

      yynewLookaheadNeeds =
	(yybool*) YYREALLOC (yystackp->yytops.yylookaheadNeeds,
			     (yystackp->yytops.yycapacity
			      * sizeof yynewLookaheadNeeds[0]));
      if (yynewLookaheadNeeds == NULL)
	yyMemoryExhausted (yystackp);
      yystackp->yytops.yylookaheadNeeds = yynewLookaheadNeeds;
    }
  yystackp->yytops.yystates[yystackp->yytops.yysize]
    = yystackp->yytops.yystates[yyk];
  yystackp->yytops.yylookaheadNeeds[yystackp->yytops.yysize]
    = yystackp->yytops.yylookaheadNeeds[yyk];
  yystackp->yytops.yysize += 1;
  return yystackp->yytops.yysize-1;
}

/** True iff Y0 and Y1 represent identical options at the top level.
 *  That is, they represent the same rule applied to RHS symbols
 *  that produce the same terminal symbols.  */
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

/** Assuming identicalOptions (Y0,Y1), destructively merge the
 *  alternative semantic values for the RHS-symbols of Y1 and Y0.  */
static void
yymergeOptionSets (yySemanticOption* yyy0, yySemanticOption* yyy1)
{
  yyGLRState *yys0, *yys1;
  int yyn;
  for (yys0 = yyy0->yystate, yys1 = yyy1->yystate,
       yyn = yyrhsLength (yyy0->yyrule);
       yyn > 0;
       yys0 = yys0->yypred, yys1 = yys1->yypred, yyn -= 1)
    {
      if (yys0 == yys1)
	break;
      else if (yys0->yyresolved)
	{
	  yys1->yyresolved = yytrue;
	  yys1->yysemantics.yysval = yys0->yysemantics.yysval;
	}
      else if (yys1->yyresolved)
	{
	  yys0->yyresolved = yytrue;
	  yys0->yysemantics.yysval = yys1->yysemantics.yysval;
	}
      else
	{
	  yySemanticOption** yyz0p;
	  yySemanticOption* yyz1;
	  yyz0p = &yys0->yysemantics.yyfirstVal;
	  yyz1 = yys1->yysemantics.yyfirstVal;
	  while (YYID (yytrue))
	    {
	      if (yyz1 == *yyz0p || yyz1 == NULL)
		break;
	      else if (*yyz0p == NULL)
		{
		  *yyz0p = yyz1;
		  break;
		}
	      else if (*yyz0p < yyz1)
		{
		  yySemanticOption* yyz = *yyz0p;
		  *yyz0p = yyz1;
		  yyz1 = yyz1->yynext;
		  (*yyz0p)->yynext = yyz;
		}
	      yyz0p = &(*yyz0p)->yynext;
	    }
	  yys1->yysemantics.yyfirstVal = yys0->yysemantics.yyfirstVal;
	}
    }
}

/** Y0 and Y1 represent two possible actions to take in a given
 *  parsing state; return 0 if no combination is possible,
 *  1 if user-mergeable, 2 if Y0 is preferred, 3 if Y1 is preferred.  */
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

static YYRESULTTAG yyresolveValue (yyGLRState* yys,
				   yyGLRStack* yystackp);


/** Resolve the previous N states starting at and including state S.  If result
 *  != yyok, some states may have been left unresolved possibly with empty
 *  semantic option chains.  Regardless of whether result = yyok, each state
 *  has been left with consistent data so that yydestroyGLRState can be invoked
 *  if necessary.  */
static YYRESULTTAG
yyresolveStates (yyGLRState* yys, int yyn,
		 yyGLRStack* yystackp)
{
  if (0 < yyn)
    {
      YYASSERT (yys->yypred);
      YYCHK (yyresolveStates (yys->yypred, yyn-1, yystackp));
      if (! yys->yyresolved)
	YYCHK (yyresolveValue (yys, yystackp));
    }
  return yyok;
}

/** Resolve the states for the RHS of OPT, perform its user action, and return
 *  the semantic value and location.  Regardless of whether result = yyok, all
 *  RHS states have been destroyed (assuming the user action destroys all RHS
 *  semantic values if invoked).  */
static YYRESULTTAG
yyresolveAction (yySemanticOption* yyopt, yyGLRStack* yystackp,
		 YYSTYPE* yyvalp, YYLTYPE* yylocp)
{
  yyGLRStackItem yyrhsVals[YYMAXRHS + YYMAXLEFT + 1];
  int yynrhs;
  int yychar_current;
  YYSTYPE yylval_current;
  YYLTYPE yylloc_current;
  YYRESULTTAG yyflag;

  yynrhs = yyrhsLength (yyopt->yyrule);
  yyflag = yyresolveStates (yyopt->yystate, yynrhs, yystackp);
  if (yyflag != yyok)
    {
      yyGLRState *yys;
      for (yys = yyopt->yystate; yynrhs > 0; yys = yys->yypred, yynrhs -= 1)
	yydestroyGLRState ("Cleanup: popping", yys);
      return yyflag;
    }

  yyrhsVals[YYMAXRHS + YYMAXLEFT].yystate.yypred = yyopt->yystate;
  yychar_current = yychar;
  yylval_current = yylval;
  yylloc_current = yylloc;
  yychar = yyopt->yyrawchar;
  yylval = yyopt->yyval;
  yylloc = yyopt->yyloc;
  yyflag = yyuserAction (yyopt->yyrule, yynrhs,
			   yyrhsVals + YYMAXRHS + YYMAXLEFT - 1,
			   yyvalp, yylocp, yystackp);
  yychar = yychar_current;
  yylval = yylval_current;
  yylloc = yylloc_current;
  return yyflag;
}

#if YYDEBUG
static void
yyreportTree (yySemanticOption* yyx, int yyindent)
{
  int yynrhs = yyrhsLength (yyx->yyrule);
  int yyi;
  yyGLRState* yys;
  yyGLRState* yystates[1 + YYMAXRHS];
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
	       yyx->yyrule - 1);
  else
    YYFPRINTF (stderr, "%*s%s -> <Rule %d, tokens %lu .. %lu>\n",
	       yyindent, "", yytokenName (yylhsNonterm (yyx->yyrule)),
	       yyx->yyrule - 1, (unsigned long int) (yys->yyposn + 1),
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

/*ARGSUSED*/ static YYRESULTTAG
yyreportAmbiguity (yySemanticOption* yyx0,
		   yySemanticOption* yyx1)
{
  YYUSE (yyx0);
  YYUSE (yyx1);

#if YYDEBUG
  YYFPRINTF (stderr, "Ambiguity detected.\n");
  YYFPRINTF (stderr, "Option 1,\n");
  yyreportTree (yyx0, 2);
  YYFPRINTF (stderr, "\nOption 2,\n");
  yyreportTree (yyx1, 2);
  YYFPRINTF (stderr, "\n");
#endif

  yyerror (YY_("syntax is ambiguous"));
  return yyabort;
}

/** Starting at and including state S1, resolve the location for each of the
 *  previous N1 states that is unresolved.  The first semantic option of a state
 *  is always chosen.  */
static void
yyresolveLocations (yyGLRState* yys1, int yyn1,
		    yyGLRStack *yystackp)
{
  if (0 < yyn1)
    {
      yyresolveLocations (yys1->yypred, yyn1 - 1, yystackp);
      if (!yys1->yyresolved)
	{
	  yySemanticOption *yyoption;
	  yyGLRStackItem yyrhsloc[1 + YYMAXRHS];
	  int yynrhs;
	  int yychar_current;
	  YYSTYPE yylval_current;
	  YYLTYPE yylloc_current;
	  yyoption = yys1->yysemantics.yyfirstVal;
	  YYASSERT (yyoption != NULL);
	  yynrhs = yyrhsLength (yyoption->yyrule);
	  if (yynrhs > 0)
	    {
	      yyGLRState *yys;
	      int yyn;
	      yyresolveLocations (yyoption->yystate, yynrhs,
				  yystackp);
	      for (yys = yyoption->yystate, yyn = yynrhs;
		   yyn > 0;
		   yys = yys->yypred, yyn -= 1)
		yyrhsloc[yyn].yystate.yyloc = yys->yyloc;
	    }
	  else
	    {
	      /* Both yyresolveAction and yyresolveLocations traverse the GSS
		 in reverse rightmost order.  It is only necessary to invoke
		 yyresolveLocations on a subforest for which yyresolveAction
		 would have been invoked next had an ambiguity not been
		 detected.  Thus the location of the previous state (but not
		 necessarily the previous state itself) is guaranteed to be
		 resolved already.  */
	      yyGLRState *yyprevious = yyoption->yystate;
	      yyrhsloc[0].yystate.yyloc = yyprevious->yyloc;
	    }
	  yychar_current = yychar;
	  yylval_current = yylval;
	  yylloc_current = yylloc;
	  yychar = yyoption->yyrawchar;
	  yylval = yyoption->yyval;
	  yylloc = yyoption->yyloc;
	  YYLLOC_DEFAULT ((yys1->yyloc), yyrhsloc, yynrhs);
	  yychar = yychar_current;
	  yylval = yylval_current;
	  yylloc = yylloc_current;
	}
    }
}

/** Resolve the ambiguity represented in state S, perform the indicated
 *  actions, and set the semantic value of S.  If result != yyok, the chain of
 *  semantic options in S has been cleared instead or it has been left
 *  unmodified except that redundant options may have been removed.  Regardless
 *  of whether result = yyok, S has been left with consistent data so that
 *  yydestroyGLRState can be invoked if necessary.  */
static YYRESULTTAG
yyresolveValue (yyGLRState* yys, yyGLRStack* yystackp)
{
  yySemanticOption* yyoptionList = yys->yysemantics.yyfirstVal;
  yySemanticOption* yybest;
  yySemanticOption** yypp;
  yybool yymerge;
  YYSTYPE yysval;
  YYRESULTTAG yyflag;
  YYLTYPE *yylocp = &yys->yyloc;

  yybest = yyoptionList;
  yymerge = yyfalse;
  for (yypp = &yyoptionList->yynext; *yypp != NULL; )
    {
      yySemanticOption* yyp = *yypp;

      if (yyidenticalOptions (yybest, yyp))
	{
	  yymergeOptionSets (yybest, yyp);
	  *yypp = yyp->yynext;
	}
      else
	{
	  switch (yypreference (yybest, yyp))
	    {
	    case 0:
	      yyresolveLocations (yys, 1, yystackp);
	      return yyreportAmbiguity (yybest, yyp);
	      break;
	    case 1:
	      yymerge = yytrue;
	      break;
	    case 2:
	      break;
	    case 3:
	      yybest = yyp;
	      yymerge = yyfalse;
	      break;
	    default:
	      /* This cannot happen so it is not worth a YYASSERT (yyfalse),
		 but some compilers complain if the default case is
		 omitted.  */
	      break;
	    }
	  yypp = &yyp->yynext;
	}
    }

  if (yymerge)
    {
      yySemanticOption* yyp;
      int yyprec = yydprec[yybest->yyrule];
      yyflag = yyresolveAction (yybest, yystackp, &yysval,
				yylocp);
      if (yyflag == yyok)
	for (yyp = yybest->yynext; yyp != NULL; yyp = yyp->yynext)
	  {
	    if (yyprec == yydprec[yyp->yyrule])
	      {
		YYSTYPE yysval_other;
		YYLTYPE yydummy;
		yyflag = yyresolveAction (yyp, yystackp, &yysval_other,
					  &yydummy);
		if (yyflag != yyok)
		  {
		    yydestruct ("Cleanup: discarding incompletely merged value for",
				yystos[yys->yylrState],
				&yysval);
		    break;
		  }
		yyuserMerge (yymerger[yyp->yyrule], &yysval, &yysval_other);
	      }
	  }
    }
  else
    yyflag = yyresolveAction (yybest, yystackp, &yysval, yylocp);

  if (yyflag == yyok)
    {
      yys->yyresolved = yytrue;
      yys->yysemantics.yysval = yysval;
    }
  else
    yys->yysemantics.yyfirstVal = NULL;
  return yyflag;
}

static YYRESULTTAG
yyresolveStack (yyGLRStack* yystackp)
{
  if (yystackp->yysplitPoint != NULL)
    {
      yyGLRState* yys;
      int yyn;

      for (yyn = 0, yys = yystackp->yytops.yystates[0];
	   yys != yystackp->yysplitPoint;
	   yys = yys->yypred, yyn += 1)
	continue;
      YYCHK (yyresolveStates (yystackp->yytops.yystates[0], yyn, yystackp
			     ));
    }
  return yyok;
}

static void
yycompressStack (yyGLRStack* yystackp)
{
  yyGLRState* yyp, *yyq, *yyr;

  if (yystackp->yytops.yysize != 1 || yystackp->yysplitPoint == NULL)
    return;

  for (yyp = yystackp->yytops.yystates[0], yyq = yyp->yypred, yyr = NULL;
       yyp != yystackp->yysplitPoint;
       yyr = yyp, yyp = yyq, yyq = yyp->yypred)
    yyp->yypred = yyr;

  yystackp->yyspaceLeft += yystackp->yynextFree - yystackp->yyitems;
  yystackp->yynextFree = ((yyGLRStackItem*) yystackp->yysplitPoint) + 1;
  yystackp->yyspaceLeft -= yystackp->yynextFree - yystackp->yyitems;
  yystackp->yysplitPoint = NULL;
  yystackp->yylastDeleted = NULL;

  while (yyr != NULL)
    {
      yystackp->yynextFree->yystate = *yyr;
      yyr = yyr->yypred;
      yystackp->yynextFree->yystate.yypred = &yystackp->yynextFree[-1].yystate;
      yystackp->yytops.yystates[0] = &yystackp->yynextFree->yystate;
      yystackp->yynextFree += 1;
      yystackp->yyspaceLeft -= 1;
    }
}

static YYRESULTTAG
yyprocessOneStack (yyGLRStack* yystackp, size_t yyk,
		   size_t yyposn)
{
  int yyaction;
  const short int* yyconflicts;
  yyRuleNum yyrule;

  while (yystackp->yytops.yystates[yyk] != NULL)
    {
      yyStateNum yystate = yystackp->yytops.yystates[yyk]->yylrState;
      YYDPRINTF ((stderr, "Stack %lu Entering state %d\n",
		  (unsigned long int) yyk, yystate));

      YYASSERT (yystate != YYFINAL);

      if (yyisDefaultedState (yystate))
	{
	  yyrule = yydefaultAction (yystate);
	  if (yyrule == 0)
	    {
	      YYDPRINTF ((stderr, "Stack %lu dies.\n",
			  (unsigned long int) yyk));
	      yymarkStackDeleted (yystackp, yyk);
	      return yyok;
	    }
	  YYCHK (yyglrReduce (yystackp, yyk, yyrule, yyfalse));
	}
      else
	{
	  yySymbol yytoken;
	  yystackp->yytops.yylookaheadNeeds[yyk] = yytrue;
	  if (yychar == YYEMPTY)
	    {
	      YYDPRINTF ((stderr, "Reading a token: "));
	      yychar = YYLEX;
	    }

	  if (yychar <= YYEOF)
	    {
	      yychar = yytoken = YYEOF;
	      YYDPRINTF ((stderr, "Now at end of input.\n"));
	    }
	  else
	    {
	      yytoken = YYTRANSLATE (yychar);
	      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
	    }

	  yygetLRActions (yystate, yytoken, &yyaction, &yyconflicts);

	  while (*yyconflicts != 0)
	    {
	      size_t yynewStack = yysplitStack (yystackp, yyk);
	      YYDPRINTF ((stderr, "Splitting off stack %lu from %lu.\n",
			  (unsigned long int) yynewStack,
			  (unsigned long int) yyk));
	      YYCHK (yyglrReduce (yystackp, yynewStack,
				  *yyconflicts, yyfalse));
	      YYCHK (yyprocessOneStack (yystackp, yynewStack,
					yyposn));
	      yyconflicts += 1;
	    }

	  if (yyisShiftAction (yyaction))
	    break;
	  else if (yyisErrorAction (yyaction))
	    {
	      YYDPRINTF ((stderr, "Stack %lu dies.\n",
			  (unsigned long int) yyk));
	      yymarkStackDeleted (yystackp, yyk);
	      break;
	    }
	  else
	    YYCHK (yyglrReduce (yystackp, yyk, -yyaction,
				yyfalse));
	}
    }
  return yyok;
}

/*ARGSUSED*/ static void
yyreportSyntaxError (yyGLRStack* yystackp)
{
  if (yystackp->yyerrState == 0)
    {
#if YYERROR_VERBOSE
      int yyn;
      yyn = yypact[yystackp->yytops.yystates[0]->yylrState];
      if (YYPACT_NINF < yyn && yyn <= YYLAST)
	{
	  yySymbol yytoken = YYTRANSLATE (yychar);
	  size_t yysize0 = yytnamerr (NULL, yytokenName (yytoken));
	  size_t yysize = yysize0;
	  size_t yysize1;
	  yybool yysize_overflow = yyfalse;
	  char* yymsg = NULL;
	  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
	  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
	  int yyx;
	  char *yyfmt;
	  char const *yyf;
	  static char const yyunexpected[] = "syntax error, unexpected %s";
	  static char const yyexpecting[] = ", expecting %s";
	  static char const yyor[] = " or %s";
	  char yyformat[sizeof yyunexpected
			+ sizeof yyexpecting - 1
			+ ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
			   * (sizeof yyor - 1))];
	  char const *yyprefix = yyexpecting;

	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  int yyxbegin = yyn < 0 ? -yyn : 0;

	  /* Stay within bounds of both yycheck and yytname.  */
	  int yychecklim = YYLAST - yyn + 1;
	  int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
	  int yycount = 1;

	  yyarg[0] = yytokenName (yytoken);
	  yyfmt = yystpcpy (yyformat, yyunexpected);

	  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      {
		if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
		  {
		    yycount = 1;
		    yysize = yysize0;
		    yyformat[sizeof yyunexpected - 1] = '\0';
		    break;
		  }
		yyarg[yycount++] = yytokenName (yyx);
		yysize1 = yysize + yytnamerr (NULL, yytokenName (yyx));
		yysize_overflow |= yysize1 < yysize;
		yysize = yysize1;
		yyfmt = yystpcpy (yyfmt, yyprefix);
		yyprefix = yyor;
	      }

	  yyf = YY_(yyformat);
	  yysize1 = yysize + strlen (yyf);
	  yysize_overflow |= yysize1 < yysize;
	  yysize = yysize1;

	  if (!yysize_overflow)
	    yymsg = (char *) YYMALLOC (yysize);

	  if (yymsg)
	    {
	      char *yyp = yymsg;
	      int yyi = 0;
	      while ((*yyp = *yyf))
		{
		  if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		    {
		      yyp += yytnamerr (yyp, yyarg[yyi++]);
		      yyf += 2;
		    }
		  else
		    {
		      yyp++;
		      yyf++;
		    }
		}
	      yyerror (yymsg);
	      YYFREE (yymsg);
	    }
	  else
	    {
	      yyerror (YY_("syntax error"));
	      yyMemoryExhausted (yystackp);
	    }
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror (YY_("syntax error"));
      yynerrs += 1;
    }
}

/* Recover from a syntax error on *YYSTACKP, assuming that *YYSTACKP->YYTOKENP,
   yylval, and yylloc are the syntactic category, semantic value, and location
   of the lookahead.  */
/*ARGSUSED*/ static void
yyrecoverSyntaxError (yyGLRStack* yystackp)
{
  size_t yyk;
  int yyj;

  if (yystackp->yyerrState == 3)
    /* We just shifted the error token and (perhaps) took some
       reductions.  Skip tokens until we can proceed.  */
    while (YYID (yytrue))
      {
	yySymbol yytoken;
	if (yychar == YYEOF)
	  yyFail (yystackp, NULL);
	if (yychar != YYEMPTY)
	  {
	    yytoken = YYTRANSLATE (yychar);
	    yydestruct ("Error: discarding",
			yytoken, &yylval);
	  }
	YYDPRINTF ((stderr, "Reading a token: "));
	yychar = YYLEX;
	if (yychar <= YYEOF)
	  {
	    yychar = yytoken = YYEOF;
	    YYDPRINTF ((stderr, "Now at end of input.\n"));
	  }
	else
	  {
	    yytoken = YYTRANSLATE (yychar);
	    YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
	  }
	yyj = yypact[yystackp->yytops.yystates[0]->yylrState];
	if (yyis_pact_ninf (yyj))
	  return;
	yyj += yytoken;
	if (yyj < 0 || YYLAST < yyj || yycheck[yyj] != yytoken)
	  {
	    if (yydefact[yystackp->yytops.yystates[0]->yylrState] != 0)
	      return;
	  }
	else if (yytable[yyj] != 0 && ! yyis_table_ninf (yytable[yyj]))
	  return;
      }

  /* Reduce to one stack.  */
  for (yyk = 0; yyk < yystackp->yytops.yysize; yyk += 1)
    if (yystackp->yytops.yystates[yyk] != NULL)
      break;
  if (yyk >= yystackp->yytops.yysize)
    yyFail (yystackp, NULL);
  for (yyk += 1; yyk < yystackp->yytops.yysize; yyk += 1)
    yymarkStackDeleted (yystackp, yyk);
  yyremoveDeletes (yystackp);
  yycompressStack (yystackp);

  /* Now pop stack until we find a state that shifts the error token.  */
  yystackp->yyerrState = 3;
  while (yystackp->yytops.yystates[0] != NULL)
    {
      yyGLRState *yys = yystackp->yytops.yystates[0];
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
			       &yylval, &yyerrloc);
	      yyglrShift (yystackp, 0, yytable[yyj],
			  yys->yyposn, &yylval, &yyerrloc);
	      yys = yystackp->yytops.yystates[0];
	      break;
	    }
	}

      if (yys->yypred != NULL)
	yydestroyGLRState ("Error: popping", yys);
      yystackp->yytops.yystates[0] = yys->yypred;
      yystackp->yynextFree -= 1;
      yystackp->yyspaceLeft += 1;
    }
  if (yystackp->yytops.yystates[0] == NULL)
    yyFail (yystackp, NULL);
}

#define YYCHK1(YYE)							     \
  do {									     \
    switch (YYE) {							     \
    case yyok:								     \
      break;								     \
    case yyabort:							     \
      goto yyabortlab;							     \
    case yyaccept:							     \
      goto yyacceptlab;							     \
    case yyerr:								     \
      goto yyuser_error;						     \
    default:								     \
      goto yybuglab;							     \
    }									     \
  } while (YYID (0))


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
  int yyresult;
  yyGLRStack yystack;
  yyGLRStack* const yystackp = &yystack;
  size_t yyposn;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY;
  yylval = yyval_default;


  if (! yyinitGLRStack (yystackp, YYINITDEPTH))
    goto yyexhaustedlab;
  switch (YYSETJMP (yystack.yyexception_buffer))
    {
    case 0: break;
    case 1: goto yyabortlab;
    case 2: goto yyexhaustedlab;
    default: goto yybuglab;
    }
  yyglrShift (&yystack, 0, 0, 0, &yylval, &yylloc);
  yyposn = 0;

  while (YYID (yytrue))
    {
      /* For efficiency, we have two loops, the first of which is
	 specialized to deterministic operation (single stack, no
	 potential ambiguity).  */
      /* Standard mode */
      while (YYID (yytrue))
	{
	  yyRuleNum yyrule;
	  int yyaction;
	  const short int* yyconflicts;

	  yyStateNum yystate = yystack.yytops.yystates[0]->yylrState;
	  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
	  if (yystate == YYFINAL)
	    goto yyacceptlab;
	  if (yyisDefaultedState (yystate))
	    {
	      yyrule = yydefaultAction (yystate);
	      if (yyrule == 0)
		{

		  yyreportSyntaxError (&yystack);
		  goto yyuser_error;
		}
	      YYCHK1 (yyglrReduce (&yystack, 0, yyrule, yytrue));
	    }
	  else
	    {
	      yySymbol yytoken;
	      if (yychar == YYEMPTY)
		{
		  YYDPRINTF ((stderr, "Reading a token: "));
		  yychar = YYLEX;
		}

	      if (yychar <= YYEOF)
		{
		  yychar = yytoken = YYEOF;
		  YYDPRINTF ((stderr, "Now at end of input.\n"));
		}
	      else
		{
		  yytoken = YYTRANSLATE (yychar);
		  YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
		}

	      yygetLRActions (yystate, yytoken, &yyaction, &yyconflicts);
	      if (*yyconflicts != 0)
		break;
	      if (yyisShiftAction (yyaction))
		{
		  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
		  yychar = YYEMPTY;
		  yyposn += 1;
		  yyglrShift (&yystack, 0, yyaction, yyposn, &yylval, &yylloc);
		  if (0 < yystack.yyerrState)
		    yystack.yyerrState -= 1;
		}
	      else if (yyisErrorAction (yyaction))
		{

		  yyreportSyntaxError (&yystack);
		  goto yyuser_error;
		}
	      else
		YYCHK1 (yyglrReduce (&yystack, 0, -yyaction, yytrue));
	    }
	}

      while (YYID (yytrue))
	{
	  yySymbol yytoken_to_shift;
	  size_t yys;

	  for (yys = 0; yys < yystack.yytops.yysize; yys += 1)
	    yystackp->yytops.yylookaheadNeeds[yys] = yychar != YYEMPTY;

	  /* yyprocessOneStack returns one of three things:

	      - An error flag.  If the caller is yyprocessOneStack, it
		immediately returns as well.  When the caller is finally
		yyparse, it jumps to an error label via YYCHK1.

	      - yyok, but yyprocessOneStack has invoked yymarkStackDeleted
		(&yystack, yys), which sets the top state of yys to NULL.  Thus,
		yyparse's following invocation of yyremoveDeletes will remove
		the stack.

	      - yyok, when ready to shift a token.

	     Except in the first case, yyparse will invoke yyremoveDeletes and
	     then shift the next token onto all remaining stacks.  This
	     synchronization of the shift (that is, after all preceding
	     reductions on all stacks) helps prevent double destructor calls
	     on yylval in the event of memory exhaustion.  */

	  for (yys = 0; yys < yystack.yytops.yysize; yys += 1)
	    YYCHK1 (yyprocessOneStack (&yystack, yys, yyposn));
	  yyremoveDeletes (&yystack);
	  if (yystack.yytops.yysize == 0)
	    {
	      yyundeleteLastStack (&yystack);
	      if (yystack.yytops.yysize == 0)
		yyFail (&yystack, YY_("syntax error"));
	      YYCHK1 (yyresolveStack (&yystack));
	      YYDPRINTF ((stderr, "Returning to deterministic operation.\n"));

	      yyreportSyntaxError (&yystack);
	      goto yyuser_error;
	    }

	  /* If any yyglrShift call fails, it will fail after shifting.  Thus,
	     a copy of yylval will already be on stack 0 in the event of a
	     failure in the following loop.  Thus, yychar is set to YYEMPTY
	     before the loop to make sure the user destructor for yylval isn't
	     called twice.  */
	  yytoken_to_shift = YYTRANSLATE (yychar);
	  yychar = YYEMPTY;
	  yyposn += 1;
	  for (yys = 0; yys < yystack.yytops.yysize; yys += 1)
	    {
	      int yyaction;
	      const short int* yyconflicts;
	      yyStateNum yystate = yystack.yytops.yystates[yys]->yylrState;
	      yygetLRActions (yystate, yytoken_to_shift, &yyaction,
			      &yyconflicts);
	      /* Note that yyconflicts were handled by yyprocessOneStack.  */
	      YYDPRINTF ((stderr, "On stack %lu, ", (unsigned long int) yys));
	      YY_SYMBOL_PRINT ("shifting", yytoken_to_shift, &yylval, &yylloc);
	      yyglrShift (&yystack, yys, yyaction, yyposn,
			  &yylval, &yylloc);
	      YYDPRINTF ((stderr, "Stack %lu now in state #%d\n",
			  (unsigned long int) yys,
			  yystack.yytops.yystates[yys]->yylrState));
	    }

	  if (yystack.yytops.yysize == 1)
	    {
	      YYCHK1 (yyresolveStack (&yystack));
	      YYDPRINTF ((stderr, "Returning to deterministic operation.\n"));
	      yycompressStack (&yystack);
	      break;
	    }
	}
      continue;
    yyuser_error:
      yyrecoverSyntaxError (&yystack);
      yyposn = yystack.yytops.yystates[0]->yyposn;
    }

 yyacceptlab:
  yyresult = 0;
  goto yyreturn;

 yybuglab:
  YYASSERT (yyfalse);
  goto yyabortlab;

 yyabortlab:
  yyresult = 1;
  goto yyreturn;

 yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturn;

 yyreturn:
  if (yychar != YYEMPTY)
    yydestruct ("Cleanup: discarding lookahead",
		YYTRANSLATE (yychar),
		&yylval);

  /* If the stack is well-formed, pop the stack until it is empty,
     destroying its entries as we go.  But free the stack regardless
     of whether it is well-formed.  */
  if (yystack.yyitems)
    {
      yyGLRState** yystates = yystack.yytops.yystates;
      if (yystates)
	{
	  size_t yysize = yystack.yytops.yysize;
	  size_t yyk;
	  for (yyk = 0; yyk < yysize; yyk += 1)
	    if (yystates[yyk])
	      {
		while (yystates[yyk])
		  {
		    yyGLRState *yys = yystates[yyk];
		    if (yys->yypred != NULL)
		      yydestroyGLRState ("Cleanup: popping", yys);
		    yystates[yyk] = yys->yypred;
		    yystack.yynextFree -= 1;
		    yystack.yyspaceLeft += 1;
		  }
		break;
	      }
	}
      yyfreeGLRStack (&yystack);
    }

  /* Make sure YYID is used.  */
  return YYID (yyresult);
}

/* DEBUGGING ONLY */
#if YYDEBUG
static void yypstack (yyGLRStack* yystackp, size_t yyk)
  __attribute__ ((__unused__));
static void yypdumpstack (yyGLRStack* yystackp) __attribute__ ((__unused__));

static void
yy_yypstack (yyGLRState* yys)
{
  if (yys->yypred)
    {
      yy_yypstack (yys->yypred);
      YYFPRINTF (stderr, " -> ");
    }
  YYFPRINTF (stderr, "%d@%lu", yys->yylrState,
             (unsigned long int) yys->yyposn);
}

static void
yypstates (yyGLRState* yyst)
{
  if (yyst == NULL)
    YYFPRINTF (stderr, "<null>");
  else
    yy_yypstack (yyst);
  YYFPRINTF (stderr, "\n");
}

static void
yypstack (yyGLRStack* yystackp, size_t yyk)
{
  yypstates (yystackp->yytops.yystates[yyk]);
}

#define YYINDEX(YYX)							     \
    ((YYX) == NULL ? -1 : (yyGLRStackItem*) (YYX) - yystackp->yyitems)


static void
yypdumpstack (yyGLRStack* yystackp)
{
  yyGLRStackItem* yyp;
  size_t yyi;
  for (yyp = yystackp->yyitems; yyp < yystackp->yynextFree; yyp += 1)
    {
      YYFPRINTF (stderr, "%3lu. ",
                 (unsigned long int) (yyp - yystackp->yyitems));
      if (*(yybool *) yyp)
	{
	  YYFPRINTF (stderr, "Res: %d, LR State: %d, posn: %lu, pred: %ld",
		     yyp->yystate.yyresolved, yyp->yystate.yylrState,
		     (unsigned long int) yyp->yystate.yyposn,
		     (long int) YYINDEX (yyp->yystate.yypred));
	  if (! yyp->yystate.yyresolved)
	    YYFPRINTF (stderr, ", firstVal: %ld",
		       (long int) YYINDEX (yyp->yystate
                                             .yysemantics.yyfirstVal));
	}
      else
	{
	  YYFPRINTF (stderr, "Option. rule: %d, state: %ld, next: %ld",
		     yyp->yyoption.yyrule - 1,
		     (long int) YYINDEX (yyp->yyoption.yystate),
		     (long int) YYINDEX (yyp->yyoption.yynext));
	}
      YYFPRINTF (stderr, "\n");
    }
  YYFPRINTF (stderr, "Tops:");
  for (yyi = 0; yyi < yystackp->yytops.yysize; yyi += 1)
    YYFPRINTF (stderr, "%lu: %ld; ", (unsigned long int) yyi,
	       (long int) YYINDEX (yystackp->yytops.yystates[yyi]));
  YYFPRINTF (stderr, "\n");
}
#endif



/* Line 2634 of glr.c  */
#line 2814 "mcrl2parser.yy"


//Uncomment the lines below to enable the use of SIZE_MAX
//#define __STDC_LIMIT_MACROS 1
//#include <stdint.h>

//Uncomment the line below to enable the use of std::cerr, std::cout and std::endl;
//#include <iostream>

