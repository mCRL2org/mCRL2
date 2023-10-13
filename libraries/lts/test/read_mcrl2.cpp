// Author(s): Muck van Weerdenburg, Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file read_mcrl2.cpp
/// \brief Test input and output of traces. 

#define BOOST_TEST_MODULE read_mcrl2

#include <boost/test/included/unit_test.hpp>
#include <exception>

#include "mcrl2/lts/trace.h"

using namespace mcrl2;
using namespace mcrl2::lts;

const std::string trace_data = "trace_mcrl2.trc";


void test_next_action(trace& t, const char* s)
{
  mcrl2::lps::multi_action a = t.current_action();
  t.increase_position();
  BOOST_CHECK((a != mcrl2::lps::multi_action()));
  if (a != mcrl2::lps::multi_action())
  {
    std::string action(pp(a));
    BOOST_CHECK(action == s);
    if (action != s)
    {
      std::cout << "--- error detected ---\n";
      std::cout << "result         : " << action << std::endl;
      std::cout << "expected result: " << s << std::endl;
    }
  }
}

BOOST_AUTO_TEST_CASE(test_main)
{
  using namespace mcrl2::data;
  using namespace mcrl2::lps;
  using namespace mcrl2::core;
  mcrl2::data::data_specification data_spec;
  process::action_label_list act_decls;
  act_decls.push_front(process::action_label(identifier_string("a"),sort_expression_list()));
  sort_expression_list s;
  s.push_front(sort_bool::bool_());
  s.push_front(sort_pos::pos());
  act_decls.push_front(process::action_label("b",s));
  act_decls.push_front(process::action_label(identifier_string("c"),sort_expression_list()));

  trace t(data_spec, act_decls);
  t.add_action(parse_multi_action("a",act_decls,data_spec));
  t.add_action(parse_multi_action("b(1,true)",act_decls,data_spec));
  t.add_action(parse_multi_action("c",act_decls,data_spec));

  t.save(trace_data);

  try
  {
    t.load(trace_data);
  }
  catch (const mcrl2::runtime_error& e)
  {
    BOOST_ERROR(e.what());
  }

  BOOST_CHECK(t.number_of_actions() == 3);
std::cerr << "STATE  " << t.number_of_states() << "\n";
  BOOST_CHECK(t.number_of_states() == 0);  // There are no explicit states in this trace.

  test_next_action(t,"a");
  test_next_action(t,"b(1, true)");
  test_next_action(t,"c");

  // Now add a few explicit states. 
  lps::state local_state1( sort_bool::true_() );
  lps::state local_state2( sort_bool::false_() );

  t.reset_position();
  t.set_state(local_state1);
  t.increase_position();
  t.set_state(local_state2);
  t.increase_position();
  t.set_state(local_state1);
  t.increase_position();
  t.set_state(local_state2);

  t.save(trace_data);

  try
  { 
    t.load(trace_data);
  }
  catch (const mcrl2::runtime_error& e)
  { 
    BOOST_ERROR(e.what());
  }

  BOOST_CHECK(t.number_of_actions() == 3);
std::cerr << "STATE  " << t.number_of_states() << "\n";
  BOOST_CHECK(t.number_of_states() == 4);  // There are no explicit states in this trace.
std::cerr << "ASTATE " << t.current_state() << "\n";
  BOOST_CHECK(t.current_state()==local_state1);
std::cerr << "NSTATE " << t.next_state() << "\n";
  BOOST_CHECK(t.next_state()==local_state2);
  test_next_action(t,"a");
std::cerr << "ASTATE " << t.current_state() << "\n";
  BOOST_CHECK(t.current_state()==local_state2);
std::cerr << "NSTATE " << t.next_state() << "\n";
  BOOST_CHECK(t.next_state()==local_state1);
  test_next_action(t,"b(1, true)");
  test_next_action(t,"c");
}

BOOST_AUTO_TEST_CASE(test_empty_trace)
{
  using namespace mcrl2::data;
  using namespace mcrl2::lps;
  using namespace mcrl2::core;
  mcrl2::data::data_specification data_spec;
  process::action_label_list act_decls;

  trace t(data_spec, act_decls);

  t.save(trace_data);

  try
  {
    t.load(trace_data);
  }
  catch (const mcrl2::runtime_error& e)
  {
    BOOST_ERROR(e.what());
  }

  BOOST_CHECK(t.number_of_actions() == 0);
  BOOST_CHECK(t.number_of_states() == 0);

  // Now add an initial state.
  
  lps::state local_state1( sort_bool::true_() );

  t.set_state(local_state1);

  t.save(trace_data);

  try
  {
    t.load(trace_data);
  }
  catch (const mcrl2::runtime_error& e)
  {
    BOOST_ERROR(e.what());
  }

  BOOST_CHECK(t.number_of_actions() == 0);
  BOOST_CHECK(t.number_of_states() == 1);

  BOOST_CHECK(t.current_state()==local_state1);

}
