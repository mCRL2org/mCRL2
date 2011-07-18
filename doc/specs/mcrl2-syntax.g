// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2-syntax.g
/// \brief dparser grammar of the mCRL2 language
//
// N.B. it is not complete yet!

// Sort Expressions

SortExpr: 'Bool'
        | 'Pos'
        | 'Nat'
        | 'Int'
        | 'Real'
        | 'List' '(' SortExpr ')'
        | 'Set' '(' SortExpr ')'
        | 'Bag' '(' SortExpr ')'
        | Id
        | '(' SortExpr ')'
        | Domain '->' SortExpr
        | 'struct' ConstrDecl ( '|' ConstrDecl )*
        ;

Domain: SortExpr ( '#' SortExpr )* ;

SortSpec: 'sort' SortDecl+ ;

SortDecl: Ids ';'
        | Ids '=' SortExpr ';'
        ;

ConstrDecl: Id ( '(' ProjDecls ')' )? ( '?' Id )? ;

ProjDecl: ( Id ':' )? Domain ;

ProjDecls: ProjDecl (',' ProjDecl)* ;

// Constructors and mappings

IdDecl: Id ':' SortExpr ;

IdsDecl: Ids ':' SortExpr ;

IdsDecls: IdsDecl ( ',' IdsDecl )* ;

OpSpec: ( 'cons' | 'map' ) ( OpDecl ';' )+ ;

OpDecl: IdsDecl ;

// Equations

VarSpec: 'var' ( IdsDecls ';' )+ ;

EqnSpec: VarSpec? 'eqn' EqnDecl+ ;

EqnDecl: DataExpr '=' DataExpr ';'
       | DataExpr '->' DataExpr '=' DataExpr ';'
       ;

// Data expressions

DataExpr: Id
        | Number
        | 'true'
        | 'false'
        | '[]'
        | '{}'
        | '[' DataExprs ']'
        | '{' BagEnumElts '}'
        | '{' IdDecl '|' DataExpr '}'
        | '{' DataExprs '}'
        | '(' DataExpr ')'
        | DataExpr '(' DataExprs ')'
        | '!' DataExpr
        | '-' DataExpr
        | '#' DataExpr
        | 'forall' IdsDecls '.' DataExpr
        | 'exists' IdsDecls '.' DataExpr
        | 'lambda' IdsDecls '.' DataExpr
        | DataExpr '.' DataExpr
        | DataExpr '*' DataExpr
        | DataExpr 'div' DataExpr
        | DataExpr 'mod' DataExpr
        | DataExpr '+' DataExpr
        | DataExpr '-' DataExpr
        | DataExpr '<' DataExpr
        | DataExpr '>' DataExpr
        | DataExpr '<=' DataExpr
        | DataExpr '>=' DataExpr
        | DataExpr 'in' DataExpr
        | DataExpr '|>' DataExpr
        | DataExpr '<|' DataExpr
        | DataExpr '++' DataExpr
        | DataExpr '==' DataExpr
        | DataExpr '!=' DataExpr
        | DataExpr '&&' DataExpr
        | DataExpr '||' DataExpr
        | DataExpr '=>' DataExpr
        | DataExpr 'whr' WhrExprs 'end'
        ;

WhrExpr: DataExpr '=' DataExpr ;

WhrExprs: WhrExpr ( ',' WhrExpr )* ;

DataExprs: DataExpr ( ',' DataExpr )* ;

BagEnumElt: DataExpr ':' DataExpr ;

BagEnumElts: BagEnumElt (',' BagEnumElt)* ;

// Communication and renaming sets

MAId: Id ( '|' Id )* ;

MAIdSet: '{' ( MAId ( ',' MAId )* )? '}' ;

CommExpr: MAId ( '->' Id )* ;

CommExprSet: '{' ( CommExpr ( ',' CommExpr )* )? '}' ;

RenExpr: Id '->' Id ;

RenExprSet: '{' ( RenExpr ( ',' RenExpr )* )? '}' ;

// Process expressions

ProcExpr: Id
        | Id '(' DataExprs ')'
        | 'delta'
        | 'tau'
        | 'sum' IdsDecls '.' ProcExpr
        | 'block' '(' MAIdSet ',' ProcExpr ')'
        | 'allow' '(' MAIdSet ',' ProcExpr ')'
        | 'hide' '(' MAIdSet ',' ProcExpr ')'
        | 'rename' '(' RenExprSet ',' ProcExpr ')'
        | 'comm' '(' CommExprSet ',' ProcExpr ')'
        | '(' ProcExpr ')'
        | ProcExpr '|' ProcExpr
        | ProcExpr '@' DataExpr
        | ProcExpr '.' ProcExpr
        | DataExpr '->' ProcExpr
        | DataExpr '->' ProcExpr '<>' ProcExpr
        | ProcExpr '<<' ProcExpr
        | ProcExpr '||' ProcExpr
        | ProcExpr '||_' ProcExpr
        | ProcExpr '+' ProcExpr
        ;

// Action declaration

ActDecl: Ids ( ':' Domain )? ';' ;

ActSpec: 'act' ActDecl+ ;

// Process and initial state declaration

ProcDecl: Id ( '(' IdsDecls ')' )? '=' ProcExpr ';' ;

ProcSpec: 'proc' ProcDecl+ ;

Init: 'init' ProcExpr ';' ;

// Data specification

DataSpec: ( SortSpec | OpSpec | EqnSpec )+ ;

// mCRL2 specification

mCRL2Spec: ( SortSpec | OpSpec | EqnSpec | ActSpec | ProcSpec | Init )+ ;

// Identifiers

Ids: Id ( ',' Id )* ;

Id: "[A-Za-z_][A-Za-z_0-9']*" ;

Number: "0|(-?[1-9][0-9]*)" ;

// Whitespace

whitespace: "([ \t\n]|(%[^\n]*\n))*" ;