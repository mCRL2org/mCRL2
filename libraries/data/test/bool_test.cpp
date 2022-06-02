// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file bool_test.cpp
/// \brief Basic regression test for data expressions.

#define BOOST_TEST_MODULE bool_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/atermpp/aterm_io.h"
#include "mcrl2/data/bool.h"

using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::data::sort_bool;

BOOST_AUTO_TEST_CASE(test_bool_sort)
{
  const basic_sort& b(bool_());
  BOOST_CHECK(b == bool_());
  BOOST_CHECK(pp(b.name()) == "Bool");
}
