// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/detail/default_values.h
/// \brief add your file description here.

#ifndef MCRL2_CORE_DETAIL_DEFAULT_VALUES_H
#define MCRL2_CORE_DETAIL_DEFAULT_VALUES_H

#include "mcrl2/core/identifier_string.h"

namespace mcrl2::core::detail
{

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
const atermpp::aterm& default_value_String()
{
  static atermpp::aterm t = core::identifier_string("@NoValue");
  return t;
}

inline
const atermpp::aterm& default_value_StringOrEmpty()
{
  static atermpp::aterm t = core::identifier_string("@NoValue");
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
const atermpp::aterm& default_value_SortCons();
const atermpp::aterm& default_value_SortStruct();
const atermpp::aterm& default_value_SortArrow();
const atermpp::aterm& default_value_UntypedSortUnknown();
const atermpp::aterm& default_value_UntypedSortsPossible();
const atermpp::aterm& default_value_UntypedSortVariable();
const atermpp::aterm& default_value_SortId();
const atermpp::aterm& default_value_SortList();
const atermpp::aterm& default_value_SortSet();
const atermpp::aterm& default_value_SortBag();
const atermpp::aterm& default_value_SortFSet();
const atermpp::aterm& default_value_SortFBag();
const atermpp::aterm& default_value_StructCons();
const atermpp::aterm& default_value_StructProj();
const atermpp::aterm& default_value_Binder();
const atermpp::aterm& default_value_Whr();
const atermpp::aterm& default_value_UntypedIdentifier();
const atermpp::aterm& default_value_DataVarId();
const atermpp::aterm& default_value_OpId();
const atermpp::aterm& default_value_UntypedDataParameter();
const atermpp::aterm& default_value_Forall();
const atermpp::aterm& default_value_Exists();
const atermpp::aterm& default_value_SetComp();
const atermpp::aterm& default_value_BagComp();
const atermpp::aterm& default_value_Lambda();
const atermpp::aterm& default_value_UntypedSetBagComp();
const atermpp::aterm& default_value_DataVarIdInit();
const atermpp::aterm& default_value_UntypedIdentifierAssignment();
const atermpp::aterm& default_value_DataSpec();
const atermpp::aterm& default_value_SortSpec();
const atermpp::aterm& default_value_ConsSpec();
const atermpp::aterm& default_value_MapSpec();
const atermpp::aterm& default_value_DataEqnSpec();
const atermpp::aterm& default_value_SortRef();
const atermpp::aterm& default_value_DataEqn();
const atermpp::aterm& default_value_MultAct();
const atermpp::aterm& default_value_TimedMultAct();
const atermpp::aterm& default_value_UntypedMultiAction();
const atermpp::aterm& default_value_Action();
const atermpp::aterm& default_value_ActId();
const atermpp::aterm& default_value_Process();
const atermpp::aterm& default_value_ProcessAssignment();
const atermpp::aterm& default_value_Delta();
const atermpp::aterm& default_value_Tau();
const atermpp::aterm& default_value_Sum();
const atermpp::aterm& default_value_Block();
const atermpp::aterm& default_value_Hide();
const atermpp::aterm& default_value_Rename();
const atermpp::aterm& default_value_Comm();
const atermpp::aterm& default_value_Allow();
const atermpp::aterm& default_value_Sync();
const atermpp::aterm& default_value_AtTime();
const atermpp::aterm& default_value_Seq();
const atermpp::aterm& default_value_IfThen();
const atermpp::aterm& default_value_IfThenElse();
const atermpp::aterm& default_value_BInit();
const atermpp::aterm& default_value_Merge();
const atermpp::aterm& default_value_LMerge();
const atermpp::aterm& default_value_Choice();
const atermpp::aterm& default_value_StochasticOperator();
const atermpp::aterm& default_value_UntypedProcessAssignment();
const atermpp::aterm& default_value_ProcVarId();
const atermpp::aterm& default_value_MultActName();
const atermpp::aterm& default_value_RenameExpr();
const atermpp::aterm& default_value_CommExpr();
const atermpp::aterm& default_value_ProcSpec();
const atermpp::aterm& default_value_ActSpec();
const atermpp::aterm& default_value_GlobVarSpec();
const atermpp::aterm& default_value_ProcEqnSpec();
const atermpp::aterm& default_value_ProcEqn();
const atermpp::aterm& default_value_ProcessInit();
const atermpp::aterm& default_value_Distribution();
const atermpp::aterm& default_value_LinProcSpec();
const atermpp::aterm& default_value_LinearProcess();
const atermpp::aterm& default_value_LinearProcessSummand();
const atermpp::aterm& default_value_LinearProcessInit();
const atermpp::aterm& default_value_StateTrue();
const atermpp::aterm& default_value_StateFalse();
const atermpp::aterm& default_value_StateNot();
const atermpp::aterm& default_value_StateMinus();
const atermpp::aterm& default_value_StateAnd();
const atermpp::aterm& default_value_StateOr();
const atermpp::aterm& default_value_StateImp();
const atermpp::aterm& default_value_StatePlus();
const atermpp::aterm& default_value_StateConstantMultiply();
const atermpp::aterm& default_value_StateConstantMultiplyAlt();
const atermpp::aterm& default_value_StateForall();
const atermpp::aterm& default_value_StateExists();
const atermpp::aterm& default_value_StateInfimum();
const atermpp::aterm& default_value_StateSupremum();
const atermpp::aterm& default_value_StateSum();
const atermpp::aterm& default_value_StateMust();
const atermpp::aterm& default_value_StateMay();
const atermpp::aterm& default_value_StateYaled();
const atermpp::aterm& default_value_StateYaledTimed();
const atermpp::aterm& default_value_StateDelay();
const atermpp::aterm& default_value_StateDelayTimed();
const atermpp::aterm& default_value_StateVar();
const atermpp::aterm& default_value_StateNu();
const atermpp::aterm& default_value_StateMu();
const atermpp::aterm& default_value_RegNil();
const atermpp::aterm& default_value_RegSeq();
const atermpp::aterm& default_value_RegAlt();
const atermpp::aterm& default_value_RegTrans();
const atermpp::aterm& default_value_RegTransOrNil();
const atermpp::aterm& default_value_UntypedRegFrm();
const atermpp::aterm& default_value_ActTrue();
const atermpp::aterm& default_value_ActFalse();
const atermpp::aterm& default_value_ActNot();
const atermpp::aterm& default_value_ActAnd();
const atermpp::aterm& default_value_ActOr();
const atermpp::aterm& default_value_ActImp();
const atermpp::aterm& default_value_ActForall();
const atermpp::aterm& default_value_ActExists();
const atermpp::aterm& default_value_ActAt();
const atermpp::aterm& default_value_ActMultAct();
const atermpp::aterm& default_value_ActionRenameRules();
const atermpp::aterm& default_value_ActionRenameRule();
const atermpp::aterm& default_value_ActionRenameSpec();
const atermpp::aterm& default_value_PBES();
const atermpp::aterm& default_value_PBEqnSpec();
const atermpp::aterm& default_value_PBInit();
const atermpp::aterm& default_value_PBEqn();
const atermpp::aterm& default_value_Mu();
const atermpp::aterm& default_value_Nu();
const atermpp::aterm& default_value_PropVarDecl();
const atermpp::aterm& default_value_PBESTrue();
const atermpp::aterm& default_value_PBESFalse();
const atermpp::aterm& default_value_PBESNot();
const atermpp::aterm& default_value_PBESAnd();
const atermpp::aterm& default_value_PBESOr();
const atermpp::aterm& default_value_PBESImp();
const atermpp::aterm& default_value_PBESForall();
const atermpp::aterm& default_value_PBESExists();
const atermpp::aterm& default_value_PropVarInst();
const atermpp::aterm& default_value_PRES();
const atermpp::aterm& default_value_PREqnSpec();
const atermpp::aterm& default_value_PRInit();
const atermpp::aterm& default_value_PREqn();
const atermpp::aterm& default_value_PRESTrue();
const atermpp::aterm& default_value_PRESFalse();
const atermpp::aterm& default_value_PRESMinus();
const atermpp::aterm& default_value_PRESAnd();
const atermpp::aterm& default_value_PRESOr();
const atermpp::aterm& default_value_PRESImp();
const atermpp::aterm& default_value_PRESPlus();
const atermpp::aterm& default_value_PRESConstantMultiply();
const atermpp::aterm& default_value_PRESConstantMultiplyAlt();
const atermpp::aterm& default_value_PRESInfimum();
const atermpp::aterm& default_value_PRESSupremum();
const atermpp::aterm& default_value_PRESSum();
const atermpp::aterm& default_value_PRESEqInf();
const atermpp::aterm& default_value_PRESEqNInf();
const atermpp::aterm& default_value_PRESCondSm();
const atermpp::aterm& default_value_PRESCondEq();
const atermpp::aterm& default_value_SortExpr();
const atermpp::aterm& default_value_SortConsType();
const atermpp::aterm& default_value_DataExpr();
const atermpp::aterm& default_value_BindingOperator();
const atermpp::aterm& default_value_WhrDecl();
const atermpp::aterm& default_value_SortDecl();
const atermpp::aterm& default_value_ProcExpr();
const atermpp::aterm& default_value_MultActOrDelta();
const atermpp::aterm& default_value_ProcInit();
const atermpp::aterm& default_value_StateFrm();
const atermpp::aterm& default_value_RegFrm();
const atermpp::aterm& default_value_ActFrm();
const atermpp::aterm& default_value_ParamIdOrAction();
const atermpp::aterm& default_value_ActionRenameRuleRHS();
const atermpp::aterm& default_value_FixPoint();
const atermpp::aterm& default_value_PBExpr();
const atermpp::aterm& default_value_PRExpr();

// SortCons
inline
const atermpp::aterm& default_value_SortCons()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_SortCons(), default_value_SortConsType(), default_value_SortExpr());
  return t;
}

