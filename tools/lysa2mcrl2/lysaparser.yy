%{
#include "lysa.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <utility>
#include <cctype>
#include <string.h>

//disable MSVC specific warnings
#ifdef BOOST_MSVC
#pragma warning( disable : 4273 4065 )
#endif


using namespace std;
using namespace lysa;


//semantic values are always stored by lysaLexer.ll as shared pointers to lysa expressions
typedef boost::shared_ptr<Expression> semval;
typedef boost::shared_ptr<String> semval_str;
#define YYSTYPE semval

//external declarations from lysalexer.ll
void lysayyerror( const char *s );
int lysayylex( void );
extern boost::shared_ptr<Expression> parse_tree;


#define YYMAXDEPTH 160000

%}

//set name prefix
%name-prefix="lysayy"

/* 
 *  TERMINALS
 *  ---------
 */



/* keywords */
%token T_DECRYPT
%token T_AS
%token T_IN
%token T_LET
%token T_SUBSET
%token T_UNION
%token T_NEW
%token T_AT
%token T_DEST
%token T_ORIG
%token T_CPDY
%token T_DY
%token T_HINT

/* interpunction */
%token T_LBRACE_SUBSCRIPT
%token T_UNDERSCORE
%token T_LPAREN
%token T_RPAREN
%token T_LBOX
%token T_RBOX
%token T_LT
%token T_GT
%token T_EQUALS
%token T_SEMICOLON
%token T_COLON
%token T_DOT
%token T_COMMA
%token T_PIPE
%token T_LBRACE
%token T_RBRACE
%token T_LBRACE_PIPE
%token T_RBRACE_PIPE
%token T_BANG
%token T_PLUS_MINUS

%token T_PLUS_OR_MINUS
%token T_IDENT
%token T_NUM
%token T_ALNUM
%token T_ISET_DEF
%token T_TYPENAME

%debug
%verbose
%error-verbose

%left T_INDEXED_PIPE
%left T_PIPE
%nonassoc T_BANG
%nonassoc T_DOT T_IN T_RPAREN
%right T_LBOX
%left T_UNION
%left T_HINT

/* 
	dangling else-ish conflict on ciphertexts having annotations or not:
	bison shifts by default, which is what we want.
*/
%expect 2 

%%
/*** PROCESSES ***/
start:
	proc 
{ 
	parse_tree = $1;
	$$ = $1;
}
proc:
	proc T_HINT 
	{
		$$->hint(static_pointer_cast<String>($2)->s);
	}

|	T_LPAREN proc T_RPAREN { $$ = $2; }
	
	/* zero */
|	T_NUM 
{ 
	if(static_pointer_cast<String>($1)->s=="0")
	{
		set_current_position(@$);
		semval e(new Zero());
		$$ = e;
	}
	else
	{
		yyerror("syntax error: \"0\" expected");
		YYERROR;
	}
}

	/* dy */
|	T_DY 
{ 
	set_current_position(@$);
	semval e(new DY());
	$$ = e;
}

	/* send */
|	T_LT terms T_GT T_DOT proc
{
	set_current_position(@$);
	semval e(new Send($2, $5));
	$$ = e;
}

	/* receive */
|	T_LPAREN pmatch_terms T_RPAREN T_DOT proc
{
	set_current_position(@$);
	semval e(new Receive($2, $5));
	$$ = e;
}

	/* decrypt */
|	T_DECRYPT term T_AS T_LBRACE pmatch_terms T_RBRACE T_COLON term anno_orig T_IN proc
{
	set_current_position(@$);
	semval e(new Decrypt($2, $5, $8, $9, $11));
	$$ = e;
}

	/* new */
|	T_LPAREN T_NEW name T_RPAREN proc
{
	set_current_position(@$);
	semval e(new New($3, false, $5));
	$$ = e;
}
|	T_LPAREN T_NEW T_PLUS_MINUS name T_RPAREN proc
{
	set_current_position(@$);
	semval e(new New($4, true, $6));
	$$ = e;
}
|	T_LPAREN T_NEW T_LBRACE_SUBSCRIPT index_defs T_RBRACE name T_RPAREN proc
{
	set_current_position(@$);
	semval e(new New($6, false, $4, $8));
	$$ = e;
}
|	T_LPAREN T_NEW T_LBRACE_SUBSCRIPT index_defs T_RBRACE T_PLUS_MINUS name T_RPAREN proc
{
	set_current_position(@$);
	semval e(new New($7, true, $4, $9));
	$$ = e;
}

	/* parallelism: */
