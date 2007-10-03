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
#define YYFINAL  5
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1159

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  67
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  41
/* YYNRULES -- Number of rules.  */
#define YYNRULES  92
/* YYNRULES -- Number of states.  */
#define YYNSTATES  169
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
       0,     0,     3,     6,    13,    17,    23,    25,    27,    29,
      32,    39,    47,    49,    53,    59,    61,    63,    65,    69,
      72,    74,    78,    80,    84,    86,    90,    92,    96,    99,
     103,   105,   107,   109,   111,   115,   117,   119,   121,   123,
     127,   129,   131,   137,   141,   142,   145,   146,   149,   151,
     155,   157,   161,   165,   170,   174,   178,   181,   185,   188,
     192,   194,   196,   198,   200,   202,   206,   208,   210,   212,
     214,   217,   220,   224,   228,   232,   234,   237,   240,   244,
     248,   252,   256,   260,   264,   268,   272,   276,   280,   284,
     288,   292,   296
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const signed char yyrhs[] =
{
      68,     0,    -1,    69,    73,    -1,    71,    78,    31,    32,
      24,    70,    -1,    19,    90,    72,    -1,    19,    79,    21,
      90,    72,    -1,     6,    -1,    20,    -1,    74,    -1,    73,
      74,    -1,    75,    78,    31,    32,    24,    76,    -1,    75,
      78,    31,    84,    32,    24,    76,    -1,     3,    -1,    19,
      89,    77,    -1,    19,    79,    21,    89,    77,    -1,    20,
      -1,    17,    -1,    80,    -1,    79,    23,    80,    -1,     7,
      81,    -1,    82,    -1,    81,    23,    82,    -1,    86,    -1,
      86,    24,   101,    -1,    86,    -1,    83,    23,    86,    -1,
      85,    -1,    84,    23,    85,    -1,     7,    83,    -1,    95,
      13,    87,    -1,    88,    -1,    15,    -1,    16,    -1,    14,
      -1,    31,    89,    32,    -1,    91,    -1,    99,    -1,    98,
      -1,    97,    -1,    90,    11,    90,    -1,    92,    -1,   100,
      -1,    93,    94,   102,    25,    96,    -1,    93,    94,    10,
      -1,    -1,   101,    35,    -1,    -1,   101,    13,    -1,    78,
      -1,    95,    23,    78,    -1,   101,    -1,    96,    23,   101,
      -1,    78,    31,    32,    -1,    78,    31,    96,    32,    -1,
      89,    22,    89,    -1,    89,    12,    89,    -1,    61,    89,
      -1,   101,    62,    89,    -1,    93,    45,    -1,    31,   101,
      32,    -1,   104,    -1,   105,    -1,   106,    -1,   107,    -1,
     103,    -1,   102,    23,   103,    -1,    78,    -1,    78,    -1,
      43,    -1,    42,    -1,    36,   101,    -1,    56,   101,    -1,
     101,    33,   101,    -1,   101,    34,   101,    -1,   101,    46,
     101,    -1,    39,    -1,    27,   101,    -1,    26,   101,    -1,
     101,    54,   101,    -1,   101,    61,   101,    -1,   101,    64,
     101,    -1,   101,    27,   101,    -1,   101,    26,   101,    -1,
     101,    53,   101,    -1,   101,    52,   101,    -1,   101,    51,
     101,    -1,   101,    50,   101,    -1,   101,    65,   101,    -1,
     101,    66,   101,    -1,   101,    49,   101,    -1,   101,    48,
     101,    -1,   101,    37,   101,    -1,   101,    47,   101,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,   108,   108,   117,   125,   129,   135,   141,   147,   151,
     158,   175,   191,   199,   203,   210,   219,   226,   230,   237,
     247,   251,   258,   262,   270,   274,   281,   285,   292,   301,
     393,   401,   406,   411,   426,   430,   431,   432,   436,   437,
     444,   450,   454,   474,   482,   484,   503,   505,   514,   518,
     525,   529,   549,   553,   560,   564,   575,   579,   596,   609,
     624,   625,   626,   627,   640,   644,   650,   677,   709,   718,
     727,   746,   765,   792,   819,   849,   858,   878,   898,   924,
     950,   976,  1002,  1028,  1054,  1080,  1106,  1132,  1158,  1184,
    1210,  1239,  1256
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
  "ModelDefinition", "ModelBody", "ModelOpen", "ModelClose",
  "ProcessDefinitions", "ProcessDefinition", "ProcOpenScope",
  "ProcessBody", "ProcCloseScope", "Identifier", "LocalVariables_csp",
  "LocalVariables", "IdentifierTypeExpression_csp",
  "IdentifierTypeExpression", "IdentifierType_csp", "FormalParameter_csp",
  "FormalParameter", "IdentifierType", "Type", "BasicType", "Statement",
  "ModelStatement", "BasicStatement", "AssignmentStatement", "OptGuard",
  "OptChannel", "Identifier_csp", "Expression_csp", "Instantiation",
  "BinaryStatement", "UnaryStatement", "AdvancedStatement", "Expression",
  "ExpressionIdentier_csp", "ExpressionIdentifier", "BasicExpression",
  "BooleanExpression", "NatIntExpression", "BoolNatIntExpression", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    67,    68,    69,    70,    70,    71,    72,    73,    73,
      74,    74,    75,    76,    76,    77,    78,    79,    79,    80,
      81,    81,    82,    82,    83,    83,    84,    84,    85,    86,
      87,    88,    88,    88,    89,    89,    89,    89,    90,    90,
      91,    91,    92,    92,    93,    93,    94,    94,    95,    95,
      96,    96,    97,    97,    98,    98,    99,    99,   100,   101,
     101,   101,   101,   101,   102,   102,   103,   104,   105,   105,
     105,   105,   105,   105,   105,   106,   106,   106,   106,   106,
     106,   106,   106,   106,   106,   106,   106,   106,   106,   106,
     106,   107,   107
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     2,     6,     3,     5,     1,     1,     1,     2,
       6,     7,     1,     3,     5,     1,     1,     1,     3,     2,
       1,     3,     1,     3,     1,     3,     1,     3,     2,     3,
       1,     1,     1,     1,     3,     1,     1,     1,     1,     3,
       1,     1,     5,     3,     0,     2,     0,     2,     1,     3,
       1,     3,     3,     4,     3,     3,     2,     3,     2,     3,
       1,     1,     1,     1,     1,     3,     1,     1,     1,     1,
       2,     2,     3,     3,     3,     1,     2,     2,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3
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
       0,     0,     0
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
       0,     0,     0
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error.  */
static const unsigned char yydefact[] =
{
       0,     6,     0,     0,     0,     1,    12,     2,     8,     0,
      16,     0,     9,     0,     0,     0,     0,     0,     0,     0,
      26,     0,    48,    28,    24,     0,     0,     0,     0,     0,
       3,     0,     0,     0,    44,    10,    27,     0,     0,     0,
       0,    17,     0,    38,    25,    33,    31,    32,    29,    30,
      49,     0,     0,    44,     0,    75,    69,    68,     0,    44,
      67,     0,     0,    35,    40,    46,    37,    36,    41,     0,
      60,    61,    62,    63,    11,    19,    20,    22,     0,     0,
       0,     0,     7,     4,     0,    77,    76,     0,     0,    70,
      71,    56,    44,    44,    15,    44,    13,    58,     0,     0,
       0,     0,     0,     0,    45,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    44,     0,     0,     0,
       0,     0,    52,     0,    50,     0,    18,    39,     0,    34,
      59,     0,    55,    54,    43,    66,     0,    64,    47,    82,
      81,    72,    73,    91,    74,    92,    90,    89,    86,    85,
      84,    83,    78,    79,    57,    80,    87,    88,    21,    23,
       0,    53,     5,    14,     0,     0,    51,    65,    42
};

/* YYPDEFGOTO[NTERM-NUM].  */
static const short int yydefgoto[] =
{
      -1,     2,     3,    30,     4,    83,     7,     8,     9,    35,
      96,    60,    40,    41,    75,    76,    23,    19,    20,    77,
      48,    49,    62,    42,    63,    64,    65,    98,    25,   123,
      43,    66,    67,    68,    69,   136,   137,    70,    71,    72,
      73
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -44
static const short int yypact[] =
{
       1,   -44,    38,    61,   -15,   -44,   -44,    61,   -44,   -15,
     -44,    48,   -44,    52,    53,    -6,    62,   -15,    63,   -17,
     -44,    69,   -44,    66,   -44,    -5,    91,   105,    89,     2,
     -44,   -15,    56,   -15,    -3,   -44,   -44,    91,   -15,    83,
       9,   -44,    46,   -44,   -44,   -44,   -44,   -44,   -44,   -44,
     -44,   950,   950,    20,   950,   -44,   -44,   -44,   950,   135,
     -44,    57,     0,   -44,   -44,   927,   -44,   -44,   -44,   269,
     -44,   -44,   -44,   -44,   -44,    92,   -44,    94,   948,   -15,
     110,   -15,   -44,   -44,   950,   961,   983,    -1,   228,   346,
     382,    23,   156,   158,   -44,   189,   -44,   -44,    44,    93,
     950,   950,   950,   950,   -44,   950,   950,   950,   950,   950,
     950,   950,   950,   950,   950,   950,   210,   950,   950,   950,
     -15,   950,   -44,    37,   418,    46,   -44,   111,   310,   -44,
     -44,     0,    23,    23,   -44,   -44,    42,   -44,   -44,  1005,
    1027,   454,   490,   526,   562,   598,   634,   670,   706,   742,
     778,   814,  1049,  1071,    23,  1093,   850,   886,   -44,   418,
     950,   -44,   -44,   -44,   -15,   950,   418,   -44,    98
};

/* YYPGOTO[NTERM-NUM].  */
static const signed char yypgoto[] =
{
     -44,   -44,   -44,   -44,   -44,    -2,   -44,   117,   -44,    88,
       4,    -4,    95,    51,   -44,     8,   -44,   -44,   106,   -14,
     -44,   -44,   -43,     3,   -44,   -44,   -44,   -44,   -44,   -33,
     -44,   -44,   -44,   -44,   -10,   -44,   -30,   -44,   -44,   -44,
     -44
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const unsigned char yytable[] =
{
      11,    17,    10,    24,    38,    13,    27,     1,    32,    38,
      87,    93,    93,    22,    10,    28,    91,    44,    33,    10,
      94,    95,    95,    51,    52,    39,    18,    22,    53,    50,
      79,   129,    80,    54,    22,    93,    55,    10,     5,    56,
      57,    85,    86,    88,    89,    95,    51,    52,    90,   131,
     132,    53,   133,    58,   134,    99,    54,    81,    59,    55,
     160,    10,    56,    57,     6,   164,    82,   165,   124,   161,
      45,    46,    47,   154,   128,    39,    58,    39,    92,    14,
      80,    59,   125,    15,   127,    16,    21,    26,    29,    31,
     139,   140,   141,   142,   135,   143,   144,   145,   146,   147,
     148,   149,   150,   151,   152,   153,   138,   155,   156,   157,
      34,   159,    17,    37,    78,   120,    22,    38,   121,   100,
     101,   160,    81,   162,    12,    74,   102,   103,   158,    61,
     105,   126,   168,    36,   167,   163,     0,     0,     0,   106,
     107,   108,   109,   110,   111,   112,   113,   114,     0,     0,
     166,     0,    10,     0,   115,   124,     0,   117,   118,   119,
     135,    51,    52,     0,     0,     0,    53,     0,     0,     0,
       0,    54,     0,    10,    55,    10,     0,    56,    57,     0,
       0,     0,    51,    52,    51,    52,     0,    53,     0,    53,
       0,    58,    54,     0,    54,    55,    59,    55,    56,    57,
      56,    57,     0,     0,     0,     0,    10,     0,     0,     0,
       0,     0,    58,     0,    58,    51,    52,    59,     0,    59,
      53,     0,     0,     0,     0,    54,     0,    10,    55,     0,
       0,    56,    57,     0,     0,     0,    51,    52,     0,     0,
       0,    53,     0,     0,     0,    58,    54,     0,     0,    55,
      59,     0,    56,    57,   100,   101,     0,     0,     0,     0,
     130,   102,   103,   104,     0,   105,    58,     0,     0,     0,
       0,    59,     0,     0,   106,   107,   108,   109,   110,   111,
     112,   113,   114,     0,     0,     0,     0,     0,     0,   115,
     116,     0,   117,   118,   119,   100,   101,     0,     0,     0,
       0,     0,   102,   103,   104,     0,   105,     0,     0,     0,
       0,     0,     0,     0,     0,   106,   107,   108,   109,   110,
     111,   112,   113,   114,     0,     0,     0,     0,     0,     0,
     115,   116,     0,   117,   118,   119,   100,   101,     0,     0,
       0,     0,   130,   102,   103,     0,     0,   105,     0,     0,
       0,     0,     0,     0,     0,     0,   106,   107,   108,   109,
     110,   111,   112,   113,   114,     0,     0,     0,     0,     0,
       0,   115,   100,   101,   117,   118,   119,     0,     0,   102,
     103,     0,     0,   105,     0,     0,     0,     0,     0,     0,
       0,     0,   106,   107,   108,   109,   110,   111,   112,   113,
     114,     0,     0,     0,     0,     0,     0,   115,   100,   101,
     117,   118,   119,     0,     0,   102,   103,     0,     0,   105,
       0,     0,     0,     0,     0,     0,     0,     0,   106,   107,
     108,   109,   110,   111,   112,   113,   114,     0,     0,     0,
       0,     0,     0,   115,   100,   101,   117,   118,   119,     0,
       0,   102,   103,     0,     0,   105,     0,     0,     0,     0,
       0,     0,     0,     0,   106,   107,   108,   109,   110,   111,
     112,   113,   114,     0,     0,     0,     0,     0,     0,   115,
     100,   101,   117,   118,   119,     0,     0,   102,   103,     0,
       0,   105,     0,     0,     0,     0,     0,     0,     0,     0,
     106,   107,   108,   109,   110,   111,   112,   113,   114,     0,
       0,     0,     0,     0,     0,   115,   100,   101,   117,   118,
     119,     0,     0,   102,   103,     0,     0,   105,     0,     0,
       0,     0,     0,     0,     0,     0,   106,   107,   108,   109,
     110,   111,   112,   113,   114,     0,     0,     0,     0,     0,
       0,   115,   100,   101,   117,   118,   119,     0,     0,   102,
     103,     0,     0,   105,     0,     0,     0,     0,     0,     0,
       0,     0,   106,   107,   108,   109,   110,   111,   112,   113,
     114,     0,     0,     0,     0,     0,     0,   115,   100,   101,
     117,   118,   119,     0,     0,   102,   103,     0,     0,   105,
       0,     0,     0,     0,     0,     0,     0,     0,   106,   107,
     108,   109,   110,   111,   112,   113,   114,     0,     0,     0,
       0,     0,     0,   115,   100,   101,   117,   118,   119,     0,
       0,   102,   103,     0,     0,   105,     0,     0,     0,     0,
       0,     0,     0,     0,   106,   107,   108,   109,   110,   111,
     112,   113,   114,     0,     0,     0,     0,     0,     0,   115,
     100,   101,   117,   118,   119,     0,     0,   102,   103,     0,
       0,   105,     0,     0,     0,     0,     0,     0,     0,     0,
     106,   107,   108,   109,   110,   111,   112,   113,   114,     0,
       0,     0,     0,     0,     0,   115,   100,   101,   117,   118,
     119,     0,     0,   102,   103,     0,     0,   105,     0,     0,
       0,     0,     0,     0,     0,     0,   106,   107,   108,   109,
     110,   111,   112,   113,   114,     0,     0,     0,     0,     0,
       0,   115,   100,   101,   117,   118,   119,     0,     0,   102,
     103,     0,     0,   105,     0,     0,     0,     0,     0,     0,
       0,     0,   106,   107,   108,   109,   110,   111,   112,   113,
     114,     0,     0,     0,     0,     0,     0,   115,   100,   101,
     117,   118,   119,     0,     0,   102,   103,     0,     0,   105,
       0,     0,     0,     0,     0,     0,     0,     0,   106,   107,
     108,   109,   110,   111,   112,   113,   114,     0,     0,     0,
       0,     0,     0,   115,   100,   101,   117,   118,   119,     0,
       0,   102,   103,     0,     0,   105,     0,     0,     0,     0,
       0,     0,     0,     0,   106,   107,   108,   109,   110,   111,
     112,   113,   114,     0,     0,     0,     0,     0,     0,   115,
     100,   101,   117,   118,   119,     0,     0,   102,   103,     0,
       0,   105,     0,     0,     0,     0,     0,     0,     0,     0,
     106,   107,   108,   109,   110,   111,   112,   113,   114,     0,
       0,     0,     0,     0,     0,   115,   100,   101,   117,   118,
     119,     0,     0,   102,   103,     0,     0,   105,     0,     0,
       0,     0,     0,     0,     0,     0,   106,   107,   108,   109,
     110,   111,   112,   113,   114,     0,     0,     0,     0,     0,
       0,   115,   100,   101,   117,   118,   119,     0,     0,   102,
     103,     0,     0,   105,     0,     0,     0,     0,     0,     0,
       0,     0,   106,   107,   108,   109,   110,   111,   112,   113,
     114,     0,     0,     0,    10,     0,     0,   115,     0,     0,
     117,   118,   119,    51,    52,     0,     0,     0,    84,     0,
       0,     0,     0,    54,     0,    10,    55,    10,     0,    56,
      57,     0,    97,     0,    51,    52,    51,    52,     0,    84,
     122,    84,     0,    58,    54,     0,    54,    55,     0,    55,
      56,    57,    56,    57,   102,   103,     0,     0,   105,     0,
       0,     0,     0,     0,    58,     0,    58,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   102,   103,     0,     0,
     105,     0,   115,     0,     0,   117,   118,   119,     0,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   102,   103,
       0,     0,   105,     0,   115,     0,     0,   117,   118,   119,
       0,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     102,   103,     0,     0,   105,     0,   115,     0,     0,   117,
     118,   119,     0,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   102,   103,     0,     0,   105,     0,   115,     0,
       0,   117,   118,   119,     0,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   102,   103,     0,     0,   105,     0,
     115,     0,     0,     0,   118,   119,     0,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   102,   103,     0,     0,
     105,     0,   115,     0,     0,     0,   118,   119,     0,   106,
     107,   108,   109,   110,   111,   112,   113,   114,     0,     0,
       0,     0,     0,     0,   115,     0,     0,     0,   118,   119
};

/* YYCONFLP[YYPACT[STATE-NUM]] -- Pointer into YYCONFL of start of
   list of conflicting reductions corresponding to action entry for
   state STATE-NUM in yytable.  0 means no conflicts.  The list in
   yyconfl is terminated by a rule number of 0.  */
static const unsigned short int yyconflp[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     3,     0,     0,     0,     0,     0,
       0,     0,     0,     5,     7,     0,     0,     0,     9,     0,
       0,     0,     0,    11,     0,     0,    13,    21,     0,    15,
      17,     0,     0,     0,     0,     0,    23,    25,     0,     0,
       0,    27,     0,    19,     0,     0,    29,     0,     0,    31,
       0,     0,    33,    35,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    37,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     1,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    59,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    39,     0,     0,     0,     0,     0,     0,     0,
       0,    41,    43,     0,     0,     0,    45,     0,     0,     0,
       0,    47,     0,   185,    49,   203,     0,    51,    53,     0,
       0,     0,   187,   189,   205,   207,     0,   191,     0,   209,
       0,    55,   193,     0,   211,   195,     0,   213,   197,   199,
     215,   217,     0,     0,     0,     0,   221,     0,     0,     0,
       0,     0,   201,     0,   219,   223,   225,     0,     0,     0,
     227,     0,     0,     0,     0,   229,     0,   239,   231,     0,
       0,   233,   235,     0,     0,     0,   241,   243,     0,     0,
       0,   245,     0,     0,     0,   237,   247,     0,     0,   249,
       0,     0,   251,   253,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   255,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   113,   115,     0,     0,     0,     0,     0,   117,
     119,     0,     0,   121,     0,     0,     0,     0,     0,     0,
       0,     0,   123,   125,   127,   129,   131,   133,   135,   137,
     139,     0,     0,     0,     0,     0,     0,   141,   149,   151,
     143,   145,   147,     0,     0,   153,   155,     0,     0,   157,
       0,     0,     0,     0,     0,     0,     0,     0,   159,   161,
     163,   165,   167,   169,   171,   173,   175,     0,     0,     0,
       0,     0,     0,   177,     0,     0,   179,   181,   183,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     309,   311,     0,     0,     0,     0,     0,   313,   315,     0,
       0,   317,     0,     0,     0,     0,     0,     0,     0,     0,
     319,   321,   323,   325,   327,   329,   331,   333,   335,     0,
       0,     0,     0,     0,     0,   337,   345,   347,   339,   341,
     343,     0,     0,   349,   351,     0,     0,   353,     0,     0,
       0,     0,     0,     0,     0,     0,   355,   357,   359,   361,
     363,   365,   367,   369,   371,     0,     0,     0,     0,     0,
       0,   373,   381,   383,   375,   377,   379,     0,     0,   385,
     387,     0,     0,   389,     0,     0,     0,     0,     0,     0,
       0,     0,   391,   393,   395,   397,   399,   401,   403,   405,
     407,     0,     0,     0,     0,     0,     0,   409,   417,   419,
     411,   413,   415,     0,     0,   421,   423,     0,     0,   425,
       0,     0,     0,     0,     0,     0,     0,     0,   427,   429,
     431,   433,   435,   437,   439,   441,   443,     0,     0,     0,
       0,     0,     0,   445,   453,   455,   447,   449,   451,     0,
       0,   457,   459,     0,     0,   461,     0,     0,     0,     0,
       0,     0,     0,     0,   463,   465,   467,   469,   471,   473,
     475,   477,   479,     0,     0,     0,     0,     0,     0,   481,
     489,   491,   483,   485,   487,     0,     0,   493,   495,     0,
       0,   497,     0,     0,     0,     0,     0,     0,     0,     0,
     499,   501,   503,   505,   507,   509,   511,   513,   515,     0,
       0,     0,     0,     0,     0,   517,   525,   527,   519,   521,
     523,     0,     0,   529,   531,     0,     0,   533,     0,     0,
       0,     0,     0,     0,     0,     0,   535,   537,   539,   541,
     543,   545,   547,   549,   551,     0,     0,     0,     0,     0,
       0,   553,   561,   563,   555,   557,   559,     0,     0,   565,
     567,     0,     0,   569,     0,     0,     0,     0,     0,     0,
       0,     0,   571,   573,   575,   577,   579,   581,   583,   585,
     587,     0,     0,     0,     0,     0,     0,   589,   597,   599,
     591,   593,   595,     0,     0,   601,   603,     0,     0,   605,
       0,     0,     0,     0,     0,     0,     0,     0,   607,   609,
     611,   613,   615,   617,   619,   621,   623,     0,     0,     0,
       0,     0,     0,   625,   633,   635,   627,   629,   631,     0,
       0,   637,   639,     0,     0,   641,     0,     0,     0,     0,
       0,     0,     0,     0,   643,   645,   647,   649,   651,   653,
     655,   657,   659,     0,     0,     0,     0,     0,     0,   661,
     669,   671,   663,   665,   667,     0,     0,   673,   675,     0,
       0,   677,     0,     0,     0,     0,     0,     0,     0,     0,
     679,   681,   683,   685,   687,   689,   691,   693,   695,     0,
       0,     0,     0,     0,     0,   697,   783,   785,   699,   701,
     703,     0,     0,   787,   789,     0,     0,   791,     0,     0,
       0,     0,     0,     0,     0,     0,   793,   795,   797,   799,
     801,   803,   805,   807,   809,     0,     0,     0,     0,     0,
       0,   811,   819,   821,   813,   815,   817,     0,     0,   823,
     825,     0,     0,   827,     0,     0,     0,     0,     0,     0,
       0,     0,   829,   831,   833,   835,   837,   839,   841,   843,
     845,     0,     0,     0,    57,     0,     0,   847,     0,     0,
     849,   851,   853,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    61,    63,     0,     0,    65,     0,
       0,     0,     0,     0,     0,     0,     0,    67,    69,    71,
      73,    75,    77,    79,    81,     0,    87,    89,     0,     0,
      91,     0,     0,     0,     0,     0,    83,    85,     0,    93,
      95,    97,    99,   101,   103,   105,   107,     0,   257,   259,
       0,     0,   261,     0,     0,     0,     0,     0,   109,   111,
       0,   263,   265,   267,   269,   271,   273,   275,   277,     0,
     283,   285,     0,     0,   287,     0,     0,     0,     0,     0,
     279,   281,     0,   289,   291,   293,   295,   297,   299,   301,
     303,     0,   705,   707,     0,     0,   709,     0,     0,     0,
       0,     0,   305,   307,     0,   711,   713,   715,   717,   719,
     721,   723,   725,     0,   731,   733,     0,     0,   735,     0,
       0,     0,     0,     0,   727,   729,     0,   737,   739,   741,
     743,   745,   747,   749,   751,     0,   757,   759,     0,     0,
     761,     0,     0,     0,     0,     0,   753,   755,     0,   763,
     765,   767,   769,   771,   773,   775,   777,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   779,   781
};

/* YYCONFL[I] -- lists of conflicting rule numbers, each terminated by
   0, pointed into by YYCONFLP.  */
static const short int yyconfl[] =
{
       0,    28,     0,    44,     0,    44,     0,    44,     0,    44,
       0,    44,     0,    44,     0,    44,     0,    44,     0,    44,
       0,    44,     0,    44,     0,    44,     0,    44,     0,    44,
       0,    44,     0,    44,     0,    44,     0,    44,     0,    44,
       0,    44,     0,    44,     0,    44,     0,    44,     0,    44,
       0,    44,     0,    44,     0,    44,     0,    46,     0,    19,
       0,    77,     0,    77,     0,    77,     0,    77,     0,    77,
       0,    77,     0,    77,     0,    77,     0,    77,     0,    77,
       0,    77,     0,    77,     0,    77,     0,    76,     0,    76,
       0,    76,     0,    76,     0,    76,     0,    76,     0,    76,
       0,    76,     0,    76,     0,    76,     0,    76,     0,    76,
       0,    76,     0,    70,     0,    70,     0,    70,     0,    70,
       0,    70,     0,    70,     0,    70,     0,    70,     0,    70,
       0,    70,     0,    70,     0,    70,     0,    70,     0,    70,
       0,    70,     0,    70,     0,    70,     0,    70,     0,    71,
       0,    71,     0,    71,     0,    71,     0,    71,     0,    71,
       0,    71,     0,    71,     0,    71,     0,    71,     0,    71,
       0,    71,     0,    71,     0,    71,     0,    71,     0,    71,
       0,    71,     0,    71,     0,    44,     0,    44,     0,    44,
       0,    44,     0,    44,     0,    44,     0,    44,     0,    44,
       0,    44,     0,    44,     0,    44,     0,    44,     0,    44,
       0,    44,     0,    44,     0,    44,     0,    44,     0,    44,
       0,    44,     0,    44,     0,    44,     0,    44,     0,    44,
       0,    44,     0,    44,     0,    44,     0,    44,     0,    44,
       0,    44,     0,    44,     0,    44,     0,    44,     0,    44,
       0,    44,     0,    44,     0,    44,     0,    82,     0,    82,
       0,    82,     0,    82,     0,    82,     0,    82,     0,    82,
       0,    82,     0,    82,     0,    82,     0,    82,     0,    82,
       0,    82,     0,    81,     0,    81,     0,    81,     0,    81,
       0,    81,     0,    81,     0,    81,     0,    81,     0,    81,
       0,    81,     0,    81,     0,    81,     0,    81,     0,    72,
       0,    72,     0,    72,     0,    72,     0,    72,     0,    72,
       0,    72,     0,    72,     0,    72,     0,    72,     0,    72,
       0,    72,     0,    72,     0,    72,     0,    72,     0,    72,
       0,    72,     0,    72,     0,    73,     0,    73,     0,    73,
       0,    73,     0,    73,     0,    73,     0,    73,     0,    73,
       0,    73,     0,    73,     0,    73,     0,    73,     0,    73,
       0,    73,     0,    73,     0,    73,     0,    73,     0,    73,
       0,    91,     0,    91,     0,    91,     0,    91,     0,    91,
       0,    91,     0,    91,     0,    91,     0,    91,     0,    91,
       0,    91,     0,    91,     0,    91,     0,    91,     0,    91,
       0,    91,     0,    91,     0,    91,     0,    74,     0,    74,
       0,    74,     0,    74,     0,    74,     0,    74,     0,    74,
       0,    74,     0,    74,     0,    74,     0,    74,     0,    74,
       0,    74,     0,    74,     0,    74,     0,    74,     0,    74,
       0,    74,     0,    92,     0,    92,     0,    92,     0,    92,
       0,    92,     0,    92,     0,    92,     0,    92,     0,    92,
       0,    92,     0,    92,     0,    92,     0,    92,     0,    92,
       0,    92,     0,    92,     0,    92,     0,    92,     0,    90,
       0,    90,     0,    90,     0,    90,     0,    90,     0,    90,
       0,    90,     0,    90,     0,    90,     0,    90,     0,    90,
       0,    90,     0,    90,     0,    90,     0,    90,     0,    90,
       0,    90,     0,    90,     0,    89,     0,    89,     0,    89,
       0,    89,     0,    89,     0,    89,     0,    89,     0,    89,
       0,    89,     0,    89,     0,    89,     0,    89,     0,    89,
       0,    89,     0,    89,     0,    89,     0,    89,     0,    89,
       0,    86,     0,    86,     0,    86,     0,    86,     0,    86,
       0,    86,     0,    86,     0,    86,     0,    86,     0,    86,
       0,    86,     0,    86,     0,    86,     0,    86,     0,    86,
       0,    86,     0,    86,     0,    86,     0,    85,     0,    85,
       0,    85,     0,    85,     0,    85,     0,    85,     0,    85,
       0,    85,     0,    85,     0,    85,     0,    85,     0,    85,
       0,    85,     0,    85,     0,    85,     0,    85,     0,    85,
       0,    85,     0,    84,     0,    84,     0,    84,     0,    84,
       0,    84,     0,    84,     0,    84,     0,    84,     0,    84,
       0,    84,     0,    84,     0,    84,     0,    84,     0,    84,
       0,    84,     0,    84,     0,    84,     0,    84,     0,    83,
       0,    83,     0,    83,     0,    83,     0,    83,     0,    83,
       0,    83,     0,    83,     0,    83,     0,    83,     0,    83,
       0,    83,     0,    83,     0,    83,     0,    83,     0,    83,
       0,    83,     0,    83,     0,    78,     0,    78,     0,    78,
       0,    78,     0,    78,     0,    78,     0,    78,     0,    78,
       0,    78,     0,    78,     0,    78,     0,    78,     0,    78,
       0,    79,     0,    79,     0,    79,     0,    79,     0,    79,
       0,    79,     0,    79,     0,    79,     0,    79,     0,    79,
       0,    79,     0,    79,     0,    79,     0,    80,     0,    80,
       0,    80,     0,    80,     0,    80,     0,    80,     0,    80,
       0,    80,     0,    80,     0,    80,     0,    80,     0,    80,
       0,    80,     0,    87,     0,    87,     0,    87,     0,    87,
       0,    87,     0,    87,     0,    87,     0,    87,     0,    87,
       0,    87,     0,    87,     0,    87,     0,    87,     0,    87,
       0,    87,     0,    87,     0,    87,     0,    87,     0,    88,
       0,    88,     0,    88,     0,    88,     0,    88,     0,    88,
       0,    88,     0,    88,     0,    88,     0,    88,     0,    88,
       0,    88,     0,    88,     0,    88,     0,    88,     0,    88,
       0,    88,     0,    88,     0
};

static const short int yycheck[] =
{
       4,     7,    17,    17,     7,     9,    23,     6,    13,     7,
      53,    12,    12,    17,    17,    32,    59,    31,    23,    17,
      20,    22,    22,    26,    27,    29,    32,    31,    31,    33,
      21,    32,    23,    36,    38,    12,    39,    17,     0,    42,
      43,    51,    52,    53,    54,    22,    26,    27,    58,    92,
      93,    31,    95,    56,    10,    65,    36,    11,    61,    39,
      23,    17,    42,    43,     3,    23,    20,    25,    78,    32,
      14,    15,    16,   116,    84,    79,    56,    81,    21,    31,
      23,    61,    79,    31,    81,    32,    24,    24,    19,    23,
     100,   101,   102,   103,    98,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,    13,   117,   118,   119,
      19,   121,     7,    24,    31,    23,   120,     7,    24,    26,
      27,    23,    11,   125,     7,    37,    33,    34,   120,    34,
      37,    80,   165,    27,   164,   131,    -1,    -1,    -1,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    -1,    -1,
     160,    -1,    17,    -1,    61,   165,    -1,    64,    65,    66,
     164,    26,    27,    -1,    -1,    -1,    31,    -1,    -1,    -1,
      -1,    36,    -1,    17,    39,    17,    -1,    42,    43,    -1,
      -1,    -1,    26,    27,    26,    27,    -1,    31,    -1,    31,
      -1,    56,    36,    -1,    36,    39,    61,    39,    42,    43,
      42,    43,    -1,    -1,    -1,    -1,    17,    -1,    -1,    -1,
      -1,    -1,    56,    -1,    56,    26,    27,    61,    -1,    61,
      31,    -1,    -1,    -1,    -1,    36,    -1,    17,    39,    -1,
      -1,    42,    43,    -1,    -1,    -1,    26,    27,    -1,    -1,
      -1,    31,    -1,    -1,    -1,    56,    36,    -1,    -1,    39,
      61,    -1,    42,    43,    26,    27,    -1,    -1,    -1,    -1,
      32,    33,    34,    35,    -1,    37,    56,    -1,    -1,    -1,
      -1,    61,    -1,    -1,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    -1,    -1,    -1,    -1,    -1,    -1,    61,
      62,    -1,    64,    65,    66,    26,    27,    -1,    -1,    -1,
      -1,    -1,    33,    34,    35,    -1,    37,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    -1,    -1,    -1,    -1,    -1,    -1,
      61,    62,    -1,    64,    65,    66,    26,    27,    -1,    -1,
      -1,    -1,    32,    33,    34,    -1,    -1,    37,    -1,    -1,
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
      -1,    -1,    -1,    -1,    -1,    61,    26,    27,    64,    65,
      66,    -1,    -1,    33,    34,    -1,    -1,    37,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    -1,    -1,    -1,    -1,    -1,
      -1,    61,    26,    27,    64,    65,    66,    -1,    -1,    33,
      34,    -1,    -1,    37,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    -1,    -1,    -1,    17,    -1,    -1,    61,    -1,    -1,
      64,    65,    66,    26,    27,    -1,    -1,    -1,    31,    -1,
      -1,    -1,    -1,    36,    -1,    17,    39,    17,    -1,    42,
      43,    -1,    45,    -1,    26,    27,    26,    27,    -1,    31,
      32,    31,    -1,    56,    36,    -1,    36,    39,    -1,    39,
      42,    43,    42,    43,    33,    34,    -1,    -1,    37,    -1,
      -1,    -1,    -1,    -1,    56,    -1,    56,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    33,    34,    -1,    -1,
      37,    -1,    61,    -1,    -1,    64,    65,    66,    -1,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    33,    34,
      -1,    -1,    37,    -1,    61,    -1,    -1,    64,    65,    66,
      -1,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      33,    34,    -1,    -1,    37,    -1,    61,    -1,    -1,    64,
      65,    66,    -1,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    33,    34,    -1,    -1,    37,    -1,    61,    -1,
      -1,    64,    65,    66,    -1,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    33,    34,    -1,    -1,    37,    -1,
      61,    -1,    -1,    -1,    65,    66,    -1,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    33,    34,    -1,    -1,
      37,    -1,    61,    -1,    -1,    -1,    65,    66,    -1,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    -1,    -1,
      -1,    -1,    -1,    -1,    61,    -1,    -1,    -1,    65,    66
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     6,    68,    69,    71,     0,     3,    73,    74,    75,
      17,    78,    74,    78,    31,    31,    32,     7,    32,    84,
      85,    24,    78,    83,    86,    95,    24,    23,    32,    19,
      70,    23,    13,    23,    19,    76,    85,    24,     7,    78,
      79,    80,    90,    97,    86,    14,    15,    16,    87,    88,
      78,    26,    27,    31,    36,    39,    42,    43,    56,    61,
      78,    79,    89,    91,    92,    93,    98,    99,   100,   101,
     104,   105,   106,   107,    76,    81,    82,    86,    31,    21,
      23,    11,    20,    72,    31,   101,   101,    89,   101,   101,
     101,    89,    21,    12,    20,    22,    77,    45,    94,   101,
      26,    27,    33,    34,    35,    37,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    61,    62,    64,    65,    66,
      23,    24,    32,    96,   101,    90,    80,    90,   101,    32,
      32,    89,    89,    89,    10,    78,   102,   103,    13,   101,
     101,   101,   101,   101,   101,   101,   101,   101,   101,   101,
     101,   101,   101,   101,    89,   101,   101,   101,    82,   101,
      23,    32,    72,    77,    23,    25,   101,   103,    96
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
#line 109 "chiparser.yy"
    { 
    	  gsDebugMsg("inputs contains a valid Chi-specification\n"); 
          safe_assign(((*yyvalp).appl), gsMakeChiSpec((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl),ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.list))));
		  spec_tree = ((*yyvalp).appl);
		;}
    break;

  case 3:
#line 118 "chiparser.yy"
    {
      	  safe_assign(((*yyvalp).appl), gsMakeModelDef((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (6))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((6) - (6))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed Model Def \n  %T\n", ((*yyvalp).appl));
        ;}
    break;

  case 4:
#line 126 "chiparser.yy"
    { safe_assign(((*yyvalp).appl), gsMakeModelSpec( ATmakeList0(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl) ));
      	  gsDebugMsg("parsed Model Body  \n  %T\n", ((*yyvalp).appl));	
		;}
    break;

  case 5:
#line 130 "chiparser.yy"
    { safe_assign(((*yyvalp).appl), gsMakeModelSpec( (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed Model Body  \n  %T\n", ((*yyvalp).appl));	
		;}
    break;

  case 6:
#line 136 "chiparser.yy"
    {
        processing_models = true;
      ;}
    break;

  case 7:
#line 142 "chiparser.yy"
    {
        processing_models = false;
      ;}
    break;

  case 8:
#line 148 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed Process Definition \n  %T\n", ((*yyvalp).list));	
		;}
    break;

  case 9:
#line 152 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed Process Definition \n  %T\n", ((*yyvalp).list));	
		;}
    break;

  case 10:
#line 159 "chiparser.yy"
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

  case 11:
#line 176 "chiparser.yy"
    { 
      	  safe_assign(((*yyvalp).appl), gsMakeProcDef((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (7))].yystate.yysemantics.yysval.appl), gsMakeProcDecl(ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (7))].yystate.yysemantics.yysval.list))), (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (7))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed proc Def\n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 12:
#line 192 "chiparser.yy"
    {
		  scope_lvl++;
		  gsDebugMsg("Increase Scope to: %d\n",scope_lvl);
		;}
    break;

  case 13:
#line 200 "chiparser.yy"
    { safe_assign(((*yyvalp).appl), gsMakeProcSpec( ATmakeList0(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl) ));
      	  gsDebugMsg("parsed ProcessBody  \n  %T\n", ((*yyvalp).appl));	
		;}
    break;

  case 14:
#line 204 "chiparser.yy"
    { safe_assign(((*yyvalp).appl), gsMakeProcSpec( (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.list), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed ProcessBody  \n  %T\n", ((*yyvalp).appl));	
		;}
    break;

  case 15:
#line 211 "chiparser.yy"
    {
		  assert(scope_lvl > 0);
		  scope_lvl--;
		  gsDebugMsg("Decrease Scope to; %d\n",scope_lvl);
          var_type_map.clear();
		;}
    break;

  case 16:
#line 220 "chiparser.yy"
    { 
 	  	  safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl) );
      	  gsDebugMsg("parsed id's\n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 17:
#line 227 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed localvariables  \n  %T\n", ((*yyvalp).list));	
		;}
    break;

  case 18:
#line 231 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed localvariables \n  %T\n", ((*yyvalp).list));	
		;}
    break;

  case 19:
#line 238 "chiparser.yy"
    {
		  safe_assign(((*yyvalp).appl), gsMakeVarSpec( ATreverse( (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.list) ) ) );
		  gsDebugMsg("parsed VariableList \n %T\n", ((*yyvalp).appl));
		;}
    break;

  case 20:
#line 248 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed id-element \n  %T\n", ((*yyvalp).list));	
		;}
    break;

  case 21:
#line 252 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed id-element \n  %T\n", ((*yyvalp).list));	
		;}
    break;

  case 22:
#line 259 "chiparser.yy"
    {
		  safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl) );
		;}
    break;

  case 23:
#line 263 "chiparser.yy"
    {
		  safe_assign(((*yyvalp).appl), gsMakeDataVarExprID ( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl) ) );
		  gsDebugMsg("parsed Identifier Type With Expression \n %T\n", ((*yyvalp).appl));
		;}
    break;

  case 24:
#line 271 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed id-element \n  %T\n", ((*yyvalp).list));	
		;}
    break;

  case 25:
#line 275 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed id-element \n  %T\n", ((*yyvalp).list));	
		;}
    break;

  case 26:
#line 282 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed formalparameter variables  \n  %T\n", ((*yyvalp).list));	
		;}
    break;

  case 27:
#line 286 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed formalparameter variables \n  %T\n", ((*yyvalp).list));	
		;}
    break;

  case 28:
#line 293 "chiparser.yy"
    {
		  safe_assign(((*yyvalp).appl), gsMakeVarDecl( ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.list)) ) );
		  gsDebugMsg("parsed VariableList \n %T\n", ((*yyvalp).appl));
		;}
    break;

  case 29:
#line 302 "chiparser.yy"
    {
		  /**
			* Build TypeCheck table for declared variables
			*
			* TODO: Add scope
			*
			**/
		  ATermList list = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list);
		  int n = ATgetLength( list );
		  for(int i = 0; i < n ; ++i ){
			 if (var_type_map.end() != var_type_map.find(ATgetFirst( list )))
			 {
			   gsErrorMsg("Variable %T is already defined!\n", ATgetFirst( list ));
			   exit(1);
			 };
			 var_type_map[ATgetFirst( list )]= (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl);
			 list = ATgetTail( list, 1 ) ;
		  }	;
		  safe_assign(((*yyvalp).appl), gsMakeDataVarID (ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list)), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl) ) );
		  gsDebugMsg("parsed IdentifierType\n %T\n", ((*yyvalp).appl));
		  gsDebugMsg("Typecheck Table %d\n", var_type_map.size()); 
  		;}
    break;

  case 31:
#line 402 "chiparser.yy"
    { 
          safe_assign(((*yyvalp).appl), gsMakeType( gsString2ATermAppl( "Bool" ) ) );
      	  gsDebugMsg("parsed Type \n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 32:
#line 407 "chiparser.yy"
    { 
          safe_assign(((*yyvalp).appl), gsMakeType( gsString2ATermAppl("Nat" ) ) );
      	  gsDebugMsg("parsed Type \n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 33:
#line 412 "chiparser.yy"
    { 
          safe_assign(((*yyvalp).appl), gsMakeType( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl) ) );
      	  gsDebugMsg("parsed Type \n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 34:
#line 427 "chiparser.yy"
    { safe_assign(((*yyvalp).appl), gsMakeParenthesisedStat( (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed id-element \n  %T\n", ((*yyvalp).appl));	
		;}
    break;

  case 39:
#line 438 "chiparser.yy"
    { safe_assign(((*yyvalp).appl), gsMakeParStat( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed Parallel statement \n  %T\n", ((*yyvalp).appl));	
		;}
    break;

  case 42:
#line 455 "chiparser.yy"
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
      	  gsDebugMsg("parsed skip statement \n  %T\n", ((*yyvalp).appl));	
		;}
    break;

  case 43:
#line 475 "chiparser.yy"
    { safe_assign(((*yyvalp).appl), gsMakeSkipStat( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.appl), gsMakeSkip() ));
      	  gsDebugMsg("parsed skip statement \n  %T\n", ((*yyvalp).appl));	
		;}
    break;

  case 44:
#line 482 "chiparser.yy"
    { safe_assign(((*yyvalp).appl), gsMakeNil() );
		;}
    break;

  case 45:
#line 485 "chiparser.yy"
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
      	  	gsDebugMsg("parsed OptGuard \n  %T\n", ((*yyvalp).appl));	
		;}
    break;

  case 46:
#line 503 "chiparser.yy"
    { safe_assign(((*yyvalp).appl), gsMakeNil() );
		;}
    break;

  case 47:
#line 506 "chiparser.yy"
    { safe_assign(((*yyvalp).appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl));
		  gsErrorMsg("OptChannel not yet implemented");
		  assert(false);
      	  gsDebugMsg("parsed OptChannel \n  %T\n", ((*yyvalp).appl));	
		;}
    break;

  case 48:
#line 515 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed id's \n  %T\n", ((*yyvalp).list));	
		;}
    break;

  case 49:
#line 519 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed id's\n  %T\n", ((*yyvalp).list));	
		;}
    break;

  case 50:
#line 526 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed expression-element \n  %T\n", ((*yyvalp).list));	
		;}
    break;

  case 51:
#line 530 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed expression-element\n  %T\n", ((*yyvalp).list));	
		;}
    break;

  case 52:
#line 550 "chiparser.yy"
    {
        safe_assign(((*yyvalp).appl), gsMakeInstantiation((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), ATmakeList0()));
      ;}
    break;

  case 53:
#line 554 "chiparser.yy"
    {
        safe_assign(((*yyvalp).appl), gsMakeInstantiation((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.appl), ATreverse((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.list))));
      ;}
    break;

  case 54:
#line 561 "chiparser.yy"
    { safe_assign(((*yyvalp).appl), gsMakeSepStat( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed SEP statement \n  %T\n", ((*yyvalp).appl));	
		;}
    break;

  case 55:
#line 565 "chiparser.yy"
    { safe_assign(((*yyvalp).appl), gsMakeAltStat( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.appl), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed ALT statement \n  %T\n", ((*yyvalp).appl));	
		;}
    break;

  case 56:
#line 576 "chiparser.yy"
    { safe_assign(((*yyvalp).appl), gsMakeStarStat( (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed STAR statement \n  %T\n", ((*yyvalp).appl));	
		;}
    break;

  case 57:
#line 580 "chiparser.yy"
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

  case 58:
#line 597 "chiparser.yy"
    { safe_assign(((*yyvalp).appl), gsMakeDeltaStat((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.appl), gsMakeDelta()));
      	  gsDebugMsg("parsed deadlock statement \n  %T\n", ((*yyvalp).appl));	
		;}
    break;

  case 59:
#line 610 "chiparser.yy"
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

  case 64:
#line 641 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATmakeList1((ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed expression-element \n  %T\n", ((*yyvalp).list));	
		;}
    break;

  case 65:
#line 645 "chiparser.yy"
    { safe_assign(((*yyvalp).list), ATinsert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.list), (ATerm) (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.appl)));
      	  gsDebugMsg("parsed expression-element\n  %T\n", ((*yyvalp).list));	
		;}
    break;

  case 66:
#line 651 "chiparser.yy"
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

  case 67:
#line 678 "chiparser.yy"
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
		      gsErrorMsg("BasicExpression: Variable %T is not defined!\n", (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl) );
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

  case 68:
#line 710 "chiparser.yy"
    { 
 	  		safe_assign(((*yyvalp).appl), 
				gsMakeExpression( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl), 
					gsMakeType( gsString2ATermAppl("Bool" )) 
				)
			);
      		gsDebugMsg("parsed Expression's\n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 69:
#line 719 "chiparser.yy"
    { 
 	  		safe_assign(((*yyvalp).appl), 
				gsMakeExpression( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl), 
					gsMakeType( gsString2ATermAppl("Bool" )) 
				)
			);
      		gsDebugMsg("parsed Expression's\n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 70:
#line 728 "chiparser.yy"
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

  case 71:
#line 747 "chiparser.yy"
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

  case 72:
#line 766 "chiparser.yy"
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

  case 73:
#line 793 "chiparser.yy"
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

  case 74:
#line 820 "chiparser.yy"
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

  case 75:
#line 850 "chiparser.yy"
    { 
 	  		safe_assign(((*yyvalp).appl), 
				gsMakeExpression( (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.appl), 
					gsMakeType( gsString2ATermAppl("Nat" )) 
				)
			);
      		gsDebugMsg("parsed Expression's\n  %T\n", ((*yyvalp).appl));
		;}
    break;

  case 76:
#line 859 "chiparser.yy"
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

  case 77:
#line 879 "chiparser.yy"
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

  case 78:
#line 899 "chiparser.yy"
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

  case 79:
#line 925 "chiparser.yy"
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

  case 80:
#line 951 "chiparser.yy"
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

  case 81:
#line 977 "chiparser.yy"
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

  case 82:
#line 1003 "chiparser.yy"
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

  case 83:
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

  case 84:
#line 1055 "chiparser.yy"
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

  case 85:
#line 1081 "chiparser.yy"
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

  case 86:
#line 1107 "chiparser.yy"
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

  case 87:
#line 1133 "chiparser.yy"
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

  case 88:
#line 1159 "chiparser.yy"
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

  case 89:
#line 1185 "chiparser.yy"
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

  case 90:
#line 1211 "chiparser.yy"
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

  case 91:
#line 1240 "chiparser.yy"
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

  case 92:
#line 1257 "chiparser.yy"
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
#line 2553 "chiparser.cpp"
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


#line 1301 "chiparser.yy"