// SortStruct
inline
const atermpp::aterm& default_value_SortStruct()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_SortStruct(), default_value_List(default_value_StructCons()));
  return t;
}

// SortArrow
inline
const atermpp::aterm& default_value_SortArrow()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_SortArrow(), default_value_List(default_value_SortExpr()), default_value_SortExpr());
  return t;
}

// UntypedSortUnknown
inline
const atermpp::aterm& default_value_UntypedSortUnknown()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_UntypedSortUnknown());
  return t;
}

// UntypedSortsPossible
inline
const atermpp::aterm& default_value_UntypedSortsPossible()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_UntypedSortsPossible(), default_value_List(default_value_SortExpr()));
  return t;
}

// UntypedSortVariable
inline
const atermpp::aterm& default_value_UntypedSortVariable()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_UntypedSortVariable(), default_value_Number());
  return t;
}

// SortId
inline
const atermpp::aterm& default_value_SortId()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_SortId(), default_value_String());
  return t;
}

// SortList
inline
const atermpp::aterm& default_value_SortList()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_SortList());
  return t;
}

// SortSet
inline
const atermpp::aterm& default_value_SortSet()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_SortSet());
  return t;
}

// SortBag
inline
const atermpp::aterm& default_value_SortBag()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_SortBag());
  return t;
}

// SortFSet
inline
const atermpp::aterm& default_value_SortFSet()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_SortFSet());
  return t;
}

