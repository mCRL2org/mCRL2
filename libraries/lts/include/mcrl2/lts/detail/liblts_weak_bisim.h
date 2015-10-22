// Author(s): Jan Friso Groote (upon request by Matthew Hennessy)
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lts/detail/liblts_weak_bisim.h
/// \brief This file defines an algorithm for weak bisimulation, by 
///        calculating the transitive tau closure and apply strong
///        bisimulation afterwards. In order to apply this algorithm
///        it is advisable to first apply a branching bisimulation reduction.

#ifndef _LIBLTS_WEAK_BISIM_H
#define _LIBLTS_WEAK_BISIM_H
#include <cmath>
#include <vector>
#include <map>
#include "mcrl2/utilities/logger.h"
#include "mcrl2/lts/lts.h"
#include "mcrl2/lts/detail/liblts_scc.h"
#include "mcrl2/lts/detail/liblts_tau_star_reduce.h"
#include "mcrl2/lts/detail/liblts_merge.h"
#include "mcrl2/lts/lts_lts.h"
#include "mcrl2/lts/lts_aut.h"
#include "mcrl2/lts/lts_fsm.h"
#include "mcrl2/lts/lts_dot.h"

namespace mcrl2
{
namespace lts
{
namespace detail
{

/** \brief Reduce transition system l with respect to strong or (divergence preserving) branching bisimulation.
 * \param[in/out] l The transition system that is reduced.
 * \param[in] branching If true branching bisimulation is applied, otherwise strong bisimulation.
 * \param[in] preserve_divergences Indicates whether loops of internal actions on states must be preserved. If false
 *            these are removed. If true these are preserved.  */
template < class LTS_TYPE>
void weak_bisimulation_reduce(
  LTS_TYPE& l,
  const bool preserve_divergences = false,
  const bool use_groote_wijs_algorithm = false)
{
  bisimulation_reduce(l,true,preserve_divergences);        // Apply branching bisimulation to l.
  size_t divergence_label;
  if (preserve_divergences)
  {
    divergence_label=mark_explicit_divergence_transitions(l);
  }
  reflexive_transitive_tau_closure(l);                    // Apply transitive tau closure to l.
  if (use_groote_wijs_algorithm)
  {
    bisimulation_reduce_gw(l,false,false);                     // Apply strong bisimulation to l.
  }
  else
  {
    bisimulation_reduce(l,false,false);                     // Apply strong bisimulation to l.
  }
  scc_reduce(l);                                          // Remove tau loops.
  if (preserve_divergences)
  {
    unmark_explicit_divergence_transitions(l,divergence_label);
  }
}


/** \brief Checks whether the two initial states of two lts's are strong or branching bisimilar.
 * \details This lts and the lts l2 are not usable anymore after this call.
 *          The space consumption is O(n) and time is O(nm). It uses the branching bisimulation
 *          algorithm by Groote and Vaandrager from 1990.
 * \param[in/out] l1 A first transition system.
 * \param[in/out] l2 A second transistion system.
 * \param[preserve_divergences] If true and branching is true, preserve tau loops on states.
 * \retval True iff the initial states of the current transition system and l2 are (divergence preserving) (branching) bisimilar */
template < class LTS_TYPE>
bool destructive_weak_bisimulation_compare(
  LTS_TYPE& l1,
  LTS_TYPE& l2,
  const bool preserve_divergences=false) 
{
  weak_bisimulation_reduce(l1,preserve_divergences);
  weak_bisimulation_reduce(l2,preserve_divergences);
  return destructive_bisimulation_compare(l1,l2);
}


/** \brief Checks whether the two initial states of two lts's are strong or branching bisimilar.
 *  \details The current transitions system and the lts l2 are first duplicated and subsequently
 *           reduced modulo bisimulation. If memory space is a concern, one could consider to
 *           use destructive_weak_bisimulation_compare. This routine uses the Groote-Vaandrager
 *           branching bisimulation routine. It runs in O(mn) and uses O(n) memory where n is the
 *           number of states and m is the number of transitions.
 * \param[in/out] l1 A first transition system.
 * \param[in/out] l2 A second transistion system.
 * \param[branching] If true branching bisimulation is used, otherwise strong bisimulation is applied.
 * \param[preserve_divergences] If true and branching is true, preserve tau loops on states.
 * \retval True iff the initial states of the current transition system and l2 are (divergence preserving) (branching) bisimilar */
template < class LTS_TYPE>
bool weak_bisimulation_compare(
  const LTS_TYPE& l1,
  const LTS_TYPE& l2)
{
  LTS_TYPE l1_copy(l1);
  LTS_TYPE l2_copy(l2);
  return destructive_weak_bisimulation_compare(l1_copy,l2_copy);
}

}
}
}
#endif  // #define _LIBLTS_WEAK_BISIM_H
