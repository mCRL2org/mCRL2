// Author(s): Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file struct.cpp

#include <cstring>
#include <cstdlib>
#include <cassert>
#include <climits>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

#include "mcrl2/core/detail/mcrl2lexer.h"
#include "mcrl2/core/detail/struct.h"

#include "mcrl2/core/messaging.h"
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/core/numeric_string.h"

#include "workarounds.h" // DECL_A

using namespace mcrl2::utilities;

namespace mcrl2 {
  namespace core {
    namespace detail {

//Local declarations
//------------------

static bool IsPNSort(ATermAppl SortExpr);
//Ret: SortExpr is a sort expression for Pos or Nat

static bool IsPNISort(ATermAppl SortExpr);
//Ret: SortExpr is a sort expression for Pos, Nat or Int

static bool IsNISort(ATermAppl SortExpr);
//Ret: SortExpr is a sort expression for Nat or Int

static bool IsPNIRSort(ATermAppl SortExpr);
//Ret: SortExpr is a sort expression for Pos, Nat, Int or Real

static ATermAppl IntersectPNSorts(ATermAppl SortExpr1, ATermAppl SortExpr2);
//Pre: IsPNSort(SortExpr1) and IsPNSort(SortExpr2)
//Ret: the sort resulting from intersecting SortExpr1 and SortExpr2

static ATermAppl IntersectPNISorts(ATermAppl SortExpr1, ATermAppl SortExpr2);
//Pre: IsPNISort(SortExpr1) and IsPNISort(SortExpr2)
//Ret: the sort resulting from intersecting SortExpr1 and SortExpr2

static ATermAppl IntersectPNIRSorts(ATermAppl SortExpr1, ATermAppl SortExpr2);
//Pre: IsPNIRSort(SortExpr1) and IsPNIRSort(SortExpr2)
//Ret: the sort resulting from intersecting SortExpr1 and SortExpr2

static ATermAppl UnitePNSorts(ATermAppl SortExpr1, ATermAppl SortExpr2);
//Pre: IsPNSort(SortExpr1) and IsPNSort(SortExpr2)
//Ret: the sort resulting from uniting SortExpr1 and SortExpr2

static ATermAppl UnitePNISorts(ATermAppl SortExpr1, ATermAppl SortExpr2);
//Pre: IsPNISort(SortExpr1) and IsPNISort(SortExpr2)
//Ret: the sort resulting from uniting SortExpr1 and SortExpr2

static ATermAppl UnitePNIRSorts(ATermAppl SortExpr1, ATermAppl SortExpr2);
//Pre: IsPNIRSort(SortExpr1) and IsPNIRSort(SortExpr2)
//Ret: the sort resulting from uniting SortExpr1 and SortExpr2

//Enabling constructor functions
//------------------------------

//create sort system identifier names
inline ATermAppl initMakeSortIdNameBool(ATermAppl &t)
{
  t = gsString2ATermAppl("Bool");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeSortIdNamePos(ATermAppl &t)
{
  t = gsString2ATermAppl("Pos");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeSortIdNameNat(ATermAppl &t)
{
  t = gsString2ATermAppl("Nat");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeSortIdNameNatPair(ATermAppl &t)
{
  t = gsString2ATermAppl("@NatPair");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeSortIdNameInt(ATermAppl &t)
{
  t = gsString2ATermAppl("Int");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeSortIdNameReal(ATermAppl &t)
{
  t = gsString2ATermAppl("Real");
  ATprotectAppl(&t);
  return t;
}

//create operation system identifier names
inline ATermAppl initMakeOpIdNameTrue(ATermAppl &t)
{
  t = gsString2ATermAppl("true");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameFalse(ATermAppl &t)
{
  t = gsString2ATermAppl("false");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameNot(ATermAppl &t)
{
  t = gsString2ATermAppl("!");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameAnd(ATermAppl &t)
{
  t = gsString2ATermAppl("&&");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameOr(ATermAppl &t)
{
  t = gsString2ATermAppl("||");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameImp(ATermAppl &t)
{
  t = gsString2ATermAppl("=>");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameEq(ATermAppl &t)
{
  t = gsString2ATermAppl("==");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameNeq(ATermAppl &t)
{
  t = gsString2ATermAppl("!=");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameIf(ATermAppl &t)
{
  t = gsString2ATermAppl("if");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameForall(ATermAppl &t)
{
  t = gsString2ATermAppl("forall");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameExists(ATermAppl &t)
{
  t = gsString2ATermAppl("exists");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameC1(ATermAppl &t)
{
  t = gsString2ATermAppl("@c1");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameCDub(ATermAppl &t)
{
  t = gsString2ATermAppl("@cDub");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameC0(ATermAppl &t)
{
  t = gsString2ATermAppl("@c0");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameCNat(ATermAppl &t)
{
  t = gsString2ATermAppl("@cNat");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameCPair(ATermAppl &t)
{
  t = gsString2ATermAppl("@cPair");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameCNeg(ATermAppl &t)
{
  t = gsString2ATermAppl("@cNeg");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameCInt(ATermAppl &t)
{
  t = gsString2ATermAppl("@cInt");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameCReal(ATermAppl &t)
{
  t = gsString2ATermAppl("@cReal");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNamePos2Nat(ATermAppl &t)
{
  t = gsString2ATermAppl("Pos2Nat");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNamePos2Int(ATermAppl &t)
{
  t = gsString2ATermAppl("Pos2Int");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNamePos2Real(ATermAppl &t)
{
  t = gsString2ATermAppl("Pos2Real");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameNat2Pos(ATermAppl &t)
{
  t = gsString2ATermAppl("Nat2Pos");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameNat2Int(ATermAppl &t)
{
  t = gsString2ATermAppl("Nat2Int");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameNat2Real(ATermAppl &t)
{
  t = gsString2ATermAppl("Nat2Real");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameInt2Pos(ATermAppl &t)
{
  t = gsString2ATermAppl("Int2Pos");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameInt2Nat(ATermAppl &t)
{
  t = gsString2ATermAppl("Int2Nat");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameInt2Real(ATermAppl &t)
{
  t = gsString2ATermAppl("Int2Real");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameReal2Pos(ATermAppl &t)
{
  t = gsString2ATermAppl("Real2Pos");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameReal2Nat(ATermAppl &t)
{
  t = gsString2ATermAppl("Real2Nat");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameReal2Int(ATermAppl &t)
{
  t = gsString2ATermAppl("Real2Int");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameLTE(ATermAppl &t)
{
  t = gsString2ATermAppl("<=");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameLT(ATermAppl &t)
{
  t = gsString2ATermAppl("<");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameGTE(ATermAppl &t)
{
  t = gsString2ATermAppl(">=");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameGT(ATermAppl &t)
{
  t = gsString2ATermAppl(">");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameMax(ATermAppl &t)
{
  t = gsString2ATermAppl("max");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameMin(ATermAppl &t)
{
  t = gsString2ATermAppl("min");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameAbs(ATermAppl &t)
{
  t = gsString2ATermAppl("abs");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameNeg(ATermAppl &t)
{
  t = gsString2ATermAppl("-");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameSucc(ATermAppl &t)
{
  t = gsString2ATermAppl("succ");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNamePred(ATermAppl &t)
{
  t = gsString2ATermAppl("pred");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameDub(ATermAppl &t)
{
  t = gsString2ATermAppl("@dub");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameAdd(ATermAppl &t)
{
  t = gsString2ATermAppl("+");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameAddC(ATermAppl &t)
{
  t = gsString2ATermAppl("@addc");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameSubt(ATermAppl &t)
{
  t = gsString2ATermAppl("-");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameGTESubt(ATermAppl &t)
{
  t = gsString2ATermAppl("@gtesubt");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameGTESubtB(ATermAppl &t)
{
  t = gsString2ATermAppl("@gtesubtb");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameMult(ATermAppl &t)
{
  t = gsString2ATermAppl("*");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameMultIR(ATermAppl &t)
{
  t = gsString2ATermAppl("@multir");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameDiv(ATermAppl &t)
{
  t = gsString2ATermAppl("div");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameMod(ATermAppl &t)
{
  t = gsString2ATermAppl("mod");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameDivMod(ATermAppl &t)
{
  t = gsString2ATermAppl("@divmod");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameGDivMod(ATermAppl &t)
{
  t = gsString2ATermAppl("@gdivmod");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameGGDivMod(ATermAppl &t)
{
  t = gsString2ATermAppl("@ggdivmod");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameFirst(ATermAppl &t)
{
  t = gsString2ATermAppl("@first");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameLast(ATermAppl &t)
{
  t = gsString2ATermAppl("@last");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameExp(ATermAppl &t)
{
  t = gsString2ATermAppl("exp");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameEven(ATermAppl &t)
{
  t = gsString2ATermAppl("@even");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameDivide(ATermAppl &t) {
  t = gsString2ATermAppl("/");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameFloor(ATermAppl &t) {
  t = gsString2ATermAppl("floor");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameCeil(ATermAppl &t) {
  t = gsString2ATermAppl("ceil");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameRound(ATermAppl &t) {
  t = gsString2ATermAppl("round");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameRedFrac(ATermAppl &t) {
  t = gsString2ATermAppl("@redfrac");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameRedFracWhr(ATermAppl &t) {
  t = gsString2ATermAppl("@redfracwhr");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameRedFracHlp(ATermAppl &t) {
  t = gsString2ATermAppl("@redfrachlp");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameEmptyList(ATermAppl &t)
{
  t = gsString2ATermAppl("[]");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameListEnum(ATermAppl &t)
{
  t = gsString2ATermAppl("@ListEnum");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameListSize(ATermAppl &t)
{
  t = gsString2ATermAppl("#");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameCons(ATermAppl &t)
{
  t = gsString2ATermAppl("|>");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameSnoc(ATermAppl &t)
{
  t = gsString2ATermAppl("<|");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameConcat(ATermAppl &t)
{
  t = gsString2ATermAppl("++");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameEltAt(ATermAppl &t)
{
  t = gsString2ATermAppl(".");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameHead(ATermAppl &t)
{
  t = gsString2ATermAppl("head");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameTail(ATermAppl &t)
{
  t = gsString2ATermAppl("tail");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameRHead(ATermAppl &t)
{
  t = gsString2ATermAppl("rhead");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameRTail(ATermAppl &t)
{
  t = gsString2ATermAppl("rtail");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameEltIn(ATermAppl &t)
{
  t = gsString2ATermAppl("in");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameSet(ATermAppl &t)
{
  t = gsString2ATermAppl("@set");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameSetFSet(ATermAppl &t)
{
  t = gsString2ATermAppl("@setfset");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameSetComp(ATermAppl &t)
{
  t = gsString2ATermAppl("@setcomp");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameEmptySet(ATermAppl &t)
{
  t = gsString2ATermAppl("{}");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameSetEnum(ATermAppl &t)
{
  t = gsString2ATermAppl("@SetEnum");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameSubSetEq(ATermAppl &t)
{
  t = gsString2ATermAppl("<=");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameSubSet(ATermAppl &t)
{
  t = gsString2ATermAppl("<");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameSetUnion(ATermAppl &t)
{
  t = gsString2ATermAppl("+");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameSetDiff(ATermAppl &t)
{
  t = gsString2ATermAppl("-");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameSetIntersect(ATermAppl &t)
{
  t = gsString2ATermAppl("*");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameSetCompl(ATermAppl &t)
{
  t = gsString2ATermAppl("!");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameBag(ATermAppl &t)
{
  t = gsString2ATermAppl("@bag");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameBagFBag(ATermAppl &t)
{
  t = gsString2ATermAppl("@bagfbag");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameBagComp(ATermAppl &t)
{
  t = gsString2ATermAppl("@bagcomp");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameEmptyBag(ATermAppl &t)
{
  t = gsString2ATermAppl("{}");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameBagEnum(ATermAppl &t)
{
  t = gsString2ATermAppl("@BagEnum");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameCount(ATermAppl &t)
{
  t = gsString2ATermAppl("count");
  ATprotectAppl(&t);
  return t;
}
inline ATermAppl initMakeOpIdNameSubBagEq(ATermAppl &t)
{
  t = gsString2ATermAppl("<=");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameSubBag(ATermAppl &t)
{
  t = gsString2ATermAppl("<");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameBagJoin(ATermAppl &t)
{
  t = gsString2ATermAppl("+");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameBagDiff(ATermAppl &t)
{
  t = gsString2ATermAppl("-");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameBagIntersect(ATermAppl &t)
{
  t = gsString2ATermAppl("*");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameBag2Set(ATermAppl &t)
{
  t = gsString2ATermAppl("Bag2Set");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameSet2Bag(ATermAppl &t)
{
  t = gsString2ATermAppl("Set2Bag");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameFSetEmpty(ATermAppl &t)
{
  t = gsString2ATermAppl("@fset_empty");
  ATprotectAppl(&t);
  return t;
}

//inline ATermAppl initMakeOpIdNameFSetCons(ATermAppl &t)
//{
//  t = gsString2ATermAppl("@fset_cons");
//  ATprotectAppl(&t);
//  return t;
//}

inline ATermAppl initMakeOpIdNameFSetInsert(ATermAppl &t)
{
  t = gsString2ATermAppl("@fset_insert");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameFSetCInsert(ATermAppl &t)
{
  t = gsString2ATermAppl("@fset_cinsert");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameFSetIn(ATermAppl &t)
{
  t = gsString2ATermAppl("@fset_in");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameFSetLTE(ATermAppl &t)
{
  t = gsString2ATermAppl("@fset_lte");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameFSetUnion(ATermAppl &t)
{
  t = gsString2ATermAppl("@fset_union");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameFSetInter(ATermAppl &t)
{
  t = gsString2ATermAppl("@fset_inter");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameFBagEmpty(ATermAppl &t)
{
  t = gsString2ATermAppl("@fbag_empty");
  ATprotectAppl(&t);
  return t;
}

//inline ATermAppl initMakeOpIdNameFBagCons(ATermAppl &t)
//{
//  t = gsString2ATermAppl("@fbag_cons");
//  ATprotectAppl(&t);
//  return t;
//}

inline ATermAppl initMakeOpIdNameFBagInsert(ATermAppl &t)
{
  t = gsString2ATermAppl("@fbag_insert");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameFBagCInsert(ATermAppl &t)
{
  t = gsString2ATermAppl("@fbag_cinsert");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameFBagCount(ATermAppl &t)
{
  t = gsString2ATermAppl("@fbag_count");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameFBagIn(ATermAppl &t)
{
  t = gsString2ATermAppl("@fbag_in");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameFBagLTE(ATermAppl &t)
{
  t = gsString2ATermAppl("@fbag_lte");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameFBagJoin(ATermAppl &t)
{
  t = gsString2ATermAppl("@fbag_join");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameFBagInter(ATermAppl &t)
{
  t = gsString2ATermAppl("@fbag_inter");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameFBagDiff(ATermAppl &t)
{
  t = gsString2ATermAppl("@fbag_diff");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameFBag2FSet(ATermAppl &t)
{
  t = gsString2ATermAppl("@fbag2fset");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameFSet2FBag(ATermAppl &t)
{
  t = gsString2ATermAppl("@fset2fbag");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameMonus(ATermAppl &t)
{
  t = gsString2ATermAppl("@monus");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameSwapZero(ATermAppl &t)
{
  t = gsString2ATermAppl("@swap_zero");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameSwapZeroAdd(ATermAppl &t)
{
  t = gsString2ATermAppl("@swap_zero_add");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameSwapZeroMin(ATermAppl &t)
{
  t = gsString2ATermAppl("@swap_zero_min");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameSwapZeroMonus(ATermAppl &t)
{
  t = gsString2ATermAppl("@swap_zero_monus");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameSwapZeroLTE(ATermAppl &t)
{
  t = gsString2ATermAppl("@swap_zero_lte");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameFalseFunc(ATermAppl &t)
{
  t = gsString2ATermAppl("@false_");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameTrueFunc(ATermAppl &t)
{
  t = gsString2ATermAppl("@true_");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameNotFunc(ATermAppl &t)
{
  t = gsString2ATermAppl("@not_");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameAndFunc(ATermAppl &t)
{
  t = gsString2ATermAppl("@and_");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameOrFunc(ATermAppl &t)
{
  t = gsString2ATermAppl("@or_");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameZeroFunc(ATermAppl &t)
{
  t = gsString2ATermAppl("@zero_");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameOneFunc(ATermAppl &t)
{
  t = gsString2ATermAppl("@one_");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameAddFunc(ATermAppl &t)
{
  t = gsString2ATermAppl("@add_");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameMinFunc(ATermAppl &t)
{
  t = gsString2ATermAppl("@min_");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameMonusFunc(ATermAppl &t)
{
  t = gsString2ATermAppl("@monus_");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameNat2BoolFunc(ATermAppl &t)
{
  t = gsString2ATermAppl("@Nat2Bool_");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameBool2NatFunc(ATermAppl &t)
{
  t = gsString2ATermAppl("@Bool2Nat_");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameBagElt(ATermAppl &t)
{
  t = gsString2ATermAppl("@bag_elt");
  ATprotectAppl(&t);
  return t;
}


//Sort expressions
//----------------

bool gsIsSortExpr(ATermAppl Term)
{
  return
    gsIsSortId(Term)        || gsIsSortCons(Term)     ||
    gsIsSortStruct(Term)    || gsIsSortArrow(Term) ||
    gsIsSortUnknown(Term)   || gsIsSortsPossible(Term);
}

bool gsIsNotInferred(ATermAppl Term)
{
  return gsIsSortUnknown(Term) || gsIsSortsPossible(Term);
}

//Creation of names for system sort identifiers

ATermAppl gsMakeSortIdNameBool() {
  static ATermAppl t = initMakeSortIdNameBool(t);
  return t;
}

ATermAppl gsMakeSortIdNamePos() {
  static ATermAppl t = initMakeSortIdNamePos(t);
  return t;
}

ATermAppl gsMakeSortIdNameNat() {
  static ATermAppl t = initMakeSortIdNameNat(t);
  return t;
}

ATermAppl gsMakeSortIdNameNatPair() {
  static ATermAppl t = initMakeSortIdNameNatPair(t);
  return t;
}

ATermAppl gsMakeSortIdNameInt() {
  static ATermAppl t = initMakeSortIdNameInt(t);
  return t;
}

ATermAppl gsMakeSortIdNameReal() {
  static ATermAppl t = initMakeSortIdNameReal(t);
  return t;
}


//Creation of sort identifiers for system defined sorts.

ATermAppl gsMakeSortIdBool()
{
  return gsMakeSortId(gsMakeSortIdNameBool());
}

ATermAppl gsMakeSortIdPos()
{
  return gsMakeSortId(gsMakeSortIdNamePos());
}

ATermAppl gsMakeSortIdNat()
{
  return gsMakeSortId(gsMakeSortIdNameNat());
}

ATermAppl gsMakeSortIdNatPair()
{
  return gsMakeSortId(gsMakeSortIdNameNatPair());
}

ATermAppl gsMakeSortIdInt()
{
  return gsMakeSortId(gsMakeSortIdNameInt());
}

ATermAppl gsMakeSortIdReal()
{
  return gsMakeSortId(gsMakeSortIdNameReal());
}

//Creation of sort expressions for system defined sorts.

ATermAppl gsMakeSortExprBool()
{
  return gsMakeSortIdBool();
}

ATermAppl gsMakeSortExprPos()
{
  return gsMakeSortIdPos();
}

ATermAppl gsMakeSortExprNat()
{
  return gsMakeSortIdNat();
}

ATermAppl gsMakeSortExprNatPair()
{
  return gsMakeSortIdNatPair();
}

ATermAppl gsMakeSortExprInt()
{
  return gsMakeSortIdInt();
}

ATermAppl gsMakeSortExprReal()
{
  return gsMakeSortIdReal();
}

ATermAppl gsMakeSortExprList(ATermAppl SortExpr)
{
  return gsMakeSortCons(gsMakeSortList(), SortExpr);
}

ATermAppl gsMakeSortExprSet(ATermAppl SortExpr)
{
  return gsMakeSortCons(gsMakeSortSet(), SortExpr);
}

ATermAppl gsMakeSortExprBag(ATermAppl SortExpr)
{
  return gsMakeSortCons(gsMakeSortBag(), SortExpr);
}

//Recognition functions for system defined sort expressions

bool gsIsSortExprBool(ATermAppl SortExpr) {
  return ATisEqual(SortExpr, gsMakeSortExprBool());
}

bool gsIsSortExprPos(ATermAppl SortExpr) {
  return ATisEqual(SortExpr, gsMakeSortExprPos());
}

bool gsIsSortExprNat(ATermAppl SortExpr) {
  return ATisEqual(SortExpr, gsMakeSortExprNat());
}

bool gsIsSortExprNatPair(ATermAppl SortExpr) {
  return ATisEqual(SortExpr, gsMakeSortExprNatPair());
}

bool gsIsSortExprInt(ATermAppl SortExpr) {
  return ATisEqual(SortExpr, gsMakeSortExprInt());
}

bool gsIsSortExprReal(ATermAppl SortExpr) {
  return ATisEqual(SortExpr, gsMakeSortExprReal());
}

bool gsIsSortExprNumeric(ATermAppl SortExpr)
{
  return IsPNIRSort(SortExpr);
}

bool gsIsSortExprList(ATermAppl Term)
{
  if (!gsIsSortCons(Term))
    return false;
  else
    return gsIsSortList(ATAgetArgument(Term, 0));
}

bool gsIsSortExprSet(ATermAppl Term)
{
  if (!gsIsSortCons(Term))
    return false;
  else
    return gsIsSortSet(ATAgetArgument(Term, 0));
}

bool gsIsSortExprBag(ATermAppl Term)
{
  if (!gsIsSortCons(Term))
    return false;
  else
    return gsIsSortBag(ATAgetArgument(Term, 0));
}

const char* gsSortStructPrefix()
{
  return "Struct@";
}

const char* gsSortListPrefix()
{
  return "List@";
}

const char* gsSortSetPrefix()
{
  return "Set@";
}

const char* gsSortFSetPrefix()
{
  return "FSet@";
}

const char* gsSortBagPrefix()
{
  return "Bag@";
}

const char* gsSortFBagPrefix()
{
  return "FBag@";
}

const char* gsLambdaPrefix()
{
  return "lambda@";
}

bool gsIsStructSortId(ATermAppl SortExpr)
{
  if (gsIsSortId(SortExpr)) {
    return strncmp(
      gsSortStructPrefix(),
      ATgetName(ATgetAFun(ATAgetArgument(SortExpr, 0))),
      strlen(gsSortStructPrefix())) == 0;
  } else {
    return false;
  }
}

bool gsIsListSortId(ATermAppl SortExpr)
{
  if (gsIsSortId(SortExpr)) {
    return strncmp(
      gsSortListPrefix(),
      ATgetName(ATgetAFun(ATAgetArgument(SortExpr, 0))),
      strlen(gsSortListPrefix())) == 0;
  } else {
    return false;
  }
}

bool gsIsSetSortId(ATermAppl SortExpr)
{
  if (gsIsSortId(SortExpr)) {
    return strncmp(
      gsSortSetPrefix(),
      ATgetName(ATgetAFun(ATAgetArgument(SortExpr, 0))),
      strlen(gsSortSetPrefix())) == 0;
  } else {
    return false;
  }
}

bool gsIsFSetSortId(ATermAppl SortExpr)
{
  if (gsIsSortId(SortExpr)) {
    return strncmp(
      gsSortFSetPrefix(),
      ATgetName(ATgetAFun(ATAgetArgument(SortExpr, 0))),
      strlen(gsSortFSetPrefix())) == 0;
  } else {
    return false;
  }
}

bool gsIsBagSortId(ATermAppl SortExpr)
{
  if (gsIsSortId(SortExpr)) {
    return strncmp(
      gsSortBagPrefix(),
      ATgetName(ATgetAFun(ATAgetArgument(SortExpr, 0))),
      strlen(gsSortBagPrefix())) == 0;
  } else {
    return false;
  }
}

bool gsIsFBagSortId(ATermAppl SortExpr)
{
  if (gsIsSortId(SortExpr)) {
    return strncmp(
      gsSortFBagPrefix(),
      ATgetName(ATgetAFun(ATAgetArgument(SortExpr, 0))),
      strlen(gsSortFBagPrefix())) == 0;
  } else {
    return false;
  }
}

bool gsIsLambdaOpId(ATermAppl DataExpr)
{
  if (gsIsOpId(DataExpr)) {
    return strncmp(
      gsLambdaPrefix(),
      ATgetName(ATgetAFun(ATAgetArgument(DataExpr, 0))),
      strlen(gsLambdaPrefix())) == 0;
  } else {
    return false;
  }
}

//Auxiliary functions concerning sort expressions
ATermAppl gsMakeSortArrow1(ATermAppl SortExprDom, ATermAppl SortExprResult)
{
  return gsMakeSortArrow(ATmakeList1((ATerm) SortExprDom), SortExprResult);
}

ATermAppl gsMakeSortArrow2(ATermAppl SortExprDom1, ATermAppl SortExprDom2,
  ATermAppl SortExprResult)
{
  return gsMakeSortArrow(
    ATmakeList2((ATerm) SortExprDom1, (ATerm) SortExprDom2),
    SortExprResult);
}

ATermAppl gsMakeSortArrow3(ATermAppl SortExprDom1, ATermAppl SortExprDom2,
  ATermAppl SortExprDom3, ATermAppl SortExprResult)
{
  return gsMakeSortArrow(
    ATmakeList3((ATerm) SortExprDom1, (ATerm) SortExprDom2,
      (ATerm) SortExprDom3),
    SortExprResult);
}

ATermAppl gsMakeSortArrow4(ATermAppl SortExprDom1, ATermAppl SortExprDom2,
  ATermAppl SortExprDom3, ATermAppl SortExprDom4, ATermAppl SortExprResult)
{
  return gsMakeSortArrow(
    ATmakeList4((ATerm) SortExprDom1, (ATerm) SortExprDom2,
      (ATerm) SortExprDom3, (ATerm) SortExprDom4),
    SortExprResult);
}

ATermAppl gsMakeSortArrowList(ATermList SortExprs, ATermAppl SortExprResult)
{
  if(ATisEmpty(SortExprs))
    return SortExprResult;
  else
    return gsMakeSortArrow(SortExprs, SortExprResult);
}

ATermAppl gsGetSortExprResult(ATermAppl SortExpr)
{
  assert(gsIsSortExpr(SortExpr));
  while (gsIsSortArrow(SortExpr)) {
    SortExpr = ATAgetArgument(SortExpr, 1);
  }
  return SortExpr;
}

ATermList gsGetSortExprDomain(ATermAppl SortExpr)
{
  assert(gsIsSortExpr(SortExpr));
  ATermList l = ATmakeList0();
  while (gsIsSortArrow(SortExpr)) {
    ATermList m = ATLgetArgument(SortExpr, 0);
    while (!ATisEmpty(m)) {
      l = ATinsert(l, ATgetFirst(m));
      m = ATgetNext(m);
    }
    SortExpr = ATAgetArgument(SortExpr, 1);
  }
  l = ATreverse(l);
  return l;
}

ATermList gsGetSortExprDomains(ATermAppl SortExpr)
{
  assert(gsIsSortExpr(SortExpr));
  ATermList l = ATmakeList0();
  while (gsIsSortArrow(SortExpr)) {
    ATermList dom = ATLgetArgument(SortExpr,0);
    l = ATinsert(l, (ATerm) dom);
    SortExpr = ATAgetArgument(SortExpr,1);
  }
  l = ATreverse(l);
  return l;
}


//Data expressions
//----------------

bool gsIsDataExpr(ATermAppl Term)
{
  return gsIsId(Term)    || gsIsDataVarId(Term)    || gsIsOpId(Term)    ||
    gsIsDataAppl(Term) || gsIsBinder(Term)     || gsIsWhr(Term);
}

ATermAppl gsGetName(ATermAppl DataExpr)
{
  assert(gsIsOpId(DataExpr) || gsIsDataVarId(DataExpr));
  return ATAgetArgument(DataExpr, 0);
}

ATermAppl gsGetSort(ATermAppl DataExpr)
{
  ATermAppl Result;
  if (gsIsDataVarId(DataExpr) || gsIsOpId(DataExpr)) {
    //DataExpr is a data variable, an operation identifier or an
    //enumeration; return its sort
    Result = ATAgetArgument(DataExpr, 1);
  } else if (gsIsDataAppl(DataExpr)) {
    //DataExpr is a product data application; return the result sort of the
    //first argument
    ATermAppl Expr = ATAgetArgument(DataExpr, 0);
    ATermAppl HeadSort = gsGetSort(Expr);
    if (gsIsSortArrow(HeadSort)) {
      Result = ATAgetArgument(HeadSort, 1);
    } else {
      Result = gsMakeSortUnknown();
    }
  } else if (gsIsBinder(DataExpr)) {
      ATermAppl BindingOperator = ATAgetArgument(DataExpr, 0);
      if (gsIsForall(BindingOperator) || gsIsExists(BindingOperator)) {
        Result = gsMakeSortExprBool();
    } else if (gsIsSetBagComp(BindingOperator)) {
      //DataExpr is a set or bag comprehension; depending on the sort of the
      //body, return Set(S) or Bag(S), where S is the sort of the variable
      //declaration
      ATermAppl Var = ATAgetArgument(DataExpr, 1);
      ATermAppl SortBody = gsGetSort(ATAgetArgument(DataExpr, 2));
      if (ATisEqual(SortBody, gsMakeSortExprBool()))
        Result = gsMakeSortCons(gsMakeSortSet(), gsGetSort(Var));
      else if (ATisEqual(SortBody, gsMakeSortExprNat()))
        Result = gsMakeSortCons(gsMakeSortBag(), gsGetSort(Var));
      else
        Result = gsMakeSortUnknown();
    } else if (gsIsSetComp(BindingOperator)) {
      //DataExpr is a set comprehension, return Set(S), where S is the sort of
      //the variable declaration
      ATermAppl Var = ATAgetArgument(DataExpr, 1);
      Result = gsMakeSortCons(gsMakeSortSet(), gsGetSort(Var));
    } else if (gsIsBagComp(BindingOperator)) {
      //DataExpr is a bag comprehension, return Bag(S), where S is the sort of
      //the variable declaration
      ATermAppl Var = ATAgetArgument(DataExpr, 1);
      Result = gsMakeSortCons(gsMakeSortBag(), gsGetSort(Var));
    } else if (gsIsLambda(BindingOperator)) {
      //DataExpr is a lambda abstraction of the form
      //  lambda x0: S0, ..., xn: Sn. e
      //return S0 -> ... -> Sn -> gsGetSort(e)
      Result = gsGetSort(ATAgetArgument(DataExpr, 2));
      Result = gsMakeSortArrow(gsGetSorts(ATLgetArgument(DataExpr, 1)), Result);
    } else {
      Result = gsMakeSortUnknown();
    }
  } else if (gsIsWhr(DataExpr)) {
    //DataExpr is a where clause; return the sort of the body
    Result = gsGetSort(ATAgetArgument(DataExpr, 0));
  } else {
    //DataExpr is a data variable or operation identifier of which the sort is
    //not known; return Unknown
    Result = gsMakeSortUnknown();
  }
  return Result;
}

ATermList gsGetSorts(ATermList DataExprs)
{
  ATermList result = ATmakeList0();
  while(!ATisEmpty(DataExprs))
  {
    ATermAppl DataExpr = ATAgetFirst(DataExprs);
    assert(gsIsDataExpr(DataExpr));
    result = ATinsert(result, (ATerm) gsGetSort(DataExpr));
    DataExprs = ATgetNext(DataExprs);
  }

  return ATreverse(result);
}

ATermAppl gsGetDataExprHead(ATermAppl DataExpr)
{
  while (gsIsDataAppl(DataExpr)) {
   DataExpr = ATAgetArgument(DataExpr, 0);
  }
  return DataExpr;
}

ATermList gsGetDataExprArgs(ATermAppl DataExpr)
{
  ATermList l = ATmakeList0();
  while (gsIsDataAppl(DataExpr)) {
    l = ATconcat(ATLgetArgument(DataExpr, 1), l);
    DataExpr = ATAgetArgument(DataExpr, 0);
  }
  return l;
}

//Creation of names for system operation identifiers

ATermAppl gsMakeOpIdNameTrue() {
  static ATermAppl t = initMakeOpIdNameTrue(t);
  return t;
}

ATermAppl gsMakeOpIdNameFalse() {
  static ATermAppl t = initMakeOpIdNameFalse(t);
  return t;
}

ATermAppl gsMakeOpIdNameNot() {
  static ATermAppl t = initMakeOpIdNameNot(t);
  return t;
}

ATermAppl gsMakeOpIdNameAnd() {
  static ATermAppl t = initMakeOpIdNameAnd(t);
  return t;
}

ATermAppl gsMakeOpIdNameOr() {
  static ATermAppl t = initMakeOpIdNameOr(t);
  return t;
}

ATermAppl gsMakeOpIdNameImp() {
  static ATermAppl t = initMakeOpIdNameImp(t);
  return t;
}

ATermAppl gsMakeOpIdNameEq() {
  static ATermAppl t = initMakeOpIdNameEq(t);
  return t;
}

ATermAppl gsMakeOpIdNameNeq() {
  static ATermAppl t = initMakeOpIdNameNeq(t);
  return t;
}

ATermAppl gsMakeOpIdNameIf() {
  static ATermAppl t = initMakeOpIdNameIf(t);
  return t;
}

ATermAppl gsMakeOpIdNameForall() {
  static ATermAppl t = initMakeOpIdNameForall(t);
  return t;
}

ATermAppl gsMakeOpIdNameExists() {
  static ATermAppl t = initMakeOpIdNameExists(t);
  return t;
}

ATermAppl gsMakeOpIdNameC1() {
  static ATermAppl t = initMakeOpIdNameC1(t);
  return t;
}

ATermAppl gsMakeOpIdNameCDub() {
  static ATermAppl t = initMakeOpIdNameCDub(t);
  return t;
}

ATermAppl gsMakeOpIdNameC0() {
  static ATermAppl t = initMakeOpIdNameC0(t);
  return t;
}

ATermAppl gsMakeOpIdNameCNat() {
  static ATermAppl t = initMakeOpIdNameCNat(t);
  return t;
}

ATermAppl gsMakeOpIdNameCPair() {
  static ATermAppl t = initMakeOpIdNameCPair(t);
  return t;
}

ATermAppl gsMakeOpIdNameCNeg() {
  static ATermAppl t = initMakeOpIdNameCNeg(t);
  return t;
}

ATermAppl gsMakeOpIdNameCInt() {
  static ATermAppl t = initMakeOpIdNameCInt(t);
  return t;
}

ATermAppl gsMakeOpIdNameCReal() {
  static ATermAppl t = initMakeOpIdNameCReal(t);
  return t;
}

ATermAppl gsMakeOpIdNamePos2Nat() {
  static ATermAppl t = initMakeOpIdNamePos2Nat(t);
  return t;
}

ATermAppl gsMakeOpIdNamePos2Int() {
  static ATermAppl t = initMakeOpIdNamePos2Int(t);
  return t;
}

ATermAppl gsMakeOpIdNamePos2Real() {
  static ATermAppl t = initMakeOpIdNamePos2Real(t);
  return t;
}

ATermAppl gsMakeOpIdNameNat2Pos() {
  static ATermAppl t = initMakeOpIdNameNat2Pos(t);
  return t;
}

ATermAppl gsMakeOpIdNameNat2Int() {
  static ATermAppl t = initMakeOpIdNameNat2Int(t);
  return t;
}

ATermAppl gsMakeOpIdNameNat2Real() {
  static ATermAppl t = initMakeOpIdNameNat2Real(t);
  return t;
}

ATermAppl gsMakeOpIdNameInt2Pos() {
  static ATermAppl t = initMakeOpIdNameInt2Pos(t);
  return t;
}

ATermAppl gsMakeOpIdNameInt2Nat() {
  static ATermAppl t = initMakeOpIdNameInt2Nat(t);
  return t;
}

ATermAppl gsMakeOpIdNameInt2Real() {
  static ATermAppl t = initMakeOpIdNameInt2Real(t);
  return t;
}

ATermAppl gsMakeOpIdNameReal2Pos() {
  static ATermAppl t = initMakeOpIdNameReal2Pos(t);
  return t;
}

ATermAppl gsMakeOpIdNameReal2Nat() {
  static ATermAppl t = initMakeOpIdNameReal2Nat(t);
  return t;
}

ATermAppl gsMakeOpIdNameReal2Int() {
  static ATermAppl t = initMakeOpIdNameReal2Int(t);
  return t;
}

ATermAppl gsMakeOpIdNameLTE() {
  static ATermAppl t = initMakeOpIdNameLTE(t);
  return t;
}

ATermAppl gsMakeOpIdNameLT() {
  static ATermAppl t = initMakeOpIdNameLT(t);
  return t;
}

ATermAppl gsMakeOpIdNameGTE() {
  static ATermAppl t = initMakeOpIdNameGTE(t);
  return t;
}

ATermAppl gsMakeOpIdNameGT() {
  static ATermAppl t = initMakeOpIdNameGT(t);
  return t;
}

ATermAppl gsMakeOpIdNameMax() {
  static ATermAppl t = initMakeOpIdNameMax(t);
  return t;
}

ATermAppl gsMakeOpIdNameMin() {
  static ATermAppl t = initMakeOpIdNameMin(t);
  return t;
}

ATermAppl gsMakeOpIdNameAbs() {
  static ATermAppl t = initMakeOpIdNameAbs(t);
  return t;
}

ATermAppl gsMakeOpIdNameNeg() {
  static ATermAppl t = initMakeOpIdNameNeg(t);
  return t;
}

ATermAppl gsMakeOpIdNameSucc() {
  static ATermAppl t = initMakeOpIdNameSucc(t);
  return t;
}

ATermAppl gsMakeOpIdNamePred() {
  static ATermAppl t = initMakeOpIdNamePred(t);
  return t;
}

ATermAppl gsMakeOpIdNameDub() {
  static ATermAppl t = initMakeOpIdNameDub(t);
  return t;
}

ATermAppl gsMakeOpIdNameAdd() {
  static ATermAppl t = initMakeOpIdNameAdd(t);
  return t;
}

ATermAppl gsMakeOpIdNameAddC() {
  static ATermAppl t = initMakeOpIdNameAddC(t);
  return t;
}

ATermAppl gsMakeOpIdNameSubt() {
  static ATermAppl t = initMakeOpIdNameSubt(t);
  return t;
}

ATermAppl gsMakeOpIdNameGTESubt() {
  static ATermAppl t = initMakeOpIdNameGTESubt(t);
  return t;
}

ATermAppl gsMakeOpIdNameGTESubtB() {
  static ATermAppl t = initMakeOpIdNameGTESubtB(t);
  return t;
}

ATermAppl gsMakeOpIdNameMult() {
  static ATermAppl t = initMakeOpIdNameMult(t);
  return t;
}

ATermAppl gsMakeOpIdNameMultIR() {
  static ATermAppl t = initMakeOpIdNameMultIR(t);
  return t;
}

ATermAppl gsMakeOpIdNameDiv() {
  static ATermAppl t = initMakeOpIdNameDiv(t);
  return t;
}

ATermAppl gsMakeOpIdNameMod() {
  static ATermAppl t = initMakeOpIdNameMod(t);
  return t;
}

ATermAppl gsMakeOpIdNameDivMod() {
  static ATermAppl t = initMakeOpIdNameDivMod(t);
  return t;
}

ATermAppl gsMakeOpIdNameGDivMod() {
  static ATermAppl t = initMakeOpIdNameGDivMod(t);
  return t;
}

ATermAppl gsMakeOpIdNameGGDivMod() {
  static ATermAppl t = initMakeOpIdNameGGDivMod(t);
  return t;
}

ATermAppl gsMakeOpIdNameFirst() {
  static ATermAppl t = initMakeOpIdNameFirst(t);
  return t;
}

ATermAppl gsMakeOpIdNameLast() {
  static ATermAppl t = initMakeOpIdNameLast(t);
  return t;
}

ATermAppl gsMakeOpIdNameExp() {
  static ATermAppl t = initMakeOpIdNameExp(t);
  return t;
}

ATermAppl gsMakeOpIdNameEven() {
  static ATermAppl t = initMakeOpIdNameEven(t);
  return t;
}

ATermAppl gsMakeOpIdNameDivide() {
  static ATermAppl t = initMakeOpIdNameDivide(t);
  return t;
}

ATermAppl gsMakeOpIdNameFloor() {
  static ATermAppl t = initMakeOpIdNameFloor(t);
  return t;
}

ATermAppl gsMakeOpIdNameCeil() {
  static ATermAppl t = initMakeOpIdNameCeil(t);
  return t;
}

ATermAppl gsMakeOpIdNameRound() {
  static ATermAppl t = initMakeOpIdNameRound(t);
  return t;
}

ATermAppl gsMakeOpIdNameRedFrac() {
  static ATermAppl t = initMakeOpIdNameRedFrac(t);
  return t;
}

ATermAppl gsMakeOpIdNameRedFracWhr() {
  static ATermAppl t = initMakeOpIdNameRedFracWhr(t);
  return t;
}

ATermAppl gsMakeOpIdNameRedFracHlp() {
  static ATermAppl t = initMakeOpIdNameRedFracHlp(t);
  return t;
}

ATermAppl gsMakeOpIdNameEmptyList() {
  static ATermAppl t = initMakeOpIdNameEmptyList(t);
  return t;
}

ATermAppl gsMakeOpIdNameListEnum() {
  static ATermAppl t = initMakeOpIdNameListEnum(t);
  return t;
}

ATermAppl gsMakeOpIdNameListSize() {
  static ATermAppl t = initMakeOpIdNameListSize(t);
  return t;
}

ATermAppl gsMakeOpIdNameCons() {
  static ATermAppl t = initMakeOpIdNameCons(t);
  return t;
}

ATermAppl gsMakeOpIdNameSnoc() {
  static ATermAppl t = initMakeOpIdNameSnoc(t);
  return t;
}

ATermAppl gsMakeOpIdNameConcat() {
  static ATermAppl t = initMakeOpIdNameConcat(t);
  return t;
}

ATermAppl gsMakeOpIdNameEltAt() {
  static ATermAppl t = initMakeOpIdNameEltAt(t);
  return t;
}

ATermAppl gsMakeOpIdNameHead() {
  static ATermAppl t = initMakeOpIdNameHead(t);
  return t;
}

ATermAppl gsMakeOpIdNameTail() {
  static ATermAppl t = initMakeOpIdNameTail(t);
  return t;
}

ATermAppl gsMakeOpIdNameRHead() {
  static ATermAppl t = initMakeOpIdNameRHead(t);
  return t;
}

ATermAppl gsMakeOpIdNameRTail() {
  static ATermAppl t = initMakeOpIdNameRTail(t);
  return t;
}

ATermAppl gsMakeOpIdNameEltIn() {
  static ATermAppl t = initMakeOpIdNameEltIn(t);
  return t;
}

ATermAppl gsMakeOpIdNameSet() {
  static ATermAppl t = initMakeOpIdNameSet(t);
  return t;
}

ATermAppl gsMakeOpIdNameSetFSet() {
  static ATermAppl t = initMakeOpIdNameSetFSet(t);
  return t;
}

ATermAppl gsMakeOpIdNameSetComp() {
  static ATermAppl t = initMakeOpIdNameSetComp(t);
  return t;
}

ATermAppl gsMakeOpIdNameEmptySet() {
  static ATermAppl t = initMakeOpIdNameEmptySet(t);
  return t;
}

ATermAppl gsMakeOpIdNameSetEnum() {
  static ATermAppl t = initMakeOpIdNameSetEnum(t);
  return t;
}

ATermAppl gsMakeOpIdNameSubSetEq() {
  static ATermAppl t = initMakeOpIdNameSubSetEq(t);
  return t;
}

ATermAppl gsMakeOpIdNameSubSet() {
  static ATermAppl t = initMakeOpIdNameSubSet(t);
  return t;
}

ATermAppl gsMakeOpIdNameSetUnion() {
  static ATermAppl t = initMakeOpIdNameSetUnion(t);
  return t;
}

ATermAppl gsMakeOpIdNameSetDiff() {
  static ATermAppl t = initMakeOpIdNameSetDiff(t);
  return t;
}

ATermAppl gsMakeOpIdNameSetIntersect() {
  static ATermAppl t = initMakeOpIdNameSetIntersect(t);
  return t;
}

ATermAppl gsMakeOpIdNameSetCompl() {
  static ATermAppl t = initMakeOpIdNameSetCompl(t);
  return t;
}

ATermAppl gsMakeOpIdNameBag() {
  static ATermAppl t = initMakeOpIdNameBag(t);
  return t;
}

ATermAppl gsMakeOpIdNameBagFBag() {
  static ATermAppl t = initMakeOpIdNameBagFBag(t);
  return t;
}

ATermAppl gsMakeOpIdNameBagComp() {
  static ATermAppl t = initMakeOpIdNameBagComp(t);
  return t;
}

ATermAppl gsMakeOpIdNameEmptyBag() {
  static ATermAppl t = initMakeOpIdNameEmptyBag(t);
  return t;
}

ATermAppl gsMakeOpIdNameBagEnum() {
  static ATermAppl t = initMakeOpIdNameBagEnum(t);
  return t;
}

ATermAppl gsMakeOpIdNameCount() {
  static ATermAppl t = initMakeOpIdNameCount(t);
  return t;
}

ATermAppl gsMakeOpIdNameSubBagEq() {
  static ATermAppl t = initMakeOpIdNameSubBagEq(t);
  return t;
}

ATermAppl gsMakeOpIdNameSubBag() {
  static ATermAppl t = initMakeOpIdNameSubBag(t);
  return t;
}


ATermAppl gsMakeOpIdNameBagJoin() {
  static ATermAppl t = initMakeOpIdNameBagJoin(t);
  return t;
}

ATermAppl gsMakeOpIdNameBagDiff() {
  static ATermAppl t = initMakeOpIdNameBagDiff(t);
  return t;
}

ATermAppl gsMakeOpIdNameBagIntersect() {
  static ATermAppl t = initMakeOpIdNameBagIntersect(t);
  return t;
}

ATermAppl gsMakeOpIdNameBag2Set() {
  static ATermAppl t = initMakeOpIdNameBag2Set(t);
  return t;
}

ATermAppl gsMakeOpIdNameSet2Bag() {
  static ATermAppl t = initMakeOpIdNameSet2Bag(t);
  return t;
}

ATermAppl gsMakeOpIdNameFSetEmpty() {
  static ATermAppl t = initMakeOpIdNameFSetEmpty(t);
  return t;
}

//ATermAppl gsMakeOpIdNameFSetCons() {
//  static ATermAppl t = initMakeOpIdNameFSetCons(t);
//  return t;
//}

ATermAppl gsMakeOpIdNameFSetInsert() {
  static ATermAppl t = initMakeOpIdNameFSetInsert(t);
  return t;
}

ATermAppl gsMakeOpIdNameFSetCInsert() {
  static ATermAppl t = initMakeOpIdNameFSetCInsert(t);
  return t;
}

ATermAppl gsMakeOpIdNameFSetIn() {
  static ATermAppl t = initMakeOpIdNameFSetIn(t);
  return t;
}

ATermAppl gsMakeOpIdNameFSetLTE() {
  static ATermAppl t = initMakeOpIdNameFSetLTE(t);
  return t;
}

ATermAppl gsMakeOpIdNameFSetUnion() {
  static ATermAppl t = initMakeOpIdNameFSetUnion(t);
  return t;
}

ATermAppl gsMakeOpIdNameFSetInter() {
  static ATermAppl t = initMakeOpIdNameFSetInter(t);
  return t;
}

ATermAppl gsMakeOpIdNameFBagEmpty() {
  static ATermAppl t = initMakeOpIdNameFBagEmpty(t);
  return t;
}

//ATermAppl gsMakeOpIdNameFBagCons() {
//  static ATermAppl t = initMakeOpIdNameFBagCons(t);
//  return t;
//}

ATermAppl gsMakeOpIdNameFBagInsert() {
  static ATermAppl t = initMakeOpIdNameFBagInsert(t);
  return t;
}

ATermAppl gsMakeOpIdNameFBagCInsert() {
  static ATermAppl t = initMakeOpIdNameFBagCInsert(t);
  return t;
}

ATermAppl gsMakeOpIdNameFBagCount() {
  static ATermAppl t = initMakeOpIdNameFBagCount(t);
  return t;
}

ATermAppl gsMakeOpIdNameFBagIn() {
  static ATermAppl t = initMakeOpIdNameFBagIn(t);
  return t;
}

ATermAppl gsMakeOpIdNameFBagLTE() {
  static ATermAppl t = initMakeOpIdNameFBagLTE(t);
  return t;
}

ATermAppl gsMakeOpIdNameFBagJoin() {
  static ATermAppl t = initMakeOpIdNameFBagJoin(t);
  return t;
}

ATermAppl gsMakeOpIdNameFBagInter() {
  static ATermAppl t = initMakeOpIdNameFBagInter(t);
  return t;
}

ATermAppl gsMakeOpIdNameFBagDiff() {
  static ATermAppl t = initMakeOpIdNameFBagDiff(t);
  return t;
}

ATermAppl gsMakeOpIdNameFBag2FSet() {
  static ATermAppl t = initMakeOpIdNameFBag2FSet(t);
  return t;
}

ATermAppl gsMakeOpIdNameFSet2FBag() {
  static ATermAppl t = initMakeOpIdNameFSet2FBag(t);
  return t;
}

ATermAppl gsMakeOpIdNameMonus() {
  static ATermAppl t = initMakeOpIdNameMonus(t);
  return t;
}

ATermAppl gsMakeOpIdNameSwapZero() {
  static ATermAppl t = initMakeOpIdNameSwapZero(t);
  return t;
}

ATermAppl gsMakeOpIdNameSwapZeroAdd() {
  static ATermAppl t = initMakeOpIdNameSwapZeroAdd(t);
  return t;
}

ATermAppl gsMakeOpIdNameSwapZeroMin() {
  static ATermAppl t = initMakeOpIdNameSwapZeroMin(t);
  return t;
}

ATermAppl gsMakeOpIdNameSwapZeroMonus() {
  static ATermAppl t = initMakeOpIdNameSwapZeroMonus(t);
  return t;
}

ATermAppl gsMakeOpIdNameSwapZeroLTE() {
  static ATermAppl t = initMakeOpIdNameSwapZeroLTE(t);
  return t;
}

ATermAppl gsMakeOpIdNameFalseFunc() {
  static ATermAppl t = initMakeOpIdNameFalseFunc(t);
  return t;
}

ATermAppl gsMakeOpIdNameTrueFunc() {
  static ATermAppl t = initMakeOpIdNameTrueFunc(t);
  return t;
}

ATermAppl gsMakeOpIdNameNotFunc() {
  static ATermAppl t = initMakeOpIdNameNotFunc(t);
  return t;
}

ATermAppl gsMakeOpIdNameAndFunc() {
  static ATermAppl t = initMakeOpIdNameAndFunc(t);
  return t;
}

ATermAppl gsMakeOpIdNameOrFunc() {
  static ATermAppl t = initMakeOpIdNameOrFunc(t);
  return t;
}

ATermAppl gsMakeOpIdNameZeroFunc() {
  static ATermAppl t = initMakeOpIdNameZeroFunc(t);
  return t;
}

ATermAppl gsMakeOpIdNameOneFunc() {
  static ATermAppl t = initMakeOpIdNameOneFunc(t);
  return t;
}

ATermAppl gsMakeOpIdNameAddFunc() {
  static ATermAppl t = initMakeOpIdNameAddFunc(t);
  return t;
}

ATermAppl gsMakeOpIdNameMinFunc() {
  static ATermAppl t = initMakeOpIdNameMinFunc(t);
  return t;
}

ATermAppl gsMakeOpIdNameMonusFunc() {
  static ATermAppl t = initMakeOpIdNameMonusFunc(t);
  return t;
}

ATermAppl gsMakeOpIdNameNat2BoolFunc() {
  static ATermAppl t = initMakeOpIdNameNat2BoolFunc(t);
  return t;
}

ATermAppl gsMakeOpIdNameBool2NatFunc() {
  static ATermAppl t = initMakeOpIdNameBool2NatFunc(t);
  return t;
}

ATermAppl gsMakeOpIdNameBagElt() {
  static ATermAppl t = initMakeOpIdNameBagElt(t);
  return t;
}


//Creation of operation identifiers for system defined operations.

ATermAppl gsMakeOpIdTrue(void)
{
  return gsMakeOpId(gsMakeOpIdNameTrue(), gsMakeSortExprBool());
}

ATermAppl gsMakeOpIdFalse(void)
{
  return gsMakeOpId(gsMakeOpIdNameFalse(), gsMakeSortExprBool());
}

ATermAppl gsMakeOpIdNot(void)
{
  return gsMakeOpId(gsMakeOpIdNameNot(),
    gsMakeSortArrow1(gsMakeSortExprBool(), gsMakeSortExprBool()));
}

ATermAppl gsMakeOpIdAnd(void)
{
  return gsMakeOpId(gsMakeOpIdNameAnd(), gsMakeSortArrow2(
    gsMakeSortExprBool(), gsMakeSortExprBool(), gsMakeSortExprBool()));
}

ATermAppl gsMakeOpIdOr(void)
{
  return gsMakeOpId(gsMakeOpIdNameOr(), gsMakeSortArrow2(
    gsMakeSortExprBool(), gsMakeSortExprBool(), gsMakeSortExprBool()));
}

ATermAppl gsMakeOpIdImp(void)
{
  return gsMakeOpId(gsMakeOpIdNameImp(), gsMakeSortArrow2(
    gsMakeSortExprBool(), gsMakeSortExprBool(), gsMakeSortExprBool()));
}

ATermAppl gsMakeOpIdEq(ATermAppl SortExpr)
{
  return gsMakeOpId(gsMakeOpIdNameEq(), gsMakeSortArrow2(
    SortExpr, SortExpr, gsMakeSortExprBool()));
}

ATermAppl gsMakeOpIdNeq(ATermAppl SortExpr)
{
  return gsMakeOpId(gsMakeOpIdNameNeq(), gsMakeSortArrow2(
    SortExpr, SortExpr, gsMakeSortExprBool()));
}

ATermAppl gsMakeOpIdIf(ATermAppl SortExpr)
{
  return gsMakeOpId(gsMakeOpIdNameIf(), gsMakeSortArrow3(
    gsMakeSortExprBool(), SortExpr, SortExpr, SortExpr));
}

ATermAppl gsMakeOpIdForall(ATermAppl SortExpr)
{
  return gsMakeOpId(gsMakeOpIdNameForall(), gsMakeSortArrow1(
    SortExpr, gsMakeSortExprBool()));
}

ATermAppl gsMakeOpIdExists(ATermAppl SortExpr)
{
  return gsMakeOpId(gsMakeOpIdNameExists(), gsMakeSortArrow1(
    SortExpr, gsMakeSortExprBool()));
}

ATermAppl gsMakeOpIdC1(void)
{
  return gsMakeOpId(gsMakeOpIdNameC1(), gsMakeSortExprPos());
}

ATermAppl gsMakeOpIdCDub(void)
{
  return gsMakeOpId(gsMakeOpIdNameCDub(), gsMakeSortArrow2(
    gsMakeSortExprBool(), gsMakeSortExprPos(), gsMakeSortExprPos()));
}

ATermAppl gsMakeOpIdC0(void)
{
  return gsMakeOpId(gsMakeOpIdNameC0(), gsMakeSortExprNat());
}

ATermAppl gsMakeOpIdCNat(void)
{
  return gsMakeOpId(gsMakeOpIdNameCNat(),
    gsMakeSortArrow1(gsMakeSortExprPos(), gsMakeSortExprNat()));
}

ATermAppl gsMakeOpIdCPair(void)
{
  return gsMakeOpId(gsMakeOpIdNameCPair(),
    gsMakeSortArrow2(gsMakeSortExprNat(), gsMakeSortExprNat(),
      gsMakeSortExprNatPair()));
}

ATermAppl gsMakeOpIdCNeg(void)
{
  return gsMakeOpId(gsMakeOpIdNameCNeg(),
    gsMakeSortArrow1(gsMakeSortExprPos(), gsMakeSortExprInt()));
}

ATermAppl gsMakeOpIdCInt(void)
{
  return gsMakeOpId(gsMakeOpIdNameCInt(),
    gsMakeSortArrow1(gsMakeSortExprNat(), gsMakeSortExprInt()));
}

ATermAppl gsMakeOpIdCReal(void)
{
  return gsMakeOpId(gsMakeOpIdNameCReal(), gsMakeSortArrow2(
    gsMakeSortExprInt(), gsMakeSortExprPos(), gsMakeSortExprReal()));
}

ATermAppl gsMakeOpIdPos2Nat(void)
{
  return gsMakeOpId(gsMakeOpIdNamePos2Nat(),
    gsMakeSortArrow1(gsMakeSortExprPos(), gsMakeSortExprNat()));
}

ATermAppl gsMakeOpIdPos2Int(void)
{
  return gsMakeOpId(gsMakeOpIdNamePos2Int(),
    gsMakeSortArrow1(gsMakeSortExprPos(), gsMakeSortExprInt()));
}

ATermAppl gsMakeOpIdPos2Real(void)
{
  return gsMakeOpId(gsMakeOpIdNamePos2Real(),
    gsMakeSortArrow1(gsMakeSortExprPos(), gsMakeSortExprReal()));
}

ATermAppl gsMakeOpIdNat2Pos(void)
{
  return gsMakeOpId(gsMakeOpIdNameNat2Pos(),
    gsMakeSortArrow1(gsMakeSortExprNat(), gsMakeSortExprPos()));
}

ATermAppl gsMakeOpIdNat2Int(void)
{
  return gsMakeOpId(gsMakeOpIdNameNat2Int(),
    gsMakeSortArrow1(gsMakeSortExprNat(), gsMakeSortExprInt()));
}

ATermAppl gsMakeOpIdNat2Real(void)
{
  return gsMakeOpId(gsMakeOpIdNameNat2Real(),
    gsMakeSortArrow1(gsMakeSortExprNat(), gsMakeSortExprReal()));
}

ATermAppl gsMakeOpIdInt2Pos(void)
{
  return gsMakeOpId(gsMakeOpIdNameInt2Pos(),
    gsMakeSortArrow1(gsMakeSortExprInt(), gsMakeSortExprPos()));
}

ATermAppl gsMakeOpIdInt2Nat(void)
{
  return gsMakeOpId(gsMakeOpIdNameInt2Nat(),
    gsMakeSortArrow1(gsMakeSortExprInt(), gsMakeSortExprNat()));
}

ATermAppl gsMakeOpIdInt2Real(void)
{
  return gsMakeOpId(gsMakeOpIdNameInt2Real(),
    gsMakeSortArrow1(gsMakeSortExprInt(), gsMakeSortExprReal()));
}

ATermAppl gsMakeOpIdReal2Pos(void)
{
  return gsMakeOpId(gsMakeOpIdNameReal2Pos(),
    gsMakeSortArrow1(gsMakeSortExprReal(), gsMakeSortExprPos()));
}

ATermAppl gsMakeOpIdReal2Nat(void)
{
  return gsMakeOpId(gsMakeOpIdNameReal2Nat(),
    gsMakeSortArrow1(gsMakeSortExprReal(), gsMakeSortExprNat()));
}

ATermAppl gsMakeOpIdReal2Int(void)
{
  return gsMakeOpId(gsMakeOpIdNameReal2Int(),
    gsMakeSortArrow1(gsMakeSortExprReal(), gsMakeSortExprInt()));
}

ATermAppl gsMakeOpIdLTE(ATermAppl SortExpr)
{
  return gsMakeOpId(gsMakeOpIdNameLTE(),
    gsMakeSortArrow2(SortExpr, SortExpr, gsMakeSortExprBool()));
}

ATermAppl gsMakeOpIdLT(ATermAppl SortExpr)
{
  return gsMakeOpId(gsMakeOpIdNameLT(),
    gsMakeSortArrow2(SortExpr, SortExpr, gsMakeSortExprBool()));
}

ATermAppl gsMakeOpIdGTE(ATermAppl SortExpr)
{
  return gsMakeOpId(gsMakeOpIdNameGTE(),
    gsMakeSortArrow2(SortExpr, SortExpr, gsMakeSortExprBool()));
}

ATermAppl gsMakeOpIdGT(ATermAppl SortExpr)
{
  return gsMakeOpId(gsMakeOpIdNameGT(),
    gsMakeSortArrow2(SortExpr, SortExpr, gsMakeSortExprBool()));
}

ATermAppl gsMakeOpIdMax(ATermAppl SortExprLHS, ATermAppl SortExprRHS)
{
  assert(IsPNIRSort(SortExprLHS));
  assert(IsPNIRSort(SortExprRHS));
  assert(
    ATisEqual(SortExprLHS, gsMakeSortExprReal()) ==
    ATisEqual(SortExprRHS, gsMakeSortExprReal())
  );
  return gsMakeOpId(gsMakeOpIdNameMax(),
    gsMakeSortArrow2(SortExprLHS, SortExprRHS,
      IntersectPNIRSorts(SortExprLHS, SortExprRHS)));
}

ATermAppl gsMakeOpIdMin(ATermAppl SortExpr)
{
  assert(IsPNIRSort(SortExpr));
  return gsMakeOpId(gsMakeOpIdNameMin(),
    gsMakeSortArrow2(SortExpr, SortExpr, SortExpr));
}

ATermAppl gsMakeOpIdAbs(ATermAppl SortExpr)
{
  assert(IsPNIRSort(SortExpr));
  return gsMakeOpId(gsMakeOpIdNameAbs(),
    gsMakeSortArrow1(SortExpr,
      (SortExpr == gsMakeSortExprInt())?gsMakeSortExprNat():SortExpr
    )
  );
}

ATermAppl gsMakeOpIdNeg(ATermAppl SortExpr)
{
  assert(IsPNIRSort(SortExpr));
  return gsMakeOpId(gsMakeOpIdNameNeg(),
    gsMakeSortArrow1(SortExpr,
      UnitePNIRSorts(SortExpr, gsMakeSortExprInt())
    )
  );
}

ATermAppl gsMakeOpIdSucc(ATermAppl SortExpr)
{
  assert(IsPNIRSort(SortExpr));
  ATermAppl ResultSort;
  if (ATisEqual(SortExpr, gsMakeSortExprNat())) {
    ResultSort = gsMakeSortExprPos();
  } else {
    ResultSort = SortExpr;
  }
  return gsMakeOpId(gsMakeOpIdNameSucc(), gsMakeSortArrow1(SortExpr, ResultSort));
}

ATermAppl gsMakeOpIdPred(ATermAppl SortExpr)
{
  assert(IsPNIRSort(SortExpr));
  ATermAppl ResultSort;
  if (ATisEqual(SortExpr, gsMakeSortExprPos())) {
    ResultSort = gsMakeSortExprNat();
  } else if (ATisEqual(SortExpr, gsMakeSortExprNat())) {
    ResultSort = gsMakeSortExprInt();
  } else {
    ResultSort = SortExpr;
  }
  return gsMakeOpId(gsMakeOpIdNamePred(), gsMakeSortArrow1(SortExpr, ResultSort));
}

ATermAppl gsMakeOpIdDub(ATermAppl SortExpr)
{
  assert(IsNISort(SortExpr));
  return gsMakeOpId(gsMakeOpIdNameDub(),
    gsMakeSortArrow2(gsMakeSortExprBool(), SortExpr, SortExpr));
}

ATermAppl gsMakeOpIdAdd(ATermAppl SortExprLHS, ATermAppl SortExprRHS)
{
  assert(IsPNIRSort(SortExprLHS));
  assert(IsPNIRSort(SortExprLHS));
  assert(
    ATisEqual(SortExprLHS, gsMakeSortExprInt()) ==
    ATisEqual(SortExprRHS, gsMakeSortExprInt())
  );
  assert(
    ATisEqual(SortExprLHS, gsMakeSortExprReal()) ==
    ATisEqual(SortExprRHS, gsMakeSortExprReal())
  );
  return gsMakeOpId(gsMakeOpIdNameAdd(),
    gsMakeSortArrow2(SortExprLHS, SortExprRHS,
      IntersectPNIRSorts(SortExprLHS, SortExprRHS)
    )
  );
}

ATermAppl gsMakeOpIdAddC(void)
{
  return gsMakeOpId(gsMakeOpIdNameAddC(),
    gsMakeSortArrow3(gsMakeSortExprBool(), gsMakeSortExprPos(),
      gsMakeSortExprPos(), gsMakeSortExprPos()));
}

ATermAppl gsMakeOpIdSubt(ATermAppl SortExpr)
{
  assert(IsPNIRSort(SortExpr));
  return gsMakeOpId(gsMakeOpIdNameSubt(),
    gsMakeSortArrow2(SortExpr, SortExpr,
      UnitePNIRSorts(SortExpr, gsMakeSortExprInt())
    )
  );
}

ATermAppl gsMakeOpIdGTESubt(ATermAppl SortExpr)
{
  assert(IsPNSort(SortExpr));
  return gsMakeOpId(gsMakeOpIdNameGTESubt(),
    gsMakeSortArrow2(SortExpr, SortExpr, gsMakeSortExprNat()));
}

ATermAppl gsMakeOpIdGTESubtB(void)
{
  return gsMakeOpId(gsMakeOpIdNameGTESubtB(),
    gsMakeSortArrow3(gsMakeSortExprBool(), gsMakeSortExprPos(),
      gsMakeSortExprPos(), gsMakeSortExprNat()));
}

ATermAppl gsMakeOpIdMult(ATermAppl SortExpr)
{
  assert(IsPNIRSort(SortExpr));
  return gsMakeOpId(gsMakeOpIdNameMult(),
    gsMakeSortArrow2(SortExpr, SortExpr, SortExpr));
}

ATermAppl gsMakeOpIdMultIR(void)
{
  return gsMakeOpId(gsMakeOpIdNameMultIR(),
    gsMakeSortArrow4(gsMakeSortExprBool(), gsMakeSortExprPos(),
      gsMakeSortExprPos(), gsMakeSortExprPos(), gsMakeSortExprPos()));
}

ATermAppl gsMakeOpIdDiv(ATermAppl SortExpr)
{
  assert(IsPNISort(SortExpr));
  return gsMakeOpId(gsMakeOpIdNameDiv(),
    gsMakeSortArrow2(SortExpr, gsMakeSortExprPos(),
      UnitePNISorts(SortExpr, gsMakeSortExprNat())
    )
  );
}

ATermAppl gsMakeOpIdMod(ATermAppl SortExpr)
{
  assert(IsPNISort(SortExpr));
  return gsMakeOpId(gsMakeOpIdNameMod(),
    gsMakeSortArrow2(SortExpr, gsMakeSortExprPos(), gsMakeSortExprNat()));
}

ATermAppl gsMakeOpIdDivMod(void)
{
  return gsMakeOpId(gsMakeOpIdNameDivMod(),
    gsMakeSortArrow2(gsMakeSortExprPos(), gsMakeSortExprPos(),
      gsMakeSortExprNatPair()));
}

ATermAppl gsMakeOpIdGDivMod(void)
{
  return gsMakeOpId(gsMakeOpIdNameGDivMod(),
    gsMakeSortArrow3(gsMakeSortExprNatPair(), gsMakeSortExprBool(),
      gsMakeSortExprPos(), gsMakeSortExprNatPair()));
}

ATermAppl gsMakeOpIdGGDivMod(void)
{
  return gsMakeOpId(gsMakeOpIdNameGGDivMod(),
    gsMakeSortArrow3(gsMakeSortExprNat(), gsMakeSortExprNat(),
      gsMakeSortExprPos(), gsMakeSortExprNatPair()));
}

ATermAppl gsMakeOpIdFirst(void)
{
  return gsMakeOpId(gsMakeOpIdNameFirst(),
    gsMakeSortArrow1(gsMakeSortExprNatPair(), gsMakeSortExprNat()));
}

ATermAppl gsMakeOpIdLast(void)
{
  return gsMakeOpId(gsMakeOpIdNameLast(),
    gsMakeSortArrow1(gsMakeSortExprNatPair(), gsMakeSortExprNat()));
}

ATermAppl gsMakeOpIdExp(ATermAppl SortExpr)
{
  assert(IsPNIRSort(SortExpr));
  return gsMakeOpId(gsMakeOpIdNameExp(), gsMakeSortArrow2(SortExpr,
     IsPNISort(SortExpr)?gsMakeSortExprNat():gsMakeSortExprInt(),
     SortExpr)
  );
}

ATermAppl gsMakeOpIdEven(void)
{
  return gsMakeOpId(gsMakeOpIdNameEven(),
    gsMakeSortArrow1(gsMakeSortExprNat(), gsMakeSortExprBool()));
}

ATermAppl gsMakeOpIdDivide(ATermAppl SortExpr)
{
  assert(IsPNIRSort(SortExpr));
  return gsMakeOpId(gsMakeOpIdNameDivide(),
    gsMakeSortArrow2(SortExpr, SortExpr, gsMakeSortExprReal()));
}

ATermAppl gsMakeOpIdFloor(void)
{
  return gsMakeOpId(gsMakeOpIdNameFloor(), gsMakeSortArrow1(
    gsMakeSortExprReal(), gsMakeSortExprInt()));
}

ATermAppl gsMakeOpIdCeil(void)
{
  return gsMakeOpId(gsMakeOpIdNameCeil(), gsMakeSortArrow1(
    gsMakeSortExprReal(), gsMakeSortExprInt()));
}

ATermAppl gsMakeOpIdRound(void)
{
  return gsMakeOpId(gsMakeOpIdNameRound(), gsMakeSortArrow1(
    gsMakeSortExprReal(), gsMakeSortExprInt()));
}

ATermAppl gsMakeOpIdRedFrac(void)
{
  return gsMakeOpId(gsMakeOpIdNameRedFrac(), gsMakeSortArrow2(
    gsMakeSortExprInt(), gsMakeSortExprInt(), gsMakeSortExprReal()));
}

ATermAppl gsMakeOpIdRedFracWhr(void)
{
  return gsMakeOpId(gsMakeOpIdNameRedFracWhr(), gsMakeSortArrow3(
    gsMakeSortExprPos(), gsMakeSortExprInt(), gsMakeSortExprNat(), gsMakeSortExprReal()));
}

ATermAppl gsMakeOpIdRedFracHlp(void)
{
  return gsMakeOpId(gsMakeOpIdNameRedFracHlp(), gsMakeSortArrow2(
    gsMakeSortExprReal(), gsMakeSortExprInt(), gsMakeSortExprReal()));
}

ATermAppl gsMakeOpIdEmptyList(ATermAppl SortExpr)
{
  return gsMakeOpId(gsMakeOpIdNameEmptyList(), SortExpr);
}

ATermAppl gsMakeOpIdListEnum(ATermAppl SortExpr)
{
  return gsMakeOpId(gsMakeOpIdNameListEnum(), SortExpr);
}

ATermAppl gsMakeOpIdListSize(ATermAppl SortExpr)
{
  return gsMakeOpId(gsMakeOpIdNameListSize(),
    gsMakeSortArrow1(SortExpr, gsMakeSortExprNat()));
}

ATermAppl gsMakeOpIdCons(ATermAppl SortExprLHS, ATermAppl SortExprRHS)
{
  return gsMakeOpId(gsMakeOpIdNameCons(),
    gsMakeSortArrow2(SortExprLHS, SortExprRHS, SortExprRHS));

}

ATermAppl gsMakeOpIdSnoc(ATermAppl SortExprLHS, ATermAppl SortExprRHS)
{
  return gsMakeOpId(gsMakeOpIdNameSnoc(),
    gsMakeSortArrow2(SortExprLHS, SortExprRHS, SortExprLHS));
}

ATermAppl gsMakeOpIdConcat(ATermAppl SortExpr)
{
  return gsMakeOpId(gsMakeOpIdNameConcat(),
    gsMakeSortArrow2(SortExpr, SortExpr, SortExpr));
}

ATermAppl gsMakeOpIdEltAt(ATermAppl SortExprDom, ATermAppl SortExprResult)
{
  return gsMakeOpId(gsMakeOpIdNameEltAt(),
    gsMakeSortArrow2(SortExprDom, gsMakeSortExprNat(), SortExprResult));
}

ATermAppl gsMakeOpIdHead(ATermAppl SortExprDom, ATermAppl SortExprResult)
{
  return gsMakeOpId(gsMakeOpIdNameHead(),
    gsMakeSortArrow1(SortExprDom, SortExprResult));
}

ATermAppl gsMakeOpIdTail(ATermAppl SortExpr)
{
  return gsMakeOpId(gsMakeOpIdNameTail(),
    gsMakeSortArrow1(SortExpr, SortExpr));
}

ATermAppl gsMakeOpIdRHead(ATermAppl SortExprDom, ATermAppl SortExprResult)
{
  return gsMakeOpId(gsMakeOpIdNameRHead(),
    gsMakeSortArrow1(SortExprDom, SortExprResult));
}

ATermAppl gsMakeOpIdRTail(ATermAppl SortExpr)
{
  return gsMakeOpId(gsMakeOpIdNameRTail(),
    gsMakeSortArrow1(SortExpr, SortExpr));
}

ATermAppl gsMakeOpIdSet(ATermAppl SortExprElt, ATermAppl SortExprFSet, ATermAppl SortExprSet)
{
  return gsMakeOpId(gsMakeOpIdNameSet(), gsMakeSortArrow2(
    gsMakeSortArrow1(SortExprElt, gsMakeSortExprBool()), SortExprFSet, SortExprSet));
}

ATermAppl gsMakeOpIdSetFSet(ATermAppl SortExprFSet, ATermAppl SortExprSet)
{
  return gsMakeOpId(gsMakeOpIdNameSetFSet(), gsMakeSortArrow1(
    SortExprFSet, SortExprSet));
}

ATermAppl gsMakeOpIdSetComp(ATermAppl SortExprElt, ATermAppl SortExprSet)
{
  return gsMakeOpId(gsMakeOpIdNameSetComp(), gsMakeSortArrow1(
    gsMakeSortArrow1(SortExprElt, gsMakeSortExprBool()), SortExprSet));
}

ATermAppl gsMakeOpIdEmptySet(ATermAppl SortExpr)
{
  return gsMakeOpId(gsMakeOpIdNameEmptySet(), SortExpr);
}

ATermAppl gsMakeOpIdSetEnum(ATermAppl SortExpr)
{
  return gsMakeOpId(gsMakeOpIdNameSetEnum(), SortExpr);
}

ATermAppl gsMakeOpIdEltIn(ATermAppl SortExprLHS, ATermAppl SortExprRHS)
{
  return gsMakeOpId(gsMakeOpIdNameEltIn(),
    gsMakeSortArrow2(SortExprLHS, SortExprRHS, gsMakeSortExprBool()));
}

ATermAppl gsMakeOpIdSetUnion(ATermAppl SortExpr)
{
  return gsMakeOpId(gsMakeOpIdNameSetUnion(),
    gsMakeSortArrow2(SortExpr, SortExpr, SortExpr));
}

ATermAppl gsMakeOpIdSetDiff(ATermAppl SortExpr)
{
  return gsMakeOpId(gsMakeOpIdNameSetDiff(),
    gsMakeSortArrow2(SortExpr, SortExpr, SortExpr));
}

ATermAppl gsMakeOpIdSetIntersect(ATermAppl SortExpr)
{
  return gsMakeOpId(gsMakeOpIdNameSetIntersect(),
    gsMakeSortArrow2(SortExpr, SortExpr, SortExpr));
}

ATermAppl gsMakeOpIdSetCompl(ATermAppl SortExpr)
{
  return gsMakeOpId(gsMakeOpIdNameSetCompl(),
    gsMakeSortArrow1(SortExpr, SortExpr));
}

ATermAppl gsMakeOpIdBag(ATermAppl SortExprElt, ATermAppl SortExprFBag, ATermAppl SortExprBag)
{
  return gsMakeOpId(gsMakeOpIdNameBag(), gsMakeSortArrow2(
    gsMakeSortArrow1(SortExprElt, gsMakeSortExprNat()), SortExprFBag, SortExprBag));
}

ATermAppl gsMakeOpIdBagFBag(ATermAppl SortExprFBag, ATermAppl SortExprBag)
{
  return gsMakeOpId(gsMakeOpIdNameBagFBag(), gsMakeSortArrow1(
    SortExprFBag, SortExprBag));
}

ATermAppl gsMakeOpIdBagComp(ATermAppl SortExprElt, ATermAppl SortExprBag)
{
  return gsMakeOpId(gsMakeOpIdNameBagComp(), gsMakeSortArrow1(
    gsMakeSortArrow1(SortExprElt, gsMakeSortExprNat()), SortExprBag));
}

ATermAppl gsMakeOpIdEmptyBag(ATermAppl SortExpr)
{
  return gsMakeOpId(gsMakeOpIdNameEmptyBag(), SortExpr);
}

ATermAppl gsMakeOpIdBagEnum(ATermAppl SortExpr)
{
  return gsMakeOpId(gsMakeOpIdNameBagEnum(), SortExpr);
}

ATermAppl gsMakeOpIdCount(ATermAppl SortExprLHS, ATermAppl SortExprRHS)
{
  return gsMakeOpId(gsMakeOpIdNameCount(),
    gsMakeSortArrow2(SortExprLHS, SortExprRHS, gsMakeSortExprNat()));
}

ATermAppl gsMakeOpIdBagJoin(ATermAppl SortExpr)
{
  return gsMakeOpId(gsMakeOpIdNameBagJoin(),
    gsMakeSortArrow2(SortExpr, SortExpr, SortExpr));
}

ATermAppl gsMakeOpIdBagDiff(ATermAppl SortExpr)
{
  return gsMakeOpId(gsMakeOpIdNameBagDiff(),
    gsMakeSortArrow2(SortExpr, SortExpr, SortExpr));
}

ATermAppl gsMakeOpIdBagIntersect(ATermAppl SortExpr)
{
  return gsMakeOpId(gsMakeOpIdNameBagIntersect(),
    gsMakeSortArrow2(SortExpr, SortExpr, SortExpr));
}

ATermAppl gsMakeOpIdBag2Set(ATermAppl SortExprDom,
  ATermAppl SortExprResult)
{
  return gsMakeOpId(gsMakeOpIdNameBag2Set(),
    gsMakeSortArrow1(SortExprDom, SortExprResult));
}

ATermAppl gsMakeOpIdSet2Bag(ATermAppl SortExprDom,
  ATermAppl SortExprResult)
{
  return gsMakeOpId(gsMakeOpIdNameSet2Bag(),
    gsMakeSortArrow1(SortExprDom, SortExprResult));
}

ATermAppl gsMakeOpIdFSetEmpty(ATermAppl SortExprFSet)
{
  return gsMakeOpId(gsMakeOpIdNameFSetEmpty(), SortExprFSet);
}

//ATermAppl gsMakeOpIdFSetCons(ATermAppl SortExprElt, ATermAppl SortExprFSet)
//{
//  return gsMakeOpId(gsMakeOpIdNameFSetCons(),
//    gsMakeSortArrow2(SortExprElt, SortExprFSet, SortExprFSet));
//}

ATermAppl gsMakeOpIdFSetInsert(ATermAppl SortExprElt, ATermAppl SortExprFSet)
{
  return gsMakeOpId(gsMakeOpIdNameFSetInsert(),
    gsMakeSortArrow2(SortExprElt, SortExprFSet, SortExprFSet));
}

ATermAppl gsMakeOpIdFSetCInsert(ATermAppl SortExprElt, ATermAppl SortExprFSet)
{
  return gsMakeOpId(gsMakeOpIdNameFSetCInsert(),
    gsMakeSortArrow3(SortExprElt, gsMakeSortExprBool(), SortExprFSet, SortExprFSet));
}

ATermAppl gsMakeOpIdFSetIn(ATermAppl SortExprElt, ATermAppl SortExprFSet)
{
  return gsMakeOpId(gsMakeOpIdNameFSetIn(),
    gsMakeSortArrow2(SortExprElt, SortExprFSet, gsMakeSortExprBool()));
}

ATermAppl gsMakeOpIdFSetLTE(ATermAppl SortExprElt, ATermAppl SortExprFSet)
{
  return gsMakeOpId(gsMakeOpIdNameFSetLTE(),
    gsMakeSortArrow3(
      gsMakeSortArrow1(SortExprElt, gsMakeSortExprBool()),
      SortExprFSet, SortExprFSet, gsMakeSortExprBool()));
}

ATermAppl gsMakeOpIdFSetUnion(ATermAppl SortExprElt, ATermAppl SortExprFSet)
{
  return gsMakeOpId(gsMakeOpIdNameFSetUnion(),
    gsMakeSortArrow4(
      gsMakeSortArrow1(SortExprElt, gsMakeSortExprBool()),
      gsMakeSortArrow1(SortExprElt, gsMakeSortExprBool()),
      SortExprFSet, SortExprFSet, SortExprFSet));
}

ATermAppl gsMakeOpIdFSetInter(ATermAppl SortExprElt, ATermAppl SortExprFSet)
{
  return gsMakeOpId(gsMakeOpIdNameFSetInter(),
    gsMakeSortArrow4(
      gsMakeSortArrow1(SortExprElt, gsMakeSortExprBool()),
      gsMakeSortArrow1(SortExprElt, gsMakeSortExprBool()),
      SortExprFSet, SortExprFSet, SortExprFSet));
}

ATermAppl gsMakeOpIdFBagEmpty(ATermAppl SortExprFBag)
{
  return gsMakeOpId(gsMakeOpIdNameFBagEmpty(), SortExprFBag);
}

//ATermAppl gsMakeOpIdFBagCons(ATermAppl SortExprElt, ATermAppl SortExprFBag)
//{
//  return gsMakeOpId(gsMakeOpIdNameFBagCons(),
//    gsMakeSortArrow3(SortExprElt, gsMakeSortExprPos(), SortExprFBag, SortExprFBag));
//}

ATermAppl gsMakeOpIdFBagInsert(ATermAppl SortExprElt, ATermAppl SortExprFBag)
{
  return gsMakeOpId(gsMakeOpIdNameFBagInsert(),
    gsMakeSortArrow3(SortExprElt, gsMakeSortExprPos(), SortExprFBag, SortExprFBag));
}

ATermAppl gsMakeOpIdFBagCInsert(ATermAppl SortExprElt, ATermAppl SortExprFBag)
{
  return gsMakeOpId(gsMakeOpIdNameFBagCInsert(),
    gsMakeSortArrow3(SortExprElt, gsMakeSortExprNat(), SortExprFBag, SortExprFBag));
}

ATermAppl gsMakeOpIdFBagCount(ATermAppl SortExprElt, ATermAppl SortExprFBag)
{
  return gsMakeOpId(gsMakeOpIdNameFBagCount(),
    gsMakeSortArrow2(SortExprElt, SortExprFBag, gsMakeSortExprNat()));
}

ATermAppl gsMakeOpIdFBagIn(ATermAppl SortExprElt, ATermAppl SortExprFBag)
{
  return gsMakeOpId(gsMakeOpIdNameFBagIn(),
    gsMakeSortArrow2(SortExprElt, SortExprFBag, gsMakeSortExprBool()));
}

ATermAppl gsMakeOpIdFBagLTE(ATermAppl SortExprElt, ATermAppl SortExprFBag)
{
  return gsMakeOpId(gsMakeOpIdNameFBagLTE(),
    gsMakeSortArrow3(
      gsMakeSortArrow1(SortExprElt, gsMakeSortExprNat()),
      SortExprFBag, SortExprFBag, gsMakeSortExprBool()));
}

ATermAppl gsMakeOpIdFBagJoin(ATermAppl SortExprElt, ATermAppl SortExprFBag)
{
  return gsMakeOpId(gsMakeOpIdNameFBagJoin(),
    gsMakeSortArrow4(
      gsMakeSortArrow1(SortExprElt, gsMakeSortExprNat()),
      gsMakeSortArrow1(SortExprElt, gsMakeSortExprNat()),
      SortExprFBag, SortExprFBag, SortExprFBag));
}

ATermAppl gsMakeOpIdFBagInter(ATermAppl SortExprElt, ATermAppl SortExprFBag)
{
  return gsMakeOpId(gsMakeOpIdNameFBagInter(),
    gsMakeSortArrow4(
      gsMakeSortArrow1(SortExprElt, gsMakeSortExprNat()),
      gsMakeSortArrow1(SortExprElt, gsMakeSortExprNat()),
      SortExprFBag, SortExprFBag, SortExprFBag));
}

ATermAppl gsMakeOpIdFBagDiff(ATermAppl SortExprElt, ATermAppl SortExprFBag)
{
  return gsMakeOpId(gsMakeOpIdNameFBagDiff(),
    gsMakeSortArrow4(
      gsMakeSortArrow1(SortExprElt, gsMakeSortExprNat()),
      gsMakeSortArrow1(SortExprElt, gsMakeSortExprNat()),
      SortExprFBag, SortExprFBag, SortExprFBag));
}

ATermAppl gsMakeOpIdFBag2FSet(ATermAppl SortExprElt, ATermAppl SortExprFBag, ATermAppl SortExprFSet)
{
  return gsMakeOpId(gsMakeOpIdNameFBag2FSet(),
    gsMakeSortArrow2(
      gsMakeSortArrow1(SortExprElt, gsMakeSortExprNat()),
      SortExprFBag, SortExprFSet));
}

ATermAppl gsMakeOpIdFSet2FBag(ATermAppl SortExprFSet, ATermAppl SortExprFBag)
{
  return gsMakeOpId(gsMakeOpIdNameFSet2FBag(),
    gsMakeSortArrow1(SortExprFSet, SortExprFBag));
}

ATermAppl gsMakeOpIdMonus()
{
  return gsMakeOpId(gsMakeOpIdNameMonus(),
    gsMakeSortArrow2(gsMakeSortExprNat(), gsMakeSortExprNat(), gsMakeSortExprNat()));
}

ATermAppl gsMakeOpIdSwapZero()
{
  return gsMakeOpId(gsMakeOpIdNameSwapZero(),
    gsMakeSortArrow2(gsMakeSortExprNat(), gsMakeSortExprNat(), gsMakeSortExprNat()));
}

ATermAppl gsMakeOpIdSwapZeroAdd()
{
  return gsMakeOpId(gsMakeOpIdNameSwapZeroAdd(),
    gsMakeSortArrow4(gsMakeSortExprNat(), gsMakeSortExprNat(), gsMakeSortExprNat(), gsMakeSortExprNat(), gsMakeSortExprNat()));
}

ATermAppl gsMakeOpIdSwapZeroMin()
{
  return gsMakeOpId(gsMakeOpIdNameSwapZeroMin(),
    gsMakeSortArrow4(gsMakeSortExprNat(), gsMakeSortExprNat(), gsMakeSortExprNat(), gsMakeSortExprNat(), gsMakeSortExprNat()));
}

ATermAppl gsMakeOpIdSwapZeroMonus()
{
  return gsMakeOpId(gsMakeOpIdNameSwapZeroMonus(),
    gsMakeSortArrow4(gsMakeSortExprNat(), gsMakeSortExprNat(), gsMakeSortExprNat(), gsMakeSortExprNat(), gsMakeSortExprNat()));
}

ATermAppl gsMakeOpIdSwapZeroLTE()
{
  return gsMakeOpId(gsMakeOpIdNameSwapZeroLTE(),
    gsMakeSortArrow3(gsMakeSortExprNat(), gsMakeSortExprNat(), gsMakeSortExprNat(), gsMakeSortExprBool()));
}

ATermAppl gsMakeOpIdFalseFunc(ATermAppl SortExprElt)
{
  return gsMakeOpId(gsMakeOpIdNameFalseFunc(),
    gsMakeSortArrow1(SortExprElt, gsMakeSortExprBool()));
}

ATermAppl gsMakeOpIdTrueFunc(ATermAppl SortExprElt)
{
  return gsMakeOpId(gsMakeOpIdNameTrueFunc(),
    gsMakeSortArrow1(SortExprElt, gsMakeSortExprBool()));
}

ATermAppl gsMakeOpIdNotFunc(ATermAppl SortExprElt)
{
  return gsMakeOpId(gsMakeOpIdNameNotFunc(),
    gsMakeSortArrow1(
      gsMakeSortArrow1(SortExprElt, gsMakeSortExprBool()),
      gsMakeSortArrow1(SortExprElt, gsMakeSortExprBool())
  ));
}

ATermAppl gsMakeOpIdAndFunc(ATermAppl SortExprElt)
{
  return gsMakeOpId(gsMakeOpIdNameAndFunc(),
    gsMakeSortArrow2(
      gsMakeSortArrow1(SortExprElt, gsMakeSortExprBool()),
      gsMakeSortArrow1(SortExprElt, gsMakeSortExprBool()),
      gsMakeSortArrow1(SortExprElt, gsMakeSortExprBool())
  ));
}

ATermAppl gsMakeOpIdOrFunc(ATermAppl SortExprElt)
{
  return gsMakeOpId(gsMakeOpIdNameOrFunc(),
    gsMakeSortArrow2(
      gsMakeSortArrow1(SortExprElt, gsMakeSortExprBool()),
      gsMakeSortArrow1(SortExprElt, gsMakeSortExprBool()),
      gsMakeSortArrow1(SortExprElt, gsMakeSortExprBool())
  ));
}

ATermAppl gsMakeOpIdZeroFunc(ATermAppl SortExprElt)
{
  return gsMakeOpId(gsMakeOpIdNameZeroFunc(),
    gsMakeSortArrow1(SortExprElt, gsMakeSortExprNat()));
}

ATermAppl gsMakeOpIdOneFunc(ATermAppl SortExprElt)
{
  return gsMakeOpId(gsMakeOpIdNameOneFunc(),
    gsMakeSortArrow1(SortExprElt, gsMakeSortExprNat()));
}

ATermAppl gsMakeOpIdAddFunc(ATermAppl SortExprElt)
{
  return gsMakeOpId(gsMakeOpIdNameAddFunc(),
    gsMakeSortArrow2(
      gsMakeSortArrow1(SortExprElt, gsMakeSortExprNat()),
      gsMakeSortArrow1(SortExprElt, gsMakeSortExprNat()),
      gsMakeSortArrow1(SortExprElt, gsMakeSortExprNat())
  ));
}

ATermAppl gsMakeOpIdMinFunc(ATermAppl SortExprElt)
{
  return gsMakeOpId(gsMakeOpIdNameMinFunc(),
    gsMakeSortArrow2(
      gsMakeSortArrow1(SortExprElt, gsMakeSortExprNat()),
      gsMakeSortArrow1(SortExprElt, gsMakeSortExprNat()),
      gsMakeSortArrow1(SortExprElt, gsMakeSortExprNat())
  ));
}

ATermAppl gsMakeOpIdMonusFunc(ATermAppl SortExprElt)
{
  return gsMakeOpId(gsMakeOpIdNameMonusFunc(),
    gsMakeSortArrow2(
      gsMakeSortArrow1(SortExprElt, gsMakeSortExprNat()),
      gsMakeSortArrow1(SortExprElt, gsMakeSortExprNat()),
      gsMakeSortArrow1(SortExprElt, gsMakeSortExprNat())
  ));
}

ATermAppl gsMakeOpIdNat2BoolFunc(ATermAppl SortExprElt)
{
  return gsMakeOpId(gsMakeOpIdNameNat2BoolFunc(),
    gsMakeSortArrow1(
      gsMakeSortArrow1(SortExprElt, gsMakeSortExprNat()),
      gsMakeSortArrow1(SortExprElt, gsMakeSortExprBool())
  ));
}

ATermAppl gsMakeOpIdBool2NatFunc(ATermAppl SortExprElt)
{
  return gsMakeOpId(gsMakeOpIdNameBool2NatFunc(),
    gsMakeSortArrow1(
      gsMakeSortArrow1(SortExprElt, gsMakeSortExprBool()),
      gsMakeSortArrow1(SortExprElt, gsMakeSortExprNat())
  ));
}

ATermAppl gsMakeOpIdBagElt(ATermAppl SortExprElt, ATermAppl SortExprFBagElt)
{
  return gsMakeOpId(gsMakeOpIdNameBagElt(),
    gsMakeSortArrow2(SortExprElt, gsMakeSortExprPos(), SortExprFBagElt)
  );
}


//Creation of data expressions for system defined operations.

ATermAppl gsMakeDataExprTrue(void)
{
  return gsMakeOpIdTrue();
}

ATermAppl gsMakeDataExprFalse(void)
{
  return gsMakeOpIdFalse();
}

ATermAppl gsMakeDataExprNot(ATermAppl DataExpr)
{
  return gsMakeDataAppl1(gsMakeOpIdNot(), DataExpr);
}

ATermAppl gsMakeDataExprAnd(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  return gsMakeDataAppl2(gsMakeOpIdAnd(), DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprOr(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  return gsMakeDataAppl2(gsMakeOpIdOr(), DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprImp(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  return gsMakeDataAppl2(gsMakeOpIdImp(), DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprForall(ATermAppl DataExpr)
{
  ATermAppl ExprSort = gsGetSort(DataExpr);
  assert(gsIsSortArrow(ExprSort));
  assert(ATisEqual(gsGetSortExprResult(ExprSort), gsMakeSortExprBool()));
  // ExprSort is of the form S->Bool
  return gsMakeDataAppl1(gsMakeOpIdForall(ExprSort),DataExpr);
}

ATermAppl gsMakeDataExprExists(ATermAppl DataExpr)
{
  ATermAppl ExprSort = gsGetSort(DataExpr);
  assert(gsIsSortArrow(ExprSort));
  assert(ATisEqual(gsGetSortExprResult(ExprSort), gsMakeSortExprBool()));
  // ExprSort is of the form S->Bool
  return gsMakeDataAppl1(gsMakeOpIdExists(ExprSort), DataExpr);
}

ATermAppl gsMakeDataExprEq(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  ATermAppl ExprSort = gsGetSort(DataExprLHS);
  assert(!gsIsSortUnknown(ExprSort));
  assert(ATisEqual(ExprSort, gsGetSort(DataExprRHS)));
  return gsMakeDataAppl2(gsMakeOpIdEq(ExprSort), DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprNeq(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  ATermAppl ExprSort = gsGetSort(DataExprLHS);
  assert(!gsIsSortUnknown(ExprSort));
  assert(ATisEqual(ExprSort, gsGetSort(DataExprRHS)));
  return gsMakeDataAppl2(gsMakeOpIdNeq(ExprSort), DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprIf(ATermAppl DataExprCond, ATermAppl DataExprThen,
  ATermAppl DataExprElse)
{
  assert(ATisEqual(gsGetSort(DataExprCond), gsMakeSortIdBool()));
  ATermAppl ExprSort = gsGetSort(DataExprThen);
  assert(!gsIsSortUnknown(ExprSort));
  assert(ATisEqual(ExprSort, gsGetSort(DataExprElse)));
  return gsMakeDataAppl3(gsMakeOpIdIf(ExprSort), DataExprCond, DataExprThen,
    DataExprElse);
}

ATermAppl gsMakeDataExprC1(void)
{
  return gsMakeOpIdC1();
}

ATermAppl gsMakeDataExprCDub(ATermAppl DataExprBit, ATermAppl DataExprPos)
{
  assert(ATisEqual(gsGetSort(DataExprBit), gsMakeSortExprBool()));
  assert(ATisEqual(gsGetSort(DataExprPos), gsMakeSortExprPos()));
  return gsMakeDataAppl2(gsMakeOpIdCDub(), DataExprBit, DataExprPos);
}

ATermAppl gsMakeDataExprC0(void)
{
  return gsMakeOpIdC0();
}

ATermAppl gsMakeDataExprCNat(ATermAppl DataExpr)
{
  assert(ATisEqual(gsGetSort(DataExpr), gsMakeSortExprPos()));
  return gsMakeDataAppl1(gsMakeOpIdCNat(), DataExpr);
}

ATermAppl gsMakeDataExprCPair(ATermAppl DataExprFst, ATermAppl DataExprLst)
{
  assert(ATisEqual(gsGetSort(DataExprFst), gsMakeSortExprNat()));
  assert(ATisEqual(gsGetSort(DataExprLst), gsMakeSortExprNat()));
  return gsMakeDataAppl2(gsMakeOpIdCPair(), DataExprFst, DataExprLst);
}

ATermAppl gsMakeDataExprCNeg(ATermAppl DataExpr)
{
  assert(ATisEqual(gsGetSort(DataExpr), gsMakeSortExprPos()));
  return gsMakeDataAppl1(gsMakeOpIdCNeg(), DataExpr);
}

ATermAppl gsMakeDataExprCInt(ATermAppl DataExpr)
{
  assert(ATisEqual(gsGetSort(DataExpr), gsMakeSortExprNat()));
  return gsMakeDataAppl1(gsMakeOpIdCInt(), DataExpr);
}

ATermAppl gsMakeDataExprCReal(ATermAppl DataExprInt, ATermAppl DataExprPos)
{
  assert(ATisEqual(gsGetSort(DataExprInt), gsMakeSortExprInt()));
  assert(ATisEqual(gsGetSort(DataExprPos), gsMakeSortExprPos()));
  return gsMakeDataAppl2(gsMakeOpIdCReal(), DataExprInt, DataExprPos);
}

ATermAppl gsMakeDataExprPos2Nat(ATermAppl DataExpr)
{
  assert(ATisEqual(gsGetSort(DataExpr), gsMakeSortExprPos()));
  return gsMakeDataAppl1(gsMakeOpIdPos2Nat(), DataExpr);
}

ATermAppl gsMakeDataExprPos2Int(ATermAppl DataExpr)
{
  assert(ATisEqual(gsGetSort(DataExpr), gsMakeSortExprPos()));
  return gsMakeDataAppl1(gsMakeOpIdPos2Int(), DataExpr);
}

ATermAppl gsMakeDataExprPos2Real(ATermAppl DataExpr)
{
  assert(ATisEqual(gsGetSort(DataExpr), gsMakeSortExprPos()));
  return gsMakeDataAppl1(gsMakeOpIdPos2Real(), DataExpr);
}

ATermAppl gsMakeDataExprNat2Pos(ATermAppl DataExpr)
{
  assert(ATisEqual(gsGetSort(DataExpr), gsMakeSortExprNat()));
  return gsMakeDataAppl1(gsMakeOpIdNat2Pos(), DataExpr);
}

ATermAppl gsMakeDataExprNat2Int(ATermAppl DataExpr)
{
  assert(ATisEqual(gsGetSort(DataExpr), gsMakeSortExprNat()));
  return gsMakeDataAppl1(gsMakeOpIdNat2Int(), DataExpr);
}

ATermAppl gsMakeDataExprNat2Real(ATermAppl DataExpr)
{
  assert(ATisEqual(gsGetSort(DataExpr), gsMakeSortExprNat()));
  return gsMakeDataAppl1(gsMakeOpIdNat2Real(), DataExpr);
}

ATermAppl gsMakeDataExprInt2Pos(ATermAppl DataExpr)
{
  assert(ATisEqual(gsGetSort(DataExpr), gsMakeSortExprInt()));
  return gsMakeDataAppl1(gsMakeOpIdInt2Pos(), DataExpr);
}

ATermAppl gsMakeDataExprInt2Nat(ATermAppl DataExpr)
{
  assert(ATisEqual(gsGetSort(DataExpr), gsMakeSortExprInt()));
  return gsMakeDataAppl1(gsMakeOpIdInt2Nat(), DataExpr);
}

ATermAppl gsMakeDataExprInt2Real(ATermAppl DataExpr)
{
  assert(ATisEqual(gsGetSort(DataExpr), gsMakeSortExprInt()));
  return gsMakeDataAppl1(gsMakeOpIdInt2Real(), DataExpr);
}

ATermAppl gsMakeDataExprReal2Pos(ATermAppl DataExpr)
{
  assert(ATisEqual(gsGetSort(DataExpr), gsMakeSortExprReal()));
  return gsMakeDataAppl1(gsMakeOpIdReal2Pos(), DataExpr);
}

ATermAppl gsMakeDataExprReal2Nat(ATermAppl DataExpr)
{
  assert(ATisEqual(gsGetSort(DataExpr), gsMakeSortExprReal()));
  return gsMakeDataAppl1(gsMakeOpIdReal2Nat(), DataExpr);
}

ATermAppl gsMakeDataExprReal2Int(ATermAppl DataExpr)
{
  assert(ATisEqual(gsGetSort(DataExpr), gsMakeSortExprReal()));
  return gsMakeDataAppl1(gsMakeOpIdReal2Int(), DataExpr);
}

ATermAppl gsMakeDataExprLTE(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprLHS), gsGetSort(DataExprRHS)));
  return gsMakeDataAppl2(gsMakeOpIdLTE(gsGetSort(DataExprLHS)),
    DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprLT(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprLHS), gsGetSort(DataExprRHS)));
  return gsMakeDataAppl2(gsMakeOpIdLT(gsGetSort(DataExprLHS)),
    DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprGTE(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprLHS), gsGetSort(DataExprRHS)));
  return gsMakeDataAppl2(gsMakeOpIdGTE(gsGetSort(DataExprLHS)),
    DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprGT(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprLHS), gsGetSort(DataExprRHS)));
  return gsMakeDataAppl2(gsMakeOpIdGT(gsGetSort(DataExprLHS)),
    DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprMax(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  return gsMakeDataAppl2(
    gsMakeOpIdMax(gsGetSort(DataExprLHS), gsGetSort(DataExprRHS)),
    DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprMin(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprLHS), gsGetSort(DataExprRHS)));
  return gsMakeDataAppl2(gsMakeOpIdMin(gsGetSort(DataExprLHS)),
    DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprAbs(ATermAppl DataExpr)
{
  return gsMakeDataAppl1(gsMakeOpIdAbs(gsGetSort(DataExpr)), DataExpr);
}

ATermAppl gsMakeDataExprNeg(ATermAppl DataExpr)
{
  return gsMakeDataAppl1(gsMakeOpIdNeg(gsGetSort(DataExpr)), DataExpr);
}

ATermAppl gsMakeDataExprSucc(ATermAppl DataExpr)
{
  return gsMakeDataAppl1(gsMakeOpIdSucc(gsGetSort(DataExpr)), DataExpr);
}

ATermAppl gsMakeDataExprPred(ATermAppl DataExpr)
{
  return gsMakeDataAppl1(gsMakeOpIdPred(gsGetSort(DataExpr)), DataExpr);
}

ATermAppl gsMakeDataExprDub(ATermAppl DataExprBit, ATermAppl DataExprNum)
{
  assert(ATisEqual(gsGetSort(DataExprBit), gsMakeSortExprBool()));
  assert(IsNISort(gsGetSort(DataExprNum)));
  return gsMakeDataAppl2(gsMakeOpIdDub(gsGetSort(DataExprNum)),
    DataExprBit, DataExprNum);
}

ATermAppl gsMakeDataExprAdd(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  return gsMakeDataAppl2(
    gsMakeOpIdAdd(gsGetSort(DataExprLHS), gsGetSort(DataExprRHS)),
    DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprAddC(ATermAppl DataExprBit, ATermAppl DataExprLHS,
  ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprBit), gsMakeSortExprBool()));
  assert(ATisEqual(gsGetSort(DataExprLHS), gsMakeSortExprPos()));
  assert(ATisEqual(gsGetSort(DataExprRHS), gsMakeSortExprPos()));
  return gsMakeDataAppl3(gsMakeOpIdAddC(), DataExprBit, DataExprLHS,
    DataExprRHS);
}

ATermAppl gsMakeDataExprSubt(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprLHS), gsGetSort(DataExprRHS)));
  return gsMakeDataAppl2(
    gsMakeOpIdSubt(gsGetSort(DataExprLHS)), DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprGTESubt(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprLHS), gsGetSort(DataExprRHS)));
  return gsMakeDataAppl2(gsMakeOpIdGTESubt(gsGetSort(DataExprLHS)),
    DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprGTESubtB(ATermAppl DataExprBit, ATermAppl DataExprLHS,
  ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprBit), gsMakeSortExprBool()));
  assert(ATisEqual(gsGetSort(DataExprLHS), gsMakeSortExprPos()));
  assert(ATisEqual(gsGetSort(DataExprRHS), gsMakeSortExprPos()));
  return gsMakeDataAppl3(gsMakeOpIdGTESubtB(), DataExprBit, DataExprLHS,
    DataExprRHS);
}

ATermAppl gsMakeDataExprMult(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprLHS), gsGetSort(DataExprRHS)));
  return gsMakeDataAppl2(gsMakeOpIdMult(gsGetSort(DataExprLHS)),
    DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprMultIR(ATermAppl DataExprBit, ATermAppl DataExprIR,
  ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprBit), gsMakeSortExprBool()));
  assert(ATisEqual(gsGetSort(DataExprIR),  gsMakeSortExprPos()));
  assert(ATisEqual(gsGetSort(DataExprLHS), gsMakeSortExprPos()));
  assert(ATisEqual(gsGetSort(DataExprRHS), gsMakeSortExprPos()));
  return gsMakeDataAppl4(gsMakeOpIdMultIR(), DataExprBit, DataExprIR,
    DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprDiv(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprRHS), gsMakeSortExprPos()));
  return gsMakeDataAppl2(gsMakeOpIdDiv(gsGetSort(DataExprLHS)),
    DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprMod(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprRHS), gsMakeSortExprPos()));
  return gsMakeDataAppl2(gsMakeOpIdMod(gsGetSort(DataExprLHS)),
    DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprDivMod(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprLHS), gsMakeSortExprPos()));
  assert(ATisEqual(gsGetSort(DataExprRHS), gsMakeSortExprPos()));
  return gsMakeDataAppl2(gsMakeOpIdDivMod(), DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprGDivMod(ATermAppl DataExprPair, ATermAppl DataExprBool,
  ATermAppl DataExprPos)
{
  assert(ATisEqual(gsGetSort(DataExprPair), gsMakeSortExprNatPair()));
  assert(ATisEqual(gsGetSort(DataExprBool), gsMakeSortExprBool()));
  assert(ATisEqual(gsGetSort(DataExprPos), gsMakeSortExprPos()));
  return gsMakeDataAppl3(gsMakeOpIdGDivMod(), DataExprPair, DataExprBool,
    DataExprPos);
}

ATermAppl gsMakeDataExprGGDivMod(ATermAppl DataExprNat1, ATermAppl DataExprNat2,
  ATermAppl DataExprPos)
{
  assert(ATisEqual(gsGetSort(DataExprNat1), gsMakeSortExprNat()));
  assert(ATisEqual(gsGetSort(DataExprNat2), gsMakeSortExprNat()));
  assert(ATisEqual(gsGetSort(DataExprPos), gsMakeSortExprPos()));
  return gsMakeDataAppl3(gsMakeOpIdGGDivMod(), DataExprNat1, DataExprNat2,
    DataExprPos);
}

ATermAppl gsMakeDataExprFirst(ATermAppl DataExpr)
{
  assert(ATisEqual(gsGetSort(DataExpr), gsMakeSortExprNatPair()));
  return gsMakeDataAppl1(gsMakeOpIdFirst(), DataExpr);
}

ATermAppl gsMakeDataExprLast(ATermAppl DataExpr)
{
  assert(ATisEqual(gsGetSort(DataExpr), gsMakeSortExprNatPair()));
  return gsMakeDataAppl1(gsMakeOpIdLast(), DataExpr);
}

ATermAppl gsMakeDataExprExp(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  ATermAppl SortExpr = gsGetSort(DataExprLHS);
  assert(IsPNIRSort(SortExpr));
  assert(ATisEqual(gsGetSort(DataExprRHS), IsPNISort(SortExpr)?gsMakeSortExprNat():gsMakeSortExprInt()));
  return gsMakeDataAppl2(gsMakeOpIdExp(SortExpr), DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprEven(ATermAppl DataExpr)
{
  assert(ATisEqual(gsGetSort(DataExpr), gsMakeSortExprNat()));
  return gsMakeDataAppl1(gsMakeOpIdEven(), DataExpr);
}

ATermAppl gsMakeDataExprDivide(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprLHS), gsGetSort(DataExprRHS)));
  return gsMakeDataAppl2(gsMakeOpIdDivide(gsGetSort(DataExprLHS)),
    DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprFloor(ATermAppl DataExpr)
{
  assert(ATisEqual(gsGetSort(DataExpr), gsMakeSortExprReal()));
  return gsMakeDataAppl1(gsMakeOpIdFloor(), DataExpr);
}

ATermAppl gsMakeDataExprCeil(ATermAppl DataExpr)
{
  assert(ATisEqual(gsGetSort(DataExpr), gsMakeSortExprReal()));
  return gsMakeDataAppl1(gsMakeOpIdCeil(), DataExpr);
}

ATermAppl gsMakeDataExprRound(ATermAppl DataExpr)
{
  assert(ATisEqual(gsGetSort(DataExpr), gsMakeSortExprReal()));
  return gsMakeDataAppl1(gsMakeOpIdRound(), DataExpr);
}

ATermAppl gsMakeDataExprRedFrac(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprLHS), gsMakeSortExprInt()));
  assert(ATisEqual(gsGetSort(DataExprRHS), gsMakeSortExprInt()));
  return gsMakeDataAppl2(gsMakeOpIdRedFrac(), DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprRedFracWhr(ATermAppl DataExprPos, ATermAppl DataExprInt, ATermAppl DataExprNat)
{
  assert(ATisEqual(gsGetSort(DataExprPos), gsMakeSortExprPos()));
  assert(ATisEqual(gsGetSort(DataExprInt), gsMakeSortExprInt()));
  assert(ATisEqual(gsGetSort(DataExprNat), gsMakeSortExprNat()));
  return gsMakeDataAppl3(gsMakeOpIdRedFracWhr(), DataExprPos, DataExprInt, DataExprNat);
}

ATermAppl gsMakeDataExprRedFracHlp(ATermAppl DataExprReal, ATermAppl DataExprInt)
{
  assert(ATisEqual(gsGetSort(DataExprReal), gsMakeSortExprReal()));
  assert(ATisEqual(gsGetSort(DataExprInt), gsMakeSortExprInt()));
  return gsMakeDataAppl2(gsMakeOpIdRedFracHlp(), DataExprReal, DataExprInt);
}

ATermAppl gsMakeDataExprEmptyList(ATermAppl SortExpr)
{
  return gsMakeOpIdEmptyList(SortExpr);
}

ATermAppl gsMakeDataExprListEnum(ATermList DataExprs, ATermAppl SortExpr)
{
  assert(gsIsSortExpr(SortExpr));
  int nDataExprs = ATgetLength(DataExprs);
  // If the list of data expressions is non-empty build up the sort expression
  if(nDataExprs > 0) {
    ATermAppl EltSort = gsGetSort(ATAgetFirst(DataExprs));
    ATermList DomSort=ATmakeList0();
    for(int i = 0; i < nDataExprs; ++i)
    {
      DomSort=ATinsert(DomSort,(ATerm) EltSort);
    }
    SortExpr=gsMakeSortArrow(DomSort, SortExpr);
    assert(ATisEqual(ATgetLength(DomSort),ATgetLength(DataExprs)));
    return gsMakeDataAppl(gsMakeOpIdListEnum(SortExpr), DataExprs);
  } // If the list of data expressions is empty, we just use SortExpr
  return gsMakeOpIdListEnum(SortExpr);
}

ATermAppl gsMakeDataExprCons(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  return gsMakeDataAppl2(
    gsMakeOpIdCons(gsGetSort(DataExprLHS), gsGetSort(DataExprRHS)),
    DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprListSize(ATermAppl DataExpr)
{
  return gsMakeDataAppl1(gsMakeOpIdListSize(gsGetSort(DataExpr)), DataExpr);
}

ATermAppl gsMakeDataExprSnoc(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  return gsMakeDataAppl2(
    gsMakeOpIdSnoc(gsGetSort(DataExprLHS), gsGetSort(DataExprRHS)),
    DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprConcat(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  return gsMakeDataAppl2(
    gsMakeOpIdConcat(gsGetSort(DataExprLHS)), DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprEltAt(ATermAppl DataExprLHS, ATermAppl DataExprRHS,
  ATermAppl SortExpr)
{
  assert(ATisEqual(gsGetSort(DataExprRHS), gsMakeSortExprNat()));
  return gsMakeDataAppl2(
    gsMakeOpIdEltAt(gsGetSort(DataExprLHS), SortExpr),
    DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprHead(ATermAppl DataExpr, ATermAppl SortExpr)
{
  return gsMakeDataAppl1(
    gsMakeOpIdHead(gsGetSort(DataExpr), SortExpr), DataExpr);
}

ATermAppl gsMakeDataExprTail(ATermAppl DataExpr)
{
  return gsMakeDataAppl1(
    gsMakeOpIdTail(gsGetSort(DataExpr)), DataExpr);
}

ATermAppl gsMakeDataExprRHead(ATermAppl DataExpr, ATermAppl SortExpr)
{
  return gsMakeDataAppl1(
    gsMakeOpIdRHead(gsGetSort(DataExpr), SortExpr), DataExpr);
}

ATermAppl gsMakeDataExprRTail(ATermAppl DataExpr)
{
  return gsMakeDataAppl1(
    gsMakeOpIdRTail(gsGetSort(DataExpr)), DataExpr);
}

ATermAppl gsMakeDataExprEltIn(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  return gsMakeDataAppl2(
    gsMakeOpIdEltIn(gsGetSort(DataExprLHS), gsGetSort(DataExprRHS)),
    DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprSet(ATermAppl DataExprFunc, ATermAppl DataExprFSet, ATermAppl SortExprSet)
{
  ATermAppl SortExprFunc = gsGetSort(DataExprFunc);
  assert(gsIsSortArrow(SortExprFunc));
  ATermAppl SortExprFuncResult = ATAgetArgument(SortExprFunc, 1);
  assert(ATisEqual(SortExprFuncResult, gsMakeSortExprBool()));
  ATermList SortExprFuncDom = ATLgetArgument(SortExprFunc, 0);
  assert(ATgetLength(SortExprFuncDom) == 1);
  return gsMakeDataAppl2(
    gsMakeOpIdSet(ATAgetFirst(SortExprFuncDom), gsGetSort(DataExprFSet), SortExprSet), DataExprFunc, DataExprFSet);
}

ATermAppl gsMakeDataExprSetFSet(ATermAppl DataExprFSet, ATermAppl SortExprSet)
{
  return gsMakeDataAppl1(
    gsMakeOpIdSetFSet(gsGetSort(DataExprFSet), SortExprSet), DataExprFSet);
}

ATermAppl gsMakeDataExprSetComp(ATermAppl DataExprFunc, ATermAppl SortExprSet)
{
  ATermAppl SortExprFunc = gsGetSort(DataExprFunc);
  assert(gsIsSortArrow(SortExprFunc));
  ATermAppl SortExprFuncResult = ATAgetArgument(SortExprFunc, 1);
  assert(ATisEqual(SortExprFuncResult, gsMakeSortExprBool()));
  ATermList SortExprFuncDom = ATLgetArgument(SortExprFunc, 0);
  assert(ATgetLength(SortExprFuncDom) == 1);
  return gsMakeDataAppl1(
    gsMakeOpIdSetComp(ATAgetFirst(SortExprFuncDom), SortExprSet), DataExprFunc);
}

ATermAppl gsMakeDataExprEmptySet(ATermAppl SortExprSet)
{
  return gsMakeOpIdEmptySet(SortExprSet);
}

ATermAppl gsMakeDataExprSetEnum(ATermList DataExprs, ATermAppl SortExpr)
{
  assert(gsIsSortExpr(SortExpr));
  int nDataExprs = ATgetLength(DataExprs);
  // If the list of data expressions is non-empty build up the sort expression
  if(nDataExprs > 0) {
    ATermAppl EltSort = gsGetSort(ATAgetFirst(DataExprs));
    ATermList DomSort=ATmakeList0();
    for(int i = 0; i < nDataExprs; ++i)
    {
      DomSort=ATinsert(DomSort,(ATerm) EltSort);
    }
    SortExpr=gsMakeSortArrow(DomSort, SortExpr);
    assert(ATisEqual(ATgetLength(DomSort),ATgetLength(DataExprs)));
    return gsMakeDataAppl(gsMakeOpIdSetEnum(SortExpr), DataExprs);
  } // If the list of data expressions is empty, we just use SortExpr
  return gsMakeOpIdSetEnum(SortExpr);
}

ATermAppl gsMakeDataExprSetUnion(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprLHS), gsGetSort(DataExprRHS)));
  return gsMakeDataAppl2(
    gsMakeOpIdSetUnion(gsGetSort(DataExprLHS)), DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprSetDiff(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprLHS), gsGetSort(DataExprRHS)));
  return gsMakeDataAppl2(
    gsMakeOpIdSetDiff(gsGetSort(DataExprLHS)), DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprSetInterSect(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprLHS), gsGetSort(DataExprRHS)));
  return gsMakeDataAppl2(
    gsMakeOpIdSetIntersect(gsGetSort(DataExprLHS)), DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprSetCompl(ATermAppl DataExpr)
{
  return gsMakeDataAppl1(gsMakeOpIdSetCompl(gsGetSort(DataExpr)), DataExpr);
}

ATermAppl gsMakeDataExprBag(ATermAppl DataExprFunc, ATermAppl DataExprFBag, ATermAppl SortExprBag)
{
  ATermAppl SortExprFunc = gsGetSort(DataExprFunc);
  assert(gsIsSortArrow(SortExprFunc));
  ATermAppl SortExprFuncResult = ATAgetArgument(SortExprFunc, 1);
  assert(ATisEqual(SortExprFuncResult, gsMakeSortExprNat()));
  ATermList SortExprFuncDom = ATLgetArgument(SortExprFunc, 0);
  assert(ATgetLength(SortExprFuncDom) == 1);
  return gsMakeDataAppl2(
    gsMakeOpIdBag(ATAgetFirst(SortExprFuncDom), gsGetSort(DataExprFBag), SortExprBag), DataExprFunc, DataExprFBag);
}

ATermAppl gsMakeDataExprBagFBag(ATermAppl DataExprFBag, ATermAppl SortExprBag)
{
  return gsMakeDataAppl1(
    gsMakeOpIdBagFBag(gsGetSort(DataExprFBag), SortExprBag), DataExprFBag);
}

ATermAppl gsMakeDataExprBagComp(ATermAppl DataExprFunc, ATermAppl SortExprBag)
{
  ATermAppl SortExprFunc = gsGetSort(DataExprFunc);
  assert(gsIsSortArrow(SortExprFunc));
  ATermAppl SortExprFuncResult = ATAgetArgument(SortExprFunc, 1);
  assert(ATisEqual(SortExprFuncResult, gsMakeSortExprNat()));
  ATermList SortExprFuncDom = ATLgetArgument(SortExprFunc, 0);
  assert(ATgetLength(SortExprFuncDom) == 1);
  return gsMakeDataAppl1(
    gsMakeOpIdBagComp(ATAgetFirst(SortExprFuncDom), SortExprBag), DataExprFunc);
}

ATermAppl gsMakeDataExprEmptyBag(ATermAppl SortExprBag)
{
  return gsMakeOpIdEmptyBag(SortExprBag);
}

ATermAppl gsMakeDataExprBagEnum(ATermList DataExprs, ATermAppl SortExpr)
{
  assert(gsIsSortExpr(SortExpr));
  int nDataExprs = ATgetLength(DataExprs);
  assert(nDataExprs % 2==0);
  nDataExprs = nDataExprs / 2;
  // If the list of data expressions is non-empty build up the sort expression
  if(nDataExprs > 0) {
    ATermAppl EltSort = gsGetSort(ATAgetFirst(DataExprs));
    ATermAppl NatSort = gsMakeSortExprNat();
    ATermList DomSort=ATmakeList0();
    for(int i = 0; i < nDataExprs; ++i)
    {
      DomSort=ATinsert(DomSort,(ATerm) EltSort);
      DomSort=ATinsert(DomSort,(ATerm) NatSort);
    }
    assert(ATgetLength(DomSort)==ATgetLength(DataExprs));
    SortExpr=gsMakeSortArrow(ATreverse(DomSort), SortExpr);
    assert(ATisEqual(ATgetLength(DomSort),ATgetLength(DataExprs)));
    return gsMakeDataAppl(gsMakeOpIdBagEnum(SortExpr), DataExprs);
  } // If the list of data expressions is empty, we just use SortExpr
  return gsMakeOpIdBagEnum(SortExpr);
}

ATermAppl gsMakeDataExprCount(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  return gsMakeDataAppl2(
    gsMakeOpIdCount(gsGetSort(DataExprLHS), gsGetSort(DataExprRHS)),
    DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprBagJoin(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprLHS), gsGetSort(DataExprRHS)));
  return gsMakeDataAppl2(
    gsMakeOpIdBagJoin(gsGetSort(DataExprLHS)), DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprBagDiff(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprLHS), gsGetSort(DataExprRHS)));
  return gsMakeDataAppl2(
    gsMakeOpIdBagDiff(gsGetSort(DataExprLHS)), DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprBagInterSect(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprLHS), gsGetSort(DataExprRHS)));
  return gsMakeDataAppl2(
    gsMakeOpIdBagIntersect(gsGetSort(DataExprLHS)), DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprBag2Set(ATermAppl DataExpr, ATermAppl SortExpr)
{
  return gsMakeDataAppl1(
    gsMakeOpIdBag2Set(gsGetSort(DataExpr), SortExpr), DataExpr);
}

ATermAppl gsMakeDataExprSet2Bag(ATermAppl DataExpr, ATermAppl SortExpr)
{
  return gsMakeDataAppl1(
    gsMakeOpIdSet2Bag(gsGetSort(DataExpr), SortExpr), DataExpr);
}

ATermAppl gsMakeDataExprFSetEmpty(ATermAppl SortExprFSet)
{
  assert(gsIsSortExpr(SortExprFSet));
  return gsMakeOpIdFSetEmpty(SortExprFSet);
}

//ATermAppl gsMakeDataExprFSetCons(ATermAppl DataExprElt, ATermAppl DataExprFSet)
//{
//  return gsMakeDataAppl2(
//    gsMakeOpIdFSetCons(gsGetSort(DataExprElt), gsGetSort(DataExprFSet)),
//      DataExprElt, DataExprFSet);
//}

ATermAppl gsMakeDataExprFSetInsert(ATermAppl DataExprElt, ATermAppl DataExprFSet)
{
  return gsMakeDataAppl2(
    gsMakeOpIdFSetInsert(gsGetSort(DataExprElt), gsGetSort(DataExprFSet)),
      DataExprElt, DataExprFSet);
}

ATermAppl gsMakeDataExprFSetCInsert(ATermAppl DataExprElt, ATermAppl DataExprBool, ATermAppl DataExprFSet)
{
  assert(gsIsSortExprBool(gsGetSort(DataExprBool)));
  return gsMakeDataAppl3(
    gsMakeOpIdFSetCInsert(gsGetSort(DataExprElt), gsGetSort(DataExprFSet)),
      DataExprElt, DataExprBool, DataExprFSet);
}

ATermAppl gsMakeDataExprFSetIn(ATermAppl DataExprElt, ATermAppl DataExprFSet)
{
  return gsMakeDataAppl2(
    gsMakeOpIdFSetIn(gsGetSort(DataExprElt), gsGetSort(DataExprFSet)),
      DataExprElt, DataExprFSet);
}

ATermAppl gsMakeDataExprFSetLTE(ATermAppl DataExprFunc, ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprLHS), gsGetSort(DataExprRHS)));
  ATermAppl SortExprFunc = gsGetSort(DataExprFunc);
  assert(gsIsSortArrow(SortExprFunc));
  assert(gsIsSortExprBool(ATAgetArgument(SortExprFunc, 1)));
  ATermList SortExprFuncDom = ATLgetArgument(SortExprFunc, 0);
  assert(ATgetLength(SortExprFuncDom) == 1);
  ATermAppl SortElt = ATAgetFirst(SortExprFuncDom);
  return gsMakeDataAppl3(
    gsMakeOpIdFSetLTE(SortElt, gsGetSort(DataExprLHS)),
      DataExprFunc, DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprFSetUnion(ATermAppl DataExprFuncLHS, ATermAppl DataExprFuncRHS, ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprFuncLHS), gsGetSort(DataExprFuncRHS)));
  assert(ATisEqual(gsGetSort(DataExprLHS), gsGetSort(DataExprRHS)));
  ATermAppl SortExprFunc = gsGetSort(DataExprFuncLHS);
  assert(gsIsSortArrow(SortExprFunc));
  assert(gsIsSortExprBool(ATAgetArgument(SortExprFunc, 1)));
  ATermList SortExprFuncDom = ATLgetArgument(SortExprFunc, 0);
  assert(ATgetLength(SortExprFuncDom) == 1);
  ATermAppl SortElt = ATAgetFirst(SortExprFuncDom);
  return gsMakeDataAppl4(
    gsMakeOpIdFSetUnion(SortElt, gsGetSort(DataExprLHS)),
      DataExprFuncLHS, DataExprFuncRHS, DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprFSetInter(ATermAppl DataExprFuncLHS, ATermAppl DataExprFuncRHS, ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprFuncLHS), gsGetSort(DataExprFuncRHS)));
  assert(ATisEqual(gsGetSort(DataExprLHS), gsGetSort(DataExprRHS)));
  ATermAppl SortExprFunc = gsGetSort(DataExprFuncLHS);
  assert(gsIsSortArrow(SortExprFunc));
  assert(gsIsSortExprBool(ATAgetArgument(SortExprFunc, 1)));
  ATermList SortExprFuncDom = ATLgetArgument(SortExprFunc, 0);
  assert(ATgetLength(SortExprFuncDom) == 1);
  ATermAppl SortElt = ATAgetFirst(SortExprFuncDom);
  return gsMakeDataAppl4(
    gsMakeOpIdFSetInter(SortElt, gsGetSort(DataExprLHS)),
      DataExprFuncLHS, DataExprFuncRHS, DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprFBagEmpty(ATermAppl SortExprFBag)
{
  return gsMakeOpIdFBagEmpty(SortExprFBag);
}

//ATermAppl gsMakeDataExprFBagCons(ATermAppl DataExprElt, ATermAppl DataExprPos, ATermAppl DataExprFBag)
//{
//  assert(gsIsSortExprPos(gsGetSort(DataExprPos)));
//  return gsMakeDataAppl3(
//    gsMakeOpIdFBagCons(gsGetSort(DataExprElt), gsGetSort(DataExprFBag)),
//      DataExprElt, DataExprPos, DataExprFBag);
//}

ATermAppl gsMakeDataExprFBagInsert(ATermAppl DataExprElt, ATermAppl DataExprPos, ATermAppl DataExprFBag)
{
  assert(gsIsSortExprPos(gsGetSort(DataExprPos)));
  return gsMakeDataAppl3(
    gsMakeOpIdFBagInsert(gsGetSort(DataExprElt), gsGetSort(DataExprFBag)),
      DataExprElt, DataExprPos, DataExprFBag);
}

ATermAppl gsMakeDataExprFBagCInsert(ATermAppl DataExprElt, ATermAppl DataExprNat, ATermAppl DataExprFBag)
{
  assert(gsIsSortExprNat(gsGetSort(DataExprNat)));
  return gsMakeDataAppl3(
    gsMakeOpIdFBagCInsert(gsGetSort(DataExprElt), gsGetSort(DataExprFBag)),
      DataExprElt, DataExprNat, DataExprFBag);
}

ATermAppl gsMakeDataExprFBagCount(ATermAppl DataExprElt, ATermAppl DataExprFBag)
{
  return gsMakeDataAppl2(
    gsMakeOpIdFBagCount(gsGetSort(DataExprElt), gsGetSort(DataExprFBag)),
      DataExprElt, DataExprFBag);
}

ATermAppl gsMakeDataExprFBagIn(ATermAppl DataExprElt, ATermAppl DataExprFBag)
{
  return gsMakeDataAppl2(
    gsMakeOpIdFBagIn(gsGetSort(DataExprElt), gsGetSort(DataExprFBag)),
      DataExprElt, DataExprFBag);
}

ATermAppl gsMakeDataExprFBagLTE(ATermAppl DataExprFunc, ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprLHS), gsGetSort(DataExprRHS)));
  ATermAppl SortExprFunc = gsGetSort(DataExprFunc);
  assert(gsIsSortArrow(SortExprFunc));
  assert(gsIsSortExprNat(ATAgetArgument(SortExprFunc, 1)));
  ATermList SortExprFuncDom = ATLgetArgument(SortExprFunc, 0);
  assert(ATgetLength(SortExprFuncDom) == 1);
  ATermAppl SortElt = ATAgetFirst(SortExprFuncDom);
  return gsMakeDataAppl3(
    gsMakeOpIdFBagLTE(SortElt, gsGetSort(DataExprLHS)),
      DataExprFunc, DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprFBagJoin(ATermAppl DataExprFuncLHS, ATermAppl DataExprFuncRHS, ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprFuncLHS), gsGetSort(DataExprFuncRHS)));
  assert(ATisEqual(gsGetSort(DataExprLHS), gsGetSort(DataExprRHS)));
  ATermAppl SortExprFunc = gsGetSort(DataExprFuncLHS);
  assert(gsIsSortArrow(SortExprFunc));
  assert(gsIsSortExprNat(ATAgetArgument(SortExprFunc, 1)));
  ATermList SortExprFuncDom = ATLgetArgument(SortExprFunc, 0);
  assert(ATgetLength(SortExprFuncDom) == 1);
  ATermAppl SortElt = ATAgetFirst(SortExprFuncDom);
  return gsMakeDataAppl4(
    gsMakeOpIdFBagJoin(SortElt, gsGetSort(DataExprLHS)),
      DataExprFuncLHS, DataExprFuncRHS, DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprFBagInter(ATermAppl DataExprFuncLHS, ATermAppl DataExprFuncRHS, ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprFuncLHS), gsGetSort(DataExprFuncRHS)));
  assert(ATisEqual(gsGetSort(DataExprLHS), gsGetSort(DataExprRHS)));
  ATermAppl SortExprFunc = gsGetSort(DataExprFuncLHS);
  assert(gsIsSortArrow(SortExprFunc));
  assert(gsIsSortExprNat(ATAgetArgument(SortExprFunc, 1)));
  ATermList SortExprFuncDom = ATLgetArgument(SortExprFunc, 0);
  assert(ATgetLength(SortExprFuncDom) == 1);
  ATermAppl SortElt = ATAgetFirst(SortExprFuncDom);
  return gsMakeDataAppl4(
    gsMakeOpIdFBagInter(SortElt, gsGetSort(DataExprLHS)),
      DataExprFuncLHS, DataExprFuncRHS, DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprFBagDiff(ATermAppl DataExprFuncLHS, ATermAppl DataExprFuncRHS, ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprFuncLHS), gsGetSort(DataExprFuncRHS)));
  assert(ATisEqual(gsGetSort(DataExprLHS), gsGetSort(DataExprRHS)));
  ATermAppl SortExprFunc = gsGetSort(DataExprFuncLHS);
  assert(gsIsSortArrow(SortExprFunc));
  assert(gsIsSortExprNat(ATAgetArgument(SortExprFunc, 1)));
  ATermList SortExprFuncDom = ATLgetArgument(SortExprFunc, 0);
  assert(ATgetLength(SortExprFuncDom) == 1);
  ATermAppl SortElt = ATAgetFirst(SortExprFuncDom);
  return gsMakeDataAppl4(
    gsMakeOpIdFBagDiff(SortElt, gsGetSort(DataExprLHS)),
      DataExprFuncLHS, DataExprFuncRHS, DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprFBag2FSet(ATermAppl DataExprFunc, ATermAppl DataExprFBag, ATermAppl SortExprFSet)
{
  ATermAppl SortExprFunc = gsGetSort(DataExprFunc);
  assert(gsIsSortArrow(SortExprFunc));
  assert(gsIsSortExprNat(ATAgetArgument(SortExprFunc, 1)));
  ATermList SortExprFuncDom = ATLgetArgument(SortExprFunc, 0);
  assert(ATgetLength(SortExprFuncDom) == 1);
  ATermAppl SortElt = ATAgetFirst(SortExprFuncDom);
  return gsMakeDataAppl2(
    gsMakeOpIdFBag2FSet(SortElt, gsGetSort(DataExprFBag), SortExprFSet),
      DataExprFunc, DataExprFBag);
}

ATermAppl gsMakeDataExprFSet2FBag(ATermAppl DataExprFSet, ATermAppl SortExprFBag)
{
  return gsMakeDataAppl1(
    gsMakeOpIdFSet2FBag(gsGetSort(DataExprFSet), SortExprFBag),
      DataExprFSet);
}

ATermAppl gsMakeDataExprMonus(ATermAppl DataExprArg1, ATermAppl DataExprArg2)
{
  assert(gsIsSortExprNat(gsGetSort(DataExprArg1)));
  assert(gsIsSortExprNat(gsGetSort(DataExprArg2)));
  return gsMakeDataAppl2(gsMakeOpIdMonus(), DataExprArg1, DataExprArg2);
}

ATermAppl gsMakeDataExprSwapZero(ATermAppl DataExprArg1, ATermAppl DataExprArg2)
{
  assert(gsIsSortExprNat(gsGetSort(DataExprArg1)));
  assert(gsIsSortExprNat(gsGetSort(DataExprArg2)));
  return gsMakeDataAppl2(gsMakeOpIdSwapZero(), DataExprArg1, DataExprArg2);
}

ATermAppl gsMakeDataExprSwapZeroAdd(ATermAppl DataExprArg1, ATermAppl DataExprArg2, ATermAppl DataExprArg3, ATermAppl DataExprArg4)
{
  assert(gsIsSortExprNat(gsGetSort(DataExprArg1)));
  assert(gsIsSortExprNat(gsGetSort(DataExprArg2)));
  assert(gsIsSortExprNat(gsGetSort(DataExprArg3)));
  assert(gsIsSortExprNat(gsGetSort(DataExprArg4)));
  return gsMakeDataAppl4(gsMakeOpIdSwapZeroAdd(), DataExprArg1, DataExprArg2, DataExprArg3, DataExprArg4);
}

ATermAppl gsMakeDataExprSwapZeroMin(ATermAppl DataExprArg1, ATermAppl DataExprArg2, ATermAppl DataExprArg3, ATermAppl DataExprArg4)
{
  assert(gsIsSortExprNat(gsGetSort(DataExprArg1)));
  assert(gsIsSortExprNat(gsGetSort(DataExprArg2)));
  assert(gsIsSortExprNat(gsGetSort(DataExprArg3)));
  assert(gsIsSortExprNat(gsGetSort(DataExprArg4)));
  return gsMakeDataAppl4(gsMakeOpIdSwapZeroMin(), DataExprArg1, DataExprArg2, DataExprArg3, DataExprArg4);
}

ATermAppl gsMakeDataExprSwapZeroMonus(ATermAppl DataExprArg1, ATermAppl DataExprArg2, ATermAppl DataExprArg3, ATermAppl DataExprArg4)
{
  assert(gsIsSortExprNat(gsGetSort(DataExprArg1)));
  assert(gsIsSortExprNat(gsGetSort(DataExprArg2)));
  assert(gsIsSortExprNat(gsGetSort(DataExprArg3)));
  assert(gsIsSortExprNat(gsGetSort(DataExprArg4)));
  return gsMakeDataAppl4(gsMakeOpIdSwapZeroMonus(), DataExprArg1, DataExprArg2, DataExprArg3, DataExprArg4);
}

ATermAppl gsMakeDataExprSwapZeroLTE(ATermAppl DataExprArg1, ATermAppl DataExprArg2, ATermAppl DataExprArg3)
{
  assert(gsIsSortExprNat(gsGetSort(DataExprArg1)));
  assert(gsIsSortExprNat(gsGetSort(DataExprArg2)));
  assert(gsIsSortExprNat(gsGetSort(DataExprArg3)));
  return gsMakeDataAppl3(gsMakeOpIdSwapZeroLTE(), DataExprArg1, DataExprArg2, DataExprArg3);
}

ATermAppl gsMakeDataExprFalseFunc(ATermAppl SortExprElt)
{
  assert(gsIsSortExpr(SortExprElt));
  return gsMakeOpIdFalseFunc(SortExprElt);
}

ATermAppl gsMakeDataExprTrueFunc(ATermAppl SortExprElt)
{
  assert(gsIsSortExpr(SortExprElt));
  return gsMakeOpIdTrueFunc(SortExprElt);
}

ATermAppl gsMakeDataExprNotFunc(ATermAppl DataExprArg)
{
  ATermAppl SortExprArg = gsGetSort(DataExprArg);
  assert(gsIsSortArrow(SortExprArg));
  assert(gsIsSortExprBool(ATAgetArgument(SortExprArg, 1)));
  ATermList SortExprDom = ATLgetArgument(SortExprArg, 0);
  assert(ATgetLength(SortExprDom) == 1);
  return gsMakeDataAppl1(
    gsMakeOpIdNotFunc(ATAgetFirst(SortExprDom)), DataExprArg);
}

ATermAppl gsMakeDataExprAndFunc(ATermAppl DataExprArg1, ATermAppl DataExprArg2)
{
  ATermAppl SortExprArg = gsGetSort(DataExprArg1);
  assert(ATisEqual(SortExprArg, gsGetSort(DataExprArg2)));
  assert(gsIsSortArrow(SortExprArg));
  assert(gsIsSortExprBool(ATAgetArgument(SortExprArg, 1)));
  ATermList SortExprDom = ATLgetArgument(SortExprArg, 0);
  assert(ATgetLength(SortExprDom) == 1);
  return gsMakeDataAppl2(
    gsMakeOpIdAndFunc(ATAgetFirst(SortExprDom)), DataExprArg1, DataExprArg2);
}

ATermAppl gsMakeDataExprOrFunc(ATermAppl DataExprArg1, ATermAppl DataExprArg2)
{
  ATermAppl SortExprArg = gsGetSort(DataExprArg1);
  assert(ATisEqual(SortExprArg, gsGetSort(DataExprArg2)));
  assert(gsIsSortArrow(SortExprArg));
  assert(gsIsSortExprBool(ATAgetArgument(SortExprArg, 1)));
  ATermList SortExprDom = ATLgetArgument(SortExprArg, 0);
  assert(ATgetLength(SortExprDom) == 1);
  return gsMakeDataAppl2(
    gsMakeOpIdOrFunc(ATAgetFirst(SortExprDom)), DataExprArg1, DataExprArg2);
}

ATermAppl gsMakeDataExprZeroFunc(ATermAppl SortExprElt)
{
  assert(gsIsSortExpr(SortExprElt));
  return gsMakeOpIdZeroFunc(SortExprElt);
}

ATermAppl gsMakeDataExprOneFunc(ATermAppl SortExprElt)
{
  assert(gsIsSortExpr(SortExprElt));
  return gsMakeOpIdOneFunc(SortExprElt);
}

ATermAppl gsMakeDataExprAddFunc(ATermAppl DataExprArg1, ATermAppl DataExprArg2)
{
  ATermAppl SortExprArg = gsGetSort(DataExprArg1);
  assert(ATisEqual(SortExprArg, gsGetSort(DataExprArg2)));
  assert(gsIsSortArrow(SortExprArg));
  assert(gsIsSortExprNat(ATAgetArgument(SortExprArg, 1)));
  ATermList SortExprDom = ATLgetArgument(SortExprArg, 0);
  assert(ATgetLength(SortExprDom) == 1);
  return gsMakeDataAppl2(
    gsMakeOpIdAddFunc(ATAgetFirst(SortExprDom)), DataExprArg1, DataExprArg2);
}

ATermAppl gsMakeDataExprMinFunc(ATermAppl DataExprArg1, ATermAppl DataExprArg2)
{
  ATermAppl SortExprArg = gsGetSort(DataExprArg1);
  assert(ATisEqual(SortExprArg, gsGetSort(DataExprArg2)));
  assert(gsIsSortArrow(SortExprArg));
  assert(gsIsSortExprNat(ATAgetArgument(SortExprArg, 1)));
  ATermList SortExprDom = ATLgetArgument(SortExprArg, 0);
  assert(ATgetLength(SortExprDom) == 1);
  return gsMakeDataAppl2(
    gsMakeOpIdMinFunc(ATAgetFirst(SortExprDom)), DataExprArg1, DataExprArg2);
}

ATermAppl gsMakeDataExprMonusFunc(ATermAppl DataExprArg1, ATermAppl DataExprArg2)
{
  ATermAppl SortExprArg = gsGetSort(DataExprArg1);
  assert(ATisEqual(SortExprArg, gsGetSort(DataExprArg2)));
  assert(gsIsSortArrow(SortExprArg));
  assert(gsIsSortExprNat(ATAgetArgument(SortExprArg, 1)));
  ATermList SortExprDom = ATLgetArgument(SortExprArg, 0);
  assert(ATgetLength(SortExprDom) == 1);
  return gsMakeDataAppl2(
    gsMakeOpIdMonusFunc(ATAgetFirst(SortExprDom)), DataExprArg1, DataExprArg2);
}

ATermAppl gsMakeDataExprNat2BoolFunc(ATermAppl DataExprArg)
{
  ATermAppl SortExprArg = gsGetSort(DataExprArg);
  assert(gsIsSortArrow(SortExprArg));
  assert(gsIsSortExprNat(ATAgetArgument(SortExprArg, 1)));
  ATermList SortExprDom = ATLgetArgument(SortExprArg, 0);
  assert(ATgetLength(SortExprDom) == 1);
  return gsMakeDataAppl1(
    gsMakeOpIdNat2BoolFunc(ATAgetFirst(SortExprDom)), DataExprArg);
}

ATermAppl gsMakeDataExprBool2NatFunc(ATermAppl DataExprArg)
{
  ATermAppl SortExprArg = gsGetSort(DataExprArg);
  assert(gsIsSortArrow(SortExprArg));
  assert(gsIsSortExprBool(ATAgetArgument(SortExprArg, 1)));
  ATermList SortExprDom = ATLgetArgument(SortExprArg, 0);
  assert(ATgetLength(SortExprDom) == 1);
  return gsMakeDataAppl1(
    gsMakeOpIdBool2NatFunc(ATAgetFirst(SortExprDom)), DataExprArg);
}

ATermAppl gsMakeDataExprBagElt(ATermAppl DataExprElt, ATermAppl DataExprPos, ATermAppl SortExprFBagElt)
{
  assert(gsIsSortExprPos(gsGetSort(DataExprPos)));
  return gsMakeDataAppl2(
    gsMakeOpIdBagElt(gsGetSort(DataExprElt), SortExprFBagElt), DataExprElt, DataExprPos);
}

//Auxiliary functions to create data expressions
ATermAppl gsMakeDataAppl1(ATermAppl DataExpr, ATermAppl DataExprArg1)
{
  return gsMakeDataAppl(DataExpr, ATmakeList1((ATerm) DataExprArg1));
}

ATermAppl gsMakeDataAppl2(ATermAppl DataExpr, ATermAppl DataExprArg1,
  ATermAppl DataExprArg2)
{
  return gsMakeDataAppl(DataExpr,
    ATmakeList2((ATerm) DataExprArg1, (ATerm) DataExprArg2));
}

ATermAppl gsMakeDataAppl3(ATermAppl DataExpr, ATermAppl DataExprArg1,
  ATermAppl DataExprArg2, ATermAppl DataExprArg3)
{
  return gsMakeDataAppl(DataExpr,
    ATmakeList3((ATerm) DataExprArg1, (ATerm) DataExprArg2,
      (ATerm) DataExprArg3));
}

ATermAppl gsMakeDataAppl4(ATermAppl DataExpr, ATermAppl DataExprArg1,
  ATermAppl DataExprArg2, ATermAppl DataExprArg3, ATermAppl DataExprArg4)
{
  return gsMakeDataAppl(DataExpr,
    ATmakeList4((ATerm) DataExprArg1, (ATerm) DataExprArg2,
      (ATerm) DataExprArg3, (ATerm) DataExprArg4));
}

ATermAppl gsMakeDataApplList(ATermAppl DataExpr,
  ATermList DataExprArgs)
{
  if (ATisEmpty(DataExprArgs))
    return DataExpr;
  else
    return gsMakeDataAppl(DataExpr, DataExprArgs);
}

ATermAppl gsMakeDataExprAndList(ATermList DataExprs)
{
  if (ATisEmpty(DataExprs))
    return gsMakeDataExprTrue();
  ATermAppl Result = ATAgetFirst(DataExprs);
  DataExprs = ATgetNext(DataExprs);
  if (ATisEmpty(DataExprs)) {
    return Result;
  }
  for ( ; !ATisEmpty(DataExprs) ; DataExprs = ATgetNext(DataExprs) ) {
    Result = gsMakeDataExprAnd(Result, ATAgetFirst(DataExprs));
  }
  return Result;
}

ATermAppl gsMakeDataExprBool_bool(bool b)
{
  if (b) {
    return gsMakeDataExprTrue();
  } else {
    return gsMakeDataExprFalse();
  }
}

ATermAppl gsMakeDataExprPos(char *p)
{
  assert(strlen(p) > 0);
  if (!strcmp(p, "1")) {
    return gsMakeDataExprC1();
  } else {
    char *d = gsStringDiv2(p);
    ATermAppl result = NULL;
    if (gsStringMod2(p) == 0) {
      result = gsMakeDataExprCDub(gsMakeDataExprFalse(), gsMakeDataExprPos(d));
    } else {
      result = gsMakeDataExprCDub(gsMakeDataExprTrue(), gsMakeDataExprPos(d));
    }
    free(d);
    return result;
  }
}

ATermAppl gsMakeDataExprPos_int(int p)
{
  assert(p > 0);
  DECL_A(s,char,NrOfChars(p)+1);
  sprintf(s, "%d", p);
  ATermAppl a = gsMakeDataExprPos(s);
  FREE_A(s);
  return a;
}

ATermAppl gsMakeDataExprNat(char *n)
{
  if (!strcmp(n, "0")) {
    return gsMakeDataExprC0();
  } else {
    return gsMakeDataExprCNat(gsMakeDataExprPos(n));
  }
}

ATermAppl gsMakeDataExprNat_int(int n)
{
  assert(n >= 0);
  DECL_A(s,char,NrOfChars(n)+1);
  sprintf(s, "%d", n);
  ATermAppl a = gsMakeDataExprNat(s);
  FREE_A(s);
  return a;
}

ATermAppl gsMakeDataExprInt(char *z)
{
  if (!strncmp(z, "-", 1)) {
    return gsMakeDataExprCNeg(gsMakeDataExprPos(z+1));
  } else {
    return gsMakeDataExprCInt(gsMakeDataExprNat(z));
  }
}

ATermAppl gsMakeDataExprInt_int(int z)
{
  DECL_A(s,char,NrOfChars(z)+1);
  sprintf(s, "%d", z);
  ATermAppl a = gsMakeDataExprInt(s);
  FREE_A(s);
  return a;
}

ATermAppl gsMakeDataExprReal(char *z)
{
  return gsMakeDataExprCReal(gsMakeDataExprInt(z), gsMakeDataExprC1());
}

ATermAppl gsMakeDataExprReal_int(int z)
{
  DECL_A(s,char,NrOfChars(z)+1);
  sprintf(s, "%d", z);
  ATermAppl a = gsMakeDataExprReal(s);
  FREE_A(s);
  return a;
}

bool gsIsPosConstant(const ATermAppl PosExpr)
{
  if (gsIsOpId(PosExpr)) {
    return ATisEqual(PosExpr, gsMakeOpIdC1());
  } else if (gsIsDataAppl(PosExpr))  {
    ATermAppl Head = gsGetDataExprHead(PosExpr);
    ATermList Args = gsGetDataExprArgs(PosExpr);
    if (ATisEqual(Head, gsMakeOpIdCDub()) && ATgetLength(Args) == 2) {
      ATermAppl ArgBool = ATAelementAt(Args, 0);
      return
        (ATisEqual(ArgBool, gsMakeOpIdTrue()) ||
         ATisEqual(ArgBool, gsMakeOpIdFalse())
        ) && gsIsPosConstant(ATAelementAt(Args, 1));
    } else return false;
  } else return false;
}

bool gsIsNatConstant(const ATermAppl NatExpr)
{
  if (gsIsOpId(NatExpr)) {
    return ATisEqual(NatExpr, gsMakeOpIdC0());
  } else if (gsIsDataAppl(NatExpr)) {
    ATermAppl Head = gsGetDataExprHead(NatExpr);
    ATermList Args = gsGetDataExprArgs(NatExpr);
    if (ATisEqual(Head, gsMakeOpIdCNat()) && ATgetLength(Args) == 1) {
      return gsIsPosConstant(ATAelementAt(Args, 0));
    } else return false;
  } else return false;
}

bool gsIsIntConstant(const ATermAppl IntExpr)
{
  if (gsIsDataAppl(IntExpr)) {
    ATermAppl Head = gsGetDataExprHead(IntExpr);
    ATermList Args = gsGetDataExprArgs(IntExpr);
    if (ATgetLength(Args) == 1) {
      if (ATisEqual(Head, gsMakeOpIdCInt())) {
        return gsIsNatConstant(ATAelementAt(Args, 0));
      } else if (ATisEqual(Head, gsMakeOpIdCNeg())) {
        return gsIsPosConstant(ATAelementAt(Args, 0));
      } else return false;
    } else return false;
  } else return false;
}

char *gsPosValue(const ATermAppl PosConstant)
{
  assert(gsIsPosConstant(PosConstant));
  char *Result = 0;
  if (gsIsOpId(PosConstant)) {
    //PosConstant is 1
    Result = (char *) malloc(2 * sizeof(char));
    Result = strcpy(Result, "1");
  } else {
    //PosConstant is of the form cDub(b)(p), where b and p are boolean and
    //positive constants, respectively
    ATermList Args = gsGetDataExprArgs(PosConstant);
    int Inc = (ATisEqual(ATAelementAt(Args, 0), gsMakeDataExprTrue()))?1:0;
    char *PosValue = gsPosValue(ATAelementAt(Args, 1));
    Result = gsStringDub(PosValue, Inc);
    free(PosValue);
  }
  return Result;
}

int gsPosValue_int(const ATermAppl PosConstant)
{
  char *s = gsPosValue(PosConstant);
  int n = strtol(s, NULL, 10);
  free(s);
  return n;
}

char *gsNatValue(const ATermAppl NatConstant)
{
  assert(gsIsNatConstant(NatConstant));
  char *Result = 0;
  if (gsIsOpId(NatConstant)) {
    //NatConstant is 0
    Result = (char *) malloc(2 * sizeof(char));
    Result = strcpy(Result, "0");
  } else {
    //NatConstant is a positive constant
    Result = gsPosValue(ATAgetArgument(NatConstant, 1));
  }
  return Result;
}

int gsNatValue_int(const ATermAppl NatConstant)
{
  char *s = gsNatValue(NatConstant);
  int n = strtol(s, NULL, 10);
  free(s);
  return n;
}

char *gsIntValue(const ATermAppl IntConstant)
{
  assert(gsIsIntConstant(IntConstant));
  char *Result = 0;
  if (ATisEqual(ATAgetArgument(IntConstant, 0), gsMakeOpIdCInt())) {
    //IntExpr is a natural number
    Result = gsNatValue(ATAgetArgument(IntConstant, 1));
  } else {
    //IntExpr is the negation of a positive number
    char *PosValue = gsPosValue(ATAgetArgument(IntConstant, 1));
    Result = (char *) malloc((strlen(PosValue)+2) * sizeof(char));
    Result = strcpy(Result, "-");
    Result = strcat(Result, PosValue);
    free(PosValue);
  }
  return Result;
}

int gsIntValue_int(const ATermAppl IntConstant)
{
  char *s = gsIntValue(IntConstant);
  int n = strtol(s, NULL, 10);
  free(s);
  return n;
}

// Recognisers for OpId's
bool gsIsOpIdEq(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      return ATisEqual(DataExpr, gsMakeOpIdEq(ATAgetFirst(domain)));
    }
  }
  return false;
}

bool gsIsOpIdNeq(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      return ATisEqual(DataExpr, gsMakeOpIdNeq(ATAgetFirst(domain)));
    }
  }
  return false;
}

bool gsIsOpIdIf(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      if(ATgetLength(domain) == 3)
      {
        return ATisEqual(DataExpr, gsMakeOpIdIf(ATAgetFirst(ATgetNext(domain))));
      }
    }
  }
  return false;
}

bool gsIsOpIdForall(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      return ATisEqual(DataExpr, gsMakeOpIdForall(ATAgetFirst(domain)));
    }
  }
  return false;
}

bool gsIsOpIdExists(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      return ATisEqual(DataExpr, gsMakeOpIdExists(ATAgetFirst(domain)));
    }
  }
  return false;
}

bool gsIsOpIdLTE(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      return ATisEqual(DataExpr, gsMakeOpIdLTE(ATAgetFirst(domain)));
    }
  }
  return false;
}

bool gsIsOpIdLT(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      return ATisEqual(DataExpr, gsMakeOpIdLT(ATAgetFirst(domain)));
    }
  }
  return false;
}

bool gsIsOpIdGTE(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      return ATisEqual(DataExpr, gsMakeOpIdGTE(ATAgetFirst(domain)));
    }
  }
  return false;
}

bool gsIsOpIdGT(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      return ATisEqual(DataExpr, gsMakeOpIdGT(ATAgetFirst(domain)));
    }
  }
  return false;
}

bool gsIsOpIdMax(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      if(ATgetLength(domain) == 2)
      {
        ATermAppl arg0 = ATAgetFirst(domain);
        ATermAppl arg1 = ATAgetFirst(ATgetNext(domain));
        return IsPNIRSort(arg0) &&
               IsPNIRSort(arg1) &&
               (ATisEqual(arg0, gsMakeSortExprReal()) == ATisEqual(arg1, gsMakeSortExprReal())) &&
               ATisEqual(DataExpr, gsMakeOpIdMax(arg0, arg1));
      }
    }
  }
  return false;
}

bool gsIsOpIdMin(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      return IsPNIRSort(ATAgetFirst(domain)) &&
             ATisEqual(DataExpr, gsMakeOpIdMin(ATAgetFirst(domain)));
    }
  }
  return false;
}

bool gsIsOpIdAbs(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      return IsPNIRSort(ATAgetFirst(domain)) &&
             ATisEqual(DataExpr, gsMakeOpIdAbs(ATAgetFirst(domain)));
    }
  }
  return false;
}

bool gsIsOpIdNeg(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      return IsPNIRSort(ATAgetFirst(domain)) &&
             ATisEqual(DataExpr, gsMakeOpIdNeg(ATAgetFirst(domain)));
    }
  }
  return false;
}

bool gsIsOpIdSucc(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      return IsPNIRSort(ATAgetFirst(domain)) &&
             ATisEqual(DataExpr, gsMakeOpIdSucc(ATAgetFirst(domain)));
    }
  }
  return false;
}

bool gsIsOpIdPred(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      return IsPNIRSort(ATAgetFirst(domain)) &&
             ATisEqual(DataExpr, gsMakeOpIdPred(ATAgetFirst(domain)));
    }
  }
  return false;
}

bool gsIsOpIdDub(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      ATermAppl arg1 = ATAgetFirst(ATgetNext(domain));
      return ATgetLength(domain) == 3 &&
             IsNISort(arg1) &&
             ATisEqual(DataExpr, gsMakeOpIdDub(arg1));
    }
  }
  return false;
}

bool gsIsOpIdAdd(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      if(ATgetLength(domain) == 2)
      {
        ATermAppl arg0 = ATAgetFirst(domain);
        ATermAppl arg1 = ATAgetFirst(ATgetNext(domain));
        return IsPNIRSort(arg0) &&
               IsPNIRSort(arg1) &&
               (ATisEqual(arg0, gsMakeSortExprInt()) == ATisEqual(arg1, gsMakeSortExprInt())) &&
               (ATisEqual(arg0, gsMakeSortExprReal()) == ATisEqual(arg1, gsMakeSortExprReal())) &&
               ATisEqual(DataExpr, gsMakeOpIdAdd(arg0, arg1));
      }
    }
  }
  return false;
}

bool gsIsOpIdSubt(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      return IsPNIRSort(ATAgetFirst(domain)) &&
             ATisEqual(DataExpr, gsMakeOpIdSubt(ATAgetFirst(domain)));
    }
  }
  return false;
}

bool gsIsOpIdGTESubt(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      return IsPNSort(ATAgetFirst(domain)) &&
             ATisEqual(DataExpr, gsMakeOpIdGTESubt(ATAgetFirst(domain)));
    }
  }
  return false;
}

bool gsIsOpIdMult(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      return IsPNIRSort(ATAgetFirst(domain)) &&
             ATisEqual(DataExpr, gsMakeOpIdMult(ATAgetFirst(domain)));
    }
  }
  return false;
}

bool gsIsOpIdDiv(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      return IsPNISort(ATAgetFirst(domain)) &&
             ATisEqual(DataExpr, gsMakeOpIdDiv(ATAgetFirst(domain)));
    }
  }
  return false;
}

bool gsIsOpIdMod(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      return IsPNISort(ATAgetFirst(domain)) &&
             ATisEqual(DataExpr, gsMakeOpIdMod(ATAgetFirst(domain)));
    }
  }
  return false;
}

bool gsIsOpIdExp(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      return IsPNIRSort(ATAgetFirst(domain)) &&
             ATisEqual(DataExpr, gsMakeOpIdExp(ATAgetFirst(domain)));
    }
  }
  return false;
}

bool gsIsOpIdDivide(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      return IsPNIRSort(ATAgetFirst(domain)) &&
             ATisEqual(DataExpr, gsMakeOpIdDivide(ATAgetFirst(domain)));
    }
  }
  return false;
}

bool gsIsOpIdEmptyList(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortId(s))
    {
      return ATisEqual(DataExpr, gsMakeOpIdEmptyList(s));
    }
  }
  return false;
}

bool gsIsOpIdListEnum(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    return ATisEqual(DataExpr, gsMakeOpIdListEnum(s));
  }
  return false;
}

bool gsIsOpIdListSize(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      return ATisEqual(DataExpr, gsMakeOpIdListSize(ATAgetFirst(domain)));
    }
  }
  return false;
}

bool gsIsOpIdCons(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      if(ATgetLength(domain) == 2)
      {
        ATermAppl arg0 = ATAgetFirst(domain);
        ATermAppl arg1 = ATAgetFirst(ATgetNext(domain));
        return ATisEqual(DataExpr, gsMakeOpIdCons(arg0, arg1));
      }
    }
  }
  return false;
}

bool gsIsOpIdSnoc(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      if(ATgetLength(domain) == 2)
      {
        ATermAppl arg0 = ATAgetFirst(domain);
        ATermAppl arg1 = ATAgetFirst(ATgetNext(domain));
        return ATisEqual(DataExpr, gsMakeOpIdSnoc(arg0, arg1));
      }
    }
  }
  return false;
}

bool gsIsOpIdConcat(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      return ATisEqual(DataExpr, gsMakeOpIdConcat(ATAgetFirst(domain)));
    }
  }
  return false;
}

bool gsIsOpIdEltAt(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      ATermAppl codomain = ATAgetArgument(s, 1);
      return ATisEqual(DataExpr, gsMakeOpIdEltAt(ATAgetFirst(domain), codomain));
    }
  }
  return false;
}

bool gsIsOpIdHead(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      ATermAppl codomain = ATAgetArgument(s, 1);
      return ATisEqual(DataExpr, gsMakeOpIdHead(ATAgetFirst(domain), codomain));
    }
  }
  return false;
}

bool gsIsOpIdTail(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      return ATisEqual(DataExpr, gsMakeOpIdTail(ATAgetFirst(domain)));
    }
  }
  return false;
}

bool gsIsOpIdRHead(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      ATermAppl codomain = ATAgetArgument(s, 1);
      return ATisEqual(DataExpr, gsMakeOpIdRHead(ATAgetFirst(domain), codomain));
    }
  }
  return false;
}

bool gsIsOpIdRTail(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      return ATisEqual(DataExpr, gsMakeOpIdRTail(ATAgetFirst(domain)));
    }
  }
  return false;
}

bool gsIsOpIdSet(ATermAppl DataExpr)
{
  if (gsIsOpId(DataExpr))
  {
    ATermAppl se = ATAgetArgument(DataExpr, 1);
    if (gsIsSortArrow(se))
    {
      ATermList domain = ATLgetArgument(se, 0);
      ATermAppl codomain = ATAgetArgument(se, 1);
      if (ATgetLength(domain) == 2) {
        ATermAppl se_func = ATAgetFirst(domain);
        ATermAppl se_fset = ATAgetFirst(ATgetNext(domain));
        if (gsIsSortArrow(se_func)) {
          ATermList se_func_dom = ATLgetArgument(se_func, 0);
          return ATisEqual(ATAgetArgument(se_func, 1), gsMakeSortExprBool())
              && ATgetLength(se_func_dom) == 1
              && ATisEqual(DataExpr, gsMakeOpIdSet(ATAgetFirst(se_func_dom), se_fset, codomain));
        }
      }
    }
  }
  return false;
}

bool gsIsOpIdSetFSet(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      ATermAppl codomain = ATAgetArgument(s, 1);
      return
        ATgetLength(domain) == 1
        && ATisEqual(DataExpr, gsMakeOpIdSetFSet(ATAgetFirst(domain), codomain));
    }
  }
  return false;
}

bool gsIsOpIdSetComp(ATermAppl DataExpr)
{
  if (gsIsOpId(DataExpr))
  {
    ATermAppl se = ATAgetArgument(DataExpr, 1);
    if (gsIsSortArrow(se))
    {
      ATermList domain = ATLgetArgument(se, 0);
      ATermAppl codomain = ATAgetArgument(se, 1);
      if (ATgetLength(domain) == 1) {
        ATermAppl se_func = ATAgetFirst(domain);
        if (gsIsSortArrow(se_func)) {
          ATermList se_func_dom = ATLgetArgument(se_func, 0);
          return ATisEqual(ATAgetArgument(se_func, 1), gsMakeSortExprBool())
              && ATgetLength(se_func_dom) == 1
              && ATisEqual(DataExpr, gsMakeOpIdSetComp(ATAgetFirst(se_func_dom), codomain));
        }
      }
    }
  }
  return false;
}

bool gsIsOpIdEmptySet(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortId(s))
    {
      return ATisEqual(DataExpr, gsMakeOpIdEmptySet(s));
    }
  }
  return false;
}

bool gsIsOpIdSetEnum(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    return ATisEqual(DataExpr, gsMakeOpIdSetEnum(s));
  }
  return false;
}

bool gsIsOpIdEltIn(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      if(ATgetLength(domain) == 2)
      {
        ATermAppl arg0 = ATAgetFirst(domain);
        ATermAppl arg1 = ATAgetFirst(ATgetNext(domain));
        return ATisEqual(DataExpr, gsMakeOpIdEltIn(arg0, arg1));
      }
    }
  }
  return false;
}

bool gsIsOpIdSetUnion(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      return ATisEqual(DataExpr, gsMakeOpIdSetUnion(ATAgetFirst(domain)));
    }
  }
  return false;
}

bool gsIsOpIdSetDiff(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      return ATisEqual(DataExpr, gsMakeOpIdSetDiff(ATAgetFirst(domain)));
    }
  }
  return false;
}

bool gsIsOpIdSetIntersect(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      return ATisEqual(DataExpr, gsMakeOpIdSetIntersect(ATAgetFirst(domain)));
    }
  }
  return false;
}

bool gsIsOpIdSetCompl(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      return ATisEqual(DataExpr, gsMakeOpIdSetCompl(ATAgetFirst(domain)));
    }
  }
  return false;
}

bool gsIsOpIdBag(ATermAppl DataExpr)
{
  if (gsIsOpId(DataExpr))
  {
    ATermAppl se = ATAgetArgument(DataExpr, 1);
    if (gsIsSortArrow(se))
    {
      ATermList domain = ATLgetArgument(se, 0);
      ATermAppl codomain = ATAgetArgument(se, 1);
      if (ATgetLength(domain) == 2) {
        ATermAppl se_func = ATAgetFirst(domain);
        ATermAppl se_fset = ATAgetFirst(ATgetNext(domain));
        if (gsIsSortArrow(se_func)) {
          ATermList se_func_dom = ATLgetArgument(se_func, 0);
          return ATisEqual(ATAgetArgument(se_func, 1), gsMakeSortExprNat())
              && ATgetLength(se_func_dom) == 1
              && ATisEqual(DataExpr, gsMakeOpIdBag(ATAgetFirst(se_func_dom), se_fset, codomain));
        }
      }
    }
  }
  return false;
}

bool gsIsOpIdBagFBag(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      ATermAppl codomain = ATAgetArgument(s, 1);
      return ATgetLength(domain) == 1
          && ATisEqual(DataExpr, gsMakeOpIdBagFBag(ATAgetFirst(domain), codomain));
    }
  }
  return false;
}

bool gsIsOpIdBagComp(ATermAppl DataExpr)
{
  if (gsIsOpId(DataExpr))
  {
    ATermAppl se = ATAgetArgument(DataExpr, 1);
    if (gsIsSortArrow(se))
    {
      ATermList domain = ATLgetArgument(se, 0);
      ATermAppl codomain = ATAgetArgument(se, 1);
      if (ATgetLength(domain) == 1) {
        ATermAppl se_func = ATAgetFirst(domain);
        if (gsIsSortArrow(se_func)) {
          ATermList se_func_dom = ATLgetArgument(se_func, 0);
          return ATisEqual(ATAgetArgument(se_func, 1), gsMakeSortExprNat())
              && ATgetLength(se_func_dom) == 1
              && ATisEqual(DataExpr, gsMakeOpIdBagComp(ATAgetFirst(se_func_dom), codomain));
        }
      }
    }
  }
  return false;
}

bool gsIsOpIdEmptyBag(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortId(s))
    {
      return ATisEqual(DataExpr, gsMakeOpIdEmptyBag(s));
    }
  }
  return false;
}

bool gsIsOpIdBagEnum(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    return ATisEqual(DataExpr, gsMakeOpIdBagEnum(s));
  }
  return false;
}

bool gsIsOpIdCount(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      if(ATgetLength(domain) == 2)
      {
        ATermAppl arg0 = ATAgetFirst(domain);
        ATermAppl arg1 = ATAgetFirst(ATgetNext(domain));
        return ATisEqual(DataExpr, gsMakeOpIdCount(arg0, arg1));
      }
    }
  }
  return false;
}

bool gsIsOpIdBagJoin(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      return ATisEqual(DataExpr, gsMakeOpIdBagJoin(ATAgetFirst(domain)));
    }
  }
  return false;
}

bool gsIsOpIdBagDiff(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      return ATisEqual(DataExpr, gsMakeOpIdBagDiff(ATAgetFirst(domain)));
    }
  }
  return false;
}

bool gsIsOpIdBagIntersect(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      return ATisEqual(DataExpr, gsMakeOpIdBagIntersect(ATAgetFirst(domain)));
    }
  }
  return false;
}

bool gsIsOpIdBag2Set(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      ATermAppl codomain = ATAgetArgument(s, 1);
      return ATisEqual(DataExpr, gsMakeOpIdBag2Set(ATAgetFirst(domain), codomain));
    }
  }
  return false;
}

bool gsIsOpIdSet2Bag(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      ATermAppl codomain = ATAgetArgument(s, 1);
      return ATisEqual(DataExpr, gsMakeOpIdSet2Bag(ATAgetFirst(domain), codomain));
    }
  }
  return false;
}

bool gsIsOpIdFSetEmpty(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    return ATisEqual(DataExpr, gsMakeOpIdFSetEmpty(s));
  }
  return false;
}

bool gsIsOpIdFSetInsert(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      ATermAppl codomain = ATAgetArgument(s, 1);
      return ATisEqual(DataExpr, gsMakeOpIdFSetInsert(ATAgetFirst(domain), codomain));
    }
  }
  return false;
}

bool gsIsOpIdFSetCInsert(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      ATermAppl codomain = ATAgetArgument(s, 1);
      return ATisEqual(DataExpr, gsMakeOpIdFSetCInsert(ATAgetFirst(domain), codomain));
    }
  }
  return false;
}

bool gsIsOpIdFSetIn(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      if(ATgetLength(domain) == 2)
      {
        ATermAppl d1 = ATAgetFirst(domain);
        ATermAppl d2 = ATAgetFirst(ATgetNext(domain));
        return ATisEqual(DataExpr, gsMakeOpIdFSetIn(d1, d2));
      }
    }
  }
  return false;
}

bool gsIsOpIdFSetLTE(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      if(ATgetLength(domain) == 3)
      {
        ATermAppl d1 = ATAgetFirst(domain);
        ATermAppl d2 = ATAgetFirst(ATgetNext(domain));
        if(gsIsSortArrow(d1))
        {
          ATermList domaind1 = ATLgetArgument(d1, 0);
          ATermAppl d11 = ATAgetFirst(domaind1);
          return ATisEqual(DataExpr, gsMakeOpIdFSetLTE(d11, d2));
        }
      }
    }
  }
  return false;
}

bool gsIsOpIdFSetUnion(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      if(ATgetLength(domain) == 4)
      {
        ATermAppl d1 = ATAgetFirst(domain);
        ATermAppl d3 = ATAgetFirst(ATgetNext(ATgetNext(domain)));
        if(gsIsSortArrow(d1))
        {
          ATermList domaind1 = ATLgetArgument(d1, 0);
          ATermAppl d11 = ATAgetFirst(domaind1);
          return ATisEqual(DataExpr, gsMakeOpIdFSetUnion(d11, d3));
        }
      }
    }
  }
  return false;
}

bool gsIsOpIdFSetInter(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      if(ATgetLength(domain) == 4)
      {
        ATermAppl d1 = ATAgetFirst(domain);
        ATermAppl d3 = ATAgetFirst(ATgetNext(ATgetNext(domain)));
        if(gsIsSortArrow(d1))
        {
          ATermList domaind1 = ATLgetArgument(d1, 0);
          ATermAppl d11 = ATAgetFirst(domaind1);
          return ATisEqual(DataExpr, gsMakeOpIdFSetInter(d11, d3));
        }
      }
    }
  }
  return false;
}

bool gsIsOpIdFBagEmpty(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    return ATisEqual(DataExpr, gsMakeOpIdFBagEmpty(s));
  }
  return false;
}

bool gsIsOpIdFBagInsert(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      ATermAppl codomain = ATAgetArgument(s, 1);
      return ATisEqual(DataExpr, gsMakeOpIdFBagInsert(ATAgetFirst(domain), codomain));
    }
  }
  return false;
}

bool gsIsOpIdFBagCInsert(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      ATermAppl codomain = ATAgetArgument(s, 1);
      return ATisEqual(DataExpr, gsMakeOpIdFBagCInsert(ATAgetFirst(domain), codomain));
    }
  }
  return false;
}

bool gsIsOpIdFBagCount(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      if(ATgetLength(domain) == 2)
      {
        ATermAppl d1 = ATAgetFirst(domain);
        ATermAppl d2 = ATAgetFirst(ATgetNext(domain));
        return ATisEqual(DataExpr, gsMakeOpIdFBagCount(d1, d2));
      }
    }
  }
  return false;
}

bool gsIsOpIdFBagIn(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      if(ATgetLength(domain) == 2)
      {
        ATermAppl d1 = ATAgetFirst(domain);
        ATermAppl d2 = ATAgetFirst(ATgetNext(domain));
        return ATisEqual(DataExpr, gsMakeOpIdFBagIn(d1, d2));
      }
    }
  }
  return false;
}

bool gsIsOpIdFBagLTE(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      if(ATgetLength(domain) == 3)
      {
        ATermAppl d1 = ATAgetFirst(domain);
        ATermAppl d2 = ATAgetFirst(ATgetNext(domain));
        if(gsIsSortArrow(d1))
        {
          ATermList domaind1 = ATLgetArgument(d1, 0);
          ATermAppl d11 = ATAgetFirst(domaind1);
          return ATisEqual(DataExpr, gsMakeOpIdFBagLTE(d11, d2));
        }
      }
    }
  }
  return false;
}

bool gsIsOpIdFBagJoin(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      if(ATgetLength(domain) == 4)
      {
        ATermAppl d1 = ATAgetFirst(domain);
        ATermAppl d3 = ATAgetFirst(ATgetNext(ATgetNext(domain)));
        if(gsIsSortArrow(d1))
        {
          ATermList domaind1 = ATLgetArgument(d1, 0);
          ATermAppl d11 = ATAgetFirst(domaind1);
          return ATisEqual(DataExpr, gsMakeOpIdFBagJoin(d11, d3));
        }
      }
    }
  }
  return false;
}

bool gsIsOpIdFBagInter(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      if(ATgetLength(domain) == 4)
      {
        ATermAppl d1 = ATAgetFirst(domain);
        ATermAppl d3 = ATAgetFirst(ATgetNext(ATgetNext(domain)));
        if(gsIsSortArrow(d1))
        {
          ATermList domaind1 = ATLgetArgument(d1, 0);
          ATermAppl d11 = ATAgetFirst(domaind1);
          return ATisEqual(DataExpr, gsMakeOpIdFBagInter(d11, d3));
        }
      }
    }
  }
  return false;
}

bool gsIsOpIdFBagDiff(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      if(ATgetLength(domain) == 4)
      {
        ATermAppl d1 = ATAgetFirst(domain);
        ATermAppl d3 = ATAgetFirst(ATgetNext(ATgetNext(domain)));
        if(gsIsSortArrow(d1))
        {
          ATermList domaind1 = ATLgetArgument(d1, 0);
          ATermAppl d11 = ATAgetFirst(domaind1);
          return ATisEqual(DataExpr, gsMakeOpIdFBagDiff(d11, d3));
        }
      }
    }
  }
  return false;
}

bool gsIsOpIdFBag2FSet(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      ATermAppl codomain = ATAgetArgument(s, 1);
      if(ATgetLength(domain) == 2)
      {
        ATermAppl d1 = ATAgetFirst(domain);
        ATermAppl d2 = ATAgetFirst(ATgetNext(domain));
        if(gsIsSortArrow(d1))
        {
          ATermList domaind1 = ATLgetArgument(d1, 0);
          ATermAppl d11 = ATAgetFirst(domaind1);
          return ATisEqual(DataExpr, gsMakeOpIdFBag2FSet(d11, d2, codomain));
        }
      }
    }
  }
  return false;
}

bool gsIsOpIdFSet2FBag(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      ATermAppl codomain = ATAgetArgument(s, 1);
      ATermAppl d1 = ATAgetFirst(domain);
      return ATisEqual(DataExpr, gsMakeOpIdFSet2FBag(d1, codomain));
    }
  }
  return false;
}

bool gsIsOpIdFalseFunc(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      ATermAppl d1 = ATAgetFirst(domain);
      return ATisEqual(DataExpr, gsMakeOpIdFalseFunc(d1));
    }
  }
  return false;
}

