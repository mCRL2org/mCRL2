// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/detail/default_values.h
/// \brief add your file description here.

#ifndef MCRL2_CORE_DETAIL_DEFAULT_VALUES_H
#define MCRL2_CORE_DETAIL_DEFAULT_VALUES_H

#include "mcrl2/atermpp/aterm_int.h"
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/core/detail/function_symbols.h"

namespace mcrl2 {

namespace core {

namespace detail {

//----------------------------------------------------------------------------------------------//
// Part 1: functions for creating default values.
//----------------------------------------------------------------------------------------------//

inline
const atermpp::aterm_int& default_value_Number()
{
  static atermpp::aterm_int t = atermpp::aterm_int(std::size_t(0));
  return t;
}

inline
const atermpp::aterm_appl& default_value_String()
{
  static atermpp::aterm_appl t = core::identifier_string("@NoValue");
  return t;
}

inline
const atermpp::aterm_appl& default_value_StringOrEmpty()
{
  static atermpp::aterm_appl t = core::identifier_string("@NoValue");
  return t;
}

inline
const atermpp::aterm_list& default_value_List()
{
  static atermpp::aterm_list t = atermpp::aterm_list();
  return t;
}

// List
inline
atermpp::aterm_list default_value_List(const atermpp::aterm& t)
{
  atermpp::aterm_list result;
  result.push_front(t);
  return result;
}

//--- start generated constructors ---//
const atermpp::aterm_appl& default_value_BooleanOr();
const atermpp::aterm_appl& default_value_StateOr();
const atermpp::aterm_appl& default_value_Hide();
const atermpp::aterm_appl& default_value_SortArrow();
const atermpp::aterm_appl& default_value_ProcessAssignment();
const atermpp::aterm_appl& default_value_Forall();
const atermpp::aterm_appl& default_value_CommExpr();
const atermpp::aterm_appl& default_value_StateNot();
const atermpp::aterm_appl& default_value_UntypedSetBagComp();
const atermpp::aterm_appl& default_value_SortFSet();
const atermpp::aterm_appl& default_value_StateImp();
const atermpp::aterm_appl& default_value_PBESExists();
const atermpp::aterm_appl& default_value_PBESImp();
const atermpp::aterm_appl& default_value_Binder();
const atermpp::aterm_appl& default_value_SortRef();
const atermpp::aterm_appl& default_value_ProcEqnSpec();
const atermpp::aterm_appl& default_value_StateForall();
const atermpp::aterm_appl& default_value_BooleanImp();
const atermpp::aterm_appl& default_value_SortId();
const atermpp::aterm_appl& default_value_UntypedAction();
const atermpp::aterm_appl& default_value_StateNu();
const atermpp::aterm_appl& default_value_RegNil();
const atermpp::aterm_appl& default_value_DataSpec();
const atermpp::aterm_appl& default_value_UntypedActMultAct();
const atermpp::aterm_appl& default_value_Tau();
const atermpp::aterm_appl& default_value_StateYaledTimed();
const atermpp::aterm_appl& default_value_SortCons();
const atermpp::aterm_appl& default_value_DataEqnSpec();
const atermpp::aterm_appl& default_value_LinearProcessSummand();
const atermpp::aterm_appl& default_value_SortSpec();
const atermpp::aterm_appl& default_value_ActionRenameRules();
const atermpp::aterm_appl& default_value_UntypedParamId();
const atermpp::aterm_appl& default_value_BooleanEquation();
const atermpp::aterm_appl& default_value_ConsSpec();
const atermpp::aterm_appl& default_value_SortList();
const atermpp::aterm_appl& default_value_Sum();
const atermpp::aterm_appl& default_value_DataVarId();
const atermpp::aterm_appl& default_value_ProcVarId();
const atermpp::aterm_appl& default_value_ProcessInit();
const atermpp::aterm_appl& default_value_UntypedIdentifier();
const atermpp::aterm_appl& default_value_BooleanFalse();
const atermpp::aterm_appl& default_value_BES();
const atermpp::aterm_appl& default_value_MapSpec();
const atermpp::aterm_appl& default_value_IfThen();
const atermpp::aterm_appl& default_value_BooleanAnd();
const atermpp::aterm_appl& default_value_LinProcSpec();
const atermpp::aterm_appl& default_value_Choice();
const atermpp::aterm_appl& default_value_LinearProcessInit();
const atermpp::aterm_appl& default_value_MultAct();
const atermpp::aterm_appl& default_value_PropVarInst();
const atermpp::aterm_appl& default_value_BagComp();
const atermpp::aterm_appl& default_value_StateDelay();
const atermpp::aterm_appl& default_value_RegAlt();
const atermpp::aterm_appl& default_value_UntypedMultAct();
const atermpp::aterm_appl& default_value_StructCons();
const atermpp::aterm_appl& default_value_Mu();
const atermpp::aterm_appl& default_value_PBEqnSpec();
const atermpp::aterm_appl& default_value_ActNot();
const atermpp::aterm_appl& default_value_BooleanTrue();
const atermpp::aterm_appl& default_value_Block();
const atermpp::aterm_appl& default_value_Rename();
const atermpp::aterm_appl& default_value_Exists();
const atermpp::aterm_appl& default_value_Sync();
const atermpp::aterm_appl& default_value_ActExists();
const atermpp::aterm_appl& default_value_ProcSpec();
const atermpp::aterm_appl& default_value_UntypedSortsPossible();
const atermpp::aterm_appl& default_value_StateMu();
const atermpp::aterm_appl& default_value_StateFalse();
const atermpp::aterm_appl& default_value_PBESFalse();
const atermpp::aterm_appl& default_value_PBESForall();
const atermpp::aterm_appl& default_value_StateTrue();
const atermpp::aterm_appl& default_value_BInit();
const atermpp::aterm_appl& default_value_UntypedSortUnknown();
const atermpp::aterm_appl& default_value_RegTrans();
const atermpp::aterm_appl& default_value_StateDelayTimed();
const atermpp::aterm_appl& default_value_Nu();
const atermpp::aterm_appl& default_value_SortStruct();
const atermpp::aterm_appl& default_value_AtTime();
const atermpp::aterm_appl& default_value_ActOr();
const atermpp::aterm_appl& default_value_Comm();
const atermpp::aterm_appl& default_value_BooleanNot();
const atermpp::aterm_appl& default_value_Delta();
const atermpp::aterm_appl& default_value_ActMultAct();
const atermpp::aterm_appl& default_value_StateAnd();
const atermpp::aterm_appl& default_value_LMerge();
const atermpp::aterm_appl& default_value_SetComp();
const atermpp::aterm_appl& default_value_ActForall();
const atermpp::aterm_appl& default_value_RenameExpr();
const atermpp::aterm_appl& default_value_Merge();
const atermpp::aterm_appl& default_value_ActSpec();
const atermpp::aterm_appl& default_value_BooleanVariable();
const atermpp::aterm_appl& default_value_Action();
const atermpp::aterm_appl& default_value_PBESAnd();
const atermpp::aterm_appl& default_value_Lambda();
const atermpp::aterm_appl& default_value_StateMust();
const atermpp::aterm_appl& default_value_Seq();
const atermpp::aterm_appl& default_value_DataVarIdInit();
const atermpp::aterm_appl& default_value_Process();
const atermpp::aterm_appl& default_value_ActAnd();
const atermpp::aterm_appl& default_value_ActionRenameSpec();
const atermpp::aterm_appl& default_value_PBES();
const atermpp::aterm_appl& default_value_StateVar();
const atermpp::aterm_appl& default_value_ActionRenameRule();
const atermpp::aterm_appl& default_value_RegSeq();
const atermpp::aterm_appl& default_value_LinearProcess();
const atermpp::aterm_appl& default_value_ActAt();
const atermpp::aterm_appl& default_value_DataEqn();
const atermpp::aterm_appl& default_value_PBESNot();
const atermpp::aterm_appl& default_value_StateExists();
const atermpp::aterm_appl& default_value_StateMay();
const atermpp::aterm_appl& default_value_PBESTrue();
const atermpp::aterm_appl& default_value_MultActName();
const atermpp::aterm_appl& default_value_IfThenElse();
const atermpp::aterm_appl& default_value_Nil();
const atermpp::aterm_appl& default_value_ProcEqn();
const atermpp::aterm_appl& default_value_StructProj();
const atermpp::aterm_appl& default_value_PBEqn();
const atermpp::aterm_appl& default_value_Whr();
const atermpp::aterm_appl& default_value_OpId();
const atermpp::aterm_appl& default_value_SortSet();
const atermpp::aterm_appl& default_value_ActFalse();
const atermpp::aterm_appl& default_value_ActId();
const atermpp::aterm_appl& default_value_StateYaled();
const atermpp::aterm_appl& default_value_PBESOr();
const atermpp::aterm_appl& default_value_UntypedProcessAssignment();
const atermpp::aterm_appl& default_value_SortFBag();
const atermpp::aterm_appl& default_value_Allow();
const atermpp::aterm_appl& default_value_PropVarDecl();
const atermpp::aterm_appl& default_value_ActImp();
const atermpp::aterm_appl& default_value_SortBag();
const atermpp::aterm_appl& default_value_PBInit();
const atermpp::aterm_appl& default_value_ActTrue();
const atermpp::aterm_appl& default_value_RegTransOrNil();
const atermpp::aterm_appl& default_value_GlobVarSpec();
const atermpp::aterm_appl& default_value_UntypedIdentifierAssignment();
const atermpp::aterm_appl& default_value_SortExpr();
const atermpp::aterm_appl& default_value_SortConsType();
const atermpp::aterm_appl& default_value_DataExpr();
const atermpp::aterm_appl& default_value_BindingOperator();
const atermpp::aterm_appl& default_value_WhrDecl();
const atermpp::aterm_appl& default_value_SortDecl();
const atermpp::aterm_appl& default_value_DataExprOrNil();
const atermpp::aterm_appl& default_value_ProcExpr();
const atermpp::aterm_appl& default_value_MultActOrDelta();
const atermpp::aterm_appl& default_value_ProcInit();
const atermpp::aterm_appl& default_value_StateFrm();
const atermpp::aterm_appl& default_value_RegFrm();
const atermpp::aterm_appl& default_value_ActFrm();
const atermpp::aterm_appl& default_value_ParamIdOrAction();
const atermpp::aterm_appl& default_value_ActionRenameRuleRHS();
const atermpp::aterm_appl& default_value_FixPoint();
const atermpp::aterm_appl& default_value_PBExpr();
const atermpp::aterm_appl& default_value_BooleanExpression();

// BooleanOr
inline
const atermpp::aterm_appl& default_value_BooleanOr()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_BooleanOr(), default_value_BooleanExpression(), default_value_BooleanExpression());
  return t;
}

// StateOr
inline
const atermpp::aterm_appl& default_value_StateOr()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_StateOr(), default_value_StateFrm(), default_value_StateFrm());
  return t;
}

