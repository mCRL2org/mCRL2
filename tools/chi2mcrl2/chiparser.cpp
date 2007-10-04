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
#define YYLAST   1217

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  67
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  48
/* YYNRULES -- Number of rules.  */
#define YYNRULES  108
/* YYNRULES -- Number of states.  */
#define YYNSTATES  198
/* YYMAXRHS -- Maximum number of symbols on right-hand side of rule.  */
#define YYMAXRHS 7
/* YYMAXLEFT -- Maximum number of symbols to the left of a handle
   accessed by $0, $-1, etc., in any rule.  */
#define YYMAXLEFT 0

/* YYTRANSLATE(X) -- Bison symbol number corresponding to X.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   319

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
      65,     2,    66,     2,     2,     2,     2,     2,     2,     2,
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
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short int yyprhs[] =
{
       0,     0,     3,     6,    13,    17,    23,    25,    28,    35,
      43,    45,    49,    55,    57,    59,    61,    65,    68,    71,
      73,    77,    79,    83,    85,    89,    91,    95,    98,   101,
     105,   107,   111,   115,   117,   121,   124,   127,   129,   133,
     137,   139,   143,   145,   147,   149,   151,   153,   157,   159,
     161,   163,   165,   169,   171,   173,   175,   181,   185,   186,
     189,   190,   193,   195,   199,   201,   205,   210,   215,   219,
     224,   228,   232,   235,   239,   242,   246,   248,   250,   252,
     254,   256,   260,   262,   264,   266,   268,   271,   274,   278,
     282,   286,   288,   291,   294,   298,   302,   306,   310,   314,
     318,   322,   326,   330,   334,   338,   342,   346,   350
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const signed char yyrhs[] =
{
      68,     0,    -1,    69,    71,    -1,     6,    76,    31,    32,
      24,    70,    -1,    19,    96,    20,    -1,    19,    77,    21,
      96,    20,    -1,    72,    -1,    71,    72,    -1,    73,    76,
      31,    32,    24,    74,    -1,    73,    76,    31,    82,    32,
      24,    74,    -1,     3,    -1,    19,    95,    75,    -1,    19,
      77,    21,    95,    75,    -1,    20,    -1,    17,    -1,    78,
      -1,    77,    23,    78,    -1,     7,    79,    -1,     9,    89,
      -1,    80,    -1,    79,    23,    80,    -1,    84,    -1,    84,
      24,   108,    -1,    84,    -1,    81,    23,    84,    -1,    83,
      -1,    82,    23,    83,    -1,     7,    81,    -1,     9,    85,
      -1,   101,    13,    93,    -1,    86,    -1,    85,    23,    86,
      -1,    87,    13,    93,    -1,    88,    -1,    87,    23,    88,
      -1,    76,    55,    -1,    76,    56,    -1,    90,    -1,    89,
      23,    90,    -1,    91,    13,    93,    -1,    92,    -1,    91,
      23,    92,    -1,    76,    -1,    94,    -1,    15,    -1,    16,
      -1,    14,    -1,    31,    95,    32,    -1,    97,    -1,   106,
      -1,   105,    -1,   104,    -1,    96,    11,    96,    -1,    98,
      -1,   103,    -1,   107,    -1,    99,   100,   109,    25,   102,
      -1,    99,   100,    10,    -1,    -1,   108,    35,    -1,    -1,
     108,    13,    -1,    76,    -1,   101,    23,    76,    -1,   108,
      -1,   102,    23,   108,    -1,    99,   108,    56,   102,    -1,
      99,   108,    55,   102,    -1,    76,    31,    32,    -1,    76,
      31,   102,    32,    -1,    95,    22,    95,    -1,    95,    12,
      95,    -1,    61,    95,    -1,   108,    62,    95,    -1,    99,
      45,    -1,    31,   108,    32,    -1,   111,    -1,   112,    -1,
     113,    -1,   114,    -1,   110,    -1,   109,    23,   110,    -1,
      76,    -1,    76,    -1,    43,    -1,    42,    -1,    36,   108,
      -1,    56,   108,    -1,   108,    33,   108,    -1,   108,    34,
     108,    -1,   108,    46,   108,    -1,    39,    -1,    27,   108,
      -1,    26,   108,    -1,   108,    54,   108,    -1,   108,    61,
     108,    -1,   108,    64,   108,    -1,   108,    27,   108,    -1,
     108,    26,   108,    -1,   108,    53,   108,    -1,   108,    52,
     108,    -1,   108,    51,   108,    -1,   108,    50,   108,    -1,
     108,    65,   108,    -1,   108,    66,   108,    -1,   108,    49,
     108,    -1,   108,    48,   108,    -1,   108,    37,   108,    -1,
     108,    47,   108,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,   114,   114,   123,   131,   135,   142,   146,   153,   170,
     186,   194,   198,   205,   214,   221,   225,   232,   237,   246,
     250,   267,   272,   292,   296,   313,   317,   324,   329,   337,
     386,   390,   397,   437,   441,   448,   453,   471,   475,   482,
     523,   527,   534,   553,   561,   566,   571,   586,   590,   591,
     592,   596,   597,   604,   605,   610,   614,   634,   642,   644,
     663,   665,   674,   678,   685,   689,   696,   711,   731,   735,
     742,   746,   757,   761,   778,   791,   806,   807,   808,   809,
     822,   826,   832,   859,   918,   927,   936,   955,   974,  1001,
    1028,  1058,  1067,  1087,  1107,  1133,  1159,  1185,  1211,  1237,
    1263,  1289,  1315,  1341,  1367,  1393,  1419,  1448,  1465
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "PROC", "MODEL_DEF", "ENUM", "MODEL",
  "VAR", "CONST", "CHAN", "SKIP", "BARS", "ALT", "COLON", "TYPE", "BOOL",
  "NAT", "ID", "TIME", "BP", "EP", "PROC_SEP", "SEP", "COMMA", "DEFINES",
  "ASSIGNMENT", "MINUS", "PLUS", "GG", "LBRACE", "RBRACE", "LBRACKET",
  "RBRACKET", "AND", "OR", "GUARD", "NOT", "EQUAL", "OLD", "NUMBER", "INT",
  "REALNUMBER", "TRUE", "FALSE", "DOT", "DEADLOCK", "IMPLIES", "NOTEQUAL",
  "GEQ", "LEQ", "MAX", "MIN", "DIV", "MOD", "POWER", "RECV", "EXCLAMATION",
  "SENDRECV", "RECVSEND", "SSEND", "RRECV", "STAR", "GUARD_REP",
  "DERIVATIVE", "DIVIDE", "'<'", "'>'", "$accept", "ChiProgram",
  "ModelDefinition", "ModelBody", "ProcessDefinitions",
  "ProcessDefinition", "ProcOpenScope", "ProcessBody", "ProcCloseScope",
  "Identifier", "LocalVariables_csp", "LocalVariables",
  "IdentifierTypeExpression_csp", "IdentifierTypeExpression",
  "IdentifierType_csp", "FormalParameter_csp", "FormalParameter",
  "IdentifierType", "ChannelDeclaration_csp", "ChannelDeclaration",
  "IdentifierChannelDeclaration_csp", "IdentifierChannelDeclaration",
  "ChannelDefinition_csp", "ChannelDefinition",
  "IdentifierChannelDefinition_csp", "IdentifierChannelDefinition", "Type",
  "BasicType", "Statement", "ModelStatement", "BasicStatement",
  "AssignmentStatement", "OptGuard", "OptChannel", "Identifier_csp",
  "Expression_csp", "CommStatement", "Instantiation", "BinaryStatement",
  "UnaryStatement", "AdvancedStatement", "Expression",
  "ExpressionIdentier_csp", "ExpressionIdentifier", "BasicExpression",
  "BooleanExpression", "NatIntExpression", "BoolNatIntExpression", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    67,    68,    69,    70,    70,    71,    71,    72,    72,
      73,    74,    74,    75,    76,    77,    77,    78,    78,    79,
      79,    80,    80,    81,    81,    82,    82,    83,    83,    84,
      85,    85,    86,    87,    87,    88,    88,    89,    89,    90,
      91,    91,    92,    93,    94,    94,    94,    95,    95,    95,
      95,    96,    96,    97,    97,    97,    98,    98,    99,    99,
     100,   100,   101,   101,   102,   102,   103,   103,   104,   104,
     105,   105,   106,   106,   107,   108,   108,   108,   108,   108,
     109,   109,   110,   111,   112,   112,   112,   112,   112,   112,
     112,   113,   113,   113,   113,   113,   113,   113,   113,   113,
     113,   113,   113,   113,   113,   113,   113,   114,   114
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     2,     6,     3,     5,     1,     2,     6,     7,
       1,     3,     5,     1,     1,     1,     3,     2,     2,     1,
       3,     1,     3,     1,     3,     1,     3,     2,     2,     3,
       1,     3,     3,     1,     3,     2,     2,     1,     3,     3,
       1,     3,     1,     1,     1,     1,     1,     3,     1,     1,
       1,     1,     3,     1,     1,     1,     5,     3,     0,     2,
       0,     2,     1,     3,     1,     3,     4,     4,     3,     4,
       3,     3,     2,     3,     2,     3,     1,     1,     1,     1,
       1,     3,     1,     1,     1,     1,     2,     2,     3,     3,
       3,     1,     2,     2,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3
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
       0,     0,     0,     0,     0,     0,     0,     0,     0
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error.  */
static const unsigned char yydefact[] =
{
       0,     0,     0,     0,    14,     0,     1,    10,     2,     6,
       0,     0,     7,     0,     0,     0,     0,     0,     0,     0,
       0,    25,     0,     3,    62,    27,    23,     0,     0,    28,
      30,     0,    33,     0,     0,     0,     0,     0,     0,     0,
      15,     0,    51,     0,     0,     0,    35,    36,     0,     0,
       0,    58,     8,    26,     0,    17,    19,    21,    42,    18,
      37,     0,    40,     0,     0,     0,     0,     4,    24,    46,
      44,    45,    29,    43,    63,    31,    32,    34,     0,     0,
      58,     0,    91,    85,    84,     0,    58,    83,     0,     0,
      48,    53,    60,    54,    50,    49,    55,     0,    76,    77,
      78,    79,     9,     0,     0,     0,     0,     0,     0,    68,
       0,    64,     0,    16,    52,    93,    92,     0,     0,    86,
      87,    72,    58,    58,    13,    58,    11,    74,     0,     0,
       0,     0,     0,     0,    59,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    58,     0,     0,     0,
      20,    22,    38,    39,    41,     0,     0,    69,     5,    47,
      75,     0,    71,    70,    57,    82,     0,    80,    61,     0,
       0,    98,    97,    88,    89,   107,    90,   108,   106,   105,
     102,   101,   100,    99,    94,    95,    73,    96,   103,   104,
      65,    12,     0,     0,    67,    66,    81,    56
};

