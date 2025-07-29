// Author(s): Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file simplifier.cpp

#ifdef DBM_PACKAGE_AVAILABLE
  #include "simplifier_dbm.h"
#endif
#include "simplifier_fourier_motzkin.h"
#include "simplifier_identity.h"

namespace mcrl2::data
{

data_specification simplifier::norm_rules_spec()
{
  data_specification ad_hoc_data;

  variable vb1("@b1", sort_bool::bool_());
  variable vb2("@b2", sort_bool::bool_());
  variable vb3("@b3", sort_bool::bool_());
  variable vp1("@p1", sort_pos::pos());
  variable vp2("@p2", sort_pos::pos());
  variable vn1("@n1", sort_nat::nat());
  variable vn2("@n2", sort_nat::nat());
  variable vn3("@n3", sort_nat::nat());
  variable vr1("@r1", sort_real::real_());
  variable vr2("@r2", sort_real::real_());
  variable vr3("@r3", sort_real::real_());

  // !a || a = true;
  ad_hoc_data.add_equation(data_equation(variable_list({vb1}), sort_bool::or_(sort_bool::not_(vb1),vb1), sort_bool::true_()));
  // a || !a = true;
  ad_hoc_data.add_equation(data_equation(variable_list({vb1}), sort_bool::or_(vb1,sort_bool::not_(vb1)), sort_bool::true_()));
  // !a && a = false;
  ad_hoc_data.add_equation(data_equation(variable_list({vb1}), sort_bool::and_(sort_bool::not_(vb1),vb1), sort_bool::false_()));
  // a && !a = false;
  ad_hoc_data.add_equation(data_equation(variable_list({vb1}), sort_bool::and_(vb1,sort_bool::not_(vb1)), sort_bool::false_()));
  // a && a = a;
  ad_hoc_data.add_equation(data_equation(variable_list({vb1}), sort_bool::and_(vb1, vb1), vb1));
  // a && (a && b) = a && b;
  ad_hoc_data.add_equation(data_equation(variable_list({vb1,vb2}), sort_bool::and_(vb1, sort_bool::and_(vb1, vb2)), sort_bool::and_(vb1, vb2)));
  ad_hoc_data.add_equation(data_equation(variable_list({vb1,vb2}), sort_bool::and_(vb1, sort_bool::and_(vb2, vb1)), sort_bool::and_(vb1, vb2)));
  // a || a = a;
  ad_hoc_data.add_equation(data_equation(variable_list({vb1}), sort_bool::or_(vb1, vb1), vb1));
  // a => b = !a || b;
  ad_hoc_data.add_equation(data_equation(variable_list({vb1, vb2}), sort_bool::implies(vb1, vb2), sort_bool::or_(sort_bool::not_(vb1), vb2)));
  // a && (!a || b) = a && b
  ad_hoc_data.add_equation(data_equation(variable_list({vb1,vb2}), sort_bool::and_(vb1, sort_bool::or_(sort_bool::not_(vb1), vb2)), sort_bool::and_(vb1, vb2)));
  // !a && (a || b) == !a && b
  ad_hoc_data.add_equation(data_equation(variable_list({vb1,vb2}), sort_bool::and_(sort_bool::not_(vb1), sort_bool::or_(vb1, vb2)), sort_bool::and_(sort_bool::not_(vb1), vb2)));
  // Pushing not inside
  ad_hoc_data.add_equation(data_equation(variable_list({vb1, vb2}), sort_bool::not_(sort_bool::and_(vb1, vb2)), sort_bool::or_(sort_bool::not_(vb1), sort_bool::not_(vb2))));
  ad_hoc_data.add_equation(data_equation(variable_list({vb1, vb2}), sort_bool::not_(sort_bool::or_(vb1, vb2)), sort_bool::and_(sort_bool::not_(vb1), sort_bool::not_(vb2))));

  // Formulate all linear equalities with positive rhs: -1 * x_P <= -5   !(1 * x_P < 5)
#ifdef MCRL2_ENABLE_MACHINENUMBERS
  ad_hoc_data.add_equation(data_equation(variable_list({vr1, vp1, vp2}),
    less_equal(vr1, sort_real::creal(sort_int::cneg(vp1), vp2)),
    sort_bool::not_(less(sort_real::times(real_minus_one(), vr1), sort_real::creal(sort_int::cint(sort_nat::pos2nat(vp1)), vp2)))));
  ad_hoc_data.add_equation(data_equation(variable_list({vr1, vp1, vp2}),
    less(vr1, sort_real::creal(sort_int::cneg(vp1), vp2)),
    sort_bool::not_(less_equal(sort_real::times(real_minus_one(), vr1), sort_real::creal(sort_int::cint(sort_nat::pos2nat(vp1)), vp2)))));
#else
  ad_hoc_data.add_equation(data_equation(variable_list({vr1, vp1, vp2}),
    less_equal(vr1, sort_real::creal(sort_int::cneg(vp1), vp2)),
    sort_bool::not_(less(sort_real::times(real_minus_one(), vr1), sort_real::creal(sort_int::cint(sort_nat::cnat(vp1)), vp2)))));
  ad_hoc_data.add_equation(data_equation(variable_list({vr1, vp1, vp2}),
    less(vr1, sort_real::creal(sort_int::cneg(vp1), vp2)),
    sort_bool::not_(less_equal(sort_real::times(real_minus_one(), vr1), sort_real::creal(sort_int::cint(sort_nat::cnat(vp1)), vp2)))));
#endif
  // -1 * (-1 * r) = r
  ad_hoc_data.add_equation(data_equation(variable_list({vr1}), sort_real::times(real_minus_one(), sort_real::times(real_minus_one(), vr1)), vr1));
  // -1 * -r = r
  ad_hoc_data.add_equation(data_equation(variable_list({vr1}), sort_real::times(real_minus_one(), sort_real::negate(vr1)), vr1));
  // r1 * (r2 + r3) = r1 * r2 + r1 * r3
  ad_hoc_data.add_equation(data_equation(variable_list({vr1,vr2,vr3}),
    sort_real::times(vr1, sort_real::plus(vr2,vr3)), sort_real::plus(sort_real::times(vr1,vr2), sort_real::times(vr1,vr3))));
  // 1 * r = r
  ad_hoc_data.add_equation(data_equation(variable_list({vr1}), sort_real::times(real_one(), vr1), vr1));
  // -1 * (r1 - r2) = r2 - r1
  ad_hoc_data.add_equation(data_equation(variable_list({vr1,vr2}), sort_real::times(real_minus_one(), sort_real::minus(vr1,vr2)), sort_real::minus(vr2,vr1)));
  // Since there are some problems with !(0 == x1) when feeding it to fourier motzkin, add the following rule
  // !(r2 == r1) = r1 > r2 || r1 < r2
  ad_hoc_data.add_equation(data_equation(variable_list({vr1,vr2}), sort_bool::not_(equal_to(vr2,vr1)), sort_bool::or_(greater(vr1, vr2), less(vr1, vr2))));

  // r2 > r3 -> !(r1 < r2) && r1 < r3 = false
  ad_hoc_data.add_equation(data_equation(variable_list({vr1,vr2,vr3}), greater(vr2, vr3),
      sort_bool::and_(sort_bool::not_(less(vr1, vr2)), less(vr1, vr3)), sort_bool::false_()));
  // r2 > r3 -> r1 < r3 && !(r1 < r2) = false
  ad_hoc_data.add_equation(data_equation(variable_list({vr1,vr2,vr3}), greater(vr2, vr3),
      sort_bool::and_(less(vr1, vr3), sort_bool::not_(less(vr1, vr2))), sort_bool::false_()));
  // r2 < r3 -> !(r1 < r2) || r1 < r3 = true
  ad_hoc_data.add_equation(data_equation(variable_list({vr1,vr2,vr3}), less(vr2, vr3),
      sort_bool::or_(sort_bool::not_(less(vr1, vr2)), less(vr1, vr3)), sort_bool::true_()));
  // r2 < r3 -> r1 < r3 || !(r1 < r2) = true
  ad_hoc_data.add_equation(data_equation(variable_list({vr1,vr2,vr3}), less(vr2, vr3),
      sort_bool::or_(less(vr1, vr3), sort_bool::not_(less(vr1, vr2))), sort_bool::true_()));

  // if(a,b,c) = (a && b) || (!a && c);
  ad_hoc_data.add_equation(data_equation(variable_list({vb1,vb2,vb3}), if_(vb1, vb2, vb3),
    sort_bool::or_(sort_bool::and_(vb1, vb2), sort_bool::and_(sort_bool::not_(vb1), vb3))));
  // (a && b) || !a = b || !a
  ad_hoc_data.add_equation(data_equation(variable_list({vb1,vb2}),
    sort_bool::or_(sort_bool::and_(vb1,vb2), sort_bool::not_(vb1)), sort_bool::or_(vb2,sort_bool::not_(vb1))));
  // !a || (a && b) = !a || b
  ad_hoc_data.add_equation(data_equation(variable_list({vb1,vb2}),
    sort_bool::or_(sort_bool::not_(vb1), sort_bool::and_(vb1,vb2)), sort_bool::or_(sort_bool::not_(vb1), vb2)));
  // a || (!a && b) = a || b
  ad_hoc_data.add_equation(data_equation(variable_list({vb1,vb2}),
    sort_bool::or_(vb1, sort_bool::and_(sort_bool::not_(vb1), vb2)), sort_bool::or_(vb1,vb2)));

  // (n1 != n2) -> n1 == n3 && n2 == n3 = false;
  ad_hoc_data.add_equation(data_equation(variable_list({vn1,vn2,vn3}), not_equal_to(vn1,vn2),
    sort_bool::and_(equal_to(vn1,vn3), equal_to(vn2,vn3)), sort_bool::false_()));
  // (n1 != n2) -> n3 == n1 && n3 == n2 = false;
  ad_hoc_data.add_equation(data_equation(variable_list({vn1,vn2,vn3}), not_equal_to(vn1,vn2),
    sort_bool::and_(equal_to(vn3,vn1), equal_to(vn3,vn2)), sort_bool::false_()));

  // pred(succ(n)) = n
  ad_hoc_data.add_equation(data_equation(variable_list({vn1}),
    sort_nat::pred(sort_nat::succ(vn1)), vn1));
  // pred(succ(p)) = p
  ad_hoc_data.add_equation(data_equation(variable_list({vp1}),
    sort_nat::pred(sort_nat::succ(vp1)), vp1));
  // succ(pred(p)) = p
  ad_hoc_data.add_equation(data_equation(variable_list({vp1}),
    sort_nat::succ(sort_nat::succ(vp1)), vp1));
  // succ(p) < p = false
  ad_hoc_data.add_equation(data_equation(variable_list({vp1}),
    less(sort_pos::succ(vp1), vp1), sort_bool::false_()));
  // p < succ(p) = true
  ad_hoc_data.add_equation(data_equation(variable_list({vp1}),
    less(vp1, sort_pos::succ(vp1)), sort_bool::true_()));
  // succ(n) < n = false
#ifdef MCRL2_ENABLE_MACHINENUMBERS
  ad_hoc_data.add_equation(data_equation(variable_list({vn1}),
    less(sort_nat::pos2nat(sort_nat::succ(vn1)), vn1), sort_bool::false_()));
  // n < succ(n) = true
  ad_hoc_data.add_equation(data_equation(variable_list({vn1}),
    less(vn1, sort_nat::pos2nat(sort_nat::succ(vn1))), sort_bool::true_()));
#else
  ad_hoc_data.add_equation(data_equation(variable_list({vn1}),
    less(sort_nat::cnat(sort_nat::succ(vn1)), vn1), sort_bool::false_()));
  // n < succ(n) = true
  ad_hoc_data.add_equation(data_equation(variable_list({vn1}),
    less(vn1, sort_nat::cnat(sort_nat::succ(vn1))), sort_bool::true_()));
#endif
  return ad_hoc_data;
}

simplifier* get_simplifier_instance(const simplifier_mode& mode, const rewriter& rewr, const rewriter& proving_rewr, const variable_list& process_parameters, const data_specification& dataspec,
  const std::map< variable, std::pair<data_expression, data_expression> >& lu_map)
{
  switch(mode)
  {
    case simplify_fm:
      return new simplifier_fourier_motzkin(rewr, proving_rewr, dataspec);
#ifdef DBM_PACKAGE_AVAILABLE
    case simplify_dbm:
      return new simplifier_dbm(rewr, proving_rewr, process_parameters, dataspec, lu_map);
#endif
    case simplify_finite_domain:
      return new simplifier_mdd(rewr, proving_rewr, dataspec);
    case simplify_identity:
      return new simplifier_identity(rewr, proving_rewr);
    case simplify_auto:
    {
      if(std::find_if(process_parameters.begin(), process_parameters.end(), [&](const variable& v){ return v.sort() == sort_real::real_();}) != process_parameters.end())
      {
#ifdef DBM_PACKAGE_AVAILABLE
        return new simplifier_dbm(rewr, proving_rewr, process_parameters, dataspec, lu_map);
#else
        return new simplifier_fourier_motzkin(rewr, proving_rewr, dataspec);
        (void) lu_map;
#endif
      }
      else
      {
        return new simplifier_mdd(rewr, proving_rewr, dataspec);
      }
    }
    default:
      throw mcrl2::runtime_error("Unknown simplifier_mode.");
  }
}

} // namespace mcrl2::data
