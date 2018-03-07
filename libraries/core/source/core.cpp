// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file core.cpp

#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/core/detail/default_values.h"
#include "mcrl2/core/detail/function_symbols.h"
#include "mcrl2/core/print.h"

namespace mcrl2 {

namespace core {

namespace detail {

std::vector<atermpp::function_symbol*> function_symbols_DataAppl;

atermpp::function_symbol function_symbol_OpId_ = atermpp::function_symbol("OpId", 3);
atermpp::function_symbol function_symbol_DataVarId_ = atermpp::function_symbol("DataVarId", 3);
std::vector<atermpp::function_symbol> function_symbols_DataAppl_;

} // namespace detail

//--- start generated core overloads ---//
std::string pp(const core::identifier_string& x) { return core::pp< core::identifier_string >(x); }
//--- end generated core overloads ---//

//--- start generated function symbol definitions ---//
const atermpp::function_symbol core::detail::function_symbols::BooleanOr = core::detail::function_symbol_BooleanOr();
  const atermpp::function_symbol core::detail::function_symbols::StateOr = core::detail::function_symbol_StateOr();
  const atermpp::function_symbol core::detail::function_symbols::Hide = core::detail::function_symbol_Hide();
  const atermpp::function_symbol core::detail::function_symbols::SortArrow = core::detail::function_symbol_SortArrow();
  const atermpp::function_symbol core::detail::function_symbols::ProcessAssignment = core::detail::function_symbol_ProcessAssignment();
  const atermpp::function_symbol core::detail::function_symbols::Forall = core::detail::function_symbol_Forall();
  const atermpp::function_symbol core::detail::function_symbols::CommExpr = core::detail::function_symbol_CommExpr();
  const atermpp::function_symbol core::detail::function_symbols::StateNot = core::detail::function_symbol_StateNot();
  const atermpp::function_symbol core::detail::function_symbols::UntypedSetBagComp = core::detail::function_symbol_UntypedSetBagComp();
  const atermpp::function_symbol core::detail::function_symbols::SortFSet = core::detail::function_symbol_SortFSet();
  const atermpp::function_symbol core::detail::function_symbols::StateImp = core::detail::function_symbol_StateImp();
  const atermpp::function_symbol core::detail::function_symbols::PBESExists = core::detail::function_symbol_PBESExists();
  const atermpp::function_symbol core::detail::function_symbols::PBESImp = core::detail::function_symbol_PBESImp();
  const atermpp::function_symbol core::detail::function_symbols::Binder = core::detail::function_symbol_Binder();
  const atermpp::function_symbol core::detail::function_symbols::StochasticOperator = core::detail::function_symbol_StochasticOperator();
  const atermpp::function_symbol core::detail::function_symbols::SortRef = core::detail::function_symbol_SortRef();
  const atermpp::function_symbol core::detail::function_symbols::ProcEqnSpec = core::detail::function_symbol_ProcEqnSpec();
  const atermpp::function_symbol core::detail::function_symbols::StateForall = core::detail::function_symbol_StateForall();
  const atermpp::function_symbol core::detail::function_symbols::BooleanImp = core::detail::function_symbol_BooleanImp();
  const atermpp::function_symbol core::detail::function_symbols::SortId = core::detail::function_symbol_SortId();
  const atermpp::function_symbol core::detail::function_symbols::StateNu = core::detail::function_symbol_StateNu();
  const atermpp::function_symbol core::detail::function_symbols::RegNil = core::detail::function_symbol_RegNil();
  const atermpp::function_symbol core::detail::function_symbols::DataSpec = core::detail::function_symbol_DataSpec();
  const atermpp::function_symbol core::detail::function_symbols::Tau = core::detail::function_symbol_Tau();
  const atermpp::function_symbol core::detail::function_symbols::StateYaledTimed = core::detail::function_symbol_StateYaledTimed();
  const atermpp::function_symbol core::detail::function_symbols::SortCons = core::detail::function_symbol_SortCons();
  const atermpp::function_symbol core::detail::function_symbols::DataEqnSpec = core::detail::function_symbol_DataEqnSpec();
  const atermpp::function_symbol core::detail::function_symbols::LinearProcessSummand = core::detail::function_symbol_LinearProcessSummand();
  const atermpp::function_symbol core::detail::function_symbols::SortSpec = core::detail::function_symbol_SortSpec();
  const atermpp::function_symbol core::detail::function_symbols::ActionRenameRules = core::detail::function_symbol_ActionRenameRules();
  const atermpp::function_symbol core::detail::function_symbols::BddFalse = core::detail::function_symbol_BddFalse();
  const atermpp::function_symbol core::detail::function_symbols::BooleanEquation = core::detail::function_symbol_BooleanEquation();
  const atermpp::function_symbol core::detail::function_symbols::ConsSpec = core::detail::function_symbol_ConsSpec();
  const atermpp::function_symbol core::detail::function_symbols::SortList = core::detail::function_symbol_SortList();
  const atermpp::function_symbol core::detail::function_symbols::Sum = core::detail::function_symbol_Sum();
  const atermpp::function_symbol core::detail::function_symbols::DataVarId = core::detail::function_symbol_DataVarId();
  const atermpp::function_symbol core::detail::function_symbols::ProcVarId = core::detail::function_symbol_ProcVarId();
  const atermpp::function_symbol core::detail::function_symbols::ProcessInit = core::detail::function_symbol_ProcessInit();
  const atermpp::function_symbol core::detail::function_symbols::UntypedIdentifier = core::detail::function_symbol_UntypedIdentifier();
  const atermpp::function_symbol core::detail::function_symbols::BooleanFalse = core::detail::function_symbol_BooleanFalse();
  const atermpp::function_symbol core::detail::function_symbols::BES = core::detail::function_symbol_BES();
  const atermpp::function_symbol core::detail::function_symbols::MapSpec = core::detail::function_symbol_MapSpec();
  const atermpp::function_symbol core::detail::function_symbols::IfThen = core::detail::function_symbol_IfThen();
  const atermpp::function_symbol core::detail::function_symbols::BooleanAnd = core::detail::function_symbol_BooleanAnd();
  const atermpp::function_symbol core::detail::function_symbols::LinProcSpec = core::detail::function_symbol_LinProcSpec();
  const atermpp::function_symbol core::detail::function_symbols::Choice = core::detail::function_symbol_Choice();
  const atermpp::function_symbol core::detail::function_symbols::LinearProcessInit = core::detail::function_symbol_LinearProcessInit();
  const atermpp::function_symbol core::detail::function_symbols::MultAct = core::detail::function_symbol_MultAct();
  const atermpp::function_symbol core::detail::function_symbols::PropVarInst = core::detail::function_symbol_PropVarInst();
  const atermpp::function_symbol core::detail::function_symbols::BagComp = core::detail::function_symbol_BagComp();
  const atermpp::function_symbol core::detail::function_symbols::StateDelay = core::detail::function_symbol_StateDelay();
  const atermpp::function_symbol core::detail::function_symbols::RegAlt = core::detail::function_symbol_RegAlt();
  const atermpp::function_symbol core::detail::function_symbols::StructCons = core::detail::function_symbol_StructCons();
  const atermpp::function_symbol core::detail::function_symbols::Mu = core::detail::function_symbol_Mu();
  const atermpp::function_symbol core::detail::function_symbols::PBEqnSpec = core::detail::function_symbol_PBEqnSpec();
  const atermpp::function_symbol core::detail::function_symbols::UntypedRegFrm = core::detail::function_symbol_UntypedRegFrm();
  const atermpp::function_symbol core::detail::function_symbols::Distribution = core::detail::function_symbol_Distribution();
  const atermpp::function_symbol core::detail::function_symbols::BooleanTrue = core::detail::function_symbol_BooleanTrue();
  const atermpp::function_symbol core::detail::function_symbols::Block = core::detail::function_symbol_Block();
  const atermpp::function_symbol core::detail::function_symbols::Rename = core::detail::function_symbol_Rename();
  const atermpp::function_symbol core::detail::function_symbols::Exists = core::detail::function_symbol_Exists();
  const atermpp::function_symbol core::detail::function_symbols::Sync = core::detail::function_symbol_Sync();
  const atermpp::function_symbol core::detail::function_symbols::ActExists = core::detail::function_symbol_ActExists();
  const atermpp::function_symbol core::detail::function_symbols::ProcSpec = core::detail::function_symbol_ProcSpec();
  const atermpp::function_symbol core::detail::function_symbols::UntypedSortsPossible = core::detail::function_symbol_UntypedSortsPossible();
  const atermpp::function_symbol core::detail::function_symbols::StateMu = core::detail::function_symbol_StateMu();
  const atermpp::function_symbol core::detail::function_symbols::BddIf = core::detail::function_symbol_BddIf();
  const atermpp::function_symbol core::detail::function_symbols::StateFalse = core::detail::function_symbol_StateFalse();
  const atermpp::function_symbol core::detail::function_symbols::PBESFalse = core::detail::function_symbol_PBESFalse();
  const atermpp::function_symbol core::detail::function_symbols::PBESForall = core::detail::function_symbol_PBESForall();
  const atermpp::function_symbol core::detail::function_symbols::StateTrue = core::detail::function_symbol_StateTrue();
  const atermpp::function_symbol core::detail::function_symbols::BInit = core::detail::function_symbol_BInit();
  const atermpp::function_symbol core::detail::function_symbols::UntypedSortUnknown = core::detail::function_symbol_UntypedSortUnknown();
  const atermpp::function_symbol core::detail::function_symbols::RegTrans = core::detail::function_symbol_RegTrans();
  const atermpp::function_symbol core::detail::function_symbols::StateDelayTimed = core::detail::function_symbol_StateDelayTimed();
  const atermpp::function_symbol core::detail::function_symbols::Nu = core::detail::function_symbol_Nu();
  const atermpp::function_symbol core::detail::function_symbols::SortStruct = core::detail::function_symbol_SortStruct();
  const atermpp::function_symbol core::detail::function_symbols::AtTime = core::detail::function_symbol_AtTime();
  const atermpp::function_symbol core::detail::function_symbols::ActOr = core::detail::function_symbol_ActOr();
  const atermpp::function_symbol core::detail::function_symbols::Comm = core::detail::function_symbol_Comm();
  const atermpp::function_symbol core::detail::function_symbols::BooleanNot = core::detail::function_symbol_BooleanNot();
  const atermpp::function_symbol core::detail::function_symbols::Delta = core::detail::function_symbol_Delta();
  const atermpp::function_symbol core::detail::function_symbols::ActMultAct = core::detail::function_symbol_ActMultAct();
  const atermpp::function_symbol core::detail::function_symbols::StateAnd = core::detail::function_symbol_StateAnd();
  const atermpp::function_symbol core::detail::function_symbols::LMerge = core::detail::function_symbol_LMerge();
  const atermpp::function_symbol core::detail::function_symbols::SetComp = core::detail::function_symbol_SetComp();
  const atermpp::function_symbol core::detail::function_symbols::ActForall = core::detail::function_symbol_ActForall();
  const atermpp::function_symbol core::detail::function_symbols::RenameExpr = core::detail::function_symbol_RenameExpr();
  const atermpp::function_symbol core::detail::function_symbols::Merge = core::detail::function_symbol_Merge();
  const atermpp::function_symbol core::detail::function_symbols::ActSpec = core::detail::function_symbol_ActSpec();
  const atermpp::function_symbol core::detail::function_symbols::BooleanVariable = core::detail::function_symbol_BooleanVariable();
  const atermpp::function_symbol core::detail::function_symbols::Action = core::detail::function_symbol_Action();
  const atermpp::function_symbol core::detail::function_symbols::PBESAnd = core::detail::function_symbol_PBESAnd();
  const atermpp::function_symbol core::detail::function_symbols::Lambda = core::detail::function_symbol_Lambda();
  const atermpp::function_symbol core::detail::function_symbols::StateMust = core::detail::function_symbol_StateMust();
  const atermpp::function_symbol core::detail::function_symbols::Seq = core::detail::function_symbol_Seq();
  const atermpp::function_symbol core::detail::function_symbols::DataVarIdInit = core::detail::function_symbol_DataVarIdInit();
  const atermpp::function_symbol core::detail::function_symbols::Process = core::detail::function_symbol_Process();
  const atermpp::function_symbol core::detail::function_symbols::ActAnd = core::detail::function_symbol_ActAnd();
  const atermpp::function_symbol core::detail::function_symbols::ActionRenameSpec = core::detail::function_symbol_ActionRenameSpec();
  const atermpp::function_symbol core::detail::function_symbols::PBES = core::detail::function_symbol_PBES();
  const atermpp::function_symbol core::detail::function_symbols::StateVar = core::detail::function_symbol_StateVar();
  const atermpp::function_symbol core::detail::function_symbols::ActionRenameRule = core::detail::function_symbol_ActionRenameRule();
  const atermpp::function_symbol core::detail::function_symbols::RegSeq = core::detail::function_symbol_RegSeq();
  const atermpp::function_symbol core::detail::function_symbols::ActNot = core::detail::function_symbol_ActNot();
  const atermpp::function_symbol core::detail::function_symbols::LinearProcess = core::detail::function_symbol_LinearProcess();
  const atermpp::function_symbol core::detail::function_symbols::ActAt = core::detail::function_symbol_ActAt();
  const atermpp::function_symbol core::detail::function_symbols::DataEqn = core::detail::function_symbol_DataEqn();
  const atermpp::function_symbol core::detail::function_symbols::PBESNot = core::detail::function_symbol_PBESNot();
  const atermpp::function_symbol core::detail::function_symbols::StateExists = core::detail::function_symbol_StateExists();
  const atermpp::function_symbol core::detail::function_symbols::StateMay = core::detail::function_symbol_StateMay();
  const atermpp::function_symbol core::detail::function_symbols::PBESTrue = core::detail::function_symbol_PBESTrue();
  const atermpp::function_symbol core::detail::function_symbols::MultActName = core::detail::function_symbol_MultActName();
  const atermpp::function_symbol core::detail::function_symbols::IfThenElse = core::detail::function_symbol_IfThenElse();
  const atermpp::function_symbol core::detail::function_symbols::UntypedSortVariable = core::detail::function_symbol_UntypedSortVariable();
  const atermpp::function_symbol core::detail::function_symbols::ProcEqn = core::detail::function_symbol_ProcEqn();
  const atermpp::function_symbol core::detail::function_symbols::StructProj = core::detail::function_symbol_StructProj();
  const atermpp::function_symbol core::detail::function_symbols::PBEqn = core::detail::function_symbol_PBEqn();
  const atermpp::function_symbol core::detail::function_symbols::Whr = core::detail::function_symbol_Whr();
  const atermpp::function_symbol core::detail::function_symbols::OpId = core::detail::function_symbol_OpId();
  const atermpp::function_symbol core::detail::function_symbols::SortSet = core::detail::function_symbol_SortSet();
  const atermpp::function_symbol core::detail::function_symbols::ActFalse = core::detail::function_symbol_ActFalse();
  const atermpp::function_symbol core::detail::function_symbols::ActId = core::detail::function_symbol_ActId();
  const atermpp::function_symbol core::detail::function_symbols::StateYaled = core::detail::function_symbol_StateYaled();
  const atermpp::function_symbol core::detail::function_symbols::PBESOr = core::detail::function_symbol_PBESOr();
  const atermpp::function_symbol core::detail::function_symbols::BddTrue = core::detail::function_symbol_BddTrue();
  const atermpp::function_symbol core::detail::function_symbols::UntypedProcessAssignment = core::detail::function_symbol_UntypedProcessAssignment();
  const atermpp::function_symbol core::detail::function_symbols::SortFBag = core::detail::function_symbol_SortFBag();
  const atermpp::function_symbol core::detail::function_symbols::Allow = core::detail::function_symbol_Allow();
  const atermpp::function_symbol core::detail::function_symbols::PropVarDecl = core::detail::function_symbol_PropVarDecl();
  const atermpp::function_symbol core::detail::function_symbols::ActImp = core::detail::function_symbol_ActImp();
  const atermpp::function_symbol core::detail::function_symbols::SortBag = core::detail::function_symbol_SortBag();
  const atermpp::function_symbol core::detail::function_symbols::PBInit = core::detail::function_symbol_PBInit();
  const atermpp::function_symbol core::detail::function_symbols::ActTrue = core::detail::function_symbol_ActTrue();
  const atermpp::function_symbol core::detail::function_symbols::RegTransOrNil = core::detail::function_symbol_RegTransOrNil();
  const atermpp::function_symbol core::detail::function_symbols::UntypedMultiAction = core::detail::function_symbol_UntypedMultiAction();
  const atermpp::function_symbol core::detail::function_symbols::GlobVarSpec = core::detail::function_symbol_GlobVarSpec();
  const atermpp::function_symbol core::detail::function_symbols::UntypedIdentifierAssignment = core::detail::function_symbol_UntypedIdentifierAssignment();
  const atermpp::function_symbol core::detail::function_symbols::UntypedDataParameter = core::detail::function_symbol_UntypedDataParameter();
//--- end generated function symbol definitions ---//

//--- start generated default value definitions ---//
const atermpp::aterm_appl core::detail::default_values::BooleanOr = core::detail::default_value_BooleanOr();
  const atermpp::aterm_appl core::detail::default_values::StateOr = core::detail::default_value_StateOr();
  const atermpp::aterm_appl core::detail::default_values::Hide = core::detail::default_value_Hide();
  const atermpp::aterm_appl core::detail::default_values::SortArrow = core::detail::default_value_SortArrow();
  const atermpp::aterm_appl core::detail::default_values::ProcessAssignment = core::detail::default_value_ProcessAssignment();
  const atermpp::aterm_appl core::detail::default_values::Forall = core::detail::default_value_Forall();
  const atermpp::aterm_appl core::detail::default_values::CommExpr = core::detail::default_value_CommExpr();
  const atermpp::aterm_appl core::detail::default_values::StateNot = core::detail::default_value_StateNot();
  const atermpp::aterm_appl core::detail::default_values::UntypedSetBagComp = core::detail::default_value_UntypedSetBagComp();
  const atermpp::aterm_appl core::detail::default_values::SortFSet = core::detail::default_value_SortFSet();
  const atermpp::aterm_appl core::detail::default_values::StateImp = core::detail::default_value_StateImp();
  const atermpp::aterm_appl core::detail::default_values::PBESExists = core::detail::default_value_PBESExists();
  const atermpp::aterm_appl core::detail::default_values::PBESImp = core::detail::default_value_PBESImp();
  const atermpp::aterm_appl core::detail::default_values::Binder = core::detail::default_value_Binder();
  const atermpp::aterm_appl core::detail::default_values::StochasticOperator = core::detail::default_value_StochasticOperator();
  const atermpp::aterm_appl core::detail::default_values::SortRef = core::detail::default_value_SortRef();
  const atermpp::aterm_appl core::detail::default_values::ProcEqnSpec = core::detail::default_value_ProcEqnSpec();
  const atermpp::aterm_appl core::detail::default_values::StateForall = core::detail::default_value_StateForall();
  const atermpp::aterm_appl core::detail::default_values::BooleanImp = core::detail::default_value_BooleanImp();
  const atermpp::aterm_appl core::detail::default_values::SortId = core::detail::default_value_SortId();
  const atermpp::aterm_appl core::detail::default_values::StateNu = core::detail::default_value_StateNu();
  const atermpp::aterm_appl core::detail::default_values::RegNil = core::detail::default_value_RegNil();
  const atermpp::aterm_appl core::detail::default_values::DataSpec = core::detail::default_value_DataSpec();
  const atermpp::aterm_appl core::detail::default_values::Tau = core::detail::default_value_Tau();
  const atermpp::aterm_appl core::detail::default_values::StateYaledTimed = core::detail::default_value_StateYaledTimed();
  const atermpp::aterm_appl core::detail::default_values::SortCons = core::detail::default_value_SortCons();
  const atermpp::aterm_appl core::detail::default_values::DataEqnSpec = core::detail::default_value_DataEqnSpec();
  const atermpp::aterm_appl core::detail::default_values::LinearProcessSummand = core::detail::default_value_LinearProcessSummand();
  const atermpp::aterm_appl core::detail::default_values::SortSpec = core::detail::default_value_SortSpec();
  const atermpp::aterm_appl core::detail::default_values::ActionRenameRules = core::detail::default_value_ActionRenameRules();
  const atermpp::aterm_appl core::detail::default_values::BddFalse = core::detail::default_value_BddFalse();
  const atermpp::aterm_appl core::detail::default_values::BooleanEquation = core::detail::default_value_BooleanEquation();
  const atermpp::aterm_appl core::detail::default_values::ConsSpec = core::detail::default_value_ConsSpec();
  const atermpp::aterm_appl core::detail::default_values::SortList = core::detail::default_value_SortList();
  const atermpp::aterm_appl core::detail::default_values::Sum = core::detail::default_value_Sum();
  const atermpp::aterm_appl core::detail::default_values::DataVarId = core::detail::default_value_DataVarId();
  const atermpp::aterm_appl core::detail::default_values::ProcVarId = core::detail::default_value_ProcVarId();
  const atermpp::aterm_appl core::detail::default_values::ProcessInit = core::detail::default_value_ProcessInit();
  const atermpp::aterm_appl core::detail::default_values::UntypedIdentifier = core::detail::default_value_UntypedIdentifier();
  const atermpp::aterm_appl core::detail::default_values::BooleanFalse = core::detail::default_value_BooleanFalse();
  const atermpp::aterm_appl core::detail::default_values::BES = core::detail::default_value_BES();
  const atermpp::aterm_appl core::detail::default_values::MapSpec = core::detail::default_value_MapSpec();
  const atermpp::aterm_appl core::detail::default_values::IfThen = core::detail::default_value_IfThen();
  const atermpp::aterm_appl core::detail::default_values::BooleanAnd = core::detail::default_value_BooleanAnd();
  const atermpp::aterm_appl core::detail::default_values::LinProcSpec = core::detail::default_value_LinProcSpec();
  const atermpp::aterm_appl core::detail::default_values::Choice = core::detail::default_value_Choice();
  const atermpp::aterm_appl core::detail::default_values::LinearProcessInit = core::detail::default_value_LinearProcessInit();
  const atermpp::aterm_appl core::detail::default_values::MultAct = core::detail::default_value_MultAct();
  const atermpp::aterm_appl core::detail::default_values::PropVarInst = core::detail::default_value_PropVarInst();
  const atermpp::aterm_appl core::detail::default_values::BagComp = core::detail::default_value_BagComp();
  const atermpp::aterm_appl core::detail::default_values::StateDelay = core::detail::default_value_StateDelay();
  const atermpp::aterm_appl core::detail::default_values::RegAlt = core::detail::default_value_RegAlt();
  const atermpp::aterm_appl core::detail::default_values::StructCons = core::detail::default_value_StructCons();
  const atermpp::aterm_appl core::detail::default_values::Mu = core::detail::default_value_Mu();
  const atermpp::aterm_appl core::detail::default_values::PBEqnSpec = core::detail::default_value_PBEqnSpec();
  const atermpp::aterm_appl core::detail::default_values::UntypedRegFrm = core::detail::default_value_UntypedRegFrm();
  const atermpp::aterm_appl core::detail::default_values::Distribution = core::detail::default_value_Distribution();
  const atermpp::aterm_appl core::detail::default_values::BooleanTrue = core::detail::default_value_BooleanTrue();
  const atermpp::aterm_appl core::detail::default_values::Block = core::detail::default_value_Block();
  const atermpp::aterm_appl core::detail::default_values::Rename = core::detail::default_value_Rename();
  const atermpp::aterm_appl core::detail::default_values::Exists = core::detail::default_value_Exists();
  const atermpp::aterm_appl core::detail::default_values::Sync = core::detail::default_value_Sync();
  const atermpp::aterm_appl core::detail::default_values::ActExists = core::detail::default_value_ActExists();
  const atermpp::aterm_appl core::detail::default_values::ProcSpec = core::detail::default_value_ProcSpec();
  const atermpp::aterm_appl core::detail::default_values::UntypedSortsPossible = core::detail::default_value_UntypedSortsPossible();
  const atermpp::aterm_appl core::detail::default_values::StateMu = core::detail::default_value_StateMu();
  const atermpp::aterm_appl core::detail::default_values::BddIf = core::detail::default_value_BddIf();
  const atermpp::aterm_appl core::detail::default_values::StateFalse = core::detail::default_value_StateFalse();
  const atermpp::aterm_appl core::detail::default_values::PBESFalse = core::detail::default_value_PBESFalse();
  const atermpp::aterm_appl core::detail::default_values::PBESForall = core::detail::default_value_PBESForall();
  const atermpp::aterm_appl core::detail::default_values::StateTrue = core::detail::default_value_StateTrue();
  const atermpp::aterm_appl core::detail::default_values::BInit = core::detail::default_value_BInit();
  const atermpp::aterm_appl core::detail::default_values::UntypedSortUnknown = core::detail::default_value_UntypedSortUnknown();
  const atermpp::aterm_appl core::detail::default_values::RegTrans = core::detail::default_value_RegTrans();
  const atermpp::aterm_appl core::detail::default_values::StateDelayTimed = core::detail::default_value_StateDelayTimed();
  const atermpp::aterm_appl core::detail::default_values::Nu = core::detail::default_value_Nu();
  const atermpp::aterm_appl core::detail::default_values::SortStruct = core::detail::default_value_SortStruct();
  const atermpp::aterm_appl core::detail::default_values::AtTime = core::detail::default_value_AtTime();
  const atermpp::aterm_appl core::detail::default_values::ActOr = core::detail::default_value_ActOr();
  const atermpp::aterm_appl core::detail::default_values::Comm = core::detail::default_value_Comm();
  const atermpp::aterm_appl core::detail::default_values::BooleanNot = core::detail::default_value_BooleanNot();
  const atermpp::aterm_appl core::detail::default_values::Delta = core::detail::default_value_Delta();
  const atermpp::aterm_appl core::detail::default_values::ActMultAct = core::detail::default_value_ActMultAct();
  const atermpp::aterm_appl core::detail::default_values::StateAnd = core::detail::default_value_StateAnd();
  const atermpp::aterm_appl core::detail::default_values::LMerge = core::detail::default_value_LMerge();
  const atermpp::aterm_appl core::detail::default_values::SetComp = core::detail::default_value_SetComp();
  const atermpp::aterm_appl core::detail::default_values::ActForall = core::detail::default_value_ActForall();
  const atermpp::aterm_appl core::detail::default_values::RenameExpr = core::detail::default_value_RenameExpr();
  const atermpp::aterm_appl core::detail::default_values::Merge = core::detail::default_value_Merge();
  const atermpp::aterm_appl core::detail::default_values::ActSpec = core::detail::default_value_ActSpec();
  const atermpp::aterm_appl core::detail::default_values::BooleanVariable = core::detail::default_value_BooleanVariable();
  const atermpp::aterm_appl core::detail::default_values::Action = core::detail::default_value_Action();
  const atermpp::aterm_appl core::detail::default_values::PBESAnd = core::detail::default_value_PBESAnd();
  const atermpp::aterm_appl core::detail::default_values::Lambda = core::detail::default_value_Lambda();
  const atermpp::aterm_appl core::detail::default_values::StateMust = core::detail::default_value_StateMust();
  const atermpp::aterm_appl core::detail::default_values::Seq = core::detail::default_value_Seq();
  const atermpp::aterm_appl core::detail::default_values::DataVarIdInit = core::detail::default_value_DataVarIdInit();
  const atermpp::aterm_appl core::detail::default_values::Process = core::detail::default_value_Process();
  const atermpp::aterm_appl core::detail::default_values::ActAnd = core::detail::default_value_ActAnd();
  const atermpp::aterm_appl core::detail::default_values::ActionRenameSpec = core::detail::default_value_ActionRenameSpec();
  const atermpp::aterm_appl core::detail::default_values::PBES = core::detail::default_value_PBES();
  const atermpp::aterm_appl core::detail::default_values::StateVar = core::detail::default_value_StateVar();
  const atermpp::aterm_appl core::detail::default_values::ActionRenameRule = core::detail::default_value_ActionRenameRule();
  const atermpp::aterm_appl core::detail::default_values::RegSeq = core::detail::default_value_RegSeq();
  const atermpp::aterm_appl core::detail::default_values::ActNot = core::detail::default_value_ActNot();
  const atermpp::aterm_appl core::detail::default_values::LinearProcess = core::detail::default_value_LinearProcess();
  const atermpp::aterm_appl core::detail::default_values::ActAt = core::detail::default_value_ActAt();
  const atermpp::aterm_appl core::detail::default_values::DataEqn = core::detail::default_value_DataEqn();
  const atermpp::aterm_appl core::detail::default_values::PBESNot = core::detail::default_value_PBESNot();
  const atermpp::aterm_appl core::detail::default_values::StateExists = core::detail::default_value_StateExists();
  const atermpp::aterm_appl core::detail::default_values::StateMay = core::detail::default_value_StateMay();
  const atermpp::aterm_appl core::detail::default_values::PBESTrue = core::detail::default_value_PBESTrue();
  const atermpp::aterm_appl core::detail::default_values::MultActName = core::detail::default_value_MultActName();
  const atermpp::aterm_appl core::detail::default_values::IfThenElse = core::detail::default_value_IfThenElse();
  const atermpp::aterm_appl core::detail::default_values::UntypedSortVariable = core::detail::default_value_UntypedSortVariable();
  const atermpp::aterm_appl core::detail::default_values::ProcEqn = core::detail::default_value_ProcEqn();
  const atermpp::aterm_appl core::detail::default_values::StructProj = core::detail::default_value_StructProj();
  const atermpp::aterm_appl core::detail::default_values::PBEqn = core::detail::default_value_PBEqn();
  const atermpp::aterm_appl core::detail::default_values::Whr = core::detail::default_value_Whr();
  const atermpp::aterm_appl core::detail::default_values::OpId = core::detail::default_value_OpId();
  const atermpp::aterm_appl core::detail::default_values::SortSet = core::detail::default_value_SortSet();
  const atermpp::aterm_appl core::detail::default_values::ActFalse = core::detail::default_value_ActFalse();
  const atermpp::aterm_appl core::detail::default_values::ActId = core::detail::default_value_ActId();
  const atermpp::aterm_appl core::detail::default_values::StateYaled = core::detail::default_value_StateYaled();
  const atermpp::aterm_appl core::detail::default_values::PBESOr = core::detail::default_value_PBESOr();
  const atermpp::aterm_appl core::detail::default_values::BddTrue = core::detail::default_value_BddTrue();
  const atermpp::aterm_appl core::detail::default_values::UntypedProcessAssignment = core::detail::default_value_UntypedProcessAssignment();
  const atermpp::aterm_appl core::detail::default_values::SortFBag = core::detail::default_value_SortFBag();
  const atermpp::aterm_appl core::detail::default_values::Allow = core::detail::default_value_Allow();
  const atermpp::aterm_appl core::detail::default_values::PropVarDecl = core::detail::default_value_PropVarDecl();
  const atermpp::aterm_appl core::detail::default_values::ActImp = core::detail::default_value_ActImp();
  const atermpp::aterm_appl core::detail::default_values::SortBag = core::detail::default_value_SortBag();
  const atermpp::aterm_appl core::detail::default_values::PBInit = core::detail::default_value_PBInit();
  const atermpp::aterm_appl core::detail::default_values::ActTrue = core::detail::default_value_ActTrue();
  const atermpp::aterm_appl core::detail::default_values::RegTransOrNil = core::detail::default_value_RegTransOrNil();
  const atermpp::aterm_appl core::detail::default_values::UntypedMultiAction = core::detail::default_value_UntypedMultiAction();
  const atermpp::aterm_appl core::detail::default_values::GlobVarSpec = core::detail::default_value_GlobVarSpec();
  const atermpp::aterm_appl core::detail::default_values::UntypedIdentifierAssignment = core::detail::default_value_UntypedIdentifierAssignment();
  const atermpp::aterm_appl core::detail::default_values::UntypedDataParameter = core::detail::default_value_UntypedDataParameter();
  const atermpp::aterm_appl core::detail::default_values::SortExpr = core::detail::default_value_SortExpr();
  const atermpp::aterm_appl core::detail::default_values::SortConsType = core::detail::default_value_SortConsType();
  const atermpp::aterm_appl core::detail::default_values::DataExpr = core::detail::default_value_DataExpr();
  const atermpp::aterm_appl core::detail::default_values::BindingOperator = core::detail::default_value_BindingOperator();
  const atermpp::aterm_appl core::detail::default_values::WhrDecl = core::detail::default_value_WhrDecl();
  const atermpp::aterm_appl core::detail::default_values::SortDecl = core::detail::default_value_SortDecl();
  const atermpp::aterm_appl core::detail::default_values::ProcExpr = core::detail::default_value_ProcExpr();
  const atermpp::aterm_appl core::detail::default_values::MultActOrDelta = core::detail::default_value_MultActOrDelta();
  const atermpp::aterm_appl core::detail::default_values::ProcInit = core::detail::default_value_ProcInit();
  const atermpp::aterm_appl core::detail::default_values::StateFrm = core::detail::default_value_StateFrm();
  const atermpp::aterm_appl core::detail::default_values::RegFrm = core::detail::default_value_RegFrm();
  const atermpp::aterm_appl core::detail::default_values::ActFrm = core::detail::default_value_ActFrm();
  const atermpp::aterm_appl core::detail::default_values::ParamIdOrAction = core::detail::default_value_ParamIdOrAction();
  const atermpp::aterm_appl core::detail::default_values::ActionRenameRuleRHS = core::detail::default_value_ActionRenameRuleRHS();
  const atermpp::aterm_appl core::detail::default_values::FixPoint = core::detail::default_value_FixPoint();
  const atermpp::aterm_appl core::detail::default_values::PBExpr = core::detail::default_value_PBExpr();
  const atermpp::aterm_appl core::detail::default_values::BooleanExpression = core::detail::default_value_BooleanExpression();
  const atermpp::aterm_appl core::detail::default_values::BddExpression = core::detail::default_value_BddExpression();
//--- end generated default value definitions ---//

} // namespace core

} // namespace mcrl2
