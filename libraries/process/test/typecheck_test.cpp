// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file typecheck_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <boost/test/included/unit_test_framework.hpp>
#include <mcrl2/lps/parse.h>
#include <mcrl2/lps/parelm.h>
#include <mcrl2/lps/specification.h>
#include "mcrl2/core/garbage_collection.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace mcrl2;

// Example provided by Tim Willemse, 7 Oct 2009
const std::string case_1 =
  "map place : List(Nat) -> List(Nat); \n"
  "                                    \n"
  "var l : List(Nat);                  \n"
  "                                    \n"
  "eqn place (l) = head(l) ++ tail(l); \n"
  "                                    \n"
  "init delta;                         \n"
  ;

const std::string case_2 =
  "map place : List(Nat) -> List(Nat); \n"
  "                                    \n"
  "var l : List(Nat);                  \n"
  "                                    \n"
  "eqn place (l) = l ++ tail(l); \n"
  "                                    \n"
  "init delta;                         \n"
  ;

void test_typechecker()
{
  BOOST_CHECK_THROW(process::parse_process_specification(case_1), mcrl2::runtime_error);
  process::parse_process_specification(case_2);
}

BOOST_AUTO_TEST_CASE(test_various)
{
  test_typechecker();
}

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  return 0;
}

/*
int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  test_typechecker();

  return 0;
} */
