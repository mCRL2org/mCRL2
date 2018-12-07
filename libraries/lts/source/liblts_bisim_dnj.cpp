// Author(s): David N. Jansen, Institute of Software, Chinese Academy of
// Sciences, Beijing, PR China
//
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/// \file lts/detail/liblts_bisim_dnj.cpp
///
/// \brief O(m log n)-time branching bisimulation algorithm
///
/// \details This file implements an efficient partition refinement algorithm
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

// The file is best viewed on a screen or in a window that is 160 characters
// wide.  The left 80 columns contain the main text of the program.  The right
// 80 columns contain assertions and other code used for debugging.

#include "mcrl2/lts/detail/liblts_bisim_dnj.h"
#include "mcrl2/lts/lts_lts.h"
#include "mcrl2/lts/lts_aut.h"
#include "mcrl2/lts/lts_fsm.h"
#include "mcrl2/lts/detail/liblts_scc.h"
#include "mcrl2/lts/detail/liblts_merge.h"
#include "mcrl2/lts/detail/stackless_coroutine.h"
#include "mcrl2/lts/detail/check_complexity.h"
#include "mcrl2/lts/detail/fixed_vector.h"
// #include "mcrl2/lts/transition.h"
// #include "mcrl2/lts/lts_utilities.h"

#include <list>  // for the list of block_bunch-slices
#include <algorithm> // for std::lower_bound()

namespace mcrl2
{
namespace lts
{
namespace detail
{
                                                                                #ifndef NDEBUG
                                                                                    /// \brief include something in Debug mode
                                                                                    /// \details In a few places, we have to include an additional parameter to
                                                                                    /// a function in Debug mode.  While it is in principle possible to use
                                                                                    /// #ifndef NDEBUG ... #endif, that would lead to distributing the code
                                                                                    /// over many code lines.  This macro expands to its arguments in Debug
// state_type and trans_type are defined in check_complexity.h.                     /// mode and to nothing otherwise.
                                                                                    #define ONLY_IF_DEBUG(...) __VA_ARGS__
                                                                                #else
                                                                                    #define ONLY_IF_DEBUG(...)
                                                                                #endif
/// \brief type used to store label numbers and counts
typedef std::size_t label_type;

namespace bisim_dnj
{

template <class LTS_TYPE> class bisim_partitioner_dnj;

/// \class iterator_or_counter
/// \brief union: either iterator or counter (for initialisation)
/// \details During initialisation, we need some counters to count the number
/// of transitions for each state.  To avoid allocating separate memory for
/// these counters, we store their value in the same place where we later
/// store an iterator.
///
/// We assume that each such variable starts out as a counter and at some point
/// becomes an iterator.  That point is marked by
/// calling convert_to_iterator().  The structure may only be destroyed
/// afterwards, as the destructor assumes it's an iterator.
template <class Iterator>
union iterator_or_counter
{
    trans_type count;
    Iterator begin;

    /// \brief Construct the object as a counter
    iterator_or_counter()  {  count = 0;  }


    /// \brief Convert the object from counter to iterator
    void convert_to_iterator(const Iterator& other)
    {
        new(&begin) Iterator(other);
    }


    /// \brief Destruct the object as an iterator
    ~iterator_or_counter()  {  begin.~Iterator();  }
};


/// \class iterator_or_null
/// \brief union: either iterator or nullptr
/// \details In some places in the data structures, we sometimes want to store
/// an iterator but sometimes nothing.  However, iterators normally cannot hold
/// something equivalent to a null pointer, so this union provides this
/// functionality.  It relies on the implementation detail that an iterator is
/// a non-null pointer, which is checked through assertions.
template <class Iterator>
union iterator_or_null
{
  private:
    const void* null;
    Iterator iter;
  public:
    /// \brief Construct an object containing no iterator
    iterator_or_null()
    {
        null = nullptr;
    }


    /// \brief Construct an object containing a valid iterator
    explicit iterator_or_null(Iterator new_iter)
    {
        new (&iter) Iterator(new_iter);                                         assert(nullptr != null);
    }


    /// \brief Destruct an object (whether it contains a valid iterator or not)
    ~iterator_or_null()
    {
        if (!is_null())  iter.~Iterator();
    }


    /// \brief Compare the object with an iterator
    /// \details If the object does not contain a valid iterator, it compares
    /// unequal with the iterator.
    bool operator==(const Iterator& other) const
    {
        return (sizeof(null) == sizeof(iter) || !is_null()) && iter == other;
    }


    bool operator!=(const Iterator& other) const
    {
        return !operator==(other);
    }


    /// \brief Compare the object with a non-null pointer
    /// \details If the object does not contain a valid iterator, it compares
    /// unequal with the pointer.
    bool operator==(const typename Iterator::value_type* const other) const
    {                                                                           assert(nullptr != other);
        return (sizeof(null) == sizeof(iter) || !is_null()) && &*iter == other;
    }


    bool operator!=(const typename Iterator::value_type* const other) const
    {
        return !operator==(other);
    }


    /// \brief Check whether the object contains a valid iterator
    bool is_null() const  {  return nullptr == null;  }


    /// \brief Clear the valid iterator stored in the object
    void clear()
    {                                                                           assert(!is_null());
        iter.~Iterator();
        null = nullptr;
    }


    /// \brief Set an object, which does not contain an iterator, to a valid
    /// iterator
    void set(Iterator new_iter)
    {                                                                           assert(is_null());
        new (&iter) Iterator(new_iter);                                         assert(!is_null());
    }


    /// \brief Change the valid iterator stored in the object to a new value
    void change(Iterator new_iter)
    {                                                                           assert(!is_null());
        iter = new_iter;                                                        assert(!is_null());
    }


    /// \brief Read the valid value of the iterator
    const Iterator& operator()() const
    {                                                                           assert(!is_null());
        return iter;
    }
};

class block_bunch_entry;
typedef bisim_gjkw::fixed_vector<block_bunch_entry>::iterator
                                                            block_bunch_iter_t;
typedef bisim_gjkw::fixed_vector<block_bunch_entry>::const_iterator
                                                      block_bunch_const_iter_t;

class action_block_entry;
typedef bisim_gjkw::fixed_vector<action_block_entry>::iterator
                                                           action_block_iter_t;
typedef bisim_gjkw::fixed_vector<action_block_entry>::const_iterator
                                                     action_block_const_iter_t;





/* ************************************************************************* */
/*                                                                           */
/*                   R E F I N A B L E   P A R T I T I O N                   */
/*                                                                           */
/* ************************************************************************* */





class state_info_entry;

typedef bisim_gjkw::fixed_vector<state_info_entry>::iterator state_info_iter_t;
typedef bisim_gjkw::fixed_vector<state_info_entry>::const_iterator
                                                       state_info_const_iter_t;

/// \class permutation_t
/// \brief stores a permutation of the states, ordered by block
/// \details This is the central concept of the _refinable partition_: the
/// permutation of the states, such that states belonging to the same block are
/// adjacent.
///
/// Iterating over the states of a block will
/// therefore be done using the permutation_t array.
typedef bisim_gjkw::fixed_vector<state_info_iter_t> permutation_t;
typedef permutation_t::iterator permutation_iter_t;
typedef permutation_t::const_iterator permutation_const_iter_t;

class block_t;
class bunch_t;

class pred_entry;
class succ_entry;
typedef bisim_gjkw::fixed_vector<pred_entry>::iterator pred_iter_t;
typedef bisim_gjkw::fixed_vector<succ_entry>::iterator succ_iter_t;

typedef bisim_gjkw::fixed_vector<pred_entry>::const_iterator pred_const_iter_t;
typedef bisim_gjkw::fixed_vector<succ_entry>::const_iterator succ_const_iter_t;

class block_bunch_slice_t;
typedef std::list<block_bunch_slice_t>::iterator block_bunch_slice_iter_t;
typedef std::list<block_bunch_slice_t>::const_iterator
                                                block_bunch_slice_const_iter_t;

enum new_block_mode_t { new_block_is_blue, new_block_is_red };

/// \class state_info_entry
/// \brief stores information about a single state
/// \details This class stores all other information about a state that the
/// partition needs.  In particular:  the block where the state belongs and the
/// position in the permutation array (i. e. the inverse of the permutation).
class state_info_entry
{
  public:
    /// \brief iterator to first inert incoming transition
    /// \details Non-inert incoming transitions of the state are stored just
    /// before the element where this iterator points to.
    ///
    /// During initialisation, this field also doubles up as a counter for the
    /// number of inert incoming transitions, and as the pointer to the first
    /// incoming transition that already has been initialised.
    iterator_or_counter<pred_iter_t> pred_inert;

    /// \brief iterator to first inert outgoing transition
    /// \details Non-inert outgoing transitions of the state are stored just
    /// before the element where this iterator points to.
    ///
    /// During initialisation, this field also doubles up as a counter for the
    /// number of inert outgoing transitions, and as the pointer to the first
    /// outgoing transition that already has been initialised.
    iterator_or_counter<succ_iter_t> succ_inert;

    /// block where the state belongs
    block_t* block;

    /// position of the state in the permutation array
    permutation_iter_t pos;

    /// \brief number of inert transitions to non-blue states
    /// \details Actually, as we cannot easily count how many inert outgoing
    /// transitions this state has, we initialize this pointer to
    /// succ_inert.begin.  Every time we find an outgoing transition to a blue
    /// state, we increase this iterator; as soon as it no longer points to an
    /// outgoing transition of this state, we have found all inert outgoing
    /// transitions.  This requires that after the inert outgoing transitions
    /// there is a transition that starts in another state, or there is a dummy
    /// transition.
    ///
    /// During initialisation, this field is used to point at the first unused
    /// slot of the (non-inert) predecessors. So we always assume that it
    /// starts as a pred_iter_t, at some moment is converted to a succ_iter_t,
    /// and then stays that way until it is destroyed.
    union not__t {
        pred_iter_t used_pred;
        succ_iter_t blue;

        not__t()  {  new(&used_pred) pred_iter_t();  };


        void convert_to_succ_iter_t()  {
            used_pred.~pred_iter_t();
            new (&blue) succ_iter_t();
        }


        ~not__t()  {  blue.~succ_iter_t();  };
    } not_;

    /// \brief iterator to first outgoing transition to the bunch of interest
    /// \details This iterator generally points to the an outgoing non-inert
    /// transition of the state, to the first transition in an out-slice.  If
    /// the bottom state has a transition in the splitter bunch (for a specific
    /// refinement), this out-slice contains the transitions in the splitter.
    /// The iterator helps to decide quickly whether a bottom state has a
    /// transition in the splitter.
    ///
    /// During initialisation, this field also doubles up as a counter for the
    /// number of non-inert outgoing transitions, and as the pointer to the
    /// first outgoing transition that already has been initialised.
    iterator_or_counter<succ_iter_t> current_out_slice;

    bool surely_has_transition_in(const bunch_t* splitter_bunch) const;
    bool surely_has_no_transition_in(const bunch_t* splitter_bunch) const;
                                                                                #ifndef NDEBUG
                                                                                    /// \brief print a short state identification for debugging
                                                                                    std::string debug_id_short() const
                                                                                    {   assert(&*s_i_begin <= this);  assert(this <= &s_i_end[-1]);
                                                                                        return std::to_string(this - &*s_i_begin);
                                                                                    }

                                                                                    /// \brief print a state identification for debugging
                                                                                    std::string debug_id() const
                                                                                    {
                                                                                        return "state " + debug_id_short();
                                                                                    }

                                                                                    /// \brief pointer at the first entry in the `state_info` array
                                                                                    static state_info_const_iter_t s_i_begin;

                                                                                    /// \brief pointer past the last entry in the `state_info` array
                                                                                    static state_info_const_iter_t s_i_end;

                                                                                    /// \brief iterator to the beginning of the permutation array
                                                                                    /// \details This iterator is required to be able to print identifications
                                                                                    /// for debugging.
                                                                                    static permutation_const_iter_t perm_begin;

                                                                                    /// \brief iterator to the end of the permutation array
                                                                                    /// \details This iterator is used in debugging assertions.
                                                                                    static permutation_const_iter_t perm_end;

                                                                                    mutable bisim_gjkw::check_complexity::state_dnj_counter_t work_counter;
                                                                                #endif
};


/// swap two permutations
static inline void iter_swap(permutation_iter_t s1, permutation_iter_t s2)
{                                                                               assert((*s1)->pos == s1);  assert((*s2)->pos == s2);
    state_info_iter_t temp = *s1;                                               assert(state_info_entry::perm_begin <= s1);
    *s1 = *s2;                                                                  assert(s1 < state_info_entry::perm_end);
    *s2 = temp;                                                                 assert(state_info_entry::perm_begin <= s2);
    (*s1)->pos = s1;                                                            assert(s2 < state_info_entry::perm_end);
    (*s2)->pos = s2;
}
/*
/// \details Note that the two permutation array entries are given as
/// references, not as iterators.  We have to jump through a hoop to turn
/// those pointers into iterators again...  We hope that the compiler optimizes
/// the calculation away.
///
/// We need the function with this signature because we sometimes sort the
/// entries in the permutation array using `std::sort()`.  However, this only
/// works if state_info_iter_t is a class (not a pointer).
static inline void swap(state_info_iter_t& s1, state_info_iter_t& s2)
{                                                                               assert(&*state_info_entry::perm_begin <= &s1);
                                                                                assert(&s1 <= &state_info_entry::perm_end[-1]);
                                                                                assert(&*state_info_entry::perm_begin <= &s2);
                                                                                assert(&s2 <= &state_info_entry::perm_end[-1]);
    iter_swap(state_info_entry::perm_begin +
                                       (&s1 - &*state_info_entry::perm_begin),
        state_info_entry::perm_begin + (&s2 - &*state_info_entry::perm_begin));
}
*/


/// \class block_t
/// \brief stores information about a block
/// \details A block corresponds to a slice of the permutation array.  As the
/// number of blocks is initially unknown, we will allocate instances of this
/// class dynamically.
///
/// The slice in the permutation array containing the states of the block is
/// subdivided into the following subslices (in this order):
/// 1. unmarked bottom states
/// 2. marked bottom states (initially empty)
/// 3. unmarked non-bottom states
/// 4. marked non-bottom states (initially empty)
///
/// A state should be marked iff it is a predecessor of the current splitter
/// (through a strong transition).  The marking is later extended to the red
/// states;  that are the states with a weak transition to the splitter.
///
/// (During the execution of some functions, blocks are subdivided further;
/// however, as these subdivisions are local to a single function, they are not
/// stored here.)
///
/// The blocks keep track of the total number of blocks allocated and number
/// themselves sequentially using the static member `nr_of_blocks`.  It is
/// therefore impossible to have multiple refinements running at the same time.
class block_t
{
  public:
    /// \brief iterator past the last state of the block
    permutation_iter_t end;

    /// \brief iterator to the first marked non-bottom state of the block
    permutation_iter_t marked_nonbottom_begin;

    /// \brief iterator to the first non-bottom state of the block
    permutation_iter_t nonbottom_begin;

    /// \brief iterator to the first marked bottom state of the block
    permutation_iter_t marked_bottom_begin;

    /// \brief iterator to the first state of the block
    permutation_iter_t begin;

    /// \brief list of stable block_bunch-slices with transitions from this
    /// block
    std::list<block_bunch_slice_t> stable_block_bunch;

    /// \brief unique sequence number of this block
    /// \details After the stuttering equivalence algorithm has terminated,
    /// this number is used as a state number in the quotient LTS.
    const state_type seqnr;

    /// \brief total number of blocks with unique sequence number allocated
    /// \details Upon starting the stuttering equivalence algorithm, the number
    /// of blocks must be zero.
    static state_type nr_of_blocks;


    /// \brief constructor
    /// \details The constructor initialises the block to: all states are
    /// bottom states, no state is marked.
    /// \param new_begin   initial iterator to the first state of the block
    /// \param new_end     initial iterator past the last state of the block
    block_t(permutation_iter_t new_begin, permutation_iter_t new_end)
      : end(new_end),
        marked_nonbottom_begin(new_end),
        nonbottom_begin(new_end),
        marked_bottom_begin(new_end),
        begin(new_begin),
        stable_block_bunch(),
        seqnr(nr_of_blocks++)
    {                                                                           assert(new_begin < new_end);
    }


    /// \brief destructor
    ~block_t()  {  }


    /// provides the number of states in the block
    state_type size() const
    {                                                                           assert(begin <= marked_bottom_begin);  assert(marked_nonbottom_begin <= end);
                                                                                assert(marked_bottom_begin <= nonbottom_begin);
                                                                                assert(nonbottom_begin <= marked_nonbottom_begin);
        return end - begin;
    }


    /// \brief provides the number of bottom states in the block
    state_type bottom_size() const
    {                                                                           assert(begin <= marked_bottom_begin);  assert(marked_nonbottom_begin <= end);
                                                                                assert(marked_bottom_begin <= nonbottom_begin);
                                                                                assert(nonbottom_begin <= marked_nonbottom_begin);
        return nonbottom_begin - begin;
    }


    /// \brief provides the number of marked bottom states in the block
    state_type marked_bottom_size() const
    {                                                                           assert(begin <= marked_bottom_begin);  assert(marked_nonbottom_begin <= end);
                                                                                assert(marked_bottom_begin <= nonbottom_begin);
                                                                                assert(nonbottom_begin <= marked_nonbottom_begin);
        return nonbottom_begin - marked_bottom_begin;
    }


    /// \brief provides the number of marked states in the block
    state_type marked_size() const
    {
        return end - marked_nonbottom_begin + marked_bottom_size();
    }


    /// provides the number of unmarked bottom states in the block
    state_type unmarked_bottom_size() const
    {                                                                           assert(begin <= marked_bottom_begin);  assert(marked_nonbottom_begin <= end);
                                                                                assert(marked_bottom_begin <= nonbottom_begin);
                                                                                assert(nonbottom_begin <= marked_nonbottom_begin);
        return marked_bottom_begin - begin;
    }


    /// provides the number of unmarked nonbottom states in the block
    state_type unmarked_nonbottom_size() const
    {                                                                           assert(begin <= marked_bottom_begin);  assert(marked_nonbottom_begin <= end);
                                                                                assert(marked_bottom_begin <= nonbottom_begin);
                                                                                assert(nonbottom_begin <= marked_nonbottom_begin);
        return marked_nonbottom_begin - nonbottom_begin;
    }


    /// \brief mark a non-bottom state
    /// \details Marking is done by moving the state to the slice of the marked
    /// non-bottom states of the block.
    /// \param s the non-bottom state that has to be marked
    /// \returns true if the state was not marked before
    bool mark_nonbottom(state_info_iter_t s)
    {                                                                           assert(this==s->block);  assert(nonbottom_begin<=s->pos);  assert(s->pos<end);
                                                                                assert(begin <= marked_bottom_begin);
                                                                                assert(marked_bottom_begin <= nonbottom_begin);
                                                                                assert(nonbottom_begin <= marked_nonbottom_begin);
        if (marked_nonbottom_begin <= s->pos)  return false;                    assert(marked_nonbottom_begin <= end);
        iter_swap(s->pos, --marked_nonbottom_begin);                            assert(nonbottom_begin <= marked_nonbottom_begin);
        return true;
    }


    /// \brief mark a state
    /// \details Marking is done by moving the state to the slice of the marked
    /// bottom or non-bottom states of the block.
    /// \param s the state that has to be marked
    /// \returns true if the state was not marked before
    bool mark(state_info_iter_t s)
    {                                                                           assert(this == s->block);  assert(begin <= s->pos);
        if (s->pos < nonbottom_begin)
        {                                                                       assert(begin <= marked_bottom_begin);  assert(marked_nonbottom_begin <= end);
                                                                                assert(nonbottom_begin <= marked_nonbottom_begin);
            if (marked_bottom_begin <= s->pos)  return false;                   assert(marked_bottom_begin <= nonbottom_begin);
            iter_swap(s->pos, --marked_bottom_begin);                           assert(begin <= marked_bottom_begin);
            return true;
        }
        return mark_nonbottom(s);
    }


    /// \brief refine a block
    /// \details This function is called after a refinement function has found
    /// where to split the block into unmarked (blue) and marked (red) states.
    /// It creates a new block for the smaller subblock.
    /// \param   old_block pointer to the block that needs to be split.
    /// \param   new_block_mode indicates whether the blue or the red block
    ///          should be the new one.  (This parameter is necessary in case
    ///          the two halves have exactly the same size.)
    /// \returns pointer to the new block
    block_t* split_off_block(enum new_block_mode_t const new_block_mode)
    {                                                                           assert(0 < marked_size());  assert(0 < unmarked_bottom_size());
        // swap contents

        // The structure of a block is
        // |  unmarked  |   marked   |  unmarked  |   marked   |
        // |   bottom   |   bottom   | non-bottom | non-bottom |
        // We have to swap the marked bottom with the unmarked non-bottom
        // states.
        //
        // It is not necessary to reset the nottoblue counters; these counters are
        // anyway only valid for the maybe-blue states.
        state_type swapcount = std::min(marked_bottom_size(),
                                                    unmarked_nonbottom_size());

        // create a new block
        permutation_iter_t const splitpoint = marked_bottom_begin +
                                                     unmarked_nonbottom_size(); assert(begin < splitpoint);  assert(splitpoint < end);
                                                                                assert((*splitpoint)->pos == splitpoint);
        block_t* new_block;
        if (new_block_is_blue == new_block_mode)
        {                                                                       assert((state_type) (splitpoint - begin) <= size()/2);
            new_block = new block_t(begin, splitpoint);
            new_block->nonbottom_begin = marked_bottom_begin;

            // adapt the old block: it only keeps the red states
            begin = splitpoint;
            nonbottom_begin = marked_nonbottom_begin;
        }
        else
        {                                                                       assert(new_block_is_red == new_block_mode);
            new_block = new block_t(splitpoint, end);                           assert((state_type) (end - splitpoint) <= size()/2);
            new_block->nonbottom_begin = marked_nonbottom_begin;

            // adapt the old block: it only keeps the blue states
            end = splitpoint;
            nonbottom_begin = marked_bottom_begin;
        }                                                                       ONLY_IF_DEBUG( new_block->work_counter = work_counter; )

        if (0 < swapcount)
        {
            // vector swap the states:
            permutation_iter_t pos1 = marked_bottom_begin,
                                                 pos2 = marked_nonbottom_begin; assert(pos1 < pos2);
            state_info_iter_t const temp = *pos1;
            for (;;)
            {
                --pos2;                                                         assert(pos1 < pos2);
                *pos1 = *pos2;
                (*pos1)->pos = pos1;
                ++pos1;
                if (0 >= --swapcount)  break;                                   assert(pos1 < pos2);
                *pos2 = *pos1;                                                  // mCRL2complexity(new_block_is_blue == new_block_mode ? pos1[-1] : *pos2, ...)
                (*pos2)->pos = pos2;                                            // -- overapproximated by the call at the end

            }
            *pos2 = temp;                                                       // mCRL2complexity(new_block_is_blue == new_block_mode ? pos1[-1] : *pos2, ...)
            (*pos2)->pos = pos2;                                                // -- overapproximated by the call at the end
        }
                                                                                #ifndef NDEBUG
                                                                                    for (permutation_iter_t s_iter = begin; s_iter < end; ++s_iter)
                                                                                    {  assert((*s_iter)->pos == s_iter);  }
                                                                                #endif
        // unmark all states in both blocks
        marked_nonbottom_begin = end;                                           assert(new_block->marked_nonbottom_begin == new_block->end);
        marked_bottom_begin = nonbottom_begin;
        new_block->marked_bottom_begin = new_block->nonbottom_begin;            assert(new_block->size() <= size());

        // set the block pointer of states is the new block
        for (permutation_iter_t s_iter = new_block->begin;
                                             s_iter < new_block->end; ++s_iter)
        {                                                                       assert((*s_iter)->pos == s_iter);
            (*s_iter)->block = new_block;                                       // mCRL2complexity (*s_iter, ...) -- subsumed in the call below
        }                                                                       mCRL2complexity(new_block, add_work(bisim_gjkw::check_complexity::
                                                                                                    split_off_block, bisim_gjkw::check_complexity::log_n -
                                                                                                    bisim_gjkw::check_complexity::ilog2(new_block->size())), );
        return new_block;
    }
                                                                                #ifndef NDEBUG
                                                                                    /// \brief print a block identification for debugging
                                                                                    std::string debug_id() const
                                                                                    {   assert(state_info_entry::perm_begin <= begin);  assert(begin < end);
                                                                                        assert(begin <= marked_bottom_begin);
                                                                                        assert(marked_bottom_begin <= nonbottom_begin);
                                                                                        assert(nonbottom_begin <= marked_nonbottom_begin);
                                                                                        assert(marked_nonbottom_begin <= end);
                                                                                        assert(end <= state_info_entry::perm_end);
                                                                                        return "block [" + std::to_string(begin-state_info_entry::perm_begin) +
                                                                                            "," + std::to_string(end - state_info_entry::perm_begin) + ") (#" +
                                                                                                                                   std::to_string(seqnr) + ")";
                                                                                    }

                                                                                    /// \brief print a slice of the partition (typically a block)
                                                                                    /// \details If the slice indicated by the parameters is not empty, the
                                                                                    /// message and the states in this slice will be printed.
                                                                                    /// \param message text printed as a title if the slice is not empty
                                                                                    /// \param begin   iterator to the beginning of the slice
                                                                                    /// \param end     iterator past the end of the slice
                                                                                    void print_block(const char* const message, permutation_const_iter_t
                                                                                                   begin_print, permutation_const_iter_t const end_print) const
                                                                                    {   assert(begin <= begin_print);  assert(end_print <= end);
                                                                                        assert(begin_print <= end_print);
                                                                                        if (0 == end_print - begin_print)  return;

                                                                                        mCRL2log(log::debug, "bisim_dnj") << '\t' << message
                                                                                                             << (1 < end_print - begin_print ? "s:\n" : ":\n");
                                                                                        do
                                                                                        {
                                                                                            mCRL2log(log::debug,"bisim_dnj") << "\t\t"
                                                                                                                                 << (*begin_print)->debug_id();
                                                                                            if (this != (*begin_print)->block)
                                                                                            {
                                                                                                mCRL2log(log::debug,"bisim_dnj") << ", inconsistent: "
                                                                                                             "points to " << (*begin_print)->block->debug_id();
                                                                                            }
                                                                                            if (begin_print != (*begin_print)->pos)
                                                                                            {
                                                                                                mCRL2log(log::debug, "bisim_dnj")
                                                                                                               << ", inconsistent pointer to state_info_entry";
                                                                                            }
                                                                                            mCRL2log(log::debug, "bisim_dnj") << '\n';
                                                                                        }
                                                                                        while (++begin_print < end_print);
                                                                                    }

                                                                                    mutable bisim_gjkw::check_complexity::block_dnj_counter_t work_counter;
                                                                                #endif
};


/// \class part_state_t
/// \brief refinable partition data structure
/// \details This class collects all information about a partition of the
/// states.
class part_state_t
{
  public:
    /// \brief permutation array
    /// \details This is the central element of the data structure:  In this
    /// array, states that belong to the same block are stored in adjacent
    /// elements.
    permutation_t permutation;

    /// \brief array with all other information about states
    /// \details We allocate 1 additional ``state'' to allow for the iterators
    /// past the last transition, as described in the documentation of
    /// `state_info_entry`.
    bisim_gjkw::fixed_vector<state_info_entry> state_info;

