// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file rewrite_large_number_test.cpp
/// \brief Tests whether terms representing large positive and integer numbers are evaluated well. 

#include <iostream>

#define BOOST_TEST_MODULE large_numbers
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/utilities/text_utility.h"

#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/detail/rewrite_strategies.h"
#include "mcrl2/data/list.h"
#include "mcrl2/data/real.h"
#include "mcrl2/data/set.h"

#include "mcrl2/data/parse.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/structured_sort.h"

#ifdef MCRL2_ENABLE_MACHINENUMBERS
#include <boost/multiprecision/cpp_int.hpp>
#endif

static const std::size_t number_of_tests =
#ifdef MCRL2_ENABLE_MACHINENUMBERS
  50;
#else
  4;
#endif

using namespace mcrl2;
using namespace mcrl2::core;
using namespace mcrl2::data;
using namespace mcrl2::data::detail;

using rewrite_strategy_vector = std::vector<rewrite_strategy>;

template <typename Rewriter>
void data_rewrite_test(Rewriter& R, const data_expression& input, const data_expression& expected_output)
{
  data_expression output = R(input);
  data_expression exp_output = R(expected_output);

  BOOST_CHECK(output == exp_output);

  if (output != exp_output)
  {
    std::cerr << "--- test failed --- " << data::pp(input) << " ->* " << data::pp(exp_output) << std::endl
              << "input    " << data::pp(input) << std::endl
              << "expected " << data::pp(exp_output) << std::endl
              << "output " << data::pp(output) << std::endl
              << " -- term representations -- " << std::endl
              << "input    " << atermpp::aterm(input) << std::endl
              << "expected " << atermpp::aterm(exp_output) << std::endl
              << "R(input) " << atermpp::aterm(output) << std::endl;
  }
}

BOOST_AUTO_TEST_CASE(multiplication_and_addition_test)
{
  // This test checks whether (x+y)(x-y) = x^2-y^2 for increasingly large numbers x and y. 
  // The test considers number of up to approximately 100 decimal digits. 
  data_specification specification;
  specification.add_context_sort(sort_nat::nat());

  rewrite_strategy_vector strategies(data::detail::get_test_rewrite_strategies(false));
  for (const rewrite_strategy strat: strategies)
  {
    std::cerr << "Multiplication and addition test1: " << strat << std::endl;
    data::rewriter R(specification, strat);

    data::data_expression x = sort_nat::nat(107);
    data::data_expression y = sort_nat::nat(10);
    for (std::size_t i=0; i<number_of_tests; ++i)
    {
      data_expression lhs = sort_nat::times(sort_nat::plus(x,y),sort_nat::monus(x,y));
      data_expression rhs = sort_nat::monus(sort_nat::times(x,x),sort_nat::times(y,y));
      data_rewrite_test(R, lhs, rhs);
      x = sort_nat::times(x,sort_nat::nat(11+i));
      y = sort_nat::times(y,sort_nat::nat(3+i));
    }
  }
}

BOOST_AUTO_TEST_CASE(mod_and_div_test)
{
  // This test checks whether x = (x div y)*y + x mod y. 
  // The test considers number of up to approximately 100 decimal digits. 
  data_specification specification;
  specification.add_context_sort(sort_nat::nat());

  rewrite_strategy_vector strategies(data::detail::get_test_rewrite_strategies(false));
  for (const mcrl2::data::rewrite_strategy& strategy: strategies)
  {
    std::cerr << "Mod and div test: " << strategy << std::endl;
    data::rewriter R(specification, strategy);

    data::data_expression x = sort_nat::nat(235);
    data::data_expression y = sort_pos::pos(78);
    for (std::size_t i=0; i<number_of_tests; ++i)
    {
      data_expression lhs = x;
      data_expression rhs = sort_nat::plus(sort_nat::times(sort_nat::div(x,y),sort_nat::pos2nat(y)),sort_nat::mod(x,y));
      data_rewrite_test(R, lhs, rhs);
      x = sort_nat::times(x,sort_nat::nat(7+i));
      y = sort_pos::times(y,sort_pos::pos(5+i));
    }
  }
}

BOOST_AUTO_TEST_CASE(square_root_test)
{
  // This test checks whether (r+1)&(r+1) > x >= r*r where r is the integer square root of x. 
  // The test considers number of up to approximately 100 decimal digits. 
  data_specification specification;
  specification.add_context_sort(sort_nat::nat());

  rewrite_strategy_vector strategies(data::detail::get_test_rewrite_strategies(false));
  for (const mcrl2::data::rewrite_strategy& strategy: strategies)
  {
    std::cerr << "Square root test: " << strategy << std::endl;
    data::rewriter R(specification, strategy);

    data::data_expression x = sort_nat::nat(831);
    for (std::size_t i=0; i<number_of_tests; ++i)
    {
       
      data_expression r = sort_nat::sqrt(x);
      data_expression check = greater_equal(x,sort_nat::times(r,r));
      data_rewrite_test(R, check, sort_bool::true_());

      data_expression r1 = sort_nat::plus(r,sort_nat::nat(1));
      check = greater(sort_nat::times(r1,r1),x);
      data_rewrite_test(R, check, sort_bool::true_());
      x = sort_nat::times(x,sort_nat::nat(23+i));
    }
  }
}

