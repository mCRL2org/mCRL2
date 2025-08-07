// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file representative_generator_test.cpp
/// \brief Add your file description here.

#define BOOST_TEST_MODULE representative_generator_test

#include <boost/test/included/unit_test.hpp>

#include "mcrl2/data/list.h"
#include "mcrl2/data/set.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/representative_generator.h"

using namespace mcrl2;
using namespace mcrl2::data;

void test_representative_generator()
{
  data_specification specification=parse_data_specification
                                   ("map f__:Nat; \n"
                                    "    g__:List(Bool);\n"
                                    "    h__:Set(Real);\n"
                                   );

  std::vector< data::structured_sort_constructor_argument > arguments;
  arguments.emplace_back("s", basic_sort("E"));
  arguments.emplace_back("n", sort_nat::nat());

  std::vector< structured_sort_constructor > constructors;
  constructors.emplace_back("d", structured_sort_constructor_argument_list(arguments.begin(), arguments.begin() + 1));
  constructors.emplace_back("e",
      structured_sort_constructor_argument_list(arguments.begin() + 1, arguments.begin() + 2));

  specification.add_alias(alias(basic_sort("D"), structured_sort(structured_sort_constructor_list(constructors.begin(), constructors.begin() + 1))));
  specification.add_alias(alias(basic_sort("E"), structured_sort(structured_sort_constructor_list(constructors.begin() + 1, constructors.begin() + 2))));

  representative_generator default_expression_generator(specification);

  // Should be true or false, since constants are preferred to other constructors or mappings
  BOOST_CHECK(default_expression_generator(sort_bool::bool_()) == sort_bool::true_() ||
              default_expression_generator(sort_bool::bool_()) == sort_bool::false_());

  // Should be c0, since constants are preferred to other constructors or mappings
  BOOST_CHECK(default_expression_generator(sort_nat::nat()) == sort_nat::c0());

  // Should be empty, since constants are preferred to other constructors or mappings
  BOOST_CHECK(default_expression_generator(sort_list::list(sort_bool::bool_())) == sort_list::empty(sort_bool::bool_()));

  // Should be e(0), since constants are preferred to other constructors or mappings
  BOOST_CHECK(default_expression_generator(basic_sort("E")) ==
              constructors[1].constructor_function(basic_sort("E"))(default_expression_generator(sort_nat::nat())));

  // Should be d(e(0)), since constants are preferred to other constructors or mappings
  BOOST_CHECK(default_expression_generator(basic_sort("D")) ==
              constructors[0].constructor_function(basic_sort("D"))(default_expression_generator(basic_sort("E"))));

  // Check whether the representative of the set of reals is the empty set of reals, or the set of all reals.
  // The answer depends on the relative ordering of the false_function and true_function in sets of function symbols.
  // This in turns depends on the place where these functions end up in the memory. 
  const data_expression t=default_expression_generator(container_sort(set_container(),data::sort_real::real_()));
  BOOST_CHECK(t == data::sort_set::constructor(data::sort_real::real_(),
                                               data::sort_set::false_function(data::sort_real::real_()),
                                               data::sort_fset::empty(data::sort_real::real_()))  ||
              t == data::sort_set::constructor(data::sort_real::real_(),
                                               data::sort_set::true_function(data::sort_real::real_()),
                                               data::sort_fset::empty(data::sort_real::real_())));

   // Check whether a proper representative term is generated for a function sort.
   const sort_expression s1=function_sort({ basic_sort("Nat")}, basic_sort("Nat"));
   BOOST_CHECK(default_expression_generator(s1).sort() == s1);
   const sort_expression s2=function_sort({ basic_sort("Nat"), basic_sort("Real")}, basic_sort("Nat"));
   BOOST_CHECK(default_expression_generator(s2).sort() == s2);
}

BOOST_AUTO_TEST_CASE(test_main)
{
  test_representative_generator();
}
