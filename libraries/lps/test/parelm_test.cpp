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
#include <mcrl2/lps/mcrl22lps.h>
#include <mcrl2/lps/parelm.h>
#include <mcrl2/lps/parse.h>
#include <mcrl2/lps/specification.h>
#include "mcrl2/lps/detail/linear_process_expression_visitor.h"
#include "mcrl2/core/garbage_collection.h"

using namespace mcrl2;
using namespace mcrl2::new_data;
using namespace mcrl2::lps;

const std::string case_1(
  "act a;\n\n"
  "proc X(i: Nat) = a.X(i);\n\n"
  "init X(2);\n");
const std::string removed_1 = "i";

const std::string case_2(
  "act a: Nat;\n\n"
  "proc X(i,j: Nat) = a(i). X(i,j);\n\n"
  "init X(0,1);\n");
const std::string removed_2 = "j";

const std::string case_3(
  "act a;\n\n"
  "proc X(i,j: Nat)   = (i == 5) -> a. X(i,j);\n\n"
  "init X(0,1);\n");
const std::string removed_3 = "j";

const std::string case_4(
  "act a;\n\n"
  "proc X(i,j: Nat) = a@i.X(i,j);\n\n"
  "init X(0,4);\n");
const std::string removed_4 = "j";

const std::string case_5(
  "act a: Nat;\n"
  "act b;\n\n"
  "proc X(i,j,k: Nat) =  a(i).X(k,j,k) +\n"
  "                         b.X(j,j,k);\n\n"
  "init X(1,2,3);");
const std::string removed_5 = "";

const std::string case_6(
  "act act1, act2, act3: Nat;\n\n"
  "proc X(i: Nat)   = (i <  5) -> act1(i).X(i+1) +\n"
  "                   (i == 5) -> act3(i).Y(i, i);\n"
  "     Y(i,j: Nat) = act2(j).Y(i,j+1);\n\n"
  "init X(0);\n");
const std::string removed_6 = "";

const std::string case_7(
  "act act1, act2, act3: Nat;\n\n"
  "proc X(i,z,j: Nat)   = (i <  5) -> act1(i)@z.X(i+1,z, j) +\n"
  "                       (i == 5) -> act3(i).X(i, j, 4);\n\n"
  "init X(0,5, 1);\n"
);
const std::string removed_7 = "";

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
const std::string removed_8 = "id_ID s31_P_init1 s32_P_init1 xi xi01 xi05 xi06 xi07";

inline
bool is_linear(const process_specification& pspec)
{
  return lps::detail::linear_process_expression_visitor().is_linear(pspec);
}

void test_parelm(const std::string& spec_text, const std::string& expected_result)
{
  specification s0;
  process_specification pspec = parse_process_specification(spec_text);
  if (is_linear(pspec))
  {
    s0 = parse_linear_process_specification(spec_text);
  }
  else
  {
    s0 = mcrl22lps(spec_text);
  }
  specification s1 = parelm(s0);
  variable_list v0 = s0.process().process_parameters();
  variable_list v1 = s1.process().process_parameters();

  // create a set of strings set1 that contains the names of expected removed parameters
  std::vector<std::string> removed = core::regex_split(expected_result, "\\s");
  std::set<std::string> set1;
  set1.insert(removed.begin(), removed.end());

  // create a set of strings set2 that contains the names of actually removed parameters
  std::set<std::string> set2;
  for (variable_list::iterator i = v0.begin(); i != v0.end(); i++)
  {
    if (std::find(v1.begin(), v1.end(), *i) == v1.end())
    {
      set2.insert(i->name());
    }
  }
  
  // check if the parelm result is correct
  if (set1 != set2)
  {
    std::cerr << "--- failed test ---\n";
    std::cerr << spec_text << std::endl;
    std::cerr << "expected result " << boost::algorithm::join(set1, " ") << std::endl;
    std::cerr << "computed result " << boost::algorithm::join(set2, " ") << std::endl;
  }
  BOOST_CHECK(set1 == set2);

  // check if both implementations of parelm behave consistently
  BOOST_CHECK(parelm(s0).process().process_parameters() == parelm2(s0).process().process_parameters());

  // check if the number of free variables is unchanged
  BOOST_CHECK(s0.process().free_variables().size() == s1.process().free_variables().size());
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  test_parelm(case_1, removed_1);
  core::garbage_collect();
  test_parelm(case_2, removed_2);
  core::garbage_collect();
  test_parelm(case_3, removed_3);
  core::garbage_collect();
  test_parelm(case_4, removed_4);
  core::garbage_collect();
  test_parelm(case_5, removed_5);
  core::garbage_collect();
  test_parelm(case_6, removed_6);
  core::garbage_collect();
  test_parelm(case_7, removed_7);
  core::garbage_collect();
  test_parelm(case_8, removed_8);
  core::garbage_collect();

  return 0;
}
