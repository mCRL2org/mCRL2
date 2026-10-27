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

#include <functional>

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

namespace
{

const data::variable x("x", data::sort_nat::nat());
const data::variable y("y", data::sort_nat::nat());
const data::variable z("z", data::sort_nat::nat());
const action_label d_label(core::identifier_string("d"), {data::sort_nat::nat(), data::sort_nat::nat()});

action d(const data::data_expression& e1, const data::data_expression& e2)
{
  return action(d_label, {e1, e2});
}

// Generates fresh names for the variables x, y and z, which do not clash with the names in these tests.
data::set_identifier_generator make_generator()
{
  data::set_identifier_generator result;
  result.add_identifiers({core::identifier_string("x"), core::identifier_string("y"), core::identifier_string("z")});
  return result;
}

} // namespace

BOOST_AUTO_TEST_CASE(test_rename_variables_away)
{
  data::mutable_map_substitution<> sigma;
  data::set_identifier_generator generator = make_generator();
  const std::function<data::variable(const data::variable&)> fresh = [&](const data::variable& v)
  {
    return data::variable(generator(v.name()), v.sort());
  };

  // Only the variables that satisfy the predicate are renamed, and the order is preserved.
  const data::variable_list result = lps::rename_variables_away(
    data::variable_list({x, y, z}),
    [](const data::variable& v) { return v == y; },
    sigma,
    fresh);
  const data::variable y1 = atermpp::down_cast<data::variable>(sigma(y));
  BOOST_CHECK(y1 != y);
  BOOST_CHECK(y1.sort() == y.sort());
  BOOST_CHECK(result == data::variable_list({x, y1, z}));
  BOOST_CHECK(sigma(x) == x);
  BOOST_CHECK(sigma(z) == z);

  // Nothing is renamed if no variable satisfies the predicate.
  data::mutable_map_substitution<> sigma2;
  const data::variable_list result2 = lps::rename_variables_away(
    data::variable_list({x, z}),
    [](const data::variable&) { return false; },
    sigma2,
    fresh);
  BOOST_CHECK(result2 == data::variable_list({x, z}));
  BOOST_CHECK(sigma2.empty());
}

BOOST_AUTO_TEST_CASE(test_rename_bound_variables_away_sum)
{
  data::set_identifier_generator generator = make_generator();

  // sum x, y. d(x, y): x must be renamed away, y not.
  const sum s({x, y}, d(x, y));
  const sum result = lps::rename_bound_variables_away(s, [](const data::variable& v) { return v == x; }, generator);
  BOOST_REQUIRE_EQUAL(result.variables().size(), 2u);
  const data::variable x1 = result.variables().front();
  BOOST_CHECK(x1 != x);
  BOOST_CHECK(result.variables().tail().front() == y);
  BOOST_CHECK(result.operand() == process_expression(d(x1, y)));

  // The free variable z in the operand is not affected, even when it is avoided.
  const sum s2({x}, d(x, z));
  const sum result2
    = lps::rename_bound_variables_away(s2, [](const data::variable& v) { return v == x || v == z; }, generator);
  const data::variable x2 = result2.variables().front();
  BOOST_CHECK(x2 != x);
  BOOST_CHECK(result2.operand() == process_expression(d(x2, z)));
}

BOOST_AUTO_TEST_CASE(test_rename_bound_variables_away_sum_nested)
{
  data::set_identifier_generator generator = make_generator();

  // sum x. sum x. d(x, x): only the outer x is renamed; the inner sum still binds the occurrences of x.
  const sum s({x}, sum({x}, d(x, x)));
  const sum result = lps::rename_bound_variables_away(s, [](const data::variable& v) { return v == x; }, generator);
  const data::variable x1 = result.variables().front();
  BOOST_CHECK(x1 != x);
  BOOST_CHECK(result.operand() == process_expression(sum({x}, d(x, x))));
}

