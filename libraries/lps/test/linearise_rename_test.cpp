// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file linearise_rename_test.cpp
/// \brief Test for applying rename operator

#define BOOST_TEST_MODULE linearise_rename_test
#include <boost/test/included/unit_test.hpp>

#include "../../process/include/mcrl2/process/rename_expression.h"
#include "../include/mcrl2/lps/linearise_rename.h"

using namespace mcrl2;
using namespace mcrl2::process;
using namespace mcrl2::lps;

struct LogDebug
{
  LogDebug()
  {
    log::logger::set_reporting_level(log::debug);
  }
};
BOOST_GLOBAL_FIXTURE(LogDebug);

inline
process::action make_action(const std::string& name, const data::data_expression_list& arguments = data::data_expression_list())
{
  const data::sort_expression_list sorts(
    arguments.begin(),
    arguments.end(),
    [](const data::data_expression& expression)
    {
      return expression.sort();
    });
  const action_label label(core::identifier_string(name), sorts);
  return process::action(label, arguments);
}

inline
rename_expression_list rename_rule_ab()
{
  return {rename_expression("a", "b")};
}

inline
rename_expression_list rename_rule_ad()
{
  return {rename_expression("a", "d")};
}

inline
rename_expression_list rename_rule_cd()
{
  return {rename_expression("c", "d")};
}

inline
rename_expression_list rename_rules_ab_cd()
{
  return {rename_expression("a", "b"), rename_expression("c", "d")};
}

BOOST_AUTO_TEST_CASE(test_rename_action)
{
  auto a = make_action("a");
  auto b = make_action("b");
  auto c = make_action("c");
  auto d = make_action("d");
  auto e = make_action("e");

  BOOST_CHECK_EQUAL(lps::rename(rename_rule_ab(), a), b);
  BOOST_CHECK_EQUAL(lps::rename(rename_rule_ab(), b), b);
  BOOST_CHECK_EQUAL(lps::rename(rename_rule_ab(), c), c);
  BOOST_CHECK_EQUAL(lps::rename(rename_rule_ab(), d), d);
  BOOST_CHECK_EQUAL(lps::rename(rename_rule_ab(), e), e);

  BOOST_CHECK_EQUAL(lps::rename(rename_rule_cd(), a), a);
  BOOST_CHECK_EQUAL(lps::rename(rename_rule_cd(), b), b);
  BOOST_CHECK_EQUAL(lps::rename(rename_rule_cd(), c), d);
  BOOST_CHECK_EQUAL(lps::rename(rename_rule_cd(), d), d);
  BOOST_CHECK_EQUAL(lps::rename(rename_rule_cd(), e), e);

  BOOST_CHECK_EQUAL(lps::rename(rename_rules_ab_cd(), a), b);
  BOOST_CHECK_EQUAL(lps::rename(rename_rules_ab_cd(), b), b);
  BOOST_CHECK_EQUAL(lps::rename(rename_rules_ab_cd(), c), d);
  BOOST_CHECK_EQUAL(lps::rename(rename_rules_ab_cd(), d), d);
  BOOST_CHECK_EQUAL(lps::rename(rename_rules_ab_cd(), e), e);
}

BOOST_AUTO_TEST_CASE(test_rename_action_with_sort)
{
  auto a = make_action("a", {data::sort_bool::true_()});
  auto b = make_action("b", {data::sort_bool::true_()});
  auto c = make_action("c", {data::variable("x", data::basic_sort("D"))});
  auto d = make_action("d", {data::variable("x", data::basic_sort("D"))});
  auto e = make_action("e");

  BOOST_CHECK_EQUAL(lps::rename(rename_rule_ab(), a), b);
  BOOST_CHECK_EQUAL(lps::rename(rename_rule_ab(), b), b);
  BOOST_CHECK_EQUAL(lps::rename(rename_rule_ab(), c), c);
  BOOST_CHECK_EQUAL(lps::rename(rename_rule_ab(), d), d);
  BOOST_CHECK_EQUAL(lps::rename(rename_rule_ab(), e), e);

  BOOST_CHECK_EQUAL(lps::rename(rename_rule_cd(), a), a);
  BOOST_CHECK_EQUAL(lps::rename(rename_rule_cd(), b), b);
  BOOST_CHECK_EQUAL(lps::rename(rename_rule_cd(), c), d);
  BOOST_CHECK_EQUAL(lps::rename(rename_rule_cd(), d), d);
  BOOST_CHECK_EQUAL(lps::rename(rename_rule_cd(), e), e);

  BOOST_CHECK_EQUAL(lps::rename(rename_rules_ab_cd(), a), b);
  BOOST_CHECK_EQUAL(lps::rename(rename_rules_ab_cd(), b), b);
  BOOST_CHECK_EQUAL(lps::rename(rename_rules_ab_cd(), c), d);
  BOOST_CHECK_EQUAL(lps::rename(rename_rules_ab_cd(), d), d);
  BOOST_CHECK_EQUAL(lps::rename(rename_rules_ab_cd(), e), e);
}

