// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file linearise_parallel_test.cpp
/// \brief Tests for the left merge of the parallel composition in the lineariser.

#define BOOST_TEST_MODULE linearise_parallel_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/data/real.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/lps/explorer.h"
#include "mcrl2/lps/find.h"
#include "mcrl2/lps/linearise_parallel.h"
#include "mcrl2/lps/parse.h"

using namespace mcrl2;
using namespace mcrl2::lps;

namespace
{

const data::variable n("n", data::sort_nat::nat());
const data::variable u("u", data::sort_real::real_());
const data::variable t("t", data::sort_real::real_());
const data::data_expression three = data::sort_real::real_(3);
const data::data_expression five = data::sort_real::real_(5);

// The ultimate delay t < 3, for t the time variable.
detail::ultimate_delay delay_until_three()
{
  return detail::ultimate_delay(t, data::variable_list(), data::less(t, three));
}

// The ultimate delay t < u, for some u: Real.
detail::ultimate_delay delay_until_u()
{
  return detail::ultimate_delay(t, {u}, data::less(t, u));
}

// The distribution of a summand without stochastic operator, as the lineariser makes it.
stochastic_distribution no_distribution()
{
  return stochastic_distribution(data::variable_list(), data::sort_real::real_one());
}

process::action make_action(const std::string& name)
{
  return process::action(process::action_label(core::identifier_string(name), data::sort_expression_list()),
    data::data_expression_list());
}

stochastic_action_summand make_summand(const process::action_list& actions,
  const data::variable_list& sumvars = data::variable_list(),
  const data::data_expression& condition = data::sort_bool::true_())
{
  return stochastic_action_summand(sumvars,
    condition,
    multi_action(actions),
    data::assignment_list(),
    no_distribution());
}

data::set_identifier_generator make_identifier_generator()
{
  data::set_identifier_generator result;
  for (const std::string& name: {"n", "u", "t", "a", "b", "Terminate"})
  {
    result.add_identifier(core::identifier_string(name));
  }
  return result;
}

data::data_expression identity(const data::data_expression& x)
{
  return x;
}

} // namespace

// An untimed summand gets the time variable of the ultimate delay as its time and as a sum variable, and the
// ultimate delay as its condition.
BOOST_AUTO_TEST_CASE(test_untimed_action_summand)
{
  data::set_identifier_generator id_generator = make_identifier_generator();
  const stochastic_action_summand summand = make_summand({make_action("a")});
  const stochastic_action_summand result = left_merge_summand(summand, delay_until_three(), id_generator, identity);

  BOOST_CHECK(result.has_time());
  BOOST_CHECK_EQUAL(result.multi_action().time(), t);
  BOOST_CHECK_EQUAL(result.summation_variables(), data::variable_list({t}));
  BOOST_CHECK_EQUAL(result.condition(), data::less(t, three));
  BOOST_CHECK_EQUAL(result.multi_action().actions(), summand.multi_action().actions());
}

// A timed summand gets the ultimate delay at its own time as an extra condition.
BOOST_AUTO_TEST_CASE(test_timed_action_summand)
{
  data::set_identifier_generator id_generator = make_identifier_generator();
  const data::data_expression condition = data::less(n, data::sort_nat::nat(2));
  const stochastic_action_summand summand(data::variable_list(),
    condition,
    multi_action(process::action_list({make_action("a")}), five),
    data::assignment_list(),
    no_distribution());
  const stochastic_action_summand result = left_merge_summand(summand, delay_until_three(), id_generator, identity);

  BOOST_CHECK_EQUAL(result.multi_action().time(), five);
  BOOST_CHECK(result.summation_variables().empty());
  BOOST_CHECK_EQUAL(result.condition(), data::sort_bool::and_(condition, data::less(five, three)));
}

// If the ultimate delay is true, an untimed summand stays untimed.
BOOST_AUTO_TEST_CASE(test_ultimate_delay_true)
{
  data::set_identifier_generator id_generator = make_identifier_generator();
  const stochastic_action_summand summand = make_summand({make_action("a")});
  const stochastic_action_summand result
    = left_merge_summand(summand, detail::ultimate_delay(t), id_generator, identity);
  BOOST_CHECK_EQUAL(result, summand);
}

