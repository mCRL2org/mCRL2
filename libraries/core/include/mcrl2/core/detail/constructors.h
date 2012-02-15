// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/detail/constructors.h
/// \brief Add your file description here.

#ifndef LPS_DETAIL_CONSTRUCTORS
#define LPS_DETAIL_CONSTRUCTORS

#include "mcrl2/core/detail/struct_core.h" // gsString2ATermAppl

namespace mcrl2
{

namespace core
{

namespace detail
{

// String
inline
ATermAppl initConstructString(ATermAppl& t)
{
  t = 0;
  ATprotect(reinterpret_cast<ATerm*>(&t));
  t = gsString2ATermAppl("@NoValue");
  return t;
}

inline
ATermAppl constructString()
{
  static ATermAppl t = initConstructString(t);
  return t;
}

// List
inline
ATermList constructList()
{
  return ATmakeList0();
}

//--- start generated code ---//
ATermAppl constructBooleanOr();
ATermAppl constructStateOr();
ATermAppl constructHide();
ATermAppl constructSortArrow();
ATermAppl constructProcessAssignment();
ATermAppl constructForall();
ATermAppl constructCommExpr();
ATermAppl constructStateNot();
ATermAppl constructBooleanFalse();
ATermAppl constructSortFSet();
ATermAppl constructStateImp();
ATermAppl constructPBESExists();
ATermAppl constructPBESImp();
ATermAppl constructBinder();
ATermAppl constructSortsPossible();
ATermAppl constructSortRef();
ATermAppl constructProcEqnSpec();
ATermAppl constructStateForall();
ATermAppl constructBooleanImp();
ATermAppl constructSortId();
ATermAppl constructStateNu();
ATermAppl constructRegNil();
ATermAppl constructDataSpec();
ATermAppl constructTau();
ATermAppl constructStateYaledTimed();
ATermAppl constructSortCons();
ATermAppl constructDataEqnSpec();
ATermAppl constructLinearProcessSummand();
ATermAppl constructSortSpec();
ATermAppl constructActionRenameRules();
ATermAppl constructBooleanEquation();
ATermAppl constructConsSpec();
ATermAppl constructSortList();
ATermAppl constructSum();
ATermAppl constructDataVarId();
ATermAppl constructProcVarId();
ATermAppl constructProcessInit();
ATermAppl constructBES();
ATermAppl constructMapSpec();
ATermAppl constructStateYaled();
ATermAppl constructBooleanAnd();
ATermAppl constructLinProcSpec();
ATermAppl constructSetBagComp();
ATermAppl constructChoice();
ATermAppl constructLinearProcessInit();
ATermAppl constructMultAct();
ATermAppl constructPropVarInst();
ATermAppl constructBagComp();
ATermAppl constructStateDelay();
ATermAppl constructIdAssignment();
ATermAppl constructRegAlt();
ATermAppl constructStructCons();
ATermAppl constructIdInit();
ATermAppl constructMu();
ATermAppl constructPBEqnSpec();
ATermAppl constructActNot();
ATermAppl constructBooleanTrue();
ATermAppl constructBlock();
ATermAppl constructRename();
ATermAppl constructExists();
ATermAppl constructSync();
ATermAppl constructActExists();
ATermAppl constructProcSpec();
ATermAppl constructStateMu();
ATermAppl constructStateFalse();
ATermAppl constructPBESForall();
ATermAppl constructStateTrue();
ATermAppl constructBInit();
ATermAppl constructPBESFalse();
ATermAppl constructDataAppl();
ATermAppl constructRegTrans();
ATermAppl constructStateDelayTimed();
ATermAppl constructNu();
ATermAppl constructSortStruct();
ATermAppl constructAtTime();
ATermAppl constructActOr();
ATermAppl constructComm();
ATermAppl constructBooleanNot();
ATermAppl constructDelta();
ATermAppl constructStateAnd();
ATermAppl constructLMerge();
ATermAppl constructSetComp();
ATermAppl constructActForall();
ATermAppl constructRenameExpr();
ATermAppl constructMerge();
ATermAppl constructIfThen();
ATermAppl constructBooleanVariable();
ATermAppl constructAction();
ATermAppl constructPBESAnd();
ATermAppl constructLambda();
ATermAppl constructStateMust();
ATermAppl constructSeq();
ATermAppl constructDataVarIdInit();
ATermAppl constructProcess();
ATermAppl constructActAnd();
ATermAppl constructActionRenameSpec();
ATermAppl constructPBES();
ATermAppl constructStateVar();
ATermAppl constructActionRenameRule();
ATermAppl constructLinearProcess();
ATermAppl constructActAt();
ATermAppl constructDataEqn();
ATermAppl constructPBESNot();
ATermAppl constructStateExists();
ATermAppl constructStateMay();
ATermAppl constructParamId();
ATermAppl constructPBESTrue();
ATermAppl constructMultActName();
ATermAppl constructIfThenElse();
ATermAppl constructNil();
ATermAppl constructProcEqn();
ATermAppl constructStructProj();
ATermAppl constructPBEqn();
ATermAppl constructWhr();
ATermAppl constructOpId();
ATermAppl constructSortSet();
ATermAppl constructActFalse();
ATermAppl constructActId();
ATermAppl constructSortUnknown();
ATermAppl constructPBESOr();
ATermAppl constructRegSeq();
ATermAppl constructSortFBag();
ATermAppl constructAllow();
ATermAppl constructPropVarDecl();
ATermAppl constructActImp();
ATermAppl constructSortBag();
ATermAppl constructPBInit();
ATermAppl constructActTrue();
ATermAppl constructRegTransOrNil();
ATermAppl constructGlobVarSpec();
ATermAppl constructActSpec();
ATermAppl constructId();
ATermAppl constructSortExpr();
ATermAppl constructSortConsType();
ATermAppl constructStringOrNil();
ATermAppl constructDataExpr();
ATermAppl constructBindingOperator();
ATermAppl constructWhrDecl();
ATermAppl constructSortDecl();
ATermAppl constructDataExprOrNil();
ATermAppl constructParamIdOrAction();
ATermAppl constructProcExpr();
ATermAppl constructMultActOrDelta();
ATermAppl constructProcInit();
ATermAppl constructStateFrm();
ATermAppl constructRegFrm();
ATermAppl constructActFrm();
ATermAppl constructActionRenameRuleRHS();
ATermAppl constructFixPoint();
ATermAppl constructPBExpr();
ATermAppl constructBooleanExpression();

// BooleanOr
inline
ATermAppl constructBooleanOr()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_BooleanOr(), reinterpret_cast<ATerm>(constructBooleanExpression()), reinterpret_cast<ATerm>(constructBooleanExpression()))));
  return t;
}

