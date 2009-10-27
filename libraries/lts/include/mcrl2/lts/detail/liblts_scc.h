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
    /** \brief Creates an scc partitioner for an LTS.
     *  \details This scc partitioner calculates a partition
     *  of the state space of the transition system l using
     *  a straightforward algorithm found in A.V. Aho, J.E. Hopcroft
     *  and J.D. Ullman, Data structures and algorithms. Addison Wesley,
     *  1987 on page 224. All states that reside on a loop of internal
     *  actions are put in the same equivalence class. The function l.is_tau
     *  is used to determine whether an action is internal. Partitioning is
     *  done immediately when an instance of this class is created.
     *  When applying the function \ref replace_transitions the
     *  automaton l is replaced by (aka shrinked to) the automaton modulo the 
     *  calculated partition.
     *  \param[in] l reference to an LTS. */
    scc_partitioner(mcrl2::lts::lts &l);

    /** \brief Destroys this partitioner. */
    ~scc_partitioner();

    /** \brief The lts for which this partioner is created is replaced by the lts modulo
     *        the calculated partition.  
     *  \details The number of states of the new lts becomes the number of 
     *        equivalence classes. In each transition the start and end state
     *        are replaced by its equivalence class. If a transition has a tau
     *        label (which is checked using the function l.is_tau) then it
     *        is preserved if either from and to state are different, or 
     *        \e preserve_divergence_loops is true. All non tau transitions are
     *        always preserved. The label numbers for preserved transitions are
     *        not changed. Note that this routine does not adapt the number of
     *        states or the initial state of the lts.
     *
     * \param[in] preserve_divergence_loops If true preserve a tau loop on states that
     *     were part of a larger tau loop in the input transition system. Otherwise idle
     *     tau loops are removed. */
    void replace_transitions(const bool preserve_divergence_loops);

    /** \brief Gives the number of bisimulation equivalence classes of the LTS.
     *  \return The number of bisimulation equivalence classes of the LTS.
     */
    unsigned int num_eq_classes() const;

    /** \brief Gives the equivalence class number of a state.
     *  The equivalence class numbers range from 0 upto (and excluding)
     *  \ref num_eq_classes().
     *  \param[in] s A state number.
     *  \return The number of the equivalence class to which \e s
     *    belongs. */
    unsigned int get_eq_class(const unsigned int s) const;

    /** \brief Returns whether two states are in the same bisimulation
     *     equivalence class.
     * \param[in] s A state number.
     * \param[in] t A state number.
     * \retval true if \e s and \e t are in the same bisimulation
     *    equivalence class;
     * \retval false otherwise. */
    bool in_same_class(const unsigned int s, const unsigned int t) const;

  private:

    typedef unsigned int state_type;
    typedef unsigned int label_type;

    mcrl2::lts::lts &aut;

    std::vector < state_type > block_index_of_a_state;
    std::vector < state_type > dfsn2state;
    state_type equivalence_class_index;

    void group_components(const state_type t, 
                          const state_type equivalence_class_index,
                          const std::map < state_type, std::vector < state_type > > &tgt_src,
                          std::vector < bool > &visited);
    void dfs_numbering(const state_type t, 
                       const std::map < state_type, std::vector < state_type > > &src_tgt,
                       std::vector < bool > &visited);
    
};
#endif // _LIBLTS_SCC_H
}
}
}
