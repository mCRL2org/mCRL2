// Author(s): Jan Martens and Maurice Laveaux
//
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
/// \file lts/detail/liblts_bisim_m.h
///
/// \brief Partition refinement algorithm for guaruanteed minimal depth
/// counter-examples.
///
/// \details
#ifndef MCRl2_LTS_LIBLTS_BRANCHING_BISIM_MINIMAL_DEPTH
#define MCRl2_LTS_LIBLTS_BRANCHING_BISIM_MINIMAL_DEPTH


#include "mcrl2/lts/detail/liblts_merge.h"
#include "mcrl2/lts/detail/liblts_scc.h"
#include "mcrl2/lts/detail/liblts_bisim_dnj.h"
#include "mcrl2/lts/lts_aut.h"
#include "mcrl2/lts/lts_dot.h"
#include "mcrl2/lts/lts_fsm.h"
#include "mcrl2/lts/lts_utilities.h"
#include "mcrl2/modal_formula/state_formula.h"

#include <fstream>

namespace mcrl2::lts::detail
{

template <class LTS_TYPE>
class branching_bisim_partitioner_minimal_depth
{
public:
  /** \brief Creates a branching bisimulation partitioner for an LTS.
   *  \details This partitioner is specifically for creating minimal depth counter-examples for branching bisimulation.
   *           It guarantees stability w.r.t. the old partition before considering new splitter blocks. This might cause
   *           this implementation to be less efficient than other partitioners. 
   *  \param l Reference to the LTS. 
   *  \param init_l2 reference to the initial state of lts2. 
   */
  branching_bisim_partitioner_minimal_depth(LTS_TYPE &l, const std::size_t init_l2)
      : initial_l2(init_l2),
	      m_lts(l)
  {}
  
  /** \brief Creates a state formula that distinguishes state s from state t.
   *  \details The states s and t are non branching bisimilar states. A distinguishign state formula phi is
   *           returned, which has the property that s \in \sem{phi} and  t \not\in\sem{phi}.
   *           Based on the preprint "Computing minimal distinguishing Hennessey-Milner formulas is NP-hard
   *           But variants are tractable", 2023 by Jan Martens and Jan Friso Groote.
   *  \param[in] s The state number for which the resulting formula should be true
   *  \param[in] t The state number for which the resulting formula should be false
   *  \return A minimal observation depth distinguishing state formula, that is often also minimum negation-depth and
   * irreducible. */
  mcrl2::state_formulas::state_formula dist_formula_mindepth(const std::size_t s, const std::size_t t)
  {
    mCRL2log(mcrl2::log::info) << "done with formula \n";
    return mcrl2::state_formulas::state_formula();
  };


private:  
  LTS_TYPE& m_lts;
  state_type initial_l2;
  state_type max_state_index = 0;
};

template < class LTS_TYPE>
bool destructive_branching_bisimulation_compare_minimal_depth(
    LTS_TYPE & l1,
    LTS_TYPE & l2,
    const std::string & counter_example_file /*= ""*/,
    const bool /*structured_output = false */)
{
  std::size_t init_l2 = l2.initial_state() + l1.num_states();
  mcrl2::lts::detail::merge(l1, l2);
  l2.clear(); // No use for l2 anymore.
      
  // First remove tau loops in case of branching bisimulation
  bool preserve_divergences = false;
  detail::scc_partitioner<LTS_TYPE> scc_part(l1);
  scc_part.replace_transition_system(preserve_divergences);
  init_l2 = scc_part.get_eq_class(init_l2);
  
  // Run a faster branching bisimulation algorithm as preprocessing, no preversing of loops.
  detail::bisim_partitioner_dnj<LTS_TYPE> branching_bisim_part(l1, true, preserve_divergences, false);
  if (branching_bisim_part.in_same_class(l1.initial_state(), init_l2))
  {
      return true; 
  }
  
  
  branching_bisim_partitioner_minimal_depth<LTS_TYPE> branching_bisim_min(l1, init_l2);
  if (branching_bisim_min.in_same_class(l1.initial_state(), init_l2))
  {
      return true; 
  }

  // LTSs are not bisimilar, we can create a counter example. 
  std::string filename = "Counterexample.mcf";
  if (!counter_example_file.empty()) {
      filename = counter_example_file;
  }

  mcrl2::state_formulas::state_formula counter_example_formula = branching_bisim_min.dist_formula_mindepth(l1.initial_state(), init_l2);
    
  std::ofstream counter_file(filename);
  counter_file << mcrl2::state_formulas::pp(counter_example_formula);
  mCRL2log(mcrl2::log::info) << "Saved counterexample to: \"" << filename << "\"" << std::endl;
  return false;
}

} // namespace mcrl2::lts::detail

#endif // MCRl2_LTS_LIBLTS_BRANCHING_BISIM_MINIMAL_DEPTH