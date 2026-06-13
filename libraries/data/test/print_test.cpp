// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_TEST_MODULE print_test

#include <boost/test/included/unit_test.hpp>

#include "mcrl2/data/parse.h"
#include "mcrl2/data/print.h"

using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::data::sort_bool;
using namespace mcrl2::data::sort_nat;

template <typename T>
void test_term(const std::string& , const T& x)
{
  std::cout << data::pp(x) << std::endl;
}

void test_term(const std::string& s)
{
  atermpp::aterm a = atermpp::read_term_from_string(s);
  if (s.starts_with("DataEqn"))
  {
    data_equation x (a);
    test_term(s, x);
  }
  else if (s.starts_with("SortCons"))
  {
    sort_expression x (a);
    test_term(s, x);
  }
  else if (s.starts_with("OpId"))
  {
    data::function_symbol x (a);
    test_term(s, x);
  }
  else
  {
    const data_expression& x = atermpp::down_cast<data_expression>(a);
    test_term(s, x);
  }
}

template <typename ExpressionType>
bool print_check(ExpressionType const& left, std::string const& right)
{
  std::clog << "Checking printing of " << data::pp(left) << " with " << right << std::endl;
  if (pp(left) != right)
  {
    std::clog << "pp(" << left << ") != " << right << std::endl;
    std::clog << left << " != " << right << std::endl;

    return false;
  }

  return true;
}

template <typename Container>
bool print_container_check(Container const& c)
{
  std::string r = data::pp(c);
  if (r == "")
  {
    std::clog << "error printing container" << std::endl;
    return false;
  }

  return true;
}

// This must remain a macro: BOOST_CHECK reports the failure at the invocation line.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define PRINT_CHECK(x,y) BOOST_CHECK(print_check(x,y))

BOOST_AUTO_TEST_CASE(test_function_symbol_print)
{
  data::function_symbol f("f", sort_bool::bool_());

  PRINT_CHECK(f, "f");
}

BOOST_AUTO_TEST_CASE(test_application_print)
{
  data::function_symbol f("f", make_function_sort_(bool_(), bool_()));
  data::function_symbol g("g", make_function_sort_(bool_(), nat(), bool_()));

  PRINT_CHECK(f(true_()), "f(true)");
  PRINT_CHECK(g(false_(), sort_nat::nat(10)), "g(false, 10)");
  PRINT_CHECK(g(f(true_()), sort_nat::nat(10)), "g(f(true), 10)");
}

BOOST_AUTO_TEST_CASE(test_abstraction_print)
{
  using namespace sort_pos;
  using namespace sort_nat;

  variable_vector x {variable("x", sort_nat::nat())};
  variable_vector y {variable("y", sort_pos::pos())};
  variable_vector xy {x[0], y[0]};

  PRINT_CHECK(lambda(x, equal_to(x[0], nat(10))), "lambda x: Nat. x == 10");
  PRINT_CHECK(lambda(xy, equal_to(xy[0], pos2nat(xy[1]))), "lambda x: Nat, y: Pos. x == y");
  PRINT_CHECK(exists(x, equal_to(x[0], nat(10))), "exists x: Nat. x == 10");
  PRINT_CHECK(exists(xy, equal_to(xy[0], pos2nat(xy[1]))), "exists x: Nat, y: Pos. x == y");
  PRINT_CHECK(forall(x, equal_to(x[0], nat(10))), "forall x: Nat. x == 10");
  PRINT_CHECK(forall(xy, equal_to(xy[0], pos2nat(xy[1]))), "forall x: Nat, y: Pos. x == y");
  PRINT_CHECK(forall(x, exists(y, not_equal_to(xy[0], pos2nat(xy[1])))), "forall x: Nat. exists y: Pos. x != y");
}

