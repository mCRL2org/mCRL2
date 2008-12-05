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
using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace mcrl2::data;
using namespace mcrl2::data::data_expr;

inline
data_operation round()
{
  sort_expression ri = sort_arrow(make_list(sort_expr::real()), sort_expr::int_());
  return data_operation(identifier_string("@round"), ri);
}

inline
data_application round(const data_expression& r)
{
  return data_application(round(), make_list(r));
}

inline
data_operation trunc()
{
  sort_expression ri = sort_arrow(make_list(sort_expr::real()), sort_expr::int_());
  return data_operation(identifier_string("@trunc"), ri);
}

inline
data_application trunc(const data_expression& r)
{
  return data_application(trunc(), make_list(r));
}

inline
data_operation divide()
{
  sort_expression rrr = sort_arrow(make_list(sort_expr::real(), sort_expr::real()), sort_expr::real());
  return data_operation(identifier_string("@divide"), rrr);
}

inline
data_application divide(const data_expression& r1, const data_expression& r2)
{
  return data_application(divide(), make_list(r1,r2));
}

inline
data_operation invert()
{
  sort_expression rr = sort_arrow(make_list(sort_expr::real()), sort_expr::real());
  return data_operation(identifier_string("@invert"), rr);
}

inline
data_application invert(const data_expression& r)
{
  return data_application(invert(), make_list(r));
}

inline
data_operation rational()
{
  sort_expression ipr = sort_arrow(make_list(sort_expr::int_(), sort_expr::pos()), sort_expr::real());
  return data_operation(identifier_string("@rational"), ipr);
}

inline
data_application rational(const data_expression& i, const data_expression& p)
{
  return data_application(rational(), make_list(i,p));
}

inline
bool is_rational(const data_expression& e)
{
  return is_data_application(e) && static_cast<const data_application&>(e).head() == rational();
}

inline
data_operation normalize_rational()
{
  sort_expression iir = sort_arrow(make_list(sort_expr::int_(), sort_expr::int_()), sort_expr::real());
  return data_operation(identifier_string("@normalize_rational"), iir);
}

inline
data_application normalize_rational(const data_expression& i1, const data_expression& i2)
{
  return data_application(normalize_rational(), make_list(i1, i2));
}

inline
data_operation gcd()
{
  sort_expression rrr = sort_arrow(make_list(sort_expr::real(), sort_expr::real()), sort_expr::real());
  return data_operation(identifier_string("@gcd"), rrr);
}

inline
data_application gcd(const data_expression& r1, const data_expression& r2)
{
  return data_application(gcd(), make_list(r1,r2));
}

inline
data_operation numerator()
{
  sort_expression ri = sort_arrow(make_list(sort_expr::real()), sort_expr::int_());
  return data_operation(identifier_string("@numerator"), ri);
}

inline
data_application numerator(const data_expression& r)
{
  return data_application(numerator(), make_list(r));
}

inline
data_operation denominator()
{
  sort_expression ri = sort_arrow(make_list(sort_expr::real()), sort_expr::int_());
  return data_operation(identifier_string("@denominator"), ri);
}

inline
data_application denominator(const data_expression& r)
{
  return data_application(denominator(), make_list(r));
}

inline
data_expression real_zero()
{
  return rational(int_(0), pos(1));
}

inline
data_operation_list additional_real_mappings()
{
  sort_expression rpr = sort_arrow(make_list(sort_expr::real(), sort_expr::pos()), sort_expr::real());

  data_operation_list r;

  r = push_front(r,round());
  r = push_front(r,trunc());
  r = push_front(r,divide());
  r = push_front(r,invert());
  r = push_front(r,rational());
  r = push_front(r,normalize_rational());
  r = push_front(r,gcd());
  //r = push_front(r,data_operation(core::detail::gsMakeOpId(core::detail::gsMakeOpIdNameExp(), rpr)));
  r = push_front(r,numerator());
  r = push_front(r, denominator());

  return r;
}

