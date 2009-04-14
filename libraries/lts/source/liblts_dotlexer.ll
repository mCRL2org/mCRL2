%{
// Author(s): Muck van Weerdenburg
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file dotlexer.ll

#include <string>
#include <cstdio>
#include <aterm2.h>
#include "mcrl2/core/messaging.h"
#include "mcrl2/lts/lts.h"
#include "liblts_dotlexer.h"
#include "mcrl2/liblts_dotparser.hpp"

using namespace mcrl2::core;

//using namespace std;

//fix for the broken cygwin versions of flex
#ifdef __CYGWIN__
#include <iostream>
using std::cerr;
using std::cin;
using std::cout;
#endif

using namespace mcrl2::lts;

//Global precondition: the ATerm library has been initialised

//external declarations
int dotyyparse(void);          /* declared in dotparser.cpp */
extern YYSTYPE dotyylval;      /* declared in dotparser.cpp */

//global declarations, used by dotparser.cpp
int  dotyylex(void);           /* lexer function */
void dotyyerror(const char *s);/* error function */

//local declarations
class concrete_dot_lexer : public dot_lexer, public dotyyFlexLexer {
public:
  concrete_dot_lexer(void);               /* constructor */
  int yylex(void);               /* the generated lexer function */
  void yyerror(const char *s);   /* error function */
  bool parse_stream(std::istream &stream, lts &l);

protected:
  void processId();
  void processQuoted();
};

//implement yylex in concrete_dot_lexer instead of dotyyFlexLexer
//(this gets rid of global variables but is ugly in its own right)
#define YY_DECL int concrete_dot_lexer::yylex()
int dotyyFlexLexer::yylex(void) { return 1; }

static concrete_dot_lexer *clexer = NULL;  /* lexer object, used by parse_stream */
dot_lexer *dot_lexer_obj = NULL;    /* lexer object, used by dotparser */

static int lineNo=1, posNo=1;
extern void dotyyerror(const char* s);
%}
%option c++
%option prefix="dotyy"
%option nounput
Quoted	   \"[^\"]*\"
Name	   [a-zA-Z_][a-zA-Z0-9_]*
Number     [-]?((\.[0-9]+)|([0-9]+(\.[0-9]+)?))

%x COMMENT
%%

"/*"      { posNo += 2; BEGIN(COMMENT); }
<COMMENT>{
"*/"      { posNo += 2;  BEGIN(INITIAL); }
\r?\n     { lineNo++; posNo=1; }
.         { posNo++; }
}
"//"[^\n]*\n                     { lineNo++; posNo=1; }
#[^\n]*\n { lineNo++; posNo=1; }

[ \t]	  { posNo += YYLeng(); }
\r?\n     { lineNo++; posNo=1; }
[dD][iI][gG][rR][aA][pP][hH]     { posNo += YYLeng(); return DIGRAPH; }
[gG][rR][aA][pP][hH]             { posNo += YYLeng(); return GRAPH; }
[sS][tT][rR][iI][cC][tT]         { posNo += YYLeng(); return STRICT; }
[sS][uU][bB][gG][rR][aA][pP][hH] { posNo += YYLeng(); return SUBGRAPH; }
[nN][oO][dD][eE]                 { posNo += YYLeng(); return NODE; }
[eE][dD][gG][eE]                 { posNo += YYLeng(); return EDGE; }
","	  { posNo += YYLeng(); return COMMA; }
":"	  { posNo += YYLeng(); return COLON; }
";"	  { posNo += YYLeng(); return SEMICOLON; }
"="	  { posNo += YYLeng(); return IS; }
"{"	  { posNo += YYLeng(); return LBRACE; }
"}"	  { posNo += YYLeng(); return RBRACE; }
"["	  { posNo += YYLeng(); return LBRACK; }
"]"	  { posNo += YYLeng(); return RBRACK; }
"->"	  { posNo += YYLeng(); return ARROW; }
"--"	  { posNo += YYLeng(); return ARROW; }
{Name}	  { processId(); return ID; }
{Quoted}  { processQuoted(); return ID; }
{Number}  { processId(); return ID; }
.         { posNo += YYLeng(); dotyyerror("unknown character"); }

%%

void concrete_dot_lexer::processId()
{
  posNo += YYLeng();
  dotyylval.aterm = ATmakeAppl0( ATmakeAFun( YYText(), 0, ATtrue ) );
}

void concrete_dot_lexer::processQuoted()
{
  posNo += YYLeng();
  std::string value = static_cast<std::string>( YYText() );
  value = value.substr( 1, value.length() - 2 );
  dotyylval.aterm = ATmakeAppl0( ATmakeAFun( value.c_str(), 0, ATtrue ) );
}

//Implementation of parse_dot

bool parse_dot(std::istream &stream, lts &l) {
  clexer = new concrete_dot_lexer();
  dot_lexer_obj = clexer;
  bool result = clexer->parse_stream(stream,l);
  delete clexer;
  dot_lexer_obj = NULL;
  clexer = NULL;
  return result;
}


//Implementation of global functions

int dotyylex(void) {
  return clexer->yylex();
}

void dotyyerror(const char *s) {
  return clexer->yyerror(s);
}

int dotyyFlexLexer::yywrap(void) {
  return 1;
}


//Implementation of concrete_dot_lexer

concrete_dot_lexer::concrete_dot_lexer(void) : dotyyFlexLexer(NULL, NULL) {
}

void concrete_dot_lexer::yyerror(const char *s) {
  fprintf(
    stderr,
    "token '%s' at position %d,%d caused the following error: %s\n",
    YYText(), lineNo, posNo, s
  );
}

bool concrete_dot_lexer::parse_stream(std::istream &stream, lts &l)
{
  switch_streams(&stream, NULL);

  lineNo=1;
  posNo=1;

  // INITIALISE
  dot_lts = &l;

  protect_table = ATindexedSetCreate(10000,50);

  // PARSE
  bool result;
  if (dotyyparse() != 0) {
    result = false;
  } else {
    result = true;
  }

  // CLEAN UP
  ATindexedSetDestroy( protect_table );

  dot_lts = NULL;

  return result;
}
