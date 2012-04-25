// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file parser_test.cpp
/// \brief Regression test for parsing a data specification.

#include <iostream>
#include <boost/range/iterator_range.hpp>
#include <boost/test/minimal.hpp>

#include "mcrl2/data/pos.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/standard.h"
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/data/basic_sort.h"

using namespace mcrl2;

void parser_test()
{
  const std::string text(
    "sort S;\n"
    "cons s:S;\n"
    "map f:S -> List(S);\n"
  );

  data::data_specification spec(data::parse_data_specification(text));

  std::cerr << "number of sorts " << spec.sorts().size() << "\n";
  BOOST_CHECK(spec.sorts().size() == 6); // Bool, S, List(S), S->List(S), Nat, @NatPair.
  BOOST_CHECK(boost::copy_range< data::function_symbol_vector >(spec.constructors(data::basic_sort("S"))).size() == 1);
  std::cerr << "number of functions " << boost::copy_range< data::function_symbol_vector >(spec.mappings()).size() << "\n";
  BOOST_CHECK(boost::copy_range< data::function_symbol_vector >(spec.mappings()).size() == 92);

  BOOST_CHECK(data::parse_data_expression("2") == data::sort_pos::pos(2));
  BOOST_CHECK(data::parse_data_expression("0") == data::sort_nat::nat(0));
  BOOST_CHECK(data::parse_sort_expression("Nat") == data::sort_nat::nat());
//  BOOST_CHECK(data::parse_data_expression("-1") == data::sort_int::int_(-1));
//  BOOST_CHECK(data::parse_data_expression("1/2") == data::sort_real::real_(1, 2));
}

// This test triggers a sort normalization problem.
void test_user_defined_sort()
{
  using namespace data;

  std::string text = "sort D = struct d1 | d2;\n";
  data_specification data_spec = parse_data_specification(text);
  sort_expression s = parse_sort_expression("D", data_spec);
}

void test_whr()
{
  using namespace data;
  data_expression x = parse_data_expression("exists n: Nat . n == 0 whr n = 1 end");
}

void test_sort()
{
  using namespace data;
  std::string text = "Pos -> Pos # Pos -> Bool";
  sort_expression s = parse_sort_expression(text);
  BOOST_CHECK(data::pp(s) == text);
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv);

  parser_test();
  test_user_defined_sort();
  test_whr();
  test_sort();

  return EXIT_SUCCESS;
}