#ifdef MCRL2_ENABLE_MACHINENUMBERS
// The tests below use numbers whose machine word digits are 0, 1 or the largest machine word,
// where carries propagate over several digits. Inputs and expected outputs are constructed
// directly from their digits, so they do not depend on the parsing of numbers or on rewriting.

using big_natural = boost::multiprecision::cpp_int;
static constexpr std::size_t word_bits = std::numeric_limits<std::size_t>::digits;

// The machine word digits of n >= 0, least significant first.
static std::vector<std::size_t> digits_of(big_natural n)
{
  std::vector<std::size_t> result;
  do
  {
    result.push_back(static_cast<std::size_t>(n & std::numeric_limits<std::size_t>::max()));
    n >>= word_bits;
  }
  while (n != 0);
  return result;
}

// The normal form of n as an expression of sort Nat.
static data_expression nat_from_digits(const big_natural& n)
{
  const std::vector<std::size_t> d = digits_of(n);
  data_expression result = sort_nat::most_significant_digit_nat(machine_number(d.back()));
  for (std::size_t i = d.size() - 1; i > 0; --i)
  {
    result = sort_nat::concat_digit(result, machine_number(d[i - 1]));
  }
  return result;
}

// The normal form of n > 0 as an expression of sort Pos.
static data_expression pos_from_digits(const big_natural& n)
{
  assert(n > 0);
  const std::vector<std::size_t> d = digits_of(n);
  data_expression result = sort_pos::most_significant_digit(machine_number(d.back()));
  for (std::size_t i = d.size() - 1; i > 0; --i)
  {
    result = sort_pos::concat_digit(result, machine_number(d[i - 1]));
  }
  return result;
}

// Positive numbers around powers of 2^word_bits, with digits 0, 1 and the largest machine word.
static std::vector<big_natural> edge_values()
{
  const big_natural base = big_natural(1) << word_bits;
  std::vector<big_natural> result{1, 2, 3};
  big_natural b = 1;
  for (std::size_t k = 1; k <= 3; ++k)
  {
    b *= base;
    const std::vector<big_natural> around_b{b - 2, b - 1, b, b + 1, 2 * b - 1, 2 * b, b * (base - 1),
                                            b * base - b + 1};
    result.insert(result.end(), around_b.begin(), around_b.end());
  }
  return result;
}

BOOST_AUTO_TEST_CASE(numeral_parsing_test)
{
  // Decimal numerals must be translated to the digits of the number they denote.
  for (const big_natural& v: edge_values())
  {
    const std::string s = v.str();
    BOOST_CHECK_MESSAGE(sort_nat::nat(s) == nat_from_digits(v), "Nat numeral " << s);
    BOOST_CHECK_MESSAGE(sort_pos::pos(s) == pos_from_digits(v), "Pos numeral " << s);
  }
}

BOOST_AUTO_TEST_CASE(edge_value_arithmetic_test)
{
  // Arithmetic on all pairs of edge values, compared with the result computed with cpp_int.
  data_specification specification;
  specification.add_context_sort(sort_nat::nat());

  const std::vector<big_natural> values = edge_values();
  rewrite_strategy_vector strategies(data::detail::get_test_rewrite_strategies(false));
  for (const rewrite_strategy strategy: strategies)
  {
    std::cerr << "Edge value arithmetic test: " << strategy << std::endl;
    data::rewriter R(specification, strategy);
    for (const big_natural& x: values)
    {
      for (const big_natural& y: values)
      {
        data_rewrite_test(R, sort_pos::plus(pos_from_digits(x), pos_from_digits(y)), pos_from_digits(x + y));
        data_rewrite_test(R, sort_pos::times(pos_from_digits(x), pos_from_digits(y)), pos_from_digits(x * y));
        data_rewrite_test(R, sort_nat::plus(nat_from_digits(x), nat_from_digits(y)), nat_from_digits(x + y));
        data_rewrite_test(R, sort_nat::times(nat_from_digits(x), nat_from_digits(y)), nat_from_digits(x * y));
        data_rewrite_test(R, sort_nat::monus(nat_from_digits(x), nat_from_digits(y)),
                          nat_from_digits(x > y ? big_natural(x - y) : big_natural(0)));
        data_rewrite_test(R, sort_nat::div(nat_from_digits(x), pos_from_digits(y)), nat_from_digits(x / y));
        data_rewrite_test(R, sort_nat::mod(nat_from_digits(x), pos_from_digits(y)), nat_from_digits(x % y));
      }
    }
  }
}
#endif // MCRL2_ENABLE_MACHINENUMBERS