| T_BANG proc
{
	set_current_position(@$);
	semval e(new Replication($2));
	$$ = e;
}
| T_PIPE T_LBRACE_SUBSCRIPT index_defs T_RBRACE proc %prec T_INDEXED_PIPE
{
	set_current_position(@$);
	semval e(new IndexedParallel($5, $3));
	$$ = e;
}
| proc T_PIPE proc
{
	set_current_position(@2);
	std::list<semval> l;
	l.push_back($1);
	l.push_back($3);
	semval e(new OrdinaryParallel(l));
	$$ = e;
}

  /* LySa let */
| T_LET T_IDENT T_SUBSET iset T_IN proc
{
	set_current_position(@$);
	semval e(new Let(LySa, $2, $4, $6));
	$$ = e;
}
  /* Typed LySa let */
| T_LET T_IDENT T_EQUALS iset T_IN proc
{
	set_current_position(@$);
	semval e(new Let(TypedLySa, $2, $4, $6));
	$$ = e;
}

/*** TERMS ***/
term:
	name
|	T_LPAREN term T_RPAREN { $$ = $2; }

	/* ciphertext */
|	T_LBRACE terms T_RBRACE T_COLON term anno_dest
{
	set_current_position(@$);
	semval e(new Ciphertext(false, $2, $5, $6));
	$$ = e;
}
|	T_LBRACE_PIPE terms T_RBRACE_PIPE T_COLON term anno_dest
{
	set_current_position(@$);
	semval e(new Ciphertext(true, $2, $5, $6));
	$$ = e;
}

	/* pattern matching terms: two options, one where there are no terms before the semicolon,
	 * and one where there *may be* none after the semicolon. meant to disallow 
	 * (;) but allow (a;) and (;a).
	 */
pmatch_terms:
	noterms T_SEMICOLON terms
{
	set_current_position(@$);
	semval e(new PMatchTerms($1, $3));
	$$ = e;
}
|	terms T_SEMICOLON terms_noterms
{
	set_current_position(@$);
	semval e(new PMatchTerms($1, $3));
	$$ = e;
}

	/* terms: separate nonterminal for 1 or more terms, 0 or more terms and 0 terms */	
terms_noterms:
	noterms | terms
noterms:
	/* empty */ 
{ 
	set_current_position(@$);
	semval e(new Terms());
	$$ = e;
}
terms:
	term
	{
		set_current_position(@$);
		semval e(new Terms($1));
		$$ = e;
	}
|	terms T_COMMA term
	{
		dynamic_pointer_cast<Terms>($1)->push_back($3);
		$$ = $1;
	}

/*** ANNOTATIONS ***/
anno_dest:
	/* empty */
	{
		set_current_position(@$);
		semval e(new Annotation(true));
		$$ = e;
	}
| T_LBOX T_AT cryptopoint T_RBOX
	{
		set_current_position(@$);
		semval e(new Annotation(true, $3));
		$$ = e;
	}
| T_LBOX T_AT cryptopoint T_DEST T_LBRACE cryptopoints T_RBRACE T_RBOX
	{
		set_current_position(@$);
		semval e(new Annotation(true, $3, $6));
		$$ = e;
	}
anno_orig:
	/* empty */
	{
		set_current_position(@$);
		semval e(new Annotation(false));
		$$ = e;
	}
| T_LBOX T_AT cryptopoint T_RBOX
	{
		set_current_position(@$);
		semval e(new Annotation(false, $3));
		$$ = e;
	}
| T_LBOX T_AT cryptopoint T_ORIG T_LBRACE cryptopoints T_RBRACE T_RBOX
	{
		set_current_position(@$);
		semval e(new Annotation(false, $3, $6));
		$$ = e;
	}

cryptopoint:
	T_IDENT subscript_indices
	{ 
		set_current_position(@$);
		semval e(new Cryptopoint($1, $2));
		$$ = e;
	}
