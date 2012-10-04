// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file binary_test.cpp
/// \brief Some simple tests for the binary algorithm.

#include <iostream>
#include <string>
#include <boost/test/included/unit_test_framework.hpp>

#include "mcrl2/utilities/test_utilities.h"

#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/binary.h"
#include "mcrl2/lps/linearise.h"
#include "mcrl2/lps/detail/test_input.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::lps;
using mcrl2::utilities::collect_after_test_case;

BOOST_GLOBAL_FIXTURE(collect_after_test_case)


///All process parameters of sort D should have been translated to
///parameters of sort Bool. This leaves only parameters of sort Bool and Pos.
BOOST_AUTO_TEST_CASE(case_1)
{
  const std::string text(
    "sort D = struct d1|d2;\n"
    "act a;\n"
    "proc P(e:D) = sum d:D . a . P(d);\n"
    "init P(d1);\n"
  );

  specification s0 = linearise(text);
  rewriter r(s0.data());
  specification s1 = s0;
  binary_algorithm<rewriter>(s1, r).run();

  variable_list parameters1 = s1.process().process_parameters();

  int bool_param_count = 0;
  for (variable_list::iterator i = parameters1.begin(); i != parameters1.end(); ++i)
  {
    BOOST_CHECK_EQUAL(i->sort(), sort_bool::bool_());
    if (i->sort() == sort_bool::bool_())
    {
      ++bool_param_count;
    }
  }
  BOOST_CHECK_EQUAL(bool_param_count, 1);
}

/*
 * Sort D has got 8 constructors, which can therefore be encoded exactly in
 * a vector of 3 boolean variables. This means that all combinations of the
 * boolean variables encode a constructor.
 * Process parameter e should be replaced by a vector of boolean variables,
 * and should be replaced in nextstate.
 * The initial state should be altered accordingly.
 */
BOOST_AUTO_TEST_CASE(case_2)
{
  const std::string text(
    "sort D = struct d1|d2|d3|d4|d5|d6|d7|d8;\n"
    "act a;\n"
    "proc P(e:D) = sum d:D . a . P(d);\n"
    "init P(d1);\n"
  );

  specification s0 = linearise(text);
  rewriter r(s0.data());
  specification s1 = s0;
  binary_algorithm<rewriter>(s1, r).run();

  int bool_param_count = 0;
  for (variable_list::iterator i = s1.process().process_parameters().begin();
       i != s1.process().process_parameters().end();
       ++i)
  {
    BOOST_CHECK_EQUAL(i->sort(), sort_bool::bool_());
    if (i->sort() == sort_bool::bool_())
    {
      ++bool_param_count;
    }
  }
  BOOST_CHECK_EQUAL(bool_param_count, 3);
}

/*
 * Sort D has got 7 constructors, which can therefore be encoded in
 * a vector of 3 boolean variables. This means that there is one combination
 * of the boolean variables that does not encode a constructor.
 * Process parameter e should be replaced by a vector of boolean variables,
 * and should be replaced in nextstate.
 * The initial state should be altered accordingly.
 */
BOOST_AUTO_TEST_CASE(case_3)
{
  const std::string text(
    "sort D = struct d1|d2|d3|d4|d5|d6|d7;\n"
    "act a;\n"
    "proc P(e:D) = sum d:D . a . P(d);\n"
    "init P(d1);\n"
  );

  specification s0 = linearise(text);
  rewriter r(s0.data());
  specification s1 = s0;
  binary_algorithm<rewriter>(s1, r).run();

  int bool_param_count = 0;
  for (variable_list::iterator i = s1.process().process_parameters().begin();
       i != s1.process().process_parameters().end();
       ++i)
  {
    BOOST_CHECK_EQUAL(i->sort(), sort_bool::bool_());
    if (i->sort() == sort_bool::bool_())
    {
      ++bool_param_count;
    }
  }
  BOOST_CHECK_EQUAL(bool_param_count, 3);
}

/*
 * Sort D has got 2 constructors, and should be encoded into one boolean
 * variable. Process parameter e should be replaced by a single boolean variable
 * and the action a(e) and the initial state should be altered accordingly.
 *
 * Note there is parameter of sort Pos because of linearisation.
 */
