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
  | DataExprUnaryOperator DataExpr
  | DataExprQuantifier DataExpr
  | DataExpr DataExprBinaryOperator DataExpr
  | DataExpr 'whr' WhrExprList 'end'
  ;

DataExprUnit
  : '(' DataExpr ')'
  | 'true'
  | 'false'
  | Id
  | Number
  | DataExprUnit '(' DataExprList ')'
  | DataExprUnaryOperator DataExprUnit
  ;

DataExprUnaryOperator
  : '!'       $unary_op_right 11
  | '-'       $unary_op_right 11
  | '#'       $unary_op_right 11
  ;

DataExprQuantifier
  : 'forall' IdsDeclList '.' $unary_op_right 0
  | 'exists' IdsDeclList '.' $unary_op_right 0
  | 'lambda' IdsDeclList '.' $unary_op_right 0
  ;

DataExprBinaryOperator
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

MultActId: ActionLabelList ;

ActionLabelList: Id ( '|' Id )* ;

MultActIdList: MultActId ( ',' MultActId )* ;

MultActIdSet: '{' MultActIdList? '}' ;

CommExpr: MultActId ( '->' Id )? ;

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
  | 'block' '(' MultActIdSet ',' ProcExpr ')'
  | 'allow' '(' MultActIdSet ',' ProcExpr ')'
  | 'hide' '(' MultActIdSet ',' ProcExpr ')'
  | 'rename' '(' RenExprSet ',' ProcExpr ')'
  | 'comm' '(' CommExprSet ',' ProcExpr ')'
  | '(' ProcExpr ')'
  | ProcExpr ProcExprBinaryOperator ProcExpr
  | ProcExpr ProcExprTimeOperator DataExprUnit
  | ProcExprUnaryOperator ProcExpr
  | DataExprUnit ProcExprThenElse $unary_left 11;
  ;

ProcExprThenElse
  : '->' ProcExpr ('<>' ProcExpr)? $unary_op_left 11;

ProcExprUnaryOperator
  : 'sum' IdsDeclList '.'     $unary_op_right 2
  ;

ProcExprBinaryOperator
  : '+'   $binary_op_right 1
  | '||'  $binary_op_right 3
  | '||_' $binary_op_right 3
  | '.'   $binary_op_right 6
  | '<<'  $binary_op_left 5
  | '|'   $binary_op_right 8
  ;

ProcExprTimeOperator: '@'       $binary_op_left 7 ;

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
  | BesExprUnaryOperator BesExpr
  | BesExpr BesExprBinaryOperator BesExpr
  | BesVar
  ;

BesInit: 'init' BesVar ';' ;

BesExprUnaryOperator: '!' $unary_op_left 4 ;

BesExprBinaryOperator
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
  | 'forall' IdsDeclList PbesExprQuantifierOperator PbesExpr
  | 'exists' IdsDeclList PbesExprQuantifierOperator PbesExpr
  | PbesExprUnaryOperator PbesExpr
  | PbesExpr PbesExprBinaryOperator PbesExpr
  | PropVarInst
  ;

PbesExprUnaryOperator: '!' $unary_op_left 4 ;

PbesExprBinaryOperator
  : '&&'      $binary_op_right 3
  | '||'      $binary_op_right 3
  | '=>'      $binary_op_right 2
  ;

PbesExprQuantifierOperator: '.' $unary_op_left 1 ;

// Action formulas

ActFrm
  : MultAct
  | DataValExpr
  | 'true'
  | 'false'
  | ActFrmUnaryOperator ActFrm
  | ActFrm ActFrmBinaryOperator ActFrm
  | 'forall' IdsDeclList ActFrmQuantifierOperator ActFrm
  | 'exists' IdsDeclList ActFrmQuantifierOperator ActFrm
  | ActFrm ActFrmTimeOperator DataExpr
  | '(' ActFrm ')'
  ;

ActFrmUnaryOperator: '!' $unary_op_left 5 ;

ActFrmBinaryOperator
  : '&&'      $binary_op_right 3
  | '||'      $binary_op_right 3
  | '=>'      $binary_op_right 2
  ;

ActFrmTimeOperator: '@' $unary_op_left 4 ;

ActFrmQuantifierOperator: '.' $unary_op_left 1 ;

// Regular formulas

RegFrm
  : ActFrm
  | 'nil'
  | RegFrm RegFrmBinaryOperator RegFrm
  | RegFrm RegFrmUnaryOperator
  | '(' RegFrm ')'
  ;

RegFrmUnaryOperator
  : '*'      $unary_op_left 3
  | '+'      $unary_op_left 3
  ;

RegFrmBinaryOperator
  : '.'      $binary_op_right 1
  | '+'      $binary_op_right 2
  ;

// State formulas

StateFrm
  : DataValExpr
  | 'true'
  | 'false'
  | StateFrmUnaryOperator StateFrm
  | StateFrm StateFrmBinaryOperator StateFrm
  | 'forall' IdsDeclList StateFrmQuantifierOperator StateFrm
  | 'exists' IdsDeclList StateFrmQuantifierOperator StateFrm
  | '[' RegFrm ']'
  | '<' RegFrm '>'
  | FixedPointOperator StateVarDecl StateFrmFixedPointOperator StateFrm
  | StateVarInst
  | 'delay' ( '@' DataExpr )?
  | 'yaled' ( '@' DataExpr )?
  | '(' StateFrm ')'
  ;

StateVarDecl: Id ( "(" IdsDeclList ")" )? ;

StateVarInst: Id ( "(" DataExprList ")" )? ;

StateFrmUnaryOperator : '!' $unary_op_left 5 ;

StateFrmBinaryOperator
  : '&&'      $binary_op_right 4
  | '||'      $binary_op_right 4
  | '=>'      $binary_op_right 3
  ;

StateFrmQuantifierOperator: '.' $unary_op_left 2 ;

StateFrmFixedPointOperator : '.' $unary_op_left 1 ;

// Action Rename Specifications

ActionRenameSpec: (SortSpec | OpSpec | EqnSpec | ActSpec | ActionRenameSpec)+ ;

ActionRenameSpec: VarSpec? 'rename' ActionRenameRule+ ;

ActionRenameRule: (DataExpr '->')? Action '=>' ActionRenameRuleRHS ';' ;

ActionRenameRuleRHS: Action | 'tau' | 'delta' ;

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