    /// \brief constructor
    /// \details The constructor allocates memory and makes the permutation and
    /// state_info arrays consistent with each other, but does not actually
    /// initialise the partition.  Immediately afterwards, the initialisation
    /// will be done in `bisim_partitioner_dnj::create_initial_partition()`.
    /// \param num_states number of states in the LTS
    part_state_t(state_type num_states)
      : permutation(num_states),
        state_info(num_states)
    {                                                                           assert(1 < num_states);  assert(0 == block_t::nr_of_blocks);
                                                                                #ifndef NDEBUG
                                                                                    state_info_entry::s_i_begin = state_info.cbegin();
                                                                                    state_info_entry::s_i_end = state_info.cend();
                                                                                    state_info_entry::perm_begin = permutation.cbegin();
                                                                                    state_info_entry::perm_end = permutation.cend();
                                                                                #endif
        permutation_iter_t perm_iter = permutation.begin();                     assert(perm_iter == state_info_entry::perm_begin);
        state_info_iter_t state_iter = state_info.begin();                      assert(state_iter == state_info_entry::s_i_begin);
        for (; permutation.end() != perm_iter; ++perm_iter, ++state_iter)
        {
            state_iter->pos = perm_iter;
            *perm_iter = state_iter;
        }                                                                       assert(perm_iter == state_info_entry::perm_end);
                                                                                assert(state_iter == state_info_entry::s_i_end);
    }

    /// \brief destructor
    /// \details The destructor also deallocates the blocks, as they are not
    /// directly referenced from anywhere.
    ~part_state_t()
    {                                                                           ONLY_IF_DEBUG( state_type deleted_blocks = 0; )
        permutation_iter_t permutation_iter = permutation.end();                assert(permutation_iter == state_info_entry::perm_end);
        while (permutation.begin() != permutation_iter)
        {
            block_t* const B = permutation_iter[-1]->block;                     assert(B->end == permutation_iter);
            permutation_iter = B->begin;                                        ONLY_IF_DEBUG( ++deleted_blocks; )
            delete B;
        }                                                                       assert(deleted_blocks == block_t::nr_of_blocks);
        block_t::nr_of_blocks = 0;                                              assert(permutation_iter == state_info_entry::perm_begin);
    }

    /// \brief calculate the size of the state space
    /// \returns the number of states in the LTS
    state_type state_size() const  {  return permutation.size();  }

    /// \brief find the block of a state
    /// \param s number of the state
    /// \returns a pointer to the block where state s resides in
    const block_t* block(state_type s) const
    {
        return state_info[s].block;
    }
                                                                                #ifndef NDEBUG
                                                                                    /// \brief print the partition per block
                                                                                    /// \details The function prints all blocks in order.  For each block, it
                                                                                    /// lists its states, separated into nonbottom and bottom states.
                                                                                    /// \param part_tr partition for the transitions
                                                                                    static void print_part()
                                                                                    {   assert(state_info_entry::perm_begin < state_info_entry::perm_end);
                                                                                        if (!mCRL2logEnabled(log::debug, "bisim_dnj"))  return;
                                                                                        const block_t* B = (*state_info_entry::perm_begin)->block;
                                                                                        do
                                                                                        {
                                                                                            mCRL2log(log::debug, "bisim_dnj") << B->debug_id() << ":\n";
                                                                                            B->print_block("Bottom state", B->begin, B->marked_bottom_begin);
                                                                                            B->print_block("Marked bottom state",
                                                                                                                   B->marked_bottom_begin, B->nonbottom_begin);
                                                                                            B->print_block("Non-bottom state",
                                                                                                                B->nonbottom_begin, B->marked_nonbottom_begin);
                                                                                            B->print_block("Marked non-bottom state",
                                                                                                                            B->marked_nonbottom_begin, B->end);
                                                                                        // go to next block
                                                                                        }
                                                                                        while (B->end<state_info_entry::perm_end && (B=(*B->end)->block,true));
                                                                                    }

                                                                                    /// \brief asserts that the partition of states is consistent
                                                                                    /// \details It also requires that no states are marked.
                                                                                    static void assert_consistency(bool branching)
                                                                                    {
                                                                                        permutation_const_iter_t block_begin = state_info_entry::perm_begin;
                                                                                        while (block_begin < state_info_entry::perm_end)
                                                                                        {
                                                                                            const block_t* const block = (*block_begin)->block;
                                                                                            // block is consistent:
                                                                                            assert(block->begin == block_begin);
                                                                                            assert(block->begin < block->marked_bottom_begin);
                                                                                            assert(block->marked_bottom_begin == block->nonbottom_begin);
                                                                                            assert(block->nonbottom_begin <= block->marked_nonbottom_begin);
                                                                                            assert(block->marked_nonbottom_begin == block->end);
                                                                                            assert(branching || block->nonbottom_begin == block->end);
                                                                                            assert(0 <= block->seqnr);
                                                                                            assert(block->seqnr < block_t::nr_of_blocks);
                                                                                            unsigned const max_block = bisim_gjkw::check_complexity::log_n -
                                                                                                            bisim_gjkw::check_complexity::ilog2(block->size());
                                                                                            mCRL2complexity(block, no_temporary_work(max_block), );

                                                                                            // states in the block are consistent:
                                                                                            for (permutation_const_iter_t perm_iter = block->begin;
                                                                                                                           perm_iter < block->end; ++perm_iter)
                                                                                            {
                                                                                                state_info_const_iter_t const state = *perm_iter;
                                                                                                // assert(pred_entry::pred_begin <= state->pred_inert.begin);
                                                                                                assert(state_info_entry::s_i_end - 1 == state ||
                                                                                                         state->pred_inert.begin <= state[1].pred_inert.begin);
                                                                                                // assert(state->pred_inert.begin <= pred_entry::pred_end);
                                                                                                // assert(succ_entry::succ_begin <=
                                                                                                //                             state->current_out_slice.begin);
                                                                                                assert(state->current_out_slice.begin <=
                                                                                                                                      state->succ_inert.begin);
                                                                                                // assert(state->succ_inert.begin <= succ_entry::succ_end);
                                                                                                // assert(state->current_out_slice.begin ==
                                                                                                //                                   state->succ_inert.begin ||
                                                                                                //   state == state->current_out_slice.begin->
                                                                                                //                                  block_bunch->pred->source);
                                                                                                if (perm_iter < block->nonbottom_begin)
                                                                                                {
                                                                                                    assert(state_info_entry::s_i_end - 1 == state || state->
                                                                                                         succ_inert.begin <= state[1].current_out_slice.begin);
                                                                                                    // assert(state->succ_inert.begin == succ_entry::succ_end ||
                                                                                                    //    state <
                                                                                                    //     state->succ_inert.begin->block_bunch->pred->source);
                                                                                                    mCRL2complexity(state,no_temporary_work(max_block,true),);
                                                                                                }
                                                                                                else
                                                                                                {
                                                                                                    // assert(state->succ_inert.begin < succ_entry::succ_end);
                                                                                                    assert(state_info_entry::s_i_end - 1 == state || state->
                                                                                                          succ_inert.begin < state[1].current_out_slice.begin);
                                                                                                    //assert(state ==
                                                                                                    //       state->succ_inert.begin->block_bunch->pred->source);
                                                                                                    mCRL2complexity(state,no_temporary_work(max_block,false),);
                                                                                                }
                                                                                                assert(block == state->block);
                                                                                                assert(perm_iter == state->pos);
                                                                                            }
                                                                                            block_begin = block->end;
                                                                                        }
                                                                                        assert(state_info_entry::perm_end == block_begin);
                                                                                    }
                                                                                #endif
};

///@} (end of group part_state)





/* ************************************************************************* */
/*                                                                           */
/*                           T R A N S I T I O N S                           */
/*                                                                           */
/* ************************************************************************* */





/// \defgroup part_trans
/// \brief data structures for transitions used during partition refinement
/// \details These definitions provide a partition for transition data
/// structure that can be used for the partition refinement algorithm.
///
/// Basically, transitions are stored in four arrays:
/// - `pred`: transitions ordered by goal state, to allow finding all
///   predecessors of a goal state.
/// - `succ`: transitions ordered by source state and bunch, to
///   allow finding all successors of a source state.  Given a transition in
///   this array, it is easy to find all transitions from the same source state
///   in the same bunch.
/// - `action_block`: a permutation of the transitions such that transitions
///   in the same bunch are adjacent, and within each bunch transitions with
///   the same action label and target block.
///   (Between two action_block-slices with different actions, there is a dummy
///   entry.)
/// - `block_bunch`: a permutation of the transitions suche that transitions
///   from the same block in the same bunch are adjacent.
///   (At the beginning of the block_bunch array, there is a dummy entry.)
/// Entries in these four arrays are linked with each other with circular
/// iterators, so that one can find the corresponding entry in another array.
///
/// Within this sort order, inert transitions are always placed after non-inert
/// transitions.
///
/// state_info_entry and block_t (defined above) contain pointers to the slices
/// of these arrays.  For bunches and block_bunch-slices, we additionally
/// create _descriptors_ that hold some information about the slice.

///@{

/// \brief information about a transition sorted per source state
class succ_entry
{
  public:
    /// \brief circular iterator to link the four transition arrays
    block_bunch_iter_t block_bunch;

    /// \brief pointer to delimit the slice of transitions in the same bunch
    /// \details For most transitions, this pointer points to the first
    /// transition that starts in the same state and belongs to the same
    /// bunch.  But if this transition is the first such transition, the
    /// pointer points to the last such transition (not one past the last, like
    /// otherwise in C and C++).
    ///
    /// For inert transitions, the value is nullptr.
    iterator_or_null<succ_iter_t> begin_or_before_end;


    /// \brief find the beginning of the out-slice
    succ_iter_t out_slice_begin() const;


    /// \brief find the bunch of the transition
    bunch_t* bunch() const;
                                                                                #ifndef NDEBUG
                                                                                    static succ_const_iter_t succ_begin;
                                                                                    static succ_const_iter_t succ_end;

                                                                                    template <class LTS_TYPE>
                                                                                    static inline void add_work_to_out_slice(
                                                                                        const bisim_partitioner_dnj<LTS_TYPE>& partitioner,
                                                                                        succ_const_iter_t out_slice_begin, enum bisim_gjkw::check_complexity::
                                                                                                             counter_type const ctr, unsigned const max_value);
                                                                                #endif
};


/// \brief information about a transition sorted per (source block, bunch) pair
class block_bunch_entry
{
  public:
    /// \brief circular iterator to link the four transition arrays
    pred_iter_t pred;

    /// \brief block_bunch-slice of which this transition is part
    /// \details The slice is null iff the transition is inert.
    iterator_or_null<block_bunch_slice_iter_t> slice;
};


/// \brief information about a transition sorted per target state
/// \details As I expect the transitions in this array to be moved least often,
/// I store the information on source and target state in this entry.  (It
/// could be stored in any of the four arrays describing the transition;
/// through the circular iterators, the information would be available anyway.)
class pred_entry
{
  public:
    /// \brief circular iterator to link the four transition arrays
    action_block_iter_t action_block;

    /// \brief source state of the transition
    state_info_iter_t source;

    /// \brief target state of the transition
    state_info_iter_t target;
                                                                                #ifndef NDEBUG
                                                                                    static pred_const_iter_t pred_begin;
                                                                                    static pred_const_iter_t pred_end;

                                                                                    /// \brief print a short transition identification for debugging
                                                                                    std::string debug_id_short() const
                                                                                    {
                                                                                        return "from " + source->debug_id_short() +
                                                                                                                             " to " + target->debug_id_short();
                                                                                    }

                                                                                    /// \brief print a transition identification for debugging
                                                                                    template <class LTS_TYPE>
                                                                                    std::string debug_id(const bisim_partitioner_dnj<LTS_TYPE>& partitioner)
                                                                                                                                                         const;

                                                                                    mutable bisim_gjkw::check_complexity::trans_dnj_counter_t work_counter;
                                                                                #endif
};


/// \brief information about a transition sorted per (action, target block)
/// pair
class action_block_entry
{
  public:
    /// \brief circular iterator to link the four transition arrays
    /// \details This iterator can be nullptr because we need to insert dummy
    /// elements between two action_block-slices during initialisation, to make
    /// it easier for first_move_transition_to_new_action_block() to detect
    /// whether two action_block-slices belong to the same action or not.
    iterator_or_null<succ_iter_t> succ;

    /// \brief pointer to delimit the slice of transitions in the same (action,
    /// block) pair
    /// \details For most transitions, this pointer points to the first
    /// transition that has the same action and goes to the same block.  But if
    /// this transition is the first such transition, the pointer points to the
    /// last such transition (not one past the last, like otherwise in C and
    /// C++).
    ///
    /// For inert transitions and dummy entries, the value is nullptr.
    iterator_or_null<action_block_iter_t> begin_or_before_end;

    /// \brief find the beginning of the action_block-slice
    action_block_iter_t action_block_slice_begin() const
    {
        action_block_iter_t result = begin_or_before_end();
        if (this < &*result)
        {                                                                       assert(this == &*result->begin_or_before_end());
            result += this - &*result; // result = action_block_iter_t(this);   // The following assertion does not always hold: the function is called
        }                                                                       // immediately after a block is refined, so it may be the case that the
                                                                                // transitions are still to be moved to different slices.
                                                                                // assert(succ()->block_bunch->pred->target->block ==
                                                                                //                           result->succ()->block_bunch->pred->target->block);
                                                                                assert(succ()->bunch() == result->succ()->bunch());
                                                                                assert(result == action_block_begin || result[-1].succ.is_null() ||
                                                                                            action_block_orig_inert_begin <= result ||
                                                                                            result[-1].succ()->block_bunch->pred->target->block !=
                                                                                                             result->succ()->block_bunch->pred->target->block);
                                                                                // assert(this has the same action as result);
        return result;
    }

/*
    /// \brief find the end of the action_block-slice
    action_block_iter_t action_block_slice_before_end() const
    {
        action_block_iter_t result = begin_or_before_end();
        if (&*result < this)
        {
            result = result->begin_or_before_end();                             assert(this <= &*result);
        }                                                                       // assert(this has the same action as result);
                                                                                assert(succ()->block_bunch->pred->target->block ==
                                                                                                             result->succ()->block_bunch->pred->target->block);
                                                                                assert(succ()->bunch() == result->succ()->bunch());
                                                                                assert(result + 1 == *action_block_end || result[1].succ.is_null() ||
                                                                                                result[1].succ()->block_bunch->pred->target->block !=
                                                                                                             result->succ()->block_bunch->pred->target->block);
        return result;
    }
*/
                                                                                #ifndef NDEBUG
                                                                                    static action_block_const_iter_t action_block_begin;
                                                                                    static const action_block_iter_t* action_block_end;
                                                                                    static action_block_const_iter_t action_block_orig_inert_begin;
                                                                                #endif
};


/// \brief bunch of transitions
/// \details Like a slice, at the end of the algorithm there will be a bunch
/// for every transition in the bisimulation quotient.  Therefore, we should
/// try to minimize the size of a bunch as much as possible.
class bunch_t
{
  public:
    action_block_iter_t end;
    action_block_iter_t begin;

    union sort_key_and_label_t
    {
        // During refinement, this field stores the sort key.
        // After refinement, it is set to the label.
        // To distinguish the two, we will use large numbers for sort keys and
        // small ones for labels; then the probability that an error message is
        // generated if someone mixes up the two is high.
        trans_type sort_key;
        label_type label;

        sort_key_and_label_t(trans_type new_sort_key)
        {
            sort_key = new_sort_key;
        }
        ~sort_key_and_label_t()  {  }
        sort_key_and_label_t(const sort_key_and_label_t&) = delete;
        void operator=(const sort_key_and_label_t&) = delete;
    } sort_key_and_label;
//  private:
    static bunch_t* first_nontrivial;
    bunch_t* next_nontrivial;
  public:
    bunch_t(action_block_iter_t new_begin, action_block_iter_t new_end,
                                                       trans_type new_sort_key)
      : end(new_end),
        begin(new_begin),
        sort_key_and_label(new_sort_key),
        next_nontrivial(nullptr)
    {  }

    static bunch_t* get_some_nontrivial()  {  return first_nontrivial;  }

    void make_nontrivial()
    {                                                                           assert(1 < end - begin);  assert(is_trivial());
                                                                                // The following assertions do not necessarily hold during initialisation:
                                                                                //assert(begin <= begin->begin_or_before_end());
        next_nontrivial = nullptr==first_nontrivial ? this : first_nontrivial;  //assert(begin->begin_or_before_end() < end[-1].begin_or_before_end());
        first_nontrivial = this;                                                //assert(end[-1].begin_or_before_end() <= end);
    }
    void make_trivial()
    {                                                                           assert(!is_trivial());  assert(first_nontrivial == this);
        first_nontrivial = this == next_nontrivial ? nullptr : next_nontrivial; assert(end - 1 == begin->begin_or_before_end());
        next_nontrivial = nullptr;                                              assert(begin == end[-1].begin_or_before_end());
    }

    /// \brief returns true iff the bunch is trivial
    /// \details If this bunch is the last in the list of non-trivial
    /// bunches, the convention is that the next pointer points to this
    /// bunch itself (to distinguish it from nullptr).
    bool is_trivial() const
    {
        return nullptr == next_nontrivial;
    }

    /// \brief split off a single action_block-slice from the bunch
    /// \details The function splits the current bunch after
    /// its first action_block-slice or before its last
    /// action_block-slice, whichever is smaller.  It creates
    /// a new bunch for the split-off slice and returns a pointer to the
    /// new bunch.  The caller has to adapt the block_bunch-slices.
    bunch_t* split_off_small_action_block_slice()
    {                                                                           assert(begin < end);  assert(!begin->succ.is_null());
        action_block_iter_t first_slice_end = begin->begin_or_before_end() + 1; assert(!end[-1].succ.is_null());
        action_block_iter_t last_slice_begin = end[-1].begin_or_before_end();   assert(begin < first_slice_end);  assert(first_slice_end <= last_slice_begin);
        bunch_t* new_bunch;                                                     assert(last_slice_begin < end);  assert(!first_slice_end[-1].succ.is_null());
        /* Line 1.11: Select a small action_block-slice in splitter_bunch    */ assert(!last_slice_begin->succ.is_null());
        //            i.e. |new_bunch| <= 1/2*|splitter_bunch|
            // It doesn't matter very much how ties are resolved here:
        if (first_slice_end - begin > end - last_slice_begin)
        {
            // Line 1.12: Create a new bunch new_bunch that consists of the
            //            selected action_block-slice
            new_bunch = new bunch_t(last_slice_begin, end,
                     sort_key_and_label.sort_key + (last_slice_begin - begin));
            end = last_slice_begin;
            if (end[-1].succ.is_null())
            {
                --end;                                                          assert(first_slice_end <= end);  assert(end->begin_or_before_end.is_null());
            }                                                                   assert(!end[-1].succ.is_null());
            if (first_slice_end == end)  make_trivial();
        }
        else
        {
            // Line 1.12: Create a new bunch new_bunch that consists of the
            //            selected action_block-slice
            new_bunch = new bunch_t(begin, first_slice_end,
                        sort_key_and_label.sort_key + (end - first_slice_end));
            begin = first_slice_end;
            if (begin->succ.is_null())
            {                                                                   assert(begin->begin_or_before_end.is_null());
                ++begin;                                                        assert(begin <= last_slice_begin);
            }                                                                   assert(!begin->succ.is_null());
            if (begin == last_slice_begin)  make_trivial();
        }                                                                       ONLY_IF_DEBUG( new_bunch->work_counter = work_counter; )
        return new_bunch;
    }

    bool operator<(const bunch_t& el) const
    {
        return sort_key_and_label.sort_key < el.sort_key_and_label.sort_key;
    }
    bool operator>(const bunch_t& el) const  {  return el.operator<(*this);  }
    bool operator<=(const bunch_t& el) const  {  return !operator>(el);  }
    bool operator>=(const bunch_t& el) const  {  return !operator<(el);  }
                                                                                #ifndef NDEBUG
                                                                                    std::string debug_id_short() const
                                                                                    {   assert(action_block_entry::action_block_begin <= begin);
                                                                                        assert(begin < end);
                                                                                        assert(end <= *action_block_entry::action_block_end);
                                                                                        return "bunch [" +
                                                                                             std::to_string(begin-action_block_entry::action_block_begin)+ ","+
                                                                                             std::to_string(end-action_block_entry::action_block_begin) + ")";
                                                                                    }

                                                                                    /// \brief print a bunch identification for debugging
                                                                                    std::string debug_id() const
                                                                                    {   assert(!end[-1].succ.is_null());
                                                                                        action_block_const_iter_t iter = begin;
                                                                                        assert(iter == iter->succ()->block_bunch->pred->action_block);
                                                                                        std::string result = debug_id_short();
                                                                                        result += " containing transition";
                                                                                        result += iter < end - 1 ? "s " : " ";
                                                                                        result += iter->succ()->block_bunch->pred->debug_id_short();
                                                                                        ++iter;
                                                                                        if (end <= iter)  return result;
                                                                                        while (iter->succ.is_null())  ++iter;
                                                                                        assert(iter < end);
                                                                                        assert(iter == iter->succ()->block_bunch->pred->action_block);
                                                                                        result += ", ";
                                                                                        result += iter->succ()->block_bunch->pred->debug_id_short();
                                                                                        if (iter < end - 3)
                                                                                        {
                                                                                            result += ", ...";
                                                                                            iter = end - 3;
                                                                                        }
                                                                                        while (++iter < end)
                                                                                        {
                                                                                            if (!iter->succ.is_null())
                                                                                            {   assert(iter == iter->succ()->block_bunch->pred->action_block);
                                                                                                result += ", ";
                                                                                                result += iter->succ()->block_bunch->pred->debug_id_short();
                                                                                            }
                                                                                        }
                                                                                        return result;
                                                                                    }

                                                                                    /// \brief calculates the maximal allowed value for work counters
                                                                                    /// associated with this bunch
                                                                                    /// \details Work counters may only be nonzero if this bunch is a
                                                                                    /// single-action bunch, i. e. all its transitions have the same action
                                                                                    /// label.  Also, only then the size can be calculated as end - begin.
                                                                                    template <class LTS_TYPE> int max_work_counter(
                                                                                                     const bisim_partitioner_dnj<LTS_TYPE>& partitioner) const;

                                                                                    mutable bisim_gjkw::check_complexity::bunch_dnj_counter_t work_counter;
                                                                                #endif
};


/// \brief Information about a set of transitions with the same source block,
/// in the same bunch
/// \details This describes the coarser partition of transitions.  In the end
/// we will have one slice for each transition, so we should try to minimize
/// this data structure as much as possible.
///
/// Also note that these slices are part of a doubly-linked list.  We cannot
/// change this to a singly-linked list because then we would have to
/// meticulously keep the list ordered, which is not possible when a block
/// loses all its bottom states.
class block_bunch_slice_t
{
  public:
    /// pointer past the end of the transitions in the block_bunch array
    block_bunch_iter_t end;

    /// bunch to which this slice belongs
    class bunch_stability_t
    {
      private:
        intptr_t data;
      public:
        bunch_stability_t(bunch_t* new_bunch, bool is_stable)
          : data(reinterpret_cast<intptr_t>(new_bunch) | (intptr_t) is_stable)
        {                                                                       assert(sizeof(bunch_t*) == sizeof(intptr_t));
                                                                                assert(0 == (reinterpret_cast<intptr_t>(new_bunch) & 3));
        }
        ~bunch_stability_t()  {  }

        void operator=(const bunch_t* new_bunch)
        {                                                                       assert(0 == (reinterpret_cast<intptr_t>(new_bunch) & 3));
            data = reinterpret_cast<intptr_t>(new_bunch) | (data & 3);
        }
        bunch_t* operator()() const
        {
            return reinterpret_cast<bunch_t*>(data & ~(intptr_t) 3);
        }
        bool is_stable__() const           {  return data & 1;         }
        void make_stable__()               {  data |= 1;               }
        void make_unstable__()             {  data &= ~(intptr_t) 1;   }
        //bool is_marked__() const           {  return 0 != (data & 2);  }
        //void mark__()                      {  data |= 2;               }
        //void unmark__()                    {  data &= ~(intptr_t) 2;   }
        //void make_unstable_and_unmark__()  {  data &= ~(intptr_t) 3;   }
    } bunch;

    bool is_stable() const           {  return bunch.is_stable__();  }
    void make_stable()               {  bunch.make_stable__();       }
    void make_unstable()             {  bunch.make_unstable__();     }
    //bool is_marked() const           {  return bunch.is_marked__();  }
    //void mark()                      {  bunch.mark__();              }
    //void unmark()                    {  bunch.unmark__();            }
    //void make_unstable_and_unmark()  {  bunch.make_unstable_and_unmark__();  }


    block_bunch_slice_t(block_bunch_iter_t new_end,
                                            bunch_t* new_bunch, bool is_stable)
      : end(new_end),
        bunch(new_bunch, is_stable)
    {  }


    bool empty() const
    {                                                                           assert(block_bunch_begin <= end);  assert(end <= *block_bunch_end);
                                                                                assert(block_bunch_begin[-1].slice != this);
        return end[-1].slice != this;
    }


    /// compute the source block of the transitions in this slice
    block_t* source_block() const
    {                                                                           assert(!empty());
        return end[-1].pred->source->block;
    }
                                                                                #ifndef NDEBUG
                                                                                    static block_bunch_const_iter_t block_bunch_begin;
                                                                                    static const block_bunch_iter_t* block_bunch_end;

                                                                                    std::string debug_id() const
                                                                                    {
                                                                                        static struct {
                                                                                            bool operator()(const block_bunch_entry& p1,
                                                                                                                           const block_bunch_slice_t* p2) const
                                                                                            {
                                                                                                return p1.slice != p2;
                                                                                            }
                                                                                        } block_bunch_not_equal;

                                                                                        assert(block_bunch_begin <= end);  assert(end <= *block_bunch_end);
                                                                                        std::string index_string = std::to_string(end - block_bunch_begin);
                                                                                        if (empty())
                                                                                        {
                                                                                            return "empty block_bunch_slice [" + index_string + "," +
                                                                                                                                            index_string + ")";
                                                                                        }
                                                                                        block_bunch_const_iter_t begin = block_bunch_begin;
                                                                                        trans_type bunch_size = bunch()->end - bunch()->begin;
                                                                                        if ((trans_type) (end - begin) > bunch_size)  begin = end - bunch_size;
                                                                                        begin = std::lower_bound(begin, (block_bunch_const_iter_t) end - 1,
                                                                                                                                  this, block_bunch_not_equal);
                                                                                        assert(begin->slice == this);
                                                                                        assert(begin == block_bunch_begin || begin[-1].slice != this);
                                                                                        return "block_bunch-slice [" + std::to_string(begin-block_bunch_begin)+
                                                                                               "," + index_string + ") containing transitions from " +
                                                                                               source_block()->debug_id() + " in " + bunch()->debug_id_short();
                                                                                    }

