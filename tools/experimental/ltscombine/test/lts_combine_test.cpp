// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_TEST_MODULE lts_combine_test
#include <boost/test/included/unit_test.hpp>

#include "../lts_combine.h"

#include "mcrl2/lps/parse.h"
#include "mcrl2/lts/lts_lts.h"
#include "mcrl2/process/parse.h"

#include <cstdio>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_set>
#include <vector>

using namespace mcrl2;

namespace
{

/// \brief An LTS with the given action declarations in which the transitions are given as (from, label, to).
lts::lts_lts_t make_lts(const process::action_label_list& declarations,
  std::size_t number_of_states,
  const std::vector<std::tuple<std::size_t, std::string, std::size_t>>& transitions)
{
  lts::lts_lts_t lts;
  lts.set_action_label_declarations(declarations);
  for (std::size_t i = 0; i < number_of_states; ++i)
  {
    lts.add_state();
  }
  lts.set_initial_state(0);
  for (const auto& [from, text, to]: transitions)
  {
    const lts::action_label_lts label(lps::parse_multi_action(text, declarations, data::data_specification()));
    lts.add_transition(lts::transition(from, lts.add_action(label), to));
  }
  return lts;
}

/// \brief Combines the LTSs in the same way as the ltscombine tool and returns the result.
lts::lts_lts_t combine(const std::vector<lts::lts_lts_t>& ltss,
  const std::string& comm,
  const std::string& allow,
  const std::string& hide,
  bool save_at_end,
  std::size_t number_of_threads)
{
  std::istringstream comm_input(comm);
  std::istringstream hide_input(hide);
  const process::communication_expression_list comm_set = parse_comm_set(comm_input);
  const process::action_name_multiset_list allow_set = parse_multi_action_name_set(allow);
  const core::identifier_string_list block_set;
  const core::identifier_string_list hide_set
    = atermpp::sort_list(parse_hide_set(hide_input), process::action_name_compare());
  const lps::detail::allow_list_cache allow_cache = lps::detail::make_allow_list_cache(allow_set);

  process::action_name_set used_actions;
  for (const lts::lts_lts_t& lts: ltss)
  {
    for (const lts::action_label_lts& label: lts.action_labels())
    {
      for (const process::action& action: label.actions())
      {
        used_actions.insert(action.label().name());
      }
    }
  }
  const process::action_name_multiset_list inner_allow_set
    = calculate_inner_allow_set(comm_set, used_actions, allow_set);
  std::unordered_set<core::identifier_string> inner_allowed_action_names;
  for (const process::action_name_multiset& names: inner_allow_set)
  {
    inner_allowed_action_names.insert(names.names().begin(), names.names().end());
  }

  const std::string filename = "lts_combine_test_output.lts";
  combine_lts(combine_lts_static_context{ltss,
    comm_set,
    block_set,
    hide_set,
    allow_cache,
    inner_allow_set,
    inner_allowed_action_names,
    filename,
    save_at_end,
    number_of_threads});

  lts::lts_lts_t result;
  result.load(filename);
  std::remove(filename.c_str());
  return result;
}

/// \returns The transitions of lts as (from, label, to) with the labels pretty printed.
std::multiset<std::tuple<std::size_t, std::string, std::size_t>> transitions(const lts::lts_lts_t& lts)
{
  std::multiset<std::tuple<std::size_t, std::string, std::size_t>> result;
  for (const lts::transition& t: lts.get_transitions())
  {
    result.emplace(t.from(), lts::pp(lts.action_label(t.label())), t.to());
  }
  return result;
}

} // namespace

BOOST_AUTO_TEST_CASE(test_communication_allow_and_hide)
{
  const process::action_label_list declarations = process::parse_action_declaration("a, b, c, d, e;");
  // The first component does a and then c or e, the second does b.
  const std::vector<lts::lts_lts_t> ltss{make_lts(declarations, 2, {{0, "a", 1}, {1, "c", 0}, {1, "e", 0}}),
    make_lts(declarations, 2, {{0, "b", 1}})};

  // a and b only occur in the communication d; e is not allowed and c is hidden.
  for (bool save_at_end: {false, true})
  {
    for (std::size_t threads: {1, 4})
    {
      const lts::lts_lts_t result = combine(ltss, "{a|b -> d}", "{d, c}", "{c}", save_at_end, threads);
      BOOST_CHECK_EQUAL(result.num_states(), 3u);
      BOOST_CHECK_EQUAL(result.num_transitions(), 2u);

      // The state indices depend on the exploration order with several threads.
      std::multiset<std::string> labels;
      for (const auto& [from, label, to]: transitions(result))
      {
        labels.insert(label);
      }
      BOOST_CHECK(labels == (std::multiset<std::string>{"d", "tau"}));
      if (threads == 1)
      {
        BOOST_CHECK(transitions(result) == (std::multiset<std::tuple<std::size_t, std::string, std::size_t>>{
          {0, "d", 1},
          {1, "tau", 2}
        }));
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(test_same_label_combination_in_different_states)
{
  // The combination a|b is reached from every state; every occurrence must be reported, and the cached label must not
  // depend on the state in which it was computed first.
  const process::action_label_list declarations = process::parse_action_declaration("a, b, c;");
  const std::vector<lts::lts_lts_t> ltss{make_lts(declarations, 3, {{0, "a", 1}, {1, "a", 2}, {2, "a", 0}}),
    make_lts(declarations, 2, {{0, "b", 1}, {1, "b", 0}})};

  const lts::lts_lts_t result = combine(ltss, "{a|b -> c}", "{c}", "{}", false, 1);
  BOOST_CHECK_EQUAL(result.num_states(), 6u);
  BOOST_CHECK_EQUAL(result.num_transitions(), 6u);
  for (const auto& [from, label, to]: transitions(result))
  {
    BOOST_CHECK_EQUAL(label, "c");
  }
}

BOOST_AUTO_TEST_CASE(test_many_components)
{
  // 3^41 > 2^64, so the label combinations of these 41 components cannot be numbered by a 64-bit integer.
  const process::action_label_list declarations = process::parse_action_declaration("a, b, c;");
  std::vector<lts::lts_lts_t> ltss{make_lts(declarations, 2, {{0, "a", 1}, {1, "c", 0}})};
  for (std::size_t i = 1; i < 41; ++i)
  {
    ltss.push_back(make_lts(declarations, 2, {{0, "b", 1}, {1, "c", 0}}));
  }
  // Only the first component can move, as b and c are not allowed.
  const lts::lts_lts_t result = combine(ltss, "{}", "{a}", "{}", false, 1);
  BOOST_CHECK_EQUAL(result.num_states(), 2u);
  BOOST_CHECK(transitions(result) == (std::multiset<std::tuple<std::size_t, std::string, std::size_t>>{{0, "a", 1}}));
}

BOOST_AUTO_TEST_CASE(test_cache_distinguishes_components)
{
  // In both components the label with index 1 is the first label added, but only a is allowed. The combined label of
  // the first component moving alone must not be reused for the second component moving alone.
  const process::action_label_list declarations = process::parse_action_declaration("a, b;");
  const std::vector<lts::lts_lts_t> ltss{make_lts(declarations, 2, {{0, "a", 1}}), make_lts(declarations, 2, {{0, "b", 1}})};

  const lts::lts_lts_t result = combine(ltss, "{}", "{a, a|b}", "{}", false, 1);
  BOOST_CHECK_EQUAL(result.num_states(), 3u);
  BOOST_CHECK(transitions(result) == (std::multiset<std::tuple<std::size_t, std::string, std::size_t>>{
    {0, "a", 1},
    {0, "a|b", 2}
  }));
}
