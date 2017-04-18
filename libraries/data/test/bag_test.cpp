// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https:// svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
// 
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http:// www.boost.org/LICENSE_1_0.txt)
// 
// / \file bag_test.cpp
// / \brief Basic regression test for bag expressions.

#include <boost/test/minimal.hpp>

#include "mcrl2/data/bag.h"
#include "mcrl2/data/fbag.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/rewriter.h"


using namespace mcrl2;
using namespace mcrl2::data;

// test whether parsing s returns an expression matching predicate p.
// Furthermore check whether the expression does not satisfy q.
template <typename Predicate, typename NegativePredicate>
void test_data_expression(const std::string& s, const variable_vector& v, Predicate p, NegativePredicate q)
{
  std::cerr << "testing data expression " << s << std::endl;
  data_expression e = parse_data_expression(s, v);
  std::cerr << "parsed expression " << e << "\n";
  BOOST_CHECK(p(e));
  BOOST_CHECK(!q(e));
}

void test_expression(const std::string& evaluate, const std::string& expected, data::rewriter r)
{
  data_expression d1 = parse_data_expression(evaluate);
  data_expression d2 = parse_data_expression(expected);
  if (r(d1)!=r(d2))
  {
    std::cerr << "Evaluating: " << evaluate << "\n";
    std::cerr << "Result: " << d1 << "\n";
    std::cerr << "Expected result: " << expected << "\n";
    BOOST_CHECK(r(d1) == r(d2));
    std::cerr << "------------------------------------------------------\n";
  }
}


