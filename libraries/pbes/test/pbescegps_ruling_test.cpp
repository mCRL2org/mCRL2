// Author(s): Jore Booy
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbescegps_ruling_test.cpp
/// \brief Tests for the ruling relation logic in pbescegps.
///
/// Tests the ruling-relation traversal (find_dominant_roots),
/// the variable selection (choose_variable_by_ruling_order),
/// and the mutual-pair pruning in compute_ruling_relation.

#include "mcrl2/pbes/detail/parse.h"
#include "mcrl2/pbes/detail/pbescegps_utilities.h"
#include "mcrl2/pbes/tools/pbescegps.h"
#include "mcrl2/pbes/txt2pbes.h"
#define BOOST_TEST_MODULE pbescegps_ruling_test
#include <boost/test/included/unit_test.hpp>

using namespace mcrl2;
using namespace mcrl2::pbes_system;
using namespace mcrl2::pbes_system::detail;

// Helper: create a data::variable with a given name
static data::variable V(const std::string& name)
{
  return data::variable(core::identifier_string(name), data::sort_bool::bool_());
}

// Helper: build a ruling_relation_type for a single equation
static ruling_relation_type make_ruling(const std::string& eq_name,
  const std::map<std::string, std::set<std::string>>& edges)
{
  ruling_relation_type result;
  for (const auto& [ruled, rulers]: edges)
  {
    for (const auto& ruler: rulers)
    {
      result[core::identifier_string(eq_name)][V(ruled)].insert(V(ruler));
    }
  }
  return result;
}

// =========================================================================
// find_dominant_roots tests
// =========================================================================

// Helper: extract the ruled_by_map for equation eq_name from a ruling_relation_type
static const std::map<data::variable, std::set<data::variable>>& get_ruled_by_map(const ruling_relation_type& ruling,
  const std::string& eq_name)
{
  return ruling.at(core::identifier_string(eq_name));
}

// A is not ruled by anyone — it is the root.
BOOST_AUTO_TEST_CASE(test_find_root_unruled)
{
  auto ruling = make_ruling("Y", {{"B", {"A"}}}); // B ruled by A
  const auto& ruled_by_map = get_ruled_by_map(ruling, "Y");

  std::map<data::variable, std::size_t> var_counts = {{V("A"), 10}, {V("B"), 3}};
  std::set<data::variable> visited;

  auto roots = find_dominant_roots(V("B"), ruled_by_map, var_counts, visited);
  BOOST_REQUIRE_EQUAL(roots.size(), 1u);
  BOOST_CHECK_EQUAL(pp(roots[0].first), "A");
  BOOST_CHECK_EQUAL(roots[0].second, 10u);
}

// Chain: C ruled by B, B ruled by A, A unruled.
BOOST_AUTO_TEST_CASE(test_find_root_chain)
{
  auto ruling = make_ruling("Y", {{"C", {"B"}}, {"B", {"A"}}});
  const auto& ruled_by_map = get_ruled_by_map(ruling, "Y");

  std::map<data::variable, std::size_t> var_counts = {{V("A"), 5}, {V("B"), 8}, {V("C"), 1}};
  std::set<data::variable> visited;

  auto roots = find_dominant_roots(V("C"), ruled_by_map, var_counts, visited);
  // Traversal: C->B->A (root). A has occ=5, B has occ=8 but is not a root (ruled by A).
  BOOST_REQUIRE_EQUAL(roots.size(), 1u);
  BOOST_CHECK_EQUAL(pp(roots[0].first), "A");
  BOOST_CHECK_EQUAL(roots[0].second, 5u);
}

// Mutual: A ruled by B, B ruled by A. The cycle is broken at B.
BOOST_AUTO_TEST_CASE(test_find_root_mutual)
{
  auto ruling = make_ruling("Y", {{"A", {"B"}}, {"B", {"A"}}});
  const auto& ruled_by_map = get_ruled_by_map(ruling, "Y");

  std::map<data::variable, std::size_t> var_counts = {{V("A"), 3}, {V("B"), 7}};
  std::set<data::variable> visited;

  auto roots = find_dominant_roots(V("A"), ruled_by_map, var_counts, visited);
  // A->B (B's ruler is A, already on the path), so B is returned with occ=7.
  BOOST_REQUIRE_EQUAL(roots.size(), 1u);
  BOOST_CHECK_EQUAL(pp(roots[0].first), "B");
  BOOST_CHECK_EQUAL(roots[0].second, 7u);
}

// No rulers at all — start is itself the root.
BOOST_AUTO_TEST_CASE(test_find_root_alone)
{
  std::map<data::variable, std::set<data::variable>> ruled_by_map;
  // empty — nothing rules anything

  std::map<data::variable, std::size_t> var_counts = {{V("X"), 42}};
  std::set<data::variable> visited;

  auto roots = find_dominant_roots(V("X"), ruled_by_map, var_counts, visited);
  BOOST_REQUIRE_EQUAL(roots.size(), 1u);
  BOOST_CHECK_EQUAL(pp(roots[0].first), "X");
  BOOST_CHECK_EQUAL(roots[0].second, 42u);
}

