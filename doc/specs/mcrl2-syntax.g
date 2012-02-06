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

${declare tokenize}
${declare longest_match}

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
  | SortExprList ('->' $binary_op_right 1) SortExpr
  | 'struct' ConstrDeclList                                         // structured sort
  ;

SortExprList: (SortExpr '#')* SortExpr         $unary_right 2 ;

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
  | '[' ']'
  | '{' '}'
  | '[' DataExprList ']'
  | '{' BagEnumEltList '}'
  | '{' VarDecl '|' DataExpr '}'
  | '{' DataExprList '}'
  | '(' DataExpr ')'
  | DataExpr '[' DataExpr '->' DataExpr ']'  $unary_left  13
  | DataExpr '(' DataExprList ')'            $unary_left  13
  | '!' DataExpr                             $unary_right 12
  | '-' DataExpr                             $unary_right 12
  | '#' DataExpr                             $unary_right 12
  | 'forall' VarsDeclList '.' DataExpr       $unary_right  1
  | 'exists' VarsDeclList '.' DataExpr       $unary_right  1
  | 'lambda' VarsDeclList '.' DataExpr       $unary_right  1
  | DataExpr ('=>' $binary_op_right 2) DataExpr
  | DataExpr ('||' $binary_op_right 3) DataExpr
  | DataExpr ('&&' $binary_op_right 4) DataExpr
  | DataExpr ('==' $binary_op_left 5) DataExpr
  | DataExpr ('!=' $binary_op_left 5) DataExpr
  | DataExpr ('<' $binary_op_left 6) DataExpr
  | DataExpr ('<=' $binary_op_left 6) DataExpr
  | DataExpr ('>=' $binary_op_left 6) DataExpr
  | DataExpr ('>' $binary_op_left 6) DataExpr
  | DataExpr ('in' $binary_op_left 6) DataExpr
  | DataExpr ('|>' $binary_op_right 7) DataExpr
  | DataExpr ('<|' $binary_op_left 8) DataExpr
  | DataExpr ('++' $binary_op_left 9) DataExpr
  | DataExpr ('+' $binary_op_left 10) DataExpr
  | DataExpr ('-' $binary_op_left 10) DataExpr
  | DataExpr ('/' $binary_op_left 11) DataExpr
  | DataExpr ('div' $binary_op_left 11) DataExpr
  | DataExpr ('mod' $binary_op_left 11) DataExpr
  | DataExpr ('*' $binary_op_left 12) DataExpr
  | DataExpr ('.' $binary_op_left 12) DataExpr
  | DataExpr 'whr' AssignmentList 'end'      $unary_left 0
  ;

DataExprUnit
  : Id
  | Number
  | 'true'
  | 'false'
  | '(' DataExpr ')'
  | DataExprUnit '(' DataExprList ')'        $unary_left  14
  | '!' DataExprUnit                         $unary_right 13
  | '-' DataExprUnit                         $unary_right 13
  | '#' DataExprUnit                         $unary_right 13
  ;

Assignment: Id '=' DataExpr ;

AssignmentList: Assignment ( ',' Assignment )* ;

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
  | Id '(' AssignmentList? ')'
  | 'delta'
  | 'tau'
  | 'block' '(' ActIdSet ',' ProcExpr ')'
  | 'allow' '(' MultActIdSet ',' ProcExpr ')'
  | 'hide' '(' ActIdSet ',' ProcExpr ')'
  | 'rename' '(' RenExprSet ',' ProcExpr ')'
  | 'comm' '(' CommExprSet ',' ProcExpr ')'
  | '(' ProcExpr ')'
  | ProcExpr ('+' $binary_op_left 1) ProcExpr
  | 'sum' VarsDeclList '.' ProcExpr             $unary_right  2
  | ProcExpr ('||' $binary_op_right 3) ProcExpr
  | ProcExpr ('||_' $binary_op_right 4) ProcExpr
  | DataExprUnit '->' ProcExpr                  $unary_right  5
  | DataExprUnit IfThen ProcExpr                $unary_right  5
  | ProcExpr ('<<' $binary_op_left 6) ProcExpr
  | ProcExpr ('.' $binary_op_right 7) ProcExpr
  | ProcExpr ('@' $binary_op_left 8) DataExprUnit
  | ProcExpr ('|' $binary_op_left 9) ProcExpr
  ;