// The stochastic operator dist x[f(x)]. p: renaming x must also rename x in the distribution f(x). Before step 2b-3
// of the refactoring, putbehind renamed the variables of a stochastic operator but not its distribution.
BOOST_AUTO_TEST_CASE(test_rename_bound_variables_away_stochastic_operator)
{
  data::set_identifier_generator generator = make_generator();

  const data::data_expression distribution
    = data::if_(data::equal_to(x, data::sort_nat::nat(0)), data::sort_real::real_(1), data::sort_real::real_(0));
  const stochastic_operator s({x}, distribution, d(x, y));
  const stochastic_operator result
    = lps::rename_bound_variables_away(s, [](const data::variable& v) { return v == x; }, generator);
  const data::variable x1 = result.variables().front();
  BOOST_CHECK(x1 != x);
  BOOST_CHECK(
    result.distribution()
    == data::if_(data::equal_to(x1, data::sort_nat::nat(0)), data::sort_real::real_(1), data::sort_real::real_(0)));
  BOOST_CHECK(result.operand() == process_expression(d(x1, y)));
}

// move_into_binder: the context d(x, z) with x free is moved into sum x, y. d(x, y). The bound x is renamed, the
// bound y is not, and f receives the variables and operand after renaming.
BOOST_AUTO_TEST_CASE(test_move_into_binder_sum)
{
  data::set_identifier_generator generator = make_generator();
  const process_expression context = d(x, z);
  data::variable_list variables_seen_by_f;

  const process_expression result = lps::move_into_binder(
    sum({x, y}, d(x, y)),
    [](const data::variable& v) { return v == x || v == z; },
    [&](const data::variable_list& variables, const process_expression& operand)
    {
      variables_seen_by_f = variables;
      return process_expression(seq(operand, context));
    },
    generator);
  BOOST_REQUIRE(is_sum(result));
  const sum& s = atermpp::down_cast<sum>(result);
  BOOST_REQUIRE_EQUAL(s.variables().size(), 2u);
  const data::variable x1 = s.variables().front();
  BOOST_CHECK(x1 != x);
  BOOST_CHECK(s.variables().tail().front() == y);
  BOOST_CHECK(variables_seen_by_f == s.variables());
  BOOST_CHECK(s.operand() == process_expression(seq(d(x1, y), context)));
}

// move_into_binder: for a stochastic operator the distribution is renamed along with the operand and kept.
BOOST_AUTO_TEST_CASE(test_move_into_binder_stochastic_operator)
{
  data::set_identifier_generator generator = make_generator();
  const process_expression context = d(x, z);
  const data::data_expression distribution
    = data::if_(data::equal_to(x, data::sort_nat::nat(0)), data::sort_real::real_(1), data::sort_real::real_(0));

  const process_expression result = lps::move_into_binder(
    stochastic_operator({x}, distribution, d(x, y)),
    [](const data::variable& v) { return v == x; },
    [&](const data::variable_list&, const process_expression& operand)
    { return process_expression(seq(operand, context)); },
    generator);
  BOOST_REQUIRE(is_stochastic_operator(result));
  const stochastic_operator& s = atermpp::down_cast<stochastic_operator>(result);
  const data::variable x1 = s.variables().front();
  BOOST_CHECK(x1 != x);
  BOOST_CHECK(
    s.distribution()
    == data::if_(data::equal_to(x1, data::sort_nat::nat(0)), data::sort_real::real_(1), data::sort_real::real_(0)));
  BOOST_CHECK(s.operand() == process_expression(seq(d(x1, y), context)));
}

