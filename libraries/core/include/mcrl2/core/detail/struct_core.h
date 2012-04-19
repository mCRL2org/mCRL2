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
const atermpp::function_symbol& function_symbol_ActAnd()
{
//  static atermpp::function_symbol function_symbol_ActAnd = core::detail::initialise_static_expression(function_symbol_ActAnd, atermpp::function_symbol("ActAnd", 2));
  static atermpp::function_symbol function_symbol_ActAnd = atermpp::function_symbol("ActAnd", 2);
  return function_symbol_ActAnd;
}

inline
bool gsIsActAnd(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_ActAnd();
}

// ActAt
inline
const atermpp::function_symbol& function_symbol_ActAt()
{
//  static atermpp::function_symbol function_symbol_ActAt = core::detail::initialise_static_expression(function_symbol_ActAt, atermpp::function_symbol("ActAt", 2));
  static atermpp::function_symbol function_symbol_ActAt = atermpp::function_symbol("ActAt", 2);
  return function_symbol_ActAt;
}

inline
bool gsIsActAt(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_ActAt();
}

// ActExists
inline
const atermpp::function_symbol& function_symbol_ActExists()
{
//  static atermpp::function_symbol function_symbol_ActExists = core::detail::initialise_static_expression(function_symbol_ActExists, atermpp::function_symbol("ActExists", 2));
  static atermpp::function_symbol function_symbol_ActExists = atermpp::function_symbol("ActExists", 2);
  return function_symbol_ActExists;
}

inline
bool gsIsActExists(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_ActExists();
}

// ActFalse
inline
const atermpp::function_symbol& function_symbol_ActFalse()
{
//  static atermpp::function_symbol function_symbol_ActFalse = core::detail::initialise_static_expression(function_symbol_ActFalse, atermpp::function_symbol("ActFalse", 0));
  static atermpp::function_symbol function_symbol_ActFalse = atermpp::function_symbol("ActFalse", 0);
  return function_symbol_ActFalse;
}

inline
bool gsIsActFalse(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_ActFalse();
}

// ActForall
inline
const atermpp::function_symbol& function_symbol_ActForall()
{
//  static atermpp::function_symbol function_symbol_ActForall = core::detail::initialise_static_expression(function_symbol_ActForall, atermpp::function_symbol("ActForall", 2));
  static atermpp::function_symbol function_symbol_ActForall = atermpp::function_symbol("ActForall", 2);
  return function_symbol_ActForall;
}

inline
bool gsIsActForall(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_ActForall();
}

// ActId
inline
const atermpp::function_symbol& function_symbol_ActId()
{
//  static atermpp::function_symbol function_symbol_ActId = core::detail::initialise_static_expression(function_symbol_ActId, atermpp::function_symbol("ActId", 2));
  static atermpp::function_symbol function_symbol_ActId = atermpp::function_symbol("ActId", 2);
  return function_symbol_ActId;
}

inline
bool gsIsActId(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_ActId();
}

// ActImp
inline
const atermpp::function_symbol& function_symbol_ActImp()
{
//  static atermpp::function_symbol function_symbol_ActImp = core::detail::initialise_static_expression(function_symbol_ActImp, atermpp::function_symbol("ActImp", 2));
  static atermpp::function_symbol function_symbol_ActImp = atermpp::function_symbol("ActImp", 2);
  return function_symbol_ActImp;
}

inline
bool gsIsActImp(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_ActImp();
}

// ActNot
inline
const atermpp::function_symbol& function_symbol_ActNot()
{
//  static atermpp::function_symbol function_symbol_ActNot = core::detail::initialise_static_expression(function_symbol_ActNot, atermpp::function_symbol("ActNot", 1));
  static atermpp::function_symbol function_symbol_ActNot = atermpp::function_symbol("ActNot", 1);
  return function_symbol_ActNot;
}

inline
bool gsIsActNot(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_ActNot();
}

// ActOr
inline
const atermpp::function_symbol& function_symbol_ActOr()
{
//  static atermpp::function_symbol function_symbol_ActOr = core::detail::initialise_static_expression(function_symbol_ActOr, atermpp::function_symbol("ActOr", 2));
  static atermpp::function_symbol function_symbol_ActOr = atermpp::function_symbol("ActOr", 2);
  return function_symbol_ActOr;
}

inline
bool gsIsActOr(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_ActOr();
}

// ActSpec
inline
const atermpp::function_symbol& function_symbol_ActSpec()
{
//  static atermpp::function_symbol function_symbol_ActSpec = core::detail::initialise_static_expression(function_symbol_ActSpec, atermpp::function_symbol("ActSpec", 1));
  static atermpp::function_symbol function_symbol_ActSpec = atermpp::function_symbol("ActSpec", 1);
  return function_symbol_ActSpec;
}

inline
bool gsIsActSpec(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_ActSpec();
}

// ActTrue
inline
const atermpp::function_symbol& function_symbol_ActTrue()
{
//  static atermpp::function_symbol function_symbol_ActTrue = core::detail::initialise_static_expression(function_symbol_ActTrue, atermpp::function_symbol("ActTrue", 0));
  static atermpp::function_symbol function_symbol_ActTrue = atermpp::function_symbol("ActTrue", 0);
  return function_symbol_ActTrue;
}

inline
bool gsIsActTrue(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_ActTrue();
}

// Action
inline
const atermpp::function_symbol& function_symbol_Action()
{
//  static atermpp::function_symbol function_symbol_Action = core::detail::initialise_static_expression(function_symbol_Action, atermpp::function_symbol("Action", 2));
  static atermpp::function_symbol function_symbol_Action = atermpp::function_symbol("Action", 2);
  return function_symbol_Action;
}

inline
bool gsIsAction(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_Action();
}

// ActionRenameRule
inline
const atermpp::function_symbol& function_symbol_ActionRenameRule()
{
//  static atermpp::function_symbol function_symbol_ActionRenameRule = core::detail::initialise_static_expression(function_symbol_ActionRenameRule, atermpp::function_symbol("ActionRenameRule", 4));
  static atermpp::function_symbol function_symbol_ActionRenameRule = atermpp::function_symbol("ActionRenameRule", 4);
  return function_symbol_ActionRenameRule;
}

inline
bool gsIsActionRenameRule(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_ActionRenameRule();
}

// ActionRenameRules
inline
const atermpp::function_symbol& function_symbol_ActionRenameRules()
{
//  static atermpp::function_symbol function_symbol_ActionRenameRules = core::detail::initialise_static_expression(function_symbol_ActionRenameRules, atermpp::function_symbol("ActionRenameRules", 1));
  static atermpp::function_symbol function_symbol_ActionRenameRules = atermpp::function_symbol("ActionRenameRules", 1);
  return function_symbol_ActionRenameRules;
}

inline
bool gsIsActionRenameRules(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_ActionRenameRules();
}

// ActionRenameSpec
inline
const atermpp::function_symbol& function_symbol_ActionRenameSpec()
{
//  static atermpp::function_symbol function_symbol_ActionRenameSpec = core::detail::initialise_static_expression(function_symbol_ActionRenameSpec, atermpp::function_symbol("ActionRenameSpec", 3));
  static atermpp::function_symbol function_symbol_ActionRenameSpec = atermpp::function_symbol("ActionRenameSpec", 3);
  return function_symbol_ActionRenameSpec;
}

inline
bool gsIsActionRenameSpec(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_ActionRenameSpec();
}

// Allow
inline
const atermpp::function_symbol& function_symbol_Allow()
{
//  static atermpp::function_symbol function_symbol_Allow = core::detail::initialise_static_expression(function_symbol_Allow, atermpp::function_symbol("Allow", 2));
  static atermpp::function_symbol function_symbol_Allow = atermpp::function_symbol("Allow", 2);
  return function_symbol_Allow;
}

inline
bool gsIsAllow(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_Allow();
}

// AtTime
inline
const atermpp::function_symbol& function_symbol_AtTime()
{
//  static atermpp::function_symbol function_symbol_AtTime = core::detail::initialise_static_expression(function_symbol_AtTime, atermpp::function_symbol("AtTime", 2));
  static atermpp::function_symbol function_symbol_AtTime = atermpp::function_symbol("AtTime", 2);
  return function_symbol_AtTime;
}

inline
bool gsIsAtTime(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_AtTime();
}

// BES
inline
const atermpp::function_symbol& function_symbol_BES()
{
//  static atermpp::function_symbol function_symbol_BES = core::detail::initialise_static_expression(function_symbol_BES, atermpp::function_symbol("BES", 2));
  static atermpp::function_symbol function_symbol_BES = atermpp::function_symbol("BES", 2);
  return function_symbol_BES;
}

inline
bool gsIsBES(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_BES();
}

// BInit
inline
const atermpp::function_symbol& function_symbol_BInit()
{
//  static atermpp::function_symbol function_symbol_BInit = core::detail::initialise_static_expression(function_symbol_BInit, atermpp::function_symbol("BInit", 2));
  static atermpp::function_symbol function_symbol_BInit = atermpp::function_symbol("BInit", 2);
  return function_symbol_BInit;
}

inline
bool gsIsBInit(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_BInit();
}

// BagComp
inline
const atermpp::function_symbol& function_symbol_BagComp()
{
//  static atermpp::function_symbol function_symbol_BagComp = core::detail::initialise_static_expression(function_symbol_BagComp, atermpp::function_symbol("BagComp", 0));
  static atermpp::function_symbol function_symbol_BagComp = atermpp::function_symbol("BagComp", 0);
  return function_symbol_BagComp;
}

inline
bool gsIsBagComp(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_BagComp();
}

// Binder
inline
const atermpp::function_symbol& function_symbol_Binder()
{
//  static atermpp::function_symbol function_symbol_Binder = core::detail::initialise_static_expression(function_symbol_Binder, atermpp::function_symbol("Binder", 3));
  static atermpp::function_symbol function_symbol_Binder = atermpp::function_symbol("Binder", 3);
  return function_symbol_Binder;
}

inline
bool gsIsBinder(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_Binder();
}

// Block
inline
const atermpp::function_symbol& function_symbol_Block()
{
//  static atermpp::function_symbol function_symbol_Block = core::detail::initialise_static_expression(function_symbol_Block, atermpp::function_symbol("Block", 2));
  static atermpp::function_symbol function_symbol_Block = atermpp::function_symbol("Block", 2);
  return function_symbol_Block;
}

inline
bool gsIsBlock(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_Block();
}

// BooleanAnd
inline
const atermpp::function_symbol& function_symbol_BooleanAnd()
{
//  static atermpp::function_symbol function_symbol_BooleanAnd = core::detail::initialise_static_expression(function_symbol_BooleanAnd, atermpp::function_symbol("BooleanAnd", 2));
  static atermpp::function_symbol function_symbol_BooleanAnd = atermpp::function_symbol("BooleanAnd", 2);
  return function_symbol_BooleanAnd;
}

inline
bool gsIsBooleanAnd(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_BooleanAnd();
}

// BooleanEquation
inline
const atermpp::function_symbol& function_symbol_BooleanEquation()
{
//  static atermpp::function_symbol function_symbol_BooleanEquation = core::detail::initialise_static_expression(function_symbol_BooleanEquation, atermpp::function_symbol("BooleanEquation", 3));
  static atermpp::function_symbol function_symbol_BooleanEquation = atermpp::function_symbol("BooleanEquation", 3);
  return function_symbol_BooleanEquation;
}

