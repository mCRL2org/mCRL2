// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file real.h
/// \brief Provides an implementation of Real numbers, supporting division.

#ifndef MCRL2_LPSRTA_REAL_H
#define MCRL2_LPSRTA_REAL_H

#include <iostream>
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/data/data_operation.h"
#include "mcrl2/data/data_application.h"
#include "mcrl2/data/data_equation.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/core/detail/data_implementation_concrete.h"

using namespace atermpp;
using namespace mcrl2;
using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace mcrl2::data;
using namespace mcrl2::data::data_expr;

inline ATermAppl initMakeOpIdNameDivide(ATermAppl &t) {
  t = gsString2ATermAppl("@/");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameTrunc(ATermAppl &t) {
  t = gsString2ATermAppl("@trunc");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameRound(ATermAppl &t) {
  t = gsString2ATermAppl("@round");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameRational(ATermAppl &t) {
  t = gsString2ATermAppl("@rational");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameNormalizeRational(ATermAppl &t) {
  t = gsString2ATermAppl("@normalize_rational");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameNormalizeRationalWhr(ATermAppl &t) {
  t = gsString2ATermAppl("@normalize_rational_whr");
  ATprotectAppl(&t);
  return t;
}

inline ATermAppl initMakeOpIdNameNormalizeRationalHelper(ATermAppl &t) {
  t = gsString2ATermAppl("@normalize_rational_helper");
  ATprotectAppl(&t);
  return t;
}

ATermAppl gsMakeOpIdNameDivide() {
  static ATermAppl t = initMakeOpIdNameDivide(t);
  return t;
}

ATermAppl gsMakeOpIdNameTrunc() {
  static ATermAppl t = initMakeOpIdNameTrunc(t);
  return t;
}

ATermAppl gsMakeOpIdNameRound() {
  static ATermAppl t = initMakeOpIdNameRound(t);
  return t;
}

ATermAppl gsMakeOpIdNameRational() {
  static ATermAppl t = initMakeOpIdNameRational(t);
  return t;
}

ATermAppl gsMakeOpIdNameNormalizeRational() {
  static ATermAppl t = initMakeOpIdNameNormalizeRational(t);
  return t;
}

ATermAppl gsMakeOpIdNameNormalizeRationalWhr() {
  static ATermAppl t = initMakeOpIdNameNormalizeRationalWhr(t);
  return t;
}

ATermAppl gsMakeOpIdNameNormalizeRationalHelper() {
  static ATermAppl t = initMakeOpIdNameNormalizeRationalHelper(t);
  return t;
}

ATermAppl gsMakeOpIdDivide(ATermAppl SortExpr)
{
  //assert(IsPNIRSort(SortExpr));
  return gsMakeOpId(gsMakeOpIdNameDivide(),
    gsMakeSortArrow2(SortExpr, SortExpr, gsMakeSortExprReal()));
}

ATermAppl gsMakeOpIdTrunc(void)
{
  return gsMakeOpId(gsMakeOpIdNameTrunc(), gsMakeSortArrow1(
    gsMakeSortExprReal(), gsMakeSortExprInt()));
}

ATermAppl gsMakeOpIdRound(void)
{
  return gsMakeOpId(gsMakeOpIdNameRound(), gsMakeSortArrow1(
    gsMakeSortExprReal(), gsMakeSortExprInt()));
}

ATermAppl gsMakeOpIdRational(void)
{
  return gsMakeOpId(gsMakeOpIdNameRational(), gsMakeSortArrow2(
    gsMakeSortExprInt(), gsMakeSortExprPos(), gsMakeSortExprReal()));
}

ATermAppl gsMakeOpIdNormalizeRational(void)
{
  return gsMakeOpId(gsMakeOpIdNameNormalizeRational(), gsMakeSortArrow2(
    gsMakeSortExprInt(), gsMakeSortExprInt(), gsMakeSortExprReal()));
}

ATermAppl gsMakeOpIdNormalizeRationalWhr(void)
{
  return gsMakeOpId(gsMakeOpIdNameNormalizeRationalWhr(), gsMakeSortArrow3(
    gsMakeSortExprPos(), gsMakeSortExprInt(), gsMakeSortExprNat(), gsMakeSortExprReal()));
}

ATermAppl gsMakeOpIdNormalizeRationalHelper(void)
{
  return gsMakeOpId(gsMakeOpIdNameNormalizeRationalHelper(), gsMakeSortArrow2(
    gsMakeSortExprReal(), gsMakeSortExprInt(), gsMakeSortExprReal()));
}

ATermAppl gsMakeDataExprDivide(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprLHS), gsGetSort(DataExprRHS)));
  return gsMakeDataAppl2(gsMakeOpIdDivide(gsGetSort(DataExprLHS)),
    DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprTrunc(ATermAppl DataExpr)
{
  assert(ATisEqual(gsGetSort(DataExpr), gsMakeSortExprReal()));
  return gsMakeDataAppl1(gsMakeOpIdTrunc(), DataExpr);
}

ATermAppl gsMakeDataExprRound(ATermAppl DataExpr)
{
  assert(ATisEqual(gsGetSort(DataExpr), gsMakeSortExprReal()));
  return gsMakeDataAppl1(gsMakeOpIdRound(), DataExpr);
}

ATermAppl gsMakeDataExprRational(ATermAppl DataExprInt, ATermAppl DataExprPos)
{
  assert(ATisEqual(gsGetSort(DataExprInt), gsMakeSortExprInt()));
  assert(ATisEqual(gsGetSort(DataExprPos), gsMakeSortExprPos()));
  return gsMakeDataAppl2(gsMakeOpIdRational(), DataExprInt, DataExprPos);
}

ATermAppl gsMakeDataExprNormalizeRational(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprLHS), gsMakeSortExprInt()));
  assert(ATisEqual(gsGetSort(DataExprRHS), gsMakeSortExprInt()));
  return gsMakeDataAppl2(gsMakeOpIdNormalizeRational(), DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprNormalizeRationalWhr(ATermAppl DataExprPos, ATermAppl DataExprInt, ATermAppl DataExprNat)
{
  assert(ATisEqual(gsGetSort(DataExprPos), gsMakeSortExprPos()));
  assert(ATisEqual(gsGetSort(DataExprInt), gsMakeSortExprInt()));
  assert(ATisEqual(gsGetSort(DataExprNat), gsMakeSortExprNat()));
  return gsMakeDataAppl3(gsMakeOpIdNormalizeRationalWhr(), DataExprPos, DataExprInt, DataExprNat);
}

ATermAppl gsMakeDataExprNormalizeRationalHelper(ATermAppl DataExprReal, ATermAppl DataExprInt)
{
  assert(ATisEqual(gsGetSort(DataExprReal), gsMakeSortExprReal()));
  assert(ATisEqual(gsGetSort(DataExprInt), gsMakeSortExprInt()));
  return gsMakeDataAppl2(gsMakeOpIdNormalizeRationalHelper(), DataExprReal, DataExprInt);
}

bool gsIsDataExprRational(ATermAppl DataExpr)
{
  if(gsIsDataAppl(DataExpr)) {
    ATermAppl t = ATAgetArgument(DataExpr,0);
    if(gsIsOpId(t)) 
      return ATAgetArgument(t,0) == gsMakeOpIdNameRational();
  }
  return false;
}

inline
data_operation_list additional_real_mappings()
{
  ATermList ops = ATmakeList(10,
    (ATerm) gsMakeOpIdDivide(gsMakeSortExprPos()),
    (ATerm) gsMakeOpIdDivide(gsMakeSortExprNat()),
    (ATerm) gsMakeOpIdDivide(gsMakeSortExprInt()),
    (ATerm) gsMakeOpIdDivide(gsMakeSortExprReal()),
    (ATerm) gsMakeOpIdTrunc(),
    (ATerm) gsMakeOpIdRound(),
    (ATerm) gsMakeOpIdRational(),
    (ATerm) gsMakeOpIdNormalizeRational(),
    (ATerm) gsMakeOpIdNormalizeRationalWhr(),
    (ATerm) gsMakeOpIdNormalizeRationalHelper()
  );
  return data_operation_list(ops);
}

inline
data_equation_list additional_real_equations()
{
  ATermAppl nil = gsMakeNil();
  ATermAppl zero = gsMakeDataExprC0();
  ATermAppl one = gsMakeDataExprC1();
  ATermAppl two = gsMakeDataExprCDub(gsMakeDataExprFalse(), one);
  ATermAppl p = gsMakeDataVarId(gsString2ATermAppl("p"), gsMakeSortExprPos());
  ATermAppl q = gsMakeDataVarId(gsString2ATermAppl("q"), gsMakeSortExprPos());
  ATermAppl m = gsMakeDataVarId(gsString2ATermAppl("m"), gsMakeSortExprNat());
  ATermAppl n = gsMakeDataVarId(gsString2ATermAppl("n"), gsMakeSortExprNat());
  ATermAppl x = gsMakeDataVarId(gsString2ATermAppl("x"), gsMakeSortExprInt());
  ATermAppl y = gsMakeDataVarId(gsString2ATermAppl("y"), gsMakeSortExprInt());
  ATermAppl r = gsMakeDataVarId(gsString2ATermAppl("r"), gsMakeSortExprReal());
  ATermAppl s = gsMakeDataVarId(gsString2ATermAppl("s"), gsMakeSortExprReal());
  ATermList pl = ATmakeList1((ATerm) p);
  ATermList nl = ATmakeList1((ATerm) n);
  ATermList xl = ATmakeList1((ATerm) x);
  ATermList rl = ATmakeList1((ATerm) r);
  ATermList pql = ATmakeList2((ATerm) p, (ATerm) q);
  ATermList pxl = ATmakeList2((ATerm) p, (ATerm) x);
  ATermList pxyl = ATmakeList3((ATerm) p, (ATerm) x, (ATerm) y);
  ATermList pqxyl = ATmakeList4((ATerm) p, (ATerm) q, (ATerm) x, (ATerm) y);
  ATermList pnxl = ATmakeList3((ATerm) p, (ATerm) n, (ATerm) x);
  ATermList mnl = ATmakeList2((ATerm) m, (ATerm) n);
  ATermList xyl = ATmakeList2((ATerm) x, (ATerm) y);
  ATermList rsl = ATmakeList2((ATerm) r, (ATerm) s);
  ATermList DataEqns = ATmakeList(34,
    //equality (Real # Real -> Bool)
    (ATerm) gsMakeDataEqn(pqxyl, nil, gsMakeDataExprEq(gsMakeDataExprRational(x,p), gsMakeDataExprRational(y,q)), gsMakeDataExprEq(gsMakeDataExprMult(x,gsMakeDataExprCInt(gsMakeDataExprCNat(q))),gsMakeDataExprMult(y,gsMakeDataExprCInt(gsMakeDataExprCNat(p))))),
    //convert Int to Real (Int -> Real)
    (ATerm) gsMakeDataEqn(xl, nil, gsMakeDataExprInt2Real(x), gsMakeDataExprRational(x, one)),
    //convert Nat to Real (Nat -> Real)
    (ATerm) gsMakeDataEqn(nl, nil, gsMakeDataExprNat2Real(n), gsMakeDataExprRational(gsMakeDataExprCInt(n), one)),
    //convert Pos to Real (Pos -> Real)
    (ATerm) gsMakeDataEqn(pl, nil, gsMakeDataExprPos2Real(p), gsMakeDataExprRational(gsMakeDataExprCInt(gsMakeDataExprCNat(p)), one)),
    //convert Real to Int (Real -> Int)
    (ATerm) gsMakeDataEqn(xl, nil, gsMakeDataExprReal2Int(gsMakeDataExprRational(x, one)), x),
    //convert Real to Nat (Real -> Nat)
    (ATerm) gsMakeDataEqn(xl, nil, gsMakeDataExprReal2Nat(gsMakeDataExprRational(x, one)), gsMakeDataExprInt2Nat(x)),
    //convert Real to Pos (Real -> Pos)
    (ATerm) gsMakeDataEqn(xl, nil, gsMakeDataExprReal2Pos(gsMakeDataExprRational(x, one)), gsMakeDataExprInt2Pos(x)),
    //less than or equal (Real # Real -> Bool)
    (ATerm) gsMakeDataEqn(rl, nil, gsMakeDataExprLTE(r,r), true_()),
    (ATerm) gsMakeDataEqn(pqxyl, nil, gsMakeDataExprLTE(gsMakeDataExprRational(x,p),gsMakeDataExprRational(y,q)), gsMakeDataExprLTE(gsMakeDataExprMult(x,gsMakeDataExprCInt(gsMakeDataExprCNat(q))),gsMakeDataExprMult(y,gsMakeDataExprCInt(gsMakeDataExprCNat(p))))),
    //less than (Real # Real -> Bool)
    (ATerm) gsMakeDataEqn(rl, nil, gsMakeDataExprLT(r,r), false_()),
    (ATerm) gsMakeDataEqn(pqxyl, nil, less(gsMakeDataExprRational(x,p),gsMakeDataExprRational(y,q)), gsMakeDataExprLT(gsMakeDataExprMult(x,gsMakeDataExprCInt(gsMakeDataExprCNat(q))),gsMakeDataExprMult(y,gsMakeDataExprCInt(gsMakeDataExprCNat(p))))),
    //greater than or equal (Real # Real -> Bool)
    (ATerm) gsMakeDataEqn(rsl, nil, gsMakeDataExprGTE(r,s), gsMakeDataExprLTE(s,r)),
    //greater than (Real # Real -> Bool)
    (ATerm) gsMakeDataEqn(rsl, nil, gsMakeDataExprGT(r,s), gsMakeDataExprLT(s,r)),
    //maximum (Real # Real -> Real)
    (ATerm) gsMakeDataEqn(rsl, nil, gsMakeDataExprMax(r,s), gsMakeDataExprIf(gsMakeDataExprLT(r,s), s, r)),
    //minimum (Real # Real -> Real)
    (ATerm) gsMakeDataEqn(rsl, nil, gsMakeDataExprMin(r,s), gsMakeDataExprIf(gsMakeDataExprLT(r,s), r, s)),
    //absolute value (Real -> Real)
    (ATerm) gsMakeDataEqn(rl, nil, gsMakeDataExprAbs(r), gsMakeDataExprIf(gsMakeDataExprLT(r,gsMakeDataExprRational(gsMakeDataExprCInt(zero), one)),gsMakeDataExprNeg(r),r)),
    //negation (Real -> Real)
    (ATerm) gsMakeDataEqn(pxl, nil, gsMakeDataExprNeg(gsMakeDataExprRational(x,p)), gsMakeDataExprRational(gsMakeDataExprNeg(x), p)),
    //successor (Real -> Real)
    (ATerm) gsMakeDataEqn(pxl, nil, gsMakeDataExprSucc(gsMakeDataExprRational(x,p)), gsMakeDataExprRational(plus(x,gsMakeDataExprCInt(gsMakeDataExprCNat(p))),p)),
    //predecessor (Real -> Real)
    (ATerm) gsMakeDataEqn(pxl, nil, gsMakeDataExprPred(gsMakeDataExprRational(x,p)), gsMakeDataExprRational(minus(x,gsMakeDataExprCInt(gsMakeDataExprCNat(p))),p)),
    //addition (Real # Real -> Real)
    (ATerm) gsMakeDataEqn(pqxyl, nil, gsMakeDataExprAdd(gsMakeDataExprRational(x,p), gsMakeDataExprRational(y,q)), gsMakeDataExprNormalizeRational(plus(gsMakeDataExprMult(x,gsMakeDataExprCInt(gsMakeDataExprCNat(q))), gsMakeDataExprMult(y,gsMakeDataExprCInt(gsMakeDataExprCNat(p)))), gsMakeDataExprCInt(gsMakeDataExprCNat(gsMakeDataExprMult(p,q))))),
    //subtraction (Real # Real -> Real)
    (ATerm) gsMakeDataEqn(pqxyl, nil, gsMakeDataExprSubt(gsMakeDataExprRational(x,p), gsMakeDataExprRational(y,q)), gsMakeDataExprNormalizeRational(minus(gsMakeDataExprMult(x,gsMakeDataExprCInt(gsMakeDataExprCNat(q))), gsMakeDataExprMult(y,gsMakeDataExprCInt(gsMakeDataExprCNat(p)))), gsMakeDataExprCInt(gsMakeDataExprCNat(gsMakeDataExprMult(p,q))))),
    //multiplication (Real # Real -> Real)
    (ATerm) gsMakeDataEqn(pqxyl, nil, gsMakeDataExprMult(gsMakeDataExprRational(x,p), gsMakeDataExprRational(y,q)), gsMakeDataExprNormalizeRational(gsMakeDataExprMult(x,y), gsMakeDataExprCInt(gsMakeDataExprCNat(gsMakeDataExprMult(p,q))))),
    //division (Real # Real -> Real)
    (ATerm) gsMakeDataEqn(pqxyl, gsMakeDataExprNeq(y, gsMakeDataExprCInt(zero)), gsMakeDataExprDivide(gsMakeDataExprRational(x,p), gsMakeDataExprRational(y,q)), gsMakeDataExprNormalizeRational(gsMakeDataExprMult(x,gsMakeDataExprCInt(gsMakeDataExprCNat(q))), gsMakeDataExprMult(y,gsMakeDataExprCInt(gsMakeDataExprCNat(p))))),
    //division (Pos # Pos -> Real)
    (ATerm) gsMakeDataEqn(pql, nil, gsMakeDataExprDivide(p,q), gsMakeDataExprNormalizeRational(gsMakeDataExprCInt(gsMakeDataExprCNat(p)), gsMakeDataExprCInt(gsMakeDataExprCNat(q)))),
    //division (Nat # Nat -> Real)
    (ATerm) gsMakeDataEqn(mnl, gsMakeDataExprNeq(m, zero), gsMakeDataExprDivide(m,n), gsMakeDataExprNormalizeRational(gsMakeDataExprCInt(m), gsMakeDataExprCInt(n))),
    //division (Int # Int -> Real)
    (ATerm) gsMakeDataEqn(xyl, gsMakeDataExprNeq(y, gsMakeDataExprCInt(zero)), gsMakeDataExprDivide(x,y), gsMakeDataExprNormalizeRational(x,y)),
    //exponentiation (Real # Nat -> Real)
    (ATerm) gsMakeDataEqn(pnxl, nil, gsMakeDataExprExp(gsMakeDataExprRational(x,p), n), gsMakeDataExprNormalizeRational(gsMakeDataExprExp(x,n), gsMakeDataExprCInt(gsMakeDataExprCNat(gsMakeDataExprExp(p,n))))),
    //trunc (Real -> Int)
    (ATerm) gsMakeDataEqn(pxl, nil, gsMakeDataExprTrunc(gsMakeDataExprRational(x,p)), gsMakeDataExprDiv(x,p)),
    //round (Real -> Int)
    (ATerm) gsMakeDataEqn(rl, nil, gsMakeDataExprRound(r), gsMakeDataExprTrunc(gsMakeDataExprAdd(r,gsMakeDataExprRational(gsMakeDataExprCInt(gsMakeDataExprCNat(one)),two)))),
    //normalize_rational (Int # Int -> Real)
    (ATerm) gsMakeDataEqn(pxl, nil, gsMakeDataExprNormalizeRational(x,gsMakeDataExprNeg(p)), gsMakeDataExprNormalizeRational(negate(x), gsMakeDataExprCInt(gsMakeDataExprCNat(p)))),
    (ATerm) gsMakeDataEqn(pxl, nil, gsMakeDataExprNormalizeRational(x,gsMakeDataExprCInt(gsMakeDataExprCNat(p))), gsMakeDataExprNormalizeRationalWhr(p, gsMakeDataExprDiv(x, p), gsMakeDataExprMod(x, p))),
    //normalize_rational_whr (Pos # Int # Nat -> Real)
    (ATerm) gsMakeDataEqn(pnxl, nil, gsMakeDataExprNormalizeRationalWhr(p, x, n), gsMakeDataExprIf(gsMakeDataExprEq(n, zero), gsMakeDataExprRational(x, one), gsMakeDataExprNormalizeRationalHelper(gsMakeDataExprNormalizeRational(gsMakeDataExprCInt(gsMakeDataExprCNat(p)), gsMakeDataExprCInt(n)), x))),
    //normalize_rational_helper (Real # Int -> Real)
    (ATerm) gsMakeDataEqn(pxyl, nil, gsMakeDataExprNormalizeRationalHelper(gsMakeDataExprRational(x, p), y), gsMakeDataExprRational(gsMakeDataExprAdd(gsMakeDataExprCInt(gsMakeDataExprCNat(p)), gsMakeDataExprMult(y, x)), gsMakeDataExprInt2Pos(x))),
    //CReal (Int -> Real)
    (ATerm) gsMakeDataEqn(xl, nil, gsMakeDataExprCReal(x), gsMakeDataExprRational(x, one))
  );

  return data_equation_list(DataEqns);
}

inline
data_specification remove_original_real_implementation(data_specification spec)
{
  // Copy-paste from data implementation
  //Declare data equations for sort Real
  ATermList el = ATmakeList0();
  ATermAppl nil = gsMakeNil();
  ATermAppl p = gsMakeDataVarId(gsString2ATermAppl("p"), gsMakeSortExprPos());
  ATermAppl n = gsMakeDataVarId(gsString2ATermAppl("n"), gsMakeSortExprNat());
  ATermAppl x = gsMakeDataVarId(gsString2ATermAppl("x"), gsMakeSortExprInt());
  ATermAppl y = gsMakeDataVarId(gsString2ATermAppl("y"), gsMakeSortExprInt());
  ATermAppl r = gsMakeDataVarId(gsString2ATermAppl("r"), gsMakeSortExprReal());
  ATermAppl s = gsMakeDataVarId(gsString2ATermAppl("s"), gsMakeSortExprReal());
  ATermList pl = ATmakeList1((ATerm) p);
  ATermList nl = ATmakeList1((ATerm) n);
  ATermList xl  = ATmakeList1((ATerm) x);
  ATermList xyl = ATmakeList2((ATerm) x, (ATerm) y);
  ATermList nxl = ATmakeList2((ATerm) n, (ATerm) x);
  ATermList rsl = ATmakeList2((ATerm) r, (ATerm) s);
  ATermList real_eqns = ATmakeList(21,
      //equality (Real -> Real -> Bool)
      (ATerm) gsMakeDataEqn(xyl,nil,
         gsMakeDataExprEq(gsMakeDataExprCReal(x), gsMakeDataExprCReal(y)),
         gsMakeDataExprEq(x, y)),
      //convert Int to Real (Int -> Real)
      (ATerm) gsMakeDataEqn(el, nil, gsMakeOpIdInt2Real(), gsMakeOpIdCReal()),
      //convert Nat to Real (Nat -> Real)
      (ATerm) gsMakeDataEqn(nl, nil,
         gsMakeDataExprNat2Real(n),
         gsMakeDataExprCReal(gsMakeDataExprCInt(n))),
      //convert Pos to Real (Pos -> Real)
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprPos2Real(p),
         gsMakeDataExprCReal(gsMakeDataExprCInt(gsMakeDataExprCNat(p)))),
      //convert Real to Int (Real -> Int)
      (ATerm) gsMakeDataEqn(xl, nil,
         gsMakeDataExprReal2Int(gsMakeDataExprCReal(x)), x),
      //convert Real to Nat (Real -> Nat)
      (ATerm) gsMakeDataEqn(xl, nil,
         gsMakeDataExprReal2Nat(gsMakeDataExprCReal(x)),
         gsMakeDataExprInt2Nat(x)),
      //convert Real to Pos (Real -> Pos)
      (ATerm) gsMakeDataEqn(xl, nil,
         gsMakeDataExprReal2Pos(gsMakeDataExprCReal(x)),
         gsMakeDataExprInt2Pos(x)),
      //less than or equal (Real -> Real -> Bool)
      (ATerm) gsMakeDataEqn(xyl,nil,
         gsMakeDataExprLTE(gsMakeDataExprCReal(x), gsMakeDataExprCReal(y)),
         gsMakeDataExprLTE(x, y)),
      //less than (Real -> Real -> Bool)
      (ATerm) gsMakeDataEqn(xyl,nil,
         gsMakeDataExprLT(gsMakeDataExprCReal(x), gsMakeDataExprCReal(y)),
         gsMakeDataExprLT(x, y)),
      //greater than or equal (Real -> Real -> Bool)
      (ATerm) gsMakeDataEqn(rsl,nil,
         gsMakeDataExprGTE(r, s), gsMakeDataExprLTE(s, r)),
      //greater than (Real -> Real -> Bool)
      (ATerm) gsMakeDataEqn(rsl,nil,
         gsMakeDataExprGT(r, s), gsMakeDataExprLT(s, r)),
      //maximum (Real -> Real -> Real)
      (ATerm) gsMakeDataEqn(rsl,nil, gsMakeDataExprMax(r, s),
         gsMakeDataExprIf(gsMakeDataExprLTE(r, s), s, r)),
      //minimum (Real -> Real -> Real)
      (ATerm) gsMakeDataEqn(rsl,nil, gsMakeDataExprMin(r, s),
         gsMakeDataExprIf(gsMakeDataExprLTE(r, s), r, s)),
      //absolute value (Real -> Real)
      (ATerm) gsMakeDataEqn(xl,nil,
         gsMakeDataExprAbs(gsMakeDataExprCReal(x)),
         gsMakeDataExprCReal(gsMakeDataExprCInt(gsMakeDataExprAbs(x)))),
      //negation (Real -> Real)
      (ATerm) gsMakeDataEqn(xl,nil,
         gsMakeDataExprNeg(gsMakeDataExprCReal(x)),
         gsMakeDataExprCReal(gsMakeDataExprNeg(x))),
      //successor (Real -> Real)
      (ATerm) gsMakeDataEqn(xl,nil,
         gsMakeDataExprSucc(gsMakeDataExprCReal(x)),
         gsMakeDataExprCReal(gsMakeDataExprSucc(x))),
      //predecessor (Real -> Real)
      (ATerm) gsMakeDataEqn(xl,nil,
         gsMakeDataExprPred(gsMakeDataExprCReal(x)),
         gsMakeDataExprCReal(gsMakeDataExprPred(x))),
      //addition (Real -> Real -> Real)
      (ATerm) gsMakeDataEqn(xyl, nil,
         gsMakeDataExprAdd(gsMakeDataExprCReal(x), gsMakeDataExprCReal(y)),
         gsMakeDataExprCReal(gsMakeDataExprAdd(x, y))),
      //subtraction (Real -> Real -> Real)
      (ATerm) gsMakeDataEqn(xyl, nil,
         gsMakeDataExprSubt(gsMakeDataExprCReal(x), gsMakeDataExprCReal(y)),
         //gsMakeDataExprCReal(gsMakeDataExprSubt(x, y))),
         //x-y == x + (-y)
         gsMakeDataExprCReal(gsMakeDataExprAdd(x, gsMakeDataExprNeg(y)))),
      //multiplication (Real -> Real -> Real)
      (ATerm) gsMakeDataEqn(xyl, nil,
         gsMakeDataExprMult(gsMakeDataExprCReal(x), gsMakeDataExprCReal(y)),
         gsMakeDataExprCReal(gsMakeDataExprMult(x, y))),
      //exponentiation (Real -> Nat -> Real)
      (ATerm) gsMakeDataEqn(nxl,nil,
         gsMakeDataExprExp(gsMakeDataExprCReal(x), n),
         gsMakeDataExprCReal(gsMakeDataExprExp(x, n)))
    );

  return set_equations(spec, subtract_list(list_arg1(aterm_appl(spec.argument(3))), real_eqns));
}

inline
data_specification replace_real_implementation(data_specification s)
{
  s = remove_original_real_implementation(s);
  s = set_mappings(s, s.mappings() + additional_real_mappings());
  s = set_equations(s, s.equations() + additional_real_equations());

  return s;
}

#endif // MCRL2_LPSRTA_REAL_H

