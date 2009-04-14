/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 1

/* Substitute the variable and function names.  */
#define yyparse lysayyparse
#define yylex   lysayylex
#define yyerror lysayyerror
#define yylval  lysayylval
#define yychar  lysayychar
#define yydebug lysayydebug
#define yynerrs lysayynerrs
#define yylloc lysayylloc

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     T_DECRYPT = 258,
     T_AS = 259,
     T_IN = 260,
     T_LET = 261,
     T_SUBSET = 262,
     T_UNION = 263,
     T_NEW = 264,
     T_AT = 265,
     T_DEST = 266,
     T_ORIG = 267,
     T_CPDY = 268,
     T_DY = 269,
     T_HINT = 270,
     T_LBRACE_SUBSCRIPT = 271,
     T_UNDERSCORE = 272,
     T_LPAREN = 273,
     T_RPAREN = 274,
     T_LBOX = 275,
     T_RBOX = 276,
     T_LT = 277,
     T_GT = 278,
     T_EQUALS = 279,
     T_SEMICOLON = 280,
     T_COLON = 281,
     T_DOT = 282,
     T_COMMA = 283,
     T_PIPE = 284,
     T_LBRACE = 285,
     T_RBRACE = 286,
     T_LBRACE_PIPE = 287,
     T_RBRACE_PIPE = 288,
     T_BANG = 289,
     T_PLUS_MINUS = 290,
     T_PLUS_OR_MINUS = 291,
     T_IDENT = 292,
     T_NUM = 293,
     T_ALNUM = 294,
     T_ISET_DEF = 295,
     T_TYPENAME = 296,
     T_INDEXED_PIPE = 297
   };
#endif
/* Tokens.  */
#define T_DECRYPT 258
#define T_AS 259
#define T_IN 260
#define T_LET 261
#define T_SUBSET 262
#define T_UNION 263
#define T_NEW 264
#define T_AT 265
#define T_DEST 266
#define T_ORIG 267
#define T_CPDY 268
#define T_DY 269
#define T_HINT 270
#define T_LBRACE_SUBSCRIPT 271
#define T_UNDERSCORE 272
#define T_LPAREN 273
#define T_RPAREN 274
#define T_LBOX 275
#define T_RBOX 276
#define T_LT 277
#define T_GT 278
#define T_EQUALS 279
#define T_SEMICOLON 280
#define T_COLON 281
#define T_DOT 282
#define T_COMMA 283
#define T_PIPE 284
#define T_LBRACE 285
#define T_RBRACE 286
#define T_LBRACE_PIPE 287
#define T_RBRACE_PIPE 288
#define T_BANG 289
#define T_PLUS_MINUS 290
#define T_PLUS_OR_MINUS 291
#define T_IDENT 292
#define T_NUM 293
#define T_ALNUM 294
#define T_ISET_DEF 295
#define T_TYPENAME 296
#define T_INDEXED_PIPE 297




/* Copy the first part of user declarations.  */
#line 1 ".\\lysaparser.yy"

#include "lysa.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <utility>
#include <cctype>
#include <string.h>

//disable MSVC specific warnings
#ifdef BOOST_MSVC
#pragma warning( disable : 4273 4065 )
#endif


using namespace std;
using namespace lysa;


//semantic values are always stored by lysaLexer.ll as shared pointers to lysa expressions
typedef boost::shared_ptr<Expression> semval;
typedef boost::shared_ptr<String> semval_str;
#define YYSTYPE semval

//external declarations from lysalexer.ll
void lysayyerror( const char *s );
int lysayylex( void );
extern boost::shared_ptr<Expression> parse_tree;


