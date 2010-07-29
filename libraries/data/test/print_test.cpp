// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <sstream>

#include <boost/test/included/unit_test_framework.hpp>

#include "mcrl2/core/garbage_collection.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/data/bool.h"
#include "mcrl2/data/nat.h"
#include "mcrl2/data/real.h"
#include "mcrl2/data/set.h"
#include "mcrl2/data/list.h"
#include "mcrl2/data/bag.h"
#include "mcrl2/data/lambda.h"
#include "mcrl2/data/exists.h"
#include "mcrl2/data/forall.h"
#include "mcrl2/data/function_update.h"
#include "mcrl2/data/print.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/detail/sort_traverser.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace mcrl2::data;
using namespace mcrl2::data::sort_bool;
using namespace mcrl2::data::sort_nat;

// Garbage collect after each case.
struct collect_after_test_case {
  ~collect_after_test_case()
  {
    mcrl2::core::garbage_collect();
  }
};

BOOST_GLOBAL_FIXTURE(collect_after_test_case)

bool print_check(data_expression const& left, std::string const& right) {
  if (pp(left) != right)
  {
    std::clog << "pp(" << pp(left) << ") != " << right << std::endl;
    std::clog << left << " != " << right << std::endl;

    return false;
  }

  return true;
}

template <typename Container>
bool print_container_check(Container const& c) {
  std::string r = pp(c);
  if(r == "")
  {
    std::clog << "error printing container" << std::endl;
    return false;
  }

  return true;
}

#define PRINT_CHECK(x,y) BOOST_CHECK(print_check(x,y))

BOOST_AUTO_TEST_CASE(test_function_symbol_print) {
  function_symbol f("f", sort_bool::bool_());

  PRINT_CHECK(f, "f");
}

BOOST_AUTO_TEST_CASE(test_application_print) {
  function_symbol f("f", make_function_sort(bool_(), bool_()));
  function_symbol g("g", make_function_sort(bool_(), nat(), bool_()));

  PRINT_CHECK(f(true_()), "f(true)");
  PRINT_CHECK(g(false_(), sort_nat::nat(10)), "g(false, 10)");
  PRINT_CHECK(g(f(true_()), sort_nat::nat(10)), "g(f(true), 10)");
}

BOOST_AUTO_TEST_CASE(test_abstraction_print) {
  using namespace sort_pos;
  using namespace sort_nat;

  variable_vector x(atermpp::make_vector(variable("x", sort_nat::nat())));
  variable_vector y(atermpp::make_vector(variable("y", sort_pos::pos())));
  variable_vector xy(atermpp::make_vector(x[0], y[0]));

  PRINT_CHECK(lambda(x, equal_to(x[0], nat(10))), "lambda x: Nat. x == 10");
  PRINT_CHECK(lambda(xy, equal_to(xy[0], pos2nat(xy[1]))), "lambda x: Nat, y: Pos. x == y");
  PRINT_CHECK(exists(x, equal_to(x[0], nat(10))), "exists x: Nat. x == 10");
  PRINT_CHECK(exists(xy, equal_to(xy[0], pos2nat(xy[1]))), "exists x: Nat, y: Pos. x == y");
  PRINT_CHECK(forall(x, equal_to(x[0], nat(10))), "forall x: Nat. x == 10");
  PRINT_CHECK(forall(xy, equal_to(xy[0], pos2nat(xy[1]))), "forall x: Nat, y: Pos. x == y");
  PRINT_CHECK(forall(x, exists(y, not_equal_to(xy[0], pos2nat(xy[1])))), "forall x: Nat. exists y: Pos. x != y");
}

