// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file implement_data_types.cpp
/// \brief Regression test for data implementation.

#include <iostream>
#include <boost/range/iterator_range.hpp>
#include <boost/test/minimal.hpp>

#include "mcrl2/data/parser.h"
#include "mcrl2/data/detail/implement_data_types.h"
#include "mcrl2/data/detail/data_implementation_concrete.h"
#include "mcrl2/core/detail/struct.h"

using namespace atermpp;
using namespace mcrl2;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace mcrl2::data::detail;

/* Old data implementation functions, plainly copied over */
void old_impl_sort_bool(t_data_decls *p_data_decls)
{
  //Declare sort Bool
  p_data_decls->sorts = ATinsert(p_data_decls->sorts, (ATerm) gsMakeSortIdBool());
  //Declare constructors for sort Bool
  p_data_decls->cons_ops = ATconcat(ATmakeList2(
      (ATerm) gsMakeOpIdTrue(),
      (ATerm) gsMakeOpIdFalse()
    ), p_data_decls->cons_ops);
  //Declare operations for sort Bool
  ATermAppl se_bool = gsMakeSortExprBool();
  p_data_decls->ops = ATconcat(ATmakeList(4,
      (ATerm) gsMakeOpIdNot(),
      (ATerm) gsMakeOpIdAnd(),
      (ATerm) gsMakeOpIdOr(),
      (ATerm) gsMakeOpIdImp()
    ), p_data_decls->ops);
  //Declare data equations for sort Bool
  ATermList el = ATmakeList0();
  ATermAppl nil = gsMakeNil();
  ATermAppl t = gsMakeDataExprTrue();
  ATermAppl f = gsMakeDataExprFalse();
  ATermAppl b = gsMakeDataVarId(gsString2ATermAppl("b"), se_bool);
  ATermList bl = ATmakeList1((ATerm) b);
  p_data_decls->data_eqns = ATconcat(ATmakeList(19,
      //logical negation (Bool -> Bool)
      (ATerm) gsMakeDataEqn(el, nil, gsMakeDataExprNot(t), f),
      (ATerm) gsMakeDataEqn(el, nil, gsMakeDataExprNot(f), t),
      (ATerm) gsMakeDataEqn(bl, nil,
                     gsMakeDataExprNot(gsMakeDataExprNot(b)), b),
      //conjunction (Bool -> Bool -> Bool)
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprAnd(b, t), b),
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprAnd(b, f), f),
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprAnd(t, b), b),
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprAnd(f, b), f),
      //disjunction (Bool -> Bool -> Bool)
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprOr(b, t), t),
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprOr(b, f), b),
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprOr(t, b), t),
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprOr(f, b), b),
      //implication (Bool -> Bool -> Bool)
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprImp(b, t), t),
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprImp(b, f),
                                            gsMakeDataExprNot(b)),
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprImp(t, b), b),
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprImp(f, b), t),
      //equality (Bool -> Bool -> Bool)
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprEq(t, b), b),
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprEq(f, b), gsMakeDataExprNot(b)),
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprEq(b, t), b),
      (ATerm) gsMakeDataEqn(bl, nil, gsMakeDataExprEq(b, f), gsMakeDataExprNot(b))
    ), p_data_decls->data_eqns);
}

void old_impl_sort_pos(t_data_decls *p_data_decls)
{
  //Declare sort Pos
  p_data_decls->sorts = ATinsert(p_data_decls->sorts, (ATerm) gsMakeSortIdPos());
  //Declare constructors for sort Pos
  p_data_decls->cons_ops = ATconcat(ATmakeList2(
      (ATerm) gsMakeOpIdC1(),
      (ATerm) gsMakeOpIdCDub()
    ), p_data_decls->cons_ops);
  //Declare operations for sort Pos
  ATermAppl se_pos = gsMakeSortExprPos();
  ATermAppl se_bool = gsMakeSortExprBool();
  p_data_decls->ops = ATconcat(ATmakeList(12,
      (ATerm) gsMakeOpIdLTE(se_pos),
      (ATerm) gsMakeOpIdLT(se_pos),
      (ATerm) gsMakeOpIdGTE(se_pos),
      (ATerm) gsMakeOpIdGT(se_pos),
      (ATerm) gsMakeOpIdMax(se_pos, se_pos),
      (ATerm) gsMakeOpIdMin(se_pos),
      (ATerm) gsMakeOpIdAbs(se_pos),
      (ATerm) gsMakeOpIdSucc(se_pos),
      (ATerm) gsMakeOpIdAdd(se_pos, se_pos),
      (ATerm) gsMakeOpIdAddC(),
      (ATerm) gsMakeOpIdMult(se_pos),
      (ATerm) gsMakeOpIdMultIR()
    ), p_data_decls->ops);
  //Declare data equations for sort Pos
  ATermList el = ATmakeList0();
  ATermAppl nil = gsMakeNil();
  ATermAppl t = gsMakeDataExprTrue();
  ATermAppl f = gsMakeDataExprFalse();
  ATermAppl one = gsMakeDataExprC1();
  ATermAppl two = gsMakeDataExprCDub(f, one);
  ATermAppl p = gsMakeDataVarId(gsString2ATermAppl("p"), se_pos);
  ATermAppl q = gsMakeDataVarId(gsString2ATermAppl("q"), se_pos);
  ATermAppl r = gsMakeDataVarId(gsString2ATermAppl("r"), se_pos);
  ATermAppl b = gsMakeDataVarId(gsString2ATermAppl("b"), se_bool);
  ATermAppl c = gsMakeDataVarId(gsString2ATermAppl("c"), se_bool);
  ATermList pl = ATmakeList1((ATerm) p);
  ATermList pql = ATmakeList2((ATerm) p, (ATerm) q);
  ATermList pqrl = ATmakeList3((ATerm) r, (ATerm) p, (ATerm) q);
  ATermList bpl = ATmakeList2((ATerm) b, (ATerm) p);
  ATermList bpql = ATmakeList3((ATerm) b, (ATerm) p, (ATerm) q);
  ATermList bcpql = ATmakeList4((ATerm) p, (ATerm) q, (ATerm) b, (ATerm) c);
  ATermList bpqrl = ATmakeList4((ATerm) r, (ATerm) p, (ATerm) q, (ATerm) b);
  p_data_decls->data_eqns = ATconcat(ATmakeList(41,
      //equality (Pos -> Pos -> Bool)
      (ATerm) gsMakeDataEqn(bpl, nil, 
         gsMakeDataExprEq(one, gsMakeDataExprCDub(b, p)), f),
      (ATerm) gsMakeDataEqn(bpl, nil, 
         gsMakeDataExprEq(gsMakeDataExprCDub(b, p), one), f),
      (ATerm) gsMakeDataEqn(pql,nil,
         gsMakeDataExprEq(gsMakeDataExprCDub(f, p), gsMakeDataExprCDub(t, q)),
         f),
      (ATerm) gsMakeDataEqn(pql,nil,
         gsMakeDataExprEq(gsMakeDataExprCDub(t, p), gsMakeDataExprCDub(f, q)),
         f),
      (ATerm) gsMakeDataEqn(bpql,nil, 
         gsMakeDataExprEq(gsMakeDataExprCDub(b, p), gsMakeDataExprCDub(b, q)),
         gsMakeDataExprEq(p, q)),
      (ATerm) gsMakeDataEqn(bcpql,nil, 
         gsMakeDataExprEq(gsMakeDataExprCDub(b, p), gsMakeDataExprCDub(c, q)),
         gsMakeDataExprAnd(gsMakeDataExprEq(b, c), gsMakeDataExprEq(p, q))),
      //less than or equal (Pos -> Pos -> Bool)
      (ATerm) gsMakeDataEqn(pl, nil, gsMakeDataExprLTE(one, p), t),
      (ATerm) gsMakeDataEqn(bpl, nil, 
         gsMakeDataExprLTE(gsMakeDataExprCDub(b, p), one), f),
      (ATerm) gsMakeDataEqn(bpql,nil, 
         gsMakeDataExprLTE(gsMakeDataExprCDub(b, p), gsMakeDataExprCDub(b, q)),
         gsMakeDataExprLTE(p, q)),
      (ATerm) gsMakeDataEqn(bpql,nil,
         gsMakeDataExprLTE(gsMakeDataExprCDub(f, p), gsMakeDataExprCDub(b, q)),
         gsMakeDataExprLTE(p, q)),
      (ATerm) gsMakeDataEqn(pql,nil,
         gsMakeDataExprLTE(gsMakeDataExprCDub(t, p), gsMakeDataExprCDub(f, q)),
         gsMakeDataExprLT(p, q)),
      (ATerm) gsMakeDataEqn(bcpql,nil,
         gsMakeDataExprLTE(gsMakeDataExprCDub(b, p), gsMakeDataExprCDub(c, q)),
         gsMakeDataExprIf(gsMakeDataExprImp(b, c),
           gsMakeDataExprLTE(p, q), gsMakeDataExprLT(p, q))),
      //less than (Pos -> Pos -> Bool)
      (ATerm) gsMakeDataEqn(pl, nil, gsMakeDataExprLT(p, one), f),
      (ATerm) gsMakeDataEqn(bpl, nil, 
         gsMakeDataExprLT(one, gsMakeDataExprCDub(b, p)), t),
      (ATerm) gsMakeDataEqn(bpql,nil, 
         gsMakeDataExprLT(gsMakeDataExprCDub(b, p), gsMakeDataExprCDub(b, q)),
         gsMakeDataExprLT(p, q)),
      (ATerm) gsMakeDataEqn(pql,nil,
         gsMakeDataExprLT(gsMakeDataExprCDub(f, p), gsMakeDataExprCDub(t, q)),
         gsMakeDataExprLTE(p, q)),
      (ATerm) gsMakeDataEqn(bpql,nil,
         gsMakeDataExprLT(gsMakeDataExprCDub(b, p), gsMakeDataExprCDub(f, q)),
         gsMakeDataExprLT(p, q)),
      (ATerm) gsMakeDataEqn(bcpql,nil,
         gsMakeDataExprLT(gsMakeDataExprCDub(b, p), gsMakeDataExprCDub(c, q)),
         gsMakeDataExprIf(gsMakeDataExprImp(c, b),
           gsMakeDataExprLT(p, q), gsMakeDataExprLTE(p, q))),
      //greater than or equal (Pos -> Pos -> Bool)
      (ATerm) gsMakeDataEqn(pql,nil,
         gsMakeDataExprGTE(p, q), gsMakeDataExprLTE(q, p)),
      //greater than (Pos -> Pos -> Bool)
      (ATerm) gsMakeDataEqn(pql,nil,
         gsMakeDataExprGT(p, q), gsMakeDataExprLT(q, p)),
      //maximum (Pos -> Pos -> Pos)
      (ATerm) gsMakeDataEqn(pql,nil, gsMakeDataExprMax(p, q),
         gsMakeDataExprIf(gsMakeDataExprLTE(p, q), q, p)),
      //minimum (Pos -> Pos -> Pos)
      (ATerm) gsMakeDataEqn(pql,nil, gsMakeDataExprMin(p, q),
         gsMakeDataExprIf(gsMakeDataExprLTE(p, q), p, q)),
      //absolute value (Pos -> Pos)
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprAbs(p), p),
      //successor (Pos -> Pos)
      (ATerm) gsMakeDataEqn(el,nil,
         gsMakeDataExprSucc(one), two),
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprSucc(gsMakeDataExprCDub(f, p)),
         gsMakeDataExprCDub(t, p)),
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprSucc(gsMakeDataExprCDub(t, p)),
         gsMakeDataExprCDub(f, gsMakeDataExprSucc(p))),
      //addition (Pos -> Pos -> Pos)
      (ATerm) gsMakeDataEqn(pql, nil, gsMakeDataExprAdd(p, q),
         gsMakeDataExprAddC(f, p, q)),
      //addition with carry (Bool -> Pos -> Pos -> Pos)
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprAddC(f, one, p), gsMakeDataExprSucc(p)),
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprAddC(t, one, p),
         gsMakeDataExprSucc(gsMakeDataExprSucc(p))),
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprAddC(f, p, one), gsMakeDataExprSucc(p)),
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprAddC(t, p, one),
         gsMakeDataExprSucc(gsMakeDataExprSucc(p))),
      (ATerm) gsMakeDataEqn(bcpql,nil, gsMakeDataExprAddC(
           b, gsMakeDataExprCDub(c, p), gsMakeDataExprCDub(c, q)),
         gsMakeDataExprCDub(b, gsMakeDataExprAddC(c, p, q))),
      (ATerm) gsMakeDataEqn(bpql,nil, gsMakeDataExprAddC(
           b, gsMakeDataExprCDub(f, p), gsMakeDataExprCDub(t, q)),
         gsMakeDataExprCDub(gsMakeDataExprNot(b), gsMakeDataExprAddC(b, p, q))),
      (ATerm) gsMakeDataEqn(bpql,nil, gsMakeDataExprAddC(
           b, gsMakeDataExprCDub(t, p), gsMakeDataExprCDub(f, q)),
         gsMakeDataExprCDub(gsMakeDataExprNot(b), gsMakeDataExprAddC(b, p, q))),
      //multiplication (Pos -> Pos -> Pos)
      (ATerm) gsMakeDataEqn(pql,
         gsMakeDataExprLTE(p, q),
         gsMakeDataExprMult(p, q),
         gsMakeDataExprMultIR(f, one, p, q)),
      (ATerm) gsMakeDataEqn(pql,
         //gsMakeDataExprGT(p, q),
         gsMakeDataExprLT(q, p),
         gsMakeDataExprMult(p, q),
         gsMakeDataExprMultIR(f, one, q, p)),
      //multiplication with intermediate result
      //  (Bool -> Pos -> Pos -> Pos -> Pos)
      (ATerm) gsMakeDataEqn(pql, nil,
         gsMakeDataExprMultIR(f, p, one, q), q),
      (ATerm) gsMakeDataEqn(pql, nil,
         gsMakeDataExprMultIR(t, p, one, q), gsMakeDataExprAddC(f, p, q)),
      (ATerm) gsMakeDataEqn(bpqrl,nil, 
         gsMakeDataExprMultIR(b, p, gsMakeDataExprCDub(f, q), r),
         gsMakeDataExprMultIR(b, p, q, gsMakeDataExprCDub(f, r))),
      (ATerm) gsMakeDataEqn(pqrl,nil, 
         gsMakeDataExprMultIR(f, p, gsMakeDataExprCDub(t, q), r),
         gsMakeDataExprMultIR(t, r, q, gsMakeDataExprCDub(f, r))),
      (ATerm) gsMakeDataEqn(pqrl,nil, 
         gsMakeDataExprMultIR(t, p, gsMakeDataExprCDub(t, q), r),
         gsMakeDataExprMultIR(t, gsMakeDataExprAddC(f, p, r), q,
           gsMakeDataExprCDub(f, r)))
    ), p_data_decls->data_eqns);
}

