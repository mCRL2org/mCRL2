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
#include "mcrl2/core/detail/default_values.h"

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

//--- start generated default value definitions ---//
atermpp::aterm_appl core::detail::default_values::BooleanOr = core::detail::default_value_BooleanOr();
  atermpp::aterm_appl core::detail::default_values::StateOr = core::detail::default_value_StateOr();
  atermpp::aterm_appl core::detail::default_values::Hide = core::detail::default_value_Hide();
  atermpp::aterm_appl core::detail::default_values::SortArrow = core::detail::default_value_SortArrow();
  atermpp::aterm_appl core::detail::default_values::ProcessAssignment = core::detail::default_value_ProcessAssignment();
  atermpp::aterm_appl core::detail::default_values::Forall = core::detail::default_value_Forall();
  atermpp::aterm_appl core::detail::default_values::CommExpr = core::detail::default_value_CommExpr();
  atermpp::aterm_appl core::detail::default_values::StateNot = core::detail::default_value_StateNot();
  atermpp::aterm_appl core::detail::default_values::UntypedSetBagComp = core::detail::default_value_UntypedSetBagComp();
  atermpp::aterm_appl core::detail::default_values::SortFSet = core::detail::default_value_SortFSet();
  atermpp::aterm_appl core::detail::default_values::StateImp = core::detail::default_value_StateImp();
  atermpp::aterm_appl core::detail::default_values::PBESExists = core::detail::default_value_PBESExists();
  atermpp::aterm_appl core::detail::default_values::PBESImp = core::detail::default_value_PBESImp();
  atermpp::aterm_appl core::detail::default_values::Binder = core::detail::default_value_Binder();
  atermpp::aterm_appl core::detail::default_values::SortRef = core::detail::default_value_SortRef();
  atermpp::aterm_appl core::detail::default_values::ProcEqnSpec = core::detail::default_value_ProcEqnSpec();
  atermpp::aterm_appl core::detail::default_values::StateForall = core::detail::default_value_StateForall();
  atermpp::aterm_appl core::detail::default_values::BooleanImp = core::detail::default_value_BooleanImp();
  atermpp::aterm_appl core::detail::default_values::SortId = core::detail::default_value_SortId();
  atermpp::aterm_appl core::detail::default_values::UntypedAction = core::detail::default_value_UntypedAction();
  atermpp::aterm_appl core::detail::default_values::StateNu = core::detail::default_value_StateNu();
  atermpp::aterm_appl core::detail::default_values::RegNil = core::detail::default_value_RegNil();
  atermpp::aterm_appl core::detail::default_values::DataSpec = core::detail::default_value_DataSpec();
  atermpp::aterm_appl core::detail::default_values::UntypedActMultAct = core::detail::default_value_UntypedActMultAct();
  atermpp::aterm_appl core::detail::default_values::Tau = core::detail::default_value_Tau();
  atermpp::aterm_appl core::detail::default_values::StateYaledTimed = core::detail::default_value_StateYaledTimed();
  atermpp::aterm_appl core::detail::default_values::SortCons = core::detail::default_value_SortCons();
  atermpp::aterm_appl core::detail::default_values::DataEqnSpec = core::detail::default_value_DataEqnSpec();
  atermpp::aterm_appl core::detail::default_values::LinearProcessSummand = core::detail::default_value_LinearProcessSummand();
  atermpp::aterm_appl core::detail::default_values::SortSpec = core::detail::default_value_SortSpec();
  atermpp::aterm_appl core::detail::default_values::ActionRenameRules = core::detail::default_value_ActionRenameRules();
  atermpp::aterm_appl core::detail::default_values::UntypedParamId = core::detail::default_value_UntypedParamId();
  atermpp::aterm_appl core::detail::default_values::BooleanEquation = core::detail::default_value_BooleanEquation();
  atermpp::aterm_appl core::detail::default_values::ConsSpec = core::detail::default_value_ConsSpec();
  atermpp::aterm_appl core::detail::default_values::SortList = core::detail::default_value_SortList();
  atermpp::aterm_appl core::detail::default_values::Sum = core::detail::default_value_Sum();
  atermpp::aterm_appl core::detail::default_values::DataVarId = core::detail::default_value_DataVarId();
  atermpp::aterm_appl core::detail::default_values::ProcVarId = core::detail::default_value_ProcVarId();
  atermpp::aterm_appl core::detail::default_values::ProcessInit = core::detail::default_value_ProcessInit();
  atermpp::aterm_appl core::detail::default_values::UntypedIdentifier = core::detail::default_value_UntypedIdentifier();
  atermpp::aterm_appl core::detail::default_values::BooleanFalse = core::detail::default_value_BooleanFalse();
  atermpp::aterm_appl core::detail::default_values::BES = core::detail::default_value_BES();
  atermpp::aterm_appl core::detail::default_values::MapSpec = core::detail::default_value_MapSpec();
  atermpp::aterm_appl core::detail::default_values::IfThen = core::detail::default_value_IfThen();
  atermpp::aterm_appl core::detail::default_values::BooleanAnd = core::detail::default_value_BooleanAnd();
  atermpp::aterm_appl core::detail::default_values::LinProcSpec = core::detail::default_value_LinProcSpec();
  atermpp::aterm_appl core::detail::default_values::Choice = core::detail::default_value_Choice();
  atermpp::aterm_appl core::detail::default_values::LinearProcessInit = core::detail::default_value_LinearProcessInit();
  atermpp::aterm_appl core::detail::default_values::MultAct = core::detail::default_value_MultAct();
  atermpp::aterm_appl core::detail::default_values::PropVarInst = core::detail::default_value_PropVarInst();
  atermpp::aterm_appl core::detail::default_values::BagComp = core::detail::default_value_BagComp();
  atermpp::aterm_appl core::detail::default_values::StateDelay = core::detail::default_value_StateDelay();
  atermpp::aterm_appl core::detail::default_values::RegAlt = core::detail::default_value_RegAlt();
  atermpp::aterm_appl core::detail::default_values::UntypedMultAct = core::detail::default_value_UntypedMultAct();
  atermpp::aterm_appl core::detail::default_values::StructCons = core::detail::default_value_StructCons();
  atermpp::aterm_appl core::detail::default_values::Mu = core::detail::default_value_Mu();
  atermpp::aterm_appl core::detail::default_values::PBEqnSpec = core::detail::default_value_PBEqnSpec();
  atermpp::aterm_appl core::detail::default_values::ActNot = core::detail::default_value_ActNot();
  atermpp::aterm_appl core::detail::default_values::BooleanTrue = core::detail::default_value_BooleanTrue();
  atermpp::aterm_appl core::detail::default_values::Block = core::detail::default_value_Block();
  atermpp::aterm_appl core::detail::default_values::Rename = core::detail::default_value_Rename();
  atermpp::aterm_appl core::detail::default_values::Exists = core::detail::default_value_Exists();
  atermpp::aterm_appl core::detail::default_values::Sync = core::detail::default_value_Sync();
  atermpp::aterm_appl core::detail::default_values::ActExists = core::detail::default_value_ActExists();
  atermpp::aterm_appl core::detail::default_values::ProcSpec = core::detail::default_value_ProcSpec();
  atermpp::aterm_appl core::detail::default_values::UntypedSortsPossible = core::detail::default_value_UntypedSortsPossible();
  atermpp::aterm_appl core::detail::default_values::StateMu = core::detail::default_value_StateMu();
  atermpp::aterm_appl core::detail::default_values::StateFalse = core::detail::default_value_StateFalse();
  atermpp::aterm_appl core::detail::default_values::PBESFalse = core::detail::default_value_PBESFalse();
  atermpp::aterm_appl core::detail::default_values::PBESForall = core::detail::default_value_PBESForall();
  atermpp::aterm_appl core::detail::default_values::StateTrue = core::detail::default_value_StateTrue();
  atermpp::aterm_appl core::detail::default_values::BInit = core::detail::default_value_BInit();
  atermpp::aterm_appl core::detail::default_values::UntypedSortUnknown = core::detail::default_value_UntypedSortUnknown();
  atermpp::aterm_appl core::detail::default_values::RegTrans = core::detail::default_value_RegTrans();
  atermpp::aterm_appl core::detail::default_values::StateDelayTimed = core::detail::default_value_StateDelayTimed();
  atermpp::aterm_appl core::detail::default_values::Nu = core::detail::default_value_Nu();
  atermpp::aterm_appl core::detail::default_values::SortStruct = core::detail::default_value_SortStruct();
  atermpp::aterm_appl core::detail::default_values::AtTime = core::detail::default_value_AtTime();
  atermpp::aterm_appl core::detail::default_values::ActOr = core::detail::default_value_ActOr();
  atermpp::aterm_appl core::detail::default_values::Comm = core::detail::default_value_Comm();
  atermpp::aterm_appl core::detail::default_values::BooleanNot = core::detail::default_value_BooleanNot();
  atermpp::aterm_appl core::detail::default_values::Delta = core::detail::default_value_Delta();
  atermpp::aterm_appl core::detail::default_values::ActMultAct = core::detail::default_value_ActMultAct();
  atermpp::aterm_appl core::detail::default_values::StateAnd = core::detail::default_value_StateAnd();
  atermpp::aterm_appl core::detail::default_values::LMerge = core::detail::default_value_LMerge();
  atermpp::aterm_appl core::detail::default_values::SetComp = core::detail::default_value_SetComp();
  atermpp::aterm_appl core::detail::default_values::ActForall = core::detail::default_value_ActForall();
  atermpp::aterm_appl core::detail::default_values::RenameExpr = core::detail::default_value_RenameExpr();
  atermpp::aterm_appl core::detail::default_values::Merge = core::detail::default_value_Merge();
  atermpp::aterm_appl core::detail::default_values::ActSpec = core::detail::default_value_ActSpec();
  atermpp::aterm_appl core::detail::default_values::BooleanVariable = core::detail::default_value_BooleanVariable();
  atermpp::aterm_appl core::detail::default_values::Action = core::detail::default_value_Action();
  atermpp::aterm_appl core::detail::default_values::PBESAnd = core::detail::default_value_PBESAnd();
  atermpp::aterm_appl core::detail::default_values::Lambda = core::detail::default_value_Lambda();
  atermpp::aterm_appl core::detail::default_values::StateMust = core::detail::default_value_StateMust();
  atermpp::aterm_appl core::detail::default_values::Seq = core::detail::default_value_Seq();
  atermpp::aterm_appl core::detail::default_values::DataVarIdInit = core::detail::default_value_DataVarIdInit();
  atermpp::aterm_appl core::detail::default_values::Process = core::detail::default_value_Process();
  atermpp::aterm_appl core::detail::default_values::ActAnd = core::detail::default_value_ActAnd();
  atermpp::aterm_appl core::detail::default_values::ActionRenameSpec = core::detail::default_value_ActionRenameSpec();
  atermpp::aterm_appl core::detail::default_values::PBES = core::detail::default_value_PBES();
  atermpp::aterm_appl core::detail::default_values::StateVar = core::detail::default_value_StateVar();
  atermpp::aterm_appl core::detail::default_values::ActionRenameRule = core::detail::default_value_ActionRenameRule();
  atermpp::aterm_appl core::detail::default_values::RegSeq = core::detail::default_value_RegSeq();
  atermpp::aterm_appl core::detail::default_values::LinearProcess = core::detail::default_value_LinearProcess();
  atermpp::aterm_appl core::detail::default_values::ActAt = core::detail::default_value_ActAt();
  atermpp::aterm_appl core::detail::default_values::DataEqn = core::detail::default_value_DataEqn();
  atermpp::aterm_appl core::detail::default_values::PBESNot = core::detail::default_value_PBESNot();
  atermpp::aterm_appl core::detail::default_values::StateExists = core::detail::default_value_StateExists();
  atermpp::aterm_appl core::detail::default_values::StateMay = core::detail::default_value_StateMay();
  atermpp::aterm_appl core::detail::default_values::PBESTrue = core::detail::default_value_PBESTrue();
  atermpp::aterm_appl core::detail::default_values::MultActName = core::detail::default_value_MultActName();
  atermpp::aterm_appl core::detail::default_values::IfThenElse = core::detail::default_value_IfThenElse();
  atermpp::aterm_appl core::detail::default_values::Nil = core::detail::default_value_Nil();
  atermpp::aterm_appl core::detail::default_values::ProcEqn = core::detail::default_value_ProcEqn();
  atermpp::aterm_appl core::detail::default_values::StructProj = core::detail::default_value_StructProj();
  atermpp::aterm_appl core::detail::default_values::PBEqn = core::detail::default_value_PBEqn();
  atermpp::aterm_appl core::detail::default_values::Whr = core::detail::default_value_Whr();
  atermpp::aterm_appl core::detail::default_values::OpId = core::detail::default_value_OpId();
  atermpp::aterm_appl core::detail::default_values::SortSet = core::detail::default_value_SortSet();
  atermpp::aterm_appl core::detail::default_values::ActFalse = core::detail::default_value_ActFalse();
  atermpp::aterm_appl core::detail::default_values::ActId = core::detail::default_value_ActId();
  atermpp::aterm_appl core::detail::default_values::StateYaled = core::detail::default_value_StateYaled();
  atermpp::aterm_appl core::detail::default_values::PBESOr = core::detail::default_value_PBESOr();
  atermpp::aterm_appl core::detail::default_values::UntypedProcessAssignment = core::detail::default_value_UntypedProcessAssignment();
  atermpp::aterm_appl core::detail::default_values::SortFBag = core::detail::default_value_SortFBag();
  atermpp::aterm_appl core::detail::default_values::Allow = core::detail::default_value_Allow();
  atermpp::aterm_appl core::detail::default_values::PropVarDecl = core::detail::default_value_PropVarDecl();
  atermpp::aterm_appl core::detail::default_values::ActImp = core::detail::default_value_ActImp();
  atermpp::aterm_appl core::detail::default_values::SortBag = core::detail::default_value_SortBag();
  atermpp::aterm_appl core::detail::default_values::PBInit = core::detail::default_value_PBInit();
  atermpp::aterm_appl core::detail::default_values::ActTrue = core::detail::default_value_ActTrue();
  atermpp::aterm_appl core::detail::default_values::RegTransOrNil = core::detail::default_value_RegTransOrNil();
  atermpp::aterm_appl core::detail::default_values::GlobVarSpec = core::detail::default_value_GlobVarSpec();
  atermpp::aterm_appl core::detail::default_values::UntypedIdentifierAssignment = core::detail::default_value_UntypedIdentifierAssignment();
//--- end generated default value definitions ---//

} // namespace core

} // namespace mcrl2
