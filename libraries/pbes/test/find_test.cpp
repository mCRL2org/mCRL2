// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbes_solve_test.cpp
/// \brief Add your file description here.

#include <boost/test/minimal.hpp>
#include "mcrl2/pbes/find.h"
//#include "mcrl2/pbes/find1.h"
#include "mcrl2/pbes/txt2pbes.h"
#include "mcrl2/core/garbage_collection.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;

template <typename VariableSet>
void print_variable_set(const VariableSet& s, std::string msg = "variables: ")
{
  std::cout << msg;
  for (typename VariableSet::const_iterator i = s.begin(); i != s.end(); ++i)
  {
    std::cout << " " << core::pp(*i);
  }
  std::cout << std::endl;
}

void test_find_free_variables()
{
  std::string test1 =
    "pbes                                                                   \n"
    "                                                                       \n"
    "nu X(b:Bool, n:Nat) = (val(b) => X(!b, n)) && (val(!b) => X(!b, n+1)); \n"
    "mu Y(c:Nat, d:Bool) = forall m:Nat. Y(c, true) || X(d, m);             \n"
    "                                                                       \n"
    "init X(true, 0);                                                       \n"
    ;

  pbes<> p = txt2pbes(test1);

  std::set<data::variable> v = find_free_variables(p);
  print_variable_set(v, "v.size() == 0:");
  BOOST_CHECK(v.size() == 0);
  
  v = find_free_variables(p.equations()[0]);
  print_variable_set(v, "v.size() == 0: ");
  BOOST_CHECK(v.size() == 0); 

  v = find_free_variables(p.equations()[1]);
  print_variable_set(v, "v.size() == 0: ");
  BOOST_CHECK(v.size() == 0); 

  v = find_free_variables(p.equations()[0].formula());
  print_variable_set(v, "v.size() == 2: ");
  BOOST_CHECK(v.size() == 2); 

  v = find_free_variables(p.equations()[1].formula());
  print_variable_set(v, "v.size() == 2: ");
  BOOST_CHECK(v.size() == 2);
  
  core::garbage_collect();
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT_DEBUG(argc, argv)

  test_find_free_variables();

  return 0;
}
