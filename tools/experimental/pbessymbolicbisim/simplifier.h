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

#include "mcrl2/data/linear_inequalities.h"
#include "mcrl2/data/merge_data_specifications.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/rewriter.h"

#include "simplifier_mode.h"

namespace mcrl2
{
namespace data
{

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
        "a_:Bool;"
        "r1,r2,r3:Real;"
      "eqn "
        "!a_ || a_ = true;"
        "a_ || !a_ = true;"
        "!a_ && a_ = false;"
        "a_ && !a_ = false;"

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
    variable vn1("n1", sort_nat::nat());
    variable vr1("r1", sort_real::real_());
    variable vr2("r2", sort_real::real_());
    variable vr3("r3", sort_real::real_());

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

    // succ(p) < p = false
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vp1),
      less(sort_pos::succ(vp1), vp1), sort_bool::false_()));
    // p < succ(p) = true
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vp1),
      less(vp1, sort_pos::succ(vp1)), sort_bool::true_()));
    // succ(n) < n = false
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vn1),
      less(sort_nat::cnat(sort_nat::succ(vn1)), vn1), sort_bool::false_()));
    // n < succ(n) = true
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vn1),
      less(vn1, sort_nat::cnat(sort_nat::succ(vn1))), sort_bool::true_()));

    return ad_hoc_data;
  }

protected:
  rewriter rewr;
  rewriter proving_rewr;
  std::map< data_expression, data_expression > cache;

  virtual data_expression simplify_expression(const data_expression& expr) = 0;

  data_expression apply_data_expression(const data_expression& expr)
  {
    if(expr == sort_bool::true_() || expr == sort_bool::false_())
    {
      return expr;
    }
    // Rewrite the expression to some kind of normal form using BDDs
    data_expression rewritten = rewr(proving_rewr(expr));
    // Check the cache
    std::map< data_expression, data_expression >::const_iterator res = cache.find(rewritten);
    if(res != cache.end())
    {
      return res->second;
    }
    // Actually simplify the expression using the implementation in one of the subclasses
    data_expression simpl;
    try
    {
      simpl = simplify_expression(rewritten);
    }
    catch(const mcrl2::runtime_error& e)
    {
      throw mcrl2::runtime_error("An exception occured while simplifying the expression " + pp(expr) + 
        ".\nThe rewritten expression is " + pp(rewritten) + ".\nThe original exception is: " + e.what());
    }

    // Insert the result in the cache, both for the original expression and the expression in normal form
    cache.insert(std::make_pair(rewritten, simpl));

    return simpl;
  }

  lambda apply_lambda(const lambda& expr)
  {
    return lambda(expr.variables(), apply_data_expression(expr.body()));
  }

public:
  simplifier(const rewriter& r, const rewriter& pr)
  : rewr(r)
  , proving_rewr(pr)
  {}

  data_expression apply(const data_expression& expr)
  {
    return is_lambda(expr) ? apply_lambda(expr) : apply_data_expression(expr);
  }
};

simplifier* get_simplifier_instance(const simplifier_mode& mode, const rewriter& rewr, const rewriter& proving_rewr, const variable_list& process_parameters, const data_specification& dataspec,
  const std::map< variable, std::pair<data_expression, data_expression> >& lu_map = std::map< variable, std::pair<data_expression, data_expression> >());

} // namespace mcrl2
} // namespace data

#endif // MCRL2_LPSSYMBOLICBISIM_SIMPLIFIER_H