// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <sstream>

#include "boost/test/minimal.hpp"

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
#include "mcrl2/data/print.h"
#include "mcrl2/data/detail/sort_traverser.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace mcrl2::data;
using namespace mcrl2::data::sort_bool;
using namespace mcrl2::data::sort_nat;

bool print_check(data_expression const& left, std::string const& right) {
  if (pp(left) != right)
  {
    std::clog << "pp(" << pp(left) << ") != " << right << std::endl;

    return false;
  }

  return true;
}

#define PRINT_CHECK(x,y) BOOST_CHECK(print_check(x,y))

void test_function_symbol_print() {
  function_symbol f("f", sort_bool::bool_());

  PRINT_CHECK(f, "f");
}

void test_application_print() {
  function_symbol f("f", function_sort(bool_(), bool_()));
  function_symbol g("g", function_sort(bool_(), nat(), bool_()));

  PRINT_CHECK(f(true_()), "f(true)");
  PRINT_CHECK(g(false_(), sort_nat::nat(10)), "g(false, 10)");
  PRINT_CHECK(g(f(true_()), sort_nat::nat(10)), "g(f(true), 10)");
}

void test_abstraction_print() {
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

bool stream_print_test(data_expression const& input, std::string const& expected_output) {
  std::ostringstream o;

  detail::pretty_print< data_expression >(o, input);

  if (o.str() != expected_output) {
    std::clog << "Expected " << expected_output << " got " << o.str() << " instead" << std::endl;

    return false;
  }

  return true;
}

void test_list_print() {
  using namespace sort_bool;
  using namespace sort_list;

  data_expression empty(nil(bool_()));

  // Using all operations
  BOOST_CHECK(stream_print_test(nil(bool_()), "[]"));
  BOOST_CHECK(stream_print_test(cons_(bool_(), true_(), empty), "[true]"));
  BOOST_CHECK(stream_print_test(cons_(bool_(), false_(), cons_(bool_(), true_(), empty)), "[false, true]"));
  BOOST_CHECK(stream_print_test(snoc(bool_(), false_(), cons_(bool_(), true_(), empty)), "[true, false]"));
  BOOST_CHECK(stream_print_test(snoc(bool_(), false_(), snoc(bool_(), true_(), empty)), "[true, false]"));
  BOOST_CHECK(stream_print_test(cons_(bool_(), in(bool_(), false_(), cons_(bool_(), true_(), empty)), empty), "[false in [true]]"));
  BOOST_CHECK(stream_print_test(snoc(bool_(), true_(), cons_(bool_(), false_(), snoc(bool_(), true_(), empty))), "[false, true, true]"));
  BOOST_CHECK(stream_print_test(in(bool_(), true_(), cons_(bool_(), false_(), snoc(bool_(), true_(), empty))), "true in [false, true]"));
  BOOST_CHECK(stream_print_test(count(bool_(), cons_(bool_(), false_(), snoc(bool_(), true_(), empty))), "#[false, true]"));
  BOOST_CHECK(stream_print_test(concat(bool_(), cons_(bool_(), true_(), empty), cons_(bool_(), false_(), snoc(bool_(), true_(), empty))), "[true] ++ [false, true]"));
  BOOST_CHECK(stream_print_test(element_at(bool_(), sort_nat::nat(1), cons_(bool_(), false_(), snoc(bool_(), true_(), empty))), "[false, true].1"));
  BOOST_CHECK(stream_print_test(head(bool_(), cons_(bool_(), false_(), cons_(bool_(), true_(), empty))), "head([false, true])"));
  BOOST_CHECK(stream_print_test(tail(bool_(), cons_(bool_(), false_(), cons_(bool_(), true_(), empty))), "tail([false, true])"));
  BOOST_CHECK(stream_print_test(rhead(bool_(), cons_(bool_(), false_(), cons_(bool_(), true_(), empty))), "rhead([false, true])"));
  BOOST_CHECK(stream_print_test(rtail(bool_(), cons_(bool_(), false_(), cons_(bool_(), true_(), empty))), "rtail([false, true])"));
  BOOST_CHECK(stream_print_test(cons_(bool_(), true_(), concat(bool_(), cons_(bool_(), true_(), empty), cons_(bool_(), false_(), empty))), "true |> [true] ++ [false]"));
  BOOST_CHECK(stream_print_test(snoc(bool_(), true_(), concat(bool_(), cons_(bool_(), true_(), empty), cons_(bool_(), false_(), empty))), "[true] ++ [false] <| true"));
  BOOST_CHECK(stream_print_test(cons_(bool_(), false_(), snoc(bool_(), true_(), concat(bool_(), cons_(bool_(), true_(), empty), cons_(bool_(), false_(), empty)))), "false |> [true] ++ [false] <| true"));

  // lists of lists
  data_expression list_empty = nil(list(bool_()));
  data_expression list_true = cons_(bool_(), true_(), empty);
  data_expression list_false_true = cons_(bool_(), false_(), cons_(bool_(), true_(), empty));

  BOOST_CHECK(stream_print_test(nil(list(bool_())), "[]"));
  BOOST_CHECK(stream_print_test(list_true, "[true]"));
  BOOST_CHECK(stream_print_test(cons_(list(bool_()), list_true, list_empty), "[[true]]"));
  BOOST_CHECK(stream_print_test(cons_(list(bool_()), list_true, cons_(list(bool_()), list_false_true, list_empty)), "[[true], [false, true]]"));
  BOOST_CHECK(stream_print_test(snoc(list(bool_()), list_true, cons_(list(bool_()), list_false_true, list_empty)), "[[false, true], [true]]"));
  BOOST_CHECK(stream_print_test(in(list(bool_()), list_true, cons_(list(bool_()), list_true, list_empty)), "[true] in [[true]]"));
  BOOST_CHECK(stream_print_test(in(list(bool_()), list_true, cons_(list(bool_()), list_true, list_empty)), "[true] in [[true]]"));

  // List enumeration
  data_expression_vector v;
  v.push_back(sort_bool::true_());
  v.push_back(sort_bool::false_());
  v.push_back(sort_bool::true_());
  v.push_back(sort_bool::true_());
  data_expression l1(list_enumeration(list(sort_bool::bool_()), v));
  BOOST_CHECK(stream_print_test(l1, "[true, false, true, true]"));

  data_expression l2(sort_list::list(sort_bool::bool_(), v));
  BOOST_CHECK(stream_print_test(l2, "[true, false, true, true]"));

  
}

void test_set_print() {
  using namespace sort_bool;
  using namespace sort_set;
  using namespace sort_fset;

  data_expression set_empty = emptyset(bool_());
  data_expression set_false = setfset(bool_(), fset_cons(bool_(), false_(), fset_empty(bool_())));
  data_expression set_true = setfset(bool_(), fset_cons(bool_(), true_(), fset_empty(bool_())));

  // Using all operations
  BOOST_CHECK(stream_print_test(emptyset(bool_()), "{}"));
  BOOST_CHECK(stream_print_test(setfset(bool_(), fset_empty(bool_())), "{}"));
  BOOST_CHECK(stream_print_test(setconstructor(bool_(), false_function(bool_()), fset_empty(bool_())), "{}"));
  BOOST_CHECK(stream_print_test(setin(bool_(), false_(), set_empty), "false in {}"));
  BOOST_CHECK(stream_print_test(setunion_(bool_(), set_false, set_true), "{false} + {true}"));
  BOOST_CHECK(stream_print_test(setintersection(bool_(), set_false, set_true), "{false} * {true}"));
  BOOST_CHECK(stream_print_test(setdifference(bool_(), set_false, set_true), "{false} - {true}"));
  BOOST_CHECK(stream_print_test(setcomplement(bool_(), set_false), "!{false}"));
}

void test_bag_print() {
  using namespace sort_bool;
  using namespace sort_bag;
  using namespace sort_fbag;
  using namespace sort_nat;

  data_expression bag_empty(emptybag(bool_()));
  data_expression fbag_empty_(fbag_empty(bool_()));
  data_expression bag_false = bagfbag(bool_(), fbag_cons(bool_(), false_(), number(sort_pos::pos(), "1"), fbag_empty_));
  data_expression bag_true = bagfbag(bool_(), fbag_cons(bool_(), true_(), number(sort_pos::pos(), "1"), fbag_empty_));

  // Using all operations
  BOOST_CHECK(stream_print_test(bag_empty, "{}"));
  BOOST_CHECK(stream_print_test(bagfbag(bool_(), fbag_empty(bool_())), "{}"));
  BOOST_CHECK(stream_print_test(bagconstructor(bool_(), zero_function(bool_()), fbag_empty_), "{}"));
  BOOST_CHECK(stream_print_test(bagin(bool_(), false_(), bag_empty), "false in {}"));
  BOOST_CHECK(stream_print_test(bagin(bool_(), false_(), bag_false), "false in {false: 1}"));
  BOOST_CHECK(stream_print_test(bagcount(bool_(), false_(), bag_true), "count(false, {true: 1})"));
  BOOST_CHECK(stream_print_test(bagjoin(bool_(), bag_false, bag_true), "{false: 1} + {true: 1}"));
  BOOST_CHECK(stream_print_test(bagintersect(bool_(), bag_false, bag_true), "{false: 1} * {true: 1}"));
  BOOST_CHECK(stream_print_test(bagdifference(bool_(), bag_false, bag_true), "{false: 1} - {true: 1}"));
}

void test_structured_sort_print() {
}

template < typename Derived >
class printer : public mcrl2::data::detail::sort_traverser< Derived >
{
  protected:
    unsigned int                m_inside_quantifier;
    std::set< sort_expression > m_result;

  public:

    printer() : m_inside_quantifier(0) {
    }

    void enter(sort_expression const& s)
    {
      if (0 < m_inside_quantifier)
      {
        m_result.insert(s);
      }
    }

    void enter(forall const& e) {
      ++m_inside_quantifier;
    }

    void leave(forall const& e) {
      --m_inside_quantifier;
    }
};

class forall_printer : public printer< forall_printer >
{};
class exists_printer : public printer< exists_printer >
{
  public:

    void enter(exists const& e) {
      ++m_inside_quantifier;
    }

    void leave(exists const& e) {
      --m_inside_quantifier;
    }
};

void test_traversal() {
}

int test_main(int argc, char** argv) {
  MCRL2_ATERMPP_INIT_DEBUG(argc, argv);

  test_function_symbol_print();
  test_application_print();
  test_abstraction_print();
  test_list_print();
  test_set_print();
  test_bag_print();
  test_structured_sort_print();

  return EXIT_SUCCESS;
}
