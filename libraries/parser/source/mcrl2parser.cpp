/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison GLR parsers in C

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

/* C GLR parser skeleton written by Paul Hilfinger.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

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
#define yylloc  mcrl2yylloc



#include "mcrl2parser.hpp"

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


/* Line 234 of glr.c.  */
#line 97 "mcrl2parser.cpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

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
#define YYFINAL  130
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1419

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  89
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  138
/* YYNRULES -- Number of rules.  */
#define YYNRULES  335
/* YYNRULES -- Number of states.  */
#define YYNSTATES  710
/* YYMAXRHS -- Maximum number of symbols on right-hand side of rule.  */
#define YYMAXRHS 6
/* YYMAXLEFT -- Maximum number of symbols to the left of a handle
   accessed by $0, $-1, etc., in any rule.  */
#define YYMAXLEFT 0

/* YYTRANSLATE(X) -- Bison symbol number corresponding to X.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   343

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
      85,    86,    87,    88
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short int yyprhs[] =
{
       0,     0,     3,     6,     9,    12,    15,    18,    21,    23,
      25,    29,    31,    33,    37,    39,    41,    44,    46,    50,
      53,    59,    60,    63,    65,    69,    71,    75,    77,    79,
      81,    85,    87,    89,    91,    93,    95,   100,   105,   110,
     112,   114,   119,   121,   125,   129,   131,   136,   141,   146,
     148,   152,   154,   159,   164,   166,   170,   174,   176,   181,
     186,   188,   192,   196,   198,   203,   208,   210,   214,   218,
     222,   226,   230,   232,   236,   238,   242,   244,   248,   250,
     254,   258,   260,   264,   268,   270,   274,   278,   280,   283,
     286,   289,   291,   296,   301,   303,   308,   310,   314,   316,
     318,   320,   322,   326,   328,   330,   332,   334,   336,   340,
     344,   348,   350,   354,   358,   364,   368,   370,   372,   374,
     378,   380,   385,   387,   389,   393,   395,   400,   402,   406,
     410,   412,   417,   419,   423,   425,   430,   432,   436,   442,
     444,   449,   451,   455,   457,   461,   463,   468,   472,   478,
     480,   485,   487,   491,   493,   497,   499,   503,   505,   509,
     511,   516,   520,   526,   528,   533,   535,   537,   539,   543,
     545,   547,   554,   561,   568,   575,   582,   584,   588,   590,
     594,   596,   600,   602,   606,   610,   612,   616,   618,   622,
     624,   628,   632,   634,   636,   640,   642,   646,   648,   652,
     654,   656,   659,   661,   663,   665,   667,   669,   671,   673,
     676,   679,   683,   685,   689,   691,   695,   698,   701,   704,
     708,   712,   714,   717,   722,   725,   729,   733,   739,   742,
     746,   750,   753,   756,   760,   762,   766,   769,   772,   776,
     780,   787,   789,   793,   797,   799,   801,   806,   811,   817,
     823,   824,   828,   830,   834,   840,   842,   846,   848,   853,
     858,   864,   870,   872,   876,   880,   882,   887,   892,   898,
     904,   906,   909,   914,   919,   923,   927,   929,   934,   939,
     945,   951,   956,   958,   960,   962,   964,   966,   970,   972,
     974,   976,   980,   982,   986,   988,   992,   994,   998,  1000,
    1003,  1006,  1008,  1011,  1014,  1016,  1020,  1022,  1024,  1028,
    1030,  1032,  1037,  1042,  1044,  1048,  1050,  1055,  1060,  1062,
    1066,  1070,  1072,  1077,  1082,  1084,  1088,  1090,  1093,  1095,
    1100,  1105,  1107,  1112,  1114,  1116
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const short int yyrhs[] =
{
      90,     0,    -1,     3,    91,    -1,     4,   105,    -1,     5,
     134,    -1,     6,   137,    -1,     7,   171,    -1,     8,   196,
      -1,    92,    -1,    96,    -1,    93,    10,    92,    -1,    94,
      -1,    95,    -1,    94,    34,    95,    -1,    96,    -1,   102,
      -1,    54,    97,    -1,    98,    -1,    97,    35,    98,    -1,
      87,    99,    -1,    87,    36,   100,    37,    99,    -1,    -1,
      31,    87,    -1,   101,    -1,   100,    28,   101,    -1,    91,
      -1,    87,    29,    91,    -1,    87,    -1,   103,    -1,   104,
      -1,    36,    91,    37,    -1,    55,    -1,    56,    -1,    57,
      -1,    58,    -1,    59,    -1,    60,    36,    91,    37,    -1,
      61,    36,    91,    37,    -1,    62,    36,    91,    37,    -1,
     106,    -1,   109,    -1,   106,    71,   107,    72,    -1,   108,
      -1,   107,    28,   108,    -1,    87,    26,   105,    -1,   110,
      -1,    68,   194,    27,   109,    -1,    69,   194,    27,   109,
      -1,    70,   194,    27,   109,    -1,   112,    -1,   112,    20,
     111,    -1,   110,    -1,    69,   194,    27,   111,    -1,    70,
     194,    27,   111,    -1,   114,    -1,   114,    18,   113,    -1,
     114,    19,   113,    -1,   112,    -1,    69,   194,    27,   113,
      -1,    70,   194,    27,   113,    -1,   116,    -1,   116,    16,
     115,    -1,   116,    17,   115,    -1,   114,    -1,    69,   194,
      27,   115,    -1,    70,   194,    27,   115,    -1,   117,    -1,
     117,    12,   117,    -1,   117,    11,   117,    -1,   117,    42,
     117,    -1,   117,    41,   117,    -1,   117,    67,   117,    -1,
     118,    -1,   120,    13,   117,    -1,   119,    -1,   118,    14,
     120,    -1,   120,    -1,   119,    15,   120,    -1,   121,    -1,
     120,    24,   121,    -1,   120,    25,   121,    -1,   122,    -1,
     121,    65,   122,    -1,   121,    66,   122,    -1,   123,    -1,
     122,    23,   123,    -1,   122,    27,   123,    -1,   125,    -1,
      32,   124,    -1,    25,   123,    -1,    34,   123,    -1,   123,
      -1,    69,   194,    27,   124,    -1,    70,   194,    27,   124,
      -1,   127,    -1,   125,    36,   126,    37,    -1,   105,    -1,
     126,    28,   105,    -1,    87,    -1,   128,    -1,   129,    -1,
     132,    -1,    36,   105,    37,    -1,    63,    -1,    64,    -1,
      88,    -1,    38,    -1,    43,    -1,    39,   126,    40,    -1,
      44,   126,    45,    -1,    44,   130,    45,    -1,   131,    -1,
     130,    28,   131,    -1,   105,    29,   105,    -1,    44,   133,
      35,   105,    45,    -1,    87,    29,    91,    -1,   135,    -1,
      74,    -1,   136,    -1,   135,    35,   136,    -1,    87,    -1,
      87,    36,   126,    37,    -1,   138,    -1,   139,    -1,   139,
      24,   138,    -1,   140,    -1,    75,   194,    27,   139,    -1,
     142,    -1,   142,    19,   141,    -1,   142,     9,   141,    -1,
     140,    -1,    75,   194,    27,   141,    -1,   144,    -1,   142,
      21,   143,    -1,   144,    -1,    75,   194,    27,   143,    -1,
     146,    -1,   123,    10,   145,    -1,   123,    10,   149,    22,
     145,    -1,   144,    -1,    75,   194,    27,   145,    -1,   150,
      -1,   150,    27,   148,    -1,   151,    -1,   150,    27,   149,
      -1,   146,    -1,    75,   194,    27,   148,    -1,   123,    10,
     148,    -1,   123,    10,   149,    22,   148,    -1,   147,    -1,
      75,   194,    27,   149,    -1,   152,    -1,   150,    33,   123,
      -1,   153,    -1,   151,    33,   123,    -1,   156,    -1,   156,
      35,   154,    -1,   156,    -1,   156,    35,   155,    -1,   152,
      -1,    75,   194,    27,   154,    -1,   123,    10,   154,    -1,
     123,    10,   155,    22,   154,    -1,   153,    -1,    75,   194,
      27,   155,    -1,   157,    -1,   136,    -1,   158,    -1,    36,
     137,    37,    -1,    73,    -1,    74,    -1,    76,    36,   159,
      28,   137,    37,    -1,    78,    36,   159,    28,   137,    37,
      -1,    79,    36,   161,    28,   137,    37,    -1,    80,    36,
     164,    28,   137,    37,    -1,    77,    36,   169,    28,   137,
      37,    -1,    43,    -1,    44,   160,    45,    -1,    87,    -1,
     160,    28,    87,    -1,    43,    -1,    44,   162,    45,    -1,
     163,    -1,   162,    28,   163,    -1,    87,    10,    87,    -1,
      43,    -1,    44,   165,    45,    -1,   166,    -1,   165,    28,
     166,    -1,   167,    -1,   167,    10,    74,    -1,   167,    10,
      87,    -1,   168,    -1,    87,    -1,   168,    35,    87,    -1,
      43,    -1,    44,   170,    45,    -1,   167,    -1,   170,    28,
     167,    -1,   172,    -1,   173,    -1,   172,   173,    -1,   174,
      -1,   178,    -1,   179,    -1,   182,    -1,   188,    -1,   191,
      -1,   195,    -1,    46,   175,    -1,   176,    30,    -1,   175,
     176,    30,    -1,   160,    -1,    87,    26,    91,    -1,    93,
      -1,    93,    10,    91,    -1,    47,   180,    -1,    48,   180,
      -1,   181,    30,    -1,   180,   181,    30,    -1,   160,    29,
      91,    -1,   183,    -1,    50,   184,    -1,    49,   186,    50,
     184,    -1,   185,    30,    -1,   184,   185,    30,    -1,   105,
      26,   105,    -1,   105,    10,   105,    26,   105,    -1,   187,
      30,    -1,   186,   187,    30,    -1,   160,    29,    91,    -1,
      51,   189,    -1,   190,    30,    -1,   189,   190,    30,    -1,
     160,    -1,   160,    29,   177,    -1,    52,   192,    -1,   193,
      30,    -1,   192,   193,    30,    -1,    87,    26,   137,    -1,
      87,    36,   194,    37,    26,   137,    -1,   187,    -1,   194,
      28,   187,    -1,    53,   137,    30,    -1,   197,    -1,   201,
      -1,    69,   194,    27,   197,    -1,    70,   194,    27,   197,
      -1,    83,    87,   198,    27,   197,    -1,    82,    87,   198,
      27,   197,    -1,    -1,    36,   199,    37,    -1,   200,    -1,
     199,    28,   200,    -1,    87,    29,    91,    26,   105,    -1,
     203,    -1,   203,    20,   202,    -1,   201,    -1,    69,   194,
      27,   202,    -1,    70,   194,    27,   202,    -1,    83,    87,
     198,    27,   202,    -1,    82,    87,   198,    27,   202,    -1,
     205,    -1,   205,    18,   204,    -1,   205,    19,   204,    -1,
     203,    -1,    69,   194,    27,   204,    -1,    70,   194,    27,
     204,    -1,    83,    87,   198,    27,   204,    -1,    82,    87,
     198,    27,   204,    -1,   207,    -1,    32,   206,    -1,    39,
     208,    40,   206,    -1,    41,   208,    42,   206,    -1,    85,
      33,   123,    -1,    84,    33,   123,    -1,   205,    -1,    69,
     194,    27,   206,    -1,    70,   194,    27,   206,    -1,    83,
      87,   198,    27,   206,    -1,    82,    87,   198,    27,   206,
      -1,    81,    36,   105,    37,    -1,   136,    -1,    63,    -1,
      64,    -1,    85,    -1,    84,    -1,    36,   196,    37,    -1,
     217,    -1,   209,    -1,   211,    -1,   212,    24,   210,    -1,
     212,    -1,   212,    24,   210,    -1,   213,    -1,   214,    27,
     212,    -1,   214,    -1,   214,    27,   212,    -1,   215,    -1,
     214,    23,    -1,   214,    24,    -1,   216,    -1,   214,    23,
      -1,   214,    24,    -1,    86,    -1,    36,   209,    37,    -1,
     217,    -1,    86,    -1,    36,   209,    37,    -1,   218,    -1,
     219,    -1,    69,   194,    27,   218,    -1,    70,   194,    27,
     218,    -1,   221,    -1,   221,    20,   220,    -1,   219,    -1,
      69,   194,    27,   220,    -1,    70,   194,    27,   220,    -1,
     223,    -1,   224,    18,   222,    -1,   224,    19,   222,    -1,
     221,    -1,    69,   194,    27,   222,    -1,    70,   194,    27,
     222,    -1,   224,    -1,   223,    33,   123,    -1,   226,    -1,
      32,   225,    -1,   224,    -1,    69,   194,    27,   225,    -1,
      70,   194,    27,   225,    -1,   134,    -1,    81,    36,   105,
      37,    -1,    63,    -1,    64,    -1,    36,   217,    37,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,   150,   150,   155,   160,   165,   170,   175,   187,   196,
     200,   209,   218,   223,   232,   241,   245,   254,   259,   268,
     273,   283,   287,   296,   301,   310,   315,   324,   329,   334,
     339,   347,   351,   355,   359,   363,   371,   375,   379,   390,
     399,   403,   412,   417,   426,   435,   439,   444,   449,   458,
     462,   472,   476,   481,   490,   494,   500,   510,   514,   519,
     528,   532,   538,   548,   552,   557,   566,   570,   576,   582,
     588,   594,   604,   608,   618,   622,   632,   636,   646,   650,
     656,   666,   670,   676,   686,   690,   696,   706,   710,   715,
     720,   729,   733,   738,   747,   751,   760,   765,   774,   779,
     783,   787,   791,   799,   804,   809,   814,   819,   828,   833,
     838,   847,   852,   861,   870,   879,   891,   896,   905,   910,
     919,   924,   936,   945,   949,   958,   962,   971,   975,   980,
     989,   993,  1002,  1006,  1015,  1019,  1028,  1032,  1037,  1046,
    1050,  1059,  1063,  1072,  1076,  1085,  1089,  1094,  1099,  1108,
    1112,  1121,  1125,  1134,  1138,  1147,  1151,  1160,  1164,  1173,
    1177,  1182,  1187,  1196,  1200,  1209,  1213,  1217,  1221,  1229,
    1234,  1243,  1248,  1253,  1258,  1263,  1272,  1277,  1286,  1291,
    1300,  1305,  1314,  1319,  1328,  1337,  1342,  1351,  1356,  1365,
    1370,  1375,  1384,  1393,  1398,  1407,  1412,  1421,  1426,  1438,
    1447,  1452,  1461,  1466,  1471,  1476,  1481,  1486,  1491,  1500,
    1509,  1514,  1523,  1532,  1541,  1546,  1555,  1564,  1574,  1579,
    1588,  1601,  1610,  1615,  1630,  1635,  1644,  1649,  1659,  1664,
    1673,  1686,  1695,  1700,  1709,  1719,  1732,  1741,  1746,  1755,
    1761,  1777,  1782,  1791,  1803,  1812,  1816,  1821,  1826,  1831,
    1841,  1845,  1855,  1860,  1869,  1878,  1882,  1891,  1895,  1900,
    1905,  1910,  1919,  1923,  1928,  1937,  1941,  1946,  1951,  1956,
    1965,  1969,  1974,  1979,  1984,  1989,  1998,  2002,  2007,  2012,
    2017,  2026,  2031,  2036,  2041,  2046,  2051,  2056,  2064,  2069,
    2078,  2082,  2091,  2095,  2104,  2108,  2117,  2121,  2130,  2134,
    2139,  2148,  2152,  2157,  2166,  2171,  2179,  2184,  2189,  2197,
    2206,  2210,  2215,  2224,  2228,  2237,  2241,  2246,  2255,  2259,
    2264,  2273,  2277,  2282,  2291,  2295,  2304,  2308,  2317,  2321,
    2326,  2335,  2340,  2345,  2350,  2355
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "TAG_SORT_EXPR", "TAG_DATA_EXPR",
  "TAG_MULT_ACT", "TAG_PROC_EXPR", "TAG_SPEC", "TAG_STATE_FRM", "LMERGE",
  "ARROW", "LTE", "GTE", "CONS", "SNOC", "CONCAT", "EQ", "NEQ", "AND",
  "BARS", "IMP", "BINIT", "ELSE", "STAR", "PLUS", "MINUS", "EQUALS", "DOT",
  "COMMA", "COLON", "SEMICOLON", "QMARK", "EXCLAM", "AT", "HASH", "BAR",
  "LPAR", "RPAR", "PBRACK", "LBRACK", "RBRACK", "LANG", "RANG", "PBRACE",
  "LBRACE", "RBRACE", "KWSORT", "KWCONS", "KWMAP", "KWVAR", "KWEQN",
  "KWACT", "KWPROC", "KWINIT", "KWSTRUCT", "BOOL", "POS", "NAT", "INT",
  "REAL", "LIST", "SET", "BAG", "CTRUE", "CFALSE", "DIV", "MOD", "IN",
  "LAMBDA", "FORALL", "EXISTS", "WHR", "END", "DELTA", "TAU", "SUM",
  "BLOCK", "ALLOW", "HIDE", "RENAME", "COMM", "VAL", "MU", "NU", "DELAY",
  "YALED", "NIL", "ID", "NUMBER", "$accept", "start", "sort_expr",
  "sort_expr_arrow", "domain_no_arrow", "domain_no_arrow_elts_hs",
  "domain_no_arrow_elt", "sort_expr_struct", "struct_constructors_bs",
  "struct_constructor", "recogniser", "struct_projections_cs",
  "struct_projection", "sort_expr_primary", "sort_constant",
  "sort_constructor", "data_expr", "data_expr_whr", "whr_decls_cs",
  "whr_decl", "data_expr_quant", "data_expr_imp", "data_expr_imp_rhs",
  "data_expr_and", "data_expr_and_rhs", "data_expr_eq", "data_expr_eq_rhs",
  "data_expr_rel", "data_expr_cons", "data_expr_snoc", "data_expr_concat",
  "data_expr_add", "data_expr_div", "data_expr_mult", "data_expr_prefix",
  "data_expr_quant_prefix", "data_expr_postfix", "data_exprs_cs",
  "data_expr_primary", "data_constant", "data_enumeration",
  "bag_enum_elts_cs", "bag_enum_elt", "data_comprehension",
  "data_var_decl", "mult_act", "param_ids_bs", "param_id", "proc_expr",
  "proc_expr_choice", "proc_expr_sum", "proc_expr_merge",
  "proc_expr_merge_rhs", "proc_expr_binit", "proc_expr_binit_rhs",
  "proc_expr_cond", "proc_expr_cond_la", "proc_expr_seq",
  "proc_expr_seq_wo_cond", "proc_expr_seq_rhs",
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
  "act_frm_quant_prefix", "act_frm_primary", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    89,    90,    90,    90,    90,    90,    90,    91,    92,
      92,    93,    94,    94,    95,    96,    96,    97,    97,    98,
      98,    99,    99,   100,   100,   101,   101,   102,   102,   102,
     102,   103,   103,   103,   103,   103,   104,   104,   104,   105,
     106,   106,   107,   107,   108,   109,   109,   109,   109,   110,
     110,   111,   111,   111,   112,   112,   112,   113,   113,   113,
     114,   114,   114,   115,   115,   115,   116,   116,   116,   116,
     116,   116,   117,   117,   118,   118,   119,   119,   120,   120,
     120,   121,   121,   121,   122,   122,   122,   123,   123,   123,
     123,   124,   124,   124,   125,   125,   126,   126,   127,   127,
     127,   127,   127,   128,   128,   128,   128,   128,   129,   129,
     129,   130,   130,   131,   132,   133,   134,   134,   135,   135,
     136,   136,   137,   138,   138,   139,   139,   140,   140,   140,
     141,   141,   142,   142,   143,   143,   144,   144,   144,   145,
     145,   146,   146,   147,   147,   148,   148,   148,   148,   149,
     149,   150,   150,   151,   151,   152,   152,   153,   153,   154,
     154,   154,   154,   155,   155,   156,   156,   156,   156,   157,
     157,   158,   158,   158,   158,   158,   159,   159,   160,   160,
     161,   161,   162,   162,   163,   164,   164,   165,   165,   166,
     166,   166,   167,   168,   168,   169,   169,   170,   170,   171,
     172,   172,   173,   173,   173,   173,   173,   173,   173,   174,
     175,   175,   176,   176,   177,   177,   178,   179,   180,   180,
     181,   182,   183,   183,   184,   184,   185,   185,   186,   186,
     187,   188,   189,   189,   190,   190,   191,   192,   192,   193,
     193,   194,   194,   195,   196,   197,   197,   197,   197,   197,
     198,   198,   199,   199,   200,   201,   201,   202,   202,   202,
     202,   202,   203,   203,   203,   204,   204,   204,   204,   204,
     205,   205,   205,   205,   205,   205,   206,   206,   206,   206,
     206,   207,   207,   207,   207,   207,   207,   207,   208,   208,
     209,   209,   210,   210,   211,   211,   212,   212,   213,   213,
     213,   214,   214,   214,   215,   215,   216,   216,   216,   217,
     218,   218,   218,   219,   219,   220,   220,   220,   221,   221,
     221,   222,   222,   222,   223,   223,   224,   224,   225,   225,
     225,   226,   226,   226,   226,   226
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     1,     1,
       3,     1,     1,     3,     1,     1,     2,     1,     3,     2,
       5,     0,     2,     1,     3,     1,     3,     1,     1,     1,
       3,     1,     1,     1,     1,     1,     4,     4,     4,     1,
       1,     4,     1,     3,     3,     1,     4,     4,     4,     1,
       3,     1,     4,     4,     1,     3,     3,     1,     4,     4,
       1,     3,     3,     1,     4,     4,     1,     3,     3,     3,
       3,     3,     1,     3,     1,     3,     1,     3,     1,     3,
       3,     1,     3,     3,     1,     3,     3,     1,     2,     2,
       2,     1,     4,     4,     1,     4,     1,     3,     1,     1,
       1,     1,     3,     1,     1,     1,     1,     1,     3,     3,
       3,     1,     3,     3,     5,     3,     1,     1,     1,     3,
       1,     4,     1,     1,     3,     1,     4,     1,     3,     3,
       1,     4,     1,     3,     1,     4,     1,     3,     5,     1,
       4,     1,     3,     1,     3,     1,     4,     3,     5,     1,
       4,     1,     3,     1,     3,     1,     3,     1,     3,     1,
       4,     3,     5,     1,     4,     1,     1,     1,     3,     1,
       1,     6,     6,     6,     6,     6,     1,     3,     1,     3,
       1,     3,     1,     3,     3,     1,     3,     1,     3,     1,
       3,     3,     1,     1,     3,     1,     3,     1,     3,     1,
       1,     2,     1,     1,     1,     1,     1,     1,     1,     2,
       2,     3,     1,     3,     1,     3,     2,     2,     2,     3,
       3,     1,     2,     4,     2,     3,     3,     5,     2,     3,
       3,     2,     2,     3,     1,     3,     2,     2,     3,     3,
       6,     1,     3,     3,     1,     1,     4,     4,     5,     5,
       0,     3,     1,     3,     5,     1,     3,     1,     4,     4,
       5,     5,     1,     3,     3,     1,     4,     4,     5,     5,
       1,     2,     4,     4,     3,     3,     1,     4,     4,     5,
       5,     4,     1,     1,     1,     1,     1,     3,     1,     1,
       1,     3,     1,     3,     1,     3,     1,     3,     1,     2,
       2,     1,     2,     2,     1,     3,     1,     1,     3,     1,
       1,     4,     4,     1,     3,     1,     4,     4,     1,     3,
       3,     1,     4,     4,     1,     3,     1,     2,     1,     4,
       4,     1,     4,     1,     1,     3
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
       0,     0,     0,     0,     0,     0
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
       0,     0,     0,     0,     0,     0
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error.  */
static const unsigned short int yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      31,    32,    33,    34,    35,     0,     0,     0,    27,     2,
       8,     0,    11,    12,     9,    15,    28,    29,     0,     0,
       0,     0,   106,     0,   107,     0,   103,   104,     0,     0,
       0,    98,   105,     3,    39,    40,    45,    49,    54,    60,
      66,    72,    74,    76,    78,    81,    84,    87,    94,    99,
     100,   101,   117,   120,     4,   116,   118,     0,   169,   170,
       0,     0,     0,     0,     0,     0,    98,     0,   166,     5,
     122,   123,   125,   127,   132,   136,   141,   151,   155,   165,
     167,     0,     0,     0,     0,     0,     0,     0,     0,     6,
     199,   200,   202,   203,   204,   205,   221,   206,   207,   208,
       0,     0,     0,     0,   283,   284,     0,     0,     0,     0,
       0,   286,   285,   282,     7,   244,   245,   255,   262,   270,
       1,     0,    21,    16,    17,     0,     0,     0,     0,     0,
      89,     0,     0,    91,    88,    90,     0,    96,     0,    98,
      96,     0,     0,   111,     0,   178,     0,   241,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    84,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     178,   212,   209,     0,     0,   216,     0,   217,     0,     0,
       0,   222,     0,   234,   231,     0,     0,   236,     0,     0,
     201,     0,     0,     0,     0,   276,   271,     0,     0,     0,
     333,   334,     0,     0,     0,   304,   331,     0,   289,   290,
       0,   294,     0,   298,   301,   306,   309,   310,   313,   318,
     324,   326,     0,     0,     0,     0,   250,   250,     0,     0,
       0,     0,     0,    30,     0,     0,    19,     0,     0,     0,
       0,    10,    13,    14,     0,     0,   102,     0,   108,     0,
       0,   109,     0,   110,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    42,     0,     0,    51,    50,     0,     0,
      57,    55,    56,     0,     0,    63,    61,    62,    68,    67,
      70,    69,    71,    75,    77,    73,    79,    80,    82,    83,
      85,    86,     0,     0,   119,   168,     0,   176,     0,     0,
     195,     0,     0,     0,   180,     0,     0,   185,     0,     0,
       0,   139,   137,   149,     0,   141,   143,   153,   155,   124,
       0,   130,   129,   128,     0,   133,   134,     0,     0,   145,
     142,   152,     0,     0,   159,   156,     0,     0,   210,     0,
       0,   218,     0,     0,   228,     0,     0,     0,   224,     0,
       0,   232,     0,     0,     0,   237,   243,     0,     0,   250,
     250,   287,     0,     0,     0,   328,   327,     0,   306,     0,
       0,     0,     0,     0,   299,   300,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   275,   274,
       0,     0,     0,     0,   257,   256,     0,     0,     0,     0,
     265,   263,   264,    22,    27,    25,     0,    23,    18,    36,
      37,    38,     0,     0,    97,   115,   113,     0,   112,     0,
     179,   230,    46,   242,    47,    48,     0,     0,    41,     0,
       0,     0,     0,     0,     0,    95,   121,   126,     0,     0,
     193,   197,   192,     0,     0,     0,     0,     0,   182,     0,
       0,   187,   189,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   213,   211,   220,   219,   223,
     229,     0,   226,   225,   214,   235,   233,   239,     0,   238,
       0,     0,     0,     0,     0,     0,     0,   305,   335,     0,
       0,     0,   272,     0,   307,   291,   292,   296,   306,   295,
       0,     0,   315,   314,   325,     0,     0,   321,   319,   320,
     273,   246,   247,   281,     0,     0,   252,     0,     0,     0,
       0,   250,   250,     0,     0,   250,   250,     0,     0,    21,
      92,    93,   114,    44,    43,     0,     0,     0,     0,     0,
       0,   177,     0,     0,     0,   196,     0,     0,     0,     0,
     181,     0,     0,   186,     0,     0,     0,     0,   138,     0,
     144,   154,     0,   163,   158,     0,     0,     0,   147,     0,
       0,   161,     0,     0,     0,     0,   277,   278,     0,     0,
       0,     0,   311,   312,   332,     0,     0,   302,   303,     0,
       0,     0,     0,     0,     0,     0,   251,   249,   248,     0,
       0,     0,     0,     0,     0,     0,     0,    26,    24,    20,
      52,    53,    58,    59,    64,    65,   171,   194,   198,   175,
     172,   184,   183,   173,   188,   190,   191,   174,   140,   150,
       0,     0,     0,   131,   135,   146,     0,   160,     0,   227,
     215,     0,   280,   279,   329,   330,   308,   293,   297,     0,
       0,     0,     0,     0,   253,   258,   259,     0,     0,   266,
     267,     0,     0,     0,     0,     0,   148,   162,   240,   316,
     317,   322,   323,     0,   261,   260,   269,   268,   164,   254
};

