%{
#include <string.h>
#include "gslexer.h"
#include "gsparser.h"
#include "gsfunc.h"

//Global precondition: the ATerm library has been initialised

//global declarations
ATermAppl GStree = NULL;         /* the parse tree */

//external declarations
int GSyyparse(void);             /* declared in gsparser.c */
extern YYSTYPE GSyylval;         /* declared in gsparser.c */
extern int fileno(FILE *stream); /* declared in stdio.h (forgotten by lex) */

//local declarations
int lineno = 1;                  /* line number in source file */
int colno  = 1;                  /* column number in source file */
void GSyyerror(char *s);         /* display error message (needed by yacc) */
int GSyywrap(void);              /* action taken after an EOF (needed by yacc) */

%}
id       [a-zA-Z\_][a-zA-Z0-9\_]*
number   "0"|("-"?[1-9][0-9]*) 

%%

[ \t]    { colno += GSyyleng; /* whitespace */ }

\r?\n    { lineno++; colno = 1; /* newline */ }

"%".*    { colno += MCyyleng; /* comment */ }

"||_"    { colno += GSyyleng; return LMERGE; }
"->"     { colno += GSyyleng; return ARROW; }
"<="     { colno += GSyyleng; return LTE; }
">="     { colno += GSyyleng; return GTE; }
"|>"     { colno += GSyyleng; return CONS; }
"<|"     { colno += GSyyleng; return SNOC; }
"++"     { colno += GSyyleng; return CONCAT; }
"=="     { colno += GSyyleng; return EQ; }
"!="     { colno += GSyyleng; return NEQ; }
"&&"     { colno += GSyyleng; return AND; }
"||"     { colno += GSyyleng; return BARS; }
"=>"     { colno += GSyyleng; return IMP; }
"<<"     { colno += GSyyleng; return BINIT; }
"*"      { colno += GSyyleng; return STAR; }
"+"      { colno += GSyyleng; return PLUS; }
"-"      { colno += GSyyleng; return MINUS; }
"="      { colno += GSyyleng; return EQUALS; }
"."      { colno += GSyyleng; return DOT; }
","      { colno += GSyyleng; return COMMA; }
":"      { colno += GSyyleng; return COLON; }
";"      { colno += GSyyleng; return SEMICOLON; }
"?"      { colno += GSyyleng; return QMARK; }
"!"      { colno += GSyyleng; return EXCLAM; }
"@"      { colno += GSyyleng; return AT; }
"#"      { colno += GSyyleng; return HASH; }
"|"      { colno += GSyyleng; return BAR; }
"("      { colno += GSyyleng; return LPAR; }
")"      { colno += GSyyleng; return RPAR; }
"[]"     { colno += GSyyleng; return PBRACK; }
"["      { colno += GSyyleng; return LBRACK; }
"]"      { colno += GSyyleng; return RBRACK; }
"<"      { colno += GSyyleng; return LANG; }
">"      { colno += GSyyleng; return RANG; }
"{}"     { colno += GSyyleng; return PBRACE; }
"{"      { colno += GSyyleng; return LBRACE; }
"}"      { colno += GSyyleng; return RBRACE; }

sort     { colno += GSyyleng; return SORT; }
cons     { colno += GSyyleng; return CONS; }
map      { colno += GSyyleng; return MAP; }
var      { colno += GSyyleng; return VAR; }
eqn      { colno += GSyyleng; return EQN; }
act      { colno += GSyyleng; return ACT; }
proc     { colno += GSyyleng; return PROC; }
init     { colno += GSyyleng; return INIT; }
Bool     { colno += GSyyleng; return BOOL; }
Pos      { colno += GSyyleng; return POS; }
Nat      { colno += GSyyleng; return NAT; }
Int      { colno += GSyyleng; return INT; }
List     { colno += GSyyleng; return LIST; }
Set      { colno += GSyyleng; return SET; }
Bag      { colno += GSyyleng; return BAG; }
Struct   { colno += GSyyleng; return STRUCT; }
true     { colno += GSyyleng; return TRUE; }
false    { colno += GSyyleng; return FALSE; }
whr      { colno += GSyyleng; return WHR; }
end      { colno += GSyyleng; return END; }
lambda   { colno += GSyyleng; return LAMBDA; }
forall   { colno += GSyyleng; return FORALL; }
exists   { colno += GSyyleng; return EXISTS; }
div      { colno += GSyyleng; return DIV; }
mod      { colno += GSyyleng; return MOD; }
in       { colno += GSyyleng; return IN; }
delta    { colno += GSyyleng; return DELTA; }
tau      { colno += GSyyleng; return TAU; }
sum      { colno += GSyyleng; return SUM; }
restrict { colno += GSyyleng; return RESTRICT; }
allow    { colno += GSyyleng; return ALLOW; }
hide     { colno += GSyyleng; return HIDE; }
rename   { colno += GSyyleng; return RENAME; }
comm     { colno += GSyyleng; return COMM; }

{id}     { colno += GSyyleng; return ID; /* identifier (not a keyword) */ }

{number} { colno += GSyyleng; return NUMBER; /* number */ }

.        { colno += GSyyleng; GSyyerror("unknown character"); /* remaining characters */ }

%%

void GSyyerror(char *s) {
  int oldcolno = colno - GSyyleng;
  if (oldcolno < 0) {
    oldcolno = 0;
  }
  fprintf(stderr, "token '%s' at position %d, %d caused the following error: %s\n", GSyytext, lineno, oldcolno, s); 
}

int GSyywrap(void) {
  return 1;
}

ATermAppl GSparse(FILE *formStream) {
  ATermAppl result = NULL;
  GStree = NULL;
  ATprotect((ATerm*) &GStree);
  lineno = 1;
  colno = 1;
  GSyyrestart(formStream);
  if (GSyyparse() != 0) {
    throwV(NULL);
  }
  //GStree contains the parsed formula
  result = GStree;
  GStree = NULL;
finally:
  ATunprotect((ATerm*) &GStree);
  return result;
}
