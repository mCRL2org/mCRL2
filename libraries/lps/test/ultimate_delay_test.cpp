// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ultimate_delay_test.cpp
/// \brief Tests for the computation and combination of ultimate delays.

#define BOOST_TEST_MODULE ultimate_delay_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/substitutions/mutable_map_substitution.h"
#include "mcrl2/lps/detail/ultimate_delay.h"
#include "mcrl2/lps/find.h"
#include "mcrl2/lps/parse.h"

using namespace mcrl2;
using namespace mcrl2::lps;

static stochastic_specification parse(const std::string& text)
{
  stochastic_specification result;
  parse_lps(text, result);
  return result;
}

// The LPS parser drops the time of deadlock summands, so it is set afterwards.
static stochastic_specification parse_with_deadlock_time(const std::string& text, std::size_t time)
{
  stochastic_specification result = parse(text);
  BOOST_REQUIRE_EQUAL(result.process().deadlock_summands().size(), 1u);
  result.process().deadlock_summands().front().deadlock().time() = data::sort_real::real_(time);
  return result;
}

static data::set_identifier_generator identifier_generator(const stochastic_specification& spec)
{
  data::set_identifier_generator result;
  result.add_identifiers(lps::find_identifiers(spec));
  return result;
}

static detail::ultimate_delay compute(const stochastic_specification& spec,
  data::set_identifier_generator& id_generator)
{
  const data::rewriter r(spec.data());
  return detail::compute_ultimate_delay(spec.process().action_summands(),
    spec.process().deadlock_summands(),
    spec.data(),
    id_generator,
    [&](const data::data_expression& t) { return r(t); });
}

static detail::ultimate_delay compute(const stochastic_specification& spec)
{
  data::set_identifier_generator id_generator = identifier_generator(spec);
  return compute(spec, id_generator);
}

// Returns the value of the constraint of delay, which must not have variables, for the time t and the values of the
// process parameters in sigma.
static data::data_expression evaluate(const stochastic_specification& spec,
  const detail::ultimate_delay& delay,
  std::size_t t,
  data::mutable_map_substitution<> sigma = data::mutable_map_substitution<>())
{
  BOOST_CHECK(delay.variables().empty());
  sigma[delay.time_var()] = data::sort_real::real_(t);
  const data::rewriter r(spec.data());
  return r(data::replace_variables(delay.constraint(), sigma));
}

// An untimed summand with condition true can always wait.
BOOST_AUTO_TEST_CASE(test_untimed_true)
{
  const stochastic_specification spec = parse("act a;\n"
                                              "proc P(n: Nat) = (n < 3) -> a @ 2 . P(n = n + 1) + a . P(n = n);\n"
                                              "init P(0);\n");
  const detail::ultimate_delay delay = compute(spec);
  BOOST_CHECK_EQUAL(delay.constraint(), data::sort_bool::true_());
  BOOST_CHECK(delay.variables().empty());
  BOOST_CHECK_EQUAL(delay.time_var().sort(), data::sort_real::real_());
}

// A process can wait until before the latest time at which a summand is enabled.
BOOST_AUTO_TEST_CASE(test_timed)
{
  const stochastic_specification spec
    = parse_with_deadlock_time("act a;\n"
                               "proc P(n: Nat) = (n < 3) -> a @ 2 . P(n = n + 1) + (n < 5) -> delta @ 4;\n"
                               "init P(0);\n",
      4);
  const detail::ultimate_delay delay = compute(spec);
  const data::variable n = spec.process().process_parameters().front();
  data::mutable_map_substitution<> sigma;
  sigma[n] = data::sort_nat::nat(0);
  BOOST_CHECK_EQUAL(evaluate(spec, delay, 1, sigma), data::sort_bool::true_());
  BOOST_CHECK_EQUAL(evaluate(spec, delay, 3, sigma), data::sort_bool::true_());
  BOOST_CHECK_EQUAL(evaluate(spec, delay, 4, sigma), data::sort_bool::false_());
  sigma[n] = data::sort_nat::nat(3);
  BOOST_CHECK_EQUAL(evaluate(spec, delay, 1, sigma), data::sort_bool::true_());
  BOOST_CHECK_EQUAL(evaluate(spec, delay, 3, sigma), data::sort_bool::true_());
  sigma[n] = data::sort_nat::nat(5);
  BOOST_CHECK_EQUAL(evaluate(spec, delay, 1, sigma), data::sort_bool::false_());
}