inline
bool gsIsBooleanEquation(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_BooleanEquation();
}

// BooleanFalse
inline
const atermpp::function_symbol& function_symbol_BooleanFalse()
{
//  static atermpp::function_symbol function_symbol_BooleanFalse = core::detail::initialise_static_expression(function_symbol_BooleanFalse, atermpp::function_symbol("BooleanFalse", 0));
  static atermpp::function_symbol function_symbol_BooleanFalse = atermpp::function_symbol("BooleanFalse", 0);
  return function_symbol_BooleanFalse;
}

inline
bool gsIsBooleanFalse(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_BooleanFalse();
}

// BooleanImp
inline
const atermpp::function_symbol& function_symbol_BooleanImp()
{
//  static atermpp::function_symbol function_symbol_BooleanImp = core::detail::initialise_static_expression(function_symbol_BooleanImp, atermpp::function_symbol("BooleanImp", 2));
  static atermpp::function_symbol function_symbol_BooleanImp = atermpp::function_symbol("BooleanImp", 2);
  return function_symbol_BooleanImp;
}

inline
bool gsIsBooleanImp(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_BooleanImp();
}

// BooleanNot
inline
const atermpp::function_symbol& function_symbol_BooleanNot()
{
//  static atermpp::function_symbol function_symbol_BooleanNot = core::detail::initialise_static_expression(function_symbol_BooleanNot, atermpp::function_symbol("BooleanNot", 1));
  static atermpp::function_symbol function_symbol_BooleanNot = atermpp::function_symbol("BooleanNot", 1);
  return function_symbol_BooleanNot;
}

inline
bool gsIsBooleanNot(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_BooleanNot();
}

// BooleanOr
inline
const atermpp::function_symbol& function_symbol_BooleanOr()
{
//  static atermpp::function_symbol function_symbol_BooleanOr = core::detail::initialise_static_expression(function_symbol_BooleanOr, atermpp::function_symbol("BooleanOr", 2));
  static atermpp::function_symbol function_symbol_BooleanOr = atermpp::function_symbol("BooleanOr", 2);
  return function_symbol_BooleanOr;
}

inline
bool gsIsBooleanOr(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_BooleanOr();
}

// BooleanTrue
inline
const atermpp::function_symbol& function_symbol_BooleanTrue()
{
//  static atermpp::function_symbol function_symbol_BooleanTrue = core::detail::initialise_static_expression(function_symbol_BooleanTrue, atermpp::function_symbol("BooleanTrue", 0));
  static atermpp::function_symbol function_symbol_BooleanTrue = atermpp::function_symbol("BooleanTrue", 0);
  return function_symbol_BooleanTrue;
}

inline
bool gsIsBooleanTrue(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_BooleanTrue();
}

// BooleanVariable
inline
const atermpp::function_symbol& function_symbol_BooleanVariable()
{
//  static atermpp::function_symbol function_symbol_BooleanVariable = core::detail::initialise_static_expression(function_symbol_BooleanVariable, atermpp::function_symbol("BooleanVariable", 1));
  static atermpp::function_symbol function_symbol_BooleanVariable = atermpp::function_symbol("BooleanVariable", 1);
  return function_symbol_BooleanVariable;
}

inline
bool gsIsBooleanVariable(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_BooleanVariable();
}

// Choice
inline
const atermpp::function_symbol& function_symbol_Choice()
{
//  static atermpp::function_symbol function_symbol_Choice = core::detail::initialise_static_expression(function_symbol_Choice, atermpp::function_symbol("Choice", 2));
  static atermpp::function_symbol function_symbol_Choice = atermpp::function_symbol("Choice", 2);
  return function_symbol_Choice;
}

inline
bool gsIsChoice(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_Choice();
}

// Comm
inline
const atermpp::function_symbol& function_symbol_Comm()
{
//  static atermpp::function_symbol function_symbol_Comm = core::detail::initialise_static_expression(function_symbol_Comm, atermpp::function_symbol("Comm", 2));
  static atermpp::function_symbol function_symbol_Comm = atermpp::function_symbol("Comm", 2);
  return function_symbol_Comm;
}

inline
bool gsIsComm(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_Comm();
}

// CommExpr
inline
const atermpp::function_symbol& function_symbol_CommExpr()
{
//  static atermpp::function_symbol function_symbol_CommExpr = core::detail::initialise_static_expression(function_symbol_CommExpr, atermpp::function_symbol("CommExpr", 2));
  static atermpp::function_symbol function_symbol_CommExpr = atermpp::function_symbol("CommExpr", 2);
  return function_symbol_CommExpr;
}

inline
bool gsIsCommExpr(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_CommExpr();
}

// ConsSpec
inline
const atermpp::function_symbol& function_symbol_ConsSpec()
{
//  static atermpp::function_symbol function_symbol_ConsSpec = core::detail::initialise_static_expression(function_symbol_ConsSpec, atermpp::function_symbol("ConsSpec", 1));
  static atermpp::function_symbol function_symbol_ConsSpec = atermpp::function_symbol("ConsSpec", 1);
  return function_symbol_ConsSpec;
}

inline
bool gsIsConsSpec(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_ConsSpec();
}

// DataAppl
inline
const atermpp::function_symbol& function_symbol_DataAppl()
{
//  static atermpp::function_symbol function_symbol_DataAppl = core::detail::initialise_static_expression(function_symbol_DataAppl, atermpp::function_symbol("DataAppl", 2));
  static atermpp::function_symbol function_symbol_DataAppl = atermpp::function_symbol("DataAppl", 2);
  return function_symbol_DataAppl;
}

inline
bool gsIsDataAppl(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_DataAppl();
}

// DataEqn
inline
const atermpp::function_symbol& function_symbol_DataEqn()
{
//  static atermpp::function_symbol function_symbol_DataEqn = core::detail::initialise_static_expression(function_symbol_DataEqn, atermpp::function_symbol("DataEqn", 4));
  static atermpp::function_symbol function_symbol_DataEqn = atermpp::function_symbol("DataEqn", 4);
  return function_symbol_DataEqn;
}

inline
bool gsIsDataEqn(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_DataEqn();
}

// DataEqnSpec
inline
const atermpp::function_symbol& function_symbol_DataEqnSpec()
{
//  static atermpp::function_symbol function_symbol_DataEqnSpec = core::detail::initialise_static_expression(function_symbol_DataEqnSpec, atermpp::function_symbol("DataEqnSpec", 1));
  static atermpp::function_symbol function_symbol_DataEqnSpec = atermpp::function_symbol("DataEqnSpec", 1);
  return function_symbol_DataEqnSpec;
}

inline
bool gsIsDataEqnSpec(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_DataEqnSpec();
}

// DataSpec
inline
const atermpp::function_symbol& function_symbol_DataSpec()
{
//  static atermpp::function_symbol function_symbol_DataSpec = core::detail::initialise_static_expression(function_symbol_DataSpec, atermpp::function_symbol("DataSpec", 4));
  static atermpp::function_symbol function_symbol_DataSpec = atermpp::function_symbol("DataSpec", 4);
  return function_symbol_DataSpec;
}

inline
bool gsIsDataSpec(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_DataSpec();
}

// DataVarId
inline
const atermpp::function_symbol& function_symbol_DataVarId()
{
//  static atermpp::function_symbol function_symbol_DataVarId = core::detail::initialise_static_expression(function_symbol_DataVarId, atermpp::function_symbol("DataVarId", 2));
  static atermpp::function_symbol function_symbol_DataVarId = atermpp::function_symbol("DataVarId", 2);
  return function_symbol_DataVarId;
}

inline
bool gsIsDataVarId(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_DataVarId();
}

// DataVarIdInit
inline
const atermpp::function_symbol& function_symbol_DataVarIdInit()
{
//  static atermpp::function_symbol function_symbol_DataVarIdInit = core::detail::initialise_static_expression(function_symbol_DataVarIdInit, atermpp::function_symbol("DataVarIdInit", 2));
  static atermpp::function_symbol function_symbol_DataVarIdInit = atermpp::function_symbol("DataVarIdInit", 2);
  return function_symbol_DataVarIdInit;
}

inline
bool gsIsDataVarIdInit(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_DataVarIdInit();
}

// Delta
inline
const atermpp::function_symbol& function_symbol_Delta()
{
//  static atermpp::function_symbol function_symbol_Delta = core::detail::initialise_static_expression(function_symbol_Delta, atermpp::function_symbol("Delta", 0));
  static atermpp::function_symbol function_symbol_Delta = atermpp::function_symbol("Delta", 0);
  return function_symbol_Delta;
}

inline
bool gsIsDelta(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_Delta();
}

// Exists
inline
const atermpp::function_symbol& function_symbol_Exists()
{
//  static atermpp::function_symbol function_symbol_Exists = core::detail::initialise_static_expression(function_symbol_Exists, atermpp::function_symbol("Exists", 0));
  static atermpp::function_symbol function_symbol_Exists = atermpp::function_symbol("Exists", 0);
  return function_symbol_Exists;
}

inline
bool gsIsExists(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_Exists();
}

// Forall
inline
const atermpp::function_symbol& function_symbol_Forall()
{
//  static atermpp::function_symbol function_symbol_Forall = core::detail::initialise_static_expression(function_symbol_Forall, atermpp::function_symbol("Forall", 0));
  static atermpp::function_symbol function_symbol_Forall = atermpp::function_symbol("Forall", 0);
  return function_symbol_Forall;
}

inline
bool gsIsForall(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_Forall();
}

// GlobVarSpec
inline
const atermpp::function_symbol& function_symbol_GlobVarSpec()
{
//  static atermpp::function_symbol function_symbol_GlobVarSpec = core::detail::initialise_static_expression(function_symbol_GlobVarSpec, atermpp::function_symbol("GlobVarSpec", 1));
  static atermpp::function_symbol function_symbol_GlobVarSpec = atermpp::function_symbol("GlobVarSpec", 1);
  return function_symbol_GlobVarSpec;
}

inline
bool gsIsGlobVarSpec(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_GlobVarSpec();
}

// Hide
inline
const atermpp::function_symbol& function_symbol_Hide()
{
//  static atermpp::function_symbol function_symbol_Hide = core::detail::initialise_static_expression(function_symbol_Hide, atermpp::function_symbol("Hide", 2));
  static atermpp::function_symbol function_symbol_Hide = atermpp::function_symbol("Hide", 2);
  return function_symbol_Hide;
}

inline
bool gsIsHide(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_Hide();
}

// Id
inline
const atermpp::function_symbol& function_symbol_Id()
{
//  static atermpp::function_symbol function_symbol_Id = core::detail::initialise_static_expression(function_symbol_Id, atermpp::function_symbol("Id", 1));
  static atermpp::function_symbol function_symbol_Id = atermpp::function_symbol("Id", 1);
  return function_symbol_Id;
}

inline
bool gsIsId(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_Id();
}

// IdAssignment
inline
const atermpp::function_symbol& function_symbol_IdAssignment()
{
//  static atermpp::function_symbol function_symbol_IdAssignment = core::detail::initialise_static_expression(function_symbol_IdAssignment, atermpp::function_symbol("IdAssignment", 2));
  static atermpp::function_symbol function_symbol_IdAssignment = atermpp::function_symbol("IdAssignment", 2);
  return function_symbol_IdAssignment;
}

inline
bool gsIsIdAssignment(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_IdAssignment();
}