// StateOr
inline
ATermAppl constructStateOr()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_StateOr(), reinterpret_cast<ATerm>(constructStateFrm()), reinterpret_cast<ATerm>(constructStateFrm()))));
  return t;
}

// Hide
inline
ATermAppl constructHide()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_Hide(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructProcExpr()))));
  return t;
}

// SortArrow
inline
ATermAppl constructSortArrow()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_SortArrow(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructSortExpr()))));
  return t;
}

// ProcessAssignment
inline
ATermAppl constructProcessAssignment()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_ProcessAssignment(), reinterpret_cast<ATerm>(constructProcVarId()), reinterpret_cast<ATerm>(constructList()))));
  return t;
}

// Forall
inline
ATermAppl constructForall()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl0(function_symbol_Forall())));
  return t;
}

// CommExpr
inline
ATermAppl constructCommExpr()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_CommExpr(), reinterpret_cast<ATerm>(constructMultActName()), reinterpret_cast<ATerm>(constructStringOrNil()))));
  return t;
}

// StateNot
inline
ATermAppl constructStateNot()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl1(function_symbol_StateNot(), reinterpret_cast<ATerm>(constructStateFrm()))));
  return t;
}

// BooleanFalse
inline
ATermAppl constructBooleanFalse()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl0(function_symbol_BooleanFalse())));
  return t;
}

// SortFSet
inline
ATermAppl constructSortFSet()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl0(function_symbol_SortFSet())));
  return t;
}

// StateImp
inline
ATermAppl constructStateImp()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_StateImp(), reinterpret_cast<ATerm>(constructStateFrm()), reinterpret_cast<ATerm>(constructStateFrm()))));
  return t;
}

