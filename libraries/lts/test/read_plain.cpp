// Author(s): Muck van Weerdenbrug, Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file read_plain.cpp

#define BOOST_TEST_MODULE read_plain
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/process/parse.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/lts/trace.h"

using namespace mcrl2;
using namespace mcrl2::lts;
using namespace mcrl2::data;

const std::string filename = "trace_plain.trc";

static bool read_trace(trace& t, const std::string& input_file)
{
  try
  {
    t.load(input_file);
  }
  catch (...)
  {
    throw;
    return false;
  }

  return true;
}

void test_next_action(trace& t, const char* s)
{
  mcrl2::lps::multi_action a = t.current_action();
  t.increase_position();
  BOOST_CHECK(a != mcrl2::lps::multi_action());
  if (a != mcrl2::lps::multi_action())
  {
std::cerr << "Current action " << a << " compare to " << s << "\n";
    BOOST_CHECK(a.actions().size() == 1);
    BOOST_CHECK(pp(a)==s);
  }
}

BOOST_AUTO_TEST_CASE(test_main)
{
  mcrl2::log::logger::set_reporting_level(mcrl2::log::debug);
  trace t;

  process::action_label_list action_decls = process::parse_action_declaration("a;") +
                                            process::parse_action_declaration("b:Nat#Bool;") +
                                            process::parse_action_declaration("c;");

  t.add_action(mcrl2::lps::parse_multi_action("a", action_decls));
  t.add_action(mcrl2::lps::parse_multi_action("b(1, true)", action_decls));
  t.add_action(mcrl2::lps::parse_multi_action("c", action_decls));

  t.save(filename,trace::tfPlain);
  BOOST_REQUIRE(read_trace(t,filename));

  BOOST_REQUIRE(t.number_of_actions() == 3);
  BOOST_REQUIRE(t.number_of_states() == 0);

  test_next_action(t,"a");
  test_next_action(t,"b(1, true)");
  test_next_action(t,"c");

  // now check this with explicit states. 

  t.reset_position();
  t.set_state(lps::state(sort_bool::true_()));
  t.increase_position();
  t.set_state(lps::state(sort_bool::false_()));
  t.increase_position();
  t.set_state(lps::state(sort_bool::true_()));
  t.increase_position();
  t.set_state(lps::state(sort_bool::false_()));

  t.save(filename,trace::tfPlain);
  BOOST_REQUIRE(read_trace(t,filename));

  BOOST_REQUIRE(t.number_of_actions() == 3);
std::cerr << "Number of states " << t.number_of_states() << "\n";
  BOOST_REQUIRE(t.number_of_states() == 0);  // In plain format states are not read back. 

  test_next_action(t,"a");
  test_next_action(t,"b(1, true)");
  test_next_action(t,"c");
}


BOOST_AUTO_TEST_CASE(test_empty)
{
  trace t;

  t.save(filename,trace::tfPlain);
  BOOST_REQUIRE(read_trace(t,filename));

  BOOST_REQUIRE(t.number_of_actions() == 0);
  BOOST_REQUIRE(t.number_of_states() == 0);

  // now check this with explicit states. 

  t.set_state(lps::state(sort_bool::true_()));

  t.save(filename);
  BOOST_REQUIRE(read_trace(t,filename));

  BOOST_REQUIRE(t.number_of_actions() == 0);
  BOOST_REQUIRE(t.number_of_states() == 1);

  BOOST_CHECK(t.current_state()==lps::state(sort_bool::true_()));
} 