ProcExprNoIf
  : Action
  | Id '(' AssignmentList? ')'
  | 'delta'
  | 'tau'
  | 'block' '(' ActIdSet ',' ProcExpr ')'
  | 'allow' '(' MultActIdSet ',' ProcExpr ')'
  | 'hide' '(' ActIdSet ',' ProcExpr ')'
  | 'rename' '(' RenExprSet ',' ProcExpr ')'
  | 'comm' '(' CommExprSet ',' ProcExpr ')'
  | '(' ProcExpr ')'
  | ProcExprNoIf ('+' $binary_op_left 1) ProcExprNoIf
  | 'sum' VarsDeclList '.' ProcExprNoIf         $unary_right  2
  | ProcExprNoIf ('||' $binary_op_right 3) ProcExprNoIf
  | ProcExprNoIf ('||_' $binary_op_right 3) ProcExprNoIf
  | DataExprUnit IfThen ProcExprNoIf            $unary_right  4
  | ProcExprNoIf ('<<' $binary_op_left 5) ProcExprNoIf
  | ProcExprNoIf ('.' $binary_op_right 6) ProcExprNoIf
  | ProcExprNoIf ('@' $binary_op_left 7) DataExprUnit
  | ProcExprNoIf ('|' $binary_op_left 8) ProcExprNoIf
  ;

IfThen: '->' ProcExprNoIf '<>' $left 0 ;

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
  | BesExpr ('=>' $binary_op_right 2) BesExpr
  | BesExpr ('||' $binary_op_right 3) BesExpr
  | BesExpr ('&&' $binary_op_right 4) BesExpr
  | '!' BesExpr              $unary_right  5
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
  | 'forall' VarsDeclList '.' PbesExpr                           $unary_right  0
  | 'exists' VarsDeclList '.' PbesExpr                           $unary_right  0
  | PbesExpr ('=>' $binary_op_right 2) PbesExpr
  | PbesExpr ('&&' $binary_op_right 3) PbesExpr
  | PbesExpr ('||' $binary_op_right 4) PbesExpr
  | '!' PbesExpr                                                 $unary_right  5
  | '(' PbesExpr ')'
  | PropVarInst
  ;

//--- Action formulas

ActFrm
  : MultAct
  | DataValExpr
  | 'true'
  | 'false'
  | 'forall' VarsDeclList '.' ActFrm                             $unary_right  0
  | 'exists' VarsDeclList '.' ActFrm                             $unary_right  0
  | ActFrm ('=>' $binary_op_right 2) ActFrm
  | ActFrm ('||' $binary_op_right 3) ActFrm
  | ActFrm ('&&' $binary_op_right 4) ActFrm
  | ActFrm ('@' $binary_op_left 5) DataExpr
  | '!' ActFrm                                                   $unary_right  6
  | '(' ActFrm ')'
  ;

//--- Regular formulas

RegFrm
  : ActFrm
  | 'nil'
  | '(' RegFrm ')'
  | RegFrm ('.' $binary_op_right 1) RegFrm
  | RegFrm ('+' $binary_op_left 2) RegFrm
  | RegFrm '*'                                                   $unary_right  3
  | RegFrm '+'                                                   $unary_right  3
  ;

//--- State formulas

StateFrm
  : DataValExpr
  | 'true'
  | 'false'
  | 'mu' StateVarDecl '.' StateFrm                               $unary_right  1
  | 'nu' StateVarDecl '.' StateFrm                               $unary_right  1
  | 'forall' VarsDeclList '.' StateFrm                           $unary_right  2
  | 'exists' VarsDeclList '.' StateFrm                           $unary_right  2
  | StateFrm ('=>' $binary_op_right 3) StateFrm
  | StateFrm ('||' $binary_op_right 4) StateFrm
  | StateFrm ('&&' $binary_op_right 5) StateFrm
  | '[' RegFrm ']' StateFrm                                      $unary_right  6
  | '<' RegFrm '>' StateFrm                                      $unary_right  6
  | '!' StateFrm                                                 $unary_right  7
  | Id ( '(' DataExprList ')' )?
  | 'delay' ( '@' DataExpr )?
  | 'yaled' ( '@' DataExpr )?
  | '(' StateFrm ')'
  ;

StateVarDecl: Id ( '(' StateVarAssignmentList ')' )? ;

StateVarAssignment: Id ':' SortExpr '=' DataExpr ;

StateVarAssignmentList: StateVarAssignment ( ',' StateVarAssignment )* ;

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

Id: "[A-Za-z_][A-Za-z_0-9']*" $term -1 ;

Number: "0|([1-9][0-9]*)" $term -1 ;

//--- Whitespace

whitespace: "([ \t\n\r]|(%[^\n\r]*))*" ;
