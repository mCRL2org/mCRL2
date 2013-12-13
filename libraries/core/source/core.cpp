// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file core.cpp

#include "mcrl2/atermpp/detail/utility.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/core/print.h"
#include "mcrl2/core/detail/function_symbols.h"

namespace mcrl2 {

namespace core {

namespace detail {

atermpp::function_symbol function_symbol_OpId_ = atermpp::function_symbol("OpId", 3);
atermpp::function_symbol function_symbol_DataVarId_ = atermpp::function_symbol("DataVarId", 3);
std::vector<atermpp::function_symbol> function_symbols_DataAppl_;

} // namespace detail

//--- start generated core overloads ---//
std::string pp(const core::identifier_string& x) { return core::pp< core::identifier_string >(x); }
std::string pp(const core::nil& x) { return core::pp< core::nil >(x); }
//--- end generated core overloads ---//

//--- start generated function symbol definitions ---//
atermpp::function_symbol core::detail::function_symbols::BooleanOr = core::detail::function_symbol_BooleanOr();
  atermpp::function_symbol core::detail::function_symbols::StateOr = core::detail::function_symbol_StateOr();
  atermpp::function_symbol core::detail::function_symbols::Hide = core::detail::function_symbol_Hide();
  atermpp::function_symbol core::detail::function_symbols::SortArrow = core::detail::function_symbol_SortArrow();
  atermpp::function_symbol core::detail::function_symbols::ProcessAssignment = core::detail::function_symbol_ProcessAssignment();
  atermpp::function_symbol core::detail::function_symbols::Forall = core::detail::function_symbol_Forall();
  atermpp::function_symbol core::detail::function_symbols::CommExpr = core::detail::function_symbol_CommExpr();
  atermpp::function_symbol core::detail::function_symbols::StateNot = core::detail::function_symbol_StateNot();
  atermpp::function_symbol core::detail::function_symbols::UntypedSetBagComp = core::detail::function_symbol_UntypedSetBagComp();
  atermpp::function_symbol core::detail::function_symbols::SortFSet = core::detail::function_symbol_SortFSet();
  atermpp::function_symbol core::detail::function_symbols::StateImp = core::detail::function_symbol_StateImp();
  atermpp::function_symbol core::detail::function_symbols::PBESExists = core::detail::function_symbol_PBESExists();
  atermpp::function_symbol core::detail::function_symbols::PBESImp = core::detail::function_symbol_PBESImp();
  atermpp::function_symbol core::detail::function_symbols::Binder = core::detail::function_symbol_Binder();
  atermpp::function_symbol core::detail::function_symbols::SortRef = core::detail::function_symbol_SortRef();
  atermpp::function_symbol core::detail::function_symbols::ProcEqnSpec = core::detail::function_symbol_ProcEqnSpec();
  atermpp::function_symbol core::detail::function_symbols::StateForall = core::detail::function_symbol_StateForall();
  atermpp::function_symbol core::detail::function_symbols::BooleanImp = core::detail::function_symbol_BooleanImp();
  atermpp::function_symbol core::detail::function_symbols::SortId = core::detail::function_symbol_SortId();
  atermpp::function_symbol core::detail::function_symbols::UntypedAction = core::detail::function_symbol_UntypedAction();
  atermpp::function_symbol core::detail::function_symbols::StateNu = core::detail::function_symbol_StateNu();
  atermpp::function_symbol core::detail::function_symbols::RegNil = core::detail::function_symbol_RegNil();
  atermpp::function_symbol core::detail::function_symbols::DataSpec = core::detail::function_symbol_DataSpec();
  atermpp::function_symbol core::detail::function_symbols::UntypedActMultAct = core::detail::function_symbol_UntypedActMultAct();
  atermpp::function_symbol core::detail::function_symbols::Tau = core::detail::function_symbol_Tau();
  atermpp::function_symbol core::detail::function_symbols::StateYaledTimed = core::detail::function_symbol_StateYaledTimed();
  atermpp::function_symbol core::detail::function_symbols::SortCons = core::detail::function_symbol_SortCons();
  atermpp::function_symbol core::detail::function_symbols::DataEqnSpec = core::detail::function_symbol_DataEqnSpec();
  atermpp::function_symbol core::detail::function_symbols::LinearProcessSummand = core::detail::function_symbol_LinearProcessSummand();
  atermpp::function_symbol core::detail::function_symbols::SortSpec = core::detail::function_symbol_SortSpec();
  atermpp::function_symbol core::detail::function_symbols::ActionRenameRules = core::detail::function_symbol_ActionRenameRules();
  atermpp::function_symbol core::detail::function_symbols::UntypedParamId = core::detail::function_symbol_UntypedParamId();
  atermpp::function_symbol core::detail::function_symbols::BooleanEquation = core::detail::function_symbol_BooleanEquation();
  atermpp::function_symbol core::detail::function_symbols::ConsSpec = core::detail::function_symbol_ConsSpec();
  atermpp::function_symbol core::detail::function_symbols::SortList = core::detail::function_symbol_SortList();
  atermpp::function_symbol core::detail::function_symbols::Sum = core::detail::function_symbol_Sum();
  atermpp::function_symbol core::detail::function_symbols::DataVarId = core::detail::function_symbol_DataVarId();
  atermpp::function_symbol core::detail::function_symbols::ProcVarId = core::detail::function_symbol_ProcVarId();
  atermpp::function_symbol core::detail::function_symbols::ProcessInit = core::detail::function_symbol_ProcessInit();
  atermpp::function_symbol core::detail::function_symbols::UntypedIdentifier = core::detail::function_symbol_UntypedIdentifier();
  atermpp::function_symbol core::detail::function_symbols::BooleanFalse = core::detail::function_symbol_BooleanFalse();
  atermpp::function_symbol core::detail::function_symbols::BES = core::detail::function_symbol_BES();
  atermpp::function_symbol core::detail::function_symbols::MapSpec = core::detail::function_symbol_MapSpec();
  atermpp::function_symbol core::detail::function_symbols::IfThen = core::detail::function_symbol_IfThen();
  atermpp::function_symbol core::detail::function_symbols::BooleanAnd = core::detail::function_symbol_BooleanAnd();
  atermpp::function_symbol core::detail::function_symbols::LinProcSpec = core::detail::function_symbol_LinProcSpec();
  atermpp::function_symbol core::detail::function_symbols::Choice = core::detail::function_symbol_Choice();
  atermpp::function_symbol core::detail::function_symbols::LinearProcessInit = core::detail::function_symbol_LinearProcessInit();
  atermpp::function_symbol core::detail::function_symbols::MultAct = core::detail::function_symbol_MultAct();
  atermpp::function_symbol core::detail::function_symbols::PropVarInst = core::detail::function_symbol_PropVarInst();
  atermpp::function_symbol core::detail::function_symbols::BagComp = core::detail::function_symbol_BagComp();
  atermpp::function_symbol core::detail::function_symbols::StateDelay = core::detail::function_symbol_StateDelay();
  atermpp::function_symbol core::detail::function_symbols::RegAlt = core::detail::function_symbol_RegAlt();
  atermpp::function_symbol core::detail::function_symbols::UntypedMultAct = core::detail::function_symbol_UntypedMultAct();
  atermpp::function_symbol core::detail::function_symbols::StructCons = core::detail::function_symbol_StructCons();
  atermpp::function_symbol core::detail::function_symbols::Mu = core::detail::function_symbol_Mu();
  atermpp::function_symbol core::detail::function_symbols::PBEqnSpec = core::detail::function_symbol_PBEqnSpec();
  atermpp::function_symbol core::detail::function_symbols::ActNot = core::detail::function_symbol_ActNot();
  atermpp::function_symbol core::detail::function_symbols::BooleanTrue = core::detail::function_symbol_BooleanTrue();
  atermpp::function_symbol core::detail::function_symbols::Block = core::detail::function_symbol_Block();
  atermpp::function_symbol core::detail::function_symbols::Rename = core::detail::function_symbol_Rename();
  atermpp::function_symbol core::detail::function_symbols::Exists = core::detail::function_symbol_Exists();
  atermpp::function_symbol core::detail::function_symbols::Sync = core::detail::function_symbol_Sync();
  atermpp::function_symbol core::detail::function_symbols::ActExists = core::detail::function_symbol_ActExists();
  atermpp::function_symbol core::detail::function_symbols::ProcSpec = core::detail::function_symbol_ProcSpec();
  atermpp::function_symbol core::detail::function_symbols::UntypedSortsPossible = core::detail::function_symbol_UntypedSortsPossible();
  atermpp::function_symbol core::detail::function_symbols::StateMu = core::detail::function_symbol_StateMu();
  atermpp::function_symbol core::detail::function_symbols::StateFalse = core::detail::function_symbol_StateFalse();
  atermpp::function_symbol core::detail::function_symbols::PBESFalse = core::detail::function_symbol_PBESFalse();
  atermpp::function_symbol core::detail::function_symbols::PBESForall = core::detail::function_symbol_PBESForall();
  atermpp::function_symbol core::detail::function_symbols::StateTrue = core::detail::function_symbol_StateTrue();
  atermpp::function_symbol core::detail::function_symbols::BInit = core::detail::function_symbol_BInit();
  atermpp::function_symbol core::detail::function_symbols::UntypedSortUnknown = core::detail::function_symbol_UntypedSortUnknown();
  atermpp::function_symbol core::detail::function_symbols::RegTrans = core::detail::function_symbol_RegTrans();
  atermpp::function_symbol core::detail::function_symbols::StateDelayTimed = core::detail::function_symbol_StateDelayTimed();
  atermpp::function_symbol core::detail::function_symbols::Nu = core::detail::function_symbol_Nu();
  atermpp::function_symbol core::detail::function_symbols::SortStruct = core::detail::function_symbol_SortStruct();
  atermpp::function_symbol core::detail::function_symbols::AtTime = core::detail::function_symbol_AtTime();
  atermpp::function_symbol core::detail::function_symbols::ActOr = core::detail::function_symbol_ActOr();
  atermpp::function_symbol core::detail::function_symbols::Comm = core::detail::function_symbol_Comm();
  atermpp::function_symbol core::detail::function_symbols::BooleanNot = core::detail::function_symbol_BooleanNot();
  atermpp::function_symbol core::detail::function_symbols::Delta = core::detail::function_symbol_Delta();
  atermpp::function_symbol core::detail::function_symbols::ActMultAct = core::detail::function_symbol_ActMultAct();
  atermpp::function_symbol core::detail::function_symbols::StateAnd = core::detail::function_symbol_StateAnd();
  atermpp::function_symbol core::detail::function_symbols::LMerge = core::detail::function_symbol_LMerge();
  atermpp::function_symbol core::detail::function_symbols::SetComp = core::detail::function_symbol_SetComp();
  atermpp::function_symbol core::detail::function_symbols::ActForall = core::detail::function_symbol_ActForall();
  atermpp::function_symbol core::detail::function_symbols::RenameExpr = core::detail::function_symbol_RenameExpr();
  atermpp::function_symbol core::detail::function_symbols::Merge = core::detail::function_symbol_Merge();
  atermpp::function_symbol core::detail::function_symbols::ActSpec = core::detail::function_symbol_ActSpec();
  atermpp::function_symbol core::detail::function_symbols::BooleanVariable = core::detail::function_symbol_BooleanVariable();
  atermpp::function_symbol core::detail::function_symbols::Action = core::detail::function_symbol_Action();
  atermpp::function_symbol core::detail::function_symbols::PBESAnd = core::detail::function_symbol_PBESAnd();
  atermpp::function_symbol core::detail::function_symbols::Lambda = core::detail::function_symbol_Lambda();
  atermpp::function_symbol core::detail::function_symbols::StateMust = core::detail::function_symbol_StateMust();
  atermpp::function_symbol core::detail::function_symbols::Seq = core::detail::function_symbol_Seq();
  atermpp::function_symbol core::detail::function_symbols::DataVarIdInit = core::detail::function_symbol_DataVarIdInit();
  atermpp::function_symbol core::detail::function_symbols::Process = core::detail::function_symbol_Process();
  atermpp::function_symbol core::detail::function_symbols::ActAnd = core::detail::function_symbol_ActAnd();
  atermpp::function_symbol core::detail::function_symbols::ActionRenameSpec = core::detail::function_symbol_ActionRenameSpec();
  atermpp::function_symbol core::detail::function_symbols::PBES = core::detail::function_symbol_PBES();
  atermpp::function_symbol core::detail::function_symbols::StateVar = core::detail::function_symbol_StateVar();
  atermpp::function_symbol core::detail::function_symbols::ActionRenameRule = core::detail::function_symbol_ActionRenameRule();
  atermpp::function_symbol core::detail::function_symbols::RegSeq = core::detail::function_symbol_RegSeq();
  atermpp::function_symbol core::detail::function_symbols::LinearProcess = core::detail::function_symbol_LinearProcess();
  atermpp::function_symbol core::detail::function_symbols::ActAt = core::detail::function_symbol_ActAt();
  atermpp::function_symbol core::detail::function_symbols::DataEqn = core::detail::function_symbol_DataEqn();
  atermpp::function_symbol core::detail::function_symbols::PBESNot = core::detail::function_symbol_PBESNot();
  atermpp::function_symbol core::detail::function_symbols::StateExists = core::detail::function_symbol_StateExists();
  atermpp::function_symbol core::detail::function_symbols::StateMay = core::detail::function_symbol_StateMay();
  atermpp::function_symbol core::detail::function_symbols::PBESTrue = core::detail::function_symbol_PBESTrue();
  atermpp::function_symbol core::detail::function_symbols::MultActName = core::detail::function_symbol_MultActName();
  atermpp::function_symbol core::detail::function_symbols::IfThenElse = core::detail::function_symbol_IfThenElse();
  atermpp::function_symbol core::detail::function_symbols::Nil = core::detail::function_symbol_Nil();
  atermpp::function_symbol core::detail::function_symbols::ProcEqn = core::detail::function_symbol_ProcEqn();
  atermpp::function_symbol core::detail::function_symbols::StructProj = core::detail::function_symbol_StructProj();
  atermpp::function_symbol core::detail::function_symbols::PBEqn = core::detail::function_symbol_PBEqn();
  atermpp::function_symbol core::detail::function_symbols::Whr = core::detail::function_symbol_Whr();
  atermpp::function_symbol core::detail::function_symbols::OpId = core::detail::function_symbol_OpId();
  atermpp::function_symbol core::detail::function_symbols::SortSet = core::detail::function_symbol_SortSet();
  atermpp::function_symbol core::detail::function_symbols::ActFalse = core::detail::function_symbol_ActFalse();
  atermpp::function_symbol core::detail::function_symbols::ActId = core::detail::function_symbol_ActId();
  atermpp::function_symbol core::detail::function_symbols::StateYaled = core::detail::function_symbol_StateYaled();
  atermpp::function_symbol core::detail::function_symbols::PBESOr = core::detail::function_symbol_PBESOr();
  atermpp::function_symbol core::detail::function_symbols::UntypedProcessAssignment = core::detail::function_symbol_UntypedProcessAssignment();
  atermpp::function_symbol core::detail::function_symbols::SortFBag = core::detail::function_symbol_SortFBag();
  atermpp::function_symbol core::detail::function_symbols::Allow = core::detail::function_symbol_Allow();
  atermpp::function_symbol core::detail::function_symbols::PropVarDecl = core::detail::function_symbol_PropVarDecl();
  atermpp::function_symbol core::detail::function_symbols::ActImp = core::detail::function_symbol_ActImp();
  atermpp::function_symbol core::detail::function_symbols::SortBag = core::detail::function_symbol_SortBag();
  atermpp::function_symbol core::detail::function_symbols::PBInit = core::detail::function_symbol_PBInit();
  atermpp::function_symbol core::detail::function_symbols::ActTrue = core::detail::function_symbol_ActTrue();
  atermpp::function_symbol core::detail::function_symbols::RegTransOrNil = core::detail::function_symbol_RegTransOrNil();
  atermpp::function_symbol core::detail::function_symbols::GlobVarSpec = core::detail::function_symbol_GlobVarSpec();
  atermpp::function_symbol core::detail::function_symbols::UntypedIdentifierAssignment = core::detail::function_symbol_UntypedIdentifierAssignment();
//--- end generated function symbol definitions ---//

} // namespace core

} // namespace mcrl2