inline
action_list make_action_list(std::initializer_list<process::action> actions)
{
  return action_list(actions.begin(), actions.end());
}

BOOST_AUTO_TEST_CASE(test_rename_action_list)
{
  auto a = make_action("a");
  auto b = make_action("b");
  auto c = make_action("c");
  auto d = make_action("d");
  auto e = make_action("e");

  // Rename on action_lists does not guarantee that the result is sorted,
  // so we sort the result before comparing it to the expected result.
  auto rename_and_sort = [](const rename_expression_list& renamings, const action_list& actions)
  {
    return atermpp::sort_list(lps::rename(renamings, actions));
  };

  BOOST_CHECK_EQUAL(rename_and_sort(rename_rule_ab(), make_action_list({a, b})), make_action_list({b, b}));
  BOOST_CHECK_EQUAL(rename_and_sort(rename_rule_ab(), make_action_list({b, b})), make_action_list({b, b}));
  BOOST_CHECK_EQUAL(rename_and_sort(rename_rule_ab(), make_action_list({b, c})), make_action_list({b, c}));
  BOOST_CHECK_EQUAL(rename_and_sort(rename_rule_ab(), make_action_list({b, d})), make_action_list({b, d}));
  BOOST_CHECK_EQUAL(rename_and_sort(rename_rule_ab(), make_action_list({c, d})), make_action_list({c, d}));
  BOOST_CHECK_EQUAL(rename_and_sort(rename_rule_ab(), make_action_list({d, e})), make_action_list({d, e}));

  BOOST_CHECK_EQUAL(rename_and_sort(rename_rule_cd(), make_action_list({a, b})), make_action_list({a, b}));
  BOOST_CHECK_EQUAL(rename_and_sort(rename_rule_cd(), make_action_list({b, b})), make_action_list({b, b}));
  BOOST_CHECK_EQUAL(rename_and_sort(rename_rule_cd(), make_action_list({b, c})), make_action_list({b, d}));
  BOOST_CHECK_EQUAL(rename_and_sort(rename_rule_cd(), make_action_list({b, d})), make_action_list({b, d}));
  BOOST_CHECK_EQUAL(rename_and_sort(rename_rule_cd(), make_action_list({c, d})), make_action_list({d, d}));
  BOOST_CHECK_EQUAL(rename_and_sort(rename_rule_cd(), make_action_list({d, e})), make_action_list({d, e}));

  BOOST_CHECK_EQUAL(rename_and_sort(rename_rules_ab_cd(), make_action_list({a, b})), make_action_list({b, b}));
  BOOST_CHECK_EQUAL(rename_and_sort(rename_rules_ab_cd(), make_action_list({b, b})), make_action_list({b, b}));
  BOOST_CHECK_EQUAL(rename_and_sort(rename_rules_ab_cd(), make_action_list({b, c})), make_action_list({b, d}));
  BOOST_CHECK_EQUAL(rename_and_sort(rename_rules_ab_cd(), make_action_list({b, d})), make_action_list({b, d}));
  BOOST_CHECK_EQUAL(rename_and_sort(rename_rules_ab_cd(), make_action_list({c, d})), make_action_list({d, d}));
  BOOST_CHECK_EQUAL(rename_and_sort(rename_rules_ab_cd(), make_action_list({d, e})), make_action_list({d, e}));
}

inline
multi_action make_multi_action(std::initializer_list<process::action> actions)
{
  process::action_list action_list;
  for (const auto& a : actions)
  {
    action_list.push_front(a);
  }
  return multi_action(action_list);
}

