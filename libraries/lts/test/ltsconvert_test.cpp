// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ltsconvert_test.cpp
/// \brief This file contains tests to see whether ltsconvert
//         reduces problems well.

#define BOOST_TEST_MODULE ltsconvert_test
// #include <iostream>
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/lts/test/test_reductions.h"

using namespace mcrl2::lts;

static lts_aut_t parse_aut(const std::string& s)
{
  std::stringstream is(s);
  lts_aut_t l;
  l.load(is);
  return l;
}

BOOST_AUTO_TEST_CASE(Test0)
{
  const std::string test0 =
    "des (0,2,2)\n"
    "(0,\"tau\",1)\n"
    "(1,\"tau\",0)\n";

  expected_sizes e;
  e.states_plain=2; e.transitions_plain=2; e.labels_plain=1;
  e.states_bisimulation=1, e.transitions_bisimulation=1, e.labels_bisimulation=1;
  e.states_branching_bisimulation=1, e.transitions_branching_bisimulation=0, e.labels_branching_bisimulation=1;
  e.states_divergence_preserving_branching_bisimulation=1, e.transitions_divergence_preserving_branching_bisimulation=1, 
         e.labels_divergence_preserving_branching_bisimulation=1;
  e.states_weak_bisimulation=1, e.transitions_weak_bisimulation=0, e.labels_weak_bisimulation=1;
  e.states_divergence_preserving_weak_bisimulation=1, e.transitions_divergence_preserving_weak_bisimulation=1, 
         e.labels_divergence_preserving_weak_bisimulation=2;
  e.states_simulation=1, e.transitions_simulation=1, e.labels_simulation=1;
  e.states_trace_equivalence=1, e.transitions_trace_equivalence=1, e.labels_trace_equivalence=1;
  e.states_weak_trace_equivalence=1, e.transitions_weak_trace_equivalence=0, e.labels_weak_trace_equivalence=1;
  e.states_determinisation=2, e.transitions_determinisation=2, e.labels_determinisation=1;
  e.is_deterministic=true;

  BOOST_CHECK(reduce_lts_in_various_ways("Test0", test0, e));
}


BOOST_AUTO_TEST_CASE(Test1)
{
  const std::string test1=
    "des (0,2,2)\n"
    "(0,\"move_to_ERROR\",1)\n"
    "(1,\"move_to_ERROR\",0)\n";

  expected_sizes e;
  e.states_plain=2; e.transitions_plain=2; e.labels_plain=2;
  e.states_bisimulation=1, e.transitions_bisimulation=1, e.labels_bisimulation=2;
  e.states_branching_bisimulation=1, e.transitions_branching_bisimulation=1, e.labels_branching_bisimulation=2;
  e.states_divergence_preserving_branching_bisimulation=1, e.transitions_divergence_preserving_branching_bisimulation=1, 
         e.labels_divergence_preserving_branching_bisimulation=2;
  e.states_weak_bisimulation=1, e.transitions_weak_bisimulation=1, e.labels_weak_bisimulation=2;
  e.states_divergence_preserving_weak_bisimulation=1, e.transitions_divergence_preserving_weak_bisimulation=1, 
         e.labels_divergence_preserving_weak_bisimulation=3;
  e.states_simulation=1, e.transitions_simulation=1, e.labels_simulation=2;
  e.states_trace_equivalence=1, e.transitions_trace_equivalence=1, e.labels_trace_equivalence=2;
  e.states_weak_trace_equivalence=1, e.transitions_weak_trace_equivalence=1, e.labels_weak_trace_equivalence=2;
  e.states_determinisation=2, e.transitions_determinisation=2, e.labels_determinisation=2;
  e.is_deterministic=true;

  BOOST_CHECK(reduce_lts_in_various_ways("Test1", test1, e));
}

BOOST_AUTO_TEST_CASE(Test2)
{
  const std::string test2=
    "des (0,2,2)\n"
    "(0,\"move_to_ERROR\",1)\n"
    "(1,\"move_to_ERROR1\",0)\n";

  expected_sizes e;
  e.states_plain=2; e.transitions_plain=2; e.labels_plain=3;
  e.states_bisimulation=2, e.transitions_bisimulation=2, e.labels_bisimulation=3;
  e.states_branching_bisimulation=2, e.transitions_branching_bisimulation=2, e.labels_branching_bisimulation=3;
  e.states_divergence_preserving_branching_bisimulation=2, e.transitions_divergence_preserving_branching_bisimulation=2, 
         e.labels_divergence_preserving_branching_bisimulation=3;
  e.states_weak_bisimulation=2, e.transitions_weak_bisimulation=2, e.labels_weak_bisimulation=3;
  e.states_divergence_preserving_weak_bisimulation=2, e.transitions_divergence_preserving_weak_bisimulation=2, 
         e.labels_divergence_preserving_weak_bisimulation=4;
  e.states_simulation=2, e.transitions_simulation=2, e.labels_simulation=3;
  e.states_trace_equivalence=2, e.transitions_trace_equivalence=2, e.labels_trace_equivalence=3;
  e.states_weak_trace_equivalence=2, e.transitions_weak_trace_equivalence=2, e.labels_weak_trace_equivalence=3;
  e.states_determinisation=2, e.transitions_determinisation=2, e.labels_determinisation=3;
  e.is_deterministic=true;

  BOOST_CHECK(reduce_lts_in_various_ways("Test2", test2, e));
}

BOOST_AUTO_TEST_CASE(Test3)
{ 
  const std::string test3=
    "des (0,3,2)\n"
    "(0,\"tau\",1)\n"
    "(1,\"tau\",0)\n"
    "(0,\"a\",1)\n";

  expected_sizes e;
  e.states_plain=2; e.transitions_plain=3; e.labels_plain=2;
  e.states_bisimulation=2, e.transitions_bisimulation=3, e.labels_bisimulation=2;
  e.states_branching_bisimulation=1, e.transitions_branching_bisimulation=1, e.labels_branching_bisimulation=2;
  e.states_divergence_preserving_branching_bisimulation=1, e.transitions_divergence_preserving_branching_bisimulation=2, 
         e.labels_divergence_preserving_branching_bisimulation=2;
  e.states_weak_bisimulation=1, e.transitions_weak_bisimulation=1, e.labels_weak_bisimulation=2;
  e.states_divergence_preserving_weak_bisimulation=1, e.transitions_divergence_preserving_weak_bisimulation=2, 
         e.labels_divergence_preserving_weak_bisimulation=3;
  e.states_simulation=2, e.transitions_simulation=3, e.labels_simulation=2;
  e.states_trace_equivalence=2, e.transitions_trace_equivalence=3, e.labels_trace_equivalence=2;
  e.states_weak_trace_equivalence=1, e.transitions_weak_trace_equivalence=1, e.labels_weak_trace_equivalence=2;
  e.states_determinisation=2, e.transitions_determinisation=3, e.labels_determinisation=2;
  e.is_deterministic=true;

  BOOST_CHECK(reduce_lts_in_various_ways("Test3", test3, e));
}

