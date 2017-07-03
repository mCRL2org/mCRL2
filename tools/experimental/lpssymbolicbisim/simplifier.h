// Author(s): Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file simplifier.h


#ifndef MCRL2_LPSSYMBOLICBISIM_SIMPLIFIER_H
#define MCRL2_LPSSYMBOLICBISIM_SIMPLIFIER_H

// #define DBM_PACKAGE_AVAILABLE 1

#include "mcrl2/data/linear_inequalities.h"
#include "mcrl2/data/merge_data_specifications.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/rewriter.h"

namespace mcrl2
{
namespace data
{

core::identifier_string iff_name()
{
  static core::identifier_string iff_name = core::identifier_string("<=>");
  return iff_name;
}

function_symbol iff()
{
  static function_symbol iff(iff_name(), make_function_sort(sort_bool::bool_(), sort_bool::bool_(), sort_bool::bool_()));
  return iff;
}

inline application iff(const data_expression& d1, const data_expression& d2)
{
  return iff()(d1, d2);
}

class simplifier
{

public:
  /**
   * \brief Creates a data_specification with rewrite rules that improve 'cannonicalness' of
   * expressions.
   * \detail A default data specification is expanded with a number of rewrite rules
   * for expressions over booleans and reals. Some rewrite rules help to simplify expressions.
   * Others try to rewrite linear inequalities to a normal form. There are also rewrite rules
   * to eliminate the [if] function symbol.
   */
  static data_specification norm_rules_spec()
  {
    data_specification ad_hoc_data = parse_data_specification(
      "var "
        "a,b,c:Bool;"
        "r1,r2,r3:Real;"
      "eqn "
        "!a || a = true;"
        "a || !a = true;"
        "!a && a = false;"
        "a && !a = false;"

        "r2 > r3 -> !(r1 < r2) && r1 < r3 = false;"
        "r2 > r3 -> r1 < r3 && !(r1 < r2) = false;"
        "r2 < r3 -> !(r1 < r2) || r1 < r3 = true;"
        "r2 < r3 -> r1 < r3 || !(r1 < r2) = true;"
      );

    variable vb1("b1", sort_bool::bool_());
    variable vb2("b2", sort_bool::bool_());
    variable vb3("b3", sort_bool::bool_());
    variable vp1("p1", sort_pos::pos());
    variable vp2("p2", sort_pos::pos());
    variable vr1("r1", sort_real::real_());
    variable vr2("r2", sort_real::real_());
    variable vr3("r3", sort_real::real_());

    ad_hoc_data.add_mapping(iff());

    //  a && a = a;
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vb1), sort_bool::and_(vb1, vb1), vb1));
    //  a && (a && b) = a && b;
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vb1,vb2), sort_bool::and_(vb1, sort_bool::and_(vb1, vb2)), sort_bool::and_(vb1, vb2)));
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vb1,vb2), sort_bool::and_(vb1, sort_bool::and_(vb2, vb1)), sort_bool::and_(vb1, vb2)));
    //  a || a = a;
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vb1), sort_bool::or_(vb1, vb1), vb1));
    //  a => b = !a || b;
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vb1, vb2), sort_bool::implies(vb1, vb2), sort_bool::or_(sort_bool::not_(vb1), vb2)));
    // a && (!a || b) = a && b
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vb1,vb2), sort_bool::and_(vb1, sort_bool::or_(sort_bool::not_(vb1), vb2)), sort_bool::and_(vb1, vb2)));
    // !a && (a || b) == !a && b
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vb1,vb2), sort_bool::and_(sort_bool::not_(vb1), sort_bool::or_(vb1, vb2)), sort_bool::and_(sort_bool::not_(vb1), vb2)));
    // Pushing not inside
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vb1, vb2), sort_bool::not_(sort_bool::and_(vb1, vb2)), sort_bool::or_(sort_bool::not_(vb1), sort_bool::not_(vb2))));
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vb1, vb2), sort_bool::not_(sort_bool::or_(vb1, vb2)), sort_bool::and_(sort_bool::not_(vb1), sort_bool::not_(vb2))));
    // Formulate all linear equalities with positive rhs: -1 * x_P <= -5   !(1 * x_P < 5)
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vr1, vp1, vp2),
      less_equal(vr1, sort_real::creal(sort_int::cneg(vp1), vp2)),
      sort_bool::not_(less(sort_real::times(real_minus_one(), vr1), sort_real::creal(sort_int::cint(sort_nat::cnat(vp1)), vp2)))));
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vr1, vp1, vp2),
      less(vr1, sort_real::creal(sort_int::cneg(vp1), vp2)),
      sort_bool::not_(less_equal(sort_real::times(real_minus_one(), vr1), sort_real::creal(sort_int::cint(sort_nat::cnat(vp1)), vp2)))));
    // -1 * (-1 * r) = r
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vr1), sort_real::times(real_minus_one(), sort_real::times(real_minus_one(), vr1)), vr1));
    // -1 * -r = r
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vr1), sort_real::times(real_minus_one(), sort_real::negate(vr1)), vr1));
    // r1 * (r2 + r3) = r1 * r2 + r1 * r3
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vr1,vr2,vr3),
      sort_real::times(vr1, sort_real::plus(vr2,vr3)), sort_real::plus(sort_real::times(vr1,vr2), sort_real::times(vr1,vr3))));
    // 1 * r = r
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vr1), sort_real::times(real_one(), vr1), vr1));
    // -1 * (r1 - r2) = r2 - r1
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vr1,vr2), sort_real::times(real_minus_one(), sort_real::minus(vr1,vr2)), sort_real::minus(vr2,vr1)));
    // Since there are some problems with !(0 == x1) when feeding it to fourier motzkin, add the following rule
    // !(r2 == r1) = r1 > r2 || r1 < r2
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vr1,vr2), sort_bool::not_(equal_to(vr2,vr1)), sort_bool::or_(greater(vr1, vr2), less(vr1, vr2))));
    // r1 < 0 = false
    // Breaks everything when expressions such as -x < 0 are encountered
    // ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vr1), less(vr1,real_zero()), sort_bool::false_()));

    // Rules for bidirectional implication (iff)
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vb1), iff(sort_bool::true_(), vb1), vb1));
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vb1), iff(vb1, sort_bool::true_()), vb1));
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vb1), iff(sort_bool::false_(), vb1), sort_bool::not_(vb1)));
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vb1), iff(vb1, sort_bool::false_()), sort_bool::not_(vb1)));

    // if(a,b,c) = (a && b) || (!a && c);
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vb1,vb2,vb3), if_(vb1, vb2, vb3),
      sort_bool::or_(sort_bool::and_(vb1, vb2), sort_bool::and_(sort_bool::not_(vb1), vb3))));
    // (a && b) || !a = b || !a
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vb1,vb2),
      sort_bool::or_(sort_bool::and_(vb1,vb2), sort_bool::not_(vb1)), sort_bool::or_(vb2,sort_bool::not_(vb1))));
    // !a || (a && b) = !a || b
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vb1,vb2),
      sort_bool::or_(sort_bool::not_(vb1), sort_bool::and_(vb1,vb2)), sort_bool::or_(sort_bool::not_(vb1), vb2)));
    // a || (!a && b) = a || b
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vb1,vb2),
      sort_bool::or_(vb1, sort_bool::and_(sort_bool::not_(vb1), vb2)), sort_bool::or_(vb1,vb2)));

    return ad_hoc_data;
  }