// The variables of the ultimate delay become sum variables, renamed away from those of the summand.
BOOST_AUTO_TEST_CASE(test_delay_variables_renamed)
{
  data::set_identifier_generator id_generator = make_identifier_generator();
  const data::data_expression condition = data::less(u, five);
  const stochastic_action_summand summand = make_summand({make_action("a")}, {u}, condition);
  const stochastic_action_summand result = left_merge_summand(summand, delay_until_u(), id_generator, identity);

  BOOST_REQUIRE_EQUAL(result.summation_variables().size(), 3U);
  data::variable_list::const_iterator i = result.summation_variables().begin();
  BOOST_CHECK_EQUAL(*i++, t);
  BOOST_CHECK_EQUAL(*i++, u);
  const data::variable u1 = *i;
  BOOST_CHECK_NE(u1, u);
  BOOST_CHECK_EQUAL(u1.sort(), u.sort());
  BOOST_CHECK_EQUAL(result.condition(), data::sort_bool::and_(condition, data::less(t, u1)));
}

// A deadlock summand is restricted in the same way.
BOOST_AUTO_TEST_CASE(test_deadlock_summand)
{
  data::set_identifier_generator id_generator = make_identifier_generator();
  const deadlock_summand summand(data::variable_list(), data::sort_bool::true_(), deadlock());
  const deadlock_summand result = left_merge_summand(summand, delay_until_three(), id_generator, identity);

  BOOST_CHECK(result.deadlock().has_time());
  BOOST_CHECK_EQUAL(result.deadlock().time(), t);
  BOOST_CHECK_EQUAL(result.summation_variables(), data::variable_list({t}));
  BOOST_CHECK_EQUAL(result.condition(), data::less(t, three));
}

// Without time the termination action is left out, the filter is applied, and deadlock summands are left out.
BOOST_AUTO_TEST_CASE(test_left_merge_without_time)
{
  data::set_identifier_generator id_generator = make_identifier_generator();
  const process::action a = make_action("a");
  const process::action b = make_action("b");
  const process::action termination = make_action("Terminate");
  const stochastic_action_summand_vector action_summands1{make_summand({a}),
    make_summand({b}),
    make_summand({termination}),
    make_summand({a}, {}, data::sort_bool::false_())};
  const deadlock_summand_vector deadlock_summands1{
    deadlock_summand(data::variable_list(), data::sort_bool::true_(), deadlock())};

  // allow({a})
  const process::action_name_multiset_list allowlist(
    {process::action_name_multiset(core::identifier_string_list({core::identifier_string("a")}))});
  const allow_block_filter filter(allowlist, true, false, termination);

  stochastic_action_summand_vector action_summands;
  deadlock_summand_vector deadlock_summands;
  left_merge(action_summands1,
    deadlock_summands1,
    detail::ultimate_delay(),
    filter,
    termination,
    true,
    id_generator,
    identity,
    action_summands,
    deadlock_summands);

  BOOST_REQUIRE_EQUAL(action_summands.size(), 1U);
  BOOST_CHECK_EQUAL(action_summands.front(), action_summands1.front());
  BOOST_CHECK(deadlock_summands.empty());
}

// With time every summand is restricted by the ultimate delay; a deadlock summand that is subsumed by an action
// summand of the result is left out, and summands with condition false are left out.
BOOST_AUTO_TEST_CASE(test_left_merge_with_time)
{
  data::set_identifier_generator id_generator = make_identifier_generator();
  const process::action a = make_action("a");
  const process::action termination = make_action("Terminate");
  const stochastic_action_summand_vector action_summands1{make_summand({a}), make_summand({termination})};
  const deadlock_summand_vector deadlock_summands1{
    deadlock_summand(data::variable_list(), data::sort_bool::true_(), deadlock(five)),
    deadlock_summand(data::variable_list(), data::sort_bool::false_(), deadlock())};

  stochastic_action_summand_vector action_summands{stochastic_action_summand(data::variable_list(),
    data::sort_bool::true_(),
    multi_action(process::action_list({a}), five),
    data::assignment_list(),
    no_distribution())};
  deadlock_summand_vector deadlock_summands;
  left_merge(action_summands1,
    deadlock_summands1,
    delay_until_three(),
    allow_block_filter(),
    termination,
    false,
    id_generator,
    identity,
    action_summands,
    deadlock_summands);

  // The deadlock at 5 with condition 5 < 3 is subsumed by the action a at 5 with condition true; the deadlock with
  // condition false is left out. Of the action summands only a takes part, now at time t with condition t < 3.
  BOOST_CHECK(deadlock_summands.empty());
  BOOST_REQUIRE_EQUAL(action_summands.size(), 2U);
  BOOST_CHECK_EQUAL(action_summands.back().multi_action().time(), t);
  BOOST_CHECK_EQUAL(action_summands.back().condition(), data::less(t, three));
}