void bag_expression_test()
{
  data::data_specification specification;

  specification.add_context_sort(sort_bag::bag(sort_pos::pos()));
  specification.add_context_sort(sort_bag::bag(sort_bool::bool_()));

  data::rewriter normaliser(specification);

  variable_vector v;
  v.push_back(parse_variable("b:Bag(Nat)"));

  BOOST_CHECK(sort_bag::is_bag(sort_bag::bag(sort_nat::nat())));
  BOOST_CHECK(!sort_bag::is_bag(sort_nat::nat()));

  test_data_expression("{x : Nat | x}", v, sort_bag::is_constructor_application, sort_bag::is_in_application);
  test_data_expression("1 in b", v, sort_bag::is_in_application, sort_bag::is_union_application);
  test_data_expression("{:} + b", v, sort_bag::is_union_application, sort_bag::is_intersection_application);
  test_data_expression("(({:} + b) - {20:1}) * {40:5}", v, sort_bag::is_intersection_application, is_less_application<data_expression>);
  test_data_expression("{10:count(20,b)} < b", v, is_less_application<data_expression>, sort_bag::is_bag_comprehension_application);
  test_data_expression("b <= {20:2}", v, is_less_equal_application<data_expression>, sort_bag::is_set2bag_application);
  test_data_expression("Set2Bag({20,30,40})", v, sort_bag::is_set2bag_application, sort_bag::is_union_application);
  test_data_expression("{20:2} + Set2Bag({20,30,40})", v, sort_bag::is_union_application, is_less_equal_application<data_expression>);
  test_data_expression("b <= Set2Bag({20,30,40})", v, is_less_equal_application<data_expression>, sort_bag::is_constructor_application);

  test_data_expression("b <= {20:2} + Set2Bag({20,30,40})", v, is_less_equal_application<data_expression>, sort_bag::is_constructor_application);

  data_expression e = parse_data_expression("{20:1}", v);
  BOOST_CHECK(sort_fbag::is_cons_application(normaliser(e)));

  e = parse_data_expression("{20:4, 30:3, 40:2}", v);
  BOOST_CHECK(sort_fbag::is_cons_application(normaliser(e)));

  e = parse_data_expression("{10:count(20,b)}", v);
  BOOST_CHECK(sort_fbag::is_cinsert_application(normaliser(e)));

  // Chect the operation == on bags
  test_expression("{:} == ({true:2} - {true:2})","true",normaliser);  // {true}-{true} is a trick to type {:} == {:}. 
  test_expression("{:} == {false:2}", "false",normaliser);
  test_expression("{:} == {true:2}", "false",normaliser);
  test_expression("{true:2} == {true:3}", "false",normaliser);
  test_expression("{false:2} == {false:2}", "true",normaliser);
  test_expression("{true:2} == {false:2, true:2}", "false",normaliser);
  test_expression("{false:2, true:2} == {false:2}", "false",normaliser);
  test_expression("{false:2, true:2} == {true:2, false:2}", "true",normaliser);

  // Check the operation < on bags.
  test_expression("{:} < ({true:2} - {true:2})","false",normaliser);  // {true}-{true} is a trick to type {:} == {:}. 
  test_expression("{true:2} < {false:4}", "false",normaliser);
  test_expression("{true:2} < {false:2}", "false",normaliser);
  test_expression("{false:2} < {true:4}", "false",normaliser);
  test_expression("{true:2} < {true:3}", "true",normaliser);
  test_expression("{false:2} < {false:1}", "false",normaliser);
  test_expression("{true:2} < {false:4, true:2}", "true",normaliser);
  test_expression("{false:2} < {false:1, true:2}", "false",normaliser);
  test_expression("{true:2} < {true:4, false:2}", "true",normaliser);
  test_expression("{false:2} < {true:4, false:2}", "true",normaliser);
  test_expression("{true:2, false:4} < {true:4}", "false",normaliser);
  test_expression("{true:2, false:4} < {false:2}", "false",normaliser);
  test_expression("{false:2, true:4} < {true:2}", "false",normaliser);
  test_expression("{false:2, true:4} < {false:5}", "false",normaliser);
  test_expression("{true:2, false:4} < {false:2, true:2}", "false",normaliser);
  test_expression("{true:2, false:4} < {true:2, false:2}", "false",normaliser);
  test_expression("{false:2, true:1} < {false:2, true:2}", "true",normaliser);
  test_expression("{false:2, true:4} < {true:7, false:2}", "true",normaliser);
  test_expression("{false:1, false:1} < {false:2}", "false",normaliser);
  
  // Check the operation <= on bags.
  test_expression("{:} <= ({true:2}-{true:2})","true",normaliser);  // {true} - {true} is a trick to type {:} == {:}.
  test_expression("{true:2} <= {false:2}", "false",normaliser);
  test_expression("{false:2} <= {true:2}", "false",normaliser);
  test_expression("{true:2} <= {true:2}", "true",normaliser);
  test_expression("{true:3} <= {true:2}", "false",normaliser);
  test_expression("{false:2} <= {false:1}", "false",normaliser);
  test_expression("{false:2} <= {false:7}", "true",normaliser);
  test_expression("{true:2} <= {false:2, true:2}", "true",normaliser);
  test_expression("{false:2} <= {false:4, true:2}", "true",normaliser);
  test_expression("{true:2} <= {true:1, false:2}", "false",normaliser);
  test_expression("{false:2} <= {true:2, false:2}", "true",normaliser);
  test_expression("{true:2, false:2} <= {true:3}", "false",normaliser);
  test_expression("{true:2, false:2} <= {false:1}", "false",normaliser);
  test_expression("{false:2, true:2} <= {true:2}", "false",normaliser);
  test_expression("{false:2, true:2} <= {false:2}", "false",normaliser);
  test_expression("{true:2, false:2} <= {false:2, true:2}", "true",normaliser);
  test_expression("{true:2, false:2} <= {true:2, false:2}", "true",normaliser);
  test_expression("{false:2, true:2} <= {false:2, true:2}", "true",normaliser);
  test_expression("{false:2, true:2} <= {true:2, false:2}", "true",normaliser);
  test_expression("{false:1, false:2} <= {false:2}", "false",normaliser);
  

  
}

int test_main(int argc, char** argv)
{
  bag_expression_test();

  return EXIT_SUCCESS;
}

