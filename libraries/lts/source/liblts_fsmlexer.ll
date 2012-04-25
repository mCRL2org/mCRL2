%{
// Author(s): Muck van Weerdenburg
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file fsmlexer.ll

#define YYSTYPE std::string
#include <string>
#include <cstdio>
// #include <aterm2.h>
#include "mcrl2/utilities/logger.h"
#include "mcrl2/lts/lts.h"
#include "liblts_fsmlexer.h"
#include "liblts_fsmparser.h"
#include "mcrl2/lts/detail/liblts_fsmparser.h"

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
int fsmyyparse(void);          /* declared in fsmparser.cpp */
extern YYSTYPE fsmyylval;      /* declared in fsmparser.cpp */

//global declarations, used by fsmparser.cpp
int  fsmyylex(void);           /* lexer function */
void fsmyyerror(const char *s);/* error function */

//local declarations
class concrete_fsm_lexer : public fsm_lexer, public fsmyyFlexLexer {
public:
  concrete_fsm_lexer(void);               /* constructor */
  int yylex(void);               /* the generated lexer function */
  void yyerror(const char *s);   /* error function */
  bool parse_stream(std::istream &stream, lts_fsm_t &l);

protected:
  void processId();
  void processQuoted();
  void processNumber();
};

//implement yylex in concrete_fsm_lexer instead of fsmyyFlexLexer
//(this gets rid of global variables but is ugly in its own right)
#define YY_DECL int concrete_fsm_lexer::yylex()
int fsmyyFlexLexer::yylex(void) { return 1; }

concrete_fsm_lexer *clexer = NULL;  /* lexer object, used by parse_stream */
fsm_lexer *fsm_lexer_obj = NULL;    /* lexer object, used by fsmparser */

int lineNo=1, posNo=1;
extern void fsmyyerror(const char* s);
void processId();
void processQuoted();
void processNumber();
%}
%option c++
%option prefix="fsmyy"
%option nounput
Quoted	   \"[^\"]*\"
Id	   [a-zA-Z_][a-zA-Z0-9_'@]*
Number     [0]|([1-9][0-9]*)

%%

[ \t]	  { posNo += YYLeng(); }
\r?\n     { lineNo++; posNo=1; return EOLN; }
"---"	  { posNo += YYLeng(); return SECSEP; }
"("	  { posNo += YYLeng(); return LPAR; }
")"	  { posNo += YYLeng(); return RPAR; }
"|"	  { posNo += YYLeng(); return BAR; }
"struct"	  { posNo += YYLeng(); return KWSTRUCT; }
"Set"	  { posNo += YYLeng(); return SET; }
"List"	  { posNo += YYLeng(); return LIST; }
"Real"	  { posNo += YYLeng(); return REAL; }
"#"	  { posNo += YYLeng(); return HASH; }
"?"	  { posNo += YYLeng(); return QMARK; }
":"	  { posNo += YYLeng(); return COLON; }
","	  { posNo += YYLeng(); return COMMA; }
"Int"	  { posNo += YYLeng(); return INT; }
"Nat"	  { posNo += YYLeng(); return NAT; }
"Pos"	  { posNo += YYLeng(); return POS; }
"Bag"	  { posNo += YYLeng(); return BAG; }
"Bool"	  { posNo += YYLeng(); return BOOL; }
"->"	  { posNo += YYLeng(); return ARROW; }
{Id}	  { processId(); return ID; }
{Quoted}  { processQuoted(); return QUOTED; }
{Number}  { processNumber(); return NUMBER; }
.         { posNo += YYLeng(); fsmyyerror("unknown character"); }

%%

void concrete_fsm_lexer::processId()
{
  posNo += YYLeng();
  // fsmyylval.result_string = ATmakeAppl0( AFun( YYText(), 0, ATtrue ) );
  // fsmyylval.result_string = YYText();
  fsmyylval = static_cast<std::string>(YYText());
}

void concrete_fsm_lexer::processQuoted()
{
  posNo += YYLeng();
  std::string value = static_cast<std::string>( YYText() );
  value = value.substr( 1, value.length() - 2 );
  fsmyylval = value;
  // fsmyylval.result_string = value;
  // fsmyylval.result_string = ATmakeAppl0( AFun( value.c_str(), 0, ATtrue ) );
}

void concrete_fsm_lexer::processNumber()
{
  posNo += YYLeng();
  // fsmyylval.number = atoi( YYText() );
  fsmyylval=static_cast<std::string>(YYText());
}

//Implementation of parse_fsm

bool parse_fsm(std::istream &stream, lts_fsm_t &l) 
{
  clexer = new concrete_fsm_lexer();
  fsm_lexer_obj = clexer;
  bool result = clexer->parse_stream(stream,l);
  delete clexer;
  fsm_lexer_obj = NULL;
  clexer = NULL;
  return result;
}


//Implementation of global functions

int fsmyylex(void) {
  return clexer->yylex();
}

void fsmyyerror(const char *s) {
  return clexer->yyerror(s);
}

int fsmyyFlexLexer::yywrap(void) {
  return 1;
}


//Implementation of concrete_fsm_lexer

concrete_fsm_lexer::concrete_fsm_lexer(void) : fsmyyFlexLexer(NULL, NULL) {
}

void concrete_fsm_lexer::yyerror(const char *s) {
  fprintf(
    stderr,
    "token '%s' at position %d,%d caused the following error: %s\n",
    YYText(), lineNo, posNo, s
  );
}

bool concrete_fsm_lexer::parse_stream(std::istream &stream, lts_fsm_t &l)
{
  switch_streams(&stream, NULL);

  lineNo=1;
  posNo=1;

  // INITIALISE
  fsm_lts = &l;

  /* protect_table = ATindexedSetCreate(10000,50);

  const_ATtype = AFun( "Type", 2, ATfalse );
  const_ATvalue = AFun( "Value", 2, ATfalse );
  // stateVector = ATempty;
  valueTable = ATempty;
  stateId = ATempty;
  typeValues = NULL;
  typeId = NULL;
  labelTable = ATtableCreate(100,50); */


  // PARSE
  bool result;
  if (fsmyyparse() != 0) 
  {
    result = false;
  } 
  else 
  {
    result = true;
  }

  // CLEAN UP
  /* ATtableDestroy( labelTable );

  ATindexedSetDestroy( protect_table ); */

  fsm_lts = NULL;

  return result;
}
