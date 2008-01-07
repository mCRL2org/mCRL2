// Author(s): Wieger Wesselink
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
#include "mcrl2/data/data.h"
#include "mcrl2/atermpp/make_list.h"

using namespace std;
using namespace atermpp;
using namespace lps;

int test_main(int argc, char** argv)
{
  MCRL2_CORE_LIBRARY_INIT(argv) 

  ATermAppl T = gsMakeDataExprTrue();
  ATermAppl F = gsMakeDataExprFalse();
  data_expression d(T);

  BOOST_CHECK(data_expr::is_true(d));
  BOOST_CHECK(!data_expr::is_false(d));
  
  data_variable v("d:D");
  data_expression e(v);

  data_variable x("x:X");
  data_variable y("y:Y");
  data_variable_list l = make_list(x, y);
  data_expression_list m = make_data_expression_list(l);

  return 0;
}
