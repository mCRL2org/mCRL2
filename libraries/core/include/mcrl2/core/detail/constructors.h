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

inline
const atermpp::aterm_appl& constructString()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(gsString2ATermAppl("@NoValue"));
  return t;
}

inline
const atermpp::aterm_appl& constructStringOrEmpty()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(gsString2ATermAppl("@NoValue"));
  return t;
}

inline
const atermpp::aterm_list& constructList()
{
  static atermpp::aterm_list t = atermpp::aterm_list();
  return t;
}

// List
inline
atermpp::aterm_list constructList(const atermpp::aterm& t)
{
  atermpp::aterm_list result;
  result.push_front(t);
  return result;
}

//--- start generated code ---//
const atermpp::aterm_appl& constructBooleanOr();
const atermpp::aterm_appl& constructStateOr();
const atermpp::aterm_appl& constructHide();
const atermpp::aterm_appl& constructSortArrow();
const atermpp::aterm_appl& constructProcessAssignment();
const atermpp::aterm_appl& constructForall();
const atermpp::aterm_appl& constructCommExpr();
const atermpp::aterm_appl& constructStateNot();
const atermpp::aterm_appl& constructBooleanFalse();
const atermpp::aterm_appl& constructSortFSet();
const atermpp::aterm_appl& constructStateImp();
const atermpp::aterm_appl& constructPBESExists();
const atermpp::aterm_appl& constructPBESImp();
const atermpp::aterm_appl& constructBinder();
const atermpp::aterm_appl& constructSortsPossible();
const atermpp::aterm_appl& constructSortRef();
const atermpp::aterm_appl& constructProcEqnSpec();
const atermpp::aterm_appl& constructStateForall();
const atermpp::aterm_appl& constructBooleanImp();
const atermpp::aterm_appl& constructSortId();
const atermpp::aterm_appl& constructStateNu();
const atermpp::aterm_appl& constructRegNil();
const atermpp::aterm_appl& constructDataSpec();
const atermpp::aterm_appl& constructTau();
const atermpp::aterm_appl& constructStateYaledTimed();
const atermpp::aterm_appl& constructSortCons();
const atermpp::aterm_appl& constructDataEqnSpec();
const atermpp::aterm_appl& constructLinearProcessSummand();
const atermpp::aterm_appl& constructSortSpec();
const atermpp::aterm_appl& constructActionRenameRules();
const atermpp::aterm_appl& constructBooleanEquation();
const atermpp::aterm_appl& constructConsSpec();
const atermpp::aterm_appl& constructSortList();
const atermpp::aterm_appl& constructSum();
const atermpp::aterm_appl& constructDataVarId();
const atermpp::aterm_appl& constructProcVarId();
const atermpp::aterm_appl& constructProcessInit();
const atermpp::aterm_appl& constructBES();
const atermpp::aterm_appl& constructMapSpec();
const atermpp::aterm_appl& constructStateYaled();
const atermpp::aterm_appl& constructBooleanAnd();
const atermpp::aterm_appl& constructLinProcSpec();
const atermpp::aterm_appl& constructSetBagComp();
const atermpp::aterm_appl& constructChoice();
const atermpp::aterm_appl& constructLinearProcessInit();
const atermpp::aterm_appl& constructMultAct();
const atermpp::aterm_appl& constructPropVarInst();
const atermpp::aterm_appl& constructBagComp();
const atermpp::aterm_appl& constructStateDelay();
const atermpp::aterm_appl& constructIdAssignment();
const atermpp::aterm_appl& constructRegAlt();
const atermpp::aterm_appl& constructStructCons();
const atermpp::aterm_appl& constructIdInit();
const atermpp::aterm_appl& constructMu();
const atermpp::aterm_appl& constructPBEqnSpec();
const atermpp::aterm_appl& constructActNot();
const atermpp::aterm_appl& constructBooleanTrue();
const atermpp::aterm_appl& constructBlock();
const atermpp::aterm_appl& constructRename();
const atermpp::aterm_appl& constructExists();
const atermpp::aterm_appl& constructSync();
const atermpp::aterm_appl& constructActExists();
const atermpp::aterm_appl& constructProcSpec();
const atermpp::aterm_appl& constructStateMu();
const atermpp::aterm_appl& constructStateFalse();
const atermpp::aterm_appl& constructPBESForall();
const atermpp::aterm_appl& constructStateTrue();
const atermpp::aterm_appl& constructBInit();
const atermpp::aterm_appl& constructPBESFalse();
const atermpp::aterm_appl& constructDataAppl();
const atermpp::aterm_appl& constructRegTrans();
const atermpp::aterm_appl& constructStateDelayTimed();
const atermpp::aterm_appl& constructNu();
const atermpp::aterm_appl& constructSortStruct();
const atermpp::aterm_appl& constructAtTime();
const atermpp::aterm_appl& constructActOr();
const atermpp::aterm_appl& constructComm();
const atermpp::aterm_appl& constructBooleanNot();
const atermpp::aterm_appl& constructDelta();
const atermpp::aterm_appl& constructStateAnd();
const atermpp::aterm_appl& constructLMerge();
const atermpp::aterm_appl& constructSetComp();
const atermpp::aterm_appl& constructActForall();
const atermpp::aterm_appl& constructRenameExpr();
const atermpp::aterm_appl& constructMerge();
const atermpp::aterm_appl& constructIfThen();
const atermpp::aterm_appl& constructBooleanVariable();
const atermpp::aterm_appl& constructAction();
const atermpp::aterm_appl& constructPBESAnd();
const atermpp::aterm_appl& constructLambda();
const atermpp::aterm_appl& constructStateMust();
const atermpp::aterm_appl& constructSeq();
const atermpp::aterm_appl& constructDataVarIdInit();
const atermpp::aterm_appl& constructProcess();
const atermpp::aterm_appl& constructActAnd();
const atermpp::aterm_appl& constructActionRenameSpec();
const atermpp::aterm_appl& constructPBES();
const atermpp::aterm_appl& constructStateVar();
const atermpp::aterm_appl& constructActionRenameRule();
const atermpp::aterm_appl& constructLinearProcess();
const atermpp::aterm_appl& constructActAt();
const atermpp::aterm_appl& constructDataEqn();
const atermpp::aterm_appl& constructPBESNot();
const atermpp::aterm_appl& constructStateExists();
const atermpp::aterm_appl& constructStateMay();
const atermpp::aterm_appl& constructParamId();
const atermpp::aterm_appl& constructPBESTrue();
const atermpp::aterm_appl& constructMultActName();
const atermpp::aterm_appl& constructIfThenElse();
const atermpp::aterm_appl& constructNil();
const atermpp::aterm_appl& constructProcEqn();
const atermpp::aterm_appl& constructStructProj();
const atermpp::aterm_appl& constructPBEqn();
const atermpp::aterm_appl& constructWhr();
const atermpp::aterm_appl& constructOpId();
const atermpp::aterm_appl& constructSortSet();
const atermpp::aterm_appl& constructActFalse();
const atermpp::aterm_appl& constructActId();
const atermpp::aterm_appl& constructSortUnknown();
const atermpp::aterm_appl& constructPBESOr();
const atermpp::aterm_appl& constructRegSeq();
const atermpp::aterm_appl& constructSortFBag();
const atermpp::aterm_appl& constructAllow();
const atermpp::aterm_appl& constructPropVarDecl();
const atermpp::aterm_appl& constructActImp();
const atermpp::aterm_appl& constructSortBag();
const atermpp::aterm_appl& constructPBInit();
const atermpp::aterm_appl& constructActTrue();
const atermpp::aterm_appl& constructRegTransOrNil();
const atermpp::aterm_appl& constructGlobVarSpec();
const atermpp::aterm_appl& constructActSpec();
const atermpp::aterm_appl& constructId();
const atermpp::aterm_appl& constructSortExpr();
const atermpp::aterm_appl& constructSortConsType();
const atermpp::aterm_appl& constructDataExpr();
const atermpp::aterm_appl& constructBindingOperator();
const atermpp::aterm_appl& constructWhrDecl();
const atermpp::aterm_appl& constructSortDecl();
const atermpp::aterm_appl& constructDataExprOrNil();
const atermpp::aterm_appl& constructParamIdOrAction();
const atermpp::aterm_appl& constructProcExpr();
const atermpp::aterm_appl& constructMultActOrDelta();
const atermpp::aterm_appl& constructProcInit();
const atermpp::aterm_appl& constructStateFrm();
const atermpp::aterm_appl& constructRegFrm();
const atermpp::aterm_appl& constructActFrm();
const atermpp::aterm_appl& constructActionRenameRuleRHS();
const atermpp::aterm_appl& constructFixPoint();
const atermpp::aterm_appl& constructPBExpr();
const atermpp::aterm_appl& constructBooleanExpression();

