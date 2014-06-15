// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file sort_expression_test.cpp
/// \brief Basic regression test for sort expressions.

#include <iostream>
#include <boost/test/minimal.hpp>

#include "mcrl2/data/parse.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/bool.h"
#include "mcrl2/data/standard_utility.h"

using namespace mcrl2;
using namespace mcrl2::data;

template < typename Rewriter >
void representation_check(Rewriter& R, data_expression const& input, data_expression const& expected, const data_specification& spec)
{
  data_expression output(R(normalize_sorts(input,spec)));

  BOOST_CHECK(normalize_sorts(expected,spec) == output);

  if (output != normalize_sorts(expected,spec))
  {
    std::clog << "--- test failed --- " << data::pp(input) << " ->* " << data::pp(expected) << std::endl
              << "input    " << data::pp(input) << std::endl
              << "expected " << data::pp(expected) << std::endl
              << "R(input) " << data::pp(output) << std::endl
              << " -- term representations -- " << std::endl
              << "input    " << input << std::endl
              << "expected " << normalize_sorts(expected,spec)<< std::endl
              << "R(input) " << normalize_sorts(output,spec) << std::endl;
  }
}

void number_test()
{
  using namespace sort_bool;
  using namespace sort_pos;
  using namespace sort_nat;
  using namespace sort_int;
  using namespace sort_real;

  BOOST_CHECK(data::detail::as_decimal_string(1) == "1");
  BOOST_CHECK(data::detail::as_decimal_string(2) == "2");
  BOOST_CHECK(data::detail::as_decimal_string(3) == "3");
  BOOST_CHECK(data::detail::as_decimal_string(4) == "4");
  BOOST_CHECK(data::detail::as_decimal_string(144) == "144");

  // Test character array arithmetic
  std::vector< char > numbers;
  numbers = data::detail::string_to_vector_number("1");
  BOOST_CHECK(data::detail::vector_number_to_string(numbers) == "1");

  data::detail::decimal_number_multiply_by_two(numbers);
  BOOST_CHECK(data::detail::vector_number_to_string(numbers) == "2");

  data::detail::decimal_number_increment(numbers);
  BOOST_CHECK(data::detail::vector_number_to_string(numbers) == "3");

  data::detail::decimal_number_increment(numbers);
  BOOST_CHECK(data::detail::vector_number_to_string(numbers) == "4");

  data::detail::decimal_number_multiply_by_two(numbers);
  BOOST_CHECK(data::detail::vector_number_to_string(numbers) == "8");

  data::detail::decimal_number_multiply_by_two(numbers);
  BOOST_CHECK(data::detail::vector_number_to_string(numbers) == "16");

  data::detail::decimal_number_divide_by_two(numbers);
  BOOST_CHECK(data::detail::vector_number_to_string(numbers) == "8");

  BOOST_CHECK(sort_pos::positive_constant_as_string(number(sort_pos::pos(), "1")) == "1");
  BOOST_CHECK(sort_pos::positive_constant_as_string(number(sort_pos::pos(), "10")) == "10");
  BOOST_CHECK(sort_nat::natural_constant_as_string(number(sort_nat::nat(), "0")) == "0");
  BOOST_CHECK(sort_nat::natural_constant_as_string(number(sort_nat::nat(), "1")) == "1");
  BOOST_CHECK(sort_nat::natural_constant_as_string(number(sort_nat::nat(), "10")) == "10");
  BOOST_CHECK(sort_int::integer_constant_as_string(number(sort_int::int_(), "-10")) == "-10");
  BOOST_CHECK(sort_int::integer_constant_as_string(number(sort_int::int_(), "10")) == "10");

  data_specification specification = parse_data_specification("sort A = Real;");

  mcrl2::data::rewriter R(specification);

  representation_check(R, number(sort_pos::pos(), "1"), sort_pos::c1(),specification);
  representation_check(R, number(sort_nat::nat(), "1"), R(normalize_sorts(pos2nat(sort_pos::c1()),specification)),specification);
  representation_check(R, number(sort_int::int_(), "-1"), R(cneg(sort_pos::c1())),specification);
  representation_check(R, normalize_sorts(number(sort_real::real_(), "1"),specification), R(normalize_sorts(pos2real(sort_pos::c1()),specification)),specification);

  representation_check(R, pos("11"), cdub(true_(), cdub(true_(), cdub(false_(), c1()))),specification);
  representation_check(R, pos(12), cdub(false_(), cdub(false_(), cdub(true_(), c1()))),specification);
  representation_check(R, nat("18"), R(normalize_sorts(pos2nat(cdub(false_(), cdub(true_(), cdub(false_(), cdub(false_(), c1()))))),specification)),specification);
  representation_check(R, nat(12), R(normalize_sorts(pos2nat(cdub(false_(), cdub(false_(), cdub(true_(), c1())))),specification)),specification);
  representation_check(R, int_("0"), R(nat2int(c0())),specification);
  representation_check(R, int_("-1"), cneg(c1()),specification);
  representation_check(R, int_(-2), cneg(cdub(false_(), c1())),specification);
  representation_check(R, real_("0"), R(normalize_sorts(nat2real(c0()),specification)),specification);
  representation_check(R, real_("-1"), R(normalize_sorts(int2real(cneg(c1())),specification)),specification);
  representation_check(R, real_(-2), R(normalize_sorts(int2real(cneg(cdub(false_(), c1()))),specification)),specification);

}

void list_construction_test()
{
  using namespace mcrl2::data::sort_list;
  using namespace mcrl2::data::sort_bool;

  data_expression_vector expressions;

  expressions.push_back(true_());
  expressions.push_back(false_());
  expressions.push_back(true_());
  expressions.push_back(false_());

  data_specification specification;

  mcrl2::data::rewriter R(specification, jitty);

  representation_check(R, sort_list::list(bool_(), expressions),
                       R(cons_(bool_(), expressions[0], cons_(bool_(), expressions[1],
                               cons_(bool_(), expressions[2], cons_(bool_(), expressions[3], empty(bool_())))))),specification);
}

void convert_test()
{

  std::vector< data_expression > l;

  l.push_back(sort_bool::true_());

  atermpp::aterm_list al = atermpp::convert< atermpp::aterm_list >(l);

  BOOST_CHECK(l.size() == al.size());

  // Could loop indefinitely if the wrong overload is chosen through type-unsafe conversion
  // The transformation below does not work, due to lacking conversions. 
  // std::vector< data_expression > r = atermpp::convert< std::vector< data_expression > >(static_cast< aterm_list >(al));
}

int test_main(int argc, char** argv)
{
  number_test();

  list_construction_test();

  convert_test();

  return EXIT_SUCCESS;
}