bool gsIsOpIdTrueFunc(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      ATermAppl d1 = ATAgetFirst(domain);
      return ATisEqual(DataExpr, gsMakeOpIdTrueFunc(d1));
    }
  }
  return false;
}

bool gsIsOpIdZeroFunc(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      ATermAppl d1 = ATAgetFirst(domain);
      return ATisEqual(DataExpr, gsMakeOpIdZeroFunc(d1));
    }
  }
  return false;
}

bool gsIsOpIdOneFunc(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      ATermAppl d1 = ATAgetFirst(domain);
      return ATisEqual(DataExpr, gsMakeOpIdOneFunc(d1));
    }
  }
  return false;
}

bool gsIsOpIdBagElt(ATermAppl DataExpr)
{
  if(gsIsOpId(DataExpr))
  {
    ATermAppl s = ATAgetArgument(DataExpr,1);
    if(gsIsSortArrow(s))
    {
      ATermList domain = ATLgetArgument(s, 0);
      if(ATgetLength(domain) == 2)
      {
        ATermAppl arg0 = ATAgetFirst(domain);
        ATermAppl arg1 = ATAgetArgument(s, 1);
        return ATisEqual(DataExpr, gsMakeOpIdBagElt(arg0, arg1));
      }
    }
  }
  return false;
}