// BooleanOr
inline
const atermpp::aterm_appl& constructBooleanOr()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_BooleanOr(), constructBooleanExpression(), constructBooleanExpression()));
  return t;
}

// StateOr
inline
const atermpp::aterm_appl& constructStateOr()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_StateOr(), constructStateFrm(), constructStateFrm()));
  return t;
}

// Hide
inline
const atermpp::aterm_appl& constructHide()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_Hide(), constructList(), constructProcExpr()));
  return t;
}

// SortArrow
inline
const atermpp::aterm_appl& constructSortArrow()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_SortArrow(), constructList(constructSortExpr()), constructSortExpr()));
  return t;
}

// ProcessAssignment
inline
const atermpp::aterm_appl& constructProcessAssignment()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_ProcessAssignment(), constructProcVarId(), constructList()));
  return t;
}

// Forall
inline
const atermpp::aterm_appl& constructForall()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_Forall()));
  return t;
}

// CommExpr
inline
const atermpp::aterm_appl& constructCommExpr()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_CommExpr(), constructMultActName(), constructString()));
  return t;
}

// StateNot
inline
const atermpp::aterm_appl& constructStateNot()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_StateNot(), constructStateFrm()));
  return t;
}

// BooleanFalse
inline
const atermpp::aterm_appl& constructBooleanFalse()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_BooleanFalse()));
  return t;
}