// PBESExists
inline
ATermAppl constructPBESExists()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_PBESExists(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructPBExpr()))));
  return t;
}

// PBESImp
inline
ATermAppl constructPBESImp()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_PBESImp(), reinterpret_cast<ATerm>(constructPBExpr()), reinterpret_cast<ATerm>(constructPBExpr()))));
  return t;
}

// Binder
inline
ATermAppl constructBinder()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl3(function_symbol_Binder(), reinterpret_cast<ATerm>(constructBindingOperator()), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructDataExpr()))));
  return t;
}

// SortsPossible
inline
ATermAppl constructSortsPossible()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl1(function_symbol_SortsPossible(), reinterpret_cast<ATerm>(constructList()))));
  return t;
}

// SortRef
inline
ATermAppl constructSortRef()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_SortRef(), reinterpret_cast<ATerm>(constructString()), reinterpret_cast<ATerm>(constructSortExpr()))));
  return t;
}

// ProcEqnSpec
inline
ATermAppl constructProcEqnSpec()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl1(function_symbol_ProcEqnSpec(), reinterpret_cast<ATerm>(constructList()))));
  return t;
}

// StateForall
inline
ATermAppl constructStateForall()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_StateForall(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructStateFrm()))));
  return t;
}

// BooleanImp
inline
ATermAppl constructBooleanImp()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_BooleanImp(), reinterpret_cast<ATerm>(constructBooleanExpression()), reinterpret_cast<ATerm>(constructBooleanExpression()))));
  return t;
}

// SortId
inline
ATermAppl constructSortId()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl1(function_symbol_SortId(), reinterpret_cast<ATerm>(constructString()))));
  return t;
}

// StateNu
inline
ATermAppl constructStateNu()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl3(function_symbol_StateNu(), reinterpret_cast<ATerm>(constructString()), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructStateFrm()))));
  return t;
}

// RegNil
inline
ATermAppl constructRegNil()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl0(function_symbol_RegNil())));
  return t;
}

// DataSpec
inline
ATermAppl constructDataSpec()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl4(function_symbol_DataSpec(), reinterpret_cast<ATerm>(constructSortSpec()), reinterpret_cast<ATerm>(constructConsSpec()), reinterpret_cast<ATerm>(constructMapSpec()), reinterpret_cast<ATerm>(constructDataEqnSpec()))));
  return t;
}

// Tau
inline
ATermAppl constructTau()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl0(function_symbol_Tau())));
  return t;
}

// StateYaledTimed
inline
ATermAppl constructStateYaledTimed()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl1(function_symbol_StateYaledTimed(), reinterpret_cast<ATerm>(constructDataExpr()))));
  return t;
}

// SortCons
inline
ATermAppl constructSortCons()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_SortCons(), reinterpret_cast<ATerm>(constructSortConsType()), reinterpret_cast<ATerm>(constructSortExpr()))));
  return t;
}

// DataEqnSpec
inline
ATermAppl constructDataEqnSpec()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl1(function_symbol_DataEqnSpec(), reinterpret_cast<ATerm>(constructList()))));
  return t;
}

// LinearProcessSummand
inline
ATermAppl constructLinearProcessSummand()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl5(function_symbol_LinearProcessSummand(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructDataExpr()), reinterpret_cast<ATerm>(constructMultActOrDelta()), reinterpret_cast<ATerm>(constructDataExprOrNil()), reinterpret_cast<ATerm>(constructList()))));
  return t;
}

// SortSpec
inline
ATermAppl constructSortSpec()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl1(function_symbol_SortSpec(), reinterpret_cast<ATerm>(constructList()))));
  return t;
}

// ActionRenameRules
inline
ATermAppl constructActionRenameRules()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl1(function_symbol_ActionRenameRules(), reinterpret_cast<ATerm>(constructList()))));
  return t;
}

// BooleanEquation
inline
ATermAppl constructBooleanEquation()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl3(function_symbol_BooleanEquation(), reinterpret_cast<ATerm>(constructFixPoint()), reinterpret_cast<ATerm>(constructBooleanVariable()), reinterpret_cast<ATerm>(constructBooleanExpression()))));
  return t;
}