void old_impl_sort_nat_pair(t_data_decls *p_data_decls)
{
  //Declare sort NatPair
  p_data_decls->sorts = ATinsert(p_data_decls->sorts, (ATerm) gsMakeSortIdNatPair());
  //Declare constructors for sort NatPair
  p_data_decls->cons_ops =
    ATconcat(ATmakeList1((ATerm) gsMakeOpIdCPair()), p_data_decls->cons_ops);
  //Declare operations for sort NatPair
  ATermAppl se_bool = gsMakeSortExprBool();
  ATermAppl se_pos = gsMakeSortExprPos();
  ATermAppl se_nat = gsMakeSortExprNat();
  p_data_decls->ops = ATconcat(ATmakeList(5,
      (ATerm) gsMakeOpIdFirst(),
      (ATerm) gsMakeOpIdLast(),
      (ATerm) gsMakeOpIdDivMod(),
      (ATerm) gsMakeOpIdGDivMod(),
      (ATerm) gsMakeOpIdGGDivMod()
    ), p_data_decls->ops);
  //Declare data equations for sort Int
  ATermList el = ATmakeList0();
  ATermAppl nil = gsMakeNil();
  ATermAppl t = gsMakeDataExprTrue();
  ATermAppl f = gsMakeDataExprFalse();
  ATermAppl one = gsMakeDataExprC1();
  ATermAppl zero = gsMakeDataExprC0();
  ATermAppl b = gsMakeDataVarId(gsString2ATermAppl("b"), se_bool);
  ATermAppl p = gsMakeDataVarId(gsString2ATermAppl("p"), se_pos);
  ATermAppl q = gsMakeDataVarId(gsString2ATermAppl("q"), se_pos);
  ATermAppl m = gsMakeDataVarId(gsString2ATermAppl("m"), se_nat);
  ATermAppl n = gsMakeDataVarId(gsString2ATermAppl("n"), se_nat);
  ATermAppl m_ = gsMakeDataVarId(gsString2ATermAppl("u"), se_nat);
  ATermAppl n_ = gsMakeDataVarId(gsString2ATermAppl("v"), se_nat);
  ATermList mnm_n_l = ATmakeList4((ATerm) n_, (ATerm) m_, (ATerm) n, (ATerm) m);
  ATermList bpl = ATmakeList2((ATerm) b, (ATerm) p);
  ATermList bpql = ATmakeList3((ATerm) b, (ATerm) p, (ATerm) q);
  ATermList bpmnl = ATmakeList4((ATerm) p, (ATerm) n, (ATerm) m, (ATerm) b);
  ATermList pnl = ATmakeList2((ATerm) n, (ATerm) p);
  ATermList pqnl = ATmakeList3((ATerm) n, (ATerm) p, (ATerm) q);
  ATermList mnl = ATmakeList2((ATerm) n, (ATerm) m);
  p_data_decls->data_eqns = ATconcat(ATmakeList(10,
      //equality (NatPair -> NatPair -> Bool)
      (ATerm) gsMakeDataEqn(mnm_n_l,nil, 
         gsMakeDataExprEq(gsMakeDataExprCPair(m, n), gsMakeDataExprCPair(m_, n_)),
         gsMakeDataExprAnd(gsMakeDataExprEq(m, m_), gsMakeDataExprEq(n, n_))),
      //first (NatPair -> Nat)
      (ATerm) gsMakeDataEqn(mnl,nil, gsMakeDataExprFirst(gsMakeDataExprCPair(m, n)), m),
      //last (NatPair -> Nat)
      (ATerm) gsMakeDataEqn(mnl,nil, gsMakeDataExprLast(gsMakeDataExprCPair(m, n)), n),
      //quotient and remainder after division (Pos -> Pos -> NatPair)
      (ATerm) gsMakeDataEqn(el,nil,
         gsMakeDataExprDivMod(one, one),
         gsMakeDataExprCPair(gsMakeDataExprCNat(one), zero)),
      (ATerm) gsMakeDataEqn(bpl,nil,
         gsMakeDataExprDivMod(one, gsMakeDataExprCDub(b, p)),
         gsMakeDataExprCPair(zero, gsMakeDataExprCNat(one))),
      (ATerm) gsMakeDataEqn(bpql,nil,
         gsMakeDataExprDivMod(gsMakeDataExprCDub(b, p), q),
         gsMakeDataExprGDivMod(gsMakeDataExprDivMod(p, q), b, q)),
      //generalised quotient and remainder after division (NatPair -> Bool -> Pos -> NatPair)
      (ATerm) gsMakeDataEqn(bpmnl,nil,
         gsMakeDataExprGDivMod(gsMakeDataExprCPair(m, n), b, p),
         gsMakeDataExprGGDivMod(gsMakeDataExprDub(b, n), m, p)),
      //generalised generalised quotient and remainder after division (Nat -> Nat -> Pos -> NatPair)
      (ATerm) gsMakeDataEqn(pnl,nil,
         gsMakeDataExprGGDivMod(zero, n, p),
         gsMakeDataExprCPair(gsMakeDataExprDub(f, n), zero)),
      (ATerm) gsMakeDataEqn(pqnl,
         gsMakeDataExprLT(p, q),
         gsMakeDataExprGGDivMod(gsMakeDataExprCNat(p), n, q),
         gsMakeDataExprCPair(gsMakeDataExprDub(f, n), gsMakeDataExprCNat(p))),
      (ATerm) gsMakeDataEqn(pqnl,
         //gsMakeDataExprGTE(p, q),
         gsMakeDataExprLTE(q, p),
         gsMakeDataExprGGDivMod(gsMakeDataExprCNat(p), n, q),
         gsMakeDataExprCPair(gsMakeDataExprDub(t, n), gsMakeDataExprGTESubtB(f, p, q)))
    ), p_data_decls->data_eqns);
}

