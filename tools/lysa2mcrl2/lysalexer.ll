%{

#include "lysa.h"
#include "lysalexer.h"
#include <string.h>
#include <math.h>
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <utility>
#include <sstream>


using namespace mcrl2::log;

/*
 * we store the semantic values of all lexical tokens in an object
 * derived from lysa::Expression. this way, we avoid using bison's
 * %union convention which disallows complex types.
 */
typedef boost::shared_ptr<lysa::Expression> semval;
#define YYSTYPE semval
#include "lysaparser.h"


//fix for newer versions of flex (>= 2.5.31)
#ifndef yywrap
#define yywrap lysayywrap
#endif

//disable MSVC specific warnings
#ifdef BOOST_MSVC
#pragma warning( disable : 4273 )
#endif

//external declarations
int lysayyparse(void);          /* declared in lysaparser.cpp */
extern YYSTYPE lysayylval;      /* declared in lysaparser.cpp */


extern YYLTYPE lysayylloc;

//global declarations, used by lysaparser.cpp
int  lysayylex(void);
void lysayyerror(const char *s);
extern "C" int lysayywrap(void);
//Note: C linkage is needed for older versions of flex (2.5.4)
// warning from confused programmer: NEWER flex versions can have
// LOWER version numbers (f.ex. 2.5.33)


//local declarations

/* the parse tree */
boost::shared_ptr<lysa::Expression> parse_tree;      


/**
 * Lexer class. the public functions implement the FlexLexer interface, the
 * protected functions are used only internally.
 */
class lysaLexer : public lysayyFlexLexer {
public:
	lysaLexer(lysa::lysa_options options) : options(options), line_nr(1), col_nr(1) {};
	int yylex(void);               /* the generated lexer function */
	void yyerror(const char *s);
	virtual int yywrap(void);
	semval parse_stream(std::istream &stream );
protected:
	int hint_paren_depth;
	std::ostringstream hint_collect;

	lysa::lysa_options options;
	std::istream *cur_stream;      /* input stream */
	int line_nr;                   /* line number in cur_streams[cur_index] */
	int col_nr;                    /* column number in cu_streams[cur_index] */
	void process_string();         /* update position, provide token to parser */
	void process_hint();           /* update position, provide hint token to parser */
	void stream_hint();           /* update position, provide hint token to parser */
	void update_pos();             /* update position */
	void process_iset(lysa::IsetDefSet def);   /* update position, provide iset token to parser */
};

//implement yylex in lysaLexer instead of lysayyFlexLexer
#define YY_DECL int lysaLexer::yylex()
int lysayyFlexLexer::yylex(void) { return 1; }

/* lexer object, used by parse_streams */
lysaLexer *lexer = NULL;       



%}
digit       [0-9]
number      "0"|([1-9]{digit}*)

