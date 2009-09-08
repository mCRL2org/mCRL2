// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file bag_test.cpp
/// \brief Basic regression test for bag expressions.

#include <boost/range/iterator_range.hpp>
#include <boost/test/minimal.hpp>

#include "mcrl2/data/list.h"
#include "mcrl2/data/parser.h"
#include "mcrl2/data/rewriter.h"

#include "mcrl2/core/garbage_collection.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::data::sort_list;

/* Test case for various list expressions, based
   on the following specification:

proc P(l: List(Nat)) = (1 in l) -> tau . P(10 |> l ++ [#l] <| 100)
                     + (l.1 == 30) -> tau . P(tail(l))
                     + (head(l) == 20) -> tau . P([rhead(l)] ++ rtail(l));

init P([20, 30, 40]);
*/
void list_expression_test()
{
  data::data_specification specification;

  specification.import_system_defined_sort(sort_list::list(sort_pos::pos()));

  data::rewriter normaliser(specification);

  variable_vector v;
  v.push_back(parse_variable("l:List(Nat)"));

  data_expression e;
  e = parse_data_expression("1 in l", v.begin(), v.end());
  BOOST_CHECK(is_in_application(e));

  e = parse_data_expression("10 |> l", v.begin(), v.end());
  BOOST_CHECK(is_cons_application(e));

  e = parse_data_expression("l <| 10", v.begin(), v.end());
  BOOST_CHECK(is_snoc_application(e));

  e = parse_data_expression("#l", v.begin(), v.end());
  BOOST_CHECK(is_count_application(e));
  
  e = parse_data_expression("[10]", v.begin(), v.end());
  BOOST_CHECK(is_cons_application(normaliser(e)));

  e = parse_data_expression("l ++ [10]", v.begin(), v.end());
  BOOST_CHECK(is_concat_application(e));

  e = parse_data_expression("l.1", v.begin(), v.end());
  BOOST_CHECK(is_element_at_application(e));

  e = parse_data_expression("head(l)", v.begin(), v.end());
  BOOST_CHECK(is_head_application(e));

  e = parse_data_expression("rhead(l)", v.begin(), v.end());
  BOOST_CHECK(is_rhead_application(e));

  e = parse_data_expression("tail(l)", v.begin(), v.end());
  BOOST_CHECK(is_tail_application(e));

  e = parse_data_expression("rtail(l)", v.begin(), v.end());
  BOOST_CHECK(is_rtail_application(e));
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv);

  list_expression_test();
  core::garbage_collect();

  return EXIT_SUCCESS;
}