BOOST_AUTO_TEST_CASE(test_list_print) {
  using namespace sort_bool;
  using namespace sort_list;

  data_expression empty(nil(bool_()));

  // Using all operations
  BOOST_CHECK(print_check(nil(bool_()), "[]"));
  BOOST_CHECK(print_check(cons_(bool_(), true_(), empty), "[true]"));
  BOOST_CHECK(print_check(cons_(bool_(), false_(), cons_(bool_(), true_(), empty)), "[false, true]"));
  //BOOST_CHECK(print_check(snoc(bool_(), cons_(bool_(), true_(), empty), false_()), "[true, false]"));
  BOOST_CHECK(print_check(snoc(bool_(), cons_(bool_(), true_(), empty), false_()), "[true] <| false"));
  BOOST_CHECK(print_check(snoc(bool_(), snoc(bool_(), empty, true_()), false_()), "[true, false]"));
  BOOST_CHECK(print_check(cons_(bool_(), in(bool_(), false_(), cons_(bool_(), true_(), empty)), empty), "[false in [true]]"));
  //BOOST_CHECK(print_check(snoc(bool_(), cons_(bool_(), false_(), snoc(bool_(), empty, true_())), true_()), "[false, true, true]"));
  BOOST_CHECK(print_check(snoc(bool_(), cons_(bool_(), false_(), snoc(bool_(), empty, true_())), true_()), "(false |> [true]) <| true"));
  //BOOST_CHECK(print_check(in(bool_(), true_(), cons_(bool_(), false_(), snoc(bool_(), empty, true_()))), "true in [false, true]"));
  BOOST_CHECK(print_check(in(bool_(), true_(), cons_(bool_(), false_(), snoc(bool_(), empty, true_()))), "true in false |> [true]"));
  //BOOST_CHECK(print_check(count(bool_(), cons_(bool_(), false_(), snoc(bool_(), empty, true_()))), "#[false, true]"));
  BOOST_CHECK(print_check(count(bool_(), cons_(bool_(), false_(), snoc(bool_(), empty, true_()))), "#(false |> [true])"));
  //BOOST_CHECK(print_check(concat(bool_(), cons_(bool_(), true_(), empty), cons_(bool_(), false_(), snoc(bool_(), empty, true_()))), "[true] ++ [false, true]"));
  BOOST_CHECK(print_check(concat(bool_(), cons_(bool_(), true_(), empty), cons_(bool_(), false_(), snoc(bool_(), empty, true_()))), "[true] ++ (false |> [true])"));
  //BOOST_CHECK(print_check(element_at(bool_(), cons_(bool_(), false_(), snoc(bool_(), empty, true_())), sort_nat::nat(1)), "[false, true].1"));
  BOOST_CHECK(print_check(element_at(bool_(), cons_(bool_(), false_(), snoc(bool_(), empty, true_())), sort_nat::nat(1)), "(false |> [true]) . 1"));
  BOOST_CHECK(print_check(head(bool_(), cons_(bool_(), false_(), cons_(bool_(), true_(), empty))), "head([false, true])"));
  BOOST_CHECK(print_check(tail(bool_(), cons_(bool_(), false_(), cons_(bool_(), true_(), empty))), "tail([false, true])"));
  BOOST_CHECK(print_check(rhead(bool_(), cons_(bool_(), false_(), cons_(bool_(), true_(), empty))), "rhead([false, true])"));
  BOOST_CHECK(print_check(rtail(bool_(), cons_(bool_(), false_(), cons_(bool_(), true_(), empty))), "rtail([false, true])"));
  BOOST_CHECK(print_check(cons_(bool_(), true_(), concat(bool_(), cons_(bool_(), true_(), empty), cons_(bool_(), false_(), empty))), "true |> [true] ++ [false]"));
  BOOST_CHECK(print_check(snoc(bool_(), concat(bool_(), cons_(bool_(), true_(), empty), cons_(bool_(), false_(), empty)), true_()), "[true] ++ [false] <| true"));
  BOOST_CHECK(print_check(cons_(bool_(), false_(), snoc(bool_(), concat(bool_(), cons_(bool_(), true_(), empty), cons_(bool_(), false_(), empty)), true_())), "false |> [true] ++ [false] <| true"));

  // lists of lists
  data_expression list_empty = nil(list(bool_()));
  data_expression list_true = cons_(bool_(), true_(), empty);
  data_expression list_false_true = cons_(bool_(), false_(), cons_(bool_(), true_(), empty));

  BOOST_CHECK(print_check(nil(list(bool_())), "[]"));
  BOOST_CHECK(print_check(list_true, "[true]"));
  BOOST_CHECK(print_check(cons_(list(bool_()), list_true, list_empty), "[[true]]"));
  BOOST_CHECK(print_check(cons_(list(bool_()), list_true, cons_(list(bool_()), list_false_true, list_empty)), "[[true], [false, true]]"));
  BOOST_CHECK(print_check(snoc(list(bool_()), cons_(list(bool_()), list_false_true, list_empty), list_true), "[[false, true]] <| [true]"));
  BOOST_CHECK(print_check(in(list(bool_()), list_true, cons_(list(bool_()), list_true, list_empty)), "[true] in [[true]]"));
  BOOST_CHECK(print_check(in(list(bool_()), list_true, cons_(list(bool_()), list_true, list_empty)), "[true] in [[true]]"));

  // List enumeration
  data_expression_vector v;
  v.push_back(sort_bool::true_());
  v.push_back(sort_bool::false_());
  v.push_back(sort_bool::true_());
  v.push_back(sort_bool::true_());
  data_expression l1(list_enumeration(list(sort_bool::bool_()), v));
  BOOST_CHECK(print_check(l1, "[true, false, true, true]"));

  data_expression l2(sort_list::list(sort_bool::bool_(), v));
  BOOST_CHECK(print_check(l2, "[true, false, true, true]"));

  
}