// SortFSet
inline
const atermpp::aterm_appl& constructSortFSet()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_SortFSet()));
  return t;
}

// StateImp
inline
const atermpp::aterm_appl& constructStateImp()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_StateImp(), constructStateFrm(), constructStateFrm()));
  return t;
}

// PBESExists
inline
const atermpp::aterm_appl& constructPBESExists()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_PBESExists(), constructList(constructDataVarId()), constructPBExpr()));
  return t;
}

// PBESImp
inline
const atermpp::aterm_appl& constructPBESImp()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_PBESImp(), constructPBExpr(), constructPBExpr()));
  return t;
}

// Binder
inline
const atermpp::aterm_appl& constructBinder()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_Binder(), constructBindingOperator(), constructList(constructDataVarId()), constructDataExpr()));
  return t;
}

// SortsPossible
inline
const atermpp::aterm_appl& constructSortsPossible()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_SortsPossible(), constructList(constructSortExpr())));
  return t;
}

// SortRef
inline
const atermpp::aterm_appl& constructSortRef()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_SortRef(), constructString(), constructSortExpr()));
  return t;
}

// ProcEqnSpec
inline
const atermpp::aterm_appl& constructProcEqnSpec()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_ProcEqnSpec(), constructList()));
  return t;
}

// StateForall
inline
const atermpp::aterm_appl& constructStateForall()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_StateForall(), constructList(constructDataVarId()), constructStateFrm()));
  return t;
}

