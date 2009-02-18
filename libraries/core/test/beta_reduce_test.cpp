// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file beta_reduce_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <boost/test/minimal.hpp>
#include "mcrl2/atermpp/atermpp.h"
#include "mcrl2/core/detail/data_common.h"
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/core/numeric_string.h"
#include "mcrl2/core/messaging.h"

using namespace mcrl2::core;
using namespace mcrl2::core::detail;

int test_main(int argc, char** argv)
{
  MCRL2_ATERM_INIT(argc, argv)

  gsSetVerboseMsg();

  ATermAppl x_name = gsString2ATermAppl("x");
  ATermAppl y_name = gsString2ATermAppl("y");
  ATermAppl x = gsMakeDataVarId(x_name, gsMakeSortExprNat());
  ATermAppl y = gsMakeDataVarId(y_name, gsMakeSortExprNat());
  ATermList xl = ATmakeList1((ATerm) x);
  ATermList yl = ATmakeList1((ATerm) y);
  ATermAppl five = gsMakeDataExprNat("5");
  ATermAppl add_x = gsMakeDataExprAdd(x, five);
  ATermAppl add_y = gsMakeDataExprAdd(y, five);
  ATermAppl lambda = gsMakeBinder(gsMakeLambda(), xl, add_x);
  ATermAppl lambda_appl = gsMakeDataAppl(lambda, yl);
  ATermAppl beta_reduced_lambda_appl = (ATermAppl) beta_reduce_term((ATerm) lambda_appl);
  BOOST_CHECK(ATisEqual(beta_reduced_lambda_appl, add_y));

  // Check that beta reduction preserves annotation if annotated with @dummy
  ATermAppl sort = gsMakeSortId(gsString2ATermAppl("sort"));
  ATerm dummy = (ATerm) gsString2ATermAppl("@dummy");
  ATermAppl u_name = gsString2ATermAppl("u");
  ATermAppl v_name = gsString2ATermAppl("v");
  ATermAppl u = gsMakeDataVarId(u_name, sort);
  ATermAppl v = gsMakeDataVarId(v_name, sort);
  u = (ATermAppl) ATsetAnnotation((ATerm) u,  dummy, dummy);
  v = (ATermAppl) ATsetAnnotation((ATerm) v,  dummy, dummy);
  ATermList ul = ATmakeList1((ATerm) u);
  ATermList vl = ATmakeList1((ATerm) v);
  ATermAppl u_lambda = gsMakeBinder(gsMakeLambda(), ul, u);
  ATermAppl u_lambda_appl = gsMakeDataAppl(u_lambda, vl);
  ATermAppl u_beta_reduced_lambda_appl = (ATermAppl) beta_reduce_term((ATerm) u_lambda_appl);
  BOOST_CHECK(ATisEqual(u_beta_reduced_lambda_appl, v));

  return 0;
}