BOOST_AUTO_TEST_CASE(test_rename_multi_action)
{
  auto a = make_action("a");
  auto b = make_action("b");
  auto c = make_action("c");
  auto d = make_action("d");
  auto e = make_action("e");

  // Single-action multi-actions
  BOOST_CHECK_EQUAL(lps::rename(rename_rule_ab(), make_multi_action({a})), make_multi_action({b}));
  BOOST_CHECK_EQUAL(lps::rename(rename_rule_ab(), make_multi_action({b})), make_multi_action({b}));
  BOOST_CHECK_EQUAL(lps::rename(rename_rule_ab(), make_multi_action({e})), make_multi_action({e}));
  BOOST_CHECK_EQUAL(lps::rename(rename_rule_ad(), make_multi_action({a})), make_multi_action({d}));

  // Multi-action with two actions; the multi_action constructor sorts the result
  BOOST_CHECK_EQUAL(lps::rename(rename_rule_ab(), make_multi_action({a, c})), make_multi_action({b, c}));
  BOOST_CHECK_EQUAL(lps::rename(rename_rule_cd(), make_multi_action({a, c})), make_multi_action({a, d}));
  BOOST_CHECK_EQUAL(lps::rename(rename_rules_ab_cd(), make_multi_action({a, c})), make_multi_action({b, d}));
  BOOST_CHECK_EQUAL(lps::rename(rename_rule_ad(), make_multi_action({a, b})), make_multi_action({b, d}));
  BOOST_CHECK_EQUAL(lps::rename(rename_rule_ad(), make_multi_action({a, c})), make_multi_action({c, d}));
  BOOST_CHECK_EQUAL(lps::rename(rename_rule_ad(), make_multi_action({a, e})), make_multi_action({d, e}));
  BOOST_CHECK_EQUAL(lps::rename(rename_rule_ad(), make_multi_action({b, c})), make_multi_action({b, c}));
}

inline
stochastic_action_summand make_summand(const multi_action& action,
                                       const data::variable_list& summation_variables = {},
                                       const data::data_expression& condition = data::sort_bool::true_(),
                                       const data::assignment_list& assignments = {})
{
  return stochastic_action_summand(summation_variables, condition, action, assignments, stochastic_distribution());
}

BOOST_AUTO_TEST_CASE(test_rename_summand)
{
  auto a = make_action("a");
  auto b = make_action("b");
  auto c = make_action("c");
  auto d = make_action("d");
  auto e = make_action("e");

  // Summand with multi_action {a}: rename a→b gives multi_action {b}
  auto summand_a = make_summand(make_multi_action({a}));
  auto renamed_a = lps::rename(rename_rule_ab(), summand_a);
  BOOST_CHECK_EQUAL(renamed_a.multi_action(), make_multi_action({b}));

  // Other summand fields are unchanged
  BOOST_CHECK_EQUAL(renamed_a.summation_variables(), summand_a.summation_variables());
  BOOST_CHECK_EQUAL(renamed_a.condition(), summand_a.condition());
  BOOST_CHECK_EQUAL(renamed_a.assignments(), summand_a.assignments());
  BOOST_CHECK_EQUAL(renamed_a.distribution(), summand_a.distribution());

  // Summand with multi_action {a, c}: rename a→b, c→d gives multi_action {b, d}
  auto summand_ac = make_summand(make_multi_action({a, c}));
  auto renamed_ac = lps::rename(rename_rules_ab_cd(), summand_ac);
  BOOST_CHECK_EQUAL(renamed_ac.multi_action(), make_multi_action({b, d}));

  // Summand whose action is not renamed
  auto summand_e = make_summand(make_multi_action({e}));
  BOOST_CHECK_EQUAL(lps::rename(rename_rule_ab(), summand_e).multi_action(), make_multi_action({e}));
}

BOOST_AUTO_TEST_CASE(test_rename_summand_vector)
{
  auto a = make_action("a");
  auto b = make_action("b");
  auto c = make_action("c");
  auto d = make_action("d");
  auto e = make_action("e");

  stochastic_action_summand_vector summands;
  summands.push_back(make_summand(make_multi_action({a})));
  summands.push_back(make_summand(make_multi_action({c})));
  summands.push_back(make_summand(make_multi_action({e})));

  lps::rename(rename_rules_ab_cd(), summands);

  BOOST_CHECK_EQUAL(summands[0].multi_action(), make_multi_action({b}));
  BOOST_CHECK_EQUAL(summands[1].multi_action(), make_multi_action({d}));
  BOOST_CHECK_EQUAL(summands[2].multi_action(), make_multi_action({e}));
}
