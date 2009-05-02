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
#include <string>
#include <set>
#include <boost/test/minimal.hpp>
#include "mcrl2/data/parser.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/lps/mcrl22lps.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/lps/process.h"
#include "mcrl2/lps/detail/linear_process_expression_visitor.h"
#include "mcrl2/lps/detail/linear_process_conversion_visitor.h"
#include "mcrl2/core/garbage_collection.h"

using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::lps;

const std::string SPEC = 
  "act  a;                  \n"
  "                         \n"
  "proc P(b: Bool) =        \n"
  "       sum c: Bool.      \n"
  "         (b == c) ->     \n"
  "         a.P(c);         \n"
  "                         \n"
  "init P(true);            \n"
  ;

void test_replace()
{
  specification spec = parse_linear_process_specification(SPEC);
  summand s = spec.process().summands().front();
  variable b("b", sort_bool_::bool_());
  variable c("c", sort_bool_::bool_());
  variable d("d", sort_bool_::bool_());
  assignment a(c, d);
  summand t = replace_data_expressions(s, a);
  std::cout << "<s>" << pp(s) << std::endl;
  std::cout << "<t>" << pp(t) << std::endl;
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  test_replace();
  core::garbage_collect();

  return 0;
}
