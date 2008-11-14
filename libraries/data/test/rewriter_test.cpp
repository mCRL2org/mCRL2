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
#include "mcrl2/atermpp/map.h"
#include "mcrl2/data/data_operation.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/parser.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/sort_arrow.h"
#include "mcrl2/data/detail/data_functional.h"

using namespace atermpp;
using namespace mcrl2;
using namespace mcrl2::core;
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

void test1()
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

void test2()
{
  rewriter r = default_data_rewriter(); 
  data_expression d1 = parse_data_expression("2+7");
  data_expression d2 = parse_data_expression("4+5");
  assert(r(d1) == r(d2));

  std::string var_decl = "m, n: Pos;\n";
  rewriter_map<atermpp::map<data_variable, data_expression> > sigma; 
  sigma[parse_data_expression("m", var_decl)] = r(parse_data_expression("3"));
  sigma[parse_data_expression("n", var_decl)] = r(parse_data_expression("4"));

  // Rewrite two data expressions, and check if they are the same
  d1 = parse_data_expression("m+n", var_decl);
  d2 = parse_data_expression("7");
  assert(r(d1, sigma) == r(d2));
}

void test3()
{
  typedef atermpp::map<data_variable, data_expression_with_variables> substitution_map;

  data_specification data_spec = parse_data_specification(
    "map dummy1:Pos;  \n"
    "var dummy2:Bool; \n"
    "    dummy3:Pos;  \n"
    "    dummy4:Nat;  \n"
    "    dummy5:Int;  \n"
    "    dummy6:Real; \n"
    "eqn dummy1 = 1;  \n"
  ); 
  rewriter_with_variables r(data_spec);
  data_expression x = parse_data_expression("b == b", "b: Bool;\n");
  std::set<data_variable> v = find_all_data_variables(x);
  BOOST_CHECK(v.size() == 1);
  
  data_expression_with_variables y(x, data_variable_list(v.begin(), v.end()));
  data_expression_with_variables z = r(y);
  std::cout << "y = " << pp(y) << " " << pp(y.variables()) << std::endl;
  BOOST_CHECK(z.variables().empty());
  
  std::string var_decl = "m, n: Pos;\n";
  rewriter_map<substitution_map> sigma; 
  data_variable m = parse_data_expression("m", var_decl);
  data_variable n = parse_data_expression("n", var_decl);
  data_variable q = parse_data_variable("q:Nat");
  sigma[m] = r(data_expression_with_variables(parse_data_expression("3")));
  sigma[n] = r(data_expression_with_variables(parse_data_expression("4")));

  data_expression_with_variables sigma_m = sigma(m);

  data_expression_with_variables d1(parse_data_expression("m+n", var_decl));
  data_expression_with_variables d2(parse_data_expression("7"));
  BOOST_CHECK(r(d1, sigma) == r(d2));
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv) 
  test1();
  test2();
  test3();

  return 0;
}
