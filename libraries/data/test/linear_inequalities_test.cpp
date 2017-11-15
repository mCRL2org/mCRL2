// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file linear_inequalities_test.cpp
/// \brief Test the linear_inequality functionality. 

#include "mcrl2/data/fourier_motzkin.h"
#include "mcrl2/data/join.h"
#include "mcrl2/data/linear_inequalities.h"
#include "mcrl2/data/parse.h"
#include <boost/test/minimal.hpp>

using namespace mcrl2;
using namespace mcrl2::core;
using namespace mcrl2::data;

void check(bool result, std::string message)
{
  if(!result) {
    std::cout << message << std::endl;
  }
  BOOST_CHECK(result);
}

void test_linear_inequality()
{
  data_specification data_spec;
  data_spec.add_context_sort(sort_real::real_());
  rewriter rewr(data_spec);

  variable vx("x", sort_real::real_());
  variable vy("y", sort_real::real_());
  linear_inequality li;
  data_expression expr;

  expr = less(real_zero(), real_zero());
  li = linear_inequality(expr, rewr);
  check(li.is_false(rewr), "Expected " + pp(expr) + "' to be false");
  check(li.lhs().empty(), "Expected left hand side of '" + pp(expr) + "' to be empty " + pp(li.lhs()));

  expr = less_equal(real_zero(), real_zero());
  li = linear_inequality(expr, rewr);
  check(li.is_true(rewr), "Expected '" + pp(expr) + "' to be true");
  check(li.lhs().empty(), "Expected left hand side of '" + pp(expr) + "' to be empty");

  expr = less_equal(sort_real::minus(vx, vx), real_one());
  li = linear_inequality(expr, rewr);
  check(li.is_true(rewr), "Expected '" + pp(expr) + "' to be true");
  check(li.lhs().empty(), "Expected left hand side of '" + pp(expr) + "' to be empty");

  expr = less_equal(sort_real::minus(vx, vx), real_one());
  li = linear_inequality(expr, rewr);
  check(li.is_true(rewr), "Expected '" + pp(expr) + "' to be true");
  check(li.lhs().empty(), "Expected left hand side of '" + pp(expr) + "' to be empty");

  expr = less_equal(sort_real::plus(vx, vy), vx);
  li = linear_inequality(expr, rewr);
  check(!li.is_true(rewr), "Expected '" + pp(expr) + "' to not be true");
  check(!li.lhs().empty(), "Expected left hand side of '" + pp(expr) + "' not to be empty");
  check(li.transform_to_data_expression() == less_equal(sort_real::times(real_one(), vy), real_zero()), 
    "Expression '" + pp(expr) + "' parsing/output problem " + pp(li.transform_to_data_expression()));

  bool got_exception = false;
  try
  {
    expr = not_equal_to(vx, vy);
    li = linear_inequality(expr, rewr);
  }
  catch(const mcrl2::runtime_error&)
  {
    got_exception = true;
  }
  check(got_exception, "Expected an exception while parsing x != y.");
}

void split_conjunction_of_inequalities_set(const data_expression& e, std::vector < linear_inequality >& v, const rewriter& r)
{
  if (sort_bool::is_and_application(e))
  {
    split_conjunction_of_inequalities_set(application(e)[0],v,r);
    split_conjunction_of_inequalities_set(application(e)[1],v,r);
  }
  else
  {
    v.push_back(linear_inequality(e,r));
  }
}

bool test_consistency_of_inequalities(const std::string& vars,
                                      const std::string& inequalities, 
                                      const bool expect_consistent)
{
  // Take care that reals are part of the data type.
  data_specification data_spec;
  variable_list variables=parse_variables(vars);
  data_spec.add_context_sort(sort_real::real_());
  const data_expression e=parse_data_expression(inequalities,variables,data_spec);

  rewriter r(data_spec);
  std::vector < linear_inequality > v_inequalities;
  split_conjunction_of_inequalities_set(e,v_inequalities,r);

  if (is_inconsistent(v_inequalities,r))
  {
    if (expect_consistent)
    {
      std::cout << "Expected consistent, found inconsistent\n";
      std::cout << variables << ": " << inequalities << "\n";
      std::cout << "Internal inequalities: " << pp_vector(v_inequalities) << "\n";
      return false;
    }
  }
  else
  {
    if (!expect_consistent)
    {
      std::cout << "Expected inconsistent, found consistent\n";
      std::cout << variables << ": " << inequalities << "\n";
      std::cout << "Internal inequalities: " << pp_vector(v_inequalities) << "\n";
      return false;
    }
  }
  return true;
}

