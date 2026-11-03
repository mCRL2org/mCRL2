// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file cluster_test.cpp
/// \brief Tests for clustering the summands of a linear process.

#define BOOST_TEST_MODULE cluster_test

#include <boost/test/included/unit_test.hpp>

#include "mcrl2/lps/cluster.h"
#include "mcrl2/lps/explorer.h"
#include "mcrl2/lps/is_well_typed.h"
#include "mcrl2/lps/parse.h"

using namespace mcrl2;
using namespace mcrl2::lps;

// A transition from a state with a multi-action to a distribution over states. The distribution is a sorted vector
// of pairs of a probability and a state, so that the order in which the states are found does not matter.
using transition = std::tuple<state, multi_action, std::vector<std::pair<data::data_expression, state>>>;

static explorer_options make_explorer_options()
{
  explorer_options options;
  options.search_strategy = exploration_strategy::es_breadth;
  options.suppress_progress_messages = true;
  return options;
}

// The transitions of the reachable state space of a stochastic or untimed linear process.
static std::set<transition> transitions(const stochastic_specification& spec)
{
  const explorer_options options = make_explorer_options();
  const data::rewriter r(spec.data(), options.rewrite_strategy);
  explorer<true, false, stochastic_specification> explorer(spec, options, r);
  std::set<transition> result;
  explorer.generate_state_space(false,
    utilities::skip(),
    [&result](std::size_t /* thread_index */,
      std::size_t /* number_of_threads */,
      const state& s0,
      std::size_t /* s0_index */,
      const multi_action& a,
      const stochastic_state& s1,
      const std::list<std::size_t>& /* s1_index */,
      std::size_t /* summand_index */)
    {
      std::vector<std::pair<data::data_expression, state>> distribution;
      for (std::size_t i = 0; i < s1.states.size(); ++i)
      {
        distribution.emplace_back(s1.probabilities[i], s1.states[i]);
      }
      std::sort(distribution.begin(), distribution.end());
      result.emplace(s0, a, distribution);
    });
  return result;
}

// The transitions of the reachable state space of a timed, non-stochastic linear process. As in lps2lts, timed
// exploration is done without stochastic operators. The states contain the time.
static std::set<transition> timed_transitions(const stochastic_specification& spec)
{
  const specification non_stochastic_spec = remove_stochastic_operators(spec);
  const explorer_options options = make_explorer_options();
  const data::rewriter r(non_stochastic_spec.data(), options.rewrite_strategy);
  explorer<false, true, specification> explorer(non_stochastic_spec, options, r);
  std::set<transition> result;
  explorer.generate_state_space(false,
    utilities::skip(),
    [&result](std::size_t /* thread_index */,
      std::size_t /* number_of_threads */,
      const state& s0,
      std::size_t /* s0_index */,
      const multi_action& a,
      const state& s1,
      std::size_t /* s1_index */,
      std::size_t /* summand_index */) {
      result.emplace(s0, a, std::vector<std::pair<data::data_expression, state>>{{data::sort_real::real_one(), s1}});
    });
  return result;
}

static stochastic_specification parse(const std::string& text)
{
  stochastic_specification result;
  parse_lps(text, result);
  return result;
}

// Clusters spec, and checks that the result is well typed, has the given numbers of summands, and has the same
// transitions as spec.
template<bool Timed = false>
static stochastic_specification check_cluster(const stochastic_specification& spec,
  const cluster_options& options,
  std::size_t number_of_action_summands,
  std::size_t number_of_deadlock_summands)
{
  stochastic_specification result = spec;
  cluster(result, options);
  std::clog << "binary = " << std::boolalpha << options.binary << "\n" << lps::pp(result) << std::endl;
  BOOST_CHECK(detail::is_well_typed(result));
  BOOST_CHECK_EQUAL(result.process().action_summands().size(), number_of_action_summands);
  BOOST_CHECK_EQUAL(result.process().deadlock_summands().size(), number_of_deadlock_summands);
  const std::set<transition> expected = Timed ? timed_transitions(spec) : transitions(spec);
  BOOST_CHECK(!expected.empty());
  BOOST_CHECK(expected == (Timed ? timed_transitions(result) : transitions(result)));
  return result;
}