// Example below represents a(tau(b+c)+b) which can be reduced to a(b+c).
BOOST_AUTO_TEST_CASE(Test4)
{ 
  const std::string test4=
    "des (0,5,6)\n"
    "(0,\"a\",1)\n"
    "(1,\"tau\",2)\n"
    "(2,\"b\",3)\n"
    "(2,\"c\",4)\n"
    "(1,\"b\",5)\n";

  expected_sizes e;
  e.states_plain=6; e.transitions_plain=5; e.labels_plain=4;
  e.states_bisimulation=4, e.transitions_bisimulation=5, e.labels_bisimulation=4;
  e.states_branching_bisimulation=3, e.transitions_branching_bisimulation=3, e.labels_branching_bisimulation=4;
  e.states_divergence_preserving_branching_bisimulation=3, e.transitions_divergence_preserving_branching_bisimulation=3,
         e.labels_divergence_preserving_branching_bisimulation=4;
  e.states_weak_bisimulation=3, e.transitions_weak_bisimulation=3, e.labels_weak_bisimulation=4;
  e.states_divergence_preserving_weak_bisimulation=3, e.transitions_divergence_preserving_weak_bisimulation=3,
         e.labels_divergence_preserving_weak_bisimulation=5;
  e.states_simulation=4, e.transitions_simulation=5, e.labels_simulation=4;
  e.states_trace_equivalence=4, e.transitions_trace_equivalence=5, e.labels_trace_equivalence=4;
  e.states_weak_trace_equivalence=3, e.transitions_weak_trace_equivalence=3, e.labels_weak_trace_equivalence=4;
  e.states_determinisation=6, e.transitions_determinisation=5, e.labels_determinisation=4;
  e.is_deterministic=true;

  BOOST_CHECK(reduce_lts_in_various_ways("Test4", test4, e));
}

// Example below is inspired by the rhs of Milner's third tau law, a(tau.b+c)=a(tau.b+c)+a.b
// which contains a non inert tau.
BOOST_AUTO_TEST_CASE(Test5)
{
  const std::string test5=
    "des (0,6,7)\n"
    "(0,\"a\",1)\n"
    "(0,\"a\",2)\n"
    "(1,\"tau\",3)\n"
    "(1,\"c\",4)\n"
    "(3,\"b\",5)\n"
    "(2,\"b\",6)\n";

  expected_sizes e;
  e.states_plain=7; e.transitions_plain=6; e.labels_plain=4;
  e.states_bisimulation=4, e.transitions_bisimulation=5, e.labels_bisimulation=4;
  e.states_branching_bisimulation=4, e.transitions_branching_bisimulation=5, e.labels_branching_bisimulation=4;
  e.states_divergence_preserving_branching_bisimulation=4, e.transitions_divergence_preserving_branching_bisimulation=5,
         e.labels_divergence_preserving_branching_bisimulation=4;
  e.states_weak_bisimulation=4, e.transitions_weak_bisimulation=4, e.labels_weak_bisimulation=4;
  e.states_divergence_preserving_weak_bisimulation=4, e.transitions_divergence_preserving_weak_bisimulation=4,
         e.labels_divergence_preserving_weak_bisimulation=5;
  e.states_simulation=4, e.transitions_simulation=5, e.labels_simulation=4;
  e.states_trace_equivalence=4, e.transitions_trace_equivalence=5, e.labels_trace_equivalence=4;
  e.states_weak_trace_equivalence=3, e.transitions_weak_trace_equivalence=3, e.labels_weak_trace_equivalence=4;
  e.states_determinisation=6, e.transitions_determinisation=5, e.labels_determinisation=4;
  e.is_deterministic=false;

  BOOST_CHECK(reduce_lts_in_various_ways("Test5", test5, e));
}

// Example below is inspired by Milner's third tau law, a(tau.b+c)=a(tau.b+c)+a.b.
// The states 1 and 2 should be identified in weak bisimulation, but not in branching
// bisimulation.
BOOST_AUTO_TEST_CASE(Test5a)
{ 
  const std::string test5a=
    "des (0,10,7)\n"
    "(0,\"d\",1)\n"
    "(0,\"d\",2)\n"
    "(1,\"a\",3)\n"
    "(2,\"a\",4)\n"
    "(2,\"a\",5)\n"
    "(3,\"b\",6)\n"
    "(3,\"tau\",4)\n"
    "(4,\"c\",6)\n"
    "(5,\"b\",6)\n"
    "(5,\"tau\",4)\n";

  expected_sizes e;
  e.states_plain=7; e.transitions_plain=10; e.labels_plain=5;
  e.states_bisimulation=6, e.transitions_bisimulation=8, e.labels_bisimulation=5;
  e.states_branching_bisimulation=6, e.transitions_branching_bisimulation=8, e.labels_branching_bisimulation=5;
  e.states_divergence_preserving_branching_bisimulation=6, e.transitions_divergence_preserving_branching_bisimulation=8,
         e.labels_divergence_preserving_branching_bisimulation=5;
  e.states_weak_bisimulation=5, e.transitions_weak_bisimulation=5, e.labels_weak_bisimulation=5;
  e.states_divergence_preserving_weak_bisimulation=5, e.transitions_divergence_preserving_weak_bisimulation=5,
         e.labels_divergence_preserving_weak_bisimulation=6;
  e.states_simulation=5, e.transitions_simulation=6, e.labels_simulation=5;
  e.states_trace_equivalence=5, e.transitions_trace_equivalence=6, e.labels_trace_equivalence=5;
  e.states_weak_trace_equivalence=4, e.transitions_weak_trace_equivalence=4, e.labels_weak_trace_equivalence=5;
  e.states_determinisation=5, e.transitions_determinisation=6, e.labels_determinisation=5;
  e.is_deterministic=false;

  BOOST_CHECK(reduce_lts_in_various_ways("Test5a", test5a, e));
}