// Hide
inline
const atermpp::aterm_appl& default_value_Hide()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_Hide(), default_value_List(), default_value_ProcExpr());
  return t;
}

// SortArrow
inline
const atermpp::aterm_appl& default_value_SortArrow()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_SortArrow(), default_value_List(default_value_SortExpr()), default_value_SortExpr());
  return t;
}

// ProcessAssignment
inline
const atermpp::aterm_appl& default_value_ProcessAssignment()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_ProcessAssignment(), default_value_ProcVarId(), default_value_List());
  return t;
}

// Forall
inline
const atermpp::aterm_appl& default_value_Forall()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_Forall());
  return t;
}

// CommExpr
inline
const atermpp::aterm_appl& default_value_CommExpr()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_CommExpr(), default_value_MultActName(), default_value_String());
  return t;
}

// StateNot
inline
const atermpp::aterm_appl& default_value_StateNot()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_StateNot(), default_value_StateFrm());
  return t;
}

// UntypedSetBagComp
inline
const atermpp::aterm_appl& default_value_UntypedSetBagComp()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_UntypedSetBagComp());
  return t;
}

// SortFSet
inline
const atermpp::aterm_appl& default_value_SortFSet()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_SortFSet());
  return t;
}

// StateImp
inline
const atermpp::aterm_appl& default_value_StateImp()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_StateImp(), default_value_StateFrm(), default_value_StateFrm());
  return t;
}