// BooleanImp
inline
const atermpp::aterm_appl& constructBooleanImp()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_BooleanImp(), constructBooleanExpression(), constructBooleanExpression()));
  return t;
}

// SortId
inline
const atermpp::aterm_appl& constructSortId()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_SortId(), constructString()));
  return t;
}

// StateNu
inline
const atermpp::aterm_appl& constructStateNu()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_StateNu(), constructString(), constructList(), constructStateFrm()));
  return t;
}

// RegNil
inline
const atermpp::aterm_appl& constructRegNil()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_RegNil()));
  return t;
}

// DataSpec
inline
const atermpp::aterm_appl& constructDataSpec()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_DataSpec(), constructSortSpec(), constructConsSpec(), constructMapSpec(), constructDataEqnSpec()));
  return t;
}

// Tau
inline
const atermpp::aterm_appl& constructTau()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_Tau()));
  return t;
}

// StateYaledTimed
inline
const atermpp::aterm_appl& constructStateYaledTimed()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_StateYaledTimed(), constructDataExpr()));
  return t;
}

// SortCons
inline
const atermpp::aterm_appl& constructSortCons()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_SortCons(), constructSortConsType(), constructSortExpr()));
  return t;
}

// DataEqnSpec
inline
const atermpp::aterm_appl& constructDataEqnSpec()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_DataEqnSpec(), constructList()));
  return t;
}

// LinearProcessSummand
inline
const atermpp::aterm_appl& constructLinearProcessSummand()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_LinearProcessSummand(), constructList(), constructDataExpr(), constructMultActOrDelta(), constructDataExprOrNil(), constructList()));
  return t;
}

// SortSpec
inline
const atermpp::aterm_appl& constructSortSpec()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_SortSpec(), constructList()));
  return t;
}

// ActionRenameRules
inline
const atermpp::aterm_appl& constructActionRenameRules()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_ActionRenameRules(), constructList()));
  return t;
}

// BooleanEquation
inline
const atermpp::aterm_appl& constructBooleanEquation()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_BooleanEquation(), constructFixPoint(), constructBooleanVariable(), constructBooleanExpression()));
  return t;
}

// ConsSpec
inline
const atermpp::aterm_appl& constructConsSpec()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_ConsSpec(), constructList()));
  return t;
}

// SortList
inline
const atermpp::aterm_appl& constructSortList()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_SortList()));
  return t;
}

// Sum
inline
const atermpp::aterm_appl& constructSum()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_Sum(), constructList(constructDataVarId()), constructProcExpr()));
  return t;
}

// DataVarId
inline
const atermpp::aterm_appl& constructDataVarId()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_DataVarId(), constructString(), constructSortExpr()));
  return t;
}

// ProcVarId
inline
const atermpp::aterm_appl& constructProcVarId()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_ProcVarId(), constructString(), constructList()));
  return t;
}

// ProcessInit
inline
const atermpp::aterm_appl& constructProcessInit()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_ProcessInit(), constructProcExpr()));
  return t;
}

// BES
inline
const atermpp::aterm_appl& constructBES()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_BES(), constructList(), constructBooleanExpression()));
  return t;
}

// MapSpec
inline
const atermpp::aterm_appl& constructMapSpec()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_MapSpec(), constructList()));
  return t;
}

// StateYaled
inline
const atermpp::aterm_appl& constructStateYaled()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_StateYaled()));
  return t;
}

// BooleanAnd
inline
const atermpp::aterm_appl& constructBooleanAnd()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_BooleanAnd(), constructBooleanExpression(), constructBooleanExpression()));
  return t;
}

// LinProcSpec
inline
const atermpp::aterm_appl& constructLinProcSpec()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_LinProcSpec(), constructDataSpec(), constructActSpec(), constructGlobVarSpec(), constructLinearProcess(), constructLinearProcessInit()));
  return t;
}

// SetBagComp
inline
const atermpp::aterm_appl& constructSetBagComp()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_SetBagComp()));
  return t;
}

