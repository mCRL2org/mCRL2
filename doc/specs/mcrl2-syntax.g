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

ConsSpec: 'cons' ( IdsDecl ';' )+ ;

MapSpec: 'map' ( IdsDecl ';' )+ ;

// Equations

GlobVarSpec: 'glob' ( IdsDeclList ';' )+ ;

VarSpec: 'var' ( IdsDeclList ';' )+ ;

EqnSpec: VarSpec? 'eqn' EqnDecl+ ;

EqnDecl: (DataExpr '->')? DataExpr '=' DataExpr ';' ;

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
  | DataExpr '[' DataExpr '->' DataExpr ']'
  | DataExpr '(' DataExprList ')'
  | '!' DataExpr                             $unary_right 11
  | '-' DataExpr                             $unary_right 11
  | '#' DataExpr                             $unary_right 11
  | DataExprQuantifier DataExpr
  | DataExpr '=>'  DataExpr                  $binary_right 1
  | DataExpr '&&'  DataExpr                  $binary_right 2
  | DataExpr '||'  DataExpr                  $binary_right 2
  | DataExpr '=='  DataExpr                  $binary_right 3
  | DataExpr '!='  DataExpr                  $binary_right 3
  | DataExpr '<'   DataExpr                  $binary_left  4
  | DataExpr '<='  DataExpr                  $binary_left  4
  | DataExpr '>='  DataExpr                  $binary_left  4
  | DataExpr '>'   DataExpr                  $binary_left  4
  | DataExpr 'in'  DataExpr                  $binary_left  4
  | DataExpr '|>'  DataExpr                  $binary_left  5
  | DataExpr '<|'  DataExpr                  $binary_left  6
  | DataExpr '++'  DataExpr                  $binary_left  7
  | DataExpr '+'   DataExpr                  $binary_left  8
  | DataExpr '-'   DataExpr                  $binary_left  8
  | DataExpr '/'   DataExpr                  $binary_left  9
  | DataExpr 'div' DataExpr                  $binary_left  9
  | DataExpr 'mod' DataExpr                  $binary_left  9
  | DataExpr '*'   DataExpr                  $binary_left 10
  | DataExpr '.'   DataExpr                  $binary_left 10
  | DataExpr 'whr' WhrExprList 'end'
  ;

DataExprUnit
  : Id
  | Number
  | 'true'
  | 'false'
  | '(' DataExpr ')'
  | DataExprUnit '(' DataExprList ')'
  | '!' DataExprUnit                         $unary_right 11
  | '-' DataExprUnit                         $unary_right 11
  | '#' DataExprUnit                         $unary_right 11
  ;

DataExprQuantifier
  : 'forall' IdsDeclList '.' $unary_op_right 0
  | 'exists' IdsDeclList '.' $unary_op_right 0
  | 'lambda' IdsDeclList '.' $unary_op_right 0
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
  | ProcExpr '+'   ProcExpr                     $binary_right 1
  | ProcExpr '||'  ProcExpr                     $binary_right 3
  | ProcExpr '||_' ProcExpr                     $binary_right 3
  | ProcExpr '.'   ProcExpr                     $binary_right 6
  | ProcExpr '<<'  ProcExpr                     $binary_left 5
  | ProcExpr '@' DataExprUnit                   $binary_left 7
  | ProcExpr '|'   ProcExpr                     $binary_right 8
  | ProcExprUnaryOperator ProcExpr
  | DataExprUnit ProcExprThenElse               $unary_left 11;
  ;

ProcExprThenElse
  : '->' ProcExpr ('<>' ProcExpr)? $unary_op_left 11;

ProcExprUnaryOperator
  : 'sum' IdsDeclList '.'     $unary_op_right 2
  ;

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

DataSpec: ( SortSpec | ConsSpec | MapSpec | EqnSpec )+ ;

// mCRL2 specification

mCRL2Spec: ( SortSpec | ConsSpec | MapSpec | EqnSpec | GlobVarSpec | ActSpec | ProcSpec | Init )+ ;

// BES

BesSpec: BesEqnSpec BesInit ;

BesEqnSpec: 'bes' BesEqnDecl+ ;

BesEqnDecl: FixedPointOperator BesVar '=' BesExpr ';' ;

BesVar: Id ;

BesExpr
  : 'true'
  | 'false'
  | '!' BesExpr              $unary_left 4
  | BesExpr '=>' BesExpr     $binary_right 2
  | BesExpr '&&' BesExpr     $binary_right 3
  | BesExpr '||' BesExpr     $binary_right 3
  | BesVar
  ;

