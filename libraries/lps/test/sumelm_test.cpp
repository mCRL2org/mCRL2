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
#include <boost/test/included/unit_test_framework.hpp>
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/sumelm.h"
#include "mcrl2/lps/linearise.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/utilities/test_utilities.h"

using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::lps;

using mcrl2::utilities::collect_after_test_case;
BOOST_GLOBAL_FIXTURE(collect_after_test_case)

void print_specifications(const specification& s0, const specification& s1)
{
  if (!(s0 == s1))
  {
    std::clog << "=== Specifications differ ===" << std::endl;
    std::clog << "    Input specification  : " << lps::pp(s0) << std::endl
              << "    Output specification : " << lps::pp(s1) << std::endl;
  }
  else
  {
    std::clog << "=== Specification are the same ===" << std::endl;
  }
}

/*
 * Test case which tries to test all possibilities for substitutions. This is a
 * test for issue #367
 */
BOOST_AUTO_TEST_CASE(bug_367)
{
  std::clog << "Test case 1" << std::endl;
  const std::string text(
    "sort S = struct s1 | s2;\n"
    "map f : S -> Bool;\n"
    "act a : S # Bool;\n"
    "proc P = sum c : S, b : Bool . (b == f(c) && c == s2) -> a(c, b) . P;\n"
    "init P;\n"
  );

  specification s0 = parse_linear_process_specification(text);
  specification s1 = s0;
  sumelm_algorithm(s1).run();
  const action_summand_vector& summands1 = s1.process().action_summands();
  for (action_summand_vector::const_iterator i = summands1.begin(); i != summands1.end(); ++i)
  {
    BOOST_CHECK(i->summation_variables().empty());
    BOOST_CHECK(data::find_all_variables(i->condition()).empty());
    BOOST_CHECK(lps::find_all_variables(i->multi_action()).empty());
  }

  print_specifications(s0,s1);
}

/// Sum variable y does not occur in the summand, and therefore must be removed.
BOOST_AUTO_TEST_CASE(no_occurrence_of_variable)
{
  std::clog << "Test case 2" << std::endl;
  const std::string text(
    "act a,b;\n"
    "proc P(s3_P: Pos) = sum y_P: Int. (s3_P == 1) -> a . P(2)\n"
    "                  + (s3_P == 2) -> b . P(1);\n"
    "init P(1);\n"
  );

  specification s0 = parse_linear_process_specification(text);
  specification s1 = s0;
  sumelm_algorithm(s1).run();
  const action_summand_vector& summands1 = s1.process().action_summands();
  for (action_summand_vector::const_iterator i = summands1.begin(); i != summands1.end(); ++i)
  {
    BOOST_CHECK(i->summation_variables().empty());
  }

  print_specifications(s0,s1);
}

/*
 * The sum variable y is equal to 4 in the condition, hence 4 should be
 * substituted for y in the summand, and sum y should be removed
 * In this case, this boils down to removing sum y, and removing 4==y from the
 * condition. The rest of the specification can be left untouched
 */
BOOST_AUTO_TEST_CASE(reverse_equality)
{
  std::clog << "Test case 3" << std::endl;
  const std::string text(
    "act a;\n"
    "proc P = sum y:Int . (4 == y) -> a . P;\n"
    "init P;\n"
  );

  specification s0 = parse_linear_process_specification(text);
  specification s1 = s0;
  sumelm_algorithm(s1).run();
  const action_summand_vector& summands1 = s1.process().action_summands();
  for (action_summand_vector::const_iterator i = summands1.begin(); i != summands1.end(); ++i)
  {
    BOOST_CHECK(i->summation_variables().empty());
    BOOST_CHECK(data::find_all_variables(i->condition()).empty());
  }

  print_specifications(s0,s1);
}

/*
 * This is the same as test case 3, except with the equality in different order.
 */
