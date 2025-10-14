// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lts_test.cpp
/// \brief Add your file description here.

#ifndef MCRL2_LTS_TEST_TEST_REDUCTIONS_H
#define MCRL2_LTS_TEST_TEST_REDUCTIONS_H

#include "mcrl2/lts/lts_algorithm.h"

using namespace mcrl2;

class expected_sizes
{
  public:
    std::size_t states_plain=-1, transitions_plain=-1, labels_plain=-1;
    std::size_t states_bisimulation=-1, transitions_bisimulation=-1, labels_bisimulation=-1;
    std::size_t states_branching_bisimulation = 0UL, transitions_branching_bisimulation = 0UL,
                labels_branching_bisimulation = 0UL;
    std::size_t states_divergence_preserving_branching_bisimulation = 0UL,
                transitions_divergence_preserving_branching_bisimulation = 0UL,
                labels_divergence_preserving_branching_bisimulation = 0UL;
    std::size_t states_weak_bisimulation = 0UL, transitions_weak_bisimulation = 0UL, labels_weak_bisimulation = 0UL;
    std::size_t states_divergence_preserving_weak_bisimulation = 0UL,
                transitions_divergence_preserving_weak_bisimulation = 0UL,
                labels_divergence_preserving_weak_bisimulation = 0UL;
    std::size_t states_simulation = 0UL, transitions_simulation = 0UL, labels_simulation = 0UL;
    std::size_t states_trace_equivalence = 0UL, transitions_trace_equivalence = 0UL, labels_trace_equivalence = 0UL;
    std::size_t states_weak_trace_equivalence = 0UL, transitions_weak_trace_equivalence = 0UL,
                labels_weak_trace_equivalence = 0UL;
    std::size_t states_determinisation = 0UL, transitions_determinisation = 0UL, labels_determinisation = 0UL;
    bool is_deterministic = false;
};

inline bool test_lts(const std::string& test_description,
    const lts::lts_aut_t& l,
    std::size_t expected_label_count,
    std::size_t expected_state_count,
    std::size_t expected_transition_count)
{
  if (l.num_action_labels() != expected_label_count)
  {
    std::cerr << "LPS test: " << test_description << " -----------------------------------------------\n";
    std::cerr << "Expected # of labels " << expected_label_count << " Actual # " << l.num_action_labels() << "\n";
    return false;
  }
  if (l.num_states() != expected_state_count)
  {
    std::cerr << "LPS test: " << test_description << " -----------------------------------------------\n";
    std::cerr << "Expected # of states " << expected_state_count << " Actual # " << l.num_states() << "\n";
    return false;
  }
  if (l.num_transitions() != expected_transition_count)
  {
    std::cerr << "LPS test: " << test_description << " -----------------------------------------------\n";
    std::cerr << "Expected # of transitions " << expected_transition_count << " Actual # " << l.num_transitions() << "\n";
    return false;
  }
  return true;
}

