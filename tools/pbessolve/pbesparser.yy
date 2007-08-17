
%{
#include <iostream> 
#include <cstdio>
#include <string.h>
#include "pbes_simple.h"
#include "pbeslexer.cpp"

#define YYDEBUG 1

extern int yylex();
extern char* yytext;

extern t_pbes_simple ps;

//*******************************************

extern int pos;

void pbeserror(const char *str);
void add(char op,int first,int second);

%}

%union {
	int position; 
	char* string;
} 

%token IDPRED IDPROP IDDATA
%token TRUE FALSE ZERO ONE LESS IS PLUS
%token EN OF NEG COMMA
%token EQ
%token MU NU FORALL EXISTS
%token LPAR RPAR

%type <string> IDPRED IDPROP IDDATA
%type <position> EN OF NEG COMMA
%type <position> TRUE FALSE ZERO ONE LESS IS PLUS FORALL EXISTS
%type <position> EQ MU NU fixpoint pbes pbes_eq pbes_expression nat_expr bool_expr
%type <position> var_enum var_list expr_enum expr_list

%left PLUS
%nonassoc IS


%start pbes



%%

pbes:
  pbes_eq
| pbes_eq pbes


pbes_eq: 
  fixpoint EQ pbes_expression 
	{ add('Q', $1, $3); $$ = pos; pos++; }


fixpoint:
  MU IDPRED 
	{ add($2[0], 0, 0); pos++; 
  	  add('m',pos-1,0); $$ = pos; pos++; }
| NU IDPRED
	{ add($2[0], 0, 0); pos++; 
	  add('n',pos-1,0); $$ = pos; pos++; }
| MU IDPRED var_list
	{ add($2[0], $3, 0); pos++; 
	  add('m',pos-1,0); $$ = pos; pos++; }
| NU IDPRED var_list
	{ add($2[0], $3, 0); pos++; 
	  add('n',pos-1,0); $$ = pos; pos++; }


pbes_expression:
  LPAR pbes_expression RPAR
	{ $$ = $2;}
| TRUE 
	{ add('T', 0, 0); $$ = pos; pos++; }
| FALSE
	{ add('F', 0, 0); $$ = pos; pos++; }
| IDPROP
	{ add($1[0], 0, 0); $$ = pos; pos++; }
| IDPRED
	{ add($1[0], 0, 0); $$ = pos; pos++;}
| IDPRED expr_list
	{ add($1[0], $2, 0); $$ = pos; pos++; }
| nat_expr LESS nat_expr
	{ add('<', $1, $3); $$ = pos; pos++; }
| nat_expr IS nat_expr 
	{ add('=', $1, $3); $$ = pos; pos++; }
| pbes_expression EN pbes_expression
	{ add('&', $1, $3); $$ = pos; pos++; }
| pbes_expression OF pbes_expression
	{ add('|', $1, $3); $$ = pos; pos++; }
| NEG pbes_expression
	{ add('!', $2, 0); $$ = pos; pos++; }



nat_expr:
LPAR nat_expr RPAR
	{ $$ = $2; }
| ZERO
	{ add('0', 0, 0); $$ = pos; pos++; }
| ONE
	{ add('1', 0, 0); $$ = pos; pos++; }
| IDDATA
	{ add($1[0], 0, 0); $$ = pos; pos++; }
| nat_expr PLUS nat_expr
	{ add('+', $1, $3); $$ = pos; pos++; }


bool_expr:
TRUE 
	{ add('T', 0, 0); $$ = pos; pos++; }
| FALSE
	{ add('F', 0, 0); $$ = pos; pos++; }
| IDPROP
	{ add($1[0], 0, 0); $$ = pos; pos++; }
| NEG bool_expr
	{ add('!', $2, 0); $$ = pos; pos++; }
| bool_expr EN bool_expr
	{ add('&', $1, $3); $$ = pos; pos++; }
| bool_expr OF bool_expr
	{ add('|', $1, $3); $$ = pos; pos++; }



var_enum:
IDDATA 
	{ add($1[0], 0, 0); $$ = pos; pos++;}
| IDPROP
	{ add($1[0], 0, 0); $$ = pos; pos++;}
| IDDATA COMMA var_enum
	{ add($1[0], 0, 0); pos++; 
	  add(',',pos-1,$3); $$ = pos; pos++;}
| IDPROP COMMA var_enum
	{ add($1[0], 0, 0); pos++; 
	  add(',',pos-1,$3); $$ = pos; pos++;} 



var_list:
LPAR var_enum RPAR
	{ $$ = $2; }



expr_enum:
nat_expr COMMA expr_enum
	{ add(',',$1,$3); $$ = pos; pos++; }
| bool_expr COMMA expr_enum
	{ add(',',$1,$3); $$ = pos; pos++; }
| nat_expr
	{ $$=$1; }
| bool_expr
	{ $$=$1; }





expr_list:
LPAR expr_enum RPAR
	{ $$ = $2; }





%%








void add(char op, int first, int second)
{
  ps->op[pos] = op;
  ps->arg1[pos] = first;
  ps->arg2[pos] = second;  
  ps->nops++;
}



void pbeserror(const char *str) {
        std::cout << str << ": " << yytext << "\n";
	exit(1);
 //       throw oss.str();
}



void parsePBES( std::string fileName ) {
  FILE* infile = fopen(fileName.c_str(),"r");
  if (infile == NULL) {
      	throw std::string("Cannot open file for reading:\n" + fileName);
       }
  else 	{

    pbesin = infile;

    pbesparse();

  }
}