// ConsSpec
inline
ATermAppl constructConsSpec()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl1(function_symbol_ConsSpec(), reinterpret_cast<ATerm>(constructList()))));
  return t;
}

// SortList
inline
ATermAppl constructSortList()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl0(function_symbol_SortList())));
  return t;
}

// Sum
inline
ATermAppl constructSum()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_Sum(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructProcExpr()))));
  return t;
}

// DataVarId
inline
ATermAppl constructDataVarId()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_DataVarId(), reinterpret_cast<ATerm>(constructString()), reinterpret_cast<ATerm>(constructSortExpr()))));
  return t;
}

// ProcVarId
inline
ATermAppl constructProcVarId()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_ProcVarId(), reinterpret_cast<ATerm>(constructString()), reinterpret_cast<ATerm>(constructList()))));
  return t;
}

// ProcessInit
inline
ATermAppl constructProcessInit()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl1(function_symbol_ProcessInit(), reinterpret_cast<ATerm>(constructProcExpr()))));
  return t;
}

// BES
inline
ATermAppl constructBES()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_BES(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructBooleanExpression()))));
  return t;
}

// MapSpec
inline
ATermAppl constructMapSpec()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl1(function_symbol_MapSpec(), reinterpret_cast<ATerm>(constructList()))));
  return t;
}

// StateYaled
inline
ATermAppl constructStateYaled()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl0(function_symbol_StateYaled())));
  return t;
}

// BooleanAnd
inline
ATermAppl constructBooleanAnd()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_BooleanAnd(), reinterpret_cast<ATerm>(constructBooleanExpression()), reinterpret_cast<ATerm>(constructBooleanExpression()))));
  return t;
}

// LinProcSpec
inline
ATermAppl constructLinProcSpec()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl5(function_symbol_LinProcSpec(), reinterpret_cast<ATerm>(constructDataSpec()), reinterpret_cast<ATerm>(constructActSpec()), reinterpret_cast<ATerm>(constructGlobVarSpec()), reinterpret_cast<ATerm>(constructLinearProcess()), reinterpret_cast<ATerm>(constructLinearProcessInit()))));
  return t;
}

// SetBagComp
inline
ATermAppl constructSetBagComp()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl0(function_symbol_SetBagComp())));
  return t;
}

// Choice
inline
ATermAppl constructChoice()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_Choice(), reinterpret_cast<ATerm>(constructProcExpr()), reinterpret_cast<ATerm>(constructProcExpr()))));
  return t;
}

// LinearProcessInit
inline
ATermAppl constructLinearProcessInit()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl1(function_symbol_LinearProcessInit(), reinterpret_cast<ATerm>(constructList()))));
  return t;
}

// MultAct
inline
ATermAppl constructMultAct()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl1(function_symbol_MultAct(), reinterpret_cast<ATerm>(constructList()))));
  return t;
}

// PropVarInst
inline
ATermAppl constructPropVarInst()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_PropVarInst(), reinterpret_cast<ATerm>(constructString()), reinterpret_cast<ATerm>(constructList()))));
  return t;
}

// BagComp
inline
ATermAppl constructBagComp()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl0(function_symbol_BagComp())));
  return t;
}

// StateDelay
inline
ATermAppl constructStateDelay()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl0(function_symbol_StateDelay())));
  return t;
}

// IdAssignment
inline
ATermAppl constructIdAssignment()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_IdAssignment(), reinterpret_cast<ATerm>(constructString()), reinterpret_cast<ATerm>(constructList()))));
  return t;
}

// RegAlt
inline
ATermAppl constructRegAlt()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_RegAlt(), reinterpret_cast<ATerm>(constructRegFrm()), reinterpret_cast<ATerm>(constructRegFrm()))));
  return t;
}

// StructCons
inline
ATermAppl constructStructCons()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl3(function_symbol_StructCons(), reinterpret_cast<ATerm>(constructString()), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructStringOrNil()))));
  return t;
}

// IdInit
inline
ATermAppl constructIdInit()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_IdInit(), reinterpret_cast<ATerm>(constructString()), reinterpret_cast<ATerm>(constructDataExpr()))));
  return t;
}

// Mu
inline
ATermAppl constructMu()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl0(function_symbol_Mu())));
  return t;
}