BOOST_AUTO_TEST_CASE(test_list_print)
{
  using namespace sort_bool;
  using namespace sort_list;

  data_expression empty_(empty(bool_()));

  // Using all operations
  BOOST_CHECK(print_check(empty(bool_()), "[]"));
  BOOST_CHECK(print_check(cons_(bool_(), true_(), empty_), "[true]"));
  BOOST_CHECK(print_check(cons_(bool_(), false_(), cons_(bool_(), true_(), empty_)), "[false, true]"));
  BOOST_CHECK(print_check(snoc(bool_(), cons_(bool_(), true_(), empty_), false_()), "[true] <| false"));
  BOOST_CHECK(print_check(snoc(bool_(), snoc(bool_(), empty_, true_()), false_()), "[true, false]"));
  BOOST_CHECK(print_check(cons_(bool_(), in(bool_(), false_(), cons_(bool_(), true_(), empty_)), empty_), "[false in [true]]"));
  BOOST_CHECK(print_check(snoc(bool_(), cons_(bool_(), false_(), snoc(bool_(), empty_, true_())), true_()), "(false |> [true]) <| true"));
  BOOST_CHECK(print_check(in(bool_(), true_(), cons_(bool_(), false_(), snoc(bool_(), empty_, true_()))), "true in false |> [true]"));
  BOOST_CHECK(print_check(count(bool_(), cons_(bool_(), false_(), snoc(bool_(), empty_, true_()))), "#(false |> [true])"));
  BOOST_CHECK(print_check(concat(bool_(), cons_(bool_(), true_(), empty_), cons_(bool_(), false_(), snoc(bool_(), empty_, true_()))), "[true] ++ (false |> [true])"));
  BOOST_CHECK(print_check(element_at(bool_(), cons_(bool_(), false_(), snoc(bool_(), empty_, true_())), sort_nat::nat(1)), "(false |> [true]) . 1"));
  BOOST_CHECK(print_check(head(bool_(), cons_(bool_(), false_(), cons_(bool_(), true_(), empty_))), "head([false, true])"));
  BOOST_CHECK(print_check(tail(bool_(), cons_(bool_(), false_(), cons_(bool_(), true_(), empty_))), "tail([false, true])"));
  BOOST_CHECK(print_check(rhead(bool_(), cons_(bool_(), false_(), cons_(bool_(), true_(), empty_))), "rhead([false, true])"));
  BOOST_CHECK(print_check(rtail(bool_(), cons_(bool_(), false_(), cons_(bool_(), true_(), empty_))), "rtail([false, true])"));
  BOOST_CHECK(print_check(cons_(bool_(), true_(), concat(bool_(), cons_(bool_(), true_(), empty_), cons_(bool_(), false_(), empty_))), "true |> [true] ++ [false]"));
  BOOST_CHECK(print_check(snoc(bool_(), concat(bool_(), cons_(bool_(), true_(), empty_), cons_(bool_(), false_(), empty_)), true_()), "[true] ++ [false] <| true"));
  BOOST_CHECK(print_check(cons_(bool_(), false_(), snoc(bool_(), concat(bool_(), cons_(bool_(), true_(), empty_), cons_(bool_(), false_(), empty_)), true_())), "false |> [true] ++ [false] <| true"));

  // lists of lists
  data_expression list_empty = empty(list(bool_()));
  data_expression list_true = cons_(bool_(), true_(), empty_);
  data_expression list_false_true = cons_(bool_(), false_(), cons_(bool_(), true_(), empty_));

  BOOST_CHECK(print_check(empty(list(bool_())), "[]"));
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

  // Sort expression
  BOOST_CHECK(print_check(list(bool_()), "List(Bool)"));


}

