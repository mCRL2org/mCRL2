// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file sort_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <string>
#include <boost/test/minimal.hpp>
#include <mcrl2/lps/specification.h>
#include <mcrl2/lps/parelm.h>
#include <mcrl2/lps/mcrl22lps.h>

using namespace atermpp;
using namespace lps;

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

int test_main(int, char*[])
{
  using namespace lps;

  ATerm bottom_of_stack;
  ATinit(0, 0, &bottom_of_stack);
  gsEnableConstructorFunctions(); 

  // case 1
  BOOST_CHECK(parelm(mcrl22lps(case_1).process()).process_parameters().empty());

  // case 2
  data_variable_list v = parelm(mcrl22lps(case_2).process()).process_parameters();

  BOOST_CHECK((v.size() == 1) && (v.front() == data_variable("i:Nat")));

  // case 3
  v = parelm(mcrl22lps(case_3).process()).process_parameters();

  BOOST_CHECK((v.size() == 1) && v.front() == data_variable("i:Nat"));

  // case 4
  v = parelm(mcrl22lps(case_3).process()).process_parameters();

  BOOST_CHECK((v.size() == 1));

  // case 5
  linear_process in(mcrl22lps(case_5).process());
  linear_process out(parelm(in));

  BOOST_CHECK(in.process_parameters().size() == out.process_parameters().size());
  BOOST_CHECK(in.summands().size() == out.summands().size());
  BOOST_CHECK(in.free_variables().size() == out.free_variables().size());

  // case 6
  in  = mcrl22lps(case_6).process();
  out = parelm(in);

  BOOST_CHECK(in.process_parameters().size() == out.process_parameters().size());
  BOOST_CHECK(in.summands().size() == out.summands().size());
  BOOST_CHECK(in.free_variables().size() == out.free_variables().size());

  return 0;
}