                                                                                    // \brief add work to transitions starting in bottom states
                                                                                    template <class LTS_TYPE>
                                                                                    bool add_work_to_bottom_transns(
                                                                                       enum bisim_gjkw::check_complexity::counter_type ctr, unsigned max_value,
                                                                                                            const bisim_partitioner_dnj<LTS_TYPE>& partitioner)
                                                                                    {   assert(!empty());
                                                                                        assert(1U == max_value);
                                                                                        const block_t* block = source_block();
                                                                                        bool result = false;
                                                                                        block_bunch_iter_t block_bunch = end;
                                                                                        do
                                                                                        {
                                                                                            --block_bunch;
                                                                                            state_info_iter_t const source = block_bunch->pred->source;
                                                                                            assert(source->block == block);
                                                                                            if (source->pos < block->nonbottom_begin)
                                                                                            {
                                                                                                // the transition starts in a (new) bottom state
                                                                                                mCRL2complexity(block_bunch->pred, add_work(ctr, max_value),
                                                                                                                                                  partitioner);
                                                                                                result = true;
                                                                                            }
                                                                                        }
                                                                                        while (block_bunch_begin < block_bunch && block_bunch[-1].slice==this);
                                                                                        return result;
                                                                                    }

                                                                                    mutable bisim_gjkw::check_complexity::block_bunch_dnj_counter_t
                                                                                                                                                  work_counter;
                                                                                #endif
};


/// \brief find the beginning of the out-slice
inline succ_iter_t succ_entry::out_slice_begin() const
{
    succ_iter_t result = begin_or_before_end();
    if (this < &*result)
    {                                                                           assert(this == &*result->begin_or_before_end());
        result += this - &*result; // result = succ_iter_t(this);
    }                                                                           assert(block_bunch->pred->source == result->block_bunch->pred->source);
                                                                                // assert(this <= result); //< holds always, based on the if() above
                                                                                assert(this <= &*result->begin_or_before_end());
                                                                                assert(block_bunch->slice() == result->block_bunch->slice());
                                                                                assert(succ_entry::succ_begin == result ||
                                                                                        result[-1].block_bunch->pred->source < block_bunch->pred->source ||
                                                                                                         *result[-1].bunch() < *block_bunch->slice()->bunch());
    return result;
}


inline bunch_t* succ_entry::bunch() const
{
    return block_bunch->slice()->bunch();
}


class part_trans_t
{
  public:
    /// \brief array containing all successor entries
    /// \details The first and last entry are dummy entries, pointing to a
    /// transition from nullptr to nullptr, to make it easier to check whether
    /// there is another transition from the current state.
    bisim_gjkw::fixed_vector<succ_entry> succ;

    /// \brief array containing all block_bunch entries
    /// \details The first entry is a dummy entry, pointing to a transition not
    /// contained in any slice, to make it easier to check whether there is
    /// another transition in the current block_bunch.
    bisim_gjkw::fixed_vector<block_bunch_entry> block_bunch;

    /// \brief array containing all predecessor entries
    /// \details The first and last entry are dummy entries, pointing to a
    /// transition to nullptr, to make it easier to check whether there is
    /// another transition to the current state.
    bisim_gjkw::fixed_vector<pred_entry> pred;

    /// \brief array containing all action_block entries
    /// \details During initialisation, the transitions are sorted according to
    /// their label. Between transitions with different labels there is a dummy
    /// entry, to make it easier to check whether there is another transition
    /// in the current action_block slice.
    bisim_gjkw::fixed_vector<action_block_entry> action_block;

    /// \brief pointer to the first inert transition in block_bunch
    block_bunch_iter_t block_bunch_inert_begin;

    /// \brief pointer to the first inert transition in action_block
    action_block_iter_t action_block_inert_begin;

    /// \brief list of unstable block_bunch-slices
    std::list<block_bunch_slice_t> unstable_block_bunch;

    /// \brief pointer to first unstable block_bunch-slice that is not
    /// currently postprocessed
    /// \details During postprocessing, only the unstable block_bunch-slices
    /// that contain transitions from the block withe new non-inert transitions
    /// need to be handled.  This pointer points to the first unstable
    /// block_bunch-slice that does not need to be postprocessed.
    ///
    /// At other times, this pointer points to the end of the unstable
    /// block_bunch-slices.
    block_bunch_slice_iter_t unstable_block_bunch_postprocess_end;


    /// \brief constructor
    part_trans_t(trans_type num_transitions, trans_type num_actions)
      : succ(num_transitions + 2),
        block_bunch(num_transitions + 1),
        pred(num_transitions + 2),
        action_block(num_transitions + num_actions - 1),
        block_bunch_inert_begin(block_bunch.end()),
        action_block_inert_begin(action_block.end()),
        unstable_block_bunch(),
        unstable_block_bunch_postprocess_end(unstable_block_bunch.end())
    {
        succ.begin()->block_bunch = block_bunch.begin();
        succ.end()[-1].block_bunch = block_bunch.begin();
        block_bunch.begin()->pred = pred.begin();                               assert(block_bunch.begin()->slice.is_null());
                                                                                #ifndef NDEBUG
                                                                                    succ_entry::succ_begin = succ.cbegin() + 1;
                                                                                    succ_entry::succ_end = succ.cend() - 1;
                                                                                    block_bunch_slice_t::block_bunch_begin = block_bunch.cbegin() + 1;
                                                                                    block_bunch_slice_t::block_bunch_end = &block_bunch_inert_begin;
                                                                                    pred_entry::pred_begin = pred.cbegin() + 1;
                                                                                    pred_entry::pred_end = pred.cend() - 1;
                                                                                    action_block_entry::action_block_begin = action_block.cbegin();
                                                                                    action_block_entry::action_block_end = &action_block_inert_begin;
                                                                                #endif
    }

    /// \brief destructor
    /// \details The destructor also deallocates the bunches, as they are not
    /// directly referenced from anywhere.
    ~part_trans_t()
    {
        action_block_iter_t action_block_iter = action_block.begin();           assert(action_block_iter == action_block_entry::action_block_begin);
        for (;;)
        {
            if (action_block_inert_begin <= action_block_iter)  return;
            if (action_block_iter->succ.is_null())
            {                                                                   assert(action_block_iter->begin_or_before_end.is_null());
                ++action_block_iter;
                if (action_block_inert_begin <= action_block_iter)  return;     assert(!action_block_iter->succ.is_null());
            }                                                                   assert(!action_block_iter->begin_or_before_end.is_null());
            bunch_t* const bunch = action_block_iter->succ()->bunch();          assert(bunch->begin == action_block_iter);
            action_block_iter = bunch->end;
            delete bunch;
        }
        // unreachable
    }

    /// \brief transition is moved to a new bunch
    /// \details This (and the next function) have to be called after a
    /// transition has changed its bunch.  The member function will adapt the
    /// transition data structure.  It assumes that the transition is
    /// non-inert.
    ///
    /// The work has to be done in two steps: We call the first step
    /// first_move_transition_to_new_bunch() for each transition in the new
    /// bunch, and then call second_move_transition_to_new_bunch() again for
    /// all these transitions.  The reason is that some data structures need to
    /// be finalised in the second phase.
    ///
    /// The first phase moves all transitions to their correct position in the
    /// out-slices and block_bunch-slices, but it doesn't yet create a fully
    /// correct new out-slice and block_bunch-slice.  It adapts
    /// current_out_slice of all states with a transition in the new bunch.
    /// \param action_block_iter_iter     transition that has to be changed
    /// \param first_transition_of_state  This is the first transition of the
    ///                                   state, so a new bunch_slice has to be
    ///                                   allocated.
    #ifdef NDEBUG
        static
    #endif
    void first_move_transition_to_new_bunch(action_block_iter_t const
                       action_block_iter, bool const first_transition_of_state)
    {

        /* -  -  -  -  -  -  -  - adapt part_tr.succ -  -  -  -  -  -  -  - */

        // Line 1.17: Register that s --> t is now part of new_bunch
        succ_iter_t const old_succ_pos = action_block_iter->succ();             assert(old_succ_pos->block_bunch->pred->action_block == action_block_iter);
        succ_iter_t out_slice_begin = old_succ_pos->out_slice_begin();          assert(out_slice_begin->block_bunch->pred->action_block->succ() ==
                                                                                                                                              out_slice_begin);
        succ_iter_t const new_succ_pos=out_slice_begin->begin_or_before_end();  assert(out_slice_begin == new_succ_pos->begin_or_before_end());
                                                                                assert(new_succ_pos<old_succ_pos->block_bunch->pred->source->succ_inert.begin);
        /* move the transition to the end of its out-slice                   */ assert(new_succ_pos->block_bunch->pred->action_block->succ() == new_succ_pos);
        if (old_succ_pos < new_succ_pos)
        {
            std::swap(old_succ_pos->block_bunch, new_succ_pos->block_bunch);
            old_succ_pos->block_bunch->pred->action_block->succ.change(
                                                                 old_succ_pos); assert(action_block_iter == new_succ_pos->block_bunch->pred->action_block);
            action_block_iter->succ.change(new_succ_pos);
        }                                                                       else  assert(old_succ_pos == new_succ_pos);

        // adapt the old out-slice immediately
            // If the old out-slice becomes empty, then out_slice_begin ==
            // new_succ_pos, so the two following assignments will assign the
            // same variable.  The second assignment is the relevant one.
        out_slice_begin->begin_or_before_end.change(new_succ_pos - 1);

        // adapt the new out-slice, as far as is possible now:
            // make the begin_or_before_end pointers of the first and last
            // transition in the slice correct immediately.  The other
            // begin_or_before_end pointers need to be corrected after all
            // transitions in the new bunch have been positioned correctly.
        if (first_transition_of_state)
        {
            // Line 1.15: Set the current out-slice pointer of s to a
            //            transition in splitter_bunch (preferably not
            //            s --> t itself)
            new_succ_pos->block_bunch->pred->source->current_out_slice.begin =
                                                               out_slice_begin;
            new_succ_pos->begin_or_before_end.change(new_succ_pos);
        }
        else
        {
            succ_iter_t out_slice_before_end =
                                         new_succ_pos[1].begin_or_before_end(); assert(new_succ_pos < out_slice_before_end);
                                                                                assert(out_slice_before_end->block_bunch->pred->action_block->succ() ==
                                                                                                                                         out_slice_before_end);
                                                                                assert(out_slice_before_end <
                                                                                                    new_succ_pos->block_bunch->pred->source->succ_inert.begin);
                                                                                assert(new_succ_pos + 1 == out_slice_before_end->begin_or_before_end());
            out_slice_before_end->begin_or_before_end.change(new_succ_pos);     assert(out_slice_begin ==
                                                                                             new_succ_pos->block_bunch->pred->source->current_out_slice.begin);
            new_succ_pos->begin_or_before_end.change(out_slice_before_end);     assert(out_slice_before_end->bunch() == new_succ_pos->bunch());
        }

        /* -  -  -  -  -  -  - adapt part_tr.block_bunch -  -  -  -  -  -  - */ assert(new_succ_pos == action_block_iter->succ());
                                                                                assert(unstable_block_bunch.empty());
        block_bunch_iter_t const old_block_bunch_pos=new_succ_pos->block_bunch; assert(old_block_bunch_pos->pred->action_block == action_block_iter);
        block_bunch_slice_iter_t const old_block_bunch_slice =
                                                  old_block_bunch_pos->slice();
        block_bunch_iter_t const new_block_bunch_pos =
                                                old_block_bunch_slice->end - 1; assert(new_block_bunch_pos->pred->action_block->succ()->block_bunch ==
                                                                                                                                          new_block_bunch_pos);
        // move the transition to the end of its block_bunch-slice
        if (old_block_bunch_pos < new_block_bunch_pos)
        {
            std::swap(old_block_bunch_pos->pred, new_block_bunch_pos->pred);
            old_block_bunch_pos->pred->action_block->succ()->block_bunch =
                                                           old_block_bunch_pos; assert(new_succ_pos == new_block_bunch_pos->pred->action_block->succ());
            new_succ_pos->block_bunch = new_block_bunch_pos;
        }                                                                       else  assert(new_block_bunch_pos == old_block_bunch_pos);

        // adapt the old block_bunch-slice
        old_block_bunch_slice->end = new_block_bunch_pos;

        // (create and) adapt the new block_bunch-slice
            // All this work is done in the second phase.
    }


    /// \brief correct transition data structure after splitting a bunch
    /// \brief transition is moved to a new bunch, phase 2
    /// \details This (and the previous function) have to be called after a
    /// transition has changed its bunch.  The member function will adapt the
    /// transition data structure.  It assumes that the transition is non-inert.
    ///
    /// The work has to be done in two steps: We call the first step
    /// first_move_transition_to_new_bunch() for each transition in the new bunch,
    /// and then call second_move_transition_to_new_bunch() again for all these
    /// transitions.  The reason is that some data structures need to be finalised
    /// in the second phase.
    ///
    /// The second phase finalizes the new out-slices and block_bunch-slices
    /// that were left half-finished by the first phase.  It assumes that all
    /// block_bunch-slices are registered as stable.
                                                                                ONLY_IF_DEBUG( template <class LTS_TYPE> )
    void second_move_transition_to_new_bunch(                                   ONLY_IF_DEBUG( const bisim_partitioner_dnj<LTS_TYPE>& partitioner, )
         action_block_iter_t const action_block_iter, bunch_t* const new_bunch)
    {                                                                           assert(nullptr != new_bunch);

        /* -  -  -  -  -  -  -  - adapt part_tr.succ -  -  -  -  -  -  -  - */

        // Line 1.17: Register that s --> t is now part of new_bunch
            // We already moved the transition in part_tr.succ to the correct
            // place in first_move_transition_to_new_bunch(); now we have to
            // set begin_or_before_end.
        succ_iter_t const new_succ_pos = action_block_iter->succ();             assert(new_succ_pos->block_bunch->pred->action_block == action_block_iter);
        succ_iter_t const new_begin_or_before_end =
                    new_succ_pos->begin_or_before_end()->begin_or_before_end(); assert(new_begin_or_before_end->block_bunch->pred->action_block->succ() ==
                                                                                                                                      new_begin_or_before_end);
        state_info_iter_t const source=new_succ_pos->block_bunch->pred->source; assert(*source->pos == source);

        if (new_begin_or_before_end < new_succ_pos)
        {
            new_succ_pos->begin_or_before_end.change(new_begin_or_before_end);
        }
                                                                                #ifndef NDEBUG
                                                                                    else
                                                                                    {   assert(new_begin_or_before_end == new_succ_pos);
                                                                                        succ_const_iter_t new_before_end =
                                                                                                                new_begin_or_before_end->begin_or_before_end();
                                                                                        if (new_begin_or_before_end <= new_before_end)
                                                                                        {   assert(source->current_out_slice.begin <= new_begin_or_before_end);
                                                                                            // This is the first transition in the new out-slice.  Test
                                                                                            // whether it is sorted according to bunch order.
                                                                                            assert(succ_entry::succ_begin == new_begin_or_before_end ||
                                                                                              new_begin_or_before_end[-1].block_bunch->pred->source < source ||
                                                                                                            *new_begin_or_before_end[-1].bunch() < *new_bunch);
                                                                                            assert(new_before_end + 1 == source->succ_inert.begin ||
                                                                                                                      *new_bunch < *new_before_end[1].bunch());
        /* -  -  -  -  -  -  - adapt part_tr.block_bunch -  -  -  -  -  -  - */         }
                                                                                    }
                                                                                #endif
        block_bunch_iter_t const new_block_bunch_pos=new_succ_pos->block_bunch; assert(new_block_bunch_pos->pred->action_block->succ() == new_succ_pos);
        block_bunch_slice_iter_t const old_block_bunch_slice =
                                                  new_block_bunch_pos->slice();
                                                                                #ifndef NDEBUG
                                                                                    if (new_bunch == old_block_bunch_slice->bunch())
                                                                                    {   // the block_bunch-slice has been updated in an earlier call.
                                                                                        assert(new_block_bunch_pos + 1 < old_block_bunch_slice->end);
                                                                                        assert(old_block_bunch_slice == new_block_bunch_pos[1].slice());
                                                                                    }
                                                                                    else
                                                                                    {   assert(*old_block_bunch_slice->bunch() < *new_bunch);
                                                                                        assert(old_block_bunch_slice->end <= new_block_bunch_pos);
                                                                                    }
                                                                                #endif
        if (new_block_bunch_pos + 1 < block_bunch_inert_begin &&
                       old_block_bunch_slice == new_block_bunch_pos[1].slice())
        {
            // This transition is not the last in the block_bunch-slice.
            return;
        }
                                                                                assert(old_block_bunch_slice->is_stable());
        // This transition is the last in the block_bunch-slice.  We do all
        // necessary work for this block_bunch-slice upon finding this
        // transition.  Because every block_bunch-slice has exactly one such
        // transition, all work is executed exactly once.
        if (old_block_bunch_slice->empty())
        {
            // The old block_bunch-slice is empty, reuse it as the new one.
            old_block_bunch_slice->end = new_block_bunch_pos + 1;
            old_block_bunch_slice->bunch = new_bunch;

            // Because now every bottom state has a transition in the new
            // bunch, and no state has a transition in the old bunch, there is
            // no need to refine this block.  So we don't make this
            // block_bunch-slice unstable.

            // unmark the states
            source->block->marked_nonbottom_begin =source->block->end;
            source->block->marked_bottom_begin=source->block->nonbottom_begin;

            return;
        }

        // Create a new block_bunch-slice and set the pointers accordingly
        block_bunch_slice_iter_t new_block_bunch_slice;
        if (1 >= source->block->size())
        {
            // the new block_bunch-slice is stable

            // unmark the state
            source->block->marked_bottom_begin=source->block->nonbottom_begin;  assert(source->block->marked_nonbottom_begin == source->block->end);

            // Insert at the appropriate position in the list of stable
            // block_bunch-slices:  immediately after the old
            // block_bunch-slice.
            new_block_bunch_slice = source->block->stable_block_bunch.
                    emplace(std::next(old_block_bunch_slice),
                                     new_block_bunch_pos + 1, new_bunch, true);
        }
        else
        {
            // The old block_bunch-slice becomes unstable, and the new
            // block_bunch-slice is created unstable.

            // Line 1.16: Add the block_bunch-slice of splitter_bunch where
            //            s --> t was in to the list of unstable
            //            block_bunch-slices
            unstable_block_bunch.splice(unstable_block_bunch.begin(),
                     source->block->stable_block_bunch, old_block_bunch_slice);
            old_block_bunch_slice->make_unstable();

            // Line 1.18: Add the block_bunch-slice of new_bunch where
            //            s --> t is now in to the list of unstable
            //            block_bunch-slices
            unstable_block_bunch.emplace_front(new_block_bunch_pos + 1,
                                                             new_bunch, false);
            new_block_bunch_slice = unstable_block_bunch.begin();
        }
                                                                                #ifndef NDEBUG
        /* set the pointers accordingly                                      */     new_block_bunch_slice->work_counter = old_block_bunch_slice->work_counter;
                                                                                #endif
        block_bunch_iter_t block_bunch_iter = old_block_bunch_slice->end;
        do
        {                                                                       assert(block_bunch_iter->slice() == old_block_bunch_slice);
                                                                                assert(block_bunch_iter->pred->action_block->succ()->block_bunch ==
                                                                                                                                             block_bunch_iter);
                block_bunch_iter->slice.change(new_block_bunch_slice);          assert(block_bunch_iter->pred->source->block == source->block);
                                                                                // mCRL2complexity(block_bunch_iter->pred, ...) -- subsumed by the call below
        }
        while (++block_bunch_iter <= new_block_bunch_pos);                      mCRL2complexity(new_block_bunch_slice, add_work(
                                                                                        bisim_gjkw::check_complexity::second_move_transition_to_new_bunch,
                                                                                                                  new_bunch->max_work_counter(partitioner)), );
    }


  private:
    /// \brief Adapt the non-inert transitions in an out-slice to a new block
    /// \details After a block has been split, the outgoing transitions of the
    /// new block need to move to the respective block_bunch-slice of the new
    /// block.
    ///
    /// This function handles all transitions in the out-slice just before
    /// `out_slice_end`, as they all belong to the same block_bunch-slice and
    /// can be moved together.
    ///
    /// The function returns the beginning of this out-slice (which can become
    /// the next out_slice_end).  It is meant to be called from the last
    /// out-slice back to the first because it inserts stable
    /// block_bunch-slices at the beginning of the list for the new block, so
    /// it would normally become ordered according to the bunch.
                                                                                ONLY_IF_DEBUG( template <class LTS_TYPE> )
    succ_iter_t move_out_slice_to_new_block(                                    ONLY_IF_DEBUG( const bisim_partitioner_dnj<LTS_TYPE>& partitioner, )
                        succ_iter_t out_slice_end, block_t* const old_block,
                            block_bunch_slice_const_iter_t const last_splitter)
    {                                                                           assert(succ_entry::succ_begin < out_slice_end);
        succ_iter_t const out_slice_begin =
                                       out_slice_end[-1].begin_or_before_end(); assert(out_slice_begin < out_slice_end);
                                                                                assert(out_slice_begin->block_bunch->pred->action_block->succ() ==
                                                                                                                                              out_slice_begin);
        block_bunch_iter_t old_block_bunch_pos = out_slice_end[-1].block_bunch; assert(old_block_bunch_pos->pred->action_block->succ()->block_bunch ==
                                                                                                                                          old_block_bunch_pos);
        block_bunch_slice_iter_t old_block_bunch_slice =
                                                  old_block_bunch_pos->slice();
        if (last_splitter == old_block_bunch_slice)  return out_slice_begin;

        state_info_iter_t const source = old_block_bunch_pos->pred->source;     assert(out_slice_end <= source->succ_inert.begin);
        block_t* new_block = source->block;                                     assert(source == out_slice_begin->block_bunch->pred->source);
                                                                                assert(*source->pos == source);
        block_bunch_iter_t after_new_block_bunch_pos =
                                                    old_block_bunch_slice->end;
        block_bunch_slice_iter_t new_block_bunch_slice;
        if (after_new_block_bunch_pos >= block_bunch_inert_begin ||
           (new_block_bunch_slice = after_new_block_bunch_pos->slice(),
            new_block != after_new_block_bunch_pos->pred->source->block ||
             old_block_bunch_slice->bunch() != new_block_bunch_slice->bunch()))
        {
            // the new block_bunch-slice is not suitable; create a new one
            // and insert it into the correct list.
            if (old_block_bunch_slice->is_stable())
            {
                // In most cases, but not always, the source is a bottom state.
                new_block->stable_block_bunch.emplace_front(
                                         old_block_bunch_slice->end,
                                         old_block_bunch_slice->bunch(), true);
                new_block_bunch_slice = new_block->stable_block_bunch.begin();
            }
            else
            {
                new_block_bunch_slice = unstable_block_bunch.emplace(std::next(
                            old_block_bunch_slice), old_block_bunch_slice->end,
                                        old_block_bunch_slice->bunch(), false);
            }                                                                   ONLY_IF_DEBUG( new_block_bunch_slice->work_counter =
                                                                                                                         old_block_bunch_slice->work_counter; )
        }                                                                       ONLY_IF_DEBUG( unsigned const max_counter=bisim_gjkw::check_complexity::log_n-
        /* move all transitions in this out-slice to the new block_bunch     */                       bisim_gjkw::check_complexity::ilog2(new_block->size()); )
        do
        {
            --out_slice_end;
            --after_new_block_bunch_pos;                                        assert(old_block_bunch_pos->slice() == old_block_bunch_slice);
            after_new_block_bunch_pos->slice.change(new_block_bunch_slice);
            std::swap(old_block_bunch_pos->pred,
                                              after_new_block_bunch_pos->pred);
            old_block_bunch_pos->pred->action_block->succ()->block_bunch =
                                                           old_block_bunch_pos; assert(out_slice_end == after_new_block_bunch_pos->pred->action_block->succ());
            out_slice_end->block_bunch = after_new_block_bunch_pos;             mCRL2complexity(out_slice_end->block_bunch->pred, add_work(bisim_gjkw::
                                                                                     check_complexity::move_out_slice_to_new_block, max_counter), partitioner);
        }
        while (out_slice_begin < out_slice_end &&
                  (old_block_bunch_pos = out_slice_end[-1].block_bunch, true));
        old_block_bunch_slice->end = after_new_block_bunch_pos;
        if (old_block_bunch_slice->empty())
        {
            if (old_block_bunch_slice->is_stable())
            {
                // If the new block is red, then the old (blue) block loses
                // exactly one stable block_bunch-slice, namely the one we just
                // stabilised for (last_splitter).  We could perhaps optimize
                // this by moving that slice as a whole to the new block --
                // perhaps later.
                //
                // If the new block is blue, then the old (red) block loses no
                // stable block_bunch-slices if it contains any bottom state.
                // If it doesn't contain any bottom state, it will definitely
                // keep last_splitter, but nothing else can be guaranteed.
                //
                // So old_block_bunch_slice may be deleted, in particular if
                // the new block is blue, but not exclusively.
                old_block->stable_block_bunch.erase(old_block_bunch_slice);
            }
            else
            {
                unstable_block_bunch.erase(old_block_bunch_slice);
            }
        }
        return out_slice_begin;
    }


    /// \brief handle one transition after a block has been split
    /// \details The main task of this method is to move the transition to the
    /// correct place in the action_block slice.
    ///
    /// This function handles phase 1.  Because the new action_block-slice
    /// cannot be adapted completely until all transitions into the new block
    /// have been handled through phase 1, the next function handles them again
    /// in phase 2.
    void first_move_transition_to_new_action_block(pred_iter_t const pred_iter)
    {
        action_block_iter_t const old_action_block_pos=pred_iter->action_block; assert(old_action_block_pos->succ()->block_bunch->pred == pred_iter);
        action_block_iter_t const action_block_slice_begin =
                              old_action_block_pos->action_block_slice_begin(); assert(action_block_slice_begin->succ()->block_bunch->pred->action_block ==
                                                                                                                                     action_block_slice_begin);
        action_block_iter_t const new_action_block_pos =
                               action_block_slice_begin->begin_or_before_end(); assert(action_block_slice_begin==new_action_block_pos->begin_or_before_end());
                                                                                assert(new_action_block_pos->succ()->block_bunch->pred->action_block ==
        /* move the transition to the end of the action_block-slice          */                                                          new_action_block_pos);
        if (old_action_block_pos < new_action_block_pos)
        {
            succ_iter_t temp = new_action_block_pos->succ();
            new_action_block_pos->succ.change(old_action_block_pos->succ());
            old_action_block_pos->succ.change(temp);
            temp->block_bunch->pred->action_block = old_action_block_pos;       assert(pred_iter == new_action_block_pos->succ()->block_bunch->pred);
            pred_iter->action_block = new_action_block_pos;

        // adapt the old action_block-slice immediately
            action_block_slice_begin->begin_or_before_end.change(
                                                     new_action_block_pos - 1);
        }
        else
        {                                                                       assert(old_action_block_pos == new_action_block_pos);
            if (action_block_slice_begin < new_action_block_pos)
            {
                // The old action_block-slice is not empty, so we have to adapt
                // the pointer at the beginning.  (If it is empty, it may
                // happen that `new_action_block_pos - 1` is an illegal value.)
                action_block_slice_begin->begin_or_before_end.change(
                                                     new_action_block_pos - 1);
            }
        }
                                                                                assert(pred_iter == new_action_block_pos->succ()->block_bunch->pred);
        // adapt the new action_block-slice, as far as is possible now
            // make the begin_or_before_end pointers of the first and last
            // transition in the slice correct immediately.  The other
            // begin_or_before_end pointers need to be corrected after all
            // transitions in the new bunch have been positioned correctly.
        if (new_action_block_pos + 1 >= action_block_inert_begin ||
            new_action_block_pos[1].succ.is_null() ||
            new_action_block_pos[1].succ()->bunch() !=
                                       new_action_block_pos->succ()->bunch() ||
            new_action_block_pos[1].succ()->block_bunch->pred->target->block !=
                                                      pred_iter->target->block)
        {
            // This is the first transition that moves to this new
            // action_block-slice.
            new_action_block_pos->begin_or_before_end.change(
                                                         new_action_block_pos);
        }
        else
        {
            action_block_iter_t const action_block_slice_before_end =
                                 new_action_block_pos[1].begin_or_before_end(); assert(new_action_block_pos < action_block_slice_before_end);
                                                                                assert(action_block_slice_before_end->succ()->block_bunch->pred->action_block==
                                                                                                                                action_block_slice_before_end);
                                                                                assert(new_action_block_pos + 1 ==
                                                                                                         action_block_slice_before_end->begin_or_before_end());
            action_block_slice_before_end->begin_or_before_end.change(
                                                         new_action_block_pos); assert(action_block_slice_before_end->succ()->block_bunch->
                                                                                                              pred->target->block == pred_iter->target->block);
            new_action_block_pos->begin_or_before_end.change(
                                                action_block_slice_before_end); assert(action_block_slice_before_end < action_block_inert_begin);
        }
    }


