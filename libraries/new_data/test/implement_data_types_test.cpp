// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file implement_data_types.cpp
/// \brief Regression test for new_data implementation.

#include <iostream>
#include <boost/range/iterator_range.hpp>
#include <boost/test/minimal.hpp>

#include "mcrl2/utilities/aterm_ext.h"
#include "mcrl2/new_data/parser.h"
#include "mcrl2/core/print.h"
#include "mcrl2/new_data/detail/implement_data_types.h"
#include "mcrl2/new_data/detail/data_implementation_concrete.h"
#include "mcrl2/core/detail/struct.h"

#include "mcrl2/new_data/basic_sort.h"
#include "mcrl2/new_data/list.h"
#include "mcrl2/new_data/set.h"
#include "mcrl2/new_data/bag.h"

using namespace atermpp;
using namespace mcrl2;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace mcrl2::new_data;
using namespace mcrl2::new_data::detail;
using namespace mcrl2::utilities;

/* Old new_data implementation functions, plainly copied over */
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
  //Declare new_data equations for sort Bool
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
  p_data_decls->ops = ATconcat(ATmakeList(8,
      (ATerm) gsMakeOpIdMax(se_pos, se_pos),
      (ATerm) gsMakeOpIdMin(se_pos),
      (ATerm) gsMakeOpIdAbs(se_pos),
      (ATerm) gsMakeOpIdSucc(se_pos),
      (ATerm) gsMakeOpIdAdd(se_pos, se_pos),
      (ATerm) gsMakeOpIdAddC(),
      (ATerm) gsMakeOpIdMult(se_pos),
      (ATerm) gsMakeOpIdMultIR()
    ), p_data_decls->ops);
  //Declare new_data equations for sort Pos
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
  p_data_decls->data_eqns = ATconcat(ATmakeList(39,
      //equality (Pos # Pos -> Bool)
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
      //less than (Pos # Pos -> Bool)
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
  std::clog << "impl_sort_natpair" << std::endl;
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
  //Declare new_data equations for sort Int
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
  p_data_decls->data_eqns = ATconcat(ATmakeList(12,
      //equality (NatPair -> NatPair -> Bool)
      (ATerm) gsMakeDataEqn(mnm_n_l,nil,
         gsMakeDataExprEq(gsMakeDataExprCPair(m, n), gsMakeDataExprCPair(m_, n_)),
         gsMakeDataExprAnd(gsMakeDataExprEq(m, m_), gsMakeDataExprEq(n, n_))),
      //less than (NatPair # NatPair -> Bool)
      (ATerm) gsMakeDataEqn(mnm_n_l,nil,
         gsMakeDataExprLT(gsMakeDataExprCPair(m, n), gsMakeDataExprCPair(m_, n_)),
         gsMakeDataExprOr(gsMakeDataExprLT(m, m_), gsMakeDataExprAnd(gsMakeDataExprEq(m, m_), gsMakeDataExprLT(n, n_)))),
      //less than or equal (NatPair # NatPair -> Bool)
      (ATerm) gsMakeDataEqn(mnm_n_l,nil,
         gsMakeDataExprLTE(gsMakeDataExprCPair(m, n), gsMakeDataExprCPair(m_, n_)),
         gsMakeDataExprOr(gsMakeDataExprLT(m, m_), gsMakeDataExprAnd(gsMakeDataExprEq(m, m_), gsMakeDataExprLTE(n, n_)))),
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
  std::clog << "end impl_sort_natpair" << std::endl;
}

void old_impl_sort_nat(t_data_decls *p_data_decls)
{
  std::clog << "impl_sort_nat" << std::endl;
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
  p_data_decls->ops = ATconcat(ATmakeList(30,
      (ATerm) gsMakeOpIdPos2Nat(),
      (ATerm) gsMakeOpIdNat2Pos(),
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
      (ATerm) gsMakeOpIdEven(),
      (ATerm) gsMakeOpIdMonus(),
      (ATerm) gsMakeOpIdSwapZero(),
      (ATerm) gsMakeOpIdSwapZeroAdd(),
      (ATerm) gsMakeOpIdSwapZeroMin(),
      (ATerm) gsMakeOpIdSwapZeroMonus(),
      (ATerm) gsMakeOpIdSwapZeroLTE()
    ), p_data_decls->ops);
  //Declare new_data equations for sort Nat
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
  p_data_decls->data_eqns = ATconcat(ATmakeList(99,
      //equality (Nat -> Nat -> Bool)
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprEq(zero, gsMakeDataExprCNat(p)), f),
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprEq(gsMakeDataExprCNat(p), zero), f),
      (ATerm) gsMakeDataEqn(pql,nil,
         gsMakeDataExprEq(gsMakeDataExprCNat(p), gsMakeDataExprCNat(q)),
         gsMakeDataExprEq(p, q)),
      //less than (Nat -> Nat -> Bool)
      (ATerm) gsMakeDataEqn(nl, nil, gsMakeDataExprLT(n, zero), f),
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprLT(zero, gsMakeDataExprCNat(p)), t),
      (ATerm) gsMakeDataEqn(pql,nil,
         gsMakeDataExprLT(gsMakeDataExprCNat(p), gsMakeDataExprCNat(q)),
         gsMakeDataExprLT(p, q)),
      //less than or equal (Nat -> Nat -> Bool)
      (ATerm) gsMakeDataEqn(nl, nil, gsMakeDataExprLTE(zero, n), t),
      (ATerm) gsMakeDataEqn(pl, nil,
         gsMakeDataExprLTE(gsMakeDataExprCNat(p), zero), f),
      (ATerm) gsMakeDataEqn(pql,nil,
         gsMakeDataExprLTE(gsMakeDataExprCNat(p), gsMakeDataExprCNat(q)),
         gsMakeDataExprLTE(p, q)),
      //convert Pos to Nat (Pos -> Nat)
      (ATerm) gsMakeDataEqn(el,nil, gsMakeOpIdPos2Nat(), gsMakeOpIdCNat()),
      //convert Nat to Pos (Nat -> Pos)
      (ATerm) gsMakeDataEqn(pl,nil,
         gsMakeDataExprNat2Pos(gsMakeDataExprCNat(p)), p),
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
         gsMakeDataExprMod(p, q)),
      //monus (Nat # Nat -> Nat)
      (ATerm) gsMakeDataEqn(mnl,
         gsMakeDataExprLTE(m, n),
         gsMakeDataExprMonus(m, n),
         zero),
      (ATerm) gsMakeDataEqn(mnl,
         //gsMakeDataExprGTE(m, n),
         gsMakeDataExprLT(n, m),
         gsMakeDataExprMonus(m, n),
         gsMakeDataExprGTESubt(m, n)),
      //swapzero (Nat # Nat -> Nat)
      (ATerm) gsMakeDataEqn(ml,nil, 
         gsMakeDataExprSwapZero(m, zero),
         m),
      (ATerm) gsMakeDataEqn(nl,nil, 
         gsMakeDataExprSwapZero(zero, n),
         n),
      (ATerm) gsMakeDataEqn(pl,nil, 
         gsMakeDataExprSwapZero(gsMakeDataExprCNat(p), gsMakeDataExprCNat(p)),
         zero),
      (ATerm) gsMakeDataEqn(pql, 
         gsMakeDataExprNeq(p, q),
         gsMakeDataExprSwapZero(gsMakeDataExprCNat(p), gsMakeDataExprCNat(q)),
         gsMakeDataExprCNat(q)),
      //swapzero_add (Nat # Nat # Nat # Nat -> Nat)
      (ATerm) gsMakeDataEqn(mnl,nil, 
         gsMakeDataExprSwapZeroAdd(zero, zero, m, n),
         gsMakeDataExprAdd(m, n)),
      (ATerm) gsMakeDataEqn(pml,nil, 
         gsMakeDataExprSwapZeroAdd(gsMakeDataExprCNat(p), zero, m, zero),
         m),
      (ATerm) gsMakeDataEqn(pqml,nil, 
         gsMakeDataExprSwapZeroAdd(gsMakeDataExprCNat(p), zero, m, gsMakeDataExprCNat(q)),
         gsMakeDataExprSwapZero(gsMakeDataExprCNat(p), gsMakeDataExprAdd(gsMakeDataExprSwapZero(gsMakeDataExprCNat(p), m), gsMakeDataExprCNat(q)))),
      (ATerm) gsMakeDataEqn(pnl,nil, 
         gsMakeDataExprSwapZeroAdd(zero, gsMakeDataExprCNat(p), zero, n),
         n),
      (ATerm) gsMakeDataEqn(pqnl,nil, 
         gsMakeDataExprSwapZeroAdd(zero, gsMakeDataExprCNat(p), gsMakeDataExprCNat(q), n),
         gsMakeDataExprSwapZero(gsMakeDataExprCNat(p), gsMakeDataExprAdd(gsMakeDataExprCNat(q), gsMakeDataExprSwapZero(gsMakeDataExprCNat(p), n)))),
      (ATerm) gsMakeDataEqn(pqmnl,nil, 
         gsMakeDataExprSwapZeroAdd(gsMakeDataExprCNat(p), gsMakeDataExprCNat(q), m, n),
         gsMakeDataExprSwapZero(
           gsMakeDataExprAdd(gsMakeDataExprCNat(p),gsMakeDataExprCNat(q)),
           gsMakeDataExprAdd(gsMakeDataExprSwapZero(gsMakeDataExprCNat(p), m), gsMakeDataExprSwapZero(gsMakeDataExprCNat(q), n)))),
      //swapzero_min (Nat # Nat # Nat # Nat -> Nat)
      (ATerm) gsMakeDataEqn(mnl,nil, 
         gsMakeDataExprSwapZeroMin(zero, zero, m, n),
         gsMakeDataExprMin(m, n)),
      (ATerm) gsMakeDataEqn(pml,nil, 
         gsMakeDataExprSwapZeroMin(gsMakeDataExprCNat(p), zero, m, zero),
         zero),
      (ATerm) gsMakeDataEqn(pqml,nil, 
         gsMakeDataExprSwapZeroMin(gsMakeDataExprCNat(p), zero, m, gsMakeDataExprCNat(q)),
         gsMakeDataExprMin(gsMakeDataExprSwapZero(gsMakeDataExprCNat(p), m), gsMakeDataExprCNat(q))),
      (ATerm) gsMakeDataEqn(pnl,nil, 
         gsMakeDataExprSwapZeroMin(zero, gsMakeDataExprCNat(p), zero, n),
         zero),
      (ATerm) gsMakeDataEqn(pqnl,nil, 
         gsMakeDataExprSwapZeroMin(zero, gsMakeDataExprCNat(p), gsMakeDataExprCNat(q), n),
         gsMakeDataExprMin(gsMakeDataExprCNat(q), gsMakeDataExprSwapZero(gsMakeDataExprCNat(p), n))),
      (ATerm) gsMakeDataEqn(pqmnl,nil, 
         gsMakeDataExprSwapZeroMin(gsMakeDataExprCNat(p), gsMakeDataExprCNat(q), m, n),
         gsMakeDataExprSwapZero(
           gsMakeDataExprMin(gsMakeDataExprCNat(p),gsMakeDataExprCNat(q)),
           gsMakeDataExprMin(gsMakeDataExprSwapZero(gsMakeDataExprCNat(p), m), gsMakeDataExprSwapZero(gsMakeDataExprCNat(q), n)))),
      //swapzero_monus (Nat # Nat # Nat # Nat -> Nat)
      (ATerm) gsMakeDataEqn(mnl,nil, 
         gsMakeDataExprSwapZeroMonus(zero, zero, m, n),
         gsMakeDataExprMonus(m, n)),
      (ATerm) gsMakeDataEqn(pml,nil, 
         gsMakeDataExprSwapZeroMonus(gsMakeDataExprCNat(p), zero, m, zero),
         m),
      (ATerm) gsMakeDataEqn(pqml,nil, 
         gsMakeDataExprSwapZeroMonus(gsMakeDataExprCNat(p), zero, m, gsMakeDataExprCNat(q)),
         gsMakeDataExprSwapZero(gsMakeDataExprCNat(p), gsMakeDataExprMonus(gsMakeDataExprSwapZero(gsMakeDataExprCNat(p), m), gsMakeDataExprCNat(q)))),
      (ATerm) gsMakeDataEqn(pnl,nil, 
         gsMakeDataExprSwapZeroMonus(zero, gsMakeDataExprCNat(p), zero, n),
         zero),
      (ATerm) gsMakeDataEqn(pqnl,nil, 
         gsMakeDataExprSwapZeroMonus(zero, gsMakeDataExprCNat(p), gsMakeDataExprCNat(q), n),
         gsMakeDataExprMonus(gsMakeDataExprCNat(q), gsMakeDataExprSwapZero(gsMakeDataExprCNat(p), n))),
      (ATerm) gsMakeDataEqn(pqmnl,nil, 
         gsMakeDataExprSwapZeroMonus(gsMakeDataExprCNat(p), gsMakeDataExprCNat(q), m, n),
         gsMakeDataExprSwapZero(
           gsMakeDataExprMonus(gsMakeDataExprCNat(p),gsMakeDataExprCNat(q)),
           gsMakeDataExprMonus(gsMakeDataExprSwapZero(gsMakeDataExprCNat(p), m), gsMakeDataExprSwapZero(gsMakeDataExprCNat(q), n)))),
      //swapzero_lte (Nat # Nat # Nat -> Bool)
      (ATerm) gsMakeDataEqn(mnl,nil, 
         gsMakeDataExprSwapZeroLTE(zero, m, n),
         gsMakeDataExprLTE(m, n)),
      (ATerm) gsMakeDataEqn(pmnl,nil, 
         gsMakeDataExprSwapZeroLTE(gsMakeDataExprCNat(p), m, n),
         gsMakeDataExprLTE(
           gsMakeDataExprSwapZero(gsMakeDataExprCNat(p),m),
           gsMakeDataExprSwapZero(gsMakeDataExprCNat(p),n)))
    ), p_data_decls->data_eqns);
  std::clog << "end impl_sort_nat" << std::endl;
}