bool gsIsOpIdSwapZero(ATermAppl DataExpr)
{
  return ATisEqual(DataExpr, gsMakeOpIdSwapZero());
}


// Regognisers for data expressions
bool gsIsDataExprTrue(ATermAppl DataExpr)
{
  return DataExpr == gsMakeOpIdTrue();
}

bool gsIsDataExprFalse(ATermAppl DataExpr)
{
  return DataExpr == gsMakeOpIdFalse();
}

bool gsIsDataExprNot(ATermAppl DataExpr)
{
  if (gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr,0);
    return gsGetSort(DataExpr) == gsMakeSortExprBool() &&
           ATisEqual(t,gsMakeOpIdNot());
  }
  return false;
}

bool gsIsDataExprAnd(ATermAppl DataExpr)
{
  if (gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr,0);
    return ATisEqual(t, gsMakeOpIdAnd());
  }
  return false;
}

bool gsIsDataExprOr(ATermAppl DataExpr)
{
  if (gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr,0);
    return ATisEqual(t, gsMakeOpIdOr());
  }
  return false;
}

bool gsIsDataExprImp(ATermAppl DataExpr)
{
  if (gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr,0);
    return ATisEqual(t, gsMakeOpIdImp());
  }
  return false;
}

bool gsIsDataExprEq(ATermAppl DataExpr)
{
  if(gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr,0);
    return gsIsOpIdEq(t);
  }
  return false;
}

