// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/detail/function_symbols.h
/// \brief add your file description here.

#ifndef MCRL2_CORE_DETAIL_FUNCTION_SYMBOLS_H
#define MCRL2_CORE_DETAIL_FUNCTION_SYMBOLS_H

#include "mcrl2/atermpp/aterm_int.h"
#include "mcrl2/atermpp/aterm_list.h"

namespace mcrl2 {

namespace core {

namespace detail {

//----------------------------------------------------------------------------------------------//
// Part 1: functions for creating function symbols.
//----------------------------------------------------------------------------------------------//

// We use a vector of pointers here, and not a vector of objects. The latter would
// result in references becoming invalid when the vector is resized, i.e., when a new
// element is added. That would mean that function_symbol_DataAppl and function_symbol_DataAppl_helper
// cannot deliver a reference.
// Another solution used in the past is a deque of objects. That turned out to be somewhat slower.
extern std::vector<std::unique_ptr<atermpp::function_symbol>> function_symbols_DataAppl;

inline
const atermpp::function_symbol& function_symbol_DataAppl_helper(std::size_t i)
{
  do
  {
    function_symbols_DataAppl.push_back(std::unique_ptr<atermpp::function_symbol>(new atermpp::function_symbol("DataAppl", function_symbols_DataAppl.size())));
  }
  while (i >= function_symbols_DataAppl.size());
  return *function_symbols_DataAppl[i];
}

inline
const atermpp::function_symbol& function_symbol_DataAppl(std::size_t i)
{
  if (i >= function_symbols_DataAppl.size())
  {
    // This helper is introduced such the function function_symbol_DataAppl, which is called very often,
    // will be inlined.
    return function_symbol_DataAppl_helper(i);
  }
  return *function_symbols_DataAppl[i];
}

inline
bool gsIsDataAppl(const atermpp::aterm_appl& Term)
{
  return Term.function() == function_symbol_DataAppl(Term.function().arity());
}

inline
bool gsIsDataAppl_no_check(const atermpp::aterm_appl& Term)
{
  return Term.function() == *function_symbols_DataAppl[Term.function().arity()];
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

//--- start generated constructors ---//
// ActAnd
inline
const atermpp::function_symbol& function_symbol_ActAnd()
{
  static const atermpp::global_function_symbol function_symbol_ActAnd("ActAnd", 2);
  return function_symbol_ActAnd;
}

// ActAt
inline
const atermpp::function_symbol& function_symbol_ActAt()
{
  static const atermpp::global_function_symbol function_symbol_ActAt("ActAt", 2);
  return function_symbol_ActAt;
}

// ActExists
inline
const atermpp::function_symbol& function_symbol_ActExists()
{
  static const atermpp::global_function_symbol function_symbol_ActExists("ActExists", 2);
  return function_symbol_ActExists;
}

// ActFalse
inline
const atermpp::function_symbol& function_symbol_ActFalse()
{
  static const atermpp::global_function_symbol function_symbol_ActFalse("ActFalse", 0);
  return function_symbol_ActFalse;
}

// ActForall
inline
const atermpp::function_symbol& function_symbol_ActForall()
{
  static const atermpp::global_function_symbol function_symbol_ActForall("ActForall", 2);
  return function_symbol_ActForall;
}

// ActId
inline
const atermpp::function_symbol& function_symbol_ActId()
{
  static const atermpp::global_function_symbol function_symbol_ActId("ActId", 2);
  return function_symbol_ActId;
}

// ActImp
inline
const atermpp::function_symbol& function_symbol_ActImp()
{
  static const atermpp::global_function_symbol function_symbol_ActImp("ActImp", 2);
  return function_symbol_ActImp;
}

// ActMultAct
inline
const atermpp::function_symbol& function_symbol_ActMultAct()
{
  static const atermpp::global_function_symbol function_symbol_ActMultAct("ActMultAct", 1);
  return function_symbol_ActMultAct;
}

// ActNot
inline
const atermpp::function_symbol& function_symbol_ActNot()
{
  static const atermpp::global_function_symbol function_symbol_ActNot("ActNot", 1);
  return function_symbol_ActNot;
}

// ActOr
inline
const atermpp::function_symbol& function_symbol_ActOr()
{
  static const atermpp::global_function_symbol function_symbol_ActOr("ActOr", 2);
  return function_symbol_ActOr;
}

// ActSpec
inline
const atermpp::function_symbol& function_symbol_ActSpec()
{
  static const atermpp::global_function_symbol function_symbol_ActSpec("ActSpec", 1);
  return function_symbol_ActSpec;
}

// ActTrue
inline
const atermpp::function_symbol& function_symbol_ActTrue()
{
  static const atermpp::global_function_symbol function_symbol_ActTrue("ActTrue", 0);
  return function_symbol_ActTrue;
}

// Action
inline
const atermpp::function_symbol& function_symbol_Action()
{
  static const atermpp::global_function_symbol function_symbol_Action("Action", 2);
  return function_symbol_Action;
}

// ActionRenameRule
inline
const atermpp::function_symbol& function_symbol_ActionRenameRule()
{
  static const atermpp::global_function_symbol function_symbol_ActionRenameRule("ActionRenameRule", 4);
  return function_symbol_ActionRenameRule;
}

// ActionRenameRules
inline
const atermpp::function_symbol& function_symbol_ActionRenameRules()
{
  static const atermpp::global_function_symbol function_symbol_ActionRenameRules("ActionRenameRules", 1);
  return function_symbol_ActionRenameRules;
}

// ActionRenameSpec
inline
const atermpp::function_symbol& function_symbol_ActionRenameSpec()
{
  static const atermpp::global_function_symbol function_symbol_ActionRenameSpec("ActionRenameSpec", 3);
  return function_symbol_ActionRenameSpec;
}

// Allow
inline
const atermpp::function_symbol& function_symbol_Allow()
{
  static const atermpp::global_function_symbol function_symbol_Allow("Allow", 2);
  return function_symbol_Allow;
}

// AtTime
inline
const atermpp::function_symbol& function_symbol_AtTime()
{
  static const atermpp::global_function_symbol function_symbol_AtTime("AtTime", 2);
  return function_symbol_AtTime;
}

// BES
inline
const atermpp::function_symbol& function_symbol_BES()
{
  static const atermpp::global_function_symbol function_symbol_BES("BES", 2);
  return function_symbol_BES;
}

// BInit
inline
const atermpp::function_symbol& function_symbol_BInit()
{
  static const atermpp::global_function_symbol function_symbol_BInit("BInit", 2);
  return function_symbol_BInit;
}

// BagComp
inline
const atermpp::function_symbol& function_symbol_BagComp()
{
  static const atermpp::global_function_symbol function_symbol_BagComp("BagComp", 0);
  return function_symbol_BagComp;
}

// BddFalse
inline
const atermpp::function_symbol& function_symbol_BddFalse()
{
  static const atermpp::global_function_symbol function_symbol_BddFalse("BddFalse", 0);
  return function_symbol_BddFalse;
}

// BddIf
inline
const atermpp::function_symbol& function_symbol_BddIf()
{
  static const atermpp::global_function_symbol function_symbol_BddIf("BddIf", 3);
  return function_symbol_BddIf;
}

// BddTrue
inline
const atermpp::function_symbol& function_symbol_BddTrue()
{
  static const atermpp::global_function_symbol function_symbol_BddTrue("BddTrue", 0);
  return function_symbol_BddTrue;
}

// Binder
inline
const atermpp::function_symbol& function_symbol_Binder()
{
  static const atermpp::global_function_symbol function_symbol_Binder("Binder", 3);
  return function_symbol_Binder;
}

// Block
inline
const atermpp::function_symbol& function_symbol_Block()
{
  static const atermpp::global_function_symbol function_symbol_Block("Block", 2);
  return function_symbol_Block;
}

// BooleanAnd
inline
const atermpp::function_symbol& function_symbol_BooleanAnd()
{
  static const atermpp::global_function_symbol function_symbol_BooleanAnd("BooleanAnd", 2);
  return function_symbol_BooleanAnd;
}

// BooleanEquation
inline
const atermpp::function_symbol& function_symbol_BooleanEquation()
{
  static const atermpp::global_function_symbol function_symbol_BooleanEquation("BooleanEquation", 3);
  return function_symbol_BooleanEquation;
}

// BooleanFalse
inline
const atermpp::function_symbol& function_symbol_BooleanFalse()
{
  static const atermpp::global_function_symbol function_symbol_BooleanFalse("BooleanFalse", 0);
  return function_symbol_BooleanFalse;
}

// BooleanImp
inline
const atermpp::function_symbol& function_symbol_BooleanImp()
{
  static const atermpp::global_function_symbol function_symbol_BooleanImp("BooleanImp", 2);
  return function_symbol_BooleanImp;
}

// BooleanNot
inline
const atermpp::function_symbol& function_symbol_BooleanNot()
{
  static const atermpp::global_function_symbol function_symbol_BooleanNot("BooleanNot", 1);
  return function_symbol_BooleanNot;
}

// BooleanOr
inline
const atermpp::function_symbol& function_symbol_BooleanOr()
{
  static const atermpp::global_function_symbol function_symbol_BooleanOr("BooleanOr", 2);
  return function_symbol_BooleanOr;
}

// BooleanTrue
inline
const atermpp::function_symbol& function_symbol_BooleanTrue()
{
  static const atermpp::global_function_symbol function_symbol_BooleanTrue("BooleanTrue", 0);
  return function_symbol_BooleanTrue;
}

// BooleanVariable
inline
const atermpp::function_symbol& function_symbol_BooleanVariable()
{
  static const atermpp::global_function_symbol function_symbol_BooleanVariable("BooleanVariable", 2);
  return function_symbol_BooleanVariable;
}

// Choice
inline
const atermpp::function_symbol& function_symbol_Choice()
{
  static const atermpp::global_function_symbol function_symbol_Choice("Choice", 2);
  return function_symbol_Choice;
}

// Comm
inline
const atermpp::function_symbol& function_symbol_Comm()
{
  static const atermpp::global_function_symbol function_symbol_Comm("Comm", 2);
  return function_symbol_Comm;
}

// CommExpr
inline
const atermpp::function_symbol& function_symbol_CommExpr()
{
  static const atermpp::global_function_symbol function_symbol_CommExpr("CommExpr", 2);
  return function_symbol_CommExpr;
}

// ConsSpec
inline
const atermpp::function_symbol& function_symbol_ConsSpec()
{
  static const atermpp::global_function_symbol function_symbol_ConsSpec("ConsSpec", 1);
  return function_symbol_ConsSpec;
}

// DataEqn
inline
const atermpp::function_symbol& function_symbol_DataEqn()
{
  static const atermpp::global_function_symbol function_symbol_DataEqn("DataEqn", 4);
  return function_symbol_DataEqn;
}

// DataEqnSpec
inline
const atermpp::function_symbol& function_symbol_DataEqnSpec()
{
  static const atermpp::global_function_symbol function_symbol_DataEqnSpec("DataEqnSpec", 1);
  return function_symbol_DataEqnSpec;
}

// DataSpec
inline
const atermpp::function_symbol& function_symbol_DataSpec()
{
  static const atermpp::global_function_symbol function_symbol_DataSpec("DataSpec", 4);
  return function_symbol_DataSpec;
}

// DataVarId
inline
const atermpp::function_symbol& function_symbol_DataVarId()
{
  static const atermpp::global_function_symbol function_symbol_DataVarId("DataVarId", 3);
  return function_symbol_DataVarId;
}

// DataVarIdInit
inline
const atermpp::function_symbol& function_symbol_DataVarIdInit()
{
  static const atermpp::global_function_symbol function_symbol_DataVarIdInit("DataVarIdInit", 2);
  return function_symbol_DataVarIdInit;
}

// Delta
inline
const atermpp::function_symbol& function_symbol_Delta()
{
  static const atermpp::global_function_symbol function_symbol_Delta("Delta", 0);
  return function_symbol_Delta;
}

// Distribution
inline
const atermpp::function_symbol& function_symbol_Distribution()
{
  static const atermpp::global_function_symbol function_symbol_Distribution("Distribution", 2);
  return function_symbol_Distribution;
}

// Exists
inline
const atermpp::function_symbol& function_symbol_Exists()
{
  static const atermpp::global_function_symbol function_symbol_Exists("Exists", 0);
  return function_symbol_Exists;
}

// Forall
inline
const atermpp::function_symbol& function_symbol_Forall()
{
  static const atermpp::global_function_symbol function_symbol_Forall("Forall", 0);
  return function_symbol_Forall;
}

// GlobVarSpec
inline
const atermpp::function_symbol& function_symbol_GlobVarSpec()
{
  static const atermpp::global_function_symbol function_symbol_GlobVarSpec("GlobVarSpec", 1);
  return function_symbol_GlobVarSpec;
}

// Hide
inline
const atermpp::function_symbol& function_symbol_Hide()
{
  static const atermpp::global_function_symbol function_symbol_Hide("Hide", 2);
  return function_symbol_Hide;
}

// IfThen
inline
const atermpp::function_symbol& function_symbol_IfThen()
{
  static const atermpp::global_function_symbol function_symbol_IfThen("IfThen", 2);
  return function_symbol_IfThen;
}

// IfThenElse
inline
const atermpp::function_symbol& function_symbol_IfThenElse()
{
  static const atermpp::global_function_symbol function_symbol_IfThenElse("IfThenElse", 3);
  return function_symbol_IfThenElse;
}

// LMerge
inline
const atermpp::function_symbol& function_symbol_LMerge()
{
  static const atermpp::global_function_symbol function_symbol_LMerge("LMerge", 2);
  return function_symbol_LMerge;
}

// Lambda
inline
const atermpp::function_symbol& function_symbol_Lambda()
{
  static const atermpp::global_function_symbol function_symbol_Lambda("Lambda", 0);
  return function_symbol_Lambda;
}

// LinProcSpec
inline
const atermpp::function_symbol& function_symbol_LinProcSpec()
{
  static const atermpp::global_function_symbol function_symbol_LinProcSpec("LinProcSpec", 5);
  return function_symbol_LinProcSpec;
}

// LinearProcess
inline
const atermpp::function_symbol& function_symbol_LinearProcess()
{
  static const atermpp::global_function_symbol function_symbol_LinearProcess("LinearProcess", 2);
  return function_symbol_LinearProcess;
}

// LinearProcessInit
inline
const atermpp::function_symbol& function_symbol_LinearProcessInit()
{
  static const atermpp::global_function_symbol function_symbol_LinearProcessInit("LinearProcessInit", 2);
  return function_symbol_LinearProcessInit;
}

// LinearProcessSummand
inline
const atermpp::function_symbol& function_symbol_LinearProcessSummand()
{
  static const atermpp::global_function_symbol function_symbol_LinearProcessSummand("LinearProcessSummand", 6);
  return function_symbol_LinearProcessSummand;
}

// MapSpec
inline
const atermpp::function_symbol& function_symbol_MapSpec()
{
  static const atermpp::global_function_symbol function_symbol_MapSpec("MapSpec", 1);
  return function_symbol_MapSpec;
}

// Merge
inline
const atermpp::function_symbol& function_symbol_Merge()
{
  static const atermpp::global_function_symbol function_symbol_Merge("Merge", 2);
  return function_symbol_Merge;
}

// Mu
inline
const atermpp::function_symbol& function_symbol_Mu()
{
  static const atermpp::global_function_symbol function_symbol_Mu("Mu", 0);
  return function_symbol_Mu;
}

// MultAct
inline
const atermpp::function_symbol& function_symbol_MultAct()
{
  static const atermpp::global_function_symbol function_symbol_MultAct("MultAct", 1);
  return function_symbol_MultAct;
}

// MultActName
inline
const atermpp::function_symbol& function_symbol_MultActName()
{
  static const atermpp::global_function_symbol function_symbol_MultActName("MultActName", 1);
  return function_symbol_MultActName;
}

// Nu
inline
const atermpp::function_symbol& function_symbol_Nu()
{
  static const atermpp::global_function_symbol function_symbol_Nu("Nu", 0);
  return function_symbol_Nu;
}

// OpId
inline
const atermpp::function_symbol& function_symbol_OpId()
{
  static const atermpp::global_function_symbol function_symbol_OpId("OpId", 3);
  return function_symbol_OpId;
}

// PBES
inline
const atermpp::function_symbol& function_symbol_PBES()
{
  static const atermpp::global_function_symbol function_symbol_PBES("PBES", 4);
  return function_symbol_PBES;
}

// PBESAnd
inline
const atermpp::function_symbol& function_symbol_PBESAnd()
{
  static const atermpp::global_function_symbol function_symbol_PBESAnd("PBESAnd", 2);
  return function_symbol_PBESAnd;
}

// PBESExists
inline
const atermpp::function_symbol& function_symbol_PBESExists()
{
  static const atermpp::global_function_symbol function_symbol_PBESExists("PBESExists", 2);
  return function_symbol_PBESExists;
}

// PBESFalse
inline
const atermpp::function_symbol& function_symbol_PBESFalse()
{
  static const atermpp::global_function_symbol function_symbol_PBESFalse("PBESFalse", 0);
  return function_symbol_PBESFalse;
}

// PBESForall
inline
const atermpp::function_symbol& function_symbol_PBESForall()
{
  static const atermpp::global_function_symbol function_symbol_PBESForall("PBESForall", 2);
  return function_symbol_PBESForall;
}

// PBESImp
inline
const atermpp::function_symbol& function_symbol_PBESImp()
{
  static const atermpp::global_function_symbol function_symbol_PBESImp("PBESImp", 2);
  return function_symbol_PBESImp;
}

// PBESNot
inline
const atermpp::function_symbol& function_symbol_PBESNot()
{
  static const atermpp::global_function_symbol function_symbol_PBESNot("PBESNot", 1);
  return function_symbol_PBESNot;
}

// PBESOr
inline
const atermpp::function_symbol& function_symbol_PBESOr()
{
  static const atermpp::global_function_symbol function_symbol_PBESOr("PBESOr", 2);
  return function_symbol_PBESOr;
}

// PBESTrue
inline
const atermpp::function_symbol& function_symbol_PBESTrue()
{
  static const atermpp::global_function_symbol function_symbol_PBESTrue("PBESTrue", 0);
  return function_symbol_PBESTrue;
}

// PBEqn
inline
const atermpp::function_symbol& function_symbol_PBEqn()
{
  static const atermpp::global_function_symbol function_symbol_PBEqn("PBEqn", 3);
  return function_symbol_PBEqn;
}

// PBEqnSpec
inline
const atermpp::function_symbol& function_symbol_PBEqnSpec()
{
  static const atermpp::global_function_symbol function_symbol_PBEqnSpec("PBEqnSpec", 1);
  return function_symbol_PBEqnSpec;
}

// PBInit
inline
const atermpp::function_symbol& function_symbol_PBInit()
{
  static const atermpp::global_function_symbol function_symbol_PBInit("PBInit", 1);
  return function_symbol_PBInit;
}

// ProcEqn
inline
const atermpp::function_symbol& function_symbol_ProcEqn()
{
  static const atermpp::global_function_symbol function_symbol_ProcEqn("ProcEqn", 3);
  return function_symbol_ProcEqn;
}

// ProcEqnSpec
inline
const atermpp::function_symbol& function_symbol_ProcEqnSpec()
{
  static const atermpp::global_function_symbol function_symbol_ProcEqnSpec("ProcEqnSpec", 1);
  return function_symbol_ProcEqnSpec;
}

// ProcSpec
inline
const atermpp::function_symbol& function_symbol_ProcSpec()
{
  static const atermpp::global_function_symbol function_symbol_ProcSpec("ProcSpec", 5);
  return function_symbol_ProcSpec;
}

// ProcVarId
inline
const atermpp::function_symbol& function_symbol_ProcVarId()
{
  static const atermpp::global_function_symbol function_symbol_ProcVarId("ProcVarId", 3);
  return function_symbol_ProcVarId;
}

// Process
inline
const atermpp::function_symbol& function_symbol_Process()
{
  static const atermpp::global_function_symbol function_symbol_Process("Process", 2);
  return function_symbol_Process;
}

// ProcessAssignment
inline
const atermpp::function_symbol& function_symbol_ProcessAssignment()
{
  static const atermpp::global_function_symbol function_symbol_ProcessAssignment("ProcessAssignment", 2);
  return function_symbol_ProcessAssignment;
}

// ProcessInit
inline
const atermpp::function_symbol& function_symbol_ProcessInit()
{
  static const atermpp::global_function_symbol function_symbol_ProcessInit("ProcessInit", 1);
  return function_symbol_ProcessInit;
}

// PropVarDecl
inline
const atermpp::function_symbol& function_symbol_PropVarDecl()
{
  static const atermpp::global_function_symbol function_symbol_PropVarDecl("PropVarDecl", 2);
  return function_symbol_PropVarDecl;
}

// PropVarInst
inline
const atermpp::function_symbol& function_symbol_PropVarInst()
{
  static const atermpp::global_function_symbol function_symbol_PropVarInst("PropVarInst", 3);
  return function_symbol_PropVarInst;
}

// RegAlt
inline
const atermpp::function_symbol& function_symbol_RegAlt()
{
  static const atermpp::global_function_symbol function_symbol_RegAlt("RegAlt", 2);
  return function_symbol_RegAlt;
}

// RegNil
inline
const atermpp::function_symbol& function_symbol_RegNil()
{
  static const atermpp::global_function_symbol function_symbol_RegNil("RegNil", 0);
  return function_symbol_RegNil;
}

// RegSeq
inline
const atermpp::function_symbol& function_symbol_RegSeq()
{
  static const atermpp::global_function_symbol function_symbol_RegSeq("RegSeq", 2);
  return function_symbol_RegSeq;
}

// RegTrans
inline
const atermpp::function_symbol& function_symbol_RegTrans()
{
  static const atermpp::global_function_symbol function_symbol_RegTrans("RegTrans", 1);
  return function_symbol_RegTrans;
}

// RegTransOrNil
inline
const atermpp::function_symbol& function_symbol_RegTransOrNil()
{
  static const atermpp::global_function_symbol function_symbol_RegTransOrNil("RegTransOrNil", 1);
  return function_symbol_RegTransOrNil;
}

// Rename
inline
const atermpp::function_symbol& function_symbol_Rename()
{
  static const atermpp::global_function_symbol function_symbol_Rename("Rename", 2);
  return function_symbol_Rename;
}

// RenameExpr
inline
const atermpp::function_symbol& function_symbol_RenameExpr()
{
  static const atermpp::global_function_symbol function_symbol_RenameExpr("RenameExpr", 2);
  return function_symbol_RenameExpr;
}

// Seq
inline
const atermpp::function_symbol& function_symbol_Seq()
{
  static const atermpp::global_function_symbol function_symbol_Seq("Seq", 2);
  return function_symbol_Seq;
}

// SetComp
inline
const atermpp::function_symbol& function_symbol_SetComp()
{
  static const atermpp::global_function_symbol function_symbol_SetComp("SetComp", 0);
  return function_symbol_SetComp;
}

// SortArrow
inline
const atermpp::function_symbol& function_symbol_SortArrow()
{
  static const atermpp::global_function_symbol function_symbol_SortArrow("SortArrow", 2);
  return function_symbol_SortArrow;
}

// SortBag
inline
const atermpp::function_symbol& function_symbol_SortBag()
{
  static const atermpp::global_function_symbol function_symbol_SortBag("SortBag", 0);
  return function_symbol_SortBag;
}

// SortCons
inline
const atermpp::function_symbol& function_symbol_SortCons()
{
  static const atermpp::global_function_symbol function_symbol_SortCons("SortCons", 2);
  return function_symbol_SortCons;
}

// SortFBag
inline
const atermpp::function_symbol& function_symbol_SortFBag()
{
  static const atermpp::global_function_symbol function_symbol_SortFBag("SortFBag", 0);
  return function_symbol_SortFBag;
}

// SortFSet
inline
const atermpp::function_symbol& function_symbol_SortFSet()
{
  static const atermpp::global_function_symbol function_symbol_SortFSet("SortFSet", 0);
  return function_symbol_SortFSet;
}

// SortId
inline
const atermpp::function_symbol& function_symbol_SortId()
{
  static const atermpp::global_function_symbol function_symbol_SortId("SortId", 1);
  return function_symbol_SortId;
}

// SortList
inline
const atermpp::function_symbol& function_symbol_SortList()
{
  static const atermpp::global_function_symbol function_symbol_SortList("SortList", 0);
  return function_symbol_SortList;
}

// SortRef
inline
const atermpp::function_symbol& function_symbol_SortRef()
{
  static const atermpp::global_function_symbol function_symbol_SortRef("SortRef", 2);
  return function_symbol_SortRef;
}

// SortSet
inline
const atermpp::function_symbol& function_symbol_SortSet()
{
  static const atermpp::global_function_symbol function_symbol_SortSet("SortSet", 0);
  return function_symbol_SortSet;
}

// SortSpec
inline
const atermpp::function_symbol& function_symbol_SortSpec()
{
  static const atermpp::global_function_symbol function_symbol_SortSpec("SortSpec", 1);
  return function_symbol_SortSpec;
}

// SortStruct
inline
const atermpp::function_symbol& function_symbol_SortStruct()
{
  static const atermpp::global_function_symbol function_symbol_SortStruct("SortStruct", 1);
  return function_symbol_SortStruct;
}

// StateAnd
inline
const atermpp::function_symbol& function_symbol_StateAnd()
{
  static const atermpp::global_function_symbol function_symbol_StateAnd("StateAnd", 2);
  return function_symbol_StateAnd;
}

// StateDelay
inline
const atermpp::function_symbol& function_symbol_StateDelay()
{
  static const atermpp::global_function_symbol function_symbol_StateDelay("StateDelay", 0);
  return function_symbol_StateDelay;
}

// StateDelayTimed
inline
const atermpp::function_symbol& function_symbol_StateDelayTimed()
{
  static const atermpp::global_function_symbol function_symbol_StateDelayTimed("StateDelayTimed", 1);
  return function_symbol_StateDelayTimed;
}

// StateExists
inline
const atermpp::function_symbol& function_symbol_StateExists()
{
  static const atermpp::global_function_symbol function_symbol_StateExists("StateExists", 2);
  return function_symbol_StateExists;
}

// StateFalse
inline
const atermpp::function_symbol& function_symbol_StateFalse()
{
  static const atermpp::global_function_symbol function_symbol_StateFalse("StateFalse", 0);
  return function_symbol_StateFalse;
}

// StateForall
inline
const atermpp::function_symbol& function_symbol_StateForall()
{
  static const atermpp::global_function_symbol function_symbol_StateForall("StateForall", 2);
  return function_symbol_StateForall;
}

// StateImp
inline
const atermpp::function_symbol& function_symbol_StateImp()
{
  static const atermpp::global_function_symbol function_symbol_StateImp("StateImp", 2);
  return function_symbol_StateImp;
}

// StateMay
inline
const atermpp::function_symbol& function_symbol_StateMay()
{
  static const atermpp::global_function_symbol function_symbol_StateMay("StateMay", 2);
  return function_symbol_StateMay;
}

// StateMu
inline
const atermpp::function_symbol& function_symbol_StateMu()
{
  static const atermpp::global_function_symbol function_symbol_StateMu("StateMu", 3);
  return function_symbol_StateMu;
}

// StateMust
inline
const atermpp::function_symbol& function_symbol_StateMust()
{
  static const atermpp::global_function_symbol function_symbol_StateMust("StateMust", 2);
  return function_symbol_StateMust;
}

// StateNot
inline
const atermpp::function_symbol& function_symbol_StateNot()
{
  static const atermpp::global_function_symbol function_symbol_StateNot("StateNot", 1);
  return function_symbol_StateNot;
}

// StateNu
inline
const atermpp::function_symbol& function_symbol_StateNu()
{
  static const atermpp::global_function_symbol function_symbol_StateNu("StateNu", 3);
  return function_symbol_StateNu;
}

// StateOr
inline
const atermpp::function_symbol& function_symbol_StateOr()
{
  static const atermpp::global_function_symbol function_symbol_StateOr("StateOr", 2);
  return function_symbol_StateOr;
}

// StateTrue
inline
const atermpp::function_symbol& function_symbol_StateTrue()
{
  static const atermpp::global_function_symbol function_symbol_StateTrue("StateTrue", 0);
  return function_symbol_StateTrue;
}

// StateVar
inline
const atermpp::function_symbol& function_symbol_StateVar()
{
  static const atermpp::global_function_symbol function_symbol_StateVar("StateVar", 2);
  return function_symbol_StateVar;
}

// StateYaled
inline
const atermpp::function_symbol& function_symbol_StateYaled()
{
  static const atermpp::global_function_symbol function_symbol_StateYaled("StateYaled", 0);
  return function_symbol_StateYaled;
}

// StateYaledTimed
inline
const atermpp::function_symbol& function_symbol_StateYaledTimed()
{
  static const atermpp::global_function_symbol function_symbol_StateYaledTimed("StateYaledTimed", 1);
  return function_symbol_StateYaledTimed;
}

// StochasticOperator
inline
const atermpp::function_symbol& function_symbol_StochasticOperator()
{
  static const atermpp::global_function_symbol function_symbol_StochasticOperator("StochasticOperator", 3);
  return function_symbol_StochasticOperator;
}

// StructCons
inline
const atermpp::function_symbol& function_symbol_StructCons()
{
  static const atermpp::global_function_symbol function_symbol_StructCons("StructCons", 3);
  return function_symbol_StructCons;
}

// StructProj
inline
const atermpp::function_symbol& function_symbol_StructProj()
{
  static const atermpp::global_function_symbol function_symbol_StructProj("StructProj", 2);
  return function_symbol_StructProj;
}

// Sum
inline
const atermpp::function_symbol& function_symbol_Sum()
{
  static const atermpp::global_function_symbol function_symbol_Sum("Sum", 2);
  return function_symbol_Sum;
}

// Sync
inline
const atermpp::function_symbol& function_symbol_Sync()
{
  static const atermpp::global_function_symbol function_symbol_Sync("Sync", 2);
  return function_symbol_Sync;
}

// Tau
inline
const atermpp::function_symbol& function_symbol_Tau()
{
  static const atermpp::global_function_symbol function_symbol_Tau("Tau", 0);
  return function_symbol_Tau;
}

// TimedMultAct
inline
const atermpp::function_symbol& function_symbol_TimedMultAct()
{
  static const atermpp::global_function_symbol function_symbol_TimedMultAct("TimedMultAct", 2);
  return function_symbol_TimedMultAct;
}

// UntypedDataParameter
inline
const atermpp::function_symbol& function_symbol_UntypedDataParameter()
{
  static const atermpp::global_function_symbol function_symbol_UntypedDataParameter("UntypedDataParameter", 2);
  return function_symbol_UntypedDataParameter;
}

// UntypedIdentifier
inline
const atermpp::function_symbol& function_symbol_UntypedIdentifier()
{
  static const atermpp::global_function_symbol function_symbol_UntypedIdentifier("UntypedIdentifier", 1);
  return function_symbol_UntypedIdentifier;
}

// UntypedIdentifierAssignment
inline
const atermpp::function_symbol& function_symbol_UntypedIdentifierAssignment()
{
  static const atermpp::global_function_symbol function_symbol_UntypedIdentifierAssignment("UntypedIdentifierAssignment", 2);
  return function_symbol_UntypedIdentifierAssignment;
}

// UntypedMultiAction
inline
const atermpp::function_symbol& function_symbol_UntypedMultiAction()
{
  static const atermpp::global_function_symbol function_symbol_UntypedMultiAction("UntypedMultiAction", 1);
  return function_symbol_UntypedMultiAction;
}

// UntypedProcessAssignment
inline
const atermpp::function_symbol& function_symbol_UntypedProcessAssignment()
{
  static const atermpp::global_function_symbol function_symbol_UntypedProcessAssignment("UntypedProcessAssignment", 2);
  return function_symbol_UntypedProcessAssignment;
}

// UntypedRegFrm
inline
const atermpp::function_symbol& function_symbol_UntypedRegFrm()
{
  static const atermpp::global_function_symbol function_symbol_UntypedRegFrm("UntypedRegFrm", 3);
  return function_symbol_UntypedRegFrm;
}

// UntypedSetBagComp
inline
const atermpp::function_symbol& function_symbol_UntypedSetBagComp()
{
  static const atermpp::global_function_symbol function_symbol_UntypedSetBagComp("UntypedSetBagComp", 0);
  return function_symbol_UntypedSetBagComp;
}

// UntypedSortUnknown
inline
const atermpp::function_symbol& function_symbol_UntypedSortUnknown()
{
  static const atermpp::global_function_symbol function_symbol_UntypedSortUnknown("UntypedSortUnknown", 0);
  return function_symbol_UntypedSortUnknown;
}

// UntypedSortVariable
inline
const atermpp::function_symbol& function_symbol_UntypedSortVariable()
{
  static const atermpp::global_function_symbol function_symbol_UntypedSortVariable("UntypedSortVariable", 1);
  return function_symbol_UntypedSortVariable;
}

// UntypedSortsPossible
inline
const atermpp::function_symbol& function_symbol_UntypedSortsPossible()
{
  static const atermpp::global_function_symbol function_symbol_UntypedSortsPossible("UntypedSortsPossible", 1);
  return function_symbol_UntypedSortsPossible;
}

// Whr
inline
const atermpp::function_symbol& function_symbol_Whr()
{
  static const atermpp::global_function_symbol function_symbol_Whr("Whr", 2);
  return function_symbol_Whr;
}
//--- end generated constructors ---//

//----------------------------------------------------------------------------------------------//
// Part 2: static variables containing function symbols.
//----------------------------------------------------------------------------------------------//

struct function_symbols
{
//--- start generated variables ---//
static const atermpp::function_symbol SortCons;
  static const atermpp::function_symbol SortStruct;
  static const atermpp::function_symbol SortArrow;
  static const atermpp::function_symbol UntypedSortUnknown;
  static const atermpp::function_symbol UntypedSortsPossible;
  static const atermpp::function_symbol UntypedSortVariable;
  static const atermpp::function_symbol SortId;
  static const atermpp::function_symbol SortList;
  static const atermpp::function_symbol SortSet;
  static const atermpp::function_symbol SortBag;
  static const atermpp::function_symbol SortFSet;
  static const atermpp::function_symbol SortFBag;
  static const atermpp::function_symbol StructCons;
  static const atermpp::function_symbol StructProj;
  static const atermpp::function_symbol Binder;
  static const atermpp::function_symbol Whr;
  static const atermpp::function_symbol UntypedIdentifier;
  static const atermpp::function_symbol DataVarId;
  static const atermpp::function_symbol OpId;
  static const atermpp::function_symbol UntypedDataParameter;
  static const atermpp::function_symbol Forall;
  static const atermpp::function_symbol Exists;
  static const atermpp::function_symbol SetComp;
  static const atermpp::function_symbol BagComp;
  static const atermpp::function_symbol Lambda;
  static const atermpp::function_symbol UntypedSetBagComp;
  static const atermpp::function_symbol DataVarIdInit;
  static const atermpp::function_symbol UntypedIdentifierAssignment;
  static const atermpp::function_symbol DataSpec;
  static const atermpp::function_symbol SortSpec;
  static const atermpp::function_symbol ConsSpec;
  static const atermpp::function_symbol MapSpec;
  static const atermpp::function_symbol DataEqnSpec;
  static const atermpp::function_symbol SortRef;
  static const atermpp::function_symbol DataEqn;
  static const atermpp::function_symbol MultAct;
  static const atermpp::function_symbol TimedMultAct;
  static const atermpp::function_symbol UntypedMultiAction;
  static const atermpp::function_symbol Action;
  static const atermpp::function_symbol ActId;
  static const atermpp::function_symbol Process;
  static const atermpp::function_symbol ProcessAssignment;
  static const atermpp::function_symbol Delta;
  static const atermpp::function_symbol Tau;
  static const atermpp::function_symbol Sum;
  static const atermpp::function_symbol Block;
  static const atermpp::function_symbol Hide;
  static const atermpp::function_symbol Rename;
  static const atermpp::function_symbol Comm;
  static const atermpp::function_symbol Allow;
  static const atermpp::function_symbol Sync;
  static const atermpp::function_symbol AtTime;
  static const atermpp::function_symbol Seq;
  static const atermpp::function_symbol IfThen;
  static const atermpp::function_symbol IfThenElse;
  static const atermpp::function_symbol BInit;
  static const atermpp::function_symbol Merge;
  static const atermpp::function_symbol LMerge;
  static const atermpp::function_symbol Choice;
  static const atermpp::function_symbol StochasticOperator;
  static const atermpp::function_symbol UntypedProcessAssignment;
  static const atermpp::function_symbol ProcVarId;
  static const atermpp::function_symbol MultActName;
  static const atermpp::function_symbol RenameExpr;
  static const atermpp::function_symbol CommExpr;
  static const atermpp::function_symbol ProcSpec;
  static const atermpp::function_symbol ActSpec;
  static const atermpp::function_symbol GlobVarSpec;
  static const atermpp::function_symbol ProcEqnSpec;
  static const atermpp::function_symbol ProcEqn;
  static const atermpp::function_symbol ProcessInit;
  static const atermpp::function_symbol Distribution;
  static const atermpp::function_symbol LinProcSpec;
  static const atermpp::function_symbol LinearProcess;
  static const atermpp::function_symbol LinearProcessSummand;
  static const atermpp::function_symbol LinearProcessInit;
  static const atermpp::function_symbol StateTrue;
  static const atermpp::function_symbol StateFalse;
  static const atermpp::function_symbol StateNot;
  static const atermpp::function_symbol StateAnd;
  static const atermpp::function_symbol StateOr;
  static const atermpp::function_symbol StateImp;
  static const atermpp::function_symbol StateForall;
  static const atermpp::function_symbol StateExists;
  static const atermpp::function_symbol StateMust;
  static const atermpp::function_symbol StateMay;
  static const atermpp::function_symbol StateYaled;
  static const atermpp::function_symbol StateYaledTimed;
  static const atermpp::function_symbol StateDelay;
  static const atermpp::function_symbol StateDelayTimed;
  static const atermpp::function_symbol StateVar;
  static const atermpp::function_symbol StateNu;
  static const atermpp::function_symbol StateMu;
  static const atermpp::function_symbol RegNil;
  static const atermpp::function_symbol RegSeq;
  static const atermpp::function_symbol RegAlt;
  static const atermpp::function_symbol RegTrans;
  static const atermpp::function_symbol RegTransOrNil;
  static const atermpp::function_symbol UntypedRegFrm;
  static const atermpp::function_symbol ActTrue;
  static const atermpp::function_symbol ActFalse;
  static const atermpp::function_symbol ActNot;
  static const atermpp::function_symbol ActAnd;
  static const atermpp::function_symbol ActOr;
  static const atermpp::function_symbol ActImp;
  static const atermpp::function_symbol ActForall;
  static const atermpp::function_symbol ActExists;
  static const atermpp::function_symbol ActAt;
  static const atermpp::function_symbol ActMultAct;
  static const atermpp::function_symbol ActionRenameRules;
  static const atermpp::function_symbol ActionRenameRule;
  static const atermpp::function_symbol ActionRenameSpec;
  static const atermpp::function_symbol PBES;
  static const atermpp::function_symbol PBEqnSpec;
  static const atermpp::function_symbol PBInit;
  static const atermpp::function_symbol PBEqn;
  static const atermpp::function_symbol Mu;
  static const atermpp::function_symbol Nu;
  static const atermpp::function_symbol PropVarDecl;
  static const atermpp::function_symbol PBESTrue;
  static const atermpp::function_symbol PBESFalse;
  static const atermpp::function_symbol PBESNot;
  static const atermpp::function_symbol PBESAnd;
  static const atermpp::function_symbol PBESOr;
  static const atermpp::function_symbol PBESImp;
  static const atermpp::function_symbol PBESForall;
  static const atermpp::function_symbol PBESExists;
  static const atermpp::function_symbol PropVarInst;
  static const atermpp::function_symbol BES;
  static const atermpp::function_symbol BooleanEquation;
  static const atermpp::function_symbol BooleanVariable;
  static const atermpp::function_symbol BooleanTrue;
  static const atermpp::function_symbol BooleanFalse;
  static const atermpp::function_symbol BooleanNot;
  static const atermpp::function_symbol BooleanAnd;
  static const atermpp::function_symbol BooleanOr;
  static const atermpp::function_symbol BooleanImp;
  static const atermpp::function_symbol BddTrue;
  static const atermpp::function_symbol BddFalse;
  static const atermpp::function_symbol BddIf;
//--- end generated variables ---//
};

} // namespace detail

} // namespace core

} // namespace mcrl2

#endif // MCRL2_CORE_DETAIL_FUNCTION_SYMBOLS_H
