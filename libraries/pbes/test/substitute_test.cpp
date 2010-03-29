// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file substitute_test.cpp
/// \brief Test for the pbes rewriters.

#include <iostream>
#include <boost/test/minimal.hpp>
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/core/garbage_collection.h"
#include "mcrl2/pbes/parse.h"
#include "mcrl2/pbes/substitution.h"
#include "mcrl2/pbes/substitute.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;

inline
pbes_expression parse(const std::string& expr)
{
  std::string var_decl = 
    "datavar    \n"
    "  d: Nat;  \n"
    "  b: Bool; \n"
    "           \n"
    "predvar    \n"
    "  X: Nat;  \n"
    "  Y: Bool; \n"
    "  Z;       \n"
    ;

  std::string data_spec = "";
  return pbes_system::parse_pbes_expression(expr, var_decl, data_spec);
}

inline
std::string print(const pbes_expression& x)
{
	return core::pp(x) + " " + x.to_string();
}

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

inline
propositional_variable make_X()
{ 
  atermpp::vector<data::variable> v;
  v.push_back(nat("d"));
  data::variable_list d = atermpp::convert<data::variable_list>(v);
  return propositional_variable(core::identifier_string("X"), d);
}

inline
propositional_variable make_Y()
{ 
  atermpp::vector<data::variable> v;
  v.push_back(bool_("b"));
  data::variable_list d = atermpp::convert<data::variable_list>(v);
  return propositional_variable(core::identifier_string("Y"), d);
}

inline
propositional_variable make_Z()
{ 
  atermpp::vector<data::variable> v;
  data::variable_list d = atermpp::convert<data::variable_list>(v);
  return propositional_variable(core::identifier_string("Z"), d);
}

void test_substitution()
{
  typedef core::term_traits<pbes_expression> tr;
  
  propositional_variable_substitution sigma;
  
  propositional_variable X = make_X();
  propositional_variable Y = make_Y();
  propositional_variable Z = make_Z();
  
  sigma[X] = parse("val(d>1) && Y(true)");
  pbes_expression t = parse("X(2)");
  propositional_variable_substitute(t, sigma);
  pbes_expression expected_result = parse("val(2 > 1) && Y(true)");
  std::cout << "expected_result = " << print(expected_result) << std::endl;
  std::cout << "t               = " << print(t) << std::endl;

  // TODO We do a string comparison, because we bump into undocumented type check issues here
  BOOST_CHECK(core::pp(t) == core::pp(expected_result));
  
  core::garbage_collect();
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT_DEBUG(argc, argv)

  test_substitution();

  return 0;
}
