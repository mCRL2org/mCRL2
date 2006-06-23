%{
//needed for newer versions of flex (>= 2.5.31)
#ifndef yywrap
#define yywrap testyywrap
#endif
extern "C" int testyywrap(void);
//C linkage is needed for compatibility with older versions of flex (2.5.4)

//fix for the broken cygwin versions of flex
#ifdef __CYGWIN__
#include <iostream>
using std::cerr;
using std::cin;
using std::cout;
#endif

int  testyylex(void);
void testyyerror(const char *s);
%}
%option c++
%option prefix="testyy"
%%
%%
void testyyerror(const char *) { }
int testyywrap(void) { return 1; }
