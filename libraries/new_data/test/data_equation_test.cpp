// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file data_equation_test.cpp
/// \brief Basic regression test for new_data equations.

#include <iostream>
#include <boost/range/iterator_range.hpp>
#include <boost/test/minimal.hpp>

#include "mcrl2/new_data/detail/container_utility.h"
#include "mcrl2/atermpp/atermpp.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/new_data/data_equation.h"
#include "mcrl2/new_data/basic_sort.h"
#include "mcrl2/new_data/variable.h"
#include "mcrl2/new_data/function_symbol.h"
#include "mcrl2/new_data/data_equation.h"
#include "mcrl2/core/garbage_collection.h"

using namespace mcrl2;
using namespace mcrl2::new_data;

void data_equation_test()
{
  basic_sort s("S");
  function_symbol c("c", s);
  function_symbol f("f", s);
  variable x("x", s);
  variable_vector xl(make_vector(x));
  boost::iterator_range<variable_vector::const_iterator> xl_range(boost::make_iterator_range(xl));

  data_equation e(xl_range, c, x, f);
  BOOST_CHECK(e.variables() == xl_range);
  BOOST_CHECK(e.condition() == c);
  BOOST_CHECK(e.lhs() == x);
  BOOST_CHECK(e.rhs() == f);
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv);

  data_equation_test();
  core::garbage_collect();

  return EXIT_SUCCESS;
}