// The termination action only combines with the termination action; other multi-actions are joined.
BOOST_AUTO_TEST_CASE(test_communication_merge_actions)
{
  const process::action a = make_action("a");
  const process::action b = make_action("b");
  const process::action termination = make_action("Terminate");
  const process::action_list terminate({termination});

  BOOST_CHECK(!communication_merge_actions({a}, terminate, termination));
  BOOST_CHECK(!communication_merge_actions(terminate, {a}, termination));
  BOOST_CHECK_EQUAL(communication_merge_actions(terminate, terminate, termination).value(), terminate);
  const process::action_list ab = communication_merge_actions({b}, {a}, termination).value();
  BOOST_CHECK_EQUAL(ab, process::action_list({a, b}));
  BOOST_CHECK(std::is_sorted(ab.begin(), ab.end()));
}

// Two timed action summands must happen at the same time; the result has the time of the first, and joins the sum
// variables, conditions, next states and distributions.
BOOST_AUTO_TEST_CASE(test_communication_merge_action_summands)
{
  const process::action a = make_action("a");
  const process::action b = make_action("b");
  const data::variable x("x", data::sort_bool::bool_());
  const data::data_expression condition1 = data::less(n, data::sort_nat::nat(2));
  const stochastic_action_summand summand1({u},
    condition1,
    multi_action(process::action_list({a}), u),
    data::assignment_list({data::assignment(n, data::sort_nat::nat(1))}),
    stochastic_distribution({x}, data::sort_real::real_(1)));
  const stochastic_action_summand summand2(data::variable_list(),
    data::sort_bool::true_(),
    multi_action(process::action_list({b}), five),
    data::assignment_list(),
    no_distribution());

  const process::action termination = make_action("Terminate");
  const process::action_list ab({a, b});
  const multi_action action
    = communication_merge_multi_action(summand1.multi_action(), summand2.multi_action(), termination).value();
  BOOST_CHECK_EQUAL(action, multi_action(ab, u));
  const stochastic_action_summand result = communication_merge_summand(summand1, summand2, action, identity);
  BOOST_CHECK_EQUAL(result.summation_variables(), data::variable_list({u}));
  BOOST_CHECK_EQUAL(result.condition(), data::sort_bool::and_(condition1, data::equal_to(u, five)));
  BOOST_CHECK_EQUAL(result.multi_action(), action);
  BOOST_CHECK_EQUAL(result.assignments(), summand1.assignments());
  BOOST_CHECK_EQUAL(result.distribution().variables(), data::variable_list({x}));

  // With only one timed summand the result has its time and no extra condition.
  const stochastic_action_summand untimed = make_summand({a});
  const multi_action action2
    = communication_merge_multi_action(untimed.multi_action(), summand2.multi_action(), termination).value();
  BOOST_CHECK_EQUAL(action2, multi_action(ab, five));
  const stochastic_action_summand result2 = communication_merge_summand(untimed, summand2, action2, identity);
  BOOST_CHECK_EQUAL(result2.condition(), data::sort_bool::true_());
  BOOST_CHECK_EQUAL(result2.multi_action(), action2);

  // Without times the result has no time.
  const multi_action action3
    = communication_merge_multi_action(untimed.multi_action(), make_summand({b}).multi_action(), termination).value();
  BOOST_CHECK(!action3.has_time());
  BOOST_CHECK(!communication_merge_multi_action(untimed.multi_action(),
    multi_action(process::action_list({termination})),
    termination));
}

