// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file list_test.cpp
/// \brief Basic regression test for list expressions.

#include <boost/range/iterator_range.hpp>
#include <boost/test/minimal.hpp>

#include "mcrl2/data/list.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/rewriter.h"

#include "mcrl2/atermpp/aterm_init.h"

using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::data::sort_list;

template <typename Predicate>
void test_data_expression(std::string s, mcrl2::data::variable_vector v, Predicate p)
{
  data_expression e = parse_data_expression(s, v.begin(), v.end());
  BOOST_CHECK(p(e));
}

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

  specification.add_context_sort(sort_list::list(sort_pos::pos()));

  data::rewriter normaliser(specification);

  variable_vector v;
  v.push_back(parse_variable("l:List(Nat)"));

  test_data_expression("1 in l", v, is_in_application);
  test_data_expression("10 |> l", v, is_cons_application);
  test_data_expression("l <| 10", v, is_snoc_application);
  test_data_expression("#l", v, is_count_application);
  test_data_expression("l ++ [10]", v, is_concat_application);
  test_data_expression("l.1", v, is_element_at_application);
  test_data_expression("head(l)", v, is_head_application);
  test_data_expression("rhead(l)", v, is_rhead_application);
  test_data_expression("tail(l)", v, is_tail_application);
  test_data_expression("rtail(l)", v, is_rtail_application);

  data_expression e = parse_data_expression("[10]", v.begin(), v.end());
  BOOST_CHECK(is_cons_application(normaliser(e)));

}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv);

  list_expression_test();

  return EXIT_SUCCESS;
}

