// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file enumerated_sort_generator_test.cpp
/// \brief Tests for the generation of enumerated sorts and case functions.

#define BOOST_TEST_MODULE enumerated_sort_generator_test

#include <boost/test/included/unit_test.hpp>

#include "mcrl2/data/enumerated_sort_generator.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/rewriter.h"

using namespace mcrl2;
using namespace mcrl2::data;

BOOST_AUTO_TEST_CASE(test_two_elements)
{
  data_specification specification;
  set_identifier_generator identifier_generator;
  enumerated_sort_generator generator(specification, identifier_generator);

  BOOST_CHECK_EQUAL(generator(2), sort_bool::bool_());
  BOOST_CHECK_EQUAL(generator.elements(2), data_expression_list({sort_bool::false_(), sort_bool::true_()}));
  BOOST_CHECK_EQUAL(generator.case_function(2, sort_nat::nat()), if_(sort_nat::nat()));
  BOOST_CHECK(specification.user_defined_equations().empty());
}

BOOST_AUTO_TEST_CASE(test_three_elements)
{
  data_specification specification;
  set_identifier_generator identifier_generator;
  // The names Enum3 and e0_3 are in use, so the generator must take other names.
  identifier_generator.add_identifier(core::identifier_string("Enum3"));
  identifier_generator.add_identifier(core::identifier_string("e0_3"));
  enumerated_sort_generator generator(specification, identifier_generator);

  const sort_expression enumerated_sort = generator(3);
  BOOST_CHECK(is_basic_sort(enumerated_sort));
  BOOST_CHECK_NE(atermpp::down_cast<basic_sort>(enumerated_sort).name(), core::identifier_string("Enum3"));
  BOOST_CHECK_EQUAL(specification.user_defined_aliases().size(), 1U);

  const data_expression_list& elements = generator.elements(3);
  BOOST_CHECK_EQUAL(elements.size(), 3U);
  for (const data_expression& e: elements)
  {
    BOOST_CHECK(is_function_symbol(e));
    BOOST_CHECK_EQUAL(e.sort(), enumerated_sort);
    BOOST_CHECK_NE(atermpp::down_cast<function_symbol>(e).name(), core::identifier_string("e0_3"));
  }

  // The same sort is returned for the same number of elements.
  BOOST_CHECK_EQUAL(generator(3), enumerated_sort);
  BOOST_CHECK_EQUAL(specification.user_defined_aliases().size(), 1U);

  const function_symbol c = generator.case_function(3, sort_nat::nat());
  BOOST_CHECK_EQUAL(c.sort(),
    function_sort(sort_expression_list({enumerated_sort, sort_nat::nat(), sort_nat::nat(), sort_nat::nat()}),
      sort_nat::nat()));
  BOOST_CHECK_EQUAL(specification.user_defined_equations().size(), 4U);

  // The same case function is returned for the same sort, and no equations are added.
  BOOST_CHECK_EQUAL(generator.case_function(3, sort_nat::nat()), c);
  BOOST_CHECK_EQUAL(specification.user_defined_equations().size(), 4U);

  // The i-th element selects the i-th argument.
  rewriter r(specification);
  const data_expression one = sort_nat::nat(1);
  const data_expression two = sort_nat::nat(2);
  const data_expression three = sort_nat::nat(3);
  const data_expression_vector arguments{one, two, three};
  std::size_t i = 0;
  for (const data_expression& e: elements)
  {
    BOOST_CHECK_EQUAL(r(application(c, e, one, two, three)), arguments[i]);
    ++i;
  }

  // C(e, x, x, x) = x, also if e is not an element.
  const variable v("v", enumerated_sort);
  BOOST_CHECK_EQUAL(r(application(c, v, two, two, two)), two);
}

BOOST_AUTO_TEST_CASE(test_different_sizes_and_sorts)
{
  data_specification specification;
  set_identifier_generator identifier_generator;
  enumerated_sort_generator generator(specification, identifier_generator);

  BOOST_CHECK_NE(generator(3), generator(4));
  BOOST_CHECK_EQUAL(generator.elements(4).size(), 4U);
  BOOST_CHECK_EQUAL(generator.elements(1).size(), 1U);

  const function_symbol c_nat = generator.case_function(4, sort_nat::nat());
  const function_symbol c_bool = generator.case_function(4, sort_bool::bool_());
  BOOST_CHECK_NE(c_nat, c_bool);
  BOOST_CHECK_NE(c_nat.name(), c_bool.name());
  BOOST_CHECK_EQUAL(atermpp::down_cast<function_sort>(c_bool.sort()).codomain(), sort_bool::bool_());
  // Five equations per case function on a sort with four elements.
  BOOST_CHECK_EQUAL(specification.user_defined_equations().size(), 10U);

  rewriter r(specification);
  BOOST_CHECK_EQUAL(r(application(c_bool,
                      data_expression_list({generator.elements(4).front(),
                        sort_bool::true_(),
                        sort_bool::false_(),
                        sort_bool::false_(),
                        sort_bool::false_()}))),
    sort_bool::true_());
}