// IdInit
inline
const atermpp::function_symbol& function_symbol_IdInit()
{
//  static atermpp::function_symbol function_symbol_IdInit = core::detail::initialise_static_expression(function_symbol_IdInit, atermpp::function_symbol("IdInit", 2));
  static atermpp::function_symbol function_symbol_IdInit = atermpp::function_symbol("IdInit", 2);
  return function_symbol_IdInit;
}

inline
bool gsIsIdInit(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_IdInit();
}

// IfThen
inline
const atermpp::function_symbol& function_symbol_IfThen()
{
//  static atermpp::function_symbol function_symbol_IfThen = core::detail::initialise_static_expression(function_symbol_IfThen, atermpp::function_symbol("IfThen", 2));
  static atermpp::function_symbol function_symbol_IfThen = atermpp::function_symbol("IfThen", 2);
  return function_symbol_IfThen;
}

inline
bool gsIsIfThen(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_IfThen();
}

// IfThenElse
inline
const atermpp::function_symbol& function_symbol_IfThenElse()
{
//  static atermpp::function_symbol function_symbol_IfThenElse = core::detail::initialise_static_expression(function_symbol_IfThenElse, atermpp::function_symbol("IfThenElse", 3));
  static atermpp::function_symbol function_symbol_IfThenElse = atermpp::function_symbol("IfThenElse", 3);
  return function_symbol_IfThenElse;
}

inline
bool gsIsIfThenElse(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_IfThenElse();
}

// LMerge
inline
const atermpp::function_symbol& function_symbol_LMerge()
{
//  static atermpp::function_symbol function_symbol_LMerge = core::detail::initialise_static_expression(function_symbol_LMerge, atermpp::function_symbol("LMerge", 2));
  static atermpp::function_symbol function_symbol_LMerge = atermpp::function_symbol("LMerge", 2);
  return function_symbol_LMerge;
}

inline
bool gsIsLMerge(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_LMerge();
}

// Lambda
inline
const atermpp::function_symbol& function_symbol_Lambda()
{
//  static atermpp::function_symbol function_symbol_Lambda = core::detail::initialise_static_expression(function_symbol_Lambda, atermpp::function_symbol("Lambda", 0));
  static atermpp::function_symbol function_symbol_Lambda = atermpp::function_symbol("Lambda", 0);
  return function_symbol_Lambda;
}

inline
bool gsIsLambda(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_Lambda();
}

// LinProcSpec
inline
const atermpp::function_symbol& function_symbol_LinProcSpec()
{
//  static atermpp::function_symbol function_symbol_LinProcSpec = core::detail::initialise_static_expression(function_symbol_LinProcSpec, atermpp::function_symbol("LinProcSpec", 5));
  static atermpp::function_symbol function_symbol_LinProcSpec = atermpp::function_symbol("LinProcSpec", 5);
  return function_symbol_LinProcSpec;
}

inline
bool gsIsLinProcSpec(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_LinProcSpec();
}

// LinearProcess
inline
const atermpp::function_symbol& function_symbol_LinearProcess()
{
//  static atermpp::function_symbol function_symbol_LinearProcess = core::detail::initialise_static_expression(function_symbol_LinearProcess, atermpp::function_symbol("LinearProcess", 2));
  static atermpp::function_symbol function_symbol_LinearProcess = atermpp::function_symbol("LinearProcess", 2);
  return function_symbol_LinearProcess;
}

inline
bool gsIsLinearProcess(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_LinearProcess();
}

// LinearProcessInit
inline
const atermpp::function_symbol& function_symbol_LinearProcessInit()
{
//  static atermpp::function_symbol function_symbol_LinearProcessInit = core::detail::initialise_static_expression(function_symbol_LinearProcessInit, atermpp::function_symbol("LinearProcessInit", 1));
  static atermpp::function_symbol function_symbol_LinearProcessInit = atermpp::function_symbol("LinearProcessInit", 1);
  return function_symbol_LinearProcessInit;
}

inline
bool gsIsLinearProcessInit(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_LinearProcessInit();
}

// LinearProcessSummand
inline
const atermpp::function_symbol& function_symbol_LinearProcessSummand()
{
//  static atermpp::function_symbol function_symbol_LinearProcessSummand = core::detail::initialise_static_expression(function_symbol_LinearProcessSummand, atermpp::function_symbol("LinearProcessSummand", 5));
  static atermpp::function_symbol function_symbol_LinearProcessSummand = atermpp::function_symbol("LinearProcessSummand", 5);
  return function_symbol_LinearProcessSummand;
}

inline
bool gsIsLinearProcessSummand(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_LinearProcessSummand();
}

// MapSpec
inline
const atermpp::function_symbol& function_symbol_MapSpec()
{
//  static atermpp::function_symbol function_symbol_MapSpec = core::detail::initialise_static_expression(function_symbol_MapSpec, atermpp::function_symbol("MapSpec", 1));
  static atermpp::function_symbol function_symbol_MapSpec = atermpp::function_symbol("MapSpec", 1);
  return function_symbol_MapSpec;
}

inline
bool gsIsMapSpec(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_MapSpec();
}

// Merge
inline
const atermpp::function_symbol& function_symbol_Merge()
{
//  static atermpp::function_symbol function_symbol_Merge = core::detail::initialise_static_expression(function_symbol_Merge, atermpp::function_symbol("Merge", 2));
  static atermpp::function_symbol function_symbol_Merge = atermpp::function_symbol("Merge", 2);
  return function_symbol_Merge;
}

inline
bool gsIsMerge(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_Merge();
}

// Mu
inline
const atermpp::function_symbol& function_symbol_Mu()
{
//  static atermpp::function_symbol function_symbol_Mu = core::detail::initialise_static_expression(function_symbol_Mu, atermpp::function_symbol("Mu", 0));
  static atermpp::function_symbol function_symbol_Mu = atermpp::function_symbol("Mu", 0);
  return function_symbol_Mu;
}

inline
bool gsIsMu(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_Mu();
}

// MultAct
inline
const atermpp::function_symbol& function_symbol_MultAct()
{
//  static atermpp::function_symbol function_symbol_MultAct = core::detail::initialise_static_expression(function_symbol_MultAct, atermpp::function_symbol("MultAct", 1));
  static atermpp::function_symbol function_symbol_MultAct = atermpp::function_symbol("MultAct", 1);
  return function_symbol_MultAct;
}

inline
bool gsIsMultAct(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_MultAct();
}

// MultActName
inline
const atermpp::function_symbol& function_symbol_MultActName()
{
//  static atermpp::function_symbol function_symbol_MultActName = core::detail::initialise_static_expression(function_symbol_MultActName, atermpp::function_symbol("MultActName", 1));
  static atermpp::function_symbol function_symbol_MultActName = atermpp::function_symbol("MultActName", 1);
  return function_symbol_MultActName;
}

inline
bool gsIsMultActName(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_MultActName();
}

// Nil
inline
const atermpp::function_symbol& function_symbol_Nil()
{
//  static atermpp::function_symbol function_symbol_Nil = core::detail::initialise_static_expression(function_symbol_Nil, atermpp::function_symbol("Nil", 0));
  static atermpp::function_symbol function_symbol_Nil = atermpp::function_symbol("Nil", 0);
  return function_symbol_Nil;
}

inline
bool gsIsNil(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_Nil();
}

// Nu
inline
const atermpp::function_symbol& function_symbol_Nu()
{
//  static atermpp::function_symbol function_symbol_Nu = core::detail::initialise_static_expression(function_symbol_Nu, atermpp::function_symbol("Nu", 0));
  static atermpp::function_symbol function_symbol_Nu = atermpp::function_symbol("Nu", 0);
  return function_symbol_Nu;
}

inline
bool gsIsNu(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_Nu();
}

// OpId
inline
const atermpp::function_symbol& function_symbol_OpId()
{
//  static atermpp::function_symbol function_symbol_OpId = core::detail::initialise_static_expression(function_symbol_OpId, atermpp::function_symbol("OpId", 2));
  static atermpp::function_symbol function_symbol_OpId = atermpp::function_symbol("OpId", 2);
  return function_symbol_OpId;
}

inline
bool gsIsOpId(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_OpId();
}

// PBES
inline
const atermpp::function_symbol& function_symbol_PBES()
{
//  static atermpp::function_symbol function_symbol_PBES = core::detail::initialise_static_expression(function_symbol_PBES, atermpp::function_symbol("PBES", 4));
  static atermpp::function_symbol function_symbol_PBES = atermpp::function_symbol("PBES", 4);
  return function_symbol_PBES;
}

inline
bool gsIsPBES(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_PBES();
}

// PBESAnd
inline
const atermpp::function_symbol& function_symbol_PBESAnd()
{
//  static atermpp::function_symbol function_symbol_PBESAnd = core::detail::initialise_static_expression(function_symbol_PBESAnd, atermpp::function_symbol("PBESAnd", 2));
  static atermpp::function_symbol function_symbol_PBESAnd = atermpp::function_symbol("PBESAnd", 2);
  return function_symbol_PBESAnd;
}

inline
bool gsIsPBESAnd(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_PBESAnd();
}

// PBESExists
inline
const atermpp::function_symbol& function_symbol_PBESExists()
{
//  static atermpp::function_symbol function_symbol_PBESExists = core::detail::initialise_static_expression(function_symbol_PBESExists, atermpp::function_symbol("PBESExists", 2));
  static atermpp::function_symbol function_symbol_PBESExists = atermpp::function_symbol("PBESExists", 2);
  return function_symbol_PBESExists;
}

inline
bool gsIsPBESExists(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_PBESExists();
}

// PBESFalse
inline
const atermpp::function_symbol& function_symbol_PBESFalse()
{
//  static atermpp::function_symbol function_symbol_PBESFalse = core::detail::initialise_static_expression(function_symbol_PBESFalse, atermpp::function_symbol("PBESFalse", 0));
  static atermpp::function_symbol function_symbol_PBESFalse = atermpp::function_symbol("PBESFalse", 0);
  return function_symbol_PBESFalse;
}

inline
bool gsIsPBESFalse(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_PBESFalse();
}

// PBESForall
inline
const atermpp::function_symbol& function_symbol_PBESForall()
{
//  static atermpp::function_symbol function_symbol_PBESForall = core::detail::initialise_static_expression(function_symbol_PBESForall, atermpp::function_symbol("PBESForall", 2));
  static atermpp::function_symbol function_symbol_PBESForall = atermpp::function_symbol("PBESForall", 2);
  return function_symbol_PBESForall;
}

inline
bool gsIsPBESForall(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_PBESForall();
}

// PBESImp
inline
const atermpp::function_symbol& function_symbol_PBESImp()
{
//  static atermpp::function_symbol function_symbol_PBESImp = core::detail::initialise_static_expression(function_symbol_PBESImp, atermpp::function_symbol("PBESImp", 2));
  static atermpp::function_symbol function_symbol_PBESImp = atermpp::function_symbol("PBESImp", 2);
  return function_symbol_PBESImp;
}

inline
bool gsIsPBESImp(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_PBESImp();
}

// PBESNot
inline
const atermpp::function_symbol& function_symbol_PBESNot()
{
//  static atermpp::function_symbol function_symbol_PBESNot = core::detail::initialise_static_expression(function_symbol_PBESNot, atermpp::function_symbol("PBESNot", 1));
  static atermpp::function_symbol function_symbol_PBESNot = atermpp::function_symbol("PBESNot", 1);
  return function_symbol_PBESNot;
}

