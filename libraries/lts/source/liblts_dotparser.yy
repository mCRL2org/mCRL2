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

%token DIGRAPH GRAPH STRICT SUBGRAPH NODE EDGE IS COMMA COLON SEMICOLON LBRACE RBRACE LBRACK RBRACK ARROW
%token <aterm> ID
%type  <aterm> attr_list a_list node_id edge_rhs

%%

dot_file : { state2id = ATindexedSetCreate(10000,50); label2id = ATindexedSetCreate(100,50); }
           digraph
           ;

digraph :  DIGRAPH id_or_empty LBRACE stmt_list_or_empty RBRACE
           { ATindexedSetDestroy(state2id); ATindexedSetDestroy(label2id); }
        |  STRICT DIGRAPH id_or_empty LBRACE stmt_list_or_empty RBRACE
           { ATindexedSetDestroy(state2id); ATindexedSetDestroy(label2id); }
        |  GRAPH id_or_empty LBRACE stmt_list_or_empty RBRACE
           { ATindexedSetDestroy(state2id); ATindexedSetDestroy(label2id); }
        |  STRICT GRAPH id_or_empty LBRACE stmt_list_or_empty RBRACE
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
     | SUBGRAPH LBRACE stmt_list RBRACE
     | SUBGRAPH ID LBRACE stmt_list RBRACE
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

edge_stmt : node_id edge_rhs                 { dot_add_transition(dot_state($1,NULL),ATgetArity(ATgetAFun($2))>1?(ATermAppl) ATgetArgument($2,1):NULL,dot_state((ATermAppl) ATgetArgument($2,0),NULL)); }
          ;

edge_rhs : ARROW node_id                 { safe_assign($$,ATmakeAppl1(ATmakeAFun("singleton",1,ATfalse),(ATerm) $2)); }
         | ARROW node_id attr_list       { if ( $3 == NULL ) { safe_assign($$,ATmakeAppl1(ATmakeAFun("singleton",1,ATfalse),(ATerm) $2)); } else { safe_assign($$,ATmakeAppl2(ATmakeAFun("pair",2,ATfalse),(ATerm) $2,(ATerm) $3)); } }
         | ARROW node_id edge_rhs        { dot_add_transition(dot_state($2,NULL),ATgetArity(ATgetAFun($3))>1?(ATermAppl) ATgetArgument($3,1):NULL,dot_state((ATermAppl) ATgetArgument($3,0),NULL)); safe_assign($$,ATsetArgument($3,(ATerm) $2,0)); }
         ;

node_stmt : node_id                  { dot_state($1,NULL); }
          | node_id attr_list        { dot_state($1,$2); }
          ;

node_id : ID                         { safe_assign($$,$1); }
        | ID COLON ID                { safe_assign($$,$1); }
        | ID COLON ID COLON ID       { safe_assign($$,$1); }

%%

static void dot_add_transition(int from, ATermAppl label, int to)
{
  if ( label == NULL )
  {
    label = ATmakeAppl0(ATmakeAFun("",0,ATtrue));
  }

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
