// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file fsm-syntax.g
/// \brief dparser grammar of the FSM format

${declare tokenize}
${declare longest_match}

FSM : ParameterList Separator StateList Separator TransitionList ;

Separator: '---' EOLN ;

ParameterList : Parameter* ;

Parameter : ParameterName '(' DomainCardinality ')' DomainName DomainValueList EOLN ;

ParameterName: Id ;

DomainCardinality : Number ;

DomainName: Id ;

DomainValueList : DomainValue* ;

DomainValue: QuotedString ;

StateList: State* ;

State: Number* EOLN ;

TransitionList: Transition* ;

Transition: Source Target Label EOLN ;

Source: Number ;

Target: Number ;

Label: QuotedString ;

//--- Identifiers

QuotedString: "\"[^\"]*\"" $term -1 ;

Id: "[A-Za-z_][A-Za-z_0-9']*" $term -1 ;

Number: "0|([1-9][0-9]*)" $term -1 ;

//--- Whitespace

EOLN: "\r?\n" ;

whitespace: "[ \t]*" ;
