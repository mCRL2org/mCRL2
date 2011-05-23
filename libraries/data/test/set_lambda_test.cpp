// Author(s): Frank Stappers
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file list_test.cpp
/// \brief Basic regression test for merging sets with lambda expressions.

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

void set_lambda_test()
{
  data_specification specification;
  specification = parse_data_specification(
"sort Label       = struct task_1; "
"sort Transition = struct trans( pi_l: Label , pi_t: ProcessTerm ); "
"map EmptyLabelSet: Set( Label );"
"eqn EmptyLabelSet ={};"
"sort ProcessTerm = "
"         struct "
"         start_task( l:Label )?is_start_task"
"       | sync( pi_1: ProcessTerm, pi_2: ProcessTerm, shared: Set(Label) )?is_sync"
"       ;"
"map R_Trans: ProcessTerm -> Set(Transition);"
"    R_start_task: ProcessTerm -> Set(Transition);"
"    R_sync_3: ProcessTerm -> Set(Transition);"
"    R_sync_4: ProcessTerm -> Set(Transition);"
""
"var v: ProcessTerm; "
"eqn R_Trans(v) = "
"       R_start_task(v) "
"     + R_sync_3(v) "
"     + R_sync_4(v) "
"     ;"
""
"R_start_task(v)  = if( is_start_task(v), { s: Transition | pi_t(s) == v && pi_l(s) == task_1}, {} );"
"R_sync_3( v ) = if( is_sync(v), { s: Transition | "
"                                   is_sync(pi_t(s)) "
"                                && pi_2(pi_t(s)) == pi_2(v) "
"                                && trans( pi_l(s), pi_1(pi_t(s))) in R_Trans( pi_1(v))"
"                                && shared(v) == shared(pi_t(s))"
"                                }, {} );"
"    R_sync_4( v ) = if( is_sync(v), { s: Transition |  "
"                                   shared(v) == shared(pi_t(s))"
"                                && pi_t(s) == v "
"                                }, {} );" );

  specification.add_context_sort(sort_nat::nat());
  specification.add_context_sort(sort_set::set_(sort_nat::nat()));

  rewriter r(specification);

  /* Test 1*/
  data_expression t1d1 = parse_data_expression(
      "(lambda v: ProcessTerm. lambda s: Transition. is_sync(pi_t(s)) && pi_2(pi_t(s)) == pi_2(v) && trans(pi_l(s), pi_1(pi_t(s))) in R_Trans(pi_1(v)) && shared(v) == shared(pi_t(s)) ) "
      "(sync(start_task(task_1), start_task(task_1), {}) )"
      "( trans( task_1, sync(start_task(task_1), start_task(task_1), {}) ) )", specification);
  data_expression t1d2 = parse_data_expression("true");
  BOOST_CHECK(r(t1d1) == r(t1d2));

  data_expression t2d1 = parse_data_expression(
      "(lambda v: ProcessTerm. lambda s: Transition. shared(v) == shared(pi_t(s)) && pi_t(s) == v)"
      "(sync(start_task(task_1), start_task(task_1), {})) "
      "( trans( task_1, sync(start_task(task_1), start_task(task_1), {}) ) ) ", specification);
  data_expression t2d2 = parse_data_expression("true");
  BOOST_CHECK(r(t2d1) == r(t2d2));

  data_expression t3d1 = mcrl2::data::sort_bool::or_( t1d1 , t2d1 );
  data_expression t3d2 = parse_data_expression("true");
  BOOST_CHECK(r(t3d1) == r(t3d2));

  data_expression t4d1 = parse_data_expression(
  " { x: Transition |"
  "       (lambda v: ProcessTerm. lambda s: Transition. "
  "                is_sync(pi_t(s)) "
  "                   && pi_2(pi_t(s)) == pi_2(v) "
  "                   && trans(pi_l(s), pi_1(pi_t(s))) in R_Trans(pi_1(v)) "
  "                   && shared(v) == shared(pi_t(s))) "
  "          (sync(start_task(task_1), start_task(task_1), {}))"
  "          (x)"
  " || "
  "       (lambda v: ProcessTerm. lambda s: Transition. shared(v) == shared(pi_t(s)) && pi_t(s) == v) "
  "          (sync(start_task(task_1), start_task(task_1), {}))"
  "       (x)}", specification );
  data_expression t4d2 = parse_data_expression( "R_Trans( sync( start_task( task_1 ), start_task(task_1) , EmptyLabelSet ) )", specification );
  std::cout << "T4-D1 :" << pp( r(t4d1) ) << std::endl;
  std::cout << "T4-D2 :" << pp( r(t4d2) ) << std::endl;
  BOOST_CHECK(r(t4d1) == r(t4d2));
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv);

  set_lambda_test();
  core::garbage_collect();

  return EXIT_SUCCESS;
}