void old_impl_sort_nat(t_data_decls *p_data_decls)
{
  //add implementation of sort NatPair, if necessary
  if (ATindexOf(p_data_decls->sorts, (ATerm) gsMakeSortIdNatPair(), 0) == -1) {
    old_impl_sort_nat_pair(p_data_decls);
  }
  //Declare sort Nat
  p_data_decls->sorts = ATinsert(p_data_decls->sorts, (ATerm) gsMakeSortIdNat());
  //Declare constructors for sort Nat
  p_data_decls->cons_ops = ATconcat(ATmakeList2(
      (ATerm) gsMakeOpIdC0(),
      (ATerm) gsMakeOpIdCNat()
    ), p_data_decls->cons_ops);
  //Declare operations for sort Nat
  ATermAppl se_nat = gsMakeSortExprNat();
  ATermAppl se_pos = gsMakeSortExprPos();
  ATermAppl se_bool = gsMakeSortExprBool();
  p_data_decls->ops = ATconcat(ATmakeList(28,
      (ATerm) gsMakeOpIdPos2Nat(),
      (ATerm) gsMakeOpIdNat2Pos(),
      (ATerm) gsMakeOpIdLTE(se_nat),
      (ATerm) gsMakeOpIdLT(se_nat),
      (ATerm) gsMakeOpIdGTE(se_nat),
      (ATerm) gsMakeOpIdGT(se_nat),
      (ATerm) gsMakeOpIdMax(se_pos, se_nat),
      (ATerm) gsMakeOpIdMax(se_nat, se_pos),
      (ATerm) gsMakeOpIdMax(se_nat, se_nat),
      (ATerm) gsMakeOpIdMin(se_nat),
      (ATerm) gsMakeOpIdAbs(se_nat),
      (ATerm) gsMakeOpIdSucc(se_nat),
      (ATerm) gsMakeOpIdPred(se_pos),
      (ATerm) gsMakeOpIdDub(se_nat),
      (ATerm) gsMakeOpIdAdd(se_pos, se_nat),
      (ATerm) gsMakeOpIdAdd(se_nat, se_pos),
      (ATerm) gsMakeOpIdAdd(se_nat, se_nat),
      (ATerm) gsMakeOpIdGTESubt(se_pos),
      (ATerm) gsMakeOpIdGTESubt(se_nat),
      (ATerm) gsMakeOpIdGTESubtB(),
      (ATerm) gsMakeOpIdMult(se_nat),
      (ATerm) gsMakeOpIdDiv(se_pos),
      (ATerm) gsMakeOpIdDiv(se_nat),
      (ATerm) gsMakeOpIdMod(se_pos),
      (ATerm) gsMakeOpIdMod(se_nat),
      (ATerm) gsMakeOpIdExp(se_pos),
      (ATerm) gsMakeOpIdExp(se_nat),
      (ATerm) gsMakeOpIdEven()
    ), p_data_decls->ops);
  //Declare data equations for sort Nat
  ATermList el = ATmakeList0();
  ATermAppl nil = gsMakeNil();
  ATermAppl t = gsMakeDataExprTrue();
  ATermAppl f = gsMakeDataExprFalse();
  ATermAppl zero = gsMakeDataExprC0();
  ATermAppl one = gsMakeDataExprC1();
  ATermAppl two = gsMakeDataExprCDub(f, one);
  ATermAppl b = gsMakeDataVarId(gsString2ATermAppl("b"), se_bool);
  ATermAppl c = gsMakeDataVarId(gsString2ATermAppl("c"), se_bool);
  ATermAppl p = gsMakeDataVarId(gsString2ATermAppl("p"), se_pos);
  ATermAppl q = gsMakeDataVarId(gsString2ATermAppl("q"), se_pos);
  ATermAppl n = gsMakeDataVarId(gsString2ATermAppl("n"), se_nat);
  ATermAppl m = gsMakeDataVarId(gsString2ATermAppl("m"), se_nat);
  ATermList pl = ATmakeList1((ATerm) p);
  ATermList bpl = ATmakeList2((ATerm) b, (ATerm) p);
  ATermList pql = ATmakeList2((ATerm) p, (ATerm) q);
  ATermList bpql = ATmakeList3((ATerm) b, (ATerm) p, (ATerm) q);
  ATermList bcpql = ATmakeList4((ATerm) b, (ATerm) c, (ATerm) p, (ATerm) q);
  ATermList pnl = ATmakeList2((ATerm) n, (ATerm) p);
  ATermList nl = ATmakeList1((ATerm) n);
  ATermList mnl = ATmakeList2((ATerm) n, (ATerm) m);
  p_data_decls->data_eqns = ATconcat(ATmakeList(75,
      //equality (Nat -> Nat -> Bool)
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprEq(zero, gsMakeDataExprCNat(p)), f),
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprEq(gsMakeDataExprCNat(p), zero), f),
      (ATerm) gsMakeDataEqn(pql,nil,
         gsMakeDataExprEq(gsMakeDataExprCNat(p), gsMakeDataExprCNat(q)),
         gsMakeDataExprEq(p, q)),
      //convert Pos to Nat (Pos -> Nat)
      (ATerm) gsMakeDataEqn(el,nil, gsMakeOpIdPos2Nat(), gsMakeOpIdCNat()),
      //convert Nat to Pos (Nat -> Pos)
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprNat2Pos(gsMakeDataExprCNat(p)), p),
      //less than or equal (Nat -> Nat -> Bool)
      (ATerm) gsMakeDataEqn(nl, nil, gsMakeDataExprLTE(zero, n), t),
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprLTE(gsMakeDataExprCNat(p), zero), f),
      (ATerm) gsMakeDataEqn(pql,nil,
         gsMakeDataExprLTE(gsMakeDataExprCNat(p), gsMakeDataExprCNat(q)),
         gsMakeDataExprLTE(p, q)),
      //less than (Nat -> Nat -> Bool)
      (ATerm) gsMakeDataEqn(nl, nil, gsMakeDataExprLT(n, zero), f),
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprLT(zero, gsMakeDataExprCNat(p)), t),
      (ATerm) gsMakeDataEqn(pql,nil,
         gsMakeDataExprLT(gsMakeDataExprCNat(p), gsMakeDataExprCNat(q)),
         gsMakeDataExprLT(p, q)),
      //greater than or equal (Nat -> Nat -> Bool)
      (ATerm) gsMakeDataEqn(mnl,nil,
         gsMakeDataExprGTE(m, n), gsMakeDataExprLTE(n, m)),
      //greater than (Nat -> Nat -> Bool)
      (ATerm) gsMakeDataEqn(mnl,nil,
         gsMakeDataExprGT(m, n), gsMakeDataExprLT(n, m)),
      //maximum (Pos -> Nat -> Pos)
      (ATerm) gsMakeDataEqn(pl,nil, gsMakeDataExprMax(p, zero), p),
      (ATerm) gsMakeDataEqn(pql,nil,
         gsMakeDataExprMax(p, gsMakeDataExprCNat(q)),
         //gsMakeDataExprMax(p, q)),
         gsMakeDataExprIf(gsMakeDataExprLTE(p, q), q, p)),
      //maximum (Nat -> Pos -> Pos)
      (ATerm) gsMakeDataEqn(pl,nil, gsMakeDataExprMax(zero, p), p),
      (ATerm) gsMakeDataEqn(pql,nil,
         gsMakeDataExprMax(gsMakeDataExprCNat(p), q),
         //gsMakeDataExprMax(p, q)),
         gsMakeDataExprIf(gsMakeDataExprLTE(p, q), q, p)),
      //maximum (Nat -> Nat -> Nat)
      (ATerm) gsMakeDataEqn(mnl,nil, gsMakeDataExprMax(m, n),
         gsMakeDataExprIf(gsMakeDataExprLTE(m, n), n, m)),
      //minimum (Nat -> Nat -> Nat)
      (ATerm) gsMakeDataEqn(mnl,nil, gsMakeDataExprMin(m, n),
         gsMakeDataExprIf(gsMakeDataExprLTE(m, n), m, n)),
      //absolute value (Nat -> Nat)
      (ATerm) gsMakeDataEqn(nl,nil,
         gsMakeDataExprAbs(n), n),
      //successor (Nat -> Pos)
      (ATerm) gsMakeDataEqn(el,nil, gsMakeDataExprSucc(zero), one),
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprSucc(gsMakeDataExprCNat(p)), gsMakeDataExprSucc(p)),
      (ATerm) gsMakeDataEqn(nl,nil,
         gsMakeDataExprSucc(gsMakeDataExprSucc(n)),
         gsMakeDataExprCDub(
           gsMakeDataExprEq(gsMakeDataExprMod(n, two), gsMakeDataExprCNat(one)),
           gsMakeDataExprSucc(gsMakeDataExprDiv(n, two)))),
      //predecessor (Pos -> Nat)
      (ATerm) gsMakeDataEqn(el,nil, gsMakeDataExprPred(one), zero),
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprPred(gsMakeDataExprCDub(t, p)),
         gsMakeDataExprCNat(gsMakeDataExprCDub(f, p))),
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprPred(gsMakeDataExprCDub(f, p)),
         gsMakeDataExprDub(t, gsMakeDataExprPred(p))),
      //double (Bool -> Nat -> Nat)
      (ATerm) gsMakeDataEqn(el,nil, gsMakeDataExprDub(f, zero), zero),
      (ATerm) gsMakeDataEqn(el,nil,
         gsMakeDataExprDub(t, zero), gsMakeDataExprCNat(one)),
      (ATerm) gsMakeDataEqn(bpl,nil,
         gsMakeDataExprDub(b, gsMakeDataExprCNat(p)),
         gsMakeDataExprCNat(gsMakeDataExprCDub(b, p))),
      //addition (Pos -> Nat -> Pos)
      (ATerm) gsMakeDataEqn(pl, nil, gsMakeDataExprAdd(p, zero), p),
      (ATerm) gsMakeDataEqn(pql, nil,
        gsMakeDataExprAdd(p, gsMakeDataExprCNat(q)),
        //gsMakeDataExprAdd(p, q)),
        gsMakeDataExprAddC(f, p, q)),
      //addition (Nat -> Pos -> Pos)
      (ATerm) gsMakeDataEqn(pl, nil, gsMakeDataExprAdd(zero, p), p),
      (ATerm) gsMakeDataEqn(pql, nil,
        gsMakeDataExprAdd(gsMakeDataExprCNat(p), q),
        //gsMakeDataExprAdd(p, q)),
        gsMakeDataExprAddC(f, p, q)),
      //addition (Nat -> Nat -> Nat)
      (ATerm) gsMakeDataEqn(nl, nil, gsMakeDataExprAdd(zero, n), n),
      (ATerm) gsMakeDataEqn(nl, nil, gsMakeDataExprAdd(n, zero), n),
      (ATerm) gsMakeDataEqn(pql,nil,
         gsMakeDataExprAdd(gsMakeDataExprCNat(p), gsMakeDataExprCNat(q)),
         gsMakeDataExprCNat(gsMakeDataExprAddC(f, p, q))),
      //GTE subtraction (Pos -> Pos -> Nat)
      (ATerm) gsMakeDataEqn(pql, nil,
         gsMakeDataExprGTESubt(p, q),
         gsMakeDataExprGTESubtB(f, p, q)),
      //GTE subtraction (Nat -> Nat -> Nat)
      (ATerm) gsMakeDataEqn(nl, nil, gsMakeDataExprGTESubt(n, zero), n),
      (ATerm) gsMakeDataEqn(pql, nil,
         gsMakeDataExprGTESubt(gsMakeDataExprCNat(p), gsMakeDataExprCNat(q)),
         //gsMakeDataExprGTESubt(p, q)),
         gsMakeDataExprGTESubtB(f, p, q)),
      //GTE subtraction with borrow (Bool -> Pos -> Pos -> Nat)
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprGTESubtB(f, p, one),
         gsMakeDataExprPred(p)),
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprGTESubtB(t, p, one),
         gsMakeDataExprPred(gsMakeDataExprNat2Pos(gsMakeDataExprPred(p)))),
      (ATerm) gsMakeDataEqn(bcpql, nil,
         gsMakeDataExprGTESubtB(b, gsMakeDataExprCDub(c, p),
           gsMakeDataExprCDub(c, q)),
         gsMakeDataExprDub(b, gsMakeDataExprGTESubtB(b, p, q))),
      (ATerm) gsMakeDataEqn(bpql, nil,
         gsMakeDataExprGTESubtB(b, gsMakeDataExprCDub(f, p),
           gsMakeDataExprCDub(t, q)),
         gsMakeDataExprDub(gsMakeDataExprNot(b),
           gsMakeDataExprGTESubtB(t, p, q))),
      (ATerm) gsMakeDataEqn(bpql, nil,
         gsMakeDataExprGTESubtB(b, gsMakeDataExprCDub(t, p),
           gsMakeDataExprCDub(f, q)),
         gsMakeDataExprDub(gsMakeDataExprNot(b),
           gsMakeDataExprGTESubtB(f, p, q))),
      //multiplication (Nat -> Nat -> Nat)
      (ATerm) gsMakeDataEqn(nl, nil, gsMakeDataExprMult(zero, n), zero),
      (ATerm) gsMakeDataEqn(nl, nil, gsMakeDataExprMult(n, zero), zero),
      (ATerm) gsMakeDataEqn(pql,nil, 
         gsMakeDataExprMult(gsMakeDataExprCNat(p), gsMakeDataExprCNat(q)),
         gsMakeDataExprCNat(gsMakeDataExprMult(p, q))),
      //exponentiation (Pos -> Nat -> Pos)
      (ATerm) gsMakeDataEqn(pl, nil, gsMakeDataExprExp(p, zero), one),
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprExp(p, gsMakeDataExprCNat(one)), p),
      (ATerm) gsMakeDataEqn(pql,nil, 
         gsMakeDataExprExp(p, gsMakeDataExprCNat(gsMakeDataExprCDub(f, q))),
         gsMakeDataExprExp(gsMakeDataExprMultIR(f, one, p, p),
           gsMakeDataExprCNat(q))),
      (ATerm) gsMakeDataEqn(pql,nil, 
         gsMakeDataExprExp(p, gsMakeDataExprCNat(gsMakeDataExprCDub(t, q))),
         gsMakeDataExprMultIR(f, one, p,
           gsMakeDataExprExp(gsMakeDataExprMultIR(f, one, p, p),
             gsMakeDataExprCNat(q)))),
      //exponentiation (Nat -> Nat -> Nat)
      (ATerm) gsMakeDataEqn(nl, nil, gsMakeDataExprExp(n, zero),
        gsMakeDataExprCNat(one)),
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprExp(zero, gsMakeDataExprCNat(p)), zero),
      (ATerm) gsMakeDataEqn(pnl, nil,
         gsMakeDataExprExp(gsMakeDataExprCNat(p), n),
         gsMakeDataExprCNat(gsMakeDataExprExp(p, n))),
      //even (Nat -> Bool)
      (ATerm) gsMakeDataEqn(el, nil, gsMakeDataExprEven(zero), t),
      (ATerm) gsMakeDataEqn(el, nil,
        gsMakeDataExprEven(gsMakeDataExprCNat(one)), f),
      (ATerm) gsMakeDataEqn(bpl, nil,
        gsMakeDataExprEven(gsMakeDataExprCNat(gsMakeDataExprCDub(b, p))),
        gsMakeDataExprNot(b)),
      //quotient after division (Pos -> Pos -> Nat)
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprDiv(p, one),
         gsMakeDataExprCNat(p)),
      (ATerm) gsMakeDataEqn(bpl, nil,
         gsMakeDataExprDiv(one, gsMakeDataExprCDub(b, p)),
         zero),
      (ATerm) gsMakeDataEqn(bpql, nil,
         gsMakeDataExprDiv(gsMakeDataExprCDub(b, p), gsMakeDataExprCDub(f, q)),
         gsMakeDataExprDiv(p, q)),
      (ATerm) gsMakeDataEqn(pql,
         gsMakeDataExprLTE(p, q),
         gsMakeDataExprDiv(gsMakeDataExprCDub(f, p), gsMakeDataExprCDub(t, q)),
         zero),
      (ATerm) gsMakeDataEqn(pql,
         gsMakeDataExprLT(q, p),
         gsMakeDataExprDiv(gsMakeDataExprCDub(f, p), gsMakeDataExprCDub(t, q)),
         gsMakeDataExprFirst(
           gsMakeDataExprGDivMod(
             gsMakeDataExprDivMod(p, gsMakeDataExprCDub(t, q)),
             f,
             gsMakeDataExprCDub(t, q)
           )
           //XXX The line above is the normal form of the line below, needed
           //for consistency with lpsrewr.
           //gsMakeDataExprDivMod(gsMakeDataExprCDub(f, p), gsMakeDataExprCDub(t, q))
         )),
      (ATerm) gsMakeDataEqn(pql,
         gsMakeDataExprLTE(p, q),
         gsMakeDataExprDiv(gsMakeDataExprCDub(t, p), gsMakeDataExprCDub(t, q)),
         gsMakeDataExprIf(gsMakeDataExprEq(p, q),
           gsMakeDataExprCNat(one), zero
         )),
      (ATerm) gsMakeDataEqn(pql,
         gsMakeDataExprLT(q, p),
         gsMakeDataExprDiv(gsMakeDataExprCDub(t, p), gsMakeDataExprCDub(t, q)),
         gsMakeDataExprFirst(
           gsMakeDataExprGDivMod(
             gsMakeDataExprDivMod(p, gsMakeDataExprCDub(t, q)),
             t,
             gsMakeDataExprCDub(t, q)
           )
           //XXX The line above is the normal form of the line below, needed
           //for consistency with lpsrewr.
           //gsMakeDataExprDivMod(gsMakeDataExprCDub(t, p), gsMakeDataExprCDub(t, q))
         )),
      //quotient after division (Nat -> Pos -> Nat)
      (ATerm) gsMakeDataEqn(pl, nil, gsMakeDataExprDiv(zero, p), zero),
      (ATerm) gsMakeDataEqn(pql,nil,
         gsMakeDataExprDiv(gsMakeDataExprCNat(p), q),
         gsMakeDataExprDiv(p, q)),
      //remainder after division (Pos -> Pos -> Nat)
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprMod(p, one),
         zero),
      (ATerm) gsMakeDataEqn(bpl, nil,
         gsMakeDataExprMod(one, gsMakeDataExprCDub(b, p)),
         gsMakeDataExprCNat(one)),
      (ATerm) gsMakeDataEqn(bpql, nil,
         gsMakeDataExprMod(gsMakeDataExprCDub(b, p), gsMakeDataExprCDub(f, q)),
         gsMakeDataExprDub(b, gsMakeDataExprMod(p, q))),
      (ATerm) gsMakeDataEqn(pql,
         gsMakeDataExprLTE(p, q),
         gsMakeDataExprMod(gsMakeDataExprCDub(f, p), gsMakeDataExprCDub(t, q)),
         gsMakeDataExprCNat(gsMakeDataExprCDub(f, p))),
      (ATerm) gsMakeDataEqn(pql,
         gsMakeDataExprLT(q, p),
         gsMakeDataExprMod(gsMakeDataExprCDub(f, p), gsMakeDataExprCDub(t, q)),
         gsMakeDataExprLast(
           gsMakeDataExprGDivMod(
             gsMakeDataExprDivMod(p, gsMakeDataExprCDub(t, q)),
             f,
             gsMakeDataExprCDub(t, q)
           )
           //XXX The line above is the normal form of the line below, needed
           //for consistency with lpsrewr.
           //gsMakeDataExprDivMod(gsMakeDataExprCDub(f, p), gsMakeDataExprCDub(t, q))
         )),
      (ATerm) gsMakeDataEqn(pql,
         gsMakeDataExprLTE(p, q),
         gsMakeDataExprMod(gsMakeDataExprCDub(t, p), gsMakeDataExprCDub(t, q)),
         gsMakeDataExprIf(gsMakeDataExprEq(p, q),
           zero, gsMakeDataExprCNat(gsMakeDataExprCDub(t, p))
         )),
      (ATerm) gsMakeDataEqn(pql,
         gsMakeDataExprLT(q, p),
         gsMakeDataExprMod(gsMakeDataExprCDub(t, p), gsMakeDataExprCDub(t, q)),
         gsMakeDataExprLast(
           gsMakeDataExprGDivMod(
             gsMakeDataExprDivMod(p, gsMakeDataExprCDub(t, q)),
             t,
             gsMakeDataExprCDub(t, q)
           )
           //XXX The line above is the normal form of the line below, needed
           //for consistency with lpsrewr.
           //gsMakeDataExprDivMod(gsMakeDataExprCDub(t, p), gsMakeDataExprCDub(t, q))
         )),
      //remainder after division (Nat -> Pos -> Nat)
      (ATerm) gsMakeDataEqn(pl, nil, gsMakeDataExprMod(zero, p), zero),
      (ATerm) gsMakeDataEqn(pql,nil, 
         gsMakeDataExprMod(gsMakeDataExprCNat(p), q),
         gsMakeDataExprMod(p, q))
    ), p_data_decls->data_eqns);
}

