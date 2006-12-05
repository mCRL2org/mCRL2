%{
#include <stdbool.h>
int yylex(void);
void yyerror(const char *);
%}
%glr-parser
%%
test:
    ;
%% 