// PBESExists
inline
const atermpp::aterm_appl& default_value_PBESExists()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_PBESExists(), default_value_List(default_value_DataVarId()), default_value_PBExpr());
  return t;
}

// PBESImp
inline
const atermpp::aterm_appl& default_value_PBESImp()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_PBESImp(), default_value_PBExpr(), default_value_PBExpr());
  return t;
}

// Binder
inline
const atermpp::aterm_appl& default_value_Binder()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_Binder(), default_value_BindingOperator(), default_value_List(default_value_DataVarId()), default_value_DataExpr());
  return t;
}

// SortRef
inline
const atermpp::aterm_appl& default_value_SortRef()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_SortRef(), default_value_SortId(), default_value_SortExpr());
  return t;
}

// ProcEqnSpec
inline
const atermpp::aterm_appl& default_value_ProcEqnSpec()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_ProcEqnSpec(), default_value_List());
  return t;
}

// StateForall
inline
const atermpp::aterm_appl& default_value_StateForall()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_StateForall(), default_value_List(default_value_DataVarId()), default_value_StateFrm());
  return t;
}

// BooleanImp
inline
const atermpp::aterm_appl& default_value_BooleanImp()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_BooleanImp(), default_value_BooleanExpression(), default_value_BooleanExpression());
  return t;
}

// SortId
inline
const atermpp::aterm_appl& default_value_SortId()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_SortId(), default_value_String());
  return t;
}

// UntypedAction
inline
const atermpp::aterm_appl& default_value_UntypedAction()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_UntypedAction(), default_value_String(), default_value_List());
  return t;
}

// StateNu
inline
const atermpp::aterm_appl& default_value_StateNu()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_StateNu(), default_value_String(), default_value_List(), default_value_StateFrm());
  return t;
}

// RegNil
inline
const atermpp::aterm_appl& default_value_RegNil()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_RegNil());
  return t;
}

// DataSpec
inline
const atermpp::aterm_appl& default_value_DataSpec()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_DataSpec(), default_value_SortSpec(), default_value_ConsSpec(), default_value_MapSpec(), default_value_DataEqnSpec());
  return t;
}

// UntypedActMultAct
inline
const atermpp::aterm_appl& default_value_UntypedActMultAct()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_UntypedActMultAct(), default_value_List());
  return t;
}

// Tau
inline
const atermpp::aterm_appl& default_value_Tau()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_Tau());
  return t;
}

// StateYaledTimed
inline
const atermpp::aterm_appl& default_value_StateYaledTimed()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_StateYaledTimed(), default_value_DataExpr());
  return t;
}

// SortCons
inline
const atermpp::aterm_appl& default_value_SortCons()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_SortCons(), default_value_SortConsType(), default_value_SortExpr());
  return t;
}

// DataEqnSpec
inline
const atermpp::aterm_appl& default_value_DataEqnSpec()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_DataEqnSpec(), default_value_List());
  return t;
}

// LinearProcessSummand
inline
const atermpp::aterm_appl& default_value_LinearProcessSummand()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_LinearProcessSummand(), default_value_List(), default_value_DataExpr(), default_value_MultActOrDelta(), default_value_DataExprOrNil(), default_value_List());
  return t;
}

// SortSpec
inline
const atermpp::aterm_appl& default_value_SortSpec()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_SortSpec(), default_value_List());
  return t;
}

// ActionRenameRules
inline
const atermpp::aterm_appl& default_value_ActionRenameRules()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_ActionRenameRules(), default_value_List());
  return t;
}

