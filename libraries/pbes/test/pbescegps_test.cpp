// Author(s): Jore Booy
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbescegps_test.cpp
/// \brief Tests for the pbescegps tool.

#include "mcrl2/pbes/tools/pbescegps.h"
#define BOOST_TEST_MODULE pbescegps_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/pbes/detail/pbescegps_utilities.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/txt2pbes.h"

using namespace mcrl2;
using namespace pbes_system;
using namespace pbes_system::detail;

static bool run_cegps(
  const std::string& text,
  pbescegps_options options,
  abstract_param_state& final_state)
{
  pbes p = txt2pbes(text, false);
  pbescegps_iterator iterator;
  return iterator.run_cegps_algorithm(p, options, final_state);
}

// Returns the set of parameter names still abstracted for the given equation.
static std::set<std::string> abstracted_names(
  const abstract_param_state& state,
  const std::string& eq_name)
{
  std::set<std::string> names;
  auto it = state.W.find(core::identifier_string(eq_name));
  if (it != state.W.end())
  {
    for (const data::variable& v: it->second)
    {
      names.insert(std::string(v.name()));
    }
  }
  return names;
}

// Convenience overload that drops the final state.
static bool run_cegps(
  const std::string& text,
  pbescegps_options options,
  const std::string& eq_name,
  std::set<std::string>& result_names)
{
  abstract_param_state final_state;
  bool r = run_cegps(text, options, final_state);
  result_names = abstracted_names(final_state, eq_name);
  return r;
}

static pbescegps_options default_options()
{
  pbescegps_options opts;
  opts.rewrite_strategy = data::rewrite_strategy::jitty;
  opts.var_choice = var_choice_strategy::lhs;
  return opts;
}

// nu equation with two parameters, no transitions.
BOOST_AUTO_TEST_CASE(test_no_transitions)
{
  std::string text =
    "pbes nu X(a: Bool, b: Bool) = val(a);"
    "init X(false, true);";
  std::set<std::string> abstracted;
  BOOST_CHECK(!run_cegps(text, default_options(), "X", abstracted));
  BOOST_CHECK((abstracted == std::set<std::string>{}));
}

// mu equation with two parameters, no transitions.
BOOST_AUTO_TEST_CASE(test_mu_equation)
{
  std::string text =
    "pbes mu X(a: Bool, b: Bool) = val(a && b);"
    "init X(true, true);";
  std::set<std::string> abstracted;
  BOOST_CHECK(run_cegps(text, default_options(), "X", abstracted));
  BOOST_CHECK((abstracted == std::set<std::string>{}));
}

// nu X with a self-transition on a only.
// X(a, b) = (val(a) || X(!a, b))
// X(false, true) = X(true, true) = true.
BOOST_AUTO_TEST_CASE(test_self_transition)
{
  std::string text =
    "pbes nu X(a: Bool, b: Bool) = (val(a) || X(!a, b));"
    "init X(false, true);";
  std::set<std::string> abstracted;
  BOOST_CHECK(run_cegps(text, default_options(), "X", abstracted));
  BOOST_CHECK((abstracted == std::set<std::string>{"a", "b"}));
}

// Two-equation system: nu Z calls mu X0.
// Z(false, true) = X0(false, true) && ... = false.
BOOST_AUTO_TEST_CASE(test_two_equations)
{
  std::string text =
    "pbes nu Z(a: Bool, b: Bool) = X0(a, b) && (val(a) || Z(!a, b));"
    "mu X0(a: Bool, b: Bool) = val(a && b);"
    "init Z(false, true);";
  abstract_param_state final_state;
  BOOST_CHECK(!run_cegps(text, default_options(), final_state));
  BOOST_CHECK((abstracted_names(final_state, "Z") == std::set<std::string>{"b"}));
  BOOST_CHECK((abstracted_names(final_state, "X0") == std::set<std::string>{"b"}));
}

// Test with --var-choice=ruling.
BOOST_AUTO_TEST_CASE(test_var_choice_ruling)
{
  std::string text =
    "pbes nu X(a: Bool, b: Bool) = (val(a) || X(!a, b));"
    "init X(false, true);";
  auto opts = default_options();
  opts.var_choice = var_choice_strategy::ruling;
  std::set<std::string> abstracted;
  BOOST_CHECK(run_cegps(text, opts, "X", abstracted));
}