// In the term a.Y with Y=tau.Y divergence-preserving bisimulation must not remove the tau.
BOOST_AUTO_TEST_CASE(Test6)
{ 
  const std::string test6=
    "des (0,2,2)\n"
    "(0,\"a\",1)\n"
    "(1,\"tau\",1)\n";
  expected_sizes e;
  e.states_plain=2; e.transitions_plain=2; e.labels_plain=2;
  e.states_bisimulation=2, e.transitions_bisimulation=2, e.labels_bisimulation=2;
  e.states_branching_bisimulation=2, e.transitions_branching_bisimulation=1, e.labels_branching_bisimulation=2;
  e.states_divergence_preserving_branching_bisimulation=2, e.transitions_divergence_preserving_branching_bisimulation=2,
         e.labels_divergence_preserving_branching_bisimulation=2;
  e.states_weak_bisimulation=2, e.transitions_weak_bisimulation=1, e.labels_weak_bisimulation=2;
  e.states_divergence_preserving_weak_bisimulation=2, e.transitions_divergence_preserving_weak_bisimulation=2,
         e.labels_divergence_preserving_weak_bisimulation=3;
  e.states_simulation=2, e.transitions_simulation=2, e.labels_simulation=2;
  e.states_trace_equivalence=2, e.transitions_trace_equivalence=2, e.labels_trace_equivalence=2;
  e.states_weak_trace_equivalence=2, e.transitions_weak_trace_equivalence=1, e.labels_weak_trace_equivalence=2;
  e.states_determinisation=2, e.transitions_determinisation=2, e.labels_determinisation=2;
  e.is_deterministic=true;

  BOOST_CHECK(reduce_lts_in_various_ways("Test6", test6, e));
}

// Test whether a mixed up sequence of tau's is properly dealt with.
// Example is essentially: tau.X where X=tau.tau.tau.tau.tau.a.X.
BOOST_AUTO_TEST_CASE(Test7)
{ 
  const std::string test7=
    "des (0,7,7)\n"
    "(0,\"tau\",4)\n"
    "(2,\"tau\",1)\n"
    "(3,\"tau\",2)\n"
    "(4,\"tau\",3)\n"
    "(1,\"tau\",5)\n"
    "(5,\"tau\",6)\n"
    "(6,\"a\",3)\n";

  expected_sizes e;
  e.states_plain=7; e.transitions_plain=7; e.labels_plain=2;
  e.states_bisimulation=7, e.transitions_bisimulation=7, e.labels_bisimulation=2;
  e.states_branching_bisimulation=1, e.transitions_branching_bisimulation=1, e.labels_branching_bisimulation=2;
  e.states_divergence_preserving_branching_bisimulation=1, e.transitions_divergence_preserving_branching_bisimulation=1,
         e.labels_divergence_preserving_branching_bisimulation=2;
  e.states_weak_bisimulation=1, e.transitions_weak_bisimulation=1, e.labels_weak_bisimulation=2;
  e.states_divergence_preserving_weak_bisimulation=1, e.transitions_divergence_preserving_weak_bisimulation=1,
         e.labels_divergence_preserving_weak_bisimulation=3;
  e.states_simulation=7, e.transitions_simulation=7, e.labels_simulation=2;
  e.states_trace_equivalence=7, e.transitions_trace_equivalence=7, e.labels_trace_equivalence=2;
  e.states_weak_trace_equivalence=1, e.transitions_weak_trace_equivalence=1, e.labels_weak_trace_equivalence=2;
  e.states_determinisation=7, e.transitions_determinisation=7, e.labels_determinisation=2;
  e.is_deterministic=true;

  BOOST_CHECK(reduce_lts_in_various_ways("Test7", test7, e));
}

// Test whether a mixed up sequence of tau's is properly dealt with.
// Example is essentially: tau.X where X=tau.tau.tau.tau.tau.a.X.
BOOST_AUTO_TEST_CASE(Test8)
{ 
  const std::string test8=
    "des (0,6,5)\n"
    "(0,\"tau\",1)\n"
    "(0,\"tau\",2)\n"
    "(0,\"tau\",3)\n"
    "(0,\"tau\",4)\n"
    "(3,\"tau\",3)\n"
    "(4,\"tau\",4)\n";

  expected_sizes e;
  e.states_plain=5; e.transitions_plain=6; e.labels_plain=1;
  e.states_bisimulation=3, e.transitions_bisimulation=3, e.labels_bisimulation=1;
  e.states_branching_bisimulation=1, e.transitions_branching_bisimulation=0, e.labels_branching_bisimulation=1;
  e.states_divergence_preserving_branching_bisimulation=3, e.transitions_divergence_preserving_branching_bisimulation=3,
         e.labels_divergence_preserving_branching_bisimulation=1;
  e.states_weak_bisimulation=1, e.transitions_weak_bisimulation=0, e.labels_weak_bisimulation=1;
  e.states_divergence_preserving_weak_bisimulation=3, e.transitions_divergence_preserving_weak_bisimulation=3,
         e.labels_divergence_preserving_weak_bisimulation=2;
  e.states_simulation=1, e.transitions_simulation=1, e.labels_simulation=1;
  e.states_trace_equivalence=1, e.transitions_trace_equivalence=1, e.labels_trace_equivalence=1;
  e.states_weak_trace_equivalence=1, e.transitions_weak_trace_equivalence=0, e.labels_weak_trace_equivalence=1;
  e.states_determinisation=3, e.transitions_determinisation=3, e.labels_determinisation=1;
  e.is_deterministic=false;

  BOOST_CHECK(reduce_lts_in_various_ways("Test8", test8, e));
}

// Test whether a tau-loop at the end of a process is correctly dealt with.
BOOST_AUTO_TEST_CASE(Test9)
{ 
  const std::string test9=
    "des (0,4,4)\n"
    "(0,\"a\",1)\n"
    "(1,\"tau\",2)\n"
    "(2,\"tau\",3)\n"
    "(3,\"tau\",1)\n";

  expected_sizes e;
  e.states_plain=4; e.transitions_plain=4; e.labels_plain=2;
  e.states_bisimulation=2, e.transitions_bisimulation=2, e.labels_bisimulation=2;
  e.states_branching_bisimulation=2, e.transitions_branching_bisimulation=1, e.labels_branching_bisimulation=2;
  e.states_divergence_preserving_branching_bisimulation=2, e.transitions_divergence_preserving_branching_bisimulation=2,
         e.labels_divergence_preserving_branching_bisimulation=2;
  e.states_weak_bisimulation=2, e.transitions_weak_bisimulation=1, e.labels_weak_bisimulation=2;
  e.states_divergence_preserving_weak_bisimulation=2, e.transitions_divergence_preserving_weak_bisimulation=2,
         e.labels_divergence_preserving_weak_bisimulation=3;
  e.states_simulation=2, e.transitions_simulation=2, e.labels_simulation=2;
  e.states_trace_equivalence=2, e.transitions_trace_equivalence=2, e.labels_trace_equivalence=2;
  e.states_weak_trace_equivalence=2, e.transitions_weak_trace_equivalence=1, e.labels_weak_trace_equivalence=2;
  e.states_determinisation=4, e.transitions_determinisation=4, e.labels_determinisation=2;
  e.is_deterministic=true;

  BOOST_CHECK(reduce_lts_in_various_ways("Test9", test9, e));
}

