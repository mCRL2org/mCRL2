// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file dot-syntax.g
/// \brief dparser grammar of the GraphViz Dot format
/// See also http://www.graphviz.org/doc/info/lang.html

//${declare tokenize}
//${declare longest_match}

//--- Dot syntax

dot : strict? (graph | digraph) ID? '{' stmt_list '}' ;

strict: "[sS][tT][rR][iI][cC][tT]" ;

graph: "[gG][rR][aA][pP][hH]" ;

subgraph_literal: "[sS][uU][bB][gG][rR][aA][pP][hH]" ;

digraph: "[dD][iI][gG][rR][aA][pP][hH]" ;

node: "[nN][oO][dD][eE]" ;

edge: "[eE][dD][gG][eE]" ;

// The official grammar is ambiguous, so we use an alternative formulation.
// stmt_list : (stmt ';'? stmt_list?)? ;
//
// The performance of the rule below is poor, so we use yet another formulation.
// stmt_list : (stmt ';'?)* ;

stmt_list
  : stmt_list2?;

stmt_list2
  : (stmt ';'?)
  | stmt_list2 (stmt ';'?);

stmt
  : node_stmt
	| edge_stmt
	| attr_stmt
	| attribute
	| subgraph
	;

attr_stmt	:	(graph | node | edge) attr_list ;

attr_list :	'[' a_list? ']' attr_list? ;

a_list : attribute ','? a_list? ;

attribute : ID '=' ID ;

edge_stmt	:	(node_id | subgraph) edgeRHS attr_list? ;

edgeRHS : edgeop (node_id | subgraph) edgeRHS? ;

node_stmt : node_id attr_list? ;

node_id : ID port? ;

port
  : ':' ID (':' compass_pt)?
  | ':' compass_pt
  ;

subgraph : (subgraph_literal ID?)? '{' stmt_list '}' ;

compass_pt : 'n' | 'ne' | 'e' | 'se' | 's' | 'sw' | 'w' | 'nw' | 'c' | '_' ;

edgeop
  : '--'
  | '->'
  ;

//--- Identifiers

ID
  : quoted
  | name
  | number
  ;

quoted : "\"([^\"]|\\\")*\"" ;

name : "[A-Za-z_][A-Za-z_0-9]*" $term -1;

number : "[-]?((\.[0-9]+)|([0-9]+(\.[0-9]+)?))" ;

//--- Whitespace

whitespace: "[ \t\r\n]*" ;
