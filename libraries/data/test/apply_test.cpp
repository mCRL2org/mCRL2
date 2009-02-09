// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file apply_test.cpp
/// \brief Add your file description here.

#include <sstream>
#include <boost/test/minimal.hpp>

#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/make_list.h"
#include "mcrl2/data/data.h"

using namespace std;
using namespace atermpp;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace mcrl2::data;
using namespace mcrl2::data::data_expr;

mcrl2::data::data_expression test_substitution(mcrl2::data::data_expression d)
{
  data_variable a("i:Int");
  data_expression b = data_expression(gsMakeDataExprInt("4"));
  data_assignment f(a, b); // i := 4

  return d.substitute(f);
}

void test_apply()
{
  data_variable a("i:Int");
  data_expression b(gsMakeDataExprInt("4"));
  data_expression c = data_expr::equal_to(a, b); // i == 4

  data_expression_list cl; //singleton Container with c
  cl = push_front(cl, c);

  data_expression_list dl = apply(cl, test_substitution); // 4 == 4

  cerr << "cl: " << cl << endl;
  cerr << "dl: " << dl << endl;
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)

  test_apply();
  return 0;
}