bool test_application_of_Fourier_Motzkin(const std::string& vars,
                                         const std::string& variables_to_be_eliminated,
                                         const std::string& inequalities,
                                         const std::string& inconsistent_with)
{
  // Take care that reals are part of the data type.
  data_specification data_spec;
  data_spec.add_context_sort(sort_real::real_());
  const variable_list variables=parse_variables(vars);
  const data_expression e_in=parse_data_expression(inequalities,variables,data_spec);
  const variable_list v_elim=data::detail::parse_variables_new(variables_to_be_eliminated);

  rewriter r(data_spec);
  std::vector < linear_inequality > v_inequalities;
  split_conjunction_of_inequalities_set(e_in,v_inequalities,r);

  std::vector < linear_inequality> resulting_inequalities;
  fourier_motzkin(v_inequalities, v_elim.begin(), v_elim.end(), resulting_inequalities, r);

  std::vector < linear_inequality> inconsistent_inequalities=resulting_inequalities;
  inconsistent_inequalities.push_back(linear_inequality(parse_data_expression(inconsistent_with,variables,data_spec),r));
  if (!is_inconsistent(inconsistent_inequalities,r, false))
  {
    std::cout << "Expected set of inequations to be inconsisten with given inequality after applying Fourier-Motzkin elimination\n";
    std::cout << "Input: " << variables << ": " << inequalities << "\n";
    std::cout << "Parsed input : " << pp_vector(v_inequalities) << "\n";
    std::cout << "Variables to be eliminated: " << v_elim << "\n";
    std::cout << "Input after applying Fourier Motzkin: " << pp_vector(resulting_inequalities) << "\n";
    std::cout << "Should be inconsistent with " << inconsistent_with << "\n";
    std::cout << "Inconsistent inequality after parsing " << pp(linear_inequality(parse_data_expression(inconsistent_with,variables,data_spec),r)) << "\n";
    return false;
  }
  return true;
}

void test_high_level_fourier_motzkin()
{
  data_specification data_spec;
  data_spec.add_context_sort(sort_real::real_());
  rewriter rewr(data_spec);
  variable vx("x", sort_real::real_());
  variable vy("y", sort_real::real_());

  data_expression expr = sort_bool::and_(equal_to(vx, vy), less(vy, sort_real::real_(2)));
  variable_list elim_vars({variable("y", sort_real::real_())});
  data_expression out;
  variable_list vars_out;
  fourier_motzkin(expr, elim_vars, out, vars_out, rewr);
  
  BOOST_CHECK(vars_out.empty());
  BOOST_CHECK(out == less(sort_real::times(real_one(), vx), sort_real::real_(2)));
}

void split_conditions_helper(const std::string& vars,
                             const std::string& expr,
                             std::vector< data_expression_list >& real_conditions,
                             std::vector< data_expression >& non_real_conditions)
{
  data_specification data_spec;
  data_spec.add_context_sort(sort_real::real_());
  const variable_list variables=parse_variables(vars);
  const data_expression e_in=parse_data_expression(expr,variables,data_spec);

  data::detail::split_condition(e_in, real_conditions, non_real_conditions);
}

