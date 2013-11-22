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

#include "mcrl2/data/index_traits.h"
#include "mcrl2/data/bool.h"
#include "mcrl2/data/io.h"

using namespace mcrl2;

void f()
{
  data::variable x("x", data::sort_bool::bool_());
}

data::variable_key_type key(const data::variable& v)
{
  return data::variable_key_type(v.name(), v.sort());
}

data::function_symbol_key_type key(const data::function_symbol& f)
{
  return data::function_symbol_key_type(f.name(), f.sort());
}

BOOST_AUTO_TEST_CASE(test_index)
{
/*
  data::register_function_symbol_hooks();
  data::register_variable_hooks();

  std::size_t value;

  // data::variable
  data::variable b("b", data::sort_bool::bool_());
  value = core::index_traits<data::variable, data::variable_key_type>::index(b);
  BOOST_CHECK(value == 0);
  value = core::index_traits<data::variable, data::variable_key_type>::max_index();
  BOOST_CHECK(value == 0);

  data::variable c("c", data::sort_bool::bool_());
  value = core::index_traits<data::variable, data::variable_key_type>::index(c);
  BOOST_CHECK(value == 1);
  value = core::index_traits<data::variable, data::variable_key_type>::max_index();
  BOOST_CHECK(value == 1);

  data::variable d = b;
  value = core::index_traits<data::variable, data::variable_key_type>::index(d);
  BOOST_CHECK(value == 0);
  value = core::index_traits<data::variable, data::variable_key_type>::max_index();
  BOOST_CHECK(value == 1);

  // data::function_symbol
  data::function_symbol f("f", data::sort_bool::bool_());
  value = core::index_traits<data::function_symbol, data::function_symbol_key_type>::index(f);
  BOOST_CHECK(value == 0);
  value = core::index_traits<data::function_symbol, data::function_symbol_key_type>::max_index();
  BOOST_CHECK(value == 0);

  data::function_symbol g("g", data::sort_bool::bool_());
  value = core::index_traits<data::function_symbol, data::function_symbol_key_type>::index(g);
  BOOST_CHECK(value == 1);
  value = core::index_traits<data::function_symbol, data::function_symbol_key_type>::max_index();
  BOOST_CHECK(value == 1);

  data::function_symbol h = f;
  value = core::index_traits<data::function_symbol, data::function_symbol_key_type>::index(h);
  BOOST_CHECK(value == 0);
  value = core::index_traits<data::function_symbol, data::function_symbol_key_type>::max_index();
  BOOST_CHECK(value == 1);
*/
}

BOOST_AUTO_TEST_CASE(test_io)
{
/*
  data::variable b("b", data::sort_bool::bool_());
  data::variable c("c", data::sort_bool::bool_());
  std::size_t b_index = core::index_traits<data::variable, data::variable_key_type>::index(b);
  std::size_t c_index = core::index_traits<data::variable, data::variable_key_type>::index(c);
  data::data_expression b_and_c = data::sort_bool::and_(b, c);
  atermpp::aterm t1 = data::add_index(b_and_c);
  std::cout << t1 << std::endl;
  atermpp::aterm t2 = data::remove_index(t1);
  BOOST_CHECK(b_and_c == t2);
*/
}
