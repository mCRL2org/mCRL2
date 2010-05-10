// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <boost/test/minimal.hpp>

#include <aterm2.h>
#include <mcrl2/atermpp/aterm_init.h>
#include <mcrl2/lts/lts_algorithm.h>

using namespace mcrl2::lts;

int test_main(int argc, char **argv)
{
  MCRL2_ATERMPP_INIT(argc,argv);

  lts l1(false,true);
  lts l2(false,true);
  lts l3(false,true);
  lts l4(false,true);
  ATerm lab_a = (ATerm) ATmakeAppl0(ATmakeAFun("a",0,ATfalse));
  ATerm lab_b = (ATerm) ATmakeAppl0(ATmakeAFun("b",0,ATfalse));
  ATerm lab_c = (ATerm) ATmakeAppl0(ATmakeAFun("c",0,ATfalse));
  ATerm lab_tau = (ATerm) ATmakeAppl0(ATmakeAFun("tau",0,ATfalse));

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

  l1.add_label(lab_a);
  l1.add_label(lab_b);
  l1.add_label(lab_c);
  l1.add_label(lab_tau,true);
  l2.add_label(lab_a);
  l2.add_label(lab_b);
  l2.add_label(lab_c);
  l2.add_label(lab_tau,true);
  l3.add_label(lab_a);
  l3.add_label(lab_b);
  l3.add_label(lab_c);
  l3.add_label(lab_tau,true);
  l4.add_label(lab_a);
  l4.add_label(lab_b);
  l4.add_label(lab_c);
  l4.add_label(lab_tau,true);

  // a.(b+c)
  l1.add_transition(0,0,1);
  l1.add_transition(1,1,2);
  l1.add_transition(1,2,3);

  // a.b+a.c
  l2.add_transition(0,0,1);
  l2.add_transition(0,0,2);
  l2.add_transition(1,1,3);
  l2.add_transition(2,2,4);

  // a.tau.(b+c)
  l3.add_transition(0,0,1);
  l3.add_transition(1,3,2);
  l3.add_transition(2,1,3);
  l3.add_transition(2,2,4);

  // a.(b+b)
  l4.add_transition(0,0,1);
  l4.add_transition(1,1,2);
  l4.add_transition(1,1,3);

  BOOST_CHECK( compare(l1,l2,lts_eq_trace) );
  BOOST_CHECK( compare(l2,l1,lts_eq_trace) );

  BOOST_CHECK( ! compare(l1,l3,lts_eq_trace) );
  BOOST_CHECK( ! compare(l3,l1,lts_eq_trace) );

  BOOST_CHECK( ! compare(l1,l4,lts_eq_trace) );
  BOOST_CHECK( ! compare(l4,l1,lts_eq_trace) );

  BOOST_CHECK( compare(l2,l3,lts_eq_weak_trace) );
  BOOST_CHECK( compare(l3,l2,lts_eq_weak_trace) );

  BOOST_CHECK( ! compare(l3,l4,lts_eq_weak_trace) );
  BOOST_CHECK( ! compare(l4,l3,lts_eq_weak_trace) );

  return 0;
}
