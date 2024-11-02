// Author(s): anonymous
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lts_test.cpp
/// \brief Add your file description here.

#define BOOST_TEST_MODULE lts_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/lts/test/test_reductions.h"

using namespace mcrl2;

BOOST_AUTO_TEST_CASE(reduce_simple_loop)
{
  std::string SIMPLE_AUT =
    "des (0,2,2)\n"
    "(0,\"a\",1)\n"
    "(1,\"a\",0)\n"
    ;

  expected_sizes expected;
  expected.states_plain=2; expected.transitions_plain=2; expected.labels_plain=2;
  expected.states_bisimulation=1, expected.transitions_bisimulation=1, expected.labels_bisimulation=2;
  expected.states_branching_bisimulation=1, expected.transitions_branching_bisimulation=1, expected.labels_branching_bisimulation=2;
  expected.states_divergence_preserving_branching_bisimulation=1, expected.transitions_divergence_preserving_branching_bisimulation=1, expected.labels_divergence_preserving_branching_bisimulation=2;
  expected.states_weak_bisimulation=1, expected.transitions_weak_bisimulation=1, expected.labels_weak_bisimulation=2;
  expected.states_divergence_preserving_weak_bisimulation=1, expected.transitions_divergence_preserving_weak_bisimulation=1, expected.labels_divergence_preserving_weak_bisimulation=3;
  expected.states_simulation=1, expected.transitions_simulation=1, expected.labels_simulation=2;
  expected.states_trace_equivalence=1, expected.transitions_trace_equivalence=1, expected.labels_trace_equivalence=2;
  expected.states_weak_trace_equivalence=1, expected.transitions_weak_trace_equivalence=1, expected.labels_weak_trace_equivalence=2;
  expected.states_determinisation=2, expected.transitions_determinisation=2, expected.labels_determinisation=2;
  expected.is_deterministic=true;

  BOOST_CHECK(reduce_lts_in_various_ways("Simple loop", SIMPLE_AUT, expected));
}

BOOST_AUTO_TEST_CASE(reduce_simple_loop_with_tau)
{
  std::string SIMPLE_AUT =
    "des (0,2,2)\n"
    "(0,\"a\",1)\n"
    "(1,\"tau\",0)\n"
    ;

  expected_sizes expected;
  expected.states_plain=2; expected.transitions_plain=2; expected.labels_plain=2;
  expected.states_bisimulation=2, expected.transitions_bisimulation=2, expected.labels_bisimulation=2;
  expected.states_branching_bisimulation=1, expected.transitions_branching_bisimulation=1, expected.labels_branching_bisimulation=2;
  expected.states_divergence_preserving_branching_bisimulation=1, expected.transitions_divergence_preserving_branching_bisimulation=1, expected.labels_divergence_preserving_branching_bisimulation=2;
  expected.states_weak_bisimulation=1, expected.transitions_weak_bisimulation=1, expected.labels_weak_bisimulation=2;
  expected.states_divergence_preserving_weak_bisimulation=1, expected.transitions_divergence_preserving_weak_bisimulation=1, expected.labels_divergence_preserving_weak_bisimulation=3;
  expected.states_simulation=2, expected.transitions_simulation=2, expected.labels_simulation=2;
  expected.states_trace_equivalence=2, expected.transitions_trace_equivalence=2, expected.labels_trace_equivalence=2;
  expected.states_weak_trace_equivalence=1, expected.transitions_weak_trace_equivalence=1, expected.labels_weak_trace_equivalence=2;
  expected.states_determinisation=2, expected.transitions_determinisation=2, expected.labels_determinisation=2;
  expected.is_deterministic=true;

  BOOST_CHECK(reduce_lts_in_various_ways("Simple loop with tau", SIMPLE_AUT, expected));
}

/* The example below was encountered by David Jansen. The problem is that
 * for branching bisimulations the tau may supersede the b, not leading to the
 * necessary splitting into two equivalence classes. */
BOOST_AUTO_TEST_CASE(tricky_example_for_branching_bisimulation)
{
  std::string TRICKY_BB =
    "des (0,3,2)\n"
    "(0,\"a\",1)\n"
    "(1,\"b\",0)\n"
    "(1,\"tau\",0)\n"
    ;

  expected_sizes expected;
  expected.states_plain=2; expected.transitions_plain=3; expected.labels_plain=3;
  expected.states_bisimulation=2, expected.transitions_bisimulation=3, expected.labels_bisimulation=3;
  expected.states_branching_bisimulation=2, expected.transitions_branching_bisimulation=3, expected.labels_branching_bisimulation=3;
  expected.states_divergence_preserving_branching_bisimulation=2, expected.transitions_divergence_preserving_branching_bisimulation=3, expected.labels_divergence_preserving_branching_bisimulation=3;
  expected.states_weak_bisimulation=2, expected.transitions_weak_bisimulation=3, expected.labels_weak_bisimulation=3;
  expected.states_divergence_preserving_weak_bisimulation=2, expected.transitions_divergence_preserving_weak_bisimulation=3, expected.labels_divergence_preserving_weak_bisimulation=4;
  expected.states_simulation=2, expected.transitions_simulation=3, expected.labels_simulation=3;
  expected.states_trace_equivalence=2, expected.transitions_trace_equivalence=3, expected.labels_trace_equivalence=3;
  expected.states_weak_trace_equivalence=2, expected.transitions_weak_trace_equivalence=3, expected.labels_weak_trace_equivalence=3;
  expected.states_determinisation=2, expected.transitions_determinisation=3, expected.labels_determinisation=3;
  expected.is_deterministic=true;

  BOOST_CHECK(reduce_lts_in_various_ways("Tricky example for branching bisimulation", TRICKY_BB, expected));
}


