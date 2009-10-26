// Author(s): Muck van Weerdenburg, Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lts/detail/liblts_scc.h

#ifndef _LIBLTS_SCC_H
#define _LIBLTS_SCC_H
#include "mcrl2/lts/lts.h"
#include <vector>
#include <map>

namespace mcrl2
{
namespace lts
{

namespace detail
{
/// \brief This class contains an scc partitioner removing inert tau loops.
class scc_partitioner
{ 

  public:
    /** Creates an scc partitioner for an LTS.
     * \param[in] l reference to an LTS. */
    scc_partitioner(mcrl2::lts::lts &l);

    /** Destroys this partitioner. */
    ~scc_partitioner();

    /** Computes the strong bisimulation equivalence classes 
     *  of the LTS. */
//     void remove_tau_loops_algorithm(void);

    /** Computes the branching bisimulation equivalence classes
     *  of the LTS. */
//    void remove_tau_loops_but_preserve_divergence_algorithm(void);

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
     * \param[in] preserve_divergence_loops If true preserve a tau loop on states that
     *     were part of a larger tau loop in the input transition system. Otherwise idle
     *     tau loops are removed.
     * \return An array containing the transitions between the
     * bisimulation equivalence classes. */
     void replace_transitions(const bool preserve_divergence_loops);

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

    typedef unsigned int state_type;
    typedef unsigned int label_type;

    mcrl2::lts::lts &aut;

    std::vector < state_type > block_index_of_a_state;
    std::vector < state_type > dfsn2state;
    state_type equivalence_class_index;
    label_type tau_label;

    void group_components(const state_type t, 
                          const state_type equivalence_class_index,
                          const std::map < state_type, std::vector < state_type > > &tgt_src,
                          std::vector < bool > &visited);
    void dfs_numbering(const state_type t, 
                       const std::map < state_type, std::vector < state_type > > &src_tgt,
                       std::vector < bool > &visited);
    
#ifndef NDEBUG
    void check_internal_consistency_of_the_partitioning_data_structure(void) const;
#endif

};
#endif // _LIBLTS_SCC_H
}
}
}
