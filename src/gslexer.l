%{
#include <string.h>
#include "gslexer.h"
#include "gsparser.h"
#include "gsfunc.h"
#include "gslowlevel.h"

//Global precondition: the ATerm library has been initialised

//global declarations
ATermAppl gsTree = NULL;         /* the parse tree */

//external declarations
int gsyyparse(void);             /* declared in gsparser.c */
extern YYSTYPE gsyylval;         /* declared in gsparser.c */
#ifndef _MSC_VER
extern int fileno(FILE *stream); /* declared in stdio.h (forgotten by lex) */
#endif

//local declarations
int LineNr = 1;                  /* line number in source file */
int ColNr  = 1;                  /* column number in source file */
void gsProcessString(void);      /* default action to be taken when a string is
                                    scanned */
void gsyyerror(char *s);         /* display error message (needed by yacc) */
#ifdef __cplusplus
extern "C" int gsyywrap(void);   /* action taken after an EOF (needed by yacc) */
#else
int gsyywrap(void);              /* action taken after an EOF (needed by yacc) */
#endif

%}
Id         [a-zA-Z\_][a-zA-Z0-9\_]*
Number     "0"|([1-9][0-9]*) 

%%

[ \t]      { ColNr += gsyyleng; /* whitespace */ }

\r?\n      { LineNr++; ColNr = 1; /* newline */ }

"%".*      { ColNr += gsyyleng; /* comment */ }

"||_"      { gsProcessString(); return LMERGE; }
"->"       { gsProcessString(); return ARROW; }
"<="       { gsProcessString(); return LTE; }
">="       { gsProcessString(); return GTE; }
"|>"       { gsProcessString(); return CONS; }
"<|"       { gsProcessString(); return SNOC; }
"++"       { gsProcessString(); return CONCAT; }
"=="       { gsProcessString(); return EQ; }
"!="       { gsProcessString(); return NEQ; }
"&&"       { gsProcessString(); return AND; }
"||"       { gsProcessString(); return BARS; }
"=>"       { gsProcessString(); return IMP; }
"<<"       { gsProcessString(); return BINIT; }
"*"        { gsProcessString(); return STAR; }
"+"        { gsProcessString(); return PLUS; }
"-"        { gsProcessString(); return MINUS; }
"="        { gsProcessString(); return EQUALS; }
"."        { gsProcessString(); return DOT; }
","        { gsProcessString(); return COMMA; }
":"        { gsProcessString(); return COLON; }
";"        { gsProcessString(); return SEMICOLON; }
"?"        { gsProcessString(); return QMARK; }
"!"        { gsProcessString(); return EXCLAM; }
"@"        { gsProcessString(); return AT; }
"#"        { gsProcessString(); return HASH; }
"|"        { gsProcessString(); return BAR; }
"("        { gsProcessString(); return LPAR; }
")"        { gsProcessString(); return RPAR; }
"[]"       { gsProcessString(); return PBRACK; }
"["        { gsProcessString(); return LBRACK; }
"]"        { gsProcessString(); return RBRACK; }
"<"        { gsProcessString(); return LANG; }
">"        { gsProcessString(); return RANG; }
"{}"       { gsProcessString(); return PBRACE; }
"{"        { gsProcessString(); return LBRACE; }
"}"        { gsProcessString(); return RBRACE; }

sort       { gsProcessString(); return KWSORT; }
cons       { gsProcessString(); return KWCONS; }
map        { gsProcessString(); return KWMAP; }
var        { gsProcessString(); return KWVAR; }
eqn        { gsProcessString(); return KWEQN; }
act        { gsProcessString(); return KWACT; }
proc       { gsProcessString(); return KWPROC; }
init       { gsProcessString(); return KWINIT; }
struct     { gsProcessString(); return KWSTRUCT; }
Bool       { gsProcessString(); return BOOL; }
Pos        { gsProcessString(); return POS; }
Nat        { gsProcessString(); return NAT; }
Int        { gsProcessString(); return INT; }
List       { gsProcessString(); return LIST; }
Set        { gsProcessString(); return SET; }
Bag        { gsProcessString(); return BAG; }
true       { gsProcessString(); return TRUE; }
false      { gsProcessString(); return FALSE; }
whr        { gsProcessString(); return WHR; }
end        { gsProcessString(); return END; }
lambda     { gsProcessString(); return LAMBDA; }
forall     { gsProcessString(); return FORALL; }
exists     { gsProcessString(); return EXISTS; }
div        { gsProcessString(); return DIV; }
mod        { gsProcessString(); return MOD; }
in         { gsProcessString(); return IN; }
delta      { gsProcessString(); return DELTA; }
tau        { gsProcessString(); return TAU; }
sum        { gsProcessString(); return SUM; }
block      { gsProcessString(); return BLOCK; }
allow      { gsProcessString(); return ALLOW; }
hide       { gsProcessString(); return HIDE; }
rename     { gsProcessString(); return RENAME; }
comm       { gsProcessString(); return COMM; }

{Id}       { gsProcessString(); return ID; }

{Number}   { gsProcessString(); return NUMBER; }

.          { 
             ColNr += gsyyleng; gsyyerror("unknown character");
             /* remaining characters */
	   }

%%

void gsProcessString(void) {
  ColNr += gsyyleng;
  gsyylval.appl = gsString2ATermAppl(gsyytext);
}

void gsyyerror(char *s) {
  int oldColNr = ColNr - gsyyleng;
  if (oldColNr < 0) {
    oldColNr = 0;
  }
  fprintf(
    stderr,
    "token '%s' at position %d, %d caused the following error: %s\n", 
    gsyytext, LineNr, oldColNr, s
  ); 
}

int gsyywrap(void) {
  return 1;
}

ATermAppl gsParse(FILE *FormStream) {
  ATermAppl Result = NULL;
  gsTree = NULL;
  ATprotectAppl(&gsTree);
  LineNr = 1;
  ColNr = 1;
  gsyyrestart(FormStream);
  if (gsyyparse() != 0) {
    Result = NULL;
  } else {
    //gsTree contains the parsed formula
    Result = gsTree;
    gsTree = NULL;
  }

  ATunprotectAppl(&gsTree);
  return Result;
}