#define YYMAXDEPTH 160000



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
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

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 248 "lysaparser.cpp"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

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

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
	     && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
    YYLTYPE yyls;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  28
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   164

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  43
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  19
/* YYNRULES -- Number of rules.  */
#define YYNRULES  61
/* YYNRULES -- Number of states.  */
#define YYNSTATES  154

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   297

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
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
      35,    36,    37,    38,    39,    40,    41,    42
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     5,     8,    12,    14,    16,    22,    28,
      40,    46,    53,    62,    72,    75,    81,    85,    92,    99,
     101,   105,   112,   119,   123,   127,   129,   131,   132,   134,
     138,   139,   144,   153,   154,   159,   168,   171,   173,   175,
     179,   182,   184,   188,   191,   196,   200,   204,   207,   210,
     213,   215,   219,   221,   225,   229,   233,   239,   241,   243,
     247,   251
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      44,     0,    -1,    45,    -1,    45,    15,    -1,    18,    45,
      19,    -1,    38,    -1,    14,    -1,    22,    50,    23,    27,
      45,    -1,    18,    47,    19,    27,    45,    -1,     3,    46,
       4,    30,    47,    31,    26,    46,    52,     5,    45,    -1,
      18,     9,    55,    19,    45,    -1,    18,     9,    35,    55,
      19,    45,    -1,    18,     9,    16,    58,    31,    55,    19,
      45,    -1,    18,     9,    16,    58,    31,    35,    55,    19,
      45,    -1,    34,    45,    -1,    29,    16,    58,    31,    45,
      -1,    45,    29,    45,    -1,     6,    37,     7,    61,     5,
      45,    -1,     6,    37,    24,    61,     5,    45,    -1,    55,
      -1,    18,    46,    19,    -1,    30,    50,    31,    26,    46,
      51,    -1,    32,    50,    33,    26,    46,    51,    -1,    49,
      25,    50,    -1,    50,    25,    48,    -1,    49,    -1,    50,
      -1,    -1,    46,    -1,    50,    28,    46,    -1,    -1,    20,
      10,    53,    21,    -1,    20,    10,    53,    11,    30,    54,
      31,    21,    -1,    -1,    20,    10,    53,    21,    -1,    20,
      10,    53,    12,    30,    54,    31,    21,    -1,    37,    56,
      -1,    37,    -1,    53,    -1,    54,    28,    53,    -1,    37,
      56,    -1,    37,    -1,    37,    36,    56,    -1,    37,    36,
      -1,    37,    56,    26,    41,    -1,    37,    26,    41,    -1,
      16,    57,    31,    -1,    17,    39,    -1,    17,    37,    -1,
      17,    38,    -1,    60,    -1,    57,    28,    60,    -1,    59,
      -1,    58,    28,    59,    -1,    37,     5,    37,    -1,    37,
       5,    61,    -1,    37,     5,    37,     8,    61,    -1,    37,
      -1,    38,    -1,    30,    57,    31,    -1,    61,     8,    61,
      -1,    40,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   114,   114,   120,   125,   128,   144,   152,   160,   168,
     176,   182,   188,   194,   202,   208,   214,   225,   232,   241,
     242,   245,   251,   263,   269,   278,   278,   281,   287,   293,
     302,   307,   313,   321,   326,   332,   340,   346,   354,   360,
     368,   374,   380,   387,   394,   401,   410,   414,   420,   426,
     435,   441,   449,   455,   461,   467,   474,   481,   481,   484,
     490,   496
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "T_DECRYPT", "T_AS", "T_IN", "T_LET",
  "T_SUBSET", "T_UNION", "T_NEW", "T_AT", "T_DEST", "T_ORIG", "T_CPDY",
  "T_DY", "T_HINT", "T_LBRACE_SUBSCRIPT", "T_UNDERSCORE", "T_LPAREN",
  "T_RPAREN", "T_LBOX", "T_RBOX", "T_LT", "T_GT", "T_EQUALS",
  "T_SEMICOLON", "T_COLON", "T_DOT", "T_COMMA", "T_PIPE", "T_LBRACE",
  "T_RBRACE", "T_LBRACE_PIPE", "T_RBRACE_PIPE", "T_BANG", "T_PLUS_MINUS",
  "T_PLUS_OR_MINUS", "T_IDENT", "T_NUM", "T_ALNUM", "T_ISET_DEF",
  "T_TYPENAME", "T_INDEXED_PIPE", "$accept", "start", "proc", "term",
  "pmatch_terms", "terms_noterms", "noterms", "terms", "anno_dest",
  "anno_orig", "cryptopoint", "cryptopoints", "name", "subscript_indices",
  "indices", "index_defs", "index_def", "index", "iset", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    43,    44,    45,    45,    45,    45,    45,    45,    45,
      45,    45,    45,    45,    45,    45,    45,    45,    45,    46,
      46,    46,    46,    47,    47,    48,    48,    49,    50,    50,
      51,    51,    51,    52,    52,    52,    53,    53,    54,    54,
      55,    55,    55,    55,    55,    55,    56,    56,    56,    56,
      57,    57,    58,    58,    59,    59,    59,    60,    60,    61,
      61,    61
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     3,     1,     1,     5,     5,    11,
       5,     6,     8,     9,     2,     5,     3,     6,     6,     1,
       3,     6,     6,     3,     3,     1,     1,     0,     1,     3,
       0,     4,     8,     0,     4,     8,     2,     1,     1,     3,
       2,     1,     3,     2,     4,     3,     3,     2,     2,     2,
       1,     3,     1,     3,     3,     3,     5,     1,     1,     3,
       3,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     6,    27,     0,     0,     0,     5,     0,
       2,     0,     0,     0,    41,     0,    19,     0,     0,    27,
       0,    28,     0,     0,     0,     0,     0,    14,     1,     3,
       0,     0,     0,     0,     0,     0,     0,    43,    40,     0,
       0,     0,     0,     0,     0,    28,     4,     0,     0,    27,
       0,     0,     0,     0,    52,    16,    20,     0,     0,    57,
      58,     0,    50,    48,    49,    47,    45,    42,     0,    27,
       0,    61,     0,     0,     0,     0,     0,     0,    23,    24,
      25,    26,    29,     0,     0,     0,     0,     0,     0,     0,
      46,    44,     0,     0,     0,     0,     0,     0,     0,    10,
       8,     7,    54,    55,    53,    15,    30,    30,    51,     0,
      59,    17,    60,    18,     0,     0,    11,     0,     0,    21,
      22,     0,     0,     0,    56,     0,    33,     0,    12,    37,
       0,     0,     0,    13,    36,     0,    31,     0,     0,     0,
       0,     9,    38,     0,     0,    34,     0,     0,     0,    39,
      32,     0,     0,    35
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     9,    20,    21,    22,    79,    23,    24,   119,   132,
     142,   143,    16,    38,    61,    53,    54,    62,    72
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -82
static const yytype_int16 yypact[] =
{
       7,    55,   -31,   -82,     5,    55,    17,     7,   -82,    24,
     -11,    55,    55,    55,    38,    57,   -82,     8,    12,     5,
      31,   -82,    44,    53,    54,    43,    76,    69,   -82,   -82,
       7,    97,    62,    42,    87,    80,    50,   104,   102,    99,
      27,    27,    76,    89,   111,    97,   -82,   105,    55,    55,
      55,   106,   126,    71,   -82,    69,   -82,   108,   109,   -82,
     -82,    72,   -82,   -82,   -82,   -82,   -82,   -82,    95,    55,
      87,   -82,    96,   100,    78,   118,     7,     7,   112,   -82,
     -82,   112,   -82,     7,    32,    76,     7,    55,    55,    87,
     -82,   -82,   110,    79,     7,    27,     7,     3,     7,    69,
      69,    69,   131,   134,   -82,   -11,   123,   123,   -82,   119,
     -82,    69,   -82,    69,    89,   125,    69,    27,   136,   -82,
     -82,    55,   128,     7,   134,   113,   129,     7,    69,   104,
      47,   138,   146,    69,   -82,   122,   -82,   113,     7,   113,
      10,    69,   -82,    83,   124,   -82,   113,   132,   113,   -82,
     -82,    84,   135,   -82
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -82,   -82,     0,     1,    86,   -82,   114,     4,    51,   -82,
     -81,     9,   -17,   -34,    90,   117,    77,    75,   -36
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      10,    44,    15,    67,    29,    73,    17,    27,     1,    25,
       1,     2,    31,     2,    18,    40,    32,    33,    30,     3,
      45,     3,   144,    19,    28,     4,    75,     5,    42,     5,
      55,   145,    41,    26,     6,    12,     6,    13,   114,     7,
      14,     7,    14,     8,   130,     8,    29,    43,   103,    14,
      46,    82,    78,    81,    34,    35,   140,    70,   135,   112,
      30,    39,    70,    47,    36,   149,    51,    71,   136,   102,
      50,    50,    71,    11,    37,    58,    99,   100,    48,    49,
     115,   124,    50,   101,    29,    12,   105,    13,   106,   107,
      50,    66,    14,    57,   111,   134,   113,   122,   116,    85,
      89,    94,    86,    90,    95,    96,    85,    89,    95,    97,
     110,   146,   146,    52,   147,   152,    56,    63,    64,    65,
      34,    35,   126,   128,    59,    60,    14,   133,    68,    69,
      76,    84,    77,    83,    87,    88,    91,    98,   141,   117,
      50,   109,    95,   118,   123,   121,   125,   127,   137,   131,
     129,   138,   139,   150,   148,    92,   153,   151,   120,    74,
      93,     0,   104,    80,   108
};