// UntypedParamId
inline
const atermpp::aterm_appl& default_value_UntypedParamId()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_UntypedParamId(), default_value_String(), default_value_List());
  return t;
}

// BooleanEquation
inline
const atermpp::aterm_appl& default_value_BooleanEquation()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_BooleanEquation(), default_value_FixPoint(), default_value_BooleanVariable(), default_value_BooleanExpression());
  return t;
}

// ConsSpec
inline
const atermpp::aterm_appl& default_value_ConsSpec()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_ConsSpec(), default_value_List());
  return t;
}

// SortList
inline
const atermpp::aterm_appl& default_value_SortList()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_SortList());
  return t;
}

// Sum
inline
const atermpp::aterm_appl& default_value_Sum()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_Sum(), default_value_List(default_value_DataVarId()), default_value_ProcExpr());
  return t;
}

// DataVarId
inline
const atermpp::aterm_appl& default_value_DataVarId()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_DataVarId(), default_value_String(), default_value_SortExpr(), default_value_Number());
  return t;
}

// ProcVarId
inline
const atermpp::aterm_appl& default_value_ProcVarId()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_ProcVarId(), default_value_String(), default_value_List(), default_value_Number());
  return t;
}

// ProcessInit
inline
const atermpp::aterm_appl& default_value_ProcessInit()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_ProcessInit(), default_value_ProcExpr());
  return t;
}

// UntypedIdentifier
inline
const atermpp::aterm_appl& default_value_UntypedIdentifier()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_UntypedIdentifier(), default_value_String());
  return t;
}

// BooleanFalse
inline
const atermpp::aterm_appl& default_value_BooleanFalse()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_BooleanFalse());
  return t;
}

// BES
inline
const atermpp::aterm_appl& default_value_BES()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_BES(), default_value_List(), default_value_BooleanExpression());
  return t;
}

// MapSpec
inline
const atermpp::aterm_appl& default_value_MapSpec()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_MapSpec(), default_value_List());
  return t;
}

// IfThen
inline
const atermpp::aterm_appl& default_value_IfThen()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_IfThen(), default_value_DataExpr(), default_value_ProcExpr());
  return t;
}

// BooleanAnd
inline
const atermpp::aterm_appl& default_value_BooleanAnd()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_BooleanAnd(), default_value_BooleanExpression(), default_value_BooleanExpression());
  return t;
}

// LinProcSpec
inline
const atermpp::aterm_appl& default_value_LinProcSpec()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_LinProcSpec(), default_value_DataSpec(), default_value_ActSpec(), default_value_GlobVarSpec(), default_value_LinearProcess(), default_value_LinearProcessInit());
  return t;
}

// Choice
inline
const atermpp::aterm_appl& default_value_Choice()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_Choice(), default_value_ProcExpr(), default_value_ProcExpr());
  return t;
}

// LinearProcessInit
inline
const atermpp::aterm_appl& default_value_LinearProcessInit()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_LinearProcessInit(), default_value_List());
  return t;
}

// MultAct
inline
const atermpp::aterm_appl& default_value_MultAct()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_MultAct(), default_value_List());
  return t;
}

// PropVarInst
inline
const atermpp::aterm_appl& default_value_PropVarInst()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_PropVarInst(), default_value_String(), default_value_List(), default_value_Number());
  return t;
}

// BagComp
inline
const atermpp::aterm_appl& default_value_BagComp()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_BagComp());
  return t;
}

// StateDelay
inline
const atermpp::aterm_appl& default_value_StateDelay()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_StateDelay());
  return t;
}

// RegAlt
inline
const atermpp::aterm_appl& default_value_RegAlt()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_RegAlt(), default_value_RegFrm(), default_value_RegFrm());
  return t;
}

// UntypedMultAct
inline
const atermpp::aterm_appl& default_value_UntypedMultAct()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_UntypedMultAct(), default_value_List());
  return t;
}

// StructCons
inline
const atermpp::aterm_appl& default_value_StructCons()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_StructCons(), default_value_String(), default_value_List(), default_value_StringOrEmpty());
  return t;
}

// Mu
inline
const atermpp::aterm_appl& default_value_Mu()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_Mu());
  return t;
}

// PBEqnSpec
inline
const atermpp::aterm_appl& default_value_PBEqnSpec()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_PBEqnSpec(), default_value_List());
  return t;
}

// ActNot
inline
const atermpp::aterm_appl& default_value_ActNot()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_ActNot(), default_value_ActFrm());
  return t;
}

// BooleanTrue
inline
const atermpp::aterm_appl& default_value_BooleanTrue()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_BooleanTrue());
  return t;
}

// Block
inline
const atermpp::aterm_appl& default_value_Block()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_Block(), default_value_List(), default_value_ProcExpr());
  return t;
}

// Rename
inline
const atermpp::aterm_appl& default_value_Rename()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_Rename(), default_value_List(), default_value_ProcExpr());
  return t;
}

// Exists
inline
const atermpp::aterm_appl& default_value_Exists()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_Exists());
  return t;
}

// Sync
inline
const atermpp::aterm_appl& default_value_Sync()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_Sync(), default_value_ProcExpr(), default_value_ProcExpr());
  return t;
}

// ActExists
inline
const atermpp::aterm_appl& default_value_ActExists()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_ActExists(), default_value_List(default_value_DataVarId()), default_value_ActFrm());
  return t;
}

