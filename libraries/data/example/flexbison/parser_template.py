from path	import *
import os
import re

PARSER_YY	=	'''
/**	\\file parser.yy Contains	the	example	Bison	parser source	*/

%{ /***	C/C++	Declarations ***/

#include <cstdio>
#include <string>
#include <vector>

#include "USER_SEMANTIC_ACTIONS_HEADER"

%}

/*** yacc/bison	Declarations ***/

/* Require bison 2.3 or	later	*/
%require "2.3"

/* add debug output	code to	generated	parser.	disable	this for release
 * versions. */
%debug

/* start symbol	is named "start" */
%start start

/* write out a header	file containing	the	token	defines	*/
%defines

/* use newer C++ skeleton	file */
%skeleton	"lalr1.cc"

/* namespace to	enclose	parser in	*/
%name-prefix="USER_NAMESPACE"

/* set the parser's	class	identifier */
%define	"parser_class_name"	"USER_PARSER_CLASSNAME"

/* keep	track	of the current position	within the input */
%locations
%initial-action
{
		// initialize	the	initial	location object
		@$.begin.filename	=	@$.end.filename	=	&driver.streamname;
};

/* The driver	is passed	by reference to	the	parser and to	the	scanner. This
 * provides	a	simple but effective pure	interface, not relying on	global
 * variables.	*/
%parse-param { class USER_DRIVER_CLASSNAME&	driver }

/* verbose error messages	*/
%error-verbose

USER_TOKEN_DEFINITION

%{

#include "USER_DRIVER_HEADER"
#include "USER_SCANNER_HEADER"

/* this	"connects" the bison parser	in the driver	to the flex	scanner	class
 * object. it	defines	the	yylex()	function call	to pull	the	next token from	the
 * current lexer object	of the driver	context. */
#undef yylex
#define	yylex	driver.lexer->lex

%}

%%

USER_GRAMMAR_DEFINITION

%%

void USER_NAMESPACE::USER_PARSER_CLASSNAME::error(const	USER_PARSER_CLASSNAME::location_type&	l,
																									const	std::string& m)
{
	driver.error(l,	m);
}
'''

SCANNER_LL = '''
/**	\\file scanner.ll	Define the example Flex	lexical	scanner	*/

%{ /***	C/C++	Declarations ***/

#include <string>

#include "USER_SCANNER_HEADER"

/* import	the	parser's token type	into a local typedef */
typedef	USER_NAMESPACE::USER_PARSER_CLASSNAME::token token;
typedef	USER_NAMESPACE::USER_PARSER_CLASSNAME::token_type	token_type;

/* By	default	yylex	returns	int, we	use	token_type.	Unfortunately	yyterminate
 * by	default	returns	0, which is	not	of token_type. */
#define	yyterminate()	return token::END

/* This	disables inclusion of	unistd.h,	which	is not available under Visual	C++
 * on	Win32. The C++ scanner uses	STL	streams	instead. */
#define	YY_NO_UNISTD_H

%}

/*** Flex	Declarations and Options ***/

/* enable	c++	scanner	class	generation */
%option	c++

/* change	the	name of	the	scanner	class. results in	"ExampleFlexLexer" */
%option	prefix="USER_SCANNER_CLASSNAME_PREFIX"

/* the manual	says "somewhat more	optimized" */
%option	batch

/* enable	scanner	to generate	debug	output.	disable	this for release
 * versions. */
%option	debug

/* no	support	for	include	files	is planned */
%option	yywrap nounput

/* enables the use of	start	condition	stacks */
%option	stack

/* The following paragraph suffices	to track locations accurately. Each	time
 * yylex is	invoked, the begin position	is moved onto	the	end	position.	*/
%{
#define	YY_USER_ACTION	yylloc->columns(yyleng);
%}

%% /***	Regular	Expressions	Part ***/

 /*	code to	place	at the beginning of	yylex()	*/
%{
		// reset location
		yylloc->step();
%}

USER_SCANNER_DEFINITION

%% /***	Additional Code	***/

namespace	example	{

Scanner::Scanner(std::istream* in,
		 std::ostream* out)
		:	USER_SCANNER_CLASSNAME(in, out)
{
}

Scanner::~Scanner()
{
}

void Scanner::set_debug(bool b)
{
		yy_flex_debug	=	b;
}

}

/* This	implementation of	ExampleFlexLexer::yylex()	is required	to fill	the
 * vtable	of the class ExampleFlexLexer. We	define the scanner's main	yylex
 * function	via	YY_DECL	to reside	in the Scanner class instead.	*/

#ifdef yylex
#undef yylex
#endif

int	USER_SCANNER_CLASSNAME::yylex()
{
		std::cerr	<< "in USER_SCANNER_CLASSNAME::yylex() !"	<< std::endl;
		return 0;
}

/* When	the	scanner	receives an	end-of-file	indication from	YY_INPUT,	it then
 * checks	the	yywrap() function. If	yywrap() returns false (zero), then	it is
 * assumed that	the	function has gone	ahead	and	set	up `yyin'	to point to
 * another input file, and scanning	continues. If	it returns true	(non-zero),
 * then	the	scanner	terminates,	returning	0	to its caller. */

int	USER_SCANNER_CLASSNAME::yywrap()
{
		return 1;
}
'''