bool gsIsDataExprNeq(ATermAppl DataExpr)
{
  if(gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr,0);
    return gsIsOpIdNeq(t);
  }
  return false;
}

bool gsIsDataExprIf(ATermAppl DataExpr)
{
  if(gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr,0);
    return gsIsOpIdIf(t);
  }
  return false;
}

bool gsIsDataExprC0(ATermAppl DataExpr)
{
  return ATisEqual(DataExpr, gsMakeOpIdC0());
}

bool gsIsDataExprC1(ATermAppl DataExpr)
{
  return ATisEqual(DataExpr, gsMakeOpIdC1());
}

bool gsIsDataExprCDub(ATermAppl DataExpr)
{
  if (gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr, 0);
    return ATisEqual(t, gsMakeOpIdCDub());
  }
  return false;
}

bool gsIsDataExprCNat(ATermAppl DataExpr)
{
  if(gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr,0);
    return ATisEqual(t, gsMakeOpIdCNat());
  }
  return false;
}

bool gsIsDataExprCPair(ATermAppl DataExpr)
{
  if(gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr,0);
    return ATisEqual(t, gsMakeOpIdCPair());
  }
  return false;
}

bool gsIsDataExprCInt(ATermAppl DataExpr)
{
  if(gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr,0);
    return ATisEqual(t, gsMakeOpIdCInt());
  }
  return false;
}

