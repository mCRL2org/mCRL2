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

{
#include "d.h"
}

// Sort Expressions

SortExpr
  : 'Bool'
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

SortDecl
  : IdList ';'
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

GlobVarSpec: 'glob' ( IdsDeclList ';' )+ ;

VarSpec: 'var' ( IdsDeclList ';' )+ ;

EqnSpec: VarSpec? 'eqn' EqnDecl+ ;

EqnDecl
  : DataExpr '=' DataExpr ';'
  | DataExpr '->' DataExpr '=' DataExpr ';'
  ;

// Data expressions

DataExpr
  : Id
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
  | dataexpr_unary_operator DataExpr
  | dataexpr_quantifier DataExpr
  | DataExpr dataexpr_binary_operator DataExpr
  | DataExpr 'whr' WhrExprList 'end'
  ;

dataexpr_disambiguation
  : '(' DataExpr ')'
  | 'true'
  | 'false'
  | Id
  | Number
  | dataexpr_disambiguation '(' DataExprList ')'
  | dataexpr_unary_operator dataexpr_disambiguation
  ;

dataexpr_unary_operator
  : '!'       $unary_op_right 11
  | '-'       $unary_op_right 11
  | '#'       $unary_op_right 11
  ;

dataexpr_quantifier
  : 'forall' IdsDeclList '.' $unary_op_right 0
  | 'exists' IdsDeclList '.' $unary_op_right 0
  | 'lambda' IdsDeclList '.' $unary_op_right 0
  ;

dataexpr_binary_operator
  : '=>'      $binary_op_right 1
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

ProcExpr
  : Action
  | 'delta'
  | 'tau'
  | 'block' '(' MAIdSet ',' ProcExpr ')'
  | 'allow' '(' MAIdSet ',' ProcExpr ')'
  | 'hide' '(' MAIdSet ',' ProcExpr ')'
  | 'rename' '(' RenExprSet ',' ProcExpr ')'
  | 'comm' '(' CommExprSet ',' ProcExpr ')'
  | '(' ProcExpr ')'
  | ProcExpr procexpr_binary_operator ProcExpr
  | ProcExpr procexpr_time_operator dataexpr_disambiguation
  | procexpr_unary_operator ProcExpr
  | dataexpr_disambiguation procexpr_thenelse $unary_left 11;
  ;

procexpr_thenelse
  : '->' ProcExpr ('<>' ProcExpr)? $unary_op_left 11;

procexpr_unary_operator
  : 'sum' IdsDeclList '.'     $unary_op_right 2
  ;

procexpr_binary_operator
  : '+'   $binary_op_right 1
  | '||'  $binary_op_right 3
  | '||_' $binary_op_right 3
  | '.'   $binary_op_right 6
  | '<<'  $binary_op_left 5
  | '|'   $binary_op_right 8
  ;

procexpr_time_operator: '@'       $binary_op_left 7 ;

// Actions

Action: Id ( '(' DataExprList ')' )? ;

ActDecl: IdList ( ':' Domain )? ';' ;

ActSpec: 'act' ActDecl+ ;

MultAct: 'tau'
       | ActionList
       ;

ActionList: Action ( '|' Action )* ;

// Process and initial state declaration

ProcDecl: Id ( '(' IdsDeclList ')' )? '=' ProcExpr ';' ;

ProcSpec: 'proc' ProcDecl+ ;

Init: 'init' ProcExpr ';' ;

// Data specification

DataSpec: ( SortSpec | OpSpec | EqnSpec )+ ;

// mCRL2 specification

mCRL2Spec: ( SortSpec | OpSpec | EqnSpec | GlobVarSpec | ActSpec | ProcSpec | Init )+ ;

// BES

BesSpec: BesEqnSpec BesInit ;

BesEqnSpec: 'bes' BesEqnDecl+ ;

BesEqnDecl: FixedPointOperator BesVar '=' BesExpr ';' ;

BesVar: Id ;

BesExpr
  : 'true'
  | 'false'
  | besexpr_unary_operator BesExpr
  | BesExpr besexpr_binary_operator BesExpr
  | BesVar
  ;

BesInit: 'init' BesVar ';' ;

besexpr_unary_operator: '!' $unary_op_left 4 ;

besexpr_binary_operator
  : '&&'      $binary_op_right 3
  | '||'      $binary_op_right 3
  | '=>'      $binary_op_right 2
  ;

// PBES

PbesSpec: DataSpec? GlobVarSpec? PbesEqnSpec PbesInit ;

PbesEqnSpec: 'pbes' PbesEqnDecl+ ;

PbesEqnDecl: FixedPointOperator PropVarDecl '=' PbesExpr ';' ;

FixedPointOperator
  : 'mu'
  | 'nu'
  ;

PropVarDecl: Id ( "(" IdsDeclList ")" )? ;

