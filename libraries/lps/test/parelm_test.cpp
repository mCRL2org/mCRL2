// Author(s): Wieger Wesselink
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
#include <mcrl2/lps/specification.h>
#include <mcrl2/lps/parelm.h>
#include <mcrl2/lps/mcrl22lps.h>
#include "gc.h" // garbage collector of ATerm Library

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
  "act a;\n\n"
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

int test_main(int argc, char** argv)
{
  using namespace lps;

  MCRL2_CORE_LIBRARY_INIT()

  specification s0;
  specification s1;
  data_variable_list v0;  
  data_variable_list v1;

  // case 1
 s0 = mcrl22lps(case_1);
 s1 = parelm(s0);
 v0 = s0.process().process_parameters(); 
 v1 = s1.process().process_parameters(); 
 BOOST_CHECK(v1.empty());

 // case 2
 s0 = mcrl22lps(case_2);
 s1 = parelm(s0);
 v0 = s0.process().process_parameters(); 
 v1 = s1.process().process_parameters();
 BOOST_CHECK((v1.size() == 1) && (v1.front() == data_variable("i:Nat")));

//  // case 3
 s0 = mcrl22lps(case_3);
 s1 = parelm(s0);
 v0 = s0.process().process_parameters(); 
 v1 = s1.process().process_parameters(); 
 BOOST_CHECK((v1.size() == 1) && v1.front() == data_variable("i:Nat"));

//  // case 4
 s0 = mcrl22lps(case_4);
 s1 = parelm(s0);
 v0 = s0.process().process_parameters(); 
 v1 = s1.process().process_parameters(); 
 BOOST_CHECK((v1.size() == 1));

//  // case 5
 s0 = mcrl22lps(case_5);
 s1 = parelm(s0);
 v0 = s0.process().process_parameters(); 
 v1 = s1.process().process_parameters(); 
 BOOST_CHECK(v0.size() == v1.size());
 BOOST_CHECK(s0.process().summands().size() == s1.process().summands().size());
 BOOST_CHECK(s0.process().free_variables().size() == s1.process().free_variables().size());

 // case 6
 s0 = mcrl22lps(case_6);
 s1 = parelm(s0);
 v0 = s0.process().process_parameters(); 
 v1 = s1.process().process_parameters(); 
 BOOST_CHECK(v0.size() == v1.size());
 BOOST_CHECK(s0.process().summands().size() == s1.process().summands().size());
 BOOST_CHECK(s0.process().free_variables().size() == s1.process().free_variables().size());

 // case 7
 s0 = mcrl22lps(case_7);
 s1 = parelm(s0);
 v0 = s0.process().process_parameters(); 
 v1 = s1.process().process_parameters(); 
 BOOST_CHECK(v0.size() == v1.size());
 BOOST_CHECK(s0.process().summands().size() == s1.process().summands().size());
 BOOST_CHECK(s0.process().free_variables().size() == s1.process().free_variables().size());

  return 0;
}