// Test whether a tau that does become non inert will act to split blocks.
BOOST_AUTO_TEST_CASE(Test10)
{ 
  const std::string test10=
    "des (0,7,8)\n"
    "(0,\"b\",1)\n"
    "(0,\"b\",2)\n"
    "(1,\"a\",3)\n"
    "(1,\"tau\",4)\n"
    "(2,\"a\",5)\n"
    "(2,\"tau\",6)\n"
    "(6,\"a\",7)\n";
 
  expected_sizes e;
  e.states_plain=8; e.transitions_plain=7; e.labels_plain=3;
  e.states_bisimulation=5, e.transitions_bisimulation=7, e.labels_bisimulation=3;
  e.states_branching_bisimulation=4, e.transitions_branching_bisimulation=5, e.labels_branching_bisimulation=3;
  e.states_divergence_preserving_branching_bisimulation=4, e.transitions_divergence_preserving_branching_bisimulation=5,
         e.labels_divergence_preserving_branching_bisimulation=3;
  e.states_weak_bisimulation=4, e.transitions_weak_bisimulation=5, e.labels_weak_bisimulation=3;
  e.states_divergence_preserving_weak_bisimulation=4, e.transitions_divergence_preserving_weak_bisimulation=5,
         e.labels_divergence_preserving_weak_bisimulation=4;
  e.states_simulation=4, e.transitions_simulation=4, e.labels_simulation=3;
  e.states_trace_equivalence=4, e.transitions_trace_equivalence=4, e.labels_trace_equivalence=3;
  e.states_weak_trace_equivalence=3, e.transitions_weak_trace_equivalence=2, e.labels_weak_trace_equivalence=3;
  e.states_determinisation=5, e.transitions_determinisation=4, e.labels_determinisation=3;
  e.is_deterministic=false;

  BOOST_CHECK(reduce_lts_in_various_ways("Test10", test10, e));
}

// Test whether tau's can repeatedly become non inert
BOOST_AUTO_TEST_CASE(Test11)
{
  const std::string test11=
    "des (0,5,5)\n"
    "(0,\"tau\",1)\n"
    "(0,\"tau\",3)\n"
    "(1,\"a\",2)\n"
    "(1,\"tau\",3)\n"
    "(3,\"tau\",4)\n";

  expected_sizes e;
  e.states_plain=5; e.transitions_plain=5; e.labels_plain=2;
  e.states_bisimulation=4, e.transitions_bisimulation=5, e.labels_bisimulation=2;
  e.states_branching_bisimulation=2, e.transitions_branching_bisimulation=2, e.labels_branching_bisimulation=2;
  e.states_divergence_preserving_branching_bisimulation=2, e.transitions_divergence_preserving_branching_bisimulation=2,
         e.labels_divergence_preserving_branching_bisimulation=2;
  e.states_weak_bisimulation=2, e.transitions_weak_bisimulation=2, e.labels_weak_bisimulation=2;
  e.states_divergence_preserving_weak_bisimulation=2, e.transitions_divergence_preserving_weak_bisimulation=2,
         e.labels_divergence_preserving_weak_bisimulation=3;
  e.states_simulation=4, e.transitions_simulation=4, e.labels_simulation=2;
  e.states_trace_equivalence=4, e.transitions_trace_equivalence=4, e.labels_trace_equivalence=2;
  e.states_weak_trace_equivalence=2, e.transitions_weak_trace_equivalence=1, e.labels_weak_trace_equivalence=2;
  e.states_determinisation=5, e.transitions_determinisation=4, e.labels_determinisation=2;
  e.is_deterministic=false;

  BOOST_CHECK(reduce_lts_in_various_ways("Test11", test11, e));
}

// The test below caused the tau_star reduction to go astray, as there
// were problems with the reachability check.
BOOST_AUTO_TEST_CASE(Test12)
{
  const std::string test12=
    "des (0,7,6)\n"
    "(0,\"tau\",1)\n"
    "(0,\"tau\",2)\n"
    "(1,\"tau\",3)\n"
    "(2,\"tau\",4)\n"
    "(2,\"a\",5)\n"
    "(3,\"c\",5)\n"
    "(4,\"b\",5)\n";

  expected_sizes e;
  e.states_plain=6; e.transitions_plain=7; e.labels_plain=4;
  e.states_bisimulation=6, e.transitions_bisimulation=7, e.labels_bisimulation=4;
  e.states_branching_bisimulation=5, e.transitions_branching_bisimulation=6, e.labels_branching_bisimulation=4;
  e.states_divergence_preserving_branching_bisimulation=5, e.transitions_divergence_preserving_branching_bisimulation=6,
         e.labels_divergence_preserving_branching_bisimulation=4;
  e.states_weak_bisimulation=5, e.transitions_weak_bisimulation=6, e.labels_weak_bisimulation=4;
  e.states_divergence_preserving_weak_bisimulation=5, e.transitions_divergence_preserving_weak_bisimulation=6,
         e.labels_divergence_preserving_weak_bisimulation=5;
  e.states_simulation=6, e.transitions_simulation=7, e.labels_simulation=4;
  e.states_trace_equivalence=4, e.transitions_trace_equivalence=5, e.labels_trace_equivalence=4;
  e.states_weak_trace_equivalence=2, e.transitions_weak_trace_equivalence=3, e.labels_weak_trace_equivalence=4;
  e.states_determinisation=4, e.transitions_determinisation=5, e.labels_determinisation=4;
  e.is_deterministic=false;

  BOOST_CHECK(reduce_lts_in_various_ways("Test12", test12, e));

  lts_aut_t t12=parse_aut(test12);
  reduce(t12,lts_red_tau_star); //Tau star reduction
  BOOST_CHECK(t12.num_states()==2 && t12.num_transitions()==3); 
}
  
// Test whether subtle tau loops are handled properly in divergence-preserving weak and branching bisimulation.
BOOST_AUTO_TEST_CASE(Test13)
{
  const std::string test13=
    "des (0,4,4)\n"
    "(0,\"tau\",1)\n"
    "(1,\"tau\",2)\n"
    "(2,\"tau\",2)\n"
    "(1,\"a\",3)\n";

  expected_sizes e;
  e.states_plain=4; e.transitions_plain=4; e.labels_plain=2;
  e.states_bisimulation=4, e.transitions_bisimulation=4, e.labels_bisimulation=2;
  e.states_branching_bisimulation=2, e.transitions_branching_bisimulation=2, e.labels_branching_bisimulation=2;
  e.states_divergence_preserving_branching_bisimulation=3, e.transitions_divergence_preserving_branching_bisimulation=3,
         e.labels_divergence_preserving_branching_bisimulation=2;
  e.states_weak_bisimulation=2, e.transitions_weak_bisimulation=2, e.labels_weak_bisimulation=2;
  e.states_divergence_preserving_weak_bisimulation=3, e.transitions_divergence_preserving_weak_bisimulation=3,
         e.labels_divergence_preserving_weak_bisimulation=3;
  e.states_simulation=4, e.transitions_simulation=4, e.labels_simulation=2;
  e.states_trace_equivalence=4, e.transitions_trace_equivalence=4, e.labels_trace_equivalence=2;
  e.states_weak_trace_equivalence=2, e.transitions_weak_trace_equivalence=1, e.labels_weak_trace_equivalence=2;
  e.states_determinisation=4, e.transitions_determinisation=4, e.labels_determinisation=2;
  e.is_deterministic=true;

  BOOST_CHECK(reduce_lts_in_various_ways("Test13", test13, e));

  lts_aut_t t13=parse_aut(test13);
  reduce(t13,lts_red_tau_star); //Tau star reduction
  BOOST_CHECK(t13.num_states()==2 && t13.num_transitions()==1);
}

