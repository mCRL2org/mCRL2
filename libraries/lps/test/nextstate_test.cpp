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
#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/data/selection.h"
#include "mcrl2/lps/nextstate/standard.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/lps/find.h"
#include "test_specifications.h"

using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::data::detail;
using namespace mcrl2::lps;
using namespace mcrl2::lps::detail;

std::string print_state(atermpp::aterm_appl s, legacy_rewriter& R)
{
  std::string result("state(");
  int index = 0;
  for (atermpp::aterm_appl::const_iterator i = s.begin(); i != s.end(); ++i)
  {
    if (index++ != 0)
    {
      result.append(", ");
    }

    result.append(core::pp(atermpp::aterm(R.convert_from(static_cast<ATerm>(*i)))));
  }
  result.append(")");
  return result;
}

void test_nextstate(specification s, size_t expected_states, size_t expected_transitions, size_t expected_transition_labels, bool per_summand = false)
{
  s.process().deadlock_summands().clear(); // It is important to clear the deadlock summands if per_summand = false

  legacy_rewriter R(s.data(),
                    mcrl2::data::used_data_equation_selector(s.data(), lps::find_function_symbols(s), s.global_variables()));

#ifdef MCRL2_REWRITE_RULE_SELECTION_DEBUG
std::clog << "--- rewrite rule selection specification ---\n";
std::clog << lps::pp(s) << std::endl;
std::clog << "--- rewrite rule selection function symbols ---\n";
std::clog << core::detail::print_set(lps::find_function_symbols(s), lps::stream_printer()) << std::endl;
#endif

  NextState* nstate = createNextState(s, R, false);

  atermpp::aterm initial_state = nstate->getInitialState();

  std::set<atermpp::aterm> visited;
  std::set<atermpp::aterm> seen;
  std::set<multi_action> transition_labels;
  size_t transitions = 0;

  std::queue<atermpp::aterm, std::deque<atermpp::aterm> > q;
  q.push(initial_state);
  seen.insert(initial_state);

  while (!q.empty())
  {
    visited.insert(q.front());

    if (per_summand)
    {
      for (size_t i = 0; i < s.process().summand_count(); ++i)
      {
        std::auto_ptr< NextStateGenerator > generator(nstate->getNextStates(q.front(), i));

        ATerm     state;
        multi_action transition;
        while (generator->next(transition, &state))
        {
          atermpp::aterm s(state);
          transition_labels.insert(transition);
          ++transitions;
          if (seen.find(s) == seen.end())
          {
            q.push(s);
            seen.insert(s);
          }
        }
      }
    }
    else
    {
      std::auto_ptr< NextStateGenerator > generator(nstate->getNextStates(q.front()));

      ATerm     state;
      multi_action transition;
      while (generator->next(transition, &state))
      {
        atermpp::aterm s(state);
        transition_labels.insert(transition);
        ++transitions;
        if (seen.find(s) == seen.end())
        {
          q.push(s);
          seen.insert(s);
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
  test_nextstate(parse_linear_process_specification(spec), 2, 1, 1, true);
}

BOOST_AUTO_TEST_CASE(test_abp)
{
  test_nextstate(parse_linear_process_specification(LINEAR_ABP), 74, 92, 19);
  test_nextstate(parse_linear_process_specification(LINEAR_ABP), 74, 92, 19, true);
}

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  return 0;
}
