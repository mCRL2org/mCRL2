// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file file_utility_test.cpp
/// \brief Add your file description here.

#include "mcrl2/utilities/file_utility.h"
#include <boost/test/included/unit_test_framework.hpp>

using namespace mcrl2;

BOOST_AUTO_TEST_CASE(test_has_extension)
{
  using utilities::has_extension;
  BOOST_CHECK(has_extension("1.pbes", "pbes"));
  BOOST_CHECK(has_extension("1.pbes.bes", "bes"));
  BOOST_CHECK(!has_extension("1.pbes.bes", "pbes"));
  BOOST_CHECK(!has_extension("bes", "bes"));
  BOOST_CHECK(!has_extension("1.pbes", "bes"));
  BOOST_CHECK(!has_extension("a.pbes.bes","pbes"));  // Ticket #1374.
  BOOST_CHECK(!has_extension("a.besp","bes"));       // Mistake in previous code (until appr. revision 14827).
}

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
  return nullptr;
}