inline
bool gsIsPBESNot(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_PBESNot();
}

// PBESOr
inline
const atermpp::function_symbol& function_symbol_PBESOr()
{
//  static atermpp::function_symbol function_symbol_PBESOr = core::detail::initialise_static_expression(function_symbol_PBESOr, atermpp::function_symbol("PBESOr", 2));
  static atermpp::function_symbol function_symbol_PBESOr = atermpp::function_symbol("PBESOr", 2);
  return function_symbol_PBESOr;
}

inline
bool gsIsPBESOr(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_PBESOr();
}

// PBESTrue
inline
const atermpp::function_symbol& function_symbol_PBESTrue()
{
//  static atermpp::function_symbol function_symbol_PBESTrue = core::detail::initialise_static_expression(function_symbol_PBESTrue, atermpp::function_symbol("PBESTrue", 0));
  static atermpp::function_symbol function_symbol_PBESTrue = atermpp::function_symbol("PBESTrue", 0);
  return function_symbol_PBESTrue;
}

inline
bool gsIsPBESTrue(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_PBESTrue();
}

// PBEqn
inline
const atermpp::function_symbol& function_symbol_PBEqn()
{
//  static atermpp::function_symbol function_symbol_PBEqn = core::detail::initialise_static_expression(function_symbol_PBEqn, atermpp::function_symbol("PBEqn", 3));
  static atermpp::function_symbol function_symbol_PBEqn = atermpp::function_symbol("PBEqn", 3);
  return function_symbol_PBEqn;
}

inline
bool gsIsPBEqn(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_PBEqn();
}

// PBEqnSpec
inline
const atermpp::function_symbol& function_symbol_PBEqnSpec()
{
//  static atermpp::function_symbol function_symbol_PBEqnSpec = core::detail::initialise_static_expression(function_symbol_PBEqnSpec, atermpp::function_symbol("PBEqnSpec", 1));
  static atermpp::function_symbol function_symbol_PBEqnSpec = atermpp::function_symbol("PBEqnSpec", 1);
  return function_symbol_PBEqnSpec;
}

inline
bool gsIsPBEqnSpec(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_PBEqnSpec();
}

// PBInit
inline
const atermpp::function_symbol& function_symbol_PBInit()
{
//  static atermpp::function_symbol function_symbol_PBInit = core::detail::initialise_static_expression(function_symbol_PBInit, atermpp::function_symbol("PBInit", 1));
  static atermpp::function_symbol function_symbol_PBInit = atermpp::function_symbol("PBInit", 1);
  return function_symbol_PBInit;
}

inline
bool gsIsPBInit(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_PBInit();
}

// ParamId
inline
const atermpp::function_symbol& function_symbol_ParamId()
{
//  static atermpp::function_symbol function_symbol_ParamId = core::detail::initialise_static_expression(function_symbol_ParamId, atermpp::function_symbol("ParamId", 2));
  static atermpp::function_symbol function_symbol_ParamId = atermpp::function_symbol("ParamId", 2);
  return function_symbol_ParamId;
}

inline
bool gsIsParamId(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_ParamId();
}

// ProcEqn
inline
const atermpp::function_symbol& function_symbol_ProcEqn()
{
//  static atermpp::function_symbol function_symbol_ProcEqn = core::detail::initialise_static_expression(function_symbol_ProcEqn, atermpp::function_symbol("ProcEqn", 3));
  static atermpp::function_symbol function_symbol_ProcEqn = atermpp::function_symbol("ProcEqn", 3);
  return function_symbol_ProcEqn;
}

inline
bool gsIsProcEqn(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_ProcEqn();
}

// ProcEqnSpec
inline
const atermpp::function_symbol& function_symbol_ProcEqnSpec()
{
//  static atermpp::function_symbol function_symbol_ProcEqnSpec = core::detail::initialise_static_expression(function_symbol_ProcEqnSpec, atermpp::function_symbol("ProcEqnSpec", 1));
  static atermpp::function_symbol function_symbol_ProcEqnSpec = atermpp::function_symbol("ProcEqnSpec", 1);
  return function_symbol_ProcEqnSpec;
}

inline
bool gsIsProcEqnSpec(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_ProcEqnSpec();
}

// ProcSpec
inline
const atermpp::function_symbol& function_symbol_ProcSpec()
{
//  static atermpp::function_symbol function_symbol_ProcSpec = core::detail::initialise_static_expression(function_symbol_ProcSpec, atermpp::function_symbol("ProcSpec", 5));
  static atermpp::function_symbol function_symbol_ProcSpec = atermpp::function_symbol("ProcSpec", 5);
  return function_symbol_ProcSpec;
}

inline
bool gsIsProcSpec(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_ProcSpec();
}

// ProcVarId
inline
const atermpp::function_symbol& function_symbol_ProcVarId()
{
//  static atermpp::function_symbol function_symbol_ProcVarId = core::detail::initialise_static_expression(function_symbol_ProcVarId, atermpp::function_symbol("ProcVarId", 2));
  static atermpp::function_symbol function_symbol_ProcVarId = atermpp::function_symbol("ProcVarId", 2);
  return function_symbol_ProcVarId;
}

inline
bool gsIsProcVarId(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_ProcVarId();
}

// Process
inline
const atermpp::function_symbol& function_symbol_Process()
{
//  static atermpp::function_symbol function_symbol_Process = core::detail::initialise_static_expression(function_symbol_Process, atermpp::function_symbol("Process", 2));
  static atermpp::function_symbol function_symbol_Process = atermpp::function_symbol("Process", 2);
  return function_symbol_Process;
}

inline
bool gsIsProcess(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_Process();
}

// ProcessAssignment
inline
const atermpp::function_symbol& function_symbol_ProcessAssignment()
{
//  static atermpp::function_symbol function_symbol_ProcessAssignment = core::detail::initialise_static_expression(function_symbol_ProcessAssignment, atermpp::function_symbol("ProcessAssignment", 2));
  static atermpp::function_symbol function_symbol_ProcessAssignment = atermpp::function_symbol("ProcessAssignment", 2);
  return function_symbol_ProcessAssignment;
}

inline
bool gsIsProcessAssignment(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_ProcessAssignment();
}

// ProcessInit
inline
const atermpp::function_symbol& function_symbol_ProcessInit()
{
//  static atermpp::function_symbol function_symbol_ProcessInit = core::detail::initialise_static_expression(function_symbol_ProcessInit, atermpp::function_symbol("ProcessInit", 1));
  static atermpp::function_symbol function_symbol_ProcessInit = atermpp::function_symbol("ProcessInit", 1);
  return function_symbol_ProcessInit;
}

inline
bool gsIsProcessInit(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_ProcessInit();
}

// PropVarDecl
inline
const atermpp::function_symbol& function_symbol_PropVarDecl()
{
//  static atermpp::function_symbol function_symbol_PropVarDecl = core::detail::initialise_static_expression(function_symbol_PropVarDecl, atermpp::function_symbol("PropVarDecl", 2));
  static atermpp::function_symbol function_symbol_PropVarDecl = atermpp::function_symbol("PropVarDecl", 2);
  return function_symbol_PropVarDecl;
}

inline
bool gsIsPropVarDecl(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_PropVarDecl();
}

// PropVarInst
inline
const atermpp::function_symbol& function_symbol_PropVarInst()
{
//  static atermpp::function_symbol function_symbol_PropVarInst = core::detail::initialise_static_expression(function_symbol_PropVarInst, atermpp::function_symbol("PropVarInst", 2));
  static atermpp::function_symbol function_symbol_PropVarInst = atermpp::function_symbol("PropVarInst", 2);
  return function_symbol_PropVarInst;
}

inline
bool gsIsPropVarInst(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_PropVarInst();
}

// RegAlt
inline
const atermpp::function_symbol& function_symbol_RegAlt()
{
//  static atermpp::function_symbol function_symbol_RegAlt = core::detail::initialise_static_expression(function_symbol_RegAlt, atermpp::function_symbol("RegAlt", 2));
  static atermpp::function_symbol function_symbol_RegAlt = atermpp::function_symbol("RegAlt", 2);
  return function_symbol_RegAlt;
}

inline
bool gsIsRegAlt(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_RegAlt();
}

// RegNil
inline
const atermpp::function_symbol& function_symbol_RegNil()
{
//  static atermpp::function_symbol function_symbol_RegNil = core::detail::initialise_static_expression(function_symbol_RegNil, atermpp::function_symbol("RegNil", 0));
  static atermpp::function_symbol function_symbol_RegNil = atermpp::function_symbol("RegNil", 0);
  return function_symbol_RegNil;
}

inline
bool gsIsRegNil(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_RegNil();
}

// RegSeq
inline
const atermpp::function_symbol& function_symbol_RegSeq()
{
//  static atermpp::function_symbol function_symbol_RegSeq = core::detail::initialise_static_expression(function_symbol_RegSeq, atermpp::function_symbol("RegSeq", 2));
  static atermpp::function_symbol function_symbol_RegSeq = atermpp::function_symbol("RegSeq", 2);
  return function_symbol_RegSeq;
}

inline
bool gsIsRegSeq(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_RegSeq();
}

// RegTrans
inline
const atermpp::function_symbol& function_symbol_RegTrans()
{
//  static atermpp::function_symbol function_symbol_RegTrans = core::detail::initialise_static_expression(function_symbol_RegTrans, atermpp::function_symbol("RegTrans", 1));
  static atermpp::function_symbol function_symbol_RegTrans = atermpp::function_symbol("RegTrans", 1);
  return function_symbol_RegTrans;
}

inline
bool gsIsRegTrans(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_RegTrans();
}

// RegTransOrNil
inline
const atermpp::function_symbol& function_symbol_RegTransOrNil()
{
//  static atermpp::function_symbol function_symbol_RegTransOrNil = core::detail::initialise_static_expression(function_symbol_RegTransOrNil, atermpp::function_symbol("RegTransOrNil", 1));
  static atermpp::function_symbol function_symbol_RegTransOrNil = atermpp::function_symbol("RegTransOrNil", 1);
  return function_symbol_RegTransOrNil;
}

inline
bool gsIsRegTransOrNil(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_RegTransOrNil();
}

// Rename
inline
const atermpp::function_symbol& function_symbol_Rename()
{
//  static atermpp::function_symbol function_symbol_Rename = core::detail::initialise_static_expression(function_symbol_Rename, atermpp::function_symbol("Rename", 2));
  static atermpp::function_symbol function_symbol_Rename = atermpp::function_symbol("Rename", 2);
  return function_symbol_Rename;
}

inline
bool gsIsRename(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_Rename();
}

// RenameExpr
inline
const atermpp::function_symbol& function_symbol_RenameExpr()
{
//  static atermpp::function_symbol function_symbol_RenameExpr = core::detail::initialise_static_expression(function_symbol_RenameExpr, atermpp::function_symbol("RenameExpr", 2));
  static atermpp::function_symbol function_symbol_RenameExpr = atermpp::function_symbol("RenameExpr", 2);
  return function_symbol_RenameExpr;
}

inline
bool gsIsRenameExpr(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_RenameExpr();
}

// Seq
inline
const atermpp::function_symbol& function_symbol_Seq()
{
//  static atermpp::function_symbol function_symbol_Seq = core::detail::initialise_static_expression(function_symbol_Seq, atermpp::function_symbol("Seq", 2));
  static atermpp::function_symbol function_symbol_Seq = atermpp::function_symbol("Seq", 2);
  return function_symbol_Seq;
}

