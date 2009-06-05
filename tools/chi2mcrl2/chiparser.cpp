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



#include "chiparser.h"

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
#define YYLAST   2568

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  90
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  60
/* YYNRULES -- Number of rules.  */
#define YYNRULES  155
/* YYNRULES -- Number of states.  */
#define YYNSTATES  306
/* YYMAXRHS -- Maximum number of symbols on right-hand side of rule.  */
#define YYMAXRHS 7
/* YYMAXLEFT -- Maximum number of symbols to the left of a handle
   accessed by $0, $-1, etc., in any rule.  */
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
       0,     0,     3,     6,    13,    17,    23,    25,    27,    30,
      37,    45,    47,    51,    57,    59,    61,    63,    67,    70,
      73,    75,    77,    81,    83,    87,    89,    93,    95,    99,
     102,   105,   109,   111,   115,   119,   125,   127,   131,   134,
     137,   139,   143,   147,   153,   155,   159,   161,   163,   167,
     169,   171,   173,   175,   177,   179,   183,   187,   193,   195,
     199,   203,   205,   207,   209,   211,   215,   217,   219,   221,
     223,   226,   232,   236,   237,   240,   241,   244,   246,   250,
     252,   256,   261,   265,   270,   274,   278,   283,   287,   291,
     294,   298,   301,   305,   307,   309,   311,   313,   315,   317,
     319,   321,   323,   325,   327,   331,   333,   335,   339,   341,
     343,   346,   349,   353,   357,   361,   363,   366,   369,   373,
     377,   381,   385,   389,   393,   397,   401,   405,   409,   413,
     417,   421,   425,   429,   433,   439,   442,   446,   450,   454,
     458,   463,   465,   469,   473,   475,   480,   485,   490,   495,
     500,   507,   514,   519,   524,   527
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const short int yyrhs[] =
{
      91,     0,    -1,    92,    95,    -1,     6,   100,    34,    35,
      27,    93,    -1,    22,   122,    94,    -1,    22,   101,    24,
     122,    94,    -1,    23,    -1,    96,    -1,    95,    96,    -1,
      97,   100,    34,    35,    27,    98,    -1,    97,   100,    34,
     106,    35,    27,    98,    -1,     3,    -1,    22,   121,    99,
      -1,    22,   101,    24,   121,    99,    -1,    23,    -1,    20,
      -1,   102,    -1,   101,    26,   102,    -1,     7,   103,    -1,
       9,   113,    -1,    10,    -1,   104,    -1,   103,    26,   104,
      -1,   108,    -1,   108,    27,   135,    -1,   108,    -1,   105,
      26,   108,    -1,   107,    -1,   106,    26,   107,    -1,     7,
     105,    -1,     9,   109,    -1,   128,    14,   117,    -1,   110,
      -1,   109,    26,   110,    -1,   111,    14,   117,    -1,   111,
      14,   135,    82,   117,    -1,   112,    -1,   111,    26,   112,
      -1,   100,    57,    -1,   100,    58,    -1,   114,    -1,   113,
      26,   114,    -1,   115,    14,   117,    -1,   115,    14,   135,
      82,   117,    -1,   116,    -1,   115,    26,   116,    -1,   100,
      -1,   118,    -1,    34,   117,    35,    -1,   119,    -1,    16,
      -1,    17,    -1,    19,    -1,    18,    -1,    15,    -1,    66,
     117,    67,    -1,    32,   117,    33,    -1,    34,   120,    26,
     117,    35,    -1,   117,    -1,   120,    26,   117,    -1,    34,
     121,    35,    -1,   123,    -1,   133,    -1,   132,    -1,   131,
      -1,   122,    12,   122,    -1,   125,    -1,   130,    -1,   124,
      -1,   134,    -1,    87,   135,    -1,   126,   127,   136,    28,
     129,    -1,   126,   127,    11,    -1,    -1,   135,    38,    -1,
      -1,   135,    14,    -1,   100,    -1,   128,    26,   100,    -1,
     135,    -1,   129,    26,   135,    -1,   126,   135,    58,   129,
      -1,   126,   135,    58,    -1,   126,   135,    57,   129,    -1,
     126,   135,    57,    -1,   100,    34,    35,    -1,   100,    34,
     129,    35,    -1,   121,    25,   121,    -1,   121,    13,   121,
      -1,    63,   121,    -1,   135,    64,   121,    -1,   126,    47,
      -1,    34,   135,    35,    -1,   138,    -1,   139,    -1,   142,
      -1,   141,    -1,   140,    -1,   143,    -1,   147,    -1,   146,
      -1,   144,    -1,   145,    -1,   137,    -1,   136,    26,   137,
      -1,   100,    -1,   100,    -1,   100,    46,   135,    -1,    45,
      -1,    44,    -1,    39,   135,    -1,    58,   135,    -1,   135,
      36,   135,    -1,   135,    37,   135,    -1,   135,    48,   135,
      -1,    41,    -1,    30,   135,    -1,    29,   135,    -1,   135,
      56,   135,    -1,   135,    63,   135,    -1,   135,    88,   135,
      -1,   135,    55,   135,    -1,   135,    54,   135,    -1,   135,
      53,   135,    -1,   135,    52,   135,    -1,   135,    80,   135,
      -1,   135,    81,   135,    -1,   135,    51,   135,    -1,   135,
      50,   135,    -1,   135,    30,   135,    -1,   135,    29,   135,
      -1,   135,    27,   135,    -1,   135,    49,   135,    -1,   135,
      70,   135,    -1,    34,   129,    26,   135,    35,    -1,    32,
      33,    -1,    32,   129,    33,    -1,   135,    83,   135,    -1,
     135,    85,   135,    -1,   135,    84,   135,    -1,    86,    34,
     135,    35,    -1,   149,    -1,   135,    69,   135,    -1,   135,
      68,   135,    -1,   148,    -1,    75,    34,   135,    35,    -1,
      71,    34,   135,    35,    -1,    72,    34,   135,    35,    -1,
      73,    34,   135,    35,    -1,    74,    34,   135,    35,    -1,
      76,    34,   135,    26,   135,    35,    -1,    77,    34,   135,
      26,   135,    35,    -1,    78,    34,   135,    35,    -1,    79,
      34,   135,    35,    -1,    66,    67,    -1,    66,   129,    67,
      -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,   166,   166,   176,   184,   188,   195,   202,   206,   213,
     230,   246,   254,   258,   265,   275,   282,   286,   293,   298,
     303,   308,   312,   329,   347,   372,   376,   393,   397,   404,
     409,   417,   468,   472,   488,   531,   578,   582,   589,   594,
     612,   616,   623,   665,   712,   716,   723,   742,   743,   749,
     755,   760,   765,   770,   771,   781,   787,   793,   802,   807,
     820,   824,   825,   826,   830,   831,   838,   839,   840,   844,
     848,   858,   879,   888,   890,   909,   911,   920,   924,   931,
     935,   942,   957,   969,   984,  1003,  1007,  1014,  1018,  1029,
    1035,  1047,  1060,  1075,  1076,  1077,  1078,  1079,  1080,  1081,
    1082,  1083,  1084,  1094,  1098,  1104,  1132,  1177,  1270,  1279,
    1288,  1299,  1310,  1323,  1336,  1352,  1361,  1373,  1385,  1398,
    1411,  1424,  1437,  1450,  1463,  1476,  1489,  1502,  1515,  1531,
    1596,  1674,  1703,  1736,  1778,  1799,  1803,  1828,  1848,  1868,
    1888,  1906,  1910,  1930,  1954,  1958,  1972,  1986,  2001,  2016,
    2031,  2055,  2080,  2094,  2103,  2107
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "PROC", "MODEL_DEF", "ENUM", "MODEL",
  "VAR", "CONST", "CHAN", "VAL", "SKIP", "BARS", "ALT", "COLON", "TYPE",
  "BOOL", "NAT", "REAL", "VOID", "ID", "TIME", "BP", "EP", "PROC_SEP",
  "SEP", "COMMA", "IS", "ASSIGNMENT", "MINUS", "PLUS", "GG", "LBRACE",
  "RBRACE", "LBRACKET", "RBRACKET", "AND", "OR", "GUARD", "NOT", "OLD",
  "NUMBER", "INT", "REALNUMBER", "TRUE", "FALSE", "DOT", "DEADLOCK",
  "IMPLIES", "NOTEQUAL", "GEQ", "LEQ", "MAX", "MIN", "DIV", "MOD", "POWER",
  "RECV", "EXCLAMATION", "SENDRECV", "RECVSEND", "SSEND", "RRECV", "STAR",
  "GUARD_REP", "DERIVATIVE", "SQLBRACKET", "SQRBRACKET", "LSUBTRACT",
  "CONCAT", "IN", "HEAD", "TAIL", "RHEAD", "RTAIL", "LENGTH", "TAKE",
  "DROP", "SORT", "INSERT", "LESS", "GREATER", "HASH", "UNION", "SUB",
  "INTERSECTION", "PICK", "DELAY", "DIVIDE", "SEND", "$accept",
  "ChiProgram", "ModelDefinition", "ModelBody", "ModelCloseScope",
  "ProcessDefinitions", "ProcessDefinition", "ProcOpenScope",
  "ProcessBody", "ProcCloseScope", "Identifier", "LocalVariables_csp",
  "LocalVariables", "IdentifierTypeExpression_csp",
  "IdentifierTypeExpression", "IdentifierType_csp", "FormalParameter_csp",
  "FormalParameter", "IdentifierType", "ChannelDeclaration_csp",
  "ChannelDeclaration", "IdentifierChannelDeclaration_csp",
  "IdentifierChannelDeclaration", "ChannelDefinition_csp",
  "ChannelDefinition", "IdentifierChannelDefinition_csp",
  "IdentifierChannelDefinition", "Type", "BasicType", "ContainerType",
  "Type_csp", "Statement", "ModelStatement", "BasicStatement",
  "DelayStatement", "AssignmentStatement", "OptGuard", "OptChannel",
  "Identifier_csp", "Expression_csp", "CommStatement", "Instantiation",
  "BinaryStatement", "UnaryStatement", "AdvancedStatement", "Expression",
  "ExpressionIdentier_csp", "ExpressionIdentifier", "BasicExpression",
  "BooleanExpression", "NatIntExpression", "PlusExpression",
  "MinusExpression", "EqualityExpression", "MemberTest",
  "RecordExpression", "SetExpression", "ListExpression", "Functions",
  "ListLiteral", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    90,    91,    92,    93,    93,    94,    95,    95,    96,
      96,    97,    98,    98,    99,   100,   101,   101,   102,   102,
     102,   103,   103,   104,   104,   105,   105,   106,   106,   107,
     107,   108,   109,   109,   110,   110,   111,   111,   112,   112,
     113,   113,   114,   114,   115,   115,   116,   117,   117,   117,
     118,   118,   118,   118,   118,   119,   119,   119,   120,   120,
     121,   121,   121,   121,   122,   122,   123,   123,   123,   123,
     124,   125,   125,   126,   126,   127,   127,   128,   128,   129,
     129,   130,   130,   130,   130,   131,   131,   132,   132,   133,
     133,   134,   135,   135,   135,   135,   135,   135,   135,   135,
     135,   135,   135,   136,   136,   137,   138,   138,   139,   139,
     139,   139,   139,   139,   139,   140,   140,   140,   140,   140,
     140,   140,   140,   140,   140,   140,   140,   140,   140,   141,
     142,   143,   143,   144,   145,   146,   146,   146,   146,   146,
     146,   147,   147,   147,   147,   148,   148,   148,   148,   148,
     148,   148,   148,   148,   149,   149
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     2,     6,     3,     5,     1,     1,     2,     6,
       7,     1,     3,     5,     1,     1,     1,     3,     2,     2,
       1,     1,     3,     1,     3,     1,     3,     1,     3,     2,
       2,     3,     1,     3,     3,     5,     1,     3,     2,     2,
       1,     3,     3,     5,     1,     3,     1,     1,     3,     1,
       1,     1,     1,     1,     1,     3,     3,     5,     1,     3,
       3,     1,     1,     1,     1,     3,     1,     1,     1,     1,
       2,     5,     3,     0,     2,     0,     2,     1,     3,     1,
       3,     4,     3,     4,     3,     3,     4,     3,     3,     2,
       3,     2,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     3,     1,     1,     3,     1,     1,
       2,     2,     3,     3,     3,     1,     2,     2,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     5,     2,     3,     3,     3,     3,
       4,     1,     3,     3,     1,     4,     4,     4,     4,     4,
       6,     6,     4,     4,     2,     3
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
       0,     0,     0,     0,     0,     0
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error.  */
static const unsigned char yydefact[] =
{
       0,     0,     0,     0,    15,     0,     1,    11,     2,     7,
       0,     0,     8,     0,     0,     0,     0,     0,     0,     0,
       0,    27,     0,     3,    77,    29,    25,     0,     0,    30,
      32,     0,    36,     0,     0,     0,     0,     0,    20,     0,
       0,    16,     0,    64,     0,     0,     0,    38,    39,     0,
       0,     0,    73,     9,    28,     0,    18,    21,    23,    46,
      19,    40,     0,    44,     0,     0,     0,     0,     6,     4,
      26,    54,    50,    51,    53,    52,     0,     0,     0,    31,
      47,    49,    78,    33,     0,     0,     0,     0,     0,   115,
     109,   108,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   106,    34,     0,    93,    94,    97,
      96,    95,    98,   101,   102,   100,    99,   144,   141,    37,
       0,    73,    73,     0,     0,     0,     0,    61,    68,    66,
      75,    67,    63,    62,    69,     0,    10,     0,     0,     0,
       0,     0,     0,    85,     0,    79,     0,    17,    65,     0,
      58,     0,     0,   117,   116,   135,     0,     0,    79,   110,
     111,   154,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      79,    89,    70,    73,    73,    14,    73,    12,    91,     0,
       0,    74,    73,    22,    24,    41,    42,     0,    45,     0,
      86,     5,    56,    48,     0,    55,   136,     0,    92,   155,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     107,   131,   130,   129,   112,   113,   114,   132,   128,   127,
     124,   123,   122,   121,   118,   119,   143,   142,   133,   125,
     126,    35,   137,   139,   138,   120,    60,     0,    88,    87,
      72,   105,     0,   103,    76,    84,    82,    90,     0,    80,
      59,    80,   146,   147,   148,   149,   145,     0,     0,   152,
     153,   140,    13,     0,     0,    83,    81,    43,    57,   134,
       0,     0,   104,    71,   150,   151
};

/* YYPDEFGOTO[NTERM-NUM].  */
static const short int yydefgoto[] =
{
      -1,     2,     3,    23,    69,     8,     9,    10,    53,   207,
     104,    40,    41,    56,    57,    25,    20,    21,    58,    29,
      30,    31,    32,    60,    61,    62,    63,   149,    80,    81,
     151,   126,    42,   127,   128,   129,   130,   209,    27,   157,
     131,    43,   132,   133,   134,   145,   272,   273,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -120
static const short int yypact[] =
{
      38,    27,    52,    59,  -120,    21,  -120,  -120,    59,  -120,
      27,    35,  -120,    39,    53,    -3,    54,    27,    27,    56,
     -11,  -120,    49,  -120,  -120,    69,  -120,    -6,    73,    76,
    -120,    12,  -120,    85,    87,    88,    27,    27,  -120,    84,
      77,  -120,    10,  -120,    27,    -5,    27,  -120,  -120,    27,
     463,    27,   223,  -120,  -120,    85,    97,  -120,   105,  -120,
     107,  -120,    14,  -120,   883,    27,   103,    27,  -120,  -120,
    -120,  -120,  -120,  -120,  -120,  -120,    -5,    -5,    -5,  -120,
    -120,  -120,  -120,  -120,  1049,  1049,   333,   463,  1049,  -120,
    -120,  -120,  1049,   398,   100,   101,   108,   109,   111,   112,
     113,   114,   115,   147,    91,  -120,  1343,  -120,  -120,  -120,
    -120,  -120,  -120,  -120,  -120,  -120,  -120,  -120,  -120,  -120,
     906,   523,   583,   966,  1049,    90,    40,  -120,  -120,  -120,
    1026,  -120,  -120,  -120,  -120,  1283,  -120,    27,  1049,    27,
     463,    27,  1049,  -120,    16,  2183,    10,  -120,   127,   153,
     106,   161,   121,   166,   166,  -120,    -8,   163,  1403,    36,
    2243,  -120,   -21,  1049,  1049,  1049,  1049,  1049,  1049,  1049,
    1049,  1049,  1049,  1049,  1049,  1049,  1049,  1049,  1049,  1049,
    1049,  1049,  1049,  1049,  1049,  1049,  1049,  1049,  1049,  1049,
    1049,  1049,  1049,  1049,    -5,  1049,  1049,  1049,  1049,     6,
    1223,    65,  2183,   643,   703,  -120,   763,  -120,  -120,    66,
     276,  -120,   823,  -120,  2183,  -120,  -120,  1463,  -120,  1049,
    -120,  -120,  -120,  -120,    -5,  -120,  -120,  1049,  -120,  -120,
    1523,  1583,  1643,  1703,  1763,  1103,  1163,  1823,  1883,  1943,
    2303,  2436,   166,   166,  2348,  2392,  2480,  2436,  2436,  2436,
    -120,  -120,    36,    36,    36,    36,    15,    15,    15,  2436,
    2436,  -120,  -120,  -120,  -120,    36,  -120,    40,   179,    65,
    -120,  -120,    96,  -120,  -120,  1049,  1049,  -120,    -5,  2183,
     159,  2003,  -120,  -120,  -120,  -120,  -120,  1049,  1049,  -120,
    -120,  -120,  -120,    27,  1049,   169,   169,  -120,  -120,  -120,
    2063,  2123,  -120,   169,  -120,  -120
};

/* YYPGOTO[NTERM-NUM].  */
static const short int yypgoto[] =
{
    -120,  -120,  -120,  -120,    50,  -120,   189,  -120,   145,   -66,
      -1,   150,   137,  -120,    67,  -120,  -120,   171,   -10,  -120,
     158,  -120,   160,  -120,    70,  -120,    74,     4,  -120,  -120,
    -120,  -119,    61,  -120,  -120,  -120,  -120,  -120,  -120,   -63,
    -120,  -120,  -120,  -120,  -120,   -13,  -120,   -83,  -120,  -120,
    -120,  -120,  -120,  -120,  -120,  -120,  -120,  -120,  -120,  -120
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const short int yytable[] =
{
       5,   144,   199,   201,    17,   219,    18,    26,    45,    13,
      71,    72,    73,    74,    75,    34,    24,    28,   219,   204,
      46,    39,    67,   156,    35,   226,    50,    76,   140,    77,
     162,   206,    19,    68,    70,    24,    59,   106,    51,   135,
     141,   266,   219,    24,     1,    82,   229,     4,    28,    79,
      28,   220,     6,   204,   105,    11,    36,   156,    37,    38,
     162,    78,     7,   205,    39,   206,    39,   183,   184,     4,
      14,   153,   154,    15,   158,   159,    22,   270,   204,   160,
      16,   150,   152,    33,   267,   268,     4,   269,   183,   184,
     206,   150,   187,   277,    17,    44,    18,   152,   195,   196,
     197,    65,    49,    66,   189,   190,   191,    52,   200,   135,
      36,   202,    37,    38,   203,    55,    66,   210,    64,   195,
     196,   197,   293,   137,   294,   214,   146,   217,   148,   158,
      47,    48,   138,   139,   163,   164,    24,   173,    59,    67,
      59,   223,   165,   166,   216,   167,   168,   169,   170,   171,
     230,   231,   232,   233,   234,   235,   236,   237,   238,   239,
     240,   241,   242,   243,   244,   245,   246,   247,   248,   249,
     250,   251,   252,   253,   254,   255,   256,   257,   258,   259,
     260,   172,   262,   263,   264,   265,   222,   224,   225,   227,
     135,   135,   204,   135,   298,   219,   221,    12,   261,   135,
     136,   292,   125,   147,   213,    54,   279,    83,   271,   215,
     302,   119,   295,   296,   281,   218,     0,     0,   183,   184,
     185,   186,   187,     0,     0,     0,     0,     0,   280,   188,
      36,   303,    37,    38,   189,   190,   191,     0,     0,     0,
       0,     0,     0,     4,     0,     0,     0,     0,     0,   195,
     196,   197,    84,    85,   198,   120,     0,   121,     0,     0,
       0,     0,    88,     0,    89,     0,     0,    90,    91,     0,
       0,     0,     0,     0,   300,   301,     0,     0,     0,     0,
       0,    92,   297,     0,     0,     0,   122,     0,     0,   123,
     274,     0,   271,     0,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   174,     0,   175,   176,     0,     0,   103,
     124,     0,   177,   178,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   275,   276,     0,     0,     0,     0,   188,
       0,     0,     0,     0,   189,   190,   191,     0,    71,    72,
      73,    74,    75,     4,     0,     0,   192,   193,     0,   195,
     196,   197,    84,    85,   198,    86,   155,    87,     0,     0,
       0,     0,    88,     0,    89,     0,     0,    90,    91,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    92,     0,     0,     0,     0,     0,     0,     0,    93,
       0,     0,     0,     0,    94,    95,    96,    97,    98,    99,
     100,   101,   102,    71,    72,    73,    74,    75,     4,   103,
       0,     0,     0,     0,     0,     0,     0,    84,    85,     0,
      86,     0,    87,     0,     0,     0,     0,    88,     0,    89,
       0,     0,    90,    91,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    92,     0,     0,     0,
       0,     0,     0,     0,    93,   161,     0,     0,     0,    94,
      95,    96,    97,    98,    99,   100,   101,   102,    71,    72,
      73,    74,    75,     4,   103,     0,     0,     0,     0,     0,
       0,     0,    84,    85,     0,    86,     0,    87,     0,     0,
       0,     0,    88,     0,    89,     0,     0,    90,    91,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    92,     0,     0,     0,     0,     0,     0,     0,    93,
       0,     0,     0,     0,    94,    95,    96,    97,    98,    99,
     100,   101,   102,     4,     0,     0,     0,     0,     0,   103,
       0,     0,    84,    85,     0,   120,     0,   121,     0,     0,
       0,     0,    88,     0,    89,     0,     0,    90,    91,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    92,     0,     0,     0,     0,   122,     0,     0,   123,
       0,     0,     0,     0,    94,    95,    96,    97,    98,    99,
     100,   101,   102,     4,     0,     0,     0,     0,     0,   103,
     124,     0,    84,    85,     0,   120,     0,   121,     0,     0,
       0,     0,    88,     0,    89,     0,     0,    90,    91,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    92,     0,     0,     0,     0,   122,     0,     0,   123,
       0,     0,     0,     0,    94,    95,    96,    97,    98,    99,
     100,   101,   102,     4,     0,     0,     0,     0,     0,   103,
     124,     0,    84,    85,     0,   120,     0,   121,     0,     0,
       0,     0,    88,     0,    89,     0,     0,    90,    91,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    92,     0,     0,     0,     0,   122,     0,     0,   123,
       0,     0,     0,     0,    94,    95,    96,    97,    98,    99,
     100,   101,   102,     4,     0,     0,     0,     0,     0,   103,
     124,     0,    84,    85,     0,   120,     0,   121,     0,     0,
       0,     0,    88,     0,    89,     0,     0,    90,    91,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    92,     0,     0,     0,     0,   122,     0,     0,   123,
       0,     0,     0,     0,    94,    95,    96,    97,    98,    99,
     100,   101,   102,     4,     0,     0,     0,     0,     0,   103,
     124,     0,    84,    85,     0,   120,     0,   121,     0,     0,
       0,     0,    88,     0,    89,     0,     0,    90,    91,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    92,     0,     0,     0,     0,   122,     0,     0,   123,
       0,     0,     0,     0,    94,    95,    96,    97,    98,    99,
     100,   101,   102,     4,     0,     0,     0,     0,     0,   103,
     124,     0,    84,    85,     0,   120,     0,   121,     0,     0,
       0,     0,    88,     0,    89,     0,     0,    90,    91,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    92,     0,     0,     0,     0,   122,     0,     0,   123,
       0,     0,     0,     0,    94,    95,    96,    97,    98,    99,
     100,   101,   102,     4,     0,     0,     0,     0,     0,   103,
     124,     0,    84,    85,     0,   120,     0,   142,   143,     0,
       0,     0,    88,     0,    89,     0,     4,    90,    91,     0,
       0,     0,     0,     0,     0,    84,    85,     0,   120,   155,
     142,    92,     0,     0,     0,    88,     0,    89,     0,   123,
      90,    91,     0,     0,    94,    95,    96,    97,    98,    99,
     100,   101,   102,     0,    92,     0,     0,     0,     0,   103,
       0,     0,   123,     0,     0,     0,     0,    94,    95,    96,
      97,    98,    99,   100,   101,   102,     4,     0,     0,     0,
       0,     0,   103,     0,     0,    84,    85,     0,   120,     0,
     142,     0,     0,     0,     0,    88,     0,    89,     0,     0,
      90,    91,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    92,     0,     0,     0,     0,     0,
       0,     0,   123,   161,     0,     0,     0,    94,    95,    96,
      97,    98,    99,   100,   101,   102,     4,     0,     0,     0,
       0,     0,   103,     0,     0,    84,    85,     0,   120,     0,
     142,     0,     0,     0,     0,    88,     0,    89,     0,     4,
      90,    91,     0,   208,     0,     0,     0,     0,    84,    85,
       0,   120,     0,   142,    92,     0,     0,     0,    88,     0,
      89,     0,   123,    90,    91,     0,     0,    94,    95,    96,
      97,    98,    99,   100,   101,   102,     0,    92,     0,     0,
       0,     0,   103,     0,     0,   123,     0,     0,     0,     0,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   287,
     174,     0,   175,   176,     0,   103,     0,     0,     0,   177,
     178,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   179,   180,   181,   182,   183,   184,   185,   186,   187,
       0,     0,     0,     0,     0,     0,   188,     0,     0,     0,
       0,   189,   190,   191,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   192,   193,     0,   195,   196,   197,   288,
     174,   198,   175,   176,     0,     0,     0,     0,     0,   177,
     178,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   179,   180,   181,   182,   183,   184,   185,   186,   187,
       0,     0,     0,     0,     0,     0,   188,     0,     0,     0,
       0,   189,   190,   191,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   192,   193,     0,   195,   196,   197,     0,
     174,   198,   175,   176,     0,     0,     0,     0,   228,   177,
     178,   211,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   179,   180,   181,   182,   183,   184,   185,   186,   187,
       0,     0,     0,     0,     0,     0,   188,   212,     0,     0,
       0,   189,   190,   191,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   192,   193,     0,   195,   196,   197,     0,
     174,   198,   175,   176,     0,     0,     0,     0,     0,   177,
     178,   211,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   179,   180,   181,   182,   183,   184,   185,   186,   187,
       0,     0,     0,     0,     0,     0,   188,   212,     0,     0,
       0,   189,   190,   191,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   192,   193,     0,   195,   196,   197,     0,
     174,   198,   175,   176,     0,     0,     0,     0,     0,   177,
     178,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   179,   180,   181,   182,   183,   184,   185,   186,   187,
       0,     0,     0,     0,     0,     0,   188,     0,     0,     0,
       0,   189,   190,   191,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   192,   193,   194,   195,   196,   197,     0,
     174,   198,   175,   176,     0,     0,     0,     0,   228,   177,
     178,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   179,   180,   181,   182,   183,   184,   185,   186,   187,
       0,     0,     0,     0,     0,     0,   188,     0,     0,     0,
       0,   189,   190,   191,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   192,   193,     0,   195,   196,   197,     0,
     174,   198,   175,   176,     0,     0,     0,     0,     0,   177,
     178,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   179,   180,   181,   182,   183,   184,   185,   186,   187,
       0,     0,     0,     0,     0,     0,   188,     0,     0,     0,
       0,   189,   190,   191,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   192,   193,   278,   195,   196,   197,     0,
     174,   198,   175,   176,     0,     0,     0,     0,   282,   177,
     178,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   179,   180,   181,   182,   183,   184,   185,   186,   187,
       0,     0,     0,     0,     0,     0,   188,     0,     0,     0,
       0,   189,   190,   191,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   192,   193,     0,   195,   196,   197,     0,
     174,   198,   175,   176,     0,     0,     0,     0,   283,   177,
     178,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   179,   180,   181,   182,   183,   184,   185,   186,   187,
       0,     0,     0,     0,     0,     0,   188,     0,     0,     0,
       0,   189,   190,   191,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   192,   193,     0,   195,   196,   197,     0,
     174,   198,   175,   176,     0,     0,     0,     0,   284,   177,
     178,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   179,   180,   181,   182,   183,   184,   185,   186,   187,
       0,     0,     0,     0,     0,     0,   188,     0,     0,     0,
       0,   189,   190,   191,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   192,   193,     0,   195,   196,   197,     0,
     174,   198,   175,   176,     0,     0,     0,     0,   285,   177,
     178,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   179,   180,   181,   182,   183,   184,   185,   186,   187,
       0,     0,     0,     0,     0,     0,   188,     0,     0,     0,
       0,   189,   190,   191,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   192,   193,     0,   195,   196,   197,     0,
     174,   198,   175,   176,     0,     0,     0,     0,   286,   177,
     178,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   179,   180,   181,   182,   183,   184,   185,   186,   187,
       0,     0,     0,     0,     0,     0,   188,     0,     0,     0,
       0,   189,   190,   191,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   192,   193,     0,   195,   196,   197,     0,
     174,   198,   175,   176,     0,     0,     0,     0,   289,   177,
     178,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   179,   180,   181,   182,   183,   184,   185,   186,   187,
       0,     0,     0,     0,     0,     0,   188,     0,     0,     0,
       0,   189,   190,   191,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   192,   193,     0,   195,   196,   197,     0,
     174,   198,   175,   176,     0,     0,     0,     0,   290,   177,
     178,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   179,   180,   181,   182,   183,   184,   185,   186,   187,
       0,     0,     0,     0,     0,     0,   188,     0,     0,     0,
       0,   189,   190,   191,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   192,   193,     0,   195,   196,   197,     0,
     174,   198,   175,   176,     0,     0,     0,     0,   291,   177,
     178,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   179,   180,   181,   182,   183,   184,   185,   186,   187,
       0,     0,     0,     0,     0,     0,   188,     0,     0,     0,
       0,   189,   190,   191,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   192,   193,     0,   195,   196,   197,     0,
     174,   198,   175,   176,     0,     0,     0,     0,   299,   177,
     178,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   179,   180,   181,   182,   183,   184,   185,   186,   187,
       0,     0,     0,     0,     0,     0,   188,     0,     0,     0,
       0,   189,   190,   191,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   192,   193,     0,   195,   196,   197,     0,
     174,   198,   175,   176,     0,     0,     0,     0,   304,   177,
     178,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   179,   180,   181,   182,   183,   184,   185,   186,   187,
       0,     0,     0,     0,     0,     0,   188,     0,     0,     0,
       0,   189,   190,   191,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   192,   193,     0,   195,   196,   197,     0,
     174,   198,   175,   176,     0,     0,     0,     0,   305,   177,
     178,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   179,   180,   181,   182,   183,   184,   185,   186,   187,
       0,     0,     0,     0,     0,     0,   188,     0,     0,     0,
       0,   189,   190,   191,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   192,   193,     0,   195,   196,   197,     0,
     174,   198,   175,   176,     0,     0,     0,     0,     0,   177,
     178,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   179,   180,   181,   182,   183,   184,   185,   186,   187,
       0,     0,     0,     0,     0,     0,   188,     0,     0,     0,
       0,   189,   190,   191,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   192,   193,     0,   195,   196,   197,     0,
     174,   198,   175,   176,     0,     0,     0,     0,     0,   177,
     178,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   179,   180,   181,   182,   183,   184,   185,   186,   187,
       0,     0,     0,     0,     0,     0,   188,     0,     0,     0,
       0,   189,   190,   191,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   192,   193,     0,   195,   196,   197,     0,
     174,   198,   175,   176,     0,     0,     0,     0,     0,   177,
     178,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   179,   180,   181,   182,   183,   184,   185,   186,   187,
       0,     0,     0,     0,     0,     0,   188,     0,     0,     0,
       0,   189,   190,   191,     0,   174,     0,   175,   176,     0,
       0,     0,     0,   192,   193,   178,   195,   196,   197,     0,
       0,   198,     0,     0,     0,     0,   179,   180,   181,   182,
     183,   184,   185,   186,   187,     0,     0,     0,     0,     0,
       0,   188,     0,     0,     0,     0,   189,   190,   191,   174,
       0,   175,   176,     0,     0,     0,     0,     0,   192,   193,
       0,   195,   196,   197,     0,     0,   198,     0,     0,     0,
     179,   180,   181,   182,   183,   184,   185,   186,   187,     0,
       0,     0,     0,     0,     0,   188,     0,     0,     0,     0,
     189,   190,   191,    -1,     0,   175,   176,     0,     0,     0,
       0,     0,   192,   193,     0,   195,   196,   197,     0,     0,
     198,     0,     0,     0,     0,    -1,    -1,    -1,   183,   184,
     185,   186,   187,     0,     0,     0,     0,     0,     0,   188,
       0,     0,     0,     0,   189,   190,   191,   174,     0,   175,
     176,     0,     0,     0,     0,     0,    -1,    -1,     0,   195,
     196,   197,     0,     0,   198,     0,     0,     0,     0,   180,
     181,   182,   183,   184,   185,   186,   187,     0,     0,     0,
       0,     0,     0,   188,     0,     0,     0,     0,   189,   190,
     191,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     192,   193,     0,   195,   196,   197,     0,     0,   198
};

/* YYCONFLP[YYPACT[STATE-NUM]] -- Pointer into YYCONFL of start of
   list of conflicting reductions corresponding to action entry for
   state STATE-NUM in yytable.  0 means no conflicts.  The list in
   yyconfl is terminated by a rule number of 0.  */
static const unsigned short int yyconflp[] =
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
       0,     0,     0,     0,     0,     1,     0,     0,     0,     0,
       0,     0,     3,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    47,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    49,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     5,     0,     0,     0,     0,     0,     0,
       0,     0,     7,     9,     0,    11,     0,    13,     0,     0,
       0,     0,    15,     0,    17,     0,     0,    19,    21,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    23,     0,     0,     0,     0,     0,     0,     0,    25,
       0,     0,     0,     0,    27,    29,    31,    33,    35,    37,
      39,    41,    43,     0,     0,     0,     0,     0,     0,    45,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    51,     0,     0,     0,     0,     0,     0,
       0,     0,    53,    55,     0,    57,     0,    59,     0,     0,
       0,     0,    61,     0,    63,     0,     0,    65,    67,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    69,     0,     0,     0,     0,     0,     0,     0,    71,
       0,     0,     0,     0,    73,    75,    77,    79,    81,    83,
      85,    87,    89,    93,     0,     0,     0,     0,     0,    91,
       0,     0,    95,    97,     0,    99,     0,   101,     0,     0,
       0,     0,   103,     0,   105,     0,     0,   107,   109,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   111,     0,     0,     0,     0,     0,     0,     0,   113,
       0,     0,     0,     0,   115,   117,   119,   121,   123,   125,
     127,   129,   131,   185,     0,     0,     0,     0,     0,   133,
       0,     0,   187,   189,     0,   191,     0,   193,     0,     0,
       0,     0,   195,     0,   197,     0,     0,   199,   201,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   203,     0,     0,     0,     0,     0,     0,     0,   205,
       0,     0,     0,     0,   207,   209,   211,   213,   215,   217,
     219,   221,   223,   227,     0,     0,     0,     0,     0,   225,
       0,     0,   229,   231,     0,   233,     0,   235,     0,     0,
       0,     0,   237,     0,   239,     0,     0,   241,   243,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   245,     0,     0,     0,     0,     0,     0,     0,   247,
       0,     0,     0,     0,   249,   251,   253,   255,   257,   259,
     261,   263,   265,   269,     0,     0,     0,     0,     0,   267,
       0,     0,   271,   273,     0,   275,     0,   277,     0,     0,
       0,     0,   279,     0,   281,     0,     0,   283,   285,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   287,     0,     0,     0,     0,     0,     0,     0,   289,
       0,     0,     0,     0,   291,   293,   295,   297,   299,   301,
     303,   305,   307,   311,     0,     0,     0,     0,     0,   309,
       0,     0,   313,   315,     0,   317,     0,   319,     0,     0,
       0,     0,   321,     0,   323,     0,     0,   325,   327,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   329,     0,     0,     0,     0,     0,     0,     0,   331,
       0,     0,     0,     0,   333,   335,   337,   339,   341,   343,
     345,   347,   349,     0,     0,     0,     0,     0,     0,   351,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   135,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     137,     0,   139,   141,     0,     0,     0,     0,     0,   143,
     145,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   147,   149,   151,   153,   155,   157,   159,   161,   163,
       0,     0,     0,     0,     0,     0,   165,     0,     0,     0,
       0,   167,   169,   171,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   173,   175,     0,   177,   179,   181,     0,
     353,   183,   355,   357,     0,     0,     0,     0,     0,   359,
     361,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   363,   365,   367,   369,   371,   373,   375,   377,   379,
       0,     0,     0,     0,     0,     0,   381,     0,     0,     0,
       0,   383,   385,   387,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   389,   391,     0,   393,   395,   397,     0,
       0,   399,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
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

/* YYCONFL[I] -- lists of conflicting rule numbers, each terminated by
   0, pointed into by YYCONFLP.  */
static const short int yyconfl[] =
{
       0,    29,     0,    30,     0,    73,     0,    73,     0,    73,
       0,    73,     0,    73,     0,    73,     0,    73,     0,    73,
       0,    73,     0,    73,     0,    73,     0,    73,     0,    73,
       0,    73,     0,    73,     0,    73,     0,    73,     0,    73,
       0,    73,     0,    73,     0,    73,     0,    18,     0,    19,
       0,    73,     0,    73,     0,    73,     0,    73,     0,    73,
       0,    73,     0,    73,     0,    73,     0,    73,     0,    73,
       0,    73,     0,    73,     0,    73,     0,    73,     0,    73,
       0,    73,     0,    73,     0,    73,     0,    73,     0,    73,
       0,    73,     0,    73,     0,    73,     0,    73,     0,    73,
       0,    73,     0,    73,     0,    73,     0,    73,     0,    73,
       0,    73,     0,    73,     0,    73,     0,    73,     0,    73,
       0,    73,     0,    73,     0,    73,     0,    73,     0,    73,
       0,    73,     0,    73,     0,    75,     0,   111,     0,   111,
       0,   111,     0,   111,     0,   111,     0,   111,     0,   111,
       0,   111,     0,   111,     0,   111,     0,   111,     0,   111,
       0,   111,     0,   111,     0,   111,     0,   111,     0,   111,
       0,   111,     0,   111,     0,   111,     0,   111,     0,   111,
       0,   111,     0,   111,     0,    73,     0,    73,     0,    73,
       0,    73,     0,    73,     0,    73,     0,    73,     0,    73,
       0,    73,     0,    73,     0,    73,     0,    73,     0,    73,
       0,    73,     0,    73,     0,    73,     0,    73,     0,    73,
       0,    73,     0,    73,     0,    73,     0,    73,     0,    73,
       0,    73,     0,    73,     0,    73,     0,    73,     0,    73,
       0,    73,     0,    73,     0,    73,     0,    73,     0,    73,
       0,    73,     0,    73,     0,    73,     0,    73,     0,    73,
       0,    73,     0,    73,     0,    73,     0,    73,     0,    73,
       0,    73,     0,    73,     0,    73,     0,    73,     0,    73,
       0,    73,     0,    73,     0,    73,     0,    73,     0,    73,
       0,    73,     0,    73,     0,    73,     0,    73,     0,    73,
       0,    73,     0,    73,     0,    73,     0,    73,     0,    73,
       0,    73,     0,    73,     0,    73,     0,    73,     0,    73,
       0,    73,     0,    73,     0,    73,     0,    73,     0,    73,
       0,    73,     0,    73,     0,    73,     0,    73,     0,    73,
       0,    73,     0,    73,     0,    73,     0,    73,     0,    73,
       0,    73,     0,   107,     0,   107,     0,   107,     0,   107,
       0,   107,     0,   107,     0,   107,     0,   107,     0,   107,
       0,   107,     0,   107,     0,   107,     0,   107,     0,   107,
       0,   107,     0,   107,     0,   107,     0,   107,     0,   107,
       0,   107,     0,   107,     0,   107,     0,   107,     0,   107,
       0
};

static const short int yycheck[] =
{
       1,    64,   121,   122,     7,    26,     9,    17,    14,    10,
      15,    16,    17,    18,    19,    26,    17,    18,    26,    13,
      26,    22,    12,    86,    35,    33,    14,    32,    14,    34,
      93,    25,    35,    23,    44,    36,    37,    50,    26,    52,
      26,    35,    26,    44,     6,    46,    67,    20,    49,    45,
      51,    35,     0,    13,    50,    34,     7,   120,     9,    10,
     123,    66,     3,    23,    65,    25,    67,    52,    53,    20,
      35,    84,    85,    34,    87,    88,    22,    11,    13,    92,
      27,    77,    78,    27,   203,   204,    20,   206,    52,    53,
      25,    87,    56,   212,     7,    26,     9,    93,    83,    84,
      85,    24,    26,    26,    68,    69,    70,    22,   121,   122,
       7,   124,     9,    10,    24,    27,    26,   130,    34,    83,
      84,    85,    26,    26,    28,   138,    65,   140,    67,   142,
      57,    58,    27,    26,    34,    34,   137,    46,   139,    12,
     141,    35,    34,    34,   140,    34,    34,    34,    34,    34,
     163,   164,   165,   166,   167,   168,   169,   170,   171,   172,
     173,   174,   175,   176,   177,   178,   179,   180,   181,   182,
     183,   184,   185,   186,   187,   188,   189,   190,   191,   192,
     193,    34,   195,   196,   197,   198,    33,    26,    67,    26,
     203,   204,    13,   206,    35,    26,   146,     8,   194,   212,
      55,   267,    52,    66,   137,    34,   219,    49,   209,   139,
     293,    51,   275,   276,   227,   141,    -1,    -1,    52,    53,
      54,    55,    56,    -1,    -1,    -1,    -1,    -1,   224,    63,
       7,   294,     9,    10,    68,    69,    70,    -1,    -1,    -1,
      -1,    -1,    -1,    20,    -1,    -1,    -1,    -1,    -1,    83,
      84,    85,    29,    30,    88,    32,    -1,    34,    -1,    -1,
      -1,    -1,    39,    -1,    41,    -1,    -1,    44,    45,    -1,
      -1,    -1,    -1,    -1,   287,   288,    -1,    -1,    -1,    -1,
      -1,    58,   278,    -1,    -1,    -1,    63,    -1,    -1,    66,
      14,    -1,   293,    -1,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    27,    -1,    29,    30,    -1,    -1,    86,
      87,    -1,    36,    37,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    -1,    -1,    -1,    -1,    63,
      -1,    -1,    -1,    -1,    68,    69,    70,    -1,    15,    16,
      17,    18,    19,    20,    -1,    -1,    80,    81,    -1,    83,
      84,    85,    29,    30,    88,    32,    33,    34,    -1,    -1,
      -1,    -1,    39,    -1,    41,    -1,    -1,    44,    45,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    66,
      -1,    -1,    -1,    -1,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    15,    16,    17,    18,    19,    20,    86,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    29,    30,    -1,
      32,    -1,    34,    -1,    -1,    -1,    -1,    39,    -1,    41,
      -1,    -1,    44,    45,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    58,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    66,    67,    -1,    -1,    -1,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    15,    16,
      17,    18,    19,    20,    86,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    29,    30,    -1,    32,    -1,    34,    -1,    -1,
      -1,    -1,    39,    -1,    41,    -1,    -1,    44,    45,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    66,
      -1,    -1,    -1,    -1,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    20,    -1,    -1,    -1,    -1,    -1,    86,
      -1,    -1,    29,    30,    -1,    32,    -1,    34,    -1,    -1,
      -1,    -1,    39,    -1,    41,    -1,    -1,    44,    45,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    58,    -1,    -1,    -1,    -1,    63,    -1,    -1,    66,
      -1,    -1,    -1,    -1,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    20,    -1,    -1,    -1,    -1,    -1,    86,
      87,    -1,    29,    30,    -1,    32,    -1,    34,    -1,    -1,
      -1,    -1,    39,    -1,    41,    -1,    -1,    44,    45,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    58,    -1,    -1,    -1,    -1,    63,    -1,    -1,    66,
      -1,    -1,    -1,    -1,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    20,    -1,    -1,    -1,    -1,    -1,    86,
      87,    -1,    29,    30,    -1,    32,    -1,    34,    -1,    -1,
      -1,    -1,    39,    -1,    41,    -1,    -1,    44,    45,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    58,    -1,    -1,    -1,    -1,    63,    -1,    -1,    66,
      -1,    -1,    -1,    -1,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    20,    -1,    -1,    -1,    -1,    -1,    86,
      87,    -1,    29,    30,    -1,    32,    -1,    34,    -1,    -1,
      -1,    -1,    39,    -1,    41,    -1,    -1,    44,    45,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    58,    -1,    -1,    -1,    -1,    63,    -1,    -1,    66,
      -1,    -1,    -1,    -1,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    20,    -1,    -1,    -1,    -1,    -1,    86,
      87,    -1,    29,    30,    -1,    32,    -1,    34,    -1,    -1,
      -1,    -1,    39,    -1,    41,    -1,    -1,    44,    45,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    58,    -1,    -1,    -1,    -1,    63,    -1,    -1,    66,
      -1,    -1,    -1,    -1,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    20,    -1,    -1,    -1,    -1,    -1,    86,
      87,    -1,    29,    30,    -1,    32,    -1,    34,    -1,    -1,
      -1,    -1,    39,    -1,    41,    -1,    -1,    44,    45,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    58,    -1,    -1,    -1,    -1,    63,    -1,    -1,    66,
      -1,    -1,    -1,    -1,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    20,    -1,    -1,    -1,    -1,    -1,    86,
      87,    -1,    29,    30,    -1,    32,    -1,    34,    35,    -1,
      -1,    -1,    39,    -1,    41,    -1,    20,    44,    45,    -1,
      -1,    -1,    -1,    -1,    -1,    29,    30,    -1,    32,    33,
      34,    58,    -1,    -1,    -1,    39,    -1,    41,    -1,    66,
      44,    45,    -1,    -1,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    -1,    58,    -1,    -1,    -1,    -1,    86,
      -1,    -1,    66,    -1,    -1,    -1,    -1,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    20,    -1,    -1,    -1,
      -1,    -1,    86,    -1,    -1,    29,    30,    -1,    32,    -1,
      34,    -1,    -1,    -1,    -1,    39,    -1,    41,    -1,    -1,
      44,    45,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    58,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    66,    67,    -1,    -1,    -1,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    20,    -1,    -1,    -1,
      -1,    -1,    86,    -1,    -1,    29,    30,    -1,    32,    -1,
      34,    -1,    -1,    -1,    -1,    39,    -1,    41,    -1,    20,
      44,    45,    -1,    47,    -1,    -1,    -1,    -1,    29,    30,
      -1,    32,    -1,    34,    58,    -1,    -1,    -1,    39,    -1,
      41,    -1,    66,    44,    45,    -1,    -1,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    -1,    58,    -1,    -1,
      -1,    -1,    86,    -1,    -1,    66,    -1,    -1,    -1,    -1,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    26,
      27,    -1,    29,    30,    -1,    86,    -1,    -1,    -1,    36,
      37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      -1,    -1,    -1,    -1,    -1,    -1,    63,    -1,    -1,    -1,
      -1,    68,    69,    70,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    80,    81,    -1,    83,    84,    85,    26,
      27,    88,    29,    30,    -1,    -1,    -1,    -1,    -1,    36,
      37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      -1,    -1,    -1,    -1,    -1,    -1,    63,    -1,    -1,    -1,
      -1,    68,    69,    70,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    80,    81,    -1,    83,    84,    85,    -1,
      27,    88,    29,    30,    -1,    -1,    -1,    -1,    35,    36,
      37,    38,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      -1,    -1,    -1,    -1,    -1,    -1,    63,    64,    -1,    -1,
      -1,    68,    69,    70,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    80,    81,    -1,    83,    84,    85,    -1,
      27,    88,    29,    30,    -1,    -1,    -1,    -1,    -1,    36,
      37,    38,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      -1,    -1,    -1,    -1,    -1,    -1,    63,    64,    -1,    -1,
      -1,    68,    69,    70,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    80,    81,    -1,    83,    84,    85,    -1,
      27,    88,    29,    30,    -1,    -1,    -1,    -1,    -1,    36,
      37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      -1,    -1,    -1,    -1,    -1,    -1,    63,    -1,    -1,    -1,
      -1,    68,    69,    70,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    80,    81,    82,    83,    84,    85,    -1,
      27,    88,    29,    30,    -1,    -1,    -1,    -1,    35,    36,
      37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      -1,    -1,    -1,    -1,    -1,    -1,    63,    -1,    -1,    -1,
      -1,    68,    69,    70,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    80,    81,    -1,    83,    84,    85,    -1,
      27,    88,    29,    30,    -1,    -1,    -1,    -1,    -1,    36,
      37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      -1,    -1,    -1,    -1,    -1,    -1,    63,    -1,    -1,    -1,
      -1,    68,    69,    70,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    80,    81,    82,    83,    84,    85,    -1,
      27,    88,    29,    30,    -1,    -1,    -1,    -1,    35,    36,
      37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      -1,    -1,    -1,    -1,    -1,    -1,    63,    -1,    -1,    -1,
      -1,    68,    69,    70,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    80,    81,    -1,    83,    84,    85,    -1,
      27,    88,    29,    30,    -1,    -1,    -1,    -1,    35,    36,
      37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      -1,    -1,    -1,    -1,    -1,    -1,    63,    -1,    -1,    -1,
      -1,    68,    69,    70,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    80,    81,    -1,    83,    84,    85,    -1,
      27,    88,    29,    30,    -1,    -1,    -1,    -1,    35,    36,
      37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      -1,    -1,    -1,    -1,    -1,    -1,    63,    -1,    -1,    -1,
      -1,    68,    69,    70,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    80,    81,    -1,    83,    84,    85,    -1,
      27,    88,    29,    30,    -1,    -1,    -1,    -1,    35,    36,
      37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      -1,    -1,    -1,    -1,    -1,    -1,    63,    -1,    -1,    -1,
      -1,    68,    69,    70,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    80,    81,    -1,    83,    84,    85,    -1,
      27,    88,    29,    30,    -1,    -1,    -1,    -1,    35,    36,
      37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      -1,    -1,    -1,    -1,    -1,    -1,    63,    -1,    -1,    -1,
      -1,    68,    69,    70,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    80,    81,    -1,    83,    84,    85,    -1,
      27,    88,    29,    30,    -1,    -1,    -1,    -1,    35,    36,
      37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      -1,    -1,    -1,    -1,    -1,    -1,    63,    -1,    -1,    -1,
      -1,    68,    69,    70,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    80,    81,    -1,    83,    84,    85,    -1,
      27,    88,    29,    30,    -1,    -1,    -1,    -1,    35,    36,
      37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      -1,    -1,    -1,    -1,    -1,    -1,    63,    -1,    -1,    -1,
      -1,    68,    69,    70,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    80,    81,    -1,    83,    84,    85,    -1,
      27,    88,    29,    30,    -1,    -1,    -1,    -1,    35,    36,
      37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      -1,    -1,    -1,    -1,    -1,    -1,    63,    -1,    -1,    -1,
      -1,    68,    69,    70,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    80,    81,    -1,    83,    84,    85,    -1,
      27,    88,    29,    30,    -1,    -1,    -1,    -1,    35,    36,
      37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      -1,    -1,    -1,    -1,    -1,    -1,    63,    -1,    -1,    -1,
      -1,    68,    69,    70,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    80,    81,    -1,    83,    84,    85,    -1,
      27,    88,    29,    30,    -1,    -1,    -1,    -1,    35,    36,
      37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      -1,    -1,    -1,    -1,    -1,    -1,    63,    -1,    -1,    -1,
      -1,    68,    69,    70,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    80,    81,    -1,    83,    84,    85,    -1,
      27,    88,    29,    30,    -1,    -1,    -1,    -1,    35,    36,
      37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      -1,    -1,    -1,    -1,    -1,    -1,    63,    -1,    -1,    -1,
      -1,    68,    69,    70,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    80,    81,    -1,    83,    84,    85,    -1,
      27,    88,    29,    30,    -1,    -1,    -1,    -1,    -1,    36,
      37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      -1,    -1,    -1,    -1,    -1,    -1,    63,    -1,    -1,    -1,
      -1,    68,    69,    70,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    80,    81,    -1,    83,    84,    85,    -1,
      27,    88,    29,    30,    -1,    -1,    -1,    -1,    -1,    36,
      37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      -1,    -1,    -1,    -1,    -1,    -1,    63,    -1,    -1,    -1,
      -1,    68,    69,    70,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    80,    81,    -1,    83,    84,    85,    -1,
      27,    88,    29,    30,    -1,    -1,    -1,    -1,    -1,    36,
      37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      -1,    -1,    -1,    -1,    -1,    -1,    63,    -1,    -1,    -1,
      -1,    68,    69,    70,    -1,    27,    -1,    29,    30,    -1,
      -1,    -1,    -1,    80,    81,    37,    83,    84,    85,    -1,
      -1,    88,    -1,    -1,    -1,    -1,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    -1,    -1,    -1,    -1,    -1,
      -1,    63,    -1,    -1,    -1,    -1,    68,    69,    70,    27,
      -1,    29,    30,    -1,    -1,    -1,    -1,    -1,    80,    81,
      -1,    83,    84,    85,    -1,    -1,    88,    -1,    -1,    -1,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    -1,
      -1,    -1,    -1,    -1,    -1,    63,    -1,    -1,    -1,    -1,
      68,    69,    70,    27,    -1,    29,    30,    -1,    -1,    -1,
      -1,    -1,    80,    81,    -1,    83,    84,    85,    -1,    -1,
      88,    -1,    -1,    -1,    -1,    49,    50,    51,    52,    53,
      54,    55,    56,    -1,    -1,    -1,    -1,    -1,    -1,    63,
      -1,    -1,    -1,    -1,    68,    69,    70,    27,    -1,    29,
      30,    -1,    -1,    -1,    -1,    -1,    80,    81,    -1,    83,
      84,    85,    -1,    -1,    88,    -1,    -1,    -1,    -1,    49,
      50,    51,    52,    53,    54,    55,    56,    -1,    -1,    -1,
      -1,    -1,    -1,    63,    -1,    -1,    -1,    -1,    68,    69,
      70,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      80,    81,    -1,    83,    84,    85,    -1,    -1,    88
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     6,    91,    92,    20,   100,     0,     3,    95,    96,
      97,    34,    96,   100,    35,    34,    27,     7,     9,    35,
     106,   107,    22,    93,   100,   105,   108,   128,   100,   109,
     110,   111,   112,    27,    26,    35,     7,     9,    10,   100,
     101,   102,   122,   131,    26,    14,    26,    57,    58,    26,
      14,    26,    22,    98,   107,    27,   103,   104,   108,   100,
     113,   114,   115,   116,    34,    24,    26,    12,    23,    94,
     108,    15,    16,    17,    18,    19,    32,    34,    66,   117,
     118,   119,   100,   110,    29,    30,    32,    34,    39,    41,
      44,    45,    58,    66,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    86,   100,   117,   135,   138,   139,   140,
     141,   142,   143,   144,   145,   146,   147,   148,   149,   112,
      32,    34,    63,    66,    87,   101,   121,   123,   124,   125,
     126,   130,   132,   133,   134,   135,    98,    26,    27,    26,
      14,    26,    34,    35,   129,   135,   122,   102,   122,   117,
     117,   120,   117,   135,   135,    33,   129,   129,   135,   135,
     135,    67,   129,    34,    34,    34,    34,    34,    34,    34,
      34,    34,    34,    46,    27,    29,    30,    36,    37,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    63,    68,
      69,    70,    80,    81,    82,    83,    84,    85,    88,   121,
     135,   121,   135,    24,    13,    23,    25,    99,    47,   127,
     135,    38,    64,   104,   135,   114,   117,   135,   116,    26,
      35,    94,    33,    35,    26,    67,    33,    26,    35,    67,
     135,   135,   135,   135,   135,   135,   135,   135,   135,   135,
     135,   135,   135,   135,   135,   135,   135,   135,   135,   135,
     135,   135,   135,   135,   135,   135,   135,   135,   135,   135,
     135,   117,   135,   135,   135,   135,    35,   121,   121,   121,
      11,   100,   136,   137,    14,    57,    58,   121,    82,   135,
     117,   135,    35,    35,    35,    35,    35,    26,    26,    35,
      35,    35,    99,    26,    28,   129,   129,   117,    35,    35,
     135,   135,   137,   129,    35,    35
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
#line 167 "chiparser.yy"
    {
          gsDebugMsg("%s;%d\n",__FILE__,__LINE__);
    	  gsDebugMsg("inputs contains a valid Chi-specification\n");
          safe_assign(((*yyvalp).appl), gsMakeChiSpec((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl),ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.list))));
		  chi_spec_tree = ((*yyvalp).appl);
		;}
    break;

  case 3:
#line 177 "chiparser.yy"
    {
      	  safe_assign(((*yyvalp).appl), gsMakeModelDef((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (6))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((6) - (6))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed Model Def \n  %T\n", ((*yyvalp).appl));
        ;}
    break;

  case 4:
#line 185 "chiparser.yy"
    { safe_assign(((*yyvalp).appl), gsMakeModelSpec( ATmakeList0(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl) ));
      	  gsDebugMsg("parsed Model Body  \n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 5:
#line 189 "chiparser.yy"
    { safe_assign(((*yyvalp).appl), gsMakeModelSpec( (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed Model Body  \n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 6:
#line 196 "chiparser.yy"
    {
          var_type_map.clear();
          chan_type_direction_map.clear();
       ;}
    break;

  case 7:
#line 203 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed Process Definition \n  %T\n", ((*yyvalp).list));
		;}
    break;

  case 8:
#line 207 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed Process Definition \n  %T\n", ((*yyvalp).list));
		;}
    break;

  case 9:
#line 214 "chiparser.yy"
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
#line 231 "chiparser.yy"
    {
      	  safe_assign(((*yyvalp).appl), gsMakeProcDef((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (7))].yystate.yysemantics.yysval.appl), gsMakeProcDecl( ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (7))].yystate.yysemantics.yysval.list))), (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (7))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed proc Def\n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 11:
#line 247 "chiparser.yy"
    {
		  scope_lvl++;
		  gsDebugMsg("Increase Scope to: %d\n",scope_lvl);
		;}
    break;

  case 12:
#line 255 "chiparser.yy"
    { safe_assign(((*yyvalp).appl), gsMakeProcSpec( ATmakeList0(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl) ));
      	  gsDebugMsg("parsed ProcessBody  \n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 13:
#line 259 "chiparser.yy"
    { safe_assign(((*yyvalp).appl), gsMakeProcSpec( ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.list)), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed ProcessBody  \n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 14:
#line 266 "chiparser.yy"
    {
		  assert(scope_lvl > 0);
		  scope_lvl--;
		  gsDebugMsg("Decrease Scope to; %d\n",scope_lvl);
          var_type_map.clear();
          chan_type_direction_map.clear();
		;}
    break;

  case 15:
#line 276 "chiparser.yy"
    {
 	  	  safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl) );
      	  gsDebugMsg("parsed id's\n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 16:
#line 283 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("LocalVariables_csp: parsed \n  %T\n", ((*yyvalp).list));
		;}
    break;

  case 17:
#line 287 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("LocalVariables_csp: parsed \n  %T\n", ((*yyvalp).list));
		;}
    break;

  case 18:
#line 294 "chiparser.yy"
    {
		  safe_assign(((*yyvalp).appl), gsMakeVarSpec( (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.list) ) );
		  gsDebugMsg("LocalVariables: parsed \n %T\n", ((*yyvalp).appl));
		;}
    break;

  case 19:
#line 299 "chiparser.yy"
    {
		  //safe_assign($$, gsMakeVarSpec( ATreverse( $2 ) ) );  //<-- gsMakeVarSpec aanpassen
		  gsDebugMsg("LocalVariables: parsed \n %T\n", ((*yyvalp).appl));
		;}
    break;

  case 21:
#line 309 "chiparser.yy"
    { safe_assign(((*yyvalp).list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list));
		  gsDebugMsg("IdentifierTypeExpression_csp: parsed \n %T\n", ((*yyvalp).list));
		;}
    break;

  case 22:
#line 313 "chiparser.yy"
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

  case 23:
#line 330 "chiparser.yy"
    {
          //An expression cannot be of type "Void"
          ATermList list = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list);
          while (!ATisEmpty(list))
          {
            if (strcmp( ATgetName(ATgetAFun(ATgetArgument(ATgetArgument(ATgetFirst(list),1),0))), "Void" ) == 0)
            {
              chigetposition();
              gsErrorMsg("Expression %T can not be of type \"void\"\n", ATgetFirst(list));
              exit(1);
            }
            list = ATgetNext( list ) ;
          }

		  safe_assign(((*yyvalp).list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list) );
		  gsDebugMsg("IdentifierTypeExpression: parsed \n %T\n", ((*yyvalp).list));
		;}
    break;

  case 24:
#line 348 "chiparser.yy"
    {
          ATermList list = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list);
          ATermList new_list = ATmakeList0();

          while (!ATisEmpty(list))
          {
            //An expression cannot be of type "void"
            if (strcmp( ATgetName(ATgetAFun(ATgetArgument(ATgetArgument(ATgetFirst(list),1),0))), "Void" ) == 0)
            {
              gsErrorMsg("Expression %T can not be of type \"void\"\n", ATgetFirst(list));
              exit(1);
            }
            new_list = ATinsert( new_list , (ATerm) gsMakeDataVarExprID( (ATermAppl) ATgetFirst(list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
            list = ATgetNext( list ) ;
          }

          safe_assign(((*yyvalp).list), new_list);

		  //safe_assign($$, gsMakeDataVarExprID ( $1, $3 ) );
		  gsDebugMsg("IdentifierTypeExpression: parsed \n %T\n", ((*yyvalp).list));
		;}
    break;

  case 25:
#line 373 "chiparser.yy"
    { safe_assign(((*yyvalp).list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list));
		  gsDebugMsg("IdentifierType_csp: parsed \n %T\n", ((*yyvalp).list));
		;}
    break;

  case 26:
#line 377 "chiparser.yy"
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

  case 27:
#line 394 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
		  gsDebugMsg("FormalParameter_csp: parsed \n %T\n", ((*yyvalp).list));
		;}
    break;

  case 28:
#line 398 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
		  gsDebugMsg("FormalParameter_csp: parsed \n %T\n", ((*yyvalp).list));
		;}
    break;

  case 29:
#line 405 "chiparser.yy"
    {
		  safe_assign(((*yyvalp).appl), gsMakeVarDecl( ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.list)) ) );
		  gsDebugMsg("FormalParameter: parsed \n %T\n", ((*yyvalp).appl));
		;}
    break;

  case 30:
#line 410 "chiparser.yy"
    {
		  safe_assign(((*yyvalp).appl), gsMakeChanDecl( ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.list)) ) );
		  gsDebugMsg("FormalParameter: parsed \n %T\n", ((*yyvalp).appl));
		;}
    break;

  case 31:
#line 418 "chiparser.yy"
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
              chigetposition();
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
              chigetposition();
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

  case 32:
#line 469 "chiparser.yy"
    { safe_assign(((*yyvalp).list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list) );
      	  gsDebugMsg("ChannelDeclaration_csp: parsed formalparameter channel  \n  %T\n", ((*yyvalp).list));
		;}
    break;

  case 33:
#line 473 "chiparser.yy"
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

  case 34:
#line 489 "chiparser.yy"
    {
          gsDebugMsg("%s;%d\n",__FILE__,__LINE__);
		  ATermList list = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list);
		  int n = ATgetLength( list );
		  for(int i = 0; i < n ; ++i ){
			if (var_type_map.end() != var_type_map.find(ATgetArgument( ATgetFirst( list ),0)))
			{
              chigetposition();
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
              chigetposition();
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
             //Hash channel is set to 0 indicating no hash is used
             ATermAppl zero = (ATermAppl) gsMakeExpression(gsString2ATermAppl("0"), (ATermAppl) gsMakeType(gsString2ATermAppl("Nat")));
             new_list = ATinsert(new_list,(ATerm) gsMakeChannelTypedID( (ATermAppl) ATgetFirst(list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl), zero ) );
			 list = ATgetNext( list ) ;
		  }

		  safe_assign(((*yyvalp).list), new_list );
		  gsDebugMsg("ChannelDefinition: parsed VariableList \n %T\n", ((*yyvalp).list));
		;}
    break;

  case 35:
#line 532 "chiparser.yy"
    {
          gsDebugMsg("%s;%d\n",__FILE__,__LINE__);

          //Make sure that Expression is a number
          UnaryTypeCheck( (ATermAppl) ATgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.appl),1), "Nat");

		  ATermList list = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.list);
		  int n = ATgetLength( list );
		  for(int i = 0; i < n ; ++i ){
			if (var_type_map.end() != var_type_map.find(ATgetArgument( ATgetFirst( list ),0)))
			{
              chigetposition();
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
              chigetposition();
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

  case 36:
#line 579 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("IdentifierChannelDeclaration_csp: parsed formalparameter channel  \n  %T\n", ((*yyvalp).list));
		;}
    break;

  case 37:
#line 583 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("IdentifierChannelDeclaration_csp: parsed formalparameter channel \n  %T\n", ((*yyvalp).list));
		;}
    break;

  case 38:
#line 590 "chiparser.yy"
    {
          safe_assign(((*yyvalp).appl), gsMakeChannelID((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl), gsMakeRecv()));
		  gsDebugMsg("IdentifierChannelDeclaration: parsed Identifier Type With Expression \n %T\n", ((*yyvalp).appl));
        ;}
    break;

  case 39:
#line 595 "chiparser.yy"
    {
          safe_assign(((*yyvalp).appl), gsMakeChannelID((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl), gsMakeSend()));
		  gsDebugMsg("IdentifierChannelDeclaration: parsed Identifier Type With Expression \n %T\n", ((*yyvalp).appl));
        ;}
    break;

  case 40:
#line 613 "chiparser.yy"
    { //safe_assign($$, ATmakeList1((ATerm) $1));
      	  gsDebugMsg("ChannelDefinition_csp: parsed \n  %T\n", ((*yyvalp).list));
		;}
    break;

  case 41:
#line 617 "chiparser.yy"
    { //safe_assign($$, ATinsert($1, (ATerm) $3));
      	  gsDebugMsg("ChannelDefinition_csp: parsed \n  %T\n", ((*yyvalp).list));
		;}
    break;

  case 42:
#line 624 "chiparser.yy"
    {
          gsDebugMsg("%s;%d\n",__FILE__,__LINE__);
          gsDebugMsg("ChannelDefinition\n");
		  ATermList list = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list);
		  int n = ATgetLength( list );
		  for(int i = 0; i < n ; ++i ){
			if (var_type_map.end() != var_type_map.find(ATgetArgument( ATgetFirst( list ),0)))
			{
              chigetposition();
			  gsErrorMsg("Variable %T is already defined!\n", ATgetFirst( list ));
			  exit(1);
			};
			list = ATgetNext( list ) ;
		  }	;

          list = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list);
		  n = ATgetLength( list );
		  for(int i = 0; i < n ; ++i ){
			if (chan_type_direction_map.end() != chan_type_direction_map.find(ATgetArgument( ATgetFirst( list ),0)))
			{
              chigetposition();
			  gsErrorMsg("Channel %T is already defined!\n", ATgetFirst( list ));
			  exit(1);
			};
			chan_type_direction_map[ATgetArgument(ATgetFirst( list ), 0)]=  make_pair( (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl), ATgetArgument(ATgetFirst( list ), 1) );
			list = ATgetNext( list ) ;
		  }	;

		  list = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list);
          ATermList new_list = ATmakeList0();
		  while(!ATisEmpty(list))
          {
             //Hash channel is set to 0 indicating no hash is used
             ATermAppl zero = (ATermAppl) gsMakeExpression(gsString2ATermAppl("0"), (ATermAppl) gsMakeType(gsString2ATermAppl("Nat")));
             new_list = ATinsert(new_list,(ATerm) gsMakeChannelTypedID( (ATermAppl) ATgetFirst(list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl), zero ) );
			 list = ATgetNext( list ) ;
		  }

		  safe_assign(((*yyvalp).list), new_list );
		  gsDebugMsg("ChannelDefinition: parsed VariableList \n %T\n", ((*yyvalp).list));
		;}
    break;

  case 43:
#line 666 "chiparser.yy"
    {
          gsDebugMsg("%s;%d\n",__FILE__,__LINE__);

          //Make sure that Expression is a number
          UnaryTypeCheck( (ATermAppl) ATgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.appl),1), "Nat");

		  ATermList list = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.list);
		  int n = ATgetLength( list );
		  for(int i = 0; i < n ; ++i ){
			if (var_type_map.end() != var_type_map.find(ATgetArgument( ATgetFirst( list ),0)))
			{
              chigetposition();
			  gsErrorMsg("Variable %T is already defined!\n", ATgetFirst( list ));
			  exit(1);
			};
			list = ATgetNext( list ) ;
		  }	;

          list = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.list);
		  n = ATgetLength( list );
		  for(int i = 0; i < n ; ++i ){
			if (chan_type_direction_map.end() != chan_type_direction_map.find(ATgetArgument( ATgetFirst( list ),0)))
			{
              chigetposition();
			  gsErrorMsg("Channel %T is already defined!\n", ATgetFirst( list ));
			  exit(1);
			};
			chan_type_direction_map[ATgetArgument(ATgetFirst( list ), 0)]=  make_pair( (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.appl), ATgetArgument(ATgetFirst( list ), 1) );
			list = ATgetNext( list ) ;
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

  case 44:
#line 713 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("IdentifierChannelDefinition_csp: parsed \n  %T\n", ((*yyvalp).list));
		;}
    break;

  case 45:
#line 717 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("IdentifierChannelDefinition_csp: parsed \n  %T\n", ((*yyvalp).list));
		;}
    break;

  case 46:
#line 724 "chiparser.yy"
    {
          safe_assign(((*yyvalp).appl), gsMakeChannelID((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl), gsMakeNil()));
		  gsDebugMsg("IdentifierChannelDefinition: parsed \n %T\n", ((*yyvalp).appl));
        ;}
    break;

  case 48:
#line 744 "chiparser.yy"
    {
          safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)  );
      	  gsDebugMsg("Type: parsed Type \n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 50:
#line 756 "chiparser.yy"
    {
          safe_assign(((*yyvalp).appl), gsMakeType( gsString2ATermAppl( "Bool" ) ) );
      	  gsDebugMsg("BasicType: parsed Type \n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 51:
#line 761 "chiparser.yy"
    {
          safe_assign(((*yyvalp).appl), gsMakeType( gsString2ATermAppl("Nat" ) ) );
      	  gsDebugMsg("BasicType: parsed Type \n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 52:
#line 766 "chiparser.yy"
    {
          safe_assign(((*yyvalp).appl), gsMakeType( gsString2ATermAppl("Void" ) ) );
      	  gsDebugMsg("BasicType: parsed Type \n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 54:
#line 772 "chiparser.yy"
    {
          safe_assign(((*yyvalp).appl), gsMakeType( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl) ) );
      	  gsDebugMsg("BasicType: parsed Type \n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 55:
#line 782 "chiparser.yy"
    {
          safe_assign(((*yyvalp).appl), gsMakeListType((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("ContainerType: parsed Type \n  %T\n", ((*yyvalp).appl));

        ;}
    break;

  case 56:
#line 788 "chiparser.yy"
    {
          safe_assign(((*yyvalp).appl), gsMakeSetType((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("ContainerType: parsed Type \n  %T\n", ((*yyvalp).appl));

        ;}
    break;

  case 57:
#line 794 "chiparser.yy"
    {
          ATermList list = ATinsert( (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.appl) );
          safe_assign(((*yyvalp).appl), gsMakeTupleType(ATreverse(list)));
      	  gsDebugMsg("ContainerType: parsed Type \n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 58:
#line 803 "chiparser.yy"
    {
          safe_assign(((*yyvalp).list), ATmakeList1( (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)) );
      	  gsDebugMsg("ChannelDeclaration_csp: parsed formalparameter channel  \n  %T\n", ((*yyvalp).list));
		;}
    break;

  case 59:
#line 808 "chiparser.yy"
    {
          safe_assign(((*yyvalp).list), ATinsert( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl) ) );
      	  gsDebugMsg("BasicType: parsed Type \n  %T\n", ((*yyvalp).list));
		;}
    break;

  case 60:
#line 821 "chiparser.yy"
    { safe_assign(((*yyvalp).appl), gsMakeParenthesisedStat( (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("Statement: parsed \n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 65:
#line 832 "chiparser.yy"
    { safe_assign(((*yyvalp).appl), gsMakeParStat( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("ModelStatement: parsed \n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 70:
#line 849 "chiparser.yy"
    {
        // Ugly Hack :D
        safe_assign(((*yyvalp).appl), gsMakeSkipStat( gsMakeNil(), gsMakeNil(), gsMakeSkip() ));
        gsDebugMsg("AssignmentStatement: parsed \n  %T\n", ((*yyvalp).appl));
      ;}
    break;

  case 71:
#line 859 "chiparser.yy"
    {
          ATermList ids = (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.list);
          ATermList exprs = (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.list);

          while(!ATisEmpty(ids))
          {
            if (!ContainerTypeChecking((ATermAppl) ATgetArgument(ATgetFirst(ids), 1), (ATermAppl) ATgetArgument(ATgetFirst(exprs), 1)))
		    {
              chigetposition();
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

  case 72:
#line 880 "chiparser.yy"
    {
          safe_assign(((*yyvalp).appl), gsMakeSkipStat( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl), gsMakeSkip() ));
      	  gsDebugMsg("AssignmentStatement: parsed \n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 73:
#line 888 "chiparser.yy"
    { safe_assign(((*yyvalp).appl), gsMakeNil() );
		;}
    break;

  case 74:
#line 891 "chiparser.yy"
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

  case 75:
#line 909 "chiparser.yy"
    { safe_assign(((*yyvalp).appl), gsMakeNil() );
		;}
    break;

  case 76:
#line 912 "chiparser.yy"
    { safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl));
		  gsErrorMsg("OptChannel not yet implemented");
		  assert(false);
      	  gsDebugMsg("OptChannel: parsed \n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 77:
#line 921 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATmakeList1( (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("Identifier_csp: parsed \n  %T\n", ((*yyvalp).list));
		;}
    break;

  case 78:
#line 925 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("Identifier_csp: parsed \n  %T\n", ((*yyvalp).list));
		;}
    break;

  case 79:
#line 932 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("Expression_csp: parsed \n  %T\n", ((*yyvalp).list));
		;}
    break;

  case 80:
#line 936 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("Expression_csp: parsed \n  %T\n", ((*yyvalp).list));
		;}
    break;

  case 81:
#line 943 "chiparser.yy"
    {
          //Check if $2 is properly typed
          //Check if $4 is properly typed
          //Check if $2 can send

          ATermAppl channel = (ATermAppl) ATgetArgument(ATgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.appl),0),0);
          ATermAppl hash    = (ATermAppl) ATgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.appl),2);
          gsDebugMsg("%T\n",(((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.appl));
          gsDebugMsg("%T\n",hash);

          safe_assign(((*yyvalp).appl), gsMakeSendStat((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.appl), channel, hash , ATreverse( (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.list)) ) );
      	  gsDebugMsg("CommStatement: parsed %T\n", ((*yyvalp).appl));
        ;}
    break;

  case 82:
#line 958 "chiparser.yy"
    {
          gsDebugMsg("%T",(((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl));

          ATermAppl channel = (ATermAppl) ATgetArgument(ATgetArgument( (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl),0),0);
          ATermAppl hash    = (ATermAppl) ATgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl),2);

          safe_assign(((*yyvalp).appl), gsMakeSendStat((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), channel, hash, ATmakeList0() ) );
      	  gsDebugMsg("CommStatement: parsed %T\n", ((*yyvalp).appl));
        ;}
    break;

  case 83:
#line 970 "chiparser.yy"
    {
          //Check if $2 is properly typed
          //Check if $4 is properly typed
          //Check if $2 can receive

          gsDebugMsg("%T",(((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.appl));

          ATermAppl channel = (ATermAppl) ATgetArgument(ATgetArgument( (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.appl),0),0);
          ATermAppl hash    = (ATermAppl) ATgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.appl),2);

          safe_assign(((*yyvalp).appl), gsMakeRecvStat((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.appl), channel, hash, ATreverse( (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.list)) ) );
      	  gsDebugMsg("CommStatement: parsed %T\n", ((*yyvalp).appl));
        ;}
    break;

  case 84:
#line 985 "chiparser.yy"
    {
          //Check if $2 is properly typed
          //Check if $4 is properly typed
          //Check if $2 can receive

          gsDebugMsg("%T",(((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl));

          ATermAppl channel = (ATermAppl) ATgetArgument(ATgetArgument( (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl),0),0);
          ATermAppl hash    = (ATermAppl) ATgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl),2);

          safe_assign(((*yyvalp).appl), gsMakeRecvStat((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), channel, hash, ATmakeList0() ) );
      	  gsDebugMsg("CommStatement: parsed %T\n", ((*yyvalp).appl));
        ;}
    break;

  case 85:
#line 1004 "chiparser.yy"
    {
        safe_assign(((*yyvalp).appl), gsMakeInstantiation((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), ATmakeList0()));
      ;}
    break;

  case 86:
#line 1008 "chiparser.yy"
    {
        safe_assign(((*yyvalp).appl), gsMakeInstantiation((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.appl), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.list))));
      ;}
    break;

  case 87:
#line 1015 "chiparser.yy"
    { safe_assign(((*yyvalp).appl), gsMakeSepStat( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed SEP statement \n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 88:
#line 1019 "chiparser.yy"
    { safe_assign(((*yyvalp).appl), gsMakeAltStat( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed ALT statement \n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 89:
#line 1030 "chiparser.yy"
    {
          gsDebugMsg("%d\n", __LINE__);
      	  safe_assign(((*yyvalp).appl), gsMakeStarStat( (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed STAR statement \n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 90:
#line 1036 "chiparser.yy"
    {
          gsDebugMsg("%d\n", __LINE__);
          gsDebugMsg("%T\n",(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl));
          gsDebugMsg("%T\n",(((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl));
          UnaryTypeCheck(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1), "Bool");
          safe_assign(((*yyvalp).appl), gsMakeGuardedStarStat( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed GuardedSTAR statement \n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 91:
#line 1048 "chiparser.yy"
    { safe_assign(((*yyvalp).appl), gsMakeDeltaStat((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl), gsMakeDelta()));
      	  gsDebugMsg("parsed deadlock statement \n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 92:
#line 1061 "chiparser.yy"
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

  case 103:
#line 1095 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed expression-element \n  %T\n", ((*yyvalp).list));
		;}
    break;

  case 104:
#line 1099 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed expression-element\n  %T\n", ((*yyvalp).list));
		;}
    break;

  case 105:
#line 1105 "chiparser.yy"
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
              chigetposition();
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

  case 106:
#line 1133 "chiparser.yy"
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
             //ATermAppl one = (ATermAppl) gsMakeExpression(gsString2ATermAppl("0"), (ATermAppl) gsMakeType(gsString2ATermAppl("Nat")));
              safe_assign(((*yyvalp).appl),
                gsMakeChannelTypedID(
                  gsMakeChannelID((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl), gsMakeNil()),
                  (ATermAppl) chan_type_direction_map[(ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)].first,
                  gsMakeNil()
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
              chigetposition();
              gsErrorMsg("BasicExpression: Variable/Channel %T is not defined!\n", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl) );
              exit(1);
          }

          gsDebugMsg("BasicExpression: parsed \n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 107:
#line 1179 "chiparser.yy"
    {
		  /**
		    * Lookup Identifier Type
		    *
		    * TODO: Add scope
		    *
		    **/
          gsDebugMsg("%s:%d\n",__FILE__, __LINE__ );

          UnaryTypeCheck( (ATermAppl) ATgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1), "Nat");

          if (var_type_map.find((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl)) != var_type_map.end())
          {
            ATermAppl tuple_type = (ATermAppl) var_type_map[(ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl)];
            gsDebugMsg("%T\n", tuple_type);

            //Check if $1 is a tuple
            if( strcmp("TupleType", ATgetName( ATgetAFun( tuple_type ) ) ) != 0 )
            {
              chigetposition();
              gsErrorMsg("%T is not a Tuple", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl));
              exit(1);
            }

            if (!is_number(ATgetName(ATgetAFun(ATgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),0)))) )
            {
              chigetposition();
              gsErrorMsg("BasicExpression: Expected a number after the \".\"\n");
              exit(1);
            }

            int index = atoi(ATgetName(ATgetAFun(ATgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),0))));

            if (index >= (int) ATgetLength( (ATermList) ATgetArgument( tuple_type, 0 ) ) )
            {
              chigetposition();
              gsErrorMsg("BasicExpression: Index value \"%d\" is out of bounds for %T\n", index, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl) );
              exit(1);
            }

            ATermList to_process = (ATermList) ATgetArgument(tuple_type, 0 );
            while (index > 0)
            {
              to_process = ATgetNext(to_process);
              --index;
            }
            ATerm type = ATgetFirst(to_process);

            safe_assign(((*yyvalp).appl),
              gsMakeTupleDot(
			    gsMakeExpression( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),
			      (ATermAppl) var_type_map[(ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl)]
			    ),
                (ATermAppl) type,
                (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)
              )
            );
            gsDebugMsg("BasicExpression (Tuple Selection): parsed \n  %T\n", ((*yyvalp).appl));
          }

          if (chan_type_direction_map.end() != chan_type_direction_map.find( (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl)))
          {
  	           //Check if # of channel is a Natural number
  	           //UnaryTypeCheck( (ATermAppl) gsMakeType($3), "Nat");

  	           safe_assign(((*yyvalp).appl),
  	             gsMakeChannelTypedID(
  	               gsMakeChannelID((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), gsMakeNil()),
  	               (ATermAppl) chan_type_direction_map[(ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl)].first,
  	               (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)
  	             )
  	           );

  	           gsDebugMsg("BasicExpression (Hashed Channel): parsed \n  %T\n", ((*yyvalp).appl));
          }

          if ( (chan_type_direction_map.end() == chan_type_direction_map.find( (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl))) &&
               (var_type_map.find((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl)) == var_type_map.end()) )
          {
              chigetposition();
              gsErrorMsg("BasicExpression: Variable/Channel %T is not defined!\n", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl) );
              exit(1);
          }
		;}
    break;

  case 108:
#line 1271 "chiparser.yy"
    {
 	  		safe_assign(((*yyvalp).appl),
				gsMakeExpression( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl),
					gsMakeType( gsString2ATermAppl("Bool" ))
				)
			);
      		gsDebugMsg("BooleanExpression: parsed \n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 109:
#line 1280 "chiparser.yy"
    {
 	  		safe_assign(((*yyvalp).appl),
				gsMakeExpression( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl),
					gsMakeType( gsString2ATermAppl("Bool" ))
				)
			);
      		gsDebugMsg("BooleanExpression: parsed \n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 110:
#line 1289 "chiparser.yy"
    {
            UnaryTypeCheck( (ATermAppl) ATgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl),1), "Bool");

 	  		safe_assign(((*yyvalp).appl),
				gsMakeUnaryExpression( gsString2ATermAppl("!" ),
				gsMakeType( gsString2ATermAppl("Bool" ) ),
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl) )
			);
      		gsDebugMsg("parsed Negation Expression's\n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 111:
#line 1300 "chiparser.yy"
    {
            UnaryTypeCheck( (ATermAppl) ATgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl),1), "Bool");

 	  		safe_assign(((*yyvalp).appl),
				gsMakeUnaryExpression( gsString2ATermAppl("!" ),
				gsMakeType( gsString2ATermAppl("Bool" ) ),
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl) )
			);
      		gsDebugMsg("parsed Negation Expression's\n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 112:
#line 1311 "chiparser.yy"
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

  case 113:
#line 1324 "chiparser.yy"
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

  case 114:
#line 1337 "chiparser.yy"
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

  case 115:
#line 1353 "chiparser.yy"
    {
 	  		safe_assign(((*yyvalp).appl),
				gsMakeExpression( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl),
					gsMakeType( gsString2ATermAppl("Nat" ))
				)
			);
      		gsDebugMsg("parsed Expression's\n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 116:
#line 1362 "chiparser.yy"
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

  case 117:
#line 1374 "chiparser.yy"
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

  case 118:
#line 1386 "chiparser.yy"
    {
            BinTypeCheck(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1), ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1), "Nat");

 	  		safe_assign(((*yyvalp).appl),
				gsMakeBinaryExpression( gsString2ATermAppl("^" ),
				gsMakeType( gsString2ATermAppl("Nat" ) ),
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl) ,
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)
				)
			);
      		gsDebugMsg("parsed Binary POWER Expression's\n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 119:
#line 1399 "chiparser.yy"
    {
            BinTypeCheck(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1), ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1), "Nat");

 	  		safe_assign(((*yyvalp).appl),
				gsMakeBinaryExpression( gsString2ATermAppl("*" ),
				gsMakeType( gsString2ATermAppl("Nat" ) ),
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl) ,
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)
				)
			);
      		gsDebugMsg("parsed Binary STAR Expression's\n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 120:
#line 1412 "chiparser.yy"
    {
            BinTypeCheck(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1), ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1), "Nat");

 	  		safe_assign(((*yyvalp).appl),
				gsMakeBinaryExpression( gsString2ATermAppl("/" ),
				gsMakeType( gsString2ATermAppl("Nat" ) ),
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl) ,
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)
				)
			);
      		gsDebugMsg("parsed Binary DIVIDE Expression's\n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 121:
#line 1425 "chiparser.yy"
    {
            BinTypeCheck(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1), ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1), "Nat");

 	  		safe_assign(((*yyvalp).appl),
				gsMakeBinaryExpression( gsString2ATermAppl("MOD" ),
				gsMakeType( gsString2ATermAppl("Nat" ) ),
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl) ,
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)
				)
			);
      		gsDebugMsg("parsed Binary MOD Expression's\n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 122:
#line 1438 "chiparser.yy"
    {
            BinTypeCheck(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1), ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1), "Nat");

 	  		safe_assign(((*yyvalp).appl),
				gsMakeBinaryExpression( gsString2ATermAppl("DIV" ),
				gsMakeType( gsString2ATermAppl("Nat" ) ),
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl) ,
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)
				)
			);
      		gsDebugMsg("parsed Binary DIV Expression's\n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 123:
#line 1451 "chiparser.yy"
    {
            BinTypeCheck(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1), ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1), "Nat");

 	  		safe_assign(((*yyvalp).appl),
				gsMakeBinaryExpression( gsString2ATermAppl("MIN" ),
				gsMakeType( gsString2ATermAppl("Nat" ) ),
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl) ,
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)
				)
			);
      		gsDebugMsg("parsed Binary MIN Expression's\n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 124:
#line 1464 "chiparser.yy"
    {
            BinTypeCheck(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1), ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1), "Nat");

 	  		safe_assign(((*yyvalp).appl),
				gsMakeBinaryExpression( gsString2ATermAppl("MAX" ),
				gsMakeType( gsString2ATermAppl("Nat" ) ),
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl) ,
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)
				)
			);
      		gsDebugMsg("parsed Binary MAX Expression's\n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 125:
#line 1477 "chiparser.yy"
    {
            BinTypeCheck(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1), ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1), "Nat");

 	  		safe_assign(((*yyvalp).appl),
				gsMakeBinaryExpression( gsString2ATermAppl("<" ),
				gsMakeType( gsString2ATermAppl("Bool" ) ),
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl) ,
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)
				)
			);
      		gsDebugMsg("parsed Binary LESS Expression's\n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 126:
#line 1490 "chiparser.yy"
    {
            BinTypeCheck(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1), ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1), "Nat");

 	  		safe_assign(((*yyvalp).appl),
				gsMakeBinaryExpression( gsString2ATermAppl(">" ),
				gsMakeType( gsString2ATermAppl("Bool" ) ),
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl) ,
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)
				)
			);
      		gsDebugMsg("parsed Binary GREATER Expression's\n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 127:
#line 1503 "chiparser.yy"
    {
            BinTypeCheck(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1), ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1), "Nat");

 	  		safe_assign(((*yyvalp).appl),
				gsMakeBinaryExpression( gsString2ATermAppl("<=" ),
				gsMakeType( gsString2ATermAppl("Bool" ) ),
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl) ,
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)
				)
			);
      		gsDebugMsg("parsed Binary LEQ Expression's\n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 128:
#line 1516 "chiparser.yy"
    {
            BinTypeCheck(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1), ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1), "Nat");

 	  		safe_assign(((*yyvalp).appl),
				gsMakeBinaryExpression( gsString2ATermAppl(">=" ),
				gsMakeType( gsString2ATermAppl("Bool" ) ),
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl) ,
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)
				)
			);
      		gsDebugMsg("parsed Binary GEQ Expression's\n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 129:
#line 1532 "chiparser.yy"
    {
       gsDebugMsg("Expression 1: %T\n", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl));
       gsDebugMsg("Expression 2: %T\n", (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl));

       bool processed = false;

       /**
         *  Plus operater on naturals
         *
         **/
       if (strcmp(ATgetName(ATgetAFun(ATAgetArgument(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1),0))), "Nat") == 0 ||
           strcmp(ATgetName(ATgetAFun(ATAgetArgument(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1),0))), "Nat") == 0
          )
       {

            BinTypeCheck(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1), ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1), "Nat");

 	  		safe_assign(((*yyvalp).appl),
				gsMakeBinaryExpression( gsString2ATermAppl("+" ),
				gsMakeType( gsString2ATermAppl("Nat" ) ),
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl) ,
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)
				)
			);
            gsDebugMsg("PlusExpression - Nat Expression parsed: \n%T\n", ((*yyvalp).appl));
            processed = true;
		}

       /**
         *  Plus operater on sets
         *
         **/
       if (strcmp(ATgetName(ATgetAFun(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1))), "SetType") == 0 ||
           strcmp(ATgetName(ATgetAFun(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1))), "SetType") == 0
          )
       {
         if(!ContainerTypeChecking(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1),  ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1)))
	     {
           chigetposition();
		   gsErrorMsg("Incompatible Types Checking failed\n");
		   exit(1);
		 }
   		 safe_assign(((*yyvalp).appl), gsMakeBinarySetExpression( (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl),
				         ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1),
                	(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
         gsDebugMsg("PlusExpression - Set Expression parsed: \n  %T\n", ((*yyvalp).appl));
         processed = true;
       }

       /**
         *  For all other types on plus
         *
         **/

       if (!processed)
       {
         chigetposition();
         gsErrorMsg("Expressions %T and %T cannot be used with \"+\"\n", ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),0), ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),0));
         exit(1);
       }
     ;}
    break;

  case 130:
#line 1597 "chiparser.yy"
    {
       gsDebugMsg("Expression 1: %T\n", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl));
       gsDebugMsg("Expression 2: %T\n", (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl));

       bool processed = false;
       /**
         *  Minus operater on naturals
         *
         **/
       if (strcmp(ATgetName(ATgetAFun(ATAgetArgument(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1),0))), "Nat") == 0 ||
           strcmp(ATgetName(ATgetAFun(ATAgetArgument(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1),0))), "Nat") == 0
          )
       {

 	  		safe_assign(((*yyvalp).appl),
				gsMakeBinaryExpression( gsString2ATermAppl("-" ),
				gsMakeType( gsString2ATermAppl("Nat" ) ),
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl) ,
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)
				)
			);
         gsDebugMsg("MinusExpression - Nat Expression parsed: \n%T\n", ((*yyvalp).appl));
         processed = true;
       }
       /**
         *  Minus operater on lists
         *
         **/
       if (strcmp(ATgetName(ATgetAFun(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1))), "ListType") == 0 ||
           strcmp(ATgetName(ATgetAFun(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1))), "ListType") == 0
          )
       {
         if(!ContainerTypeChecking(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1),  ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1)))
	     {
           chigetposition();
		   gsErrorMsg("Incompatible Types Checking failed\n");
		   exit(1);
		 }
   		 safe_assign(((*yyvalp).appl), gsMakeBinaryListExpression( (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl),
				         ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1),
                	(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
         gsDebugMsg("MinusExpression - Literal Expression parsed: \n  %T\n", ((*yyvalp).appl));
         processed = true;
       }
       /**
         *  Minus operater on sets
         *
         **/
       if (strcmp(ATgetName(ATgetAFun(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1))), "SetType") == 0 ||
           strcmp(ATgetName(ATgetAFun(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1))), "SetType") == 0
          )
       {
         if(!ContainerTypeChecking(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1),  ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1)))
	     {
           chigetposition();
		   gsErrorMsg("Incompatible Types Checking failed\n");
		   exit(1);
		 }
   		 safe_assign(((*yyvalp).appl), gsMakeBinarySetExpression( (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl),
				         ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1),
                	(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
         gsDebugMsg("MinusExpression - Set Expression parsed: \n  %T\n", ((*yyvalp).appl));
         processed = true;
       }


       if (!processed)
       {
         chigetposition();
         gsErrorMsg("Expressions %T and %T cannot be used with \"-\"\n", ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),0), ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),0));
         exit(1);
       }

	 ;}
    break;

  case 131:
#line 1675 "chiparser.yy"
    {
			/**
			  * Type Checking
			  *
			  **/
			if(  !(ContainerTypeChecking(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1),  ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1)))
              )
				{
                  chigetposition();
				  gsErrorMsg("EqualityExpression: Incompatible Types Checking failed\n");
				  exit(1);
				};

            if (strcmp(ATgetName(ATgetAFun(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1))), "SetType") == 0 )
		    {
			  safe_assign(((*yyvalp).appl), gsMakeBinarySetExpression(
                                  gsString2ATermAppl("=="),
			  		  gsMakeType( gsString2ATermAppl("Bool" )),
			  (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      		  gsDebugMsg("EqualityExpression parsed: \n  %T\n", ((*yyvalp).appl));
            } else {
			  safe_assign(((*yyvalp).appl), gsMakeBinaryExpression(
                                  gsString2ATermAppl("=="),
			  		  gsMakeType( gsString2ATermAppl("Bool" )),
			  (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      		  gsDebugMsg("EqualityExpression parsed: \n  %T\n", ((*yyvalp).appl));
            }
		;}
    break;

  case 132:
#line 1704 "chiparser.yy"
    {
			/**
			  * Type Checking
			  *
			  **/
			if(  !(ContainerTypeChecking(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1),  ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1)))
              )
				{
                  chigetposition();
				  gsErrorMsg("EqualityExpression: Incompatible Types Checking failed\n");
				  exit(1);
				};

            if (strcmp(ATgetName(ATgetAFun(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1))), "SetType") == 0 )
		    {
			  safe_assign(((*yyvalp).appl), gsMakeBinarySetExpression(
                                  gsString2ATermAppl("!="),
			  		  gsMakeType( gsString2ATermAppl("Bool" )),
			  (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      		  gsDebugMsg("EqualityExpression parsed: \n  %T\n", ((*yyvalp).appl));
            } else {
			  safe_assign(((*yyvalp).appl), gsMakeBinaryExpression( gsString2ATermAppl("!="),
			  		  gsMakeType( gsString2ATermAppl("Bool" )),
			  (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      		  gsDebugMsg("EqualityExpression parsed: \n  %T\n", ((*yyvalp).appl));
            }
		;}
    break;

  case 133:
#line 1737 "chiparser.yy"
    {
       bool processed = false;
       if (strcmp(ATgetName(ATgetAFun(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1))), "SetType") == 0 )
       {
         if(!ContainerTypeChecking(gsMakeSetType(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1)),  ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1)))
	     {
           chigetposition();
		   gsErrorMsg("Incompatible Types Checking failed\n");
		   exit(1);
		 }
   		 safe_assign(((*yyvalp).appl), gsMakeBinarySetExpression( (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl),
					gsMakeType( gsString2ATermAppl("Bool" )),
                	(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
         gsDebugMsg("MemberTest - SetLiteral parsed: \n  %T\n", ((*yyvalp).appl));
         processed = true;
       }

       if (strcmp(ATgetName(ATgetAFun(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1))), "ListType") == 0 )
         {
         if(!ContainerTypeChecking(gsMakeListType(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1)),  ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1)))
	       {
		     gsErrorMsg("Incompatible Types Checking failed\n");
		     exit(1);
		   }
 	  	 safe_assign(((*yyvalp).appl), gsMakeBinaryListExpression( (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl),
					gsMakeType( gsString2ATermAppl("Bool" )),
			        (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      	 gsDebugMsg("MemberTest - ListExpression parsed: \n  %T\n", ((*yyvalp).appl));
         processed = true;
         }

         if (!processed)
         {
           chigetposition();
           gsErrorMsg("Experrsions %T and %T cannot be used with \"in\"", ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),0), ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),0));
           exit(1);
        }
    ;}
    break;

  case 134:
#line 1779 "chiparser.yy"
    {
      	  gsDebugMsg("R:%d",__LINE__);
          ATermList tuple_type = ATmakeList0();
		  ATermList to_process = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.list);
          to_process = ATinsert(to_process, (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.appl));
		  while(!ATisEmpty(to_process))
          {
             ATerm elementType = ATgetArgument(ATgetFirst(to_process),1);
             tuple_type= ATinsert(tuple_type, elementType);

			 to_process = ATgetNext( to_process) ;
		  }
          to_process = ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.appl) );
          safe_assign(((*yyvalp).appl), gsMakeTupleLiteral( ATreverse( to_process ), gsMakeTupleType( tuple_type ) ) );
      	  gsDebugMsg("RecordExpression parsed: \n  %T\n", ((*yyvalp).appl));

     ;}
    break;

  case 135:
#line 1800 "chiparser.yy"
    {
          safe_assign(((*yyvalp).appl), gsMakeSetLiteral( ATmakeList0(), gsMakeSetType(gsMakeType(gsMakeNil()))));
      ;}
    break;

  case 136:
#line 1804 "chiparser.yy"
    {
      	  gsDebugMsg("R:%d",__LINE__);
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
               chigetposition();
               gsErrorMsg("SetLiteral contains mixed types %T and %T\n"
                         , type, elementType);
               exit(1);
             }
			 to_process = ATgetNext( to_process) ;
		  }
          safe_assign(((*yyvalp).appl), gsMakeSetLiteral( ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.list)), gsMakeSetType((ATermAppl) type)));
      	  gsDebugMsg("SetLiteral parsed: \n  %T\n", ((*yyvalp).appl));
      ;}
    break;

  case 137:
#line 1829 "chiparser.yy"
    {
	   	  /**
		    * Type Checking
		    *
		    **/
		  if(  !((ContainerTypeChecking(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1),  ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1)))
             && (strcmp(ATgetName(ATgetAFun(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1))), "SetType") == 0 ))
             )
			{
              chigetposition();
			  gsErrorMsg("r%d: Union failed: Incompatible Types Checking failed:\n %T and %T\n", __LINE__, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl));
			  exit(1);
			};

  		  safe_assign(((*yyvalp).appl), gsMakeBinarySetExpression( (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl),
				ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1),
		        (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
   		  gsDebugMsg("BinarySetExpression parsed: \n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 138:
#line 1849 "chiparser.yy"
    {
	   	  /**
		    * Type Checking
		    *
		    **/
		  if(  !((ContainerTypeChecking(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1),  ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1)))
             && (strcmp(ATgetName(ATgetAFun(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1))), "SetType") == 0 ))
             )
			{
              chigetposition();
			  gsErrorMsg("r%d: Intersection failed: Incompatible Types Checking failed:\n %T and %T\n", __LINE__, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl));
			  exit(1);
			};

  		  safe_assign(((*yyvalp).appl), gsMakeBinarySetExpression( (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl),
				ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1),
		        (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
   		  gsDebugMsg("BinarySetExpression parsed: \n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 139:
#line 1869 "chiparser.yy"
    {
	   	  /**
		    * Type Checking
		    *
		    **/
		  if(  !((ContainerTypeChecking(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1),  ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1)))
             && (strcmp(ATgetName(ATgetAFun(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1))), "SetType") == 0 ))
             )
			{
              chigetposition();
			  gsErrorMsg("r%d: Subsection failed: Incompatible Types Checking failed:\n %T and %T\n", __LINE__, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl));
			  exit(1);
			};

  		  safe_assign(((*yyvalp).appl), gsMakeBinarySetExpression( (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl),
				gsMakeType( gsString2ATermAppl("Bool" ) ),
		        (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
   		  gsDebugMsg("BinarySetExpression parsed: \n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 140:
#line 1889 "chiparser.yy"
    {
            gsDebugMsg("R:%d\n",__LINE__);
			if(!(strcmp(ATgetName(ATgetAFun(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl),1))), "ListType") == 0 ))
				{
                  chigetposition();
				  gsErrorMsg("Functions: %T cannot used on %T", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl));
				  exit(1);
				};

 	  		safe_assign(((*yyvalp).appl), gsMakeFunction( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.appl),
			     (ATermAppl) ATgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl),1),
			(((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl)));
      		gsDebugMsg("Functions parsed: \n  %T\n", ((*yyvalp).appl));
      ;}
    break;

  case 141:
#line 1907 "chiparser.yy"
    {
        safe_assign(((*yyvalp).appl), ((*yyvalp).appl));
      ;}
    break;

  case 142:
#line 1911 "chiparser.yy"
    {
			/**
			  * Type Checking
			  *
			  **/
			if(  !((ContainerTypeChecking(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1),  ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1)))
              && (strcmp(ATgetName(ATgetAFun(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1))), "ListType") == 0 ))
              )
				{
                  chigetposition();
				  gsErrorMsg("Concatination failed: Incompatible Types Checking failed:\n %T and %T\n", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl));
				  exit(1);
				};

 	  		safe_assign(((*yyvalp).appl), gsMakeBinaryListExpression( (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl),
					ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1),
			(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      		gsDebugMsg("ListExpression parsed: \n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 143:
#line 1931 "chiparser.yy"
    {
			/**
			  * Type Checking
			  *
			  **/
			if(  !((ContainerTypeChecking(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1),  ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1)))
              && (strcmp(ATgetName(ATgetAFun(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1))), "ListType") == 0 ))
              )
				{
                  chigetposition();
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};

 	  		safe_assign(((*yyvalp).appl), gsMakeBinaryListExpression( (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl),
					ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1),
			(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      		gsDebugMsg("ListExpression parsed: \n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 145:
#line 1959 "chiparser.yy"
    {
			if(!(strcmp(ATgetName(ATgetAFun(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl),1))), "ListType") == 0 ))
				{
                  chigetposition();
				  gsErrorMsg("Functions: %T cannot used on %T", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl));
				  exit(1);
				};

 	  		safe_assign(((*yyvalp).appl), gsMakeFunction( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.appl),
					gsMakeType( gsString2ATermAppl("Nat" )),
			(((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl)));
      		gsDebugMsg("Functions parsed: \n  %T\n", ((*yyvalp).appl));
      ;}
    break;

  case 146:
#line 1973 "chiparser.yy"
    {
			if(!(strcmp(ATgetName(ATgetAFun(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl),1))), "ListType") == 0 ))
				{
                  chigetposition();
				  gsErrorMsg("Functions: %T cannot used on %T", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl));
				  exit(1);
				};

 	  		safe_assign(((*yyvalp).appl), gsMakeFunction( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.appl),
				  (ATermAppl) ATgetArgument(ATgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl),1),0),
			(((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl)));
      		gsDebugMsg("Functions parsed: \n  %T\n", ((*yyvalp).appl));
      ;}
    break;

  case 147:
#line 1987 "chiparser.yy"
    {
            gsDebugMsg("R:%d\n",__LINE__);
			if(!(strcmp(ATgetName(ATgetAFun(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl),1))), "ListType") == 0 ))
				{
                  chigetposition();
				  gsErrorMsg("Functions: %T cannot used on %T", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl));
				  exit(1);
				};

 	  		safe_assign(((*yyvalp).appl), gsMakeFunction( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.appl),
			     (ATermAppl) ATgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl),1),
			(((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl)));
      		gsDebugMsg("Functions parsed: \n  %T\n", ((*yyvalp).appl));
      ;}
    break;

  case 148:
#line 2002 "chiparser.yy"
    {
            gsDebugMsg("R:%d\n",__LINE__);
			if(!(strcmp(ATgetName(ATgetAFun(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl),1))), "ListType") == 0 ))
				{
                  chigetposition();
				  gsErrorMsg("Functions: %T cannot used on %T", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl));
				  exit(1);
				};

 	  		safe_assign(((*yyvalp).appl), gsMakeFunction( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.appl),
				  (ATermAppl) ATgetArgument(ATgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl),1),0),
			(((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl)));
      		gsDebugMsg("Functions parsed: \n  %T\n", ((*yyvalp).appl));
      ;}
    break;

  case 149:
#line 2017 "chiparser.yy"
    {
            gsDebugMsg("R:%d\n",__LINE__);
			if(!(strcmp(ATgetName(ATgetAFun(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl),1))), "ListType") == 0 ))
				{
                  chigetposition();
				  gsErrorMsg("Functions: %T cannot used on %T", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl));
				  exit(1);
				};

 	  		safe_assign(((*yyvalp).appl), gsMakeFunction( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.appl),
			       (ATermAppl) ATgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl),1),
			(((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.appl)));
      		gsDebugMsg("Functions parsed: \n  %T\n", ((*yyvalp).appl));
      ;}
    break;

  case 150:
#line 2032 "chiparser.yy"
    {
            gsDebugMsg("R:%d\n",__LINE__);
			if(!(strcmp(ATgetName(ATgetAFun(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (6))].yystate.yysemantics.yysval.appl),1))), "ListType") == 0 ))
				{
                  chigetposition();
				  gsErrorMsg("Functions: %T cannot used on %T\n", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (6))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (6))].yystate.yysemantics.yysval.appl));
				  exit(1);
				};

			if(! (ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (6))].yystate.yysemantics.yysval.appl),1) == gsMakeType( gsString2ATermAppl("Nat" ) ) ) )
				{
                  chigetposition();
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

  case 151:
#line 2056 "chiparser.yy"
    {
            gsDebugMsg("R:%d\n",__LINE__);
			if(!(strcmp(ATgetName(ATgetAFun(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (6))].yystate.yysemantics.yysval.appl),1))), "ListType") == 0 ))
				{
                  chigetposition();
				  gsErrorMsg("Functions: %T cannot used on %T\n", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (6))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (6))].yystate.yysemantics.yysval.appl));
				  exit(1);
				};

			if(! (ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (6))].yystate.yysemantics.yysval.appl),1) == gsMakeType( gsString2ATermAppl("Nat" ) ) ) )
				{
                  chigetposition();
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

  case 152:
#line 2081 "chiparser.yy"
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

  case 153:
#line 2095 "chiparser.yy"
    {
        chigetposition();
        gsErrorMsg("%T is not supported", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.appl));
        exit(1);
      ;}
    break;

  case 154:
#line 2104 "chiparser.yy"
    {
          safe_assign(((*yyvalp).appl), gsMakeListLiteral( ATmakeList0(), gsMakeListType(gsMakeType(gsMakeNil()))));
      ;}
    break;

  case 155:
#line 2108 "chiparser.yy"
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
               chigetposition();
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
#line 3984 "chiparser.cpp"
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
  ((yytable_value) == YYTABLE_NINF)

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


#line 2159 "chiparser.yy"


void BinTypeCheck(ATermAppl arg1, ATermAppl arg2, std::string type)
{
    if(arg1 != arg2)
        {
          chigetposition();
          gsErrorMsg("BinTypeCheck: Incompatible Types Checking failed\n");
          exit(1);
        };
    if(arg1 != gsMakeType(gsString2ATermAppl(type.c_str())))
        {
          chigetposition();
          gsErrorMsg("Expected type %s\n", type.c_str());
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
          chigetposition();
          gsErrorMsg("UnaryTypeCheck: Incompatible Type, expected %s\n", type.c_str());
          exit(1);
        };
  return;
}

bool ContainerTypeChecking(ATermAppl arg1, ATermAppl arg2)
{
  gsDebugMsg("%s;%d\n",__FILE__,__LINE__);
  gsDebugMsg("arg1: %T\n", arg1);
  gsDebugMsg("arg2: %T\n", arg2);
  if(arg1 == arg2)
  {
    return true;
  }

  gsDebugMsg("ContainerTypeChecking: %T, %T\n",arg1, arg2);
  if((strcmp(ATgetName(ATgetAFun(arg1)), ATgetName(ATgetAFun(arg2)))==0)
     && ( ( strcmp(ATgetName(ATgetAFun(arg1)), "ListType") == 0 ) ||
          ( strcmp(ATgetName(ATgetAFun(arg1)), "SetType") == 0  )
        )

    )
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

bool is_number(std::string s)
{
   for (int i = 0; i < (int) s.length(); i++) {
       if (!std::isdigit(s[i]))
           return false;
   }

   return true;
}