// SortFBag
inline
const atermpp::aterm& default_value_SortFBag()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_SortFBag());
  return t;
}

// StructCons
inline
const atermpp::aterm& default_value_StructCons()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_StructCons(), default_value_String(), default_value_List(), default_value_StringOrEmpty());
  return t;
}

// StructProj
inline
const atermpp::aterm& default_value_StructProj()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_StructProj(), default_value_StringOrEmpty(), default_value_SortExpr());
  return t;
}

// Binder
inline
const atermpp::aterm& default_value_Binder()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_Binder(), default_value_BindingOperator(), default_value_List(default_value_DataVarId()), default_value_DataExpr());
  return t;
}

// Whr
inline
const atermpp::aterm& default_value_Whr()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_Whr(), default_value_DataExpr(), default_value_List(default_value_WhrDecl()));
  return t;
}

// UntypedIdentifier
inline
const atermpp::aterm& default_value_UntypedIdentifier()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_UntypedIdentifier(), default_value_String());
  return t;
}

// DataVarId
inline
const atermpp::aterm& default_value_DataVarId()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_DataVarId(), default_value_String(), default_value_SortExpr());
  return t;
}

// OpId
inline
const atermpp::aterm& default_value_OpId()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_OpId(), default_value_String(), default_value_SortExpr(), default_value_Number());
  return t;
}

// UntypedDataParameter
inline
const atermpp::aterm& default_value_UntypedDataParameter()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_UntypedDataParameter(), default_value_String(), default_value_List());
  return t;
}

// Forall
inline
const atermpp::aterm& default_value_Forall()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_Forall());
  return t;
}

// Exists
inline
const atermpp::aterm& default_value_Exists()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_Exists());
  return t;
}

// SetComp
inline
const atermpp::aterm& default_value_SetComp()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_SetComp());
  return t;
}

// BagComp
inline
const atermpp::aterm& default_value_BagComp()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_BagComp());
  return t;
}

// Lambda
inline
const atermpp::aterm& default_value_Lambda()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_Lambda());
  return t;
}

// UntypedSetBagComp
inline
const atermpp::aterm& default_value_UntypedSetBagComp()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_UntypedSetBagComp());
  return t;
}

// DataVarIdInit
inline
const atermpp::aterm& default_value_DataVarIdInit()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_DataVarIdInit(), default_value_DataVarId(), default_value_DataExpr());
  return t;
}

// UntypedIdentifierAssignment
inline
const atermpp::aterm& default_value_UntypedIdentifierAssignment()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_UntypedIdentifierAssignment(), default_value_String(), default_value_DataExpr());
  return t;
}

// DataSpec
inline
const atermpp::aterm& default_value_DataSpec()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_DataSpec(), default_value_SortSpec(), default_value_ConsSpec(), default_value_MapSpec(), default_value_DataEqnSpec());
  return t;
}

// SortSpec
inline
const atermpp::aterm& default_value_SortSpec()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_SortSpec(), default_value_List());
  return t;
}

// ConsSpec
inline
const atermpp::aterm& default_value_ConsSpec()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_ConsSpec(), default_value_List());
  return t;
}

// MapSpec
inline
const atermpp::aterm& default_value_MapSpec()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_MapSpec(), default_value_List());
  return t;
}

// DataEqnSpec
inline
const atermpp::aterm& default_value_DataEqnSpec()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_DataEqnSpec(), default_value_List());
  return t;
}

// SortRef
inline
const atermpp::aterm& default_value_SortRef()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_SortRef(), default_value_SortId(), default_value_SortExpr());
  return t;
}

// DataEqn
inline
const atermpp::aterm& default_value_DataEqn()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_DataEqn(), default_value_List(), default_value_DataExpr(), default_value_DataExpr(), default_value_DataExpr());
  return t;
}

// MultAct
inline
const atermpp::aterm& default_value_MultAct()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_MultAct(), default_value_List());
  return t;
}

// TimedMultAct
inline
const atermpp::aterm& default_value_TimedMultAct()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_TimedMultAct(), default_value_List(), default_value_DataExpr());
  return t;
}

// UntypedMultiAction
inline
const atermpp::aterm& default_value_UntypedMultiAction()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_UntypedMultiAction(), default_value_List());
  return t;
}

// Action
inline
const atermpp::aterm& default_value_Action()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_Action(), default_value_ActId(), default_value_List());
  return t;
}

// ActId
inline
const atermpp::aterm& default_value_ActId()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_ActId(), default_value_String(), default_value_List());
  return t;
}

// Process
inline
const atermpp::aterm& default_value_Process()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_Process(), default_value_ProcVarId(), default_value_List());
  return t;
}

// ProcessAssignment
inline
const atermpp::aterm& default_value_ProcessAssignment()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_ProcessAssignment(), default_value_ProcVarId(), default_value_List());
  return t;
}

// Delta
inline
const atermpp::aterm& default_value_Delta()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_Delta());
  return t;
}

// Tau
inline
const atermpp::aterm& default_value_Tau()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_Tau());
  return t;
}

// Sum
inline
const atermpp::aterm& default_value_Sum()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_Sum(), default_value_List(default_value_DataVarId()), default_value_ProcExpr());
  return t;
}

// Block
inline
const atermpp::aterm& default_value_Block()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_Block(), default_value_List(), default_value_ProcExpr());
  return t;
}

// Hide
inline
const atermpp::aterm& default_value_Hide()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_Hide(), default_value_List(), default_value_ProcExpr());
  return t;
}

// Rename
inline
const atermpp::aterm& default_value_Rename()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_Rename(), default_value_List(), default_value_ProcExpr());
  return t;
}

// Comm
inline
const atermpp::aterm& default_value_Comm()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_Comm(), default_value_List(), default_value_ProcExpr());
  return t;
}