|	T_IDENT 
	{ 
		set_current_position(@$);
		semval e(new Cryptopoint($1));
		$$ = e;
	}

cryptopoints:
	cryptopoint
	{
		set_current_position(@$);
		semval e(new Cryptopoints($1));
		$$ = e;
	}
|	cryptopoints T_COMMA cryptopoint
	{
		dynamic_pointer_cast<Cryptopoints>($1)->push_back($3);
		$$ = $1;
	}

/*** IDENTIFIERS ***/
name:
	T_IDENT subscript_indices
	{ 
		set_current_position(@$);
		semval e(new Name($1, $2));
		$$ = e;
	}
|	T_IDENT 
	{ 
		set_current_position(@$);
		semval e(new Name($1));
		$$ = e;
	}
|	T_IDENT T_PLUS_OR_MINUS subscript_indices
	{ 
		set_current_position(@$);
		bool isPlus = static_pointer_cast<String>($2)->s=="+";
		semval e(new ASymName($1, $3, isPlus));
		$$ = e;
	}
|	T_IDENT T_PLUS_OR_MINUS 
	{ 
		set_current_position(@$);
		bool isPlus = static_pointer_cast<String>($2)->s=="+";
		semval e(new ASymName($1, isPlus));
		$$ = e;
	}
|	T_IDENT subscript_indices T_COLON T_TYPENAME
	{ 
		set_current_position(@$);
		std::string t = static_pointer_cast<String>($4)->s;
		semval e(new TypedVar($1, $2, (t=="C") ? TypedVar::V_Ciphertext : TypedVar::V_Name));
		$$ = e;
	}
|	T_IDENT T_COLON T_TYPENAME
	{ 
		set_current_position(@$);
		std::string t = static_pointer_cast<String>($3)->s;
		semval e(new TypedVar($1, (t=="C") ? TypedVar::V_Ciphertext : TypedVar::V_Name));

		$$ = e;
	}
subscript_indices:
	T_LBRACE_SUBSCRIPT indices T_RBRACE
	{
		$$ = $2;
	}
|	T_UNDERSCORE T_ALNUM
	{
		set_current_position(@2);
		semval e(new Indices($2, true));
		$$ = e;
	}
|	T_UNDERSCORE T_IDENT
	{
		set_current_position(@2);
		semval e(new Indices($2, true));
		$$ = e;
	}
|	T_UNDERSCORE T_NUM
	{
		set_current_position(@2);
		semval e(new Indices($2, true));
		$$ = e;
	}

/*** META-VARIABLES ***/
indices:
	index
	{
		set_current_position(@$);
		semval e(new Indices($1));
		$$ = e;
	}
|	indices T_COMMA index
	{
		static_pointer_cast<Indices>($1)->push_back(static_pointer_cast<String>($3)->s);
		semval e($1);
		$$ = e;
	}

index_defs:
	index_def
	{
		set_current_position(@$);
		semval e(new IndexDefs($1));
		$$ = e;
	}
|	index_defs T_COMMA index_def
	{
		dynamic_pointer_cast<IndexDefs>($1)->push_back($3);
		$$ = $1;
	}
index_def:
	T_IDENT T_IN T_IDENT
	{
		set_current_position(@$);
		semval e(new IndexDef($1, $3));
		$$ = e;
	}
|	T_IDENT T_IN iset
	{
		set_current_position(@$);
		semval empty(new String(""));
		semval e(new IndexDef($1, empty, $3));
		$$ = e;
	}
|	T_IDENT T_IN T_IDENT T_UNION iset
	{
		set_current_position(@$);
		semval e(new IndexDef($1, $3, $5));
		$$ = e;
	}
index:
	T_IDENT | T_NUM

iset:
	T_LBRACE indices T_RBRACE
	{
		set_current_position(@$);
		semval e(new IsetIndices($2));
		$$ = e;
	}
|	iset T_UNION iset
	{
		set_current_position(@$);
		semval e(new IsetUnion($1, $3));
		$$ = e;
	}
|	T_ISET_DEF
	{
		set_current_position(@$);
		semval e(new IsetDef($1));
		$$ = e;
	}
;


%%

//no local functions definitions.