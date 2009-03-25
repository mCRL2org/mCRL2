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
ATermAppl emptystring;
AFun singleton_fun, pair_fun, no_incoming_fun, has_incoming_fun;


// Function declarations
static void dot_add_transition(int from, ATermAppl label, int to);
static void dot_add_transitions(ATermList states, ATermAppl label);
static int dot_state(ATermAppl id, ATermAppl label);
static void set_has_incoming(int state);

#define safe_assign(lhs, rhs) { ATbool b; ATindexedSetPut(dot_lexer_obj->protect_table, (ATerm) rhs, &b); lhs = rhs; }

//external declarations from dotlexer.ll
void dotyyerror(const char *s);
int dotyylex(void);
%}

%union {
  ATermAppl aterm;
  ATermList atermlist;
}

//set name prefix
%name-prefix="dotyy"

%start dot_file

%token DIGRAPH GRAPH STRICT SUBGRAPH NODE EDGE IS COMMA COLON SEMICOLON LBRACE RBRACE LBRACK RBRACK ARROW
%token <aterm> ID
%type  <aterm> attr_list a_list node_id
%type  <atermlist> edge_list

%%

dot_file : { state2id = ATindexedSetCreate(10000,50);
             label2id = ATindexedSetCreate(100,50);
             emptystring = ATmakeAppl0(ATmakeAFun("",0,ATtrue));
             ATprotectAppl(&emptystring);
             singleton_fun = ATmakeAFun("singleton",1,ATfalse);
             ATprotectAFun(singleton_fun);
             pair_fun = ATmakeAFun("pair",2,ATfalse);
             ATprotectAFun(pair_fun);
             no_incoming_fun = ATmakeAFun("no_incoming",2,ATfalse);
             ATprotectAFun(no_incoming_fun);
             has_incoming_fun = ATmakeAFun("has_incoming",2,ATfalse);
             ATprotectAFun(has_incoming_fun);
           }
           digraph
           { ATindexedSetDestroy(state2id);
             ATindexedSetDestroy(label2id);
             ATunprotectAppl(&emptystring);
             ATunprotectAFun(singleton_fun);
             ATunprotectAFun(pair_fun);
             ATunprotectAFun(no_incoming_fun);
             ATunprotectAFun(has_incoming_fun);
           }
           ;

digraph :  DIGRAPH id_or_empty LBRACE stmt_list_or_empty RBRACE
        |  STRICT DIGRAPH id_or_empty LBRACE stmt_list_or_empty RBRACE
        |  GRAPH id_or_empty LBRACE stmt_list_or_empty RBRACE
        |  STRICT GRAPH id_or_empty LBRACE stmt_list_or_empty RBRACE
        ;

id_or_empty : ID
            |
            ;

stmt_list_or_empty : stmt_list
                   |
                   ;

stmt_list : stmt
          | stmt SEMICOLON
          | stmt_list stmt
          | stmt_list stmt SEMICOLON
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
          | LBRACK a_list RBRACK            { safe_assign($$,$2); }
          | attr_list LBRACK RBRACK         { safe_assign($$,$1); }
          | attr_list LBRACK a_list RBRACK  { if ( $1 != NULL ) { safe_assign($$,$1); } else { safe_assign($$,$3); } }
          ;

a_list : ID                     { safe_assign($$,NULL); }
       | a_list ID              { safe_assign($$,$1); }
       | ID COMMA               { safe_assign($$,NULL); }
       | a_list ID COMMA        { safe_assign($$,$1); }
       | ID IS ID               { if ( !strcmp(ATgetName(ATgetAFun($1)),"label") ) { safe_assign($$,$3); } else { safe_assign($$,NULL); } }
       | a_list ID IS ID        { if ( !strcmp(ATgetName(ATgetAFun($2)),"label") ) { safe_assign($$,$4); } else { safe_assign($$,$1); } }
       | ID IS ID COMMA         { if ( !strcmp(ATgetName(ATgetAFun($1)),"label") ) { safe_assign($$,$3); } else { safe_assign($$,NULL); } }
       | a_list ID IS ID COMMA  { if ( !strcmp(ATgetName(ATgetAFun($2)),"label") ) { safe_assign($$,$4); } else { safe_assign($$,$1); } }

edge_stmt : edge_list
                     { dot_add_transitions($1,NULL); }
          | edge_list attr_list
                     { dot_add_transitions($1,$2); }
          ;

edge_list : node_id ARROW node_id   { safe_assign($$,ATmakeList2((ATerm) $3,(ATerm) $1)); }
          | edge_list ARROW node_id { safe_assign($$,ATinsert($1,(ATerm) $3)); }
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
    label = emptystring;
  }

  ATbool b;
  int idx = ATindexedSetPut(label2id,(ATerm) label,&b);

  if ( b == ATtrue )
  {
    dot_lexer_obj->dot_lts->add_label((ATerm) label,!strcmp(ATgetName(ATgetAFun(label)),"tau"));
  }

  set_has_incoming(to);
  dot_lexer_obj->dot_lts->add_transition(from,idx,to);
}

static void dot_add_transitions(ATermList states, ATermAppl label)
{
  states = ATreverse(states); // to make sure that the first state in the file is added first and thus gets id 0
  ATermAppl from = (ATermAppl) ATgetFirst(states);
  states = ATgetNext(states);
  for (; !ATisEmpty(states); states=ATgetNext(states))
  {
    ATermAppl to = (ATermAppl) ATgetFirst(states);
    dot_add_transition(dot_state(from,NULL),label,dot_state(to,NULL));
    from = to;
  }
}

static int dot_state(ATermAppl id, ATermAppl label)
{
  ATbool b;
  int idx = ATindexedSetPut(state2id,(ATerm) id,&b);

  if ( b == ATtrue )
  {
    dot_lexer_obj->dot_lts->add_state((ATerm) ATmakeAppl2(no_incoming_fun,(ATerm) id,(ATerm) emptystring));
  }
  if ( label != NULL )
  {
    ATermAppl oldval = (ATermAppl) dot_lexer_obj->dot_lts->state_value(idx);
    dot_lexer_obj->dot_lts->set_state(idx,(ATerm) ATsetArgument(oldval,(ATerm) label,1));
  }

  return idx;
}

static void set_has_incoming(int state)
{
  ATermAppl oldval = (ATermAppl) dot_lexer_obj->dot_lts->state_value(state);
  if ( ATisEqualAFun(no_incoming_fun,ATgetAFun(oldval)) )
  {
    dot_lexer_obj->dot_lts->set_state(state,(ATerm) ATmakeAppl2(has_incoming_fun,ATgetArgument(oldval,0),ATgetArgument(oldval,1)));
  }
}