BOOST_AUTO_TEST_CASE(test_action_summands)
{
  const stochastic_specification spec = parse("act a: Nat;\n"
                                              "    b;\n"
                                              "proc P(n: Nat) =\n"
                                              "       (n < 3) -> a(n) . P(n = n + 1)\n"
                                              "     + sum m: Nat. (m < 2 && n == 3) -> a(m) . P(n = m)\n"
                                              "     + (n == 1) -> a(5) . P(n = 0)\n"
                                              "     + (n == 0) -> b . P(n = 2)\n"
                                              "     + (n == 2) -> b . P(n = 3);\n"
                                              "init P(0);\n");

  // The three summands with a are clustered with an enumerated sort with three elements, the two summands with b
  // with Bool.
  const stochastic_specification result = check_cluster(spec, cluster_options(), 2, 0);
  BOOST_CHECK_EQUAL(result.data().user_defined_aliases().size(), spec.data().user_defined_aliases().size() + 1);

  // With binary, no enumerated sort is generated.
  const stochastic_specification binary_result = check_cluster(spec, cluster_options{true}, 2, 0);
  BOOST_CHECK_EQUAL(binary_result.data().user_defined_aliases().size(), spec.data().user_defined_aliases().size());
}

BOOST_AUTO_TEST_CASE(test_timed_and_deadlock_summands)
{
  // Summands are only clustered with summands that also have a time, or that also have no time.
  stochastic_specification spec = parse("act a: Nat;\n"
                                        "proc P(n: Nat) =\n"
                                        "       (n == 0) -> a(0) @ 1 . P(n = 1)\n"
                                        "     + (n == 1) -> a(1) @ 2 . P(n = 2)\n"
                                        "     + (n == 2) -> a(2) . P(n = 3)\n"
                                        "     + delta;\n"
                                        "init P(0);\n");
  // The parser drops the time of a deadlock summand, so the timed deadlock summands (n == 3) -> delta @ 4 and
  // (n == 4) -> delta @ 5 are added explicitly.
  const data::variable& n = spec.process().process_parameters().front();
  for (std::size_t i = 3; i <= 4; ++i)
  {
    spec.process().deadlock_summands().emplace_back(data::variable_list(),
      data::equal_to(n, data::sort_nat::nat(i)),
      deadlock(data::sort_real::real_(i + 1)));
  }

  check_cluster<true>(spec, cluster_options(), 2, 2);
  check_cluster<true>(spec, cluster_options{true}, 2, 2);
}

BOOST_AUTO_TEST_CASE(test_stochastic_summands)
{
  // The stochastic variables x and y are merged. In the first summand y does not occur, so it is fixed to a default
  // value.
  const stochastic_specification spec = parse("act a: Bool;\n"
                                              "proc P(b, c: Bool) =\n"
                                              "       a(b) . dist x: Bool[1 / 2] . P(b = x)\n"
                                              "     + (!b) -> a(c) . dist x, y: Bool[1 / 4] . P(b = x, c = y);\n"
                                              "init P(false, false);\n");

  check_cluster(spec, cluster_options(), 1, 0);
  check_cluster(spec, cluster_options{true}, 1, 0);
}

BOOST_AUTO_TEST_CASE(test_name_clashes)
{
  // The sum variables m of the two summands have different sorts and are both kept; one of them is renamed. The names
  // e and Enum3 are in use, so the clustering must use other names.
  const stochastic_specification spec = parse("sort Enum3 = struct c1 | c2;\n"
                                              "act a;\n"
                                              "proc P(e: Enum3, n: Nat) =\n"
                                              "       sum m: Nat. (m < 2 && n == 0) -> a . P(n = m + 1)\n"
                                              "     + sum m: Bool. (n == 1) -> a . P(e = if(m, c1, c2), n = 2)\n"
                                              "     + (n == 2) -> a . P(n = 0);\n"
                                              "init P(c1, 0);\n");

  check_cluster(spec, cluster_options(), 1, 0);
  check_cluster(spec, cluster_options{true}, 1, 0);
}
