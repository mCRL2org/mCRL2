
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
#define yyparse mcrl2yyparse
#define yylex   mcrl2yylex
#define yyerror mcrl2yyerror
#define yylval  mcrl2yylval
#define yychar  mcrl2yychar
#define yydebug mcrl2yydebug
#define yynerrs mcrl2yynerrs
#define yylloc  mcrl2yylloc

/* Copy the first part of user declarations.  */

/* Line 172 of glr.c  */
#line 18 "mcrl2parser.yy"


#include <stdio.h>
#include <string.h>

#include <aterm2.h>
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/data/standard_utility.h"

using namespace mcrl2::core;
using namespace mcrl2::core::detail;

//Global precondition: the ATerm library has been initialised

//external declarations from mcrl2lexer.ll
void mcrl2yyerror(const char *s);
int mcrl2yylex(void);
extern ATerm mcrl2_spec_tree;
extern ATermIndexedSet mcrl2_parser_protect_table;

#ifdef _MSC_VER
#define yyfalse 0
#define yytrue 1
#endif

#define YYMAXDEPTH 640000

//local declarations
ATermAppl gsDataSpecEltsToSpec(ATermList SpecElts);
//Pre: SpecElts contains zero or more occurrences of sort, constructor,
//     operation and data equation specifications.
//Ret: data specification containing one sort, constructor, operation,
//     and data equation specification, in that order.

ATermAppl gsProcSpecEltsToSpec(ATermList SpecElts);
//Pre: SpecElts contains one process initialisation and zero or more
//     occurrences of sort, constructor, operation, data equation, action and
//     process equation specifications.
//Ret: process specification containing one sort, constructor, operation,
//     data equation, action and process equation specification, and one
//     process initialisation, in that order.

ATermAppl gsActionRenameEltsToActionRename(ATermList SpecElts);
//Pre: ActionRenameElts contains zero or more occurrences of
//     sort, constructor, operation, equation, action and action rename
//     rules.
//Ret: specification containing one sort, constructor, operation, equation,
//     action and action rename rules in that order.

ATermAppl gsPBESSpecEltsToSpec(ATermList SpecElts);
//Pre: SpecElts contains one parameterised boolean initialisation and zero or
//     more occurrences of sort, constructor, operation, data equation, action
//     and parameterised boolean equation specifications.
//Ret: BPES specification containing one sort, constructor, operation,
//     data equation, action and parameterised boolean equation specification,
//     and one parameterised boolean initialisation, in that order.

#define safe_assign(lhs, rhs) { ATbool b; lhs = rhs; ATindexedSetPut(mcrl2_parser_protect_table, (ATerm) lhs, &b); }


