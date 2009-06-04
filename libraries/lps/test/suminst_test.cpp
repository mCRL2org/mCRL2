// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file suminst_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <string>
#include <boost/test/minimal.hpp>
#include <mcrl2/lps/specification.h>
#include <mcrl2/lps/suminst.h>
#include <mcrl2/lps/mcrl22lps.h>
#include "mcrl2/core/garbage_collection.h"

using namespace atermpp;
using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::lps;

///sum d:D should be unfolded
void test_case_1(const t_suminst_options& opts)
{
  const std::string text(
    "sort D = struct d1|d2;\n"
    "act a;\n"
    "proc X = sum d:D . a . X;\n"
    "init X;\n"
  );

  specification s0 = mcrl22lps(text);
  rewriter r(s0.data(), opts.strategy);
  specification s1 = instantiate_sums(s0, r, opts);
std::clog << pp(s0) << std::endl;
std::clog << pp(s1) << std::endl;
  summand_list summands1 = s1.process().summands();
  for(summand_list::iterator i = summands1.begin(); i != summands1.end(); ++i)
  {
    BOOST_CHECK(i->summation_variables().empty());
  }

  // TODO: Check that d1 and d2 actually occur in the process.
}

///sum d:D should be unfolded (multiple occurrences of d per summand)
void test_case_2(const t_suminst_options& opts)
{
  const std::string text(
    "sort D = struct d1|d2;\n"
    "act a:D;\n"
    "    b;\n"
    "proc X(x:D) = sum d:D . a(d) . b . X(d);\n"
    "init X(d1);\n"
  );

  specification s0 = mcrl22lps(text);
  rewriter r(s0.data(), opts.strategy);
  specification s1 = instantiate_sums(s0, r, opts);
  summand_list summands1 = s1.process().summands();
  for(summand_list::iterator i = summands1.begin(); i != summands1.end(); ++i)
  {
    BOOST_CHECK(i->summation_variables().empty());
  }

  // TODO: Check that d1 and d2 actually occur in the process.
}

///sum d:D should not be removed, hence there should be a summand for
///which d is a sum variable.
void test_case_3(const t_suminst_options& opts)
{
  const std::string text(
    "sort D;\n"
    "act a:D;\n"
    "proc X = sum d:D . a(d) . X;\n"
    "init X;\n"
  );

  specification s0 = mcrl22lps(text);
  rewriter r(s0.data(), opts.strategy);
  specification s1 = instantiate_sums(s0, r, opts);
  summand_list summands1 = s1.process().summands();
  bool sum_occurs = false;
  for(summand_list::iterator i = summands1.begin(); i != summands1.end(); ++i)
  {
    sum_occurs = sum_occurs || !i->summation_variables().empty();
  }
  BOOST_CHECK(sum_occurs);
}

///This is a test in which tau summands occur.
///We override opts such that only tau summands are instantiated.
///Note: Test case 5 tests the same specification, but uses the defaults.
void test_case_4(const t_suminst_options& opts)
{
  const std::string text(
    "sort S = struct s1 | s2 | s3;\n"
    "     T = struct t1 | t2 | t3;\n"
    "act a;\n"
    "proc P = sum s : S . tau . P\n"
    "       + sum t : T . a . P;\n"
    "init P;\n"
  );

  t_suminst_options new_opts = opts;
  new_opts.tau_only = true;

  specification s0 = mcrl22lps(text);
  rewriter r(s0.data(), opts.strategy);
  specification s1 = instantiate_sums(s0, r, new_opts);
  summand_list summands1 = s1.process().summands();
  bool tau_sum_occurs = false;
  bool sum_occurs = false;
  for(summand_list::iterator i = summands1.begin(); i != summands1.end(); ++i)
  {
    if(i->is_tau())
    {
      tau_sum_occurs = tau_sum_occurs || !i->summation_variables().empty();
    }
    else
    {
      sum_occurs = sum_occurs || !i->summation_variables().empty();
    }
  }
  BOOST_CHECK(!tau_sum_occurs);
  BOOST_CHECK(sum_occurs);
}

///This is a test in which tau summands occur.
///Both sum variables should be expanded, hence no sum variable may occur in the
///result.
///Note: Test case 4 tests the same specification, but only expands the tau
///summands.
void test_case_5(const t_suminst_options& opts)
{
  const std::string text(
    "sort S = struct s1 | s2 | s3;\n"
    "     T = struct t1 | t2 | t3;\n"
    "act a;\n"
    "proc P = sum s : S . tau . P\n"
    "       + sum t : T . a . P;\n"
    "init P;\n"
  );

  specification s0 = mcrl22lps(text);
  rewriter r(s0.data(), opts.strategy);
  specification s1 = instantiate_sums(s0, r, opts);
  summand_list summands1 = s1.process().summands();
  bool tau_sum_occurs = false;
  bool sum_occurs = false;
  for(summand_list::iterator i = summands1.begin(); i != summands1.end(); ++i)
  {
    if(i->is_tau())
    {
      tau_sum_occurs = tau_sum_occurs || !i->summation_variables().empty();
    }
    else
    {
      sum_occurs = sum_occurs || !i->summation_variables().empty();
    }
  }
  BOOST_CHECK(!tau_sum_occurs);
  BOOST_CHECK(!sum_occurs);
}

int test_main(int ac, char** av)
{
  MCRL2_ATERMPP_INIT(ac, av)

  t_suminst_options opts;

  test_case_1(opts);
  core::garbage_collect();
  test_case_2(opts);
  core::garbage_collect();
  test_case_3(opts);
  core::garbage_collect();
  test_case_4(opts);
  core::garbage_collect();
  test_case_5(opts);
  core::garbage_collect();

  return 0;
}

