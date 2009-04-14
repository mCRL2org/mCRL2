// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file sumelm_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <string>
#include <boost/test/minimal.hpp>
#include <mcrl2/lps/specification.h>
#include <mcrl2/lps/sumelm.h>
#include <mcrl2/lps/mcrl22lps.h>

using namespace atermpp;
using namespace mcrl2::data;
using namespace mcrl2::lps;

/*
 * Test case which tries to test all possibilities for substitutions. This is a
 * test for issue #367
 */
void test_case_1()
{
  const std::string text(
    "sort S = struct s1 | s2;\n"
    "map f : S -> Bool;\n"
    "act a : S # Bool;\n"
    "proc P = sum c : S, b : Bool . (b == f(c) && c == s2) -> a(c, b) . P;\n"
    "init P;\n"
  );

  specification s0 = mcrl22lps(text);
  specification s1 = sumelm(s0);
  summand_list summands1 = s1.process().summands();
  for(summand_list::iterator i = summands1.begin(); i != summands1.end(); ++i)
  {
    BOOST_CHECK(i->summation_variables().empty());
    BOOST_CHECK(find_all_data_variables(i->condition()).empty());
    BOOST_CHECK(find_all_data_variables(i->actions()).empty());
  }
}

/// Sum variable y does not occur in the summand, and therefore must be removed.
void test_case_2()
{
  const std::string text(
    "act a,b;\n"
    "proc P = sum y:Int . a . b . P;\n"
    "init P;\n"
  );

  specification s0 = mcrl22lps(text);
  specification s1 = sumelm(s0);
  summand_list summands1 = s1.process().summands();
  for(summand_list::iterator i = summands1.begin(); i != summands1.end(); ++i)
  {
    BOOST_CHECK(i->summation_variables().empty());
  }
}

/*
 * The sum variable y is equal to 4 in the condition, hence 4 should be
 * substituted for y in the summand, and sum y should be removed
 * In this case, this boils down to removing sum y, and removing 4==y from the
 * condition. The rest of the specification can be left untouched
 */
void test_case_3()
{
  const std::string text(
    "act a;\n"
    "proc P = sum y:Int . (4 == y) -> a . P;\n"
    "init P;\n"
  );

  specification s0 = mcrl22lps(text);
  specification s1 = sumelm(s0);
  summand_list summands1 = s1.process().summands();
  for(summand_list::iterator i = summands1.begin(); i != summands1.end(); ++i)
  {
    BOOST_CHECK(i->summation_variables().empty());
    BOOST_CHECK(find_all_data_variables(i->condition()).empty());
  }
}

/*
 * This is the same as test case 3, except with the equality in different order.
 */
void test_case_4()
{
  const std::string text(
    "act a;\n"
    "proc P = sum y:Int . (y == 4) -> a . P;\n"
    "init P;\n"
  );

  specification s0 = mcrl22lps(text);
  specification s1 = sumelm(s0);
  summand_list summands1 = s1.process().summands();
  for(summand_list::iterator i = summands1.begin(); i != summands1.end(); ++i)
  {
    BOOST_CHECK(i->summation_variables().empty());
    BOOST_CHECK(find_all_data_variables(i->condition()).empty());
  }
}

/*
 * Test whether sum variables are correctly removed from actions and timing.
 */
void test_case_5()
{
  const std::string text(
    "act a,b:Int;\n"
    "proc P = sum y:Int . (y == 4) -> a(y)@y . b(y*2)@(y+1) . P;\n"
    "init P;\n"
  );

  specification s0 = mcrl22lps(text);
  specification s1 = sumelm(s0);
  summand_list summands1 = s1.process().summands();
  std::set<data_variable> parameters = find_all_data_variables(s1.process().process_parameters());
  for(summand_list::iterator i = summands1.begin(); i != summands1.end(); ++i)
  {
    BOOST_CHECK(i->summation_variables().empty());

    // Check that the only data variables in the condition and time
    // are process parameters
    std::set<data_variable> condition_vars = find_all_data_variables(i->condition());
    for (std::set<data_variable>::iterator j = condition_vars.begin()
        ; j != condition_vars.end()
        ; ++j)
    {
      BOOST_CHECK(parameters.find(*j) != parameters.end());
    }

    if (i->has_time())
    {
      std::set<data_variable> time_vars = find_all_data_variables(i->time());
      for (std::set<data_variable>::iterator j = time_vars.begin()
          ; j != time_vars.end()
          ; ++j)
      {
        BOOST_CHECK(parameters.find(*j) != parameters.end());
      }
    }
  }
}

/*
 * Test that a sum variable is not removed when it occurs in both sides of a
 * variable.
 */