// Choice
inline
const atermpp::aterm_appl& constructChoice()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_Choice(), constructProcExpr(), constructProcExpr()));
  return t;
}

// LinearProcessInit
inline
const atermpp::aterm_appl& constructLinearProcessInit()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_LinearProcessInit(), constructList()));
  return t;
}

// MultAct
inline
const atermpp::aterm_appl& constructMultAct()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_MultAct(), constructList()));
  return t;
}

// PropVarInst
inline
const atermpp::aterm_appl& constructPropVarInst()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_PropVarInst(), constructString(), constructList()));
  return t;
}

// BagComp
inline
const atermpp::aterm_appl& constructBagComp()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_BagComp()));
  return t;
}

// StateDelay
inline
const atermpp::aterm_appl& constructStateDelay()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_StateDelay()));
  return t;
}

// IdAssignment
inline
const atermpp::aterm_appl& constructIdAssignment()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_IdAssignment(), constructString(), constructList()));
  return t;
}

// RegAlt
inline
const atermpp::aterm_appl& constructRegAlt()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_RegAlt(), constructRegFrm(), constructRegFrm()));
  return t;
}

// StructCons
inline
const atermpp::aterm_appl& constructStructCons()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_StructCons(), constructString(), constructList(), constructStringOrEmpty()));
  return t;
}

// IdInit
inline
const atermpp::aterm_appl& constructIdInit()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_IdInit(), constructString(), constructDataExpr()));
  return t;
}

// Mu
inline
const atermpp::aterm_appl& constructMu()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_Mu()));
  return t;
}

// PBEqnSpec
inline
const atermpp::aterm_appl& constructPBEqnSpec()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_PBEqnSpec(), constructList()));
  return t;
}

// ActNot
inline
const atermpp::aterm_appl& constructActNot()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_ActNot(), constructActFrm()));
  return t;
}

// BooleanTrue
inline
const atermpp::aterm_appl& constructBooleanTrue()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_BooleanTrue()));
  return t;
}

// Block
inline
const atermpp::aterm_appl& constructBlock()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_Block(), constructList(), constructProcExpr()));
  return t;
}

// Rename
inline
const atermpp::aterm_appl& constructRename()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_Rename(), constructList(), constructProcExpr()));
  return t;
}

// Exists
inline
const atermpp::aterm_appl& constructExists()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_Exists()));
  return t;
}

// Sync
inline
const atermpp::aterm_appl& constructSync()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_Sync(), constructProcExpr(), constructProcExpr()));
  return t;
}

// ActExists
inline
const atermpp::aterm_appl& constructActExists()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_ActExists(), constructList(constructDataVarId()), constructActFrm()));
  return t;
}

// ProcSpec
inline
const atermpp::aterm_appl& constructProcSpec()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_ProcSpec(), constructDataSpec(), constructActSpec(), constructGlobVarSpec(), constructProcEqnSpec(), constructProcInit()));
  return t;
}

// StateMu
inline
const atermpp::aterm_appl& constructStateMu()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_StateMu(), constructString(), constructList(), constructStateFrm()));
  return t;
}

// StateFalse
inline
const atermpp::aterm_appl& constructStateFalse()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_StateFalse()));
  return t;
}

// PBESForall
inline
const atermpp::aterm_appl& constructPBESForall()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_PBESForall(), constructList(constructDataVarId()), constructPBExpr()));
  return t;
}

// StateTrue
inline
const atermpp::aterm_appl& constructStateTrue()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_StateTrue()));
  return t;
}

// BInit
inline
const atermpp::aterm_appl& constructBInit()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_BInit(), constructProcExpr(), constructProcExpr()));
  return t;
}

// PBESFalse
inline
const atermpp::aterm_appl& constructPBESFalse()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_PBESFalse()));
  return t;
}

// DataAppl
inline
const atermpp::aterm_appl& constructDataAppl()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_DataAppl(), constructDataExpr(), constructList(constructDataExpr())));
  return t;
}

