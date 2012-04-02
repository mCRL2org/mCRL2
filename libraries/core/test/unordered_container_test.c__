// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file identifier_string_test.cpp
/// \brief Add your file description here.

#include <boost/test/included/unit_test_framework.hpp>
#include <unordered_set>
#include <unordered_map>


BOOST_AUTO_TEST_CASE(test_unordered_set)
{
  std::unordered_set<int> s;
  s.insert(3);
  s.insert(3);
  s.insert(481);
  BOOST_CHECK_EQUAL(s.size(),2);
}


BOOST_AUTO_TEST_CASE(test_unordered_multiset)
{
  std::unordered_multiset<int> s;
  s.insert(3);
  s.insert(3);
  s.insert(481);
  BOOST_CHECK_EQUAL(s.size(),3);
}

BOOST_AUTO_TEST_CASE(test_unordered_map)
{
  std::unordered_map<int, bool> s;
  s.insert(std::pair<int, bool>(3,true));
  s.insert(std::pair<int, bool>(3,false));
  s.insert(std::pair<int, bool>(481,true));
  BOOST_CHECK_EQUAL(s.size(),2);
}

BOOST_AUTO_TEST_CASE(test_unordered_multimap)
{
  std::unordered_multimap<int, bool> s;
  s.insert(std::pair<int, bool>(3,true));
  s.insert(std::pair<int, bool>(3,false));
  s.insert(std::pair<int, bool>(481,true));
  BOOST_CHECK_EQUAL(s.size(),3);
}

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
  return 0;
}
