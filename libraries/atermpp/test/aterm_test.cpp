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
#include <sstream>
#include <boost/test/minimal.hpp>

#include "mcrl2/atermpp/aterm_io.h"
#include "mcrl2/atermpp/aterm_int.h"
#include "mcrl2/atermpp/aterm_string.h"

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

void test_aterm_string()
{
  aterm_string empty = empty_string();
  std::ostringstream out;
  out << empty;
  BOOST_CHECK(out.str() == "");
}

void test_aterm_io()
{
  aterm_string empty = empty_string();
  std::ostringstream out;
  write_term_to_text_stream(empty, out);
  std::istringstream in(out.str());
  aterm_string empty2 = aterm_cast<aterm_string>(read_term_from_text_stream(in));
  BOOST_CHECK(empty == empty2);
}

int test_main(int argc, char* argv[])
{
  test_aterm();
  test_aterm_string();
  test_aterm_io();

  return 0;
}