// Test whether LTSs with one state and one essential transition are reduced
BOOST_AUTO_TEST_CASE(Test14)
{
  const std::string test14=
    "des (0,2,1)\n"
    "(0,\"a\",0)\n"
    "(0,\"a\",0)\n";

  expected_sizes e;
  e.states_plain=1; e.transitions_plain=2; e.labels_plain=2;
  e.states_bisimulation=1, e.transitions_bisimulation=1, e.labels_bisimulation=2;
  e.states_branching_bisimulation=1, e.transitions_branching_bisimulation=1, e.labels_branching_bisimulation=2;
  e.states_divergence_preserving_branching_bisimulation=1, e.transitions_divergence_preserving_branching_bisimulation=1,
         e.labels_divergence_preserving_branching_bisimulation=2;
  e.states_weak_bisimulation=1, e.transitions_weak_bisimulation=1, e.labels_weak_bisimulation=2;
  e.states_divergence_preserving_weak_bisimulation=1, e.transitions_divergence_preserving_weak_bisimulation=1,
         e.labels_divergence_preserving_weak_bisimulation=3;
  e.states_simulation=1, e.transitions_simulation=1, e.labels_simulation=2;
  e.states_trace_equivalence=1, e.transitions_trace_equivalence=1, e.labels_trace_equivalence=2;
  e.states_weak_trace_equivalence=1, e.transitions_weak_trace_equivalence=1, e.labels_weak_trace_equivalence=2;
  e.states_determinisation=1, e.transitions_determinisation=1, e.labels_determinisation=2;
  e.is_deterministic=true;

  BOOST_CHECK(reduce_lts_in_various_ways("Test14", test14, e));
}
  
// Test whether LTSs with one state and more than one essential transitions are reduced
BOOST_AUTO_TEST_CASE(Test15)
{
  const std::string test15=
    "des (0,3,1)\n"
    "(0,\"a\",0)\n"
    "(0,\"b\",0)\n"
    "(0,\"b\",0)\n";

  expected_sizes e;
  e.states_plain=1; e.transitions_plain=3; e.labels_plain=3;
  e.states_bisimulation=1, e.transitions_bisimulation=2, e.labels_bisimulation=3;
  e.states_branching_bisimulation=1, e.transitions_branching_bisimulation=2, e.labels_branching_bisimulation=3;
  e.states_divergence_preserving_branching_bisimulation=1, e.transitions_divergence_preserving_branching_bisimulation=2,
         e.labels_divergence_preserving_branching_bisimulation=3;
  e.states_weak_bisimulation=1, e.transitions_weak_bisimulation=2, e.labels_weak_bisimulation=3;
  e.states_divergence_preserving_weak_bisimulation=1, e.transitions_divergence_preserving_weak_bisimulation=2,
         e.labels_divergence_preserving_weak_bisimulation=4;
  e.states_simulation=1, e.transitions_simulation=2, e.labels_simulation=3;
  e.states_trace_equivalence=1, e.transitions_trace_equivalence=2, e.labels_trace_equivalence=3;
  e.states_weak_trace_equivalence=1, e.transitions_weak_trace_equivalence=2, e.labels_weak_trace_equivalence=3;
  e.states_determinisation=1, e.transitions_determinisation=2, e.labels_determinisation=3;
  e.is_deterministic=true;

  BOOST_CHECK(reduce_lts_in_various_ways("Test15", test15, e));
}
  
// Tricky test for branching bisimulation. If the co-split is not properly implemented
// this example reduces to two states, which is wrong. 
BOOST_AUTO_TEST_CASE(Test16)
{
  const std::string test16=
    "des(0,5,3)\n"
    "(0,\"a\",0)\n"
    "(0,\"b\",0)\n"
    "(0,\"tau\",1)\n"
    "(1,\"b\",1)\n"
    "(1,\"a\",2)\n";

  expected_sizes e;
  e.states_plain=3; e.transitions_plain=5; e.labels_plain=3;
  e.states_bisimulation=3, e.transitions_bisimulation=5, e.labels_bisimulation=3;
  e.states_branching_bisimulation=3, e.transitions_branching_bisimulation=5, e.labels_branching_bisimulation=3;
  e.states_divergence_preserving_branching_bisimulation=3, e.transitions_divergence_preserving_branching_bisimulation=5,
         e.labels_divergence_preserving_branching_bisimulation=3;
  e.states_weak_bisimulation=3, e.transitions_weak_bisimulation=5, e.labels_weak_bisimulation=3;
  e.states_divergence_preserving_weak_bisimulation=3, e.transitions_divergence_preserving_weak_bisimulation=5,
         e.labels_divergence_preserving_weak_bisimulation=4;
  e.states_simulation=3, e.transitions_simulation=5, e.labels_simulation=3;
  e.states_trace_equivalence=3, e.transitions_trace_equivalence=5, e.labels_trace_equivalence=3;
  e.states_weak_trace_equivalence=1, e.transitions_weak_trace_equivalence=2, e.labels_weak_trace_equivalence=3;
  e.states_determinisation=3, e.transitions_determinisation=5, e.labels_determinisation=3;
  e.is_deterministic=true;

  BOOST_CHECK(reduce_lts_in_various_ways("Test16", test16, e));
}

