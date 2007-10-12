// Author(s): Wieger Wesselink
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
#include "mcrl2/data/utility.h"
#include "mcrl2/lps/mcrl22lps.h"
#include "mcrl2/lps/rename.h"

using namespace std;
using namespace atermpp;
using namespace lps;
using namespace lps::detail;

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
  
void test_rename()
{
  specification spec = mcrl22lps(SPECIFICATION);
  linear_process p = spec.process();
  std::set<identifier_string> forbidden_names;
  forbidden_names.insert(identifier_string("x"));
  forbidden_names.insert(identifier_string("y"));
  forbidden_names.insert(identifier_string("z"));
  linear_process q = rename_summation_variables(p, forbidden_names);

  for (summand_list::iterator i = q.summands().begin(); i != q.summands().end(); ++i)
  {
    for (data_variable_list::iterator j = i->summation_variables().begin(); j != i->summation_variables().end(); ++j)
    {
      BOOST_CHECK(std::find(forbidden_names.begin(), forbidden_names.end(), j->name()) == forbidden_names.end());
    }
  }
}

int test_main(int argc, char* argv[])
{
  aterm bottom_of_stack;
  aterm_init(bottom_of_stack);
  gsEnableConstructorFunctions();

  test_rename(); 
  return 0;
}
