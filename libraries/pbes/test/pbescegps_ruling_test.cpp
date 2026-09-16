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

#include "mcrl2/pbes/detail/pbescegps_utilities.h"
#include "mcrl2/pbes/tools/pbescegps.h"
#include "mcrl2/pbes/txt2pbes.h"
#include <functional>
#define BOOST_TEST_MODULE pbescegps_ruling_test
#include <boost/test/included/unit_test.hpp>

using namespace mcrl2;
using namespace mcrl2::pbes_system;
using namespace mcrl2::pbes_system::detail;

// Create a boolean data variable with the given name.
static data::variable V(const std::string& name)
{
  return data::variable(core::identifier_string(name), data::sort_bool::bool_());
}

// Build a ruling_relation_type for a single equation.
static ruling_relation_type make_ruling(const std::string& eq_name,
  const std::map<std::string, std::set<std::string>>& edges)
{
  ruling_relation_type result;
  for (const auto& [ruled, rulers]: edges)
  {
    for (const auto& ruler: rulers)
    {
      result.ruled_by[core::identifier_string(eq_name)][V(ruled)].insert(V(ruler));
    }
  }
  result.compute_tree_sizes();
  return result;
}

// dominant_ancestors tests

// Extract the ruled_by_map for equation eq_name.
static const ruling_relation_type::equation_relation& get_ruled_by_map(const ruling_relation_type& ruling,
  const std::string& eq_name)
{
  return ruling.ruled_by.at(core::identifier_string(eq_name));
}

// B is ruled by A: its dominator closure is {A}.
BOOST_AUTO_TEST_CASE(test_dominators_direct)
{
  auto ruling = make_ruling("Y", {{"B", {"A"}}}); // B ruled by A
  const auto& ruled_by_map = get_ruled_by_map(ruling, "Y");

  auto ancestors = dominant_ancestors(V("B"), ruled_by_map);
  BOOST_REQUIRE_EQUAL(ancestors.size(), 1u);
  BOOST_CHECK_EQUAL(pp(*ancestors.begin()), "A");
}

// Chain C ruled by B ruled by A: the closure of C contains both B and A.
BOOST_AUTO_TEST_CASE(test_dominators_chain)
{
  auto ruling = make_ruling("Y", {{"C", {"B"}}, {"B", {"A"}}});
  const auto& ruled_by_map = get_ruled_by_map(ruling, "Y");

  auto ancestors = dominant_ancestors(V("C"), ruled_by_map);
  BOOST_REQUIRE_EQUAL(ancestors.size(), 2u);
  BOOST_CHECK(ancestors == std::set<data::variable>({V("A"), V("B")}));
}

// No rulers — the closure is empty.
BOOST_AUTO_TEST_CASE(test_dominators_root)
{
  auto ruling = make_ruling("Y", {{"B", {"A"}}});
  const auto& ruled_by_map = get_ruled_by_map(ruling, "Y");

  BOOST_CHECK_EQUAL(dominant_ancestors(V("A"), ruled_by_map).size(), 0u);
}

// Diamond: both branches collapse into the same ancestor set.
BOOST_AUTO_TEST_CASE(test_dominators_diamond)
{
  auto ruling = make_ruling("Y", {{"D", {"B", "C"}}, {"B", {"A"}}, {"C", {"A"}}});
  const auto& ruled_by_map = get_ruled_by_map(ruling, "Y");

  auto ancestors = dominant_ancestors(V("D"), ruled_by_map);
  BOOST_REQUIRE_EQUAL(ancestors.size(), 3u);
  BOOST_CHECK(ancestors == std::set<data::variable>({V("A"), V("B"), V("C")}));
}

// choose_variable_by_ruling_order tests

// A ruled by B; both essential. B (the root) is selected.
BOOST_AUTO_TEST_CASE(test_choose_selects_root)
{
  ruling_relation_type ruling = make_ruling("Y", {{"A", {"B"}}});

  std::set<data::variable> essential = {V("A"), V("B")};

  auto result = choose_variable_by_ruling_order(core::identifier_string("Y"), essential, ruling);
  BOOST_CHECK(result.has_value());
  BOOST_CHECK_EQUAL(pp(*result), "B");
}

// r1 rules three parameters (b, e, x), r2 only two (c, x): the larger tree wins.
BOOST_AUTO_TEST_CASE(test_choose_largest_tree_size_root)
{
  ruling_relation_type ruling = make_ruling("Y", {{"x", {"b", "c"}}, {"b", {"r1"}}, {"c", {"r2"}}, {"e", {"r1"}}});

  std::set<data::variable> essential = {V("x"), V("r1"), V("r2")};

  auto result = choose_variable_by_ruling_order(core::identifier_string("Y"), essential, ruling);
  BOOST_REQUIRE(result.has_value());
  BOOST_CHECK_EQUAL(pp(*result), "r1");
}

