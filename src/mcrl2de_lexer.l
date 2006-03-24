%{
#include <string.h>
#include "mcrl2de_lexer.h"
#include "mcrl2de_parser.hpp"
#include "libstruct.h"
#include "liblowlevel.h"

//Global precondition: the ATerm library has been initialised

//external declarations
int mcrl2deyyparse(void);          /* declared in mcrl2deparser.cpp */
extern YYSTYPE mcrl2deyylval;      /* declared in mcrl2deparser.cpp */
#ifndef _MSC_VER
extern int fileno(FILE *stream);   /* declared in stdio.h (forgotten by flex) */
#endif

//global declarations, used by mcrl2parser.cpp
int  mcrl2deyylex(void);           /* lexer function */
void mcrl2deyyerror(const char *s);/* error function */
ATermAppl DETree = NULL;           /* the parse tree */

//local declarations
int LineNr = 1;                  /* line number in source file */
int ColNr  = 1;                  /* column number in source file */
mcrl2deyyFlexLexer *lexer = NULL;  /* lexer object */

#define ProcessString() ColNr += YYLeng(); mcrl2deyylval.appl = gsString2ATermAppl(YYText());

%}
Id         [a-zA-Z\_][a-zA-Z0-9\_']*
Number     "0"|([1-9][0-9]*) 

%option c++
%option noyywrap
%option prefix="mcrl2deyy"
%option nounput

%%

[ \t]      { ColNr += YYLeng(); /* whitespace */ }

\r?\n      { LineNr++; ColNr = 1; /* newline */ }

"%".*      { ColNr += YYLeng(); /* comment */ }

"||_"      { ProcessString(); return LMERGE; }
"->"       { ProcessString(); return ARROW; }
"<="       { ProcessString(); return LTE; }
">="       { ProcessString(); return GTE; }
"|>"       { ProcessString(); return CONS; }
"<|"       { ProcessString(); return SNOC; }
"++"       { ProcessString(); return CONCAT; }
"=="       { ProcessString(); return EQ; }
"!="       { ProcessString(); return NEQ; }
"&&"       { ProcessString(); return AND; }
"||"       { ProcessString(); return BARS; }
"=>"       { ProcessString(); return IMP; }
"<<"       { ProcessString(); return BINIT; }
"<>"       { ProcessString(); return ELSE; }
"*"        { ProcessString(); return STAR; }
"+"        { ProcessString(); return PLUS; }
"-"        { ProcessString(); return MINUS; }
"="        { ProcessString(); return EQUALS; }
"."        { ProcessString(); return DOT; }
","        { ProcessString(); return COMMA; }
":"        { ProcessString(); return COLON; }
";"        { ProcessString(); return SEMICOLON; }
"?"        { ProcessString(); return QMARK; }
"!"        { ProcessString(); return EXCLAM; }
"@"        { ProcessString(); return AT; }
"#"        { ProcessString(); return HASH; }
"|"        { ProcessString(); return BAR; }
"("        { ProcessString(); return LPAR; }
")"        { ProcessString(); return RPAR; }
"[]"       { ProcessString(); return PBRACK; }
"["        { ProcessString(); return LBRACK; }
"]"        { ProcessString(); return RBRACK; }
"<"        { ProcessString(); return LANG; }
">"        { ProcessString(); return RANG; }
"{}"       { ProcessString(); return PBRACE; }
"{"        { ProcessString(); return LBRACE; }
"}"        { ProcessString(); return RBRACE; }

sort       { ProcessString(); return KWSORT; }
cons       { ProcessString(); return KWCONS; }
map        { ProcessString(); return KWMAP; }
var        { ProcessString(); return KWVAR; }
eqn        { ProcessString(); return KWEQN; }
act        { ProcessString(); return KWACT; }
proc       { ProcessString(); return KWPROC; }
init       { ProcessString(); return KWINIT; }
struct     { ProcessString(); return KWSTRUCT; }
Bool       { ProcessString(); return BOOL; }
Pos        { ProcessString(); return POS; }
Nat        { ProcessString(); return NAT; }
Int        { ProcessString(); return INT; }
List       { ProcessString(); return LIST; }
Set        { ProcessString(); return SET; }
Bag        { ProcessString(); return BAG; }
true       { ProcessString(); return TRUE; }
false      { ProcessString(); return FALSE; }
whr        { ProcessString(); return WHR; }
end        { ProcessString(); return END; }
lambda     { ProcessString(); return LAMBDA; }
forall     { ProcessString(); return FORALL; }
exists     { ProcessString(); return EXISTS; }
div        { ProcessString(); return DIV; }
mod        { ProcessString(); return MOD; }
in         { ProcessString(); return IN; }
delta      { ProcessString(); return DELTA; }
tau        { ProcessString(); return TAU; }
sum        { ProcessString(); return SUM; }
block      { ProcessString(); return BLOCK; }
allow      { ProcessString(); return ALLOW; }
hide       { ProcessString(); return HIDE; }
rename     { ProcessString(); return RENAME; }
comm       { ProcessString(); return COMM; }

{Id}       { ProcessString(); return ID; }

{Number}   { ProcessString(); return NUMBER; }

.          { 
             ColNr += YYLeng(); mcrl2deyyerror("unknown character");
             /* remaining characters */
	   }

%%

int mcrl2deyylex(void) {
  return lexer->yylex();
}

void mcrl2deyyerror(const char *s) {
  int oldColNr = ColNr - lexer->YYLeng();
  if (oldColNr < 0) {
    oldColNr = 0;
  }
  fprintf(
    stderr,
    "token '%s' at position %d, %d caused the following error: %s\n", 
    lexer->YYText(), LineNr, oldColNr, s
  ); 
}

ATermAppl mcrl2deParse(std::istream &DEStream) {
  ATermAppl Result = NULL;
  DETree = NULL;
  ATprotectAppl(&DETree);
  LineNr = 1;
  ColNr = 1;
  lexer = new mcrl2deyyFlexLexer(&DEStream);
  if (mcrl2deyyparse() != 0) {
    Result = NULL;
  } else {
    //DETree contains the parsed data expression
    Result = DETree;
    DETree = NULL;
  }
  ATunprotectAppl(&DETree);
  delete lexer;
  return Result;
}
