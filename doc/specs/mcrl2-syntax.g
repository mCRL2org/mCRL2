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

//--- Sort expressions and sort declarations

SortExpr
  : 'Bool'                                                          // booleans
  | 'Pos'                                                           // positive numbers
  | 'Nat'                                                           // natural numbers
  | 'Int'                                                           // integers
  | 'Real'                                                          // reals
  | 'List' '(' SortExpr ')'                                         // list sort
  | 'Set' '(' SortExpr ')'                                          // set sort
  | 'Bag' '(' SortExpr ')'                                          // bag sort
  | Id                                                              // sort reference
  | '(' SortExpr ')'                                                // sort expression with parentheses
  | SortExprList '->' SortExpr                 $binary_right 1      // higher-order sort
  | 'struct' ConstrDeclList                                         // structured sort
  ;

SortExprList: SortExpr ( '#' SortExpr )* ;

SortSpec: 'sort' SortDecl+ ;

SortDecl
  : IdList ';'
  | Id '=' SortExpr ';'
  ;

ConstrDecl: Id ( '(' ProjDeclList ')' )? ( '?' Id )? ;

ConstrDeclList: ConstrDecl ( '|' ConstrDecl )* ;

ProjDecl: ( Id ':' )? SortExpr ;

ProjDeclList: ProjDecl ( ',' ProjDecl )* ;

//--- Constructors and mappings

IdsDecl: IdList ':' SortExpr ;

ConsSpec: 'cons' ( IdsDecl ';' )+ ;

MapSpec: 'map' ( IdsDecl ';' )+ ;

//--- Equations

GlobVarSpec: 'glob' ( VarsDeclList ';' )+ ;

VarSpec: 'var' ( VarsDeclList ';' )+ ;

EqnSpec: VarSpec? 'eqn' EqnDecl+ ;

EqnDecl: (DataExpr '->')? DataExpr '=' DataExpr ';' ;

//--- Data expressions

VarDecl: Id ':' SortExpr ;

VarsDecl: IdList ':' SortExpr ;

VarsDeclList: VarsDecl ( ',' VarsDecl )* ;

DataExpr
  : Id
  | Number
  | 'true'
  | 'false'
  | '[]'
  | '{}'
  | '[' DataExprList ']'
  | '{' BagEnumEltList '}'
  | '{' VarDecl '|' DataExpr '}'
  | '{' DataExprList '}'
  | '(' DataExpr ')'
  | DataExpr '[' DataExpr '->' DataExpr ']'
  | DataExpr '(' DataExprList ')'
  | '!' DataExpr                             $unary_right 11
  | '-' DataExpr                             $unary_right 11
  | '#' DataExpr                             $unary_right 11
  | 'forall' VarsDeclList '.' DataExpr       $unary_left 0
  | 'exists' VarsDeclList '.' DataExpr       $unary_left 0
  | 'lambda' VarsDeclList '.' DataExpr       $unary_left 0
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

WhrExpr: DataExpr '=' DataExpr ;

WhrExprList: WhrExpr ( ',' WhrExpr )* ;

DataExprList: DataExpr ( ',' DataExpr )* ;

BagEnumElt: DataExpr ':' DataExpr ;

BagEnumEltList: BagEnumElt ( ',' BagEnumElt )* ;

//--- Communication and renaming sets

ActIdSet: '{' IdList '}' ;

MultActId: Id ( '|' Id )* ;

MultActIdList: MultActId ( ',' MultActId )* ;

MultActIdSet: '{' MultActIdList? '}' ;

CommExprRhs: '->' (Id | 'tau') ;

CommExpr: Id '|' MultActId CommExprRhs? ;

CommExprList: CommExpr ( ',' CommExpr )* ;

CommExprSet: '{' CommExprList? '}' ;

RenExpr: Id '->' Id ;

RenExprList: RenExpr ( ',' RenExpr )* ;

RenExprSet: '{' RenExprList? '}' ;

//--- Process expressions

ProcExpr
  : Action
  | 'delta'
  | 'tau'
  | 'block' '(' ActIdSet ',' ProcExpr ')'
  | 'allow' '(' MultActIdSet ',' ProcExpr ')'
  | 'hide' '(' ActIdSet ',' ProcExpr ')'
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
  | DataExprUnit ProcExprThenElse               $unary_left 11
  | 'sum' VarsDeclList '.' ProcExpr             $unary_left 2
  ;

ProcExprThenElse: '->' ProcExpr ('<>' ProcExpr)? $unary_op_left 11;

//--- Actions

Action: Id ( '(' DataExprList ')' )? ;

ActDecl: IdList ( ':' SortExprList )? ';' ;

ActSpec: 'act' ActDecl+ ;

MultAct
  : 'tau'
  | ActionList
  ;

ActionList: Action ( '|' Action )* ;

//--- Process and initial state declaration

