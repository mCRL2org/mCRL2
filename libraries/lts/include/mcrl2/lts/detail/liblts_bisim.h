// Author(s): Muck van Weerdenburg, Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lts/detail/liblts_bisim.h

#ifndef _LIBLTS_BISIM_H
#define _LIBLTS_BISIM_H
#include <vector>
#include <map>
#include "mcrl2/lts/lts.h"
#include "mcrl2/trace/trace.h"
#include "mcrl2/lts/lts_utilities.h"

namespace mcrl2
{
namespace lts
{
namespace detail
{

class bisim_partitioner
{ 

  public:
    /** \brief Creates a bisimulation partitioner for an LTS.
     *  \details This bisimulation partitioner applies the algorithm
     *  defined in J.F. Groote and F.W. Vaandrager. An efficient algorithm for branching bisimulation and stuttering 
     *  equivalence. In M.S. Paterson, editor, Proceedings 17th ICALP, Warwick, volume 443 of Lecture Notes in Computer 
     *  Science, pages 626-638. Springer-Verlag, 1990. The only difference is that this algorithm uses actions labels
     *  on transitions. Therefore, each list of non_inert_transitions is grouped such that all transitions with the
     *  same label are grouped together. Tau transitions (which have as label index the number of labels) occur at the
     *  beginning of this list.
     *    
     *  If branching is true, then branching bisimulation is used, otherwise strong bisimulation is applied.
     *  If preserve_divergence is true, then branching must be true. In this case states with an internal tau loop
     *  are considered to be different from states without a tau loop. In this way the divergences are preserved.
     *
     *  The input transition system is not allowed to contain tau loops, except that if preserve_divergence is true
     *  tau loops on a single state are allowed as they indicate divergences. Using the scc partitioner the tau
     *  loops must first be removed before applying this algorithm.
     *  \warning Note that when compiled with optimisations, bisimulation partitioning
     *  is much faster than compiled without any optimisation. The difference can go up to a factor 10.
     *  \param[in] l Reference to the LTS. The LTS l is only changed if \ref replace_transitions is called. */
    bisim_partitioner(
             mcrl2::lts::lts &l, 
             const bool branching=false, 
             const bool preserve_divergence=false);

    /** \brief Destroys this partitioner. */
    ~bisim_partitioner();

    /** \brief Replaces the transition relation of the current lts by the transitions
     *         of the bisimulation reduced transition system.
     * \details Each transition (s,l,s') is replaced by a transition (t,l,t') where
     * t and t' are the equivalence classes to which classes of the LTS. If the label l is
     * internal, which is detected using the function is_tau, then it is only returned
     * if t!=t' or preserve_divergence=true. This effectively removes all inert transitions.
     * Duplicates are removed from the transitions in the new lts.
     * Note that the number of states nor the initial state are not adapted by this method.
     * These must be set separately.
     *
     * \pre The bisimulation equivalence classes have been computed.
     * \param[in] branching Causes non internal transitions to be removed.
     * \param[in] preserve_divergences Preserves tau loops on states. */
    void replace_transitions(const bool branching, const bool preserve_divergences);

    /** \brief Gives the number of bisimulation equivalence classes of the LTS.
     *  \return The number of bisimulation equivalence classes of the LTS.
     */
    unsigned int num_eq_classes() const;

    /** \brief Gives the bisimulation equivalence class number of a state.
     *  \param[in] s A state number.
     *  \return The number of the bisimulation equivalence class to which \e s belongs. */
    unsigned int get_eq_class(const unsigned int s) const;

    /** \brief Returns whether two states are in the same bisimulation equivalence class.
     *  \param[in] s A state number.
     *  \param[in] t A state number.
     *  \retval true if \e s and \e t are in the same bisimulation equivalence class;
     *  \retval false otherwise. */
    bool in_same_class(const unsigned int s, const unsigned int t) const;

    /** \brief Returns a vector of counter traces.
     *  \details The states s and t are non bisimilar states. If they are
     *           bisimilar an exception is raised. A counter trace of the form sigma a is
     *           returned, which has the property that s-sigma->s'-a-> and t-sigma->t'-/a->,
     *           or vice versa, s-sigma->s'-/a-> and t-sigma->t'-a->. A vector of such
     *           counter traces is returned.
     *  \param[in] s A state number.
     *  \param[in] t A state number.
     *  \return A vector containing counter traces. */
    std::vector < mcrl2::trace::Trace > counter_traces(const unsigned int s, const unsigned int t);

  private:

    typedef unsigned int block_index_type;
    typedef unsigned int state_type;
    typedef unsigned int label_type;

    state_type max_state_index;
    mcrl2::lts::lts &aut;

    struct non_bottom_state
    { state_type state;
      std::vector < state_type > inert_transitions; // Only the target state is interesting.

      non_bottom_state(const state_type s)
                        : state(s)
      {}
      non_bottom_state(const state_type s, const std::vector < state_type > &it)
                        : state(s), inert_transitions(it)
      {}
    };

