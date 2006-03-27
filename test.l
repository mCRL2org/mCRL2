%{
//needed for newer versions of flex (>= 2.5.31)
#ifndef yywrap
#define yywrap mcrl2yywrap
#endif
extern "C" int mcrl2yywrap(void);
//C linkage is needed for compatibility with older versions of flex (2.5.4)

//global declarations, used by mcrl2parser.cpp
int  mcrl2yylex(void);           /* lexer function */
void mcrl2yyerror(const char *s);/* error function */

%}

%option c++
%option prefix="mcrl2yy"
%option nounput

%%


%%

//int mcrl2yylex(void) {
//}
//.          { mcrl2yyerror("unknown character"); }

void mcrl2yyerror(const char *) {
}

int mcrl2yywrap(void) {
    return 1;
}
