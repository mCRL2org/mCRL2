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
#define yyparse chiyyparse
#define yylex   chiyylex
#define yyerror chiyyerror
#define yylval  chiyylval
#define yychar  chiyychar
#define yydebug chiyydebug
#define yynerrs chiyynerrs
#define yylloc  chiyylloc



#include "chiparser.hpp"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
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
#line 97 "chiparser.cpp"

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
#define YYFINAL  6
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1946

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  81
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  54
/* YYNRULES -- Number of rules.  */
#define YYNRULES  136
/* YYNRULES -- Number of states.  */
#define YYNSTATES  276
/* YYMAXRHS -- Maximum number of symbols on right-hand side of rule.  */
#define YYMAXRHS 7
/* YYMAXLEFT -- Maximum number of symbols to the left of a handle
   accessed by $0, $-1, etc., in any rule.  */
#define YYMAXLEFT 0

/* YYTRANSLATE(X) -- Bison symbol number corresponding to X.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   335

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
      75,    76,    77,    78,    79,    80
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short int yyprhs[] =
{
       0,     0,     3,     6,    13,    17,    23,    25,    27,    30,
      37,    45,    47,    51,    57,    59,    61,    63,    67,    70,
      73,    75,    79,    81,    85,    87,    91,    93,    97,   100,
     103,   107,   109,   113,   117,   123,   125,   129,   132,   135,
     137,   141,   145,   151,   153,   157,   159,   161,   165,   167,
     169,   171,   173,   177,   181,   187,   189,   193,   197,   199,
     201,   203,   205,   209,   211,   213,   215,   221,   225,   226,
     229,   230,   233,   235,   239,   241,   245,   250,   255,   259,
     264,   268,   272,   275,   279,   282,   286,   288,   290,   292,
     294,   296,   298,   302,   304,   306,   310,   312,   314,   317,
     320,   324,   328,   332,   334,   337,   340,   344,   348,   352,
     356,   360,   364,   368,   372,   376,   380,   384,   388,   392,
     396,   400,   402,   406,   410,   414,   416,   421,   426,   431,
     436,   441,   448,   455,   460,   465,   468
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const short int yyrhs[] =
{
      82,     0,    -1,    83,    86,    -1,     6,    91,    31,    32,
      24,    84,    -1,    19,   113,    85,    -1,    19,    92,    21,
     113,    85,    -1,    20,    -1,    87,    -1,    86,    87,    -1,
      88,    91,    31,    32,    24,    89,    -1,    88,    91,    31,
      97,    32,    24,    89,    -1,     3,    -1,    19,   112,    90,
      -1,    19,    92,    21,   112,    90,    -1,    20,    -1,    17,
      -1,    93,    -1,    92,    23,    93,    -1,     7,    94,    -1,
       9,   104,    -1,    95,    -1,    94,    23,    95,    -1,    99,
      -1,    99,    24,   125,    -1,    99,    -1,    96,    23,    99,
      -1,    98,    -1,    97,    23,    98,    -1,     7,    96,    -1,
       9,   100,    -1,   118,    13,   108,    -1,   101,    -1,   100,
      23,   101,    -1,   102,    13,   108,    -1,   102,    13,   125,
      79,   108,    -1,   103,    -1,   102,    23,   103,    -1,    91,
      54,    -1,    91,    55,    -1,   105,    -1,   104,    23,   105,
      -1,   106,    13,   108,    -1,   106,    13,   125,    79,   108,
      -1,   107,    -1,   106,    23,   107,    -1,    91,    -1,   109,
      -1,    31,   108,    32,    -1,   110,    -1,    15,    -1,    16,
      -1,    14,    -1,    63,   108,    64,    -1,    29,   108,    30,
      -1,    31,   111,    23,   108,    32,    -1,   108,    -1,   111,
      23,   108,    -1,    31,   112,    32,    -1,   114,    -1,   123,
      -1,   122,    -1,   121,    -1,   113,    11,   113,    -1,   115,
      -1,   120,    -1,   124,    -1,   116,   117,   126,    25,   119,
      -1,   116,   117,    10,    -1,    -1,   125,    35,    -1,    -1,
     125,    13,    -1,    91,    -1,   118,    23,    91,    -1,   125,
      -1,   119,    23,   125,    -1,   116,   125,    55,   119,    -1,
     116,   125,    54,   119,    -1,    91,    31,    32,    -1,    91,
      31,   119,    32,    -1,   112,    22,   112,    -1,   112,    12,
     112,    -1,    60,   112,    -1,   125,    61,   112,    -1,   116,
      44,    -1,    31,   125,    32,    -1,   128,    -1,   129,    -1,
     130,    -1,   131,    -1,   132,    -1,   127,    -1,   126,    23,
     127,    -1,    91,    -1,    91,    -1,    91,    43,   125,    -1,
      42,    -1,    41,    -1,    36,   125,    -1,    55,   125,    -1,
     125,    33,   125,    -1,   125,    34,   125,    -1,   125,    45,
     125,    -1,    38,    -1,    27,   125,    -1,    26,   125,    -1,
     125,    53,   125,    -1,   125,    60,   125,    -1,   125,    80,
     125,    -1,   125,    27,   125,    -1,   125,    26,   125,    -1,
     125,    52,   125,    -1,   125,    51,   125,    -1,   125,    50,
     125,    -1,   125,    49,   125,    -1,   125,    77,   125,    -1,
     125,    78,   125,    -1,   125,    48,   125,    -1,   125,    47,
     125,    -1,   125,    24,   125,    -1,   125,    46,   125,    -1,
     134,    -1,   125,    67,   125,    -1,   125,    66,   125,    -1,
     125,    65,   125,    -1,   133,    -1,    72,    31,   125,    32,
      -1,    68,    31,   125,    32,    -1,    69,    31,   125,    32,
      -1,    70,    31,   125,    32,    -1,    71,    31,   125,    32,
      -1,    73,    31,   125,    23,   125,    32,    -1,    74,    31,
     125,    23,   125,    32,    -1,    75,    31,   125,    32,    -1,
      76,    31,   125,    32,    -1,    63,    64,    -1,    63,   119,
      64,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,   128,   128,   138,   146,   150,   157,   164,   168,   175,
     192,   208,   216,   220,   227,   237,   244,   248,   255,   260,
     269,   273,   290,   295,   315,   319,   336,   340,   347,   352,
     360,   409,   413,   429,   469,   514,   518,   525,   530,   548,
     552,   559,   598,   643,   647,   654,   673,   674,   680,   686,
     691,   696,   706,   712,   718,   727,   732,   745,   749,   750,
     751,   755,   756,   763,   764,   769,   773,   793,   801,   803,
     822,   824,   833,   837,   844,   848,   855,   873,   894,   898,
     905,   909,   920,   926,   936,   949,   964,   965,   966,   967,
     968,   980,   984,   990,  1017,  1061,  1103,  1112,  1121,  1132,
    1143,  1156,  1169,  1185,  1194,  1206,  1218,  1231,  1244,  1257,
    1270,  1283,  1296,  1309,  1322,  1335,  1348,  1361,  1374,  1390,
    1408,  1430,  1434,  1453,  1472,  1495,  1499,  1512,  1525,  1539,
    1553,  1567,  1589,  1612,  1626,  1634,  1638
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "PROC", "MODEL_DEF", "ENUM", "MODEL",
  "VAR", "CONST", "CHAN", "SKIP", "BARS", "ALT", "COLON", "TYPE", "BOOL",
  "NAT", "ID", "TIME", "BP", "EP", "PROC_SEP", "SEP", "COMMA", "IS",
  "ASSIGNMENT", "MINUS", "PLUS", "GG", "LBRACE", "RBRACE", "LBRACKET",
  "RBRACKET", "AND", "OR", "GUARD", "NOT", "OLD", "NUMBER", "INT",
  "REALNUMBER", "TRUE", "FALSE", "DOT", "DEADLOCK", "IMPLIES", "NOTEQUAL",
  "GEQ", "LEQ", "MAX", "MIN", "DIV", "MOD", "POWER", "RECV", "EXCLAMATION",
  "SENDRECV", "RECVSEND", "SSEND", "RRECV", "STAR", "GUARD_REP",
  "DERIVATIVE", "SQLBRACKET", "SQRBRACKET", "LSUBTRACT", "CONCAT", "IN",
  "HEAD", "TAIL", "RHEAD", "RTAIL", "LENGTH", "TAKE", "DROP", "SORT",
  "INSERT", "LESS", "GREATER", "HASH", "DIVIDE", "$accept", "ChiProgram",
  "ModelDefinition", "ModelBody", "ModelCloseScope", "ProcessDefinitions",
  "ProcessDefinition", "ProcOpenScope", "ProcessBody", "ProcCloseScope",
  "Identifier", "LocalVariables_csp", "LocalVariables",
  "IdentifierTypeExpression_csp", "IdentifierTypeExpression",
  "IdentifierType_csp", "FormalParameter_csp", "FormalParameter",
  "IdentifierType", "ChannelDeclaration_csp", "ChannelDeclaration",
  "IdentifierChannelDeclaration_csp", "IdentifierChannelDeclaration",
  "ChannelDefinition_csp", "ChannelDefinition",
  "IdentifierChannelDefinition_csp", "IdentifierChannelDefinition", "Type",
  "BasicType", "ContainerType", "Type_csp", "Statement", "ModelStatement",
  "BasicStatement", "AssignmentStatement", "OptGuard", "OptChannel",
  "Identifier_csp", "Expression_csp", "CommStatement", "Instantiation",
  "BinaryStatement", "UnaryStatement", "AdvancedStatement", "Expression",
  "ExpressionIdentier_csp", "ExpressionIdentifier", "BasicExpression",
  "BooleanExpression", "NatIntExpression", "BoolNatIntExpression",
  "ListExpression", "Functions", "ListLiteral", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    81,    82,    83,    84,    84,    85,    86,    86,    87,
      87,    88,    89,    89,    90,    91,    92,    92,    93,    93,
      94,    94,    95,    95,    96,    96,    97,    97,    98,    98,
      99,   100,   100,   101,   101,   102,   102,   103,   103,   104,
     104,   105,   105,   106,   106,   107,   108,   108,   108,   109,
     109,   109,   110,   110,   110,   111,   111,   112,   112,   112,
     112,   113,   113,   114,   114,   114,   115,   115,   116,   116,
     117,   117,   118,   118,   119,   119,   120,   120,   121,   121,
     122,   122,   123,   123,   124,   125,   125,   125,   125,   125,
     125,   126,   126,   127,   128,   128,   129,   129,   129,   129,
     129,   129,   129,   130,   130,   130,   130,   130,   130,   130,
     130,   130,   130,   130,   130,   130,   130,   130,   130,   131,
     131,   132,   132,   132,   132,   132,   133,   133,   133,   133,
     133,   133,   133,   133,   133,   134,   134
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     2,     6,     3,     5,     1,     1,     2,     6,
       7,     1,     3,     5,     1,     1,     1,     3,     2,     2,
       1,     3,     1,     3,     1,     3,     1,     3,     2,     2,
       3,     1,     3,     3,     5,     1,     3,     2,     2,     1,
       3,     3,     5,     1,     3,     1,     1,     3,     1,     1,
       1,     1,     3,     3,     5,     1,     3,     3,     1,     1,
       1,     1,     3,     1,     1,     1,     5,     3,     0,     2,
       0,     2,     1,     3,     1,     3,     4,     4,     3,     4,
       3,     3,     2,     3,     2,     3,     1,     1,     1,     1,
       1,     1,     3,     1,     1,     3,     1,     1,     2,     2,
       3,     3,     3,     1,     2,     2,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     1,     3,     3,     3,     1,     4,     4,     4,     4,
       4,     6,     6,     4,     4,     2,     3
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
       0,     0,     0,     0,     0,     0,     0
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
       0,     0,     0,     0,     0,     0,     0
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error.  */
static const unsigned char yydefact[] =
{
       0,     0,     0,     0,    15,     0,     1,    11,     2,     7,
       0,     0,     8,     0,     0,     0,     0,     0,     0,     0,
       0,    26,     0,     3,    72,    28,    24,     0,     0,    29,
      31,     0,    35,     0,     0,     0,     0,     0,     0,     0,
      16,     0,    61,     0,     0,     0,    37,    38,     0,     0,
       0,    68,     9,    27,     0,    18,    20,    22,    45,    19,
      39,     0,    43,     0,     0,     0,     0,     6,     4,    25,
      51,    49,    50,     0,     0,     0,    30,    46,    48,    73,
      32,     0,     0,     0,     0,   103,    97,    96,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    94,
      33,     0,    86,    87,    88,    89,    90,   125,   121,    36,
      68,    68,     0,     0,     0,    58,    63,    70,    64,    60,
      59,    65,     0,    10,     0,     0,     0,     0,     0,     0,
      78,     0,    74,     0,    17,    62,     0,    55,     0,     0,
     105,   104,     0,    98,    99,   135,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    82,    68,    68,    14,    68,    12,    84,     0,     0,
      69,    68,    21,    23,    40,    41,     0,    44,     0,    79,
       5,    53,    47,     0,    52,    85,   136,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    95,   119,   110,   109,
     100,   101,   102,   120,   118,   117,   114,   113,   112,   111,
     106,   107,   124,   123,   122,   115,   116,    34,   108,    57,
       0,    81,    80,    67,    93,     0,    91,    71,     0,     0,
      83,     0,    75,    56,   127,   128,   129,   130,   126,     0,
       0,   133,   134,    13,     0,     0,    77,    76,    42,    54,
       0,     0,    92,    66,   131,   132
};