/* YYPDEFGOTO[NTERM-NUM].  */
static const short int yydefgoto[] =
{
      -1,     2,     3,    23,     8,     9,    10,    52,   126,    87,
      39,    40,    55,    56,    25,    20,    21,    57,    29,    30,
      31,    32,    59,    60,    61,    62,    72,    73,    89,    41,
      90,    91,    92,   128,    27,   110,    93,    42,    94,    95,
      96,    97,   166,   167,    98,    99,   100,   101
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -156
static const short int yypact[] =
{
      13,    16,    59,    38,  -156,    23,  -156,  -156,    38,  -156,
      16,    30,  -156,    44,    43,    -2,    53,    16,    16,    57,
     -12,  -156,    36,  -156,  -156,    70,  -156,    -5,    74,    85,
    -156,     0,  -156,    95,    -3,    97,    16,    16,    93,    47,
    -156,    41,  -156,    16,    96,    16,  -156,  -156,    16,    96,
      16,    92,  -156,  -156,    95,   103,  -156,   108,  -156,   110,
    -156,    11,  -156,   985,    16,    91,    16,  -156,  -156,  -156,
    -156,  -156,  -156,  -156,  -156,  -156,  -156,  -156,  1008,  1008,
     153,  1008,  -156,  -156,  -156,  1008,   176,  -156,    84,    28,
    -156,  -156,   987,  -156,  -156,  -156,  -156,   327,  -156,  -156,
    -156,  -156,  -156,    16,  1008,    16,    96,    16,  1008,  -156,
      37,   404,    46,  -156,   125,  1019,  1041,   -10,   286,   440,
     476,    34,   199,   222,  -156,   245,  -156,  -156,    54,   112,
    1008,  1008,  1008,  1008,  -156,  1008,  1008,  1008,  1008,  1008,
    1008,  1008,  1008,  1008,  1008,  1008,   268,  1008,  1008,  1008,
    -156,   404,  -156,  -156,  -156,   368,  1008,  -156,  -156,  -156,
    -156,    28,    34,    34,  -156,  -156,    90,  -156,  -156,  1008,
    1008,  1063,  1085,   512,   548,   584,   620,   656,   692,   728,
     764,   800,   836,   872,  1107,  1129,    34,  1151,   908,   944,
     404,  -156,    16,  1008,   114,   114,  -156,   114
};

/* YYPGOTO[NTERM-NUM].  */
static const short int yypgoto[] =
{
    -156,  -156,  -156,  -156,  -156,   133,  -156,    88,   -18,    -1,
      99,    79,  -156,    48,  -156,  -156,   113,   -14,  -156,   104,
    -156,   105,  -156,    49,  -156,    50,   -48,  -156,   -49,    56,
    -156,  -156,  -156,  -156,  -156,  -155,  -156,  -156,  -156,  -156,
    -156,   -53,  -156,   -36,  -156,  -156,  -156,  -156
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const unsigned char yytable[] =
{
       5,    76,   123,    26,    17,    17,    18,    18,    44,    13,
     111,    34,   125,    49,   194,   195,    24,    28,    45,     1,
      35,    38,   159,    50,   106,   115,   116,   118,   119,    68,
      19,   117,   120,     4,   107,    24,    58,   121,   197,   129,
     123,     7,    24,    36,    74,    37,   123,    28,   124,    28,
     125,   151,    66,     4,    11,   155,   125,    66,   153,     6,
     156,    67,    14,    38,   164,    38,   158,    16,    64,   157,
      65,     4,    22,   161,   162,    15,   163,   171,   172,   173,
     174,    33,   175,   176,   177,   178,   179,   180,   181,   182,
     183,   184,   185,    43,   187,   188,   189,   186,    36,    36,
      37,    37,    24,   190,    58,   122,    58,    65,    48,     4,
      69,    70,    71,   192,    51,   193,   111,   111,    78,    79,
     112,    54,   114,    80,    63,   168,   103,   165,    81,    46,
      47,    82,   104,   105,    83,    84,    66,   156,   130,   131,
     111,    12,   102,   191,   113,   132,   133,    53,    85,   135,
      88,   150,    75,    86,   152,    77,   196,   154,   136,   137,
     138,   139,   140,   141,   142,   143,   144,   169,   170,     0,
       4,     0,     0,   145,     0,     0,   147,   148,   149,    78,
      79,     0,     0,     0,    80,     0,     0,     0,     0,    81,
       0,   165,    82,     4,     0,    83,    84,     0,     0,     0,
       0,     0,    78,    79,     0,     0,     0,    80,     0,    85,
       0,     0,    81,     0,    86,    82,     4,     0,    83,    84,
       0,     0,     0,     0,     0,    78,    79,     0,     0,     0,
      80,     0,    85,     0,     0,    81,     0,    86,    82,     4,
       0,    83,    84,     0,     0,     0,     0,     0,    78,    79,
       0,     0,     0,    80,     0,    85,     0,     0,    81,     0,
      86,    82,     4,     0,    83,    84,     0,     0,     0,     0,
       0,    78,    79,     0,     0,     0,    80,     0,    85,     0,
       0,    81,     0,    86,    82,     4,     0,    83,    84,     0,
       0,     0,     0,     0,    78,    79,     0,     0,     0,    80,
       0,    85,     0,     0,    81,     0,    86,    82,     0,     0,
      83,    84,   130,   131,     0,     0,     0,     0,   160,   132,
     133,   134,     0,   135,    85,     0,     0,     0,     0,    86,
       0,     0,   136,   137,   138,   139,   140,   141,   142,   143,
     144,     0,     0,     0,     0,     0,     0,   145,   146,     0,
     147,   148,   149,   130,   131,     0,     0,     0,     0,     0,
     132,   133,   134,     0,   135,     0,     0,     0,     0,     0,
       0,     0,     0,   136,   137,   138,   139,   140,   141,   142,
     143,   144,     0,     0,     0,     0,     0,     0,   145,   146,
       0,   147,   148,   149,   130,   131,     0,     0,     0,     0,
     160,   132,   133,     0,     0,   135,     0,     0,     0,     0,
       0,     0,     0,     0,   136,   137,   138,   139,   140,   141,
     142,   143,   144,     0,     0,     0,     0,     0,     0,   145,
     130,   131,   147,   148,   149,     0,     0,   132,   133,     0,
       0,   135,     0,     0,     0,     0,     0,     0,     0,     0,
     136,   137,   138,   139,   140,   141,   142,   143,   144,     0,
       0,     0,     0,     0,     0,   145,   130,   131,   147,   148,
     149,     0,     0,   132,   133,     0,     0,   135,     0,     0,
       0,     0,     0,     0,     0,     0,   136,   137,   138,   139,
     140,   141,   142,   143,   144,     0,     0,     0,     0,     0,
       0,   145,   130,   131,   147,   148,   149,     0,     0,   132,
     133,     0,     0,   135,     0,     0,     0,     0,     0,     0,
       0,     0,   136,   137,   138,   139,   140,   141,   142,   143,
     144,     0,     0,     0,     0,     0,     0,   145,   130,   131,
     147,   148,   149,     0,     0,   132,   133,     0,     0,   135,
       0,     0,     0,     0,     0,     0,     0,     0,   136,   137,
     138,   139,   140,   141,   142,   143,   144,     0,     0,     0,
       0,     0,     0,   145,   130,   131,   147,   148,   149,     0,
       0,   132,   133,     0,     0,   135,     0,     0,     0,     0,
       0,     0,     0,     0,   136,   137,   138,   139,   140,   141,
     142,   143,   144,     0,     0,     0,     0,     0,     0,   145,
     130,   131,   147,   148,   149,     0,     0,   132,   133,     0,
       0,   135,     0,     0,     0,     0,     0,     0,     0,     0,
     136,   137,   138,   139,   140,   141,   142,   143,   144,     0,
       0,     0,     0,     0,     0,   145,   130,   131,   147,   148,
     149,     0,     0,   132,   133,     0,     0,   135,     0,     0,
       0,     0,     0,     0,     0,     0,   136,   137,   138,   139,
     140,   141,   142,   143,   144,     0,     0,     0,     0,     0,
       0,   145,   130,   131,   147,   148,   149,     0,     0,   132,
     133,     0,     0,   135,     0,     0,     0,     0,     0,     0,
       0,     0,   136,   137,   138,   139,   140,   141,   142,   143,
     144,     0,     0,     0,     0,     0,     0,   145,   130,   131,
     147,   148,   149,     0,     0,   132,   133,     0,     0,   135,
       0,     0,     0,     0,     0,     0,     0,     0,   136,   137,
     138,   139,   140,   141,   142,   143,   144,     0,     0,     0,
       0,     0,     0,   145,   130,   131,   147,   148,   149,     0,
       0,   132,   133,     0,     0,   135,     0,     0,     0,     0,
       0,     0,     0,     0,   136,   137,   138,   139,   140,   141,
     142,   143,   144,     0,     0,     0,     0,     0,     0,   145,
     130,   131,   147,   148,   149,     0,     0,   132,   133,     0,
       0,   135,     0,     0,     0,     0,     0,     0,     0,     0,
     136,   137,   138,   139,   140,   141,   142,   143,   144,     0,
       0,     0,     0,     0,     0,   145,   130,   131,   147,   148,
     149,     0,     0,   132,   133,     0,     0,   135,     0,     0,
       0,     0,     0,     0,     0,     0,   136,   137,   138,   139,
     140,   141,   142,   143,   144,     0,     0,     0,     0,     0,
       0,   145,   130,   131,   147,   148,   149,     0,     0,   132,
     133,     0,     0,   135,     0,     0,     0,     0,     0,     0,
       0,     0,   136,   137,   138,   139,   140,   141,   142,   143,
     144,     0,     0,     0,     0,     0,     0,   145,   130,   131,
     147,   148,   149,     0,     0,   132,   133,     0,     0,   135,
       0,     0,     0,     0,     0,     0,     0,     0,   136,   137,
     138,   139,   140,   141,   142,   143,   144,     0,     0,     0,
       0,     0,     0,   145,   130,   131,   147,   148,   149,     0,
       0,   132,   133,     0,     0,   135,     0,     0,     0,     0,
       0,     0,     0,     0,   136,   137,   138,   139,   140,   141,
     142,   143,   144,     0,     0,     0,     0,     0,     0,   145,
     130,   131,   147,   148,   149,     0,     0,   132,   133,     0,
       0,   135,     0,     0,     0,     0,     0,     0,     0,     0,
     136,   137,   138,   139,   140,   141,   142,   143,   144,     0,
       0,     0,     4,     0,     4,   145,     0,     0,   147,   148,
     149,    78,    79,    78,    79,     0,   108,   109,   108,     0,
       0,    81,     0,    81,    82,     4,    82,    83,    84,    83,
      84,     0,   127,     0,    78,    79,     0,     0,     0,   108,
       0,    85,     0,    85,    81,     0,     0,    82,     0,     0,
      83,    84,   132,   133,     0,     0,   135,     0,     0,     0,
       0,     0,     0,     0,    85,   136,   137,   138,   139,   140,
     141,   142,   143,   144,   132,   133,     0,     0,   135,     0,
     145,     0,     0,   147,   148,   149,     0,   136,   137,   138,
     139,   140,   141,   142,   143,   144,   132,   133,     0,     0,
     135,     0,   145,     0,     0,   147,   148,   149,     0,   136,
     137,   138,   139,   140,   141,   142,   143,   144,   132,   133,
       0,     0,   135,     0,   145,     0,     0,   147,   148,   149,
       0,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     132,   133,     0,     0,   135,     0,   145,     0,     0,   147,
     148,   149,     0,   136,   137,   138,   139,   140,   141,   142,
     143,   144,   132,   133,     0,     0,   135,     0,   145,     0,
       0,     0,   148,   149,     0,   136,   137,   138,   139,   140,
     141,   142,   143,   144,   132,   133,     0,     0,   135,     0,
     145,     0,     0,     0,   148,   149,     0,   136,   137,   138,
     139,   140,   141,   142,   143,   144,     0,     0,     0,     0,
       0,     0,   145,     0,     0,     0,   148,   149
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
       0,     0,     0,     1,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     3,     5,
       0,     0,     0,     0,     0,     0,     0,     0,     7,     9,
       0,     0,     0,    11,     0,     0,    23,     0,    13,     0,
       0,    15,     0,    25,    17,    19,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    21,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      27,     0,     0,     0,     0,     0,     0,     0,     0,    29,
      31,     0,     0,     0,    33,     0,     0,     0,     0,    35,
       0,     0,    37,    45,     0,    39,    41,     0,     0,     0,
       0,     0,    47,    49,     0,     0,     0,    51,     0,    43,
       0,     0,    53,     0,     0,    55,   189,     0,    57,    59,
       0,     0,     0,     0,     0,   191,   193,     0,     0,     0,
     195,     0,    61,     0,     0,   197,     0,     0,   199,   207,
       0,   201,   203,     0,     0,     0,     0,     0,   209,   211,
       0,     0,     0,   213,     0,   205,     0,     0,   215,     0,
       0,   217,   225,     0,   219,   221,     0,     0,     0,     0,
       0,   227,   229,     0,     0,     0,   231,     0,   223,     0,
       0,   233,     0,     0,   235,   243,     0,   237,   239,     0,
       0,     0,     0,     0,   245,   247,     0,     0,     0,   249,
       0,   241,     0,     0,   251,     0,     0,   253,     0,     0,
     255,   257,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   259,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   117,   119,     0,     0,
       0,     0,     0,   121,   123,     0,     0,   125,     0,     0,
       0,     0,     0,     0,     0,     0,   127,   129,   131,   133,
     135,   137,   139,   141,   143,     0,     0,     0,     0,     0,
       0,   145,   153,   155,   147,   149,   151,     0,     0,   157,
     159,     0,     0,   161,     0,     0,     0,     0,     0,     0,
       0,     0,   163,   165,   167,   169,   171,   173,   175,   177,
     179,     0,     0,     0,     0,     0,     0,   181,   313,   315,
     183,   185,   187,     0,     0,   317,   319,     0,     0,   321,
       0,     0,     0,     0,     0,     0,     0,     0,   323,   325,
     327,   329,   331,   333,   335,   337,   339,     0,     0,     0,
       0,     0,     0,   341,   349,   351,   343,   345,   347,     0,
       0,   353,   355,     0,     0,   357,     0,     0,     0,     0,
       0,     0,     0,     0,   359,   361,   363,   365,   367,   369,
     371,   373,   375,     0,     0,     0,     0,     0,     0,   377,
     385,   387,   379,   381,   383,     0,     0,   389,   391,     0,
       0,   393,     0,     0,     0,     0,     0,     0,     0,     0,
     395,   397,   399,   401,   403,   405,   407,   409,   411,     0,
       0,     0,     0,     0,     0,   413,   421,   423,   415,   417,
     419,     0,     0,   425,   427,     0,     0,   429,     0,     0,
       0,     0,     0,     0,     0,     0,   431,   433,   435,   437,
     439,   441,   443,   445,   447,     0,     0,     0,     0,     0,
       0,   449,   457,   459,   451,   453,   455,     0,     0,   461,
     463,     0,     0,   465,     0,     0,     0,     0,     0,     0,
       0,     0,   467,   469,   471,   473,   475,   477,   479,   481,
     483,     0,     0,     0,     0,     0,     0,   485,   493,   495,
     487,   489,   491,     0,     0,   497,   499,     0,     0,   501,
       0,     0,     0,     0,     0,     0,     0,     0,   503,   505,
     507,   509,   511,   513,   515,   517,   519,     0,     0,     0,
       0,     0,     0,   521,   529,   531,   523,   525,   527,     0,
       0,   533,   535,     0,     0,   537,     0,     0,     0,     0,
       0,     0,     0,     0,   539,   541,   543,   545,   547,   549,
     551,   553,   555,     0,     0,     0,     0,     0,     0,   557,
     565,   567,   559,   561,   563,     0,     0,   569,   571,     0,
       0,   573,     0,     0,     0,     0,     0,     0,     0,     0,
     575,   577,   579,   581,   583,   585,   587,   589,   591,     0,
       0,     0,     0,     0,     0,   593,   601,   603,   595,   597,
     599,     0,     0,   605,   607,     0,     0,   609,     0,     0,
       0,     0,     0,     0,     0,     0,   611,   613,   615,   617,
     619,   621,   623,   625,   627,     0,     0,     0,     0,     0,
       0,   629,   637,   639,   631,   633,   635,     0,     0,   641,
     643,     0,     0,   645,     0,     0,     0,     0,     0,     0,
       0,     0,   647,   649,   651,   653,   655,   657,   659,   661,
     663,     0,     0,     0,     0,     0,     0,   665,   673,   675,
     667,   669,   671,     0,     0,   677,   679,     0,     0,   681,
       0,     0,     0,     0,     0,     0,     0,     0,   683,   685,
     687,   689,   691,   693,   695,   697,   699,     0,     0,     0,
       0,     0,     0,   701,   787,   789,   703,   705,   707,     0,
       0,   791,   793,     0,     0,   795,     0,     0,     0,     0,
       0,     0,     0,     0,   797,   799,   801,   803,   805,   807,
     809,   811,   813,     0,     0,     0,     0,     0,     0,   815,
     823,   825,   817,   819,   821,     0,     0,   827,   829,     0,
       0,   831,     0,     0,     0,     0,     0,     0,     0,     0,
     833,   835,   837,   839,   841,   843,   845,   847,   849,     0,
       0,     0,     0,     0,    63,   851,     0,     0,   853,   855,
     857,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    65,    67,     0,     0,    69,     0,     0,     0,
       0,     0,     0,     0,     0,    71,    73,    75,    77,    79,
      81,    83,    85,     0,    91,    93,     0,     0,    95,     0,
       0,     0,     0,     0,    87,    89,     0,    97,    99,   101,
     103,   105,   107,   109,   111,     0,   261,   263,     0,     0,
     265,     0,     0,     0,     0,     0,   113,   115,     0,   267,
     269,   271,   273,   275,   277,   279,   281,     0,   287,   289,
       0,     0,   291,     0,     0,     0,     0,     0,   283,   285,
       0,   293,   295,   297,   299,   301,   303,   305,   307,     0,
     709,   711,     0,     0,   713,     0,     0,     0,     0,     0,
     309,   311,     0,   715,   717,   719,   721,   723,   725,   727,
     729,     0,   735,   737,     0,     0,   739,     0,     0,     0,
       0,     0,   731,   733,     0,   741,   743,   745,   747,   749,
     751,   753,   755,     0,   761,   763,     0,     0,   765,     0,
       0,     0,     0,     0,   757,   759,     0,   767,   769,   771,
     773,   775,   777,   779,   781,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   783,   785
};

/* YYCONFL[I] -- lists of conflicting rule numbers, each terminated by
   0, pointed into by YYCONFLP.  */
static const short int yyconfl[] =
{
       0,    27,     0,    28,     0,    58,     0,    58,     0,    58,
       0,    58,     0,    58,     0,    58,     0,    58,     0,    58,
       0,    58,     0,    17,     0,    18,     0,    58,     0,    58,
       0,    58,     0,    58,     0,    58,     0,    58,     0,    58,
       0,    58,     0,    58,     0,    58,     0,    58,     0,    58,
       0,    58,     0,    58,     0,    58,     0,    58,     0,    58,
       0,    58,     0,    60,     0,    93,     0,    93,     0,    93,
       0,    93,     0,    93,     0,    93,     0,    93,     0,    93,
       0,    93,     0,    93,     0,    93,     0,    93,     0,    93,
       0,    92,     0,    92,     0,    92,     0,    92,     0,    92,
       0,    92,     0,    92,     0,    92,     0,    92,     0,    92,
       0,    92,     0,    92,     0,    92,     0,    86,     0,    86,
       0,    86,     0,    86,     0,    86,     0,    86,     0,    86,
       0,    86,     0,    86,     0,    86,     0,    86,     0,    86,
       0,    86,     0,    86,     0,    86,     0,    86,     0,    86,
       0,    86,     0,    87,     0,    87,     0,    87,     0,    87,
       0,    87,     0,    87,     0,    87,     0,    87,     0,    87,
       0,    87,     0,    87,     0,    87,     0,    87,     0,    87,
       0,    87,     0,    87,     0,    87,     0,    87,     0,    58,
       0,    58,     0,    58,     0,    58,     0,    58,     0,    58,
       0,    58,     0,    58,     0,    58,     0,    58,     0,    58,
       0,    58,     0,    58,     0,    58,     0,    58,     0,    58,
       0,    58,     0,    58,     0,    58,     0,    58,     0,    58,
       0,    58,     0,    58,     0,    58,     0,    58,     0,    58,
       0,    58,     0,    58,     0,    58,     0,    58,     0,    58,
       0,    58,     0,    58,     0,    58,     0,    58,     0,    58,
       0,    98,     0,    98,     0,    98,     0,    98,     0,    98,
       0,    98,     0,    98,     0,    98,     0,    98,     0,    98,
       0,    98,     0,    98,     0,    98,     0,    97,     0,    97,
       0,    97,     0,    97,     0,    97,     0,    97,     0,    97,
       0,    97,     0,    97,     0,    97,     0,    97,     0,    97,
       0,    97,     0,    88,     0,    88,     0,    88,     0,    88,
       0,    88,     0,    88,     0,    88,     0,    88,     0,    88,
       0,    88,     0,    88,     0,    88,     0,    88,     0,    88,
       0,    88,     0,    88,     0,    88,     0,    88,     0,    89,
       0,    89,     0,    89,     0,    89,     0,    89,     0,    89,
       0,    89,     0,    89,     0,    89,     0,    89,     0,    89,
       0,    89,     0,    89,     0,    89,     0,    89,     0,    89,
       0,    89,     0,    89,     0,   107,     0,   107,     0,   107,
       0,   107,     0,   107,     0,   107,     0,   107,     0,   107,
       0,   107,     0,   107,     0,   107,     0,   107,     0,   107,
       0,   107,     0,   107,     0,   107,     0,   107,     0,   107,
       0,    90,     0,    90,     0,    90,     0,    90,     0,    90,
       0,    90,     0,    90,     0,    90,     0,    90,     0,    90,
       0,    90,     0,    90,     0,    90,     0,    90,     0,    90,
       0,    90,     0,    90,     0,    90,     0,   108,     0,   108,
       0,   108,     0,   108,     0,   108,     0,   108,     0,   108,
       0,   108,     0,   108,     0,   108,     0,   108,     0,   108,
       0,   108,     0,   108,     0,   108,     0,   108,     0,   108,
       0,   108,     0,   106,     0,   106,     0,   106,     0,   106,
       0,   106,     0,   106,     0,   106,     0,   106,     0,   106,
       0,   106,     0,   106,     0,   106,     0,   106,     0,   106,
       0,   106,     0,   106,     0,   106,     0,   106,     0,   105,
       0,   105,     0,   105,     0,   105,     0,   105,     0,   105,
       0,   105,     0,   105,     0,   105,     0,   105,     0,   105,
       0,   105,     0,   105,     0,   105,     0,   105,     0,   105,
       0,   105,     0,   105,     0,   102,     0,   102,     0,   102,
       0,   102,     0,   102,     0,   102,     0,   102,     0,   102,
       0,   102,     0,   102,     0,   102,     0,   102,     0,   102,
       0,   102,     0,   102,     0,   102,     0,   102,     0,   102,
       0,   101,     0,   101,     0,   101,     0,   101,     0,   101,
       0,   101,     0,   101,     0,   101,     0,   101,     0,   101,
       0,   101,     0,   101,     0,   101,     0,   101,     0,   101,
       0,   101,     0,   101,     0,   101,     0,   100,     0,   100,
       0,   100,     0,   100,     0,   100,     0,   100,     0,   100,
       0,   100,     0,   100,     0,   100,     0,   100,     0,   100,
       0,   100,     0,   100,     0,   100,     0,   100,     0,   100,
       0,   100,     0,    99,     0,    99,     0,    99,     0,    99,
       0,    99,     0,    99,     0,    99,     0,    99,     0,    99,
       0,    99,     0,    99,     0,    99,     0,    99,     0,    99,
       0,    99,     0,    99,     0,    99,     0,    99,     0,    94,
       0,    94,     0,    94,     0,    94,     0,    94,     0,    94,
       0,    94,     0,    94,     0,    94,     0,    94,     0,    94,
       0,    94,     0,    94,     0,    95,     0,    95,     0,    95,
       0,    95,     0,    95,     0,    95,     0,    95,     0,    95,
       0,    95,     0,    95,     0,    95,     0,    95,     0,    95,
       0,    96,     0,    96,     0,    96,     0,    96,     0,    96,
       0,    96,     0,    96,     0,    96,     0,    96,     0,    96,
       0,    96,     0,    96,     0,    96,     0,   103,     0,   103,
       0,   103,     0,   103,     0,   103,     0,   103,     0,   103,
       0,   103,     0,   103,     0,   103,     0,   103,     0,   103,
       0,   103,     0,   103,     0,   103,     0,   103,     0,   103,
       0,   103,     0,   104,     0,   104,     0,   104,     0,   104,
       0,   104,     0,   104,     0,   104,     0,   104,     0,   104,
       0,   104,     0,   104,     0,   104,     0,   104,     0,   104,
       0,   104,     0,   104,     0,   104,     0,   104,     0
};

static const short int yycheck[] =
{
       1,    49,    12,    17,     7,     7,     9,     9,    13,    10,
      63,    23,    22,    13,   169,   170,    17,    18,    23,     6,
      32,    22,    32,    23,    13,    78,    79,    80,    81,    43,
      32,    80,    85,    17,    23,    36,    37,    86,   193,    92,
      12,     3,    43,     7,    45,     9,    12,    48,    20,    50,
      22,   104,    11,    17,    31,   108,    22,    11,   106,     0,
      23,    20,    32,    64,    10,    66,    20,    24,    21,    32,
      23,    17,    19,   122,   123,    31,   125,   130,   131,   132,
     133,    24,   135,   136,   137,   138,   139,   140,   141,   142,
     143,   144,   145,    23,   147,   148,   149,   146,     7,     7,
       9,     9,   103,   156,   105,    21,   107,    23,    23,    17,
      14,    15,    16,    23,    19,    25,   169,   170,    26,    27,
      64,    24,    66,    31,    31,    13,    23,   128,    36,    55,
      56,    39,    24,    23,    42,    43,    11,    23,    26,    27,
     193,     8,    54,   161,    65,    33,    34,    34,    56,    37,
      51,   103,    48,    61,   105,    50,   192,   107,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    -1,
      17,    -1,    -1,    61,    -1,    -1,    64,    65,    66,    26,
      27,    -1,    -1,    -1,    31,    -1,    -1,    -1,    -1,    36,
      -1,   192,    39,    17,    -1,    42,    43,    -1,    -1,    -1,
      -1,    -1,    26,    27,    -1,    -1,    -1,    31,    -1,    56,
      -1,    -1,    36,    -1,    61,    39,    17,    -1,    42,    43,
      -1,    -1,    -1,    -1,    -1,    26,    27,    -1,    -1,    -1,
      31,    -1,    56,    -1,    -1,    36,    -1,    61,    39,    17,
      -1,    42,    43,    -1,    -1,    -1,    -1,    -1,    26,    27,
      -1,    -1,    -1,    31,    -1,    56,    -1,    -1,    36,    -1,
      61,    39,    17,    -1,    42,    43,    -1,    -1,    -1,    -1,
      -1,    26,    27,    -1,    -1,    -1,    31,    -1,    56,    -1,
      -1,    36,    -1,    61,    39,    17,    -1,    42,    43,    -1,
      -1,    -1,    -1,    -1,    26,    27,    -1,    -1,    -1,    31,
      -1,    56,    -1,    -1,    36,    -1,    61,    39,    -1,    -1,
      42,    43,    26,    27,    -1,    -1,    -1,    -1,    32,    33,
      34,    35,    -1,    37,    56,    -1,    -1,    -1,    -1,    61,
      -1,    -1,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    -1,    -1,    -1,    -1,    -1,    -1,    61,    62,    -1,
      64,    65,    66,    26,    27,    -1,    -1,    -1,    -1,    -1,
      33,    34,    35,    -1,    37,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    -1,    -1,    -1,    -1,    -1,    -1,    61,    62,
      -1,    64,    65,    66,    26,    27,    -1,    -1,    -1,    -1,
      32,    33,    34,    -1,    -1,    37,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    -1,    -1,    -1,    -1,    -1,    -1,    61,
      26,    27,    64,    65,    66,    -1,    -1,    33,    34,    -1,
      -1,    37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    -1,
      -1,    -1,    -1,    -1,    -1,    61,    26,    27,    64,    65,
      66,    -1,    -1,    33,    34,    -1,    -1,    37,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    -1,    -1,    -1,    -1,    -1,
      -1,    61,    26,    27,    64,    65,    66,    -1,    -1,    33,
      34,    -1,    -1,    37,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    -1,    -1,    -1,    -1,    -1,    -1,    61,    26,    27,
      64,    65,    66,    -1,    -1,    33,    34,    -1,    -1,    37,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    -1,    -1,    -1,
      -1,    -1,    -1,    61,    26,    27,    64,    65,    66,    -1,
      -1,    33,    34,    -1,    -1,    37,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    -1,    -1,    -1,    -1,    -1,    -1,    61,
      26,    27,    64,    65,    66,    -1,    -1,    33,    34,    -1,
      -1,    37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    -1,
      -1,    -1,    -1,    -1,    -1,    61,    26,    27,    64,    65,
      66,    -1,    -1,    33,    34,    -1,    -1,    37,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    -1,    -1,    -1,    -1,    -1,
      -1,    61,    26,    27,    64,    65,    66,    -1,    -1,    33,
      34,    -1,    -1,    37,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    -1,    -1,    -1,    -1,    -1,    -1,    61,    26,    27,
      64,    65,    66,    -1,    -1,    33,    34,    -1,    -1,    37,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    -1,    -1,    -1,
      -1,    -1,    -1,    61,    26,    27,    64,    65,    66,    -1,
      -1,    33,    34,    -1,    -1,    37,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    -1,    -1,    -1,    -1,    -1,    -1,    61,
      26,    27,    64,    65,    66,    -1,    -1,    33,    34,    -1,
      -1,    37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    -1,
      -1,    -1,    -1,    -1,    -1,    61,    26,    27,    64,    65,
      66,    -1,    -1,    33,    34,    -1,    -1,    37,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    -1,    -1,    -1,    -1,    -1,
      -1,    61,    26,    27,    64,    65,    66,    -1,    -1,    33,
      34,    -1,    -1,    37,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    -1,    -1,    -1,    -1,    -1,    -1,    61,    26,    27,
      64,    65,    66,    -1,    -1,    33,    34,    -1,    -1,    37,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    -1,    -1,    -1,
      -1,    -1,    -1,    61,    26,    27,    64,    65,    66,    -1,
      -1,    33,    34,    -1,    -1,    37,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    -1,    -1,    -1,    -1,    -1,    -1,    61,
      26,    27,    64,    65,    66,    -1,    -1,    33,    34,    -1,
      -1,    37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    -1,
      -1,    -1,    17,    -1,    17,    61,    -1,    -1,    64,    65,
      66,    26,    27,    26,    27,    -1,    31,    32,    31,    -1,
      -1,    36,    -1,    36,    39,    17,    39,    42,    43,    42,
      43,    -1,    45,    -1,    26,    27,    -1,    -1,    -1,    31,
      -1,    56,    -1,    56,    36,    -1,    -1,    39,    -1,    -1,
      42,    43,    33,    34,    -1,    -1,    37,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    56,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    33,    34,    -1,    -1,    37,    -1,
      61,    -1,    -1,    64,    65,    66,    -1,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    33,    34,    -1,    -1,
      37,    -1,    61,    -1,    -1,    64,    65,    66,    -1,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    33,    34,
      -1,    -1,    37,    -1,    61,    -1,    -1,    64,    65,    66,
      -1,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      33,    34,    -1,    -1,    37,    -1,    61,    -1,    -1,    64,
      65,    66,    -1,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    33,    34,    -1,    -1,    37,    -1,    61,    -1,
      -1,    -1,    65,    66,    -1,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    33,    34,    -1,    -1,    37,    -1,
      61,    -1,    -1,    -1,    65,    66,    -1,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    -1,    -1,    -1,    -1,
      -1,    -1,    61,    -1,    -1,    -1,    65,    66
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     6,    68,    69,    17,    76,     0,     3,    71,    72,
      73,    31,    72,    76,    32,    31,    24,     7,     9,    32,
      82,    83,    19,    70,    76,    81,    84,   101,    76,    85,
      86,    87,    88,    24,    23,    32,     7,     9,    76,    77,
      78,    96,   104,    23,    13,    23,    55,    56,    23,    13,
      23,    19,    74,    83,    24,    79,    80,    84,    76,    89,
      90,    91,    92,    31,    21,    23,    11,    20,    84,    14,
      15,    16,    93,    94,    76,    86,    93,    88,    26,    27,
      31,    36,    39,    42,    43,    56,    61,    76,    77,    95,
      97,    98,    99,   103,   105,   106,   107,   108,   111,   112,
     113,   114,    74,    23,    24,    23,    13,    23,    31,    32,
     102,   108,    96,    78,    96,   108,   108,    95,   108,   108,
     108,    95,    21,    12,    20,    22,    75,    45,   100,   108,
      26,    27,    33,    34,    35,    37,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    61,    62,    64,    65,    66,
      80,   108,    90,    93,    92,   108,    23,    32,    20,    32,
      32,    95,    95,    95,    10,    76,   109,   110,    13,    55,
      56,   108,   108,   108,   108,   108,   108,   108,   108,   108,
     108,   108,   108,   108,   108,   108,    95,   108,   108,   108,
     108,    75,    23,    25,   102,   102,   110,   102
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
#line 115 "chiparser.yy"
    { 
    	  gsDebugMsg("inputs contains a valid Chi-specification\n"); 
          safe_assign(((*yyvalp).appl), gsMakeChiSpec((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl),ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.list))));
		  spec_tree = ((*yyvalp).appl);
		;}
    break;

  case 3:
#line 124 "chiparser.yy"
    {
      	  safe_assign(((*yyvalp).appl), gsMakeModelDef((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (6))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((6) - (6))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed Model Def \n  %T\n", ((*yyvalp).appl));
        ;}
    break;

  case 4:
#line 132 "chiparser.yy"
    { safe_assign(((*yyvalp).appl), gsMakeModelSpec( ATmakeList0(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl) ));
      	  gsDebugMsg("parsed Model Body  \n  %T\n", ((*yyvalp).appl));	
		;}
    break;

  case 5:
#line 136 "chiparser.yy"
    { safe_assign(((*yyvalp).appl), gsMakeModelSpec( (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed Model Body  \n  %T\n", ((*yyvalp).appl));	
		;}
    break;

  case 6:
#line 143 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed Process Definition \n  %T\n", ((*yyvalp).list));	
		;}
    break;

  case 7:
#line 147 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed Process Definition \n  %T\n", ((*yyvalp).list));	
		;}
    break;

  case 8:
#line 154 "chiparser.yy"
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

  case 9:
#line 171 "chiparser.yy"
    { 
      	  safe_assign(((*yyvalp).appl), gsMakeProcDef((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (7))].yystate.yysemantics.yysval.appl), gsMakeProcDecl( ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (7))].yystate.yysemantics.yysval.list))), (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (7))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed proc Def\n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 10:
#line 187 "chiparser.yy"
    {
		  scope_lvl++;
		  gsDebugMsg("Increase Scope to: %d\n",scope_lvl);
		;}
    break;

  case 11:
#line 195 "chiparser.yy"
    { safe_assign(((*yyvalp).appl), gsMakeProcSpec( ATmakeList0(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl) ));
      	  gsDebugMsg("parsed ProcessBody  \n  %T\n", ((*yyvalp).appl));	
		;}
    break;

  case 12:
#line 199 "chiparser.yy"
    { safe_assign(((*yyvalp).appl), gsMakeProcSpec( ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.list)), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed ProcessBody  \n  %T\n", ((*yyvalp).appl));	
		;}
    break;

  case 13:
#line 206 "chiparser.yy"
    {
		  assert(scope_lvl > 0);
		  scope_lvl--;
		  gsDebugMsg("Decrease Scope to; %d\n",scope_lvl);
          var_type_map.clear();
		;}
    break;

  case 14:
#line 215 "chiparser.yy"
    { 
 	  	  safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl) );
      	  gsDebugMsg("parsed id's\n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 15:
#line 222 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("LocalVariables_csp: parsed \n  %T\n", ((*yyvalp).list));	
		;}
    break;

  case 16:
#line 226 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("LocalVariables_csp: parsed \n  %T\n", ((*yyvalp).list));	
		;}
    break;

  case 17:
#line 233 "chiparser.yy"
    {
		  safe_assign(((*yyvalp).appl), gsMakeVarSpec( (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.list) ) );
		  gsDebugMsg("LocalVariables: parsed \n %T\n", ((*yyvalp).appl));
		;}
    break;

  case 18:
#line 238 "chiparser.yy"
    {
		  //safe_assign($$, gsMakeVarSpec( ATreverse( $2 ) ) );  //<-- gsMakeVarSpec aanpassen
		  gsDebugMsg("LocalVariables: parsed \n %T\n", ((*yyvalp).appl));
		;}
    break;

  case 19:
#line 247 "chiparser.yy"
    { safe_assign(((*yyvalp).list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list));
		  gsDebugMsg("IdentifierTypeExpression_csp: parsed \n %T\n", ((*yyvalp).list));
		;}
    break;

  case 20:
#line 251 "chiparser.yy"
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

  case 21:
#line 268 "chiparser.yy"
    {
		  safe_assign(((*yyvalp).list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list) );
		  gsDebugMsg("IdentifierTypeExpression: parsed \n %T\n", ((*yyvalp).list));
		;}
    break;

  case 22:
#line 273 "chiparser.yy"
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

  case 23:
#line 293 "chiparser.yy"
    { safe_assign(((*yyvalp).list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list));
		  gsDebugMsg("IdentifierType_csp: parsed \n %T\n", ((*yyvalp).list));
		;}
    break;

  case 24:
#line 297 "chiparser.yy"
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

  case 25:
#line 314 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
		  gsDebugMsg("FormalParameter_csp: parsed \n %T\n", ((*yyvalp).list));
		;}
    break;

  case 26:
#line 318 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
		  gsDebugMsg("FormalParameter_csp: parsed \n %T\n", ((*yyvalp).list));
		;}
    break;

  case 27:
#line 325 "chiparser.yy"
    {
		  safe_assign(((*yyvalp).appl), gsMakeVarDecl( ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.list)) ) );
		  gsDebugMsg("FormalParameter: parsed \n %T\n", ((*yyvalp).appl));
		;}
    break;

  case 28:
#line 330 "chiparser.yy"
    {
		  safe_assign(((*yyvalp).appl), gsMakeChanDecl( ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.list)) ) );
		  gsDebugMsg("FormalParameter: parsed \n %T\n", ((*yyvalp).appl));
		;}
    break;

  case 29:
#line 338 "chiparser.yy"
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

  case 30:
#line 387 "chiparser.yy"
    { safe_assign(((*yyvalp).list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.list) );
      	  gsDebugMsg("ChannelDeclaration_csp: parsed formalparameter channel  \n  %T\n", ((*yyvalp).list));	
		;}
    break;

  case 31:
#line 391 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.list)));
      	  gsDebugMsg("ChannelDeclaration_csp: parsed formalparameter channel \n  %T\n", ((*yyvalp).list));	
		;}
    break;

  case 32:
#line 398 "chiparser.yy"
    {
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
             new_list = ATinsert(new_list,(ATerm) gsMakeChannelTypedID( (ATermAppl) ATgetFirst(list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)) );
			 list = ATgetNext( list ) ;
		  }

		  safe_assign(((*yyvalp).list), new_list );
		  gsDebugMsg("ChannelDefinition: parsed VariableList \n %T\n", ((*yyvalp).list));
		;}
    break;

  case 33:
#line 438 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("IdentifierChannelDeclaration_csp: parsed formalparameter channel  \n  %T\n", ((*yyvalp).list));	
		;}
    break;

  case 34:
#line 442 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("IdentifierChannelDeclaration_csp: parsed formalparameter channel \n  %T\n", ((*yyvalp).list));	
		;}
    break;

  case 35:
#line 449 "chiparser.yy"
    {
          safe_assign(((*yyvalp).appl), gsMakeChannelID((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl), gsMakeRecv()));
		  gsDebugMsg("IdentifierChannelDeclaration: parsed Identifier Type With Expression \n %T\n", ((*yyvalp).appl));
        ;}
    break;

  case 36:
#line 454 "chiparser.yy"
    {
          safe_assign(((*yyvalp).appl), gsMakeChannelID((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl), gsMakeSend()));
		  gsDebugMsg("IdentifierChannelDeclaration: parsed Identifier Type With Expression \n %T\n", ((*yyvalp).appl));
        ;}
    break;

  case 37:
#line 472 "chiparser.yy"
    { //safe_assign($$, ATmakeList1((ATerm) $1));
      	  gsDebugMsg("ChannelDefinition_csp: parsed \n  %T\n", ((*yyvalp).list));	
		;}
    break;

  case 38:
#line 476 "chiparser.yy"
    { //safe_assign($$, ATinsert($1, (ATerm) $3));
      	  gsDebugMsg("ChannelDefinition_csp: parsed \n  %T\n", ((*yyvalp).list));	
		;}
    break;

  case 39:
#line 483 "chiparser.yy"
    {
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
             gsDebugMsg("%T",ATgetFirst(list));
             new_list = ATinsert(new_list,(ATerm) gsMakeChannelTypedID( (ATermAppl) ATgetFirst(list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)) );
			 list = ATgetNext( list ) ;
		  }

		  safe_assign(((*yyvalp).list), new_list );
		  gsDebugMsg("ChannelDefinition: parsed VariableList \n %T\n", ((*yyvalp).list));
		;}
    break;

  case 40:
#line 524 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("IdentifierChannelDefinition_csp: parsed \n  %T\n", ((*yyvalp).list));	
		;}
    break;

  case 41:
#line 528 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("IdentifierChannelDefinition_csp: parsed \n  %T\n", ((*yyvalp).list));	
		;}
    break;

  case 42:
#line 535 "chiparser.yy"
    {
          safe_assign(((*yyvalp).appl), gsMakeChannelID((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl), gsMakeNil()));
		  gsDebugMsg("IdentifierChannelDefinition: parsed \n %T\n", ((*yyvalp).appl));
        ;}
    break;

  case 44:
#line 562 "chiparser.yy"
    { 
          safe_assign(((*yyvalp).appl), gsMakeType( gsString2ATermAppl( "Bool" ) ) );
      	  gsDebugMsg("BasicType: parsed Type \n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 45:
#line 567 "chiparser.yy"
    { 
          safe_assign(((*yyvalp).appl), gsMakeType( gsString2ATermAppl("Nat" ) ) );
      	  gsDebugMsg("BasicType: parsed Type \n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 46:
#line 572 "chiparser.yy"
    { 
          safe_assign(((*yyvalp).appl), gsMakeType( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl) ) );
      	  gsDebugMsg("BasicType: parsed Type \n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 47:
#line 587 "chiparser.yy"
    { safe_assign(((*yyvalp).appl), gsMakeParenthesisedStat( (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("Statement: parsed \n  %T\n", ((*yyvalp).appl));	
		;}
    break;

  case 52:
#line 598 "chiparser.yy"
    { safe_assign(((*yyvalp).appl), gsMakeParStat( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("ModelStatement: parsed \n  %T\n", ((*yyvalp).appl));	
		;}
    break;

  case 56:
#line 615 "chiparser.yy"
    {
          ATermList ids = (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.list);
          ATermList exprs = (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.list);

          while(!ATisEmpty(ids))
          { 
            if (ATgetArgument(ATgetFirst(ids), 1) != ATgetArgument(ATgetFirst(exprs), 1))
		    { 
              gsErrorMsg("Incompatible Types Checking failed\n");
		      exit(1);
            }
            ids = ATgetNext(ids);
            exprs = ATgetNext(exprs);
          }  
 
          safe_assign(((*yyvalp).appl), gsMakeAssignmentStat((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.appl), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.list)), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.list)) ) );
      	  gsDebugMsg("AssignmentStatement: parsed \n  %T\n", ((*yyvalp).appl));	
		;}
    break;

  case 57:
#line 635 "chiparser.yy"
    { safe_assign(((*yyvalp).appl), gsMakeSkipStat( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl), gsMakeSkip() ));
      	  gsDebugMsg("AssignmentStatement: parsed \n  %T\n", ((*yyvalp).appl));	
		;}
    break;

  case 58:
#line 642 "chiparser.yy"
    { safe_assign(((*yyvalp).appl), gsMakeNil() );
		;}
    break;

  case 59:
#line 645 "chiparser.yy"
    { 
			/**
			  * Type Checking
			  *
			  **/
			if(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl),1) != gsMakeType(gsString2ATermAppl("Bool")))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};


			safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl) );
      	  	gsDebugMsg("OptGuard: parsed \n  %T\n", ((*yyvalp).appl));	
		;}
    break;

  case 60:
#line 663 "chiparser.yy"
    { safe_assign(((*yyvalp).appl), gsMakeNil() );
		;}
    break;

  case 61:
#line 666 "chiparser.yy"
    { safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl));
		  gsErrorMsg("OptChannel not yet implemented");
		  assert(false);
      	  gsDebugMsg("OptChannel: parsed \n  %T\n", ((*yyvalp).appl));	
		;}
    break;

  case 62:
#line 675 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATmakeList1( (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("Identifier_csp: parsed \n  %T\n", ((*yyvalp).list));	
		;}
    break;

  case 63:
#line 679 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("Identifier_csp: parsed \n  %T\n", ((*yyvalp).list));	
		;}
    break;

  case 64:
#line 686 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("Expression_csp: parsed \n  %T\n", ((*yyvalp).list));	
		;}
    break;

  case 65:
#line 690 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("Expression_csp: parsed \n  %T\n", ((*yyvalp).list));	
		;}
    break;

  case 66:
#line 697 "chiparser.yy"
    {
          //Check if $2 is properly typed
          //Check if $4 is properly typed
          //Check if $2 can send

          ATermAppl channel = (ATermAppl) ATgetArgument(ATgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.appl),0),0);   

          safe_assign(((*yyvalp).appl), gsMakeSendStat((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.appl),  channel, ATreverse( (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.list)) ) );
      	  gsDebugMsg("parsed expression-element \n  %T\n", ((*yyvalp).appl));	
        ;}
    break;

  case 67:
#line 712 "chiparser.yy"
    {
          //Check if $2 is properly typed
          //Check if $4 is properly typed
          //Check if $2 can receive

          gsDebugMsg("%T",(((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.appl));
 
          ATermAppl channel = (ATermAppl) ATgetArgument(ATgetArgument( (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.appl),0),0);   

          safe_assign(((*yyvalp).appl), gsMakeRecvStat((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.appl), channel, ATreverse( (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.list)) ) );
      	  gsDebugMsg("parsed expression-element \n  %T\n", ((*yyvalp).appl));	
        ;}
    break;

  case 68:
#line 732 "chiparser.yy"
    {
        safe_assign(((*yyvalp).appl), gsMakeInstantiation((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), ATmakeList0()));
      ;}
    break;

  case 69:
#line 736 "chiparser.yy"
    {
        safe_assign(((*yyvalp).appl), gsMakeInstantiation((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.appl), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.list))));
      ;}
    break;

  case 70:
#line 743 "chiparser.yy"
    { safe_assign(((*yyvalp).appl), gsMakeSepStat( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed SEP statement \n  %T\n", ((*yyvalp).appl));	
		;}
    break;

  case 71:
#line 747 "chiparser.yy"
    { safe_assign(((*yyvalp).appl), gsMakeAltStat( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed ALT statement \n  %T\n", ((*yyvalp).appl));	
		;}
    break;

  case 72:
#line 758 "chiparser.yy"
    { safe_assign(((*yyvalp).appl), gsMakeStarStat( (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed STAR statement \n  %T\n", ((*yyvalp).appl));	
		;}
    break;

  case 73:
#line 762 "chiparser.yy"
    {
			/**
			  * Type Checking
			  *
			  **/	
			if(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1) != gsMakeType(gsString2ATermAppl("Bool")))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};
          safe_assign(((*yyvalp).appl), gsMakeGuardedStarStat( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed GuardedSTAR statement \n  %T\n", ((*yyvalp).appl));	
		;}
    break;

  case 74:
#line 779 "chiparser.yy"
    { safe_assign(((*yyvalp).appl), gsMakeDeltaStat((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl), gsMakeDelta()));
      	  gsDebugMsg("parsed deadlock statement \n  %T\n", ((*yyvalp).appl));	
		;}
    break;

  case 75:
#line 792 "chiparser.yy"
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

  case 80:
#line 823 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed expression-element \n  %T\n", ((*yyvalp).list));	
		;}
    break;

  case 81:
#line 827 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed expression-element\n  %T\n", ((*yyvalp).list));	
		;}
    break;

  case 82:
#line 833 "chiparser.yy"
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

  case 83:
#line 860 "chiparser.yy"
    {
		  /**  
		    * Lookup Identifier Type
		    *
		    * TODO: Add scope
		    *
		    **/

          bool channel_exists = false;
          bool variable_exists = false;
          if (chan_type_direction_map.end() == chan_type_direction_map.find( (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)))
            {
                
//              gsErrorMsg("BasicExpression: Channel %T is not defined!\n", $1 );
//              exit(1);
            } else {
              channel_exists = true;
              safe_assign(((*yyvalp).appl), 
                gsMakeChannelTypedID(
                  gsMakeChannelID((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl), gsMakeNil()),
                  (ATermAppl) chan_type_direction_map[(ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)].first
                )
              );
            }
          /* safe_assign($$, 
            gsMakeExpression( $1, 
              (ATermAppl) chan_type_direction_map[(ATerm) $1] 
            )
          ); */
          // Determine if the expression is defined already 
          if (var_type_map.end() == var_type_map.find( (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)))
            {
            } else {
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

  case 84:
#line 919 "chiparser.yy"
    { 
 	  		safe_assign(((*yyvalp).appl), 
				gsMakeExpression( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl), 
					gsMakeType( gsString2ATermAppl("Bool" )) 
				)
			);
      		gsDebugMsg("BooleanExpression: parsed \n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 85:
#line 928 "chiparser.yy"
    { 
 	  		safe_assign(((*yyvalp).appl), 
				gsMakeExpression( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl), 
					gsMakeType( gsString2ATermAppl("Bool" )) 
				)
			);
      		gsDebugMsg("BooleanExpression: parsed \n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 86:
#line 937 "chiparser.yy"
    { 
			/**
			  * Type Checking
			  *
			  **/	
			if(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl),1) != gsMakeType(gsString2ATermAppl("Bool")))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};

 	  		safe_assign(((*yyvalp).appl), 
				gsMakeUnaryExpression( gsString2ATermAppl("!" ),
				gsMakeType( gsString2ATermAppl("Bool" ) ), 
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl) ) 
			);
      		gsDebugMsg("parsed Negation Expression's\n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 87:
#line 956 "chiparser.yy"
    { 
			/**
			  * Type Checking
			  *
			  **/	
			if(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl),1) != gsMakeType(gsString2ATermAppl("Bool")))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};

 	  		safe_assign(((*yyvalp).appl), 
				gsMakeUnaryExpression( gsString2ATermAppl("!" ),
				gsMakeType( gsString2ATermAppl("Bool" ) ), 
				(((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl) ) 
			);
      		gsDebugMsg("parsed Negation Expression's\n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 88:
#line 975 "chiparser.yy"
    { 
			/**
			  * Type Checking
			  *
			  **/	
			if(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1) != ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};
			if(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1) != gsMakeType(gsString2ATermAppl("Bool")))

				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};

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

  case 89:
#line 1002 "chiparser.yy"
    { 
			/**
			  * Type Checking
			  *
			  **/	
			if(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1) != ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};
			if(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1) != gsMakeType(gsString2ATermAppl("Bool")))

				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};

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

  case 90:
#line 1029 "chiparser.yy"
    { 
			/**
			  * Type Checking
			  *
			  **/	
			if(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1) != ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};
			if(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1) != gsMakeType(gsString2ATermAppl("Bool")))

				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};

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

  case 91:
#line 1059 "chiparser.yy"
    { 
 	  		safe_assign(((*yyvalp).appl), 
				gsMakeExpression( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl), 
					gsMakeType( gsString2ATermAppl("Nat" )) 
				)
			);
      		gsDebugMsg("parsed Expression's\n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 92:
#line 1068 "chiparser.yy"
    { 
			/**
			  * Type Checking
			  *
			  **/	
			if(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl),1) == gsString2ATermAppl("Nat"))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};

 	  		safe_assign(((*yyvalp).appl), 
				gsMakeUnaryExpression( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl), 
					gsMakeType( gsString2ATermAppl("Nat" ) ),
					(((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl) 
				)
			);
      		gsDebugMsg("parsed UnaryExpression's\n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 93:
#line 1088 "chiparser.yy"
    { 
			/**
			  * Type Checking
			  *
			  **/	
			if(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl),1) == gsMakeType(gsString2ATermAppl("Nat")))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};

 	  		safe_assign(((*yyvalp).appl), 
				gsMakeUnaryExpression((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl), 
					gsMakeType( gsString2ATermAppl("Nat" ) ),
					(((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl)
				)
			);
      		gsDebugMsg("parsed UnaryExpression's\n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 94:
#line 1108 "chiparser.yy"
    { 
			/**
			  * Type Checking
			  *
			  **/	
			if(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1) != ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};
			if(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1) != gsMakeType(gsString2ATermAppl("Nat")))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};

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

  case 95:
#line 1134 "chiparser.yy"
    { 
			/**
			  * Type Checking
			  *
			  **/	
			if(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1) != ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};
			if(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1) != gsMakeType(gsString2ATermAppl("Nat")))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};

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

  case 96:
#line 1160 "chiparser.yy"
    { 
			/**
			  * Type Checking
			  *
			  **/	
			if(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1) != ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};
			if(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1) != gsMakeType(gsString2ATermAppl("Nat")))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};

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

  case 97:
#line 1186 "chiparser.yy"
    { 
			/**
			  * Type Checking
			  *
			  **/	
			if(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1) != ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};
			if(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1) != gsMakeType(gsString2ATermAppl("Nat")))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};

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

  case 98:
#line 1212 "chiparser.yy"
    { 
			/**
			  * Type Checking
			  *
			  **/	
			if(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1) != ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};
			if(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1) != gsMakeType(gsString2ATermAppl("Nat")))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};

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

  case 99:
#line 1238 "chiparser.yy"
    { 
			/**
			  * Type Checking
			  *
			  **/	
			if(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1) != ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};
			if(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1) != gsMakeType(gsString2ATermAppl("Nat")))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};

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

  case 100:
#line 1264 "chiparser.yy"
    { 
			/**
			  * Type Checking
			  *
			  **/	
			if(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1) != ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};
			if(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1) != gsMakeType(gsString2ATermAppl("Nat")))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};

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

  case 101:
#line 1290 "chiparser.yy"
    { 
			/**
			  * Type Checking
			  *
			  **/	
			if(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1) != ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};
			if(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1) != gsMakeType(gsString2ATermAppl("Nat")))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};

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

  case 102:
#line 1316 "chiparser.yy"
    { 
			/**
			  * Type Checking
			  *
			  **/	
			if(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1) != ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};
			if(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1) != gsMakeType(gsString2ATermAppl("Nat")))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};

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

  case 103:
#line 1342 "chiparser.yy"
    { 
			/**
			  * Type Checking
			  *
			  **/	
			if(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1) != ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};
			if(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1) != gsMakeType(gsString2ATermAppl("Nat")))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};

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

  case 104:
#line 1368 "chiparser.yy"
    { 
			/**
			  * Type Checking
			  *
			  **/	
			if(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1) != ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};
			if(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1) != gsMakeType(gsString2ATermAppl("Nat")))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};

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

  case 105:
#line 1394 "chiparser.yy"
    { 
			/**
			  * Type Checking
			  *
			  **/	
			if(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1) != ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};
			if(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1) != gsMakeType(gsString2ATermAppl("Nat")))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};

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

  case 106:
#line 1420 "chiparser.yy"
    { 
			/**
			  * Type Checking
			  *
			  **/	
			if(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1) != ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};
			if(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1) != gsMakeType(gsString2ATermAppl("Nat")))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};

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

  case 107:
#line 1449 "chiparser.yy"
    {
			/**
			  * Type Checking
			  *
			  **/	
			if(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1) != ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};
		
			safe_assign(((*yyvalp).appl), gsMakeBinaryExpression( (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl),  
					gsMakeType( gsString2ATermAppl("Bool" )), 
			(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      		gsDebugMsg("parsed UnaryExpression's\n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 108:
#line 1466 "chiparser.yy"
    { 
			/**
			  * Type Checking
			  *
			  **/	
			if(ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl),1) != ATAgetArgument((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl),1))
				{
				  gsErrorMsg("Incompatible Types Checking failed\n");
				  exit(1);
				};

 	  		safe_assign(((*yyvalp).appl), gsMakeBinaryExpression( (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl),  
					gsMakeType( gsString2ATermAppl("Bool" )), 
			(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      		gsDebugMsg("parsed UnaryExpression's\n  %T\n", ((*yyvalp).appl));
		;}
    break;


/* Line 930 of glr.c.  */
#line 2872 "chiparser.cpp"
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


#line 1510 "chiparser.yy"



