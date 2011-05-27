// Author(s): Frank Stappers
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file list_test.cpp
/// \brief Basic regression test for quantifier expressions.

#include <boost/range/iterator_range.hpp>
#include <boost/test/minimal.hpp>

#include "mcrl2/data/list.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/set.h"
#include "mcrl2/data/fset.h"
#include "mcrl2/data/standard.h"

#include "mcrl2/core/garbage_collection.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::data::sort_list;

void quantifier_expression_test(mcrl2::data::rewriter::strategy s)
{
  data_specification specification;
  specification = parse_data_specification(
                                       "sort S = struct s1?is_s1 | s2?is_s2;"
                                       "sort T = struct t;"
                                       "sort L = List(T);");

  specification.add_context_sort(sort_nat::nat());
  specification.add_context_sort(sort_set::set_(sort_nat::nat()));

  rewriter r(specification, s);

  /* Test 1*/
  data_expression t1d1 = parse_data_expression("exists x: Bool. x == false");
  data_expression t1d2 = parse_data_expression("true");
  BOOST_CHECK(r(t1d1) == r(t1d2));

  /* Test 2*/
  data_expression t2d1 = parse_data_expression("exists x: Nat. (  x in {1,2,25,600} && 25 == x )", specification);
  data_expression t2d2 = parse_data_expression("true");
  BOOST_CHECK(r(t2d1) == r(t2d2));

  /* Test 3*/
  data_expression t3d1 = parse_data_expression("forall x: Nat. exists y: Nat. y == x");
  data_expression t3d2 = parse_data_expression("true");
  BOOST_CHECK(r(t3d1) == r(t3d2));

  /* Test 4*/
  data_expression t4d1 = parse_data_expression("exists s: S.( is_s1(s) && is_s2(s) )", specification);
  data_expression t4d2 = parse_data_expression("false");
  BOOST_CHECK(r(t4d1) == r(t4d2));

  /* Test 5*/
  data_expression t5d1 = parse_data_expression("exists s: S.( s == s2 && is_s2(s) )", specification);
  data_expression t5d2 = parse_data_expression("true");
  BOOST_CHECK(r(t5d1) == r(t5d2));

  /* Test 6*/
  data_expression t6d1 = parse_data_expression("forall y: T. y in [t]", specification);
  data_expression t6d2 = parse_data_expression("true");
  BOOST_CHECK(r(t6d1) == r(t6d2));

  /* Test 7*/
  data_expression t7d1 = parse_data_expression("forall b: Bool. b", specification);
  data_expression t7d2 = parse_data_expression("false");
  BOOST_CHECK(r(t7d1) == r(t7d2));

  /* Test 8*/
  data_expression t8d1 = parse_data_expression("forall x: Nat. x == 3", specification);
  data_expression t8d2 = parse_data_expression("false");
  BOOST_CHECK(r(t8d1) == r(t8d2));

}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv);

  std::cout << "jitty" << std::endl;
  quantifier_expression_test(mcrl2::data::rewriter::jitty);
  core::garbage_collect();

//#ifdef MCRL2_INNERC_AVAILABLE
//  std::cout << "innermost_compiling" << std::endl;
//  quantifier_expression_test(mcrl2::data::rewriter::innermost_compiling);
//  core::garbage_collect();
//#endif
//
//#ifdef MCRL2_JITTYC_AVAILABLE
//  std::cout << "jitty_compiling" << std::endl;
//  quantifier_expression_test(mcrl2::data::rewriter::jitty_compiling);
//  core::garbage_collect();
//#endif

  return EXIT_SUCCESS;
}
