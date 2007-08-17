%{
#include "pbesparser.hpp"
#include <string.h>
%}

%%

\/\/.*          /* ignore comments */


"F" {return (FALSE);}
"T" {return (TRUE);}

"0" {return (ZERO);}
"1" {return (ONE);}

[a-c] {
 yylval.string = (char*)calloc(sizeof(yytext), sizeof(char));
 strcpy(yylval.string, yytext);
 return (IDPROP);
}
[i-j] {
 yylval.string = (char*)calloc(sizeof(yytext), sizeof(char));
 strcpy(yylval.string, yytext);
 return (IDDATA);
}
[X-Z]  {
 yylval.string = (char*)calloc(sizeof(yytext), sizeof(char));
 strcpy(yylval.string, yytext);
 return (IDPRED);
}


"(" {return (LPAR);}
")" {return (RPAR);}


"<" {return (LESS);}
"==" {return (IS);}
"+" {return (PLUS);}



"mu" {return (MU);}
"nu" {return (NU);}


"&&" {return (EN);}
"||" {return (OF);}
"!" {return (NEG);}
"," {return(COMMA);}
"A" {return (FORALL);}
"E" {return (EXISTS);}


"=" {return (EQ);}
[ \t]         /* ignore whitespace */
.     return yytext[0];

%%

int yywrap(){
 return 1;
}

