// Author(s): Jeroen Keiren
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file decluster_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <string>
#include <boost/test/minimal.hpp>
#include <mcrl2/lps/specification.h>
#include <mcrl2/lps/decluster.h>
#include <mcrl2/lps/mcrl22lps.h>

using namespace atermpp;
using namespace mcrl2::data;
using namespace mcrl2::lps;

///sum d:D should be unfolded
void test_case_1(const t_decluster_options& opts)
{
  const std::string text(
    "sort D = struct d1|d2;\n"
    "act a;\n"
    "proc X = sum d:D . a . X;\n"
    "init X;\n"
  );

  specification s0 = mcrl22lps(text);
  Rewriter* r = createRewriter(s0.data(), opts.strategy);
  specification s1 = decluster(s0, *r, opts);
  summand_list summands1 = s1.process().summands();
  for(summand_list::iterator i = summands1.begin(); i != summands1.end(); ++i)
  {
    BOOST_CHECK(i->summation_variables().empty());
  }

  // TODO: Check that d1 and d2 actually occur in the process.
}

///sum d:D should be unfolded (multiple occurrences of d per summand)
void test_case_2(const t_decluster_options& opts)
{
  const std::string text(
    "sort D = struct d1|d2;\n"
    "act a:D;\n"
    "    b;\n"
    "proc X(x:D) = sum d:D . a(d) . b . X(d);\n"
    "init X(d1);\n"
  );

  specification s0 = mcrl22lps(text);
  Rewriter* r = createRewriter(s0.data(), opts.strategy);
  specification s1 = decluster(s0, *r, opts);
  summand_list summands1 = s1.process().summands();
  for(summand_list::iterator i = summands1.begin(); i != summands1.end(); ++i)
  {
    BOOST_CHECK(i->summation_variables().empty());
  }

  // TODO: Check that d1 and d2 actually occur in the process.
}

///sum d:D should not be removed, hence there should be a summand for
///which d is a sum variable.
void test_case_3(const t_decluster_options& opts)
{
  const std::string text(
    "sort D;\n"
    "act a:D;\n"
    "proc X = sum d:D . a(d) . X;\n"
    "init X;\n"
  );

  specification s0 = mcrl22lps(text);
  Rewriter* r = createRewriter(s0.data(), opts.strategy);
  specification s1 = decluster(s0, *r, opts);
  summand_list summands1 = s1.process().summands();
  bool sum_occurs = false;
  for(summand_list::iterator i = summands1.begin(); i != summands1.end(); ++i)
  {
    sum_occurs = sum_occurs || !i->summation_variables().empty();
  }
  BOOST_CHECK(sum_occurs);
}


int test_main(int ac, char** av)
{
  MCRL2_ATERM_INIT(ac, av)

  t_decluster_options opts;

  test_case_1(opts);
  test_case_2(opts);
  test_case_3(opts);

  return 0;
}

