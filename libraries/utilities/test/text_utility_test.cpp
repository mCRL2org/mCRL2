// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file text_utility_test.cpp
/// \brief Add your file description here.

#include "mcrl2/utilities/text_utility.h"
#include <boost/test/included/unit_test_framework.hpp>

using namespace mcrl2;

std::string TEXT1 =
  "abc % afdsd     \n"
  "%               \n"
  "%               \n"
  "ddasdfsd%adsfs%-\n"
  ;

std::string TEXT2 =
  "abc \n"
  "\n"
  "\n"
  "ddasdfsd\n"
  ;

BOOST_AUTO_TEST_CASE(test_remove_comments)
{
  std::string text = utilities::remove_comments(TEXT1);
  BOOST_CHECK(text == TEXT2);
}

std::string TEXT3 =
  "abcdef gh       \n"
  "c\n"
  "\n"
  "abcde ghi"
  ;

std::string TEXT4 =
  "abcde\n"
  "f gh\n"
  "c\n"
  "\n"
  "abcde\n"
  "ghi"
  ;

BOOST_AUTO_TEST_CASE(test_word_wrap)
{
  std::string text = utilities::word_wrap_text(TEXT3, 5);
  BOOST_CHECK(text == TEXT4);
}

BOOST_AUTO_TEST_CASE(test_number2string)
{
  BOOST_CHECK(utilities::number2string(0) == "0");
  BOOST_CHECK(utilities::number2string(1) == "1");
  BOOST_CHECK(utilities::number2string(10) == "10");
  BOOST_CHECK(utilities::number2string(123456789) == "123456789");
}

BOOST_AUTO_TEST_CASE(test_regex_split)
{
  BOOST_CHECK(utilities::regex_split("a := b; \n", ":=") == std::vector<std::string>({ "a", "b;" }));
  BOOST_CHECK(utilities::regex_split("a+b*c/d ", "[+*/]") == std::vector<std::string>({ "a", "b", "c", "d" }));
}

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
  return nullptr;
}