// ProcSpec
inline
const atermpp::aterm_appl& default_value_ProcSpec()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_ProcSpec(), default_value_DataSpec(), default_value_ActSpec(), default_value_GlobVarSpec(), default_value_ProcEqnSpec(), default_value_ProcInit());
  return t;
}

// UntypedSortsPossible
inline
const atermpp::aterm_appl& default_value_UntypedSortsPossible()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_UntypedSortsPossible(), default_value_List(default_value_SortExpr()));
  return t;
}

// StateMu
inline
const atermpp::aterm_appl& default_value_StateMu()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_StateMu(), default_value_String(), default_value_List(), default_value_StateFrm());
  return t;
}

// StateFalse
inline
const atermpp::aterm_appl& default_value_StateFalse()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_StateFalse());
  return t;
}

// PBESFalse
inline
const atermpp::aterm_appl& default_value_PBESFalse()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_PBESFalse());
  return t;
}

// PBESForall
inline
const atermpp::aterm_appl& default_value_PBESForall()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_PBESForall(), default_value_List(default_value_DataVarId()), default_value_PBExpr());
  return t;
}

// StateTrue
inline
const atermpp::aterm_appl& default_value_StateTrue()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_StateTrue());
  return t;
}

// BInit
inline
const atermpp::aterm_appl& default_value_BInit()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_BInit(), default_value_ProcExpr(), default_value_ProcExpr());
  return t;
}

// UntypedSortUnknown
inline
const atermpp::aterm_appl& default_value_UntypedSortUnknown()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_UntypedSortUnknown());
  return t;
}

// RegTrans
inline
const atermpp::aterm_appl& default_value_RegTrans()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_RegTrans(), default_value_RegFrm());
  return t;
}

// StateDelayTimed
inline
const atermpp::aterm_appl& default_value_StateDelayTimed()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_StateDelayTimed(), default_value_DataExpr());
  return t;
}

// Nu
inline
const atermpp::aterm_appl& default_value_Nu()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_Nu());
  return t;
}

// SortStruct
inline
const atermpp::aterm_appl& default_value_SortStruct()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_SortStruct(), default_value_List(default_value_StructCons()));
  return t;
}

// AtTime
inline
const atermpp::aterm_appl& default_value_AtTime()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_AtTime(), default_value_ProcExpr(), default_value_DataExpr());
  return t;
}

// ActOr
inline
const atermpp::aterm_appl& default_value_ActOr()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_ActOr(), default_value_ActFrm(), default_value_ActFrm());
  return t;
}

// Comm
inline
const atermpp::aterm_appl& default_value_Comm()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_Comm(), default_value_List(), default_value_ProcExpr());
  return t;
}

// BooleanNot
inline
const atermpp::aterm_appl& default_value_BooleanNot()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_BooleanNot(), default_value_BooleanExpression());
  return t;
}

// Delta
inline
const atermpp::aterm_appl& default_value_Delta()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_Delta());
  return t;
}

// ActMultAct
inline
const atermpp::aterm_appl& default_value_ActMultAct()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_ActMultAct(), default_value_List());
  return t;
}

// StateAnd
inline
const atermpp::aterm_appl& default_value_StateAnd()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_StateAnd(), default_value_StateFrm(), default_value_StateFrm());
  return t;
}

// LMerge
inline
const atermpp::aterm_appl& default_value_LMerge()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_LMerge(), default_value_ProcExpr(), default_value_ProcExpr());
  return t;
}

// SetComp
inline
const atermpp::aterm_appl& default_value_SetComp()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_SetComp());
  return t;
}

// ActForall
inline
const atermpp::aterm_appl& default_value_ActForall()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_ActForall(), default_value_List(default_value_DataVarId()), default_value_ActFrm());
  return t;
}

// RenameExpr
inline
const atermpp::aterm_appl& default_value_RenameExpr()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_RenameExpr(), default_value_String(), default_value_String());
  return t;
}

// Merge
inline
const atermpp::aterm_appl& default_value_Merge()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_Merge(), default_value_ProcExpr(), default_value_ProcExpr());
  return t;
}

// ActSpec
inline
const atermpp::aterm_appl& default_value_ActSpec()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_ActSpec(), default_value_List());
  return t;
}

// BooleanVariable
inline
const atermpp::aterm_appl& default_value_BooleanVariable()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_BooleanVariable(), default_value_String(), default_value_Number());
  return t;
}

// Action
inline
const atermpp::aterm_appl& default_value_Action()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_Action(), default_value_ActId(), default_value_List());
  return t;
}

// PBESAnd
inline
const atermpp::aterm_appl& default_value_PBESAnd()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_PBESAnd(), default_value_PBExpr(), default_value_PBExpr());
  return t;
}

// Lambda
inline
const atermpp::aterm_appl& default_value_Lambda()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_Lambda());
  return t;
}

// StateMust
inline
const atermpp::aterm_appl& default_value_StateMust()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_StateMust(), default_value_RegFrm(), default_value_StateFrm());
  return t;
}

// Seq
inline
const atermpp::aterm_appl& default_value_Seq()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_Seq(), default_value_ProcExpr(), default_value_ProcExpr());
  return t;
}

// DataVarIdInit
inline
const atermpp::aterm_appl& default_value_DataVarIdInit()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_DataVarIdInit(), default_value_DataVarId(), default_value_DataExpr());
  return t;
}