    /// \brief handle one transition after a block has been split, phase 2
    /// \details Because the new action_block-slice cannot be adapted
    /// completely until all transitions into the new block have been handled
    /// through phase 1 see the previous function), this function handles them
    /// again in phase 2.
    static void second_move_transition_to_new_action_block(pred_iter_t const
                                                                     pred_iter)
    {
        action_block_iter_t const new_action_block_pos=pred_iter->action_block; assert(new_action_block_pos->succ()->block_bunch->pred == pred_iter);
        action_block_iter_t const old_begin_or_before_end =
                                   new_action_block_pos->begin_or_before_end(); assert(old_begin_or_before_end->succ()->block_bunch->pred->action_block ==
                                                                                                                                      old_begin_or_before_end);
        action_block_iter_t const new_begin_or_before_end =
                                old_begin_or_before_end->begin_or_before_end(); assert(new_begin_or_before_end->succ()->block_bunch->pred->action_block ==
                                                                                                                                      new_begin_or_before_end);
        if (new_begin_or_before_end < new_action_block_pos)
        {                                                                       assert(old_begin_or_before_end ==
                                                                                                               new_begin_or_before_end->begin_or_before_end());
            new_action_block_pos->begin_or_before_end.change(
                                                      new_begin_or_before_end); assert(new_action_block_pos <= old_begin_or_before_end);
            return;
        }                                                                       assert(new_begin_or_before_end == new_action_block_pos);
        if (old_begin_or_before_end < new_action_block_pos)  return;

        // this is the first transition in the new action_block-slice.
        // Check whether the bunch it belongs to has become nontrivial.
        bunch_t* bunch = new_action_block_pos->succ()->bunch();
        if (!bunch->is_trivial())  return;                                      assert(old_begin_or_before_end + 1 == bunch->end);
        if (bunch->begin < new_action_block_pos)
        {
            bunch->make_nontrivial();
        }
    }


    /// \brief adapt data structures for a transition that has become non-inert
    /// \details If the action_block-slice and the block_bunch-slice that
    /// precede the inert transitions in the respective arrays fit, the
    /// transition is added to these arrays instead of creating a new one.
    /// This only works if:
    /// - the action_block-slice has the same target block and the same action
    ///   as old_pred_pos
    /// - the block_bunch-slice has the same source block as old_pred_pos
    /// - the bunch must contain the action_block-slice.
    /// If only the last two conditions are fulfilled, we can start a new
    /// action_block-slice in the same bunch.  (It would be best for this if
    /// the red subblock's block_bunch-slice would be the new one, because that
    /// would generally allow to add the new non-inert transitions to the last
    /// splitter.)
    ///
    /// The state is only marked if is becomes a new bottom state.  Otherwise,
    /// the marking/unmarking of the state is unchanged.
    /// \returns true iff the state became a new bottom state
    bool make_noninert(pred_iter_t const old_pred_pos, iterator_or_null<
                 block_bunch_slice_iter_t>* const new_noninert_block_bunch_ptr)
    {
        action_block_iter_t const old_action_block_pos =
                                                    old_pred_pos->action_block;
        succ_iter_t const old_succ_pos = old_action_block_pos->succ();
        block_bunch_iter_t const old_block_bunch_pos=old_succ_pos->block_bunch; assert(old_pred_pos == old_block_bunch_pos->pred);

        state_info_iter_t const source = old_pred_pos->source;                  assert(*source->pos == source);
        state_info_iter_t const target = old_pred_pos->target;                  assert(*target->pos == target);
        block_t* const source_block = source->block;                            assert(source_block->nonbottom_begin <= source->pos);

        pred_iter_t const new_pred_pos = target->pred_inert.begin++;            assert(new_pred_pos->action_block->succ()->block_bunch->pred == new_pred_pos);
        action_block_iter_t new_action_block_pos = action_block_inert_begin++;  assert(new_action_block_pos->succ()->block_bunch->pred->action_block ==
                                                                                                                                             new_action_block_pos);
        succ_iter_t const new_succ_pos = source->succ_inert.begin++;            assert(new_succ_pos->block_bunch->pred->action_block->succ() == new_succ_pos);
        block_bunch_iter_t const new_block_bunch_pos=block_bunch_inert_begin++; assert(new_block_bunch_pos->pred->action_block->succ()->block_bunch ==
                                                                                                                                          new_block_bunch_pos);
        // adapt pred
        if (new_pred_pos < old_pred_pos)
        {
            // We need std::swap here to swap the whole content, including work
            // counters in case we measure the work.
            std::swap(*old_pred_pos, *new_pred_pos);                            assert(old_pred_pos->action_block->succ()->block_bunch->pred == new_pred_pos);
            old_pred_pos->action_block->succ()->block_bunch->pred=old_pred_pos;
        }                                                                       else  assert(new_pred_pos == old_pred_pos);
        new_pred_pos->action_block = new_action_block_pos;

        /* adapt action_block                                                */ assert(new_action_block_pos->begin_or_before_end.is_null());
        if (new_action_block_pos < old_action_block_pos)
        {                                                                       assert(old_action_block_pos->begin_or_before_end.is_null());
            old_action_block_pos->succ.change(new_action_block_pos->succ());    assert(old_action_block_pos->succ()->block_bunch->pred->action_block ==
                                                                                                                                         new_action_block_pos);
            old_action_block_pos->succ()->block_bunch->pred->action_block =
                                                          old_action_block_pos;
        }                                                                       else  assert(new_action_block_pos == old_action_block_pos);
        new_action_block_pos->succ.change(new_succ_pos);
        // new_action_block_pos->begin_or_before_end.set(...); -- see below

        /* adapt succ                                                        */ assert(new_succ_pos->begin_or_before_end.is_null());
        if (new_succ_pos < old_succ_pos)
        {                                                                       assert(old_succ_pos->begin_or_before_end.is_null());
            old_succ_pos->block_bunch = new_succ_pos->block_bunch;              assert(old_succ_pos->block_bunch->pred->action_block->succ() == new_succ_pos);
            old_succ_pos->block_bunch->pred->action_block->succ.change(
                                                                 old_succ_pos);
        }                                                                       else  assert(new_succ_pos == old_succ_pos);
        new_succ_pos->block_bunch = new_block_bunch_pos;
        // new_succ_pos->begin_or_before_end.set(...); -- see below

        // make the state a bottom state if necessary
        bool became_bottom = false;                                             assert(succ_entry::succ_end->block_bunch->pred->source != source);
        if (source != source->succ_inert.begin->block_bunch->pred->source)
        {
            // make the state a marked bottom state
            if (source->pos >= source_block->marked_nonbottom_begin)
            {
                iter_swap(source->pos, source_block->marked_nonbottom_begin++);
            }                                                                   assert(source->pos < source->block->marked_nonbottom_begin);
            iter_swap(source->pos, source_block->nonbottom_begin++);
            became_bottom = true;
        }

        /* adapt block_bunch                                                 */ assert(new_block_bunch_pos->slice.is_null());
        if (new_block_bunch_pos < old_block_bunch_pos)
        {                                                                       assert(old_block_bunch_pos->slice.is_null());
            old_block_bunch_pos->pred = new_block_bunch_pos->pred;              assert(old_block_bunch_pos->pred->action_block->succ()->block_bunch ==
                                                                                                                                          new_block_bunch_pos);
            old_block_bunch_pos->pred->action_block->succ()->block_bunch =
                                                           old_block_bunch_pos;
        }                                                                       else  assert(new_block_bunch_pos == old_block_bunch_pos);
        new_block_bunch_pos->pred = new_pred_pos;
        // new_block_bunch_pos->slice = ...; -- see below

        bunch_t* new_noninert_bunch;
        if (!new_noninert_block_bunch_ptr->is_null())
        {
            // There is already some new non-inert transition from this block.
            // So we can reuse this block_bunch and its bunch.
            // (However, it may be the case that the current transition goes to
            // another block; in the latter case, we have to create a new
            // action_block-slice.)

            // extend the bunch
            new_noninert_bunch = (*new_noninert_block_bunch_ptr)()->bunch();    assert(new_action_block_pos == new_noninert_bunch->end ||
                                                                                                        (new_action_block_pos[-1].succ.is_null() &&
                                                                                                         new_action_block_pos - 1 == new_noninert_bunch->end));
            new_noninert_bunch->end = action_block_inert_begin;
                                                                                assert((*new_noninert_block_bunch_ptr)()->is_stable());
            /* extend the block_bunch-slice                                  */ assert((*new_noninert_block_bunch_ptr)()->end == new_block_bunch_pos);
            (*new_noninert_block_bunch_ptr)()->end = block_bunch_inert_begin;
            new_block_bunch_pos->slice.set((*new_noninert_block_bunch_ptr)());

            /* adapt the action_block-slice                                  */ assert(new_noninert_bunch->begin < new_action_block_pos);
            if (!new_action_block_pos[-1].succ.is_null() && target->block ==
             new_action_block_pos[-1].succ()->block_bunch->pred->target->block)
            {
                // the action_block-slice is suitable: extend it
                action_block_iter_t const action_block_slice_begin =
                                new_action_block_pos[-1].begin_or_before_end(); assert(new_action_block_pos - 1 ==
                                                                                                              action_block_slice_begin->begin_or_before_end());
                                                                                assert(action_block_slice_begin->succ()->block_bunch->pred->action_block ==
                                                                                                                                     action_block_slice_begin);
                action_block_slice_begin->begin_or_before_end.change(
                                                         new_action_block_pos);
                new_action_block_pos->begin_or_before_end.set(
                                                     action_block_slice_begin);
            }
            else
            {
                // create a new action_block-slice
                new_action_block_pos->begin_or_before_end.set(
                                                         new_action_block_pos);
                if (new_noninert_bunch->is_trivial())
                {                                                               // Only during initialisation, it may happen that we add new non-inert
                    new_noninert_bunch->make_nontrivial();                      // transitions to a nontrivial bunch:
                }                                                               else assert(action_block_entry::action_block_begin==new_noninert_bunch->begin);
            }

            /* adapt the out-slice                                           */ assert(source != succ_entry::succ_begin[-1].block_bunch->pred->source);
            if (source == new_succ_pos[-1].block_bunch->pred->source &&
                                new_succ_pos[-1].bunch() == new_noninert_bunch)
            {
                // the out-slice is suitable: extend it.
                succ_iter_t out_slice_begin =
                                        new_succ_pos[-1].begin_or_before_end(); assert(new_succ_pos - 1 == out_slice_begin->begin_or_before_end());
                out_slice_begin->begin_or_before_end.change(new_succ_pos);      assert(out_slice_begin->block_bunch->pred->action_block->succ() ==
                                                                                                                                              out_slice_begin);
                new_succ_pos->begin_or_before_end.set(out_slice_begin);
                return became_bottom;
            }
        }
        else
        {
            // create a new bunch for noninert transitions
            new_noninert_bunch = new bunch_t(new_action_block_pos,
                   action_block_inert_begin,
                   new_action_block_pos - action_block.end() + TRANS_TYPE_MAX);

            // create a new block_bunch-slice
            // (While it's technically unstable, we immediately stabilise for
            // it afterwards.  So we already insert it in the list of stable
            // slices.)
            block_bunch_slice_iter_t new_noninert_block_bunch = source_block->
             stable_block_bunch.emplace(source_block->stable_block_bunch.end(),
                            block_bunch_inert_begin, new_noninert_bunch, true);
            new_block_bunch_pos->slice.set(new_noninert_block_bunch);
            new_noninert_block_bunch_ptr->set(new_noninert_block_bunch);

            // create a new action_block-slice
            new_action_block_pos->begin_or_before_end.set(
                                                         new_action_block_pos);
        }                                                                       assert(succ_entry::succ_begin == new_succ_pos ||
                                                                                                new_succ_pos[-1].block_bunch->pred->source < source ||
        /* create a new out-slice                                            */                               *new_succ_pos[-1].bunch() < *new_noninert_bunch);
        new_succ_pos->begin_or_before_end.set(new_succ_pos);
        return became_bottom;
    }


  public:
    /// \brief Split all data structures after a new block has been created
    /// \details This function splits the block_bunch- and action_block-slices
    /// to reflect that some transitions now start or end in the new block.
    /// They can no longer be in the same slice as the transitions that start
    /// or end in the old block, respectively.  It also marks the transitions
    /// that have become non-inert as such and finds new bottom states.
    ///
    /// Its time complexity is O(1 + |in(new_block)| + |out(new_block)|).
    /// \param new_block  the new block
    /// \param old_block  the old block (from which new_block was split off)
    /// \param new_noninert_block_bunch  indicates to which block_bunch-slice
    ///                                  new non-inert transitions should be
    ///                                  added:  if this parameter is null,
    ///                                  a new slice is created; if it is not
    ///                                  null, new non-inert transitions are
    ///                                  added to this block_bunch-slice (which
    ///                                  must be in the correct position to
    ///                                  allow this).
    /// \param last_splitter  the splitter that caused new_block and old_block
    ///                       to separate from each other
    /// \param new_block_mode  indicates whether the new block is blue or red
                                                                                ONLY_IF_DEBUG( template<class LTS_TYPE> )
    void adapt_transitions_for_new_block(block_t* const new_block,
        block_t* const old_block,                                               ONLY_IF_DEBUG( const bisim_partitioner_dnj<LTS_TYPE>& partitioner, )
	bool use_splitter_for_new_noninert_block_bunch,
                        block_bunch_slice_iter_t const last_splitter,
                                    enum new_block_mode_t const new_block_mode)
    {                                                                           assert(last_splitter->is_stable());
        iterator_or_null<block_bunch_slice_iter_t> new_noninert_block_bunch;
        if (use_splitter_for_new_noninert_block_bunch)
        {
            new_noninert_block_bunch.set(last_splitter);
        }
        // We begin with a bottom state so the new block gets a sorted list of
        // stable block_bunch-slices.
        permutation_iter_t s_iter = new_block->begin;                           assert(s_iter < new_block->end);
        do
        {
            state_info_iter_t const s = *s_iter;                                assert(new_block == s->block);
                                                                                assert(s->pos == s_iter);
            /* -  -  -  -  -  -  adapt part_tr.block_bunch  -  -  -  -  -  - */
                                                                                assert(s != succ_entry::succ_begin[-1].block_bunch->pred->source);
        // Line 2.29: for all outgoing non-inert transitions of the new block
        //            do
            for (succ_iter_t succ_iter = s->succ_inert.begin;
                                s == succ_iter[-1].block_bunch->pred->source; )
            {
                // Line 2.30: Create a new block bunch-slice if necessary
                // and
                // Line 2.31: Move the transition to the new block_bunch-slice
                succ_iter = move_out_slice_to_new_block(                        ONLY_IF_DEBUG( partitioner, )
                                          succ_iter, old_block, last_splitter); assert(succ_iter->block_bunch->pred->action_block->succ() == succ_iter);
        // Line 2.32: end for                                                   // add_work_to_out_slice(succ_iter, ...) -- subsumed in the call below
            }

            /*-  -  -  -  -  -  adapt part_tr.action_block  -  -  -  -  -  -*/
                                                                                assert(s != pred_entry::pred_begin[-1].target);
        // Line 2.33: for all incoming non-inert transitions of the new block
        //            do
            for (pred_iter_t pred_iter = s->pred_inert.begin;
                                                  s == (--pred_iter)->target; )
            {                                                                   assert(pred_entry::pred_begin <= pred_iter);
                /* Line 2.34: Create a new action block-slice if necessary   */ assert(pred_iter->action_block->succ()->block_bunch->pred == pred_iter);
                // and
                // Line 2.35: Move the transition to the new action block-slice
                first_move_transition_to_new_action_block(pred_iter);           // mCRL2complexity(pred_iter, ...) -- subsumed in the call below
        // Line 2.36: end for
            }                                                                   // mCRL2complexity(s, ...) -- subsumed in the call at the end
        }
        while (++s_iter < new_block->end);
        if (new_block_is_red == new_block_mode)
        {                                                                       assert(last_splitter->source_block() == new_block);
            // The last_splitter slice moves completely from the old to the new
            // block.  We move it as a whole to the new block_bunch list.
            new_block->stable_block_bunch.splice(
                    new_block->stable_block_bunch.begin(),
                                 old_block->stable_block_bunch, last_splitter);
        }                                                                       else assert(last_splitter->source_block() == old_block);

        // We cannot join the loop above with the one below because transitions
        // in the action_block-slices need to be handled in two phases.

        // Line 2.33: for all incoming non-inert transitions of the new block do
        for (permutation_iter_t s_iter = new_block->begin;
                                             s_iter < new_block->end; ++s_iter)
        {
            state_info_iter_t const s = *s_iter;                                assert(s->pos == s_iter);  assert(s != pred_entry::pred_begin[-1].target);
            for (pred_iter_t pred_iter = s->pred_inert.begin;
                                                  (--pred_iter)->target == s; )
            {                                                                   assert(pred_entry::pred_begin <= pred_iter);
                /* Line 2.34: Create a new action block-slice if necessary   */ assert(pred_iter->action_block->succ()->block_bunch->pred == pred_iter);
                // and
                // Line 2.35: Move the transition to the new action block-slice
                second_move_transition_to_new_action_block(pred_iter);          // mCRL2complexity(pred_iter, ...) -- subsumed in the call below
        // Line 2.36: end for
            }                                                                   // mCRL2complexity(s, ...) -- subsumed in the call at the end
        }
                                                                                assert(0 == new_block->marked_size());  assert(0==old_block->marked_size());
        /* -  -  -  -  -  - find new non-inert transitions -  -  -  -  -  - */  assert(block_bunch.end() - block_bunch_inert_begin ==
                                                                                                                action_block.end() - action_block_inert_begin);
        if (block_bunch_inert_begin < block_bunch.end())
        {
            // Line 2.37: if the new block is blue then
            if (new_block_is_blue == new_block_mode)
            {                                                                   assert(old_block == (*new_block->end)->block);
                // Line 2.38: for all incoming inert transitions t --tau--> s
                //                                          of the new block do
                permutation_iter_t target_iter = new_block->begin;              assert(new_block->end < state_info_entry::perm_end);
                do
                {
                    state_info_iter_t const s = *target_iter;                   assert(s->pos == target_iter);
                    // check all incoming inert transitions of s, whether they
                    /* still start in new_block                              */ assert(s != pred_entry::pred_end->target);
                    for (pred_iter_t pred_iter = s->pred_inert.begin;
                                           s == pred_iter->target; ++pred_iter)
                    {                                                           assert(pred_iter < pred_entry::pred_end);
                                                                                assert(pred_iter->action_block->succ()->block_bunch->pred == pred_iter);
                        state_info_iter_t const t = pred_iter->source;          assert(*t->pos == t);
                        // Line 2.39: if t is in the red subblock then ...
                        if (new_block != t->block)
                        {                                                       assert(old_block == t->block);
                            // Line 2.39: ... Make t --tau--> s non-inert ...
                            if (!make_noninert(pred_iter,
                                                    &new_noninert_block_bunch))
                                      //< make_noninert() may modify *pred_iter
                            {
                                // Line 2.39: ... and Mark t
                                old_block->mark_nonbottom(t);
                            }
                        }                                                       // mCRL2complexity(old value of *pred_iter, ...) -- overapproximated by the
                // Line 2.40: end for                                           // call below
                    }                                                           // mCRL2complexity(s, ...) -- subsumed in the call at the end
                }
                while (++target_iter < new_block->end);                         assert(0 < old_block->bottom_size());
            // Line 2.41: else
            }
            else
            {                                                                   assert(new_block_is_red == new_block_mode);
                // Line 2.42: for all outgoing inert transitions s --tau--> t
                //                                          of the new block do
                    /* We have to be careful because make_noninert may make a*/ assert(old_block == new_block->begin[-1]->block);
                    /* state move either forward (to the marked states) or   */ assert(state_info_entry::perm_begin < new_block->begin);
                    /* back (to the bottom states).                          */ assert(0 < old_block->bottom_size());
                for (permutation_iter_t source_iter=new_block->nonbottom_begin;
                             source_iter < new_block->marked_nonbottom_begin; )
                {
                    state_info_iter_t const s = *source_iter;                   assert(s->pos == source_iter);
                    // check all outgoing inert transitions of s, whether they
                    /* still end in new_block.                               */ assert(succ_entry::succ_end->block_bunch->pred->source != s);
                    succ_iter_t succ_iter = s->succ_inert.begin;                assert(succ_iter < succ_entry::succ_end);
                    bool dont_mark = true;                                      assert(s == succ_iter->block_bunch->pred->source);
                    do
                    {                                                           assert(succ_iter->block_bunch->pred->action_block->succ() == succ_iter);
                        // 2.43: if t is in the blue subblock then ...
                        if (new_block !=
                                   succ_iter->block_bunch->pred->target->block)
                        {                                                       assert(old_block == succ_iter->block_bunch->pred->target->block);
                            // Line 2.43: Make s --tau--> t non-inert ...
                            dont_mark = make_noninert(
                                                  succ_iter->block_bunch->pred,
                                                    &new_noninert_block_bunch);
                        }                                                        // mCRL2complexity(succ_iter->block_bunch->pred, ...) -- overapproximated by
                // Line 2.44: end for                                            // the call below
                    }
                    while (s == (++succ_iter)->block_bunch->pred->source);
                    // Line 2.43: ... and Mark s
                    if (dont_mark)  ++source_iter;
                    else  new_block->mark_nonbottom(s);                         assert(new_block->nonbottom_begin <= source_iter);
                                                                                // mCRL2complexity(s, ...) -- overapproximated by the call at the end
                }
            // Line 2.45: end if
            }
        }                                                                       else  assert(block_bunch_inert_begin == block_bunch.end());
                                                                                mCRL2complexity(new_block, add_work(bisim_gjkw::check_complexity::
                                                                                    adapt_transitions_for_new_block, bisim_gjkw::check_complexity::log_n -
                                                                                                    bisim_gjkw::check_complexity::ilog2(new_block->size())), );
    }
                                                                                #ifndef NDEBUG
                                                                                    /// \brief print all transitions
                                                                                    /// \details Transitions are printed organised into bunches.
                                                                                    template <class LTS_TYPE>
                                                                                    void print_trans(const bisim_partitioner_dnj<LTS_TYPE>& partitioner) const
                                                                                    {
                                                                                        if (!mCRL2logEnabled(log::debug, "bisim_dnj"))  return;
                                                                                        // for all bunches
                                                                                        action_block_const_iter_t bunch_begin =
                                                                                                                        action_block_entry::action_block_begin;
                                                                                        do
                                                                                        {
                                                                                            if (bunch_begin >= action_block.cend())
                                                                                            {
                                                                                                mCRL2log(log::debug, "bisim_dnj") << "No transitions.\n";
                                                                                                return;
                                                                                            }
                                                                                        } while (bunch_begin->succ.is_null() &&
                                                                                           (assert(bunch_begin->begin_or_before_end.is_null()), ++bunch_begin,
                                                                                            true));
                                                                                        do
                                                                                        {
                                                                                            action_block_const_iter_t bunch_end;
                                                                                            action_block_const_iter_t action_block_slice_end;
                                                                                            if (bunch_begin->succ()->block_bunch->slice.is_null())
                                                                                            {
                                                                                                mCRL2log(log::debug, "bisim_dnj") <<"Inert transition slice [";
                                                                                                action_block_slice_end = bunch_end = action_block.cend();
                                                                                            }
                                                                                            else
                                                                                            {
                                                                                                const bunch_t* const bunch = bunch_begin->succ()->bunch();
                                                                                                mCRL2log(log::debug, "bisim_dnj") << bunch->debug_id_short()
                                                                                                                                << ":\n\taction_block-slice [";
                                                                                                bunch_end = bunch->end;
                                                                                                assert(bunch->begin == bunch_begin);
                                                                                                assert(bunch_begin < bunch_end);
                                                                                                assert(bunch_end <= action_block_inert_begin);

                                                                                                action_block_slice_end = bunch_begin->begin_or_before_end()+1;
                                                                                            }
                                                                                            assert(action_block_slice_end <= bunch_end);
                                                                                            // for all transition sets in Bu
                                                                                            action_block_const_iter_t action_block_slice_begin = bunch_begin;
                                                                                            do
                                                                                            {
                                                                                                mCRL2log(log::debug,"bisim_dnj") << (action_block_slice_begin -
                                                                                                                 action_block_entry::action_block_begin) << ","
                                                                                                    << (action_block_slice_end -
                                                                                                             action_block_entry::action_block_begin) << "):\n";
                                                                                                // for all transitions in Tr
                                                                                                for(action_block_const_iter_t tr_iter=action_block_slice_begin;
                                                                                                                   tr_iter < action_block_slice_end; ++tr_iter)
                                                                                                {
                                                                                                    mCRL2log(log::debug, "bisim_dnj") << "\t\t"
                                                                                                                << tr_iter->succ()->block_bunch->
                                                                                                                           pred->debug_id(partitioner) << '\n';
                                                                                                }
                                                                                                // go to next transition set
                                                                                                action_block_slice_begin = action_block_slice_end;
                                                                                                while (action_block_slice_begin < bunch_end &&
                                                                                                                      action_block_slice_begin->succ.is_null())
                                                                                                {
                                                                                                    assert(action_block_slice_begin->
                                                                                                                                begin_or_before_end.is_null());
                                                                                                    ++action_block_slice_begin;
                                                                                                    assert(action_block_slice_begin < bunch_end);
                                                                                                }
                                                                                                if (action_block_slice_begin >= bunch_end)  break;
                                                                                                action_block_slice_end =
                                                                                                           action_block_slice_begin->begin_or_before_end() + 1;
                                                                                                mCRL2log(log::debug, "bisim_dnj") << "\taction_block-slice [";
                                                                                            }
                                                                                            while (true);
                                                                                            // go to next bunch
                                                                                            bunch_begin = bunch_end;
                                                                                            while(bunch_begin<action_block.cend()&&bunch_begin->succ.is_null())
                                                                                            {
                                                                                                assert(bunch_begin->begin_or_before_end.is_null());
                                                                                                ++bunch_begin;
                                                                                            }
                                                                                        }
                                                                                        while (bunch_begin < action_block.cend());
                                                                                    }
                                                                                #endif
};

///@} (end of group part_trans)





/* ************************************************************************* */
/*                                                                           */
/*                            A L G O R I T H M S                            */
/*                                                                           */
/* ************************************************************************* */





/// \defgroup part_refine
/// \brief classes to calculate the stutter equivalence quotient of a LTS
///@{



/*=============================================================================
=                                 main class                                  =
=============================================================================*/





enum refine_mode_t { extend_from_marked_states,
                     extend_from_marked_states_for_init_and_postprocess,
                     extend_from_splitter,
                     extend_from_bottom_state_markings_and_splitter };


/// \brief Internal functional to compare new bottom states
/// \details The functions in this class are used to sort and search
/// through the new bottom states.  For efficiency, they are ordered
/// according to their current out-slice (containing transitions to a
/// specific bunch).  That means that new bottom states can quickly be
/// separated into those that can reach that bunch and those that cannot.
static struct {
    /// \brief Comparison operator for the current out-slice of new bottom
    /// states
    /// \details This variant can be used for std::lower_bound, to compare
    /// a current out-slice pointer with a bunch directly.
    bool operator()(state_info_const_iter_t p1, const bunch_t* p2) const
    {                                                                           assert(succ_entry::succ_begin[-1].block_bunch->pred->source != p1);
        return p1->current_out_slice.begin[-1].block_bunch->pred->source!=p1 ||
                                *p1->current_out_slice.begin[-1].bunch() < *p2;
    }

