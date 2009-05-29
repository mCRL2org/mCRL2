// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file parelm_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <string>
#include <boost/test/minimal.hpp>
#include <boost/algorithm/string.hpp>
#include <mcrl2/core/text_utility.h>
#include <mcrl2/lps/parse.h>
#include <mcrl2/lps/parelm.h>
#include <mcrl2/lps/specification.h>
#include "mcrl2/lps/detail/specification_property_map.h"
#include "mcrl2/core/garbage_collection.h"

using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::lps;

const std::string case_1(
  "act a;\n\n"
  "proc X(i: Nat) = a.X(i);\n\n"
  "init X(2);\n");
const std::string expected_1 = "process_parameter_names = ";

const std::string case_2(
  "act a: Nat;\n\n"
  "proc X(i,j: Nat) = a(i). X(i,j);\n\n"
  "init X(0,1);\n");
const std::string expected_2 = "process_parameter_names = i";

const std::string case_3(
  "act a;\n\n"
  "proc X(i,j: Nat)   = (i == 5) -> a. X(i,j);\n\n"
  "init X(0,1);\n");
const std::string expected_3 = "process_parameter_names = i";

const std::string case_4(
  "act a;\n\n"
  "proc X(i,j: Nat) = a@i.X(i,j);\n\n"
  "init X(0,4);\n");
const std::string expected_4 = "process_parameter_names = i";

const std::string case_5(
  "act a: Nat;\n"
  "act b;\n\n"
  "proc X(i,j,k: Nat) =  a(i).X(k,j,k) +\n"
  "                         b.X(j,j,k);\n\n"
  "init X(1,2,3);");
const std::string expected_5 = "process_parameter_names = i, j, k";

// % non-linear process corresponding to case 6
// act act1, act2, act3: Nat;
// proc X(i: Nat)   = (i <  5) -> act1(i).X(i+1) +
//                    (i == 5) -> act3(i).Y(i, i);
//      Y(i,j: Nat) = act2(j).Y(i,j+1);
// init X(0);
const std::string case_6 =
  "act  act1,act2,act3: Nat;          \n"
  "                                   \n"
  "var  dc1: Nat;                     \n"
  "proc P(s3_X: Pos, i_X,j_X: Nat) =  \n"
  "       (s3_X == 1 && i_X == 5) ->  \n"
  "         act3(i_X) .               \n"
  "         P(2, i_X, i_X)            \n"
  "     + (s3_X == 1 && i_X < 5) ->   \n"
  "         act1(i_X) .               \n"
  "         P(1, i_X + 1, dc1)        \n"
  "     + (s3_X == 2) ->              \n"
  "         act2(j_X) .               \n"
  "         P(2, i_X, j_X + 1);       \n"
  "                                   \n"
  "var  dc: Nat;                      \n"
  "init P(1, 0, dc);                  \n"
  ;

const std::string case_7(
  "act act1, act2, act3: Nat;\n\n"
  "proc X(i,z,j: Nat)   = (i <  5) -> act1(i)@z.X(i+1,z, j) +\n"
  "                       (i == 5) -> act3(i).X(i, j, 4);\n\n"
  "init X(0,5, 1);\n"
);
const std::string expected_7 = "process_parameter_names = i, z, j";

// Example given by Jan Friso. Parameter xi08 is erroneously found.
const std::string case_8 =
  "sort Comp = struct smaller?is_smaller | equal?is_equal | larger?is_larger;                                           \n"
  "                                                                                                                     \n"
  "map  k: Real;                                                                                                        \n"
  "                                                                                                                     \n"
  "eqn  k  =  2;                                                                                                        \n"
  "                                                                                                                     \n"
  "act  get,_get,__get,set,_set,__set: Nat;                                                                             \n"
  "     cs_in,cs_out: Pos;                                                                                              \n"
  "                                                                                                                     \n"
  "proc P(s32_P_init1,s31_P_init1: Pos, id_ID: Nat, xi,xi00,xi01,xi02,xi03,xi04,xi05,xi06,xi07,xi08: Comp) =            \n"
  "    ((((!is_smaller(xi08) && !is_smaller(xi04)) && !is_smaller(xi03)) && !is_smaller(xi02)) && !is_smaller(xi00)) -> \n"
  "         delta;                                                                                                      \n"
  "                                                                                                                     \n"
  " init P(1, 1, 0, equal, equal, equal, larger, equal, larger, equal, equal, equal, equal);                            \n"
  ;
const std::string expected_8 = "process_parameter_names = xi00, xi02, xi03, xi04, xi08";

void test_parelm(const std::string& message, const std::string& spec_text, const std::string& expected_result)
{
  specification s0 = parse_linear_process_specification(spec_text);
  specification s1 = parelm(s0);
  lps::detail::specification_property_map info(s1);  
  BOOST_CHECK(data::detail::compare_property_maps(message, info, expected_result));
}

void test_parelm()
{
  test_parelm("case_1", case_1, expected_1);
  test_parelm("case_2", case_2, expected_2);
  test_parelm("case_3", case_3, expected_3);
  test_parelm("case_4", case_4, expected_4);
  test_parelm("case_5", case_5, expected_5);
  test_parelm("case_7", case_7, expected_7);
  test_parelm("case_8", case_8, expected_8);
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  test_parelm();

  return 0;
}
