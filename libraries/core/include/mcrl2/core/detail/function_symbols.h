// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/detail/function_symbols.h
/// \brief add your file description here.

#ifndef MCRL2_CORE_DETAIL_FUNCTION_SYMBOLS_H
#define MCRL2_CORE_DETAIL_FUNCTION_SYMBOLS_H

#include "mcrl2/atermpp/function_symbol.h"

namespace mcrl2 {

namespace core {

namespace detail {

struct function_symbols
{
//--- start generated function symbol declarations ---//
static atermpp::function_symbol BooleanOr;
  static atermpp::function_symbol StateOr;
  static atermpp::function_symbol Hide;
  static atermpp::function_symbol SortArrow;
  static atermpp::function_symbol ProcessAssignment;
  static atermpp::function_symbol Forall;
  static atermpp::function_symbol CommExpr;
  static atermpp::function_symbol StateNot;
  static atermpp::function_symbol UntypedSetBagComp;
  static atermpp::function_symbol SortFSet;
  static atermpp::function_symbol StateImp;
  static atermpp::function_symbol PBESExists;
  static atermpp::function_symbol PBESImp;
  static atermpp::function_symbol Binder;
  static atermpp::function_symbol SortRef;
  static atermpp::function_symbol ProcEqnSpec;
  static atermpp::function_symbol StateForall;
  static atermpp::function_symbol BooleanImp;
  static atermpp::function_symbol SortId;
  static atermpp::function_symbol UntypedAction;
  static atermpp::function_symbol StateNu;
  static atermpp::function_symbol RegNil;
  static atermpp::function_symbol DataSpec;
  static atermpp::function_symbol UntypedActMultAct;
  static atermpp::function_symbol Tau;
  static atermpp::function_symbol StateYaledTimed;
  static atermpp::function_symbol SortCons;
  static atermpp::function_symbol DataEqnSpec;
  static atermpp::function_symbol LinearProcessSummand;
  static atermpp::function_symbol SortSpec;
  static atermpp::function_symbol ActionRenameRules;
  static atermpp::function_symbol UntypedParamId;
  static atermpp::function_symbol BooleanEquation;
  static atermpp::function_symbol ConsSpec;
  static atermpp::function_symbol SortList;
  static atermpp::function_symbol Sum;
  static atermpp::function_symbol DataVarId;
  static atermpp::function_symbol ProcVarId;
  static atermpp::function_symbol ProcessInit;
  static atermpp::function_symbol UntypedIdentifier;
  static atermpp::function_symbol BooleanFalse;
  static atermpp::function_symbol BES;
  static atermpp::function_symbol MapSpec;
  static atermpp::function_symbol IfThen;
  static atermpp::function_symbol BooleanAnd;
  static atermpp::function_symbol LinProcSpec;
  static atermpp::function_symbol Choice;
  static atermpp::function_symbol LinearProcessInit;
  static atermpp::function_symbol MultAct;
  static atermpp::function_symbol PropVarInst;
  static atermpp::function_symbol BagComp;
  static atermpp::function_symbol StateDelay;
  static atermpp::function_symbol RegAlt;
  static atermpp::function_symbol UntypedMultAct;
  static atermpp::function_symbol StructCons;
  static atermpp::function_symbol Mu;
  static atermpp::function_symbol PBEqnSpec;
  static atermpp::function_symbol ActNot;
  static atermpp::function_symbol BooleanTrue;
  static atermpp::function_symbol Block;
  static atermpp::function_symbol Rename;
  static atermpp::function_symbol Exists;
  static atermpp::function_symbol Sync;
  static atermpp::function_symbol ActExists;
  static atermpp::function_symbol ProcSpec;
  static atermpp::function_symbol UntypedSortsPossible;
  static atermpp::function_symbol StateMu;
  static atermpp::function_symbol StateFalse;
  static atermpp::function_symbol PBESFalse;
  static atermpp::function_symbol PBESForall;
  static atermpp::function_symbol StateTrue;
  static atermpp::function_symbol BInit;
  static atermpp::function_symbol UntypedSortUnknown;
  static atermpp::function_symbol RegTrans;
  static atermpp::function_symbol StateDelayTimed;
  static atermpp::function_symbol Nu;
  static atermpp::function_symbol SortStruct;
  static atermpp::function_symbol AtTime;
  static atermpp::function_symbol ActOr;
  static atermpp::function_symbol Comm;
  static atermpp::function_symbol BooleanNot;
  static atermpp::function_symbol Delta;
  static atermpp::function_symbol ActMultAct;
  static atermpp::function_symbol StateAnd;
  static atermpp::function_symbol LMerge;
  static atermpp::function_symbol SetComp;
  static atermpp::function_symbol ActForall;
  static atermpp::function_symbol RenameExpr;
  static atermpp::function_symbol Merge;
  static atermpp::function_symbol ActSpec;
  static atermpp::function_symbol BooleanVariable;
  static atermpp::function_symbol Action;
  static atermpp::function_symbol PBESAnd;
  static atermpp::function_symbol Lambda;
  static atermpp::function_symbol StateMust;
  static atermpp::function_symbol Seq;
  static atermpp::function_symbol DataVarIdInit;
  static atermpp::function_symbol Process;
  static atermpp::function_symbol ActAnd;
  static atermpp::function_symbol ActionRenameSpec;
  static atermpp::function_symbol PBES;
  static atermpp::function_symbol StateVar;
  static atermpp::function_symbol ActionRenameRule;
  static atermpp::function_symbol RegSeq;
  static atermpp::function_symbol LinearProcess;
  static atermpp::function_symbol ActAt;
  static atermpp::function_symbol DataEqn;
  static atermpp::function_symbol PBESNot;
  static atermpp::function_symbol StateExists;
  static atermpp::function_symbol StateMay;
  static atermpp::function_symbol PBESTrue;
  static atermpp::function_symbol MultActName;
  static atermpp::function_symbol IfThenElse;
  static atermpp::function_symbol Nil;
  static atermpp::function_symbol ProcEqn;
  static atermpp::function_symbol StructProj;
  static atermpp::function_symbol PBEqn;
  static atermpp::function_symbol Whr;
  static atermpp::function_symbol OpId;
  static atermpp::function_symbol SortSet;
  static atermpp::function_symbol ActFalse;
  static atermpp::function_symbol ActId;
  static atermpp::function_symbol StateYaled;
  static atermpp::function_symbol PBESOr;
  static atermpp::function_symbol UntypedProcessAssignment;
  static atermpp::function_symbol SortFBag;
  static atermpp::function_symbol Allow;
  static atermpp::function_symbol PropVarDecl;
  static atermpp::function_symbol ActImp;
  static atermpp::function_symbol SortBag;
  static atermpp::function_symbol PBInit;
  static atermpp::function_symbol ActTrue;
  static atermpp::function_symbol RegTransOrNil;
  static atermpp::function_symbol GlobVarSpec;
  static atermpp::function_symbol UntypedIdentifierAssignment;
//--- end generated function symbol declarations ---//
};

} // namespace detail

} // namespace core

} // namespace mcrl2

#endif // MCRL2_CORE_DETAIL_FUNCTION_SYMBOLS_H