// for_each_process_identifier visits the process instances from left to right, every occurrence.
BOOST_AUTO_TEST_CASE(test_for_each_process_identifier)
{
  const process_identifier P(core::identifier_string("P"), {});
  const process_identifier Q(core::identifier_string("Q"), {});
  const process_identifier R(core::identifier_string("R"), {n});
  const process_expression p = choice(seq(a, process_instance_assignment(Q, {})),
    if_then_else(b,
      sum({x}, process_instance_assignment(R, {data::assignment(n, x)})),
      seq(process_instance_assignment(P, {}), process_instance_assignment(Q, {}))));

  std::vector<process_identifier> visited;
  lps::for_each_process_identifier(p, [&](const process_identifier& id) { visited.push_back(id); });
  BOOST_CHECK(visited == std::vector<process_identifier>({Q, R, P, Q}));
}

namespace
{

const process_identifier P_xy(core::identifier_string("P"), {x, y});

process_instance P(const data::data_expression& e1, const data::data_expression& e2)
{
  return process_instance(P_xy, {e1, e2});
}

process_instance_assignment P_assignment(const data::assignment_list& assignments)
{
  return process_instance_assignment(P_xy, assignments);
}

const data::data_expression three = data::sort_nat::nat(3);

} // namespace

// P(x, 3) becomes P(y = 3): the assignment x = x is left out, unless it must be kept.
BOOST_AUTO_TEST_CASE(test_to_process_instance_assignment)
{
  BOOST_CHECK(lps::to_process_instance_assignment(P(x, three), [](const data::variable&) { return false; })
              == P_assignment({data::assignment(y, three)}));
  BOOST_CHECK(lps::to_process_instance_assignment(P(x, three), [](const data::variable& v) { return v == x; })
              == P_assignment({data::assignment(x, x), data::assignment(y, three)}));
  // An argument that is another parameter is not left out.
  BOOST_CHECK(lps::to_process_instance_assignment(P(y, x), [](const data::variable&) { return false; })
              == P_assignment({data::assignment(x, y), data::assignment(y, x)}));
}

// Process instances become process instance assignments, keeping x = x where x is bound by an enclosing sum, and f is
// applied to every process instance assignment from left to right.
BOOST_AUTO_TEST_CASE(test_replace_process_instance_assignments)
{
  const process_expression p
    = choice(P(x, three), seq(sum({x}, P(x, y)), P_assignment({data::assignment(y, three), data::assignment(x, z)})));
  std::vector<process_instance_assignment> visited;
  const process_expression result = lps::replace_process_instance_assignments(p,
    [&](const process_instance_assignment& a)
    {
      visited.push_back(a);
      return a;
    });
  const process_instance_assignment a1 = P_assignment({data::assignment(y, three)});
  const process_instance_assignment a2 = P_assignment({data::assignment(x, x)});
  // f receives an existing process instance assignment as it is; the lineariser sorts its assignments.
  const process_instance_assignment a3 = P_assignment({data::assignment(y, three), data::assignment(x, z)});
  BOOST_CHECK(result == choice(a1, seq(sum({x}, a2), a3)));
  BOOST_CHECK(visited == std::vector<process_instance_assignment>({a1, a2, a3}));

  // x = x is kept only within the scope of the sum.
  const process_expression q = seq(sum({x}, P(x, x)), P(x, x));
  BOOST_CHECK(lps::replace_process_instance_assignments(q, [](const process_instance_assignment& a) { return a; })
              == seq(sum({x}, P_assignment({data::assignment(x, x), data::assignment(y, x)})),
                P_assignment({data::assignment(y, x)})));

  // A variable bound by a stochastic operator is not kept; only sum variables are.
  const data::data_expression distribution = data::sort_real::real_(1);
  BOOST_CHECK(lps::replace_process_instance_assignments(stochastic_operator({x}, distribution, P(x, three)),
                [](const process_instance_assignment& a) { return a; })
              == stochastic_operator({x}, distribution, P_assignment({data::assignment(y, three)})));
}