// RegTrans
inline
const atermpp::aterm_appl& constructRegTrans()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_RegTrans(), constructRegFrm()));
  return t;
}

// StateDelayTimed
inline
const atermpp::aterm_appl& constructStateDelayTimed()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_StateDelayTimed(), constructDataExpr()));
  return t;
}

// Nu
inline
const atermpp::aterm_appl& constructNu()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_Nu()));
  return t;
}

// SortStruct
inline
const atermpp::aterm_appl& constructSortStruct()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_SortStruct(), constructList(constructStructCons())));
  return t;
}

// AtTime
inline
const atermpp::aterm_appl& constructAtTime()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_AtTime(), constructProcExpr(), constructDataExpr()));
  return t;
}

// ActOr
inline
const atermpp::aterm_appl& constructActOr()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_ActOr(), constructActFrm(), constructActFrm()));
  return t;
}

// Comm
inline
const atermpp::aterm_appl& constructComm()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_Comm(), constructList(), constructProcExpr()));
  return t;
}

// BooleanNot
inline
const atermpp::aterm_appl& constructBooleanNot()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_BooleanNot(), constructBooleanExpression()));
  return t;
}

// Delta
inline
const atermpp::aterm_appl& constructDelta()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_Delta()));
  return t;
}

// StateAnd
inline
const atermpp::aterm_appl& constructStateAnd()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_StateAnd(), constructStateFrm(), constructStateFrm()));
  return t;
}

// LMerge
inline
const atermpp::aterm_appl& constructLMerge()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_LMerge(), constructProcExpr(), constructProcExpr()));
  return t;
}

// SetComp
inline
const atermpp::aterm_appl& constructSetComp()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_SetComp()));
  return t;
}

// ActForall
inline
const atermpp::aterm_appl& constructActForall()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_ActForall(), constructList(constructDataVarId()), constructActFrm()));
  return t;
}

// RenameExpr
inline
const atermpp::aterm_appl& constructRenameExpr()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_RenameExpr(), constructString(), constructString()));
  return t;
}

// Merge
inline
const atermpp::aterm_appl& constructMerge()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_Merge(), constructProcExpr(), constructProcExpr()));
  return t;
}

// IfThen
inline
const atermpp::aterm_appl& constructIfThen()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_IfThen(), constructDataExpr(), constructProcExpr()));
  return t;
}

// BooleanVariable
inline
const atermpp::aterm_appl& constructBooleanVariable()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_BooleanVariable(), constructString()));
  return t;
}

// Action
inline
const atermpp::aterm_appl& constructAction()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_Action(), constructActId(), constructList()));
  return t;
}

// PBESAnd
inline
const atermpp::aterm_appl& constructPBESAnd()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_PBESAnd(), constructPBExpr(), constructPBExpr()));
  return t;
}

// Lambda
inline
const atermpp::aterm_appl& constructLambda()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_Lambda()));
  return t;
}

// StateMust
inline
const atermpp::aterm_appl& constructStateMust()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_StateMust(), constructRegFrm(), constructStateFrm()));
  return t;
}

// Seq
inline
const atermpp::aterm_appl& constructSeq()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_Seq(), constructProcExpr(), constructProcExpr()));
  return t;
}

// DataVarIdInit
inline
const atermpp::aterm_appl& constructDataVarIdInit()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_DataVarIdInit(), constructDataVarId(), constructDataExpr()));
  return t;
}

// Process
inline
const atermpp::aterm_appl& constructProcess()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_Process(), constructProcVarId(), constructList()));
  return t;
}

// ActAnd
inline
const atermpp::aterm_appl& constructActAnd()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_ActAnd(), constructActFrm(), constructActFrm()));
  return t;
}

// ActionRenameSpec
inline
const atermpp::aterm_appl& constructActionRenameSpec()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_ActionRenameSpec(), constructDataSpec(), constructActSpec(), constructActionRenameRules()));
  return t;
}

