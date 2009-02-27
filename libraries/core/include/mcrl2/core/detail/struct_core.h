// Author(s): Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file libstruct_core.h

#ifndef MCRL2_LIBSTRUCT_CORE_H
#define MCRL2_LIBSTRUCT_CORE_H

//This file describes the functions that can be used for the internal ATerm
//structure.

#include <aterm2.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>


namespace mcrl2 {
  namespace core {
    namespace detail {

//Global precondition: the ATerm library has been initialised

//--- begin generated code
// ActAnd
inline
AFun initAFunActAnd(AFun& f)
{
  f = ATmakeAFun("ActAnd", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunActAnd()
{
  static AFun AFunActAnd = initAFunActAnd(AFunActAnd);
  return AFunActAnd;
}

inline
bool gsIsActAnd(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunActAnd();
}

// ActAt
inline
AFun initAFunActAt(AFun& f)
{
  f = ATmakeAFun("ActAt", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunActAt()
{
  static AFun AFunActAt = initAFunActAt(AFunActAt);
  return AFunActAt;
}

inline
bool gsIsActAt(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunActAt();
}

// ActExists
inline
AFun initAFunActExists(AFun& f)
{
  f = ATmakeAFun("ActExists", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunActExists()
{
  static AFun AFunActExists = initAFunActExists(AFunActExists);
  return AFunActExists;
}

inline
bool gsIsActExists(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunActExists();
}

// ActFalse
inline
AFun initAFunActFalse(AFun& f)
{
  f = ATmakeAFun("ActFalse", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunActFalse()
{
  static AFun AFunActFalse = initAFunActFalse(AFunActFalse);
  return AFunActFalse;
}

inline
bool gsIsActFalse(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunActFalse();
}

// ActForall
inline
AFun initAFunActForall(AFun& f)
{
  f = ATmakeAFun("ActForall", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunActForall()
{
  static AFun AFunActForall = initAFunActForall(AFunActForall);
  return AFunActForall;
}

inline
bool gsIsActForall(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunActForall();
}

// ActId
inline
AFun initAFunActId(AFun& f)
{
  f = ATmakeAFun("ActId", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunActId()
{
  static AFun AFunActId = initAFunActId(AFunActId);
  return AFunActId;
}

inline
bool gsIsActId(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunActId();
}

// ActImp
inline
AFun initAFunActImp(AFun& f)
{
  f = ATmakeAFun("ActImp", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunActImp()
{
  static AFun AFunActImp = initAFunActImp(AFunActImp);
  return AFunActImp;
}

inline
bool gsIsActImp(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunActImp();
}

// ActNot
inline
AFun initAFunActNot(AFun& f)
{
  f = ATmakeAFun("ActNot", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunActNot()
{
  static AFun AFunActNot = initAFunActNot(AFunActNot);
  return AFunActNot;
}

inline
bool gsIsActNot(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunActNot();
}

// ActOr
inline
AFun initAFunActOr(AFun& f)
{
  f = ATmakeAFun("ActOr", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunActOr()
{
  static AFun AFunActOr = initAFunActOr(AFunActOr);
  return AFunActOr;
}

inline
bool gsIsActOr(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunActOr();
}

// ActSpec
inline
AFun initAFunActSpec(AFun& f)
{
  f = ATmakeAFun("ActSpec", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunActSpec()
{
  static AFun AFunActSpec = initAFunActSpec(AFunActSpec);
  return AFunActSpec;
}

inline
bool gsIsActSpec(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunActSpec();
}

// ActTrue
inline
AFun initAFunActTrue(AFun& f)
{
  f = ATmakeAFun("ActTrue", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunActTrue()
{
  static AFun AFunActTrue = initAFunActTrue(AFunActTrue);
  return AFunActTrue;
}

inline
bool gsIsActTrue(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunActTrue();
}

// Action
inline
AFun initAFunAction(AFun& f)
{
  f = ATmakeAFun("Action", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunAction()
{
  static AFun AFunAction = initAFunAction(AFunAction);
  return AFunAction;
}

inline
bool gsIsAction(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunAction();
}

// ActionRenameRule
inline
AFun initAFunActionRenameRule(AFun& f)
{
  f = ATmakeAFun("ActionRenameRule", 4, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunActionRenameRule()
{
  static AFun AFunActionRenameRule = initAFunActionRenameRule(AFunActionRenameRule);
  return AFunActionRenameRule;
}

inline
bool gsIsActionRenameRule(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunActionRenameRule();
}

// ActionRenameRules
inline
AFun initAFunActionRenameRules(AFun& f)
{
  f = ATmakeAFun("ActionRenameRules", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunActionRenameRules()
{
  static AFun AFunActionRenameRules = initAFunActionRenameRules(AFunActionRenameRules);
  return AFunActionRenameRules;
}

inline
bool gsIsActionRenameRules(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunActionRenameRules();
}

// ActionRenameSpec
inline
AFun initAFunActionRenameSpec(AFun& f)
{
  f = ATmakeAFun("ActionRenameSpec", 3, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunActionRenameSpec()
{
  static AFun AFunActionRenameSpec = initAFunActionRenameSpec(AFunActionRenameSpec);
  return AFunActionRenameSpec;
}

inline
bool gsIsActionRenameSpec(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunActionRenameSpec();
}

// Allow
inline
AFun initAFunAllow(AFun& f)
{
  f = ATmakeAFun("Allow", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunAllow()
{
  static AFun AFunAllow = initAFunAllow(AFunAllow);
  return AFunAllow;
}

inline
bool gsIsAllow(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunAllow();
}

// AtTime
inline
AFun initAFunAtTime(AFun& f)
{
  f = ATmakeAFun("AtTime", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunAtTime()
{
  static AFun AFunAtTime = initAFunAtTime(AFunAtTime);
  return AFunAtTime;
}

inline
bool gsIsAtTime(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunAtTime();
}

// BES
inline
AFun initAFunBES(AFun& f)
{
  f = ATmakeAFun("BES", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunBES()
{
  static AFun AFunBES = initAFunBES(AFunBES);
  return AFunBES;
}

inline
bool gsIsBES(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunBES();
}

// BInit
inline
AFun initAFunBInit(AFun& f)
{
  f = ATmakeAFun("BInit", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunBInit()
{
  static AFun AFunBInit = initAFunBInit(AFunBInit);
  return AFunBInit;
}

inline
bool gsIsBInit(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunBInit();
}

// BagComp
inline
AFun initAFunBagComp(AFun& f)
{
  f = ATmakeAFun("BagComp", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunBagComp()
{
  static AFun AFunBagComp = initAFunBagComp(AFunBagComp);
  return AFunBagComp;
}

inline
bool gsIsBagComp(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunBagComp();
}

// Binder
inline
AFun initAFunBinder(AFun& f)
{
  f = ATmakeAFun("Binder", 3, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunBinder()
{
  static AFun AFunBinder = initAFunBinder(AFunBinder);
  return AFunBinder;
}

inline
bool gsIsBinder(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunBinder();
}

// Block
inline
AFun initAFunBlock(AFun& f)
{
  f = ATmakeAFun("Block", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunBlock()
{
  static AFun AFunBlock = initAFunBlock(AFunBlock);
  return AFunBlock;
}

inline
bool gsIsBlock(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunBlock();
}

// BooleanAnd
inline
AFun initAFunBooleanAnd(AFun& f)
{
  f = ATmakeAFun("BooleanAnd", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunBooleanAnd()
{
  static AFun AFunBooleanAnd = initAFunBooleanAnd(AFunBooleanAnd);
  return AFunBooleanAnd;
}

inline
bool gsIsBooleanAnd(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunBooleanAnd();
}

// BooleanEquation
inline
AFun initAFunBooleanEquation(AFun& f)
{
  f = ATmakeAFun("BooleanEquation", 3, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunBooleanEquation()
{
  static AFun AFunBooleanEquation = initAFunBooleanEquation(AFunBooleanEquation);
  return AFunBooleanEquation;
}

inline
bool gsIsBooleanEquation(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunBooleanEquation();
}

// BooleanFalse
inline
AFun initAFunBooleanFalse(AFun& f)
{
  f = ATmakeAFun("BooleanFalse", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunBooleanFalse()
{
  static AFun AFunBooleanFalse = initAFunBooleanFalse(AFunBooleanFalse);
  return AFunBooleanFalse;
}

inline
bool gsIsBooleanFalse(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunBooleanFalse();
}

// BooleanImp
inline
AFun initAFunBooleanImp(AFun& f)
{
  f = ATmakeAFun("BooleanImp", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunBooleanImp()
{
  static AFun AFunBooleanImp = initAFunBooleanImp(AFunBooleanImp);
  return AFunBooleanImp;
}

inline
bool gsIsBooleanImp(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunBooleanImp();
}

// BooleanNot
inline
AFun initAFunBooleanNot(AFun& f)
{
  f = ATmakeAFun("BooleanNot", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunBooleanNot()
{
  static AFun AFunBooleanNot = initAFunBooleanNot(AFunBooleanNot);
  return AFunBooleanNot;
}

inline
bool gsIsBooleanNot(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunBooleanNot();
}

// BooleanOr
inline
AFun initAFunBooleanOr(AFun& f)
{
  f = ATmakeAFun("BooleanOr", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunBooleanOr()
{
  static AFun AFunBooleanOr = initAFunBooleanOr(AFunBooleanOr);
  return AFunBooleanOr;
}

inline
bool gsIsBooleanOr(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunBooleanOr();
}

// BooleanTrue
inline
AFun initAFunBooleanTrue(AFun& f)
{
  f = ATmakeAFun("BooleanTrue", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunBooleanTrue()
{
  static AFun AFunBooleanTrue = initAFunBooleanTrue(AFunBooleanTrue);
  return AFunBooleanTrue;
}

inline
bool gsIsBooleanTrue(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunBooleanTrue();
}

// BooleanVariable
inline
AFun initAFunBooleanVariable(AFun& f)
{
  f = ATmakeAFun("BooleanVariable", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunBooleanVariable()
{
  static AFun AFunBooleanVariable = initAFunBooleanVariable(AFunBooleanVariable);
  return AFunBooleanVariable;
}

inline
bool gsIsBooleanVariable(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunBooleanVariable();
}

// Choice
inline
AFun initAFunChoice(AFun& f)
{
  f = ATmakeAFun("Choice", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunChoice()
{
  static AFun AFunChoice = initAFunChoice(AFunChoice);
  return AFunChoice;
}

inline
bool gsIsChoice(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunChoice();
}

// Comm
inline
AFun initAFunComm(AFun& f)
{
  f = ATmakeAFun("Comm", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunComm()
{
  static AFun AFunComm = initAFunComm(AFunComm);
  return AFunComm;
}

inline
bool gsIsComm(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunComm();
}

// CommExpr
inline
AFun initAFunCommExpr(AFun& f)
{
  f = ATmakeAFun("CommExpr", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunCommExpr()
{
  static AFun AFunCommExpr = initAFunCommExpr(AFunCommExpr);
  return AFunCommExpr;
}

inline
bool gsIsCommExpr(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunCommExpr();
}

// ConsSpec
inline
AFun initAFunConsSpec(AFun& f)
{
  f = ATmakeAFun("ConsSpec", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunConsSpec()
{
  static AFun AFunConsSpec = initAFunConsSpec(AFunConsSpec);
  return AFunConsSpec;
}

inline
bool gsIsConsSpec(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunConsSpec();
}

// DataAppl
inline
AFun initAFunDataAppl(AFun& f)
{
  f = ATmakeAFun("DataAppl", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunDataAppl()
{
  static AFun AFunDataAppl = initAFunDataAppl(AFunDataAppl);
  return AFunDataAppl;
}

inline
bool gsIsDataAppl(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunDataAppl();
}

// DataEqn
inline
AFun initAFunDataEqn(AFun& f)
{
  f = ATmakeAFun("DataEqn", 4, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunDataEqn()
{
  static AFun AFunDataEqn = initAFunDataEqn(AFunDataEqn);
  return AFunDataEqn;
}

inline
bool gsIsDataEqn(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunDataEqn();
}

// DataEqnSpec
inline
AFun initAFunDataEqnSpec(AFun& f)
{
  f = ATmakeAFun("DataEqnSpec", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunDataEqnSpec()
{
  static AFun AFunDataEqnSpec = initAFunDataEqnSpec(AFunDataEqnSpec);
  return AFunDataEqnSpec;
}

inline
bool gsIsDataEqnSpec(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunDataEqnSpec();
}

// DataSpec
inline
AFun initAFunDataSpec(AFun& f)
{
  f = ATmakeAFun("DataSpec", 4, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunDataSpec()
{
  static AFun AFunDataSpec = initAFunDataSpec(AFunDataSpec);
  return AFunDataSpec;
}

inline
bool gsIsDataSpec(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunDataSpec();
}

// DataVarId
inline
AFun initAFunDataVarId(AFun& f)
{
  f = ATmakeAFun("DataVarId", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunDataVarId()
{
  static AFun AFunDataVarId = initAFunDataVarId(AFunDataVarId);
  return AFunDataVarId;
}

inline
bool gsIsDataVarId(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunDataVarId();
}

// DataVarIdInit
inline
AFun initAFunDataVarIdInit(AFun& f)
{
  f = ATmakeAFun("DataVarIdInit", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunDataVarIdInit()
{
  static AFun AFunDataVarIdInit = initAFunDataVarIdInit(AFunDataVarIdInit);
  return AFunDataVarIdInit;
}

inline
bool gsIsDataVarIdInit(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunDataVarIdInit();
}

// Delta
inline
AFun initAFunDelta(AFun& f)
{
  f = ATmakeAFun("Delta", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunDelta()
{
  static AFun AFunDelta = initAFunDelta(AFunDelta);
  return AFunDelta;
}

inline
bool gsIsDelta(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunDelta();
}

// Exists
inline
AFun initAFunExists(AFun& f)
{
  f = ATmakeAFun("Exists", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunExists()
{
  static AFun AFunExists = initAFunExists(AFunExists);
  return AFunExists;
}

inline
bool gsIsExists(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunExists();
}

// Forall
inline
AFun initAFunForall(AFun& f)
{
  f = ATmakeAFun("Forall", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunForall()
{
  static AFun AFunForall = initAFunForall(AFunForall);
  return AFunForall;
}

inline
bool gsIsForall(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunForall();
}

// Hide
inline
AFun initAFunHide(AFun& f)
{
  f = ATmakeAFun("Hide", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunHide()
{
  static AFun AFunHide = initAFunHide(AFunHide);
  return AFunHide;
}

inline
bool gsIsHide(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunHide();
}

// Id
inline
AFun initAFunId(AFun& f)
{
  f = ATmakeAFun("Id", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunId()
{
  static AFun AFunId = initAFunId(AFunId);
  return AFunId;
}

inline
bool gsIsId(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunId();
}

// IdAssignment
inline
AFun initAFunIdAssignment(AFun& f)
{
  f = ATmakeAFun("IdAssignment", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunIdAssignment()
{
  static AFun AFunIdAssignment = initAFunIdAssignment(AFunIdAssignment);
  return AFunIdAssignment;
}

inline
bool gsIsIdAssignment(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunIdAssignment();
}

// IdInit
inline
AFun initAFunIdInit(AFun& f)
{
  f = ATmakeAFun("IdInit", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunIdInit()
{
  static AFun AFunIdInit = initAFunIdInit(AFunIdInit);
  return AFunIdInit;
}

inline
bool gsIsIdInit(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunIdInit();
}

// IfThen
inline
AFun initAFunIfThen(AFun& f)
{
  f = ATmakeAFun("IfThen", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunIfThen()
{
  static AFun AFunIfThen = initAFunIfThen(AFunIfThen);
  return AFunIfThen;
}

inline
bool gsIsIfThen(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunIfThen();
}

// IfThenElse
inline
AFun initAFunIfThenElse(AFun& f)
{
  f = ATmakeAFun("IfThenElse", 3, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunIfThenElse()
{
  static AFun AFunIfThenElse = initAFunIfThenElse(AFunIfThenElse);
  return AFunIfThenElse;
}

inline
bool gsIsIfThenElse(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunIfThenElse();
}

// LMerge
inline
AFun initAFunLMerge(AFun& f)
{
  f = ATmakeAFun("LMerge", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunLMerge()
{
  static AFun AFunLMerge = initAFunLMerge(AFunLMerge);
  return AFunLMerge;
}

inline
bool gsIsLMerge(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunLMerge();
}

// Lambda
inline
AFun initAFunLambda(AFun& f)
{
  f = ATmakeAFun("Lambda", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunLambda()
{
  static AFun AFunLambda = initAFunLambda(AFunLambda);
  return AFunLambda;
}

inline
bool gsIsLambda(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunLambda();
}

// LinProcSpec
inline
AFun initAFunLinProcSpec(AFun& f)
{
  f = ATmakeAFun("LinProcSpec", 4, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunLinProcSpec()
{
  static AFun AFunLinProcSpec = initAFunLinProcSpec(AFunLinProcSpec);
  return AFunLinProcSpec;
}

inline
bool gsIsLinProcSpec(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunLinProcSpec();
}

// LinearProcess
inline
AFun initAFunLinearProcess(AFun& f)
{
  f = ATmakeAFun("LinearProcess", 3, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunLinearProcess()
{
  static AFun AFunLinearProcess = initAFunLinearProcess(AFunLinearProcess);
  return AFunLinearProcess;
}

inline
bool gsIsLinearProcess(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunLinearProcess();
}

// LinearProcessInit
inline
AFun initAFunLinearProcessInit(AFun& f)
{
  f = ATmakeAFun("LinearProcessInit", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunLinearProcessInit()
{
  static AFun AFunLinearProcessInit = initAFunLinearProcessInit(AFunLinearProcessInit);
  return AFunLinearProcessInit;
}

inline
bool gsIsLinearProcessInit(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunLinearProcessInit();
}

// LinearProcessSummand
inline
AFun initAFunLinearProcessSummand(AFun& f)
{
  f = ATmakeAFun("LinearProcessSummand", 5, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunLinearProcessSummand()
{
  static AFun AFunLinearProcessSummand = initAFunLinearProcessSummand(AFunLinearProcessSummand);
  return AFunLinearProcessSummand;
}

inline
bool gsIsLinearProcessSummand(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunLinearProcessSummand();
}

// MapSpec
inline
AFun initAFunMapSpec(AFun& f)
{
  f = ATmakeAFun("MapSpec", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunMapSpec()
{
  static AFun AFunMapSpec = initAFunMapSpec(AFunMapSpec);
  return AFunMapSpec;
}

inline
bool gsIsMapSpec(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunMapSpec();
}

// Merge
inline
AFun initAFunMerge(AFun& f)
{
  f = ATmakeAFun("Merge", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunMerge()
{
  static AFun AFunMerge = initAFunMerge(AFunMerge);
  return AFunMerge;
}

inline
bool gsIsMerge(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunMerge();
}

// Mu
inline
AFun initAFunMu(AFun& f)
{
  f = ATmakeAFun("Mu", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunMu()
{
  static AFun AFunMu = initAFunMu(AFunMu);
  return AFunMu;
}

inline
bool gsIsMu(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunMu();
}

// MultAct
inline
AFun initAFunMultAct(AFun& f)
{
  f = ATmakeAFun("MultAct", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunMultAct()
{
  static AFun AFunMultAct = initAFunMultAct(AFunMultAct);
  return AFunMultAct;
}

inline
bool gsIsMultAct(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunMultAct();
}

// MultActName
inline
AFun initAFunMultActName(AFun& f)
{
  f = ATmakeAFun("MultActName", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunMultActName()
{
  static AFun AFunMultActName = initAFunMultActName(AFunMultActName);
  return AFunMultActName;
}

inline
bool gsIsMultActName(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunMultActName();
}

// Nil
inline
AFun initAFunNil(AFun& f)
{
  f = ATmakeAFun("Nil", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunNil()
{
  static AFun AFunNil = initAFunNil(AFunNil);
  return AFunNil;
}

inline
bool gsIsNil(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunNil();
}

// Nu
inline
AFun initAFunNu(AFun& f)
{
  f = ATmakeAFun("Nu", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunNu()
{
  static AFun AFunNu = initAFunNu(AFunNu);
  return AFunNu;
}

inline
bool gsIsNu(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunNu();
}

// OpId
inline
AFun initAFunOpId(AFun& f)
{
  f = ATmakeAFun("OpId", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunOpId()
{
  static AFun AFunOpId = initAFunOpId(AFunOpId);
  return AFunOpId;
}

inline
bool gsIsOpId(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunOpId();
}

// PBES
inline
AFun initAFunPBES(AFun& f)
{
  f = ATmakeAFun("PBES", 3, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunPBES()
{
  static AFun AFunPBES = initAFunPBES(AFunPBES);
  return AFunPBES;
}

inline
bool gsIsPBES(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunPBES();
}

// PBESAnd
inline
AFun initAFunPBESAnd(AFun& f)
{
  f = ATmakeAFun("PBESAnd", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunPBESAnd()
{
  static AFun AFunPBESAnd = initAFunPBESAnd(AFunPBESAnd);
  return AFunPBESAnd;
}

inline
bool gsIsPBESAnd(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunPBESAnd();
}

// PBESExists
inline
AFun initAFunPBESExists(AFun& f)
{
  f = ATmakeAFun("PBESExists", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunPBESExists()
{
  static AFun AFunPBESExists = initAFunPBESExists(AFunPBESExists);
  return AFunPBESExists;
}

inline
bool gsIsPBESExists(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunPBESExists();
}

// PBESFalse
inline
AFun initAFunPBESFalse(AFun& f)
{
  f = ATmakeAFun("PBESFalse", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunPBESFalse()
{
  static AFun AFunPBESFalse = initAFunPBESFalse(AFunPBESFalse);
  return AFunPBESFalse;
}

inline
bool gsIsPBESFalse(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunPBESFalse();
}

// PBESForall
inline
AFun initAFunPBESForall(AFun& f)
{
  f = ATmakeAFun("PBESForall", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunPBESForall()
{
  static AFun AFunPBESForall = initAFunPBESForall(AFunPBESForall);
  return AFunPBESForall;
}

inline
bool gsIsPBESForall(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunPBESForall();
}

// PBESImp
inline
AFun initAFunPBESImp(AFun& f)
{
  f = ATmakeAFun("PBESImp", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunPBESImp()
{
  static AFun AFunPBESImp = initAFunPBESImp(AFunPBESImp);
  return AFunPBESImp;
}

inline
bool gsIsPBESImp(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunPBESImp();
}

// PBESNot
inline
AFun initAFunPBESNot(AFun& f)
{
  f = ATmakeAFun("PBESNot", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunPBESNot()
{
  static AFun AFunPBESNot = initAFunPBESNot(AFunPBESNot);
  return AFunPBESNot;
}

inline
bool gsIsPBESNot(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunPBESNot();
}

// PBESOr
inline
AFun initAFunPBESOr(AFun& f)
{
  f = ATmakeAFun("PBESOr", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunPBESOr()
{
  static AFun AFunPBESOr = initAFunPBESOr(AFunPBESOr);
  return AFunPBESOr;
}

inline
bool gsIsPBESOr(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunPBESOr();
}

// PBESTrue
inline
AFun initAFunPBESTrue(AFun& f)
{
  f = ATmakeAFun("PBESTrue", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunPBESTrue()
{
  static AFun AFunPBESTrue = initAFunPBESTrue(AFunPBESTrue);
  return AFunPBESTrue;
}

inline
bool gsIsPBESTrue(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunPBESTrue();
}

// PBEqn
inline
AFun initAFunPBEqn(AFun& f)
{
  f = ATmakeAFun("PBEqn", 3, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunPBEqn()
{
  static AFun AFunPBEqn = initAFunPBEqn(AFunPBEqn);
  return AFunPBEqn;
}

inline
bool gsIsPBEqn(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunPBEqn();
}

// PBEqnSpec
inline
AFun initAFunPBEqnSpec(AFun& f)
{
  f = ATmakeAFun("PBEqnSpec", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunPBEqnSpec()
{
  static AFun AFunPBEqnSpec = initAFunPBEqnSpec(AFunPBEqnSpec);
  return AFunPBEqnSpec;
}

inline
bool gsIsPBEqnSpec(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunPBEqnSpec();
}

// PBInit
inline
AFun initAFunPBInit(AFun& f)
{
  f = ATmakeAFun("PBInit", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunPBInit()
{
  static AFun AFunPBInit = initAFunPBInit(AFunPBInit);
  return AFunPBInit;
}

inline
bool gsIsPBInit(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunPBInit();
}

// ParamId
inline
AFun initAFunParamId(AFun& f)
{
  f = ATmakeAFun("ParamId", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunParamId()
{
  static AFun AFunParamId = initAFunParamId(AFunParamId);
  return AFunParamId;
}

inline
bool gsIsParamId(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunParamId();
}

// ProcEqn
inline
AFun initAFunProcEqn(AFun& f)
{
  f = ATmakeAFun("ProcEqn", 4, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunProcEqn()
{
  static AFun AFunProcEqn = initAFunProcEqn(AFunProcEqn);
  return AFunProcEqn;
}

inline
bool gsIsProcEqn(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunProcEqn();
}

// ProcEqnSpec
inline
AFun initAFunProcEqnSpec(AFun& f)
{
  f = ATmakeAFun("ProcEqnSpec", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunProcEqnSpec()
{
  static AFun AFunProcEqnSpec = initAFunProcEqnSpec(AFunProcEqnSpec);
  return AFunProcEqnSpec;
}

inline
bool gsIsProcEqnSpec(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunProcEqnSpec();
}

// ProcSpec
inline
AFun initAFunProcSpec(AFun& f)
{
  f = ATmakeAFun("ProcSpec", 4, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunProcSpec()
{
  static AFun AFunProcSpec = initAFunProcSpec(AFunProcSpec);
  return AFunProcSpec;
}

inline
bool gsIsProcSpec(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunProcSpec();
}

// ProcVarId
inline
AFun initAFunProcVarId(AFun& f)
{
  f = ATmakeAFun("ProcVarId", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunProcVarId()
{
  static AFun AFunProcVarId = initAFunProcVarId(AFunProcVarId);
  return AFunProcVarId;
}

inline
bool gsIsProcVarId(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunProcVarId();
}

// Process
inline
AFun initAFunProcess(AFun& f)
{
  f = ATmakeAFun("Process", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunProcess()
{
  static AFun AFunProcess = initAFunProcess(AFunProcess);
  return AFunProcess;
}

inline
bool gsIsProcess(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunProcess();
}

// ProcessAssignment
inline
AFun initAFunProcessAssignment(AFun& f)
{
  f = ATmakeAFun("ProcessAssignment", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunProcessAssignment()
{
  static AFun AFunProcessAssignment = initAFunProcessAssignment(AFunProcessAssignment);
  return AFunProcessAssignment;
}

inline
bool gsIsProcessAssignment(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunProcessAssignment();
}

// ProcessInit
inline
AFun initAFunProcessInit(AFun& f)
{
  f = ATmakeAFun("ProcessInit", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunProcessInit()
{
  static AFun AFunProcessInit = initAFunProcessInit(AFunProcessInit);
  return AFunProcessInit;
}

inline
bool gsIsProcessInit(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunProcessInit();
}

// PropVarDecl
inline
AFun initAFunPropVarDecl(AFun& f)
{
  f = ATmakeAFun("PropVarDecl", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunPropVarDecl()
{
  static AFun AFunPropVarDecl = initAFunPropVarDecl(AFunPropVarDecl);
  return AFunPropVarDecl;
}

inline
bool gsIsPropVarDecl(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunPropVarDecl();
}

// PropVarInst
inline
AFun initAFunPropVarInst(AFun& f)
{
  f = ATmakeAFun("PropVarInst", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunPropVarInst()
{
  static AFun AFunPropVarInst = initAFunPropVarInst(AFunPropVarInst);
  return AFunPropVarInst;
}

inline
bool gsIsPropVarInst(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunPropVarInst();
}

// RegAlt
inline
AFun initAFunRegAlt(AFun& f)
{
  f = ATmakeAFun("RegAlt", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunRegAlt()
{
  static AFun AFunRegAlt = initAFunRegAlt(AFunRegAlt);
  return AFunRegAlt;
}

inline
bool gsIsRegAlt(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunRegAlt();
}

// RegNil
inline
AFun initAFunRegNil(AFun& f)
{
  f = ATmakeAFun("RegNil", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunRegNil()
{
  static AFun AFunRegNil = initAFunRegNil(AFunRegNil);
  return AFunRegNil;
}

inline
bool gsIsRegNil(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunRegNil();
}

// RegSeq
inline
AFun initAFunRegSeq(AFun& f)
{
  f = ATmakeAFun("RegSeq", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunRegSeq()
{
  static AFun AFunRegSeq = initAFunRegSeq(AFunRegSeq);
  return AFunRegSeq;
}

inline
bool gsIsRegSeq(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunRegSeq();
}

// RegTrans
inline
AFun initAFunRegTrans(AFun& f)
{
  f = ATmakeAFun("RegTrans", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunRegTrans()
{
  static AFun AFunRegTrans = initAFunRegTrans(AFunRegTrans);
  return AFunRegTrans;
}

inline
bool gsIsRegTrans(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunRegTrans();
}

// RegTransOrNil
inline
AFun initAFunRegTransOrNil(AFun& f)
{
  f = ATmakeAFun("RegTransOrNil", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunRegTransOrNil()
{
  static AFun AFunRegTransOrNil = initAFunRegTransOrNil(AFunRegTransOrNil);
  return AFunRegTransOrNil;
}

inline
bool gsIsRegTransOrNil(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunRegTransOrNil();
}

// Rename
inline
AFun initAFunRename(AFun& f)
{
  f = ATmakeAFun("Rename", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunRename()
{
  static AFun AFunRename = initAFunRename(AFunRename);
  return AFunRename;
}

inline
bool gsIsRename(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunRename();
}

// RenameExpr
inline
AFun initAFunRenameExpr(AFun& f)
{
  f = ATmakeAFun("RenameExpr", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunRenameExpr()
{
  static AFun AFunRenameExpr = initAFunRenameExpr(AFunRenameExpr);
  return AFunRenameExpr;
}

inline
bool gsIsRenameExpr(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunRenameExpr();
}

// Seq
inline
AFun initAFunSeq(AFun& f)
{
  f = ATmakeAFun("Seq", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunSeq()
{
  static AFun AFunSeq = initAFunSeq(AFunSeq);
  return AFunSeq;
}

inline
bool gsIsSeq(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunSeq();
}

// SetBagComp
inline
AFun initAFunSetBagComp(AFun& f)
{
  f = ATmakeAFun("SetBagComp", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunSetBagComp()
{
  static AFun AFunSetBagComp = initAFunSetBagComp(AFunSetBagComp);
  return AFunSetBagComp;
}

inline
bool gsIsSetBagComp(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunSetBagComp();
}

// SetComp
inline
AFun initAFunSetComp(AFun& f)
{
  f = ATmakeAFun("SetComp", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunSetComp()
{
  static AFun AFunSetComp = initAFunSetComp(AFunSetComp);
  return AFunSetComp;
}

inline
bool gsIsSetComp(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunSetComp();
}

// SortArrow
inline
AFun initAFunSortArrow(AFun& f)
{
  f = ATmakeAFun("SortArrow", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunSortArrow()
{
  static AFun AFunSortArrow = initAFunSortArrow(AFunSortArrow);
  return AFunSortArrow;
}

inline
bool gsIsSortArrow(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunSortArrow();
}

// SortBag
inline
AFun initAFunSortBag(AFun& f)
{
  f = ATmakeAFun("SortBag", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunSortBag()
{
  static AFun AFunSortBag = initAFunSortBag(AFunSortBag);
  return AFunSortBag;
}

inline
bool gsIsSortBag(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunSortBag();
}

// SortCons
inline
AFun initAFunSortCons(AFun& f)
{
  f = ATmakeAFun("SortCons", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunSortCons()
{
  static AFun AFunSortCons = initAFunSortCons(AFunSortCons);
  return AFunSortCons;
}

inline
bool gsIsSortCons(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunSortCons();
}

// SortFBag
inline
AFun initAFunSortFBag(AFun& f)
{
  f = ATmakeAFun("SortFBag", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunSortFBag()
{
  static AFun AFunSortFBag = initAFunSortFBag(AFunSortFBag);
  return AFunSortFBag;
}

inline
bool gsIsSortFBag(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunSortFBag();
}

// SortFSet
inline
AFun initAFunSortFSet(AFun& f)
{
  f = ATmakeAFun("SortFSet", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunSortFSet()
{
  static AFun AFunSortFSet = initAFunSortFSet(AFunSortFSet);
  return AFunSortFSet;
}

inline
bool gsIsSortFSet(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunSortFSet();
}

// SortId
inline
AFun initAFunSortId(AFun& f)
{
  f = ATmakeAFun("SortId", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunSortId()
{
  static AFun AFunSortId = initAFunSortId(AFunSortId);
  return AFunSortId;
}

inline
bool gsIsSortId(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunSortId();
}

// SortList
inline
AFun initAFunSortList(AFun& f)
{
  f = ATmakeAFun("SortList", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunSortList()
{
  static AFun AFunSortList = initAFunSortList(AFunSortList);
  return AFunSortList;
}

inline
bool gsIsSortList(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunSortList();
}

// SortRef
inline
AFun initAFunSortRef(AFun& f)
{
  f = ATmakeAFun("SortRef", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunSortRef()
{
  static AFun AFunSortRef = initAFunSortRef(AFunSortRef);
  return AFunSortRef;
}

inline
bool gsIsSortRef(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunSortRef();
}

// SortSet
inline
AFun initAFunSortSet(AFun& f)
{
  f = ATmakeAFun("SortSet", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunSortSet()
{
  static AFun AFunSortSet = initAFunSortSet(AFunSortSet);
  return AFunSortSet;
}

inline
bool gsIsSortSet(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunSortSet();
}

// SortSpec
inline
AFun initAFunSortSpec(AFun& f)
{
  f = ATmakeAFun("SortSpec", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunSortSpec()
{
  static AFun AFunSortSpec = initAFunSortSpec(AFunSortSpec);
  return AFunSortSpec;
}

inline
bool gsIsSortSpec(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunSortSpec();
}

// SortStruct
inline
AFun initAFunSortStruct(AFun& f)
{
  f = ATmakeAFun("SortStruct", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunSortStruct()
{
  static AFun AFunSortStruct = initAFunSortStruct(AFunSortStruct);
  return AFunSortStruct;
}

inline
bool gsIsSortStruct(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunSortStruct();
}

// SortUnknown
inline
AFun initAFunSortUnknown(AFun& f)
{
  f = ATmakeAFun("SortUnknown", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunSortUnknown()
{
  static AFun AFunSortUnknown = initAFunSortUnknown(AFunSortUnknown);
  return AFunSortUnknown;
}

inline
bool gsIsSortUnknown(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunSortUnknown();
}

// SortsPossible
inline
AFun initAFunSortsPossible(AFun& f)
{
  f = ATmakeAFun("SortsPossible", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunSortsPossible()
{
  static AFun AFunSortsPossible = initAFunSortsPossible(AFunSortsPossible);
  return AFunSortsPossible;
}

inline
bool gsIsSortsPossible(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunSortsPossible();
}

// StateAnd
inline
AFun initAFunStateAnd(AFun& f)
{
  f = ATmakeAFun("StateAnd", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunStateAnd()
{
  static AFun AFunStateAnd = initAFunStateAnd(AFunStateAnd);
  return AFunStateAnd;
}

inline
bool gsIsStateAnd(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunStateAnd();
}

// StateDelay
inline
AFun initAFunStateDelay(AFun& f)
{
  f = ATmakeAFun("StateDelay", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunStateDelay()
{
  static AFun AFunStateDelay = initAFunStateDelay(AFunStateDelay);
  return AFunStateDelay;
}

inline
bool gsIsStateDelay(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunStateDelay();
}

// StateDelayTimed
inline
AFun initAFunStateDelayTimed(AFun& f)
{
  f = ATmakeAFun("StateDelayTimed", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunStateDelayTimed()
{
  static AFun AFunStateDelayTimed = initAFunStateDelayTimed(AFunStateDelayTimed);
  return AFunStateDelayTimed;
}

inline
bool gsIsStateDelayTimed(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunStateDelayTimed();
}

// StateExists
inline
AFun initAFunStateExists(AFun& f)
{
  f = ATmakeAFun("StateExists", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunStateExists()
{
  static AFun AFunStateExists = initAFunStateExists(AFunStateExists);
  return AFunStateExists;
}

inline
bool gsIsStateExists(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunStateExists();
}

// StateFalse
inline
AFun initAFunStateFalse(AFun& f)
{
  f = ATmakeAFun("StateFalse", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunStateFalse()
{
  static AFun AFunStateFalse = initAFunStateFalse(AFunStateFalse);
  return AFunStateFalse;
}

inline
bool gsIsStateFalse(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunStateFalse();
}

// StateForall
inline
AFun initAFunStateForall(AFun& f)
{
  f = ATmakeAFun("StateForall", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunStateForall()
{
  static AFun AFunStateForall = initAFunStateForall(AFunStateForall);
  return AFunStateForall;
}

inline
bool gsIsStateForall(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunStateForall();
}

// StateImp
inline
AFun initAFunStateImp(AFun& f)
{
  f = ATmakeAFun("StateImp", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunStateImp()
{
  static AFun AFunStateImp = initAFunStateImp(AFunStateImp);
  return AFunStateImp;
}

inline
bool gsIsStateImp(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunStateImp();
}

// StateMay
inline
AFun initAFunStateMay(AFun& f)
{
  f = ATmakeAFun("StateMay", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunStateMay()
{
  static AFun AFunStateMay = initAFunStateMay(AFunStateMay);
  return AFunStateMay;
}

inline
bool gsIsStateMay(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunStateMay();
}

// StateMu
inline
AFun initAFunStateMu(AFun& f)
{
  f = ATmakeAFun("StateMu", 3, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunStateMu()
{
  static AFun AFunStateMu = initAFunStateMu(AFunStateMu);
  return AFunStateMu;
}

inline
bool gsIsStateMu(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunStateMu();
}

// StateMust
inline
AFun initAFunStateMust(AFun& f)
{
  f = ATmakeAFun("StateMust", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunStateMust()
{
  static AFun AFunStateMust = initAFunStateMust(AFunStateMust);
  return AFunStateMust;
}

inline
bool gsIsStateMust(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunStateMust();
}

// StateNot
inline
AFun initAFunStateNot(AFun& f)
{
  f = ATmakeAFun("StateNot", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunStateNot()
{
  static AFun AFunStateNot = initAFunStateNot(AFunStateNot);
  return AFunStateNot;
}

inline
bool gsIsStateNot(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunStateNot();
}

// StateNu
inline
AFun initAFunStateNu(AFun& f)
{
  f = ATmakeAFun("StateNu", 3, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunStateNu()
{
  static AFun AFunStateNu = initAFunStateNu(AFunStateNu);
  return AFunStateNu;
}

inline
bool gsIsStateNu(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunStateNu();
}

// StateOr
inline
AFun initAFunStateOr(AFun& f)
{
  f = ATmakeAFun("StateOr", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunStateOr()
{
  static AFun AFunStateOr = initAFunStateOr(AFunStateOr);
  return AFunStateOr;
}

inline
bool gsIsStateOr(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunStateOr();
}

// StateTrue
inline
AFun initAFunStateTrue(AFun& f)
{
  f = ATmakeAFun("StateTrue", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunStateTrue()
{
  static AFun AFunStateTrue = initAFunStateTrue(AFunStateTrue);
  return AFunStateTrue;
}

inline
bool gsIsStateTrue(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunStateTrue();
}

// StateVar
inline
AFun initAFunStateVar(AFun& f)
{
  f = ATmakeAFun("StateVar", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunStateVar()
{
  static AFun AFunStateVar = initAFunStateVar(AFunStateVar);
  return AFunStateVar;
}

inline
bool gsIsStateVar(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunStateVar();
}

// StateYaled
inline
AFun initAFunStateYaled(AFun& f)
{
  f = ATmakeAFun("StateYaled", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunStateYaled()
{
  static AFun AFunStateYaled = initAFunStateYaled(AFunStateYaled);
  return AFunStateYaled;
}

inline
bool gsIsStateYaled(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunStateYaled();
}

// StateYaledTimed
inline
AFun initAFunStateYaledTimed(AFun& f)
{
  f = ATmakeAFun("StateYaledTimed", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunStateYaledTimed()
{
  static AFun AFunStateYaledTimed = initAFunStateYaledTimed(AFunStateYaledTimed);
  return AFunStateYaledTimed;
}

inline
bool gsIsStateYaledTimed(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunStateYaledTimed();
}

// StructCons
inline
AFun initAFunStructCons(AFun& f)
{
  f = ATmakeAFun("StructCons", 3, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunStructCons()
{
  static AFun AFunStructCons = initAFunStructCons(AFunStructCons);
  return AFunStructCons;
}

inline
bool gsIsStructCons(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunStructCons();
}

// StructProj
inline
AFun initAFunStructProj(AFun& f)
{
  f = ATmakeAFun("StructProj", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunStructProj()
{
  static AFun AFunStructProj = initAFunStructProj(AFunStructProj);
  return AFunStructProj;
}

inline
bool gsIsStructProj(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunStructProj();
}

// Sum
inline
AFun initAFunSum(AFun& f)
{
  f = ATmakeAFun("Sum", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunSum()
{
  static AFun AFunSum = initAFunSum(AFunSum);
  return AFunSum;
}

inline
bool gsIsSum(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunSum();
}

// Sync
inline
AFun initAFunSync(AFun& f)
{
  f = ATmakeAFun("Sync", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunSync()
{
  static AFun AFunSync = initAFunSync(AFunSync);
  return AFunSync;
}

inline
bool gsIsSync(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunSync();
}

// Tau
inline
AFun initAFunTau(AFun& f)
{
  f = ATmakeAFun("Tau", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunTau()
{
  static AFun AFunTau = initAFunTau(AFunTau);
  return AFunTau;
}

inline
bool gsIsTau(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunTau();
}

// Whr
inline
AFun initAFunWhr(AFun& f)
{
  f = ATmakeAFun("Whr", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunWhr()
{
  static AFun AFunWhr = initAFunWhr(AFunWhr);
  return AFunWhr;
}

inline
bool gsIsWhr(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunWhr();
}

inline
ATermAppl gsMakeActAnd(ATermAppl ActFrm_0, ATermAppl ActFrm_1)
{
  return ATmakeAppl2(gsAFunActAnd(), (ATerm) ActFrm_0, (ATerm) ActFrm_1);
}

inline
ATermAppl gsMakeActAt(ATermAppl ActFrm_0, ATermAppl DataExpr_1)
{
  return ATmakeAppl2(gsAFunActAt(), (ATerm) ActFrm_0, (ATerm) DataExpr_1);
}

inline
ATermAppl gsMakeActExists(ATermList DataVarId_0, ATermAppl ActFrm_1)
{
  return ATmakeAppl2(gsAFunActExists(), (ATerm) DataVarId_0, (ATerm) ActFrm_1);
}

inline
ATermAppl gsMakeActFalse()
{
  return ATmakeAppl0(gsAFunActFalse());
}

inline
ATermAppl gsMakeActForall(ATermList DataVarId_0, ATermAppl ActFrm_1)
{
  return ATmakeAppl2(gsAFunActForall(), (ATerm) DataVarId_0, (ATerm) ActFrm_1);
}

inline
ATermAppl gsMakeActId(ATermAppl String_0, ATermList SortExpr_1)
{
  return ATmakeAppl2(gsAFunActId(), (ATerm) String_0, (ATerm) SortExpr_1);
}

inline
ATermAppl gsMakeActImp(ATermAppl ActFrm_0, ATermAppl ActFrm_1)
{
  return ATmakeAppl2(gsAFunActImp(), (ATerm) ActFrm_0, (ATerm) ActFrm_1);
}

inline
ATermAppl gsMakeActNot(ATermAppl ActFrm_0)
{
  return ATmakeAppl1(gsAFunActNot(), (ATerm) ActFrm_0);
}

inline
ATermAppl gsMakeActOr(ATermAppl ActFrm_0, ATermAppl ActFrm_1)
{
  return ATmakeAppl2(gsAFunActOr(), (ATerm) ActFrm_0, (ATerm) ActFrm_1);
}

inline
ATermAppl gsMakeActSpec(ATermList ActId_0)
{
  return ATmakeAppl1(gsAFunActSpec(), (ATerm) ActId_0);
}

inline
ATermAppl gsMakeActTrue()
{
  return ATmakeAppl0(gsAFunActTrue());
}

inline
ATermAppl gsMakeAction(ATermAppl ActId_0, ATermList DataExpr_1)
{
  return ATmakeAppl2(gsAFunAction(), (ATerm) ActId_0, (ATerm) DataExpr_1);
}

inline
ATermAppl gsMakeActionRenameRule(ATermList DataVarId_0, ATermAppl DataExprOrNil_1, ATermAppl ParamIdOrAction_2, ATermAppl ActionRenameRuleRHS_3)
{
  return ATmakeAppl4(gsAFunActionRenameRule(), (ATerm) DataVarId_0, (ATerm) DataExprOrNil_1, (ATerm) ParamIdOrAction_2, (ATerm) ActionRenameRuleRHS_3);
}

inline
ATermAppl gsMakeActionRenameRules(ATermList ActionRenameRule_0)
{
  return ATmakeAppl1(gsAFunActionRenameRules(), (ATerm) ActionRenameRule_0);
}

inline
ATermAppl gsMakeActionRenameSpec(ATermAppl DataSpec_0, ATermAppl ActSpec_1, ATermAppl ActionRenameRules_2)
{
  return ATmakeAppl3(gsAFunActionRenameSpec(), (ATerm) DataSpec_0, (ATerm) ActSpec_1, (ATerm) ActionRenameRules_2);
}

inline
ATermAppl gsMakeAllow(ATermList MultActName_0, ATermAppl ProcExpr_1)
{
  return ATmakeAppl2(gsAFunAllow(), (ATerm) MultActName_0, (ATerm) ProcExpr_1);
}

inline
ATermAppl gsMakeAtTime(ATermAppl ProcExpr_0, ATermAppl DataExpr_1)
{
  return ATmakeAppl2(gsAFunAtTime(), (ATerm) ProcExpr_0, (ATerm) DataExpr_1);
}

inline
ATermAppl gsMakeBES(ATermList BooleanEquation_0, ATermAppl BooleanExpression_1)
{
  return ATmakeAppl2(gsAFunBES(), (ATerm) BooleanEquation_0, (ATerm) BooleanExpression_1);
}

inline
ATermAppl gsMakeBInit(ATermAppl ProcExpr_0, ATermAppl ProcExpr_1)
{
  return ATmakeAppl2(gsAFunBInit(), (ATerm) ProcExpr_0, (ATerm) ProcExpr_1);
}

inline
ATermAppl gsMakeBagComp()
{
  return ATmakeAppl0(gsAFunBagComp());
}

inline
ATermAppl gsMakeBinder(ATermAppl BindingOperator_0, ATermList DataVarId_1, ATermAppl DataExpr_2)
{
  return ATmakeAppl3(gsAFunBinder(), (ATerm) BindingOperator_0, (ATerm) DataVarId_1, (ATerm) DataExpr_2);
}

inline
ATermAppl gsMakeBlock(ATermList String_0, ATermAppl ProcExpr_1)
{
  return ATmakeAppl2(gsAFunBlock(), (ATerm) String_0, (ATerm) ProcExpr_1);
}

inline
ATermAppl gsMakeBooleanAnd(ATermAppl BooleanExpression_0, ATermAppl BooleanExpression_1)
{
  return ATmakeAppl2(gsAFunBooleanAnd(), (ATerm) BooleanExpression_0, (ATerm) BooleanExpression_1);
}

inline
ATermAppl gsMakeBooleanEquation(ATermAppl FixPoint_0, ATermAppl BooleanVariable_1, ATermAppl BooleanExpression_2)
{
  return ATmakeAppl3(gsAFunBooleanEquation(), (ATerm) FixPoint_0, (ATerm) BooleanVariable_1, (ATerm) BooleanExpression_2);
}

inline
ATermAppl gsMakeBooleanFalse()
{
  return ATmakeAppl0(gsAFunBooleanFalse());
}

inline
ATermAppl gsMakeBooleanImp(ATermAppl BooleanExpression_0, ATermAppl BooleanExpression_1)
{
  return ATmakeAppl2(gsAFunBooleanImp(), (ATerm) BooleanExpression_0, (ATerm) BooleanExpression_1);
}

inline
ATermAppl gsMakeBooleanNot(ATermAppl BooleanExpression_0)
{
  return ATmakeAppl1(gsAFunBooleanNot(), (ATerm) BooleanExpression_0);
}

inline
ATermAppl gsMakeBooleanOr(ATermAppl BooleanExpression_0, ATermAppl BooleanExpression_1)
{
  return ATmakeAppl2(gsAFunBooleanOr(), (ATerm) BooleanExpression_0, (ATerm) BooleanExpression_1);
}

inline
ATermAppl gsMakeBooleanTrue()
{
  return ATmakeAppl0(gsAFunBooleanTrue());
}

inline
ATermAppl gsMakeBooleanVariable(ATermAppl String_0)
{
  return ATmakeAppl1(gsAFunBooleanVariable(), (ATerm) String_0);
}

inline
ATermAppl gsMakeChoice(ATermAppl ProcExpr_0, ATermAppl ProcExpr_1)
{
  return ATmakeAppl2(gsAFunChoice(), (ATerm) ProcExpr_0, (ATerm) ProcExpr_1);
}

inline
ATermAppl gsMakeComm(ATermList CommExpr_0, ATermAppl ProcExpr_1)
{
  return ATmakeAppl2(gsAFunComm(), (ATerm) CommExpr_0, (ATerm) ProcExpr_1);
}

inline
ATermAppl gsMakeCommExpr(ATermAppl MultActName_0, ATermAppl StringOrNil_1)
{
  return ATmakeAppl2(gsAFunCommExpr(), (ATerm) MultActName_0, (ATerm) StringOrNil_1);
}

inline
ATermAppl gsMakeConsSpec(ATermList OpId_0)
{
  return ATmakeAppl1(gsAFunConsSpec(), (ATerm) OpId_0);
}

inline
ATermAppl gsMakeDataAppl(ATermAppl DataExpr_0, ATermList DataExpr_1)
{
  return ATmakeAppl2(gsAFunDataAppl(), (ATerm) DataExpr_0, (ATerm) DataExpr_1);
}

inline
ATermAppl gsMakeDataEqn(ATermList DataVarId_0, ATermAppl DataExprOrNil_1, ATermAppl DataExpr_2, ATermAppl DataExpr_3)
{
  return ATmakeAppl4(gsAFunDataEqn(), (ATerm) DataVarId_0, (ATerm) DataExprOrNil_1, (ATerm) DataExpr_2, (ATerm) DataExpr_3);
}

inline
ATermAppl gsMakeDataEqnSpec(ATermList DataEqn_0)
{
  return ATmakeAppl1(gsAFunDataEqnSpec(), (ATerm) DataEqn_0);
}

inline
ATermAppl gsMakeDataSpec(ATermAppl SortSpec_0, ATermAppl ConsSpec_1, ATermAppl MapSpec_2, ATermAppl DataEqnSpec_3)
{
  return ATmakeAppl4(gsAFunDataSpec(), (ATerm) SortSpec_0, (ATerm) ConsSpec_1, (ATerm) MapSpec_2, (ATerm) DataEqnSpec_3);
}

inline
ATermAppl gsMakeDataVarId(ATermAppl String_0, ATermAppl SortExpr_1)
{
  return ATmakeAppl2(gsAFunDataVarId(), (ATerm) String_0, (ATerm) SortExpr_1);
}

inline
ATermAppl gsMakeDataVarIdInit(ATermAppl DataVarId_0, ATermAppl DataExpr_1)
{
  return ATmakeAppl2(gsAFunDataVarIdInit(), (ATerm) DataVarId_0, (ATerm) DataExpr_1);
}

inline
ATermAppl gsMakeDelta()
{
  return ATmakeAppl0(gsAFunDelta());
}

inline
ATermAppl gsMakeExists()
{
  return ATmakeAppl0(gsAFunExists());
}

inline
ATermAppl gsMakeForall()
{
  return ATmakeAppl0(gsAFunForall());
}

inline
ATermAppl gsMakeHide(ATermList String_0, ATermAppl ProcExpr_1)
{
  return ATmakeAppl2(gsAFunHide(), (ATerm) String_0, (ATerm) ProcExpr_1);
}

inline
ATermAppl gsMakeId(ATermAppl String_0)
{
  return ATmakeAppl1(gsAFunId(), (ATerm) String_0);
}

inline
ATermAppl gsMakeIdAssignment(ATermAppl String_0, ATermList IdInit_1)
{
  return ATmakeAppl2(gsAFunIdAssignment(), (ATerm) String_0, (ATerm) IdInit_1);
}

inline
ATermAppl gsMakeIdInit(ATermAppl String_0, ATermAppl DataExpr_1)
{
  return ATmakeAppl2(gsAFunIdInit(), (ATerm) String_0, (ATerm) DataExpr_1);
}

inline
ATermAppl gsMakeIfThen(ATermAppl DataExpr_0, ATermAppl ProcExpr_1)
{
  return ATmakeAppl2(gsAFunIfThen(), (ATerm) DataExpr_0, (ATerm) ProcExpr_1);
}

inline
ATermAppl gsMakeIfThenElse(ATermAppl DataExpr_0, ATermAppl ProcExpr_1, ATermAppl ProcExpr_2)
{
  return ATmakeAppl3(gsAFunIfThenElse(), (ATerm) DataExpr_0, (ATerm) ProcExpr_1, (ATerm) ProcExpr_2);
}

inline
ATermAppl gsMakeLMerge(ATermAppl ProcExpr_0, ATermAppl ProcExpr_1)
{
  return ATmakeAppl2(gsAFunLMerge(), (ATerm) ProcExpr_0, (ATerm) ProcExpr_1);
}

inline
ATermAppl gsMakeLambda()
{
  return ATmakeAppl0(gsAFunLambda());
}

inline
ATermAppl gsMakeLinProcSpec(ATermAppl DataSpec_0, ATermAppl ActSpec_1, ATermAppl LinearProcess_2, ATermAppl LinearProcessInit_3)
{
  return ATmakeAppl4(gsAFunLinProcSpec(), (ATerm) DataSpec_0, (ATerm) ActSpec_1, (ATerm) LinearProcess_2, (ATerm) LinearProcessInit_3);
}

inline
ATermAppl gsMakeLinearProcess(ATermList DataVarId_0, ATermList DataVarId_1, ATermList LinearProcessSummand_2)
{
  return ATmakeAppl3(gsAFunLinearProcess(), (ATerm) DataVarId_0, (ATerm) DataVarId_1, (ATerm) LinearProcessSummand_2);
}

inline
ATermAppl gsMakeLinearProcessInit(ATermList DataVarId_0, ATermList DataVarIdInit_1)
{
  return ATmakeAppl2(gsAFunLinearProcessInit(), (ATerm) DataVarId_0, (ATerm) DataVarIdInit_1);
}

inline
ATermAppl gsMakeLinearProcessSummand(ATermList DataVarId_0, ATermAppl DataExpr_1, ATermAppl MultActOrDelta_2, ATermAppl DataExprOrNil_3, ATermList DataVarIdInit_4)
{
  return ATmakeAppl5(gsAFunLinearProcessSummand(), (ATerm) DataVarId_0, (ATerm) DataExpr_1, (ATerm) MultActOrDelta_2, (ATerm) DataExprOrNil_3, (ATerm) DataVarIdInit_4);
}

inline
ATermAppl gsMakeMapSpec(ATermList OpId_0)
{
  return ATmakeAppl1(gsAFunMapSpec(), (ATerm) OpId_0);
}

inline
ATermAppl gsMakeMerge(ATermAppl ProcExpr_0, ATermAppl ProcExpr_1)
{
  return ATmakeAppl2(gsAFunMerge(), (ATerm) ProcExpr_0, (ATerm) ProcExpr_1);
}

inline
ATermAppl gsMakeMu()
{
  return ATmakeAppl0(gsAFunMu());
}

inline
ATermAppl gsMakeMultAct(ATermList ParamIdOrAction_0)
{
  return ATmakeAppl1(gsAFunMultAct(), (ATerm) ParamIdOrAction_0);
}

inline
ATermAppl gsMakeMultActName(ATermList String_0)
{
  return ATmakeAppl1(gsAFunMultActName(), (ATerm) String_0);
}

inline
ATermAppl gsMakeNil()
{
  return ATmakeAppl0(gsAFunNil());
}

inline
ATermAppl gsMakeNu()
{
  return ATmakeAppl0(gsAFunNu());
}

inline
ATermAppl gsMakeOpId(ATermAppl String_0, ATermAppl SortExpr_1)
{
  return ATmakeAppl2(gsAFunOpId(), (ATerm) String_0, (ATerm) SortExpr_1);
}

inline
ATermAppl gsMakePBES(ATermAppl DataSpec_0, ATermAppl PBEqnSpec_1, ATermAppl PBInit_2)
{
  return ATmakeAppl3(gsAFunPBES(), (ATerm) DataSpec_0, (ATerm) PBEqnSpec_1, (ATerm) PBInit_2);
}

inline
ATermAppl gsMakePBESAnd(ATermAppl PBExpr_0, ATermAppl PBExpr_1)
{
  return ATmakeAppl2(gsAFunPBESAnd(), (ATerm) PBExpr_0, (ATerm) PBExpr_1);
}

inline
ATermAppl gsMakePBESExists(ATermList DataVarId_0, ATermAppl PBExpr_1)
{
  return ATmakeAppl2(gsAFunPBESExists(), (ATerm) DataVarId_0, (ATerm) PBExpr_1);
}

inline
ATermAppl gsMakePBESFalse()
{
  return ATmakeAppl0(gsAFunPBESFalse());
}

inline
ATermAppl gsMakePBESForall(ATermList DataVarId_0, ATermAppl PBExpr_1)
{
  return ATmakeAppl2(gsAFunPBESForall(), (ATerm) DataVarId_0, (ATerm) PBExpr_1);
}

inline
ATermAppl gsMakePBESImp(ATermAppl PBExpr_0, ATermAppl PBExpr_1)
{
  return ATmakeAppl2(gsAFunPBESImp(), (ATerm) PBExpr_0, (ATerm) PBExpr_1);
}

inline
ATermAppl gsMakePBESNot(ATermAppl PBExpr_0)
{
  return ATmakeAppl1(gsAFunPBESNot(), (ATerm) PBExpr_0);
}

inline
ATermAppl gsMakePBESOr(ATermAppl PBExpr_0, ATermAppl PBExpr_1)
{
  return ATmakeAppl2(gsAFunPBESOr(), (ATerm) PBExpr_0, (ATerm) PBExpr_1);
}

inline
ATermAppl gsMakePBESTrue()
{
  return ATmakeAppl0(gsAFunPBESTrue());
}

inline
ATermAppl gsMakePBEqn(ATermAppl FixPoint_0, ATermAppl PropVarDecl_1, ATermAppl PBExpr_2)
{
  return ATmakeAppl3(gsAFunPBEqn(), (ATerm) FixPoint_0, (ATerm) PropVarDecl_1, (ATerm) PBExpr_2);
}

inline
ATermAppl gsMakePBEqnSpec(ATermList DataVarId_0, ATermList PBEqn_1)
{
  return ATmakeAppl2(gsAFunPBEqnSpec(), (ATerm) DataVarId_0, (ATerm) PBEqn_1);
}

inline
ATermAppl gsMakePBInit(ATermList DataVarId_0, ATermAppl PropVarInst_1)
{
  return ATmakeAppl2(gsAFunPBInit(), (ATerm) DataVarId_0, (ATerm) PropVarInst_1);
}

inline
ATermAppl gsMakeParamId(ATermAppl String_0, ATermList DataExpr_1)
{
  return ATmakeAppl2(gsAFunParamId(), (ATerm) String_0, (ATerm) DataExpr_1);
}

inline
ATermAppl gsMakeProcEqn(ATermList DataVarId_0, ATermAppl ProcVarId_1, ATermList DataVarId_2, ATermAppl ProcExpr_3)
{
  return ATmakeAppl4(gsAFunProcEqn(), (ATerm) DataVarId_0, (ATerm) ProcVarId_1, (ATerm) DataVarId_2, (ATerm) ProcExpr_3);
}

inline
ATermAppl gsMakeProcEqnSpec(ATermList ProcEqn_0)
{
  return ATmakeAppl1(gsAFunProcEqnSpec(), (ATerm) ProcEqn_0);
}

inline
ATermAppl gsMakeProcSpec(ATermAppl DataSpec_0, ATermAppl ActSpec_1, ATermAppl ProcEqnSpec_2, ATermAppl ProcInit_3)
{
  return ATmakeAppl4(gsAFunProcSpec(), (ATerm) DataSpec_0, (ATerm) ActSpec_1, (ATerm) ProcEqnSpec_2, (ATerm) ProcInit_3);
}

inline
ATermAppl gsMakeProcVarId(ATermAppl String_0, ATermList SortExpr_1)
{
  return ATmakeAppl2(gsAFunProcVarId(), (ATerm) String_0, (ATerm) SortExpr_1);
}

inline
ATermAppl gsMakeProcess(ATermAppl ProcVarId_0, ATermList DataExpr_1)
{
  // Check whether lengths of process type and its arguments match.
  // Could be replaced by at test for equal types.

  assert(ATgetLength((ATermList)ATgetArgument(ProcVarId_0,1))==ATgetLength(DataExpr_1));
  return ATmakeAppl2(gsAFunProcess(), (ATerm) ProcVarId_0, (ATerm) DataExpr_1);
}

inline
ATermAppl gsMakeProcessAssignment(ATermAppl ProcVarId_0, ATermList DataVarIdInit_1)
{
  return ATmakeAppl2(gsAFunProcessAssignment(), (ATerm) ProcVarId_0, (ATerm) DataVarIdInit_1);
}

inline
ATermAppl gsMakeProcessInit(ATermList DataVarId_0, ATermAppl ProcExpr_1)
{
  return ATmakeAppl2(gsAFunProcessInit(), (ATerm) DataVarId_0, (ATerm) ProcExpr_1);
}

inline
ATermAppl gsMakePropVarDecl(ATermAppl String_0, ATermList DataVarId_1)
{
  return ATmakeAppl2(gsAFunPropVarDecl(), (ATerm) String_0, (ATerm) DataVarId_1);
}

inline
ATermAppl gsMakePropVarInst(ATermAppl String_0, ATermList DataExpr_1)
{
  return ATmakeAppl2(gsAFunPropVarInst(), (ATerm) String_0, (ATerm) DataExpr_1);
}

inline
ATermAppl gsMakeRegAlt(ATermAppl RegFrm_0, ATermAppl RegFrm_1)
{
  return ATmakeAppl2(gsAFunRegAlt(), (ATerm) RegFrm_0, (ATerm) RegFrm_1);
}

inline
ATermAppl gsMakeRegNil()
{
  return ATmakeAppl0(gsAFunRegNil());
}

inline
ATermAppl gsMakeRegSeq(ATermAppl RegFrm_0, ATermAppl RegFrm_1)
{
  return ATmakeAppl2(gsAFunRegSeq(), (ATerm) RegFrm_0, (ATerm) RegFrm_1);
}

inline
ATermAppl gsMakeRegTrans(ATermAppl RegFrm_0)
{
  return ATmakeAppl1(gsAFunRegTrans(), (ATerm) RegFrm_0);
}

inline
ATermAppl gsMakeRegTransOrNil(ATermAppl RegFrm_0)
{
  return ATmakeAppl1(gsAFunRegTransOrNil(), (ATerm) RegFrm_0);
}

inline
ATermAppl gsMakeRename(ATermList RenameExpr_0, ATermAppl ProcExpr_1)
{
  return ATmakeAppl2(gsAFunRename(), (ATerm) RenameExpr_0, (ATerm) ProcExpr_1);
}

inline
ATermAppl gsMakeRenameExpr(ATermAppl String_0, ATermAppl String_1)
{
  return ATmakeAppl2(gsAFunRenameExpr(), (ATerm) String_0, (ATerm) String_1);
}

inline
ATermAppl gsMakeSeq(ATermAppl ProcExpr_0, ATermAppl ProcExpr_1)
{
  return ATmakeAppl2(gsAFunSeq(), (ATerm) ProcExpr_0, (ATerm) ProcExpr_1);
}

inline
ATermAppl gsMakeSetBagComp()
{
  return ATmakeAppl0(gsAFunSetBagComp());
}

inline
ATermAppl gsMakeSetComp()
{
  return ATmakeAppl0(gsAFunSetComp());
}

inline
ATermAppl gsMakeSortArrow(ATermList SortExpr_0, ATermAppl SortExpr_1)
{
  return ATmakeAppl2(gsAFunSortArrow(), (ATerm) SortExpr_0, (ATerm) SortExpr_1);
}

inline
ATermAppl gsMakeSortBag()
{
  return ATmakeAppl0(gsAFunSortBag());
}

inline
ATermAppl gsMakeSortCons(ATermAppl SortConsType_0, ATermAppl SortExpr_1)
{
  return ATmakeAppl2(gsAFunSortCons(), (ATerm) SortConsType_0, (ATerm) SortExpr_1);
}

inline
ATermAppl gsMakeSortFBag()
{
  return ATmakeAppl0(gsAFunSortFBag());
}

inline
ATermAppl gsMakeSortFSet()
{
  return ATmakeAppl0(gsAFunSortFSet());
}

inline
ATermAppl gsMakeSortId(ATermAppl String_0)
{
  return ATmakeAppl1(gsAFunSortId(), (ATerm) String_0);
}

inline
ATermAppl gsMakeSortList()
{
  return ATmakeAppl0(gsAFunSortList());
}

inline
ATermAppl gsMakeSortRef(ATermAppl String_0, ATermAppl SortExpr_1)
{
  return ATmakeAppl2(gsAFunSortRef(), (ATerm) String_0, (ATerm) SortExpr_1);
}

inline
ATermAppl gsMakeSortSet()
{
  return ATmakeAppl0(gsAFunSortSet());
}

inline
ATermAppl gsMakeSortSpec(ATermList SortDecl_0)
{
  return ATmakeAppl1(gsAFunSortSpec(), (ATerm) SortDecl_0);
}

inline
ATermAppl gsMakeSortStruct(ATermList StructCons_0)
{
  return ATmakeAppl1(gsAFunSortStruct(), (ATerm) StructCons_0);
}

inline
ATermAppl gsMakeSortUnknown()
{
  return ATmakeAppl0(gsAFunSortUnknown());
}

inline
ATermAppl gsMakeSortsPossible(ATermList SortExpr_0)
{
  return ATmakeAppl1(gsAFunSortsPossible(), (ATerm) SortExpr_0);
}

inline
ATermAppl gsMakeStateAnd(ATermAppl StateFrm_0, ATermAppl StateFrm_1)
{
  return ATmakeAppl2(gsAFunStateAnd(), (ATerm) StateFrm_0, (ATerm) StateFrm_1);
}

inline
ATermAppl gsMakeStateDelay()
{
  return ATmakeAppl0(gsAFunStateDelay());
}

inline
ATermAppl gsMakeStateDelayTimed(ATermAppl DataExpr_0)
{
  return ATmakeAppl1(gsAFunStateDelayTimed(), (ATerm) DataExpr_0);
}

inline
ATermAppl gsMakeStateExists(ATermList DataVarId_0, ATermAppl StateFrm_1)
{
  return ATmakeAppl2(gsAFunStateExists(), (ATerm) DataVarId_0, (ATerm) StateFrm_1);
}

inline
ATermAppl gsMakeStateFalse()
{
  return ATmakeAppl0(gsAFunStateFalse());
}

inline
ATermAppl gsMakeStateForall(ATermList DataVarId_0, ATermAppl StateFrm_1)
{
  return ATmakeAppl2(gsAFunStateForall(), (ATerm) DataVarId_0, (ATerm) StateFrm_1);
}

inline
ATermAppl gsMakeStateImp(ATermAppl StateFrm_0, ATermAppl StateFrm_1)
{
  return ATmakeAppl2(gsAFunStateImp(), (ATerm) StateFrm_0, (ATerm) StateFrm_1);
}

inline
ATermAppl gsMakeStateMay(ATermAppl RegFrm_0, ATermAppl StateFrm_1)
{
  return ATmakeAppl2(gsAFunStateMay(), (ATerm) RegFrm_0, (ATerm) StateFrm_1);
}

inline
ATermAppl gsMakeStateMu(ATermAppl String_0, ATermList DataVarIdInit_1, ATermAppl StateFrm_2)
{
  return ATmakeAppl3(gsAFunStateMu(), (ATerm) String_0, (ATerm) DataVarIdInit_1, (ATerm) StateFrm_2);
}

inline
ATermAppl gsMakeStateMust(ATermAppl RegFrm_0, ATermAppl StateFrm_1)
{
  return ATmakeAppl2(gsAFunStateMust(), (ATerm) RegFrm_0, (ATerm) StateFrm_1);
}

inline
ATermAppl gsMakeStateNot(ATermAppl StateFrm_0)
{
  return ATmakeAppl1(gsAFunStateNot(), (ATerm) StateFrm_0);
}

inline
ATermAppl gsMakeStateNu(ATermAppl String_0, ATermList DataVarIdInit_1, ATermAppl StateFrm_2)
{
  return ATmakeAppl3(gsAFunStateNu(), (ATerm) String_0, (ATerm) DataVarIdInit_1, (ATerm) StateFrm_2);
}

inline
ATermAppl gsMakeStateOr(ATermAppl StateFrm_0, ATermAppl StateFrm_1)
{
  return ATmakeAppl2(gsAFunStateOr(), (ATerm) StateFrm_0, (ATerm) StateFrm_1);
}

inline
ATermAppl gsMakeStateTrue()
{
  return ATmakeAppl0(gsAFunStateTrue());
}

inline
ATermAppl gsMakeStateVar(ATermAppl String_0, ATermList DataExpr_1)
{
  return ATmakeAppl2(gsAFunStateVar(), (ATerm) String_0, (ATerm) DataExpr_1);
}

inline
ATermAppl gsMakeStateYaled()
{
  return ATmakeAppl0(gsAFunStateYaled());
}

inline
ATermAppl gsMakeStateYaledTimed(ATermAppl DataExpr_0)
{
  return ATmakeAppl1(gsAFunStateYaledTimed(), (ATerm) DataExpr_0);
}

inline
ATermAppl gsMakeStructCons(ATermAppl String_0, ATermList StructProj_1, ATermAppl StringOrNil_2)
{
  return ATmakeAppl3(gsAFunStructCons(), (ATerm) String_0, (ATerm) StructProj_1, (ATerm) StringOrNil_2);
}

inline
ATermAppl gsMakeStructProj(ATermAppl StringOrNil_0, ATermAppl SortExpr_1)
{
  return ATmakeAppl2(gsAFunStructProj(), (ATerm) StringOrNil_0, (ATerm) SortExpr_1);
}

inline
ATermAppl gsMakeSum(ATermList DataVarId_0, ATermAppl ProcExpr_1)
{
  return ATmakeAppl2(gsAFunSum(), (ATerm) DataVarId_0, (ATerm) ProcExpr_1);
}

inline
ATermAppl gsMakeSync(ATermAppl ProcExpr_0, ATermAppl ProcExpr_1)
{
  return ATmakeAppl2(gsAFunSync(), (ATerm) ProcExpr_0, (ATerm) ProcExpr_1);
}

inline
ATermAppl gsMakeTau()
{
  return ATmakeAppl0(gsAFunTau());
}

inline
ATermAppl gsMakeWhr(ATermAppl DataExpr_0, ATermList WhrDecl_1)
{
  return ATmakeAppl2(gsAFunWhr(), (ATerm) DataExpr_0, (ATerm) WhrDecl_1);
}
//--- end generated code

//Conversion between strings and quoted ATermAppl's
//-------------------------------------------------

inline
ATermAppl gsString2ATermAppl(const char *s)
//Ret: quoted constant s, if s != NULL
//     unquoted constant Nil, if s == NULL
{
  if (s != NULL) {
    return ATmakeAppl0(ATmakeAFun(s, 0, ATtrue));
  } else {
    return gsMakeNil();
  }
}

inline
bool gsIsString(ATermAppl term)
//Ret: term is a quoted constant
{
  AFun head = ATgetAFun(term);
  return ((ATgetArity(head) == 0) && (ATisQuoted(head) == ATtrue));
}

inline
char *gsATermAppl2String(ATermAppl term)
//Ret: string s, if term is a quoted constant s
//     NULL, otherwise
{
  if (gsIsString(term)) {
    return ATgetName(ATgetAFun(term));
  } else {
    return NULL;
  }
}

inline
bool gsIsNumericString(const char* s)
//Ret: true if s is of form "0 | -? [1-9][0-9]*", false otherwise
{
  if (s == NULL) return false;
  if (s[0] == '\0') return false;
  if (s[0] == '-') ++s;
  if (s[0] == '\0') return false;
  if (s[0] == '0') {
    ++s;
    if (s[0] == '\0') return true;
    else return false;
  }
  for (; s[0] != '\0'; ++s)
    if(!isdigit(s[0])) return false;
  return true;
}

ATermAppl gsFreshString2ATermAppl(const char *s, ATerm Term, bool TryNoSuffix);
//Pre: Term is an ATerm containing ATermAppl's and ATermList's only
//     s is not NULL
//Ret: "s", if it does not occur in Term, and TryNoSuffix holds
//     "sk" as a quoted ATermAppl constant, where k is the smallest natural
//     number such that "sk" does not occur in Term, otherwise

    }
  }
}

#endif // MCRL2_LIBSTRUCT_CORE_H