// Allow
inline
const atermpp::aterm& default_value_Allow()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_Allow(), default_value_List(), default_value_ProcExpr());
  return t;
}

// Sync
inline
const atermpp::aterm& default_value_Sync()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_Sync(), default_value_ProcExpr(), default_value_ProcExpr());
  return t;
}

// AtTime
inline
const atermpp::aterm& default_value_AtTime()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_AtTime(), default_value_ProcExpr(), default_value_DataExpr());
  return t;
}

// Seq
inline
const atermpp::aterm& default_value_Seq()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_Seq(), default_value_ProcExpr(), default_value_ProcExpr());
  return t;
}

// IfThen
inline
const atermpp::aterm& default_value_IfThen()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_IfThen(), default_value_DataExpr(), default_value_ProcExpr());
  return t;
}

// IfThenElse
inline
const atermpp::aterm& default_value_IfThenElse()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_IfThenElse(), default_value_DataExpr(), default_value_ProcExpr(), default_value_ProcExpr());
  return t;
}

// BInit
inline
const atermpp::aterm& default_value_BInit()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_BInit(), default_value_ProcExpr(), default_value_ProcExpr());
  return t;
}

// Merge
inline
const atermpp::aterm& default_value_Merge()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_Merge(), default_value_ProcExpr(), default_value_ProcExpr());
  return t;
}

// LMerge
inline
const atermpp::aterm& default_value_LMerge()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_LMerge(), default_value_ProcExpr(), default_value_ProcExpr());
  return t;
}

// Choice
inline
const atermpp::aterm& default_value_Choice()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_Choice(), default_value_ProcExpr(), default_value_ProcExpr());
  return t;
}

// StochasticOperator
inline
const atermpp::aterm& default_value_StochasticOperator()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_StochasticOperator(), default_value_List(default_value_DataVarId()), default_value_DataExpr(), default_value_ProcExpr());
  return t;
}

// UntypedProcessAssignment
inline
const atermpp::aterm& default_value_UntypedProcessAssignment()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_UntypedProcessAssignment(), default_value_String(), default_value_List());
  return t;
}

// ProcVarId
inline
const atermpp::aterm& default_value_ProcVarId()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_ProcVarId(), default_value_String(), default_value_List());
  return t;
}

// MultActName
inline
const atermpp::aterm& default_value_MultActName()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_MultActName(), default_value_List(default_value_String()));
  return t;
}

// RenameExpr
inline
const atermpp::aterm& default_value_RenameExpr()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_RenameExpr(), default_value_String(), default_value_String());
  return t;
}

// CommExpr
inline
const atermpp::aterm& default_value_CommExpr()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_CommExpr(), default_value_MultActName(), default_value_String());
  return t;
}

// ProcSpec
inline
const atermpp::aterm& default_value_ProcSpec()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_ProcSpec(), default_value_DataSpec(), default_value_ActSpec(), default_value_GlobVarSpec(), default_value_ProcEqnSpec(), default_value_ProcInit());
  return t;
}

// ActSpec
inline
const atermpp::aterm& default_value_ActSpec()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_ActSpec(), default_value_List());
  return t;
}

// GlobVarSpec
inline
const atermpp::aterm& default_value_GlobVarSpec()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_GlobVarSpec(), default_value_List());
  return t;
}

// ProcEqnSpec
inline
const atermpp::aterm& default_value_ProcEqnSpec()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_ProcEqnSpec(), default_value_List());
  return t;
}

// ProcEqn
inline
const atermpp::aterm& default_value_ProcEqn()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_ProcEqn(), default_value_ProcVarId(), default_value_List(), default_value_ProcExpr());
  return t;
}

// ProcessInit
inline
const atermpp::aterm& default_value_ProcessInit()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_ProcessInit(), default_value_ProcExpr());
  return t;
}

// Distribution
inline
const atermpp::aterm& default_value_Distribution()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_Distribution(), default_value_List(), default_value_DataExpr());
  return t;
}

// LinProcSpec
inline
const atermpp::aterm& default_value_LinProcSpec()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_LinProcSpec(), default_value_DataSpec(), default_value_ActSpec(), default_value_GlobVarSpec(), default_value_LinearProcess(), default_value_LinearProcessInit());
  return t;
}

// LinearProcess
inline
const atermpp::aterm& default_value_LinearProcess()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_LinearProcess(), default_value_List(), default_value_List());
  return t;
}

// LinearProcessSummand
inline
const atermpp::aterm& default_value_LinearProcessSummand()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_LinearProcessSummand(), default_value_List(), default_value_DataExpr(), default_value_MultActOrDelta(), default_value_DataExpr(), default_value_List(), default_value_Distribution());
  return t;
}

// LinearProcessInit
inline
const atermpp::aterm& default_value_LinearProcessInit()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_LinearProcessInit(), default_value_List(), default_value_Distribution());
  return t;
}

// StateTrue
inline
const atermpp::aterm& default_value_StateTrue()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_StateTrue());
  return t;
}

// StateFalse
inline
const atermpp::aterm& default_value_StateFalse()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_StateFalse());
  return t;
}

// StateNot
inline
const atermpp::aterm& default_value_StateNot()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_StateNot(), default_value_StateFrm());
  return t;
}

// StateMinus
inline
const atermpp::aterm& default_value_StateMinus()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_StateMinus(), default_value_StateFrm());
  return t;
}

// StateAnd
inline
const atermpp::aterm& default_value_StateAnd()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_StateAnd(), default_value_StateFrm(), default_value_StateFrm());
  return t;
}

// StateOr
inline
const atermpp::aterm& default_value_StateOr()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_StateOr(), default_value_StateFrm(), default_value_StateFrm());
  return t;
}

