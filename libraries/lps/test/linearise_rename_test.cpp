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
#include <boost/algorithm/string/predicate.hpp>
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
  data::sort_expression_list sorts;
  for(const auto& expression : arguments)
  {
    sorts.push_front(expression.sort());
  }
  sorts = atermpp::reverse(sorts);

  const action_label label(core::identifier_string(name), sorts);
  return process::action(label, arguments);
}

inline
rename_expression_list rename_rule_ab()
{
  rename_expression_list result;
  result.push_front(rename_expression("a", "b"));
  return result;
}

inline
rename_expression_list rename_rule_cd()
{
  rename_expression_list result;
  result.push_front(rename_expression("c", "d"));
  return result;
}

inline
rename_expression_list rename_rules_ab_cd()
{
  rename_expression_list result;
  result.push_front(rename_expression("c", "d"));
  result.push_front(rename_expression("a", "b"));
  return result;
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

BOOST_AUTO_TEST_CASE(test_rename_action_list)
{
  auto a = make_action("a");
  auto b = make_action("b");
  auto c = make_action("c");
  auto d = make_action("d");
  auto e = make_action("e");

  action_list ab;
  ab.push_front(b);
  ab.push_front(a);

  action_list bb;
  bb.push_front(b);
  bb.push_front(b);

  action_list bc;
  bc.push_front(c);
  bc.push_front(b);

  action_list bd;
  bd.push_front(d);
  bd.push_front(b);

  action_list cd;
  cd.push_front(d);
  cd.push_front(c);

  action_list dd;
  dd.push_front(d);
  dd.push_front(d);

  action_list de;
  de.push_front(e);
  de.push_front(d);

  BOOST_CHECK_EQUAL(lps::rename(rename_rule_ab(), ab), bb);
  BOOST_CHECK_EQUAL(lps::rename(rename_rule_ab(), bb), bb);
  BOOST_CHECK_EQUAL(lps::rename(rename_rule_ab(), bc), bc);
  BOOST_CHECK_EQUAL(lps::rename(rename_rule_ab(), bd), bd);
  BOOST_CHECK_EQUAL(lps::rename(rename_rule_ab(), cd), cd);
  BOOST_CHECK_EQUAL(lps::rename(rename_rule_ab(), de), de);

  BOOST_CHECK_EQUAL(lps::rename(rename_rule_cd(), ab), ab);
  BOOST_CHECK_EQUAL(lps::rename(rename_rule_cd(), bb), bb);
  BOOST_CHECK_EQUAL(lps::rename(rename_rule_cd(), bc), bd);
  BOOST_CHECK_EQUAL(lps::rename(rename_rule_cd(), bd), bd);
  BOOST_CHECK_EQUAL(lps::rename(rename_rule_cd(), cd), dd);
  BOOST_CHECK_EQUAL(lps::rename(rename_rule_cd(), de), de);

  BOOST_CHECK_EQUAL(lps::rename(rename_rules_ab_cd(), ab), bb);
  BOOST_CHECK_EQUAL(lps::rename(rename_rules_ab_cd(), bb), bb);
  BOOST_CHECK_EQUAL(lps::rename(rename_rules_ab_cd(), bc), bd);
  BOOST_CHECK_EQUAL(lps::rename(rename_rules_ab_cd(), bd), bd);
  BOOST_CHECK_EQUAL(lps::rename(rename_rules_ab_cd(), cd), dd);
  BOOST_CHECK_EQUAL(lps::rename(rename_rules_ab_cd(), de), de);
}

// TODO: extend with tests for renaming multiactions, summands and lps.