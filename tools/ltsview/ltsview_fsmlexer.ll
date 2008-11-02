%{
// Author(s): Bas Ploeger and Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ltsview_fsmlexer.ll

#include <string>
#include "ltsview_fsmlexer.h"
#include "ltsview_fsmparser.hpp"
/* import the parser's token type into a local typedef */
typedef ltsview::LTSViewFSMParser::token token;
typedef ltsview::LTSViewFSMParser::token_type token_type;

/* Work around an incompatibility in flex (at least versions 2.5.31 through
 * 2.5.33): it generates code that does not conform to C89.  See Debian bug
 * 333231 <http://bugs.debian.org/cgi-bin/bugreport.cgi?bug=333231>.  */
#undef yywrap
#define yywrap()        1

/* By default yylex returns int, we use token_type. Unfortunately yyterminate
 * by default returns 0, which is not of token_type. */
#define yyterminate() return token::EOFL

/* This disables inclusion of unistd.h, which is not available under Visual C++
 * on Win32. The C++ scanner uses STL streams instead. */
#define YY_NO_UNISTD_H

%}

/*** Flex Declarations and Options ***/

/* enable c++ scanner class generation */
%option c++

/* change the name of the scanner class. results in "LTSViewFSMFlexLexer" */
%option prefix="LTSViewFSM"

/* the manual says "somewhat more optimized" */
%option batch

/* no support for include files is planned */
%option nounput 

/* enables the use of start condition stacks */
%option stack

/* The following paragraph suffices to track locations accurately. Each time
 * yylex is invoked, the begin position is moved onto the end position. */
%{
#define YY_USER_ACTION  yylloc->columns(yyleng);
%}

%%
 
 /* code to place at the beginning of yylex() */
%{
    // reset location
    yylloc->step();

    // variable for quoted strings
    std::string quotedstring;
%}

 /* white space */
[ \t\r]+ {
  yylloc->step();
}

 /* end of lines */
\n {
  yylloc->lines(yyleng);
  yylloc->step();
  return token::EOLN;
}

 /* section separator */
"---" {
  return token::SECSEP;
}

 /* parenthesis left */
"(" { 
  return token::LPAR;
}

 /* parenthesis right */
")" { 
  return token::RPAR;
}

 /* arrow symbol */
"->" {
  return token::ARROW;
}

 /* ID */
[a-zA-Z_][a-zA-Z0-9_'@]* {
  yylval->stringVal = new std::string(yytext,yyleng);
  return token::ID;
}

 /* Quoted string */
\"[^\"]*\" {
  quotedstring = std::string(yytext,yyleng);
  yylval->stringVal = new std::string(quotedstring.substr(1,
                                      quotedstring.length()-2));
  return token::QUOTED;
}

 /* Number */
[0]|([1-9][0-9]*) {
  yylval->integerVal = atoi(yytext);
  return token::NUMBER;
}

 /* pass all other characters up to bison */
. { 
  return static_cast<token_type>(*yytext);
}

%%

namespace ltsview {

LTSViewFSMLexer::LTSViewFSMLexer(std::istream* in,
                 std::ostream* out)
    : LTSViewFSMFlexLexer(in, out)
{
}

LTSViewFSMLexer::~LTSViewFSMLexer()
{
}

}

/* This implementation of LTSViewFSMFlexLexer::yylex() is required to fill the
 * vtable of the class LTSViewFSMFlexLexer. We define the scanner's main yylex
 * function via YY_DECL to reside in the Scanner class instead. */

#ifdef yylex
#undef yylex
#endif

int LTSViewFSMFlexLexer::yylex() {
    return 0;
}