// A combination with a deadlock summand can delay until the earliest of both times.
BOOST_AUTO_TEST_CASE(test_communication_merge_deadlock_summands)
{
  const deadlock_summand timed(data::variable_list({u}), data::sort_bool::true_(), deadlock(five));
  const deadlock_summand untimed(data::variable_list(), data::sort_bool::true_(), deadlock());
  const stochastic_action_summand action_at_three(data::variable_list(),
    data::sort_bool::true_(),
    multi_action(process::action_list({make_action("a")}), three),
    data::assignment_list(),
    no_distribution());

  const deadlock_summand both = communication_merge_summand(action_at_three, timed, identity);
  BOOST_CHECK_EQUAL(both.summation_variables(), data::variable_list({u}));
  BOOST_CHECK_EQUAL(both.deadlock().time(), data::sort_real::minimum(three, five));

  BOOST_CHECK_EQUAL(communication_merge_summand(untimed, timed, identity).deadlock().time(), five);
  BOOST_CHECK_EQUAL(communication_merge_summand(timed, untimed, identity).deadlock().time(), five);
  BOOST_CHECK(!communication_merge_summand(untimed, untimed, identity).deadlock().has_time());

  // With condition false the time is not computed.
  const deadlock_summand never(data::variable_list(), data::sort_bool::false_(), deadlock(three));
  const deadlock_summand none = communication_merge_summand(never, timed, identity);
  BOOST_CHECK_EQUAL(none.condition(), data::sort_bool::false_());
  BOOST_CHECK(!none.deadlock().has_time());
}

// Without time only action summands are combined, the filter is applied, and the termination action only combines
// with the termination action.
BOOST_AUTO_TEST_CASE(test_communication_merge_without_time)
{
  const process::action a = make_action("a");
  const process::action b = make_action("b");
  const process::action termination = make_action("Terminate");
  const stochastic_action_summand_vector action_summands1{make_summand({a}), make_summand({termination})};
  const stochastic_action_summand_vector action_summands2{make_summand({b}),
    make_summand({a}),
    make_summand({termination})};
  const deadlock_summand_vector deadlock_summands{
    deadlock_summand(data::variable_list(), data::sort_bool::true_(), deadlock())};

  // block({a}) leaves a|b and a|a out.
  const process::action_name_multiset_list blocklist(
    {process::action_name_multiset(core::identifier_string_list({core::identifier_string("a")}))});
  const allow_block_filter filter(blocklist, false, true, termination);

  stochastic_action_summand_vector action_summands;
  deadlock_summand_vector result_deadlock_summands;
  communication_merge(action_summands1,
    deadlock_summands,
    action_summands2,
    deadlock_summands,
    filter,
    termination,
    true,
    identity,
    action_summands,
    result_deadlock_summands);

  BOOST_REQUIRE_EQUAL(action_summands.size(), 1U);
  BOOST_CHECK_EQUAL(action_summands.front().multi_action().actions(), process::action_list({termination}));
  BOOST_CHECK(result_deadlock_summands.empty());

  // Without the filter also a|b and a|a are there.
  action_summands.clear();
  communication_merge(action_summands1,
    deadlock_summands,
    action_summands2,
    deadlock_summands,
    allow_block_filter(),
    termination,
    true,
    identity,
    action_summands,
    result_deadlock_summands);
  BOOST_CHECK_EQUAL(action_summands.size(), 3U);
  BOOST_CHECK(result_deadlock_summands.empty());
}

// With time the combinations with deadlock summands are added as well, unless they are subsumed.
BOOST_AUTO_TEST_CASE(test_communication_merge_with_time)
{
  const process::action a = make_action("a");
  const process::action termination = make_action("Terminate");
  const data::data_expression condition = data::less(n, data::sort_nat::nat(2));
  const stochastic_action_summand_vector action_summands1{make_summand({a}, {}, condition)};
  const deadlock_summand_vector deadlock_summands1;
  const stochastic_action_summand_vector action_summands2;
  const deadlock_summand_vector deadlock_summands2{
    deadlock_summand(data::variable_list(), data::sort_bool::true_(), deadlock(five))};

  stochastic_action_summand_vector action_summands;
  deadlock_summand_vector deadlock_summands;
  communication_merge(action_summands1,
    deadlock_summands1,
    action_summands2,
    deadlock_summands2,
    allow_block_filter(),
    termination,
    false,
    identity,
    action_summands,
    deadlock_summands);

  BOOST_CHECK(action_summands.empty());
  BOOST_REQUIRE_EQUAL(deadlock_summands.size(), 1U);
  BOOST_CHECK_EQUAL(deadlock_summands.front().condition(), condition);
  BOOST_CHECK_EQUAL(deadlock_summands.front().deadlock().time(), five);
}

