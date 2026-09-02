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

// Return the set of still-abstracted parameter names for equation eq_name.
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

// nu equation, no transitions: nothing abstracted.
BOOST_AUTO_TEST_CASE(test_no_transitions)
{
  std::string text =
    "pbes nu X(a: Bool, b: Bool) = val(a);"
    "init X(false, true);";
  std::set<std::string> abstracted;
  BOOST_CHECK(!run_cegps(text, default_options(), "X", abstracted));
  BOOST_CHECK((abstracted == std::set<std::string>{}));
}

// mu equation, no transitions: nothing abstracted.
BOOST_AUTO_TEST_CASE(test_mu_equation)
{
  std::string text =
    "pbes mu X(a: Bool, b: Bool) = val(a && b);"
    "init X(true, true);";
  std::set<std::string> abstracted;
  BOOST_CHECK(run_cegps(text, default_options(), "X", abstracted));
  BOOST_CHECK((abstracted == std::set<std::string>{}));
}

// nu X self-transition on a: both params abstracted.
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

// Two equations: nu Z calls mu X0.
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

// --var-choice=ruling.
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

// ruling + rules_ideal.
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

// --var-choice=count.
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

// --var-choice=rhs: both params abstracted.
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

// --var-choice=all: both params abstracted.
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

// Ruling relation: guard(a) changes a and c, so a rules c.
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

// Regression: the candidate PVI comparison in select_variable advanced the
// successor iterator for abstracted parameters as well, misaligning it with
// the candidate iterator; the prematurely exhausted successor iterator was
// then treated as a match, so every candidate matched and the guard of the
// unrelated first transition (!val(p)) drove the refinement instead of the
// guard of the transition chosen by the strategy (val(q)).
//
// The abstraction state leaves only n concrete, and the over-approximation
// strategy chooses the transition with n=7, so select_variable must match
// only the candidate with argument 7 for n and un-abstract q; p (a red
// herring that never resolves the over-approximation) must stay abstracted.
BOOST_AUTO_TEST_CASE(test_select_variable_strategy_alignment)
{
  std::string text =
    "pbes nu X(p: Bool, q: Bool, n: Nat) ="
    "(val(p) || X(true, !q, 5))"
    " && (val(q) && (X(true, !q, 7) || (X(true, !q, 9) && val(n < 5))));"
    "init X(false, false, 0);";
  pbes p = txt2pbes(text);
  pbescegps_options opts = default_options();
  pbescegps_iterator iterator;
  iterator.initialize(p, opts);

  // Abstract p and q; only n stays concrete.
  abstract_param_state state;
  const pbes_equation& eq = detail::find_equation_by_name(p, core::identifier_string("X"))->get();
  const data::variable_list params = eq.variable().parameters();
  state.add_abstracted_variable(p, core::identifier_string("X"), atermpp::down_cast<data::variable>(*params.begin()));
  state.add_abstracted_variable(p,
    core::identifier_string("X"),
    atermpp::down_cast<data::variable>(*std::next(params.begin())));

  // Solve both approximations: the under-approximation is FALSE, the
  // over-approximation TRUE, so the refinement machinery is required.
  structure_graph under_graph;
  structure_graph over_graph;
  BOOST_CHECK(!iterator.solve_approximation_cached(p, state, false, opts, under_graph));
  BOOST_CHECK(iterator.solve_approximation_cached(p, state, true, opts, over_graph));

  // The FALSE vertex in the under-approximation mismatches its counterpart in
  // the over-approximation, so select_variable is reached immediately.
  pbes under_pbes = iterator.apply_abstraction_to_pbes(p, state, false, opts);
  pbes over_pbes = iterator.apply_abstraction_to_pbes(p, state, true, opts);
  data::rewriter datar(p.data(), opts.rewrite_strategy);
  ruling_relation_type ruling;
  pbescegps_refine_strategies refine;
  BOOST_CHECK(
    refine.refine_using_strategies(p, under_pbes, over_pbes, state, opts, under_graph, over_graph, datar, ruling));
  BOOST_CHECK((abstracted_names(state, "X") == std::set<std::string>{"p"}));
}