// Diamond: D ruled by {B, C}, B ruled by A, C ruled by A, A unruled.
BOOST_AUTO_TEST_CASE(test_find_root_diamond)
{
  auto ruling = make_ruling("Y", {{"D", {"B", "C"}}, {"B", {"A"}}, {"C", {"A"}}});
  const auto& ruled_by_map = get_ruled_by_map(ruling, "Y");

  std::map<data::variable, std::size_t> var_counts = {{V("A"), 20}};
  std::set<data::variable> visited;

  auto roots = find_dominant_roots(V("D"), ruled_by_map, var_counts, visited);
  // Both branches lead to the same root A; it is reported only once.
  BOOST_REQUIRE_EQUAL(roots.size(), 1u);
  BOOST_CHECK_EQUAL(pp(roots[0].first), "A");
  BOOST_CHECK_EQUAL(roots[0].second, 20u);
}

// Two independent roots: D ruled by {B, C}, B ruled by A1, C ruled by A2.
// The roots are returned ordered by dominance (occurrence count).
BOOST_AUTO_TEST_CASE(test_find_roots_ordered)
{
  auto ruling = make_ruling("Y", {{"D", {"B", "C"}}, {"B", {"A1"}}, {"C", {"A2"}}});
  const auto& ruled_by_map = get_ruled_by_map(ruling, "Y");

  std::map<data::variable, std::size_t> var_counts = {{V("A1"), 20}, {V("A2"), 30}};
  std::set<data::variable> visited;

  auto roots = find_dominant_roots(V("D"), ruled_by_map, var_counts, visited);
  BOOST_REQUIRE_EQUAL(roots.size(), 2u);
  BOOST_CHECK_EQUAL(pp(roots[0].first), "A2");
  BOOST_CHECK_EQUAL(roots[0].second, 30u);
  BOOST_CHECK_EQUAL(pp(roots[1].first), "A1");
  BOOST_CHECK_EQUAL(roots[1].second, 20u);
}

// =========================================================================
// choose_variable_by_ruling_order tests
// =========================================================================

// Simple chain: A ruled by B, B unruled. Both abstracted.
// Should select B (the root) since it rules A.
BOOST_AUTO_TEST_CASE(test_choose_selects_root)
{
  ruling_relation_type ruling = make_ruling("Y",
    {
      {"A", {"B"}} // A ruled by B
    });

  std::set<data::variable> essential = {V("A"), V("B")};

  pbes_expression formula = pbes_system::true_();
  auto result = choose_variable_by_ruling_order(core::identifier_string("Y"), essential, ruling, formula);
  // B is the dominant root of A and is itself essential.
  BOOST_CHECK(result.has_value());
  BOOST_CHECK_EQUAL(pp(*result), "B");
}

// The most dominant root is not essential, so the next root in the ordered
// list is chosen instead of falling back to the starting variable.
BOOST_AUTO_TEST_CASE(test_choose_most_dominant_essential_root)
{
  // x ruled by {b, c}, b ruled by r1, c ruled by r2. r1 and r2 are roots.
  ruling_relation_type ruling = make_ruling("Y", {{"x", {"b", "c"}}, {"b", {"r1"}}, {"c", {"r2"}}});

  std::set<data::variable> essential = {V("x"), V("r2")}; // r1 is already concrete

  // Occurrence counts: x: 5, r1: 3, r2: 2.
  pbes_expression formula = parse_pbes_expression("val(x) || val(x) || val(x) || val(x) || val(x) || "
                                                  "val(r1) || val(r1) || val(r1) || val(r2) || val(r2)",
    "datavar x: Bool; r1: Bool; r2: Bool;\npredvar\n");

  auto result = choose_variable_by_ruling_order(core::identifier_string("Y"), essential, ruling, formula);
  // r1 (occ 3) is the most dominant root of x but not essential,
  // so r2 (occ 2) is chosen over falling back to x (occ 5).
  BOOST_REQUIRE(result.has_value());
  BOOST_CHECK_EQUAL(pp(*result), "r2");
}

// Root is not in essential_vars — should fall back to the starting variable.
BOOST_AUTO_TEST_CASE(test_choose_fallback_root_not_essential)
{
  // A ruled by B, but B is NOT in essential_vars (already concrete).
  ruling_relation_type ruling = make_ruling("Y", {{"A", {"B"}}});

  std::set<data::variable> essential = {V("A")}; // B is NOT essential

  pbes_expression formula = pbes_system::true_();
  auto result = choose_variable_by_ruling_order(core::identifier_string("Y"), essential, ruling, formula);
  // Should return A (the only essential variable), not B.
  BOOST_CHECK(result.has_value());
  BOOST_CHECK_EQUAL(pp(*result), "A");
}

