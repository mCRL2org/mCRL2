// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <boost/test/included/unit_test_framework.hpp>

#include <mcrl2/lts/lts_algorithm.h>
#include <mcrl2/lts/lts_aut.h>

using namespace mcrl2::lts;

static lts_aut_t parse_aut(const std::string& s)
{
  std::stringstream is(s);
  lts_aut_t l;
  l.load(is);
  return l;
}

// a.(b+c)
const std::string l1 =
  "des(0,3,4)\n"
  "(0,\"a\",1)\n"
  "(1,\"b\",2)\n"
  "(1,\"c\",3)\n";

// a.b+a.c
const std::string l2 =
  "des(0,4,5)\n"
  "(0,\"a\",1)\n"
  "(0,\"a\",2)\n"
  "(1,\"b\",3)\n"
  "(2,\"c\",4)\n";

// a.tau.(b+c)
const std::string l3 =
  "des(0,4,5)\n"
  "(0,\"a\",1)\n"
  "(1,\"tau\",2)\n"
  "(2,\"b\",3)\n"
  "(2,\"c\",4)\n";

// a.(b+b)
const std::string l4 =
  "des(0,3,4)\n"
  "(0,\"a\",1)\n"
  "(1,\"b\",2)\n"
  "(1,\"b\",3)\n";

// a
const std::string a =
  "des (0,1,2)\n"
  "(0,\"a\",1)\n";

// b
const std::string b =
  "des (0,1,2)\n"
  "(0,\"b\",1)\n";

static inline
bool compare(const std::string& s1, const std::string& s2, lts_equivalence eq, bool counterexample=false)
{
  lts_aut_t t1=parse_aut(s1);
  lts_aut_t t2=parse_aut(s2);
  return compare(t1, t2, eq, counterexample);
}

BOOST_AUTO_TEST_CASE(test_symmetric_trace_1_2)
{
  BOOST_CHECK(compare(l1,l2,lts_eq_trace));
  BOOST_CHECK(compare(l2,l1,lts_eq_trace));
}

BOOST_AUTO_TEST_CASE(test_symmetric_trace_1_3)
{
  BOOST_CHECK(!compare(l1,l3,lts_eq_trace));
  BOOST_CHECK(!compare(l3,l1,lts_eq_trace));
}

BOOST_AUTO_TEST_CASE(test_symmetric_trace_1_4)
{
  BOOST_CHECK(!compare(l1,l4,lts_eq_trace));
  BOOST_CHECK(!compare(l4,l1,lts_eq_trace));
}

BOOST_AUTO_TEST_CASE(test_symmetric_weak_trace_2_3)
{
  BOOST_CHECK(compare(l2,l3,lts_eq_weak_trace));
  BOOST_CHECK(compare(l3,l2,lts_eq_weak_trace));
}

BOOST_AUTO_TEST_CASE(test_symmetric_weak_trace_3_4)
{
  BOOST_CHECK(!compare(l4,l3,lts_eq_weak_trace));
  BOOST_CHECK(!compare(l3,l4,lts_eq_weak_trace));
}

// Regression test for bug #1082
BOOST_AUTO_TEST_CASE(test_bisim_a_b)
{
  BOOST_CHECK(!compare(a,b,lts_eq_bisim, true));
}

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
  return 0;
}