BOOST_AUTO_TEST_CASE(reduce_abp)
{
  std::string ABP_AUT =
    "des (0,92,74)\n"
    "(0,\"r1(d1)\",1)\n"
    "(0,\"r1(d2)\",2)\n"
    "(1,\"tau\",3)\n"
    "(2,\"tau\",4)\n"
    "(3,\"tau\",5)\n"
    "(3,\"tau\",6)\n"
    "(4,\"tau\",7)\n"
    "(4,\"tau\",8)\n"
    "(5,\"tau\",9)\n"
    "(6,\"tau\",10)\n"
    "(7,\"tau\",11)\n"
    "(8,\"tau\",12)\n"
    "(9,\"tau\",13)\n"
    "(10,\"s4(d1)\",14)\n"
    "(11,\"tau\",15)\n"
    "(12,\"s4(d2)\",16)\n"
    "(13,\"tau\",17)\n"
    "(13,\"tau\",18)\n"
    "(14,\"tau\",19)\n"
    "(15,\"tau\",20)\n"
    "(15,\"tau\",21)\n"
    "(16,\"tau\",22)\n"
    "(17,\"tau\",1)\n"
    "(18,\"tau\",1)\n"
    "(19,\"tau\",23)\n"
    "(19,\"tau\",24)\n"
    "(20,\"tau\",2)\n"
    "(21,\"tau\",2)\n"
    "(22,\"tau\",25)\n"
    "(22,\"tau\",26)\n"
    "(23,\"tau\",27)\n"
    "(24,\"tau\",28)\n"
    "(25,\"tau\",29)\n"
    "(26,\"tau\",28)\n"
    "(27,\"tau\",30) \n"
    "(28,\"r1(d1)\",31)\n"
    "(28,\"r1(d2)\",32)\n"
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
    "(53,\"tau\",59)\n"
    "(54,\"tau\",31)\n"
    "(55,\"tau\",31)\n"
    "(56,\"tau\",60)\n"
    "(56,\"tau\",61)\n"
    "(57,\"tau\",32)\n"
    "(58,\"tau\",32)\n"
    "(59,\"tau\",62)\n"
    "(59,\"tau\",63)\n"
    "(60,\"tau\",64)\n"
    "(61,\"tau\",0)\n"
    "(62,\"tau\",65)\n"
    "(63,\"tau\",0)\n"
    "(64,\"tau\",66)\n"
    "(65,\"tau\",67)\n"
    "(66,\"tau\",68)\n"
    "(66,\"tau\",69)\n"
    "(67,\"tau\",70)\n"
    "(67,\"tau\",71)\n"
    "(68,\"tau\",72)\n"
    "(69,\"tau\",72)\n"
    "(70,\"tau\",73)\n"
    "(71,\"tau\",73)\n"
    "(72,\"tau\",56)\n"
    "(73,\"tau\",59)\n"
    ;

  expected_sizes expected;
  expected.states_plain=74; expected.transitions_plain=92; expected.labels_plain=5;
  expected.states_bisimulation=24, expected.transitions_bisimulation=28, expected.labels_bisimulation=5;
  expected.states_branching_bisimulation=3, expected.transitions_branching_bisimulation=4, expected.labels_branching_bisimulation=5;
  expected.states_divergence_preserving_branching_bisimulation=6, expected.transitions_divergence_preserving_branching_bisimulation=10, expected.labels_divergence_preserving_branching_bisimulation=5;
  expected.states_weak_bisimulation=3, expected.transitions_weak_bisimulation=4, expected.labels_weak_bisimulation=5;
  expected.states_divergence_preserving_weak_bisimulation=6, expected.transitions_divergence_preserving_weak_bisimulation=10, expected.labels_divergence_preserving_weak_bisimulation=6;
  expected.states_simulation=24, expected.transitions_simulation=28, expected.labels_simulation=5;
  expected.states_trace_equivalence=19, expected.transitions_trace_equivalence=24, expected.labels_trace_equivalence=5;
  expected.states_weak_trace_equivalence=3, expected.transitions_weak_trace_equivalence=4, expected.labels_weak_trace_equivalence=5;
  expected.states_determinisation=53, expected.transitions_determinisation=66, expected.labels_determinisation=5;
  expected.is_deterministic=false;

  BOOST_CHECK(reduce_lts_in_various_ways("Alternating bit protocol", ABP_AUT, expected));
}

