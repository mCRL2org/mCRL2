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
#include <vector>
#include <map>
#include "mcrl2/lts/lts.h"
#include "mcrl2/utilities/logger.h"

namespace mcrl2
{
namespace lts
{

namespace detail
{
/// \brief This class contains an scc partitioner removing inert tau loops.

template < class LTS_TYPE>
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
    scc_partitioner(LTS_TYPE& l);

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
    size_t num_eq_classes() const;

    /** \brief Gives the equivalence class number of a state.
     *  The equivalence class numbers range from 0 upto (and excluding)
     *  \ref num_eq_classes().
     *  \param[in] s A state number.
     *  \return The number of the equivalence class to which \e s
     *    belongs. */
    size_t get_eq_class(const size_t s) const;

    /** \brief Returns whether two states are in the same bisimulation
     *     equivalence class.
     * \param[in] s A state number.
     * \param[in] t A state number.
     * \retval true if \e s and \e t are in the same bisimulation
     *    equivalence class;
     * \retval false otherwise. */
    bool in_same_class(const size_t s, const size_t t) const;

  private:

    typedef size_t state_type;
    typedef size_t label_type;

    LTS_TYPE& aut;

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


template < class LTS_TYPE>
scc_partitioner<LTS_TYPE>::scc_partitioner(LTS_TYPE& l)
  :aut(l)
{
  mCRL2log(log::debug) << "Tau loop (SCC) partitioner created for " << l.num_states() << " states and " <<
              l.num_transitions() << " transitions" << std::endl;

  // read and store tau transitions.
  std::map < state_type, std::vector < state_type > > src_tgt;
  const std::vector <transition> &trans=aut.get_transitions();
  for (std::vector <transition>::const_iterator r=trans.begin(); r!=trans.end(); ++r)
  {
    if (aut.is_tau(r->label()))
    {
      src_tgt[r->from()].push_back(r->to());
    }
  }
  // Initialise the data structures
  std::vector<bool> visited(aut.num_states(),false);

  // Number the states via a depth first search
  for (state_type i=0; i<aut.num_states(); ++i)
  {
    dfs_numbering(i,src_tgt,visited);
  }
  src_tgt.clear();

  std::map < state_type, std::vector < state_type > > tgt_src;
  for (std::vector <transition>::const_iterator r=trans.begin(); r!=trans.end(); ++r)
  {
    if (aut.is_tau(r->label()))
    {
      tgt_src[r->to()].push_back(r->from());
    }
  }
  equivalence_class_index=0;
  block_index_of_a_state=std::vector < state_type >(aut.num_states(),0);
  for (std::vector < state_type >::reverse_iterator i=dfsn2state.rbegin();
       i!=dfsn2state.rend(); ++i)
  {
    if (visited[*i])  // Visited is used inversely here.
    {
      group_components(*i,equivalence_class_index,tgt_src,visited);
      equivalence_class_index++;
    }
  }
  mCRL2log(log::debug) << "Tau loop (SCC) partitioner reduces lts to " << equivalence_class_index << " states." << std::endl;

  dfsn2state.clear();
}

template < class LTS_TYPE>
scc_partitioner<LTS_TYPE>::~scc_partitioner()
{
}

template < class LTS_TYPE>
void scc_partitioner<LTS_TYPE>::replace_transitions(const bool preserve_divergence_loops)
{
  // Put all the non inert transitions in a set. Add the transitions that form a self
  // loop. Such transitions only exist in case divergence preserving branching bisimulation is
  // used. A set is used to remove double occurrences of transitions.
  std::set < transition > resulting_transitions;
  const std::vector <transition> &trans=aut.get_transitions();
  for (std::vector <transition>::const_iterator r=trans.begin(); r!=trans.end(); ++r)
  {
    if (!aut.is_tau(r->label()) ||
        preserve_divergence_loops ||
        block_index_of_a_state[r->from()]!=block_index_of_a_state[r->to()])
    {
      resulting_transitions.insert(
        transition(
          block_index_of_a_state[r->from()],
          r->label(),
          block_index_of_a_state[r->to()]));
    }
  }

  aut.clear_transitions();
  // Copy the transitions from the set into the transition system.

  for (std::set < transition >::const_iterator i=resulting_transitions.begin();
       i!=resulting_transitions.end(); ++i)
  {
    aut.add_transition(transition(i->from(),i->label(),i->to()));
  }
}

template < class LTS_TYPE>
size_t scc_partitioner<LTS_TYPE>::num_eq_classes() const
{
  return equivalence_class_index;
}

template < class LTS_TYPE>
size_t scc_partitioner<LTS_TYPE>::get_eq_class(const size_t s) const
{
  return block_index_of_a_state[s];
}

template < class LTS_TYPE>
bool scc_partitioner<LTS_TYPE>::in_same_class(const size_t s, const size_t t) const
{
  return get_eq_class(s)==get_eq_class(t);
}

// Private methods of scc_partitioner

template < class LTS_TYPE>
void scc_partitioner<LTS_TYPE>::group_components(
  const state_type t,
  const state_type equivalence_class_index,
  const std::map < state_type, std::vector < state_type > > &tgt_src,
  std::vector < bool > &visited)
{
  if (!visited[t])
  {
    return;
  }
  {
    visited[t] = false;
    if (tgt_src.count(t)>0)
    {
      const std::vector < state_type > &sources = tgt_src.find(t)->second;
      for (std::vector < state_type >::const_iterator i=sources.begin();
           i!=sources.end(); ++i)
      {
        group_components(*i,equivalence_class_index,tgt_src,visited);
      }
    }
    block_index_of_a_state[t]=equivalence_class_index;
  }
}

template < class LTS_TYPE>
void scc_partitioner<LTS_TYPE>::dfs_numbering(
  const state_type t,
  const std::map < state_type, std::vector < state_type > > &src_tgt,
  std::vector < bool > &visited)
{
  if (visited[t])
  {
    return;
  }
  visited[t] = true;
  if (src_tgt.count(t)>0)
  {
    const std::vector < state_type > &targets = src_tgt.find(t)->second;
    for (std::vector < state_type >::const_iterator i=targets.begin();
         i!=targets.end() ; ++i)
    {
      dfs_numbering(*i,src_tgt,visited);
    }
  }
  dfsn2state.push_back(t);
}

} // namespace detail

template < class LTS_TYPE>
void scc_reduce(LTS_TYPE& l,const bool preserve_divergence_loops = false)
{
  detail::scc_partitioner<LTS_TYPE> scc_part(l);
  scc_part.replace_transitions(preserve_divergence_loops);
  l.set_num_states(scc_part.num_eq_classes());
  l.set_initial_state(scc_part.get_eq_class(l.initial_state()));
}

}
}
#endif // _LIBLTS_SCC_H