    /// \brief Comparison operator for the current out-slice of new bottom
    /// states
    /// \details This variant can be used for std::sort, to compare the
    /// current out-slices of two states.
    bool operator()(state_info_const_iter_t p1,state_info_const_iter_t p2)const
    {                                                                           assert(succ_entry::succ_begin[-1].block_bunch->pred->source != p2);
        return p2->current_out_slice.begin[-1].block_bunch->pred->source==p2 &&
                       operator()(p1, p2->current_out_slice.begin[-1].bunch());
    }
} before_current_out_slice_less;


/// \class bisim_partitioner_dnj
/// \brief implements the main algorithm for the branching bisimulation
/// quotient
template <class LTS_TYPE>
class bisim_partitioner_dnj
{
  private:
    /// \brief automaton that is being reduced
    LTS_TYPE& aut;

    /// \brief partition of the state space into blocks
    part_state_t part_st;

    /// \brief partitions of the transitions (with bunches and
    /// action_block-slices)
    part_trans_t part_tr;

    /// \brief action label slices
    /// \details In part_tr.action_block, no information about the action label
    /// is actually stored with the transitions, to save memory.  Entry l of
    /// this array contains a pointer to the first entry in
    /// part_tr.action_block with label l.
    ///
    /// During initialisation, entry l of this array contains a counter to
    /// indicate how many non-inert transitions with action label l have been
    /// found.
    bisim_gjkw::fixed_vector<iterator_or_counter<action_block_iter_t> >
                                                                  action_label;

    /// \brief true iff branching (not strong) bisimulation has been requested
    bool const branching;

    /// \brief true iff divergence-preserving branching bisimulation has been
    /// requested
    /// \details Note that this field must be false if strong bisimulation has
    /// been requested.  There is no such thing as divergence-preserving strong
    /// bisimulation.
    bool const preserve_divergence;
                                                                                #ifndef NDEBUG
                                                                                    friend class pred_entry;
                                                                                    friend class bunch_t;
                                                                                #endif
  public:
    /// \brief constructor
    /// \details The constructor constructs the data structures and immediately
    /// calculates the partition corresponding with the bisimulation quotient.
    /// It destroys the transitions on the LTS (to save memory) but does not
    /// adapt the LTS to represent the quotient's transitions.
    /// \param new_aut                 LTS that needs to be reduced
    /// \param new_branching           If true branching bisimulation is used,
    ///                                otherwise strong bisimulation is
    ///                                applied.
    /// \param new_preserve_divergence If true and branching is true, preserve
    ///                                tau loops on states.
    bisim_partitioner_dnj(LTS_TYPE& new_aut, bool new_branching = false,
                                          bool new_preserve_divergence = false)
      : aut(new_aut),
        part_st(new_aut.num_states()),
        part_tr(new_aut.num_transitions(), new_aut.num_action_labels()),
        action_label(new_aut.num_action_labels()),
        branching(new_branching),
        preserve_divergence(new_preserve_divergence)
    {                                                                           assert(branching || !preserve_divergence);
      create_initial_partition();                                               ONLY_IF_DEBUG( action_block_entry::action_block_orig_inert_begin =
                                                                                                                            part_tr.action_block_inert_begin; )
      refine_partition_until_it_becomes_stable();
    }
    ~bisim_partitioner_dnj()  {  }


    /// \brief Calculate the number of equivalence classes
    /// \details The number of equivalence classes (which is valid after the
    /// partition has been constructed) is equal to the number of states in the
    /// bisimulation quotient.
    static state_type num_eq_classes()
    {
        return block_t::nr_of_blocks;
    }


