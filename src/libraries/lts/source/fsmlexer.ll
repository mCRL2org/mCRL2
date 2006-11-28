%{
#include <string>
#include "aterm2.h"
#include "lts/liblts.h"
#include "fsmlexer.h"
#include "fsmparser.hpp"
//using namespace std;

//fix for the broken cygwin versions of flex
#ifdef __CYGWIN__
#include <iostream>
using std::cerr;
using std::cin;
using std::cout;
#endif

using namespace mcrl2::lts;

//fix for newer versions of flex (>= 2.5.31)
#ifndef yywrap
#define yywrap fsmyywrap
#endif

//Global precondition: the ATerm library has been initialised

//external declarations
int fsmyyparse(void);          /* declared in fsmparser.cpp */
extern YYSTYPE fsmyylval;      /* declared in fsmparser.cpp */
#if !defined(_MSC_VER) && !defined(__MINGW32__)
extern int fileno(FILE *stream); /* declared in stdio.h (forgotten by flex) */
#endif

//global declarations, used by fsmparser.cpp
int  fsmyylex(void);           /* lexer function */
void fsmyyerror(const char *s);/* error function */
extern "C" int fsmyywrap(void);/* wrap function */
//Note: C linkage is needed for older versions of flex (2.5.4)

//local declarations
class concrete_fsm_lexer : public fsm_lexer, public fsmyyFlexLexer {
public:
  concrete_fsm_lexer(void);               /* constructor */
  int yylex(void);               /* the generated lexer function */
  void yyerror(const char *s);   /* error function */
  int yywrap(void);              /* wrap function */
  bool parse_stream(std::istream &stream, lts &l);

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
"->"	  { posNo += YYLeng(); return ARROW; }
"fan_in"  { posNo += YYLeng(); return FANIN; }
"fan_out" { posNo += YYLeng(); return FANOUT; }
"node_nr" { posNo += YYLeng(); return NODENR; }
{Id}	  { processId(); return ID; }
{Quoted}  { processQuoted(); return QUOTED; }
{Number}  { processNumber(); return NUMBER; }
.         { posNo += YYLeng(); fsmyyerror("unknown character"); }

%%

void concrete_fsm_lexer::processId()
{
  posNo += YYLeng();
  fsmyylval.aterm = ATmakeAppl0( ATmakeAFun( YYText(), 0, ATtrue ) ); 
}

void concrete_fsm_lexer::processQuoted()
{
  posNo += YYLeng();
  std::string value = static_cast<std::string>( YYText() );
  value = value.substr( 1, value.length() - 2 );
  fsmyylval.aterm = ATmakeAppl0( ATmakeAFun( value.c_str(), 0, ATtrue ) );
} 

void concrete_fsm_lexer::processNumber()
{
  posNo += YYLeng();
  fsmyylval.number = atoi( YYText() );
}

//Implementation of parse_fsm

bool parse_fsm(std::istream &stream, lts &l) {
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

int fsmyywrap(void) {
  return clexer->yywrap();
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

int concrete_fsm_lexer::yywrap(void) {
  return 1;
}

bool concrete_fsm_lexer::parse_stream(std::istream &stream, lts &l)
{
  switch_streams(&stream, NULL);

  lineNo=1;
  posNo=1;

  // INITIALISE
  fsm_lts = &l;
  
  const_ATtypeid = ATmakeAFun( "TypeId", 2, ATfalse );
  ATprotectAFun( const_ATtypeid );
  const_ATparmid = ATmakeAFun( "ParamId", 2, ATfalse );
  ATprotectAFun( const_ATparmid );
  const_ATvalue = ATmakeAFun( "Value", 2, ATfalse );
  ATprotectAFun( const_ATvalue );
  const_ATstate = ATmakeAFun( "State", 2, ATfalse );
  ATprotectAFun( const_ATstate );
  const_ATparam = ATmakeAFun( "Param", 2, ATfalse );
  ATprotectAFun( const_ATparam );
  stateVector = ATempty;
  ATprotectList( &stateVector );
  valueTable = ATempty;
  ATprotectList( &valueTable );
  stateId = ATempty;
  ATprotectList( &stateId );
  typeValues = NULL;
  ATprotectList( &typeValues );
  typeId = NULL;
  ATprotectAppl( &typeId );
  labelTable = ATtableCreate(100,50);
  

  // PARSE
  bool result;
  if (fsmyyparse() != 0) {
    result = false;
  } else {
    result = true;
  }
    
  // CLEAN UP
  ATunprotectAFun( const_ATtypeid );
  ATunprotectAFun( const_ATparmid );
  ATunprotectAFun( const_ATvalue );
  ATunprotectAFun( const_ATstate );
  ATunprotectAFun( const_ATparam );
  ATunprotectList( &stateVector );
  ATunprotectList( &valueTable );
  ATunprotectList( &stateId );
  ATunprotectList( &typeValues );
  ATunprotectAppl( &typeId );
  ATtableDestroy( labelTable );
  
  fsm_lts = NULL;

  return result;
}
