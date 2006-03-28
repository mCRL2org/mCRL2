%{
#include <cstring>
#include "mcrl2lexer.h"
#include "mcrl2parser.hpp"
#include "libstruct.h"
#include "liblowlevel.h"

//fix for the broken cygwin versions of flex
#ifdef __CYGWIN__
#include <iostream>
#endif

//fix for newer versions of flex (>= 2.5.31)
#ifndef yywrap
#define yywrap mcrl2yywrap
#endif

//Global precondition: the ATerm library has been initialised

//external declarations
int mcrl2yyparse(void);          /* declared in mcrl2parser.cpp */
extern YYSTYPE mcrl2yylval;      /* declared in mcrl2parser.cpp */
#ifndef _MSC_VER
extern int fileno(FILE *stream); /* declared in stdio.h (forgotten by flex) */
#endif

//global declarations, used by mcrl2parser.cpp
int  mcrl2yylex(void);           /* lexer function */
void mcrl2yyerror(const char *s);/* error function */
extern "C" int mcrl2yywrap(void);/* wrap function */
//Note: C linkage is needed for older versions of flex (2.5.4)
ATermAppl spec_tree = NULL;      /* the parse tree */

//local declarations
class mcrl2_lexer : public mcrl2yyFlexLexer {
public:
  mcrl2_lexer(void);             /* constructor */
  int yylex(void);               /* the generated lexer function */
  void yyerror(const char *s);   /* error function */
  int yywrap(void);              /* wrap function */
  ATermAppl parse_streams(std::vector<std::istream*> &streams);
protected:
  std::vector<std::istream*> *cur_streams;/* current input streams */
  int cur_index;                 /* current index in current streams */
  int line_nr;                   /* line number in cur_streams[cur_index] */
  int col_nr;                    /* column number in cu_streams[cur_index] */
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

"spec" { process_string(); return TAG_SPEC; }
"sort_expr" { process_string(); return TAG_SORT_EXPR; }
"data_expr" { process_string(); return TAG_DATA_EXPR; }
"proc_expr" { process_string(); return TAG_PROC_EXPR; }

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
init       { process_string(); return KWINIT; }
struct     { process_string(); return KWSTRUCT; }
Bool       { process_string(); return BOOL; }
Pos        { process_string(); return POS; }
Nat        { process_string(); return NAT; }
Int        { process_string(); return INT; }
List       { process_string(); return LIST; }
Set        { process_string(); return SET; }
Bag        { process_string(); return BAG; }
true       { process_string(); return CTRUE; }
false      { process_string(); return CFALSE; }
whr        { process_string(); return WHR; }
end        { process_string(); return END; }
lambda     { process_string(); return LAMBDA; }
forall     { process_string(); return FORALL; }
exists     { process_string(); return EXISTS; }
div        { process_string(); return DIV; }
mod        { process_string(); return MOD; }
in         { process_string(); return IN; }
delta      { process_string(); return DELTA; }
tau        { process_string(); return TAU; }
sum        { process_string(); return SUM; }
block      { process_string(); return BLOCK; }
allow      { process_string(); return ALLOW; }
hide       { process_string(); return HIDE; }
rename     { process_string(); return RENAME; }
comm       { process_string(); return COMM; }

{Id}       { process_string(); return ID; }

{Number}   { process_string(); return NUMBER; }

.          { 
             col_nr += YYLeng(); yyerror("unknown character");
             /* remaining characters */
	   }

%%

//Implementation of parse_streams

ATermAppl parse_streams(std::vector<std::istream*> &streams) {
  lexer = new mcrl2_lexer();
  ATermAppl result = lexer->parse_streams(streams);
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

int mcrl2yywrap(void) {
  return lexer->yywrap();
}


//Implementation of mcrl2_lexer

mcrl2_lexer::mcrl2_lexer(void) : mcrl2yyFlexLexer(NULL, NULL) {
  line_nr = 1;
  col_nr = 1;
  cur_streams = NULL;
  cur_index = -1;
}

void mcrl2_lexer::yyerror(const char *s) {
  int oldcol_nr = col_nr - YYLeng();
  if (oldcol_nr < 0) {
    oldcol_nr = 0;
  }
  fprintf(
    stderr,
    "token '%s' at position %d, %d caused the following error: %s\n", 
    YYText(), line_nr, oldcol_nr, s
  ); 
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

ATermAppl mcrl2_lexer::parse_streams(std::vector<std::istream*> &streams) {
  ATermAppl result = NULL;
  if (streams.size() == 0) {
    return result;
  }
  //streams.size() > 0
  spec_tree = NULL;
  ATprotectAppl(&spec_tree);
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
  ATunprotectAppl(&spec_tree);
  return result;
}
