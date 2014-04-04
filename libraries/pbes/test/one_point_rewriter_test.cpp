// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file rewriter_test.cpp
/// \brief Test for PBES rewriters.

//#define MCRL2_PBES_EXPRESSION_BUILDER_DEBUG
//#define MCRL2_ENUMERATE_QUANTIFIERS_BUILDER_DEBUG
//#define MCRL2_ENUMERATE_QUANTIFIERS_REWRITER_DEBUG
//#define PBES_REWRITE_TEST_DEBUG

#include <functional>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <boost/test/minimal.hpp>
#include "mcrl2/data/parse.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/detail/parse_substitutions.h"
#include "mcrl2/pbes/detail/normalize_and_or.h"
#include "mcrl2/pbes/detail/data2pbes_rewriter.h"
#include "mcrl2/pbes/parse.h"
#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/pbes/rewrite.h"
#include "mcrl2/pbes/txt2pbes.h"
#include "mcrl2/pbes/one_point_rule_rewriter.h"
#include "mcrl2/utilities/text_utility.h"
#include "mcrl2/utilities/detail/test_operation.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;

const std::string VARIABLE_SPECIFICATION =
  "datavar         \n"
  "  b:  Bool;     \n"
  "  b1: Bool;     \n"
  "  b2: Bool;     \n"
  "  b3: Bool;     \n"
  "                \n"
  "  n:  Nat;      \n"
  "  n1: Nat;      \n"
  "  n2: Nat;      \n"
  "  n3: Nat;      \n"
  "                \n"
  "  p:  Pos;      \n"
  "  p1: Pos;      \n"
  "  p2: Pos;      \n"
  "  p3: Pos;      \n"
  "                \n"
  "predvar         \n"
  "  X;            \n"
  "  Y: Nat;       \n"
  "  W: Bool;      \n"
  "  Z: Bool, Nat; \n"
  "  X0;           \n"
  "  X1: Bool;     \n"
  "  X2: Nat, Nat; \n"
  "  X3: Bool, Nat;\n"
  "  X4: Nat, Bool;\n"
  ;

// PBES expression parser
class parser
{
  protected:
    std::string m_var_decl;
    std::string m_data_spec;

  public:

    parser(const std::string& var_decl = VARIABLE_SPECIFICATION, const std::string& data_spec = "")
      : m_var_decl(var_decl),
        m_data_spec(data_spec)
    {}

    pbes_expression operator()(const std::string& expr)
    {
      return pbes_system::parse_pbes_expression(expr, m_var_decl, m_data_spec);
    }
};

void test_one_point_rule_rewriter(const std::string& expr1, const std::string& expr2)
{
  one_point_rule_rewriter R;
  BOOST_CHECK(utilities::detail::test_operation(
    expr1,
    expr2,
    parser(),
    std::equal_to<pbes_expression>(),
    R,
    "R1",
    &utilities::detail::identity<pbes_expression>,
    "R2"
  ));
}

void test_one_point_rule_rewriter()
{
  std::cout << "<test_one_point_rule_rewriter>" << std::endl;
  one_point_rule_rewriter R;
  pbes_system::pbes_expression x;
  pbes_system::pbes_expression y;

  x = pbes_system::parse_pbes_expression("forall n: Nat. val(n != 3) || val(n == 5)");
  y = R(x);
  std::clog << "x = " << pbes_system::pp(x) << std::endl;
  std::clog << "y = " << pbes_system::pp(y) << std::endl;
  BOOST_CHECK(pbes_system::pp(y) == "3 == 5" || pbes_system::pp(y) == "5 == 3");

  x = pbes_system::parse_pbes_expression("exists n: Nat. val(n == 3) && val(n == 5)");
  y = R(x);
  std::clog << "x = " << pbes_system::pp(x) << std::endl;
  std::clog << "y = " << pbes_system::pp(y) << std::endl;
  BOOST_CHECK(pbes_system::pp(y) == "3 == 5" || pbes_system::pp(y) == "5 == 3");

  test_one_point_rule_rewriter("forall c: Bool. forall b: Bool. val(b) => val(b || c)", "val(!false)");
  test_one_point_rule_rewriter("forall d:Nat. val(d == 1) => Y(d)", "Y(1)");
  test_one_point_rule_rewriter("forall m:Nat. exists n:Nat. val(m == n) && Y(n)", "forall m: Nat. Y(m)");
  test_one_point_rule_rewriter("forall m:Nat. exists n:Nat. val(n == m) && Y(n)", "forall m: Nat. Y(m)");
  test_one_point_rule_rewriter("exists m:Nat. forall n:Nat. val(m != n) || Y(n)", "exists m: Nat. Y(m)");
  test_one_point_rule_rewriter("exists m:Nat. forall n:Nat. val(n != m) || Y(n)", "exists m: Nat. Y(m)");

  // N.B. The result of this test depends on the order of the clauses.
  // test_one_point_rule_rewriter("forall p: Bool, q: Bool. val(!(p == q)) || val(!q) || val(!(p == true))", "val(false)");
}

int test_main(int argc, char* argv[])
{
  log::mcrl2_logger::set_reporting_level(log::debug, "one_point_rewriter");
  test_one_point_rule_rewriter();

  return 0;
}