// Directly nested stochastic operators are joined into one, also below other operators; stochastic operators that are
// separated by another operator are not.
BOOST_AUTO_TEST_CASE(test_join_nested_stochastic_operators)
{
  data::set_identifier_generator generator = make_generator();
  const data::data_expression f
    = data::if_(data::equal_to(x, data::sort_nat::nat(0)), data::sort_real::real_(1), data::sort_real::real_(0));
  const data::data_expression g = data::if_(data::equal_to(y, x), data::sort_real::real_(1), data::sort_real::real_(0));
  const data::data_expression h = data::sort_real::real_(1);
  const process_expression p = stochastic_operator({x}, f, stochastic_operator({y}, g, stochastic_operator({z}, h, a)));
  const process_expression joined
    = stochastic_operator({x, y, z}, data::sort_real::times(data::sort_real::times(f, g), h), a);
  BOOST_CHECK(lps::join_nested_stochastic_operators(p, generator) == joined);
  BOOST_CHECK(lps::join_nested_stochastic_operators(choice(a, p), generator) == choice(a, joined));

  const process_expression q = stochastic_operator({x}, f, seq(a, stochastic_operator({y}, g, d(x, y))));
  BOOST_CHECK(lps::join_nested_stochastic_operators(q, generator) == q);
}

// dist x[f]. dist x[g(x)]. d(x, y): the inner x is renamed to x1, as the outer x is not visible below the inner
// operator. Without renaming, the result bound x twice and f applied to the inner x.
BOOST_AUTO_TEST_CASE(test_join_nested_stochastic_operators_binding_the_same_variable)
{
  data::set_identifier_generator generator = make_generator();
  const data::data_expression f
    = data::if_(data::equal_to(x, data::sort_nat::nat(0)), data::sort_real::real_(1), data::sort_real::real_(0));
  const data::data_expression g
    = data::if_(data::equal_to(x, data::sort_nat::nat(1)), data::sort_real::real_(1), data::sort_real::real_(0));
  const process_expression result
    = lps::join_nested_stochastic_operators(stochastic_operator({x}, f, stochastic_operator({x}, g, d(x, y))),
      generator);
  BOOST_REQUIRE(is_stochastic_operator(result));
  const stochastic_operator& s = atermpp::down_cast<stochastic_operator>(result);
  BOOST_REQUIRE_EQUAL(s.variables().size(), 2u);
  const data::variable x1 = s.variables().tail().front();
  BOOST_CHECK(s.variables().front() == x);
  BOOST_CHECK(x1 != x);
  BOOST_CHECK(
    s.distribution()
    == data::sort_real::times(f,
      data::if_(data::equal_to(x1, data::sort_nat::nat(1)), data::sort_real::real_(1), data::sort_real::real_(0))));
  BOOST_CHECK(s.operand() == process_expression(d(x1, y)));
}

// dist x[f(x, y)]. dist y[g]. d(x, y), where y is free in f: the inner y is renamed, so that it does not capture the y
// in f. Without renaming, f referred to the inner y.
BOOST_AUTO_TEST_CASE(test_join_nested_stochastic_operators_capturing_a_free_variable)
{
  data::set_identifier_generator generator = make_generator();
  const data::data_expression f = data::if_(data::equal_to(x, y), data::sort_real::real_(1), data::sort_real::real_(0));
  const data::data_expression g = data::sort_real::real_(1);
  const process_expression result
    = lps::join_nested_stochastic_operators(stochastic_operator({x}, f, stochastic_operator({y}, g, d(x, y))),
      generator);
  BOOST_REQUIRE(is_stochastic_operator(result));
  const stochastic_operator& s = atermpp::down_cast<stochastic_operator>(result);
  BOOST_REQUIRE_EQUAL(s.variables().size(), 2u);
  const data::variable y1 = s.variables().tail().front();
  BOOST_CHECK(s.variables().front() == x);
  BOOST_CHECK(y1 != y);
  BOOST_CHECK(s.distribution() == data::sort_real::times(f, g));
  BOOST_CHECK(s.operand() == process_expression(d(x, y1)));
}