// Peterson's protocol has the interesting property that the number of states modulo branching bisimulation
// differs from the number of states modulo weak bisimulation, as observed by Rob van Glabbeek.
BOOST_AUTO_TEST_CASE(reduce_peterson)
{
  std::string PETERSON_AUT =
    "des (0,59,35)\n"
    "(0,\"wish(1)\",1)\n"
    "(0,\"wish(0)\",2)\n"
    "(1,\"tau\",3)\n"
    "(1,\"wish(0)\",4)\n"
    "(2,\"wish(1)\",4)\n"
    "(2,\"tau\",5)\n"
    "(3,\"tau\",6)\n"
    "(3,\"wish(0)\",7)\n"
    "(4,\"tau\",7)\n"
    "(4,\"tau\",8)\n"
    "(5,\"wish(1)\",8)\n"
    "(6,\"enter(1)\",9)\n"
    "(6,\"wish(0)\",10)\n"
    "(7,\"tau\",11)\n"
    "(8,\"tau\",12)\n"
    "(9,\"leave(1)\",13)\n"
    "(9,\"wish(0)\",14)\n"
    "(10,\"enter(1)\",14)\n"
    "(10,\"tau\",15)\n"
    "(11,\"tau\",15)\n"
    "(12,\"tau\",16)\n"
    "(13,\"tau\",17)\n"
    "(13,\"wish(0)\",18)\n"
    "(14,\"leave(1)\",18)\n"
    "(14,\"tau\",19)\n"
    "(15,\"enter(1)\",19)\n"
    "(16,\"enter(0)\",20)\n"
    "(17,\"wish(1)\",1)\n"
    "(17,\"wish(0)\",21)\n"
    "(18,\"tau\",21)\n"
    "(18,\"tau\",22)\n"
    "(19,\"leave(1)\",22)\n"
    "(20,\"leave(0)\",23)\n"
    "(21,\"wish(1)\",4)\n"
    "(21,\"tau\",24)\n"
    "(22,\"tau\",24)\n"
    "(23,\"tau\",3)\n"
    "(24,\"wish(1)\",8)\n"
    "(24,\"tau\",25)\n"
    "(25,\"enter(0)\",26)\n"
    "(25,\"wish(1)\",27)\n"
    "(26,\"leave(0)\",28)\n"
    "(26,\"wish(1)\",29)\n"
    "(27,\"enter(0)\",29)\n"
    "(27,\"tau\",16)\n"
    "(28,\"wish(1)\",30)\n"
    "(28,\"tau\",31)\n"
    "(29,\"leave(0)\",30)\n"
    "(29,\"tau\",20)\n"
    "(30,\"tau\",23)\n"
    "(30,\"tau\",32)\n"
    "(31,\"wish(1)\",32)\n"
    "(31,\"wish(0)\",33)\n"
    "(32,\"tau\",3)\n"
    "(32,\"wish(0)\",34)\n"
    "(33,\"wish(1)\",34)\n"
    "(33,\"tau\",24)\n"
    "(34,\"tau\",7)\n"
    "(34,\"tau\",8)\n"
    ;

  expected_sizes expected;
  expected.states_plain=35; expected.transitions_plain=59; expected.labels_plain=7;
  expected.states_bisimulation=31, expected.transitions_bisimulation=51, expected.labels_bisimulation=7;
  expected.states_branching_bisimulation=21, expected.transitions_branching_bisimulation=37, expected.labels_branching_bisimulation=7;
  expected.states_divergence_preserving_branching_bisimulation=21, expected.transitions_divergence_preserving_branching_bisimulation=37, expected.labels_divergence_preserving_branching_bisimulation=7;
  expected.states_weak_bisimulation=19, expected.transitions_weak_bisimulation=33, expected.labels_weak_bisimulation=7;
  expected.states_divergence_preserving_weak_bisimulation=19, expected.transitions_divergence_preserving_weak_bisimulation=33, expected.labels_divergence_preserving_weak_bisimulation=8;
  expected.states_simulation=31, expected.transitions_simulation=49, expected.labels_simulation=7;
  expected.states_trace_equivalence=34, expected.transitions_trace_equivalence=52, expected.labels_trace_equivalence=7;
  expected.states_weak_trace_equivalence=18, expected.transitions_weak_trace_equivalence=29, expected.labels_weak_trace_equivalence=7;
  expected.states_determinisation=40, expected.transitions_determinisation=63, expected.labels_determinisation=7;
  expected.is_deterministic=false;

  BOOST_CHECK(reduce_lts_in_various_ways("Peterson protocol", PETERSON_AUT, expected));
}