BOOST_AUTO_TEST_CASE(equality)
{
  std::clog << "Test case 4" << std::endl;
  const std::string text(
    "act a;\n"
    "proc P = sum y:Int . (y == 4) -> a . P;\n"
    "init P;\n"
  );

  specification s0 = parse_linear_process_specification(text);
  specification s1 = s0;
  sumelm_algorithm(s1).run();
  const action_summand_vector& summands1 = s1.process().action_summands();
  for (action_summand_vector::const_iterator i = summands1.begin(); i != summands1.end(); ++i)
  {
    BOOST_CHECK(i->summation_variables().empty());
    BOOST_CHECK(data::find_all_variables(i->condition()).empty());
  }

  print_specifications(s0,s1);
}

/*
 * Test whether sum variables are correctly removed from actions and timing.
 */
BOOST_AUTO_TEST_CASE(actions_and_time)
{
  std::clog << "Test case 5" << std::endl;
  const std::string text(
    "act a,b:Int;\n"
    "proc P = sum y:Int . (y == 4) -> a(y)@y . b(y*2)@(y+1) . P;\n"
    "init P;\n"
  );

  // FIXME, this test case requires the parser to allow parsing of free
  // variables.
  specification s0 = linearise(text);
  specification s1 = s0;
  sumelm_algorithm(s1).run();
  std::set<variable> parameters = mcrl2::data::find_all_variables(s1.process().process_parameters());
  const action_summand_vector& summands1 = s1.process().action_summands();
  for (action_summand_vector::const_iterator i = summands1.begin(); i != summands1.end(); ++i)
  {
    BOOST_CHECK(i->summation_variables().empty());

    // Check that the only data variables in the condition and time
    // are process parameters
    std::set<variable> condition_vars = data::find_all_variables(i->condition());
    for (std::set<variable>::iterator j = condition_vars.begin()
                                          ; j != condition_vars.end()
         ; ++j)
    {
      BOOST_CHECK(parameters.find(*j) != parameters.end());
    }

    if (i->has_time())
    {
      std::set<variable> time_vars = data::find_all_variables(i->multi_action().time());
      for (std::set<variable>::iterator j = time_vars.begin()
                                            ; j != time_vars.end()
           ; ++j)
      {
        BOOST_CHECK(parameters.find(*j) != parameters.end());
      }
    }
  }

  print_specifications(s0,s1);
}

/*
 * Test that a sum variable is not removed when it occurs in both sides of a
 * variable.
 */
