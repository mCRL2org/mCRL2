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
#include "mcrl2/lps/linearise_parallel.h"

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
    stochastic_distribution());
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
    stochastic_distribution());
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
    stochastic_distribution())};
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