void old_impl_sort_int(t_data_decls *p_data_decls)
{
  //Declare sort Int
  p_data_decls->sorts = ATinsert(p_data_decls->sorts, (ATerm) gsMakeSortIdInt());
  //Declare constructors for sort Int
  p_data_decls->cons_ops = ATconcat(ATmakeList2(
      (ATerm) gsMakeOpIdCInt(),
      (ATerm) gsMakeOpIdCNeg()
    ), p_data_decls->cons_ops);
  //Declare operations for sort Int
  ATermAppl se_pos = gsMakeSortExprPos();
  ATermAppl se_nat = gsMakeSortExprNat();
  ATermAppl se_int = gsMakeSortExprInt();
  p_data_decls->ops = ATconcat(ATmakeList(30,
      (ATerm) gsMakeOpIdNat2Int(),
      (ATerm) gsMakeOpIdInt2Nat(),
      (ATerm) gsMakeOpIdPos2Int(),
      (ATerm) gsMakeOpIdInt2Pos(),
      (ATerm) gsMakeOpIdLTE(se_int),
      (ATerm) gsMakeOpIdLT(se_int),
      (ATerm) gsMakeOpIdGTE(se_int),
      (ATerm) gsMakeOpIdGT(se_int),
      (ATerm) gsMakeOpIdMax(se_pos, se_int),
      (ATerm) gsMakeOpIdMax(se_int, se_pos),
      (ATerm) gsMakeOpIdMax(se_nat, se_int),
      (ATerm) gsMakeOpIdMax(se_int, se_nat),
      (ATerm) gsMakeOpIdMax(se_int, se_int),
      (ATerm) gsMakeOpIdMin(se_int),
      (ATerm) gsMakeOpIdAbs(se_int),
      (ATerm) gsMakeOpIdNeg(se_pos),
      (ATerm) gsMakeOpIdNeg(se_nat),
      (ATerm) gsMakeOpIdNeg(se_int),
      (ATerm) gsMakeOpIdSucc(se_int),
      (ATerm) gsMakeOpIdPred(se_nat),
      (ATerm) gsMakeOpIdPred(se_int),
      (ATerm) gsMakeOpIdDub(se_int),
      (ATerm) gsMakeOpIdAdd(se_int, se_int),
      (ATerm) gsMakeOpIdSubt(se_pos),
      (ATerm) gsMakeOpIdSubt(se_nat),
      (ATerm) gsMakeOpIdSubt(se_int),
      (ATerm) gsMakeOpIdMult(se_int),
      (ATerm) gsMakeOpIdDiv(se_int),
      (ATerm) gsMakeOpIdMod(se_int),
      (ATerm) gsMakeOpIdExp(se_int)
    ), p_data_decls->ops);
  //Declare data equations for sort Int
  ATermList el = ATmakeList0();
  ATermAppl nil = gsMakeNil();
  ATermAppl one = gsMakeDataExprC1();
  ATermAppl zero = gsMakeDataExprC0();
  ATermAppl t = gsMakeDataExprTrue();
  ATermAppl f = gsMakeDataExprFalse();
  ATermAppl b = gsMakeDataVarId(gsString2ATermAppl("b"), gsMakeSortExprBool());
  ATermAppl p = gsMakeDataVarId(gsString2ATermAppl("p"), gsMakeSortExprPos());
  ATermAppl q = gsMakeDataVarId(gsString2ATermAppl("q"), gsMakeSortExprPos());
  ATermAppl m = gsMakeDataVarId(gsString2ATermAppl("m"), gsMakeSortExprNat());
  ATermAppl n = gsMakeDataVarId(gsString2ATermAppl("n"), gsMakeSortExprNat());
  ATermAppl x = gsMakeDataVarId(gsString2ATermAppl("x"), gsMakeSortExprInt());
  ATermAppl y = gsMakeDataVarId(gsString2ATermAppl("y"), gsMakeSortExprInt());
  ATermList pl = ATmakeList1((ATerm) p);
  ATermList pql = ATmakeList2((ATerm) p, (ATerm) q);
  ATermList nl = ATmakeList1((ATerm) n);
  ATermList bnl = ATmakeList2((ATerm) n, (ATerm) b);
  ATermList pnl = ATmakeList2((ATerm) n, (ATerm) p);
  ATermList mnl = ATmakeList2((ATerm) n, (ATerm) m);
  ATermList xyl = ATmakeList2((ATerm) x, (ATerm) y);
  p_data_decls->data_eqns = ATconcat(ATmakeList(64,
      //equality (Int -> Int -> Bool)
      (ATerm) gsMakeDataEqn(mnl,nil, 
         gsMakeDataExprEq(gsMakeDataExprCInt(m), gsMakeDataExprCInt(n)),
         gsMakeDataExprEq(m, n)),
      (ATerm) gsMakeDataEqn(pnl,nil, 
         gsMakeDataExprEq(gsMakeDataExprCInt(n), gsMakeDataExprCNeg(p)), f),
      (ATerm) gsMakeDataEqn(pnl,nil, 
         gsMakeDataExprEq(gsMakeDataExprCNeg(p), gsMakeDataExprCInt(n)), f),
      (ATerm) gsMakeDataEqn(pql,nil, 
         gsMakeDataExprEq(gsMakeDataExprCNeg(p), gsMakeDataExprCNeg(q)),
         gsMakeDataExprEq(p, q)),
      //convert Nat to Int (Nat -> Int)
      (ATerm) gsMakeDataEqn(el, nil, gsMakeOpIdNat2Int(), gsMakeOpIdCInt()),
      //convert Int to Nat (Int -> Nat)
      (ATerm) gsMakeDataEqn(nl, nil,
         gsMakeDataExprInt2Nat(gsMakeDataExprCInt(n)), n),
      //convert Pos to Int (Pos -> Int)
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprPos2Int(p), gsMakeDataExprCInt(gsMakeDataExprCNat(p))),
      //convert Int to Pos (Int -> Pos)
      (ATerm) gsMakeDataEqn(nl, nil,
         gsMakeDataExprInt2Pos(gsMakeDataExprCInt(n)),
         gsMakeDataExprNat2Pos(n)),
      //less than or equal (Int -> Int -> Bool)
      (ATerm) gsMakeDataEqn(mnl,nil, 
         gsMakeDataExprLTE(gsMakeDataExprCInt(m), gsMakeDataExprCInt(n)),
         gsMakeDataExprLTE(m, n)),
      (ATerm) gsMakeDataEqn(pnl,nil, 
         gsMakeDataExprLTE(gsMakeDataExprCInt(n), gsMakeDataExprCNeg(p)), f),
      (ATerm) gsMakeDataEqn(pnl,nil, 
         gsMakeDataExprLTE(gsMakeDataExprCNeg(p), gsMakeDataExprCInt(n)), t),
      (ATerm) gsMakeDataEqn(pql,nil, 
         gsMakeDataExprLTE(gsMakeDataExprCNeg(p), gsMakeDataExprCNeg(q)),
         gsMakeDataExprLTE(q, p)),
      //less than (Int -> Int -> Bool)
      (ATerm) gsMakeDataEqn(mnl,nil, 
         gsMakeDataExprLT(gsMakeDataExprCInt(m), gsMakeDataExprCInt(n)),
         gsMakeDataExprLT(m, n)),
      (ATerm) gsMakeDataEqn(pnl,nil, 
         gsMakeDataExprLT(gsMakeDataExprCInt(n), gsMakeDataExprCNeg(p)), f),
      (ATerm) gsMakeDataEqn(pnl,nil, 
         gsMakeDataExprLT(gsMakeDataExprCNeg(p), gsMakeDataExprCInt(n)), t),
      (ATerm) gsMakeDataEqn(pql,nil, 
         gsMakeDataExprLT(gsMakeDataExprCNeg(p), gsMakeDataExprCNeg(q)),
         gsMakeDataExprLT(q, p)),
      //greater than or equal (Int -> Int -> Bool)
      (ATerm) gsMakeDataEqn(xyl,nil,
         gsMakeDataExprGTE(x, y), gsMakeDataExprLTE(y, x)),
      //greater than (Int -> Int -> Bool)
      (ATerm) gsMakeDataEqn(xyl,nil,
         gsMakeDataExprGT(x, y), gsMakeDataExprLT(y, x)),
      //maximum (Pos -> Int -> Pos)
      (ATerm) gsMakeDataEqn(pnl,nil,
         gsMakeDataExprMax(p, gsMakeDataExprCInt(n)),
         //gsMakeDataExprMax(p, n)),
         gsMakeDataExprMax(p, n)),
      (ATerm) gsMakeDataEqn(pql,nil,
         gsMakeDataExprMax(p, gsMakeDataExprCNeg(q)), p),
      //maximum (Int -> Pos -> Pos)
      (ATerm) gsMakeDataEqn(pnl,nil,
         gsMakeDataExprMax(gsMakeDataExprCInt(n), p),
         gsMakeDataExprMax(n, p)),
      (ATerm) gsMakeDataEqn(pql,nil,
         gsMakeDataExprMax(gsMakeDataExprCNeg(q), p), p),
      //maximum (Nat -> Int -> Nat)
      (ATerm) gsMakeDataEqn(mnl,nil,
         gsMakeDataExprMax(m, gsMakeDataExprCInt(n)),
         //gsMakeDataExprMax(m, n)),
         gsMakeDataExprIf(gsMakeDataExprLTE(m, n), n, m)),
      (ATerm) gsMakeDataEqn(pnl,nil,
         gsMakeDataExprMax(n, gsMakeDataExprCNeg(p)), n),
      //maximum (Int -> Nat -> Nat)
      (ATerm) gsMakeDataEqn(mnl,nil,
         gsMakeDataExprMax(gsMakeDataExprCInt(m), n),
         //gsMakeDataExprMax(m, n)),
         gsMakeDataExprIf(gsMakeDataExprLTE(m, n), n, m)),
      (ATerm) gsMakeDataEqn(pnl,nil,
         gsMakeDataExprMax(gsMakeDataExprCNeg(p), n), n),
      //maximum (Int -> Int -> Int)
      (ATerm) gsMakeDataEqn(xyl,nil, gsMakeDataExprMax(x, y),
         gsMakeDataExprIf(gsMakeDataExprLTE(x, y), y, x)),
      //minimum (Int -> Int -> Int)
      (ATerm) gsMakeDataEqn(xyl,nil, gsMakeDataExprMin(x, y),
         gsMakeDataExprIf(gsMakeDataExprLTE(x, y), x, y)),
      //absolute value (Int -> Nat)
      (ATerm) gsMakeDataEqn(nl,nil,
         gsMakeDataExprAbs(gsMakeDataExprCInt(n)), n),
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprAbs(gsMakeDataExprCNeg(p)),
         gsMakeDataExprCNat(p)),
      //negation (Pos -> Int)
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprNeg(p), gsMakeDataExprCNeg(p)),
      //negation (Nat -> Int)
      (ATerm) gsMakeDataEqn(el,nil,
         gsMakeDataExprNeg(zero), gsMakeDataExprCInt(zero)),
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprNeg(gsMakeDataExprCNat(p)),
         gsMakeDataExprCNeg(p)),
      //negation (Int -> Int)
      (ATerm) gsMakeDataEqn(nl,nil,
         gsMakeDataExprNeg(gsMakeDataExprCInt(n)), gsMakeDataExprNeg(n)),
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprNeg(gsMakeDataExprCNeg(p)),
         gsMakeDataExprCInt(gsMakeDataExprCNat(p))),
      //successor (Int -> Int)
      (ATerm) gsMakeDataEqn(nl,nil,
         gsMakeDataExprSucc(gsMakeDataExprCInt(n)),
         gsMakeDataExprCInt(gsMakeDataExprCNat(gsMakeDataExprSucc(n)))),
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprSucc(gsMakeDataExprCNeg(p)),
           gsMakeDataExprNeg(gsMakeDataExprPred(p))),      
      //predecessor (Nat -> Int)
      (ATerm) gsMakeDataEqn(el,nil,
         gsMakeDataExprPred(zero), gsMakeDataExprCNeg(one)),
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprPred(gsMakeDataExprCNat(p)),
         gsMakeDataExprCInt(gsMakeDataExprPred(p))),
      //predecessor (Int -> Int)
      (ATerm) gsMakeDataEqn(nl,nil,
         gsMakeDataExprPred(gsMakeDataExprCInt(n)), gsMakeDataExprPred(n)),
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprPred(gsMakeDataExprCNeg(p)),
         gsMakeDataExprCNeg(gsMakeDataExprSucc(p))),
      //double (Bool -> Int -> Int)
      (ATerm) gsMakeDataEqn(bnl,nil,
         gsMakeDataExprDub(b, gsMakeDataExprCInt(n)),
         gsMakeDataExprCInt(gsMakeDataExprDub(b, n))),
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprDub(f, gsMakeDataExprCNeg(p)),
         gsMakeDataExprCNeg(gsMakeDataExprCDub(f, p))),
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprDub(t, gsMakeDataExprCNeg(p)),
         //gsMakeDataExprNeg(gsMakeDataExprPred(gsMakeDataExprCDub(f, p)))),
         gsMakeDataExprNeg(gsMakeDataExprDub(t, gsMakeDataExprPred(p)))),
      //addition (Int -> Int -> Int)
      (ATerm) gsMakeDataEqn(mnl, nil,
         gsMakeDataExprAdd(gsMakeDataExprCInt(m), gsMakeDataExprCInt(n)),
         gsMakeDataExprCInt(gsMakeDataExprAdd(m, n))),
      (ATerm) gsMakeDataEqn(pnl, nil,
         gsMakeDataExprAdd(gsMakeDataExprCInt(n), gsMakeDataExprCNeg(p)),
         gsMakeDataExprSubt(n, gsMakeDataExprCNat(p))),
      (ATerm) gsMakeDataEqn(pnl, nil,
         gsMakeDataExprAdd(gsMakeDataExprCNeg(p), gsMakeDataExprCInt(n)),
         gsMakeDataExprSubt(n, gsMakeDataExprCNat(p))),
      (ATerm) gsMakeDataEqn(pql, nil,
         gsMakeDataExprAdd(gsMakeDataExprCNeg(p), gsMakeDataExprCNeg(q)),
         gsMakeDataExprCNeg(gsMakeDataExprAddC(f, p, q))),
      //subtraction (Pos -> Pos -> Int)
      (ATerm) gsMakeDataEqn(pql,
         //gsMakeDataExprGTE(p, q),
         gsMakeDataExprLTE(q, p),
         gsMakeDataExprSubt(p, q),
         //gsMakeDataExprCInt(gsMakeDataExprGTESubt(p, q))),
         gsMakeDataExprCInt(gsMakeDataExprGTESubtB(f, p, q))),
      (ATerm) gsMakeDataEqn(pql,
         gsMakeDataExprLT(p, q),
         gsMakeDataExprSubt(p, q),
         //gsMakeDataExprNeg(gsMakeDataExprGTESubt(q, p))),
         gsMakeDataExprNeg(gsMakeDataExprGTESubtB(f, q, p))),
      //subtraction (Nat -> Nat -> Int)
      (ATerm) gsMakeDataEqn(mnl,
         //gsMakeDataExprGTE(m, n),
         gsMakeDataExprLTE(n, m),
         gsMakeDataExprSubt(m, n),
         gsMakeDataExprCInt(gsMakeDataExprGTESubt(m, n))),
      (ATerm) gsMakeDataEqn(mnl,
         gsMakeDataExprLT(m, n),
         gsMakeDataExprSubt(m, n),
         gsMakeDataExprNeg(gsMakeDataExprGTESubt(n, m))),
      //subtraction (Int -> Int -> Int)
      (ATerm) gsMakeDataEqn(xyl, nil,
         gsMakeDataExprSubt(x, y), gsMakeDataExprAdd(x, gsMakeDataExprNeg(y))),
      //multiplication (Int -> Int -> Int)
      (ATerm) gsMakeDataEqn(mnl, nil,
         gsMakeDataExprMult(gsMakeDataExprCInt(m), gsMakeDataExprCInt(n)),
         gsMakeDataExprCInt(gsMakeDataExprMult(m, n))),
      (ATerm) gsMakeDataEqn(pnl, nil,
         gsMakeDataExprMult(gsMakeDataExprCInt(n), gsMakeDataExprCNeg(p)),
         gsMakeDataExprNeg(gsMakeDataExprMult(gsMakeDataExprCNat(p), n))),
      (ATerm) gsMakeDataEqn(pnl, nil,
         gsMakeDataExprMult(gsMakeDataExprCNeg(p), gsMakeDataExprCInt(n)),
         gsMakeDataExprNeg(gsMakeDataExprMult(gsMakeDataExprCNat(p), n))),
      (ATerm) gsMakeDataEqn(pql, nil,
         gsMakeDataExprMult(gsMakeDataExprCNeg(p), gsMakeDataExprCNeg(q)),
         gsMakeDataExprCInt(gsMakeDataExprCNat(gsMakeDataExprMult(p, q)))),
      //quotient after division (Int -> Pos -> Int)
      (ATerm) gsMakeDataEqn(pnl, nil,
         gsMakeDataExprDiv(gsMakeDataExprCInt(n), p),
         gsMakeDataExprCInt(gsMakeDataExprDiv(n, p))),
      (ATerm) gsMakeDataEqn(pql, nil,
         gsMakeDataExprDiv(gsMakeDataExprCNeg(p), q),
         gsMakeDataExprCNeg(gsMakeDataExprSucc(
           gsMakeDataExprDiv(gsMakeDataExprPred(p), q)))),
      //remainder after division (Int -> Pos -> Nat)
      (ATerm) gsMakeDataEqn(pnl, nil,
         gsMakeDataExprMod(gsMakeDataExprCInt(n), p),
         gsMakeDataExprMod(n, p)),
      (ATerm) gsMakeDataEqn(pql, nil,
         gsMakeDataExprMod(gsMakeDataExprCNeg(p), q),
         gsMakeDataExprInt2Nat(gsMakeDataExprSubt(q, gsMakeDataExprSucc(
           gsMakeDataExprMod(gsMakeDataExprPred(p), q))))),
      //exponentiation (Int -> Nat -> Int)
      (ATerm) gsMakeDataEqn(mnl,nil,
         gsMakeDataExprExp(gsMakeDataExprCInt(m), n),
         gsMakeDataExprCInt(gsMakeDataExprExp(m, n))),
      (ATerm) gsMakeDataEqn(pnl,
         gsMakeDataExprEven(n),
         gsMakeDataExprExp(gsMakeDataExprCNeg(p), n),
         gsMakeDataExprCInt(gsMakeDataExprCNat(gsMakeDataExprExp(p, n)))),
      (ATerm) gsMakeDataEqn(pnl,
         gsMakeDataExprNot(gsMakeDataExprEven(n)),
         gsMakeDataExprExp(gsMakeDataExprCNeg(p), n),
         gsMakeDataExprCNeg(gsMakeDataExprExp(p, n)))
    ), p_data_decls->data_eqns);
  //add implementation of sort Nat, if necessary
  if (ATindexOf(p_data_decls->sorts, (ATerm) gsMakeSortIdNat(), 0) == -1) {
    old_impl_sort_nat(p_data_decls);
  }
}