// Another tricky test for branching bisimulation. The tau is non inert, but when state
// 1 becomes a bottom state, state 1 and 2 are part of the same constellation, not leading
// to a split. Later the tau is not investigated, as the tau is only a co-transition to block
// {2, 3, 4}, but it is not investigated because there is no splitting tau transition. 
BOOST_AUTO_TEST_CASE(Test17)
{
  const std::string test17=
    "des(0, 7, 6)\n"
    "(0,\"a\",1)\n"
    "(0,\"a\",3)\n"
    "(0,\"a\",4)\n"
    "(0,\"a\",5)\n"
    "(1,\"tau\",2)\n"
    "(1,\"b\",2)\n"
    "(5,\"b\",2)\n";

  expected_sizes e;
  e.states_plain=6; e.transitions_plain=7; e.labels_plain=3;
  e.states_bisimulation=4, e.transitions_bisimulation=6, e.labels_bisimulation=3;
  e.states_branching_bisimulation=4, e.transitions_branching_bisimulation=6, e.labels_branching_bisimulation=3;
  e.states_divergence_preserving_branching_bisimulation=4, e.transitions_divergence_preserving_branching_bisimulation=6,
         e.labels_divergence_preserving_branching_bisimulation=3;
  e.states_weak_bisimulation=4, e.transitions_weak_bisimulation=5, e.labels_weak_bisimulation=3;
  e.states_divergence_preserving_weak_bisimulation=4, e.transitions_divergence_preserving_weak_bisimulation=5,
         e.labels_divergence_preserving_weak_bisimulation=4;
  e.states_simulation=3, e.transitions_simulation=3, e.labels_simulation=3;
  e.states_trace_equivalence=3, e.transitions_trace_equivalence=3, e.labels_trace_equivalence=3;
  e.states_weak_trace_equivalence=3, e.transitions_weak_trace_equivalence=2, e.labels_weak_trace_equivalence=3;
  e.states_determinisation=3, e.transitions_determinisation=3, e.labels_determinisation=3;
  e.is_deterministic=false;

  BOOST_CHECK(reduce_lts_in_various_ways("Test17", test17, e));
}

// This example is also interesting as it uses quite a lot of the features of the branching bisimulation 
// reduction algorithms. 
BOOST_AUTO_TEST_CASE(Test18)
{
  const std::string test18=
    "des(0,14,6)\n"
    "(0,\"a\",0)\n"
    "(0,\"b\",0)\n"
    "(1,\"b\",0)\n"
    "(5,\"b\",4)\n"
    "(1,\"tau\",1)\n"
    "(3,\"tau\",2)\n"
    "(0,\"tau\",0)\n"
    "(4,\"tau\",0)\n"
    "(0,\"tau\",2)\n"
    "(4,\"tau\",1)\n"
    "(0,\"tau\",5)\n"
    "(2,\"tau\",2)\n"
    "(4,\"tau\",3)\n"
    "(1,\"tau\",3)\n";

  expected_sizes e;
  e.states_plain=6; e.transitions_plain=14; e.labels_plain=3;
  e.states_bisimulation=5, e.transitions_bisimulation=13, e.labels_bisimulation=3;
  e.states_branching_bisimulation=5, e.transitions_branching_bisimulation=10, e.labels_branching_bisimulation=3;
  e.states_divergence_preserving_branching_bisimulation=5, e.transitions_divergence_preserving_branching_bisimulation=13,
         e.labels_divergence_preserving_branching_bisimulation=3;
  e.states_weak_bisimulation=5, e.transitions_weak_bisimulation=8, e.labels_weak_bisimulation=3;
  e.states_divergence_preserving_weak_bisimulation=5, e.transitions_divergence_preserving_weak_bisimulation=11,
         e.labels_divergence_preserving_weak_bisimulation=4;
  e.states_simulation=1, e.transitions_simulation=3, e.labels_simulation=3;
  e.states_trace_equivalence=1, e.transitions_trace_equivalence=3, e.labels_trace_equivalence=3;
  e.states_weak_trace_equivalence=1, e.transitions_weak_trace_equivalence=2, e.labels_weak_trace_equivalence=3;
  e.states_determinisation=4, e.transitions_determinisation=12, e.labels_determinisation=3;
  e.is_deterministic=false;

  BOOST_CHECK(reduce_lts_in_various_ways("Test18", test18, e));

}

// This example is provided by JFG to test the GJ bisimulation minimisation algorithm;
// the example was handled incorrectly.
BOOST_AUTO_TEST_CASE(Test19)
{
  const std::string test19=
    "des(0,15,6)\n"
    "(1,\"a\",3)\n"
    "(0,\"b\",3)\n"
    "(1,\"b\",3)\n"
    "(3,\"b\",5)\n"
    "(4,\"b\",5)\n"
    "(5,\"b\",0)\n"
    "(0,\"tau\",0)\n"
    "(3,\"tau\",2)\n"
    "(1,\"tau\",2)\n"
    "(0,\"tau\",2)\n"
    "(3,\"tau\",5)\n"
    "(1,\"tau\",3)\n"
    "(0,\"tau\",0)\n"
    "(1,\"tau\",4)\n"
    "(1,\"tau\",1)\n";

  expected_sizes e;
  e.states_plain=6; e.transitions_plain=15; e.labels_plain=3;
  e.states_bisimulation=6, e.transitions_bisimulation=14, e.labels_bisimulation=3;
  e.states_branching_bisimulation=6, e.transitions_branching_bisimulation=12, e.labels_branching_bisimulation=3;
  e.states_divergence_preserving_branching_bisimulation=6, e.transitions_divergence_preserving_branching_bisimulation=14,
         e.labels_divergence_preserving_branching_bisimulation=3;
  e.states_weak_bisimulation=6, e.transitions_weak_bisimulation=11, e.labels_weak_bisimulation=3;
  e.states_divergence_preserving_weak_bisimulation=6, e.transitions_divergence_preserving_weak_bisimulation=13,
         e.labels_divergence_preserving_weak_bisimulation=4;
  e.states_simulation=3, e.transitions_simulation=5, e.labels_simulation=2;
  e.states_trace_equivalence=3, e.transitions_trace_equivalence=5, e.labels_trace_equivalence=3;
  e.states_weak_trace_equivalence=1, e.transitions_weak_trace_equivalence=1, e.labels_weak_trace_equivalence=2;
  e.states_determinisation=5, e.transitions_determinisation=8, e.labels_determinisation=3;
  e.is_deterministic=false;

  BOOST_CHECK(reduce_lts_in_various_ways("Test19", test19, e));

}