inline
data_equation_list additional_real_equations()
{
  data_variable b("b", sort_expr::bool_());
  data_variable p("p", sort_expr::pos());
  data_variable q("q", sort_expr::pos());
  data_variable n("n", sort_expr::nat());
  data_variable x("x", sort_expr::int_());
  data_variable y("y", sort_expr::int_());
  data_variable r("r", sort_expr::real());
  data_variable s("s", sort_expr::real());

  using namespace core::detail;
  data_equation_list res;

  res = push_front(res, data_equation(make_list(x), gsMakeNil(), gsMakeDataExprCReal(x), rational(x, pos(1))));
  res = push_front(res, data_equation(make_list(p), gsMakeNil(), gsMakeDataExprPos2Real(p), rational(gsMakeDataExprPos2Int(p), pos(1))));
  res = push_front(res, data_equation(make_list(n), gsMakeNil(), gsMakeDataExprNat2Real(n), rational(gsMakeDataExprNat2Int(n), pos(1))));
  res = push_front(res, data_equation(make_list(x), gsMakeNil(), gsMakeDataExprInt2Real(x), rational(x, pos(1))));
  res = push_front(res, data_equation(make_list(x), gsMakeNil(), gsMakeDataExprReal2Pos(rational(x, pos(1))), gsMakeDataExprInt2Pos(x)));
  res = push_front(res, data_equation(make_list(x), gsMakeNil(), gsMakeDataExprReal2Nat(rational(x, pos(1))), gsMakeDataExprInt2Nat(x)));
  res = push_front(res, data_equation(make_list(x), gsMakeNil(), gsMakeDataExprReal2Int(rational(x, pos(1))), x));
  res = push_front(res, data_equation(make_list(x,p), gsMakeNil(), trunc(rational(x,p)), divides(x,p)));
  res = push_front(res, data_equation(make_list(r), gsMakeNil(), round(r), trunc(plus(r,rational(int_(1),pos(2))))));
  res = push_front(res, data_equation(make_list(p,q,x,y), gsMakeNil(), equal_to(rational(x,p), rational(y,q)), equal_to(multiplies(x,gsMakeDataExprPos2Int(q)),multiplies(y,gsMakeDataExprPos2Int(p)))));
  res = push_front(res, data_equation(make_list(r), gsMakeNil(), less(r,r), false_()));
  res = push_front(res, data_equation(make_list(p,q,x,y), gsMakeNil(), less(rational(x,p),rational(y,q)), less(multiplies(x,gsMakeDataExprPos2Int(q)),multiplies(y,gsMakeDataExprPos2Int(p)))));
  res = push_front(res, data_equation(make_list(r), gsMakeNil(), less_equal(r,r), true_()));
  res = push_front(res, data_equation(make_list(p,q,x,y), gsMakeNil(), less_equal(rational(x,p),rational(y,q)), less_equal(multiplies(x,gsMakeDataExprPos2Int(q)),multiplies(y,gsMakeDataExprPos2Int(p)))));
  res = push_front(res, data_equation(make_list(r,s), gsMakeNil(), greater(r,s), less(s,r)));
  res = push_front(res, data_equation(make_list(r,s), gsMakeNil(), greater_equal(r,s), less_equal(s,r)));
  res = push_front(res, data_equation(make_list(r,s), gsMakeNil(), max_(r,s), if_(less(r,s), s, r)));
  res = push_front(res, data_equation(make_list(r,s), gsMakeNil(), min_(r,s), if_(less(r,s), r, s)));
  res = push_front(res, data_equation(make_list(r), gsMakeNil(), abs(r), if_(less(r,real_zero()),negate(r),r)));
  res = push_front(res, data_equation(make_list(x,p), gsMakeNil(), negate(rational(x,p)), rational(negate(x), p)));
  res = push_front(res, data_equation(make_list(r), gsMakeNil(), negate(negate(r)), r));
  res = push_front(res, data_equation(make_list(x,p), gsMakeNil(), gsMakeDataExprSucc(rational(x,p)), rational(plus(x,gsMakeDataExprPos2Int(p)),p)));
  res = push_front(res, data_equation(make_list(x,p), gsMakeNil(), gsMakeDataExprPred(rational(x,p)), rational(minus(x,gsMakeDataExprPos2Int(p)),p)));
  res = push_front(res, data_equation(make_list(x,p), greater(x,int_(0)), invert(rational(x,p)), rational(gsMakeDataExprPos2Int(p), gsMakeDataExprInt2Pos(x))));
  res = push_front(res, data_equation(make_list(x,p), less(x,int_(0)), invert(rational(x,p)), rational(negate(p), gsMakeDataExprInt2Pos(negate(x)))));
  res = push_front(res, data_equation(make_list(r), gsMakeNil(), plus(r, negate(r)), real_zero()));
  res = push_front(res, data_equation(make_list(r), gsMakeNil(), plus(negate(r), r), real_zero()));
  res = push_front(res, data_equation(make_list(r,p), gsMakeNil(), plus(r, rational(int_(0),p)), r));
  res = push_front(res, data_equation(make_list(r,p), gsMakeNil(), plus(rational(int_(0),p), r), r));
  res = push_front(res, data_equation(make_list(x,y,p,q), gsMakeNil(), plus(rational(x,p), rational(y,q)), normalize_rational(plus(multiplies(x,gsMakeDataExprPos2Int(q)), multiplies(y,gsMakeDataExprPos2Int(p))), gsMakeDataExprPos2Int(multiplies(p,q)))));
  res = push_front(res, data_equation(make_list(r), gsMakeNil(), minus(r,r), real_zero()));
  res = push_front(res, data_equation(make_list(r,p), gsMakeNil(), minus(r, rational(int_(0),p)), r));
  res = push_front(res, data_equation(make_list(r,p), gsMakeNil(), minus(rational(int_(0),p), r), negate(r)));
  res = push_front(res, data_equation(make_list(x,y,p,q), gsMakeNil(), minus(rational(x,p), rational(y,q)), normalize_rational(minus(multiplies(x,gsMakeDataExprPos2Int(q)), multiplies(y,gsMakeDataExprPos2Int(p))), gsMakeDataExprPos2Int(multiplies(p,q)))));
  res = push_front(res, data_equation(make_list(r,p), gsMakeNil(), multiplies(r, rational(int_(0), p)), real_zero()));
  res = push_front(res, data_equation(make_list(r,p), gsMakeNil(), multiplies(rational(int_(0), p), r), real_zero()));
  res = push_front(res, data_equation(make_list(r), gsMakeNil(), multiplies(rational(int_(1), pos(1)), r), r));
  res = push_front(res, data_equation(make_list(r), gsMakeNil(), multiplies(r, rational(int_(1), pos(1))), r));
  res = push_front(res, data_equation(make_list(x,y,p,q), gsMakeNil(), multiplies(rational(x,p), rational(y,q)), normalize_rational(multiplies(x,y), gsMakeDataExprPos2Int(multiplies(p,q)))));
  res = push_front(res, data_equation(make_list(r,p), gsMakeNil(), divide(rational(int_(0), p), r), real_zero()));
  res = push_front(res, data_equation(make_list(r), gsMakeNil(), divide(r, rational(int_(1), pos(1))), r));
  res = push_front(res, data_equation(make_list(x,y,p,q), gsMakeNil(), divide(rational(x,p), rational(y,q)), normalize_rational(multiplies(x,gsMakeDataExprPos2Int(q)), multiplies(y,gsMakeDataExprPos2Int(p)))));
  res = push_front(res, data_equation(make_list(r), gsMakeNil(), gsMakeDataExprExp(r, nat(0)), rational(int_(1), pos(1))));
  res = push_front(res, data_equation(make_list(r), gsMakeNil(), gsMakeDataExprExp(r, nat(1)), r));
  res = push_front(res, data_equation(make_list(r,p), gsMakeNil(), gsMakeDataExprExp(r, gsMakeDataExprCNat(gsMakeDataExprCDub(gsMakeDataExprTrue(),p))), multiplies(r,multiplies(gsMakeDataExprExp(r,gsMakeDataExprPos2Nat(p)), gsMakeDataExprExp(r,gsMakeDataExprPos2Nat(p))))));
  res = push_front(res, data_equation(make_list(r,p), gsMakeNil(), gsMakeDataExprExp(r,gsMakeDataExprPos2Nat(gsMakeDataExprCDub(gsMakeDataExprFalse(),p))), multiplies(gsMakeDataExprExp(r,gsMakeDataExprPos2Nat(p)), gsMakeDataExprExp(r,gsMakeDataExprPos2Nat(p)))));
  //res = push_front(res, data_equation(make_list(r,p), gsMakeNil(), gsMakeDataExprExp(r, gsMakeDataExprPos2Nat(p)), gsMakeDataExprExp(r,p)));
  //res = push_front(res, data_equation(make_list(r), gsMakeNil(), gsMakeDataExprExp(r, pos(1)), r));
  //res = push_front(res, data_equation(make_list(r,p), gsMakeNil(), gsMakeDataExprExp(r, gsMakeDataExprSucc(multiplies(pos(2), p))), multiplies(r,multiplies(gsMakeDataExprExp(r,p), gsMakeDataExprExp(r,p)))));
  //res = push_front(res, data_equation(make_list(r,p), gsMakeNil(), gsMakeDataExprExp(r,multiplies(pos(2),p)), multiplies(gsMakeDataExprExp(r,p), gsMakeDataExprExp(r,p))));

  res = push_front(res, data_equation(make_list(x,p), gsMakeNil(), numerator(rational(x,p)), x));
  res = push_front(res, data_equation(make_list(x,p), gsMakeNil(), denominator(rational(x,p)), p));
  res = push_front(res, data_equation(make_list(x,p), gsMakeNil(), normalize_rational(x,negate(p)), normalize_rational(negate(x), gsMakeDataExprPos2Int(p))));
  res = push_front(res, data_equation(make_list(x,p), gsMakeNil(), normalize_rational(x,gsMakeDataExprPos2Int(p)), if_(equal_to(modulus(x,p), nat(0)), rational(divides(x,p), pos(1)), rational(plus(denominator(normalize_rational(gsMakeDataExprPos2Int(p),gsMakeDataExprNat2Int(modulus(x,p)))), multiplies(divides(x,p), numerator(normalize_rational(gsMakeDataExprPos2Int(p),gsMakeDataExprNat2Int(modulus(x,p)))))), gsMakeDataExprInt2Pos(numerator(normalize_rational(gsMakeDataExprPos2Int(p),gsMakeDataExprNat2Int(modulus(x,p)))))))));

  return res;
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