    /// \brief Get the equivalence class of a state
    /// \details After running the minimisation algorithm, this function
    /// produces the number of the equivalence class of a state.  This number
    /// is the same as the number of the state in the minimised LTS to which
    /// the original state is mapped.
    /// \param s state whose equivalence class needs to be found
    /// \returns sequence number of the equivalence class of state s
    state_type get_eq_class(state_type s) const
    {
        return part_st.block(s)->seqnr;
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
    /// \param[out] arbitrary_state_per_block    If this pointer is != nullptr,
    ///                                the function fills the vector with, per
    ///                                equivalence class, the number of an
    ///                                arbitrary original state in the class.
    void finalize_minimized_LTS(std::vector<std::size_t>* const
                                                     arbitrary_state_per_block)
    {
        if (nullptr != arbitrary_state_per_block)
        {
            arbitrary_state_per_block->resize(num_eq_classes());
                                                                                #ifndef NDEBUG
                                                                                    arbitrary_state_per_block->assign(num_eq_classes(), (state_type) -1);
                                                                                #endif
            // for all blocks
            permutation_const_iter_t s_iter = part_st.permutation.cbegin();     assert(s_iter == state_info_entry::perm_begin);
            do
            {
                const block_t* const B = (*s_iter)->block;                      assert(0 <= B->seqnr);  assert(B->seqnr < arbitrary_state_per_block->size());
                                                                                assert((state_type) -1 == (*arbitrary_state_per_block)[B->seqnr]);
                (*arbitrary_state_per_block)[B->seqnr] =
                                         *s_iter - part_st.state_info.cbegin();
                s_iter = B->end;
            }
            while (s_iter < part_st.permutation.cend());                        assert(s_iter == state_info_entry::perm_end);
        }

        // The labels have already been stored in sort_key_and_label.label by
        // refine_partition_until_it_becomes_stable().

        // for all blocks
        permutation_const_iter_t s_iter = part_st.permutation.cbegin();         assert(s_iter == state_info_entry::perm_begin);
        do
        {
            const block_t* const B = (*s_iter)->block;
            // for all block_bunch-slices of the block
            for(block_bunch_slice_const_iter_t
                    trans_iter = B->stable_block_bunch.cbegin();
                      B->stable_block_bunch.cend() != trans_iter; ++trans_iter)
            {                                                                   assert(trans_iter->is_stable());  assert(!trans_iter->empty());
                pred_const_iter_t const pred = trans_iter->end[-1].pred;        assert(pred->source->block == B);
                /* add a transition from the source block to the goal block  */ assert(pred->action_block->succ()->block_bunch->pred == pred);
                /* with the indicated label.                                 */ assert(pred->action_block->succ()->block_bunch->slice() == trans_iter);
                label_type label=trans_iter->bunch()->sort_key_and_label.label; assert(0 <= label);  assert(label < action_label.size());
                aut.add_transition(transition(B->seqnr, label,
                                                  pred->target->block->seqnr));
            }
            s_iter = B->end;
        }
        while (s_iter < part_st.permutation.cend());                            assert(s_iter == state_info_entry::perm_end);

        // Merge the states, by setting the state labels of each state to the
        // concatenation of the state labels of its equivalence class.

        if (aut.has_state_info())   /* If there are no state labels
                                                    this step can be ignored */
        {
            /* Create a vector for the new labels */
            bisim_gjkw::fixed_vector<typename LTS_TYPE::state_label_t>
                                                  new_labels(num_eq_classes());

            for (state_type i = aut.num_states(); i > 0; )
            {
                --i;
                const state_type new_index = get_eq_class(i);
                new_labels[new_index]=aut.state_label(i)+new_labels[new_index];
            }

            aut.set_num_states(num_eq_classes());
            for (state_type i = 0; i < num_eq_classes(); ++i)
            {
                aut.set_state_label(i, new_labels[i]);
            }
        }
        else
        {
            aut.set_num_states(num_eq_classes());
        }

        aut.set_initial_state(get_eq_class(aut.initial_state()));
    }


    /// \brief Check whether two states are in the same equivalence class
    /// \param s, t states that need to be compared
    /// \returns true iff the two states are in the same equivalence class
    bool in_same_class(state_type s, state_type t) const
    {
        return part_st.block(s) == part_st.block(t);
    }
  private:

    /*--------------------------- main algorithm ----------------------------*/

    /// \brief Create a partition satisfying the main invariant
    /// \details Before the actual bisimulation minimisation can start, this
    /// function needs to be called to create a partition that satisfies the
    /// main invariant of the efficient O(m log n) branching bisimulation
    /// minimisation.
    ///
    /// It puts all non-inert transitions into a single bunch, containing one
    /// action_block-slice for each action label.  It creates a single block
    /// (or possibly two, if there are states that never will do any visible
    /// action).  As a side effect, it deletes all transitions from the LTS
    /// that is stored with the partitioner;  information about the transitions
    /// is kept in data structures that are suitable for the efficient
    /// algorithm.
    ///
    /// For divergence-preserving branching bisimulation, we only need to treat
    /// tau-self-loops as non-inert transitions.  In other texts, this is
    /// sometimes described as temporarily renaming the tau-self-loops to
    /// self-loops with a special label.  However, as there are no other
    /// non-inert tau transitions, we can simply put them in their own
    /// action_block-slice, separate from the inert tau transitions.  (It would
    /// be an error to mix the inert transitions with the self-loops in the
    /// same slice.)
    void create_initial_partition()
    {
        // log::mcrl2_logger::set_reporting_level(log::debug, "bisim_dnj");

        mCRL2log(log::verbose, "bisim_dnj") << "Strictly O(m log n) "
             << (branching ? (preserve_divergence
                                           ? "divergence-preserving branching "
                                           : "branching ")
                           : "")
             << "bisimulation partitioner created for " << part_st.state_size()
             << " states and " << aut.num_transitions() << " transitions\n";

        // create one block for all states

        block_t* B = new block_t(
                       part_st.permutation.begin(), part_st.permutation.end()); assert(B->begin == state_info_entry::perm_begin);
                                                                                assert(B->end == state_info_entry::perm_end);
        // Iterate over the transitions to count how to order them in
        // part_trans_t

        // counters for the non-inert outgoing and incoming transitions per
        // state are provided in part_st.state_info.  These counters have been
        // initialised to zero in the constructor of part_state_t.
        // counters for the non-inert transition per label are stored in
        // action_label.
                                                                                assert(action_label.size() == aut.num_action_labels());
        // counter for the total number of inert transitions:
        trans_type inert_transitions = 0;
        for (const transition& t: aut.get_transitions())
        {
            if (branching&&aut.is_tau(aut.apply_hidden_label_map(t.label()))&&  ((
                                                            t.from() != t.to())   || (assert(preserve_divergence), false)))
            {
                // The transition is inert.
                ++part_st.state_info[t.from()].succ_inert.count;
                ++inert_transitions;

                // The source state should become non-bottom:
                if (part_st.state_info[t.from()].pos < B->nonbottom_begin)
                {
                    iter_swap(part_st.state_info[t.from()].pos,
                                                         --B->nonbottom_begin);
                    // we do not yet update the marked_bottom_begin pointer
                }
            }
            else
            {
                // The transition is non-inert.  (It may be a self-loop).
                ++part_st.state_info[t.from()].current_out_slice.count;

                ++action_label[aut.apply_hidden_label_map(t.label())].count;
            }
            ++part_st.state_info[t.to()].pred_inert.count;
        }
        // Now we update the marked_bottom_begin pointer:
        B->marked_bottom_begin = B->nonbottom_begin;

        // set the pointers to transition slices in the state info entries

        // We set them all to the end of the respective slice here.  Then, with
        // every transition, the pointer will be reduced by one, so that after
        // placing all transitions it will point to the beginning of the slice.

        pred_iter_t next_pred_begin = part_tr.pred.begin() + 1;                 assert(next_pred_begin == pred_entry::pred_begin);
        succ_iter_t next_succ_begin = part_tr.succ.begin() + 1;                 assert(next_succ_begin == succ_entry::succ_begin);
        state_info_iter_t state_iter = part_st.state_info.begin();              assert(state_iter == state_info_entry::s_i_begin);
        for (; state_iter < part_st.state_info.end(); ++state_iter)
        {
            state_iter->block = B;

            state_iter->not_.used_pred = next_pred_begin;
            next_pred_begin += state_iter->pred_inert.count;
            state_iter->pred_inert.convert_to_iterator(next_pred_begin);

                // create slice descriptors in part_tr.succ for each state with
                // outgoing transitions.
            state_iter->current_out_slice.convert_to_iterator(next_succ_begin +
                                          state_iter->current_out_slice.count);
            if (next_succ_begin < state_iter->current_out_slice.begin)
            {
                next_succ_begin->begin_or_before_end.set(
                                      state_iter->current_out_slice.begin - 1);
                succ_iter_t out_slice_begin = next_succ_begin;
                while (++next_succ_begin < state_iter->current_out_slice.begin)
                {
                    next_succ_begin->begin_or_before_end.set(out_slice_begin);  // mCRL2complexity(next_succ_begin->block_bunch->pred, ...) -- subsumed in the
                }                                                               // call below

                // Line 1.3: Mark all states with a visible transition
                B->mark(state_iter);
            }
            state_iter->succ_inert.convert_to_iterator(next_succ_begin +
                                                 state_iter->succ_inert.count);
                                                                                #ifndef NDEBUG
                                                                                    while (next_succ_begin < state_iter->succ_inert.begin)
                                                                                    {   assert(next_succ_begin->begin_or_before_end.is_null());
                                                                                        ++next_succ_begin;
                                                                                    }
                                                                                #endif
            next_succ_begin = state_iter->succ_inert.begin;                     // mCRL2complexity(*state_iter, ...) -- subsumed in the call at the end
        }                                                                       assert(state_iter == state_info_entry::s_i_end);

        // Line 1.4: Organise the visible transitions in action block-slices
        //           and one block bunch-slice

        // create a single bunch containing all non-inert transitions

        part_tr.action_block_inert_begin =
                                part_tr.action_block.end() - inert_transitions;
        part_tr.block_bunch_inert_begin =
                                 part_tr.block_bunch.end() - inert_transitions;

        bunch_t* bunch = nullptr;

        // create a single block_bunch entry for all non-inert transitions
        if (part_tr.action_block.begin() < part_tr.action_block_inert_begin)
        {
            bunch = new bunch_t(part_tr.action_block.begin(),
                                 part_tr.action_block_inert_begin,
                                 TRANS_TYPE_MAX - part_tr.action_block.size());
            B->stable_block_bunch.emplace_front(
                                 part_tr.block_bunch_inert_begin, bunch, true);
        }

        // create slice descriptors in part_tr.action_block for each label

        // The action_block array shall have the tau transitions at the end:
        // first the non-inert tau transitions (during initialisation, that are
        // only the tau self-loops), then the tau transitions that have become
        // non-inert and finally the inert transitions.
        // Transitions with other labels are placed from beginning to end.
        // Every such transition block except the last one ends with a dummy
        /* entry.  If there are transition labels without transitions,       */ assert(part_tr.action_block.size() ==
        /* multiple dummy entries will be placed side-by-side.               */                               aut.num_transitions() + action_label.size() - 1);
        action_block_iter_t next_action_label_begin =
                                                  part_tr.action_block.begin(); assert(next_action_label_begin == action_block_entry::action_block_begin);
        label_type num_labels_with_transitions = 0;
        label_type label = action_label.size();
        trans_type const n_square = part_st.state_size()*part_st.state_size();  ONLY_IF_DEBUG( trans_type max_transitions = n_square; )
        bunch_t::first_nontrivial = nullptr;
        do
        {
            --label;
            if (0 < action_label[label].count)
            {
                if (++num_labels_with_transitions == 2)
                {
                    // This is the second action_block-slice, so the bunch is
                    // not yet marked as nontrivial but it should be.
                    bunch->make_nontrivial();
                }
                if (n_square < action_label[label].count)
                {
                    mCRL2log(log::warning) << "There are "
                        << action_label[label].count << ' '
                        << pp(aut.action_label(label)) << "-transitions.  "
                        "This is more than n^2 (= " << n_square << ").  I "
                        "cannot guarantee that branching bisimulation runs in "
                        "time O(m log n).\n";                                   ONLY_IF_DEBUG(  if (max_transitions < action_label[label].count)
                                                                                                    {   max_transitions = action_label[label].count;   }  )
                }
                // initialise begin_or_before_end pointers for this
                // action_block-slice
                action_label[label].convert_to_iterator(
                          next_action_label_begin + action_label[label].count);
                next_action_label_begin->begin_or_before_end.set(
                                                action_label[label].begin - 1);
                action_block_iter_t action_block_slice_begin =
                                                       next_action_label_begin;
                while (++next_action_label_begin < action_label[label].begin)
                {
                    next_action_label_begin->begin_or_before_end.set(
                                                     action_block_slice_begin); // mCRL2complexity(next_action_label_begin->succ()->block_bunch->pred, ...) --
                }                                                               // subsumed in the call at the end
            }
            else
            {                                                                   // The assertion ensures that there are not many unused labels:
                action_label[label].begin = next_action_label_begin;            assert(0 == label);
            }
        }
        while (0 < label &&
                   (/* insert a dummy entry                                  */ assert(next_action_label_begin < part_tr.action_block_inert_begin),
                                                                                assert(next_action_label_begin->succ.is_null()),
                                                                                assert(next_action_label_begin->begin_or_before_end.is_null()),
                    ++next_action_label_begin,                          true)); assert(next_action_label_begin == part_tr.action_block_inert_begin);

        /* distribute the transitions over the data structures               */ ONLY_IF_DEBUG( bisim_gjkw::check_complexity::init(2 * max_transitions); )

        part_tr.pred.begin()->source = part_st.state_info.end();
        part_tr.pred.begin()->target = part_st.state_info.end();
        // part_tr.pred.end()[-1].source = part_st.state_info.end();
        part_tr.pred.end()[-1].target = part_st.state_info.end();
        block_bunch_iter_t next_block_bunch = part_tr.block_bunch.begin() + 1;  assert(next_block_bunch == block_bunch_slice_t::block_bunch_begin);
        for (const transition& t: aut.get_transitions())
        {                                                                       assert(part_st.state_info[t.from()].block == B);
            succ_iter_t succ_pos;
            block_bunch_iter_t block_bunch_pos;
            pred_iter_t pred_pos;
            action_block_iter_t action_block_pos;

            if (branching&&aut.is_tau(aut.apply_hidden_label_map(t.label()))&&  ((
                                                            t.from() != t.to())   || (assert(preserve_divergence), false)))
            {
                // It is a (normal) inert transition: place near the end of the
                // respective pred/succ slices, just before the other inert
                // transitions.
                succ_pos = --part_st.state_info[t.from()].succ_inert.begin;     assert(succ_pos->begin_or_before_end.is_null());
                block_bunch_pos = part_tr.block_bunch.end()-inert_transitions;  assert(block_bunch_pos >= part_tr.block_bunch_inert_begin);
                pred_pos = --part_st.state_info[t.to()].pred_inert.begin;       assert(block_bunch_pos->slice.is_null());
                action_block_pos=part_tr.action_block.end()-inert_transitions;  assert(action_block_pos >= part_tr.action_block_inert_begin);
                --inert_transitions;                                            assert(action_block_pos->begin_or_before_end.is_null());
            }
            else
            {
                // It is a non-inert transition (possibly a self-loop): place
                // at the end of the respective succ slice and at the beginning
                // of the respective pred slice.
                succ_pos =
                        --part_st.state_info[t.from()].current_out_slice.begin; assert(succ_pos->begin_or_before_end() <= succ_pos ||
                                                                                           succ_pos->begin_or_before_end()->begin_or_before_end() == succ_pos);
                block_bunch_pos = next_block_bunch++;                           assert(block_bunch_pos < part_tr.block_bunch_inert_begin);
                pred_pos = part_st.state_info[t.to()].not_.used_pred++;
                action_block_pos =
                   --action_label[aut.apply_hidden_label_map(t.label())].begin; assert(action_block_pos->begin_or_before_end() <= action_block_pos ||
                                                                                                action_block_pos->begin_or_before_end()->
                                                                                                                    begin_or_before_end() == action_block_pos);
                block_bunch_pos->slice.set(B->stable_block_bunch.begin());      assert(action_block_pos < part_tr.action_block_inert_begin);
            }
            succ_pos->block_bunch = block_bunch_pos;
            block_bunch_pos->pred = pred_pos;
            pred_pos->action_block = action_block_pos;
            pred_pos->source = part_st.state_info.begin() + t.from();
            pred_pos->target = part_st.state_info.begin() + t.to();
            action_block_pos->succ.set(succ_pos);                               // mCRL2complexity(pred_pos, ...) -- subsumed in the call at the end
        }                                                                       assert(0 == inert_transitions);
        /* delete transitions already -- they are no longer needed.  We will */ assert(next_block_bunch == part_tr.block_bunch_inert_begin);
        // add new transitions at the end of minimisation.
        aut.clear_transitions();

        // convert not_ from pred_iter_t to succ_iter_t: (Normally this is
        // trivial, but debug versions of the iterators might have nontrivial
        // constructors and destructors.)
        for (state_info_iter_t s = part_st.state_info.begin();
                                             s < part_st.state_info.end(); ++s)
        {
            s->not_.convert_to_succ_iter_t();
        }

        if (nullptr != bunch)
        {                                                                       assert(!bunch->begin->succ.is_null());
            if (bunch->end[-1].succ.is_null())
            {
                --bunch->end;                                                   assert(bunch->begin < bunch->end);
            }                                                                   assert(!bunch->end[-1].succ.is_null());
        }
                                                                                mCRL2complexity(B, add_work(
        /* Line 1.5: if there are marked states then                         */                  bisim_gjkw::check_complexity::create_initial_partition, 1), );
        if (0 < B->marked_size())
        {                                                                       ONLY_IF_DEBUG( part_state_t::print_part();  part_tr.print_trans(*this); )
            // Line 1.6: refine(all states, all visible transitions,
            //                  extend from marked states)
            B = refine(B,
                    /* splitter block_bunch */ B->stable_block_bunch.begin(),
                           extend_from_marked_states_for_init_and_postprocess);
            // Line 1.7: Unmark all states
                // We can ignore possible new non-inert transitions, as every
                // red bottom state already has a transition in bunch.
            B->marked_nonbottom_begin = B->end;
            B->marked_bottom_begin = B->nonbottom_begin;
        // Line 1.8: end if
        }
    }
                                                                                #ifndef NDEBUG
                                                                                    /// \brief assert that the data structure is consistent and stable
                                                                                    /// \details The data structure is tested against a large number of
                                                                                    /// assertions to ensure that everything is consistent, e. g. pointers that
                                                                                    /// should point to successors of state s actually point to a transition
                                                                                    /// that starts in s.
                                                                                    ///
                                                                                    /// Additionally, it is asserted that the partition is stable. i. e. every
                                                                                    /// bottom state in every block can reach exactly every bunch in the list
                                                                                    /// of bunches that should be reachable from it, and every nonbottom state
                                                                                    /// can reach a subset of them.
                                                                                    void assert_stability() const
                                                                                    {
                                                                                        part_state_t::assert_consistency(branching);

                                                                                        assert(part_tr.succ.size() == part_tr.block_bunch.size() + 1);
                                                                                        assert(part_tr.pred.size() == part_tr.block_bunch.size() + 1);
                                                                                        assert(part_tr.action_block.size() ==
                                                                                                         part_tr.block_bunch.size() + action_label.size() - 2);
                                                                                        if (part_tr.block_bunch.empty())  return;

                                                                                        assert(part_tr.unstable_block_bunch.empty());
                                                                                        /* for (block_bunch_slice_const_iter_t block_bunch =
                                                                                                                         part_tr.unstable_block_bunch.cbegin();
                                                                                             part_tr.unstable_block_bunch.cend() != block_bunch; ++block_bunch)
                                                                                        {
                                                                                            assert(!block_bunch->is_stable());
                                                                                        } */

                                                                                        // for all blocks
                                                                                        for (permutation_const_iter_t block_begin=state_info_entry::perm_begin;
                                                                                                                    block_begin < state_info_entry::perm_end; )
                                                                                        {
                                                                                            const block_t* const block = (*block_begin)->block;
                                                                                            unsigned const max_block = bisim_gjkw::check_complexity::log_n -
                                                                                                            bisim_gjkw::check_complexity::ilog2(block->size());
                                                                                            // iterators have no predefined hash, so we store pointers:
                                                                                            std::unordered_set<const block_bunch_slice_t*>
                                                                                                                                         block_bunch_check_set;
                                                                                            block_bunch_check_set.reserve(block->stable_block_bunch.size());

                                                                                            // for all stable block_bunch-slices of the block
                                                                                            for (block_bunch_slice_const_iter_t block_bunch =
                                                                                                                            block->stable_block_bunch.cbegin();
                                                                                                block_bunch != block->stable_block_bunch.cend(); ++block_bunch)
                                                                                            {
                                                                                                assert(block_bunch->source_block() == block);
                                                                                                assert(block_bunch->is_stable());
                                                                                                block_bunch_check_set.insert(&*block_bunch);
                                                                                                mCRL2complexity(block_bunch, no_temporary_work(
                                                                                                             block_bunch->bunch()->max_work_counter(*this)), );
                                                                                            }

                                                                                            // for all states in the block
                                                                                            for (permutation_const_iter_t perm_begin = block->begin;
                                                                                                                         perm_begin < block->end; ++perm_begin)
                                                                                            {
                                                                                                state_info_const_iter_t const state = *perm_begin;
                                                                                                succ_const_iter_t out_slice_end = state->succ_inert.begin;
                                                                                                trans_type block_bunch_count = 0;
                                                                                                // for all out-slices of the state
                                                                                                assert(state != succ_entry::succ_begin[-1].
                                                                                                                                    block_bunch->pred->source);
                                                                                                if (state == out_slice_end[-1].block_bunch->pred->source)
                                                                                                {
                                                                                                    const bunch_t* bunch;
                                                                                                    do
                                                                                                    {
                                                                                                        block_bunch_slice_const_iter_t block_bunch_slice =
                                                                                                                        out_slice_end[-1].block_bunch->slice();
                                                                                                        bunch = block_bunch_slice->bunch();
                                                                                                        assert(block == block_bunch_slice->source_block());
                                                                                                        if (block_bunch_slice->is_stable())
                                                                                                        {
                                                                                                            assert(1 == block_bunch_check_set.count(
                                                                                                                                         &*block_bunch_slice));
                                                                                                            ++block_bunch_count;
                                                                                                        }
                                                                                                        else  assert(0); //< all block_bunch-slices should be
                                                                                                                         //  stable
                                                                                                        unsigned const max_bunch =
                                                                                                                                bunch->max_work_counter(*this);
                                                                                                        succ_const_iter_t const out_slice_begin =
                                                                                                                       out_slice_end[-1].begin_or_before_end();
                                                                                                        assert(out_slice_begin < out_slice_end);
                                                                                                        assert(out_slice_begin->begin_or_before_end() + 1 ==
                                                                                                                                                out_slice_end);

                                                                                                        // for all transitions in the out-slice
                                                                                                        do
                                                                                                        {
                                                                                                            --out_slice_end;
                                                                                                            assert(bunch->begin <=
                                                                                                               out_slice_end->block_bunch->pred->action_block);
                                                                                                            assert(out_slice_end->block_bunch->pred->
                                                                                                                                    action_block < bunch->end);
                                                                                                            assert(block_bunch_slice ==
                                                                                                                          out_slice_end->block_bunch->slice());
                                                                                                            if (out_slice_end->block_bunch + 1 !=
                                                                                                                                        block_bunch_slice->end)
                                                                                                            {
                                                                                                                assert(out_slice_end->block_bunch + 1 <
                                                                                                                                       block_bunch_slice->end);
                                                                                                                assert(block_bunch_slice ==
                                                                                                                        out_slice_end->block_bunch[1].slice());
                                                                                                            }
                                                                                                            mCRL2complexity(out_slice_end->block_bunch->pred,
                                                                                                                no_temporary_work(max_block, bisim_gjkw::
                                                                                                                    check_complexity::log_n - bisim_gjkw::
                                                                                                                    check_complexity::ilog2(out_slice_end->
                                                                                                                    block_bunch->pred->target->block->size()),
                                                                                                                    max_bunch,
                                                                                                                    perm_begin<block->nonbottom_begin), *this);
                                                                                                        }
                                                                                                        while (out_slice_begin < out_slice_end &&
                                                                                                           (assert(out_slice_begin ==
                                                                                                                      out_slice_end->begin_or_before_end()),
                                                                                                            assert(out_slice_end !=
                                                                                                                            state->current_out_slice.begin),
                                                                                                                                                        true));
                                                                                                    }
                                                                                                    while (state==out_slice_end[-1].block_bunch->pred->source&&
                                                                                                        (assert(out_slice_end[-1].bunch()->sort_key_and_label.
                                                                                                                sort_key <= bunch->end - bunch->begin +
                                                                                                                   bunch->sort_key_and_label.sort_key), true));
                                                                                                }
                                                                                                assert(out_slice_end <= state->current_out_slice.begin);
                                                                                                if (perm_begin < block->nonbottom_begin)
                                                                                                {
                                                                                                    assert(block_bunch_check_set.size() == block_bunch_count);
                                                                                                }
                                                                                            }
                                                                                            block_begin = block->end;
                                                                                        }
                                                                                        assert(action_block_entry::action_block_begin <=
                                                                                                                             part_tr.action_block_inert_begin);
                                                                                        assert(block_bunch_slice_t::block_bunch_begin <=
                                                                                                                              part_tr.block_bunch_inert_begin);
                                                                                        if (branching)
                                                                                        {
                                                                                            assert(part_tr.action_block_inert_begin <=
                                                                                                                                   part_tr.action_block.end());
                                                                                            assert(part_tr.block_bunch_inert_begin <=
                                                                                                                                    part_tr.block_bunch.end());
                                                                                            assert(part_tr.block_bunch.end()-part_tr.block_bunch_inert_begin ==
                                                                                                part_tr.action_block.end() - part_tr.action_block_inert_begin);

                                                                                            // for all inert transitions
                                                                                            for (action_block_const_iter_t action_block =
                                                                                                                              part_tr.action_block_inert_begin;
                                                                                                    action_block < part_tr.action_block.cend(); ++action_block)
                                                                                            {
                                                                                                assert(action_block->begin_or_before_end.is_null());
                                                                                                succ_const_iter_t const succ_iter = action_block->succ();
                                                                                                assert(succ_iter->block_bunch->slice.is_null());
                                                                                                pred_const_iter_t const pred_iter=succ_iter->block_bunch->pred;
                                                                                                assert(action_block == pred_iter->action_block);
                                                                                                assert(part_tr.block_bunch_inert_begin <=
                                                                                                                                       succ_iter->block_bunch);
                                                                                                assert(pred_iter->source != pred_iter->target);
                                                                                                assert(pred_iter->source->block == pred_iter->target->block);
                                                                                                assert(pred_iter->source->succ_inert.begin <= succ_iter);
                                                                                                assert(pred_iter->source->succ_inert.begin == succ_iter ||
                                                                                                   succ_iter[-1].block_bunch->pred->source==pred_iter->source);
                                                                                                assert(pred_iter->target->pred_inert.begin <= pred_iter);
                                                                                                assert(pred_iter->target->pred_inert.begin == pred_iter ||
                                                                                                                    pred_iter[-1].target == pred_iter->target);
                                                                                                unsigned const max_block = bisim_gjkw::check_complexity::log_n-
                                                                                                                    bisim_gjkw::check_complexity::ilog2(
                                                                                                                             pred_iter->target->block->size());
                                                                                                mCRL2complexity(pred_iter, no_temporary_work(max_block,
                                                                                                                                  max_block, 0, false), *this);
                                                                                            }
                                                                                        }
                                                                                        else
                                                                                        {
                                                                                            assert(!preserve_divergence);
                                                                                            assert(part_tr.action_block_inert_begin ==
                                                                                                                                   part_tr.action_block.end());
                                                                                            assert(part_tr.block_bunch_inert_begin==part_tr.block_bunch.end());
                                                                                        }
                                                                                        action_block_const_iter_t action_slice_end =
                                                                                                                              part_tr.action_block_inert_begin;
                                                                                        // for all action labels and bunches
                                                                                        label_type label = 0;
                                                                                        const bunch_t* previous_bunch = nullptr;
                                                                                        do
                                                                                        {
                                                                                            assert(action_block_entry::action_block_begin <=
                                                                                                                                    action_label[label].begin);
                                                                                            assert(action_label[label].begin <= action_slice_end);
                                                                                            assert(action_slice_end <= part_tr.action_block_inert_begin);
                                                                                            // for all action_block slices
                                                                                            for (action_block_const_iter_t action_block_slice_end =
                                                                                                                                             action_slice_end;
                                                                                                          action_label[label].begin < action_block_slice_end; )
                                                                                            {
                                                                                                action_block_const_iter_t const action_block_slice_begin =
                                                                                                              action_block_slice_end[-1].begin_or_before_end();
                                                                                                assert(action_block_slice_begin < action_block_slice_end);
                                                                                                assert(action_block_slice_end <= action_slice_end);
                                                                                                const block_t* const target_block = action_block_slice_begin->
                                                                                                                      succ()->block_bunch->pred->target->block;
                                                                                                const bunch_t* const bunch = action_block_slice_begin->
                                                                                                                         succ()->block_bunch->slice()->bunch();
                                                                                                if (previous_bunch != bunch)
                                                                                                {
                                                                                                    assert(nullptr == previous_bunch);
                                                                                                    previous_bunch = bunch;
                                                                                                    assert(bunch->end == action_block_slice_end);
                                                                                                    assert(TRANS_TYPE_MAX - part_tr.action_block.size() <=
                                                                                                                           bunch->sort_key_and_label.sort_key);
                                                                                                    if (bunch->begin == action_block_slice_begin)
                                                                                                    {
                                                                                                        // Perhaps this does not always hold; sometimes, an
                                                                                                        // action_block slice disappears but the bunch cannot
                                                                                                        // be made trivial.
                                                                                                        assert(bunch->is_trivial());
                                                                                                    }
                                                                                                    else  assert(!bunch->is_trivial());
                                                                                                    mCRL2complexity(bunch,
                                                                                                          no_temporary_work(bunch->max_work_counter(*this)), );
                                                                                                }
                                                                                                if(bunch->begin == action_block_slice_begin)
                                                                                                {
                                                                                                    previous_bunch = nullptr;
                                                                                                }
                                                                                                else  assert(bunch->begin < action_block_slice_begin);

                                                                                                assert(action_block_slice_begin->begin_or_before_end() + 1 ==
                                                                                                                                       action_block_slice_end);
                                                                                                // for all transitions in the action_block slice
                                                                                                action_block_const_iter_t action_block=action_block_slice_end;
                                                                                                do
                                                                                                {
                                                                                                    --action_block;
                                                                                                    succ_const_iter_t const succ_iter = action_block->succ();
                                                                                                    pred_const_iter_t const pred_iter =
                                                                                                                                  succ_iter->block_bunch->pred;
                                                                                                    assert(action_block == pred_iter->action_block);
                                                                                                    assert(succ_iter->block_bunch <
                                                                                                                              part_tr.block_bunch_inert_begin);
                                                                                                    assert(!branching || !aut.is_tau(label) ||
                                                                                                        pred_iter->source->block != pred_iter->target->block ||
                                                                                                                     (preserve_divergence &&
                                                                                                                      pred_iter->source == pred_iter->target));
                                                                                                    assert(succ_iter < pred_iter->source->succ_inert.begin);
                                                                                                    assert(succ_iter+1==pred_iter->source->succ_inert.begin ||
                                                                                                                succ_iter[1].block_bunch->pred->source ==
                                                                                                                                            pred_iter->source);
                                                                                                    assert(pred_iter < pred_iter->target->pred_inert.begin);
                                                                                                    assert(pred_iter+1==pred_iter->target->pred_inert.begin ||
                                                                                                                     pred_iter[1].target == pred_iter->target);
                                                                                                    assert(target_block == pred_iter->target->block);
                                                                                                    assert(bunch == succ_iter->block_bunch->slice()->bunch());
                                                                                                }
                                                                                                while (action_block_slice_begin < action_block &&
                                                                                                   (// some properties only need to be checked for states that
                                                                                                    // are not the first one:
                                                                                                    assert(action_block->begin_or_before_end() ==
                                                                                                                             action_block_slice_begin), true));
                                                                                                action_block_slice_end = action_block_slice_begin;
                                                                                            }
                                                                                            if (action_slice_end < part_tr.action_block_inert_begin)
                                                                                            {
                                                                                                // there is a dummy transition between action labels
                                                                                                assert(action_slice_end->succ.is_null());
                                                                                                assert(action_slice_end->begin_or_before_end.is_null());
                                                                                            }
                                                                                        }
                                                                                        while (++label < action_label.size() &&
                                                                                                 (action_slice_end = action_label[label - 1].begin - 1, true));
                                                                                        assert(nullptr == previous_bunch);
                                                                                    }
                                                                                #endif
    /// \brief Run (branching) bisimulation minimisation in time O(m log n)
    /// \details This function assumes that the partitioner object stores a LTS
    /// with a partition satisfying the invariant:
    ///
    /// If a state contains a transition in a bunch, then every bottom state in
    /// the same block contains a transition in that bunch.
    ///
    /// The function runs the efficient O(m log n) algorithm for branching
    /// bisimulation minimisation on the LTS that has been stored in the
    /// partitioner:  As long as there are nontrivial bunches, it selects one,
    /// subdivides it into two bunches and then stabilises the partition for
    /// these bunches.  As a result, the partition stored in the partitioner
    /// will become stable.
    ///
    /// Parameters and return value are implicit with this function:  the LTS,
    /// the partition and the flags of the bisimulation algorithm are all
    /// stored in the partitioner object.
    void refine_partition_until_it_becomes_stable()
    {
        // Line 1.9: while there is a non-trivial bunch do
        for (;;)
        {                                                                       // mCRL2complexity(...) -- this loop will be ascribed to (the transitions in)
                                                                                // the new bunch below.
            /*----------------------- find a splitter -----------------------*/ ONLY_IF_DEBUG( part_state_t::print_part();  part_tr.print_trans(*this);
                                                                                                                                          assert_stability(); )
            // Line 1.10: Select a non-trivial splitter bunch splitter_bunch
            bunch_t* const splitter_bunch = bunch_t::get_some_nontrivial();
            if (nullptr == splitter_bunch)  break;
            // Line 1.11: Select a small action_block-slice in splitter_bunch
            // and
            /* Line 1.12: Create a new bunch new_bunch that consists of the  */ ONLY_IF_DEBUG( mCRL2log(log::debug, "bisim_dnj") << "Refining "
            /*            selected action_block-slice                        */                                        << splitter_bunch->debug_id() << '\n'; )
            bunch_t* const new_bunch =
                          splitter_bunch->split_off_small_action_block_slice();
                                                                                #ifndef NDEBUG
            /*--------------- find predecessors of new_bunch ----------------*/     mCRL2log(log::debug, "bisim_dnj") << "Splitting off "
                                                                                                                              << new_bunch->debug_id() << '\n';
            /* Line 1.13: for all transitions s --> t in new_bunch do        */     unsigned const max_splitter_counter = new_bunch->max_work_counter(*this);
                                                                                #endif
            action_block_iter_t splitter_iter = new_bunch->begin;               assert(splitter_iter < new_bunch->end);
            do
            {                                                                   assert(splitter_iter->succ()->block_bunch->pred->action_block==splitter_iter);
                state_info_iter_t const source =
                              splitter_iter->succ()->block_bunch->pred->source;
                // Line 1.14: Mark s
                bool const first_transition_of_state =
                                                   source->block->mark(source);
                // Line 1.15: Set the current out-slice pointer of s to a
                //            transition in splitter_bunch (preferably not
                //            s --> t itself)
                // Line 1.17: Register that s --> t is now part of new_bunch
                part_tr.first_move_transition_to_new_bunch(splitter_iter,
                                                    first_transition_of_state); // mCRL2complexity(splitter_iter->succ()->block_bunch->pred, ...) -- subsumed
            // Line 1.19: end for                                               // in the call below
            }
            while (++splitter_iter < new_bunch->end);

            // We cannot join the loop above with the loop below!

            // Line 1.13: for all transitions s --> t in new_bunch do
            splitter_iter = new_bunch->begin;                                   assert(splitter_iter < new_bunch->end);
            do
            {
                // Line 1.16: Add the block_bunch-slice of splitter_bunch where
                //            s --> t was in to the list of unstable
                //            block_bunch-slices
                // Line 1.17: Register that s --> t is now part of new_bunch
                // Line 1.18: Add the block_bunch-slice of new_bunch where
                //            s --> t is now in to the list of unstable
                //            block_bunch-slices
                part_tr.second_move_transition_to_new_bunch(                    ONLY_IF_DEBUG( *this, )
                                                     splitter_iter, new_bunch); // mCRL2complexity(splitter_iter->succ()->block_bunch->pred, ...) -- subsumed
            // Line 1.19: end for                                               // in the call below
            }
            while (++splitter_iter < new_bunch->end);                           mCRL2complexity(new_bunch, add_work(bisim_gjkw::check_complexity::
                                                                                                           refine_partition_until_it_becomes_stable__find_pred,
            /*----------------- stabilise the partition again ---------------*/                                                       max_splitter_counter), );

            // Line 1.20: while the list of unstable block_bunch-slices is not
            //                  empty do
            while (!part_tr.unstable_block_bunch.empty())
            {                                                                   // The work in this iteration is ascribed to some transition in the
                // Line 1.21: splitter := the first unstable block bunch-slice  // splitter block_bunch-slice.  Therefore, we increase the counter of that
                    // The first element of the unstable block_bunch-slices     // block_bunch-slice.  This is allowed because we started with a small
                    // list should be a small splitter of a block that is       // splitter.
                    // marked accordingly.                                      //
                block_bunch_slice_iter_t splitter =                             // We have to call mCRL2complexity here because splitter may be split up later.
                                          part_tr.unstable_block_bunch.begin(); mCRL2complexity(splitter, add_work(bisim_gjkw::check_complexity::
                                                                                  refine_partition_until_it_becomes_stable__stabilize, max_splitter_counter),);
                block_t* refine_block = splitter->source_block();               assert(!splitter->is_stable());  assert(splitter->bunch() == new_bunch);
                /* Line 1.22: Remove splitter from the list of unstable      */ assert(!splitter->empty());
                /*            block_bunch-slices                             */ assert(1 < refine_block->size());  assert(0 < refine_block->marked_size());
                refine_block->stable_block_bunch.splice(
                    refine_block->stable_block_bunch.end(),
                                       part_tr.unstable_block_bunch, splitter);
                splitter->make_stable();                                        // test whether the next splitter actually belongs to this block:
                /* Line 1.23: (red_block, blue_block) := refine(source block */ assert(*part_tr.unstable_block_bunch.front().bunch() < *splitter->bunch());
                /*         of splitter, splitter, extend from marked states) */ assert(part_tr.unstable_block_bunch.front().source_block() == refine_block);
                permutation_iter_t refine_block_begin = refine_block->begin;    assert((*refine_block_begin)->pos == refine_block_begin);
                block_t* red_block = refine(refine_block, splitter,
                                                    extend_from_marked_states);
                // Line 1.24: Mark unstable block bunch-slices of blue_block as
                //            stable
                if (refine_block_begin < red_block->begin)
                {
                    block_t* const blue_block = (*refine_block_begin)->block;   assert(blue_block->end == red_block->begin);
                    block_bunch_slice_iter_t blue_splitter =
                                          part_tr.unstable_block_bunch.begin(); assert(0 == blue_block->marked_size());
                    if (part_tr.unstable_block_bunch.end() != blue_splitter &&
                      (blue_splitter->source_block() == blue_block ||
                       (++blue_splitter!=part_tr.unstable_block_bunch.end() &&
                                 blue_splitter->source_block() == blue_block)))
                    {                                                           assert(!blue_splitter->is_stable());
                        /* The next ``unstable'' block_bunch is the blue     */ assert(blue_splitter->bunch() == splitter_bunch);
                        // subblock's slice.  Actually that is already stable.
                        blue_block->stable_block_bunch.splice(
                            blue_block->stable_block_bunch.end(),
                                  part_tr.unstable_block_bunch, blue_splitter);
                        blue_splitter->make_stable();
                    }
                }
                // Line 1.25: if red_block has marked states (i. e. it has new
                //               non-inert transitions) then
                if (0 < red_block->marked_size())
                {
                    // Line 1.26: red_block := postprocess(red_block)
                    red_block = postprocess_new_noninert(red_block, splitter);
                    // Line 1.27: if red block == {} then
                    //                Continue this loop, i. e. go to Line 1.19
                    if (nullptr == red_block)  continue;
                // Line 1.28: end if
                }
                // Line 1.29: splitter := the first (remaining) unstable
                //                        block_bunch-slice
                    // Now the first element of the unstable block_bunch-slices
                    // list should be a large splitter of red_block, and it
                    // should be handled using the current_out_slice pointers.
                splitter = part_tr.unstable_block_bunch.begin();
                if (part_tr.unstable_block_bunch.end() == splitter)  break;     assert(!splitter->is_stable());
                // Line 1.30: if red_block == source block of splitter then
                if (red_block == splitter->source_block())
                {                                                               assert(splitter->bunch() == splitter_bunch);
                    // Line 1.31: Remove splitter from the list of unstable
                    //            block_bunch-slices
                    red_block->stable_block_bunch.splice(
                        red_block->stable_block_bunch.end(),
                                       part_tr.unstable_block_bunch, splitter);
                    splitter->make_stable();

                    // Line 1.32: (red_block, ) := refine(red_block, splitter,
                    //                                    extend from splitter)
                    if (1 >= red_block->size())  continue;
                    red_block = refine(red_block,splitter,extend_from_splitter  ONLY_IF_DEBUG( , new_bunch )
                                                                             );
                    // Line 1.33: if red_block has marked states (i. e. it has
                    //               new non-inert transitions) then
                    if (0 < red_block->marked_size())
                    {
                        // Line 1.34: postprocess(red_block)
                        postprocess_new_noninert(red_block, splitter);
                    // Line 1.35: end if
                    }
                // Line 1.36: end if
                }
            // Line 1.37: end while
            }
        // Line 1.38: end while
        }

        // store the labels with the action_block-slices
        // As every action_block-slice is a (trivial) bunch at the same time,
        // we can reuse the field sort_key_and_label.label (instead of
        // sort_key_and_label.sort_key) to store the label.
        action_block_const_iter_t action_block_iter_end =
                                              part_tr.action_block_inert_begin;
        label_type label = 0;                                                   assert(label < action_label.size());
        do
        {
            for (action_block_iter_t action_block_iter =
                                                     action_label[label].begin;
              action_block_iter < action_block_iter_end;
              action_block_iter = action_block_iter->begin_or_before_end() + 1)
            {                                                                   assert(action_block_iter->succ()->block_bunch->pred->action_block ==
                                                                                                                                            action_block_iter);
                action_block_iter->succ()->bunch()->sort_key_and_label.label =
                                                                         label; assert(action_block_iter <= action_block_iter->begin_or_before_end());
            }
        }
        while (++label < action_label.size() &&
            (action_block_iter_end = action_label[label - 1].begin - 1, true));
    }

    /*----------------- Refine -- Algorithm 3 of [GJKW 2017] ----------------*/

    /// \brief Split a block according to a splitter
    /// \details The function splits `refine_block` into the red part (states
    /// with a transition in `splitter`) and the blue part (states without a
    /// transition in `splitter`).  Depending on `mode`, the states are primed
    /// as follows:
    ///
    /// - If `mode == extend_from_marked_states`, then all states with a
    ///   transition must have been marked already.
    /// - If `mode == extend_from_marked_states_for_init_and_postprocess`,
    ///   states are marked as above.  The only difference is the handling of
    ///   new non-inert transitions.
    /// - If `mode == extend_from_splitter`, then no states must be marked;
    ///   the initial states with a transition in `splitter` are searched by
    ///   `refine()` itself.
    /// - If `mode == extend_from_bottom_state_markings_and_splitter`, then
    ///   bottom states with a transition must have been marked already, but
    ///   there may be non-bottom states that also have a transition, which are
    ///   searched by `refine()`.
    ///
    /// The  function  will  also  adapt  all  data  structures  and  determine
    /// which  transitions  have  changed  from  inert  to  non-inert.   States
    /// with  a  new  non-inert  transition  will  be  marked  upon  returning.
    /// Normally,  the  new  non-inert  transitions  are  moved  to  a  new
    /// bunch,  which  will  be  specially  created.   However,  if  `mode ==
    /// extend_from_marked_states_for_init_and_postprocess`, then the new
    /// non-inert transitions will be added to `splitter` (which must hold
    /// transitions that have just become non-inert before this call to
    /// `refine()`).  If the resulting block contains marked states, the caller
    /// has to call `postprocess_new_noninert()` to stabilise the block because
    /// the new bunch may make the block unstable.
    /// \param refine_block  block that needs to be refined
    /// \param splitter      transition set that makes the block unstable
    /// \param mode          indicates how to find states with a transition in
    ///                      `splitter`, as described above
    /// \returns (a pointer to) the red subblock.  It is an error to call the
    /// function with settings that lead to an empty red subblock.  (An empty
    /// blue subblock is ok.)
    block_t* refine(block_t* const refine_block,
        block_bunch_slice_iter_t const splitter, enum refine_mode_t const mode  ONLY_IF_DEBUG( , const bunch_t* const new_bunch = nullptr )
                                                                              )
    {                                                                           assert(refine_block == splitter->source_block());
                                                                                #ifndef NDEBUG
                                                                                    mCRL2log(log::debug, "bisim_dnj") << "refine(" << refine_block->debug_id()
                                                                                        << ',' << splitter->debug_id()
                                                                                        << (extend_from_marked_states_for_init_and_postprocess == mode
                                                                                           ? ",extend_from_marked_states_for_init_and_postprocess,"
                                                                                           : (extend_from_marked_states == mode
                                                                                             ? ",extend_from_marked_states,"
                                                                                             : (extend_from_bottom_state_markings_and_splitter == mode
                                                                                               ? ",extend_from_bottom_state_markings_and_splitter,"
                                                                                               : (extend_from_splitter == mode
                                                                                                 ? ",extend_from_splitter,"
                                                                                                 : ",UNKNOWN MODE,"))))
                                                                                        << (nullptr == new_bunch ? std::string("nullptr")
                                                                                                                             : new_bunch->debug_id()) << ")\n";
                                                                                #endif

                                                                                assert(1 < refine_block->size());
                                                                                assert(0 < refine_block->marked_size() ||
                                                                                            extend_from_splitter == mode ||
                                                                                            extend_from_bottom_state_markings_and_splitter == mode);
        block_t* red_block;                                                     assert(0 == refine_block->marked_size() || extend_from_splitter != mode);
                                                                                assert((nullptr == new_bunch) == (extend_from_splitter != mode));
        COROUTINES_SECTION
            // shared variables of both coroutines
            permutation_iter_t notblue_initialised_end =
                                                 refine_block->nonbottom_begin;

            // Line 2.2: need_slow_test := (mode not in {extend from marked
            //                            states, extend from marked states for
            //                              initialisation and postprocessing})
            bool need_slow_test = extend_from_marked_states != mode &&
                    extend_from_marked_states_for_init_and_postprocess != mode;

            // variable declarations of the blue coroutine
            permutation_iter_t blue_visited_end;
            permutation_iter_t blue_blue_nonbottom_end;
            pred_iter_t blue_pred_iter;
            state_info_iter_t blue_t;
            succ_const_iter_t blue_end;

            // variable declarations of the red coroutine
            union R
            {
                block_bunch_iter_t fromred;
                permutation_iter_t block;
                R(){}  ~R(){}  // constructor and destructor are required
            } red_visited_begin;
            pred_iter_t red_pred_iter;
            pred_const_iter_t red_pred_inert_end;

            COROUTINE_LABELS(   (REFINE_RED_PREDECESSOR_HANDLED)
                                (REFINE_BLUE_PREDECESSOR_HANDLED)
                                (REFINE_RED_STATE_HANDLED)
                                (REFINE_BLUE_STATE_HANDLED)
                                (REFINE_BLUE_TESTING)
                                (REFINE_RED_COLLECT_FROMRED)
                                (REFINE_BLUE_COLLECT_BOTTOM))

            /*---------------------- find blue states -----------------------*/

            COROUTINE(1)
                // Line 2.4l: if mode = extend_from_splitter then
                if (extend_from_splitter == mode)
                {
                    // we have to decide which unmarked bottom states are blue.
                    // So we walk over all of them and check whether they have
                    // a transition to the bunch of FromRed or not.
                    blue_visited_end = refine_block->begin;

                    /*  -  -  -  -  collect blue bottom states  -  -  -  -  */

                    // Line 2.5l: for all non-red bottom states s do
                        // We use the variable blue_visited_end in this loop
                        // to indicate the boundary between blue states (those
                        // in the interval [refine_block->begin,
                        // blue_visited_end) ) and Test states (those in
                        // [blue_visited_end, refine_block->nonbottom_begin) ).
                    COROUTINE_WHILE (REFINE_BLUE_COLLECT_BOTTOM,
                        blue_visited_end < refine_block->marked_bottom_begin &&
                                                                need_slow_test)
                    {
                        state_info_iter_t s = *blue_visited_end;                assert(s->pos == blue_visited_end);
                        // Line 2.6l: if s has a transition in splitter then
                        if (s->surely_has_transition_in(splitter->bunch()))
                        {
                            // Line 2.7l: Make s a red state
                            iter_swap(blue_visited_end,
                                          --refine_block->marked_bottom_begin);
                            if (refine_block->marked_size() >
                                                      refine_block->size() / 2)
                            {                                                   // refine() has been called from Line 1.30, and this is a bottom state with
                                ABORT_OTHER_COROUTINE();                        // transitions in both the small and the large splitter.  We should ascribe the
                            }                                                   // work to the transition in the small splitter.
                                                                                #ifndef NDEBUG
                                                                                    succ_iter_t in_small_splitter =
                                                                                                         s->current_out_slice.begin->begin_or_before_end() + 1;
                                                                                    assert(in_small_splitter->block_bunch->pred->action_block->succ() ==
                                                                                                                                            in_small_splitter);
                                                                                    assert(s->current_out_slice.begin < in_small_splitter);
                                                                                    assert(in_small_splitter < s->succ_inert.begin);
                                                                                    assert(in_small_splitter->bunch() == new_bunch);
                                                                                    succ_entry::add_work_to_out_slice(*this, in_small_splitter,
                        /* Line 2.8l: else                                   */          bisim_gjkw::check_complexity::refine_blue__found_red_bottom_state, 1);
                                                                                #endif
                        }
                        else
                        {                                                       assert(s->surely_has_no_transition_in(splitter->bunch()));
                            // Line 2.9l: Make s a blue state
                            ++blue_visited_end;
                            if ((state_type) (blue_visited_end -
                               refine_block->begin) > refine_block->size() / 2)
                            {
                                ABORT_THIS_COROUTINE();                         // ascribe the work to the blue bottom state itself:
                            }                                                   mCRL2complexity(s, add_work(
                        /* Line 2.10l: end if                                */      bisim_gjkw::check_complexity::refine_blue__found_blue_bottom_state, 1), );
                        }
                    // Line 2.11l: end for
                    }
                    END_COROUTINE_WHILE;
                // Line 2.12l: end if
                }

                    // done implicitly: we now regard all unmarked bottom
                    // states as blue, i. e. the whole interval
                    // [refine_block->bottom_begin,
                    // refine_block->marked_bottom_begin).
                if(refine_block->unmarked_bottom_size()>refine_block->size()/2)
                {
                    ABORT_THIS_COROUTINE();
                }
                if (0 == refine_block->unmarked_bottom_size())
                {
                    // all bottom states are red, so there cannot be any blue
                    // states.  Unmark all states, as there are no transitions
                    // that have become non-inert.
                    refine_block->marked_nonbottom_begin = refine_block->end;
                    refine_block->marked_bottom_begin =
                                                 refine_block->nonbottom_begin;
                    red_block = refine_block;                                   ONLY_IF_DEBUG( finalise_blue_is_smaller(nullptr,red_block, new_bunch, *this); )
                    TERMINATE_COROUTINE_SUCCESSFULLY();
                }

                 /*  -  -  -  -  -  -  visit blue states  -  -  -  -  -  -  */

                // Line 2.13l: for all blue states s do
                blue_visited_end = refine_block->begin;
                blue_blue_nonbottom_end = refine_block->nonbottom_begin;
                COROUTINE_DO_WHILE (REFINE_BLUE_STATE_HANDLED,
                                    blue_visited_end < blue_blue_nonbottom_end)
                {
                    // Line 2.14l: for all inert transitions t --tau--> s do
                    blue_pred_iter = (*blue_visited_end)->pred_inert.begin;     assert(pred_entry::pred_begin[-1].target != *blue_visited_end);
                    COROUTINE_FOR (REFINE_BLUE_PREDECESSOR_HANDLED, (void) 0,
                                blue_pred_iter->target == *blue_visited_end,
                                                              ++blue_pred_iter)
                    {
                        blue_t = blue_pred_iter->source;                        assert(refine_block->nonbottom_begin <= blue_t->pos);
                        /* Line 2.15l: if t is red then  Skip state t        */ assert(blue_t->pos < refine_block->end);
                        if (refine_block->marked_nonbottom_begin<=blue_t->pos)
                        {
                            goto continuation;
                        }
                        if (notblue_initialised_end <= blue_t->pos)
                        {
                            blue_t->not_.blue = blue_t->succ_inert.begin;
                            iter_swap(blue_t->pos, notblue_initialised_end++);
                        }                                                       assert(blue_t != succ_entry::succ_end->block_bunch->pred->source);
                        // Line 2.16l: notblue [t] := notblue [t] − 1
                        ++blue_t->not_.blue;
                        // Line 2.17l: if notblue[t] > 0 then  Skip state t
                        if (blue_t ==
                                  blue_t->not_.blue->block_bunch->pred->source)
                        {
                            goto continuation;
                        }
                        // Line 2.18l: if need_slow_test then
                        if (need_slow_test)
                        {
                            if(blue_t->surely_has_transition_in(
                                                            splitter->bunch()))
                            {
                                goto continuation;
                            }
                            if (!blue_t->surely_has_no_transition_in(
                                                            splitter->bunch()))
                            {                                                   assert(blue_t != succ_entry::succ_begin[-1].block_bunch->pred->source);
                                // Line 2.19l: for all non-inert t --alpha--> u
                                //             do
                                blue_end = blue_t->succ_inert.begin;            assert(succ_entry::succ_begin < blue_end);
                                                                                assert(blue_t == blue_end[-1].block_bunch->pred->source);
                                COROUTINE_DO_WHILE(REFINE_BLUE_TESTING,blue_t==
                                        blue_end[-1].block_bunch->pred->source)
                                {
                                    blue_end =
                                            blue_end[-1].begin_or_before_end(); assert(blue_end->block_bunch->pred->source == blue_t);
                                    // Line 2.20l: if t --alpha--> u in
                                    //             splitter  then  Skip t
                                    block_bunch_slice_const_iter_t const
                                            block_bunch =
                                                blue_end->block_bunch->slice();
                                    if (block_bunch == splitter)
                                    {
                                        goto continuation;
                                                    //< break and then continue
                                    }
                                    if (*block_bunch->bunch() <
                                                            *splitter->bunch())
                                    {
                                        break;
                                    }                                           ONLY_IF_DEBUG( succ_entry::add_work_to_out_slice(*this, blue_end,
                                /* Line 2.21l: end for                       */                     bisim_gjkw::check_complexity::refine_blue__slow_test, 1); )
                                }
                                END_COROUTINE_DO_WHILE;
                        // Line 2.22l: end if
                            }
                        }                                                       assert(blue_blue_nonbottom_end <= blue_t->pos);
                        /* Line 2.23l: Make t a blue state                   */ assert(blue_t->pos < notblue_initialised_end);
                        iter_swap(blue_t->pos, blue_blue_nonbottom_end++);
                        if (blue_blue_nonbottom_end -
                                    refine_block->nonbottom_begin +
                                    refine_block->unmarked_bottom_size() >
                                                      refine_block->size() / 2)
                        {
                            ABORT_THIS_COROUTINE();
                        }
                    // Line 2.24: end for
                    continuation:                                               mCRL2complexity(blue_pred_iter, add_work(bisim_gjkw::
                                                                                    check_complexity::refine_blue__handle_transition_to_blue_state, 1), *this);
                    }
                    END_COROUTINE_FOR;                                          mCRL2complexity(*blue_visited_end, add_work(bisim_gjkw::
                /* Line 2.25l: end for                                       */          check_complexity::refine_blue__find_predecessors_of_blue_state, 1), );
                    ++blue_visited_end;
                    if (refine_block->marked_bottom_begin == blue_visited_end)
                    {
                        blue_visited_end = refine_block->nonbottom_begin;
                    }
                }
                END_COROUTINE_DO_WHILE;

                /* -  -  -  -  -  -  split off blue block  -  -  -  -  -  - */

                // Line 2.26l: Abort the red coroutine
                ABORT_OTHER_COROUTINE();
                // Line 2.27l: Create a new block of the blue states
                    // All non-blue states are red.
                refine_block->marked_nonbottom_begin = blue_blue_nonbottom_end;
                red_block = refine_block;
                block_t* const blue_block =
                              refine_block->split_off_block(new_block_is_blue);
                // Lines 2.29-2.45
                part_tr.adapt_transitions_for_new_block(blue_block,
                                                                refine_block,   ONLY_IF_DEBUG( *this, )
                   extend_from_marked_states_for_init_and_postprocess == mode,
                                                  splitter, new_block_is_blue); ONLY_IF_DEBUG(finalise_blue_is_smaller(blue_block,red_block,new_bunch,*this);)
            END_COROUTINE

            /*----------------------- find red states -----------------------*/

            COROUTINE(2)
                if (refine_block->marked_size() > refine_block->size() / 2)
                {
                    ABORT_THIS_COROUTINE();
                }

                /* -  -  -  -  -  collect states from FromRed  -  -  -  -  - */

                // Line 2.4r: if need_slow_test then
                if (need_slow_test)
                {
                    // Line 2.5r: for all transitions s --> t in splitter do
                    red_visited_begin.fromred = splitter->end;                  assert(block_bunch_slice_t::block_bunch_begin[-1].slice != splitter);
                    COROUTINE_DO_WHILE(REFINE_RED_COLLECT_FROMRED,
                               red_visited_begin.fromred[-1].slice == splitter)
                    {                                                           assert(block_bunch_slice_t::block_bunch_begin < red_visited_begin.fromred);
                        --red_visited_begin.fromred;
                        state_info_iter_t s =
                                       red_visited_begin.fromred->pred->source; assert(s->block == refine_block);  assert(*s->pos == s);
                        // Line 2.9r: Make s a red state
                        if (refine_block->nonbottom_begin <= s->pos &&
                                              s->pos < notblue_initialised_end)
                        {
                            // The non-bottom state has a transition to a blue
                            // state, so notblue is initialised; however, now
                            // it is discovered to be red anyway.
                            iter_swap(s->pos, --notblue_initialised_end);
                        }
                        if (refine_block->mark(s) &&
                          refine_block->marked_size() > refine_block->size()/2)
                        {
                            ABORT_THIS_COROUTINE();
                        }                                                       mCRL2complexity(red_visited_begin.fromred->pred, add_work(bisim_gjkw::
                    /* Line 2.10r: end for                                   */     check_complexity::refine_red__handle_transition_from_red_state, 1), *this);
                    }
                    END_COROUTINE_DO_WHILE;

                    // Line 2.11r: need_slow_test := false
                        // The shared variable need_slow_test is set to false
                        // as soon as the transitions splitter have been
                        // completed.
                    need_slow_test = false;
                // Line 2.12r: end if
                }

                /*-  -  -  -  -  -  -  visit red states  -  -  -  -  -  -  -*/  assert(0 != refine_block->marked_size());

                // Line 2.13r: for all red states s do
                red_visited_begin.block = refine_block->nonbottom_begin;
                if (refine_block->marked_bottom_begin==red_visited_begin.block)
                {
                    // It may happen that all found states are non-bottom
                    // states.  (In that case, some of these states will become
                    // new bottom states.)
                    red_visited_begin.block = refine_block->end;
                }
                COROUTINE_DO_WHILE(REFINE_RED_STATE_HANDLED, refine_block->
                             marked_nonbottom_begin != red_visited_begin.block)
                {
                    --red_visited_begin.block;                                  assert(pred_entry::pred_end->target != *red_visited_begin.block);
                    // Line 2.14r: for all inert transitions t --tau--> s do
                    COROUTINE_FOR(REFINE_RED_PREDECESSOR_HANDLED,red_pred_iter=
                        (*red_visited_begin.block)->pred_inert.begin,
                        red_pred_iter->target == *red_visited_begin.block,
                                                               ++red_pred_iter)
                    {
                        state_info_iter_t const t = red_pred_iter->source;      assert(refine_block->nonbottom_begin <= t->pos);
                        /* Line 2.23r: Make t a red state                    */ assert(*t->pos == t);  assert(t->pos < refine_block->end);
                        if (t->pos < notblue_initialised_end)
                        {
                            // The state has a transition to a blue state, so
                            // notblue is initialised; however, now it is
                            // discovered to be red anyway.
                            iter_swap(t->pos, --notblue_initialised_end);
                        }
                        if (refine_block->mark_nonbottom(t) &&
                          refine_block->marked_size() > refine_block->size()/2)
                        {
                            ABORT_THIS_COROUTINE();
                        }                                                       mCRL2complexity(red_pred_iter, add_work(bisim_gjkw::
                    /* Line 2.24r: end for                                   */       check_complexity::refine_red__handle_transition_to_red_state, 1), *this);
                    }
                    END_COROUTINE_FOR;                                          mCRL2complexity(*red_visited_begin.block, add_work(bisim_gjkw::
                /* Line 2.25r: end for                                       */            check_complexity::refine_red__find_predecessors_of_red_state, 1), );
                    if (refine_block->marked_bottom_begin ==
                                                     red_visited_begin.block &&
                       red_visited_begin.block < refine_block->nonbottom_begin)
                    {
                        red_visited_begin.block = refine_block->end;
                    }
                }
                END_COROUTINE_DO_WHILE;

                /*  -  -  -  -  -  -  split off red block  -  -  -  -  -  -  */

                // Line 2.26r: Abort the blue coroutine
                ABORT_OTHER_COROUTINE();
                // Line 2.27r: Create a new block of the red states
                    // All non-red states are blue.
                red_block = refine_block->split_off_block(new_block_is_red);
                // Lines 2.29-2.45
                part_tr.adapt_transitions_for_new_block(red_block,refine_block, ONLY_IF_DEBUG( *this, )
                    extend_from_marked_states_for_init_and_postprocess == mode,
                                                   splitter, new_block_is_red); ONLY_IF_DEBUG( finalise_red_is_smaller(refine_block, red_block, *this); )
            END_COROUTINE
        END_COROUTINES_SECTION
        return red_block;
    }

    /*--------- PostprocessNewBottom -- Algorithm 4 of [GJKW 2017] ----------*/

    /// \brief Prepare a block for postprocesing
    /// \details When this function starts, it assumes that the states with a
    /// new non-inert transition in refine_block are marked.  It is an error if
    /// it does not contain any marked states.
    ///
    /// The function separates the states with new non-inert transitions from
    /// those without;  as a result, the red subblock (which contains states
    /// with new non-inert transitions) will contain at least one new bottom
    /// state (and no old bottom states).  It then sorts the new bottom states
    /// according to the first bunch in which they have transitions and marks
    /// all block_bunch-slices of refine_block as unstable.
    /// \param refine_block   block containing states with new non-inert
    ///                       transitions that need to be stabilised
    /// \param last_splitter  splitter of the last separation before, i. e. the
    ///                       splitter that made these transitions non-inert
    ///                       (refine_block should already be stable w. r. t.
    ///                       last_splitter).
    /// \param first_preparation  If true, then the function also makes sure
    ///                           that all unstable block_bunch-slice of
    ///                           refine_block are before
    ///                           unstable_block_bunch_postprocess_end.
    /// \returns the block containing the old bottom states (and every state in
    ///          refine_block that cannot reach any new non-inert transition),
    ///          i. e. the blue subblock of the separation
    block_t* prepare_for_postprocessing(
                block_t* refine_block, block_bunch_slice_iter_t last_splitter,
                                                        bool first_preparation)
    {                                                                           assert(refine_block == last_splitter->source_block());
        block_t* blue_block;                                                    assert(block_bunch_slice_t::block_bunch_begin<part_tr.block_bunch_inert_begin);
        block_bunch_slice_iter_t new_noninert_block_bunch =
                                   part_tr.block_bunch_inert_begin[-1].slice(); assert(last_splitter->is_stable());
        /* Line 4.2: if refine block has unmarked bottom states then         */ assert(refine_block == new_noninert_block_bunch->source_block());
            /* The noninert block_bunch-slice is already stable, so we do not*/ assert(new_noninert_block_bunch->is_stable());
            /* need to move it to the stable block_bunch-slices.             */ assert(0 < refine_block->marked_size());
        if (0 < refine_block->unmarked_bottom_size())
        {
            // Line 4.3: (refine block,result) := refine(refine block, new
            //                non-inert transitions, extend from marked states
            //                           for initialisation and postprocessing)
            refine_block = refine(refine_block, new_noninert_block_bunch,
                           extend_from_marked_states_for_init_and_postprocess); assert(state_info_entry::perm_begin < refine_block->begin);
            blue_block = refine_block->begin[-1]->block;
            // If more new noninert transitions are found, we do not need to
            // handle them, as every bottom state already has a transition in
            // new_noninert_block_bunch->bunch.
            if (0 < refine_block->marked_bottom_size())
            {
                // New non-inert transitions have been found.  In that case, we
                // also have to refine w. r. t. the last splitter.  We set the
                // variable `last_splitter` to new_noninert_block_bunch so it
                // won't disturb in the test below.
                last_splitter = new_noninert_block_bunch;
            }
        // Line 4.4: else
        }
        else
        {
            // Line 4.5: result := {}
            blue_block = nullptr;
        // Line 4.6: end if
        }
        refine_block->marked_bottom_begin = refine_block->nonbottom_begin;
        refine_block->marked_nonbottom_begin = refine_block->end;

        // if this is the first call to prepare_for_postprocessing in the
        // beginning of postprocess_new_noninert, then
        // unstable_block_bunch_postprocess_end may point to a slice of the red
        // subblock; as we stabilise for all subblocks of it during
        // postprocessing, we pull it before
        // unstable_block_bunch_postprocess_end.
        if (first_preparation)
        {
            block_bunch_slice_iter_t red_splitter =
                        part_tr.unstable_block_bunch_postprocess_end =
                                          part_tr.unstable_block_bunch.begin();

            if (part_tr.unstable_block_bunch.end() != red_splitter)
            {
                if (red_splitter->source_block() == refine_block)
                {
                    ++part_tr.unstable_block_bunch_postprocess_end;
                }
                else if (++red_splitter!=part_tr.unstable_block_bunch.end() &&
                                  red_splitter->source_block() == refine_block)
                {
                    part_tr.unstable_block_bunch.splice(
                        part_tr.unstable_block_bunch_postprocess_end,
                                   part_tr.unstable_block_bunch, red_splitter);
                }
            }
        }
                                                                                #ifndef NDEBUG
                                                                                    // assert that the remainder of the unstable block_bunch slice list after
                                                                                    // part_tr.unstable_block_bunch_postprocess_end does not contain any
                                                                                    // block_bunch slice whose source_block() is refine_block.
                                                                                    for (block_bunch_slice_const_iter_t block_bunch_iter =
                                                                                                                part_tr.unstable_block_bunch_postprocess_end;
                                                                                                      block_bunch_iter != part_tr.unstable_block_bunch.cend();
                                                                                                                                            ++block_bunch_iter)
                                                                                    {
                                                                                        assert(block_bunch_iter->source_block() != refine_block);
                                                                                    }
                                                                                #endif
        if (1 >= refine_block->size())  return blue_block;

        // Line 4.7: for all bottom states s in refine_block do
        permutation_iter_t s_iter = refine_block->begin;                        assert(s_iter < refine_block->nonbottom_begin);
        do
        {
            state_info_iter_t const s = *s_iter;                                assert(s->pos == s_iter);
            // Line 4.8: Set the current out-slice pointer of s
            //                                          past its last out-slice
            s->current_out_slice.begin = s->succ_inert.begin;                   // mCRL2complexity(s, ...) -- subsumed in the call below
        // Line 4.9: end for
        }
        while (++s_iter < refine_block->nonbottom_begin);

        // Line 4.10: Sort the bottom states of refine_block according to the
        //                  out-slice just before the current out-slice pointer
        std::sort(refine_block->begin, refine_block->nonbottom_begin,
                                                before_current_out_slice_less);
        s_iter = refine_block->begin;                                           assert(s_iter < refine_block->nonbottom_begin);
        do
        {
            (*s_iter)->pos = s_iter;                                            mCRL2complexity(*s_iter, add_work(bisim_gjkw::check_complexity::
                                                                                                                             prepare_for_postprocessing, 1), );
        }
        while (++s_iter < refine_block->nonbottom_begin);

        // Line 4.11: Make all block_bunch-slices of refine_block unstable
            // However, the bunch of new noninert transitions and the bunch
            // that was the last splitter do not need to be handled (as long as
            // there are no further new bottom states).
        for (block_bunch_slice_iter_t block_bunch_iter =
            refine_block->stable_block_bunch.begin();
                  refine_block->stable_block_bunch.end() != block_bunch_iter; )
        {                                                                       assert(block_bunch_iter->is_stable());
            block_bunch_slice_iter_t const next_block_bunch_iter =
                                                   std::next(block_bunch_iter);
            if (block_bunch_iter != last_splitter &&
                                  block_bunch_iter != new_noninert_block_bunch)
            {
                part_tr.unstable_block_bunch.splice(
                    part_tr.unstable_block_bunch_postprocess_end,
                           refine_block->stable_block_bunch, block_bunch_iter);
                block_bunch_iter->make_unstable();
            }
                                                                                #ifndef NDEBUG
                                                                                    // Try to assign this work to a transition from a bottom state in
                                                                                    // block_bunch_iter.
                                                                                    // If that does not succeed, temporarily assign it to the block_bunch
                                                                                    // itself.  Later, we shall find a bottom state to which this work can be
                                                                                    // assigned.
                                                                                    assert(!block_bunch_iter->work_counter.has_temporary_work());
                                                                                    if (!block_bunch_iter->add_work_to_bottom_transns(bisim_gjkw::
                                                                                          check_complexity::prepare_for_postprocessing__make_unstable_a_priori,
                                                                                                                                                     1, *this))
                                                                                    {
                                                                                        mCRL2complexity(block_bunch_iter, add_work(bisim_gjkw::
                                                                                                         check_complexity::
                                                                                                         prepare_for_postprocessing__make_unstable_temp, 1), );
                                                                                        assert(block_bunch_iter->work_counter.has_temporary_work());
                                                                                    }
                                                                                #endif
            block_bunch_iter = next_block_bunch_iter;
        }
        // Line 4.12: return result {i. e. the part of refine_block
        //                                           without new bottom states}
        return blue_block;
    }


    /// \brief Split a block with new non-inert transitions as needed
    /// \details The function splits refine_block by stabilising for all
    /// bunches in which it contains transitions.
    ///
    /// When this function starts, it assumes that the states with a new
    /// non-inert transition in refine_block are marked.  It is an error if it
    /// does not contain any marked states.
    ///
    /// The function first separates the states with new non-inert transitions
    /// from those without;  as a result, the red subblock (which contains
    /// states with new non-inert transitions) will contain at least one new
    /// bottom state.  Then it walks through all the bunches that can be
    /// reached from this subblock to separate it into smaller, stable
    /// subblocks.
    /// \param refine_block   block containing states with new non-inert
    ///                       transitions that need to be stabilised
    /// \param last_splitter  splitter of the last separation before, i. e. the
    ///                       splitter that made these transitions non-inert
    ///                       (refine_block should already be stable w. r. t.
    ///                       last_splitter).
    /// \returns the block containing the old bottom states (and every state in
    ///          refine_block that cannot reach any new non-inert transition),
    ///          i. e. the blue subblock of the first separation above
    block_t* postprocess_new_noninert(
                 block_t* refine_block, block_bunch_slice_iter_t last_splitter)
    {

        /*-------------- collect reachable block_bunch-slices ---------------*/

        // Line 3.2: result := prepare_for_postprocessing(refine_block)
        block_t* const result_block =
                 prepare_for_postprocessing(refine_block, last_splitter, true);

        /*----------- stabilise w. r. t. found block_bunch-slices -----------*/

        // Line 3.3: for all block_bunch-slices splitter
        //         that emanate from (a part of) the original block \ result do
        block_bunch_slice_iter_t splitter;
        while ((splitter = part_tr.unstable_block_bunch.begin())
                               != part_tr.unstable_block_bunch_postprocess_end)
        {                                                                       assert(!splitter->is_stable());
            // Line 3.4: refine_block := the source block of splitter
            refine_block = splitter->source_block();                            assert(0 == refine_block->marked_size());
                                                                                assert(refine_block->begin < refine_block->nonbottom_begin);
            if (1 >= refine_block->size())
            {
                // make the splitter stable and do nothing else.
                refine_block->stable_block_bunch.splice(
                            refine_block->stable_block_bunch.end(),
                                       part_tr.unstable_block_bunch, splitter);
                splitter->make_stable();
                                                                                #ifndef NDEBUG
                                                                                    // now splitter must have some transitions that start in bottom states:
                                                                                    if (splitter->work_counter.has_temporary_work())
                                                                                    {
                                                                                        assert(splitter->add_work_to_bottom_transns(bisim_gjkw::
                                                                                            check_complexity::
                                                                                            prepare_for_postprocessing__make_unstable_a_posteriori, 1, *this));
                                                                                        splitter->work_counter.reset_temporary_work();
                                                                                    }
                                                                                    assert(splitter->add_work_to_bottom_transns(bisim_gjkw::check_complexity::
                                                                                                      postprocess_new_noninert__stabilize_a_priori, 1, *this));
                                                                                #endif
                continue;
            }
            // Line 3.5: splitter' := the block_bunch-slice of the out-slice
            //                         just before the current out-slice
            //                         of the last bottom state in refine_block
            state_info_iter_t last_bottom_state =
                                             refine_block->nonbottom_begin[-1]; assert(last_bottom_state !=
            /* Line 3.6: if splitter < splitter' then                        */                          succ_entry::succ_begin[-1].block_bunch->pred->source);
            if (last_bottom_state->current_out_slice.begin[-1].
                              block_bunch->pred->source != last_bottom_state ||
                *last_bottom_state->current_out_slice.begin[-1].bunch() <
                                                          *splitter->bunch() ||
                (// Line 3.7: splitter := splitter'
                 splitter = last_bottom_state->
                            current_out_slice.begin[-1].block_bunch->slice(),   assert(part_tr.unstable_block_bunch_postprocess_end != splitter),
                 !splitter->is_stable() &&
                 // Line 3.8: Mark the bottom states
                 //                          that have a transition in splitter
                 (refine_block->marked_bottom_begin = std::lower_bound(
                   refine_block->begin, refine_block->nonbottom_begin - 1,
                          splitter->bunch(), before_current_out_slice_less),    assert((*refine_block->marked_bottom_begin)->
            /* Line 3.9: end if                                              */                      current_out_slice.begin[-1].bunch() == splitter->bunch()),
                                                                        true)))
            {                                                                   assert(refine_block->begin == refine_block->marked_bottom_begin ||
                                                                                        refine_block->marked_bottom_begin[-1]->current_out_slice.begin ==
                                                                                                                                      succ_entry::succ_begin ||
                                                                                        refine_block->marked_bottom_begin[-1]->current_out_slice.begin[-1].
                                                                                           block_bunch->pred->source < refine_block->marked_bottom_begin[-1] ||
                /* Line 3.10: Remove splitter from the list                  */         *refine_block->marked_bottom_begin[-1]->current_out_slice.begin[-1].
                /*                            of unstable block_bunch-slices */                                                  bunch() < *splitter->bunch());
                refine_block->stable_block_bunch.splice(
                            refine_block->stable_block_bunch.end(),
                                       part_tr.unstable_block_bunch, splitter);
                splitter->make_stable();
                                                                                #ifndef NDEBUG
                                                                                    bool stabilize_a_priori_succeeded = splitter->add_work_to_bottom_transns(
                /* Line 3.11: (refine_block, _) := refine(refine_block,      */                        bisim_gjkw::check_complexity::
                /*  splitter, extend from bottom state markings and splitter)*/                        postprocess_new_noninert__stabilize_a_priori, 1, *this);
                                                                                #endif
                refine_block = refine(refine_block, splitter,
                               extend_from_bottom_state_markings_and_splitter);
                                                                                #ifndef NDEBUG
                                                                                    // now splitter must have some transitions that start in bottom states:
                                                                                    if (splitter->work_counter.has_temporary_work())
                                                                                    {
                                                                                        assert(splitter->add_work_to_bottom_transns(bisim_gjkw::
                                                                                            check_complexity::
                                                                                            prepare_for_postprocessing__make_unstable_a_posteriori, 1, *this));
                                                                                        splitter->work_counter.reset_temporary_work();
                                                                                    }
                                                                                    // account for the work a bit early, so as to avoid trouble if `splitter`
                                                                                    // is divided later:
                                                                                    if (!stabilize_a_priori_succeeded)
                                                                                    {
                                                                                        assert(splitter->add_work_to_bottom_transns(bisim_gjkw::
                                                                                                  check_complexity::
                /* Line 3.12: if refine block has marked states              */                   postprocess_new_noninert__stabilize_a_posteriori, 1, *this));
                /*             (i. e. it has new non-inert transitions) then */     }
                                                                                #endif
                if (0 < refine_block->marked_size())
                {
                    // Line 3.13: refine_block :=
                    //                 prepare_for_postprocessing(refine_block)
                    refine_block = prepare_for_postprocessing(refine_block,
                                                              splitter, false);
                    if (nullptr == refine_block)  continue;
                // Line 3.14: end if
                }
            }
                                                                                #ifndef NDEBUG
                                                                                    else
                                                                                    {   // If the following assertion does not hold, uncomment the if below.
                                                                                        assert(!splitter->work_counter.has_temporary_work());
                                                                                        // now splitter must have some transitions that start in bottom states:
                                                                                        // if (splitter->work_counter.has_temporary_work())
                                                                                        // {
                                                                                        //     assert(splitter->add_work_to_bottom_transns(bisim_gjkw::
                                                                                        //                 check_complexity::
                                                                                        //              prepare_for_postprocessing__make_unstable_a_posteriori,
                                                                                        //                                                          1, *this));
                                                                                        //     splitter->work_counter.reset_temporary_work();
                                                                                        // }
                                                                                        // account for the work a bit early, so as to avoid trouble if
                                                                                        // `splitter` is divided later:
                                                                                        assert(splitter->add_work_to_bottom_transns(bisim_gjkw::
                                                                                            check_complexity::postprocess_new_noninert__stabilize_a_priori,
                                                                                                                                                    1, *this));
            /* Line 3.15: for all bottom states s in refine_block do         */     }
                                                                                #endif
            permutation_iter_t s_iter = refine_block->begin;                    assert(s_iter < refine_block->nonbottom_begin);
            do
            {
                state_info_iter_t const s = *s_iter;                            assert(s->pos == s_iter);
                                                                                assert(succ_entry::succ_begin < s->current_out_slice.begin);
                                                                                assert(s == s->current_out_slice.begin[-1].block_bunch->pred->source);
                /* Line 3.16: Decrease the current out-slice pointer of s    */ assert(s->current_out_slice.begin[-1].begin_or_before_end() <
                /*                                 to the previous out-slice */                                                    s->current_out_slice.begin);
                s->current_out_slice.begin =
                          s->current_out_slice.begin[-1].begin_or_before_end(); // succ_entry::add_work_to_out_slice(s->current_out_slice.begin, ...) --
            // Line 3.17: end for                                               // subsumed in the call below
            }
            while (++s_iter < refine_block->nonbottom_begin);
            // Line 3.18: Sort the bottom states of refine_block according to
            //          the out-slice just before the current out-slice pointer
            std::sort(refine_block->begin, refine_block->nonbottom_begin,
                                                before_current_out_slice_less);
            s_iter = refine_block->begin;                                       assert(s_iter < refine_block->nonbottom_begin);
            do
            {
                (*s_iter)->pos = s_iter;                                        ONLY_IF_DEBUG( succ_entry::add_work_to_out_slice(*this,
                                                                                            (*s_iter)->current_out_slice.begin,
                                                                                            bisim_gjkw::check_complexity::postprocess_new_noninert__sort, 1); )
            }
            while (++s_iter < refine_block->nonbottom_begin);                   // assert(splitter->add_work_to_bottom_transns(...)) -- see above
        // Line 3.19: end for
        }
        part_tr.unstable_block_bunch_postprocess_end =
                                            part_tr.unstable_block_bunch.end();
        // Line 3.20: return result {i. e. the part of block
        //                                           without new bottom states}
        return result_block;
    }
};

///@} (end of group part_refine)





/* ************************************************************************* */
/*                                                                           */
/*                       I M P L E M E N T A T I O N S                       */
/*                                                                           */
/* ************************************************************************* */





// This section contains implementations of functions that refer to details of
// classes defined later, so they could not be defined at the point of
// declaration.

/// \brief quick check to find out whether the state has a transition in `SpBu`
/// \details If the current_out_slice pointer happens to be set to the `SpBu`
/// the function can quickly find out whether the state has a transition in
/// `SpBu`.
/// \param SpBu bunch of interest
/// \returns true if the state is known to have a transition in `SpBu`
/// \memberof state_info_entry
inline bool state_info_entry::surely_has_transition_in(const bunch_t* const
                                                                    SpBu) const
{                                                                               assert(this == &*current_out_slice.begin->block_bunch->pred->source);
                                                                                assert(current_out_slice.begin <= succ_inert.begin);
                                                                                assert(current_out_slice.begin == succ_inert.begin ||
                                                                                     succ_entry::succ_begin == current_out_slice.begin ||
                                                                                     &*current_out_slice.begin[-1].block_bunch->pred->source < this ||
                                                                                     *current_out_slice.begin[-1].bunch() < *current_out_slice.begin->bunch());
    return current_out_slice.begin < succ_inert.begin &&
                                      SpBu == current_out_slice.begin->bunch();
}


/// \brief quick check to find out whether the state has _no_ transition in
/// `SpBu`
/// \details If the current_out_slice pointer happens to be set to the new
/// bunch `NewBu` (which is the immediate successor of the most recent splitter
/// bunch `SpBu`), the function can quickly find out whether the state has a
/// transition in `SpBu`.
/// \param SpBu bunch of interest
/// \returns true if the state is known to have _no_ transition in `SpBu`
/// \memberof state_info_entry
inline bool state_info_entry::surely_has_no_transition_in(const bunch_t* const
                                                                    SpBu) const
{                                                                               assert(this == &*current_out_slice.begin->block_bunch->pred->source);
                                                                                assert(current_out_slice.begin <= succ_inert.begin);
                                                                                assert(current_out_slice.begin == succ_inert.begin ||
                                                                                     succ_entry::succ_begin == current_out_slice.begin ||
                                                                                     &*current_out_slice.begin[-1].block_bunch->pred->source < this ||
                                                                                     *current_out_slice.begin[-1].bunch() < *current_out_slice.begin->bunch());
    return (current_out_slice.begin == succ_inert.begin ||
                                  *SpBu < *current_out_slice.begin->bunch()) &&
        (&*current_out_slice.begin[-1].block_bunch->pred->source != this ||
                               *current_out_slice.begin[-1].bunch() < *SpBu);
}
                                                                                #ifndef NDEBUG
                                                                                    template <class LTS_TYPE>
                                                                                    /* static */ inline void succ_entry::add_work_to_out_slice(
                                                                                        const bisim_partitioner_dnj<LTS_TYPE>& partitioner,
                                                                                        succ_const_iter_t out_slice_begin, enum bisim_gjkw::check_complexity::
                                                                                                              counter_type const ctr, unsigned const max_value)
                                                                                    {
                                                                                        succ_const_iter_t const out_slice_before_end =
                                                                                                                        out_slice_begin->begin_or_before_end();
                                                                                        assert(out_slice_begin <= out_slice_before_end);
                                                                                        mCRL2complexity(out_slice_begin->block_bunch->pred,
                                                                                                                        add_work(ctr, max_value), partitioner);
                                                                                        while (++out_slice_begin <= out_slice_before_end)
                                                                                        {
                                                                                            // treat temporary counters specially
                                                                                            mCRL2complexity(out_slice_begin->block_bunch->pred,
                                                                                                            add_work_notemporary(ctr, max_value), partitioner);
                                                                                        }
                                                                                    }

