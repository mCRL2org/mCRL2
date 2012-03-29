// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file sigref.h
/// \brief Implementation of LTS reductions using the signature refinement
///        approach of S. Blom and S. Orzan.
/// \author Jeroen Keiren

#ifndef MCRL2_LTS_SIGREF_H
#define MCRL2_LTS_SIGREF_H

#include <set>
#include <map>
#include <iostream>
#include "mcrl2/lts/lts.h"
#include "mcrl2/lts/lts_utilities.h"
#include "mcrl2/utilities/logger.h"

namespace mcrl2
{
namespace lts
{

/** \brief A signature is a pair of an action label and a block */
typedef std::set<std::pair<size_t, size_t> > signature_t;

/** \brief Base class for signature computation */
template < class LTS_T >
class signature
{
protected:
  /** \brief The labelled transition system for which the signature is computed */
  const LTS_T& m_lts;

  /** \brief Signature stored per state */
  std::vector<signature_t> m_sig;

public:
  /** \brief Constructor
    */
  signature(const LTS_T& lts_)
    : m_lts(lts_), m_sig(m_lts.num_states(), signature_t())
  {}

  /** \brief Compute a new signature based on \a partition.
    * \param[in] partition The current partition
    */
  virtual void compute_signature(const std::vector<size_t>& partition) = 0;

  /** \brief Compute the transitions for the quotient according to \a partition.
    * \param[in] partition The partition that is used to compute the quotient
    * \param[out] transitions A set to which the transitions of the quotient are written
    */
  virtual void quotient_transitions(std::set<transition>& transitions, const std::vector<size_t>& partition)
  {
    for(std::vector<transition>::const_iterator i = m_lts.get_transitions().begin(); i != m_lts.get_transitions().end(); ++i)
    {
      transitions.insert(transition(partition[i->from()], i->label(), partition[i->to()]));
    }
  }

  /** \brief Return the signature for state \a i.
    * \param[in] i The state for which to return the signature.
    * \pre i < m_lts.num_states().
    */
  virtual const signature_t& get_signature(size_t i) const
  {
    return m_sig[i];
  }
};

/** \brief Class for computing the signature for strong bisimulation */
template < class LTS_T >
class signature_bisim: public signature<LTS_T>
{
protected:
  using signature<LTS_T>::m_lts;
  using signature<LTS_T>::m_sig;

public:
  /** \brief Constructor */
  signature_bisim(const LTS_T& lts_)
    : signature<LTS_T>(lts_)
  {
    mCRL2log(log::verbose, "sigref") << "initialising signature computation for strong bisimulation" << std::endl;
  }

  /** \overload */
  virtual void
  compute_signature(const std::vector<size_t>& partition)
  {
    // compute signatures
    m_sig = std::vector<signature_t>(m_lts.num_states(), signature_t());
    for(std::vector<transition>::const_iterator i = m_lts.get_transitions().begin(); i != m_lts.get_transitions().end(); ++i)
    {
      m_sig[i->from()].insert(std::make_pair(i->label(), partition[i->to()]));
    }
  }

};

/** \brief Class for computing the signature for branching bisimulation */
template < class LTS_T >
class signature_branching_bisim: public signature<LTS_T>
{
protected:
  using signature<LTS_T>::m_lts;
  using signature<LTS_T>::m_sig;

  /** \brief Store the incoming transitions per state */
  outgoing_transitions_per_state_t m_prev_transitions;

  /** \brief Insert function
    * \param[in] partition The current partition
    * \param[in] t source state
    * \param[in] label_ transition label
    * \param[in] block target block
    *
    * Insert function as described in S. Blom, S. Orzan,
    * "Distributed Branching Bisimulation Reduction of State Spaces",
    * Proc. PDMC 2003.
    *
    * Inserts the pair (label_, block) in the signature of t, as well as
    * the signatures of all tau-predecessors of t within the same block.
    */
  void insert(const std::vector<size_t>& partition, const size_t t, const size_t label_, const size_t block)
  {
    if(m_sig[t].insert(std::make_pair(label_, block)).second)
    {
      std::pair<outgoing_transitions_per_state_t::const_iterator, outgoing_transitions_per_state_t::const_iterator> pred_range
          = m_prev_transitions.equal_range(t);

      for(outgoing_transitions_per_state_t::const_iterator i = pred_range.first; i != pred_range.second; ++i)
      {
        assert(from(i) == t);
        if(m_lts.is_tau(label(i)) && partition[from(i)] == partition[to(i)])
        {
          insert(partition, to(i), label_, block);
        }
      }
    }
  }

public:
  /** \brief Constructor  */
  signature_branching_bisim(const LTS_T& lts_)
    : signature<LTS_T>(lts_),
      m_prev_transitions(transitions_per_outgoing_state_reversed(lts_.get_transitions()))
  {
    mCRL2log(log::verbose, "sigref") << "initialising signature computation for branching bisimulation" << std::endl;
  }

