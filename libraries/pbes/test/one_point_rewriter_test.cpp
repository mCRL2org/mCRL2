// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file one_point_rewriter_test.cpp
/// \brief Test for PBES rewriters.

#include "mcrl2/data/rewriter.h"
#include "mcrl2/pbes/parse.h"
#include "mcrl2/pbes/rewrite.h"
#include "mcrl2/pbes/rewriters/one_point_rule_rewriter.h"
#include "mcrl2/pbes/rewriters/simplify_rewriter.h"
#include "mcrl2/pbes/txt2pbes.h"
#include "mcrl2/utilities/detail/test_operation.h"
#include "mcrl2/utilities/text_utility.h"
#include <boost/test/included/unit_test_framework.hpp>
#include <functional>
#include <iostream>
#include <set>
#include <sstream>
#include <string>

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
  data::data_specification dataspec;
  dataspec.add_context_sort(data::sort_nat::nat()); // TODO: is there a more elegant way to achieve this?
  data::rewriter r(dataspec);
  simplify_data_rewriter<data::rewriter> S(r);
  BOOST_CHECK(utilities::detail::test_operation(
    expr1,
    expr2,
    parser(),
    std::equal_to<pbes_expression>(),
    [&](const pbes_expression& x) { return S(R(x)); },
    "R1",
    S,
    "R2"
  ));
}

BOOST_AUTO_TEST_CASE(one_point_rule_rewriter_test)
{
  std::cout << "<test_one_point_rule_rewriter>" << std::endl;
  test_one_point_rule_rewriter("forall n: Nat. val(n != 3) || val(n == 5)", "val(false)");
  test_one_point_rule_rewriter("exists n: Nat. val(n == 3) && val(n == 5)", "val(false)");
  test_one_point_rule_rewriter("forall c: Bool. forall b: Bool. val(b) => val(b || c)", "val(!false)");
  test_one_point_rule_rewriter("forall d:Nat. val(d == 1) => Y(d)", "Y(1)");
  test_one_point_rule_rewriter("forall m:Nat. exists n:Nat. val(m == n) && Y(n)", "forall m: Nat. Y(m)");
  test_one_point_rule_rewriter("forall m:Nat. exists n:Nat. val(n == m) && Y(n)", "forall m: Nat. Y(m)");
  test_one_point_rule_rewriter("exists m:Nat. forall n:Nat. val(m != n) || Y(n)", "exists m: Nat. Y(m)");
  test_one_point_rule_rewriter("exists m:Nat. forall n:Nat. val(n != m) || Y(n)", "exists m: Nat. Y(m)");
  test_one_point_rule_rewriter("forall p: Bool, q: Bool. val(!(p == q)) || val(!q) || val(!(p == true))", "val(false)");
}

BOOST_AUTO_TEST_CASE(ticket_1388)
{
  std::string text;
  bool normalize = false;
  pbes p;
  one_point_rule_rewriter R;

  text =
    "pbes mu X = exists e: Bool. exists b: Bool, d: Bool. val(b == e) && val(d == b);\n"
    "init X;\n"
    ;
  p = txt2pbes(text, normalize);
  pbes_rewrite(p, R);
  BOOST_CHECK(p.is_closed() && p.is_well_typed());

  // text =
  //   "pbes nu X = forall b,c: Bool. !val(b == false) || !val(c == b);\n"
  //   "init X;"
  //   ;
  // p = txt2pbes(text, normalize);
  // pbes_rewrite(p, R);
  // BOOST_CHECK(p.is_closed() && p.is_well_typed());
}

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
  log::mcrl2_logger::set_reporting_level(log::debug, "one_point_rewriter");
  return nullptr;
}
