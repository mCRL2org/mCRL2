// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file bag_test.cpp
/// \brief Basic regression test for bag expressions.

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
void test_data_expression(const std::string& s, variable_vector v, Predicate p, NegativePredicate q)
{
  std::cerr << "testing data expression " << s << std::endl;
  data_expression e = parse_data_expression(s, v);
  std::cerr << "parsed expression " << e << "\n";
  BOOST_CHECK(p(e));
  BOOST_CHECK(!q(e));
}

/* Test case for various bag sort expressions, based
   on the following specification:

proc P(b: Bag(Nat)) = (1 in b) -> tau . P(({:} + b) - {20:1} * {40:5})
                    + ({10:count(20, b)} < b) -> tau . P(b)
                    + (b <= {20:2} + Set2Bag({20,30,40})) -> tau . P(b);

init P({20:4, 30:3, 40:2});

*/
void bag_expression_test()
{
  data::data_specification specification;

  specification.add_context_sort(sort_bag::bag(sort_pos::pos()));

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
}

int test_main(int argc, char** argv)
{
  bag_expression_test();

  return EXIT_SUCCESS;
}

