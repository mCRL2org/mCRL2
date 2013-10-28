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

#include <iostream>
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/data/bool.h"
#include "mcrl2/data/index_traits.h"

using namespace mcrl2;

BOOST_AUTO_TEST_CASE(test_index)
{
  data::variable b("b", data::sort_bool::bool_());
  data::index_traits<data::variable>::insert(b); // should be done in the constructor of b
  data::index_traits<data::variable>::print("1");
  BOOST_CHECK(data::index_traits<data::variable>::index(b) == 0);
  BOOST_CHECK(data::index_traits<data::variable>::max_index() == 0);

  data::variable c("c", data::sort_bool::bool_());
  std::size_t index = data::index_traits<data::variable>::insert(c); // should be done in the constructor of b
  data::index_traits<data::variable>::print("2");
  BOOST_CHECK(index == 1);
  BOOST_CHECK(data::index_traits<data::variable>::index(c) == 1);
  BOOST_CHECK_EQUAL(data::index_traits<data::variable>::max_index(), 1);

  data::variable d = b;
  data::index_traits<data::variable>::insert(d); // should be done in the constructor of b
  data::index_traits<data::variable>::print("3");
  BOOST_CHECK(data::index_traits<data::variable>::index(d) == 0);
  BOOST_CHECK_EQUAL(data::index_traits<data::variable>::max_index(), 1);
}