// No ruling relation for this equation — should return nullopt.
BOOST_AUTO_TEST_CASE(test_choose_no_ruling)
{
  ruling_relation_type ruling;
  std::set<data::variable> essential = {V("A")};
  pbes_expression formula = pbes_system::true_();

  auto result = choose_variable_by_ruling_order(core::identifier_string("MISSING"), essential, ruling, formula);
  BOOST_CHECK(!result.has_value());
}

// =========================================================================
// Mutual-pair pruning tests (indirect, via the tool)
// =========================================================================

// PBES with mutual ruling: guard(a) changes b, guard(b) changes a.
// After pruning, only the stronger direction survives.
// X(a, b) = (val(a) || X(!a, b)) && (val(b) || X(a, !b))
// Guard of first transition: a. Changes: a. → a rules a (self, excluded).
// Guard of second transition: b. Changes: b. → b rules b (self, excluded).
// No mutual pairs here because guard vars and changed vars don't overlap.
// Let's make a PBES where they DO overlap.

// Y(a, b) = (val(a) || Y(b, a))
// Guard: a. Changes: a→b, b→a. So a rules b, b rules a. Mutual!
// After pruning: both have count 1, so one direction is removed.
BOOST_AUTO_TEST_CASE(test_mutual_pruning_via_tool)
{
  // This PBES has a mutual ruling pair.
  // Y(a, b) = (val(a) || Y(b, a))
  // init Y(false, false)
  std::string text = "pbes nu Y(a: Bool, b: Bool) = (val(a) || Y(b, a));"
                     "init Y(false, false);";

  pbescegps_options opts;
  opts.rewrite_strategy = data::rewrite_strategy::jitty;
  opts.var_choice = var_choice_strategy::ruling;

  pbes p = txt2pbes(text, false);
  pbescegps_iterator iterator;
  // This should complete without getting stuck.
  bool result = iterator.run_cegps_algorithm(p, opts);
  // Y(false, false) = (val(false) || Y(false, false)) = Y(false, false)
  // This is a tautology for the nu equation, so it should be TRUE.
  BOOST_CHECK(result);
}

// PBES with asymmetric ruling: guard(a) changes b (count 2), guard(b) changes a (count 1).
// The stronger direction (a rules b) survives, the weaker is pruned.
// Y(a, b) = (val(a) || Y(b, b)) && (val(a) || Y(!b, a))
// Transition 1: guard=a, changes a→b, b→b (b doesn't change). Only a changes.
//   So a rules b? No — a is the guard, b doesn't change.
// Let me construct a clearer example.

// Y(a, b) = (val(a && b) || Y(!a, !b))
// Guard: a && b. Changes: a→!a, b→!b. So {a,b} rules {a,b} — but self-rules excluded.
// So a rules b, b rules a. Both count 1. After pruning: one direction removed.
BOOST_AUTO_TEST_CASE(test_symmetric_ruling)
{
  std::string text = "pbes nu Y(a: Bool, b: Bool) = (val(a && b) || Y(!a, !b));"
                     "init Y(false, false);";

  pbescegps_options opts;
  opts.rewrite_strategy = data::rewrite_strategy::jitty;
  opts.var_choice = var_choice_strategy::ruling;

  pbes p = txt2pbes(text, false);
  pbescegps_iterator iterator;
  bool result = iterator.run_cegps_algorithm(p, opts);
  // Y(false, false) = (val(false && false) || Y(true, true))
  //                  = Y(true, true)
  // Y(true, true) = (val(true && true) || Y(false, false))
  //               = true || Y(false, false) = true
  // So Y(false, false) = true.
  BOOST_CHECK(result);
}

// PBES with 3 parameters where one rules the other two.
// Y(a, b, c) = (val(a) || Y(!a, b, c)) && (val(b) || Y(a, !b, c))
// Guard 1: a. Changes: a→!a. So a rules nothing (only a changes, self-excluded).
// Guard 2: b. Changes: b→!b. So b rules nothing.
// Not interesting. Let me make a guards-change-different-params example.

// Y(a, b) = (val(a) || Y(a, !b))
// Guard: a. Changes: b→!b. So a rules b. No mutual pair.
BOOST_AUTO_TEST_CASE(test_asymmetric_ruling_one_direction)
{
  std::string text = "pbes nu Y(a: Bool, b: Bool) = (val(a) || Y(a, !b));"
                     "init Y(false, false);";

  pbescegps_options opts;
  opts.rewrite_strategy = data::rewrite_strategy::jitty;
  opts.var_choice = var_choice_strategy::ruling;

  pbes p = txt2pbes(text, false);
  pbescegps_iterator iterator;
  bool result = iterator.run_cegps_algorithm(p, opts);
  // Y(false, false) = (val(false) || Y(false, true))
  //                  = Y(false, true)
  // Y(false, true) = (val(false) || Y(false, false))
  //                = Y(false, false)
  // Cycle: Y(false,false) = Y(false,true) = Y(false,false) = ...
  // For nu, greatest fixed point: both are true.
  BOOST_CHECK(result);
}