// r1 (tree size 3) is not essential; r2 (tree size 2) chosen.
BOOST_AUTO_TEST_CASE(test_choose_largest_essential_tree_size_root)
{
  ruling_relation_type ruling = make_ruling("Y", {{"x", {"b", "c"}}, {"b", {"r1"}}, {"c", {"r2"}}, {"e", {"r1"}}});

  std::set<data::variable> essential = {V("x"), V("r2")}; // r1 already concrete

  auto result = choose_variable_by_ruling_order(core::identifier_string("Y"), essential, ruling);
  BOOST_REQUIRE(result.has_value());
  BOOST_CHECK_EQUAL(pp(*result), "r2");
}

// Chain A ruled by B ruled by C, with only A and B essential: B is the essential
// ancestor (tree size 1) and is selected.
BOOST_AUTO_TEST_CASE(test_choose_extra_essential_ancestor)
{
  ruling_relation_type ruling = make_ruling("Y", {{"A", {"B"}}, {"B", {"C"}}});

  std::set<data::variable> essential = {V("A"), V("B")}; // C already concrete

  auto result = choose_variable_by_ruling_order(core::identifier_string("Y"), essential, ruling);
  BOOST_REQUIRE(result.has_value());
  BOOST_CHECK_EQUAL(pp(*result), "B");
}

// Root B not essential; the fallback starting variable A has no dominance
// (tree size 0), so nothing is selected.
BOOST_AUTO_TEST_CASE(test_choose_fallback_root_not_essential)
{
  ruling_relation_type ruling = make_ruling("Y", {{"A", {"B"}}});

  std::set<data::variable> essential = {V("A")}; // B not essential

  auto result = choose_variable_by_ruling_order(core::identifier_string("Y"), essential, ruling);
  BOOST_CHECK(!result.has_value());
}

// No ruling relation for the equation — returns nullopt.
BOOST_AUTO_TEST_CASE(test_choose_no_ruling)
{
  ruling_relation_type ruling;
  std::set<data::variable> essential = {V("A")};

  auto result = choose_variable_by_ruling_order(core::identifier_string("MISSING"), essential, ruling);
  BOOST_CHECK(!result.has_value());
}

// flip_frozen_rulers tests

// b never changes, so the edge "a ruled by b" is flipped to "b ruled by a".
BOOST_AUTO_TEST_CASE(test_flip_frozen_rulers)
{
  ruling_statistics_type stats;
  const core::identifier_string eq("Y");
  stats.changes[eq][V("a")] = 2; // a changes, b is frozen
  stats.counts[eq][V("a")][V("b")] = 2;

  flip_frozen_rulers(stats);

  BOOST_CHECK(!stats.counts[eq].contains(V("a")));
  BOOST_REQUIRE(stats.counts[eq].contains(V("b")));
  BOOST_CHECK_EQUAL(stats.counts[eq].at(V("b")).at(V("a")), 2u);
}

// A ruler that changes is not frozen, so the edge direction is left untouched.
BOOST_AUTO_TEST_CASE(test_flip_frozen_rulers_keeps_dynamic_ruler)
{
  ruling_statistics_type stats;
  const core::identifier_string eq("Y");
  stats.changes[eq][V("a")] = 1;
  stats.changes[eq][V("c")] = 1; // c changes, so it is not frozen
  stats.counts[eq][V("a")][V("c")] = 1;

  flip_frozen_rulers(stats);

  BOOST_REQUIRE(stats.counts[eq].contains(V("a")));
  BOOST_CHECK(stats.counts[eq].at(V("a")).contains(V("c")));
  BOOST_CHECK(!stats.counts[eq].contains(V("c")));
}

// compute_tree_sizes tests

// Chain C -> B -> A: A has two descendants, B has one, C has none.
BOOST_AUTO_TEST_CASE(test_tree_sizes_chain)
{
  ruling_relation_type ruling = make_ruling("Y", {{"C", {"B"}}, {"B", {"A"}}});

  const auto& sizes = ruling.tree_size.at(core::identifier_string("Y"));
  BOOST_CHECK_EQUAL(sizes.at(V("A")), 2u);
  BOOST_CHECK_EQUAL(sizes.at(V("B")), 1u);
  BOOST_CHECK_EQUAL(sizes.at(V("C")), 0u);
}