namespace
{

// A transition from a state with a multi-action to a distribution over states, as in cluster_test.cpp.
using transition = std::tuple<state, multi_action, std::vector<std::pair<data::data_expression, state>>>;

// The transitions of the reachable state space of an untimed linear process.
std::set<transition> transitions(const stochastic_specification& spec)
{
  explorer_options options;
  options.search_strategy = exploration_strategy::es_breadth;
  options.suppress_progress_messages = true;
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

// The state of the parallel composition with the values of s1 followed by those of s2.
state join(const state& s1, const state& s2)
{
  std::vector<data::data_expression> values(s1.begin(), s1.end());
  values.insert(values.end(), s2.begin(), s2.end());
  return state(values.begin(), values.size());
}

// The single target state of a transition without stochastic choice, and its probability.
const std::pair<data::data_expression, state>& target(const transition& t)
{
  BOOST_REQUIRE_EQUAL(std::get<2>(t).size(), 1U);
  return std::get<2>(t).front();
}

// The transitions that are reachable from init.
std::set<transition> reachable(const std::set<transition>& transitions, const state& init)
{
  std::set<state> visited{init};
  std::vector<state> todo{init};
  std::set<transition> result;
  while (!todo.empty())
  {
    const state s = todo.back();
    todo.pop_back();
    for (const transition& t: transitions)
    {
      if (std::get<0>(t) == s)
      {
        result.insert(t);
        if (visited.insert(target(t).second).second)
        {
          todo.push_back(target(t).second);
        }
      }
    }
  }
  return result;
}

// The transitions of the parallel composition of two processes without shared parameters, computed from the
// transitions of both: every state of the first can do its transitions in every state of the second and vice
// versa, and both can do a transition together, with the union of the multi-actions. Only the multi-actions that
// keep is true for are kept, and only the transitions that are reachable from the initial state.
template<typename Keep>
std::set<transition> product(const std::set<transition>& transitions1,
  const state& init1,
  const std::set<transition>& transitions2,
  const state& init2,
  Keep keep)
{
  const auto states = [](const std::set<transition>& transitions, const state& init)
  {
    std::set<state> result{init};
    for (const transition& t: transitions)
    {
      result.insert(std::get<0>(t));
      result.insert(target(t).second);
    }
    return result;
  };
  const std::set<state> states1 = states(transitions1, init1);
  const std::set<state> states2 = states(transitions2, init2);

  std::set<transition> result;
  const auto add = [&](const state& s0, const multi_action& a, const data::data_expression& p, const state& s1)
  {
    if (keep(a))
    {
      result.emplace(s0, a, std::vector<std::pair<data::data_expression, state>>{{p, s1}});
    }
  };
  for (const transition& t1: transitions1)
  {
    for (const state& s2: states2)
    {
      add(join(std::get<0>(t1), s2), std::get<1>(t1), target(t1).first, join(target(t1).second, s2));
    }
  }
  for (const transition& t2: transitions2)
  {
    for (const state& s1: states1)
    {
      add(join(s1, std::get<0>(t2)), std::get<1>(t2), target(t2).first, join(s1, target(t2).second));
    }
  }
  for (const transition& t1: transitions1)
  {
    for (const transition& t2: transitions2)
    {
      add(join(std::get<0>(t1), std::get<0>(t2)),
        multi_action(std::get<1>(t1).actions() + std::get<1>(t2).actions()),
        target(t1).first,
        join(target(t1).second, target(t2).second));
    }
  }
  return reachable(result, join(init1, init2));
}

state initial_state(const stochastic_specification& spec)
{
  const data::data_expression_list& init = spec.initial_process().expressions();
  return state(init.begin(), init.size());
}

// The untimed parallel composition of two linear processes without stochastic operators, with the functions of the
// lineariser. Without time the ultimate delays are not used.
stochastic_specification
compose(const stochastic_specification& spec1, const stochastic_specification& spec2, const allow_block_filter& filter)
{
  data::set_identifier_generator id_generator;
  id_generator.add_identifiers(lps::find_identifiers(spec1));
  id_generator.add_identifiers(lps::find_identifiers(spec2));
  const data::rewriter r(spec1.data());
  const auto rewrite = [&](const data::data_expression& x)
  {
    return r(x);
  };

  stochastic_action_summand_vector action_summands;
  deadlock_summand_vector deadlock_summands;
  parallel_composition(spec1.process().action_summands(),
    spec1.process().deadlock_summands(),
    detail::ultimate_delay(),
    spec2.process().action_summands(),
    spec2.process().deadlock_summands(),
    detail::ultimate_delay(),
    filter,
    make_action("Terminate"),
    true,
    id_generator,
    rewrite,
    action_summands,
    deadlock_summands);

  const data::variable_list parameters
    = parallel_composition_parameters(spec1.process().process_parameters(), spec2.process().process_parameters());
  // Neither process is stochastic, so the initial distribution of the first is that of both.
  return stochastic_specification(spec1.data(),
    spec1.action_labels(),
    spec1.global_variables(),
    stochastic_linear_process(parameters, deadlock_summands, action_summands),
    stochastic_process_initializer(spec1.initial_process().expressions() + spec2.initial_process().expressions(),
      spec1.initial_process().distribution()));
}

// Parses a linear process without stochastic operators, and gives its action summands the distribution 1 over no
// variables, as the lineariser does (the parser leaves the distribution undefined).
stochastic_specification parse(const std::string& text)
{
  stochastic_specification result;
  parse_lps(text, result);
  for (stochastic_action_summand& summand: result.process().action_summands())
  {
    summand.distribution() = no_distribution();
  }
  return result;
}

const std::string process_p = "act a, b: Nat;\n"
                              "proc P(x: Nat) = (x < 2) -> a(x) . P(x = x + 1) + (x == 2) -> a(0) . P(x = 0);\n"
                              "init P(0);\n";
const std::string process_q = "act a, b: Nat;\n"
                              "proc Q(y: Nat) = sum n: Nat. (n < 2 && y < 1) -> b(n) . Q(y = y + n + 1);\n"
                              "init Q(0);\n";

} // namespace

// Without time the parallel composition has the transitions of the product of both processes. (With time the left
// merge adds a sum over a time variable of sort Real, which the explorer cannot enumerate; the timed summands are
// tested above.)
BOOST_AUTO_TEST_CASE(test_parallel_composition)
{
  const stochastic_specification p = parse(process_p);
  const stochastic_specification q = parse(process_q);
  const std::set<transition> expected = product(transitions(p),
    initial_state(p),
    transitions(q),
    initial_state(q),
    [](const multi_action&) { return true; });
  BOOST_CHECK(!expected.empty());
  const stochastic_specification pq = compose(p, q, allow_block_filter());
  BOOST_CHECK_EQUAL(pq.process().process_parameters().size(), 2U);
  BOOST_CHECK(transitions(pq) == expected);
}

// With an inline allow only the allowed multi-actions remain, and a deadlock summand true -> delta is added.
BOOST_AUTO_TEST_CASE(test_parallel_composition_with_allow)
{
  const stochastic_specification p = parse(process_p);
  const stochastic_specification q = parse(process_q);
  const process::action_name_multiset_list allowlist({process::action_name_multiset(
    core::identifier_string_list({core::identifier_string("a"), core::identifier_string("b")}))});
  const allow_block_filter filter(allowlist, true, false, make_action("Terminate"));

  const std::set<transition> expected = product(transitions(p),
    initial_state(p),
    transitions(q),
    initial_state(q),
    [&](const multi_action& a) { return filter.keeps(a); });
  BOOST_CHECK(!expected.empty());
  const stochastic_specification pq = compose(p, q, filter);
  BOOST_CHECK(transitions(pq) == expected);
  BOOST_REQUIRE_EQUAL(pq.process().deadlock_summands().size(), 1U);
  BOOST_CHECK_EQUAL(pq.process().deadlock_summands().front().condition(), data::sort_bool::true_());
}

// The parameters of the second process that are also parameters of the first occur only once.
BOOST_AUTO_TEST_CASE(test_parallel_composition_parameters)
{
  const data::variable x("x", data::sort_nat::nat());
  const data::variable y("y", data::sort_nat::nat());
  const data::variable z("z", data::sort_nat::nat());
  BOOST_CHECK_EQUAL(parallel_composition_parameters({x, y}, {z, y}), data::variable_list({x, y, z}));
  BOOST_CHECK_EQUAL(parallel_composition_parameters({}, {z, y}), data::variable_list({z, y}));
}