// PBEqnSpec
inline
ATermAppl constructPBEqnSpec()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl1(function_symbol_PBEqnSpec(), reinterpret_cast<ATerm>(constructList()))));
  return t;
}

// ActNot
inline
ATermAppl constructActNot()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl1(function_symbol_ActNot(), reinterpret_cast<ATerm>(constructActFrm()))));
  return t;
}

// BooleanTrue
inline
ATermAppl constructBooleanTrue()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl0(function_symbol_BooleanTrue())));
  return t;
}

// Block
inline
ATermAppl constructBlock()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_Block(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructProcExpr()))));
  return t;
}

// Rename
inline
ATermAppl constructRename()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_Rename(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructProcExpr()))));
  return t;
}

// Exists
inline
ATermAppl constructExists()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl0(function_symbol_Exists())));
  return t;
}

// Sync
inline
ATermAppl constructSync()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_Sync(), reinterpret_cast<ATerm>(constructProcExpr()), reinterpret_cast<ATerm>(constructProcExpr()))));
  return t;
}

// ActExists
inline
ATermAppl constructActExists()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_ActExists(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructActFrm()))));
  return t;
}

// ProcSpec
inline
ATermAppl constructProcSpec()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl5(function_symbol_ProcSpec(), reinterpret_cast<ATerm>(constructDataSpec()), reinterpret_cast<ATerm>(constructActSpec()), reinterpret_cast<ATerm>(constructGlobVarSpec()), reinterpret_cast<ATerm>(constructProcEqnSpec()), reinterpret_cast<ATerm>(constructProcInit()))));
  return t;
}

// StateMu
inline
ATermAppl constructStateMu()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl3(function_symbol_StateMu(), reinterpret_cast<ATerm>(constructString()), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructStateFrm()))));
  return t;
}

// StateFalse
inline
ATermAppl constructStateFalse()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl0(function_symbol_StateFalse())));
  return t;
}

// PBESForall
inline
ATermAppl constructPBESForall()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_PBESForall(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructPBExpr()))));
  return t;
}

// StateTrue
inline
ATermAppl constructStateTrue()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl0(function_symbol_StateTrue())));
  return t;
}

// BInit
inline
ATermAppl constructBInit()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_BInit(), reinterpret_cast<ATerm>(constructProcExpr()), reinterpret_cast<ATerm>(constructProcExpr()))));
  return t;
}

// PBESFalse
inline
ATermAppl constructPBESFalse()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl0(function_symbol_PBESFalse())));
  return t;
}

// DataAppl
inline
ATermAppl constructDataAppl()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_DataAppl(), reinterpret_cast<ATerm>(constructDataExpr()), reinterpret_cast<ATerm>(constructList()))));
  return t;
}

// RegTrans
inline
ATermAppl constructRegTrans()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl1(function_symbol_RegTrans(), reinterpret_cast<ATerm>(constructRegFrm()))));
  return t;
}

// StateDelayTimed
inline
ATermAppl constructStateDelayTimed()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl1(function_symbol_StateDelayTimed(), reinterpret_cast<ATerm>(constructDataExpr()))));
  return t;
}

// Nu
inline
ATermAppl constructNu()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl0(function_symbol_Nu())));
  return t;
}

// SortStruct
inline
ATermAppl constructSortStruct()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl1(function_symbol_SortStruct(), reinterpret_cast<ATerm>(constructList()))));
  return t;
}

// AtTime
inline
ATermAppl constructAtTime()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_AtTime(), reinterpret_cast<ATerm>(constructProcExpr()), reinterpret_cast<ATerm>(constructDataExpr()))));
  return t;
}

// ActOr
inline
ATermAppl constructActOr()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_ActOr(), reinterpret_cast<ATerm>(constructActFrm()), reinterpret_cast<ATerm>(constructActFrm()))));
  return t;
}

// Comm
inline
ATermAppl constructComm()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_Comm(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructProcExpr()))));
  return t;
}

// BooleanNot
inline
ATermAppl constructBooleanNot()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl1(function_symbol_BooleanNot(), reinterpret_cast<ATerm>(constructBooleanExpression()))));
  return t;
}

// Delta
inline
ATermAppl constructDelta()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl0(function_symbol_Delta())));
  return t;
}