BOOST_AUTO_TEST_CASE(both_sides_of_equality)
{
  std::clog << "Test case 6" << std::endl;
  const std::string text(
    "act a;\n"
    "proc P = sum y:Int . (y == y + 1) -> a . P;\n"
    "init P;\n"
  );

  specification s0 = parse_linear_process_specification(text);
  specification s1 = s0;
  sumelm_algorithm(s1).run();
  int sumvar_count = 0;
  const action_summand_vector& summands1 = s1.process().action_summands();
  for (action_summand_vector::const_iterator i = summands1.begin(); i != summands1.end(); ++i)
  {
    if (!i->summation_variables().empty())
    {
      ++sumvar_count;
    }
  }
  BOOST_CHECK(sumvar_count == 1);
  BOOST_CHECK(s0 == s1);

  print_specifications(s0,s1);
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
BOOST_AUTO_TEST_CASE(three_sums_remove_two)
{
  std::clog << "Test case 7" << std::endl;
  const std::string text(
    "sort D = struct d1 | d2 | d3;\n"
    "map g : D -> D;\n"
    "act a;\n"
    "proc P(c:D) = sum d:D . sum e:D . sum f:D . (d == e && e == g(e) && e == f) -> a . P(d);\n"
    "init P(d1);\n"
  );

  specification s0 = parse_linear_process_specification(text);
  specification s1 = s0;
  sumelm_algorithm(s1).run();
  int sumvar_count = 0;
  const action_summand_vector& summands1 = s1.process().action_summands();
  for (action_summand_vector::const_iterator i = summands1.begin(); i != summands1.end(); ++i)
  {
    if (!i->summation_variables().empty())
    {
      ++sumvar_count;
    }
  }
  BOOST_CHECK(sumvar_count == 1);

  print_specifications(s0,s1);
}

/*
 * Sum variable d occurs twice in the lefthandside of an equality
 * The strongest set of substitutions possible is e := d; f := d, hence removing
 * two summations.
 * (Test introduced after a bugreport by M. Voorhoeve)
 */
BOOST_AUTO_TEST_CASE(twice_lhs)
{
  std::clog << "Test case 8" << std::endl;
  const std::string text(
    "sort D = struct d1 | d2 | d3;\n"
    "act a;\n"
    "proc P(c:D) = sum d:D . sum e:D . sum f:D . (d == e && d == f) -> a . P(d);\n"
    "init P(d1);\n"
  );

  specification s0 = parse_linear_process_specification(text);
  specification s1 = s0;
  sumelm_algorithm(s1).run();
  int sumvar_count = 0;
  const action_summand_vector& summands1 = s1.process().action_summands();
  for (action_summand_vector::const_iterator i = summands1.begin(); i != summands1.end(); ++i)
  {
    if (!i->summation_variables().empty())
    {
      ++sumvar_count;
      BOOST_CHECK(data::find_all_variables(i->condition()).empty());
    }
  }
  BOOST_CHECK(sumvar_count == 1);

  print_specifications(s0,s1);
}

/*
 * Test introduced after a bug report by Bas Ploeger. In contrary to what was
 * assumed before, sum variables may not be unconditionally removed from delta summands.
 */
BOOST_AUTO_TEST_CASE(no_unconditional_sum_removal_from_delta)
{
  std::clog << "Test case 9" << std::endl;
  const std::string text(
    "proc P = sum y:Nat . (y > 10) -> delta;\n"
    "init P;\n"
  );

  specification s0 = parse_linear_process_specification(text);
  specification s1 = s0;
  sumelm_algorithm(s1).run();
  int sumvar_count = 0;
  const deadlock_summand_vector& summands1 = s1.process().deadlock_summands();
  for (deadlock_summand_vector::const_iterator i = summands1.begin(); i != summands1.end(); ++i)
  {
    if (!i->summation_variables().empty())
    {
      ++sumvar_count;
    }
  }
  BOOST_CHECK(sumvar_count == 1);

  print_specifications(s0,s1);
}

///Test case for issue #380
BOOST_AUTO_TEST_CASE(bug_380)
{
  std::clog << "Test case 10" << std::endl;
  const std::string text(
    "act a:Nat;\n"
    "proc P(n0: Nat) = sum n: Nat. (n == n0 && n == 1) -> a(n0) . P(n);\n"
    "init P(0);\n"
  );

  specification s0 = parse_linear_process_specification(text);
  specification s1 = s0;
  sumelm_algorithm(s1).run();
  int sumvar_count = 0;
  const action_summand_vector& summands1 = s1.process().action_summands();
  for (action_summand_vector::const_iterator i = summands1.begin(); i != summands1.end(); ++i)
  {
    BOOST_CHECK(i->condition() != sort_bool::true_());
    if (!i->summation_variables().empty())
    {
      ++sumvar_count;
    }
  }
  BOOST_CHECK(sumvar_count == 0);

  print_specifications(s0, s1);
}

///Test case for issue #380
BOOST_AUTO_TEST_CASE(test_boolean_variables)
{
  std::clog << "Test case 11" << std::endl;
  const std::string text(
    "act a:Bool#Bool;\n"
    "proc P = sum b,c: Bool. (b && !c) -> a(b,c) . P;\n"
    "init P;\n"
  );

  specification s0 = parse_linear_process_specification(text);
  specification s1 = s0;
  sumelm_algorithm(s1).run();

  action_summand_vector v(s1.process().action_summands());
  for(action_summand_vector::const_iterator i = v.begin(); i != v.end(); ++i)
  {
    BOOST_CHECK_EQUAL(i->summation_variables().size(), 0u);
  }

  print_specifications(s0, s1);
}

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
  return 0;
}