static const yytype_int16 yycheck[] =
{
       0,    18,     1,    37,    15,    41,    37,     7,     3,     5,
       3,     6,    11,     6,     9,     7,    12,    13,    29,    14,
      19,    14,    12,    18,     0,    18,    43,    22,    16,    22,
      30,    21,    24,    16,    29,    30,    29,    32,    35,    34,
      37,    34,    37,    38,   125,    38,    15,    35,    84,    37,
      19,    50,    48,    49,    16,    17,   137,    30,    11,    95,
      29,     4,    30,    19,    26,   146,    23,    40,    21,    37,
      28,    28,    40,    18,    36,    33,    76,    77,    25,    25,
      97,   117,    28,    83,    15,    30,    86,    32,    87,    88,
      28,    41,    37,    31,    94,   129,    96,   114,    98,    28,
      28,     5,    31,    31,     8,     5,    28,    28,     8,    31,
      31,    28,    28,    37,    31,    31,    19,    37,    38,    39,
      16,    17,   121,   123,    37,    38,    37,   127,    26,    30,
      19,     5,    27,    27,    26,    26,    41,    19,   138,     8,
      28,    31,     8,    20,    19,    26,    10,    19,    10,    20,
      37,     5,    30,    21,    30,    69,    21,   148,   107,    42,
      70,    -1,    85,    49,    89
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     6,    14,    18,    22,    29,    34,    38,    44,
      45,    18,    30,    32,    37,    46,    55,    37,     9,    18,
      45,    46,    47,    49,    50,    50,    16,    45,     0,    15,
      29,    46,    50,    50,    16,    17,    26,    36,    56,     4,
       7,    24,    16,    35,    55,    46,    19,    19,    25,    25,
      28,    23,    37,    58,    59,    45,    19,    31,    33,    37,
      38,    57,    60,    37,    38,    39,    41,    56,    26,    30,
      30,    40,    61,    61,    58,    55,    19,    27,    50,    48,
      49,    50,    46,    27,     5,    28,    31,    26,    26,    28,
      31,    41,    47,    57,     5,     8,     5,    31,    19,    45,
      45,    45,    37,    61,    59,    45,    46,    46,    60,    31,
      31,    45,    61,    45,    35,    55,    45,     8,    20,    51,
      51,    26,    55,    19,    61,    10,    46,    19,    45,    37,
      53,    20,    52,    45,    56,    11,    21,    10,     5,    30,
      53,    45,    53,    54,    12,    21,    28,    31,    30,    53,
      21,    54,    31,    21
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value, Location); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE (yylocationp);
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

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, yylocationp)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yylsp, yyrule)
    YYSTYPE *yyvsp;
    YYLTYPE *yylsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       , &(yylsp[(yyi + 1) - (yynrhs)])		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, yylsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
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
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
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
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
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

      yyarg[0] = yytname[yytype];
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
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
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
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, yylocationp)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;
/* Location data for the look-ahead symbol.  */
YYLTYPE yylloc;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;

  /* The location stack.  */
  YYLTYPE yylsa[YYINITDEPTH];
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;
  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[2];

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;
  yylsp = yyls;
