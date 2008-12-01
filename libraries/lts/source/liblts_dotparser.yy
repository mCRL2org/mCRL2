// Author(s): Muck van Weerdenburg
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file dotparser.yy

%{
#include <vector>
#include "mcrl2/lts/lts.h"
#include "liblts_dotparser.h"
#include "liblts_dotlexer.h"

// Local variables
ATermIndexedSet state2id, label2id;

// Function declarations
static void dot_add_transition(int from, ATermAppl label, int to);
static int dot_state(ATermAppl id, ATermAppl label);

#define safe_assign(lhs, rhs) { ATbool b; ATindexedSetPut(dot_lexer_obj->protect_table, (ATerm) rhs, &b); lhs = rhs; }

//external declarations from dotlexer.ll
void dotyyerror(const char *s);
int dotyylex(void);
%}

%union {
  ATermAppl aterm;
}

//set name prefix
%name-prefix="dotyy"

%start dot_file

%token DIGRAPH GRAPH NODE EDGE IS COMMA SEMICOLON LBRACE RBRACE LBRACK RBRACK ARROW
%token <aterm> ID
%type  <aterm> attr_list a_list

%%

dot_file : { state2id = ATindexedSetCreate(10000,50); label2id = ATindexedSetCreate(100,50); }
           DIGRAPH id_or_empty LBRACE stmt_list_or_empty RBRACE
           { ATindexedSetDestroy(state2id); ATindexedSetDestroy(label2id); }
           ;

id_or_empty : ID
            |
            ;

stmt_list_or_empty : stmt_list
                   |
                   ;

stmt_list : stmt
          | stmt SEMICOLON
          | stmt stmt_list
          | stmt SEMICOLON stmt_list
          ;

stmt : node_stmt
     | edge_stmt
     | attr_stmt
     | ID IS ID
     ;

attr_stmt : GRAPH attr_list
          | NODE attr_list
          | EDGE attr_list
          ;

attr_list : LBRACK RBRACK                   { safe_assign($$,NULL); }
          | LBRACK RBRACK attr_list         { safe_assign($$,$3); }
          | LBRACK a_list RBRACK            { safe_assign($$,$2); }
          | LBRACK a_list RBRACK attr_list  { if ( $2 != NULL ) { safe_assign($$,$2); } else { safe_assign($$,$4); } }
          ;

a_list : ID                     { safe_assign($$,NULL); }
       | ID a_list              { safe_assign($$,$2); }
       | ID COMMA               { safe_assign($$,NULL); }
       | ID COMMA a_list        { safe_assign($$,$3); }
       | ID IS ID               { if ( !strcmp(ATgetName(ATgetAFun($$)),"label") ) { safe_assign($$,$3); } else { safe_assign($$,NULL); } }
       | ID IS ID a_list        { if ( !strcmp(ATgetName(ATgetAFun($$)),"label") ) { safe_assign($$,$3); } else { safe_assign($$,$4); } }
       | ID IS ID COMMA         { if ( !strcmp(ATgetName(ATgetAFun($$)),"label") ) { safe_assign($$,$3); } else { safe_assign($$,NULL); } }
       | ID IS ID COMMA a_list  { if ( !strcmp(ATgetName(ATgetAFun($$)),"label") ) { safe_assign($$,$3); } else { safe_assign($$,$5); } }

edge_stmt : ID ARROW ID                 { dot_add_transition(dot_state($1,NULL),NULL,dot_state($3,NULL)); }
          | ID ARROW ID attr_list       { dot_add_transition(dot_state($1,NULL),$4,dot_state($3,NULL)); }
          ;

node_stmt : ID                  { dot_state($1,NULL); }
          | ID attr_list        { dot_state($1,$2); }
          ;

%%

static void dot_add_transition(int from, ATermAppl label, int to)
{
  ATbool b;
  int idx = ATindexedSetPut(label2id,(ATerm) label,&b);

  if ( b == ATtrue )
  {
    dot_lexer_obj->dot_lts->add_label((ATerm) label,!strcmp(ATgetName(ATgetAFun(label)),"tau"));
  }

  dot_lexer_obj->dot_lts->add_transition(from,idx,to);
}

static int dot_state(ATermAppl id, ATermAppl label)
{
  ATbool b;
  int idx = ATindexedSetPut(state2id,(ATerm) id,&b);

  if ( b == ATtrue )
  {
    dot_lexer_obj->dot_lts->add_state((ATerm) id);
  }
  if ( label != NULL && ATisEqual(dot_lexer_obj->dot_lts->state_value(idx),id) )
  {
    dot_lexer_obj->dot_lts->set_state(idx,(ATerm) ATmakeAppl1(ATmakeAFun(ATgetName(ATgetAFun(id)),1,ATtrue),(ATerm) label));
  }

  return idx;
}