BOOST_AUTO_TEST_CASE(test_reachability)
{
  std::string REACH =
    "des (0,4,5)       \n"
    "(0,\"reachable\",1)\n"
    "(1,\"reachable1\",2)\n"
    "(1,\"reachable2\",3)\n"
    "(4,\"unreachable\",0)\n"
    ;

  std::size_t expected_label_count = 5;
  std::size_t expected_state_count = 5;
  std::size_t expected_transition_count = 4;

  std::istringstream is(REACH);
  lts::lts_aut_t l_reach;
  l_reach.load(is);
  BOOST_CHECK(test_lts("reach test",l_reach, expected_label_count, expected_state_count, expected_transition_count));
  BOOST_CHECK(!reachability_check(l_reach,false));
  reachability_check(l_reach,true);
  BOOST_CHECK(test_lts("reach test after reachability reduction",
                       l_reach, expected_label_count-1, expected_state_count-1, expected_transition_count-1));
  BOOST_CHECK(reachability_check(l_reach,false));
}

// The example below caused failures in the GW mlogn branching bisimulation
// algorithm when cleaning the code up.
BOOST_AUTO_TEST_CASE(failing_test_groote_wijs_algorithm)
{
  std::string GWLTS =
    "des(0,29,10)\n"
    "(0,\"a\",5)\n"
    "(1,\"a\",3)\n"
    "(2,\"a\",6)\n"
    "(3,\"a\",8)\n"
    "(4,\"a\",5)\n"
    "(5,\"a\",6)\n"
    "(6,\"a\",4)\n"
    "(7,\"a\",8)\n"
    "(8,\"a\",9)\n"
    "(9,\"a\",6)\n"
    "(0,\"b\",9)\n"
    "(1,\"b\",3)\n"
    "(2,\"b\",3)\n"
    "(3,\"b\",7)\n"
    "(4,\"b\",9)\n"
    "(5,\"b\",8)\n"
    "(6,\"b\",5)\n"
    "(7,\"b\",9)\n"
    "(8,\"b\",1)\n"
    "(9,\"b\",6)\n"
    "(6,\"c\",7)\n"
    "(8,\"c\",9)\n"
    "(2,\"d\",7)\n"
    "(3,\"d\",2)\n"
    "(5,\"d\",6)\n"
    "(7,\"d\",4)\n"
    "(8,\"tau\",0)\n"
    "(4,\"tau\",1)\n"
    "(3,\"tau\",7)\n"
    ;

  std::size_t expected_label_count = 5;
  std::size_t expected_state_count = 10;
  std::size_t expected_transition_count = 29;

  std::istringstream is(GWLTS);
  lts::lts_aut_t l_gw;
  l_gw.load(is);
  lts::lts_aut_t l=l_gw;
  reduce(l,lts::lts_eq_branching_bisim);
  BOOST_CHECK(test_lts("gw problem (branching bisimulation [Jansen/Groote/Keiren/Wijs 2019])",
                       l,expected_label_count, expected_state_count, expected_transition_count));
  l=l_gw;
  reduce(l,lts::lts_eq_branching_bisim_gv);
  BOOST_CHECK(test_lts("gw problem (branching bisimulation [Groote/Vaandrager 1990])",
                       l,expected_label_count, expected_state_count, expected_transition_count));
  l=l_gw;
  reduce(l,lts::lts_eq_branching_bisim_gjkw);
  BOOST_CHECK(test_lts("gw problem (branching bisimulation [Groote/Jansen/Keiren/Wijs 2017])",
                       l,expected_label_count, expected_state_count, expected_transition_count));
  l=l_gw;
  reduce(l,lts::lts_eq_branching_bisim_sigref);
  BOOST_CHECK(test_lts("gw problem (branching bisimulation signature [Blom/Orzan 2003])",
                       l,expected_label_count, expected_state_count, expected_transition_count));
}

// The following counterexample was taken from
// Jansen/Keiren: Stuttering equivalence is too slow! Eprint arXiv: 1603.05789,
// 2016. http://arxiv.org/abs/1603.05789
// It has not been implemented fully. The problem is that it is difficult to
// prescribe the order in which refinements have to be done.

