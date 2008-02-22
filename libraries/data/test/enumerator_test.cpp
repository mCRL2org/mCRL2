// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file enumerator_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <string>
#include <set>
#include <boost/test/minimal.hpp>
#include "mcrl2/atermpp/atermpp.h"
#include "mcrl2/atermpp/make_list.h"
#include "mcrl2/data/data_operation.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/parser.h"
#include "mcrl2/data/sort_arrow.h"
#include "mcrl2/data/sort_utility.h"
#include "mcrl2/data/detail/data_functional.h"

using namespace atermpp;
using namespace mcrl2::data;
using namespace mcrl2::data::detail;
using namespace mcrl2::lps;

const std::string DATA_SPEC1 =
  "sort A;                     \n"
  "sort B;                     \n"
  "sort C;                     \n"
  "sort D;                     \n"
  "sort S1 = A # B -> C;       \n"
  "sort S2 = (A -> B) -> C;    \n"
  "sort S3 = A -> (B -> C);    \n"
  "sort S4 = A # (A -> B) -> C;\n"
  "sort S5 = A # A -> B -> C;  \n"
  "                            \n"
  "cons s1: S1;                \n"
  "cons s2: S2;                \n"
  "cons s3: S3;                \n"
  "cons s4: S4;                \n"
  "cons s5: S5;                \n" 
;  

enumerator make_enumerator(rewriter& datar, const data_specification& data_spec)
{
  enumerator result(datar, data_spec); 
  return result;
}

void test_enumerator()
{
  using namespace data_expr;

  data_specification data = parse_data_specification(DATA_SPEC1);
  rewriter datar(data);
  enumerator e(datar, data);
  sort_expression s1 = sort_expr::bool_();
  atermpp::vector<data_expression> v = e.enumerate_finite_sort(s1);

  data_variable d1(core::identifier_string("d1"), sort_expr::bool_());
  data_variable d2(core::identifier_string("d2"), sort_expr::bool_());
  data_variable d3(core::identifier_string("d3"), sort_expr::bool_());
  std::vector<data_variable> vars;
  vars.push_back(d1);
  vars.push_back(d2);
  data_expression t = and_(data_expr::equal_to(d1, d2), data_expr::not_equal_to(d1,d3));
  atermpp::set<data_expression> w = e.enumerate_expression_values(t, vars.begin(), vars.end());
  std::cout << "<variables>" << pp(data_expression_list(w.begin(), w.end())) << std::endl;

  // copy an enumerator
  enumerator e1 = e;
  w = e1.enumerate_expression_values(t, vars.begin(), vars.end());

  // enumerator as return value
  enumerator e2 = make_enumerator(datar, data);
  w = e2.enumerate_expression_values(t, vars.begin(), vars.end());
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)
  test_enumerator();

  return 0;
}