BesInit: 'init' BesVar ';' ;

// PBES

PbesSpec: DataSpec? GlobVarSpec? PbesEqnSpec PbesInit ;

PbesEqnSpec: 'pbes' PbesEqnDecl+ ;

PbesEqnDecl: FixedPointOperator PropVarDecl '=' PbesExpr ';' ;

FixedPointOperator
  : 'mu'
  | 'nu'
  ;

PropVarDecl: Id ( '(' IdsDeclList ')' )? ;

PropVarInst: Id ( '(' DataExprList ')' )? ;

PbesInit: 'init' PropVarInst ';' ;

DataValExpr: 'val' '(' DataExpr ')' ;

PbesExpr
  : DataValExpr
  | 'true'
  | 'false'
  | 'forall' IdsDeclList PbesExprQuantifierOperator PbesExpr
  | 'exists' IdsDeclList PbesExprQuantifierOperator PbesExpr
  | '!' PbesExpr                                                 $unary_left 4
  | PbesExpr '=>' PbesExpr                                       $binary_right 2
  | PbesExpr '&&' PbesExpr                                       $binary_right 3
  | PbesExpr '||' PbesExpr                                       $binary_right 3
  | PropVarInst
  ;

PbesExprQuantifierOperator: '.' $unary_op_left 1 ;

// Action formulas

ActFrm
  : MultAct
  | DataValExpr
  | 'true'
  | 'false'
  | '!' ActFrm                                                   $unary_left 5
  | ActFrm '=>' ActFrm                                           $binary_right 2
  | ActFrm '&&' ActFrm                                           $binary_right 3
  | ActFrm '||' ActFrm                                           $binary_right 3
  | 'forall' IdsDeclList ActFrmQuantifierOperator ActFrm
  | 'exists' IdsDeclList ActFrmQuantifierOperator ActFrm
  | ActFrm '@' DataExpr                                          $unary_left 4
  | '(' ActFrm ')'
  ;

ActFrmQuantifierOperator: '.' $unary_op_left 1 ;

// Regular formulas

RegFrm
  : ActFrm
  | 'nil'
  | '(' RegFrm ')'
  | '*' RegFrm                                                   $unary_left 3
  | '+' RegFrm                                                   $unary_left 3
  | RegFrm '.' RegFrm                                            $binary_right 1
  | RegFrm '+' RegFrm                                            $binary_right 2
  ;

// State formulas

StateFrm
  : DataValExpr
  | 'true'
  | 'false'
  | '!' StateFrm                                                 $unary_left 5
  | StateFrm '&&' StateFrm                                       $binary_op_right 4
  | StateFrm '||' StateFrm                                       $binary_op_right 4
  | StateFrm '=>' StateFrm                                       $binary_op_right 3
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

StateVarDecl: Id ( '(' IdsDeclList ')' )? ;

StateVarInst: Id ( '(' DataExprList ')' )? ;

StateFrmQuantifierOperator: '.' $unary_op_left 2 ;

StateFrmFixedPointOperator : '.' $unary_op_left 1 ;

// Action Rename Specifications

ActionRenameSpec: (SortSpec | ConsSpec | MapSpec | EqnSpec | ActSpec | ActionRenameRuleSpec)+ ;

ActionRenameRuleSpec: VarSpec? 'rename' ActionRenameRule+ ;

ActionRenameRule: (DataExpr '->')? Action '=>' ActionRenameRuleRHS ';' ;

ActionRenameRuleRHS: Action | 'tau' | 'delta' ;

// Identifiers

IdList: Id ( ',' Id )* ;

//--- start Id definition ---//
// This section contains the definition of an Id. Some C-code (between braces)
// and a semantic action (between brackets) have been added to disallow reserved
// words to be used as an Id.
{
  const char *reserved_words[] = {
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
    "pbes"  ,
    "bes"   ,
    NULL
  };
  static int is_one_of(char *s, const char **list) {
    while (*list) { if (!strcmp(s, *list)) return 1; list++; }
    return 0;
  }
}

Id: "[A-Za-z_][A-Za-z_0-9']*"
[
  char *ts = dup_str($n0.start_loc.s, $n0.end);
  if (is_one_of(ts, reserved_words)) {
    d_free(ts);
    ${reject};
  }
  d_free(ts);
];
//--- end Id definition ---//

Number: "0|(-?[1-9][0-9]*)" ;

// Whitespace

whitespace: "([ \t\n]|(%[^\n]*\n))*" ;