ProcDecl: Id ( '(' VarsDeclList ')' )? '=' ProcExpr ';' ;

ProcSpec: 'proc' ProcDecl+ ;

Init: 'init' ProcExpr ';' ;

//--- Data specification

DataSpec: ( SortSpec | ConsSpec | MapSpec | EqnSpec )+ ;

//--- mCRL2 specification

mCRL2Spec: mCRL2SpecElt* Init mCRL2SpecElt* ;

mCRL2SpecElt
  : SortSpec
  | ConsSpec
  | MapSpec
  | EqnSpec
  | GlobVarSpec
  | ActSpec
  | ProcSpec
  ;

//--- BES

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
  | '(' BesExpr ')'
  | BesVar
  ;

BesInit: 'init' BesVar ';' ;

//--- PBES

PbesSpec: DataSpec? GlobVarSpec? PbesEqnSpec PbesInit ;

PbesEqnSpec: 'pbes' PbesEqnDecl+ ;

PbesEqnDecl: FixedPointOperator PropVarDecl '=' PbesExpr ';' ;

FixedPointOperator
  : 'mu'
  | 'nu'
  ;

PropVarDecl: Id ( '(' VarsDeclList ')' )? ;

PropVarInst: Id ( '(' DataExprList ')' )? ;

PbesInit: 'init' PropVarInst ';' ;

DataValExpr: 'val' '(' DataExpr ')' ;

PbesExpr
  : DataValExpr
  | 'true'
  | 'false'
  | 'forall' VarsDeclList '.' PbesExpr                           $unary_left 0
  | 'exists' VarsDeclList '.' PbesExpr                           $unary_left 0
  | '!' PbesExpr                                                 $unary_left 4
  | PbesExpr '=>' PbesExpr                                       $binary_right 2
  | PbesExpr '&&' PbesExpr                                       $binary_right 3
  | PbesExpr '||' PbesExpr                                       $binary_right 3
  | '(' PbesExpr ')'
  | PropVarInst
  ;

//--- Action formulas

ActFrm
  : MultAct
  | DataValExpr
  | 'true'
  | 'false'
  | '!' ActFrm                                                   $unary_left 5
  | ActFrm '=>' ActFrm                                           $binary_right 2
  | ActFrm '&&' ActFrm                                           $binary_right 3
  | ActFrm '||' ActFrm                                           $binary_right 3
  | 'forall' VarsDeclList '.' ActFrm                             $unary_left 0
  | 'exists' VarsDeclList '.' ActFrm                             $unary_left 0
  | ActFrm '@' DataExpr                                          $unary_left 4
  | '(' ActFrm ')'
  ;

//--- Regular formulas

RegFrm
  : ActFrm
  | 'nil'
  | '(' RegFrm ')'
  | '*' RegFrm                                                   $unary_left 3
  | '+' RegFrm                                                   $unary_left 3
  | RegFrm '.' RegFrm                                            $binary_right 1
  | RegFrm '+' RegFrm                                            $binary_right 2
  ;

//--- State formulas

StateFrm
  : DataValExpr
  | 'true'
  | 'false'
  | '!' StateFrm                                                 $unary_left 5
  | StateFrm '=>' StateFrm                                       $binary_op_right 3
  | StateFrm '&&' StateFrm                                       $binary_op_right 4
  | StateFrm '||' StateFrm                                       $binary_op_right 4
  | 'forall' VarsDeclList '.' StateFrm                           $unary_left 2
  | 'exists' VarsDeclList '.' StateFrm                           $unary_left 2
  | '[' RegFrm ']'
  | '<' RegFrm '>'
  | 'mu' StateVarDecl '.' StateFrm                               $unary_left 1
  | 'nu' StateVarDecl '.' StateFrm                               $unary_left 1
  | Id ( '(' DataExprList ')' )?
  | 'delay' ( '@' DataExpr )?
  | 'yaled' ( '@' DataExpr )?
  | '(' StateFrm ')'
  ;

StateVarDecl: Id ( '(' VarsDeclList ')' )? ;

//--- Action Rename Specifications

ActionRenameSpec: (SortSpec | ConsSpec | MapSpec | EqnSpec | ActSpec | ActionRenameRuleSpec)+ ;

ActionRenameRuleSpec: VarSpec? 'rename' ActionRenameRule+ ;

ActionRenameRule: (DataExpr '->')? Action '=>' ActionRenameRuleRHS ';' ;

ActionRenameRuleRHS
  : Action
  | 'tau'
  | 'delta'
  ;

//--- Identifiers

IdList: Id ( ',' Id )* ;

//-- start Id definition --//
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
    "nil"   ,
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
//-- end Id definition --//

Number: "0|(-?[1-9][0-9]*)" ;

//--- Whitespace

whitespace: "([ \t\n]|(%[^\n]*\n))*" ;
