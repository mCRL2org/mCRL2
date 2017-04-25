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

#include <boost/test/minimal.hpp>
#include <iostream>
#include <mcrl2/lps/linearise.h>
#include <mcrl2/lps/suminst.h>
#include <string>

using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::lps;

///sum d:D should be unfolded
void test_case_1()
{
  const std::string text(
    "sort D = struct d1|d2;\n"
    "act a;\n"
    "proc X = sum d:D . a . X;\n"
    "init X;\n"
  );

  specification s0=remove_stochastic_operators(linearise(text));
  rewriter r(s0.data());
  specification s1(s0);
  suminst_algorithm<rewriter, specification>(s1,r).run();
  std::clog << lps::pp(s0) << std::endl;
  std::clog << lps::pp(s1) << std::endl;
  const action_summand_vector& summands1 = s1.process().action_summands();
  for (action_summand_vector::const_iterator i = summands1.begin(); i != summands1.end(); ++i)
  {
    BOOST_CHECK(i->summation_variables().empty());
  }

  // TODO: Check that d1 and d2 actually occur in the process.
}

///sum d:D should be unfolded (multiple occurrences of d per summand)
void test_case_2()
{
  const std::string text(
    "sort D = struct d1|d2;\n"
    "act a:D;\n"
    "    b;\n"
    "proc X(x:D) = sum d:D . a(d) . b . X(d);\n"
    "init X(d1);\n"
  );

  specification s0=remove_stochastic_operators(linearise(text));
  rewriter r(s0.data());
  specification s1(s0);
  suminst_algorithm<rewriter, specification>(s1, r).run();
  const action_summand_vector& summands1 = s1.process().action_summands();
  for (action_summand_vector::const_iterator i = summands1.begin(); i != summands1.end(); ++i)
  {
    BOOST_CHECK(i->summation_variables().empty());
  }

  // TODO: Check that d1 and d2 actually occur in the process.
}

///sum d:D should not be removed, hence there should be a summand for
///which d is a sum variable.
void test_case_3()
{
  const std::string text(
    "sort D;\n"
    "act a:D;\n"
    "proc X = sum d:D . a(d) . X;\n"
    "init X;\n"
  );

  specification s0=remove_stochastic_operators(linearise(text));
  rewriter r(s0.data());
  specification s1(s0);
  suminst_algorithm<rewriter, specification>(s1, r).run();
  bool sum_occurs = false;
  const action_summand_vector& summands1 = s1.process().action_summands();
  for (const auto & i : summands1)
  {
    sum_occurs = sum_occurs || !i.summation_variables().empty();
  }
  BOOST_CHECK(sum_occurs);
}

///This is a test in which tau summands occur.
///We override opts such that only tau summands are instantiated.
///Note: Test case 5 tests the same specification, but uses the defaults.
void test_case_4()
{
  const std::string text(
    "sort S = struct s1 | s2 | s3;\n"
    "     T = struct t1 | t2 | t3;\n"
    "act a;\n"
    "proc P = sum s : S . tau . P\n"
    "       + sum t : T . a . P;\n"
    "init P;\n"
  );

  specification s0=remove_stochastic_operators(linearise(text));
  rewriter r(s0.data());
  specification s1(s0);
  suminst_algorithm<rewriter, specification>(s1, r, finite_sorts(s1.data()), true).run();
  bool tau_sum_occurs = false;
  bool sum_occurs = false;
  const action_summand_vector& summands1 = s1.process().action_summands();
  for (const auto & i : summands1)
  {
    if (i.is_tau())
    {
      tau_sum_occurs = tau_sum_occurs || !i.summation_variables().empty();
    }
    else
    {
      sum_occurs = sum_occurs || !i.summation_variables().empty();
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
void test_case_5()
{
  const std::string text(
    "sort S = struct s1 | s2 | s3;\n"
    "     T = struct t1 | t2 | t3;\n"
    "act a;\n"
    "proc P = sum s : S . tau . P\n"
    "       + sum t : T . a . P;\n"
    "init P;\n"
  );

  specification s0=remove_stochastic_operators(linearise(text));
  rewriter r(s0.data());
  specification s1(s0);
  suminst_algorithm<rewriter, specification>(s1, r).run();
  bool tau_sum_occurs = false;
  bool sum_occurs = false;
  const action_summand_vector& summands1 = s1.process().action_summands();
  for (const auto & i : summands1)
  {
    if (i.is_tau())
    {
      tau_sum_occurs = tau_sum_occurs || !i.summation_variables().empty();
    }
    else
    {
      sum_occurs = sum_occurs || !i.summation_variables().empty();
    }
  }
  BOOST_CHECK(!tau_sum_occurs);
  BOOST_CHECK(!sum_occurs);
}

void test_case_6()
{
  const std::string text(
    "proc P(n0:Nat) = sum n : Nat . (n == n0) -> delta@n . P(n);\n"
    "init P(5);\n"
  );

  specification s0=remove_stochastic_operators(linearise(text));
  rewriter r(s0.data());
  specification s1(s0);
  suminst_algorithm<rewriter, specification>(s1, r, std::set<data::sort_expression>(s1.data().sorts().begin(),s1.data().sorts().end())).run();
  const action_summand_vector& summands1 = s1.process().action_summands();
  for (action_summand_vector::const_iterator i = summands1.begin(); i != summands1.end(); ++i)
  {
    BOOST_CHECK(i->summation_variables().empty());
  }
}

void test_case_7()
{
  const std::string text(
    "sort S;\n"
    "act a:S;\n"
    "proc P = sum s : S . a(s) . P;\n"
    "init P;\n"
  );

  specification s0=remove_stochastic_operators(linearise(text));
  rewriter r(s0.data());
  specification s1(s0);
  suminst_algorithm<rewriter, specification>(s1, r, std::set<data::sort_expression>(s1.data().sorts().begin(),s1.data().sorts().end())).run();
  int sum_count = 0;
  const action_summand_vector& summands1 = s1.process().action_summands();
  for (const auto & i : summands1)
  {
    sum_count += i.summation_variables().size();
  }
  BOOST_CHECK(sum_count == 1);
}

int test_main(int ac, char** av)
{
  std::clog << "test case 1" << std::endl;
  test_case_1();
  std::clog << "test case 2" << std::endl;
  test_case_2();
  std::clog << "test case 3" << std::endl;
  test_case_3();
  std::clog << "test case 4" << std::endl;
  test_case_4();
  std::clog << "test case 5" << std::endl;
  test_case_5();
  std::clog << "test case 6" << std::endl;
  test_case_6();

  return 0;
}

