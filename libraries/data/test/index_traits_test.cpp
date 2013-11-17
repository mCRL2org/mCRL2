// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file index_traits_test.cpp
/// \brief Test for index_traits.

#define BOOST_TEST_MODULE index_traits
#define MCRL2_USE_INDEX_TRAITS

#include <iostream>
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/data/bool.h"
#include "mcrl2/data/index_traits.h"

using namespace mcrl2;

void f()
{
  data::variable x("x", data::sort_bool::bool_());
}

BOOST_AUTO_TEST_CASE(test_index)
{
  /* Hooks are now registered automatically.
     data::register_function_symbol_hooks();
     data::register_variable_hooks(); */

  // data::variable
  data::variable b("b", data::sort_bool::bool_());
  BOOST_CHECK(data::index_traits<data::variable>::index(b) == 0);
  BOOST_CHECK(data::index_traits<data::variable>::max_index() == 0);

  data::variable c("c", data::sort_bool::bool_());
  BOOST_CHECK(data::index_traits<data::variable>::index(c) == 1);
  BOOST_CHECK_EQUAL(data::index_traits<data::variable>::max_index(), 1);

  data::variable d = b;
  BOOST_CHECK(data::index_traits<data::variable>::index(d) == 0);
  BOOST_CHECK_EQUAL(data::index_traits<data::variable>::max_index(), 1);

  // data::function_symbol
  data::function_symbol f("f", data::sort_bool::bool_());
  BOOST_CHECK(data::index_traits<data::function_symbol>::index(f) == 0);
  BOOST_CHECK(data::index_traits<data::function_symbol>::max_index() == 0);

  data::function_symbol g("g", data::sort_bool::bool_());
  BOOST_CHECK(data::index_traits<data::function_symbol>::index(g) == 1);
  BOOST_CHECK_EQUAL(data::index_traits<data::function_symbol>::max_index(), 1);

  data::function_symbol h = f;
  BOOST_CHECK(data::index_traits<data::function_symbol>::index(h) == 0);
  BOOST_CHECK_EQUAL(data::index_traits<data::function_symbol>::max_index(), 1);
}

