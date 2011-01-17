// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file builder_test.cpp
/// \brief Builder tests.

#include <set>
#include <iostream>
#include <boost/test/minimal.hpp>

#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/core/garbage_collection.h"
#include "mcrl2/data/builder.h"
#include "mcrl2/data/add_binding.h"
#include "mcrl2/data/parse.h"

using namespace mcrl2;
using namespace mcrl2::data;

template <typename Derived>
struct my_builder: public add_data_variable_binding<data::data_expression_builder, Derived>
{
  typedef add_data_variable_binding<data::data_expression_builder, Derived> super; 
  using super::enter;
  using super::leave;
  using super::operator();
  using super::is_bound;

  std::multiset<variable> unbound;

  data_expression operator()(const variable& v)
  {
    if (!is_bound(v))
    {
      unbound.insert(v);
    }
    return super::operator()(v);
  }
};

void test_binding()
{
  variable c("c", sort_bool::bool_());
  variable_vector v;
  v.push_back(c);
  data_expression x = parse_data_expression("exists b: Bool. if(c, c, b)", v.begin(), v.end());
  core::apply_builder<my_builder> f;
  f(x);
  BOOST_CHECK(f.unbound.size() == 2);
  size_t count = f.unbound.erase(c);
  BOOST_CHECK(count == 2);
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv);

  test_binding();

  return EXIT_SUCCESS;
}