// StateImp
inline
const atermpp::aterm& default_value_StateImp()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_StateImp(), default_value_StateFrm(), default_value_StateFrm());
  return t;
}

// StatePlus
inline
const atermpp::aterm& default_value_StatePlus()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_StatePlus(), default_value_StateFrm(), default_value_StateFrm());
  return t;
}

// StateConstantMultiply
inline
const atermpp::aterm& default_value_StateConstantMultiply()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_StateConstantMultiply(), default_value_DataExpr(), default_value_StateFrm());
  return t;
}

// StateConstantMultiplyAlt
inline
const atermpp::aterm& default_value_StateConstantMultiplyAlt()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_StateConstantMultiplyAlt(), default_value_StateFrm(), default_value_DataExpr());
  return t;
}

// StateForall
inline
const atermpp::aterm& default_value_StateForall()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_StateForall(), default_value_List(default_value_DataVarId()), default_value_StateFrm());
  return t;
}

// StateExists
inline
const atermpp::aterm& default_value_StateExists()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_StateExists(), default_value_List(default_value_DataVarId()), default_value_StateFrm());
  return t;
}

// StateInfimum
inline
const atermpp::aterm& default_value_StateInfimum()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_StateInfimum(), default_value_List(default_value_DataVarId()), default_value_StateFrm());
  return t;
}

// StateSupremum
inline
const atermpp::aterm& default_value_StateSupremum()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_StateSupremum(), default_value_List(default_value_DataVarId()), default_value_StateFrm());
  return t;
}

// StateSum
inline
const atermpp::aterm& default_value_StateSum()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_StateSum(), default_value_List(default_value_DataVarId()), default_value_StateFrm());
  return t;
}

// StateMust
inline
const atermpp::aterm& default_value_StateMust()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_StateMust(), default_value_RegFrm(), default_value_StateFrm());
  return t;
}

// StateMay
inline
const atermpp::aterm& default_value_StateMay()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_StateMay(), default_value_RegFrm(), default_value_StateFrm());
  return t;
}

// StateYaled
inline
const atermpp::aterm& default_value_StateYaled()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_StateYaled());
  return t;
}

// StateYaledTimed
inline
const atermpp::aterm& default_value_StateYaledTimed()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_StateYaledTimed(), default_value_DataExpr());
  return t;
}

// StateDelay
inline
const atermpp::aterm& default_value_StateDelay()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_StateDelay());
  return t;
}

// StateDelayTimed
inline
const atermpp::aterm& default_value_StateDelayTimed()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_StateDelayTimed(), default_value_DataExpr());
  return t;
}

// StateVar
inline
const atermpp::aterm& default_value_StateVar()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_StateVar(), default_value_String(), default_value_List());
  return t;
}

// StateNu
inline
const atermpp::aterm& default_value_StateNu()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_StateNu(), default_value_String(), default_value_List(), default_value_StateFrm());
  return t;
}

// StateMu
inline
const atermpp::aterm& default_value_StateMu()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_StateMu(), default_value_String(), default_value_List(), default_value_StateFrm());
  return t;
}

// RegNil
inline
const atermpp::aterm& default_value_RegNil()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_RegNil());
  return t;
}

// RegSeq
inline
const atermpp::aterm& default_value_RegSeq()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_RegSeq(), default_value_RegFrm(), default_value_RegFrm());
  return t;
}

// RegAlt
inline
const atermpp::aterm& default_value_RegAlt()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_RegAlt(), default_value_RegFrm(), default_value_RegFrm());
  return t;
}

// RegTrans
inline
const atermpp::aterm& default_value_RegTrans()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_RegTrans(), default_value_RegFrm());
  return t;
}

// RegTransOrNil
inline
const atermpp::aterm& default_value_RegTransOrNil()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_RegTransOrNil(), default_value_RegFrm());
  return t;
}

// UntypedRegFrm
inline
const atermpp::aterm& default_value_UntypedRegFrm()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_UntypedRegFrm(), default_value_String(), default_value_RegFrm(), default_value_RegFrm());
  return t;
}

// ActTrue
inline
const atermpp::aterm& default_value_ActTrue()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_ActTrue());
  return t;
}

// ActFalse
inline
const atermpp::aterm& default_value_ActFalse()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_ActFalse());
  return t;
}

// ActNot
inline
const atermpp::aterm& default_value_ActNot()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_ActNot(), default_value_ActFrm());
  return t;
}

// ActAnd
inline
const atermpp::aterm& default_value_ActAnd()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_ActAnd(), default_value_ActFrm(), default_value_ActFrm());
  return t;
}

// ActOr
inline
const atermpp::aterm& default_value_ActOr()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_ActOr(), default_value_ActFrm(), default_value_ActFrm());
  return t;
}

// ActImp
inline
const atermpp::aterm& default_value_ActImp()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_ActImp(), default_value_ActFrm(), default_value_ActFrm());
  return t;
}

// ActForall
inline
const atermpp::aterm& default_value_ActForall()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_ActForall(), default_value_List(default_value_DataVarId()), default_value_ActFrm());
  return t;
}

// ActExists
inline
const atermpp::aterm& default_value_ActExists()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_ActExists(), default_value_List(default_value_DataVarId()), default_value_ActFrm());
  return t;
}

// ActAt
inline
const atermpp::aterm& default_value_ActAt()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_ActAt(), default_value_ActFrm(), default_value_DataExpr());
  return t;
}

// ActMultAct
inline
const atermpp::aterm& default_value_ActMultAct()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_ActMultAct(), default_value_List());
  return t;
}

// ActionRenameRules
inline
const atermpp::aterm& default_value_ActionRenameRules()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_ActionRenameRules(), default_value_List());
  return t;
}

