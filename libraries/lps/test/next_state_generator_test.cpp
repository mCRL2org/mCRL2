// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file next_state_generator_test.cpp
/// \brief Test for next_state_generator class.

#include <queue>
#include <boost/test/included/unit_test_framework.hpp>
#include "mcrl2/data/rewriter.h"
#include "mcrl2/lps/next_state_generator.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/lps/state.h"

using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::data::detail;
using namespace mcrl2::lps;
using namespace mcrl2::lps::detail;

void test_initial_state_successors(const specification& lps_spec)
{
  data::rewriter rewriter(lps_spec.data());
  next_state_generator generator(lps_spec, rewriter);
  next_state_generator::substitution_t sigma;

  for (next_state_generator::iterator it = generator.begin(generator.initial_state(), &sigma); it != generator.end(); it++)
  {
    std::cout << lps::pp(it->state()) << std::endl;
  }
}

void test_next_state_generator(const specification& lps_spec, size_t expected_states, size_t expected_transitions, size_t expected_transition_labels, bool enumeration_caching, bool summand_pruning, bool per_summand)
{
  data::rewriter rewriter(lps_spec.data());
  next_state_generator generator(lps_spec, rewriter, enumeration_caching, summand_pruning);
  next_state_generator::substitution_t sigma;

  state initial_state = generator.initial_state();

  std::set<state> visited;
  std::set<state> seen;
  std::set<multi_action> transition_labels;
  size_t transitions = 0;

  std::queue<state, std::deque<state> > q;
  q.push(initial_state);
  seen.insert(initial_state);

  while (!q.empty())
  {
    visited.insert(q.front());

    if (per_summand)
    {
      for (size_t i = 0; i < lps_spec.process().action_summands().size(); ++i)
      {
        for (next_state_generator::iterator it = generator.begin(q.front(), &sigma, i); it != generator.end(); it++)
        {
          transition_labels.insert(it->action());
          ++transitions;
          if (seen.find(it->state()) == seen.end())
          {
            q.push(it->state());
            seen.insert(it->state());
          }
        }
      }
    }
    else
    {
      for (next_state_generator::iterator it = generator.begin(q.front(), &sigma); it != generator.end(); it++)
      {
        transition_labels.insert(it->action());
        ++transitions;
        if (seen.find(it->state()) == seen.end())
        {
          q.push(it->state());
          seen.insert(it->state());
        }
      }
    }
    q.pop();
  }

  BOOST_CHECK(seen.size() == visited.size());
  BOOST_CHECK(seen.size() == expected_states);
  BOOST_CHECK(transitions == expected_transitions);
  BOOST_CHECK(transition_labels.size() == expected_transition_labels);
}

BOOST_AUTO_TEST_CASE(test_abp)
{
  std::string text(
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

  specification spec = parse_linear_process_specification(text);
  test_initial_state_successors(spec);
  // Test all 2**3 possible feature flags
  for (size_t i = 0; i < 8; i++)
  {
    test_next_state_generator(spec, 74, 92, 19, i & 1, i & 2, i & 4);
  }
}

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
  return 0;
}
