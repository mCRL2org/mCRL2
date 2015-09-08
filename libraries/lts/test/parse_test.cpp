// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file parse_test.cpp
/// \brief Add your file description here.

#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdio>

#include <boost/test/minimal.hpp>
#include <boost/xpressive/xpressive.hpp>

#include "mcrl2/lts/parse.h"

using namespace mcrl2;

void parse_fsm(const std::string& text, lts::lts_fsm_t& result)
{
  std::string temp_filename = "parse_test.tmp";
  std::ofstream to(temp_filename.c_str());
  to << text;
  to.close();
  result.loadnew(temp_filename);
  std::remove(temp_filename.c_str());
}

inline
std::string print_state_label(const lts::detail::state_label_fsm& label)
{
  std::ostringstream out;
  for (lts::detail::state_label_fsm::const_iterator i = label.begin(); i != label.end(); ++i)
  {
    out << *i << " ";
  }
  return out.str();
}

inline
std::string print_fsm(const lts::lts_fsm_t& l)
{
  std::ostringstream out;

  out << "#states: "         << l.num_state_labels() << "\n" <<
         "#action labels: "  << l.num_action_labels() << "\n"<<
         "#transitions: "    << l.num_transitions() << "\n" <<
         "#has state labels" << (l.has_state_info() ? " yes\n" : " no\n");

  out << "Initial state is " << l.initial_state() << "\n";

  for(unsigned int i = 0; i < l.num_state_labels(); ++i)
  {
    out << "State " << i << " has value " << print_state_label(l.state_label(i)) << "\n";
  }

  for (unsigned int i = 0; i < l.num_action_labels(); ++i)
  {
    out << "Action label " << i << " has value " << l.action_label(i) << (l.is_tau(i) ? " (is internal)" : " (is external)") << "\n";
  }

  for(std::vector<lts::transition>::const_iterator i = l.get_transitions().begin(); i != l.get_transitions().end(); ++i)
  {
    out << "Transition [" << i->from() << "," << i->label() << "," << i->to() << "]\n";
  }

  return out.str();
}

void test_fsm(const std::string& FSM)
{
  lts::lts_fsm_t fsm1;
  lts::lts_fsm_t fsm2;
  lts::lts_fsm_t fsm3;

  std::string text1;
  std::string text2;
  std::string text3;

  lts::parse_fsm_specification(FSM, fsm1);
  parse_fsm(FSM, fsm2);
  lts::parse_fsm_specification_simple(FSM, fsm3);

  text1 = print_fsm(fsm1);
  text2 = print_fsm(fsm2);
  text3 = print_fsm(fsm3);

  if (text1 != text2 || text1 != text3)
  {
    std::cout << "--- text1 ---" << std::endl;
    std::cout << text1 << std::endl;
    std::cout << "--- text2 ---" << std::endl;
    std::cout << text2 << std::endl;
    std::cout << "--- text3 ---" << std::endl;
    std::cout << text3 << std::endl;
  }
  BOOST_CHECK(text1 == text2 && text1 == text3);
}

void test_fsm()
{
  std::string FSM;

  FSM =
    "b(2) Bool \"F\" \"T\" \n"
    "n(2) Nat \"1\" \"2\"  \n"
    "---                   \n"
    "0 0                   \n"
    "0 1                   \n"
    "1 0                   \n"
    "1 1                   \n"
    "---                   \n"
    "1 2 \"increase\"      \n"
    "1 3 \"on\"            \n"
    "2 4 \"on\"            \n"
    "2 1 \"decrease\"      \n"
    "3 1 \"off\"           \n"
    "3 4 \"increase\"      \n"
    "4 2 \"off\"           \n"
    "4 3 \"decrease\"      \n"
    ;
  test_fsm(FSM);

  FSM =
    "b(2) Bool # Bool -> Nat \"F\" \"T\" \n"
    "n(2) Nat -> Nat \"1\" \"2\"         \n"
    "---                                 \n"
    "0 0                                 \n"
    "0 1                                 \n"
    "1 0                                 \n"
    "1 1                                 \n"
    "---                                 \n"
    "1 2 \"increase\"                    \n"
    "1 3 \"on\"                          \n"
    "2 4 \"on\"                          \n"
    "2 1 \"decrease\"                    \n"
    "3 1 \"off\"                         \n"
    "3 4 \"increase\"                    \n"
    "4 2 \"off\"                         \n"
    "4 3 \"decrease\"                    \n"
    ;
  test_fsm(FSM);

  FSM =
    "---         \n"
    "---         \n"
    "1 1 \"tau\" \n"
    ;
  test_fsm(FSM);
}

void test_read_integers(const std::string& text, const std::vector<std::size_t>& expected_result)
{
  std::vector<std::size_t> result = lts::detail::read_integers(text);
  BOOST_CHECK(result == expected_result);
}

void test_read_integers()
{
  test_read_integers("  1 2 3 ", { 1, 2, 3 });
  test_read_integers("  ", { });
  test_read_integers("1", { 1 });
  test_read_integers(" 1", { 1 });
  test_read_integers("1 ", { 1 });
}

int test_main(int argc, char** argv)
{
  test_fsm();
  test_read_integers();

  return 0;
}
