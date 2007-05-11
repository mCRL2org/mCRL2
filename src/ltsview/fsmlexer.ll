%{
#include <string.h>
#include "fsmparser.hpp"

int lineNo=1, posNo=1;
extern void fsmerror(const char* s);
extern char* string_buffer;
extern unsigned int string_buffer_size;
void processId();
void processNumber();
%}
%option nounput
Quoted	\"[^\"]*\"
Id			[a-zA-Z_][a-zA-Z0-9_'@]*
Number	[0]|([1-9][0-9]*) 

%%

[ \t]			{ posNo += fsmleng; }
\r?\n			{ lineNo++; posNo=1; return EOLN; }
"---"			{ posNo += fsmleng; return SECSEP; }
"("				{ posNo += fsmleng; return LPAR; }
")"				{ posNo += fsmleng; return RPAR; }
"->"			{ posNo += fsmleng; return ARROW; }
"fan_in"	{ posNo += fsmleng; return FANIN; }
"fan_out"	{ posNo += fsmleng; return FANOUT; }
"node_nr"	{ posNo += fsmleng; return NODENR; }
{Id}			{ processId(); return ID; }
{Quoted}	{ processId(); return QUOTED; }
{Number}	{ processNumber(); return NUMBER; }
.					{ posNo += fsmleng; fsmerror("unknown character"); }

%%

void processId() {
	posNo += fsmleng;
  if (string_buffer_size < strlen(fsmtext)+1) {
    string_buffer_size = strlen(fsmtext)+1;
    string_buffer = (char*)realloc(string_buffer,string_buffer_size*sizeof(char));
    if (string_buffer == NULL) {
      fsmerror("out of memory");
    }
  }
  fsmlval.str = strcpy(string_buffer,fsmtext);
}

void processNumber() {
	posNo += fsmleng;
	fsmlval.num = atoi(fsmtext);
}
