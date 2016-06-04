// Author(s): Wieger Wesselink, Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ltsconvert_test.cpp
/// \brief This file contains tests to see whether ltsconvert
//         reduces problems well.

// #include <iostream>
#include <string>
#include <sstream>

#include <boost/test/included/unit_test_framework.hpp>

#include "mcrl2/lts/lts_algorithm.h"
#include "mcrl2/lts/lts_aut.h"

using namespace mcrl2::lts;

static lts_aut_t parse_aut(const std::string& s)
{
  std::stringstream is(s);
  lts_aut_t l;
  l.load(is);
  return l;
}

const std::string test1=
  "des (0,2,2)\n"
  "(0,\"move_to_ERROR\",1)\n"
  "(1,\"move_to_ERROR\",0)\n";

const std::string test2=
  "des (0,2,2)\n"
  "(0,\"move_to_ERROR\",1)\n"
  "(1,\"move_to_ERROR1\",0)\n";

const std::string test3=
  "des (0,3,2)\n"
  "(0,\"tau\",1)\n"
  "(1,\"tau\",0)\n"
  "(0,\"a\",1)\n";

// Example below represents a(tau(b+c)+b) which can be reduced to a(b+c).
const std::string test4=
  "des (0,5,6)\n"
  "(0,\"a\",1)\n"
  "(1,\"tau\",2)\n"
  "(2,\"b\",3)\n"
  "(2,\"c\",4)\n"
  "(1,\"b\",5)\n";

// Example below is inspired by the rhs of Milner's third tau law, a(tau.b+c)=a(tau.b+c)+a.b
// which contains a non inert tau.
const std::string test5=
  "des (0,6,7)\n"
  "(0,\"a\",1)\n"
  "(0,\"a\",2)\n"
  "(1,\"tau\",3)\n"
  "(1,\"c\",4)\n"
  "(3,\"b\",5)\n"
  "(2,\"b\",6)\n";

// Example below is inspired by Milner's third tau law, a(tau.b+c)=a(tau.b+c)+a.b.
// The states 1 and 2 should be identified in weak bisimulation, but not in branching
// bisimulation.
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


// In the term a.Y with Y=tau.Y divergence preserving bisimulation must not remove the tau.
const std::string test6=
  "des (0,2,2)\n"
  "(0,\"a\",1)\n"
  "(1,\"tau\",1)\n";

// Test whether a mixed up sequence of tau's is properly dealt with.
// Example is essentially: tau.X where X=tau.tau.tau.tau.tau.a.X.
const std::string test7=
  "des (0,7,7)\n"
  "(0,\"tau\",4)\n"
  "(2,\"tau\",1)\n"
  "(3,\"tau\",2)\n"
  "(4,\"tau\",3)\n"
  "(1,\"tau\",5)\n"
  "(5,\"tau\",6)\n"
  "(6,\"a\",3)\n";

// Test whether a mixed up sequence of tau's is properly dealt with.
// Example is essentially: tau.X where X=tau.tau.tau.tau.tau.a.X.
const std::string test8=
  "des (0,6,5)\n"
  "(0,\"tau\",1)\n"
  "(0,\"tau\",2)\n"
  "(0,\"tau\",3)\n"
  "(0,\"tau\",4)\n"
  "(3,\"tau\",3)\n"
  "(4,\"tau\",4)\n";

// Test whether a tau-loop at the end of a process is correctly dealt with.
const std::string test9=
  "des (0,4,4)\n"
  "(0,\"a\",1)\n"
  "(1,\"tau\",2)\n"
  "(2,\"tau\",3)\n"
  "(3,\"tau\",1)\n";

// Test whether a tau that does become non inert will act to split blocks.
const std::string test10=
  "des (0,7,8)\n"
  "(0,\"b\",1)\n"
  "(0,\"b\",2)\n"
  "(1,\"a\",3)\n"
  "(1,\"tau\",4)\n"
  "(2,\"a\",5)\n"
  "(2,\"tau\",6)\n"
  "(6,\"a\",7)\n";

// Test whether tau's can repeatedly become non inert
const std::string test11=
  "des (0,5,5)\n"
  "(0,\"tau\",1)\n"
  "(0,\"tau\",3)\n"
  "(1,\"a\",2)\n"
  "(1,\"tau\",3)\n"
  "(3,\"tau\",4)\n";

// The test below caused the tau_star reduction to go astray, as there
// were problems with the reachability check.
const std::string test12=
  "des (0,7,6)\n"
  "(0,\"tau\",1)\n"
  "(0,\"tau\",2)\n"
  "(1,\"tau\",3)\n"
  "(2,\"tau\",4)\n"
  "(2,\"a\",5)\n"
  "(3,\"c\",5)\n"
  "(4,\"b\",5)\n";