bool gsIsDataExprCReal(ATermAppl DataExpr)
{
  if(gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr,0);
    return ATisEqual(t, gsMakeOpIdCReal());
  }
  return false;
}

bool gsIsDataExprPos2Nat(ATermAppl DataExpr)
{
  if(gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr,0);
    return ATisEqual(t, gsMakeOpIdPos2Nat());
  }
  return false;
}

bool gsIsDataExprPos2Int(ATermAppl DataExpr)
{
  if(gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr,0);
    return ATisEqual(t, gsMakeOpIdPos2Int());
  }
  return false;
}

bool gsIsDataExprPos2Real(ATermAppl DataExpr)
{
  if(gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr,0);
    return ATisEqual(t, gsMakeOpIdPos2Real());
  }
  return false;
}

bool gsIsDataExprNat2Int(ATermAppl DataExpr)
{
  if(gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr,0);
    return ATisEqual(t, gsMakeOpIdNat2Int());
  }
  return false;
}

bool gsIsDataExprNat2Real(ATermAppl DataExpr)
{
  if(gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr,0);
    return ATisEqual(t, gsMakeOpIdNat2Real());
  }
  return false;
}

bool gsIsDataExprInt2Real(ATermAppl DataExpr)
{
  if(gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr,0);
    return ATisEqual(t, gsMakeOpIdInt2Real());
  }
  return false;
}

