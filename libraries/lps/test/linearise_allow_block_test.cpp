// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file linearise_allow_block_test.cpp
/// \brief Test for applying allow and block operator

#define BOOST_TEST_MODULE linearise_allow_block_test
#include <boost/algorithm/string/predicate.hpp>
#include <boost/test/included/unit_test.hpp>

#include "../include/mcrl2/lps/linearise_allow_block.h"

using namespace mcrl2;
using namespace mcrl2::process;
using namespace mcrl2::lps;

struct LogDebug
{
  LogDebug()
  {
    log::logger::set_reporting_level(log::log_level_t::debug);
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

inline lps::detail::allow_list_cache allow_ab()
{
  core::identifier_string_list result;
  result.push_front(core::identifier_string("b"));
  result.push_front(core::identifier_string("a"));
  process::action_name_multiset_list multiset_list;
  multiset_list.push_front(process::action_name_multiset(result));
  return lps::detail::make_allow_list_cache(multiset_list);
}

inline lps::detail::allow_list_cache allow_abb()
{
  core::identifier_string_list result;
  result.push_front(core::identifier_string("b"));
  result.push_front(core::identifier_string("b"));
  result.push_front(core::identifier_string("a"));
  process::action_name_multiset_list multiset_list;
  multiset_list.push_front(process::action_name_multiset(result));
  return lps::detail::make_allow_list_cache(multiset_list);
}

inline lps::detail::allow_list_cache allow_cd()
{
  core::identifier_string_list result;
  result.push_front(core::identifier_string("d"));
  result.push_front(core::identifier_string("c"));
  process::action_name_multiset_list multiset_list;
  multiset_list.push_front(process::action_name_multiset(result));
  return lps::detail::make_allow_list_cache(multiset_list);
}

inline lps::detail::allow_list_cache allow_ab_abb_cd()
{
  process::action_name_multiset_list result;

  // Create multisets for each allowed action
  core::identifier_string_list ab;
  ab.push_front(core::identifier_string("b"));
  ab.push_front(core::identifier_string("a"));

  core::identifier_string_list abb;
  abb.push_front(core::identifier_string("b"));
  abb.push_front(core::identifier_string("b"));
  abb.push_front(core::identifier_string("a"));

  core::identifier_string_list cd;
  cd.push_front(core::identifier_string("d"));
  cd.push_front(core::identifier_string("c"));

  result.push_front(process::action_name_multiset(cd));
  result.push_front(process::action_name_multiset(abb));
  result.push_front(process::action_name_multiset(ab));

  return lps::detail::make_allow_list_cache(result);
}

BOOST_AUTO_TEST_CASE(test_single_allow)
{
  auto a = make_action("a");
  auto b = make_action("b");
  auto c = make_action("c");
  auto termination_action = make_action("Terminate");

  action_list a_;
  a_.push_front(a);

  BOOST_ASSERT(!allow_(allow_ab(), a_, termination_action));
  BOOST_ASSERT(!allow_(allow_abb(), a_, termination_action));
  BOOST_ASSERT(!allow_(allow_cd(), a_, termination_action));

  action_list ab;
  ab.push_front(b);
  ab.push_front(a);

  BOOST_ASSERT(allow_(allow_ab(), ab, termination_action));
  BOOST_ASSERT(!allow_(allow_abb(), ab, termination_action));
  BOOST_ASSERT(!allow_(allow_cd(), ab, termination_action));

  action_list abb;
  abb.push_front(b);
  abb.push_front(b);
  abb.push_front(a);

  BOOST_ASSERT(!allow_(allow_ab(), abb, termination_action));
  BOOST_ASSERT(allow_(allow_abb(), abb, termination_action));
  BOOST_ASSERT(!allow_(allow_cd(), abb, termination_action));

  action_list bb;
  bb.push_front(b);
  bb.push_front(b);

  BOOST_ASSERT(!allow_(allow_ab(), bb, termination_action));
  BOOST_ASSERT(!allow_(allow_abb(), bb, termination_action));
  BOOST_ASSERT(!allow_(allow_cd(), bb, termination_action));
}

BOOST_AUTO_TEST_CASE(test_allow)
{
  auto a = make_action("a");
  auto b = make_action("b");
  auto c = make_action("c");
  auto termination_action = make_action("Terminate");

  BOOST_ASSERT(allow_(allow_ab_abb_cd(), action_list(), termination_action));
  action_list terminate;
  terminate.push_front(termination_action);
  BOOST_ASSERT(allow_(allow_ab_abb_cd(), terminate, termination_action));

  action_list a_;
  a_.push_front(a);
  BOOST_ASSERT(!allow_(allow_ab_abb_cd(), a_, termination_action));

  action_list ab;
  ab.push_front(b);
  ab.push_front(a);
  BOOST_ASSERT(allow_(allow_ab_abb_cd(), ab, termination_action));

  action_list abb;
  abb.push_front(b);
  abb.push_front(b);
  abb.push_front(a);
  BOOST_ASSERT(allow_(allow_ab_abb_cd(), abb, termination_action));

  action_list bb;
  bb.push_front(b);
  bb.push_front(b);
  BOOST_ASSERT(!allow_(allow_ab_abb_cd(), bb, termination_action));
}

BOOST_AUTO_TEST_CASE(test_allow_block_filter)
{
  const process::action a = make_action("a");
  const process::action b = make_action("b");
  const process::action c = make_action("c");
  const process::action termination_action = make_action("Terminate");

  const action_list empty;
  const action_list terminate({termination_action});
  const action_list a_({a});
  const action_list ab({a, b});
  const action_list bc({b, c});

  // Without allow or block every multi-action is kept.
  const allow_block_filter keep_all;
  BOOST_CHECK(!keep_all.is_allow() && !keep_all.is_block());
  BOOST_CHECK(keep_all.keeps(empty));
  BOOST_CHECK(keep_all.keeps(a_));
  BOOST_CHECK(keep_all.keeps(ab));

  // allow({a|b, c}): the empty multi-action and the termination action are always allowed.
  const process::action_name_multiset_list allowlist({process::action_name_multiset(core::identifier_string_list(
                                                        {core::identifier_string("a"), core::identifier_string("b")})),
    process::action_name_multiset(core::identifier_string_list({core::identifier_string("c")}))});
  const allow_block_filter allow(allowlist, true, false, termination_action);
  BOOST_CHECK(allow.is_allow() && !allow.is_block());
  BOOST_CHECK_EQUAL(allow.size(), 2U);
  BOOST_CHECK(allow.keeps(empty));
  BOOST_CHECK(allow.keeps(terminate));
  BOOST_CHECK(!allow.keeps(a_));
  BOOST_CHECK(allow.keeps(ab));
  BOOST_CHECK(!allow.keeps(bc));

  // block({b, c}): a multi-action is blocked if one of its actions is.
  const process::action_name_multiset_list blocklist({process::action_name_multiset(
    core::identifier_string_list({core::identifier_string("b"), core::identifier_string("c")}))});
  const allow_block_filter block(blocklist, false, true, termination_action);
  BOOST_CHECK(!block.is_allow() && block.is_block());
  BOOST_CHECK_EQUAL(block.size(), 2U);
  BOOST_CHECK(block.keeps(empty));
  BOOST_CHECK(block.keeps(terminate));
  BOOST_CHECK(block.keeps(a_));
  BOOST_CHECK(!block.keeps(ab));
  BOOST_CHECK(!block.keeps(bc));

  // On a multi-action only its actions count, not its time.
  BOOST_CHECK(allow.keeps(multi_action(ab, data::sort_real::real_(1))));
  BOOST_CHECK(!allow.keeps(multi_action(a_)));
  BOOST_CHECK(!block.keeps(multi_action(ab, data::sort_real::real_(1))));
}
