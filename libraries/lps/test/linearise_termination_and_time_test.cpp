// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file linearise_termination_and_time_test.cpp
/// \brief Tests that the lineariser determines whether processes can terminate and whether they contain time by a
///        fixpoint over mutually recursive processes. In X = a.Y, Y = b.X + c only Y has a direct way to terminate (or
///        to contain time); a single evaluation of X before Y misses it. Every specification is given twice, with the
///        names swapped, so that the tests do not depend on the order in which the processes are evaluated.

#define BOOST_TEST_MODULE linearise_termination_and_time_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/lps/linearise.h"

using namespace mcrl2;
using namespace mcrl2::lps;

const std::vector<t_lin_method> all_methods
  = {t_lin_method::lmRegular, t_lin_method::lmRegular2, t_lin_method::lmStack};

// Linearises spec with all methods and checks the result with check.
static void check_linearisation(const std::string& spec,
  const std::function<void(const stochastic_specification&)>& check,
  const bool ignore_time = true)
{
  for (const t_lin_method method: all_methods)
  {
    t_lin_options options;
    options.lin_method = method;
    options.ignore_time = ignore_time;
    const stochastic_specification result = linearise(spec, options);
    BOOST_TEST_CONTEXT("method " << method << ", specification\n" << spec << "result\n" << lps::pp(result))
    {
      check(result);
    }
  }
}

// Whether an action with the given name occurs in an action summand of the result.
static bool has_action(const stochastic_specification& result, const std::string& name)
{
  const stochastic_action_summand_vector& summands = result.process().action_summands();
  return std::any_of(summands.begin(),
    summands.end(),
    [&](const stochastic_action_summand& s)
    {
      const process::action_list& actions = s.multi_action().actions();
      return std::any_of(actions.begin(),
        actions.end(),
        [&](const process::action& a) { return std::string(a.label().name()) == name; });
    });
}

// Y can terminate, so X = Y . e can perform e after Y. The lineariser drops e if it concludes that Y cannot terminate.
BOOST_AUTO_TEST_CASE(sequential_composition_after_a_terminating_recursive_process)
{
  const std::function<void(const stochastic_specification&)> check = [](const stochastic_specification& result)
  {
    BOOST_CHECK(has_action(result, "e"));
  };
  check_linearisation("act a, b, c, e;\n"
                      "proc X = Y . e;\n"
                      "     Y = a . Z;\n"
                      "     Z = b . Y + c;\n"
                      "init X;\n",
    check);
  check_linearisation("act a, b, c, e;\n"
                      "proc X = Z . e;\n"
                      "     Z = a . Y;\n"
                      "     Y = b . Z + c;\n"
                      "init X;\n",
    check);
}

// X can terminate, so the parallel composition X || d terminates, which the lineariser shows with the action
// Terminate. It leaves out Terminate if it concludes that X cannot terminate.
BOOST_AUTO_TEST_CASE(parallel_composition_with_a_terminating_recursive_process)
{
  const std::function<void(const stochastic_specification&)> check = [](const stochastic_specification& result)
  {
    BOOST_CHECK(has_action(result, "Terminate"));
  };
  check_linearisation("act a, b, c, d;\n"
                      "proc X = a . Y;\n"
                      "     Y = b . X + c;\n"
                      "init X || d;\n",
    check);
  check_linearisation("act a, b, c, d;\n"
                      "proc Y = a . X;\n"
                      "     X = b . Y + c;\n"
                      "init Y || d;\n",
    check);
}

// X contains time, because it can reach c@1. The processes do not terminate, so only the time analysis is tested. In a
// timed linearisation the lineariser adds the summand true -> delta, which allows the process to idle forever, only if
// it concludes that the initial process does not contain time.
BOOST_AUTO_TEST_CASE(timed_recursive_process)
{
  const std::function<void(const stochastic_specification&)> check = [](const stochastic_specification& result)
  {
    const deadlock_summand_vector& summands = result.process().deadlock_summands();
    BOOST_CHECK(std::none_of(summands.begin(),
      summands.end(),
      [](const deadlock_summand& s) { return !s.deadlock().has_time() && s.condition() == data::sort_bool::true_(); }));
  };
  check_linearisation("act a, b, c;\n"
                      "proc X = a . Y;\n"
                      "     Y = b . X + c@1 . X;\n"
                      "init X;\n",
    check,
    false);
  check_linearisation("act a, b, c;\n"
                      "proc Y = a . X;\n"
                      "     X = b . Y + c@1 . Y;\n"
                      "init Y;\n",
    check,
    false);
}