void old_impl_sort_real(t_data_decls *p_data_decls)
{
  //Declare sort Real
  p_data_decls->sorts = ATinsert(p_data_decls->sorts, (ATerm) gsMakeSortIdReal());
  //Declare constructors for sort Real
  //Declare operations for sort Real
  ATermAppl se_real = gsMakeSortExprReal();
  p_data_decls->ops = ATconcat(ATmakeList(21,
      (ATerm) gsMakeOpIdCReal(),
      (ATerm) gsMakeOpIdPos2Real(),
      (ATerm) gsMakeOpIdNat2Real(),
      (ATerm) gsMakeOpIdInt2Real(),
      (ATerm) gsMakeOpIdReal2Pos(),
      (ATerm) gsMakeOpIdReal2Nat(),
      (ATerm) gsMakeOpIdReal2Int(),
      (ATerm) gsMakeOpIdLTE(se_real),
      (ATerm) gsMakeOpIdLT(se_real),
      (ATerm) gsMakeOpIdGTE(se_real),
      (ATerm) gsMakeOpIdGT(se_real),
      (ATerm) gsMakeOpIdMax(se_real, se_real),
      (ATerm) gsMakeOpIdMin(se_real),
      (ATerm) gsMakeOpIdAbs(se_real),
      (ATerm) gsMakeOpIdNeg(se_real),
      (ATerm) gsMakeOpIdSucc(se_real),
      (ATerm) gsMakeOpIdPred(se_real),
      (ATerm) gsMakeOpIdAdd(se_real, se_real),
      (ATerm) gsMakeOpIdSubt(se_real),
      (ATerm) gsMakeOpIdMult(se_real),
      (ATerm) gsMakeOpIdExp(se_real)
    ), p_data_decls->ops);
  //Declare data equations for sort Int
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
  p_data_decls->data_eqns = ATconcat(ATmakeList(21,
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
    ), p_data_decls->data_eqns);
  //add implementation of sort Int, if necessary
  if (ATindexOf(p_data_decls->sorts, (ATerm) gsMakeSortIdInt(), 0) == -1) {
    std::cerr << "implement integers" << std::endl;
    old_impl_sort_int(p_data_decls);
  }
}

