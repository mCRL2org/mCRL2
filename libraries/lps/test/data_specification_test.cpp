// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file data_specification_test.cpp
/// \brief Test for data specifications. This test belongs to the data
/// library, but unfortunately the data library contains no parser for
/// data specifications.

#define BOOST_TEST_MODULE data_specification_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/lps/linearise.h"

using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::lps;

const std::string SPECIFICATION =
  "act a;                                  \n"
  "                                        \n"
  "proc P(b:Bool) = a. P(b);               \n"
  "                                        \n"
  "init P(false);                          \n"
  ;

BOOST_AUTO_TEST_CASE(test_main)
{
  stochastic_specification spec = linearise(SPECIFICATION);
  data_specification data = spec.data();
  BOOST_CHECK(data.is_certainly_finite(sort_bool::bool_()));
  BOOST_CHECK(!data.is_certainly_finite(sort_nat::nat()));
}
