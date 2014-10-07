// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file specification_test.cpp
/// \brief Add your file description here.

#include <boost/test/minimal.hpp>
#include "mcrl2/lps/is_stochastic.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/lps/print.h"
#include "mcrl2/lps/stochastic_specification.h"

using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::lps;

void test_empty_distribution()
{
  stochastic_distribution dist;
  std::cout << "empty dist = " << dist << std::endl;
  BOOST_CHECK(!dist.is_defined());
}

void test_remove_stochastic_operators()
{
  std::string text =
    "act a;\n"
    "proc P = a.P;\n"
    "init P;"
    ;

  stochastic_specification src;
  parse_lps(text, src);
  BOOST_CHECK(!is_stochastic(src));

  specification dest;
  remove_stochastic_operators(src, dest);
  std::cout << "dest = " << dest;
  BOOST_CHECK(lps::pp(src) == lps::pp(dest));
}

void test_is_stochastic()
{
  std::string text =
    "act  throw: Bool;                       \n"
    "                                        \n"
    "proc P = dist b:Bool[1/2].throw(b) . P; \n"
    "                                        \n"
    "init P;                                 \n"
    ;
  stochastic_specification spec;
  parse_lps(text, spec);
  BOOST_CHECK(is_stochastic(spec));

  text =
    "act  throw: Bool;         \n"
    "                          \n"
    "proc P = throw(true) . P; \n"
    "                          \n"
    "init dist b:Bool[1/2] . P;\n"
    ;
  parse_lps(text, spec);
  BOOST_CHECK(is_stochastic(spec));
}

int test_main(int argc, char* argv[])
{
  test_empty_distribution();
  test_remove_stochastic_operators();
  test_is_stochastic();

  return 0;
}
