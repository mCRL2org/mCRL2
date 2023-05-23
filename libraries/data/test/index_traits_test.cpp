// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file index_traits_test.cpp
/// \brief Test for index_traits.

#define BOOST_TEST_MODULE index_traits

#include <boost/test/included/unit_test.hpp>

#include "mcrl2/data/bool.h"
#include "mcrl2/data/detail/io.h"

using namespace mcrl2;

void f()
{
  data::variable x("x", data::sort_bool::bool_());
}

BOOST_AUTO_TEST_CASE(test_io)
{
/*
  data::variable b("b", data::sort_bool::bool_());
  data::variable c("c", data::sort_bool::bool_());
  std::size_t b_index = atermpp::detail::index_traits<data::variable, data::variable_key_type, 2>::index(b);
  std::size_t c_index = atermpp::detail::index_traits<data::variable, data::variable_key_type, 2>::index(c);
  data::data_expression b_and_c = data::sort_bool::and_(b, c);
  atermpp::aterm t1 = data::add_index(b_and_c);
  std::cout << t1 << std::endl;
  atermpp::aterm t2 = data::remove_index(t1);
  BOOST_CHECK(b_and_c == t2);
*/
}
