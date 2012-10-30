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
#include "mcrl2/data/utility.h"
#include "mcrl2/data/parse.h"

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
  arguments.push_back(structured_sort_constructor_argument("s", basic_sort("E")));
  arguments.push_back(structured_sort_constructor_argument("n", sort_nat::nat()));

  std::vector< structured_sort_constructor > constructors;
  constructors.push_back(structured_sort_constructor("d", boost::make_iterator_range(arguments.begin(), arguments.begin() + 1)));
  constructors.push_back(structured_sort_constructor("e", boost::make_iterator_range(arguments.begin() + 1, arguments.begin() + 2)));

  specification.add_alias(alias(basic_sort("D"), structured_sort(boost::make_iterator_range(constructors.begin(), constructors.begin() + 1))));
  specification.add_alias(alias(basic_sort("E"), structured_sort(boost::make_iterator_range(constructors.begin() + 1, constructors.begin() + 2))));

  representative_generator default_expression_generator(specification);

  // Should be true or false, since constants are preferred to other constructors or mappings
  BOOST_CHECK(default_expression_generator(sort_bool::bool_()) == sort_bool::true_() ||
              default_expression_generator(sort_bool::bool_()) == sort_bool::false_());

  // Should be c0, since constants are preferred to other constructors or mappings
  BOOST_CHECK(default_expression_generator(sort_nat::nat()) == sort_nat::c0());

  // Should be nil, since constants are preferred to other constructors or mappings
  BOOST_CHECK(default_expression_generator(sort_list::list(sort_bool::bool_())) == sort_list::nil(sort_bool::bool_()));

  // Should be e(0), since constants are preferred to other constructors or mappings
  BOOST_CHECK(default_expression_generator(basic_sort("E")) ==
              boost::next(constructors.begin(), 1)->constructor_function(basic_sort("E"))(default_expression_generator(sort_nat::nat())));

  // Should be d(e(0)), since constants are preferred to other constructors or mappings
  BOOST_CHECK(default_expression_generator(basic_sort("D")) ==
              boost::next(constructors.begin(), 0)->constructor_function(basic_sort("D"))(default_expression_generator(basic_sort("E"))));

  // Check whether the representative of the set of reals is the empty set of reals.
  BOOST_CHECK(default_expression_generator(container_sort(set_container(),data::sort_real::real_())) ==
              data::sort_set::empty(data::sort_real::real_()));


}

int test_main(int argc, char** argv)
{
  test_representative_generator();

  return 0;
}
