// Author(s): Jan Friso Groote (with edits by David N. Jansen)
//
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/// \file lts/detail/liblts_bisim_gj.h
///
/// \brief O(m log n)-time branching bisimulation algorithm similar to liblts_bisim_dnj.h
///        which does not use bunches, i.e., partitions of transitions. This algorithm
///        should be slightly faster, but in particular use less memory than liblts_bisim_dnj.h.
///        Otherwise the functionality is exactly the same.

// TODO:
// Merge identifying whether there is a splitter and actual splitting (Done. No performance effect).
// Use BLC lists for the main split.
// Maintain a co-splitter and a splitter to enable a co-split.
// Eliminate two pointers in transition_type (done).
// JFG: Optimise swap. 

#ifndef LIBLTS_BISIM_GJ_H
#define LIBLTS_BISIM_GJ_H

#include <iomanip> // for std::fixed, std::setprecision(), std::setw()
#include <ctime> // for std::clock_t, std::clock()
//#include "mcrl2/utilities/hash_utility.h"
#include "mcrl2/lts/detail/liblts_scc.h"
#include "mcrl2/lts/detail/liblts_merge.h"
#include "mcrl2/lts/detail/check_complexity.h"
#include "mcrl2/lts/detail/fixed_vector.h"
#include "mcrl2/lts/detail/simple_list.h"
#define linked_list simple_list

// Decided on 2025-01-06:
// JFG will try whether USE_INCLUDE_CO and XCLUDECO_AND_HIT_MAIN bring great
// performance advantages but otherwise we shall remove them to simplify the
// code.

// If `USE_FOUR_WAY_SPLIT` is defined, then splitting a block will use a
// four-way split function that can split under a main and a co-splitter at the
// same time, together with creating the new bottom state block.
#define USE_FOUR_WAY_SPLIT

// The following two are sub-options for `USE_FOUR_WAY_SPLIT`.
// If (in addition to `USE_FOUR_WAY_SPLIT`) one defines `USE_INCLUDE_CO`, then
// the MultiSub coroutine will try to avoid waiting by marking all states with
// a transition in the co-splitter as NcludeCo (but only in turns when MultiSub
// has nothing else to do), so that they can be handled faster by XcludeCo
// later.
//#define USE_INCLUDE_CO

// If (in addition to `USE_FOUR_WAY_SPLIT`) one defines `XCLUDECO_AND_HIT_MAIN`
// then the XcludeCo coroutine will remember whether a state has a transition
// in the main splitter (i.e. it is marked Hit_Main in the beginning) to avoid
// searching through the outgoing transitions later.
//#define XCLUDECO_AND_HIT_MAIN

// A very small experiment did not show significant differences; there may
// be a less-than-significant tendency for the base version of the four-way
// split (better than the versions with additional features), and a
// less-than-significant tendency for the new linked list.

namespace mcrl2
{
namespace lts
{
namespace detail
{

template <class LTS_TYPE> class bisim_partitioner_gj;

namespace bisimulation_gj
{

// Forward declaration.
struct state_type_gj;
struct block_type;
struct constellation_type;
struct transition_type;
struct outgoing_transition_type;

typedef std::size_t state_index;
typedef std::size_t transition_index;


typedef std::size_t label_index;
typedef fixed_vector<outgoing_transition_type>::iterator
                                                       outgoing_transitions_it;
typedef fixed_vector<outgoing_transition_type>::const_iterator
                                                 outgoing_transitions_const_it;

constexpr constellation_type* null_constellation=nullptr;
constexpr transition_index null_transition=-1;
constexpr label_index null_action=-1;
constexpr state_index null_state=-1;
constexpr block_type* null_block=nullptr;
constexpr transition_index undefined=0;
constexpr transition_index Rmarked=-1;
constexpr transition_index Umarked=1;

#ifdef USE_FOUR_WAY_SPLIT
  /// \brief the number of counter values that can be used for one subblock
  /// \details There are three or four singular values (`undefined`,
  /// `marked_MultiSub`, `marked_Hit_Main` and possibly `marked_NcludeCo`),
  /// and the other values needs to be distributed over three or four subblocks
  /// (ReachAlw, XcludeCo, MissMain and possibly XcludeCo_and_Hit_Main).
  constexpr transition_index marked_range=
    (std::numeric_limits<transition_index>::max()-2
                                                 #ifdef USE_INCLUDE_CO
                                                   -1
                                                 #endif
                                                 )/
                                                   #ifdef XCLUDECO_AND_HIT_MAIN
                                                     4;
                                                   #else
                                                     3;
                                                   #endif

  enum subblocks { ReachAlw=0,// states that can reach always all splitters
                   MissMain,  // states that cannot inertly reach the main
                              // splitter (while it is not empty)
                   XcludeCo,  // states that cannot inertly reach the
                              // co-splitter (while it is not empty)
                   #ifdef XCLUDECO_AND_HIT_MAIN
                     XcludeCo_and_Hit_Main, // states that cannot inertly
                              // reach the co-splitter (while it is not
                              // empty), but that have a strong transition
                              // in the main splitter
                   #endif
                   MultiSub}; // states that can inertly reach multiple of
                              // the above subblocks
                // The following values are used only for temporary marking
                // and are not really associated with a subblock:
                // Hit_Main -- states that can (non-inertly) reach the main
                //             splitter; they can be in any subblock except
                //             MissMain.  Necessary for correctness.
                // NcludeCo -- states that can (non-inertly) reach the
                //             co-splitter; they can be in any subblock
                //             except XcludeCo.  Optional.

  /// \brief base marking value for a subblock
  /// \details If the counter has this value, the state definitely belongs
  /// to the respective subblock.
  static inline constexpr transition_index marked(enum subblocks subblock)
  {
    return
                                                                                #ifdef XCLUDECO_AND_HIT_MAIN
                                                                                  assert(ReachAlw==subblock || MissMain==subblock || XcludeCo==subblock ||
                                                                                         XcludeCo_and_Hit_Main==subblock || MultiSub==subblock),
                                                                                #else
                                                                                  assert(ReachAlw==subblock || MissMain==subblock || XcludeCo==subblock ||
                                                                                                                            MultiSub==subblock),
                                                                                #endif
           marked_range*subblock+1;
  }

  constexpr transition_index marked_MultiSub=marked(MultiSub);                  static_assert(marked_MultiSub<std::numeric_limits<transition_index>::max());
  constexpr transition_index marked_Hit_Main=marked_MultiSub+1;
  #ifdef USE_INCLUDE_CO
    constexpr transition_index marked_NcludeCo=marked_Hit_Main+1;               static_assert(marked_Hit_Main<std::numeric_limits<transition_index>::max());
  #endif

  /// \brief checks whether a counter value is a marking for a given subblock
  static inline constexpr bool is_in_marked_range_of
                            (transition_index counter, enum subblocks subblock)
  {
    return                                                                      assert(ReachAlw==subblock || MissMain==subblock || XcludeCo==subblock),
    #ifdef XCLUDECO_AND_HIT_MAIN
           XcludeCo==subblock
               ? counter-marked(subblock)<2*marked_range
               :
    #endif
                 counter-marked(subblock)<marked_range;
  }
#endif

/// The function clear() takes care that a container frees memory when it is
/// cleared and it is large.
template <class CONTAINER>
static inline void clear(CONTAINER& c)
{
  if (c.size()>1000) { c=CONTAINER(); } else { c.clear(); }
}

// The struct below facilitates to walk through a LBC_list starting from an
// arbitrary transition.
typedef transition_index* BLC_list_iterator; // should not be nullptr
typedef transition_index* BLC_list_iterator_or_null; // can be nullptr
typedef const transition_index* BLC_list_const_iterator; // should not be nullptr

/// information about a transition stored in m_outgoing_transitions
struct outgoing_transition_type
{
  /// pointer to the corresponding entry in m_BLC_transitions
  union iterator_or_counter
  {
    /// \brief transition index (used during initialisation)
    transition_index transitions;
    /// \brief pointer to the corresponding entry in `m_BLC_transitions` (used during main part of the algorithm)
    BLC_list_iterator BLC_transitions;
    /// \brief Construct the object as a transition index
    iterator_or_counter()
      : transitions()
    {}
    /// \brief Convert the object from counter to iterator
    void convert_to_iterator(const BLC_list_iterator other)
    {
      new (&BLC_transitions) BLC_list_iterator(other);
    }
    /// \brief Destruct the object as an iterator
    ~iterator_or_counter()  {  BLC_transitions.~BLC_list_iterator();  }
  } ref;

  /// this pointer is used to find transitions with the same source state, action label, and target constellation
  /// (Transitions are grouped according to these in m_outgoing_transitions.)
  /// For most transitions, it points to the last transition with the same source state, action label, and target constellation;
  /// but if this transition is the last one in the group, start_same_saC points to the first transition in the group.
  outgoing_transitions_it start_same_saC;

  // The default initialiser does not initialize the fields of this struct.
  outgoing_transition_type()
  {}

  outgoing_transition_type(const outgoing_transitions_it sssaC)
   : ref(),
     start_same_saC(sssaC)
  {}
};

/// a pointer to a state, i.e. a reference to a state
struct state_in_block_pointer
{
  state_in_block_pointer(fixed_vector<state_type_gj>::iterator new_ref_state)
   : ref_state(new_ref_state)
  {}

  state_in_block_pointer()
  {}

  fixed_vector<state_type_gj>::iterator ref_state;

  bool operator==(const state_in_block_pointer& other) const
  {
    return ref_state==other.ref_state;
  }

  bool operator!=(const state_in_block_pointer& other) const
  {
    return ref_state!=other.ref_state;
  }
};

/// a vector with an additional (internal) field to indicate how much work has been
/// done already on it.
class todo_state_vector
{
  std::size_t m_todo_indicator=0;
  std::vector<state_in_block_pointer> m_vec;

  public:
    typedef std::vector<state_in_block_pointer>::const_iterator const_iterator;
                                                                                #ifndef NDEBUG
                                                                                  bool find(const state_in_block_pointer s) const
                                                                                  {
                                                                                    return std::find(m_vec.begin(), m_vec.end(), s)!=m_vec.end();
                                                                                  }
                                                                                #endif
    void add_todo(const state_in_block_pointer s)
    {                                                                           assert(!find(s));
      m_vec.push_back(s);
    }

    std::size_t todo_is_empty() const
    {
      return m_vec.size()==m_todo_indicator;
    }

    // Move a state from the todo part to the definitive vector.
    state_in_block_pointer move_from_todo()
    {                                                                           assert(!todo_is_empty());
      state_in_block_pointer result=m_vec[m_todo_indicator];
      m_todo_indicator++;
      return result;
    }

    void swap_vec(std::vector<state_in_block_pointer>& other_vec)
    {
      m_vec.swap(other_vec);
      m_todo_indicator=0;
    }

    std::size_t size() const
    {
      return m_vec.size();
    }

    std::size_t empty() const
    {
      return m_vec.empty();
    }

    const_iterator begin() const
    {
      return m_vec.begin();
    }

    const_iterator end() const
    {
      return m_vec.end();
    }

    const state_in_block_pointer* data() const
    {
      return m_vec.data();
    }

    const state_in_block_pointer* data_end() const
    {
      return m_vec.data() + m_vec.size();
    }

    const state_in_block_pointer& front() const
    {
      return m_vec.front();
    }

    //const state_in_block_pointer& back() const
    //{
    //  return m_vec.back();
    //}

#ifdef USE_FOUR_WAY_SPLIT
    void reserve(std::vector<state_in_block_pointer>::size_type new_cap)
    {
      m_vec.reserve(new_cap);
    }

    typedef std::vector<state_in_block_pointer>::iterator iterator;

    iterator begin()
    {
      return m_vec.begin();
    }

    iterator end()
    {
      return m_vec.end();
    }

    // add all elements in [begin, end) to the vector
    void add_todo(iterator begin, iterator end)
    {
      m_vec.insert(m_vec.end(), begin, end);
    }
#endif

    void clear()
    {
      m_todo_indicator=0;
      bisimulation_gj::clear(m_vec);
    }
};



// Below the four main data structures are listed.
/// information about a state
struct state_type_gj
{
  /// block of the state
  block_type* block=null_block;
  /// first incoming transition
  std::vector<transition>::iterator start_incoming_transitions;
  /// first outgoing transition
  outgoing_transitions_it start_outgoing_transitions;
  /// pointer to the corresponding entry in m_states_in_blocks
  state_in_block_pointer* ref_states_in_blocks;
  /// number of outgoing block-inert transitions
  transition_index no_of_outgoing_block_inert_transitions=0;
  /// counter used during splitting
  /// If this counter is set to undefined (-1), it is considered to be not yet
  /// visited.
  /// If this counter is set to Rmarked (-2), the state is considered to be in
  /// the R-subblock.
  /// If this counter is a positive number, it is the number of outgoing
  /// block-inert transitions that have not yet been handled.
  /// If this counter is 0, the state is considered to be in the U-subblock.
  transition_index counter=undefined;
                                                                                #ifndef NDEBUG
                                                                                  /// \brief print a short state identification for debugging
                                                                                  template<class LTS_TYPE>
                                                                                  std::string debug_id_short(const bisim_partitioner_gj<LTS_TYPE>& partitioner) const
                                                                                  {
                                                                                    assert(partitioner.m_states.data()<=this);
                                                                                    assert(this<partitioner.m_states.data_end());
                                                                                    return std::to_string(this-partitioner.m_states.data());
                                                                                  }

                                                                                  /// \brief print a state identification for debugging
                                                                                  template<class LTS_TYPE>
                                                                                  std::string debug_id(const bisim_partitioner_gj<LTS_TYPE>& partitioner) const
                                                                                  {
                                                                                    return "state " + debug_id_short(partitioner);
                                                                                  }
                                                                                #endif
                                                                                #if !defined(NDEBUG) || defined(COUNT_WORK_BALANCE)
                                                                                  mutable check_complexity::state_gj_counter_t work_counter;
                                                                                #endif
};

/// The following type gives the start and end indications of the transitions
/// for the same block, label and constellation in the array m_BLC_transitions.
struct BLC_indicators
{
  BLC_list_iterator start_same_BLC;

  // If the source block of the BLC_indicator has new bottom states,
  // it is undefined whether the BLC_indicator should be regarded as stable or
  // unstable. Otherwise, the BLC_indicator is regarded as stable if and only
  // if start_marked_BLC is ==nullptr.
  BLC_list_iterator_or_null start_marked_BLC;
  BLC_list_iterator end_same_BLC;

  BLC_indicators(BLC_list_iterator start, BLC_list_iterator end,bool is_stable)
   : start_same_BLC(start),
     start_marked_BLC(is_stable ? nullptr : end),
     end_same_BLC(end)
  {                                                                             assert(nullptr!=start_same_BLC);  assert(nullptr!=end_same_BLC);
                                                                                assert(start_same_BLC<=end_same_BLC);
  }

  bool is_stable() const
  {                                                                             assert(nullptr!=start_same_BLC);  assert(nullptr!=end_same_BLC);
                                                                                assert(nullptr==start_marked_BLC || start_same_BLC<=start_marked_BLC);
                                                                                assert(nullptr==start_marked_BLC || start_marked_BLC<=end_same_BLC);
                                                                                assert(start_same_BLC<=end_same_BLC);
    return nullptr==start_marked_BLC;
  }

  /// This function returns true iff the BLC set contains at least one
  /// marked transition.
  bool has_marked_transitions() const
  {
    if (is_stable())
    {
      return false;
    }                                                                           assert(start_same_BLC<=start_marked_BLC);
                                                                                assert(start_marked_BLC<=end_same_BLC);
    return start_marked_BLC<end_same_BLC;
  }

  void make_stable()
  {                                                                             assert(!is_stable());
    start_marked_BLC=nullptr;
  }

  void make_unstable()
  {                                                                             assert(is_stable());
    start_marked_BLC=end_same_BLC;
  }

  bool operator==(const BLC_indicators& other) const
  {
    return start_same_BLC==other.start_same_BLC &&
           start_marked_BLC==other.start_marked_BLC &&
           end_same_BLC==other.end_same_BLC;
  }

  bool operator!=(const BLC_indicators& other) const
  {
    return !operator==(other);
  }
                                                                                #ifndef NDEBUG
                                                                                  /// \brief print a B_to_C slice identification for debugging
                                                                                  /// \details This function is only available if compiled in Debug mode.
                                                                                  template<class LTS_TYPE>
                                                                                  std::string debug_id(const bisim_partitioner_gj<LTS_TYPE>& partitioner,
                                                                                                                 const block_type* from_block=nullptr) const
                                                                                  {
                                                                                    assert(partitioner.m_BLC_transitions.data()<=start_same_BLC);
                                                                                    assert(nullptr==start_marked_BLC || start_same_BLC<=start_marked_BLC);
                                                                                    assert(nullptr==start_marked_BLC || start_marked_BLC<=end_same_BLC);
                                                                                    assert(start_same_BLC<=end_same_BLC);
                                                                                    assert(end_same_BLC<=partitioner.m_BLC_transitions.data_end());
                                                                                    std::string result("BLC set ["+std::to_string(std::distance<BLC_list_const_iterator>(&*partitioner.m_BLC_transitions.begin(), start_same_BLC))+","+std::to_string(std::distance<BLC_list_const_iterator>(&*partitioner.m_BLC_transitions.begin(), end_same_BLC))+")");
                                                                                    if (start_same_BLC==end_same_BLC)
                                                                                    {
                                                                                      return "Empty "+result;
                                                                                    }
                                                                                    result += " from "+(nullptr==from_block ? partitioner.m_states[partitioner.m_aut.get_transitions()[*start_same_BLC].from()].block : from_block)->debug_id(partitioner);
                                                                                    result += " to ";
                                                                                    result += partitioner.m_states[partitioner.m_aut.get_transitions()[*start_same_BLC].to()].block->c.onstellation->debug_id(partitioner);
                                                                                    result += " containing the ";
                                                                                    if (std::distance(start_same_BLC, end_same_BLC)>1)
                                                                                    {
                                                                                        result+=std::to_string(std::distance(start_same_BLC, end_same_BLC));
                                                                                        result += " transitions ";
                                                                                    }
                                                                                    else
                                                                                    {
                                                                                        result += "transition ";
                                                                                    }
                                                                                    BLC_list_const_iterator iter = start_same_BLC;
                                                                                    if (start_marked_BLC == iter)
                                                                                    {
                                                                                        result += "| ";
                                                                                    }
                                                                                    result += partitioner.m_transitions[*iter].debug_id_short(partitioner);
                                                                                    if (std::distance(start_same_BLC, end_same_BLC)>4)
                                                                                    {
                                                                                        ++iter;
                                                                                        result += start_marked_BLC == iter ? " | " : ", ";
                                                                                        result += partitioner.m_transitions[*iter].debug_id_short(partitioner);
                                                                                        result += std::next(iter) == start_marked_BLC ? " | ..."
                                                                                                  : (!is_stable() && start_marked_BLC>std::next(iter) && start_marked_BLC<=end_same_BLC-3 ? ", ..|.." : ", ...");
                                                                                        iter = end_same_BLC-3;
                                                                                    }
                                                                                    while (++iter!=end_same_BLC)
                                                                                    {
                                                                                        result += start_marked_BLC == iter ? " | " : ", ";
                                                                                        result += partitioner.m_transitions[*iter].debug_id_short(partitioner);
                                                                                    }
                                                                                    if (start_marked_BLC == iter)
                                                                                    {
                                                                                        result += " |";
                                                                                    }
                                                                                    return result;
                                                                                  }
                                                                                #endif
                                                                                #if !defined(NDEBUG) || defined(COUNT_WORK_BALANCE)
                                                                                  mutable check_complexity::BLC_gj_counter_t work_counter;
                                                                                #endif
};

/// information about a transition
/// The source, label and target of the transition are not stored here but in
/// m_aut.get_transitions(), to save memory.
struct transition_type
{
  // The position of the transition type corresponds to m_aut.get_transitions().
  // std::size_t from, label, to are found in m_aut.get_transitions().
  linked_list<BLC_indicators>::iterator transitions_per_block_to_constellation;
  outgoing_transitions_it ref_outgoing_transitions;  // This refers to the position of this transition in m_outgoing_transitions.
                                                     // During initialisation m_outgoing_transitions contains the indices of this
                                                     // transition. After initialisation m_outgoing_transitions refers to the corresponding
                                                     // entry in m_BLC_transitions, of which the field transition contains the index
                                                     // of this transition.
                                                                                #ifndef NDEBUG
                                                                                  /// \brief print a short transition identification for debugging
                                                                                  /// \details This function is only available if compiled in Debug mode.
                                                                                  template<class LTS_TYPE>
                                                                                  std::string debug_id_short(const bisim_partitioner_gj<LTS_TYPE>& partitioner) const
                                                                                  {
                                                                                    assert(partitioner.m_transitions.data()<=this);
                                                                                    assert(this<partitioner.m_transitions.data_end());
                                                                                    const transition& t=partitioner.m_aut.get_transitions()
                                                                                                                       [this-partitioner.m_transitions.data()];
                                                                                    return partitioner.m_states[t.from()].debug_id_short(partitioner) + " -" +
                                                                                           pp(partitioner.m_aut.action_label(t.label())) + "-> " +
                                                                                           partitioner.m_states[t.to()].debug_id_short(partitioner);
                                                                                  }

                                                                                  /// \brief print a transition identification for debugging
                                                                                  /// \details This function is only available if compiled in Debug mode.
                                                                                  template<class LTS_TYPE>
                                                                                  std::string debug_id(const bisim_partitioner_gj<LTS_TYPE>& partitioner) const
                                                                                  {
                                                                                    return "transition " + debug_id_short(partitioner);
                                                                                  }
                                                                                #endif
                                                                                #if !defined(NDEBUG) || defined(COUNT_WORK_BALANCE)
                                                                                  mutable check_complexity::trans_gj_counter_t work_counter;
                                                                                #endif
};

/// information about a block
struct block_type
{
  union constellation_or_first_unmarked_bottom_state
  {
    /// constellation that the block is in
    constellation_type* onstellation;
    /// \brief field used during initialisation for the first unmarked bottom state
    /// \details This cannot be an iterator, otherwise the requirement that the
    /// destructor is trivial would be violated.
    state_in_block_pointer* first_unmarked_bottom_state;

    constellation_or_first_unmarked_bottom_state(constellation_type* new_c)
      :onstellation(new_c)
    {}
  } c;

  /// first state of the block in m_states_in_blocks
  /// States in [start_bottom_states, sta.rt_non_bottom_states) are bottom
  /// states in the block
  state_in_block_pointer* start_bottom_states;

  union start_non_bottom_states_or_state_in_reduced_LTS
  {
    /// first non-bottom state of the block in m_states_in_blocks
    /// States in [sta.rt_non_bottom_states, end_states) are non-bottom states
    /// in the block.
    ///
    /// If m_branching==false, we have sta.rt_non_bottom_states==end_states.
    ///
    /// This cannot be an iterator because then the destructor would be
    /// non-trivial.
    state_in_block_pointer* rt_non_bottom_states;

    /// \brief state index in the reduced LTS
    /// \details After partition refinement has finished, the boundary between
    /// bottom and non-bottom states is no longer needed.  Therefore, we use
    /// the same space to store a block number instead.  This block number is
    /// the same as the state number in the reduced LTS.
    state_index te_in_reduced_LTS;

    start_non_bottom_states_or_state_in_reduced_LTS(state_in_block_pointer* s)
      : rt_non_bottom_states(s)
    {}
  } sta;

  /// pointer past the last state in the block
  state_in_block_pointer* end_states;
  /// list of descriptors of all BLC sets that contain transitions starting in
  /// the block.
  /// The first element in the list contains inert transitions (if they exist);
  /// BLC sets that are regarded as unstable are near the end of the list.
  union btc_R
  {
    // linked_list should be trivially destructible
    linked_list<BLC_indicators> to_constellation;                               static_assert(std::is_trivially_destructible
                                                                                                                        <linked_list<BLC_indicators> >::value);
    /// \brief pointer to vector of marked states
    /// \details During initialisation (when there is only one constellation)
    /// the same space as `to_constellation` is actually used for something
    /// else.
    ///
    /// In the initial refinement in `create_initial_partition()`, blocks
    /// are split according to which action labels they can (inertly) reach.
    /// If R!=nullptr, this block has been registered as a block where some
    /// state has a transition with the current label under investigation.
    /// Such states, if they are non-bottom, are inserted into the vector
    /// `*R`.  If R!=nullptr, the block has also been inserted into
    /// the vector `blocks_that_need_refinement` (a local variable of
    /// `create_initial_partition()`).
    std::vector<state_in_block_pointer>* R;

    /// \brief constructor
    /// \details Note: `if_R_is_nullptr_then_to_constellation_is_empty_list()`
    /// depends on the fact that the constructor creates the variant field `R`.
    btc_R()
      : R(nullptr)
    {}

    /// \brief indicates whether the default values of the union members agree
    /// \details If this function returns `false`, it is necesssary to
    /// explicitly construct every `block.to_constellation` list during
    /// initialisation.  Otherwise, it would be enough to just keep `R` as
    /// `nullptr`.  The function is not a constexpr but it is optimized away
    /// completely (at least by my compiler, DNJ).
    static bool if_R_is_nullptr_then_to_constellation_is_empty_list()
    {
      btc_R test_should_be_empty_BLC_list=btc_R();                              assert(nullptr==test_should_be_empty_BLC_list.R);
      if constexpr (sizeof(test_should_be_empty_BLC_list.R)!=
                        sizeof(test_should_be_empty_BLC_list.to_constellation))
      {
        return false;
      }
      if (test_should_be_empty_BLC_list.to_constellation.empty() &&
          test_should_be_empty_BLC_list.to_constellation==
                                                 linked_list<BLC_indicators>())
      {
        // no need to change `test_should_be_empty_BLC_list` from a pointer
        // to a linked_list explicitly, as the two seem to have the same bit
        // pattern; the destructor will work fine.
        return true;
      }
      // The destructor expects a linked list:
      new (&test_should_be_empty_BLC_list) linked_list<BLC_indicators>();
      return false;
    }
  } block;

  /// \brief copy constructor. Required by MSCV.
  block_type(const block_type& other)
    : c(other.c.onstellation),
      start_bottom_states(other.start_bottom_states),
      sta(other.sta.rt_non_bottom_states),
      end_states(other.end_states),
      block(other.block),
      contains_new_bottom_states(other.contains_new_bottom_states)
  {}

  /// a boolean that is true iff the block contains new bottom states; in that
  /// case the block will be ignored until stabilizeB() handles all blocks
  /// with new bottom states.  Such a block must also be added to the list
  /// m_blocks_with_new_bottom_states.
  bool contains_new_bottom_states;

  block_type(state_in_block_pointer* beginning_of_states,
             constellation_type* new_c)
    : c(new_c),
      start_bottom_states(beginning_of_states),
      sta(beginning_of_states),
      end_states(beginning_of_states),
      block(),
      contains_new_bottom_states(false)
  {}

  /// constructor for first block and for four_way_splitB()
  block_type(state_in_block_pointer* start_bottom,
             state_in_block_pointer* start_non_bottom,
             state_in_block_pointer* end,
             constellation_type* new_c)
    : c(new_c),
      start_bottom_states(start_bottom),
      sta(start_non_bottom),
      end_states(end),
      block(),
      contains_new_bottom_states(false)
  {                                                                             assert(start_bottom<=start_non_bottom);  assert(start_non_bottom<=end);
  }
                                                                                #ifndef NDEBUG
                                                                                  /// \brief print a block identification for debugging
                                                                                  template<class LTS_TYPE>
                                                                                  std::string debug_id(const bisim_partitioner_gj<LTS_TYPE>& partitioner) const
                                                                                  { assert(partitioner.m_states_in_blocks.data()<=start_bottom_states);
                                                                                    assert(start_bottom_states<=sta.rt_non_bottom_states);
                                                                                    assert(sta.rt_non_bottom_states<=end_states);
                                                                                    assert(end_states<=partitioner.m_states_in_blocks.data_end());
                                                                                    return"block ["+std::to_string
                                                                                          (std::distance<const state_in_block_pointer*>
                                                                                            (partitioner.m_states_in_blocks.data(), start_bottom_states))+","+
                                                                                        std::to_string
                                                                                          (std::distance<const state_in_block_pointer*>
                                                                                            (partitioner.m_states_in_blocks.data(), end_states))+")";
                                                                                  }
                                                                                #endif
                                                                                #if !defined(NDEBUG) || defined(COUNT_WORK_BALANCE)
                                                                                  mutable check_complexity::block_gj_counter_t work_counter;
                                                                                #endif
};

/// information about a constellation
struct constellation_type
{
  /// points to the first state in m_states_in_blocks
  state_in_block_pointer* start_const_states;

  /// points past the last state in m_states_in_blocks
  state_in_block_pointer* end_const_states;

  constellation_type(state_in_block_pointer* const new_start,
                     state_in_block_pointer* const new_end)
    : start_const_states(new_start),
      end_const_states(new_end)
  {}
                                                                                #ifndef NDEBUG
                                                                                  /// \brief print a constellation identification for debugging
                                                                                  template<class LTS_TYPE>
                                                                                  std::string debug_id(const bisim_partitioner_gj<LTS_TYPE>& partitioner) const
                                                                                  { assert(partitioner.m_states_in_blocks.data()<=start_const_states);
                                                                                    assert(start_const_states<end_const_states);
                                                                                    assert(end_const_states<=partitioner.m_states_in_blocks.data_end());
                                                                                    return "constellation ["+std::to_string
                                                                                          (std::distance<const state_in_block_pointer*>
                                                                                            (partitioner.m_states_in_blocks.data(), start_const_states))+","+
                                                                                        std::to_string
                                                                                          (std::distance<const state_in_block_pointer*>
                                                                                            (partitioner.m_states_in_blocks.data(), end_const_states))+")";
                                                                                  }
                                                                                #endif
};

} // end namespace bisimulation_gj


/*=============================================================================
=                                 main class                                  =
=============================================================================*/


using namespace mcrl2::lts::detail::bisimulation_gj;

/// \class bisim_partitioner_gj
/// \brief implements the main algorithm for the branching bisimulation quotient
template <class LTS_TYPE>
class bisim_partitioner_gj
{
  private:

    typedef std::unordered_set<state_index> set_of_states_type;
    typedef std::unordered_set<transition_index> set_of_transitions_type;
                                                                                #ifndef NDEBUG
                                                                                  public: // needed for the debugging functions, e.g. debug_id().
                                                                                #endif
    /// \brief automaton that is being reduced
    LTS_TYPE& m_aut;

    // Generic data structures.
    fixed_vector<state_type_gj> m_states;

    fixed_vector<outgoing_transition_type> m_outgoing_transitions;
                                                                  // During refining this contains the index in m_BLC_transition, of which
                                                                  // the transition field contains the index of the transition.
    fixed_vector<transition_type> m_transitions;
    fixed_vector<state_in_block_pointer> m_states_in_blocks;
    state_index no_of_blocks;
    state_index no_of_constellations;
    fixed_vector<transition_index> m_BLC_transitions;
  private:
    std::vector<block_type*> m_blocks_with_new_bottom_states;

#ifndef USE_FOUR_WAY_SPLIT
    /// Below are the two vectors that contain the marked and unmarked states,
    /// which are internally split in a part for states to be investigated, and
    /// a part for states that belong definitively to this set.
    todo_state_vector m_R, m_U;
    std::vector<state_in_block_pointer> m_U_counter_reset_vector;
#endif

    /// The following variable contains all non-trivial constellations.
    std::vector<constellation_type*> m_non_trivial_constellations;

    std::vector<linked_list<BLC_indicators>::iterator>
                                                m_BLC_indicators_to_be_deleted;

#ifndef USE_FOUR_WAY_SPLIT
    /// In m_co_splitters_to_be_checked we store pairs (transition,from_block).
    /// The transition is in an unstable co-splitter that may have no
    /// corresponding main splitter. In that case it should be made stable.
    std::vector<std::pair<BLC_list_iterator, block_type*> >
                                                  m_co_splitters_to_be_checked;
#endif

    /// \brief true iff branching (not strong) bisimulation has been requested
    const bool m_branching;

    /// \brief true iff divergence-preserving branching bisimulation has been
    /// requested
    /// \details Note that this field must be false if strong bisimulation has
    /// been requested.  There is no such thing as divergence-preserving strong
    /// bisimulation.
    const bool m_preserve_divergence;

    /// The auxiliary function below can be removed, but is now used to express
    /// that the hidden_label_map does not need to be applied, while still
    /// leaving it in the code.
    static typename LTS_TYPE::labels_size_type m_aut_apply_hidden_label_map
                                        (typename LTS_TYPE::labels_size_type l)
    {
      return l;
    }

    /// The function assumes that m_branching is true and tests whether
    /// transition t is inert during initialisation under that condition
    bool is_inert_during_init_if_branching(const transition& t) const
    {                                                                           assert(m_branching);
      return m_aut.is_tau(m_aut_apply_hidden_label_map(t.label())) &&
             (!m_preserve_divergence || t.from() != t.to());
    }

    /// The function tests whether transition t is inert during initialisation,
    /// i.e. when there is only one source/target block.
    bool is_inert_during_init(const transition& t) const
    {
      return m_branching && is_inert_during_init_if_branching(t);
    }

    /// The function calculates the label index of transition t, where
    /// tau-self-loops get the special index `divergent_label` if
    /// divergence needs to be preserved
    label_index label_or_divergence(const transition& t,
                                    const label_index divergent_label=-2
                                        /* different from null_action */) const
    {
      label_index result = m_aut_apply_hidden_label_map(t.label());             assert(divergent_label!=result);  assert(null_action!=divergent_label);
      if (m_preserve_divergence && (                                            assert(m_branching),
           t.from() == t.to()) &&
          m_aut.is_tau(result))
      {
        return divergent_label;
      }
      return result;
    }
                                                                                #ifndef NDEBUG
                                                                                  /// \brief Checks whether the transition data structure is correct
                                                                                  /// \returns true iff all checks pass
                                                                                  /// \details Checks whether the pointers incoming transitions -> outgoing
                                                                                  /// transitions -> BLC transitions -> incoming transitions are consistent;
                                                                                  /// whether the pointers from states to incoming and outgoing transitions are
                                                                                  /// consistent; whether the pointers from BLC indicators to BLC sets are
                                                                                  /// consistent.
                                                                                  ///
                                                                                  /// If `check_block_to_constellation`, it also checks whether every
                                                                                  /// transition is in one BLC set of its source block.
                                                                                  ///
                                                                                  /// If `check_temporary_complexity_counters`, it also checks that no more
                                                                                  /// work is accounted for in temporary complexity counters.  If
                                                                                  /// `initialisation` holds, all states are treated as non-bottom states (so
                                                                                  /// that later one might handle all bottom states as new bottom states in the
                                                                                  /// very first call to `stabilizeB()`).  In any case, the BLC sets need to be
                                                                                  /// fully initialised.
                                                                                  void check_transitions(const bool initialisation,
                                                                                                         const bool check_temporary_complexity_counters,
                                                                                                         const bool check_block_to_constellation = true) const
                                                                                  {
                                                                                    for(transition_index ti=0; ti<m_aut.num_transitions(); ++ti)
                                                                                    {
                                                                                      const BLC_list_const_iterator btc_ti=
                                                                                               m_transitions[ti].ref_outgoing_transitions->ref.BLC_transitions;
                                                                                      assert(*btc_ti==ti);

                                                                                      const transition& t=m_aut.get_transitions()[ti];
                                                                                      assert(&*m_states[t.to()].start_incoming_transitions<=&t);
                                                                                      if (t.to()+1!=m_aut.num_states())
                                                                                      {
                                                                                        assert(&t<=&*std::prev(m_states[t.to()+1].start_incoming_transitions));
                                                                                      }
                                                                                      else
                                                                                      {
                                                                                        assert(&t<=&m_aut.get_transitions().back());
                                                                                      }

                                                                                      assert(m_states[t.from()].start_outgoing_transitions<=
                                                                                                                   m_transitions[ti].ref_outgoing_transitions);
                                                                                      if (t.from()+1==m_aut.num_states())
                                                                                      {
                                                                                        assert(m_transitions[ti].ref_outgoing_transitions<
                                                                                                                                 m_outgoing_transitions.end());
                                                                                      }
                                                                                      else
                                                                                      {
                                                                                        assert(m_transitions[ti].ref_outgoing_transitions<
                                                                                                            m_states[t.from() + 1].start_outgoing_transitions);
                                                                                      }

                                                                                      assert(m_transitions[ti].
                                                                                               transitions_per_block_to_constellation->start_same_BLC<=btc_ti);
                                                                                      assert(btc_ti<m_transitions[ti].
                                                                                                         transitions_per_block_to_constellation->end_same_BLC);

                                                                                      if (!check_block_to_constellation)
                                                                                        continue;

                                                                                      block_type* const b=m_states[t.from()].block;

                                                                                      const label_index t_label = label_or_divergence(t);
                                                                                      bool found=false;
                                                                                      for(const BLC_indicators& blc: b->block.to_constellation)
                                                                                      {
                                                                                        if (!blc.is_stable())
                                                                                        {
                                                                                          assert(blc.start_same_BLC<=blc.start_marked_BLC);
                                                                                          assert(blc.start_marked_BLC<=blc.end_same_BLC);
                                                                                        }
                                                                                        assert(blc.start_same_BLC<blc.end_same_BLC);
                                                                                        transition& first_t = m_aut.get_transitions()[*blc.start_same_BLC];
                                                                                        assert(b == m_states[first_t.from()].block);
                                                                                        if (t_label == label_or_divergence(first_t) &&
                                                                                            m_states[first_t.to()].block->c.onstellation ==
                                                                                                                        m_states[t.to()].block->c.onstellation)
                                                                                        {
// if (found) { std::cerr << "Found multiple BLC sets with transitions (" << b->debug_id(*this) << " -" << m_aut.action_label(t.label()) << "-> " << m_states[t.to()].block->c.onstellation->debug_id(*this) << ")\n"; }
                                                                                          assert(!found);
                                                                                          assert(blc.start_same_BLC <= btc_ti);
                                                                                          assert(btc_ti<blc.end_same_BLC);
                                                                                          assert(&blc == &*m_transitions[ti].transitions_per_block_to_constellation);
                                                                                          found = true;
                                                                                        }
                                                                                      }
                                                                                      assert(found);
                                                                                      if (check_temporary_complexity_counters)
                                                                                      {
                                                                                        block_type* const targetb = m_states[t.to()].block;
                                                                                        const unsigned max_sourceB = check_complexity::log_n-
                                                                                                        check_complexity::ilog2(number_of_states_in_block(*b));
                                                                                        const unsigned max_targetC = check_complexity::log_n-
                                                                                              check_complexity::ilog2(number_of_states_in_constellation
                                                                                                                                   (*targetb->c.onstellation));
                                                                                        const unsigned max_targetB = check_complexity::log_n-
                                                                                                  check_complexity::ilog2(number_of_states_in_block(*targetb));
                                                                                        mCRL2complexity(&m_transitions[ti],
                                                                                                no_temporary_work(max_sourceB, max_targetC, max_targetB,
                                                                                                !initialisation &&
                                                                                                0==m_states[t.from()].no_of_outgoing_block_inert_transitions),
                                                                                                                                                        *this);
                                                                                      }
                                                                                    }
                                                                                  }

                                                                                  /// \brief Checks whether data structures are consistent
                                                                                  /// \returns true iff all checks pass
                                                                                  /// \details Checks whether states are in their blocks; the pointers outgoing
                                                                                  /// transition (-> BLC transition) -> incoming transition -> outgoing
                                                                                  /// transition are consistent; whether the saC slices (source state, action,
                                                                                  /// target constellation) are correct; whether blocks are correct.
                                                                                  [[nodiscard]]
                                                                                  bool check_data_structures(const std::string& tag, const bool initialisation=false, const bool check_temporary_complexity_counters=true) const
                                                                                  {
                                                                                    mCRL2log(log::debug) << "Check data structures: " << tag << ".\n";
                                                                                    assert(m_states.size()==m_aut.num_states());
                                                                                    assert(m_states_in_blocks.size()==m_aut.num_states());
                                                                                    assert(m_transitions.size()==m_aut.num_transitions());
                                                                                    assert(m_outgoing_transitions.size()==m_aut.num_transitions());
                                                                                    assert(m_BLC_transitions.size()==m_aut.num_transitions());

                                                                                    // Check that the elements in m_states are well formed.
                                                                                    for (fixed_vector<state_type_gj>::iterator si=
                                                                                            const_cast<fixed_vector<state_type_gj>&>(m_states).begin();
                                                                                                                                      si<m_states.cend(); si++)
                                                                                    {
                                                                                      const state_type_gj& s=*si;

                                                                                      assert(s.counter==undefined);
                                                                                      assert(s.block->start_bottom_states< s.block->sta.rt_non_bottom_states);
                                                                                      assert(s.block->sta.rt_non_bottom_states<=s.block->end_states);

                                                                                      // In the following line we need that si is an iterator (not a const_iterator)
                                                                                      assert(std::find(s.block->start_bottom_states,
                                                                                                    s.block->end_states,
                                                                                                    state_in_block_pointer(si))!=s.block->end_states);

                                                                                      assert(s.ref_states_in_blocks->ref_state==si);

                                                                                      // ensure that in the incoming transitions we first have the transitions
                                                                                      // with label tau, and then the other transitions:
                                                                                      bool maybe_tau=true;
                                                                                      const std::vector<transition>::const_iterator end_it1=
                                                                                            std::next(si)>=m_states.end() ? m_aut.get_transitions().end()
                                                                                                                   : std::next(si)->start_incoming_transitions;
                                                                                      for (std::vector<transition>::const_iterator
                                                                                                            it=s.start_incoming_transitions; it!=end_it1; ++it)
                                                                                      {
                                                                                        const transition& t=*it;
                                                                                        if (m_aut.is_tau(m_aut_apply_hidden_label_map(t.label())))
                                                                                        {
                                                                                          assert(maybe_tau);
                                                                                        }
                                                                                        else
                                                                                        {
                                                                                          maybe_tau=false;
                                                                                        }
                                                                                        // potentially we might test that the transitions are grouped per label
                                                                                      }

                                                                                      // Check that for each state the outgoing transitions satisfy the
                                                                                      // following invariant:  First there are (originally) inert transitions
                                                                                      // (inert transitions may be separated over multiple constellations, so
                                                                                      // we cannot require that the inert transitions come before other
                                                                                      // tau-transitions).  Then there are other transitions sorted per label
                                                                                      // and constellation.
                                                                                      std::unordered_set<std::pair<label_index, const constellation_type*> >
                                                                                                                                           constellations_seen;

                                                                                      maybe_tau=true;
                                                                                      // The construction below is to enable translation on Windows.
                                                                                      const outgoing_transitions_const_it end_it2=
                                                                                            std::next(si)>=m_states.end() ? m_outgoing_transitions.cend()
                                                                                                                   : std::next(si)->start_outgoing_transitions;
                                                                                      for(outgoing_transitions_const_it it=s.start_outgoing_transitions;
                                                                                                                                             it!=end_it2; ++it)
                                                                                      {
                                                                                        const transition& t=m_aut.get_transitions()[!initialisation
                                                                                                             ? *it->ref.BLC_transitions : it->ref.transitions];
// if (t.from() != si) { std::cerr << m_transitions[*it->ref.BLC_transitions].debug_id(*this) << " is an outgoing transition of state " << si << "!\n"; }
                                                                                        assert(m_states.cbegin()+t.from()==si);
                                                                                        assert(m_transitions[!initialisation ? *it->ref.BLC_transitions
                                                                                                          : it->ref.transitions].ref_outgoing_transitions==it);
                                                                                        assert((it->start_same_saC>it &&
                                                                                                it->start_same_saC<m_outgoing_transitions.end() &&
                                                                                                ((it+1)->start_same_saC==it->start_same_saC ||
                                                                                                 (it+1)->start_same_saC<=it)) ||
                                                                                               (it->start_same_saC<=it &&
                                                                                                (it+1==m_outgoing_transitions.end() ||
                                                                                                 (it+1)->start_same_saC>it)));
// if (it->start_same_saC < it->start_same_saC->start_same_saC) { std::cerr << "Now checking transitions " << m_transitions[*it->start_same_saC->ref.BLC_transitions].debug_id_short(*this) << " ... " << m_transitions[*it->start_same_saC->start_same_saC->ref.BLC_transitions].debug_id_short(*this) << '\n'; }
                                                                                        const label_index t_label = label_or_divergence(t);
                                                                                        // The following for loop is only executed if it is the last transition in the saC-slice.
                                                                                        for(outgoing_transitions_const_it itt=it->start_same_saC;
                                                                                                                 itt<it->start_same_saC->start_same_saC; ++itt)
                                                                                        {
                                                                                          const transition& t1=m_aut.get_transitions()[!initialisation
                                                                                                           ? *itt->ref.BLC_transitions : itt->ref.transitions];
// if (t1.from()!=si) { assert(!initialisation); std::cerr << m_transitions[*itt->ref.BLC_transitions].debug_id(*this) << " does not start in state " << si << '\n'; }
                                                                                          assert(m_states.cbegin()+t1.from()==si);
                                                                                          assert(label_or_divergence(t1) == t_label);
                                                                                          assert(m_states[t.to()].block->c.onstellation==
                                                                                                                      m_states[t1.to()].block->c.onstellation);
                                                                                        }

                                                                                        const label_index label = label_or_divergence(t);
                                                                                        // Check that if the target constellation, if not new, is equal to the
                                                                                        // target constellation of the previous outgoing transition.
                                                                                        const constellation_type* t_to_constellation=
                                                                                                                        m_states[t.to()].block->c.onstellation;
                                                                                        if (constellations_seen.count(std::pair(label,t_to_constellation))>0)
                                                                                        {
                                                                                          assert(it!=s.start_outgoing_transitions);
                                                                                          const transition& old_t=m_aut.get_transitions()[!initialisation
                                                                                                                          ? *std::prev(it)->ref.BLC_transitions
                                                                                                                          : std::prev(it)->ref.transitions];
                                                                                          assert(label_or_divergence(old_t)==label);
                                                                                          assert(t_to_constellation==
                                                                                                                   m_states[old_t.to()].block->c.onstellation);
                                                                                        }
                                                                                        else
                                                                                        {
                                                                                          if (m_branching && m_aut.is_tau(label))
                                                                                          {
//if (!maybe_tau) { std::cerr << "maybe_tau==false for " << m_transitions[!initialisation ? *it->ref.BLC_transitions : it->ref.transitions].debug_id(*this) << '\n'; }
                                                                                            assert(maybe_tau);
                                                                                          }
                                                                                          else
                                                                                          {
                                                                                            maybe_tau=false;
                                                                                          }
                                                                                          constellations_seen.emplace(label,t_to_constellation);
                                                                                        }
                                                                                      }
                                                                                    }
                                                                                    // Check that the elements in m_transitions are well formed.
                                                                                    if (!initialisation)
                                                                                    {
                                                                                      check_transitions(initialisation, check_temporary_complexity_counters);
                                                                                    }
                                                                                    // Check that the elements in m_blocks are well formed.
                                                                                    {
                                                                                      set_of_transitions_type all_transitions;
                                                                                      transition_index actual_no_of_non_constellation_inert_BLC_sets=0;
                                                                                      for (const state_in_block_pointer* si=m_states_in_blocks.data();
                                                                                        m_states_in_blocks.data_end()!=si; si=si->ref_state->block->end_states)
                                                                                      {
                                                                                        const block_type& b=*si->ref_state->block;
                                                                                        const constellation_type& c=*b.c.onstellation;
                                                                                        assert(m_states_in_blocks.data()<=c.start_const_states);
                                                                                        assert(c.start_const_states<=b.start_bottom_states);
                                                                                        assert(b.start_bottom_states<b.sta.rt_non_bottom_states);
                                                                                        assert(b.sta.rt_non_bottom_states<=b.end_states);
                                                                                        assert(b.end_states<=c.end_const_states);
                                                                                        assert(c.end_const_states<=m_states_in_blocks.data_end());

                                                                                        unsigned char const max_B=check_complexity::log_n-
                                                                                                         check_complexity::ilog2(number_of_states_in_block(b));
                                                                                        unsigned char const max_C=check_complexity::log_n-check_complexity::
                                                                                                   ilog2(number_of_states_in_constellation(*b.c.onstellation));
                                                                                        for (const state_in_block_pointer*
                                                                                                is=b.start_bottom_states; is!=b.sta.rt_non_bottom_states; ++is)
                                                                                        {
                                                                                          assert(is->ref_state->block==&b);
                                                                                          assert(is->ref_state->no_of_outgoing_block_inert_transitions==0);
                                                                                          if (check_temporary_complexity_counters)
                                                                                          {
                                                                                            // During initialisation, new bottom state counters must remain 0
                                                                                            mCRL2complexity(is->ref_state, no_temporary_work(max_B,
                                                                                                                                      !initialisation), *this);
                                                                                          }
                                                                                        }
                                                                                        for (const state_in_block_pointer*
                                                                                                         is=b.sta.rt_non_bottom_states; is!=b.end_states; ++is)
                                                                                        {
                                                                                          assert(is->ref_state->block==&b);
                                                                                          assert(is->ref_state->no_of_outgoing_block_inert_transitions>0);
                                                                                          // Because there cannot be new bottom states among non-bottom states,
                                                                                          // we can always check the temporary work of non-bottom states:
                                                                                          mCRL2complexity(is->ref_state,no_temporary_work(max_B,false),*this);
                                                                                        }
                                                                                        // Because a block has no temporary or new-bottom-state-related
                                                                                        // counters, we can always check its temporary work:
                                                                                        mCRL2complexity(&b, no_temporary_work(max_C, max_B), *this);

                                                                                        if (!initialisation)
                                                                                        {
                                                                                          assert(b.block.to_constellation.check_linked_list());
                                                                                          for (linked_list<BLC_indicators>::const_iterator
                                                                                                                    ind=b.block.to_constellation.begin();
                                                                                                                    ind!=b.block.to_constellation.end(); ++ind)
                                                                                          {
                                                                                            assert(ind->start_same_BLC<ind->end_same_BLC);
                                                                                            const transition& first_transition=
                                                                                                               m_aut.get_transitions()[*(ind->start_same_BLC)];
                                                                                            const label_index first_transition_label=
                                                                                                                         label_or_divergence(first_transition);
                                                                                            if(!is_inert_during_init(first_transition) ||
                                                                                               m_states[first_transition.from()].block->c.onstellation!=
                                                                                                         m_states[first_transition.to()].block->c.onstellation)
                                                                                            {
                                                                                              ++actual_no_of_non_constellation_inert_BLC_sets;
                                                                                            }
                                                                                            for(BLC_list_const_iterator i=ind->start_same_BLC;
                                                                                                                                      i<ind->end_same_BLC; ++i)
                                                                                            {
                                                                                              const transition& t=m_aut.get_transitions()[*i];
                                                                                              assert(m_transitions[*i].transitions_per_block_to_constellation==
                                                                                                                                                          ind);
                                                                                              all_transitions.emplace(*i);
                                                                                              assert(m_states[t.from()].block==&b);
                                                                                              assert(m_states[t.to()].block->c.onstellation==
                                                                                                        m_states[first_transition.to()].block->c.onstellation);
                                                                                              assert(label_or_divergence(t)==first_transition_label);
                                                                                              if (is_inert_during_init(t) && b.c.onstellation==
                                                                                                                        m_states[t.to()].block->c.onstellation)
                                                                                              {
                                                                                                // The inert transitions should be in the first element of
                                                                                                // `block.to_constellation`:
                                                                                                assert(b.block.to_constellation.begin()==ind);
                                                                                              }
                                                                                            }
                                                                                            if (check_temporary_complexity_counters)
                                                                                            {
                                                                                              mCRL2complexity(ind, no_temporary_work(max_C,
                                                                                                 check_complexity::log_n-check_complexity::ilog2
                                                                                                   (number_of_states_in_constellation(*m_states
                                                                                                      [first_transition.to()].block->c.onstellation))), *this);
                                                                                            }
                                                                                          }
                                                                                        }
                                                                                      }
                                                                                      if (!initialisation) {
                                                                                        assert(all_transitions.size()==m_aut.num_transitions());
                                                                                        assert(actual_no_of_non_constellation_inert_BLC_sets==
                                                                                                                       no_of_non_constellation_inert_BLC_sets);
                                                                                      }
                                                                                      // destruct `all_transitions` here
                                                                                    }

                                                                                    // TODO: Check that the elements in m_constellations are well formed.

                                                                                    // Check that the states in m_states_in_blocks refer to with ref_states_in_block to the right position.
                                                                                    // and that a state is correctly designated as a (non-)bottom state.
                                                                                    for (const state_in_block_pointer*
                                                                                          si=m_states_in_blocks.data(); si<m_states_in_blocks.data_end(); ++si)
                                                                                    {
                                                                                      assert(si==si->ref_state->ref_states_in_blocks);
                                                                                    }

                                                                                    // Check that the blocks in m_blocks_with_new_bottom_states are bottom states.
                                                                                    for(const block_type* bi: m_blocks_with_new_bottom_states)
                                                                                    {
                                                                                      assert(bi->contains_new_bottom_states);
                                                                                    }

                                                                                    // Check that the non-trivial constellations are non trivial.
                                                                                    for(const constellation_type* ci: m_non_trivial_constellations)
                                                                                    {
                                                                                      // There are at least two blocks in a non-trivial constellation.
                                                                                      const block_type* const first_bi=ci->start_const_states->ref_state->block;
                                                                                      const block_type* const last_bi=std::prev(ci->end_const_states)->ref_state->block;
                                                                                      assert(first_bi != last_bi);
                                                                                    }
                                                                                    return true;
                                                                                  }

                                                                                  /// \brief Checks the main invariant of the partition refinement algorithm
                                                                                  /// \returns true iff the main invariant holds
                                                                                  /// \details Checks the following invariant:
                                                                                  ///     If a block has a constellation-non-inert transition, then every
                                                                                  ///     bottom state has a constellation-non-inert transition with the same
                                                                                  ///     label to the same target constellation.
                                                                                  /// It is assumed that the BLC data structure is correct, so we conveniently
                                                                                  /// use that to verify the invariant.
                                                                                  ///
                                                                                  /// The function can also check a partial invariant while stabilisation has
                                                                                  /// not yet finished. If calM != nullptr, then we have:
                                                                                  ///     The above invariant may be violated for BLC sets that are still to
                                                                                  ///     be stabilized, as given by the main splitters in calM.
                                                                                  ///     (calM_elt indicates how far stabilization has handled calM already.)
                                                                                  ///     (block_label_to_cotransition indicates the co-splitters that belong
                                                                                  ///     to the main splitters in calM.)
                                                                                  ///     It may also be violated for blocks that contain new bottom states,
                                                                                  ///     as indicated by m_blocks_with_new_bottom_states.
                                                                                  ///
                                                                                  /// Additionally, the function ensures that only transitions in BLC sets
                                                                                  /// satisfying the above conditions are marked:
                                                                                  ///     Transitions may only be marked in BLC sets that are still to be
                                                                                  ///     stabilized, as given by calM (including co-splitters); they may
                                                                                  ///     also be marked if they start in new bottom states, as indicated by
                                                                                  ///     m_blocks_with_new_bottom_states, or if they start in a singleton
                                                                                  ///     block.
                                                                                  [[nodiscard]]
                                                                                  bool check_stability(const std::string& tag,
                                                                                        const std::vector<std::pair<BLC_list_iterator, BLC_list_iterator> >*
                                                                                                                                                calM=nullptr,
                                                                                        const std::pair<BLC_list_iterator,BLC_list_iterator>* calM_elt=nullptr,
                                                                                        const constellation_type* const old_constellation=null_constellation,
                                                                                        const constellation_type* const new_constellation=null_constellation)
                                                                                                                                                          const
                                                                                  {
                                                                                    assert((old_constellation==null_constellation &&
                                                                                            new_constellation==null_constellation   ) ||
                                                                                           (old_constellation!=null_constellation &&
                                                                                            new_constellation!=null_constellation &&
                                                                                            old_constellation!=new_constellation    ));
                                                                                    mCRL2log(log::debug) << "Check stability: " << tag << ".\n";
                                                                                    for (const state_in_block_pointer* si=m_states_in_blocks.data();
                                                                                        m_states_in_blocks.data_end()!=si; si=si->ref_state->block->end_states)
                                                                                    {
                                                                                      const block_type& b=*si->ref_state->block;
                                                                                      bool previous_stable=true;
                                                                                      for (linked_list<BLC_indicators>::const_iterator
                                                                                                   ind=b.block.to_constellation.begin();
                                                                                                   ind!=b.block.to_constellation.end(); ++ind)
                                                                                      {
                                                                                        set_of_states_type all_source_bottom_states;

                                                                                        assert(ind->start_same_BLC<ind->end_same_BLC);
                                                                                        const transition&first_t=m_aut.get_transitions()[*ind->start_same_BLC];
                                                                                        const label_index first_t_label=label_or_divergence(first_t);
                                                                                        const bool all_transitions_in_BLC_are_inert =
                                                                                                   is_inert_during_init(first_t) && b.c.onstellation==
                                                                                                                  m_states[first_t.to()].block->c.onstellation;
                                                                                        assert(!all_transitions_in_BLC_are_inert ||
                                                                                               b.block.to_constellation.begin()==ind);
                                                                                        for (BLC_list_const_iterator i=ind->start_same_BLC;
                                                                                                                                      i<ind->end_same_BLC; ++i)
                                                                                        {
                                                                                          assert(m_BLC_transitions.data()<=i);
                                                                                          assert(i<m_BLC_transitions.data_end());
                                                                                          const transition& t=m_aut.get_transitions()[*i];
// if (m_states[t.from()].block != bi) { std::cerr << m_transitions[*ind->start_same_BLC].debug_id(*this) << " should start in block " << bi << '\n'; }
                                                                                          assert(m_states[t.from()].block == &b);
                                                                                          assert(label_or_divergence(t) == first_t_label);
                                                                                          assert(m_states[t.to()].block->c.onstellation==
                                                                                                                 m_states[first_t.to()].block->c.onstellation);
                                                                                          if (is_inert_during_init(t) && b.c.onstellation==
                                                                                                                        m_states[t.to()].block->c.onstellation)
                                                                                          {
                                                                                            assert(all_transitions_in_BLC_are_inert);
                                                                                          }
                                                                                          else
                                                                                          {
                                                                                            // This is a constellation-non-inert transition.
                                                                                            assert(!all_transitions_in_BLC_are_inert);
                                                                                            if (0 == m_states[t.from()].no_of_outgoing_block_inert_transitions)
                                                                                            {
                                                                                              assert(b.start_bottom_states<=
                                                                                                                      m_states[t.from()].ref_states_in_blocks);
                                                                                              assert(m_states[t.from()].ref_states_in_blocks<
                                                                                                                                   b.sta.rt_non_bottom_states);
                                                                                              all_source_bottom_states.emplace(t.from());
                                                                                            }
                                                                                            else
                                                                                            {
                                                                                              assert(b.sta.rt_non_bottom_states<=
                                                                                                                      m_states[t.from()].ref_states_in_blocks);
                                                                                              assert(m_states[t.from()].ref_states_in_blocks < b.end_states);
                                                                                            }
                                                                                          }
                                                                                        }
                                                                                        assert(all_source_bottom_states.size()<=static_cast<std::size_t>
                                                                                           (std::distance(b.start_bottom_states, b.sta.rt_non_bottom_states)));
                                                                                        // check that every bottom state has a transition in this BLC entry:
                                                                                        bool eventual_instability_is_ok = true;
                                                                                        bool eventual_marking_is_ok = true;
                                                                                        if (!all_transitions_in_BLC_are_inert &&
                                                                                            all_source_bottom_states.size()!=static_cast<std::size_t>
                                                                                            (std::distance(b.start_bottom_states, b.sta.rt_non_bottom_states)))
                                                                                        {
                                                                                          // only splitters should be instable.
                                                                                          mCRL2log(log::debug) << "Not all "
                                                                                            << std::distance(b.start_bottom_states, b.sta.rt_non_bottom_states)
                                                                                              << (m_branching ? " bottom states have a transition in the "
                                                                                                              : " states have a transition in the ")
                                                                                              << ind->debug_id(*this) << ": transitions found from states";
                                                                                          for (set_of_states_type::iterator
                                                                                                           asbc_it=all_source_bottom_states.begin();
                                                                                                           asbc_it!=all_source_bottom_states.end() ; ++asbc_it)
                                                                                          { mCRL2log(log::debug) << ' ' << *asbc_it; }
                                                                                          mCRL2log(log::debug) << '\n';
                                                                                          eventual_instability_is_ok = false;
                                                                                        }
                                                                                        if (!ind->is_stable())
                                                                                        {
                                                                                          // only splitters should contain marked transitions.
                                                                                          mCRL2log(log::debug) << ind->debug_id(*this) << " contains " << std::distance(ind->start_marked_BLC, ind->end_same_BLC) << " marked transitions.\n";
                                                                                          eventual_marking_is_ok = false;
                                                                                        }
                                                                                        if (b.contains_new_bottom_states)
                                                                                        {
                                                                                          /* I would like the following to check more closely because in a
                                                                                             block with new bottom states, one should have...
                                                                                          if (!eventual_marking_is_ok)
                                                                                          {
                                                                                            eventual_marking_is_ok = true;
                                                                                            for (BLC_list_const_iterator i=ind->start_marked_BLC; i<ind->end_same_BLC; ++i)
                                                                                            {
                                                                                              const state_index from = m_aut.get_transitions()[*i].from();
                                                                                              // assert(m_states[from].block == bi); -- already checked earlier
                                                                                              if (0 != m_states[from].no_of_outgoing_block_inert_transitions)
                                                                                              {
                                                                                                // the state is a non-bottom state
                                                                                                eventual_marking_is_ok = false;
                                                                                                break;
                                                                                              }
                                                                                            }
                                                                                            if (eventual_marking_is_ok)
                                                                                            {
                                                                                              mCRL2log(log::debug) << "  This is ok because all marked transitions begin in new bottom states of " << bi->debug_id(*this) << ".\n";
                                                                                              eventual_instability_is_ok = true;
                                                                                            }
                                                                                          } */
                                                                                          if (!(eventual_instability_is_ok && eventual_marking_is_ok))
                                                                                          {
                                                                                            mCRL2log(log::debug) << "  This is ok because " << b.debug_id(*this) << " contains new bottom states.\n";
                                                                                            eventual_instability_is_ok = true;
                                                                                            eventual_marking_is_ok = true;
                                                                                          }
                                                                                        }
                                                                                        if (!(eventual_instability_is_ok && eventual_marking_is_ok) && nullptr != calM && calM->begin() != calM->end())
                                                                                        {
                                                                                          std::vector<std::pair<BLC_list_iterator, BLC_list_iterator> >::const_iterator calM_iter = calM->begin();
                                                                                          if (nullptr != calM_elt)
                                                                                          {
                                                                                            for(;;)
                                                                                            {
                                                                                              assert(calM->end() != calM_iter);
                                                                                              if (calM_iter->first <= calM_elt->first && calM_elt->second <= calM_iter->second)
                                                                                              {
                                                                                                break;
                                                                                              }
                                                                                              ++calM_iter;
                                                                                            }
                                                                                            if (calM_elt->first<=ind->start_same_BLC && ind->end_same_BLC<=calM_elt->second)
                                                                                            {
                                                                                              mCRL2log(log::debug) <<"  This is ok because the BLC set ("
                                                                                                  << b.debug_id(*this) << " -" << m_aut.action_label(first_t.label())
                                                                                                  << "-> " << m_states[first_t.to()].
                                                                                                                         block->c.onstellation->debug_id(*this)
                                                                                                  << ") is soon going to be a main splitter.\n";
                                                                                              eventual_instability_is_ok = true;
                                                                                              eventual_marking_is_ok = true;
                                                                                            }
                                                                                            else
                                                                                            {
                                                                                              if (old_constellation==
                                                                                                                  m_states[first_t.to()].block->c.onstellation)
                                                                                              {
                                                                                                const linked_list<BLC_indicators>::const_iterator main_splitter=b.block.to_constellation.next(ind);
                                                                                                if (main_splitter!=b.block.to_constellation.end())
                                                                                                {
                                                                                                  assert(main_splitter->start_same_BLC < main_splitter->end_same_BLC);
                                                                                                  const transition& main_t = m_aut.get_transitions()[*main_splitter->start_same_BLC];
                                                                                                  assert(m_states[main_t.from()].block == &b);
                                                                                                  if(label_or_divergence(first_t)==label_or_divergence(main_t)
                                                                                                     && m_states[main_t.to()].block->c.onstellation==
                                                                                                                                             new_constellation)
                                                                                                  {
//std::cerr << "Corresponding main splitter: " << main_splitter->debug_id(*this) << '\n';
                                                                                                    if (calM_elt->first<=main_splitter->start_same_BLC && main_splitter->end_same_BLC<=calM_elt->second)
                                                                                                    {
                                                                                                      assert(new_constellation==
                                                                                                                  m_states[main_t.to()].block->c.onstellation);
                                                                                                      mCRL2log(log::debug) << "  This is ok because the BLC set (" << b.debug_id(*this) << " -" << m_aut.action_label(first_t.label()) << "-> " << old_constellation->debug_id(*this) << ") is soon going to be a co-splitter.\n";
                                                                                                      eventual_instability_is_ok = true;
                                                                                                      eventual_marking_is_ok = true;
                                                                                                    }
//else { std::cerr << "Main splitter is not in calM_elt = [" << std::distance(m_BLC_transitions.begin(), calM_elt->first) << ',' << std::distance(m_BLC_transitions.begin(), calM_elt->second) << ")\n" };
                                                                                                  }
                                                                                                }
                                                                                              }
                                                                                            }
                                                                                            ++calM_iter;
                                                                                          }
                                                                                          for(; !(eventual_instability_is_ok && eventual_marking_is_ok) && calM->end() != calM_iter; ++calM_iter)
                                                                                          {
                                                                                            if (calM_iter->first<=ind->start_same_BLC && ind->end_same_BLC<=calM_iter->second)
                                                                                            {
                                                                                              mCRL2log(log::debug) <<"  This is ok because the BLC set ("
                                                                                                  << b.debug_id(*this) << " -" << m_aut.action_label(first_t.label())
                                                                                                  << "-> "
                                                                                                  << m_states[first_t.to()].block->c.onstellation->debug_id(*this)
                                                                                                  << ") is going to be a main splitter later.\n";
                                                                                              eventual_instability_is_ok = true;
                                                                                              eventual_marking_is_ok = true;
                                                                                            }
                                                                                            else
                                                                                            {
                                                                                              if (old_constellation==
                                                                                                                  m_states[first_t.to()].block->c.onstellation)
                                                                                              {
                                                                                                const linked_list<BLC_indicators>::const_iterator main_splitter=b.block.to_constellation.next(ind);
                                                                                                if (main_splitter != b.block.to_constellation.end())
                                                                                                {
                                                                                                  assert(main_splitter->start_same_BLC < main_splitter->end_same_BLC);
                                                                                                  const transition& main_t = m_aut.get_transitions()[*main_splitter->start_same_BLC];
                                                                                                  assert(m_states[main_t.from()].block == &b);
                                                                                                  if(label_or_divergence(first_t)==label_or_divergence(main_t)
                                                                                                     && m_states[main_t.to()].block->c.onstellation==
                                                                                                                                             new_constellation)
                                                                                                  {
                                                                                                    if (calM_iter->first<=main_splitter->start_same_BLC && main_splitter->end_same_BLC<=calM_iter->second)
                                                                                                    {
                                                                                                      assert(new_constellation==
                                                                                                                  m_states[main_t.to()].block->c.onstellation);
                                                                                                      mCRL2log(log::debug) << "  This is ok because the BLC "
                                                                                                          "set (" << b.debug_id(*this) << " -"
                                                                                                          << m_aut.action_label(first_t.label())
                                                                                                          << "-> " << old_constellation->debug_id(*this)
                                                                                                          << ") is going to be a co-splitter later.\n";
                                                                                                      eventual_instability_is_ok = true;
                                                                                                      eventual_marking_is_ok = true;
                                                                                                    }
//else { std::cerr << "Main splitter is not in calM_iter = [" << std::distance(m_BLC_transitions.begin(), calM_iter->first) << ',' << std::distance(m_BLC_transitions.begin(), calM_iter->second) << ")\n" };
                                                                                                  }
                                                                                                }
                                                                                              }
                                                                                            }
                                                                                          }
                                                                                        }
                                                                                        if (1>=number_of_states_in_block(b))
                                                                                        {
                                                                                          if (!eventual_marking_is_ok)
                                                                                          {
                                                                                            mCRL2log(log::debug) << "  (This is ok because the source block contains only 1 state.)\n";
                                                                                            eventual_marking_is_ok = true;
                                                                                          }
                                                                                        }
                                                                                        else if (1<no_of_constellations /* i.e. !initialisation */ &&
                                                                                                   !b.contains_new_bottom_states)
                                                                                        {
                                                                                          assert(eventual_marking_is_ok);
                                                                                          assert(eventual_instability_is_ok);
                                                                                          if (null_constellation==old_constellation && ind->is_stable()) {
                                                                                            assert(previous_stable);
                                                                                          }
                                                                                          else
                                                                                          {
                                                                                            previous_stable=false;
                                                                                          }
                                                                                        }
                                                                                      }
                                                                                    }
                                                                                    mCRL2log(log::debug) << "Check stability finished: " << tag << ".\n";
                                                                                    return true;
                                                                                  }

                                                                                  /// \brief Prints the list of BLC sets as debug output
                                                                                  void display_BLC_list(const block_type* const bi) const
                                                                                  {
                                                                                    mCRL2log(log::debug) << "\n  BLC_List\n";
                                                                                    for(const BLC_indicators& blc_it: bi->block.to_constellation)
                                                                                    {
                                                                                      const transition& first_t=m_aut.get_transitions()[*blc_it.start_same_BLC];
                                                                                      const label_index l=label_or_divergence(first_t, null_action);
                                                                                      mCRL2log(log::debug) << "\n    BLC set "
                                                                                          << std::distance<BLC_list_const_iterator>(m_BLC_transitions.data(),
                                                                                                                               blc_it.start_same_BLC) << " -- "
                                                                                          << std::distance<BLC_list_const_iterator>(m_BLC_transitions.data(),
                                                                                                                                  blc_it.end_same_BLC)
                                                                                          << " of " << (null_action==l ? "divergent self-loop " : pp(m_aut.action_label(l))+"-")
                                                                                          << "transitions to " << m_states[first_t.to()].block->c.onstellation->debug_id(*this) << ":\n";
                                                                                      for (BLC_list_const_iterator i=blc_it.start_same_BLC; ; ++i)
                                                                                      {
                                                                                        if (i == blc_it.start_marked_BLC)
                                                                                        {
                                                                                          mCRL2log(log::debug) << "        (The BLC set is unstable, and the "
                                                                                                                       " following transitions are marked.)\n";
                                                                                        }
                                                                                        if (i>=blc_it.end_same_BLC)
                                                                                        {
                                                                                          break;
                                                                                        }
                                                                                        const transition& t=m_aut.get_transitions()[*i];
                                                                                        mCRL2log(log::debug) << "        " << t.from() << " -"
                                                                                                           << m_aut.action_label(t.label()) << "-> " << t.to();
                                                                                        if (is_inert_during_init(t) &&
                                                                                            m_states[t.from()].block==m_states[t.to()].block)
                                                                                        {
                                                                                          mCRL2log(log::debug) << " (block-inert)";
                                                                                        }
                                                                                        else if (is_inert_during_init(t) &&
                                                                                                 m_states[t.from()].block->c.onstellation==
                                                                                                                        m_states[t.to()].block->c.onstellation)
                                                                                        {
                                                                                          mCRL2log(log::debug) << " (constellation-inert)";
                                                                                        }
                                                                                        mCRL2log(log::debug) << '\n';
                                                                                      }
                                                                                    }
                                                                                    mCRL2log(log::debug) << "  BLC_List end\n";
                                                                                  }

                                                                                  /// \brief Prints the partition refinement data structure as debug output
                                                                                  void print_data_structures(const std::string& header,
                                                                                                             const bool initialisation=false) const
                                                                                  {
                                                                                    if (!mCRL2logEnabled(log::debug))  {  return;  }
                                                                                    mCRL2log(log::debug) << "========= PRINT DATASTRUCTURE: " << header << " =======================================\n"
                                                                                                            "++++++++++++++++++++    States    ++++++++++++++++++++++++++++\n";
                                                                                    for(state_index si=0; si<m_aut.num_states(); ++si)
                                                                                    {
                                                                                      mCRL2log(log::debug) << "State " << si <<" (" << m_states[si].block->debug_id(*this) << "):\n"
                                                                                                              "  #Inert outgoing transitions: " << m_states[si].no_of_outgoing_block_inert_transitions << "\n"

                                                                                                              "  Incoming transitions:\n";
                                                                                      std::vector<transition>::const_iterator end=(si+1==m_aut.num_states()?m_aut.get_transitions().end():m_states[si+1].start_incoming_transitions);
                                                                                      for(std::vector<transition>::const_iterator it=m_states[si].start_incoming_transitions; it!=end; ++it)
                                                                                      {
                                                                                         mCRL2log(log::debug) << "    " << ptr(*it) << "\n";
                                                                                      }

                                                                                      mCRL2log(log::debug) << "  Outgoing transitions:\n";
                                                                                      label_index t_label=m_aut.tau_label_index();
                                                                                      const constellation_type* to_constln=null_constellation;
                                                                                      for(outgoing_transitions_const_it it=m_states[si].start_outgoing_transitions;
                                                                                                      it!=m_outgoing_transitions.end() &&
                                                                                                      (si+1>=m_aut.num_states() || it!=m_states[si+1].start_outgoing_transitions);
                                                                                                   ++it)
                                                                                      {
                                                                                        const transition& t=m_aut.get_transitions()[!initialisation
                                                                                                             ? *it->ref.BLC_transitions : it->ref.transitions];
                                                                                        bool start_same_saC_valid=
                                                                                            m_outgoing_transitions.cbegin()<=it->start_same_saC &&
                                                                                            it->start_same_saC<m_outgoing_transitions.end();
                                                                                        if (start_same_saC_valid &&
                                                                                            it->start_same_saC->start_same_saC==it &&
                                                                                            it->start_same_saC >= it)
                                                                                        {
                                                                                          // it is at the beginning of a saC slice
                                                                                          const label_index old_t_label=t_label;
                                                                                          t_label=label_or_divergence(t, null_action);
                                                                                          to_constln=m_states[t.to()].block->c.onstellation;
                                                                                          mCRL2log(log::debug) << "    -  -  -  - saC slice of "
                                                                                                << (null_action==t_label ? "divergent self-loop " : pp(m_aut.action_label(t_label))+"-")
                                                                                                << "transitions to " << to_constln->debug_id(*this)
                                                                                                << (m_aut.is_tau(t_label) && !m_aut.is_tau(old_t_label)
                                                                                                        ? " -- error: tau-transitions should come first\n"
                                                                                                        : ":\n");
                                                                                        }
                                                                                        mCRL2log(log::debug) << "    " << ptr(t);
                                                                                        if (start_same_saC_valid)
                                                                                        {
                                                                                          if (label_or_divergence(t, null_action)!=t_label)
                                                                                          {
                                                                                            mCRL2log(log::debug) << " -- error: different label";
                                                                                          }
                                                                                          if (!initialisation && m_states[t.to()].block->c.onstellation!=to_constln)
                                                                                          {
                                                                                            mCRL2log(log::debug) << " -- error: different target " << m_states[t.to()].block->c.onstellation->debug_id(*this);
                                                                                          }
                                                                                          if (it->start_same_saC->start_same_saC == it)
                                                                                          {
                                                                                            // Transition t must be the beginning and/or the end of a saC-slice
                                                                                            if (it->start_same_saC >= it && it > m_outgoing_transitions.cbegin())
                                                                                            {
                                                                                              // Transition t must be the beginning of a saC-slice
                                                                                              const transition& prev_t=m_aut.get_transitions()[
                                                                                                        !initialisation ? *std::prev(it)->ref.BLC_transitions
                                                                                                                            : std::prev(it)->ref.transitions];
                                                                                              if (prev_t.from()==t.from() &&
                                                                                                  label_or_divergence(prev_t)==t_label &&
                                                                                                  (initialisation ||
                                                                                                   m_states[prev_t.to()].block->c.onstellation==
                                                                                                                       m_states[t.to()].block->c.onstellation))
                                                                                              {
                                                                                                mCRL2log(log::debug) << " -- error: not the beginning of a saC-slice";
                                                                                              }
                                                                                            }
                                                                                            if (it->start_same_saC <= it && std::next(it) < m_outgoing_transitions.end())
                                                                                            {
                                                                                              // Transition t must be the end of a saC-slice
                                                                                              const transition& next_t=m_aut.get_transitions()[
                                                                                                        !initialisation ? *std::next(it)->ref.BLC_transitions
                                                                                                                            : std::next(it)->ref.transitions];
                                                                                              if (next_t.from()==t.from() &&
                                                                                                  label_or_divergence(next_t)==t_label &&
                                                                                                  (initialisation ||
                                                                                                   m_states[next_t.to()].block->c.onstellation==
                                                                                                                       m_states[t.to()].block->c.onstellation))
                                                                                              {
                                                                                                mCRL2log(log::debug) << " -- error: not the end of a saC-slice";
                                                                                              }
                                                                                            }
                                                                                          }
                                                                                          else if (it->start_same_saC > it ? it->start_same_saC->start_same_saC > it : it->start_same_saC->start_same_saC < it)
                                                                                          {
                                                                                            mCRL2log(log::debug) << " -- error: not pointing to its own saC-slice";
                                                                                          }
                                                                                        }
                                                                                        mCRL2log(log::debug) << '\n';
                                                                                      }
                                                                                      mCRL2log(log::debug) << "  Ref states in blocks: " << std::distance<fixed_vector<state_type_gj>::const_iterator>(m_states.cbegin(), m_states[si].ref_states_in_blocks->ref_state) << ". Must be " << si <<".\n";
                                                                                      mCRL2log(log::debug) << "---------------------------------------------------\n";
                                                                                    }
                                                                                    mCRL2log(log::debug) << "++++++++++++++++++++ Transitions ++++++++++++++++++++++++++++\n";
                                                                                    for(transition_index ti=0; ti<m_aut.num_transitions(); ++ti)
                                                                                    {
                                                                                      const transition& t=m_aut.get_transitions()[ti];
                                                                                      mCRL2log(log::debug) << "Transition " << ti <<": " << t.from()
                                                                                                                            << " -" << m_aut.action_label(t.label()) << "-> "
                                                                                                                            << t.to() << "\n";
                                                                                    }

                                                                                    mCRL2log(log::debug) << "++++++++++++++++++++ Blocks ++++++++++++++++++++++++++++\n";
                                                                                    for (const state_in_block_pointer* si=m_states_in_blocks.data();
                                                                                        m_states_in_blocks.data_end()!=si; si=si->ref_state->block->end_states)
                                                                                    {
                                                                                      block_type* const bi=si->ref_state->block;
                                                                                      mCRL2log(log::debug) << "  Block " << bi;
                                                                                      if (!initialisation) {
                                                                                        mCRL2log(log::debug) << " (" << bi->c.onstellation->debug_id(*this) << ')';
                                                                                      }
                                                                                      mCRL2log(log::debug) << ":\n  " << std::distance(bi->start_bottom_states,
                                                                                                                       bi->sta.rt_non_bottom_states)
                                                                                          << (m_branching ? " Bottom state" : " State")
                                                                                          << (1==std::distance(bi->start_bottom_states,
                                                                                                               bi->sta.rt_non_bottom_states) ? ": " : "s: ");
                                                                                      for (const state_in_block_pointer*
                                                                                         sit=bi->start_bottom_states; sit!=bi->sta.rt_non_bottom_states; ++sit)
                                                                                      {
                                                                                        mCRL2log(log::debug) << sit->ref_state->debug_id_short(*this) << "  ";
                                                                                      }
                                                                                      if (m_branching)
                                                                                      {
                                                                                        mCRL2log(log::debug) << "\n  " << std::distance
                                                                                                                 (bi->sta.rt_non_bottom_states, bi->end_states)
                                                                                            << " Non-bottom state" << (1==std::distance
                                                                                                                 (bi->sta.rt_non_bottom_states, bi->end_states)
                                                                                                                  ? ": " : "s: ");
                                                                                        for (const state_in_block_pointer*
                                                                                                  sit=bi->sta.rt_non_bottom_states; sit!=bi->end_states; ++sit)
                                                                                        {
                                                                                          mCRL2log(log::debug) << sit->ref_state->debug_id_short(*this) << "  ";
                                                                                        }
                                                                                      }
                                                                                      else
                                                                                      {
                                                                                        assert(bi->sta.rt_non_bottom_states==bi->end_states);
                                                                                      }
                                                                                      if (!initialisation)
                                                                                      {
                                                                                        display_BLC_list(bi);
                                                                                      }
                                                                                      mCRL2log(log::debug) << "\n";
                                                                                    }

                                                                                    mCRL2log(log::debug) << "++++++++++++++++++++ Constellations ++++++++++++++++++++++++++++\n";
                                                                                    for (const state_in_block_pointer* si=m_states_in_blocks.data();
                                                                                                     m_states_in_blocks.data_end()!=si;
                                                                                                     si=si->ref_state->block->c.onstellation->end_const_states)
                                                                                    {
                                                                                      const constellation_type* const ci=si->ref_state->block->c.onstellation;
                                                                                      mCRL2log(log::debug) << "  " << ci->debug_id(*this) << ":\n";
                                                                                      mCRL2log(log::debug) << "    Blocks in constellation:";
                                                                                      for (const state_in_block_pointer*
                                                                                                            constln_it=ci->start_const_states;
                                                                                                            constln_it<ci->end_const_states; )
                                                                                      {
                                                                                        const block_type* const bi=constln_it->ref_state->block;
                                                                                        mCRL2log(log::debug) << " " << bi->debug_id(*this);
                                                                                        constln_it = bi->end_states;
                                                                                      }
                                                                                      mCRL2log(log::debug) << "\n";
                                                                                    }
                                                                                    mCRL2log(log::debug) << "Non-trivial constellations:";
                                                                                    for (const constellation_type* ci: m_non_trivial_constellations)
                                                                                    {
                                                                                      mCRL2log(log::debug) << " " << ci->debug_id(*this);
                                                                                    }

                                                                                    mCRL2log(log::debug) <<
                                                                                         "\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n"
                                                                                         "Outgoing transitions:\n";

                                                                                    for (outgoing_transitions_const_it pi = m_outgoing_transitions.cbegin();
                                                                                                                       pi < m_outgoing_transitions.cend(); ++pi)
                                                                                    {
                                                                                      const transition& t=m_aut.get_transitions()[!initialisation
                                                                                                             ? *pi->ref.BLC_transitions : pi->ref.transitions];
                                                                                      mCRL2log(log::debug) << "  " << t.from() << " -"
                                                                                                           << m_aut.action_label(t.label()) << "-> " << t.to();
                                                                                      if (m_outgoing_transitions.cbegin()<=pi->start_same_saC &&
                                                                                          pi->start_same_saC<m_outgoing_transitions.end())
                                                                                      {
                                                                                        const transition& t1=m_aut.get_transitions()[!initialisation
                                                                                                                     ? *pi->start_same_saC->ref.BLC_transitions
                                                                                                                     : pi->start_same_saC->ref.transitions];
                                                                                        mCRL2log(log::debug) << "  \t(same saC: " << t1.from() << " -" << m_aut.action_label(t1.label()) << "-> " << t1.to();
                                                                                        const label_index t_label = label_or_divergence(t);
                                                                                        if (pi->start_same_saC->start_same_saC == pi)
                                                                                        {
                                                                                          // Transition t must be the beginning and/or the end of a saC-slice
                                                                                          if (pi->start_same_saC >= pi && pi > m_outgoing_transitions.cbegin())
                                                                                          {
                                                                                            // Transition t must be the beginning of a saC-slice
                                                                                            const transition& prev_t=m_aut.get_transitions()[
                                                                                                      !initialisation ? *std::prev(pi)->ref.BLC_transitions
                                                                                                                          : std::prev(pi)->ref.transitions];
                                                                                            if (prev_t.from()==t.from() &&
                                                                                                label_or_divergence(prev_t)==t_label &&
                                                                                                (initialisation ||
                                                                                                 m_states[prev_t.to()].block->c.onstellation==
                                                                                                                       m_states[t.to()].block->c.onstellation))
                                                                                            {
                                                                                              mCRL2log(log::debug) << " -- error: not the beginning of a saC-slice";
                                                                                            }
                                                                                          }
                                                                                          if (pi->start_same_saC <= pi && std::next(pi) < m_outgoing_transitions.end())
                                                                                          {
                                                                                            // Transition t must be the end of a saC-slice
                                                                                            const transition& next_t=m_aut.get_transitions()[
                                                                                                      !initialisation ? *std::next(pi)->ref.BLC_transitions
                                                                                                                          : std::next(pi)->ref.transitions];
                                                                                            if (next_t.from()==t.from() &&
                                                                                                label_or_divergence(next_t)==t_label &&
                                                                                                (initialisation ||
                                                                                                 m_states[next_t.to()].block->c.onstellation==
                                                                                                                       m_states[t.to()].block->c.onstellation))
                                                                                            {
                                                                                              mCRL2log(log::debug) << " -- error: not the end of a saC-slice";
                                                                                            }
                                                                                          }
                                                                                        }
                                                                                        else if (pi->start_same_saC > pi ? pi->start_same_saC->start_same_saC > pi : pi->start_same_saC->start_same_saC < pi)
                                                                                        {
                                                                                          mCRL2log(log::debug) << " -- error: not in its own saC-slice";
                                                                                        }
                                                                                        mCRL2log(log::debug) << ')';
                                                                                      }
                                                                                      mCRL2log(log::debug) << '\n';
                                                                                    }
                                                                                    mCRL2log(log::debug) << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n"
                                                                                                            "New bottom blocks to be investigated:";

                                                                                    for(const block_type* bi: m_blocks_with_new_bottom_states)
                                                                                    {
                                                                                      mCRL2log(log::debug) << "  " << bi->debug_id(*this) << '\n';
                                                                                    }

                                                                                    mCRL2log(log::debug) << "\n========= END PRINT DATASTRUCTURE: " << header << " =======================================\n";
                                                                                  }
                                                                                #endif // ifndef NDEBUG
  public:
    /// \brief Calculate the number of equivalence classes
    /// \details The number of equivalence classes (which is valid after the
    /// partition has been constructed) is equal to the number of states in the
    /// bisimulation quotient.
    std::size_t num_eq_classes() const
    {
      return no_of_blocks;
    }


    /// \brief Get the equivalence class of a state
    /// \details After running the minimisation algorithm, this function
    /// produces the number of the equivalence class of a state.  This number
    /// is the same as the number of the state in the minimised LTS to which
    /// the original state is mapped.
    /// \param s state whose equivalence class needs to be found
    /// \returns sequence number of the equivalence class of state s
    state_index get_eq_class(const state_index si) const
    {                                                                           assert(si<m_states.size());
      return m_states[si].block->sta.te_in_reduced_LTS;
    }


    /// \brief Adapt the LTS after minimisation
    /// \details After the efficient branching bisimulation minimisation, the
    /// information about the quotient LTS is only stored in the partition data
    /// structure of the partitioner object.  This function exports the
    /// information back to the LTS by adapting its states and transitions:  it
    /// updates the number of states and adds those transitions that are
    /// mandated by the partition data structure.  If desired, it also creates
    /// a vector containing an arbritrary (example) original state per
    /// equivalence class.
    ///
    /// The main parameter and return value are implicit with this function: a
    /// reference to the LTS was stored in the object by the constructor.
    void finalize_minimized_LTS()
    {
      // Assign numbers to the blocks (i.e. to the states of the reduced LTS)
      // One could devise a fancy scheme where the block containing state i
      // tries to get block number i; but let's just do something simple now.
      // We no longer need sta.rt_non_bottom_states at this moment, so we can
      // reuse that field to store the block number:
      state_index block_number=0;
      for (state_in_block_pointer*
            si=m_states_in_blocks.data(); m_states_in_blocks.data_end()!=si;
                                           si=si->ref_state->block->end_states)
      {
        block_type* const bi=si->ref_state->block;
        // destruct bi->sta.rt_non_bottom_states; -- trivial
        new (&bi->sta.te_in_reduced_LTS) state_index(block_number);
        ++block_number;
      }

      // The transitions are most efficiently directly extracted from the
      // block.to_constellation lists in blocks.
      typename std::remove_reference<decltype(m_aut.get_transitions())>::type
                                                                             T;
      for (state_in_block_pointer*
            si=m_states_in_blocks.data(); m_states_in_blocks.data_end()!=si;
                                           si=si->ref_state->block->end_states)
      {
        const block_type& B=*si->ref_state->block;                              //mCRL2complexity(&B, add_work(..., 1), *this);
                                                                                    // Because every block is touched exactly once, we do not store a
                                                                                    // physical counter for this.
        for(const BLC_indicators blc_ind: B.block.to_constellation)
        {                                                                       // mCRL2complexity(&blc_ind, add_work(..., 1), *this);
                                                                                    // Because every BLC set is touched exactly once, we do not store
                                                                                    // a physical counter for this.
                                                                                assert(blc_ind.start_same_BLC<blc_ind.end_same_BLC);
          const transition& t=m_aut.get_transitions()[*blc_ind.start_same_BLC];
          const state_index new_to=get_eq_class(t.to());
          if (!is_inert_during_init(t) || B.sta.te_in_reduced_LTS!=new_to)
          {
            T.emplace_back(B.sta.te_in_reduced_LTS, t.label(), new_to);
          }
        }
      }
      m_aut.clear_transitions();
      T.swap(m_aut.get_transitions());
      //
      // Merge the states, by setting the state labels of each state to the
      // concatenation of the state labels of its equivalence class.

      if (m_aut.has_state_info())   // If there are no state labels
      {                             // this step is not needed
        /* Create a vector for the new labels */
        typename std::remove_reference<decltype(m_aut.state_labels())>::type
                                                  new_labels(num_eq_classes());

        for(std::size_t i=0; i<m_aut.num_states(); ++i)
        {                                                                       //mCRL2complexity(&m_states[i], add_work(..., 1), *this);
                                                                                    // Because every state is touched exactly once, we do not store a
                                                                                    // physical counter for this.
          const state_index new_index(get_eq_class(i));
          new_labels[new_index]=new_labels[new_index]+m_aut.state_label(i);
        }

        m_aut.set_num_states(num_eq_classes(), false);                          assert(0==m_aut.num_state_labels());
        //m_aut.clear_state_labels();
        new_labels.swap(m_aut.state_labels());
      }
      else
      {
        m_aut.set_num_states(num_eq_classes(), false);
      }

      m_aut.set_initial_state(get_eq_class(m_aut.initial_state()));
    }


    /// \brief Check whether two states are in the same equivalence class.
    /// \param s first state that needs to be compared.
    /// \param t second state that needs to be compared.
    /// \returns true iff the two states are in the same equivalence class.
    bool in_same_class(state_index const s, state_index const t) const
    {
      return get_eq_class(s) == get_eq_class(t);
    }
  private:
                                                                                #ifndef NDEBUG
                                                                                  std::string ptr(const transition& t) const
                                                                                  {
                                                                                    return std::to_string(t.from())+" -"+pp(m_aut.action_label(t.label()))+
                                                                                                                                "-> "+std::to_string(t.to());
                                                                                  }
                                                                                #endif
    /*--------------------------- main algorithm ----------------------------*/

    /*----------------- splitB -- Algorithm 3 of [GJ 2024] -----------------*/

    /// \brief return the number of states in block `B`
    state_index number_of_states_in_block(const block_type& B) const
    {                                                                           assert(B.start_bottom_states<B.end_states);
      return std::distance(B.start_bottom_states, B.end_states);
    }

    /// \brief return the number of states in constellation `C`
    state_index number_of_states_in_constellation(const constellation_type& C)
                                                                          const
    {                                                                           assert(C.start_const_states<C.end_const_states);
      return std::distance(C.start_const_states, C.end_const_states);
    }

    /// \brief swap the contents of `pos1` and `pos2`, assuming they are different
    void swap_states_in_states_in_block_never_equal(
                    state_in_block_pointer* pos1, state_in_block_pointer* pos2)
    {                                                                           assert(m_states_in_blocks.data()<=pos1);
      std::swap(*pos1,*pos2);                                                   assert(pos1<m_states_in_blocks.data_end());
      pos1->ref_state->ref_states_in_blocks=pos1;                               assert(m_states_in_blocks.data()<=pos2);
      pos2->ref_state->ref_states_in_blocks=pos2;                               assert(pos2<m_states_in_blocks.data_end());  assert(pos1!=pos2);
    }

    /// \brief swap the contents of `pos1` and `pos2` if they are different
    void swap_states_in_states_in_block(
                    state_in_block_pointer* pos1, state_in_block_pointer* pos2)
    {
      if (pos1!=pos2)
      {
        swap_states_in_states_in_block_never_equal(pos1, pos2);
      }
    }

    /// \brief Move the contents of `pos1` to `pos2`, those of `pos2` to `pos3` and those of `pos3` to `pos1`
    /// \details The function requires that `pos3` lies in between `pos1` and
    /// `pos2`.  It also requires that `pos2` and `pos3` are different.
    void swap_states_in_states_in_block_23_never_equal(
              state_in_block_pointer* pos1,
              state_in_block_pointer* pos2,
              state_in_block_pointer* pos3)
    {                                                                           assert(m_states_in_blocks.data()<=pos2);   assert(pos2<pos3);
                                                                                assert(pos1<m_states_in_blocks.data_end());
      if (pos1==pos3)
      {
        std::swap(*pos1,*pos2);
      }
      else
      {                                                                         assert(pos3<pos1);
        const state_in_block_pointer temp=*pos1;
        *pos1=*pos3;
        *pos3=*pos2;
        *pos2=temp;

        pos3->ref_state->ref_states_in_blocks=pos3;
      }
      pos1->ref_state->ref_states_in_blocks=pos1;
      pos2->ref_state->ref_states_in_blocks=pos2;
    }

    /// \brief Move the contents of `pos1` to `pos2`, those of `pos2` to `pos3` and those of `pos3` to `pos1`
    /// \details The function requires that `pos3` lies in between `pos1` and
    /// `pos2`.  The swap is only executed if the positions are different.
    void swap_states_in_states_in_block(
              state_in_block_pointer* pos1,
              state_in_block_pointer* pos2,
              state_in_block_pointer* pos3)
    {
      if (pos2==pos3)
      {
        swap_states_in_states_in_block(pos1,pos2);
      }
      else
      {
        swap_states_in_states_in_block_23_never_equal(pos1,pos2,pos3);
      }
    }

    /// \brief Swap the range [`pos1`, `pos1` + `count`) with the range [`pos2`, `pos2` + `count`)
    /// \details `pos1` must come before `pos2`.
    /// (If the ranges overlap, only swap the non-overlapping part.)
    /// The function requires `count` > 0 and `pos1` < `pos2`
    /// (this is sufficient for how it's used below: to swap new bottom states
    /// into their proper places; also, the work counters assume that
    /// [`assign_work_to`, `assign_work_to` + `count`) is assigned the work.)
    void multiple_swap_states_in_states_in_block(
              state_in_block_pointer* pos1,
              state_in_block_pointer* pos2,
              state_index count
                                                                                #if !defined(NDEBUG) || defined(COUNT_WORK_BALANCE)
                                                                                  , const state_in_block_pointer* assign_work_to,
                                                                                    unsigned char const max_B,
                                                                                    enum check_complexity::counter_type const ctr=check_complexity::
                                                                                                       multiple_swap_states_in_block__swap_state_in_small_block
                                                                                #endif
              )
    {                                                                           assert(count<m_aut.num_states());  assert(m_states_in_blocks.data()<=pos1);
      /* if (pos1 > pos2)  std::swap(pos1, pos2);                            */ assert(pos1<pos2);  assert(pos2<=m_states_in_blocks.data_end()-count);
      {
        std::make_signed<state_index>::type
                                       overlap=std::distance(pos2, pos1)+count;
        if (overlap > 0)
        {
          count -= overlap;
                                                                                #if !defined(NDEBUG) || defined(COUNT_WORK_BALANCE)
                                                                                  // If we do not change `assign_work_to`, then there should be no overlap
                                                                                  // between the area starting at `pos2` and the one at `assign_work_to`;
                                                                                  // otherwise it may happen that work is assigned to unexpected counters.
                                                                                  if (pos2==assign_work_to) {
                                                                                    assign_work_to+=overlap;
                                                                                  } else  {  assert(assign_work_to+count<=pos2+overlap ||
                                                                                                    pos2+overlap+count<=assign_work_to);  }
                                                                                #endif
          pos2 += overlap;
        }
      }                                                                         assert(0 < count);
      state_in_block_pointer temp=*pos1;
      while (--count > 0)
      {                                                                         mCRL2complexity(assign_work_to->ref_state, add_work(ctr, max_B), *this);
                                                                                #if !defined(NDEBUG) || defined(COUNT_WORK_BALANCE)
                                                                                  ++assign_work_to;
                                                                                #endif
        *pos1 = *pos2;
        pos1->ref_state->ref_states_in_blocks=pos1;
        ++pos1;
        *pos2 = *pos1;
        pos2->ref_state->ref_states_in_blocks=pos2;
        ++pos2;
      }
      *pos1 = *pos2;
      pos1->ref_state->ref_states_in_blocks=pos1;
      *pos2 = temp;
      pos2->ref_state->ref_states_in_blocks=pos2;
                                                                                #ifndef NDEBUG
                                                                                  for (fixed_vector<state_type_gj>::const_iterator
                                                                                                                si=m_states.cbegin(); si<m_states.cend(); ++si)
                                                                                  {
                                                                                    assert(si==si->ref_states_in_blocks->ref_state);
                                                                                  }
                                                                                #endif
    }

    /// \brief marks the transition indicated by `out_pos`.
    /// \details (We use an outgoing_transitions_it because it points to the
    /// m_BLC_transitions entry that needs to be updated.)
    void mark_BLC_transition(const outgoing_transitions_it out_pos)
    {
      BLC_list_iterator old_pos = out_pos->ref.BLC_transitions;
      linked_list<BLC_indicators>::iterator ind =
                m_transitions[*old_pos].transitions_per_block_to_constellation; assert(ind->start_same_BLC<=old_pos);
                                                                                assert(old_pos<m_BLC_transitions.data_end());
                                                                                assert(old_pos<ind->end_same_BLC);  assert(!ind->is_stable());
      if (old_pos < ind->start_marked_BLC)
      {
        /* The transition is not marked */                                      assert(ind->start_same_BLC<ind->start_marked_BLC);
        BLC_list_iterator new_pos = std::prev(ind->start_marked_BLC);           assert(ind->start_same_BLC<=new_pos);  assert(new_pos<ind->end_same_BLC);
                                                                                assert(new_pos<m_BLC_transitions.data_end());
        if (old_pos < new_pos)
        {
          std::swap(*old_pos, *new_pos);
          m_transitions[*old_pos].ref_outgoing_transitions->
                                                 ref.BLC_transitions = old_pos; assert(out_pos==m_transitions[*new_pos].ref_outgoing_transitions);
          out_pos->ref.BLC_transitions = new_pos;
        }
        ind->start_marked_BLC--;
      }

                                                                                #ifndef NDEBUG
                                                                                  for (BLC_list_const_iterator it=m_BLC_transitions.data();
                                                                                                                         it<m_BLC_transitions.data_end(); ++it)
                                                                                  {
                                                                                    assert(m_transitions[*it].ref_outgoing_transitions->ref.BLC_transitions==
                                                                                                                                                           it);
                                                                                    assert(m_transitions[*it].transitions_per_block_to_constellation->
                                                                                                                                           start_same_BLC<=it);
                                                                                    assert(it<
                                                                                      m_transitions[*it].transitions_per_block_to_constellation->end_same_BLC);
                                                                                  }
                                                                                #endif
    }

#ifndef USE_FOUR_WAY_SPLIT
    /// Split the block `B` by moving the elements in R to the front in m_states,
    /// and add a new element B_new at the end of m_blocks referring to R.
    /// Adapt B.start_bottom_states, B.sta.rt_non_bottom_states and
    /// B.end_states, and do the same for B_new.
    /// returns pointer to the new block
    template <bool initialisation=false>
    block_type* split_block_B_into_R_and_BminR(
        block_type* const B,
        state_in_block_pointer* first_bottom_state_in_R,
        state_in_block_pointer* last_bottom_state_in_R,
        const todo_state_vector& R)
    {                                                                           assert(B->start_bottom_states<=first_bottom_state_in_R);
//std::cerr << "split_block_B_into_R_and_BminR" << (initialisation ? "<true>(" : "(") << B->debug_id(*this)
//<< ",&m_states_in_blocks[" << std::distance(m_states_in_blocks.begin(), first_bottom_state_in_R)
//<< "..." << std::distance(m_states_in_blocks.begin(), last_bottom_state_in_R)
//<< "),R = {";
//for(auto s:R){ std::cerr << ' ' << std::distance(m_states.begin(), s.ref_state); }
//std::cerr << " }"
//")\n";
//std::cerr << "SPLIT BLOCK " << B << " by removing "; for(auto s = first_bottom_state_in_R; s < last_bottom_state_in_R; ++s){ std::cerr << std::distance(m_states.begin(), s->ref_state) << ' ';} for(auto s:R){ std::cerr << ' ' << std::distance(m_states.begin(), s.ref_state); } std::cerr << '\n';
      /* Basic administration. Make a new block and add it to the current    */ assert(first_bottom_state_in_R<=last_bottom_state_in_R);
      /* constellation.                                                      */ assert(last_bottom_state_in_R<=B->sta.rt_non_bottom_states);
      constellation_type* const ci=B->c.onstellation;
      block_type* const B_new=
                #ifdef USE_POOL_ALLOCATOR
                    simple_list<BLC_indicators>::get_pool().
                    template construct<block_type>
                #else
                    new block_type
                #endif
                                  (B->start_bottom_states, ci);
      ++no_of_blocks;
                                                                                #if !defined(NDEBUG) || defined(COUNT_WORK_BALANCE)
                                                                                  B_new->work_counter=B->work_counter;
                                                                                #endif
                                                                                assert(m_states_in_blocks.data()<=ci->start_const_states);
                                                                                assert(ci->start_const_states<ci->end_const_states);
                                                                                assert(ci->end_const_states<=m_states_in_blocks.data_end());
      if (ci->start_const_states->ref_state->block==
            std::prev(ci->end_const_states)->ref_state->block)
      {                                                                         assert(std::find(m_non_trivial_constellations.begin(),
        /* This constellation was trivial, as it will be split add it to the */                  m_non_trivial_constellations.end(),
        /* non-trivial constellations.                                       */                  ci)==m_non_trivial_constellations.end());
        m_non_trivial_constellations.emplace_back(ci);
      }                                                                         // The size of the new block is not yet number_of_states_in_block(...)
                                                                                #if !defined(NDEBUG) || defined(COUNT_WORK_BALANCE)
                                                                                  unsigned char const max_B = check_complexity::log_n -
                                                                                      check_complexity::ilog2(std::distance(first_bottom_state_in_R,
      /* Carry out the split.                                                */                                             last_bottom_state_in_R) + R.size());
                                                                                #endif
      if (B->start_bottom_states!=first_bottom_state_in_R)
      {                                                                         assert(first_bottom_state_in_R!=last_bottom_state_in_R);
        multiple_swap_states_in_states_in_block(
                B->start_bottom_states, first_bottom_state_in_R,
                std::distance(first_bottom_state_in_R, last_bottom_state_in_R)
                                                                                #if !defined(NDEBUG) || defined(COUNT_WORK_BALANCE)
                                                                                  , first_bottom_state_in_R, max_B
                                                                                #endif
                );
        last_bottom_state_in_R = B->start_bottom_states +
                std::distance(first_bottom_state_in_R, last_bottom_state_in_R);
        first_bottom_state_in_R = B->start_bottom_states;
      }
                                                                                assert(B_new->start_bottom_states==first_bottom_state_in_R);
      B_new->sta.rt_non_bottom_states=last_bottom_state_in_R;
      // Update the block pointers for R-bottom states:
      for (state_in_block_pointer*
             s_it=first_bottom_state_in_R; s_it<last_bottom_state_in_R; ++s_it)
      {                                                                         mCRL2complexity(s_it->ref_state, add_work(check_complexity::
                                                                                                split_block_B_into_R_and_BminR__carry_out_split, max_B), *this);
//std::cerr << "MOVE STATE TO NEW BLOCK: " << std::distance(m_states.begin(), s_it->ref_state) << "\n";
                                                                                assert(B==s_it->ref_state->block);
        s_it->ref_state->block=B_new;                                           assert(s_it->ref_state->ref_states_in_blocks==s_it);
      }
      // Now the R bottom states are in the correct position, and we don't have
      // to look into them any more.
      B_new->end_states=last_bottom_state_in_R;
      B->start_bottom_states=last_bottom_state_in_R;

        // Move the non-bottom states to their correct positions:
        for(state_in_block_pointer s: R)
        {                                                                       mCRL2complexity(s.ref_state, add_work(check_complexity::
                                                                                               split_block_B_into_R_and_BminR__carry_out_split, max_B), *this);
//std::cerr << "MOVE STATE TO NEW BLOCK: " << s << "\n";
                                                                                assert(B==s.ref_state->block);
          s.ref_state->block=B_new;
          state_in_block_pointer* pos=s.ref_state->ref_states_in_blocks;        assert(pos>=B->sta.rt_non_bottom_states); // it's a non-bottom state
          // pos -> B.start_bottom_states -> B.sta.rt_non_bottom_states -> pos
          swap_states_in_states_in_block(pos, B->start_bottom_states,
                                                  B->sta.rt_non_bottom_states);
          B->sta.rt_non_bottom_states++;
          B->start_bottom_states++;
          B_new->end_states++;                                                  assert(B->start_bottom_states<=B->sta.rt_non_bottom_states);
                                                                                assert(B_new->start_bottom_states<B_new->end_states);
        }
      return B_new;
    }
#endif

    /// \brief Move the content of i1 to i2, i2 to i3 and i3 to i1.
    void swap_three_iterators_and_update_m_transitions(
                                                    const BLC_list_iterator i1,
                                                    const BLC_list_iterator i2,
                                                    const BLC_list_iterator i3)
    {                                                                           assert(i3<=i2);  assert(i2<=i1);
      if (i1==i3)
      {
        return;
      }
      if ((i1==i2)||(i2==i3))
      {
        std::swap(*i1,*i3);
        m_transitions[*i1].ref_outgoing_transitions->ref.BLC_transitions = i1;
        m_transitions[*i3].ref_outgoing_transitions->ref.BLC_transitions = i3;
      }
      else  // swap all three elements.
      {
        transition_index temp = *i1;
        *i1=*i2;
        *i2=*i3;
        *i3=temp;
        m_transitions[*i1].ref_outgoing_transitions->ref.BLC_transitions = i1;
        m_transitions[*i2].ref_outgoing_transitions->ref.BLC_transitions = i2;
        m_transitions[*i3].ref_outgoing_transitions->ref.BLC_transitions = i3;
      }
    }

    /// \brief Swap transition `ti` from BLC set `old_BLC_block` to BLC set `new_BLC_block`
    /// \param ti             transition that needs to be swapped
    /// \param new_BLC_block  new BLC set, where the transition should go to
    /// \param old_BLC_block  old BLC set, where the transition was in originally
    /// \returns true iff the last element of `old_BLC_block` has been removed
    /// \details It is assumed that the new BLC set is located precisely before
    /// the old BLC set in `m_BLC_transitions`.
    /// This routine cannot be used in the initialisation phase, but only
    /// during refinement.
    ///
    /// This variant of the swap routine assumes that transition `ti` is only
    /// marked if it is in a singleton block or in a block containing new
    /// bottom states.  In both cases, it is not necessary to maintain
    /// transition markings; so `ti` will always be treated as unmarked, and
    /// the new BLC set must be stable.
    /// (However, it may happen that other transitions in `old_BLC_block` are
    /// marked, and then their marking must be kept.)
    [[nodiscard]]
    bool swap_in_the_doubly_linked_list_LBC_in_blocks_new_constellation(
               const transition_index ti,
               linked_list<BLC_indicators>::iterator new_BLC_block,
               linked_list<BLC_indicators>::iterator old_BLC_block)
    {                                                                           assert(new_BLC_block->is_stable());
      BLC_list_iterator old_position=
               m_transitions[ti].ref_outgoing_transitions->ref.BLC_transitions; assert(old_BLC_block->start_same_BLC <= old_position);
                                                                                assert(old_position<old_BLC_block->end_same_BLC);
                                                                                assert(new_BLC_block->end_same_BLC==old_BLC_block->start_same_BLC);
                                                                                assert(m_transitions[ti].transitions_per_block_to_constellation == old_BLC_block);
                                                                                assert(ti == *old_position);
      if (!old_BLC_block->is_stable())
      {                                                                         assert(old_BLC_block->start_same_BLC<=old_BLC_block->start_marked_BLC);
                                                                                assert(old_BLC_block->start_marked_BLC<=old_BLC_block->end_same_BLC);
        if (old_BLC_block->start_marked_BLC<=old_position)
        {                                                                       assert(m_states[m_aut.get_transitions()[ti].from()].block->
                                                                                                                                  contains_new_bottom_states ||
                                                                                       number_of_states_in_block
                                                                                                     (*m_states[m_aut.get_transitions()[ti].from()].block)<=1);
          // It's ok to unmark transitions because they start in blocks with
          // new bottom states, or in blocks with only 1 state.
          // (However, it may happen that unmarked transitions from other blocks
          // are moved, even if there are marked transitions in the same BLC set---
          // namely for the co-splitters. So we can only unmark transitions if
          // transition ti itself is marked.)
          old_BLC_block->start_marked_BLC=old_BLC_block->end_same_BLC;
        }
      }
      if (old_position!=old_BLC_block->start_same_BLC)
      {
        std::swap(*old_position,*old_BLC_block->start_same_BLC);
        m_transitions[*old_position].ref_outgoing_transitions->
                                            ref.BLC_transitions = old_position;
        m_transitions[*old_BLC_block->start_same_BLC].
                    ref_outgoing_transitions->ref.BLC_transitions =
                                                 old_BLC_block->start_same_BLC;
      }
      new_BLC_block->end_same_BLC=++old_BLC_block->start_same_BLC;
      m_transitions[ti].transitions_per_block_to_constellation=new_BLC_block;
//std::cerr << " to new " << new_BLC_block->debug_id(*this) << '\n';
      return old_BLC_block->start_same_BLC==old_BLC_block->end_same_BLC;
    }

    /// \brief Move transition `t` with transition index `ti` to a new BLC set
    /// \param index_block_B  block forming a new constellation, at the same time target of `t`
    /// \param t              transition that needs to be moved
    /// \param ti             (redundant) transition index of t
    /// \returns true iff a new BLC set for non-constellation-inert transitions has been created
    /// \details Called if the target state of transition `t` switches to a new
    /// constellation; at the moment of calling, the new constellation only
    /// contains block `index_block_B`.
    ///
    /// If the transition is not constellation-inert (or does not remain
    /// constellation-inert), it is moved to a BLC set just after the current
    /// BLC set in its list of BLC sets.  If no suitable BLC set exists yet, it
    /// will be created in that position of the list.  In this way, a main
    /// splitter (i.e. a BLC set with transitions to the new constellation)
    /// will always immediately succeed its co-splitter.
    ///
    /// Counting the number of BLC sets requires that the new block still has
    /// the old constellation number.
    [[nodiscard]]
    bool update_the_doubly_linked_list_LBC_new_constellation(
               block_type* const index_block_B,
               const transition& t,
               const transition_index ti)
    {                                                                           assert(m_states[t.to()].block==index_block_B);
//std::cerr << "update_the_doubly_linked_list_LBC_new_constellation(" << index_block_B->debug_id(*this) << ',' << m_transitions[ti].debug_id(*this) << ")\n";
      block_type* const from_block=m_states[t.from()].block;                    assert(&m_aut.get_transitions()[ti] == &t);
      bool new_block_created = false;                                           assert(from_block->block.to_constellation.check_linked_list());
      linked_list<BLC_indicators>::iterator this_block_to_constellation=
                      m_transitions[ti].transitions_per_block_to_constellation;
                                                                                #ifndef NDEBUG
                                                                                  // Check whether this_block_to_constellation is in the corresponding list
                                                                                  for (linked_list<BLC_indicators>::const_iterator i=from_block->block.to_constellation.begin();
                                                                                       i!=this_block_to_constellation; ++i)
                                                                                  {
                                                                                    assert(i!=from_block->block.to_constellation.end());
                                                                                  }
                                                                                #endif
                                                                                assert(this_block_to_constellation!=from_block->block.to_constellation.end());
                                                                                assert(this_block_to_constellation->start_same_BLC <= m_transitions[ti].ref_outgoing_transitions->ref.BLC_transitions);
                                                                                // the transition is never marked:
                                                                                assert(from_block->contains_new_bottom_states ||
                                                                                       number_of_states_in_block(*from_block)<=1 ||
                                                                                       this_block_to_constellation->is_stable() ||
                                                                                       m_transitions[ti].ref_outgoing_transitions->ref.BLC_transitions<this_block_to_constellation->start_marked_BLC);
      linked_list<BLC_indicators>::iterator next_block_to_constellation;
      // if this transition is inert, it is inserted in a block in front.
      // Otherwise, it is inserted after the current element in the list.
      if (is_inert_during_init(t) && from_block==index_block_B)
      {
        next_block_to_constellation=from_block->block.to_constellation.begin(); assert(next_block_to_constellation->start_same_BLC <
                                                                                                                      next_block_to_constellation->end_same_BLC);
                                                                                assert(m_states[m_aut.get_transitions()[*(next_block_to_constellation->start_same_BLC)].from()].block==index_block_B);
                                                                                assert(m_aut.is_tau(m_aut_apply_hidden_label_map(m_aut.get_transitions()[*(next_block_to_constellation->start_same_BLC)].label())));
        if (next_block_to_constellation==this_block_to_constellation)
        {
          // Make a new entry in the list block.to_constellation, at the
          // beginning;
//std::cerr << "Creating new BLC set for inert " << m_transitions[ti].debug_id(*this) << ": ";

          next_block_to_constellation=from_block->block.to_constellation.
                       emplace_front(//first_block_to_constellation,
                             this_block_to_constellation->start_same_BLC,
                             this_block_to_constellation->start_same_BLC,true);
                                                                                #if !defined(NDEBUG) || defined(COUNT_WORK_BALANCE)
                                                                                  next_block_to_constellation->work_counter = this_block_to_constellation->work_counter;
                                                                                #endif
        }                                                                       else
                                                                                {
//std::cerr << "Extending existing BLC set for inert " << m_transitions[ti].debug_id(*this) << ": ";
                                                                                  assert(m_states[m_aut.get_transitions()[*(next_block_to_constellation->start_same_BLC)].to()].block==index_block_B);
                                                                                }
      }
      else
      {
        // The transition is not constellation-inert.
        // The transition will be placed in a BLC set immediately after the BLC
        // set it came from, so that main splitters (with transitions to the
        // new constellation) come after co-splitters (with transitions to the
        // old constellation).

        // This method also ensures that transitions from the old constellation
        // to the old constellation will remain at the beginning of their
        // respective BLC set.
        next_block_to_constellation=from_block->
                      block.to_constellation.next(this_block_to_constellation);
        const transition* first_t;
        if (next_block_to_constellation==
                                    from_block->block.to_constellation.end() ||
            (first_t=&m_aut.get_transitions()
                      [*(next_block_to_constellation->start_same_BLC)],
             assert(m_states[first_t->from()].block==from_block),
             m_states[first_t->to()].block!=index_block_B) ||
            label_or_divergence(*first_t)!=label_or_divergence(t))
        {
//std::cerr << "Creating new BLC set for " << m_transitions[ti].debug_id(*this) << ": ";
          // Make a new entry in the list next_block_to_constellation, after the current list element.
          new_block_created = true;
          next_block_to_constellation=from_block->block.to_constellation.
                       emplace_after(this_block_to_constellation,
                             this_block_to_constellation->start_same_BLC,
                             this_block_to_constellation->start_same_BLC,true);
                                                                                #if !defined(NDEBUG) || defined(COUNT_WORK_BALANCE)
          /* The entry will be marked as unstable later                      */   next_block_to_constellation->work_counter=
                                                                                                                     this_block_to_constellation->work_counter;
                                                                                #endif
          ++no_of_non_constellation_inert_BLC_sets;
        }
//else { std::cerr << "Extending existing BLC set for " << m_transitions[ti].debug_id(*this) << ": "; }
      }

      if (swap_in_the_doubly_linked_list_LBC_in_blocks_new_constellation(ti,
                     next_block_to_constellation, this_block_to_constellation))
      {
        from_block->block.to_constellation.erase(this_block_to_constellation);
//std::cerr << "Erasing the old BLC set";
        if (!is_inert_during_init(t) ||
            from_block->c.onstellation!=index_block_B->c.onstellation)
            // i.e. if transition t is not a (formerly) inert transition (in
            // that case, the old BLC set would havold BLC set did *not*
            // contain the (formerly) inert tau-transitions from the old to the
            // new constellation, or the still-inert tau-transitions of the
            // old constellation
        {
          --no_of_non_constellation_inert_BLC_sets;
        }
      }
//std::cerr << '\n';
                                                                                #ifndef NDEBUG
                                                                                  check_transitions(false, false, false);
                                                                                #endif
      return new_block_created;
    }

    /// \brief Swap transition `ti` from BLC set `old_BLC_block` to BLC set `new_BLC_block`
    /// \param ti             transition that needs to be swapped
    /// \param new_BLC_block  new BLC set, where the transition should go to
    /// \param old_BLC_block  old BLC set, where the transition was in originally
    /// \returns true iff the last element of `old_BLC_block` has been removed
    /// \details It is assumed that the new BLC set is located precisely before
    /// the old BLC set in `m_BLC_transitions`.
    /// This routine cannot be used in the initialisation phase, but only
    /// during refinement.
    ///
    /// The stability state of old and new BLC set may be different.
    /// However, the procedure is only called for different stability states
    /// if `ti` is a constellation-inert transition and some *other*
    /// constellation-inert transitions have been marked for being no longer
    /// block-inert.  (So, it will not happen that `ti` is marked and the new
    /// BLC set is stable.)
    [[nodiscard]]
    bool swap_in_the_doubly_linked_list_LBC_in_blocks_new_block(
               const transition_index ti,
               linked_list<BLC_indicators>::iterator new_BLC_block,
               linked_list<BLC_indicators>::iterator old_BLC_block)
    {                                                                           assert(new_BLC_block->end_same_BLC==old_BLC_block->start_same_BLC);
      BLC_list_iterator old_position =
               m_transitions[ti].ref_outgoing_transitions->ref.BLC_transitions; assert(old_BLC_block->start_same_BLC<=old_position);
                                                                                assert(old_position<old_BLC_block->end_same_BLC);
                                                                                #ifndef NDEBUG
                                                                                  if (!old_BLC_block->is_stable())
                                                                                  {
                                                                                    assert(old_BLC_block->start_same_BLC<=old_BLC_block->start_marked_BLC);
                                                                                    assert(old_BLC_block->start_marked_BLC<=old_BLC_block->end_same_BLC);
                                                                                  }
                                                                                  if (!new_BLC_block->is_stable())
                                                                                  {
                                                                                    assert(new_BLC_block->start_same_BLC<=new_BLC_block->start_marked_BLC);
                                                                                    assert(new_BLC_block->start_marked_BLC<=new_BLC_block->end_same_BLC);
                                                                                  }
                                                                                #endif
                                                                                assert(m_transitions[ti].transitions_per_block_to_constellation==
                                                                                                                                                old_BLC_block);
                                                                                //assert(new block==m_states[m_aut.get_transitions()[ti].from()].block);
                                                                                assert(ti==*old_position);
      if (old_BLC_block->is_stable() ||
          old_position<old_BLC_block->start_marked_BLC)
      {                                                                         assert(old_BLC_block->start_same_BLC <= old_position);
//std::cerr << "Moving unmarked " << m_transitions[*old_position].debug_id(*this);
        if (new_BLC_block->is_stable())
        {
                                                                                #ifndef NDEBUG
                                                                                  if (!old_BLC_block->is_stable())
                                                                                  {
                                                                                    // different stability status: check that ti is inert
                                                                                    const transition& t=m_aut.get_transitions()[ti];
                                                                                    assert(is_inert_during_init(t) &&
                                                                                           (no_of_constellations<=1 /* i.e. initialisation */ ||
                                                                                            m_states[t.from()].block->c.onstellation==
                                                                                                                      m_states[t.to()].block->c.onstellation));
                                                                                  }
                                                                                #endif
          if (old_position!=old_BLC_block->start_same_BLC)
          {
            std::swap(*old_position, *old_BLC_block->start_same_BLC);
            m_transitions[*old_position].ref_outgoing_transitions->
                                              ref.BLC_transitions=old_position;
            m_transitions[*old_BLC_block->start_same_BLC].
                      ref_outgoing_transitions->ref.BLC_transitions=
                                                 old_BLC_block->start_same_BLC;
          }
        }
        else
        {                                                                       assert(new_BLC_block->start_marked_BLC<=old_BLC_block->start_same_BLC);
                                                                                #ifndef NDEBUG
                                                                                  if (old_BLC_block->is_stable())
                                                                                  {
                                                                                    // different stability status: check that ti is inert
                                                                                    const transition& t=m_aut.get_transitions()[ti];
                                                                                    assert(is_inert_during_init(t) &&
                                                                                           (/*initialisation ||*/
                                                                                            m_states[t.from()].block->c.onstellation==
                                                                                                                      m_states[t.to()].block->c.onstellation));
                                                                                  }
                                                                                #endif
          swap_three_iterators_and_update_m_transitions(old_position,
             old_BLC_block->start_same_BLC, new_BLC_block->start_marked_BLC++);
        }
      }
      else
      {                                                                         assert(old_BLC_block->start_marked_BLC <= old_position);
//std::cerr << "Moving marked " << m_transitions[*old_position].debug_id(*this);
                                                                                assert(old_BLC_block->start_same_BLC <= old_BLC_block->start_marked_BLC);
                                                                                assert(!new_BLC_block->is_stable());
        swap_three_iterators_and_update_m_transitions(old_position,
             old_BLC_block->start_marked_BLC++, old_BLC_block->start_same_BLC);
      }
      m_transitions[ti].transitions_per_block_to_constellation=new_BLC_block;
      new_BLC_block->end_same_BLC=++old_BLC_block->start_same_BLC;
//std::cerr << " to new " << new_BLC_block->debug_id(*this) << '\n';
      return old_BLC_block->start_same_BLC==old_BLC_block->end_same_BLC;
    }

    /// \brief Update the BLC list of transition `ti`, which now starts in block `new_bi`
    /// \param old_bi             the former block where the source state of `ti` was in
    /// \param new_bi             the current block where the source state of `ti` moves to
    /// \param ti                 index of the transition whose source state moved to a new block
    /// \param old_constellation  target constellation of co-splitters
    /// \details If the transition was part of a stable BLC set, or is
    /// constellation-inert, the new BLC set where it goes to is also stable.
    /// If the transition is part of an unstable BLC set, the order of
    /// main/co-splitters is maintained.  This order states that a co-splitter
    /// (i.e. any BLC set with non-constellation-inert transitions whose target
    /// state is in `old_constellation`) immediately precedes its corresponding
    /// main splitter (i.e. a BLC set with non-constellation-inert transitions
    /// whose target state is in the newest constellation, with the same action
    /// labels as the co-splitter).
    ///
    /// To maintain the order, it may happen that the old BLC set (where `ti`
    /// comes from) needs to be kept even if it becomes empty; then it will be
    /// added to `m_BLC_indicators_to_be_deleted` for deletion after all
    /// transitions of `new_bi` have been handled.
    ///
    /// When splitting a co-splitter, it may occasionally happen that one part
    /// of the co-splitter has no corresponding main splitter.  That is: if
    /// some block B is split into `old_bi` and `new_bi`, then it may happen
    /// that for some OTHER main splitter S', all transitions start in one of
    /// the subblocks, but the transitions of the co-splitter corresponding to
    /// S' start in both subblocks.  Then the co-splitter part that loses its
    /// correspondence with S' should be changed to stable.  This can happen
    /// both ways: the co-splitter part may belong to `old_bi` or to `new_bi`.
    /// When a co-splitter is created that is susceptible to this condition,
    /// one of its transitions is added to `m_co_splitters_to_be_checked`.
    /// After having handled all transitions of `new_bi`, the caller will have
    /// to check all co-splitters in this set whether they still have a
    /// corresponding main splitter.
    void update_the_doubly_linked_list_LBC_new_block(
               block_type* const old_bi,
               block_type* const new_bi,
               const transition_index ti,
               constellation_type* old_constellation,
               constellation_type*const new_constellation
                 // used to maintain the order of BLC sets:
                 // main splitter BLC sets (target constellation == new constellation) follow immediately
                 // after co-splitter BLC sets (target constellation == old_constellation) in the BLC sets
               )
    {                                                                           assert(old_bi->block.to_constellation.check_linked_list());
//std::cerr << "update_the_doubly_linked_list_LBC_new_block(old_bi==" << old_bi << ", new_bi==" << new_bi << ", transition_index==" << ti << "==" << m_transitions[ti].debug_id_short(*this) << ", old_constellation==" << old_constellation->debug_id(*this) << ")\n";
      const transition& t=m_aut.get_transitions()[ti];                          assert(new_bi->block.to_constellation.check_linked_list());
                                                                                assert(m_states[t.from()].block==new_bi);
      linked_list<BLC_indicators>::iterator this_block_to_constellation=
                      m_transitions[ti].transitions_per_block_to_constellation;
//std::cerr << "This transition is originally in " << this_block_to_constellation->debug_id(*this, old_bi) << '\n';
                                                                                #ifndef NDEBUG
                                                                                  // Check whether this_block_to_constellation is in the corresponding list
                                                                                  for (linked_list<BLC_indicators>::const_iterator i=old_bi->block.to_constellation.begin();
                                                                                       i!=this_block_to_constellation; ++i)
                                                                                  {
                                                                                    assert(i!=old_bi->block.to_constellation.end());
                                                                                  }
                                                                                #endif
      const label_index a=label_or_divergence(t);
      constellation_type* const to_constln=
                                        m_states[t.to()].block->c.onstellation;
      linked_list<BLC_indicators>::iterator new_BLC_block;
      const bool t_is_inert=is_inert_during_init(t);
      if (t_is_inert && to_constln==new_bi->c.onstellation)
      {
          /* Before correcting the BLC lists, we already inserted an empty   */ assert(this_block_to_constellation==old_bi->block.to_constellation.begin());
          /* BLC_indicator into the list to take the constellation-inert     */
          /* transitions. */                                                    assert(!new_bi->block.to_constellation.empty());
          new_BLC_block=new_bi->block.to_constellation.begin();                 assert(this_block_to_constellation->start_same_BLC==new_BLC_block->end_same_BLC);
                                                                                #ifndef NDEBUG
//std::cerr << "This transition remains constellation-inert and is moved to " << new_BLC_block->debug_id(*this) << '\n';
                                                                                  if (new_BLC_block->start_same_BLC<new_BLC_block->end_same_BLC)
                                                                                  {
                                                                                    const transition& inert_t=m_aut.get_transitions()[*new_BLC_block->start_same_BLC];
                                                                                    assert(new_bi==m_states[inert_t.from()].block);
                                                                                    assert(a==label_or_divergence(inert_t));
                                                                                    assert(to_constln==m_states[inert_t.to()].block->c.onstellation);
                                                                                  }
                                                                                #endif
      }
      else
      {
        transition_index perhaps_new_BLC_block_transition;
        const transition* perhaps_new_BLC_t;
        if (this_block_to_constellation->start_same_BLC!=
                                                    m_BLC_transitions.data() &&
            (perhaps_new_BLC_block_transition=
                    *std::prev(this_block_to_constellation->start_same_BLC),
             perhaps_new_BLC_t=
                 &m_aut.get_transitions()[perhaps_new_BLC_block_transition],
             m_states[perhaps_new_BLC_t->from()].block==new_bi) &&
            a==label_or_divergence(*perhaps_new_BLC_t) &&
            to_constln==m_states
                               [perhaps_new_BLC_t->to()].block->c.onstellation)
        {
          // Found the entry where the transition should go to
          // Move the current transition to the new list.
          new_BLC_block=m_transitions[perhaps_new_BLC_block_transition].
                                        transitions_per_block_to_constellation;
                                                                                #ifndef NDEBUG
//std::cerr << "It will be moved to the existing " << new_BLC_block->debug_id(*this) << '\n';
                                                                                  if (this_block_to_constellation->is_stable()) { assert(new_BLC_block->is_stable()); }
                                                                                  else { assert(!new_BLC_block->is_stable()); }
                                                                                #endif
        }
        else
        {
          // Make a new entry in the list next_block_to_constellation;

          // We first calculate the position where the new BLC set should go to
          // in new_position.
          // Default position: at the beginning.
          linked_list<BLC_indicators>::iterator new_position=
                                          new_bi->block.to_constellation.end(); assert(!is_inert_during_init(t)||to_constln!=new_bi->c.onstellation);
#ifndef USE_FOUR_WAY_SPLIT
          BLC_list_iterator_or_null old_co_splitter_end=nullptr;
          if (this_block_to_constellation->is_stable())
          {
//std::cerr << "Transition is in a stable BLC set.\n";
            if (m_branching)
            {
              // We always inserted a new BLC set for the inert transitions,
              // so we can place the BLC set after that one.
              new_position=new_bi->block.to_constellation.begin();
//std::cerr << "    and is placed after " << new_position->debug_id(*this) << '\n';
            }
            // Because the BLC set is stable, one does not need to keep main
            // splitter and co-splitter together.
            // If the BLC set becomes empty, one can immediately delete it.
            old_constellation=null_constellation;
          }
          else
#endif
          {
            if (new_bi->block.to_constellation.empty())
            {                                                                   assert(!m_branching);
              /* This is the first transition that is moved. */                 assert(new_bi->block.to_constellation.end()==new_position);
//std::cerr << "This is the first transition that is moved.\n";
            }
            else
            {
              // default position: place it at the end of the list
              new_position=new_bi->block.to_constellation.before_end();         assert(new_bi->block.to_constellation.end()!=new_position);
            }
            if (null_constellation!=old_constellation)
            {
              if (t_is_inert &&
                  ((to_constln==new_constellation &&
                    new_bi->c.onstellation==old_constellation) ||
                      // < The transition goes from the old constellation to
                      // the splitter block and was constellation-inert before.
                      // It is in a main splitter without (unstable)
                      // co-splitter. We do not need to find the co-splitter.
                   (to_constln==old_constellation &&
                    new_bi->c.onstellation==new_constellation)))
                      // < The formerly constellation-inert transition goes
                      // from the new constellation to the old constellation,
                      // it is in a co-splitter without (unstable) main
                      // splitter, and this co-splitter was handled as the
                      // first splitting action.
              {
                old_constellation=null_constellation;
//std::cerr << (old_constellation==to_constln ? "This transition was constellation-inert earlier, so we do not need to find a main splitter.\n" : "This transition was constellation-inert earlier, so we do not need to find a co-splitter.\n");
              }
              else
              {                                                                 assert(old_constellation!=new_constellation);
                // The following comments are all formulated for the case that
                // this_block_to_constellation is a main splitter (except when
                // indicated explicitly).
                linked_list<BLC_indicators>::const_iterator old_co_splitter;
                constellation_type* co_to_constln;
                if ((old_constellation==to_constln &&
                     // i.e. `this_block_to_constellation` is a co-splitter
                     (old_co_splitter=old_bi->block.to_constellation.
                                             next(this_block_to_constellation),
                      co_to_constln=new_constellation,
//(std::cerr << "Transition is originally in a co-splitter; "),
                                                                      true)) ||
                    (new_constellation==to_constln &&
                     // i.e. `this_block_to_constellation` is a main splitter
                     (old_co_splitter=old_bi->block.to_constellation.
                                             prev(this_block_to_constellation),
                      co_to_constln=old_constellation,
//(std::cerr << "Transition is originally in a main splitter; "),
                                                                        true)))
                {
                  if (old_bi->block.to_constellation.end()!=old_co_splitter)
                  {
//std::cerr << (old_constellation == to_constln ? "Current old main splitter candidate: " : "Current old co-splitter candidate: ") << old_co_splitter->debug_id(*this, old_bi);
                    // If the co-splitter belonging to
                    // `this_block_to_constellation` exists, then it is
                    // `old_co_splitter` (but if there is no such co-splitter,
                    // `old_co_splitter` could be a different main splitter, a
                    // different co-splitter without main splitter, or a
                    // completely unrelated splitter).

                    // Try to find out whether there is already a corresponding
                    // co-splitter in `new_bi->block.to_constellation`
                    // This co-splitter would be just before `old_co_splitter`
                    // in `m_BLC_transitions`.
                    if (new_bi->block.to_constellation.end()!=new_position &&
                        m_BLC_transitions.data()<
                                               old_co_splitter->start_same_BLC)
                        // i.e. this is not the first transition -- neither the
                        // first to be moved to the new block nor the first in
                        // m_BLC_transitions
                    {
                      // Check the transition in the potential corresponding
                      // new co-splitter:
                      const transition_index perhaps_new_co_spl_transition=
                                   *std::prev(old_co_splitter->start_same_BLC);
                      const transition& perhaps_new_co_spl_t=
                        m_aut.get_transitions()[perhaps_new_co_spl_transition];
                      if(new_bi==m_states[perhaps_new_co_spl_t.from()].block &&
                         a==label_or_divergence(perhaps_new_co_spl_t) &&
                         co_to_constln==m_states
                             [perhaps_new_co_spl_t.to()].block->c.onstellation)
                      {
                        // `perhaps_new_co_spl_transition` is in the
                        // corresponding new co-splitter; place the new BLC set
                        // immediately after this co-splitter in the list
                        // `new_bi->block.to_constellation`.
                        new_position=m_transitions
                                      [perhaps_new_co_spl_transition].
                                        transitions_per_block_to_constellation;
                        if (old_constellation==to_constln)
                        {
                          // (`this_block_to_constellation` was a co-splitter:)
                          // `perhaps_new_co_spl_transition` is in the new main
                          // splitter; place the new BLC set immediately before
                          // this main splitter in the list
                          // `new_bi->block.to_constellation`.
                          new_position=new_bi->block.to_constellation.
                                                            prev(new_position);
//std::cerr << ". This is a real old main splitter.\n";
                        }
//else { std::cerr << ". This is a real old co-splitter.\n"; }
                                                                                #ifndef NDEBUG
                        /* The new co-splitter was found, and                */   if (old_co_splitter->start_same_BLC<old_co_splitter->end_same_BLC)
                        /* `old_co_splitter` must have been the old          */   {
                        /* co-splitter.                                      */     const transition& co_t=m_aut.get_transitions()
                                                                                                                            [*old_co_splitter->start_same_BLC];
                        /* Now the new main splitter is about to be created. */     assert(old_bi==m_states[co_t.from()].block ||
                        /* In this case it is ok to delete                   */            new_bi==m_states[co_t.from()].block);
                        /* `this_block_to_constellation` when it becomes     */     assert(a==label_or_divergence(co_t));
                        /* empty; therefore we set `old_constellation` in a  */     assert(co_to_constln==m_states[co_t.to()].block->c.onstellation);
                        /* way that it's going to delete it immediately:     */   }
                                                                                #endif
                        old_constellation=null_constellation;
                        // We should not use `old_constellation` for anything
                        // else after this point.
                      }
#ifdef USE_FOUR_WAY_SPLIT
//else  {  std::cerr << ". The transition just before it in m_BLC_transitions (" << m_transitions[perhaps_new_co_spl_transition].debug_id_short(*this) << ") does not belong to the new co-splitter.\n";  }
#else
                      else if (new_constellation==to_constln)
                      {
                        const transition* co_t;
                        if (old_co_splitter->start_same_BLC<
                                               old_co_splitter->end_same_BLC &&
                            (co_t=&m_aut.get_transitions()
                                          [*old_co_splitter->start_same_BLC],   assert(old_bi==m_states[co_t->from()].block ||
                                                                                       new_bi==m_states[co_t->from()].block),
                             a==label_or_divergence(*co_t)) &&
                            old_constellation==m_states
                                            [co_t->to()].block->c.onstellation)
                        {
//std::cerr << ". The transition just before it in m_BLC_transitions (" << m_transitions[perhaps_new_co_spl_transition].debug_id_short(*this) << ") does not belong to the new co-splitter.\n";
                          old_co_splitter_end=old_co_splitter->end_same_BLC;
                        }
                        else
                        {
//std::cerr << ". In fact, it is not a true candidate.\n";
                          // As there is no old co-splitter, we do not need to keep an empty old main splitter.
                          old_constellation=null_constellation;
                        }
                      }                                                         else
                                                                                {
                                                                                  assert(old_constellation==to_constln);
//std::cerr << ". The transition just before it in m_BLC_transitions (" << m_transitions[perhaps_new_co_spl_transition].debug_id_short(*this) << ") does not belong to the new main splitter.\n";
                                                                                }
#endif
                    }
#ifdef USE_FOUR_WAY_SPLIT
//else  {  std::cerr << ". This candidate is at the beginning of m_BLC_transitions, or it is the first transition. There is no new co-splitter yet.\n";  }
#else
                    else if (new_constellation==to_constln)
                    {
                      const transition* co_t;
                      if (old_co_splitter->start_same_BLC<
                                               old_co_splitter->end_same_BLC &&
                          (co_t=&m_aut.get_transitions()
                                          [*old_co_splitter->start_same_BLC],   assert(old_bi==m_states[co_t->from()].block ||
                                                                                       new_bi==m_states[co_t->from()].block),
                           a==label_or_divergence(*co_t)) &&
                          old_constellation==m_states
                                            [co_t->to()].block->c.onstellation)
                      {
//std::cerr << ". This candidate is at the beginning of m_BLC_transitions, or it is the first transition. There is no new co-splitter yet.\n";
                        old_co_splitter_end=old_co_splitter->end_same_BLC;
                      }
                      else
                      {
//std::cerr << ". In fact, it is a false candidate.\n";
                        // As there is no old co-splitter, we do not need to
                        // keep an empty old main splitter.
                        old_constellation=null_constellation;
                      }
                    }                                                           else
                                                                                {
                                                                                  assert(old_constellation==to_constln);
//std::cerr << ". This candidate is at the beginning of m_BLC_transitions, or it is the first transition. There is no new main splitter yet.\n";
                                                                                }
#endif
                  }
                  else
                  {
                    // this_block_to_constellation is a main splitter
                    // but it has no corresponding co-splitter.
                    // If it becomes empty, one can immediately delete it.
                    old_constellation=null_constellation;
//std::cerr << (old_constellation == to_constln ? "There is no candidate old main splitter.\n" : "There is no candidate old co-splitter.\n");
                  }
                }
                else
                {
                  // this_block_to_constellation is neither a main splitter nor
                  // a co-splitter.  If it becomes empty, one can immediately
                  // delete it.
                  old_constellation=null_constellation;
                }
              }
            }
#ifdef USE_FOUR_WAY_SPLIT
            else if (this_block_to_constellation->is_stable())
            {
              // default position during new bottom splits: at the beginning of
              // the list (but after the BLC set of inert transitions)
              new_position=m_branching ? new_bi->block.to_constellation.begin()
                                       : new_bi->block.to_constellation.end();
            }
#endif
          }                                                                     assert(!m_branching || new_bi->block.to_constellation.end()!=new_position);
          const BLC_list_iterator old_BLC_start=this_block_to_constellation->start_same_BLC;
          new_BLC_block=new_bi->block.to_constellation.emplace_after
                          (new_position, old_BLC_start, old_BLC_start,
                                     this_block_to_constellation->is_stable());
                                                                                #if !defined(NDEBUG) || defined(COUNT_WORK_BALANCE)
                                                                                  new_BLC_block->work_counter=this_block_to_constellation->work_counter;
                                                                                #endif
          ++no_of_non_constellation_inert_BLC_sets;
#ifndef USE_FOUR_WAY_SPLIT
          if (null_constellation!=old_constellation)
          {
            // This is a new main splitter without new co-splitter, or a new
            // co-splitter without new main splitter
            if (old_constellation==to_constln)
            {
              // this is a new co-splitter but we haven't yet found the new
              // main splitter.  Perhaps we will have to stabilize it later.
              m_co_splitters_to_be_checked.emplace_back
                                       (new_BLC_block->start_same_BLC, new_bi);
//std::cerr << "We will check the stability of the new and old co-splitters later.\n";
              /* We may also have to stabilize the old co-splitter later.    */ assert(m_BLC_transitions.data()<this_block_to_constellation->end_same_BLC);
              m_co_splitters_to_be_checked.emplace_back
                (std::prev(this_block_to_constellation->end_same_BLC), old_bi);
            }
            else
            {                                                                   assert(new_constellation==to_constln);
              /* this is a new main splitter but we haven't yet found the new*/ assert(nullptr!=old_co_splitter_end);
              /* co-splitter.  Perhaps we will have to stabilize the old     */ assert(m_BLC_transitions.data()<old_co_splitter_end);
              /* co-splitter later.                                          */ assert(old_co_splitter_end<=m_BLC_transitions.data_end());
              m_co_splitters_to_be_checked.emplace_back
                                      (std::prev(old_co_splitter_end), old_bi);
//std::cerr << "We will check the stability of the old co-splitter later.\n";
            }
          }
#endif
        }
      }
      const bool last_element_removed=
              swap_in_the_doubly_linked_list_LBC_in_blocks_new_block(ti,
                                   new_BLC_block, this_block_to_constellation);

      if (last_element_removed)
      {
//std::cerr << "This was the last element in the old BLC set, which ";
        if (null_constellation != old_constellation)
        {
          // Sometimes we could still remove this_block_to_constellation
          // immediately (namely if the new main splitter and the new
          // co-splitter already exist, or if the old co-splitter does not
          // exist at all).  A few such cases are handled above, but other
          // cases would require additional, possibly extensive, checks:
          // if (co_block_found) {
          //   copy more or less the code from above that decides
          //   whether this_block_to_constellation is a main splitter
          //   that has an old co-splitter but not a new co-splitter
          //   or vice versa.
          // }
//std::cerr << "will be deleted later.";
          m_BLC_indicators_to_be_deleted.push_back
                                                 (this_block_to_constellation);
        }
        else
        {
//std::cerr << "is deleted immediately.";
          // Remove this element.
          old_bi->block.to_constellation.erase(this_block_to_constellation);
        }
        if (!t_is_inert || to_constln!=new_bi->c.onstellation)
        {
          --no_of_non_constellation_inert_BLC_sets;
        }
//std::cerr << '\n';
      }                                                                         assert(old_bi->block.to_constellation.check_linked_list());
                                                                                #ifndef NDEBUG
                                                                                  assert(new_bi->block.to_constellation.check_linked_list());
                                                                                  check_transitions(no_of_constellations<=1, false, false);
                                                                                #endif
      return;
    }

#ifdef USE_FOUR_WAY_SPLIT
    /// \brief reset a range of state counters to `undefined`
    /// \details The function is prepared for a situation when we join the
    /// `block` and `counter` fields together into one `block_plus_counter`.
    /// That is why it checks that only counters of states in block `bi` are
    /// reset.
    void clear_state_counters(
            std::vector<state_in_block_pointer>::const_iterator begin,
            std::vector<state_in_block_pointer>::const_iterator const end,
            block_type* const block)
    {
      (void) block; // avoid unused parameter warning
      while (begin!=end)
      {                                                                         assert(block==begin->ref_state->block);
        begin->ref_state->counter=undefined;
        ++begin;
      }
    }

#else

    /// \brief Reset state counters to `undefined`
    /// \param restrict_to_R  If this is true, it is assumed that `m_U` was empty
    /// \details `simple_splitB()` and a few other functions set
    /// `m_states[...].counter` to a defined value; after finishing them, one
    /// needs to reset it to undefined without touching too many counters.
    /// This function resets all counters that have been changed back to
    /// `undefined`.  Changes have been recorded in `m_R` and sometimes in
    /// `m_U_counter_reset_vector`.
    void clear_state_counters(bool restrict_to_R=false)
    {
      for(const state_in_block_pointer& si: m_R)
      {                                                                         assert(Rmarked == si.ref_state->counter);
        // The work in this loop is accounted for by the same action that set
        // the counter to `Rmarked`
        si.ref_state->counter=undefined;
      }
      if (restrict_to_R)
      {                                                                         assert(m_U_counter_reset_vector.empty());
        return;
      }
      for(const state_in_block_pointer& si: m_U_counter_reset_vector)
      {                                                                         assert(undefined!=si.ref_state->counter || m_R.find(si));
        /* The work in this loop is accounted for by the action that set the */ assert(Rmarked!=si.ref_state->counter);
        // counter to some defined value (possibly to `Rmarked`, but then it
        // has already been changed back to `undefined` by the loop above).
        si.ref_state->counter=undefined;
      }
      clear(m_U_counter_reset_vector);
    }

    /// \brief Split block `B` into the part with a transition in `splitter` and the rest
    /// \param B                            index of the block that must be split
    /// \param splitter                     set of transitions used to split `B`
    /// \param first_unmarked_bottom_state  separates between bottom states of `B` with and without transition in `splitter`
    /// \param splitter_end_unmarked_BLC    indicates until where in `splitter` unmarked transitions still need to be checked
    /// \returns the block index of subblock R, the subblock with transitions in `splitter`
    /// \details The function finds the non-bottom states in block `B` that can
    /// inertly reach a state with a transition in splitter and splits `B` in R
    /// and `B`\R.  The complexity is conform to the smaller subblock, either R
    /// or `B`\R.  The BLC list and bottom states of the subblocks are *not*
    /// updated.
    ///
    /// The function assumes that the sources of *marked* transitions in
    /// splitter have already been handled: the bottom states are in
    /// [`B->start_bottom_states`, `first_unmarked_bottom_state`),
    /// while the non-bottom states are in `m_R`. A split is always needed.
    /// (Unmarked transitions in the splitter may need to be checked still, but
    /// they can only add non-bottom states to R.)
    ///
    /// Usually `splitter_end_unmarked_BLC==splitter->start_marked_BLC`;
    /// however, occasionally it is necessary to split using only a subset of
    /// splitter.
    /// `splitter_end_unmarked_BLC` can be used to indicate that this is the
    /// case; the function will only treat
    /// [`splitter->start_same_BLC`, `splitter_end_unmarked_BLC`) as unmarked
    /// transitions.
    template <bool initialisation=false,
                          class Iterator=linked_list<BLC_indicators>::iterator>
    block_type* simple_splitB(block_type* const B,
                  Iterator splitter,
                  state_in_block_pointer* const first_unmarked_bottom_state,
                  const BLC_list_iterator_or_null splitter_end_unmarked_BLC,
                  block_type*& ptr_to_new_block)
    {
//std::cerr << "simple_splitB" << (initialisation ? "<true>(" : "(") << B->debug_id(*this) << ",...)\n";
      typedef enum { initializing, state_checking, aborted,
                     aborted_after_initialisation,
                     incoming_inert_transition_checking,
                     outgoing_action_constellation_check } status_type;         assert(1 < number_of_states_in_block(*B));
      status_type U_status=state_checking;                                      assert(!B->contains_new_bottom_states);
      status_type R_status=initializing;                                        assert(m_U.empty());  assert(m_U_counter_reset_vector.empty());
      BLC_list_iterator M_it;
      if constexpr (initialisation)
      {
        M_it=splitter_end_unmarked_BLC;
      }
      else
      {                                                                         assert(splitter_end_unmarked_BLC<=splitter->end_same_BLC);
        M_it=splitter->start_same_BLC;                                          assert(splitter_end_unmarked_BLC==splitter->start_marked_BLC ||
                                                                                       splitter_end_unmarked_BLC==M_it);
                                                                                assert(m_BLC_transitions.data()<=M_it);  assert(!splitter->is_stable());
                                                                                assert(splitter->start_same_BLC<splitter->end_same_BLC);
                                                                                assert(splitter->end_same_BLC<=m_BLC_transitions.data_end());
                                                                                assert(m_states[m_aut.get_transitions()
                                                                                                                 [*splitter->start_same_BLC].from()].block==B);
      }
      std::vector<transition>::iterator current_U_incoming_transition_iterator;
      std::vector<transition>::iterator
                                    current_U_incoming_transition_iterator_end; assert(M_it <= splitter_end_unmarked_BLC);
      state_in_block_pointer current_U_outgoing_state;
      outgoing_transitions_it current_U_outgoing_transition_iterator;
      outgoing_transitions_it current_U_outgoing_transition_iterator_end;
      std::vector<transition>::iterator current_R_incoming_transition_iterator;
      state_in_block_pointer* current_R_incoming_bottom_state_iterator=
                                                        B->start_bottom_states; assert(current_R_incoming_bottom_state_iterator<=first_unmarked_bottom_state);
      std::vector<transition>::iterator
                                    current_R_incoming_transition_iterator_end; assert(current_R_incoming_bottom_state_iterator<first_unmarked_bottom_state ||
                                                                                       !m_R.empty() || M_it<splitter_end_unmarked_BLC);
      state_in_block_pointer* current_U_incoming_bottom_state_iterator=
                                                   first_unmarked_bottom_state; assert(current_U_incoming_bottom_state_iterator<B->sta.rt_non_bottom_states);
      const std::make_signed<state_index>::type max_R_nonbottom_size=
                    number_of_states_in_block(*B)/2-
                                    std::distance(B->start_bottom_states,
                                                  first_unmarked_bottom_state); // can underflow
      if (max_R_nonbottom_size <
                  static_cast<std::make_signed<state_index>::type>(m_R.size()))
      {                                                                         assert(number_of_states_in_block(*B)/2 < std::distance(
                                                                                      B->start_bottom_states, first_unmarked_bottom_state)+m_R.size());
        R_status = (M_it == splitter_end_unmarked_BLC)
                   ? aborted_after_initialisation : aborted;
      }
      else
      {                                                                         assert(number_of_states_in_block(*B)/2 >= std::distance(
                                                                                      B->start_bottom_states, first_unmarked_bottom_state)+m_R.size());
        if (B->sta.rt_non_bottom_states==B->end_states)
        {                                                                       assert(B->start_bottom_states<first_unmarked_bottom_state);
          /* There are no non-bottom states, hence we do not need to carry */   assert(std::distance(B->start_bottom_states,
          /* out the tau closure. */                                                                 first_unmarked_bottom_state) <=
          /* Also, there cannot be any new bottom states. */                          std::distance(first_unmarked_bottom_state, B->sta.rt_non_bottom_states));
                                                                                assert(m_R.empty()); // m_R.clear(); is superfluous
                                                                                assert(m_U.empty()); // m_U.clear(); is superfluous;
                                                                                assert(m_U_counter_reset_vector.empty()); // clear_state_counters(); is superfluous
          // split_block B into R and B\R.
          ptr_to_new_block=split_block_B_into_R_and_BminR<initialisation>
                 (B, B->start_bottom_states, first_unmarked_bottom_state, m_R);
          return ptr_to_new_block;
        }
        else if (M_it==splitter_end_unmarked_BLC)
        {
          // There are no more transitions in the splitter whose source states
          // need to be added to R.
                                                                                //assert(B->start_bottom_states < first_unmarked_bottom_state || !m_R.empty());
          R_status=state_checking;
        }
      }
      const std::make_signed<state_index>::type max_U_nonbottom_size=
                  number_of_states_in_block(*B)/2-
                  std::distance(first_unmarked_bottom_state,
                                B->sta.rt_non_bottom_states); // can underflow
      if (max_U_nonbottom_size<0)
      {                                                                         assert(static_cast<std::make_signed<state_index>::type>
                                                                                                (number_of_states_in_block(*B)/2) <
                                                                                std::distance(first_unmarked_bottom_state, B->sta.rt_non_bottom_states));
        U_status=aborted_after_initialisation;                                  assert(aborted != R_status); assert(aborted_after_initialisation != R_status);
      }
      else
      {                                                                         assert(static_cast<std::make_signed<state_index>::type>
                                                                                                (number_of_states_in_block(*B)/2) >=
                                                                                      std::distance(first_unmarked_bottom_state, B->sta.rt_non_bottom_states));
        if (B->sta.rt_non_bottom_states==B->end_states)
        {                                                                       assert(first_unmarked_bottom_state<B->sta.rt_non_bottom_states);
          /* There are no non-bottom states, hence we do not need to carry */   assert(std::distance(first_unmarked_bottom_state,
          /* out the tau closure.                                          */                        B->sta.rt_non_bottom_states) <=
          /* Also, there cannot be any new bottom states.                  */              std::distance(B->start_bottom_states, first_unmarked_bottom_state));
                                                                                assert(m_R.empty()); // m_R.clear(); is superfluous
                                                                                assert(m_U.empty()); // m_U.clear(); is superfluous;
                                                                                assert(m_U_counter_reset_vector.empty()); // clear_state_counters(); is superfluous
          // split_block B into U and B\U.
          ptr_to_new_block=split_block_B_into_R_and_BminR<initialisation>
                          (B, first_unmarked_bottom_state, B->end_states, m_U);
          return B;
        }
      }                                                                         assert(B->sta.rt_non_bottom_states<B->end_states);
      label_index a=null_action;
      constellation_type* C;
      if constexpr (!initialisation)
      {                                                                         assert(splitter->start_same_BLC<splitter->end_same_BLC);
        const transition& first_t=
                            m_aut.get_transitions()[*splitter->start_same_BLC];
        a=label_or_divergence(first_t);
        C=m_states[first_t.to()].block->c.onstellation;
      }                                                                         else{assert(state_checking==R_status||aborted_after_initialisation==R_status);}
                                                                                #ifndef NDEBUG
                                                                                  if constexpr (!initialisation)
                                                                                  {
                                                                                    if (initializing==R_status || aborted==R_status)
                                                                                    {
                                                                                      // For constellation-inert splitters (which only happens for the special
      /* Algorithm 3, line 3.2 left.                                         */       // split to separate new from old bottom states), one has to mark all
                                                                                      // transitions in the splitter.
      /* start coroutines. Each co-routine handles one state, and then gives */       assert(!m_aut.is_tau(a) || B->c.onstellation != C);
      /* control to the other co-routine. The coroutines can be found        */     }
      /* sequentially below surrounded by a while loop.                      */   }
                                                                                #endif
//std::cerr << "simple_splitB() before while\n";
      while (true)
      {                                                                         assert(U_status!=aborted_after_initialisation ||
                                                                                       (R_status!=aborted && R_status!=aborted_after_initialisation));
                                                                                #ifndef NDEBUG
                                                                                  for(state_in_block_pointer si=state_in_block_pointer(m_states.begin()); si.ref_state<m_states.end(); ++si.ref_state)
                                                                                  {
                                                                                    if (si.ref_state->block!=B || 0==si.ref_state->no_of_outgoing_block_inert_transitions)
                                                                                    {
                                                                                      assert((initialisation &&
                                                                                              0!=si.ref_state->no_of_outgoing_block_inert_transitions) ||
                                                                                             undefined==si.ref_state->counter);
                                                                                      assert(!m_R.find(si));
                                                                                      assert(!m_U.find(si));
                                                                                      assert(std::find(m_U_counter_reset_vector.begin(), m_U_counter_reset_vector.end(), si) == m_U_counter_reset_vector.end());
                                                                                    }
                                                                                    else
                                                                                    {
                                                                                      switch(si.ref_state->counter)
                                                                                      {
                                                                                      case undefined:  assert(!m_U.find(si)); assert(!m_R.find(si));
                                                                                                       assert(std::find(m_U_counter_reset_vector.begin(), m_U_counter_reset_vector.end(), si) == m_U_counter_reset_vector.end());
                                                                                                       break;
                                                                                      case Rmarked:    assert( m_R.find(si)); assert(!m_U.find(si));
                                                                                                       // It can happen that the state has a tau-transition
                                                                                                       // to a U-state, then it will be in the
                                                                                                       // m_U_counter_reset_vector.
                                                                                                       break;
                                                                                      case Umarked:    assert(!m_R.find(si)); // It can happen that the state
                                                                                                                              // is in U or is not in U
                                                                                                       assert(std::find(m_U_counter_reset_vector.begin(), m_U_counter_reset_vector.end(), si) != m_U_counter_reset_vector.end());
                                                                                                       break;
                                                                                      default:         assert(!m_R.find(si)); assert(!m_U.find(si));
                                                                                                       assert(std::find(m_U_counter_reset_vector.begin(), m_U_counter_reset_vector.end(), si) != m_U_counter_reset_vector.end());
                                                                                                       break;
                                                                                      }
                                                                                    }
        /* The code for the right co-routine.                                */   }
                                                                                #endif
        if (incoming_inert_transition_checking==R_status) // 18178 times (large 1394-fin.lts example: 372431 times)
        {                                                                       assert(current_R_incoming_transition_iterator <
                                                                                                                    current_R_incoming_transition_iterator_end);
                                                                                mCRL2complexity(&m_transitions[std::distance(m_aut.get_transitions().begin(), current_R_incoming_transition_iterator)],
                                                                                              add_work(check_complexity::simple_splitB_R__handle_transition_to_R_state, 1), *this);
                                                                                assert(m_aut.is_tau(m_aut_apply_hidden_label_map(current_R_incoming_transition_iterator->label())));
                                                                                assert(m_states[current_R_incoming_transition_iterator->to()].block==B);
          const transition& tr=*current_R_incoming_transition_iterator;
          if (m_states[tr.from()].block==B &&
              !(m_preserve_divergence && tr.from() == tr.to()))
          {
            if (m_states[tr.from()].counter!=Rmarked)
            {                                                                   assert(!m_R.find(m_states.begin() + tr.from()));
//std::cerr << "R_todo2 insert: " << tr.from() << "\n";
              m_R.add_todo(m_states.begin() + tr.from());
              m_states[tr.from()].counter=Rmarked;

              // Algorithm 3, line 3.10 and line 3.11, right.
              if (m_R.size()>static_cast<state_index>(max_R_nonbottom_size))
              {                                                                 assert(aborted_after_initialisation!=U_status);
                R_status=aborted_after_initialisation;
                goto R_handled_and_is_not_state_checking;
              }
            }                                                                   else  {  assert(m_R.find(m_states.begin() + tr.from()));  }
                                                                                assert(!m_U.find(m_states.begin() + tr.from()));
          }
          ++current_R_incoming_transition_iterator;
          if (current_R_incoming_transition_iterator!=
                                  current_R_incoming_transition_iterator_end &&
              m_aut.is_tau(m_aut_apply_hidden_label_map
                            (current_R_incoming_transition_iterator->label())))
          {
            goto R_handled_and_is_not_state_checking;
          }
          R_status=state_checking;
        }
        else if (state_checking==R_status) // 18014 times (large 1394-fin.lts example: 331708 times)
        {
          const state_in_block_pointer s=
                  (current_R_incoming_bottom_state_iterator<
                                                    first_unmarked_bottom_state
                                  ? *current_R_incoming_bottom_state_iterator++
                                  : m_R.move_from_todo());
                                                                                mCRL2complexity(s.ref_state, add_work(check_complexity::simple_splitB_R__find_predecessors, 1), *this);
//std::cerr << "R insert: " << s.ref_state->debug_id(*this) << "\n";
                                                                                assert(s.ref_state->block==B);
          if (std::next(s.ref_state)==m_states.end())
          {
            current_R_incoming_transition_iterator_end=
                                                 m_aut.get_transitions().end();
          }
          else
          {
            current_R_incoming_transition_iterator_end=
                            std::next(s.ref_state)->start_incoming_transitions;
          }
          current_R_incoming_transition_iterator=
                                       s.ref_state->start_incoming_transitions;
          if (current_R_incoming_transition_iterator!=
                                  current_R_incoming_transition_iterator_end &&
              m_aut.is_tau(m_aut_apply_hidden_label_map
                            (current_R_incoming_transition_iterator->label())))
          {
            R_status=incoming_inert_transition_checking;
            goto R_handled_and_is_not_state_checking;
          }
        }
        else if (initializing!=R_status)
        {
                                                                                assert(aborted_after_initialisation==R_status || // 10280 times (large 1394-fin.lts example: 550 times)
                                                                                       aborted==R_status); // 663 times (large 1394-fin.lts example: 584 times)
          goto R_handled_and_is_not_state_checking;
        }
        else // initializing==R_status: 2742 times (large 1394-fin.lts example: 1200 times)
        {                                                                       assert(M_it<splitter_end_unmarked_BLC);
            // Algorithm 3, line 3.3, right.
          const state_in_block_pointer
                    si(m_states.begin()+m_aut.get_transitions()[*M_it].from()); mCRL2complexity(&m_transitions[*M_it], add_work(check_complexity::
                                                                                                   simple_splitB_R__handle_transition_from_R_state, 1), *this);
                                                                                assert(si.ref_state->block==B);
                                                                                assert(!is_inert_during_init(m_aut.get_transitions()[*M_it]) ||
                                                                                       B->c.onstellation!=
                                                                                          m_states[m_aut.get_transitions()[*M_it].to()].block->c.onstellation);
          ++M_it;
          if (0==si.ref_state->no_of_outgoing_block_inert_transitions)
          {                                                                     assert(B->start_bottom_states<=si.ref_state->ref_states_in_blocks);
            /* The state is a bottom state, it should be in R already */        assert(si.ref_state->ref_states_in_blocks<first_unmarked_bottom_state);
                                                                                assert(!m_R.find(si));
          }
          else if (si.ref_state->counter!=Rmarked)
          {                                                                     assert(B->sta.rt_non_bottom_states<=si.ref_state->ref_states_in_blocks);
            /* The state is a nonbottom state that is not yet in R */           assert(si.ref_state->ref_states_in_blocks<B->end_states);
            m_R.add_todo(si);
            si.ref_state->counter=Rmarked;
//std::cerr << "R_todo1 insert: " << si.ref_state->debug_id(*this) << "\n";
            if (m_R.size()>static_cast<state_index>(max_R_nonbottom_size))
            {                                                                   assert(aborted_after_initialisation!=U_status);
              R_status=aborted;
              goto R_handled_and_is_not_state_checking;
            }
          }                                                                     else  {  assert(m_R.find(si));  }
                                                                                assert(!m_U.find(si));
          if (M_it!=splitter_end_unmarked_BLC)
          {
            goto R_handled_and_is_not_state_checking;
          }                                                                     assert(B->sta.rt_non_bottom_states<B->end_states);
          R_status=state_checking;
        }                                                                       assert(state_checking==R_status);
        if (current_R_incoming_bottom_state_iterator==
                            first_unmarked_bottom_state && m_R.todo_is_empty())
        {                                                                       assert(0 < std::distance(B->start_bottom_states,
                                                                                                                    first_unmarked_bottom_state) + m_R.size());
//std::cerr << "R empty: " << "\n";
          /* split_block B into R and B\R. */                                   assert(std::distance(B->start_bottom_states, first_unmarked_bottom_state) +
                                                                                                                m_R.size() <= number_of_states_in_block(*B)/2);
          ptr_to_new_block=split_block_B_into_R_and_BminR
             <initialisation>(B, B->start_bottom_states,
                                    first_unmarked_bottom_state, m_R);
          clear_state_counters();
          m_R.clear();
          m_U.clear();
          return ptr_to_new_block;
        }
    R_handled_and_is_not_state_checking:                                        assert(state_checking!=R_status ||
                                                                                       current_R_incoming_bottom_state_iterator!=first_unmarked_bottom_state ||
                                                                                       !m_R.todo_is_empty());
                                                                                #ifndef NDEBUG
                                                                                  for (state_in_block_pointer si=state_in_block_pointer(m_states.begin());
                                                                                                                   si.ref_state<m_states.end(); ++si.ref_state)
                                                                                  {
                                                                                    if (si.ref_state->block!=B ||
                                                                                        0==si.ref_state->no_of_outgoing_block_inert_transitions)
                                                                                    {
                                                                                      assert((initialisation &&
                                                                                              0!=si.ref_state->no_of_outgoing_block_inert_transitions) ||
                                                                                             undefined==si.ref_state->counter);
                                                                                      assert(!m_R.find(si));
                                                                                      assert(!m_U.find(si));
                                                                                      assert(std::find(m_U_counter_reset_vector.begin(),
                                                                                          m_U_counter_reset_vector.end(), si)==m_U_counter_reset_vector.end());
                                                                                    }
                                                                                    else
                                                                                    {
                                                                                      switch(si.ref_state->counter)
                                                                                      {
                                                                                      case undefined:  assert(!m_U.find(si)); assert(!m_R.find(si));
                                                                                                       assert(std::find(m_U_counter_reset_vector.begin(), m_U_counter_reset_vector.end(), si) == m_U_counter_reset_vector.end());
                                                                                                       break;
                                                                                      case Rmarked:    assert( m_R.find(si)); assert(!m_U.find(si));
                                                                                                       // It can happen that the state has a tau-transition
                                                                                                       // to a U-state, then it will be in the
                                                                                                       // m_U_counter_reset_vector.
                                                                                                       break;
                                                                                      case Umarked:    assert(!m_R.find(si)); // It can happen that the state
                                                                                                                              // is in U or is not in U
                                                                                                       assert(std::find(m_U_counter_reset_vector.begin(), m_U_counter_reset_vector.end(), si) != m_U_counter_reset_vector.end());
                                                                                                       break;
                                                                                      default:         assert(!m_R.find(si)); assert(!m_U.find(si));
                                                                                                       assert(std::find(m_U_counter_reset_vector.begin(), m_U_counter_reset_vector.end(), si) != m_U_counter_reset_vector.end());
                                                                                                       break;
                                                                                      }
                                                                                    }
                                                                                  }
                                                                                #endif
        // The code for the left co-routine.
        if (incoming_inert_transition_checking==U_status) // 20299 times (large 1394-fin.lts example: 360327 times)
        {                                                                       assert(current_U_incoming_transition_iterator<
                                                                                                                   current_U_incoming_transition_iterator_end);
//std::cerr << "U_incoming_inert_transition_checking\n";
            /* Algorithm 3, line 3.8, left.                                  */ assert(m_aut.is_tau(m_aut_apply_hidden_label_map
                                                                                                           (current_U_incoming_transition_iterator->label())));
            /* Check one incoming transition.                                */ mCRL2complexity(&m_transitions[std::distance(m_aut.get_transitions().begin(),
            /* Algorithm 3, line 3.12, left.                                 */                                       current_U_incoming_transition_iterator)],
                                                                                                add_work(check_complexity::
                                                                                                     simple_splitB_U__handle_transition_to_U_state, 1), *this);
          current_U_outgoing_state=state_in_block_pointer(m_states.begin()+
                               current_U_incoming_transition_iterator->from()); assert(m_states[current_U_incoming_transition_iterator->to()].block==B);
          current_U_incoming_transition_iterator++;
//std::cerr << "FROM " << std::distance(m_states.begin(), current_U_outgoing_state.ref_state) << "\n";
          if (current_U_outgoing_state.ref_state->block==B &&
              !(m_preserve_divergence &&
                std::prev(current_U_incoming_transition_iterator)->from()==
                std::prev(current_U_incoming_transition_iterator)->to()))
          {                                                                     assert(!m_U.find(current_U_outgoing_state));
            if (current_U_outgoing_state.ref_state->counter!=Rmarked)
            {
              if (current_U_outgoing_state.ref_state->counter==undefined)
              {
                  // Algorithm 3, line 3.13, left.
                  // Algorithm 3, line 3.15 and 3.18, left.
                current_U_outgoing_state.ref_state->counter=Umarked-1+
                            current_U_outgoing_state.ref_state->
                                        no_of_outgoing_block_inert_transitions;
                m_U_counter_reset_vector.push_back(current_U_outgoing_state);
              }
              else
              {                                                                 assert(std::find(m_U_counter_reset_vector.begin(),
                                                                                                 m_U_counter_reset_vector.end(),
                                                                                                 current_U_outgoing_state) != m_U_counter_reset_vector.end());
                /* Algorithm 3, line 3.18, left. */                             assert(current_U_outgoing_state.ref_state->counter>Umarked);
                current_U_outgoing_state.ref_state->counter--;
              }
//std::cerr << "COUNTER " << current_U_outgoing_state.ref_state->counter << "\n";
                // Algorithm 3, line 3.19, left.
              if (current_U_outgoing_state.ref_state->counter==Umarked)
              {
                if (initializing==R_status || aborted==R_status)
                {
                  // Start searching for an outgoing transition with action a
                  // to constellation C.
                  current_U_outgoing_transition_iterator=
                              current_U_outgoing_state.ref_state->
                                                    start_outgoing_transitions; assert(m_outgoing_transitions.begin()<=current_U_outgoing_transition_iterator);
                                                                                assert(current_U_outgoing_transition_iterator<m_outgoing_transitions.end());
                  current_U_outgoing_transition_iterator_end=
                      (std::next(current_U_outgoing_state.ref_state)>=
                                                                m_states.end()
                          ? m_outgoing_transitions.end()
                          : std::next(current_U_outgoing_state.ref_state)->
                                                   start_outgoing_transitions); assert(current_U_outgoing_transition_iterator<
                                                                                                                    current_U_outgoing_transition_iterator_end);
                                                                                assert(m_states.begin()+m_aut.get_transitions()[*current_U_outgoing_transition_iterator->ref.BLC_transitions].from()==current_U_outgoing_state.ref_state);
                  U_status=outgoing_action_constellation_check;
                  goto U_handled_and_is_not_state_checking;
                }
                    // The state can be added to U_todo immediately.
                                                                                #ifndef NDEBUG
                                                                                  if constexpr (!initialisation)
                                                                                  {
                                                                                    // check that the state has no transition in the splitter
                                                                                    const outgoing_transitions_it out_it_end =
                                                                                          std::next(current_U_outgoing_state.ref_state)>=m_states.end()
                                                                                                ? m_outgoing_transitions.end()
                                                                                                : std::next(current_U_outgoing_state.ref_state)->
                                                                                                                                    start_outgoing_transitions;
                                                                                    for (outgoing_transitions_it out_it=current_U_outgoing_state.ref_state->
                                                                                                                                    start_outgoing_transitions;
                                                                                             out_it<out_it_end; ++out_it)
                                                                                    {
                                                                                      assert(m_outgoing_transitions.begin()<=out_it);
                                                                                      assert(out_it<m_outgoing_transitions.end());
                                                                                      assert(m_BLC_transitions.data()<=out_it->ref.BLC_transitions);
                                                                                      assert(out_it->ref.BLC_transitions<m_BLC_transitions.data_end());
                                                                                      /* assert(0<=*out_it->ref.BLC_transitions); Always true */
                                                                                      assert(*out_it->ref.BLC_transitions<m_aut.num_transitions());
                                                                                      assert(m_transitions[*out_it->ref.BLC_transitions].ref_outgoing_transitions==out_it);
                                                                                      const transition& t=m_aut.get_transitions()[*out_it->ref.BLC_transitions];
                                                                                      assert(m_states.begin()+t.from()==current_U_outgoing_state.ref_state);
                                                                                      if (a==label_or_divergence(t) &&
                                                                                          C==m_states[t.to()].block->c.onstellation)
                                                                                      {
                                                                                            // The transition is in the splitter, so it must be in the part of the splitter that is disregarded.
//std::cerr << "State " << std::distance(m_states.begin(), current_U_outgoing_state.ref_state) << " has a transition in the splitter, namely " << m_transitions[*out_it->ref.BLC_transitions].debug_id_short(*this) << '\n';
                                                                                        assert(out_it->ref.BLC_transitions>=splitter_end_unmarked_BLC);
                                                                                        assert(splitter->start_same_BLC<=out_it->ref.BLC_transitions);
                                                                                        assert(out_it->ref.BLC_transitions<splitter->end_same_BLC);
                                                                                      }
                                                                                    }
                                                                                  }
                                                                                #endif
                m_U.add_todo(current_U_outgoing_state);
                    // Algorithm 3, line 3.10 and line 3.11 left.
                if (m_U.size()>static_cast<state_index>(max_U_nonbottom_size))
                {                                                               assert(aborted!=R_status); assert(aborted_after_initialisation!=R_status);
                  U_status=aborted_after_initialisation;
                  goto U_handled_and_is_not_state_checking;
                }
              }
            }                                                                   else  {  assert(m_R.find(current_U_outgoing_state));  }
          }
          if (current_U_incoming_transition_iterator!=
                                  current_U_incoming_transition_iterator_end &&
                m_aut.is_tau(m_aut_apply_hidden_label_map(
                             current_U_incoming_transition_iterator->label())))
          {                                                                     assert(incoming_inert_transition_checking==U_status);
            goto U_handled_and_is_not_state_checking;
          }
          U_status=state_checking;
        }
        else if (state_checking==U_status) // 18920 times (large 1394-fin.lts example: 340893 times)
        {
//std::cerr << "U_state_checking\n";

            // Algorithm 3, line 3.23 and line 3.24, left.
          const state_in_block_pointer s=
                          (current_U_incoming_bottom_state_iterator<
                                                    B->sta.rt_non_bottom_states
                               ? *current_U_incoming_bottom_state_iterator++
                               : m_U.move_from_todo());                         assert(!m_R.find(s));
                                                                                mCRL2complexity(s.ref_state, add_work(check_complexity::simple_splitB_U__find_predecessors, 1), *this);
//std::cerr << "U insert/ U_todo_remove: " << s.ref_state->debug_id(*this) << "\n";
          current_U_incoming_transition_iterator=
                                       s.ref_state->start_incoming_transitions;
          current_U_incoming_transition_iterator_end=
                        (std::next(s.ref_state)>=m_states.end()
                         ? m_aut.get_transitions().end()
                         : std::next(s.ref_state)->start_incoming_transitions);
          if (current_U_incoming_transition_iterator!=
                                  current_U_incoming_transition_iterator_end &&
              m_aut.is_tau(m_aut_apply_hidden_label_map(
                             current_U_incoming_transition_iterator->label())))
          {
            U_status=incoming_inert_transition_checking;
            goto U_handled_and_is_not_state_checking;
          }
        }
        else if (initialisation ||
                 aborted_after_initialisation==U_status) // 6284 times (large 1394-fin.lts example: 2500 times)
        {                                                                       assert(aborted_after_initialisation==U_status);
          goto U_handled_and_is_not_state_checking;
        }
        else if constexpr (!initialisation) // the condition holds always
        {                                                                       assert(outgoing_action_constellation_check==U_status); // 911 times (large 1394-fin.lts example: 912 times)
//std::cerr << "U_outgoing_action_constellation_check\n";
                                                                                assert(current_U_outgoing_transition_iterator!=
                                                                                                                   current_U_outgoing_transition_iterator_end);
                                                                                  // will only be used if the transitions are not constellation-inert:
                                                                                assert(!m_aut.is_tau(a) || B->c.onstellation!=C);
                                                                                assert(splitter_end_unmarked_BLC==splitter->start_marked_BLC);
                                                                                mCRL2complexity((&m_transitions[*current_U_outgoing_transition_iterator->ref.BLC_transitions]), add_work(check_complexity::simple_splitB_U__handle_transition_from_potential_U_state, 1), *this);
                                                                                #ifndef NDEBUG
                                                                                  // This is one step in the coroutine, so we should assign the work to exactly one transition.
                                                                                  // But to make sure, we also mark the other transitions that we skipped in the optimisation.
                                                                                  for (outgoing_transitions_it out_it=current_U_outgoing_transition_iterator; out_it<current_U_outgoing_transition_iterator->start_same_saC; )
                                                                                  {
                                                                                    ++out_it;
                                                                                    mCRL2complexity(&m_transitions[*out_it->ref.BLC_transitions], add_work_notemporary(check_complexity::simple_splitB_U__handle_transition_from_potential_U_state, 1), *this);
                                                                                  }
                                                                                #endif
          const transition& t_local=m_aut.get_transitions()
                [*current_U_outgoing_transition_iterator->ref.BLC_transitions];
          current_U_outgoing_transition_iterator= // This is an optimisation.
                        current_U_outgoing_transition_iterator->start_same_saC;
          ++current_U_outgoing_transition_iterator;                             assert(m_states.begin()+t_local.from()==current_U_outgoing_state.ref_state);
                                                                                assert(m_branching);
          if (m_states[t_local.to()].block->c.onstellation==C &&
              label_or_divergence(t_local) == a)
          {
                // This state must be blocked.
          }
          else if (current_U_outgoing_transition_iterator==
                                    current_U_outgoing_transition_iterator_end)
          {                                                                     assert(!m_U.find(current_U_outgoing_state));
//std::cerr << "U_todo4 insert: " << std::distance(m_states.begin(), current_U_outgoing_state.ref_state) << "   " << m_U.size() << "    " << number_of_states_in_block(*B) << "\n";
            m_U.add_todo(current_U_outgoing_state);
              // Algorithm 3, line 3.10 and line 3.11 left.
            if (m_U.size()>static_cast<state_index>(max_U_nonbottom_size))
            {                                                                   assert(aborted!=R_status); assert(aborted_after_initialisation!=R_status);
              U_status=aborted_after_initialisation;
              goto U_handled_and_is_not_state_checking;
            }
          }
          else
          {
            goto U_handled_and_is_not_state_checking;
          }

          if (current_U_incoming_transition_iterator!=
                                  current_U_incoming_transition_iterator_end &&
              m_aut.is_tau(m_aut_apply_hidden_label_map(
                             current_U_incoming_transition_iterator->label())))
          {
            U_status = incoming_inert_transition_checking;
            goto U_handled_and_is_not_state_checking;
          }
          U_status=state_checking;
        }                                                                       else  {  assert(0);  }
                                                                                assert(state_checking==U_status);
        if (current_U_incoming_bottom_state_iterator==
                            B->sta.rt_non_bottom_states && m_U.todo_is_empty())
        {
          // split_block B into U and B\U.
//std::cerr << "U_todo empty:\n";
                                                                                assert(0 < std::distance(first_unmarked_bottom_state,
                                                                                                                    B->sta.rt_non_bottom_states) + m_U.size());
                                                                                assert(std::distance(first_unmarked_bottom_state, B->sta.rt_non_bottom_states)+
                                                                                                                 m_U.size() <= number_of_states_in_block(*B)/2);
          ptr_to_new_block=split_block_B_into_R_and_BminR<initialisation>
            (B, first_unmarked_bottom_state, B->sta.rt_non_bottom_states, m_U);
          clear_state_counters();
          m_R.clear();
          m_U.clear();
          return B;
        }
    U_handled_and_is_not_state_checking:                                        assert(state_checking!=U_status ||
                                                                                       current_U_incoming_bottom_state_iterator!=B->sta.rt_non_bottom_states ||
                                                                                       !m_U.todo_is_empty());
      }                                                                         assert(0);
    }

    // Mark all states that are sources of the marked transitions in splitter
    // and move them to the beginning.
    // (Marking of bottom states is done by moving them to the beginning of the
    // bottom states. Marking of non-bottom states is done by adding them to m_R
    // and setting m_states[*].counter = Rmarked.)
    // Also make the splitter stable and move it to the beginning of the respective
    // list of BLC sets.
    // If all bottom states are marked, reset markers and m_R.
    // Otherwise, if not all
    // bottom states are touched, leave the marked non-bottom states in m_R and leave
    // the markers in m_states[*].counter in place.
    // The marked bottom states are moved to the front in m_states_in_blocks and
    // the return value indicates the position (in m_states_in_blocks)
    // of the first non-marked bottom state.
    [[nodiscard]]
    state_in_block_pointer*
    not_all_bottom_states_are_touched(
            block_type* const bi,
            linked_list<BLC_indicators>::iterator splitter
                                                                                #ifndef NDEBUG
                                                                                  , const BLC_list_const_iterator splitter_end_unmarked_BLC
                                                                                    // = splitter->start_marked_BLC -- this default argument is not allowed
                                                                                #endif
            )
    {                                                                           assert(!splitter->is_stable());
//std::cerr << "not_all_bottom_states_are_touched(" << bi << ',' << splitter->debug_id(*this) << ")\n";
                                                                                assert(bi==
                                                                                    m_states[m_aut.get_transitions()[*splitter->start_same_BLC].from()].block);
                                                                                assert(splitter_end_unmarked_BLC<=splitter->start_marked_BLC);
      block_type& B=*bi;                                                        assert(m_R.empty());  assert(m_U.empty());
                                                                                assert(1 < number_of_states_in_block(B));
                                                                                // If the above assertion is false, one can just: return B.end_states;
      state_in_block_pointer*first_unmarked_bottom_state=B.start_bottom_states; assert(!B.contains_new_bottom_states);
      // Now go through the marked transitions in detail:
      BLC_list_iterator marked_t_it = splitter->start_marked_BLC;
      for(; marked_t_it<splitter->end_same_BLC; ++marked_t_it)
      {
        const transition& t = m_aut.get_transitions()[*marked_t_it];            // mCRL2complexity(&m_transitions[*marked_t_it], add_work(...), *this);
                                                                                    // not needed as this work can be attributed to marking of the transition
        const state_in_block_pointer s(m_states.begin()+t.from());
        state_in_block_pointer* const pos_s=s.ref_state->ref_states_in_blocks;  assert(B.start_bottom_states<=pos_s);  assert(pos_s<B.end_states);
        if (first_unmarked_bottom_state<=pos_s)
        {
          if (0==s.ref_state->no_of_outgoing_block_inert_transitions)
          {                                                                     assert(pos_s<B.sta.rt_non_bottom_states);
            swap_states_in_states_in_block(first_unmarked_bottom_state, pos_s); assert(undefined==s.ref_state->counter);
            first_unmarked_bottom_state++;
          }
          else if (Rmarked!=s.ref_state->counter)
          {                                                                     assert(B.sta.rt_non_bottom_states<=pos_s);
            m_R.add_todo(s);                                                    assert(undefined==s.ref_state->counter);
            s.ref_state->counter=Rmarked;
          }
        }
      }
                                                                                #ifndef NDEBUG
                                                                                  // ensure that the unmarked transitions do not add any bottom states.
                                                                                  assert(splitter->start_same_BLC <= splitter_end_unmarked_BLC);
                                                                                  for(BLC_list_const_iterator i = splitter->start_same_BLC; i < splitter_end_unmarked_BLC; ++i)
                                                                                  {
                                                                                    assert(splitter_end_unmarked_BLC == splitter->start_marked_BLC);
                                                                                    const transition& t = m_aut.get_transitions()[*i];
                                                                                    const state_in_block_pointer s(m_states.begin()+t.from());
                                                                                    assert(s.ref_state->block == bi);
                                                                                    const state_in_block_pointer*const pos_s=s.ref_state->ref_states_in_blocks;
                                                                                    assert(*pos_s==s);
                                                                                    assert(B.start_bottom_states <= pos_s);
                                                                                    assert(pos_s < B.end_states);
                                                                                    if (0==s.ref_state->no_of_outgoing_block_inert_transitions)
                                                                                    {
                                                                                      // State s is a bottom state. It should already have been marked.
                                                                                      assert(pos_s<first_unmarked_bottom_state);
                                                                                    }
                                                                                    else
                                                                                    {
                                                                                      assert(B.sta.rt_non_bottom_states <= pos_s);
                                                                                    }
                                                                                  }
                                                                                #endif
      if (first_unmarked_bottom_state==B.sta.rt_non_bottom_states)
      {
        // All bottom states are marked. No splitting is possible. Reset m_R,
        // m_states[s].counter for s in m_R.
        clear_state_counters(true);
        m_R.clear();
      }
      return first_unmarked_bottom_state;
    }

    /// \brief Make the transition `t` non-block-inert.
    /// \details The transition must go from one block to another but it cannot
    /// be constellation-inert yet.
    void make_transition_non_block_inert(const transition& t)
    {                                                                           assert(is_inert_during_init(t));
                                                                                assert(m_states[t.to()].block!=m_states[t.from()].block);
      m_states[t.from()].no_of_outgoing_block_inert_transitions--;              assert(no_of_constellations<=1 /* initialisation */ ||
                                                                                       m_states[t.to()].block->c.onstellation ==
                                                                                                                     m_states[t.from()].block->c.onstellation);
//std::cerr << "    " << m_transitions[std::distance<const transition *>(m_aut.get_transitions().data(), &t)].debug_id(*this) << " has become non-block-inert\n";
    }
#endif

    /// \brief Moves the former non-bottom state `si` to the bottom states
    /// \details The block of si is not yet inserted into the set of blocks
    /// with new bottom states.
    void change_non_bottom_state_to_bottom_state(
                                const fixed_vector<state_type_gj>::iterator si)
    {                                                                           assert(m_states.begin()<=si);
//std::cerr << "change_non_bottom_state_to_bottom_state(" << si->debug_id(*this) << ")\n";
      block_type* bi = si->block;                                               assert(si<m_states.end());
      swap_states_in_states_in_block(si->ref_states_in_blocks,
                                                 bi->sta.rt_non_bottom_states); assert(0 == si->no_of_outgoing_block_inert_transitions);
      bi->sta.rt_non_bottom_states++;                                           assert(!bi->contains_new_bottom_states);
      ++no_of_new_bottom_states;
    }

    /// \brief Makes splitter stable and moves it to the beginning of the list
    void make_stable_and_move_to_start_of_BLC(block_type* const from_block,
                          const linked_list<BLC_indicators>::iterator splitter)
    {                                                                           assert(from_block->block.to_constellation.end()!=splitter);
      splitter->make_stable();                                                  assert(splitter->start_same_BLC<splitter->end_same_BLC);
                                                                                #ifndef NDEBUG
                                                                                  const transition& t=m_aut.get_transitions()[*splitter->start_same_BLC];
                                                                                  assert(from_block==m_states[t.from()].block);
                                                                                #endif
      linked_list<BLC_indicators>& btc=from_block->block.to_constellation;      assert(!btc.empty());
      if (splitter!=btc.begin())
      {
        linked_list<BLC_indicators>::iterator move_splitter_after=btc.end();
        if (m_branching)
        {                                                                       // The following assertion may fail because we sometimes make a splitter
          const transition& perhaps_inert_t=                                    // stable before all BLC sets are split:
                         m_aut.get_transitions()[*btc.begin()->start_same_BLC]; // assert(m_states[perhaps_inert_t.from()].block==from_block);
          if (is_inert_during_init_if_branching(perhaps_inert_t) &&
              m_states[perhaps_inert_t.to()].block->c.onstellation==
                                                    from_block->c.onstellation)
          {
            move_splitter_after=btc.begin();
          }
        }
        btc.splice_to_after(move_splitter_after, btc, splitter);
      }
    }

#ifndef USE_FOUR_WAY_SPLIT
    /// \brief determines the target constellation of a BLC set
    /// \details needed because during initialisation, `splitter` may be of a
    /// different type without a field `start_same_BLC`.
    template<bool initialisation, class Iterator>
    inline constellation_type* target_constellation(Iterator splitter) const
    {
      if constexpr (initialisation)
      {                                                                         assert(0);
        return nullptr;
      }
      else
      {
        return m_states[m_aut.get_transitions()
                       [*splitter->start_same_BLC].to()].block->c.onstellation;
      }
    }

    /// \brief Split block `B` into states with a transition in `splitter` (and their tau-predecessors) and other states
    /// \param B                            block that is split
    /// \param splitter                     BLC set that contains the relevant transitions from `B`
    /// \param first_unmarked_bottom_state  bottom states [`B->start_bottom`, `first_unmarked_bottom_state`) have transitions in `splitter`
    /// \param splitter_end_unmarked_BLC    transitions [`splitter->start_same_BLC`, `splitter_end_unmarked_BLC`) still need to be considered
    /// \param old_constellation            if this is a main- or co-split, this is the target constellation of the co-split
    /// \param split_off_new_bottom_states  true if new bottom states need to be separated further
    /// \returns index of the subblock containing states with transitions in `splitter`
    /// \details This function assumes that block `B` has already been prepared
    /// by `not_all_bottom_states_are_touched()` (namely, bottom states are
    /// already separated and non-bottom states with a marked transition in
    /// `splitter` are in `m_R`), and that a split is necessary.  The split is
    /// executed in time O(min(|R|,|U|).
    ///
    /// The splitter is marked as stable and moved to a suitable position at or
    /// near the beginning of the list of BLC sets.
    /// (If `!split_off_new_bottom_states`, it was already the first BLC set.)
    /// This means that the order of main splitter/co-splitter is not
    /// maintained; the recommended course of actions for the caller is:
    /// first remember in which slice of `m_BLC_transitions` the co-splitter is
    /// stored, then call `splitB()`, then recover the co-splitter from this
    /// slice.  (The co-splitter part that starts in the U-subblock is actually
    /// already stable and just needs to be marked so; the part that starts in
    /// the R-subblock may require another call of splitB().)
    template <bool initialisation=false,
              class Iterator=linked_list<BLC_indicators>::iterator>
    block_type* splitB(block_type* const B,
                      Iterator splitter,
                      state_in_block_pointer* first_unmarked_bottom_state,
                      const BLC_list_iterator splitter_end_unmarked_BLC
                                    /* =splitter->start_marked_BLC -- but
                                       this default argument is not allowed */,
                      block_type*& bi,
                      constellation_type* const old_constellation,
                      constellation_type* const new_constellation,
                      const bool split_off_new_bottom_states=true)
    {
//std::cerr << (initialisation ? "splitB<true>(" : "splitB(") << B->debug_id(*this) << ", splitter==";
//if constexpr (!initialisation) { std::cerr << splitter->debug_id(*this); }
//else { std::cerr << splitter; }
//std::cerr << ", first_unmarked_bottom_state==" << first_unmarked_bottom_state->ref_state->debug_id(*this) << ", splitter_end_unmarked_BLC==";
//if constexpr (!initialisation) { std::cerr << (split_off_new_bottom_states && splitter_end_unmarked_BLC == splitter->start_marked_BLC ? "start_marked_BLC" : (splitter_end_unmarked_BLC == splitter->start_same_BLC ? "start_same_BLC" : "?")); }
//else { std::cerr << splitter_end_unmarked_BLC; }
//std::cerr << ", old_constellation==" << old_constellation->debug_id(*this) << ", split_off_new_bottom_states==" << split_off_new_bottom_states << ")\n";
//std::cerr << (m_branching ? "Marked bottom states:" : "Marked states:"); for (const state_in_block_pointer* it=B->start_bottom_states; it!=first_unmarked_bottom_state; ++it) { std::cerr << ' ' << std::distance(m_states.begin(), it->ref_state); }
//std::cerr << (m_branching ? "\nUnmarked bottom states:" : "\nUnmarked states:"); for (const state_in_block_pointer* it=first_unmarked_bottom_state; it!=B->sta.rt_non_bottom_states; ++it) { std::cerr << ' ' << std::distance(m_states.begin(), it->ref_state); } std::cerr << '\n';
//if (m_branching) { std::cerr << "Additionally, " << m_R.size() << " non-bottom states have been marked.\n"; }
                                                                                if constexpr (initialisation) {
                                                                                  assert(no_of_constellations==1);
                                                                                  assert(null_constellation==old_constellation);
                                                                                  assert(!split_off_new_bottom_states);
                                                                                } else {
                                                                                  assert(split_off_new_bottom_states ||
                                                                                         B->block.to_constellation.begin()==splitter);
                                                                                }
                                                                                if constexpr (!initialisation) {
                                                                                  assert(m_states[m_aut.get_transitions()[*splitter->start_same_BLC].from()].
                                                                                                                                            block==B);
                                                                                  assert(!splitter->is_stable());
                                                                                  assert(!split_off_new_bottom_states ||
                                                                                         splitter_end_unmarked_BLC==splitter->start_marked_BLC);
                                                                                }
      if (1 >= number_of_states_in_block(*B))
      {
        mCRL2log(log::debug) << "Trying to split up singleton block "<<B<<'\n';
        clear_state_counters();
        return null_block;
      }
      block_type* R_block=simple_splitB<initialisation, Iterator>(B, splitter,
                   first_unmarked_bottom_state, splitter_end_unmarked_BLC, bi); assert(bi == R_block || B == R_block);
//std::cerr << "Split block of size " << number_of_states_in_block(*B) + number_of_states_in_block(*bi) << " taking away " << number_of_states_in_block(*bi) << " states (namely"; for (auto it=bi->start_bottom_states; it<bi->end_states; ++it) { std::cerr << ' ' << std::distance(m_states.begin(), it->ref_state); } std::cerr << (bi == R_block ? ", the R-subblock)\n" : ", the U-subblock)\n");
                                                                                assert(number_of_states_in_block(*B) >= number_of_states_in_block(*bi));
                                                                                // Because we visit all states of block bi and almost all their incoming and
                                                                                // outgoing transitions, we subsume all this bookkeeping in a single block
                                                                                // counter:
                                                                                mCRL2complexity(bi, add_work(check_complexity::
      /* Update the BLC_list, and bottom states, and invariant on inert      */     splitB__update_BLC_of_smaller_subblock, check_complexity::log_n -
      /* transitions.                                                        */     check_complexity::ilog2(number_of_states_in_block(*bi))), *this);
      state_in_block_pointer* const
                     start_new_bottom_states=R_block->sta.rt_non_bottom_states;
      linked_list<BLC_indicators>::iterator R_to_U_tau_splitter =
                                         R_block->block.to_constellation.end(); assert(bi->block.to_constellation.empty());
      bool skip_transitions_in_splitter=false;                                  assert(m_BLC_indicators_to_be_deleted.empty());
      bool make_splitter_stable_early=false;

      if (initialisation || !split_off_new_bottom_states ||
          null_constellation==old_constellation ||
          target_constellation<initialisation, Iterator>(splitter)==
                                                             old_constellation)
      {
        // splitter is not a main splitter. So it can be stabilized already
        // (as we do not need to maintain the order of main/co-splitter).
        make_splitter_stable_early=true;
//std::cerr << "Making splitter ";
        if constexpr (!initialisation)
        {
//std::cerr << splitter->debug_id(*this) << " stable early.\n";
          // However, we may need to move the splitter to its new constellation
          // at the same time.
          if (bi==R_block &&
              (initialisation || split_off_new_bottom_states ||
               splitter_end_unmarked_BLC==splitter->start_marked_BLC))
          {
            splitter->make_stable();
            // move splitter as a whole from its current list to the new list
            // where it will be part of:
            // (This only works if the whole splitter BLC set has moved.)
            R_block->block.to_constellation.splice(
                                         R_block->block.to_constellation.end(),
                                         B->block.to_constellation, splitter);
            skip_transitions_in_splitter=true;
//std::cerr << "    and moving it to the BLC list of " << R_block << '\n';
          }
          else if (!initialisation && split_off_new_bottom_states &&
                   null_constellation==old_constellation)
          {
            // Only during new bottom state splits, we really need to move the
            // splitter to the beginning of the list of splitters.
            make_stable_and_move_to_start_of_BLC(R_block, splitter);
          }
          else
          {
            // During initialisation, we do not bother to move the splitter to
            // the beginning of the BLC list, as every splitter will become
            // stable.
            splitter->make_stable();
          }
        }                                                                       else
                                                                                {
                                                                                  assert(nullptr==splitter);
//std::cerr << "(nullptr) stable early.\n";
                                                                                }
      }
      else if constexpr (!initialisation) // actually the condition always
                                          // holds, but we need "constexpr"
      {                                                                         assert(!splitter->is_stable());
        // unmark all states in the splitter (needed to avoid that parts of the
        // splitter will retain marked states)
        splitter->start_marked_BLC=splitter->end_same_BLC;
        if (bi==R_block)
        {
          // insert a dummy old main splitter to help with placing the
          // corresponding co-splitter correctly:
//std::cerr << "Inserting a dummy old main splitter at m_BLC_transitions[" << std::distance(m_BLC_transitions.data(), splitter->end_same_BLC) << "]\n";
          m_BLC_indicators_to_be_deleted.push_back(
                B->block.to_constellation.emplace_after(splitter,
                         splitter->end_same_BLC, splitter->end_same_BLC, false)
          );                                                                    // ...->work_counter=... -- not needed, as the set will be deleted soon again.
          // ++no_of_non_constellation_inert_BLC_sets; -- not needed, as BLC sets in m_BLC_indicators_to_be_deleted are not counted
          // move splitter as a whole from its current list to the new list
          // where it will be part of:
          // (This only works if the whole splitter BLC set has moved, i.e.
          // only if split_off_new_bottom_states == true.)
          R_block->block.to_constellation.splice(
                                         R_block->block.to_constellation.end(),
                                         B->block.to_constellation, splitter);
          skip_transitions_in_splitter=true;
        }
      }                                                                         else  {  assert(0);  }
                                                                                assert(!initialisation || make_splitter_stable_early);
//std::cerr << "initialisation==" << initialisation
//<< ", m_branching==" << m_branching << ", split_off_new_bottom_states==" << split_off_new_bottom_states
//<< ", make_splitter_stable_early==" << make_splitter_stable_early << ", skip_transitions_in_splitter==" << skip_transitions_in_splitter << '\n';
      if (!initialisation && m_branching)
      {
        if (initialisation || split_off_new_bottom_states ||
            !make_splitter_stable_early || !skip_transitions_in_splitter)
        {
                                                                                #ifndef NDEBUG
          /* insert an empty BLC set into bi->block.to_constellation         */   if (!B->block.to_constellation.empty())  {  const transition* t;
          /* for the inert transitions out of *bi (unless the                */     assert(bi->block.to_constellation.empty() ||
          /* splitter inserted above was the set of inert transitions),      */            (t=&m_aut.get_transitions()[*bi->
          /* to avoid the need to check whether such a BLC set already exists*/                              block.to_constellation.begin()->start_same_BLC],
          /* in update_the_doubly_linked_list_LBC_new_block().               */             !is_inert_during_init(*t)) ||
          /* This set, if it remains empty, will need to be deleted          */            m_states[t->from()].block->c.onstellation!=
          /* after updating the BLC sets.                                    */                                    m_states[t->to()].block->c.onstellation);  }
                                                                                #endif
//std::cerr << "Inserting an empty BLC set for the constellation-inert transitions into the BLC list of " << bi->debug_id(*this) << '\n';
          BLC_list_iterator start_inert_BLC=
               B->block.to_constellation.empty()
                  ? m_BLC_transitions.data_end() // there are no inert transitions but we still insert a dummy set
                  : B->block.to_constellation.begin()->start_same_BLC; // if there are inert transitions, they are here
                                                                                linked_list<BLC_indicators>::iterator new_inert_BLC_set=
          bi->block.to_constellation.emplace_front(start_inert_BLC,
                                                        start_inert_BLC, true);
                                                                                #if !defined(NDEBUG) || defined(COUNT_WORK_BALANCE)
                                                                                  if (!B->block.to_constellation.empty() &&
                                                                                      start_inert_BLC<B->block.to_constellation.begin()->end_same_BLC)
                                                                                      // the first element that's currently in the old BLC list could be the
                                                                                      // dummy main splitter
                                                                                  {
                                                                                    const transition&perhaps_inert_t=m_aut.get_transitions()[*start_inert_BLC];
//std::cerr << "perhaps_inert_t==" << m_transitions[*start_inert_BLC].debug_id(*this) << '\n';
                                                                                    assert(m_states[perhaps_inert_t.from()].block==B ||
                                                                                           m_states[perhaps_inert_t.from()].block==bi);
                                                                                    if(is_inert_during_init(perhaps_inert_t) &&
                                                                                       m_states[perhaps_inert_t.to()].block->c.onstellation==B->c.onstellation)
                                                                                    {
                                                                                      // This are really the inert transitions, so we should copy the work
                                                                                      // counter
                                                                                      new_inert_BLC_set->work_counter=
                                                                                                               B->block.to_constellation.begin()->work_counter;
                                                                                    }
                                                                                  }
                                                                                #else
                                                                                  (void) new_inert_BLC_set; // avoid unused variable warning
                                                                                #endif
          // ++no_of_non_constellation_inert_BLC_sets; -- not needed, as this BLC set will contain constellation-inert transitions
        }                                                                       else
                                                                                {
                                                                                  #ifndef NDEBUG
                                                                                    assert(!bi->block.to_constellation.empty());
                                                                                    assert(bi->block.to_constellation.begin()->start_same_BLC<
                                                                                                             bi->block.to_constellation.begin()->end_same_BLC);
                                                                                    const transition& t=m_aut.get_transitions()
                                                                                                         [*bi->block.to_constellation.begin()->start_same_BLC];
                                                                                    assert(is_inert_during_init(t));
                                                                                    assert(m_states[t.from()].block->c.onstellation==
                                                                                                                       m_states[t.to()].block->c.onstellation);
                                                                                  #endif
                                                                                }
      }
                                                                                #ifndef USE_FOUR_WAY_SPLIT
      /* Recall new LBC positions.                                           */   assert(m_co_splitters_to_be_checked.empty());
                                                                                #endif
      for (state_in_block_pointer*
                       ssi=bi->start_bottom_states; ssi!=bi->end_states; ++ssi)
      {
        state_type_gj& s=*ssi->ref_state;                                       assert(s.ref_states_in_blocks==ssi);
                                                                                // mCRL2complexity(s, add_work(..., max_bi_counter), *this);
        const outgoing_transitions_it end_it=                                       // is subsumed in the above call
                       (std::next(ssi->ref_state)==m_states.end())
                       ? m_outgoing_transitions.end()
                       : std::next(ssi->ref_state)->start_outgoing_transitions; assert(s.block==bi);
        if constexpr (!initialisation)
        {
          // update the BLC_lists.
          for(outgoing_transitions_it ti=s.start_outgoing_transitions;
                                                              ti!=end_it; ti++)
          {                                                                     assert(m_states.begin()+m_aut.get_transitions()
                                                                                                            [*ti->ref.BLC_transitions].from()==ssi->ref_state);
                                                                                // mCRL2complexity(&m_transitions[*ti->ref.BLC_transitions],
                                                                                //                 add_work(..., max_bi_counter), *this);
            if (!skip_transitions_in_splitter ||                                    // is subsumed in the above call
                m_transitions[*ti->ref.BLC_transitions].
                              transitions_per_block_to_constellation!=splitter)
            {
              update_the_doubly_linked_list_LBC_new_block(B, bi,
               *ti->ref.BLC_transitions, old_constellation, new_constellation);
            }
          }
        }

        // Situation below is only relevant if the new block contains the
        // R-states:
        if (bi==R_block)
        {
          if (0 < s.no_of_outgoing_block_inert_transitions)
          {                                                                     assert(ssi>=R_block->sta.rt_non_bottom_states);
            // si is a non_bottom_state in the smallest block containing M.
//std::cerr << "Checking whether outgoing transitions of " << s.debug_id(*this) << " have become non-block-inert:\n";
            bool non_bottom_state_becomes_bottom_state=true;                    assert(m_branching);

            for(outgoing_transitions_it ti=s.start_outgoing_transitions;
                                                              ti!=end_it; ti++)
            {                                                                   // mCRL2complexity(&m_transitions[*ti->ref.BLC_transitions],
                                                                                //                 add_work(..., max_bi_counter), *this);
              const transition& t=m_aut.get_transitions()[!initialisation           // is subsumed in the above call
                             ? *ti->ref.BLC_transitions : ti->ref.transitions]; assert(m_states.begin()+t.from()==ssi->ref_state);
              if (!m_aut.is_tau(m_aut_apply_hidden_label_map(t.label())))
              {
                break;
              }
              if (m_states[t.to()].block==B)
              {
                // if (is_inert_during_init(t)) -- always true because it
                                                // cannot be a tau-self-loop
                {
                  // This is a transition that has become non-block-inert.
                  // (However, it is still constellation-inert.)
                  make_transition_non_block_inert(t);
                  if (!initialisation && split_off_new_bottom_states)
                  {
                    const linked_list<BLC_indicators>::iterator new_splitter=
                                  m_transitions[*ti->ref.BLC_transitions].
                                        transitions_per_block_to_constellation; assert(R_block->block.to_constellation.begin()==new_splitter);
                    if (R_to_U_tau_splitter==
                                         R_block->block.to_constellation.end())
                    {
                      R_to_U_tau_splitter=new_splitter;
                      R_to_U_tau_splitter->make_unstable();
                    }                                                           else
                                                                                {
                                                                                  assert(R_to_U_tau_splitter==new_splitter);
                    /* immediately mark this transition, in case we get */        assert(!R_to_U_tau_splitter->is_stable());
                    /* new bottom states: */                                    }
                    mark_BLC_transition(ti);
                  }
                }
              }
              else if (m_preserve_divergence && t.from()==t.to())
              {                                                                 assert(!is_inert_during_init(t));
                // transition is a divergent tau-self-loop. It never was inert.
              }
              else if (m_states[t.to()].block==R_block)
              {                                                                 assert(is_inert_during_init(t));
                // There is an outgoing inert tau. State remains non-bottom.
                non_bottom_state_becomes_bottom_state=false;
              }
            }
            if (non_bottom_state_becomes_bottom_state)
            {                                                                   assert(initialisation || !split_off_new_bottom_states ||
              /* The state at si has become a bottom_state. */                         R_block->block.to_constellation.end()!=R_to_U_tau_splitter);
              change_non_bottom_state_to_bottom_state(ssi->ref_state);
            }
          }
        }
        // Investigate the incoming formerly inert tau transitions.
        else if (R_block->sta.rt_non_bottom_states<R_block->end_states)
        {
//std::cerr << "Checking whether incoming transitions of " << s.debug_id(*this) << " have become non-block-inert:\n";
          const std::vector<transition>::iterator it_end =
                      std::next(ssi->ref_state)>=m_states.end()
                      ? m_aut.get_transitions().end()
                      : std::next(ssi->ref_state)->start_incoming_transitions;
          for(std::vector<transition>::iterator it =
                                s.start_incoming_transitions; it!=it_end; it++)
          {                                                                     // mCRL2complexity(&m_transitions[std::distance(m_aut.get_transitions().
                                                                                //                 begin(), it)], add_work(..., max_bi_counter), *this);
                                                                                    // subsumed in the above call
            const transition& t=*it;                                            assert(m_states.begin()+t.to()==ssi->ref_state);
            if (!m_aut.is_tau(m_aut_apply_hidden_label_map(t.label())))
            {
              break; // All tau transitions have been investigated.
            }

            const fixed_vector<state_type_gj>::iterator from
                                                   (m_states.begin()+t.from());
            if (from->block==R_block)
            {
              // This transition did become non-block-inert.
              // (However, it is still constellation-inert.)
              make_transition_non_block_inert(t);
              if (!initialisation && split_off_new_bottom_states)
              {
                const linked_list<BLC_indicators>::iterator new_splitter=
                   m_transitions[std::distance(m_aut.get_transitions().begin(),
                                  it)].transitions_per_block_to_constellation;  assert(R_block->block.to_constellation.begin()==new_splitter);
                if (R_to_U_tau_splitter==R_block->block.to_constellation.end())
                {
                  R_to_U_tau_splitter=new_splitter;
                  R_to_U_tau_splitter->make_unstable();
                }                                                               else
                                                                                {
                                                                                  assert(R_to_U_tau_splitter==new_splitter);
                /* immediately mark this transition, in case we get new */        assert(!R_to_U_tau_splitter->is_stable());
                /* bottom states: */                                            }
                mark_BLC_transition(m_transitions
                      [std::distance(m_aut.get_transitions().begin(), it)].
                                                     ref_outgoing_transitions);
              }
              // Check whether from is a new bottom state.
              if (from->no_of_outgoing_block_inert_transitions==0)
              {
                // This state has no more outgoing inert transitions. It
                // becomes a bottom state.
                change_non_bottom_state_to_bottom_state(from);
              }
            }
          }
        }
      }                                                                         assert(R_block->start_bottom_states<R_block->sta.rt_non_bottom_states);
      if constexpr (!initialisation)
      {
        if (m_branching)
        {                                                                         assert(!bi->block.to_constellation.empty());
          // Before the loop we inserted an empty BLC set for the inert
          // transitions into bi->block.to_constellation.
          // If it is still empty, we have to remove it again.
          linked_list<BLC_indicators>::iterator
                                  inert_ind=bi->block.to_constellation.begin();
          if (inert_ind->start_same_BLC==inert_ind->end_same_BLC)
          {                                                                     assert(inert_ind->is_stable());
            bi->block.to_constellation.erase(inert_ind);
          }                                                                     else
                                                                                { // assert(perhaps_inert_ind->is_stable()); -- it may be unstable
                                                                                  //                         if there are tau-transitions from R_block==bi to U
                                                                                  #ifndef NDEBUG
                                                                                    const transition& inert_t=
                                                                                                   m_aut.get_transitions()[*inert_ind->start_same_BLC];
                                                                                    assert(is_inert_during_init(inert_t));
                                                                                    assert(bi==m_states[inert_t.from()].block);
                                                                                    assert(bi->c.onstellation==m_states[inert_t.to()].block->c.onstellation);
                                                                                  #endif
                                                                                }
        }
        for (std::vector<linked_list<BLC_indicators>::iterator>::iterator
                                 it=m_BLC_indicators_to_be_deleted.begin();
                                 it<m_BLC_indicators_to_be_deleted.end(); ++it)
        {                                                                       assert((*it)->start_same_BLC==(*it)->end_same_BLC);
                                                                                // the work in this loop can be attributed to the operation that aded this BLC
                                                                                // set to m_BLC_indicators_to_be_deleted
          B->block.to_constellation.erase(*it);
        }
        clear(m_BLC_indicators_to_be_deleted);
                                                                                #ifndef NDEBUG
                                                                                  for (const BLC_indicators& B_it : B->block.to_constellation)
                                                                                  {
                                                                                    assert(B_it.start_same_BLC<B_it.end_same_BLC);
                                                                                  }
                                                                                  for (const BLC_indicators& bi_it : bi->block.to_constellation)
                                                                                  {
                                                                                    assert(bi_it.start_same_BLC<bi_it.end_same_BLC);
                                                                                  }
                                                                                #endif
#ifndef USE_FOUR_WAY_SPLIT
        for (const std::pair<BLC_list_iterator, block_type*>&
                               co_splitter_check: m_co_splitters_to_be_checked)
        {                                                                       assert(m_BLC_transitions.data()<=co_splitter_check.first);
                                                                                // The work in this loop can be attributed to the operation that added
                                                                                // `co_splitter_check` to `m_co_splitters_to_be_checked`
          const transition_index co_transition=*co_splitter_check.first;        assert(co_splitter_check.first<m_BLC_transitions.data_end());
          block_type* const from_block=co_splitter_check.second;                assert(B==from_block || bi==from_block);
          const transition& co_t=m_aut.get_transitions()[co_transition];        assert(old_constellation==m_states[co_t.to()].block->c.onstellation);
          if (m_states[co_t.from()].block==from_block)
          {
            linked_list<BLC_indicators>::iterator co_splitter=
                            m_transitions[co_transition].
                                        transitions_per_block_to_constellation; assert(co_splitter->start_same_BLC<=co_splitter_check.first);
//std::cerr << "Checking co-splitter " << co_splitter->debug_id(*this);
                                                                                assert(co_splitter_check.first<co_splitter->end_same_BLC);
                                                                                assert(!co_splitter->is_stable());
          /*const transition* sp_t;
            if (!split_off_new_bottom_states &&
                (sp_t=&m_aut.get_transitions()[*splitter->start_same_BLC],
                 assert(R_block==m_states[sp_t->from()].block),
                 label_or_divergence(*sp_t)==label_or_divergence(co_t)) &&
                new_constellation==m_states[sp_t->to()].block->c.onstellation)
            {
              // This is actually (a part of) the co-splitter belonging to the current main splitter
              // So we leave the co-splitter as it is.
//std::cerr << ": belongs to the main splitter used in the current split\n";
            }
            else*/
            {
              linked_list<BLC_indicators>& btc=
                                            from_block->block.to_constellation;
              linked_list<BLC_indicators>::const_iterator main_splitter=
                                                         btc.next(co_splitter);
              const transition* t;
              if (from_block->block.to_constellation.end()==main_splitter ||
                  (t=&m_aut.get_transitions()[*main_splitter->start_same_BLC],  assert(m_states[t->from()].block==from_block),
                   label_or_divergence(co_t)!=label_or_divergence(*t)) ||
                  new_constellation!=m_states[t->to()].block->c.onstellation)
              {
                /* co_splitter does not have a corresponding main splitter   */ assert(!co_splitter->has_marked_transitions());
//std::cerr << ": does not have a corresponding main splitter";  if (from_block->block.to_constellation.end()!=main_splitter) { std::cerr << ", candidate was " << main_splitter->debug_id(*this); }  std::cerr << '\n';
                // Note that if splitter is a main splitter, then the
                // co-splitter that belongs to it should be identifiable.  That
                // is why we do not always make the splitter stable early, even
                // though in the main routine we do no longer need the link
                // between main and co-splitter maintained after the main
                // split.
                co_splitter->make_stable();
              }
//else { std::cerr << ": its main splitter is " << main_splitter->debug_id(*this) << '\n'; }
            }
          }                                                                     else  {  assert(bi==m_states[co_t.from()].block);  }
        }
        clear(m_co_splitters_to_be_checked);
#endif
      }                                                                         else
                                                                                {
                                                                                  assert(m_BLC_indicators_to_be_deleted.empty());
                                                                                  #ifndef USE_FOUR_WAY_SPLIT
                                                                                    assert(m_co_splitters_to_be_checked.empty());
                                                                                  #endif
                                                                                }
                                                                                #if !defined(NDEBUG) || defined(COUNT_WORK_BALANCE)
                                                                                  unsigned char const max_block=check_complexity::log_n-
                                                                                                       check_complexity::ilog2(number_of_states_in_block(*bi));
                                                                                  if (bi==R_block)
                                                                                  {
                                                                                    // account for the work in R
                                                                                    for (const state_in_block_pointer*
                                                                                                             s=bi->start_bottom_states; s!=bi->end_states; ++s)
                                                                                    {
                                                                                      mCRL2complexity(s->ref_state, finalise_work(check_complexity::simple_splitB_R__find_predecessors, check_complexity::simple_splitB__find_predecessors_of_R_or_U_state, max_block), *this);
                                                                                      // incoming tau-transitions of s
                                                                                      const std::vector<transition>::iterator in_ti_end = std::next(s->ref_state)>=m_states.end() ? m_aut.get_transitions().end() : std::next(s->ref_state)->start_incoming_transitions;
                                                                                      for (std::vector<transition>::iterator ti=s->ref_state->start_incoming_transitions; ti!=in_ti_end; ++ti)
                                                                                      {
                                                                                        if (!m_aut.is_tau(m_aut_apply_hidden_label_map(ti->label())))  break;
                                                                                        mCRL2complexity(&m_transitions[std::distance(m_aut.get_transitions().begin(), ti)], finalise_work(check_complexity::simple_splitB_R__handle_transition_to_R_state, check_complexity::simple_splitB__handle_transition_to_R_or_U_state, max_block), *this);
                                                                                      }
                                                                                      // outgoing transitions of s
                                                                                      const outgoing_transitions_it out_ti_end=std::next(s->ref_state)>=m_states.end() ? m_outgoing_transitions.end() : std::next(s->ref_state)->start_outgoing_transitions;
                                                                                      for (outgoing_transitions_it ti=s->ref_state->start_outgoing_transitions; ti!=out_ti_end; ++ti)
                                                                                      {
                                                                                        mCRL2complexity(&m_transitions[!initialisation ? *ti->ref.BLC_transitions : ti->ref.transitions], finalise_work(check_complexity::simple_splitB_R__handle_transition_from_R_state, check_complexity::simple_splitB__handle_transition_from_R_or_U_state, max_block), *this);
                                                                                        // We also need to cancel the work on outgoing transitions of U-state candidates that turned out to be new bottom states:
                                                                                        mCRL2complexity(&m_transitions[!initialisation ? *ti->ref.BLC_transitions : ti->ref.transitions], cancel_work(check_complexity::simple_splitB_U__handle_transition_from_potential_U_state), *this);
                                                                                      }
                                                                                    }
                                                                                    // ensure not too much work has been done on U
                                                                                    for (const state_in_block_pointer*
                                                                                                               s=B->start_bottom_states; s!=B->end_states; ++s)
                                                                                    {
                                                                                      mCRL2complexity(s->ref_state, cancel_work(check_complexity::simple_splitB_U__find_predecessors), *this);
                                                                                      // incoming tau-transitions of s
                                                                                      const std::vector<transition>::iterator in_ti_end=std::next(s->ref_state)>=m_states.end() ? m_aut.get_transitions().end() : std::next(s->ref_state)->start_incoming_transitions;
                                                                                      for (std::vector<transition>::iterator ti=s->ref_state->start_incoming_transitions; ti!=in_ti_end; ++ti)
                                                                                      {
                                                                                        if (!m_aut.is_tau(m_aut_apply_hidden_label_map(ti->label())))  break;
                                                                                        mCRL2complexity(&m_transitions[std::distance(m_aut.get_transitions().begin(), ti)], cancel_work(check_complexity::simple_splitB_U__handle_transition_to_U_state), *this);
                                                                                      }
                                                                                      // outgoing transitions of s
                                                                                      const outgoing_transitions_it out_ti_end=std::next(s->ref_state)>=m_states.end() ? m_outgoing_transitions.end() : std::next(s->ref_state)->start_outgoing_transitions;
                                                                                      for (outgoing_transitions_it ti=s->ref_state->start_outgoing_transitions; ti!=out_ti_end; ++ti)
                                                                                      {
                                                                                        mCRL2complexity(&m_transitions[!initialisation ? *ti->ref.BLC_transitions : ti->ref.transitions], cancel_work(check_complexity::simple_splitB_U__handle_transition_from_potential_U_state), *this);
                                                                                      }
                                                                                    }
                                                                                  }
                                                                                  else
                                                                                  {
                                                                                    // account for the work in U
                                                                                    for (const state_in_block_pointer*
                                                                                                             s=bi->start_bottom_states; s!=bi->end_states; ++s)
                                                                                    {
                                                                                      mCRL2complexity(s->ref_state, finalise_work(check_complexity::simple_splitB_U__find_predecessors, check_complexity::simple_splitB__find_predecessors_of_R_or_U_state, max_block), *this);
                                                                                      // incoming tau-transitions of s
                                                                                      const std::vector<transition>::iterator in_ti_end = std::next(s->ref_state)>=m_states.end() ? m_aut.get_transitions().end() : std::next(s->ref_state)->start_incoming_transitions;
                                                                                      for (std::vector<transition>::iterator ti=s->ref_state->start_incoming_transitions; ti!=in_ti_end; ++ti)
                                                                                      {
                                                                                        if (!m_aut.is_tau(m_aut_apply_hidden_label_map(ti->label())))  break;
                                                                                        mCRL2complexity(&m_transitions[std::distance(m_aut.get_transitions().begin(), ti)], finalise_work(check_complexity::simple_splitB_U__handle_transition_to_U_state, check_complexity::simple_splitB__handle_transition_to_R_or_U_state, max_block), *this);
                                                                                      }
                                                                                      // outgoing transitions of s
                                                                                      const outgoing_transitions_it out_ti_end=std::next(s->ref_state)>=m_states.end() ? m_outgoing_transitions.end() : std::next(s->ref_state)->start_outgoing_transitions;
                                                                                      for (outgoing_transitions_it ti = s->ref_state->start_outgoing_transitions; ti!=out_ti_end; ++ti)
                                                                                      {
                                                                                        mCRL2complexity(&m_transitions[!initialisation ? *ti->ref.BLC_transitions : ti->ref.transitions], finalise_work(check_complexity::simple_splitB_U__handle_transition_from_potential_U_state, check_complexity::simple_splitB__handle_transition_from_R_or_U_state, max_block), *this);
                                                                                      }
                                                                                    }
                                                                                    // ensure not too much work has been done on R
                                                                                    for (const state_in_block_pointer*
                                                                                                               s=B->start_bottom_states; s!=B->end_states; ++s)
                                                                                    {
                                                                                      mCRL2complexity(s->ref_state, cancel_work(check_complexity::simple_splitB_R__find_predecessors), *this);
                                                                                      // incoming tau-transitions of s
                                                                                      const std::vector<transition>::iterator in_ti_end = std::next(s->ref_state)>=m_states.end() ? m_aut.get_transitions().end() : std::next(s->ref_state)->start_incoming_transitions;
                                                                                      for (std::vector<transition>::iterator ti=s->ref_state->start_incoming_transitions; ti!=in_ti_end; ++ti)
                                                                                      {
                                                                                        if (!m_aut.is_tau(m_aut_apply_hidden_label_map(ti->label())))  break;
                                                                                        mCRL2complexity(&m_transitions[std::distance(m_aut.get_transitions().begin(), ti)], cancel_work(check_complexity::simple_splitB_R__handle_transition_to_R_state), *this);
                                                                                      }
                                                                                      // outgoing transitions of s
                                                                                      const outgoing_transitions_it out_ti_end=std::next(s->ref_state)>=m_states.end() ? m_outgoing_transitions.end() : std::next(s->ref_state)->start_outgoing_transitions;
                                                                                      for (outgoing_transitions_it ti=s->ref_state->start_outgoing_transitions; ti!=out_ti_end; ++ti)
                                                                                      {
                                                                                        mCRL2complexity(&m_transitions[!initialisation ? *ti->ref.BLC_transitions : ti->ref.transitions], cancel_work(check_complexity::simple_splitB_R__handle_transition_from_R_state), *this);
                                                                                        // We also need to move the work on outgoing transitions of U-state candidates that turned out to be new bottom states:
                                                                                        mCRL2complexity(&m_transitions[!initialisation ? *ti->ref.BLC_transitions : ti->ref.transitions], finalise_work(check_complexity::simple_splitB_U__handle_transition_from_potential_U_state, check_complexity::simple_splitB__test_outgoing_transitions_found_new_bottom_state,
                                                                                                0==s->ref_state->no_of_outgoing_block_inert_transitions ? 1 : 0), *this);
                                                                                      }
                                                                                    }
                                                                                  }
                                                                                  check_complexity::check_temporary_work();
                                                                                #endif // ifndef NDEBUG
      if (initialisation || split_off_new_bottom_states)
      {
        if (start_new_bottom_states<R_block->sta.rt_non_bottom_states)
        {                                                                       assert(initialisation || !R_block->block.to_constellation.empty());
          /* There are new bottom states, and (unless we are initialising)   */ assert(initialisation ||
          /* we have to separate them immediately from the old bottom        */        R_block->block.to_constellation.begin()==R_to_U_tau_splitter);
          /* states.  This will make sure that the next call of stabilizeB() */ assert(initialisation || R_to_U_tau_splitter->has_marked_transitions());
          // will not encounter blocks with both old and new bottom states.
          block_type* new_bottom_block;
          if (initialisation ||
              start_new_bottom_states==R_block->start_bottom_states)
          {
            // all bottom states in this block are new
            // (or, during initialisation, we can regard all bottom states in
            // this block as new, as they have never been treated as new bottom
            // states earlier.)
            if constexpr (!initialisation)
            {
              R_to_U_tau_splitter->make_stable();
            }
            new_bottom_block = R_block;
            R_block = null_block;
            // During stabilizeB(), the splitter should be made stable again,
            // even the part starting in the new bottom states.
            // However, during stabilizeB() we have null_constellation ==
            /* old_constellation, so it has already been made stable.        */ assert(make_splitter_stable_early || null_constellation!=old_constellation);
          }
          else if constexpr (!initialisation)
          {                                                                     assert(number_of_states_in_block(*R_block)>1);
            const BLC_list_iterator splitter_start_same_BLC=
                                                      splitter->start_same_BLC;
                                                                                #ifndef NDEBUG
                                                                                  const BLC_list_iterator splitter_end_same_BLC=splitter->end_same_BLC;
                                                                                  const BLC_list_iterator R_to_U_tau_splitter_start_same_BLC=
            /* Some tau-transitions from R to U may come out of states that  */                                            R_to_U_tau_splitter->start_same_BLC;
            /* are not (yet) new bottom states.  Therefore we still have to  */   const BLC_list_iterator R_to_U_tau_splitter_end_same_BLC=
            /* go through the movement of constructing m_R:                  */                                              R_to_U_tau_splitter->end_same_BLC;
                                                                                #endif
            first_unmarked_bottom_state=not_all_bottom_states_are_touched
                                                  (R_block, R_to_U_tau_splitter
                                                                                #ifndef NDEBUG
                                                                                  , R_to_U_tau_splitter->start_same_BLC
                                                                                #endif
                                                   );                           assert(R_to_U_tau_splitter_start_same_BLC==
                                                                                                                          R_to_U_tau_splitter->start_same_BLC);
                                                                                assert(std::distance(start_new_bottom_states,
                                                                                                     R_block->sta.rt_non_bottom_states)==
                                                                                       std::distance(R_block->start_bottom_states,
                                                                                                                                first_unmarked_bottom_state));
            block_type* new_block;
            new_bottom_block=splitB<initialisation>(R_block,
                                    R_to_U_tau_splitter,
                                    first_unmarked_bottom_state,
                                    R_to_U_tau_splitter->start_same_BLC,
                                    new_block,
                                    old_constellation, new_constellation,
                                    false);
//std::cerr << "new_bottom_block==" << new_bottom_block << '\n';
            if (R_block == new_bottom_block)
            {                                                                   assert(m_states[m_aut.get_transitions()[*std::prev(
              /* The new block contains the old bottom states.               */         R_to_U_tau_splitter_end_same_BLC)].from()].block==new_bottom_block);
              R_block=new_block;
              /* All parts of R_to_U_tau_splitter should be made stable.     */ assert(new_bottom_block->block.to_constellation.begin()==
              /* The new part is already stable: as it contains inert        */           m_transitions[*std::prev(R_to_U_tau_splitter_end_same_BLC)].
              /* transitions, it is always created stably.                   */                                     transitions_per_block_to_constellation);
              /* However, the old part must still contain some transitions   */ assert(!new_bottom_block->block.to_constellation.empty());
              /* that are not marked stable; it cannot be empty, as the      */ assert(new_bottom_block->block.to_constellation.begin()->is_stable());
              /* new_bottom_block always has some constellation-inert        */ assert(m_states[m_aut.get_transitions()
              /* transitions.                                                */                           [*splitter_start_same_BLC].from()].block==R_block);

              // The original splitter part of the old bottom states
              // (i.e. the part of the new block) should be made stable.
              if (!make_splitter_stable_early)
              {                                                                 assert(null_constellation!=old_constellation);
                const linked_list<BLC_indicators>::iterator
                        new_part_of_splitter=
                            m_transitions[*splitter_start_same_BLC].
                                        transitions_per_block_to_constellation; assert(new_part_of_splitter!=splitter);
                /* As the splitter both contains transitions from every old  */ assert(splitter_start_same_BLC==new_part_of_splitter->start_same_BLC);
                /* bottom state and from every new bottom state, it must     */ assert(new_part_of_splitter==m_transitions
                /* have been separated into two parts.                       */         [*splitter_start_same_BLC].transitions_per_block_to_constellation);
                new_part_of_splitter->make_stable();
              }
            }
            else
            {                                                                   assert(new_block==new_bottom_block);
              /* All parts of R_to_U_tau_splitter should be made stable.     */ assert(m_states[m_aut.get_transitions()
              /* The new part is already stable: as it contains inert        */        [*R_to_U_tau_splitter_start_same_BLC].from()].block==new_bottom_block);
//std::cerr << "Transition at m_transitions[*R_to_U_tau_splitter_start_same_BLC]==" << m_transitions[*R_to_U_tau_splitter_start_same_BLC].debug_id(*this) << '\n';
//std::cerr << "m_transitions[*R_to_U_tau_splitter_start_same_BLC].transitions_per_block_to_constellation==" << m_transitions[*R_to_U_tau_splitter_start_same_BLC].transitions_per_block_to_constellation->debug_id(*this) << '\n';
//std::cerr << "new_bottom_block->block.to_constellation.begin()==" << new_bottom_block->block.to_constellation.begin()->debug_id(*this) << '\n';
              /* transitions, it is always created stably.                   */ assert(new_bottom_block->block.to_constellation.begin()==
              /* It cannot be empty, as the new bottom block always has some */                 m_transitions[*R_to_U_tau_splitter_start_same_BLC].
              /* constellation-inert transitions.                            */                                        transitions_per_block_to_constellation);
              /* However, the old part may still contain some transitions.   */ assert(!new_bottom_block->block.to_constellation.empty());
                                                                                assert(new_bottom_block->block.to_constellation.begin()->is_stable());
              /* Also, R_block still has transitions in the original splitter*/ assert(!R_block->block.to_constellation.empty());
                                                                                #ifndef NDEBUG
                                                                                  if (R_to_U_tau_splitter_end_same_BLC==
                                                                                                         R_block->block.to_constellation.begin()->end_same_BLC)
                                                                                  {
                                                                                    assert(R_block->block.to_constellation.begin()->is_stable());
                                                                                  } else {
                                                                                    // R_block must have lost all its inert transitions.
                                                                                    assert(R_to_U_tau_splitter_end_same_BLC==
                                                                                               new_bottom_block->block.to_constellation.begin()->end_same_BLC);
                                                                                    assert(new_bottom_block->block.to_constellation.begin()==
                                                                                          m_transitions[*std::prev(R_to_U_tau_splitter_end_same_BLC)].
                                                                                                                       transitions_per_block_to_constellation);
                                                                                    const transition& not_inert_t=m_aut.get_transitions()
                                                                                                    [*R_block->block.to_constellation.begin()->start_same_BLC];
                                                                                    assert(R_block==m_states[not_inert_t.from()].block);
                                                                                    assert(!is_inert_during_init(not_inert_t) ||
                                                                                           R_block->c.onstellation!=
                                                                                                             m_states[not_inert_t.to()].block->c.onstellation);
                                                                                  }
                                                                                #endif
              /* The original splitter part of the old bottom states         */ assert(m_states[m_aut.get_transitions()
              /* (i.e. the part of the old block) should be made stable.     */             [*std::prev(splitter_end_same_BLC)].from()].block==R_block);
              if (!make_splitter_stable_early)
              {                                                                 assert(null_constellation!=old_constellation);
                splitter->make_stable();
              }
            }                                                                   assert(!R_block->block.to_constellation.empty());
                                                                                assert(splitter_end_same_BLC==splitter->end_same_BLC);
                                                                                assert(splitter==m_transitions[*std::prev(splitter_end_same_BLC)].
                                                                                                                       transitions_per_block_to_constellation);
          }                                                                     else  {  assert(0);  }
                                                                                /* assert(0 <= new_bottom_block); Always true */  
                                                                                assert(!new_bottom_block->contains_new_bottom_states);
//std::cerr << "new_bottom_block = " << new_bottom_block << ", R_block = " << R_block << '\n';
          new_bottom_block->contains_new_bottom_states=true;                    assert(!initialisation || nullptr==new_bottom_block->block.R);
          m_blocks_with_new_bottom_states.push_back(new_bottom_block);
        }
        else if constexpr (!initialisation)
        {
          // No new bottom states found.
          // Still there might be some transitions that became non-inert.
          if (R_block->block.to_constellation.end()!=R_to_U_tau_splitter)
          {                                                                     assert(R_block->block.to_constellation.begin()==R_to_U_tau_splitter);
            // unmark the transitions from R to U.
            // We do not need to stabilize the (formerly inert) tau-transitions
            // from R to U, because they are still in the same constellation.
            R_to_U_tau_splitter->make_stable();
          }
          if (!make_splitter_stable_early)
          {                                                                     assert(null_constellation!=old_constellation);
            splitter->make_stable();
          }
        }
      }                                                                         else  {  assert(R_block->block.to_constellation.end()==R_to_U_tau_splitter);  }
      return R_block;
    }

#else // #ifndef USE_FOUR_WAY_SPLIT

    /// \brief Move states in a set to a specific position in `m_states_in_block`
    /// \param R       vector of states that need to be moved
    /// \param to_pos  position where the first state in `R` needs to move to
    /// \details The work on this is assigned to the states in vector `R`.
    void move_nonbottom_states_to(const todo_state_vector& R,
                                  state_in_block_pointer* to_pos
                                                                                #if !defined(NDEBUG) || defined(COUNT_WORK_BALANCE)
                                                                                  , state_index new_block_bottom_size
                                                                                #endif
                                                                              )
    {
//std::cerr << "move_nonbottom_states_to( { "; for (state_in_block_pointer st: R) std::cerr << st.ref_state->debug_id_short(*this) << ' ';
//std::cerr << "}, &m_states_in_blocks[" << std::distance(m_states_in_blocks.begin(), to_pos) << "...])\n";
                                                                                #if !defined(NDEBUG) || defined(COUNT_WORK_BALANCE)
                                                                                  unsigned char const max_B=check_complexity::log_n-
                                                                                                       check_complexity::ilog2(new_block_bottom_size+R.size());
                                                                                #endif
      for (state_in_block_pointer st: R)
      {                                                                         mCRL2complexity(st.ref_state, add_work(check_complexity::
                                                                                               split_block_B_into_R_and_BminR__carry_out_split, max_B), *this);
        swap_states_in_states_in_block(to_pos++,
                                           st.ref_state->ref_states_in_blocks);
      }
      return;
    }

    /// \brief Update all BLC sets after a new block has been created
    /// \param old_bi             index of the old block from which states have been taken
    /// \param new_bi             index of the new block
    /// \param old_constellation  old constellation that was split most recently
    /// \details The old constellation is used to maintain the order of
    /// main/co-splitter pairs in the list of BLC sets (remember that we should
    /// have the main splitter immediately before its co-splitter).
    block_type* update_BLC_sets_new_block(block_type* const old_bi,
                                   block_type* const new_bi,
                                   constellation_type* const old_constellation,
                                   constellation_type* const new_constellation)
    {
//std::cerr << "update_BLC_sets_new_block(" << old_bi->debug_id(*this) << ", " << new_bi->debug_id(*this) << ", " << old_constellation->debug_id(*this) << ")\n";
      // adapt the BLC sets of a new block B in a way that they are consistent
      /* with the previous version...                                        */ assert(!old_bi->block.to_constellation.empty());
      if (m_branching)
      {
//std::cerr << "Inserting an empty BLC set for the constellation-inert transitions into the BLC list of " << new_bi->debug_id(*this) << '\n';
        BLC_list_iterator start_inert_BLC=
                        old_bi->block.to_constellation.begin()->start_same_BLC; // if there are inert transitions, they are here
                                                                                linked_list<BLC_indicators>::iterator new_inert_BLC_set=
        new_bi->block.to_constellation.emplace_front(start_inert_BLC,
                                                        start_inert_BLC, true);
                                                                                #if !defined(NDEBUG) || defined(COUNT_WORK_BALANCE)
                                                                                  assert(start_inert_BLC<
                                                                                                         old_bi->block.to_constellation.begin()->end_same_BLC);
                                                                                  const transition& perhaps_inert_t=m_aut.get_transitions()[*start_inert_BLC];
                                                                                  assert(m_states[perhaps_inert_t.from()].block==old_bi ||
                                                                                         m_states[perhaps_inert_t.from()].block==new_bi);
                                                                                  if (is_inert_during_init(perhaps_inert_t) &&
                                                                                      m_states[perhaps_inert_t.to()].block->c.onstellation==
                                                                                                                                        old_bi->c.onstellation)
                                                                                  {
                                                                                    // This are really the inert transitions, so we should copy the work
                                                                                    // counter
                                                                                    new_inert_BLC_set->work_counter=
                                                                                                          old_bi->block.to_constellation.begin()->work_counter;
                                                                                  }
                                                                                #else
                                                                                  (void) new_inert_BLC_set; // avoid unused variable warning
                                                                                #endif
      }

      const state_in_block_pointer* const it_end=new_bi->end_states;
      for (state_in_block_pointer*
                              it=new_bi->start_bottom_states; it_end!=it; ++it)
      {                                                                         assert(new_bi==it->ref_state->block);
        outgoing_transitions_const_it const out_it_end=
                std::next(it->ref_state)==m_states.end()
                        ? m_outgoing_transitions.end()
                        : std::next(it->ref_state)->start_outgoing_transitions;
        for (outgoing_transitions_it out_it=it->ref_state->
                      start_outgoing_transitions; out_it_end!=out_it; ++out_it)
        {
          update_the_doubly_linked_list_LBC_new_block(old_bi, new_bi,
           *out_it->ref.BLC_transitions, old_constellation, new_constellation);
        }
      }

      if (m_branching)
      {                                                                         assert(!new_bi->block.to_constellation.empty());
        // If the dummy set inserted before the loop is still empty, we remove
        // it again.
        // Before the loop we inserted an empty BLC set for the inert
        // transitions into new_bi->block.to_constellation.
        // If it is still empty, we have to remove it again.
        linked_list<BLC_indicators>::iterator
                              inert_ind=new_bi->block.to_constellation.begin();
        if (inert_ind->start_same_BLC==inert_ind->end_same_BLC)
        {                                                                       assert(inert_ind->is_stable());
          new_bi->block.to_constellation.erase(inert_ind);
        }
      }

      for (std::vector<linked_list<BLC_indicators>::iterator>::iterator
                                 it=m_BLC_indicators_to_be_deleted.begin();
                                 it<m_BLC_indicators_to_be_deleted.end(); ++it)
      {                                                                         assert((*it)->start_same_BLC==(*it)->end_same_BLC);
                                                                                // the work in this loop can be attributed to the operation that added this BLC
        old_bi->block.to_constellation.erase(*it);                              // set to m_BLC_indicators_to_be_deleted
      }
      clear(m_BLC_indicators_to_be_deleted);

      // Actually it is not necessary to maintain the order (first stable, then
      // unstable) in the BLC list during the main/co-split phase; this order
      // is only needed in the new bottom split phase.  So it would probably be
      // ok to just leave these co-splitters where they are actually and only
      // make them stable.
      return new_bi;
    }

    /// \brief create a new block and adapt the BLC sets, and reset state counters
    /// \param start_bottom_states      pointer to the first bottom state of the new block in `m_states_in_blocks`
    /// \param start_non_bottom_states  pointer to the first non-bottom state of the new block in `m_states_in_blocks`
    /// \param end_states               pointer past the last state of the new block in `m_states_in_blocks`
    /// \param constellation            constellation of the new block
    /// \param old_constellation        old constellation of the most recent constellation-split, used to update the BLC sets
    template<bool initialisation = false>
    block_type* create_new_block(
            state_in_block_pointer* start_bottom_states,
            state_in_block_pointer* const start_non_bottom_states,
            state_in_block_pointer* const end_states,
            block_type* const old_block_index,
            constellation_type* const old_constellation,
            constellation_type* const new_constellation)
    {
      constellation_type* const constellation=old_block_index->c.onstellation;  assert(constellation->start_const_states<=start_bottom_states);
                                                                                assert(start_bottom_states<end_states);
      block_type* const new_block_index=
                #ifdef USE_POOL_ALLOCATOR
                    simple_list<BLC_indicators>::get_pool().
                    template construct<block_type>
                #else
                    new block_type
                #endif
                          (start_bottom_states,
                           start_non_bottom_states, end_states, constellation); assert(end_states<=constellation->end_const_states);
      ++no_of_blocks;
                                                                                #ifndef NDEBUG
                                                                                  new_block_index->work_counter=old_block_index->work_counter;
                                                                                #endif

//std::cerr << "Creating new " << new_block_index->debug_id(*this) << " from " << old_block_index->debug_id(*this) << " with states";
      for(; start_bottom_states<start_non_bottom_states; ++start_bottom_states)
      {                                                                         assert(0==
                                                                                       start_bottom_states->ref_state->no_of_outgoing_block_inert_transitions);
//std::cerr << ' ' << std::distance(m_states.begin(), start_bottom_states->ref_state);
                                                                                assert(old_block_index==start_bottom_states->ref_state->block);
        start_bottom_states->ref_state->block=new_block_index;                  assert(start_bottom_states->ref_state->counter==undefined);
      }
//std::cerr << " |";
      for (; start_bottom_states<end_states; ++start_bottom_states)
      {                                                                         assert(0!=
                                                                                       start_bottom_states->ref_state->no_of_outgoing_block_inert_transitions);
//std::cerr << ' ' << std::distance(m_states.begin(), start_bottom_states->ref_state);
                                                                                assert(old_block_index==start_bottom_states->ref_state->block);
        start_bottom_states->ref_state->block=new_block_index;
        start_bottom_states->ref_state->counter=undefined;
      }
//std::cerr << '\n';

      if constexpr (initialisation)
      {
        return new_block_index;
      }
      return update_BLC_sets_new_block(old_block_index, new_block_index,
                                         old_constellation, new_constellation);
    }

#if 0
    // starting an idea to implement a direct five-way-reorder function to move
    // the states to their destination towards the end of the four-way-split.
    template<unsigned no_of_bottom_slices, unsigned no_of_non_bottom_slices>
    void multi_way_reorder()
    {
      static_assert(0<=no_of_bottom_slices);
      static_assert(no_of_bottom_slices<=2);
      static_assert(0<=no_of_non_bottom_slices);
      static_assrt(no_of_non_bottom_slices<=4);
      // There are up to two bottom subblocks (MissMain and XcludeCo) that need
      // to be moved:
        // a bottom slice is indicated with:
        // - state_in_block_pointer* a start iterator for the current location
        // - a start iterator for the desired location
        // - an end iterator for the desired location
        // The requirement is to move the states in the range between start and
        // end iterator to the desired location. We always have start<=desired.
        // To determine overlap: current_start+(desired_end-desired_start) is
        // the current end; if that is >desired_start, there is overlap.
      // There are up to four non-bottom subblocks that need to be moved:
        // a non-bottom slice is indicated with
        // - std::vector<state_in_block_pointer>::iterator a start iterator for the current location
        // - a start iterator for the desired location
        // - and end iterator for the desired location
      // Why start and end iterators for the desired location? That makes it
      // easier to identify what kind of states should go into a hole.
      // The slices should be ordered by desired location in a vector; then we
      // can find a state type based on the location.

      // not sure why bottom states should be preferred. I think my original
      // thought was: almost every swap will involve some non-bottom state, so
      // having the bottom states out of the way would simplify the logic.

      // I suggest to implement the choice between the different subblocks
      // using a template parameter that indicates the number of subblocks with
      // unfinished states.
      state_in_block_pointer* hole;
      fixed_vector<state_type_gj>::iterator state;

      if constexpr (0<no_of_bottom_slices)
      {
        do
        {
          pick up a bottom state that is in a place where it shouldn't be;
          do
          {
            if the current picked-up state is in a finished subblock
            {
              move the state to its final position (and pick up the state that
              was there)
            }
            else
            {
              do
              {
                find a state that can be placed in the hole;
                (and move the hole to where that state came from)
              }
              while (the picked-up state does not fit into the hole);
              break;
            }
          }
          while (the picked-up state does not fit into the hole);
          place the current picked-up state in the hole;
        }
        while (there are bottom states that need to be moved);
      }
      else
      {
        while (...) {
          pick up a non-bottom state that is in a place where it shouldn't be
          etc. rest is the same as above
        }
      }
    }
#endif

    /// \brief makes incoming transitions from block `MultiSub_block_index` non-block-inert
    void check_incoming_tau_transitions_become_noninert(
          block_type* MultiSub_block_index,
          state_in_block_pointer* start_bottom,
          state_in_block_pointer* const end_non_bottom)
    {
      for (; start_bottom!=end_non_bottom; ++start_bottom)
      {
//std::cerr << "Checking whether incoming transitions of " << start_bottom->ref_state->debug_id(*this) << " have become non-block-inert:\n";
        std::vector<transition>::const_iterator const in_it_end=
           std::next(start_bottom->ref_state)>=m_states.end()
              ? m_aut.get_transitions().end()
              : std::next(start_bottom->ref_state)->start_incoming_transitions; assert(start_bottom->ref_state->block!=MultiSub_block_index);
        for (std::vector<transition>::iterator
                 in_it=start_bottom->ref_state->start_incoming_transitions;
                    in_it!=in_it_end &&
                    m_aut.is_tau(m_aut_apply_hidden_label_map(in_it->label()));
                                                                       ++in_it)
        {
          const fixed_vector<state_type_gj>::iterator
                                           from=m_states.begin()+in_it->from(); assert(m_states[in_it->to()].ref_states_in_blocks==start_bottom);
          if (MultiSub_block_index==from->block)
          {
            // make_transition_non_block_inert(*in_it);
//std::cerr << "    " << m_transitions[std::distance(m_aut.get_transitions().begin(), in_it)].debug_id(*this) << " has become non-block-inert";
            if (0== --from->no_of_outgoing_block_inert_transitions)
            {
//std::cerr << " and its source is a new bottom state";
              change_non_bottom_state_to_bottom_state(from);
            }
//std::cerr << '\n';
          }
        }
      }
    }

    /// \brief find the next constellation after `splitter_it`'s in the `same_saC` slice of the outgoing transitions
    /// \details Assumes that the BLC sets are fully initialized.
    linked_list<BLC_indicators>::const_iterator
           next_target_constln_in_same_saC(state_in_block_pointer const src,
                               BLC_list_const_iterator const splitter_it) const
    {                                                                           assert(m_states.begin()+m_aut.get_transitions()[*splitter_it].from()==
                                                                                                                                                src.ref_state);
      outgoing_transitions_const_it
                   out_it=m_transitions[*splitter_it].ref_outgoing_transitions;
      if (out_it<out_it->start_same_saC)
      {
        out_it=out_it->start_same_saC;
      }
      ++out_it;
      outgoing_transitions_const_it const
                out_it_end=std::next(src.ref_state)>=m_states.end()
                        ? m_outgoing_transitions.end()
                        : std::next(src.ref_state)->start_outgoing_transitions;
      if (out_it<out_it_end)
      {
        return m_transitions[*out_it->ref.BLC_transitions].
                                        transitions_per_block_to_constellation;
      }
      else
      {
        return linked_list<BLC_indicators>::end();
      }
    }

    /// \brief split a block (using main and co-splitter) into up to four subblocks
    /// \details The function takes one or two splitter BLC sets (a main
    /// splitter and a co-splitter, both with source block `bi`) and refines
    /// the bottom states in their source block into a sublock that can reach
    /// all splitters provided ("ReachAlw"), a subblock that avoids the main
    /// splitter ("MissMain") and a subblock that avoids the co-splitter
    /// ("XcludeCo").  The non-bottom states that can inertly reach only one
    /// subblock of bottom states are then added to that subblock.  This is
    /// achieved by going through the block-inert predecessors of every state
    /// that is known to be in one of these three subblocks and counting how
    /// many go to one subblock; when all block-inert successors of a certain
    /// state in block `bi` are determined to be in the same subblock, that
    /// state is also added to it.  Otherwise -- when the block-inert
    /// successors of a certain state are in different subblocks --, that state
    /// is moved to a fourth subblock ("MultiSub").  This fourth subblock will
    /// contain new bottom states and will later need to be stabilized under
    /// all outgoing BLC sets.
    ///
    /// The main splitter is assumed to be _small_ (mostly because its target
    /// states are in the small new constellation; occasionally because the
    /// source block is in the small new constellation) and therefore can be
    /// read completely before searching block-inert predecessors of states
    /// that are already known to be in one of the subblocks.
    /// The co-splitter (if given) is not assumed to be small, but it is known
    /// that the main splitter has just been separated from it.  Therefore it
    /// is possible to determine quickly for states with a transition in the
    /// main splitter whether they also have a transition in the co-splitter
    /// using the `start_same_saC` pointer of the source state of a transition
    /// in the main splitter; that is why bottom states can quickly refined
    /// into the three parts.  For potential non-bottom states in XcludeCo, one
    /// has to go through all their non-inert outgoing transitions to determine
    /// whether any of them are in the co-splitter.  If yes, the state that was
    /// potentially in XcludeCo gets to be in MultiSub instead (because it can
    /// inertly reach both the main and the co-splitter [ReachAlw], and it can
    /// inertly reach XcludeCo).
    /// To ensure that the search through block-inert predecessors is quick, it
    /// is broken off after three subblocks have been completed; all remaining
    /// states then must be in the unfinished subblock.  In this way, every
    /// action during the search for block-inert predecessors can be assigned
    /// to a _small_ subblock: either to a state in it, or an incoming or an
    /// outgoing transition.
    ///
    /// If only the main splitter is given, XcludeCo is empty.
    ///
    /// For new bottom splits, one only gives the co-splitter because one can
    /// only look through the transitions from _new bottom_ states before
    /// starting the search for the smallest subblocks.  Then MissMain is
    /// empty.  The mechanism to decide when to handle a transition of the
    /// co-splitter is: we require that every bottom state with a transition in
    /// the co-splitter has a marked transition.  Otherwise this function
    /// ignores marked transitions completely.
    ///
    /// If initialisation==true, the parameters beyond `bi` are ignored; the
    /// information on how to initialize the refinement is taken from `*bi`,
    /// which contains a pointer to the first unmarked bottom state and a
    /// vector of marked non-bottom states.  The marked bottom states go into
    /// ReachAlw, the marked non-bottom states into potentially-ReachAlw, and
    /// then `four_way_splitB()` continues as with an empty co-splitter.
    ///
    /// \param bi                 index of the block being split
    /// \param main_splitter      small BLC set under which the block needs to be stabilized
    /// \param co-splitter        (possibly large) BLC set under which the block needs to be stabilized
    /// \param old_constellation  target constellation of all co-splitters, used to keep other main and co-splitters together
    /// \returns block index of the ReachAlw subblock if it exists; or `null_block` if ReachAlw is empty
    template <bool has_main_splitter, bool has_co_splitter>
    block_type* four_way_splitB(block_type* const bi,
                     linked_list<BLC_indicators>::iterator const main_splitter,
                     linked_list<BLC_indicators>::iterator const co_splitter,
                     constellation_type* const old_constellation,
                     constellation_type* const new_constellation)
    {
//std::cerr << "four_way_splitB(main splitter=="
//<< (linked_list<BLC_indicators>::end()==main_splitter ? std::string("none") : main_splitter->debug_id(*this))
//<< ", co-splitter=="
//<< (linked_list<BLC_indicators>::end()==co_splitter ? std::string("none") : co_splitter->debug_id(*this))
//<< ", old_constellation==" << old_constellation << ")\n";
                                                                                assert(1<number_of_states_in_block(*bi));
                                                                                assert(!bi->contains_new_bottom_states);
                                                                                assert(has_main_splitter==(linked_list<BLC_indicators>::end()!=main_splitter));
                                                                                assert(has_co_splitter  ==(linked_list<BLC_indicators>::end()!=  co_splitter));
      /// \brief potential non-bottom states
      /// \details These vectors contain non-bottom states that have been found
      /// when going through predecessors of a subblock.
      ///
      /// The variable is declared `static` to avoid repeated deallocations and
      /// reallocations while the algorithm runs many refinements.
      ///
      /// We do not need a separate array for XcludeCo_and_Hit_Main states;
      /// they can share with the XcludeCo states.
      static std::vector<state_in_block_pointer>potential_non_bottom_states[3]; assert(potential_non_bottom_states[ReachAlw].empty());
                                                                                assert(potential_non_bottom_states[MissMain].empty());
                                                                                assert(potential_non_bottom_states[XcludeCo].empty());
      static std::vector<state_in_block_pointer>
                                          potential_non_bottom_states_Hit_Main; assert(potential_non_bottom_states_Hit_Main.empty());
      #ifdef USE_INCLUDE_CO
        static std::vector<state_in_block_pointer>
                                          potential_non_bottom_states_NcludeCo; assert(potential_non_bottom_states_NcludeCo.empty());
      #endif

      /// \brief proven non-bottom states
      /// \details These vectors contain all non-bottom states of which the
      /// procedure has proven that they are in the respective subblock, unless
      /// the corresponding coroutine has been aborted; all their block-inert
      /// successors are already in the subblock.
      ///
      /// The variable is declared `static` to avoid repeated deallocations and
      /// reallocations while the algorithm runs many refinements.
      ///
      /// Again, we do not need a separate array for the XcludeCo_and_Hit_Main
      /// states; this information is not needed once it's been determined that
      /// a state is in XcludeCo.
      ///
      /// The fourth entry in this array is for MultiSub; it should be in the
      /// same array to allow to find the three other arrays with coroutine^1,
      /// coroutine^2 and coroutine^3.
      static todo_state_vector non_bottom_states[4];

      #define non_bottom_states_MultiSub non_bottom_states[3]

      // Non-bottom states have a `counter` field that indicates their subblock
      // status: the field contains the sum of a base value, that indicates
      // which subblock they are (potentially) in, and a counter that indicates
      // how many block-inert successors still neeed to be checked.

      /// \brief distribution of bottom states
      /// \details Bottom states are distributed over the subblocks by placing
      /// them in a specific slice of the bottom states of block `bi`: at the
      /// beginning there will be ReachAlw-bottom states, then XcludeCo-bottom
      /// states and at the end MissMain-bottom states.  The iterators indicate
      /// the place where every slice starts; at the same time, this is the end
      /// of the previous slice.
      state_in_block_pointer* start_bottom_states[4];                           assert(non_bottom_states[ReachAlw].empty());
      start_bottom_states[ReachAlw]=bi->start_bottom_states;                    assert(non_bottom_states[MissMain].empty());
      start_bottom_states[MissMain]=bi->start_bottom_states;                    assert(non_bottom_states[XcludeCo].empty());
      start_bottom_states[XcludeCo+1]=bi->sta.rt_non_bottom_states;             assert(non_bottom_states_MultiSub.empty());
      #define bottom_size(coroutine) (                                          assert(ReachAlw==(coroutine)||MissMain==(coroutine)||XcludeCo==(coroutine)),  \
                                                                                assert(start_bottom_states[(coroutine)]<=start_bottom_states[(coroutine)+1]), \
              static_cast<state_type>                                         \
                         (std::distance(start_bottom_states[(coroutine)],     \
                                        start_bottom_states[(coroutine)+1])))
      #define bottom_and_non_bottom_size(coroutine) (                           assert(aborted!=status[(coroutine)]), \
              bottom_size((coroutine))+non_bottom_states[(coroutine)].size())

      /// \brief next unhandled co-splitter transition
      /// \details MultiSub may go through the co-splitter transitions at some
      /// point of the algorithm; this iterator is used to store which
      /// transition MultiSub will handle next.  (The variable is already
      /// declared here just for initialisation.)
      BLC_list_iterator co_splitter_iter_MultiSub;
      BLC_list_const_iterator co_splitter_iter_end_MultiSub;

      if (has_main_splitter /* needed for correctness */)
      {
        // by default states are in MissMain:
        start_bottom_states[XcludeCo]=bi->sta.rt_non_bottom_states;             assert(main_splitter->is_stable());
                                                                                #ifndef NDEBUG
                                                                                  const transition&
                                                                                                main_t=m_aut.get_transitions()[*main_splitter->start_same_BLC];
                                                                                  assert(bi==m_states[main_t.from()].block);
                                                                                  #define debug_coroutine_of_ReachAlw "ReachAlw"
                                                                                  #define debug_coroutine_of_MissMain "MissMain"
                                                                                  #define debug_coroutine_of_XcludeCo "XcludeCo"
                                                                                  #define debug_coroutine_of_MultiSub "MultiSub"
                                                                                  #define debug_coroutine_of_Hit_Main "Hit_Main"
                                                                                  #ifdef XCLUDECO_AND_HIT_MAIN
        /* 1. All transitions in the main splitter are looked through.       */
        /*    For each state with a transition in the main splitter, it      */
        /*    is possible to check whether it has a transition in the        */
        /*    co-splitter or not, using the `start_same_saC` pointer.        */
        /*    We distribute the states as follows:                           */
                                                                                    #define debug_coroutine_of(coroutine) (                                   \
        /*    - bottom states with transitions in all provided splitters are */                                      assert((enum subblocks) 0<=(coroutine)), \
        /*      moved to ReachAlw                                            */                                      assert((coroutine)<(enum subblocks) 5),  \
                                                                                           &debug_coroutine_of_MultiSub "\0" debug_coroutine_of_ReachAlw "\0" \
        /*    - bottom states without a transition in the co-splitter, even  */             debug_coroutine_of_MissMain "\0" debug_coroutine_of_XcludeCo "\0" \
        /*      though it is provided, are moved to XcludeCo.  (If there is  */             debug_coroutine_of_XcludeCo "_and_" debug_coroutine_of_Hit_Main   \
        /*      no co-splitter, XcludeCo is empty.)                          */                                                        [((coroutine)+1)%5*9])
                                                                                  #else
        /*    - bottom states without a transition in the main splitter, even*/
        /*      though it is provided, are moved to MissMain.  (If there is  */
        /*      no main splitter, MissMain is empty.)                        */
                                                                                    #define debug_coroutine_of(coroutine) (                                   \
        /*    - non-bottom states with transitions to both splitters are     */        assert((enum subblocks)0<=(coroutine)&&(coroutine)<(enum subblocks)4), \
        /*      potentially-ReachAlw; we will check their block-inert        */       &debug_coroutine_of_ReachAlw "\0" debug_coroutine_of_MissMain "\0"      \
        /*      successors later to see whether they end up there or in      */        debug_coroutine_of_XcludeCo "\0" debug_coroutine_of_MultiSub           \
        /*      MultiSub instead.                                            */                                                              [9*(coroutine)])
                                                                                  #endif
        /*    - non-bottom states with a transition in the main splitter can */   assert(std::strcmp(debug_coroutine_of(ReachAlw),
        /*      end up everywhere except in MissMain, depending on their     */                      debug_coroutine_of_ReachAlw)==0);
        /*      inert successors.  We provisionally mark them Hit_Main; upon */   assert(std::strcmp(debug_coroutine_of(MissMain),
        /*      getting handled by the search for inert predecessors in the  */                      debug_coroutine_of_MissMain)==0);
        /*      coroutines, they will move to the correct subblock.          */   assert(std::strcmp(debug_coroutine_of(XcludeCo),
        /*      (If such a state is later found to be in XcludeCo, we might  */                      debug_coroutine_of_XcludeCo)==0);
        /*      use the information that it has no transition in the         */   assert(std::strcmp(debug_coroutine_of(MultiSub),
        /*      co-splitter.  Perhaps include later.)                        */                      debug_coroutine_of_MultiSub)==0);
                                                                                  #define coroutine_of_counter(ctr)                     \
        /*    The running time for this is assigned to the transitions in    */       (assert(marked((enum subblocks) 0)<=(ctr)),       \
        /*    the main splitter, which contains transitions to the new small */        assert((ctr)<=marked_MultiSub),                  \
        /*    constellation.                                                 */        ((ctr)-marked((enum subblocks) 0))/marked_range)
                                                                                  #ifdef USE_INCLUDE_CO
                                                                                    #define debug_coroutine_of_NcludeCo "NcludeCo"
                                                                                    #define debug_coroutine_of_counter(ctr)                    \
                                                                                          (marked_Hit_Main!=(ctr)                              \
                                                                                           ? marked_NcludeCo!=(ctr)                            \
                                                                                             ? debug_coroutine_of(coroutine_of_counter((ctr))) \
                                                                                             : debug_coroutine_of_NcludeCo                     \
                                                                                           : debug_coroutine_of_Hit_Main)
                                                                                  #else
                                                                                    #define debug_coroutine_of_counter(ctr)                                   \
                                                                                      (marked_Hit_Main!=(ctr)?debug_coroutine_of(coroutine_of_counter((ctr))) \
                                                                                                             :debug_coroutine_of_Hit_Main)
                                                                                  #endif
                                                                                #endif
                                                                                #if !defined(NDEBUG) || defined(COUNT_WORK_BALANCE)
                                                                                  const unsigned char max_C=check_complexity::log_n-check_complexity::
                                                                                                  ilog2(number_of_states_in_constellation(*new_constellation));
                                                                                #endif
        if (bi->block.to_constellation.end()!=co_splitter)
        {                                                                       mCRL2complexity(main_splitter, add_work(check_complexity::
                                                                                          four_way_splitB__handle_transitions_in_main_splitter, max_C), *this);
                                                                                #ifndef NDEBUG
          /* This is a normal main/co-split (where `main_splitter` contains  */   const transition& co_t=m_aut.get_transitions()[*co_splitter->start_same_BLC];
          /* transitions to the _small_ new constellation and `co_splitter`  */   assert(bi==m_states[co_t.from()].block);
          /* transitions from the same block with the same label to the old  */   assert(label_or_divergence(main_t)==label_or_divergence(co_t));
          /* constellation).  None of these transitions are                  */   assert(!is_inert_during_init(main_t) ||
          /* constellation-inert.                                            */          (new_constellation!=bi->c.onstellation &&
                                                                                          old_constellation!=bi->c.onstellation));
                                                                                #endif
          co_splitter_iter_MultiSub=co_splitter->start_same_BLC;                assert(new_constellation==m_states[main_t.to()].block->c.onstellation);
          co_splitter_iter_end_MultiSub=co_splitter->end_same_BLC;              assert(old_constellation==m_states[co_t.to()].block->c.onstellation);
        }
        else
        {                                                                       assert(is_inert_during_init(main_t));
                                                                                #if !defined(NDEBUG) || defined(COUNT_WORK_BALANCE)
          /* This is a tau co-split (where `main_splitter` contains          */   if (old_constellation==bi->c.onstellation) {
          /* tau-transitions from the _small_ new constellation to the old   */     // This is still a normal split with tau-transitions
          /* constellation), or it is a tau main split of the old            */     mCRL2complexity(main_splitter, add_work(check_complexity::
          /* constellation (where `main_splitter` contains tau-transitions   */           four_way_splitB__handle_transitions_in_main_splitter, max_C), *this);
          /* from the old constellation to the _small_ new constellation,    */     assert(new_constellation==m_states[main_t.to()].block->c.onstellation);
          /* and there is no co-splitter because its transitions are still   */   } else {
          /* constellation-inert).                                           */     // This is a tau co-split
                                                                                    mCRL2complexity(main_splitter, add_work(check_complexity::
                                                                                       refine_partition_until_it_becomes_stable__prepare_cosplit,max_C),*this);
                                                                                    assert(new_constellation==bi->c.onstellation);
                                                                                    assert(number_of_states_in_block(*bi)==
                                                                                                        number_of_states_in_constellation(*new_constellation));
                                                                                    assert(old_constellation==m_states[main_t.to()].block->c.onstellation);
                                                                                  }
                                                                                #endif
          co_splitter_iter_MultiSub=m_BLC_transitions.data_end();
          co_splitter_iter_end_MultiSub=m_BLC_transitions.data_end();
        }

        // Mark all sources of transitions in the main splitter:
        BLC_list_iterator splitter_it=main_splitter->start_same_BLC;            assert(splitter_it!=main_splitter->end_same_BLC);
        do
        {                                                                       // mCRL2complexity(&m_transitions[*splitter_it], add_work(...), *this);
          state_in_block_pointer const src=m_states.begin()+                        // is subsumed in the above call
                                  m_aut.get_transitions()[*splitter_it].from();
//std::cerr << "In main splitter: " << m_transitions[*splitter_it].debug_id(*this);
          if (0==src.ref_state->no_of_outgoing_block_inert_transitions)
          {                                                                     assert(bi->start_bottom_states<=src.ref_state->ref_states_in_blocks);
            /* src is a ReachAlw-bottom state or an XcludeCo-bottom state    */ assert(src.ref_state->ref_states_in_blocks<bi->sta.rt_non_bottom_states);
            if (src.ref_state->ref_states_in_blocks<
                                                 start_bottom_states[MissMain])
            {
                                                                                #ifndef NDEBUG
//std::cerr << ", source is already in " debug_coroutine_of_ReachAlw "-bottom\n";
                                                                                  if (bi->block.to_constellation.end()!=co_splitter) {
                                                                                    assert(has_main_splitter || has_co_splitter),
              /* source state is already in ReachAlw-bottom                  */     assert(next_target_constln_in_same_saC(src, splitter_it)==co_splitter);
                                                                                  }
                                                                                #endif
            }
            else if (!has_co_splitter /* needed for correctness */)
            {
              /* state belongs to ReachAlw                                   */ assert(ReachAlw+1==MissMain);
              swap_states_in_states_in_block(start_bottom_states[MissMain],
                                          src.ref_state->ref_states_in_blocks);
              ++start_bottom_states[MissMain];
//std::cerr << ", source moved to " debug_coroutine_of_ReachAlw "-bottom (co-splitter is empty)\n";
            }
            else if (start_bottom_states[MissMain+1]<=
                                           src.ref_state->ref_states_in_blocks)
            {
                                                                                #ifndef NDEBUG
//std::cerr << ", source is already in " debug_coroutine_of_XcludeCo "-bottom\n";
                                                                                  assert(bi->block.to_constellation.end()!=co_splitter);
                                                                                  outgoing_transitions_const_it const out_it_end=std::next(src.ref_state)>=
              /* source state is already in XcludeCo-bottom                  */          m_states.end() ? m_outgoing_transitions.end()
                                                                                                        : std::next(src.ref_state)->start_outgoing_transitions;
                                                                                  for (outgoing_transitions_const_it out_it=src.ref_state->
                                                                                                      start_outgoing_transitions; out_it!=out_it_end; ++out_it)
                                                                                  {
                                                                                    assert(m_transitions[*out_it->ref.BLC_transitions].
                                                                                                          transitions_per_block_to_constellation!=co_splitter);
                                                                                  }
                                                                                #endif
            }
            else if (next_target_constln_in_same_saC(src, splitter_it)==
                                                                   co_splitter)
            {
              /* state belongs to ReachAlw                                   */ assert(ReachAlw+1==MissMain);
              swap_states_in_states_in_block(start_bottom_states[MissMain],
                                          src.ref_state->ref_states_in_blocks);
              ++start_bottom_states[MissMain];
//std::cerr << ", source moved to " debug_coroutine_of_ReachAlw "-bottom\n";
            }
            else
            {
              /* state belongs to XcludeCo                                   */ assert(MissMain+1==XcludeCo);
              --start_bottom_states[MissMain+1];
              swap_states_in_states_in_block(start_bottom_states[MissMain+1],
                                          src.ref_state->ref_states_in_blocks);
//std::cerr << ", source moved to " debug_coroutine_of_XcludeCo "-bottom\n";
            }
          }
          else
          {
            /* src has outgoing tau transitions; it might end in the         */ assert(bi->sta.rt_non_bottom_states<=src.ref_state->ref_states_in_blocks);
            /* MultiSub-subblock.                                            */ assert(src.ref_state->ref_states_in_blocks<bi->end_states);
            if (undefined==src.ref_state->counter)
            {
              if (!has_co_splitter /* needed for correctness */ ||
                next_target_constln_in_same_saC(src, splitter_it)==co_splitter)
              {
//std::cerr << ", source inserted into potentially " debug_coroutine_of_ReachAlw "-nonbottom\n";
                src.ref_state->counter=marked(ReachAlw)+
                         src.ref_state->no_of_outgoing_block_inert_transitions; assert(is_in_marked_range_of(src.ref_state->counter, ReachAlw));
                potential_non_bottom_states[ReachAlw].push_back(src);
              }
              else
              {
//std::cerr << ", source inserted into potentially " debug_coroutine_of_Hit_Main "-nonbottom\n";
                src.ref_state->counter=marked_Hit_Main;
                potential_non_bottom_states_Hit_Main.push_back(src);
                                                                                #ifndef NDEBUG
                                                                                  outgoing_transitions_const_it const out_it_end=std::next(src.ref_state)>=
                                                                                         m_states.end() ? m_outgoing_transitions.end()
                                                                                                        : std::next(src.ref_state)->start_outgoing_transitions;
                                                                                  for (outgoing_transitions_const_it out_it=src.ref_state->
                                                                                                      start_outgoing_transitions; out_it!=out_it_end; ++out_it)
                                                                                  {
                                                                                    assert(has_main_splitter || has_co_splitter),
                                                                                    assert(m_transitions[*out_it->ref.BLC_transitions].
                                                                                                          transitions_per_block_to_constellation!=co_splitter);
                                                                                  }
                                                                                #endif
              }
            }
                                                                                #ifndef NDEBUG
                                                                                  else if (marked_Hit_Main==src.ref_state->counter) {
//std::cerr << ", source is already in potentially " debug_coroutine_of_Hit_Main "-nonbottom\n";
                                                                                    assert(bi->block.to_constellation.end()!=co_splitter);
                                                                                  } else {
                                                                                    assert(is_in_marked_range_of(src.ref_state->counter, ReachAlw));
//std::cerr << ", source is already in potentially " debug_coroutine_of_ReachAlw "-nonbottom\n";
                                                                                    if (bi->block.to_constellation.end()!=co_splitter) {
                                                                                      assert(has_main_splitter || has_co_splitter),
                                                                                      assert(next_target_constln_in_same_saC(src, splitter_it)==co_splitter);
                                                                                    }
                                                                                  }
                                                                                #endif
          }
          ++splitter_it;
        }
        while (splitter_it!=main_splitter->end_same_BLC);
      }
      else if (has_co_splitter /* needed for correctness */)
      {                                                                         assert(bi->block.to_constellation.end()!=co_splitter);
        /* This is a bottom state split.  We can only go through the         */ assert(!co_splitter->is_stable());
        /* transitions from (new) bottom states in the co-splitter before    */ assert(null_constellation==old_constellation);
        // starting the coroutines.

        // MissMain is empty; XcludeCo is the default subblock for bottom
        // states without a transition in the co-splitter.  But we will set
        // `start_bottom_states[XcludeCo]` only after the for loop below.

        co_splitter_iter_MultiSub=co_splitter->start_same_BLC;
        co_splitter_iter_end_MultiSub=co_splitter->start_marked_BLC;

        for (BLC_list_iterator co_splitter_it=co_splitter->start_marked_BLC;
                   co_splitter_it!=co_splitter->end_same_BLC; ++co_splitter_it)
        {                                                                       // We can assign the work to the marked transition and do not need a counter
          state_in_block_pointer const src=m_states.begin()+                    // for this loop.
                               m_aut.get_transitions()[*co_splitter_it].from(); assert(0==src.ref_state->no_of_outgoing_block_inert_transitions);
//std::cerr << "In co-splitter: " << m_transitions[*co_splitter_it].debug_id(*this);
                                                                                assert(bi->start_bottom_states<=src.ref_state->ref_states_in_blocks);
          /* src is a ReachAlw-bottom state                                  */ assert(src.ref_state->ref_states_in_blocks<bi->sta.rt_non_bottom_states);
          if (start_bottom_states[MissMain]<=
                                           src.ref_state->ref_states_in_blocks)
          {
            swap_states_in_states_in_block(start_bottom_states[MissMain],
                                          src.ref_state->ref_states_in_blocks);
//std::cerr << ", source moved to " debug_coroutine_of_ReachAlw "-bottom\n";
            ++start_bottom_states[MissMain];
          }
//else { std::cerr << ", source is already in " debug_coroutine_of_ReachAlw "-bottom\n"; }
        }
        start_bottom_states[XcludeCo]=start_bottom_states[MissMain];
        make_stable_and_move_to_start_of_BLC(bi, co_splitter);
      }
      else
      {                                                                         assert(linked_list<BLC_indicators>::end()==main_splitter);
        // This is a refinement during initialisation

        start_bottom_states[MissMain]=bi->c.first_unmarked_bottom_state;        assert(linked_list<BLC_indicators>::end()==co_splitter);
        start_bottom_states[MissMain+1]=bi->sta.rt_non_bottom_states;           assert(nullptr!=bi->block.R);
        potential_non_bottom_states[ReachAlw].swap(*bi->block.R);               assert(null_constellation==old_constellation);

        // reset the information in bi so it looks like a normal block:
        delete bi->block.R;
        bi->block.R=nullptr;                                                    assert(null_constellation!=new_constellation);
        // destroy bi->c.first_unmarked_bottom_state; -- trivial
        new (&bi->c) block_type::
               constellation_or_first_unmarked_bottom_state(new_constellation);

        for (state_in_block_pointer st: potential_non_bottom_states[ReachAlw])
        {                                                                       assert(0<st.ref_state->no_of_outgoing_block_inert_transitions);
          st.ref_state->counter=marked(ReachAlw)+
                          st.ref_state->no_of_outgoing_block_inert_transitions; assert(is_in_marked_range_of(st.ref_state->counter, ReachAlw));
        }
        co_splitter_iter_MultiSub=m_BLC_transitions.data_end();
        co_splitter_iter_end_MultiSub=m_BLC_transitions.data_end();
      }

      /* 2. If the block does not contain non-bottom states, all states have */ assert(bi->start_bottom_states==start_bottom_states[ReachAlw]);
      /*    been distributed.  Finalize the refinement and return.  (There   */ assert(start_bottom_states[ReachAlw]<=start_bottom_states[MissMain]);
      /*    may be up to three subblocks, namely ReachAlw/MissMain/XcludeCo. */ assert(start_bottom_states[MissMain]<=start_bottom_states[XcludeCo]);
      /*    Pick the first and the last subblock and split off the smaller   */ assert(start_bottom_states[XcludeCo]<=start_bottom_states[XcludeCo+1]);
      /*    of the two.  Then compare the remaining two subblocks and again  */ assert(start_bottom_states[XcludeCo+1]==bi->sta.rt_non_bottom_states);
      //    split off the smaller one.)
      if (bi->sta.rt_non_bottom_states==bi->end_states)
      {
//std::cerr << "All bottom states, split decisions finished\n";
        block_type* ReachAlw_block_index=null_block;
        constellation_type* const constellation=bi->c.onstellation;
        bool constellation_was_trivial=
                  constellation->start_const_states->ref_state->block==
                  std::prev(constellation->end_const_states)->ref_state->block;
        bool constellation_becomes_nontrivial=false;                            assert(has_co_splitter || 0==bottom_size(XcludeCo));
        if (has_co_splitter && bottom_size(ReachAlw)<bottom_size(XcludeCo))
        {                                                                       assert(bi->start_bottom_states==start_bottom_states[ReachAlw]);
          if (0<bottom_size(ReachAlw))
          {
            bi->start_bottom_states=start_bottom_states[ReachAlw+1];
            ReachAlw_block_index=create_new_block
                      <!has_main_splitter && !has_co_splitter>
                      (start_bottom_states[ReachAlw],
                       start_bottom_states[ReachAlw+1],
                       start_bottom_states[ReachAlw+1], bi,
                       old_constellation, new_constellation);
            constellation_becomes_nontrivial=true;
          }                                                                     assert(has_main_splitter || 0==bottom_size(MissMain));
          if(!has_main_splitter || bottom_size(MissMain)<bottom_size(XcludeCo))
          {                                                                     assert(bi->start_bottom_states==start_bottom_states[MissMain]);
            if (has_main_splitter && 0<bottom_size(MissMain))
            {
              bi->start_bottom_states=start_bottom_states[MissMain+1];
              create_new_block<!has_main_splitter && !has_co_splitter>
                      (start_bottom_states[MissMain],
                       start_bottom_states[MissMain+1],
                       start_bottom_states[MissMain+1], bi,
                       old_constellation, new_constellation);
              constellation_becomes_nontrivial=true;
            }
          }
          else if (0<bottom_size(XcludeCo))
          {                                                                     assert(bi->end_states==start_bottom_states[XcludeCo+1]);
            bi->sta.rt_non_bottom_states=start_bottom_states[XcludeCo];
            bi->end_states=start_bottom_states[XcludeCo];
            create_new_block<!has_main_splitter && !has_co_splitter>
                      (start_bottom_states[XcludeCo],
                       start_bottom_states[XcludeCo+1],
                       start_bottom_states[XcludeCo+1], bi,
                       old_constellation, new_constellation);
            constellation_becomes_nontrivial=true;
          }
        }
        else
        {                                                                       assert(bi->end_states==start_bottom_states[XcludeCo+1]);
          if (has_co_splitter && 0<bottom_size(XcludeCo))
          {
            bi->sta.rt_non_bottom_states=start_bottom_states[XcludeCo];
            bi->end_states=start_bottom_states[XcludeCo];
            create_new_block<!has_main_splitter && !has_co_splitter>
                      (start_bottom_states[XcludeCo],
                       start_bottom_states[XcludeCo+1],
                       start_bottom_states[XcludeCo+1], bi,
                       old_constellation, new_constellation);
            constellation_becomes_nontrivial=true;
          }                                                                     assert((has_main_splitter || !has_co_splitter) || 0==bottom_size(MissMain));
          if ((has_main_splitter || !has_co_splitter) &&
              bottom_size(ReachAlw)<bottom_size(MissMain))
          {                                                                     assert(bi->start_bottom_states==start_bottom_states[ReachAlw]);
            if (0<bottom_size(ReachAlw))
            {
              bi->start_bottom_states=start_bottom_states[ReachAlw+1];
              ReachAlw_block_index=create_new_block
                      <!has_main_splitter && !has_co_splitter>
                      (start_bottom_states[ReachAlw],
                       start_bottom_states[ReachAlw+1],
                       start_bottom_states[ReachAlw+1], bi,
                       old_constellation, new_constellation);
              constellation_becomes_nontrivial=true;
            }
          }
          else
          {
            ReachAlw_block_index=bi;
            if ((has_main_splitter || !has_co_splitter) &&
                0<bottom_size(MissMain))
            {                                                                   assert(bi->end_states==start_bottom_states[MissMain+1]);
              bi->sta.rt_non_bottom_states=start_bottom_states[MissMain];
              bi->end_states=start_bottom_states[MissMain];
              create_new_block<!has_main_splitter && !has_co_splitter>
                      (start_bottom_states[MissMain],
                       start_bottom_states[MissMain+1],
                       start_bottom_states[MissMain+1], bi,
                       old_constellation, new_constellation);
              constellation_becomes_nontrivial=true;
            }
          }
        }

        if (constellation_becomes_nontrivial && constellation_was_trivial)
        {                                                                       assert(std::find(m_non_trivial_constellations.begin(),
          /* This constellation was trivial, as it will be split add it to   */                  m_non_trivial_constellations.end(),
          /* the non-trivial constellations.                                 */                  constellation)==m_non_trivial_constellations.end());
          m_non_trivial_constellations.emplace_back(constellation);
        }
        return ReachAlw_block_index;
      }                                                                         assert(m_branching);

      // 3. We distinguish situations where some of these subblocks are empty:
      //    - If there are no MissMain-bottom states, then MissMain will be
      //      empty.
      //    - It may also happen that there are no XcludeCo-bottom states but
      //      there are ReachAlw-bottom states because every bottom state with
      //      a transition in the main splitter also has a transition in the
      //      co-splitter; then it is clear from the start that XcludeCo is
      //      empty.  Potential-XcludeCo non-bottom states are in MultiSub
      //      instead.
      //    - It may be that there are no ReachAlw-bottom states but there are
      //      XcludeCo-bottom states because no bottom state with a transition
      //      in the main splitter has a transition in the co-splitter; then it
      //      is clear from the start that ReachAlw is empty.
      //      Potential-ReachAlw non-bottom states are in MultiSub instead.
      //    Empty subblocks are considered finished.

      // 4. We decide whether one of the subblocks is already too large (more
      //    than 50% of the unfinished states); if yes, this subblock is
      //    immediately aborted.  At most one subblock can be aborted at any
      //    time.  The aborted subblock is *not* considered finished.
      /*    (We use variable `no_of_unfinished_states_in_block` to record the*/ assert(non_bottom_states[ReachAlw].empty());
      /*    number of unfinished states as long as there is no aborted       */ assert(non_bottom_states[MissMain].empty());
      /*    subblock; as soon as a subblock is aborted, it is set to the     */ assert(non_bottom_states[XcludeCo].empty());
      /*    largest possible value to avoid aborting another subblock.)      */ assert(non_bottom_states_MultiSub.empty());

      enum { state_checking,
             incoming_inert_transition_checking,
             outgoing_constellation_checking,
             aborted, finished } status[3], status_MultiSub;
      state_in_block_pointer* current_bottom_state_iter[3];

      // the number of states in the block that are not yet in finished
      // subblocks; but if some process has been aborted already, it is equal
      // to `std::numeric_limits<state_index>::max()`:
      state_index no_of_unfinished_states_in_block=
                                                number_of_states_in_block(*bi);

      /// \brief Abort if there are too many bottom states in a subblock, used before the coroutines start
      /// \details This macro applies to ReachAlw, MissMain, or XcludeCo.
      ///
      /// If the bottom states alone already cover more than half of
      /// a block, the corresponding coroutine does not need to start.
      #define abort_if_bottom_size_too_large(coroutine)                                                                                        \
          ((                                                                    assert(non_bottom_states[(coroutine)].empty()),                \
            bottom_size((coroutine))>no_of_unfinished_states_in_block/2) &&                                                                    \
           (                                                                    assert(std::numeric_limits<state_index>::max()!=               \
                                                                                                            no_of_unfinished_states_in_block), \
            no_of_unfinished_states_in_block=                                                                                                  \
                                       std::numeric_limits<state_index>::max(), assert(m_aut.num_states()<no_of_unfinished_states_in_block/2), \
            status[(coroutine)]=aborted,                                                                                                       \
            true))

      /// \brief Abort if there are too many states in subblock MultiSub
      /// \details: If the states, possibly after adding i additional states,
      /// cover more than half of the states in the unfinished subblocks,
      /// MultiSub can be aborted.  The parameter i allows to apply the test
      /// even before adding a state, to avoid storing data that is immediately
      /// going to be abolished.
      ///
      /// MultiSub has only non-bottom states, so we need a macro that
      /// is different from the other subblocks.
      ///
      /// This macro can be used before the coroutines start or while they run.
      #define abort_if_non_bottom_size_too_large_MultiSub(i)                                                                                   \
          ((                                                                    assert(aborted!=status_MultiSub),                              \
            non_bottom_states_MultiSub.size()+(i+0)>                                                                                           \
                                         no_of_unfinished_states_in_block/2) &&                                                                \
           (                                                                    assert(std::numeric_limits<state_index>::max()!=               \
                                                                                                            no_of_unfinished_states_in_block), \
            no_of_unfinished_states_in_block=                                                                                                  \
                                       std::numeric_limits<state_index>::max(), assert(m_aut.num_states()<no_of_unfinished_states_in_block/2), \
            status_MultiSub=aborted,                                                                                                           \
            true))

      /// \brief Abort if there are too many states in a subblock
      /// \details: If the states, possibly after adding i additional states,
      /// cover more than half of the states in the unfinished subblocks, the
      /// coroutine can be aborted.  The parameter i allows to apply the test
      /// even before adding a state, to avoid storing data that is immediately
      /// going to be abolished.
      ///
      /// If the coroutine is aborted, its non-bottom state vector is
      /// immediately cleared, as it is of no use any more.  (Marked counters
      /// can be found through `potential_non_bottom_states`.)
      ///
      /// This macro can be used while the coroutines run.
      #define abort_if_size_too_large(coroutine, i)                                                                                            \
          (bottom_and_non_bottom_size((coroutine))+(i+0)>                                                                                      \
                                          no_of_unfinished_states_in_block/2 &&                                                                \
           (                                                                    assert(std::numeric_limits<state_index>::max()!=               \
                                                                                                            no_of_unfinished_states_in_block), \
            no_of_unfinished_states_in_block=                                                                                                  \
                                       std::numeric_limits<state_index>::max(), assert(m_aut.num_states()<no_of_unfinished_states_in_block/2), \
            status[(coroutine)]=aborted,                                                                                                       \
            non_bottom_states[(coroutine)].clear(),                                                                                             \
            true))

      int no_of_finished_searches=0;      // including the MultiSub-search
      int no_of_running_searches=0;       // does not include the MultiSub-search
      enum subblocks running_searches[3]; // does not include the MultiSub-search

      if ((!has_main_splitter && has_co_splitter) || 0==bottom_size(MissMain))
      {
        /* MissMain is empty and finishes early.  There are no states that   */ assert(potential_non_bottom_states[MissMain].empty());
        // might be moved to MultiSub.
//std::cerr << debug_coroutine_of_MissMain " is already finished (empty)\n";

        // if (finished==status[ReachAlw]) {
          // In principle, one would move the NcludeCo-states to MultiSub now.
          // But there are no NcludeCo-states yet (unless we would allow marked
          // non-bottom transitions in a split without main splitter).
        // }
        ++no_of_finished_searches;
        status[MissMain]=finished;
      }
      else if (!abort_if_bottom_size_too_large(MissMain))
      {
        running_searches[no_of_running_searches]=MissMain;
        ++no_of_running_searches;
        current_bottom_state_iter[MissMain]=start_bottom_states[MissMain];
        status[MissMain]=state_checking;
      }
//else std::cerr << debug_coroutine_of_MissMain " is already aborted\n";

      if (!has_co_splitter || 0==bottom_size(XcludeCo))
      {
        /* XcludeCo is empty and finishes early.                             */ assert(potential_non_bottom_states[XcludeCo].empty());
//std::cerr << debug_coroutine_of_XcludeCo " is already finished (empty)\n";
        if (finished==status[MissMain])
        {
//std::cerr << debug_coroutine_of_MultiSub " is already finished (empty) and the split is trivial.\n";
          //++no_of_finished_searches;
          //status_MultiSub=finished;
          // This is a trivial split and nothing needs to be done.
          // If XcludeCo were not yet finished, it could still happen that
          // some states are found to have a transition in the co-splitter,
          /* so they would yet be added to MultiSub.                         */ assert(0==bottom_and_non_bottom_size(MissMain));

          clear_state_counters(potential_non_bottom_states[ReachAlw].begin(),
                              potential_non_bottom_states[ReachAlw].end(), bi); assert(potential_non_bottom_states[MissMain].empty());
          clear(potential_non_bottom_states[ReachAlw]);
          if (has_main_splitter && has_co_splitter)
          {
            clear_state_counters(potential_non_bottom_states_Hit_Main.begin(),
                               potential_non_bottom_states_Hit_Main.end(), bi);
            clear(potential_non_bottom_states_Hit_Main);
          }                                                                     else  {  assert(potential_non_bottom_states_Hit_Main.empty());  }
          return bi;
        }
        ++no_of_finished_searches;
        status[XcludeCo]=finished;
      }
      else
      {
        if (!abort_if_bottom_size_too_large(XcludeCo))
        {
          running_searches[no_of_running_searches]=XcludeCo;
          ++no_of_running_searches;
          current_bottom_state_iter[XcludeCo]=start_bottom_states[XcludeCo];
          status[XcludeCo]=state_checking;
        }
//else std::cerr << debug_coroutine_of_XcludeCo " is already aborted\n";
      }

      if (0==bottom_size(ReachAlw))
      {
        // ReachAlw is empty and finishes early.  Its non-bottom states are
        // actually in MultiSub (because they can inertly reach a XcludeCo- or
        /* MissMain-bottom-state).                                           */ assert(non_bottom_states_MultiSub.empty());
        non_bottom_states_MultiSub.swap_vec
                                       (potential_non_bottom_states[ReachAlw]);
        if (!has_co_splitter || finished==status[XcludeCo])
        {
          // both ReachAlw and XcludeCo are empty.  So the Hit_Main states must
          // be in MultiSub.  (MultiSub has not yet been aborted.)
          if (has_main_splitter && has_co_splitter)
          {
            if (!non_bottom_states_MultiSub.empty())
            {
              non_bottom_states_MultiSub.add_todo
                                 (potential_non_bottom_states_Hit_Main.begin(),
                                  potential_non_bottom_states_Hit_Main.end());
              clear(potential_non_bottom_states_Hit_Main);
            }
            else
            {
              non_bottom_states_MultiSub.swap_vec
                                        (potential_non_bottom_states_Hit_Main);
            }
          }                                                                     else  {  assert(potential_non_bottom_states_Hit_Main.empty());  }
        }
        for (state_in_block_pointer st: non_bottom_states_MultiSub)
        {                                                                       // The work can be assigned to the same main splitter transition(s) that made
                                                                                // the state get into ReachAlw (depending on whether the source or target
          st.ref_state->counter=marked_MultiSub;                                // constellation are new, see above).
        }
        ++no_of_finished_searches;
        status[ReachAlw]=finished;
//std::cerr << debug_coroutine_of_ReachAlw " is already finished (empty)\n";
      }
      else if (!abort_if_bottom_size_too_large(ReachAlw))
      {
        running_searches[no_of_running_searches]=ReachAlw;
        ++no_of_running_searches;
        current_bottom_state_iter[ReachAlw]=start_bottom_states[ReachAlw];
        status[ReachAlw]=state_checking;
      }
//else std::cerr << debug_coroutine_of_ReachAlw " is already aborted\n";

      status_MultiSub=state_checking;
      abort_if_non_bottom_size_too_large_MultiSub();
//&& (std::cerr << debug_coroutine_of_MultiSub " is already aborted\n");

      // 5. We start the coroutines for the non-empty, non-aborted subblocks.
      //    Every coroutine executes one step in turn.  The coroutines stop as
      //    soon as three of them have finished (including empty subblocks).
      //    Generally the X-coroutine finds predecessors of states that are
      //    determined to be in the X-subblock and adds them first to the
      //    potentially-X states; as soon as every successor of a state is
      //    known to be in the X-subblock, the state is determined to be in the
      //    X-subblock itself.
      //    There are two twists here:
      //    - The coroutine for the R-subblock needs to check, when all
      //      successors are known to be in the R-subblock, whether the state
      //      has a transition in the co-splitter; if yes, the state is
      //      actually a new bottom state in the N-subblock (all its inert
      //      successors are in R but the state itself is in N).
      //    - Predecessors of N-states are immediately added to the N-subblock
      //      because for them, having one N-successor is enough.  There is no
      //      need to maintain a set of potentially-N states.

      std::vector<transition>::iterator current_source_iter[3],
                                                  current_source_iter_MultiSub;
      std::vector<transition>::const_iterator current_source_iter_end[3],
                                              current_source_iter_end_MultiSub;

      state_in_block_pointer current_source_XcludeCo;
      outgoing_transitions_const_it current_outgoing_iter_start_XcludeCo;
      outgoing_transitions_const_it current_outgoing_iter_XcludeCo;             assert(co_splitter_iter_MultiSub<=co_splitter_iter_end_MultiSub);
      for (;;)
      {                                                                         assert(2>=no_of_finished_searches);
        state_in_block_pointer* new_start_bottom_states_plus_one[3];
        state_in_block_pointer* new_end_bottom_states_plus_one[2];
        #define new_start_bottom_states(idx) (assert(1<=(idx)), assert((idx)<=3), new_start_bottom_states_plus_one[(idx)-1])
        #define new_end_bottom_states(idx) (assert(1<=(idx)), assert((idx)<=2), new_end_bottom_states_plus_one[(idx)-1])
        #define new_end_bottom_states_MultiSub (new_start_bottom_states_plus_one[2])
        for (int current_search_index=0; current_search_index<
                                no_of_running_searches; ++current_search_index)
        {
          const enum subblocks
                         current_search=running_searches[current_search_index]; assert(0<=current_search);  assert(current_search<MultiSub);

//std::cerr << "Doing one step for coroutine " << debug_coroutine_of(current_search) << ": ";
          if (incoming_inert_transition_checking==status[current_search])
          {                                                                     assert(current_source_iter[current_search]<
                                                                                                                      current_source_iter_end[current_search]);
//std::cerr << "incoming_inert_transition " << m_transitions[std::distance(m_aut.get_transitions().begin(), current_source_iter[current_search])].debug_id(*this);
                                                                                mCRL2complexity(&m_transitions[std::distance(m_aut.get_transitions().begin(),
                                                                                        current_source_iter[current_search])], add_work(check_complexity::
                                                                                                     simple_splitB_U__handle_transition_to_U_state, 1), *this);
            const transition& tr=*current_source_iter[current_search]++;        assert(m_aut.is_tau(m_aut_apply_hidden_label_map(tr.label())));
            state_in_block_pointer const src=m_states.begin()+tr.from();        assert(m_states[tr.to()].block==bi);
            if (src.ref_state->block==bi &&
                !(m_preserve_divergence && tr.from()==tr.to()))
            {                                                                   assert(!non_bottom_states[ReachAlw].find(src));
                                                                                assert(!non_bottom_states[MissMain].find(src));
                                                                                assert(!non_bottom_states[XcludeCo].find(src));
              const transition_index current_counter=src.ref_state->counter;
              if(  (   (   undefined==current_counter
                        || (   has_main_splitter && has_co_splitter
                            && marked_Hit_Main==current_counter
                            && MissMain!=current_search        )
#ifdef USE_INCLUDE_CO
                        || (   has_co_splitter
                            && marked_NcludeCo==current_counter
                            && XcludeCo!=current_search        )
#endif
                                                                )
                    && (src.ref_state->counter=marked(
                                        #ifdef XCLUDECO_AND_HIT_MAIN
                                          has_main_splitter&&has_co_splitter &&
                                          marked_Hit_Main==current_counter &&
                                          XcludeCo==current_search
                                          ? XcludeCo_and_Hit_Main :
                                        #endif
                                        current_search)+src.ref_state->
                                     no_of_outgoing_block_inert_transitions,    assert(std::find(potential_non_bottom_states[current_search].begin(),
                                                                                                 potential_non_bottom_states[current_search].end(), src)==
                                                                                                 potential_non_bottom_states[current_search].end()),
                        potential_non_bottom_states[current_search].
                                                             push_back(src),
//(std::cerr << ": source added to potential-" << debug_coroutine_of(current_search) << "-nonbottom"),
                        true                                                ))
                 || is_in_marked_range_of(current_counter, current_search)    )
              {                                                                 assert(is_in_marked_range_of(src.ref_state->counter, current_search));
                --src.ref_state->counter;                                       assert(is_in_marked_range_of(src.ref_state->counter, current_search));
                                                                                assert(!non_bottom_states_MultiSub.find(src));
                if (marked((enum subblocks) 0)==
                                         src.ref_state->counter % marked_range)
                {
                  // all inert transitions of src point to the current
                  // subblock
                  if (has_co_splitter &&
                      marked(XcludeCo)==src.ref_state->counter &&
                      #ifndef XCLUDECO_AND_HIT_MAIN
                        (!has_main_splitter ||
                         marked_Hit_Main!=current_counter) && // this test seems to shave off 1-2% of the coroutine steps
                      #endif
                      co_splitter_iter_MultiSub!=co_splitter_iter_end_MultiSub)
                  {                                                             assert(bi->block.to_constellation.end()!=co_splitter);
//std::cerr << ", going to check outgoing transitions of potential " debug_coroutine_of_XcludeCo "-state\n";
                    // but XcludeCo needs to check whether src has a transition
                    // in the co-splitter
                    // (This can be avoided if we remember that the state had
                    // been in Hit_Main earlier; then we know that it had a
                    // transition in the main splitter but none in the
                    // co-splitter.)
                    current_source_XcludeCo=src;
                    status[XcludeCo]=outgoing_constellation_checking;
                    current_outgoing_iter_start_XcludeCo=
                                     src.ref_state->start_outgoing_transitions;
                    current_outgoing_iter_XcludeCo=
                      std::next(src.ref_state)>=m_states.end()
                        ? m_outgoing_transitions.end()
                        : std::next(src.ref_state)->start_outgoing_transitions; assert(current_outgoing_iter_start_XcludeCo<current_outgoing_iter_XcludeCo);
                    continue;
                  }
//std::cerr << ": source added to " << debug_coroutine_of(current_search);
                  if (abort_if_size_too_large(current_search, 1))
                  {                                                             assert(running_searches[current_search_index]==current_search);
                    --no_of_running_searches;                                   assert(current_search_index<=no_of_running_searches);
                    running_searches[current_search_index]=
                                      running_searches[no_of_running_searches]; assert(std::find(potential_non_bottom_states[current_search].begin(),
                                                                                                 potential_non_bottom_states[current_search].end(), src)!=
                                                                                                 potential_non_bottom_states[current_search].end());
                    --current_search_index;
//std::cerr << " and aborted\n";
                    continue;
                  }
                  #ifdef XCLUDECO_AND_HIT_MAIN
                    if (has_main_splitter && has_co_splitter)
                    {
                      src.ref_state->counter=marked(current_search);
                    }
                  #endif
                  non_bottom_states[current_search].add_todo(src);
                }
              }
              else if (marked_MultiSub!=src.ref_state->counter)
              {
//std::cerr << ": source is already potentially-" << debug_coroutine_of_counter(src.ref_state->counter) << " and is moved to " debug_coroutine_of_MultiSub;
                // The state has block-inert transitions to multiple
                // subblocks (or it is Hit_Main and the current search is
                // MissMain).  It should be added to MultiSub.
                src.ref_state->counter=marked_MultiSub;                         assert(!non_bottom_states_MultiSub.find(src));
                if (aborted!=status_MultiSub &&
                    !abort_if_non_bottom_size_too_large_MultiSub(1))
                {
                  non_bottom_states_MultiSub.add_todo(src);
                }
//else { std::cerr << ", which is or was aborted"; }
              }                                                                 else {
//std::cerr << ": source is already in " debug_coroutine_of_MultiSub;
                                                                                  assert(non_bottom_states_MultiSub.find(src));
                                                                                }
            }
//else { std::cerr << " is not block-inert"; }
//std::cerr << '\n';
            if (current_source_iter[current_search]!=
                                     current_source_iter_end[current_search] &&
                m_aut.is_tau(m_aut_apply_hidden_label_map
                               (current_source_iter[current_search]->label())))
            {
              continue;
            }
            status[current_search]=state_checking;
          }
          else if (!has_co_splitter || state_checking==status[current_search])
          {                                                                     assert(state_checking==status[current_search]);
            state_in_block_pointer const tgt=
                    current_bottom_state_iter[current_search]<
                                          start_bottom_states[current_search+1]
                          ? *current_bottom_state_iter[current_search]++
                          : non_bottom_states[current_search].move_from_todo(); assert(!non_bottom_states[current_search^1].find(tgt));

//std::cerr << "state_checking " << tgt.ref_state->debug_id(*this) << '\n';
            /* Prepare for the sources of tgt to be added to the subblock    */ mCRL2complexity(tgt.ref_state,
                                                                                     add_work(check_complexity::simple_splitB_U__find_predecessors, 1), *this);
            current_source_iter[current_search]=
                                     tgt.ref_state->start_incoming_transitions; assert(!non_bottom_states[current_search^2].find(tgt));
            current_source_iter_end[current_search]=
                  std::next(tgt.ref_state)>=m_states.end()
                        ? m_aut.get_transitions().end()
                        : std::next(tgt.ref_state)->start_incoming_transitions; assert(!non_bottom_states[current_search^3].find(tgt));
            if (current_source_iter[current_search]<
                                     current_source_iter_end[current_search] &&
                m_aut.is_tau(m_aut_apply_hidden_label_map
                               (current_source_iter[current_search]->label())))
            {
              status[current_search]=incoming_inert_transition_checking;
              continue;
            }
          }
          else
          {                                                                     assert(XcludeCo==current_search);
                                                                                assert(outgoing_constellation_checking==status[XcludeCo]);
            --current_outgoing_iter_XcludeCo;                                   assert(current_outgoing_iter_start_XcludeCo<=
                                                                                                               current_outgoing_iter_XcludeCo->start_same_saC);
//std::cerr << "outgoing_constellation_checking " << m_transitions[assert(has_main_splitter || has_co_splitter), *current_outgoing_iter_XcludeCo->ref.BLC_transitions].debug_id(*this);
                                                                                assert(current_outgoing_iter_XcludeCo->start_same_saC<=
                                                                                                                                current_outgoing_iter_XcludeCo);
                                                                                #if !defined(NDEBUG) || defined(COUNT_WORK_BALANCE)
                                                                                  // Assign the work to the transitions in the same_saC slice
                                                                                  outgoing_transitions_const_it out_it=
                                                                                                                current_outgoing_iter_XcludeCo->start_same_saC;
                                                                                  assert(has_main_splitter || has_co_splitter);
                                                                                  mCRL2complexity(&m_transitions[*out_it->ref.BLC_transitions],
                                                                                        add_work(check_complexity::
                                                                                         simple_splitB_U__handle_transition_from_potential_U_state, 1), *this);
                                                                                  #ifndef NDEBUG
                                                                                    while (++out_it<=current_outgoing_iter_XcludeCo) {
                                                                                      assert(has_main_splitter || has_co_splitter);
                                                                                      mCRL2complexity(&m_transitions[*out_it->ref.BLC_transitions],
                                                                                         add_work_notemporary(check_complexity::
                                                                                         simple_splitB_U__handle_transition_from_potential_U_state, 1), *this);
                                                                                    }
                                                                                  #endif
                                                                                #endif
                                                                                assert(!non_bottom_states[ReachAlw].find(current_source_XcludeCo));
                                                                                assert(!non_bottom_states[XcludeCo].find(current_source_XcludeCo));
                                                                                assert(!non_bottom_states[MissMain].find(current_source_XcludeCo));
                                                                                assert(marked(XcludeCo)==current_source_XcludeCo.ref_state->counter ||
                                                                                       marked_MultiSub==current_source_XcludeCo.ref_state->counter);
            linked_list<BLC_indicators>::const_iterator const current_splitter=
                m_transitions[                                                  assert(has_main_splitter || has_co_splitter),
                      *current_outgoing_iter_XcludeCo->ref.BLC_transitions].
                                        transitions_per_block_to_constellation; assert(bi->block.to_constellation.end()!=co_splitter);
            if (current_splitter==co_splitter)
            {
//std::cerr << ": source belongs to " debug_coroutine_of_MultiSub;
              /* The state has a transition in the co-splitter, so it should */
              /* not be added to XcludeCo.  Instead, add it to MultiSub:     */
              if (marked_MultiSub!=current_source_XcludeCo.ref_state->counter)
              {
                // It doesn't happen often that the source is marked MultiSub
                // exactly while XcludeCo is running this search -- so we do
                // not test this very often.
                current_source_XcludeCo.ref_state->counter=marked_MultiSub;     assert(!non_bottom_states_MultiSub.find(current_source_XcludeCo));
                if (aborted!=status_MultiSub &&
                    !abort_if_non_bottom_size_too_large_MultiSub(1))
                {                                                               assert(aborted!=status_MultiSub);
                  non_bottom_states_MultiSub.add_todo(current_source_XcludeCo);
                }
//else { std::cerr << ", which is or was aborted"; }
              }                                                                 else  assert(non_bottom_states_MultiSub.find(current_source_XcludeCo));
//std::cerr << '\n';
            }
            else if ((has_main_splitter && current_splitter==main_splitter      && (assert(std::find(potential_non_bottom_states_Hit_Main.begin(),
                                                                                         potential_non_bottom_states_Hit_Main.end(), current_source_XcludeCo)!=
                                                                                                             potential_non_bottom_states_Hit_Main.end()),
//std::cerr << ": is in main splitter, so there was no transition in the co-splitter, source belongs to " debug_coroutine_of_XcludeCo,
                                                                                                                                                          true)
                                                                          ) ||
                     (current_outgoing_iter_XcludeCo=
                                current_outgoing_iter_XcludeCo->start_same_saC,
                      current_outgoing_iter_start_XcludeCo==
                                               current_outgoing_iter_XcludeCo   && (assert(std::find(potential_non_bottom_states_Hit_Main.begin(),
                                                                                         potential_non_bottom_states_Hit_Main.end(), current_source_XcludeCo)==
                                                                                                             potential_non_bottom_states_Hit_Main.end()),
//std::cerr << ": no transitions in co-splitter found, source belongs to " debug_coroutine_of_XcludeCo,
                                                                                                                                                          true)
                                                                             ))
            {                                                                   assert(marked(XcludeCo)==current_source_XcludeCo.ref_state->counter);
              // Transitions to the co-splitter must come after transitions
              // to the main splitter; so we can break off the search early
              // when we see the main splitter (because if it had also a
              // transition in the co-splitter, the state would have been added
              // to Hit_Main).
              if (abort_if_size_too_large(XcludeCo, 1))
              {                                                                 assert(running_searches[current_search_index]==XcludeCo);
//std::cerr << ", which is aborted\n";
                --no_of_running_searches;                                       assert(current_search_index<=no_of_running_searches);
                running_searches[current_search_index]=
                                      running_searches[no_of_running_searches]; assert(std::find(potential_non_bottom_states[XcludeCo].begin(),
                                                                                        potential_non_bottom_states[XcludeCo].end(), current_source_XcludeCo)!=
                                                                                        potential_non_bottom_states[XcludeCo].end());
                --current_search_index;
                continue;
              }
              non_bottom_states[XcludeCo].add_todo(current_source_XcludeCo);
//std::cerr << '\n';
            }
            else
            {
//std::cerr << ": still undecided\n";
              continue;
            }
            // At this point the search for outgoing transitions has finished
            // (and XcludeCo is still running). We can go back to the previous
            // status.
            if (current_source_iter[XcludeCo]!=
                                           current_source_iter_end[XcludeCo] &&
                m_aut.is_tau(m_aut_apply_hidden_label_map
                                     (current_source_iter[XcludeCo]->label())))
            {
              status[XcludeCo]=incoming_inert_transition_checking;
              continue;
            }
            status[XcludeCo]=state_checking;
          }

          /* Now we have done one step in the handling of this subblock.  If */ assert(state_checking==status[current_search]);
          /* we reach this point, it is time to check whether the subblock is*/ assert(MultiSub!=current_search);
          // finished.
          if (current_bottom_state_iter[current_search]==
                                       start_bottom_states[current_search+1] &&
              non_bottom_states[current_search].todo_is_empty())
          {
            // the current search is completed. Finish the subblock:
            status[current_search]=finished;
//std::cerr << debug_coroutine_of(current_search) << " is finished.\n";
            ++no_of_finished_searches;
                                                                                #if !defined(NDEBUG) || defined(COUNT_WORK_BALANCE)
                                                                                  // Finalise the work distribution here:
//std::cerr << "Finalising counters for " << debug_coroutine_of(current_search) << '\n';
                                                                                  // Forget the balance of earlier processes that finished:
                                                                                  // (If MultiSub is unfinished, the third process does enough work to tilt the
                                                                                  // balance into the positive.  If another process is unfinished, then
                                                                                  // MultiSub and the last process that finished before MultiSub together
                                                                                  // should provide enough credit.)
                                                                                  check_complexity::check_temporary_work();
                                                                                  // move the work from temporary state counters to final ones
                                                                                  const unsigned char max_new_B=check_complexity::log_n-
                                                                                           check_complexity::ilog2(bottom_and_non_bottom_size(current_search));
                                                                                  for (const state_in_block_pointer* s=start_bottom_states[current_search];
                                                                                                       (s!=start_bottom_states[current_search+1] ||
                                                                                                        (s=non_bottom_states[current_search].data(), true)) &&
                                                                                                       s!=non_bottom_states[current_search].data_end(); ++s)
                                                                                  {
                                                                                    mCRL2complexity(s->ref_state, finalise_work(check_complexity::
                                                                                        simple_splitB_U__find_predecessors, check_complexity::
                                                                                          simple_splitB__find_predecessors_of_R_or_U_state, max_new_B), *this);
                                                                                    // incoming tau-transitions of s
                                                                                    const std::vector<transition>::const_iterator in_ti_end=
                                                                                        std::next(s->ref_state)>=m_states.end() ? m_aut.get_transitions().end()
                                                                                                         : std::next(s->ref_state)->start_incoming_transitions;
                                                                                    for (std::vector<transition>::const_iterator
                                                                                              ti=s->ref_state->start_incoming_transitions; ti!=in_ti_end; ++ti)
                                                                                    {
                                                                                      if (!m_aut.is_tau(m_aut_apply_hidden_label_map(ti->label()))) { break; }
                                                                                      mCRL2complexity(&m_transitions[std::distance(m_aut.get_transitions().
                                                                                                           cbegin(), ti)], finalise_work(check_complexity::
                                                                                          simple_splitB_U__handle_transition_to_U_state, check_complexity::
                                                                                          simple_splitB__handle_transition_to_R_or_U_state, max_new_B), *this);
                                                                                    }
                                                                                    if (has_co_splitter && XcludeCo==current_search &&
                                                                                        0!=s->ref_state->no_of_outgoing_block_inert_transitions)
                                                                                    {
                                                                                      // outgoing transitions of s
                                                                                      const outgoing_transitions_const_it out_ti_end=
                                                                                         std::next(s->ref_state)>=m_states.end() ? m_outgoing_transitions.end()
                                                                                                         : std::next(s->ref_state)->start_outgoing_transitions;
                                                                                      for (outgoing_transitions_const_it
                                                                                             ti=s->ref_state->start_outgoing_transitions; ti!=out_ti_end; ++ti)
                                                                                      {
                                                                                        assert(has_main_splitter || has_co_splitter);
                                                                                        mCRL2complexity(&m_transitions[*ti->ref.BLC_transitions],
                                                                                          finalise_work(check_complexity::
                                                                                                  simple_splitB_U__handle_transition_from_potential_U_state,
                                                                                            check_complexity::
                                                                                                  simple_splitB__handle_transition_from_R_or_U_state,
                                                                                                                                            max_new_B), *this);
                                                                                      }
                                                                                    }
                                                                                  }
                                                                                  if (has_co_splitter && XcludeCo==current_search)
                                                                                  {
                                                                                    // Also handle the work for states that were potentially in XcludeCo but
                                                                                    // turned out to be new bottom states.  The states that ended up actually
                                                                                    // in XcludeCo have already been handled above.  We just go over all states
                                                                                    // again, as only the non-XcludeCo-states have the relevant counter !=0.
                                                                                    // (We cannot only go over non_bottom_states_MultiSub because some states
                                                                                    // may have been handled by XcludeCo after MultiSub became too large.)
                                                                                    for (const state_in_block_pointer*
                                                                                                        s=bi->sta.rt_non_bottom_states; s!=bi->end_states; ++s)
                                                                                    {
                                                                                      // outgoing transitions of s
                                                                                      const outgoing_transitions_const_it out_ti_end=
                                                                                         std::next(s->ref_state)>=m_states.end() ? m_outgoing_transitions.end()
                                                                                                         : std::next(s->ref_state)->start_outgoing_transitions;
                                                                                      for (outgoing_transitions_const_it
                                                                                             ti=s->ref_state->start_outgoing_transitions; ti!=out_ti_end; ++ti)
                                                                                      {
                                                                                        assert(has_main_splitter || has_co_splitter);
                                                                                        mCRL2complexity(&m_transitions[*ti->ref.BLC_transitions], finalise_work
                                                                                            (check_complexity::
                                                                                                     simple_splitB_U__handle_transition_from_potential_U_state,
                                                                                             check_complexity::
                                                                                               simple_splitB__test_outgoing_transitions_found_new_bottom_state,
                                                                                             1), *this);
                                                                                        // At this point we have not yet identified the new bottom states,
                                                                                        // so we cannot be more specific than giving ``1'' as the new counter
                                                                                        // value to be assigned if there has been work.  After identifying the
                                                                                        // new bottom states, we could be more strict and require ``0'' in
                                                                                        // states that are still non-bottom.
                                                                                      }
                                                                                    }
                                                                                  }
                                                                                #endif
            if (3>no_of_finished_searches)
            {
              /* If MultiSub is not empty, then the following reserve() call */ assert(finished!=status_MultiSub);
              // would reserve an overapproximation of the needed space,
              // because some states likely have moved from current_search to
              // MultiSub already.  Therefore I do not include it.  Only if
              // MultiSub.size() is less than what is added to it there may be
              // multiple reallocations.  Only if MultiSub.size() is less than
              // 1/3 of what is added to it there will be multiple
              // reallocations.
              if (non_bottom_states_MultiSub.empty())
              {
                non_bottom_states_MultiSub.reserve
                           (// non_bottom_states_MultiSub.size()
                            +potential_non_bottom_states[current_search].size()
                            -non_bottom_states[current_search].size());
              }
              for (state_in_block_pointer st:
                                   potential_non_bottom_states[current_search])
              {                                                                 // The work in this loop can be assigned to the same transition(s) that made
                                                                                // st go into `potential_non_bottom_states[current_search]`.  (It can now be
                                                                                // a final counter, as we know for sure the subblock is not aborted.)
                if (marked_MultiSub!=st.ref_state->counter)
                {                                                               assert(is_in_marked_range_of(st.ref_state->counter, current_search));
                  if (marked(current_search)!=st.ref_state->counter)
                  {                                                             assert(!non_bottom_states_MultiSub.find(st));
                                                                                assert(!non_bottom_states[ReachAlw].find(st));
                    non_bottom_states_MultiSub.add_todo(st);                    assert(!non_bottom_states[XcludeCo].find(st));
                    st.ref_state->counter=marked_MultiSub;                      assert(!non_bottom_states[MissMain].find(st));
                  }                                                             else  {  assert(non_bottom_states[current_search].find(st));  }
                }                                                               else  {  assert(!non_bottom_states[current_search].find(st));  }
              }                                                                 assert(running_searches[current_search_index]==current_search);
              clear(potential_non_bottom_states[current_search]);
              --no_of_running_searches;                                         assert(current_search_index<=no_of_running_searches);
              running_searches[current_search_index]=
                                      running_searches[no_of_running_searches]; assert(has_co_splitter || finished==status[XcludeCo]);
              --current_search_index; /* is now -1, 0 or +1 */                  assert((has_main_splitter && has_co_splitter) ||
                                                                                       potential_non_bottom_states_Hit_Main.empty());
              if (has_main_splitter && has_co_splitter &&
                  finished==status[ReachAlw] && aborted!=status_MultiSub)
              {
                if (finished==status[XcludeCo])
                {                                                               assert(1>=no_of_running_searches);
                  /* The Hit_Main states can be assigned to MultiSub because */ assert(finished!=status[MissMain]);
                  /* they cannot be in ReachAlw or XcludeCo                  */ assert(finished!=status_MultiSub);
                  for (state_in_block_pointer st:
                                          potential_non_bottom_states_Hit_Main)
                  {                                                             assert(0<st.ref_state->no_of_outgoing_block_inert_transitions);
                                                                                // The work in this loop can be assigned to the same transitions in
                                                                                // the main splitter as the one(s) that made st become a member of
                                                                                // `potential_non_bottom_states_Hit_Main`.
                                                                                assert(!non_bottom_states[MissMain].find(st));
                    if (marked_Hit_Main==st.ref_state->counter)
                    {                                                           assert(!non_bottom_states_MultiSub.find(st));
//std::cerr << "    and consequently, " debug_coroutine_of_Hit_Main "-" << st.ref_state->debug_id(*this) << " is added to " debug_coroutine_of_MultiSub ".\n";
                      non_bottom_states_MultiSub.add_todo(st);                  assert(!non_bottom_states[ReachAlw].find(st));
                      st.ref_state->counter=marked_MultiSub;                    assert(!non_bottom_states[XcludeCo].find(st));
                    }                                                           else  {  assert(marked(ReachAlw)==st.ref_state->counter ||
                                                                                                marked(XcludeCo)==st.ref_state->counter ||
                                                                                                marked_MultiSub==st.ref_state->counter);  }
                  }
//std::cerr << "    and consequently, " debug_coroutine_of_Hit_Main " is guaranteed to be empty.\n";
                  clear(potential_non_bottom_states_Hit_Main);
                }
                #ifdef USE_INCLUDE_CO
                  else if (finished==status[MissMain])
                  {                                                             assert(1>=no_of_running_searches);
                    /* The NcludeCo states can be assigned to MultiSub       */ assert(finished!=status[XcludeCo]);
                    /* because they cannot be in ReachAlw or MissMain        */ assert(finished!=status_MultiSub);
                    for (state_in_block_pointer st:
                                          potential_non_bottom_states_NcludeCo)
                    {                                                           assert(0<st.ref_state->no_of_outgoing_block_inert_transitions);
                                                                                // The work in this loop can be assigned to the same transition in
                                                                                // the co-splitter as the one(s) that made st become a member of
                                                                                // `potential_non_bottom_states_NcludeCo`.
                                                                                assert(!non_bottom_states[XcludeCo].find(st));
                      if (marked_NcludeCo==st.ref_state->counter)
                      {                                                         assert(!non_bottom_states_MultiSub.find(st));
//std::cerr << "    and consequently, " debug_coroutine_of_NcludeCo "-" << st.ref_state->debug_id(*this) << " is added to " debug_coroutine_of_MultiSub ".\n";
                        non_bottom_states_MultiSub.add_todo(st);                assert(!non_bottom_states[ReachAlw].find(st));
                        st.ref_state->counter=marked_MultiSub;                  assert(!non_bottom_states[MissMain].find(st));
                      }                                                         else  {  assert(marked(ReachAlw)==st.ref_state->counter ||
                                                                                                marked(MissMain)==st.ref_state->counter ||
                                                                                                marked_MultiSub==st.ref_state->counter);  }
                    }
//std::cerr << "    and consequently, " debug_coroutine_of_NcludeCo " is guaranteed to be empty.\n";
                    clear (potential_non_bottom_states_NcludeCo);
                  }
                #endif
              }
              if (std::numeric_limits<state_index>::max()!=
                                              no_of_unfinished_states_in_block)
              {                                                                 assert(0<no_of_running_searches);  assert(no_of_running_searches<=2);
                                                                                assert(aborted!=status[ReachAlw]);  assert(aborted!=status[XcludeCo]);
                                                                                assert(aborted!=status[MissMain]);  assert(aborted!=status_MultiSub);
                no_of_unfinished_states_in_block-=
                                    bottom_and_non_bottom_size(current_search); assert(aborted!=status[running_searches[0]]);
                /* Try to find out whether some other process needs to be    */ assert(finished!=status[running_searches[0]]);
                /* aborted, now that we have a more strict size bound.       */ assert(aborted!=status_MultiSub);
                if (abort_if_size_too_large(running_searches[0], ))
                {
//std::cerr << "    and consequently, " << debug_coroutine_of(running_searches[0]) << " is aborted.\n";
                  // The if test is not necessary, as the result will just
                  // be ignored if 1==no_of_running_searches, because we will
                  // have 0==no_of_running_searches after the decrement a few
                  // lines further down.
                  // if (1<no_of_running_searches)
                  // {
                    running_searches[0]=running_searches[1];
                  // }
                  if (0==current_search_index)
                  {
                    --current_search_index;
                  }
                  --no_of_running_searches; // is now 0 or 1
                }
                else if (1<no_of_running_searches && (                          assert(aborted!=status[running_searches[1]]),
                                                                                assert(finished!=status[running_searches[1]]),
                         abort_if_size_too_large(running_searches[1], )))
                {
//std::cerr << "    and consequently, " << debug_coroutine_of(running_searches[1]) << " is aborted.\n";
                  // if (1==current_search_index)  { --current_search_index; }
                  // < will be ignored, because the new search index will
                  // then become 1 again, which is >= the number of running
                  // searches, so the inner main loop will be exited anyway.
                  --no_of_running_searches;                                     assert(1==no_of_running_searches);
                }
                else
                {
                  abort_if_non_bottom_size_too_large_MultiSub();
//&& (std::cerr << "    and consequently, " debug_coroutine_of_MultiSub " is aborted.\n");
                }
              }
              continue;
            }

            /* All three subblocks ReachAlw/XcludeCo/MissMain are finished.  */ assert(finished==status[MissMain]);  assert(finished==status[XcludeCo]);
            /* MultiSub is unfinished.                                       */ assert(finished==status[ReachAlw]);

            /* Calculate the placement of subblocks:                         */
            new_start_bottom_states(ReachAlw+1)=
                            start_bottom_states[ReachAlw+1]+
                                            non_bottom_states[ReachAlw].size();
            new_end_bottom_states(MissMain)=
                       new_start_bottom_states(MissMain)+bottom_size(MissMain);
            new_start_bottom_states(MissMain+1)=
                              new_end_bottom_states(MissMain)+
                                            non_bottom_states[MissMain].size();
            new_end_bottom_states(XcludeCo)=
                       new_start_bottom_states(XcludeCo)+bottom_size(XcludeCo);
            new_end_bottom_states_MultiSub=new_end_bottom_states(XcludeCo)+
                                            non_bottom_states[XcludeCo].size();
                                                                                #if !defined(NDEBUG) || defined(COUNT_WORK_BALANCE)
                                                                                  // Finish the accounting.  First check that there were not too many waiting
                                                                                  // cycles:  (This check may have been done in MultiSub but we cannot be sure;
                                                                                  // MultiSub may have been aborted earlier.)
                                                                                  check_complexity::check_waiting_cycles();
                                                                                  // After this check we are no longer allowed to wait, and we are allowed to
                                                                                  // cancel work.
                                                                                  if (has_co_splitter) {
                                                                                    // Cancel work in the whole block. Actually only the work in MultiSub needs
                                                                                    // to be cancelled, but the states may not yet have moved there.
                                                                                    for (const state_in_block_pointer*
                                                                                               s=bi->start_bottom_states; s!=bi->sta.rt_non_bottom_states; ++s)
                                                                                    {
                                                                                      // outgoing transitions of s
                                                                                      const outgoing_transitions_it out_ti_end=
                                                                                         std::next(s->ref_state)>=m_states.end() ? m_outgoing_transitions.end()
                                                                                                         : std::next(s->ref_state)->start_outgoing_transitions;
                                                                                      for (outgoing_transitions_it
                                                                                             ti=s->ref_state->start_outgoing_transitions; ti!=out_ti_end; ++ti)
                                                                                      {
                                                                                        assert(has_main_splitter || has_co_splitter);
                                                                                        mCRL2complexity(&m_transitions[*ti->ref.BLC_transitions],
                                                                                              cancel_work(check_complexity::
                                                                                                      simple_splitB_R__handle_transition_from_R_state), *this);
                                                                                      }
                                                                                    }
                                                                                  }
                                                                                  for (const state_in_block_pointer*
                                                                                                        s=bi->sta.rt_non_bottom_states; s!=bi->end_states; ++s)
                                                                                  {
                                                                                    mCRL2complexity(s->ref_state, cancel_work
                                                                                                (check_complexity::simple_splitB_R__find_predecessors), *this);
                                                                                    // incoming tau-transitions of s
                                                                                    const std::vector<transition>::iterator in_ti_end=
                                                                                        std::next(s->ref_state)>=m_states.end() ? m_aut.get_transitions().end()
                                                                                                         : std::next(s->ref_state)->start_incoming_transitions;
                                                                                    for (std::vector<transition>::iterator
                                                                                              ti=s->ref_state->start_incoming_transitions; ti!=in_ti_end; ++ti)
                                                                                    {
                                                                                      if (!m_aut.is_tau(m_aut_apply_hidden_label_map(ti->label()))) { break; }
                                                                                      mCRL2complexity(&m_transitions[std::distance(m_aut.
                                                                                            get_transitions().begin(), ti)], cancel_work(check_complexity::
                                                                                                        simple_splitB_R__handle_transition_to_R_state), *this);
                                                                                    }
                                                                                    if (has_co_splitter) {
                                                                                      // outgoing transitions of s
                                                                                      const outgoing_transitions_it out_ti_end=
                                                                                         std::next(s->ref_state)>=m_states.end() ? m_outgoing_transitions.end()
                                                                                                         : std::next(s->ref_state)->start_outgoing_transitions;
                                                                                      for (outgoing_transitions_it
                                                                                             ti=s->ref_state->start_outgoing_transitions; ti!=out_ti_end; ++ti)
                                                                                      {
                                                                                        assert(has_main_splitter || has_co_splitter);
                                                                                        mCRL2complexity(&m_transitions[*ti->ref.BLC_transitions],
                                                                                              cancel_work(check_complexity::
                                                                                                      simple_splitB_R__handle_transition_from_R_state), *this);
                                                                                      }
                                                                                    }
                                                                                  }
                                                                                  // Reset the work balance counters:
                                                                                  check_complexity::check_temporary_work();
                                                                                #endif
            if (new_end_bottom_states_MultiSub==bi->end_states)
            {
              // As MultiSub is empty, we do not need to split off one of the
              // other (non-empty) subblocks. Choose the largest one.
              enum subblocks max_process=XcludeCo;
              state_index max_size=bottom_and_non_bottom_size(XcludeCo);
              if (!has_co_splitter ||
                  (has_main_splitter &&
                   max_size<bottom_and_non_bottom_size(MissMain)))
              {
                max_size=bottom_and_non_bottom_size(MissMain);
                max_process=MissMain;
              }                                                                 else  {  assert(bottom_and_non_bottom_size(MissMain)<=max_size);  }
              if (max_size<bottom_and_non_bottom_size(ReachAlw))
              {
                max_size=bottom_and_non_bottom_size(ReachAlw);
                max_process=ReachAlw;
              }
              status_MultiSub=finished;
//std::cerr << "Aborting " << debug_coroutine_of(max_process) << " instead of aborting " debug_coroutine_of_MultiSub " because the latter is empty\n";
              status[max_process]=aborted;
              // we need to swap the vectors for clearing the state counters:
              clear(potential_non_bottom_states[current_search]);               assert(potential_non_bottom_states[max_process].empty());
              non_bottom_states[max_process].swap_vec
                                    (potential_non_bottom_states[max_process]);
                                                                                #ifndef NDEBUG
                                                                                  for (state_in_block_pointer st: potential_non_bottom_states_Hit_Main) {
                                                                                    assert(has_main_splitter);  assert(has_co_splitter);
              /* All Hit_Main states must have been assigned to some         */     assert(marked(ReachAlw)==st.ref_state->counter ||
              /* subblock, so there is no need to clear these state counters */            marked(XcludeCo)==st.ref_state->counter);
              /* as well:                                                    */   }
                                                                                #endif
              if (has_main_splitter && has_co_splitter)
              {
                clear(potential_non_bottom_states_Hit_Main);
              }
              #ifdef USE_INCLUDE_CO
                                                                                #ifndef NDEBUG
                                                                                  for (state_in_block_pointer st: potential_non_bottom_states_NcludeCo) {
                                                                                    assert(has_co_splitter);
                /* All NcludeCo states must have been assigned to some       */     assert(marked(ReachAlw)==st.ref_state->counter ||
                /* subblock, so there is no need to clear these state        */            marked(MissMain)==st.ref_state->counter);
                /* counters as well:                                         */   }
                                                                                #endif
                if (has_co_splitter)
                {
                  clear(potential_non_bottom_states_NcludeCo);
                }
              #endif
              goto end_for_empty_MultiSub_subblock;
            }

            constellation_type* const constellation=bi->c.onstellation;
            if (constellation->start_const_states->ref_state->block==
                std::prev(constellation->end_const_states)->ref_state->block)
            {                                                                   assert(std::find(m_non_trivial_constellations.begin(),
              /* This constellation was trivial, as it will be split add it  */                  m_non_trivial_constellations.end(),
              /* to the non-trivial constellations.                          */                  constellation)==m_non_trivial_constellations.end());
              m_non_trivial_constellations.emplace_back(constellation);
            }

            // Split off MultiSub -- actually just make *bi smaller
            block_type* const MultiSub_block_index=bi;
            bi->start_bottom_states=new_end_bottom_states_MultiSub;             assert(bi->start_bottom_states<bi->end_states);
            bi->sta.rt_non_bottom_states=new_end_bottom_states_MultiSub;
            // We have to clear state counters of the current search because
            // some of these states may be actually MultiSub-states that have
            // not yet been identified as such:
            clear_state_counters
                       (potential_non_bottom_states[current_search].begin(),
                        potential_non_bottom_states[current_search].end(), bi);
            clear(potential_non_bottom_states[current_search]);                 assert(potential_non_bottom_states[ReachAlw].empty());
            /* The other processes have finished earlier and transferred     */ assert(potential_non_bottom_states[XcludeCo].empty());
            /* their states in potential_non_bottom_states to MultiSub.      */ assert(potential_non_bottom_states[MissMain].empty());
            clear_state_counters(non_bottom_states_MultiSub.begin(),
                                 non_bottom_states_MultiSub.end(), bi);
            clear(non_bottom_states_MultiSub);
            // Some Hit_Main states may also be not-yet-found MultiSub states,
            // so we have to clear these state counters as well.
            clear_state_counters
                              (potential_non_bottom_states_Hit_Main.begin(),
                               potential_non_bottom_states_Hit_Main.end(), bi);
            clear(potential_non_bottom_states_Hit_Main);
            #ifdef USE_INCLUDE_CO
              // Some NcludeCo states may also be not-yet-found MultiSub states,
              // so we have to clear these state counters as well.
              clear_state_counters
                              (potential_non_bottom_states_NcludeCo.begin(),
                               potential_non_bottom_states_NcludeCo.end(), bi);
              clear(potential_non_bottom_states_NcludeCo);
            #endif
                                                                                assert(has_co_splitter ||
                                                                                       new_start_bottom_states(XcludeCo)==new_start_bottom_states(XcludeCo+1));
            /* Split off the third subblock (XcludeCo)                       */ static_assert(2==XcludeCo);  assert(finished==status[XcludeCo]);
            if (has_co_splitter &&
                new_start_bottom_states(XcludeCo)!=
                                           new_start_bottom_states(XcludeCo+1))
            {                                                                   assert(0<bottom_size(XcludeCo));
              move_nonbottom_states_to(non_bottom_states[XcludeCo],
                                              new_end_bottom_states(XcludeCo)
                                                                                #if !defined(NDEBUG) || defined(COUNT_WORK_BALANCE)
                                                                                  , bottom_size(XcludeCo)
                                                                                #endif
                                                                             );
              if (start_bottom_states[XcludeCo]!=
                                             new_start_bottom_states(XcludeCo))
              {
                multiple_swap_states_in_states_in_block
                    (start_bottom_states[XcludeCo],
                     new_start_bottom_states(XcludeCo), bottom_size(XcludeCo)
                                                                                #if !defined(NDEBUG) || defined(COUNT_WORK_BALANCE)
                                                                                  , start_bottom_states[XcludeCo],
                                                                                    check_complexity::log_n-
                                                                                                  check_complexity::ilog2(bottom_and_non_bottom_size(XcludeCo))
                                                                                #endif
                                                                             );
              }
              clear(non_bottom_states[XcludeCo]); // cannot clear before the above call to bottom_and_non_bottom_size(2)
              create_new_block<!has_main_splitter && !has_co_splitter>
                  (new_start_bottom_states(XcludeCo),
                   new_end_bottom_states(XcludeCo),
                   new_start_bottom_states(XcludeCo+1), bi,
                   old_constellation, new_constellation);
              check_incoming_tau_transitions_become_noninert
                                         (MultiSub_block_index,
                                          new_start_bottom_states(XcludeCo),
                                          new_start_bottom_states(XcludeCo+1));
            }                                                                   else {
//std::cerr << "Not creating a new block for " debug_coroutine_of_XcludeCo " as it's empty\n";
                                                                                  assert(0==bottom_size(XcludeCo));assert(non_bottom_states[XcludeCo].empty());
                                                                                }
                                                                                assert(has_co_splitter ||
                                                                                       new_start_bottom_states(MissMain)!=new_start_bottom_states(MissMain+1));
            /* Split off the second subblock (MissMain)                      */ static_assert(1==MissMain);  assert(finished==status[MissMain]);
            if (!has_co_splitter ||
                (has_main_splitter &&
                 new_start_bottom_states(MissMain)!=
                                          new_start_bottom_states(MissMain+1)))
            {                                                                   assert(0<bottom_size(MissMain));
              move_nonbottom_states_to(non_bottom_states[MissMain],
                                              new_end_bottom_states(MissMain)
                                                                                #if !defined(NDEBUG) || defined(COUNT_WORK_BALANCE)
                                                                                  , bottom_size(MissMain)
                                                                                #endif
                                                                             );
              if (start_bottom_states[MissMain]!=
                                             new_start_bottom_states(MissMain))
              {
                multiple_swap_states_in_states_in_block
                    (start_bottom_states[MissMain],
                     new_start_bottom_states(MissMain), bottom_size(MissMain)
                                                                                #if !defined(NDEBUG) || defined(COUNT_WORK_BALANCE)
                                                                                  , start_bottom_states[MissMain],
                                                                                    check_complexity::log_n-
                                                                                                  check_complexity::ilog2(bottom_and_non_bottom_size(MissMain))
                                                                                #endif
                                                                             );
              }
              clear(non_bottom_states[MissMain]); // cannot clear before the above call to bottom_and_non_bottom_size(XcludeCo)
              create_new_block<!has_main_splitter && !has_co_splitter>
                  (new_start_bottom_states(MissMain),
                   new_end_bottom_states(MissMain),
                   new_start_bottom_states(MissMain+1), bi,
                   old_constellation, new_constellation);
              check_incoming_tau_transitions_become_noninert
                                         (MultiSub_block_index,
                                          new_start_bottom_states(MissMain),
                                          new_start_bottom_states(MissMain+1));
            }                                                                   else {
//std::cerr << "Not creating a new block for " debug_coroutine_of_MissMain " as it's empty\n";
                                                                                  assert(0==bottom_size(MissMain));assert(non_bottom_states[MissMain].empty());
                                                                                }

            /* Split off the first subblock (ReachAlw)                       */ static_assert(0==ReachAlw);  assert(finished==status[ReachAlw]);
            block_type* ReachAlw_block_index=null_block;
            if (start_bottom_states[ReachAlw]!=
                                           new_start_bottom_states(ReachAlw+1))
            {                                                                   assert(0<bottom_size(ReachAlw));
              move_nonbottom_states_to(non_bottom_states[ReachAlw],
                                              start_bottom_states[ReachAlw+1]
                                                                                #if !defined(NDEBUG) || defined(COUNT_WORK_BALANCE)
                                                                                  , bottom_size(ReachAlw)
                                                                                #endif
                                                                             );
              clear(non_bottom_states[ReachAlw]);
              ReachAlw_block_index=create_new_block
                  <!has_main_splitter && !has_co_splitter>
                  (start_bottom_states[ReachAlw],
                   start_bottom_states[ReachAlw+1],
                   new_start_bottom_states(ReachAlw+1), bi,
                   old_constellation, new_constellation);
              check_incoming_tau_transitions_become_noninert
                                         (MultiSub_block_index,
                                          start_bottom_states[ReachAlw],
                                          new_start_bottom_states(ReachAlw+1));
            }                                                                   else {
//std::cerr << "Not creating a new block for " debug_coroutine_of_ReachAlw " as it's empty\n";
                                                                                  assert(0==bottom_size(ReachAlw));assert(non_bottom_states[ReachAlw].empty());
                                                                                }
            MultiSub_block_index->contains_new_bottom_states=true;              assert(MultiSub_block_index->start_bottom_states<
                                                                                                               MultiSub_block_index->sta.rt_non_bottom_states);
            m_blocks_with_new_bottom_states.push_back(MultiSub_block_index);
            return ReachAlw_block_index;
          }
        } // end of inner coroutine loop for the ReachAlw/XcludeCo/MissMain-states

        // Now do one step for the MultiSub-states:
//std::cerr << "Doing one step for coroutine " debug_coroutine_of_MultiSub ": ";

        if (incoming_inert_transition_checking==status_MultiSub)
        {                                                                       assert(current_source_iter_MultiSub<current_source_iter_end_MultiSub);
//std::cerr << "incoming_inert_transition_checking " << m_transitions[std::distance(m_aut.get_transitions().begin(), current_source_iter_MultiSub)].debug_id(*this);
                                                                                mCRL2complexity(&m_transitions[std::distance(m_aut.get_transitions().begin(),
                                                                                            current_source_iter_MultiSub)], add_work(check_complexity::
                                                                                                     simple_splitB_R__handle_transition_to_R_state, 1), *this);
          const transition& tr=*current_source_iter_MultiSub++;                 assert(m_aut.is_tau(m_aut_apply_hidden_label_map(tr.label())));
          state_in_block_pointer const src=m_states.begin()+tr.from();          assert(m_states[tr.to()].block==bi);
          if (src.ref_state->block==bi &&
              !(m_preserve_divergence && tr.from()==tr.to()))
          {
            if (marked_MultiSub!=src.ref_state->counter)
            {                                                                   assert(!non_bottom_states[ReachAlw].find(src));
              src.ref_state->counter=marked_MultiSub;                           assert(!non_bottom_states_MultiSub.find(src));
                                                                                assert(!non_bottom_states[XcludeCo].find(src));
              non_bottom_states_MultiSub.add_todo(src);                         assert(!non_bottom_states[MissMain].find(src));
//std::cerr << ": source added to " debug_coroutine_of_MultiSub;
              if (abort_if_non_bottom_size_too_large_MultiSub())
              {
//std::cerr << ", which is aborted\n";
                continue;
              }
//std::cerr << '\n';
            }                                                                   else {
//std::cerr << ": source is already in " debug_coroutine_of_MultiSub "\n";
                                                                                  assert(non_bottom_states_MultiSub.find(src));
                                                                                }
          }
//else std::cerr << " is not block-inert\n";
          if (current_source_iter_MultiSub==current_source_iter_end_MultiSub ||
              !m_aut.is_tau(m_aut_apply_hidden_label_map
                                      (current_source_iter_MultiSub->label())))
          {
            status_MultiSub=state_checking;
          }
        }
        else if (state_checking==status_MultiSub)
        {
//std::cerr << "state_checking ";
          if (!non_bottom_states_MultiSub.todo_is_empty())
          {
            state_in_block_pointer
                               tgt=non_bottom_states_MultiSub.move_from_todo();
//std::cerr << tgt.ref_state->debug_id(*this) << '\n';
            /* Prepare for the sources of tgt to be added to the subblock    */ mCRL2complexity(tgt.ref_state,
                                                                                     add_work(check_complexity::simple_splitB_R__find_predecessors, 1), *this);
            current_source_iter_MultiSub=
                                     tgt.ref_state->start_incoming_transitions;
            current_source_iter_end_MultiSub=
                  std::next(tgt.ref_state)>=m_states.end()
                        ? m_aut.get_transitions().end()
                        : std::next(tgt.ref_state)->start_incoming_transitions;
            if(current_source_iter_MultiSub<current_source_iter_end_MultiSub &&
                m_aut.is_tau(m_aut_apply_hidden_label_map
                                      (current_source_iter_MultiSub->label())))
            {
              status_MultiSub=incoming_inert_transition_checking;
            }
            continue;
          }
          #ifndef USE_INCLUDE_CO
            if (1>=no_of_finished_searches)
            {
//std::cerr << "but there is nothing to do\n";
                                                                                #if !defined(NDEBUG) || defined(COUNT_WORK_BALANCE)
              /* Nothing can be done now for the MultiSub-subblock; we just  */   check_complexity::wait();
              // have to wait for another subblock to give us some initial
              // MultiSub-state.
                                                                                #endif
              continue;
            }
          #endif
          if (has_co_splitter && finished!=status[XcludeCo] &&
              co_splitter_iter_MultiSub!=co_splitter_iter_end_MultiSub)
          {
            // Because we have nothing else to do, we handle one transition in
            // the co-splitter.  However, it may turn out later that the
            // transitions actually starts in the aborted subblock, and then we
            // will have to count this step as a waiting cycle as well.
            do
            {
              const transition&
                         t=m_aut.get_transitions()[*co_splitter_iter_MultiSub];
//std::cerr << "but actually initializing co-splitter " << m_transitions[*co_splitter_iter_MultiSub].debug_id(*this);
              ++co_splitter_iter_MultiSub;
              state_in_block_pointer src=m_states.begin()+t.from();             assert(src.ref_state->block==bi);
              if (0==src.ref_state->no_of_outgoing_block_inert_transitions)
              {                                                                 assert(!(start_bottom_states[XcludeCo]<=src.ref_state->ref_states_in_blocks &&
                                                                                         src.ref_state->ref_states_in_blocks<start_bottom_states[XcludeCo+1]));
                                                                                #if !defined(NDEBUG) || defined(COUNT_WORK_BALANCE)
                                                                                  const enum subblocks source_coroutine=src.ref_state->ref_states_in_blocks<
                                                                                                         start_bottom_states[ReachAlw+1] ? ReachAlw : MissMain;
                                                                                  if (// 1<no_of_finished_searches || -- not necessary to test this
                                                                                      finished==status[source_coroutine] ||
                                                                                      (aborted!=status[source_coroutine] &&
                                                                                       std::numeric_limits<state_index>::max()==
                                                                                                                             no_of_unfinished_states_in_block))
                                                                                  {
                                                                                    // The source subblock will not be aborted, so we can count this as normal
                                                                                    // work
                                                                                    mCRL2complexity(&m_transitions[*std::prev(co_splitter_iter_MultiSub)],
                                                                                          add_work(check_complexity::
                                                                                                   simple_splitB_R__handle_transition_from_R_state, 1), *this);
//std::cerr << '\n';
                                                                                  } else {
                                                                                    // We cannot be sure whether this transitions starts in a subblock that
                                                                                    // will be aborted, so we still register it as a waiting cycle.
                                                                                    check_complexity::wait();
//std::cerr << " (accounting as a wait cycle)\n";
                                                                                  }
                                                                                #endif
              }
              else
              {
                                                                                #if !defined(NDEBUG) || defined(COUNT_WORK_BALANCE)
                                                                                  if (1<no_of_finished_searches || aborted==status[XcludeCo]) {
                                                                                    // It is clear that the transition does not start in an aborted subblock.
                                                                                    mCRL2complexity(&m_transitions[*std::prev(co_splitter_iter_MultiSub)],
                                                                                          add_work(check_complexity::
                                                                                                   simple_splitB_R__handle_transition_from_R_state, 1), *this);
//std::cerr << '\n';
                                                                                  } else {
                                                                                    check_complexity::wait();
//std::cerr << " (accounting as a wait cycle)\n";
                                                                                  }
                                                                                #endif
                #ifdef USE_INCLUDE_CO
                  if (undefined==src.ref_state->counter &&
                      1>=no_of_finished_searches)
                  {
                    // At least one of ReachAlw or MissMain is not finished,
                    // so we cannot be sure that src is in MultiSub yet.
                    src.ref_state->counter=marked_NcludeCo;
                    potential_non_bottom_states_NcludeCo.push_back(src);
                    break;
                  }
                #endif
                if ((undefined==src.ref_state->counter                          && (assert(finished==status[ReachAlw]),
                                                                                    assert(finished==status[MissMain]), true)
                                                      ) ||
                    is_in_marked_range_of(src.ref_state->counter, XcludeCo))
                {                                                               assert(!non_bottom_states[ReachAlw].find(src));
                  /* The only subblocks that src could go to are XcludeCo    */ assert(!non_bottom_states[MissMain].find(src));
                  /* and MultiSub.  But because it has a transition in the   */ assert(!non_bottom_states[XcludeCo].find(src));
                  /* co-splitter, it cannot go to XcludeCo.                  */ assert(!non_bottom_states_MultiSub.find(src));
                  src.ref_state->counter=marked_MultiSub;
                  non_bottom_states_MultiSub.add_todo(src);
                  if (0==no_of_running_searches)
                  {
                    // MultiSub is the only running search (and XcludeCo is not
                    // finished, so it must be aborted), so we can as well
                    // continue this loop until we've found all such states.
                    // We also know that MultiSub cannot become too large.
                    continue;
                  }
                  abort_if_non_bottom_size_too_large_MultiSub();
                  break;
                }                                                               else  {  assert(marked_Hit_Main!=src.ref_state->counter);  }
              }
              if (0!=no_of_running_searches)
              {
                break;
              }
            }
            while (                                                             assert(0==no_of_running_searches), assert(aborted==status[XcludeCo]),
                   co_splitter_iter_MultiSub!=co_splitter_iter_end_MultiSub);
          }
          else
          {
            #ifdef USE_INCLUDE_CO
              if (1>=no_of_finished_searches)
              {
//std::cerr << "but there is nothing to do\n";
                                                                                #if !defined(NDEBUG) || defined(COUNT_WORK_BALANCE)
                /* Nothing can be done now for the MultiSub-subblock; we just*/   check_complexity::wait();
                // have to wait for another subblock to give us some initial
                // MultiSub-state.
                                                                                #endif
                continue;
              }
            #endif                                                              // Now check that there were not too many waiting cycles:
                                                                                #ifndef NDEBUG
                                                                                  check_complexity::check_waiting_cycles();
                                                                                  // After this check we are no longer allowed to wait (and we are allowed to
                                                                                  // cancel work).
                                                                                #endif
            // If finished==status[XcludeCo]:
                // At most one of MissMain and ReachAlw is not finished.
                // If MissMain is not finished, all states with non-exclusive
                // block-inert transitions to XcludeCo or ReachAlw have
                // been added to MultiSub.  Also all states that would
                // be in XcludeCo except for their transition in the
                // co-splitter have been added to MultiSub.  The search for
                // MissMain-predecessors will not add any further states to
                // MultiSub.
                // If ReachAlw is not finished, the situation is similar.
                // Therefore, we can finish MultiSub.
            // If finished!=status[XcludeCo] &&
            //    co_splitter_iter_MultiSub==co_splitter_iter_end_MultiSub:
                // Until now, XcludeCo and MultiSub were still running, and it
                // was unclear which of the two was smaller.  Now it has turned
                // out that MultiSub has finished all it can do, so XcludeCo
                // shall be aborted.
            status_MultiSub=finished;                                           assert(3==++no_of_finished_searches);
//std::cerr << "\n" debug_coroutine_of_MultiSub " is finished.\n";

            // Calculate the placement of subblocks, and also clear state
            // counters of the aborted subblock:
            new_end_bottom_states_MultiSub=bi->end_states-
                                             non_bottom_states_MultiSub.size();

            if (!has_co_splitter || finished==status[XcludeCo])
            {
              new_end_bottom_states(XcludeCo)=
                              new_start_bottom_states(XcludeCo+1)-
                                            non_bottom_states[XcludeCo].size();
              new_start_bottom_states(XcludeCo)=
                         new_end_bottom_states(XcludeCo)-bottom_size(XcludeCo);
              #ifdef USE_INCLUDE_CO
                // Some NcludeCo states may still linger around in the aborted
                // subblock.  So we also have to clear these state counters.
                if (has_co_splitter)
                {
                  clear_state_counters
                              (potential_non_bottom_states_NcludeCo.begin(),
                               potential_non_bottom_states_NcludeCo.end(), bi);
                }                                                               else  {  assert(potential_non_bottom_states_NcludeCo.empty());  }
              #endif
              if ((!has_main_splitter && has_co_splitter) ||
                  finished==status[MissMain])
              {                                                                 assert(finished!=status[ReachAlw]);
                new_end_bottom_states(MissMain)=
                              new_start_bottom_states(MissMain+1)-
                                            non_bottom_states[MissMain].size();
                new_start_bottom_states(MissMain)=
                         new_end_bottom_states(MissMain)-bottom_size(MissMain);
                // clear the state counters of the aborted subblock:
                clear(non_bottom_states[ReachAlw]);
                clear_state_counters
                             (potential_non_bottom_states[ReachAlw].begin(),
                              potential_non_bottom_states[ReachAlw].end(), bi);
                clear(potential_non_bottom_states[ReachAlw]);
                // Some Hit_Main states may still linger around in the aborted
                // subblock.  So we also have to clear these state counters.
                if (has_main_splitter && has_co_splitter)
                {
                  clear_state_counters
                              (potential_non_bottom_states_Hit_Main.begin(),
                               potential_non_bottom_states_Hit_Main.end(), bi);
                }                                                               else  {  assert(potential_non_bottom_states_Hit_Main.empty());  }
              }
              else
              {                                                                 assert(finished==status[ReachAlw]);
                new_start_bottom_states(MissMain)=
                              start_bottom_states[ReachAlw+1]+
                                            non_bottom_states[ReachAlw].size();
                new_end_bottom_states(MissMain)=
                       new_start_bottom_states(MissMain)+bottom_size(MissMain);
                // clear the state counters of the aborted subblock:
                clear(non_bottom_states[MissMain]);
                clear_state_counters
                             (potential_non_bottom_states[MissMain].begin(),
                              potential_non_bottom_states[MissMain].end(), bi);
                clear(potential_non_bottom_states[MissMain]);
                // All Hit_Main states must have been captured by another
                // subblock.  So we can just delete them.
              }
            }
            else
            {                                                                   assert(finished==status[ReachAlw]);
              new_start_bottom_states(MissMain)=
                              start_bottom_states[ReachAlw+1]+
                                            non_bottom_states[ReachAlw].size(); assert(finished==status[MissMain]);
              new_end_bottom_states(MissMain)=
                       new_start_bottom_states(MissMain)+bottom_size(MissMain);
              new_start_bottom_states(XcludeCo)=
                              new_end_bottom_states(MissMain)+
                                            non_bottom_states[MissMain].size();
              new_end_bottom_states(XcludeCo)=
                       new_start_bottom_states(XcludeCo)+bottom_size(XcludeCo);
              // clear the state counters of the aborted subblock:
              clear(non_bottom_states[XcludeCo]);
              clear_state_counters
                             (potential_non_bottom_states[XcludeCo].begin(),
                              potential_non_bottom_states[XcludeCo].end(), bi);
              clear(potential_non_bottom_states[XcludeCo]);
              #ifdef USE_INCLUDE_CO
                // All NcludeCo states must have been hit by some other
                // subblock and included there.  So we can just delete them.
              #endif
              if (has_main_splitter)
              {
                // Some Hit_Main states may still linger around.
                clear_state_counters
                              (potential_non_bottom_states_Hit_Main.begin(),
                               potential_non_bottom_states_Hit_Main.end(), bi);
              }                                                                 else  {  assert(potential_non_bottom_states_Hit_Main.empty());  }
            }
            if (has_co_splitter)
            {
              if (has_main_splitter)
              {
                clear(potential_non_bottom_states_Hit_Main);
              }                                                                 else  {  assert(potential_non_bottom_states_Hit_Main.empty());  }
              #ifdef USE_INCLUDE_CO
                clear(potential_non_bottom_states_NcludeCo);
              #endif
            }                                                                   else  {  assert(potential_non_bottom_states_Hit_Main.empty());  }
                                                                                #if !defined(NDEBUG) || defined(COUNT_WORK_BALANCE)
                                                                                  // Finish the accounting.
                                                                                  // (We have already called `check_complexity::check_waiting_cycles()`, so we
                                                                                  // are no longer allowed to wait, and we are allowed to cancel work.)
                                                                                  // Cancel work in the whole block (actually only work in the aborted subblock
                                                                                  // will be cancelled, but we go through the whole block because the states
                                                                                  // have not yet been positioned correctly; also, most likely not all its
                                                                                  // non-bottom states will be in `non_bottom_states[...]`).
                                                                                  {
//std::cerr << "Finalising work in " debug_coroutine_of_MultiSub ", part 1\n";
                                                                                    state_type max_NcludeCo_size=std::distance(
                                                                                                               new_end_bottom_states_MultiSub, bi->end_states);
                                                                                    if (finished==status[ReachAlw]) {
                                                                                      max_NcludeCo_size=std::max<state_type>(max_NcludeCo_size, std::distance(
                                                                                          start_bottom_states[ReachAlw], new_start_bottom_states(ReachAlw+1)));
                                                                                    }
                                                                                    if (finished==status[MissMain]) {
                                                                                      max_NcludeCo_size=std::max<state_type>(max_NcludeCo_size, std::distance(
                                                                                       new_start_bottom_states(MissMain),new_start_bottom_states(MissMain+1)));
                                                                                    }
                                                                                    const unsigned char max_NcludeCo_B=0==max_NcludeCo_size ? 0
                                                                                          : check_complexity::log_n-check_complexity::ilog2(max_NcludeCo_size);
                                                                                    const state_in_block_pointer* s=bi->start_bottom_states;
                                                                                    do {
                                                                                      mCRL2complexity(s->ref_state, cancel_work
                                                                                                (check_complexity::simple_splitB_U__find_predecessors), *this);
                                                                                      // incoming tau-transitions of s
                                                                                      const std::vector<transition>::const_iterator in_ti_end=
                                                                                        std::next(s->ref_state)>=m_states.end() ? m_aut.get_transitions().end()
                                                                                                         : std::next(s->ref_state)->start_incoming_transitions;
                                                                                      for (std::vector<transition>::const_iterator
                                                                                              ti=s->ref_state->start_incoming_transitions; ti!=in_ti_end; ++ti)
                                                                                      {
                                                                                        if(!m_aut.is_tau(m_aut_apply_hidden_label_map(ti->label()))) { break; }
                                                                                        mCRL2complexity(&m_transitions[std::distance(m_aut.get_transitions().
                                                                                                cbegin(), ti)], cancel_work(check_complexity::
                                                                                                        simple_splitB_U__handle_transition_to_U_state), *this);
                                                                                      }
                                                                                      #ifdef USE_INCLUDE_CO
                                                                                        if (!has_co_splitter) {
                                                                                          continue;
                                                                                        }
                                                                                      #else
                                                                                        if (finished==status[XcludeCo]) {
                                                                                          continue;
                                                                                        }
                                                                                      #endif
                                                                                      // outgoing transitions of s
                                                                                      const outgoing_transitions_const_it out_ti_end=
                                                                                         std::next(s->ref_state)>=m_states.end() ? m_outgoing_transitions.end()
                                                                                                         : std::next(s->ref_state)->start_outgoing_transitions;
                                                                                      for (outgoing_transitions_const_it
                                                                                             ti=s->ref_state->start_outgoing_transitions; ti!=out_ti_end; ++ti)
                                                                                      {
                                                                                        assert(has_main_splitter || has_co_splitter);
                                                                                        mCRL2complexity(&m_transitions[*ti->ref.BLC_transitions],
                                                                                            cancel_work(check_complexity::
                                                                                            simple_splitB_U__handle_transition_from_potential_U_state), *this);
                                                                                        // We should also finalise the co-splitter transitions handled by
                                                                                        // MultiSub (which may exist even if MultiSub is empty):
                                                                                        mCRL2complexity(&m_transitions[*ti->ref.BLC_transitions],
                                                                                              finalise_work(check_complexity::
                                                                                                            simple_splitB_R__handle_transition_from_R_state,
                                                                                                            check_complexity::
                                                                                                            simple_splitB__handle_transition_from_R_or_U_state,
                                                                                                                                       max_NcludeCo_B), *this);
                                                                                      }
                                                                                    } while (++s!=bi->end_states);
                                                                                  }
                                                                                #endif
            // split off MultiSub
            // This can be done only after the aborted subblock has cleared
            // its state counters.  But it should be done before the other
            /* splits, so it is easy to detect which transitions are no      */ assert((state_index) std::distance(new_end_bottom_states_MultiSub,
            /* longer block-inert.                                           */                            bi->end_states)==non_bottom_states_MultiSub.size());
            if (new_end_bottom_states_MultiSub!=bi->end_states)
            {                                                                   assert(!non_bottom_states_MultiSub.empty());
              /* As MultiSub is not empty, a trivial constellation will      */ assert(bi->start_bottom_states<new_end_bottom_states_MultiSub);
              // become non-trivial.
              constellation_type* const constellation=bi->c.onstellation;
              if (constellation->start_const_states->ref_state->block==
                  std::prev(constellation->end_const_states)->ref_state->block)
              {                                                                 assert(std::find(m_non_trivial_constellations.begin(),
                /* This constellation was trivial, as it will be split add it*/                  m_non_trivial_constellations.end(),
                /* to the non-trivial constellations.                        */                  constellation)==m_non_trivial_constellations.end());
                m_non_trivial_constellations.emplace_back(constellation);
              }

              move_nonbottom_states_to(non_bottom_states_MultiSub,
                                               new_end_bottom_states_MultiSub
                                                                                #if !defined(NDEBUG) || defined(COUNT_WORK_BALANCE)
                                                                                  , 0
                                                                                #endif
                                                                             );
              clear(non_bottom_states_MultiSub);
              block_type* const MultiSub_block_index=
                  create_new_block<!has_main_splitter && !has_co_splitter>
                             (new_end_bottom_states_MultiSub,
                              new_end_bottom_states_MultiSub,
                              bi->end_states, bi,
                              null_constellation, null_constellation);
                                                                                #if !defined(NDEBUG) || defined(COUNT_WORK_BALANCE)
                                                                                  // Finalise the work in MultiSub.  This should be done after calling `
                                                                                  // check_complexity::check_waiting_cycles()` so MultiSub cannot make its own
                                                                                  // waiting time appear small.
//std::cerr << "Finalising work in " debug_coroutine_of_MultiSub ", part 2\n";
                                                                                  const unsigned char max_new_B=check_complexity::log_n-check_complexity::ilog2
                                                                                      (std::distance(new_end_bottom_states_MultiSub, bi->end_states));
                                                                                  const state_in_block_pointer* s=new_end_bottom_states_MultiSub;
                                                                                  do {
                                                                                    mCRL2complexity(s->ref_state, finalise_work(check_complexity::
                                                                                          simple_splitB_R__find_predecessors, check_complexity::
                                                                                          simple_splitB__find_predecessors_of_R_or_U_state, max_new_B), *this);
                                                                                    // incoming tau-transitions of s
                                                                                    const std::vector<transition>::iterator in_ti_end=
                                                                                        std::next(s->ref_state)>=m_states.end() ? m_aut.get_transitions().end()
                                                                                                         : std::next(s->ref_state)->start_incoming_transitions;
                                                                                    for (std::vector<transition>::iterator
                                                                                              ti=s->ref_state->start_incoming_transitions; ti!=in_ti_end; ++ti)
                                                                                    {
                                                                                      if (!m_aut.is_tau(m_aut_apply_hidden_label_map(ti->label()))) { break; }
                                                                                      mCRL2complexity(&m_transitions[std::distance(m_aut.get_transitions().
                                                                                                begin(), ti)], finalise_work(check_complexity::
                                                                                          simple_splitB_R__handle_transition_to_R_state, check_complexity::
                                                                                          simple_splitB__handle_transition_to_R_or_U_state, max_new_B), *this);
                                                                                    }
                                                                                    // outgoing transitions of s -- already done above if necessary
                                                                                    ++s;
                                                                                  } while (s!=bi->end_states);
                                                                                  // Reset the work balance counters:
                                                                                  check_complexity::check_temporary_work();
                                                                                #endif
              // check transitions that have become non-block-inert:
              for (state_in_block_pointer*
                   nst_it=new_end_bottom_states_MultiSub;
                                              nst_it!=bi->end_states; ++nst_it)
              {
                outgoing_transitions_const_it const out_it_end=
                  std::next(nst_it->ref_state)>=m_states.end()
                    ? m_outgoing_transitions.end()
                    : std::next(nst_it->ref_state)->start_outgoing_transitions;
                outgoing_transitions_it out_it=nst_it->ref_state->
                                                    start_outgoing_transitions; assert(out_it!=out_it_end);
//std::cerr << "Checking whether outgoing transitions of " << nst_it->ref_state->debug_id(*this) << " have become non-block-inert:\n";
                const transition* tr=&m_aut.get_transitions()
                      [has_main_splitter || has_co_splitter /* needed for correctness */
                       ?*out_it->ref.BLC_transitions :out_it->ref.transitions]; assert(0<nst_it->ref_state->no_of_outgoing_block_inert_transitions);
                do
                {                                                               assert(m_states.begin()+tr->from()==nst_it->ref_state);
                                                                                assert(m_aut.is_tau(m_aut_apply_hidden_label_map(tr->label())));
                  if (m_states[tr->to()].block==bi)
                  {                                                             assert(is_inert_during_init(*tr));
                    /* This is a transition that has become non-block-inert. */ assert(bi->start_bottom_states<=m_states[tr->to()].ref_states_in_blocks);
                    /* (However, it is still constellation-inert.)           */
                    /* make_transition_non_inert(*tr)                        */ assert(m_states[tr->to()].ref_states_in_blocks<new_end_bottom_states_MultiSub);
                    /* < would just execute the decrement "--" below:        */ assert(0<nst_it->ref_state->no_of_outgoing_block_inert_transitions);
//std::cerr << "    " << m_transitions[has_main_splitter || has_co_splitter ? *out_it->ref.BLC_transitions : out_it->ref.transitions].debug_id(*this) << " has become non-block-inert";
                    if (0== --nst_it->ref_state->
                                        no_of_outgoing_block_inert_transitions)
                    {
                      // The state at nst_it has become a bottom_state.
//std::cerr << " and its source is a new bottom state\n";
                      change_non_bottom_state_to_bottom_state
                                                           (nst_it->ref_state);
                      break;
                    }
                  }                                                             else {
//std::cerr << " does not change inertness because it's going to " << m_states[tr->to()].block->debug_id(*this);
                                                                                  assert(new_end_bottom_states_MultiSub<=
                                                                                                                     m_states[tr->to()].ref_states_in_blocks ||
                                                                                        m_states[tr->to()].ref_states_in_blocks<start_bottom_states[ReachAlw]);
                                                                                }
//std::cerr << '\n';
                  ++out_it;
                }
                while (out_it!=out_it_end &&
                  (tr=&m_aut.get_transitions()
                      [has_main_splitter || has_co_splitter /* needed for correctness */
                       ?*out_it->ref.BLC_transitions :out_it->ref.transitions],
                     m_aut.is_tau(m_aut_apply_hidden_label_map(tr->label()))));
              }                                                                 assert(MultiSub_block_index->start_bottom_states<
                                                                                                               MultiSub_block_index->sta.rt_non_bottom_states);
              MultiSub_block_index->contains_new_bottom_states=true;
              m_blocks_with_new_bottom_states.push_back(MultiSub_block_index);
            }
            else
            {
                                                                                #ifndef NDEBUG
                                                                                  // Reset the work balance counters:
                                                                                  check_complexity::check_temporary_work();
                                                                                #endif
        end_for_empty_MultiSub_subblock:
//std::cerr << "Not creating a new block for " debug_coroutine_of_MultiSub " as it's empty\n";
                                                                                assert(non_bottom_states_MultiSub.empty());
              if (has_co_splitter
                  ? has_main_splitter
                    ? (start_bottom_states[ReachAlw]!=
                                     new_start_bottom_states(ReachAlw+1))+
                      (new_start_bottom_states(MissMain)!=
                                     new_start_bottom_states(MissMain+1))+
                      (new_start_bottom_states(XcludeCo)!=
                                     new_start_bottom_states(XcludeCo+1))>1
                    : (                                                         assert(new_start_bottom_states(MissMain)==new_start_bottom_states(MissMain+1)),
                       start_bottom_states[ReachAlw]!=
                                        new_start_bottom_states(ReachAlw+1) &&
                       new_start_bottom_states(XcludeCo)!=
                                        new_start_bottom_states(XcludeCo+1))
                  : (                                                           assert(new_start_bottom_states(XcludeCo)==new_start_bottom_states(XcludeCo+1)),
                     start_bottom_states[ReachAlw]!=
                                        new_start_bottom_states(ReachAlw+1) &&
                     new_start_bottom_states(MissMain)!=
                                        new_start_bottom_states(MissMain+1)))
              {
                constellation_type* const constellation=bi->c.onstellation;
                if (constellation->start_const_states->ref_state->block==
                  std::prev(constellation->end_const_states)->ref_state->block)
                {                                                               assert(std::find(m_non_trivial_constellations.begin(),
                  /* This constellation was trivial, as it will be split add */                  m_non_trivial_constellations.end(),
                  /* it to the non-trivial constellations.                   */                  constellation)==m_non_trivial_constellations.end());
                  m_non_trivial_constellations.emplace_back(constellation);
                }
              }                                                                 else assert(0 && "trying to find out whether this condition always holds");
            }

            /* Split off the third subblock (XcludeCo)                       */ assert(finished!=status[XcludeCo] || static_cast<state_index>(std::distance
                                                                                    (new_start_bottom_states(XcludeCo), new_start_bottom_states(XcludeCo+1)))==
                                                                                                                         bottom_and_non_bottom_size(XcludeCo));
            if (has_co_splitter && new_start_bottom_states(XcludeCo)!=
                                           new_start_bottom_states(XcludeCo+1))
            {                                                                   assert(0!=bottom_size(XcludeCo));
              if (start_bottom_states[XcludeCo]!=
                                             new_start_bottom_states(XcludeCo))
              {
                                                                                #if !defined(NDEBUG) || defined(COUNT_WORK_BALANCE)
                                                                                  const state_in_block_pointer* acct_iter;
                                                                                  state_index acct_B_size;
                                                                                  if (finished==status[XcludeCo]) {
                                                                                    acct_iter=start_bottom_states[XcludeCo];
                                                                                    acct_B_size=bottom_and_non_bottom_size(XcludeCo);
                                                                                  } else {
                                                                                    // If XcludeCo is aborted, the work can be assigned to the non-bottom
                                                                                    // states of ReachAlw and MissMain.
                                                                                    assert(non_bottom_states[XcludeCo].empty());
                                                                                    assert(finished==status[ReachAlw]);  assert(finished==status[MissMain]);
                                                                                    state_type count=std::min<state_type>(bottom_size(XcludeCo),
                                                                                                              std::distance(start_bottom_states[XcludeCo],
                                                                                                                           new_start_bottom_states(XcludeCo)));
                                                                                    if (non_bottom_states[MissMain].size()>=count) {
                                                                                      acct_iter=non_bottom_states[MissMain].data();
                                                                                      acct_B_size=bottom_and_non_bottom_size(MissMain);
                                                                                    } else if (non_bottom_states[ReachAlw].size()>=count) {
                                                                                      acct_iter=non_bottom_states[ReachAlw].data();
                                                                                      acct_B_size=bottom_and_non_bottom_size(ReachAlw);
                                                                                    } else {
                                                                                      assert(count<=non_bottom_states[MissMain].size()+
                                                                                                    non_bottom_states[ReachAlw].size());
                                                                                      // As we are not going to use  `non_bottom_states[XcludeCo]` for anything
                                                                                      // else, we just replace its content by the relevant states.
                                                                                      non_bottom_states[XcludeCo]=non_bottom_states[MissMain];
                                                                                      non_bottom_states[XcludeCo].add_todo(non_bottom_states[ReachAlw].begin(),
                                                                                                  non_bottom_states[ReachAlw].begin()
                                                                                                                  +(count-non_bottom_states[XcludeCo].size()));
                                                                                      acct_iter=non_bottom_states[XcludeCo].data();
                                                                                      acct_B_size=std::max(bottom_and_non_bottom_size(MissMain),
                                                                                                           bottom_and_non_bottom_size(ReachAlw));
//std::cerr << "Both " debug_coroutine_of_MissMain " (" << non_bottom_states[MissMain].size() << " states) and " debug_coroutine_of_ReachAlw " (" << non_bottom_states[ReachAlw].size() << " states) are too small to account for moving " << count << " states for the aborted " debug_coroutine_of_XcludeCo " bottom states (" << bottom_size(XcludeCo) << " bottom states).\n";
                                                                                    }
                                                                                  }
                                                                                #endif
                multiple_swap_states_in_states_in_block
                    (start_bottom_states[XcludeCo],
                     new_start_bottom_states(XcludeCo), bottom_size(XcludeCo)
                                                                                #if !defined(NDEBUG) || defined(COUNT_WORK_BALANCE)
                                                                                  , acct_iter, check_complexity::log_n-check_complexity::ilog2(acct_B_size),
                                                                                    finished==status[XcludeCo]
                                                                                    ?check_complexity::multiple_swap_states_in_block__swap_state_in_small_block
                                                                                    :check_complexity::
                                                                                               multiple_swap_states_in_block__account_for_swap_in_aborted_block
                                                                                #endif
                                                                             );
              }

              if (finished==status[XcludeCo])
              {                                                                 assert(potential_non_bottom_states[XcludeCo].empty());
                move_nonbottom_states_to(non_bottom_states[XcludeCo],
                                             new_end_bottom_states(XcludeCo)
                                                                                #if !defined(NDEBUG) || defined(COUNT_WORK_BALANCE)
                                                                                  , bottom_size(XcludeCo)
                                                                                #endif
                                                                             );
                clear(non_bottom_states[XcludeCo]);
                create_new_block<!has_main_splitter && !has_co_splitter>
                  (new_start_bottom_states(XcludeCo),
                   new_end_bottom_states(XcludeCo),
                   new_start_bottom_states(XcludeCo+1), bi,
                   old_constellation, new_constellation);
              }
              else
              {
                                                                                #if !defined(NDEBUG) || defined(COUNT_WORK_BALANCE)
                                                                                  // delete what we've stored in non_bottom_states[XcludeCo] just for
                                                                                  // accounting
                                                                                  clear(non_bottom_states[XcludeCo]);
                                                                                #endif
                bi->start_bottom_states=new_start_bottom_states(XcludeCo);
                bi->sta.rt_non_bottom_states=new_end_bottom_states(XcludeCo);   assert(bi->start_bottom_states<bi->sta.rt_non_bottom_states);
                bi->end_states=new_start_bottom_states(XcludeCo+1);             assert(bi->sta.rt_non_bottom_states<=bi->end_states);
              }
            }                                                                   else {
//std::cerr << "Not creating a new block for " debug_coroutine_of_XcludeCo " as it's empty\n";
                                                                                  assert(0==bottom_size(XcludeCo));assert(non_bottom_states[XcludeCo].empty());
                                                                                  assert(finished==status[XcludeCo]);
                                                                                }
            /* Split off the second subblock (MissMain)                      */ assert(finished!=status[MissMain] || static_cast<state_index>(std::distance
                                                                                    (new_start_bottom_states(MissMain), new_start_bottom_states(MissMain+1)))==
                                                                                                                         bottom_and_non_bottom_size(MissMain));
            if ((has_main_splitter || !has_co_splitter) &&
                new_start_bottom_states(MissMain)!=
                                           new_start_bottom_states(MissMain+1))
            {                                                                   assert(0!=bottom_size(MissMain));
              // If MissMain is aborted, then swapping these bottom states can
              // be accounted for by the non-bottom states of ReachAlw.
              // The function will not execute more swaps than their size.
              if (start_bottom_states[MissMain]!=
                                             new_start_bottom_states(MissMain))
              {
                multiple_swap_states_in_states_in_block
                     (start_bottom_states[MissMain],
                     new_start_bottom_states(MissMain), bottom_size(MissMain)
                                                                                #if !defined(NDEBUG) || defined(COUNT_WORK_BALANCE)
                                                                                  , finished==status[MissMain] ? start_bottom_states[MissMain]
                                                                                                               : non_bottom_states[ReachAlw].data(),
                                                                                    check_complexity::log_n-check_complexity::ilog2
                                                                                        (finished==status[MissMain] ? bottom_and_non_bottom_size(MissMain)
                                                                                                                    : bottom_and_non_bottom_size(ReachAlw)),
                                                                                    finished==status[MissMain]
                                                                                    ?check_complexity::multiple_swap_states_in_block__swap_state_in_small_block
                                                                                    :check_complexity::
                                                                                               multiple_swap_states_in_block__account_for_swap_in_aborted_block
                                                                                #endif
                                                                             );
              }
              if (finished==status[MissMain])
              {                                                                 assert(potential_non_bottom_states[MissMain].empty());
                move_nonbottom_states_to(non_bottom_states[MissMain],
                                             new_end_bottom_states(MissMain)
                                                                                #if !defined(NDEBUG) || defined(COUNT_WORK_BALANCE)
                                                                                  , bottom_size(MissMain)
                                                                                #endif
                                                                             );
                clear(non_bottom_states[MissMain]);
                create_new_block<!has_main_splitter && !has_co_splitter>
                  (new_start_bottom_states(MissMain),
                   new_end_bottom_states(MissMain),
                   new_start_bottom_states(MissMain+1), bi,
                   old_constellation, new_constellation);
              }
              else
              {
                bi->start_bottom_states=new_start_bottom_states(MissMain);
                bi->sta.rt_non_bottom_states=new_end_bottom_states(MissMain);   assert(bi->start_bottom_states<bi->sta.rt_non_bottom_states);
                bi->end_states=new_start_bottom_states(MissMain+1);             assert(bi->sta.rt_non_bottom_states<=bi->end_states);
              }
            }                                                                   else {
//std::cerr << "Not creating a new block for " debug_coroutine_of_MissMain " as it's empty\n";
                                                                                  assert(0==bottom_size(MissMain));assert(non_bottom_states[MissMain].empty());
                                                                                  assert(finished==status[MissMain]);
                                                                                }
            /* Split off the first subblock (ReachAlw)                       */ assert(finished!=status[ReachAlw] || static_cast<state_index>(std::distance
                                                                                        (start_bottom_states[ReachAlw], new_start_bottom_states(ReachAlw+1)))==
                                                                                                                         bottom_and_non_bottom_size(ReachAlw));
            block_type* ReachAlw_block_index=null_block;
            if (start_bottom_states[ReachAlw]!=
                                           new_start_bottom_states(ReachAlw+1))
            {                                                                   assert(0<bottom_size(ReachAlw));
              if (finished==status[ReachAlw])
              {                                                                 assert(potential_non_bottom_states[ReachAlw].empty());
                move_nonbottom_states_to(non_bottom_states[ReachAlw],
                                              start_bottom_states[ReachAlw+1]
                                                                                #if !defined(NDEBUG) || defined(COUNT_WORK_BALANCE)
                                                                                  , bottom_size(ReachAlw)
                                                                                #endif
                                                                             );
                clear(non_bottom_states[ReachAlw]);
                ReachAlw_block_index=create_new_block
                      <!has_main_splitter && !has_co_splitter>
                      (start_bottom_states[ReachAlw],
                       start_bottom_states[ReachAlw+1],
                       new_start_bottom_states(ReachAlw+1), bi,
                       old_constellation, new_constellation);
              }
              else
              {                                                                 assert(bi->start_bottom_states==start_bottom_states[ReachAlw]);
                bi->sta.rt_non_bottom_states=start_bottom_states[ReachAlw+1];   assert(bi->start_bottom_states<bi->sta.rt_non_bottom_states);
                bi->end_states=new_start_bottom_states(ReachAlw+1);             assert(bi->sta.rt_non_bottom_states<=bi->end_states);
                ReachAlw_block_index=bi;
              }
            }                                                                   else {
//std::cerr << "Not creating a new block for " debug_coroutine_of_ReachAlw " as it's empty\n";
                                                                                  assert(0==bottom_size(ReachAlw));assert(non_bottom_states[ReachAlw].empty());
                                                                                }
            return ReachAlw_block_index; // leave the function completely, as we have finished.
          }
        }                                                                       else {
                                                                                  assert(aborted==status_MultiSub);
//std::cerr << "but it's already aborted\n";
                                                                                }
      } // end of outer coroutine loop for ReachAlw/MissMain/XcludeCo and MultiSub together

      #undef bottom_size
      #undef abort_if_bottom_size_too_large
      #undef abort_if_non_bottom_size_too_large_MultiSub
      #undef abort_if_size_too_large
      #undef bottom_and_non_bottom_size
    }
#endif // #ifdef USE_FOUR_WAY_SPLIT

    transition_index accumulate_entries(
                              std::vector<transition_index>& action_counter,
                              const std::vector<label_index>& todo_stack) const
    {
      transition_index sum=0;
      for(label_index index: todo_stack)
      {                                                                         // The work in this loop is attributed to the transitions with label `index`
        transition_index n=sum;
        sum=sum+action_counter[index];
        action_counter[index]=n;
      }
      return sum;
    }

//================================================= Create initial partition ========================================================
    /// \brief create one BLC set for the block starting at `pos`
    /// \details The BLC set is created, inserted into the list
    /// `block.to_constellation` of the block, and the pointers from
    /// transitions to it are adapted.  The function also adapts the
    /// `ref.BLC_transitions` pointer of the transitions in the BLC set.
    void order_BLC_transitions_single_BLC_set(
                                        state_in_block_pointer* const pos,
                                              BLC_list_iterator start_same_BLC,
                                        const BLC_list_iterator end_same_BLC)
    {                                                                           assert(start_same_BLC<end_same_BLC);
      block_type* const bi=pos->ref_state->block;                               assert(pos==bi->start_bottom_states);
      linked_list<BLC_indicators>::iterator blc=bi->
         block.to_constellation.emplace_back(start_same_BLC,end_same_BLC,true);
      if (!is_inert_during_init(m_aut.get_transitions()[*start_same_BLC]))
      {
        ++no_of_non_constellation_inert_BLC_sets;
      }
      do
      {                                                                         assert(bi==m_states[m_aut.get_transitions()[*start_same_BLC].from()].block);
        m_transitions[*start_same_BLC].transitions_per_block_to_constellation=
                                                                           blc; mCRL2complexity(&m_transitions[*start_same_BLC], add_work(check_complexity::
                                                                                      order_BLC_transitions__sort_transition, check_complexity::log_n), *this);
        m_transitions[*start_same_BLC].ref_outgoing_transitions->
                                            ref.BLC_transitions=start_same_BLC;
      }
      while (++start_same_BLC<end_same_BLC);
    }

    /// \brief order `m_BLC_transition` entries according to source block
    /// \param start_same_BLC  first transition to be handled
    /// \param end_same_BLC    iterator past the last transition to be handled
    /// \param min_block       lower bound to the block `start_bottom_states` that can be expected
    /// \param max_block       upper bound to the block `start_bottom_states` that can be expected
    /// \details This function assumes that all transitions in the range
    /// [`start_same_BLC`, `end_same_BLC`) have the same label and the same
    /// target constellation.  They have source blocks whose field
    /// `start_bottom_states` is in the range
    /// [`min_block`, `max_block`].  It groups these transitions according
    /// to their source blocks and inserts the corresponding
    /// `linked_list<BLC_indicators>` entries in the source blocks.  The
    /// algorithm used is similar to quicksort, but the pivot value is
    /// determined by numeric calculations instead of selection from the data.
    ///
    /// The function is intended to be used during initialisation, if one does
    /// not use `m_BLC_transitions` during the first refinements.
    void order_BLC_transitions(const BLC_list_iterator start_same_BLC,
                      const BLC_list_iterator end_same_BLC,
                      state_in_block_pointer* min_block,
                      state_in_block_pointer* max_block)
    {                                                                           assert(start_same_BLC<end_same_BLC);
                                                                                assert(min_block->ref_state->block->start_bottom_states==min_block);
                                                                                assert(max_block->ref_state->block->start_bottom_states==max_block);
      if (min_block==max_block)
      {
        order_BLC_transitions_single_BLC_set(min_block,
                                                 start_same_BLC, end_same_BLC);
        return;
      }                                                                         else  {  assert(min_block<max_block);  }
                                                                                #if !defined(NDEBUG) || defined(COUNT_WORK_BALANCE)
                                                                                  const unsigned char max_sort=check_complexity::log_n-
                                                                                                                check_complexity::ilog2(max_block-min_block+1);
                                                                                #endif
      state_in_block_pointer* pivot=min_block+(max_block-min_block+1)/2;
      pivot=pivot->ref_state->block->start_bottom_states; // round down
      state_in_block_pointer* min_below_pivot=pivot;
      state_in_block_pointer* max_above_pivot=pivot;
      #define max_below_pivot min_block
      #define min_above_pivot max_block
      // move transitions with source_block==pivot to the beginning,
      // transitions with source_block<pivot to the middle,
      // transitions with source_block>pivot to the end
      // (similar to quicksort with equal keys)
      BLC_list_iterator end_equal_to_pivot=start_same_BLC;
      BLC_list_iterator end_smaller_than_pivot=start_same_BLC;
      BLC_list_iterator begin_larger_than_pivot=end_same_BLC;
      for (;;)
      {
        for (;;)
        {                                                                       assert(end_smaller_than_pivot<begin_larger_than_pivot);
                                                                                #ifndef NDEBUG
                                                                                  { const state_in_block_pointer* sb;
                                                                                    BLC_list_const_iterator it=start_same_BLC;
                                                                                    assert(it<=end_equal_to_pivot);
                                                                                    for (; it<end_equal_to_pivot; ++it) {
                                                                                      assert(m_states[m_aut.get_transitions()[*it].from()].block->
                                                                                                                                   start_bottom_states==pivot);
                                                                                    }
                                                                                    assert(it<=end_smaller_than_pivot);
                                                                                    for (; it<end_smaller_than_pivot; ++it) {
                                                                                      assert(max_below_pivot<pivot);
                                                                                      sb=m_states[m_aut.get_transitions()[*it].from()].block->
                                                                                                                                           start_bottom_states;
                                                                                      assert(sb>=min_below_pivot);  assert(sb<=max_below_pivot);
                                                                                    }
                                                                                    assert(it<begin_larger_than_pivot);
                                                                                    for (it=begin_larger_than_pivot; it<end_same_BLC; ++it) {
                                                                                      assert(pivot<min_above_pivot);
                                                                                      sb=m_states[m_aut.get_transitions()[*it].from()].block->
                                                                                                                                           start_bottom_states;
                                                                                      assert(sb>=min_above_pivot);  assert(sb<=max_above_pivot);
                                                                                    }
                                                                                  }
                                                                                #endif
                                                                                mCRL2complexity(&m_transitions[*end_smaller_than_pivot], add_work(
                                                                                   check_complexity::order_BLC_transitions__sort_transition, max_sort), *this);
          state_in_block_pointer* const source_block=
                m_states[m_aut.get_transitions()
                  [*end_smaller_than_pivot].from()].block->start_bottom_states;
          if (source_block==pivot)
          {
            std::swap(*end_equal_to_pivot++, *end_smaller_than_pivot);
          }
          else if (source_block>pivot)
          {
            if (source_block<min_above_pivot)
            {
              min_above_pivot=source_block;
            }
            if (source_block>max_above_pivot)
            {
              max_above_pivot=source_block;
            }
            break;
          }
          else
          {
            if (source_block<min_below_pivot)
            {
              min_below_pivot=source_block;
            }
            if (source_block>max_below_pivot)
            {
              max_below_pivot=source_block;
            }
          }
          ++end_smaller_than_pivot;
          if (end_smaller_than_pivot>=begin_larger_than_pivot)
          {
            goto break_two_loops;
          }
        }
        // Now *end_smaller_than_pivot contains an element with
        // source_block > pivot
        for (;;)
        {                                                                       assert(end_smaller_than_pivot<begin_larger_than_pivot);
                                                                                #ifndef NDEBUG
                                                                                  { const state_in_block_pointer* sb;
                                                                                    BLC_list_const_iterator it=start_same_BLC;
                                                                                    assert(it<=end_equal_to_pivot);
                                                                                    for (; it<end_equal_to_pivot; ++it) {
                                                                                      assert(m_states[m_aut.get_transitions()[*it].from()].block->
                                                                                                                                   start_bottom_states==pivot);
                                                                                    }
                                                                                    assert(it<=end_smaller_than_pivot);
                                                                                    for (; it<end_smaller_than_pivot; ++it) {
                                                                                      assert(max_below_pivot<pivot);
                                                                                      sb=m_states[m_aut.get_transitions()[*it].from()].block->
                                                                                                                                           start_bottom_states;
                                                                                      assert(sb>=min_below_pivot);  assert(sb<=max_below_pivot);
                                                                                    }
                                                                                    assert(it<begin_larger_than_pivot);  assert(pivot<min_above_pivot);
                                                                                    sb=m_states[m_aut.get_transitions()[*it].from()].
                                                                                                                                    block->start_bottom_states;
                                                                                    assert(sb>=min_above_pivot);  assert(sb<=max_above_pivot);
                                                                                    for (it=begin_larger_than_pivot; it<end_same_BLC; ++it) {
                                                                                      sb=m_states[m_aut.get_transitions()[*it].from()].block->
                                                                                                                                           start_bottom_states;
                                                                                      assert(sb>=min_above_pivot);  assert(sb<=max_above_pivot);
                                                                                    }
                                                                                  }
                                                                                #endif
          --begin_larger_than_pivot;
          if (end_smaller_than_pivot>=begin_larger_than_pivot)
          {
            goto break_two_loops;
          }                                                                     mCRL2complexity(&m_transitions[*begin_larger_than_pivot], add_work(
                                                                                   check_complexity::order_BLC_transitions__sort_transition, max_sort), *this);
          state_in_block_pointer* const source_block=
              m_states[m_aut.get_transitions()
                 [*begin_larger_than_pivot].from()].block->start_bottom_states;
          if (source_block==pivot)
          {                                                                     assert(end_smaller_than_pivot<begin_larger_than_pivot);
            transition_index temp=*begin_larger_than_pivot;                     assert(end_equal_to_pivot<=end_smaller_than_pivot);
            *begin_larger_than_pivot=*end_smaller_than_pivot;
            *end_smaller_than_pivot=*end_equal_to_pivot;
            *end_equal_to_pivot=temp;
            ++end_equal_to_pivot;
            ++end_smaller_than_pivot;
            if (end_smaller_than_pivot>=begin_larger_than_pivot)
            {
              goto break_two_loops;
            }
            break;
          }
          if (source_block<pivot)
          {
            if (source_block<min_below_pivot)
            {
              min_below_pivot=source_block;
            }
            if (source_block>max_below_pivot)
            {
              max_below_pivot=source_block;
            }
            std::swap(*end_smaller_than_pivot, *begin_larger_than_pivot);
            ++end_smaller_than_pivot;
            if (end_smaller_than_pivot>=begin_larger_than_pivot)
            {
              goto break_two_loops;
            }
            break;
          }                                                                     assert(min_above_pivot<=max_above_pivot);
          if (source_block<min_above_pivot)
          {
            min_above_pivot=source_block;
          }
          else if (source_block>max_above_pivot)
          {
            max_above_pivot=source_block;
          }
        }
      }
      break_two_loops: ;                                                        assert(end_smaller_than_pivot==begin_larger_than_pivot);
                                                                                #ifndef NDEBUG
                                                                                  { const state_in_block_pointer* sb;
                                                                                    BLC_list_const_iterator it=start_same_BLC;
                                                                                    assert(it<=end_equal_to_pivot);
                                                                                    for (; it<end_equal_to_pivot; ++it) {
                                                                                      assert(m_states[m_aut.get_transitions()[*it].from()].block->
                                                                                                                                   start_bottom_states==pivot);
                                                                                    }
                                                                                    assert(it<=end_smaller_than_pivot);
                                                                                    for (; it<end_smaller_than_pivot; ++it) {
                                                                                      assert(max_below_pivot<pivot);
                                                                                      sb=m_states[m_aut.get_transitions()[*it].from()].block->
                                                                                                                                           start_bottom_states;
                                                                                      assert(sb>=min_below_pivot);  assert(sb<=max_below_pivot);
                                                                                    }
                                                                                    assert(it==begin_larger_than_pivot);
                                                                                    assert(it<=end_same_BLC);
                                                                                    for (; it<end_same_BLC; ++it) {
                                                                                      assert(pivot<min_above_pivot);
                                                                                      sb=m_states[m_aut.get_transitions()[*it].from()].block->
                                                                                                                                           start_bottom_states;
                                                                                      assert(sb>=min_above_pivot);  assert(sb<=max_above_pivot);
                                                                                    }
                                                                                  }
                                                                                #endif
      if (start_same_BLC<end_equal_to_pivot)
      {
        order_BLC_transitions_single_BLC_set(pivot,
                                           start_same_BLC, end_equal_to_pivot);
      }
      // Now try to use only tail recursion:
      if (min_above_pivot>=max_above_pivot)
      {
        if (begin_larger_than_pivot<end_same_BLC)
        {                                                                       assert(min_above_pivot==max_above_pivot);
          order_BLC_transitions_single_BLC_set(min_above_pivot,
                                        begin_larger_than_pivot, end_same_BLC);
        }
        if (end_equal_to_pivot<begin_larger_than_pivot)
        {
          order_BLC_transitions(end_equal_to_pivot, begin_larger_than_pivot,
                                             min_below_pivot, max_below_pivot);
        }
        return;
      }
      if (min_below_pivot>=max_below_pivot)
      {
        if (end_equal_to_pivot<begin_larger_than_pivot)
        {                                                                       assert(min_below_pivot==max_below_pivot);
          order_BLC_transitions_single_BLC_set(min_below_pivot,
                                  end_equal_to_pivot, begin_larger_than_pivot);
        }
        if (begin_larger_than_pivot<end_same_BLC)
        {
          order_BLC_transitions(begin_larger_than_pivot, end_same_BLC,
                                             min_above_pivot, max_above_pivot);
        }
        return;
      }                                                                         assert(end_equal_to_pivot<begin_larger_than_pivot);
                                                                                assert(min_below_pivot<max_below_pivot);
                                                                                assert(begin_larger_than_pivot<end_same_BLC);
      /* Here we cannot do tail recursion                                    */ assert(min_above_pivot<max_above_pivot);
      order_BLC_transitions(end_equal_to_pivot, begin_larger_than_pivot,
                                             min_below_pivot, max_below_pivot);
      // Hopefully the compiler turns this tail recursion into iteration
      order_BLC_transitions(begin_larger_than_pivot, end_same_BLC,
                                             min_above_pivot, max_above_pivot);
      #undef max_below_pivot
      #undef min_above_pivot
    }

    // Algorithm 4. Stabilize the current partition with respect to the current constellation
    // given that the blocks in m_blocks_with_new_bottom_states do contain new bottom states.
    // Stabilisation is always called after initialisation, i.e., m_aut.get_transitions()[ti].transition refers
    // to a position in m_BLC_transitions, where the transition index of this transition can be found.

    void stabilizeB()
    {
      if (m_blocks_with_new_bottom_states.empty())
      {
        return;
      }
      bool initial_stabilization=true;
      // Qhat contains the slices of BLC transitions that still need stabilization
      std::vector<std::pair<BLC_list_iterator, BLC_list_iterator> > Qhat;
                                                                                #if !defined(NDEBUG) || defined(COUNT_WORK_BALANCE)
                                                                                  std::vector<std::pair<BLC_list_const_iterator, BLC_list_const_iterator> >
                                                                                                                          initialize_qhat_work_to_assign_later;
                                                                                  std::vector<std::pair<BLC_list_const_iterator, BLC_list_const_iterator> >
                                                                                                                          stabilize_work_to_assign_later;
                                                                                #endif
      for (;;)
      {                                                                         // mCRL2complexity(all bottom states, add_work(..., 1), *this);
                                                                                    // not necessary, as the inner loop is always executed
                                                                                assert(!m_blocks_with_new_bottom_states.empty());
        for(block_type* const bi: m_blocks_with_new_bottom_states)
        {                                                                       assert(bi->contains_new_bottom_states);
                                                                                #if !defined(NDEBUG) || defined(COUNT_WORK_BALANCE)
//std::cerr << bi->debug_id(*this) << " has new bottom states.\n";
                                                                                  // The work in this loop is assigned to the (new) bottom states in bi
                                                                                  // It cannot be assigned to the block bi because there may be more new bottom
                                                                                  // states later.
                                                                                  const state_in_block_pointer* new_bott_it=bi->start_bottom_states;
                                                                                  assert(new_bott_it < bi->sta.rt_non_bottom_states);
                                                                                  do
                                                                                  {
                                                                                    mCRL2complexity(new_bott_it->ref_state,
                                                                                              add_work(check_complexity::stabilizeB__prepare_block, 1), *this);
                                                                                  }
                                                                                  while (++new_bott_it<bi->sta.rt_non_bottom_states);
                                                                                #endif
          bi->contains_new_bottom_states=false;
          if (1 >= number_of_states_in_block(*bi))
          {
            // blocks with only 1 state do not need to be stabilized further
//std::cerr << "    But it has only 1 state.\n";
            continue;
          }

          typename linked_list<BLC_indicators>::iterator
                                        ind=bi->block.to_constellation.begin();
                                                                                #ifndef NDEBUG
                                                                                  assert(!bi->block.to_constellation.empty());
                                                                                  assert(ind->start_same_BLC<ind->end_same_BLC);
                                                                                  const transition& first_t=m_aut.get_transitions()[*ind->start_same_BLC];
                                                                                  assert(m_states[first_t.from()].block==bi);
                                                                                  assert(is_inert_during_init(first_t) &&
                                                                                         bi->c.onstellation==m_states[first_t.to()].block->c.onstellation);
          /* The first BLC-set is constellation-inert, so skip it            */   assert(ind->is_stable());
                                                                                #endif
          ++ind;
          for (; bi->block.to_constellation.end()!=ind; ++ind)
          {
            if (!initial_stabilization && !ind->is_stable())
            {
//std::cerr << "    " << ind->debug_id(*this) << " is already unstable, it and its successors won't be added to Qhat again\n";
                                                                                #ifndef NDEBUG
              /* This is a new bottom block that was found during            */   // Check that all other BLC sets are already unstable
              /* stabilizeB().  Therefore, the subsequent BLC sets are       */   while (++ind!=bi->block.to_constellation.end())
              /* already somewhere in Qhat, and stabilizing for them two     */   {
              /* times is not needed.                                        */     assert(!ind->is_stable());
                                                                                    // marked transitions would start in new bottom states found
                                                                                    // earlier:
                                                                                    assert(!ind->has_marked_transitions());
                                                                                  }
                                                                                #endif
              break;
            }
//std::cerr << "    Preparing to stabilize under " << ind->debug_id(*this) << '\n';
            ind->start_marked_BLC=ind->end_same_BLC;
                                                                                #ifndef NDEBUG
                                                                                  assert(!ind->has_marked_transitions());
                                                                                  assert(ind->start_same_BLC<ind->end_same_BLC);
                                                                                  const transition& first_t = m_aut.get_transitions()[*ind->start_same_BLC];
                                                                                  assert(m_states[first_t.from()].block == bi);
              /* The BLC set transitions are not constellation-inert, so we  */   assert(!is_inert_during_init(first_t) ||
              /* need to stabilize under them                                */          bi->c.onstellation!=m_states[first_t.to()].block->c.onstellation);
                                                                                #endif
              Qhat.emplace_back(ind->start_same_BLC, ind->end_same_BLC);
                                                                                #if !defined(NDEBUG) || defined(COUNT_WORK_BALANCE)
                                                                                  // The work is assigned to the transitions out of new bottom states in ind.
                                                                                  // Try to find a new bottom state to which to assign it.
                                                                                  bool work_assigned = false;
                                                                                  // assign the work to the transitions out of bottom states in this BLC-set
                                                                                  for (BLC_list_const_iterator work_it = ind->start_same_BLC;
                                                                                                                          work_it<ind->end_same_BLC; ++work_it)
                                                                                  {
                                                                                    // assign the work to this transition
                                                                                    if (0==m_states[m_aut.get_transitions()
                                                                                                     [*work_it].from()].no_of_outgoing_block_inert_transitions)
                                                                                    {
                                                                                      #ifndef NDEBUG
                                                                                        if (work_assigned)
                                                                                        {
                                                                                          mCRL2complexity(&m_transitions[*work_it], add_work_notemporary(
                                                                                                     check_complexity::stabilizeB__initialize_Qhat, 1), *this);
                                                                                          continue;
                                                                                        }
                                                                                      #endif
                                                                                      mCRL2complexity(&m_transitions[*work_it], add_work(
                                                                                                     check_complexity::stabilizeB__initialize_Qhat, 1), *this);
                                                                                      work_assigned = true;
                                                                                      #ifdef NDEBUG
                                                                                        break;
                                                                                      #endif
                                                                                    }
                                                                                  }
                                                                                  if (!work_assigned)
                                                                                  {
                                                                                    // We register that we still have to find a transition from a new bottom
                                                                                    // state in this slice.
//std::cerr << "Haven't yet found a transition from a new bottom state in " << ind->debug_id(*this) << " to assign the initialization of Qhat to\n";
                                                                                    initialize_qhat_work_to_assign_later.emplace_back(ind->start_same_BLC,
                                                                                                                                      ind->end_same_BLC);
                                                                                  }
                                                                                #endif
          }

// 2. Administration: Mark all transitions out of (new) bottom states
          state_in_block_pointer* si=bi->start_bottom_states;                   assert(si<bi->sta.rt_non_bottom_states);
          do
          {                                                                     mCRL2complexity(si->ref_state, add_work(
                                                                                         check_complexity::stabilizeB__distribute_states_over_Phat, 1), *this);
            outgoing_transitions_it end_it=
                  std::next(si->ref_state)>=m_states.end()
                        ? m_outgoing_transitions.end()
                        : std::next(si->ref_state)->start_outgoing_transitions; assert(si->ref_state->block==bi);
            for (outgoing_transitions_it ti=
                    si->ref_state->start_outgoing_transitions; ti<end_it; ++ti)
            {                                                                   // mCRL2complexity(&m_transitions[m_BLC_transitions[ti->transition]],
                                                                                //                 add_work(..., 1), *this);
              const transition& t=                                                  // subsumed under the above counter
                             m_aut.get_transitions()[*ti->ref.BLC_transitions]; assert(m_states.begin()+t.from()==si->ref_state);
              if (!is_inert_during_init_if_branching(t) ||
                  bi->c.onstellation!=m_states[t.to()].block->c.onstellation)
              {
                // the transition is not constellation-inert, so mark it
                mark_BLC_transition(ti);
              }
              else
              {                                                                 assert(ti <= ti->start_same_saC);
                // skip all other constellation-inert transitions
                // (this is an optimization)
                ti = ti->start_same_saC;
              }
            }
            ++si;
          }
          while (si<bi->sta.rt_non_bottom_states);
        }
        clear(m_blocks_with_new_bottom_states);
        initial_stabilization=false;

// 3. As long as there are registered slices in m_BLC_transitions, select any one of them.
//    Take the first BLC_indicator that has transitions in this slice; remove it from the slice;
//    if the slice is now empty remove it from the register.
//    Do a normal splitB() under this splitter.
//    If more new bottom states are created, store them in the new m_blocks_with_new_bottom_states.

        // Algorithm 4, line 4.8.
        // inner loop to be executed until further new bottom states are found:
        do
        {
          if (Qhat.empty())
          {                                                                     assert(check_data_structures("End of stabilizeB()"));
            /* nothing needs to be stabilized any more.                      */ assert(check_stability("End of stabilizeB()"));
                                                                                // Therefore, it is impossible that further new bottom states are
                                                                                // found in these rounds.  So all work must have been accounted for:
                                                                                assert(initialize_qhat_work_to_assign_later.empty());
                                                                                assert(stabilize_work_to_assign_later.empty());
            return;
          }                                                                     // mCRL2complexity(..., add_work(..., max_C), *this);
          // Algorithm 4, line 4.9.                                                 // not needed as the inner loop is always executed at least once.
                                                                                //print_data_structures("New bottom state loop");
                                                                                assert(check_data_structures("New bottom state loop", false, false));
          std::pair<BLC_list_iterator,BLC_list_iterator>& Qhat_elt=Qhat.back(); assert(check_stability("New bottom state loop", &Qhat));
                                                                                assert(Qhat_elt.first<Qhat_elt.second);
          const linked_list<BLC_indicators>::iterator splitter=
                        m_transitions[*std::prev(Qhat_elt.second)].
                                        transitions_per_block_to_constellation; assert(splitter->end_same_BLC==Qhat_elt.second);
//std::cerr << "Now stabilizing under " << splitter->debug_id(*this) << '\n';
          Qhat_elt.second=splitter->start_same_BLC;                             assert(Qhat_elt.first<=Qhat_elt.second);
          if (Qhat_elt.first==Qhat_elt.second)
          {
            Qhat.pop_back(); // invalidates Qhat_elt
          }
                                                                                assert(splitter->start_same_BLC<splitter->end_same_BLC);
          const transition& first_t=
                            m_aut.get_transitions()[*splitter->start_same_BLC];
          block_type* const from_block_index=m_states[first_t.from()].block;    assert(!from_block_index->contains_new_bottom_states);
                                                                                #if !defined(NDEBUG) || defined(COUNT_WORK_BALANCE)
                                                                                  // The work is assigned to the transitions out of new bottom states in splitter.
                                                                                  bool work_assigned=false;
                                                                                  for (BLC_list_const_iterator work_it=splitter->start_same_BLC;
                                                                                                                     work_it<splitter->end_same_BLC; ++work_it)
                                                                                  {
                                                                                    // assign the work to this transition
                                                                                    if (0==m_states[m_aut.get_transitions()[*work_it].from()].
                                                                                                                        no_of_outgoing_block_inert_transitions)
                                                                                    {
                                                                                      #ifndef NDEBUG
                                                                                        if (work_assigned)
                                                                                        {
                                                                                          mCRL2complexity(&m_transitions[*work_it], add_work_notemporary(
                                                                                                           check_complexity::stabilizeB__main_loop, 1), *this);
                                                                                          continue;
                                                                                        }
                                                                                      #endif
                                                                                      mCRL2complexity(&m_transitions[*work_it],
                                                                                                  add_work(check_complexity::stabilizeB__main_loop, 1), *this);
                                                                                      work_assigned=true;
                                                                                      #ifdef NDEBUG
                                                                                        break;
                                                                                      #endif
                                                                                    }
                                                                                  }
                                                                                  if (!work_assigned)
                                                                                  {
                                                                                    // We register that we still have to find a transition from a new bottom
                                                                                    // state in this slice.
//std::cerr << "Haven't yet found a transition from a new bottom state in " << splitter->debug_id(*this) << " to assign the main loop work to\n";
                                                                                    stabilize_work_to_assign_later.emplace_back(splitter->start_same_BLC,
                                                                                                                                splitter->end_same_BLC);
                                                                                  }
                                                                                #endif
          if (std::distance(from_block_index->start_bottom_states,
                            from_block_index->end_states)<=1)
          {
            // a block with 1 state does not need to be split
            //splitter->make_stable();
//std::cerr << "No stabilization is needed because the source block contains only 1 state.\n";
          }
          else
          {                                                                     assert(!is_inert_during_init(first_t) || from_block_index->c.onstellation!=
                                                                                                                 m_states[first_t.to()].block->c.onstellation);
#ifndef USE_FOUR_WAY_SPLIT
            // Algorithm 4, line 4.10.
            state_in_block_pointer* first_unmarked_bottom_state=
                    not_all_bottom_states_are_touched(from_block_index,splitter
                                                                                #ifndef NDEBUG
                                                                                  , splitter->start_marked_BLC
                                                                                #endif
                        );
            if (first_unmarked_bottom_state<
                                    from_block_index->sta.rt_non_bottom_states)
            {
//std::cerr << "PERFORM A NEW BOTTOM STATE SPLIT\n";
              /* Algorithm 4, line 4.11, and implicitly 4.12, 4.13 and 4.18. */ assert(m_states[m_aut.get_transitions()[*splitter->start_same_BLC].from()].
                                                                                                                      block==m_states[first_t.from()].block);
              block_type* new_block;
              splitB(from_block_index, splitter,
                      first_unmarked_bottom_state, splitter->start_marked_BLC,
                            new_block, null_constellation, null_constellation);
            }
            else
            {
//std::cerr << "No split is needed because every bottom state has a transition in the splitter.\n";
              make_stable_and_move_to_start_of_BLC(from_block_index, splitter);
            }
#else
            four_way_splitB<false, true>(from_block_index, from_block_index->
                                       block.to_constellation.end(), splitter,
                                       null_constellation, null_constellation);
#endif
          }
        }
        while (m_blocks_with_new_bottom_states.empty());
                                                                                #if !defined(NDEBUG) || defined(COUNT_WORK_BALANCE)
                                                                                  // Further new bottom states have been found, so we now have a chance at
                                                                                  // assigning the initialization of Qhat that had not yet been assigned
                                                                                  // earlier.
                                                                                  for (std::vector<std::pair<BLC_list_const_iterator,BLC_list_const_iterator> >
                                                                                            ::iterator qhat_it=initialize_qhat_work_to_assign_later.begin();
                                                                                                         qhat_it!=initialize_qhat_work_to_assign_later.end(); )
                                                                                  {
                                                                                    bool new_bottom_state_with_transition_found=false;
                                                                                    for (BLC_list_const_iterator work_it=qhat_it->first;
                                                                                                                            work_it<qhat_it->second; ++work_it)
                                                                                    {
                                                                                      const state_index t_from=m_aut.get_transitions()[*work_it].from();
                                                                                      if (0==m_states[t_from].no_of_outgoing_block_inert_transitions)
                                                                                      {
                                                                                        // t_from is a new bottom state, so we can assign the work to this
                                                                                        // transition
                                                                                        #ifndef NDEBUG
                                                                                          if (new_bottom_state_with_transition_found)
                                                                                          {
                                                                                            mCRL2complexity(&m_transitions[*work_it], add_work_notemporary
                                                                                                           (check_complexity::
                                                                                                            stabilizeB__initialize_Qhat_afterwards, 1), *this);
                                                                                            continue;
                                                                                          }
                                                                                        #endif
                                                                                        mCRL2complexity(&m_transitions[*work_it], add_work(check_complexity::
                                                                                                            stabilizeB__initialize_Qhat_afterwards, 1), *this);
                                                                                        new_bottom_state_with_transition_found=true;
                                                                                        #ifdef NDEBUG
                                                                                          break;
                                                                                        #endif
                                                                                      }
                                                                                    }
                                                                                    if (new_bottom_state_with_transition_found)
                                                                                    {
                                                                                      // The work has been assigned successfully, so we can replace this
                                                                                      // entry of initialize_qhat_work_to_assign_later with the last one.
                                                                                      #ifndef NDEBUG
                                                                                        bool at_end=
                                                                                                std::next(qhat_it)==initialize_qhat_work_to_assign_later.end();
                                                                                      #endif
                                                                                      *qhat_it=initialize_qhat_work_to_assign_later.back();
                                                                                      initialize_qhat_work_to_assign_later.pop_back();
                                                                                      assert(at_end == (qhat_it==initialize_qhat_work_to_assign_later.end()));
                                                                                    }
                                                                                    else
                                                                                    {
                                                                                      ++qhat_it;
                                                                                    }
                                                                                  }

                                                                                  // We shall also try and find further new bottom states to which to assign
                                                                                  // the main loop iterations that had not yet been assigned earlier.
                                                                                  for (std::vector<std::pair<BLC_list_const_iterator,BLC_list_const_iterator> >
                                                                                            ::iterator stabilize_it=stabilize_work_to_assign_later.begin();
                                                                                                          stabilize_it!=stabilize_work_to_assign_later.end(); )
                                                                                  {
                                                                                    bool new_bottom_state_with_transition_found=false;
                                                                                    for (BLC_list_const_iterator work_it=stabilize_it->first;
                                                                                                                       work_it<stabilize_it->second; ++work_it)
                                                                                    {
                                                                                      const state_index t_from=m_aut.get_transitions()[*work_it].from();
                                                                                      if (0==m_states[t_from].no_of_outgoing_block_inert_transitions)
                                                                                      {
                                                                                        // t_from is a new bottom state, so we can assign the work to this
                                                                                        // transition
                                                                                        #ifndef NDEBUG
                                                                                          if (new_bottom_state_with_transition_found)
                                                                                          {
                                                                                            mCRL2complexity(&m_transitions[*work_it], add_work_notemporary(
                                                                                                check_complexity::stabilizeB__main_loop_afterwards, 1), *this);
                                                                                            continue;
                                                                                          }
                                                                                        #endif
                                                                                        mCRL2complexity(&m_transitions[*work_it], add_work(check_complexity::
                                                                                                                  stabilizeB__main_loop_afterwards, 1), *this);
                                                                                        new_bottom_state_with_transition_found=true;
                                                                                        #ifdef NDEBUG
                                                                                          break;
                                                                                        #endif
                                                                                      }
                                                                                    }
                                                                                    if (new_bottom_state_with_transition_found)
                                                                                    {
                                                                                      // The work has been assigned successfully, so we can replace this
                                                                                      // entry of stabilize_work_to_assign_later with the last one.
                                                                                      #ifndef NDEBUG
                                                                                        bool at_end=
                                                                                                 std::next(stabilize_it)==stabilize_work_to_assign_later.end();
                                                                                      #endif
                                                                                      *stabilize_it=stabilize_work_to_assign_later.back();
                                                                                      stabilize_work_to_assign_later.pop_back();
                                                                                      assert(at_end == (stabilize_it==stabilize_work_to_assign_later.end()));
                                                                                    }
                                                                                    else
                                                                                    {
                                                                                      ++stabilize_it;
                                                                                    }
                                                                                  }
                                                                                #endif
      }                                                                         assert(0); // unreachable
    }

    void create_initial_partition()
    {
      mCRL2log(log::verbose) << "An O(m log n) "
           << (m_branching ? (m_preserve_divergence
                                         ? "divergence-preserving branching "
                                         : "branching ")
                         : "")
           << "bisimulation partitioner created for " << m_aut.num_states()
           << " states and " << m_aut.num_transitions()
           << " transitions (using the experimental algorithm GJ2024).\n";
                                                                                #if !defined(NDEBUG) || defined(COUNT_WORK_BALANCE)
      /* Initialisation. */                                                       check_complexity::init(2 * m_aut.num_states());
                                                                                  // we need ``2*'' because there is one additional call to splitB during initialisation
                                                                                #endif
      group_transitions_on_tgt_label(m_aut);

      // Count the number of occurring action labels.
                                                                                assert((unsigned) m_preserve_divergence <= 1);
      mCRL2log(log::verbose) << "Start initialisation of the BLC list in the "
                                            "initialisation, after sorting.\n";
      constellation_type* const initial_constellation=
                #ifdef USE_POOL_ALLOCATOR
                    simple_list<BLC_indicators>::get_pool().
                    template construct<constellation_type>
                #else
                    new constellation_type
                #endif
                    (m_states_in_blocks.data(), m_states_in_blocks.data_end()); assert(1==no_of_constellations);
      block_type* const initial_block=
                #ifdef USE_POOL_ALLOCATOR
                    simple_list<BLC_indicators>::get_pool().
                    template construct<block_type>
                #else
                    new block_type
                #endif
                     (m_states_in_blocks.data(), m_states_in_blocks.data_end(),
                         m_states_in_blocks.data_end(), initial_constellation); assert(1==no_of_blocks);
      linked_list<BLC_indicators>
                              temporary_BLC_list=linked_list<BLC_indicators>();
      {
        std::vector<label_index> todo_stack_actions;
        std::vector<transition_index> count_transitions_per_action
             (m_aut.num_action_labels() + (unsigned) m_preserve_divergence, 0);
        if (m_branching)
        {
          // ensure that inert transitions come first and set the number of
          // transitions to a nonzero value so it doesn't trigger
          // todo_stack_actions.push_back(...) in the loop
          todo_stack_actions.push_back(m_aut.tau_label_index());
          count_transitions_per_action[m_aut.tau_label_index()] = 1;
        }
        for(transition_index ti=0; ti<m_aut.num_transitions(); ++ti)
        {
          const transition& t=m_aut.get_transitions()[ti];                      // mCRL2complexity(&m_transitions[ti], add_work(..., 1), *this);
                                                                                  // Because every transition is touched exactly once, we do not store a physical counter for this.
          const label_index label=label_or_divergence(t,
                                                    m_aut.num_action_labels()); assert(m_aut.apply_hidden_label_map(t.label())==t.label());
          transition_index& c=count_transitions_per_action[label];
          if (c==0)
          {
            todo_stack_actions.push_back(label);
          }
          c++;
        }
        if (m_branching)
        {
          assert(m_aut.is_tau(todo_stack_actions.front()));
          --count_transitions_per_action[m_aut.tau_label_index()];
        }
        accumulate_entries(count_transitions_per_action, todo_stack_actions);
        for (transition_index ti=0; ti<m_aut.num_transitions(); ++ti)
        {                                                                       // mCRL2complexity(&m_transitions[ti], add_work(..., 1), *this);
                                                                                  // Because every transition is touched exactly once, we do not store a physical counter for this.
          const transition& t=m_aut.get_transitions()[ti];
          const label_index label = label_or_divergence(t,
                                                    m_aut.num_action_labels());
          transition_index& c=count_transitions_per_action[label];              assert(0 <= c); assert(c < m_aut.num_transitions());
          m_BLC_transitions[c]=ti;
          c++;
        }

        // create BLC_indicators for every action label:
        std::vector<label_index>::const_iterator
                                               a_it=todo_stack_actions.begin();
        if (a_it!=todo_stack_actions.end() &&
            (0!=count_transitions_per_action[*a_it] ||                          (assert(m_branching), assert(m_aut.is_tau(*a_it)),
             ++a_it!=todo_stack_actions.end()))                                 )
        {
          BLC_list_iterator start_index=m_BLC_transitions.data();
          do
          {                                                                     // mCRL2complexity(..., add_work(..., 1), *this);
            const label_index a=*a_it;                                              // not needed because the inner loop is always executed
//std::cerr << "  Initialising m_BLC_transitions for action " << (m_aut.num_action_labels() == a ? "(tau-self-loops)" : pp(m_aut.action_label(a))) << '\n';
            const BLC_list_iterator end_index=
                      m_BLC_transitions.data()+count_transitions_per_action[a]; assert(end_index<=m_BLC_transitions.data_end());
            // create a BLC_indicator and insert it into the list...
            temporary_BLC_list.emplace_back(start_index, end_index, true);      assert(start_index<end_index);
            start_index=end_index;
          }
          while (++a_it!=todo_stack_actions.end());                             assert(start_index==m_BLC_transitions.data_end());
        }
        // destroy and deallocate `todo_stack_actions` and
        // `count_transitions_per_action` here.
      }

      // Group transitions per outgoing state.
      mCRL2log(log::verbose) << "Start setting outgoing transitions\n";
      {
        fixed_vector<transition_index> count_outgoing_transitions_per_state
                                                       (m_aut.num_states(), 0);
        for(const transition& t: m_aut.get_transitions())
        {                                                                       // mCRL2complexity(&m_transitions[std::distance
                                                                                //             (m_aut.get_transitions().data(), &t)], add_work(..., 1), *this);
          count_outgoing_transitions_per_state[t.from()]++;                         // Because every transition is touched exactly once,
          if (is_inert_during_init(t))                                              // we do not store a physical counter for this.
          {
            m_states[t.from()].no_of_outgoing_block_inert_transitions++;
          }
        }

        // We now set the outgoing transition per state pointer to the first
        // non-inert transition.
        // The counters for outgoing transitions calculated above are reset to
        // 0 and will later contain the number of transitions already stored.
        // Every time an inert transition is stored, the outgoing transition
        // per state pointer is reduced by one.
        outgoing_transitions_it current_outgoing_transitions=
                                                m_outgoing_transitions.begin();

        // place transitions and set pointers to incoming/outgoing transitions
        for (state_index s=0; s<m_aut.num_states(); ++s)
        {                                                                       // mCRL2complexity(&m_states[s], add_work(..., 1), *this);
          if (marked_range<=m_states[s].no_of_outgoing_block_inert_transitions)     // Because every state is touched exactly once,
          {                                                                         // we do not store a physical counter for this.
            mCRL2log(log::error) << "State " << s << " has "
                    << m_states[s].no_of_outgoing_block_inert_transitions
                    << " outgoing block-inert transitions.  However, the "
                       "four-way-split can handle at most " << (marked_range-1)
                    << " outgoing block-inert transitions per state.  "
                       "Aborting now.\n";
            exit(EXIT_FAILURE);
          }
          m_states[s].start_outgoing_transitions=current_outgoing_transitions+
                            m_states[s].no_of_outgoing_block_inert_transitions;
          current_outgoing_transitions+=
                                       count_outgoing_transitions_per_state[s];
          count_outgoing_transitions_per_state[s]=0;
          // meaning of this counter changes to: number of outgoing transitions
          // already stored
        }                                                                       assert(m_outgoing_transitions.end()==current_outgoing_transitions);

        mCRL2log(log::verbose) << "Moving incoming and outgoing transitions\n";

        if (!m_BLC_transitions.empty())
        {
          // This cannot be a normal for loop because m_BLC_transitions.end()
          // cannot be dereferenced, and std::prev(m_BLC_transitions.end()) is
          // only allowed if m_BLC_transitions is not empty.
          BLC_list_iterator ti=m_BLC_transitions.data();
          do
          {                                                                     // mCRL2complexity(&m_transitions[*ti], add_work(..., 1), *this);
            const transition& t=m_aut.get_transitions()[*ti];                       // Because every transition is touched exactly once,
            if (is_inert_during_init(t))                                            // we do not store a physical counter for this.
            {
              m_transitions[*ti].ref_outgoing_transitions =
                               --m_states[t.from()].start_outgoing_transitions;
            }
            else
            {
              m_transitions[*ti].ref_outgoing_transitions =
                        m_states[t.from()].start_outgoing_transitions +
                                count_outgoing_transitions_per_state[t.from()];
            }
            m_transitions[*ti].ref_outgoing_transitions->ref.transitions=*ti;
            ++count_outgoing_transitions_per_state[t.from()];
            ++ti;
          }
          while (ti<m_BLC_transitions.data_end());
        }
        // destroy and deallocate count_outgoing_transitions_per_state here.
      }

      state_index current_state=null_state;                                     assert(current_state + 1 == 0);
      // bool tau_transitions_passed=true;
      // TODO: This should be combined with another pass through all transitions.
      for(std::vector<transition>::iterator it=m_aut.get_transitions().begin();
                                       it!=m_aut.get_transitions().end(); it++)
      {                                                                         // mCRL2complexity(&m_transitions[std::distance
                                                                                //            (m_aut.get_transitions().begin(), it)], add_work(..., 1), *this);
        const transition& t=*it;                                                    // Because every transition is touched exactly once,
        if (t.to()!=current_state)                                                  // we do not store a physical counter for this.
        {
          for (state_index i=current_state+1; i<=t.to(); ++i)
          {                                                                     // ensure that every state is visited at most once:
                                                                                mCRL2complexity(&m_states[i], add_work(check_complexity::
                                                                                          create_initial_partition__set_start_incoming_transitions, 1), *this);
//std::cerr << "SET start_incoming_transitions for state " << i << "\n";
            m_states[i].start_incoming_transitions=it;
          }
          current_state=t.to();
        }
      }
      for (state_index i=current_state+1; i<m_aut.num_states(); ++i)
      {                                                                         mCRL2complexity(&m_states[i], add_work(check_complexity::
                                                                                          create_initial_partition__set_start_incoming_transitions, 1), *this);
//std::cerr << "SET residual start_incoming_transitions for state " << i << "\n";
        m_states[i].start_incoming_transitions=m_aut.get_transitions().end();
      }

      // Set the start_same_saC fields in m_outgoing_transitions.
      outgoing_transitions_it it = m_outgoing_transitions.end();
      if (m_outgoing_transitions.begin() < it)
      {
        --it;
        const transition& t=m_aut.get_transitions()[it->ref.transitions];
        state_index current_state = t.from();
        label_index current_label = label_or_divergence(t);
        outgoing_transitions_it current_end_same_saC = it;
        while (m_outgoing_transitions.begin() < it)
        {
          --it;                                                                 // mCRL2complexity(&m_transitions[*it->ref.BLC_transitions or
                                                                                //                              it->ref.transitions], add_work(..., 1), *this);
          const transition& t=m_aut.get_transitions()[it->ref.transitions];         // Because every transition is touched exactly once,
          const label_index new_label = label_or_divergence(t);                     // we do not store a physical counter for this.
          if (current_state == t.from() && current_label == new_label)
          {
            // We encounter a transition with the same saC.
            // Let it refer to the end.
            it->start_same_saC = current_end_same_saC;
          }
          else
          {
            // We encounter a transition with a different saC.
            current_state = t.from();
            current_label = new_label;
            current_end_same_saC->start_same_saC = std::next(it);
            current_end_same_saC = it;
          }
        }
        current_end_same_saC->start_same_saC = m_outgoing_transitions.begin();
      }
                                                                                assert(m_states_in_blocks.size()==m_aut.num_states());
//std::cerr << "Start filling states_in_blocks\n";
      state_in_block_pointer* lower_i=m_states_in_blocks.data();                assert(initial_block->start_bottom_states==lower_i);
      state_in_block_pointer* upper_i=m_states_in_blocks.data_end();            assert(initial_block->end_states==upper_i);
      for (fixed_vector<state_type_gj>::iterator i=m_states.begin();
                                                         i<m_states.end(); ++i)
      {                                                                         // mCRL2complexity(&m_states[i], add_work(..., 1), *this);
        if (0<i->no_of_outgoing_block_inert_transitions)                            // Because every state is touched exactly once,
        {                                                                           // we do not store a physical counter for this.
          --upper_i;
          upper_i->ref_state=i;
          i->ref_states_in_blocks=upper_i;
        }
        else
        {
          lower_i->ref_state=i;
          i->ref_states_in_blocks=lower_i;
          ++lower_i;
        }
        i->block=initial_block;
      }                                                                         assert(lower_i == upper_i);
      initial_block->sta.rt_non_bottom_states = lower_i;

      // Everything except `m_BLC_transitions` is now completely initialized.
                                                                                //print_data_structures("After initial reading before splitting in the initialisation", true);
                                                                                assert(check_data_structures("After initial reading before splitting in the initialisation", true, false));
      // The initial partition has been constructed. Continue with the initialisation.
      mCRL2log(log::verbose) << "Start refining in the initialisation\n";

      // We have not yet fully instantiated the BLC sets.
      // Therefore, we run a kind of simplified stabilisation: we do not need
      // to check the target constellation but only the action of the
      // transition.
      if (!temporary_BLC_list.empty())
      {
        linked_list<BLC_indicators>::iterator blc_it=
                                                    temporary_BLC_list.begin(); assert(blc_it->start_same_BLC<blc_it->end_same_BLC);
        if (!is_inert_during_init
                          (m_aut.get_transitions()[*blc_it->start_same_BLC]) ||
            ++blc_it!=temporary_BLC_list.end())
        {
          do
          {                                                                     // mCRL2complexity(blc_it, add_work(...), *this);
            std::vector<block_type*> blocks_that_need_refinement;                   // not needed because the inner loop is always executed at least once
            BLC_list_iterator trans_it=blc_it->start_same_BLC;                  assert(trans_it<blc_it->end_same_BLC);
                                                                                #if !defined(NDEBUG) || defined(COUNT_WORK_BALANCE)
                                                                                  const label_index a=label_or_divergence(m_aut.get_transitions()[*trans_it]);
                                                                                #endif
//std::cerr << "Now marking source states of transitions starting at " << m_transitions[*trans_it].debug_id(*this) << '\n';
            do
            {
              // mark the source state of *trans_it:
              const transition& t=m_aut.get_transitions()[*trans_it];           assert(label_or_divergence(t)==a);
              const state_in_block_pointer s(m_states.begin()+t.from());        // mCRL2complexity(&m_transitions[*trans_it], add_work(...), *this);
              block_type& B=*s.ref_state->block;                                    // because every transition is touched exactly once,
              if (nullptr==B.block.R)                                               // we do not include a physical counter for this
              {                                                                 assert(std::find(blocks_that_need_refinement.begin(),
                                                                                                 blocks_that_need_refinement.end(), s.ref_state->block)==
                                                                                                                            blocks_that_need_refinement.end());
                if (B.contains_new_bottom_states ||
                    number_of_states_in_block(B)<=1)
                {
                  continue;
                }
                B.block.R=new std::vector<state_in_block_pointer>();
                blocks_that_need_refinement.push_back(s.ref_state->block);
                // B.c.on.~constellation_and_new_bottom_states(); -- trivial
                B.c.first_unmarked_bottom_state=B.start_bottom_states;
              }                                                                 else  {  assert(std::find(blocks_that_need_refinement.begin(),
                                                                                                 blocks_that_need_refinement.end(), s.ref_state->block)!=
                                                                                                                         blocks_that_need_refinement.end());  }
              state_in_block_pointer* const
                                       pos_s=s.ref_state->ref_states_in_blocks; assert(B.start_bottom_states<=pos_s);  assert(pos_s<B.end_states);
              if (B.c.first_unmarked_bottom_state<=&*pos_s)
              {
                if (0==s.ref_state->no_of_outgoing_block_inert_transitions)
                {                                                               assert(pos_s<B.sta.rt_non_bottom_states);
                  swap_states_in_states_in_block
                                      (B.c.first_unmarked_bottom_state, pos_s); assert(undefined==s.ref_state->counter);
                  B.c.first_unmarked_bottom_state++;
                }
                else if (Rmarked!=s.ref_state->counter)
                {                                                               assert(undefined==s.ref_state->counter);
                  B.block.R->push_back(s);
                  s.ref_state->counter=Rmarked;                                 assert(B.sta.rt_non_bottom_states<=pos_s);
                }                                                               else  {  assert(B.sta.rt_non_bottom_states<=pos_s);  }
              }
            }
            while (++trans_it<blc_it->end_same_BLC);

            for (block_type* const bi : blocks_that_need_refinement)
            {                                                                   assert(nullptr!=bi->block.R);
//std::cerr << "Now refining " << bi->debug_id(*this) << '\n';
#ifdef USE_FOUR_WAY_SPLIT
                four_way_splitB<false, false>(bi,
                        linked_list<BLC_indicators>::end(), // no main splitter
                        linked_list<BLC_indicators>::end(), // no co-splitter
                        null_constellation, initial_constellation);
                    // The function will retrieve first_unmarked_bottom_state
                    // and the vector of potential ReachAlw-states from *bi.

                    // The only parameter that we still have to convey is the
                    // initial constellation, so four_way_splitB() can make
                    // the block look normal after it has retrieved the
                    // information.
#else
                                                                                assert(m_R.empty());
              m_R.swap_vec(*bi->block.R);
              delete bi->block.R;
              bi->block.R=nullptr;
              state_in_block_pointer* const first_unmarked_bottom_state=
                                             bi->c.first_unmarked_bottom_state; assert(bi->start_bottom_states<=first_unmarked_bottom_state);
              // prepare for next label:
              // destroy bi->c.first_unmarked_bottom_state; -- trivial
              new (&bi->c.on) block_type::
                    constellation_or_first_unmarked_bottom_state::
                    constellation_and_new_bottom_states(initial_constellation); assert(bi->start_bottom_states!=first_unmarked_bottom_state ||
                                                                                       !m_R.empty());
                                                                                #if !defined(NDEBUG) || defined(COUNT_WORK_BALANCE)
                                                                                  // two identical for loops, only the loop variable `s` is different.
                                                                                  for (const state_in_block_pointer*
                                                                                                s=bi->start_bottom_states; s!=first_unmarked_bottom_state; ++s)
                                                                                  { // assign the work to the a-transitions from *s->ref_state with label a
                                                                                    outgoing_transitions_const_it out_it_end=
                                                                                       std::next(s->ref_state)==m_states.end() ? m_outgoing_transitions.end()
                                                                                                         : std::next(s->ref_state)->start_outgoing_transitions;
                                                                                    outgoing_transitions_const_it
                                                                                                               out_it=s->ref_state->start_outgoing_transitions;
                                                                                    assert(out_it<out_it_end);
                                                                                    for (;;)
                                                                                    { assert(out_it<=out_it->start_same_saC);
                                                                                      const transition& t=m_aut.get_transitions()[out_it->ref.transitions];
                                                                                      if (label_or_divergence(t)==a)
                                                                                      { out_it_end=out_it->start_same_saC;
                                                                                        do
                                                                                        { mCRL2complexity(&m_transitions[out_it->ref.transitions], add_work(
                                                                                            check_complexity::create_initial_partition__refine_block,1),*this);
                                                                                        }
                                                                                        while (++out_it<=out_it_end);
                                                                                        break;
                                                                                      }
                                                                                      else
                                                                                      { // skip the complete saC slice
                                                                                        out_it=std::next(out_it->start_same_saC);
                                                                                        assert(out_it<out_it_end); // we should find some a-transitions
                                                                                      }
                                                                                    }
                                                                                  }
                                                                                  for (state_in_block_pointer s: m_R)
                                                                                  { // assign the work to the a-transitions from *s->ref_state with label a
                                                                                    outgoing_transitions_const_it out_it_end=
                                                                                       std::next(s.ref_state)==m_states.end() ? m_outgoing_transitions.end()
                                                                                                          : std::next(s.ref_state)->start_outgoing_transitions;
                                                                                    outgoing_transitions_const_it
                                                                                                                out_it=s.ref_state->start_outgoing_transitions;
                                                                                    assert(out_it<out_it_end);
                                                                                    for (;;)
                                                                                    { assert(out_it<=out_it->start_same_saC);
                                                                                      const transition& t=m_aut.get_transitions()[out_it->ref.transitions];
                                                                                      if (label_or_divergence(t)==a)
                                                                                      { out_it_end=out_it->start_same_saC;
                                                                                        do
                                                                                        { mCRL2complexity(&m_transitions[out_it->ref.transitions], add_work(
                                                                                            check_complexity::create_initial_partition__refine_block,1),*this);
                                                                                        }
                                                                                        while (++out_it<=out_it_end);
                                                                                        break;
                                                                                      }
                                                                                      else
                                                                                      { // skip the complete saC slice
                                                                                        out_it=std::next(out_it->start_same_saC);
                                                                                        assert(out_it<out_it_end); // we should find some a-transitions
                                                                                      }
                                                                                    }
                                                                                  }
                                                                                #endif
              if (first_unmarked_bottom_state<bi->sta.rt_non_bottom_states)
              {
                block_type* new_block;
                splitB<true>(bi, nullptr /* no splitter */,
                             first_unmarked_bottom_state,
                             nullptr /* no splitter_end_unmarked_BLC */,
                             new_block,
                             null_constellation, null_constellation,
                             false);
              }
              else
              {                                                                 assert(first_unmarked_bottom_state==bi->sta.rt_non_bottom_states);
                clear_state_counters(true);
                m_R.clear();
              }
#endif
            }
          }
          while (++blc_it!=temporary_BLC_list.end());
        }
      }
      if (!block_type::btc_R::
                         if_R_is_nullptr_then_to_constellation_is_empty_list())
      {
        // We need to explicitly convert the null pointers stored in block.R
        // to empty lists for block.to_constellation.
//std::cerr << "Initializing block.to_constellation lists one-by-one\n";
        state_in_block_pointer* st_it=m_states_in_blocks.data();                assert(m_states_in_blocks.data_end()!=st_it);
        do
        {
          block_type* const blk_it=st_it->ref_state->block;                     assert(nullptr==blk_it->block.R);
          // delete blk_it->block.R;  -- not needed, as it should be nullptr
          // destroy blk_it->block.R; -- not needed, as it is a pointer and
          //                             has a trivial destructor.
          new(&blk_it->block.to_constellation) linked_list<BLC_indicators>();
          st_it=blk_it->end_states;
        }
        while (m_states_in_blocks.data_end()!=st_it);
      }
      // Now create the correct BLC sets
      state_in_block_pointer* const
                    last_block_start=std::prev(m_states_in_blocks.end())->
                                         ref_state->block->start_bottom_states;
      linked_list<BLC_indicators>::iterator blc_it;
      while(blc_it=temporary_BLC_list.begin(),temporary_BLC_list.end()!=blc_it)
      {                                                                         // mCRL2complexity(blc_it, add_work(...), *this);
        order_BLC_transitions(blc_it->start_same_BLC, blc_it->end_same_BLC,         // Because every BLC set (= set of transitions with the same label)
                                  m_states_in_blocks.data(), last_block_start);     // is touched exactly once, we do not store a physicaal counter for it.
        // erase the elements from the list as we go (this is needed so the
        // pool allocator adds them to the free list)
        temporary_BLC_list.erase(blc_it);
      }
      // Algorithm 1, line 1.4 is implicitly done in the call to splitB above.

      /* Algorithm 1, line 1.5.                                              */ //print_data_structures("End initialisation");
                                                                                assert(check_stability("End initialisation"));
      mCRL2log(log::verbose) << "Start stabilizing in the initialisation\n";    assert(check_data_structures("End initialisation", false, false));

      end_initial_part=std::clock();

      stabilizeB();
    }

    /// \brief find a splitter for the tau-transitions from the new constellation to the old constellation
    /// \param index_block_B   block that forms the new constellation
    /// \param old_constellation  index of the old constellation
    /// \returns splitter that contains the tau-transitions from `index_block_B` to `old_constellation`
    /// \details If no such splitter exists,
    /// `linked_list<BLC_indicators>::end()` is returned.
    ///
    /// The function uses the fact that the first element of the list
    /// `block.to_constellation` contains the inert transitions (if there are
    /// any), and just after splitting the new constellation off from the old
    /// one, the element immediately after that the tau-transitions from the
    /// new to the old constellation.
    linked_list<BLC_indicators>::iterator find_inert_co_transition_for_block(
                       block_type* const index_block_B,
                       const constellation_type* const old_constellation,
                       const constellation_type* const new_constellation) const
    {
      linked_list< BLC_indicators >::iterator
                          btc_it=index_block_B->block.to_constellation.begin();
      if (btc_it == index_block_B->block.to_constellation.end())
      {
        // The new constellation has no outgoing transitions at all.
        return index_block_B->block.to_constellation.end();
      }                                                                         assert(btc_it->start_same_BLC<btc_it->end_same_BLC);
      const transition& btc_t=
                            m_aut.get_transitions()[*(btc_it->start_same_BLC)];
      if (!is_inert_during_init_if_branching(btc_t))
      {
        // The new constellation has no outgoing tau-transitions at all (except
        // possibly tau-self-loops, for divergence-preserving branching
        // bisimulation).
        return index_block_B->block.to_constellation.end();
      }
      if (m_states[btc_t.to()].block->c.onstellation==old_constellation)
      {
        // The new constellation has no inert transitions but it does have
        // tau-transitions to the old constellation (which were inert before).
        return btc_it;
      }
      if (m_states[btc_t.to()].block->c.onstellation!=new_constellation)
      {
        // The new constellation, before it was separated from the old one,
        // had no constellation-inert outgoing transitions.
        return index_block_B->block.to_constellation.end();
      }
      // *btc_it is the BLC_indicator for the inert transitions of the new
      // constellation.  Try the second element in the list:
      btc_it=index_block_B->block.to_constellation.next(btc_it);
      if (btc_it == index_block_B->block.to_constellation.end())
      {
        // The new constellation has no other outgoing transitions.
        return index_block_B->block.to_constellation.end();
      }                                                                         assert(btc_it->start_same_BLC<btc_it->end_same_BLC);
      const transition& btc2_t=
                            m_aut.get_transitions()[*(btc_it->start_same_BLC)];
      if (!is_inert_during_init_if_branching(btc2_t) ||
          old_constellation!=m_states[btc2_t.to()].block->c.onstellation)
      {
        // The new constellation has no tau-transitions to the old
        // constellation.
        return index_block_B->block.to_constellation.end();
      }
      return btc_it;
    }

    /// \brief Select a block that is not the largest block in a non-trivial constellation.
    /// \returns the index of such a block
    /// \details Either the first or the last block of a constellation is
    /// selected; also, the constellation bounds are adapted accordingly.
    /// However, the caller will have to create a new constellation and set the
    /// block's `constellation` field.
    ///
    /// To ensure the time complexity bounds, it is necessary that the
    /// block returned contains at most 50% of the states in its constellation.
    /// The smaller the better.
    block_type* select_and_remove_a_block_in_a_non_trivial_constellation()
    {                                                                           assert(!m_non_trivial_constellations.empty());
      // Do the minimal checking, i.e., only check two blocks in a constellation.
      constellation_type* const ci=m_non_trivial_constellations.back();
      block_type* index_block_B=ci->start_const_states->ref_state->block; // The first block.
      block_type* second_block_B=
                             std::prev(ci->end_const_states)->ref_state->block; // The last block.

      if (number_of_states_in_block(*index_block_B)<=
                                    number_of_states_in_block(*second_block_B))
      {
        ci->start_const_states=index_block_B->end_states;
      }
      else
      {
        ci->end_const_states=second_block_B->start_bottom_states;
        index_block_B=second_block_B;
      }
      return index_block_B;
    }

// =================================================================================================================================
//
//   refine_partition_until_it_becomes_stable.
//
// =================================================================================================================================

    /// \brief number of new bottom states found after constructing the initial partition
    /// \details This count includes all states that were non-bottom state in
    /// the (unstable) trivial partition with a single block.
    state_index no_of_new_bottom_states;

    /// \brief number of non-inert BLC sets in the partition
    /// \details The sets that are in `m_BLC_indicators_to_be_deleted` are not
    /// included in this count.  Nor are sets that contain constellation-inert
    /// transitions.
    transition_index no_of_non_constellation_inert_BLC_sets;

    void refine_partition_until_it_becomes_stable()
    {
      // This represents the while loop in Algorithm 1 from line 1.6 to 1.25.

      // The instruction below has complexity O(|Act|);
      // calM will contain the m_BLC_transitions slices that need stabilization:
      std::vector<std::pair<BLC_list_iterator, BLC_list_iterator> > calM;
      // Algorithm 1, line 1.6.
      clock_t next_print_time = clock();
      const clock_t rounded_start_time = next_print_time - CLOCKS_PER_SEC/2;
      while (true)
      {                                                                         //print_data_structures("MAIN LOOP");
                                                                                assert(check_data_structures("MAIN LOOP"));
                                                                                assert(check_stability("MAIN LOOP"));
        if (mCRL2logEnabled(log::verbose))
        {
          if (std::clock_t now = std::clock(); next_print_time <= now ||
                                          m_non_trivial_constellations.empty())
          {

            /* -  -  -  -  -print progress information-  -  -  -  - */

            // The formula below should ensure that `next_print_time`
            // increases by a whole number of minutes, so that the
            // progress information is printed every minute (or, if
            // one iteration takes more than one minute, after a whole
            // number of minutes).
            next_print_time+=((now-next_print_time)/(60*CLOCKS_PER_SEC)
                                                    + 1) * (60*CLOCKS_PER_SEC);
            now = (now - rounded_start_time) / CLOCKS_PER_SEC;
            if (0 != now)
            {
              if (60 <= now)
              {
                if (3600 <= now)
                {
                    mCRL2log(log::verbose) << now / 3600 << " h ";
                    now %= 3600;
                }
                mCRL2log(log::verbose) << now / 60 << " min ";
                now %= 60;
              }
              mCRL2log(log::verbose) << now
                              << " sec passed since starting the main loop.\n";
            }
            #define PRINT_SG_PL(counter, sg_string, pl_string) \
                      (counter) << (1 == (counter) ? (sg_string) : (pl_string))
            mCRL2log(log::verbose)
              << (m_non_trivial_constellations.empty()
                                        ? "The reduced LTS contains "
                                        : "The reduced LTS contains at least ")
              << PRINT_SG_PL(no_of_blocks, " state and ", " states and ")
              << PRINT_SG_PL(no_of_non_constellation_inert_BLC_sets,
                                              " transition.", " transitions.");
            if (1 < no_of_blocks)
            {
              #define PRINT_INT_PERCENTAGE(num,denom) \
                                        (((num) * 200 + (denom)) / (denom) / 2)
              mCRL2log(log::verbose) << " Estimated "
                << PRINT_INT_PERCENTAGE(no_of_constellations - 1,
                                                no_of_blocks - 1)
                << "% done.";
              #undef PRINT_INT_PERCENTAGE
            }
            mCRL2log(log::verbose)
            //  << " Logarithmic estimate: "
            //  << (int)(100.5+std::log((double) no_of_constellations/
            //                      no_of_blocks)
            //                  *log_initial_nr_of_blocks)
            //  << "% done."
                << "\nThe current partition contains ";
            if (m_branching)
            {
              mCRL2log(log::verbose)
                  << PRINT_SG_PL(no_of_new_bottom_states,
                                " new bottom state, ", " new bottom states, ");
            }                                                                   else  assert(0 == no_of_new_bottom_states);
            mCRL2log(log::verbose)
              << PRINT_SG_PL(no_of_constellations,
                     " constellation (of which ", " constellations (of which ")
              << PRINT_SG_PL(m_non_trivial_constellations.size(),
                                  " is nontrivial).\n", " are nontrivial).\n");
            #undef PRINT_SG_PL
          }
        }
        if (m_non_trivial_constellations.empty())
        {
          break;
        }
        // Algorithm 1, line 1.7.
        block_type* index_block_B=
                    select_and_remove_a_block_in_a_non_trivial_constellation();
        constellation_type* const old_constellation=
                                                 index_block_B->c.onstellation;
//std::cerr << "REMOVE " << index_block_B->debug_id(*this) << " from " << old_constellation->debug_id(*this) << "\n";

        // Algorithm 1, line 1.8.
        if (old_constellation->start_const_states->ref_state->block==
            std::prev(old_constellation->end_const_states)->ref_state->block)
        {                                                                       assert(m_non_trivial_constellations.back()==old_constellation);
          // Constellation has become trivial.
          m_non_trivial_constellations.pop_back();
        }
        constellation_type* const new_constellation=
                #ifdef USE_POOL_ALLOCATOR
                    simple_list<BLC_indicators>::get_pool().
                    template construct<constellation_type>
                #else
                    new constellation_type
                #endif
                                          (index_block_B->start_bottom_states,
                                                    index_block_B->end_states);
        ++no_of_constellations;
                                                                                #if !defined(NDEBUG) || defined(COUNT_WORK_BALANCE)
        /* Block index_block_B is moved to the new constellation but we shall*/   // new_constellation->work_counter=old_constellation->work_counter;
        /* not yet assign                                                    */
        /* index_block_B->c.onstellation=new_constellation;                  */   unsigned char const max_C=check_complexity::log_n-check_complexity::
                                                                                                  ilog2(number_of_states_in_constellation(*new_constellation));
                                                                                  mCRL2complexity(index_block_B, add_work(check_complexity::
                                                                                       refine_partition_until_it_becomes_stable__find_splitter, max_C), *this);
                                                                                #endif
        // Here the variables block.to_constellation and the doubly linked list
        // L_B->C in blocks must be still be updated.
        // This happens further below.

        for (state_in_block_pointer* i=index_block_B->start_bottom_states;
                                             i!=index_block_B->end_states; ++i)
        {                                                                       // mCRL2complexity(m_states[*i], add_work(..., max_C), *this);
          // and visit the incoming transitions.                                    // subsumed under the above counter
          const std::vector<transition>::iterator end_it=
                   (std::next(i->ref_state)==m_states.end())
                         ? m_aut.get_transitions().end()
                         : std::next(i->ref_state)->start_incoming_transitions;
          for(std::vector<transition>::iterator
                    j=i->ref_state->start_incoming_transitions; j!=end_it; ++j)
          {
            const transition& t=*j;
            const transition_index t_index=
                             std::distance(m_aut.get_transitions().begin(), j);
            // Update the state-action-constellation (saC) references in        // mCRL2complexity(&m_transitions[t_index], add_work(..., max_C), *this);
            // m_outgoing_transitions.                                              // subsumed under the above counter
            const outgoing_transitions_it old_pos=
                               m_transitions[t_index].ref_outgoing_transitions;
            const outgoing_transitions_it end_same_saC=
                            old_pos->start_same_saC < old_pos
                                           ? old_pos : old_pos->start_same_saC;
            const outgoing_transitions_it new_pos=end_same_saC->start_same_saC; assert(m_states[t.from()].start_outgoing_transitions<=new_pos);
                                                                                assert(new_pos<=old_pos);
            if (old_pos != new_pos)
            {
              std::swap(old_pos->ref.BLC_transitions,
                        new_pos->ref.BLC_transitions);
              m_transitions[*old_pos->ref.BLC_transitions].
                                              ref_outgoing_transitions=old_pos;
              m_transitions[*new_pos->ref.BLC_transitions].
                                              ref_outgoing_transitions=new_pos;
            }
            if (new_pos < end_same_saC)
            {
              end_same_saC->start_same_saC = std::next(new_pos);
            }
            // correct start_same_saC provisionally: make them at least point
            // at each other.  In the new saC-slice, all transitions point to
            // the first one, except the first one: that shall point at the
            // last one.
            new_pos->start_same_saC = new_pos;
            if (m_states[t.from()].start_outgoing_transitions<new_pos)
            {
              // Check if t is the first transition in the new saC slice:
              const transition& prev_t = m_aut.get_transitions()
                                    [*std::prev(new_pos)->ref.BLC_transitions];
              assert(prev_t.from() == t.from());
              if (m_states[prev_t.to()].block == index_block_B &&
                  label_or_divergence(prev_t) == label_or_divergence(t))
              {
                // prev_t also belongs to the new saC slice.
                new_pos->start_same_saC = std::prev(new_pos)->start_same_saC;   assert(m_states[t.from()].start_outgoing_transitions<=new_pos->start_same_saC);
                                                                                assert(new_pos->start_same_saC<new_pos);
                                                                                assert(std::prev(new_pos)==new_pos->start_same_saC->start_same_saC);
                new_pos->start_same_saC->start_same_saC = new_pos;
              }
            }
          }
        }
        calM.clear();

        // Walk through all states in block B
        for (state_in_block_pointer* i=index_block_B->start_bottom_states;
                                             i!=index_block_B->end_states; ++i)
        {                                                                       // mCRL2complexity(m_states[*i], add_work(..., max_C), *this);
          // and visit the incoming transitions.                                    // subsumed under the above counter
          const std::vector<transition>::iterator end_it=
                  (std::next(i->ref_state)==m_states.end())
                         ? m_aut.get_transitions().end()
                         : std::next(i->ref_state)->start_incoming_transitions;
          for(std::vector<transition>::iterator
                    j=i->ref_state->start_incoming_transitions; j!=end_it; ++j)
          {
            const transition& t=*j;
            const transition_index t_index=
                             std::distance(m_aut.get_transitions().begin(), j); assert(m_states[t.to()].block == index_block_B);
            bool source_block_is_singleton=
                     (1>=number_of_states_in_block(*m_states[t.from()].block)); // mCRL2complexity(&m_transitions[t_index], add_work(..., max_C), *this);
                                                                                    // subsumed under the above counter
            // Give the saC slice of this transition its final correction
            const outgoing_transitions_it out_pos=
                               m_transitions[t_index].ref_outgoing_transitions;
            const outgoing_transitions_it start_new_saC=
                                                       out_pos->start_same_saC;
            if (start_new_saC < out_pos)
            {
              // not the first transition in the saC-slice
              if (out_pos < start_new_saC->start_same_saC)
              {
                // not the last transition in the saC-slice
                out_pos->start_same_saC = start_new_saC->start_same_saC;
              }
            }

#ifndef USE_FOUR_WAY_SPLIT
            // mark one cotransition from the same source state (if it exists)
            // If the main transitions are constellation-inert (i.e. they go
            // from index_block_B to index_block_B), we shall not stabilize
            // under them, so we shall also not mark these transitions.
            // (This is also needed to ensure that the BLC set for the special
            // new-bottom-state split does not contain any spurious markings.)
            // If the main transitions start in the old constellation, the
            // co-transitions are constellation-inert and we do not need to
            // stabilize under them.

            else if (!m_states[t.from()].block->contains_new_bottom_states &&
                     !source_block_is_singleton &&
                     (!is_inert_during_init(t) ||
                      (/* m_states[t.from()].block != index_block_B && */
                                                                                assert(index_block_B->c.onstellation==old_constellation),
                       m_states[t.from()].block->c.onstellation!=
                                                           old_constellation)))
            {
              // This is the first transition in the saC-slice
              // It is not inert, and it has not just become non-inert.
              // (Note that the constellation of index_block_B has not yet been changed.)
              // Now we set the pointer to a co-splitter.
              const outgoing_transitions_it start_old_saC=
                                                      std::next(start_new_saC);
              if (start_old_saC < (t.from()+1 >= m_aut.num_states()
                            ? m_outgoing_transitions.end()
                            : m_states[t.from()+1].start_outgoing_transitions))
              {                                                                 assert(start_old_saC<=start_old_saC->start_same_saC);
                const transition& old_t=m_aut.get_transitions()
                                         [*start_old_saC->ref.BLC_transitions]; assert(old_t.from() == t.from());
                if (label_or_divergence(t) == label_or_divergence(old_t) &&
                    old_constellation==
                                    m_states[old_t.to()].block->c.onstellation)
                {
//std::cerr << "Marking " << m_transitions[*start_old_saC->ref.BLC_transitions].debug_id(*this) << " as a cotransition.\n";
                  BLC_indicators& ind=*m_transitions[t_index].
                                        transitions_per_block_to_constellation;
                  if (ind.is_stable())
                  {
                    ind.make_unstable();
                    // The co-splitter will be moved to its position at the end
                    // of the BLC list later.
                  }
                  mark_BLC_transition(start_old_saC);
                }
              }
            }
#endif

            // Update the doubly linked list L_B->C in blocks as the constellation is split in B and C\B.
            if (update_the_doubly_linked_list_LBC_new_constellation(index_block_B, t, t_index) &&
                !source_block_is_singleton &&
                (!is_inert_during_init(t) || index_block_B!=m_states[t.from()].block))
            {
              // a new BLC set has been constructed, insert its start position into calM.
              // (unless the source block is a singleton)
              BLC_list_iterator BLC_pos = m_transitions[t_index].ref_outgoing_transitions->ref.BLC_transitions;
              assert(t_index == *BLC_pos);
              calM.emplace_back(BLC_pos, BLC_pos);
//std::cerr << "This transition is in a main splitter.\n";
              // The end-position (the second element in the pair) will need to be corrected later.
            }
          }
        }
        index_block_B->c.onstellation=new_constellation;

        // mark all states in main splitters and correct the end-positions of
        // calM entries
        for (std::vector<std::pair<BLC_list_iterator, BLC_list_iterator> >::
                        iterator calM_elt=calM.begin(); calM_elt!=calM.end(); )
        {
          linked_list <BLC_indicators>::iterator ind=m_transitions
                     [*calM_elt->first].transitions_per_block_to_constellation; mCRL2complexity(ind, add_work(check_complexity::
                                                                                   refine_partition_until_it_becomes_stable__correct_end_of_calM,max_C),*this);
//std::cerr << "Checking whether " << ind->debug_id(*this) << " is a splitter: ";
          /* check if all transitions were moved to the new constellation,   */ assert(ind->start_same_BLC==calM_elt->first);
          /* or some transitions to the old constellation have remained:     */ assert(!ind->has_marked_transitions());
          const transition& last_t=
                        m_aut.get_transitions()[*std::prev(ind->end_same_BLC)]; assert(m_states[last_t.to()].block->c.onstellation==new_constellation);
                                                                                assert(ind->start_same_BLC<ind->end_same_BLC);
          const transition* next_t=nullptr;
          if ((is_inert_during_init(last_t) &&
               m_states[last_t.from()].block->c.onstellation==
                                                        old_constellation &&
               (                                                                assert(m_states[last_t.from()].block!=index_block_B),
//std::cerr << "yes, it was constellation-inert earlier but is no more\n",
                                                                      true)) ||
              (ind->end_same_BLC<m_BLC_transitions.data_end() &&
               (next_t=&m_aut.get_transitions()[*ind->end_same_BLC],
                m_states[last_t.from()].block==m_states[next_t->from()].block&&
                label_or_divergence(last_t)==label_or_divergence(*next_t) &&
                old_constellation==
                                m_states[next_t->to()].block->c.onstellation
//&& (std::cerr << "yes, there are transitions in the corresponding co-splitter\n", true)
                                                                            )))
          {
            // there are some transitions to the corresponding co-splitter,
            // so we will have to stabilize the block
            calM_elt->second = ind->end_same_BLC;
#ifndef USE_FOUR_WAY_SPLIT
            ind->make_unstable();
            ind->start_marked_BLC=ind->start_same_BLC;                          // The mCRL2complexity call above assigns work to every transition in ind,
                                                                                // so we are allowed to mark all transitions at once.
            if (nullptr!=next_t)
            {
              // the co-splitter has already been marked as unstable if there
              // are transitions from bottom states, but it needs to be marked
              // always
              const linked_list<BLC_indicators>::iterator co_ind=
                              m_transitions[*ind->end_same_BLC].
                                        transitions_per_block_to_constellation;
              if (co_ind->is_stable())
              {
                co_ind->make_unstable();
              }                                                                 else  {  assert(co_ind->has_marked_transitions());  }
                                                                                assert(co_ind->start_same_BLC==ind->end_same_BLC);
            }
#endif
            ++calM_elt;
          }
          else
          {
//std::cerr << "no, all transitions in the old BLC set have moved to the new BLC set\n";
            // all transitions in the old BLC set have moved to the new BLC
            // set; as the old BLC set was stable, so is the new one.
                                                                                #ifndef NDEBUG
            /* We can skip this element.                                     */   bool at_end=std::next(calM_elt)==calM.end();
                                                                                #endif
            calM_elt->first=calM.back().first;
            calM.pop_back();                                                    assert(at_end == (calM_elt==calM.end()));
          }
        }

        // ---------------------------------------------------------------------------------------------
        // First carry out a co-split of B with respect to C\B and an action tau.
        if (m_branching)
        {
          linked_list<BLC_indicators>::iterator tau_co_splitter=
           find_inert_co_transition_for_block(index_block_B,
                                         old_constellation, new_constellation);

          // Algorithm 1, line 1.19.
          if (index_block_B->block.to_constellation.end()!=tau_co_splitter)
          {
            // The tau co-splitter contains transitions that have just become
            // non-inert.
            ++no_of_non_constellation_inert_BLC_sets;
//std::cerr << " There is a tau-co-splitter";
            if (number_of_states_in_block(*index_block_B) > 1)
            {
#ifndef USE_FOUR_WAY_SPLIT
              tau_co_splitter->make_unstable();                                 // We have to give credit for marking all transitions in the splitter at once:
              tau_co_splitter->start_marked_BLC=
                                               tau_co_splitter->start_same_BLC; mCRL2complexity(tau_co_splitter, add_work(check_complexity::
                                                                                     refine_partition_until_it_becomes_stable__prepare_cosplit, max_C), *this);
              // The routine below has a side effect, as it sets m_R for all bottom states of block B.
              state_in_block_pointer* first_unmarked_bottom_state=
               not_all_bottom_states_are_touched(index_block_B, tau_co_splitter
                                                                                #ifndef NDEBUG
                                                                                  , tau_co_splitter->start_marked_BLC
                                                                                #endif
                            );
              if (first_unmarked_bottom_state<
                                       index_block_B->sta.rt_non_bottom_states)
              {                                                                 assert(m_states[m_aut.get_transitions()
                                                                                              [*tau_co_splitter->start_same_BLC].from()].block==index_block_B);
              // Algorithm 1, line 1.10.
                block_type* new_block;
//std::cerr << "DO A TAU CO SPLIT " << old_constellation << "\n";
                splitB(index_block_B, tau_co_splitter,
                        first_unmarked_bottom_state,
                        tau_co_splitter->start_marked_BLC,
                        new_block,
                        old_constellation, // needed, because index_block_B
                              // might be split again later under other labels.
                        new_constellation
                        );
              }
              else
              {
                tau_co_splitter->make_stable();
                // no need to move the splitter to the beginning of the list,
                // as we never moved it from there anyway.
              }
#else
                                                                                assert(tau_co_splitter->is_stable());
//std::cerr << "DO A TAU CO SPLIT " << old_constellation << "\n";
              four_way_splitB<true, false>(index_block_B, tau_co_splitter,
                        index_block_B->block.to_constellation.end(),
                        old_constellation, // needed, because index_block_B
                              // might be split again later under other labels.
                        new_constellation
                        );
#endif
//std::cerr << "The new constellation now consists of the blocks:\n";
//for (const state_in_block_pointer* blk_it=new_constellation->start_const_states;
//  blk_it!=new_constellation->end_const_states;
//  blk_it=blk_it->ref_state->block->end_states) { std::cerr << "    " << blk_it->ref_state->block->debug_id(*this) << " with states {";
//  for (const state_in_block_pointer* st_it=blk_it->ref_state->block->start_bottom_states; ; ++st_it) { if(st_it==blk_it->ref_state->block->sta.rt_non_bottom_states) std::cerr << " |"; if (st_it==blk_it->ref_state->block->end_states) break; std::cerr << ' ' << st_it->ref_state->debug_id_short(*this); }
//  std::cerr << " }\n"; }
            }
          }
//else std::cerr << "There is no tau co-splitter\n";
        }
        // Algorithm 1, line 1.10.
        for (std::pair<BLC_list_iterator, BLC_list_iterator> calM_elt: calM)
        {                                                                       // mCRL2complexity(..., add_work(..., max_C), *this);
                                                                                    // not needed as the inner loop is always executed at least once.
                                                                                //print_data_structures("Main loop");
                                                                                assert(check_stability("Main loop", &calM, &calM_elt, old_constellation, new_constellation));
                                                                                assert(check_data_structures("Main loop", false, false));
          /* Algorithm 1, line 1.11.                                         */ assert(calM_elt.first < calM_elt.second);
          do
          {
            linked_list<BLC_indicators>::iterator splitter=
                          m_transitions[*std::prev(calM_elt.second)].
                                        transitions_per_block_to_constellation; mCRL2complexity(splitter, add_work(check_complexity::
                                                                                    refine_partition_until_it_becomes_stable__execute_main_split,max_C),*this);
                                                                                assert(splitter->start_same_BLC<calM_elt.second);
                                                                                assert(splitter->end_same_BLC==calM_elt.second);
                                                                                #ifndef USE_FOUR_WAY_SPLIT
                                                                                  assert(!splitter->is_stable());
                                                                                  assert(splitter->start_same_BLC<=splitter->start_marked_BLC);
                                                                                  assert(splitter->start_marked_BLC<=splitter->end_same_BLC);
                                                                                #endif
            calM_elt.second = splitter->start_same_BLC;                         assert(splitter->start_same_BLC<splitter->end_same_BLC);

            const transition& first_t=
                            m_aut.get_transitions()[*splitter->start_same_BLC];
            const label_index a=label_or_divergence(first_t);                   assert(m_states[first_t.to()].block->c.onstellation==new_constellation);
            block_type* Bpp=m_states[first_t.from()].block;                     assert(Bpp->c.onstellation!=new_constellation ||
                                                                                       !is_inert_during_init(first_t));
//std::cerr << "INVESTIGATE ACTION " << (m_aut.num_action_labels()==a ? "(tau-self-loops)" : pp(m_aut.action_label(a))) << ", source block==" << Bpp->debug_id(*this) << ", target constellation==" << new_constellation->debug_id(*this) << ", splitter==" << splitter->debug_id(*this) << '\n';
            if (number_of_states_in_block(*Bpp) <= 1)
            {
              // a block with 1 state does not need to be split
//std::cerr << "No main/co-split is needed because the block contains only 1 state.\n";
              //splitter->make_stable();
              // The following would require to calculate the co-splitter even
              // in this case:
              //if (Bpp->block.to_constellation.end()!=co_splitter)
              //{
              //  co_splitter->make_stable();
              //}
            }
            else if (Bpp->contains_new_bottom_states)
            {
              // The block Bpp contains new bottom states, and it is not
              // necessary to spend any work on it now.
              // We will later stabilize it in stabilizeB().
//std::cerr << "No main/co-split is needed because the block contains new bottom states.\n";
              //splitter->make_stable();
              // The following would require to calculate the co-splitter even
              // in this case:
              //if (Bpp->block.to_constellation.end()!=co_splitter)
              //{
              //  co_splitter->make_stable();
              //}
            }
            else
            {
#ifndef USE_FOUR_WAY_SPLIT
                                                                                assert(splitter->has_marked_transitions());
                                                                                assert(splitter->start_marked_BLC==splitter->start_same_BLC);
              linked_list<BLC_indicators>::iterator co_splitter=
                                    Bpp->block.to_constellation.prev(splitter);
              if (Bpp->block.to_constellation.end()!=co_splitter)
              {                                                                 assert(co_splitter->start_same_BLC<co_splitter->end_same_BLC);
//std::cerr << "Current co-splitter candidate is: " << co_splitter->debug_id(*this);
                const transition& co_t=m_aut.get_transitions()
                                                [*co_splitter->start_same_BLC]; assert(m_states[co_t.from()].block==Bpp);
                if (a!=label_or_divergence(co_t) ||
                    old_constellation!=
                                     m_states[co_t.to()].block->c.onstellation)
                {
                  // This is not the real co-splitter.
                  co_splitter=Bpp->block.to_constellation.end();
//std::cerr << " -- not the true co-splitter";
                }
                else if (is_inert_during_init(co_t) &&
                         Bpp->c.onstellation==old_constellation)
                {                                                               assert(co_splitter->is_stable());
                  // the co-splitter contains constellation-inert
                  // transitions, so no stabilisation under this co-splitter is needed.
                  co_splitter=Bpp->block.to_constellation.end();
//std::cerr << " -- constellation-inert, so no co-split is needed";
                }
//std::cerr << '\n';
              }
              state_in_block_pointer* first_unmarked_bottom_state=
                      not_all_bottom_states_are_touched(Bpp, splitter
                                                                                #ifndef NDEBUG
                                                                                  , splitter->start_marked_BLC
                                                                                #endif
                        );
              if (first_unmarked_bottom_state<Bpp->sta.rt_non_bottom_states)
              {                                                                 assert(m_states[m_aut.get_transitions()[*splitter->start_same_BLC].from()].
                                                                                                                  block==Bpp);
                const BLC_list_iterator_or_null co_splitter_begin=
                        Bpp->block.to_constellation.end()==co_splitter
                                       ? nullptr : co_splitter->start_same_BLC;
                BLC_list_iterator_or_null co_splitter_end=
                        Bpp->block.to_constellation.end()==co_splitter
                                         ? nullptr : co_splitter->end_same_BLC;
//std::cerr << "PERFORM A MAIN SPLIT\n";
              // Algorithm 1, line 1.12.
                block_type* new_block;
                Bpp = splitB(Bpp, splitter,
                                        first_unmarked_bottom_state,
                                        splitter->start_marked_BLC,
                                        new_block,
                                        old_constellation, new_constellation);
                if (linked_list<BLC_indicators>::end()!=co_splitter)
                {
                  // we need to use linked_list<BLC_indicators>::end(), the
                  // static function, because it is unclear whether Bpp is
                  // still a valid block (it could be that all states in the
                  // R-subblock are new bottom states). But even in that case
                  // we have to declare the co-splitter part of the U-subblock
                  // stable, so we cannot just skip the loop that comes here:

                  // The former co-splitter has been separated into up to three parts:
                  // one with transitions starting in the R-subblock (that will be the true co-splitter);
                  // one with transitions starting in the U-subblock (that actually is already stable);
                  // one with transitions starting in new bottom states and their predecessors.
                  co_splitter=linked_list<BLC_indicators>::end();               assert(co_splitter_begin<co_splitter_end);
                                                                                #ifndef NDEBUG
                                                                                  int number_of_iterations=0;
                                                                                #endif
                  do
                  {
                                                                                #ifndef NDEBUG
                                                                                  // ensure that there are at most 3 iterations in this loop
                                                                                  // (to keep time complexity within O(1)).
                                                                                  ++number_of_iterations; assert(number_of_iterations<=3);
                                                                                #endif
                    linked_list<BLC_indicators>::iterator candidate=
                              m_transitions[*std::prev(co_splitter_end)].
                                        transitions_per_block_to_constellation;
                    co_splitter_end=candidate->start_same_BLC;
                    const transition& t=
                           m_aut.get_transitions()[*candidate->start_same_BLC]; assert(m_states[t.to()].block->c.onstellation==old_constellation);
                    block_type* const from_block=m_states[t.from()].block;
                    if (from_block==Bpp)
                    {                                                           assert(null_block!=Bpp);
                      /* This is the part that starts in the R-subblock      */ assert(linked_list<BLC_indicators>::end()==co_splitter);
//std::cerr << "Co-splitter updated to " << candidate->debug_id(*this) << '\n';
                      co_splitter=candidate;
                    }
                    else if (from_block->contains_new_bottom_states)
                    {
                      // This is the part that starts in the new bottom block.
                      // Don't do anything.
//std::cerr << candidate->debug_id(*this) << " was (part of) the co-splitter but now starts in the new bottom block.\n";
                      //candidate->make_stable(); // forget state markings
                    }
                    else
                    {
                      // This is the part that starts in the U-subblock.
                      // It is actually already stable.
                      // So we should mark it as such and move it to the
                      /* beginning of its list of BLC sets.                  */ assert(!candidate->has_marked_transitions());
//std::cerr << candidate->debug_id(*this) << " was (part of) the co-splitter but now starts in the U-subblock.\n";
                      if (!candidate->is_stable()) // sometimes the checking of
                            // co-splitters turns this co-splitter part stable,
                            // as it never has a corresponding splitter
                      {
                        candidate->make_stable();
                      }
                    }
                  }
                  while (co_splitter_begin<co_splitter_end);
                }
                if (null_block == Bpp)
                {
                  // all bottom states in the R-subblock are new bottom states.
                  // Then no co-split is needed to stabilize the old bottom
                  // states in the R-block!
                  continue;
                }
                // Algorithm 1, line 1.13.
                // Algorithm 1, line 1.14 is implicitly done in the call of
                // splitB above.
              }
              else
              {
//std::cerr << "No main split is needed because all bottom states have a transition in the splitter.\n";
                splitter->make_stable();
              }                                                                 assert(!Bpp->contains_new_bottom_states);
              // Algorithm 1, line 1.17 and line 1.18.
              if (Bpp->block.to_constellation.end()!=co_splitter)
              {                                                                 assert(!co_splitter->is_stable());
//std::cerr << "BLOCK THAT IS CO-SPLIT " << Bpp << "\n";
                // Algorithm 1, line 1.19.
                if (1>=number_of_states_in_block(*Bpp))
                {
//std::cerr << "No co-split is needed because the remaining block contains only 1 state.\n";
                  //co_splitter->make_stable();
                }
                else
                {
                  first_unmarked_bottom_state=
                      not_all_bottom_states_are_touched(Bpp, co_splitter
                                                                                #ifndef NDEBUG
                                                                                  , co_splitter->start_marked_BLC
                                                                                #endif
                            );
                  if(first_unmarked_bottom_state<Bpp->sta.rt_non_bottom_states)
                  {                                                             assert(m_states[m_aut.get_transitions()[*co_splitter->start_same_BLC].from()].
                                                                                                                                        block==Bpp);
                    block_type* new_block;
//std::cerr << "PERFORM A CO-SPLIT\n";
                    splitB(Bpp, co_splitter,
                        first_unmarked_bottom_state,
                        co_splitter->start_marked_BLC,
                        new_block,
                        old_constellation, new_constellation);
                  }
                  else
                  {
//std::cerr << "No co-split is needed because all bottom states have a transition in the co-splitter.\n";
                    co_splitter->make_stable();
                  }
                }
                // Algorithm 1, line 1.20 and 1.21. P is updated implicitly when splitting Bpp.
              }
              else
              {                                                                 // check that there is really no co-splitter
                                                                                #ifndef NDEBUG
                                                                                  if (m_branching && m_aut.is_tau(a) &&
                                                                                      Bpp->c.onstellation==old_constellation)
                                                                                  {
//std::cerr << "No co-split is needed because the co-splitting transitions are constellation-inert.\n";
                                                                                  }
                                                                                  else
                                                                                  {
                                                                                    for (const BLC_indicators& ind : Bpp->block.to_constellation)
                                                                                    {
                                                                                      assert(ind.start_same_BLC<ind.end_same_BLC);
                                                                                      const transition& co_t=m_aut.get_transitions()[*ind.start_same_BLC];
                                                                                      assert(m_states[co_t.from()].block==Bpp);
                                                                                      assert(label_or_divergence(co_t)!=a ||
                                                                                             m_states[co_t.to()].block->c.onstellation!=old_constellation);
                                                                                    }
//std::cerr << "No co-split is needed because there is no co-splitter.\n";
                                                                                  }
                                                                                #endif
              }
#else
                                                                                assert(splitter->is_stable());
              if (is_inert_during_init(first_t) &&
                  old_constellation==Bpp->c.onstellation)
              {
                // The co-splitter would be constellation-inert, so no co-split
                // is needed
                four_way_splitB<true, false>(Bpp, splitter,
                                Bpp->block.to_constellation.end(),
                                         old_constellation, new_constellation);
              }
              else
              {
                linked_list<BLC_indicators>::iterator co_splitter=
                                    Bpp->block.to_constellation.prev(splitter);
                const transition* co_t;
                if (Bpp->block.to_constellation.end()!=co_splitter &&
                    (                                                           assert(co_splitter->is_stable()),
                                                                                assert(co_splitter->start_same_BLC<co_splitter->end_same_BLC),
                     co_t=&m_aut.get_transitions()
                                                [*co_splitter->start_same_BLC], assert(m_states[co_t->from()].block==Bpp),
                     a==label_or_divergence(*co_t) &&
                     old_constellation==
                                   m_states[co_t->to()].block->c.onstellation))
                {
//std::cerr << "Co-splitter: " << co_splitter->debug_id(*this) << '\n';
                  four_way_splitB<true, true>(Bpp, splitter, co_splitter,
                                         old_constellation, new_constellation);
                }
//else std::cerr << "No stabilization needed because the main splitter of non-inert transitions has no co-splitter.\n";
              }
#endif
            }
          }
          while (calM_elt.first < calM_elt.second);
        }                                                                       //print_data_structures("Before stabilize");
                                                                                assert(check_data_structures("Before stabilize", false, false));
                                                                                assert(check_stability("Before stabilize"));
        stabilizeB();
      }
                                                                                #if !defined(NDEBUG) || defined(COUNT_WORK_BALANCE)
                                                                                  check_complexity::print_grand_totals();
                                                                                #endif
    }

  public:
    /// time measurement after the end of create_initial_partition()
    std::clock_t end_initial_part;

    /// \brief constructor
    /// \details The constructor constructs the data structures and immediately
    /// calculates the partition corresponding with the bisimulation quotient.
    /// It destroys the transitions on the LTS (to save memory) but does not
    /// adapt the LTS to represent the quotient's transitions.
    /// It is assumed that there are no tau-loops in aut.
    /// \param aut                 LTS that needs to be reduced
    /// \param branching           If true branching bisimulation is used,
    ///                            otherwise strong bisimulation is
    ///                            applied.
    /// \param preserve_divergence If true and branching is true, preserve
    ///                            tau loops on states.
    bisim_partitioner_gj(LTS_TYPE& aut,
                         const bool branching = false,
                         const bool preserve_divergence = false)
      : m_aut(aut),
        m_states(aut.num_states()),
        m_outgoing_transitions(aut.num_transitions()),
        m_transitions(aut.num_transitions()),
        m_states_in_blocks(aut.num_states()),
        no_of_blocks(1),
        no_of_constellations(1),
        m_BLC_transitions(aut.num_transitions()),
        m_branching(branching),
        m_preserve_divergence(preserve_divergence),
        no_of_new_bottom_states(0),
        no_of_non_constellation_inert_BLC_sets(0)
    {                                                                           assert(m_branching || !m_preserve_divergence);
//log::logger::set_reporting_level(log::debug);
      mCRL2log(log::verbose) << "Start initialisation.\n";
      create_initial_partition();
      mCRL2log(log::verbose) << "After initialisation there are "
              << no_of_blocks << " equivalence classes. Start refining. \n";
      refine_partition_until_it_becomes_stable();                               assert(check_data_structures("READY"));
    }
};

/* ************************************************************************* */
/*                                                                           */
/*                             I N T E R F A C E                             */
/*                                                                           */
/* ************************************************************************* */


/// \brief nonmember functions serving as interface with the rest of mCRL2
/// \details These functions are copied, almost without changes, from
/// liblts_bisim_gw.h, which was written by Anton Wijs.

/// \brief Reduce transition system l with respect to strong or
/// (divergence-preserving) branching bisimulation.
/// \param[in,out] l                   The transition system that is reduced.
/// \param         branching           If true branching bisimulation is
///                                    applied, otherwise strong bisimulation.
/// \param         preserve_divergence Indicates whether loops of internal
///                                    actions on states must be preserved.  If
///                                    false these are removed.  If true these
///                                    are preserved.
template <class LTS_TYPE>
void bisimulation_reduce_gj(LTS_TYPE& l, const bool branching = false,
                                         const bool preserve_divergence = false)
{
    if (1 >= l.num_states())
    {
        mCRL2log(log::warning) << "There is only 1 state in the LTS. It is not "
                "guaranteed that branching bisimulation minimisation runs in "
                "time O(m log n).\n";
    }

    const std::clock_t start_SCC=std::clock();

    // Line 1.2: Find tau-SCCs and contract each of them to a single state
    mCRL2log(log::verbose) << "Start SCC\n";
    if (branching)
    {
        scc_reduce(l, preserve_divergence);
    }

    const std::clock_t start_part=std::clock();

    // Now apply the branching bisimulation reduction algorithm.  If there
    // are no taus, this will automatically yield strong bisimulation.
    mCRL2log(log::verbose) << "Start Partitioning\n";
    bisim_partitioner_gj<LTS_TYPE> bisim_part(l,branching,preserve_divergence);

    const std::clock_t end_part=std::clock();

    // Assign the reduced LTS
    mCRL2log(log::verbose) << "Start finalizing\n";
    bisim_part.finalize_minimized_LTS();

    if (mCRL2logEnabled(log::verbose))
    {
        const std::clock_t end_finalizing=std::clock();

        //auto old_reporting_level=log::logger::get_reporting_level();
        //log::logger::set_reporting_level(log::verbose);

        const int prec = std::lrint(std::log10(CLOCKS_PER_SEC) + 0.19897000433602);
            // For example, if CLOCKS_PER_SEC>=     20: >=2 digits
            //              If CLOCKS_PER_SEC>=    200: >=3 digits
            //              If CLOCKS_PER_SEC>=2000000: >=7 digits

        mCRL2log(log::verbose) << "Running times measured using "
            #ifdef USE_FOUR_WAY_SPLIT
                "the four-way split"
                #ifdef USE_INCLUDE_CO
                    " with NcludeCo"
                    #ifdef XCLUDECO_AND_HIT_MAIN
                        " and XcludeCo_and_Hit_Main"
                    #endif
                #else
                    #ifdef XCLUDECO_AND_HIT_MAIN
                        " with XcludeCo_and_Hit_Main"
                    #endif
                #endif
            #else
                "the two-way split"
            #endif
            #if !defined(NDEBUG) || defined(COUNT_WORK_BALANCE)
                " and with counting the cancelled steps in the coroutines"
            #endif
            "\n";

        double runtime[5];
        runtime[0]=(double) (end_finalizing                        -                        start_SCC)/CLOCKS_PER_SEC; // total time
        runtime[1]=(double) (                                                    start_part-start_SCC)/CLOCKS_PER_SEC;
        runtime[2]=(double) (                        bisim_part.end_initial_part-start_part          )/CLOCKS_PER_SEC;
        runtime[3]=(double) (               end_part-bisim_part.end_initial_part                     )/CLOCKS_PER_SEC;
        runtime[4]=(double) (end_finalizing-end_part                                                 )/CLOCKS_PER_SEC;
        if (runtime[0]>=60.0)
        {
            int min[sizeof(runtime)/sizeof(runtime[0])];
            for (unsigned i = 0; i < sizeof(runtime)/sizeof(runtime[0]); ++i)
            {
                min[i] = trunc(runtime[i] / 60.0);
                runtime[i] -= 60 * min[i];
            }
            if (min[0]>=60)
            {
                int h[sizeof(runtime)/sizeof(runtime[0])];
                for (unsigned i = 0; i < sizeof(runtime)/sizeof(runtime[0]); ++i)
                {
                    h[i] = min[i] / 60;
                    min[i] %= 60;
                }
                int width = trunc(log10(h[0])) + 1;

                mCRL2log(log::verbose) << std::fixed << std::setprecision(prec)
                    << "Time spent on contracting SCCs: " << std::setw(width) << h[1] << "h " << std::setw(2) << min[1] << "min " << std::setw(prec+3) << runtime[1] << "s\n"
                       "Time spent on initial partition:" << std::setw(width) << h[2] << "h " << std::setw(2) << min[2] << "min " << std::setw(prec+3) << runtime[2] << "s\n"
                       "Time spent on stabilize+refine: " << std::setw(width) << h[3] << "h " << std::setw(2) << min[3] << "min " << std::setw(prec+3) << runtime[3] << "s\n"
                       "Time spent on finalizing:       " << std::setw(width) << h[4] << "h " << std::setw(2) << min[4] << "min " << std::setw(prec+3) << runtime[4] << "s\n"
                       "Total CPU time:                 " << std::setw(width) << h[0] << "h " << std::setw(2) << min[0] << "min " << std::setw(prec+3) << runtime[0] << "s\n"
                    << std::defaultfloat;
            }
            else
            {
                mCRL2log(log::verbose) << std::fixed << std::setprecision(prec)
                    << "Time spent on contracting SCCs: " << std::setw(2) << min[1] << "min " << std::setw(prec+3) << runtime[1] << "s\n"
                       "Time spent on initial partition:" << std::setw(2) << min[2] << "min " << std::setw(prec+3) << runtime[2] << "s\n"
                       "Time spent on stabilize+refine: " << std::setw(2) << min[3] << "min " << std::setw(prec+3) << runtime[3] << "s\n"
                       "Time spent on finalizing:       " << std::setw(2) << min[4] << "min " << std::setw(prec+3) << runtime[4] << "s\n"
                       "Total CPU time:                 " << std::setw(2) << min[0] << "min " << std::setw(prec+3) << runtime[0] << "s\n"
                    << std::defaultfloat;
            }
        }
        else
        {
            mCRL2log(log::verbose) << std::fixed << std::setprecision(prec)
                << "Time spent on contracting SCCs: " << std::setw(prec+3) << runtime[1] << "s\n"
                   "Time spent on initial partition:" << std::setw(prec+3) << runtime[2] << "s\n"
                   "Time spent on stabilize+refine: " << std::setw(prec+3) << runtime[3] << "s\n"
                   "Time spent on finalizing:       " << std::setw(prec+3) << runtime[4] << "s\n"
                   "Total CPU time:                 " << std::setw(prec+3) << runtime[0] << "s\n"
                << std::defaultfloat;
        }

        //log::logger::set_reporting_level(old_reporting_level);
    }
}


/// \brief Checks whether the two initial states of two LTSs are strong or
/// (divergence-preserving) branching bisimilar.
/// \details This routine uses the O(m log n) branching bisimulation algorithm
/// developed in 2018 by David N. Jansen.  It runs in O(m log n) time and uses
/// O(m) memory, where n is the number of states and m is the number of
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
/// \param         generate_counter_examples  (non-functional, only in the
///                                    interface for historical reasons)
/// \returns True iff the initial states of the transition systems l1 and l2
/// are ((divergence-preserving) branching) bisimilar.
template <class LTS_TYPE>
bool destructive_bisimulation_compare_gj(LTS_TYPE& l1, LTS_TYPE& l2,
        const bool branching = false, const bool preserve_divergence = false,
        const bool generate_counter_examples = false,
        const std::string& /*counter_example_file*/ = "",
        bool /*structured_output*/ = false)
{
    if (generate_counter_examples)
    {
        mCRL2log(log::warning) << "The GJ24 branching bisimulation "
                              "algorithm does not generate counterexamples.\n";
    }
    std::size_t init_l2(l2.initial_state() + l1.num_states());
    detail::merge(l1, std::move(l2));
    l2.clear(); // No use for l2 anymore.

    // Line 2.1: Find tau-SCCs and contract each of them to a single state
    if (branching)
    {
        detail::scc_partitioner<LTS_TYPE> scc_part(l1);
        scc_part.replace_transition_system(preserve_divergence);
        init_l2 = scc_part.get_eq_class(init_l2);
    }                                                                           else  {  assert(!preserve_divergence);  }
                                                                                assert(1 < l1.num_states());
    bisim_partitioner_gj<LTS_TYPE>bisim_part(l1,branching,preserve_divergence);

    return bisim_part.in_same_class(l1.initial_state(), init_l2);
}


/// \brief Checks whether the two initial states of two LTSs are strong or
/// (divergence-preserving) branching bisimilar.
/// \details The LTSs l1 and l2 are first duplicated and subsequently reduced
/// modulo bisimulation.  If memory is a concern, one could consider to use
/// destructive_bisimulation_compare().  This routine uses the O(m log n)
/// branching bisimulation algorithm developed in 2018 by David N. Jansen.  It
/// runs in O(m log n) time and uses O(m) memory, where n is the number of
/// states and m is the number of transitions.
/// \param l1                  A first transition system.
/// \param l2                  A second transistion system.
/// \param branching           If true branching bisimulation is used,
///                            otherwise strong bisimulation is applied.
/// \param preserve_divergence If true and branching is true, preserve tau
///                            loops on states.
/// \retval True iff the initial states of the transition systems l1 and l2
/// are ((divergence-preserving) branching) bisimilar.
template <class LTS_TYPE>
inline bool bisimulation_compare_gj(const LTS_TYPE& l1, const LTS_TYPE& l2,
          const bool branching = false, const bool preserve_divergence = false)
{
    LTS_TYPE l1_copy(l1);
    LTS_TYPE l2_copy(l2);
    return destructive_bisimulation_compare_gj(l1_copy, l2_copy, branching,
                                                          preserve_divergence);
}


} // end namespace detail
} // end namespace lts
} // end namespace mcrl2

#endif // ifndef LIBLTS_BISIM_GJ_H