bool gsIsDataExprCNeg(ATermAppl DataExpr)
{
  if(gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr, 0);
    return ATisEqual(t, gsMakeOpIdCNeg());
  }
  return false;
}

bool gsIsDataExprLTE(ATermAppl DataExpr)
{
  if(gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr,0);
    return gsIsOpIdLTE(t);
  }
  return false;
}

bool gsIsDataExprLT(ATermAppl DataExpr)
{
  if(gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr,0);
    return gsIsOpIdLT(t);
  }
  return false;
}

bool gsIsDataExprGTE(ATermAppl DataExpr)
{
  if(gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr,0);
    return gsIsOpIdGTE(t);
  }
  return false;
}

bool gsIsDataExprGT(ATermAppl DataExpr)
{
  if(gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr,0);
    return gsIsOpIdGT(t);
  }
  return false;
}

bool gsIsDataExprMax(ATermAppl DataExpr)
{
  if(gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr,0);
    return gsIsOpIdMax(t);
  }
  return false;
}

bool gsIsDataExprMin(ATermAppl DataExpr)
{
  if(gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr,0);
    return gsIsOpIdMin(t);
  }
  return false;
}

bool gsIsDataExprAbs(ATermAppl DataExpr)
{
  if(gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr,0);
    return gsIsOpIdAbs(t);
  }
  return false;
}