BOOST_AUTO_TEST_CASE(test_set_print)
{
  using namespace sort_bool;

  data_expression set_empty = sort_fset::empty(bool_());
  data_expression set_false = sort_set::set_fset(bool_(), sort_fset::cons_(bool_(), false_(), sort_fset::empty(bool_())));
  data_expression set_true = sort_set::set_fset(bool_(), sort_fset::cons_(bool_(), true_(), sort_fset::empty(bool_())));

  // Using all operations
  BOOST_CHECK(print_check(sort_fset::empty(bool_()), "{}"));
  BOOST_CHECK(print_check(sort_set::set_fset(bool_(), sort_fset::empty(bool_())), "{}"));
  BOOST_CHECK(print_check(sort_set::constructor(bool_(), sort_set::false_function(bool_()), sort_fset::empty(bool_())), "{}"));
  BOOST_CHECK(print_check(sort_set::in(bool_(), false_(), set_empty), "false in {}"));
  BOOST_CHECK(print_check(sort_set::union_(bool_(), set_false, set_true), "{false} + {true}"));
  BOOST_CHECK(print_check(sort_set::intersection(bool_(), set_false, set_true), "{false} * {true}"));
  BOOST_CHECK(print_check(sort_set::difference(bool_(), set_false, set_true), "{false} - {true}"));
  BOOST_CHECK(print_check(sort_set::complement(bool_(), set_false), "!{false}"));

  // Some parsed expressions
  BOOST_CHECK(print_check(parse_data_expression("{true}"), "{true}"));
  BOOST_CHECK(print_check(parse_data_expression("{true, false}"), "{true, false}"));
  BOOST_CHECK(print_check(parse_data_expression("{ b: Bool | b }"), "{ b: Bool | b }"));
  BOOST_CHECK(print_check(parse_data_expression("{ x: Nat | x == 0 }"), "{ x: Nat | x == 0 }"));
  BOOST_CHECK(print_check(parse_data_expression("{ x: Nat | (lambda y: Nat. y == 0)(x) }"), "{ x: Nat | (lambda y: Nat. y == 0)(x) }"));

  // Some types
  BOOST_CHECK(print_check(sort_fset::fset(bool_()), "FSet(Bool)"));
  BOOST_CHECK(print_check(parse_sort_expression("Set(Nat)"), "Set(Nat)"));
}