void implement_bool_test()
{
  t_data_decls data_decls_old;
  t_data_decls data_decls_new;
  initialize_data_decls(&data_decls_old);
  initialize_data_decls(&data_decls_new);
  old_impl_sort_bool(&data_decls_old);
  impl_sort_bool(&data_decls_new);

  std::cerr << "old sorts: " << aterm_list(data_decls_old.sorts) << std::endl;
  std::cerr << "new sorts: " << aterm_list(data_decls_new.sorts) << std::endl;
  std::cerr << "old constructors: " << aterm_list(data_decls_old.cons_ops) << std::endl;
  std::cerr << "new constructors: " << aterm_list(data_decls_new.cons_ops) << std::endl;
  std::cerr << "old functions: " << aterm_list(data_decls_old.ops) << std::endl;
  std::cerr << "new functions: " << aterm_list(data_decls_new.ops) << std::endl;
  std::cerr << "old equations: " << aterm_list(data_decls_old.data_eqns) << std::endl;
  std::cerr << "new equations: " << aterm_list(data_decls_new.data_eqns) << std::endl;

  BOOST_CHECK(data_decls_old.sorts     == data_decls_new.sorts);
  BOOST_CHECK(data_decls_old.cons_ops  == data_decls_new.cons_ops);
  BOOST_CHECK(data_decls_old.ops       == data_decls_new.ops);
  BOOST_CHECK(data_decls_old.data_eqns == data_decls_new.data_eqns);
}