/* YYPDEFGOTO[NTERM-NUM].  */
static const short int yydefgoto[] =
{
      -1,     7,   435,    20,    21,    22,    23,    24,   133,   134,
     266,   436,   437,    25,    26,    27,   210,    44,   292,   293,
      45,    46,   297,    47,   301,    48,   306,    49,    50,    51,
      52,    53,    54,    55,    56,   144,    57,   148,    58,    59,
      60,   152,   153,    61,   154,   236,    65,    78,    79,    80,
      81,    82,   352,    83,   355,    84,   658,    85,   343,   360,
     659,    86,   346,    87,   347,   365,   594,    88,    89,    90,
     329,   156,   336,   477,   478,   339,   480,   481,   482,   472,
     332,   473,    99,   100,   101,   102,   202,   203,   505,   103,
     104,   205,   206,   105,   106,   211,   212,   208,   157,   107,
     214,   215,   108,   217,   218,   158,   109,   124,   125,   416,
     545,   546,   126,   425,   127,   431,   128,   226,   129,   237,
     238,   525,   239,   240,   241,   242,   243,   244,   528,   246,
     247,   533,   248,   538,   249,   250,   396,   251
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -554
static const short int yypact[] =
{
     286,   171,   780,    84,    53,   296,   300,   161,   171,   -34,
    -554,  -554,  -554,  -554,  -554,   128,   136,   139,  -554,  -554,
    -554,   198,   177,  -554,    39,  -554,  -554,  -554,  1207,  1093,
    1207,   780,  -554,   780,  -554,  1078,  -554,  -554,   133,   133,
     133,  -554,  -554,  -554,   144,  -554,  -554,   216,    96,   126,
      69,   240,   244,   157,   140,    56,  -554,   225,  -554,  -554,
    -554,  -554,  -554,   228,  -554,   246,  -554,   566,  -554,  -554,
     133,   243,   247,   251,   262,   268,   676,   299,  -554,  -554,
    -554,   290,  -554,   178,  -554,  -554,    57,  -554,   276,  -554,
    -554,   233,   133,   133,   133,   780,   133,   253,    53,  -554,
     296,  -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,
    1227,   300,    38,    38,  -554,  -554,   133,   133,   319,   270,
     279,   293,   347,  -554,  -554,  -554,  -554,   366,   200,  -554,
    -554,   353,    82,   363,  -554,   171,   171,   171,   171,   171,
    -554,   133,   133,  -554,  -554,  -554,   355,  -554,    54,   379,
     381,    27,    43,  -554,   378,  -554,   214,  -554,   221,   310,
     344,   331,  1135,  1150,  1150,  1192,  1192,  1207,  1207,  1207,
    1207,  1207,  1207,  1207,  1207,  1207,  1207,  1207,  1207,  1207,
    1207,   780,   780,   338,   299,   385,   361,   352,   357,   352,
     360,   387,   701,    53,   758,   758,   841,   863,  1207,   920,
     410,   420,   233,   445,   431,   133,   449,   133,    19,   458,
      83,   780,   465,   439,   133,   474,   113,   253,   480,   487,
    -554,   133,   133,   374,   394,  -554,  -554,   477,   475,    38,
    -554,  -554,   133,   133,   491,   211,  -554,   493,  -554,  -554,
     513,  -554,   217,  -554,  -554,   158,  -554,  -554,   520,   515,
     451,  -554,   519,   446,   450,   780,   533,   533,  1207,  1207,
    1237,  1291,  1291,  -554,   468,   516,  -554,   -34,   549,   577,
     586,  -554,  -554,  -554,   466,   470,  -554,   780,  -554,   171,
     780,  -554,   780,  -554,   780,   512,   171,   780,   133,   780,
     780,   602,    14,  -554,   133,   133,  -554,  -554,   133,   133,
    -554,  -554,  -554,   133,   133,  -554,  -554,  -554,  -554,  -554,
    -554,  -554,  -554,   484,   484,  -554,   140,   140,    56,    56,
    -554,  -554,   129,   141,  -554,  -554,    53,  -554,   133,   619,
    -554,   565,   627,   632,  -554,   575,   636,  -554,   565,   644,
     133,  -554,  -554,  -554,   655,   189,   651,  -554,   132,  -554,
     133,  -554,  -554,  -554,   133,  -554,  -554,   133,   686,  -554,
    -554,  -554,   133,   689,  -554,  -554,   171,   672,  -554,   171,
     675,  -554,   780,   680,  -554,   780,   780,   688,  -554,   171,
     695,  -554,    53,   133,   704,  -554,  -554,   472,   492,   533,
     533,  -554,  1316,   133,   133,  -554,  -554,   699,   706,   494,
     498,   780,  1227,   460,   239,   327,   460,  1330,  1207,  1332,
    1332,  1227,   300,   300,   709,   670,   711,   731,  -554,  -554,
     133,   133,   673,   674,  -554,  -554,   133,   133,   679,   681,
    -554,  -554,  -554,  -554,   730,  -554,   146,  -554,  -554,  -554,
    -554,  -554,  1093,  1093,  -554,  -554,  -554,   381,  -554,   717,
    -554,  -554,  -554,  -554,  -554,  -554,   780,   331,  -554,   504,
     508,   530,   537,   540,   552,  -554,  -554,  -554,    76,    53,
    -554,  -554,   728,    77,    53,    53,   757,    92,  -554,    53,
     110,  -554,   759,    53,   554,   942,   999,  1207,  1021,   557,
     562,   568,   999,   579,  1021,  -554,  -554,  -554,  -554,   780,
    -554,   744,  -554,  -554,   761,  -554,  -554,  -554,   173,  -554,
    1227,  1227,   745,   746,   706,   584,   590,   329,  -554,  1316,
    1316,   747,  -554,    38,  -554,  -554,   762,   335,  -554,   763,
     133,   133,  -554,  -554,  -554,   133,   133,  -554,  -554,  -554,
    -554,  -554,  -554,  -554,   753,   176,  -554,   300,   300,   594,
     599,   533,   533,   604,   610,   533,   533,   171,   516,   754,
    -554,  -554,  -554,  -554,  -554,  1135,  1135,  1150,  1150,  1192,
    1192,  -554,   756,   708,   565,  -554,   766,   767,   712,   575,
    -554,   769,   565,  -554,    86,   770,   701,   133,  -554,   133,
    -554,  -554,   133,  -554,  -554,   758,   841,   863,  -554,   776,
     920,  -554,   778,   780,   171,   765,  -554,  -554,  1227,  1227,
     475,   475,  -554,  -554,  -554,   771,   460,  -554,  -554,   460,
     630,   639,   652,   654,   171,   670,  -554,  -554,  -554,  1237,
    1237,   782,   783,  1291,  1291,   784,   786,  -554,  -554,  -554,
    -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,
    -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,
     659,   661,   666,  -554,  -554,  -554,   863,  -554,   920,  -554,
    -554,    53,  -554,  -554,  -554,  -554,  -554,  -554,  -554,  1330,
    1330,  1332,  1332,   789,  -554,  -554,  -554,  1237,  1237,  -554,
    -554,  1291,  1291,   942,   999,  1021,  -554,  -554,  -554,  -554,
    -554,  -554,  -554,   780,  -554,  -554,  -554,  -554,  -554,  -554
};

/* YYPGOTO[NTERM-NUM].  */
static const short int yypgoto[] =
{
    -554,  -554,     9,   682,   438,  -554,   687,  -103,  -554,   558,
     269,  -554,   271,  -554,  -554,  -554,    -2,  -554,  -554,   370,
      78,  -142,   142,  -141,  -110,  -114,  -105,  -554,   103,  -554,
    -554,   544,   548,   572,    -3,   287,  -554,     3,  -554,  -554,
    -554,  -554,   559,  -554,  -554,   827,  -554,    40,   -63,   646,
     514,  -154,  -193,  -554,   257,  -177,  -168,  -176,  -554,  -429,
    -174,  -179,  -554,  -167,  -428,  -444,  -478,  -189,  -554,  -554,
     653,     7,  -554,  -554,   275,  -554,  -554,   265,  -322,  -554,
    -554,  -554,  -554,  -554,   755,  -554,  -554,   656,  -554,  -554,
    -554,   768,   -19,  -554,  -554,   488,  -203,  -554,   -49,  -554,
    -554,   645,  -554,  -554,   647,   321,  -554,   751,  -345,  -227,
    -554,   238,  -185,  -553,  -205,  -257,   -96,  -363,  -554,   752,
    -201,   254,  -554,  -369,  -554,  -359,  -554,  -554,  -106,   232,
    -395,    74,  -351,  -348,  -554,  -217,   145,  -554
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -309
static const short int yytable[] =
{
      43,    77,   353,   348,   185,   432,   245,   245,   377,   471,
      19,   395,   532,   345,   225,   341,   602,   131,   344,   356,
     296,   359,   300,   300,   342,   140,   143,   145,   397,   146,
     417,   147,   364,   150,   526,   219,   273,   529,   151,   522,
     351,   351,   457,    66,   527,   209,   123,   527,   540,   -14,
     601,   305,   305,   132,   302,   277,   430,   430,   537,   537,
     593,   307,   539,   598,   184,   146,   593,   541,   542,   372,
     228,   282,   281,   -14,   229,   424,   685,   686,    28,   179,
     167,   168,   277,   180,   197,    29,   458,    30,   283,    67,
     198,    32,    33,   375,   278,    77,    34,    35,   201,   204,
     204,   230,   231,   213,   285,   574,   155,   232,   233,   376,
     169,   170,    62,   264,   163,   164,    36,    37,   265,   234,
     579,   571,   575,   398,   235,    63,    68,    69,    70,    71,
      72,    73,    74,    75,   704,   705,   171,   580,   582,   382,
      76,    42,   165,   166,   268,   269,   270,   606,   607,   383,
     123,   123,    66,    66,  -157,   583,   667,   277,    62,   373,
     655,   130,   512,   513,   135,  -155,   465,   488,   665,   277,
     174,    63,   136,   656,   558,   137,   320,   321,   466,   147,
     147,   175,   176,   559,   322,   323,   370,   194,   370,    77,
      77,    77,    77,    77,   358,   361,   363,   195,  -288,   196,
    -288,   288,   627,   628,   625,   177,   178,     8,   138,   201,
     605,   139,   204,   626,   204,   161,   486,   708,   261,   262,
     155,   213,   198,   324,   697,     9,    10,    11,    12,    13,
      14,    15,    16,    17,  -307,  -307,   162,   696,  -307,   453,
     404,   405,   285,   286,   406,   672,   673,   526,   287,   288,
     678,   667,   648,   414,   172,   418,   419,   527,    18,   173,
     527,   181,  -302,  -302,   182,   665,  -302,   593,    66,    66,
     308,   309,   310,   311,   312,   444,   273,   315,   446,   187,
     447,   183,   449,   188,   532,   532,   514,   189,   445,     1,
       2,     3,     4,     5,     6,   451,   377,   348,   190,   348,
     123,   123,   123,   348,   191,   348,   225,   345,   341,   192,
     359,   199,   590,   345,   193,   225,   359,   588,   599,   507,
     200,   364,   615,    77,   631,   632,   258,   364,   635,   636,
     537,   537,   110,   701,   702,   468,   111,   289,   288,   112,
     216,   113,    91,    92,    93,    94,    95,    96,    97,    98,
    -303,  -303,  -308,  -308,  -303,   255,  -308,   256,   617,   618,
     159,   160,   619,   114,   115,   452,   257,   454,   455,   116,
     117,   290,   288,   501,   502,   495,   689,   690,   497,    77,
     259,   118,   119,   120,   121,   122,   260,    63,   326,   288,
     263,   186,   276,   395,   395,   327,   328,   348,   267,   521,
     330,   331,   663,   334,   335,   534,   572,   345,   279,   341,
     280,   576,   577,   284,   225,   225,   581,   398,   291,   356,
     585,   359,   325,   296,   296,    63,   300,   300,   430,   430,
     337,   338,    66,   364,   706,   707,   366,   253,   254,   143,
     143,   351,   123,    66,   424,   424,    66,    66,   285,    66,
      66,   123,   123,   123,   563,   305,   305,   642,   643,   285,
     369,   389,   274,   275,   644,   645,    77,   285,   379,   409,
     410,    77,    77,   412,   288,   368,    77,   413,   288,   371,
      77,   390,    77,   358,   591,   363,   430,   430,   374,   358,
     359,   363,   228,   442,   288,   378,   523,   443,   288,   510,
     288,   364,   424,   424,   381,   348,   348,   228,   175,   176,
     385,   392,   225,   225,   391,   345,   341,   386,   359,   511,
     288,   519,   288,   230,   231,   520,   288,   401,   364,   232,
     233,   565,   288,   402,    62,   566,   288,   403,   230,   231,
     407,   234,   387,   388,   393,   394,   524,    63,   408,    62,
     123,   123,     8,   399,   400,   433,   234,   567,   288,    66,
      66,   411,    63,    66,   568,   288,   637,   569,   288,   415,
       9,    10,    11,    12,    13,    14,    15,    16,    17,   570,
     288,   586,   288,    77,   595,   288,   439,   123,   123,   596,
     288,    28,    77,    77,   358,   597,   288,   363,    29,   450,
      30,   669,    67,   434,    32,    33,   600,   288,   698,    34,
      35,   610,   288,   670,   440,   459,   460,   611,   288,   461,
     462,   629,   288,   441,   463,   464,   630,   288,   456,    36,
      37,   633,   288,   683,    38,    39,    40,   634,   288,    68,
      69,    70,    71,    72,    73,    74,    75,   469,   123,   123,
      66,    66,   470,    76,    42,   474,    66,   679,   288,    66,
     475,   484,   476,   358,   479,   363,   680,   288,    77,   123,
     123,   489,   483,   123,   123,   490,  -120,   485,   491,   681,
     288,   682,   288,   493,   487,  -120,   693,   288,   694,   288,
      77,   358,   363,   695,   288,   -98,   492,  -120,  -120,   494,
     -98,   709,   496,   -98,   508,   498,  -120,   640,   641,  -120,
     500,  -120,   182,   -98,   515,   516,   313,   314,   503,    66,
      66,    66,    66,   316,   317,   506,    28,   123,   123,   560,
     561,   123,   123,    29,   509,    30,   517,    67,   547,    32,
      33,   549,   550,   518,    34,    35,   543,   553,   554,   318,
     319,   612,   613,   699,   700,   674,   675,   544,   548,   557,
     551,   552,   562,   573,    36,    37,   555,   578,   556,   584,
     603,   604,   608,   609,    68,    69,   340,    71,    72,    73,
      74,    75,   624,    28,   614,   264,   616,  -297,    76,    42,
      29,   671,    30,   646,    67,   647,    32,    33,   666,   651,
     668,    34,    35,   649,   650,    28,   653,   657,   676,   687,
     688,   691,    29,   692,    30,   703,    31,   504,    32,    33,
     271,    36,    37,    34,    35,   438,   272,   564,   639,   638,
      64,    68,    69,   350,    71,    72,    73,    74,    75,   349,
     467,   448,   333,    36,    37,    76,    42,   654,    38,    39,
      40,   620,   621,   664,   652,   220,   622,   623,   367,   380,
     499,   207,   227,   684,   384,   252,    28,    41,    42,     0,
     677,     0,     0,    29,     0,    30,     0,    67,     0,    32,
      33,     0,     0,     0,    34,    35,     0,     0,    28,     0,
       0,     0,     0,     0,     0,    29,     0,    30,     0,    67,
       0,    32,    33,     0,    36,    37,    34,    35,   660,     0,
     661,     0,     0,   662,    68,    69,   354,    71,    72,    73,
      74,    75,     0,     0,     0,     0,    36,    37,    76,    42,
       0,     0,     0,     0,     0,     0,    68,    69,   357,    71,
      72,    73,    74,    75,     0,    28,     0,     0,     0,     0,
      76,    42,    29,     0,    30,     0,    67,     0,    32,    33,
       0,     0,     0,    34,    35,     0,     0,    28,     0,     0,
       0,     0,     0,     0,    29,     0,    30,     0,    67,     0,
      32,    33,     0,    36,    37,    34,    35,     0,     0,     0,
       0,     0,     0,    68,    69,   362,    71,    72,    73,    74,
      75,     0,     0,     0,     0,    36,    37,    76,    42,     0,
       0,     0,     0,     0,     0,    68,    69,   587,    71,    72,
      73,    74,    75,     0,    28,     0,     0,     0,     0,    76,
      42,    29,     0,    30,     0,    67,     0,    32,    33,     0,
       0,     0,    34,    35,     0,     0,    28,     0,     0,     0,
       0,     0,     0,    29,     0,    30,     0,    67,     0,    32,
      33,     0,    36,    37,    34,    35,     0,     0,     0,     0,
       0,     0,    68,    69,   589,    71,    72,    73,    74,    75,
       0,     0,     0,     0,    36,    37,    76,    42,     0,     0,
       0,     0,     0,     0,    68,    69,   592,    71,    72,    73,
      74,    75,     0,    28,     0,     0,     0,     0,    76,    42,
      29,     0,    30,     0,    31,     0,    32,    33,    28,     0,
       0,    34,    35,     0,     0,    29,     0,    30,     0,    31,
       0,    32,    33,     0,     0,     0,    34,    35,     0,     0,
       0,    36,    37,     0,     0,     0,    38,    39,    40,     0,
       0,     0,     0,     0,     0,     0,    36,    37,     0,     0,
      28,     0,   141,   142,     0,   149,    42,    29,     0,    30,
       0,    31,     0,    32,    33,    28,     0,     0,    34,    35,
      41,    42,    29,     0,    30,     0,    31,     0,    32,    33,
       0,     0,     0,    34,    35,     0,     0,     0,    36,    37,
       0,     0,     0,     0,   294,   295,     0,     0,     0,     0,
       0,     0,     0,    36,    37,     0,     0,    28,     0,   298,
     299,     0,    41,    42,    29,     0,    30,     0,    31,     0,
      32,    33,    28,     0,     0,    34,    35,    41,    42,    29,
       0,    30,     0,    31,     0,    32,    33,     0,     0,     0,
      34,    35,     0,     0,     0,    36,    37,     0,     0,   110,
       0,   303,   304,   111,     0,     0,   112,     0,   113,   110,
      36,    37,     0,   111,     0,     0,   112,     0,   113,    41,
      42,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     114,   115,     0,     0,    41,    42,   221,   222,     0,     0,
     114,   115,     0,     0,     0,     0,   420,   421,   118,   223,
     224,   121,   122,     0,    63,     0,     0,     0,   118,   422,
     423,   121,   122,   110,    63,     0,     0,   111,     0,     0,
     112,     0,   113,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   228,     0,
       0,     0,   392,     0,   114,   115,     0,     0,     0,     0,
     426,   427,   228,     0,   228,     0,   392,     0,   392,     0,
       0,     0,   118,   428,   429,   121,   122,     0,    63,   230,
     231,     0,     0,     0,     0,   232,   233,     0,     0,     0,
      62,     0,     0,   230,   231,   230,   231,   234,     0,   530,
     531,   535,   536,    63,    62,     0,    62,     0,     0,     0,
       0,   234,     0,   234,     0,     0,     0,    63,     0,    63
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
       0,     0,     0,     0,     0,    17,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    15,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    19,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    11,     0,
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
       0,     0,     0,     0,     0,     1,     0,     0,     0,     0,
       3,     0,     0,     5,     0,     0,     0,     0,     0,     0,
       0,     0,     7,     9,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0
};

/* YYCONFL[I] -- lists of conflicting rule numbers, each terminated by
   0, pointed into by YYCONFLP.  */
static const short int yyconfl[] =
{
       0,   120,     0,   120,     0,   120,     0,    98,     0,   120,
       0,    16,     0,    98,     0,   296,     0,   157,     0,   296,
       0
};

static const short int yycheck[] =
{
       2,     4,   195,   192,    67,   262,   112,   113,   211,   331,
       1,   228,   407,   192,   110,   192,   494,     8,   192,   196,
     162,   197,   163,   164,   192,    28,    29,    30,   229,    31,
     257,    33,   199,    35,   403,    98,   139,   406,    35,   402,
     194,   195,    28,     3,   403,    94,     6,   406,   411,    10,
     494,   165,   166,    87,   164,    28,   261,   262,   409,   410,
     488,   166,   410,   492,    67,    67,   494,   412,   413,    50,
      32,    28,    45,    34,    36,   260,   629,   630,    25,    23,
      11,    12,    28,    27,    27,    32,    72,    34,    45,    36,
      33,    38,    39,    10,    40,    98,    43,    44,    91,    92,
      93,    63,    64,    96,    28,    28,    87,    69,    70,    26,
      41,    42,    74,    31,    18,    19,    63,    64,    36,    81,
      28,    45,    45,   229,    86,    87,    73,    74,    75,    76,
      77,    78,    79,    80,   687,   688,    67,    45,    28,    26,
      87,    88,    16,    17,   135,   136,   137,   510,   511,    36,
     110,   111,   112,   113,    22,    45,   600,    28,    74,   208,
      74,     0,   389,   390,    36,    33,    37,    35,   597,    28,
      13,    87,    36,    87,    28,    36,   179,   180,    37,   181,
     182,    24,    25,    37,   181,   182,   205,     9,   207,   192,
     193,   194,   195,   196,   197,   198,   199,    19,    40,    21,
      42,    28,   547,   548,    28,    65,    66,    36,    10,   202,
      37,    34,   205,    37,   207,    71,    27,   695,    18,    19,
      87,   214,    33,   183,   668,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    23,    24,    20,   666,    27,   288,
      23,    24,    28,    29,    27,   608,   609,   616,    27,    28,
     619,   695,   574,   255,    14,   258,   259,   616,    87,    15,
     619,    36,    23,    24,    36,   694,    27,   695,   228,   229,
     167,   168,   169,   170,   171,   277,   379,   174,   280,    36,
     282,    35,   284,    36,   679,   680,   392,    36,   279,     3,
       4,     5,     6,     7,     8,   286,   499,   486,    36,   488,
     260,   261,   262,   492,    36,   494,   402,   486,   485,    10,
     486,    35,   486,   492,    24,   411,   492,   485,   492,   382,
      87,   488,   523,   326,   551,   552,    33,   494,   555,   556,
     681,   682,    32,   681,   682,   328,    36,    27,    28,    39,
      87,    41,    46,    47,    48,    49,    50,    51,    52,    53,
      23,    24,    23,    24,    27,    36,    27,    87,    23,    24,
      39,    40,    27,    63,    64,   287,    87,   289,   290,    69,
      70,    27,    28,   375,   376,   366,   633,   634,   369,   382,
      33,    81,    82,    83,    84,    85,    20,    87,    27,    28,
      37,    70,    37,   610,   611,    43,    44,   586,    35,   401,
      43,    44,   595,    43,    44,   408,   469,   586,    29,   586,
      29,   474,   475,    35,   510,   511,   479,   523,    87,   596,
     483,   597,    37,   565,   566,    87,   567,   568,   633,   634,
      43,    44,   392,   600,   691,   692,    26,   116,   117,   442,
     443,   595,   402,   403,   629,   630,   406,   407,    28,   409,
     410,   411,   412,   413,   456,   569,   570,   567,   568,    28,
      29,    87,   141,   142,   569,   570,   469,    28,    29,    18,
      19,   474,   475,    27,    28,    30,   479,    27,    28,    30,
     483,    87,   485,   486,   487,   488,   691,   692,    30,   492,
     666,   494,    32,    27,    28,    30,    36,    27,    28,    27,
      28,   668,   687,   688,    30,   694,   695,    32,    24,    25,
      30,    36,   608,   609,    37,   694,   693,    30,   694,    27,
      28,    27,    28,    63,    64,    27,    28,    36,   695,    69,
      70,    27,    28,    40,    74,    27,    28,    24,    63,    64,
      20,    81,   221,   222,    69,    70,    86,    87,    33,    74,
     510,   511,    36,   232,   233,    87,    81,    27,    28,   519,
     520,    42,    87,   523,    27,    28,   557,    27,    28,    36,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    27,
      28,    27,    28,   586,    27,    28,    37,   547,   548,    27,
      28,    25,   595,   596,   597,    27,    28,   600,    32,    87,
      34,   603,    36,    87,    38,    39,    27,    28,   671,    43,
      44,    27,    28,   604,    37,   294,   295,    27,    28,   298,
     299,    27,    28,    37,   303,   304,    27,    28,    26,    63,
      64,    27,    28,   624,    68,    69,    70,    27,    28,    73,
      74,    75,    76,    77,    78,    79,    80,    28,   608,   609,
     610,   611,    87,    87,    88,    28,   616,    27,    28,   619,
      28,   340,    87,   666,    28,   668,    27,    28,   671,   629,
     630,   350,    28,   633,   634,   354,     0,    22,   357,    27,
      28,    27,    28,   362,    33,     9,    27,    28,    27,    28,
     693,   694,   695,    27,    28,    19,    10,    21,    22,    10,
      24,   703,    30,    27,   383,    30,    30,   565,   566,    33,
      30,    35,    36,    37,   393,   394,   172,   173,    30,   679,
     680,   681,   682,   175,   176,    30,    25,   687,   688,   442,
     443,   691,   692,    32,    30,    34,    37,    36,    27,    38,
      39,   420,   421,    37,    43,    44,    37,   426,   427,   177,
     178,   519,   520,   679,   680,   610,   611,    87,    27,    29,
      87,    87,    45,    35,    63,    64,    87,    10,    87,    10,
      26,    10,    27,    27,    73,    74,    75,    76,    77,    78,
      79,    80,    29,    25,    37,    31,    24,    24,    87,    88,
      32,    26,    34,    37,    36,    87,    38,    39,    22,    87,
      22,    43,    44,    37,    37,    25,    37,    37,    37,    27,
      27,    27,    32,    27,    34,    26,    36,   379,    38,    39,
     138,    63,    64,    43,    44,   267,   139,   457,   559,   558,
       3,    73,    74,    75,    76,    77,    78,    79,    80,   193,
     326,   282,   189,    63,    64,    87,    88,   582,    68,    69,
      70,   530,   531,   596,   579,   100,   535,   536,   202,   214,
     372,    93,   111,   625,   217,   113,    25,    87,    88,    -1,
     616,    -1,    -1,    32,    -1,    34,    -1,    36,    -1,    38,
      39,    -1,    -1,    -1,    43,    44,    -1,    -1,    25,    -1,
      -1,    -1,    -1,    -1,    -1,    32,    -1,    34,    -1,    36,
      -1,    38,    39,    -1,    63,    64,    43,    44,   587,    -1,
     589,    -1,    -1,   592,    73,    74,    75,    76,    77,    78,
      79,    80,    -1,    -1,    -1,    -1,    63,    64,    87,    88,
      -1,    -1,    -1,    -1,    -1,    -1,    73,    74,    75,    76,
      77,    78,    79,    80,    -1,    25,    -1,    -1,    -1,    -1,
      87,    88,    32,    -1,    34,    -1,    36,    -1,    38,    39,
      -1,    -1,    -1,    43,    44,    -1,    -1,    25,    -1,    -1,
      -1,    -1,    -1,    -1,    32,    -1,    34,    -1,    36,    -1,
      38,    39,    -1,    63,    64,    43,    44,    -1,    -1,    -1,
      -1,    -1,    -1,    73,    74,    75,    76,    77,    78,    79,
      80,    -1,    -1,    -1,    -1,    63,    64,    87,    88,    -1,
      -1,    -1,    -1,    -1,    -1,    73,    74,    75,    76,    77,
      78,    79,    80,    -1,    25,    -1,    -1,    -1,    -1,    87,
      88,    32,    -1,    34,    -1,    36,    -1,    38,    39,    -1,
      -1,    -1,    43,    44,    -1,    -1,    25,    -1,    -1,    -1,
      -1,    -1,    -1,    32,    -1,    34,    -1,    36,    -1,    38,
      39,    -1,    63,    64,    43,    44,    -1,    -1,    -1,    -1,
      -1,    -1,    73,    74,    75,    76,    77,    78,    79,    80,
      -1,    -1,    -1,    -1,    63,    64,    87,    88,    -1,    -1,
      -1,    -1,    -1,    -1,    73,    74,    75,    76,    77,    78,
      79,    80,    -1,    25,    -1,    -1,    -1,    -1,    87,    88,
      32,    -1,    34,    -1,    36,    -1,    38,    39,    25,    -1,
      -1,    43,    44,    -1,    -1,    32,    -1,    34,    -1,    36,
      -1,    38,    39,    -1,    -1,    -1,    43,    44,    -1,    -1,
      -1,    63,    64,    -1,    -1,    -1,    68,    69,    70,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    63,    64,    -1,    -1,
      25,    -1,    69,    70,    -1,    87,    88,    32,    -1,    34,
      -1,    36,    -1,    38,    39,    25,    -1,    -1,    43,    44,
      87,    88,    32,    -1,    34,    -1,    36,    -1,    38,    39,
      -1,    -1,    -1,    43,    44,    -1,    -1,    -1,    63,    64,
      -1,    -1,    -1,    -1,    69,    70,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    63,    64,    -1,    -1,    25,    -1,    69,
      70,    -1,    87,    88,    32,    -1,    34,    -1,    36,    -1,
      38,    39,    25,    -1,    -1,    43,    44,    87,    88,    32,
      -1,    34,    -1,    36,    -1,    38,    39,    -1,    -1,    -1,
      43,    44,    -1,    -1,    -1,    63,    64,    -1,    -1,    32,
      -1,    69,    70,    36,    -1,    -1,    39,    -1,    41,    32,
      63,    64,    -1,    36,    -1,    -1,    39,    -1,    41,    87,
      88,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      63,    64,    -1,    -1,    87,    88,    69,    70,    -1,    -1,
      63,    64,    -1,    -1,    -1,    -1,    69,    70,    81,    82,
      83,    84,    85,    -1,    87,    -1,    -1,    -1,    81,    82,
      83,    84,    85,    32,    87,    -1,    -1,    36,    -1,    -1,
      39,    -1,    41,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    32,    -1,
      -1,    -1,    36,    -1,    63,    64,    -1,    -1,    -1,    -1,
      69,    70,    32,    -1,    32,    -1,    36,    -1,    36,    -1,
      -1,    -1,    81,    82,    83,    84,    85,    -1,    87,    63,
      64,    -1,    -1,    -1,    -1,    69,    70,    -1,    -1,    -1,
      74,    -1,    -1,    63,    64,    63,    64,    81,    -1,    69,
      70,    69,    70,    87,    74,    -1,    74,    -1,    -1,    -1,
      -1,    81,    -1,    81,    -1,    -1,    -1,    87,    -1,    87
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     3,     4,     5,     6,     7,     8,    90,    36,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    87,    91,
      92,    93,    94,    95,    96,   102,   103,   104,    25,    32,
      34,    36,    38,    39,    43,    44,    63,    64,    68,    69,
      70,    87,    88,   105,   106,   109,   110,   112,   114,   116,
     117,   118,   119,   120,   121,   122,   123,   125,   127,   128,
     129,   132,    74,    87,   134,   135,   136,    36,    73,    74,
      75,    76,    77,    78,    79,    80,    87,   123,   136,   137,
     138,   139,   140,   142,   144,   146,   150,   152,   156,   157,
     158,    46,    47,    48,    49,    50,    51,    52,    53,   171,
     172,   173,   174,   178,   179,   182,   183,   188,   191,   195,
      32,    36,    39,    41,    63,    64,    69,    70,    81,    82,
      83,    84,    85,   136,   196,   197,   201,   203,   205,   207,
       0,    91,    87,    97,    98,    36,    36,    36,    10,    34,
     123,    69,    70,   123,   124,   123,   105,   105,   126,    87,
     105,   126,   130,   131,   133,    87,   160,   187,   194,   194,
     194,    71,    20,    18,    19,    16,    17,    11,    12,    41,
      42,    67,    14,    15,    13,    24,    25,    65,    66,    23,
      27,    36,    36,    35,   123,   137,   194,    36,    36,    36,
      36,    36,    10,    24,     9,    19,    21,    27,    33,    35,
      87,   160,   175,   176,   160,   180,   181,   180,   186,   187,
     105,   184,   185,   160,   189,   190,    87,   192,   193,   137,
     173,    69,    70,    82,    83,   205,   206,   196,    32,    36,
      63,    64,    69,    70,    81,    86,   134,   208,   209,   211,
     212,   213,   214,   215,   216,   217,   218,   219,   221,   223,
     224,   226,   208,   194,   194,    36,    87,    87,    33,    33,
      20,    18,    19,    37,    31,    36,    99,    35,    91,    91,
      91,    92,    95,    96,   194,   194,    37,    28,    40,    29,
      29,    45,    28,    45,    35,    28,    29,    27,    28,    27,
      27,    87,   107,   108,    69,    70,   110,   111,    69,    70,
     112,   113,   113,    69,    70,   114,   115,   115,   117,   117,
     117,   117,   117,   120,   120,   117,   121,   121,   122,   122,
     123,   123,   126,   126,   136,    37,    27,    43,    44,   159,
      43,    44,   169,   159,    43,    44,   161,    43,    44,   164,
      75,   144,   145,   147,   149,   150,   151,   153,   156,   138,
      75,   140,   141,   141,    75,   143,   144,    75,   123,   146,
     148,   123,    75,   123,   152,   154,    26,   176,    30,    29,
     181,    30,    50,   187,    30,    10,    26,   185,    30,    29,
     190,    30,    26,    36,   193,    30,    30,   194,   194,    87,
      87,    37,    36,    69,    70,   224,   225,   209,   217,   194,
     194,    36,    40,    24,    23,    24,    27,    20,    33,    18,
      19,    42,    27,    27,   105,    36,   198,   198,   123,   123,
      69,    70,    82,    83,   201,   202,    69,    70,    82,    83,
     203,   204,   204,    87,    87,    91,   100,   101,    98,    37,
      37,    37,    27,    27,   105,    91,   105,   105,   131,   105,
      87,    91,   109,   187,   109,   109,    26,    28,    72,   194,
     194,   194,   194,   194,   194,    37,    37,   139,   160,    28,
      87,   167,   168,   170,    28,    28,    87,   162,   163,    28,
     165,   166,   167,    28,   194,    22,    27,    33,    35,   194,
     194,   194,    10,   194,    10,    91,    30,    91,    30,   184,
      30,   105,   105,    30,    93,   177,    30,   137,   194,    30,
      27,    27,   198,   198,   217,   194,   194,    37,    37,    27,
      27,   105,   206,    36,    86,   210,   212,   214,   217,   212,
      69,    70,   219,   220,   123,    69,    70,   221,   222,   222,
     206,   197,   197,    37,    87,   199,   200,    27,    27,   194,
     194,    87,    87,   194,   194,    87,    87,    29,    28,    37,
     124,   124,    45,   105,   108,    27,    27,    27,    27,    27,
      27,    45,   137,    35,    28,    45,   137,   137,    10,    28,
      45,   137,    28,    45,    10,   137,    27,    75,   145,    75,
     149,   123,    75,   153,   155,    27,    27,    27,   148,   149,
      27,   154,   155,    26,    10,    37,   206,   206,    27,    27,
      27,    27,   218,   218,    37,   209,    24,    23,    24,    27,
     194,   194,   194,   194,    29,    28,    37,   197,   197,    27,
      27,   198,   198,    27,    27,   198,   198,    91,   101,    99,
     111,   111,   113,   113,   115,   115,    37,    87,   167,    37,
      37,    87,   163,    37,   166,    74,    87,    37,   145,   149,
     194,   194,   194,   141,   143,   148,    22,   154,    22,   105,
      91,    26,   206,   206,   225,   225,    37,   210,   212,    27,
      27,    27,    27,    91,   200,   202,   202,    27,    27,   204,
     204,    27,    27,    27,    27,    27,   148,   154,   137,   220,
     220,   222,   222,    26,   202,   202,   204,   204,   155,   105
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
#line 151 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl));
      spec_tree = ((*yyvalp).appl);
    ;}
    break;

  case 3:
#line 156 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl));
      spec_tree = ((*yyvalp).appl);
    ;}
    break;

  case 4:
#line 161 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl));
      spec_tree = ((*yyvalp).appl);
    ;}
    break;

  case 5:
#line 166 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl));
      spec_tree = ((*yyvalp).appl);
    ;}
    break;

  case 6:
#line 171 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl));
      spec_tree = ((*yyvalp).appl);
    ;}
    break;

  case 7:
#line 176 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl));
      spec_tree = ((*yyvalp).appl);
    ;}
    break;

  case 8:
#line 188 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed sort expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 9:
#line 197 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 10:
#line 201 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSortArrow((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed arrow sort\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 11:
#line 210 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed non-arrow domain\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 12:
#line 219 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed non-arrow domain elements\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 13:
#line 224 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed non-arrow domain elements\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 14:
#line 233 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed non-arrow domain element\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 15:
#line 242 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 16:
#line 246 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSortStruct(ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.list))));
      gsDebugMsg("parsed structured sort\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 17:
#line 255 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed structured sort constructors\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 18:
#line 260 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed structured sort constructors\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 19:
#line 269 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStructCons((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl), ATmakeList0(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed structured sort constructor\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 20:
#line 274 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStructCons((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.appl), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.list)), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed structured sort constructor\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 21:
#line 283 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeNil());
      gsDebugMsg("parsed recogniser\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 22:
#line 288 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed recogniser id\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 23:
#line 297 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed structured sort projections\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 24:
#line 302 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed structured sort projections\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 25:
#line 311 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStructProj(gsMakeNil(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed structured sort projection\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 26:
#line 316 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStructProj((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed structured sort projection\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 27:
#line 325 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSortId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed primary sort\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 28:
#line 330 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed sort constant\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 29:
#line 335 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed sort constructor\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 30:
#line 340 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 31:
#line 348 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSortId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 32:
#line 352 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSortId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 33:
#line 356 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSortId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 34:
#line 360 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSortId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 35:
#line 364 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSortId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 36:
#line 372 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSortExprList((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 37:
#line 376 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSortExprSet((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 38:
#line 380 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSortExprBag((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 39:
#line 391 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed data expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 40:
#line 400 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 41:
#line 404 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeWhr((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.appl), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.list))));
      gsDebugMsg("parsed where clause\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 42:
#line 413 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed where clause declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 43:
#line 418 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed where clause declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 44:
#line 427 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeWhrDecl((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed where clause declaration\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 45:
#line 436 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 46:
#line 440 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBinder(gsMakeLambda(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed lambda abstraction\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 47:
#line 445 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBinder(gsMakeForall(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 48:
#line 450 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBinder(gsMakeExists(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 49:
#line 459 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 50:
#line 463 "mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed implication\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 51:
#line 473 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 52:
#line 477 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBinder(gsMakeForall(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 53:
#line 482 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBinder(gsMakeExists(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 54:
#line 491 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 55:
#line 495 "mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed conjunction\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 56:
#line 501 "mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed disjunction\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 57:
#line 511 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 58:
#line 515 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBinder(gsMakeForall(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 59:
#line 520 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBinder(gsMakeExists(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 60:
#line 529 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 61:
#line 533 "mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed equality expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 62:
#line 539 "mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed equality expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 63:
#line 549 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 64:
#line 553 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBinder(gsMakeForall(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 65:
#line 558 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBinder(gsMakeExists(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 66:
#line 567 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 67:
#line 571 "mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed relational expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 68:
#line 577 "mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed relational expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 69:
#line 583 "mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed relational expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 70:
#line 589 "mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed relational expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 71:
#line 595 "mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed relational expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 72:
#line 605 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 73:
#line 609 "mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed list cons expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 74:
#line 619 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 75:
#line 623 "mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed list snoc expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 76:
#line 633 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 77:
#line 637 "mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed list concat expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 78:
#line 647 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 79:
#line 651 "mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed addition or set union\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 80:
#line 657 "mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed subtraction or set difference\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 81:
#line 667 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 82:
#line 671 "mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed div expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 83:
#line 677 "mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed mod expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 84:
#line 687 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 85:
#line 691 "mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed multiplication or set intersection\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 86:
#line 697 "mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed list at expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 87:
#line 707 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 88:
#line 711 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl)), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl))));
      gsDebugMsg("parsed prefix data expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 89:
#line 716 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl)), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl))));
      gsDebugMsg("parsed prefix data expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 90:
#line 721 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl)), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl))));
      gsDebugMsg("parsed prefix data expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 91:
#line 730 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 92:
#line 734 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBinder(gsMakeForall(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 93:
#line 739 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBinder(gsMakeExists(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 94:
#line 748 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 95:
#line 752 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeDataAppl((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.appl), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.list))));
      gsDebugMsg("parsed postfix data expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 96:
#line 761 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed data expressions\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 97:
#line 766 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed data expressions\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 98:
#line 775 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed primary data expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 99:
#line 780 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 100:
#line 784 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 101:
#line 788 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 102:
#line 792 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 103:
#line 800 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed data constant\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 104:
#line 805 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed data constant\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 105:
#line 810 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed data constant\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 106:
#line 815 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed data constant\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 107:
#line 820 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed data constant\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 108:
#line 829 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeDataAppl(gsMakeId(gsMakeOpIdNameListEnum()), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.list))));
      gsDebugMsg("parsed data enumeration\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 109:
#line 834 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeDataAppl(gsMakeId(gsMakeOpIdNameSetEnum()), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.list))));
      gsDebugMsg("parsed data enumeration\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 110:
#line 839 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeDataAppl(gsMakeId(gsMakeOpIdNameBagEnum()), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.list))));
      gsDebugMsg("parsed data enumeration\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 111:
#line 848 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list));
      gsDebugMsg("parsed bag enumeration elements\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 112:
#line 853 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATconcat((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed bag enumeration elements\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 113:
#line 862 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed bag enumeration element\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 114:
#line 871 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBinder(gsMakeSetBagComp(), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.appl)), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed data comprehension\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 115:
#line 880 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeDataVarId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed data variable declaration\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 116:
#line 892 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeMultAct(ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list))));
      gsDebugMsg("parsed multi-action\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 117:
#line 897 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeMultAct(ATmakeList0()));
      gsDebugMsg("parsed multi-action\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 118:
#line 906 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed parameterised id's\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 119:
#line 911 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed parameterised id's\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 120:
#line 920 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeParamId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl), ATmakeList0()));
      gsDebugMsg("parsed action or process\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 121:
#line 925 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeParamId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.appl), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.list))));
      gsDebugMsg("parsed action or process\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 122:
#line 937 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed process expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 123:
#line 946 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 124:
#line 950 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeChoice((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed choice expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 125:
#line 959 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 126:
#line 963 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSum((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed summation\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 127:
#line 972 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 128:
#line 976 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeMerge((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed merge expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 129:
#line 981 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeLMerge((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed left merge expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 130:
#line 990 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 131:
#line 994 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSum((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed summation\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 132:
#line 1003 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 133:
#line 1007 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBInit((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed bounded initialisation expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 134:
#line 1016 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 135:
#line 1020 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSum((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed summation\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 136:
#line 1029 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 137:
#line 1033 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeIfThen((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed conditional expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 138:
#line 1038 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeIfThenElse((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed conditional expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 139:
#line 1047 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 140:
#line 1051 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSum((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed summation\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 141:
#line 1060 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 142:
#line 1064 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSeq((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed sequential expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 143:
#line 1073 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 144:
#line 1077 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSeq((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed sequential expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 145:
#line 1086 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 146:
#line 1090 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSum((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed summation\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 147:
#line 1095 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeIfThen((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed conditional expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 148:
#line 1100 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeIfThenElse((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed conditional expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 149:
#line 1109 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 150:
#line 1113 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSum((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed summation\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 151:
#line 1122 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 152:
#line 1126 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeAtTime((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed at time expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 153:
#line 1135 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 154:
#line 1139 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeAtTime((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed at time expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 155:
#line 1148 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 156:
#line 1152 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSync((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed sync expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 157:
#line 1161 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 158:
#line 1165 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSync((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed sync expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 159:
#line 1174 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 160:
#line 1178 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSum((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed summation\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 161:
#line 1183 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeIfThen((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed conditional expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 162:
#line 1188 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeIfThenElse((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed conditional expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 163:
#line 1197 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 164:
#line 1201 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSum((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed summation\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 165:
#line 1210 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 166:
#line 1214 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 167:
#line 1218 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 168:
#line 1222 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 169:
#line 1230 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeDelta());
      gsDebugMsg("parsed process constant\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 170:
#line 1235 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeTau());
      gsDebugMsg("parsed process constant\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 171:
#line 1244 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBlock((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (6))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (6))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed process quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 172:
#line 1249 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeHide((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (6))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (6))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed process quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 173:
#line 1254 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeRename((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (6))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (6))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed process quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 174:
#line 1259 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeComm((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (6))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (6))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed process quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 175:
#line 1264 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeAllow((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (6))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (6))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed process quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 176:
#line 1273 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList0());
      gsDebugMsg("parsed action name set\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 177:
#line 1278 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed action name set\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 178:
#line 1287 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed id's\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 179:
#line 1292 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed id's\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 180:
#line 1301 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList0());
      gsDebugMsg("parsed renaming expression set\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 181:
#line 1306 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed renaming expression set\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 182:
#line 1315 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed renaming expressions\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 183:
#line 1320 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed renaming expressions\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 184:
#line 1329 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeRenameExpr((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed renaming expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 185:
#line 1338 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList0());
      gsDebugMsg("parsed communication expression set\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 186:
#line 1343 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed communication expression set\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 187:
#line 1352 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed communication expressions\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 188:
#line 1357 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed communication expressions\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 189:
#line 1366 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeCommExpr((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl), gsMakeNil()));
      gsDebugMsg("parsed communication expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 190:
#line 1371 "mcrl2parser.yy"
    {      
      safe_assign(((*yyvalp).appl), gsMakeCommExpr((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), gsMakeNil()));
      gsDebugMsg("parsed communication expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 191:
#line 1376 "mcrl2parser.yy"
    {      
      safe_assign(((*yyvalp).appl), gsMakeCommExpr((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed communication expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 192:
#line 1385 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeMultActName(ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list))));
      gsDebugMsg("parsed multi action name\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 193:
#line 1394 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed id's\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 194:
#line 1399 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed id's\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 195:
#line 1408 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList0());
      gsDebugMsg("parsed multi action name set\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 196:
#line 1413 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed multi action name set\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 197:
#line 1422 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed multi action names\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 198:
#line 1427 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed multi action names\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 199:
#line 1439 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsSpecEltsToSpec(ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list))));
      gsDebugMsg("parsed specification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 200:
#line 1448 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed specification elements\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 201:
#line 1453 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed specification elements\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 202:
#line 1462 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed specification element\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 203:
#line 1467 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed specification element\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 204:
#line 1472 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed specification element\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 205:
#line 1477 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed specification element\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 206:
#line 1482 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed specification element\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 207:
#line 1487 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed specification element\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 208:
#line 1492 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed specification element\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 209:
#line 1501 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSortSpec((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed sort specification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 210:
#line 1510 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.list));
      gsDebugMsg("parsed sort declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 211:
#line 1515 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATconcat((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed sort declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 212:
#line 1524 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList0());
      int n = ATgetLength((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list));
      for (int i = 0; i < n; i++) {
        safe_assign(((*yyvalp).list), ATinsert(((*yyvalp).list), (ATerm) gsMakeSortId(ATAelementAt((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list), i))));
      }
      gsDebugMsg("parsed standard sort declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 213:
#line 1533 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) gsMakeSortRef((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl))));
      gsDebugMsg("parsed reference sort declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 214:
#line 1542 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list));
      gsDebugMsg("parsed domain\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 215:
#line 1547 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) gsMakeSortArrow((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl))));
      gsDebugMsg("parsed domain\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 216:
#line 1556 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeConsSpec((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed constructor operation specification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 217:
#line 1565 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeMapSpec((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed operation specification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 218:
#line 1575 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.list));
      gsDebugMsg("parsed operation declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 219:
#line 1580 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATconcat((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed operation declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 220:
#line 1589 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList0());
      int n = ATgetLength((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list));
      for (int i = 0; i < n; i++) {
        safe_assign(((*yyvalp).list), ATinsert(((*yyvalp).list), (ATerm) gsMakeOpId(ATAelementAt((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), i), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl))));
      }
      gsDebugMsg("parsed operation declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 221:
#line 1602 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeDataEqnSpec((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed equation specification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 222:
#line 1611 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed equation section\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 223:
#line 1616 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList0());
      int n = ATgetLength((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.list));
      for (int i = 0; i < n; i++) {
        ATermAppl DataEqn = ATAelementAt((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.list), i);
        ((*yyvalp).list) = ATinsert(((*yyvalp).list), (ATerm) gsMakeDataEqn((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), ATAgetArgument(DataEqn, 1),
          ATAgetArgument(DataEqn, 2), ATAgetArgument(DataEqn, 3)));
      }
      gsDebugMsg("parsed equation section\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 224:
#line 1631 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed equation declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 225:
#line 1636 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed equation declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 226:
#line 1645 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeDataEqn(ATmakeList0(), gsMakeNil(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed equation declaration\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 227:
#line 1650 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeDataEqn(ATmakeList0(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed equation declaration\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 228:
#line 1660 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.list));
      gsDebugMsg("parsed data variable declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 229:
#line 1665 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATconcat((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed data variable declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 230:
#line 1674 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList0());
      int n = ATgetLength((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list));
      for (int i = 0; i < n; i++) {
        safe_assign(((*yyvalp).list), ATinsert(((*yyvalp).list), (ATerm) gsMakeDataVarId(ATAelementAt((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), i), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl))));
      }
      gsDebugMsg("parsed data variable declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 231:
#line 1687 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActSpec((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed action specification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 232:
#line 1696 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.list));
      gsDebugMsg("parsed action declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 233:
#line 1701 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATconcat((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed action declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 234:
#line 1710 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList0());
      int n = ATgetLength((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list));
      for (int i = 0; i < n; i++) {
        ((*yyvalp).list) = ATinsert(((*yyvalp).list),
          (ATerm) gsMakeActId(ATAelementAt((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list), i), ATmakeList0()));
      }
      gsDebugMsg("parsed action declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 235:
#line 1720 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList0());
      int n = ATgetLength((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list));
      for (int i = 0; i < n; i++) {
        safe_assign(((*yyvalp).list), ATinsert(((*yyvalp).list), (ATerm) gsMakeActId(ATAelementAt((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), i), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.list))));
      }
      gsDebugMsg("parsed action declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 236:
#line 1733 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeProcEqnSpec(ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.list))));
      gsDebugMsg("parsed process specification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 237:
#line 1742 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed process declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 238:
#line 1747 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed process declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 239:
#line 1756 "mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeProcEqn(
        ATmakeList0(), gsMakeProcVarId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), ATmakeList0()), ATmakeList0(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed process declaration\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 240:
#line 1762 "mcrl2parser.yy"
    {
      ATermList SortExprs = ATmakeList0();
      int n = ATgetLength((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (6))].yystate.yysemantics.yysval.list));
      for (int i = 0; i < n; i++) {
        SortExprs = ATinsert(SortExprs, ATgetArgument(ATAelementAt((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (6))].yystate.yysemantics.yysval.list), i), 1));
      }      
      ((*yyvalp).appl) = gsMakeProcEqn(
        ATmakeList0(), gsMakeProcVarId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (6))].yystate.yysemantics.yysval.appl), ATreverse(SortExprs)), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (6))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((6) - (6))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed process declaration\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 241:
#line 1778 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list));
      gsDebugMsg("parsed data variable declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 242:
#line 1783 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATconcat((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed data variable declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 243:
#line 1792 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeProcessInit(ATmakeList0(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed initialisation\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 244:
#line 1804 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed state formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 245:
#line 1813 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 246:
#line 1817 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateForall((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 247:
#line 1822 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateExists((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 248:
#line 1827 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateNu((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 249:
#line 1832 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateMu((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 250:
#line 1841 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList0());
      gsDebugMsg("parsed fixpoint parameters\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 251:
#line 1846 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed fixpoint parameters\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 252:
#line 1856 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed data variable declaration and initialisations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 253:
#line 1861 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed data variable declaration and initialisations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 254:
#line 1870 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeDataVarIdInit(gsMakeDataVarId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.appl)), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed data variable declaration and initialisation\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 255:
#line 1879 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 256:
#line 1883 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateImp((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed implication\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 257:
#line 1892 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 258:
#line 1896 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateForall((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 259:
#line 1901 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateExists((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 260:
#line 1906 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateNu((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 261:
#line 1911 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateMu((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 262:
#line 1920 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 263:
#line 1924 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateAnd((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed conjunction\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 264:
#line 1929 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateOr((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed disjunction\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 265:
#line 1938 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 266:
#line 1942 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateForall((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 267:
#line 1947 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateExists((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 268:
#line 1952 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateNu((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 269:
#line 1957 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateMu((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 270:
#line 1966 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 271:
#line 1970 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateNot((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed prefix state formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 272:
#line 1975 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateMust((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed prefix state formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 273:
#line 1980 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateMay((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed prefix state formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 274:
#line 1985 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateYaledTimed((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed prefix state formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 275:
#line 1990 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateDelayTimed((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed prefix state formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 276:
#line 1999 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 277:
#line 2003 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateForall((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 278:
#line 2008 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateExists((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 279:
#line 2013 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateNu((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 280:
#line 2018 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateMu((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 281:
#line 2027 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed postfix state formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 282:
#line 2032 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateVar(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl), 0), ATLgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl), 1)));
      gsDebugMsg("parsed primary state formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 283:
#line 2037 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateTrue());
      gsDebugMsg("parsed primary state formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 284:
#line 2042 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateFalse());
      gsDebugMsg("parsed primary state formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 285:
#line 2047 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateYaled());
      gsDebugMsg("parsed primary state formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 286:
#line 2052 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateDelay());
      gsDebugMsg("parsed primary state formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 287:
#line 2057 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 288:
#line 2065 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed regular formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 289:
#line 2070 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed regular formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 290:
#line 2079 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 291:
#line 2083 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeRegAlt((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed alternative composition\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 292:
#line 2092 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 293:
#line 2096 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeRegAlt((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed alternative composition\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 294:
#line 2105 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 295:
#line 2109 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeRegSeq((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed sequential composition\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 296:
#line 2118 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 297:
#line 2122 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeRegSeq((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed sequential composition\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 298:
#line 2131 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 299:
#line 2135 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeRegTransOrNil((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed postfix regular formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 300:
#line 2140 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeRegTrans((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed postfix regular formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 301:
#line 2149 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 302:
#line 2153 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeRegTransOrNil((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed postfix regular formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 303:
#line 2158 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeRegTrans((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed postfix regular formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 304:
#line 2167 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeRegNil());
      gsDebugMsg("parsed primary regular formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 305:
#line 2172 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 306:
#line 2180 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed regular formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 307:
#line 2185 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeRegNil());
      gsDebugMsg("parsed primary regular formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 308:
#line 2190 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 309:
#line 2198 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed action formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 310:
#line 2207 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 311:
#line 2211 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActForall((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 312:
#line 2216 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActExists((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 313:
#line 2225 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 314:
#line 2229 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActImp((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed implication\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 315:
#line 2238 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 316:
#line 2242 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActForall((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 317:
#line 2247 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActExists((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 318:
#line 2256 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 319:
#line 2260 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActAnd((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed conjunction\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 320:
#line 2265 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActOr((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed disjunction\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 321:
#line 2274 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 322:
#line 2278 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActForall((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 323:
#line 2283 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActExists((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 324:
#line 2292 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 325:
#line 2296 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActAt((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed conjunction\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 326:
#line 2305 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 327:
#line 2309 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActNot((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed prefix action formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 328:
#line 2318 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 329:
#line 2322 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActForall((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 330:
#line 2327 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActExists((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 331:
#line 2336 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed primary action formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 332:
#line 2341 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed primary action formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 333:
#line 2346 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActTrue());
      gsDebugMsg("parsed primary action formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 334:
#line 2351 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActFalse());
      gsDebugMsg("parsed primary action formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 335:
#line 2356 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl));
    ;}
    break;


/* Line 930 of glr.c.  */
#line 4480 "mcrl2parser.cpp"
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
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(((yyGLRStackItem const *)yyvsp)[YYFILL ((yyi + 1) - (yynrhs))].yystate.yysemantics.yysval)
		       		       );
      fprintf (stderr, "\n");
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
	      yytoken = YYTRANSLATE (yychar);
	      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
	    }
	  else
	    yytoken = YYTRANSLATE (yychar);
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
   of the look-ahead.  */
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
	yytoken = YYTRANSLATE (yychar);
	YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
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
		  yytoken = YYTRANSLATE (yychar);
		  YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
		}
	      else
		yytoken = YYTRANSLATE (yychar);
	      yygetLRActions (yystate, yytoken, &yyaction, &yyconflicts);
	      if (*yyconflicts != 0)
		break;
	      if (yyisShiftAction (yyaction))
		{
		  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
		  if (yychar != YYEOF)
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
  if (yychar != YYEOF && yychar != YYEMPTY)
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
#ifdef YYDEBUG
static void yypstack (yyGLRStack* yystackp, size_t yyk)
  __attribute__ ((__unused__));
static void yypdumpstack (yyGLRStack* yystackp) __attribute__ ((__unused__));

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
      fprintf (stderr, "%3lu. ", (unsigned long int) (yyp - yystackp->yyitems));
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
		   yyp->yyoption.yyrule - 1,
		   (long int) YYINDEX (yyp->yyoption.yystate),
		   (long int) YYINDEX (yyp->yyoption.yynext));
	}
      fprintf (stderr, "\n");
    }
  fprintf (stderr, "Tops:");
  for (yyi = 0; yyi < yystackp->yytops.yysize; yyi += 1)
    fprintf (stderr, "%lu: %ld; ", (unsigned long int) yyi,
	     (long int) YYINDEX (yystackp->yytops.yystates[yyi]));
  fprintf (stderr, "\n");
}
#endif


#line 2362 "mcrl2parser.yy"
 

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

