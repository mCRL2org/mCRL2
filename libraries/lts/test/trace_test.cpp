// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file trace_test.cpp
/// \brief Regression tests for the boundary behaviour of trace::next_state()
///        and trace::current_time().

#define BOOST_TEST_MODULE trace_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/process/parse.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/lts/trace.h"

using namespace mcrl2;
using namespace mcrl2::lts;
using namespace mcrl2::data;

// next_state() returns the state following the current position. It reads
// m_states[m_pos+1], so it must only succeed when that element exists. This is
// a regression test for an off-by-one error where next_state() checked
// m_pos >= m_states.size() (allowing m_states[m_pos+1] to be read out of
// bounds) instead of m_pos+1 >= m_states.size().
BOOST_AUTO_TEST_CASE(test_next_state_off_by_one)
{
  process::action_label_list action_decls = process::parse_action_declaration("a;")
                                             + process::parse_action_declaration("b;");

  trace t;
  t.add_action(mcrl2::lps::parse_multi_action("a", action_decls));
  t.add_action(mcrl2::lps::parse_multi_action("b", action_decls));

  // Add states for positions 0 and 1, but deliberately not for position 2.
  const lps::state s0(sort_bool::true_());
  const lps::state s1(sort_bool::false_());
  t.reset_position();
  t.set_state(s0);
  t.increase_position();
  t.set_state(s1);

  BOOST_REQUIRE(t.number_of_actions() == 2);
  BOOST_REQUIRE(t.number_of_states() == 2);

  // At position 0 the next state exists and equals s1.
  t.reset_position();
  BOOST_CHECK(t.next_state() == s1);

  // At position 1 there is no state at position 2, so next_state() must throw
  // instead of reading m_states[2] out of bounds.
  t.increase_position();
  BOOST_CHECK_THROW(t.next_state(), mcrl2::runtime_error);
}

// current_time() returns the time of the action at the current position. When
// the current position is at the end of the trace there is no action, so it
// must return a default (undefined) data_expression instead of reading
// m_actions[m_pos] out of bounds.
BOOST_AUTO_TEST_CASE(test_current_time_at_end)
{
  process::action_label_list action_decls = process::parse_action_declaration("a;");

  // A trace with a single action whose time is the real number 5.
  const mcrl2::lps::multi_action untimed_a = mcrl2::lps::parse_multi_action("a", action_decls);
  const mcrl2::lps::multi_action timed_a(untimed_a.actions(), sort_real::real_(5));

  trace t;
  t.add_action(timed_a);

  // At position 0 the action exists and its time is 5.
  t.reset_position();
  BOOST_CHECK(t.current_time() == sort_real::real_(5));
  BOOST_CHECK(!t.current_time().is_default_data_expression());

  // At the end position there is no action, so the time is the default
  // (undefined) data_expression.
  t.increase_position();
  BOOST_CHECK(t.current_time().is_default_data_expression());
}

// The time of an untimed action is undefined_real(), which differs from the
// default data_expression that current_time() returns past the end of the
// trace.
BOOST_AUTO_TEST_CASE(test_current_time_untimed_action)
{
  process::action_label_list action_decls = process::parse_action_declaration("a;");

  trace t;
  t.add_action(mcrl2::lps::parse_multi_action("a", action_decls));

  t.reset_position();
  BOOST_CHECK(t.current_time() == undefined_real());
  BOOST_CHECK(!t.current_time().is_default_data_expression());
}

// The constructor that reads a trace from a file (whose redundant try/catch was
// removed) must still load a previously saved trace correctly.
BOOST_AUTO_TEST_CASE(test_load_via_constructor)
{
  process::action_label_list action_decls = process::parse_action_declaration("a;")
                                             + process::parse_action_declaration("b;");

  trace t;
  t.add_action(mcrl2::lps::parse_multi_action("a", action_decls));
  t.add_action(mcrl2::lps::parse_multi_action("b", action_decls));

  const std::string filename = "trace_test_ctor.trc";
  t.save(filename);

  const trace t2(filename);
  BOOST_CHECK(t2.number_of_actions() == 2);
}
