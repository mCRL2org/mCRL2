#include <sstream>
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#include <iostream>
#include <cstdio>

#include <boost/test/minimal.hpp>

#include <tipi/mime_type.hpp>

#define BOOST_TEST_SHOW_PROGRESS yes

using boost::unit_test::test_suite;

void object_construction() {
  tipi::mime_type t("mcrl2");

  BOOST_CHECK(t.get_sub_type() == "mcrl2" && t.get_main_type() == "application");

  t = tipi::mime_type("text/mcrl2");

  BOOST_CHECK(t.get_sub_type() == "mcrl2" && t.get_main_type() == "text");
}

int test_main(int argc, char * argv[]) {
  using namespace boost;

  /* Change log parameters */
  object_construction();

  return 0;
}

