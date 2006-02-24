%{
#include <stdio.h>
#include <string>
#include "aterm/aterm2.h"
#include "fsmlexer.h"
using namespace std;

int lineNo=1, posNo=1;
extern void fsmerror(const char* s);
void processId();
void processQuoted();
void processNumber();
%}
%option nounput
Quoted	   \"[^\"]*\"
Id         [a-zA-Z\_][a-zA-Z0-9\_\-\'@]*
Number     [0]|([1-9][0-9]*) 

%%

[ \t]	  { posNo += fsmleng; }
\r?\n     { lineNo++; posNo=1; return EOLN; }
"---"	  { posNo += fsmleng; return SECSEP; }
"("	  { posNo += fsmleng; return LBRACK; }
")"	  { posNo += fsmleng; return RBRACK; }
","	  { posNo += fsmleng; return COMMA; }
"fan_in"  { posNo += fsmleng; return FANIN; }
"fan_out" { posNo += fsmleng; return FANOUT; }
"node_nr" { posNo += fsmleng; return NODENR; }
{Id}	  { processId(); return ID; }
{Quoted}  { processQuoted(); return QUOTED; }
{Number}  { processNumber(); return NUMBER; }
.         { posNo += fsmleng; fsmerror("unknown character"); }

%%

void processId()
{
  posNo += fsmleng;
  fsmlval.aterm = ATmakeAppl0( ATmakeAFun( fsmtext, 0, ATfalse ) ); 
}

void processQuoted()
{
  posNo += fsmleng;
  string value = static_cast<string>( fsmtext );
  value = value.substr( 1, value.length() - 2 );
  fsmlval.aterm = ATmakeAppl0( ATmakeAFun( value.c_str(), 0, ATfalse ) );
} 

void processNumber()
{
  posNo += fsmleng;
  fsmlval.number = atoi( fsmtext );
}
