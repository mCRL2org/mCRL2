// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file stochastic_variable_scope_test.cpp
/// \brief Tests that the binding-aware lps traversers and builders respect the scope of stochastic variables.
/// \details In a stochastic action summand sum d. c -> a . dist e[f] . P(g) the stochastic variables e are bound in
///          the distribution f and the next state g only, not in the condition c or the multi-action a. In the summand
///          below, the k in the condition and the action is the process parameter k, which has the same name and sort
///          as the stochastic variable k. All three test cases fail when the stochastic variables are bound in the
///          whole summand, as lps::add_data_variable_binding does.

#define BOOST_TEST_MODULE stochastic_variable_scope_test
#include "mcrl2/data/substitutions/mutable_map_substitution.h"
#include "mcrl2/lps/find.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/lps/print.h"
#include "mcrl2/lps/replace.h"

#include <boost/test/included/unit_test.hpp>

using namespace mcrl2;

namespace
{

lps::stochastic_specification parse_stochastic(const std::string& text)
{
  lps::stochastic_specification result;
  lps::parse_lps(text, result);
  return result;
}

const std::string SPEC1 = "act a: Bool;\n"
                          "proc P(k: Bool) = k -> a(k) . dist k: Bool[1 / 2] . P(k = !k);\n"
                          "init P(true);\n";

const std::string SPEC2 = "act a: Bool;\n"
                          "proc P(k1: Bool) = k1 -> a(k1) . dist k: Bool[1 / 2] . P(k1 = !k);\n"
                          "init P(true);\n";

const data::variable k("k", data::sort_bool::bool_());
const data::variable k1("k1", data::sort_bool::bool_());

} // namespace

BOOST_AUTO_TEST_CASE(test_find_free_variables1)
{
  const lps::stochastic_specification spec = parse_stochastic(SPEC1);
  const lps::stochastic_action_summand& summand = spec.process().action_summands().front();
  const std::set<data::variable> free = lps::find_free_variables(summand);
  BOOST_CHECK_MESSAGE(free == std::set<data::variable>{k},
    "find_free_variables(" << lps::pp(summand) << ") = " << core::detail::print_set(free) << ", expected { k }");
}

BOOST_AUTO_TEST_CASE(test_find_free_variables2)
{ 
  const lps::stochastic_specification spec = parse_stochastic(SPEC2);
  const lps::stochastic_action_summand& summand = spec.process().action_summands().front();
  const std::set<data::variable> free = lps::find_free_variables(summand);
  BOOST_CHECK_MESSAGE(free == std::set<data::variable>{k1},
    "find_free_variables(" << lps::pp(summand) << ") = " << core::detail::print_set(free) << ", expected {k1}");
}

BOOST_AUTO_TEST_CASE(test_search_free_variable1)
{
  const lps::stochastic_specification spec = parse_stochastic(SPEC1);
  const lps::stochastic_action_summand& summand = spec.process().action_summands().front();
  BOOST_CHECK_MESSAGE(lps::search_free_variable(summand, k),
    "search_free_variable(" << lps::pp(summand) << ", k) = false, expected true");
}

BOOST_AUTO_TEST_CASE(test_search_free_variable2)
{
  const lps::stochastic_specification spec = parse_stochastic(SPEC2);
  const lps::stochastic_action_summand& summand = spec.process().action_summands().front();
  BOOST_CHECK_MESSAGE(!lps::search_free_variable(summand, k),
    "search_free_variable(" << lps::pp(summand) << ", k) = true, expected false");
}

BOOST_AUTO_TEST_CASE(test_replace_free_variables1)
{
  const lps::stochastic_specification spec = parse_stochastic(SPEC1);
  lps::stochastic_action_summand summand = spec.process().action_summands().front();
  data::mutable_map_substitution<> sigma;
  sigma[k] = data::sort_bool::false_();
  lps::replace_free_variables(summand, sigma);

  const lps::stochastic_specification expected
    = parse_stochastic("act a: Bool;\n"
                       "proc P(k: Bool) = false -> a(false) . dist k: Bool[1 / 2] . P(k = !k);\n"
                       "init P(true);\n");
  BOOST_CHECK_EQUAL(lps::pp(summand), lps::pp(expected.process().action_summands().front()));
}

BOOST_AUTO_TEST_CASE(test_replace_free_variables2)
{
  const lps::stochastic_specification spec = parse_stochastic(SPEC2);
  lps::stochastic_action_summand summand = spec.process().action_summands().front();
  data::mutable_map_substitution<> sigma;
  sigma[k] = data::sort_bool::false_();
  lps::replace_free_variables(summand, sigma);

  const lps::stochastic_specification expected
    = parse_stochastic("act a: Bool;\n"
                       "proc P(k1: Bool) = k1 -> a(k1) . dist k: Bool[1 / 2] . P(k1 = !k);\n"
                       "init P(true);\n");
  BOOST_CHECK_EQUAL(lps::pp(summand), lps::pp(expected.process().action_summands().front()));
}