void counterexample_jk_1(std::size_t k)
{
    // numbering scheme of states:
    // states 0..k-1 are the blue squares
    // state k is the orange circle
    // states k+1..2k are the red triangles
    // states 2k+1 and 2k+2 are the grey pentagons
    // The grey diamonds are inserted as extra Kripke states.

    assert(1 < k);
    std::string CJK1 = "des(0," + std::to_string(5*k+2) + "," + std::to_string(2*k+3) + ")\n";

    for (std::size_t i = 0; i < k; ++i)
    {
        CJK1 += "(" + std::to_string(k) + ",a" + std::to_string(i) + "," + std::to_string(k) + ")\n"
                "(" + std::to_string(k) + ",tau," + std::to_string(i) + ")\n"
                "(0,a" + std::to_string(i) + "," + std::to_string(k) + ")\n";
    }
    for (std::size_t i = k-1; i > 0; --i)
    {
        CJK1 += "(" + std::to_string(i) + ",tau," + std::to_string(i-1) + ")\n"
                "(" + std::to_string(i+k+1) + ",tau," + std::to_string(i+k) + ")\n";
    }
    CJK1 += "(" + std::to_string(k+1) + ",tau," + std::to_string(k) + ")\n"
            "(" + std::to_string(2*k+1) + ",a," + std::to_string(2*k+2) + ")\n"
            "(" + std::to_string(k) + ",tau," + std::to_string(2*k+1) + ")\n"
            "(0,tau," + std::to_string(2*k+2) + ")\n";

    std::size_t expected_label_count = k+2;
    std::size_t expected_state_count = 4;
    std::size_t expected_transition_count = 10;

    std::istringstream is(CJK1);
    lts::lts_aut_t l_cjk1;
    l_cjk1.load(is);
    lts::lts_aut_t l=l_cjk1;
    reduce(l,lts::lts_eq_branching_bisim);
    BOOST_CHECK(test_lts("counterexample JK 1 (branching bisimulation [Groote/Jansen/Keiren/Wijs 2017])",
                         l,expected_label_count, expected_state_count, expected_transition_count));
    l=l_cjk1;
    reduce(l,lts::lts_eq_branching_bisim_gv);
    BOOST_CHECK(test_lts("counterexample JK 1 (branching bisimulation [Groote/Vaandrager 1990])",
                         l,expected_label_count, expected_state_count, expected_transition_count));
}


// The following LTS is a counterexample to the algorithm to postprocess new
// bottom states.  Found the error in November 2016.
// The central part of the model consists of states 0--3.  The algorithm will
// split w. r. t. the constellation for "a"-labelled transitions.  This makes
// state 1 blue and the other states red.  Also, state 2 will become a new
// bottom state.  After that, it should split for "b"-labelled transitions, so
// that state 3 will get into a separate block;  however, it will not find this
// label.

// If the algorithm later splits w. r. t. "b", then the final result would
// still be correct.  The model is therefore sensitive to the order in which
// transitions appear.  To remove this sensitivity, I added copies of the
// states for other permutations.

// In the meantime, the bug is corrected:  this is why the first part of the
// algorithm now follows a much simpler line than previously.
BOOST_AUTO_TEST_CASE(counterexample_postprocessing)
{
  std::string POSTPROCESS_AUT =
    "des(0,33,13)\n"
    "(0,\"a\",0)\n"
    "(0,\"b\",0)\n"
    "(0,\"c\",0)\n"

    "(1,\"b\",0)\n"
    "(1,\"c\",0)\n"
    "(2,\"tau\",1)\n"
    "(2,\"a\",0)\n"
    "(3,\"tau\",2)\n"
    "(3,\"a\",0)\n"
    "(3,\"b\",0)\n"
    "(4,\"tau\",2)\n" // state 3 copied, with b and c swapped
    "(4,\"a\",0)\n"
    "(4,\"c\",0)\n"

    "(5,\"a\",0)\n" // states 1-4 copied, with a and b swapped
    "(5,\"c\",0)\n"
    "(6,\"tau\",5)\n"
    "(6,\"b\",0)\n"
    "(7,\"tau\",6)\n"
    "(7,\"a\",0)\n"
    "(7,\"b\",0)\n"
    "(8,\"tau\",6)\n"
    "(8,\"b\",0)\n"
    "(8,\"c\",0)\n"

    "(9,\"a\",0)\n" // states 1-4 copied, with a and c swapped
    "(9,\"b\",0)\n"
    "(10,\"tau\",9)\n"
    "(10,\"c\",0)\n"
    "(11,\"tau\",10)\n"
    "(11,\"b\",0)\n"
    "(11,\"c\",0)\n"
    "(12,\"tau\",10)\n"
    "(12,\"a\",0)\n"
    "(12,\"c\",0)\n"
    ;

  std::size_t expected_label_count = 4;
  std::size_t expected_state_count = 13;
  std::size_t expected_transition_count = 33;

  std::istringstream is(POSTPROCESS_AUT);
  lts::lts_aut_t l_gw;
  l_gw.load(is);
  lts::lts_aut_t l=l_gw;
  reduce(l,lts::lts_eq_branching_bisim);
  BOOST_CHECK(test_lts("postprocessing problem (branching bisimulation [Jansen/Groote/Keiren/Wijs 2019])",
                       l,expected_label_count, expected_state_count, expected_transition_count));
  l=l_gw;
  reduce(l,lts::lts_eq_branching_bisim_gv);
  BOOST_CHECK(test_lts("postprocessing problem (branching bisimulation [Groote/Vaandrager 1990])",
                       l,expected_label_count, expected_state_count, expected_transition_count));
  l=l_gw;
  reduce(l,lts::lts_eq_branching_bisim_gjkw);
  BOOST_CHECK(test_lts("postprocessing problem (branching bisimulation [Groote/Jansen/Keiren/Wijs 2017])",
                       l,expected_label_count, expected_state_count, expected_transition_count));
  l=l_gw;
  reduce(l,lts::lts_eq_branching_bisim_sigref);
  BOOST_CHECK(test_lts("postprocessing problem (branching bisimulation signature [Blom/Orzan 2003])",
                       l,expected_label_count, expected_state_count, expected_transition_count));
}