void test_case_6()
{
  const std::string text(
    "act a;\n"
    "proc P = sum y:Int . (y == y + 1) -> a . P;\n"
    "init P;\n"
  );

  specification s0 = mcrl22lps(text);
  specification s1 = sumelm(s0);
  summand_list summands1 = s1.process().summands();
  int sumvar_count = 0;
  for(summand_list::iterator i = summands1.begin(); i != summands1.end(); ++i)
  {
    if (!i->summation_variables().empty())
    {
      ++sumvar_count;
    }
  }
  BOOST_CHECK(sumvar_count == 1);
  BOOST_CHECK(s0 == s1);
}

/*
 * The sum variable e occurs in the condition in the following ways:
 * - As righthandside of an equality
 * - Both as lefthandside and righthandside of an equality
 * - As lefthandside of an equality
 * The sum variable d occurs in the lefthandside of an equality in the
 * condition.
 * The sum variable f occurs in the righthandside of an equality in the
 * condition.
 *
 * Result:
 * - The summation over d is removed by substituting e for d
 * - The summation over e is removed by substituting f for e
 * This should leave only a sum f:D . f == g(f) -> a . X(f).
 */
void test_case_7()
{
  const std::string text(
    "sort D = struct d1 | d2 | d3;\n"
    "map g : D -> D;\n"
    "act a;\n"
    "proc P(c:D) = sum d:D . sum e:D . sum f:D . (d == e && e == g(e) && e == f) -> a . P(d);\n"
    "init P(d1);\n"
  );

  specification s0 = mcrl22lps(text);
  specification s1 = sumelm(s0);
  summand_list summands1 = s1.process().summands();
  int sumvar_count = 0;
  for(summand_list::iterator i = summands1.begin(); i != summands1.end(); ++i)
  {
    if (!i->summation_variables().empty())
    {
      ++sumvar_count;
    }
  }
  BOOST_CHECK(sumvar_count == 1);
}

/*
 * Sum variable d occurs twice in the lefthandside of an equality
 * The strongest set of substitutions possible is e := d; f := d, hence removing
 * two summations.
 * (Test introduced after a bugreport by M. Voorhoeve)
 */
void test_case_8()
{
  const std::string text(
    "sort D = struct d1 | d2 | d3;\n"
    "act a;\n"
    "proc P(c:D) = sum d:D . sum e:D . sum f:D . (d == e && d == f) -> a . P(d);\n"
    "init P(d1);\n"
  );

  specification s0 = mcrl22lps(text);
  specification s1 = sumelm(s0);
  summand_list summands1 = s1.process().summands();
  int sumvar_count = 0;
  for(summand_list::iterator i = summands1.begin(); i != summands1.end(); ++i)
  {
    if (!i->summation_variables().empty())
    {
      ++sumvar_count;
      BOOST_CHECK(find_all_data_variables(i->condition()).empty());
    }
  }
  BOOST_CHECK(sumvar_count == 1);
}

/*
 * Test introduced after a bug report by Bas Ploeger. In contrary to what was
 * assumed before, sum variables may not be removed from delta summands.
 */
void test_case_9()
{
  const std::string text(
    "proc P = sum y:Bool . y -> delta . P;\n"
    "init P;\n"
  );

  specification s0 = mcrl22lps(text);
  specification s1 = sumelm(s0);
  summand_list summands1 = s1.process().summands();
  int sumvar_count = 0;
  for(summand_list::iterator i = summands1.begin(); i != summands1.end(); ++i)
  {
    if (!i->summation_variables().empty())
    {
      ++sumvar_count;
    }
  }
  BOOST_CHECK(sumvar_count == 1);
}

///Test case for issue #380
void test_case_10()
{
  const std::string text(
  "act a:Nat;\n"
  "proc P(n0: Nat) = sum n: Nat. (n == n0 && n == 1) -> a(n0) . P(n);\n"
  "init P(0);\n"
  );

  specification s0 = mcrl22lps(text);
  specification s1 = sumelm(s0);
  summand_list summands1 = s1.process().summands();
  int sumvar_count = 0;
  for(summand_list::iterator i = summands1.begin(); i != summands1.end(); ++i)
  {
    BOOST_CHECK(i->condition() != data_expr::true_());
    if (!i->summation_variables().empty())
    {
      ++sumvar_count;
    }
  }
  BOOST_CHECK(sumvar_count == 0);
}

int test_main(int ac, char** av)
{
  MCRL2_ATERM_INIT(ac, av)

  test_case_1();
  test_case_2();
  test_case_3();
  test_case_4();
  test_case_5();
  test_case_6();
  test_case_7();
  test_case_8();
  test_case_9();
  test_case_10();

  return 0;
}

