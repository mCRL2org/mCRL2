// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file parse_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <boost/test/minimal.hpp>
#include "mcrl2/lts/parse.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace mcrl2;

void test_fsm()
{
  std::string FSM1 =
    "b(2) Bool \"F\" \"T\" \n"
    "n(2) Nat \"1\" \"2\"  \n"
    "---                   \n"
    "0 0                   \n"
    "0 1                   \n"
    "1 0                   \n"
    "1 1                   \n"
    "---                   \n"
    "1 2 \"increase\"      \n"
    "1 3 \"on\"            \n"
    "2 4 \"on\"            \n"
    "2 1 \"decrease\"      \n"
    "3 1 \"off\"           \n"
    "3 4 \"increase\"      \n"
    "4 2 \"off\"           \n"
    "4 3 \"decrease\"      \n"
    ;
  lts::parse_fsm_specification_new(FSM1);

  std::string FSM2 =
    "b(2) Bool # Bool -> Nat \"F\" \"T\" \n"
    "n(2) Napt -> Nat \"1\" \"2\"        \n"
    "---                                 \n"
    "0 0                                 \n"
    "0 1                                 \n"
    "1 0                                 \n"
    "1 1                                 \n"
    "---                                 \n"
    "1 2 \"increase\"                    \n"
    "1 3 \"on\"                          \n"
    "2 4 \"on\"                          \n"
    "2 1 \"decrease\"                    \n"
    "3 1 \"off\"                         \n"
    "3 4 \"increase\"                    \n"
    "4 2 \"off\"                         \n"
    "4 3 \"decrease\"                    \n"
    ;
  lts::parse_fsm_specification_new(FSM2);
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT_DEBUG(argc, argv)

  test_fsm();

  return 0;
}