/* Line 172 of glr.c  */
#line 130 "mcrl2parser.cpp"



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
#line 165 "mcrl2parser.cpp"

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
#define YYFINAL  164
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1635

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  96
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  169
/* YYNRULES -- Number of rules.  */
#define YYNRULES  408
/* YYNRULES -- Number of states.  */
#define YYNSTATES  856
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
      27,    30,    33,    36,    38,    40,    44,    46,    48,    52,
      54,    56,    59,    61,    65,    68,    74,    75,    78,    80,
      84,    86,    90,    92,    94,    96,   100,   102,   104,   106,
     108,   110,   115,   120,   125,   127,   129,   134,   136,   140,
     144,   146,   151,   156,   161,   163,   167,   171,   173,   177,
     179,   184,   189,   191,   195,   199,   201,   206,   211,   213,
     217,   221,   223,   228,   233,   238,   240,   244,   248,   252,
     256,   260,   262,   266,   268,   272,   274,   278,   280,   284,
     288,   290,   294,   298,   302,   304,   308,   312,   314,   317,
     320,   323,   325,   330,   335,   337,   342,   344,   348,   350,
     352,   354,   356,   360,   362,   364,   366,   368,   371,   374,
     378,   382,   386,   388,   392,   396,   402,   406,   408,   410,
     413,   415,   417,   419,   421,   424,   427,   431,   433,   437,
     439,   443,   445,   449,   452,   455,   458,   462,   466,   468,
     471,   476,   479,   483,   487,   493,   496,   500,   502,   504,
     506,   510,   512,   517,   519,   521,   525,   527,   532,   534,
     538,   542,   544,   549,   551,   555,   557,   562,   564,   568,
     574,   576,   581,   583,   587,   589,   593,   595,   600,   604,
     610,   612,   617,   619,   623,   625,   629,   631,   635,   637,
     641,   643,   648,   652,   658,   660,   665,   667,   669,   671,
     673,   677,   679,   681,   685,   690,   697,   704,   711,   718,
     725,   728,   732,   735,   739,   741,   745,   749,   752,   756,
     758,   762,   764,   768,   772,   776,   778,   782,   785,   789,
     791,   795,   797,   799,   801,   804,   806,   808,   810,   812,
     814,   817,   820,   824,   826,   830,   833,   836,   839,   843,
     847,   854,   858,   860,   862,   867,   872,   878,   884,   885,
     889,   891,   895,   901,   903,   907,   909,   914,   919,   925,
     931,   933,   937,   941,   943,   948,   953,   959,   965,   967,
     970,   975,   980,   984,   988,   990,   995,  1000,  1006,  1012,
    1017,  1019,  1021,  1023,  1025,  1027,  1031,  1033,  1035,  1037,
    1041,  1043,  1047,  1049,  1053,  1055,  1059,  1061,  1064,  1067,
    1069,  1072,  1075,  1077,  1081,  1083,  1085,  1089,  1091,  1093,
    1098,  1103,  1105,  1109,  1111,  1116,  1121,  1123,  1127,  1131,
    1133,  1138,  1143,  1145,  1149,  1151,  1154,  1156,  1161,  1166,
    1168,  1173,  1175,  1177,  1181,  1183,  1185,  1188,  1190,  1192,
    1194,  1196,  1199,  1204,  1207,  1211,  1217,  1221,  1223,  1225,
    1227,  1229,  1234,  1239,  1241,  1245,  1247,  1252,  1257,  1259,
    1263,  1267,  1269,  1274,  1279,  1281,  1284,  1286,  1291,  1296,
    1301,  1303,  1305,  1307,  1311,  1313,  1315,  1318,  1320,  1322,
    1324,  1326,  1329,  1332,  1336,  1341,  1349,  1351,  1353
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const short int yyrhs[] =
{
      97,     0,    -1,     3,    94,    -1,     4,    98,    -1,     5,
     112,    -1,     6,   143,    -1,     7,   160,    -1,     8,   163,
      -1,     9,   198,    -1,    10,   209,    -1,    11,   240,    -1,
      12,   257,    -1,    13,   159,    -1,    99,    -1,   103,    -1,
     100,    15,    99,    -1,   101,    -1,   102,    -1,   101,    40,
     102,    -1,   103,    -1,   109,    -1,    60,   104,    -1,   105,
      -1,   104,    41,   105,    -1,    94,   106,    -1,    94,    42,
     107,    43,   106,    -1,    -1,    37,    94,    -1,   108,    -1,
     107,    34,   108,    -1,    98,    -1,    94,    35,    98,    -1,
      94,    -1,   110,    -1,   111,    -1,    42,    98,    43,    -1,
      61,    -1,    62,    -1,    63,    -1,    64,    -1,    65,    -1,
      66,    42,    98,    43,    -1,    67,    42,    98,    43,    -1,
      68,    42,    98,    43,    -1,   113,    -1,   116,    -1,   113,
      78,   114,    79,    -1,   115,    -1,   114,    34,   115,    -1,
      94,    32,   112,    -1,   119,    -1,    75,   117,    33,   116,
      -1,    76,   117,    33,   116,    -1,    77,   117,    33,   116,
      -1,   118,    -1,   117,    34,   118,    -1,   149,    35,    98,
      -1,   121,    -1,   121,    25,   120,    -1,   119,    -1,    76,
     117,    33,   120,    -1,    77,   117,    33,   120,    -1,   123,
      -1,   123,    23,   122,    -1,   123,    24,   122,    -1,   121,
      -1,    76,   117,    33,   122,    -1,    77,   117,    33,   122,
      -1,   125,    -1,   125,    21,   124,    -1,   125,    22,   124,
      -1,   123,    -1,    75,   117,    33,   124,    -1,    76,   117,
      33,   124,    -1,    77,   117,    33,   124,    -1,   126,    -1,
     126,    17,   126,    -1,   126,    16,   126,    -1,   126,    47,
     126,    -1,   126,    46,   126,    -1,   126,    74,   126,    -1,
     127,    -1,   129,    18,   126,    -1,   128,    -1,   127,    19,
     129,    -1,   129,    -1,   128,    20,   129,    -1,   130,    -1,
     129,    30,   130,    -1,   129,    31,   130,    -1,   131,    -1,
     130,    72,   131,    -1,   130,    73,   131,    -1,   130,    28,
     131,    -1,   132,    -1,   131,    29,   132,    -1,   131,    33,
     132,    -1,   134,    -1,    38,   133,    -1,    31,   132,    -1,
      40,   132,    -1,   132,    -1,    76,   117,    33,   133,    -1,
      77,   117,    33,   133,    -1,   136,    -1,   134,    42,   135,
      43,    -1,   112,    -1,   135,    34,   112,    -1,    94,    -1,
     137,    -1,   138,    -1,   141,    -1,    42,   112,    43,    -1,
      69,    -1,    70,    -1,    71,    -1,    95,    -1,    44,    45,
      -1,    48,    49,    -1,    44,   135,    45,    -1,    48,   135,
      49,    -1,    48,   139,    49,    -1,   140,    -1,   139,    34,
     140,    -1,   112,    35,   112,    -1,    48,   142,    41,   112,
      49,    -1,    94,    35,    98,    -1,   144,    -1,   145,    -1,
     144,   145,    -1,   146,    -1,   151,    -1,   152,    -1,   155,
      -1,    50,   147,    -1,   148,    36,    -1,   147,   148,    36,
      -1,   149,    -1,    94,    32,    98,    -1,    94,    -1,   149,
      34,    94,    -1,   100,    -1,   100,    15,    98,    -1,    51,
     153,    -1,    52,   153,    -1,   154,    36,    -1,   153,   154,
      36,    -1,   149,    35,    98,    -1,   156,    -1,    54,   157,
      -1,    53,   159,    54,   157,    -1,   158,    36,    -1,   157,
     158,    36,    -1,   112,    32,   112,    -1,   112,    15,   112,
      32,   112,    -1,   118,    36,    -1,   159,   118,    36,    -1,
     161,    -1,    81,    -1,   162,    -1,   161,    41,   162,    -1,
      94,    -1,    94,    42,   135,    43,    -1,   164,    -1,   165,
      -1,   165,    30,   164,    -1,   166,    -1,    82,   117,    33,
     165,    -1,   168,    -1,   168,    24,   167,    -1,   168,    14,
     167,    -1,   166,    -1,    82,   117,    33,   167,    -1,   170,
      -1,   168,    26,   169,    -1,   170,    -1,    82,   117,    33,
     169,    -1,   172,    -1,   132,    15,   171,    -1,   132,    15,
     175,    27,   171,    -1,   170,    -1,    82,   117,    33,   171,
      -1,   176,    -1,   176,    33,   174,    -1,   177,    -1,   176,
      33,   175,    -1,   172,    -1,    82,   117,    33,   174,    -1,
     132,    15,   174,    -1,   132,    15,   175,    27,   174,    -1,
     173,    -1,    82,   117,    33,   175,    -1,   178,    -1,   176,
      39,   132,    -1,   179,    -1,   177,    39,   132,    -1,   182,
      -1,   182,    41,   180,    -1,   182,    -1,   182,    41,   181,
      -1,   178,    -1,    82,   117,    33,   180,    -1,   132,    15,
     180,    -1,   132,    15,   181,    27,   180,    -1,   179,    -1,
      82,   117,    33,   181,    -1,   183,    -1,   162,    -1,   184,
      -1,   185,    -1,    42,   163,    43,    -1,    80,    -1,    81,
      -1,    94,    42,    43,    -1,    94,    42,   114,    43,    -1,
      83,    42,   186,    34,   163,    43,    -1,    85,    42,   186,
      34,   163,    43,    -1,    86,    42,   187,    34,   163,    43,
      -1,    87,    42,   190,    34,   163,    43,    -1,    84,    42,
     195,    34,   163,    43,    -1,    48,    49,    -1,    48,   149,
      49,    -1,    48,    49,    -1,    48,   188,    49,    -1,   189,
      -1,   188,    34,   189,    -1,    94,    15,    94,    -1,    48,
      49,    -1,    48,   191,    49,    -1,   192,    -1,   191,    34,
     192,    -1,   193,    -1,   193,    15,    81,    -1,   193,    15,
      94,    -1,    94,    41,   194,    -1,    94,    -1,   194,    41,
      94,    -1,    48,    49,    -1,    48,   196,    49,    -1,   197,
      -1,   196,    34,   197,    -1,   194,    -1,   199,    -1,   200,
      -1,   199,   200,    -1,   145,    -1,   201,    -1,   204,    -1,
     205,    -1,   208,    -1,    55,   202,    -1,   203,    36,    -1,
     202,   203,    36,    -1,   149,    -1,   149,    35,   150,    -1,
      56,   159,    -1,    57,   206,    -1,   207,    36,    -1,   206,
     207,    36,    -1,    94,    32,   163,    -1,    94,    42,   117,
      43,    32,   163,    -1,    59,   163,    36,    -1,   210,    -1,
     214,    -1,    76,   117,    33,   210,    -1,    77,   117,    33,
     210,    -1,    90,    94,   211,    33,   210,    -1,    89,    94,
     211,    33,   210,    -1,    -1,    42,   212,    43,    -1,   213,
      -1,   212,    34,   213,    -1,    94,    35,    98,    32,   112,
      -1,   216,    -1,   216,    25,   215,    -1,   214,    -1,    76,
     117,    33,   215,    -1,    77,   117,    33,   215,    -1,    90,
      94,   211,    33,   215,    -1,    89,    94,   211,    33,   215,
      -1,   218,    -1,   218,    23,   217,    -1,   218,    24,   217,
      -1,   216,    -1,    76,   117,    33,   217,    -1,    77,   117,
      33,   217,    -1,    90,    94,   211,    33,   217,    -1,    89,
      94,   211,    33,   217,    -1,   220,    -1,    38,   219,    -1,
      44,   221,    45,   219,    -1,    46,   221,    47,   219,    -1,
      92,    39,   132,    -1,    91,    39,   132,    -1,   218,    -1,
      76,   117,    33,   219,    -1,    77,   117,    33,   219,    -1,
      90,    94,   211,    33,   219,    -1,    89,    94,   211,    33,
     219,    -1,    88,    42,   112,    43,    -1,   162,    -1,    69,
      -1,    70,    -1,    92,    -1,    91,    -1,    42,   209,    43,
      -1,   230,    -1,   222,    -1,   224,    -1,   225,    30,   223,
      -1,   225,    -1,   225,    30,   223,    -1,   226,    -1,   227,
      33,   225,    -1,   227,    -1,   227,    33,   225,    -1,   228,
      -1,   227,    29,    -1,   227,    30,    -1,   229,    -1,   227,
      29,    -1,   227,    30,    -1,    93,    -1,    42,   222,    43,
      -1,   230,    -1,    93,    -1,    42,   222,    43,    -1,   231,
      -1,   232,    -1,    76,   117,    33,   231,    -1,    77,   117,
      33,   231,    -1,   234,    -1,   234,    25,   233,    -1,   232,
      -1,    76,   117,    33,   233,    -1,    77,   117,    33,   233,
      -1,   236,    -1,   237,    23,   235,    -1,   237,    24,   235,
      -1,   234,    -1,    76,   117,    33,   235,    -1,    77,   117,
      33,   235,    -1,   237,    -1,   236,    39,   132,    -1,   239,
      -1,    38,   238,    -1,   237,    -1,    76,   117,    33,   238,
      -1,    77,   117,    33,   238,    -1,   160,    -1,    88,    42,
     112,    43,    -1,    69,    -1,    70,    -1,    42,   230,    43,
      -1,   241,    -1,   242,    -1,   241,   242,    -1,   145,    -1,
     201,    -1,   243,    -1,   244,    -1,    86,   245,    -1,    53,
     159,    86,   245,    -1,   246,    36,    -1,   245,   246,    36,
      -1,   112,    15,   162,    25,   247,    -1,   162,    25,   247,
      -1,   162,    -1,   183,    -1,   249,    -1,   250,    -1,    76,
     117,    33,   249,    -1,    77,   117,    33,   249,    -1,   252,
      -1,   252,    25,   251,    -1,   250,    -1,    76,   117,    33,
     251,    -1,    77,   117,    33,   251,    -1,   254,    -1,   254,
      23,   253,    -1,   254,    24,   253,    -1,   252,    -1,    76,
     117,    33,   253,    -1,    77,   117,    33,   253,    -1,   256,
      -1,    38,   255,    -1,   254,    -1,    76,   117,    33,   255,
      -1,    77,   117,    33,   255,    -1,    88,    42,   112,    43,
      -1,   162,    -1,    69,    -1,    70,    -1,    42,   248,    43,
      -1,   258,    -1,   259,    -1,   258,   259,    -1,   145,    -1,
     204,    -1,   260,    -1,   264,    -1,    58,   261,    -1,   262,
      36,    -1,   261,   262,    36,    -1,   263,    94,    32,   248,
      -1,   263,    94,    42,   117,    43,    32,   248,    -1,    89,
      -1,    90,    -1,    59,   162,    36,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,   383,   383,   388,   393,   398,   403,   408,   413,   418,
     423,   428,   433,   445,   454,   458,   467,   476,   481,   490,
     499,   503,   512,   517,   526,   531,   541,   545,   554,   559,
     568,   573,   582,   587,   592,   597,   605,   609,   613,   617,
     621,   629,   633,   637,   648,   657,   661,   670,   675,   684,
     693,   697,   702,   707,   717,   722,   731,   744,   748,   758,
     762,   767,   776,   780,   786,   796,   800,   805,   814,   818,
     824,   834,   838,   843,   848,   857,   861,   867,   873,   879,
     885,   895,   899,   909,   913,   923,   927,   937,   941,   947,
     957,   961,   967,   973,   983,   987,   993,  1003,  1007,  1012,
    1017,  1026,  1030,  1035,  1044,  1048,  1063,  1068,  1077,  1082,
    1086,  1090,  1094,  1102,  1107,  1112,  1117,  1122,  1127,  1136,
    1141,  1146,  1155,  1160,  1169,  1178,  1187,  1199,  1208,  1213,
    1222,  1227,  1232,  1237,  1246,  1255,  1260,  1269,  1278,  1287,
    1292,  1301,  1306,  1315,  1324,  1334,  1339,  1348,  1361,  1370,
    1375,  1391,  1396,  1405,  1410,  1420,  1425,  1437,  1442,  1451,
    1456,  1465,  1470,  1482,  1491,  1495,  1504,  1508,  1517,  1521,
    1526,  1535,  1539,  1548,  1552,  1561,  1565,  1574,  1578,  1583,
    1592,  1596,  1605,  1609,  1618,  1622,  1631,  1635,  1640,  1645,
    1654,  1658,  1667,  1671,  1680,  1684,  1693,  1697,  1706,  1710,
    1719,  1723,  1728,  1733,  1742,  1746,  1755,  1759,  1763,  1769,
    1773,  1781,  1786,  1795,  1800,  1809,  1814,  1819,  1824,  1829,
    1838,  1843,  1852,  1857,  1866,  1871,  1880,  1889,  1894,  1903,
    1908,  1917,  1922,  1927,  1936,  1945,  1950,  1959,  1964,  1973,
    1978,  1987,  1999,  2008,  2013,  2022,  2027,  2032,  2037,  2042,
    2051,  2060,  2065,  2074,  2084,  2097,  2106,  2115,  2120,  2129,
    2135,  2150,  2162,  2171,  2175,  2180,  2185,  2190,  2200,  2204,
    2214,  2219,  2228,  2237,  2241,  2250,  2254,  2259,  2264,  2269,
    2278,  2282,  2287,  2296,  2300,  2305,  2310,  2315,  2324,  2328,
    2333,  2338,  2343,  2348,  2357,  2361,  2366,  2371,  2376,  2385,
    2390,  2395,  2400,  2405,  2410,  2415,  2423,  2428,  2437,  2441,
    2450,  2454,  2463,  2467,  2476,  2480,  2489,  2493,  2498,  2507,
    2511,  2516,  2525,  2530,  2538,  2543,  2548,  2556,  2565,  2569,
    2574,  2583,  2587,  2596,  2600,  2605,  2614,  2618,  2623,  2632,
    2636,  2641,  2650,  2654,  2663,  2667,  2676,  2680,  2685,  2694,
    2699,  2704,  2709,  2714,  2725,  2734,  2739,  2748,  2753,  2758,
    2767,  2776,  2781,  2799,  2804,  2813,  2818,  2827,  2832,  2844,
    2853,  2857,  2862,  2871,  2875,  2884,  2888,  2893,  2902,  2906,
    2911,  2920,  2924,  2929,  2938,  2942,  2951,  2955,  2960,  2969,
    2974,  2979,  2984,  2989,  3001,  3010,  3015,  3024,  3029,  3034,
    3039,  3048,  3057,  3062,  3071,  3077,  3087,  3092,  3101
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
  "NUMBER", "$accept", "start", "sort_expr", "sort_expr_arrow",
  "domain_no_arrow", "domain_no_arrow_elts_hs", "domain_no_arrow_elt",
  "sort_expr_struct", "struct_constructors_bs", "struct_constructor",
  "recogniser", "struct_projections_cs", "struct_projection",
  "sort_expr_primary", "sort_constant", "sort_constructor", "data_expr",
  "data_expr_whr", "id_inits_cs", "id_init", "data_expr_quant",
  "data_vars_decls_cs", "data_vars_decl", "data_expr_imp",
  "data_expr_imp_rhs", "data_expr_and", "data_expr_and_rhs",
  "data_expr_eq", "data_expr_eq_rhs", "data_expr_rel", "data_expr_cons",
  "data_expr_snoc", "data_expr_concat", "data_expr_add", "data_expr_div",
  "data_expr_mult", "data_expr_prefix", "data_expr_quant_prefix",
  "data_expr_postfix", "data_exprs_cs", "data_expr_primary",
  "data_constant", "data_enumeration", "bag_enum_elts_cs", "bag_enum_elt",
  "data_comprehension", "data_var_decl", "data_spec", "data_spec_elts",
  "data_spec_elt", "sort_spec", "sorts_decls_scs", "sorts_decl", "ids_cs",
  "domain", "cons_spec", "map_spec", "ops_decls_scs", "ops_decl",
  "data_eqn_spec", "data_eqn_sect", "data_eqn_decls_scs", "data_eqn_decl",
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
      97,    97,    97,    98,    99,    99,   100,   101,   101,   102,
     103,   103,   104,   104,   105,   105,   106,   106,   107,   107,
     108,   108,   109,   109,   109,   109,   110,   110,   110,   110,
     110,   111,   111,   111,   112,   113,   113,   114,   114,   115,
     116,   116,   116,   116,   117,   117,   118,   119,   119,   120,
     120,   120,   121,   121,   121,   122,   122,   122,   123,   123,
     123,   124,   124,   124,   124,   125,   125,   125,   125,   125,
     125,   126,   126,   127,   127,   128,   128,   129,   129,   129,
     130,   130,   130,   130,   131,   131,   131,   132,   132,   132,
     132,   133,   133,   133,   134,   134,   135,   135,   136,   136,
     136,   136,   136,   137,   137,   137,   137,   137,   137,   138,
     138,   138,   139,   139,   140,   141,   142,   143,   144,   144,
     145,   145,   145,   145,   146,   147,   147,   148,   148,   149,
     149,   150,   150,   151,   152,   153,   153,   154,   155,   156,
     156,   157,   157,   158,   158,   159,   159,   160,   160,   161,
     161,   162,   162,   163,   164,   164,   165,   165,   166,   166,
     166,   167,   167,   168,   168,   169,   169,   170,   170,   170,
     171,   171,   172,   172,   173,   173,   174,   174,   174,   174,
     175,   175,   176,   176,   177,   177,   178,   178,   179,   179,
     180,   180,   180,   180,   181,   181,   182,   182,   182,   182,
     182,   183,   183,   184,   184,   185,   185,   185,   185,   185,
     186,   186,   187,   187,   188,   188,   189,   190,   190,   191,
     191,   192,   192,   192,   193,   194,   194,   195,   195,   196,
     196,   197,   198,   199,   199,   200,   200,   200,   200,   200,
     201,   202,   202,   203,   203,   204,   205,   206,   206,   207,
     207,   208,   209,   210,   210,   210,   210,   210,   211,   211,
     212,   212,   213,   214,   214,   215,   215,   215,   215,   215,
     216,   216,   216,   217,   217,   217,   217,   217,   218,   218,
     218,   218,   218,   218,   219,   219,   219,   219,   219,   220,
     220,   220,   220,   220,   220,   220,   221,   221,   222,   222,
     223,   223,   224,   224,   225,   225,   226,   226,   226,   227,
     227,   227,   228,   228,   229,   229,   229,   230,   231,   231,
     231,   232,   232,   233,   233,   233,   234,   234,   234,   235,
     235,   235,   236,   236,   237,   237,   238,   238,   238,   239,
     239,   239,   239,   239,   240,   241,   241,   242,   242,   242,
     243,   244,   244,   245,   245,   246,   246,   247,   247,   248,
     249,   249,   249,   250,   250,   251,   251,   251,   252,   252,
     252,   253,   253,   253,   254,   254,   255,   255,   255,   256,
     256,   256,   256,   256,   257,   258,   258,   259,   259,   259,
     259,   260,   261,   261,   262,   262,   263,   263,   264
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     1,     1,     3,     1,     1,     3,     1,
       1,     2,     1,     3,     2,     5,     0,     2,     1,     3,
       1,     3,     1,     1,     1,     3,     1,     1,     1,     1,
       1,     4,     4,     4,     1,     1,     4,     1,     3,     3,
       1,     4,     4,     4,     1,     3,     3,     1,     3,     1,
       4,     4,     1,     3,     3,     1,     4,     4,     1,     3,
       3,     1,     4,     4,     4,     1,     3,     3,     3,     3,
       3,     1,     3,     1,     3,     1,     3,     1,     3,     3,
       1,     3,     3,     3,     1,     3,     3,     1,     2,     2,
       2,     1,     4,     4,     1,     4,     1,     3,     1,     1,
       1,     1,     3,     1,     1,     1,     1,     2,     2,     3,
       3,     3,     1,     3,     3,     5,     3,     1,     1,     2,
       1,     1,     1,     1,     2,     2,     3,     1,     3,     1,
       3,     1,     3,     2,     2,     2,     3,     3,     1,     2,
       4,     2,     3,     3,     5,     2,     3,     1,     1,     1,
       3,     1,     4,     1,     1,     3,     1,     4,     1,     3,
       3,     1,     4,     1,     3,     1,     4,     1,     3,     5,
       1,     4,     1,     3,     1,     3,     1,     4,     3,     5,
       1,     4,     1,     3,     1,     3,     1,     3,     1,     3,
       1,     4,     3,     5,     1,     4,     1,     1,     1,     1,
       3,     1,     1,     3,     4,     6,     6,     6,     6,     6,
       2,     3,     2,     3,     1,     3,     3,     2,     3,     1,
       3,     1,     3,     3,     3,     1,     3,     2,     3,     1,
       3,     1,     1,     1,     2,     1,     1,     1,     1,     1,
       2,     2,     3,     1,     3,     2,     2,     2,     3,     3,
       6,     3,     1,     1,     4,     4,     5,     5,     0,     3,
       1,     3,     5,     1,     3,     1,     4,     4,     5,     5,
       1,     3,     3,     1,     4,     4,     5,     5,     1,     2,
       4,     4,     3,     3,     1,     4,     4,     5,     5,     4,
       1,     1,     1,     1,     1,     3,     1,     1,     1,     3,
       1,     3,     1,     3,     1,     3,     1,     2,     2,     1,
       2,     2,     1,     3,     1,     1,     3,     1,     1,     4,
       4,     1,     3,     1,     4,     4,     1,     3,     3,     1,
       4,     4,     1,     3,     1,     2,     1,     4,     4,     1,
       4,     1,     1,     3,     1,     1,     2,     1,     1,     1,
       1,     2,     4,     2,     3,     5,     3,     1,     1,     1,
       1,     4,     4,     1,     3,     1,     4,     4,     1,     3,
       3,     1,     4,     4,     1,     2,     1,     4,     4,     4,
       1,     1,     1,     3,     1,     1,     2,     1,     1,     1,
       1,     2,     2,     3,     4,     7,     1,     1,     3
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
       0,     0,     0,     0,     0,     0,     0,     0,     0
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
       0,     0,     0,     0,     0,     0,     0,     0,     0
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error.  */
static const unsigned short int yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     2,     0,     0,    36,    37,    38,    39,
      40,     0,     0,     0,    32,     3,    13,     0,    16,    17,
      14,    20,    33,    34,     0,     0,     0,     0,     0,     0,
     113,   114,   115,     0,     0,     0,   108,   116,     4,    44,
      45,    50,    57,    62,    68,    75,    81,    83,    85,    87,
      90,    94,    97,   104,   109,   110,   111,     0,     0,     0,
       0,     0,     5,   127,   128,   130,   131,   132,   133,   148,
     158,   161,     6,   157,   159,     0,   211,   212,     0,     0,
       0,     0,     0,     0,   108,     0,   207,     7,   163,   164,
     166,   168,   173,   177,   182,   192,   196,   206,   208,   209,
       0,     0,     0,     0,   245,     8,   242,   243,   246,   247,
     248,   249,     0,     0,     0,     0,   301,   302,     0,     0,
       0,     0,     0,   304,   303,   300,     9,   262,   263,   273,
     280,   288,     0,     0,   357,   358,    10,   354,   355,   359,
     360,     0,     0,   397,   398,    11,   394,   395,   399,   400,
     139,     0,     0,    12,     1,     0,    26,    21,    22,     0,
       0,     0,     0,     0,    99,     0,     0,   101,    98,   100,
       0,   117,   106,     0,   118,   108,   106,     0,     0,   122,
       0,     0,    54,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   139,   134,     0,
     137,     0,   143,     0,   144,     0,     0,   149,     0,   129,
       0,     0,    94,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   253,
     250,     0,   255,     0,   256,     0,     0,   244,     0,     0,
       0,     0,   294,   289,     0,     0,     0,   351,   352,     0,
       0,     0,   322,   349,     0,   307,   308,     0,   312,     0,
     316,   319,   324,   327,   328,   331,   336,   342,   344,     0,
       0,     0,     0,   268,   268,     0,     0,     0,     0,     0,
       0,   108,     0,     0,   361,     0,   356,   406,   407,   401,
       0,     0,     0,   396,   155,     0,     0,     0,    35,     0,
       0,    24,     0,     0,     0,     0,    15,    18,    19,     0,
       0,   112,     0,   119,     0,     0,   120,     0,   121,     0,
       0,     0,     0,     0,     0,     0,    47,     0,     0,    59,
      58,     0,     0,    65,    63,    64,     0,     0,     0,    71,
      69,    70,    77,    76,    79,    78,    80,    84,    86,    82,
      88,    89,    93,    91,    92,    95,    96,     0,     0,     0,
     135,     0,     0,   145,     0,     0,     0,     0,   151,     0,
     160,   210,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   213,   108,     0,     0,   180,   178,   190,     0,
     182,   184,   194,   196,   165,     0,   171,   170,   169,     0,
     174,   175,     0,     0,   186,   183,   193,     0,     0,   200,
     197,     0,     0,   251,     0,     0,     0,   257,   261,     0,
       0,   268,   268,   305,     0,     0,     0,   346,   345,     0,
     324,     0,     0,     0,     0,     0,   317,   318,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     293,   292,     0,     0,     0,     0,   275,   274,     0,     0,
       0,     0,   283,   281,   282,     0,     0,     0,     0,   363,
       0,   402,     0,   408,   140,    56,   156,    27,    32,    30,
       0,    28,    23,    41,    42,    43,     0,     0,   107,   126,
     124,     0,   123,     0,    51,    55,    52,    53,     0,     0,
      46,     0,     0,     0,     0,     0,     0,     0,   105,   138,
     136,   147,   146,   150,     0,   153,   152,   162,   167,   220,
       0,     0,   237,   235,   241,     0,   239,     0,     0,   222,
       0,     0,   224,     0,   227,     0,     0,   229,   231,     0,
     214,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   141,   254,   252,   259,     0,   258,     0,     0,
       0,     0,     0,     0,     0,   323,   353,     0,     0,     0,
     290,     0,   325,   309,   310,   314,   324,   313,     0,     0,
     333,   332,   343,     0,     0,   339,   337,   338,   291,   264,
     265,   299,     0,     0,   270,     0,     0,     0,     0,   268,
     268,     0,     0,   268,   268,   362,     0,   367,   368,   366,
     364,   403,     0,     0,     0,     0,    26,   102,   103,   125,
      49,    48,     0,     0,     0,     0,     0,     0,     0,     0,
     221,     0,     0,     0,   238,     0,     0,     0,     0,   223,
       0,     0,     0,   228,     0,     0,     0,     0,   179,     0,
     185,   195,     0,   204,   199,     0,     0,     0,   188,     0,
       0,   202,     0,     0,     0,   295,   296,     0,     0,     0,
       0,   329,   330,   350,     0,     0,   320,   321,     0,     0,
       0,     0,     0,     0,     0,   269,   267,   266,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   391,
     392,     0,     0,     0,   390,   404,   369,   370,   373,   378,
     384,     0,    31,    29,    25,    60,    61,    66,    67,    72,
      73,    74,   154,   215,   236,   240,   219,   216,   226,   225,
     217,   234,   230,   232,   233,   218,   181,   191,     0,     0,
       0,   172,   176,   187,     0,   201,     0,   142,     0,   298,
     297,   347,   348,   326,   311,   315,     0,     0,     0,     0,
       0,   271,   276,   277,     0,     0,   284,   285,     0,     0,
     365,     0,     0,   386,   385,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   189,   203,   260,   334,
     335,   340,   341,     0,   279,   278,   287,   286,     0,     0,
     393,     0,     0,     0,     0,     0,   375,   374,     0,     0,
     381,   379,   380,     0,   205,   272,     0,     0,   371,   372,
     389,     0,     0,     0,     0,   405,   387,   388,     0,     0,
       0,     0,   376,   377,   382,   383
};

