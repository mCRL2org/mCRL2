// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file linearise_binder_test.cpp
/// \brief Tests that the lineariser keeps the scopes of variables bound by sum and dist intact when it moves a
///        context into the binder: a time stamp (wraptime), a sequential composition (putbehind) or a condition.

#define BOOST_TEST_MODULE linearise_binder_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/lps/find.h"
#include "mcrl2/lps/is_well_typed.h"
#include "mcrl2/lps/linearise.h"

using namespace mcrl2;
using namespace mcrl2::lps;

using check_function = std::function<void(const stochastic_specification&)>;

const std::vector<t_lin_method> all_methods
  = {t_lin_method::lmRegular, t_lin_method::lmRegular2, t_lin_method::lmStack};
// The stack method does not support stochastic operators ("Expect seq or name putting processes on a stack").
const std::vector<t_lin_method> stochastic_methods = {t_lin_method::lmRegular, t_lin_method::lmRegular2};

// Linearises spec with the given methods, with and without rewriting, and checks that the result has no free
// variables (other than global variables), is well typed, and satisfies check.
static void check_linearisation(
  const std::string& spec,
  const check_function& check = [](const stochastic_specification&) {},
  const std::vector<t_lin_method>& methods = all_methods)
{
  for (const t_lin_method method: methods)
  {
    for (const bool norewrite: {false, true})
    {
      t_lin_options options;
      options.lin_method = method;
      options.norewrite = norewrite;
      // Corresponds to mcrl22lps --no-rewrite, which also disables constelm.
      options.do_not_apply_constelm = norewrite;
      const stochastic_specification result = linearise(spec, options);
      BOOST_TEST_CONTEXT("method " << method << (norewrite ? ", no rewriting" : "") << ", result\n" << lps::pp(result))
      {
        std::set<data::variable> free = lps::find_free_variables(result);
        for (const data::variable& v: result.global_variables())
        {
          free.erase(v);
        }
        BOOST_CHECK_MESSAGE(free.empty(), "free variables " << core::detail::print_set(free));
        BOOST_CHECK(lps::detail::is_well_typed(result));
        check(result);
      }
    }
  }
}

// The time stamp of every action summand refers only to process parameters, not to summation variables.
static void check_time_without_summation_variables(const stochastic_specification& result)
{
  for (const stochastic_action_summand& s: result.process().action_summands())
  {
    BOOST_CHECK(s.multi_action().has_time());
    std::set<data::variable> V = data::find_free_variables(s.multi_action().time());
    for (const data::variable& v: s.summation_variables())
    {
      BOOST_CHECK_MESSAGE(V.find(v) == V.end(),
        "the time stamp of " << lps::pp(s) << " contains the summation variable " << data::pp(v));
    }
  }
}

// A non-trivial distribution depends on the variables it binds.
static void check_distribution(const stochastic_distribution& d)
{
  if (!d.variables().empty())
  {
    std::set<data::variable> V = data::find_free_variables(d.distribution());
    BOOST_CHECK_MESSAGE(std::any_of(d.variables().begin(),
                          d.variables().end(),
                          [&](const data::variable& v) { return V.find(v) != V.end(); }),
      "the distribution " << lps::pp(d) << " does not depend on its variables");
  }
}

// Every non-trivial distribution depends on the variables it binds.
static void check_distributions_use_their_variables(const stochastic_specification& result)
{
  for (const stochastic_action_summand& s: result.process().action_summands())
  {
    check_distribution(s.distribution());
  }
  check_distribution(result.initial_process().distribution());
}

// wraptime: in (sum t: Real. a(t)) @ t the time stamp t is the parameter, not the bound t. Moving the time stamp
// into the sum must rename the bound variable and leave the time stamp alone.
BOOST_AUTO_TEST_CASE(time_stamp_moved_into_sum)
{
  const std::string spec = "act a: Real;\n"
                           "proc P(t: Real) = (sum t: Real. a(t)) @ t . P(t + 1);\n"
                           "init P(1);\n";
  check_linearisation(spec, check_time_without_summation_variables);
}

// As above, with a condition on the bound variable and a time stamp that is an expression.
BOOST_AUTO_TEST_CASE(time_stamp_expression_moved_into_sum)
{
  const std::string spec = "act a: Real;\n"
                           "proc P(t: Real) = (sum t: Real. (t > 0) -> a(t)) @ (t + 1) . P(t + 1);\n"
                           "init P(1);\n";
  check_linearisation(spec, check_time_without_summation_variables);
}