inline
bool gsIsSeq(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_Seq();
}

// SetBagComp
inline
const atermpp::function_symbol& function_symbol_SetBagComp()
{
//  static atermpp::function_symbol function_symbol_SetBagComp = core::detail::initialise_static_expression(function_symbol_SetBagComp, atermpp::function_symbol("SetBagComp", 0));
  static atermpp::function_symbol function_symbol_SetBagComp = atermpp::function_symbol("SetBagComp", 0);
  return function_symbol_SetBagComp;
}

inline
bool gsIsSetBagComp(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_SetBagComp();
}

// SetComp
inline
const atermpp::function_symbol& function_symbol_SetComp()
{
//  static atermpp::function_symbol function_symbol_SetComp = core::detail::initialise_static_expression(function_symbol_SetComp, atermpp::function_symbol("SetComp", 0));
  static atermpp::function_symbol function_symbol_SetComp = atermpp::function_symbol("SetComp", 0);
  return function_symbol_SetComp;
}

inline
bool gsIsSetComp(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_SetComp();
}

// SortArrow
inline
const atermpp::function_symbol& function_symbol_SortArrow()
{
//  static atermpp::function_symbol function_symbol_SortArrow = core::detail::initialise_static_expression(function_symbol_SortArrow, atermpp::function_symbol("SortArrow", 2));
  static atermpp::function_symbol function_symbol_SortArrow = atermpp::function_symbol("SortArrow", 2);
  return function_symbol_SortArrow;
}

inline
bool gsIsSortArrow(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_SortArrow();
}

// SortBag
inline
const atermpp::function_symbol& function_symbol_SortBag()
{
//  static atermpp::function_symbol function_symbol_SortBag = core::detail::initialise_static_expression(function_symbol_SortBag, atermpp::function_symbol("SortBag", 0));
  static atermpp::function_symbol function_symbol_SortBag = atermpp::function_symbol("SortBag", 0);
  return function_symbol_SortBag;
}

inline
bool gsIsSortBag(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_SortBag();
}

// SortCons
inline
const atermpp::function_symbol& function_symbol_SortCons()
{
//  static atermpp::function_symbol function_symbol_SortCons = core::detail::initialise_static_expression(function_symbol_SortCons, atermpp::function_symbol("SortCons", 2));
  static atermpp::function_symbol function_symbol_SortCons = atermpp::function_symbol("SortCons", 2);
  return function_symbol_SortCons;
}

inline
bool gsIsSortCons(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_SortCons();
}

// SortFBag
inline
const atermpp::function_symbol& function_symbol_SortFBag()
{
//  static atermpp::function_symbol function_symbol_SortFBag = core::detail::initialise_static_expression(function_symbol_SortFBag, atermpp::function_symbol("SortFBag", 0));
  static atermpp::function_symbol function_symbol_SortFBag = atermpp::function_symbol("SortFBag", 0);
  return function_symbol_SortFBag;
}

inline
bool gsIsSortFBag(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_SortFBag();
}

// SortFSet
inline
const atermpp::function_symbol& function_symbol_SortFSet()
{
//  static atermpp::function_symbol function_symbol_SortFSet = core::detail::initialise_static_expression(function_symbol_SortFSet, atermpp::function_symbol("SortFSet", 0));
  static atermpp::function_symbol function_symbol_SortFSet = atermpp::function_symbol("SortFSet", 0);
  return function_symbol_SortFSet;
}

inline
bool gsIsSortFSet(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_SortFSet();
}

// SortId
inline
const atermpp::function_symbol& function_symbol_SortId()
{
//  static atermpp::function_symbol function_symbol_SortId = core::detail::initialise_static_expression(function_symbol_SortId, atermpp::function_symbol("SortId", 1));
  static atermpp::function_symbol function_symbol_SortId = atermpp::function_symbol("SortId", 1);
  return function_symbol_SortId;
}

inline
bool gsIsSortId(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_SortId();
}

// SortList
inline
const atermpp::function_symbol& function_symbol_SortList()
{
//  static atermpp::function_symbol function_symbol_SortList = core::detail::initialise_static_expression(function_symbol_SortList, atermpp::function_symbol("SortList", 0));
  static atermpp::function_symbol function_symbol_SortList = atermpp::function_symbol("SortList", 0);
  return function_symbol_SortList;
}

inline
bool gsIsSortList(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_SortList();
}

// SortRef
inline
const atermpp::function_symbol& function_symbol_SortRef()
{
//  static atermpp::function_symbol function_symbol_SortRef = core::detail::initialise_static_expression(function_symbol_SortRef, atermpp::function_symbol("SortRef", 2));
  static atermpp::function_symbol function_symbol_SortRef = atermpp::function_symbol("SortRef", 2);
  return function_symbol_SortRef;
}

inline
bool gsIsSortRef(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_SortRef();
}

// SortSet
inline
const atermpp::function_symbol& function_symbol_SortSet()
{
//  static atermpp::function_symbol function_symbol_SortSet = core::detail::initialise_static_expression(function_symbol_SortSet, atermpp::function_symbol("SortSet", 0));
  static atermpp::function_symbol function_symbol_SortSet = atermpp::function_symbol("SortSet", 0);
  return function_symbol_SortSet;
}

inline
bool gsIsSortSet(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_SortSet();
}

// SortSpec
inline
const atermpp::function_symbol& function_symbol_SortSpec()
{
//  static atermpp::function_symbol function_symbol_SortSpec = core::detail::initialise_static_expression(function_symbol_SortSpec, atermpp::function_symbol("SortSpec", 1));
  static atermpp::function_symbol function_symbol_SortSpec = atermpp::function_symbol("SortSpec", 1);
  return function_symbol_SortSpec;
}

inline
bool gsIsSortSpec(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_SortSpec();
}

// SortStruct
inline
const atermpp::function_symbol& function_symbol_SortStruct()
{
//  static atermpp::function_symbol function_symbol_SortStruct = core::detail::initialise_static_expression(function_symbol_SortStruct, atermpp::function_symbol("SortStruct", 1));
  static atermpp::function_symbol function_symbol_SortStruct = atermpp::function_symbol("SortStruct", 1);
  return function_symbol_SortStruct;
}

inline
bool gsIsSortStruct(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_SortStruct();
}

// SortUnknown
inline
const atermpp::function_symbol& function_symbol_SortUnknown()
{
//  static atermpp::function_symbol function_symbol_SortUnknown = core::detail::initialise_static_expression(function_symbol_SortUnknown, atermpp::function_symbol("SortUnknown", 0));
  static atermpp::function_symbol function_symbol_SortUnknown = atermpp::function_symbol("SortUnknown", 0);
  return function_symbol_SortUnknown;
}

inline
bool gsIsSortUnknown(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_SortUnknown();
}

// SortsPossible
inline
const atermpp::function_symbol& function_symbol_SortsPossible()
{
//  static atermpp::function_symbol function_symbol_SortsPossible = core::detail::initialise_static_expression(function_symbol_SortsPossible, atermpp::function_symbol("SortsPossible", 1));
  static atermpp::function_symbol function_symbol_SortsPossible = atermpp::function_symbol("SortsPossible", 1);
  return function_symbol_SortsPossible;
}

inline
bool gsIsSortsPossible(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_SortsPossible();
}

// StateAnd
inline
const atermpp::function_symbol& function_symbol_StateAnd()
{
//  static atermpp::function_symbol function_symbol_StateAnd = core::detail::initialise_static_expression(function_symbol_StateAnd, atermpp::function_symbol("StateAnd", 2));
  static atermpp::function_symbol function_symbol_StateAnd = atermpp::function_symbol("StateAnd", 2);
  return function_symbol_StateAnd;
}

inline
bool gsIsStateAnd(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_StateAnd();
}

// StateDelay
inline
const atermpp::function_symbol& function_symbol_StateDelay()
{
//  static atermpp::function_symbol function_symbol_StateDelay = core::detail::initialise_static_expression(function_symbol_StateDelay, atermpp::function_symbol("StateDelay", 0));
  static atermpp::function_symbol function_symbol_StateDelay = atermpp::function_symbol("StateDelay", 0);
  return function_symbol_StateDelay;
}

inline
bool gsIsStateDelay(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_StateDelay();
}

// StateDelayTimed
inline
const atermpp::function_symbol& function_symbol_StateDelayTimed()
{
//  static atermpp::function_symbol function_symbol_StateDelayTimed = core::detail::initialise_static_expression(function_symbol_StateDelayTimed, atermpp::function_symbol("StateDelayTimed", 1));
  static atermpp::function_symbol function_symbol_StateDelayTimed = atermpp::function_symbol("StateDelayTimed", 1);
  return function_symbol_StateDelayTimed;
}

inline
bool gsIsStateDelayTimed(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_StateDelayTimed();
}

// StateExists
inline
const atermpp::function_symbol& function_symbol_StateExists()
{
//  static atermpp::function_symbol function_symbol_StateExists = core::detail::initialise_static_expression(function_symbol_StateExists, atermpp::function_symbol("StateExists", 2));
  static atermpp::function_symbol function_symbol_StateExists = atermpp::function_symbol("StateExists", 2);
  return function_symbol_StateExists;
}

inline
bool gsIsStateExists(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_StateExists();
}

// StateFalse
inline
const atermpp::function_symbol& function_symbol_StateFalse()
{
//  static atermpp::function_symbol function_symbol_StateFalse = core::detail::initialise_static_expression(function_symbol_StateFalse, atermpp::function_symbol("StateFalse", 0));
  static atermpp::function_symbol function_symbol_StateFalse = atermpp::function_symbol("StateFalse", 0);
  return function_symbol_StateFalse;
}

inline
bool gsIsStateFalse(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_StateFalse();
}

// StateForall
inline
const atermpp::function_symbol& function_symbol_StateForall()
{
//  static atermpp::function_symbol function_symbol_StateForall = core::detail::initialise_static_expression(function_symbol_StateForall, atermpp::function_symbol("StateForall", 2));
  static atermpp::function_symbol function_symbol_StateForall = atermpp::function_symbol("StateForall", 2);
  return function_symbol_StateForall;
}

inline
bool gsIsStateForall(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_StateForall();
}

// StateImp
inline
const atermpp::function_symbol& function_symbol_StateImp()
{
//  static atermpp::function_symbol function_symbol_StateImp = core::detail::initialise_static_expression(function_symbol_StateImp, atermpp::function_symbol("StateImp", 2));
  static atermpp::function_symbol function_symbol_StateImp = atermpp::function_symbol("StateImp", 2);
  return function_symbol_StateImp;
}

inline
bool gsIsStateImp(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_StateImp();
}

// StateMay
inline
const atermpp::function_symbol& function_symbol_StateMay()
{
//  static atermpp::function_symbol function_symbol_StateMay = core::detail::initialise_static_expression(function_symbol_StateMay, atermpp::function_symbol("StateMay", 2));
  static atermpp::function_symbol function_symbol_StateMay = atermpp::function_symbol("StateMay", 2);
  return function_symbol_StateMay;
}

inline
bool gsIsStateMay(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_StateMay();
}

// StateMu
inline
const atermpp::function_symbol& function_symbol_StateMu()
{
//  static atermpp::function_symbol function_symbol_StateMu = core::detail::initialise_static_expression(function_symbol_StateMu, atermpp::function_symbol("StateMu", 3));
  static atermpp::function_symbol function_symbol_StateMu = atermpp::function_symbol("StateMu", 3);
  return function_symbol_StateMu;
}

