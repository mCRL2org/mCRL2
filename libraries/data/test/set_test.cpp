// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file set_test.cpp
/// \brief Basic regression test for set expressions.

#include <boost/range/iterator_range.hpp>
#include <boost/test/minimal.hpp>

#include "mcrl2/data/standard.h"
#include "mcrl2/data/set.h"
#include "mcrl2/data/fset.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/rewriter.h"

#include "mcrl2/core/garbage_collection.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::data::sort_set;
using namespace mcrl2::data::sort_fset;

template <typename Predicate>
void test_data_expression(const std::string& s, variable_vector v, Predicate p)
{
  std::cerr << "testing data expression " << s << std::endl;
  data_expression e = parse_data_expression(s, v.begin(), v.end());
  BOOST_CHECK(p(e));
}

/* Test case for various set expressions, based
   on the following specification:

proc P(s: Set(Nat)) = (1 in s) -> tau . P({} + s - {20} * {40})
                    + ({10} < s) -> tau . P(!s)
                    + (s <= {20} + Bag2Set({20:4, 30:3, 40:2})) -> tau . P(s)
                    + (s <= { n:Nat | true }) -> tau . P(s);

init P({20, 30, 40});

*/
void set_expression_test()
{
  data::data_specification specification;

  specification.add_context_sort(sort_set::set_(sort_pos::pos()));

  data::rewriter normaliser(specification);

  variable_vector v;
  v.push_back(parse_variable("s:Set(Nat)"));

  test_data_expression("{x : Nat | x < 10}", v, is_setcomprehension_application);
  test_data_expression("!s", v, is_setcomplement_application);
  test_data_expression("s * {1,2,3}", v, is_setintersection_application);
  test_data_expression("s - {3,1,2}", v, is_setdifference_application);
  test_data_expression("1 in s", v, is_setin_application);
  test_data_expression("{} + s", v, is_setunion_application);
  test_data_expression("(({} + s) - {20}) * {40}", v, is_setintersection_application);
  test_data_expression("{10} < s", v, is_less_application<data_expression>);
  test_data_expression("s <= {10}", v, is_less_equal_application<data_expression>);
  test_data_expression("{20} + {30}", v, is_setunion_application);

  data_expression e = parse_data_expression("{20}", v.begin(), v.end());
  BOOST_CHECK(is_setconstructor_application(normaliser(e)));

  e = parse_data_expression("{20, 30, 40}", v.begin(), v.end());
  BOOST_CHECK(is_setconstructor_application(normaliser(e)));
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv);

  set_expression_test();
  core::garbage_collect();

  return EXIT_SUCCESS;
}

