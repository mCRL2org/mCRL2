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

#include "mcrl2/lts/parse.h"

using namespace mcrl2;

void parse_fsm(const std::string& text, lts::lts_fsm_t& result)
{
  std::string temp_filename = "parse_test.tmp";
  std::ofstream to(temp_filename.c_str());
  to << text;
  to.close();
  result.loadnew(temp_filename);
  remove(temp_filename.c_str());
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

void test_fsm()
{
  lts::lts_fsm_t fsm1;
  lts::lts_fsm_t fsm2;
  std::string text1;
  std::string text2;
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
  lts::parse_fsm_specification(FSM, fsm1);
  parse_fsm(FSM, fsm2);
  text1 = print_fsm(fsm1);
  text2 = print_fsm(fsm2);
  if (text1 != text2)
  {
    std::cout << "--- text1 ---" << std::endl;
    std::cout << text1 << std::endl;
    std::cout << "--- text2 ---" << std::endl;
    std::cout << text2 << std::endl;
  }
  BOOST_CHECK(text1 == text2);

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
  lts::parse_fsm_specification(FSM, fsm1);
  parse_fsm(FSM, fsm2);
  text1 = print_fsm(fsm1);
  text2 = print_fsm(fsm2);
  BOOST_CHECK(text1 == text2);
}

int test_main(int argc, char** argv)
{
  test_fsm();

  return 0;
}
