// Author(s): Muck van Weerdenburg
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file dotparser.yy

%{

#define YYSTYPE std::string
#include <string>
#include <vector>
#include <cstring>
#include "liblts_dotparser.h"
#include "liblts_dotlexer.h"

// Function declarations
static void dot_add_transition(size_t from, const std::string &label_string, size_t to);
static void dot_add_transitions(const std::vector <std::string> &states, const std::string &label);
static size_t dot_state(const std::string &id, const std::string &label);

//external declarations from dotlexer.ll
void dotyyerror(const char *s);
int dotyylex(void);
%}

// %union {
//   ATermAppl aterm;
//   ATermList atermlist;
// }

//more verbose and specific error messages
%error-verbose

//set name prefix
%name-prefix="dotyy"

%start dot_file

%token DIGRAPH GRAPH STRICT SUBGRAPH NODE EDGE IS COMMA COLON SEMICOLON LBRACE RBRACE LBRACK RBRACK ARROW
%token ID
// %type  <aterm> attr_list a_list node_id
// %type  <atermlist> edge_list

%%

dot_file : 
           digraph
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

attr_list : LBRACK RBRACK                   { $$=""; }
          | LBRACK a_list RBRACK            { $$=$2; }
          | attr_list LBRACK RBRACK         { $$=$1; }
          | attr_list LBRACK a_list RBRACK  { if ( $1!="" ) { $$=$1; } else { $$=$3; } }
          ;

a_list : ID                     { $$=""; }
       | a_list ID              { $$=$1; }
       | ID COMMA               { $$=""; }
       | a_list ID COMMA        { $$=$1; }
       | ID IS ID               { if ($1=="label") { $$=$3; } else { $$=""; } }
       | a_list ID IS ID        { if ($2=="label") { $$=$4; } else { $$=$1; } }
       | ID IS ID COMMA         { if ($1=="label") { $$=$3; } else { $$=""; } }
       | a_list ID IS ID COMMA  { if ($2=="label") { $$=$4; } else { $$=$1; } }

edge_stmt :  
            edge_list
                     { dot_add_transitions(dot_lexer_obj->state_sequence,""); }
          | 
              
            edge_list attr_list
                     { dot_add_transitions(dot_lexer_obj->state_sequence,$2); }
          ;

edge_list : node_id ARROW node_id   { dot_lexer_obj->state_sequence.clear();
                                      dot_lexer_obj->state_sequence.push_back($1);
                                      dot_lexer_obj->state_sequence.push_back($3); 
                                    }
          | edge_list ARROW node_id { dot_lexer_obj->state_sequence.push_back($3); }
          ;

node_stmt : node_id                  { dot_state($1,""); }
          | node_id attr_list        { dot_state($1,$2); }
          ;

node_id : ID                         { $$=$1; }
        | ID COLON ID                { $$=$1; }
        | ID COLON ID COLON ID       { $$=$1; }

%%

static void dot_add_transition(size_t from, const std::string &label_string, size_t to)
{
  using namespace mcrl2::lts;

  std::map < std::string, size_t>::const_iterator label_index=dot_lexer_obj->labelTable.find(label_string);
  if (label_index==dot_lexer_obj->labelTable.end())
  { 
    // Not found. This label does not occur in the dot.
    const lts_dot_t::labels_size_type n=dot_lexer_obj->dot_lts->add_action(label_string,label_string=="tau");
    dot_lexer_obj->labelTable[label_string]=n;
    dot_lexer_obj->dot_lts->add_transition(transition(from,n,to));
  }
  else
  { 
    // The label is found. It already exists.
    dot_lexer_obj->dot_lts->add_transition(transition(from,label_index->second,to));
  }
}

static void dot_add_transitions(const std::vector <std::string> &state_sequence, const std::string &label)
{
  assert(!state_sequence.empty());
  std::vector <std::string>::const_iterator i=state_sequence.begin();
  std::string from_string = *i;
  for (++i; i!=state_sequence.end(); ++i)
  {
    const std::string to_string = *i;
    dot_add_transition(dot_state(from_string,""),label,dot_state(to_string,""));
    from_string = to_string;
  }
}

static size_t dot_state(const std::string &id, const std::string &label)
{
  using namespace mcrl2::lts::detail;
  size_t idx;

  std::map < std::string, size_t>::const_iterator state_index=dot_lexer_obj->stateTable.find(id);
  if (state_index==dot_lexer_obj->stateTable.end())
  { 
    // Not found. This state does not occur in the dot.
    idx=dot_lexer_obj->dot_lts->add_state(state_label_dot(id,label));
    dot_lexer_obj->stateTable[id]=idx;
  }
  else
  { 
    // The state name is found. It already exists. Overwrite it if label is non trivial.
    idx=state_index->second;
    if (label!="")
    { 
      dot_lexer_obj->dot_lts->set_state_label(idx, state_label_dot(id,label));
    }
  }
  return idx;
}
