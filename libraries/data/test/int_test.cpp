// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file data_expression_test.cpp
/// \brief Basic regression test for data expressions.

#include <iostream>
#include <boost/range/iterator_range.hpp>
#include <boost/test/minimal.hpp>

#include "mcrl2/data/standard_utility.h"

#include "mcrl2/core/garbage_collection.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace mcrl2;
using namespace mcrl2::data;

void recogniser_test()
{
  variable i("i", sort_int::int_());
  application minus(sort_int::minus(i, sort_int::int_("5")));
  application negate(sort_int::negate(i));

  BOOST_CHECK(sort_int::is_minus_application(minus));
  BOOST_CHECK(!sort_int::is_negate_application(minus));
  BOOST_CHECK(sort_int::is_negate_application(negate));
  BOOST_CHECK(!sort_int::is_minus_application(negate));
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv);

  recogniser_test();
  core::garbage_collect();

  return EXIT_SUCCESS;
}
