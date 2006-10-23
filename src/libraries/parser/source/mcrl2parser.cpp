/* A Bison parser, made by GNU Bison 2.2.  */

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
#define YYBISON_VERSION "2.2"

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
#line 97 "../source/mcrl2parser.cpp"

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
#define YYLAST   1417

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
     217,    -1,    86,    -1,    36,   208,    37,    -1,   218,    -1,
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
       0,   142,   142,   147,   152,   157,   162,   167,   179,   188,
     192,   201,   210,   215,   224,   233,   237,   246,   251,   260,
     265,   275,   279,   288,   293,   302,   307,   316,   321,   326,
     331,   339,   343,   347,   351,   355,   363,   367,   371,   382,
     391,   395,   404,   409,   418,   427,   431,   436,   441,   450,
     454,   464,   468,   473,   482,   486,   492,   502,   506,   511,
     520,   524,   530,   540,   544,   549,   558,   562,   568,   574,
     580,   586,   596,   600,   610,   614,   624,   628,   638,   642,
     648,   658,   662,   668,   678,   682,   688,   698,   702,   707,
     712,   721,   725,   730,   739,   743,   752,   757,   766,   771,
     775,   779,   783,   791,   796,   801,   806,   811,   820,   825,
     830,   839,   844,   853,   862,   871,   883,   888,   897,   902,
     911,   916,   928,   937,   941,   950,   954,   963,   967,   972,
     981,   985,   994,   998,  1007,  1011,  1020,  1024,  1029,  1038,
    1042,  1051,  1055,  1064,  1068,  1077,  1081,  1086,  1091,  1100,
    1104,  1113,  1117,  1126,  1130,  1139,  1143,  1152,  1156,  1165,
    1169,  1174,  1179,  1188,  1192,  1201,  1205,  1209,  1213,  1221,
    1226,  1235,  1240,  1245,  1250,  1255,  1264,  1269,  1278,  1283,
    1292,  1297,  1306,  1311,  1320,  1329,  1334,  1343,  1348,  1357,
    1362,  1367,  1376,  1385,  1390,  1399,  1404,  1413,  1418,  1430,
    1439,  1444,  1453,  1458,  1463,  1468,  1473,  1478,  1483,  1492,
    1501,  1506,  1515,  1524,  1533,  1538,  1547,  1556,  1566,  1571,
    1580,  1593,  1602,  1607,  1622,  1627,  1636,  1641,  1651,  1656,
    1665,  1678,  1687,  1692,  1701,  1711,  1724,  1733,  1738,  1747,
    1753,  1769,  1774,  1783,  1795,  1804,  1808,  1813,  1818,  1823,
    1833,  1837,  1847,  1852,  1861,  1870,  1874,  1883,  1887,  1892,
    1897,  1902,  1911,  1915,  1920,  1929,  1933,  1938,  1943,  1948,
    1957,  1961,  1966,  1971,  1976,  1981,  1990,  1994,  1999,  2004,
    2009,  2018,  2023,  2028,  2033,  2038,  2043,  2048,  2056,  2061,
    2070,  2074,  2083,  2087,  2096,  2100,  2109,  2113,  2122,  2126,
    2131,  2140,  2144,  2149,  2158,  2163,  2171,  2176,  2181,  2189,
    2198,  2202,  2207,  2216,  2220,  2229,  2233,  2238,  2247,  2251,
    2256,  2265,  2269,  2274,  2283,  2287,  2296,  2300,  2309,  2313,
    2318,  2327,  2332,  2337,  2342,  2347
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
     250,   287,     0,     0,     0,   328,   327,     0,     0,   306,
       0,     0,     0,     0,     0,   299,   300,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   275,
     274,     0,     0,     0,     0,   257,   256,     0,     0,     0,
       0,   265,   263,   264,    22,    27,    25,     0,    23,    18,
      36,    37,    38,     0,     0,    97,   115,   113,     0,   112,
       0,   179,   230,    46,   242,    47,    48,     0,     0,    41,
       0,     0,     0,     0,     0,     0,    95,   121,   126,     0,
       0,   193,   197,   192,     0,     0,     0,     0,     0,   182,
       0,     0,   187,   189,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   213,   211,   220,   219,
     223,   229,     0,   226,   225,   214,   235,   233,   239,     0,
     238,     0,     0,     0,     0,     0,     0,     0,   308,   305,
     335,     0,     0,     0,   272,     0,   307,   291,   292,   296,
     306,   295,     0,     0,   315,   314,   325,     0,     0,   321,
     319,   320,   273,   246,   247,   281,     0,     0,   252,     0,
       0,     0,     0,   250,   250,     0,     0,   250,   250,     0,
       0,    21,    92,    93,   114,    44,    43,     0,     0,     0,
       0,     0,     0,   177,     0,     0,     0,   196,     0,     0,
       0,     0,   181,     0,     0,   186,     0,     0,     0,     0,
     138,     0,   144,   154,     0,   163,   158,     0,     0,     0,
     147,     0,     0,   161,     0,     0,     0,     0,   277,   278,
       0,     0,     0,     0,   311,   312,   332,     0,   302,   303,
       0,     0,     0,     0,     0,     0,     0,   251,   249,   248,
       0,     0,     0,     0,     0,     0,     0,     0,    26,    24,
      20,    52,    53,    58,    59,    64,    65,   171,   194,   198,
     175,   172,   184,   183,   173,   188,   190,   191,   174,   140,
     150,     0,     0,     0,   131,   135,   146,     0,   160,     0,
     227,   215,     0,   280,   279,   329,   330,   293,   297,     0,
       0,     0,     0,     0,   253,   258,   259,     0,     0,   266,
     267,     0,     0,     0,     0,     0,   148,   162,   240,   316,
     317,   322,   323,     0,   261,   260,   269,   268,   164,   254
};

