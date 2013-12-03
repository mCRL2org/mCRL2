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

//This file describes the functions that can be used for the internal aterm
//structure.

#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "mcrl2/atermpp/aterm_int.h"
#include "mcrl2/atermpp/aterm_list.h"

namespace mcrl2
{
namespace core
{
namespace detail
{

using namespace atermpp;

inline
bool operator==(const size_t x, const atermpp::function_symbol& y)
{
  // return x.name() == y.name() && x.arity() == y.arity() && x.is_quoted() == y.is_quoted();
  return x == function_symbol(y).number();
}

//Global precondition: the aterm library has been initialised

// DataAppl
extern std::vector<atermpp::function_symbol> function_symbols_DataAppl_;

inline std::vector<atermpp::function_symbol>& function_symbols_DataAppl()
{
  // static std::vector<atermpp::function_symbol> function_symbols_DataAppl;
  return function_symbols_DataAppl_;
}

inline
const atermpp::function_symbol& function_symbol_DataAppl_helper(size_t i)
{
  std::vector<atermpp::function_symbol>& syms = function_symbols_DataAppl();
  do 
  {
    syms.push_back(atermpp::function_symbol("DataAppl", syms.size()));
  }
  while (i >= syms.size());
  return syms[i];
}

inline
const atermpp::function_symbol& function_symbol_DataAppl(size_t i)
{
  std::vector<atermpp::function_symbol>& syms = function_symbols_DataAppl();
  if (i<syms.size()) 
  { 
    return syms[i];
  }
  return function_symbol_DataAppl_helper(i);
}

inline
bool gsIsDataAppl(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_DataAppl(Term.function().arity());
}

// DataVarIdNoIndex
inline
const atermpp::function_symbol& function_symbol_DataVarIdNoIndex()
{
  static atermpp::function_symbol f = atermpp::function_symbol("DataVarIdNoIndex", 2);
  return f;
}

// OpIdIndex
inline
const atermpp::function_symbol& function_symbol_OpIdNoIndex()
{
  static atermpp::function_symbol f = atermpp::function_symbol("OpIdNoIndex", 2);
  return f;
}

// ProcVarIdNoIndex
inline
const atermpp::function_symbol& function_symbol_ProcVarIdNoIndex()
{
  static atermpp::function_symbol f = atermpp::function_symbol("ProcVarIdNoIndex", 2);
  return f;
}

// BooleanVariableNoIndex
inline
const atermpp::function_symbol& function_symbol_BooleanVariableNoIndex()
{
  static atermpp::function_symbol f = atermpp::function_symbol("BooleanVariableNoIndex", 1);
  return f;
}

// PropVarInstNoIndex
inline
const atermpp::function_symbol& function_symbol_PropVarInstNoIndex()
{
  static atermpp::function_symbol f = atermpp::function_symbol("PropVarInstNoIndex", 2);
  return f;
}

//--- start generated code ---//
// ActAnd
inline
const atermpp::function_symbol& function_symbol_ActAnd()
{
  static atermpp::function_symbol function_symbol_ActAnd = atermpp::function_symbol("ActAnd", 2);
  return function_symbol_ActAnd;
}

inline
bool gsIsActAnd(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_ActAnd();
}

// ActAt
inline
const atermpp::function_symbol& function_symbol_ActAt()
{
  static atermpp::function_symbol function_symbol_ActAt = atermpp::function_symbol("ActAt", 2);
  return function_symbol_ActAt;
}

inline
bool gsIsActAt(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_ActAt();
}

// ActExists
inline
const atermpp::function_symbol& function_symbol_ActExists()
{
  static atermpp::function_symbol function_symbol_ActExists = atermpp::function_symbol("ActExists", 2);
  return function_symbol_ActExists;
}

inline
bool gsIsActExists(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_ActExists();
}

// ActFalse
inline
const atermpp::function_symbol& function_symbol_ActFalse()
{
  static atermpp::function_symbol function_symbol_ActFalse = atermpp::function_symbol("ActFalse", 0);
  return function_symbol_ActFalse;
}

inline
bool gsIsActFalse(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_ActFalse();
}

// ActForall
inline
const atermpp::function_symbol& function_symbol_ActForall()
{
  static atermpp::function_symbol function_symbol_ActForall = atermpp::function_symbol("ActForall", 2);
  return function_symbol_ActForall;
}

inline
bool gsIsActForall(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_ActForall();
}

// ActId
inline
const atermpp::function_symbol& function_symbol_ActId()
{
  static atermpp::function_symbol function_symbol_ActId = atermpp::function_symbol("ActId", 2);
  return function_symbol_ActId;
}

inline
bool gsIsActId(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_ActId();
}

// ActImp
inline
const atermpp::function_symbol& function_symbol_ActImp()
{
  static atermpp::function_symbol function_symbol_ActImp = atermpp::function_symbol("ActImp", 2);
  return function_symbol_ActImp;
}

inline
bool gsIsActImp(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_ActImp();
}

// ActMultAct
inline
const atermpp::function_symbol& function_symbol_ActMultAct()
{
  static atermpp::function_symbol function_symbol_ActMultAct = atermpp::function_symbol("ActMultAct", 1);
  return function_symbol_ActMultAct;
}

inline
bool gsIsActMultAct(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_ActMultAct();
}

// ActNot
inline
const atermpp::function_symbol& function_symbol_ActNot()
{
  static atermpp::function_symbol function_symbol_ActNot = atermpp::function_symbol("ActNot", 1);
  return function_symbol_ActNot;
}

inline
bool gsIsActNot(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_ActNot();
}

// ActOr
inline
const atermpp::function_symbol& function_symbol_ActOr()
{
  static atermpp::function_symbol function_symbol_ActOr = atermpp::function_symbol("ActOr", 2);
  return function_symbol_ActOr;
}

inline
bool gsIsActOr(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_ActOr();
}

// ActSpec
inline
const atermpp::function_symbol& function_symbol_ActSpec()
{
  static atermpp::function_symbol function_symbol_ActSpec = atermpp::function_symbol("ActSpec", 1);
  return function_symbol_ActSpec;
}

inline
bool gsIsActSpec(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_ActSpec();
}

// ActTrue
inline
const atermpp::function_symbol& function_symbol_ActTrue()
{
  static atermpp::function_symbol function_symbol_ActTrue = atermpp::function_symbol("ActTrue", 0);
  return function_symbol_ActTrue;
}

inline
bool gsIsActTrue(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_ActTrue();
}

// Action
inline
const atermpp::function_symbol& function_symbol_Action()
{
  static atermpp::function_symbol function_symbol_Action = atermpp::function_symbol("Action", 2);
  return function_symbol_Action;
}

inline
bool gsIsAction(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_Action();
}

// ActionRenameRule
inline
const atermpp::function_symbol& function_symbol_ActionRenameRule()
{
  static atermpp::function_symbol function_symbol_ActionRenameRule = atermpp::function_symbol("ActionRenameRule", 4);
  return function_symbol_ActionRenameRule;
}

inline
bool gsIsActionRenameRule(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_ActionRenameRule();
}

// ActionRenameRules
inline
const atermpp::function_symbol& function_symbol_ActionRenameRules()
{
  static atermpp::function_symbol function_symbol_ActionRenameRules = atermpp::function_symbol("ActionRenameRules", 1);
  return function_symbol_ActionRenameRules;
}

inline
bool gsIsActionRenameRules(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_ActionRenameRules();
}

// ActionRenameSpec
inline
const atermpp::function_symbol& function_symbol_ActionRenameSpec()
{
  static atermpp::function_symbol function_symbol_ActionRenameSpec = atermpp::function_symbol("ActionRenameSpec", 3);
  return function_symbol_ActionRenameSpec;
}

inline
bool gsIsActionRenameSpec(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_ActionRenameSpec();
}

// Allow
inline
const atermpp::function_symbol& function_symbol_Allow()
{
  static atermpp::function_symbol function_symbol_Allow = atermpp::function_symbol("Allow", 2);
  return function_symbol_Allow;
}

inline
bool gsIsAllow(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_Allow();
}

// AtTime
inline
const atermpp::function_symbol& function_symbol_AtTime()
{
  static atermpp::function_symbol function_symbol_AtTime = atermpp::function_symbol("AtTime", 2);
  return function_symbol_AtTime;
}

inline
bool gsIsAtTime(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_AtTime();
}

// BES
inline
const atermpp::function_symbol& function_symbol_BES()
{
  static atermpp::function_symbol function_symbol_BES = atermpp::function_symbol("BES", 2);
  return function_symbol_BES;
}

inline
bool gsIsBES(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_BES();
}

// BInit
inline
const atermpp::function_symbol& function_symbol_BInit()
{
  static atermpp::function_symbol function_symbol_BInit = atermpp::function_symbol("BInit", 2);
  return function_symbol_BInit;
}

inline
bool gsIsBInit(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_BInit();
}

// BagComp
inline
const atermpp::function_symbol& function_symbol_BagComp()
{
  static atermpp::function_symbol function_symbol_BagComp = atermpp::function_symbol("BagComp", 0);
  return function_symbol_BagComp;
}

inline
bool gsIsBagComp(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_BagComp();
}

// Binder
inline
const atermpp::function_symbol& function_symbol_Binder()
{
  static atermpp::function_symbol function_symbol_Binder = atermpp::function_symbol("Binder", 3);
  return function_symbol_Binder;
}

inline
bool gsIsBinder(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_Binder();
}

// Block
inline
const atermpp::function_symbol& function_symbol_Block()
{
  static atermpp::function_symbol function_symbol_Block = atermpp::function_symbol("Block", 2);
  return function_symbol_Block;
}

inline
bool gsIsBlock(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_Block();
}

// BooleanAnd
inline
const atermpp::function_symbol& function_symbol_BooleanAnd()
{
  static atermpp::function_symbol function_symbol_BooleanAnd = atermpp::function_symbol("BooleanAnd", 2);
  return function_symbol_BooleanAnd;
}

inline
bool gsIsBooleanAnd(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_BooleanAnd();
}

// BooleanEquation
inline
const atermpp::function_symbol& function_symbol_BooleanEquation()
{
  static atermpp::function_symbol function_symbol_BooleanEquation = atermpp::function_symbol("BooleanEquation", 3);
  return function_symbol_BooleanEquation;
}

inline
bool gsIsBooleanEquation(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_BooleanEquation();
}

// BooleanFalse
inline
const atermpp::function_symbol& function_symbol_BooleanFalse()
{
  static atermpp::function_symbol function_symbol_BooleanFalse = atermpp::function_symbol("BooleanFalse", 0);
  return function_symbol_BooleanFalse;
}

inline
bool gsIsBooleanFalse(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_BooleanFalse();
}

// BooleanImp
inline
const atermpp::function_symbol& function_symbol_BooleanImp()
{
  static atermpp::function_symbol function_symbol_BooleanImp = atermpp::function_symbol("BooleanImp", 2);
  return function_symbol_BooleanImp;
}

inline
bool gsIsBooleanImp(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_BooleanImp();
}

// BooleanNot
inline
const atermpp::function_symbol& function_symbol_BooleanNot()
{
  static atermpp::function_symbol function_symbol_BooleanNot = atermpp::function_symbol("BooleanNot", 1);
  return function_symbol_BooleanNot;
}

inline
bool gsIsBooleanNot(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_BooleanNot();
}

// BooleanOr
inline
const atermpp::function_symbol& function_symbol_BooleanOr()
{
  static atermpp::function_symbol function_symbol_BooleanOr = atermpp::function_symbol("BooleanOr", 2);
  return function_symbol_BooleanOr;
}

inline
bool gsIsBooleanOr(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_BooleanOr();
}

// BooleanTrue
inline
const atermpp::function_symbol& function_symbol_BooleanTrue()
{
  static atermpp::function_symbol function_symbol_BooleanTrue = atermpp::function_symbol("BooleanTrue", 0);
  return function_symbol_BooleanTrue;
}

inline
bool gsIsBooleanTrue(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_BooleanTrue();
}

// BooleanVariable
inline
const atermpp::function_symbol& function_symbol_BooleanVariable()
{
  static atermpp::function_symbol function_symbol_BooleanVariable = atermpp::function_symbol("BooleanVariable", 2);
  return function_symbol_BooleanVariable;
}

inline
bool gsIsBooleanVariable(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_BooleanVariable();
}

// Choice
inline
const atermpp::function_symbol& function_symbol_Choice()
{
  static atermpp::function_symbol function_symbol_Choice = atermpp::function_symbol("Choice", 2);
  return function_symbol_Choice;
}

inline
bool gsIsChoice(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_Choice();
}

// Comm
inline
const atermpp::function_symbol& function_symbol_Comm()
{
  static atermpp::function_symbol function_symbol_Comm = atermpp::function_symbol("Comm", 2);
  return function_symbol_Comm;
}

inline
bool gsIsComm(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_Comm();
}

// CommExpr
inline
const atermpp::function_symbol& function_symbol_CommExpr()
{
  static atermpp::function_symbol function_symbol_CommExpr = atermpp::function_symbol("CommExpr", 2);
  return function_symbol_CommExpr;
}

inline
bool gsIsCommExpr(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_CommExpr();
}

// ConsSpec
inline
const atermpp::function_symbol& function_symbol_ConsSpec()
{
  static atermpp::function_symbol function_symbol_ConsSpec = atermpp::function_symbol("ConsSpec", 1);
  return function_symbol_ConsSpec;
}

inline
bool gsIsConsSpec(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_ConsSpec();
}

// DataEqn
inline
const atermpp::function_symbol& function_symbol_DataEqn()
{
  static atermpp::function_symbol function_symbol_DataEqn = atermpp::function_symbol("DataEqn", 4);
  return function_symbol_DataEqn;
}

inline
bool gsIsDataEqn(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_DataEqn();
}

// DataEqnSpec
inline
const atermpp::function_symbol& function_symbol_DataEqnSpec()
{
  static atermpp::function_symbol function_symbol_DataEqnSpec = atermpp::function_symbol("DataEqnSpec", 1);
  return function_symbol_DataEqnSpec;
}

inline
bool gsIsDataEqnSpec(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_DataEqnSpec();
}

// DataSpec
inline
const atermpp::function_symbol& function_symbol_DataSpec()
{
  static atermpp::function_symbol function_symbol_DataSpec = atermpp::function_symbol("DataSpec", 4);
  return function_symbol_DataSpec;
}

inline
bool gsIsDataSpec(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_DataSpec();
}

// DataVarId
extern atermpp::function_symbol function_symbol_DataVarId_;

inline
const atermpp::function_symbol& function_symbol_DataVarId()
{
  // static atermpp::function_symbol function_symbol_DataVarId = atermpp::function_symbol("DataVarId", 3);
  return function_symbol_DataVarId_;
}

inline
bool gsIsDataVarId(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_DataVarId();
}

// DataVarIdInit
inline
const atermpp::function_symbol& function_symbol_DataVarIdInit()
{
  static atermpp::function_symbol function_symbol_DataVarIdInit = atermpp::function_symbol("DataVarIdInit", 2);
  return function_symbol_DataVarIdInit;
}

inline
bool gsIsDataVarIdInit(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_DataVarIdInit();
}

// Delta
inline
const atermpp::function_symbol& function_symbol_Delta()
{
  static atermpp::function_symbol function_symbol_Delta = atermpp::function_symbol("Delta", 0);
  return function_symbol_Delta;
}

inline
bool gsIsDelta(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_Delta();
}

// Exists
inline
const atermpp::function_symbol& function_symbol_Exists()
{
  static atermpp::function_symbol function_symbol_Exists = atermpp::function_symbol("Exists", 0);
  return function_symbol_Exists;
}

inline
bool gsIsExists(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_Exists();
}

// Forall
inline
const atermpp::function_symbol& function_symbol_Forall()
{
  static atermpp::function_symbol function_symbol_Forall = atermpp::function_symbol("Forall", 0);
  return function_symbol_Forall;
}

inline
bool gsIsForall(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_Forall();
}

// GlobVarSpec
inline
const atermpp::function_symbol& function_symbol_GlobVarSpec()
{
  static atermpp::function_symbol function_symbol_GlobVarSpec = atermpp::function_symbol("GlobVarSpec", 1);
  return function_symbol_GlobVarSpec;
}

inline
bool gsIsGlobVarSpec(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_GlobVarSpec();
}

// Hide
inline
const atermpp::function_symbol& function_symbol_Hide()
{
  static atermpp::function_symbol function_symbol_Hide = atermpp::function_symbol("Hide", 2);
  return function_symbol_Hide;
}

inline
bool gsIsHide(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_Hide();
}

// IfThen
inline
const atermpp::function_symbol& function_symbol_IfThen()
{
  static atermpp::function_symbol function_symbol_IfThen = atermpp::function_symbol("IfThen", 2);
  return function_symbol_IfThen;
}

inline
bool gsIsIfThen(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_IfThen();
}

// IfThenElse
inline
const atermpp::function_symbol& function_symbol_IfThenElse()
{
  static atermpp::function_symbol function_symbol_IfThenElse = atermpp::function_symbol("IfThenElse", 3);
  return function_symbol_IfThenElse;
}

inline
bool gsIsIfThenElse(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_IfThenElse();
}

// LMerge
inline
const atermpp::function_symbol& function_symbol_LMerge()
{
  static atermpp::function_symbol function_symbol_LMerge = atermpp::function_symbol("LMerge", 2);
  return function_symbol_LMerge;
}

inline
bool gsIsLMerge(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_LMerge();
}

// Lambda
inline
const atermpp::function_symbol& function_symbol_Lambda()
{
  static atermpp::function_symbol function_symbol_Lambda = atermpp::function_symbol("Lambda", 0);
  return function_symbol_Lambda;
}

inline
bool gsIsLambda(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_Lambda();
}

// LinProcSpec
inline
const atermpp::function_symbol& function_symbol_LinProcSpec()
{
  static atermpp::function_symbol function_symbol_LinProcSpec = atermpp::function_symbol("LinProcSpec", 5);
  return function_symbol_LinProcSpec;
}

inline
bool gsIsLinProcSpec(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_LinProcSpec();
}

// LinearProcess
inline
const atermpp::function_symbol& function_symbol_LinearProcess()
{
  static atermpp::function_symbol function_symbol_LinearProcess = atermpp::function_symbol("LinearProcess", 2);
  return function_symbol_LinearProcess;
}

inline
bool gsIsLinearProcess(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_LinearProcess();
}

// LinearProcessInit
inline
const atermpp::function_symbol& function_symbol_LinearProcessInit()
{
  static atermpp::function_symbol function_symbol_LinearProcessInit = atermpp::function_symbol("LinearProcessInit", 1);
  return function_symbol_LinearProcessInit;
}

inline
bool gsIsLinearProcessInit(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_LinearProcessInit();
}

// LinearProcessSummand
inline
const atermpp::function_symbol& function_symbol_LinearProcessSummand()
{
  static atermpp::function_symbol function_symbol_LinearProcessSummand = atermpp::function_symbol("LinearProcessSummand", 5);
  return function_symbol_LinearProcessSummand;
}

inline
bool gsIsLinearProcessSummand(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_LinearProcessSummand();
}

// MapSpec
inline
const atermpp::function_symbol& function_symbol_MapSpec()
{
  static atermpp::function_symbol function_symbol_MapSpec = atermpp::function_symbol("MapSpec", 1);
  return function_symbol_MapSpec;
}

inline
bool gsIsMapSpec(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_MapSpec();
}

// Merge
inline
const atermpp::function_symbol& function_symbol_Merge()
{
  static atermpp::function_symbol function_symbol_Merge = atermpp::function_symbol("Merge", 2);
  return function_symbol_Merge;
}

inline
bool gsIsMerge(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_Merge();
}

// Mu
inline
const atermpp::function_symbol& function_symbol_Mu()
{
  static atermpp::function_symbol function_symbol_Mu = atermpp::function_symbol("Mu", 0);
  return function_symbol_Mu;
}

inline
bool gsIsMu(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_Mu();
}

// MultAct
inline
const atermpp::function_symbol& function_symbol_MultAct()
{
  static atermpp::function_symbol function_symbol_MultAct = atermpp::function_symbol("MultAct", 1);
  return function_symbol_MultAct;
}

inline
bool gsIsMultAct(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_MultAct();
}

// MultActName
inline
const atermpp::function_symbol& function_symbol_MultActName()
{
  static atermpp::function_symbol function_symbol_MultActName = atermpp::function_symbol("MultActName", 1);
  return function_symbol_MultActName;
}

inline
bool gsIsMultActName(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_MultActName();
}

// Nil
inline
const atermpp::function_symbol& function_symbol_Nil()
{
  static atermpp::function_symbol function_symbol_Nil = atermpp::function_symbol("Nil", 0);
  return function_symbol_Nil;
}

inline
bool gsIsNil(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_Nil();
}

// Nu
inline
const atermpp::function_symbol& function_symbol_Nu()
{
  static atermpp::function_symbol function_symbol_Nu = atermpp::function_symbol("Nu", 0);
  return function_symbol_Nu;
}

inline
bool gsIsNu(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_Nu();
}

// OpId
/* inline
const atermpp::function_symbol& function_symbol_OpId()
{
  static atermpp::function_symbol function_symbol_OpId = atermpp::function_symbol("OpId", 3);
  return function_symbol_OpId;
} */

extern atermpp::function_symbol function_symbol_OpId_; // = atermpp::function_symbol("OpId", 3);

inline
const atermpp::function_symbol& function_symbol_OpId()
{
  return function_symbol_OpId_;
}

inline
bool gsIsOpId(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_OpId();
} 

// PBES
inline
const atermpp::function_symbol& function_symbol_PBES()
{
  static atermpp::function_symbol function_symbol_PBES = atermpp::function_symbol("PBES", 4);
  return function_symbol_PBES;
}

inline
bool gsIsPBES(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_PBES();
}

// PBESAnd
inline
const atermpp::function_symbol& function_symbol_PBESAnd()
{
  static atermpp::function_symbol function_symbol_PBESAnd = atermpp::function_symbol("PBESAnd", 2);
  return function_symbol_PBESAnd;
}

inline
bool gsIsPBESAnd(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_PBESAnd();
}

// PBESExists
inline
const atermpp::function_symbol& function_symbol_PBESExists()
{
  static atermpp::function_symbol function_symbol_PBESExists = atermpp::function_symbol("PBESExists", 2);
  return function_symbol_PBESExists;
}

inline
bool gsIsPBESExists(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_PBESExists();
}

// PBESFalse
inline
const atermpp::function_symbol& function_symbol_PBESFalse()
{
  static atermpp::function_symbol function_symbol_PBESFalse = atermpp::function_symbol("PBESFalse", 0);
  return function_symbol_PBESFalse;
}

inline
bool gsIsPBESFalse(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_PBESFalse();
}

// PBESForall
inline
const atermpp::function_symbol& function_symbol_PBESForall()
{
  static atermpp::function_symbol function_symbol_PBESForall = atermpp::function_symbol("PBESForall", 2);
  return function_symbol_PBESForall;
}

inline
bool gsIsPBESForall(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_PBESForall();
}

// PBESImp
inline
const atermpp::function_symbol& function_symbol_PBESImp()
{
  static atermpp::function_symbol function_symbol_PBESImp = atermpp::function_symbol("PBESImp", 2);
  return function_symbol_PBESImp;
}

inline
bool gsIsPBESImp(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_PBESImp();
}

// PBESNot
inline
const atermpp::function_symbol& function_symbol_PBESNot()
{
  static atermpp::function_symbol function_symbol_PBESNot = atermpp::function_symbol("PBESNot", 1);
  return function_symbol_PBESNot;
}

inline
bool gsIsPBESNot(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_PBESNot();
}

// PBESOr
inline
const atermpp::function_symbol& function_symbol_PBESOr()
{
  static atermpp::function_symbol function_symbol_PBESOr = atermpp::function_symbol("PBESOr", 2);
  return function_symbol_PBESOr;
}

inline
bool gsIsPBESOr(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_PBESOr();
}

// PBESTrue
inline
const atermpp::function_symbol& function_symbol_PBESTrue()
{
  static atermpp::function_symbol function_symbol_PBESTrue = atermpp::function_symbol("PBESTrue", 0);
  return function_symbol_PBESTrue;
}

inline
bool gsIsPBESTrue(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_PBESTrue();
}

// PBEqn
inline
const atermpp::function_symbol& function_symbol_PBEqn()
{
  static atermpp::function_symbol function_symbol_PBEqn = atermpp::function_symbol("PBEqn", 3);
  return function_symbol_PBEqn;
}

inline
bool gsIsPBEqn(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_PBEqn();
}

// PBEqnSpec
inline
const atermpp::function_symbol& function_symbol_PBEqnSpec()
{
  static atermpp::function_symbol function_symbol_PBEqnSpec = atermpp::function_symbol("PBEqnSpec", 1);
  return function_symbol_PBEqnSpec;
}

inline
bool gsIsPBEqnSpec(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_PBEqnSpec();
}

// PBInit
inline
const atermpp::function_symbol& function_symbol_PBInit()
{
  static atermpp::function_symbol function_symbol_PBInit = atermpp::function_symbol("PBInit", 1);
  return function_symbol_PBInit;
}

inline
bool gsIsPBInit(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_PBInit();
}

// ProcEqn
inline
const atermpp::function_symbol& function_symbol_ProcEqn()
{
  static atermpp::function_symbol function_symbol_ProcEqn = atermpp::function_symbol("ProcEqn", 3);
  return function_symbol_ProcEqn;
}

inline
bool gsIsProcEqn(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_ProcEqn();
}

// ProcEqnSpec
inline
const atermpp::function_symbol& function_symbol_ProcEqnSpec()
{
  static atermpp::function_symbol function_symbol_ProcEqnSpec = atermpp::function_symbol("ProcEqnSpec", 1);
  return function_symbol_ProcEqnSpec;
}

inline
bool gsIsProcEqnSpec(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_ProcEqnSpec();
}

// ProcSpec
inline
const atermpp::function_symbol& function_symbol_ProcSpec()
{
  static atermpp::function_symbol function_symbol_ProcSpec = atermpp::function_symbol("ProcSpec", 5);
  return function_symbol_ProcSpec;
}

inline
bool gsIsProcSpec(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_ProcSpec();
}

// ProcVarId
inline
const atermpp::function_symbol& function_symbol_ProcVarId()
{
  static atermpp::function_symbol function_symbol_ProcVarId = atermpp::function_symbol("ProcVarId", 3);
  return function_symbol_ProcVarId;
}

inline
bool gsIsProcVarId(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_ProcVarId();
}

// Process
inline
const atermpp::function_symbol& function_symbol_Process()
{
  static atermpp::function_symbol function_symbol_Process = atermpp::function_symbol("Process", 2);
  return function_symbol_Process;
}

inline
bool gsIsProcess(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_Process();
}

// ProcessAssignment
inline
const atermpp::function_symbol& function_symbol_ProcessAssignment()
{
  static atermpp::function_symbol function_symbol_ProcessAssignment = atermpp::function_symbol("ProcessAssignment", 2);
  return function_symbol_ProcessAssignment;
}

inline
bool gsIsProcessAssignment(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_ProcessAssignment();
}

// ProcessInit
inline
const atermpp::function_symbol& function_symbol_ProcessInit()
{
  static atermpp::function_symbol function_symbol_ProcessInit = atermpp::function_symbol("ProcessInit", 1);
  return function_symbol_ProcessInit;
}

inline
bool gsIsProcessInit(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_ProcessInit();
}

// PropVarDecl
inline
const atermpp::function_symbol& function_symbol_PropVarDecl()
{
  static atermpp::function_symbol function_symbol_PropVarDecl = atermpp::function_symbol("PropVarDecl", 2);
  return function_symbol_PropVarDecl;
}

inline
bool gsIsPropVarDecl(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_PropVarDecl();
}

// PropVarInst
inline
const atermpp::function_symbol& function_symbol_PropVarInst()
{
  static atermpp::function_symbol function_symbol_PropVarInst = atermpp::function_symbol("PropVarInst", 3);
  return function_symbol_PropVarInst;
}

inline
bool gsIsPropVarInst(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_PropVarInst();
}

// RegAlt
inline
const atermpp::function_symbol& function_symbol_RegAlt()
{
  static atermpp::function_symbol function_symbol_RegAlt = atermpp::function_symbol("RegAlt", 2);
  return function_symbol_RegAlt;
}

inline
bool gsIsRegAlt(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_RegAlt();
}

// RegNil
inline
const atermpp::function_symbol& function_symbol_RegNil()
{
  static atermpp::function_symbol function_symbol_RegNil = atermpp::function_symbol("RegNil", 0);
  return function_symbol_RegNil;
}

inline
bool gsIsRegNil(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_RegNil();
}

// RegSeq
inline
const atermpp::function_symbol& function_symbol_RegSeq()
{
  static atermpp::function_symbol function_symbol_RegSeq = atermpp::function_symbol("RegSeq", 2);
  return function_symbol_RegSeq;
}

inline
bool gsIsRegSeq(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_RegSeq();
}

// RegTrans
inline
const atermpp::function_symbol& function_symbol_RegTrans()
{
  static atermpp::function_symbol function_symbol_RegTrans = atermpp::function_symbol("RegTrans", 1);
  return function_symbol_RegTrans;
}

inline
bool gsIsRegTrans(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_RegTrans();
}

// RegTransOrNil
inline
const atermpp::function_symbol& function_symbol_RegTransOrNil()
{
  static atermpp::function_symbol function_symbol_RegTransOrNil = atermpp::function_symbol("RegTransOrNil", 1);
  return function_symbol_RegTransOrNil;
}

inline
bool gsIsRegTransOrNil(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_RegTransOrNil();
}

// Rename
inline
const atermpp::function_symbol& function_symbol_Rename()
{
  static atermpp::function_symbol function_symbol_Rename = atermpp::function_symbol("Rename", 2);
  return function_symbol_Rename;
}

inline
bool gsIsRename(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_Rename();
}

// RenameExpr
inline
const atermpp::function_symbol& function_symbol_RenameExpr()
{
  static atermpp::function_symbol function_symbol_RenameExpr = atermpp::function_symbol("RenameExpr", 2);
  return function_symbol_RenameExpr;
}

inline
bool gsIsRenameExpr(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_RenameExpr();
}

// Seq
inline
const atermpp::function_symbol& function_symbol_Seq()
{
  static atermpp::function_symbol function_symbol_Seq = atermpp::function_symbol("Seq", 2);
  return function_symbol_Seq;
}

inline
bool gsIsSeq(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_Seq();
}

// SetComp
inline
const atermpp::function_symbol& function_symbol_SetComp()
{
  static atermpp::function_symbol function_symbol_SetComp = atermpp::function_symbol("SetComp", 0);
  return function_symbol_SetComp;
}

inline
bool gsIsSetComp(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_SetComp();
}

// SortArrow
inline
const atermpp::function_symbol& function_symbol_SortArrow()
{
  static atermpp::function_symbol function_symbol_SortArrow = atermpp::function_symbol("SortArrow", 2);
  return function_symbol_SortArrow;
}

inline
bool gsIsSortArrow(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_SortArrow();
}

// SortBag
inline
const atermpp::function_symbol& function_symbol_SortBag()
{
  static atermpp::function_symbol function_symbol_SortBag = atermpp::function_symbol("SortBag", 0);
  return function_symbol_SortBag;
}

inline
bool gsIsSortBag(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_SortBag();
}

// SortCons
inline
const atermpp::function_symbol& function_symbol_SortCons()
{
  static atermpp::function_symbol function_symbol_SortCons = atermpp::function_symbol("SortCons", 2);
  return function_symbol_SortCons;
}

inline
bool gsIsSortCons(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_SortCons();
}

// SortFBag
inline
const atermpp::function_symbol& function_symbol_SortFBag()
{
  static atermpp::function_symbol function_symbol_SortFBag = atermpp::function_symbol("SortFBag", 0);
  return function_symbol_SortFBag;
}

inline
bool gsIsSortFBag(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_SortFBag();
}

// SortFSet
inline
const atermpp::function_symbol& function_symbol_SortFSet()
{
  static atermpp::function_symbol function_symbol_SortFSet = atermpp::function_symbol("SortFSet", 0);
  return function_symbol_SortFSet;
}

inline
bool gsIsSortFSet(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_SortFSet();
}

// SortId
inline
const atermpp::function_symbol& function_symbol_SortId()
{
  static atermpp::function_symbol function_symbol_SortId = atermpp::function_symbol("SortId", 1);
  return function_symbol_SortId;
}

inline
bool gsIsSortId(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_SortId();
}

// SortList
inline
const atermpp::function_symbol& function_symbol_SortList()
{
  static atermpp::function_symbol function_symbol_SortList = atermpp::function_symbol("SortList", 0);
  return function_symbol_SortList;
}

inline
bool gsIsSortList(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_SortList();
}

// SortRef
inline
const atermpp::function_symbol& function_symbol_SortRef()
{
  static atermpp::function_symbol function_symbol_SortRef = atermpp::function_symbol("SortRef", 2);
  return function_symbol_SortRef;
}

inline
bool gsIsSortRef(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_SortRef();
}

// SortSet
inline
const atermpp::function_symbol& function_symbol_SortSet()
{
  static atermpp::function_symbol function_symbol_SortSet = atermpp::function_symbol("SortSet", 0);
  return function_symbol_SortSet;
}

inline
bool gsIsSortSet(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_SortSet();
}

// SortSpec
inline
const atermpp::function_symbol& function_symbol_SortSpec()
{
  static atermpp::function_symbol function_symbol_SortSpec = atermpp::function_symbol("SortSpec", 1);
  return function_symbol_SortSpec;
}

inline
bool gsIsSortSpec(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_SortSpec();
}

// SortStruct
inline
const atermpp::function_symbol& function_symbol_SortStruct()
{
  static atermpp::function_symbol function_symbol_SortStruct = atermpp::function_symbol("SortStruct", 1);
  return function_symbol_SortStruct;
}

inline
bool gsIsSortStruct(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_SortStruct();
}

// StateAnd
inline
const atermpp::function_symbol& function_symbol_StateAnd()
{
  static atermpp::function_symbol function_symbol_StateAnd = atermpp::function_symbol("StateAnd", 2);
  return function_symbol_StateAnd;
}

inline
bool gsIsStateAnd(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_StateAnd();
}

// StateDelay
inline
const atermpp::function_symbol& function_symbol_StateDelay()
{
  static atermpp::function_symbol function_symbol_StateDelay = atermpp::function_symbol("StateDelay", 0);
  return function_symbol_StateDelay;
}

inline
bool gsIsStateDelay(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_StateDelay();
}

// StateDelayTimed
inline
const atermpp::function_symbol& function_symbol_StateDelayTimed()
{
  static atermpp::function_symbol function_symbol_StateDelayTimed = atermpp::function_symbol("StateDelayTimed", 1);
  return function_symbol_StateDelayTimed;
}

inline
bool gsIsStateDelayTimed(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_StateDelayTimed();
}

// StateExists
inline
const atermpp::function_symbol& function_symbol_StateExists()
{
  static atermpp::function_symbol function_symbol_StateExists = atermpp::function_symbol("StateExists", 2);
  return function_symbol_StateExists;
}

inline
bool gsIsStateExists(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_StateExists();
}

// StateFalse
inline
const atermpp::function_symbol& function_symbol_StateFalse()
{
  static atermpp::function_symbol function_symbol_StateFalse = atermpp::function_symbol("StateFalse", 0);
  return function_symbol_StateFalse;
}

inline
bool gsIsStateFalse(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_StateFalse();
}

// StateForall
inline
const atermpp::function_symbol& function_symbol_StateForall()
{
  static atermpp::function_symbol function_symbol_StateForall = atermpp::function_symbol("StateForall", 2);
  return function_symbol_StateForall;
}

inline
bool gsIsStateForall(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_StateForall();
}

// StateImp
inline
const atermpp::function_symbol& function_symbol_StateImp()
{
  static atermpp::function_symbol function_symbol_StateImp = atermpp::function_symbol("StateImp", 2);
  return function_symbol_StateImp;
}

inline
bool gsIsStateImp(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_StateImp();
}

// StateMay
inline
const atermpp::function_symbol& function_symbol_StateMay()
{
  static atermpp::function_symbol function_symbol_StateMay = atermpp::function_symbol("StateMay", 2);
  return function_symbol_StateMay;
}

inline
bool gsIsStateMay(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_StateMay();
}

// StateMu
inline
const atermpp::function_symbol& function_symbol_StateMu()
{
  static atermpp::function_symbol function_symbol_StateMu = atermpp::function_symbol("StateMu", 3);
  return function_symbol_StateMu;
}

inline
bool gsIsStateMu(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_StateMu();
}

// StateMust
inline
const atermpp::function_symbol& function_symbol_StateMust()
{
  static atermpp::function_symbol function_symbol_StateMust = atermpp::function_symbol("StateMust", 2);
  return function_symbol_StateMust;
}

inline
bool gsIsStateMust(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_StateMust();
}

// StateNot
inline
const atermpp::function_symbol& function_symbol_StateNot()
{
  static atermpp::function_symbol function_symbol_StateNot = atermpp::function_symbol("StateNot", 1);
  return function_symbol_StateNot;
}

inline
bool gsIsStateNot(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_StateNot();
}

// StateNu
inline
const atermpp::function_symbol& function_symbol_StateNu()
{
  static atermpp::function_symbol function_symbol_StateNu = atermpp::function_symbol("StateNu", 3);
  return function_symbol_StateNu;
}

inline
bool gsIsStateNu(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_StateNu();
}

// StateOr
inline
const atermpp::function_symbol& function_symbol_StateOr()
{
  static atermpp::function_symbol function_symbol_StateOr = atermpp::function_symbol("StateOr", 2);
  return function_symbol_StateOr;
}

inline
bool gsIsStateOr(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_StateOr();
}

// StateTrue
inline
const atermpp::function_symbol& function_symbol_StateTrue()
{
  static atermpp::function_symbol function_symbol_StateTrue = atermpp::function_symbol("StateTrue", 0);
  return function_symbol_StateTrue;
}

inline
bool gsIsStateTrue(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_StateTrue();
}

// StateVar
inline
const atermpp::function_symbol& function_symbol_StateVar()
{
  static atermpp::function_symbol function_symbol_StateVar = atermpp::function_symbol("StateVar", 2);
  return function_symbol_StateVar;
}

inline
bool gsIsStateVar(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_StateVar();
}

// StateYaled
inline
const atermpp::function_symbol& function_symbol_StateYaled()
{
  static atermpp::function_symbol function_symbol_StateYaled = atermpp::function_symbol("StateYaled", 0);
  return function_symbol_StateYaled;
}

inline
bool gsIsStateYaled(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_StateYaled();
}

// StateYaledTimed
inline
const atermpp::function_symbol& function_symbol_StateYaledTimed()
{
  static atermpp::function_symbol function_symbol_StateYaledTimed = atermpp::function_symbol("StateYaledTimed", 1);
  return function_symbol_StateYaledTimed;
}

inline
bool gsIsStateYaledTimed(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_StateYaledTimed();
}

// StructCons
inline
const atermpp::function_symbol& function_symbol_StructCons()
{
  static atermpp::function_symbol function_symbol_StructCons = atermpp::function_symbol("StructCons", 3);
  return function_symbol_StructCons;
}

inline
bool gsIsStructCons(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_StructCons();
}

// StructProj
inline
const atermpp::function_symbol& function_symbol_StructProj()
{
  static atermpp::function_symbol function_symbol_StructProj = atermpp::function_symbol("StructProj", 2);
  return function_symbol_StructProj;
}

inline
bool gsIsStructProj(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_StructProj();
}

// Sum
inline
const atermpp::function_symbol& function_symbol_Sum()
{
  static atermpp::function_symbol function_symbol_Sum = atermpp::function_symbol("Sum", 2);
  return function_symbol_Sum;
}

inline
bool gsIsSum(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_Sum();
}

// Sync
inline
const atermpp::function_symbol& function_symbol_Sync()
{
  static atermpp::function_symbol function_symbol_Sync = atermpp::function_symbol("Sync", 2);
  return function_symbol_Sync;
}

inline
bool gsIsSync(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_Sync();
}

// Tau
inline
const atermpp::function_symbol& function_symbol_Tau()
{
  static atermpp::function_symbol function_symbol_Tau = atermpp::function_symbol("Tau", 0);
  return function_symbol_Tau;
}

inline
bool gsIsTau(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_Tau();
}

// UntypedActMultAct
inline
const atermpp::function_symbol& function_symbol_UntypedActMultAct()
{
  static atermpp::function_symbol function_symbol_UntypedActMultAct = atermpp::function_symbol("UntypedActMultAct", 1);
  return function_symbol_UntypedActMultAct;
}

inline
bool gsIsUntypedActMultAct(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_UntypedActMultAct();
}

// UntypedAction
inline
const atermpp::function_symbol& function_symbol_UntypedAction()
{
  static atermpp::function_symbol function_symbol_UntypedAction = atermpp::function_symbol("UntypedAction", 2);
  return function_symbol_UntypedAction;
}

inline
bool gsIsUntypedAction(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_UntypedAction();
}

// UntypedIdentifier
inline
const atermpp::function_symbol& function_symbol_UntypedIdentifier()
{
  static atermpp::function_symbol function_symbol_UntypedIdentifier = atermpp::function_symbol("UntypedIdentifier", 1);
  return function_symbol_UntypedIdentifier;
}

inline
bool gsIsUntypedIdentifier(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_UntypedIdentifier();
}

// UntypedIdentifierAssignment
inline
const atermpp::function_symbol& function_symbol_UntypedIdentifierAssignment()
{
  static atermpp::function_symbol function_symbol_UntypedIdentifierAssignment = atermpp::function_symbol("UntypedIdentifierAssignment", 2);
  return function_symbol_UntypedIdentifierAssignment;
}

inline
bool gsIsUntypedIdentifierAssignment(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_UntypedIdentifierAssignment();
}

// UntypedMultAct
inline
const atermpp::function_symbol& function_symbol_UntypedMultAct()
{
  static atermpp::function_symbol function_symbol_UntypedMultAct = atermpp::function_symbol("UntypedMultAct", 1);
  return function_symbol_UntypedMultAct;
}

inline
bool gsIsUntypedMultAct(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_UntypedMultAct();
}

// UntypedParamId
inline
const atermpp::function_symbol& function_symbol_UntypedParamId()
{
  static atermpp::function_symbol function_symbol_UntypedParamId = atermpp::function_symbol("UntypedParamId", 2);
  return function_symbol_UntypedParamId;
}

inline
bool gsIsUntypedParamId(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_UntypedParamId();
}

// UntypedProcessAssignment
inline
const atermpp::function_symbol& function_symbol_UntypedProcessAssignment()
{
  static atermpp::function_symbol function_symbol_UntypedProcessAssignment = atermpp::function_symbol("UntypedProcessAssignment", 2);
  return function_symbol_UntypedProcessAssignment;
}

inline
bool gsIsUntypedProcessAssignment(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_UntypedProcessAssignment();
}

// UntypedSetBagComp
inline
const atermpp::function_symbol& function_symbol_UntypedSetBagComp()
{
  static atermpp::function_symbol function_symbol_UntypedSetBagComp = atermpp::function_symbol("UntypedSetBagComp", 0);
  return function_symbol_UntypedSetBagComp;
}

inline
bool gsIsUntypedSetBagComp(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_UntypedSetBagComp();
}

// UntypedSortUnknown
inline
const atermpp::function_symbol& function_symbol_UntypedSortUnknown()
{
  static atermpp::function_symbol function_symbol_UntypedSortUnknown = atermpp::function_symbol("UntypedSortUnknown", 0);
  return function_symbol_UntypedSortUnknown;
}

inline
bool gsIsUntypedSortUnknown(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_UntypedSortUnknown();
}

// UntypedSortsPossible
inline
const atermpp::function_symbol& function_symbol_UntypedSortsPossible()
{
  static atermpp::function_symbol function_symbol_UntypedSortsPossible = atermpp::function_symbol("UntypedSortsPossible", 1);
  return function_symbol_UntypedSortsPossible;
}

inline
bool gsIsUntypedSortsPossible(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_UntypedSortsPossible();
}

// Whr
inline
const atermpp::function_symbol& function_symbol_Whr()
{
  static atermpp::function_symbol function_symbol_Whr = atermpp::function_symbol("Whr", 2);
  return function_symbol_Whr;
}

inline
bool gsIsWhr(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_Whr();
}

inline
aterm_appl gsMakeActAnd(const aterm& ActFrm_0, const aterm& ActFrm_1)
{
  return aterm_appl(function_symbol_ActAnd(), ActFrm_0, ActFrm_1);
}

inline
aterm_appl gsMakeActAt(const aterm& ActFrm_0, const aterm& DataExpr_1)
{
  return aterm_appl(function_symbol_ActAt(), ActFrm_0, DataExpr_1);
}

inline
aterm_appl gsMakeActExists(const aterm_list& DataVarId_0, const aterm& ActFrm_1)
{
  return aterm_appl(function_symbol_ActExists(), DataVarId_0, ActFrm_1);
}

inline
aterm_appl gsMakeActFalse()
{
  return aterm_appl(function_symbol_ActFalse());
}

inline
aterm_appl gsMakeActForall(const aterm_list& DataVarId_0, const aterm& ActFrm_1)
{
  return aterm_appl(function_symbol_ActForall(), DataVarId_0, ActFrm_1);
}

inline
aterm_appl gsMakeActId(const aterm& String_0, const aterm_list& SortExpr_1)
{
  return aterm_appl(function_symbol_ActId(), String_0, SortExpr_1);
}

inline
aterm_appl gsMakeActImp(const aterm& ActFrm_0, const aterm& ActFrm_1)
{
  return aterm_appl(function_symbol_ActImp(), ActFrm_0, ActFrm_1);
}

inline
aterm_appl gsMakeActMultAct(const aterm_list& Action_0)
{
  return aterm_appl(function_symbol_ActMultAct(), Action_0);
}

inline
aterm_appl gsMakeActNot(const aterm& ActFrm_0)
{
  return aterm_appl(function_symbol_ActNot(), ActFrm_0);
}

inline
aterm_appl gsMakeActOr(const aterm& ActFrm_0, const aterm& ActFrm_1)
{
  return aterm_appl(function_symbol_ActOr(), ActFrm_0, ActFrm_1);
}

inline
aterm_appl gsMakeActSpec(const aterm_list& ActId_0)
{
  return aterm_appl(function_symbol_ActSpec(), ActId_0);
}

inline
aterm_appl gsMakeActTrue()
{
  return aterm_appl(function_symbol_ActTrue());
}

inline
aterm_appl gsMakeAction(const aterm& ActId_0, const aterm_list& DataExpr_1)
{
  return aterm_appl(function_symbol_Action(), ActId_0, DataExpr_1);
}

inline
aterm_appl gsMakeActionRenameRule(const aterm_list& DataVarId_0, const aterm& DataExpr_1, const aterm& ParamIdOrAction_2, const aterm& ActionRenameRuleRHS_3)
{
  return aterm_appl(function_symbol_ActionRenameRule(), DataVarId_0, DataExpr_1, ParamIdOrAction_2, ActionRenameRuleRHS_3);
}

inline
aterm_appl gsMakeActionRenameRules(const aterm_list& ActionRenameRule_0)
{
  return aterm_appl(function_symbol_ActionRenameRules(), ActionRenameRule_0);
}

inline
aterm_appl gsMakeActionRenameSpec(const aterm& DataSpec_0, const aterm& ActSpec_1, const aterm& ActionRenameRules_2)
{
  return aterm_appl(function_symbol_ActionRenameSpec(), DataSpec_0, ActSpec_1, ActionRenameRules_2);
}

inline
aterm_appl gsMakeAllow(const aterm_list& MultActName_0, const aterm& ProcExpr_1)
{
  return aterm_appl(function_symbol_Allow(), MultActName_0, ProcExpr_1);
}

inline
aterm_appl gsMakeAtTime(const aterm& ProcExpr_0, const aterm& DataExpr_1)
{
  return aterm_appl(function_symbol_AtTime(), ProcExpr_0, DataExpr_1);
}

inline
aterm_appl gsMakeBES(const aterm_list& BooleanEquation_0, const aterm& BooleanExpression_1)
{
  return aterm_appl(function_symbol_BES(), BooleanEquation_0, BooleanExpression_1);
}

inline
aterm_appl gsMakeBInit(const aterm& ProcExpr_0, const aterm& ProcExpr_1)
{
  return aterm_appl(function_symbol_BInit(), ProcExpr_0, ProcExpr_1);
}

inline
aterm_appl gsMakeBagComp()
{
  return aterm_appl(function_symbol_BagComp());
}

inline
aterm_appl gsMakeBinder(const aterm& BindingOperator_0, const aterm_list& DataVarId_1, const aterm& DataExpr_2)
{
  return aterm_appl(function_symbol_Binder(), BindingOperator_0, DataVarId_1, DataExpr_2);
}

inline
aterm_appl gsMakeBlock(const aterm_list& String_0, const aterm& ProcExpr_1)
{
  return aterm_appl(function_symbol_Block(), String_0, ProcExpr_1);
}

inline
aterm_appl gsMakeBooleanAnd(const aterm& BooleanExpression_0, const aterm& BooleanExpression_1)
{
  return aterm_appl(function_symbol_BooleanAnd(), BooleanExpression_0, BooleanExpression_1);
}

inline
aterm_appl gsMakeBooleanEquation(const aterm& FixPoint_0, const aterm& BooleanVariable_1, const aterm& BooleanExpression_2)
{
  return aterm_appl(function_symbol_BooleanEquation(), FixPoint_0, BooleanVariable_1, BooleanExpression_2);
}

inline
aterm_appl gsMakeBooleanFalse()
{
  return aterm_appl(function_symbol_BooleanFalse());
}

inline
aterm_appl gsMakeBooleanImp(const aterm& BooleanExpression_0, const aterm& BooleanExpression_1)
{
  return aterm_appl(function_symbol_BooleanImp(), BooleanExpression_0, BooleanExpression_1);
}

inline
aterm_appl gsMakeBooleanNot(const aterm& BooleanExpression_0)
{
  return aterm_appl(function_symbol_BooleanNot(), BooleanExpression_0);
}

inline
aterm_appl gsMakeBooleanOr(const aterm& BooleanExpression_0, const aterm& BooleanExpression_1)
{
  return aterm_appl(function_symbol_BooleanOr(), BooleanExpression_0, BooleanExpression_1);
}

inline
aterm_appl gsMakeBooleanTrue()
{
  return aterm_appl(function_symbol_BooleanTrue());
}

inline
aterm_appl gsMakeBooleanVariable(const aterm& String_0, const aterm& Number_1)
{
  return aterm_appl(function_symbol_BooleanVariable(), String_0, Number_1);
}

inline
aterm_appl gsMakeChoice(const aterm& ProcExpr_0, const aterm& ProcExpr_1)
{
  return aterm_appl(function_symbol_Choice(), ProcExpr_0, ProcExpr_1);
}

inline
aterm_appl gsMakeComm(const aterm_list& CommExpr_0, const aterm& ProcExpr_1)
{
  return aterm_appl(function_symbol_Comm(), CommExpr_0, ProcExpr_1);
}

inline
aterm_appl gsMakeCommExpr(const aterm& MultActName_0, const aterm& String_1)
{
  return aterm_appl(function_symbol_CommExpr(), MultActName_0, String_1);
}

inline
aterm_appl gsMakeConsSpec(const aterm_list& OpId_0)
{
  return aterm_appl(function_symbol_ConsSpec(), OpId_0);
}

inline
aterm_appl gsMakeDataEqn(const aterm_list& DataVarId_0, const aterm& DataExpr_1, const aterm& DataExpr_2, const aterm& DataExpr_3)
{
  return aterm_appl(function_symbol_DataEqn(), DataVarId_0, DataExpr_1, DataExpr_2, DataExpr_3);
}

inline
aterm_appl gsMakeDataEqnSpec(const aterm_list& DataEqn_0)
{
  return aterm_appl(function_symbol_DataEqnSpec(), DataEqn_0);
}

inline
aterm_appl gsMakeDataSpec(const aterm& SortSpec_0, const aterm& ConsSpec_1, const aterm& MapSpec_2, const aterm& DataEqnSpec_3)
{
  return aterm_appl(function_symbol_DataSpec(), SortSpec_0, ConsSpec_1, MapSpec_2, DataEqnSpec_3);
}

inline
aterm_appl gsMakeDataVarId(const aterm& String_0, const aterm& SortExpr_1, const aterm& Number_2)
{
  return aterm_appl(function_symbol_DataVarId(), String_0, SortExpr_1, Number_2);
}

inline
aterm_appl gsMakeDataVarIdInit(const aterm& DataVarId_0, const aterm& DataExpr_1)
{
  return aterm_appl(function_symbol_DataVarIdInit(), DataVarId_0, DataExpr_1);
}

inline
aterm_appl gsMakeDelta()
{
  return aterm_appl(function_symbol_Delta());
}

inline
aterm_appl gsMakeExists()
{
  return aterm_appl(function_symbol_Exists());
}

inline
aterm_appl gsMakeForall()
{
  return aterm_appl(function_symbol_Forall());
}

inline
aterm_appl gsMakeGlobVarSpec(const aterm_list& DataVarId_0)
{
  return aterm_appl(function_symbol_GlobVarSpec(), DataVarId_0);
}

inline
aterm_appl gsMakeHide(const aterm_list& String_0, const aterm& ProcExpr_1)
{
  return aterm_appl(function_symbol_Hide(), String_0, ProcExpr_1);
}

inline
aterm_appl gsMakeIfThen(const aterm& DataExpr_0, const aterm& ProcExpr_1)
{
  return aterm_appl(function_symbol_IfThen(), DataExpr_0, ProcExpr_1);
}

inline
aterm_appl gsMakeIfThenElse(const aterm& DataExpr_0, const aterm& ProcExpr_1, const aterm& ProcExpr_2)
{
  return aterm_appl(function_symbol_IfThenElse(), DataExpr_0, ProcExpr_1, ProcExpr_2);
}

inline
aterm_appl gsMakeLMerge(const aterm& ProcExpr_0, const aterm& ProcExpr_1)
{
  return aterm_appl(function_symbol_LMerge(), ProcExpr_0, ProcExpr_1);
}

inline
aterm_appl gsMakeLambda()
{
  return aterm_appl(function_symbol_Lambda());
}

inline
aterm_appl gsMakeLinProcSpec(const aterm& DataSpec_0, const aterm& ActSpec_1, const aterm& GlobVarSpec_2, const aterm& LinearProcess_3, const aterm& LinearProcessInit_4)
{
  return aterm_appl(function_symbol_LinProcSpec(), DataSpec_0, ActSpec_1, GlobVarSpec_2, LinearProcess_3, LinearProcessInit_4);
}

inline
aterm_appl gsMakeLinearProcess(const aterm_list& DataVarId_0, const aterm_list& LinearProcessSummand_1)
{
  return aterm_appl(function_symbol_LinearProcess(), DataVarId_0, LinearProcessSummand_1);
}

inline
aterm_appl gsMakeLinearProcessInit(const aterm_list& DataVarIdInit_0)
{
  return aterm_appl(function_symbol_LinearProcessInit(), DataVarIdInit_0);
}

inline
aterm_appl gsMakeLinearProcessSummand(const aterm_list& DataVarId_0, const aterm& DataExpr_1, const aterm& MultActOrDelta_2, const aterm& DataExprOrNil_3, const aterm_list& DataVarIdInit_4)
{
  return aterm_appl(function_symbol_LinearProcessSummand(), DataVarId_0, DataExpr_1, MultActOrDelta_2, DataExprOrNil_3, DataVarIdInit_4);
}

inline
aterm_appl gsMakeMapSpec(const aterm_list& OpId_0)
{
  return aterm_appl(function_symbol_MapSpec(), OpId_0);
}

inline
aterm_appl gsMakeMerge(const aterm& ProcExpr_0, const aterm& ProcExpr_1)
{
  return aterm_appl(function_symbol_Merge(), ProcExpr_0, ProcExpr_1);
}

inline
aterm_appl gsMakeMu()
{
  return aterm_appl(function_symbol_Mu());
}

inline
aterm_appl gsMakeMultAct(const aterm_list& Action_0)
{
  return aterm_appl(function_symbol_MultAct(), Action_0);
}

inline
aterm_appl gsMakeMultActName(const aterm_list& String_0)
{
  return aterm_appl(function_symbol_MultActName(), String_0);
}

inline
aterm_appl gsMakeNil()
{
  return aterm_appl(function_symbol_Nil());
}

inline
aterm_appl gsMakeNu()
{
  return aterm_appl(function_symbol_Nu());
}

/* inline
aterm_appl gsMakeOpId(const aterm& String_0, const aterm& SortExpr_1, const aterm& Number_2)
{
  return aterm_appl(function_symbol_OpId(), String_0, SortExpr_1, Number_2);
} */

inline
aterm_appl gsMakePBES(const aterm& DataSpec_0, const aterm& GlobVarSpec_1, const aterm& PBEqnSpec_2, const aterm& PBInit_3)
{
  return aterm_appl(function_symbol_PBES(), DataSpec_0, GlobVarSpec_1, PBEqnSpec_2, PBInit_3);
}

inline
aterm_appl gsMakePBESAnd(const aterm& PBExpr_0, const aterm& PBExpr_1)
{
  return aterm_appl(function_symbol_PBESAnd(), PBExpr_0, PBExpr_1);
}

inline
aterm_appl gsMakePBESExists(const aterm_list& DataVarId_0, const aterm& PBExpr_1)
{
  return aterm_appl(function_symbol_PBESExists(), DataVarId_0, PBExpr_1);
}

inline
aterm_appl gsMakePBESFalse()
{
  return aterm_appl(function_symbol_PBESFalse());
}

inline
aterm_appl gsMakePBESForall(const aterm_list& DataVarId_0, const aterm& PBExpr_1)
{
  return aterm_appl(function_symbol_PBESForall(), DataVarId_0, PBExpr_1);
}

inline
aterm_appl gsMakePBESImp(const aterm& PBExpr_0, const aterm& PBExpr_1)
{
  return aterm_appl(function_symbol_PBESImp(), PBExpr_0, PBExpr_1);
}

inline
aterm_appl gsMakePBESNot(const aterm& PBExpr_0)
{
  return aterm_appl(function_symbol_PBESNot(), PBExpr_0);
}

inline
aterm_appl gsMakePBESOr(const aterm& PBExpr_0, const aterm& PBExpr_1)
{
  return aterm_appl(function_symbol_PBESOr(), PBExpr_0, PBExpr_1);
}

inline
aterm_appl gsMakePBESTrue()
{
  return aterm_appl(function_symbol_PBESTrue());
}

inline
aterm_appl gsMakePBEqn(const aterm& FixPoint_0, const aterm& PropVarDecl_1, const aterm& PBExpr_2)
{
  return aterm_appl(function_symbol_PBEqn(), FixPoint_0, PropVarDecl_1, PBExpr_2);
}

inline
aterm_appl gsMakePBEqnSpec(const aterm_list& PBEqn_0)
{
  return aterm_appl(function_symbol_PBEqnSpec(), PBEqn_0);
}

inline
aterm_appl gsMakePBInit(const aterm& PropVarInst_0)
{
  return aterm_appl(function_symbol_PBInit(), PropVarInst_0);
}

inline
aterm_appl gsMakeProcEqn(const aterm& ProcVarId_0, const aterm_list& DataVarId_1, const aterm& ProcExpr_2)
{
  return aterm_appl(function_symbol_ProcEqn(), ProcVarId_0, DataVarId_1, ProcExpr_2);
}

inline
aterm_appl gsMakeProcEqnSpec(const aterm_list& ProcEqn_0)
{
  return aterm_appl(function_symbol_ProcEqnSpec(), ProcEqn_0);
}

inline
aterm_appl gsMakeProcSpec(const aterm& DataSpec_0, const aterm& ActSpec_1, const aterm& GlobVarSpec_2, const aterm& ProcEqnSpec_3, const aterm& ProcInit_4)
{
  return aterm_appl(function_symbol_ProcSpec(), DataSpec_0, ActSpec_1, GlobVarSpec_2, ProcEqnSpec_3, ProcInit_4);
}

inline
aterm_appl gsMakeProcVarId(const aterm& String_0, const aterm_list& DataVarId_1, const aterm& Number_2)
{
  return aterm_appl(function_symbol_ProcVarId(), String_0, DataVarId_1, Number_2);
}

inline
aterm_appl gsMakeProcess(const aterm& ProcVarId_0, const aterm_list& DataExpr_1)
{
  return aterm_appl(function_symbol_Process(), ProcVarId_0, DataExpr_1);
}

inline
aterm_appl gsMakeProcessAssignment(const aterm& ProcVarId_0, const aterm_list& DataVarIdInit_1)
{
  return aterm_appl(function_symbol_ProcessAssignment(), ProcVarId_0, DataVarIdInit_1);
}

inline
aterm_appl gsMakeProcessInit(const aterm& ProcExpr_0)
{
  return aterm_appl(function_symbol_ProcessInit(), ProcExpr_0);
}

inline
aterm_appl gsMakePropVarDecl(const aterm& String_0, const aterm_list& DataVarId_1)
{
  return aterm_appl(function_symbol_PropVarDecl(), String_0, DataVarId_1);
}

inline
aterm_appl gsMakePropVarInst(const aterm& String_0, const aterm_list& DataExpr_1, const aterm& Number_2)
{
  return aterm_appl(function_symbol_PropVarInst(), String_0, DataExpr_1, Number_2);
}

inline
aterm_appl gsMakeRegAlt(const aterm& RegFrm_0, const aterm& RegFrm_1)
{
  return aterm_appl(function_symbol_RegAlt(), RegFrm_0, RegFrm_1);
}

inline
aterm_appl gsMakeRegNil()
{
  return aterm_appl(function_symbol_RegNil());
}

inline
aterm_appl gsMakeRegSeq(const aterm& RegFrm_0, const aterm& RegFrm_1)
{
  return aterm_appl(function_symbol_RegSeq(), RegFrm_0, RegFrm_1);
}

inline
aterm_appl gsMakeRegTrans(const aterm& RegFrm_0)
{
  return aterm_appl(function_symbol_RegTrans(), RegFrm_0);
}

inline
aterm_appl gsMakeRegTransOrNil(const aterm& RegFrm_0)
{
  return aterm_appl(function_symbol_RegTransOrNil(), RegFrm_0);
}

inline
aterm_appl gsMakeRename(const aterm_list& RenameExpr_0, const aterm& ProcExpr_1)
{
  return aterm_appl(function_symbol_Rename(), RenameExpr_0, ProcExpr_1);
}

inline
aterm_appl gsMakeRenameExpr(const aterm& String_0, const aterm& String_1)
{
  return aterm_appl(function_symbol_RenameExpr(), String_0, String_1);
}

inline
aterm_appl gsMakeSeq(const aterm& ProcExpr_0, const aterm& ProcExpr_1)
{
  return aterm_appl(function_symbol_Seq(), ProcExpr_0, ProcExpr_1);
}

inline
aterm_appl gsMakeSetComp()
{
  return aterm_appl(function_symbol_SetComp());
}

inline
aterm_appl gsMakeSortArrow(const aterm_list& SortExpr_0, const aterm& SortExpr_1)
{
  return aterm_appl(function_symbol_SortArrow(), SortExpr_0, SortExpr_1);
}

inline
aterm_appl gsMakeSortBag()
{
  return aterm_appl(function_symbol_SortBag());
}

inline
aterm_appl gsMakeSortCons(const aterm& SortConsType_0, const aterm& SortExpr_1)
{
  return aterm_appl(function_symbol_SortCons(), SortConsType_0, SortExpr_1);
}

inline
aterm_appl gsMakeSortFBag()
{
  return aterm_appl(function_symbol_SortFBag());
}

inline
aterm_appl gsMakeSortFSet()
{
  return aterm_appl(function_symbol_SortFSet());
}

inline
aterm_appl gsMakeSortId(const aterm& String_0)
{
  return aterm_appl(function_symbol_SortId(), String_0);
}

inline
aterm_appl gsMakeSortList()
{
  return aterm_appl(function_symbol_SortList());
}

inline
aterm_appl gsMakeSortRef(const aterm& SortId_0, const aterm& SortExpr_1)
{
  return aterm_appl(function_symbol_SortRef(), SortId_0, SortExpr_1);
}

inline
aterm_appl gsMakeSortSet()
{
  return aterm_appl(function_symbol_SortSet());
}

inline
aterm_appl gsMakeSortSpec(const aterm_list& SortDecl_0)
{
  return aterm_appl(function_symbol_SortSpec(), SortDecl_0);
}

inline
aterm_appl gsMakeSortStruct(const aterm_list& StructCons_0)
{
  return aterm_appl(function_symbol_SortStruct(), StructCons_0);
}

inline
aterm_appl gsMakeStateAnd(const aterm& StateFrm_0, const aterm& StateFrm_1)
{
  return aterm_appl(function_symbol_StateAnd(), StateFrm_0, StateFrm_1);
}

inline
aterm_appl gsMakeStateDelay()
{
  return aterm_appl(function_symbol_StateDelay());
}

inline
aterm_appl gsMakeStateDelayTimed(const aterm& DataExpr_0)
{
  return aterm_appl(function_symbol_StateDelayTimed(), DataExpr_0);
}

inline
aterm_appl gsMakeStateExists(const aterm_list& DataVarId_0, const aterm& StateFrm_1)
{
  return aterm_appl(function_symbol_StateExists(), DataVarId_0, StateFrm_1);
}

inline
aterm_appl gsMakeStateFalse()
{
  return aterm_appl(function_symbol_StateFalse());
}

inline
aterm_appl gsMakeStateForall(const aterm_list& DataVarId_0, const aterm& StateFrm_1)
{
  return aterm_appl(function_symbol_StateForall(), DataVarId_0, StateFrm_1);
}

inline
aterm_appl gsMakeStateImp(const aterm& StateFrm_0, const aterm& StateFrm_1)
{
  return aterm_appl(function_symbol_StateImp(), StateFrm_0, StateFrm_1);
}

inline
aterm_appl gsMakeStateMay(const aterm& RegFrm_0, const aterm& StateFrm_1)
{
  return aterm_appl(function_symbol_StateMay(), RegFrm_0, StateFrm_1);
}

inline
aterm_appl gsMakeStateMu(const aterm& String_0, const aterm_list& DataVarIdInit_1, const aterm& StateFrm_2)
{
  return aterm_appl(function_symbol_StateMu(), String_0, DataVarIdInit_1, StateFrm_2);
}

inline
aterm_appl gsMakeStateMust(const aterm& RegFrm_0, const aterm& StateFrm_1)
{
  return aterm_appl(function_symbol_StateMust(), RegFrm_0, StateFrm_1);
}

inline
aterm_appl gsMakeStateNot(const aterm& StateFrm_0)
{
  return aterm_appl(function_symbol_StateNot(), StateFrm_0);
}

inline
aterm_appl gsMakeStateNu(const aterm& String_0, const aterm_list& DataVarIdInit_1, const aterm& StateFrm_2)
{
  return aterm_appl(function_symbol_StateNu(), String_0, DataVarIdInit_1, StateFrm_2);
}

inline
aterm_appl gsMakeStateOr(const aterm& StateFrm_0, const aterm& StateFrm_1)
{
  return aterm_appl(function_symbol_StateOr(), StateFrm_0, StateFrm_1);
}

inline
aterm_appl gsMakeStateTrue()
{
  return aterm_appl(function_symbol_StateTrue());
}

inline
aterm_appl gsMakeStateVar(const aterm& String_0, const aterm_list& DataExpr_1)
{
  return aterm_appl(function_symbol_StateVar(), String_0, DataExpr_1);
}

inline
aterm_appl gsMakeStateYaled()
{
  return aterm_appl(function_symbol_StateYaled());
}

inline
aterm_appl gsMakeStateYaledTimed(const aterm& DataExpr_0)
{
  return aterm_appl(function_symbol_StateYaledTimed(), DataExpr_0);
}

inline
aterm_appl gsMakeStructCons(const aterm& String_0, const aterm_list& StructProj_1, const aterm& StringOrEmpty_2)
{
  return aterm_appl(function_symbol_StructCons(), String_0, StructProj_1, StringOrEmpty_2);
}

inline
aterm_appl gsMakeStructProj(const aterm& StringOrEmpty_0, const aterm& SortExpr_1)
{
  return aterm_appl(function_symbol_StructProj(), StringOrEmpty_0, SortExpr_1);
}

inline
aterm_appl gsMakeSum(const aterm_list& DataVarId_0, const aterm& ProcExpr_1)
{
  return aterm_appl(function_symbol_Sum(), DataVarId_0, ProcExpr_1);
}

inline
aterm_appl gsMakeSync(const aterm& ProcExpr_0, const aterm& ProcExpr_1)
{
  return aterm_appl(function_symbol_Sync(), ProcExpr_0, ProcExpr_1);
}

inline
aterm_appl gsMakeTau()
{
  return aterm_appl(function_symbol_Tau());
}

inline
aterm_appl gsMakeUntypedActMultAct(const aterm_list& UntypedAction_0)
{
  return aterm_appl(function_symbol_UntypedActMultAct(), UntypedAction_0);
}

inline
aterm_appl gsMakeUntypedAction(const aterm& String_0, const aterm_list& DataExpr_1)
{
  return aterm_appl(function_symbol_UntypedAction(), String_0, DataExpr_1);
}

inline
aterm_appl gsMakeUntypedIdentifier(const aterm& String_0)
{
  return aterm_appl(function_symbol_UntypedIdentifier(), String_0);
}

inline
aterm_appl gsMakeUntypedIdentifierAssignment(const aterm& String_0, const aterm& DataExpr_1)
{
  return aterm_appl(function_symbol_UntypedIdentifierAssignment(), String_0, DataExpr_1);
}

inline
aterm_appl gsMakeUntypedMultAct(const aterm_list& UntypedAction_0)
{
  return aterm_appl(function_symbol_UntypedMultAct(), UntypedAction_0);
}

inline
aterm_appl gsMakeUntypedParamId(const aterm& String_0, const aterm_list& DataExpr_1)
{
  return aterm_appl(function_symbol_UntypedParamId(), String_0, DataExpr_1);
}

inline
aterm_appl gsMakeUntypedProcessAssignment(const aterm& String_0, const aterm_list& UntypedIdentifierAssignment_1)
{
  return aterm_appl(function_symbol_UntypedProcessAssignment(), String_0, UntypedIdentifierAssignment_1);
}

inline
aterm_appl gsMakeUntypedSetBagComp()
{
  return aterm_appl(function_symbol_UntypedSetBagComp());
}

inline
aterm_appl gsMakeUntypedSortUnknown()
{
  return aterm_appl(function_symbol_UntypedSortUnknown());
}

inline
aterm_appl gsMakeUntypedSortsPossible(const aterm_list& SortExpr_0)
{
  return aterm_appl(function_symbol_UntypedSortsPossible(), SortExpr_0);
}

inline
aterm_appl gsMakeWhr(const aterm& DataExpr_0, const aterm_list& WhrDecl_1)
{
  return aterm_appl(function_symbol_Whr(), DataExpr_0, WhrDecl_1);
}
//--- end generated code ---//

}
}
}

#endif // MCRL2_LIBSTRUCT_CORE_H