BOOST_AUTO_TEST_CASE(regression_delete_old_bb_slice)
{
  std::string POSTPROCESS_AUT =
    "des(0,163,100)\n"
    "(33,\"a\",79)\n"
    "(87,\"a\",2)\n"
    "(4,\"b\",35)\n"
    "(13,\"b\",10)\n"
    "(15,\"b\",32)\n"
    "(31,\"b\",3)\n"
    "(35,\"b\",86)\n"
    "(47,\"b\",73)\n"
    "(61,\"b\",10)\n"
    "(67,\"b\",59)\n"
    "(81,\"b\",38)\n"
    "(89,\"b\",77)\n"
    "(94,\"b\",34)\n"
    "(20,\"tau\",10)\n"
    "(65,\"tau\",61)\n"
    "(38,\"tau\",64)\n"
    "(86,\"tau\",41)\n"
    "(18,\"tau\",55)\n"
    "(50,\"tau\",65)\n"
    "(28,\"tau\",50)\n"
    "(29,\"tau\",55)\n"
    "(43,\"tau\",85)\n"
    "(93,\"tau\",8)\n"
    "(54,\"tau\",72)\n"
    "(52,\"tau\",47)\n"
    "(7,\"tau\",36)\n"
    "(6,\"tau\",60)\n"
    "(48,\"tau\",43)\n"
    "(77,\"tau\",68)\n"
    "(90,\"tau\",34)\n"
    "(18,\"tau\",21)\n"
    "(18,\"tau\",56)\n"
    "(36,\"tau\",70)\n"
    "(54,\"tau\",28)\n"
    "(62,\"tau\",40)\n"
    "(5,\"tau\",18)\n"
    "(91,\"tau\",47)\n"
    "(42,\"tau\",27)\n"
    "(34,\"tau\",70)\n"
    "(23,\"tau\",63)\n"
    "(19,\"tau\",70)\n"
    "(39,\"tau\",43)\n"
    "(86,\"tau\",34)\n"
    "(25,\"tau\",73)\n"
    "(16,\"tau\",4)\n"
    "(34,\"tau\",30)\n"
    "(70,\"tau\",20)\n"
    "(30,\"tau\",22)\n"
    "(51,\"tau\",97)\n"
    "(5,\"tau\",67)\n"
    "(80,\"tau\",13)\n"
    "(66,\"tau\",59)\n"
    "(24,\"tau\",23)\n"
    "(95,\"tau\",82)\n"
    "(5,\"tau\",2)\n"
    "(82,\"tau\",9)\n"
    "(40,\"tau\",46)\n"
    "(94,\"tau\",31)\n"
    "(19,\"tau\",96)\n"
    "(34,\"tau\",32)\n"
    "(62,\"tau\",24)\n"
    "(74,\"tau\",8)\n"
    "(9,\"tau\",76)\n"
    "(98,\"tau\",50)\n"
    "(25,\"tau\",62)\n"
    "(89,\"tau\",95)\n"
    "(1,\"tau\",56)\n"
    "(44,\"tau\",66)\n"
    "(1,\"tau\",45)\n"
    "(73,\"tau\",60)\n"
    "(70,\"tau\",98)\n"
    "(36,\"tau\",14)\n"
    "(18,\"tau\",27)\n"
    "(87,\"tau\",27)\n"
    "(65,\"tau\",17)\n"
    "(57,\"tau\",97)\n"
    "(98,\"tau\",8)\n"
    "(29,\"tau\",25)\n"
    "(59,\"tau\",97)\n"
    "(1,\"tau\",94)\n"
    "(30,\"tau\",74)\n"
    "(53,\"tau\",90)\n"
    "(50,\"tau\",19)\n"
    "(41,\"tau\",81)\n"
    "(73,\"tau\",97)\n"
    "(97,\"tau\",62)\n"
    "(40,\"tau\",59)\n"
    "(33,\"tau\",86)\n"
    "(16,\"tau\",47)\n"
    "(50,\"tau\",72)\n"
    "(90,\"tau\",68)\n"
    "(90,\"tau\",63)\n"
    "(17,\"tau\",75)\n"
    "(70,\"tau\",49)\n"
    "(85,\"tau\",33)\n"
    "(25,\"tau\",52)\n"
    "(63,\"tau\",99)\n"
    "(22,\"tau\",29)\n"
    "(47,\"tau\",31)\n"
    "(39,\"tau\",88)\n"
    "(41,\"tau\",88)\n"
    "(49,\"tau\",83)\n"
    "(60,\"tau\",34)\n"
    "(85,\"tau\",59)\n"
    "(12,\"tau\",6)\n"
    "(47,\"tau\",99)\n"
    "(47,\"tau\",23)\n"
    "(77,\"tau\",73)\n"
    "(78,\"tau\",55)\n"
    "(7,\"tau\",6)\n"
    "(0,\"tau\",67)\n"
    "(66,\"tau\",12)\n"
    "(75,\"tau\",31)\n"
    "(25,\"tau\",80)\n"
    "(53,\"tau\",35)\n"
    "(83,\"tau\",79)\n"
    "(74,\"tau\",88)\n"
    "(57,\"tau\",80)\n"
    "(7,\"tau\",77)\n"
    "(77,\"tau\",8)\n"
    "(87,\"tau\",1)\n"
    "(59,\"tau\",54)\n"
    "(66,\"tau\",33)\n"
    "(86,\"tau\",80)\n"
    "(90,\"tau\",30)\n"
    "(1,\"tau\",4)\n"
    "(47,\"tau\",78)\n"
    "(75,\"tau\",47)\n"
    "(26,\"tau\",7)\n"
    "(6,\"tau\",93)\n"
    "(51,\"tau\",14)\n"
    "(7,\"tau\",77)\n"
    "(13,\"tau\",67)\n"
    "(65,\"tau\",77)\n"
    "(41,\"tau\",39)\n"
    "(91,\"tau\",96)\n"
    "(69,\"tau\",38)\n"
    "(71,\"tau\",77)\n"
    "(81,\"tau\",56)\n"
    "(53,\"tau\",44)\n"
    "(50,\"tau\",88)\n"
    "(65,\"tau\",4)\n"
    "(39,\"tau\",49)\n"
    "(82,\"tau\",93)\n"
    "(20,\"tau\",13)\n"
    "(4,\"tau\",49)\n"
    "(16,\"tau\",6)\n"
    "(42,\"tau\",9)\n"
    "(74,\"tau\",3)\n"
    "(17,\"tau\",21)\n"
    "(7,\"tau\",80)\n"
    "(58,\"tau\",84)\n"
    "(74,\"tau\",62)\n"
    "(81,\"tau\",58)\n"
    "(19,\"tau\",87)\n"
    "(42,\"tau\",45)\n"
    "(26,\"tau\",30)\n"
    "(57,\"tau\",87)\n"
    "(57,\"tau\",96)\n"
    "(34,\"tau\",32)\n"
    "(43,\"tau\",11)\n"
    "(53,\"tau\",35)\n"
    "(30,\"tau\",56)\n"
    ;

  std::size_t expected_label_count = 3;
  std::size_t expected_state_count = 17;
  std::size_t expected_transition_count = 43;

  std::istringstream is(POSTPROCESS_AUT);
  lts::lts_aut_t l_gw;
  l_gw.load(is);
  lts::lts_aut_t l=l_gw;
  reduce(l,lts::lts_eq_divergence_preserving_branching_bisim);
  BOOST_CHECK(test_lts("regression test for GJKW bug (branching bisimulation [Jansen/Groote/Keiren/Wijs 2019])",
                       l,expected_label_count, expected_state_count, expected_transition_count));
  l=l_gw;
  reduce(l,lts::lts_eq_divergence_preserving_branching_bisim_gv);
  BOOST_CHECK(test_lts("regression test for GJKW bug (branching bisimulation [Groote/Vaandrager 1990])",
                       l,expected_label_count, expected_state_count, expected_transition_count));
  l=l_gw;
  reduce(l,lts::lts_eq_divergence_preserving_branching_bisim_gjkw);
  BOOST_CHECK(test_lts("regression test for GJKW bug (branching bisimulation [Groote/Jansen/Keiren/Wijs 2017])",
                       l,expected_label_count, expected_state_count, expected_transition_count));
  l=l_gw;
  reduce(l,lts::lts_eq_divergence_preserving_branching_bisim_sigref);
  BOOST_CHECK(test_lts("regression test for GJKW bug (branching bisimulation signature [Blom/Orzan 2003])",
                       l,expected_label_count, expected_state_count, expected_transition_count));
}