// ActionRenameRule
inline
const atermpp::aterm& default_value_ActionRenameRule()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_ActionRenameRule(), default_value_List(), default_value_DataExpr(), default_value_ParamIdOrAction(), default_value_ActionRenameRuleRHS());
  return t;
}

// ActionRenameSpec
inline
const atermpp::aterm& default_value_ActionRenameSpec()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_ActionRenameSpec(), default_value_DataSpec(), default_value_ActSpec(), default_value_ActionRenameRules());
  return t;
}

// PBES
inline
const atermpp::aterm& default_value_PBES()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_PBES(), default_value_DataSpec(), default_value_GlobVarSpec(), default_value_PBEqnSpec(), default_value_PBInit());
  return t;
}

// PBEqnSpec
inline
const atermpp::aterm& default_value_PBEqnSpec()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_PBEqnSpec(), default_value_List());
  return t;
}

// PBInit
inline
const atermpp::aterm& default_value_PBInit()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_PBInit(), default_value_PropVarInst());
  return t;
}

// PBEqn
inline
const atermpp::aterm& default_value_PBEqn()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_PBEqn(), default_value_FixPoint(), default_value_PropVarDecl(), default_value_PBExpr());
  return t;
}

// Mu
inline
const atermpp::aterm& default_value_Mu()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_Mu());
  return t;
}

// Nu
inline
const atermpp::aterm& default_value_Nu()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_Nu());
  return t;
}

// PropVarDecl
inline
const atermpp::aterm& default_value_PropVarDecl()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_PropVarDecl(), default_value_String(), default_value_List());
  return t;
}

// PBESTrue
inline
const atermpp::aterm& default_value_PBESTrue()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_PBESTrue());
  return t;
}

// PBESFalse
inline
const atermpp::aterm& default_value_PBESFalse()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_PBESFalse());
  return t;
}

// PBESNot
inline
const atermpp::aterm& default_value_PBESNot()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_PBESNot(), default_value_PBExpr());
  return t;
}

// PBESAnd
inline
const atermpp::aterm& default_value_PBESAnd()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_PBESAnd(), default_value_PBExpr(), default_value_PBExpr());
  return t;
}

// PBESOr
inline
const atermpp::aterm& default_value_PBESOr()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_PBESOr(), default_value_PBExpr(), default_value_PBExpr());
  return t;
}

// PBESImp
inline
const atermpp::aterm& default_value_PBESImp()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_PBESImp(), default_value_PBExpr(), default_value_PBExpr());
  return t;
}

// PBESForall
inline
const atermpp::aterm& default_value_PBESForall()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_PBESForall(), default_value_List(default_value_DataVarId()), default_value_PBExpr());
  return t;
}

// PBESExists
inline
const atermpp::aterm& default_value_PBESExists()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_PBESExists(), default_value_List(default_value_DataVarId()), default_value_PBExpr());
  return t;
}

// PropVarInst
inline
const atermpp::aterm& default_value_PropVarInst()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_PropVarInst(), default_value_String(), default_value_List());
  return t;
}

// PRES
inline
const atermpp::aterm& default_value_PRES()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_PRES(), default_value_DataSpec(), default_value_GlobVarSpec(), default_value_PREqnSpec(), default_value_PRInit());
  return t;
}

// PREqnSpec
inline
const atermpp::aterm& default_value_PREqnSpec()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_PREqnSpec(), default_value_List());
  return t;
}

// PRInit
inline
const atermpp::aterm& default_value_PRInit()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_PRInit(), default_value_PropVarInst());
  return t;
}

// PREqn
inline
const atermpp::aterm& default_value_PREqn()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_PREqn(), default_value_FixPoint(), default_value_PropVarDecl(), default_value_PRExpr());
  return t;
}

// PRESTrue
inline
const atermpp::aterm& default_value_PRESTrue()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_PRESTrue());
  return t;
}

// PRESFalse
inline
const atermpp::aterm& default_value_PRESFalse()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_PRESFalse());
  return t;
}

// PRESMinus
inline
const atermpp::aterm& default_value_PRESMinus()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_PRESMinus(), default_value_PRExpr());
  return t;
}

// PRESAnd
inline
const atermpp::aterm& default_value_PRESAnd()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_PRESAnd(), default_value_PRExpr(), default_value_PRExpr());
  return t;
}

// PRESOr
inline
const atermpp::aterm& default_value_PRESOr()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_PRESOr(), default_value_PRExpr(), default_value_PRExpr());
  return t;
}

// PRESImp
inline
const atermpp::aterm& default_value_PRESImp()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_PRESImp(), default_value_PRExpr(), default_value_PRExpr());
  return t;
}

// PRESPlus
inline
const atermpp::aterm& default_value_PRESPlus()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_PRESPlus(), default_value_PRExpr(), default_value_PRExpr());
  return t;
}

// PRESConstantMultiply
inline
const atermpp::aterm& default_value_PRESConstantMultiply()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_PRESConstantMultiply(), default_value_DataExpr(), default_value_PRExpr());
  return t;
}

// PRESConstantMultiplyAlt
inline
const atermpp::aterm& default_value_PRESConstantMultiplyAlt()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_PRESConstantMultiplyAlt(), default_value_PRExpr(), default_value_DataExpr());
  return t;
}

// PRESInfimum
inline
const atermpp::aterm& default_value_PRESInfimum()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_PRESInfimum(), default_value_List(default_value_DataVarId()), default_value_PRExpr());
  return t;
}

// PRESSupremum
inline
const atermpp::aterm& default_value_PRESSupremum()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_PRESSupremum(), default_value_List(default_value_DataVarId()), default_value_PRExpr());
  return t;
}