// The following examples are provided by JFG to test the version checked in on 20 November 2024
// of the GJ bisimulation minimisation algorithm.
BOOST_AUTO_TEST_CASE(Test20)
{
  const std::string test20=
    "des(0,13,6)\n"
    "(0,\"a\",2)\n"
    "(3,\"a\",4)\n"
    "(2,\"b\",4)\n"
    "(5,\"b\",2)\n"
    "(0,\"tau\",2)\n"
    "(1,\"tau\",1)\n"
    "(1,\"tau\",4)\n"
    "(0,\"tau\",1)\n"
    "(5,\"tau\",5)\n"
    "(0,\"tau\",3)\n"
    "(0,\"tau\",5)\n"
    "(3,\"tau\",5)\n"
    "(4,\"tau\",4)\n";

  expected_sizes e;
  e.states_plain=6; e.transitions_plain=13; e.labels_plain=3;
  e.states_bisimulation=5, e.transitions_bisimulation=11, e.labels_bisimulation=3;
  e.states_branching_bisimulation=5, e.transitions_branching_bisimulation=9, e.labels_branching_bisimulation=3;
  e.states_divergence_preserving_branching_bisimulation=5, e.transitions_divergence_preserving_branching_bisimulation=11,
         e.labels_divergence_preserving_branching_bisimulation=3;
  e.states_weak_bisimulation=5, e.transitions_weak_bisimulation=8, e.labels_weak_bisimulation=3;
  e.states_divergence_preserving_weak_bisimulation=5, e.transitions_divergence_preserving_weak_bisimulation=10,
         e.labels_divergence_preserving_weak_bisimulation=4;
  e.states_simulation=5, e.transitions_simulation=10, e.labels_simulation=3;
  e.states_trace_equivalence=6, e.transitions_trace_equivalence=11, e.labels_trace_equivalence=3;
  e.states_weak_trace_equivalence=3, e.transitions_weak_trace_equivalence=3, e.labels_weak_trace_equivalence=3;
  e.states_determinisation=6, e.transitions_determinisation=11, e.labels_determinisation=3;
  e.is_deterministic=false;

  BOOST_CHECK(reduce_lts_in_various_ways("Test20", test20, e));
}

BOOST_AUTO_TEST_CASE(Test20a)
{
  const std::string test20a=
    "des(0,125,60)\n"
    "(12,\"a\",46)\n"
    "(32,\"a\",37)\n"
    "(52,\"a\",27)\n"
    "(54,\"a\",35)\n"
    "(0,\"b\",57)\n"
    "(1,\"b\",55)\n"
    "(2,\"b\",47)\n"
    "(4,\"b\",8)\n"
    "(6,\"b\",11)\n"
    "(9,\"b\",31)\n"
    "(12,\"b\",52)\n"
    "(14,\"b\",24)\n"
    "(16,\"b\",16)\n"
    "(20,\"b\",58)\n"
    "(21,\"b\",8)\n"
    "(22,\"b\",8)\n"
    "(24,\"b\",48)\n"
    "(25,\"b\",39)\n"
    "(29,\"b\",43)\n"
    "(30,\"b\",4)\n"
    "(31,\"b\",52)\n"
    "(32,\"b\",40)\n"
    "(33,\"b\",57)\n"
    "(39,\"b\",34)\n"
    "(40,\"b\",42)\n"
    "(44,\"b\",58)\n"
    "(46,\"b\",39)\n"
    "(47,\"b\",55)\n"
    "(48,\"b\",44)\n"
    "(50,\"b\",33)\n"
    "(51,\"b\",42)\n"
    "(53,\"b\",21)\n"
    "(56,\"b\",34)\n"
    "(58,\"b\",55)\n"
    "(59,\"b\",37)\n"
    "(22,\"tau\",8)\n"
    "(22,\"tau\",14)\n"
    "(40,\"tau\",46)\n"
    "(35,\"tau\",21)\n"
    "(29,\"tau\",23)\n"
    "(25,\"tau\",53)\n"
    "(58,\"tau\",26)\n"
    "(11,\"tau\",20)\n"
    "(52,\"tau\",3)\n"
    "(58,\"tau\",51)\n"
    "(31,\"tau\",0)\n"
    "(33,\"tau\",57)\n"
    "(35,\"tau\",3)\n"
    "(19,\"tau\",12)\n"
    "(41,\"tau\",59)\n"
    "(18,\"tau\",53)\n"
    "(36,\"tau\",38)\n"
    "(10,\"tau\",34)\n"
    "(55,\"tau\",0)\n"
    "(5,\"tau\",51)\n"
    "(18,\"tau\",58)\n"
    "(15,\"tau\",15)\n"
    "(30,\"tau\",19)\n"
    "(20,\"tau\",37)\n"
    "(11,\"tau\",56)\n"
    "(43,\"tau\",48)\n"
    "(24,\"tau\",26)\n"
    "(23,\"tau\",30)\n"
    "(54,\"tau\",35)\n"
    "(10,\"tau\",27)\n"
    "(21,\"tau\",49)\n"
    "(3,\"tau\",0)\n"
    "(50,\"tau\",29)\n"
    "(53,\"tau\",24)\n"
    "(50,\"tau\",57)\n"
    "(4,\"tau\",9)\n"
    "(53,\"tau\",15)\n"
    "(46,\"tau\",35)\n"
    "(24,\"tau\",18)\n"
    "(39,\"tau\",6)\n"
    "(33,\"tau\",37)\n"
    "(43,\"tau\",34)\n"
    "(43,\"tau\",49)\n"
    "(34,\"tau\",42)\n"
    "(40,\"tau\",6)\n"
    "(47,\"tau\",27)\n"
    "(18,\"tau\",17)\n"
    "(6,\"tau\",28)\n"
    "(6,\"tau\",38)\n"
    "(17,\"tau\",34)\n"
    "(13,\"tau\",42)\n"
    "(45,\"tau\",52)\n"
    "(48,\"tau\",30)\n"
    "(54,\"tau\",14)\n"
    "(0,\"tau\",46)\n"
    "(1,\"tau\",56)\n"
    "(1,\"tau\",56)\n"
    "(41,\"tau\",57)\n"
    "(2,\"tau\",17)\n"
    "(28,\"tau\",54)\n"
    "(18,\"tau\",56)\n"
    "(3,\"tau\",12)\n"
    "(1,\"tau\",18)\n"
    "(58,\"tau\",46)\n"
    "(37,\"tau\",28)\n"
    "(43,\"tau\",20)\n"
    "(23,\"tau\",0)\n"
    "(34,\"tau\",31)\n"
    "(48,\"tau\",22)\n"
    "(2,\"tau\",52)\n"
    "(25,\"tau\",10)\n"
    "(50,\"tau\",36)\n"
    "(20,\"tau\",32)\n"
    "(36,\"tau\",39)\n"
    "(51,\"tau\",19)\n"
    "(47,\"tau\",21)\n"
    "(36,\"tau\",51)\n"
    "(26,\"tau\",35)\n"
    "(44,\"tau\",3)\n"
    "(0,\"tau\",42)\n"
    "(45,\"tau\",37)\n"
    "(13,\"tau\",35)\n"
    "(58,\"tau\",19)\n"
    "(6,\"tau\",48)\n"
    "(3,\"tau\",24)\n"
    "(15,\"tau\",58)\n"
    "(10,\"tau\",29)\n"
    "(26,\"tau\",13)\n"
    "(36,\"tau\",39)\n"
    "(2,\"tau\",16)\n";

  expected_sizes e;
  e.states_plain=60; e.transitions_plain=125; e.labels_plain=3;
  e.states_bisimulation=54, e.transitions_bisimulation=123, e.labels_bisimulation=3;
  e.states_branching_bisimulation=32, e.transitions_branching_bisimulation=86, e.labels_branching_bisimulation=3;
  e.states_divergence_preserving_branching_bisimulation=32, e.transitions_divergence_preserving_branching_bisimulation=87,
         e.labels_divergence_preserving_branching_bisimulation=3;
  e.states_weak_bisimulation=16, e.transitions_weak_bisimulation=32, e.labels_weak_bisimulation=3;
  e.states_divergence_preserving_weak_bisimulation=16, e.transitions_divergence_preserving_weak_bisimulation=33,
         e.labels_divergence_preserving_weak_bisimulation=4;
  e.states_simulation=38, e.transitions_simulation=77, e.labels_simulation=3;
  e.states_trace_equivalence=260, e.transitions_trace_equivalence=619, e.labels_trace_equivalence=3;
  e.states_weak_trace_equivalence=1, e.transitions_weak_trace_equivalence=2, e.labels_weak_trace_equivalence=3;
  e.states_determinisation=325, e.transitions_determinisation=770, e.labels_determinisation=3;
  e.is_deterministic=false;

  BOOST_CHECK(reduce_lts_in_various_ways("Test20a", test20a, e));
}