// Process
inline
const atermpp::aterm_appl& default_value_Process()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_Process(), default_value_ProcVarId(), default_value_List());
  return t;
}

// ActAnd
inline
const atermpp::aterm_appl& default_value_ActAnd()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_ActAnd(), default_value_ActFrm(), default_value_ActFrm());
  return t;
}

// ActionRenameSpec
inline
const atermpp::aterm_appl& default_value_ActionRenameSpec()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_ActionRenameSpec(), default_value_DataSpec(), default_value_ActSpec(), default_value_ActionRenameRules());
  return t;
}

// PBES
inline
const atermpp::aterm_appl& default_value_PBES()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_PBES(), default_value_DataSpec(), default_value_GlobVarSpec(), default_value_PBEqnSpec(), default_value_PBInit());
  return t;
}

// StateVar
inline
const atermpp::aterm_appl& default_value_StateVar()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_StateVar(), default_value_String(), default_value_List());
  return t;
}

// ActionRenameRule
inline
const atermpp::aterm_appl& default_value_ActionRenameRule()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_ActionRenameRule(), default_value_List(), default_value_DataExpr(), default_value_ParamIdOrAction(), default_value_ActionRenameRuleRHS());
  return t;
}

// RegSeq
inline
const atermpp::aterm_appl& default_value_RegSeq()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_RegSeq(), default_value_RegFrm(), default_value_RegFrm());
  return t;
}

// LinearProcess
inline
const atermpp::aterm_appl& default_value_LinearProcess()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_LinearProcess(), default_value_List(), default_value_List());
  return t;
}

// ActAt
inline
const atermpp::aterm_appl& default_value_ActAt()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_ActAt(), default_value_ActFrm(), default_value_DataExpr());
  return t;
}

// DataEqn
inline
const atermpp::aterm_appl& default_value_DataEqn()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_DataEqn(), default_value_List(), default_value_DataExpr(), default_value_DataExpr(), default_value_DataExpr());
  return t;
}

// PBESNot
inline
const atermpp::aterm_appl& default_value_PBESNot()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_PBESNot(), default_value_PBExpr());
  return t;
}

// StateExists
inline
const atermpp::aterm_appl& default_value_StateExists()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_StateExists(), default_value_List(default_value_DataVarId()), default_value_StateFrm());
  return t;
}

// StateMay
inline
const atermpp::aterm_appl& default_value_StateMay()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_StateMay(), default_value_RegFrm(), default_value_StateFrm());
  return t;
}

// PBESTrue
inline
const atermpp::aterm_appl& default_value_PBESTrue()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_PBESTrue());
  return t;
}

// MultActName
inline
const atermpp::aterm_appl& default_value_MultActName()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_MultActName(), default_value_List(default_value_String()));
  return t;
}

// IfThenElse
inline
const atermpp::aterm_appl& default_value_IfThenElse()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_IfThenElse(), default_value_DataExpr(), default_value_ProcExpr(), default_value_ProcExpr());
  return t;
}

// Nil
inline
const atermpp::aterm_appl& default_value_Nil()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_Nil());
  return t;
}

// ProcEqn
inline
const atermpp::aterm_appl& default_value_ProcEqn()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_ProcEqn(), default_value_ProcVarId(), default_value_List(), default_value_ProcExpr());
  return t;
}

// StructProj
inline
const atermpp::aterm_appl& default_value_StructProj()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_StructProj(), default_value_StringOrEmpty(), default_value_SortExpr());
  return t;
}

// PBEqn
inline
const atermpp::aterm_appl& default_value_PBEqn()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_PBEqn(), default_value_FixPoint(), default_value_PropVarDecl(), default_value_PBExpr());
  return t;
}

// Whr
inline
const atermpp::aterm_appl& default_value_Whr()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_Whr(), default_value_DataExpr(), default_value_List(default_value_WhrDecl()));
  return t;
}

// OpId
inline
const atermpp::aterm_appl& default_value_OpId()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_OpId(), default_value_String(), default_value_SortExpr(), default_value_Number());
  return t;
}

// SortSet
inline
const atermpp::aterm_appl& default_value_SortSet()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_SortSet());
  return t;
}

// ActFalse
inline
const atermpp::aterm_appl& default_value_ActFalse()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_ActFalse());
  return t;
}

// ActId
inline
const atermpp::aterm_appl& default_value_ActId()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_ActId(), default_value_String(), default_value_List());
  return t;
}

// StateYaled
inline
const atermpp::aterm_appl& default_value_StateYaled()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_StateYaled());
  return t;
}

// PBESOr
inline
const atermpp::aterm_appl& default_value_PBESOr()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_PBESOr(), default_value_PBExpr(), default_value_PBExpr());
  return t;
}

// UntypedProcessAssignment
inline
const atermpp::aterm_appl& default_value_UntypedProcessAssignment()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_UntypedProcessAssignment(), default_value_String(), default_value_List());
  return t;
}

// SortFBag
inline
const atermpp::aterm_appl& default_value_SortFBag()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_SortFBag());
  return t;
}

// Allow
inline
const atermpp::aterm_appl& default_value_Allow()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_Allow(), default_value_List(), default_value_ProcExpr());
  return t;
}

// PropVarDecl
inline
const atermpp::aterm_appl& default_value_PropVarDecl()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_PropVarDecl(), default_value_String(), default_value_List());
  return t;
}

