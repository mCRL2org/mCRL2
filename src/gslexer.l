%{
#include <string.h>
#include "gslexer.h"
#include "gsparser.h"
#include "gsfunc.h"

//Global precondition: the ATerm library has been initialised

//global declarations
ATermAppl gsTree = NULL;         /* the parse tree */

//external declarations
int gsyyparse(void);             /* declared in gsparser.c */
extern YYSTYPE gsyylval;         /* declared in gsparser.c */
extern int fileno(FILE *stream); /* declared in stdio.h (forgotten by lex) */

//local declarations
int LineNr = 1;                  /* line number in source file */
int ColNr  = 1;                  /* column number in source file */
void gsyyerror(char *s);         /* display error message (needed by yacc) */
int gsyywrap(void);              /* action taken after an EOF (needed by yacc) */

%}
Id       [a-zA-Z\_][a-zA-Z0-9\_]*
Number   "0"|("-"?[1-9][0-9]*) 

%%

[ \t]    { ColNr += gsyyleng; /* whitespace */ }

\r?\n    { LineNr++; ColNr = 1; /* newline */ }

"%".*    { ColNr += gsyyleng; /* comment */ }

"||_"    { ColNr += gsyyleng; return LMERGE; }
"->"     { ColNr += gsyyleng; return ARROW; }
"<="     { ColNr += gsyyleng; return LTE; }
">="     { ColNr += gsyyleng; return GTE; }
"|>"     { ColNr += gsyyleng; return CONS; }
"<|"     { ColNr += gsyyleng; return SNOC; }
"++"     { ColNr += gsyyleng; return CONCAT; }
"=="     { ColNr += gsyyleng; return EQ; }
"!="     { ColNr += gsyyleng; return NEQ; }
"&&"     { ColNr += gsyyleng; return AND; }
"||"     { ColNr += gsyyleng; return BARS; }
"=>"     { ColNr += gsyyleng; return IMP; }
"<<"     { ColNr += gsyyleng; return BINIT; }
"*"      { ColNr += gsyyleng; return STAR; }
"+"      { ColNr += gsyyleng; return PLUS; }
"-"      { ColNr += gsyyleng; return MINUS; }
"="      { ColNr += gsyyleng; return EQUALS; }
"."      { ColNr += gsyyleng; return DOT; }
","      { ColNr += gsyyleng; return COMMA; }
":"      { ColNr += gsyyleng; return COLON; }
";"      { ColNr += gsyyleng; return SEMICOLON; }
"?"      { ColNr += gsyyleng; return QMARK; }
"!"      { ColNr += gsyyleng; return EXCLAM; }
"@"      { ColNr += gsyyleng; return AT; }
"#"      { ColNr += gsyyleng; return HASH; }
"|"      { ColNr += gsyyleng; return BAR; }
"("      { ColNr += gsyyleng; return LPAR; }
")"      { ColNr += gsyyleng; return RPAR; }
"[]"     { ColNr += gsyyleng; return PBRACK; }
"["      { ColNr += gsyyleng; return LBRACK; }
"]"      { ColNr += gsyyleng; return RBRACK; }
"<"      { ColNr += gsyyleng; return LANG; }
">"      { ColNr += gsyyleng; return RANG; }
"{}"     { ColNr += gsyyleng; return PBRACE; }
"{"      { ColNr += gsyyleng; return LBRACE; }
"}"      { ColNr += gsyyleng; return RBRACE; }

sort     { ColNr += gsyyleng; return SORT; }
cons     { ColNr += gsyyleng; return CONS; }
map      { ColNr += gsyyleng; return MAP; }
var      { ColNr += gsyyleng; return VAR; }
eqn      { ColNr += gsyyleng; return EQN; }
act      { ColNr += gsyyleng; return ACT; }
proc     { ColNr += gsyyleng; return PROC; }
init     { ColNr += gsyyleng; return INIT; }
struct   { ColNr += gsyyleng; return STRUCT; }
Bool     { ColNr += gsyyleng; return BOOL; }
Pos      { ColNr += gsyyleng; return POS; }
Nat      { ColNr += gsyyleng; return NAT; }
Int      { ColNr += gsyyleng; return INT; }
List     { ColNr += gsyyleng; return LIST; }
Set      { ColNr += gsyyleng; return SET; }
Bag      { ColNr += gsyyleng; return BAG; }
true     { ColNr += gsyyleng; return TRUE; }
false    { ColNr += gsyyleng; return FALSE; }
whr      { ColNr += gsyyleng; return WHR; }
end      { ColNr += gsyyleng; return END; }
lambda   { ColNr += gsyyleng; return LAMBDA; }
forall   { ColNr += gsyyleng; return FORALL; }
exists   { ColNr += gsyyleng; return EXISTS; }
div      { ColNr += gsyyleng; return DIV; }
mod      { ColNr += gsyyleng; return MOD; }
in       { ColNr += gsyyleng; return IN; }
delta    { ColNr += gsyyleng; return DELTA; }
tau      { ColNr += gsyyleng; return TAU; }
sum      { ColNr += gsyyleng; return SUM; }
restrict { ColNr += gsyyleng; return RESTRICT; }
allow    { ColNr += gsyyleng; return ALLOW; }
hide     { ColNr += gsyyleng; return HIDE; }
rename   { ColNr += gsyyleng; return RENAME; }
comm     { ColNr += gsyyleng; return COMM; }

{Id}     { /* identifier (not a keyword) */
           ColNr += gsyyleng;
           gsyylval.appl = gsMakeId(gsyytext);
	   return ID;
	 }

{Number} { /* number */
           ColNr += gsyyleng;
	   gsyylval.appl = gsMakeNumber(gsyytext);
           return NUMBER;
	 }

.        { ColNr += gsyyleng; gsyyerror("unknown character"); /* remaining characters */ }

%%

void gsyyerror(char *s) {
  int oldColNr = ColNr - gsyyleng;
  if (oldColNr < 0) {
    oldColNr = 0;
  }
  fprintf(stderr, "token '%s' at position %d, %d caused the following error: %s\n", gsyytext, LineNr, oldColNr, s); 
}

int gsyywrap(void) {
  return 1;
}

ATermAppl gsParse(FILE *FormStream) {
  ATermAppl Result = NULL;
  gsTree = NULL;
  ATprotect((ATerm*) &gsTree);
  LineNr = 1;
  ColNr = 1;
  gsyyrestart(FormStream);
  if (gsyyparse() != 0) {
    ThrowV(NULL);
  }
  //gsTree contains the parsed formula
  Result = gsTree;
  gsTree = NULL;
finally:
  ATunprotect((ATerm*) &gsTree);
  return Result;
}