/* YYPDEFGOTO[NTERM-NUM].  */
static const short int yydefgoto[] =
{
      -1,     2,     3,    23,    68,     8,     9,    10,    52,   186,
      99,    39,    40,    55,    56,    25,    20,    21,    57,    29,
      30,    31,    32,    59,    60,    61,    62,   137,    77,    78,
     138,   114,    41,   115,   116,   117,   188,    27,   146,   118,
      42,   119,   120,   121,   122,   245,   246,   102,   103,   104,
     105,   106,   107,   108
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -107
static const short int yypact[] =
{
       1,    15,    20,    37,  -107,    14,  -107,  -107,    37,  -107,
      15,    21,  -107,    28,    66,    43,    74,    15,    15,    84,
     -17,  -107,    63,  -107,  -107,    90,  -107,    -5,    64,    92,
    -107,    33,  -107,    98,    51,   100,    15,    15,    56,    65,
    -107,     3,  -107,    15,    67,    15,  -107,  -107,    15,   340,
      15,   155,  -107,  -107,    98,    99,  -107,   102,  -107,   108,
    -107,    48,  -107,   400,    15,    94,    15,  -107,  -107,  -107,
    -107,  -107,  -107,    67,    67,    67,  -107,  -107,  -107,  -107,
    -107,   755,   755,   340,   755,  -107,  -107,  -107,   755,   277,
     159,   167,   168,   169,   170,   171,   172,   173,   175,   122,
    -107,   969,  -107,  -107,  -107,  -107,  -107,  -107,  -107,  -107,
     423,   483,   506,    83,    72,  -107,  -107,   566,  -107,  -107,
    -107,  -107,   921,  -107,    15,   755,    15,   340,    15,   755,
    -107,    68,  1413,     3,  -107,   160,   177,   176,   186,   147,
     -23,   341,  1006,  1450,  1487,  -107,   -21,   755,   755,   755,
     755,   755,   755,   755,   755,   755,   755,   755,   755,   755,
     755,   755,   755,   755,   755,   755,   755,   755,   755,   755,
     755,   755,   755,   755,   755,   755,   755,    67,   755,    19,
     873,    77,   589,   649,  -107,   672,  -107,  -107,     2,   222,
    -107,   732,  -107,  1413,  -107,  -107,  1043,  -107,   755,  -107,
    -107,  -107,  -107,    67,  -107,  -107,  -107,  1080,  1117,  1154,
    1191,  1228,   788,   836,  1265,  1302,  1524,    87,  1635,  1657,
    1561,  1598,    95,  1694,   134,   136,   140,   142,   144,   146,
    1812,  1834,  1716,  1753,  1775,   151,   161,  -107,  1868,  -107,
      72,    77,    77,  -107,  -107,    82,  -107,  -107,   755,   755,
      77,    67,  1413,   180,  -107,  -107,  -107,  -107,  -107,   755,
     755,  -107,  -107,  -107,    15,   755,   190,   190,  -107,  -107,
    1339,  1376,  -107,   190,  -107,  -107
};

/* YYPGOTO[NTERM-NUM].  */
static const short int yypgoto[] =
{
    -107,  -107,  -107,  -107,    81,  -107,   208,  -107,   163,   -20,
      -1,   181,   154,  -107,    97,  -107,  -107,   188,    -4,  -107,
     189,  -107,   191,  -107,   110,  -107,   111,   -11,  -107,  -107,
    -107,  -106,    45,  -107,  -107,  -107,  -107,  -107,   -60,  -107,
    -107,  -107,  -107,  -107,   -15,  -107,   -26,  -107,  -107,  -107,
    -107,  -107,  -107,  -107
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const unsigned short int yytable[] =
{
       5,   157,   198,   131,   179,   181,    34,     1,    44,    13,
     160,   161,   243,    26,    66,    35,    24,    28,    45,     4,
       6,    38,   162,    67,   164,   165,   166,   167,   168,   169,
     170,   183,     4,    76,   101,    24,    58,   171,   100,    69,
       7,   185,    24,   206,    79,    11,    49,    28,   132,    28,
      17,   239,    18,    14,   175,   176,    50,   178,    17,    15,
      18,   127,   136,    38,   139,    38,   140,   141,   142,   143,
      36,   128,    37,   144,   132,    19,   240,   241,   139,   242,
       4,    70,    71,    72,   183,   250,    64,    63,    65,   183,
      16,   198,   184,    22,   185,   180,    73,   132,    74,   185,
     199,    36,   189,    37,   182,   264,    65,   265,    33,   133,
     193,   135,   196,    43,   142,    48,   195,    51,    46,    47,
     160,   161,   124,    24,    54,    58,   125,    58,   160,   161,
      75,   126,   207,   208,   209,   210,   211,   212,   213,   214,
     215,   216,   217,   218,   219,   220,   221,   222,   223,   224,
     225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,    36,   238,    37,   156,   237,   160,   161,   160,
     161,    66,     4,   160,   161,   160,   161,   160,   161,   160,
     161,    81,    82,   252,   160,   161,   110,   244,   266,   267,
     147,    84,   253,    85,   160,   161,    86,    87,   148,   149,
     150,   151,   152,   153,   154,   273,   155,   201,   202,   203,
      88,   204,   269,   198,   200,   111,    12,   123,   112,   134,
     263,   192,    53,    90,    91,    92,    93,    94,    95,    96,
      97,    98,   113,   132,   132,   247,   194,    80,   272,   197,
     268,   109,     0,     0,   270,   271,   157,     0,   158,   159,
     132,     0,     0,     0,     0,   160,   161,     0,     0,     0,
       0,     0,     0,   244,     0,     0,     0,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   248,   249,     0,     0,
       0,     0,   171,     0,     0,     0,     0,   172,   173,   174,
       0,    70,    71,    72,     4,     0,     0,     0,     0,   175,
     176,     0,   178,    81,    82,     0,    73,     0,    83,     0,
       0,     0,     0,    84,     0,    85,     0,     0,    86,    87,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    88,     0,     0,     0,     0,     0,     0,     0,
      89,   145,     0,     0,     0,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    70,    71,    72,     4,     0,     0,
       0,     0,     0,     0,     0,   157,    81,    82,     0,    73,
       0,    83,     0,     0,   160,   161,    84,     0,    85,     0,
       0,    86,    87,     0,     0,     0,   162,     0,   164,   165,
     166,   167,   168,   169,   170,    88,     0,     0,     0,     0,
       0,   171,     0,    89,     0,     0,     0,     0,    90,    91,
      92,    93,    94,    95,    96,    97,    98,     4,   175,   176,
       0,   178,     0,     0,     0,     0,    81,    82,     0,     0,
       0,   129,   130,     0,     0,     0,    84,     0,    85,     0,
       4,    86,    87,     0,     0,     0,     0,     0,     0,    81,
      82,     0,     0,     0,   110,    88,     0,     0,     0,    84,
       0,    85,     0,   112,    86,    87,     0,     0,    90,    91,
      92,    93,    94,    95,    96,    97,    98,     0,    88,     0,
       0,     0,     0,   111,     0,     0,   112,     0,     0,     0,
       0,    90,    91,    92,    93,    94,    95,    96,    97,    98,
       4,     0,     0,     0,     0,     0,     0,     0,     0,    81,
      82,     0,     0,     0,   110,     0,     0,     0,     0,    84,
       0,    85,     0,     4,    86,    87,     0,     0,     0,     0,
       0,     0,    81,    82,     0,     0,     0,   129,    88,     0,
       0,     0,    84,   111,    85,     0,   112,    86,    87,     0,
       0,    90,    91,    92,    93,    94,    95,    96,    97,    98,
       0,    88,     0,     0,     0,     0,     0,     0,     0,   112,
     145,     0,     0,     0,    90,    91,    92,    93,    94,    95,
      96,    97,    98,     4,     0,     0,     0,     0,     0,     0,
       0,     0,    81,    82,     0,     0,     0,   129,     0,     0,
       0,     0,    84,     0,    85,     0,     4,    86,    87,     0,
     187,     0,     0,     0,     0,    81,    82,     0,     0,     0,
     110,    88,     0,     0,     0,    84,     0,    85,     0,   112,
      86,    87,     0,     0,    90,    91,    92,    93,    94,    95,
      96,    97,    98,     0,    88,     0,     0,     0,     0,   111,
       0,     0,   112,     0,     0,     0,     0,    90,    91,    92,
      93,    94,    95,    96,    97,    98,     4,     0,     0,     0,
       0,     0,     0,     0,     0,    81,    82,     0,     0,     0,
     110,     0,     0,     0,     0,    84,     0,    85,     0,     4,
      86,    87,     0,     0,     0,     0,     0,     0,    81,    82,
       0,     0,     0,   110,    88,     0,     0,     0,    84,   111,
      85,     0,   112,    86,    87,     0,     0,    90,    91,    92,
      93,    94,    95,    96,    97,    98,     0,    88,     0,     0,
       0,     0,   111,     0,     0,   112,     0,     0,     0,     0,
      90,    91,    92,    93,    94,    95,    96,    97,    98,     4,
       0,     0,     0,     0,     0,     0,     0,     0,    81,    82,
       0,     0,     0,   110,     0,     0,     0,     0,    84,     0,
      85,     0,     4,    86,    87,     0,     0,     0,     0,     0,
       0,    81,    82,     0,     0,     0,   129,    88,     0,     0,
       0,    84,   111,    85,     0,   112,    86,    87,     0,     0,
      90,    91,    92,    93,    94,    95,    96,    97,    98,     0,
      88,   259,   157,     0,   158,   159,     0,     0,   112,     0,
       0,   160,   161,    90,    91,    92,    93,    94,    95,    96,
      97,    98,     0,   162,   163,   164,   165,   166,   167,   168,
     169,   170,     0,     0,     0,     0,     0,     0,   171,     0,
       0,     0,     0,   172,   173,   174,     0,     0,     0,   260,
     157,     0,   158,   159,     0,   175,   176,     0,   178,   160,
     161,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   162,   163,   164,   165,   166,   167,   168,   169,   170,
       0,     0,     0,     0,     0,     0,   171,   157,     0,   158,
     159,   172,   173,   174,     0,   205,   160,   161,   190,     0,
       0,     0,     0,   175,   176,     0,   178,     0,   162,   163,
     164,   165,   166,   167,   168,   169,   170,     0,     0,     0,
       0,     0,     0,   171,   191,     0,     0,     0,   172,   173,
     174,     0,     0,     0,     0,   157,     0,   158,   159,     0,
     175,   176,     0,   178,   160,   161,   190,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   162,   163,   164,   165,
     166,   167,   168,   169,   170,     0,     0,     0,     0,     0,
       0,   171,   191,     0,     0,     0,   172,   173,   174,     0,
       0,     0,     0,   157,     0,   158,   159,     0,   175,   176,
       0,   178,   160,   161,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   162,   163,   164,   165,   166,   167,
     168,   169,   170,     0,     0,     0,     0,     0,     0,   171,
     157,     0,   158,   159,   172,   173,   174,     0,   205,   160,
     161,     0,     0,     0,     0,     0,   175,   176,   177,   178,
       0,   162,   163,   164,   165,   166,   167,   168,   169,   170,
       0,     0,     0,     0,     0,     0,   171,   157,     0,   158,
     159,   172,   173,   174,     0,     0,   160,   161,     0,     0,
       0,     0,     0,   175,   176,     0,   178,     0,   162,   163,
     164,   165,   166,   167,   168,   169,   170,     0,     0,     0,
       0,     0,     0,   171,   157,     0,   158,   159,   172,   173,
     174,     0,   254,   160,   161,     0,     0,     0,     0,     0,
     175,   176,   251,   178,     0,   162,   163,   164,   165,   166,
     167,   168,   169,   170,     0,     0,     0,     0,     0,     0,
     171,   157,     0,   158,   159,   172,   173,   174,     0,   255,
     160,   161,     0,     0,     0,     0,     0,   175,   176,     0,
     178,     0,   162,   163,   164,   165,   166,   167,   168,   169,
     170,     0,     0,     0,     0,     0,     0,   171,   157,     0,
     158,   159,   172,   173,   174,     0,   256,   160,   161,     0,
       0,     0,     0,     0,   175,   176,     0,   178,     0,   162,
     163,   164,   165,   166,   167,   168,   169,   170,     0,     0,
       0,     0,     0,     0,   171,   157,     0,   158,   159,   172,
     173,   174,     0,   257,   160,   161,     0,     0,     0,     0,
       0,   175,   176,     0,   178,     0,   162,   163,   164,   165,
     166,   167,   168,   169,   170,     0,     0,     0,     0,     0,
       0,   171,   157,     0,   158,   159,   172,   173,   174,     0,
     258,   160,   161,     0,     0,     0,     0,     0,   175,   176,
       0,   178,     0,   162,   163,   164,   165,   166,   167,   168,
     169,   170,     0,     0,     0,     0,     0,     0,   171,   157,
       0,   158,   159,   172,   173,   174,     0,   261,   160,   161,
       0,     0,     0,     0,     0,   175,   176,     0,   178,     0,
     162,   163,   164,   165,   166,   167,   168,   169,   170,     0,
       0,     0,     0,     0,     0,   171,   157,     0,   158,   159,
     172,   173,   174,     0,   262,   160,   161,     0,     0,     0,
       0,     0,   175,   176,     0,   178,     0,   162,   163,   164,
     165,   166,   167,   168,   169,   170,     0,     0,     0,     0,
       0,     0,   171,   157,     0,   158,   159,   172,   173,   174,
       0,   274,   160,   161,     0,     0,     0,     0,     0,   175,
     176,     0,   178,     0,   162,   163,   164,   165,   166,   167,
     168,   169,   170,     0,     0,     0,     0,     0,     0,   171,
     157,     0,   158,   159,   172,   173,   174,     0,   275,   160,
     161,     0,     0,     0,     0,     0,   175,   176,     0,   178,
       0,   162,   163,   164,   165,   166,   167,   168,   169,   170,
       0,     0,     0,     0,     0,     0,   171,   157,     0,   158,
     159,   172,   173,   174,     0,     0,   160,   161,     0,     0,
       0,     0,     0,   175,   176,     0,   178,     0,   162,   163,
     164,   165,   166,   167,   168,   169,   170,     0,     0,     0,
       0,     0,     0,   171,   157,     0,   158,   159,   172,   173,
     174,     0,     0,   160,   161,     0,     0,     0,     0,     0,
     175,   176,     0,   178,     0,   162,   163,   164,   165,   166,
     167,   168,   169,   170,     0,     0,     0,     0,     0,     0,
     171,   157,     0,   158,   159,   172,   173,   174,     0,     0,
     160,   161,     0,     0,     0,     0,     0,   175,   176,     0,
     178,     0,   162,   163,   164,   165,   166,   167,   168,   169,
     170,     0,     0,     0,     0,     0,     0,   171,   157,     0,
     158,   159,   172,   173,   174,     0,     0,   160,   161,     0,
       0,     0,     0,     0,   175,   176,     0,   178,     0,   162,
     163,   164,   165,   166,   167,   168,   169,   170,     0,     0,
       0,     0,     0,     0,   171,   157,     0,   158,   159,   172,
     173,   174,     0,     0,   160,   161,     0,     0,     0,     0,
       0,   175,   176,     0,   178,     0,   162,   163,   164,   165,
     166,   167,   168,   169,   170,     0,     0,     0,     0,     0,
       0,   171,   157,     0,   158,   159,   172,   173,   174,     0,
       0,   160,   161,     0,     0,     0,     0,     0,   175,   176,
       0,   178,     0,   162,   163,   164,   165,   166,   167,   168,
     169,   170,     0,     0,     0,     0,     0,     0,   171,   157,
       0,     0,     0,   172,   173,   174,     0,     0,   160,   161,
       0,     0,     0,     0,     0,   175,   176,     0,   178,     0,
     162,   157,   164,   165,   166,   167,   168,   169,   170,     0,
     160,   161,     0,     0,     0,   171,     0,     0,     0,     0,
       0,     0,   162,     0,   164,   165,   166,   167,   168,   169,
     170,     0,   175,   176,     0,   178,     0,   171,   157,     0,
       0,     0,     0,     0,     0,     0,     0,   160,   161,     0,
       0,     0,     0,     0,   175,   176,     0,   178,     0,   162,
     157,   164,   165,   166,   167,   168,   169,   170,     0,   160,
     161,     0,     0,     0,   171,     0,     0,     0,     0,     0,
       0,   162,     0,   164,   165,   166,   167,   168,   169,   170,
       0,   175,   176,     0,   178,     0,   171,   157,     0,     0,
       0,     0,     0,     0,     0,     0,   160,   161,     0,     0,
       0,     0,     0,   175,   176,     0,   178,     0,   162,   157,
     164,   165,   166,   167,   168,   169,   170,     0,   160,   161,
       0,     0,     0,   171,     0,     0,     0,     0,     0,     0,
     162,     0,   164,   165,   166,   167,   168,   169,   170,     0,
     175,   176,     0,   178,     0,   171,   157,     0,     0,     0,
       0,     0,     0,     0,     0,   160,   161,     0,     0,     0,
       0,     0,   175,   176,     0,   178,     0,   162,   157,   164,
     165,   166,   167,   168,   169,   170,     0,   160,   161,     0,
       0,     0,   171,     0,     0,     0,     0,     0,     0,   162,
       0,   164,   165,   166,   167,   168,   169,   170,     0,   175,
     176,     0,   157,     0,   171,     0,     0,     0,     0,     0,
       0,   160,   161,     0,     0,     0,     0,     0,     0,     0,
       0,   175,   176,   162,     0,   164,   165,   166,   167,   168,
     169,   170,     0,     0,     0,     0,     0,     0,   171,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   175,   176
};

/* YYCONFLP[YYPACT[STATE-NUM]] -- Pointer into YYCONFL of start of
   list of conflicting reductions corresponding to action entry for
   state STATE-NUM in yytable.  0 means no conflicts.  The list in
   yyconfl is terminated by a rule number of 0.  */
static const unsigned short int yyconflp[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     125,   127,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     1,     0,     3,     0,     0,     0,     0,
     411,   413,    43,     0,     0,     0,     0,     0,   507,   509,
       0,    45,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   515,   517,   519,
     521,     0,     5,   523,   525,   527,   529,   531,   533,   535,
     537,     7,     9,     0,   559,   561,    11,     0,     0,     0,
       0,    13,     0,    15,   563,   565,    17,    19,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      21,     0,     0,     0,     0,     0,     0,     0,    23,     0,
       0,     0,     0,    25,    27,    29,    31,    33,    35,    37,
      39,    41,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   129,   131,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      47,     0,     0,     0,     0,     0,     0,     0,     0,    49,
      51,     0,     0,     0,    53,     0,     0,     0,     0,    55,
       0,    57,     0,     0,    59,    61,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    63,     0,
       0,     0,     0,     0,     0,     0,    65,     0,     0,     0,
       0,    67,    69,    71,    73,    75,    77,    79,    81,    83,
      85,     0,     0,     0,     0,     0,     0,     0,     0,    87,
      89,     0,     0,     0,    91,     0,     0,     0,     0,    93,
       0,    95,     0,     0,    97,    99,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   101,     0,
       0,     0,     0,     0,     0,     0,   103,     0,     0,     0,
       0,   105,   107,   109,   111,   113,   115,   117,   119,   121,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   123,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   217,     0,     0,     0,
       0,     0,     0,     0,     0,   219,   221,     0,     0,     0,
     223,     0,     0,     0,     0,   225,     0,   227,     0,     0,
     229,   231,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   233,     0,     0,     0,     0,     0,
       0,     0,   235,     0,     0,     0,     0,   237,   239,   241,
     243,   245,   247,   249,   251,   253,   255,     0,     0,     0,
       0,     0,     0,     0,     0,   257,   259,     0,     0,     0,
     261,     0,     0,     0,     0,   263,     0,   265,     0,   293,
     267,   269,     0,     0,     0,     0,     0,     0,   295,   297,
       0,     0,     0,   299,   271,     0,     0,     0,   301,     0,
     303,     0,   273,   305,   307,     0,     0,   275,   277,   279,
     281,   283,   285,   287,   289,   291,     0,   309,     0,     0,
       0,     0,     0,     0,     0,   311,     0,     0,     0,     0,
     313,   315,   317,   319,   321,   323,   325,   327,   329,   331,
       0,     0,     0,     0,     0,     0,     0,     0,   333,   335,
       0,     0,     0,   337,     0,     0,     0,     0,   339,     0,
     341,     0,     0,   343,   345,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   347,     0,     0,
       0,     0,     0,     0,     0,   349,     0,     0,     0,     0,
     351,   353,   355,   357,   359,   361,   363,   365,   367,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   133,     0,   135,   137,     0,     0,
       0,     0,     0,   139,   141,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   143,   145,   147,   149,   151,
     153,   155,   157,   159,     0,     0,     0,     0,     0,     0,
     161,   175,     0,   177,   179,   163,   165,   167,     0,     0,
     181,   183,     0,     0,     0,     0,     0,   169,   171,     0,
     173,     0,   185,   187,   189,   191,   193,   195,   197,   199,
     201,     0,     0,     0,     0,     0,     0,   203,   369,     0,
     371,   373,   205,   207,   209,     0,     0,   375,   377,     0,
       0,     0,     0,     0,   211,   213,     0,   215,     0,   379,
     381,   383,   385,   387,   389,   391,   393,   395,     0,     0,
       0,     0,     0,     0,   397,   423,     0,   425,   427,   399,
     401,   403,     0,     0,   429,   431,     0,     0,     0,     0,
       0,   405,   407,     0,   409,     0,   433,   435,   437,   439,
     441,   443,   445,   447,   449,     0,     0,     0,     0,     0,
       0,   451,   465,     0,   467,   469,   453,   455,   457,     0,
       0,   471,   473,     0,     0,     0,     0,     0,   459,   461,
       0,   463,     0,   475,   477,   479,   481,   483,   485,   487,
     489,   491,     0,     0,     0,     0,     0,     0,   493,     0,
       0,     0,     0,   495,   497,   499,     0,     0,   415,   417,
       0,     0,     0,     0,     0,   501,   503,     0,   505,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     419,   421,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   511,   513,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   547,
     549,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   551,   553,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   555,   557,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   539,   541,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   543,   545,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   567,   569,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0
};

/* YYCONFL[I] -- lists of conflicting rule numbers, each terminated by
   0, pointed into by YYCONFLP.  */
static const short int yyconfl[] =
{
       0,    28,     0,    29,     0,    68,     0,    68,     0,    68,
       0,    68,     0,    68,     0,    68,     0,    68,     0,    68,
       0,    68,     0,    68,     0,    68,     0,    68,     0,    68,
       0,    68,     0,    68,     0,    68,     0,    68,     0,    68,
       0,    68,     0,    18,     0,    19,     0,    68,     0,    68,
       0,    68,     0,    68,     0,    68,     0,    68,     0,    68,
       0,    68,     0,    68,     0,    68,     0,    68,     0,    68,
       0,    68,     0,    68,     0,    68,     0,    68,     0,    68,
       0,    68,     0,    68,     0,    68,     0,    68,     0,    68,
       0,    68,     0,    68,     0,    68,     0,    68,     0,    68,
       0,    68,     0,    68,     0,    68,     0,    68,     0,    68,
       0,    68,     0,    68,     0,    68,     0,    68,     0,    68,
       0,    68,     0,    70,     0,   105,     0,   105,     0,   104,
       0,   104,     0,    98,     0,    98,     0,    98,     0,    98,
       0,    98,     0,    98,     0,    98,     0,    98,     0,    98,
       0,    98,     0,    98,     0,    98,     0,    98,     0,    98,
       0,    98,     0,    98,     0,    98,     0,    98,     0,    98,
       0,    98,     0,    98,     0,    99,     0,    99,     0,    99,
       0,    99,     0,    99,     0,    99,     0,    99,     0,    99,
       0,    99,     0,    99,     0,    99,     0,    99,     0,    99,
       0,    99,     0,    99,     0,    99,     0,    99,     0,    99,
       0,    99,     0,    99,     0,    99,     0,    68,     0,    68,
       0,    68,     0,    68,     0,    68,     0,    68,     0,    68,
       0,    68,     0,    68,     0,    68,     0,    68,     0,    68,
       0,    68,     0,    68,     0,    68,     0,    68,     0,    68,
       0,    68,     0,    68,     0,    68,     0,    68,     0,    68,
       0,    68,     0,    68,     0,    68,     0,    68,     0,    68,
       0,    68,     0,    68,     0,    68,     0,    68,     0,    68,
       0,    68,     0,    68,     0,    68,     0,    68,     0,    68,
       0,    68,     0,    68,     0,    68,     0,    68,     0,    68,
       0,    68,     0,    68,     0,    68,     0,    68,     0,    68,
       0,    68,     0,    68,     0,    68,     0,    68,     0,    68,
       0,    68,     0,    68,     0,    68,     0,    68,     0,    68,
       0,    68,     0,    68,     0,    68,     0,    68,     0,    68,
       0,    68,     0,    68,     0,    68,     0,    68,     0,    68,
       0,    68,     0,    68,     0,    68,     0,    68,     0,    68,
       0,    68,     0,    68,     0,    68,     0,    68,     0,    95,
       0,    95,     0,    95,     0,    95,     0,    95,     0,    95,
       0,    95,     0,    95,     0,    95,     0,    95,     0,    95,
       0,    95,     0,    95,     0,    95,     0,    95,     0,    95,
       0,    95,     0,    95,     0,    95,     0,    95,     0,    95,
       0,   119,     0,   119,     0,   110,     0,   110,     0,   109,
       0,   109,     0,   100,     0,   100,     0,   100,     0,   100,
       0,   100,     0,   100,     0,   100,     0,   100,     0,   100,
       0,   100,     0,   100,     0,   100,     0,   100,     0,   100,
       0,   100,     0,   100,     0,   100,     0,   100,     0,   100,
       0,   100,     0,   100,     0,   101,     0,   101,     0,   101,
       0,   101,     0,   101,     0,   101,     0,   101,     0,   101,
       0,   101,     0,   101,     0,   101,     0,   101,     0,   101,
       0,   101,     0,   101,     0,   101,     0,   101,     0,   101,
       0,   101,     0,   101,     0,   101,     0,   102,     0,   102,
       0,   120,     0,   120,     0,   118,     0,   118,     0,   117,
       0,   117,     0,   114,     0,   114,     0,   113,     0,   113,
       0,   112,     0,   112,     0,   111,     0,   111,     0,   106,
       0,   106,     0,   107,     0,   107,     0,   124,     0,   124,
       0,   123,     0,   123,     0,   122,     0,   122,     0,   115,
       0,   115,     0,   116,     0,   116,     0,   108,     0,   108,
       0
};

static const short int yycheck[] =
{
       1,    24,    23,    63,   110,   111,    23,     6,    13,    10,
      33,    34,    10,    17,    11,    32,    17,    18,    23,    17,
       0,    22,    45,    20,    47,    48,    49,    50,    51,    52,
      53,    12,    17,    44,    49,    36,    37,    60,    49,    43,
       3,    22,    43,    64,    45,    31,    13,    48,    63,    50,
       7,    32,     9,    32,    77,    78,    23,    80,     7,    31,
       9,    13,    73,    64,    75,    66,    81,    82,    83,    84,
       7,    23,     9,    88,    89,    32,   182,   183,    89,   185,
      17,    14,    15,    16,    12,   191,    21,    31,    23,    12,
      24,    23,    20,    19,    22,   110,    29,   112,    31,    22,
      32,     7,   117,     9,    21,    23,    23,    25,    24,    64,
     125,    66,   127,    23,   129,    23,   127,    19,    54,    55,
      33,    34,    23,   124,    24,   126,    24,   128,    33,    34,
      63,    23,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,     7,   178,     9,    43,   177,    33,    34,    33,
      34,    11,    17,    33,    34,    33,    34,    33,    34,    33,
      34,    26,    27,   198,    33,    34,    31,   188,   248,   249,
      31,    36,   203,    38,    33,    34,    41,    42,    31,    31,
      31,    31,    31,    31,    31,   265,    31,    30,    32,    23,
      55,    64,    32,    23,   133,    60,     8,    54,    63,    65,
     240,   124,    34,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    51,   248,   249,    13,   126,    48,   264,   128,
     251,    50,    -1,    -1,   259,   260,    24,    -1,    26,    27,
     265,    -1,    -1,    -1,    -1,    33,    34,    -1,    -1,    -1,
      -1,    -1,    -1,   264,    -1,    -1,    -1,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    -1,    -1,
      -1,    -1,    60,    -1,    -1,    -1,    -1,    65,    66,    67,
      -1,    14,    15,    16,    17,    -1,    -1,    -1,    -1,    77,
      78,    -1,    80,    26,    27,    -1,    29,    -1,    31,    -1,
      -1,    -1,    -1,    36,    -1,    38,    -1,    -1,    41,    42,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    55,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      63,    64,    -1,    -1,    -1,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    14,    15,    16,    17,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    24,    26,    27,    -1,    29,
      -1,    31,    -1,    -1,    33,    34,    36,    -1,    38,    -1,
      -1,    41,    42,    -1,    -1,    -1,    45,    -1,    47,    48,
      49,    50,    51,    52,    53,    55,    -1,    -1,    -1,    -1,
      -1,    60,    -1,    63,    -1,    -1,    -1,    -1,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    17,    77,    78,
      -1,    80,    -1,    -1,    -1,    -1,    26,    27,    -1,    -1,
      -1,    31,    32,    -1,    -1,    -1,    36,    -1,    38,    -1,
      17,    41,    42,    -1,    -1,    -1,    -1,    -1,    -1,    26,
      27,    -1,    -1,    -1,    31,    55,    -1,    -1,    -1,    36,
      -1,    38,    -1,    63,    41,    42,    -1,    -1,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    -1,    55,    -1,
      -1,    -1,    -1,    60,    -1,    -1,    63,    -1,    -1,    -1,
      -1,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      17,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    26,
      27,    -1,    -1,    -1,    31,    -1,    -1,    -1,    -1,    36,
      -1,    38,    -1,    17,    41,    42,    -1,    -1,    -1,    -1,
      -1,    -1,    26,    27,    -1,    -1,    -1,    31,    55,    -1,
      -1,    -1,    36,    60,    38,    -1,    63,    41,    42,    -1,
      -1,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      -1,    55,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    63,
      64,    -1,    -1,    -1,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    17,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    26,    27,    -1,    -1,    -1,    31,    -1,    -1,
      -1,    -1,    36,    -1,    38,    -1,    17,    41,    42,    -1,
      44,    -1,    -1,    -1,    -1,    26,    27,    -1,    -1,    -1,
      31,    55,    -1,    -1,    -1,    36,    -1,    38,    -1,    63,
      41,    42,    -1,    -1,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    -1,    55,    -1,    -1,    -1,    -1,    60,
      -1,    -1,    63,    -1,    -1,    -1,    -1,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    17,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    26,    27,    -1,    -1,    -1,
      31,    -1,    -1,    -1,    -1,    36,    -1,    38,    -1,    17,
      41,    42,    -1,    -1,    -1,    -1,    -1,    -1,    26,    27,
      -1,    -1,    -1,    31,    55,    -1,    -1,    -1,    36,    60,
      38,    -1,    63,    41,    42,    -1,    -1,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    -1,    55,    -1,    -1,
      -1,    -1,    60,    -1,    -1,    63,    -1,    -1,    -1,    -1,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    17,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    26,    27,
      -1,    -1,    -1,    31,    -1,    -1,    -1,    -1,    36,    -1,
      38,    -1,    17,    41,    42,    -1,    -1,    -1,    -1,    -1,
      -1,    26,    27,    -1,    -1,    -1,    31,    55,    -1,    -1,
      -1,    36,    60,    38,    -1,    63,    41,    42,    -1,    -1,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    -1,
      55,    23,    24,    -1,    26,    27,    -1,    -1,    63,    -1,
      -1,    33,    34,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    -1,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    -1,    -1,    -1,    -1,    -1,    -1,    60,    -1,
      -1,    -1,    -1,    65,    66,    67,    -1,    -1,    -1,    23,
      24,    -1,    26,    27,    -1,    77,    78,    -1,    80,    33,
      34,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      -1,    -1,    -1,    -1,    -1,    -1,    60,    24,    -1,    26,
      27,    65,    66,    67,    -1,    32,    33,    34,    35,    -1,
      -1,    -1,    -1,    77,    78,    -1,    80,    -1,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    -1,    -1,    -1,
      -1,    -1,    -1,    60,    61,    -1,    -1,    -1,    65,    66,
      67,    -1,    -1,    -1,    -1,    24,    -1,    26,    27,    -1,
      77,    78,    -1,    80,    33,    34,    35,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    -1,    -1,    -1,    -1,    -1,
      -1,    60,    61,    -1,    -1,    -1,    65,    66,    67,    -1,
      -1,    -1,    -1,    24,    -1,    26,    27,    -1,    77,    78,
      -1,    80,    33,    34,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    -1,    -1,    -1,    -1,    -1,    -1,    60,
      24,    -1,    26,    27,    65,    66,    67,    -1,    32,    33,
      34,    -1,    -1,    -1,    -1,    -1,    77,    78,    79,    80,
      -1,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      -1,    -1,    -1,    -1,    -1,    -1,    60,    24,    -1,    26,
      27,    65,    66,    67,    -1,    -1,    33,    34,    -1,    -1,
      -1,    -1,    -1,    77,    78,    -1,    80,    -1,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    -1,    -1,    -1,
      -1,    -1,    -1,    60,    24,    -1,    26,    27,    65,    66,
      67,    -1,    32,    33,    34,    -1,    -1,    -1,    -1,    -1,
      77,    78,    79,    80,    -1,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    -1,    -1,    -1,    -1,    -1,    -1,
      60,    24,    -1,    26,    27,    65,    66,    67,    -1,    32,
      33,    34,    -1,    -1,    -1,    -1,    -1,    77,    78,    -1,
      80,    -1,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    -1,    -1,    -1,    -1,    -1,    -1,    60,    24,    -1,
      26,    27,    65,    66,    67,    -1,    32,    33,    34,    -1,
      -1,    -1,    -1,    -1,    77,    78,    -1,    80,    -1,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    -1,    -1,
      -1,    -1,    -1,    -1,    60,    24,    -1,    26,    27,    65,
      66,    67,    -1,    32,    33,    34,    -1,    -1,    -1,    -1,
      -1,    77,    78,    -1,    80,    -1,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    -1,    -1,    -1,    -1,    -1,
      -1,    60,    24,    -1,    26,    27,    65,    66,    67,    -1,
      32,    33,    34,    -1,    -1,    -1,    -1,    -1,    77,    78,
      -1,    80,    -1,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    -1,    -1,    -1,    -1,    -1,    -1,    60,    24,
      -1,    26,    27,    65,    66,    67,    -1,    32,    33,    34,
      -1,    -1,    -1,    -1,    -1,    77,    78,    -1,    80,    -1,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    -1,
      -1,    -1,    -1,    -1,    -1,    60,    24,    -1,    26,    27,
      65,    66,    67,    -1,    32,    33,    34,    -1,    -1,    -1,
      -1,    -1,    77,    78,    -1,    80,    -1,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    -1,    -1,    -1,    -1,
      -1,    -1,    60,    24,    -1,    26,    27,    65,    66,    67,
      -1,    32,    33,    34,    -1,    -1,    -1,    -1,    -1,    77,
      78,    -1,    80,    -1,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    -1,    -1,    -1,    -1,    -1,    -1,    60,
      24,    -1,    26,    27,    65,    66,    67,    -1,    32,    33,
      34,    -1,    -1,    -1,    -1,    -1,    77,    78,    -1,    80,
      -1,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      -1,    -1,    -1,    -1,    -1,    -1,    60,    24,    -1,    26,
      27,    65,    66,    67,    -1,    -1,    33,    34,    -1,    -1,
      -1,    -1,    -1,    77,    78,    -1,    80,    -1,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    -1,    -1,    -1,
      -1,    -1,    -1,    60,    24,    -1,    26,    27,    65,    66,
      67,    -1,    -1,    33,    34,    -1,    -1,    -1,    -1,    -1,
      77,    78,    -1,    80,    -1,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    -1,    -1,    -1,    -1,    -1,    -1,
      60,    24,    -1,    26,    27,    65,    66,    67,    -1,    -1,
      33,    34,    -1,    -1,    -1,    -1,    -1,    77,    78,    -1,
      80,    -1,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    -1,    -1,    -1,    -1,    -1,    -1,    60,    24,    -1,
      26,    27,    65,    66,    67,    -1,    -1,    33,    34,    -1,
      -1,    -1,    -1,    -1,    77,    78,    -1,    80,    -1,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    -1,    -1,
      -1,    -1,    -1,    -1,    60,    24,    -1,    26,    27,    65,
      66,    67,    -1,    -1,    33,    34,    -1,    -1,    -1,    -1,
      -1,    77,    78,    -1,    80,    -1,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    -1,    -1,    -1,    -1,    -1,
      -1,    60,    24,    -1,    26,    27,    65,    66,    67,    -1,
      -1,    33,    34,    -1,    -1,    -1,    -1,    -1,    77,    78,
      -1,    80,    -1,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    -1,    -1,    -1,    -1,    -1,    -1,    60,    24,
      -1,    -1,    -1,    65,    66,    67,    -1,    -1,    33,    34,
      -1,    -1,    -1,    -1,    -1,    77,    78,    -1,    80,    -1,
      45,    24,    47,    48,    49,    50,    51,    52,    53,    -1,
      33,    34,    -1,    -1,    -1,    60,    -1,    -1,    -1,    -1,
      -1,    -1,    45,    -1,    47,    48,    49,    50,    51,    52,
      53,    -1,    77,    78,    -1,    80,    -1,    60,    24,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    33,    34,    -1,
      -1,    -1,    -1,    -1,    77,    78,    -1,    80,    -1,    45,
      24,    47,    48,    49,    50,    51,    52,    53,    -1,    33,
      34,    -1,    -1,    -1,    60,    -1,    -1,    -1,    -1,    -1,
      -1,    45,    -1,    47,    48,    49,    50,    51,    52,    53,
      -1,    77,    78,    -1,    80,    -1,    60,    24,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    33,    34,    -1,    -1,
      -1,    -1,    -1,    77,    78,    -1,    80,    -1,    45,    24,
      47,    48,    49,    50,    51,    52,    53,    -1,    33,    34,
      -1,    -1,    -1,    60,    -1,    -1,    -1,    -1,    -1,    -1,
      45,    -1,    47,    48,    49,    50,    51,    52,    53,    -1,
      77,    78,    -1,    80,    -1,    60,    24,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    33,    34,    -1,    -1,    -1,
      -1,    -1,    77,    78,    -1,    80,    -1,    45,    24,    47,
      48,    49,    50,    51,    52,    53,    -1,    33,    34,    -1,
      -1,    -1,    60,    -1,    -1,    -1,    -1,    -1,    -1,    45,
      -1,    47,    48,    49,    50,    51,    52,    53,    -1,    77,
      78,    -1,    24,    -1,    60,    -1,    -1,    -1,    -1,    -1,
      -1,    33,    34,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    77,    78,    45,    -1,    47,    48,    49,    50,    51,
      52,    53,    -1,    -1,    -1,    -1,    -1,    -1,    60,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    77,    78
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     6,    82,    83,    17,    91,     0,     3,    86,    87,
      88,    31,    87,    91,    32,    31,    24,     7,     9,    32,
      97,    98,    19,    84,    91,    96,    99,   118,    91,   100,
     101,   102,   103,    24,    23,    32,     7,     9,    91,    92,
      93,   113,   121,    23,    13,    23,    54,    55,    23,    13,
      23,    19,    89,    98,    24,    94,    95,    99,    91,   104,
     105,   106,   107,    31,    21,    23,    11,    20,    85,    99,
      14,    15,    16,    29,    31,    63,   108,   109,   110,    91,
     101,    26,    27,    31,    36,    38,    41,    42,    55,    63,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    91,
     108,   125,   128,   129,   130,   131,   132,   133,   134,   103,
      31,    60,    63,    92,   112,   114,   115,   116,   120,   122,
     123,   124,   125,    89,    23,    24,    23,    13,    23,    31,
      32,   119,   125,   113,    93,   113,   108,   108,   111,   108,
     125,   125,   125,   125,   125,    64,   119,    31,    31,    31,
      31,    31,    31,    31,    31,    31,    43,    24,    26,    27,
      33,    34,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    60,    65,    66,    67,    77,    78,    79,    80,   112,
     125,   112,    21,    12,    20,    22,    90,    44,   117,   125,
      35,    61,    95,   125,   105,   108,   125,   107,    23,    32,
      85,    30,    32,    23,    64,    32,    64,   125,   125,   125,
     125,   125,   125,   125,   125,   125,   125,   125,   125,   125,
     125,   125,   125,   125,   125,   125,   125,   125,   125,   125,
     125,   125,   125,   125,   125,   125,   125,   108,   125,    32,
     112,   112,   112,    10,    91,   126,   127,    13,    54,    55,
     112,    79,   125,   108,    32,    32,    32,    32,    32,    23,
      23,    32,    32,    90,    23,    25,   119,   119,   108,    32,
     125,   125,   127,   119,    32,    32
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
#line 129 "chiparser.yy"
    { 
          gsDebugMsg("%s;%d\n",__FILE__,__LINE__);
    	  gsDebugMsg("inputs contains a valid Chi-specification\n"); 
          safe_assign(((*yyvalp).appl), gsMakeChiSpec((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl),ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.list))));
		  spec_tree = ((*yyvalp).appl);
		;}
    break;

  case 3:
#line 139 "chiparser.yy"
    {
      	  safe_assign(((*yyvalp).appl), gsMakeModelDef((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (6))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((6) - (6))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed Model Def \n  %T\n", ((*yyvalp).appl));
        ;}
    break;

  case 4:
#line 147 "chiparser.yy"
    { safe_assign(((*yyvalp).appl), gsMakeModelSpec( ATmakeList0(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl) ));
      	  gsDebugMsg("parsed Model Body  \n  %T\n", ((*yyvalp).appl));	
		;}
    break;

  case 5:
#line 151 "chiparser.yy"
    { safe_assign(((*yyvalp).appl), gsMakeModelSpec( (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed Model Body  \n  %T\n", ((*yyvalp).appl));	
		;}
    break;

  case 6:
#line 158 "chiparser.yy"
    {
          var_type_map.clear();
          chan_type_direction_map.clear();
       ;}
    break;

  case 7:
#line 165 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed Process Definition \n  %T\n", ((*yyvalp).list));	
		;}
    break;

  case 8:
#line 169 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed Process Definition \n  %T\n", ((*yyvalp).list));	
		;}
    break;

  case 9:
#line 176 "chiparser.yy"
    { 
          /**
            * Check if Identifier is not already used
            *
            **/
          if(used_process_identifiers.find((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (6))].yystate.yysemantics.yysval.appl)) != used_process_identifiers.end())
          {
            gsErrorMsg("Duplicate definition for process %T", (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (6))].yystate.yysemantics.yysval.appl));
            exit(1);
          } else {
            used_process_identifiers.insert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (6))].yystate.yysemantics.yysval.appl));
          }
         
      	  safe_assign(((*yyvalp).appl), gsMakeProcDef((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (6))].yystate.yysemantics.yysval.appl), gsMakeProcDecl(ATmakeList0()) ,(((yyGLRStackItem const *)yyvsp)[YYFILL ((6) - (6))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed proc Def \n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 10:
#line 193 "chiparser.yy"
    { 
      	  safe_assign(((*yyvalp).appl), gsMakeProcDef((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (7))].yystate.yysemantics.yysval.appl), gsMakeProcDecl( ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (7))].yystate.yysemantics.yysval.list))), (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (7))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed proc Def\n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 11:
#line 209 "chiparser.yy"
    {
		  scope_lvl++;
		  gsDebugMsg("Increase Scope to: %d\n",scope_lvl);
		;}
    break;

  case 12:
#line 217 "chiparser.yy"
    { safe_assign(((*yyvalp).appl), gsMakeProcSpec( ATmakeList0(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl) ));
      	  gsDebugMsg("parsed ProcessBody  \n  %T\n", ((*yyvalp).appl));	
		;}
    break;

  case 13:
#line 221 "chiparser.yy"
    { safe_assign(((*yyvalp).appl), gsMakeProcSpec( ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.list)), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed ProcessBody  \n  %T\n", ((*yyvalp).appl));	
		;}
    break;

  case 14:
#line 228 "chiparser.yy"
    {
		  assert(scope_lvl > 0);
		  scope_lvl--;
		  gsDebugMsg("Decrease Scope to; %d\n",scope_lvl);
          var_type_map.clear();
          chan_type_direction_map.clear();
		;}
    break;

  case 15:
#line 238 "chiparser.yy"
    { 
 	  	  safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl) );
      	  gsDebugMsg("parsed id's\n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 16:
#line 245 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("LocalVariables_csp: parsed \n  %T\n", ((*yyvalp).list));	
		;}
    break;

  case 17:
#line 249 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("LocalVariables_csp: parsed \n  %T\n", ((*yyvalp).list));	
		;}
    break;

  case 18:
#line 256 "chiparser.yy"
    {
		  safe_assign(((*yyvalp).appl), gsMakeVarSpec( (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.list) ) );
		  gsDebugMsg("LocalVariables: parsed \n %T\n", ((*yyvalp).appl));
		;}
    break;

  case 19:
#line 261 "chiparser.yy"
    {
		  //safe_assign($$, gsMakeVarSpec( ATreverse( $2 ) ) );  //<-- gsMakeVarSpec aanpassen
		  gsDebugMsg("LocalVariables: parsed \n %T\n", ((*yyvalp).appl));
		;}
    break;

  case 20:
#line 270 "chiparser.yy"
    { safe_assign(((*yyvalp).list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list));
		  gsDebugMsg("IdentifierTypeExpression_csp: parsed \n %T\n", ((*yyvalp).list));
		;}
    break;

  case 21:
#line 274 "chiparser.yy"
    { 
          ATermList new_list = (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.list);
          ATermList list = ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list));
          while (!ATisEmpty(list)) 
          {
             gsDebugMsg("%T",ATgetFirst(list));
             new_list = ATinsert( new_list , ATgetFirst(list));
             list = ATgetNext( list ) ;
          }
          
          safe_assign(((*yyvalp).list), new_list);
		  gsDebugMsg("IdentifierTypeExpression_csp: parsed \n %T\n", ((*yyvalp).list));
		;}
    break;

  case 22:
#line 291 "chiparser.yy"
    {
		  safe_assign(((*yyvalp).list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list) );
		  gsDebugMsg("IdentifierTypeExpression: parsed \n %T\n", ((*yyvalp).list));
		;}
    break;

  case 23:
#line 296 "chiparser.yy"
    {
          ATermList list = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list);
          ATermList new_list = ATmakeList0();

          while (!ATisEmpty(list)) 
          {
             gsDebugMsg("%T",ATgetFirst(list));
             new_list = ATinsert( new_list , (ATerm) gsMakeDataVarExprID( (ATermAppl) ATgetFirst(list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
             list = ATgetNext( list ) ;
          }
          
          safe_assign(((*yyvalp).list), new_list);

		  //safe_assign($$, gsMakeDataVarExprID ( $1, $3 ) );
		  gsDebugMsg("IdentifierTypeExpression: parsed \n %T\n", ((*yyvalp).list));
		;}
    break;

  case 24:
#line 316 "chiparser.yy"
    { safe_assign(((*yyvalp).list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list));
		  gsDebugMsg("IdentifierType_csp: parsed \n %T\n", ((*yyvalp).list));
		;}
    break;

  case 25:
#line 320 "chiparser.yy"
    {
          ATermList new_list = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list);
          ATermList list = ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.list));
          while (!ATisEmpty(list)) 
          {
             gsDebugMsg("%T",ATgetFirst(list));
             new_list = ATinsert( new_list , ATgetFirst(list));
             list = ATgetNext( list ) ;
          }
          
          safe_assign(((*yyvalp).list), new_list);
		  gsDebugMsg("IdentifierType_csp: parsed \n %T\n", ((*yyvalp).list));
		;}
    break;

  case 26:
#line 337 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
		  gsDebugMsg("FormalParameter_csp: parsed \n %T\n", ((*yyvalp).list));
		;}
    break;

  case 27:
#line 341 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
		  gsDebugMsg("FormalParameter_csp: parsed \n %T\n", ((*yyvalp).list));
		;}
    break;

  case 28:
#line 348 "chiparser.yy"
    {
		  safe_assign(((*yyvalp).appl), gsMakeVarDecl( ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.list)) ) );
		  gsDebugMsg("FormalParameter: parsed \n %T\n", ((*yyvalp).appl));
		;}
    break;

  case 29:
#line 353 "chiparser.yy"
    {
		  safe_assign(((*yyvalp).appl), gsMakeChanDecl( ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.list)) ) );
		  gsDebugMsg("FormalParameter: parsed \n %T\n", ((*yyvalp).appl));
		;}
    break;

  case 30:
#line 361 "chiparser.yy"
    {
		  /**
			* Build TypeCheck table for declared variables
			*
			* TODO: Add scope
			*
			**/
          ATermList list = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list);
		  int n = ATgetLength( list );
		  for(int i = 0; i < n ; ++i )
          {
			 if (chan_type_direction_map.end() != chan_type_direction_map.find(ATgetFirst( list )))
			 {
			   gsErrorMsg("Channel %T is already defined!\n", ATgetFirst( list ));
			   exit(1);
			 };
			 list = ATgetTail( list, 1 ) ;
		  }	;

		  list = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list);
		  n = ATgetLength( list );
		  for(int i = 0; i < n ; ++i ){
			 if (var_type_map.end() != var_type_map.find(ATgetFirst( list )))
			 {
			   gsErrorMsg("Variable %T is already defined!\n", ATgetFirst( list ));
			   exit(1);
			 };
			 var_type_map[ATgetFirst( list )]= (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl);
			 list = ATgetTail( list, 1 ) ;
		  }	;
         
          list = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list);
          ATermList new_list = ATmakeList0();

          while (!ATisEmpty(list)) 
          {
             gsDebugMsg("%T",ATgetFirst(list));
             new_list = ATinsert( new_list , (ATerm) gsMakeDataVarID( (ATermAppl) ATgetFirst(list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
             list = ATgetNext( list ) ;
          }
          
          safe_assign(((*yyvalp).list), ATreverse(new_list));
		  gsDebugMsg("IdentifierType: parsed \n %T\n", ((*yyvalp).list));
		  gsDebugMsg("Typecheck Table %d\n", var_type_map.size()); 
  		;}
    break;

  case 31:
#line 410 "chiparser.yy"
    { safe_assign(((*yyvalp).list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list) );
      	  gsDebugMsg("ChannelDeclaration_csp: parsed formalparameter channel  \n  %T\n", ((*yyvalp).list));	
		;}
    break;

  case 32:
#line 414 "chiparser.yy"
    { 
          ATermList new_list = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list);
          ATermList list = ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.list));
          while (!ATisEmpty(list)) 
          {
             gsDebugMsg("%T",ATgetFirst(list));
             new_list = ATinsert( new_list , ATgetFirst(list));
             list = ATgetNext( list ) ;
          }
          safe_assign(((*yyvalp).list), new_list);
      	  gsDebugMsg("ChannelDeclaration_csp: parsed formalparameter channel \n  %T\n", ((*yyvalp).list));	
		;}
    break;

  case 33:
#line 430 "chiparser.yy"
    {
          gsDebugMsg("%s;%d\n",__FILE__,__LINE__);
		  ATermList list = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list);
		  int n = ATgetLength( list );
		  for(int i = 0; i < n ; ++i ){
			 if (var_type_map.end() != var_type_map.find(ATgetArgument( ATgetFirst( list ),0)))
			 {
			   gsErrorMsg("Variable %T is already defined!\n", ATgetFirst( list ));
			   exit(1);
			 };
			 list = ATgetTail( list, 1 ) ;
		  }	;
          gsDebugMsg("\n%T\n", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list));
		  
          list = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list);
		  n = ATgetLength( list );
		  for(int i = 0; i < n ; ++i ){
			 if (chan_type_direction_map.end() != chan_type_direction_map.find(ATgetArgument( ATgetFirst( list ),0)))
			 {
			   gsErrorMsg("Channel %T is already defined!\n", ATgetFirst( list ));
			   exit(1);
			 };
			 chan_type_direction_map[ATgetArgument(ATgetFirst( list ), 0)]=  make_pair( (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl), ATgetArgument(ATgetFirst( list ), 1) );
			 list = ATgetTail( list, 1 ) ;
		  }	;

		  list = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list);
          ATermList new_list = ATmakeList0();
		  while(!ATisEmpty(list))

          {
             ATermAppl one = (ATermAppl) gsMakeExpression(gsString2ATermAppl("1"), (ATermAppl) gsMakeType(gsString2ATermAppl("Nat")));
             new_list = ATinsert(new_list,(ATerm) gsMakeChannelTypedID( (ATermAppl) ATgetFirst(list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl), one ) );
			 list = ATgetNext( list ) ;
		  }

		  safe_assign(((*yyvalp).list), new_list );
		  gsDebugMsg("ChannelDefinition: parsed VariableList \n %T\n", ((*yyvalp).list));
		;}
    break;

  case 34:
#line 470 "chiparser.yy"
    {
          gsDebugMsg("%s;%d\n",__FILE__,__LINE__);

          //Make sure that Expression is a number 
          UnaryTypeCheck( (ATermAppl) ATgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.appl),1), "Nat");
        
		  ATermList list = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.list);
		  int n = ATgetLength( list );
		  for(int i = 0; i < n ; ++i ){
			 if (var_type_map.end() != var_type_map.find(ATgetArgument( ATgetFirst( list ),0)))
			 {
			   gsErrorMsg("Variable %T is already defined!\n", ATgetFirst( list ));
			   exit(1);
			 };
			 list = ATgetTail( list, 1 ) ;
		  }	;
		  
          list = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.list);
		  n = ATgetLength( list );
		  for(int i = 0; i < n ; ++i ){
			 if (chan_type_direction_map.end() != chan_type_direction_map.find(ATgetArgument( ATgetFirst( list ),0)))
			 {
			   gsErrorMsg("Channel %T is already defined!\n", ATgetFirst( list ));
			   exit(1);
			 };
			 chan_type_direction_map[ATgetArgument(ATgetFirst( list ), 0)]=  make_pair( (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl), ATgetArgument(ATgetFirst( list ), 1) );
			 list = ATgetTail( list, 1 ) ;
		  }	;

		  list = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.list);
          ATermList new_list = ATmakeList0();
		  while(!ATisEmpty(list))
          {
             gsDebugMsg("%T",ATgetFirst(list));
             new_list = ATinsert(new_list,(ATerm) gsMakeChannelTypedID( (ATermAppl) ATgetFirst(list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.appl) ) );
			 list = ATgetNext( list ) ;
		  }

		  safe_assign(((*yyvalp).list), new_list );
		  gsDebugMsg("ChannelDefinition: parsed VariableList \n %T\n", ((*yyvalp).list));
		;}
    break;

  case 35:
#line 515 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("IdentifierChannelDeclaration_csp: parsed formalparameter channel  \n  %T\n", ((*yyvalp).list));	
		;}
    break;

  case 36:
#line 519 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("IdentifierChannelDeclaration_csp: parsed formalparameter channel \n  %T\n", ((*yyvalp).list));	
		;}
    break;

  case 37:
#line 526 "chiparser.yy"
    {
          safe_assign(((*yyvalp).appl), gsMakeChannelID((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl), gsMakeRecv()));
		  gsDebugMsg("IdentifierChannelDeclaration: parsed Identifier Type With Expression \n %T\n", ((*yyvalp).appl));
        ;}
    break;

  case 38:
#line 531 "chiparser.yy"
    {
          safe_assign(((*yyvalp).appl), gsMakeChannelID((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl), gsMakeSend()));
		  gsDebugMsg("IdentifierChannelDeclaration: parsed Identifier Type With Expression \n %T\n", ((*yyvalp).appl));
        ;}
    break;

  case 39:
#line 549 "chiparser.yy"
    { //safe_assign($$, ATmakeList1((ATerm) $1));
      	  gsDebugMsg("ChannelDefinition_csp: parsed \n  %T\n", ((*yyvalp).list));	
		;}
    break;

  case 40:
#line 553 "chiparser.yy"
    { //safe_assign($$, ATinsert($1, (ATerm) $3));
      	  gsDebugMsg("ChannelDefinition_csp: parsed \n  %T\n", ((*yyvalp).list));	
		;}
    break;

  case 41:
#line 560 "chiparser.yy"
    {
          gsDebugMsg("%s;%d\n",__FILE__,__LINE__);
          gsDebugMsg("ChannelDefinition\n");
		  ATermList list = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list);
		  int n = ATgetLength( list );
		  for(int i = 0; i < n ; ++i ){
			 if (var_type_map.end() != var_type_map.find(ATgetArgument( ATgetFirst( list ),0)))
			 {
			   gsErrorMsg("Variable %T is already defined!\n", ATgetFirst( list ));
			   exit(1);
			 };
			 list = ATgetTail( list, 1 ) ;
		  }	;
		  
          list = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list);
		  n = ATgetLength( list );
		  for(int i = 0; i < n ; ++i ){
			 if (chan_type_direction_map.end() != chan_type_direction_map.find(ATgetArgument( ATgetFirst( list ),0)))
			 {
			   gsErrorMsg("Channel %T is already defined!\n", ATgetFirst( list ));
			   exit(1);
			 };
			 chan_type_direction_map[ATgetArgument(ATgetFirst( list ), 0)]=  make_pair( (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl), ATgetArgument(ATgetFirst( list ), 1) );
			 list = ATgetTail( list, 1 ) ;
		  }	;

		  list = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list);
          ATermList new_list = ATmakeList0();
		  while(!ATisEmpty(list))
          {
             ATermAppl one = (ATermAppl) gsMakeExpression(gsString2ATermAppl("1"), (ATermAppl) gsMakeType(gsString2ATermAppl("Nat")));
             new_list = ATinsert(new_list,(ATerm) gsMakeChannelTypedID( (ATermAppl) ATgetFirst(list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl), one) );
			 list = ATgetNext( list ) ;
		  }

		  safe_assign(((*yyvalp).list), new_list );
		  gsDebugMsg("ChannelDefinition: parsed VariableList \n %T\n", ((*yyvalp).list));
		;}
    break;

  case 42:
#line 599 "chiparser.yy"
    {
          gsDebugMsg("%s;%d\n",__FILE__,__LINE__);

          //Make sure that Expression is a number 
          UnaryTypeCheck( (ATermAppl) ATgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.appl),1), "Nat");
        
		  ATermList list = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.list);
		  int n = ATgetLength( list );
		  for(int i = 0; i < n ; ++i ){
			 if (var_type_map.end() != var_type_map.find(ATgetArgument( ATgetFirst( list ),0)))
			 {
			   gsErrorMsg("Variable %T is already defined!\n", ATgetFirst( list ));
			   exit(1);
			 };
			 list = ATgetTail( list, 1 ) ;
		  }	;
		  
          list = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.list);
		  n = ATgetLength( list );
		  for(int i = 0; i < n ; ++i ){
			 if (chan_type_direction_map.end() != chan_type_direction_map.find(ATgetArgument( ATgetFirst( list ),0)))
			 {
			   gsErrorMsg("Channel %T is already defined!\n", ATgetFirst( list ));
			   exit(1);
			 };
			 chan_type_direction_map[ATgetArgument(ATgetFirst( list ), 0)]=  make_pair( (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl), ATgetArgument(ATgetFirst( list ), 1) );
			 list = ATgetTail( list, 1 ) ;
		  }	;

		  list = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.list);
          ATermList new_list = ATmakeList0();
		  while(!ATisEmpty(list))
          {
             gsDebugMsg("%T",ATgetFirst(list));
             new_list = ATinsert(new_list,(ATerm) gsMakeChannelTypedID( (ATermAppl) ATgetFirst(list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.appl) ) );
			 list = ATgetNext( list ) ;
		  }

		  safe_assign(((*yyvalp).list), new_list );
		  gsDebugMsg("ChannelDefinition: parsed VariableList \n %T\n", ((*yyvalp).list));
		;}
    break;

  case 43:
#line 644 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("IdentifierChannelDefinition_csp: parsed \n  %T\n", ((*yyvalp).list));	
		;}
    break;

  case 44:
#line 648 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("IdentifierChannelDefinition_csp: parsed \n  %T\n", ((*yyvalp).list));	
		;}
    break;

  case 45:
#line 655 "chiparser.yy"
    {
          safe_assign(((*yyvalp).appl), gsMakeChannelID((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl), gsMakeNil()));
		  gsDebugMsg("IdentifierChannelDefinition: parsed \n %T\n", ((*yyvalp).appl));
        ;}
    break;

  case 47:
#line 675 "chiparser.yy"
    { 
          safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)  );
      	  gsDebugMsg("Type: parsed Type \n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 49:
#line 687 "chiparser.yy"
    { 
          safe_assign(((*yyvalp).appl), gsMakeType( gsString2ATermAppl( "Bool" ) ) );
      	  gsDebugMsg("BasicType: parsed Type \n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 50:
#line 692 "chiparser.yy"
    { 
          safe_assign(((*yyvalp).appl), gsMakeType( gsString2ATermAppl("Nat" ) ) );
      	  gsDebugMsg("BasicType: parsed Type \n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 51:
#line 697 "chiparser.yy"
    { 
          safe_assign(((*yyvalp).appl), gsMakeType( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl) ) );
      	  gsDebugMsg("BasicType: parsed Type \n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 52:
#line 707 "chiparser.yy"
    {
          safe_assign(((*yyvalp).appl), gsMakeListType((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("ContainerType: parsed Type \n  %T\n", ((*yyvalp).appl));

        ;}
    break;

  case 53:
#line 713 "chiparser.yy"
    {
          safe_assign(((*yyvalp).appl), gsMakeSetType((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("ContainerType: parsed Type \n  %T\n", ((*yyvalp).appl));

        ;}
    break;

  case 54:
#line 719 "chiparser.yy"
    { 
          ATermList list = ATinsert( (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.appl) ); 
          safe_assign(((*yyvalp).appl), gsMakeTupleType(ATreverse(list)));
      	  gsDebugMsg("ContainerType: parsed Type \n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 55:
#line 728 "chiparser.yy"
    { 
          safe_assign(((*yyvalp).list), ATmakeList1( (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)) );
      	  gsDebugMsg("ChannelDeclaration_csp: parsed formalparameter channel  \n  %T\n", ((*yyvalp).list));	
		;}
    break;

  case 56:
#line 733 "chiparser.yy"
    { 
          safe_assign(((*yyvalp).list), ATinsert( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl) ) );
      	  gsDebugMsg("BasicType: parsed Type \n  %T\n", ((*yyvalp).list));
		;}
    break;

  case 57:
#line 746 "chiparser.yy"
    { safe_assign(((*yyvalp).appl), gsMakeParenthesisedStat( (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("Statement: parsed \n  %T\n", ((*yyvalp).appl));	
		;}
    break;

  case 62:
#line 757 "chiparser.yy"
    { safe_assign(((*yyvalp).appl), gsMakeParStat( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("ModelStatement: parsed \n  %T\n", ((*yyvalp).appl));	
		;}
    break;

  case 66:
#line 774 "chiparser.yy"
    {
          ATermList ids = (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.list);
          ATermList exprs = (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.list);

          while(!ATisEmpty(ids))
          { 
            if (!ContainerTypeChecking((ATermAppl) ATgetArgument(ATgetFirst(ids), 1), (ATermAppl) ATgetArgument(ATgetFirst(exprs), 1)))
		    { 
              gsErrorMsg("Assignment failed: Incompatible Types Checking failed %T and %T\n", ids, exprs);
		      exit(1);
            }
            ids = ATgetNext(ids);
            exprs = ATgetNext(exprs);
          }  
 
          safe_assign(((*yyvalp).appl), gsMakeAssignmentStat((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.appl), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.list)), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.list)) ) );
      	  gsDebugMsg("AssignmentStatement: parsed \n  %T\n", ((*yyvalp).appl));	
		;}
    break;

  case 67:
#line 794 "chiparser.yy"
    { safe_assign(((*yyvalp).appl), gsMakeSkipStat( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl), gsMakeSkip() ));
      	  gsDebugMsg("AssignmentStatement: parsed \n  %T\n", ((*yyvalp).appl));	
		;}
    break;

  case 68:
#line 801 "chiparser.yy"
    { safe_assign(((*yyvalp).appl), gsMakeNil() );
		;}
    break;

  case 69:
#line 804 "chiparser.yy"
    { 
			/**
			  * Type Checking
			  *
			  **/
			if(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl),1) != gsMakeType(gsString2ATermAppl("Bool")))
				{
				  gsErrorMsg("OptGaurd failed: Incompatible Types Checking failed\n");
				  exit(1);
				};


			safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl) );
      	  	gsDebugMsg("OptGuard: parsed \n  %T\n", ((*yyvalp).appl));	
		;}
    break;

  case 70:
#line 822 "chiparser.yy"
    { safe_assign(((*yyvalp).appl), gsMakeNil() );
		;}
    break;

  case 71:
#line 825 "chiparser.yy"
    { safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl));
		  gsErrorMsg("OptChannel not yet implemented");
		  assert(false);
      	  gsDebugMsg("OptChannel: parsed \n  %T\n", ((*yyvalp).appl));	
		;}
    break;

  case 72:
#line 834 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATmakeList1( (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("Identifier_csp: parsed \n  %T\n", ((*yyvalp).list));	
		;}
    break;

  case 73:
#line 838 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("Identifier_csp: parsed \n  %T\n", ((*yyvalp).list));	
		;}
    break;

  case 74:
#line 845 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("Expression_csp: parsed \n  %T\n", ((*yyvalp).list));	
		;}
    break;

  case 75:
#line 849 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("Expression_csp: parsed \n  %T\n", ((*yyvalp).list));	
		;}
    break;

  case 76:
#line 856 "chiparser.yy"
    {
          //Check if $2 is properly typed
          //Check if $4 is properly typed
          //Check if $2 can send

          ATermAppl channel = (ATermAppl) ATgetArgument(ATgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.appl),0),0);   
          ATermAppl hash    = (ATermAppl) ATgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.appl),2); 
          gsDebugMsg("%T\n",(((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.appl));
          gsDebugMsg("%T\n",hash);

          safe_assign(((*yyvalp).appl), gsMakeSendStat((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.appl), channel, hash , ATreverse( (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.list)) ) );
      	  gsDebugMsg("parsed expression-element \n  %T\n", ((*yyvalp).appl));	
        ;}
    break;

  case 77:
#line 874 "chiparser.yy"
    {
          //Check if $2 is properly typed
          //Check if $4 is properly typed
          //Check if $2 can receive

          gsDebugMsg("%T",(((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.appl));
 
          ATermAppl channel = (ATermAppl) ATgetArgument(ATgetArgument( (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.appl),0),0);   
          ATermAppl hash    = (ATermAppl) ATgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.appl),2); 

          safe_assign(((*yyvalp).appl), gsMakeRecvStat((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.appl), channel, hash, ATreverse( (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.list)) ) );
      	  gsDebugMsg("parsed expression-element \n  %T\n", ((*yyvalp).appl));	
        ;}
    break;

  case 78:
#line 895 "chiparser.yy"
    {
        safe_assign(((*yyvalp).appl), gsMakeInstantiation((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), ATmakeList0()));
      ;}
    break;

  case 79:
#line 899 "chiparser.yy"
    {
        safe_assign(((*yyvalp).appl), gsMakeInstantiation((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.appl), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.list))));
      ;}
    break;

  case 80:
#line 906 "chiparser.yy"
    { safe_assign(((*yyvalp).appl), gsMakeSepStat( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed SEP statement \n  %T\n", ((*yyvalp).appl));	
		;}
    break;

  case 81:
#line 910 "chiparser.yy"
    { safe_assign(((*yyvalp).appl), gsMakeAltStat( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed ALT statement \n  %T\n", ((*yyvalp).appl));	
		;}
    break;

  case 82:
#line 921 "chiparser.yy"
    { 
          gsDebugMsg("%d\n", __LINE__);
      	  safe_assign(((*yyvalp).appl), gsMakeStarStat( (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed STAR statement \n  %T\n", ((*yyvalp).appl));	
		;}
    break;

  case 83:
#line 927 "chiparser.yy"
    {
          gsDebugMsg("%d\n", __LINE__);
          UnaryTypeCheck(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1), "Bool");
          safe_assign(((*yyvalp).appl), gsMakeGuardedStarStat( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed GuardedSTAR statement \n  %T\n", ((*yyvalp).appl));	
		;}
    break;

  case 84:
#line 937 "chiparser.yy"
    { safe_assign(((*yyvalp).appl), gsMakeDeltaStat((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl), gsMakeDelta()));
      	  gsDebugMsg("parsed deadlock statement \n  %T\n", ((*yyvalp).appl));	
		;}
    break;

  case 85:
#line 950 "chiparser.yy"
    { 
			/**
			  * Type Checking inherhit
			  *
			  **/	

 	  		safe_assign(((*yyvalp).appl), 
				gsMakeUnaryExpression( gsString2ATermAppl("()" ),
				ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl),1), 
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl) ) 
			);
      		gsDebugMsg("parsed Negation Expression's\n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 91:
#line 981 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed expression-element \n  %T\n", ((*yyvalp).list));	
		;}
    break;

  case 92:
#line 985 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed expression-element\n  %T\n", ((*yyvalp).list));	
		;}
    break;

  case 93:
#line 991 "chiparser.yy"
    {
		  /**  
		    * Lookup Identifier Type
		    *
		    * TODO: Add scope
		    *
		    **/
		  
		  // Determine if the expression is defined already 
		  if (var_type_map.end() == var_type_map.find( (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)))
		    {
		      gsErrorMsg("ExpressionIdentifier: Variable %T is not defined!\n", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl) );
		      exit(1);
		    };
		  
		  //Type the expression
 	  	  safe_assign(((*yyvalp).appl), 
			gsMakeExpression( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl), 
			  (ATermAppl) var_type_map[(ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)] 
			)
		  );
      	  gsDebugMsg("parsed Identifier's\n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 94:
#line 1018 "chiparser.yy"
    {
		  /**  
		    * Lookup Identifier Type
		    *
		    * TODO: Add scope
		    *
		    **/

          bool channel_exists = false;
          bool variable_exists = false;
          if (chan_type_direction_map.end() != chan_type_direction_map.find( (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)))
          {
             channel_exists = true;
             ATermAppl one = (ATermAppl) gsMakeExpression(gsString2ATermAppl("1"), (ATermAppl) gsMakeType(gsString2ATermAppl("Nat")));
              safe_assign(((*yyvalp).appl), 
                gsMakeChannelTypedID(
                  gsMakeChannelID((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl), gsMakeNil()),
                  (ATermAppl) chan_type_direction_map[(ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)].first,
                  one
                )
              );
            }

          // Determine if the expression is defined already 
          if (var_type_map.end() != var_type_map.find( (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)))
          {
              variable_exists = true;
              //Type the expression
              safe_assign(((*yyvalp).appl), 
              gsMakeExpression( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl), 
                (ATermAppl) var_type_map[(ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)] 
              )
             );
          }
 
          if(!channel_exists && !variable_exists)
          {
              gsErrorMsg("BasicExpression: Variable/Channel %T is not defined!\n", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl) );
              exit(1);
          }
  
          gsDebugMsg("BasicExpression: parsed \n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 95:
#line 1062 "chiparser.yy"
    {
		  /**  
		    * Lookup Identifier Type
		    *
		    * TODO: Add scope
		    *
		    **/

          bool channel_exists = false;
          bool variable_exists = false;
          if (chan_type_direction_map.end() != chan_type_direction_map.find( (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl)))
          {
            channel_exists = true;
          }
 
          if(!channel_exists)
          {
              gsErrorMsg("BasicExpression: Variable/Channel %T is not defined!\n", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl) );
              exit(1);
          }

          //Check if # of channel is a Natural number
          UnaryTypeCheck( (ATermAppl) ATgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1), "Nat");

          safe_assign(((*yyvalp).appl), 
            gsMakeChannelTypedID(
              gsMakeChannelID((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), gsMakeNil()),
              (ATermAppl) chan_type_direction_map[(ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl)].first,
              (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)  
            )
          );
  
          gsDebugMsg("BasicExpression: parsed \n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 96:
#line 1104 "chiparser.yy"
    { 
 	  		safe_assign(((*yyvalp).appl), 
				gsMakeExpression( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl), 
					gsMakeType( gsString2ATermAppl("Bool" )) 
				)
			);
      		gsDebugMsg("BooleanExpression: parsed \n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 97:
#line 1113 "chiparser.yy"
    { 
 	  		safe_assign(((*yyvalp).appl), 
				gsMakeExpression( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl), 
					gsMakeType( gsString2ATermAppl("Bool" )) 
				)
			);
      		gsDebugMsg("BooleanExpression: parsed \n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 98:
#line 1122 "chiparser.yy"
    { 
            UnaryTypeCheck(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl),1), "Bool");

 	  		safe_assign(((*yyvalp).appl), 
				gsMakeUnaryExpression( gsString2ATermAppl("!" ),
				gsMakeType( gsString2ATermAppl("Bool" ) ), 
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl) ) 
			);
      		gsDebugMsg("parsed Negation Expression's\n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 99:
#line 1133 "chiparser.yy"
    { 
            UnaryTypeCheck(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl),1), "Bool");

 	  		safe_assign(((*yyvalp).appl), 
				gsMakeUnaryExpression( gsString2ATermAppl("!" ),
				gsMakeType( gsString2ATermAppl("Bool" ) ), 
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl) ) 
			);
      		gsDebugMsg("parsed Negation Expression's\n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 100:
#line 1144 "chiparser.yy"
    { 
            BinTypeCheck(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1), ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1), "Bool");

 	  		safe_assign(((*yyvalp).appl), 
				gsMakeBinaryExpression( gsString2ATermAppl("&&" ),
				gsMakeType( gsString2ATermAppl("Bool" ) ),
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl) , 
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)  
				)
			);
      		gsDebugMsg("parsed Binary AND Expression's\n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 101:
#line 1157 "chiparser.yy"
    { 
            BinTypeCheck(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1), ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1), "Bool");

 	  		safe_assign(((*yyvalp).appl), 
				gsMakeBinaryExpression( gsString2ATermAppl("||" ),
				gsMakeType( gsString2ATermAppl("Bool" ) ),
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl) , 
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)  
				)
			);
      		gsDebugMsg("parsed Binary AND Expression's\n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 102:
#line 1170 "chiparser.yy"
    { 
            BinTypeCheck(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1), ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1), "Bool");

 	  		safe_assign(((*yyvalp).appl), 
				gsMakeBinaryExpression( gsString2ATermAppl("->" ),
				gsMakeType( gsString2ATermAppl("Bool" ) ),
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl) , 
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)  
				)
			);
      		gsDebugMsg("parsed Binary AND Expression's\n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 103:
#line 1186 "chiparser.yy"
    { 
 	  		safe_assign(((*yyvalp).appl), 
				gsMakeExpression( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl), 
					gsMakeType( gsString2ATermAppl("Nat" )) 
				)
			);
      		gsDebugMsg("parsed Expression's\n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 104:
#line 1195 "chiparser.yy"
    { 
            UnaryTypeCheck(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl),1), "Nat");

 	  		safe_assign(((*yyvalp).appl), 
				gsMakeUnaryExpression( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl), 
					gsMakeType( gsString2ATermAppl("Nat" ) ),
					(((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl) 
				)
			);
      		gsDebugMsg("parsed UnaryExpression's\n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 105:
#line 1207 "chiparser.yy"
    { 
            UnaryTypeCheck(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl),1), "Nat");

 	  		safe_assign(((*yyvalp).appl), 
				gsMakeUnaryExpression((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl), 
					gsMakeType( gsString2ATermAppl("Nat" ) ),
					(((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl)
				)
			);
      		gsDebugMsg("parsed UnaryExpression's\n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 106:
#line 1219 "chiparser.yy"
    { 
            BinTypeCheck(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1), ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1), "Nat");

 	  		safe_assign(((*yyvalp).appl), 
				gsMakeBinaryExpression( gsString2ATermAppl("^" ),
				gsMakeType( gsString2ATermAppl("Nat" ) ),
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl) , 
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)  
				) 
			);
      		gsDebugMsg("parsed Binary AND Expression's\n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 107:
#line 1232 "chiparser.yy"
    { 
            BinTypeCheck(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1), ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1), "Nat");

 	  		safe_assign(((*yyvalp).appl), 
				gsMakeBinaryExpression( gsString2ATermAppl("*" ),
				gsMakeType( gsString2ATermAppl("Nat" ) ),
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl) , 
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)  
				) 
			);
      		gsDebugMsg("parsed Binary AND Expression's\n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 108:
#line 1245 "chiparser.yy"
    { 
            BinTypeCheck(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1), ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1), "Nat");

 	  		safe_assign(((*yyvalp).appl), 
				gsMakeBinaryExpression( gsString2ATermAppl("/" ),
				gsMakeType( gsString2ATermAppl("Nat" ) ),
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl) , 
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)  
				) 
			);
      		gsDebugMsg("parsed Binary AND Expression's\n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 109:
#line 1258 "chiparser.yy"
    { 
            BinTypeCheck(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1), ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1), "Nat");

 	  		safe_assign(((*yyvalp).appl), 
				gsMakeBinaryExpression( gsString2ATermAppl("+" ),
				gsMakeType( gsString2ATermAppl("Nat" ) ),
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl) , 
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)  
				) 
			);
      		gsDebugMsg("parsed Binary AND Expression's\n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 110:
#line 1271 "chiparser.yy"
    { 
            BinTypeCheck(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1), ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1), "Nat");

 	  		safe_assign(((*yyvalp).appl), 
				gsMakeBinaryExpression( gsString2ATermAppl("-" ),
				gsMakeType( gsString2ATermAppl("Nat" ) ),
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl) , 
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)  
				) 
			);
      		gsDebugMsg("parsed Binary AND Expression's\n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 111:
#line 1284 "chiparser.yy"
    { 
            BinTypeCheck(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1), ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1), "Nat");

 	  		safe_assign(((*yyvalp).appl), 
				gsMakeBinaryExpression( gsString2ATermAppl("MOD" ),
				gsMakeType( gsString2ATermAppl("Nat" ) ),
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl) , 
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)  
				) 
			);
      		gsDebugMsg("parsed Binary AND Expression's\n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 112:
#line 1297 "chiparser.yy"
    { 
            BinTypeCheck(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1), ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1), "Nat");

 	  		safe_assign(((*yyvalp).appl), 
				gsMakeBinaryExpression( gsString2ATermAppl("DIV" ),
				gsMakeType( gsString2ATermAppl("Nat" ) ),
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl) , 
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)  
				) 
			);
      		gsDebugMsg("parsed Binary AND Expression's\n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 113:
#line 1310 "chiparser.yy"
    { 
            BinTypeCheck(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1), ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1), "Nat");

 	  		safe_assign(((*yyvalp).appl), 
				gsMakeBinaryExpression( gsString2ATermAppl("MIN" ),
				gsMakeType( gsString2ATermAppl("Nat" ) ),
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl) , 
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)  
				) 
			);
      		gsDebugMsg("parsed Binary AND Expression's\n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 114:
#line 1323 "chiparser.yy"
    { 
            BinTypeCheck(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1), ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1), "Nat");

 	  		safe_assign(((*yyvalp).appl), 
				gsMakeBinaryExpression( gsString2ATermAppl("MAX" ),
				gsMakeType( gsString2ATermAppl("Nat" ) ),
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl) , 
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)  
				) 
			);
      		gsDebugMsg("parsed Binary AND Expression's\n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 115:
#line 1336 "chiparser.yy"
    { 
            BinTypeCheck(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1), ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1), "Nat");

 	  		safe_assign(((*yyvalp).appl), 
				gsMakeBinaryExpression( gsString2ATermAppl("<" ),
				gsMakeType( gsString2ATermAppl("Nat" ) ),
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl) , 
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)  
				) 
			);
      		gsDebugMsg("parsed Binary AND Expression's\n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 116:
#line 1349 "chiparser.yy"
    { 
            BinTypeCheck(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1), ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1), "Nat");

 	  		safe_assign(((*yyvalp).appl), 
				gsMakeBinaryExpression( gsString2ATermAppl(">" ),
				gsMakeType( gsString2ATermAppl("Nat" ) ),
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl) , 
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)  
				) 
			);
      		gsDebugMsg("parsed Binary AND Expression's\n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 117:
#line 1362 "chiparser.yy"
    { 
            BinTypeCheck(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1), ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1), "Nat");

 	  		safe_assign(((*yyvalp).appl), 
				gsMakeBinaryExpression( gsString2ATermAppl("<=" ),
				gsMakeType( gsString2ATermAppl("Nat" ) ),
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl) , 
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)  
				) 
			);
      		gsDebugMsg("parsed Binary AND Expression's\n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 118:
#line 1375 "chiparser.yy"
    { 
            BinTypeCheck(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1), ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1), "Nat");

 	  		safe_assign(((*yyvalp).appl), 
				gsMakeBinaryExpression( gsString2ATermAppl(">=" ),
				gsMakeType( gsString2ATermAppl("Nat" ) ),
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl) , 
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)  
				) 
			);
      		gsDebugMsg("parsed Binary AND Expression's\n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 119:
#line 1391 "chiparser.yy"
    {
			/**
			  * Type Checking
			  *
			  **/	
			if(  !(ContainerTypeChecking(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1),  ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1)))
              )
				{
				  gsErrorMsg("BoolNatIntExpression: Incompatible Types Checking failed\n");
				  exit(1);
				};
		
			safe_assign(((*yyvalp).appl), gsMakeBinaryExpression( (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl),  
					gsMakeType( gsString2ATermAppl("Bool" )), 
			(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      		gsDebugMsg("BoolNatIntExpression parsed: \n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 120:
#line 1409 "chiparser.yy"
    { 
			/**
			  * Type Checking
			  *
			  **/	
			if(  !((ContainerTypeChecking(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1),  ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1)))
              && (strcmp(ATgetName(ATgetAFun(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1))), "ListType") == 0 ))
              )
				{
				  gsErrorMsg("BoolNatIntExpression: Incompatible Types Checking failed\n");
				  exit(1);
				};

 	  		safe_assign(((*yyvalp).appl), gsMakeBinaryExpression( (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl),  
					gsMakeType( gsString2ATermAppl("Bool" )), 
			(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      		gsDebugMsg("BoolNatIntExpression parsed: \n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 121:
#line 1431 "chiparser.yy"
    {
        safe_assign(((*yyvalp).appl), ((*yyvalp).appl));
      ;}
    break;

  case 122:
#line 1435 "chiparser.yy"
    { 
			/**
			  * Type Checking
			  *
			  **/	
			if(  !((ContainerTypeChecking(gsMakeListType(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1)),  ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1)))
              && (strcmp(ATgetName(ATgetAFun(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1))), "ListType") == 0 ))
              )
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};

 	  		safe_assign(((*yyvalp).appl), gsMakeBinaryListExpression( (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl),  
					gsMakeType( gsString2ATermAppl("Bool" )), 
			(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      		gsDebugMsg("ListExpression parsed: \n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 123:
#line 1454 "chiparser.yy"
    { 
			/**
			  * Type Checking
			  *
			  **/	
			if(  !((ContainerTypeChecking(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1),  ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1)))
              && (strcmp(ATgetName(ATgetAFun(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1))), "ListType") == 0 ))
              )
				{
				  gsErrorMsg("Concatination failed: Incompatible Types Checking failed:\n %T and %T\n", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl));
				  exit(1);
				};

 	  		safe_assign(((*yyvalp).appl), gsMakeBinaryListExpression( (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl),  
					ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1), 
			(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      		gsDebugMsg("ListExpression parsed: \n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 124:
#line 1473 "chiparser.yy"
    { 
			/**
			  * Type Checking
			  *
			  **/	
			if(  !((ContainerTypeChecking(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1),  ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1)))
              && (strcmp(ATgetName(ATgetAFun(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1))), "ListType") == 0 ))
              )
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};

 	  		safe_assign(((*yyvalp).appl), gsMakeBinaryListExpression( (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl),  
					ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1), 
			(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      		gsDebugMsg("ListExpression parsed: \n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 126:
#line 1500 "chiparser.yy"
    {
			if(!(strcmp(ATgetName(ATgetAFun(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl),1))), "ListType") == 0 ))
				{
				  gsErrorMsg("Functions: %T cannot used on %T", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl));
				  exit(1);
				};

 	  		safe_assign(((*yyvalp).appl), gsMakeFunction( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.appl),  
					gsMakeType( gsString2ATermAppl("Nat" )), 
			(((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl)));
      		gsDebugMsg("Functions parsed: \n  %T\n", ((*yyvalp).appl));
      ;}
    break;

  case 127:
#line 1513 "chiparser.yy"
    {
			if(!(strcmp(ATgetName(ATgetAFun(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl),1))), "ListType") == 0 ))
				{
				  gsErrorMsg("Functions: %T cannot used on %T", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl));
				  exit(1);
				};

 	  		safe_assign(((*yyvalp).appl), gsMakeFunction( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.appl),  
				  (ATermAppl) ATgetArgument(ATgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl),1),0), 
			(((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl)));
      		gsDebugMsg("Functions parsed: \n  %T\n", ((*yyvalp).appl));
      ;}
    break;

  case 128:
#line 1526 "chiparser.yy"
    {
            gsDebugMsg("R:%d\n",__LINE__);
			if(!(strcmp(ATgetName(ATgetAFun(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl),1))), "ListType") == 0 ))
				{
				  gsErrorMsg("Functions: %T cannot used on %T", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl));
				  exit(1);
				};

 	  		safe_assign(((*yyvalp).appl), gsMakeFunction( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.appl),  
			     (ATermAppl) ATgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl),1), 
			(((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl)));
      		gsDebugMsg("Functions parsed: \n  %T\n", ((*yyvalp).appl));
      ;}
    break;

  case 129:
#line 1540 "chiparser.yy"
    {
            gsDebugMsg("R:%d\n",__LINE__);
			if(!(strcmp(ATgetName(ATgetAFun(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl),1))), "ListType") == 0 ))
				{
				  gsErrorMsg("Functions: %T cannot used on %T", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl));
				  exit(1);
				};

 	  		safe_assign(((*yyvalp).appl), gsMakeFunction( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.appl),  
				  (ATermAppl) ATgetArgument(ATgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl),1),0), 
			(((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl)));
      		gsDebugMsg("Functions parsed: \n  %T\n", ((*yyvalp).appl));
      ;}
    break;

  case 130:
#line 1554 "chiparser.yy"
    {
            gsDebugMsg("R:%d\n",__LINE__);
			if(!(strcmp(ATgetName(ATgetAFun(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl),1))), "ListType") == 0 ))
				{
				  gsErrorMsg("Functions: %T cannot used on %T", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl));
				  exit(1);
				};

 	  		safe_assign(((*yyvalp).appl), gsMakeFunction( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.appl),  
			       (ATermAppl) ATgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl),1), 
			(((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl)));
      		gsDebugMsg("Functions parsed: \n  %T\n", ((*yyvalp).appl));
      ;}
    break;

  case 131:
#line 1568 "chiparser.yy"
    {
            gsDebugMsg("R:%d\n",__LINE__);
			if(!(strcmp(ATgetName(ATgetAFun(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (6))].yystate.yysemantics.yysval.appl),1))), "ListType") == 0 ))
				{
				  gsErrorMsg("Functions: %T cannot used on %T\n", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (6))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (6))].yystate.yysemantics.yysval.appl));
				  exit(1);
				};

			if(! (ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (6))].yystate.yysemantics.yysval.appl),1) == gsMakeType( gsString2ATermAppl("Nat" ) ) ) )
				{
				  gsErrorMsg("Functions: %T cannot used on 2nd argument %T\n", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (6))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (6))].yystate.yysemantics.yysval.appl));
				  gsErrorMsg("Type checking failed\n");
				  exit(1);
				};

 	  		safe_assign(((*yyvalp).appl), gsMakeFunction2( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (6))].yystate.yysemantics.yysval.appl),  
			       (ATermAppl) ATgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (6))].yystate.yysemantics.yysval.appl),1), 
			       (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (6))].yystate.yysemantics.yysval.appl), 
                   (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (6))].yystate.yysemantics.yysval.appl)));
      		gsDebugMsg("Functions parsed: \n%T\n", ((*yyvalp).appl));
      ;}
    break;

  case 132:
#line 1590 "chiparser.yy"
    {
            gsDebugMsg("R:%d\n",__LINE__);
			if(!(strcmp(ATgetName(ATgetAFun(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (6))].yystate.yysemantics.yysval.appl),1))), "ListType") == 0 ))
				{
				  gsErrorMsg("Functions: %T cannot used on %T\n", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (6))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (6))].yystate.yysemantics.yysval.appl));
				  exit(1);
				};

			if(! (ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (6))].yystate.yysemantics.yysval.appl),1) == gsMakeType( gsString2ATermAppl("Nat" ) ) ) )
				{
				  gsErrorMsg("Functions: %T cannot used on 2nd argument %T\n", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (6))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (6))].yystate.yysemantics.yysval.appl));
				  gsErrorMsg("Type checking failed\n");
				  exit(1);
				};

 	  		safe_assign(((*yyvalp).appl), gsMakeFunction2( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (6))].yystate.yysemantics.yysval.appl),  
			       (ATermAppl) ATgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (6))].yystate.yysemantics.yysval.appl),1), 
			       (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (6))].yystate.yysemantics.yysval.appl), 
                   (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (6))].yystate.yysemantics.yysval.appl)));
      		gsDebugMsg("Functions parsed: \n%T\n", ((*yyvalp).appl));
      ;}
    break;

  case 133:
#line 1613 "chiparser.yy"
    {
            gsDebugMsg("R:%d\n",__LINE__);
			if(!(strcmp(ATgetName(ATgetAFun(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl),1))), "ListType") == 0 ))
				{
				  gsErrorMsg("Functions: %T cannot used on %T", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl));
				  exit(1);
				};

 	  		safe_assign(((*yyvalp).appl), gsMakeFunction( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.appl),  
			       (ATermAppl) ATgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl),1), 
			(((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl)));
      		gsDebugMsg("Functions parsed: \n  %T\n", ((*yyvalp).appl));
      ;}
    break;

  case 134:
#line 1627 "chiparser.yy"
    {
        gsErrorMsg("%T is not supported", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.appl));
        exit(1);
      ;}
    break;

  case 135:
#line 1635 "chiparser.yy"
    {
          safe_assign(((*yyvalp).appl), gsMakeListLiteral( ATmakeList0(), gsMakeListType(gsMakeType(gsMakeNil()))));
      ;}
    break;

  case 136:
#line 1639 "chiparser.yy"
    {
      	  gsDebugMsg("Entering ListLiteral\n");
          ATerm type; 
		  ATermList to_process = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.list);
		  while(!ATisEmpty(to_process))
          {
             ATerm elementType = ATgetArgument(ATgetFirst(to_process),1);
             if (ATgetLength(to_process) == ATgetLength((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.list)))
             {
               type = elementType;
             }
             gsDebugMsg("%T\n",ATgetFirst(to_process));
             if (type != elementType )
             {
               gsErrorMsg("ListLiteral contains mixed types %T and %T\n"
                         , type, elementType);
               exit(1);
             }
			 to_process = ATgetNext( to_process) ;
		  }
          safe_assign(((*yyvalp).appl), gsMakeListLiteral( ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.list)), gsMakeListType((ATermAppl) type)));
      	  gsDebugMsg("ListLiteral parsed: \n  %T\n", ((*yyvalp).appl));
      ;}
    break;


/* Line 930 of glr.c.  */
#line 3343 "chiparser.cpp"
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


#line 1689 "chiparser.yy"


void BinTypeCheck(ATermAppl arg1, ATermAppl arg2, std::string type)
{
    if(arg1 != arg2)
        {
          gsErrorMsg("BinTypeCheck: Incompatible Types Checking failed\n");
          exit(1);
        };
    if(arg1 != gsMakeType(gsString2ATermAppl(type.c_str())))
        {
          gsErrorMsg("Expected type ", type.c_str());
          exit(1);
        };
  return;
}

void UnaryTypeCheck(ATermAppl arg1, std::string type)
{
    gsDebugMsg("%s;%d\n",__FILE__,__LINE__);
    gsDebugMsg("arg1: %T\n", arg1);
    ATermAppl arg2 = gsMakeType(gsString2ATermAppl(type.c_str()));
    gsDebugMsg("arg2: %T\n", arg2);

    if( arg1 != arg2 )
        {
          gsErrorMsg("UnaryTypeCheck: Incompatible Type, expected %s\n", type.c_str());
          exit(1);
        };
  return;
}

bool ContainerTypeChecking(ATermAppl arg1, ATermAppl arg2)
{
  gsDebugMsg("%s;%d\n",__FILE__,__LINE__);
  if(arg1 == arg2)
  {
    return true;
  }
  
  gsDebugMsg("ContainerTypeChecking: %T, %T\n",arg1, arg2);
  if((strcmp(ATgetName(ATgetAFun(arg1)), ATgetName(ATgetAFun(arg2)))==0)  
     && (strcmp(ATgetName(ATgetAFun(arg1)), "ListType") == 0 ))
    {
      if(((ATermAppl) ATgetArgument(arg2,0) == gsMakeType(gsMakeNil())) || 
         ((ATermAppl) ATgetArgument(arg1,0) == gsMakeType(gsMakeNil()))
        )
      {
        return true;
      }
      return ContainerTypeChecking((ATermAppl) ATgetArgument(arg1,0), ATermAppl (ATgetArgument(arg2,0))); 
    }

  if(  (strcmp(ATgetName(ATgetAFun(arg1)),ATgetName(ATgetAFun(arg2)))==0) 
    && (strcmp(ATgetName(ATgetAFun(arg1)), "Type") == 0 )
    )
  {
    if(arg1 == arg2)
    { 
      return true;
    } else {
      return false;
    }
  } 
  return false;
}    