// ActImp
inline
const atermpp::aterm_appl& default_value_ActImp()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_ActImp(), default_value_ActFrm(), default_value_ActFrm());
  return t;
}

// SortBag
inline
const atermpp::aterm_appl& default_value_SortBag()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_SortBag());
  return t;
}

// PBInit
inline
const atermpp::aterm_appl& default_value_PBInit()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_PBInit(), default_value_PropVarInst());
  return t;
}

// ActTrue
inline
const atermpp::aterm_appl& default_value_ActTrue()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_ActTrue());
  return t;
}

// RegTransOrNil
inline
const atermpp::aterm_appl& default_value_RegTransOrNil()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_RegTransOrNil(), default_value_RegFrm());
  return t;
}

// GlobVarSpec
inline
const atermpp::aterm_appl& default_value_GlobVarSpec()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_GlobVarSpec(), default_value_List());
  return t;
}

// UntypedIdentifierAssignment
inline
const atermpp::aterm_appl& default_value_UntypedIdentifierAssignment()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(function_symbol_UntypedIdentifierAssignment(), default_value_String(), default_value_DataExpr());
  return t;
}

// SortExpr
inline
const atermpp::aterm_appl& default_value_SortExpr()
{
  return default_value_SortId();
}

// SortConsType
inline
const atermpp::aterm_appl& default_value_SortConsType()
{
  return default_value_SortList();
}

// DataExpr
inline
const atermpp::aterm_appl& default_value_DataExpr()
{
  return default_value_DataVarId();
}

// BindingOperator
inline
const atermpp::aterm_appl& default_value_BindingOperator()
{
  return default_value_Forall();
}

// WhrDecl
inline
const atermpp::aterm_appl& default_value_WhrDecl()
{
  return default_value_DataVarIdInit();
}

// SortDecl
inline
const atermpp::aterm_appl& default_value_SortDecl()
{
  return default_value_SortId();
}

// DataExprOrNil
inline
const atermpp::aterm_appl& default_value_DataExprOrNil()
{
  return default_value_DataExpr();
}

// ProcExpr
inline
const atermpp::aterm_appl& default_value_ProcExpr()
{
  return default_value_Action();
}

// MultActOrDelta
inline
const atermpp::aterm_appl& default_value_MultActOrDelta()
{
  return default_value_MultAct();
}

// ProcInit
inline
const atermpp::aterm_appl& default_value_ProcInit()
{
  return default_value_ProcessInit();
}

// StateFrm
inline
const atermpp::aterm_appl& default_value_StateFrm()
{
  return default_value_DataExpr();
}

// RegFrm
inline
const atermpp::aterm_appl& default_value_RegFrm()
{
  return default_value_ActFrm();
}

// ActFrm
inline
const atermpp::aterm_appl& default_value_ActFrm()
{
  return default_value_DataExpr();
}

// ParamIdOrAction
inline
const atermpp::aterm_appl& default_value_ParamIdOrAction()
{
  return default_value_UntypedAction();
}

// ActionRenameRuleRHS
inline
const atermpp::aterm_appl& default_value_ActionRenameRuleRHS()
{
  return default_value_UntypedAction();
}

// FixPoint
inline
const atermpp::aterm_appl& default_value_FixPoint()
{
  return default_value_Mu();
}

// PBExpr
inline
const atermpp::aterm_appl& default_value_PBExpr()
{
  return default_value_DataExpr();
}

// BooleanExpression
inline
const atermpp::aterm_appl& default_value_BooleanExpression()
{
  return default_value_BooleanTrue();
}
//--- end generated constructors ---//

//----------------------------------------------------------------------------------------------//
// Part 2: static variables containing default values.
//----------------------------------------------------------------------------------------------//

