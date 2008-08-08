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

data::rewriter make_data_rewriter(const data_specification& data_spec)
{
  data::rewriter datar(data_spec); 
  return datar;
}

struct A
{
  data::rewriter& r_;
  
  A(data::rewriter& r)
    : r_(r)
  { }
};

A make_A(data::rewriter& d)
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

  std::cout << "NAT3" << core::pp(datar(plus(nat(1), nat(2)))) << std::endl;
  BOOST_CHECK(datar(plus(nat(1), nat(2))) == nat(3));
  
  // copy a rewriter
  data::rewriter datar1 = datar;
  t = datar1(greater(min_(x,y), z));

  // rewriter as return value
  data::rewriter datar2 = make_data_rewriter(data);
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
