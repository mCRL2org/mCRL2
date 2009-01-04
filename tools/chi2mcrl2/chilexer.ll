%{

#include <cstring>
#include <cmath>
#include <iostream>
#include <vector>
#include "libstruct_core.h"
#include "mcrl2/core/messaging.h"
#include "chiparser.hpp"
#include <map>
#include <set>
#include <utility>

using namespace mcrl2::core;
using namespace std;

int line = 1, col = 1;
int scope_lvl;

map<ATerm, ATerm> var_type_map;
set<ATermAppl> used_process_identifiers;
map<ATerm, pair<ATerm,ATerm> > chan_type_direction_map;
int parsing_mode;

extern ATermAppl spec_tree;
extern ATermIndexedSet parser_protect_table;


/**
 * yyerror() is invoked when the lexer or the parser encounter an error.
 *
 */
int yyerror(const char *s)
{
  gsErrorMsg("%s at line: %d col: %d\n",s,line,col);
  return 0;
}

//Global precondition: the ATerm library has been initialised

//external declarations
int chiyyparse(void);          /* declared in parser.cpp */
extern YYSTYPE chiyylval;      /* declared in parser.cpp */

//global declarations, used by chiparser.cpp
int  chiyylex(void);           /* lexer function */
void chiyyerror(const char *s);/* error function */
void chigetposition();
ATermAppl spec_tree = NULL;      /* the parse tree */
ATermIndexedSet parser_protect_table = NULL; /* table to protect parsed ATerms */

//local declarations
class chiLexer : public chiyyFlexLexer {
public:
  chiLexer(void);                /* constructor */
  int yylex(void);               /* the generated lexer function */
  void yyerror(const char *s);   /* error function */
  ATermAppl parse_stream(std::istream &stream );
  void getposition();
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
"//".*      { col_nr += YYLeng(); /* comment */ }

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
"="     { process_string(); return IS; }
":="    { process_string(); return ASSIGNMENT; }
"-"     { process_string(); return MINUS; }
"+"		{ process_string(); return PLUS; }
"/="	{ process_string(); return NOTEQUAL; }
"<="	{ process_string(); return LEQ; }
">="	{ process_string(); return GEQ; }
">"     { process_string(); return GREATER; }
"<"     { process_string(); return LESS; }
"&&"    { process_string(); return AND; }
"and"	{ process_string(); return AND; }
"or"	{ process_string(); return OR; }
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
"/"     { process_string(); return DIVIDE; } 
"["     { process_string(); return SQLBRACKET; }
"]"		{ process_string(); return SQRBRACKET; }
"#"		{ process_string(); return HASH; }
 
"proc"    { process_string(); return PROC; }
"model"   { process_string(); return MODEL; }
"var"		{ process_string(); return VAR; }
"enum"    { process_string(); return ENUM; }
"skip"	{ process_string(); return SKIP; }
"true"	{ process_string(); return TRUE; }
"false"	{ process_string(); return FALSE; }
"chan"	{ process_string(); return CHAN; }
"min"		{ process_string(); return MIN; }
"max"		{ process_string(); return MAX; }

"const"   { process_string(); return CONST; }
"time"	{ process_string(); return TIME; }
"delay"	{ process_string(); return DELAY; }

"in"	{ process_string(); return IN; }
"++"	{ process_string(); return CONCAT; }
"--"    { process_string(); return LSUBTRACT; }

"len"   { process_string(); return LENGTH; }
"hd"    { process_string(); return HEAD; }
"tl"    { process_string(); return TAIL; }
"hr"    { process_string(); return RHEAD; }
"tr"    { process_string(); return RTAIL; }
"take"  { process_string(); return TAKE; }
"drop"  { process_string(); return DROP; }
"sort"  { process_string(); return SORT; }
"insert" { process_string(); return INSERT; }

"pick"  { process_string(); return PICK;}
"/\\"	{ process_string(); return INTERSECTION; }
"\\/"	{ process_string(); return UNION; }
"sub"   { process_string(); return SUB; }
 
"bool"	{ process_string(); return BOOL; }
"nat"    { process_string(); return NAT; }
"void"  { process_string(); return VOID; }
"string" { process_string(); return TYPE; }

"deadlock" { process_string();return DEADLOCK; }
"delta" 	{ process_string();return DEADLOCK; }
"old"		{ process_string(); return OLD;}

"val"		{ process_string(); yyerror("The \"val\" operator is not supported\n");}
"real"	{ process_string(); yyerror("The \"real\" operator is not supported\n");}
	

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

void chigetposition() {
  return lexer->getposition();
}

int chiyyFlexLexer::yywrap(void) {
  return 1;
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
  gsErrorMsg(
    "token '%s' at position %d, %d caused the following error: %s\n", 
    YYText(), line_nr, oldcol_nr, s
  ); 
}

void chiLexer::getposition()
{
  int oldcol_nr = col_nr - YYLeng();
  if (oldcol_nr < 0) {
    oldcol_nr = 0;
  }
  gsErrorMsg(
    "Near position Line: %d, Column: %d:\n", 
    line_nr, oldcol_nr 
  ); 
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