struct default_values
{
//--- start generated variables ---//
static atermpp::aterm_appl BooleanOr;
  static atermpp::aterm_appl StateOr;
  static atermpp::aterm_appl Hide;
  static atermpp::aterm_appl SortArrow;
  static atermpp::aterm_appl ProcessAssignment;
  static atermpp::aterm_appl Forall;
  static atermpp::aterm_appl CommExpr;
  static atermpp::aterm_appl StateNot;
  static atermpp::aterm_appl UntypedSetBagComp;
  static atermpp::aterm_appl SortFSet;
  static atermpp::aterm_appl StateImp;
  static atermpp::aterm_appl PBESExists;
  static atermpp::aterm_appl PBESImp;
  static atermpp::aterm_appl Binder;
  static atermpp::aterm_appl SortRef;
  static atermpp::aterm_appl ProcEqnSpec;
  static atermpp::aterm_appl StateForall;
  static atermpp::aterm_appl BooleanImp;
  static atermpp::aterm_appl SortId;
  static atermpp::aterm_appl UntypedAction;
  static atermpp::aterm_appl StateNu;
  static atermpp::aterm_appl RegNil;
  static atermpp::aterm_appl DataSpec;
  static atermpp::aterm_appl UntypedActMultAct;
  static atermpp::aterm_appl Tau;
  static atermpp::aterm_appl StateYaledTimed;
  static atermpp::aterm_appl SortCons;
  static atermpp::aterm_appl DataEqnSpec;
  static atermpp::aterm_appl LinearProcessSummand;
  static atermpp::aterm_appl SortSpec;
  static atermpp::aterm_appl ActionRenameRules;
  static atermpp::aterm_appl UntypedParamId;
  static atermpp::aterm_appl BooleanEquation;
  static atermpp::aterm_appl ConsSpec;
  static atermpp::aterm_appl SortList;
  static atermpp::aterm_appl Sum;
  static atermpp::aterm_appl DataVarId;
  static atermpp::aterm_appl ProcVarId;
  static atermpp::aterm_appl ProcessInit;
  static atermpp::aterm_appl UntypedIdentifier;
  static atermpp::aterm_appl BooleanFalse;
  static atermpp::aterm_appl BES;
  static atermpp::aterm_appl MapSpec;
  static atermpp::aterm_appl IfThen;
  static atermpp::aterm_appl BooleanAnd;
  static atermpp::aterm_appl LinProcSpec;
  static atermpp::aterm_appl Choice;
  static atermpp::aterm_appl LinearProcessInit;
  static atermpp::aterm_appl MultAct;
  static atermpp::aterm_appl PropVarInst;
  static atermpp::aterm_appl BagComp;
  static atermpp::aterm_appl StateDelay;
  static atermpp::aterm_appl RegAlt;
  static atermpp::aterm_appl UntypedMultAct;
  static atermpp::aterm_appl StructCons;
  static atermpp::aterm_appl Mu;
  static atermpp::aterm_appl PBEqnSpec;
  static atermpp::aterm_appl ActNot;
  static atermpp::aterm_appl BooleanTrue;
  static atermpp::aterm_appl Block;
  static atermpp::aterm_appl Rename;
  static atermpp::aterm_appl Exists;
  static atermpp::aterm_appl Sync;
  static atermpp::aterm_appl ActExists;
  static atermpp::aterm_appl ProcSpec;
  static atermpp::aterm_appl UntypedSortsPossible;
  static atermpp::aterm_appl StateMu;
  static atermpp::aterm_appl StateFalse;
  static atermpp::aterm_appl PBESFalse;
  static atermpp::aterm_appl PBESForall;
  static atermpp::aterm_appl StateTrue;
  static atermpp::aterm_appl BInit;
  static atermpp::aterm_appl UntypedSortUnknown;
  static atermpp::aterm_appl RegTrans;
  static atermpp::aterm_appl StateDelayTimed;
  static atermpp::aterm_appl Nu;
  static atermpp::aterm_appl SortStruct;
  static atermpp::aterm_appl AtTime;
  static atermpp::aterm_appl ActOr;
  static atermpp::aterm_appl Comm;
  static atermpp::aterm_appl BooleanNot;
  static atermpp::aterm_appl Delta;
  static atermpp::aterm_appl ActMultAct;
  static atermpp::aterm_appl StateAnd;
  static atermpp::aterm_appl LMerge;
  static atermpp::aterm_appl SetComp;
  static atermpp::aterm_appl ActForall;
  static atermpp::aterm_appl RenameExpr;
  static atermpp::aterm_appl Merge;
  static atermpp::aterm_appl ActSpec;
  static atermpp::aterm_appl BooleanVariable;
  static atermpp::aterm_appl Action;
  static atermpp::aterm_appl PBESAnd;
  static atermpp::aterm_appl Lambda;
  static atermpp::aterm_appl StateMust;
  static atermpp::aterm_appl Seq;
  static atermpp::aterm_appl DataVarIdInit;
  static atermpp::aterm_appl Process;
  static atermpp::aterm_appl ActAnd;
  static atermpp::aterm_appl ActionRenameSpec;
  static atermpp::aterm_appl PBES;
  static atermpp::aterm_appl StateVar;
  static atermpp::aterm_appl ActionRenameRule;
  static atermpp::aterm_appl RegSeq;
  static atermpp::aterm_appl LinearProcess;
  static atermpp::aterm_appl ActAt;
  static atermpp::aterm_appl DataEqn;
  static atermpp::aterm_appl PBESNot;
  static atermpp::aterm_appl StateExists;
  static atermpp::aterm_appl StateMay;
  static atermpp::aterm_appl PBESTrue;
  static atermpp::aterm_appl MultActName;
  static atermpp::aterm_appl IfThenElse;
  static atermpp::aterm_appl Nil;
  static atermpp::aterm_appl ProcEqn;
  static atermpp::aterm_appl StructProj;
  static atermpp::aterm_appl PBEqn;
  static atermpp::aterm_appl Whr;
  static atermpp::aterm_appl OpId;
  static atermpp::aterm_appl SortSet;
  static atermpp::aterm_appl ActFalse;
  static atermpp::aterm_appl ActId;
  static atermpp::aterm_appl StateYaled;
  static atermpp::aterm_appl PBESOr;
  static atermpp::aterm_appl UntypedProcessAssignment;
  static atermpp::aterm_appl SortFBag;
  static atermpp::aterm_appl Allow;
  static atermpp::aterm_appl PropVarDecl;
  static atermpp::aterm_appl ActImp;
  static atermpp::aterm_appl SortBag;
  static atermpp::aterm_appl PBInit;
  static atermpp::aterm_appl ActTrue;
  static atermpp::aterm_appl RegTransOrNil;
  static atermpp::aterm_appl GlobVarSpec;
  static atermpp::aterm_appl UntypedIdentifierAssignment;
//--- end generated variables ---//
};

} // namespace detail

} // namespace core

} // namespace mcrl2

#endif // MCRL2_CORE_DETAIL_DEFAULT_VALUES_H