// A diamond: C is reached through both B1 and B2 but must be counted once.
BOOST_AUTO_TEST_CASE(test_tree_sizes_shared_descendant)
{
  ruling_relation_type ruling = make_ruling("Y", {{"C", {"B1", "B2"}}, {"B1", {"A"}}, {"B2", {"A"}}});

  const auto& sizes = ruling.tree_size.at(core::identifier_string("Y"));
  BOOST_CHECK_EQUAL(sizes.at(V("A")), 3u); // rules C, B1 and B2
  BOOST_CHECK_EQUAL(sizes.at(V("B1")), 1u);
  BOOST_CHECK_EQUAL(sizes.at(V("B2")), 1u);
  BOOST_CHECK_EQUAL(sizes.at(V("C")), 0u);
}

// Recomputing after a mutation keeps the cache in sync.
BOOST_AUTO_TEST_CASE(test_tree_sizes_recompute)
{
  ruling_relation_type ruling = make_ruling("Y", {{"B", {"A"}}});
  BOOST_CHECK_EQUAL(ruling.tree_size.at(core::identifier_string("Y")).at(V("A")), 1u);

  ruling.ruled_by[core::identifier_string("Y")][V("C")].insert(V("B"));
  ruling.compute_tree_sizes();

  const auto& sizes = ruling.tree_size.at(core::identifier_string("Y"));
  BOOST_CHECK_EQUAL(sizes.at(V("A")), 2u); // now rules B and C
  BOOST_CHECK_EQUAL(sizes.at(V("B")), 1u); // now rules C
  BOOST_CHECK_EQUAL(sizes.at(V("C")), 0u);
}

// Mutual-pair pruning tests

// Y(a, b) = (val(a && b) || Y(!a, !b)): a<->b mutual, both count 1.
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
  BOOST_CHECK(result);
}

// Y(a, b) = (val(a) || Y(a, !b)): guard a changes b, so a rules b (no mutual pair).
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
  BOOST_CHECK(result);
}

// Check that the ruling relation for equation eq_name is acyclic.
static bool relation_acyclic(const ruling_relation_type& ruling, const std::string& eq_name)
{
  const auto& ruled_by_map = ruling.ruled_by.at(core::identifier_string(eq_name));
  std::set<data::variable> on_path;
  std::set<data::variable> done;
  std::function<bool(const data::variable&)> has_cycle = [&](const data::variable& current) -> bool
  {
    if (on_path.contains(current))
    {
      return true;
    }
    if (done.contains(current))
    {
      return false;
    }
    on_path.insert(current);
    auto it = ruled_by_map.find(current);
    if (it != ruled_by_map.end())
    {
      for (const data::variable& ruler: it->second)
      {
        if (has_cycle(ruler))
        {
          return true;
        }
      }
    }
    on_path.erase(current);
    done.insert(current);
    return false;
  };
  for (const auto& [d_m, rulers]: ruled_by_map)
  {
    if (has_cycle(d_m))
    {
      return false;
    }
  }
  return true;
}

// Mutual pair with equal counts: the tie is broken by name, so exactly one
// direction of the a <-> b cycle survives and the relation is acyclic.
BOOST_AUTO_TEST_CASE(test_mutual_cycle_pruned)
{
  std::string text = "pbes nu Y(a: Bool, b: Bool) = (val(a && b) || Y(!a, !b));"
                     "init Y(false, false);";

  pbescegps_options opts;
  opts.rewrite_strategy = data::rewrite_strategy::jitty;
  opts.var_choice = var_choice_strategy::ruling;

  pbes p = txt2pbes(text, false);
  pbescegps_iterator iterator;
  bool result = iterator.run_cegps_algorithm(p, opts);
  BOOST_REQUIRE(result);

  const auto& ruled_by_map = iterator.ruling_relation().ruled_by.at(core::identifier_string("Y"));
  bool a_ruled_by_b = ruled_by_map.contains(V("a")) && ruled_by_map.at(V("a")).contains(V("b"));
  bool b_ruled_by_a = ruled_by_map.contains(V("b")) && ruled_by_map.at(V("b")).contains(V("a"));
  BOOST_CHECK_MESSAGE(!(a_ruled_by_b && b_ruled_by_a), "mutual pair a <-> b was not pruned to a single direction");
  BOOST_CHECK_MESSAGE(relation_acyclic(iterator.ruling_relation(), "Y"), "ruling relation still contains a cycle");
}

