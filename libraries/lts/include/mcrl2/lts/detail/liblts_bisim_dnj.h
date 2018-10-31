// Author(s): David N. Jansen, Institute of Software, Chinese Academy of
// Sciences, Beijing, PR China
//
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/// \file lts/detail/liblts_bisim_dnj.h
///
/// \brief O(m log n)-time branching bisimulation algorithm
///
/// \details This file declares an efficient partition refinement algorithm
/// for labelled transition systems inspired by Groote / Jansen / Keiren / Wijs
/// (2017) and Valmari (2009) to calculate the branching bisimulation classes
/// of a labelled transition system.  Different from the 2017 article, it does
/// not translate the LTS to a Kripke structure, but works on the LTS directly.
/// We hope that in this way the memory use can be reduced.
///
/// Partition refinement means that the algorithm maintains a partition of the
/// state space of the LTS into ``blocks''.  A block contains all states in one
/// or several branching bisimulation equivalence classes.  Blocks are being
/// refined until every block contains exactly one branching bisimulation
/// equivalence class.
///
/// The algorithm divides the non-inert transitions into *action_block-slices.*
/// An action_block-slice contains all transitions with a given action label
/// into one block.  One or more action_block-slices are joined into a *bunch.*
/// Bunches register which blocks have already been stabilised:
///
/// > Invariant:  The blocks are stable under the bunches, i. e. if a block
/// > has a transition in a bunch, then every bottom state in this block has
/// > a transition in this bunch.
///
/// However, if a bunch is non-trivial (i. e., it contains more than one
/// action_block-slice), its slices may split some blocks into finer parts:
/// not all states may have a transition in the same action_block-slice.  So, a
/// refinement step consists of moving one small action_block-slice from a
/// non-trivial bunch to a new bunch and then splitting blocks to restore the
/// Invariant.  Splitting is always done by finding a small subblock and move
/// the states in that subblock to a new block.  Note that we always handle a
/// small part of some larger unit; that ensures that each state and transition
/// is only touched O(log n) times.
///
/// After splitting blocks, some inert transitions may have become non-inert.
/// These transitions mostly need to be inserted into their own bunch.  Also,
/// some states may have lost all their inert transitions and become new bottom
/// states.  These states need to be handled as well to reestablish the
/// Invariant.
///
/// Overall, we spend time as follows:
/// - Every transition is moved to a new bunch at most
///   log<SUB>2</SUB>(n<SUP>2</SUP>) = 2*log<SUB>2</SUB>(n) times.  Every move
///   leads to O(1) work.
/// - Every state is moved to a new block at most log<SUB>2</SUB>(n) times.
///   Every move leads to work proportional to the number of its incoming and
///   outgoing transitions.
/// - Every state becomes a new bottom state at most once.  When this happens,
///   this leads to work proportional to log(n) times the number of its
///   incoming and outgoing transitions.
/// When summing this up over all states and transitions, we get that the
/// algorithm spends time in O(m log n), where m is the number of transitions
/// and n ist the number of states.
///
/// \author David N. Jansen, Institute of Software, Chinese Academy of
/// Sciences, Beijing, PR China

#ifndef LIBLTS_BISIM_DNJ_H
#define LIBLTS_BISIM_DNJ_H

#include <cstddef>
#include <vector>

namespace mcrl2
{
namespace lts
{
namespace detail
{





/* ************************************************************************* */
/*                                                                           */
/*                             I N T E R F A C E                             */
/*                                                                           */
/* ************************************************************************* */





/// \defgroup part_interface
/// \brief nonmember functions serving as interface with the rest of mCRL2
/// \details These functions are copied, almost without changes, from
/// liblts_bisim_gw.h, which was written by Anton Wijs.
///@{

/// \brief Reduce transition system l with respect to strong or
/// (divergence-preserving) branching bisimulation.
/// \param[in,out] l                   The transition system that is reduced.
/// \param         branching           If true branching bisimulation is
///                                    applied, otherwise strong bisimulation.
/// \param         preserve_divergence Indicates whether loops of internal
///                                    actions on states must be preserved. If
///                                    false these are removed. If true these
///                                    are preserved.
/// \param[out]    arbitrary_state_per_block    If this pointer is != nullptr,
///                                    the function fills the vector with, per
///                                    equivalence class, the number of an
///                                    arbitrary original state in the class.
template <class LTS_TYPE>
void bisimulation_reduce_dnj(LTS_TYPE& l, bool branching = false,
                bool preserve_divergence = false,
                std::vector<std::size_t>* arbitrary_state_per_block = nullptr);


/// \brief Checks whether the two initial states of two LTSs are strong or
/// branching bisimilar.
/// \details This routine uses the O(m log n) branching bisimulation algorithm
/// developed in 2018 by David N. Jansen.  It runs in O(m log n) time and uses
/// O(n) memory, where n is the number of states and m is the number of
/// transitions.
///
/// The LTSs l1 and l2 are not usable anymore after this call.
/// \param[in,out] l1                  A first transition system.
/// \param[in,out] l2                  A second transistion system.
/// \param         branching           If true branching bisimulation is used,
///                                    otherwise strong bisimulation is
///                                    applied.
/// \param         preserve_divergence If true and branching is true, preserve
///                                    tau loops on states.
/// \returns True iff the initial states of the transition systems l1 and l2
/// are (divergence-preserving) (branching) bisimilar.
template <class LTS_TYPE>
bool destructive_bisimulation_compare_dnj(LTS_TYPE& l1, LTS_TYPE& l2,
                bool branching = false, bool preserve_divergence = false,
                                       bool generate_counter_examples = false);


/// \brief Checks whether the two initial states of two LTSs are strong or
/// branching bisimilar.
/// \details The LTSs l1 and l2 are first duplicated and subsequently reduced
/// modulo bisimulation.  If memory is a concern, one could consider to use
/// destructive_bisimulation_compare().  This routine uses the O(m log n)
/// branching bisimulation algorithm developed in 2018 by David N. Jansen.  It
/// runs in O(m log n) time and uses O(n) memory, where n is the number of
/// states and m is the number of transitions.
/// \param l1                  A first transition system.
/// \param l2                  A second transistion system.
/// \param branching           If true branching bisimulation is used,
///                            otherwise strong bisimulation is applied.
/// \param preserve_divergence If true and branching is true, preserve tau
///                            loops on states.
/// \retval True iff the initial states of the transition systems l1 and l2
/// are (divergence-preserving) (branching) bisimilar.
template <class LTS_TYPE>
inline bool bisimulation_compare_dnj(const LTS_TYPE& l1, const LTS_TYPE& l2,
          bool const branching = false, bool const preserve_divergence = false)
{
    LTS_TYPE l1_copy(l1);
    LTS_TYPE l2_copy(l2);
    return destructive_bisimulation_compare_dnj(l1_copy, l2_copy, branching,
                                                          preserve_divergence);
}

///@} (end of group part_interface)

} // end namespace detail
} // end namespace lts
} // end namespace mcrl2

#endif // ifndef LIBLTS_BISIM_DNJ_H