// Test whether subtle tau loops are handled properly in divergence preserving weak and branching bisimulation.
const std::string test13=
  "des (0,4,4)\n"
  "(0,\"tau\",1)\n"
  "(1,\"tau\",2)\n"
  "(2,\"tau\",2)\n"
  "(1,\"a\",3)\n";



BOOST_AUTO_TEST_CASE(test_state_space_reductions)
{
  std::cerr << "Test1\n";
  lts_aut_t t1=parse_aut(test1);
  reduce(t1,lts_eq_bisim); //Strong bisimulation reduction
  BOOST_CHECK(t1.num_states()==1 && t1.num_transitions()==1);

  std::cerr << "Test2\n";
  lts_aut_t t2=parse_aut(test2);
  reduce(t2,lts_eq_bisim); //Strong bisimulation reduction
  BOOST_CHECK(t2.num_states()==2 && t2.num_transitions()==2);

  std::cerr << "Test3\n";
  lts_aut_t t3=parse_aut(test3);
  reduce(t3,lts_eq_bisim); //Strong bisimulation reduction
  BOOST_CHECK(t3.num_states()==2 && t3.num_transitions()==3);
  t3=parse_aut(test3);
  reduce(t3,lts_eq_branching_bisim); //Branching bisimulation reduction
  BOOST_CHECK(t3.num_states()==1 && t3.num_transitions()==1);
  t3=parse_aut(test3);
  reduce(t3,lts_eq_branching_bisim_gw); //Branching bisimulation reduction, Groote/Wijs 2016
  BOOST_CHECK(t3.num_states()==1 && t3.num_transitions()==1);
  t3=parse_aut(test3);
  reduce(t3,lts_eq_branching_bisim_gjkw); //Branching bisimulation reduction, Groote/Jansen/Keiren/Wijs
  BOOST_CHECK(t3.num_states()==1 && t3.num_transitions()==1);
  t3=parse_aut(test3);
  reduce(t3,lts_eq_branching_bisim_sigref); //Branching bisimulation reduction sigref
  BOOST_CHECK(t3.num_states()==1 && t3.num_transitions()==1);
  t3=parse_aut(test3);
  reduce(t3,lts_eq_weak_bisim); //Weak bisimulation reduction
  BOOST_CHECK(t3.num_states()==1 && t3.num_transitions()==1);
  t3=parse_aut(test3);
  reduce(t3,lts_eq_weak_bisim_sigref); //Weak bisimulation reduction sigref
  BOOST_CHECK(t3.num_states()==1 && t3.num_transitions()==1);

  //Divergence preserving branching bisimulation reduction
  t3=parse_aut(test3);
  reduce(t3,lts_eq_divergence_preserving_branching_bisim);
  BOOST_CHECK(t3.num_states()==1 && t3.num_transitions()==2);
  t3=parse_aut(test3);
  reduce(t3,lts_eq_divergence_preserving_branching_bisim_gw);
  BOOST_CHECK(t3.num_states()==1 && t3.num_transitions()==2);
  t3=parse_aut(test3);
  reduce(t3,lts_eq_divergence_preserving_branching_bisim_gjkw);
  BOOST_CHECK(t3.num_states()==1 && t3.num_transitions()==2);
  t3=parse_aut(test3);
  reduce(t3,lts_eq_divergence_preserving_branching_bisim_sigref);
  BOOST_CHECK(t3.num_states()==1 && t3.num_transitions()==2);
  t3=parse_aut(test3);
  reduce(t3,lts_eq_divergence_preserving_weak_bisim);
  BOOST_CHECK(t3.num_states()==1 && t3.num_transitions()==2);
  t3=parse_aut(test3);
  reduce(t3,lts_eq_divergence_preserving_weak_bisim_sigref);
  BOOST_CHECK(t3.num_states()==1 && t3.num_transitions()==2);

  std::cerr << "Test4\n";
  lts_aut_t t4=parse_aut(test4);
  reduce(t4,lts_eq_bisim); //Strong bisimulation reduction
  BOOST_CHECK(t4.num_states()==4 && t4.num_transitions()==5);
  t4=parse_aut(test4);
  reduce(t4,lts_eq_branching_bisim); //Branching bisimulation reduction
  BOOST_CHECK(t4.num_states()==3 && t4.num_transitions()==3);
  t4=parse_aut(test4);
  reduce(t4,lts_eq_branching_bisim_gw); //Branching bisimulation reduction, Groote/Wijs
  BOOST_CHECK(t4.num_states()==3 && t4.num_transitions()==3);
  t4=parse_aut(test4);
  reduce(t4,lts_eq_branching_bisim_gjkw); //Branching bisimulation reduction, Groote/Jansen/Keiren/Wijs
  BOOST_CHECK(t4.num_states()==3 && t4.num_transitions()==3);
  t4=parse_aut(test4);
  reduce(t4,lts_eq_branching_bisim_sigref);
  BOOST_CHECK(t4.num_states()==3 && t4.num_transitions()==3);
  t4=parse_aut(test4);
  reduce(t4,lts_eq_weak_bisim); //Weak bisimulation reduction
  BOOST_CHECK(t4.num_states()==3 && t4.num_transitions()==3);
  t4=parse_aut(test4);
  reduce(t4,lts_eq_weak_bisim_sigref);
  BOOST_CHECK(t4.num_states()==3 && t4.num_transitions()==3);

  std::cerr << "Test5\n";
  lts_aut_t t5=parse_aut(test5);
  reduce(t5,lts_eq_bisim); //Strong bisimulation reduction
  BOOST_CHECK(t5.num_states()==4 && t5.num_transitions()==5);
  t5=parse_aut(test5);
  reduce(t5,lts_eq_branching_bisim); //Branching bisimulation reduction
  BOOST_CHECK(t5.num_states()==4 && t5.num_transitions()==5);
  t5=parse_aut(test5);
  reduce(t5,lts_eq_branching_bisim_gw); //Branching bisimulation reduction, Groote/Wijs
  BOOST_CHECK(t5.num_states()==4 && t5.num_transitions()==5);
  t5=parse_aut(test5);
  reduce(t5,lts_eq_branching_bisim_gjkw); //Branching bisimulation reduction, Groote/Jansen/Keiren/Wijs
  BOOST_CHECK(t5.num_states()==4 && t5.num_transitions()==5);
  t5=parse_aut(test5);
  reduce(t5,lts_eq_branching_bisim_sigref);
  BOOST_CHECK(t5.num_states()==4 && t5.num_transitions()==5);
  t5=parse_aut(test5);
  reduce(t5,lts_eq_weak_bisim); //Weak bisimulation reduction
  BOOST_CHECK(t5.num_states()==4 && t5.num_transitions()==6);
  t5=parse_aut(test5);
  reduce(t5,lts_eq_weak_bisim_sigref);
  BOOST_CHECK(t5.num_states()==4 && t5.num_transitions()==6);

  std::cerr << "Test5a\n";
  lts_aut_t t5a=parse_aut(test5a);
  reduce(t5a,lts_eq_bisim); //Strong bisimulation reduction
  BOOST_CHECK(t5a.num_states()==6 && t5a.num_transitions()==8);
  t5a=parse_aut(test5a);
  reduce(t5a,lts_eq_branching_bisim); //Branching bisimulation reduction
  BOOST_CHECK(t5a.num_states()==6 && t5a.num_transitions()==8);
  t5a=parse_aut(test5a);
  reduce(t5a,lts_eq_branching_bisim_gw); //Branching bisimulation reduction, Groote/Wijs
  BOOST_CHECK(t5a.num_states()==6 && t5a.num_transitions()==8);
  t5a=parse_aut(test5a);
  reduce(t5a,lts_eq_branching_bisim_gjkw); //Branching bisimulation reduction, Groote/Jansen/Keiren/Wijs
  BOOST_CHECK(t5a.num_states()==6 && t5a.num_transitions()==8);
  t5a=parse_aut(test5a);
  reduce(t5a,lts_eq_branching_bisim_sigref); //Branching bisimulation reduction sigref
  BOOST_CHECK(t5a.num_states()==6 && t5a.num_transitions()==8);
  t5a=parse_aut(test5a);
  reduce(t5a,lts_eq_weak_bisim); //Weak bisimulation reduction
  BOOST_CHECK(t5a.num_states()==5 && t5a.num_transitions()==7);
  t5a=parse_aut(test5a);
  reduce(t5a,lts_eq_weak_bisim_sigref); //Weak bisimulation reduction sigref
  BOOST_CHECK(t5a.num_states()==5 && t5a.num_transitions()==7);

  std::cerr << "Test6\n";
  lts_aut_t t6=parse_aut(test6);
  reduce(t6,lts_eq_bisim); //Strong bisimulation reduction
  BOOST_CHECK(t6.num_states()==2 && t6.num_transitions()==2);
  std::cerr << "Test6a\n";
  t6=parse_aut(test6);
  reduce(t6,lts_eq_divergence_preserving_branching_bisim); //Divergence preserving branching bisimulation reduction
  BOOST_CHECK(t6.num_states()==2 && t6.num_transitions()==2);
  t6=parse_aut(test6);
  reduce(t6,lts_eq_divergence_preserving_branching_bisim_gw); //Divergence preserving branching bisimulation reduction, Groote/Wijs
  BOOST_CHECK(t6.num_states()==2 && t6.num_transitions()==2);
  t6=parse_aut(test6);
  reduce(t6,lts_eq_divergence_preserving_branching_bisim_gjkw); //Divergence preserving branching bisimulation reduction, Groote/Jansen/Keiren/Wijs
  BOOST_CHECK(t6.num_states()==2 && t6.num_transitions()==2);
  t6=parse_aut(test6);
  reduce(t6,lts_eq_divergence_preserving_branching_bisim_sigref);
  BOOST_CHECK(t6.num_states()==2 && t6.num_transitions()==2);
  t6=parse_aut(test6);
  reduce(t6,lts_eq_divergence_preserving_weak_bisim); //Divergence preserving weak bisimulation reduction
  BOOST_CHECK(t6.num_states()==2 && t6.num_transitions()==2);
  t6=parse_aut(test6);
  reduce(t6,lts_eq_divergence_preserving_weak_bisim_sigref);
  BOOST_CHECK(t6.num_states()==2 && t6.num_transitions()==2);
  std::cerr << "Test6b\n";
  t6=parse_aut(test6);
  reduce(t6,lts_eq_branching_bisim); //Branching bisimulation reduction
  BOOST_CHECK(t6.num_states()==2 && t6.num_transitions()==1);
  t6=parse_aut(test6);
  reduce(t6,lts_eq_branching_bisim_gw); //Branching bisimulation reduction, Groote/Wijs
  BOOST_CHECK(t6.num_states()==2 && t6.num_transitions()==1);
  t6=parse_aut(test6);
  reduce(t6,lts_eq_branching_bisim_gjkw); //Branching bisimulation reduction, Groote/Jansen/Keiren/Wijs
  BOOST_CHECK(t6.num_states()==2 && t6.num_transitions()==1);
  t6=parse_aut(test6);
  reduce(t6,lts_eq_branching_bisim_sigref);
  BOOST_CHECK(t6.num_states()==2 && t6.num_transitions()==1);
  t6=parse_aut(test6);
  reduce(t6,lts_eq_weak_bisim); //Weak bisimulation reduction
  BOOST_CHECK(t6.num_states()==2 && t6.num_transitions()==1);
  t6=parse_aut(test6);
  reduce(t6,lts_eq_weak_bisim_sigref);
  BOOST_CHECK(t6.num_states()==2 && t6.num_transitions()==1);

  std::cerr << "Test7\n";
  lts_aut_t t7=parse_aut(test7);
  reduce(t7,lts_eq_bisim); //Strong bisimulation reduction
  BOOST_CHECK(t7.num_states()==7 && t7.num_transitions()==7);
  t7=parse_aut(test7);
  reduce(t7,lts_eq_divergence_preserving_branching_bisim); //Divergence preserving branching bisimulation reduction
  BOOST_CHECK(t7.num_states()==1 && t7.num_transitions()==1);
  t7=parse_aut(test7);
  reduce(t7,lts_eq_divergence_preserving_branching_bisim_gw); //Divergence preserving branching bisimulation reduction, Groote/Wijs
  BOOST_CHECK(t7.num_states()==1 && t7.num_transitions()==1);
  t7=parse_aut(test7);
  reduce(t7,lts_eq_divergence_preserving_branching_bisim_gjkw); //Divergence preserving branching bisimulation reduction, Groote/Jansen/Keiren/Wijs
  BOOST_CHECK(t7.num_states()==1 && t7.num_transitions()==1);
  t7=parse_aut(test7);
  reduce(t7,lts_eq_divergence_preserving_branching_bisim_sigref);
  BOOST_CHECK(t7.num_states()==1 && t7.num_transitions()==1);
  t7=parse_aut(test7);
  reduce(t7,lts_eq_divergence_preserving_weak_bisim); //Divergence preserving weak bisimulation reduction
  BOOST_CHECK(t7.num_states()==1 && t7.num_transitions()==1);
  t7=parse_aut(test7);
  reduce(t7,lts_eq_divergence_preserving_weak_bisim_sigref);
  BOOST_CHECK(t7.num_states()==1 && t7.num_transitions()==1);
  t7=parse_aut(test7);
  reduce(t7,lts_eq_branching_bisim); //Branching bisimulation reduction
  BOOST_CHECK(t7.num_states()==1 && t7.num_transitions()==1);
  t7=parse_aut(test7);
  reduce(t7,lts_eq_branching_bisim_gw); //Branching bisimulation reduction, Groote/Wijs
  BOOST_CHECK(t7.num_states()==1 && t7.num_transitions()==1);
  t7=parse_aut(test7);
  reduce(t7,lts_eq_branching_bisim_gjkw); //Branching bisimulation reduction, Groote/Jansen/Keiren/Wijs
  BOOST_CHECK(t7.num_states()==1 && t7.num_transitions()==1);
  t7=parse_aut(test7);
  reduce(t7,lts_eq_branching_bisim_sigref);
  BOOST_CHECK(t7.num_states()==1 && t7.num_transitions()==1);
  t7=parse_aut(test7);
  reduce(t7,lts_eq_weak_bisim); //Weak bisimulation reduction
  BOOST_CHECK(t7.num_states()==1 && t7.num_transitions()==1);
  t7=parse_aut(test7);
  reduce(t7,lts_eq_weak_bisim_sigref);
  BOOST_CHECK(t7.num_states()==1 && t7.num_transitions()==1);

  std::cerr << "Test8\n";
  lts_aut_t t8=parse_aut(test8);
  reduce(t8,lts_eq_bisim); //Strong bisimulation reduction
  BOOST_CHECK(t8.num_states()==3 && t8.num_transitions()==3);
  std::cerr << "Test8a\n";
  t8=parse_aut(test8);
  reduce(t8,lts_eq_divergence_preserving_branching_bisim); //Divergence preserving branching bisimulation reduction
  BOOST_CHECK(t8.num_states()==3 && t8.num_transitions()==3);
  t8=parse_aut(test8);
  reduce(t8,lts_eq_divergence_preserving_branching_bisim_gw); //Divergence preserving branching bisimulation reduction, Groote/Wijs
  BOOST_CHECK(t8.num_states()==3 && t8.num_transitions()==3);
  t8=parse_aut(test8);
  reduce(t8,lts_eq_divergence_preserving_branching_bisim_gjkw); //Divergence preserving branching bisimulation reduction, Groote/Jansen/Keiren/Wijs
  BOOST_CHECK(t8.num_states()==3 && t8.num_transitions()==3);
  t8=parse_aut(test8);
  reduce(t8,lts_eq_divergence_preserving_branching_bisim_sigref);
  BOOST_CHECK(t8.num_states()==3 && t8.num_transitions()==3);
  t8=parse_aut(test8);
  reduce(t8,lts_eq_divergence_preserving_weak_bisim); //Divergence preserving weak bisimulation reduction
  BOOST_CHECK(t8.num_states()==3 && t8.num_transitions()==4);
  t8=parse_aut(test8);
  reduce(t8,lts_eq_divergence_preserving_weak_bisim_sigref);
  BOOST_CHECK(t8.num_states()==3 && t8.num_transitions()==4);
  std::cerr << "Test8b\n";
  t8=parse_aut(test8);
  reduce(t8,lts_eq_branching_bisim); //Branching bisimulation reduction
  BOOST_CHECK(t8.num_states()==1 && t8.num_transitions()==0);
  t8=parse_aut(test8);
  reduce(t8,lts_eq_branching_bisim_gw); //Branching bisimulation reduction, Groote/Wijs
  BOOST_CHECK(t8.num_states()==1 && t8.num_transitions()==0);
  t8=parse_aut(test8);
  reduce(t8,lts_eq_branching_bisim_gjkw); //Branching bisimulation reduction, Groote/Jansen/Keiren/Wijs
  BOOST_CHECK(t8.num_states()==1 && t8.num_transitions()==0);
  t8=parse_aut(test8);
  reduce(t8,lts_eq_branching_bisim_sigref);
  BOOST_CHECK(t8.num_states()==1 && t8.num_transitions()==0);
  t8=parse_aut(test8);
  reduce(t8,lts_eq_weak_bisim); //Weak bisimulation reduction
  BOOST_CHECK(t8.num_states()==1 && t8.num_transitions()==0);
  t8=parse_aut(test8);
  reduce(t8,lts_eq_weak_bisim_sigref);
  BOOST_CHECK(t8.num_states()==1 && t8.num_transitions()==0);

  std::cerr << "Test9\n";
  lts_aut_t t9=parse_aut(test9);
  reduce(t9,lts_eq_bisim); //Strong bisimulation reduction
  BOOST_CHECK(t9.num_states()==2 && t9.num_transitions()==2);
  t9=parse_aut(test9);
  reduce(t9,lts_eq_branching_bisim); //Branching bisimulation reduction
  BOOST_CHECK(t9.num_states()==2 && t9.num_transitions()==1);
  t9=parse_aut(test9);
  reduce(t9,lts_eq_branching_bisim_gw); //Branching bisimulation reduction, Groote/Wijs
  BOOST_CHECK(t9.num_states()==2 && t9.num_transitions()==1);
  t9=parse_aut(test9);
  reduce(t9,lts_eq_branching_bisim_gjkw); //Branching bisimulation reduction, Groote/Jansen/Keiren/Wijs
  BOOST_CHECK(t9.num_states()==2 && t9.num_transitions()==1);
  t9=parse_aut(test9);
  reduce(t9,lts_eq_branching_bisim_sigref);
  BOOST_CHECK(t9.num_states()==2 && t9.num_transitions()==1);
  t9=parse_aut(test9);
  reduce(t9,lts_eq_weak_bisim); //Weak bisimulation reduction
  BOOST_CHECK(t9.num_states()==2 && t9.num_transitions()==1);
  t9=parse_aut(test9);
  reduce(t9,lts_eq_weak_bisim_sigref);
  BOOST_CHECK(t9.num_states()==2 && t9.num_transitions()==1);
  t9=parse_aut(test9);
  reduce(t9,lts_eq_divergence_preserving_branching_bisim); //Divergence preserving branching bisimulation reduction
  BOOST_CHECK(t9.num_states()==2 && t9.num_transitions()==2);
  t9=parse_aut(test9);
  reduce(t9,lts_eq_divergence_preserving_branching_bisim_gw); //Divergence preserving branching bisimulation reduction, Groote/Wijs
  BOOST_CHECK(t9.num_states()==2 && t9.num_transitions()==2);
  t9=parse_aut(test9);
  reduce(t9,lts_eq_divergence_preserving_branching_bisim_gjkw); //Divergence preserving branching bisimulation reduction, Groote/Jansen/Keiren/Wijs
  BOOST_CHECK(t9.num_states()==2 && t9.num_transitions()==2);
  t9=parse_aut(test9);
  reduce(t9,lts_eq_divergence_preserving_branching_bisim_sigref);
  BOOST_CHECK(t9.num_states()==2 && t9.num_transitions()==2);
  t9=parse_aut(test9);
  reduce(t9,lts_eq_divergence_preserving_weak_bisim); //Divergence preserving weak bisimulation reduction
  BOOST_CHECK(t9.num_states()==2 && t9.num_transitions()==2);
  t9=parse_aut(test9);
  reduce(t9,lts_eq_divergence_preserving_weak_bisim_sigref);
  BOOST_CHECK(t9.num_states()==2 && t9.num_transitions()==2);

  std::cerr << "Test10\n";
  lts_aut_t t10=parse_aut(test10);
  reduce(t10,lts_eq_bisim); //Strong bisimulation reduction
  BOOST_CHECK(t10.num_states()==5 && t10.num_transitions()==7);
  t10=parse_aut(test10);
  reduce(t10,lts_eq_branching_bisim); //Branching bisimulation reduction
  BOOST_CHECK(t10.num_states()==4 && t10.num_transitions()==5);
  t10=parse_aut(test10);
  reduce(t10,lts_eq_branching_bisim_gw); //Branching bisimulation reduction, Groote/Wijs
  BOOST_CHECK(t10.num_states()==4 && t10.num_transitions()==5);
  t10=parse_aut(test10);
  reduce(t10,lts_eq_branching_bisim_gjkw); //Branching bisimulation reduction, Groote/Jansen/Keiren/Wijs
  BOOST_CHECK(t10.num_states()==4 && t10.num_transitions()==5);
  t10=parse_aut(test10);
  reduce(t10,lts_eq_branching_bisim_sigref);
  BOOST_CHECK(t10.num_states()==4 && t10.num_transitions()==5);
  t10=parse_aut(test10);
  reduce(t10,lts_eq_weak_bisim); //Weak bisimulation reduction
  BOOST_CHECK(t10.num_states()==4 && t10.num_transitions()==6);
  t10=parse_aut(test10);
  reduce(t10,lts_eq_weak_bisim_sigref);
  BOOST_CHECK(t10.num_states()==4 && t10.num_transitions()==6);
  t10=parse_aut(test10);
  reduce(t10,lts_eq_divergence_preserving_branching_bisim); //Divergence preserving branching bisimulation reduction
  BOOST_CHECK(t10.num_states()==4 && t10.num_transitions()==5);
  t10=parse_aut(test10);
  reduce(t10,lts_eq_divergence_preserving_branching_bisim_gw); //Divergence preserving branching bisimulation reduction, Groote/Wijs
  BOOST_CHECK(t10.num_states()==4 && t10.num_transitions()==5);
  t10=parse_aut(test10);
  reduce(t10,lts_eq_divergence_preserving_branching_bisim_gjkw); //Divergence preserving branching bisimulation reduction, Groote/Jansen/Keiren/Wijs
  BOOST_CHECK(t10.num_states()==4 && t10.num_transitions()==5);
  t10=parse_aut(test10);
  reduce(t10,lts_eq_divergence_preserving_branching_bisim_sigref);
  BOOST_CHECK(t10.num_states()==4 && t10.num_transitions()==5);
  t10=parse_aut(test10);
  reduce(t10,lts_eq_divergence_preserving_weak_bisim); //Divergence preserving weak bisimulation reduction
  BOOST_CHECK(t10.num_states()==4 && t10.num_transitions()==6);
  t10=parse_aut(test10);
  reduce(t10,lts_eq_divergence_preserving_weak_bisim_sigref);
  BOOST_CHECK(t10.num_states()==4 && t10.num_transitions()==6);

  std::cerr << "Test11\n";
  lts_aut_t t11=parse_aut(test11);
  reduce(t11,lts_eq_bisim); //Strong bisimulation reduction
  BOOST_CHECK(t11.num_states()==4 && t11.num_transitions()==5);
  t11=parse_aut(test11);
  reduce(t11,lts_eq_branching_bisim); //Branching bisimulation reduction
  BOOST_CHECK(t11.num_states()==2 && t11.num_transitions()==2);
  t11=parse_aut(test11);
  reduce(t11,lts_eq_branching_bisim_gw); //Branching bisimulation reduction, Groote/Wijs
  BOOST_CHECK(t11.num_states()==2 && t11.num_transitions()==2);
  t11=parse_aut(test11);
  reduce(t11,lts_eq_branching_bisim_gjkw); //Branching bisimulation reduction, Groote/Jansen/Keiren/Wijs
  BOOST_CHECK(t11.num_states()==2 && t11.num_transitions()==2);
  t11=parse_aut(test11);
  reduce(t11,lts_eq_branching_bisim_sigref);
  BOOST_CHECK(t11.num_states()==2 && t11.num_transitions()==2);
  t11=parse_aut(test11);
  reduce(t11,lts_eq_weak_bisim); //Weak bisimulation reduction
  BOOST_CHECK(t11.num_states()==2 && t11.num_transitions()==2);
  t11=parse_aut(test11);
  reduce(t11,lts_eq_weak_bisim_sigref);
  BOOST_CHECK(t11.num_states()==2 && t11.num_transitions()==2);
  t11=parse_aut(test11);
  reduce(t11,lts_eq_divergence_preserving_branching_bisim); //Divergence preserving branching bisimulation reduction
  BOOST_CHECK(t11.num_states()==2 && t11.num_transitions()==2);
  t11=parse_aut(test11);
  reduce(t11,lts_eq_divergence_preserving_branching_bisim_gw); //Divergence preserving branching bisimulation reduction, Groote/Wijs
  BOOST_CHECK(t11.num_states()==2 && t11.num_transitions()==2);
  t11=parse_aut(test11);
  reduce(t11,lts_eq_divergence_preserving_branching_bisim_gjkw); //Divergence preserving branching bisimulation reduction, Groote/Jansen/Keiren/Wijs
  BOOST_CHECK(t11.num_states()==2 && t11.num_transitions()==2);
  t11=parse_aut(test11);
  reduce(t11,lts_eq_divergence_preserving_branching_bisim_sigref);
  BOOST_CHECK(t11.num_states()==2 && t11.num_transitions()==2);
  t11=parse_aut(test11);
  reduce(t11,lts_eq_divergence_preserving_weak_bisim); //Divergence preserving weak bisimulation reduction
  BOOST_CHECK(t11.num_states()==2 && t11.num_transitions()==2);
  t11=parse_aut(test11);
  reduce(t11,lts_eq_divergence_preserving_weak_bisim_sigref);
  BOOST_CHECK(t11.num_states()==2 && t11.num_transitions()==2);

  std::cerr << "Test12\n";
  lts_aut_t t12=parse_aut(test12);
  reduce(t12,lts_eq_bisim); //Strong bisimulation reduction
  BOOST_CHECK(t12.num_states()==6 && t12.num_transitions()==7);
  t12=parse_aut(test12);
  reduce(t12,lts_eq_branching_bisim); //Branching bisimulation reduction
  BOOST_CHECK(t12.num_states()==5 && t12.num_transitions()==6);
  t12=parse_aut(test12);
  reduce(t12,lts_eq_branching_bisim_gw); //Branching bisimulation reduction, Groote/Wijs
  BOOST_CHECK(t12.num_states()==5 && t12.num_transitions()==6);
  t12=parse_aut(test12);
  reduce(t12,lts_eq_branching_bisim_gjkw); //Branching bisimulation reduction, Groote/Jansen/Keiren/Wijs
  BOOST_CHECK(t12.num_states()==5 && t12.num_transitions()==6);
  t12=parse_aut(test12);
  reduce(t12,lts_eq_branching_bisim_sigref);
  BOOST_CHECK(t12.num_states()==5 && t12.num_transitions()==6);
  t12=parse_aut(test12);
  reduce(t12,lts_eq_weak_bisim); //Weak bisimulation reduction
  BOOST_CHECK(t12.num_states()==5 && t12.num_transitions()==11);
  t12=parse_aut(test12);
  reduce(t12,lts_eq_weak_bisim_sigref);
  BOOST_CHECK(t12.num_states()==5 && t12.num_transitions()==11);
  t12=parse_aut(test12);
  reduce(t12,lts_eq_divergence_preserving_branching_bisim); //Divergence preserving branching bisimulation reduction
  BOOST_CHECK(t12.num_states()==5 && t12.num_transitions()==6);
  t12=parse_aut(test12);
  reduce(t12,lts_eq_divergence_preserving_branching_bisim_gw); //Divergence preserving branching bisimulation reduction, Groote/Wijs
  BOOST_CHECK(t12.num_states()==5 && t12.num_transitions()==6);
  t12=parse_aut(test12);
  reduce(t12,lts_eq_divergence_preserving_branching_bisim_gjkw); //Divergence preserving branching bisimulation reduction, Groote/Jansen/Keiren/Wijs
  BOOST_CHECK(t12.num_states()==5 && t12.num_transitions()==6);
  t12=parse_aut(test12);
  reduce(t12,lts_eq_divergence_preserving_branching_bisim_sigref);
  BOOST_CHECK(t12.num_states()==5 && t12.num_transitions()==6);
  t12=parse_aut(test12);
  reduce(t12,lts_eq_divergence_preserving_weak_bisim); //Divergence preserving weak bisimulation reduction
  BOOST_CHECK(t12.num_states()==5 && t12.num_transitions()==11);
  t12=parse_aut(test12);
  reduce(t12,lts_eq_divergence_preserving_weak_bisim_sigref);
  BOOST_CHECK(t12.num_states()==5 && t12.num_transitions()==11);
  t12=parse_aut(test12);
  reduce(t12,lts_red_tau_star); //Tau star reduction
  BOOST_CHECK(t12.num_states()==2 && t12.num_transitions()==3);

  std::cerr << "Test13\n";
  lts_aut_t t13=parse_aut(test13);
  reduce(t13,lts_eq_bisim); //Strong bisimulation reduction
  BOOST_CHECK(t13.num_states()==4 && t13.num_transitions()==4);
  t13=parse_aut(test13);
  reduce(t13,lts_eq_branching_bisim); //Branching bisimulation reduction
  BOOST_CHECK(t13.num_states()==2 && t13.num_transitions()==2);
  t13=parse_aut(test13);
  reduce(t13,lts_eq_branching_bisim_gw); //Branching bisimulation reduction, Groote/Wijs
  BOOST_CHECK(t13.num_states()==2 && t13.num_transitions()==2);
  t13=parse_aut(test13);
  reduce(t13,lts_eq_branching_bisim_gjkw); //Branching bisimulation reduction, Groote/Jansen/Keiren/Wijs
  BOOST_CHECK(t13.num_states()==2 && t13.num_transitions()==2);
  t13=parse_aut(test13);
  reduce(t13,lts_eq_branching_bisim_sigref);
  BOOST_CHECK(t13.num_states()==2 && t13.num_transitions()==2);
  t13=parse_aut(test13);
  reduce(t13,lts_eq_weak_bisim); //Weak bisimulation reduction
  BOOST_CHECK(t13.num_states()==2 && t13.num_transitions()==2);
  t13=parse_aut(test13);
  reduce(t13,lts_eq_weak_bisim_sigref);
  BOOST_CHECK(t13.num_states()==2 && t13.num_transitions()==2);
  t13=parse_aut(test13);
  reduce(t13,lts_eq_divergence_preserving_branching_bisim); //Divergence preserving branching bisimulation reduction
  BOOST_CHECK(t13.num_states()==3 && t13.num_transitions()==3);
  t13=parse_aut(test13);
  reduce(t13,lts_eq_divergence_preserving_branching_bisim_gw); //Divergence preserving branching bisimulation reduction, Groote/Wijs
  BOOST_CHECK(t13.num_states()==3 && t13.num_transitions()==3);
  t13=parse_aut(test13);
  reduce(t13,lts_eq_divergence_preserving_branching_bisim_gjkw); //Divergence preserving branching bisimulation reduction, Groote/Jansen/Keiren/Wijs
  BOOST_CHECK(t13.num_states()==3 && t13.num_transitions()==3);
  t13=parse_aut(test13);
  reduce(t13,lts_eq_divergence_preserving_branching_bisim_sigref);
  BOOST_CHECK(t13.num_states()==3 && t13.num_transitions()==3);
  t13=parse_aut(test13);
  reduce(t13,lts_eq_divergence_preserving_weak_bisim); //Divergence preserving weak bisimulation reduction
  BOOST_CHECK(t13.num_states()==3 && t13.num_transitions()==4);
  t13=parse_aut(test13);
  reduce(t13,lts_eq_divergence_preserving_weak_bisim_sigref);
  BOOST_CHECK(t13.num_states()==3 && t13.num_transitions()==4);
  t13=parse_aut(test13);
  reduce(t13,lts_red_tau_star); //Tau star reduction
  BOOST_CHECK(t13.num_states()==2 && t13.num_transitions()==1);
}

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
  return 0;
}

