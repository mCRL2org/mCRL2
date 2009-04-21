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
#include <boost/test/minimal.hpp>
#include <mcrl2/lps/specification.h>
#include <mcrl2/lps/binary.h>
#include <mcrl2/lps/mcrl22lps.h>
#include "mcrl2/core/garbage_collection.h"

using namespace atermpp;
using namespace mcrl2;
using namespace mcrl2::new_data;
using namespace mcrl2::lps;

///All process parameters of sort D should have been translated to
///parameters of sort Bool. This leaves only parameters of sort Bool and Pos.
void test_case_1()
{
  const std::string text(
    "sort D = struct d1|d2;\n"
    "act a;\n"
    "proc P(e:D) = sum d:D . a . P(d);\n"
    "init P(d1);\n"
  );

  specification s0 = mcrl22lps(text);
  rewriter r(s0.data());
  specification s1 = binary(s0, r);

  summand_list summands1 = s1.process().summands();
  variable_list parameters1 = s1.process().process_parameters();

  int bool_param_count = 0;
  for (variable_list::iterator i = parameters1.begin(); i != parameters1.end(); ++i)
  {
    BOOST_CHECK(i->sort() == sort_bool_::bool_());
    if (i->sort() == sort_bool_::bool_())
    {
      ++bool_param_count;
    }
  }
  BOOST_CHECK(bool_param_count == 1);
}

/*
 * Sort D has got 8 constructors, which can therefore be encoded exactly in
 * a vector of 3 boolean variables. This means that all combinations of the
 * boolean variables encode a constructor.
 * Process parameter e should be replaced by a vector of boolean variables,
 * and should be replaced in nextstate.
 * The initial state should be altered accordingly.
 */
void test_case_2()
{
  const std::string text(
    "sort D = struct d1|d2|d3|d4|d5|d6|d7|d8;\n"
    "act a;\n"
    "proc P(e:D) = sum d:D . a . P(d);\n"
    "init P(d1);\n"
  );

  specification s0 = mcrl22lps(text);
  rewriter r(s0.data());
  specification s1 = binary(s0, r);
  summand_list summands1 = s1.process().summands();

  int bool_param_count = 0;
  for (variable_list::iterator i = s1.process().process_parameters().begin();
       i != s1.process().process_parameters().end();
       ++i)
  {
    BOOST_CHECK(i->sort() == sort_bool_::bool_());
    if (i->sort() == sort_bool_::bool_())
    {
      ++bool_param_count;
    }
  }
  BOOST_CHECK(bool_param_count == 3);
}

/*
 * Sort D has got 7 constructors, which can therefore be encoded in
 * a vector of 3 boolean variables. This means that there is one combination
 * of the boolean variables that does not encode a constructor.
 * Process parameter e should be replaced by a vector of boolean variables,
 * and should be replaced in nextstate.
 * The initial state should be altered accordingly.
 */
void test_case_3()
{
  const std::string text(
    "sort D = struct d1|d2|d3|d4|d5|d6|d7;\n"
    "act a;\n"
    "proc P(e:D) = sum d:D . a . P(d);\n"
    "init P(d1);\n"
  );

  specification s0 = mcrl22lps(text);
  rewriter r(s0.data());
  specification s1 = binary(s0, r);
  summand_list summands1 = s1.process().summands();

  int bool_param_count = 0;
  for (variable_list::iterator i = s1.process().process_parameters().begin();
       i != s1.process().process_parameters().end();
       ++i)
  {
    BOOST_CHECK(i->sort() == sort_bool_::bool_());
    if (i->sort() == sort_bool_::bool_())
    {
      ++bool_param_count;
    }
  }
  BOOST_CHECK(bool_param_count == 3);
}

/*
 * Sort D has got 2 constructors, and should be encoded into one boolean
 * variable. Process parameter e should be replaced by a single boolean variable
 * and the action a(e) and the initial state should be altered accordingly.
 *
 * Note there is parameter of sort Pos because of linearisation.
 */
void test_case_4()
{
  const std::string text(
    "sort D = struct d1|d2;\n"
    "act a,b:D;\n"
    "proc P(e:D) = sum d:D . a(e) . b(d) . P(d);\n"
    "init P(d1);\n"
  );

  specification s0 = mcrl22lps(text);
  rewriter r(s0.data());
  specification s1 = binary(s0, r);
  summand_list summands1 = s1.process().summands();

  int bool_param_count = 0;
  for (variable_list::iterator i = s1.process().process_parameters().begin();
       i != s1.process().process_parameters().end();
       ++i)
  {
    BOOST_CHECK(i->sort() == sort_pos::pos() || i->sort() == sort_bool_::bool_());
    if (i->sort() == sort_bool_::bool_())
    {
      ++bool_param_count;
    }
  }
  BOOST_CHECK(bool_param_count == 2);
}

/*
 * Sort D has got 9 constructors, therefore it should be encoded in
 * a vector of 4 boolean variables. Note that this does leave a lot of
 * combinations of booleans that are not used (7 to be precise).
 * Process parameter e should be replaced by a vector of boolean variables,
 * and should be replaced in nextstate.
 * The initial state should be altered accordingly.
 */
void test_case_5()
{
  const std::string text(
    "sort D = struct d1|d2|d3|d4|d5|d6|d7|d8|d9;\n"
    "act a;\n"
    "proc P(e:D) = sum d:D . a . P(d);\n"
    "init P(d1);\n"
  );

  specification s0 = mcrl22lps(text);
  rewriter r(s0.data());
  specification s1 = binary(s0, r);
  summand_list summands1 = s1.process().summands();

  int bool_param_count = 0;
  for (variable_list::iterator i = s1.process().process_parameters().begin();
       i != s1.process().process_parameters().end();
       ++i)
  {
    BOOST_CHECK(i->sort() == sort_bool_::bool_());
    if (i->sort() == sort_bool_::bool_())
    {
      ++bool_param_count;
    }
  }
  BOOST_CHECK(bool_param_count == 4);
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
void test_case_6()
{
  const std::string text(
    "sort D = struct d1(E) | d2(E);\n"
    "     E = struct e1 | e2;\n"
    "act a;\n"
    "proc P(e:D) = sum d:D . a . P(d);\n"
    "init P(d1(e1));\n"
  );

  specification s0 = mcrl22lps(text);
  rewriter r(s0.data());
  specification s1 = binary(s0, r);
  summand_list summands1 = s1.process().summands();

  int bool_param_count = 0;
  for (variable_list::iterator i = s1.process().process_parameters().begin();
       i != s1.process().process_parameters().end();
       ++i)
  {
    BOOST_CHECK(i->sort() == sort_bool_::bool_());
    if (i->sort() == sort_bool_::bool_())
    {
      ++bool_param_count;
    }
  }
  BOOST_CHECK(bool_param_count == 2);
}

int test_main(int ac, char** av)
{
  MCRL2_ATERM_INIT(ac, av)

  test_case_1();
  core::garbage_collect();
  test_case_2();
  core::garbage_collect();
  test_case_3();
  core::garbage_collect();
  test_case_4();
  core::garbage_collect();
  test_case_5();
  core::garbage_collect();
  test_case_6();
  core::garbage_collect();

  return 0;
}

