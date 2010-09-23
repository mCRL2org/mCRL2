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
#include "mcrl2/bes/bes_parse.h"
#include "mcrl2/core/garbage_collection.h"

#include "mcrl2/bes/traverser.h"
#include "mcrl2/core/detail/find.h"

using namespace mcrl2;
using namespace mcrl2::bes;

template <typename Container, typename OutputIterator>
void my_find_variables(Container const& container, OutputIterator o)
{
  core::detail::make_find_helper<boolean_variable, traverser, OutputIterator>(o)(container);
}

template <typename Container>
std::set<boolean_variable> my_find_variables(Container const& container)
{
  std::set<boolean_variable> result;
  my_find_variables(container, std::inserter(result, result.end()));
  return result;
}

void test_my_find()
{
  std::string bes1 =
    "pbes              \n"
    "                  \n"
    "nu X1 = X2 && X1; \n"
    "mu X2 = X1 || X2; \n"
    "                  \n"
    "init X1;          \n"
    ;
  boolean_equation_system<> b;
  std::stringstream from(bes1);
  from >> b;

  std::set<boolean_variable> v;

  //--- find_variables ---//
  v = my_find_variables(b);
  BOOST_CHECK(v.size() == 2);
  BOOST_CHECK(v.find(boolean_variable("X1")) != v.end());   
  BOOST_CHECK(v.find(boolean_variable("X2")) != v.end());   

  boolean_equation eq = b.equations().front();
  std::clog << "<eq>" << pp(eq) << std::endl;
  v = my_find_variables(eq);
  BOOST_CHECK(v.size() == 2);
  BOOST_CHECK(v.find(boolean_variable("X1")) != v.end());   
  BOOST_CHECK(v.find(boolean_variable("X2")) != v.end());   

  boolean_expression x = eq.formula();
  std::clog << "<x>" << pp(x) << std::endl;
  v = my_find_variables(x);
  BOOST_CHECK(v.size() == 2);
  BOOST_CHECK(v.find(boolean_variable("X1")) != v.end());   
  BOOST_CHECK(v.find(boolean_variable("X2")) != v.end());   

  core::garbage_collect();
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv);

  test_my_find();

  return EXIT_SUCCESS;
}
