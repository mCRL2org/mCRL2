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

// #define MCRL2_PBES_STATEGRAPH_CHECK_GUARDS

#include "mcrl2/pbes/detail/guard_traverser.h"
#include "mcrl2/pbes/detail/stategraph_global_reset_variables.h"
#include "mcrl2/pbes/detail/stategraph_local_reset_variables.h"
#include "mcrl2/pbes/find.h"
#include "mcrl2/pbes/significant_variables.h"
#include "mcrl2/pbes/txt2pbes.h"
#include "mcrl2/utilities/detail/split.h"
#include <boost/test/minimal.hpp>
#include <iostream>
#include <set>
#include <utility>

using namespace mcrl2;
using namespace pbes_system;

// prints the names of the variables (sorted alphabetically)
std::string print_set(const std::set<data::variable>& variables)
{
  std::ostringstream out;
  std::set<std::string> s;
  for (const data::variable& v: variables)
  {
    s.insert(std::string(v.name()));
  }
  out << "{";
  for (auto i = s.begin(); i != s.end(); ++i)
  {
    if (i != s.begin())
    {
      out << ", ";
    }
    out << *i;
  }
  out << "}";
  return out.str();
}

propositional_variable_instantiation propvar(const std::string& name)
{
  return propositional_variable_instantiation(core::identifier_string(name), data::data_expression_list());
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

void test_significant_variables(const pbes_expression& x, const std::string& expected_result)
{
  std::set<data::variable> v = significant_variables(x);
  std::string result = print_set(v);
  check_result(pbes_system::pp(x), result, expected_result, "significant_variables");
}

void test_significant_variables()
{
  std::string text =
    "pbes                                                    \n"
    "   mu X0(b: Bool) = val(b);                             \n"
    "   mu X1 = true;                                        \n"
    "   mu X2(b: Bool) = val(b) && forall b: Bool. val(b);   \n"
    "   mu X3(b: Bool) = val(b) && forall c: Bool. val(c);   \n"
    "init X1;                                                \n"
    ;

  bool normalize = false;
  pbes p = txt2pbes(text, normalize);
  const std::vector<pbes_equation>& eqn = p.equations();
  test_significant_variables(eqn[0].formula(), "{b}");
  test_significant_variables(eqn[1].formula(), "{}");
  test_significant_variables(eqn[2].formula(), "{b}");
  test_significant_variables(eqn[2].formula(), "{b}");
}

// find propositional variable instantiation with the given name
propositional_variable_instantiation find_propvar(const std::string& name, const pbes_expression& x)
{
  for (const propositional_variable_instantiation& v: find_propositional_variable_instantiations(x))
  {
    if (v.name() == core::identifier_string(name))
    {
      return v;
    }
  }
  throw mcrl2::runtime_error("propvar not found!");
  return propositional_variable_instantiation();
}

void test_guard(const std::string& pbesspec, const std::string& X, const std::string& expected_result)
{
  bool normalize = false;
  pbes p = txt2pbes(pbesspec, normalize);
  pbes_expression x1 = p.equations().front().formula();
  propositional_variable_instantiation X1 = find_propvar(X, x1);
  simplify_data_rewriter<data::rewriter> R(p.data());

  detail::guard_traverser f(p.data());
  f.apply(x1);
  BOOST_CHECK(f.expression_stack.back().check_guards(x1, R));

//  pbes_expression g = detail::guard(X1, x1);
//  std::string result = pbes_system::pp(g);
//  check_result(X, result, expected_result, "");
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

void test_local_stategraph()
{
  std::string text;
  bool normalize = false;
  pbes p;

  text =
    "sort D = struct d1 | d2;\n"
    "\n"
    "pbes nu X(s: Nat, d: D)   = forall v: D. forall e1: D. val(!(e1 == v)) || val(!(s == 1)) || Y(2, e1, v);\n"
    "     nu Y(s: Nat, d,v: D) = val(!(s == 2)) || Y(3, d, v);\n"
    "\n"
    "init X(1, d1);\n"
   ;
  p = txt2pbes(text, normalize);
  // This fails because no suitable must graph is found
  // pbes_system::detail::local_reset_variables_algorithm(p).run();

  text =
    "pbes\n"
    "nu X(c:Pos, d:Nat) = val(c == 1) => Y(1,d+1) && (val(c == 1) => X(1,d+2));\n"
    "nu Y(c:Pos, d:Nat) = (val(c == 1) => Y(c,d+1)) && (val(d > 0));\n"
    "init X(1,0);\n"
    ;
  p = txt2pbes(text, normalize);
  pbesstategraph_options options;
  options.use_global_variant = false;
  pbes_system::detail::local_reset_variables_algorithm(p, options).run();
}

// Test cases provided by Tim Willemse, 28-06-2013
void test_cfp()
{
  std::string text =
    "----------                                                                            \n"
    "pbes                                                                                  \n"
    "nu X(i,j,n,m:Nat) =                                                                   \n"
    "(val(i == 1) => X(0,j+1,n+1,m+1)) &&                                                  \n"
    "(val(i == 0) => X(i+1,j+1,n+1,m+1));                                                  \n"
    "                                                                                      \n"
    "init                                                                                  \n"
    "X(0,0,0,0);                                                                           \n"
    "                                                                                      \n"
    "expected_result                                                                       \n"
    "{(X, i)}                                                                              \n"
    "----------                                                                            \n"
    "pbes                                                                                  \n"
    "nu X(i,j,n,m:Nat) =                                                                   \n"
    "(val(i == 1) => Y(0,j,n+1,m+1)) &&                                                    \n"
    "(val(i == 0) => X(i+1,j+1,n+1,m+1));                                                  \n"
    "                                                                                      \n"
    "nu Y(i,j,n,m:Nat) =                                                                   \n"
    "(val(i == 1) => X(0,j,n,m) ) &&                                                       \n"
    "(val(i == 0) => Y(1,n,j,m) );                                                         \n"
    "                                                                                      \n"
    "init                                                                                  \n"
    "X(0,0,0,0);                                                                           \n"
    "                                                                                      \n"
    "expected_result                                                                       \n"
    "{(X, i)}, {(Y, i)}                                                                    \n"
    "----------                                                                            \n"
    "pbes                                                                                  \n"
    "nu X(i,j,n,m:Nat) =                                                                   \n"
    "(val(i == 1) => Y(0,j,n+1,m+1)) &&                                                    \n"
    "(val(i == 0) => X(i+1,j+1,n+1,m+1));                                                  \n"
    "                                                                                      \n"
    "nu Y(i,j,n,m:Nat) =                                                                   \n"
    "(val(i == 1) => X(0,j,n,m) ) &&                                                       \n"
    "(val(i == 0) => Y(1,j,n,m) );                                                         \n"
    "                                                                                      \n"
    "init                                                                                  \n"
    "X(0,0,0,0);                                                                           \n"
    "                                                                                      \n"
    "expected_result                                                                       \n"
    "{(X, i)}, {(Y, i)}                                                                    \n"
    "----------                                                                            \n"
    "pbes                                                                                  \n"
    "nu X(i,j,n,m:Nat) =                                                                   \n"
    "(val(i == 1) => Y(i,j,n+1,m+1)) &&                                                    \n"
    "(val(i == 0) => X(i+1,j+1,n+1,m+1));                                                  \n"
    "                                                                                      \n"
    "nu Y(i,j,n,m:Nat) =                                                                   \n"
    "(val(i == 1) => X(0,j,n,m) ) &&                                                       \n"
    "(val(i == 0) => Y(1,j,n,m) );                                                         \n"
    "                                                                                      \n"
    "init                                                                                  \n"
    "X(0,0,0,0);                                                                           \n"
    "                                                                                      \n"
    "expected_result                                                                       \n"
    "{(X, i), (Y, i)}                                                                      \n"
    "----------                                                                            \n"
    "pbes                                                                                  \n"
    "nu X(i,j,n,m:Nat) =                                                                   \n"
    "(val(i == 1) => Y(i,j,n+1,m+1)) &&                                                    \n"
    "(val(i == 0) => X(i+1,j+1,n+1,m+1));                                                  \n"
    "                                                                                      \n"
    "nu Y(i,j,n,m:Nat) =                                                                   \n"
    "(val(i == 1) => X(0,j,n,m) ) &&                                                       \n"
    "(val(i == 0) => Y(1,j,n,m) ) &&                                                       \n"
    "(val(i == 1) => Y(0,j,n,m) );                                                         \n"
    "                                                                                      \n"
    "init                                                                                  \n"
    "X(0,0,0,0);                                                                           \n"
    "                                                                                      \n"
    "expected_result                                                                       \n"
    "{(X, i), (Y, i)}                                                                      \n"
    "----------                                                                            \n"
    "pbes                                                                                  \n"
    "nu X(i,j,n,m:Nat) =                                                                   \n"
    "(val(i == 1) => Y(i,j,n+1,m+1)) &&                                                    \n"
    "(val(i == 0) => X(i+1,j+1,n+1,m+1));                                                  \n"
    "                                                                                      \n"
    "nu Y(i,j,n,m:Nat) =                                                                   \n"
    "(val(i == 1) => X(0,j,n,m) ) &&                                                       \n"
    "(val(i == 0) => Y(1,j,n,m) ) &&                                                       \n"
    "(val(i == 1) => Y(0,j,n,m) );                                                         \n"
    "                                                                                      \n"
    "init                                                                                  \n"
    "X(0,0,0,0);                                                                           \n"
    "                                                                                      \n"
    "expected_result                                                                       \n"
    "{(X, i), (Y, i)}                                                                      \n"
    "----------                                                                            \n"
    "pbes                                                                                  \n"
    "nu X(i,j,n,m:Nat) =                                                                   \n"
    "(val(i == 1) => Y(i,j,n+1,m+1)) &&                                                    \n"
    "(val(i == 0) => X(i+1,j+1,n+1,m+1));                                                  \n"
    "                                                                                      \n"
    "nu Y(i,j,n,m:Nat) =                                                                   \n"
    "(val(i == 1) => X(j,j,n,m) ) &&                                                       \n"
    "(val(i == 0) => X(i,j,n,m) ) &&                                                       \n"
    "(val(i == 0) => Y(1,j,n,m) ) &&                                                       \n"
    "(val(i == 1) => Y(0,j,n,m) );                                                         \n"
    "                                                                                      \n"
    "init                                                                                  \n"
    "X(0,0,0,0);                                                                           \n"
    "                                                                                      \n"
    "expected_result                                                                       \n"
    "{}                                                                                    \n"
    "----------                                                                            \n"
    "pbes                                                                                  \n"
    "nu X(i,j,n,m:Nat) =                                                                   \n"
    "(val(i == 1) => Y(i,j,n+1,m+1)) &&                                                    \n"
    "(val(i == 0) => X(i+1,j+1,n+1,m+1));                                                  \n"
    "                                                                                      \n"
    "nu Y(i,j,n,m:Nat) =                                                                   \n"
    "(val(i == 1) => X(j,j,n,m) ) &&                                                       \n"
    "(val(i == 0) => Z(i,j,n,m) ) &&                                                       \n"
    "(val(i == 0) => Y(1,j,n,m) ) &&                                                       \n"
    "(val(i == 1) => Y(0,j,n,m) );                                                         \n"
    "                                                                                      \n"
    "nu Z(i,j,n,m:Nat) =                                                                   \n"
    "(val(i== 0) => X(i,j,n,m)) &&                                                         \n"
    "(val(i== 1) => Z(i,j,n,m));                                                           \n"
    "                                                                                      \n"
    "init                                                                                  \n"
    "X(0,0,0,0);                                                                           \n"
    "                                                                                      \n"
    "expected_result                                                                       \n"
    "{}                                                                                    \n"
    "----------                                                                            \n"
    "pbes                                                                                  \n"
    "nu X(i,j,n:Nat) =                                                                     \n"
    "(val(i == 1) => Y(i,j,n+1)) &&                                                        \n"
    "(val(i == 0) => X(i+1,j+1,n+1));                                                      \n"
    "                                                                                      \n"
    "nu Y(i,j,n:Nat) =                                                                     \n"
    "(val(i == 1) => X(j,j,n) )                                                            \n"
    ";                                                                                     \n"
    "                                                                                      \n"
    "init                                                                                  \n"
    "X(0,0,0);                                                                             \n"
    "                                                                                      \n"
    "expected_result                                                                       \n"
    "{(Y, i)}                                                                              \n"
    "----------                                                                            \n"
    "pbes                                                                                  \n"
    "nu X(i,j,n:Nat) =                                                                     \n"
    "(val(i == 1) => Y(i,j,n+1)) &&                                                        \n"
    "(val(i == 0) => X(i+1,j+1,n+1));                                                      \n"
    "                                                                                      \n"
    "nu Y(i,j,n:Nat) =                                                                     \n"
    "(val(i == 1) => X(i,j,n) ) &&                                                         \n"
    "(val(i == 1) => Z(i,j,n) )                                                            \n"
    ";                                                                                     \n"
    "                                                                                      \n"
    "nu Z(i,j,n:Nat) =                                                                     \n"
    "(val(j == 1) => X(i,j,n)) &&                                                          \n"
    "(val(i == 1) => Z(0,j,n))                                                             \n"
    ";                                                                                     \n"
    "                                                                                      \n"
    "init                                                                                  \n"
    "X(0,0,0);                                                                             \n"
    "                                                                                      \n"
    "expected_result                                                                       \n"
    "{(X, i), (Y, i), (Z, i)}                                                              \n"
    "----------                                                                            \n"
    "pbes                                                                                  \n"
    "nu X(i,j,n:Nat) =                                                                     \n"
    "(val(i == 1) => Y(i,j,n+1)) &&                                                        \n"
    "(val(i == 0) => X(i+1,j+1,n+1));                                                      \n"
    "                                                                                      \n"
    "nu Y(i,j,n:Nat) =                                                                     \n"
    "(val(i == 1) => X(i,j,n) ) &&                                                         \n"
    "(val(i == 1) => Z(i,j,n) )                                                            \n"
    ";                                                                                     \n"
    "                                                                                      \n"
    "nu Z(i,j,n:Nat) =                                                                     \n"
    "(val(j == 1) => X(j,i,n)) &&                                                          \n"
    "(val(i == 1) => Z(0,j,n))                                                             \n"
    ";                                                                                     \n"
    "                                                                                      \n"
    "init                                                                                  \n"
    "X(0,0,0);                                                                             \n"
    "                                                                                      \n"
    "expected_result                                                                       \n"
    "{}                                                                                    \n"
    "----------                                                                            \n"
    "pbes                                                                                  \n"
    "nu X(i,j,n:Nat) =                                                                     \n"
    "(val(i == 1) => Y(i,j,n+1)) &&                                                        \n"
    "(val(i == 0) => X(i+1,i+1,n+1));                                                      \n"
    "                                                                                      \n"
    "nu Y(i,j,n:Nat) =                                                                     \n"
    "(val(i == 1) => X(i,j,n) ) &&                                                         \n"
    "(val(i == 1) => Z(i,j,n) )                                                            \n"
    ";                                                                                     \n"
    "                                                                                      \n"
    "nu Z(i,j,n:Nat) =                                                                     \n"
    "(val(j == 1) => X(i,j,n)) &&                                                          \n"
    "(val(i == 1) => Z(0,j,n))                                                             \n"
    ";                                                                                     \n"
    "                                                                                      \n"
    "init                                                                                  \n"
    "X(0,0,0);                                                                             \n"
    "                                                                                      \n"
    "expected_result                                                                       \n"
    "{(X, i), (Y, i), (Z, i)}                                                              \n"
    "----------                                                                            \n"
    "pbes                                                                                  \n"
    "nu X(i,j,n:Nat) =                                                                     \n"
    "(val(i == 1) => Y(i,j,n+1)) &&                                                        \n"
    "(val(i == 0) => X(1,1,n+1));                                                          \n"
    "                                                                                      \n"
    "nu Y(i,j,n:Nat) =                                                                     \n"
    "(val(i == 1) => X(i,j,n) ) &&                                                         \n"
    "(val(i == 1) => Z(i,j,n) )                                                            \n"
    ";                                                                                     \n"
    "                                                                                      \n"
    "nu Z(i,j,n:Nat) =                                                                     \n"
    "(val(j == 1) => X(i,j,n)) &&                                                          \n"
    "(val(i == 1) => Z(0,j,n))                                                             \n"
    ";                                                                                     \n"
    "                                                                                      \n"
    "init                                                                                  \n"
    "X(0,0,0);                                                                             \n"
    "                                                                                      \n"
    "expected_result                                                                       \n"
    "{(X, i), (Y, i), (Z, i)}                                                              \n"
    "----------                                                                            \n"
    "pbes                                                                                  \n"
    "nu X(i,j,n:Nat) =                                                                     \n"
    "(val(i == 0) => Y(i,j,n+1));                                                          \n"
    "                                                                                      \n"
    "nu Y(i,j,n:Nat) =                                                                     \n"
    "(val(i==0) => Z(i,j,n+1));                                                            \n"
    "                                                                                      \n"
    "nu Z(i,j,n:Nat) =                                                                     \n"
    "(val(i==0) => Z(1,j,n)) &&                                                            \n"
    "(val(i==1) => (X(i,j,n) || Y(i,j,n)));                                                \n"
    "                                                                                      \n"
    "init                                                                                  \n"
    "X(0,0,0);                                                                             \n"
    "                                                                                      \n"
    "expected_result                                                                       \n"
    "{(X, i), (Y, i), (Z, i)}, {(X, j), (Y, j), (Z, j)}                                    \n"
    "----------                                                                            \n"
    "pbes                                                                                  \n"
    "nu X(i,n:Nat) =                                                                       \n"
    "(val(i == 0) => Y(i,n+1));                                                            \n"
    "                                                                                      \n"
    "nu Y(i,n:Nat) =                                                                       \n"
    "(val(i==0) => Z(i,n+1));                                                              \n"
    "                                                                                      \n"
    "nu Z(i,n:Nat) =                                                                       \n"
    "(val(i==0) => X(i,n));                                                                \n"
    "                                                                                      \n"
    "init                                                                                  \n"
    "X(0,0);                                                                               \n"
    "                                                                                      \n"
    "expected_result                                                                       \n"
    "{(X, i), (Y, i), (Z, i)}                                                              \n"
    "----------                                                                            \n"
    "pbes                                                                                  \n"
    "nu X(n:Nat) = Y(n+1);                                                                 \n"
    "nu Y(n:Nat) = Z(n+1);                                                                 \n"
    "nu Z(n:Nat) =  X(n);                                                                  \n"
    "                                                                                      \n"
    "init X(0);                                                                            \n"
    "                                                                                      \n"
    "expected_result                                                                       \n"
    "{}                                                                                    \n"
    "----------                                                                            \n"
    "pbes                                                                                  \n"
    "nu X(i,n:Nat) =                                                                       \n"
    "(val(i == 0) => Y(i,n+1));                                                            \n"
    "                                                                                      \n"
    "nu Y(i,n:Nat) =                                                                       \n"
    "(val(i==0) => Z(i,0,n+1)) &&                                                          \n"
    "(val(i==0) => X(i,n+1));                                                              \n"
    "                                                                                      \n"
    "nu Z(i,j,n:Nat) =                                                                     \n"
    "(val(i==0 && n == 1) => Z(1,j,n)) &&                                                  \n"
    "(val(i==1) => X(n,j));                                                                \n"
    "                                                                                      \n"
    "init                                                                                  \n"
    "X(0,0);                                                                               \n"
    "                                                                                      \n"
    "expected_result                                                                       \n"
    "{(Z, i)}                                                                              \n"
    "----------                                                                            \n"
    "pbes                                                                                  \n"
    "nu X(i,n:Nat) =                                                                       \n"
    "(val(i == 0) => Y(i,n));                                                              \n"
    "                                                                                      \n"
    "nu Y(i,n:Nat) =                                                                       \n"
    "(val(i==0) => Z(i,0)) &&                                                              \n"
    "(val(i==0) => X(i,2));                                                                \n"
    "                                                                                      \n"
    "nu Z(i,n:Nat) =                                                                       \n"
    "(val(i==0 && n == 1) => Z(1,n)) &&                                                    \n"
    "(val(i==1) => X(n,i));                                                                \n"
    "                                                                                      \n"
    "init                                                                                  \n"
    "X(0,0);                                                                               \n"
    "                                                                                      \n"
    "expected_result                                                                       \n"
    "{(X, n), (Y, n), (Z, i)}                                                              \n"
    "----------                                                                            \n"
    "pbes                                                                                  \n"
    "nu X(i,n:Nat) =                                                                       \n"
    "(val(i == 0) => Y(i,n));                                                              \n"
    "                                                                                      \n"
    "nu Y(i,n:Nat) =                                                                       \n"
    "(val(i==0) => X(i,0)) &&                                                              \n"
    "(val(i==1) => X(n,2));                                                                \n"
    "                                                                                      \n"
    "init                                                                                  \n"
    "X(0,0);                                                                               \n"
    "                                                                                      \n"
    "expected_result                                                                       \n"
    "{}                                                                                    \n"
    "----------                                                                            \n"
    "pbes                                                                                  \n"
    "nu X(i,n:Nat) =                                                                       \n"
    "(val(i == 0) => Y(i,n+1));                                                            \n"
    "                                                                                      \n"
    "nu Y(i,n:Nat) =                                                                       \n"
    "(val(i==0) => X(i,0)) &&                                                              \n"
    "(val(i==1) => X(n,2));                                                                \n"
    "                                                                                      \n"
    "init                                                                                  \n"
    "X(0,0);                                                                               \n"
    "                                                                                      \n"
    "expected_result                                                                       \n"
    "{}                                                                                    \n"
    "----------                                                                            \n"
    "pbes                                                                                  \n"
    "nu X(n,i:Nat) = Y(n+1,0);                                                             \n"
    "                                                                                      \n"
    "nu Y(i,n:Nat) = X(0,n);                                                               \n"
    "                                                                                      \n"
    "init                                                                                  \n"
    "X(0,0);                                                                               \n"
    "                                                                                      \n"
    "expected_result                                                                       \n"
    "{(X, i), (Y, n)}                                                                      \n"
    "----------                                                                            \n"
    "pbes                                                                                  \n"
    "nu X(i,n:Nat) = forall j:Nat.(val(i == 0) => Y(i,n+j));                               \n"
    "                                                                                      \n"
    "nu Y(i,n:Nat) = (val(i==1) => X(n,2));                                                \n"
    "                                                                                      \n"
    "                                                                                      \n"
    "init                                                                                  \n"
    "X(0,0);                                                                               \n"
    "                                                                                      \n"
    "expected_result                                                                       \n"
    "{(Y, i)}                                                                              \n"
    "----------                                                                            \n"
    "%                                                                                     \n"
    "% The below PBES should only have (Y,i) as a control flow parameter                   \n"
    "%                                                                                     \n"
    "                                                                                      \n"
    "pbes                                                                                  \n"
    "nu X(n:Nat) = forall j:Nat. Y(0,n+j);                                                 \n"
    "                                                                                      \n"
    "nu Y(i,n:Nat) = (val(i==1) => X(2));                                                  \n"
    "                                                                                      \n"
    "init                                                                                  \n"
    "X(0);                                                                                 \n"
    "                                                                                      \n"
    "expected_result                                                                       \n"
    "{(Y, i)}                                                                              \n"
    "----------                                                                            \n"
    "pbes                                                                                  \n"
    "nu X(n:Nat) = forall j:Nat. Y(0,j);                                                   \n"
    "                                                                                      \n"
    "nu Y(i,n:Nat) = (val(i==1) => X(2));                                                  \n"
    "                                                                                      \n"
    "                                                                                      \n"
    "init                                                                                  \n"
    "X(0);                                                                                 \n"
    "                                                                                      \n"
    "expected_result                                                                       \n"
    "{(X, n)}, {(Y, i)}                                                                    \n"
    "----------                                                                            \n"
    "pbes                                                                                  \n"
    "                                                                                      \n"
    "nu X(i,j:Nat) =                                                                       \n"
    "(val(i==0) => X(0,i)) &&                                                              \n"
    "(val(j==1) => X(j,1));                                                                \n"
    "                                                                                      \n"
    "init X(0,0);                                                                          \n"
    "                                                                                      \n"
    "expected_result                                                                       \n"
    "{}                                                                                    \n"
    "----------                                                                            \n"
    "%This solution assumes strict separation between CFPs and DPs, if this assumption is  \n"
    "%dropped, the following is the expected solution                                      \n"
    "%{(X, n), (Y, n)}                                                                     \n"
    "pbes                                                                                  \n"
    "nu X(i,n:Nat) =                                                                       \n"
    "(val(i == 0) => Y(i,n));                                                              \n"
    "                                                                                      \n"
    "nu Y(i,n:Nat) =                                                                       \n"
    "(val(i==0) => X(i,0)) &&                                                              \n"
    "(val(i==1) => X(n+1,2));                                                              \n"
    "                                                                                      \n"
    "init                                                                                  \n"
    "X(0,0);                                                                               \n"
    "                                                                                      \n"
    "expected_result                                                                       \n"
    "{}                                                                                    \n"
    ;

  std::vector<std::string> test_cases = utilities::detail::split_text(text, "----------");
  for (const std::string& s: test_cases)
  {
    std::vector<std::string> keywords;
    keywords.push_back("pbes");
    keywords.push_back("expected_result");
    std::map<std::string, std::string> test_case = utilities::detail::split_text_using_keywords(s, keywords);
    pbes p = txt2pbes(test_case["pbes"], false);
    std::string expected_result = boost::trim_copy(test_case["expected_result"]);
    if (!expected_result.empty())
    {
      expected_result = boost::trim_copy(expected_result.substr(15));
    }
    std::vector<std::string> lines = utilities::detail::nonempty_lines(expected_result);
    std::set<std::string> lineset(lines.begin(), lines.end());
    std::string expected = utilities::string_join(lineset, ", ");
    pbesstategraph_options options;
    pbes_system::detail::stategraph_algorithm algorithm(p, options);
    algorithm.run();
    //std::string result = utilities::string_join(print_connected_components(algorithm.connected_components(), algorithm), ", ");
    //
    //if (result != expected)
    //{
    //  BOOST_CHECK(result == expected);
    //  std::cout << "--- Control flow test failed ---" << std::endl;
    //  std::cout << test_case["pbes"] << std::endl;
    //  std::cout << "result               = " << result   << std::endl;
    //  std::cout << "expected result      = " << expected << std::endl;
    //}
  }
}

int test_main(int, char**)
{
  log::mcrl2_logger::set_reporting_level(log::debug, "stategraph");
  test_guard();
  test_significant_variables();
  test_local_stategraph();
  test_cfp(); // This does not longer work since the computation of components has been changed.

  return 0;
}