// PBES
inline
const atermpp::aterm_appl& constructPBES()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_PBES(), constructDataSpec(), constructGlobVarSpec(), constructPBEqnSpec(), constructPBInit()));
  return t;
}

// StateVar
inline
const atermpp::aterm_appl& constructStateVar()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_StateVar(), constructString(), constructList()));
  return t;
}

// ActionRenameRule
inline
const atermpp::aterm_appl& constructActionRenameRule()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_ActionRenameRule(), constructList(), constructDataExpr(), constructParamIdOrAction(), constructActionRenameRuleRHS()));
  return t;
}

// LinearProcess
inline
const atermpp::aterm_appl& constructLinearProcess()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_LinearProcess(), constructList(), constructList()));
  return t;
}

// ActAt
inline
const atermpp::aterm_appl& constructActAt()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_ActAt(), constructActFrm(), constructDataExpr()));
  return t;
}

// DataEqn
inline
const atermpp::aterm_appl& constructDataEqn()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_DataEqn(), constructList(), constructDataExpr(), constructDataExpr(), constructDataExpr()));
  return t;
}

// PBESNot
inline
const atermpp::aterm_appl& constructPBESNot()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_PBESNot(), constructPBExpr()));
  return t;
}

// StateExists
inline
const atermpp::aterm_appl& constructStateExists()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_StateExists(), constructList(constructDataVarId()), constructStateFrm()));
  return t;
}

// StateMay
inline
const atermpp::aterm_appl& constructStateMay()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_StateMay(), constructRegFrm(), constructStateFrm()));
  return t;
}

// ParamId
inline
const atermpp::aterm_appl& constructParamId()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_ParamId(), constructString(), constructList()));
  return t;
}

// PBESTrue
inline
const atermpp::aterm_appl& constructPBESTrue()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_PBESTrue()));
  return t;
}

// MultActName
inline
const atermpp::aterm_appl& constructMultActName()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_MultActName(), constructList(constructString())));
  return t;
}

// IfThenElse
inline
const atermpp::aterm_appl& constructIfThenElse()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_IfThenElse(), constructDataExpr(), constructProcExpr(), constructProcExpr()));
  return t;
}

// Nil
inline
const atermpp::aterm_appl& constructNil()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_Nil()));
  return t;
}

// ProcEqn
inline
const atermpp::aterm_appl& constructProcEqn()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_ProcEqn(), constructProcVarId(), constructList(), constructProcExpr()));
  return t;
}

// StructProj
inline
const atermpp::aterm_appl& constructStructProj()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_StructProj(), constructStringOrEmpty(), constructSortExpr()));
  return t;
}

// PBEqn
inline
const atermpp::aterm_appl& constructPBEqn()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_PBEqn(), constructFixPoint(), constructPropVarDecl(), constructPBExpr()));
  return t;
}

// Whr
inline
const atermpp::aterm_appl& constructWhr()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_Whr(), constructDataExpr(), constructList(constructWhrDecl())));
  return t;
}

// OpId
inline
const atermpp::aterm_appl& constructOpId()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_OpId(), constructString(), constructSortExpr()));
  return t;
}

// SortSet
inline
const atermpp::aterm_appl& constructSortSet()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_SortSet()));
  return t;
}

// ActFalse
inline
const atermpp::aterm_appl& constructActFalse()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_ActFalse()));
  return t;
}

// ActId
inline
const atermpp::aterm_appl& constructActId()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_ActId(), constructString(), constructList()));
  return t;
}

// SortUnknown
inline
const atermpp::aterm_appl& constructSortUnknown()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_SortUnknown()));
  return t;
}

// PBESOr
inline
const atermpp::aterm_appl& constructPBESOr()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_PBESOr(), constructPBExpr(), constructPBExpr()));
  return t;
}

// RegSeq
inline
const atermpp::aterm_appl& constructRegSeq()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_RegSeq(), constructRegFrm(), constructRegFrm()));
  return t;
}