// Test ruling + rules_ideal flag.
BOOST_AUTO_TEST_CASE(test_ruling_with_rules_ideal)
{
  std::string text =
    "pbes nu X(a: Bool, b: Bool) = (val(a) || X(!a, b));"
    "init X(false, true);";
  auto opts = default_options();
  opts.var_choice = var_choice_strategy::ruling;
  opts.rules_ideal = true;
  std::set<std::string> abstracted;
  BOOST_CHECK(run_cegps(text, opts, "X", abstracted));
}

// Test with --var-choice=count.
BOOST_AUTO_TEST_CASE(test_var_choice_count)
{
  std::string text =
    "pbes nu X(a: Bool, b: Bool) = (val(a) || X(!a, b));"
    "init X(false, true);";
  auto opts = default_options();
  opts.var_choice = var_choice_strategy::count;
  std::set<std::string> abstracted;
  BOOST_CHECK(run_cegps(text, opts, "X", abstracted));
}

// Test with --var-choice=rhs.
BOOST_AUTO_TEST_CASE(test_var_choice_rhs)
{
  std::string text =
    "pbes nu X(a: Bool, b: Bool) = (val(a) || X(!a, b));"
    "init X(false, true);";
  auto opts = default_options();
  opts.var_choice = var_choice_strategy::rhs;
  std::set<std::string> abstracted;
  BOOST_CHECK(run_cegps(text, opts, "X", abstracted));
  BOOST_CHECK((abstracted == std::set<std::string>{"a", "b"}));
}

// Test with --var-choice=all.
BOOST_AUTO_TEST_CASE(test_var_choice_all)
{
  std::string text =
    "pbes nu X(a: Bool, b: Bool) = (val(a) || X(!a, b));"
    "init X(false, true);";
  auto opts = default_options();
  opts.var_choice = var_choice_strategy::all;
  std::set<std::string> abstracted;
  BOOST_CHECK(run_cegps(text, opts, "X", abstracted));
  BOOST_CHECK((abstracted == std::set<std::string>{"a", "b"}));
}

// Larger system: nu Z with multiple guarded self-transitions.
// Z(false, red) = Z(true, green) = Z(true, red) = true.
BOOST_AUTO_TEST_CASE(test_multiple_guarded_transitions)
{
  std::string text =
    "sort Colour = struct red | green;"
    ""
    "pbes nu Z(a: Bool, c: Colour) ="
    "(val(a) || Z(!a, if(a, green, c))) &&"
    "(val(!(c == green)) || Z(a, red));"
    "init Z(false, red);";
  std::set<std::string> abstracted;
  BOOST_CHECK(run_cegps(text, default_options(), "Z", abstracted));
  BOOST_CHECK((abstracted == std::set<std::string>{"a", "c"}));
}

// Test ruling relation: guard(a) changes a and c. a rules c.
BOOST_AUTO_TEST_CASE(test_ruling_relation_mutual)
{
  std::string text =
    "sort Colour = struct red | green;"
    ""
    "pbes nu Z(a: Bool, c: Colour) ="
    "(val(a) || Z(!a, if(a, green, c)));"
    "init Z(false, red);";
  auto opts = default_options();
  opts.var_choice = var_choice_strategy::ruling;
  std::set<std::string> abstracted;
  BOOST_CHECK(run_cegps(text, opts, "Z", abstracted));
  BOOST_CHECK((abstracted == std::set<std::string>{"a", "c"}));
}

// Two equations with overlapping parameters.
// Y(a, b) = X(a, b) && (val(a) || Y(!a, b))
// X(a, b) = val(b)
// Y(false, false) = false.
BOOST_AUTO_TEST_CASE(test_two_equations_shared_params)
{
  std::string text =
    "pbes nu Y(a: Bool, b: Bool) = X(a, b) && (val(a) || Y(!a, b));"
    "mu X(a: Bool, b: Bool) = val(b);"
    "init Y(false, false);";
  abstract_param_state final_state;
  BOOST_CHECK(!run_cegps(text, default_options(), final_state));
  BOOST_CHECK((abstracted_names(final_state, "Y") == std::set<std::string>{}));
  BOOST_CHECK((abstracted_names(final_state, "X") == std::set<std::string>{"a"}));
}
