// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file stategraph_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <utility>
#include <set>
#include <boost/test/minimal.hpp>
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/pbes/detail/guard_traverser.h"
#include "mcrl2/pbes/find.h"
#include "mcrl2/pbes/txt2pbes.h"

using namespace mcrl2;
using namespace pbes_system;

propositional_variable_instantiation propvar(const std::string& name)
{
  return propositional_variable_instantiation(core::identifier_string(name), data::sort_expression_list());
}

void check_result(const std::string& expression, const std::string& result, const std::string& expected_result, const std::string& title)
{
  if (result != expected_result)
  {
    std::cout << "--- failure in " << title << " ---" << std::endl;
    std::cout << "expression      = " << expression << std::endl;
    std::cout << "result          = " << result << std::endl;
    std::cout << "expected result = " << expected_result << std::endl;
    BOOST_CHECK(result == expected_result);
  }
}

// find propositional variable instantiation with the given name
propositional_variable_instantiation find_propvar(const std::string& name, const pbes_expression& x)
{
  std::set<propositional_variable_instantiation> V = find_propositional_variable_instantiations(x);
  for (std::set<propositional_variable_instantiation>::const_iterator i = V.begin(); i != V.end(); ++i)
  {
    if (i->name() == core::identifier_string(name))
    {
      return *i;
    }
  }
  throw mcrl2::runtime_error("propvar not found!");
  return propositional_variable_instantiation();
}

void test_guard(const std::string& pbesspec, const std::string& X, const std::string& expected_result)
{
  bool normalize = false;
  pbes<> p = txt2pbes(pbesspec, normalize);
  pbes_expression x1 = p.equations().front().formula();
  propositional_variable_instantiation X1 = find_propvar(X, x1);

  detail::guard_traverser f;
  f(x1);
  BOOST_CHECK(f.expression_stack.back().check_guards(x1));

  pbes_expression g = detail::guard(X1, x1);
  std::string result = pbes_system::pp(g);
  check_result(X, result, expected_result, "");
}

