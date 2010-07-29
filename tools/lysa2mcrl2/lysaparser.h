
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
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



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE lysayylval;

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

extern YYLTYPE lysayylloc;

