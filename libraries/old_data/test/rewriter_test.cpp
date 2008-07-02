// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
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
#include "mcrl2/old_data/data_operation.h"
#include "mcrl2/old_data/parser.h"
#include "mcrl2/old_data/rewriter.h"
#include "mcrl2/old_data/sort_arrow.h"
#include "mcrl2/old_data/detail/data_functional.h"

using namespace atermpp;
using namespace mcrl2;
using namespace mcrl2::old_data;

data_variable nat(std::string name)
{
  return data_variable(core::identifier_string(name) , sort_expr::nat());
}

old_data::rewriter make_data_rewriter(const data_specification& data_spec)
{
  old_data::rewriter datar(data_spec); 
  return datar;
}

struct A
{
  old_data::rewriter& r_;
  
  A(old_data::rewriter& r)
    : r_(r)
  { }
};

A make_A(old_data::rewriter& d)
{
  A result(d);
  return result;
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
  
  // copy a rewriter
  old_data::rewriter datar1 = datar;
  t = datar1(greater(min_(x,y), z));

  // rewriter as return value
  old_data::rewriter datar2 = make_data_rewriter(data);
  t = datar2(greater(min_(x,y), z));
  
  A a(datar);
  data_expression qa = a.r_(t);

  A b = a;
  data_expression qb = b.r_(t);
  
  A c = make_A(datar);
  data_expression qc = c.r_(t);
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv) 
  test_rewriter();

  return 0;
}