PropVarInst: Id ( "(" DataExprList ")" )? ;

PbesInit: 'init' PropVarInst ';' ;

DataValExpr: 'val' '(' DataExpr ')' ;

PbesExpr
  : DataValExpr
  | 'true'
  | 'false'
  | 'forall' IdsDeclList pbesexpr_quantifier_operator PbesExpr
  | 'exists' IdsDeclList pbesexpr_quantifier_operator PbesExpr
  | pbesexpr_unary_operator PbesExpr
  | PbesExpr pbesexpr_binary_operator PbesExpr
  | PropVarInst
  ;

pbesexpr_unary_operator: '!' $unary_op_left 4 ;

pbesexpr_binary_operator
  : '&&'      $binary_op_right 3
  | '||'      $binary_op_right 3
  | '=>'      $binary_op_right 2
  ;

pbesexpr_quantifier_operator: '.' $unary_op_left 1 ;

// Action formulas

ActFrm
  : MultAct
  | DataValExpr
  | 'true'
  | 'false'
  | actfrm_unary_operator ActFrm
  | ActFrm actfrm_binary_operator ActFrm
  | 'forall' IdsDeclList actfrm_quantifier_operator ActFrm
  | 'exists' IdsDeclList actfrm_quantifier_operator ActFrm
  | ActFrm acttime_operator DataExpr
  | '(' ActFrm ')'
  ;

actfrm_unary_operator: '!' $unary_op_left 5 ;

actfrm_binary_operator
  : '&&'      $binary_op_right 3
  | '||'      $binary_op_right 3
  | '=>'      $binary_op_right 2
  ;

acttime_operator: '@' $unary_op_left 4 ;

actfrm_quantifier_operator: '.' $unary_op_left 1 ;

// Regular formulas

RegFrm
  : ActFrm
  | 'nil'
  | RegFrm regfrm_binary_operator RegFrm
  | RegFrm regfrm_unary_operator
  | '(' RegFrm ')'
  ;

regfrm_unary_operator
  : '*'      $unary_op_left 3
  | '+'      $unary_op_left 3
  ;

regfrm_binary_operator
  : '.'      $binary_op_right 1
  | '+'      $binary_op_right 2
  ;

// State formulas

StateFrm
  : DataValExpr
  | 'true'
  | 'false'
  | statefrm_unary_operator StateFrm
  | StateFrm statefrm_binary_operator StateFrm
  | 'forall' IdsDeclList statefrm_quantifier_operator StateFrm
  | 'exists' IdsDeclList statefrm_quantifier_operator StateFrm
  | '[' RegFrm ']'
  | '<' RegFrm '>'
  | FixedPointOperator StateVarDecl statefrm_fixedpoint_operator StateFrm
  | StateVarInst
  | 'delay' ( '@' DataExpr )?
  | 'yaled' ( '@' DataExpr )?
  | '(' StateFrm ')'
  ;

StateVarDecl: Id ( "(" IdsDeclList ")" )? ;

StateVarInst: Id ( "(" DataExprList ")" )? ;

statefrm_unary_operator : '!' $unary_op_left 5 ;

statefrm_binary_operator
  : '&&'      $binary_op_right 4
  | '||'      $binary_op_right 4
  | '=>'      $binary_op_right 3
  ;

statefrm_quantifier_operator: '.' $unary_op_left 2 ;

statefrm_fixedpoint_operator : '.' $unary_op_left 1 ;

// Identifiers

IdList: Id ( ',' Id )* ;

{
  char *reserved_words[] = {
    "sort"  ,
    "cons"  ,
    "map"   ,
    "var"   ,
    "eqn"   ,
    "act"   ,
    "proc"  ,
    "init"  ,
    "delta" ,
    "tau"   ,
    "sum"   ,
    "block" ,
    "allow" ,
    "hide"  ,
    "rename",
    "comm"  ,
    "struct",
    "Bool"  ,
    "Pos"   ,
    "Nat"   ,
    "Int"   ,
    "Real"  ,
    "List"  ,
    "Set"   ,
    "Bag"   ,
    "true"  ,
    "false" ,
    "whr"   ,
    "end"   ,
    "lambda",
    "forall",
    "exists",
    "div"   ,
    "mod"   ,
    "in"    ,
    NULL
  };
  static int is_one_of(char *s, char **list) {
    while (*list) { if (!strcmp(s, *list)) return 1; list++; }
    return 0;
  }
}

Id: "[A-Za-z_][A-Za-z_0-9']*" $term -2
[
  char *ts = dup_str($n0.start_loc.s, $n0.end);
  if (is_one_of(ts, reserved_words)) {
    d_free(ts);
    ${reject};
  }
  d_free(ts);
];

Number: "0|(-?[1-9][0-9]*)" ;

// Whitespace

whitespace: "([ \t\n]|(%[^\n]*\n))*" ;