static bool reduce_lts_in_various_ways(const std::string& test_description,
                                       const std::string& lts,
                                       const expected_sizes& expected)
{
  std::cerr << "LPS test: " << test_description << "\n";
  std::istringstream is(lts);
  lts::lts_aut_t l_in;
  l_in.load(is);
  if (!test_lts(test_description + " (plain input)",
          l_in,
          expected.labels_plain,
          expected.states_plain,
          expected.transitions_plain))
  {
    return false;
  }
  lts::lts_aut_t l=l_in;
  reduce(l,lts::lts_eq_none);
  if (!test_lts(test_description + " (no reduction)",
          l,
          expected.labels_plain,
          expected.states_plain,
          expected.transitions_plain))
  {
    return false;
  }
  l=l_in;
  reduce(l,lts::lts_eq_bisim_jgkw);
  if (!test_lts(test_description + " (bisimulation [Jansen/Groote/Keiren/Wijs 2019])",l, expected.labels_bisimulation,expected.states_bisimulation, expected.transitions_bisimulation)) return false;
  l=l_in;
  reduce(l,lts::lts_eq_bisim_gj);
  if (!test_lts(test_description + " (bisimulation [Groote/Jansen 2025])",l, expected.labels_bisimulation,expected.states_bisimulation, expected.transitions_bisimulation)) return false;
  l=l_in;
  reduce(l,lts::lts_eq_bisim_gj_lazy_BLC);
  if (!test_lts(test_description + " (bisimulation [Groote/Jansen 2025] lazy BLC)",l, expected.labels_bisimulation,expected.states_bisimulation, expected.transitions_bisimulation)) return false;
  l=l_in;
  reduce(l,lts::lts_eq_bisim_gv);
  if (!test_lts(test_description + " (bisimulation [Groote/Vaandrager 1990])",
          l,
          expected.labels_bisimulation,
          expected.states_bisimulation,
          expected.transitions_bisimulation))
  {
    return false;
  }
  l=l_in;
  reduce(l,lts::lts_eq_bisim_gjkw);
  if (!test_lts(test_description + " (bisimulation [Groote/Jansen/Keiren/Wijs 2017)",
          l,
          expected.labels_bisimulation,
          expected.states_bisimulation,
          expected.transitions_bisimulation))
  {
    return false;
  }
  l=l_in;
  reduce(l,lts::lts_eq_bisim_sigref);
  if (!test_lts(test_description + " (bisimulation signature [Blom/Orzan 2003])",
          l,
          expected.labels_bisimulation,
          expected.states_bisimulation,
          expected.transitions_bisimulation))
  {
    return false;
  }
  l=l_in;
  reduce(l,lts::lts_eq_branching_bisim_jgkw);
  if (!test_lts(test_description + " (branching bisimulation [Jansen/Groote/Keiren/Wijs 2019])",l, expected.labels_branching_bisimulation,expected.states_branching_bisimulation, expected.transitions_branching_bisimulation)) return false;
  l=l_in;
  reduce(l,lts::lts_eq_branching_bisim_gj);
  if (!test_lts(test_description + " (branching bisimulation [Groote/Jansen 2025])",l, expected.labels_branching_bisimulation,expected.states_branching_bisimulation, expected.transitions_branching_bisimulation)) return false;
  l=l_in;
  reduce(l,lts::lts_eq_branching_bisim_gj_lazy_BLC);
  if (!test_lts(test_description + " (branching bisimulation [Groote/Jansen 2025]) lazy BLC",l, expected.labels_branching_bisimulation,expected.states_branching_bisimulation, expected.transitions_branching_bisimulation)) return false;
  l=l_in;
  reduce(l,lts::lts_eq_branching_bisim_gv);
  if (!test_lts(test_description + " (branching bisimulation [Groote/Vaandrager 1990])",
          l,
          expected.labels_branching_bisimulation,
          expected.states_branching_bisimulation,
          expected.transitions_branching_bisimulation))
  {
    return false;
  }
  l=l_in;
  reduce(l,lts::lts_eq_branching_bisim_gjkw);
  if (!test_lts(test_description + " (branching bisimulation [Groote/Jansen/Keiren/Wijs 2017])",
          l,
          expected.labels_branching_bisimulation,
          expected.states_branching_bisimulation,
          expected.transitions_branching_bisimulation))
  {
    return false;
  }
  l=l_in;
  reduce(l,lts::lts_eq_branching_bisim_sigref);
  if (!test_lts(test_description + " (branching bisimulation signature [Blom/Orzan 2003])",
          l,
          expected.labels_branching_bisimulation,
          expected.states_branching_bisimulation,
          expected.transitions_branching_bisimulation))
  {
    return false;
  }
  l=l_in;
  reduce(l,lts::lts_eq_divergence_preserving_branching_bisim_jgkw);
  if (!test_lts(test_description + " (divergence-preserving branching bisimulation [Jansen/Groote/Keiren/Wijs 2019])",l,
                                      expected.labels_divergence_preserving_branching_bisimulation,
                                      expected.states_divergence_preserving_branching_bisimulation,
                                      expected.transitions_divergence_preserving_branching_bisimulation)) return false;
  l=l_in;
  reduce(l,lts::lts_eq_divergence_preserving_branching_bisim_gj);
  if (!test_lts(test_description + " (divergence-preserving branching bisimulation [Groote/Jansen 2025])",l,
                                      expected.labels_divergence_preserving_branching_bisimulation,
                                      expected.states_divergence_preserving_branching_bisimulation,
                                      expected.transitions_divergence_preserving_branching_bisimulation)) return false;
  l=l_in;
  reduce(l,lts::lts_eq_divergence_preserving_branching_bisim_gj_lazy_BLC);
  if (!test_lts(test_description + " (divergence-preserving branching bisimulation [Groote/Jansen 2025] lazy BLC)",l,
                                      expected.labels_divergence_preserving_branching_bisimulation,
                                      expected.states_divergence_preserving_branching_bisimulation,
                                      expected.transitions_divergence_preserving_branching_bisimulation)) return false;
  l=l_in;
  reduce(l,lts::lts_eq_divergence_preserving_branching_bisim_gv);
  if (!test_lts(test_description + " (divergence-preserving branching bisimulation [Groote/Vaandrager 1990])",
          l,
          expected.labels_divergence_preserving_branching_bisimulation,
          expected.states_divergence_preserving_branching_bisimulation,
          expected.transitions_divergence_preserving_branching_bisimulation))
  {
    return false;
  }
  l=l_in;
  reduce(l,lts::lts_eq_divergence_preserving_branching_bisim_gjkw);
  if (!test_lts(test_description + " (divergence-preserving branching bisimulation [Groote/Jansen/Keiren/Wijs 2017])",
          l,
          expected.labels_divergence_preserving_branching_bisimulation,
          expected.states_divergence_preserving_branching_bisimulation,
          expected.transitions_divergence_preserving_branching_bisimulation))
  {
    return false;
  }
  l=l_in;
  reduce(l,lts::lts_eq_divergence_preserving_branching_bisim_sigref);
  if (!test_lts(test_description + " (divergence-preserving branching bisimulation signature [Blom/Orzan 2003])",
          l,
          expected.labels_divergence_preserving_branching_bisimulation,
          expected.states_divergence_preserving_branching_bisimulation,
          expected.transitions_divergence_preserving_branching_bisimulation))
  {
    return false;
  }
  l=l_in;
  reduce(l,lts::lts_eq_weak_bisim);
  if (!test_lts(test_description + " (weak bisimulation)",
          l,
          expected.labels_weak_bisimulation,
          expected.states_weak_bisimulation,
          expected.transitions_weak_bisimulation))
  {
    return false;
  }
  l=l_in;
  reduce(l,lts::lts_eq_divergence_preserving_weak_bisim);
  if (!test_lts(test_description + " (divergence-preserving weak bisimulation)",
          l,
          expected.labels_divergence_preserving_weak_bisimulation,
          expected.states_divergence_preserving_weak_bisimulation,
          expected.transitions_divergence_preserving_weak_bisimulation))
  {
    return false;
  }
  l=l_in;
  reduce(l,lts::lts_eq_sim);
  if (!test_lts(test_description + " (simulation equivalence)",
          l,
          expected.labels_simulation,
          expected.states_simulation,
          expected.transitions_simulation))
  {
    return false;
  }
  l=l_in;
  reduce(l,lts::lts_eq_trace);
  if (!test_lts(test_description + " (trace equivalence)",
          l,
          expected.labels_trace_equivalence,
          expected.states_trace_equivalence,
          expected.transitions_trace_equivalence))
  {
    return false;
  }
  l=l_in;
  reduce(l,lts::lts_eq_weak_trace);
  if (!test_lts(test_description + " (weak trace equivalence)",
          l,
          expected.labels_weak_trace_equivalence,
          expected.states_weak_trace_equivalence,
          expected.transitions_weak_trace_equivalence))
  {
    return false;
  }
  l=l_in;
  if (expected.is_deterministic)
  {
    if (!is_deterministic(l))
    {
      std::cerr << "Expect the lts to be deterministic, but it is non-deterministic: " << test_description << " \n";
      return false;
    };
  }
  else
  {
    if (is_deterministic(l))
    {
      std::cerr << "Expect the lts to be non-deterministic, but it is deterministic: " << test_description << " \n";
      return false;
    }
  }

  reduce(l,lts::lts_red_determinisation);
  if (!test_lts(test_description + " (determinisation)",
          l,
          expected.labels_determinisation,
          expected.states_determinisation,
          expected.transitions_determinisation))
  {
    return false;
  };
  if (!is_deterministic(l))
  {
    std::cerr << "LTS is non deterministic after deterministation: " << test_description << "\n";
    return false;
  }
  return true;
}

#endif // MCRL2_LTS_TEST_TEST_REDUCTIONS_H
