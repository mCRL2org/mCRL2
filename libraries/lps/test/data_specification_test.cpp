// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file data_specification_test.cpp
/// \brief Test for data specifications. This test belongs to the data
/// library, but unfortunately the data library contains no parser for
/// data specifications.

#include <boost/test/minimal.hpp>
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/linearise.h"
#include "mcrl2/atermpp/aterm_init.h"

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

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)

  specification spec = linearise(SPECIFICATION);
  data_specification data = spec.data();
  BOOST_CHECK(data.is_certainly_finite(sort_bool::bool_()));
  BOOST_CHECK(!data.is_certainly_finite(sort_nat::nat()));

  return 0;
}