  /** \overload */
  virtual void compute_signature(const std::vector<size_t>& partition)
  {
    // compute signatures
    m_sig = std::vector<signature_t>(m_lts.num_states(), signature_t());
    for(std::vector<transition>::const_iterator i = m_lts.get_transitions().begin(); i != m_lts.get_transitions().end(); ++i)
    {
      if (!(m_lts.is_tau(i->label()) && (partition[i->from()] == partition[i->to()])))
      {
        insert(partition, i->from(), i->label(), partition[i->to()]);
      }
    }
  }

  /** \overload */
  virtual void quotient_transitions(std::set<transition>& transitions, const std::vector<size_t>& partition)
  {
    for(std::vector<transition>::const_iterator i = m_lts.get_transitions().begin(); i != m_lts.get_transitions().end(); ++i)
    {
      if(partition[i->from()] != partition[i->to()] || !m_lts.is_tau(i->label()))
      {
        transitions.insert(transition(partition[i->from()], i->label(), partition[i->to()]));
      }
    }
  }
};

/** \brief Class for computing the signature for divergence preserving branching bisimulation */
template < class LTS_T >
class signature_divergence_preserving_branching_bisim: public signature_branching_bisim<LTS_T>
{
protected:
  using signature_branching_bisim<LTS_T>::m_lts;
  using signature_branching_bisim<LTS_T>::m_sig;
  using signature_branching_bisim<LTS_T>::insert;

  /** \brief Record for each vertex whether it is in a tau-scc */
  std::vector<bool> m_divergent;

  /** \brief Iterative implementation of Tarjan's SCC algorithm.
   *
   * based on an earlier implementation by Sjoerd Cranen
   *
   * For the non-trivial tau-sccs (i.e. SCCs with more than one state, or
   * single-state SCCs with a tau-loop, we set \a m_divergent to true.
   */
  void compute_tau_sccs()
  {
    size_t unused = 1, lastscc = 1;
    std::vector<size_t> scc(m_lts.num_states(), 0);
	  std::vector<size_t> low(m_lts.num_states(), 0);
	  std::stack<size_t> stack;
	  std::stack<size_t> sccstack;

    // Record forward transition relation sorted by state.
	  outgoing_transitions_per_state_t m_lts_succ_transitions(transitions_per_outgoing_state(m_lts.get_transitions()));

	  for (size_t i = 0; i < m_lts.num_states(); ++i)
	  {
		  if (scc[i] == 0)
			  stack.push(i);
		  while (!stack.empty())
		  {
			  size_t vi = stack.top();

        // Outgoing transitions of vi.
        std::pair<outgoing_transitions_per_state_t::const_iterator, outgoing_transitions_per_state_t::const_iterator> succ_range
          = m_lts_succ_transitions.equal_range(vi);

			  if (low[vi] == 0 && scc[vi] == 0)
			  {
				  scc[vi] = unused;
				  low[vi] = unused++;
				  sccstack.push(vi);

				  for (outgoing_transitions_per_state_t::const_iterator t = succ_range.first; t != succ_range.second; ++t)
				  {
            if ((low[to(t)] == 0) and (scc[to(t)] == 0) && (m_lts.is_tau(label(t))))
					  {
						  stack.push(to(t));
						}
				  }
			  }
			  else
			  {
				  for (outgoing_transitions_per_state_t::const_iterator t = succ_range.first; t != succ_range.second; ++t)
				  {
					  if ((low[to(t)] != 0) && (m_lts.is_tau(label(t))))
						  low[vi] = low[vi] < low[to(t)] ? low[vi] : low[to(t)];
				  }
				  if (low[vi] == scc[vi])
				  {
					  size_t tos, scc_id = lastscc++;
            std::vector<size_t> this_scc;
					  do
					  {
						  tos = sccstack.top();
						  low[tos] = 0;
						  scc[tos] = scc_id;
						  sccstack.pop();
              this_scc.push_back(tos);
					  }
					  while (tos != vi);

            // We only consider non-trivial sccs, hence
            // if the scc consists of a single schate, check whether it has a tau-loop
            if(this_scc.size() == 1)
            {
              for(outgoing_transitions_per_state_t::const_iterator i = succ_range.first; i != succ_range.second; ++i)
              {
                if(from(i) == to(i) && m_lts.is_tau(label(i)))
                {
                  m_divergent[tos] = true;
                  break;
                }
              }
            }
            else
            {
              m_divergent[tos] = true;
            }
				  }
				  stack.pop();
			  }
		  }
	  }
  }

public:
  /** \brief Constructor
    *
    * This initialises \a m_divergent to record for each vertex whether it is
    * in a tau-scc.
    */
  signature_divergence_preserving_branching_bisim(const LTS_T& lts_)
    : signature_branching_bisim<LTS_T>(lts_),
      m_divergent(lts_.num_states(), false)
  {
    mCRL2log(log::verbose, "sigref") << "initialising signature computation for divergence preserving branching bisimulation" << std::endl;
    compute_tau_sccs();
  }

