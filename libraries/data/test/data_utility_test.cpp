// Author(s): Wieger Wesselink
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
#include <boost/bind.hpp>
#include <boost/test/minimal.hpp>
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/print.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/core/garbage_collection.h"

using namespace mcrl2;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace mcrl2::data;
using namespace mcrl2::data::sort_bool;

void test_fresh_variables()
{
  variable_list w = atermpp::make_list(variable("d", basic_sort("D")), variable("e", basic_sort("E")), variable("f", basic_sort("F")));
  std::set<std::string> context;
  context.insert("e");
  context.insert("f_00");
  variable_list w1 = fresh_variables(w, context);
  std::cout << "w1 = " << data::pp(w1) << std::endl;

  context.clear();
  context.insert("e3_Sx0");
  context.insert("e_Sx0");
  context.insert("n0_Sx0");
  context.insert("n_S");
  context.insert("s3_S");
  std::cout << "\n" << core::detail::print_set(context, core::detail::default_printer(), "context") << std::endl;
  variable_list yi = atermpp::make_list(variable("e3_S", basic_sort("A")));
  std::cout << "\nyi " << data::pp(yi) << std::endl;
  variable_list y = fresh_variables(yi, context);
  std::cout << "\ny " << data::pp(y) << std::endl;
  BOOST_CHECK(y.size() == 1);
  BOOST_CHECK(std::string(y.front().name()) != " e3_Sx0");
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)

  test_fresh_variables();
  core::garbage_collect();

  return 0;
}