    struct block
    { 
      state_type state_index;                   // The state number that represent the states in this block
      block_index_type block_index;             // The sequence number of this block.
      block_index_type parent_block_index;      // Index of the parent block. 
                                                // If there is no parent block, this refers to the block
                                                // itself.
      std::pair < label_type, block_index_type > splitter; 
                                                // The action and block that caused this block to split.
                                                // This information is only defined if the block has been split.
      std::vector < state_type > bottom_states; // The non bottom states must be ordered
                                                // on tau reachability. The deepest
                                                // states occur first in the vector.
      std::vector < non_bottom_state > non_bottom_states;
      // The non_inert transitions are grouped per label. The (non-inert) transitions
      // with tau labels are at the end of this vector.
      std::vector < transition > non_inert_transitions; 

      void swap(block &b)
      { 
        state_type state_index1=b.state_index;
        b.state_index=state_index;
        state_index=state_index1;

        block_index_type block_index1=b.block_index;
        b.block_index=block_index;
        block_index=block_index1;

        block_index_type parent_block_index1=b.parent_block_index;
        b.parent_block_index=parent_block_index;
        parent_block_index=parent_block_index1;

        std::pair < label_type, block_index_type > splitter1=b.splitter;
        b.splitter=splitter;
        splitter=splitter1;
        bottom_states.swap(b.bottom_states);
        non_bottom_states.swap(b.non_bottom_states);
        non_inert_transitions.swap(b.non_inert_transitions);
      }
    };

    std::vector < block > blocks;

    std::vector < bool > block_is_active;       // Indicates whether this is still a block in the partition.
                                                // Blocks that are split become inactive.
    std::vector < state_type > block_index_of_a_state;
    std::vector < bool > block_flags;
    std::vector < bool > block_is_in_to_be_processed;
    std::vector < bool > state_flags;

    std::vector< block_index_type > to_be_processed;
    std::vector< block_index_type > BL; 
    const label_type tau_label;

    void create_initial_partition(const bool branching, 
                                  const bool preserve_divergences);
    void refine_partition_until_it_becomes_stable(const bool branching, const bool preserve_divergence);
    void refine_partion_with_respect_to_divergences(void);
    void split_the_blocks_in_BL(bool &, const label_type, const block_index_type splitter_block);
    void order_recursively_on_tau_reachability(
                                 const state_type s,
                                 std::map < state_type, std::vector < state_type > > &inert_transition_map,
                                 std::vector < non_bottom_state > &new_non_bottom_states,
                                 std::set < state_type > &visited);
    void order_on_tau_reachability(std::vector < non_bottom_state > &non_bottom_states);
    std::vector < mcrl2::trace::Trace > counter_traces_aux(
                           const unsigned int s,
                           const unsigned int t,
                           const mcrl2::lts::outgoing_transitions_per_state_action_t &outgoing_transitions) const;

#ifndef NDEBUG
    void check_internal_consistency_of_the_partitioning_data_structure(const bool branching, const bool preserve_divergence) const;
#endif

};


 /** \brief Reduce transition system l with respect to strong or (divergence preserving) branching bisimulation.
  * \param[in/out] l The transition system that is reduced.
  * \param[in] branching If true branching bisimulation is applied, otherwise strong bisimulation.
  * \param[in] preserve_divergences Indicates whether loops of internal actions on states must be preserved. If false
  *            these are removed. If true these are preserved.  */
 void bisimulation_reduce(
            lts &l,
            const bool branching = false,
            const bool preserve_divergences = false);


 /** \brief Checks whether the two initial states of two lts's are strong or branching bisimilar.
  * \details This lts and the lts l2 are not usable anymore after this call.
  *          The space consumption is O(n) and time is O(nm). It uses the branching bisimulation
  *          algorithm by Groote and Vaandrager from 1990.
  * \param[in/out] l1 A first transition system.
  * \param[in/out] l2 A second transistion system.
  * \param[branching] If true branching bisimulation is used, otherwise strong bisimulation is applied.
  * \param[preserve_divergences] If true and branching is true, preserve tau loops on states.
  * \retval True iff the initial states of the current transition system and l2 are (divergence preserving) (branching) bisimilar */
 bool destructive_bisimulation_compare(
          lts &l1,
          lts &l2, 
          const bool branching=false, 
          const bool preserve_divergences=false,
          const bool generate_counter_examples = false );


 /** \brief Checks whether the two initial states of two lts's are strong or branching bisimilar.
  *  \details The current transitions system and the lts l2 are first duplicated and subsequently
  *           reduced modulo bisimulation. If memory space is a concern, one could consider to
  *           use destructive_bisimulation_compare. This routine uses the Groote-Vaandrager
  *           branching bisimulation routine. It runs in O(mn) and uses O(n) memory where n is the
  *           number of states and m is the number of transitions.
  * \param[in/out] l1 A first transition system.
  * \param[in/out] l2 A second transistion system.
  * \param[branching] If true branching bisimulation is used, otherwise strong bisimulation is applied.
  * \param[preserve_divergences] If true and branching is true, preserve tau loops on states.
  * \retval True iff the initial states of the current transition system and l2 are (divergence preserving) (branching) bisimilar */
 bool bisimulation_compare(
          const lts &l1,
          const lts &l2, 
          const bool branching=false, 
          const bool preserve_divergences=false,
          const bool generate_counter_examples = false); 

}
}
}
#endif