// Three parameters forming a cycle: b guards a, c guards b, a guards c.
// break_ruling_cycles must reduce this longer cycle to an acyclic relation.
BOOST_AUTO_TEST_CASE(test_three_cycle_pruned)
{
  std::string text = "pbes nu Y(a: Bool, b: Bool, c: Bool) = "
                     "(val(b) || Y(!a, b, c)) && (val(c) || Y(a, !b, c)) && (val(a) || Y(a, b, !c));"
                     "init Y(false, false, false);";

  pbescegps_options opts;
  opts.rewrite_strategy = data::rewrite_strategy::jitty;
  opts.var_choice = var_choice_strategy::ruling;

  pbes p = txt2pbes(text, false);
  pbescegps_iterator iterator;
  bool result = iterator.run_cegps_algorithm(p, opts);
  BOOST_REQUIRE(result);

  BOOST_CHECK_MESSAGE(relation_acyclic(iterator.ruling_relation(), "Y"), "ruling relation still contains a cycle");
}

// remove_transitive_rulings tests

// Chain C ruled by B ruled by A plus the implied edge C ruled by A: only the
// transitive edge C -> A is dropped, the covering edges survive.
BOOST_AUTO_TEST_CASE(test_remove_transitive_chain)
{
  ruling_relation_type ruling = make_ruling("Y", {{"C", {"A", "B"}}, {"B", {"A"}}});

  remove_transitive_rulings(ruling);

  const auto& ruled_by_map = get_ruled_by_map(ruling, "Y");
  BOOST_REQUIRE(ruled_by_map.contains(V("C")));
  BOOST_CHECK(!ruled_by_map.at(V("C")).contains(V("A")));
  BOOST_CHECK(ruled_by_map.at(V("C")).contains(V("B")));
  BOOST_CHECK(ruled_by_map.contains(V("B")) && ruled_by_map.at(V("B")).contains(V("A")));
}

// Diamond D -> {B, C} -> A plus the implied edge D ruled by A: dropped, while
// the independent edges D ruled by B and D ruled by C are kept.
BOOST_AUTO_TEST_CASE(test_remove_transitive_diamond)
{
  ruling_relation_type ruling = make_ruling("Y", {{"D", {"A", "B", "C"}}, {"B", {"A"}}, {"C", {"A"}}});

  remove_transitive_rulings(ruling);

  const auto& ruled_by_map = get_ruled_by_map(ruling, "Y");
  BOOST_REQUIRE(ruled_by_map.contains(V("D")));
  BOOST_CHECK(!ruled_by_map.at(V("D")).contains(V("A")));
  BOOST_CHECK(ruled_by_map.at(V("D")).contains(V("B")));
  BOOST_CHECK(ruled_by_map.at(V("D")).contains(V("C")));
}

// Two separate roots A1, A2 both ruling d transitively through different
// chains: both implied edges are dropped.
BOOST_AUTO_TEST_CASE(test_remove_transitive_two_roots)
{
  ruling_relation_type ruling = make_ruling("Y", {{"d", {"A1", "A2", "b", "c"}}, {"b", {"A1"}}, {"c", {"A2"}}});

  remove_transitive_rulings(ruling);

  const auto& ruled_by_map = get_ruled_by_map(ruling, "Y");
  BOOST_REQUIRE(ruled_by_map.contains(V("d")));
  BOOST_CHECK(!ruled_by_map.at(V("d")).contains(V("A1")));
  BOOST_CHECK(!ruled_by_map.at(V("d")).contains(V("A2")));
  BOOST_CHECK(ruled_by_map.at(V("d")).contains(V("b")));
  BOOST_CHECK(ruled_by_map.at(V("d")).contains(V("c")));
}

// A single long chain C -> B -> A with an implied 3-step edge: the check must
// look past direct edges. Also verifies that a node left without rulers is
// erased from the map.
BOOST_AUTO_TEST_CASE(test_remove_transitive_indirect)
{
  ruling_relation_type ruling = make_ruling("Y", {{"C", {"A", "B"}}, {"B", {"A"}}});

  remove_transitive_rulings(ruling);

  // C is still reachable from A through B; entry for C keeps only B.
  const auto& ruled_by_map = get_ruled_by_map(ruling, "Y");
  BOOST_CHECK_EQUAL(ruled_by_map.at(V("C")).size(), 1u);
}

// An equation whose edges are all covering pairs stays unchanged.
BOOST_AUTO_TEST_CASE(test_remove_transitive_nothing_to_do)
{
  ruling_relation_type ruling = make_ruling("Y", {{"B", {"A"}}, {"C", {"A"}}});

  remove_transitive_rulings(ruling);

  const auto& ruled_by_map = get_ruled_by_map(ruling, "Y");
  BOOST_REQUIRE_EQUAL(ruled_by_map.size(), 2u);
  BOOST_CHECK(ruled_by_map.at(V("B")).contains(V("A")));
  BOOST_CHECK(ruled_by_map.at(V("C")).contains(V("A")));
}
