%{
#include <stdio.h>
#include <string.h>
#include "fsmlexer.h"

int lineNo=1, posNo=1;
extern void fsmerror(const char* s);

%}
Quoted	   \"[^\"]*\"
Id         [a-zA-Z\_][a-zA-Z0-9\_\-\']*
Number     [0]|([1-9][0-9]*) 

%%

[ \t]	  posNo += fsmleng;
\r?\n     lineNo++; posNo=1; fsmlval.str = strdup(fsmtext); return EOLN;
"---"	  posNo += fsmleng; fsmlval.str = strdup(fsmtext); return SECSEP;
"\""	  posNo += fsmleng; fsmlval.str = strdup(fsmtext); return QUOTE;
"("	  posNo += fsmleng; fsmlval.str = strdup(fsmtext); return LBRACK;
")"	  posNo += fsmleng; fsmlval.str = strdup(fsmtext); return RBRACK;
","	  posNo += fsmleng; fsmlval.str = strdup(fsmtext); return COMMA;
fan_in	  posNo += fsmleng; fsmlval.str = strdup(fsmtext); return FANIN;
fan_out	  posNo += fsmleng; fsmlval.str = strdup(fsmtext); return FANOUT;
node_nr	  posNo += fsmleng; fsmlval.str = strdup(fsmtext); return NODENR;
{Id}	  posNo += fsmleng; fsmlval.str = strdup(fsmtext); return ID;
{Number}  posNo += fsmleng; fsmlval.number = atoi( fsmtext ); return NUMBER;
{Quoted}  posNo += fsmleng; fsmlval.str = strdup(fsmtext); return QUOTED;
.         posNo += fsmleng; fsmerror("unknown character");

%%
