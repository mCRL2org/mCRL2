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
#include "mcrl2/lts/lts.h"
#include <vector>
#include <map>

namespace mcrl2
{
namespace lts
{
namespace detail
{

class bisim_partitioner
{ 

  public:
    /** Creates a partitioner for an LTS.
     * \param[in] l Pointer to the LTS. */
    bisim_partitioner(
             mcrl2::lts::lts &l, 
             const bool branching=false, 
             const bool preserve_divergence=false);

    /** Destroys this partitioner. */
    ~bisim_partitioner();

    /** Gives the transition relation on the computed equivalence
     * classes of the LTS. The label numbers of the transitions
     * correspond to the label numbers of the LTS that was passed as an
     * argument to the constructor of this partitioner.
     * The state numbers of the transitions are the equivalence class
     * numbers which range from 0 upto (and excluding) \ref num_eq_classes().
     *
     * \pre The bisimulation equivalence classes have been computed.
     * \param[out] nt Used to store the number of transitions between the
     * bisimulation equivalence classes.
     * \param[out] size Used to store the length of the returned array.
     * \return An array containing the transitions between the
     * bisimulation equivalence classes. */
    void replace_transitions(const bool branching, const bool preserve_divergences);

    /** Gives the number of bisimulation equivalence classes of the LTS.
     * \pre The bisimulation equivalence classes have been computed.
     * \return The number of bisimulation equivalence classes of the LTS.
     */
    unsigned int num_eq_classes() const;

    /** Gives the equivalence class number of a state.
     * The equivalence class numbers range from 0 upto (and excluding)
     * \ref num_eq_classes().
     * \pre The bisimulation equivalence classes have been computed.
     * \param[in] s A state number.
     * \return The number of the equivalence class to which \e s
     * belongs. */
    unsigned int get_eq_class(const unsigned int s) const;

    /** Returns whether two states are in the same bisimulation
     * equivalence class.
     * \pre The bisimulation equivalence classes have been computed.
     * \param[in] s A state number.
     * \param[in] t A state number.
     * \retval true if \e s and \e t are in the same bisimulation
     * equivalence class;
     * \retval false otherwise. */
    bool in_same_class(const unsigned int s, const unsigned int t) const;

  private:

    typedef unsigned int block_index_type;
    typedef unsigned int state_type;
    typedef unsigned int label_type;

    mcrl2::lts::lts &aut;
    bool partition_has_been_calculated;

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
    { block_index_type block_index;
      std::vector < state_type > bottom_states; // The non bottom states must be ordered
                                                // on tau reachability. The deepest
                                                // states occur first in the vector.
      std::vector < non_bottom_state > non_bottom_states;
      // The non_inert transitions are grouped per label. The (non-inert) transitions
      // with tau labels are at the end of this vector.
      std::vector < transition > non_inert_transitions; 

      void swap(block &b)
      { block_index_type block_index1=b.block_index;
        b.block_index=block_index;
        block_index=block_index1;
        bottom_states.swap(b.bottom_states);
        non_bottom_states.swap(b.non_bottom_states);
        non_inert_transitions.swap(b.non_inert_transitions);
      }
    };

    std::vector < block > blocks;

    std::vector < state_type > block_index_of_a_state;
    std::vector < bool > block_flags;
    std::vector < bool > block_is_in_to_be_processed;
    std::vector < bool > state_flags;

    std::vector< block_index_type > to_be_processed;
    std::vector< block_index_type > BL; 
    label_type tau_label;
    bool preserve_divergences_loops;

    void create_initial_partition(const bool branching, 
                                  const bool preserve_divergences);
    void refine_partition_until_it_becomes_stable(const bool preserve_divergence);
    void refine_partion_with_respect_to_divergences(void);
    void split_the_blocks_in_BL(bool &);
    void order_recursively_on_tau_reachability(
                                 const state_type s,
                                 std::map < state_type, std::vector < state_type > > &inert_transition_map,
                                 std::vector < non_bottom_state > &new_non_bottom_states,
                                 std::set < state_type > &visited);
    void order_on_tau_reachability(std::vector < non_bottom_state > &non_bottom_states);

#ifndef NDEBUG
    void check_internal_consistency_of_the_partitioning_data_structure(const bool preserve_divergence) const;
#endif

};
#endif
}
}
}
