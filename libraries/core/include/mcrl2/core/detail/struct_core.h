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

#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/core/detail/construction_utility.h"

namespace mcrl2
{
namespace core
{
namespace detail
{

//Global precondition: the ATerm library has been initialised

//--- start generated code ---//
// ActAnd
inline
atermpp::function_symbol function_symbol_ActAnd()
{
  static atermpp::function_symbol function_symbol_ActAnd = core::detail::initialise_static_expression(function_symbol_ActAnd, atermpp::function_symbol("ActAnd", 2));
  return function_symbol_ActAnd;
}

inline
bool gsIsActAnd(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_ActAnd();
}

// ActAt
inline
atermpp::function_symbol function_symbol_ActAt()
{
  static atermpp::function_symbol function_symbol_ActAt = core::detail::initialise_static_expression(function_symbol_ActAt, atermpp::function_symbol("ActAt", 2));
  return function_symbol_ActAt;
}

inline
bool gsIsActAt(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_ActAt();
}

// ActExists
inline
atermpp::function_symbol function_symbol_ActExists()
{
  static atermpp::function_symbol function_symbol_ActExists = core::detail::initialise_static_expression(function_symbol_ActExists, atermpp::function_symbol("ActExists", 2));
  return function_symbol_ActExists;
}

inline
bool gsIsActExists(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_ActExists();
}

// ActFalse
inline
atermpp::function_symbol function_symbol_ActFalse()
{
  static atermpp::function_symbol function_symbol_ActFalse = core::detail::initialise_static_expression(function_symbol_ActFalse, atermpp::function_symbol("ActFalse", 0));
  return function_symbol_ActFalse;
}

inline
bool gsIsActFalse(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_ActFalse();
}

// ActForall
inline
atermpp::function_symbol function_symbol_ActForall()
{
  static atermpp::function_symbol function_symbol_ActForall = core::detail::initialise_static_expression(function_symbol_ActForall, atermpp::function_symbol("ActForall", 2));
  return function_symbol_ActForall;
}

inline
bool gsIsActForall(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_ActForall();
}

// ActId
inline
atermpp::function_symbol function_symbol_ActId()
{
  static atermpp::function_symbol function_symbol_ActId = core::detail::initialise_static_expression(function_symbol_ActId, atermpp::function_symbol("ActId", 2));
  return function_symbol_ActId;
}

inline
bool gsIsActId(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_ActId();
}

// ActImp
inline
atermpp::function_symbol function_symbol_ActImp()
{
  static atermpp::function_symbol function_symbol_ActImp = core::detail::initialise_static_expression(function_symbol_ActImp, atermpp::function_symbol("ActImp", 2));
  return function_symbol_ActImp;
}

inline
bool gsIsActImp(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_ActImp();
}

// ActNot
inline
atermpp::function_symbol function_symbol_ActNot()
{
  static atermpp::function_symbol function_symbol_ActNot = core::detail::initialise_static_expression(function_symbol_ActNot, atermpp::function_symbol("ActNot", 1));
  return function_symbol_ActNot;
}

inline
bool gsIsActNot(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_ActNot();
}

// ActOr
inline
atermpp::function_symbol function_symbol_ActOr()
{
  static atermpp::function_symbol function_symbol_ActOr = core::detail::initialise_static_expression(function_symbol_ActOr, atermpp::function_symbol("ActOr", 2));
  return function_symbol_ActOr;
}

inline
bool gsIsActOr(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_ActOr();
}

// ActSpec
inline
atermpp::function_symbol function_symbol_ActSpec()
{
  static atermpp::function_symbol function_symbol_ActSpec = core::detail::initialise_static_expression(function_symbol_ActSpec, atermpp::function_symbol("ActSpec", 1));
  return function_symbol_ActSpec;
}

inline
bool gsIsActSpec(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_ActSpec();
}

// ActTrue
inline
atermpp::function_symbol function_symbol_ActTrue()
{
  static atermpp::function_symbol function_symbol_ActTrue = core::detail::initialise_static_expression(function_symbol_ActTrue, atermpp::function_symbol("ActTrue", 0));
  return function_symbol_ActTrue;
}

inline
bool gsIsActTrue(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_ActTrue();
}

// Action
inline
atermpp::function_symbol function_symbol_Action()
{
  static atermpp::function_symbol function_symbol_Action = core::detail::initialise_static_expression(function_symbol_Action, atermpp::function_symbol("Action", 2));
  return function_symbol_Action;
}

inline
bool gsIsAction(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_Action();
}

// ActionRenameRule
inline
atermpp::function_symbol function_symbol_ActionRenameRule()
{
  static atermpp::function_symbol function_symbol_ActionRenameRule = core::detail::initialise_static_expression(function_symbol_ActionRenameRule, atermpp::function_symbol("ActionRenameRule", 4));
  return function_symbol_ActionRenameRule;
}

inline
bool gsIsActionRenameRule(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_ActionRenameRule();
}

// ActionRenameRules
inline
atermpp::function_symbol function_symbol_ActionRenameRules()
{
  static atermpp::function_symbol function_symbol_ActionRenameRules = core::detail::initialise_static_expression(function_symbol_ActionRenameRules, atermpp::function_symbol("ActionRenameRules", 1));
  return function_symbol_ActionRenameRules;
}

inline
bool gsIsActionRenameRules(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_ActionRenameRules();
}

// ActionRenameSpec
inline
atermpp::function_symbol function_symbol_ActionRenameSpec()
{
  static atermpp::function_symbol function_symbol_ActionRenameSpec = core::detail::initialise_static_expression(function_symbol_ActionRenameSpec, atermpp::function_symbol("ActionRenameSpec", 3));
  return function_symbol_ActionRenameSpec;
}

inline
bool gsIsActionRenameSpec(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_ActionRenameSpec();
}

// Allow
inline
atermpp::function_symbol function_symbol_Allow()
{
  static atermpp::function_symbol function_symbol_Allow = core::detail::initialise_static_expression(function_symbol_Allow, atermpp::function_symbol("Allow", 2));
  return function_symbol_Allow;
}

inline
bool gsIsAllow(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_Allow();
}

// AtTime
inline
atermpp::function_symbol function_symbol_AtTime()
{
  static atermpp::function_symbol function_symbol_AtTime = core::detail::initialise_static_expression(function_symbol_AtTime, atermpp::function_symbol("AtTime", 2));
  return function_symbol_AtTime;
}

inline
bool gsIsAtTime(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_AtTime();
}

// BES
inline
atermpp::function_symbol function_symbol_BES()
{
  static atermpp::function_symbol function_symbol_BES = core::detail::initialise_static_expression(function_symbol_BES, atermpp::function_symbol("BES", 2));
  return function_symbol_BES;
}

inline
bool gsIsBES(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_BES();
}

// BInit
inline
atermpp::function_symbol function_symbol_BInit()
{
  static atermpp::function_symbol function_symbol_BInit = core::detail::initialise_static_expression(function_symbol_BInit, atermpp::function_symbol("BInit", 2));
  return function_symbol_BInit;
}

inline
bool gsIsBInit(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_BInit();
}

// BagComp
inline
atermpp::function_symbol function_symbol_BagComp()
{
  static atermpp::function_symbol function_symbol_BagComp = core::detail::initialise_static_expression(function_symbol_BagComp, atermpp::function_symbol("BagComp", 0));
  return function_symbol_BagComp;
}

inline
bool gsIsBagComp(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_BagComp();
}

// Binder
inline
atermpp::function_symbol function_symbol_Binder()
{
  static atermpp::function_symbol function_symbol_Binder = core::detail::initialise_static_expression(function_symbol_Binder, atermpp::function_symbol("Binder", 3));
  return function_symbol_Binder;
}

inline
bool gsIsBinder(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_Binder();
}

// Block
inline
atermpp::function_symbol function_symbol_Block()
{
  static atermpp::function_symbol function_symbol_Block = core::detail::initialise_static_expression(function_symbol_Block, atermpp::function_symbol("Block", 2));
  return function_symbol_Block;
}

inline
bool gsIsBlock(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_Block();
}

// BooleanAnd
inline
atermpp::function_symbol function_symbol_BooleanAnd()
{
  static atermpp::function_symbol function_symbol_BooleanAnd = core::detail::initialise_static_expression(function_symbol_BooleanAnd, atermpp::function_symbol("BooleanAnd", 2));
  return function_symbol_BooleanAnd;
}

inline
bool gsIsBooleanAnd(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_BooleanAnd();
}

// BooleanEquation
inline
atermpp::function_symbol function_symbol_BooleanEquation()
{
  static atermpp::function_symbol function_symbol_BooleanEquation = core::detail::initialise_static_expression(function_symbol_BooleanEquation, atermpp::function_symbol("BooleanEquation", 3));
  return function_symbol_BooleanEquation;
}

inline
bool gsIsBooleanEquation(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_BooleanEquation();
}

// BooleanFalse
inline
atermpp::function_symbol function_symbol_BooleanFalse()
{
  static atermpp::function_symbol function_symbol_BooleanFalse = core::detail::initialise_static_expression(function_symbol_BooleanFalse, atermpp::function_symbol("BooleanFalse", 0));
  return function_symbol_BooleanFalse;
}

inline
bool gsIsBooleanFalse(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_BooleanFalse();
}

// BooleanImp
inline
atermpp::function_symbol function_symbol_BooleanImp()
{
  static atermpp::function_symbol function_symbol_BooleanImp = core::detail::initialise_static_expression(function_symbol_BooleanImp, atermpp::function_symbol("BooleanImp", 2));
  return function_symbol_BooleanImp;
}

inline
bool gsIsBooleanImp(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_BooleanImp();
}

// BooleanNot
inline
atermpp::function_symbol function_symbol_BooleanNot()
{
  static atermpp::function_symbol function_symbol_BooleanNot = core::detail::initialise_static_expression(function_symbol_BooleanNot, atermpp::function_symbol("BooleanNot", 1));
  return function_symbol_BooleanNot;
}

inline
bool gsIsBooleanNot(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_BooleanNot();
}

// BooleanOr
inline
atermpp::function_symbol function_symbol_BooleanOr()
{
  static atermpp::function_symbol function_symbol_BooleanOr = core::detail::initialise_static_expression(function_symbol_BooleanOr, atermpp::function_symbol("BooleanOr", 2));
  return function_symbol_BooleanOr;
}

inline
bool gsIsBooleanOr(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_BooleanOr();
}

// BooleanTrue
inline
atermpp::function_symbol function_symbol_BooleanTrue()
{
  static atermpp::function_symbol function_symbol_BooleanTrue = core::detail::initialise_static_expression(function_symbol_BooleanTrue, atermpp::function_symbol("BooleanTrue", 0));
  return function_symbol_BooleanTrue;
}

inline
bool gsIsBooleanTrue(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_BooleanTrue();
}

// BooleanVariable
inline
atermpp::function_symbol function_symbol_BooleanVariable()
{
  static atermpp::function_symbol function_symbol_BooleanVariable = core::detail::initialise_static_expression(function_symbol_BooleanVariable, atermpp::function_symbol("BooleanVariable", 1));
  return function_symbol_BooleanVariable;
}

inline
bool gsIsBooleanVariable(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_BooleanVariable();
}

// Choice
inline
atermpp::function_symbol function_symbol_Choice()
{
  static atermpp::function_symbol function_symbol_Choice = core::detail::initialise_static_expression(function_symbol_Choice, atermpp::function_symbol("Choice", 2));
  return function_symbol_Choice;
}

inline
bool gsIsChoice(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_Choice();
}

// Comm
inline
atermpp::function_symbol function_symbol_Comm()
{
  static atermpp::function_symbol function_symbol_Comm = core::detail::initialise_static_expression(function_symbol_Comm, atermpp::function_symbol("Comm", 2));
  return function_symbol_Comm;
}

inline
bool gsIsComm(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_Comm();
}

// CommExpr
inline
atermpp::function_symbol function_symbol_CommExpr()
{
  static atermpp::function_symbol function_symbol_CommExpr = core::detail::initialise_static_expression(function_symbol_CommExpr, atermpp::function_symbol("CommExpr", 2));
  return function_symbol_CommExpr;
}

inline
bool gsIsCommExpr(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_CommExpr();
}

// ConsSpec
inline
atermpp::function_symbol function_symbol_ConsSpec()
{
  static atermpp::function_symbol function_symbol_ConsSpec = core::detail::initialise_static_expression(function_symbol_ConsSpec, atermpp::function_symbol("ConsSpec", 1));
  return function_symbol_ConsSpec;
}

inline
bool gsIsConsSpec(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_ConsSpec();
}

// DataAppl
inline
atermpp::function_symbol function_symbol_DataAppl()
{
  static atermpp::function_symbol function_symbol_DataAppl = core::detail::initialise_static_expression(function_symbol_DataAppl, atermpp::function_symbol("DataAppl", 2));
  return function_symbol_DataAppl;
}

inline
bool gsIsDataAppl(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_DataAppl();
}

// DataEqn
inline
atermpp::function_symbol function_symbol_DataEqn()
{
  static atermpp::function_symbol function_symbol_DataEqn = core::detail::initialise_static_expression(function_symbol_DataEqn, atermpp::function_symbol("DataEqn", 4));
  return function_symbol_DataEqn;
}

inline
bool gsIsDataEqn(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_DataEqn();
}

// DataEqnSpec
inline
atermpp::function_symbol function_symbol_DataEqnSpec()
{
  static atermpp::function_symbol function_symbol_DataEqnSpec = core::detail::initialise_static_expression(function_symbol_DataEqnSpec, atermpp::function_symbol("DataEqnSpec", 1));
  return function_symbol_DataEqnSpec;
}

inline
bool gsIsDataEqnSpec(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_DataEqnSpec();
}

// DataSpec
inline
atermpp::function_symbol function_symbol_DataSpec()
{
  static atermpp::function_symbol function_symbol_DataSpec = core::detail::initialise_static_expression(function_symbol_DataSpec, atermpp::function_symbol("DataSpec", 4));
  return function_symbol_DataSpec;
}

inline
bool gsIsDataSpec(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_DataSpec();
}

// DataVarId
inline
atermpp::function_symbol function_symbol_DataVarId()
{
  static atermpp::function_symbol function_symbol_DataVarId = core::detail::initialise_static_expression(function_symbol_DataVarId, atermpp::function_symbol("DataVarId", 2));
  return function_symbol_DataVarId;
}

inline
bool gsIsDataVarId(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_DataVarId();
}

// DataVarIdInit
inline
atermpp::function_symbol function_symbol_DataVarIdInit()
{
  static atermpp::function_symbol function_symbol_DataVarIdInit = core::detail::initialise_static_expression(function_symbol_DataVarIdInit, atermpp::function_symbol("DataVarIdInit", 2));
  return function_symbol_DataVarIdInit;
}

inline
bool gsIsDataVarIdInit(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_DataVarIdInit();
}

// Delta
inline
atermpp::function_symbol function_symbol_Delta()
{
  static atermpp::function_symbol function_symbol_Delta = core::detail::initialise_static_expression(function_symbol_Delta, atermpp::function_symbol("Delta", 0));
  return function_symbol_Delta;
}

inline
bool gsIsDelta(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_Delta();
}

// Exists
inline
atermpp::function_symbol function_symbol_Exists()
{
  static atermpp::function_symbol function_symbol_Exists = core::detail::initialise_static_expression(function_symbol_Exists, atermpp::function_symbol("Exists", 0));
  return function_symbol_Exists;
}

inline
bool gsIsExists(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_Exists();
}

// Forall
inline
atermpp::function_symbol function_symbol_Forall()
{
  static atermpp::function_symbol function_symbol_Forall = core::detail::initialise_static_expression(function_symbol_Forall, atermpp::function_symbol("Forall", 0));
  return function_symbol_Forall;
}

inline
bool gsIsForall(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_Forall();
}

// GlobVarSpec
inline
atermpp::function_symbol function_symbol_GlobVarSpec()
{
  static atermpp::function_symbol function_symbol_GlobVarSpec = core::detail::initialise_static_expression(function_symbol_GlobVarSpec, atermpp::function_symbol("GlobVarSpec", 1));
  return function_symbol_GlobVarSpec;
}

inline
bool gsIsGlobVarSpec(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_GlobVarSpec();
}

// Hide
inline
atermpp::function_symbol function_symbol_Hide()
{
  static atermpp::function_symbol function_symbol_Hide = core::detail::initialise_static_expression(function_symbol_Hide, atermpp::function_symbol("Hide", 2));
  return function_symbol_Hide;
}

inline
bool gsIsHide(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_Hide();
}

// Id
inline
atermpp::function_symbol function_symbol_Id()
{
  static atermpp::function_symbol function_symbol_Id = core::detail::initialise_static_expression(function_symbol_Id, atermpp::function_symbol("Id", 1));
  return function_symbol_Id;
}

inline
bool gsIsId(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_Id();
}

// IdAssignment
inline
atermpp::function_symbol function_symbol_IdAssignment()
{
  static atermpp::function_symbol function_symbol_IdAssignment = core::detail::initialise_static_expression(function_symbol_IdAssignment, atermpp::function_symbol("IdAssignment", 2));
  return function_symbol_IdAssignment;
}

inline
bool gsIsIdAssignment(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_IdAssignment();
}

// IdInit
inline
atermpp::function_symbol function_symbol_IdInit()
{
  static atermpp::function_symbol function_symbol_IdInit = core::detail::initialise_static_expression(function_symbol_IdInit, atermpp::function_symbol("IdInit", 2));
  return function_symbol_IdInit;
}

inline
bool gsIsIdInit(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_IdInit();
}

// IfThen
inline
atermpp::function_symbol function_symbol_IfThen()
{
  static atermpp::function_symbol function_symbol_IfThen = core::detail::initialise_static_expression(function_symbol_IfThen, atermpp::function_symbol("IfThen", 2));
  return function_symbol_IfThen;
}

inline
bool gsIsIfThen(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_IfThen();
}

// IfThenElse
inline
atermpp::function_symbol function_symbol_IfThenElse()
{
  static atermpp::function_symbol function_symbol_IfThenElse = core::detail::initialise_static_expression(function_symbol_IfThenElse, atermpp::function_symbol("IfThenElse", 3));
  return function_symbol_IfThenElse;
}

inline
bool gsIsIfThenElse(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_IfThenElse();
}

// LMerge
inline
atermpp::function_symbol function_symbol_LMerge()
{
  static atermpp::function_symbol function_symbol_LMerge = core::detail::initialise_static_expression(function_symbol_LMerge, atermpp::function_symbol("LMerge", 2));
  return function_symbol_LMerge;
}

inline
bool gsIsLMerge(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_LMerge();
}

// Lambda
inline
atermpp::function_symbol function_symbol_Lambda()
{
  static atermpp::function_symbol function_symbol_Lambda = core::detail::initialise_static_expression(function_symbol_Lambda, atermpp::function_symbol("Lambda", 0));
  return function_symbol_Lambda;
}

inline
bool gsIsLambda(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_Lambda();
}

// LinProcSpec
inline
atermpp::function_symbol function_symbol_LinProcSpec()
{
  static atermpp::function_symbol function_symbol_LinProcSpec = core::detail::initialise_static_expression(function_symbol_LinProcSpec, atermpp::function_symbol("LinProcSpec", 5));
  return function_symbol_LinProcSpec;
}

inline
bool gsIsLinProcSpec(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_LinProcSpec();
}

// LinearProcess
inline
atermpp::function_symbol function_symbol_LinearProcess()
{
  static atermpp::function_symbol function_symbol_LinearProcess = core::detail::initialise_static_expression(function_symbol_LinearProcess, atermpp::function_symbol("LinearProcess", 2));
  return function_symbol_LinearProcess;
}

inline
bool gsIsLinearProcess(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_LinearProcess();
}

// LinearProcessInit
inline
atermpp::function_symbol function_symbol_LinearProcessInit()
{
  static atermpp::function_symbol function_symbol_LinearProcessInit = core::detail::initialise_static_expression(function_symbol_LinearProcessInit, atermpp::function_symbol("LinearProcessInit", 1));
  return function_symbol_LinearProcessInit;
}

inline
bool gsIsLinearProcessInit(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_LinearProcessInit();
}

// LinearProcessSummand
inline
atermpp::function_symbol function_symbol_LinearProcessSummand()
{
  static atermpp::function_symbol function_symbol_LinearProcessSummand = core::detail::initialise_static_expression(function_symbol_LinearProcessSummand, atermpp::function_symbol("LinearProcessSummand", 5));
  return function_symbol_LinearProcessSummand;
}

inline
bool gsIsLinearProcessSummand(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_LinearProcessSummand();
}

// MapSpec
inline
atermpp::function_symbol function_symbol_MapSpec()
{
  static atermpp::function_symbol function_symbol_MapSpec = core::detail::initialise_static_expression(function_symbol_MapSpec, atermpp::function_symbol("MapSpec", 1));
  return function_symbol_MapSpec;
}

inline
bool gsIsMapSpec(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_MapSpec();
}

// Merge
inline
atermpp::function_symbol function_symbol_Merge()
{
  static atermpp::function_symbol function_symbol_Merge = core::detail::initialise_static_expression(function_symbol_Merge, atermpp::function_symbol("Merge", 2));
  return function_symbol_Merge;
}

inline
bool gsIsMerge(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_Merge();
}

// Mu
inline
atermpp::function_symbol function_symbol_Mu()
{
  static atermpp::function_symbol function_symbol_Mu = core::detail::initialise_static_expression(function_symbol_Mu, atermpp::function_symbol("Mu", 0));
  return function_symbol_Mu;
}

inline
bool gsIsMu(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_Mu();
}

// MultAct
inline
atermpp::function_symbol function_symbol_MultAct()
{
  static atermpp::function_symbol function_symbol_MultAct = core::detail::initialise_static_expression(function_symbol_MultAct, atermpp::function_symbol("MultAct", 1));
  return function_symbol_MultAct;
}

inline
bool gsIsMultAct(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_MultAct();
}

// MultActName
inline
atermpp::function_symbol function_symbol_MultActName()
{
  static atermpp::function_symbol function_symbol_MultActName = core::detail::initialise_static_expression(function_symbol_MultActName, atermpp::function_symbol("MultActName", 1));
  return function_symbol_MultActName;
}

inline
bool gsIsMultActName(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_MultActName();
}

// Nil
inline
atermpp::function_symbol function_symbol_Nil()
{
  static atermpp::function_symbol function_symbol_Nil = core::detail::initialise_static_expression(function_symbol_Nil, atermpp::function_symbol("Nil", 0));
  return function_symbol_Nil;
}

inline
bool gsIsNil(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_Nil();
}

// Nu
inline
atermpp::function_symbol function_symbol_Nu()
{
  static atermpp::function_symbol function_symbol_Nu = core::detail::initialise_static_expression(function_symbol_Nu, atermpp::function_symbol("Nu", 0));
  return function_symbol_Nu;
}

inline
bool gsIsNu(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_Nu();
}

// OpId
inline
atermpp::function_symbol function_symbol_OpId()
{
  static atermpp::function_symbol function_symbol_OpId = core::detail::initialise_static_expression(function_symbol_OpId, atermpp::function_symbol("OpId", 2));
  return function_symbol_OpId;
}

inline
bool gsIsOpId(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_OpId();
}

// PBES
inline
atermpp::function_symbol function_symbol_PBES()
{
  static atermpp::function_symbol function_symbol_PBES = core::detail::initialise_static_expression(function_symbol_PBES, atermpp::function_symbol("PBES", 4));
  return function_symbol_PBES;
}

inline
bool gsIsPBES(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_PBES();
}

// PBESAnd
inline
atermpp::function_symbol function_symbol_PBESAnd()
{
  static atermpp::function_symbol function_symbol_PBESAnd = core::detail::initialise_static_expression(function_symbol_PBESAnd, atermpp::function_symbol("PBESAnd", 2));
  return function_symbol_PBESAnd;
}

inline
bool gsIsPBESAnd(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_PBESAnd();
}

// PBESExists
inline
atermpp::function_symbol function_symbol_PBESExists()
{
  static atermpp::function_symbol function_symbol_PBESExists = core::detail::initialise_static_expression(function_symbol_PBESExists, atermpp::function_symbol("PBESExists", 2));
  return function_symbol_PBESExists;
}

inline
bool gsIsPBESExists(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_PBESExists();
}

// PBESFalse
inline
atermpp::function_symbol function_symbol_PBESFalse()
{
  static atermpp::function_symbol function_symbol_PBESFalse = core::detail::initialise_static_expression(function_symbol_PBESFalse, atermpp::function_symbol("PBESFalse", 0));
  return function_symbol_PBESFalse;
}

inline
bool gsIsPBESFalse(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_PBESFalse();
}

// PBESForall
inline
atermpp::function_symbol function_symbol_PBESForall()
{
  static atermpp::function_symbol function_symbol_PBESForall = core::detail::initialise_static_expression(function_symbol_PBESForall, atermpp::function_symbol("PBESForall", 2));
  return function_symbol_PBESForall;
}

inline
bool gsIsPBESForall(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_PBESForall();
}

// PBESImp
inline
atermpp::function_symbol function_symbol_PBESImp()
{
  static atermpp::function_symbol function_symbol_PBESImp = core::detail::initialise_static_expression(function_symbol_PBESImp, atermpp::function_symbol("PBESImp", 2));
  return function_symbol_PBESImp;
}

inline
bool gsIsPBESImp(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_PBESImp();
}

// PBESNot
inline
atermpp::function_symbol function_symbol_PBESNot()
{
  static atermpp::function_symbol function_symbol_PBESNot = core::detail::initialise_static_expression(function_symbol_PBESNot, atermpp::function_symbol("PBESNot", 1));
  return function_symbol_PBESNot;
}

inline
bool gsIsPBESNot(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_PBESNot();
}

// PBESOr
inline
atermpp::function_symbol function_symbol_PBESOr()
{
  static atermpp::function_symbol function_symbol_PBESOr = core::detail::initialise_static_expression(function_symbol_PBESOr, atermpp::function_symbol("PBESOr", 2));
  return function_symbol_PBESOr;
}

inline
bool gsIsPBESOr(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_PBESOr();
}

// PBESTrue
inline
atermpp::function_symbol function_symbol_PBESTrue()
{
  static atermpp::function_symbol function_symbol_PBESTrue = core::detail::initialise_static_expression(function_symbol_PBESTrue, atermpp::function_symbol("PBESTrue", 0));
  return function_symbol_PBESTrue;
}

inline
bool gsIsPBESTrue(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_PBESTrue();
}

// PBEqn
inline
atermpp::function_symbol function_symbol_PBEqn()
{
  static atermpp::function_symbol function_symbol_PBEqn = core::detail::initialise_static_expression(function_symbol_PBEqn, atermpp::function_symbol("PBEqn", 3));
  return function_symbol_PBEqn;
}

inline
bool gsIsPBEqn(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_PBEqn();
}

// PBEqnSpec
inline
atermpp::function_symbol function_symbol_PBEqnSpec()
{
  static atermpp::function_symbol function_symbol_PBEqnSpec = core::detail::initialise_static_expression(function_symbol_PBEqnSpec, atermpp::function_symbol("PBEqnSpec", 1));
  return function_symbol_PBEqnSpec;
}

inline
bool gsIsPBEqnSpec(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_PBEqnSpec();
}

// PBInit
inline
atermpp::function_symbol function_symbol_PBInit()
{
  static atermpp::function_symbol function_symbol_PBInit = core::detail::initialise_static_expression(function_symbol_PBInit, atermpp::function_symbol("PBInit", 1));
  return function_symbol_PBInit;
}

inline
bool gsIsPBInit(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_PBInit();
}

// ParamId
inline
atermpp::function_symbol function_symbol_ParamId()
{
  static atermpp::function_symbol function_symbol_ParamId = core::detail::initialise_static_expression(function_symbol_ParamId, atermpp::function_symbol("ParamId", 2));
  return function_symbol_ParamId;
}

inline
bool gsIsParamId(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_ParamId();
}

// ProcEqn
inline
atermpp::function_symbol function_symbol_ProcEqn()
{
  static atermpp::function_symbol function_symbol_ProcEqn = core::detail::initialise_static_expression(function_symbol_ProcEqn, atermpp::function_symbol("ProcEqn", 3));
  return function_symbol_ProcEqn;
}

inline
bool gsIsProcEqn(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_ProcEqn();
}

// ProcEqnSpec
inline
atermpp::function_symbol function_symbol_ProcEqnSpec()
{
  static atermpp::function_symbol function_symbol_ProcEqnSpec = core::detail::initialise_static_expression(function_symbol_ProcEqnSpec, atermpp::function_symbol("ProcEqnSpec", 1));
  return function_symbol_ProcEqnSpec;
}

inline
bool gsIsProcEqnSpec(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_ProcEqnSpec();
}

// ProcSpec
inline
atermpp::function_symbol function_symbol_ProcSpec()
{
  static atermpp::function_symbol function_symbol_ProcSpec = core::detail::initialise_static_expression(function_symbol_ProcSpec, atermpp::function_symbol("ProcSpec", 5));
  return function_symbol_ProcSpec;
}

inline
bool gsIsProcSpec(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_ProcSpec();
}

// ProcVarId
inline
atermpp::function_symbol function_symbol_ProcVarId()
{
  static atermpp::function_symbol function_symbol_ProcVarId = core::detail::initialise_static_expression(function_symbol_ProcVarId, atermpp::function_symbol("ProcVarId", 2));
  return function_symbol_ProcVarId;
}

inline
bool gsIsProcVarId(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_ProcVarId();
}

// Process
inline
atermpp::function_symbol function_symbol_Process()
{
  static atermpp::function_symbol function_symbol_Process = core::detail::initialise_static_expression(function_symbol_Process, atermpp::function_symbol("Process", 2));
  return function_symbol_Process;
}

inline
bool gsIsProcess(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_Process();
}

// ProcessAssignment
inline
atermpp::function_symbol function_symbol_ProcessAssignment()
{
  static atermpp::function_symbol function_symbol_ProcessAssignment = core::detail::initialise_static_expression(function_symbol_ProcessAssignment, atermpp::function_symbol("ProcessAssignment", 2));
  return function_symbol_ProcessAssignment;
}

inline
bool gsIsProcessAssignment(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_ProcessAssignment();
}

// ProcessInit
inline
atermpp::function_symbol function_symbol_ProcessInit()
{
  static atermpp::function_symbol function_symbol_ProcessInit = core::detail::initialise_static_expression(function_symbol_ProcessInit, atermpp::function_symbol("ProcessInit", 1));
  return function_symbol_ProcessInit;
}

inline
bool gsIsProcessInit(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_ProcessInit();
}

// PropVarDecl
inline
atermpp::function_symbol function_symbol_PropVarDecl()
{
  static atermpp::function_symbol function_symbol_PropVarDecl = core::detail::initialise_static_expression(function_symbol_PropVarDecl, atermpp::function_symbol("PropVarDecl", 2));
  return function_symbol_PropVarDecl;
}

inline
bool gsIsPropVarDecl(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_PropVarDecl();
}

// PropVarInst
inline
atermpp::function_symbol function_symbol_PropVarInst()
{
  static atermpp::function_symbol function_symbol_PropVarInst = core::detail::initialise_static_expression(function_symbol_PropVarInst, atermpp::function_symbol("PropVarInst", 2));
  return function_symbol_PropVarInst;
}

inline
bool gsIsPropVarInst(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_PropVarInst();
}

// RegAlt
inline
atermpp::function_symbol function_symbol_RegAlt()
{
  static atermpp::function_symbol function_symbol_RegAlt = core::detail::initialise_static_expression(function_symbol_RegAlt, atermpp::function_symbol("RegAlt", 2));
  return function_symbol_RegAlt;
}

inline
bool gsIsRegAlt(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_RegAlt();
}

// RegNil
inline
atermpp::function_symbol function_symbol_RegNil()
{
  static atermpp::function_symbol function_symbol_RegNil = core::detail::initialise_static_expression(function_symbol_RegNil, atermpp::function_symbol("RegNil", 0));
  return function_symbol_RegNil;
}

inline
bool gsIsRegNil(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_RegNil();
}

// RegSeq
inline
atermpp::function_symbol function_symbol_RegSeq()
{
  static atermpp::function_symbol function_symbol_RegSeq = core::detail::initialise_static_expression(function_symbol_RegSeq, atermpp::function_symbol("RegSeq", 2));
  return function_symbol_RegSeq;
}

inline
bool gsIsRegSeq(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_RegSeq();
}

// RegTrans
inline
atermpp::function_symbol function_symbol_RegTrans()
{
  static atermpp::function_symbol function_symbol_RegTrans = core::detail::initialise_static_expression(function_symbol_RegTrans, atermpp::function_symbol("RegTrans", 1));
  return function_symbol_RegTrans;
}

inline
bool gsIsRegTrans(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_RegTrans();
}

// RegTransOrNil
inline
atermpp::function_symbol function_symbol_RegTransOrNil()
{
  static atermpp::function_symbol function_symbol_RegTransOrNil = core::detail::initialise_static_expression(function_symbol_RegTransOrNil, atermpp::function_symbol("RegTransOrNil", 1));
  return function_symbol_RegTransOrNil;
}

inline
bool gsIsRegTransOrNil(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_RegTransOrNil();
}

// Rename
inline
atermpp::function_symbol function_symbol_Rename()
{
  static atermpp::function_symbol function_symbol_Rename = core::detail::initialise_static_expression(function_symbol_Rename, atermpp::function_symbol("Rename", 2));
  return function_symbol_Rename;
}

inline
bool gsIsRename(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_Rename();
}

// RenameExpr
inline
atermpp::function_symbol function_symbol_RenameExpr()
{
  static atermpp::function_symbol function_symbol_RenameExpr = core::detail::initialise_static_expression(function_symbol_RenameExpr, atermpp::function_symbol("RenameExpr", 2));
  return function_symbol_RenameExpr;
}

inline
bool gsIsRenameExpr(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_RenameExpr();
}

// Seq
inline
atermpp::function_symbol function_symbol_Seq()
{
  static atermpp::function_symbol function_symbol_Seq = core::detail::initialise_static_expression(function_symbol_Seq, atermpp::function_symbol("Seq", 2));
  return function_symbol_Seq;
}

inline
bool gsIsSeq(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_Seq();
}

// SetBagComp
inline
atermpp::function_symbol function_symbol_SetBagComp()
{
  static atermpp::function_symbol function_symbol_SetBagComp = core::detail::initialise_static_expression(function_symbol_SetBagComp, atermpp::function_symbol("SetBagComp", 0));
  return function_symbol_SetBagComp;
}

inline
bool gsIsSetBagComp(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_SetBagComp();
}

// SetComp
inline
atermpp::function_symbol function_symbol_SetComp()
{
  static atermpp::function_symbol function_symbol_SetComp = core::detail::initialise_static_expression(function_symbol_SetComp, atermpp::function_symbol("SetComp", 0));
  return function_symbol_SetComp;
}

inline
bool gsIsSetComp(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_SetComp();
}

// SortArrow
inline
atermpp::function_symbol function_symbol_SortArrow()
{
  static atermpp::function_symbol function_symbol_SortArrow = core::detail::initialise_static_expression(function_symbol_SortArrow, atermpp::function_symbol("SortArrow", 2));
  return function_symbol_SortArrow;
}

inline
bool gsIsSortArrow(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_SortArrow();
}

// SortBag
inline
atermpp::function_symbol function_symbol_SortBag()
{
  static atermpp::function_symbol function_symbol_SortBag = core::detail::initialise_static_expression(function_symbol_SortBag, atermpp::function_symbol("SortBag", 0));
  return function_symbol_SortBag;
}

inline
bool gsIsSortBag(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_SortBag();
}

// SortCons
inline
atermpp::function_symbol function_symbol_SortCons()
{
  static atermpp::function_symbol function_symbol_SortCons = core::detail::initialise_static_expression(function_symbol_SortCons, atermpp::function_symbol("SortCons", 2));
  return function_symbol_SortCons;
}

inline
bool gsIsSortCons(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_SortCons();
}

// SortFBag
inline
atermpp::function_symbol function_symbol_SortFBag()
{
  static atermpp::function_symbol function_symbol_SortFBag = core::detail::initialise_static_expression(function_symbol_SortFBag, atermpp::function_symbol("SortFBag", 0));
  return function_symbol_SortFBag;
}

inline
bool gsIsSortFBag(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_SortFBag();
}

// SortFSet
inline
atermpp::function_symbol function_symbol_SortFSet()
{
  static atermpp::function_symbol function_symbol_SortFSet = core::detail::initialise_static_expression(function_symbol_SortFSet, atermpp::function_symbol("SortFSet", 0));
  return function_symbol_SortFSet;
}

inline
bool gsIsSortFSet(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_SortFSet();
}

// SortId
inline
atermpp::function_symbol function_symbol_SortId()
{
  static atermpp::function_symbol function_symbol_SortId = core::detail::initialise_static_expression(function_symbol_SortId, atermpp::function_symbol("SortId", 1));
  return function_symbol_SortId;
}

inline
bool gsIsSortId(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_SortId();
}

// SortList
inline
atermpp::function_symbol function_symbol_SortList()
{
  static atermpp::function_symbol function_symbol_SortList = core::detail::initialise_static_expression(function_symbol_SortList, atermpp::function_symbol("SortList", 0));
  return function_symbol_SortList;
}

inline
bool gsIsSortList(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_SortList();
}

// SortRef
inline
atermpp::function_symbol function_symbol_SortRef()
{
  static atermpp::function_symbol function_symbol_SortRef = core::detail::initialise_static_expression(function_symbol_SortRef, atermpp::function_symbol("SortRef", 2));
  return function_symbol_SortRef;
}

inline
bool gsIsSortRef(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_SortRef();
}

// SortSet
inline
atermpp::function_symbol function_symbol_SortSet()
{
  static atermpp::function_symbol function_symbol_SortSet = core::detail::initialise_static_expression(function_symbol_SortSet, atermpp::function_symbol("SortSet", 0));
  return function_symbol_SortSet;
}

inline
bool gsIsSortSet(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_SortSet();
}

// SortSpec
inline
atermpp::function_symbol function_symbol_SortSpec()
{
  static atermpp::function_symbol function_symbol_SortSpec = core::detail::initialise_static_expression(function_symbol_SortSpec, atermpp::function_symbol("SortSpec", 1));
  return function_symbol_SortSpec;
}

inline
bool gsIsSortSpec(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_SortSpec();
}

// SortStruct
inline
atermpp::function_symbol function_symbol_SortStruct()
{
  static atermpp::function_symbol function_symbol_SortStruct = core::detail::initialise_static_expression(function_symbol_SortStruct, atermpp::function_symbol("SortStruct", 1));
  return function_symbol_SortStruct;
}

inline
bool gsIsSortStruct(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_SortStruct();
}

// SortUnknown
inline
atermpp::function_symbol function_symbol_SortUnknown()
{
  static atermpp::function_symbol function_symbol_SortUnknown = core::detail::initialise_static_expression(function_symbol_SortUnknown, atermpp::function_symbol("SortUnknown", 0));
  return function_symbol_SortUnknown;
}

inline
bool gsIsSortUnknown(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_SortUnknown();
}

// SortsPossible
inline
atermpp::function_symbol function_symbol_SortsPossible()
{
  static atermpp::function_symbol function_symbol_SortsPossible = core::detail::initialise_static_expression(function_symbol_SortsPossible, atermpp::function_symbol("SortsPossible", 1));
  return function_symbol_SortsPossible;
}

inline
bool gsIsSortsPossible(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_SortsPossible();
}

// StateAnd
inline
atermpp::function_symbol function_symbol_StateAnd()
{
  static atermpp::function_symbol function_symbol_StateAnd = core::detail::initialise_static_expression(function_symbol_StateAnd, atermpp::function_symbol("StateAnd", 2));
  return function_symbol_StateAnd;
}

inline
bool gsIsStateAnd(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_StateAnd();
}

// StateDelay
inline
atermpp::function_symbol function_symbol_StateDelay()
{
  static atermpp::function_symbol function_symbol_StateDelay = core::detail::initialise_static_expression(function_symbol_StateDelay, atermpp::function_symbol("StateDelay", 0));
  return function_symbol_StateDelay;
}

inline
bool gsIsStateDelay(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_StateDelay();
}

// StateDelayTimed
inline
atermpp::function_symbol function_symbol_StateDelayTimed()
{
  static atermpp::function_symbol function_symbol_StateDelayTimed = core::detail::initialise_static_expression(function_symbol_StateDelayTimed, atermpp::function_symbol("StateDelayTimed", 1));
  return function_symbol_StateDelayTimed;
}

inline
bool gsIsStateDelayTimed(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_StateDelayTimed();
}

// StateExists
inline
atermpp::function_symbol function_symbol_StateExists()
{
  static atermpp::function_symbol function_symbol_StateExists = core::detail::initialise_static_expression(function_symbol_StateExists, atermpp::function_symbol("StateExists", 2));
  return function_symbol_StateExists;
}

inline
bool gsIsStateExists(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_StateExists();
}

// StateFalse
inline
atermpp::function_symbol function_symbol_StateFalse()
{
  static atermpp::function_symbol function_symbol_StateFalse = core::detail::initialise_static_expression(function_symbol_StateFalse, atermpp::function_symbol("StateFalse", 0));
  return function_symbol_StateFalse;
}

inline
bool gsIsStateFalse(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_StateFalse();
}

// StateForall
inline
atermpp::function_symbol function_symbol_StateForall()
{
  static atermpp::function_symbol function_symbol_StateForall = core::detail::initialise_static_expression(function_symbol_StateForall, atermpp::function_symbol("StateForall", 2));
  return function_symbol_StateForall;
}

inline
bool gsIsStateForall(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_StateForall();
}

// StateImp
inline
atermpp::function_symbol function_symbol_StateImp()
{
  static atermpp::function_symbol function_symbol_StateImp = core::detail::initialise_static_expression(function_symbol_StateImp, atermpp::function_symbol("StateImp", 2));
  return function_symbol_StateImp;
}

inline
bool gsIsStateImp(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_StateImp();
}

// StateMay
inline
atermpp::function_symbol function_symbol_StateMay()
{
  static atermpp::function_symbol function_symbol_StateMay = core::detail::initialise_static_expression(function_symbol_StateMay, atermpp::function_symbol("StateMay", 2));
  return function_symbol_StateMay;
}

inline
bool gsIsStateMay(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_StateMay();
}

// StateMu
inline
atermpp::function_symbol function_symbol_StateMu()
{
  static atermpp::function_symbol function_symbol_StateMu = core::detail::initialise_static_expression(function_symbol_StateMu, atermpp::function_symbol("StateMu", 3));
  return function_symbol_StateMu;
}

inline
bool gsIsStateMu(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_StateMu();
}

// StateMust
inline
atermpp::function_symbol function_symbol_StateMust()
{
  static atermpp::function_symbol function_symbol_StateMust = core::detail::initialise_static_expression(function_symbol_StateMust, atermpp::function_symbol("StateMust", 2));
  return function_symbol_StateMust;
}

inline
bool gsIsStateMust(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_StateMust();
}

// StateNot
inline
atermpp::function_symbol function_symbol_StateNot()
{
  static atermpp::function_symbol function_symbol_StateNot = core::detail::initialise_static_expression(function_symbol_StateNot, atermpp::function_symbol("StateNot", 1));
  return function_symbol_StateNot;
}

inline
bool gsIsStateNot(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_StateNot();
}

// StateNu
inline
atermpp::function_symbol function_symbol_StateNu()
{
  static atermpp::function_symbol function_symbol_StateNu = core::detail::initialise_static_expression(function_symbol_StateNu, atermpp::function_symbol("StateNu", 3));
  return function_symbol_StateNu;
}

inline
bool gsIsStateNu(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_StateNu();
}

// StateOr
inline
atermpp::function_symbol function_symbol_StateOr()
{
  static atermpp::function_symbol function_symbol_StateOr = core::detail::initialise_static_expression(function_symbol_StateOr, atermpp::function_symbol("StateOr", 2));
  return function_symbol_StateOr;
}

inline
bool gsIsStateOr(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_StateOr();
}

// StateTrue
inline
atermpp::function_symbol function_symbol_StateTrue()
{
  static atermpp::function_symbol function_symbol_StateTrue = core::detail::initialise_static_expression(function_symbol_StateTrue, atermpp::function_symbol("StateTrue", 0));
  return function_symbol_StateTrue;
}

inline
bool gsIsStateTrue(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_StateTrue();
}

// StateVar
inline
atermpp::function_symbol function_symbol_StateVar()
{
  static atermpp::function_symbol function_symbol_StateVar = core::detail::initialise_static_expression(function_symbol_StateVar, atermpp::function_symbol("StateVar", 2));
  return function_symbol_StateVar;
}

inline
bool gsIsStateVar(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_StateVar();
}

// StateYaled
inline
atermpp::function_symbol function_symbol_StateYaled()
{
  static atermpp::function_symbol function_symbol_StateYaled = core::detail::initialise_static_expression(function_symbol_StateYaled, atermpp::function_symbol("StateYaled", 0));
  return function_symbol_StateYaled;
}

inline
bool gsIsStateYaled(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_StateYaled();
}

// StateYaledTimed
inline
atermpp::function_symbol function_symbol_StateYaledTimed()
{
  static atermpp::function_symbol function_symbol_StateYaledTimed = core::detail::initialise_static_expression(function_symbol_StateYaledTimed, atermpp::function_symbol("StateYaledTimed", 1));
  return function_symbol_StateYaledTimed;
}

inline
bool gsIsStateYaledTimed(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_StateYaledTimed();
}

// StructCons
inline
atermpp::function_symbol function_symbol_StructCons()
{
  static atermpp::function_symbol function_symbol_StructCons = core::detail::initialise_static_expression(function_symbol_StructCons, atermpp::function_symbol("StructCons", 3));
  return function_symbol_StructCons;
}

inline
bool gsIsStructCons(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_StructCons();
}

// StructProj
inline
atermpp::function_symbol function_symbol_StructProj()
{
  static atermpp::function_symbol function_symbol_StructProj = core::detail::initialise_static_expression(function_symbol_StructProj, atermpp::function_symbol("StructProj", 2));
  return function_symbol_StructProj;
}

inline
bool gsIsStructProj(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_StructProj();
}

// Sum
inline
atermpp::function_symbol function_symbol_Sum()
{
  static atermpp::function_symbol function_symbol_Sum = core::detail::initialise_static_expression(function_symbol_Sum, atermpp::function_symbol("Sum", 2));
  return function_symbol_Sum;
}

inline
bool gsIsSum(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_Sum();
}

// Sync
inline
atermpp::function_symbol function_symbol_Sync()
{
  static atermpp::function_symbol function_symbol_Sync = core::detail::initialise_static_expression(function_symbol_Sync, atermpp::function_symbol("Sync", 2));
  return function_symbol_Sync;
}

inline
bool gsIsSync(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_Sync();
}

// Tau
inline
atermpp::function_symbol function_symbol_Tau()
{
  static atermpp::function_symbol function_symbol_Tau = core::detail::initialise_static_expression(function_symbol_Tau, atermpp::function_symbol("Tau", 0));
  return function_symbol_Tau;
}

inline
bool gsIsTau(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_Tau();
}

// Whr
inline
atermpp::function_symbol function_symbol_Whr()
{
  static atermpp::function_symbol function_symbol_Whr = core::detail::initialise_static_expression(function_symbol_Whr, atermpp::function_symbol("Whr", 2));
  return function_symbol_Whr;
}

inline
bool gsIsWhr(atermpp::aterm_appl Term)
{
  return ATgetAFun(Term) == function_symbol_Whr();
}

inline
ATermAppl gsMakeActAnd(ATermAppl ActFrm_0, ATermAppl ActFrm_1)
{
  return ATmakeAppl2(function_symbol_ActAnd(), static_cast_ATerm(ActFrm_0), static_cast_ATerm(ActFrm_1));
}

inline
ATermAppl gsMakeActAt(ATermAppl ActFrm_0, ATermAppl DataExpr_1)
{
  return ATmakeAppl2(function_symbol_ActAt(), static_cast_ATerm(ActFrm_0), static_cast_ATerm(DataExpr_1));
}

inline
ATermAppl gsMakeActExists(ATermList DataVarId_0, ATermAppl ActFrm_1)
{
  return ATmakeAppl2(function_symbol_ActExists(), static_cast_ATerm(DataVarId_0), static_cast_ATerm(ActFrm_1));
}

inline
ATermAppl gsMakeActFalse()
{
  return ATmakeAppl0(function_symbol_ActFalse());
}

inline
ATermAppl gsMakeActForall(ATermList DataVarId_0, ATermAppl ActFrm_1)
{
  return ATmakeAppl2(function_symbol_ActForall(), static_cast_ATerm(DataVarId_0), static_cast_ATerm(ActFrm_1));
}

inline
ATermAppl gsMakeActId(ATermAppl String_0, ATermList SortExpr_1)
{
  return ATmakeAppl2(function_symbol_ActId(), static_cast_ATerm(String_0), static_cast_ATerm(SortExpr_1));
}

inline
ATermAppl gsMakeActImp(ATermAppl ActFrm_0, ATermAppl ActFrm_1)
{
  return ATmakeAppl2(function_symbol_ActImp(), static_cast_ATerm(ActFrm_0), static_cast_ATerm(ActFrm_1));
}

inline
ATermAppl gsMakeActNot(ATermAppl ActFrm_0)
{
  return ATmakeAppl1(function_symbol_ActNot(), static_cast_ATerm(ActFrm_0));
}

inline
ATermAppl gsMakeActOr(ATermAppl ActFrm_0, ATermAppl ActFrm_1)
{
  return ATmakeAppl2(function_symbol_ActOr(), static_cast_ATerm(ActFrm_0), static_cast_ATerm(ActFrm_1));
}

inline
ATermAppl gsMakeActSpec(ATermList ActId_0)
{
  return ATmakeAppl1(function_symbol_ActSpec(), static_cast_ATerm(ActId_0));
}

inline
ATermAppl gsMakeActTrue()
{
  return ATmakeAppl0(function_symbol_ActTrue());
}

inline
ATermAppl gsMakeAction(ATermAppl ActId_0, ATermList DataExpr_1)
{
  return ATmakeAppl2(function_symbol_Action(), static_cast_ATerm(ActId_0), static_cast_ATerm(DataExpr_1));
}

inline
ATermAppl gsMakeActionRenameRule(ATermList DataVarId_0, ATermAppl DataExpr_1, ATermAppl ParamIdOrAction_2, ATermAppl ActionRenameRuleRHS_3)
{
  return ATmakeAppl4(function_symbol_ActionRenameRule(), static_cast_ATerm(DataVarId_0), static_cast_ATerm(DataExpr_1), static_cast_ATerm(ParamIdOrAction_2), static_cast_ATerm(ActionRenameRuleRHS_3));
}

inline
ATermAppl gsMakeActionRenameRules(ATermList ActionRenameRule_0)
{
  return ATmakeAppl1(function_symbol_ActionRenameRules(), static_cast_ATerm(ActionRenameRule_0));
}

inline
ATermAppl gsMakeActionRenameSpec(ATermAppl DataSpec_0, ATermAppl ActSpec_1, ATermAppl ActionRenameRules_2)
{
  return ATmakeAppl3(function_symbol_ActionRenameSpec(), static_cast_ATerm(DataSpec_0), static_cast_ATerm(ActSpec_1), static_cast_ATerm(ActionRenameRules_2));
}

inline
ATermAppl gsMakeAllow(ATermList MultActName_0, ATermAppl ProcExpr_1)
{
  assert(!ATisEmpty(MultActName_0));
  return ATmakeAppl2(function_symbol_Allow(), static_cast_ATerm(MultActName_0), static_cast_ATerm(ProcExpr_1));
}

inline
ATermAppl gsMakeAtTime(ATermAppl ProcExpr_0, ATermAppl DataExpr_1)
{
  return ATmakeAppl2(function_symbol_AtTime(), static_cast_ATerm(ProcExpr_0), static_cast_ATerm(DataExpr_1));
}

inline
ATermAppl gsMakeBES(ATermList BooleanEquation_0, ATermAppl BooleanExpression_1)
{
  return ATmakeAppl2(function_symbol_BES(), static_cast_ATerm(BooleanEquation_0), static_cast_ATerm(BooleanExpression_1));
}

inline
ATermAppl gsMakeBInit(ATermAppl ProcExpr_0, ATermAppl ProcExpr_1)
{
  return ATmakeAppl2(function_symbol_BInit(), static_cast_ATerm(ProcExpr_0), static_cast_ATerm(ProcExpr_1));
}

inline
ATermAppl gsMakeBagComp()
{
  return ATmakeAppl0(function_symbol_BagComp());
}

inline
ATermAppl gsMakeBinder(ATermAppl BindingOperator_0, ATermList DataVarId_1, ATermAppl DataExpr_2)
{
  return ATmakeAppl3(function_symbol_Binder(), static_cast_ATerm(BindingOperator_0), static_cast_ATerm(DataVarId_1), static_cast_ATerm(DataExpr_2));
}

inline
ATermAppl gsMakeBlock(ATermList String_0, ATermAppl ProcExpr_1)
{
  return ATmakeAppl2(function_symbol_Block(), static_cast_ATerm(String_0), static_cast_ATerm(ProcExpr_1));
}

inline
ATermAppl gsMakeBooleanAnd(ATermAppl BooleanExpression_0, ATermAppl BooleanExpression_1)
{
  return ATmakeAppl2(function_symbol_BooleanAnd(), static_cast_ATerm(BooleanExpression_0), static_cast_ATerm(BooleanExpression_1));
}

inline
ATermAppl gsMakeBooleanEquation(ATermAppl FixPoint_0, ATermAppl BooleanVariable_1, ATermAppl BooleanExpression_2)
{
  return ATmakeAppl3(function_symbol_BooleanEquation(), static_cast_ATerm(FixPoint_0), static_cast_ATerm(BooleanVariable_1), static_cast_ATerm(BooleanExpression_2));
}

inline
ATermAppl gsMakeBooleanFalse()
{
  return ATmakeAppl0(function_symbol_BooleanFalse());
}

inline
ATermAppl gsMakeBooleanImp(ATermAppl BooleanExpression_0, ATermAppl BooleanExpression_1)
{
  return ATmakeAppl2(function_symbol_BooleanImp(), static_cast_ATerm(BooleanExpression_0), static_cast_ATerm(BooleanExpression_1));
}

inline
ATermAppl gsMakeBooleanNot(ATermAppl BooleanExpression_0)
{
  return ATmakeAppl1(function_symbol_BooleanNot(), static_cast_ATerm(BooleanExpression_0));
}

inline
ATermAppl gsMakeBooleanOr(ATermAppl BooleanExpression_0, ATermAppl BooleanExpression_1)
{
  return ATmakeAppl2(function_symbol_BooleanOr(), static_cast_ATerm(BooleanExpression_0), static_cast_ATerm(BooleanExpression_1));
}

inline
ATermAppl gsMakeBooleanTrue()
{
  return ATmakeAppl0(function_symbol_BooleanTrue());
}

inline
ATermAppl gsMakeBooleanVariable(ATermAppl String_0)
{
  return ATmakeAppl1(function_symbol_BooleanVariable(), static_cast_ATerm(String_0));
}

inline
ATermAppl gsMakeChoice(ATermAppl ProcExpr_0, ATermAppl ProcExpr_1)
{
  return ATmakeAppl2(function_symbol_Choice(), static_cast_ATerm(ProcExpr_0), static_cast_ATerm(ProcExpr_1));
}

inline
ATermAppl gsMakeComm(ATermList CommExpr_0, ATermAppl ProcExpr_1)
{
  return ATmakeAppl2(function_symbol_Comm(), static_cast_ATerm(CommExpr_0), static_cast_ATerm(ProcExpr_1));
}

inline
ATermAppl gsMakeCommExpr(ATermAppl MultActName_0, ATermAppl StringOrNil_1)
{
  return ATmakeAppl2(function_symbol_CommExpr(), static_cast_ATerm(MultActName_0), static_cast_ATerm(StringOrNil_1));
}

inline
ATermAppl gsMakeConsSpec(ATermList OpId_0)
{
  return ATmakeAppl1(function_symbol_ConsSpec(), static_cast_ATerm(OpId_0));
}

inline
ATermAppl gsMakeDataAppl(ATermAppl DataExpr_0, ATermList DataExpr_1)
{
  return ATmakeAppl2(function_symbol_DataAppl(), static_cast_ATerm(DataExpr_0), static_cast_ATerm(DataExpr_1));
}

inline
ATermAppl gsMakeDataEqn(ATermList DataVarId_0, ATermAppl DataExpr_1, ATermAppl DataExpr_2, ATermAppl DataExpr_3)
{
  return ATmakeAppl4(function_symbol_DataEqn(), static_cast_ATerm(DataVarId_0), static_cast_ATerm(DataExpr_1), static_cast_ATerm(DataExpr_2), static_cast_ATerm(DataExpr_3));
}

inline
ATermAppl gsMakeDataEqnSpec(ATermList DataEqn_0)
{
  return ATmakeAppl1(function_symbol_DataEqnSpec(), static_cast_ATerm(DataEqn_0));
}

inline
ATermAppl gsMakeDataSpec(ATermAppl SortSpec_0, ATermAppl ConsSpec_1, ATermAppl MapSpec_2, ATermAppl DataEqnSpec_3)
{
  return ATmakeAppl4(function_symbol_DataSpec(), static_cast_ATerm(SortSpec_0), static_cast_ATerm(ConsSpec_1), static_cast_ATerm(MapSpec_2), static_cast_ATerm(DataEqnSpec_3));
}

inline
ATermAppl gsMakeDataVarId(ATermAppl String_0, ATermAppl SortExpr_1)
{
  return ATmakeAppl2(function_symbol_DataVarId(), static_cast_ATerm(String_0), static_cast_ATerm(SortExpr_1));
}

inline
ATermAppl gsMakeDataVarIdInit(ATermAppl DataVarId_0, ATermAppl DataExpr_1)
{
  return ATmakeAppl2(function_symbol_DataVarIdInit(), static_cast_ATerm(DataVarId_0), static_cast_ATerm(DataExpr_1));
}

inline
ATermAppl gsMakeDelta()
{
  return ATmakeAppl0(function_symbol_Delta());
}

inline
ATermAppl gsMakeExists()
{
  return ATmakeAppl0(function_symbol_Exists());
}

inline
ATermAppl gsMakeForall()
{
  return ATmakeAppl0(function_symbol_Forall());
}

inline
ATermAppl gsMakeGlobVarSpec(ATermList DataVarId_0)
{
  return ATmakeAppl1(function_symbol_GlobVarSpec(), static_cast_ATerm(DataVarId_0));
}

inline
ATermAppl gsMakeHide(ATermList String_0, ATermAppl ProcExpr_1)
{
  return ATmakeAppl2(function_symbol_Hide(), static_cast_ATerm(String_0), static_cast_ATerm(ProcExpr_1));
}

inline
ATermAppl gsMakeId(ATermAppl String_0)
{
  return ATmakeAppl1(function_symbol_Id(), static_cast_ATerm(String_0));
}

inline
ATermAppl gsMakeIdAssignment(ATermAppl String_0, ATermList IdInit_1)
{
  return ATmakeAppl2(function_symbol_IdAssignment(), static_cast_ATerm(String_0), static_cast_ATerm(IdInit_1));
}

inline
ATermAppl gsMakeIdInit(ATermAppl String_0, ATermAppl DataExpr_1)
{
  return ATmakeAppl2(function_symbol_IdInit(), static_cast_ATerm(String_0), static_cast_ATerm(DataExpr_1));
}

inline
ATermAppl gsMakeIfThen(ATermAppl DataExpr_0, ATermAppl ProcExpr_1)
{
  return ATmakeAppl2(function_symbol_IfThen(), static_cast_ATerm(DataExpr_0), static_cast_ATerm(ProcExpr_1));
}

inline
ATermAppl gsMakeIfThenElse(ATermAppl DataExpr_0, ATermAppl ProcExpr_1, ATermAppl ProcExpr_2)
{
  return ATmakeAppl3(function_symbol_IfThenElse(), static_cast_ATerm(DataExpr_0), static_cast_ATerm(ProcExpr_1), static_cast_ATerm(ProcExpr_2));
}

inline
ATermAppl gsMakeLMerge(ATermAppl ProcExpr_0, ATermAppl ProcExpr_1)
{
  return ATmakeAppl2(function_symbol_LMerge(), static_cast_ATerm(ProcExpr_0), static_cast_ATerm(ProcExpr_1));
}

inline
ATermAppl gsMakeLambda()
{
  return ATmakeAppl0(function_symbol_Lambda());
}

inline
ATermAppl gsMakeLinProcSpec(ATermAppl DataSpec_0, ATermAppl ActSpec_1, ATermAppl GlobVarSpec_2, ATermAppl LinearProcess_3, ATermAppl LinearProcessInit_4)
{
  return ATmakeAppl5(function_symbol_LinProcSpec(), static_cast_ATerm(DataSpec_0), static_cast_ATerm(ActSpec_1), static_cast_ATerm(GlobVarSpec_2), static_cast_ATerm(LinearProcess_3), static_cast_ATerm(LinearProcessInit_4));
}

inline
ATermAppl gsMakeLinearProcess(ATermList DataVarId_0, ATermList LinearProcessSummand_1)
{
  return ATmakeAppl2(function_symbol_LinearProcess(), static_cast_ATerm(DataVarId_0), static_cast_ATerm(LinearProcessSummand_1));
}

inline
ATermAppl gsMakeLinearProcessInit(ATermList DataVarIdInit_0)
{
  return ATmakeAppl1(function_symbol_LinearProcessInit(), static_cast_ATerm(DataVarIdInit_0));
}

inline
ATermAppl gsMakeLinearProcessSummand(ATermList DataVarId_0, ATermAppl DataExpr_1, ATermAppl MultActOrDelta_2, ATermAppl DataExprOrNil_3, ATermList DataVarIdInit_4)
{
  return ATmakeAppl5(function_symbol_LinearProcessSummand(), static_cast_ATerm(DataVarId_0), static_cast_ATerm(DataExpr_1), static_cast_ATerm(MultActOrDelta_2), static_cast_ATerm(DataExprOrNil_3), static_cast_ATerm(DataVarIdInit_4));
}

inline
ATermAppl gsMakeMapSpec(ATermList OpId_0)
{
  return ATmakeAppl1(function_symbol_MapSpec(), static_cast_ATerm(OpId_0));
}

inline
ATermAppl gsMakeMerge(ATermAppl ProcExpr_0, ATermAppl ProcExpr_1)
{
  return ATmakeAppl2(function_symbol_Merge(), static_cast_ATerm(ProcExpr_0), static_cast_ATerm(ProcExpr_1));
}

inline
ATermAppl gsMakeMu()
{
  return ATmakeAppl0(function_symbol_Mu());
}

inline
ATermAppl gsMakeMultAct(ATermList ParamIdOrAction_0)
{
  return ATmakeAppl1(function_symbol_MultAct(), static_cast_ATerm(ParamIdOrAction_0));
}

inline
ATermAppl gsMakeMultActName(ATermList String_0)
{
  return ATmakeAppl1(function_symbol_MultActName(), static_cast_ATerm(String_0));
}

inline
ATermAppl gsMakeNil()
{
  return ATmakeAppl0(function_symbol_Nil());
}

inline
ATermAppl gsMakeNu()
{
  return ATmakeAppl0(function_symbol_Nu());
}

inline
ATermAppl gsMakeOpId(ATermAppl String_0, ATermAppl SortExpr_1)
{
  return ATmakeAppl2(function_symbol_OpId(), static_cast_ATerm(String_0), static_cast_ATerm(SortExpr_1));
}

inline
ATermAppl gsMakePBES(ATermAppl DataSpec_0, ATermAppl GlobVarSpec_1, ATermAppl PBEqnSpec_2, ATermAppl PBInit_3)
{
  return ATmakeAppl4(function_symbol_PBES(), static_cast_ATerm(DataSpec_0), static_cast_ATerm(GlobVarSpec_1), static_cast_ATerm(PBEqnSpec_2), static_cast_ATerm(PBInit_3));
}

inline
ATermAppl gsMakePBESAnd(ATermAppl PBExpr_0, ATermAppl PBExpr_1)
{
  return ATmakeAppl2(function_symbol_PBESAnd(), static_cast_ATerm(PBExpr_0), static_cast_ATerm(PBExpr_1));
}

inline
ATermAppl gsMakePBESExists(ATermList DataVarId_0, ATermAppl PBExpr_1)
{
  return ATmakeAppl2(function_symbol_PBESExists(), static_cast_ATerm(DataVarId_0), static_cast_ATerm(PBExpr_1));
}

inline
ATermAppl gsMakePBESFalse()
{
  return ATmakeAppl0(function_symbol_PBESFalse());
}

inline
ATermAppl gsMakePBESForall(ATermList DataVarId_0, ATermAppl PBExpr_1)
{
  return ATmakeAppl2(function_symbol_PBESForall(), static_cast_ATerm(DataVarId_0), static_cast_ATerm(PBExpr_1));
}

inline
ATermAppl gsMakePBESImp(ATermAppl PBExpr_0, ATermAppl PBExpr_1)
{
  return ATmakeAppl2(function_symbol_PBESImp(), static_cast_ATerm(PBExpr_0), static_cast_ATerm(PBExpr_1));
}

inline
ATermAppl gsMakePBESNot(ATermAppl PBExpr_0)
{
  return ATmakeAppl1(function_symbol_PBESNot(), static_cast_ATerm(PBExpr_0));
}

inline
ATermAppl gsMakePBESOr(ATermAppl PBExpr_0, ATermAppl PBExpr_1)
{
  return ATmakeAppl2(function_symbol_PBESOr(), static_cast_ATerm(PBExpr_0), static_cast_ATerm(PBExpr_1));
}

inline
ATermAppl gsMakePBESTrue()
{
  return ATmakeAppl0(function_symbol_PBESTrue());
}

inline
ATermAppl gsMakePBEqn(ATermAppl FixPoint_0, ATermAppl PropVarDecl_1, ATermAppl PBExpr_2)
{
  return ATmakeAppl3(function_symbol_PBEqn(), static_cast_ATerm(FixPoint_0), static_cast_ATerm(PropVarDecl_1), static_cast_ATerm(PBExpr_2));
}

inline
ATermAppl gsMakePBEqnSpec(ATermList PBEqn_0)
{
  return ATmakeAppl1(function_symbol_PBEqnSpec(), static_cast_ATerm(PBEqn_0));
}

inline
ATermAppl gsMakePBInit(ATermAppl PropVarInst_0)
{
  return ATmakeAppl1(function_symbol_PBInit(), static_cast_ATerm(PropVarInst_0));
}

inline
ATermAppl gsMakeParamId(ATermAppl String_0, ATermList DataExpr_1)
{
  return ATmakeAppl2(function_symbol_ParamId(), static_cast_ATerm(String_0), static_cast_ATerm(DataExpr_1));
}

inline
ATermAppl gsMakeProcEqn(ATermAppl ProcVarId_0, ATermList DataVarId_1, ATermAppl ProcExpr_2)
{
  return ATmakeAppl3(function_symbol_ProcEqn(), static_cast_ATerm(ProcVarId_0), static_cast_ATerm(DataVarId_1), static_cast_ATerm(ProcExpr_2));
}

inline
ATermAppl gsMakeProcEqnSpec(ATermList ProcEqn_0)
{
  return ATmakeAppl1(function_symbol_ProcEqnSpec(), static_cast_ATerm(ProcEqn_0));
}

inline
ATermAppl gsMakeProcSpec(ATermAppl DataSpec_0, ATermAppl ActSpec_1, ATermAppl GlobVarSpec_2, ATermAppl ProcEqnSpec_3, ATermAppl ProcInit_4)
{
  return ATmakeAppl5(function_symbol_ProcSpec(), static_cast_ATerm(DataSpec_0), static_cast_ATerm(ActSpec_1), static_cast_ATerm(GlobVarSpec_2), static_cast_ATerm(ProcEqnSpec_3), static_cast_ATerm(ProcInit_4));
}

inline
ATermAppl gsMakeProcVarId(ATermAppl String_0, ATermList SortExpr_1)
{
  return ATmakeAppl2(function_symbol_ProcVarId(), static_cast_ATerm(String_0), static_cast_ATerm(SortExpr_1));
}

inline
ATermAppl gsMakeProcess(ATermAppl ProcVarId_0, ATermList DataExpr_1)
{
  return ATmakeAppl2(function_symbol_Process(), static_cast_ATerm(ProcVarId_0), static_cast_ATerm(DataExpr_1));
}

inline
ATermAppl gsMakeProcessAssignment(ATermAppl ProcVarId_0, ATermList DataVarIdInit_1)
{
  return ATmakeAppl2(function_symbol_ProcessAssignment(), static_cast_ATerm(ProcVarId_0), static_cast_ATerm(DataVarIdInit_1));
}

inline
ATermAppl gsMakeProcessInit(ATermAppl ProcExpr_0)
{
  return ATmakeAppl1(function_symbol_ProcessInit(), static_cast_ATerm(ProcExpr_0));
}

inline
ATermAppl gsMakePropVarDecl(ATermAppl String_0, ATermList DataVarId_1)
{
  return ATmakeAppl2(function_symbol_PropVarDecl(), static_cast_ATerm(String_0), static_cast_ATerm(DataVarId_1));
}

inline
ATermAppl gsMakePropVarInst(ATermAppl String_0, ATermList DataExpr_1)
{
  return ATmakeAppl2(function_symbol_PropVarInst(), static_cast_ATerm(String_0), static_cast_ATerm(DataExpr_1));
}

inline
ATermAppl gsMakeRegAlt(ATermAppl RegFrm_0, ATermAppl RegFrm_1)
{
  return ATmakeAppl2(function_symbol_RegAlt(), static_cast_ATerm(RegFrm_0), static_cast_ATerm(RegFrm_1));
}

inline
ATermAppl gsMakeRegNil()
{
  return ATmakeAppl0(function_symbol_RegNil());
}

inline
ATermAppl gsMakeRegSeq(ATermAppl RegFrm_0, ATermAppl RegFrm_1)
{
  return ATmakeAppl2(function_symbol_RegSeq(), static_cast_ATerm(RegFrm_0), static_cast_ATerm(RegFrm_1));
}

inline
ATermAppl gsMakeRegTrans(ATermAppl RegFrm_0)
{
  return ATmakeAppl1(function_symbol_RegTrans(), static_cast_ATerm(RegFrm_0));
}

inline
ATermAppl gsMakeRegTransOrNil(ATermAppl RegFrm_0)
{
  return ATmakeAppl1(function_symbol_RegTransOrNil(), static_cast_ATerm(RegFrm_0));
}

inline
ATermAppl gsMakeRename(ATermList RenameExpr_0, ATermAppl ProcExpr_1)
{
  return ATmakeAppl2(function_symbol_Rename(), static_cast_ATerm(RenameExpr_0), static_cast_ATerm(ProcExpr_1));
}

inline
ATermAppl gsMakeRenameExpr(ATermAppl String_0, ATermAppl String_1)
{
  return ATmakeAppl2(function_symbol_RenameExpr(), static_cast_ATerm(String_0), static_cast_ATerm(String_1));
}

inline
ATermAppl gsMakeSeq(ATermAppl ProcExpr_0, ATermAppl ProcExpr_1)
{
  return ATmakeAppl2(function_symbol_Seq(), static_cast_ATerm(ProcExpr_0), static_cast_ATerm(ProcExpr_1));
}

inline
ATermAppl gsMakeSetBagComp()
{
  return ATmakeAppl0(function_symbol_SetBagComp());
}

inline
ATermAppl gsMakeSetComp()
{
  return ATmakeAppl0(function_symbol_SetComp());
}

inline
ATermAppl gsMakeSortArrow(ATermList SortExpr_0, ATermAppl SortExpr_1)
{
  return ATmakeAppl2(function_symbol_SortArrow(), static_cast_ATerm(SortExpr_0), static_cast_ATerm(SortExpr_1));
}

inline
ATermAppl gsMakeSortBag()
{
  return ATmakeAppl0(function_symbol_SortBag());
}

inline
ATermAppl gsMakeSortCons(ATermAppl SortConsType_0, ATermAppl SortExpr_1)
{
  return ATmakeAppl2(function_symbol_SortCons(), static_cast_ATerm(SortConsType_0), static_cast_ATerm(SortExpr_1));
}

inline
ATermAppl gsMakeSortFBag()
{
  return ATmakeAppl0(function_symbol_SortFBag());
}

inline
ATermAppl gsMakeSortFSet()
{
  return ATmakeAppl0(function_symbol_SortFSet());
}

inline
ATermAppl gsMakeSortId(ATermAppl String_0)
{
  return ATmakeAppl1(function_symbol_SortId(), static_cast_ATerm(String_0));
}

inline
ATermAppl gsMakeSortList()
{
  return ATmakeAppl0(function_symbol_SortList());
}

inline
ATermAppl gsMakeSortRef(ATermAppl String_0, ATermAppl SortExpr_1)
{
  return ATmakeAppl2(function_symbol_SortRef(), static_cast_ATerm(String_0), static_cast_ATerm(SortExpr_1));
}

inline
ATermAppl gsMakeSortSet()
{
  return ATmakeAppl0(function_symbol_SortSet());
}

inline
ATermAppl gsMakeSortSpec(ATermList SortDecl_0)
{
  return ATmakeAppl1(function_symbol_SortSpec(), static_cast_ATerm(SortDecl_0));
}

inline
ATermAppl gsMakeSortStruct(ATermList StructCons_0)
{
  return ATmakeAppl1(function_symbol_SortStruct(), static_cast_ATerm(StructCons_0));
}

inline
ATermAppl gsMakeSortUnknown()
{
  return ATmakeAppl0(function_symbol_SortUnknown());
}

inline
ATermAppl gsMakeSortsPossible(ATermList SortExpr_0)
{
  return ATmakeAppl1(function_symbol_SortsPossible(), static_cast_ATerm(SortExpr_0));
}

inline
ATermAppl gsMakeStateAnd(ATermAppl StateFrm_0, ATermAppl StateFrm_1)
{
  return ATmakeAppl2(function_symbol_StateAnd(), static_cast_ATerm(StateFrm_0), static_cast_ATerm(StateFrm_1));
}

inline
ATermAppl gsMakeStateDelay()
{
  return ATmakeAppl0(function_symbol_StateDelay());
}

inline
ATermAppl gsMakeStateDelayTimed(ATermAppl DataExpr_0)
{
  return ATmakeAppl1(function_symbol_StateDelayTimed(), static_cast_ATerm(DataExpr_0));
}

inline
ATermAppl gsMakeStateExists(ATermList DataVarId_0, ATermAppl StateFrm_1)
{
  return ATmakeAppl2(function_symbol_StateExists(), static_cast_ATerm(DataVarId_0), static_cast_ATerm(StateFrm_1));
}

inline
ATermAppl gsMakeStateFalse()
{
  return ATmakeAppl0(function_symbol_StateFalse());
}

inline
ATermAppl gsMakeStateForall(ATermList DataVarId_0, ATermAppl StateFrm_1)
{
  return ATmakeAppl2(function_symbol_StateForall(), static_cast_ATerm(DataVarId_0), static_cast_ATerm(StateFrm_1));
}

inline
ATermAppl gsMakeStateImp(ATermAppl StateFrm_0, ATermAppl StateFrm_1)
{
  return ATmakeAppl2(function_symbol_StateImp(), static_cast_ATerm(StateFrm_0), static_cast_ATerm(StateFrm_1));
}

inline
ATermAppl gsMakeStateMay(ATermAppl RegFrm_0, ATermAppl StateFrm_1)
{
  return ATmakeAppl2(function_symbol_StateMay(), static_cast_ATerm(RegFrm_0), static_cast_ATerm(StateFrm_1));
}

inline
ATermAppl gsMakeStateMu(ATermAppl String_0, ATermList DataVarIdInit_1, ATermAppl StateFrm_2)
{
  return ATmakeAppl3(function_symbol_StateMu(), static_cast_ATerm(String_0), static_cast_ATerm(DataVarIdInit_1), static_cast_ATerm(StateFrm_2));
}

inline
ATermAppl gsMakeStateMust(ATermAppl RegFrm_0, ATermAppl StateFrm_1)
{
  return ATmakeAppl2(function_symbol_StateMust(), static_cast_ATerm(RegFrm_0), static_cast_ATerm(StateFrm_1));
}

inline
ATermAppl gsMakeStateNot(ATermAppl StateFrm_0)
{
  return ATmakeAppl1(function_symbol_StateNot(), static_cast_ATerm(StateFrm_0));
}

inline
ATermAppl gsMakeStateNu(ATermAppl String_0, ATermList DataVarIdInit_1, ATermAppl StateFrm_2)
{
  return ATmakeAppl3(function_symbol_StateNu(), static_cast_ATerm(String_0), static_cast_ATerm(DataVarIdInit_1), static_cast_ATerm(StateFrm_2));
}

inline
ATermAppl gsMakeStateOr(ATermAppl StateFrm_0, ATermAppl StateFrm_1)
{
  return ATmakeAppl2(function_symbol_StateOr(), static_cast_ATerm(StateFrm_0), static_cast_ATerm(StateFrm_1));
}

inline
ATermAppl gsMakeStateTrue()
{
  return ATmakeAppl0(function_symbol_StateTrue());
}

inline
ATermAppl gsMakeStateVar(ATermAppl String_0, ATermList DataExpr_1)
{
  return ATmakeAppl2(function_symbol_StateVar(), static_cast_ATerm(String_0), static_cast_ATerm(DataExpr_1));
}

inline
ATermAppl gsMakeStateYaled()
{
  return ATmakeAppl0(function_symbol_StateYaled());
}

inline
ATermAppl gsMakeStateYaledTimed(ATermAppl DataExpr_0)
{
  return ATmakeAppl1(function_symbol_StateYaledTimed(), static_cast_ATerm(DataExpr_0));
}

inline
ATermAppl gsMakeStructCons(ATermAppl String_0, ATermList StructProj_1, ATermAppl StringOrNil_2)
{
  return ATmakeAppl3(function_symbol_StructCons(), static_cast_ATerm(String_0), static_cast_ATerm(StructProj_1), static_cast_ATerm(StringOrNil_2));
}

inline
ATermAppl gsMakeStructProj(ATermAppl StringOrNil_0, ATermAppl SortExpr_1)
{
  return ATmakeAppl2(function_symbol_StructProj(), static_cast_ATerm(StringOrNil_0), static_cast_ATerm(SortExpr_1));
}

inline
ATermAppl gsMakeSum(ATermList DataVarId_0, ATermAppl ProcExpr_1)
{
  return ATmakeAppl2(function_symbol_Sum(), static_cast_ATerm(DataVarId_0), static_cast_ATerm(ProcExpr_1));
}

inline
ATermAppl gsMakeSync(ATermAppl ProcExpr_0, ATermAppl ProcExpr_1)
{
  return ATmakeAppl2(function_symbol_Sync(), static_cast_ATerm(ProcExpr_0), static_cast_ATerm(ProcExpr_1));
}

inline
ATermAppl gsMakeTau()
{
  return ATmakeAppl0(function_symbol_Tau());
}

inline
ATermAppl gsMakeWhr(ATermAppl DataExpr_0, ATermList WhrDecl_1)
{
  return ATmakeAppl2(function_symbol_Whr(), static_cast_ATerm(DataExpr_0), static_cast_ATerm(WhrDecl_1));
}
//--- end generated code ---//

//Conversion between strings and quoted ATermAppl's
//-------------------------------------------------

inline
ATermAppl gsString2ATermAppl(const char* s)
//Ret: quoted constant s, if s != NULL
//     unquoted constant Nil, if s == NULL
{
  if (s != NULL)
  {
    return ATmakeAppl0(ATmakeAFun(s, 0, true));
  }
  else
  {
    return gsMakeNil();
  }
}

inline
bool gsIsString(ATermAppl term)
//Ret: term is a quoted constant
{
  AFun head = ATgetAFun(term);
  return ((ATgetArity(head) == 0) && (ATisQuoted(head) == true));
}

inline
char* gsATermAppl2String(ATermAppl term)
//Ret: string s, if term is a quoted constant s
//     NULL, otherwise
{
  if (gsIsString(term))
  {
    return ATgetName(ATgetAFun(term));
  }
  else
  {
    return NULL;
  }
}

inline
bool gsIsNumericString(const char* s)
//Ret: true if s is of form "0 | -? [1-9][0-9]*", false otherwise
{
  if (s == NULL)
  {
    return false;
  }
  if (s[0] == '\0')
  {
    return false;
  }
  if (s[0] == '-')
  {
    ++s;
  }
  if (s[0] == '\0')
  {
    return false;
  }
  if (s[0] == '0')
  {
    ++s;
    if (s[0] == '\0')
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  for (; s[0] != '\0'; ++s)
    if (!isdigit(s[0]))
    {
      return false;
    }
  return true;
}

ATermAppl gsFreshString2ATermAppl(const char* s, ATerm Term, bool TryNoSuffix);
//Pre: Term is an ATerm containing ATermAppl's and ATermList's only
//     s is not NULL
//Ret: "s", if it does not occur in Term, and TryNoSuffix holds
//     "sk" as a quoted ATermAppl constant, where k is the smallest natural
//     number such that "sk" does not occur in Term, otherwise

// ----------------- gsIsDataExpr and gsIsSortExpr ---------------------- //

///\pre Term is not NULL
///\return Term is a sort expression
inline
bool gsIsSortExpr(ATermAppl Term)
{
  return
    gsIsSortId(Term)        || gsIsSortCons(Term)     ||
    gsIsSortStruct(Term)    || gsIsSortArrow(Term) ||
    gsIsSortUnknown(Term)   || gsIsSortsPossible(Term);
}

///\pre Term is not NULL
///\return Term is a data expression
inline
bool gsIsDataExpr(ATermAppl Term)
{
  return gsIsId(Term)    || gsIsDataVarId(Term)    || gsIsOpId(Term)    ||
         gsIsDataAppl(Term) || gsIsBinder(Term)     || gsIsWhr(Term);
}

///\return Term is a state formula
inline
bool gsIsStateFrm(ATermAppl Term)
{
  return gsIsDataExpr(Term)
         || gsIsStateTrue(Term)
         || gsIsStateFalse(Term)
         || gsIsStateNot(Term)
         || gsIsStateAnd(Term)
         || gsIsStateOr(Term)
         || gsIsStateImp(Term)
         || gsIsStateForall(Term)
         || gsIsStateExists(Term)
         || gsIsStateMust(Term)
         || gsIsStateMay(Term)
         || gsIsStateYaled(Term)
         || gsIsStateYaledTimed(Term)
         || gsIsStateDelay(Term)
         || gsIsStateDelayTimed(Term)
         || gsIsStateVar(Term)
         || gsIsStateNu(Term)
         || gsIsStateMu(Term);
}

///\pre Term is not NULL
///\return Term is a action formula
inline
bool gsIsActFrm(ATermAppl Term)
{
  return
    gsIsMultAct(Term) || gsIsDataExpr(Term) || gsIsActTrue(Term) ||
    gsIsActFalse(Term) || gsIsActNot(Term) || gsIsActAnd(Term) ||
    gsIsActOr(Term) || gsIsActImp(Term) || gsIsActForall(Term) ||
    gsIsActExists(Term) || gsIsActAt(Term);
}

///\pre Term is not NULL
///\return Term is a regular formula
inline
bool gsIsRegFrm(ATermAppl Term)
{
  return
    gsIsActFrm(Term) || gsIsRegNil(Term) || gsIsRegSeq(Term) ||
    gsIsRegAlt(Term) || gsIsRegTrans(Term) || gsIsRegTransOrNil(Term);
}

///\pre Term is not NULL
///\return Term is a process expression
inline
bool gsIsProcExpr(ATermAppl Term)
{
  return gsIsParamId(Term)
         || gsIsIdAssignment(Term)
         || gsIsAction(Term)
         || gsIsProcess(Term)
         || gsIsProcessAssignment(Term)
         || gsIsDelta(Term)
         || gsIsTau(Term)
         || gsIsSum(Term)
         || gsIsBlock(Term)
         || gsIsHide(Term)
         || gsIsRename(Term)
         || gsIsComm(Term)
         || gsIsAllow(Term)
         || gsIsSync(Term)
         || gsIsAtTime(Term)
         || gsIsSeq(Term)
         || gsIsIfThen(Term)
         || gsIsIfThenElse(Term)
         || gsIsBInit(Term)
         || gsIsMerge(Term)
         || gsIsLMerge(Term)
         || gsIsChoice(Term);
}

// PBES's
// ------

///\pre Term is not NULL
///\return Term is a Parameterised Boolean Expression
inline
bool gsIsPBExpr(ATermAppl Term)
{
  return gsIsDataExpr(Term)
         || gsIsPBESTrue(Term)
         || gsIsPBESFalse(Term)
         || gsIsPBESNot(Term)
         || gsIsPBESAnd(Term)
         || gsIsPBESOr(Term)
         || gsIsPBESImp(Term)
         || gsIsPBESForall(Term)
         || gsIsPBESExists(Term)
         || gsIsPropVarInst(Term)
         ;
}

///\pre Term is not NULL
///\return Term is a fixpoint
inline
bool gsIsFixpoint(ATermAppl Term)
{
  return gsIsMu(Term) || gsIsNu(Term);
}

}
}
}

#endif // MCRL2_LIBSTRUCT_CORE_H
