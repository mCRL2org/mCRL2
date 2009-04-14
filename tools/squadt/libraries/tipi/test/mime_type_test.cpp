// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <sstream>
#include <iostream>
#include <cstdio>

#include <boost/test/minimal.hpp>

#include <tipi/mime_type.hpp>

#define BOOST_TEST_SHOW_PROGRESS yes

using boost::unit_test::test_suite;

void object_construction() {
  tipi::mime_type t("mcrl2");

  BOOST_CHECK(t.sub_type() == "mcrl2" && t.main_type() == "application");

  t = tipi::mime_type("text/mcrl2");

  BOOST_CHECK(t.sub_type() == "mcrl2" && t.main_type() == "text");
}

int test_main(int argc, char * argv[]) {
  using namespace boost;

  /* Change log parameters */
  object_construction();

  return 0;
}

