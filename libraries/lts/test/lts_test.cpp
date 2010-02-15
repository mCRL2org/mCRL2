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
#include "mcrl2/core/garbage_collection.h"
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/lts/lts.h"

using namespace mcrl2;

void test_lts(const std::string& aut, 
              unsigned int expected_label_count,
              unsigned int expected_state_count,
              unsigned int expected_transition_count
             )
{
  lts::lts l;
  std::istringstream is(aut);
  l.read_from(is, lts::lts_aut);
  BOOST_CHECK(l.num_labels()      == expected_label_count     );
  BOOST_CHECK(l.num_states()      == expected_state_count     );
  BOOST_CHECK(l.num_transitions() == expected_transition_count);
  core::garbage_collect();
}

void test_abp()
{
  std::string ABP_AUT =
  "des (0,92,74)            \n"
  "(0,\"r1(d1)\",1)         \n"
  "(0,\"r1(d2)\",2)         \n"
  "(1,\"c2(d1, true)\",3)   \n"
  "(2,\"c2(d2, true)\",4)   \n"
  "(3,\"i\",5)              \n"
  "(3,\"i\",6)              \n"
  "(4,\"i\",7)              \n"
  "(4,\"i\",8)              \n"
  "(5,\"c3(e)\",9)          \n"
  "(6,\"c3(d1, true)\",10)  \n"
  "(7,\"c3(e)\",11)         \n"
  "(8,\"c3(d2, true)\",12)  \n"
  "(9,\"c5(false)\",13)     \n"
  "(10,\"s4(d1)\",14)       \n"
  "(11,\"c5(false)\",15)    \n"
  "(12,\"s4(d2)\",16)       \n"
  "(13,\"i\",17)            \n"
  "(13,\"i\",18)            \n"
  "(14,\"c5(true)\",19)     \n"
  "(15,\"i\",20)            \n"
  "(15,\"i\",21)            \n"
  "(16,\"c5(true)\",22)     \n"
  "(17,\"c6(e)\",1)         \n"
  "(18,\"c6(false)\",1)     \n"
  "(19,\"i\",23)            \n"
  "(19,\"i\",24)            \n"
  "(20,\"c6(e)\",2)         \n"
  "(21,\"c6(false)\",2)     \n"
  "(22,\"i\",25)            \n"
  "(22,\"i\",26)            \n"
  "(23,\"c6(e)\",27)        \n"
  "(24,\"c6(true)\",28)     \n"
  "(25,\"c6(e)\",29)        \n"
  "(26,\"c6(true)\",28)     \n"
  "(27,\"c2(d1, true)\",30) \n"
  "(28,\"r1(d1)\",31)       \n"
  "(28,\"r1(d2)\",32)       \n"
  "(29,\"c2(d2, true)\",33) \n"
  "(30,\"i\",34)            \n"
  "(30,\"i\",35)            \n"
  "(31,\"c2(d1, false)\",36)\n"
  "(32,\"c2(d2, false)\",37)\n"
  "(33,\"i\",38)            \n"
  "(33,\"i\",39)            \n"
  "(34,\"c3(e)\",40)        \n"
  "(35,\"c3(d1, true)\",40) \n"
  "(36,\"i\",41)            \n"
  "(36,\"i\",42)            \n"
  "(37,\"i\",43)            \n"
  "(37,\"i\",44)            \n"
  "(38,\"c3(e)\",45)        \n"
  "(39,\"c3(d2, true)\",45) \n"
  "(40,\"c5(true)\",19)     \n"
  "(41,\"c3(e)\",46)        \n"
  "(42,\"c3(d1, false)\",47)\n"
  "(43,\"c3(e)\",48)        \n"
  "(44,\"c3(d2, false)\",49)\n"
  "(45,\"c5(true)\",22)     \n"
  "(46,\"c5(true)\",50)     \n"
  "(47,\"s4(d1)\",51)       \n"
  "(48,\"c5(true)\",52)     \n"
  "(49,\"s4(d2)\",53)       \n"
  "(50,\"i\",54)            \n"
  "(50,\"i\",55)            \n"
  "(51,\"c5(false)\",56)    \n"
  "(52,\"i\",57)            \n"
  "(52,\"i\",58)            \n"
  "(53,\"c5(false)\",59)    \n"
  "(54,\"c6(e)\",31)        \n"
  "(55,\"c6(true)\",31)     \n"
  "(56,\"i\",60)            \n"
  "(56,\"i\",61)            \n"
  "(57,\"c6(e)\",32)        \n"
  "(58,\"c6(true)\",32)     \n"
  "(59,\"i\",62)            \n"
  "(59,\"i\",63)            \n"
  "(60,\"c6(e)\",64)        \n"
  "(61,\"c6(false)\",0)     \n"
  "(62,\"c6(e)\",65)        \n"
  "(63,\"c6(false)\",0)     \n"
  "(64,\"c2(d1, false)\",66)\n"
  "(65,\"c2(d2, false)\",67)\n"
  "(66,\"i\",68)            \n"
  "(66,\"i\",69)            \n"
  "(67,\"i\",70)            \n"
  "(67,\"i\",71)            \n"
  "(68,\"c3(e)\",72)        \n"
  "(69,\"c3(d1, false)\",72)\n"
  "(70,\"c3(e)\",73)        \n"
  "(71,\"c3(d2, false)\",73)\n"
  "(72,\"c5(false)\",56)    \n"
  "(73,\"c5(false)\",59)    \n"
  ;

  unsigned int expected_label_count = 19;
  unsigned int expected_state_count = 74;
  unsigned int expected_transition_count = 92;
  test_lts(ABP_AUT, expected_label_count, expected_state_count, expected_transition_count);
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)
  
  test_abp();

  return 0;
}

