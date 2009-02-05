// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file data_expression.cpp
/// \brief Add your file description here.

#include <cstdlib>
#include <string>
#include <boost/test/minimal.hpp>
#include "mcrl2/data/sort_identifier.h"
#include "mcrl2/data/data.h"
#include "mcrl2/atermpp/make_list.h"

using namespace std;
using namespace atermpp;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace mcrl2::data;

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)

  ATermAppl T = gsMakeDataExprTrue();
  ATermAppl F = gsMakeDataExprFalse();
  data_expression d(T);

  BOOST_CHECK(data_expr::is_true(d));
  BOOST_CHECK(!data_expr::is_false(d));

  data_variable v("d:D");
  data_expression e(v);
  BOOST_CHECK(v.sort() == e.sort());

  data_variable x("x:X");
  data_variable y("y:Y");
  data_variable_list l = make_list(x, y);
  data_expression_list m = make_data_expression_list(l);

  data_operation f(identifier_string("f"), sort_identifier("T"));
  data_expression ef(f);
  BOOST_CHECK(f.sort() == ef.sort());

  return 0;
}
