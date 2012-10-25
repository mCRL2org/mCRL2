// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file aterm_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <string>
#include <boost/test/minimal.hpp>

#include "mcrl2/atermpp/aterm_io.h"
#include "mcrl2/atermpp/aterm_int.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace std;
using namespace atermpp;

void test_aterm()
{
  atermpp::aterm a = read_term_from_string("f(x)");
  atermpp::aterm label = read_term_from_string("label");

  atermpp::aterm d = aterm_int(10);
  BOOST_CHECK(aterm_int(d).value() == 10);

  atermpp::aterm e = atermpp::aterm();
  BOOST_CHECK(e==atermpp::aterm());
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)
  test_aterm();
  return 0;
}