BOOST_AUTO_TEST_CASE(test_set_print) {
  using namespace sort_bool;
  using namespace sort_set;
  using namespace sort_fset;

  data_expression set_empty = emptyset(bool_());
  data_expression set_false = setfset(bool_(), fset_cons(bool_(), false_(), fset_empty(bool_())));
  data_expression set_true = setfset(bool_(), fset_cons(bool_(), true_(), fset_empty(bool_())));

  // Using all operations
  BOOST_CHECK(print_check(emptyset(bool_()), "{}"));
  BOOST_CHECK(print_check(setfset(bool_(), fset_empty(bool_())), "{}"));
  BOOST_CHECK(print_check(setconstructor(bool_(), false_function(bool_()), fset_empty(bool_())), "{}"));
  BOOST_CHECK(print_check(setin(bool_(), false_(), set_empty), "false in {}"));
  BOOST_CHECK(print_check(setunion_(bool_(), set_false, set_true), "{false} + {true}"));
  BOOST_CHECK(print_check(setintersection(bool_(), set_false, set_true), "{false} * {true}"));
  BOOST_CHECK(print_check(setdifference(bool_(), set_false, set_true), "{false} - {true}"));
  BOOST_CHECK(print_check(setcomplement(bool_(), set_false), "!{false}"));

  // Some parsed expressions
  BOOST_CHECK(print_check(parse_data_expression("{true}"), "{true}"));
  BOOST_CHECK(print_check(parse_data_expression("{true, false}"), "{true, false}"));
  BOOST_CHECK(print_check(parse_data_expression("{ b: Bool | b }"), "{ b: Bool | b }"));
  BOOST_CHECK(print_check(parse_data_expression("{ x: Nat | x == 0 }"), "{ x: Nat | x == 0 }"));
  BOOST_CHECK(print_check(parse_data_expression("{ x: Nat | (lambda y: Nat. y == 0)(x) }"), "{ x: Nat | (lambda y: Nat. y == 0)(x) }"));
}