void old_impl_sort_int(t_data_decls *p_data_decls)
{
  std::clog << "impl_sort_int" << std::endl;
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
  p_data_decls->ops = ATconcat(ATmakeList(26,
      (ATerm) gsMakeOpIdNat2Int(),
      (ATerm) gsMakeOpIdInt2Nat(),
      (ATerm) gsMakeOpIdPos2Int(),
      (ATerm) gsMakeOpIdInt2Pos(),
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
  //Declare new_data equations for sort Int
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
  p_data_decls->data_eqns = ATconcat(ATmakeList(62,
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
  std::clog << "end impl_sort_int" << std::endl;
}

void old_impl_sort_real(t_data_decls *p_data_decls)
{
  //Declare sort Real
  p_data_decls->sorts = ATinsert(p_data_decls->sorts, (ATerm) gsMakeSortIdReal());
  //Declare constructors for sort Real (none)
  //Declare operations for sort Real
  ATermAppl se_pos  = gsMakeSortExprPos();
  ATermAppl se_nat  = gsMakeSortExprNat();
  ATermAppl se_int  = gsMakeSortExprInt();
  ATermAppl se_real = gsMakeSortExprReal();
  p_data_decls->ops = ATconcat(ATmakeList(27,
      (ATerm) gsMakeOpIdCReal(),
      (ATerm) gsMakeOpIdPos2Real(),
      (ATerm) gsMakeOpIdNat2Real(),
      (ATerm) gsMakeOpIdInt2Real(),
      (ATerm) gsMakeOpIdReal2Pos(),
      (ATerm) gsMakeOpIdReal2Nat(),
      (ATerm) gsMakeOpIdReal2Int(),
      (ATerm) gsMakeOpIdMin(se_real),
      (ATerm) gsMakeOpIdMax(se_real, se_real),
      (ATerm) gsMakeOpIdAbs(se_real),
      (ATerm) gsMakeOpIdNeg(se_real),
      (ATerm) gsMakeOpIdSucc(se_real),
      (ATerm) gsMakeOpIdPred(se_real),
      (ATerm) gsMakeOpIdAdd(se_real, se_real),
      (ATerm) gsMakeOpIdSubt(se_real),
      (ATerm) gsMakeOpIdMult(se_real),
      (ATerm) gsMakeOpIdExp(se_real),
      (ATerm) gsMakeOpIdDivide(se_pos),
      (ATerm) gsMakeOpIdDivide(se_nat),
      (ATerm) gsMakeOpIdDivide(se_int),
      (ATerm) gsMakeOpIdDivide(se_real),
      (ATerm) gsMakeOpIdFloor(),
      (ATerm) gsMakeOpIdCeil(),
      (ATerm) gsMakeOpIdRound(),
      (ATerm) gsMakeOpIdRedFrac(),
      (ATerm) gsMakeOpIdRedFracWhr(),
      (ATerm) gsMakeOpIdRedFracHlp()
    ), p_data_decls->ops);

  //Declare new_data equations for sort Int
  ATermAppl nil = gsMakeNil();
  ATermAppl t = gsMakeDataExprTrue();
  ATermAppl f = gsMakeDataExprFalse();
  ATermAppl zero = gsMakeDataExprC0();
  ATermAppl one = gsMakeDataExprC1();
  ATermAppl two = gsMakeDataExprCDub(f, one);
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
  ATermList xl  = ATmakeList1((ATerm) x);
  ATermList rl = ATmakeList1((ATerm) r);
  ATermList pql = ATmakeList2((ATerm) p, (ATerm) q);
  ATermList pxl = ATmakeList2((ATerm) p, (ATerm) x);
  ATermList pxyl = ATmakeList3((ATerm) p, (ATerm) x, (ATerm) y);
  ATermList pqxl = ATmakeList3((ATerm) p, (ATerm) q, (ATerm) x);
  ATermList pqxyl = ATmakeList4((ATerm) p, (ATerm) q, (ATerm) x, (ATerm) y);
  ATermList pnxl = ATmakeList3((ATerm) p, (ATerm) n, (ATerm) x);
  ATermList mnl = ATmakeList2((ATerm) m, (ATerm) n);
  ATermList xyl = ATmakeList2((ATerm) x, (ATerm) y);
  ATermList rsl = ATmakeList2((ATerm) r, (ATerm) s);
  p_data_decls->data_eqns = ATconcat(ATmakeList(34,
      //equality (Real # Real -> Bool)
      (ATerm) gsMakeDataEqn(pqxyl, nil, gsMakeDataExprEq(gsMakeDataExprCReal(x,p), gsMakeDataExprCReal(y,q)), gsMakeDataExprEq(gsMakeDataExprMult(x,gsMakeDataExprCInt(gsMakeDataExprCNat(q))),gsMakeDataExprMult(y,gsMakeDataExprCInt(gsMakeDataExprCNat(p))))),
      //less than (Real # Real -> Bool)
      (ATerm) gsMakeDataEqn(rl, nil, gsMakeDataExprLT(r,r), f),
      (ATerm) gsMakeDataEqn(pqxyl, nil, gsMakeDataExprLT(gsMakeDataExprCReal(x,p),gsMakeDataExprCReal(y,q)), gsMakeDataExprLT(gsMakeDataExprMult(x,gsMakeDataExprCInt(gsMakeDataExprCNat(q))),gsMakeDataExprMult(y,gsMakeDataExprCInt(gsMakeDataExprCNat(p))))),
      //less than or equal (Real # Real -> Bool)
      (ATerm) gsMakeDataEqn(rl, nil, gsMakeDataExprLTE(r,r), t),
      (ATerm) gsMakeDataEqn(pqxyl, nil, gsMakeDataExprLTE(gsMakeDataExprCReal(x,p),gsMakeDataExprCReal(y,q)), gsMakeDataExprLTE(gsMakeDataExprMult(x,gsMakeDataExprCInt(gsMakeDataExprCNat(q))),gsMakeDataExprMult(y,gsMakeDataExprCInt(gsMakeDataExprCNat(p))))),
      //convert Int to Real (Int -> Real)
      (ATerm) gsMakeDataEqn(xl, nil, gsMakeDataExprInt2Real(x), gsMakeDataExprCReal(x, one)),
      //convert Nat to Real (Nat -> Real)
      (ATerm) gsMakeDataEqn(nl, nil, gsMakeDataExprNat2Real(n), gsMakeDataExprCReal(gsMakeDataExprCInt(n), one)),
      //convert Pos to Real (Pos -> Real)
      (ATerm) gsMakeDataEqn(pl, nil, gsMakeDataExprPos2Real(p), gsMakeDataExprCReal(gsMakeDataExprCInt(gsMakeDataExprCNat(p)), one)),
      //convert Real to Int (Real -> Int)
      (ATerm) gsMakeDataEqn(xl, nil, gsMakeDataExprReal2Int(gsMakeDataExprCReal(x, one)), x),
      //convert Real to Nat (Real -> Nat)
      (ATerm) gsMakeDataEqn(xl, nil, gsMakeDataExprReal2Nat(gsMakeDataExprCReal(x, one)), gsMakeDataExprInt2Nat(x)),
      //convert Real to Pos (Real -> Pos)
      (ATerm) gsMakeDataEqn(xl, nil, gsMakeDataExprReal2Pos(gsMakeDataExprCReal(x, one)), gsMakeDataExprInt2Pos(x)),
      //minimum (Real # Real -> Real)
      (ATerm) gsMakeDataEqn(rsl, nil, gsMakeDataExprMin(r,s), gsMakeDataExprIf(gsMakeDataExprLT(r,s), r, s)),
      //maximum (Real # Real -> Real)
      (ATerm) gsMakeDataEqn(rsl, nil, gsMakeDataExprMax(r,s), gsMakeDataExprIf(gsMakeDataExprLT(r,s), s, r)),
      //absolute value (Real -> Real)
      (ATerm) gsMakeDataEqn(rl, nil, gsMakeDataExprAbs(r), gsMakeDataExprIf(gsMakeDataExprLT(r,gsMakeDataExprCReal(gsMakeDataExprCInt(zero), one)),gsMakeDataExprNeg(r),r)),
      //negation (Real -> Real)
      (ATerm) gsMakeDataEqn(pxl, nil, gsMakeDataExprNeg(gsMakeDataExprCReal(x,p)), gsMakeDataExprCReal(gsMakeDataExprNeg(x), p)),
      //successor (Real -> Real)
      (ATerm) gsMakeDataEqn(pxl, nil, gsMakeDataExprSucc(gsMakeDataExprCReal(x,p)), gsMakeDataExprCReal(gsMakeDataExprAdd(x,gsMakeDataExprCInt(gsMakeDataExprCNat(p))),p)),
      //predecessor (Real -> Real)
      (ATerm) gsMakeDataEqn(pxl, nil, gsMakeDataExprPred(gsMakeDataExprCReal(x,p)), gsMakeDataExprCReal(gsMakeDataExprSubt(x,gsMakeDataExprCInt(gsMakeDataExprCNat(p))),p)),
      //addition (Real # Real -> Real)
      (ATerm) gsMakeDataEqn(pqxyl, nil, gsMakeDataExprAdd(gsMakeDataExprCReal(x,p), gsMakeDataExprCReal(y,q)), gsMakeDataExprRedFrac(gsMakeDataExprAdd(gsMakeDataExprMult(x,gsMakeDataExprCInt(gsMakeDataExprCNat(q))), gsMakeDataExprMult(y,gsMakeDataExprCInt(gsMakeDataExprCNat(p)))), gsMakeDataExprCInt(gsMakeDataExprCNat(gsMakeDataExprMult(p,q))))),
      //subtraction (Real # Real -> Real)
      (ATerm) gsMakeDataEqn(pqxyl, nil, gsMakeDataExprSubt(gsMakeDataExprCReal(x,p), gsMakeDataExprCReal(y,q)), gsMakeDataExprRedFrac(gsMakeDataExprSubt(gsMakeDataExprMult(x,gsMakeDataExprCInt(gsMakeDataExprCNat(q))), gsMakeDataExprMult(y,gsMakeDataExprCInt(gsMakeDataExprCNat(p)))), gsMakeDataExprCInt(gsMakeDataExprCNat(gsMakeDataExprMult(p,q))))),
      //multiplication (Real # Real -> Real)
      (ATerm) gsMakeDataEqn(pqxyl, nil, gsMakeDataExprMult(gsMakeDataExprCReal(x,p), gsMakeDataExprCReal(y,q)), gsMakeDataExprRedFrac(gsMakeDataExprMult(x,y), gsMakeDataExprCInt(gsMakeDataExprCNat(gsMakeDataExprMult(p,q))))),
      //division (Real # Real -> Real)
      (ATerm) gsMakeDataEqn(pqxyl, gsMakeDataExprNeq(y, gsMakeDataExprCInt(zero)), gsMakeDataExprDivide(gsMakeDataExprCReal(x,p), gsMakeDataExprCReal(y,q)), gsMakeDataExprRedFrac(gsMakeDataExprMult(x,gsMakeDataExprCInt(gsMakeDataExprCNat(q))), gsMakeDataExprMult(y,gsMakeDataExprCInt(gsMakeDataExprCNat(p))))),
      //division (Pos # Pos -> Real)
      (ATerm) gsMakeDataEqn(pql, nil, gsMakeDataExprDivide(p,q), gsMakeDataExprRedFrac(gsMakeDataExprCInt(gsMakeDataExprCNat(p)), gsMakeDataExprCInt(gsMakeDataExprCNat(q)))),
      //division (Nat # Nat -> Real)
      (ATerm) gsMakeDataEqn(mnl, gsMakeDataExprNeq(m, zero), gsMakeDataExprDivide(m,n), gsMakeDataExprRedFrac(gsMakeDataExprCInt(m), gsMakeDataExprCInt(n))),
      //division (Int # Int -> Real)
      (ATerm) gsMakeDataEqn(xyl, gsMakeDataExprNeq(y, gsMakeDataExprCInt(zero)), gsMakeDataExprDivide(x,y), gsMakeDataExprRedFrac(x,y)),
      //exponentiation (Real # Int -> Real)
      (ATerm) gsMakeDataEqn(pnxl, nil, gsMakeDataExprExp(gsMakeDataExprCReal(x,p), gsMakeDataExprCInt(n)), gsMakeDataExprRedFrac(gsMakeDataExprExp(x,n), gsMakeDataExprCInt(gsMakeDataExprCNat(gsMakeDataExprExp(p,n))))),
      (ATerm) gsMakeDataEqn(pqxl, gsMakeDataExprNeq(x, gsMakeDataExprCInt(zero)), gsMakeDataExprExp(gsMakeDataExprCReal(x,p), gsMakeDataExprCNeg(q)), gsMakeDataExprRedFrac(gsMakeDataExprCInt(gsMakeDataExprCNat(gsMakeDataExprExp(p,gsMakeDataExprCNat(q)))), gsMakeDataExprExp(x,gsMakeDataExprCNat(q)))),
      //floor (Real -> Int)
      (ATerm) gsMakeDataEqn(pxl, nil, gsMakeDataExprFloor(gsMakeDataExprCReal(x,p)), gsMakeDataExprDiv(x,p)),
      //ceil (Real -> Int)
      (ATerm) gsMakeDataEqn(rl, nil, gsMakeDataExprCeil(r), gsMakeDataExprNeg(gsMakeDataExprFloor(gsMakeDataExprNeg(r)))),
      //round (Real -> Int)
      (ATerm) gsMakeDataEqn(rl, nil, gsMakeDataExprRound(r), gsMakeDataExprFloor(gsMakeDataExprAdd(r,gsMakeDataExprCReal(gsMakeDataExprCInt(gsMakeDataExprCNat(one)),two)))),
      //redfrac (Int # Int -> Real)
      (ATerm) gsMakeDataEqn(pxl, nil, gsMakeDataExprRedFrac(x,gsMakeDataExprCNeg(p)), gsMakeDataExprRedFrac(gsMakeDataExprNeg(x), gsMakeDataExprCInt(gsMakeDataExprCNat(p)))),
      (ATerm) gsMakeDataEqn(pxl, nil, gsMakeDataExprRedFrac(x,gsMakeDataExprCInt(gsMakeDataExprCNat(p))), gsMakeDataExprRedFracWhr(p, gsMakeDataExprDiv(x, p), gsMakeDataExprMod(x, p))),
      //redfracwhr (Pos # Int # Nat -> Real)
      (ATerm) gsMakeDataEqn(pxl, nil, gsMakeDataExprRedFracWhr(p, x, zero), gsMakeDataExprCReal(x, one)),
      (ATerm) gsMakeDataEqn(pqxl, nil, gsMakeDataExprRedFracWhr(p, x, gsMakeDataExprCNat(q)), gsMakeDataExprRedFracHlp(gsMakeDataExprRedFrac(gsMakeDataExprCInt(gsMakeDataExprCNat(p)), gsMakeDataExprCInt(gsMakeDataExprCNat(q))), x)),
      //redfrachlp (Real # Int -> Real)
      (ATerm) gsMakeDataEqn(pxyl, nil, gsMakeDataExprRedFracHlp(gsMakeDataExprCReal(x, p), y), gsMakeDataExprCReal(gsMakeDataExprAdd(gsMakeDataExprCInt(gsMakeDataExprCNat(p)), gsMakeDataExprMult(y, x)), gsMakeDataExprInt2Pos(x)))
      ), p_data_decls->data_eqns);
  //add implementation of sort Int, if necessary
  if (ATindexOf(p_data_decls->sorts, (ATerm) gsMakeSortIdInt(), 0) == -1) {
    std::clog << "implement integers" << std::endl;
    old_impl_sort_int(p_data_decls);
  }
}

ATermList old_build_list_equations(ATermAppl sort_elt, ATermAppl sort_list)
{
  data_equation_list equations = sort_list::list_generate_equations_code(sort_expression(sort_elt));
  //Declare new_data equations for sort sort_id
  ATermList el = ATmakeList0();
  ATermAppl el_sort_id = gsMakeDataExprEmptyList(sort_list);
  ATermAppl s_sort_id = gsMakeDataVarId(gsString2ATermAppl("s"), sort_list);
  ATermAppl t_sort_id = gsMakeDataVarId(gsString2ATermAppl("t"), sort_list);
  ATermAppl d_sort_elt = gsMakeDataVarId(gsString2ATermAppl("d"), sort_elt);
  ATermAppl e_sort_elt = gsMakeDataVarId(gsString2ATermAppl("e"), sort_elt);
  ATermAppl p = gsMakeDataVarId(gsString2ATermAppl("p"), gsMakeSortExprPos());
  ATermAppl ds = gsMakeDataExprCons(d_sort_elt, s_sort_id);
  ATermAppl es = gsMakeDataExprCons(e_sort_elt, s_sort_id);
  ATermAppl et = gsMakeDataExprCons(e_sort_elt, t_sort_id);
  ATermAppl nil = gsMakeNil();
  ATermAppl zero = gsMakeDataExprC0();
  ATermAppl t = gsMakeDataExprTrue();
  ATermAppl f = gsMakeDataExprFalse();
  ATermList dl = ATmakeList1((ATerm) d_sort_elt);
  ATermList sl = ATmakeList1((ATerm) s_sort_id);
  ATermList dsl = ATmakeList2((ATerm) d_sort_elt, (ATerm) s_sort_id);
  ATermList desl = ATmakeList3((ATerm) d_sort_elt, (ATerm) e_sort_elt,
    (ATerm) s_sort_id);
  ATermList dstl = ATmakeList3((ATerm) t_sort_id, (ATerm) d_sort_elt,
    (ATerm) s_sort_id);
  ATermList destl = ATmakeList4((ATerm) t_sort_id, (ATerm) s_sort_id,
    (ATerm) d_sort_elt, (ATerm) e_sort_elt);
  ATermList dspl = ATmakeList3((ATerm) d_sort_elt, (ATerm) s_sort_id, (ATerm) p);

  ATermList new_data_eqns = ATmakeList(20,
      //equality (sort_id -> sort_id -> Bool)
      (ATerm) gsMakeDataEqn(dsl, nil, gsMakeDataExprEq(el_sort_id, ds), f),
      (ATerm) gsMakeDataEqn(dsl, nil, gsMakeDataExprEq(ds, el_sort_id), f),
      (ATerm) gsMakeDataEqn(destl, nil,
        gsMakeDataExprEq(ds, et),
        gsMakeDataExprAnd(
          gsMakeDataExprEq(d_sort_elt, e_sort_elt),
          gsMakeDataExprEq(s_sort_id, t_sort_id))),
      //element test (sort_elt -> sort_id -> Bool)
      (ATerm) gsMakeDataEqn(dl, nil,
        gsMakeDataExprEltIn(d_sort_elt, el_sort_id),
        f),
      (ATerm) gsMakeDataEqn(desl, nil,
        gsMakeDataExprEltIn(d_sort_elt, es),
        gsMakeDataExprOr(
          gsMakeDataExprEq(d_sort_elt, e_sort_elt),
          gsMakeDataExprEltIn(d_sort_elt, s_sort_id))),
      //list size (sort_id -> Nat)
      (ATerm) gsMakeDataEqn(el, nil,
        gsMakeDataExprListSize(el_sort_id),
        gsMakeDataExprC0()),
      (ATerm) gsMakeDataEqn(dsl, nil,
        gsMakeDataExprListSize(ds),
        gsMakeDataExprCNat(
          gsMakeDataExprSucc(gsMakeDataExprListSize(s_sort_id)))),
      //list snoc (sort_id -> sort_elt -> sort_id)
      (ATerm) gsMakeDataEqn(dl, nil,
        gsMakeDataExprSnoc(el_sort_id, d_sort_elt),
        gsMakeDataExprCons(d_sort_elt, el_sort_id)),
      (ATerm) gsMakeDataEqn(desl, nil,
        gsMakeDataExprSnoc(ds, e_sort_elt),
        gsMakeDataExprCons(d_sort_elt, gsMakeDataExprSnoc(s_sort_id, e_sort_elt))),
      //list concatenation (sort_id -> sort_id -> sort_id)
      (ATerm) gsMakeDataEqn(sl, nil,
        gsMakeDataExprConcat(el_sort_id, s_sort_id),
        s_sort_id),
      (ATerm) gsMakeDataEqn(dstl, nil,
        gsMakeDataExprConcat(ds, t_sort_id),
        gsMakeDataExprCons(d_sort_elt, gsMakeDataExprConcat(s_sort_id, t_sort_id))),
      (ATerm) gsMakeDataEqn(sl, nil,
        gsMakeDataExprConcat(s_sort_id, el_sort_id),
        s_sort_id),
      //list element at (sort_id -> Nat -> sort_elt)
      (ATerm) gsMakeDataEqn(dsl, nil,
        gsMakeDataExprEltAt(ds, zero, sort_elt),
        d_sort_elt),
      (ATerm) gsMakeDataEqn(dspl, nil,
        gsMakeDataExprEltAt(ds, gsMakeDataExprCNat(p), sort_elt),
        gsMakeDataExprEltAt(s_sort_id, gsMakeDataExprPred(p), sort_elt)),
      //head (sort_id -> sort_elt)
      (ATerm) gsMakeDataEqn(dsl, nil,
         gsMakeDataExprHead(ds, sort_elt),
         d_sort_elt),
      //tail (sort_id -> sort_id)
      (ATerm) gsMakeDataEqn(dsl, nil,
         gsMakeDataExprTail(ds),
         s_sort_id),
      //right head (sort_id -> sort_elt)
      (ATerm) gsMakeDataEqn(dl, nil,
        gsMakeDataExprRHead(gsMakeDataExprCons(d_sort_elt, el_sort_id), sort_elt),
        d_sort_elt),
      (ATerm) gsMakeDataEqn(desl, nil,
        gsMakeDataExprRHead(gsMakeDataExprCons(d_sort_elt,
          gsMakeDataExprCons(e_sort_elt, s_sort_id)), sort_elt),
        gsMakeDataExprRHead(gsMakeDataExprCons(e_sort_elt, s_sort_id), sort_elt)),
      //right tail (sort_id -> sort_id)
      (ATerm) gsMakeDataEqn(dl, nil,
        gsMakeDataExprRTail(gsMakeDataExprCons(d_sort_elt, el_sort_id)),
        el_sort_id),
      (ATerm) gsMakeDataEqn(desl, nil,
        gsMakeDataExprRTail(gsMakeDataExprCons(d_sort_elt,
          gsMakeDataExprCons(e_sort_elt, s_sort_id))),
        gsMakeDataExprCons(d_sort_elt,
          gsMakeDataExprRTail(gsMakeDataExprCons(e_sort_elt, s_sort_id)))));

  return new_data_eqns;
}

ATermAppl old_impl_sort_list(ATermAppl sort_list, ATermList *p_substs,
  t_data_decls *p_data_decls)
{
  assert(gsIsSortExprList(sort_list));
  ATermAppl sort_elt = ATAgetArgument(sort_list, 1);

  //declare constructors for sort sort_id
  ATermList new_cons_ops = ATmakeList2(
      (ATerm) gsMakeOpIdEmptyList(sort_list),
      (ATerm) gsMakeOpIdCons(sort_elt, sort_list));

  //Declare operations for sort sort_id
  ATermList new_ops = ATmakeList(9,
      (ATerm) gsMakeOpIdEltIn(sort_elt, sort_list),
      (ATerm) gsMakeOpIdListSize(sort_list),
      (ATerm) gsMakeOpIdSnoc(sort_list, sort_elt),
      (ATerm) gsMakeOpIdConcat(sort_list),
      (ATerm) gsMakeOpIdEltAt(sort_list, sort_elt),
      (ATerm) gsMakeOpIdHead(sort_list, sort_elt),
      (ATerm) gsMakeOpIdTail(sort_list),
      (ATerm) gsMakeOpIdRHead(sort_list, sort_elt),
      (ATerm) gsMakeOpIdRTail(sort_list));

  ATermList new_data_eqns = old_build_list_equations(sort_elt, sort_list);

  //declare fresh sort identifier for sort_list
  ATermAppl sort_id = make_fresh_list_sort_id((ATerm) p_data_decls->sorts);
  p_data_decls->sorts = ATinsert(p_data_decls->sorts, (ATerm) sort_id);
  //implement sort_elt
  //this needs to be done first in order to keep the substitutions sound!
  sort_elt = impl_exprs_appl(sort_elt, p_substs, p_data_decls);
  //add substitution for sort_list
  ATermAppl subst = gsMakeSubst_Appl(sort_list, sort_id);
  *p_substs = gsAddSubstToSubsts(subst, *p_substs);

  //perform substitutions
  new_cons_ops = gsSubstValues_List(*p_substs, new_cons_ops, true);
  p_data_decls->cons_ops = ATconcat(new_cons_ops, p_data_decls->cons_ops);
  new_ops = gsSubstValues_List(*p_substs, new_ops, true);
  p_data_decls->ops = ATconcat(new_ops, p_data_decls->ops);
  new_data_eqns = gsSubstValues_List(*p_substs, new_data_eqns, true);
  p_data_decls->data_eqns = ATconcat(new_data_eqns, p_data_decls->data_eqns);

  //add implementation of sort Nat, if necessary
  if (ATindexOf(p_data_decls->sorts, (ATerm) gsMakeSortIdNat(), 0) == -1) {
    impl_sort_nat(p_data_decls);
  }
  return sort_id;
}

ATermList old_build_set_equations(ATermAppl sort_elt, ATermAppl sort_set)
{
  //declare new_data equations for sort sort_id
  ATermAppl sort_func = gsMakeSortArrow1(sort_elt, gsMakeSortExprBool());
  ATermList el = ATmakeList0();
  ATermAppl s_sort_id = gsMakeDataVarId(gsString2ATermAppl("s"), sort_set);
  ATermAppl t_sort_id = gsMakeDataVarId(gsString2ATermAppl("t"), sort_set);
  ATermAppl f_sort_func = gsMakeDataVarId(gsString2ATermAppl("f"), sort_func);
  ATermAppl g_sort_func = gsMakeDataVarId(gsString2ATermAppl("g"), sort_func);
  ATermAppl d_sort_elt = gsMakeDataVarId(gsString2ATermAppl("d"), sort_elt);
  ATermAppl x_sort_elt = gsMakeDataVarId(gsString2ATermAppl("x"), sort_elt);
  ATermAppl nil = gsMakeNil();
  ATermAppl f = gsMakeDataExprFalse();
  ATermList stl = ATmakeList2((ATerm) t_sort_id, (ATerm) s_sort_id);
  ATermList fl = ATmakeList1((ATerm) f_sort_func);
  ATermList dfl = ATmakeList2((ATerm) f_sort_func, (ATerm) d_sort_elt);
  ATermList fgl = ATmakeList2((ATerm) f_sort_func, (ATerm) g_sort_func);
  ATermAppl false_func =
    gsMakeBinder(gsMakeLambda(), ATmakeList1((ATerm) x_sort_elt), f);
  ATermAppl imp_func =
    gsMakeBinder(gsMakeLambda(), ATmakeList1((ATerm) x_sort_elt),
      gsMakeDataExprImp(
        gsMakeDataAppl1(f_sort_func, x_sort_elt),
        gsMakeDataAppl1(g_sort_func, x_sort_elt)
      )
    );
  ATermAppl OrFunc =
    gsMakeBinder(gsMakeLambda(), ATmakeList1((ATerm) x_sort_elt),
      gsMakeDataExprOr(
        gsMakeDataAppl1(f_sort_func, x_sort_elt),
        gsMakeDataAppl1(g_sort_func, x_sort_elt)
      )
    );
  ATermAppl and_func =
    gsMakeBinder(gsMakeLambda(), ATmakeList1((ATerm) x_sort_elt),
      gsMakeDataExprAnd(
        gsMakeDataAppl1(f_sort_func, x_sort_elt),
        gsMakeDataAppl1(g_sort_func, x_sort_elt)
      )
    );
  ATermAppl not_func =
    gsMakeBinder(gsMakeLambda(), ATmakeList1((ATerm) x_sort_elt),
      gsMakeDataExprNot(gsMakeDataAppl1(f_sort_func, x_sort_elt))
    );

  ATermList new_data_eqns = ATmakeList(9,
      //equality (sort_id -> sort_id -> Bool)
      (ATerm) gsMakeDataEqn(fgl, nil,
        gsMakeDataExprEq(
          gsMakeDataExprSetComp(f_sort_func, sort_set),
          gsMakeDataExprSetComp(g_sort_func, sort_set)), 
        gsMakeDataExprEq(f_sort_func, g_sort_func)),
      //empty set (sort_id)
      (ATerm) gsMakeDataEqn(el, nil,
        gsMakeDataExprEmptySet(sort_set),
        gsMakeDataExprSetComp(false_func, sort_set)),
      //element test (sort_elt -> sort_id -> Bool)
      (ATerm) gsMakeDataEqn(dfl, nil,
        gsMakeDataExprEltIn(d_sort_elt, gsMakeDataExprSetComp(f_sort_func, sort_set)),
        gsMakeDataAppl1(f_sort_func, d_sort_elt)),
      //subset or equal (sort_id -> sort_id -> Bool)
      (ATerm) gsMakeDataEqn(fgl, nil,
        gsMakeDataExprSubSetEq(
          gsMakeDataExprSetComp(f_sort_func, sort_set),
          gsMakeDataExprSetComp(g_sort_func, sort_set)), 
        gsMakeDataExprForall(imp_func)),
      //proper subset (sort_id -> sort_id -> Bool)
      (ATerm) gsMakeDataEqn(stl, nil,
        gsMakeDataExprSubSet(s_sort_id, t_sort_id),
        gsMakeDataExprAnd(
          gsMakeDataExprSubSetEq(s_sort_id, t_sort_id), 
          gsMakeDataExprNeq(s_sort_id, t_sort_id)
        )),
      //union (sort_id -> sort_id -> sort_id)
      (ATerm) gsMakeDataEqn(fgl, nil,
        gsMakeDataExprSetUnion(
          gsMakeDataExprSetComp(f_sort_func, sort_set),
          gsMakeDataExprSetComp(g_sort_func, sort_set)), 
        gsMakeDataExprSetComp(OrFunc, sort_set)),
      //difference (sort_id -> sort_id -> sort_id)
      (ATerm) gsMakeDataEqn(stl, nil,
        gsMakeDataExprSetDiff(s_sort_id, t_sort_id),
        gsMakeDataExprSetInterSect(s_sort_id, gsMakeDataExprSetCompl(t_sort_id))),
      //intersection (sort_id -> sort_id -> sort_id)
      (ATerm) gsMakeDataEqn(fgl, nil,
        gsMakeDataExprSetInterSect(
          gsMakeDataExprSetComp(f_sort_func, sort_set),
          gsMakeDataExprSetComp(g_sort_func, sort_set)), 
        gsMakeDataExprSetComp(and_func, sort_set)),
      //complement (sort_id -> sort_id)
      (ATerm) gsMakeDataEqn(fl, nil,
        gsMakeDataExprSetCompl(gsMakeDataExprSetComp(f_sort_func, sort_set)),
        gsMakeDataExprSetComp(not_func, sort_set)));
      //simplification of combinations of functions false, not, imp, and, or
      ////left unit of the or function
      //(ATerm) gsMakeDataEqn(fl, nil,
      //  gsMakeDataAppl2(gsGetDataExprHead(OrFunc), false_func, f_sort_func),
      //  f_sort_func),
      ////right unit of the or function
      //(ATerm) gsMakeDataEqn(fl, nil,
      //  gsMakeDataAppl2(gsGetDataExprHead(OrFunc), f_sort_func, false_func),
      //  f_sort_func)

  return new_data_eqns;
}

ATermAppl old_impl_sort_set(ATermAppl sort_set, ATermList *p_substs,
  t_data_decls *p_data_decls)
{
  assert(gsIsSortExprSet(sort_set));
  ATermAppl sort_elt = ATAgetArgument(sort_set, 1);

  //implement expressions in the target sort of sort_set
  //this needs to be done first to keep the substitutions sound!
  impl_exprs_appl(sort_elt, p_substs, p_data_decls);

  //declare fresh sort identifier for sort_set
  ATermAppl sort_id = make_fresh_set_sort_id((ATerm) p_data_decls->sorts);
  p_data_decls->sorts = ATinsert(p_data_decls->sorts, (ATerm) sort_id);
  //add substitution for sort_set
  ATermAppl subst = gsMakeSubst_Appl(sort_set, sort_id);
  *p_substs = gsAddSubstToSubsts(subst, *p_substs);

  //declare operations for sort sort_id
  ATermList new_ops = ATmakeList(9,
      (ATerm) gsMakeOpIdSetComp(gsMakeSortArrow1(sort_elt, gsMakeSortExprBool()), sort_set),
      (ATerm) gsMakeOpIdEmptySet(sort_set),
      (ATerm) gsMakeOpIdEltIn(sort_elt, sort_set),
      (ATerm) gsMakeOpIdSubSetEq(sort_set),
      (ATerm) gsMakeOpIdSubSet(sort_set),
      (ATerm) gsMakeOpIdSetUnion(sort_set),
      (ATerm) gsMakeOpIdSetDiff(sort_set),
      (ATerm) gsMakeOpIdSetIntersect(sort_set),
      (ATerm) gsMakeOpIdSetCompl(sort_set));

  ATermList new_data_eqns = old_build_set_equations(sort_elt, sort_set);

  new_data_eqns = impl_exprs_list(new_data_eqns, p_substs, p_data_decls);

  //perform substitutions
  new_ops = gsSubstValues_List(*p_substs, new_ops, true);
  p_data_decls->ops = ATconcat(new_ops, p_data_decls->ops);
  new_data_eqns = gsSubstValues_List(*p_substs, new_data_eqns, true);
  p_data_decls->data_eqns = ATconcat(new_data_eqns, p_data_decls->data_eqns);

  return sort_id;
}

ATermList old_build_bag_equations(ATermAppl sort_elt, ATermAppl sort_bag, ATermAppl sort_set)
{
  //declare new_data equations for sort sort_id
  ATermAppl sort_func = gsMakeSortArrow1(sort_elt, gsMakeSortExprNat());
  ATermList el = ATmakeList0();
  ATermAppl s_sort_id = gsMakeDataVarId(gsString2ATermAppl("s"), sort_bag);
  ATermAppl t_sort_id = gsMakeDataVarId(gsString2ATermAppl("t"), sort_bag);
  ATermAppl f_sort_func = gsMakeDataVarId(gsString2ATermAppl("f"), sort_func);
  ATermAppl g_sort_func = gsMakeDataVarId(gsString2ATermAppl("g"), sort_func);
  ATermAppl d_sort_elt = gsMakeDataVarId(gsString2ATermAppl("d"), sort_elt);
  ATermAppl x_sort_elt = gsMakeDataVarId(gsString2ATermAppl("x"), sort_elt);
  ATermAppl y_sort_elt = gsMakeDataVarId(gsString2ATermAppl("y"), sort_elt);
  ATermAppl u_sort_set = gsMakeDataVarId(gsString2ATermAppl("u"), sort_set);
  ATermAppl m = gsMakeDataVarId(gsString2ATermAppl("m"), gsMakeSortExprNat());
  ATermAppl n = gsMakeDataVarId(gsString2ATermAppl("n"), gsMakeSortExprNat());
  ATermAppl nil = gsMakeNil();
  ATermAppl zero = gsMakeDataExprC0();
  ATermList sl = ATmakeList1((ATerm) s_sort_id);
  ATermList stl = ATmakeList2((ATerm) t_sort_id, (ATerm) s_sort_id);
  ATermList dsl = ATmakeList2((ATerm) d_sort_elt, (ATerm) s_sort_id);
  ATermList ul = ATmakeList1((ATerm) u_sort_set);
  ATermList dfl = ATmakeList2((ATerm) f_sort_func, (ATerm) d_sort_elt);
  ATermList fgl = ATmakeList2((ATerm) f_sort_func, (ATerm) g_sort_func);
  ATermAppl zero_func =
    gsMakeBinder(gsMakeLambda(), ATmakeList1((ATerm) x_sort_elt), zero);
  ATermAppl lte_func =
    gsMakeBinder(gsMakeLambda(), ATmakeList1((ATerm) x_sort_elt),
      gsMakeDataExprLTE(
        gsMakeDataAppl1(f_sort_func, x_sort_elt),
        gsMakeDataAppl1(g_sort_func, x_sort_elt)
      )
    );
  ATermAppl add_func =
    gsMakeBinder(gsMakeLambda(), ATmakeList1((ATerm) x_sort_elt),
      gsMakeDataExprAdd(
        gsMakeDataAppl1(f_sort_func, x_sort_elt),
        gsMakeDataAppl1(g_sort_func, x_sort_elt)
      )
    );
  ATermAppl subt_max0_func =
    gsMakeBinder(gsMakeLambda(), ATmakeList1((ATerm) y_sort_elt),
      gsMakeDataAppl1(
        gsMakeDataAppl1(
          gsMakeBinder(gsMakeLambda(), ATmakeList1((ATerm) m),
            gsMakeBinder(gsMakeLambda(), ATmakeList1((ATerm) n),
              gsMakeDataExprIf(
                gsMakeDataExprGT(m, n), gsMakeDataExprGTESubt(m, n), zero
              )
            )
          ),
        gsMakeDataAppl1(f_sort_func, y_sort_elt)),
      gsMakeDataAppl1(g_sort_func, y_sort_elt)));
    /*
  ATermAppl subt_max0_func =
    gsMakeBinder(gsMakeLambda(), ATmakeList1((ATerm) y_sort_elt),
      gsMakeDataAppl2(
        gsMakeBinder(gsMakeLambda(), ATmakeList2((ATerm) m, (ATerm) n),
          gsMakeDataExprIf(
            gsMakeDataExprGT(m, n), gsMakeDataExprGTESubt(m, n), zero
          )
        ),
        gsMakeDataAppl1(f_sort_func, y_sort_elt),
        gsMakeDataAppl1(g_sort_func, y_sort_elt)));
    */
  /*
  ATermAppl subt_max0_func =
    gsMakeBinder(gsMakeLambda(), ATmakeList1((ATerm) x_sort_elt),
      gsMakeWhr(
        gsMakeDataExprIf(
          gsMakeDataExprGT(m, n), gsMakeDataExprGTESubt(m, n), zero
        ), ATmakeList2(
          (ATerm) gsMakeDataVarIdInit(m, gsMakeDataAppl1(f_sort_func, x_sort_elt)),
          (ATerm) gsMakeDataVarIdInit(n, gsMakeDataAppl1(g_sort_func, x_sort_elt))
        )
      )
    );
  */
  ATermAppl min_func =
    gsMakeBinder(gsMakeLambda(), ATmakeList1((ATerm) x_sort_elt),
      gsMakeDataExprMin(
        gsMakeDataAppl1(f_sort_func, x_sort_elt),
        gsMakeDataAppl1(g_sort_func, x_sort_elt)
      )
    );
  ATermAppl bag2set_func =
    gsMakeBinder(gsMakeLambda(), ATmakeList1((ATerm) x_sort_elt),
      gsMakeDataExprEltIn(x_sort_elt, s_sort_id)
    );
  ATermAppl set2bag_func =
    gsMakeBinder(gsMakeLambda(), ATmakeList1((ATerm) x_sort_elt),
      gsMakeDataExprIf(
        gsMakeDataExprEltIn(x_sort_elt, u_sort_set),
        gsMakeDataExprNat_int(1),
        gsMakeDataExprNat_int(0)
      )
    );

  ATermList new_data_eqns = ATmakeList(11,
      //equality (sort_id -> sort_id -> Bool)
      (ATerm) gsMakeDataEqn(fgl, nil,
        gsMakeDataExprEq(
          gsMakeDataExprBagComp(f_sort_func, sort_bag),
          gsMakeDataExprBagComp(g_sort_func, sort_bag)), 
        gsMakeDataExprEq(f_sort_func, g_sort_func)),
      //empty bag (sort_id)
      (ATerm) gsMakeDataEqn(el, nil,
        gsMakeDataExprEmptyBag(sort_bag),
        gsMakeDataExprBagComp(zero_func, sort_bag)),
      //count (sort_elt -> sort_id -> Bool)
      (ATerm) gsMakeDataEqn(dfl, nil,
        gsMakeDataExprCount(d_sort_elt, gsMakeDataExprBagComp(f_sort_func, sort_bag)),
        gsMakeDataAppl1(f_sort_func, d_sort_elt)),
      //element test (sort_elt -> sort_id -> Bool)
      (ATerm) gsMakeDataEqn(dsl, nil,
        gsMakeDataExprEltIn(d_sort_elt, s_sort_id),
        gsMakeDataExprGT(gsMakeDataExprCount(d_sort_elt, s_sort_id), zero)),
      //subbag or equal (sort_id -> sort_id -> Bool)
      (ATerm) gsMakeDataEqn(fgl, nil,
        gsMakeDataExprSubBagEq(
          gsMakeDataExprBagComp(f_sort_func, sort_bag),
          gsMakeDataExprBagComp(g_sort_func, sort_bag)), 
        gsMakeDataExprForall(lte_func)),
      //proper subbag (sort_id -> sort_id -> Bool)
      (ATerm) gsMakeDataEqn(stl, nil,
        gsMakeDataExprSubBag(s_sort_id, t_sort_id),
        gsMakeDataExprAnd(
          gsMakeDataExprSubBagEq(s_sort_id, t_sort_id), 
          gsMakeDataExprNeq(s_sort_id, t_sort_id)
        )),
      //union (sort_id -> sort_id -> sort_id)
      (ATerm) gsMakeDataEqn(fgl, nil,
        gsMakeDataExprBagUnion(
          gsMakeDataExprBagComp(f_sort_func, sort_bag),
          gsMakeDataExprBagComp(g_sort_func, sort_bag)), 
        gsMakeDataExprBagComp(add_func, sort_bag)),
      //difference (sort_id -> sort_id -> sort_id)
      (ATerm) gsMakeDataEqn(fgl, nil,
        gsMakeDataExprBagDiff(
          gsMakeDataExprBagComp(f_sort_func, sort_bag),
          gsMakeDataExprBagComp(g_sort_func, sort_bag)), 
        gsMakeDataExprBagComp(subt_max0_func, sort_bag)),
      //intersection (sort_id -> sort_id -> sort_id)
      (ATerm) gsMakeDataEqn(fgl, nil,
        gsMakeDataExprBagInterSect(
          gsMakeDataExprBagComp(f_sort_func, sort_bag),
          gsMakeDataExprBagComp(g_sort_func, sort_bag)), 
        gsMakeDataExprBagComp(min_func, sort_bag)),
      //Bag2Set (sort_id -> sort_set)
      (ATerm) gsMakeDataEqn(sl, nil,
        gsMakeDataExprBag2Set(s_sort_id, sort_set),
        gsMakeDataExprSetComp(bag2set_func, sort_set)),
      //Set2Bag (sort_set -> sort_id)
      (ATerm) gsMakeDataEqn(ul, nil,
        gsMakeDataExprSet2Bag(u_sort_set, sort_bag),
        gsMakeDataExprBagComp(set2bag_func, sort_bag)));

  return new_data_eqns;
}

ATermAppl old_impl_sort_bag(ATermAppl sort_bag, ATermList *p_substs,
  t_data_decls *p_data_decls)
{
  assert(gsIsSortExprBag(sort_bag));
  ATermAppl sort_elt = ATAgetArgument(sort_bag, 1);
  ATermAppl sort_set = gsMakeSortExprSet(sort_elt);

  //implement expressions in the target sort of sort_bag
  //this needs to be done first in order to keep the substitutions sound!
  impl_exprs_appl(sort_elt, p_substs, p_data_decls);
  //add implementation of sort Set(sort_elt), if necessary
  ATermAppl sort_set_impl =
    (ATermAppl) gsSubstValues(*p_substs, (ATerm) sort_set, false);
  if (ATisEqual(sort_set_impl, sort_set)) {
    //Set(sort_elt) is not implemented yet, because it does not occur as an lhs
    //in the list of substitutions in *p_substs
    sort_set_impl = impl_sort_set(sort_set, p_substs, p_data_decls);
  }
  //declare fresh sort identifier for sort_bag
  ATermAppl sort_id = make_fresh_bag_sort_id((ATerm) p_data_decls->sorts);
  p_data_decls->sorts = ATinsert(p_data_decls->sorts, (ATerm) sort_id);
  //add substitution for this identifier
  ATermAppl subst = gsMakeSubst_Appl(sort_bag, sort_id);
  *p_substs = gsAddSubstToSubsts(subst, *p_substs);

  //declare operations for sort sort_id
  ATermList new_ops = ATmakeList(11,
      (ATerm) gsMakeOpIdBagComp(gsMakeSortArrow1(sort_elt, gsMakeSortExprNat()), sort_bag),
      (ATerm) gsMakeOpIdEmptyBag(sort_bag),
      (ATerm) gsMakeOpIdCount(sort_elt, sort_bag),
      (ATerm) gsMakeOpIdEltIn(sort_elt, sort_bag),
      (ATerm) gsMakeOpIdSubBagEq(sort_bag),
      (ATerm) gsMakeOpIdSubBag(sort_bag),
      (ATerm) gsMakeOpIdBagUnion(sort_bag),
      (ATerm) gsMakeOpIdBagDiff(sort_bag),
      (ATerm) gsMakeOpIdBagIntersect(sort_bag),
      (ATerm) gsMakeOpIdBag2Set(sort_bag, sort_set),
      (ATerm) gsMakeOpIdSet2Bag(sort_set, sort_bag));

  ATermList new_data_eqns = old_build_bag_equations(sort_elt, sort_bag, sort_set);

  new_data_eqns = impl_exprs_list(new_data_eqns, p_substs, p_data_decls);

  //perform substitutions
  new_ops = gsSubstValues_List(*p_substs, new_ops, true);
  p_data_decls->ops = ATconcat(new_ops, p_data_decls->ops);
  new_data_eqns = gsSubstValues_List(*p_substs, new_data_eqns, true);
  p_data_decls->data_eqns = ATconcat(new_data_eqns, p_data_decls->data_eqns);

  //add implementation of sort Nat, if necessary
  if (ATindexOf(p_data_decls->sorts, (ATerm) gsMakeSortIdNat(), 0) == -1) {
    impl_sort_nat(p_data_decls);
  }
  return sort_id;
}

void old_impl_standard_functions_sort(ATermAppl sort, t_data_decls *p_data_decls)
{
  assert(gsIsSortExpr(sort));
  //Declare operations for sort
  p_data_decls->ops = ATconcat(ATmakeList(7,
      (ATerm) gsMakeOpIdEq(sort),
      (ATerm) gsMakeOpIdNeq(sort),
      (ATerm) gsMakeOpIdIf(sort),
      (ATerm) gsMakeOpIdLT(sort),
      (ATerm) gsMakeOpIdLTE(sort),
      (ATerm) gsMakeOpIdGTE(sort),
      (ATerm) gsMakeOpIdGT(sort)
    ), p_data_decls->ops);
  //Declare new_data equations for sort sort
  ATermAppl x = gsMakeDataVarId(gsString2ATermAppl("x"), sort);
  ATermAppl y = gsMakeDataVarId(gsString2ATermAppl("y"), sort);
  ATermAppl b = gsMakeDataVarId(gsString2ATermAppl("b"), gsMakeSortExprBool());
  ATermAppl nil = gsMakeNil();
  ATermAppl t = gsMakeDataExprTrue();
  ATermAppl f = gsMakeDataExprFalse();
  ATermList xl = ATmakeList1((ATerm) x);
  ATermList xyl = ATmakeList2((ATerm) x, (ATerm) y);
  ATermList bxl = ATmakeList2((ATerm) b, (ATerm) x);
  p_data_decls->data_eqns = ATconcat(ATmakeList(9,
      //equality (sort_arrow -> sort_arrow -> Bool)
      (ATerm) gsMakeDataEqn(xl, nil,
        gsMakeDataExprEq(x, x), t),
      //inequality (sort_arrow -> sort_arrow -> Bool)
      (ATerm) gsMakeDataEqn(xyl,nil,
        gsMakeDataExprNeq(x, y),
        gsMakeDataExprNot(gsMakeDataExprEq(x, y))),
      //conditional (Bool -> sort_arrow -> sort_arrow -> sort_arrow)
      (ATerm) gsMakeDataEqn(xyl,nil,
        gsMakeDataExprIf(t, x, y),
        x),
      (ATerm) gsMakeDataEqn(xyl,nil,
        gsMakeDataExprIf(f, x, y),
        y),
      (ATerm) gsMakeDataEqn(bxl,nil,
        gsMakeDataExprIf(b, x, x),
        x),
      //less than (sort # sort -> Bool)
      (ATerm) gsMakeDataEqn(xl, nil,
        gsMakeDataExprLT(x, x), f),
      //less than or equal (sort # sort -> Bool)
      (ATerm) gsMakeDataEqn(xl, nil,
        gsMakeDataExprLTE(x, x), t),
      //greater than or equal (sort # sort -> Bool)
      (ATerm) gsMakeDataEqn(xyl,nil,
        gsMakeDataExprGTE(x, y),
        gsMakeDataExprLTE(y, x)),
      //greater than (sort # sort -> Bool)
      (ATerm) gsMakeDataEqn(xyl,nil,
        gsMakeDataExprGT(x, y),
        gsMakeDataExprLT(y, x))
    ), p_data_decls->data_eqns);
}

ATermAppl old_impl_sort_struct(ATermAppl sort_struct,
     ATermList *p_substs, t_data_decls *p_data_decls, bool recursive = true)
{
  assert(gsIsSortStruct(sort_struct));
  //declare fresh sort identifier for sort_struct 
  ATermAppl sort_id = make_fresh_struct_sort_id((ATerm) p_data_decls->sorts); 
  assert(gsIsSortId(sort_id));
  assert(gsCount((ATerm) sort_id, (ATerm) p_data_decls->sorts) == 0);
  //declare sort sort_id as representative of sort sort_struct
  p_data_decls->sorts = ATinsert(p_data_decls->sorts, (ATerm) sort_id);
  //add substitution sort_struct -> sort_id
  ATermAppl subst = gsMakeSubst_Appl(sort_struct, sort_id);
  *p_substs = gsAddSubstToSubsts(subst, *p_substs);
  //store constructor, projection and recogniser operations for this identifier
  ATermList cons_ops = ATmakeList0();
  ATermList proj_ops = ATmakeList0();
  ATermList projs = ATmakeList0();
  ATermList rec_ops = ATmakeList0();
  ATermList recs = ATmakeList0();
  ATermList struct_conss = ATLgetArgument(sort_struct, 0);
  while (!ATisEmpty(struct_conss))
  {
    ATermAppl struct_cons = ATAgetFirst(struct_conss);
    ATermAppl cons_name = ATAgetArgument(struct_cons, 0);
    ATermList struct_projs = ATLgetArgument(struct_cons, 1);
    ATermAppl rec_name = ATAgetArgument(struct_cons, 2);
    ATermList struct_cons_sorts = ATmakeList0();
    //store projection operations in proj_ops and store the implementations of
    //the sorts in struct_cons_sorts
    int i = 0;
    while (!ATisEmpty(struct_projs))
    {
      ATermAppl struct_proj = ATAgetFirst(struct_projs);
      ATermAppl proj_name = ATAgetArgument(struct_proj, 0);
      ATermAppl proj_sort = ATAgetArgument(struct_proj, 1);
      if (recursive) {
        proj_sort = impl_exprs_appl(proj_sort, p_substs, p_data_decls);
      }
      struct_cons_sorts = ATinsert(struct_cons_sorts, (ATerm) proj_sort);
      //store projection operation in proj_ops and projs
      if (!gsIsNil(proj_name)) {
        ATermAppl proj_op = gsMakeOpId(proj_name, gsMakeSortArrow1(sort_id, proj_sort));
        proj_ops = ATinsert(proj_ops, (ATerm) proj_op);
        projs = ATinsert(projs, (ATerm) ATmakeList2((ATerm) proj_op, (ATerm) ATmakeInt(i)));
      }
      struct_projs = ATgetNext(struct_projs);
      i++;
    }
    struct_cons_sorts = ATreverse(struct_cons_sorts);
    //store constructor operation in cons_ops
    ATermAppl cons_op = gsMakeOpId(cons_name, gsMakeSortArrowList(struct_cons_sorts, sort_id));
    cons_ops = ATinsert(cons_ops, (ATerm) cons_op);
    //store recogniser in rec_ops and recs
    if (!gsIsNil(rec_name)) {
      ATermAppl rec_op = gsMakeOpId(rec_name, gsMakeSortArrow1(sort_id, gsMakeSortExprBool()));
      rec_ops = ATinsert(rec_ops, (ATerm) rec_op);
      recs = ATinsert(recs, (ATerm) ATmakeList2((ATerm) rec_op, (ATerm) cons_op));
    }
    //add constructor to projs
    ATermList tmpl = ATmakeList0();
    for (; !ATisEmpty(projs); projs=ATgetNext(projs))
    {
      tmpl = ATinsert(tmpl, (ATerm) ATappend(ATLgetFirst(projs), (ATerm) cons_op));
    }
    projs = ATreverse(tmpl);
    struct_conss = ATgetNext(struct_conss);
  }
  cons_ops = ATreverse(cons_ops);
  proj_ops = ATreverse(proj_ops);
  projs = ATreverse(projs);
  rec_ops = ATreverse(rec_ops);
  recs = ATreverse(recs);
  //add declarations for the constructor, projection and recogniser operations
  p_data_decls->cons_ops = ATconcat(cons_ops, p_data_decls->cons_ops);
  p_data_decls->ops = ATconcat(ATconcat(proj_ops, rec_ops), p_data_decls->ops);
  //Declare new_data equations for structured sort
  // XXX more intelligent variable names would be nice
  ATermList vars = ATmakeList3(
    (ATerm) gsMakeDataVarId(gsString2ATermAppl("b"), gsMakeSortExprBool()),
    (ATerm) gsMakeDataVarId(gsString2ATermAppl("x"), sort_id),
    (ATerm) gsMakeDataVarId(gsString2ATermAppl("y"), sort_id)
  );
  ATermList id_ctx = ATconcat(p_data_decls->sorts,
                      ATconcat(p_data_decls->ops,p_data_decls->cons_ops));
  //Create equations for projection functions
  ATermList proj_eqns = ATmakeList0();
  for (; !ATisEmpty(projs); projs=ATgetNext(projs))
  {
    ATermList l = ATLgetFirst(projs);
    ATermAppl proj_op = ATAgetFirst(l);
    l = ATgetNext(l);
    int proj_op_index = ATgetInt((ATermInt) ATgetFirst(l));
    l = ATgetNext(l);
    ATermAppl cons_op = ATAgetFirst(l);
    //Create (fresh) variable arguments for constructor cons_op
    ATermList args = create_op_id_args(cons_op, &vars, (ATerm) id_ctx);
    //Create equation for projection function proj_op
    proj_eqns = ATinsert(proj_eqns,
      (ATerm) gsMakeDataEqn(args, gsMakeNil(),
        gsMakeDataAppl1(proj_op, gsMakeDataApplList(cons_op, args)),
        ATAelementAt(args, proj_op_index)));
  }
  //Create equations for recognition functions
  ATermList rec_eqns = ATmakeList0();
  for (; !ATisEmpty(recs); recs=ATgetNext(recs))
  {
    ATermList l = ATLgetFirst(recs);
    ATermAppl rec_op = ATAgetFirst(l);
    l = ATgetNext(l);
    ATermAppl rec_cons_op = ATAgetFirst(l);
    //Create equation for every constructor
    for (ATermList m = cons_ops; !ATisEmpty(m); m=ATgetNext(m))
    {
      ATermAppl cons_op = ATAgetFirst(m);
      //Create (fresh) variable arguments for constructor cons_op
      ATermList args = create_op_id_args(cons_op, &vars, (ATerm) id_ctx);
      //Create equation for recognition function rec_op and constructor cons_op
      rec_eqns = ATinsert(rec_eqns,
         (ATerm) gsMakeDataEqn(args, gsMakeNil(),
           gsMakeDataAppl1(rec_op, gsMakeDataApplList(cons_op, args)),
           (ATisEqual(cons_op, rec_cons_op)) ? gsMakeDataExprTrue() : gsMakeDataExprFalse()));
    }
  }
  //Create equations for ==, <, and <= for every pair of constructors
  ATermList eq_eqns = ATmakeList0();
  ATermList lt_eqns = ATmakeList0();
  ATermList lte_eqns = ATmakeList0();
  int i = 0; //represents the index of the constructor on the left-hand side
  for (ATermList l=cons_ops; !ATisEmpty(l); l=ATgetNext(l), i++)
  {
    int j = 0; //represents the index of the constructor on the right-hand side
    for (ATermList m=cons_ops; !ATisEmpty(m); m=ATgetNext(m), j++)
    {
      ATermAppl cons_op_lhs = ATAgetFirst(l);
      ATermAppl cons_op_rhs = ATAgetFirst(m);
      // Create (fresh) variable arguments for constructor cons_op_lhs
      ATermList args_lhs = create_op_id_args(cons_op_lhs, &vars, (ATerm) id_ctx);
      // Create (fresh) variable arguments for constructor cons_op_rhs,
      // where we make sure that we don't use the vars that occur in args_lhs 
      ATermList tmpvars = subtract_list(vars, args_lhs);
      ATermList args_rhs = create_op_id_args(cons_op_rhs, &tmpvars, (ATerm) ATconcat(args_lhs, id_ctx));
      // Update vars
      vars = merge_list(vars, args_rhs);
      // Create right-hand sides for ==, <, <=
      ATermAppl eq_rhs;
      ATermAppl lt_rhs;
      ATermAppl lte_rhs;
      if (i!=j) {
        // Constructors are different:
        // - rhs for c_i == d_i: false
        // - rhs for c_i <  d_i: i < j
        // - rhs for c_i <= d_i: i < j
        eq_rhs  = gsMakeDataExprFalse();
        lt_rhs  = (i < j) ? gsMakeDataExprTrue() : gsMakeDataExprFalse();
        lte_rhs = lt_rhs;
      } else { // i==j
        // Constructors are the same
        if (ATisEmpty(args_lhs)) {
          // Constructors do not have any arguments:
          // - rhs for c == c: true
          // - rhs for c <  c: false
          // - rhs for c <= c: true
          eq_rhs  = gsMakeDataExprTrue();
          lt_rhs  = gsMakeDataExprFalse();
          lte_rhs = gsMakeDataExprTrue();
        } else {
          // Constructors have one or more arguments:
          // - rhs for c(x0,...,xn) == c(y0,..,yn):
          //     x0 == y0 && ... && xn == yn
          // - rhs for c(x0,...,xn) < c(y0,..,yn):
          //     x0 < y0                                                     , when n = 0
          //     x0 < y0 || (x0 == y0 && x1 < y1)                            , when n = 1
          //     x0 < y0 || (x0 == y0 && (x1 < y1 || (x1 == x1 && x2 < y2))) , when n = 2
          //     etcetera
          // - rhs for c(x0,...,xn) <= c(y0,..,yn):
          //     x0 <= y0                                                    , when n = 0
          //     x0 < y0 || (x0 == y0 && x1 <= y1)                           , when n = 1
          //     x0 < y0 || (x0 == y0 && (x1 < y1 || (x1 == x1 && x2 <= y2))), when n = 2
          //     etcetera
          ATermList n = ATreverse(args_lhs);
          ATermList o = ATreverse(args_rhs);
          ATermAppl n0 = ATAgetFirst(n);
          ATermAppl o0 = ATAgetFirst(o);
          eq_rhs  = gsMakeDataExprEq(n0, o0);
          lt_rhs  = gsMakeDataExprLT(n0, o0);
          lte_rhs = gsMakeDataExprLTE(n0, o0);
          n = ATgetNext(n);
          o = ATgetNext(o);
          for ( ; !ATisEmpty(n) ; n = ATgetNext(n), o = ATgetNext(o)) {
            n0 = ATAgetFirst(n);
            o0 = ATAgetFirst(o);
            eq_rhs  = gsMakeDataExprAnd(gsMakeDataExprEq(n0, o0), eq_rhs);
            lt_rhs  = gsMakeDataExprOr(gsMakeDataExprLT(n0, o0), gsMakeDataExprAnd(gsMakeDataExprEq(n0, o0), lt_rhs));
            lte_rhs = gsMakeDataExprOr(gsMakeDataExprLT(n0, o0), gsMakeDataExprAnd(gsMakeDataExprEq(n0, o0), lte_rhs));
          }
        }
      }
      //Create equations for ==, < and <= for constructors i and j
      ATermList args = ATconcat(args_lhs, args_rhs);
      ATermAppl cons_expr_lhs = gsMakeDataApplList(cons_op_lhs, args_lhs);
      ATermAppl cons_expr_rhs = gsMakeDataApplList(cons_op_rhs, args_rhs);
      eq_eqns = ATinsert(eq_eqns, (ATerm) gsMakeDataEqn(args, gsMakeNil(), gsMakeDataExprEq(cons_expr_lhs, cons_expr_rhs), eq_rhs));
      lt_eqns = ATinsert(lt_eqns, (ATerm) gsMakeDataEqn(args, gsMakeNil(), gsMakeDataExprLT(cons_expr_lhs, cons_expr_rhs), lt_rhs));
      lte_eqns = ATinsert(lte_eqns, (ATerm) gsMakeDataEqn(args, gsMakeNil(), gsMakeDataExprLTE(cons_expr_lhs, cons_expr_rhs), lte_rhs));
    }
  }
  //Add equations to data_eqns
  p_data_decls->data_eqns =
    ATconcat(ATreverse(eq_eqns),
    ATconcat(ATreverse(lt_eqns),
    ATconcat(ATreverse(lte_eqns),
    ATconcat(ATreverse(proj_eqns),
    ATconcat(ATreverse(rec_eqns),
      p_data_decls->data_eqns
    )))));

  return sort_id;
}

void implement_bool_test()
{
  t_data_decls data_decls_old;
  t_data_decls data_decls_new;
  initialize_data_decls(&data_decls_old);
  initialize_data_decls(&data_decls_new);
  old_impl_sort_bool(&data_decls_old);
  impl_sort_bool(&data_decls_new);

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

  BOOST_CHECK(data_decls_old.sorts     == data_decls_new.sorts);
  BOOST_CHECK(data_decls_old.cons_ops  == data_decls_new.cons_ops);
  BOOST_CHECK(data_decls_old.ops       == data_decls_new.ops);
  std::clog << "OLD " << mcrl2::core::pp(data_decls_old.ops) << std::endl
            << "NEW " << mcrl2::core::pp(data_decls_new.ops) << std::endl;
  BOOST_CHECK(data_decls_old.data_eqns == data_decls_new.data_eqns);
  std::clog << "OLD " << mcrl2::core::pp(data_decls_old.data_eqns) << std::endl
            << "NEW " << mcrl2::core::pp(data_decls_new.data_eqns) << std::endl;
}

void implement_nat_test()
{
  t_data_decls data_decls_old;
  t_data_decls data_decls_new;
  initialize_data_decls(&data_decls_old);
  initialize_data_decls(&data_decls_new);
  old_impl_sort_nat(&data_decls_old);
  impl_sort_nat(&data_decls_new);

  BOOST_CHECK(data_decls_old.sorts     == data_decls_new.sorts);
  BOOST_CHECK(data_decls_old.cons_ops  == data_decls_new.cons_ops);
  BOOST_CHECK(data_decls_old.ops       == data_decls_new.ops);
  std::clog << "OLD " << mcrl2::core::pp(data_decls_old.ops) << std::endl
            << "NEW " << mcrl2::core::pp(data_decls_new.ops) << std::endl;
  BOOST_CHECK(data_decls_old.data_eqns == data_decls_new.data_eqns);
  std::clog << "OLD " << mcrl2::core::pp(data_decls_old.data_eqns) << std::endl
            << "NEW " << mcrl2::core::pp(data_decls_new.data_eqns) << std::endl;
}

void implement_int_test()
{
  t_data_decls data_decls_old;
  t_data_decls data_decls_new;
  initialize_data_decls(&data_decls_old);
  initialize_data_decls(&data_decls_new);
  old_impl_sort_int(&data_decls_old);
  impl_sort_int(&data_decls_new);

  BOOST_CHECK(data_decls_old.sorts     == data_decls_new.sorts);
  BOOST_CHECK(data_decls_old.cons_ops  == data_decls_new.cons_ops);
  BOOST_CHECK(data_decls_old.ops       == data_decls_new.ops);
  std::clog << "OLD " << mcrl2::core::pp(data_decls_old.ops) << std::endl
            << "NEW " << mcrl2::core::pp(data_decls_new.ops) << std::endl;
  BOOST_CHECK(data_decls_old.data_eqns == data_decls_new.data_eqns);
  std::clog << "OLD " << mcrl2::core::pp(data_decls_old.data_eqns) << std::endl
            << "NEW " << mcrl2::core::pp(data_decls_new.data_eqns) << std::endl;
}

void implement_real_test()
{
  t_data_decls data_decls_old;
  t_data_decls data_decls_new;
  initialize_data_decls(&data_decls_old);
  initialize_data_decls(&data_decls_new);
  old_impl_sort_real(&data_decls_old);
  impl_sort_real(&data_decls_new);

  BOOST_CHECK(data_decls_old.sorts     == data_decls_new.sorts);
  BOOST_CHECK(data_decls_old.cons_ops  == data_decls_new.cons_ops);
  BOOST_CHECK(data_decls_old.ops       == data_decls_new.ops);
  std::clog << "OLD " << mcrl2::core::pp(data_decls_old.ops) << std::endl
            << "NEW " << mcrl2::core::pp(data_decls_new.ops) << std::endl;
  BOOST_CHECK(data_decls_old.data_eqns == data_decls_new.data_eqns);
  std::clog << "OLD " << mcrl2::core::pp(data_decls_old.data_eqns) << std::endl
            << "NEW " << mcrl2::core::pp(data_decls_new.data_eqns) << std::endl;
}

void implement_list_test()
{
  t_data_decls data_decls_old;
  t_data_decls data_decls_new;
  initialize_data_decls(&data_decls_old);
  initialize_data_decls(&data_decls_new);
  // TODO: Clean up
  ATermList old_substs = ATmakeList0();
  ATermList new_substs = ATmakeList0();
  basic_sort s("S");
  container_sort ls(new_data::sort_list::list(s));
  old_impl_sort_list(ls, &old_substs, &data_decls_old);
  impl_sort_list(ls, &new_substs, &data_decls_new);

  BOOST_CHECK(data_decls_old.sorts     == data_decls_new.sorts);
  BOOST_CHECK(data_decls_old.cons_ops  == data_decls_new.cons_ops);
  BOOST_CHECK(data_decls_old.ops       == data_decls_new.ops);
  std::clog << "OLD " << mcrl2::core::pp(data_decls_old.ops) << std::endl
            << "NEW " << mcrl2::core::pp(data_decls_new.ops) << std::endl;
  BOOST_CHECK(data_decls_old.data_eqns == data_decls_new.data_eqns);
  std::clog << "OLD " << mcrl2::core::pp(data_decls_old.data_eqns) << std::endl
            << "NEW " << mcrl2::core::pp(data_decls_new.data_eqns) << std::endl;
}

void implement_set_test()
{
  t_data_decls data_decls_old;
  t_data_decls data_decls_new;
  initialize_data_decls(&data_decls_old);
  initialize_data_decls(&data_decls_new);
  // TODO: Clean up
  ATermList old_substs = ATmakeList0();
  ATermList new_substs = ATmakeList0();
  basic_sort s("S");
  container_sort ss(new_data::sort_set::set(s));
  old_impl_sort_set(ss, &old_substs, &data_decls_old);
  impl_sort_set(ss, &new_substs, &data_decls_new);

  BOOST_CHECK(data_decls_old.sorts     == data_decls_new.sorts);
  BOOST_CHECK(data_decls_old.cons_ops  == data_decls_new.cons_ops);
  BOOST_CHECK(data_decls_old.ops       == data_decls_new.ops);
  BOOST_CHECK(data_decls_old.data_eqns == data_decls_new.data_eqns);
  std::clog << "OLD " << mcrl2::core::pp(data_decls_old.ops) << std::endl
            << "NEW " << mcrl2::core::pp(data_decls_new.ops) << std::endl;
  BOOST_CHECK(data_decls_old.data_eqns == data_decls_new.data_eqns);
  std::clog << "OLD " << mcrl2::core::pp(data_decls_old.data_eqns) << std::endl
            << "NEW " << mcrl2::core::pp(data_decls_new.data_eqns) << std::endl;
}

void implement_bag_test()
{
  t_data_decls data_decls_old;
  t_data_decls data_decls_new;
  initialize_data_decls(&data_decls_old);
  initialize_data_decls(&data_decls_new);
  // TODO: Clean up
  ATermList old_substs = ATmakeList0();
  ATermList new_substs = ATmakeList0();
  basic_sort s("S");
  container_sort bs(new_data::sort_bag::bag(s));
  old_impl_sort_bag(bs, &old_substs, &data_decls_old);
  impl_sort_bag(bs, &new_substs, &data_decls_new);

  BOOST_CHECK(data_decls_old.sorts     == data_decls_new.sorts);
  BOOST_CHECK(data_decls_old.cons_ops  == data_decls_new.cons_ops);
  BOOST_CHECK(data_decls_old.ops       == data_decls_new.ops);
  BOOST_CHECK(data_decls_old.data_eqns == data_decls_new.data_eqns);
  std::clog << "OLD " << mcrl2::core::pp(data_decls_old.ops) << std::endl
            << "NEW " << mcrl2::core::pp(data_decls_new.ops) << std::endl;
  BOOST_CHECK(data_decls_old.data_eqns == data_decls_new.data_eqns);
  std::clog << "OLD " << mcrl2::core::pp(data_decls_old.data_eqns) << std::endl
            << "NEW " << mcrl2::core::pp(data_decls_new.data_eqns) << std::endl;
}

void implement_standard_functions_test()
{
  basic_sort s("S");
  t_data_decls data_decls_old;
  t_data_decls data_decls_new;
  initialize_data_decls(&data_decls_old);
  initialize_data_decls(&data_decls_new);
  old_impl_standard_functions_sort(s, &data_decls_old);
  impl_standard_functions_sort(s, &data_decls_new);

  std::clog << "old constructors:" << std::endl;
  for(aterm_list::iterator i = aterm_list(data_decls_old.cons_ops).begin(); i != aterm_list(data_decls_old.cons_ops).end(); ++i)
  {
    std::clog << *i << std::endl;
  }
  std::clog << "new constructors:" << std::endl;
  for(aterm_list::iterator i = aterm_list(data_decls_new.cons_ops).begin(); i != aterm_list(data_decls_new.cons_ops).end(); ++i)
  {
    std::clog << *i << std::endl;
  }
  std::clog << "old functions:" << std::endl;
  for(aterm_list::iterator i = aterm_list(data_decls_old.ops).begin(); i != aterm_list(data_decls_old.ops).end(); ++i)
  {
    std::clog << *i << std::endl;
  }
  std::clog << "new functions:" << std::endl;
  for(aterm_list::iterator i = aterm_list(data_decls_new.ops).begin(); i != aterm_list(data_decls_new.ops).end(); ++i)
  {
    std::clog << *i << std::endl;
  }
  std::clog << "old equations:" << std::endl;
  for(aterm_list::iterator i = aterm_list(data_decls_old.data_eqns).begin(); i != aterm_list(data_decls_old.data_eqns).end(); ++i)
  {
    std::clog << *i << std::endl;
  }
  std::clog << "new equations:" << std::endl;
  for(aterm_list::iterator i = aterm_list(data_decls_new.data_eqns).begin(); i != aterm_list(data_decls_new.data_eqns).end(); ++i)
  {
    std::clog << *i << std::endl;
  }

  BOOST_CHECK(data_decls_old.sorts     == data_decls_new.sorts);
  BOOST_CHECK(data_decls_old.cons_ops  == data_decls_new.cons_ops);
  BOOST_CHECK(data_decls_old.ops       == data_decls_new.ops);
  BOOST_CHECK(data_decls_old.data_eqns == data_decls_new.data_eqns);
}

void implement_structured_sort_test()
{
  t_data_decls data_decls_old;
  t_data_decls data_decls_new;
  initialize_data_decls(&data_decls_old);
  initialize_data_decls(&data_decls_new);
  // TODO: Clean up
  ATermList old_substs = ATmakeList0();
  ATermList new_substs = ATmakeList0();
  atermpp::vector< new_data::structured_sort_constructor_argument > arguments;
  arguments.push_back(new_data::structured_sort_constructor_argument("a0", basic_sort("A")));
  arguments.push_back(new_data::structured_sort_constructor_argument(basic_sort("B")));
  arguments.push_back(new_data::structured_sort_constructor_argument("n0", sort_nat::nat()));

  atermpp::vector< new_data::structured_sort_constructor > constructors;
  // without arguments or recogniser
  constructors.push_back(new_data::structured_sort_constructor("c0"));
  // without arguments, with recogniser
  constructors.push_back(new_data::structured_sort_constructor("c1", std::string("is_one")));
  // with arguments, without recogniser
  constructors.push_back(new_data::structured_sort_constructor("a",
     boost::make_iterator_range(arguments.begin(), arguments.begin() + 1)));
  // two arguments, with recogniser
  constructors.push_back(new_data::structured_sort_constructor("b",
     boost::make_iterator_range(arguments.begin() + 1, arguments.begin() + 2), "is_b"));
  constructors.push_back(new_data::structured_sort_constructor("c",
     boost::make_iterator_range(arguments.begin() + 1, arguments.end()), "is_c"));

  new_data::structured_sort ls(boost::make_iterator_range(constructors));
  old_impl_sort_struct(ls, &old_substs, &data_decls_old);
  impl_exprs_appl(ls, &new_substs, &data_decls_new);

  BOOST_CHECK(data_decls_old.sorts     == data_decls_new.sorts);
  std::clog << "OLD " << mcrl2::core::pp(data_decls_old.sorts) << std::endl
            << "NEW " << mcrl2::core::pp(data_decls_new.sorts) << std::endl;
  BOOST_CHECK(data_decls_old.cons_ops  == data_decls_new.cons_ops);
  std::clog << "OLD " << mcrl2::core::pp(data_decls_old.cons_ops) << std::endl
            << "NEW " << mcrl2::core::pp(data_decls_new.cons_ops) << std::endl;
  BOOST_CHECK(data_decls_old.ops       == data_decls_new.ops);
  std::clog << "OLD " << mcrl2::core::pp(data_decls_old.ops) << std::endl
            << "NEW " << mcrl2::core::pp(data_decls_new.ops) << std::endl;
  BOOST_CHECK(data_decls_old.data_eqns == data_decls_new.data_eqns);
  std::clog << "OLD " << mcrl2::core::pp(data_decls_old.data_eqns) << std::endl
            << "NEW " << mcrl2::core::pp(data_decls_new.data_eqns) << std::endl;
}

void implement_data_specification_test()
{
  const std::string text(
    "sort S;\n"
    "cons s:S;\n"
    "map f:S -> List(S);\n"
  );

  new_data::data_specification spec(new_data::parse_data_specification(text));
  atermpp::aterm_appl impl_spec = new_data::detail::implement_data_specification(spec);
  atermpp::aterm_appl old_impl_spec = new_data::parse_data_specification_and_implement(text);

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
  implement_list_test();
  implement_set_test();
  implement_bag_test();
  implement_structured_sort_test();
  implement_standard_functions_test();
  implement_data_specification_test();

  return EXIT_SUCCESS;
}