void implement_pos_test()
{
  t_data_decls data_decls_old;
  t_data_decls data_decls_new;
  initialize_data_decls(&data_decls_old);
  initialize_data_decls(&data_decls_new);
  old_impl_sort_pos(&data_decls_old);
  impl_sort_pos(&data_decls_new);

  std::cerr << "old sorts: " << aterm_list(data_decls_old.sorts) << std::endl;
  std::cerr << "new sorts: " << aterm_list(data_decls_new.sorts) << std::endl;
  std::cerr << "old constructors: " << aterm_list(data_decls_old.cons_ops) << std::endl;
  std::cerr << "new constructors: " << aterm_list(data_decls_new.cons_ops) << std::endl;
  std::cerr << "old functions: " << aterm_list(data_decls_old.ops) << std::endl;
  std::cerr << "new functions: " << aterm_list(data_decls_new.ops) << std::endl;
  std::cerr << "old equations:" << std::endl;
  for (aterm_list::iterator i = aterm_list(data_decls_old.data_eqns).begin(); i != aterm_list(data_decls_old.data_eqns).end(); ++i)
  {
    std::cerr << *i << std::endl;
  }
  std::cerr << "new equations:" << std::endl;
  for (aterm_list::iterator i = aterm_list(data_decls_new.data_eqns).begin(); i != aterm_list(data_decls_new.data_eqns).end(); ++i)
  {
    std::cerr << *i << std::endl;
  }

  BOOST_CHECK(data_decls_old.sorts     == data_decls_new.sorts);
  BOOST_CHECK(data_decls_old.cons_ops  == data_decls_new.cons_ops);
  BOOST_CHECK(data_decls_old.ops       == data_decls_new.ops);
  BOOST_CHECK(data_decls_old.data_eqns == data_decls_new.data_eqns);
}