#if YYLTYPE_IS_TRIVIAL
  /* Initialize the default location before parsing starts.  */
  yylloc.first_line   = yylloc.last_line   = 1;
  yylloc.first_column = yylloc.last_column = 0;
#endif

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;
	YYLTYPE *yyls1 = yyls;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);
	yyls = yyls1;
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);
	YYSTACK_RELOCATE (yyls);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
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

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;
  *++yylsp = yylloc;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 115 ".\\lysaparser.yy"
    { 
	parse_tree = (yyvsp[(1) - (1)]);
	(yyval) = (yyvsp[(1) - (1)]);
;}
    break;

  case 3:
#line 121 ".\\lysaparser.yy"
    {
		(yyval)->hint(static_pointer_cast<String>((yyvsp[(2) - (2)]))->s);
	;}
    break;

  case 4:
#line 125 ".\\lysaparser.yy"
    { (yyval) = (yyvsp[(2) - (3)]); ;}
    break;

  case 5:
#line 129 ".\\lysaparser.yy"
    { 
	if(static_pointer_cast<String>((yyvsp[(1) - (1)]))->s=="0")
	{
		set_current_position((yyloc));
		semval e(new Zero());
		(yyval) = e;
	}
	else
	{
		yyerror("syntax error: \"0\" expected");
		YYERROR;
	}
;}
    break;

  case 6:
#line 145 ".\\lysaparser.yy"
    { 
	set_current_position((yyloc));
	semval e(new DY());
	(yyval) = e;
;}
    break;

  case 7:
#line 153 ".\\lysaparser.yy"
    {
	set_current_position((yyloc));
	semval e(new Send((yyvsp[(2) - (5)]), (yyvsp[(5) - (5)])));
	(yyval) = e;
;}
    break;

  case 8:
#line 161 ".\\lysaparser.yy"
    {
	set_current_position((yyloc));
	semval e(new Receive((yyvsp[(2) - (5)]), (yyvsp[(5) - (5)])));
	(yyval) = e;
;}
    break;

  case 9:
#line 169 ".\\lysaparser.yy"
    {
	set_current_position((yyloc));
	semval e(new Decrypt((yyvsp[(2) - (11)]), (yyvsp[(5) - (11)]), (yyvsp[(8) - (11)]), (yyvsp[(9) - (11)]), (yyvsp[(11) - (11)])));
	(yyval) = e;
;}
    break;

  case 10:
#line 177 ".\\lysaparser.yy"
    {
	set_current_position((yyloc));
	semval e(new New((yyvsp[(3) - (5)]), false, (yyvsp[(5) - (5)])));
	(yyval) = e;
;}
    break;

  case 11:
#line 183 ".\\lysaparser.yy"
    {
	set_current_position((yyloc));
	semval e(new New((yyvsp[(4) - (6)]), true, (yyvsp[(6) - (6)])));
	(yyval) = e;
;}
    break;

  case 12:
#line 189 ".\\lysaparser.yy"
    {
	set_current_position((yyloc));
	semval e(new New((yyvsp[(6) - (8)]), false, (yyvsp[(4) - (8)]), (yyvsp[(8) - (8)])));
	(yyval) = e;
;}
    break;

  case 13:
#line 195 ".\\lysaparser.yy"
    {
	set_current_position((yyloc));
	semval e(new New((yyvsp[(7) - (9)]), true, (yyvsp[(4) - (9)]), (yyvsp[(9) - (9)])));
	(yyval) = e;
;}
    break;

  case 14:
#line 203 ".\\lysaparser.yy"
    {
	set_current_position((yyloc));
	semval e(new Replication((yyvsp[(2) - (2)])));
	(yyval) = e;
;}
    break;

  case 15:
#line 209 ".\\lysaparser.yy"
    {
	set_current_position((yyloc));
	semval e(new IndexedParallel((yyvsp[(5) - (5)]), (yyvsp[(3) - (5)])));
	(yyval) = e;
;}
    break;

  case 16:
#line 215 ".\\lysaparser.yy"
    {
	set_current_position((yylsp[(2) - (3)]));
	std::list<semval> l;
	l.push_back((yyvsp[(1) - (3)]));
	l.push_back((yyvsp[(3) - (3)]));
	semval e(new OrdinaryParallel(l));
	(yyval) = e;
;}
    break;

  case 17:
#line 226 ".\\lysaparser.yy"
    {
	set_current_position((yyloc));
	semval e(new Let(LySa, (yyvsp[(2) - (6)]), (yyvsp[(4) - (6)]), (yyvsp[(6) - (6)])));
	(yyval) = e;
;}
    break;

  case 18:
#line 233 ".\\lysaparser.yy"
    {
	set_current_position((yyloc));
	semval e(new Let(TypedLySa, (yyvsp[(2) - (6)]), (yyvsp[(4) - (6)]), (yyvsp[(6) - (6)])));
	(yyval) = e;
;}
    break;

  case 20:
#line 242 ".\\lysaparser.yy"
    { (yyval) = (yyvsp[(2) - (3)]); ;}
    break;

  case 21:
#line 246 ".\\lysaparser.yy"
    {
	set_current_position((yyloc));
	semval e(new Ciphertext(false, (yyvsp[(2) - (6)]), (yyvsp[(5) - (6)]), (yyvsp[(6) - (6)])));
	(yyval) = e;
;}
    break;

  case 22:
#line 252 ".\\lysaparser.yy"
    {
	set_current_position((yyloc));
	semval e(new Ciphertext(true, (yyvsp[(2) - (6)]), (yyvsp[(5) - (6)]), (yyvsp[(6) - (6)])));
	(yyval) = e;
;}
    break;

  case 23:
#line 264 ".\\lysaparser.yy"
    {
	set_current_position((yyloc));
	semval e(new PMatchTerms((yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])));
	(yyval) = e;
;}
    break;

  case 24:
#line 270 ".\\lysaparser.yy"
    {
	set_current_position((yyloc));
	semval e(new PMatchTerms((yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])));
	(yyval) = e;
;}
    break;

  case 27:
#line 281 ".\\lysaparser.yy"
    { 
	set_current_position((yyloc));
	semval e(new Terms());
	(yyval) = e;
;}
    break;

  case 28:
#line 288 ".\\lysaparser.yy"
    {
		set_current_position((yyloc));
		semval e(new Terms((yyvsp[(1) - (1)])));
		(yyval) = e;
	;}
    break;

  case 29:
#line 294 ".\\lysaparser.yy"
    {
		dynamic_pointer_cast<Terms>((yyvsp[(1) - (3)]))->push_back((yyvsp[(3) - (3)]));
		(yyval) = (yyvsp[(1) - (3)]);
	;}
    break;

  case 30:
#line 302 ".\\lysaparser.yy"
    {
		set_current_position((yyloc));
		semval e(new Annotation(true));
		(yyval) = e;
	;}
    break;

  case 31:
#line 308 ".\\lysaparser.yy"
    {
		set_current_position((yyloc));
		semval e(new Annotation(true, (yyvsp[(3) - (4)])));
		(yyval) = e;
	;}
    break;

  case 32:
#line 314 ".\\lysaparser.yy"
    {
		set_current_position((yyloc));
		semval e(new Annotation(true, (yyvsp[(3) - (8)]), (yyvsp[(6) - (8)])));
		(yyval) = e;
	;}
    break;

  case 33:
#line 321 ".\\lysaparser.yy"
    {
		set_current_position((yyloc));
		semval e(new Annotation(false));
		(yyval) = e;
	;}
    break;

  case 34:
#line 327 ".\\lysaparser.yy"
    {
		set_current_position((yyloc));
		semval e(new Annotation(false, (yyvsp[(3) - (4)])));
		(yyval) = e;
	;}
    break;

  case 35:
#line 333 ".\\lysaparser.yy"
    {
		set_current_position((yyloc));
		semval e(new Annotation(false, (yyvsp[(3) - (8)]), (yyvsp[(6) - (8)])));
		(yyval) = e;
	;}
    break;

  case 36:
#line 341 ".\\lysaparser.yy"
    { 
		set_current_position((yyloc));
		semval e(new Cryptopoint((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])));
		(yyval) = e;
	;}
    break;

  case 37:
#line 347 ".\\lysaparser.yy"
    { 
		set_current_position((yyloc));
		semval e(new Cryptopoint((yyvsp[(1) - (1)])));
		(yyval) = e;
	;}
    break;

  case 38:
#line 355 ".\\lysaparser.yy"
    {
		set_current_position((yyloc));
		semval e(new Cryptopoints((yyvsp[(1) - (1)])));
		(yyval) = e;
	;}
    break;

  case 39:
#line 361 ".\\lysaparser.yy"
    {
		dynamic_pointer_cast<Cryptopoints>((yyvsp[(1) - (3)]))->push_back((yyvsp[(3) - (3)]));
		(yyval) = (yyvsp[(1) - (3)]);
	;}
    break;

  case 40:
#line 369 ".\\lysaparser.yy"
    { 
		set_current_position((yyloc));
		semval e(new Name((yyvsp[(1) - (2)]), (yyvsp[(2) - (2)])));
		(yyval) = e;
	;}
    break;

  case 41:
#line 375 ".\\lysaparser.yy"
    { 
		set_current_position((yyloc));
		semval e(new Name((yyvsp[(1) - (1)])));
		(yyval) = e;
	;}
    break;

  case 42:
#line 381 ".\\lysaparser.yy"
    { 
		set_current_position((yyloc));
		bool isPlus = static_pointer_cast<String>((yyvsp[(2) - (3)]))->s=="+";
		semval e(new ASymName((yyvsp[(1) - (3)]), (yyvsp[(3) - (3)]), isPlus));
		(yyval) = e;
	;}
    break;

  case 43:
#line 388 ".\\lysaparser.yy"
    { 
		set_current_position((yyloc));
		bool isPlus = static_pointer_cast<String>((yyvsp[(2) - (2)]))->s=="+";
		semval e(new ASymName((yyvsp[(1) - (2)]), isPlus));
		(yyval) = e;
	;}
    break;

  case 44:
#line 395 ".\\lysaparser.yy"
    { 
		set_current_position((yyloc));
		std::string t = static_pointer_cast<String>((yyvsp[(4) - (4)]))->s;
		semval e(new TypedVar((yyvsp[(1) - (4)]), (yyvsp[(2) - (4)]), (t=="C") ? TypedVar::V_Ciphertext : TypedVar::V_Name));
		(yyval) = e;
	;}
    break;

  case 45:
#line 402 ".\\lysaparser.yy"
    { 
		set_current_position((yyloc));
		std::string t = static_pointer_cast<String>((yyvsp[(3) - (3)]))->s;
		semval e(new TypedVar((yyvsp[(1) - (3)]), (t=="C") ? TypedVar::V_Ciphertext : TypedVar::V_Name));

		(yyval) = e;
	;}
    break;

  case 46:
#line 411 ".\\lysaparser.yy"
    {
		(yyval) = (yyvsp[(2) - (3)]);
	;}
    break;

  case 47:
#line 415 ".\\lysaparser.yy"
    {
		set_current_position((yylsp[(2) - (2)]));
		semval e(new Indices((yyvsp[(2) - (2)]), true));
		(yyval) = e;
	;}
    break;

  case 48:
#line 421 ".\\lysaparser.yy"
    {
		set_current_position((yylsp[(2) - (2)]));
		semval e(new Indices((yyvsp[(2) - (2)]), true));
		(yyval) = e;
	;}
    break;

  case 49:
#line 427 ".\\lysaparser.yy"
    {
		set_current_position((yylsp[(2) - (2)]));
		semval e(new Indices((yyvsp[(2) - (2)]), true));
		(yyval) = e;
	;}
    break;

  case 50:
#line 436 ".\\lysaparser.yy"
    {
		set_current_position((yyloc));
		semval e(new Indices((yyvsp[(1) - (1)])));
		(yyval) = e;
	;}
    break;

  case 51:
#line 442 ".\\lysaparser.yy"
    {
		static_pointer_cast<Indices>((yyvsp[(1) - (3)]))->push_back(static_pointer_cast<String>((yyvsp[(3) - (3)]))->s);
		semval e((yyvsp[(1) - (3)]));
		(yyval) = e;
	;}
    break;

  case 52:
#line 450 ".\\lysaparser.yy"
    {
		set_current_position((yyloc));
		semval e(new IndexDefs((yyvsp[(1) - (1)])));
		(yyval) = e;
	;}
    break;

  case 53:
#line 456 ".\\lysaparser.yy"
    {
		dynamic_pointer_cast<IndexDefs>((yyvsp[(1) - (3)]))->push_back((yyvsp[(3) - (3)]));
		(yyval) = (yyvsp[(1) - (3)]);
	;}
    break;

  case 54:
#line 462 ".\\lysaparser.yy"
    {
		set_current_position((yyloc));
		semval e(new IndexDef((yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])));
		(yyval) = e;
	;}
    break;

  case 55:
#line 468 ".\\lysaparser.yy"
    {
		set_current_position((yyloc));
		semval empty(new String(""));
		semval e(new IndexDef((yyvsp[(1) - (3)]), empty, (yyvsp[(3) - (3)])));
		(yyval) = e;
	;}
    break;

  case 56:
#line 475 ".\\lysaparser.yy"
    {
		set_current_position((yyloc));
		semval e(new IndexDef((yyvsp[(1) - (5)]), (yyvsp[(3) - (5)]), (yyvsp[(5) - (5)])));
		(yyval) = e;
	;}
    break;

  case 59:
#line 485 ".\\lysaparser.yy"
    {
		set_current_position((yyloc));
		semval e(new IsetIndices((yyvsp[(2) - (3)])));
		(yyval) = e;
	;}
    break;

  case 60:
#line 491 ".\\lysaparser.yy"
    {
		set_current_position((yyloc));
		semval e(new IsetUnion((yyvsp[(1) - (3)]), (yyvsp[(3) - (3)])));
		(yyval) = e;
	;}
    break;

  case 61:
#line 497 ".\\lysaparser.yy"
    {
		set_current_position((yyloc));
		semval e(new IsetDef((yyvsp[(1) - (1)])));
		(yyval) = e;
	;}
    break;


/* Line 1267 of yacc.c.  */
#line 2092 "lysaparser.cpp"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }

  yyerror_range[0] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval, &yylloc);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  yyerror_range[0] = yylsp[1-yylen];
  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      yyerror_range[0] = *yylsp;
      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;

  yyerror_range[1] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the look-ahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, (yyerror_range - 1), 2);
  *++yylsp = yyloc;

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval, &yylloc);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, yylsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


#line 505 ".\\lysaparser.yy"


//no local functions definitions.