inline
bool gsIsStateMu(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_StateMu();
}

// StateMust
inline
const atermpp::function_symbol& function_symbol_StateMust()
{
//  static atermpp::function_symbol function_symbol_StateMust = core::detail::initialise_static_expression(function_symbol_StateMust, atermpp::function_symbol("StateMust", 2));
  static atermpp::function_symbol function_symbol_StateMust = atermpp::function_symbol("StateMust", 2);
  return function_symbol_StateMust;
}

inline
bool gsIsStateMust(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_StateMust();
}

// StateNot
inline
const atermpp::function_symbol& function_symbol_StateNot()
{
//  static atermpp::function_symbol function_symbol_StateNot = core::detail::initialise_static_expression(function_symbol_StateNot, atermpp::function_symbol("StateNot", 1));
  static atermpp::function_symbol function_symbol_StateNot = atermpp::function_symbol("StateNot", 1);
  return function_symbol_StateNot;
}

inline
bool gsIsStateNot(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_StateNot();
}

// StateNu
inline
const atermpp::function_symbol& function_symbol_StateNu()
{
//  static atermpp::function_symbol function_symbol_StateNu = core::detail::initialise_static_expression(function_symbol_StateNu, atermpp::function_symbol("StateNu", 3));
  static atermpp::function_symbol function_symbol_StateNu = atermpp::function_symbol("StateNu", 3);
  return function_symbol_StateNu;
}

inline
bool gsIsStateNu(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_StateNu();
}

// StateOr
inline
const atermpp::function_symbol& function_symbol_StateOr()
{
//  static atermpp::function_symbol function_symbol_StateOr = core::detail::initialise_static_expression(function_symbol_StateOr, atermpp::function_symbol("StateOr", 2));
  static atermpp::function_symbol function_symbol_StateOr = atermpp::function_symbol("StateOr", 2);
  return function_symbol_StateOr;
}

inline
bool gsIsStateOr(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_StateOr();
}

// StateTrue
inline
const atermpp::function_symbol& function_symbol_StateTrue()
{
//  static atermpp::function_symbol function_symbol_StateTrue = core::detail::initialise_static_expression(function_symbol_StateTrue, atermpp::function_symbol("StateTrue", 0));
  static atermpp::function_symbol function_symbol_StateTrue = atermpp::function_symbol("StateTrue", 0);
  return function_symbol_StateTrue;
}

inline
bool gsIsStateTrue(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_StateTrue();
}

// StateVar
inline
const atermpp::function_symbol& function_symbol_StateVar()
{
//  static atermpp::function_symbol function_symbol_StateVar = core::detail::initialise_static_expression(function_symbol_StateVar, atermpp::function_symbol("StateVar", 2));
  static atermpp::function_symbol function_symbol_StateVar = atermpp::function_symbol("StateVar", 2);
  return function_symbol_StateVar;
}

inline
bool gsIsStateVar(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_StateVar();
}

// StateYaled
inline
const atermpp::function_symbol& function_symbol_StateYaled()
{
//  static atermpp::function_symbol function_symbol_StateYaled = core::detail::initialise_static_expression(function_symbol_StateYaled, atermpp::function_symbol("StateYaled", 0));
  static atermpp::function_symbol function_symbol_StateYaled = atermpp::function_symbol("StateYaled", 0);
  return function_symbol_StateYaled;
}

inline
bool gsIsStateYaled(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_StateYaled();
}

// StateYaledTimed
inline
const atermpp::function_symbol& function_symbol_StateYaledTimed()
{
//  static atermpp::function_symbol function_symbol_StateYaledTimed = core::detail::initialise_static_expression(function_symbol_StateYaledTimed, atermpp::function_symbol("StateYaledTimed", 1));
  static atermpp::function_symbol function_symbol_StateYaledTimed = atermpp::function_symbol("StateYaledTimed", 1);
  return function_symbol_StateYaledTimed;
}

inline
bool gsIsStateYaledTimed(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_StateYaledTimed();
}

// StructCons
inline
const atermpp::function_symbol& function_symbol_StructCons()
{
//  static atermpp::function_symbol function_symbol_StructCons = core::detail::initialise_static_expression(function_symbol_StructCons, atermpp::function_symbol("StructCons", 3));
  static atermpp::function_symbol function_symbol_StructCons = atermpp::function_symbol("StructCons", 3);
  return function_symbol_StructCons;
}

inline
bool gsIsStructCons(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_StructCons();
}

// StructProj
inline
const atermpp::function_symbol& function_symbol_StructProj()
{
//  static atermpp::function_symbol function_symbol_StructProj = core::detail::initialise_static_expression(function_symbol_StructProj, atermpp::function_symbol("StructProj", 2));
  static atermpp::function_symbol function_symbol_StructProj = atermpp::function_symbol("StructProj", 2);
  return function_symbol_StructProj;
}

inline
bool gsIsStructProj(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_StructProj();
}

// Sum
inline
const atermpp::function_symbol& function_symbol_Sum()
{
//  static atermpp::function_symbol function_symbol_Sum = core::detail::initialise_static_expression(function_symbol_Sum, atermpp::function_symbol("Sum", 2));
  static atermpp::function_symbol function_symbol_Sum = atermpp::function_symbol("Sum", 2);
  return function_symbol_Sum;
}

inline
bool gsIsSum(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_Sum();
}

// Sync
inline
const atermpp::function_symbol& function_symbol_Sync()
{
//  static atermpp::function_symbol function_symbol_Sync = core::detail::initialise_static_expression(function_symbol_Sync, atermpp::function_symbol("Sync", 2));
  static atermpp::function_symbol function_symbol_Sync = atermpp::function_symbol("Sync", 2);
  return function_symbol_Sync;
}

inline
bool gsIsSync(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_Sync();
}

// Tau
inline
const atermpp::function_symbol& function_symbol_Tau()
{
//  static atermpp::function_symbol function_symbol_Tau = core::detail::initialise_static_expression(function_symbol_Tau, atermpp::function_symbol("Tau", 0));
  static atermpp::function_symbol function_symbol_Tau = atermpp::function_symbol("Tau", 0);
  return function_symbol_Tau;
}

inline
bool gsIsTau(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_Tau();
}

// Whr
inline
const atermpp::function_symbol& function_symbol_Whr()
{
//  static atermpp::function_symbol function_symbol_Whr = core::detail::initialise_static_expression(function_symbol_Whr, atermpp::function_symbol("Whr", 2));
  static atermpp::function_symbol function_symbol_Whr = atermpp::function_symbol("Whr", 2);
  return function_symbol_Whr;
}

inline
bool gsIsWhr(const atermpp::aterm_appl& Term)
{
  return ATgetAFun(Term) == function_symbol_Whr();
}

inline
ATermAppl gsMakeActAnd(const ATermAppl& ActFrm_0, const ATermAppl& ActFrm_1)
{
  return ATmakeAppl2(function_symbol_ActAnd(), ActFrm_0, ActFrm_1);
}

inline
ATermAppl gsMakeActAt(const ATermAppl& ActFrm_0, const ATermAppl& DataExpr_1)
{
  return ATmakeAppl2(function_symbol_ActAt(), ActFrm_0, DataExpr_1);
}

inline
ATermAppl gsMakeActExists(const ATermList& DataVarId_0, const ATermAppl& ActFrm_1)
{
  return ATmakeAppl2(function_symbol_ActExists(), DataVarId_0, ActFrm_1);
}

inline
ATermAppl gsMakeActFalse()
{
  return ATmakeAppl0(function_symbol_ActFalse());
}

inline
ATermAppl gsMakeActForall(const ATermList& DataVarId_0, const ATermAppl& ActFrm_1)
{
  return ATmakeAppl2(function_symbol_ActForall(), DataVarId_0, ActFrm_1);
}

inline
ATermAppl gsMakeActId(const ATermAppl& String_0, const ATermList& SortExpr_1)
{
  return ATmakeAppl2(function_symbol_ActId(), String_0, SortExpr_1);
}

inline
ATermAppl gsMakeActImp(const ATermAppl& ActFrm_0, const ATermAppl& ActFrm_1)
{
  return ATmakeAppl2(function_symbol_ActImp(), ActFrm_0, ActFrm_1);
}

inline
ATermAppl gsMakeActNot(const ATermAppl& ActFrm_0)
{
  return ATmakeAppl1(function_symbol_ActNot(), ActFrm_0);
}

inline
ATermAppl gsMakeActOr(const ATermAppl& ActFrm_0, const ATermAppl& ActFrm_1)
{
  return ATmakeAppl2(function_symbol_ActOr(), ActFrm_0, ActFrm_1);
}

inline
ATermAppl gsMakeActSpec(const ATermList& ActId_0)
{
  return ATmakeAppl1(function_symbol_ActSpec(), ActId_0);
}

inline
ATermAppl gsMakeActTrue()
{
  return ATmakeAppl0(function_symbol_ActTrue());
}

inline
ATermAppl gsMakeAction(const ATermAppl& ActId_0, const ATermList& DataExpr_1)
{
  return ATmakeAppl2(function_symbol_Action(), ActId_0, DataExpr_1);
}

inline
ATermAppl gsMakeActionRenameRule(const ATermList& DataVarId_0, const ATermAppl& DataExpr_1, const ATermAppl& ParamIdOrAction_2, const ATermAppl& ActionRenameRuleRHS_3)
{
  return ATmakeAppl4(function_symbol_ActionRenameRule(), DataVarId_0, DataExpr_1, ParamIdOrAction_2, ActionRenameRuleRHS_3);
}

inline
ATermAppl gsMakeActionRenameRules(const ATermList& ActionRenameRule_0)
{
  return ATmakeAppl1(function_symbol_ActionRenameRules(), ActionRenameRule_0);
}

inline
ATermAppl gsMakeActionRenameSpec(const ATermAppl& DataSpec_0, const ATermAppl& ActSpec_1, const ATermAppl& ActionRenameRules_2)
{
  return ATmakeAppl3(function_symbol_ActionRenameSpec(), DataSpec_0, ActSpec_1, ActionRenameRules_2);
}

inline
ATermAppl gsMakeAllow(const ATermList& MultActName_0, const ATermAppl& ProcExpr_1)
{
  return ATmakeAppl2(function_symbol_Allow(), MultActName_0, ProcExpr_1);
}

inline
ATermAppl gsMakeAtTime(const ATermAppl& ProcExpr_0, const ATermAppl& DataExpr_1)
{
  return ATmakeAppl2(function_symbol_AtTime(), ProcExpr_0, DataExpr_1);
}

inline
ATermAppl gsMakeBES(const ATermList& BooleanEquation_0, const ATermAppl& BooleanExpression_1)
{
  return ATmakeAppl2(function_symbol_BES(), BooleanEquation_0, BooleanExpression_1);
}

inline
ATermAppl gsMakeBInit(const ATermAppl& ProcExpr_0, const ATermAppl& ProcExpr_1)
{
  return ATmakeAppl2(function_symbol_BInit(), ProcExpr_0, ProcExpr_1);
}

inline
ATermAppl gsMakeBagComp()
{
  return ATmakeAppl0(function_symbol_BagComp());
}

