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
  branching_bisim_partitioner_minimal_depth(LTS_TYPE& l, const std::size_t init_l2)
      : initial_l2(init_l2),
        m_lts(l)
  {
    // Initialize data structures
    std::map<state_type, std::size_t> state2num_silent_in;
    std::map<state_type, std::size_t> state2num_silent_out;
    std::map<state_type, std::size_t> state2num_trans_out;

    for (state_type s = 0; s < m_lts.num_states(); s++)
    {
      state2block[s] = 0;
      state2num_touched[s] = 0;
      state2num_silent_in[s] = 0;
      state2num_silent_out[s] = 0;
      state2num_trans_out[s] = 0;
    }

    for (transition trans : m_lts.get_transitions())
    {
      state2num_trans_out[trans.from()] += 1;
      if (m_lts.is_tau(m_lts.apply_hidden_label_map(trans.label())))
      {
        state2num_silent_in[trans.to()] += 1;
        state2num_silent_out[trans.from()] += 1;
      }
    }
    for (state_type s = 0; s < m_lts.num_states(); s++)
    {
      if (state2num_silent_out[s] == 0)
      {
        bottom_states.push_back(s);
      }
      trans_out[s] = std::vector<observation>(state2num_trans_out[s]);
      silent_out[s] = std::set<state_type>();
      silent_in[s] = std::set<state_type>();
    }

    for (transition trans : m_lts.get_transitions())
    {
      trans_out[trans.from()].push_back(std::make_pair(trans.label(), trans.to()));
      if (m_lts.is_tau(m_lts.apply_hidden_label_map(trans.label())))
      {
        silent_out[trans.from()].insert(trans.to());
        silent_in[trans.to()].insert(trans.from());
      }
    }

    // Initialize the partition with a single block.
    blocks.push_back(block());
    blocks[0].state_index = 0;
    blocks[0].block_index = 0;
    blocks[0].parent_block_index = 0;
    blocks[0].level = 0;

    // Now we can start refining the partition.
    std::size_t num_old_blocks = 0;
    std::size_t num_blocks_created = 1;
    std::size_t level = 0;
    while (num_blocks_created > num_old_blocks)
    {
      level += 1;
      num_old_blocks = num_blocks_created;
      num_blocks_created = refine_partition();
      mCRL2log(mcrl2::log::info) << "Refined partition to " << num_blocks_created << " blocks on level " << level
                                 << "." << std::endl;
    }
  }

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
  }

  bool in_same_class(const std::size_t s, const std::size_t t)
  {
	return state2block[s] == state2block[t];
  }


