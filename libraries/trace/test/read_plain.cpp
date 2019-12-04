// Author(s): Unknown
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file read_plain.cpp

#define BOOST_TEST_MODULE read_plain
#include <boost/test/included/unit_test_framework.hpp>

#include "mcrl2/trace/trace.h"

using namespace std;
using namespace mcrl2::trace;

static const char* trace_str = "a\nb(1,true)\nc\n";

static bool read_trace(Trace& t, const char* s)
{
  stringstream ss(s);

  try
  {
    t.load(ss);
  }
  catch (...)
  {
    return false;
  }

  return true;
}

void test_next_action(Trace& t, const char* s)
{
  mcrl2::lps::multi_action a = t.currentAction();
  t.increasePosition();
  BOOST_CHECK(a != mcrl2::lps::multi_action());
  if (a != mcrl2::lps::multi_action())
  {
    BOOST_CHECK(a.actions().size() == 1);
    BOOST_CHECK(pp(a)==s);
  }
}

BOOST_AUTO_TEST_CASE(test_main)
{
  Trace t;

  BOOST_REQUIRE(read_trace(t,trace_str));

  BOOST_REQUIRE(t.number_of_actions() == 3);

  test_next_action(t,"a");
  test_next_action(t,"b(1,true)");
  test_next_action(t,"c");
}
