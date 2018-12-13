// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file sort_expression_test.cpp
/// \brief Basic regression test for sort expressions.

#include <boost/test/minimal.hpp>
#include <iostream>

#include "mcrl2/data/bool.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/rewriter.h"
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
    std::cerr << "--- test failed --- " << data::pp(input) << " ->* " << data::pp(expected) << std::endl
              << "input    " << data::pp(input) << std::endl
              << "expected " << data::pp(expected) << std::endl
              << "R(input) " << data::pp(output) << std::endl
              << " -- term representations -- " << std::endl
              << "input    " << atermpp::aterm(input) << std::endl
              << "expected " << atermpp::aterm(normalize_sorts(expected,spec)) << std::endl
              << "R(input) " << atermpp::aterm(normalize_sorts(output,spec)) << std::endl;
  }
}

void number_test()
{
  using namespace sort_bool;
  using namespace sort_pos;
  using namespace sort_nat;
  using namespace sort_int;
  using namespace sort_real;

  // BOOST_CHECK(data::detail::as_decimal_string(1) == "1");
  // BOOST_CHECK(data::detail::as_decimal_string(2) == "2");
  // BOOST_CHECK(data::detail::as_decimal_string(3) == "3");
  // BOOST_CHECK(data::detail::as_decimal_string(4) == "4");
  // BOOST_CHECK(data::detail::as_decimal_string(144) == "144");

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

  // data::detail::decimal_number_divide_by_two(numbers);
  // BOOST_CHECK(data::detail::vector_number_to_string(numbers) == "8");

  BOOST_CHECK(sort_pos::positive_constant_as_string(number(sort_pos::pos(), "1")) == "1");
  BOOST_CHECK(sort_pos::positive_constant_as_string(number(sort_pos::pos(), "10")) == "10");
  BOOST_CHECK(sort_nat::natural_constant_as_string(number(sort_nat::nat(), "0")) == "0");
  BOOST_CHECK(sort_nat::natural_constant_as_string(number(sort_nat::nat(), "1")) == "1");
  BOOST_CHECK(sort_nat::natural_constant_as_string(number(sort_nat::nat(), "10")) == "10");
  BOOST_CHECK(sort_int::integer_constant_as_string(number(sort_int::int_(), "-10")) == "-10");
  BOOST_CHECK(sort_int::integer_constant_as_string(number(sort_int::int_(), "10")) == "10");
  BOOST_CHECK(sort_int::integer_constant_as_string(number(sort_int::int_(), "-932847982347982347982378947792")) == "-932847982347982347982378947792");

  data_specification specification = parse_data_specification("sort A = Real;");

  mcrl2::data::rewriter R(specification);

  representation_check(R, number(sort_pos::pos(), "1"), sort_pos::pos(1),specification);
  representation_check(R, number(sort_nat::nat(), "1"), R(normalize_sorts(pos2nat(sort_pos::pos(1)),specification)),specification);
  representation_check(R, number(sort_int::int_(), "-1"), R(cneg(sort_pos::pos(1))),specification);
  representation_check(R, normalize_sorts(number(sort_real::real_(), "1"),specification), R(normalize_sorts(pos2real(sort_pos::c1()),specification)),specification);

  representation_check(R, pos("11"), sort_pos::pos(11),specification);
  representation_check(R, pos(12), sort_pos::pos(12),specification);
  representation_check(R, nat("18"), sort_nat::nat(18),specification);
  representation_check(R, nat(12), R(pos2nat(sort_pos::pos(12))),specification);
  representation_check(R, int_("0"), R(nat2int(c0())),specification);
  representation_check(R, int_("-1"), R(cneg(c1())),specification);
  representation_check(R, int_(-2), int_(-2),specification);
  representation_check(R, real_("0"), R(nat2real(c0())),specification);
  representation_check(R, real_("-1"), R(int2real(cneg(c1()))),specification);
  representation_check(R, real_(-2), R(int2real(cneg(sort_pos::plus(c1(),c1())))),specification);

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

  atermpp::aterm_list al(l.begin(),l.end());

  BOOST_CHECK(l.size() == al.size());
}

void number_string_convert_test()
{
  std::string s="17989";
  for(std::size_t i=0; i<30; ++i,  s = s + "043")
  {
    BOOST_CHECK(sort_pos::positive_constant_as_string(sort_pos::pos(s)) == s);
  }

  s="7387";
  for(std::size_t i=0; i<30; ++i,  s = s + "232")
  {
    BOOST_CHECK(sort_nat::natural_constant_as_string(sort_nat::nat(s)) == s);
  }

  s="-37568";
  for(std::size_t i=0; i<30; ++i,  s = s + "715")
  {
    BOOST_CHECK(sort_int::integer_constant_as_string(sort_int::int_(s)) == s);
  }
}

int test_main(int argc, char** argv)
{
  number_string_convert_test();

  number_test();

  list_construction_test();

  convert_test();

  return EXIT_SUCCESS;
}

