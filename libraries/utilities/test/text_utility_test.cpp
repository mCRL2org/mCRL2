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

#include <boost/test/minimal.hpp>
#include "mcrl2/utilities/text_utility.h"

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

void test_remove_comments()
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

void test_word_wrap()
{
  std::string text = utilities::word_wrap_text(TEXT3, 5);
  BOOST_CHECK(text == TEXT4);
}

void test_number2string() 
{ 
  BOOST_CHECK(utilities::number2string(0) == "0"); 
  BOOST_CHECK(utilities::number2string(1) == "1"); 
  BOOST_CHECK(utilities::number2string(10) == "10"); 
  BOOST_CHECK(utilities::number2string(123456789) == "123456789"); 
} 

int test_main(int, char*[])
{
  test_remove_comments();
  test_word_wrap();
  test_number2string(); 

  return 0;
}