inline
ATermAppl gsMakeBinder(const ATermAppl& BindingOperator_0, const ATermList& DataVarId_1, const ATermAppl& DataExpr_2)
{
  return ATmakeAppl3(function_symbol_Binder(), BindingOperator_0, DataVarId_1, DataExpr_2);
}

inline
ATermAppl gsMakeBlock(const ATermList& String_0, const ATermAppl& ProcExpr_1)
{
  return ATmakeAppl2(function_symbol_Block(), String_0, ProcExpr_1);
}

inline
ATermAppl gsMakeBooleanAnd(const ATermAppl& BooleanExpression_0, const ATermAppl& BooleanExpression_1)
{
  return ATmakeAppl2(function_symbol_BooleanAnd(), BooleanExpression_0, BooleanExpression_1);
}

inline
ATermAppl gsMakeBooleanEquation(const ATermAppl& FixPoint_0, const ATermAppl& BooleanVariable_1, const ATermAppl& BooleanExpression_2)
{
  return ATmakeAppl3(function_symbol_BooleanEquation(), FixPoint_0, BooleanVariable_1, BooleanExpression_2);
}

inline
ATermAppl gsMakeBooleanFalse()
{
  return ATmakeAppl0(function_symbol_BooleanFalse());
}

inline
ATermAppl gsMakeBooleanImp(const ATermAppl& BooleanExpression_0, const ATermAppl& BooleanExpression_1)
{
  return ATmakeAppl2(function_symbol_BooleanImp(), BooleanExpression_0, BooleanExpression_1);
}

inline
ATermAppl gsMakeBooleanNot(const ATermAppl& BooleanExpression_0)
{
  return ATmakeAppl1(function_symbol_BooleanNot(), BooleanExpression_0);
}

inline
ATermAppl gsMakeBooleanOr(const ATermAppl& BooleanExpression_0, const ATermAppl& BooleanExpression_1)
{
  return ATmakeAppl2(function_symbol_BooleanOr(), BooleanExpression_0, BooleanExpression_1);
}

inline
ATermAppl gsMakeBooleanTrue()
{
  return ATmakeAppl0(function_symbol_BooleanTrue());
}

inline
ATermAppl gsMakeBooleanVariable(const ATermAppl& String_0)
{
  return ATmakeAppl1(function_symbol_BooleanVariable(), String_0);
}

inline
ATermAppl gsMakeChoice(const ATermAppl& ProcExpr_0, const ATermAppl& ProcExpr_1)
{
  return ATmakeAppl2(function_symbol_Choice(), ProcExpr_0, ProcExpr_1);
}

inline
ATermAppl gsMakeComm(const ATermList& CommExpr_0, const ATermAppl& ProcExpr_1)
{
  return ATmakeAppl2(function_symbol_Comm(), CommExpr_0, ProcExpr_1);
}

inline
ATermAppl gsMakeCommExpr(const ATermAppl& MultActName_0, const ATermAppl& StringOrNil_1)
{
  return ATmakeAppl2(function_symbol_CommExpr(), MultActName_0, StringOrNil_1);
}

inline
ATermAppl gsMakeConsSpec(const ATermList& OpId_0)
{
  return ATmakeAppl1(function_symbol_ConsSpec(), OpId_0);
}

inline
ATermAppl gsMakeDataAppl(const ATermAppl& DataExpr_0, const ATermList& DataExpr_1)
{
  return ATmakeAppl2(function_symbol_DataAppl(), DataExpr_0, DataExpr_1);
}

inline
ATermAppl gsMakeDataEqn(const ATermList& DataVarId_0, const ATermAppl& DataExpr_1, const ATermAppl& DataExpr_2, const ATermAppl& DataExpr_3)
{
  return ATmakeAppl4(function_symbol_DataEqn(), DataVarId_0, DataExpr_1, DataExpr_2, DataExpr_3);
}

inline
ATermAppl gsMakeDataEqnSpec(const ATermList& DataEqn_0)
{
  return ATmakeAppl1(function_symbol_DataEqnSpec(), DataEqn_0);
}

inline
ATermAppl gsMakeDataSpec(const ATermAppl& SortSpec_0, const ATermAppl& ConsSpec_1, const ATermAppl& MapSpec_2, const ATermAppl& DataEqnSpec_3)
{
  return ATmakeAppl4(function_symbol_DataSpec(), SortSpec_0, ConsSpec_1, MapSpec_2, DataEqnSpec_3);
}

inline
ATermAppl gsMakeDataVarId(const ATermAppl& String_0, const ATermAppl& SortExpr_1)
{
  return ATmakeAppl2(function_symbol_DataVarId(), String_0, SortExpr_1);
}

