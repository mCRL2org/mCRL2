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