private:
  LTS_TYPE& m_lts;
  state_type initial_l2;
  state_type max_state_index = 0;
  typedef std::size_t block_index_type;
  typedef std::size_t level_type;
  // This tuple is meant for an observation (s', a, s'') such that s -(silent)-> s' -a-> s''.
  typedef std::tuple<block_index_type, label_type, block_index_type> branching_observation_type;
  typedef std::set<branching_observation_type> signature_type;
  typedef std::pair<label_type, state_type> observation;

  std::map<state_type, std::set<state_type>> silent_in;
  std::map<state_type, std::set<state_type>> silent_out;
  std::map<state_type, std::vector<observation>> trans_out;
  std::map<state_type, std::size_t> state2num_touched;
  std::map<state_type, block_index_type> state2block;
  std::map<state_type, signature_type> state2sig;
  std::vector<state_type> bottom_states;
  struct block
  {
    state_type state_index;       // The state number that represent the states in this block
    block_index_type block_index; // The sequence number of this block.
    block_index_type
        parent_block_index; // Index of the parent block. If there is no parent block, this refers to the block itself.
    level_type level;       // The level of the block in the partition.

    void swap(block& b)
    {
      std::swap(b.state_index, state_index);
      std::swap(b.block_index, block_index);
      std::swap(b.parent_block_index, parent_block_index);
      std::swap(b.level, level);
    }
  };
  std::vector<block> blocks;
  
  signature_type get_signature(state_type s)
  {
	signature_type sig;
    // Add the block index of the state to the signature.
    sig.insert(std::make_tuple(state2block[s], m_lts.tau_label_index(), state2block[s]));

    for (state_type target : silent_out[s])
	{
      sig.insert(state2sig[target].begin(), state2sig[target].end());
	}
    for (observation t : trans_out[s])
	{
      if (t.first != m_lts.tau_label_index() || state2block[s] != state2block[t.second])
      {
        sig.insert(std::make_tuple(state2block[s], t.first, state2block[t.second]));
      }
	}
	state2sig[s] = sig;
	return sig;
  }

  // Refine the partition exactly one level.
  std::size_t refine_partition()
  {
    std::queue<state_type> frontier;
    // start with bottom states.  
    for (auto s : bottom_states)
    {
	  frontier.push(s);
    }
    std::map<signature_type, block_index_type> sig2block;
    std::map<state_type, block_index_type> state2block_new;
    std::size_t num_blocks_created = 0;
    //Compute signatures in order of bottom states and reachability order.
    //Debug info for frontier
    int num_added_to_frontier = 0;
    while (!frontier.empty())
    {
      state_type state = frontier.front();
      frontier.pop();
      signature_type sig = get_signature(state);
      if (sig2block.find(sig) == sig2block.end())
	  {
        // Create the new block
        blocks.push_back(block());
        num_blocks_created += 1;
        block_index_type new_block_id = blocks.size() -1;    
        sig2block[sig] = new_block_id;
        blocks[new_block_id].state_index = state;
        blocks[new_block_id].block_index = new_block_id;
        blocks[new_block_id].parent_block_index = state2block[state];
        blocks[new_block_id].level = blocks[state2block[state]].level + 1;
	  }
      state2block_new[state] = sig2block[sig];
      state2num_touched[state] = 0;

      for (state_type backward : silent_in[state])
      {
        size_t max_out = silent_out[backward].size(); 
        state2num_touched[backward] += 1;
        if (state2num_touched[backward] == max_out) {
          num_added_to_frontier += 1;
          frontier.push(backward);
        }
      }
    }
    mCRL2log(mcrl2::log::info) << "Added " << num_added_to_frontier << " states to the frontier." << std::endl;
    // Now we have computed the new blocks, we can redefine the partition.
    state2block = state2block_new;
    return num_blocks_created; 
  }
};

template <class LTS_TYPE>
bool destructive_branching_bisimulation_compare_minimal_depth(LTS_TYPE& l1,
    LTS_TYPE& l2,
    const std::string& counter_example_file /*= ""*/,
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
  if (false)
  {
    detail::bisim_partitioner_dnj branching_bisim_part(l1, true, preserve_divergences);
    init_l2 = branching_bisim_part.get_eq_class(init_l2);
    branching_bisim_part.finalize_minimized_LTS();
    if (branching_bisim_part.in_same_class(l1.initial_state(), init_l2))
    {
      return true;
    }
    init_l2 = branching_bisim_part.get_eq_class(init_l2);
  }

  // Log that we continue to the slower partition refinement.
  mCRL2log(mcrl2::log::info) << "Starting minimal depth partition refinement." << std::endl;

  branching_bisim_partitioner_minimal_depth<LTS_TYPE> branching_bisim_min(l1, init_l2);

  if (branching_bisim_min.in_same_class(l1.initial_state(), init_l2))
  {
    assert(false);
    // This should should not have happened.
    return true;
  }

  // LTSs are not bisimilar, we can create a counter example.
  std::string filename = "Counterexample.mcf";
  if (!counter_example_file.empty())
  {
    filename = counter_example_file;
  }

  mcrl2::state_formulas::state_formula counter_example_formula
      = branching_bisim_min.dist_formula_mindepth(l1.initial_state(), init_l2);

  std::ofstream counter_file(filename);
  counter_file << mcrl2::state_formulas::pp(counter_example_formula);
  mCRL2log(mcrl2::log::info) << "Saved counterexample to: \"" << filename << "\"" << std::endl;
  return false;
}

} // namespace mcrl2::lts::detail
#endif // MCRl2_LTS_LIBLTS_BRANCHING_BISIM_MINIMAL_DEPTH