  /** \overload
    *
    * Compute the signature as in branching bisimulation. In addition, add the
    * (tau, B) for edges s -tau-> t for which s,t in B and m_divergent[t]
    */
  virtual void compute_signature(const std::vector<size_t>& partition)
  {
    // compute signatures
    m_sig = std::vector<signature_t>(m_lts.num_states(), signature_t());
    for(std::vector<transition>::const_iterator i = m_lts.get_transitions().begin(); i != m_lts.get_transitions().end(); ++i)
    {
      if(!(partition[i->from()] == partition[i->to()] && m_lts.is_tau(i->label()))
         || m_divergent[i->to()])
      {
        insert(partition, i->from(), i->label(), partition[i->to()]);
      }
    }
  }

  /** \overload */
  virtual void quotient_transitions(std::set<transition>& transitions, const std::vector<size_t>& partition)
  {
    for(std::vector<transition>::const_iterator i = m_lts.get_transitions().begin(); i != m_lts.get_transitions().end(); ++i)
    {
      if(!(partition[i->from()] == partition[i->to()] && m_lts.is_tau(i->label()))
         || m_sig[i->from()].find(std::make_pair(i->label(), partition[i->to()])) != m_sig[i->from()].end())
      {
        transitions.insert(transition(partition[i->from()], i->label(), partition[i->to()]));
      }
    }
  }
};


/** \brief Signature based reductions for labelled transition systems.
  *
  * The implementation is based on the description in
  * S. Blom, S. Orzan. "Distributed Branching Bisimulation Reduction of State
  * Spaces", in Proc. PDMC 2003.
  *
  * The specific signature is a parameter of the algorithm.
  */
template < class LTS_T, typename Signature >
class sigref
{

protected:
  /** \brief Current partition; for each state (size_t) the block in which
             it resides is recorded. */
  std::vector<size_t> m_partition;

  /** \brief The number of blocks in the current partition */
  size_t m_count;

  /** \brief The LTS that we are reducing */
  LTS_T& m_lts;

  /** \brief Instance of a class performing the signature computation for the
             current equivalence */
  Signature m_signature;

  /** \brief Print a signature (for debugging purposes) */
  std::string print_sig(const signature_t& sig)
  {
    std::stringstream os;
    os << "{ ";
    for(signature_t::const_iterator i = sig.begin(); i != sig.end() ; ++i)
    {
      os << " (" << pp(m_lts.action_label(i->first)) << ", " << i->second << ") ";
    }
    os << " }";
    return os.str();
  }

  /** \brief Compute the partition. Repeatedly updates the signatures, and
             the partition, until the partition stabilises */
  void compute_partition()
  {
    size_t count_prev = m_count;
    size_t iterations = 0;

    m_lts.sort_transitions(mcrl2::lts::lbl_tgt_src);

    do
    {
      mCRL2log(log::verbose, "sigref") << "Iteration " << iterations
                                       << " currently have " << m_count << " blocks" << std::endl;

      m_signature.compute_signature(m_partition);

      count_prev = m_count;

      // Map signatures to block numbers
      std::map<signature_t, size_t> hashtable;
      m_count = 0;
      for(size_t i = 0; i < m_lts.num_states(); ++i)
      {
        if(hashtable.find(m_signature.get_signature(i)) == hashtable.end())
        {
          mCRL2log(log::debug, "sigref") << "Adding block for signature " << print_sig(m_signature.get_signature(i)) << std::endl;
          hashtable[m_signature.get_signature(i)] = m_count++;
        }
      }

      // Map states to block numbers
      for(size_t i = 0; i < m_lts.num_states(); ++i)
      {
        m_partition[i] = hashtable[m_signature.get_signature(i)];
      }

      ++iterations;

    } while (count_prev != m_count);

    mCRL2log(log::verbose, "sigref") << "Done after " << iterations << " iterations with " << m_count << " blocks" << std::endl;
  }

  /** \brief Perform the quotient with respect to the partition that has
             been computed */
  void quotient()
  {
    // Assign the reduced LTS
    m_lts.set_num_states(m_count);
    m_lts.set_initial_state(m_partition[m_lts.initial_state()]);

    // Compute quotient transitions
    // implemented in the signature class because it differs per equivalence.
    std::set<transition> transitions;
    m_signature.quotient_transitions(transitions, m_partition);

    // Set quotient transitions
    m_lts.clear_transitions();
    for(std::set<transition>::const_iterator i = transitions.begin(); i != transitions.end(); ++i)
    {
      m_lts.add_transition(*i);
    }
  }

public:
  /** \brief Constructor
    * \param[in] lts_ The LTS that is being reduced
    */
  sigref(LTS_T& lts_)
    : m_partition(std::vector<size_t>(lts_.num_states(), 0)),
      m_count(0),
      m_lts(lts_),
      m_signature(lts_)
  {}

  /** \brief Perform the reduction, modulo the equivalence for which the
    *        signature has been passed in as template parameter
    */
  void run()
  {
    // No need for state labels in the reduced LTS.
    m_lts.clear_state_labels();
    compute_partition();
    quotient();
  }
};

} // namespace lts
} // namespace mcrl2

#endif // MCRL2_LTS_SIGREF_H
