// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file parse_test.cpp
/// \brief Regression test for parsing process expressions

#define MCRL2_DEBUG_EXPRESSION_BUILDER

#include <iostream>
#include <boost/range/iterator_range.hpp>
#include <boost/test/minimal.hpp>

#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/process/parse.h"

using namespace mcrl2;
using namespace mcrl2::process;

std::string DATA_DECL =
  "glob                   \n"
  "  m: Nat;              \n"
  "                       \n"
  "act                    \n"
  "  a: Nat;              \n"
  ;

std::string PROC_DECL =
  "proc P(n:Nat);         \n"
  ;

void test_parse_process_specification()
{
  std::string text =
    "act  a: Nat;                                   \n"
    "                                               \n"
    "glob v: Nat;                                   \n"
    "                                               \n"
    "proc P(i,j: Nat) = P(1, 1); \n"
    "                                               \n"
    "init P(i = 1, j = v);                          \n"
    ;

  process_specification p = parse_process_specification(text);
}

void test_parse()
{
  process_expression x = parse_process_expression("a(m).P(0)", DATA_DECL, PROC_DECL);
  BOOST_CHECK(process::pp(x) == "a(m) . P(0)");
  test_parse_process_specification();
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv);

  test_parse_process_specification();
  test_parse();

  return EXIT_SUCCESS;
}