// A summand whose time is a sum variable that does not occur in the condition can happen at any time, so only its
// condition counts.
BOOST_AUTO_TEST_CASE(test_free_time)
{
  const stochastic_specification spec
    = parse_with_deadlock_time("act a;\n"
                               "proc P(n: Nat) = sum u: Real. (n < 3) -> a @ u . P(n = n + 1) + delta @ 4;\n"
                               "init P(0);\n",
      4);
  const detail::ultimate_delay delay = compute(spec);
  const data::variable n = spec.process().process_parameters().front();
  data::mutable_map_substitution<> sigma;
  sigma[n] = data::sort_nat::nat(0);
  BOOST_CHECK_EQUAL(evaluate(spec, delay, 10, sigma), data::sort_bool::true_());
  sigma[n] = data::sort_nat::nat(3);
  BOOST_CHECK_EQUAL(evaluate(spec, delay, 10, sigma), data::sort_bool::false_());
  BOOST_CHECK_EQUAL(evaluate(spec, delay, 3, sigma), data::sort_bool::true_());
}

// The sum variables that occur in the conditions become the variables of the ultimate delay; variables of the same
// sort of different summands are merged, and the time variable is fresh.
BOOST_AUTO_TEST_CASE(test_sum_variables)
{
  const stochastic_specification spec = parse(
    "act a, b;\n"
    "proc P(timevar: Nat) = sum u: Real. (u < 3) -> a @ u . P() + sum v: Real, m: Nat. (v < m) -> b @ v . P();\n"
    "init P(0);\n");
  data::set_identifier_generator id_generator = identifier_generator(spec);
  const detail::ultimate_delay delay = compute(spec, id_generator);
  BOOST_CHECK_NE(delay.time_var().name(), core::identifier_string("timevar"));
  BOOST_CHECK_EQUAL(delay.variables().size(), 2u);
  std::set<data::variable> allowed(delay.variables().begin(), delay.variables().end());
  allowed.insert(delay.time_var());
  for (const data::variable& v: data::find_free_variables(delay.constraint()))
  {
    BOOST_CHECK_MESSAGE(allowed.find(v) != allowed.end(), "unexpected free variable " << v);
  }
}

// Combining renames the variables of the second delay that clash with those of the first, and replaces its time
// variable by that of the first.
BOOST_AUTO_TEST_CASE(test_combine)
{
  const data::variable t1("t1", data::sort_real::real_());
  const data::variable t2("t2", data::sort_real::real_());
  const data::variable u("u", data::sort_real::real_());
  const data::data_expression three = data::sort_real::real_(3);
  const detail::ultimate_delay delay1(t1, {u}, data::sort_bool::and_(data::less(t1, u), data::less(u, three)));
  const detail::ultimate_delay delay2(t2, {u}, data::less(t2, u));

  data::set_identifier_generator id_generator;
  id_generator.add_identifier(core::identifier_string("t1"));
  id_generator.add_identifier(core::identifier_string("t2"));
  id_generator.add_identifier(core::identifier_string("u"));
  const detail::ultimate_delay combined = detail::combine_ultimate_delays(delay1, delay2, id_generator);

  BOOST_CHECK_EQUAL(combined.time_var(), t1);
  BOOST_REQUIRE_EQUAL(combined.variables().size(), 2u);
  BOOST_CHECK_EQUAL(combined.variables().front(), u);
  const data::variable u2 = *std::next(combined.variables().begin());
  BOOST_CHECK_NE(u2, u);
  BOOST_CHECK_EQUAL(u2.sort(), data::sort_real::real_());
  BOOST_CHECK_EQUAL(combined.constraint(), data::sort_bool::and_(delay1.constraint(), data::less(t1, u2)));
}
