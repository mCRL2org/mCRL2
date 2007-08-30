%{

#include "chiparser.hpp"
#include <string.h>
#include <math.h>
#include <iostream>
#include "libstruct.h"
#include <vector>
#include "print/messaging.h"

//fix for newer versions of flex (>= 2.5.31)
#ifndef yywrap
#define yywrap chiyywrap
#endif

#ifdef __cplusplus
using namespace ::mcrl2::utilities;
#endif

int line = 1, col = 1;

extern ATermAppl spec_tree;
extern ATermIndexedSet parser_protect_table;

/**
 * yyerror() is invoked when the lexer or the parser encounter an error.
 *
 */
int yyerror(const char *s)
{
  printf("error: %s at line: %d col: %d\n",s,line,col);
  return 0;
}

//Global precondition: the ATerm library has been initialised

//external declarations
int chiyyparse(void);          /* declared in parser.cpp */
extern YYSTYPE chiyylval;      /* declared in parser.cpp */

//global declarations, used by chiparser.cpp
int  chiyylex(void);           /* lexer function */
void chiyyerror(const char *s);/* error function */
extern "C" int chiyywrap(void);/* wrap function */
//Note: C linkage is needed for older versions of flex (2.5.4)
ATermAppl spec_tree = NULL;      /* the parse tree */
ATermIndexedSet parser_protect_table = NULL; /* table to protect parsed ATerms */

//local declarations
class chiLexer : public chiyyFlexLexer {
public:
  chiLexer(void);                /* constructor */
  int yylex(void);               /* the generated lexer function */
  void yyerror(const char *s);   /* error function */
  int yywrap(void);              /* wrap function */
  ATermAppl parse_stream(std::istream &stream );
protected:
  std::istream *cur_stream;      /* input stream */
  int line_nr;                   /* line number in cur_streams[cur_index] */
  int col_nr;                    /* column number in cu_streams[cur_index] */
  void process_string(void);     /* update position, provide token to parser */
};

//implement yylex in chiLexer instead of chiyyFlexLexer
#define YY_DECL int chiLexer::yylex()
int chiyyFlexLexer::yylex(void) { return 1; }

chiLexer *lexer = NULL;       /* lexer object, used by parse_streams */


%}
digit       [0-9]
Number      "0"|([1-9]{digit}*)
RealNumber  {Number}\.({digit}*) 
exponent    [eE][+-]?{Number}
letter      [a-zA-Z]
identifier  {letter}[a-zA-Z0-9\_']*

%option c++
%option prefix="chiyy"
%option nounput

%%
[ \t]      { col_nr += YYLeng(); /* whitespace */ }
\r?\n      { col_nr = 1; ++line_nr; /* newline */ }
"%".*      { col_nr += YYLeng(); /* comment */ }

"|["    { process_string(); return BP; }
"]|"    { process_string(); return EP; }
"||"    { process_string(); return BARS; }
"|"		{ process_string(); return ALT; }
";"		{ process_string(); return SEP; }
"*"		{ process_string(); return STAR; }
"*>"	{ process_string(); return GUARD_REP; }

">>"	{ process_string(); return GG; }
"::"    { process_string(); return PROC_SEP; }
","     { process_string(); return COMMA; }
":"     { process_string(); return COLON; }
"="     { process_string(); return DEFINES; }
":="    { process_string(); return ASSIGNMENT; }
"-"     { process_string(); return MINUS; }
"+"		{ process_string(); return PLUS; }
"=="	{ process_string(); return EQUAL; }
"/="	{ process_string(); return NOTEQUAL; }
"<="	{ process_string(); return LEQ; }
">="	{ process_string(); return GEQ; }
"&&"    { process_string(); return AND; }
"and"	{ process_string(); return AND; }
"/\\"	{ process_string(); return AND; }
"or"	{ process_string(); return OR; }
"\\/"	{ process_string(); return OR; }
"->"	{ process_string(); return GUARD; }
"=>"	{ process_string(); return IMPLIES; }
"not"	{ process_string(); return NOT;}
"!"		{ process_string(); return EXCLAMATION;}
"{"     { process_string(); return LBRACE; }
"}"     { process_string(); return RBRACE; }
"("     { process_string(); return LBRACKET; }
")"		{ process_string(); return RBRACKET; }
"."		{ process_string(); return DOT; }
"?"		{ process_string(); return RECV; }
"?!"	{ process_string(); return RECVSEND; }
"!?"	{ process_string(); return SENDRECV; }
"!!"	{ process_string(); return SSEND; }
"??"	{ process_string(); return RRECV; }
"'"		{ process_string(); return DERIVATIVE; }
"^"		{ process_string(); return POWER; }
"mod"	{ process_string(); return MOD; }
"div"	{ process_string(); return DIV; }
 
proc    { process_string(); return PROC; }
var		{ process_string(); return VAR; }
enum    { process_string(); return ENUM; }
skip	{ process_string(); return SKIP; }
true	{ process_string(); return TRUE; }
false	{ process_string(); return FALSE; }
chan	{ process_string(); return CHAN; }
min		{ process_string(); return MIN; }
max		{ process_string(); return MAX; }

const   { process_string(); return CONST; }
time	{ process_string(); return TIME; }


bool	{ process_string(); return TYPE; }
nat     { process_string(); return TYPE; }
int		{ process_string(); return TYPE; }
real	{ process_string(); return TYPE; }
string  { process_string(); return TYPE; }

deadlock { process_string();return DEADLOCK; }
old		{ process_string(); return OLD;}

{identifier}    { process_string(); return ID; }
{Number} { process_string(); return NUMBER; }
{RealNumber} { process_string(); return REALNUMBER; }
.       { col_nr += YYLeng(); yyerror("unknown character"); }


%%

//Implementation of parse_stream 

ATermAppl parse_stream ( std::istream &stream ) {
  lexer = new chiLexer();
  ATermAppl result = lexer->parse_stream(stream);
  delete lexer;
  return result;
}

//Implementation of the global functions

int chiyylex(void) {
  return lexer->yylex();
}

void chiyyerror(const char *s) {
  return lexer->yyerror(s);
}

int chiyywrap(void) {
  return lexer->yywrap();
}


//Implementation of chiLexer

chiLexer::chiLexer(void) : chiyyFlexLexer(NULL, NULL) {
  line_nr = 1;
  col_nr = 1;
}

void chiLexer::yyerror(const char *s) {
  int oldcol_nr = col_nr - YYLeng();
  if (oldcol_nr < 0) {
    oldcol_nr = 0;
  }
  printf(
    "token '%s' at position %d, %d caused the following error: %s\n", 
    YYText(), line_nr, oldcol_nr, s
  ); 
}

int chiLexer::yywrap(void) {
/** 
  * When the scanner receives an end-of-file indication from YY_INPUT, it  
  * checks the `yywrap()' function. Because we only have one input stream,
  * we need to terminate the parser, by returning the 1.
  *
  **/
  return 1;
}

void chiLexer::process_string(void) {
  col_nr += YYLeng();
  chiyylval.appl = gsString2ATermAppl(YYText());
}

ATermAppl chiLexer::parse_stream (std::istream &stream ) {
/**
  * Pre: stream is opened for reading
  * Post:the content of tag followed by stream is parsed
  * Ret: the parsed content, if everything went ok
  *      NULL, otherwise
  *
  **/ 
  
  ATermAppl result = NULL;
  spec_tree = NULL;
  ATprotectAppl(&spec_tree);
  parser_protect_table = ATindexedSetCreate(10000, 50);
  line_nr = 1;
  col_nr = 1;
  cur_stream  = &stream ;
  switch_streams(cur_stream , NULL);
  if (chiyyparse() != 0) {
    result = NULL;
  } else {
    //spec_tree contains the parsed specification
    result = spec_tree;
    spec_tree = NULL;
  }
  
  ATindexedSetDestroy(parser_protect_table);
  ATunprotectAppl(&spec_tree);
  
  return result;
}