/* any word starting with an alphabetic character, no underscores, not ending with a dash */
identifier  ([a-zA-Z][a-zA-Z0-9\-']*[a-zA-Z0-9'])|([a-zA-Z])
alnum       [a-zA-Z0-9]+
string      "\""[^"]"\""

%option c++
%option prefix="lysayy"
%option nounput
%s INIT
%x COMMENT
%x HINT

%%
[ \t]   { col_nr += YYLeng();			/* whitespace */ }
\r?\n   { col_nr = 1; ++line_nr;	/* newline */ }
"//".*  { col_nr += YYLeng();			/* comment until end-of-line */ }

"/*"						{BEGIN(COMMENT); col_nr += YYLeng();	/* comment start */ }
<COMMENT>"*/"   {BEGIN(INIT); col_nr += YYLeng();			/* comment end */ }
<COMMENT>.      {col_nr += YYLeng();									/* comment contents */}

"#("				{
	BEGIN(HINT); 
	update_pos(); 
	hint_paren_depth = 0; 
	hint_collect.str("");  
}
<HINT>")"   {
	if(!hint_paren_depth)
	{
		BEGIN(INIT); 
		process_hint();
		return T_HINT;
	}
	else
	{
		hint_paren_depth--;
		update_pos();
		hint_collect << YYText();
	}
}
<HINT>"("   {
	update_pos();
	hint_collect << YYText();
	hint_paren_depth++;
}
<HINT>[^\)\(]+     {
	update_pos();
	hint_collect << YYText();
}

"decrypt"   { update_pos(); return T_DECRYPT; }
"as"        { update_pos(); return T_AS; }
"in"        { update_pos(); return T_IN; }
"let"       { update_pos(); return T_LET; }
"subset"    { update_pos(); return T_SUBSET; }
"union"	    { update_pos(); return T_UNION; }
"DY"	      { update_pos(); return T_DY; }
"ZERO"      { process_iset(lysa::ZERO);				 return T_ISET_DEF; }
"NATURAL1"  { process_iset(lysa::NATURAL1);  return T_ISET_DEF; }
"NATURAL2"  { process_iset(lysa::NATURAL2);  return T_ISET_DEF; }
"NATURAL3"  { process_iset(lysa::NATURAL3);  return T_ISET_DEF; }
"NATURAL01" { process_iset(lysa::NATURAL01); return T_ISET_DEF; }
"NATURAL02" { process_iset(lysa::NATURAL02); return T_ISET_DEF; }
"NATURAL03" { process_iset(lysa::NATURAL03); return T_ISET_DEF; }
"new"       { update_pos(); return T_NEW; }
"at"        { update_pos(); return T_AT; }
"dest"      { update_pos(); return T_DEST; }
"orig"      { update_pos(); return T_ORIG; }
"CPDY"      { update_pos(); return T_CPDY; }

[+\-] { process_string(); return T_PLUS_OR_MINUS; }
[CN]  { process_string(); return T_TYPENAME; }
"_{" { update_pos(); return T_LBRACE_SUBSCRIPT; }
"_"	 { update_pos(); return T_UNDERSCORE; }
"("  { update_pos(); return T_LPAREN; }
")"  { update_pos(); return T_RPAREN; }
"["  { update_pos(); return T_LBOX; }
"]"  { update_pos(); return T_RBOX; }
"<"  { update_pos(); return T_LT; }
">"  { update_pos(); return T_GT; }
"="  { update_pos(); return T_EQUALS; }
";"  { update_pos(); return T_SEMICOLON; }
":"  { update_pos(); return T_COLON; }
"."  { update_pos(); return T_DOT; }
","  { update_pos(); return T_COMMA; }
"|"  { update_pos(); return T_PIPE; }
"{"  { update_pos(); return T_LBRACE; }
"}"  { update_pos(); return T_RBRACE; }
"{|" { update_pos(); return T_LBRACE_PIPE; }
"|}" { update_pos(); return T_RBRACE_PIPE; }
"!"  { update_pos(); return T_BANG; }
"+-" { update_pos(); return T_PLUS_MINUS; }

{identifier}    { process_string(); return T_IDENT; }
{number}        { process_string(); return T_NUM; }
{alnum}         { process_string(); return T_ALNUM; }

%%
//Implementation of the global functions, needed for interfacing with bison.
semval parse_stream ( std::istream &stream, lysa::lysa_options options ) {
  lexer = new lysaLexer(options);
  semval result = lexer->parse_stream(stream);
  delete lexer;
  return result;
}

int lysayylex(void) {
  return lexer->yylex();
}

void lysayyerror(const char *s) {
  return lexer->yyerror(s);
}

int lysayywrap(void) {
  return lexer->yywrap();
}


void lysaLexer::yyerror(const char *s) {
	int old_col_nr = std::max(1, col_nr - YYLeng());
	mCRL2log(error) << "token '" << YYText() << "' at position" << line_nr << ", "
	                << old_col_nr << " caused the following error:" << std::endl << "  " << s << std::endl;
}

int lysaLexer::yywrap(void) {
/** 
  * When the scanner receives an end-of-file indication from YY_INPUT, it  
  * checks the `yywrap()' function. Because we only have one input stream,
  * we need to terminate the parser, by returning the 1.
  **/
  return 1;
}

void lysaLexer::update_pos()
{
	lysayylloc.first_line = line_nr;
	lysayylloc.first_column = col_nr;

	col_nr += YYLeng();

	lysayylloc.last_line = line_nr;
	lysayylloc.last_column = col_nr;
}

void lysaLexer::process_string() {
	update_pos();
	std::string s(YYText());
	semval v(new lysa::String(s));
	lysayylval = v;
}

void lysaLexer::process_hint() {
	update_pos();
	std::string s(hint_collect.str());
	semval v(new lysa::String(s));
	lysayylval = v;
}

void lysaLexer::process_iset(lysa::IsetDefSet def) {
 update_pos();
	semval d(new lysa::IsetDefSetSemval(def));
	lysayylval = d;
}


semval lysaLexer::parse_stream(std::istream &stream)
{
	line_nr = 1;
	col_nr = 1;
	cur_stream  = &stream ;
	switch_streams(cur_stream , NULL);

	int res = 0;
	try
	{
		lysa::start_parsing(options);
		res = lysayyparse();
	}
	catch(std::string s)
	{
		yyerror(s.c_str());
	}
	catch(char* s)
	{
		yyerror(s);
	}
	if (res != 0)
	{
		// return empty shared_ptr.
		semval result;
		return result;
	}
	else
	{
		//return a new shared_ptr to the parse tree.
		semval result(parse_tree);
		parse_tree.reset();
		return result;
	}
}

//weird workaround: i do not understand why, but this seems to work.
//maybe it is because build\workarounds\FlexLexer.h is for flex 2.5.35 
//which I have not managed to find a windows port for, so i use 2.5.34.
#undef yywrap
int lysayyFlexLexer::yywrap(void) { return 1; }
