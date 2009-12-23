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

// Test case corresponding to issue #528
const std::string case_3 =
  "sort S = struct c;                  \n"
  "map succ: S -> S;                   \n"
  "eqn succ(c) = c;                    \n"
  "init delta;                         \n"
  ;

// First test case corresponding to issue #663
const std::string case_4 =
  "map const: Pos;                  \n"
  "eqn const = 10;                  \n"
  "                                 \n"
  "proc P1(i: Nat) = delta;         \n"
  "                                 \n"
  "init P1(const);                  \n"
  ;

// Second test case corresponding to issue #663
const std::string case_5 =
  "map const: Pos;                  \n"
  "eqn const = 10;                  \n"
  "                                 \n"
  "proc P1(i: Nat) = delta;         \n"
  "                                 \n"
  "init P1(Nat2Pos(Pos2Nat(const)));\n"
  ;

// Third test case corresponding to issue #663
const std::string case_5a =
  "map const: Pos;                  \n"
  "eqn const = 10;                  \n"
  "                                 \n"
  "proc P1(i: Nat) = delta;         \n"
  "                                 \n"
  "init P1(Pos2Nat(const));\n"
  ;

// Test case for issue #644
const std::string case_6 =
  "const maybe: Bool;               \n"
  "init delta;                      \n"
  ;

// First test case for issue #626
const std::string case_7 =
  "sort S = struct c( x: Int );     \n"
  "                                 \n"
  "map f :(S -> Bool)#S -> S;       \n"
  "var pred: S -> Bool;             \n"
  "    s: S;                        \n"
  "eqn f (pred, s) = s;             \n"
  "                                 \n"
  "act a: (S);                      \n"
  "                                 \n"
  "init a( f( lambda x:S. x(x) < 0 , c( 0 ) ) );\n"
  ;

// Second test case for issue #626
const std::string case_8 =
  "sort S = struct c( x: Int );     \n"
  "                                 \n"
  "map f :(S -> Bool)#S -> S;       \n"
  "var pred: S -> Bool;             \n"
  "    s: S;                        \n"
  "eqn f (pred, s) = s;             \n"
  "                                 \n"
  "act a: (S);                      \n"
  "                                 \n"
  "init a( f( lambda i:S. x(i) < 0 , c( 0 ) ) );\n"
  ;

// First test case for issue #629, empty sorts
const std::string case_9 =
  "sort L_1=struct insert(L_1) ;    \n"
  "init delta;                      \n"
  ;

// Second test case for issue #629, empty sorts
const std::string case_10 =
  "sort L_2=struct insert(L_3);     \n"
  "     L_3=struct insert(L_2);     \n"
  "init delta;                      \n"
  ;

// Third test case for issue #629, empty sorts
const std::string case_11 =
  "sort D;                          \n"
  "cons f:D->D;                     \n"
  "init delta;                      \n"
  ;

void test_typechecker_case(std::string const& spec, bool const expected_result)
{
  std::clog << std::endl
            << "<---- testing specification: ---->" << std::endl
            << spec << std::endl;
  if(expected_result)
  {
    std::clog << "expected result: success" << std::endl;
    process::parse_process_specification(spec);
  }
  else
  {
    std::clog << "expected result:: failure" << std::endl;
    BOOST_CHECK_THROW(process::parse_process_specification(spec), mcrl2::runtime_error);
  }
}

BOOST_AUTO_TEST_CASE(test_various)
{
  // Mapping containing all cases, with expected result,
  // Note that false means that an mcrl2::runtime error is expected.
  std::map<std::string, bool> cases;
  cases[case_1] = false;
  cases[case_2] = true;
  cases[case_3] = true;
  cases[case_4] = true;
  cases[case_5] = true;
  cases[case_5a] = true;
  cases[case_6] = false;
  cases[case_7] = true;
  cases[case_8] = true;
  cases[case_9] = false;
  cases[case_10] = false;
  cases[case_11] = false;

  for(std::map<std::string, bool>::const_iterator i = cases.begin(); i != cases.end(); ++i)
  {
    test_typechecker_case(i->first, i->second);
  }
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