def	apply_substitutions(text,	sigma):
	text = re.sub('USER_DRIVER_HEADER'					 , sigma['USER_DRIVER_HEADER'						], text)
	text = re.sub('USER_PARSER_HEADER'					 , sigma['USER_PARSER_HEADER'						], text)
	text = re.sub('USER_SCANNER_HEADER'					 , sigma['USER_SCANNER_HEADER'					], text)
	text = re.sub('USER_SEMANTIC_ACTIONS_HEADER' , sigma['USER_SEMANTIC_ACTIONS_HEADER' ], text)
	text = re.sub('USER_NAMESPACE'							 , sigma['USER_NAMESPACE'								], text)
	text = re.sub('USER_DRIVER_CLASSNAME'				 , sigma['USER_DRIVER_CLASSNAME'				], text)
	text = re.sub('USER_PARSER_CLASSNAME'				 , sigma['USER_PARSER_CLASSNAME'				], text)
	text = re.sub('USER_SCANNER_CLASSNAME_PREFIX', sigma['USER_SCANNER_CLASSNAME_PREFIX'], text)
	text = re.sub('USER_SCANNER_CLASSNAME'			 , sigma['USER_SCANNER_CLASSNAME'				], text)
	text = re.sub('USER_TOKEN_DEFINITION'				 , sigma['USER_TOKEN_DEFINITION'				], text)
	text = re.sub('USER_GRAMMAR_DEFINITION'			 , sigma['USER_GRAMMAR_DEFINITION'			], text)
	text = re.sub('USER_SCANNER_DEFINITION'			 , sigma['USER_SCANNER_DEFINITION'			], text)
	return text

def	make_parser(parser_filename,
								scanner_filename,
								USER_DRIVER_HEADER					 ,
								USER_PARSER_HEADER					 ,
								USER_SCANNER_HEADER					 ,
                USER_SEMANTIC_ACTIONS_HEADER ,
								USER_NAMESPACE							 ,
								USER_DRIVER_CLASSNAME				 ,
								USER_PARSER_CLASSNAME				 ,
								USER_SCANNER_CLASSNAME_PREFIX,
								USER_SCANNER_CLASSNAME			 ,
								USER_TOKEN_DEFINITION				 ,
								USER_GRAMMAR_DEFINITION			 ,
								USER_SCANNER_DEFINITION
							 ):
	sigma	=	{}
	sigma['USER_DRIVER_HEADER'					 ] = USER_DRIVER_HEADER
	sigma['USER_PARSER_HEADER'					 ] = USER_PARSER_HEADER
	sigma['USER_SCANNER_HEADER'					 ] = USER_SCANNER_HEADER
	sigma['USER_SEMANTIC_ACTIONS_HEADER' ] = USER_SEMANTIC_ACTIONS_HEADER
	sigma['USER_NAMESPACE'							 ] = USER_NAMESPACE
	sigma['USER_DRIVER_CLASSNAME'				 ] = USER_DRIVER_CLASSNAME
	sigma['USER_PARSER_CLASSNAME'				 ] = USER_PARSER_CLASSNAME
	sigma['USER_SCANNER_CLASSNAME_PREFIX'] = USER_SCANNER_CLASSNAME_PREFIX
	sigma['USER_SCANNER_CLASSNAME'			 ] = USER_SCANNER_CLASSNAME
	sigma['USER_TOKEN_DEFINITION'				 ] = USER_TOKEN_DEFINITION
	sigma['USER_GRAMMAR_DEFINITION'			 ] = USER_GRAMMAR_DEFINITION
	sigma['USER_SCANNER_DEFINITION'			 ] = USER_SCANNER_DEFINITION
	text = apply_substitutions(PARSER_YY,	sigma)
	path(parser_filename).write_text(text)
	text = apply_substitutions(SCANNER_LL, sigma)
	path(scanner_filename).write_text(text)
	parser_cpp_filename	=	parser_filename[:-2] + 'cpp'
	scanner_cpp_filename = scanner_filename[:-2] + 'cpp'
	os.system('bison -o	%s --defines=%s	%s'	%	(parser_cpp_filename,	USER_PARSER_HEADER,	parser_filename))
	os.system('flex -o %s %s' % (scanner_cpp_filename, scanner_filename))