BOOST_AUTO_TEST_CASE(case_4)
{
  const std::string text(
    "sort D = struct d1|d2;\n"
    "act a,b:D;\n"
    "proc P(e:D) = sum d:D . a(e) . b(d) . P(d);\n"
    "init P(d1);\n"
  );

  specification s0 = linearise(text);
  rewriter r(s0.data());
  specification s1 = s0;
  binary_algorithm<rewriter>(s1, r).run();

  int bool_param_count = 0;
  for (variable_list::iterator i = s1.process().process_parameters().begin();
       i != s1.process().process_parameters().end();
       ++i)
  {
    BOOST_CHECK(i->sort() == sort_pos::pos() || i->sort() == sort_bool::bool_());
    if (i->sort() == sort_bool::bool_())
    {
      ++bool_param_count;
    }
  }
  BOOST_CHECK_EQUAL(bool_param_count, 2);
}

/*
 * Sort D has got 9 constructors, therefore it should be encoded in
 * a vector of 4 boolean variables. Note that this does leave a lot of
 * combinations of booleans that are not used (7 to be precise).
 * Process parameter e should be replaced by a vector of boolean variables,
 * and should be replaced in nextstate.
 * The initial state should be altered accordingly.
 */
BOOST_AUTO_TEST_CASE(case_5)
{
  const std::string text(
    "sort D = struct d1|d2|d3|d4|d5|d6|d7|d8|d9;\n"
    "act a;\n"
    "proc P(e:D) = sum d:D . a . P(d);\n"
    "init P(d1);\n"
  );

  specification s0 = linearise(text);
  rewriter r(s0.data());
  specification s1 = s0;
  binary_algorithm<rewriter>(s1, r).run();

  int bool_param_count = 0;
  for (variable_list::iterator i = s1.process().process_parameters().begin();
       i != s1.process().process_parameters().end();
       ++i)
  {
    BOOST_CHECK_EQUAL(i->sort(), sort_bool::bool_());
    if (i->sort() == sort_bool::bool_())
    {
      ++bool_param_count;
    }
  }
  BOOST_CHECK_EQUAL(bool_param_count, 4);
}

/*
 * Sort D is a sort with 4 constructors. It is a recursive construct of two
 * constructors parameterized with sort E, which is in turn a sort which has
 * got two simple constructors.
 * Sort D can be coded in a vector of 2 boolean variables, in which all
 * combinations are used.
 * Process parameter e should be replaced by a vector of boolean variables,
 * and should be replaced in nextstate.
 * The initial state should be altered accordingly.
 */
BOOST_AUTO_TEST_CASE(case_6)
{
  const std::string text(
    "sort D = struct d1(E) | d2(E);\n"
    "     E = struct e1 | e2;\n"
    "act a;\n"
    "proc P(e:D) = sum d:D . a . P(d);\n"
    "init P(d1(e1));\n"
  );

  specification s0 = linearise(text);
  rewriter r(s0.data());
  specification s1 = s0;
  binary_algorithm<rewriter>(s1, r).run();

  int bool_param_count = 0;
  for (variable_list::iterator i = s1.process().process_parameters().begin();
       i != s1.process().process_parameters().end();
       ++i)
  {
    BOOST_CHECK_EQUAL(i->sort(), sort_bool::bool_());
    if (i->sort() == sort_bool::bool_())
    {
      ++bool_param_count;
    }
  }
  BOOST_CHECK_EQUAL(bool_param_count, 2);
}

// This test case shows a bug where apparently d1 and d2 are mapped to the
// same boolean value. Test case was provided by Jan Friso Groote along with
// bug 623.
BOOST_AUTO_TEST_CASE(bug_623)
{
  const std::string text(
    "sort D;\n"
    "cons d1,d2:D;\n"
    "act a:D#D;\n"
    "proc X(e1,e2:D) = a(e1,e2) . X(d1,d2);\n"
    "init X(d2,d1);\n"
  );

  specification s0 = linearise(text);
  rewriter r(s0.data());
  specification s1 = s0;
  binary_algorithm<rewriter>(s1, r).run();
  action_summand_vector summands1 = s1.process().action_summands();
  for (action_summand_vector::const_iterator i = summands1.begin(); i != summands1.end(); ++i)
  {
    data_expression_list next_state = i->next_state(s1.process().process_parameters());
    BOOST_CHECK_EQUAL(next_state.size(), 2);
    BOOST_CHECK_NE(*next_state.begin(), *(++next_state.begin()));
    std::clog << "erroneous next state " << data::pp(next_state) << std::endl;
  }

}

BOOST_AUTO_TEST_CASE(abp)
{
  specification spec = linearise(lps::detail::ABP_SPECIFICATION());
  std::clog << "--- before ---\n" << lps::pp(spec) << std::endl;
  rewriter r(spec.data());
  binary_algorithm<rewriter>(spec, r).run();
  std::clog << "--- after ---\n" << lps::pp(spec) << std::endl;
  BOOST_CHECK(is_well_typed(spec));
  core::garbage_collect();
}

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  return 0;
}

