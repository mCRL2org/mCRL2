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
action_name_multiset allow_ab()
{
  core::identifier_string_list result;
  result.push_front(core::identifier_string("b"));
  result.push_front(core::identifier_string("a"));
  return action_name_multiset(result);
}

inline
action_name_multiset allow_abb()
{
  core::identifier_string_list result;
  result.push_front(core::identifier_string("b"));
  result.push_front(core::identifier_string("b"));
  result.push_front(core::identifier_string("a"));
  return action_name_multiset(result);
}

inline
action_name_multiset allow_cd()
{
  core::identifier_string_list result;
  result.push_front(core::identifier_string("d"));
  result.push_front(core::identifier_string("c"));
  return action_name_multiset(result);
}

inline
action_name_multiset_list allow_ab_abb_cd()
{
  action_name_multiset_list result;
  result.push_front(allow_cd());
  result.push_front(allow_abb());
  result.push_front(allow_ab());
  return result;
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

// TODO: extend with tests for block.