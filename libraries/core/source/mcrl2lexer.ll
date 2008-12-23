%{
// Author(s): Aad Mathijssen
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2lexer.ll

#include <cstring>
#include <cstdio>
#include "mcrl2/core/detail/mcrl2lexer.h"
#include "mcrl2/core/detail/mcrl2parser.hpp"
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/core/messaging.h"

//fix for the broken cygwin versions of flex
#ifdef __CYGWIN__
#include <iostream>
using std::cerr;
using std::cin;
using std::cout;
#endif

using namespace mcrl2::utilities;
using namespace mcrl2::core;

//Global precondition: the ATerm library has been initialised

//external declarations
int mcrl2yyparse(void);          /* declared in mcrl2parser.cpp */
extern YYSTYPE mcrl2yylval;      /* declared in mcrl2parser.cpp */
extern int mcrl2yydebug;         /* declared in mcrl2parser.cpp */

//global declarations, used by mcrl2parser.cpp
int  mcrl2yylex(void);           /* lexer function */
void mcrl2yyerror(const char *s);/* error function */
ATerm spec_tree = NULL;      /* the parse tree */
ATermIndexedSet parser_protect_table = NULL; /* table to protect parsed ATerms */

//local declarations
class mcrl2_lexer : public mcrl2yyFlexLexer {
public:
  mcrl2_lexer(bool print_parse_errors); /* constructor */
  int yylex(void);               /* the generated lexer function */
  void yyerror(const char *s);   /* error function */
  int yywrap(void);              /* wrap function */
  ATerm parse_streams(std::vector<std::istream*> &streams);
protected:
  std::vector<std::istream*> *cur_streams;/* current input streams */
  int cur_index;                 /* current index in current streams */
  int line_nr;                   /* line number in cur_streams[cur_index] */
  int col_nr;                    /* column number in cu_streams[cur_index] */
  bool show_errors;
  void process_string(void);     /* update position, provide token to parser */
};

//implement yylex in mcrl2_lexer instead of mcrl2yyFlexLexer
//(this gets rid of global variables but is ugly in its own right)
#define YY_DECL int mcrl2_lexer::yylex()
int mcrl2yyFlexLexer::yylex(void) { return 1; }

mcrl2_lexer *lexer = NULL;       /* lexer object, used by parse_streams */

%}
Id         [a-zA-Z\_][a-zA-Z0-9\_']*
Number     "0"|([1-9][0-9]*) 

%option c++
%option prefix="mcrl2yy"
%option nounput

%%

[ \t]      { col_nr += YYLeng(); /* whitespace */ }

\r?\n      { line_nr++; col_nr = 1; /* newline */ }

"%".*      { col_nr += YYLeng(); /* comment */ }

"identifier"    { process_string(); return TAG_IDENTIFIER; }
"sort_expr"     { process_string(); return TAG_SORT_EXPR; }
"data_expr"     { process_string(); return TAG_DATA_EXPR; }
"data_spec"     { process_string(); return TAG_DATA_SPEC; }
"mult_act"      { process_string(); return TAG_MULT_ACT; }
"proc_expr"     { process_string(); return TAG_PROC_EXPR; }
"proc_spec"     { process_string(); return TAG_PROC_SPEC; }
"state_frm"     { process_string(); return TAG_STATE_FRM; }
"action_rename" { process_string(); return TAG_ACTION_RENAME; }
"pbes_spec"     { process_string(); return TAG_PBES_SPEC; }
"data_vars"     { process_string(); return TAG_DATA_VARS; }

"||_"      { process_string(); return LMERGE; }
"->"       { process_string(); return ARROW; }
"<="       { process_string(); return LTE; }
">="       { process_string(); return GTE; }
"|>"       { process_string(); return CONS; }
"<|"       { process_string(); return SNOC; }
"++"       { process_string(); return CONCAT; }
"=="       { process_string(); return EQ; }
"!="       { process_string(); return NEQ; }
"&&"       { process_string(); return AND; }
"||"       { process_string(); return BARS; }
"=>"       { process_string(); return IMP; }
"<<"       { process_string(); return BINIT; }
"<>"       { process_string(); return ELSE; }
"/"        { process_string(); return SLASH; }
"*"        { process_string(); return STAR; }
"+"        { process_string(); return PLUS; }
"-"        { process_string(); return MINUS; }
"="        { process_string(); return EQUALS; }
"."        { process_string(); return DOT; }
","        { process_string(); return COMMA; }
":"        { process_string(); return COLON; }
";"        { process_string(); return SEMICOLON; }
"?"        { process_string(); return QMARK; }
"!"        { process_string(); return EXCLAM; }
"@"        { process_string(); return AT; }
"#"        { process_string(); return HASH; }
"|"        { process_string(); return BAR; }
"("        { process_string(); return LPAR; }
")"        { process_string(); return RPAR; }
"[]"       { process_string(); return PBRACK; }
"["        { process_string(); return LBRACK; }
"]"        { process_string(); return RBRACK; }
"<"        { process_string(); return LANG; }
">"        { process_string(); return RANG; }
"{}"       { process_string(); return PBRACE; }
"{"        { process_string(); return LBRACE; }
"}"        { process_string(); return RBRACE; }

sort       { process_string(); return KWSORT; }
cons       { process_string(); return KWCONS; }
map        { process_string(); return KWMAP; }
var        { process_string(); return KWVAR; }
eqn        { process_string(); return KWEQN; }
act        { process_string(); return KWACT; }
proc       { process_string(); return KWPROC; }
pbes       { process_string(); return KWPBES; }
init       { process_string(); return KWINIT; }

struct     { process_string(); return KWSTRUCT; }
Bool       { process_string(); return BOOL; }
Pos        { process_string(); return POS; }
Nat        { process_string(); return NAT; }
Int        { process_string(); return INT; }
Real       { process_string(); return REAL; }
List       { process_string(); return LIST; }
Set        { process_string(); return SET; }
Bag        { process_string(); return BAG; }

true       { process_string(); return CTRUE; }
false      { process_string(); return CFALSE; }
div        { process_string(); return DIV; }
mod        { process_string(); return MOD; }
in         { process_string(); return IN; }
lambda     { process_string(); return LAMBDA; }
forall     { process_string(); return FORALL; }
exists     { process_string(); return EXISTS; }
whr        { process_string(); return WHR; }
end        { process_string(); return END; }

delta      { process_string(); return DELTA; }
tau        { process_string(); return TAU; }
sum        { process_string(); return SUM; }
block      { process_string(); return BLOCK; }
allow      { process_string(); return ALLOW; }
hide       { process_string(); return HIDE; }
rename     { process_string(); return RENAME; }
comm       { process_string(); return COMM; }

val        { process_string(); return VAL; }
mu         { process_string(); return MU; }
nu         { process_string(); return NU; }
delay      { process_string(); return DELAY; }
yaled      { process_string(); return YALED; }
nil        { process_string(); return NIL; }

{Id}       { process_string(); return ID; }

{Number}   { process_string(); return NUMBER; }

.          { 
             col_nr += YYLeng(); yyerror("unknown character");
             /* remaining characters */
	   }

%%

//Implementation of parse_streams

ATerm parse_streams(std::vector<std::istream*> &streams, bool print_parse_errors) {
  lexer = new mcrl2_lexer(print_parse_errors);
  ATerm result = lexer->parse_streams(streams);
  delete lexer;
  return result;
}


//Implementation of global functions

int mcrl2yylex(void) {
  return lexer->yylex();
}

void mcrl2yyerror(const char *s) {
  return lexer->yyerror(s);
}

int mcrl2yyFlexLexer::yywrap(void) {
  return 1;
}


//Implementation of mcrl2_lexer

mcrl2_lexer::mcrl2_lexer(bool print_parse_errors) : mcrl2yyFlexLexer(NULL, NULL) {
  line_nr = 1;
  col_nr = 1;
  cur_streams = NULL;
  cur_index = -1;
  show_errors = print_parse_errors;
}

void mcrl2_lexer::yyerror(const char *s) {
  if ( show_errors )
  {
    int oldcol_nr = col_nr - YYLeng();
    if (oldcol_nr < 0) {
      oldcol_nr = 0;
    }
    gsErrorMsg(
      "token '%s' at position %d, %d caused the following error: %s\n", 
      YYText(), line_nr, oldcol_nr, s
    ); 
  }
}

int mcrl2_lexer::yywrap(void) {
  if (cur_streams == NULL) {
    return 1;
  }
  //SpecStreams != NULL
  if (!(cur_index >= 0 && cur_index < (int) cur_streams->size()-1)) {
    return 1;
  }
  //0 <= cur_index < cur_streams->size()-1
  line_nr = 1;
  col_nr = 1;
  cur_index++;
  switch_streams((*cur_streams)[cur_index], NULL);
  return 0;
}

void mcrl2_lexer::process_string(void) {
  col_nr += YYLeng();
  mcrl2yylval.appl = gsString2ATermAppl(YYText());
}

ATerm mcrl2_lexer::parse_streams(std::vector<std::istream*> &streams) {
  //uncomment the line below to let bison generate debug information 
  //mcrl2yydebug = 1;
  ATerm result = NULL;
  if (streams.size() == 0) {
    return result;
  }
  //streams.size() > 0
  spec_tree = NULL;
  ATprotect(&spec_tree);
  parser_protect_table = ATindexedSetCreate(10000, 50);
  line_nr = 1;
  col_nr = 1;
  cur_index = 0;
  cur_streams = &streams;
  switch_streams((*cur_streams)[0], NULL);
  if (mcrl2yyparse() != 0) {
    result = NULL;
  } else {
    //spec_tree contains the parsed specification
    result = spec_tree;
    spec_tree = NULL;
  }
  ATindexedSetDestroy(parser_protect_table);
  ATunprotect(&spec_tree);
  return result;
}