/* YYPDEFGOTO[NTERM-NUM].  */
static const short int yydefgoto[] =
{
      -1,    12,   499,    26,    27,    28,    29,    30,   167,   168,
     321,   500,   501,    31,    32,    33,   182,    49,   345,   346,
      50,   191,   192,    51,   350,    52,   354,    53,   360,    54,
      55,    56,    57,    58,    59,    60,    61,   178,    62,   389,
      63,    64,    65,   188,   189,    66,   190,    72,    73,   114,
      75,   218,   219,   162,   573,    76,    77,   222,   223,    78,
      79,   227,   228,   163,   273,    83,    96,    97,    98,    99,
     100,   417,   101,   420,   102,   756,   103,   408,   425,   757,
     104,   411,   105,   412,   430,   674,   106,   107,   108,   109,
     394,   399,   551,   552,   401,   556,   557,   558,   544,   396,
     545,   546,   115,   116,   117,   118,   250,   251,   119,   120,
     254,   255,   121,   136,   137,   468,   613,   614,   138,   477,
     139,   483,   140,   263,   141,   274,   275,   593,   276,   277,
     278,   279,   280,   281,   596,   283,   284,   601,   285,   606,
     286,   287,   448,   288,   146,   147,   148,   149,   150,   304,
     305,   629,   725,   726,   727,   827,   728,   831,   729,   794,
     730,   155,   156,   157,   158,   309,   310,   311,   159
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -727
static const short int yypact[] =
{
    1463,   -17,   725,  1274,   151,    95,   905,   190,  1414,   111,
     469,    96,   133,  -727,   725,   127,  -727,  -727,  -727,  -727,
    -727,   104,   210,   235,  -727,  -727,  -727,   270,   250,  -727,
      57,  -727,  -727,  -727,    60,  1347,    60,  1274,   600,  1271,
    -727,  -727,  -727,    96,    96,    96,  -727,  -727,  -727,   246,
    -727,  -727,   321,   155,   244,    68,   336,   352,    76,    39,
     324,  -727,   335,  -727,  -727,  -727,  -727,   289,    96,    96,
      96,  1274,  -727,   151,  -727,  -727,  -727,  -727,  -727,  -727,
    -727,   357,  -727,   368,  -727,   828,  -727,  -727,    96,   369,
     372,   383,   394,   413,   625,   449,  -727,  -727,  -727,   438,
    -727,   198,  -727,  -727,   194,  -727,   448,  -727,  -727,  -727,
      96,    96,   398,   905,  -727,  -727,   190,  -727,  -727,  -727,
    -727,  -727,  1454,  1414,   181,   181,  -727,  -727,    96,    96,
     460,   418,   422,   479,   485,  -727,  -727,  -727,  -727,   515,
     272,  -727,    96,  1286,  -727,  -727,  -727,   111,  -727,  -727,
    -727,   220,   461,  -727,  -727,  -727,   469,  -727,  -727,  -727,
    -727,   521,   328,    96,  -727,   518,   211,   531,  -727,   725,
     725,   725,   725,   725,  -727,    96,    96,  -727,  -727,  -727,
     534,  -727,  -727,   139,  -727,   552,   555,    79,   101,  -727,
     540,   353,  -727,   390,   407,   498,  1359,  1417,  1417,  1344,
    1344,    60,    60,    60,    60,    60,    60,    60,    60,    60,
      60,    60,    60,    60,    60,    60,  1274,   577,   289,   578,
     585,   411,    96,   596,    96,    16,    71,  1274,   601,  -727,
    1274,   461,   449,   583,   414,   593,   595,   593,   598,   602,
     889,   963,   905,   982,   982,  1040,  1059,    60,  1117,   450,
      96,   618,    96,   186,   398,   638,   666,  -727,    96,    96,
     597,   612,  -727,  -727,   622,   297,   181,  -727,  -727,    96,
      96,   672,   292,  -727,   673,  -727,  -727,   690,  -727,   346,
    -727,  -727,   286,  -727,  -727,   699,   687,   487,  -727,   685,
     481,   496,  1274,   698,   698,    60,    60,  1483,  1512,  1512,
      62,    84,   729,   731,  1286,   734,  -727,  -727,  -727,   220,
     761,   674,   773,  -727,  -727,   717,   725,   782,  -727,   735,
     825,  -727,   127,   718,   777,   787,  -727,  -727,  -727,   499,
     501,  -727,  1274,  -727,   725,  1274,  -727,  1274,  -727,  1274,
    1274,    96,  1274,  1274,   801,    42,  -727,    96,    96,  -727,
    -727,    96,    96,  -727,  -727,  -727,    96,    96,    96,  -727,
    -727,  -727,  -727,  -727,  -727,  -727,  -727,   506,   506,  -727,
      39,    39,   324,   324,   324,  -727,  -727,   191,   725,   798,
    -727,   725,   799,  -727,  1274,  1274,  1274,   800,  -727,   221,
    -727,  -727,   905,    29,   803,    40,   804,   805,    43,   807,
      47,   808,  -727,   801,   233,    96,  -727,  -727,  -727,   820,
     199,   809,  -727,   159,  -727,    96,  -727,  -727,  -727,    96,
    -727,  -727,    96,   837,  -727,  -727,  -727,    96,   838,  -727,
    -727,   725,   819,  -727,   905,    96,   821,  -727,  -727,   505,
     509,   698,   698,  -727,   475,    96,    96,  -727,  -727,   813,
     815,   513,   516,  1274,  1454,  1283,   351,   375,  1283,   681,
      60,  1356,  1356,  1454,  1414,  1414,   817,   767,   829,   830,
    -727,  -727,    96,    96,   775,   779,  -727,  -727,    96,    96,
     780,   781,  -727,  -727,  -727,  1286,   461,     7,   835,  -727,
     841,  -727,   188,  -727,  -727,  -727,  -727,  -727,   843,  -727,
     236,  -727,  -727,  -727,  -727,  -727,  1347,  1347,  -727,  -727,
    -727,   555,  -727,   831,  -727,  -727,  -727,  -727,  1274,   498,
    -727,   520,   526,   546,   551,   560,   562,   565,  -727,  -727,
    -727,  -727,  -727,  1274,   847,  -727,  -727,  -727,  -727,  -727,
     118,   905,  -727,  -727,   840,   123,  -727,   905,   905,  -727,
     867,   126,  -727,   905,  -727,   842,   136,  -727,   869,   905,
    -727,   569,  1136,  1194,    60,  1213,   571,   573,   579,  1194,
     584,  1213,   879,  -727,  -727,  -727,   248,  -727,  1454,  1454,
     862,   863,   815,   590,   626,   377,  -727,   475,   475,   857,
    -727,   181,  -727,  -727,   871,   405,  -727,   872,    96,    96,
    -727,  -727,  -727,    96,    96,  -727,  -727,  -727,  -727,  -727,
    -727,  -727,   881,   263,  -727,  1414,  1414,   629,   639,   698,
     698,   645,   653,   698,   698,  1286,   882,  -727,  -727,  -727,
    -727,  -727,   559,    96,   725,   825,   880,  -727,  -727,  -727,
    -727,  -727,  1359,  1359,  1417,  1417,  1344,  1344,  1344,  1274,
    -727,   875,   812,   827,  -727,   883,   885,   836,   845,  -727,
     898,   827,   850,  -727,   100,   903,   963,    96,  -727,    96,
    -727,  -727,    96,  -727,  -727,   982,  1040,  1059,  -727,   897,
    1117,  -727,   921,   725,   893,  -727,  -727,  1454,  1454,   297,
     297,  -727,  -727,  -727,   907,  1283,  -727,  -727,  1283,   655,
     663,   665,   667,   725,   767,  -727,  -727,  -727,  1483,  1483,
     918,   919,  1512,  1512,   922,   923,     7,  1440,   559,  -727,
    -727,    96,    96,   912,  -727,  -727,  -727,  -727,   932,   686,
    -727,   264,  -727,  -727,  -727,  -727,  -727,  -727,  -727,  -727,
    -727,  -727,  -727,  -727,  -727,  -727,  -727,  -727,  -727,  -727,
    -727,   840,  -727,  -727,  -727,  -727,  -727,  -727,   683,   695,
     697,  -727,  -727,  -727,  1059,  -727,  1117,  -727,   905,  -727,
    -727,  -727,  -727,  -727,  -727,  -727,   681,   681,  1356,  1356,
     929,  -727,  -727,  -727,  1483,  1483,  -727,  -727,  1512,  1512,
    -727,    96,    96,  -727,  -727,   920,   701,   715,  1274,  1538,
    1541,  1541,   930,  1136,  1194,  1213,  -727,  -727,  -727,  -727,
    -727,  -727,  -727,  1274,  -727,  -727,  -727,  -727,   719,   721,
    -727,   559,   559,   926,    96,    96,  -727,  -727,    96,    96,
    -727,  -727,  -727,   559,  -727,  -727,  1440,  1440,  -727,  -727,
    -727,   726,   730,   732,   744,  -727,  -727,  -727,  1538,  1538,
    1541,  1541,  -727,  -727,  -727,  -727
};

/* YYPGOTO[NTERM-NUM].  */
static const short int yypgoto[] =
{
    -727,  -727,    22,   810,   539,  -727,   822,  -158,  -727,   651,
     344,  -727,   358,  -727,  -727,  -727,     8,  -727,   741,   477,
      55,   143,    11,  -191,   156,  -169,  -166,  -160,  -150,  -727,
     112,  -727,  -727,   575,   591,    90,   174,   296,  -727,    21,
    -727,  -727,  -727,  -727,   660,  -727,  -727,  -727,  -727,    59,
    -727,  -727,   784,   -11,  -727,  -727,  -727,   935,    56,  -727,
    -727,   614,  -225,    -6,  1001,  -727,    -5,   -76,   766,   617,
    -213,  -238,  -727,   334,  -203,  -221,  -233,  -727,  -550,  -215,
    -240,  -727,  -175,  -517,  -537,  -516,  -237,  -460,  -727,  -727,
     778,  -727,  -727,   354,  -727,  -727,   355,  -727,   360,  -727,
    -727,   361,  -727,  -727,   900,    52,  -727,   768,    15,  -727,
    -727,   765,  -727,   902,  -399,  -259,  -727,   319,  -276,  -626,
    -282,  -285,  -112,  -383,  -727,   904,  -254,   332,  -727,  -414,
    -727,  -412,  -727,  -727,  -117,   217,  -406,    31,  -410,  -429,
    -727,  -247,   124,  -727,  -727,  -727,   884,  -727,  -727,   543,
    -281,   320,  -656,     1,  -709,   -24,  -726,  -706,  -668,   -10,
    -727,  -727,  -727,   886,  -727,  -727,   728,  -727,  -727
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -327
static const short int yytable[] =
{
      84,   410,   387,   135,   413,   349,   418,   282,   282,   233,
     262,    48,   449,   424,   484,   328,   482,   482,   447,   678,
     407,   476,   161,   488,    25,   154,   409,   628,   353,   353,
     416,   416,   355,   607,   681,   469,   165,   256,   406,   359,
     359,   594,   421,   595,   597,   180,   595,   186,   673,   793,
     361,   605,   605,   600,   673,   682,   220,   221,   221,   183,
     187,   145,   795,    74,   225,   609,   610,   211,   144,   153,
     384,   590,   -19,   429,   830,   830,   519,    13,   539,   226,
     608,   161,   782,   783,   201,   202,   385,    86,    87,   542,
     826,    34,   549,   180,   208,   832,   554,   -19,    35,   249,
      36,    81,    37,   386,    38,   252,   209,   210,    39,  -108,
     160,   212,   213,   332,   203,   204,   384,   135,   135,    84,
      84,   520,   161,   160,   830,   830,   230,   763,   336,    40,
      41,    42,   229,   164,   543,   337,   300,   550,   303,   826,
     826,   555,   205,   765,   854,   855,   169,   312,   485,   450,
     338,   302,   315,   161,    46,    47,   160,   653,   814,   815,
     658,    67,    68,    69,   142,    71,   110,   650,   793,   793,
     662,   154,   654,   332,   317,   659,    80,   845,   197,   198,
      95,   753,   580,   581,   333,   663,  -198,   193,   194,    81,
     160,   323,   324,   325,   754,   685,   686,   143,  -196,   145,
     565,    67,    68,    69,    70,    71,   144,   220,   174,   177,
     179,   221,   243,   221,   806,   153,   706,   707,   434,   265,
     632,   166,   244,   266,   245,   332,   390,   246,   435,   807,
     633,   234,   563,   247,   528,   226,   317,   377,   247,   249,
      67,    68,    69,    70,    71,   110,   111,   112,   319,   113,
     267,   268,   170,   320,   763,   332,   628,   269,   270,   232,
      84,    84,    80,   317,   537,   199,   200,   519,   765,   271,
     635,   290,   291,   328,   272,    81,   560,   171,   382,   636,
     382,   594,   341,   595,   775,   172,   595,    95,   673,   834,
     173,   684,   135,   135,   135,   298,   299,   704,   341,   303,
     466,   372,   373,   374,   769,   770,   705,   802,   387,   307,
     308,   317,   302,   362,   363,   364,   365,   366,   329,   330,
     369,  -325,  -325,   410,   195,  -325,   413,   582,   413,   410,
     424,  -306,   413,  -306,   413,   265,   424,   694,   495,   444,
     508,   668,   262,   510,   488,   511,   196,   513,   670,   811,
     812,   262,   515,   214,   679,   206,   509,   215,   575,   406,
     710,   711,   315,   316,   714,   715,   267,   268,   605,   605,
     600,   600,   207,   445,   446,   456,   457,   216,    80,   458,
    -320,  -320,   540,   217,  -320,   271,   340,   341,   375,   376,
     429,    81,   226,   534,   535,   514,   429,   516,   517,   230,
     529,   439,   440,   531,  -321,  -321,  -326,  -326,  -321,   231,
    -326,   235,   451,   452,   236,    95,    95,    95,    95,    95,
     423,   426,   428,   342,   341,   237,   410,   786,   787,   413,
     482,   482,   476,   476,   696,   697,   238,   761,   698,    84,
     343,   341,   447,   447,   424,   315,   381,   392,   341,   135,
      84,   349,   349,    84,    84,   239,    84,    84,   135,   135,
     135,   589,   416,   406,   241,   651,   262,   262,   242,   470,
     471,   655,   656,   421,   450,   353,   353,   660,   737,   738,
     303,   626,   627,   665,   315,   431,   359,   359,   359,   248,
     521,   522,   253,   302,   523,   524,   739,   740,   741,   525,
     526,   527,   292,   816,   817,   429,   482,   482,   476,   476,
     461,   462,   293,   265,   464,   341,   294,   444,   295,    67,
      68,    69,    70,    71,   296,   111,   640,   151,   152,   465,
     341,   424,   506,   341,   507,   341,   209,   210,   578,   341,
     297,   226,   579,   341,   267,   268,   587,   341,   561,   588,
     341,   269,   270,   642,   341,    81,    80,   314,   566,   643,
     341,   318,   567,   271,   410,   568,    95,   413,   413,    81,
     570,   424,   322,   135,   135,   262,   262,   331,   576,   644,
     341,   339,    84,    84,   645,   341,    84,   334,   583,   584,
     335,   429,   344,   646,   341,   647,   341,   717,   648,   341,
     406,   718,   666,   341,   675,   341,   676,   341,    95,   378,
     135,   135,   677,   341,   380,   617,   618,   680,   341,   315,
     303,   621,   622,   689,   341,  -161,   391,   724,   719,   720,
     429,    34,   383,   302,   602,   721,   722,   388,    35,  -161,
      36,   393,    37,   395,    38,   181,   398,   723,    39,  -108,
     400,  -161,  -161,    81,   433,  -108,   732,   742,  -108,   690,
     341,  -161,   708,   341,  -161,   443,  -161,   240,  -108,    40,
      41,    42,   709,   341,   437,    43,    44,    45,   712,   341,
     177,   177,   135,   135,    84,    84,   713,   341,   776,   341,
      84,   441,   808,    84,    46,    47,   777,   341,   778,   341,
     779,   341,   438,   135,   135,   767,   442,   135,   135,   800,
     801,   627,   724,   724,   453,    95,   803,   341,   454,   265,
     455,    95,    95,   444,   459,   780,   460,    95,   804,   341,
     805,   341,   463,    95,   821,   341,    95,   423,   671,   428,
     467,   699,   700,   423,   486,   428,   701,   702,   822,   341,
     267,   268,   836,   341,   837,   341,   487,   598,   599,   848,
     341,   503,    80,   849,   341,   850,   341,    14,   492,   271,
     489,    84,    84,    84,    84,    81,   731,   851,   341,   135,
     135,   367,   368,   135,   135,    15,    16,    17,    18,    19,
      20,    21,    22,    23,   724,   724,   724,   491,   735,   736,
     370,   371,   637,   638,   691,   692,   823,   809,   810,   493,
     758,   494,   759,   771,   772,   760,   724,   724,   496,    24,
     504,   835,   838,   839,   852,   853,   846,   847,   724,   497,
     505,   724,   724,   518,   530,   532,   536,   541,   547,   548,
      95,   553,   559,   724,   724,   724,   724,   562,   564,    95,
      95,   423,   569,   571,   428,   574,   585,   577,   586,    34,
     611,   612,   615,   616,   796,   797,    35,    14,    36,   619,
      85,   630,    38,   620,   623,   624,    39,   631,   634,   649,
     639,   652,   657,   661,   664,    15,    16,    17,    18,    19,
      20,    21,    22,    23,   683,   687,   688,    40,    41,    42,
     693,   695,  -315,    43,    44,    45,   744,   716,    86,    87,
      88,    89,    90,    91,    92,    93,   703,   319,   743,   498,
      34,   543,    94,    47,   764,   768,   746,    35,   747,    36,
     748,    37,   402,    38,   818,   819,    34,    39,   423,   550,
     428,   750,    95,    35,   555,    36,   755,    85,   766,    38,
     773,   784,   785,    39,   798,   788,   789,   799,    40,    41,
      42,   813,   833,   820,    43,    44,    45,   841,   842,   840,
     572,   843,   844,   502,    40,    41,    42,    95,   423,   428,
     734,   404,   326,   403,    47,    86,    87,    88,    89,    90,
      91,    92,    93,   733,    34,   327,   641,   512,   533,    94,
      47,    35,   379,    36,   224,    85,    82,    38,   414,   538,
     762,    39,   749,    34,   745,   397,   257,   752,   432,   436,
      35,   751,    36,   781,    85,   264,    38,   774,   625,   289,
      39,   306,    40,    41,    42,     0,   790,   490,     0,     0,
       0,     0,   313,    86,    87,   405,    89,    90,    91,    92,
      93,    40,    41,    42,     0,     0,     0,    94,    47,     0,
       0,     0,    86,    87,   415,    89,    90,    91,    92,    93,
       0,    34,     0,     0,     0,     0,    94,    47,    35,     0,
      36,     0,    85,     0,    38,     0,     0,     0,    39,     0,
      34,     0,     0,     0,     0,     0,     0,    35,     0,    36,
       0,    85,     0,    38,     0,     0,     0,    39,     0,    40,
      41,    42,     0,     0,     0,     0,     0,     0,     0,     0,
      86,    87,   419,    89,    90,    91,    92,    93,    40,    41,
      42,     0,     0,     0,    94,    47,     0,     0,     0,    86,
      87,   422,    89,    90,    91,    92,    93,     0,    34,     0,
       0,     0,     0,    94,    47,    35,     0,    36,     0,    85,
       0,    38,     0,     0,     0,    39,     0,    34,     0,     0,
       0,     0,     0,     0,    35,     0,    36,     0,    85,     0,
      38,     0,     0,     0,    39,     0,    40,    41,    42,     0,
       0,     0,     0,     0,     0,     0,     0,    86,    87,   427,
      89,    90,    91,    92,    93,    40,    41,    42,     0,     0,
       0,    94,    47,     0,     0,     0,    86,    87,   667,    89,
      90,    91,    92,    93,     0,    34,     0,     0,     0,     0,
      94,    47,    35,     0,    36,     0,    85,     0,    38,     0,
       0,     0,    39,     0,    34,     0,     0,     0,     0,     0,
       0,    35,     0,    36,     0,    85,     0,    38,     0,     0,
       0,    39,     0,    40,    41,    42,     0,     0,     0,     0,
       0,     0,     0,     0,    86,    87,   669,    89,    90,    91,
      92,    93,    40,    41,    42,     0,     0,     0,    94,    47,
       0,     0,     0,    86,    87,   672,    89,    90,    91,    92,
      93,     0,    34,     0,     0,    34,     0,    94,    47,    35,
       0,    36,    35,    37,    36,    38,    37,    34,    38,    39,
     184,   265,    39,     0,    35,   591,    36,     0,    37,     0,
      38,     0,     0,     0,    39,     0,     0,     0,     0,     0,
      40,    41,    42,    40,    41,    42,    43,    44,    45,    43,
      44,    45,   267,   268,     0,    40,    41,    42,     0,   269,
     270,    43,    44,    45,    80,   185,    47,     0,    46,    47,
       0,   271,     0,     0,     0,    34,   592,    81,    34,     0,
     301,    47,    35,     0,    36,    35,    37,    36,    38,    37,
      34,    38,    39,     0,   265,    39,     0,    35,   444,    36,
       0,    37,     0,    38,     0,     0,     0,    39,     0,     0,
       0,     0,     0,    40,    41,    42,    40,    41,    42,   356,
     357,   358,     0,   175,   176,   267,   268,     0,    40,    41,
      42,     0,   603,   604,     0,   347,   348,    80,    46,    47,
       0,    46,    47,     0,   271,     0,     0,     0,    34,     0,
      81,     0,   122,    46,    47,    35,   123,    36,   124,    37,
     125,    38,     0,     0,     0,    39,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,     0,   717,     0,
       0,     0,   718,   126,   127,     0,    40,    41,    42,     0,
     128,   129,   122,   351,   352,     0,   123,     0,   124,     0,
     125,     0,   130,   131,   132,   133,   134,     0,    81,   719,
     720,    46,    47,     0,     0,     0,   791,   792,     0,     0,
       0,   122,     0,   126,   127,   123,     0,   124,   723,   125,
     258,   259,     0,     0,    81,     0,     0,     0,     0,     0,
       0,     0,   130,   260,   261,   133,   134,     0,    81,     0,
     122,     0,   126,   127,   123,     0,   124,     0,   125,   472,
     473,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   130,   474,   475,   133,   134,   717,    81,     0,   717,
     718,   126,   127,   718,     0,     0,     0,     0,   478,   479,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     130,   480,   481,   133,   134,     0,    81,   719,   720,     0,
     719,   720,     0,     0,   824,   825,     0,   828,   829,     0,
       0,     0,     0,     0,     0,     0,   723,     0,     0,   723,
       0,     0,    81,     0,     0,    81
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
       0,     0,     0,     0,     0,     0,     0,     0,     0,    17,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    19,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    21,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    15,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    23,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    11,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    13,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     1,
       0,     0,     0,     0,     0,     3,     0,     0,     5,     0,
       0,     0,     0,     0,     0,     0,     0,     7,     9,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
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

/* YYCONFL[I] -- lists of conflicting rule numbers, each terminated by
   0, pointed into by YYCONFLP.  */
static const short int yyconfl[] =
{
       0,   161,     0,   161,     0,   161,     0,   108,     0,   161,
       0,    21,     0,   108,     0,   314,     0,   161,     0,   108,
       0,   198,     0,   314,     0
};

static const short int yycheck[] =
{
       5,   241,   227,     8,   241,   196,   244,   124,   125,    85,
     122,     3,   266,   246,   299,   173,   298,   299,   265,   569,
     241,   297,    11,   304,     2,    10,   241,   487,   197,   198,
     243,   244,   198,   462,   571,   294,    14,   113,   241,   199,
     200,   455,   245,   455,   458,    37,   458,    39,   565,   717,
     200,   461,   462,   459,   571,   571,    67,    68,    69,    38,
      39,     9,   718,     4,    70,   464,   465,    28,     9,    10,
      54,   454,    15,   248,   800,   801,    34,    94,    49,    71,
     463,    70,   708,   709,    16,    17,    15,    80,    81,    49,
     799,    31,    49,    85,    18,   801,    49,    40,    38,   110,
      40,    94,    42,    32,    44,   111,    30,    31,    48,    25,
      94,    72,    73,    34,    46,    47,    54,   122,   123,   124,
     125,    79,   111,    94,   850,   851,    42,   677,    49,    69,
      70,    71,    73,     0,    94,    34,   142,    94,   143,   848,
     849,    94,    74,   680,   850,   851,    42,   152,    86,   266,
      49,   143,    34,   142,    94,    95,    94,    34,   784,   785,
      34,    50,    51,    52,    53,    54,    55,    49,   836,   837,
      34,   156,    49,    34,   163,    49,    81,   833,    23,    24,
       6,    81,   441,   442,    45,    49,    27,    44,    45,    94,
      94,   169,   170,   171,    94,   578,   579,    86,    39,   147,
      41,    50,    51,    52,    53,    54,   147,   218,    34,    35,
      36,   222,    14,   224,   764,   156,   615,   616,    32,    38,
      32,    94,    24,    42,    26,    34,   231,    33,    42,   766,
      42,    88,    33,    39,    43,   227,   225,   216,    39,   250,
      50,    51,    52,    53,    54,    55,    56,    57,    37,    59,
      69,    70,    42,    42,   804,    34,   716,    76,    77,    85,
     265,   266,    81,   252,    43,    21,    22,    34,   805,    88,
      34,   128,   129,   431,    93,    94,    43,    42,   222,    43,
     224,   695,    34,   695,   698,    15,   698,   113,   805,   805,
      40,    43,   297,   298,   299,    23,    24,    34,    34,   304,
     292,   211,   212,   213,   687,   688,    43,    43,   533,    89,
      90,   300,   304,   201,   202,   203,   204,   205,   175,   176,
     208,    29,    30,   563,    78,    33,   563,   444,   565,   569,
     563,    45,   569,    47,   571,    38,   569,   591,   316,    42,
     332,   562,   454,   335,   625,   337,    25,   339,   563,   778,
     779,   463,   341,    29,   569,    19,   334,    33,   434,   562,
     619,   620,    34,    35,   623,   624,    69,    70,   778,   779,
     776,   777,    20,    76,    77,    29,    30,    42,    81,    33,
      29,    30,   393,    94,    33,    88,    33,    34,   214,   215,
     565,    94,   384,   385,   386,   340,   571,   342,   343,    42,
     378,   258,   259,   381,    29,    30,    29,    30,    33,    41,
      33,    42,   269,   270,    42,   241,   242,   243,   244,   245,
     246,   247,   248,    33,    34,    42,   666,   712,   713,   666,
     712,   713,   708,   709,    29,    30,    42,   675,    33,   444,
      33,    34,   689,   690,   677,    34,    35,    33,    34,   454,
     455,   642,   643,   458,   459,    42,   461,   462,   463,   464,
     465,   453,   675,   666,    15,   541,   578,   579,    30,   295,
     296,   547,   548,   676,   591,   644,   645,   553,   644,   645,
     485,   486,   487,   559,    34,    35,   646,   647,   648,    41,
     347,   348,    94,   485,   351,   352,   646,   647,   648,   356,
     357,   358,    42,   788,   789,   680,   788,   789,   784,   785,
      23,    24,    94,    38,    33,    34,    94,    42,    39,    50,
      51,    52,    53,    54,    39,    56,   518,    58,    59,    33,
      34,   764,    33,    34,    33,    34,    30,    31,    33,    34,
      25,   533,    33,    34,    69,    70,    33,    34,   405,    33,
      34,    76,    77,    33,    34,    94,    81,    36,   415,    33,
      34,    43,   419,    88,   804,   422,   392,   804,   805,    94,
     427,   804,    41,   578,   579,   687,   688,    43,   435,    33,
      34,    41,   587,   588,    33,    34,   591,    35,   445,   446,
      35,   766,    94,    33,    34,    33,    34,    38,    33,    34,
     803,    42,    33,    34,    33,    34,    33,    34,   434,    32,
     615,   616,    33,    34,    36,   472,   473,    33,    34,    34,
     625,   478,   479,    33,    34,     0,    43,   632,    69,    70,
     805,    31,    36,   625,   460,    76,    77,    36,    38,    14,
      40,    48,    42,    48,    44,    45,    48,    88,    48,    24,
      48,    26,    27,    94,    36,    30,   634,   649,    33,    33,
      34,    36,    33,    34,    39,    43,    41,    42,    43,    69,
      70,    71,    33,    34,    36,    75,    76,    77,    33,    34,
     506,   507,   687,   688,   689,   690,    33,    34,    33,    34,
     695,    94,   768,   698,    94,    95,    33,    34,    33,    34,
      33,    34,    36,   708,   709,   683,    94,   712,   713,    23,
      24,   716,   717,   718,    42,   541,    33,    34,    45,    38,
      30,   547,   548,    42,    25,   703,    39,   553,    33,    34,
      33,    34,    47,   559,    33,    34,   562,   563,   564,   565,
      42,   598,   599,   569,    15,   571,   603,   604,    33,    34,
      69,    70,    33,    34,    33,    34,    25,    76,    77,    33,
      34,    43,    81,    33,    34,    33,    34,    42,    94,    88,
      36,   776,   777,   778,   779,    94,   633,    33,    34,   784,
     785,   206,   207,   788,   789,    60,    61,    62,    63,    64,
      65,    66,    67,    68,   799,   800,   801,    36,   642,   643,
     209,   210,   506,   507,   587,   588,   798,   776,   777,    36,
     667,    94,   669,   689,   690,   672,   821,   822,    36,    94,
      43,   813,   821,   822,   848,   849,   836,   837,   833,    94,
      43,   836,   837,    32,    36,    36,    36,    34,    34,    34,
     666,    34,    34,   848,   849,   850,   851,    27,    39,   675,
     676,   677,    15,    15,   680,    36,    43,    36,    43,    31,
      43,    94,    33,    33,   721,   722,    38,    42,    40,    94,
      42,    36,    44,    94,    94,    94,    48,    36,    35,    32,
      49,    41,    15,    41,    15,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    15,    33,    33,    69,    70,    71,
      43,    30,    30,    75,    76,    77,    94,    25,    80,    81,
      82,    83,    84,    85,    86,    87,    35,    37,    43,    94,
      31,    94,    94,    95,    27,    32,    43,    38,    43,    40,
      94,    42,    43,    44,   791,   792,    31,    48,   764,    94,
     766,    43,   768,    38,    94,    40,    43,    42,    27,    44,
      43,    33,    33,    48,    42,    33,    33,    25,    69,    70,
      71,    32,    32,    43,    75,    76,    77,   824,   825,    43,
     431,   828,   829,   322,    69,    70,    71,   803,   804,   805,
     636,   240,   172,    94,    95,    80,    81,    82,    83,    84,
      85,    86,    87,   635,    31,   173,   519,   337,   384,    94,
      95,    38,   218,    40,    69,    42,     5,    44,   242,   392,
     676,    48,   658,    31,   653,   237,   116,   662,   250,   254,
      38,   661,    40,   704,    42,   123,    44,   695,   485,   125,
      48,   147,    69,    70,    71,    -1,   716,   309,    -1,    -1,
      -1,    -1,   156,    80,    81,    82,    83,    84,    85,    86,
      87,    69,    70,    71,    -1,    -1,    -1,    94,    95,    -1,
      -1,    -1,    80,    81,    82,    83,    84,    85,    86,    87,
      -1,    31,    -1,    -1,    -1,    -1,    94,    95,    38,    -1,
      40,    -1,    42,    -1,    44,    -1,    -1,    -1,    48,    -1,
      31,    -1,    -1,    -1,    -1,    -1,    -1,    38,    -1,    40,
      -1,    42,    -1,    44,    -1,    -1,    -1,    48,    -1,    69,
      70,    71,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      80,    81,    82,    83,    84,    85,    86,    87,    69,    70,
      71,    -1,    -1,    -1,    94,    95,    -1,    -1,    -1,    80,
      81,    82,    83,    84,    85,    86,    87,    -1,    31,    -1,
      -1,    -1,    -1,    94,    95,    38,    -1,    40,    -1,    42,
      -1,    44,    -1,    -1,    -1,    48,    -1,    31,    -1,    -1,
      -1,    -1,    -1,    -1,    38,    -1,    40,    -1,    42,    -1,
      44,    -1,    -1,    -1,    48,    -1,    69,    70,    71,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    80,    81,    82,
      83,    84,    85,    86,    87,    69,    70,    71,    -1,    -1,
      -1,    94,    95,    -1,    -1,    -1,    80,    81,    82,    83,
      84,    85,    86,    87,    -1,    31,    -1,    -1,    -1,    -1,
      94,    95,    38,    -1,    40,    -1,    42,    -1,    44,    -1,
      -1,    -1,    48,    -1,    31,    -1,    -1,    -1,    -1,    -1,
      -1,    38,    -1,    40,    -1,    42,    -1,    44,    -1,    -1,
      -1,    48,    -1,    69,    70,    71,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    80,    81,    82,    83,    84,    85,
      86,    87,    69,    70,    71,    -1,    -1,    -1,    94,    95,
      -1,    -1,    -1,    80,    81,    82,    83,    84,    85,    86,
      87,    -1,    31,    -1,    -1,    31,    -1,    94,    95,    38,
      -1,    40,    38,    42,    40,    44,    42,    31,    44,    48,
      49,    38,    48,    -1,    38,    42,    40,    -1,    42,    -1,
      44,    -1,    -1,    -1,    48,    -1,    -1,    -1,    -1,    -1,
      69,    70,    71,    69,    70,    71,    75,    76,    77,    75,
      76,    77,    69,    70,    -1,    69,    70,    71,    -1,    76,
      77,    75,    76,    77,    81,    94,    95,    -1,    94,    95,
      -1,    88,    -1,    -1,    -1,    31,    93,    94,    31,    -1,
      94,    95,    38,    -1,    40,    38,    42,    40,    44,    42,
      31,    44,    48,    -1,    38,    48,    -1,    38,    42,    40,
      -1,    42,    -1,    44,    -1,    -1,    -1,    48,    -1,    -1,
      -1,    -1,    -1,    69,    70,    71,    69,    70,    71,    75,
      76,    77,    -1,    76,    77,    69,    70,    -1,    69,    70,
      71,    -1,    76,    77,    -1,    76,    77,    81,    94,    95,
      -1,    94,    95,    -1,    88,    -1,    -1,    -1,    31,    -1,
      94,    -1,    38,    94,    95,    38,    42,    40,    44,    42,
      46,    44,    -1,    -1,    -1,    48,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    -1,    38,    -1,
      -1,    -1,    42,    69,    70,    -1,    69,    70,    71,    -1,
      76,    77,    38,    76,    77,    -1,    42,    -1,    44,    -1,
      46,    -1,    88,    89,    90,    91,    92,    -1,    94,    69,
      70,    94,    95,    -1,    -1,    -1,    76,    77,    -1,    -1,
      -1,    38,    -1,    69,    70,    42,    -1,    44,    88,    46,
      76,    77,    -1,    -1,    94,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    88,    89,    90,    91,    92,    -1,    94,    -1,
      38,    -1,    69,    70,    42,    -1,    44,    -1,    46,    76,
      77,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    88,    89,    90,    91,    92,    38,    94,    -1,    38,
      42,    69,    70,    42,    -1,    -1,    -1,    -1,    76,    77,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      88,    89,    90,    91,    92,    -1,    94,    69,    70,    -1,
      69,    70,    -1,    -1,    76,    77,    -1,    76,    77,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    88,    -1,    -1,    88,
      -1,    -1,    94,    -1,    -1,    94
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned short int yystos[] =
{
       0,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    97,    94,    42,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    94,    98,    99,   100,   101,   102,
     103,   109,   110,   111,    31,    38,    40,    42,    44,    48,
      69,    70,    71,    75,    76,    77,    94,    95,   112,   113,
     116,   119,   121,   123,   125,   126,   127,   128,   129,   130,
     131,   132,   134,   136,   137,   138,   141,    50,    51,    52,
      53,    54,   143,   144,   145,   146,   151,   152,   155,   156,
      81,    94,   160,   161,   162,    42,    80,    81,    82,    83,
      84,    85,    86,    87,    94,   132,   162,   163,   164,   165,
     166,   168,   170,   172,   176,   178,   182,   183,   184,   185,
      55,    56,    57,    59,   145,   198,   199,   200,   201,   204,
     205,   208,    38,    42,    44,    46,    69,    70,    76,    77,
      88,    89,    90,    91,    92,   162,   209,   210,   214,   216,
     218,   220,    53,    86,   145,   201,   240,   241,   242,   243,
     244,    58,    59,   145,   204,   257,   258,   259,   260,   264,
      94,   118,   149,   159,     0,    98,    94,   104,   105,    42,
      42,    42,    15,    40,   132,    76,    77,   132,   133,   132,
     112,    45,   112,   135,    49,    94,   112,   135,   139,   140,
     142,   117,   118,   117,   117,    78,    25,    23,    24,    21,
      22,    16,    17,    46,    47,    74,    19,    20,    18,    30,
      31,    28,    72,    73,    29,    33,    42,    94,   147,   148,
     149,   149,   153,   154,   153,   159,   112,   157,   158,   145,
      42,    41,   132,   163,   117,    42,    42,    42,    42,    42,
      42,    15,    30,    14,    24,    26,    33,    39,    41,   149,
     202,   203,   159,    94,   206,   207,   163,   200,    76,    77,
      89,    90,   218,   219,   209,    38,    42,    69,    70,    76,
      77,    88,    93,   160,   221,   222,   224,   225,   226,   227,
     228,   229,   230,   231,   232,   234,   236,   237,   239,   221,
     117,   117,    42,    94,    94,    39,    39,    25,    23,    24,
     159,    94,   112,   162,   245,   246,   242,    89,    90,   261,
     262,   263,   162,   259,    36,    34,    35,   118,    43,    37,
      42,   106,    41,    98,    98,    98,    99,   102,   103,   117,
     117,    43,    34,    45,    35,    35,    49,    34,    49,    41,
      33,    34,    33,    33,    94,   114,   115,    76,    77,   119,
     120,    76,    77,   121,   122,   122,    75,    76,    77,   123,
     124,   124,   126,   126,   126,   126,   126,   129,   129,   126,
     130,   130,   131,   131,   131,   132,   132,   135,    32,   148,
      36,    35,   154,    36,    54,    15,    32,   158,    36,   135,
     162,    43,    33,    48,   186,    48,   195,   186,    48,   187,
      48,   190,    43,    94,   114,    82,   170,   171,   173,   175,
     176,   177,   179,   182,   164,    82,   166,   167,   167,    82,
     169,   170,    82,   132,   172,   174,   132,    82,   132,   178,
     180,    35,   203,    36,    32,    42,   207,    36,    36,   117,
     117,    94,    94,    43,    42,    76,    77,   237,   238,   222,
     230,   117,   117,    42,    45,    30,    29,    30,    33,    25,
      39,    23,    24,    47,    33,    33,   112,    42,   211,   211,
     132,   132,    76,    77,    89,    90,   214,   215,    76,    77,
      89,    90,   216,   217,   217,    86,    15,    25,   246,    36,
     262,    36,    94,    36,    94,    98,    36,    94,    94,    98,
     107,   108,   105,    43,    43,    43,    33,    33,   112,    98,
     112,   112,   140,   112,   116,   118,   116,   116,    32,    34,
      79,   117,   117,   117,   117,   117,   117,   117,    43,    98,
      36,    98,    36,   157,   112,   112,    36,    43,   165,    49,
     149,    34,    49,    94,   194,   196,   197,    34,    34,    49,
      94,   188,   189,    34,    49,    94,   191,   192,   193,    34,
      43,   117,    27,    33,    39,    41,   117,   117,   117,    15,
     117,    15,   100,   150,    36,   163,   117,    36,    33,    33,
     211,   211,   230,   117,   117,    43,    43,    33,    33,   112,
     219,    42,    93,   223,   225,   227,   230,   225,    76,    77,
     232,   233,   132,    76,    77,   234,   235,   235,   219,   210,
     210,    43,    94,   212,   213,    33,    33,   117,   117,    94,
      94,   117,   117,    94,    94,   245,   162,   162,   183,   247,
      36,    36,    32,    42,    35,    34,    43,   133,   133,    49,
     112,   115,    33,    33,    33,    33,    33,    33,    33,    32,
      49,   163,    41,    34,    49,   163,   163,    15,    34,    49,
     163,    41,    34,    49,    15,   163,    33,    82,   171,    82,
     175,   132,    82,   179,   181,    33,    33,    33,   174,   175,
      33,   180,   181,    15,    43,   219,   219,    33,    33,    33,
      33,   231,   231,    43,   222,    30,    29,    30,    33,   117,
     117,   117,   117,    35,    34,    43,   210,   210,    33,    33,
     211,   211,    33,    33,   211,   211,    25,    38,    42,    69,
      70,    76,    77,    88,   162,   248,   249,   250,   252,   254,
     256,   117,    98,   108,   106,   120,   120,   122,   122,   124,
     124,   124,   112,    43,    94,   197,    43,    43,    94,   189,
      43,   194,   192,    81,    94,    43,   171,   175,   117,   117,
     117,   167,   169,   174,    27,   180,    27,    98,    32,   219,
     219,   238,   238,    43,   223,   225,    33,    33,    33,    33,
      98,   213,   215,   215,    33,    33,   217,   217,    33,    33,
     247,    76,    77,   254,   255,   248,   117,   117,    42,    25,
      23,    24,    43,    33,    33,    33,   174,   180,   163,   233,
     233,   235,   235,    32,   215,   215,   217,   217,   117,   117,
      43,    33,    33,   112,    76,    77,   250,   251,    76,    77,
     252,   253,   253,    32,   181,   112,    33,    33,   249,   249,
      43,   117,   117,   117,   117,   248,   255,   255,    33,    33,
      33,    33,   251,   251,   253,   253
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
#line 384 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).term), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl));
      mcrl2_spec_tree = ((*yyvalp).term);
    ;}
    break;

  case 3:

/* Line 936 of glr.c  */
#line 389 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).term), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl));
      mcrl2_spec_tree = ((*yyvalp).term);
    ;}
    break;

  case 4:

/* Line 936 of glr.c  */
#line 394 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).term), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl));
      mcrl2_spec_tree = ((*yyvalp).term);
    ;}
    break;

  case 5:

/* Line 936 of glr.c  */
#line 399 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).term), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl));
      mcrl2_spec_tree = ((*yyvalp).term);
    ;}
    break;

  case 6:

/* Line 936 of glr.c  */
#line 404 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).term), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl));
      mcrl2_spec_tree = ((*yyvalp).term);
    ;}
    break;

  case 7:

/* Line 936 of glr.c  */
#line 409 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).term), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl));
      mcrl2_spec_tree = ((*yyvalp).term);
    ;}
    break;

  case 8:

/* Line 936 of glr.c  */
#line 414 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).term), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl));
      mcrl2_spec_tree = ((*yyvalp).term);
    ;}
    break;

  case 9:

/* Line 936 of glr.c  */
#line 419 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).term), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl));
      mcrl2_spec_tree = ((*yyvalp).term);
    ;}
    break;

  case 10:

/* Line 936 of glr.c  */
#line 424 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).term), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl));
      mcrl2_spec_tree = ((*yyvalp).term);
    ;}
    break;

  case 11:

/* Line 936 of glr.c  */
#line 429 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).term), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl));
      mcrl2_spec_tree = ((*yyvalp).term);
    ;}
    break;

  case 12:

/* Line 936 of glr.c  */
#line 434 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).term), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.list));
      mcrl2_spec_tree = ((*yyvalp).term);
    ;}
    break;

  case 13:

/* Line 936 of glr.c  */
#line 446 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed sort expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 14:

/* Line 936 of glr.c  */
#line 455 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 15:

/* Line 936 of glr.c  */
#line 459 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSortArrow((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed arrow sort\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 16:

/* Line 936 of glr.c  */
#line 468 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed non-arrow domain\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 17:

/* Line 936 of glr.c  */
#line 477 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed non-arrow domain elements\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 18:

/* Line 936 of glr.c  */
#line 482 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed non-arrow domain elements\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 19:

/* Line 936 of glr.c  */
#line 491 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed non-arrow domain element\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 20:

/* Line 936 of glr.c  */
#line 500 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 21:

/* Line 936 of glr.c  */
#line 504 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSortStruct(ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.list))));
      gsDebugMsg("parsed structured sort\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 22:

/* Line 936 of glr.c  */
#line 513 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed structured sort constructors\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 23:

/* Line 936 of glr.c  */
#line 518 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed structured sort constructors\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 24:

/* Line 936 of glr.c  */
#line 527 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStructCons((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl), ATmakeList0(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed structured sort constructor\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 25:

/* Line 936 of glr.c  */
#line 532 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStructCons((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.appl), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.list)), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed structured sort constructor\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 26:

/* Line 936 of glr.c  */
#line 541 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeNil());
      gsDebugMsg("parsed recogniser\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 27:

/* Line 936 of glr.c  */
#line 546 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed recogniser id\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 28:

/* Line 936 of glr.c  */
#line 555 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed structured sort projections\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 29:

/* Line 936 of glr.c  */
#line 560 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed structured sort projections\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 30:

/* Line 936 of glr.c  */
#line 569 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStructProj(gsMakeNil(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed structured sort projection\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 31:

/* Line 936 of glr.c  */
#line 574 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStructProj((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed structured sort projection\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 32:

/* Line 936 of glr.c  */
#line 583 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSortId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed primary sort\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 33:

/* Line 936 of glr.c  */
#line 588 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed sort constant\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 34:

/* Line 936 of glr.c  */
#line 593 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed sort constructor\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 35:

/* Line 936 of glr.c  */
#line 598 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 36:

/* Line 936 of glr.c  */
#line 606 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSortId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 37:

/* Line 936 of glr.c  */
#line 610 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSortId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 38:

/* Line 936 of glr.c  */
#line 614 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSortId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 39:

/* Line 936 of glr.c  */
#line 618 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSortId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 40:

/* Line 936 of glr.c  */
#line 622 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSortId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
    ;}
    break;

  case 41:

/* Line 936 of glr.c  */
#line 630 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), mcrl2::data::sort_list::list(mcrl2::data::sort_expression((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl))));
    ;}
    break;

  case 42:

/* Line 936 of glr.c  */
#line 634 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), mcrl2::data::sort_set::set_(mcrl2::data::sort_expression((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl))));
    ;}
    break;

  case 43:

/* Line 936 of glr.c  */
#line 638 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), mcrl2::data::sort_bag::bag(mcrl2::data::sort_expression((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl))));
    ;}
    break;

  case 44:

/* Line 936 of glr.c  */
#line 649 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed data expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 45:

/* Line 936 of glr.c  */
#line 658 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 46:

/* Line 936 of glr.c  */
#line 662 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeWhr((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.appl), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.list))));
      gsDebugMsg("parsed where clause\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 47:

/* Line 936 of glr.c  */
#line 671 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed identifier initialisations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 48:

/* Line 936 of glr.c  */
#line 676 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed identifier initialisations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 49:

/* Line 936 of glr.c  */
#line 685 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeIdInit((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed identifier initialisation\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 50:

/* Line 936 of glr.c  */
#line 694 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 51:

/* Line 936 of glr.c  */
#line 698 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBinder(gsMakeLambda(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed lambda abstraction\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 52:

/* Line 936 of glr.c  */
#line 703 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBinder(gsMakeForall(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 53:

/* Line 936 of glr.c  */
#line 708 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBinder(gsMakeExists(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 54:

/* Line 936 of glr.c  */
#line 718 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list));
      gsDebugMsg("parsed data variable declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 55:

/* Line 936 of glr.c  */
#line 723 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATconcat((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed data variable declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 56:

/* Line 936 of glr.c  */
#line 732 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList0());
      int n = ATgetLength((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list));
      for (int i = 0; i < n; i++) {
        safe_assign(((*yyvalp).list), ATinsert(((*yyvalp).list), (ATerm) gsMakeDataVarId(ATAelementAt((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), i), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl))));
      }
      gsDebugMsg("parsed data variable declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 57:

/* Line 936 of glr.c  */
#line 745 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 58:

/* Line 936 of glr.c  */
#line 749 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl),
        gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)), ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl))));
      gsDebugMsg("parsed implication\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 59:

/* Line 936 of glr.c  */
#line 759 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 60:

/* Line 936 of glr.c  */
#line 763 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBinder(gsMakeForall(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 61:

/* Line 936 of glr.c  */
#line 768 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBinder(gsMakeExists(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 62:

/* Line 936 of glr.c  */
#line 777 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 63:

/* Line 936 of glr.c  */
#line 781 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl),
        gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)), ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl))));
      gsDebugMsg("parsed conjunction\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 64:

/* Line 936 of glr.c  */
#line 787 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl),
        gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)), ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl))));
      gsDebugMsg("parsed disjunction\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 65:

/* Line 936 of glr.c  */
#line 797 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 66:

/* Line 936 of glr.c  */
#line 801 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBinder(gsMakeForall(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 67:

/* Line 936 of glr.c  */
#line 806 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBinder(gsMakeExists(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 68:

/* Line 936 of glr.c  */
#line 815 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 69:

/* Line 936 of glr.c  */
#line 819 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl),
        gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)), ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl))));
      gsDebugMsg("parsed equality expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 70:

/* Line 936 of glr.c  */
#line 825 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl),
        gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)), ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl))));
      gsDebugMsg("parsed equality expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 71:

/* Line 936 of glr.c  */
#line 835 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 72:

/* Line 936 of glr.c  */
#line 839 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBinder(gsMakeLambda(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed lambda abstraction\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 73:

/* Line 936 of glr.c  */
#line 844 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBinder(gsMakeForall(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 74:

/* Line 936 of glr.c  */
#line 849 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBinder(gsMakeExists(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 75:

/* Line 936 of glr.c  */
#line 858 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 76:

/* Line 936 of glr.c  */
#line 862 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl),
        gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)), ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl))));
      gsDebugMsg("parsed relational expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 77:

/* Line 936 of glr.c  */
#line 868 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl),
        gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)), ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl))));
      gsDebugMsg("parsed relational expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 78:

/* Line 936 of glr.c  */
#line 874 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl),
        gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)), ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl))));
      gsDebugMsg("parsed relational expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 79:

/* Line 936 of glr.c  */
#line 880 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl),
        gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)), ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl))));
      gsDebugMsg("parsed relational expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 80:

/* Line 936 of glr.c  */
#line 886 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl),
        gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)), ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl))));
      gsDebugMsg("parsed relational expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 81:

/* Line 936 of glr.c  */
#line 896 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 82:

/* Line 936 of glr.c  */
#line 900 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl),
        gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)), ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl))));
      gsDebugMsg("parsed list cons expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 83:

/* Line 936 of glr.c  */
#line 910 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 84:

/* Line 936 of glr.c  */
#line 914 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl),
        gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)), ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl))));
      gsDebugMsg("parsed list snoc expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 85:

/* Line 936 of glr.c  */
#line 924 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 86:

/* Line 936 of glr.c  */
#line 928 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl),
        gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)), ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl))));
      gsDebugMsg("parsed list concat expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 87:

/* Line 936 of glr.c  */
#line 938 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 88:

/* Line 936 of glr.c  */
#line 942 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl),
        gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)), ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl))));
      gsDebugMsg("parsed addition or set union\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 89:

/* Line 936 of glr.c  */
#line 948 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl),
        gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)), ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl))));
      gsDebugMsg("parsed subtraction or set difference\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 90:

/* Line 936 of glr.c  */
#line 958 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 91:

/* Line 936 of glr.c  */
#line 962 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl),
        gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)), ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl))));
      gsDebugMsg("parsed div expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 92:

/* Line 936 of glr.c  */
#line 968 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl),
        gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)), ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl))));
      gsDebugMsg("parsed mod expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 93:

/* Line 936 of glr.c  */
#line 974 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl),
        gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)), ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl))));
      gsDebugMsg("parsed division expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 94:

/* Line 936 of glr.c  */
#line 984 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 95:

/* Line 936 of glr.c  */
#line 988 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl),
        gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)), ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl))));
      gsDebugMsg("parsed multiplication or set intersection\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 96:

/* Line 936 of glr.c  */
#line 994 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl),
        gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)), ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl))));
      gsDebugMsg("parsed list at expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 97:

/* Line 936 of glr.c  */
#line 1004 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 98:

/* Line 936 of glr.c  */
#line 1008 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl)), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl))));
      gsDebugMsg("parsed prefix data expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 99:

/* Line 936 of glr.c  */
#line 1013 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl)), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl))));
      gsDebugMsg("parsed prefix data expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 100:

/* Line 936 of glr.c  */
#line 1018 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeDataAppl(gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl)), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl))));
      gsDebugMsg("parsed prefix data expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 101:

/* Line 936 of glr.c  */
#line 1027 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 102:

/* Line 936 of glr.c  */
#line 1031 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBinder(gsMakeForall(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 103:

/* Line 936 of glr.c  */
#line 1036 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBinder(gsMakeExists(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 104:

/* Line 936 of glr.c  */
#line 1045 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 105:

/* Line 936 of glr.c  */
#line 1049 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeDataAppl((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.appl), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.list))));
      gsDebugMsg("parsed postfix data expression (function application)\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 106:

/* Line 936 of glr.c  */
#line 1064 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed data expressions\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 107:

/* Line 936 of glr.c  */
#line 1069 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed data expressions\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 108:

/* Line 936 of glr.c  */
#line 1078 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed primary data expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 109:

/* Line 936 of glr.c  */
#line 1083 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 110:

/* Line 936 of glr.c  */
#line 1087 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 111:

/* Line 936 of glr.c  */
#line 1091 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 112:

/* Line 936 of glr.c  */
#line 1095 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 113:

/* Line 936 of glr.c  */
#line 1103 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed data constant\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 114:

/* Line 936 of glr.c  */
#line 1108 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed data constant\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 115:

/* Line 936 of glr.c  */
#line 1113 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed data constant\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 116:

/* Line 936 of glr.c  */
#line 1118 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed data constant\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 117:

/* Line 936 of glr.c  */
#line 1123 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeId(mcrl2::data::sort_list::nil_name()));
      gsDebugMsg("parsed data constant\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 118:

/* Line 936 of glr.c  */
#line 1128 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeId(mcrl2::data::sort_set::emptyset_name()));
      gsDebugMsg("parsed data constant\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 119:

/* Line 936 of glr.c  */
#line 1137 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeDataAppl(gsMakeId(mcrl2::data::sort_list::list_enumeration_name()), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.list))));
      gsDebugMsg("parsed data enumeration\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 120:

/* Line 936 of glr.c  */
#line 1142 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeDataAppl(gsMakeId(mcrl2::data::sort_set::set_enumeration_name()), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.list))));
      gsDebugMsg("parsed data enumeration\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 121:

/* Line 936 of glr.c  */
#line 1147 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeDataAppl(gsMakeId(mcrl2::data::sort_bag::bag_enumeration_name()), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.list))));
      gsDebugMsg("parsed data enumeration\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 122:

/* Line 936 of glr.c  */
#line 1156 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list));
      gsDebugMsg("parsed bag enumeration elements\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 123:

/* Line 936 of glr.c  */
#line 1161 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATconcat((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed bag enumeration elements\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 124:

/* Line 936 of glr.c  */
#line 1170 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList2((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed bag enumeration element\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 125:

/* Line 936 of glr.c  */
#line 1179 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBinder(gsMakeSetBagComp(), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.appl)), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed data comprehension\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 126:

/* Line 936 of glr.c  */
#line 1188 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeDataVarId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed data variable declaration\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 127:

/* Line 936 of glr.c  */
#line 1200 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsDataSpecEltsToSpec(ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list))));
      gsDebugMsg("parsed data specification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 128:

/* Line 936 of glr.c  */
#line 1209 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed data specification elements\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 129:

/* Line 936 of glr.c  */
#line 1214 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed data specification elements\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 130:

/* Line 936 of glr.c  */
#line 1223 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed data specification element\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 131:

/* Line 936 of glr.c  */
#line 1228 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed data specification element\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 132:

/* Line 936 of glr.c  */
#line 1233 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed data specification element\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 133:

/* Line 936 of glr.c  */
#line 1238 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed data specification element\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 134:

/* Line 936 of glr.c  */
#line 1247 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSortSpec((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed sort specification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 135:

/* Line 936 of glr.c  */
#line 1256 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.list));
      gsDebugMsg("parsed sort declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 136:

/* Line 936 of glr.c  */
#line 1261 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATconcat((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed sort declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 137:

/* Line 936 of glr.c  */
#line 1270 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList0());
      int n = ATgetLength((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list));
      for (int i = 0; i < n; i++) {
        safe_assign(((*yyvalp).list), ATinsert(((*yyvalp).list), (ATerm) gsMakeSortId(ATAelementAt((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list), i))));
      }
      gsDebugMsg("parsed standard sort declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 138:

/* Line 936 of glr.c  */
#line 1279 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) gsMakeSortRef((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl))));
      gsDebugMsg("parsed reference sort declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 139:

/* Line 936 of glr.c  */
#line 1288 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed id's\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 140:

/* Line 936 of glr.c  */
#line 1293 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed id's\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 141:

/* Line 936 of glr.c  */
#line 1302 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list));
      gsDebugMsg("parsed domain\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 142:

/* Line 936 of glr.c  */
#line 1307 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) gsMakeSortArrow((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl))));
      gsDebugMsg("parsed domain\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 143:

/* Line 936 of glr.c  */
#line 1316 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeConsSpec((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed constructor operation specification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 144:

/* Line 936 of glr.c  */
#line 1325 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeMapSpec((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed operation specification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 145:

/* Line 936 of glr.c  */
#line 1335 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.list));
      gsDebugMsg("parsed operation declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 146:

/* Line 936 of glr.c  */
#line 1340 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATconcat((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed operation declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 147:

/* Line 936 of glr.c  */
#line 1349 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList0());
      int n = ATgetLength((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list));
      for (int i = 0; i < n; i++) {
        safe_assign(((*yyvalp).list), ATinsert(((*yyvalp).list), (ATerm) gsMakeOpId(ATAelementAt((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), i), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl))));
      }
      gsDebugMsg("parsed operation declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 148:

/* Line 936 of glr.c  */
#line 1362 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeDataEqnSpec((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed data equation specification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 149:

/* Line 936 of glr.c  */
#line 1371 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed data equation section\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 150:

/* Line 936 of glr.c  */
#line 1376 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList0());
      int n = ATgetLength((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.list));
      for (int i = 0; i < n; i++) {
        ATermAppl DataEqn = ATAelementAt((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.list), i);
        safe_assign(((*yyvalp).list),
          ATinsert(((*yyvalp).list), (ATerm) gsMakeDataEqn((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), ATAgetArgument(DataEqn, 1),
            ATAgetArgument(DataEqn, 2), ATAgetArgument(DataEqn, 3))));
      }
      gsDebugMsg("parsed data equation section\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 151:

/* Line 936 of glr.c  */
#line 1392 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed data equation declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 152:

/* Line 936 of glr.c  */
#line 1397 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed equation declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 153:

/* Line 936 of glr.c  */
#line 1406 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeDataEqn(ATmakeList0(), mcrl2::data::sort_bool::true_(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed data equation declaration\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 154:

/* Line 936 of glr.c  */
#line 1411 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeDataEqn(ATmakeList0(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed data equation declaration\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 155:

/* Line 936 of glr.c  */
#line 1421 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.list));
      gsDebugMsg("parsed data variable declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 156:

/* Line 936 of glr.c  */
#line 1426 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATconcat((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed data variable declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 157:

/* Line 936 of glr.c  */
#line 1438 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeMultAct(ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list))));
      gsDebugMsg("parsed multi-action\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 158:

/* Line 936 of glr.c  */
#line 1443 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeMultAct(ATmakeList0()));
      gsDebugMsg("parsed multi-action\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 159:

/* Line 936 of glr.c  */
#line 1452 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed parameterised id's\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 160:

/* Line 936 of glr.c  */
#line 1457 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed parameterised id's\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 161:

/* Line 936 of glr.c  */
#line 1466 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeParamId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl), ATmakeList0()));
      gsDebugMsg("parsed action or process\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 162:

/* Line 936 of glr.c  */
#line 1471 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeParamId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.appl), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.list))));
      gsDebugMsg("parsed action or process\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 163:

/* Line 936 of glr.c  */
#line 1483 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed process expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 164:

/* Line 936 of glr.c  */
#line 1492 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 165:

/* Line 936 of glr.c  */
#line 1496 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeChoice((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed choice expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 166:

/* Line 936 of glr.c  */
#line 1505 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 167:

/* Line 936 of glr.c  */
#line 1509 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSum((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed summation\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 168:

/* Line 936 of glr.c  */
#line 1518 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 169:

/* Line 936 of glr.c  */
#line 1522 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeMerge((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed merge expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 170:

/* Line 936 of glr.c  */
#line 1527 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeLMerge((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed left merge expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 171:

/* Line 936 of glr.c  */
#line 1536 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 172:

/* Line 936 of glr.c  */
#line 1540 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSum((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed summation\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 173:

/* Line 936 of glr.c  */
#line 1549 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 174:

/* Line 936 of glr.c  */
#line 1553 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBInit((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed bounded initialisation expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 175:

/* Line 936 of glr.c  */
#line 1562 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 176:

/* Line 936 of glr.c  */
#line 1566 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSum((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed summation\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 177:

/* Line 936 of glr.c  */
#line 1575 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 178:

/* Line 936 of glr.c  */
#line 1579 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeIfThen((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed conditional expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 179:

/* Line 936 of glr.c  */
#line 1584 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeIfThenElse((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed conditional expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 180:

/* Line 936 of glr.c  */
#line 1593 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 181:

/* Line 936 of glr.c  */
#line 1597 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSum((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed summation\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 182:

/* Line 936 of glr.c  */
#line 1606 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 183:

/* Line 936 of glr.c  */
#line 1610 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSeq((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed sequential expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 184:

/* Line 936 of glr.c  */
#line 1619 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 185:

/* Line 936 of glr.c  */
#line 1623 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSeq((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed sequential expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 186:

/* Line 936 of glr.c  */
#line 1632 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 187:

/* Line 936 of glr.c  */
#line 1636 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSum((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed summation\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 188:

/* Line 936 of glr.c  */
#line 1641 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeIfThen((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed conditional expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 189:

/* Line 936 of glr.c  */
#line 1646 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeIfThenElse((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed conditional expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 190:

/* Line 936 of glr.c  */
#line 1655 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 191:

/* Line 936 of glr.c  */
#line 1659 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSum((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed summation\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 192:

/* Line 936 of glr.c  */
#line 1668 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 193:

/* Line 936 of glr.c  */
#line 1672 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeAtTime((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed at time expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 194:

/* Line 936 of glr.c  */
#line 1681 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 195:

/* Line 936 of glr.c  */
#line 1685 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeAtTime((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed at time expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 196:

/* Line 936 of glr.c  */
#line 1694 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 197:

/* Line 936 of glr.c  */
#line 1698 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSync((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed sync expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 198:

/* Line 936 of glr.c  */
#line 1707 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 199:

/* Line 936 of glr.c  */
#line 1711 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSync((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed sync expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 200:

/* Line 936 of glr.c  */
#line 1720 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 201:

/* Line 936 of glr.c  */
#line 1724 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSum((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed summation\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 202:

/* Line 936 of glr.c  */
#line 1729 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeIfThen((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed conditional expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 203:

/* Line 936 of glr.c  */
#line 1734 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeIfThenElse((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed conditional expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 204:

/* Line 936 of glr.c  */
#line 1743 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 205:

/* Line 936 of glr.c  */
#line 1747 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeSum((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed summation\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 206:

/* Line 936 of glr.c  */
#line 1756 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 207:

/* Line 936 of glr.c  */
#line 1760 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 208:

/* Line 936 of glr.c  */
#line 1764 "mcrl2parser.yy"
    {
      //mcrl2yyerror("process assignments are not yet supported");
      // YYABORT
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 209:

/* Line 936 of glr.c  */
#line 1770 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 210:

/* Line 936 of glr.c  */
#line 1774 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 211:

/* Line 936 of glr.c  */
#line 1782 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeDelta());
      gsDebugMsg("parsed process constant\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 212:

/* Line 936 of glr.c  */
#line 1787 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeTau());
      gsDebugMsg("parsed process constant\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 213:

/* Line 936 of glr.c  */
#line 1796 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeIdAssignment((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), ATmakeList0()));
      gsDebugMsg("parsed identifier assignment\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 214:

/* Line 936 of glr.c  */
#line 1801 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeIdAssignment((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.appl), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.list))));
      gsDebugMsg("parsed identifier assignment\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 215:

/* Line 936 of glr.c  */
#line 1810 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeBlock((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (6))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (6))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed process quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 216:

/* Line 936 of glr.c  */
#line 1815 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeHide((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (6))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (6))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed process quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 217:

/* Line 936 of glr.c  */
#line 1820 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeRename((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (6))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (6))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed process quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 218:

/* Line 936 of glr.c  */
#line 1825 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeComm((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (6))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (6))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed process quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 219:

/* Line 936 of glr.c  */
#line 1830 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeAllow((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (6))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (6))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed process quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 220:

/* Line 936 of glr.c  */
#line 1839 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList0());
      gsDebugMsg("parsed action name set\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 221:

/* Line 936 of glr.c  */
#line 1844 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed action name set\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 222:

/* Line 936 of glr.c  */
#line 1853 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList0());
      gsDebugMsg("parsed renaming expression set\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 223:

/* Line 936 of glr.c  */
#line 1858 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed renaming expression set\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 224:

/* Line 936 of glr.c  */
#line 1867 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed renaming expressions\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 225:

/* Line 936 of glr.c  */
#line 1872 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed renaming expressions\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 226:

/* Line 936 of glr.c  */
#line 1881 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeRenameExpr((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed renaming expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 227:

/* Line 936 of glr.c  */
#line 1890 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList0());
      gsDebugMsg("parsed communication expression set\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 228:

/* Line 936 of glr.c  */
#line 1895 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed communication expression set\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 229:

/* Line 936 of glr.c  */
#line 1904 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed communication expressions\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 230:

/* Line 936 of glr.c  */
#line 1909 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed communication expressions\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 231:

/* Line 936 of glr.c  */
#line 1918 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeCommExpr((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl), gsMakeNil()));
      gsDebugMsg("parsed communication expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 232:

/* Line 936 of glr.c  */
#line 1923 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeCommExpr((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), gsMakeNil()));
      gsDebugMsg("parsed communication expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 233:

/* Line 936 of glr.c  */
#line 1928 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeCommExpr((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed communication expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 234:

/* Line 936 of glr.c  */
#line 1937 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeMultActName(ATinsert(ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.list)), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl))));
      gsDebugMsg("parsed left-hand side of communication expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 235:

/* Line 936 of glr.c  */
#line 1946 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed id's\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 236:

/* Line 936 of glr.c  */
#line 1951 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed id's\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 237:

/* Line 936 of glr.c  */
#line 1960 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList0());
      gsDebugMsg("parsed multi action name set\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 238:

/* Line 936 of glr.c  */
#line 1965 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed multi action name set\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 239:

/* Line 936 of glr.c  */
#line 1974 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed multi action names\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 240:

/* Line 936 of glr.c  */
#line 1979 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed multi action names\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 241:

/* Line 936 of glr.c  */
#line 1988 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeMultActName(ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list))));
      gsDebugMsg("parsed multi action name\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 242:

/* Line 936 of glr.c  */
#line 2000 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsProcSpecEltsToSpec(ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list))));
      gsDebugMsg("parsed process specification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 243:

/* Line 936 of glr.c  */
#line 2009 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed process specification elements\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 244:

/* Line 936 of glr.c  */
#line 2014 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed process specification elements\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 245:

/* Line 936 of glr.c  */
#line 2023 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed process specification element\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 246:

/* Line 936 of glr.c  */
#line 2028 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed process specification element\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 247:

/* Line 936 of glr.c  */
#line 2033 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed process specification element\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 248:

/* Line 936 of glr.c  */
#line 2038 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed process specification element\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 249:

/* Line 936 of glr.c  */
#line 2043 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed process specification element\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 250:

/* Line 936 of glr.c  */
#line 2052 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActSpec((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed action specification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 251:

/* Line 936 of glr.c  */
#line 2061 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.list));
      gsDebugMsg("parsed action declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 252:

/* Line 936 of glr.c  */
#line 2066 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATconcat((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed action declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 253:

/* Line 936 of glr.c  */
#line 2075 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList0());
      int n = ATgetLength((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list));
      for (int i = 0; i < n; i++) {
        safe_assign(((*yyvalp).list),
          ATinsert(((*yyvalp).list), (ATerm) gsMakeActId(ATAelementAt((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list), i), ATmakeList0())));
      }
      gsDebugMsg("parsed action declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 254:

/* Line 936 of glr.c  */
#line 2085 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList0());
      int n = ATgetLength((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list));
      for (int i = 0; i < n; i++) {
        safe_assign(((*yyvalp).list), ATinsert(((*yyvalp).list), (ATerm) gsMakeActId(ATAelementAt((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), i), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.list))));
      }
      gsDebugMsg("parsed action declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 255:

/* Line 936 of glr.c  */
#line 2098 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeGlobVarSpec((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed global variables\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 256:

/* Line 936 of glr.c  */
#line 2107 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeProcEqnSpec(ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.list))));
      gsDebugMsg("parsed process equation specification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 257:

/* Line 936 of glr.c  */
#line 2116 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed process equation declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 258:

/* Line 936 of glr.c  */
#line 2121 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed process equation declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 259:

/* Line 936 of glr.c  */
#line 2130 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeProcEqn(
        gsMakeProcVarId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), ATmakeList0()), ATmakeList0(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed process equation declaration\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 260:

/* Line 936 of glr.c  */
#line 2136 "mcrl2parser.yy"
    {
      ATermList SortExprs = ATmakeList0();
      int n = ATgetLength((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (6))].yystate.yysemantics.yysval.list));
      for (int i = 0; i < n; i++) {
        SortExprs = ATinsert(SortExprs, ATgetArgument(ATAelementAt((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (6))].yystate.yysemantics.yysval.list), i), 1));
      }
      safe_assign(((*yyvalp).appl), gsMakeProcEqn(
        gsMakeProcVarId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (6))].yystate.yysemantics.yysval.appl), ATreverse(SortExprs)), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (6))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((6) - (6))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed process equation declaration\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 261:

/* Line 936 of glr.c  */
#line 2151 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeProcessInit((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed initialisation\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 262:

/* Line 936 of glr.c  */
#line 2163 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed state formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 263:

/* Line 936 of glr.c  */
#line 2172 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 264:

/* Line 936 of glr.c  */
#line 2176 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateForall((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 265:

/* Line 936 of glr.c  */
#line 2181 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateExists((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 266:

/* Line 936 of glr.c  */
#line 2186 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateNu((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 267:

/* Line 936 of glr.c  */
#line 2191 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateMu((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 268:

/* Line 936 of glr.c  */
#line 2200 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList0());
      gsDebugMsg("parsed fixpoint parameters\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 269:

/* Line 936 of glr.c  */
#line 2205 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed fixpoint parameters\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 270:

/* Line 936 of glr.c  */
#line 2215 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed data variable declaration and initialisations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 271:

/* Line 936 of glr.c  */
#line 2220 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed data variable declaration and initialisations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 272:

/* Line 936 of glr.c  */
#line 2229 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeDataVarIdInit(gsMakeDataVarId((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.appl)), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed data variable declaration and initialisation\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 273:

/* Line 936 of glr.c  */
#line 2238 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 274:

/* Line 936 of glr.c  */
#line 2242 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateImp((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed implication\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 275:

/* Line 936 of glr.c  */
#line 2251 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 276:

/* Line 936 of glr.c  */
#line 2255 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateForall((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 277:

/* Line 936 of glr.c  */
#line 2260 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateExists((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 278:

/* Line 936 of glr.c  */
#line 2265 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateNu((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 279:

/* Line 936 of glr.c  */
#line 2270 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateMu((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 280:

/* Line 936 of glr.c  */
#line 2279 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 281:

/* Line 936 of glr.c  */
#line 2283 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateAnd((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed conjunction\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 282:

/* Line 936 of glr.c  */
#line 2288 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateOr((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed disjunction\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 283:

/* Line 936 of glr.c  */
#line 2297 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 284:

/* Line 936 of glr.c  */
#line 2301 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateForall((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 285:

/* Line 936 of glr.c  */
#line 2306 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateExists((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 286:

/* Line 936 of glr.c  */
#line 2311 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateNu((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 287:

/* Line 936 of glr.c  */
#line 2316 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateMu((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 288:

/* Line 936 of glr.c  */
#line 2325 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 289:

/* Line 936 of glr.c  */
#line 2329 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateNot((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed prefix state formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 290:

/* Line 936 of glr.c  */
#line 2334 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateMust((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed prefix state formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 291:

/* Line 936 of glr.c  */
#line 2339 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateMay((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed prefix state formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 292:

/* Line 936 of glr.c  */
#line 2344 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateYaledTimed((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed prefix state formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 293:

/* Line 936 of glr.c  */
#line 2349 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateDelayTimed((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed prefix state formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 294:

/* Line 936 of glr.c  */
#line 2358 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 295:

/* Line 936 of glr.c  */
#line 2362 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateForall((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 296:

/* Line 936 of glr.c  */
#line 2367 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateExists((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 297:

/* Line 936 of glr.c  */
#line 2372 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateNu((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 298:

/* Line 936 of glr.c  */
#line 2377 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateMu((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 299:

/* Line 936 of glr.c  */
#line 2386 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed primary state formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 300:

/* Line 936 of glr.c  */
#line 2391 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateVar(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl), 0), ATLgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl), 1)));
      gsDebugMsg("parsed primary state formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 301:

/* Line 936 of glr.c  */
#line 2396 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateTrue());
      gsDebugMsg("parsed primary state formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 302:

/* Line 936 of glr.c  */
#line 2401 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateFalse());
      gsDebugMsg("parsed primary state formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 303:

/* Line 936 of glr.c  */
#line 2406 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateYaled());
      gsDebugMsg("parsed primary state formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 304:

/* Line 936 of glr.c  */
#line 2411 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeStateDelay());
      gsDebugMsg("parsed primary state formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 305:

/* Line 936 of glr.c  */
#line 2416 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 306:

/* Line 936 of glr.c  */
#line 2424 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed regular formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 307:

/* Line 936 of glr.c  */
#line 2429 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed regular formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 308:

/* Line 936 of glr.c  */
#line 2438 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 309:

/* Line 936 of glr.c  */
#line 2442 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeRegAlt((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed alternative composition\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 310:

/* Line 936 of glr.c  */
#line 2451 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 311:

/* Line 936 of glr.c  */
#line 2455 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeRegAlt((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed alternative composition\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 312:

/* Line 936 of glr.c  */
#line 2464 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 313:

/* Line 936 of glr.c  */
#line 2468 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeRegSeq((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed sequential composition\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 314:

/* Line 936 of glr.c  */
#line 2477 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 315:

/* Line 936 of glr.c  */
#line 2481 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeRegSeq((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed sequential composition\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 316:

/* Line 936 of glr.c  */
#line 2490 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 317:

/* Line 936 of glr.c  */
#line 2494 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeRegTransOrNil((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed postfix regular formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 318:

/* Line 936 of glr.c  */
#line 2499 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeRegTrans((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed postfix regular formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 319:

/* Line 936 of glr.c  */
#line 2508 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 320:

/* Line 936 of glr.c  */
#line 2512 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeRegTransOrNil((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed postfix regular formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 321:

/* Line 936 of glr.c  */
#line 2517 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeRegTrans((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed postfix regular formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 322:

/* Line 936 of glr.c  */
#line 2526 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeRegNil());
      gsDebugMsg("parsed primary regular formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 323:

/* Line 936 of glr.c  */
#line 2531 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 324:

/* Line 936 of glr.c  */
#line 2539 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed regular formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 325:

/* Line 936 of glr.c  */
#line 2544 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeRegNil());
      gsDebugMsg("parsed primary regular formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 326:

/* Line 936 of glr.c  */
#line 2549 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 327:

/* Line 936 of glr.c  */
#line 2557 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed action formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 328:

/* Line 936 of glr.c  */
#line 2566 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 329:

/* Line 936 of glr.c  */
#line 2570 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActForall((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 330:

/* Line 936 of glr.c  */
#line 2575 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActExists((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 331:

/* Line 936 of glr.c  */
#line 2584 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 332:

/* Line 936 of glr.c  */
#line 2588 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActImp((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed implication\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 333:

/* Line 936 of glr.c  */
#line 2597 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 334:

/* Line 936 of glr.c  */
#line 2601 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActForall((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 335:

/* Line 936 of glr.c  */
#line 2606 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActExists((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 336:

/* Line 936 of glr.c  */
#line 2615 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 337:

/* Line 936 of glr.c  */
#line 2619 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActAnd((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed conjunction\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 338:

/* Line 936 of glr.c  */
#line 2624 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActOr((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed disjunction\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 339:

/* Line 936 of glr.c  */
#line 2633 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 340:

/* Line 936 of glr.c  */
#line 2637 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActForall((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 341:

/* Line 936 of glr.c  */
#line 2642 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActExists((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 342:

/* Line 936 of glr.c  */
#line 2651 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 343:

/* Line 936 of glr.c  */
#line 2655 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActAt((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed conjunction\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 344:

/* Line 936 of glr.c  */
#line 2664 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 345:

/* Line 936 of glr.c  */
#line 2668 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActNot((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed prefix action formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 346:

/* Line 936 of glr.c  */
#line 2677 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 347:

/* Line 936 of glr.c  */
#line 2681 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActForall((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 348:

/* Line 936 of glr.c  */
#line 2686 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActExists((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 349:

/* Line 936 of glr.c  */
#line 2695 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed primary action formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 350:

/* Line 936 of glr.c  */
#line 2700 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed primary action formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 351:

/* Line 936 of glr.c  */
#line 2705 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActTrue());
      gsDebugMsg("parsed primary action formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 352:

/* Line 936 of glr.c  */
#line 2710 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActFalse());
      gsDebugMsg("parsed primary action formula\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 353:

/* Line 936 of glr.c  */
#line 2715 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 354:

/* Line 936 of glr.c  */
#line 2726 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsActionRenameEltsToActionRename(ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list))));
      gsDebugMsg("parsed action rename specification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 355:

/* Line 936 of glr.c  */
#line 2735 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed action rename specification element\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 356:

/* Line 936 of glr.c  */
#line 2740 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed action rename specification element\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 357:

/* Line 936 of glr.c  */
#line 2749 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed action rename specification element\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 358:

/* Line 936 of glr.c  */
#line 2754 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed action rename specification element\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 359:

/* Line 936 of glr.c  */
#line 2759 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed action rename specification element\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 360:

/* Line 936 of glr.c  */
#line 2768 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActionRenameRules((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list)));
      gsDebugMsg("parsed action rename specification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 361:

/* Line 936 of glr.c  */
#line 2777 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.list));
      gsDebugMsg("parsed action rename rule section\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 362:

/* Line 936 of glr.c  */
#line 2782 "mcrl2parser.yy"
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
      gsDebugMsg("parsed action rename rule section\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 363:

/* Line 936 of glr.c  */
#line 2800 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed action rename rules\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 364:

/* Line 936 of glr.c  */
#line 2805 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed action rename rules\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 365:

/* Line 936 of glr.c  */
#line 2814 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActionRenameRule(ATmakeList0(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed action rename rule\n %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 366:

/* Line 936 of glr.c  */
#line 2819 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeActionRenameRule(ATmakeList0(), mcrl2::data::sort_bool::true_(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed action rename rule\n %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 367:

/* Line 936 of glr.c  */
#line 2828 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed right-hand-side of an action rename rule\n %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 368:

/* Line 936 of glr.c  */
#line 2833 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed right-hand-side of an action rename rule\n %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 369:

/* Line 936 of glr.c  */
#line 2845 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed parameterised boolean expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 370:

/* Line 936 of glr.c  */
#line 2854 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 371:

/* Line 936 of glr.c  */
#line 2858 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakePBESForall((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 372:

/* Line 936 of glr.c  */
#line 2863 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakePBESExists((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 373:

/* Line 936 of glr.c  */
#line 2872 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 374:

/* Line 936 of glr.c  */
#line 2876 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakePBESImp((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed implication\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 375:

/* Line 936 of glr.c  */
#line 2885 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 376:

/* Line 936 of glr.c  */
#line 2889 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakePBESForall((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 377:

/* Line 936 of glr.c  */
#line 2894 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakePBESExists((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 378:

/* Line 936 of glr.c  */
#line 2903 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 379:

/* Line 936 of glr.c  */
#line 2907 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakePBESAnd((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed conjunction\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 380:

/* Line 936 of glr.c  */
#line 2912 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakePBESOr((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed disjunction\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 381:

/* Line 936 of glr.c  */
#line 2921 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 382:

/* Line 936 of glr.c  */
#line 2925 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakePBESForall((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 383:

/* Line 936 of glr.c  */
#line 2930 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakePBESExists((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 384:

/* Line 936 of glr.c  */
#line 2939 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 385:

/* Line 936 of glr.c  */
#line 2943 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakePBESNot((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed negation\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 386:

/* Line 936 of glr.c  */
#line 2952 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 387:

/* Line 936 of glr.c  */
#line 2956 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakePBESForall((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 388:

/* Line 936 of glr.c  */
#line 2961 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakePBESExists((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed quantification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 389:

/* Line 936 of glr.c  */
#line 2970 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed primary expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 390:

/* Line 936 of glr.c  */
#line 2975 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakePropVarInst(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl), 0), ATLgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl), 1)));
      gsDebugMsg("parsed primary expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 391:

/* Line 936 of glr.c  */
#line 2980 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakePBESTrue());
      gsDebugMsg("parsed primary expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 392:

/* Line 936 of glr.c  */
#line 2985 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakePBESFalse());
      gsDebugMsg("parsed primary expression\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 393:

/* Line 936 of glr.c  */
#line 2990 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl));
    ;}
    break;

  case 394:

/* Line 936 of glr.c  */
#line 3002 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsPBESSpecEltsToSpec(ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list))));
      gsDebugMsg("parsed PBES specification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 395:

/* Line 936 of glr.c  */
#line 3011 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed PBES specification elements\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 396:

/* Line 936 of glr.c  */
#line 3016 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed PBES specification elements\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 397:

/* Line 936 of glr.c  */
#line 3025 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
     gsDebugMsg("parsed PBES specification element\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 398:

/* Line 936 of glr.c  */
#line 3030 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed PBES specification element\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 399:

/* Line 936 of glr.c  */
#line 3035 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed PBES specification element\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 400:

/* Line 936 of glr.c  */
#line 3040 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl));
      gsDebugMsg("parsed PBES specification element\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 401:

/* Line 936 of glr.c  */
#line 3049 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakePBEqnSpec(ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.list))));
      gsDebugMsg("parsed parameterised boolean equation specification\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 402:

/* Line 936 of glr.c  */
#line 3058 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed parameterised boolean equation declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 403:

/* Line 936 of glr.c  */
#line 3063 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed parameterised boolean equation declarations\n  %T\n", ((*yyvalp).list));
    ;}
    break;

  case 404:

/* Line 936 of glr.c  */
#line 3072 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl),
        gsMakePBEqn((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.appl), gsMakePropVarDecl((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.appl), ATmakeList0()), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed parameterised boolean equation declaration\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 405:

/* Line 936 of glr.c  */
#line 3078 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl),
        gsMakePBEqn((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (7))].yystate.yysemantics.yysval.appl), gsMakePropVarDecl((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (7))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (7))].yystate.yysemantics.yysval.list)), (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (7))].yystate.yysemantics.yysval.appl)));
      gsDebugMsg("parsed parameterised boolean equation declaration\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 406:

/* Line 936 of glr.c  */
#line 3088 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeMu());
      gsDebugMsg("parsed fixpoint\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 407:

/* Line 936 of glr.c  */
#line 3093 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl), gsMakeNu());
      gsDebugMsg("parsed fixpoint\n  %T\n", ((*yyvalp).appl));
    ;}
    break;

  case 408:

/* Line 936 of glr.c  */
#line 3102 "mcrl2parser.yy"
    {
      safe_assign(((*yyvalp).appl),
        gsMakePBInit(gsMakePropVarInst(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl), 0), ATLgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl), 1))));
      gsDebugMsg("parsed initialisation\n  %T\n", ((*yyvalp).appl));
    ;}
    break;



/* Line 936 of glr.c  */
#line 6147 "mcrl2parser.cpp"
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
#line 3109 "mcrl2parser.yy"


//Uncomment the lines below to enable the use of SIZE_MAX
//#define __STDC_LIMIT_MACROS 1
//#include <stdint.h>

//Uncomment the line below to enable the use of std::cerr, std::cout and std::endl;
//#include <iostream>

ATermAppl gsDataSpecEltsToSpec(ATermList SpecElts)
{
  ATermAppl Result = NULL;
  ATermList SortDecls = ATmakeList0();
  ATermList ConsDecls = ATmakeList0();
  ATermList MapDecls = ATmakeList0();
  ATermList DataEqnDecls = ATmakeList0();
  int n = ATgetLength(SpecElts);
  for (int i = 0; i < n; i++) {
    ATermAppl SpecElt = ATAelementAt(SpecElts, i);
    ATermList SpecEltArg0 = ATLgetArgument(SpecElt, 0);
    if (gsIsSortSpec(SpecElt)) {
      SortDecls = ATconcat(SortDecls, SpecEltArg0);
    } else if (gsIsConsSpec(SpecElt)) {
      ConsDecls = ATconcat(ConsDecls, SpecEltArg0);
    } else if (gsIsMapSpec(SpecElt)) {
      MapDecls = ATconcat(MapDecls, SpecEltArg0);
    } else if (gsIsDataEqnSpec(SpecElt)) {
      DataEqnDecls = ATconcat(DataEqnDecls, SpecEltArg0);
    }
  }
  Result = gsMakeDataSpec(
    gsMakeSortSpec(SortDecls),
    gsMakeConsSpec(ConsDecls),
    gsMakeMapSpec(MapDecls),
    gsMakeDataEqnSpec(DataEqnDecls)
  );
  //Uncomment the lines below to check if the parser stack size isn't too big
  //std::cerr << "SIZE_MAX:              " << SIZE_MAX << std::endl;
  //std::cerr << "YYMAXDEPTH:            " << YYMAXDEPTH << std::endl;
  //std::cerr << "sizeof (yyGLRStackItem): " << sizeof (yyGLRStackItem) << std::endl;
  //std::cerr << "SIZE_MAX < YYMAXDEPTH * sizeof (yyGLRStackItem): " << (SIZE_MAX < YYMAXDEPTH * sizeof (yyGLRStackItem)) << std::endl;
  return Result;
}

ATermAppl gsProcSpecEltsToSpec(ATermList SpecElts)
{
  ATermAppl Result = NULL;
  ATermList SortDecls = ATmakeList0();
  ATermList ConsDecls = ATmakeList0();
  ATermList MapDecls = ATmakeList0();
  ATermList DataEqnDecls = ATmakeList0();
  ATermList GlobVars = ATmakeList0();
  ATermList ActDecls = ATmakeList0();
  ATermList ProcEqnDecls = ATmakeList0();
  ATermAppl ProcInit = NULL;
  int n = ATgetLength(SpecElts);
  for (int i = 0; i < n; i++) {
    ATermAppl SpecElt = ATAelementAt(SpecElts, i);
    if (gsIsProcessInit(SpecElt)) {
      if (ProcInit == NULL) {
        ProcInit = SpecElt;
      } else {
        //ProcInit != NULL
        gsErrorMsg("parse error: multiple initialisations\n");
        return NULL;
      }
    } else {
      ATermList SpecEltArg0 = ATLgetArgument(SpecElt, 0);
      if (gsIsGlobVarSpec(SpecElt)) {
        GlobVars = ATconcat(GlobVars, SpecEltArg0);
      } else if (gsIsSortSpec(SpecElt)) {
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
  if (ProcInit == NULL) {
    gsErrorMsg("parse error: missing initialisation\n");
    return NULL;
  }
  Result = gsMakeProcSpec(
    gsMakeDataSpec(
      gsMakeSortSpec(SortDecls),
      gsMakeConsSpec(ConsDecls),
      gsMakeMapSpec(MapDecls),
      gsMakeDataEqnSpec(DataEqnDecls)
    ),
    gsMakeActSpec(ActDecls),
    gsMakeGlobVarSpec(GlobVars),
    gsMakeProcEqnSpec(ProcEqnDecls),
    ProcInit
  );
  //Uncomment the lines below to check if the parser stack size isn't too big
  //std::cerr << "SIZE_MAX:              " << SIZE_MAX << std::endl;
  //std::cerr << "YYMAXDEPTH:            " << YYMAXDEPTH << std::endl;
  //std::cerr << "sizeof (yyGLRStackItem): " << sizeof (yyGLRStackItem) << std::endl;
  //std::cerr << "SIZE_MAX < YYMAXDEPTH * sizeof (yyGLRStackItem): " << (SIZE_MAX < YYMAXDEPTH * sizeof (yyGLRStackItem)) << std::endl;
  return Result;
}

ATermAppl gsPBESSpecEltsToSpec(ATermList SpecElts)
{
  ATermAppl Result = NULL;
  ATermList SortDecls = ATmakeList0();
  ATermList ConsDecls = ATmakeList0();
  ATermList MapDecls = ATmakeList0();
  ATermList DataEqnDecls = ATmakeList0();
  ATermList GlobVars = ATmakeList0();
  ATermAppl PBEqnSpec = NULL;
  ATermAppl PBInit = NULL;
  int n = ATgetLength(SpecElts);
  for (int i = 0; i < n; i++) {
    ATermAppl SpecElt = ATAelementAt(SpecElts, i);
    if (gsIsPBEqnSpec(SpecElt)) {
      if (PBEqnSpec == NULL) {
        PBEqnSpec = SpecElt;
      } else {
        //PBEqnSpec != NULL
        gsErrorMsg("parse error: multiple parameterised boolean equation specifications\n");
        return NULL;
      }
    } else if (gsIsPBInit(SpecElt)) {
      if (PBInit == NULL) {
        PBInit = SpecElt;
      } else {
        //PBInit != NULL
        gsErrorMsg("parse error: multiple initialisations\n");
        return NULL;
      }
    } else {
      ATermList SpecEltArg0 = ATLgetArgument(SpecElt, 0);
      if (gsIsGlobVarSpec(SpecElt)) {
        GlobVars = ATconcat(GlobVars, SpecEltArg0);
      } else if (gsIsSortSpec(SpecElt)) {
        SortDecls = ATconcat(SortDecls, SpecEltArg0);
      } else if (gsIsConsSpec(SpecElt)) {
        ConsDecls = ATconcat(ConsDecls, SpecEltArg0);
      } else if (gsIsMapSpec(SpecElt)) {
        MapDecls = ATconcat(MapDecls, SpecEltArg0);
      } else if (gsIsDataEqnSpec(SpecElt)) {
        DataEqnDecls = ATconcat(DataEqnDecls, SpecEltArg0);
      }
    }
  }
  //check whether a parameterised boolean equation specification is present
  if (PBEqnSpec == NULL) {
    gsErrorMsg("parse error: missing parameterised boolean equation specification\n");
    return NULL;
  }
  //check whether an initialisation is present
  if (PBInit == NULL) {
    gsErrorMsg("parse error: missing initialisation\n");
    return NULL;
  }
  Result = gsMakePBES(
    gsMakeDataSpec(
      gsMakeSortSpec(SortDecls),
      gsMakeConsSpec(ConsDecls),
      gsMakeMapSpec(MapDecls),
      gsMakeDataEqnSpec(DataEqnDecls)
    ),
    gsMakeGlobVarSpec(GlobVars),
    PBEqnSpec,
    PBInit
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
    } else if (gsIsActionRenameRules(ActionRenameElt)) {
      ActionRenameRules = ATconcat(ActionRenameRules, ActionRenameEltArg0);
    } else {
      assert(false);
    }
  }

  Result = gsMakeActionRenameSpec(
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

