// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <boost/test/minimal.hpp>

#include <mcrl2/atermpp/aterm_init.h>
#include <mcrl2/lts/lts_algorithm.h>
#include <mcrl2/lts/lts_aut.h>

using namespace mcrl2::lts;

int test_main(int argc, char** argv)
{
  lts_aut_t l1;
  lts_aut_t l2;
  lts_aut_t l3;
  lts_aut_t l4;
  const mcrl2::lts::detail::action_label_string lab_a("a");
  const mcrl2::lts::detail::action_label_string lab_b("b");
  const mcrl2::lts::detail::action_label_string lab_c("c");
  const mcrl2::lts::detail::action_label_string lab_tau("tau");

  for (int i=0; i<5; i++)
  {
    l1.add_state();
    l2.add_state();
    l3.add_state();
    l4.add_state();
  }
  l1.set_initial_state(0);
  l2.set_initial_state(0);
  l3.set_initial_state(0);
  l4.set_initial_state(0);

  l1.add_action(lab_a);
  l1.add_action(lab_b);
  l1.add_action(lab_c);
  l1.add_action(lab_tau,true);
  l2.add_action(lab_a);
  l2.add_action(lab_b);
  l2.add_action(lab_c);
  l2.add_action(lab_tau,true);
  l3.add_action(lab_a);
  l3.add_action(lab_b);
  l3.add_action(lab_c);
  l3.add_action(lab_tau,true);
  l4.add_action(lab_a);
  l4.add_action(lab_b);
  l4.add_action(lab_c);
  l4.add_action(lab_tau,true);

  // a.(b+c)
  l1.add_transition(transition(0,0,1));
  l1.add_transition(transition(1,1,2));
  l1.add_transition(transition(1,2,3));

  // a.b+a.c
  l2.add_transition(transition(0,0,1));
  l2.add_transition(transition(0,0,2));
  l2.add_transition(transition(1,1,3));
  l2.add_transition(transition(2,2,4));

  // a.tau.(b+c)
  l3.add_transition(transition(0,0,1));
  l3.add_transition(transition(1,3,2));
  l3.add_transition(transition(2,1,3));
  l3.add_transition(transition(2,2,4));

  // a.(b+b)
  l4.add_transition(transition(0,0,1));
  l4.add_transition(transition(1,1,2));
  l4.add_transition(transition(1,1,3));
  BOOST_CHECK(compare(l1,l2,lts_eq_trace));
  BOOST_CHECK(compare(l2,l1,lts_eq_trace));

  BOOST_CHECK(! compare(l1,l3,lts_eq_trace));
  BOOST_CHECK(! compare(l3,l1,lts_eq_trace));

  BOOST_CHECK(! compare(l1,l4,lts_eq_trace));
  BOOST_CHECK(! compare(l4,l1,lts_eq_trace));

  BOOST_CHECK(compare(l2,l3,lts_eq_weak_trace));
  BOOST_CHECK(compare(l3,l2,lts_eq_weak_trace));

  BOOST_CHECK(! compare(l3,l4,lts_eq_weak_trace));
  BOOST_CHECK(! compare(l4,l3,lts_eq_weak_trace));

  return 0;
}
