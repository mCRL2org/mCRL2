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
#include "mcrl2/data/nat.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/function_sort.h"
#include "mcrl2/data/detail/data_functional.h"
#include "mcrl2/data/detail/parse_substitution.h"
#include "mcrl2/data/detail/test_rewriters.h"
#include "mcrl2/data/detail/one_point_rule_preprocessor.h"
#include "mcrl2/data/rewriters/simplify_rewriter.h"
#include "mcrl2/data/print.h"
#include "mcrl2/utilities/text_utility.h"

using namespace mcrl2;
using namespace mcrl2::core;
using namespace mcrl2::data;
using namespace mcrl2::data::detail;

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
  using namespace mcrl2::data::sort_nat;

  std::string DATA_SPEC1 =
    "sort D = struct d1(Nat)?is_d1 | d2(arg2:Nat)?is_d2;\n"
    ;
  data_specification data = parse_data_specification(DATA_SPEC1);
  rewriter datar(data);
  variable x("x", sort_nat::nat());
  variable y("y", sort_nat::nat());
  variable z("z", sort_nat::nat());
  data_expression t = datar(greater(minimum(x,y), z));

  BOOST_CHECK(datar(plus(parse_data_expression("1"),
                         parse_data_expression("2"))) == datar(parse_data_expression("3")));

  // copy a rewriter
  data::rewriter datar1 = datar;
  t = datar1(greater(minimum(x,y), z));

  // rewriter as return value
  data::rewriter datar2 = make_data_rewriter(data);
  t = datar2(greater(minimum(x,y), z));

  A a(datar);
  data_expression qa = a.r_(t);

  A b = a;
  data_expression qb = b.r_(t);

  A c = make_A(datar);
  data_expression qc = c.r_(t);
}

void test2()
{
  using namespace mcrl2::data::sort_nat;

  data_specification data_spec;

  data_spec.add_context_sort(nat());

  rewriter r(data_spec);
  data_expression d1 = parse_data_expression("2+7");
  data_expression d2 = parse_data_expression("4+5");
  BOOST_CHECK(r(d1) == r(d2));

  std::string var_decl = "m, n: Pos;\n";
  data::rewriter::substitution_type sigma;
  sigma[atermpp::down_cast<variable>(parse_data_expression("m", var_decl))] = r(parse_data_expression("3"));
  sigma[atermpp::down_cast<variable>(parse_data_expression("n", var_decl))] = r(parse_data_expression("4"));

  // Rewrite two data expressions, and check if they are the same
  d1 = parse_data_expression("m+n", var_decl);
  d2 = parse_data_expression("7");
  BOOST_CHECK(r(d1, sigma) == r(d2));
}

void test3()
{
  typedef data::rewriter::substitution_type substitution_function;

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
  std::set<variable> v = find_all_variables(x);
  BOOST_CHECK(v.size() == 1);

  data_expression_with_variables y(x, variable_list(v.begin(), v.end()));
  data_expression_with_variables z = r(y);
  std::cout << "y = " << data::pp(y) << " " << data::pp(y.variables()) << std::endl;
  std::cout << "z = " << data::pp(z) << " " << data::pp(z.variables()) << std::endl;
  BOOST_CHECK(z.variables().empty());

  std::string var_decl = "m, n: Pos;\n";
  substitution_function sigma;
  variable m(variable("m", sort_pos::pos()));
  variable n(variable("n", sort_pos::pos()));
  sigma[m] = r(data_expression_with_variables(parse_data_expression("3")));
  sigma[n] = r(data_expression_with_variables(parse_data_expression("4")));

  data_expression_with_variables sigma_m = data::replace_variables(static_cast<const data_expression&>(m), sigma);

  data_expression_with_variables d1(parse_data_expression("m+n", var_decl));
  data_expression_with_variables d2(parse_data_expression("7"));
  BOOST_CHECK(r(d1, sigma) == r(d2));

  BOOST_CHECK(d1.variables().size() == 0);
  data_expression_with_variables rd1 = r(d1);
  BOOST_CHECK(rd1.variables().size() == 2);
}