/* YYPDEFGOTO[NTERM-NUM].  */
static const short int yydefgoto[] =
{
      -1,     7,   436,    20,    21,    22,    23,    24,   133,   134,
     266,   437,   438,    25,    26,    27,   210,    44,   292,   293,
      45,    46,   297,    47,   301,    48,   306,    49,    50,    51,
      52,    53,    54,    55,    56,   144,    57,   148,    58,    59,
      60,   152,   153,    61,   154,   236,    65,    78,    79,    80,
      81,    82,   352,    83,   355,    84,   659,    85,   343,   360,
     660,    86,   346,    87,   347,   365,   596,    88,    89,    90,
     329,   156,   336,   478,   479,   339,   481,   482,   483,   473,
     332,   474,    99,   100,   101,   102,   202,   203,   506,   103,
     104,   205,   206,   105,   106,   211,   212,   208,   157,   107,
     214,   215,   108,   217,   218,   158,   109,   124,   125,   417,
     547,   548,   126,   426,   127,   432,   128,   226,   129,   397,
     238,   527,   239,   240,   241,   242,   243,   244,   530,   246,
     247,   535,   248,   540,   249,   250,   396,   251
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -576
static const short int yypact[] =
{
     242,    46,  1034,     1,   661,   169,  1163,    76,    46,    55,
    -576,  -576,  -576,  -576,  -576,    91,   129,   163,  -576,  -576,
    -576,   196,   195,  -576,    52,  -576,  -576,  -576,  1148,    85,
    1148,  1034,  -576,  1034,  -576,  1049,  -576,  -576,   147,   147,
     147,  -576,  -576,  -576,    68,  -576,  -576,   223,   272,   300,
      48,   240,   257,   150,   255,   237,  -576,   243,  -576,  -576,
    -576,  -576,  -576,   247,  -576,   224,  -576,   457,  -576,  -576,
     147,   251,   258,   271,   274,   289,  1380,   331,  -576,  -576,
    -576,   308,  -576,   113,  -576,  -576,   151,  -576,   326,  -576,
    -576,   256,   147,   147,   147,  1034,   147,   278,   661,  -576,
     169,  -576,  -576,  -576,  -576,  -576,  -576,  -576,  -576,  -576,
    1189,  1163,   290,   290,  -576,  -576,   147,   147,   334,   285,
     293,   357,   361,  -576,  -576,  -576,  -576,   382,   311,  -576,
    -576,   368,    43,   375,  -576,    46,    46,    46,    46,    46,
    -576,   147,   147,  -576,  -576,  -576,   390,  -576,    44,   401,
     404,    38,    49,  -576,   414,  -576,   318,  -576,   322,   324,
     328,   349,   585,  1091,  1091,  1106,  1106,  1148,  1148,  1148,
    1148,  1148,  1148,  1148,  1148,  1148,  1148,  1148,  1148,  1148,
    1148,  1034,  1034,   352,   331,   437,   330,   319,   323,   319,
     325,   339,   718,   661,   740,   740,   797,   819,  1148,   876,
     454,   459,   256,   464,   356,   147,   467,   147,    11,   469,
      59,  1034,   479,   358,   147,   482,   105,   278,   511,   513,
    -576,   147,   147,   417,   463,  -576,  -576,   540,  1280,   290,
    -576,  -576,   147,   147,   550,   137,  -576,   548,  -576,  -576,
     567,  -576,   209,  -576,  -576,   171,  -576,  -576,   582,   575,
     374,  -576,   574,   395,   429,  1034,   591,   591,  1148,  1148,
    1224,  1250,  1250,  -576,   531,   622,  -576,    55,   583,   594,
     596,  -576,  -576,  -576,   433,   443,  -576,  1034,  -576,    46,
    1034,  -576,  1034,  -576,  1034,   535,    46,  1034,   147,  1034,
    1034,   630,    42,  -576,   147,   147,  -576,  -576,   147,   147,
    -576,  -576,  -576,   147,   147,  -576,  -576,  -576,  -576,  -576,
    -576,  -576,  -576,   478,   478,  -576,   255,   255,   237,   237,
    -576,  -576,    50,   119,  -576,  -576,   661,  -576,   147,   631,
    -576,   576,   637,   657,  -576,   602,   666,  -576,   576,   668,
     147,  -576,  -576,  -576,   676,   154,   675,  -576,   103,  -576,
     147,  -576,  -576,  -576,   147,  -576,  -576,   147,   704,  -576,
    -576,  -576,   147,   705,  -576,  -576,    46,   686,  -576,    46,
     687,  -576,  1034,   688,  -576,  1034,  1034,   693,  -576,    46,
     696,  -576,   661,   147,   699,  -576,  -576,   480,   489,   591,
     591,  -576,  1289,   147,   147,  -576,  -576,   707,   708,   709,
     491,   495,  1034,  1189,   506,   214,   234,   506,  1309,  1148,
    1324,  1324,  1189,  1163,  1163,   710,   643,   706,   715,  -576,
    -576,   147,   147,   664,   671,  -576,  -576,   147,   147,   672,
     673,  -576,  -576,  -576,  -576,   724,  -576,   134,  -576,  -576,
    -576,  -576,  -576,    85,    85,  -576,  -576,  -576,   404,  -576,
     719,  -576,  -576,  -576,  -576,  -576,  -576,  1034,   349,  -576,
     501,   512,   519,   521,   526,   528,  -576,  -576,  -576,    71,
     661,  -576,  -576,   720,    81,   661,   661,   753,    90,  -576,
     661,    92,  -576,   756,   661,   530,   898,   955,  1148,   977,
     532,   536,   539,   955,   544,   977,  -576,  -576,  -576,  -576,
    1034,  -576,   741,  -576,  -576,   758,  -576,  -576,  -576,   160,
    -576,  1189,  1189,   742,   743,   734,   554,   556,  -576,  -576,
    -576,  1289,  1289,   736,  -576,   290,  -576,  -576,   751,   262,
    -576,   761,   147,   147,  -576,  -576,  -576,   147,   147,  -576,
    -576,  -576,  -576,  -576,  -576,  -576,   748,   175,  -576,  1163,
    1163,   570,   573,   591,   591,   577,   579,   591,   591,    46,
     622,   749,  -576,  -576,  -576,  -576,  -576,   585,   585,  1091,
    1091,  1106,  1106,  -576,   750,   701,   576,  -576,   752,   762,
     703,   602,  -576,   763,   576,  -576,    56,   764,   718,   147,
    -576,   147,  -576,  -576,   147,  -576,  -576,   740,   797,   819,
    -576,   780,   876,  -576,   785,  1034,    46,   760,  -576,  -576,
    1189,  1189,  1280,  1280,  -576,  -576,  -576,   506,  -576,  -576,
     506,   584,   586,   598,   609,    46,   643,  -576,  -576,  -576,
    1224,  1224,   781,   782,  1250,  1250,   783,   784,  -576,  -576,
    -576,  -576,  -576,  -576,  -576,  -576,  -576,  -576,  -576,  -576,
    -576,  -576,  -576,  -576,  -576,  -576,  -576,  -576,  -576,  -576,
    -576,   611,   613,   615,  -576,  -576,  -576,   819,  -576,   876,
    -576,  -576,   661,  -576,  -576,  -576,  -576,  -576,  -576,  1309,
    1309,  1324,  1324,   786,  -576,  -576,  -576,  1224,  1224,  -576,
    -576,  1250,  1250,   898,   955,   977,  -576,  -576,  -576,  -576,
    -576,  -576,  -576,  1034,  -576,  -576,  -576,  -576,  -576,  -576
};

/* YYPGOTO[NTERM-NUM].  */
static const short int yypgoto[] =
{
    -576,  -576,     9,   683,   444,  -576,   685,  -128,  -576,   558,
     265,  -576,   270,  -576,  -576,  -576,    -2,  -576,  -576,   376,
     -89,  -130,    77,  -144,  -111,  -143,   -93,  -576,    57,  -576,
    -576,   474,   486,   490,    -3,   244,  -576,    28,  -576,  -576,
    -576,  -576,   555,  -576,  -576,   829,  -576,    40,   -63,   645,
     516,  -155,  -181,  -576,   241,  -187,  -190,  -179,  -576,  -459,
    -145,  -189,  -576,  -171,  -465,  -398,  -451,  -184,  -576,  -576,
     654,     0,  -576,  -576,   264,  -576,  -576,   263,  -324,  -576,
    -576,  -576,  -576,  -576,   746,  -576,  -576,   646,  -576,  -576,
    -576,   757,    35,  -576,  -576,   477,  -173,  -576,   -49,  -576,
    -576,   638,  -576,  -576,   639,    41,  -576,   754,  -364,  -220,
    -576,   228,  -212,  -575,  -246,  -226,   -97,  -345,  -576,   590,
     635,   249,  -576,  -350,  -576,  -336,  -576,  -576,   -61,   185,
    -387,    31,  -369,  -405,  -576,  -216,   100,  -576
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -308
static const short int yytable[] =
{
      43,    77,   342,   345,   185,   341,   541,   472,   348,   356,
      19,   273,   395,   225,   353,   431,   431,   131,   359,   300,
     300,   534,   305,   305,   595,   140,   143,   145,   364,   146,
     595,   147,   296,   150,   600,   219,   433,   418,   377,   351,
     351,   539,   539,    66,   604,   209,   123,   344,   425,   543,
     544,   245,   245,   302,   528,   685,   686,   531,   524,   167,
     168,   372,   -14,   151,   184,   146,   277,   542,   529,   375,
     458,   529,   277,   307,   264,    62,   130,   282,   277,   265,
     159,   160,     8,   281,   278,   376,   -14,   466,    63,   169,
     170,   201,   204,   204,   283,    77,   213,   603,   155,   285,
       9,    10,    11,    12,    13,    14,    15,    16,    17,   576,
      28,   186,   704,   705,   459,   171,   573,    29,   581,    30,
     584,    31,   194,    32,    33,  -157,   577,   135,    34,    35,
     656,   382,   195,    18,   196,   582,  -155,   585,   489,   161,
     666,   383,   132,   657,   268,   269,   270,   277,    36,    37,
     123,   123,    66,    66,   141,   142,   467,   253,   254,   373,
    -307,  -307,   560,   174,  -307,   136,   608,   609,   399,   513,
     514,   561,    41,    42,   175,   176,   320,   321,   197,   147,
     147,   487,   274,   275,   198,   628,   629,   198,   288,    77,
      77,    77,    77,    77,   358,   361,   363,   607,   453,   137,
     455,   456,   201,   626,   668,   204,   138,   204,   696,   322,
     323,  -288,   627,  -288,   213,    91,    92,    93,    94,    95,
      96,    97,    98,   324,   308,   309,   310,   311,   312,   139,
     595,   315,   405,   406,   155,   666,   407,  -302,  -302,   454,
     370,  -302,   370,   162,   708,     1,     2,     3,     4,     5,
       6,   273,   649,   415,   172,   419,   420,  -303,  -303,   183,
     179,  -303,   387,   388,   180,   673,   674,   528,    66,    66,
     678,   697,   173,   400,   401,   445,   701,   702,   447,   181,
     448,   529,   450,   182,   529,   618,   619,   187,   446,   620,
     163,   164,   534,   534,   188,   452,   590,   668,   345,   341,
     123,   123,   123,   348,   345,   348,   225,   189,   359,   348,
     190,   348,   539,   539,   359,   225,   165,   166,   364,   508,
     177,   178,   228,    77,   364,   191,   229,   377,   469,   261,
     262,   515,   193,   632,   633,   460,   461,   636,   637,   462,
     463,   192,   592,   200,   464,   465,   285,   286,   601,   287,
     288,   289,   288,   230,   231,   290,   288,   326,   288,   232,
     233,   199,   327,   328,    62,   216,   330,   331,   334,   335,
     255,   234,   256,   502,   503,   496,   235,    63,   498,    77,
     257,   485,   337,   338,   285,   369,   285,   379,   431,   431,
     258,   490,   410,   411,   259,   491,   395,   395,   492,   345,
     523,   341,   260,   494,   348,   263,   536,   574,   689,   690,
     267,   356,   578,   579,   225,   225,   664,   583,   425,   425,
     359,   587,   413,   288,   509,   300,   300,   276,   305,   305,
     279,   364,    66,   280,   516,   517,   291,   296,   296,    63,
     143,   143,   351,   123,    66,   431,   431,    66,    66,   284,
      66,    66,   123,   123,   123,   565,   414,   288,   643,   644,
     443,   288,   551,   552,   399,   706,   707,    77,   555,   556,
     444,   288,    77,    77,   325,   425,   425,    77,   645,   646,
     366,    77,    28,    77,   358,   593,   363,   285,   359,    29,
     358,    30,   363,    67,   368,    32,    33,   371,   364,   374,
      34,    35,   175,   176,   389,   345,   341,   511,   288,   378,
     348,   348,   381,   225,   225,   359,   512,   288,   521,   288,
      36,    37,   522,   288,   364,    38,    39,    40,   567,   288,
      68,    69,    70,    71,    72,    73,    74,    75,   228,   568,
     288,   385,   525,   386,    76,    42,   569,   288,   570,   288,
     390,   123,   123,   571,   288,   572,   288,   588,   288,   597,
     288,    66,    66,   598,   288,    66,   599,   288,   638,   230,
     231,   602,   288,   621,   622,   232,   233,   391,   623,   624,
      62,   612,   288,   613,   288,    77,   402,   234,   403,   123,
     123,   404,   526,    63,    77,    77,   358,   630,   288,   363,
     631,   288,   408,   670,   634,   288,   635,   288,   409,   698,
      28,   679,   288,   680,   288,   671,   412,    29,   434,    30,
     440,    31,   451,    32,    33,   681,   288,   416,    34,    35,
     661,   441,   662,   442,   683,   663,   682,   288,   693,   288,
     694,   288,   695,   288,   641,   642,   313,   314,    36,    37,
     123,   123,    66,    66,   294,   295,   457,    66,     8,   470,
      66,   316,   317,   471,   358,   475,   363,   318,   319,    77,
     123,   123,    41,    42,   123,   123,     9,    10,    11,    12,
      13,    14,    15,    16,    17,   476,    28,   562,   563,   477,
      77,   358,   363,    29,   480,    30,   484,    67,   486,    32,
      33,   709,   237,   252,    34,    35,   614,   615,   488,   435,
     699,   700,   675,   676,   493,   495,   497,   499,   501,    66,
      66,    66,    66,   504,    36,    37,   507,   123,   123,   510,
     546,   123,   123,   549,    68,    69,    70,    71,    72,    73,
      74,    75,   550,    28,   518,   519,   520,   545,    76,    42,
      29,   553,    30,   559,    67,   575,    32,    33,   554,   557,
     558,    34,    35,   580,   564,    28,   586,   605,   606,   610,
     611,   520,    29,   616,    30,   617,    67,   625,    32,    33,
     264,    36,    37,    34,    35,  -297,   672,   647,   648,   650,
     652,    68,    69,   340,    71,    72,    73,    74,    75,   651,
     654,   658,   667,    36,    37,    76,    42,   669,   687,   688,
     691,   692,   703,    68,    69,   350,    71,    72,    73,    74,
      75,   271,    28,   505,   272,   439,   640,    76,    42,    29,
     639,    30,    64,    67,   566,    32,    33,   449,   349,   665,
      34,    35,   468,   333,    28,   653,   220,   655,   367,   500,
     207,    29,   380,    30,   684,    67,   384,    32,    33,     0,
      36,    37,    34,    35,   398,   227,   677,     0,     0,     0,
      68,    69,   354,    71,    72,    73,    74,    75,     0,     0,
       0,     0,    36,    37,    76,    42,     0,     0,     0,     0,
       0,     0,    68,    69,   357,    71,    72,    73,    74,    75,
       0,    28,     0,     0,     0,     0,    76,    42,    29,     0,
      30,     0,    67,     0,    32,    33,     0,     0,     0,    34,
      35,     0,     0,    28,     0,     0,     0,     0,     0,     0,
      29,     0,    30,     0,    67,     0,    32,    33,     0,    36,
      37,    34,    35,     0,     0,     0,     0,     0,     0,    68,
      69,   362,    71,    72,    73,    74,    75,     0,     0,     0,
       0,    36,    37,    76,    42,     0,     0,     0,     0,     0,
       0,    68,    69,   589,    71,    72,    73,    74,    75,     0,
      28,     0,     0,     0,     0,    76,    42,    29,     0,    30,
       0,    67,     0,    32,    33,     0,     0,     0,    34,    35,
       0,     0,    28,     0,     0,     0,     0,     0,     0,    29,
       0,    30,     0,    67,     0,    32,    33,     0,    36,    37,
      34,    35,     0,     0,     0,     0,     0,     0,    68,    69,
     591,    71,    72,    73,    74,    75,     0,     0,     0,     0,
      36,    37,    76,    42,     0,     0,     0,     0,     0,     0,
      68,    69,   594,    71,    72,    73,    74,    75,     0,    28,
       0,     0,     0,     0,    76,    42,    29,     0,    30,     0,
      31,     0,    32,    33,    28,     0,     0,    34,    35,     0,
       0,    29,     0,    30,     0,    31,     0,    32,    33,     0,
       0,     0,    34,    35,     0,     0,     0,    36,    37,     0,
       0,     0,    38,    39,    40,     0,     0,     0,     0,     0,
       0,     0,    36,    37,     0,     0,    28,    38,    39,    40,
       0,    41,    42,    29,     0,    30,     0,    31,     0,    32,
      33,    28,     0,     0,    34,    35,   149,    42,    29,     0,
      30,     0,    31,     0,    32,    33,     0,     0,     0,    34,
      35,     0,     0,     0,    36,    37,     0,     0,     0,     0,
     298,   299,     0,     0,     0,     0,     0,     0,     0,    36,
      37,     0,     0,    28,     0,   303,   304,     0,    41,    42,
      29,     0,    30,     0,    31,     0,    32,    33,     0,     0,
       0,    34,    35,    41,    42,   110,     0,     0,     0,   111,
       0,     0,   112,     0,   113,     0,     0,     0,     0,     0,
       0,    36,    37,     0,     0,     0,     0,     0,     0,     0,
       0,   110,     0,     0,     0,   111,   114,   115,   112,     0,
     113,     0,   116,   117,     0,    41,    42,     0,     0,     0,
       0,     0,     0,     0,   118,   119,   120,   121,   122,     0,
      63,     0,   114,   115,     0,     0,   110,     0,   221,   222,
     111,     0,     0,   112,     0,   113,     0,     0,     0,     0,
     118,   223,   224,   121,   122,     0,    63,     0,     0,     0,
       0,     0,   110,     0,     0,     0,   111,   114,   115,   112,
       0,   113,     0,   421,   422,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   118,   423,   424,   121,   122,
       0,    63,   228,   114,   115,     0,   392,     0,     0,   427,
     428,   228,     0,     0,     0,   392,     0,     0,     0,     0,
       0,   118,   429,   430,   121,   122,     0,    63,     0,     0,
       0,   228,     0,   230,   231,   392,     0,     0,     0,   393,
     394,     0,   230,   231,    62,     0,   228,     0,   232,   233,
     392,   234,     0,    62,     0,     0,     0,    63,     0,     0,
     234,     0,   230,   231,     0,     0,    63,     0,   532,   533,
    -120,     0,     0,    62,     0,     0,     0,   230,   231,  -120,
     234,     0,     0,   537,   538,     0,    63,     0,    62,   -98,
       0,  -120,  -120,     0,   -98,   234,     0,   -98,     0,     0,
    -120,    63,     0,  -120,     0,  -120,   182,   -98
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
       0,     0,     0,     0,     0,     0,    17,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    15,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    23,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      11,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      13,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    19,    21,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
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
       0,     0,     0,     0,     0,     0,     7,     9
};

/* YYCONFL[I] -- lists of conflicting rule numbers, each terminated by
   0, pointed into by YYCONFLP.  */
static const short int yyconfl[] =
{
       0,   120,     0,   120,     0,   120,     0,    98,     0,   120,
       0,    16,     0,    98,     0,   296,     0,   157,     0,   289,
       0,   288,     0,   296,     0
};

static const short int yycheck[] =
{
       2,     4,   192,   192,    67,   192,   411,   331,   192,   196,
       1,   139,   228,   110,   195,   261,   262,     8,   197,   163,
     164,   408,   165,   166,   489,    28,    29,    30,   199,    31,
     495,    33,   162,    35,   493,    98,   262,   257,   211,   194,
     195,   410,   411,     3,   495,    94,     6,   192,   260,   413,
     414,   112,   113,   164,   404,   630,   631,   407,   403,    11,
      12,    50,    10,    35,    67,    67,    28,   412,   404,    10,
      28,   407,    28,   166,    31,    74,     0,    28,    28,    36,
      39,    40,    36,    45,    40,    26,    34,    37,    87,    41,
      42,    91,    92,    93,    45,    98,    96,   495,    87,    28,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    28,
      25,    70,   687,   688,    72,    67,    45,    32,    28,    34,
      28,    36,     9,    38,    39,    22,    45,    36,    43,    44,
      74,    26,    19,    87,    21,    45,    33,    45,    35,    71,
     599,    36,    87,    87,   135,   136,   137,    28,    63,    64,
     110,   111,   112,   113,    69,    70,    37,   116,   117,   208,
      23,    24,    28,    13,    27,    36,   511,   512,   229,   389,
     390,    37,    87,    88,    24,    25,   179,   180,    27,   181,
     182,    27,   141,   142,    33,   549,   550,    33,    28,   192,
     193,   194,   195,   196,   197,   198,   199,    37,   287,    36,
     289,   290,   202,    28,   602,   205,    10,   207,   667,   181,
     182,    40,    37,    42,   214,    46,    47,    48,    49,    50,
      51,    52,    53,   183,   167,   168,   169,   170,   171,    34,
     695,   174,    23,    24,    87,   694,    27,    23,    24,   288,
     205,    27,   207,    20,   695,     3,     4,     5,     6,     7,
       8,   379,   576,   255,    14,   258,   259,    23,    24,    35,
      23,    27,   221,   222,    27,   610,   611,   617,   228,   229,
     620,   669,    15,   232,   233,   277,   681,   682,   280,    36,
     282,   617,   284,    36,   620,    23,    24,    36,   279,    27,
      18,    19,   679,   680,    36,   286,   486,   695,   487,   486,
     260,   261,   262,   487,   493,   489,   403,    36,   487,   493,
      36,   495,   681,   682,   493,   412,    16,    17,   489,   382,
      65,    66,    32,   326,   495,    36,    36,   500,   328,    18,
      19,   392,    24,   553,   554,   294,   295,   557,   558,   298,
     299,    10,   487,    87,   303,   304,    28,    29,   493,    27,
      28,    27,    28,    63,    64,    27,    28,    27,    28,    69,
      70,    35,    43,    44,    74,    87,    43,    44,    43,    44,
      36,    81,    87,   375,   376,   366,    86,    87,   369,   382,
      87,   340,    43,    44,    28,    29,    28,    29,   634,   635,
      33,   350,    18,    19,    33,   354,   612,   613,   357,   588,
     402,   588,    20,   362,   588,    37,   409,   470,   634,   635,
      35,   598,   475,   476,   511,   512,   597,   480,   630,   631,
     599,   484,    27,    28,   383,   569,   570,    37,   571,   572,
      29,   602,   392,    29,   393,   394,    87,   567,   568,    87,
     443,   444,   597,   403,   404,   691,   692,   407,   408,    35,
     410,   411,   412,   413,   414,   457,    27,    28,   569,   570,
      27,    28,   421,   422,   525,   691,   692,   470,   427,   428,
      27,    28,   475,   476,    37,   687,   688,   480,   571,   572,
      26,   484,    25,   486,   487,   488,   489,    28,   667,    32,
     493,    34,   495,    36,    30,    38,    39,    30,   669,    30,
      43,    44,    24,    25,    87,   694,   693,    27,    28,    30,
     694,   695,    30,   610,   611,   694,    27,    28,    27,    28,
      63,    64,    27,    28,   695,    68,    69,    70,    27,    28,
      73,    74,    75,    76,    77,    78,    79,    80,    32,    27,
      28,    30,    36,    30,    87,    88,    27,    28,    27,    28,
      87,   511,   512,    27,    28,    27,    28,    27,    28,    27,
      28,   521,   522,    27,    28,   525,    27,    28,   559,    63,
      64,    27,    28,   532,   533,    69,    70,    37,   537,   538,
      74,    27,    28,    27,    28,   588,    36,    81,    40,   549,
     550,    24,    86,    87,   597,   598,   599,    27,    28,   602,
      27,    28,    20,   605,    27,    28,    27,    28,    33,   672,
      25,    27,    28,    27,    28,   606,    42,    32,    87,    34,
      37,    36,    87,    38,    39,    27,    28,    36,    43,    44,
     589,    37,   591,    37,   625,   594,    27,    28,    27,    28,
      27,    28,    27,    28,   567,   568,   172,   173,    63,    64,
     610,   611,   612,   613,    69,    70,    26,   617,    36,    28,
     620,   175,   176,    87,   667,    28,   669,   177,   178,   672,
     630,   631,    87,    88,   634,   635,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    28,    25,   443,   444,    87,
     693,   694,   695,    32,    28,    34,    28,    36,    22,    38,
      39,   703,   112,   113,    43,    44,   521,   522,    33,    87,
     679,   680,   612,   613,    10,    10,    30,    30,    30,   679,
     680,   681,   682,    30,    63,    64,    30,   687,   688,    30,
      87,   691,   692,    27,    73,    74,    75,    76,    77,    78,
      79,    80,    27,    25,    37,    37,    37,    37,    87,    88,
      32,    87,    34,    29,    36,    35,    38,    39,    87,    87,
      87,    43,    44,    10,    45,    25,    10,    26,    10,    27,
      27,    37,    32,    37,    34,    24,    36,    29,    38,    39,
      31,    63,    64,    43,    44,    24,    26,    37,    87,    37,
      87,    73,    74,    75,    76,    77,    78,    79,    80,    37,
      37,    37,    22,    63,    64,    87,    88,    22,    27,    27,
      27,    27,    26,    73,    74,    75,    76,    77,    78,    79,
      80,   138,    25,   379,   139,   267,   561,    87,    88,    32,
     560,    34,     3,    36,   458,    38,    39,   282,   193,   598,
      43,    44,   326,   189,    25,   581,   100,   584,   202,   372,
      93,    32,   214,    34,   626,    36,   217,    38,    39,    -1,
      63,    64,    43,    44,   229,   111,   617,    -1,    -1,    -1,
      73,    74,    75,    76,    77,    78,    79,    80,    -1,    -1,
      -1,    -1,    63,    64,    87,    88,    -1,    -1,    -1,    -1,
      -1,    -1,    73,    74,    75,    76,    77,    78,    79,    80,
      -1,    25,    -1,    -1,    -1,    -1,    87,    88,    32,    -1,
      34,    -1,    36,    -1,    38,    39,    -1,    -1,    -1,    43,
      44,    -1,    -1,    25,    -1,    -1,    -1,    -1,    -1,    -1,
      32,    -1,    34,    -1,    36,    -1,    38,    39,    -1,    63,
      64,    43,    44,    -1,    -1,    -1,    -1,    -1,    -1,    73,
      74,    75,    76,    77,    78,    79,    80,    -1,    -1,    -1,
      -1,    63,    64,    87,    88,    -1,    -1,    -1,    -1,    -1,
      -1,    73,    74,    75,    76,    77,    78,    79,    80,    -1,
      25,    -1,    -1,    -1,    -1,    87,    88,    32,    -1,    34,
      -1,    36,    -1,    38,    39,    -1,    -1,    -1,    43,    44,
      -1,    -1,    25,    -1,    -1,    -1,    -1,    -1,    -1,    32,
      -1,    34,    -1,    36,    -1,    38,    39,    -1,    63,    64,
      43,    44,    -1,    -1,    -1,    -1,    -1,    -1,    73,    74,
      75,    76,    77,    78,    79,    80,    -1,    -1,    -1,    -1,
      63,    64,    87,    88,    -1,    -1,    -1,    -1,    -1,    -1,
      73,    74,    75,    76,    77,    78,    79,    80,    -1,    25,
      -1,    -1,    -1,    -1,    87,    88,    32,    -1,    34,    -1,
      36,    -1,    38,    39,    25,    -1,    -1,    43,    44,    -1,
      -1,    32,    -1,    34,    -1,    36,    -1,    38,    39,    -1,
      -1,    -1,    43,    44,    -1,    -1,    -1,    63,    64,    -1,
      -1,    -1,    68,    69,    70,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    63,    64,    -1,    -1,    25,    68,    69,    70,
      -1,    87,    88,    32,    -1,    34,    -1,    36,    -1,    38,
      39,    25,    -1,    -1,    43,    44,    87,    88,    32,    -1,
      34,    -1,    36,    -1,    38,    39,    -1,    -1,    -1,    43,
      44,    -1,    -1,    -1,    63,    64,    -1,    -1,    -1,    -1,
      69,    70,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    63,
      64,    -1,    -1,    25,    -1,    69,    70,    -1,    87,    88,
      32,    -1,    34,    -1,    36,    -1,    38,    39,    -1,    -1,
      -1,    43,    44,    87,    88,    32,    -1,    -1,    -1,    36,
      -1,    -1,    39,    -1,    41,    -1,    -1,    -1,    -1,    -1,
      -1,    63,    64,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    32,    -1,    -1,    -1,    36,    63,    64,    39,    -1,
      41,    -1,    69,    70,    -1,    87,    88,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    81,    82,    83,    84,    85,    -1,
      87,    -1,    63,    64,    -1,    -1,    32,    -1,    69,    70,
      36,    -1,    -1,    39,    -1,    41,    -1,    -1,    -1,    -1,
      81,    82,    83,    84,    85,    -1,    87,    -1,    -1,    -1,
      -1,    -1,    32,    -1,    -1,    -1,    36,    63,    64,    39,
      -1,    41,    -1,    69,    70,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    81,    82,    83,    84,    85,
      -1,    87,    32,    63,    64,    -1,    36,    -1,    -1,    69,
      70,    32,    -1,    -1,    -1,    36,    -1,    -1,    -1,    -1,
      -1,    81,    82,    83,    84,    85,    -1,    87,    -1,    -1,
      -1,    32,    -1,    63,    64,    36,    -1,    -1,    -1,    69,
      70,    -1,    63,    64,    74,    -1,    32,    -1,    69,    70,
      36,    81,    -1,    74,    -1,    -1,    -1,    87,    -1,    -1,
      81,    -1,    63,    64,    -1,    -1,    87,    -1,    69,    70,
       0,    -1,    -1,    74,    -1,    -1,    -1,    63,    64,     9,
      81,    -1,    -1,    69,    70,    -1,    87,    -1,    74,    19,
      -1,    21,    22,    -1,    24,    81,    -1,    27,    -1,    -1,
      30,    87,    -1,    33,    -1,    35,    36,    37
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
      87,    37,    36,    69,    70,   224,   225,   208,   209,   217,
     194,   194,    36,    40,    24,    23,    24,    27,    20,    33,
      18,    19,    42,    27,    27,   105,    36,   198,   198,   123,
     123,    69,    70,    82,    83,   201,   202,    69,    70,    82,
      83,   203,   204,   204,    87,    87,    91,   100,   101,    98,
      37,    37,    37,    27,    27,   105,    91,   105,   105,   131,
     105,    87,    91,   109,   187,   109,   109,    26,    28,    72,
     194,   194,   194,   194,   194,   194,    37,    37,   139,   160,
      28,    87,   167,   168,   170,    28,    28,    87,   162,   163,
      28,   165,   166,   167,    28,   194,    22,    27,    33,    35,
     194,   194,   194,    10,   194,    10,    91,    30,    91,    30,
     184,    30,   105,   105,    30,    93,   177,    30,   137,   194,
      30,    27,    27,   198,   198,   217,   194,   194,    37,    37,
      37,    27,    27,   105,   206,    36,    86,   210,   212,   214,
     217,   212,    69,    70,   219,   220,   123,    69,    70,   221,
     222,   222,   206,   197,   197,    37,    87,   199,   200,    27,
      27,   194,   194,    87,    87,   194,   194,    87,    87,    29,
      28,    37,   124,   124,    45,   105,   108,    27,    27,    27,
      27,    27,    27,    45,   137,    35,    28,    45,   137,   137,
      10,    28,    45,   137,    28,    45,    10,   137,    27,    75,
     145,    75,   149,   123,    75,   153,   155,    27,    27,    27,
     148,   149,    27,   154,   155,    26,    10,    37,   206,   206,
      27,    27,    27,    27,   218,   218,    37,    24,    23,    24,
      27,   194,   194,   194,   194,    29,    28,    37,   197,   197,
      27,    27,   198,   198,    27,    27,   198,   198,    91,   101,
      99,   111,   111,   113,   113,   115,   115,    37,    87,   167,
      37,    37,    87,   163,    37,   166,    74,    87,    37,   145,
     149,   194,   194,   194,   141,   143,   148,    22,   154,    22,
     105,    91,    26,   206,   206,   225,   225,   210,   212,    27,
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
yy_symbol_value_print (FILE *yyoutput, int yytype, const YYSTYPE * const yyvaluep)
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
yy_symbol_print (FILE *yyoutput, int yytype, const YYSTYPE * const yyvaluep)
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
# define YYRECOVERING (yystackp->yyerrState != 0)
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
#line 143 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl);
      spec_tree = ((*yyvalp).appl);
    ;}
    break;

  case 3:
#line 148 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl);
      spec_tree = ((*yyvalp).appl);
    ;}
    break;

  case 4:
#line 153 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl);
      spec_tree = ((*yyvalp).appl);
    ;}
    break;

  case 5:
#line 158 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl);
      spec_tree = ((*yyvalp).appl);
    ;}
    break;

  case 6:
#line 163 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl);
      spec_tree = ((*yyvalp).appl);
    ;}
    break;

  case 7:
#line 168 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl);
      spec_tree = ((*yyvalp).appl);
    ;}
    break;

  case 8:
#line 180 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
      gsDebugMsg("parsed sort expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 9:
#line 189 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 10:
#line 193 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeSortArrowProd((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed arrow sort\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 11:
#line 202 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list));
      gsDebugMsg("parsed non-arrow domain\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 12:
#line 211 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed non-arrow domain elements\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 13:
#line 216 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed non-arrow domain elements\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 14:
#line 225 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
      gsDebugMsg("parsed non-arrow domain element\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 15:
#line 234 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 16:
#line 238 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeSortStruct(ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed structured sort\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 17:
#line 247 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed structured sort constructors\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 18:
#line 252 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed structured sort constructors\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 19:
#line 261 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeStructCons((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl), ATmakeList0(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed structured sort constructor\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 20:
#line 266 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeStructCons((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.appl), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.list)), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed structured sort constructor\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 21:
#line 275 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeNil();
      gsDebugMsg("parsed recogniser\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 22:
#line 280 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl);
      gsDebugMsg("parsed recogniser id\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 23:
#line 289 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed structured sort projections\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 24:
#line 294 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed structured sort projections\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 25:
#line 303 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeStructProj(gsMakeNil(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed structured sort projection\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 26:
#line 308 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeStructProj((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed structured sort projection\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 27:
#line 317 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeSortId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed primary sort\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 28:
#line 322 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
      gsDebugMsg("parsed sort constant\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 29:
#line 327 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
      gsDebugMsg("parsed sort constructor\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 30:
#line 332 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 31:
#line 340 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeSortId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 32:
#line 344 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeSortId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 33:
#line 348 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeSortId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 34:
#line 352 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeSortId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 35:
#line 356 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeSortId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 36:
#line 364 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeSortList((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 37:
#line 368 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeSortSet((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 38:
#line 372 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeSortBag((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 39:
#line 383 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
      gsDebugMsg("parsed data expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 40:
#line 392 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 41:
#line 396 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeWhr((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.appl), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed where clause\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 42:
#line 405 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed where clause declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 43:
#line 410 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed where clause declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 44:
#line 419 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeWhrDecl((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed where clause declaration\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 45:
#line 428 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 46:
#line 432 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeLambda((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed lambda abstraction\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 47:
#line 437 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeForall((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 48:
#line 442 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeExists((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 49:
#line 451 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 50:
#line 455 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataApplProd(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed implication\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 51:
#line 465 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 52:
#line 469 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeForall((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 53:
#line 474 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeExists((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 54:
#line 483 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 55:
#line 487 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataApplProd(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed conjunction\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 56:
#line 493 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataApplProd(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed disjunction\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 57:
#line 503 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 58:
#line 507 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeForall((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 59:
#line 512 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeExists((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 60:
#line 521 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 61:
#line 525 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataApplProd(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed equality expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 62:
#line 531 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataApplProd(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed equality expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 63:
#line 541 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 64:
#line 545 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeForall((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 65:
#line 550 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeExists((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 66:
#line 559 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 67:
#line 563 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataApplProd(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed relational expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 68:
#line 569 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataApplProd(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed relational expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 69:
#line 575 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataApplProd(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed relational expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 70:
#line 581 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataApplProd(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed relational expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 71:
#line 587 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataApplProd(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed relational expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 72:
#line 597 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 73:
#line 601 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataApplProd(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed list cons expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 74:
#line 611 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 75:
#line 615 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataApplProd(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed list snoc expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 76:
#line 625 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 77:
#line 629 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataApplProd(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed list concat expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 78:
#line 639 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 79:
#line 643 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataApplProd(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed addition or set union\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 80:
#line 649 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataApplProd(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed subtraction or set difference\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 81:
#line 659 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 82:
#line 663 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataApplProd(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed div expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 83:
#line 669 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataApplProd(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed mod expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 84:
#line 679 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 85:
#line 683 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataApplProd(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed multiplication or set intersection\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 86:
#line 689 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataApplProd(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)),
        ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed list at expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 87:
#line 699 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 88:
#line 703 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataApplProd(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl)), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed prefix data expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 89:
#line 708 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataApplProd(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl)), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed prefix data expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 90:
#line 713 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataApplProd(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl)), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed prefix data expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 91:
#line 722 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 92:
#line 726 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeForall((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 93:
#line 731 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeExists((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 94:
#line 740 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 95:
#line 744 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataApplProd((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.appl), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed postfix data expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 96:
#line 753 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed data expressions\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 97:
#line 758 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed data expressions\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 98:
#line 767 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed primary data expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 99:
#line 772 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 100:
#line 776 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 101:
#line 780 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 102:
#line 784 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 103:
#line 792 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed data constant\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 104:
#line 797 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed data constant\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 105:
#line 802 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeNumber((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl), gsMakeUnknown());
      gsDebugMsg("parsed data constant\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 106:
#line 807 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed data constant\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 107:
#line 812 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed data constant\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 108:
#line 821 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeListEnum(ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.list)), gsMakeUnknown());
      gsDebugMsg("parsed data enumeration\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 109:
#line 826 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeSetEnum(ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.list)), gsMakeUnknown());
      gsDebugMsg("parsed data enumeration\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 110:
#line 831 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeBagEnum(ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.list)), gsMakeUnknown());
      gsDebugMsg("parsed data enumeration\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 111:
#line 840 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed bag enumeration elements\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 112:
#line 845 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed bag enumeration elements\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 113:
#line 854 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeBagEnumElt((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed bag enumeration element\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 114:
#line 863 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeSetBagComp((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed data comprehension\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 115:
#line 872 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataVarId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed data variable declaration\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 116:
#line 884 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeMultAct(ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed multi-action\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 117:
#line 889 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeMultAct(ATmakeList0());
      gsDebugMsg("parsed multi-action\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 118:
#line 898 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed parameterised id's\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 119:
#line 903 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed parameterised id's\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 120:
#line 912 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeParamId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl), ATmakeList0());
      gsDebugMsg("parsed action or process\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 121:
#line 917 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeParamId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.appl), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed action or process\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 122:
#line 929 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
      gsDebugMsg("parsed process expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 123:
#line 938 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 124:
#line 942 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeChoice((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed choice expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 125:
#line 951 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 126:
#line 955 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeSum((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed summation\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 127:
#line 964 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 128:
#line 968 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeMerge((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed merge expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 129:
#line 973 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeLMerge((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed left merge expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 130:
#line 982 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 131:
#line 986 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeSum((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed summation\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 132:
#line 995 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 133:
#line 999 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeBInit((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed bounded initialisation expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 134:
#line 1008 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 135:
#line 1012 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeSum((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed summation\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 136:
#line 1021 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 137:
#line 1025 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeCond((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl), gsMakeDelta());
      gsDebugMsg("parsed conditional expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 138:
#line 1030 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeCond((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed conditional expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 139:
#line 1039 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 140:
#line 1043 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeSum((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed summation\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 141:
#line 1052 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 142:
#line 1056 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeSeq((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed sequential expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 143:
#line 1065 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 144:
#line 1069 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeSeq((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed sequential expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 145:
#line 1078 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 146:
#line 1082 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeSum((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed summation\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 147:
#line 1087 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeCond((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl), gsMakeDelta());
      gsDebugMsg("parsed conditional expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 148:
#line 1092 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeCond((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed conditional expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 149:
#line 1101 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 150:
#line 1105 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeSum((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed summation\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 151:
#line 1114 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 152:
#line 1118 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeAtTime((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed at time expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 153:
#line 1127 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 154:
#line 1131 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeAtTime((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed at time expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 155:
#line 1140 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 156:
#line 1144 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeSync((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed sync expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 157:
#line 1153 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 158:
#line 1157 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeSync((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed sync expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 159:
#line 1166 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 160:
#line 1170 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeSum((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed summation\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 161:
#line 1175 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeCond((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl), gsMakeDelta());
      gsDebugMsg("parsed conditional expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 162:
#line 1180 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeCond((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed conditional expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 163:
#line 1189 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 164:
#line 1193 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeSum((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed summation\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 165:
#line 1202 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 166:
#line 1206 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 167:
#line 1210 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 168:
#line 1214 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 169:
#line 1222 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDelta();
      gsDebugMsg("parsed process constant\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 170:
#line 1227 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeTau();
      gsDebugMsg("parsed process constant\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 171:
#line 1236 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeBlock((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (6))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (6))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed process quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 172:
#line 1241 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeHide((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (6))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (6))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed process quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 173:
#line 1246 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeRename((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (6))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (6))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed process quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 174:
#line 1251 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeComm((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (6))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (6))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed process quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 175:
#line 1256 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeAllow((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (6))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (6))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed process quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 176:
#line 1265 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATmakeList0();
      gsDebugMsg("parsed action name set\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 177:
#line 1270 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.list));
      gsDebugMsg("parsed action name set\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 178:
#line 1279 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed id's\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 179:
#line 1284 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed id's\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 180:
#line 1293 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATmakeList0();
      gsDebugMsg("parsed renaming expression set\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 181:
#line 1298 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.list));
      gsDebugMsg("parsed renaming expression set\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 182:
#line 1307 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed renaming expressions\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 183:
#line 1312 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed renaming expressions\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 184:
#line 1321 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeRenameExpr((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed renaming expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 185:
#line 1330 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATmakeList0();
      gsDebugMsg("parsed communication expression set\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 186:
#line 1335 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.list));
      gsDebugMsg("parsed communication expression set\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 187:
#line 1344 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed communication expressions\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 188:
#line 1349 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed communication expressions\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 189:
#line 1358 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeCommExpr((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl), gsMakeNil());
      gsDebugMsg("parsed communication expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 190:
#line 1363 "../source/mcrl2parser.yy"
    {      
      ((*yyvalp).appl) = gsMakeCommExpr((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), gsMakeNil());
      gsDebugMsg("parsed communication expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 191:
#line 1368 "../source/mcrl2parser.yy"
    {      
      ((*yyvalp).appl) = gsMakeCommExpr((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed communication expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 192:
#line 1377 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeMultActName(ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed multi action name\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 193:
#line 1386 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed id's\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 194:
#line 1391 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed id's\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 195:
#line 1400 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATmakeList0();
      gsDebugMsg("parsed multi action name set\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 196:
#line 1405 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.list));
      gsDebugMsg("parsed multi action name set\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 197:
#line 1414 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed multi action names\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 198:
#line 1419 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed multi action names\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 199:
#line 1431 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsSpecEltsToSpec(ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed specification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 200:
#line 1440 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed specification elements\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 201:
#line 1445 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed specification elements\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 202:
#line 1454 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
      gsDebugMsg("parsed specification element\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 203:
#line 1459 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
      gsDebugMsg("parsed specification element\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 204:
#line 1464 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
      gsDebugMsg("parsed specification element\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 205:
#line 1469 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
      gsDebugMsg("parsed specification element\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 206:
#line 1474 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
      gsDebugMsg("parsed specification element\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 207:
#line 1479 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
      gsDebugMsg("parsed specification element\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 208:
#line 1484 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
      gsDebugMsg("parsed specification element\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 209:
#line 1493 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeSortSpec((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.list));
      gsDebugMsg("parsed sort specification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 210:
#line 1502 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.list);
      gsDebugMsg("parsed sort declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 211:
#line 1507 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATconcat((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.list));
      gsDebugMsg("parsed sort declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 212:
#line 1516 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATmakeList0();
      int n = ATgetLength((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list));
      for (int i = 0; i < n; i++) {
        ((*yyvalp).list) = ATinsert(((*yyvalp).list), (ATerm) gsMakeSortId(ATAelementAt((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list), i)));
      }
      gsDebugMsg("parsed standard sort declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 213:
#line 1525 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATmakeList1((ATerm) gsMakeSortRef((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed reference sort declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 214:
#line 1534 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list);
      gsDebugMsg("parsed domain\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 215:
#line 1539 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATmakeList1((ATerm) gsMakeSortArrowProd((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed domain\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 216:
#line 1548 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeConsSpec((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.list));
      gsDebugMsg("parsed constructor operation specification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 217:
#line 1557 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeMapSpec((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.list));
      gsDebugMsg("parsed operation specification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 218:
#line 1567 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.list);
      gsDebugMsg("parsed operation declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 219:
#line 1572 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATconcat((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.list));
      gsDebugMsg("parsed operation declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 220:
#line 1581 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATmakeList0();
      int n = ATgetLength((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list));
      for (int i = 0; i < n; i++) {
        ((*yyvalp).list) = ATinsert(((*yyvalp).list), (ATerm) gsMakeOpId(ATAelementAt((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), i), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      }
      gsDebugMsg("parsed operation declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 221:
#line 1594 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataEqnSpec((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list));
      gsDebugMsg("parsed equation specification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 222:
#line 1603 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.list));
      gsDebugMsg("parsed equation section\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 223:
#line 1608 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATmakeList0();
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
#line 1623 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed equation declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 225:
#line 1628 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed equation declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 226:
#line 1637 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataEqn(ATmakeList0(), gsMakeNil(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed equation declaration\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 227:
#line 1642 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataEqn(ATmakeList0(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed equation declaration\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 228:
#line 1652 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.list);
      gsDebugMsg("parsed data variable declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 229:
#line 1657 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATconcat((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.list));
      gsDebugMsg("parsed data variable declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 230:
#line 1666 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATmakeList0();
      int n = ATgetLength((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list));
      for (int i = 0; i < n; i++) {
        ((*yyvalp).list) = ATinsert(((*yyvalp).list), (ATerm) gsMakeDataVarId(ATAelementAt((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), i), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      }
      gsDebugMsg("parsed data variable declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 231:
#line 1679 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeActSpec((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.list));
      gsDebugMsg("parsed action specification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 232:
#line 1688 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.list);
      gsDebugMsg("parsed action declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 233:
#line 1693 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATconcat((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.list));
      gsDebugMsg("parsed action declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 234:
#line 1702 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATmakeList0();
      int n = ATgetLength((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list));
      for (int i = 0; i < n; i++) {
        ((*yyvalp).list) = ATinsert(((*yyvalp).list),
          (ATerm) gsMakeActId(ATAelementAt((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list), i), ATmakeList0()));
      }
      gsDebugMsg("parsed action declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 235:
#line 1712 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATmakeList0();
      int n = ATgetLength((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list));
      for (int i = 0; i < n; i++) {
        ((*yyvalp).list) = ATinsert(((*yyvalp).list), (ATerm) gsMakeActId(ATAelementAt((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), i), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.list)));
      }
      gsDebugMsg("parsed action declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 236:
#line 1725 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeProcEqnSpec(ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed process specification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 237:
#line 1734 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed process declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 238:
#line 1739 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed process declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 239:
#line 1748 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeProcEqn(
        ATmakeList0(), gsMakeProcVarId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), ATmakeList0()), ATmakeList0(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed process declaration\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 240:
#line 1754 "../source/mcrl2parser.yy"
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
#line 1770 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list);
      gsDebugMsg("parsed data variable declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 242:
#line 1775 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATconcat((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.list));
      gsDebugMsg("parsed data variable declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 243:
#line 1784 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeInit(ATmakeList0(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed initialisation\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 244:
#line 1796 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
      gsDebugMsg("parsed state formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 245:
#line 1805 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 246:
#line 1809 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeStateForall((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 247:
#line 1814 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeStateExists((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 248:
#line 1819 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeStateNu((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 249:
#line 1824 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeStateMu((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 250:
#line 1833 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATmakeList0();
      gsDebugMsg("parsed fixpoint parameters\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 251:
#line 1838 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.list));
      gsDebugMsg("parsed fixpoint parameters\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 252:
#line 1848 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed data variable declaration and initialisations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 253:
#line 1853 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).list) = ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed data variable declaration and initialisations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 254:
#line 1862 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeDataVarIdInit((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed data variable declaration and initialisation\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 255:
#line 1871 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 256:
#line 1875 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeStateImp((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed implication\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 257:
#line 1884 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 258:
#line 1888 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeStateForall((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 259:
#line 1893 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeStateExists((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 260:
#line 1898 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeStateNu((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 261:
#line 1903 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeStateMu((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 262:
#line 1912 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 263:
#line 1916 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeStateAnd((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed conjunction\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 264:
#line 1921 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeStateOr((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed disjunction\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 265:
#line 1930 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 266:
#line 1934 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeStateForall((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 267:
#line 1939 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeStateExists((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 268:
#line 1944 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeStateNu((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 269:
#line 1949 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeStateMu((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 270:
#line 1958 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 271:
#line 1962 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeStateNot((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed prefix state formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 272:
#line 1967 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeStateMust((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed prefix state formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 273:
#line 1972 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeStateMay((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed prefix state formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 274:
#line 1977 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeStateYaledTimed((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed prefix state formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 275:
#line 1982 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeStateDelayTimed((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed prefix state formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 276:
#line 1991 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 277:
#line 1995 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeStateForall((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 278:
#line 2000 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeStateExists((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 279:
#line 2005 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeStateNu((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 280:
#line 2010 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeStateMu((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 281:
#line 2019 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl);
      gsDebugMsg("parsed postfix state formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 282:
#line 2024 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeStateVar(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl), 0), ATLgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl), 1));
      gsDebugMsg("parsed primary state formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 283:
#line 2029 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeStateTrue();
      gsDebugMsg("parsed primary state formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 284:
#line 2034 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeStateFalse();
      gsDebugMsg("parsed primary state formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 285:
#line 2039 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeStateYaled();
      gsDebugMsg("parsed primary state formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 286:
#line 2044 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeStateDelay();
      gsDebugMsg("parsed primary state formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 287:
#line 2049 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 288:
#line 2057 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
      gsDebugMsg("parsed regular formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 289:
#line 2062 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
      gsDebugMsg("parsed regular formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 290:
#line 2071 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 291:
#line 2075 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeRegAlt((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed alternative composition\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 292:
#line 2084 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 293:
#line 2088 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeRegAlt((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed alternative composition\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 294:
#line 2097 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 295:
#line 2101 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeRegSeq((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed sequential composition\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 296:
#line 2110 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 297:
#line 2114 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeRegSeq((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed sequential composition\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 298:
#line 2123 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 299:
#line 2127 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeRegTransOrNil((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed postfix regular formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 300:
#line 2132 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeRegTrans((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed postfix regular formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 301:
#line 2141 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 302:
#line 2145 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeRegTransOrNil((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed postfix regular formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 303:
#line 2150 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeRegTrans((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed postfix regular formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 304:
#line 2159 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeRegNil();
      gsDebugMsg("parsed primary regular formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 305:
#line 2164 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 306:
#line 2172 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
      gsDebugMsg("parsed regular formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 307:
#line 2177 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeRegNil();
      gsDebugMsg("parsed primary regular formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 308:
#line 2182 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 309:
#line 2190 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
      gsDebugMsg("parsed action formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 310:
#line 2199 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 311:
#line 2203 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeActForall((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 312:
#line 2208 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeActExists((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 313:
#line 2217 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 314:
#line 2221 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeActImp((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed implication\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 315:
#line 2230 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 316:
#line 2234 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeActForall((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 317:
#line 2239 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeActExists((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 318:
#line 2248 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 319:
#line 2252 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeActAnd((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed conjunction\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 320:
#line 2257 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeActOr((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed disjunction\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 321:
#line 2266 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 322:
#line 2270 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeActForall((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 323:
#line 2275 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeActExists((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 324:
#line 2284 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 325:
#line 2288 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeActAt((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed conjunction\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 326:
#line 2297 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 327:
#line 2301 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeActNot((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed prefix action formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 328:
#line 2310 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
    ;}
    break;

  case 329:
#line 2314 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeActForall((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 330:
#line 2319 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeActExists((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 331:
#line 2328 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl);
      gsDebugMsg("parsed primary action formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 332:
#line 2333 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl);
      gsDebugMsg("parsed primary action formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 333:
#line 2338 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeActTrue();
      gsDebugMsg("parsed primary action formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 334:
#line 2343 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = gsMakeActFalse();
      gsDebugMsg("parsed primary action formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 335:
#line 2348 "../source/mcrl2parser.yy"
    {
      ((*yyvalp).appl) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl);
    ;}
    break;


/* Line 930 of glr.c.  */
#line 4480 "../source/mcrl2parser.cpp"
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
	      yyGLRState *yyprevious = yyoption->yystate;
	      YYASSERT (yyprevious->yyresolved);
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

  return yyresult;
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


#line 2354 "../source/mcrl2parser.yy"
 

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
    if (gsIsInit(SpecElt)) {
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
    gsMakeSortSpec(SortDecls),
    gsMakeConsSpec(ConsDecls),
    gsMakeMapSpec(MapDecls),
    gsMakeDataEqnSpec(DataEqnDecls),
    gsMakeActSpec(ActDecls),
    gsMakeProcEqnSpec(ProcEqnDecls),
    Init
  );
  return Result;
}

