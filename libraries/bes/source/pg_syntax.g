// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pg-syntax.g
/// \brief dparser grammar of the pg solver format for parity games

${declare longest_match}

// <parity_game> ::= [parity <identifier> ;] <node_spec>+
// <node_spec> ::= <identifier> <priority> <owner> <successors> [name] ;
// <identifier> ::= N
// <priority> ::= N
// <owner> ::= 0 | 1
// <successors> ::= <identifier> (, <identifier>)*
// <name> ::= " ( any ASCII string not containing `"') "
// In this N means a natural number.
// There must be whitespace characters between the following pairs of tokens:
// <identifier> and <priority>, <priority> and <owner>, <owner> and <identifier>

//--- pgsolver

ParityGame : ('parity' Id ';')? ('start' Id ';')? NodeSpecList ;

NodeSpecList : NodeSpec+ ;

NodeSpec : Id Priority Owner Successors Name? ';' ;

Id : Number ;

Priority : Number ;

Owner : "[01]" ;

Successors : Id (',' Id)* ;

Name : "\"[^\"]*\"" ;

Number: "0|([1-9][0-9]*)" $term -1 ;

//--- Whitespace

whitespace: "([ \t\n\r]|(%[^\n\r]*))*" ;