BOOST_AUTO_TEST_CASE(is_deterministic_test1)
{
  std::string automaton =
    "des(0,2,2)\n"
    "(0,\"a\",1)\n"
    "(0,\"a\",1)\n";

  std::istringstream is(automaton);
  lts::lts_aut_t l_det;
  l_det.load(is);
  BOOST_CHECK(is_deterministic(l_det));
}

BOOST_AUTO_TEST_CASE(is_deterministic_test2)
{
  std::string automaton =
    "des(0,2,2)\n"
    "(0,\"a\",1)\n"
    "(0,\"a\",0)\n";

  std::istringstream is(automaton);
  lts::lts_aut_t l_det;
  l_det.load(is);
  BOOST_CHECK(!is_deterministic(l_det));
}

BOOST_AUTO_TEST_CASE(hide_actions1)
{
  std::string automaton =
     "des (0,4,3)\n"
     "(0,\"<state>\",1)\n"
     "(1,\"return|hello\",2)\n"
     "(1,\"return\",2)\n"
     "(2,\"world\",1)\n";

  std::istringstream is(automaton);
  lts::lts_aut_t l;
  l.load(is);
  std::vector<std::string>hidden_actions(1,"hello");
  l.apply_hidden_actions(hidden_actions);
  reduce(l,lts::lts_eq_bisim);
  std::size_t expected_label_count = 5;
  std::size_t expected_state_count = 3;
  std::size_t expected_transition_count = 3;
  BOOST_CHECK(test_lts("regression test for GJKW bug (branching bisimulation [Jansen/Groote/Keiren/Wijs 2019])",
                       l,expected_label_count, expected_state_count, expected_transition_count));
}


