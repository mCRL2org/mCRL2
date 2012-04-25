// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lts_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <sstream>
#include <boost/test/minimal.hpp>
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/lts/lts_algorithm.h"
#include "mcrl2/lts/lts_aut.h"

using namespace mcrl2;

void test_lts(const std::string& test_description,
              const lts::lts_aut_t& l,
              size_t expected_label_count,
              size_t expected_state_count,
              size_t expected_transition_count
             )
{
  std::cout << "LPS test: " << test_description << " -----------------------------------------------\n";
  BOOST_CHECK(l.num_action_labels() == expected_label_count);
  if (l.num_action_labels() != expected_label_count)
  {
    std::cout << "Expected # of labels " << expected_label_count << " Actual # " << l.num_action_labels() << "\n";
  }
  BOOST_CHECK(l.num_states() == expected_state_count);
  if (l.num_states() != expected_state_count)
  {
    std::cout << "Expected # of states " << expected_state_count << " Actual # " << l.num_states() << "\n";
  }
  BOOST_CHECK(l.num_transitions() == expected_transition_count);
  if (l.num_transitions() != expected_transition_count)
  {
    std::cout << "Expected # of transitions " << expected_transition_count << " Actual # " << l.num_transitions() << "\n";
  }
}

void test_abp()
{
  std::string ABP_AUT =
    "des (0,92,74)            \n"
    "(0,\"r1(d1)\",1)         \n"
    "(0,\"r1(d2)\",2)         \n"
    "(1,\"tau\",3)   \n"
    "(2,\"tau\",4)   \n"
    "(3,\"tau\",5)              \n"
    "(3,\"tau\",6)              \n"
    "(4,\"tau\",7)              \n"
    "(4,\"tau\",8)              \n"
    "(5,\"tau\",9)          \n"
    "(6,\"tau\",10)  \n"
    "(7,\"tau\",11)         \n"
    "(8,\"tau\",12)  \n"
    "(9,\"tau\",13)     \n"
    "(10,\"s4(d1)\",14)       \n"
    "(11,\"tau\",15)    \n"
    "(12,\"s4(d2)\",16)       \n"
    "(13,\"tau\",17)            \n"
    "(13,\"tau\",18)            \n"
    "(14,\"tau\",19)     \n"
    "(15,\"tau\",20)            \n"
    "(15,\"tau\",21)            \n"
    "(16,\"tau\",22)     \n"
    "(17,\"tau\",1)         \n"
    "(18,\"tau\",1)     \n"
    "(19,\"tau\",23)            \n"
    "(19,\"tau\",24)            \n"
    "(20,\"tau\",2)         \n"
    "(21,\"tau\",2)     \n"
    "(22,\"tau\",25)            \n"
    "(22,\"tau\",26)            \n"
    "(23,\"tau\",27)        \n"
    "(24,\"tau\",28)     \n"
    "(25,\"tau\",29)        \n"
    "(26,\"tau\",28)     \n"
    "(27,\"tau\",30) \n"
    "(28,\"r1(d1)\",31)       \n"
    "(28,\"r1(d2)\",32)       \n"
    "(29,\"tau\",33)\n"
    "(30,\"tau\",34)\n"
    "(30,\"tau\",35)\n"
    "(31,\"tau\",36)\n"
    "(32,\"tau\",37)\n"
    "(33,\"tau\",38)\n"
    "(33,\"tau\",39)\n"
    "(34,\"tau\",40)\n"
    "(35,\"tau\",40)\n"
    "(36,\"tau\",41)\n"
    "(36,\"tau\",42)\n"
    "(37,\"tau\",43)\n"
    "(37,\"tau\",44)\n"
    "(38,\"tau\",45)\n"
    "(39,\"tau\",45)\n"
    "(40,\"tau\",19)\n"
    "(41,\"tau\",46)\n"
    "(42,\"tau\",47)\n"
    "(43,\"tau\",48)\n"
    "(44,\"tau\",49)\n"
    "(45,\"tau\",22)\n"
    "(46,\"tau\",50)\n"
    "(47,\"s4(d1)\",51)\n"
    "(48,\"tau\",52)\n"
    "(49,\"s4(d2)\",53)\n"
    "(50,\"tau\",54)\n"
    "(50,\"tau\",55)\n"
    "(51,\"tau\",56)\n"
    "(52,\"tau\",57)\n"
    "(52,\"tau\",58)\n"
    "(53,\"tau\",59)    \n"
    "(54,\"tau\",31)        \n"
    "(55,\"tau\",31)     \n"
    "(56,\"tau\",60)            \n"
    "(56,\"tau\",61)            \n"
    "(57,\"tau\",32)        \n"
    "(58,\"tau\",32)     \n"
    "(59,\"tau\",62)            \n"
    "(59,\"tau\",63)            \n"
    "(60,\"tau\",64)        \n"
    "(61,\"tau\",0)     \n"
    "(62,\"tau\",65)        \n"
    "(63,\"tau\",0)     \n"
    "(64,\"tau\",66)\n"
    "(65,\"tau\",67)\n"
    "(66,\"tau\",68)            \n"
    "(66,\"tau\",69)            \n"
    "(67,\"tau\",70)            \n"
    "(67,\"tau\",71)            \n"
    "(68,\"tau\",72)        \n"
    "(69,\"tau\",72)\n"
    "(70,\"tau\",73)        \n"
    "(71,\"tau\",73)\n"
    "(72,\"tau\",56)    \n"
    "(73,\"tau\",59)    \n"
    ;

  size_t expected_label_count = 5;
  size_t expected_state_count = 74;
  size_t expected_transition_count = 92;
  std::istringstream is(ABP_AUT);
  lts::lts_aut_t l_abp;
  l_abp.load(is);
  test_lts("abp test",l_abp, expected_label_count, expected_state_count, expected_transition_count);
  lts::lts_aut_t l=l_abp;
  reduce(l,lts::lts_eq_none);
  test_lts("abp test eq_none",l,expected_label_count, expected_state_count, expected_transition_count);
  l=l_abp;
  reduce(l,lts::lts_eq_bisim);
  test_lts("abp test bisimulation",l,expected_label_count, 24, 28);
  l=l_abp;
  reduce(l,lts::lts_eq_branching_bisim);
  test_lts("abp test branching bisimulation",l,expected_label_count, 3, 4);
  l=l_abp;
  reduce(l,lts::lts_eq_divergence_preserving_branching_bisim);
  test_lts("abp test divergence preserving branching bisimulation",l,expected_label_count, 6, 10);
  l=l_abp;
  reduce(l,lts::lts_eq_sim);
  test_lts("abp test simulation",l,expected_label_count, 24, 28);
  l=l_abp;
  reduce(l,lts::lts_eq_trace);
  test_lts("abp test trace",l,expected_label_count, 19, 24);
  l=l_abp;
  std::cerr << "ACTION LABELS " << l.num_action_labels() << "\n";
  reduce(l,lts::lts_eq_weak_trace);
  test_lts("abp test weak trace",l,5, 3, 4);
  l=l_abp;
  BOOST_CHECK(!is_deterministic(l));
  reduce(l,lts::lts_red_determinisation);
  test_lts("abp test determinisation",l,expected_label_count, 53, 66);
  BOOST_CHECK(is_deterministic(l));
}

void test_reachability()
{
  std::string REACH =
    "des (0,4,5)       \n"
    "(0,\"reachable\",1)\n"
    "(1,\"reachable1\",2)\n"
    "(1,\"reachable2\",3)\n"
    "(4,\"unreachable\",0)\n"
    ;

  size_t expected_label_count = 4;
  size_t expected_state_count = 5;
  size_t expected_transition_count = 4;

  std::istringstream is(REACH);
  lts::lts_aut_t l_reach;
  l_reach.load(is);
  test_lts("reach test",l_reach, expected_label_count, expected_state_count, expected_transition_count);
  BOOST_CHECK(!reachability_check(l_reach,false));
  reachability_check(l_reach,true);
  test_lts("reach test after reachability reduction",l_reach, expected_label_count-1, expected_state_count-1, expected_transition_count-1);
  BOOST_CHECK(reachability_check(l_reach,false));
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  test_abp();
  test_reachability();

  return 0;
}

