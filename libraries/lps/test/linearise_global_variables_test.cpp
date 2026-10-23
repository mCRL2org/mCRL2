// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file linearise_global_variables_test.cpp
/// \brief Test that global variables occurring in the linearised process are declared.

#define BOOST_TEST_MODULE linearise_global_variables_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/lps/find.h"
#include "mcrl2/lps/is_well_typed.h"
#include "mcrl2/lps/linearise.h"

using namespace mcrl2;
using namespace mcrl2::lps;

// Linearise spec and check that every free variable of the result is declared as a global variable.
static void check_global_variables_declared(const std::string& spec, const t_lin_options& options)
{
  const stochastic_specification result = linearise(spec, options);
  std::set<data::variable> undeclared = lps::find_free_variables(result);
  for (const data::variable& v: result.global_variables())
  {
    undeclared.erase(v);
  }
  BOOST_CHECK_MESSAGE(undeclared.empty(),
    "undeclared free variables " << core::detail::print_set(undeclared) << " in\n"
                                 << lps::pp(result));
  BOOST_CHECK(lps::detail::is_well_typed(result));
}

static void check_global_variables_declared(const std::string& spec)
{
  t_lin_options options;
  check_global_variables_declared(spec, options);
  // Corresponds to mcrl22lps --no-rewrite, which also disables constelm.
  options.norewrite = true;
  options.do_not_apply_constelm = true;
  check_global_variables_declared(spec, options);
}

// The global variable dc occurs in the condition of a summand, outside a binder.
BOOST_AUTO_TEST_CASE(global_variable_in_condition)
{
  const std::string spec = "act a;\n"
                           "glob dc: Nat;\n"
                           "proc P(n: Nat) = (n < dc) -> a . P(n + 1);\n"
                           "init P(0);\n";
  check_global_variables_declared(spec);
}

// The global variable dc occurs in an action argument, outside a binder.
BOOST_AUTO_TEST_CASE(global_variable_in_action)
{
  const std::string spec = "act a: Nat;\n"
                           "glob dc: Nat;\n"
                           "proc P = a(dc + 1) . P;\n"
                           "init P;\n";
  check_global_variables_declared(spec);
}

// The global variable dc occurs in the next state of a summand, outside a binder.
BOOST_AUTO_TEST_CASE(global_variable_in_next_state)
{
  const std::string spec = "act a: Nat;\n"
                           "glob dc: Nat;\n"
                           "proc P(n: Nat) = a(n) . P(n + dc);\n"
                           "init P(0);\n";
  check_global_variables_declared(spec);
}

// The global variable dc occurs in the initial state, outside a binder.
BOOST_AUTO_TEST_CASE(global_variable_in_initial_state)
{
  const std::string spec = "act a: Nat;\n"
                           "glob dc: Nat;\n"
                           "proc P(n: Nat) = a(n) . P(n + 1);\n"
                           "init P(dc);\n";
  check_global_variables_declared(spec);
}

// The global variable dc only occurs in the body of a lambda abstraction.
BOOST_AUTO_TEST_CASE(global_variable_in_lambda)
{
  const std::string spec = "act a: Nat -> Nat;\n"
                           "glob dc: Nat;\n"
                           "proc P = a(lambda x: Nat. x + dc) . P;\n"
                           "init P;\n";
  check_global_variables_declared(spec);
}

// The global variable dc only occurs in a where clause. With rewriting the where clause is
// eliminated, so the problem only shows with norewrite (and without constelm, which rewrites).
BOOST_AUTO_TEST_CASE(global_variable_in_where_clause)
{
  const std::string spec = "act a: Nat;\n"
                           "glob dc: Nat;\n"
                           "proc P = a(y whr y = dc end) . P;\n"
                           "init P;\n";
  check_global_variables_declared(spec);
}

// The global variable dc only occurs in the condition of a summand, inside a quantifier.
// The quantifier ranges over a function sort to prevent the rewriter from (slowly) enumerating it.
BOOST_AUTO_TEST_CASE(global_variable_in_quantifier)
{
  const std::string spec = "act a;\n"
                           "glob dc: Nat;\n"
                           "proc P = (exists f: Nat -> Bool. f(dc)) -> a . P;\n"
                           "init P;\n";
  check_global_variables_declared(spec);
}