// SortFBag
inline
const atermpp::aterm_appl& constructSortFBag()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_SortFBag()));
  return t;
}

// Allow
inline
const atermpp::aterm_appl& constructAllow()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_Allow(), constructList(), constructProcExpr()));
  return t;
}

// PropVarDecl
inline
const atermpp::aterm_appl& constructPropVarDecl()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_PropVarDecl(), constructString(), constructList()));
  return t;
}

// ActImp
inline
const atermpp::aterm_appl& constructActImp()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_ActImp(), constructActFrm(), constructActFrm()));
  return t;
}

// SortBag
inline
const atermpp::aterm_appl& constructSortBag()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_SortBag()));
  return t;
}

// PBInit
inline
const atermpp::aterm_appl& constructPBInit()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_PBInit(), constructPropVarInst()));
  return t;
}

// ActTrue
inline
const atermpp::aterm_appl& constructActTrue()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_ActTrue()));
  return t;
}

// RegTransOrNil
inline
const atermpp::aterm_appl& constructRegTransOrNil()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_RegTransOrNil(), constructRegFrm()));
  return t;
}

// GlobVarSpec
inline
const atermpp::aterm_appl& constructGlobVarSpec()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_GlobVarSpec(), constructList()));
  return t;
}

// ActSpec
inline
const atermpp::aterm_appl& constructActSpec()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_ActSpec(), constructList()));
  return t;
}

// Id
inline
const atermpp::aterm_appl& constructId()
{
  static atermpp::aterm_appl t = atermpp::aterm_appl(atermpp::term_appl<aterm>(function_symbol_Id(), constructString()));
  return t;
}

// SortExpr
inline
const atermpp::aterm_appl& constructSortExpr()
{
  return constructSortId();
}

// SortConsType
inline
const atermpp::aterm_appl& constructSortConsType()
{
  return constructSortList();
}

// DataExpr
inline
const atermpp::aterm_appl& constructDataExpr()
{
  return constructId();
}

// BindingOperator
inline
const atermpp::aterm_appl& constructBindingOperator()
{
  return constructSetBagComp();
}

// WhrDecl
inline
const atermpp::aterm_appl& constructWhrDecl()
{
  return constructIdInit();
}

// SortDecl
inline
const atermpp::aterm_appl& constructSortDecl()
{
  return constructSortId();
}

// DataExprOrNil
inline
const atermpp::aterm_appl& constructDataExprOrNil()
{
  return constructDataExpr();
}

// ParamIdOrAction
inline
const atermpp::aterm_appl& constructParamIdOrAction()
{
  return constructParamId();
}

// ProcExpr
inline
const atermpp::aterm_appl& constructProcExpr()
{
  return constructParamId();
}

// MultActOrDelta
inline
const atermpp::aterm_appl& constructMultActOrDelta()
{
  return constructMultAct();
}

// ProcInit
inline
const atermpp::aterm_appl& constructProcInit()
{
  return constructProcessInit();
}

// StateFrm
inline
const atermpp::aterm_appl& constructStateFrm()
{
  return constructDataExpr();
}

// RegFrm
inline
const atermpp::aterm_appl& constructRegFrm()
{
  return constructActFrm();
}

// ActFrm
inline
const atermpp::aterm_appl& constructActFrm()
{
  return constructMultAct();
}

// ActionRenameRuleRHS
inline
const atermpp::aterm_appl& constructActionRenameRuleRHS()
{
  return constructParamId();
}

// FixPoint
inline
const atermpp::aterm_appl& constructFixPoint()
{
  return constructMu();
}

// PBExpr
inline
const atermpp::aterm_appl& constructPBExpr()
{
  return constructDataExpr();
}

// BooleanExpression
inline
const atermpp::aterm_appl& constructBooleanExpression()
{
  return constructBooleanTrue();
}

//--- end generated code ---//


} // namespace detail

} // namespace core

} // mcrl2

#endif // LPS_DETAIL_CONSTRUCTORS