bool gsIsDataExprNeg(ATermAppl DataExpr)
{
  if(gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr,0);
    return gsIsOpIdNeg(t);
  }
  return false;
}

bool gsIsDataExprSucc(ATermAppl DataExpr)
{
  if(gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr,0);
    return gsIsOpIdSucc(t);
  }
  return false;
}

bool gsIsDataExprPred(ATermAppl DataExpr)
{
  if(gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr,0);
    return gsIsOpIdPred(t);
  }
  return false;
}

bool gsIsDataExprDub(ATermAppl DataExpr)
{
  if(gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr,0);
    return gsIsOpIdDub(t);
  }
  return false;
}

bool gsIsDataExprAdd(ATermAppl DataExpr)
{
  if(gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr,0);
    return gsIsOpIdAdd(t);
  }
  return false;
}

bool gsIsDataExprAddC(ATermAppl DataExpr)
{
  if(gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr,0);
    return ATisEqual(t, gsMakeOpIdAddC());
  }
  return false;
}

bool gsIsDataExprSubt(ATermAppl DataExpr)
{
  if(gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr,0);
    return gsIsOpIdSubt(t);
  }
  return false;
}

bool gsIsDataExprGTESubt(ATermAppl DataExpr)
{
  if(gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr,0);
    return gsIsOpIdGTESubt(t);
  }
  return false;
}