inline
ATermAppl gsMakeDataVarIdInit(const ATermAppl& DataVarId_0, const ATermAppl& DataExpr_1)
{
  return ATmakeAppl2(function_symbol_DataVarIdInit(), DataVarId_0, DataExpr_1);
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
ATermAppl gsMakeGlobVarSpec(const ATermList& DataVarId_0)
{
  return ATmakeAppl1(function_symbol_GlobVarSpec(), DataVarId_0);
}

inline
ATermAppl gsMakeHide(const ATermList& String_0, const ATermAppl& ProcExpr_1)
{
  return ATmakeAppl2(function_symbol_Hide(), String_0, ProcExpr_1);
}

inline
ATermAppl gsMakeId(const ATermAppl& String_0)
{
  return ATmakeAppl1(function_symbol_Id(), String_0);
}

inline
ATermAppl gsMakeIdAssignment(const ATermAppl& String_0, const ATermList& IdInit_1)
{
  return ATmakeAppl2(function_symbol_IdAssignment(), String_0, IdInit_1);
}

inline
ATermAppl gsMakeIdInit(const ATermAppl& String_0, const ATermAppl& DataExpr_1)
{
  return ATmakeAppl2(function_symbol_IdInit(), String_0, DataExpr_1);
}

inline
ATermAppl gsMakeIfThen(const ATermAppl& DataExpr_0, const ATermAppl& ProcExpr_1)
{
  return ATmakeAppl2(function_symbol_IfThen(), DataExpr_0, ProcExpr_1);
}

inline
ATermAppl gsMakeIfThenElse(const ATermAppl& DataExpr_0, const ATermAppl& ProcExpr_1, const ATermAppl& ProcExpr_2)
{
  return ATmakeAppl3(function_symbol_IfThenElse(), DataExpr_0, ProcExpr_1, ProcExpr_2);
}

inline
ATermAppl gsMakeLMerge(const ATermAppl& ProcExpr_0, const ATermAppl& ProcExpr_1)
{
  return ATmakeAppl2(function_symbol_LMerge(), ProcExpr_0, ProcExpr_1);
}

inline
ATermAppl gsMakeLambda()
{
  return ATmakeAppl0(function_symbol_Lambda());
}

inline
ATermAppl gsMakeLinProcSpec(const ATermAppl& DataSpec_0, const ATermAppl& ActSpec_1, const ATermAppl& GlobVarSpec_2, const ATermAppl& LinearProcess_3, const ATermAppl& LinearProcessInit_4)
{
  return ATmakeAppl5(function_symbol_LinProcSpec(), DataSpec_0, ActSpec_1, GlobVarSpec_2, LinearProcess_3, LinearProcessInit_4);
}

inline
ATermAppl gsMakeLinearProcess(const ATermList& DataVarId_0, const ATermList& LinearProcessSummand_1)
{
  return ATmakeAppl2(function_symbol_LinearProcess(), DataVarId_0, LinearProcessSummand_1);
}

inline
ATermAppl gsMakeLinearProcessInit(const ATermList& DataVarIdInit_0)
{
  return ATmakeAppl1(function_symbol_LinearProcessInit(), DataVarIdInit_0);
}

inline
ATermAppl gsMakeLinearProcessSummand(const ATermList& DataVarId_0, const ATermAppl& DataExpr_1, const ATermAppl& MultActOrDelta_2, const ATermAppl& DataExprOrNil_3, const ATermList& DataVarIdInit_4)
{
  return ATmakeAppl5(function_symbol_LinearProcessSummand(), DataVarId_0, DataExpr_1, MultActOrDelta_2, DataExprOrNil_3, DataVarIdInit_4);
}

inline
ATermAppl gsMakeMapSpec(const ATermList& OpId_0)
{
  return ATmakeAppl1(function_symbol_MapSpec(), OpId_0);
}

inline
ATermAppl gsMakeMerge(const ATermAppl& ProcExpr_0, const ATermAppl& ProcExpr_1)
{
  return ATmakeAppl2(function_symbol_Merge(), ProcExpr_0, ProcExpr_1);
}

inline
ATermAppl gsMakeMu()
{
  return ATmakeAppl0(function_symbol_Mu());
}

inline
ATermAppl gsMakeMultAct(const ATermList& ParamIdOrAction_0)
{
  return ATmakeAppl1(function_symbol_MultAct(), ParamIdOrAction_0);
}

inline
ATermAppl gsMakeMultActName(const ATermList& String_0)
{
  return ATmakeAppl1(function_symbol_MultActName(), String_0);
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
ATermAppl gsMakeOpId(const ATermAppl& String_0, const ATermAppl& SortExpr_1)
{
  return ATmakeAppl2(function_symbol_OpId(), String_0, SortExpr_1);
}

inline
ATermAppl gsMakePBES(const ATermAppl& DataSpec_0, const ATermAppl& GlobVarSpec_1, const ATermAppl& PBEqnSpec_2, const ATermAppl& PBInit_3)
{
  return ATmakeAppl4(function_symbol_PBES(), DataSpec_0, GlobVarSpec_1, PBEqnSpec_2, PBInit_3);
}

inline
ATermAppl gsMakePBESAnd(const ATermAppl& PBExpr_0, const ATermAppl& PBExpr_1)
{
  return ATmakeAppl2(function_symbol_PBESAnd(), PBExpr_0, PBExpr_1);
}

inline
ATermAppl gsMakePBESExists(const ATermList& DataVarId_0, const ATermAppl& PBExpr_1)
{
  return ATmakeAppl2(function_symbol_PBESExists(), DataVarId_0, PBExpr_1);
}

inline
ATermAppl gsMakePBESFalse()
{
  return ATmakeAppl0(function_symbol_PBESFalse());
}

inline
ATermAppl gsMakePBESForall(const ATermList& DataVarId_0, const ATermAppl& PBExpr_1)
{
  return ATmakeAppl2(function_symbol_PBESForall(), DataVarId_0, PBExpr_1);
}

inline
ATermAppl gsMakePBESImp(const ATermAppl& PBExpr_0, const ATermAppl& PBExpr_1)
{
  return ATmakeAppl2(function_symbol_PBESImp(), PBExpr_0, PBExpr_1);
}

inline
ATermAppl gsMakePBESNot(const ATermAppl& PBExpr_0)
{
  return ATmakeAppl1(function_symbol_PBESNot(), PBExpr_0);
}

inline
ATermAppl gsMakePBESOr(const ATermAppl& PBExpr_0, const ATermAppl& PBExpr_1)
{
  return ATmakeAppl2(function_symbol_PBESOr(), PBExpr_0, PBExpr_1);
}

inline
ATermAppl gsMakePBESTrue()
{
  return ATmakeAppl0(function_symbol_PBESTrue());
}

inline
ATermAppl gsMakePBEqn(const ATermAppl& FixPoint_0, const ATermAppl& PropVarDecl_1, const ATermAppl& PBExpr_2)
{
  return ATmakeAppl3(function_symbol_PBEqn(), FixPoint_0, PropVarDecl_1, PBExpr_2);
}

inline
ATermAppl gsMakePBEqnSpec(const ATermList& PBEqn_0)
{
  return ATmakeAppl1(function_symbol_PBEqnSpec(), PBEqn_0);
}

inline
ATermAppl gsMakePBInit(const ATermAppl& PropVarInst_0)
{
  return ATmakeAppl1(function_symbol_PBInit(), PropVarInst_0);
}

inline
ATermAppl gsMakeParamId(const ATermAppl& String_0, const ATermList& DataExpr_1)
{
  return ATmakeAppl2(function_symbol_ParamId(), String_0, DataExpr_1);
}

inline
ATermAppl gsMakeProcEqn(const ATermAppl& ProcVarId_0, const ATermList& DataVarId_1, const ATermAppl& ProcExpr_2)
{
  return ATmakeAppl3(function_symbol_ProcEqn(), ProcVarId_0, DataVarId_1, ProcExpr_2);
}

inline
ATermAppl gsMakeProcEqnSpec(const ATermList& ProcEqn_0)
{
  return ATmakeAppl1(function_symbol_ProcEqnSpec(), ProcEqn_0);
}

inline
ATermAppl gsMakeProcSpec(const ATermAppl& DataSpec_0, const ATermAppl& ActSpec_1, const ATermAppl& GlobVarSpec_2, const ATermAppl& ProcEqnSpec_3, const ATermAppl& ProcInit_4)
{
  return ATmakeAppl5(function_symbol_ProcSpec(), DataSpec_0, ActSpec_1, GlobVarSpec_2, ProcEqnSpec_3, ProcInit_4);
}

inline
ATermAppl gsMakeProcVarId(const ATermAppl& String_0, const ATermList& SortExpr_1)
{
  return ATmakeAppl2(function_symbol_ProcVarId(), String_0, SortExpr_1);
}

inline
ATermAppl gsMakeProcess(const ATermAppl& ProcVarId_0, const ATermList& DataExpr_1)
{
  return ATmakeAppl2(function_symbol_Process(), ProcVarId_0, DataExpr_1);
}

inline
ATermAppl gsMakeProcessAssignment(const ATermAppl& ProcVarId_0, const ATermList& DataVarIdInit_1)
{
  return ATmakeAppl2(function_symbol_ProcessAssignment(), ProcVarId_0, DataVarIdInit_1);
}

inline
ATermAppl gsMakeProcessInit(const ATermAppl& ProcExpr_0)
{
  return ATmakeAppl1(function_symbol_ProcessInit(), ProcExpr_0);
}

inline
ATermAppl gsMakePropVarDecl(const ATermAppl& String_0, const ATermList& DataVarId_1)
{
  return ATmakeAppl2(function_symbol_PropVarDecl(), String_0, DataVarId_1);
}

inline
ATermAppl gsMakePropVarInst(const ATermAppl& String_0, const ATermList& DataExpr_1)
{
  return ATmakeAppl2(function_symbol_PropVarInst(), String_0, DataExpr_1);
}

inline
ATermAppl gsMakeRegAlt(const ATermAppl& RegFrm_0, const ATermAppl& RegFrm_1)
{
  return ATmakeAppl2(function_symbol_RegAlt(), RegFrm_0, RegFrm_1);
}

inline
ATermAppl gsMakeRegNil()
{
  return ATmakeAppl0(function_symbol_RegNil());
}

inline
ATermAppl gsMakeRegSeq(const ATermAppl& RegFrm_0, const ATermAppl& RegFrm_1)
{
  return ATmakeAppl2(function_symbol_RegSeq(), RegFrm_0, RegFrm_1);
}

inline
ATermAppl gsMakeRegTrans(const ATermAppl& RegFrm_0)
{
  return ATmakeAppl1(function_symbol_RegTrans(), RegFrm_0);
}

inline
ATermAppl gsMakeRegTransOrNil(const ATermAppl& RegFrm_0)
{
  return ATmakeAppl1(function_symbol_RegTransOrNil(), RegFrm_0);
}

inline
ATermAppl gsMakeRename(const ATermList& RenameExpr_0, const ATermAppl& ProcExpr_1)
{
  return ATmakeAppl2(function_symbol_Rename(), RenameExpr_0, ProcExpr_1);
}

inline
ATermAppl gsMakeRenameExpr(const ATermAppl& String_0, const ATermAppl& String_1)
{
  return ATmakeAppl2(function_symbol_RenameExpr(), String_0, String_1);
}

inline
ATermAppl gsMakeSeq(const ATermAppl& ProcExpr_0, const ATermAppl& ProcExpr_1)
{
  return ATmakeAppl2(function_symbol_Seq(), ProcExpr_0, ProcExpr_1);
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
ATermAppl gsMakeSortArrow(const ATermList& SortExpr_0, const ATermAppl& SortExpr_1)
{
  return ATmakeAppl2(function_symbol_SortArrow(), SortExpr_0, SortExpr_1);
}

inline
ATermAppl gsMakeSortBag()
{
  return ATmakeAppl0(function_symbol_SortBag());
}

inline
ATermAppl gsMakeSortCons(const ATermAppl& SortConsType_0, const ATermAppl& SortExpr_1)
{
  return ATmakeAppl2(function_symbol_SortCons(), SortConsType_0, SortExpr_1);
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
ATermAppl gsMakeSortId(const ATermAppl& String_0)
{
  return ATmakeAppl1(function_symbol_SortId(), String_0);
}

inline
ATermAppl gsMakeSortList()
{
  return ATmakeAppl0(function_symbol_SortList());
}

inline
ATermAppl gsMakeSortRef(const ATermAppl& String_0, const ATermAppl& SortExpr_1)
{
  return ATmakeAppl2(function_symbol_SortRef(), String_0, SortExpr_1);
}

inline
ATermAppl gsMakeSortSet()
{
  return ATmakeAppl0(function_symbol_SortSet());
}

inline
ATermAppl gsMakeSortSpec(const ATermList& SortDecl_0)
{
  return ATmakeAppl1(function_symbol_SortSpec(), SortDecl_0);
}

inline
ATermAppl gsMakeSortStruct(const ATermList& StructCons_0)
{
  return ATmakeAppl1(function_symbol_SortStruct(), StructCons_0);
}

inline
ATermAppl gsMakeSortUnknown()
{
  return ATmakeAppl0(function_symbol_SortUnknown());
}

inline
ATermAppl gsMakeSortsPossible(const ATermList& SortExpr_0)
{
  return ATmakeAppl1(function_symbol_SortsPossible(), SortExpr_0);
}

inline
ATermAppl gsMakeStateAnd(const ATermAppl& StateFrm_0, const ATermAppl& StateFrm_1)
{
  return ATmakeAppl2(function_symbol_StateAnd(), StateFrm_0, StateFrm_1);
}

inline
ATermAppl gsMakeStateDelay()
{
  return ATmakeAppl0(function_symbol_StateDelay());
}

inline
ATermAppl gsMakeStateDelayTimed(const ATermAppl& DataExpr_0)
{
  return ATmakeAppl1(function_symbol_StateDelayTimed(), DataExpr_0);
}

inline
ATermAppl gsMakeStateExists(const ATermList& DataVarId_0, const ATermAppl& StateFrm_1)
{
  return ATmakeAppl2(function_symbol_StateExists(), DataVarId_0, StateFrm_1);
}

inline
ATermAppl gsMakeStateFalse()
{
  return ATmakeAppl0(function_symbol_StateFalse());
}

inline
ATermAppl gsMakeStateForall(const ATermList& DataVarId_0, const ATermAppl& StateFrm_1)
{
  return ATmakeAppl2(function_symbol_StateForall(), DataVarId_0, StateFrm_1);
}

inline
ATermAppl gsMakeStateImp(const ATermAppl& StateFrm_0, const ATermAppl& StateFrm_1)
{
  return ATmakeAppl2(function_symbol_StateImp(), StateFrm_0, StateFrm_1);
}

inline
ATermAppl gsMakeStateMay(const ATermAppl& RegFrm_0, const ATermAppl& StateFrm_1)
{
  return ATmakeAppl2(function_symbol_StateMay(), RegFrm_0, StateFrm_1);
}

inline
ATermAppl gsMakeStateMu(const ATermAppl& String_0, const ATermList& DataVarIdInit_1, const ATermAppl& StateFrm_2)
{
  return ATmakeAppl3(function_symbol_StateMu(), String_0, DataVarIdInit_1, StateFrm_2);
}

inline
ATermAppl gsMakeStateMust(const ATermAppl& RegFrm_0, const ATermAppl& StateFrm_1)
{
  return ATmakeAppl2(function_symbol_StateMust(), RegFrm_0, StateFrm_1);
}

inline
ATermAppl gsMakeStateNot(const ATermAppl& StateFrm_0)
{
  return ATmakeAppl1(function_symbol_StateNot(), StateFrm_0);
}

inline
ATermAppl gsMakeStateNu(const ATermAppl& String_0, const ATermList& DataVarIdInit_1, const ATermAppl& StateFrm_2)
{
  return ATmakeAppl3(function_symbol_StateNu(), String_0, DataVarIdInit_1, StateFrm_2);
}

inline
ATermAppl gsMakeStateOr(const ATermAppl& StateFrm_0, const ATermAppl& StateFrm_1)
{
  return ATmakeAppl2(function_symbol_StateOr(), StateFrm_0, StateFrm_1);
}

inline
ATermAppl gsMakeStateTrue()
{
  return ATmakeAppl0(function_symbol_StateTrue());
}

inline
ATermAppl gsMakeStateVar(const ATermAppl& String_0, const ATermList& DataExpr_1)
{
  return ATmakeAppl2(function_symbol_StateVar(), String_0, DataExpr_1);
}

inline
ATermAppl gsMakeStateYaled()
{
  return ATmakeAppl0(function_symbol_StateYaled());
}

inline
ATermAppl gsMakeStateYaledTimed(const ATermAppl& DataExpr_0)
{
  return ATmakeAppl1(function_symbol_StateYaledTimed(), DataExpr_0);
}

inline
ATermAppl gsMakeStructCons(const ATermAppl& String_0, const ATermList& StructProj_1, const ATermAppl& StringOrNil_2)
{
  return ATmakeAppl3(function_symbol_StructCons(), String_0, StructProj_1, StringOrNil_2);
}

inline
ATermAppl gsMakeStructProj(const ATermAppl& StringOrNil_0, const ATermAppl& SortExpr_1)
{
  return ATmakeAppl2(function_symbol_StructProj(), StringOrNil_0, SortExpr_1);
}

inline
ATermAppl gsMakeSum(const ATermList& DataVarId_0, const ATermAppl& ProcExpr_1)
{
  return ATmakeAppl2(function_symbol_Sum(), DataVarId_0, ProcExpr_1);
}

inline
ATermAppl gsMakeSync(const ATermAppl& ProcExpr_0, const ATermAppl& ProcExpr_1)
{
  return ATmakeAppl2(function_symbol_Sync(), ProcExpr_0, ProcExpr_1);
}

inline
ATermAppl gsMakeTau()
{
  return ATmakeAppl0(function_symbol_Tau());
}

inline
ATermAppl gsMakeWhr(const ATermAppl& DataExpr_0, const ATermList& WhrDecl_1)
{
  return ATmakeAppl2(function_symbol_Whr(), DataExpr_0, WhrDecl_1);
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
