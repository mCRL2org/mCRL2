// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file find_test.cpp
/// \brief Test for find functions.

#include <algorithm>
#include <iterator>
#include <set>
#include <vector>
#include <boost/test/minimal.hpp>
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/lps/find.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/core/garbage_collection.h"

using namespace mcrl2;
using namespace mcrl2::lps;

std::string SPEC = 
  "glob                      \n"
  "  m: Nat;                 \n"
  "                          \n"
  "act                       \n"
  "  a: Nat;                 \n"
  "                          \n"
  "proc                      \n"
  "  P(n:Nat) = a(m).P(n+1); \n"
  "                          \n"
  "init P(0);                \n"
  ;
  
inline
data::variable nat(std::string name)
{
  return data::variable(core::identifier_string(name), data::sort_nat::nat());
}

inline
data::variable pos(std::string name)
{
  return data::variable(core::identifier_string(name), data::sort_pos::pos());
}

inline
data::variable bool_(std::string name)
{
  return data::variable(core::identifier_string(name), data::sort_bool::bool_());
}

void test_find()
{
  specification spec = parse_linear_process_specification(SPEC);
  std::cout << spec.process().action_summands().size() << std::endl;
  action_summand s = spec.process().action_summands().front();
  action a = s.multi_action().actions().front();

  //--- find_variables ---//
  data::variable m = nat("m"); 
  std::set<data::variable> v = lps::find_variables(a);
  v = lps::find_variables(s);
  BOOST_CHECK(v.find(m) != v.end());   

  //--- find_sort_expressions ---//
  std::set<data::sort_expression> e = lps::find_sort_expressions(a);
  std::cout << "e.size() = " << e.size() << std::endl;
  BOOST_CHECK(std::find(e.begin(), e.end(), data::sort_nat::nat()) != e.end());
  BOOST_CHECK(std::find(e.begin(), e.end(), data::sort_pos::pos()) == e.end());
}

void test_free_variables()
{
  lps::specification specification(parse_linear_process_specification(
    "act a : Bool;\n"
    "proc X = a((forall x : Nat. exists y : Nat. x < y)).X;\n"
    "init X;\n"
  ));

  std::set<data::variable> free_variables = find_free_variables(specification.process());
  BOOST_CHECK(free_variables.find(data::variable("x", data::sort_nat::nat())) == free_variables.end());
  BOOST_CHECK(free_variables.find(data::variable("y", data::sort_nat::nat())) == free_variables.end());

  specification = parse_linear_process_specification(
    "act a;\n"
    "proc X(z : Bool) = (z && forall x : Nat. exists y : Nat. x < y) -> a.X(!z);\n"
    "init X(true);\n"
  );
  free_variables = find_free_variables(specification.process());
  BOOST_CHECK(free_variables.find(data::variable("x", data::sort_nat::nat())) == free_variables.end());
  BOOST_CHECK(free_variables.find(data::variable("y", data::sort_nat::nat())) == free_variables.end());

  BOOST_CHECK(is_well_typed(specification));
  core::garbage_collect();
}

void test_search()
{
  lps::specification spec(parse_linear_process_specification(
    "act a : Bool;\n"
    "proc X = a((forall x : Nat. exists y : Nat. x < y)).X;\n"
    "init X;\n"
  ));
  data::variable v("x", data::sort_nat::nat());
  lps::search_free_variable(spec.process().summands(), v);
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv);

  test_find();
  test_free_variables();
  test_search();

  return EXIT_SUCCESS;
}