                                                                                    static struct {
                                                                                        bool operator()(iterator_or_counter<action_block_iter_t> const& p1,
                                                                                                                 action_block_iter_t const& action_block) const
                                                                                        {
                                                                                            return p1.begin > action_block;
                                                                                        }
                                                                                    } action_label_greater;

                                                                                    /// \brief print a transition identification for debugging
                                                                                    template <class LTS_TYPE>
                                                                                    inline std::string pred_entry::debug_id(
                                                                                                      const bisim_partitioner_dnj<LTS_TYPE>& partitioner) const
                                                                                    {
                                                                                        // Search for the action label in partitioner.action_label
                                                                                        label_type const label = std::lower_bound(
                                                                                            partitioner.action_label.cbegin(), partitioner.action_label.cend(),
                                                                                                                      action_block, action_label_greater) -
                                                                                                                             partitioner.action_label.cbegin();
                                                                                        assert(0 <= label && label < partitioner.action_label.size());
                                                                                        assert(partitioner.action_label[label].begin <= action_block);
                                                                                        assert(0==label||action_block<partitioner.action_label[label-1].begin);
                                                                                        // class lts_lts_t uses a function pp() to transform the action label
                                                                                        // to a string.
                                                                                        return pp(partitioner.aut.action_label(label)) + "-transition " +
                                                                                                                                              debug_id_short();
                                                                                    }

