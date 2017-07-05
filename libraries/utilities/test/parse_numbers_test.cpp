// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file parse_test.cpp
/// \brief Add your file description here.

#include <boost/test/included/unit_test_framework.hpp>

#include "mcrl2/utilities/parse_numbers.h"

using namespace mcrl2;

void test_parse_natural_number_sequence(const std::string& text, const std::vector<std::size_t>& expected_result)
{
  std::vector<std::size_t> result = utilities::parse_natural_number_sequence(text);
  BOOST_CHECK(result == expected_result);
}

BOOST_AUTO_TEST_CASE(parse_natural_numbers_test)
{
  test_parse_natural_number_sequence("  1 2 3 ", { 1, 2, 3 });
  test_parse_natural_number_sequence("  ", { });
  test_parse_natural_number_sequence("1", { 1 });
  test_parse_natural_number_sequence(" 1", { 1 });
  test_parse_natural_number_sequence("1 ", { 1 });

  BOOST_CHECK_THROW(utilities::parse_natural_number_sequence("a"), mcrl2::runtime_error);
  BOOST_CHECK_THROW(utilities::parse_natural_number_sequence("12 a"), mcrl2::runtime_error);
  BOOST_CHECK_THROW(utilities::parse_natural_number_sequence("12 3a "), mcrl2::runtime_error);
}

void test_parse_number(const std::string& text, std::size_t expected_result)
{
  std::size_t result = utilities::parse_natural_number(text);
  BOOST_CHECK_EQUAL(result, expected_result);
}

BOOST_AUTO_TEST_CASE(parse_number_test)
{
  test_parse_number("10", 10);
  test_parse_number(" 10", 10);
  test_parse_number("  10  ", 10);
  test_parse_number(" 023  ", 23);

  BOOST_CHECK_THROW(utilities::parse_natural_number(""), mcrl2::runtime_error);
  BOOST_CHECK_THROW(utilities::parse_natural_number("   "), mcrl2::runtime_error);
  BOOST_CHECK_THROW(utilities::parse_natural_number("a"), mcrl2::runtime_error);
  BOOST_CHECK_THROW(utilities::parse_natural_number("12 a"), mcrl2::runtime_error);
  BOOST_CHECK_THROW(utilities::parse_natural_number("12 3a "), mcrl2::runtime_error);
}

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
  return nullptr;
}
