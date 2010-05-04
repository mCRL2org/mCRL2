// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file nextstate_test.cpp
/// \brief Test for nextstate generator

#include <queue>
#include <boost/test/included/unit_test_framework.hpp>
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/atermpp/deque.h"
#include "mcrl2/data/selection.h"
#include "mcrl2/lps/nextstate.h"
#include "mcrl2/lps/parse.h"

using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::data::detail;
using namespace mcrl2::lps;
using namespace mcrl2::lps::detail;

void test_nextstate(specification const& s, size_t expected_states, size_t expected_transitions, size_t expected_transition_labels)
{
  rewriter R(s.data(),
           mcrl2::data::used_data_equation_selector(s.data(), lps::specification_to_aterm(s)));

  mcrl2::data::enumerator_factory< mcrl2::data::classic_enumerator< > > E(s.data(), R);

  NextState* nstate = createNextState(s, E, false);

  atermpp::aterm initial_state = nstate->getInitialState();

  atermpp::set<atermpp::aterm> visited;
  atermpp::set<atermpp::aterm> seen;
  atermpp::set<atermpp::aterm_appl> transition_labels;
  size_t transitions = 0;

  std::queue<atermpp::aterm, atermpp::deque<atermpp::aterm> > q;
  q.push(initial_state);
  seen.insert(initial_state);

  while(!q.empty())
  {
    visited.insert(q.front());
    std::auto_ptr< NextStateGenerator > generator(nstate->getNextStates(q.front()));

    ATerm     state;
    ATermAppl transition;
    while(generator->next(&transition, &state))
    {
      atermpp::aterm s(state);
      atermpp::aterm_appl t(transition);
      transition_labels.insert(t);
      ++transitions;
      if(seen.find(s) == seen.end())
      {
        q.push(s);
        seen.insert(s);
      }
    }

    q.pop();
  }

  BOOST_CHECK(seen.size() == visited.size());
  BOOST_CHECK(seen.size() == expected_states);
  BOOST_CHECK(transitions == expected_transitions);
  BOOST_CHECK(transition_labels.size() == expected_transition_labels);

  std::cerr << "Seen " << seen.size() << " states and " << transitions << " transitions, with " << transition_labels.size() << " labels" << std::endl;
  std::cerr << "Expected " << expected_states << " states and " << expected_transitions << " transitions, with " << expected_transition_labels << " labels" << std::endl;
}

BOOST_AUTO_TEST_CASE(single_state_test)
{
  std::string spec(
    "act  a;\n"
    "proc P(s3: Pos) =\n"
    "       (s3 == 1) ->\n"
    "        a .\n"
    "        P(s3 = 2)\n"
    "     + delta;\n"
    "init P(1);\n"
  );
  test_nextstate(parse_linear_process_specification(spec), 2, 1, 1);
}

BOOST_AUTO_TEST_CASE(test_abp)
{
  std::string spec(
    "sort Error = struct e;\n"
    "    D = struct d1 | d2;\n"

    "act  i;\n"
    "     c6,r6,s6: Error;\n"
    "     c6,r6,s6,c5,r5,s5: Bool;\n"
    "     c3,r3,s3: Error;\n"
    "     c3,r3,s3,c2,r2,s2: D # Bool;\n"
    "     s4,r1: D;\n"

    "proc P(s30_S: Pos, d_S: D, b_S: Bool, s31_K: Pos, d_K: D, b_K: Bool, s32_L: Pos, b_L: Bool, s33_R: Pos, d_R: D, b_R: Bool) =\n"
    "       sum d0_S: D.\n"
    "         (s30_S == 1) ->\n"
    "         r1(d0_S) .\n"
    "         P(s30_S = 2, d_S = d0_S)\n"
    "     + sum e0_K: Bool.\n"
    "         (s31_K == 2) ->\n"
    "         i .\n"
    "         P(s31_K = if(e0_K, 4, 3), d_K = if(e0_K, d1, d_K), b_K = if(e0_K, true, b_K))\n"
    "     + sum e1_L: Bool.\n"
    "         (s32_L == 2) ->\n"
    "         i .\n"
    "         P(s32_L = if(e1_L, 4, 3), b_L = if(e1_L, true, b_L))\n"
    "     + (s33_R == 2) ->\n"
    "         s4(d_R) .\n"
    "         P(s33_R = 3, d_R = d1)\n"
    "     + sum e2_R: Bool.\n"
    "         (s32_L == 1 && if(e2_R, s33_R == 4, s33_R == 3)) ->\n"
    "         c5(if(e2_R, !b_R, b_R)) .\n"
    "         P(s32_L = 2, b_L = if(e2_R, !b_R, b_R), s33_R = 1, d_R = d1, b_R = if(e2_R, b_R, !b_R))\n"
    "     + (s31_K == 4 && s33_R == 1) ->\n"
    "         c3(e) .\n"
    "         P(s31_K = 1, d_K = d1, b_K = true, s33_R = 4, d_R = d1)\n"
    "     + sum e3_R: Bool.\n"
    "         ((s31_K == 3 && s33_R == 1) && if(e3_R, !b_R, b_R) == b_K) ->\n"
    "         c3(d_K, if(e3_R, !b_R, b_R)) .\n"
    "         P(s31_K = 1, d_K = d1, b_K = true, s33_R = if(e3_R, 4, 2), d_R = if(e3_R, d1, d_K))\n"
    "     + (s30_S == 2 && s31_K == 1) ->\n"
    "         c2(d_S, b_S) .\n"
    "         P(s30_S = 3, s31_K = 2, d_K = d_S, b_K = b_S)\n"
    "     + (s30_S == 3 && s32_L == 4) ->\n"
    "         c6(e) .\n"
    "         P(s30_S = 2, s32_L = 1, b_L = true)\n"
    "     + sum e_S: Bool.\n"
    "         ((s30_S == 3 && s32_L == 3) && if(e_S, b_S, !b_S) == b_L) ->\n"
    "         c6(if(e_S, b_S, !b_S)) .\n"
    "         P(s30_S = if(e_S, 1, 2), d_S = if(e_S, d1, d_S), b_S = if(e_S, !b_S, b_S), s32_L = 1, b_L = true)\n"
    "     + delta;\n"

    "init P(1, d1, true, 1, d1, true, 1, true, 1, d1, true);\n"
  );

  test_nextstate(parse_linear_process_specification(spec), 74, 92, 19);
}

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  return 0;
}
