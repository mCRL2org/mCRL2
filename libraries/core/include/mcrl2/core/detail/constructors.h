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

namespace mcrl2 {

namespace core {

namespace detail {

// String
inline
ATermAppl initConstructString(ATermAppl& t)
{
  t = gsString2ATermAppl("@NoValue");
  ATprotect(reinterpret_cast<ATerm*>(&t));
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

//--- begin generated code
ATermAppl constructBooleanOr();
ATermAppl constructProcEqn();
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
ATermAppl constructStateOr();
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
ATermAppl initConstructBooleanOr(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunBooleanOr(), reinterpret_cast<ATerm>(constructBooleanExpression()), reinterpret_cast<ATerm>(constructBooleanExpression()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructBooleanOr()
{
  static ATermAppl t = initConstructBooleanOr(t);
  return t;
}

// ProcEqn
inline
ATermAppl initConstructProcEqn(ATermAppl& t)
{
  t = ATmakeAppl4(gsAFunProcEqn(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructProcVarId()), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructProcExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructProcEqn()
{
  static ATermAppl t = initConstructProcEqn(t);
  return t;
}

// Hide
inline
ATermAppl initConstructHide(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunHide(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructProcExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructHide()
{
  static ATermAppl t = initConstructHide(t);
  return t;
}

// SortArrow
inline
ATermAppl initConstructSortArrow(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunSortArrow(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructSortExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructSortArrow()
{
  static ATermAppl t = initConstructSortArrow(t);
  return t;
}

// ProcessAssignment
inline
ATermAppl initConstructProcessAssignment(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunProcessAssignment(), reinterpret_cast<ATerm>(constructProcVarId()), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructProcessAssignment()
{
  static ATermAppl t = initConstructProcessAssignment(t);
  return t;
}

// Forall
inline
ATermAppl initConstructForall(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFunForall());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructForall()
{
  static ATermAppl t = initConstructForall(t);
  return t;
}

// CommExpr
inline
ATermAppl initConstructCommExpr(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunCommExpr(), reinterpret_cast<ATerm>(constructMultActName()), reinterpret_cast<ATerm>(constructStringOrNil()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructCommExpr()
{
  static ATermAppl t = initConstructCommExpr(t);
  return t;
}

// StateNot
inline
ATermAppl initConstructStateNot(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunStateNot(), reinterpret_cast<ATerm>(constructStateFrm()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructStateNot()
{
  static ATermAppl t = initConstructStateNot(t);
  return t;
}

// BooleanFalse
inline
ATermAppl initConstructBooleanFalse(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFunBooleanFalse());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructBooleanFalse()
{
  static ATermAppl t = initConstructBooleanFalse(t);
  return t;
}

// SortFSet
inline
ATermAppl initConstructSortFSet(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFunSortFSet());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructSortFSet()
{
  static ATermAppl t = initConstructSortFSet(t);
  return t;
}

// StateImp
inline
ATermAppl initConstructStateImp(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunStateImp(), reinterpret_cast<ATerm>(constructStateFrm()), reinterpret_cast<ATerm>(constructStateFrm()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructStateImp()
{
  static ATermAppl t = initConstructStateImp(t);
  return t;
}

// PBESExists
inline
ATermAppl initConstructPBESExists(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunPBESExists(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructPBExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructPBESExists()
{
  static ATermAppl t = initConstructPBESExists(t);
  return t;
}

// PBESImp
inline
ATermAppl initConstructPBESImp(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunPBESImp(), reinterpret_cast<ATerm>(constructPBExpr()), reinterpret_cast<ATerm>(constructPBExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructPBESImp()
{
  static ATermAppl t = initConstructPBESImp(t);
  return t;
}

// Binder
inline
ATermAppl initConstructBinder(ATermAppl& t)
{
  t = ATmakeAppl3(gsAFunBinder(), reinterpret_cast<ATerm>(constructBindingOperator()), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructDataExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructBinder()
{
  static ATermAppl t = initConstructBinder(t);
  return t;
}

// SortsPossible
inline
ATermAppl initConstructSortsPossible(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunSortsPossible(), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructSortsPossible()
{
  static ATermAppl t = initConstructSortsPossible(t);
  return t;
}

// SortRef
inline
ATermAppl initConstructSortRef(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunSortRef(), reinterpret_cast<ATerm>(constructString()), reinterpret_cast<ATerm>(constructSortExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructSortRef()
{
  static ATermAppl t = initConstructSortRef(t);
  return t;
}

// ProcEqnSpec
inline
ATermAppl initConstructProcEqnSpec(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunProcEqnSpec(), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructProcEqnSpec()
{
  static ATermAppl t = initConstructProcEqnSpec(t);
  return t;
}

// StateForall
inline
ATermAppl initConstructStateForall(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunStateForall(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructStateFrm()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructStateForall()
{
  static ATermAppl t = initConstructStateForall(t);
  return t;
}

// BooleanImp
inline
ATermAppl initConstructBooleanImp(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunBooleanImp(), reinterpret_cast<ATerm>(constructBooleanExpression()), reinterpret_cast<ATerm>(constructBooleanExpression()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructBooleanImp()
{
  static ATermAppl t = initConstructBooleanImp(t);
  return t;
}

// SortId
inline
ATermAppl initConstructSortId(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunSortId(), reinterpret_cast<ATerm>(constructString()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructSortId()
{
  static ATermAppl t = initConstructSortId(t);
  return t;
}

// StateNu
inline
ATermAppl initConstructStateNu(ATermAppl& t)
{
  t = ATmakeAppl3(gsAFunStateNu(), reinterpret_cast<ATerm>(constructString()), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructStateFrm()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructStateNu()
{
  static ATermAppl t = initConstructStateNu(t);
  return t;
}

// RegNil
inline
ATermAppl initConstructRegNil(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFunRegNil());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructRegNil()
{
  static ATermAppl t = initConstructRegNil(t);
  return t;
}

// DataSpec
inline
ATermAppl initConstructDataSpec(ATermAppl& t)
{
  t = ATmakeAppl4(gsAFunDataSpec(), reinterpret_cast<ATerm>(constructSortSpec()), reinterpret_cast<ATerm>(constructConsSpec()), reinterpret_cast<ATerm>(constructMapSpec()), reinterpret_cast<ATerm>(constructDataEqnSpec()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructDataSpec()
{
  static ATermAppl t = initConstructDataSpec(t);
  return t;
}

// Tau
inline
ATermAppl initConstructTau(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFunTau());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructTau()
{
  static ATermAppl t = initConstructTau(t);
  return t;
}

// StateYaledTimed
inline
ATermAppl initConstructStateYaledTimed(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunStateYaledTimed(), reinterpret_cast<ATerm>(constructDataExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructStateYaledTimed()
{
  static ATermAppl t = initConstructStateYaledTimed(t);
  return t;
}

// SortCons
inline
ATermAppl initConstructSortCons(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunSortCons(), reinterpret_cast<ATerm>(constructSortConsType()), reinterpret_cast<ATerm>(constructSortExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructSortCons()
{
  static ATermAppl t = initConstructSortCons(t);
  return t;
}

// DataEqnSpec
inline
ATermAppl initConstructDataEqnSpec(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunDataEqnSpec(), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructDataEqnSpec()
{
  static ATermAppl t = initConstructDataEqnSpec(t);
  return t;
}

// LinearProcessSummand
inline
ATermAppl initConstructLinearProcessSummand(ATermAppl& t)
{
  t = ATmakeAppl5(gsAFunLinearProcessSummand(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructDataExpr()), reinterpret_cast<ATerm>(constructMultActOrDelta()), reinterpret_cast<ATerm>(constructDataExprOrNil()), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructLinearProcessSummand()
{
  static ATermAppl t = initConstructLinearProcessSummand(t);
  return t;
}

// SortSpec
inline
ATermAppl initConstructSortSpec(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunSortSpec(), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructSortSpec()
{
  static ATermAppl t = initConstructSortSpec(t);
  return t;
}

// ActionRenameRules
inline
ATermAppl initConstructActionRenameRules(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunActionRenameRules(), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructActionRenameRules()
{
  static ATermAppl t = initConstructActionRenameRules(t);
  return t;
}

// BooleanEquation
inline
ATermAppl initConstructBooleanEquation(ATermAppl& t)
{
  t = ATmakeAppl3(gsAFunBooleanEquation(), reinterpret_cast<ATerm>(constructFixPoint()), reinterpret_cast<ATerm>(constructBooleanVariable()), reinterpret_cast<ATerm>(constructBooleanExpression()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructBooleanEquation()
{
  static ATermAppl t = initConstructBooleanEquation(t);
  return t;
}

// ConsSpec
inline
ATermAppl initConstructConsSpec(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunConsSpec(), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructConsSpec()
{
  static ATermAppl t = initConstructConsSpec(t);
  return t;
}

// SortList
inline
ATermAppl initConstructSortList(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFunSortList());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructSortList()
{
  static ATermAppl t = initConstructSortList(t);
  return t;
}

// Sum
inline
ATermAppl initConstructSum(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunSum(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructProcExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructSum()
{
  static ATermAppl t = initConstructSum(t);
  return t;
}

// DataVarId
inline
ATermAppl initConstructDataVarId(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunDataVarId(), reinterpret_cast<ATerm>(constructString()), reinterpret_cast<ATerm>(constructSortExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructDataVarId()
{
  static ATermAppl t = initConstructDataVarId(t);
  return t;
}

// ProcVarId
inline
ATermAppl initConstructProcVarId(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunProcVarId(), reinterpret_cast<ATerm>(constructString()), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructProcVarId()
{
  static ATermAppl t = initConstructProcVarId(t);
  return t;
}

// ProcessInit
inline
ATermAppl initConstructProcessInit(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunProcessInit(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructProcExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructProcessInit()
{
  static ATermAppl t = initConstructProcessInit(t);
  return t;
}

// BES
inline
ATermAppl initConstructBES(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunBES(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructBooleanExpression()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructBES()
{
  static ATermAppl t = initConstructBES(t);
  return t;
}

// MapSpec
inline
ATermAppl initConstructMapSpec(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunMapSpec(), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructMapSpec()
{
  static ATermAppl t = initConstructMapSpec(t);
  return t;
}

// StateYaled
inline
ATermAppl initConstructStateYaled(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFunStateYaled());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructStateYaled()
{
  static ATermAppl t = initConstructStateYaled(t);
  return t;
}

// BooleanAnd
inline
ATermAppl initConstructBooleanAnd(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunBooleanAnd(), reinterpret_cast<ATerm>(constructBooleanExpression()), reinterpret_cast<ATerm>(constructBooleanExpression()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructBooleanAnd()
{
  static ATermAppl t = initConstructBooleanAnd(t);
  return t;
}

// LinProcSpec
inline
ATermAppl initConstructLinProcSpec(ATermAppl& t)
{
  t = ATmakeAppl4(gsAFunLinProcSpec(), reinterpret_cast<ATerm>(constructDataSpec()), reinterpret_cast<ATerm>(constructActSpec()), reinterpret_cast<ATerm>(constructLinearProcess()), reinterpret_cast<ATerm>(constructLinearProcessInit()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructLinProcSpec()
{
  static ATermAppl t = initConstructLinProcSpec(t);
  return t;
}

// SetBagComp
inline
ATermAppl initConstructSetBagComp(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFunSetBagComp());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructSetBagComp()
{
  static ATermAppl t = initConstructSetBagComp(t);
  return t;
}

// Choice
inline
ATermAppl initConstructChoice(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunChoice(), reinterpret_cast<ATerm>(constructProcExpr()), reinterpret_cast<ATerm>(constructProcExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructChoice()
{
  static ATermAppl t = initConstructChoice(t);
  return t;
}

// LinearProcessInit
inline
ATermAppl initConstructLinearProcessInit(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunLinearProcessInit(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructLinearProcessInit()
{
  static ATermAppl t = initConstructLinearProcessInit(t);
  return t;
}

// MultAct
inline
ATermAppl initConstructMultAct(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunMultAct(), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructMultAct()
{
  static ATermAppl t = initConstructMultAct(t);
  return t;
}

// PropVarInst
inline
ATermAppl initConstructPropVarInst(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunPropVarInst(), reinterpret_cast<ATerm>(constructString()), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructPropVarInst()
{
  static ATermAppl t = initConstructPropVarInst(t);
  return t;
}

// BagComp
inline
ATermAppl initConstructBagComp(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFunBagComp());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructBagComp()
{
  static ATermAppl t = initConstructBagComp(t);
  return t;
}

// StateDelay
inline
ATermAppl initConstructStateDelay(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFunStateDelay());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructStateDelay()
{
  static ATermAppl t = initConstructStateDelay(t);
  return t;
}

// IdAssignment
inline
ATermAppl initConstructIdAssignment(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunIdAssignment(), reinterpret_cast<ATerm>(constructString()), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructIdAssignment()
{
  static ATermAppl t = initConstructIdAssignment(t);
  return t;
}

// RegAlt
inline
ATermAppl initConstructRegAlt(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunRegAlt(), reinterpret_cast<ATerm>(constructRegFrm()), reinterpret_cast<ATerm>(constructRegFrm()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructRegAlt()
{
  static ATermAppl t = initConstructRegAlt(t);
  return t;
}

// StructCons
inline
ATermAppl initConstructStructCons(ATermAppl& t)
{
  t = ATmakeAppl3(gsAFunStructCons(), reinterpret_cast<ATerm>(constructString()), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructStringOrNil()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructStructCons()
{
  static ATermAppl t = initConstructStructCons(t);
  return t;
}

// IdInit
inline
ATermAppl initConstructIdInit(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunIdInit(), reinterpret_cast<ATerm>(constructString()), reinterpret_cast<ATerm>(constructDataExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructIdInit()
{
  static ATermAppl t = initConstructIdInit(t);
  return t;
}

// Mu
inline
ATermAppl initConstructMu(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFunMu());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructMu()
{
  static ATermAppl t = initConstructMu(t);
  return t;
}

// PBEqnSpec
inline
ATermAppl initConstructPBEqnSpec(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunPBEqnSpec(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructPBEqnSpec()
{
  static ATermAppl t = initConstructPBEqnSpec(t);
  return t;
}

// ActNot
inline
ATermAppl initConstructActNot(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunActNot(), reinterpret_cast<ATerm>(constructActFrm()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructActNot()
{
  static ATermAppl t = initConstructActNot(t);
  return t;
}

// BooleanTrue
inline
ATermAppl initConstructBooleanTrue(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFunBooleanTrue());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructBooleanTrue()
{
  static ATermAppl t = initConstructBooleanTrue(t);
  return t;
}

// Block
inline
ATermAppl initConstructBlock(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunBlock(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructProcExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructBlock()
{
  static ATermAppl t = initConstructBlock(t);
  return t;
}

// Rename
inline
ATermAppl initConstructRename(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunRename(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructProcExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructRename()
{
  static ATermAppl t = initConstructRename(t);
  return t;
}

// Exists
inline
ATermAppl initConstructExists(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFunExists());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructExists()
{
  static ATermAppl t = initConstructExists(t);
  return t;
}

// Sync
inline
ATermAppl initConstructSync(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunSync(), reinterpret_cast<ATerm>(constructProcExpr()), reinterpret_cast<ATerm>(constructProcExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructSync()
{
  static ATermAppl t = initConstructSync(t);
  return t;
}

// ActExists
inline
ATermAppl initConstructActExists(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunActExists(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructActFrm()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructActExists()
{
  static ATermAppl t = initConstructActExists(t);
  return t;
}

// ProcSpec
inline
ATermAppl initConstructProcSpec(ATermAppl& t)
{
  t = ATmakeAppl4(gsAFunProcSpec(), reinterpret_cast<ATerm>(constructDataSpec()), reinterpret_cast<ATerm>(constructActSpec()), reinterpret_cast<ATerm>(constructProcEqnSpec()), reinterpret_cast<ATerm>(constructProcInit()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructProcSpec()
{
  static ATermAppl t = initConstructProcSpec(t);
  return t;
}

// StateMu
inline
ATermAppl initConstructStateMu(ATermAppl& t)
{
  t = ATmakeAppl3(gsAFunStateMu(), reinterpret_cast<ATerm>(constructString()), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructStateFrm()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructStateMu()
{
  static ATermAppl t = initConstructStateMu(t);
  return t;
}

// StateFalse
inline
ATermAppl initConstructStateFalse(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFunStateFalse());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructStateFalse()
{
  static ATermAppl t = initConstructStateFalse(t);
  return t;
}

// PBESForall
inline
ATermAppl initConstructPBESForall(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunPBESForall(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructPBExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructPBESForall()
{
  static ATermAppl t = initConstructPBESForall(t);
  return t;
}

// StateTrue
inline
ATermAppl initConstructStateTrue(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFunStateTrue());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructStateTrue()
{
  static ATermAppl t = initConstructStateTrue(t);
  return t;
}

// BInit
inline
ATermAppl initConstructBInit(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunBInit(), reinterpret_cast<ATerm>(constructProcExpr()), reinterpret_cast<ATerm>(constructProcExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructBInit()
{
  static ATermAppl t = initConstructBInit(t);
  return t;
}

// PBESFalse
inline
ATermAppl initConstructPBESFalse(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFunPBESFalse());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructPBESFalse()
{
  static ATermAppl t = initConstructPBESFalse(t);
  return t;
}

// DataAppl
inline
ATermAppl initConstructDataAppl(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunDataAppl(), reinterpret_cast<ATerm>(constructDataExpr()), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructDataAppl()
{
  static ATermAppl t = initConstructDataAppl(t);
  return t;
}

// RegTrans
inline
ATermAppl initConstructRegTrans(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunRegTrans(), reinterpret_cast<ATerm>(constructRegFrm()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructRegTrans()
{
  static ATermAppl t = initConstructRegTrans(t);
  return t;
}

// StateDelayTimed
inline
ATermAppl initConstructStateDelayTimed(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunStateDelayTimed(), reinterpret_cast<ATerm>(constructDataExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructStateDelayTimed()
{
  static ATermAppl t = initConstructStateDelayTimed(t);
  return t;
}

// Nu
inline
ATermAppl initConstructNu(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFunNu());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructNu()
{
  static ATermAppl t = initConstructNu(t);
  return t;
}

// SortStruct
inline
ATermAppl initConstructSortStruct(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunSortStruct(), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructSortStruct()
{
  static ATermAppl t = initConstructSortStruct(t);
  return t;
}

// AtTime
inline
ATermAppl initConstructAtTime(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunAtTime(), reinterpret_cast<ATerm>(constructProcExpr()), reinterpret_cast<ATerm>(constructDataExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructAtTime()
{
  static ATermAppl t = initConstructAtTime(t);
  return t;
}

// ActOr
inline
ATermAppl initConstructActOr(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunActOr(), reinterpret_cast<ATerm>(constructActFrm()), reinterpret_cast<ATerm>(constructActFrm()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructActOr()
{
  static ATermAppl t = initConstructActOr(t);
  return t;
}

// Comm
inline
ATermAppl initConstructComm(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunComm(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructProcExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructComm()
{
  static ATermAppl t = initConstructComm(t);
  return t;
}

// BooleanNot
inline
ATermAppl initConstructBooleanNot(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunBooleanNot(), reinterpret_cast<ATerm>(constructBooleanExpression()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructBooleanNot()
{
  static ATermAppl t = initConstructBooleanNot(t);
  return t;
}

// Delta
inline
ATermAppl initConstructDelta(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFunDelta());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructDelta()
{
  static ATermAppl t = initConstructDelta(t);
  return t;
}

// StateAnd
inline
ATermAppl initConstructStateAnd(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunStateAnd(), reinterpret_cast<ATerm>(constructStateFrm()), reinterpret_cast<ATerm>(constructStateFrm()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructStateAnd()
{
  static ATermAppl t = initConstructStateAnd(t);
  return t;
}

// LMerge
inline
ATermAppl initConstructLMerge(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunLMerge(), reinterpret_cast<ATerm>(constructProcExpr()), reinterpret_cast<ATerm>(constructProcExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructLMerge()
{
  static ATermAppl t = initConstructLMerge(t);
  return t;
}

// SetComp
inline
ATermAppl initConstructSetComp(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFunSetComp());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructSetComp()
{
  static ATermAppl t = initConstructSetComp(t);
  return t;
}

// ActForall
inline
ATermAppl initConstructActForall(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunActForall(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructActFrm()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructActForall()
{
  static ATermAppl t = initConstructActForall(t);
  return t;
}

// RenameExpr
inline
ATermAppl initConstructRenameExpr(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunRenameExpr(), reinterpret_cast<ATerm>(constructString()), reinterpret_cast<ATerm>(constructString()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructRenameExpr()
{
  static ATermAppl t = initConstructRenameExpr(t);
  return t;
}

// Merge
inline
ATermAppl initConstructMerge(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunMerge(), reinterpret_cast<ATerm>(constructProcExpr()), reinterpret_cast<ATerm>(constructProcExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructMerge()
{
  static ATermAppl t = initConstructMerge(t);
  return t;
}

// IfThen
inline
ATermAppl initConstructIfThen(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunIfThen(), reinterpret_cast<ATerm>(constructDataExpr()), reinterpret_cast<ATerm>(constructProcExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructIfThen()
{
  static ATermAppl t = initConstructIfThen(t);
  return t;
}

// BooleanVariable
inline
ATermAppl initConstructBooleanVariable(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunBooleanVariable(), reinterpret_cast<ATerm>(constructString()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructBooleanVariable()
{
  static ATermAppl t = initConstructBooleanVariable(t);
  return t;
}

// Action
inline
ATermAppl initConstructAction(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunAction(), reinterpret_cast<ATerm>(constructActId()), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructAction()
{
  static ATermAppl t = initConstructAction(t);
  return t;
}

// PBESAnd
inline
ATermAppl initConstructPBESAnd(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunPBESAnd(), reinterpret_cast<ATerm>(constructPBExpr()), reinterpret_cast<ATerm>(constructPBExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructPBESAnd()
{
  static ATermAppl t = initConstructPBESAnd(t);
  return t;
}

// Lambda
inline
ATermAppl initConstructLambda(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFunLambda());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructLambda()
{
  static ATermAppl t = initConstructLambda(t);
  return t;
}

// StateMust
inline
ATermAppl initConstructStateMust(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunStateMust(), reinterpret_cast<ATerm>(constructRegFrm()), reinterpret_cast<ATerm>(constructStateFrm()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructStateMust()
{
  static ATermAppl t = initConstructStateMust(t);
  return t;
}

// Seq
inline
ATermAppl initConstructSeq(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunSeq(), reinterpret_cast<ATerm>(constructProcExpr()), reinterpret_cast<ATerm>(constructProcExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructSeq()
{
  static ATermAppl t = initConstructSeq(t);
  return t;
}

// DataVarIdInit
inline
ATermAppl initConstructDataVarIdInit(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunDataVarIdInit(), reinterpret_cast<ATerm>(constructDataVarId()), reinterpret_cast<ATerm>(constructDataExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructDataVarIdInit()
{
  static ATermAppl t = initConstructDataVarIdInit(t);
  return t;
}

// Process
inline
ATermAppl initConstructProcess(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunProcess(), reinterpret_cast<ATerm>(constructProcVarId()), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructProcess()
{
  static ATermAppl t = initConstructProcess(t);
  return t;
}

// ActAnd
inline
ATermAppl initConstructActAnd(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunActAnd(), reinterpret_cast<ATerm>(constructActFrm()), reinterpret_cast<ATerm>(constructActFrm()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructActAnd()
{
  static ATermAppl t = initConstructActAnd(t);
  return t;
}

// ActionRenameSpec
inline
ATermAppl initConstructActionRenameSpec(ATermAppl& t)
{
  t = ATmakeAppl3(gsAFunActionRenameSpec(), reinterpret_cast<ATerm>(constructDataSpec()), reinterpret_cast<ATerm>(constructActSpec()), reinterpret_cast<ATerm>(constructActionRenameRules()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructActionRenameSpec()
{
  static ATermAppl t = initConstructActionRenameSpec(t);
  return t;
}

// PBES
inline
ATermAppl initConstructPBES(ATermAppl& t)
{
  t = ATmakeAppl3(gsAFunPBES(), reinterpret_cast<ATerm>(constructDataSpec()), reinterpret_cast<ATerm>(constructPBEqnSpec()), reinterpret_cast<ATerm>(constructPBInit()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructPBES()
{
  static ATermAppl t = initConstructPBES(t);
  return t;
}

// StateVar
inline
ATermAppl initConstructStateVar(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunStateVar(), reinterpret_cast<ATerm>(constructString()), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructStateVar()
{
  static ATermAppl t = initConstructStateVar(t);
  return t;
}

// ActionRenameRule
inline
ATermAppl initConstructActionRenameRule(ATermAppl& t)
{
  t = ATmakeAppl4(gsAFunActionRenameRule(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructDataExprOrNil()), reinterpret_cast<ATerm>(constructParamIdOrAction()), reinterpret_cast<ATerm>(constructActionRenameRuleRHS()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructActionRenameRule()
{
  static ATermAppl t = initConstructActionRenameRule(t);
  return t;
}

// LinearProcess
inline
ATermAppl initConstructLinearProcess(ATermAppl& t)
{
  t = ATmakeAppl3(gsAFunLinearProcess(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructLinearProcess()
{
  static ATermAppl t = initConstructLinearProcess(t);
  return t;
}

// ActAt
inline
ATermAppl initConstructActAt(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunActAt(), reinterpret_cast<ATerm>(constructActFrm()), reinterpret_cast<ATerm>(constructDataExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructActAt()
{
  static ATermAppl t = initConstructActAt(t);
  return t;
}

// DataEqn
inline
ATermAppl initConstructDataEqn(ATermAppl& t)
{
  t = ATmakeAppl4(gsAFunDataEqn(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructDataExprOrNil()), reinterpret_cast<ATerm>(constructDataExpr()), reinterpret_cast<ATerm>(constructDataExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructDataEqn()
{
  static ATermAppl t = initConstructDataEqn(t);
  return t;
}

// PBESNot
inline
ATermAppl initConstructPBESNot(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunPBESNot(), reinterpret_cast<ATerm>(constructPBExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructPBESNot()
{
  static ATermAppl t = initConstructPBESNot(t);
  return t;
}

// StateExists
inline
ATermAppl initConstructStateExists(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunStateExists(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructStateFrm()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructStateExists()
{
  static ATermAppl t = initConstructStateExists(t);
  return t;
}

// StateMay
inline
ATermAppl initConstructStateMay(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunStateMay(), reinterpret_cast<ATerm>(constructRegFrm()), reinterpret_cast<ATerm>(constructStateFrm()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructStateMay()
{
  static ATermAppl t = initConstructStateMay(t);
  return t;
}

// ParamId
inline
ATermAppl initConstructParamId(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunParamId(), reinterpret_cast<ATerm>(constructString()), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructParamId()
{
  static ATermAppl t = initConstructParamId(t);
  return t;
}

// PBESTrue
inline
ATermAppl initConstructPBESTrue(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFunPBESTrue());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructPBESTrue()
{
  static ATermAppl t = initConstructPBESTrue(t);
  return t;
}

// MultActName
inline
ATermAppl initConstructMultActName(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunMultActName(), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructMultActName()
{
  static ATermAppl t = initConstructMultActName(t);
  return t;
}

// IfThenElse
inline
ATermAppl initConstructIfThenElse(ATermAppl& t)
{
  t = ATmakeAppl3(gsAFunIfThenElse(), reinterpret_cast<ATerm>(constructDataExpr()), reinterpret_cast<ATerm>(constructProcExpr()), reinterpret_cast<ATerm>(constructProcExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructIfThenElse()
{
  static ATermAppl t = initConstructIfThenElse(t);
  return t;
}

// Nil
inline
ATermAppl initConstructNil(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFunNil());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructNil()
{
  static ATermAppl t = initConstructNil(t);
  return t;
}

// StateOr
inline
ATermAppl initConstructStateOr(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunStateOr(), reinterpret_cast<ATerm>(constructStateFrm()), reinterpret_cast<ATerm>(constructStateFrm()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructStateOr()
{
  static ATermAppl t = initConstructStateOr(t);
  return t;
}

// StructProj
inline
ATermAppl initConstructStructProj(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunStructProj(), reinterpret_cast<ATerm>(constructStringOrNil()), reinterpret_cast<ATerm>(constructSortExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructStructProj()
{
  static ATermAppl t = initConstructStructProj(t);
  return t;
}

// PBEqn
inline
ATermAppl initConstructPBEqn(ATermAppl& t)
{
  t = ATmakeAppl3(gsAFunPBEqn(), reinterpret_cast<ATerm>(constructFixPoint()), reinterpret_cast<ATerm>(constructPropVarDecl()), reinterpret_cast<ATerm>(constructPBExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructPBEqn()
{
  static ATermAppl t = initConstructPBEqn(t);
  return t;
}

// Whr
inline
ATermAppl initConstructWhr(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunWhr(), reinterpret_cast<ATerm>(constructDataExpr()), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructWhr()
{
  static ATermAppl t = initConstructWhr(t);
  return t;
}

// OpId
inline
ATermAppl initConstructOpId(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunOpId(), reinterpret_cast<ATerm>(constructString()), reinterpret_cast<ATerm>(constructSortExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructOpId()
{
  static ATermAppl t = initConstructOpId(t);
  return t;
}

// SortSet
inline
ATermAppl initConstructSortSet(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFunSortSet());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructSortSet()
{
  static ATermAppl t = initConstructSortSet(t);
  return t;
}

// ActFalse
inline
ATermAppl initConstructActFalse(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFunActFalse());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructActFalse()
{
  static ATermAppl t = initConstructActFalse(t);
  return t;
}

// ActId
inline
ATermAppl initConstructActId(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunActId(), reinterpret_cast<ATerm>(constructString()), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructActId()
{
  static ATermAppl t = initConstructActId(t);
  return t;
}

// SortUnknown
inline
ATermAppl initConstructSortUnknown(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFunSortUnknown());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructSortUnknown()
{
  static ATermAppl t = initConstructSortUnknown(t);
  return t;
}

// PBESOr
inline
ATermAppl initConstructPBESOr(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunPBESOr(), reinterpret_cast<ATerm>(constructPBExpr()), reinterpret_cast<ATerm>(constructPBExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructPBESOr()
{
  static ATermAppl t = initConstructPBESOr(t);
  return t;
}

// RegSeq
inline
ATermAppl initConstructRegSeq(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunRegSeq(), reinterpret_cast<ATerm>(constructRegFrm()), reinterpret_cast<ATerm>(constructRegFrm()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructRegSeq()
{
  static ATermAppl t = initConstructRegSeq(t);
  return t;
}

// SortFBag
inline
ATermAppl initConstructSortFBag(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFunSortFBag());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructSortFBag()
{
  static ATermAppl t = initConstructSortFBag(t);
  return t;
}

// Allow
inline
ATermAppl initConstructAllow(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunAllow(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructProcExpr()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructAllow()
{
  static ATermAppl t = initConstructAllow(t);
  return t;
}

// PropVarDecl
inline
ATermAppl initConstructPropVarDecl(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunPropVarDecl(), reinterpret_cast<ATerm>(constructString()), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructPropVarDecl()
{
  static ATermAppl t = initConstructPropVarDecl(t);
  return t;
}

// ActImp
inline
ATermAppl initConstructActImp(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunActImp(), reinterpret_cast<ATerm>(constructActFrm()), reinterpret_cast<ATerm>(constructActFrm()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructActImp()
{
  static ATermAppl t = initConstructActImp(t);
  return t;
}

// SortBag
inline
ATermAppl initConstructSortBag(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFunSortBag());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructSortBag()
{
  static ATermAppl t = initConstructSortBag(t);
  return t;
}

// PBInit
inline
ATermAppl initConstructPBInit(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunPBInit(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructPropVarInst()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructPBInit()
{
  static ATermAppl t = initConstructPBInit(t);
  return t;
}

// ActTrue
inline
ATermAppl initConstructActTrue(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFunActTrue());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructActTrue()
{
  static ATermAppl t = initConstructActTrue(t);
  return t;
}

// RegTransOrNil
inline
ATermAppl initConstructRegTransOrNil(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunRegTransOrNil(), reinterpret_cast<ATerm>(constructRegFrm()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructRegTransOrNil()
{
  static ATermAppl t = initConstructRegTransOrNil(t);
  return t;
}

// ActSpec
inline
ATermAppl initConstructActSpec(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunActSpec(), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructActSpec()
{
  static ATermAppl t = initConstructActSpec(t);
  return t;
}

// Id
inline
ATermAppl initConstructId(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunId(), reinterpret_cast<ATerm>(constructString()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructId()
{
  static ATermAppl t = initConstructId(t);
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
//--- end generated code


} // namespace detail

} // namespace core

} // mcrl2

#endif // LPS_DETAIL_CONSTRUCTORS
