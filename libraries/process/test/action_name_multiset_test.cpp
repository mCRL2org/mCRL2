// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file action_name_multiset_test.cpp
/// \brief Tests for the sorted-storage invariant of action_name_multiset and block.

#define BOOST_TEST_MODULE action_name_multiset_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/process/action_name_multiset.h"
#include "mcrl2/process/process_expression.h"

using namespace mcrl2;

// Helper to make a core::identifier_string_list from initializer list of strings.
static core::identifier_string_list make_id_list(std::initializer_list<const char*> names)
{
  std::vector<core::identifier_string> v;
  for (const char* s : names)
  {
    v.emplace_back(s);
  }
  return core::identifier_string_list(v.begin(), v.end());
}

// Invariant: action_name_multiset always stores names in sorted (lexicographic) order.

BOOST_AUTO_TEST_CASE(test_empty)
{
  process::action_name_multiset ms(make_id_list({}));
  BOOST_CHECK(ms.names().empty());
}

BOOST_AUTO_TEST_CASE(test_singleton)
{
  process::action_name_multiset ms(make_id_list({"a"}));
  core::identifier_string_list expected = make_id_list({"a"});
  BOOST_CHECK_EQUAL(ms.names(), expected);
}

BOOST_AUTO_TEST_CASE(test_already_sorted)
{
  process::action_name_multiset ms(make_id_list({"a", "b", "c"}));
  core::identifier_string_list expected = make_id_list({"a", "b", "c"});
  BOOST_CHECK_EQUAL(ms.names(), expected);
}

BOOST_AUTO_TEST_CASE(test_unsorted_input_stored_sorted)
{
  process::action_name_multiset ms(make_id_list({"c", "a", "b"}));
  core::identifier_string_list expected = make_id_list({"a", "b", "c"});
  BOOST_CHECK_EQUAL(ms.names(), expected);
}

BOOST_AUTO_TEST_CASE(test_reverse_sorted_input)
{
  process::action_name_multiset ms(make_id_list({"z", "m", "a"}));
  core::identifier_string_list expected = make_id_list({"a", "m", "z"});
  BOOST_CHECK_EQUAL(ms.names(), expected);
}

BOOST_AUTO_TEST_CASE(test_with_duplicates)
{
  // Multisets may contain duplicates; sorting should still work.
  process::action_name_multiset ms(make_id_list({"b", "a", "b", "a"}));
  core::identifier_string_list expected = make_id_list({"a", "a", "b", "b"});
  BOOST_CHECK_EQUAL(ms.names(), expected);
}

// Invariant: block always stores block_set in sorted (lexicographic) order.

BOOST_AUTO_TEST_CASE(test_block_empty)
{
  process::block b(make_id_list({}), process::delta());
  BOOST_CHECK(b.block_set().empty());
}

BOOST_AUTO_TEST_CASE(test_block_singleton)
{
  process::block b(make_id_list({"a"}), process::delta());
  core::identifier_string_list expected = make_id_list({"a"});
  BOOST_CHECK_EQUAL(b.block_set(), expected);
}

BOOST_AUTO_TEST_CASE(test_block_unsorted_input_stored_sorted)
{
  process::block b(make_id_list({"c", "a", "b"}), process::delta());
  core::identifier_string_list expected = make_id_list({"a", "b", "c"});
  BOOST_CHECK_EQUAL(b.block_set(), expected);
}
