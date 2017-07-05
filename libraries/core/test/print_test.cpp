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

#define MCRL2_PRINT_DEBUG

#include "mcrl2/core/print.h"
#include <boost/test/minimal.hpp>
#include <sstream>

using namespace mcrl2;

void test_print()
{
  // print directly using core::detail::printer
  std::ostringstream out1;
  core::detail::apply_printer<core::detail::printer> printer(out1);
  core::identifier_string s("abc");
  printer.apply(s);
  BOOST_CHECK(out1.str() == "abc");

  // print using the one argument function print
  std::ostringstream out2;
  core::stream_printer()(s, out2);
  BOOST_CHECK(out2.str() == "abc");

  // print using the two argument function print
  BOOST_CHECK(core::pp(s) == "abc");

}

int test_main(int argc, char** argv)
{
  test_print();
  return 0;
}