// PRESSum
inline
const atermpp::aterm& default_value_PRESSum()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_PRESSum(), default_value_List(default_value_DataVarId()), default_value_PRExpr());
  return t;
}

// PRESEqInf
inline
const atermpp::aterm& default_value_PRESEqInf()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_PRESEqInf(), default_value_PRExpr());
  return t;
}

// PRESEqNInf
inline
const atermpp::aterm& default_value_PRESEqNInf()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_PRESEqNInf(), default_value_PRExpr());
  return t;
}

// PRESCondSm
inline
const atermpp::aterm& default_value_PRESCondSm()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_PRESCondSm(), default_value_PRExpr(), default_value_PRExpr(), default_value_PRExpr());
  return t;
}

// PRESCondEq
inline
const atermpp::aterm& default_value_PRESCondEq()
{
  static const atermpp::aterm t = atermpp::aterm(function_symbol_PRESCondEq(), default_value_PRExpr(), default_value_PRExpr(), default_value_PRExpr());
  return t;
}

// SortExpr
inline
const atermpp::aterm& default_value_SortExpr()
{
  return default_value_SortId();
}

// SortConsType
inline
const atermpp::aterm& default_value_SortConsType()
{
  return default_value_SortList();
}

// DataExpr
inline
const atermpp::aterm& default_value_DataExpr()
{
  return default_value_DataVarId();
}

// BindingOperator
inline
const atermpp::aterm& default_value_BindingOperator()
{
  return default_value_Forall();
}

// WhrDecl
inline
const atermpp::aterm& default_value_WhrDecl()
{
  return default_value_DataVarIdInit();
}

// SortDecl
inline
const atermpp::aterm& default_value_SortDecl()
{
  return default_value_SortId();
}

// ProcExpr
inline
const atermpp::aterm& default_value_ProcExpr()
{
  return default_value_Action();
}

// MultActOrDelta
inline
const atermpp::aterm& default_value_MultActOrDelta()
{
  return default_value_MultAct();
}

// ProcInit
inline
const atermpp::aterm& default_value_ProcInit()
{
  return default_value_ProcessInit();
}

// StateFrm
inline
const atermpp::aterm& default_value_StateFrm()
{
  return default_value_DataExpr();
}

// RegFrm
inline
const atermpp::aterm& default_value_RegFrm()
{
  return default_value_ActFrm();
}

// ActFrm
inline
const atermpp::aterm& default_value_ActFrm()
{
  return default_value_DataExpr();
}

// ParamIdOrAction
inline
const atermpp::aterm& default_value_ParamIdOrAction()
{
  return default_value_UntypedDataParameter();
}

// ActionRenameRuleRHS
inline
const atermpp::aterm& default_value_ActionRenameRuleRHS()
{
  return default_value_UntypedDataParameter();
}

// FixPoint
inline
const atermpp::aterm& default_value_FixPoint()
{
  return default_value_Mu();
}

// PBExpr
inline
const atermpp::aterm& default_value_PBExpr()
{
  return default_value_DataExpr();
}

// PRExpr
inline
const atermpp::aterm& default_value_PRExpr()
{
  return default_value_DataExpr();
}
//--- end generated constructors ---//

//----------------------------------------------------------------------------------------------//
// Part 2: static variables containing default values.
//----------------------------------------------------------------------------------------------//

