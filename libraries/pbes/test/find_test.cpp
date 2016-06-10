// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file find_test.cpp
/// \brief Add your file description here.

#include <boost/test/minimal.hpp>
#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/pbes/find.h"
#include "mcrl2/pbes/parse.h"
#include "mcrl2/pbes/print.h"
#include "mcrl2/pbes/txt2pbes.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;

inline
data::variable nat(const std::string& name)
{
  return data::variable(core::identifier_string(name), data::sort_nat::nat());
}

inline
data::variable pos(const std::string& name)
{
  return data::variable(core::identifier_string(name), data::sort_pos::pos());
}

inline
data::variable bool_(const std::string& name)
{
  return data::variable(core::identifier_string(name), data::sort_bool::bool_());
}

void test_find()
{
  const std::string VARSPEC =
    "datavar         \n"
    "  m: Nat;       \n"
    "  n: Nat;       \n"
    "                \n"
    "predvar         \n"
    "  X: Bool, Pos; \n"
    "  Y: Nat;       \n"
    ;

  pbes_expression x = parse_pbes_expression("X(true, 2) && Y(n+1) && Y(m)", VARSPEC);

  //--- find_all_variables ---//
  data::variable m = nat("m");
  data::variable n = nat("n");
  std::set<data::variable> v = pbes_system::find_all_variables(x);
  BOOST_CHECK(v.find(m) != v.end());
  BOOST_CHECK(v.find(n) != v.end());

  //--- find_sort_expressions ---//
  std::set<data::sort_expression> e = pbes_system::find_sort_expressions(x);
  BOOST_CHECK(std::find(e.begin(), e.end(), data::sort_nat::nat()) != e.end());
  BOOST_CHECK(std::find(e.begin(), e.end(), data::sort_pos::pos()) != e.end());

}

void test_free_variables()
{
  const std::string VARSPEC =
    "datavar         \n"
    "  n: Nat;       \n"
    "                \n"
    "predvar         \n"
    "  X: Bool, Pos; \n"
    "  Y: Nat;       \n"
    ;

  pbes_expression x = parse_pbes_expression("forall m:Nat.(X(true, 2) && Y(n+1) && Y(m))", VARSPEC);
  data::variable m = nat("m");
  data::variable n = nat("n");

  std::set<data::variable> free_variables = pbes_system::find_free_variables(x);
  std::cout << "free variables: " << core::detail::print_set(free_variables) << std::endl;
  BOOST_CHECK(free_variables.find(m) == free_variables.end());
  BOOST_CHECK(free_variables.find(n) != free_variables.end());
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

  pbes p = txt2pbes(test1);

  std::set<data::variable> v = find_free_variables(p);
  std::cout << "variables: " << core::detail::print_set(v) << std::endl;
  BOOST_CHECK(v.size() == 0);

  v = find_free_variables(p.equations()[0]);
  std::cout << "variables: " << core::detail::print_set(v) << std::endl;
  BOOST_CHECK(v.size() == 0);

  v = find_free_variables(p.equations()[1]);
  std::cout << "variables: " << core::detail::print_set(v) << std::endl;
  BOOST_CHECK(v.size() == 0);

  v = find_free_variables(p.equations()[0].formula());
  std::cout << "variables: " << core::detail::print_set(v) << std::endl;
  BOOST_CHECK(v.size() == 2);

  v = find_free_variables(p.equations()[1].formula());
  std::cout << "variables: " << core::detail::print_set(v) << std::endl;
  BOOST_CHECK(v.size() == 2);

}

int test_main(int argc, char** argv)
{
  test_find();
  test_free_variables();
  test_find_free_variables();

  return 0;
}