BOOST_AUTO_TEST_CASE(hide_actions2)
{
  std::string automaton =
     "des (0,4,3)\n"
     "(0,\"state\",1)\n"
     "(1,\"hello\",2)\n"
     "(0,\"return\",2)\n"
     "(2,\"world\",1)\n";

  std::istringstream is(automaton);
  lts::lts_aut_t l;
  l.load(is);
  std::vector<std::string>hidden_actions(1,"hello");
  l.apply_hidden_actions(hidden_actions);
  reduce(l,lts::lts_eq_branching_bisim);
  std::size_t expected_label_count = 5;
  std::size_t expected_state_count = 2;
  std::size_t expected_transition_count = 3;
  BOOST_CHECK(test_lts("regression test for GJKW bug (branching bisimulation [Jansen/Groote/Keiren/Wijs 2019])",
                       l,expected_label_count, expected_state_count, expected_transition_count));
}

BOOST_AUTO_TEST_CASE(hide_actions3)
{
  std::string automaton =
     "des (0,4,3)\n"
     "(0,\"<state>\",1)\n"
     "(1,\"return|hello\",2)\n"
     "(1,\"return\",2)\n"
     "(2,\"world\",1)\n";

  std::istringstream is(automaton);
  lts::lts_aut_t l;
  l.load(is);
  std::vector<std::string>hidden_actions(1,"hello");
  l.record_hidden_actions(hidden_actions);
  reduce(l,lts::lts_eq_bisim);
  std::size_t expected_label_count = 5;
  std::size_t expected_state_count = 3;
  std::size_t expected_transition_count = 3;
  BOOST_CHECK(test_lts("regression test for GJKW bug (branching bisimulation [Jansen/Groote/Keiren/Wijs 2019])",
                       l,expected_label_count, expected_state_count, expected_transition_count));
}

BOOST_AUTO_TEST_CASE(hide_actions4)
{
  std::string automaton =
     "des (0,4,3)\n"
     "(0,\"state\",1)\n"
     "(1,\"hello\",2)\n"
     "(0,\"return\",2)\n"
     "(2,\"world\",1)\n";

  std::istringstream is(automaton);
  lts::lts_aut_t l;
  l.load(is);
  std::vector<std::string>hidden_actions(1,"hello");
  l.record_hidden_actions(hidden_actions);
  reduce(l,lts::lts_eq_branching_bisim);
  std::size_t expected_label_count = 5;
  std::size_t expected_state_count = 2;
  std::size_t expected_transition_count = 3;
  BOOST_CHECK(test_lts("regression test for GJKW bug (branching bisimulation [Jansen/Groote/Keiren/Wijs 2019])",
                       l,expected_label_count, expected_state_count, expected_transition_count));
}

BOOST_AUTO_TEST_CASE(hide_actions5)
{
  std::string automaton =
     "des (0,3,2)\n"
     "(0,\"RA !ADD (0, EMPTYSET) a\",1)\n"
     "(1,\"RA !ADD (0, EMPTYSET)|a\",0)\n"
     "(1,\"RA !ADD (0, EMPTYSET)\",0)\n";

  std::istringstream is(automaton);
  lts::lts_aut_t l;
  l.load(is);
  std::vector<std::string>hidden_actions(1,"a");
  l.record_hidden_actions(hidden_actions);
  reduce(l,lts::lts_eq_branching_bisim);
  std::size_t expected_label_count = 4;
  std::size_t expected_state_count = 2;
  std::size_t expected_transition_count = 2;
  BOOST_CHECK(test_lts("regression test for incorrectly hiding actions with an odd structure (bug #1786)", 
                       l,expected_label_count, expected_state_count, expected_transition_count));
}