BOOST_AUTO_TEST_CASE(test_bag_print)
{
  using namespace sort_bool;
  using namespace sort_nat;

  data_expression bag_empty(sort_fbag::empty(bool_()));
  data_expression fbag_empty_(sort_fbag::empty(bool_()));
  data_expression bag_false = sort_bag::bag_fbag(bool_(), sort_fbag::cons_(bool_(), false_(), number(sort_pos::pos(), "1"), fbag_empty_));
  data_expression bag_true = sort_bag::bag_fbag(bool_(), sort_fbag::cons_(bool_(), true_(), number(sort_pos::pos(), "1"), fbag_empty_));

  // Using all operations
  BOOST_CHECK(print_check(bag_empty, "{:}"));
  BOOST_CHECK(print_check(sort_bag::bag_fbag(bool_(), sort_fbag::empty(bool_())), "{:}"));
  BOOST_CHECK(print_check(sort_bag::constructor(bool_(), sort_bag::zero_function(bool_()), fbag_empty_), "{:}"));
  BOOST_CHECK(print_check(sort_bag::in(bool_(), false_(), bag_empty), "false in {:}"));
  BOOST_CHECK(print_check(sort_bag::in(bool_(), false_(), bag_false), "false in {false: 1}"));
  BOOST_CHECK(print_check(sort_bag::count(bool_(), false_(), bag_true), "count(false, {true: 1})"));
  BOOST_CHECK(print_check(sort_bag::union_(bool_(), bag_false, bag_true), "{false: 1} + {true: 1}"));
  BOOST_CHECK(print_check(sort_bag::intersection(bool_(), bag_false, bag_true), "{false: 1} * {true: 1}"));
  BOOST_CHECK(print_check(sort_bag::difference(bool_(), bag_false, bag_true), "{false: 1} - {true: 1}"));

  // Some parsed expressions
  BOOST_CHECK(print_check(parse_data_expression("{true: 2}"), "{true: 2}"));
  BOOST_CHECK(print_check(parse_data_expression("{false: 3, true: 1}"), "{false: 3, true: 1}"));
  BOOST_CHECK(print_check(parse_data_expression("{ b: Bool | if(b, Pos2Nat(2), Pos2Nat(3)) }"), "{ b: Bool | if(b, 2, 3) }"));
  BOOST_CHECK(print_check(parse_data_expression("{ x: Nat | x * x }"), "{ x: Nat | x * x }"));
  BOOST_CHECK(print_check(parse_data_expression("{ x: Nat | (lambda y: Nat. y * y)(x) }"), "{ x: Nat | (lambda y: Nat. y * y)(x) }"));

  // Some types
  BOOST_CHECK(print_check(sort_fbag::fbag(bool_()), "FBag(Bool)"));
  BOOST_CHECK(print_check(parse_sort_expression("Bag(Nat)"), "Bag(Nat)"));
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

BOOST_AUTO_TEST_CASE(test_standard_sort_expressions)
{
  BOOST_CHECK(print_check(sort_bool::bool_(), "Bool"));
  BOOST_CHECK(print_check(sort_pos::pos(), "Pos"));
  BOOST_CHECK(print_check(sort_nat::nat(), "Nat"));
  BOOST_CHECK(print_check(sort_int::int_(), "Int"));
  BOOST_CHECK(print_check(sort_real::real_(), "Real"));
}

BOOST_AUTO_TEST_CASE(test_mod)
{
  data::data_expression x = parse_data_expression("(1 + 2) mod 3");
  BOOST_CHECK(sort_nat::is_mod_application(x));

  application left = atermpp::down_cast<application>(sort_nat::left(x));
  std::cout << "left = " << left << " " << data::pp(left) << std::endl;
  BOOST_CHECK(data::detail::is_plus(left));

  BOOST_CHECK(data::pp(x) == "(1 + 2) mod 3");
  std::cout << "x = " << x << " " << data::pp(x) << std::endl;

  x = parse_data_expression("(2 - 1) mod 3");
  left = atermpp::down_cast<application>(sort_int::left(x));
  std::cout << "left = " << left << " " << data::pp(left) << std::endl;
  BOOST_CHECK(data::detail::is_minus(left));
  std::cout << "precedence(left) = " << precedence(left) << std::endl;

  BOOST_CHECK(data::sort_nat::is_nat(x.sort()));
  BOOST_CHECK(data::sort_int::is_mod_application(x));

  BOOST_CHECK(data::pp(x) == "(2 - 1) mod 3");
  std::cout << "x = " << x << " " << data::pp(x) << std::endl;
}

BOOST_AUTO_TEST_CASE(test_sort_expressions)
{
  data::sort_expression x = parse_sort_expression("Bool # Nat -> (Pos -> Real)");
  std::string xtext = data::pp(x);
  data::sort_expression y = parse_sort_expression(xtext);
  std::string ytext = data::pp(y);
  std::cout << "original = " << "Bool # Nat -> (Pos -> Real)" << std::endl;
  std::cout << "xtext    = " << xtext << std::endl;
  std::cout << "ytext    = " << ytext << std::endl;
  BOOST_CHECK(x == y);
  BOOST_CHECK(xtext == ytext);
  BOOST_CHECK(data::pp(data::untyped_sort()) == "untyped_sort");
}

BOOST_AUTO_TEST_CASE(test_set_print2)
{
  using namespace sort_bool;

  data_expression one = parse_data_expression("1");
  data_expression x = sort_fset::insert(sort_nat::nat(), one, sort_fset::empty(sort_nat::nat()));
  const sort_expression& s = sort_nat::nat();

  data_expression true_  = sort_set::false_function(s);
  data_expression false_ = sort_set::true_function(s);
  data_expression f      = parse_function_symbol("f: Pos -> Bool");

  data_expression x1 = sort_set::constructor(s, true_, x);
  data_expression x2 = sort_set::constructor(s, false_, x);
  data_expression x3 = sort_set::constructor(s, f, x);

  BOOST_CHECK_EQUAL(data::pp(x1), "{1}");
  BOOST_CHECK_EQUAL(data::pp(x2), "!{1}");
  BOOST_CHECK_EQUAL(data::pp(x3), "{ x: Pos | f(x) != x in {1} }");
}

BOOST_AUTO_TEST_CASE(test_fset_print)
{
  using namespace sort_bool;

  data_expression one = parse_data_expression("1");
  data_expression x = parse_data_expression("{1, 2}");
  data_expression y = parse_data_expression("{3}");
  const sort_expression& s = sort_pos::pos();
  data_expression f = parse_function_symbol("f: Pos -> Bool");
  data_expression g = parse_function_symbol("g: Pos -> Bool");
  data_expression false_ = sort_set::false_function(s);
  data_expression true_ = sort_set::true_function(s);

  data_expression xy_union = sort_set::fset_union(s, false_, false_, x, y);
  data_expression xy_intersection = sort_set::fset_intersection(s, false_, false_, x, y);
  data_expression xy_difference = sort_fset::difference(s, x, y);
  data_expression xy_in = sort_fset::in(s, one, x);

  BOOST_CHECK_EQUAL(data::pp(xy_union)       , "{1, 2} + {3}");
  BOOST_CHECK_EQUAL(data::pp(xy_intersection), "{1, 2} * {3}");
  BOOST_CHECK_EQUAL(data::pp(xy_difference)  , "{1, 2} - {3}");
  BOOST_CHECK_EQUAL(data::pp(xy_in)          , "1 in {1, 2}");

  xy_union = sort_set::fset_union(s, f, false_, x, y);
  xy_intersection = sort_set::fset_intersection(s, f, false_, x, y);
  BOOST_CHECK_EQUAL(data::pp(xy_union)       , "{1, 2} + { x: Pos | !f(x) && x in {3} }");
  BOOST_CHECK_EQUAL(data::pp(xy_intersection), "{1, 2} * { x: Pos | !f(x) && x in {3} }");

  xy_union = sort_set::fset_union(s, f, g, x, y);
  xy_intersection = sort_set::fset_intersection(s, f, g, x, y);
  BOOST_CHECK_EQUAL(data::pp(xy_union)       , "{ x: Pos | !g(x) && x in {1, 2} } + { x: Pos | !f(x) && x in {3} }");
  BOOST_CHECK_EQUAL(data::pp(xy_intersection), "{ x: Pos | !g(x) && x in {1, 2} } * { x: Pos | !f(x) && x in {3} }");
}

BOOST_AUTO_TEST_CASE(test_precedence)
{
  data::data_expression x = parse_data_expression("exists b:Bool. true");
  BOOST_CHECK(is_exists(x));
  BOOST_CHECK(precedence(x) == 1);

  data::variable a("a", sort_real::real_());
  data::variable b("b", sort_real::real_());
  std::vector<data::variable> variables = { a, b };
  data::data_expression y = parse_data_expression("-(a + b)", variables);
  std::string py = data::pp(y);
  BOOST_CHECK(py == "-(a + b)");
}

// The test case below tests whether an excessive amount of
// memory is required when printing terms. 
BOOST_AUTO_TEST_CASE(printing_terms_takes_a_lot_of_memory)
{
   const std::string text(
    "sort S;\n"
    "cons d0:S;\n"
    "     f:S -> S;\n"
  );

  data::data_specification spec(data::parse_data_specification(text));

  data_expression f = parse_function_symbol("f: S -> S",text);
  std::cerr << f << "\n";
  data_expression t = parse_data_expression("d0",spec);
  std::cerr << t << "\n";

  for(size_t i=0; i<40; ++i)
  {
    t=application(f,t);
    std::cerr << t << "\n";
  }
  std::cerr << "Print term " << data::pp(t) << "\n";;
}

