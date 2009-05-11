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
#include "mcrl2/core/text_utility.h"

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
  std::string text = core::remove_comments(TEXT1);
  BOOST_CHECK(text == TEXT2);
}

int test_main(int, char*[])
{
  return 0;
}
