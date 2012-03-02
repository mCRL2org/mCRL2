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

#include <iostream>
#include <sstream>
#include <boost/test/minimal.hpp>
#include "mcrl2/lts/parse.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace mcrl2;

std::string print_state_label(const lts::detail::state_label_fsm& label)
{
  std::ostringstream out;
  for (lts::detail::state_label_fsm::const_iterator i = label.begin(); i != label.end(); ++i)
  {
    out << *i << " ";
  }
  return out.str();
}

void print_fsm(const lts::lts_fsm_t& l)
{
  // Get the number of states, state values, action labels and transitions.
  std::cout << "#states: "         << l.num_state_labels() << "\n" <<
               "#action labels: "  << l.num_action_labels() << "\n"<<
               "#transitions: "    << l.num_transitions() << "\n" <<
               "#has state labels" << (l.has_state_info() ? " yes\n" : " no\n");

  std::cout << "Initial state is " << l.initial_state() << "\n";

  for(unsigned int i=0; i<l.num_state_labels(); ++i)
  {
    std::cout << "State " << i << " has value " << print_state_label(l.state_label(i)) << "\n";
  }

  for (unsigned int i = 0; i < l.num_action_labels(); ++i)
  {
    std::cout << "Action label " << i << " has value " << l.action_label(i) << (l.is_tau(i) ? " (is internal)" : " (is external)") << "\n";
  }

  for(std::vector<lts::transition>::const_iterator i = l.get_transitions().begin(); i != l.get_transitions().end(); ++i)
  {
    std::cerr << "Transition [" << i->from() << "," << i->label() << "," << i->to() << "]\n";
  }
}

void test_fsm()
{
  lts::lts_fsm_t fsm;

  std::string FSM1 =
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
  lts::parse_fsm_specification(FSM1, fsm);
  print_fsm(fsm);

  std::string FSM2 =
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
  lts::parse_fsm_specification(FSM2, fsm);
  print_fsm(fsm);
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT_DEBUG(argc, argv)

  test_fsm();

  return 0;
}