BOOST_AUTO_TEST_CASE(test_bag_print) {
  using namespace sort_bool;
  using namespace sort_bag;
  using namespace sort_fbag;
  using namespace sort_nat;

  data_expression bag_empty(emptybag(bool_()));
  data_expression fbag_empty_(fbag_empty(bool_()));
  data_expression bag_false = bagfbag(bool_(), fbag_cons(bool_(), false_(), number(sort_pos::pos(), "1"), fbag_empty_));
  data_expression bag_true = bagfbag(bool_(), fbag_cons(bool_(), true_(), number(sort_pos::pos(), "1"), fbag_empty_));

  // Using all operations
  BOOST_CHECK(print_check(bag_empty, "{}"));
  BOOST_CHECK(print_check(bagfbag(bool_(), fbag_empty(bool_())), "{}"));
  BOOST_CHECK(print_check(bagconstructor(bool_(), zero_function(bool_()), fbag_empty_), "{}"));
  BOOST_CHECK(print_check(bagin(bool_(), false_(), bag_empty), "false in {}"));
  BOOST_CHECK(print_check(bagin(bool_(), false_(), bag_false), "false in {false: 1}"));
  BOOST_CHECK(print_check(bagcount(bool_(), false_(), bag_true), "count(false, {true: 1})"));
  BOOST_CHECK(print_check(bagjoin(bool_(), bag_false, bag_true), "{false: 1} + {true: 1}"));
  BOOST_CHECK(print_check(bagintersect(bool_(), bag_false, bag_true), "{false: 1} * {true: 1}"));
  BOOST_CHECK(print_check(bagdifference(bool_(), bag_false, bag_true), "{false: 1} - {true: 1}"));

  // Some parsed expressions
  BOOST_CHECK(print_check(parse_data_expression("{true: 2}"), "{true: 2}"));
  BOOST_CHECK(print_check(parse_data_expression("{false: 3, true: 1}"), "{false: 3, true: 1}"));
  BOOST_CHECK(print_check(parse_data_expression("{ b: Bool | if(b, Pos2Nat(2), Pos2Nat(3)) }"), "{ b: Bool | if(b, 2, 3) }"));
  BOOST_CHECK(print_check(parse_data_expression("{ x: Nat | x * x }"), "{ x: Nat | x * x }"));
  BOOST_CHECK(print_check(parse_data_expression("{ x: Nat | (lambda y: Nat. y * y)(x) }"), "{ x: Nat | (lambda y: Nat. y * y)(x) }"));
}

BOOST_AUTO_TEST_CASE(test_function_update_print)
{
  PRINT_CHECK(function_update(sort_nat::nat(), sort_bool::bool_()), "@func_update");
  PRINT_CHECK(parse_data_expression("(lambda x: Bool. x)[true -> false]"), "(lambda x: Bool. x)[true -> false]");
  PRINT_CHECK(parse_data_expression("(lambda x: Bool. x)[true -> false][false -> true]"), "(lambda x: Bool. x)[true -> false][false -> true]");
  PRINT_CHECK(parse_data_expression("(lambda n: Nat. n mod 2 == 0)[0 -> false]"), "(lambda n: Nat. n mod 2 == 0)[0 -> false]");
}

BOOST_AUTO_TEST_CASE(test_rewrite_rule_bool_print)
{
  BOOST_CHECK(print_container_check(sort_bool::bool_generate_equations_code()));
}

BOOST_AUTO_TEST_CASE(test_rewrite_rule_pos_print)
{
  BOOST_CHECK(print_container_check(sort_pos::pos_generate_equations_code()));
}

BOOST_AUTO_TEST_CASE(test_rewrite_rule_nat_print)
{
  BOOST_CHECK(print_container_check(sort_nat::nat_generate_equations_code()));
}

BOOST_AUTO_TEST_CASE(test_rewrite_rule_int_print)
{
  BOOST_CHECK(print_container_check(sort_int::int_generate_equations_code()));
}

BOOST_AUTO_TEST_CASE(test_rewrite_rule_real_print)
{
  BOOST_CHECK(print_container_check(sort_real::real_generate_equations_code()));
}

BOOST_AUTO_TEST_CASE(test_rewrite_rule_fset_print)
{
  BOOST_CHECK(print_container_check(sort_fset::fset_generate_equations_code(sort_bool::bool_())));
}

BOOST_AUTO_TEST_CASE(test_rewrite_rule_set_print)
{
  BOOST_CHECK(print_container_check(sort_set::set_generate_equations_code(sort_bool::bool_())));
}

BOOST_AUTO_TEST_CASE(test_rewrite_rule_fbag_print)
{
  BOOST_CHECK(print_container_check(sort_fbag::fbag_generate_equations_code(sort_bool::bool_())));
}

BOOST_AUTO_TEST_CASE(test_rewrite_rule_bag_print)
{
  BOOST_CHECK(print_container_check(sort_bag::bag_generate_equations_code(sort_bool::bool_())));
}

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  return 0;
}