void test_guard()
{
  std::string text =
    "pbes                          \n"
    "   mu X1 = X1 || X2;          \n"
    "   mu X2 = true;              \n"
    "init X2;                      \n"
    ;
  test_guard(text, "X1", "true");

  text =
    "pbes                                     \n"
    "   mu X1(b: Bool) = X1(true) && val(b);  \n"
    "   mu X2 = true;                         \n"
    "init X2;                                 \n"
    ;
  test_guard(text, "X1", "b");

  text =
    "pbes                                     \n"
    "   mu X1(b: Bool) = X1(true) || val(b);  \n"
    "   mu X2 = true;                         \n"
    "init X2;                                 \n"
    ;
  test_guard(text, "X1", "!val(b)");

  text =
    "pbes                                             \n"
    "   mu X1(b: Bool) = X1(true) || (val(b) && X2);  \n"
    "   mu X2 = true;                                 \n"
    "init X2;                                         \n"
    ;
  test_guard(text, "X1", "true");

  text =
    "pbes                                                       \n"
    "   mu X1(b: Bool) = (forall c: Bool. val(c)) || X1(true);  \n"
    "   mu X2 = true;                                           \n"
    "init X2;                                                   \n"
    ;
  test_guard(text, "X1", "!(forall c: Bool. val(c))");

  text =
    "pbes                                                       \n"
    "   mu X1(b: Bool) = (forall c: Bool. val(c)) && X1(true);  \n"
    "   mu X2 = true;                                           \n"
    "init X2;                                                   \n"
    ;
  test_guard(text, "X1", "forall c: Bool. val(c)");

  text =
    "pbes                                                 \n"
    "   mu X1(b: Bool) = forall c: Bool. val(c) || X1(c); \n"
    "   mu X2 = true;                                     \n"
    "init X2;                                             \n"
    ;
  test_guard(text, "X1", "!val(c)");

  text =
    "pbes                                                             \n"
    "   mu X1(b: Bool) = val(b) || (forall c: Bool. val(c) || X1(c)); \n"
    "   mu X2 = true;                                                 \n"
    "init X2;                                                         \n"
    ;
  test_guard(text, "X1", "!val(b) && !val(c)");

  text =
    "pbes                                                                     \n"
    "   mu X1(b: Bool) = (val(b) && X2) || (forall c: Bool. val(c) || X1(c)); \n"
    "   mu X2 = true;                                                         \n"
    "init X2;                                                                 \n"
    ;
  test_guard(text, "X1", "!val(c)");

  text =
    "pbes                                                                     \n"
    "   mu X1(b: Bool) = (val(b) || X2) || (forall c: Bool. val(c) || X1(c)); \n"
    "   mu X2 = true;                                                         \n"
    "init X2;                                                                 \n"
    ;
  test_guard(text, "X1", "!val(c)");

  text =
    "pbes                                                                      \n"
    "   mu X1(b: Bool) = (!val(b) || X2) || (forall c: Bool. val(c) || X1(c)); \n"
    "   mu X2 = true;                                                          \n"
    "init X2;                                                                  \n"
    ;
  test_guard(text, "X1", "!val(c)");

  text =
    "pbes                                                                             \n"
    "   mu X0(b, c, d, e: Bool) = (val(b) || X1) && (val(c) || X2) && (val(d) || X3); \n"
    "   mu X1 = true;                                                                 \n"
    "   mu X2 = true;                                                                 \n"
    "   mu X3 = true;                                                                 \n"
    "init X2;                                                                         \n"
    ;
  test_guard(text, "X1", "!val(b)");

  text =
    "pbes                                                   \n"
    "   mu X0(b, c, d, e: Bool) = (val(b) || val(e) && X1); \n"
    "   mu X1 = true;                                       \n"
    "init X1;                                               \n"
    ;
  test_guard(text, "X1", "!val(b) && val(e)");

  text =
    "pbes                                                                     \n"
    "   mu X0(b, c, d, e: Bool) = (val(b) || val(e) && X1) && (val(c) || X2); \n"
    "   mu X1 = true;                                                         \n"
    "   mu X2 = true;                                                         \n"
    "init X1;                                                                 \n"
    ;
  test_guard(text, "X1", "!val(b) && val(e)");

  text =
    "pbes nu X1(s,d: Nat) = (!val((s == 1)) || X1(2, d)) &&\n"
    "                       (!val((s == 2)) || X2(3, d)) &&\n"
    "                       (!val((s == 3)) || X3(4, d)) &&\n"
    "                       (!val((s == 4)) || (forall e: Nat. val(!(e < 10)) || X4(5, 2 * e))) &&\n"
    "                       (!val((s == 5)) || val(d mod 2 == 0) && X5(6, d)) &&\n"
    "                       (!val((s == 6)) || X6(7, d)) &&\n"
    "                       (!val((s == 7)) || X7(1, d));\n"
    "     nu X2(s,d: Nat) = true;\n"
    "     nu X3(s,d: Nat) = true;\n"
    "     nu X4(s,d: Nat) = true;\n"
    "     nu X5(s,d: Nat) = true;\n"
    "     nu X6(s,d: Nat) = true;\n"
    "     nu X7(s,d: Nat) = true;\n"
    "init X1(0, 0);\n"
    ;
  test_guard(text, "X1", "s == 1");
  test_guard(text, "X2", "s == 2");
  test_guard(text, "X3", "s == 3");
  test_guard(text, "X4", "val(s == 4) && !val(!(e < 10))");
  test_guard(text, "X5", "val(s == 5) && val(d mod 2 == 0)");
  test_guard(text, "X6", "s == 6");
  test_guard(text, "X7", "s == 7");
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT_DEBUG(argc, argv)

  log::mcrl2_logger::set_reporting_level(log::debug, "stategraph");
  test_guard();

  return 0;
}