struct default_values
{
//--- start generated variables ---//
static const atermpp::aterm SortCons;
  static const atermpp::aterm SortStruct;
  static const atermpp::aterm SortArrow;
  static const atermpp::aterm UntypedSortUnknown;
  static const atermpp::aterm UntypedSortsPossible;
  static const atermpp::aterm UntypedSortVariable;
  static const atermpp::aterm SortId;
  static const atermpp::aterm SortList;
  static const atermpp::aterm SortSet;
  static const atermpp::aterm SortBag;
  static const atermpp::aterm SortFSet;
  static const atermpp::aterm SortFBag;
  static const atermpp::aterm StructCons;
  static const atermpp::aterm StructProj;
  static const atermpp::aterm Binder;
  static const atermpp::aterm Whr;
  static const atermpp::aterm UntypedIdentifier;
  static const atermpp::aterm DataVarId;
  static const atermpp::aterm OpId;
  static const atermpp::aterm UntypedDataParameter;
  static const atermpp::aterm Forall;
  static const atermpp::aterm Exists;
  static const atermpp::aterm SetComp;
  static const atermpp::aterm BagComp;
  static const atermpp::aterm Lambda;
  static const atermpp::aterm UntypedSetBagComp;
  static const atermpp::aterm DataVarIdInit;
  static const atermpp::aterm UntypedIdentifierAssignment;
  static const atermpp::aterm DataSpec;
  static const atermpp::aterm SortSpec;
  static const atermpp::aterm ConsSpec;
  static const atermpp::aterm MapSpec;
  static const atermpp::aterm DataEqnSpec;
  static const atermpp::aterm SortRef;
  static const atermpp::aterm DataEqn;
  static const atermpp::aterm MultAct;
  static const atermpp::aterm TimedMultAct;
  static const atermpp::aterm UntypedMultiAction;
  static const atermpp::aterm Action;
  static const atermpp::aterm ActId;
  static const atermpp::aterm Process;
  static const atermpp::aterm ProcessAssignment;
  static const atermpp::aterm Delta;
  static const atermpp::aterm Tau;
  static const atermpp::aterm Sum;
  static const atermpp::aterm Block;
  static const atermpp::aterm Hide;
  static const atermpp::aterm Rename;
  static const atermpp::aterm Comm;
  static const atermpp::aterm Allow;
  static const atermpp::aterm Sync;
  static const atermpp::aterm AtTime;
  static const atermpp::aterm Seq;
  static const atermpp::aterm IfThen;
  static const atermpp::aterm IfThenElse;
  static const atermpp::aterm BInit;
  static const atermpp::aterm Merge;
  static const atermpp::aterm LMerge;
  static const atermpp::aterm Choice;
  static const atermpp::aterm StochasticOperator;
  static const atermpp::aterm UntypedProcessAssignment;
  static const atermpp::aterm ProcVarId;
  static const atermpp::aterm MultActName;
  static const atermpp::aterm RenameExpr;
  static const atermpp::aterm CommExpr;
  static const atermpp::aterm ProcSpec;
  static const atermpp::aterm ActSpec;
  static const atermpp::aterm GlobVarSpec;
  static const atermpp::aterm ProcEqnSpec;
  static const atermpp::aterm ProcEqn;
  static const atermpp::aterm ProcessInit;
  static const atermpp::aterm Distribution;
  static const atermpp::aterm LinProcSpec;
  static const atermpp::aterm LinearProcess;
  static const atermpp::aterm LinearProcessSummand;
  static const atermpp::aterm LinearProcessInit;
  static const atermpp::aterm StateTrue;
  static const atermpp::aterm StateFalse;
  static const atermpp::aterm StateNot;
  static const atermpp::aterm StateMinus;
  static const atermpp::aterm StateAnd;
  static const atermpp::aterm StateOr;
  static const atermpp::aterm StateImp;
  static const atermpp::aterm StatePlus;
  static const atermpp::aterm StateConstantMultiply;
  static const atermpp::aterm StateConstantMultiplyAlt;
  static const atermpp::aterm StateForall;
  static const atermpp::aterm StateExists;
  static const atermpp::aterm StateInfimum;
  static const atermpp::aterm StateSupremum;
  static const atermpp::aterm StateSum;
  static const atermpp::aterm StateMust;
  static const atermpp::aterm StateMay;
  static const atermpp::aterm StateYaled;
  static const atermpp::aterm StateYaledTimed;
  static const atermpp::aterm StateDelay;
  static const atermpp::aterm StateDelayTimed;
  static const atermpp::aterm StateVar;
  static const atermpp::aterm StateNu;
  static const atermpp::aterm StateMu;
  static const atermpp::aterm RegNil;
  static const atermpp::aterm RegSeq;
  static const atermpp::aterm RegAlt;
  static const atermpp::aterm RegTrans;
  static const atermpp::aterm RegTransOrNil;
  static const atermpp::aterm UntypedRegFrm;
  static const atermpp::aterm ActTrue;
  static const atermpp::aterm ActFalse;
  static const atermpp::aterm ActNot;
  static const atermpp::aterm ActAnd;
  static const atermpp::aterm ActOr;
  static const atermpp::aterm ActImp;
  static const atermpp::aterm ActForall;
  static const atermpp::aterm ActExists;
  static const atermpp::aterm ActAt;
  static const atermpp::aterm ActMultAct;
  static const atermpp::aterm ActionRenameRules;
  static const atermpp::aterm ActionRenameRule;
  static const atermpp::aterm ActionRenameSpec;
  static const atermpp::aterm PBES;
  static const atermpp::aterm PBEqnSpec;
  static const atermpp::aterm PBInit;
  static const atermpp::aterm PBEqn;
  static const atermpp::aterm Mu;
  static const atermpp::aterm Nu;
  static const atermpp::aterm PropVarDecl;
  static const atermpp::aterm PBESTrue;
  static const atermpp::aterm PBESFalse;
  static const atermpp::aterm PBESNot;
  static const atermpp::aterm PBESAnd;
  static const atermpp::aterm PBESOr;
  static const atermpp::aterm PBESImp;
  static const atermpp::aterm PBESForall;
  static const atermpp::aterm PBESExists;
  static const atermpp::aterm PropVarInst;
  static const atermpp::aterm PRES;
  static const atermpp::aterm PREqnSpec;
  static const atermpp::aterm PRInit;
  static const atermpp::aterm PREqn;
  static const atermpp::aterm PRESTrue;
  static const atermpp::aterm PRESFalse;
  static const atermpp::aterm PRESMinus;
  static const atermpp::aterm PRESAnd;
  static const atermpp::aterm PRESOr;
  static const atermpp::aterm PRESImp;
  static const atermpp::aterm PRESPlus;
  static const atermpp::aterm PRESConstantMultiply;
  static const atermpp::aterm PRESConstantMultiplyAlt;
  static const atermpp::aterm PRESInfimum;
  static const atermpp::aterm PRESSupremum;
  static const atermpp::aterm PRESSum;
  static const atermpp::aterm PRESEqInf;
  static const atermpp::aterm PRESEqNInf;
  static const atermpp::aterm PRESCondSm;
  static const atermpp::aterm PRESCondEq;
  static const atermpp::aterm SortExpr;
  static const atermpp::aterm SortConsType;
  static const atermpp::aterm DataExpr;
  static const atermpp::aterm BindingOperator;
  static const atermpp::aterm WhrDecl;
  static const atermpp::aterm SortDecl;
  static const atermpp::aterm ProcExpr;
  static const atermpp::aterm MultActOrDelta;
  static const atermpp::aterm ProcInit;
  static const atermpp::aterm StateFrm;
  static const atermpp::aterm RegFrm;
  static const atermpp::aterm ActFrm;
  static const atermpp::aterm ParamIdOrAction;
  static const atermpp::aterm ActionRenameRuleRHS;
  static const atermpp::aterm FixPoint;
  static const atermpp::aterm PBExpr;
  static const atermpp::aterm PRExpr;
//--- end generated variables ---//
};

} // namespace mcrl2::core::detail

#endif // MCRL2_CORE_DETAIL_DEFAULT_VALUES_H