// StateAnd
inline
ATermAppl constructStateAnd()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_StateAnd(), reinterpret_cast<ATerm>(constructStateFrm()), reinterpret_cast<ATerm>(constructStateFrm()))));
  return t;
}

// LMerge
inline
ATermAppl constructLMerge()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_LMerge(), reinterpret_cast<ATerm>(constructProcExpr()), reinterpret_cast<ATerm>(constructProcExpr()))));
  return t;
}

// SetComp
inline
ATermAppl constructSetComp()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl0(function_symbol_SetComp())));
  return t;
}

// ActForall
inline
ATermAppl constructActForall()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_ActForall(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructActFrm()))));
  return t;
}

// RenameExpr
inline
ATermAppl constructRenameExpr()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_RenameExpr(), reinterpret_cast<ATerm>(constructString()), reinterpret_cast<ATerm>(constructString()))));
  return t;
}

// Merge
inline
ATermAppl constructMerge()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_Merge(), reinterpret_cast<ATerm>(constructProcExpr()), reinterpret_cast<ATerm>(constructProcExpr()))));
  return t;
}

// IfThen
inline
ATermAppl constructIfThen()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_IfThen(), reinterpret_cast<ATerm>(constructDataExpr()), reinterpret_cast<ATerm>(constructProcExpr()))));
  return t;
}

// BooleanVariable
inline
ATermAppl constructBooleanVariable()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl1(function_symbol_BooleanVariable(), reinterpret_cast<ATerm>(constructString()))));
  return t;
}

// Action
inline
ATermAppl constructAction()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_Action(), reinterpret_cast<ATerm>(constructActId()), reinterpret_cast<ATerm>(constructList()))));
  return t;
}

// PBESAnd
inline
ATermAppl constructPBESAnd()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_PBESAnd(), reinterpret_cast<ATerm>(constructPBExpr()), reinterpret_cast<ATerm>(constructPBExpr()))));
  return t;
}

// Lambda
inline
ATermAppl constructLambda()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl0(function_symbol_Lambda())));
  return t;
}

// StateMust
inline
ATermAppl constructStateMust()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_StateMust(), reinterpret_cast<ATerm>(constructRegFrm()), reinterpret_cast<ATerm>(constructStateFrm()))));
  return t;
}

// Seq
inline
ATermAppl constructSeq()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_Seq(), reinterpret_cast<ATerm>(constructProcExpr()), reinterpret_cast<ATerm>(constructProcExpr()))));
  return t;
}

// DataVarIdInit
inline
ATermAppl constructDataVarIdInit()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_DataVarIdInit(), reinterpret_cast<ATerm>(constructDataVarId()), reinterpret_cast<ATerm>(constructDataExpr()))));
  return t;
}

// Process
inline
ATermAppl constructProcess()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_Process(), reinterpret_cast<ATerm>(constructProcVarId()), reinterpret_cast<ATerm>(constructList()))));
  return t;
}

// ActAnd
inline
ATermAppl constructActAnd()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_ActAnd(), reinterpret_cast<ATerm>(constructActFrm()), reinterpret_cast<ATerm>(constructActFrm()))));
  return t;
}

// ActionRenameSpec
inline
ATermAppl constructActionRenameSpec()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl3(function_symbol_ActionRenameSpec(), reinterpret_cast<ATerm>(constructDataSpec()), reinterpret_cast<ATerm>(constructActSpec()), reinterpret_cast<ATerm>(constructActionRenameRules()))));
  return t;
}

// PBES
inline
ATermAppl constructPBES()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl4(function_symbol_PBES(), reinterpret_cast<ATerm>(constructDataSpec()), reinterpret_cast<ATerm>(constructGlobVarSpec()), reinterpret_cast<ATerm>(constructPBEqnSpec()), reinterpret_cast<ATerm>(constructPBInit()))));
  return t;
}

// StateVar
inline
ATermAppl constructStateVar()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_StateVar(), reinterpret_cast<ATerm>(constructString()), reinterpret_cast<ATerm>(constructList()))));
  return t;
}

// ActionRenameRule
inline
ATermAppl constructActionRenameRule()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl4(function_symbol_ActionRenameRule(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructDataExpr()), reinterpret_cast<ATerm>(constructParamIdOrAction()), reinterpret_cast<ATerm>(constructActionRenameRuleRHS()))));
  return t;
}