BOOST_AUTO_TEST_CASE(Test21)
{
  const std::string test21=
    "des(0,101,60)\n"
    "(26,\"a\",0)\n"
    "(29,\"a\",4)\n"
    "(39,\"a\",44)\n"
    "(40,\"a\",15)\n"
    "(48,\"a\",54)\n"
    "(1,\"b\",27)\n"
    "(2,\"b\",11)\n"
    "(3,\"b\",34)\n"
    "(4,\"b\",22)\n"
    "(5,\"b\",47)\n"
    "(6,\"b\",28)\n"
    "(10,\"b\",3)\n"
    "(11,\"b\",24)\n"
    "(13,\"b\",6)\n"
    "(14,\"b\",32)\n"
    "(17,\"b\",22)\n"
    "(21,\"b\",42)\n"
    "(22,\"b\",45)\n"
    "(23,\"b\",58)\n"
    "(24,\"b\",8)\n"
    "(25,\"b\",49)\n"
    "(26,\"b\",7)\n"
    "(28,\"b\",26)\n"
    "(30,\"b\",16)\n"
    "(31,\"b\",52)\n"
    "(33,\"b\",29)\n"
    "(34,\"b\",27)\n"
    "(36,\"b\",41)\n"
    "(38,\"b\",32)\n"
    "(39,\"b\",4)\n"
    "(45,\"b\",12)\n"
    "(47,\"b\",24)\n"
    "(48,\"b\",29)\n"
    "(49,\"b\",14)\n"
    "(50,\"b\",16)\n"
    "(53,\"b\",0)\n"
    "(54,\"b\",8)\n"
    "(55,\"b\",19)\n"
    "(56,\"b\",13)\n"
    "(58,\"b\",14)\n"
    "(59,\"b\",27)\n"
    "(45,\"tau\",46)\n"
    "(51,\"tau\",43)\n"
    "(23,\"tau\",38)\n"
    "(15,\"tau\",11)\n"
    "(21,\"tau\",37)\n"
    "(0,\"tau\",19)\n"
    "(34,\"tau\",32)\n"
    "(19,\"tau\",27)\n"
    "(33,\"tau\",40)\n"
    "(51,\"tau\",52)\n"
    "(40,\"tau\",46)\n"
    "(38,\"tau\",55)\n"
    "(26,\"tau\",32)\n"
    "(35,\"tau\",37)\n"
    "(20,\"tau\",41)\n"
    "(54,\"tau\",51)\n"
    "(28,\"tau\",43)\n"
    "(31,\"tau\",31)\n"
    "(31,\"tau\",3)\n"
    "(39,\"tau\",0)\n"
    "(19,\"tau\",52)\n"
    "(51,\"tau\",57)\n"
    "(18,\"tau\",18)\n"
    "(22,\"tau\",4)\n"
    "(7,\"tau\",28)\n"
    "(43,\"tau\",4)\n"
    "(39,\"tau\",47)\n"
    "(59,\"tau\",45)\n"
    "(47,\"tau\",16)\n"
    "(18,\"tau\",24)\n"
    "(39,\"tau\",7)\n"
    "(16,\"tau\",52)\n"
    "(25,\"tau\",32)\n"
    "(5,\"tau\",58)\n"
    "(13,\"tau\",48)\n"
    "(23,\"tau\",35)\n"
    "(29,\"tau\",36)\n"
    "(57,\"tau\",53)\n"
    "(32,\"tau\",28)\n"
    "(16,\"tau\",25)\n"
    "(29,\"tau\",41)\n"
    "(29,\"tau\",34)\n"
    "(17,\"tau\",51)\n"
    "(9,\"tau\",46)\n"
    "(56,\"tau\",30)\n"
    "(49,\"tau\",21)\n"
    "(23,\"tau\",19)\n"
    "(12,\"tau\",49)\n"
    "(2,\"tau\",22)\n"
    "(16,\"tau\",28)\n"
    "(0,\"tau\",41)\n"
    "(0,\"tau\",45)\n"
    "(50,\"tau\",36)\n"
    "(48,\"tau\",8)\n"
    "(5,\"tau\",22)\n"
    "(13,\"tau\",39)\n"
    "(12,\"tau\",37)\n"
    "(20,\"tau\",18)\n"
    "(46,\"tau\",42)\n"
    "(15,\"tau\",28)\n";

  expected_sizes e;
  e.states_plain=60; e.transitions_plain=101; e.labels_plain=3;
  e.states_bisimulation=49, e.transitions_bisimulation=94, e.labels_bisimulation=3;
  e.states_branching_bisimulation=40, e.transitions_branching_bisimulation=81, e.labels_branching_bisimulation=3;
  e.states_divergence_preserving_branching_bisimulation=41, e.transitions_divergence_preserving_branching_bisimulation=84,
         e.labels_divergence_preserving_branching_bisimulation=3;
  e.states_weak_bisimulation=36, e.transitions_weak_bisimulation=69, e.labels_weak_bisimulation=3;
  e.states_divergence_preserving_weak_bisimulation=37, e.transitions_divergence_preserving_weak_bisimulation=72,
         e.labels_divergence_preserving_weak_bisimulation=4;
  e.states_simulation=14, e.transitions_simulation=20, e.labels_simulation=3;
  e.states_trace_equivalence=14, e.transitions_trace_equivalence=20, e.labels_trace_equivalence=3;
  e.states_weak_trace_equivalence=5, e.transitions_weak_trace_equivalence=6, e.labels_weak_trace_equivalence=3;
  e.states_determinisation=18, e.transitions_determinisation=24, e.labels_determinisation=3;
  e.is_deterministic=false;

  BOOST_CHECK(reduce_lts_in_various_ways("Test21", test21, e));
}