void implement_nat_test()
{
  t_data_decls data_decls_old;
  t_data_decls data_decls_new;
  initialize_data_decls(&data_decls_old);
  initialize_data_decls(&data_decls_new);
  old_impl_sort_nat(&data_decls_old);
  impl_sort_nat(&data_decls_new);

  std::cerr << "old sorts: " << aterm_list(data_decls_old.sorts) << std::endl;
  std::cerr << "new sorts: " << aterm_list(data_decls_new.sorts) << std::endl;
  std::cerr << "old constructors: " << aterm_list(data_decls_old.cons_ops) << std::endl;
  std::cerr << "new constructors: " << aterm_list(data_decls_new.cons_ops) << std::endl;
  std::cerr << "old functions:" << std::endl;
  for (aterm_list::iterator i = aterm_list(data_decls_old.ops).begin(); i != aterm_list(data_decls_old.data_eqns).end(); ++i)
  {
    std::cerr << *i << std::endl;
  }
  std::cerr << "new functions:" << std::endl;
  for (aterm_list::iterator i = aterm_list(data_decls_new.ops).begin(); i != aterm_list(data_decls_new.data_eqns).end(); ++i)
  {
    std::cerr << *i << std::endl;
  }
  std::cerr << "old equations:" << std::endl;
  for (aterm_list::iterator i = aterm_list(data_decls_old.data_eqns).begin(); i != aterm_list(data_decls_old.data_eqns).end(); ++i)
  {
    std::cerr << *i << std::endl;
  }
  std::cerr << "new equations:" << std::endl;
  for (aterm_list::iterator i = aterm_list(data_decls_new.data_eqns).begin(); i != aterm_list(data_decls_new.data_eqns).end(); ++i)
  {
    std::cerr << *i << std::endl;
  }

  BOOST_CHECK(data_decls_old.sorts     == data_decls_new.sorts);
  BOOST_CHECK(data_decls_old.cons_ops  == data_decls_new.cons_ops);
  BOOST_CHECK(data_decls_old.ops       == data_decls_new.ops);
  BOOST_CHECK(data_decls_old.data_eqns == data_decls_new.data_eqns);
}

void implement_int_test()
{
  t_data_decls data_decls_old;
  t_data_decls data_decls_new;
  initialize_data_decls(&data_decls_old);
  initialize_data_decls(&data_decls_new);
  old_impl_sort_int(&data_decls_old);
  impl_sort_int(&data_decls_new);

  std::cerr << "old sorts: " << aterm_list(data_decls_old.sorts) << std::endl;
  std::cerr << "new sorts: " << aterm_list(data_decls_new.sorts) << std::endl;
  std::cerr << "old constructors: " << aterm_list(data_decls_old.cons_ops) << std::endl;
  std::cerr << "new constructors: " << aterm_list(data_decls_new.cons_ops) << std::endl;
  std::cerr << "old functions:" << std::endl;
  for (aterm_list::iterator i = aterm_list(data_decls_old.ops).begin(); i != aterm_list(data_decls_old.data_eqns).end(); ++i)
  {
    std::cerr << *i << std::endl;
  }
  std::cerr << "new functions:" << std::endl;
  for (aterm_list::iterator i = aterm_list(data_decls_new.ops).begin(); i != aterm_list(data_decls_new.data_eqns).end(); ++i)
  {
    std::cerr << *i << std::endl;
  }
  std::cerr << "old equations:" << std::endl;
  for (aterm_list::iterator i = aterm_list(data_decls_old.data_eqns).begin(); i != aterm_list(data_decls_old.data_eqns).end(); ++i)
  {
    std::cerr << *i << std::endl;
  }
  std::cerr << "new equations:" << std::endl;
  for (aterm_list::iterator i = aterm_list(data_decls_new.data_eqns).begin(); i != aterm_list(data_decls_new.data_eqns).end(); ++i)
  {
    std::cerr << *i << std::endl;
  }

  BOOST_CHECK(data_decls_old.sorts     == data_decls_new.sorts);
  BOOST_CHECK(data_decls_old.cons_ops  == data_decls_new.cons_ops);
  BOOST_CHECK(data_decls_old.ops       == data_decls_new.ops);
  BOOST_CHECK(data_decls_old.data_eqns == data_decls_new.data_eqns);
}

void implement_real_test()
{
  t_data_decls data_decls_old;
  t_data_decls data_decls_new;
  initialize_data_decls(&data_decls_old);
  initialize_data_decls(&data_decls_new);
  old_impl_sort_real(&data_decls_old);
  impl_sort_real(&data_decls_new);

  std::cerr << "old sorts: " << aterm_list(data_decls_old.sorts) << std::endl;
  std::cerr << "new sorts: " << aterm_list(data_decls_new.sorts) << std::endl;
  std::cerr << "old constructors: " << aterm_list(data_decls_old.cons_ops) << std::endl;
  std::cerr << "new constructors: " << aterm_list(data_decls_new.cons_ops) << std::endl;
  std::cerr << "old functions:" << std::endl;
  for (aterm_list::iterator i = aterm_list(data_decls_old.ops).begin(); i != aterm_list(data_decls_old.data_eqns).end(); ++i)
  {
    std::cerr << *i << std::endl;
  }
  std::cerr << "new functions:" << std::endl;
  for (aterm_list::iterator i = aterm_list(data_decls_new.ops).begin(); i != aterm_list(data_decls_new.data_eqns).end(); ++i)
  {
    std::cerr << *i << std::endl;
  }
  std::cerr << "old equations:" << std::endl;
  for (aterm_list::iterator i = aterm_list(data_decls_old.data_eqns).begin(); i != aterm_list(data_decls_old.data_eqns).end(); ++i)
  {
    std::cerr << *i << std::endl;
  }
  std::cerr << "new equations:" << std::endl;
  for (aterm_list::iterator i = aterm_list(data_decls_new.data_eqns).begin(); i != aterm_list(data_decls_new.data_eqns).end(); ++i)
  {
    std::cerr << *i << std::endl;
  }

  BOOST_CHECK(data_decls_old.sorts     == data_decls_new.sorts);
  BOOST_CHECK(data_decls_old.cons_ops  == data_decls_new.cons_ops);
  BOOST_CHECK(data_decls_old.ops       == data_decls_new.ops);
  BOOST_CHECK(data_decls_old.data_eqns == data_decls_new.data_eqns);
}

void implement_data_specification_test()
{
  const std::string text(
    "sort S;\n"
    "cons s:S;\n"
    "map f:S -> List(S);\n"
  );

  data::data_specification spec(data::parse_data_specification(text));
  atermpp::aterm_appl impl_spec = data::detail::implement_data_specification(spec);
  atermpp::aterm_appl old_impl_spec = data::parse_data_specification_and_implement(text);

  BOOST_CHECK(impl_spec == old_impl_spec);
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv);

  implement_bool_test();
  implement_pos_test();
  implement_nat_test();
  implement_int_test();
  implement_real_test();
  implement_data_specification_test();

  return EXIT_SUCCESS;
}