protected:
  rewriter rewr;
  rewriter proving_rewr;
  std::map< data_expression, data_expression > cache;

  virtual data_expression simplify_expression(const data_expression& expr) = 0;

  data_expression apply_data_expression(const data_expression& expr, const mutable_indexed_substitution<> sigma)
  {
    // Rewrite the expression to some kind of normal form using BDDs
    data_expression rewritten = rewr(proving_rewr(expr,sigma));
    // Check the cache
    std::map< data_expression, data_expression >::const_iterator res = cache.find(rewritten);
    if(res != cache.end())
    {
      return res->second;
    }
    // Actually simplify the expression using the implementation in one of the subclasses
    data_expression simpl(simplify_expression(rewritten));

    // Insert the result in the cache, both for the original expression and the expression in normal form
    cache.insert(std::make_pair(rewritten, simpl));

    return simpl;
  }

  lambda apply_lambda(const lambda& expr, const mutable_indexed_substitution<> sigma)
  {
    return lambda(expr.variables(), apply_data_expression(expr.body(), sigma));
  }

public:
  simplifier(rewriter r, rewriter pr)
  : rewr(r)
  , proving_rewr(pr)
  {}

  data_expression apply(const data_expression& expr)
  {
    const mutable_indexed_substitution<> sigma;
    return apply(expr, sigma);
  }

  data_expression apply(const data_expression& expr, const mutable_indexed_substitution<> sigma)
  {
    return is_lambda(expr) ? apply_lambda(expr, sigma) : apply_data_expression(expr,sigma);
  }
};


} // namespace mcrl2
} // namespace data

#ifdef DBM_PACKAGE_AVAILABLE
  #include "simplifier_dbm.h"
#else
  #include "simplifier_fourier_motzkin.h"
#endif

namespace mcrl2 {
namespace data{

simplifier* get_simplifier_instance(const rewriter& rewr, const rewriter& proving_rewr, const variable_list& process_parameters, const data_specification& dataspec,
  const std::map< variable, std::pair<data_expression, data_expression> >& lu_map = std::map< variable, std::pair<data_expression, data_expression> >())
{
#ifdef DBM_PACKAGE_AVAILABLE
  return new simplifier_dbm(rewr, proving_rewr, process_parameters, dataspec, lu_map);
#else
  return new simplifier_fourier_motzkin(rewr, proving_rewr);
  (void) process_parameters;
  (void) dataspec;
  (void) lu_map;
#endif
}

} // namespace mcrl2
} // namespace data

#endif // MCRL2_LPSSYMBOLICBISIM_SIMPLIFIER_H