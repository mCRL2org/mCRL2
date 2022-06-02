// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/utilities/file_utility.h"

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/included/unit_test.hpp>

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
