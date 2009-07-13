// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file data_utility_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <string>
#include <set>
#include <algorithm>

#include "boost/bind.hpp"
#include "boost/test/minimal.hpp"

#include "mcrl2/data/representative_generator.h"
#include "mcrl2/data/nat.h"
#include "mcrl2/data/bool.h"
#include "mcrl2/data/list.h"
#include "mcrl2/data/structured_sort.h"
#include "mcrl2/core/garbage_collection.h"
#include "mcrl2/data/utility.h"

using namespace mcrl2;
using namespace mcrl2::data;

void test_representative_generator()
{
  data_specification specification;

  specification.import_system_defined_sort(sort_nat::nat());
  specification.import_system_defined_sort(sort_list::list(sort_bool::bool_()));

  atermpp::vector< data::structured_sort_constructor_argument > arguments;
  arguments.push_back(structured_sort_constructor_argument(basic_sort("E"), "s"));
  arguments.push_back(structured_sort_constructor_argument(sort_nat::nat(), "n"));

  atermpp::vector< structured_sort_constructor > constructors;
  constructors.push_back(structured_sort_constructor("d", boost::make_iterator_range(arguments.begin(), arguments.begin() + 1)));
  constructors.push_back(structured_sort_constructor("e", boost::make_iterator_range(arguments.begin() + 1, arguments.begin() + 2)));

  specification.add_sort(alias(basic_sort("D"), structured_sort(boost::make_iterator_range(constructors.begin(), constructors.begin() + 1))));
  specification.add_sort(alias(basic_sort("E"), structured_sort(boost::make_iterator_range(constructors.begin() + 1, constructors.begin() + 2))));

  representative_generator default_expression_generator(specification);

  // Check whether it can handle structured sorts as sort expression
  BOOST_CHECK(default_expression_generator(structured_sort(boost::make_iterator_range(constructors.begin(), constructors.begin() + 1))));

  // Should be true or false, since constants are preferred to other constructors or mappings
  BOOST_CHECK(default_expression_generator(sort_bool::bool_()) == sort_bool::true_() ||
              default_expression_generator(sort_bool::bool_()) == sort_bool::false_());

  // Should be c0, since constants are preferred to other constructors or mappings
  BOOST_CHECK(default_expression_generator(sort_nat::nat()) == sort_nat::c0());

  // Should be nil, since constants are preferred to other constructors or mappings
  BOOST_CHECK(default_expression_generator(sort_list::list(sort_bool::bool_())) == sort_list::nil(sort_bool::bool_()));

  // Should be e(0), since constants are preferred to other constructors or mappings
  BOOST_CHECK(default_expression_generator(basic_sort("E")) ==
      application(boost::next(constructors.begin(), 1)->constructor_function(specification.find_referenced_sort(basic_sort("E"))), default_expression_generator(sort_nat::nat())));

  // Should be d(e(0)), since constants are preferred to other constructors or mappings
  BOOST_CHECK(default_expression_generator(basic_sort("D")) ==
       application(boost::next(constructors.begin(), 0)->constructor_function(specification.find_referenced_sort(basic_sort("D"))), default_expression_generator(basic_sort("E"))));
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)

  test_representative_generator();
  core::garbage_collect();

  return 0;
}