bool gsIsDataExprGTESubtB(ATermAppl DataExpr)
{
  if(gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr,0);
    return ATisEqual(t, gsMakeOpIdGTESubtB());
  }
  return false;
}

bool gsIsDataExprMult(ATermAppl DataExpr)
{
  if(gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr,0);
    return gsIsOpIdMult(t);
  }
  return false;
}

bool gsIsDataExprMultIR(ATermAppl DataExpr)
{
  if(gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr,0);
    return ATisEqual(t, gsMakeOpIdMultIR());
  }
  return false;
}

bool gsIsDataExprDiv(ATermAppl DataExpr)
{
  if(gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr,0);
    return gsIsOpIdDiv(t);
  }
  return false;
}

bool gsIsDataExprMod(ATermAppl DataExpr)
{
  if(gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr,0);
    return gsIsOpIdMod(t);
  }
  return false;
}

bool gsIsDataExprDivMod(ATermAppl DataExpr)
{
  if(gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr,0);
    return ATisEqual(t, gsMakeOpIdDivMod());
  }
  return false;
}

bool gsIsDataExprGDivMod(ATermAppl DataExpr)
{
  if(gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr,0);
    return ATisEqual(t, gsMakeOpIdGDivMod());
  }
  return false;
}

bool gsIsDataExprGGDivMod(ATermAppl DataExpr)
{
  if(gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr,0);
    return ATisEqual(t, gsMakeOpIdGGDivMod());
  }
  return false;
}

bool gsIsDataExprEven(ATermAppl DataExpr)
{
  if(gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr,0);
    return ATisEqual(t, gsMakeOpIdEven());
  }
  return false;
}

bool gsIsDataExprNumber(ATermAppl DataExpr)
{
  if (!gsIsOpId(DataExpr)) return false;
  ATermAppl Name = gsGetName(DataExpr);
  char* s = gsATermAppl2String(Name);
  if (s == NULL) return false;
  return gsIsNumericString(s);
}

bool gsIsDataExprEmptyList(ATermAppl DataExpr)
{
  return gsIsOpIdEmptyList(DataExpr);
}

bool gsIsDataExprListEnum(ATermAppl DataExpr)
{
  if (gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr, 0);
    return gsIsOpIdListEnum(t);
  }
  return false;
}

bool gsIsDataExprCons(ATermAppl DataExpr)
{
  if (gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr, 0);
    return gsIsOpIdCons(t);
  }
  return false;
}

bool gsIsDataExprSetEnum(ATermAppl DataExpr)
{
  if (gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr, 0);
    return gsIsOpIdSetEnum(t);
  }
  else
  {
    return gsIsOpIdBagEnum(DataExpr);
  }
}

bool gsIsDataExprBagEnum(ATermAppl DataExpr)
{
  if (gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr, 0);
    return gsIsOpIdBagEnum(t);
  }
  else
  {
    return gsIsOpIdBagEnum(DataExpr);
  }
}

bool gsIsDataExprSet(ATermAppl DataExpr)
{
  if (gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr, 0);
    return gsIsOpIdSet(t);
  }
  return false;
}

bool gsIsDataExprSetFSet(ATermAppl DataExpr)
{
  if (gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr, 0);
    return gsIsOpIdSetFSet(t);
  }
  return false;
}

bool gsIsDataExprSetComp(ATermAppl DataExpr)
{
  if (gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr, 0);
    return gsIsOpIdSetComp(t);
  }
  return false;
}

bool gsIsDataExprBag(ATermAppl DataExpr)
{
  if (gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr, 0);
    return gsIsOpIdBag(t);
  }
  return false;
}

bool gsIsDataExprBagFBag(ATermAppl DataExpr)
{
  if (gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr, 0);
    return gsIsOpIdBagFBag(t);
  }
  return false;
}

bool gsIsDataExprBagComp(ATermAppl DataExpr)
{
  if (gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr, 0);
    return gsIsOpIdBagComp(t);
  }
  return false;
}

bool gsIsDataExprForall(ATermAppl DataExpr)
{
  if (gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr, 0);
    return gsIsOpIdForall(t);
  }
  return false;
}

bool gsIsDataExprExists(ATermAppl DataExpr)
{
  if (gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr, 0);
    return gsIsOpIdExists(t);
  }
  return false;
}

bool gsIsDataExprFalseFunc(ATermAppl DataExpr)
{
  return gsIsOpIdFalseFunc(DataExpr);
}

bool gsIsDataExprTrueFunc(ATermAppl DataExpr)
{
  return gsIsOpIdTrueFunc(DataExpr);
}

bool gsIsDataExprZeroFunc(ATermAppl DataExpr)
{
  return gsIsOpIdZeroFunc(DataExpr);
}

bool gsIsDataExprOneFunc(ATermAppl DataExpr)
{
  return gsIsOpIdOneFunc(DataExpr);
}

bool gsIsDataExprFSetEmpty(ATermAppl DataExpr)
{
  return gsIsOpIdFSetEmpty(DataExpr);
}

bool gsIsDataExprFSetInsert(ATermAppl DataExpr)
{
  if(gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr, 0);
    return gsIsOpIdFSetInsert(t);
  }
  return false;
}

bool gsIsDataExprFBagEmpty(ATermAppl DataExpr)
{
  return gsIsOpIdFBagEmpty(DataExpr);
}

bool gsIsDataExprFBagCInsert(ATermAppl DataExpr)
{
  if(gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr, 0);
    return gsIsOpIdFBagCInsert(t);
  }
  return false;
}

bool gsIsDataExprBagElt(ATermAppl DataExpr)
{
  if(gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr, 0);
    return gsIsOpIdBagElt(t);
  }
  return false;
}

bool gsIsDataExprSwapZero(ATermAppl DataExpr)
{
  if(gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr, 0);
    return gsIsOpIdSwapZero(t);
  }
  return false;
}

//Multiactions
//------------

ATermAppl gsSortMultAct(ATermAppl MultAct)
{
  assert(gsIsMultAct(MultAct));
  ATermList l = ATLgetArgument(MultAct,0);
  unsigned int len = ATgetLength(l);
  DECL_A(acts,ATerm,len);
  for (unsigned int i=0; !ATisEmpty(l); l=ATgetNext(l),i++)
  {
    acts[i] = ATgetFirst(l);
  }
  //l is empty

  for (unsigned int i=1; i<len; i++)
  {
    unsigned int j = i;
    // XXX comparison is fast but does not define a unique result (i.e. the
    // result is dependent on the specific run of a program)
    while ( acts[j] < acts[j-1] )
    {
      ATerm t = acts[j];
      acts[j] = acts[j-1];
      acts[j-1] = t;
    }
  }

  //l is empty
  for (unsigned int i=0; i<len; i++)
  {
    l = ATinsert(l,acts[len-i-1]);
  }
  FREE_A(acts);
  return gsMakeMultAct(l);
}


//Process expressions
//-------------------

bool gsIsProcExpr(ATermAppl Term)
{
  return gsIsParamId(Term)
      || gsIsIdAssignment(Term)
      || gsIsAction(Term)
      || gsIsProcess(Term)
      || gsIsProcessAssignment(Term)
      || gsIsDelta(Term)
      || gsIsTau (Term)
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


//Mu-calculus formulas
//--------------------

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

bool gsIsRegFrm(ATermAppl Term)
{
  return
    gsIsActFrm(Term) || gsIsRegNil(Term) || gsIsRegSeq(Term) ||
    gsIsRegAlt(Term) || gsIsRegTrans(Term) || gsIsRegTransOrNil(Term);
}

bool gsIsActFrm(ATermAppl Term)
{
  return
    gsIsMultAct(Term) || gsIsDataExpr(Term) || gsIsActTrue(Term) ||
    gsIsActFalse(Term) || gsIsActNot(Term) || gsIsActAnd(Term) ||
    gsIsActOr(Term) || gsIsActImp(Term) || gsIsActForall(Term) ||
    gsIsActExists(Term) || gsIsActAt(Term);
}

// Data specifications
// -------------------

ATermAppl gsMakeEmptyDataSpec()
{
  return gsMakeDataSpec(
    gsMakeSortSpec(ATmakeList0()),
    gsMakeConsSpec(ATmakeList0()),
    gsMakeMapSpec(ATmakeList0()),
    gsMakeDataEqnSpec(ATmakeList0())
  );
}

// PBES's
// ------

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

bool gsIsFixpoint(ATermAppl Term)
{
  return gsIsMu(Term) || gsIsNu(Term);
}

//Local declarations
//------------------
bool IsPNSort(ATermAppl SortExpr)
{
  return
    gsIsSortExprPos(SortExpr) ||
    gsIsSortExprNat(SortExpr);
}

bool IsPNISort(ATermAppl SortExpr)
{
  return
    gsIsSortExprPos(SortExpr) ||
    gsIsSortExprNat(SortExpr) ||
    gsIsSortExprInt(SortExpr);
}

bool IsNISort(ATermAppl SortExpr)
{
  return
    gsIsSortExprNat(SortExpr) ||
    gsIsSortExprInt(SortExpr);
}

bool IsPNIRSort(ATermAppl SortExpr)
{
  return
    gsIsSortExprPos(SortExpr) ||
    gsIsSortExprNat(SortExpr) ||
    gsIsSortExprInt(SortExpr) ||
    gsIsSortExprReal(SortExpr);
}

ATermAppl IntersectPNSorts(ATermAppl SortExpr1, ATermAppl SortExpr2)
{
  assert(IsPNSort(SortExpr1));
  assert(IsPNSort(SortExpr2));
  ATermAppl seNat = gsMakeSortExprNat();
  if (ATisEqual(SortExpr1, seNat)) {
    //SortExpr1 is the biggest type, return SortExpr2
    return SortExpr2;
  } else if (ATisEqual(SortExpr2, seNat)) {
    //SortExpr2 is the biggest type, return SortExpr1
    return SortExpr1;
  } else {
    //SortExpr1 and SortExpr2 are both of sort Pos
    return gsMakeSortExprPos();
  }
}

ATermAppl IntersectPNISorts(ATermAppl SortExpr1, ATermAppl SortExpr2)
{
  assert(IsPNISort(SortExpr1));
  assert(IsPNISort(SortExpr2));
  ATermAppl seInt = gsMakeSortExprInt();
  if (ATisEqual(SortExpr1, seInt)) {
    //SortExpr1 is the biggest type, return SortExpr2
    return SortExpr2;
  } else if (ATisEqual(SortExpr2, seInt)) {
    //SortExpr2 is the biggest type, return SortExpr1
    return SortExpr1;
  } else {
    //SortExpr1 and SortExpr2 are both PN sorts
    return IntersectPNSorts(SortExpr1, SortExpr2);
  }
}

ATermAppl IntersectPNIRSorts(ATermAppl SortExpr1, ATermAppl SortExpr2)
{
  assert(IsPNIRSort(SortExpr1));
  assert(IsPNIRSort(SortExpr2));
  ATermAppl seReal = gsMakeSortExprReal();
  if (ATisEqual(SortExpr1, seReal)) {
    //SortExpr1 is the biggest type, return SortExpr2
    return SortExpr2;
  } else if (ATisEqual(SortExpr2, seReal)) {
    //SortExpr2 is the biggest type, return SortExpr1
    return SortExpr1;
  } else {
    //SortExpr1 and SortExpr2 are both PNI sorts
    return IntersectPNISorts(SortExpr1, SortExpr2);
  }
}

ATermAppl UnitePNSorts(ATermAppl SortExpr1, ATermAppl SortExpr2)
{
  assert(IsPNSort(SortExpr1));
  assert(IsPNSort(SortExpr2));
  ATermAppl seNat = gsMakeSortExprNat();
  if (ATisEqual(SortExpr1, seNat) || ATisEqual(SortExpr2, seNat)) {
    //SortExpr1 or SortExpr2 is the biggest type, return it
    return seNat;
  } else {
    //SortExpr1 and SortExpr2 are both of sort Pos
    return gsMakeSortExprPos();
  }
}

ATermAppl UnitePNISorts(ATermAppl SortExpr1, ATermAppl SortExpr2)
{
  assert(IsPNISort(SortExpr1));
  assert(IsPNISort(SortExpr2));
  ATermAppl seInt = gsMakeSortExprInt();
  if (ATisEqual(SortExpr1, seInt) || ATisEqual(SortExpr2, seInt)) {
    //SortExpr1 or SortExpr2 is the biggest type, return it
    return seInt;
  } else {
    //SortExpr1 and SortExpr2 are both PN sorts
    return UnitePNSorts(SortExpr1, SortExpr2);
  }
}

ATermAppl UnitePNIRSorts(ATermAppl SortExpr1, ATermAppl SortExpr2)
{
  assert(IsPNIRSort(SortExpr1));
  assert(IsPNIRSort(SortExpr2));
  ATermAppl seReal = gsMakeSortExprReal();
  if (ATisEqual(SortExpr1, seReal) || ATisEqual(SortExpr2, seReal)) {
    //SortExpr1 or SortExpr2 is the biggest type, return it
    return seReal;
  } else {
    //SortExpr1 and SortExpr2 are both PNI sorts
    return UnitePNISorts(SortExpr1, SortExpr2);
  }
}

bool gsIsUserIdentifier(const std::string &name) {
  std::istringstream *stream = new std::istringstream("identifier "+name);
  std::vector<std::istream*> streams(1,stream);
  ATerm result = parse_streams(streams,false);
  delete stream;
  return result != NULL;
}

    }
  }
}

