%{
#include <string>
#include "fsmparser.hpp"

int lineNo=1, posNo=1;
std::string strval;
int intval;
extern void fsmerror(const char* s);
void processId();
void processQuoted();
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
{Quoted}	{ processQuoted(); return QUOTED; }
{Number}	{ processNumber(); return NUMBER; }
.					{ posNo += fsmleng; fsmerror("unknown character"); }

%%

void processId() {
	posNo += fsmleng;
	fsmlval.str = strdup(fsmtext);
}

void processQuoted() {
	posNo += fsmleng;
	std::string val = static_cast<std::string>(fsmtext);
	val = val.substr(1,val.length()-2);
	fsmlval.str = strdup(val.c_str());
} 

void processNumber() {
	posNo += fsmleng;
	fsmlval.num = atoi(fsmtext);
}
