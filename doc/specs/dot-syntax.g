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

${declare longest_match}

//--- Dot syntax

graph : 'strict/i'? ('graph/i' | 'digraph/i') ID? '{' stmt_list '}' ;

stmt_list : (stmt ';'? stmt_list?)? ;

stmt
  : node_stmt
	| edge_stmt
	| attr_stmt
	| ID '=' ID
	| subgraph
	;

attr_stmt	:	('graph/i' | 'node/i' | 'edge/i') attr_list ;

attr_list :	'[' a_list? ']' attr_list? ;

a_list : ID ('=' ID)? ','? a_list? ;

edge_stmt	:	(node_id | subgraph) edgeRHS attr_list? ;

edgeRHS : edgeop (node_id | subgraph) edgeRHS? ;

node_stmt : node_id attr_list? ;

node_id : ID port? ;

port
  : ':' ID (':' compass_pt)?
  | ':' compass_pt
  ;

subgraph : ('subgraph/i' ID?)? '{' stmt_list '}' ;

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

quoted : "\"[^\"]*\"" ;
name : "[a-zA-Z_][a-zA-Z0-9_]*" ;
number : "[-]?((\.[0-9]+)|([0-9]+(\.[0-9]+)?))" ;

//--- Whitespace

whitespace: "[ \t\r\n]*" ;
