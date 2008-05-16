// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file linearization_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <string>
#include <boost/test/minimal.hpp>
#include <mcrl2/lps/mcrl22lps.h>

using namespace mcrl2::lps;

// Parameter i should be removed
const std::string case_1(
  "act a;\n\n"
  "proc X(i: Nat) = a.X(i);\n\n"
  "init X(2);\n");

// Parameter j should be removed
const std::string case_2(
  "act a: Nat;\n\n"
  "proc X(i,j: Nat) = a(i). X(i,j);\n\n"
  "init X(0,1);\n");

// Parameter j should be removed
const std::string case_3(
  "act a;\n\n"
  "proc X(i,j: Nat)   = (i == 5) -> a. X(i,j);\n\n"
  "init X(0,1);\n");

// Parameter j should be removed
const std::string case_4(
  "act a\n\n"
  "proc X(i,j: Nat) = a@i.X(i,j);\n\n"
  "init X(0,4);\n");

// Nothing should be removed
const std::string case_5(
  "act a: Nat;\n"
  "act b;\n\n"
  "proc X(i,j,k: Nat) =  a(i).X(k,j,k) +\n"
  "                         b.X(j,j,k);\n\n"
  "init X(1,2,3);");

// Nothing should be removed
const std::string case_6(
  "act act1, act2, act3: Nat;\n\n"
  "proc X(i: Nat)   = (i <  5) -> act1(i).X(i+1) +\n"
  "                   (i == 5) -> act3(i).Y(i, i);\n"
  "     Y(i,j: Nat) = act2(j).Y(i,j+1);\n\n"
  "init X(0);\n");

const std::string case_7(
  "act act1, act2, act3: Nat;\n\n"
  "proc X(i,z,j: Nat)   = (i <  5) -> act1(i)@z.X(i+1,z, j) +\n"
  "                       (i == 5) -> act3(i).X(i, j, 4);\n\n"
  "init X(0,5, 1);\n"
);

void test_multiple_linearization_calls()
{
  specification spec;
  spec = mcrl22lps(case_1);
  spec = mcrl22lps(case_2);
  spec = mcrl22lps(case_3);
  spec = mcrl22lps(case_4);
  spec = mcrl22lps(case_5);
  spec = mcrl22lps(case_6);
  spec = mcrl22lps(case_7);
}

void test_struct()
{
  std::string text =
  "sort D = struct d1(Nat)?is_d1 | d2(arg2:Nat)?is_d2;\n"
  "                                                   \n"
  "init true->delta;                                  \n"
  ;
  specification spec = mcrl22lps(text); 
}

int test_main(int argc, char** argv )
{
  MCRL2_ATERM_INIT(argc, argv)

  test_struct();
  // test_multiple_linearization_calls();

  return 0;
}
