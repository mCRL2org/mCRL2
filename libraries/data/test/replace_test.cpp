// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file replace_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <vector>
#include <boost/test/minimal.hpp>

#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/atermpp/make_list.h"
#include "mcrl2/core/garbage_collection.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/data/detail/data_functional.h"

using namespace mcrl2;
using namespace mcrl2::data;

void test_assignment_list()
{
  using namespace mcrl2::data::sort_bool;

  std::cerr << "assignment_list replace" << std::endl;

  variable d1("d1", basic_sort("D"));
  variable d2("d2", basic_sort("D"));
  variable d3("d3", basic_sort("D"));
  variable e1("e1", basic_sort("D"));
  variable e2("e2", basic_sort("D"));
  variable e3("e3", basic_sort("D"));

  assignment_vector l;
  l.push_back(assignment(d1, e1));
  l.push_back(assignment(e1, e2));
  l.push_back(assignment(e2, e3));

  data_expression t  = and_(equal_to(d1, e1), not_equal_to(e2, d3));
  data_expression t0 = and_(equal_to(e1, e2), not_equal_to(e3, d3));
  data_expression t2 = data::replace_free_variables(t, assignment_sequence_substitution(assignment_list(l.begin(), l.end())));
  std::cerr << "t  == " << data::pp(t) << std::endl;
  std::cerr << "t2 == " << data::pp(t2) << std::endl;
  BOOST_CHECK(t0 == t2);

  assignment_list m1 = atermpp::make_list(
                         assignment(d1, d2),
                         assignment(e1, d1)
                       );
  assignment_list m2 = data::replace_variables(m1, assignment(d2, d1));
  assignment_list m3 = atermpp::make_list(
                         assignment(d1, d1),
                         assignment(e1, d1)
                       );
  BOOST_CHECK(m2 == m3);
  std::cout << "<m2>" << data::pp(m2) << std::endl;
  std::cout << "<m3>" << data::pp(m3) << std::endl;
  core::garbage_collect();
}

void test_variable_replace()
{
  using namespace mcrl2::data::sort_bool;

  std::cerr << "variable replace" << std::endl;

  variable d1("d1", basic_sort("D"));
  variable d2("d2", basic_sort("D"));
  variable d3("d3", basic_sort("D"));
  variable_vector variables;
  variables.push_back(d1);
  variables.push_back(d2);
  variables.push_back(d3);

  variable x("x", basic_sort("D"));
  variable y("y", basic_sort("D"));
  variable z("z", basic_sort("D"));
  data_expression e1 = x;
  data_expression e2 = z;
  data_expression e3 = y;
  data_expression_vector replacements;
  replacements.push_back(e1);
  replacements.push_back(e2);
  replacements.push_back(e3);

  std::vector<variable> v;
  v.push_back(d1);
  v.push_back(d2);
  v.push_back(d3);
  std::list<data_expression> l;
  l.push_back(e1);
  l.push_back(e2);
  l.push_back(e3);

  data_expression t  = and_(equal_to(d1, d2), not_equal_to(d2, d3));
  data_expression t1 = data::replace_free_variables(t, make_sequence_sequence_substitution(variables, replacements));
  data_expression t2 = data::replace_free_variables(t, make_sequence_sequence_substitution(v, l));
  std::cerr << "t  == " << data::pp(t) << std::endl;
  std::cerr << "t1 == " << data::pp(t1) << std::endl;
  std::cerr << "t2 == " << data::pp(t2) << std::endl;
  BOOST_CHECK(t1 == t2);

  t = and_(equal_to(d1, d2), not_equal_to(d2, d3));
  BOOST_CHECK(t1 == replace_variables(t, make_sequence_sequence_substitution(variables, replacements)));
  BOOST_CHECK(t1 == replace_variables(t, make_sequence_sequence_substitution(variables, replacements)));
  BOOST_CHECK(t1 == replace_variables(t, make_sequence_sequence_substitution(v, l)));
  BOOST_CHECK(t1 == replace_variables(t, make_mutable_map_substitution(variables, replacements)));
  BOOST_CHECK(t1 == replace_free_variables(t, make_sequence_sequence_substitution(variables, replacements)));
  BOOST_CHECK(t1 == replace_free_variables(t, make_sequence_sequence_substitution(variables, replacements)));
  BOOST_CHECK(t1 == replace_free_variables(t, make_sequence_sequence_substitution(v, l)));
  BOOST_CHECK(t1 == replace_free_variables(t, make_mutable_map_substitution(variables, replacements)));
  core::garbage_collect();
}

void test_replace_with_binders()
{
  std::cerr << "replace with binders" << std::endl;
  mutable_map_substitution< > sigma;
  data_expression input1(variable("c", sort_bool::bool_()));
  data_expression input2(parse_data_expression("exists b: Bool, c: Bool. if(b, c, b)"));

  sigma[variable("c", sort_bool::bool_())] = sort_bool::false_();

  BOOST_CHECK(replace_free_variables(input1, sigma) == sort_bool::false_());

  // variable c is bound and should not be replaced
  BOOST_CHECK(replace_free_variables(input2, sigma) == input2);
  core::garbage_collect();
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)

  test_assignment_list();
  test_variable_replace();
  test_replace_with_binders();

  return 0;
}
