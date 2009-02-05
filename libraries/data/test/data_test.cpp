// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file data_test.cpp
/// \brief Add your file description here.

#include <string>
#include <iostream>
#include <boost/test/minimal.hpp>

#include "mcrl2/atermpp/vector.h"
#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/data/data_operators.h"
#include "mcrl2/data/data.h"

using namespace mcrl2::data;
using namespace atermpp;
using namespace mcrl2::data::data_expr;

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)

  data_variable v;
  std::string name = v.name();
  BOOST_CHECK(name == "@NoValue");

  data_expression d;
  data_expression d1 = pos(2);
  data_expression d2 = pos(4);
  d = negate       (d1   ); BOOST_CHECK(is_negate       (d));
  d = plus         (d1,d2); BOOST_CHECK(is_plus         (d));
  d = minus        (d1,d2); BOOST_CHECK(is_minus        (d));
  d = multiplies   (d1,d2); BOOST_CHECK(is_multiplies   (d));
  d = divides      (d1,d2); BOOST_CHECK(is_divides      (d));
  d = modulus      (d1,d2); BOOST_CHECK(is_modulus      (d));
  d = equal_to     (d1,d2); BOOST_CHECK(is_equal_to     (d));
  d = not_equal_to (d1,d2); BOOST_CHECK(is_not_equal_to (d));
  d = less         (d1,d2); BOOST_CHECK(is_less         (d));
  d = greater      (d1,d2); BOOST_CHECK(is_greater      (d));
  d = less_equal   (d1,d2); BOOST_CHECK(is_less_equal   (d));
  d = greater_equal(d1,d2); BOOST_CHECK(is_greater_equal(d));
  d = min_         (d1,d2); BOOST_CHECK(is_min          (d));
  d = max_         (d1,d2); BOOST_CHECK(is_max          (d));
  d = abs          (d1   ); BOOST_CHECK(is_abs          (d));

  return 0;
}
