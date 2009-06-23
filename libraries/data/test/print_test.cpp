// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

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
#include "mcrl2/data/detail/expression_traverser.h"

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

  PRINT_CHECK(application(f, true_()), "f(true)");
  PRINT_CHECK(application(g, false_(), sort_nat::nat(10)), "g(false, 10)");
  PRINT_CHECK(application(g, application(f, true_()), sort_nat::nat(10)), "g(f(true), 10)");
}

void test_abstraction_print() {
  using namespace sort_pos;
  using namespace sort_nat;

  variable_vector x(make_vector(variable("x", sort_nat::nat())));
  variable_vector y(make_vector(variable("y", sort_pos::pos())));
  variable_vector xy(make_vector(x[0], y[0]));

  PRINT_CHECK(lambda(x, equal_to(x[0], nat(10))), "lambda x: Nat. x == 10");
  PRINT_CHECK(lambda(xy, equal_to(xy[0], pos2nat(xy[1]))), "lambda x: Nat, y: Pos. x == y");
  PRINT_CHECK(exists(x, equal_to(x[0], nat(10))), "exists x: Nat. x == 10");
  PRINT_CHECK(exists(xy, equal_to(xy[0], pos2nat(xy[1]))), "exists x: Nat, y: Pos. x == y");
  PRINT_CHECK(forall(x, equal_to(x[0], nat(10))), "forall x: Nat. x == 10");
  PRINT_CHECK(forall(xy, equal_to(xy[0], pos2nat(xy[1]))), "forall x: Nat, y: Pos. x == y");
  PRINT_CHECK(forall(x, exists(y, not_equal_to(xy[0], pos2nat(xy[1])))), "forall x: Nat. exists y: Pos. x != y");
}

void test_set_print() {
  using namespace sort_nat;
  using namespace sort_set;
  using namespace sort_fset;

  data_expression s1(setfset(nat(), fsetinsert(nat(), nat(1), fset_empty(nat()))));
  data_expression s2(setfset(nat(), fsetinsert(nat(), nat(2), fset_empty(nat()))));
  data_expression s12(setfset(nat(), fsetinsert(nat(), nat(1), fsetinsert(nat(), nat(2), fset_empty(nat())))));

//  PRINT_CHECK(s1, "{ 1 }");
//  PRINT_CHECK(s12, "{ 1, 2 }");
}

void test_bag_print() {
}

void test_structured_sort_print() {
}

template < typename Derived >
class printer : public mcrl2::data::detail::sort_expression_traverser< Derived >
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
  test_set_print();
  test_bag_print();
  test_structured_sort_print();

  return EXIT_SUCCESS;
}