                                                                                    /// \brief calculates the maximal allowed value for work counters
                                                                                    /// associated with this bunch
                                                                                    /// \details Work counters may only be nonzero if this bunch is a
                                                                                    /// single-action bunch, i. e. all its transitions have the same action
                                                                                    /// label.  Also, only then the size can be calculated as end - begin.
                                                                                    template <class LTS_TYPE>
                                                                                    inline int bunch_t::max_work_counter(
                                                                                                      const bisim_partitioner_dnj<LTS_TYPE>& partitioner) const
                                                                                    {
                                                                                        // verify that the bunch only has a single action label.
                                                                                        // Search for the action label in partitioner.action_label
                                                                                        label_type const label = std::lower_bound(
                                                                                            partitioner.action_label.cbegin(), partitioner.action_label.cend(),
                                                                                                                                begin, action_label_greater) -
                                                                                                                             partitioner.action_label.cbegin();
                                                                                        assert(0 <= label && label < partitioner.action_label.size());
                                                                                        assert(partitioner.action_label[label].begin <= begin);
                                                                                        assert(0 == label || begin < partitioner.action_label[label-1].begin);
                                                                                        if (0 == label || end < partitioner.action_label[label - 1].begin)
                                                                                        {
                                                                                            assert(bisim_gjkw::check_complexity::ilog2(end - begin) <=
                                                                                                                          bisim_gjkw::check_complexity::log_n);
                                                                                            return bisim_gjkw::check_complexity::log_n -
                                                                                                              bisim_gjkw::check_complexity::ilog2(end - begin);
                                                                                        }
                                                                                        return 0;
                                                                                    }
                                                                                #endif

// definitions of static members
state_type block_t::nr_of_blocks;
bunch_t* bunch_t::first_nontrivial;
                                                                                #ifndef NDEBUG
                                                                                    state_info_const_iter_t state_info_entry::s_i_begin;
                                                                                    state_info_const_iter_t state_info_entry::s_i_end;
                                                                                    permutation_const_iter_t state_info_entry::perm_begin;
                                                                                    permutation_const_iter_t state_info_entry::perm_end;
                                                                                    succ_const_iter_t succ_entry::succ_begin;
                                                                                    succ_const_iter_t succ_entry::succ_end;
                                                                                    block_bunch_const_iter_t block_bunch_slice_t::block_bunch_begin;
                                                                                    const block_bunch_iter_t* block_bunch_slice_t::block_bunch_end;
                                                                                    pred_const_iter_t pred_entry::pred_begin;
                                                                                    pred_const_iter_t pred_entry::pred_end;
                                                                                    action_block_const_iter_t action_block_entry::action_block_begin;
                                                                                    const action_block_iter_t* action_block_entry::action_block_end;
                                                                                    action_block_const_iter_t
                                                                                                             action_block_entry::action_block_orig_inert_begin;

                                                                                    // \brief moves temporary work counters to normal ones if the blue block is
                                                                                    // smaller
                                                                                    template <class LTS_TYPE>
                                                                                    static void finalise_blue_is_smaller(const block_t* const blue_block,
                                                                                                const block_t* const red_block, const bunch_t* const new_bunch,
                                                                                                            const bisim_partitioner_dnj<LTS_TYPE>& partitioner)
                                                                                    {
                                                                                        unsigned const max_bunch = nullptr == new_bunch ? 0
                                                                                                                    : new_bunch->max_work_counter(partitioner);
                                                                                        if (nullptr != blue_block)
                                                                                        {
                                                                                            unsigned const max_blue_block=bisim_gjkw::check_complexity::log_n -
                                                                                                       bisim_gjkw::check_complexity::ilog2(blue_block->size());
                                                                                            // finalise work counters for the blue states and their transitions
                                                                                            for(permutation_const_iter_t s_iter = blue_block->begin;
                                                                                                                            s_iter < blue_block->end; ++s_iter)
                                                                                            {
                                                                                                state_info_const_iter_t const s = *s_iter;
                                                                                                mCRL2complexity(s, finalise_work(bisim_gjkw::check_complexity::
                                                                                                        refine_blue__found_blue_bottom_state, bisim_gjkw::
                                                                                                        check_complexity::refine__found_blue_bottom_state,
                                                                                                                                            max_blue_block), );
                                                                                                mCRL2complexity(s, finalise_work(bisim_gjkw::check_complexity::
                                                                                                        refine_blue__find_predecessors_of_blue_state,
                                                                                                        bisim_gjkw::check_complexity::
                                                                                                        refine__find_predecessors_of_red_or_blue_state,
                                                                                                                                            max_blue_block), );
                                                                                                assert(s != pred_entry::pred_end->target);
                                                                                                for(pred_const_iter_t pred_iter = s->pred_inert.begin;
                                                                                                                           s == pred_iter->target; ++pred_iter)
                                                                                                {
                                                                                                    mCRL2complexity(pred_iter, finalise_work(
                                                                                                            bisim_gjkw::check_complexity::
                                                                                                            refine_blue__handle_transition_to_blue_state,
                                                                                                            bisim_gjkw::check_complexity::
                                                                                                            refine__handle_transition_to_red_or_blue_state,
                                                                                                                                 max_blue_block), partitioner);
                                                                                                }
                                                                                                // Sometimes, inert transitions become transitions from red to
                                                                                                // blue states; therefore, we also have to walk through the
                                                                                                // noninert predecessors of blue states:
                                                                                                assert(s != pred_entry::pred_begin[-1].target);
                                                                                                for(pred_const_iter_t pred_iter = s->pred_inert.begin;
                                                                                                                                  s == (--pred_iter)->target; )
                                                                                                {
                                                                                                    mCRL2complexity(pred_iter, finalise_work(
                                                                                                            bisim_gjkw::check_complexity::
                                                                                                            refine_blue__handle_transition_to_blue_state,
                                                                                                            bisim_gjkw::check_complexity::
                                                                                                            refine__handle_transition_to_red_or_blue_state,
                                                                                                                                 max_blue_block), partitioner);
                                                                                                }
                                                                                                assert(s !=
                                                                                                         succ_entry::succ_begin[-1].block_bunch->pred->source);
                                                                                                for(succ_const_iter_t succ_iter = s->succ_inert.begin;
                                                                                                               s == (--succ_iter)->block_bunch->pred->source; )
                                                                                                {
                                                                                                    mCRL2complexity(succ_iter->block_bunch->pred,finalise_work(
                                                                                                          bisim_gjkw::check_complexity::refine_blue__slow_test,
                                                                                                          bisim_gjkw::check_complexity::
                                                                                                          refine__handle_transition_from_red_or_blue_state,
                                                                                                                                 max_blue_block), partitioner);
                                                                                                }
                                                                                            }
                                                                                        }
                                                                                        // cancel work counters for the red states and their transitions, and
                                                                                        // also account for work done in the blue coroutine on red states
                                                                                        for (permutation_const_iter_t s_iter = red_block->begin;
                                                                                                                             s_iter < red_block->end; ++s_iter)
                                                                                        {
                                                                                            state_info_const_iter_t const s = *s_iter;
                                                                                            mCRL2complexity(s, cancel_work(bisim_gjkw::check_complexity::
                                                                                                                refine_red__find_predecessors_of_red_state), );
                                                                                            assert(s != pred_entry::pred_end->target);
                                                                                            for (pred_const_iter_t pred_iter = s->pred_inert.begin;
                                                                                                                           s == pred_iter->target; ++pred_iter)
                                                                                            {
                                                                                                mCRL2complexity(pred_iter, cancel_work(
                                                                                                     bisim_gjkw::check_complexity::
                                                                                                     refine_red__handle_transition_to_red_state), partitioner);
                                                                                            }
                                                                                            assert(s != succ_entry::succ_begin[-1].block_bunch->pred->source);
                                                                                            for (succ_const_iter_t succ_iter = s->succ_inert.begin;
                                                                                                               s == (--succ_iter)->block_bunch->pred->source; )
                                                                                            {
                                                                                                mCRL2complexity(succ_iter->block_bunch->pred, cancel_work(
                                                                                                   bisim_gjkw::check_complexity::
                                                                                                   refine_red__handle_transition_from_red_state), partitioner);
                                                                                                // the following counters measure work done in the blue
                                                                                                // coroutine that found red states.
                                                                                                mCRL2complexity(succ_iter->block_bunch->pred, finalise_work(
                                                                                                        bisim_gjkw::check_complexity::
                                                                                                        refine_blue__found_red_bottom_state,
                                                                                                        bisim_gjkw::check_complexity::
                                                                                                        refine__handle_transition_in_FromRed,
                                                                                                                                      max_bunch), partitioner);
                                                                                                mCRL2complexity(succ_iter->block_bunch->pred,finalise_work(
                                                                                                        bisim_gjkw::check_complexity::refine_blue__slow_test,
                                                                                                        bisim_gjkw::check_complexity::
                                                                                                           refine__slow_test_found_red_state, 1), partitioner);
                                                                                            }
                                                                                        }
                                                                                        bisim_gjkw::check_complexity::check_temporary_work();
                                                                                    }

                                                                                    // \brief moves temporary work counters to normal ones if the red block is
                                                                                    // smaller
                                                                                    template <class LTS_TYPE>
                                                                                    static void finalise_red_is_smaller(const block_t* const blue_block,
                                                                                                            const block_t* const red_block,
                                                                                                            const bisim_partitioner_dnj<LTS_TYPE>& partitioner)
                                                                                    {
                                                                                        unsigned const max_red_block = bisim_gjkw::check_complexity::log_n -
                                                                                                        bisim_gjkw::check_complexity::ilog2(red_block->size());
                                                                                        // cancel work counters for the blue states and their transitions
                                                                                        for (permutation_const_iter_t s_iter = blue_block->begin;
                                                                                                                            s_iter < blue_block->end; ++s_iter)
                                                                                        {
                                                                                            state_info_const_iter_t const s = *s_iter;
                                                                                            mCRL2complexity(s, cancel_work(bisim_gjkw::check_complexity::
                                                                                                                      refine_blue__found_blue_bottom_state), );
                                                                                            mCRL2complexity(s, cancel_work(bisim_gjkw::check_complexity::
                                                                                                              refine_blue__find_predecessors_of_blue_state), );
                                                                                            assert(s != pred_entry::pred_end->target);
                                                                                            for (pred_const_iter_t pred_iter = s->pred_inert.begin;
                                                                                                                           s == pred_iter->target; ++pred_iter)
                                                                                            {
                                                                                                mCRL2complexity(pred_iter, cancel_work(
                                                                                                   bisim_gjkw::check_complexity::
                                                                                                   refine_blue__handle_transition_to_blue_state), partitioner);
                                                                                            }
                                                                                            // Sometimes, inert transitions become transitions from red to
                                                                                            // blue states; therefore, we also have to walk through the
                                                                                            // noninert predecessors of blue states:
                                                                                            assert(s != pred_entry::pred_begin[-1].target);
                                                                                            for (pred_const_iter_t pred_iter = s->pred_inert.begin;
                                                                                                                                  s == (--pred_iter)->target; )
                                                                                            {
                                                                                                mCRL2complexity(pred_iter, cancel_work(
                                                                                                   bisim_gjkw::check_complexity::
                                                                                                   refine_blue__handle_transition_to_blue_state), partitioner);
                                                                                            }
                                                                                            assert(s != succ_entry::succ_begin[-1].block_bunch->pred->source);
                                                                                            for (succ_const_iter_t succ_iter = s->succ_inert.begin;
                                                                                                               s == (--succ_iter)->block_bunch->pred->source; )
                                                                                            {
                                                                                                mCRL2complexity(succ_iter->block_bunch->pred, cancel_work(
                                                                                                        bisim_gjkw::check_complexity::refine_blue__slow_test),
                                                                                                                                                  partitioner);
                                                                                            }
                                                                                        }
                                                                                        // finalise work counters for the red states and their transitions
                                                                                        for (permutation_const_iter_t s_iter = red_block->begin;
                                                                                                                             s_iter < red_block->end; ++s_iter)
                                                                                        {
                                                                                            state_info_const_iter_t const s = *s_iter;
                                                                                            mCRL2complexity(s, finalise_work(bisim_gjkw::check_complexity::
                                                                                                    refine_red__find_predecessors_of_red_state,
                                                                                                    bisim_gjkw::check_complexity::
                                                                                                    refine__find_predecessors_of_red_or_blue_state,
                                                                                                                                             max_red_block), );
                                                                                            assert(s != pred_entry::pred_end->target);
                                                                                            for (pred_const_iter_t pred_iter = s->pred_inert.begin;
                                                                                                                           s == pred_iter->target; ++pred_iter)
                                                                                            {
                                                                                                mCRL2complexity(pred_iter, finalise_work(
                                                                                                        bisim_gjkw::check_complexity::
                                                                                                        refine_red__handle_transition_to_red_state,
                                                                                                        bisim_gjkw::check_complexity::
                                                                                                        refine__handle_transition_to_red_or_blue_state,
                                                                                                                                  max_red_block), partitioner);
                                                                                            }
                                                                                            assert(s != succ_entry::succ_begin[-1].block_bunch->pred->source);
                                                                                            for (succ_const_iter_t succ_iter = s->succ_inert.begin;
                                                                                                               s == (--succ_iter)->block_bunch->pred->source; )
                                                                                            {
                                                                                                mCRL2complexity(succ_iter->block_bunch->pred, finalise_work(
                                                                                                        bisim_gjkw::check_complexity::
                                                                                                        refine_red__handle_transition_from_red_state,
                                                                                                        bisim_gjkw::check_complexity::
                                                                                                        refine__handle_transition_from_red_or_blue_state,
                                                                                                                                  max_red_block), partitioner);
                                                                                                // the following counter actually is work done in the blue
                                                                                                // coroutine that found red states.
                                                                                                mCRL2complexity(succ_iter->block_bunch->pred, cancel_work(
                                                                                                            bisim_gjkw::check_complexity::
                                                                                                            refine_blue__found_red_bottom_state), partitioner);
                                                                                                mCRL2complexity(succ_iter->block_bunch->pred, cancel_work(
                                                                                                        bisim_gjkw::check_complexity::refine_blue__slow_test),
                                                                                                                                                  partitioner);
                                                                                            }
                                                                                        }
                                                                                        bisim_gjkw::check_complexity::check_temporary_work();
                                                                                    }
                                                                                #endif
} // end namespace bisim_dnj





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
void bisimulation_reduce_dnj(LTS_TYPE& l, bool const branching /* = false */,
     bool const preserve_divergence /* = false */,
     std::vector<std::size_t>* const arbitrary_state_per_block /* = nullptr */)
{
    // First, contract tau-SCCs to single states in case of branching
    // bisimulation.
    if (branching)
    {
        scc_reduce(l, preserve_divergence);
    }

    // Second, apply the branching bisimulation reduction algorithm. If there
    // are no taus, this will automatically yield strong bisimulation.
    if (1 < l.num_states())
    {
        bisim_dnj::bisim_partitioner_dnj<LTS_TYPE> bisim_part(l, branching,
                                                          preserve_divergence);

        // Assign the reduced LTS
        bisim_part.finalize_minimized_LTS(arbitrary_state_per_block);
    }
}


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
                            bool const branching /* = false */,
                            bool const preserve_divergence /* = false */,
                            bool const generate_counter_examples /* = false */)
{
    if (generate_counter_examples)
    {
        mCRL2log(log::warning) << "The DNJ branching bisimulation "
                              "algorithm does not generate counterexamples.\n";
    }
    std::size_t init_l2 = l2.initial_state() + l1.num_states();
    merge(l1, l2);
    l2.clear(); // No use for l2 anymore.

    // First, contract tau-SCCs to single states in case of branching
    // bisimulation.
    if (branching)
    {
        scc_partitioner<LTS_TYPE> scc_part(l1);
        scc_part.replace_transition_system(preserve_divergence);
        init_l2 = scc_part.get_eq_class(init_l2);
    }

    if (1 >= l1.num_states())  return true;

    bisim_dnj::bisim_partitioner_dnj<LTS_TYPE> bisim_part(l1, branching,
                                                          preserve_divergence);

    return bisim_part.in_same_class(l1.initial_state(), init_l2);
}



/*=============================================================================
=                       explicit instantiation requests                       =
=============================================================================*/



template void bisimulation_reduce_dnj(lts_lts_t& l, bool branching, bool
     preserve_divergence, std::vector<std::size_t>* arbitrary_state_per_block);
template void bisimulation_reduce_dnj(lts_aut_t& l, bool branching, bool
     preserve_divergence, std::vector<std::size_t>* arbitrary_state_per_block);
template void bisimulation_reduce_dnj(lts_fsm_t& l, bool branching, bool
     preserve_divergence, std::vector<std::size_t>* arbitrary_state_per_block);

template bool destructive_bisimulation_compare_dnj(lts_lts_t& l1,lts_lts_t& l2,
     bool branching, bool preserve_divergence, bool generate_counter_examples);
template bool destructive_bisimulation_compare_dnj(lts_aut_t& l1,lts_aut_t& l2,
     bool branching, bool preserve_divergence, bool generate_counter_examples);
template bool destructive_bisimulation_compare_dnj(lts_fsm_t& l1,lts_fsm_t& l2,
     bool branching, bool preserve_divergence, bool generate_counter_examples);

///@} (end of group part_interface)

} // end namespace detail
} // end namespace lts
} // end namespace mcrl2