void test_split_conditions()
{
  std::vector < data_expression_list > real_conditions;
  std::vector < data_expression > non_real_conditions;

  split_conditions_helper("x,y:Real, a,b,c:Bool;", "a && b", real_conditions, non_real_conditions);
  BOOST_CHECK(real_conditions.size() == 1);
  BOOST_CHECK(non_real_conditions.size() == 1);
  BOOST_CHECK(real_conditions[0].size() == 0);
  BOOST_CHECK(sort_bool::is_and_application(non_real_conditions[0]));
  real_conditions.clear(); non_real_conditions.clear();

  split_conditions_helper("x,y:Real, a,b,c:Bool;", "(a || b) && c", real_conditions, non_real_conditions);
  BOOST_CHECK(real_conditions.size() == 1);
  BOOST_CHECK(non_real_conditions.size() == 1);
  BOOST_CHECK(real_conditions[0].size() == 0);
  BOOST_CHECK(sort_bool::is_and_application(non_real_conditions[0]));
  BOOST_CHECK(sort_bool::is_or_application(sort_bool::left(non_real_conditions[0])));
  real_conditions.clear(); non_real_conditions.clear();

  split_conditions_helper("x,y:Real, a,b,c:Bool;", "(a || b) && x < 5", real_conditions, non_real_conditions);
  BOOST_CHECK(real_conditions.size() == 1);
  BOOST_CHECK(non_real_conditions.size() == 1);
  BOOST_CHECK(real_conditions[0].size() == 1);
  BOOST_CHECK(is_less_application(real_conditions[0][0]));
  BOOST_CHECK(sort_bool::is_or_application(non_real_conditions[0]));
  real_conditions.clear(); non_real_conditions.clear();

  split_conditions_helper("x,y:Real, a,b,c:Bool;", "(a || b) && (x == 3 || y > 4)", real_conditions, non_real_conditions);
  BOOST_CHECK(real_conditions.size() == 2);
  BOOST_CHECK(non_real_conditions.size() == 2);
  BOOST_CHECK(real_conditions[0].size() == 1);
  BOOST_CHECK(real_conditions[1].size() == 1);
  BOOST_CHECK((is_equal_to_application(real_conditions[0][0]) && is_greater_application(real_conditions[1][0])) ||
              (is_equal_to_application(real_conditions[1][0]) && is_greater_application(real_conditions[0][0])));
  BOOST_CHECK(non_real_conditions[0] == non_real_conditions[1]);
  real_conditions.clear(); non_real_conditions.clear();

  split_conditions_helper("x,y:Real, a,b,c:Bool;", "(x == y || y < 0) && (x == 3 || y > 4)", real_conditions, non_real_conditions);
  BOOST_CHECK(real_conditions.size() == 4);
  BOOST_CHECK(non_real_conditions.size() == 4);
  for(int i = 0; i < 4; i++)
  {
    BOOST_CHECK(real_conditions[i].size() == 2);
    BOOST_CHECK(non_real_conditions[i] == sort_bool::true_());
  }
  real_conditions.clear(); non_real_conditions.clear();

  split_conditions_helper("x,y:Real, a,b,c:Bool;", "(x == y || a) && (x == 3 || b)", real_conditions, non_real_conditions);
  BOOST_CHECK(real_conditions.size() == 4);
  BOOST_CHECK(non_real_conditions.size() == 4);
  for(int i = 0; i < 4; i++)
  {
    std::set< data_expression > split_without_true = split_and(non_real_conditions[i]);
    split_without_true.erase(sort_bool::true_());
    BOOST_CHECK(real_conditions[i].size() + split_without_true.size() == 2);
  }
  real_conditions.clear(); non_real_conditions.clear();
}

int test_main(int /* argc */, char** /* argv[]*/)
{
  test_linear_inequality();
  test_split_conditions();

  BOOST_CHECK(test_consistency_of_inequalities("x:Real;", "x<3  && x>=4", false));
  BOOST_CHECK(test_consistency_of_inequalities("x:Real;", "x<3  && x>=2", true));
  BOOST_CHECK(test_consistency_of_inequalities("x:Real;", "x<3  && x>=3", false));
  BOOST_CHECK(test_consistency_of_inequalities("x:Real;", "x<=3  && x>=3", true));
  BOOST_CHECK(test_consistency_of_inequalities("u:Real;","0 <= u && -u <= -4 && -u < 0",true));
  BOOST_CHECK(test_consistency_of_inequalities("u,t:Real;","u + -t <= 1 && -u <= -4 && t < u && -u < 0 && -t <= 0 ",true));
  BOOST_CHECK(test_consistency_of_inequalities("u,t,l:Real;","u + -t <= 1 && -u <= -4 && -u + l < 0 && -u < 0 && -t <= 0 && -l + t <= 0",true));

  BOOST_CHECK(test_application_of_Fourier_Motzkin("x,y:Real;", "y:Real;", "-y + x < 0 &&  y < 2", "x>=2"));
  test_high_level_fourier_motzkin();
  return 0;
}

