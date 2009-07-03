// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file rename_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <iterator>
#include <set>
#include <boost/test/minimal.hpp>
#include "mcrl2/data/set_identifier_generator.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/lps/linearise.h"
#include "mcrl2/core/garbage_collection.h"

using namespace std;
using namespace atermpp;
using namespace mcrl2;
using namespace mcrl2::core;
using namespace mcrl2::data;
using namespace mcrl2::lps;
using namespace mcrl2::lps::detail;

const std::string SPECIFICATION =
"% Test Case 3                                                     \n"
"%                                                                 \n"
"% rename:                                                         \n"
"% var                                                             \n"
"%   x:Bool;                                                       \n"
"%   y:Nat;                                                        \n"
"%   z:Nat;                                                        \n"
"% rename                                                          \n"
"%   a(x,y) => a(x,y);                                             \n"
"                                                                  \n"
"act                                                               \n"
"  a: Bool#Nat;                                                    \n"
"                                                                  \n"
"proc                                                              \n"
"  X(x:Bool, y:Nat)= sum z:Nat. (y<=z && z<3) -> a(x,y).X(!x,y+1); \n"
"                                                                  \n"
"init                                                              \n"
"  X(true,0);                                                      \n"
;

const std::string SPECIFICATION2 =
"act a:Nat;                              \n"
"                                        \n"
"map smaller: Nat#Nat -> Bool;           \n"
"                                        \n"
"var x,y : Nat;                          \n"
"                                        \n"
"eqn smaller(x,y) = x < y;               \n"
"                                        \n"
"proc P(n:Nat) = sum m: Nat. a(m). P(m); \n"
"                                        \n"
"init P(0);                              \n"
;

const std::string SPECIFICATION3 =
"act a;                                  \n"
"                                        \n"
"proc P(b:Bool) = a. P(b);               \n"
"                                        \n"
"init P(false);                          \n"
;

void test_lps_rename()
{
  specification spec = linearise(SPECIFICATION);
  linear_process p = spec.process();
  std::set<identifier_string> forbidden_names;
  forbidden_names.insert(identifier_string("x"));
  forbidden_names.insert(identifier_string("y"));
  forbidden_names.insert(identifier_string("z"));
  // linear_process q = rename_summation_variables(p, forbidden_names, "_S");

  // summand_list summands = q.summands();
  // for (summand_list::iterator i = summands.begin(); i != summands.end(); ++i)
  // {
  //   variable_list summation_variables(i->summation_variables());
  //   for (variable_list::iterator j = summation_variables.begin(); j != summation_variables.end(); ++j)
  //   {
  //     BOOST_CHECK(std::find(forbidden_names.begin(), forbidden_names.end(), j->name()) == forbidden_names.end());
  //   }
  // }

  // p = rename_process_parameters(p, forbidden_names, "_P");
  // spec = rename_process_parameters(spec, forbidden_names, "_S");
}

void test_rename()
{
  specification spec = linearise(SPECIFICATION3);
  std::set<identifier_string> forbidden_names;
  // specification spec2 = rename_process_parameters(spec, forbidden_names, "_A");
  // std::cout << "<spec>"  << lps::pp(spec) << std::endl;
  // std::cout << "<spec2>" << lps::pp(spec2) << std::endl;
  // BOOST_CHECK(spec2.process().process_parameters().size() == 1);
  // BOOST_CHECK(spec.process().process_parameters().front().name() == spec2.process().process_parameters().front().name());
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  test_rename();
  core::garbage_collect();
  test_lps_rename();
  core::garbage_collect();

  return 0;
}
