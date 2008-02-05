// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file find_test.cpp
/// \brief Test for find functions.

#include <boost/test/minimal.hpp>
#include "mcrl2/atermpp/atermpp.h"
#include "mcrl2/data/find.h"
#include "mcrl2/utilities/aterm_ext.h"

using namespace mcrl2::core;
using namespace mcrl2::data;
using namespace mcrl2::data::data_expr;

int test_main(int argc, char* argv[])
{
  MCRL2_ATERM_INIT(argc, argv) 

  data_variable d1( identifier_string("d1"), sort_expr::nat());
  data_variable d2( identifier_string("d2"), sort_expr::nat());
  data_variable d3( identifier_string("d3"), sort_expr::nat());
  data_variable d4(identifier_string("d4"), sort_expr::nat());

  data_expression e = and_(equal_to(d1, d2), not_equal_to(d2, d3));
  
  BOOST_CHECK(find_data_variable(e, d1));
  BOOST_CHECK(find_data_variable(e, d2));
  BOOST_CHECK(find_data_variable(e, d3));
  BOOST_CHECK(!find_data_variable(e, d4));

  return 0;
}