// LinearProcess
inline
ATermAppl constructLinearProcess()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_LinearProcess(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructList()))));
  return t;
}

// ActAt
inline
ATermAppl constructActAt()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_ActAt(), reinterpret_cast<ATerm>(constructActFrm()), reinterpret_cast<ATerm>(constructDataExpr()))));
  return t;
}

// DataEqn
inline
ATermAppl constructDataEqn()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl4(function_symbol_DataEqn(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructDataExpr()), reinterpret_cast<ATerm>(constructDataExpr()), reinterpret_cast<ATerm>(constructDataExpr()))));
  return t;
}

// PBESNot
inline
ATermAppl constructPBESNot()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl1(function_symbol_PBESNot(), reinterpret_cast<ATerm>(constructPBExpr()))));
  return t;
}

// StateExists
inline
ATermAppl constructStateExists()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_StateExists(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructStateFrm()))));
  return t;
}

// StateMay
inline
ATermAppl constructStateMay()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_StateMay(), reinterpret_cast<ATerm>(constructRegFrm()), reinterpret_cast<ATerm>(constructStateFrm()))));
  return t;
}

// ParamId
inline
ATermAppl constructParamId()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_ParamId(), reinterpret_cast<ATerm>(constructString()), reinterpret_cast<ATerm>(constructList()))));
  return t;
}

// PBESTrue
inline
ATermAppl constructPBESTrue()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl0(function_symbol_PBESTrue())));
  return t;
}

// MultActName
inline
ATermAppl constructMultActName()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl1(function_symbol_MultActName(), reinterpret_cast<ATerm>(constructList()))));
  return t;
}

// IfThenElse
inline
ATermAppl constructIfThenElse()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl3(function_symbol_IfThenElse(), reinterpret_cast<ATerm>(constructDataExpr()), reinterpret_cast<ATerm>(constructProcExpr()), reinterpret_cast<ATerm>(constructProcExpr()))));
  return t;
}

// Nil
inline
ATermAppl constructNil()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl0(function_symbol_Nil())));
  return t;
}

// ProcEqn
inline
ATermAppl constructProcEqn()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl3(function_symbol_ProcEqn(), reinterpret_cast<ATerm>(constructProcVarId()), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructProcExpr()))));
  return t;
}

// StructProj
inline
ATermAppl constructStructProj()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_StructProj(), reinterpret_cast<ATerm>(constructStringOrNil()), reinterpret_cast<ATerm>(constructSortExpr()))));
  return t;
}

// PBEqn
inline
ATermAppl constructPBEqn()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl3(function_symbol_PBEqn(), reinterpret_cast<ATerm>(constructFixPoint()), reinterpret_cast<ATerm>(constructPropVarDecl()), reinterpret_cast<ATerm>(constructPBExpr()))));
  return t;
}

// Whr
inline
ATermAppl constructWhr()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_Whr(), reinterpret_cast<ATerm>(constructDataExpr()), reinterpret_cast<ATerm>(constructList()))));
  return t;
}

// OpId
inline
ATermAppl constructOpId()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_OpId(), reinterpret_cast<ATerm>(constructString()), reinterpret_cast<ATerm>(constructSortExpr()))));
  return t;
}

// SortSet
inline
ATermAppl constructSortSet()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl0(function_symbol_SortSet())));
  return t;
}

// ActFalse
inline
ATermAppl constructActFalse()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl0(function_symbol_ActFalse())));
  return t;
}

// ActId
inline
ATermAppl constructActId()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_ActId(), reinterpret_cast<ATerm>(constructString()), reinterpret_cast<ATerm>(constructList()))));
  return t;
}

// SortUnknown
inline
ATermAppl constructSortUnknown()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl0(function_symbol_SortUnknown())));
  return t;
}

// PBESOr
inline
ATermAppl constructPBESOr()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_PBESOr(), reinterpret_cast<ATerm>(constructPBExpr()), reinterpret_cast<ATerm>(constructPBExpr()))));
  return t;
}