// putbehind: the sequential composition (dist y. p) . q with y bound again in q.
BOOST_AUTO_TEST_CASE(sequence_behind_dist_with_sum)
{
  const std::string spec = "act a, b: Bool;\n"
                           "proc P = (dist y: Bool[if(y, 1/4, 3/4)] . a(y)) . (sum y: Bool. b(y)) . P;\n"
                           "init P;\n";
  check_linearisation(spec, check_distributions_use_their_variables, stochastic_methods);
}

// putbehind: the sequential composition (dist y. p) . (dist y. q).
BOOST_AUTO_TEST_CASE(sequence_behind_dist_with_dist)
{
  const std::string spec
    = "act a, b: Bool;\n"
      "proc P(x: Bool) = (dist y: Bool[if(y, 1/4, 3/4)] . a(y)) . (dist y: Bool[if(y, 1/3, 2/3)] . b(y)) . P(!x);\n"
      "init P(true);\n";
  check_linearisation(spec, check_distributions_use_their_variables, stochastic_methods);
}

// putbehind: a distribution in the middle of a sequence, followed by an action on a parameter.
BOOST_AUTO_TEST_CASE(dist_in_the_middle_of_a_sequence)
{
  const std::string spec = "act a, b: Bool;\n"
                           "proc P(x: Bool) = a(x) . (dist y: Bool[if(y, 1/4, 3/4)] . a(y)) . b(x) . P(!x);\n"
                           "init P(true);\n";
  check_linearisation(spec, check_distributions_use_their_variables, stochastic_methods);
}

// putbehind: (sum y. p) . q where q uses a parameter with the same name as the bound variable.
BOOST_AUTO_TEST_CASE(sequence_behind_sum)
{
  const std::string spec = "act a, b: Nat;\n"
                           "proc P(y: Nat) = (sum y: Nat. (y < 3) -> a(y)) . b(y) . P(y + 1);\n"
                           "init P(0);\n";
  check_linearisation(spec);
}

// distribute_condition: c -> (sum y. p) where the condition uses a parameter with the same name as the bound variable.
BOOST_AUTO_TEST_CASE(condition_moved_into_sum)
{
  const std::string spec = "act a: Nat;\n"
                           "proc P(y: Nat) = (y < 5) -> ((sum y: Nat. (y < 3) -> a(y)) . P(y + 1));\n"
                           "init P(0);\n";
  check_linearisation(spec);
}

// Every lambda in a condition has a free variable (the parameter n, or the global variable k after constelm), and
// there is at least one lambda. A lambda without free variables has captured the variable that replaced n.
static void check_lambdas_have_a_free_variable(const stochastic_specification& result)
{
  std::vector<atermpp::aterm> lambdas;
  for (const stochastic_action_summand& s: result.process().action_summands())
  {
    atermpp::find_all_if(s.condition(), data::is_lambda, std::back_inserter(lambdas));
  }
  for (const deadlock_summand& s: result.process().deadlock_summands())
  {
    atermpp::find_all_if(s.condition(), data::is_lambda, std::back_inserter(lambdas));
  }
  BOOST_CHECK(!lambdas.empty());
  for (const atermpp::aterm& l: lambdas)
  {
    BOOST_CHECK_MESSAGE(!data::find_free_variables(atermpp::down_cast<data::lambda>(l)).empty(),
      "a variable is captured in " << data::pp(atermpp::down_cast<data::lambda>(l)));
  }
}

// The global variable k is the initial value of the constant parameter n, so constelm substitutes n := k, also in the
// ultimate delay condition, which ends up in the condition of the timed summand of Q. The lambda binds a variable
// that is also called k, so this substitution must rename it. The mapping f has no equations, so the rewriter keeps
// the lambda.
BOOST_AUTO_TEST_CASE(global_variable_substituted_under_a_binder)
{
  const std::string spec = "map  f: (Pos -> Bool) -> Bool;\n"
                           "glob k: Pos;\n"
                           "act  a: Pos;\n"
                           "     b;\n"
                           "proc P(n: Pos) = f(lambda k: Pos. k == n) -> a(n) . P(n);\n"
                           "     Q = b @ 1 . Q;\n"
                           "init P(k) || Q;\n";
  check_linearisation(spec, check_lambdas_have_a_free_variable);
}
