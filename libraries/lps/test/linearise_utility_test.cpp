// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file linearise_utility_test.cpp
/// \brief Tests for the utilities of the lineariser.

#define BOOST_TEST_MODULE linearise_utility_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/lps/linearise_utility.h"
#include "mcrl2/process/print.h"

using namespace mcrl2;
using namespace mcrl2::process;

namespace
{

const data::variable b("b", data::sort_bool::bool_());
const data::variable n("n", data::sort_nat::nat());
const action_label a_label(core::identifier_string("a"), {});
const action_label c_label(core::identifier_string("c"), {data::sort_bool::bool_()});
const action a(a_label, {});
const action c_b(c_label, {b});
const process_expression delta0 = lps::delta_at_zero();

// The identity, as used by the lineariser when rewriting is switched off.
data::data_expression no_rewrite(const data::data_expression& x)
{
  return x;
}

// Rewrites b to true.
data::data_expression b_is_true(const data::data_expression& x)
{
  return x == b ? data::sort_bool::true_() : x;
}

void check(const process_expression& p,
  const process_expression& expected,
  data::data_expression (*R)(const data::data_expression&) = no_rewrite)
{
  const process_expression result = lps::simplify_pcrl(p, R);
  BOOST_CHECK_MESSAGE(result == expected,
    "simplify_pcrl(" << process::pp(p) << ") is " << process::pp(result) << ", expected " << process::pp(expected));
}

} // namespace

BOOST_AUTO_TEST_CASE(test_choice)
{
  check(choice(delta0, a), a);
  check(choice(a, delta0), a);
  check(choice(delta0, delta0), delta0);
  check(choice(a, c_b), choice(a, c_b));
  // Only delta@0 is removed, not delta or delta at another time.
  check(choice(delta(), a), choice(delta(), a));
  const process_expression delta1 = at(delta(), data::sort_real::real_(1));
  check(choice(delta1, a), choice(delta1, a));
}

BOOST_AUTO_TEST_CASE(test_seq)
{
  check(seq(delta0, a), delta0);
  // delta@0 at the end of a sequence is kept.
  check(seq(a, delta0), seq(a, delta0));
}

BOOST_AUTO_TEST_CASE(test_if_then)
{
  check(if_then(data::sort_bool::false_(), a), delta0);
  check(if_then(data::sort_bool::true_(), a), a);
  check(if_then(b, a), if_then(b, a));
  check(if_then(b, a), a, b_is_true);
}

BOOST_AUTO_TEST_CASE(test_if_then_else)
{
  check(if_then_else(data::sort_bool::false_(), a, c_b), c_b);
  check(if_then_else(data::sort_bool::true_(), a, c_b), a);
  check(if_then_else(b, a, c_b), if_then_else(b, a, c_b));
  check(if_then_else(b, a, c_b), a, b_is_true);
}

BOOST_AUTO_TEST_CASE(test_nested)
{
  // A condition that becomes false yields delta@0, which is then removed from the choice.
  check(choice(if_then(data::sort_bool::false_(), a), if_then(data::sort_bool::true_(), c_b)), c_b);
  check(seq(if_then(data::sort_bool::false_(), a), c_b), delta0);
  // Simplification takes place below other operators, but a sum over delta@0 is not removed.
  check(sum({n}, if_then(data::sort_bool::false_(), a)), sum({n}, delta0));
  check(choice(sum({n}, if_then(data::sort_bool::false_(), a)), a), choice(sum({n}, delta0), a));
  check(at(if_then(data::sort_bool::true_(), a), data::sort_real::real_(1)), at(a, data::sort_real::real_(1)));
}

BOOST_AUTO_TEST_CASE(test_only_conditions_are_rewritten)
{
  // The rewriter is applied to conditions only, not to action arguments.
  check(c_b, c_b, b_is_true);
  check(if_then(b, c_b), c_b, b_is_true);
}
