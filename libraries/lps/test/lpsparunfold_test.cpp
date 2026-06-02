// Author(s): Frank Stappers
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpsparunfold_test.cpp
/// \brief Tests for the lpsparunfold algorithm.

#define BOOST_TEST_MODULE lpsparunfold_test
#include <boost/algorithm/string.hpp>
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/lps/lpsparunfoldlib.h"
#include "mcrl2/lps/parse.h"

using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::lps;


BOOST_AUTO_TEST_CASE(test_main)
{
    /**
      * Unfold Pos process parameter at index 0
      *
      **/

    std::string case_1 =
      "% Test Case 1 -- Unfold Pos                                                      \n"
      "%                                                                                \n"
      "% The first process parameter i of type Pos is is unfolded                       \n"
      "                                                                                 \n"
      "act action :Pos;                                                                 \n"
      "                                                                                 \n"
      "proc P(i: Pos) = action(i). P(i);                                                \n"
      "                                                                                 \n"
      "init P(1);                                                                       \n"
      ;


    stochastic_specification s0;
    parse_lps(case_1,s0);
    variable_list p0 = s0.process().process_parameters();

    /* Requirements */
    if (p0.size() != 1)
    {
      std::clog << "--- failed test ---" << std::endl;
      std::clog << case_1 << std::endl;
      std::clog << "expected 1 process parameter" << std::endl;
      std::clog << "encountered " << p0.size() << "process parameters" << std::endl;
    }
    BOOST_CHECK(p0.size() == 1);

    std::string t0 = data::pp(p0.front().sort());
    if (t0.compare("Pos") != 0)
    {
      std::clog << "--- failed test ---" << std::endl;
      std::clog << case_1 << std::endl;
      std::clog << "expected process parameter to be of type Pos" << std::endl;
      std::clog << "encountered process parameter of type " << p0.front().sort() << std::endl;
    }
    BOOST_CHECK(t0.compare("Pos") == 0);

    /* Return */

    std::map< mcrl2::data::sort_expression , lps::unfold_cache_element > unfold_cache;
    lpsparunfold lpsparunfold(s0, unfold_cache);
    lpsparunfold.algorithm(0);
    variable_list p1 = s0.process().process_parameters();
#ifdef MCRL2_ENABLE_MACHINENUMBERS
    if (p1.size() != 2)
    {
      std::clog << "--- failed test ---" << std::endl;
      std::clog << case_1 << std::endl;
      std::clog << "expected result to have 2 process parameters" << std::endl;
      std::clog << "computed " << p1.size() << " process parameters" << std::endl;
    }
#else
    if (p1.size() != 3)
    {
      std::clog << "--- failed test ---" << std::endl;
      std::clog << case_1 << std::endl;
      std::clog << "expected result to have 3 process parameters" << std::endl;
      std::clog << "computed " << p1.size() << " process parameters" << std::endl;
    }
#endif

    for (variable_list::iterator i = p1.begin(); i != p1.end(); ++i)
    {
      if (std::distance(p1.begin(), i) == 1 && data::pp(i->sort()).compare("Bool") != 0)
      {
        std::clog << "--- failed test ---" << std::endl;
        std::clog << lps::pp(s0) << std::endl;
        std::clog << "expected 2nd process parameter to be of type Bool" << std::endl;
        std::clog << "computed process parameter of type "  << data::pp(i->sort()) << std::endl;
      }
#ifdef MCRL2_ENABLE_MACHINENUMBERS
      BOOST_CHECK(!(std::distance(p1.begin(), i) == 1 && data::pp(i->sort()).compare("Bool") == 0));
#else
      BOOST_CHECK(!(std::distance(p1.begin(), i) == 1 && data::pp(i->sort()).compare("Bool") != 0));
#endif
      if (std::distance(p1.begin(), i) == 2 && data::pp(i->sort()).compare("Pos") != 0)
      {
        std::clog << "--- failed test ---" << std::endl;
        std::clog << lps::pp(s0) << std::endl;
        std::clog << "expected 3th process parameter to be of type Pos " << std::endl;
        std::clog << "computed process parameter of type "  << data::pp(i->sort()) << std::endl;
      }
      BOOST_CHECK(!(std::distance(p1.begin(), i) == 2 && data::pp(i->sort()).compare("Pos") != 0));
    }
}

// Regression test: unfolding a List(T) parameter when a pattern-matching mapping has a
// nested constructor subpattern caused create_cases() to look up a case function for a
// sort (List(Pair)) that was never registered in the Pair cache element.
// Issue originally spotted by Jan Friso Groote.
BOOST_AUTO_TEST_CASE(test_unfold_list_pair_nested_constructor_pattern)
{
  // `toggle` pattern-matches on |> with the Pair-typed head expressed as `pair(n, b)`.
  // After unfolding the List(Pair) parameter, the pattern-match unfolder encounters
  // Det_ListPair(toggle(C_ListPair(...))), unfolds `toggle`, and eventually calls
  // create_cases(pi_ListPair(x), [List(Pair)-typed rhs]), needing case_functions[List(Pair)]
  // in the Pair cache — which did not exist before the fix.
  const std::string spec_text =
    "sort Pair = struct pair(Nat, Bool);\n"
    "\n"
    "map  toggle: List(Pair) -> List(Pair);\n"
    "     cons_pair: Nat # Bool # List(Pair) -> List(Pair);\n"
    "var  n: Nat; b: Bool; t: List(Pair);\n"
    "eqn  toggle([]) = [];\n"
    "     toggle(pair(n, b) |> t) = pair(n, !b) |> toggle(t);\n"
    "     cons_pair(n, b, t) = pair(n, b) |> t;\n"
    "\n"
    "act  add: Nat # Bool;\n"
    "     flip;\n"
    "\n"
    "proc P(s: List(Pair)) =\n"
    "       sum n: Nat, b: Bool. add(n, b) . P(cons_pair(n, b, s))\n"
    "     + flip . P(toggle(s));\n"
    "\n"
    "init P([]);\n";

  stochastic_specification spec;
  parse_lps(spec_text, spec);

  std::map<data::sort_expression, unfold_cache_element> cache;
  lpsparunfold unfolder(spec, cache, false, false, true);
  // Index 0 is s: List(Pair).  Unfolding it triggers the pattern-match unfolder on
  // Det_ListPair(toggle(C_ListPair(...))), which hits the missing case function.
  BOOST_CHECK_NO_THROW(unfolder.algorithm(0));
}
