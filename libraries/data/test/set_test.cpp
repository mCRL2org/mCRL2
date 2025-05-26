// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file set_test.cpp
/// \brief Basic regression test for set expressions.

#define BOOST_TEST_MODULE set_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/data/parse.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/set.h"


using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::data::sort_set;
using namespace mcrl2::data::sort_fset;

template <typename Predicate>
void test_data_expression(const std::string& s, const variable_vector& v, Predicate p)
{
  std::cerr << "testing data expression " << s << std::endl;
  data_expression e = parse_data_expression(s, v);
  BOOST_CHECK(p(e));
}


void test_expression(const std::string& evaluate, const std::string& expected, data::rewriter r)
{
  const data_expression d1 = parse_data_expression(evaluate);
  const data_expression rd1 =r(d1);
  const data_expression d2 = parse_data_expression(expected);
  const data_expression rd2 =r(d2);

  if (rd1!=rd2)
  {
    std::cerr << "------------------------------------------------------\n";
    std::cerr << "Evaluating: " << d1 << " of sort " << d1.sort() << "\n";
    std::cerr << "Result: " << rd1 << " of sort " << rd1.sort() << "\n";
    std::cerr << "Expected result: " << d2 << " of sort " << d2.sort() << "\n";
    std::cerr << "Evaluated expected result " << rd2 << " of sort " << rd2.sort() <<"\n";
    BOOST_CHECK(rd1 == rd2);
    std::cerr << "------------------------------------------------------\n";
  }
}

void set_expression_test()
{
  data::data_specification specification;

  specification.add_context_sort(sort_pos::pos());
  specification.add_context_sort(sort_fset::fset(sort_pos::pos()));
  specification.add_context_sort(sort_set::set_(sort_pos::pos()));
  specification.add_context_sort(sort_set::set_(sort_bool::bool_()));

  data::rewriter normaliser(specification);

  variable_vector v;
  v.push_back(parse_variable("s:Set(Nat)"));

  test_data_expression("{x : Nat | x < 10}", v, sort_set::is_constructor_application);
  test_data_expression("!s", v, sort_set::is_complement_application);
  test_data_expression("s * {}", v, sort_set::is_intersection_application);
  test_data_expression("s * {1,2,3}", v, sort_set::is_intersection_application);
  test_data_expression("s - {3,1,2}", v, sort_set::is_difference_application);
  test_data_expression("1 in s", v, sort_set::is_in_application);
  test_data_expression("{} + s", v, sort_set::is_union_application);
  test_data_expression("(({} + s) - {20}) * {40}", v, sort_set::is_intersection_application);
  test_data_expression("{10} < s", v, is_less_application<data_expression>);
  test_data_expression("s <= {10}", v, is_less_equal_application<data_expression>);
  test_data_expression("{20} + {30}", v, sort_set::is_union_application);

  test_expression("{1,2}", "{2,1}", normaliser);

  data_expression t1d1a = parse_data_expression("{1,2,3}");
  data_expression t1d2a = parse_data_expression("{2,1}");
  BOOST_CHECK(normaliser(t1d1a) != normaliser(t1d2a));

  test_expression("{1,2} == {1,2}", "true", normaliser);
  test_expression("{1,2,3} == {1,2,1}", "false", normaliser);
  test_expression("({1,2} != {2,3})", "true", normaliser);
  test_expression("(!{1,2}) == {1,2}", "false", normaliser);
  test_expression("(!!{1,2}) == {2,1}", "true", normaliser);

  data_expression e = parse_data_expression("{20}", v);
  BOOST_CHECK(sort_fset::is_cons_application(normaliser(e)));

  e = parse_data_expression("{20, 30, 40}", v);
  BOOST_CHECK(sort_fset::is_cons_application(normaliser(e)));

  test_expression("pick({20})", "20", normaliser);
  test_expression("pick({true})", "true", normaliser);
  test_expression("pick({20}) + 1", "21", normaliser);
  test_expression("pick({20}) == 20", "true", normaliser);
  test_expression("#({20})", "Pos2Nat(1)", normaliser);

  test_expression("{} == { b: Bool | true } - { true, false }","true",normaliser);

  test_expression("{ b: Bool | true } - { true, false } == {}","true", normaliser);

  // Check the operation == on sets.
  test_expression("{} == ({true} - {true})","true",normaliser);  // {true}-{true} is a trick to type {} == {}.
  test_expression("{} == {false}", "false",normaliser);
  test_expression("{} == {true}", "false",normaliser);
  test_expression("{true} == {true}", "true",normaliser);
  test_expression("{false} == {false}", "true",normaliser);
  test_expression("{true} == {false, true}", "false",normaliser);
  test_expression("{false, true} == {false}", "false",normaliser);
  test_expression("{false, true} == {true, false}", "true",normaliser);

  // Check the operation < on sets.
  test_expression("{} < ({true} - {true})","false",normaliser);  // {true}-{true} is a trick to type {} == {}.
  test_expression("{true} < {false}", "false",normaliser);
  test_expression("{false} < {true}", "false",normaliser);
  test_expression("{true} < {true}", "false",normaliser);
  test_expression("{false} < {false}", "false",normaliser);
  test_expression("{true} < {false, true}", "true",normaliser);
  test_expression("{false} < {false, true}", "true",normaliser);
  test_expression("{true} < {true, false}", "true",normaliser);
  test_expression("{false} < {true, false}", "true",normaliser);
  test_expression("{true, false} < {true}", "false",normaliser);
  test_expression("{true, false} < {false}", "false",normaliser);
  test_expression("{false, true} < {true}", "false",normaliser);
  test_expression("{false, true} < {false}", "false",normaliser);
  test_expression("{true, false} < {false, true}", "false",normaliser);
  test_expression("{true, false} < {true, false}", "false",normaliser);
  test_expression("{false, true} < {false, true}", "false",normaliser);
  test_expression("{false, true} < {true, false}", "false",normaliser);
  test_expression("{false, false} < {false}", "false",normaliser);

  // Check the operation <= on sets.
  test_expression("{} <= ({true}-{true})","true",normaliser);  // {true} - {true} is a trick to type {} == {}.
  test_expression("{true} <= {false}", "false",normaliser);
  test_expression("{false} <= {true}", "false",normaliser);
  test_expression("{true} <= {true}", "true",normaliser);
  test_expression("{false} <= {false}", "true",normaliser);
  test_expression("{true} <= {false, true}", "true",normaliser);
  test_expression("{false} <= {false, true}", "true",normaliser);
  test_expression("{true} <= {true, false}", "true",normaliser);
  test_expression("{false} <= {true, false}", "true",normaliser);
  test_expression("{true, false} <= {true}", "false",normaliser);
  test_expression("{true, false} <= {false}", "false",normaliser);
  test_expression("{false, true} <= {true}", "false",normaliser);
  test_expression("{false, true} <= {false}", "false",normaliser);
  test_expression("{true, false} <= {false, true}", "true",normaliser);
  test_expression("{true, false} <= {true, false}", "true",normaliser);
  test_expression("{false, true} <= {false, true}", "true",normaliser);
  test_expression("{false, true} <= {true, false}", "true",normaliser);
  test_expression("{false, false} <= {false}", "true",normaliser);

  // Test finite set and set interaction
  test_expression("{true, false} - { b : Bool | b }","{false}",normaliser);
  test_expression("{true, false} * { b : Bool | b }","{true}",normaliser);
  test_expression("{ b : Bool | b } * {true, false}","{true}",normaliser);
}

BOOST_AUTO_TEST_CASE(test_main)
{
  set_expression_test();
}
