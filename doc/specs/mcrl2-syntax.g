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
        | 'struct' ConstrDeclList
        ;

Domain: SortExprList ;

SortExprList: SortExpr ( '#' SortExpr )* ;

SortSpec: 'sort' SortDecl+ ;

SortDecl: IdList ';'
        | IdList '=' SortExpr ';'
        ;

ConstrDecl: Id ( '(' ProjDeclList ')' )? ( '?' Id )? ;

ConstrDeclList: ConstrDecl ( '|' ConstrDecl )* ;

ProjDecl: ( Id ':' )? Domain ;

ProjDeclList: ProjDecl ( ',' ProjDecl )* ;

// Constructors and mappings

IdDecl: Id ':' SortExpr ;

IdsDecl: IdList ':' SortExpr ;

IdsDeclList: IdsDecl ( ',' IdsDecl )* ;

OpSpec: ( 'cons' | 'map' ) ( OpDecl ';' )+ ;

OpDecl: IdsDecl ;

// Equations

VarSpec: 'var' ( IdsDeclList ';' )+ ;

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
        | '[' DataExprList ']'
        | '{' BagEnumEltList '}'
        | '{' IdDecl '|' DataExpr '}'
        | '{' DataExprList '}'
        | '(' DataExpr ')'
        | DataExpr '(' DataExprList ')'
        | '!' DataExpr
        | '-' DataExpr
        | '#' DataExpr
        | 'forall' IdsDeclList '.' DataExpr
        | 'exists' IdsDeclList '.' DataExpr
        | 'lambda' IdsDeclList '.' DataExpr
        | DataExpr dataexpr_operator DataExpr
        | DataExpr 'whr' WhrExprList 'end'
        ;

dataexpr_operator: '=>'      $binary_op_right 1
                 | '&&'      $binary_op_right 2
                 | '||'      $binary_op_right 2
                 | '=='      $binary_op_right 3
                 | '!='      $binary_op_right 3
                 | '<'       $binary_op_left 4
                 | '<='      $binary_op_left 4
                 | '>='      $binary_op_left 4
                 | '>'       $binary_op_left 4
                 | 'in'      $binary_op_left 4
                 | '|>'      $binary_op_left 5
                 | '<|'      $binary_op_left 6
                 | '++'      $binary_op_left 7
                 | '+'       $binary_op_left 8
                 | '-'       $binary_op_left 8
                 | '/'       $binary_op_left 9
                 | 'div'     $binary_op_left 9
                 | 'mod'     $binary_op_left 9
                 | '*'       $binary_op_left 10
                 | '.'       $binary_op_left 10
                 ;

WhrExpr: DataExpr '=' DataExpr ;

WhrExprList: WhrExpr ( ',' WhrExpr )* ;

DataExprList: DataExpr ( ',' DataExpr )* ;

BagEnumElt: DataExpr ':' DataExpr ;

BagEnumEltList: BagEnumElt ( ',' BagEnumElt )* ;

// Communication and renaming sets

MAId: ActionLabelList ;

ActionLabelList: Id ( '|' Id )* ;

MAIdList: MAId ( ',' MAId )* ;

MAIdSet: '{' MAIdList? '}' ;

CommExpr: MAId ( '->' Id )? ;

CommExprList: CommExpr ( ',' CommExpr )* ;

CommExprSet: '{' CommExprList? '}' ;

RenExpr: Id '->' Id ;

RenExprList: RenExpr ( ',' RenExpr )* ;

RenExprSet: '{' RenExprList? '}' ;

// Process expressions

ProcExpr: Id
        | Id '(' DataExprList ')'
        | 'delta'
        | 'tau'
        | 'sum' IdsDeclList sum_operator ProcExpr
        | 'block' '(' MAIdSet ',' ProcExpr ')'
        | 'allow' '(' MAIdSet ',' ProcExpr ')'
        | 'hide' '(' MAIdSet ',' ProcExpr ')'
        | 'rename' '(' RenExprSet ',' ProcExpr ')'
        | 'comm' '(' CommExprSet ',' ProcExpr ')'
        | '(' ProcExpr ')'
        | ProcExpr procexpr_operator ProcExpr
        | ProcExpr time_operator DataExpr
        | DataExpr if_operator ProcExpr
        | DataExpr if_operator ProcExpr '<>' ProcExpr
        ;

// The descending order of precedence of the operators is: "|", "@", ".", { "<<", ">>" }, "->", { "||", "||_" }, "sum", "+".

procexpr_operator: '+'   $binary_op_right 1
                 | '||'  $binary_op_right 3
                 | '||_' $binary_op_right 3
                 | '.'   $binary_op_right 6
                 | '<<'  $binary_op_left 5
                 | '|'   $binary_op_right 8
                 ;

time_operator: '@'       $binary_op_left 7 ;

sum_operator: '.'        $unary_op_left 2 ;

if_operator: '->'        $binary_op_right 4 ;

// Action declaration

ActDecl: IdList ( ':' Domain )? ';' ;

ActSpec: 'act' ActDecl+ ;

// Process and initial state declaration

ProcDecl: Id ( '(' IdsDeclList ')' )? '=' ProcExpr ';' ;

ProcSpec: 'proc' ProcDecl+ ;

Init: 'init' ProcExpr ';' ;

// Data specification

DataSpec: ( SortSpec | OpSpec | EqnSpec )+ ;

// mCRL2 specification

mCRL2Spec: ( SortSpec | OpSpec | EqnSpec | ActSpec | ProcSpec | Init )+ ;

// Identifiers

IdList: Id ( ',' Id )* ;

Id: "[A-Za-z_][A-Za-z_0-9']*" ;

Number: "0|(-?[1-9][0-9]*)" ;

// Whitespace

whitespace: "([ \t\n]|(%[^\n]*\n))*" ;