template <typename Rewriter>
void test_expressions(Rewriter R, std::string const& expr1, std::string const& expr2, std::string const& declarations, const data_specification& data_spec, std::string substitution_text)
{
  data::rewriter::substitution_type sigma;
  data::detail::parse_substitution(substitution_text, sigma, data_spec);
  data_expression d1 = parse_data_expression(expr1, declarations, data_spec);
  data_expression d2 = parse_data_expression(expr2, declarations, data_spec);
  if (R(d1, sigma) != R(d2))
  {
    BOOST_CHECK(R(d1, sigma) == R(d2));
    std::cout << "--- failed test --- " << expr1 << " -> " << expr2 << std::endl;
    std::cout << "d1           " << d1 << std::endl;
    std::cout << "d2           " << d2 << std::endl;
    std::cout << "sigma\n      " << sigma << std::endl;
    std::cout << "R(d1, sigma) " << R(d1, sigma) << std::endl;
    std::cout << "R(d2)        " << R(d2) << std::endl;
  }
}

void test4()
{
  data_specification data_spec;

  data::rewriter R(data_spec);

  std::string expr1 = "exists b: Bool. if(c, c, b)";
//  std::string expr2 = "true"; // rewriter cannot deal with abstraction yet
  std::string expr2 = "exists b: Bool. if(true, true, b)";
  std::string sigma = "[c: Bool := true]";
  test_expressions(R, expr1, expr2, "c: Bool;", data_spec, sigma);
}

void allocation_test()
{
  data_specification data_spec;
  std::auto_ptr< data::rewriter > R_heap(new data::rewriter(data_spec));
  data::rewriter                  R_stack(data_spec);

  R_stack(parse_data_expression("1 == 2"));
  R_stack(parse_data_expression("1 == 2"));

  (*R_heap)(parse_data_expression("1 == 2"));
  (*R_heap)(parse_data_expression("1 == 2"));
}

void one_point_rule_preprocessor_test()
{
  using namespace data::detail;

  test_rewriters(N(one_point_rule_preprocessor()), N(I), "!(true && false)", "!(true) || !(false)");
  test_rewriters(N(one_point_rule_preprocessor()), N(I), "!!b", "b");
  test_rewriters(N(one_point_rule_preprocessor()), N(I), "!!!!b", "b");
  test_rewriters(N(one_point_rule_preprocessor()), N(I), "!(b1 && b2 && b3)", "!b1 || !b2 || !b3");
  test_rewriters(N(one_point_rule_preprocessor()), N(I), "!(b1 || b2 || b3)", "!b1 && !b2 && !b3");
  test_rewriters(N(one_point_rule_preprocessor()), N(I), "!(b1 == b2)", "b1 != b2");
  test_rewriters(N(one_point_rule_preprocessor()), N(I), "!(b1 != b2)", "b1 == b2");
  test_rewriters(N(one_point_rule_preprocessor()), N(I), "!(b1 == b2 && b1 == b3)", "b1 != b2 || b1 != b3");
  test_rewriters(N(one_point_rule_preprocessor()), N(I), "!(b1 != b2 || b1 != b3)", "b1 == b2 && b1 == b3");
  test_rewriters(N(one_point_rule_preprocessor()), N(I), "!(n1 != n2 || n1 != n2 + 1)", "n1 == n2 && n1 == n2 + 1");
}

void simplify_rewriter_test()
{
  data::simplify_rewriter R;
  test_rewriters(N(R), N(I), "!(true && false)", "true");
  test_rewriters(N(R), N(I), "exists n:Nat, b:Bool. b", "exists b:Bool. b");
  test_rewriters(N(R), N(I), "forall b:Bool. !!b", "forall b:Bool. b");
}

int test_main(int argc, char** argv)
{
  test1();
  test2();
  test3();
  test4();
  one_point_rule_preprocessor_test();
  simplify_rewriter_test();

  return 0;
}
