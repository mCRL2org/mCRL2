// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file identifier_string_test.cpp
/// \brief Add your file description here.

#define BOOST_TEST_MODULE identifier_string_test

#include <iostream>
#include <boost/test/included/unit_test_framework.hpp>
#include "mcrl2/core/identifier_string.h"

using namespace mcrl2;

// Example that didn't compile, submitted by Jan Friso.
void test_identifier_string()
{
  core::identifier_string a("abc");
  const core::identifier_string& b = a;

  const core::identifier_string_list l;
  for (const auto& s: l)
  {
    std::cout << s;
  }
}

BOOST_AUTO_TEST_CASE(test_main)
{
  test_identifier_string();
}