// RegSeq
inline
ATermAppl constructRegSeq()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_RegSeq(), reinterpret_cast<ATerm>(constructRegFrm()), reinterpret_cast<ATerm>(constructRegFrm()))));
  return t;
}

// SortFBag
inline
ATermAppl constructSortFBag()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl0(function_symbol_SortFBag())));
  return t;
}

// Allow
inline
ATermAppl constructAllow()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_Allow(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructProcExpr()))));
  return t;
}

// PropVarDecl
inline
ATermAppl constructPropVarDecl()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_PropVarDecl(), reinterpret_cast<ATerm>(constructString()), reinterpret_cast<ATerm>(constructList()))));
  return t;
}

// ActImp
inline
ATermAppl constructActImp()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl2(function_symbol_ActImp(), reinterpret_cast<ATerm>(constructActFrm()), reinterpret_cast<ATerm>(constructActFrm()))));
  return t;
}

// SortBag
inline
ATermAppl constructSortBag()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl0(function_symbol_SortBag())));
  return t;
}

// PBInit
inline
ATermAppl constructPBInit()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl1(function_symbol_PBInit(), reinterpret_cast<ATerm>(constructPropVarInst()))));
  return t;
}

// ActTrue
inline
ATermAppl constructActTrue()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl0(function_symbol_ActTrue())));
  return t;
}

// RegTransOrNil
inline
ATermAppl constructRegTransOrNil()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl1(function_symbol_RegTransOrNil(), reinterpret_cast<ATerm>(constructRegFrm()))));
  return t;
}

// GlobVarSpec
inline
ATermAppl constructGlobVarSpec()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl1(function_symbol_GlobVarSpec(), reinterpret_cast<ATerm>(constructList()))));
  return t;
}

// ActSpec
inline
ATermAppl constructActSpec()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl1(function_symbol_ActSpec(), reinterpret_cast<ATerm>(constructList()))));
  return t;
}

// Id
inline
ATermAppl constructId()
{
  static atermpp::aterm_appl t = core::detail::initialise_static_expression(t, atermpp::aterm_appl(ATmakeAppl1(function_symbol_Id(), reinterpret_cast<ATerm>(constructString()))));
  return t;
}

// SortExpr
inline
ATermAppl constructSortExpr()
{
  return constructSortId();
}

// SortConsType
inline
ATermAppl constructSortConsType()
{
  return constructSortList();
}

// StringOrNil
inline
ATermAppl constructStringOrNil()
{
  return constructString();
}

// DataExpr
inline
ATermAppl constructDataExpr()
{
  return constructId();
}

// BindingOperator
inline
ATermAppl constructBindingOperator()
{
  return constructSetBagComp();
}

// WhrDecl
inline
ATermAppl constructWhrDecl()
{
  return constructIdInit();
}

// SortDecl
inline
ATermAppl constructSortDecl()
{
  return constructSortId();
}

// DataExprOrNil
inline
ATermAppl constructDataExprOrNil()
{
  return constructDataExpr();
}

// ParamIdOrAction
inline
ATermAppl constructParamIdOrAction()
{
  return constructParamId();
}

// ProcExpr
inline
ATermAppl constructProcExpr()
{
  return constructParamId();
}

// MultActOrDelta
inline
ATermAppl constructMultActOrDelta()
{
  return constructMultAct();
}

// ProcInit
inline
ATermAppl constructProcInit()
{
  return constructProcessInit();
}

// StateFrm
inline
ATermAppl constructStateFrm()
{
  return constructDataExpr();
}

// RegFrm
inline
ATermAppl constructRegFrm()
{
  return constructActFrm();
}

// ActFrm
inline
ATermAppl constructActFrm()
{
  return constructMultAct();
}

// ActionRenameRuleRHS
inline
ATermAppl constructActionRenameRuleRHS()
{
  return constructParamId();
}

// FixPoint
inline
ATermAppl constructFixPoint()
{
  return constructMu();
}

// PBExpr
inline
ATermAppl constructPBExpr()
{
  return constructDataExpr();
}

// BooleanExpression
inline
ATermAppl constructBooleanExpression()
{
  return constructBooleanTrue();
}

//--- end generated code ---//


} // namespace detail

} // namespace core

} // mcrl2

#endif // LPS_DETAIL_CONSTRUCTORS
