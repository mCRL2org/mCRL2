// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file rewriter_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <string>
#include <set>
#include <boost/test/minimal.hpp>
#include "mcrl2/atermpp/atermpp.h"
#include "mcrl2/atermpp/make_list.h"
#include "mcrl2/data/data_operation.h"
#include "mcrl2/data/parser.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/sort_arrow.h"
#include "mcrl2/data/detail/data_functional.h"

using namespace atermpp;
using namespace mcrl2;
using namespace mcrl2::data;

data_variable nat(std::string name)
{
  return data_variable(core::identifier_string(name) , sort_expr::nat());
}

void test_rewriter()
{
  using namespace data_expr;
  
  std::string DATA_SPEC1 =
  "sort D = struct d1(Nat)?is_d1 | d2(arg2:Nat)?is_d2;\n"
  ;
  data_specification data = parse_data_specification(DATA_SPEC1);
  rewriter datar(data);
  data_variable x = nat("x");
  data_variable y = nat("y");
  data_variable z = nat("z"); 
  data_expression t = datar(greater(min_(x,y), z));
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv) 
  test_rewriter();

  return 0;
}
