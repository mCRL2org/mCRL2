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
/// \details This file implements an efficient partition refinement algorithm
/// for labelled transition systems inspired by Groote / Jansen / Keiren / Wijs
/// and Valmari (2009) to calculate the branching bisimulation classes of a
/// labelled transition system.
///
/// Different from that article, it does not translate a LTS to a Kripke
/// structure but works on the LTS directly.
///
/// \author David N. Jansen, Institute of Software, Chinese Academy of
/// Sciences, Beijing, PR China

#ifndef LIBLTS_BISIM_DNJ_H
#define LIBLTS_BISIM_DNJ_H

#include <list>  // for the list of block_bunch-slices

#include "mcrl2/lts/detail/liblts_scc.h"
#include "mcrl2/lts/detail/liblts_merge.h"
#include "mcrl2/lts/detail/fixed_vector.h"

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
                                                                                    /// mode and to nothing otherwise.
                                                                                    #define ONLY_IF_DEBUG(...) __VA_ARGS__
                                                                                #else
                                                                                    #define ONLY_IF_DEBUG(...)
                                                                                #endif
// state_type and trans_type are defined in check_complexity.h.
typedef std::size_t state_type;
typedef std::size_t trans_type;

/// \brief type used to store label numbers and counts
typedef std::size_t label_type;

namespace bisim_dnj
{

// auxiliary template types
template <class Iterator>
union iterator_or_counter
{
    trans_type count;
    Iterator begin;

    iterator_or_counter()  {  count = 0;  }
    ~iterator_or_counter()  {  }

    const iterator_or_counter& operator=(const iterator_or_counter& other)
    {                                                                           assert(sizeof(count) == sizeof(begin));
        begin = other.begin;
        return *this;
    }
    bool operator==(const iterator_or_counter& other) const
    {                                                                           assert(sizeof(count) == sizeof(begin));
        return begin == other.begin;
    }
    bool operator!=(const iterator_or_counter& other) const
    {
        return !operator==(other);
    }
};

template <class Iterator>
union iterator_or_null
{
  private:
    const void* null;
    // sometimes we also need a third type, a label.
    label_type label;
  public:
    Iterator iter;

    iterator_or_null()  {  }
    explicit iterator_or_null(Iterator new_iter)
    {
        iter = new_iter;                                                        assert(nullptr != null);
    }
    explicit iterator_or_null(std::nullptr_t new_null)
    {                                                                           assert(nullptr == new_null);
        null = new_null;
    }
    ~iterator_or_null()  {  }

    const iterator_or_null& operator=(const iterator_or_null& other)
    {                                                                           assert(sizeof(null) == sizeof(iter));  assert(sizeof(null) >= sizeof(label));
        iter = other.iter;
        return *this;
    }
    bool operator==(const iterator_or_null& other) const
    {                                                                           assert(sizeof(null) == sizeof(iter));  assert(sizeof(null) >= sizeof(label));
        return iter == other.iter;
    }
    bool operator!=(const iterator_or_null& other) const
    {
        return !operator==(other);
    }

    bool is_null() const  {  return nullptr == null;  }
    void clear()  {  null = nullptr;  }
    void set(Iterator new_iter)
    {
        iter = new_iter;                                                        assert(!is_null());
    }
    const Iterator& operator()() const
    {                                                                           assert(!is_null());
        return iter;
    }

    void set_label(label_type new_label)  {  label = new_label;  }
    label_type get_label() const  {  return label;  }
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

typedef state_info_entry* state_info_ptr;
typedef const state_info_entry* state_info_const_ptr;

/// \class permutation_t
/// \brief stores a permutation of the states, ordered by block
/// \details This is the central concept of the _refinable partition_: the
/// permutation of the states, such that states belonging to the same block are
/// adjacent.
///
/// Iterating over the states of a block will
/// therefore be done using the permutation_t array.
typedef bisim_gjkw::fixed_vector<state_info_ptr> permutation_t;
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
///
/// A `state_info_entry` only works correctly if it is part of an array where
/// there is one more `state_info_entry`.  The reason is that iterators past
/// the last transition are not actually stored here, as they are equal to the
/// iterator to the first transition of the next state.  The array will contain
/// one additional ``state'' that is only used for these pointers.
class state_info_entry
{
  public:
    /// \brief iterator to first incoming transition
    /// \details also serves as iterator past the last incoming transition of
    /// the previous state.
    iterator_or_counter<pred_iter_t> pred;

    /// \brief iterator to first outgoing transition
    /// \details also serves as iterator past the last outgoing transition of
    /// the previous state.
    iterator_or_counter<succ_iter_t> succ;

    /// iterator to first _inert_ incoming transition
    iterator_or_counter<pred_iter_t> pred_inert;

    /// iterator to first _inert_ outgoing transition
    iterator_or_counter<succ_iter_t> succ_inert;

    /// block where the state belongs
    block_t* block;

    /// position of the state in the permutation array
    permutation_iter_t pos;

    /// number of inert transitions to non-blue states
    state_type notblue;

    /// iterator to first outgoing transition to the bunch of interest
    succ_iter_t current_out_slice;

    /// iterator past the last incoming transition
    pred_const_iter_t pred_cend() const
    {                                                                           assert(s_i_begin <= this);  assert(this < s_i_end);
        return this[1].pred.begin;
    }
    pred_iter_t pred_end()
    {                                                                           assert(s_i_begin <= this);  assert(this < s_i_end);
        return this[1].pred.begin;
    }

    /// iterator past the last outgoing transition
    succ_const_iter_t succ_cend() const
    {                                                                           assert(s_i_begin <= this);  assert(this < s_i_end);
        return this[1].succ.begin;
    }
    succ_iter_t succ_end()
    {                                                                           assert(s_i_begin <= this);  assert(this < s_i_end);
        return this[1].succ.begin;
    }

    bool surely_has_transition_to(const bunch_t* SpBu) const;
    bool surely_has_no_transition_to(const bunch_t* SpBu) const;
                                                                                #ifndef NDEBUG
                                                                                /// \brief print a short state identification for debugging
                                                                                std::string debug_id_short() const
                                                                                {   assert(s_i_begin <= this);  assert(this < s_i_end);
                                                                                    return std::to_string(this - s_i_begin);
                                                                                }

                                                                                /// \brief print a state identification for debugging
                                                                                std::string debug_id() const
                                                                                {
                                                                                    return "state " + debug_id_short();
                                                                                }

                                                                                /// \brief pointer at the first entry in the `state_info` array
                                                                                static state_info_const_ptr s_i_begin;

                                                                                /// \brief pointer past the last actual entry in the `state_info` array
                                                                                /// \details `state_info` actually contains an additional entry that is only
                                                                                /// used partially, namely to store pointers to the end of the transition
                                                                                /// slices of the last state.  In other words, `s_i_end` points at this
                                                                                /// additional, partially used entry.
                                                                                static state_info_const_ptr s_i_end;
                                                                                #endif
};


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
    /// iterator past the last state of the block
    permutation_iter_t end;

    /// iterator to the first marked non-bottom state of the block
    permutation_iter_t marked_nonbottom_begin;

    /// iterator to the first non-bottom state of the block
    permutation_iter_t nonbottom_begin;

    /// iterator to the first marked bottom state of the block
    permutation_iter_t marked_bottom_begin;

    /// iterator to the first state of the block
    permutation_iter_t begin;

    /// \details We assume that the list of stable block_bunch-slices generally
    /// does not contain empty slices; the list of unstable block_bunch-slices
    /// may contain empty slices (that should be deleted but cannot because the
    /// forward_list doesn't allow us to find the predecessor of the slice).
    ///
    /// When refining a block, it can happen that a block_bunch-slice becomes
    /// empty, but upon careful analysis we see that these situations are very
    /// much restricted.  As a bottom state contains a transition in every
    /// stable block_bunch-slice, such a slice becomes empty only if the red
    /// subblock becomes a block without any bottom states (and is the larger
    /// subblock).  But if that subblock doesn't have any bottom states, we
    /// will soon move all its stable block_bunch-slices to the unstable ones
    /// (individually, to ensure that they are properly marked as unstable),
    /// so we will have time to remove any slice that may have become empty.
    ///
    /// In the list of unstable block_bunch-slices, a slice may become empty
    /// more often.  If it's the from_red slice (of the red subblock), it can
    /// be removed in constant time, as the list of unstable block_slices
    /// contains only one or two elements in this situation.  If a slice
    /// becomes empty during postprocessing, the situation is a bit more
    /// complex, but still manageable:  Every time a block_bunch-slice becomes
    /// empty, this can be ascribed to a specific transition that moved from
    /// the unstable to the stable block_bunch-slices.  If we make sure that
    /// the empty slice is removed before that transition becomes unstable
    /// again and is part of a splitter another time the memory overhead is
    /// limited.  A transition can only become unstable during postprocessing
    /// when additional new bottom states are found.  In that case we should
    /// make sure the empty slice is removed before that transition is part
    /// of a splitter again.  This can be achieved by appending additional
    /// block_bunch-slices always at the end of the list.
    ///
    /// In principle, it is necessary to keep the list of stable
    /// block_bunch-slices sorted in the order of bunches.  We need that in
    /// particular to make the from_red block_bunch-slice unstable:  we need
    /// access to its predecessor.  Also, if a stable block_bunch-slice becomes
    /// empty (because all its transitions move to the new block), it has to be
    /// removed from the list.
    /// In principle, this is realized by reordering the list of stable
    /// block_bunch-slices of the new block after a split.  However, in
    /// practice, this list is generated in the order in which the last state
    /// of the new block has stored its out-slices; as this is a bottom state
    /// in most cases, the list will automatically be sorted.  The list only
    /// becomes unordered during postprocessing, when we create a block that
    /// does not contain any old bottom state.  But at that moment, we will
    /// stabilize w. r. t. all block_bunch-slices anyway, and we have time to
    /// walk through them after stabilisation and sort them by going over the
    /// out-slices of a (new, stabilized) bottom state.
    ///
    /// It is probably possible to remove stable_block_bunch_before_end if one
    /// handles new bottom states in the right way.

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
    bool mark_nonbottom(state_info_ptr s);

    /// \brief mark a state
    /// \details Marking is done by moving the state to the slice of the marked
    /// bottom or non-bottom states of the block.  If `s` is an old bottom
    /// state, it is treated as if it already were marked.
    /// \param s the state that has to be marked
    /// \returns true if the state was not marked before
    bool mark(state_info_ptr s);

    /// \brief refine the block
    /// \details This function is called after a refinement function has found
    /// where to split the block into unmarked (blue) and marked (red) states.
    /// It creates a new block for the blue states.
    /// \param   new_block_mode indicates whether the blue or the red block
    ///          should be the new one.
    /// \returns pointer to the new block
    block_t* split_off_block(enum new_block_mode_t new_block_mode);
                                                                                #ifndef NDEBUG
                                                                                /// \brief print a block identification for debugging
                                                                                std::string debug_id() const
                                                                                {   assert(perm_begin <= begin); assert(begin < end); assert(end <= perm_end);
                                                                                    return "block [" + std::to_string(begin - perm_begin) + "," +
                                                                                       std::to_string(end - perm_begin) + ") (#" + std::to_string(seqnr) + ")";
                                                                                }

                                                                                /// \brief provide an iterator to the beginning of the permutation array
                                                                                /// \details This iterator is required to be able to print identifications
                                                                                /// for debugging.
                                                                                static permutation_iter_t perm_begin;
                                                                                static permutation_const_iter_t perm_end;
                                                                                #endif
};


/// swap two permutations
/*
/// \details Note that the two permutation array entries are given as
/// references, not as iterators.  We have to jump through a hoop to turn
/// those pointers into iterators again...  We hope that the compiler optimizes
/// the calculation away.
///
/// We need the function with this signature because we sometimes sort the
/// entries in the permutation array using `std::sort()`.  However, this only
/// works if state_info_ptr is a class (not a pointer).
static inline void swap(state_info_ptr& s1, state_info_ptr& s2)
{                                                                               assert(&*block_t::perm_begin <= &s1);  assert(&s1 < &*block_t::perm_end);
                                                                                assert(&*block_t::perm_begin <= &s2);  assert(&s2 < &*block_t::perm_end);
    iter_swap(block_t::perm_begin + (&s1 - &*block_t::perm_begin),
                          block_t::perm_begin + (&s2 - &*block_t::perm_begin));
}
*/
static inline void iter_swap(permutation_iter_t s1, permutation_iter_t s2)
{                                                                               assert((*s1)->pos == s1);  assert((*s2)->pos == s2);
    state_info_ptr temp = *s1;                                                  assert(block_t::perm_begin <= s1);  assert(s1 < block_t::perm_end);
    *s1 = *s2;                                                                  assert(block_t::perm_begin <= s2);  assert(s2 < block_t::perm_end);
    *s2 = temp;
    (*s1)->pos = s1;
    (*s2)->pos = s2;
}


/// \brief mark a non-bottom state
/// \details Marking is done by moving the state to the slice of the marked
/// non-bottom states of the block.
/// \param s the non-bottom state that has to be marked
/// \returns true if the state was not marked before
inline bool block_t::mark_nonbottom(state_info_ptr s)
{                                                                               assert(this==s->block);  assert(nonbottom_begin<=s->pos);  assert(s->pos<end);
                                                                                assert(begin <= marked_bottom_begin);
                                                                                assert(marked_bottom_begin <= nonbottom_begin);
                                                                                assert(nonbottom_begin <= marked_nonbottom_begin);
    if (marked_nonbottom_begin <= s->pos)  return false;                        assert(marked_nonbottom_begin <= end);
    iter_swap(s->pos, --marked_nonbottom_begin);                                assert(nonbottom_begin <= marked_nonbottom_begin);
    return true;
}


/// \brief mark a state
/// \details Marking is done by moving the state to the slice of the marked
/// bottom or non-bottom states of the block.  If `s` is an old bottom
/// state, it is treated as if it already were marked.
/// \param s the state that has to be marked
/// \returns true if the state was not marked before
inline bool block_t::mark(state_info_ptr s)
{                                                                               assert(this == s->block);  assert(begin <= s->pos);
    if (s->pos < nonbottom_begin)
    {                                                                           assert(begin <= marked_bottom_begin);  assert(marked_nonbottom_begin <= end);
                                                                                assert(nonbottom_begin <= marked_nonbottom_begin);
        if (marked_bottom_begin <= s->pos)  return false;                       assert(marked_bottom_begin <= nonbottom_begin);
        iter_swap(s->pos, --marked_bottom_begin);                               assert(begin <= marked_bottom_begin);
        return true;
    }
    return mark_nonbottom(s);
}



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
    /// \details The constructor allocates memory, but does not actually
    /// initialise the partition.  Immediately afterwards, the initialisation
    /// will be done in `bisim_partitioner_dnj::create_initial_partition()`.
    /// \param n number of states in the Kripke structure
    part_state_t(state_type num_states)
      : permutation(num_states),
        state_info(num_states + 1)
    {                                                                           assert(0 == block_t::nr_of_blocks);
                                                                                #ifndef NDEBUG
                                                                                block_t::perm_begin=permutation.begin();  block_t::perm_end=permutation.end();
                                                                                state_info_entry::s_i_begin = state_info.data();
                                                                                state_info_entry::s_i_end = state_info_entry::s_i_begin+num_states;
                                                                                #endif
        permutation_iter_t perm_iter = permutation.begin();
        state_info_ptr state_iter = &*state_info.begin();
        for (; permutation.end() != perm_iter; ++perm_iter, ++state_iter)
        {
            state_iter->pos = perm_iter;
            *perm_iter = state_iter;
        }
    }

    /// \brief destructor
    /// \details The destructor also deallocates the blocks, as they are not
    /// directly referenced from anywhere.
    ~part_state_t()
    {
        permutation_iter_t permutation_iter = permutation.end();                ONLY_IF_DEBUG( state_type deleted_blocks = 0; )
        while (permutation.begin() != permutation_iter)
        {
            block_t* const B = permutation_iter[-1]->block;                     assert(B->end == permutation_iter);
            permutation_iter = B->begin;                                        ONLY_IF_DEBUG( ++deleted_blocks; )
            delete B;
        }                                                                       assert(deleted_blocks == block_t::nr_of_blocks);
        block_t::nr_of_blocks = 0;
    }

    /// \brief provide size of state space
    /// \returns the stored size of the state space
    state_type state_size() const  {  return permutation.size();  }

    /// \brief find block of a state
    /// \param s number of the state
    /// \returns a pointer to the block where state s resides in
    const block_t* block(state_type s) const
    {
        return state_info[s].block;
    }
                                                                                #ifndef NDEBUG
                                                                                private:
                                                                                /// \brief print a slice of the partition (typically a block)
                                                                                /// \details If the slice indicated by the parameters is not empty, the states
                                                                                /// in this slice will be printed.
                                                                                /// \param message text printed as a title if the slice is not empty
                                                                                /// \param B       block that is being printed (it is checked whether states
                                                                                ///                belong to this block)
                                                                                /// \param begin   iterator to the beginning of the slice
                                                                                /// \param end     iterator past the end of the slice
                                                                                void print_block(const char* message, const block_t* B,
                                                                                           permutation_const_iter_t begin, permutation_const_iter_t end) const;
                                                                                public:
                                                                                /// \brief print the partition per block
                                                                                /// \details The function prints all blocks in order.  For each block, it lists
                                                                                /// its states, separated into nonbottom and bottom states.
                                                                                /// \param part_tr partition for the transitions
                                                                                void print_part() const;

                                                                                /// \brief asserts that the partition of states is consistent
                                                                                void assert_consistency(bool branching) const;
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
///   in the same bunch.  It is possible to find out, in time logarithmic in
///   the out-degree, whether a state has a transition in a given bunch.
/// - `action_block`: a permutation of the transitions such that transitions
///   in the same bunch are adjacent, and within each bunch transitions with
///   the same action label and target block.
/// - `block_bunch`: a permutation of the transitions suche that transitions
///   from the same block in the same bunch are adjacent.
///
/// Within this sort order, inert transitions are always placed after non-inert
/// transitions.
///
/// state_info_entry and block_t (defined above) contain pointers to the slices
/// of these arrays.  For the incoming transitions, they contain enough
/// information; for the outgoing and the B_a_B-transitions, we additionally
/// use so-called _descriptors_ that show which slice belongs together.

///@{

/// \brief (non-inert and inert) successors sorted per state and bunch
class succ_entry
{
  public:
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

    /// \brief find the end of the out-slice
    succ_iter_t out_slice_before_end() const;

    bunch_t* bunch() const;
};


/// \brief (non-inert) successors sorted per (block, bunch) pair
class block_bunch_entry
{
  public:
    pred_iter_t pred;

    // The slice is null iff the transition is inert.
    iterator_or_null<block_bunch_slice_iter_t> slice;
};


} // end namespace bisim_dnj

// forward declaration:
template <class LTS_TYPE> class bisim_partitioner_dnj;

namespace bisim_dnj
{

/// \brief (non-inert and inert) predecessors sorted per state
class pred_entry
{
  public:
    action_block_iter_t action_block;

    /// \brief source state of the transition
    state_info_ptr source;

    /// \brief target state of the transition
    /// \details Because pred_entry is moved seldomly, we put this information
    /// here so it is easier to move other entries.
    state_info_ptr target;
                                                                                #ifndef NDEBUG
                                                                                /// \brief print a short transition identification for debugging
                                                                                std::string debug_id_short() const
                                                                                {
                                                                                    return "from " +source->debug_id_short() +" to " +target->debug_id_short();
                                                                                }

                                                                                /// \brief print a transition identification for debugging
                                                                                template <class LTS_TYPE>
                                                                                std::string debug_id(const bisim_partitioner_dnj<LTS_TYPE>& partitioner) const;
                                                                                #endif
};


/// \brief (non-inert) predecessors sorted per (action, block) pair
class action_block_entry
{
  public:
    /// \brief iterator to the part_tr.succ entry of the transition
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
            result += this - &*result; // result = iterator(this);
        }
                                                                                #ifndef NDEBUG
                                                                                // assert(this has the same action as result);
                                                                                // The following assertion does not always hold: the function is called
                                                                                // immediately after a block is refined, so it may be the case that the
                                                                                // transitions are still to be moved to different slices.
                                                                                // assert(succ()->block_bunch->pred->target->block ==
                                                                                //               result->succ()->block_bunch->pred->target->block);
                                                                                assert(succ()->bunch() == result->succ()->bunch());
                                                                                assert(result == action_block_begin || result[-1].succ.is_null() ||
                                                                                                action_block_orig_inert_begin <= result ||
                                                                                                result[-1].succ()->block_bunch->pred->target->block !=
                                                                                                             result->succ()->block_bunch->pred->target->block);
                                                                                #endif
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
          : data(reinterpret_cast<intptr_t>(new_bunch) | is_stable)
        {                                                                       assert(sizeof(bunch_t*) == sizeof(intptr_t));
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

    /// compute the source block of the transitions in this slice
    block_t* source_block() const;

    block_bunch_slice_t(block_bunch_iter_t new_end,
                                            bunch_t* new_bunch, bool is_stable)
      : end(new_end),
        bunch(new_bunch, is_stable)
    {  }

    bool empty() const;

    static block_bunch_const_iter_t block_bunch_begin;
                                                                                #ifndef NDEBUG
                                                                                static const block_bunch_iter_t* block_bunch_end;
                                                                                std::string debug_id() const;
                                                                                #endif
};


/// \brief bunch of transitions
/// \details Like a slice, at the end of the algorithm there will be a bunch
/// for every transition in the bisimulation quotient.  Therefore, we should
/// try to minimize the size of a bunch as much as possible.  Perhaps we can
/// reuse some part:  for example, do we need `postprocess.list_entry` and
/// `new_slice_pos` at the same time?  Otherwise we can make a union { } out of
// them, so the structure will contain 5 pointers instead of 6.
/// Can we reduce by one more pointer?  Perhaps we can set these pointers to
/// `nullptr` whenever unused, so we can test `== nullptr` instead of the
/// test done here?  Perhaps define a few helper macros to assign `nullptr` to
/// an iterator that is actually a wrapper around a pointer?
class bunch_t
{
  public:
    action_block_iter_t end;
    action_block_iter_t begin;

    trans_type const sort_id;
//  private:
    static bunch_t* first_nontrivial;
    bunch_t* next_nontrivial;
  public:
    bunch_t(action_block_iter_t new_begin, action_block_iter_t new_end,
                                                        trans_type new_sort_id)
      : end(new_end),
        begin(new_begin),
        sort_id(new_sort_id),
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
                                                                                assert(last_slice_begin < end);  assert(!first_slice_end[-1].succ.is_null());
        /* 2.6: Create a new bunch NewBu and ... */                             assert(!last_slice_begin->succ.is_null());
        bunch_t* new_bunch;

        // 2.5: Choose a small splitter B_a_B slice SpSl subset of SpBu,
        //      i.e. |SpSl| <= 1/2*|SpBu|
        /// It doesn't matter very much how ties are resolved here:
        if (first_slice_end - begin > end - last_slice_begin)
        {
            new_bunch = new bunch_t(last_slice_begin, end,
                                         sort_id + (last_slice_begin - begin));
            // 2.6: ... and move SpB from SpC to NewC
            end = last_slice_begin;
            while (end[-1].succ.is_null())
            {
                --end;                                                          assert(first_slice_end <= end);  assert(end->begin_or_before_end.is_null());
            }
            if (first_slice_end == end)  make_trivial();
        }
        else
        {
            new_bunch = new bunch_t(begin, first_slice_end,
                                            sort_id + (end - first_slice_end));
            // 2.6: ... and move SpB from SpC to NewC
            begin = first_slice_end;
            while (begin->succ.is_null())
            {                                                                   assert(begin->begin_or_before_end.is_null());
                ++begin;                                                        assert(begin <= last_slice_begin);
            }
            if (begin == last_slice_begin)  make_trivial();
        }
        return new_bunch;
    }

    bool operator<(const bunch_t& el) const  {  return sort_id < el.sort_id;  }
    bool operator>(const bunch_t& el) const  {  return el.operator<(*this);  }
    bool operator<=(const bunch_t& el) const  {  return !operator>(el);  }
    bool operator>=(const bunch_t& el) const  {  return !operator<(el);  }
                                                                                #ifndef NDEBUG
                                                                                std::string debug_id_short() const
                                                                                {   assert(action_block_entry::action_block_begin <= begin);
                                                                                    assert(begin < end);  assert(end <= *action_block_entry::action_block_end);
                                                                                    return "bunch [" +
                                                                                         std::to_string(begin - action_block_entry::action_block_begin) + "," +
                                                                                         std::to_string(end - action_block_entry::action_block_begin) + ")";
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
                                                                                #endif
};


class part_trans_t
{
  public:
    bisim_gjkw::fixed_vector<succ_entry> succ;
    bisim_gjkw::fixed_vector<block_bunch_entry> block_bunch;
    bisim_gjkw::fixed_vector<pred_entry> pred;
    bisim_gjkw::fixed_vector<action_block_entry> action_block;

    block_bunch_iter_t block_bunch_inert_begin;
    action_block_iter_t action_block_inert_begin;

    std::list<block_bunch_slice_t> unstable_block_bunch;
    block_bunch_slice_iter_t unstable_block_bunch_postprocess_end;

    part_trans_t(trans_type num_transitions, trans_type num_actions)
      : succ(num_transitions + 1),
        block_bunch(num_transitions),
        pred(num_transitions + 1),
        action_block(num_transitions + num_actions - 1),
        block_bunch_inert_begin(block_bunch.end()),
        action_block_inert_begin(action_block.end()),
        unstable_block_bunch(),
        unstable_block_bunch_postprocess_end(unstable_block_bunch.end())
    {
        block_bunch_slice_t::block_bunch_begin = block_bunch.begin();
                                                                                #ifndef NDEBUG
                                                                                block_bunch_slice_t::block_bunch_end = &block_bunch_inert_begin;
                                                                                action_block_entry::action_block_begin = action_block.begin();
                                                                                action_block_entry::action_block_end = &action_block_inert_begin;
                                                                                #endif
    }

    /// \brief destructor
    /// \details The destructor also deallocates the bunches, as they are not
    /// directly referenced from anywhere.
    ~part_trans_t()
    {
        action_block_iter_t action_block_iter = action_block.begin();
        for (;;)
        {
            for (;;)
            {
                if (action_block_inert_begin <= action_block_iter)  return;
                if (!action_block_iter->succ.is_null())  break;                 assert(action_block_iter->begin_or_before_end.is_null());
                ++action_block_iter;
            }
            bunch_t* const bunch = action_block_iter->succ()->bunch();          assert(bunch->begin == action_block_iter);
            action_block_iter = bunch->end;
            delete bunch;
        }
        // unreachable
    }

    /// \brief correct transition data structure after splitting a bunch
    void first_move_transition_to_new_bunch(action_block_iter_t
                            action_block_iter, bool first_transition_of_state);
    /// \brief correct transition data structure after splitting a bunch
    void second_move_transition_to_new_bunch(
                    action_block_iter_t action_block_iter, bunch_t* new_bunch);

  private:
    /// \brief Adapt the non-inert transitions in an out-slice to a new block
    succ_iter_t move_out_slice_to_new_block(succ_iter_t out_slice_end,
             block_t* old_block, block_bunch_slice_const_iter_t last_splitter);

    /// \brief handle one transition after a block has been split
    /// \details The main task of this method is to move the transition to the
    /// correct place in the action_block slice.
    void first_move_transition_to_new_action_block(pred_iter_t pred_iter);
    /// \brief handle one transition after a block has been split, phase 2
    void second_move_transition_to_new_action_block(pred_iter_t pred_iter);

    /// \brief adapt data structures for a transition that has become non-inert
    bool make_noninert(pred_iter_t old_pred_pos,
         iterator_or_null<block_bunch_slice_iter_t>* new_noninert_block_bunch);

  public:                                                                
    /// \brief Split all data structures after a new block has been created
    /// \details This function splits the block_bunch- and action_block-slices
    /// to reflect that some transitions now start or end in the new block.
    /// They can no longer be in the same slice as the transitions that start
    /// or end in the old block, respectively.  It also marks the transitions
    /// that have become non-inert as such and finds new bottom states.
    ///
    /// Its time complexity is O(1 + |in(NewB)| + |out(NewB)|).
    void adapt_transitions_for_new_block(block_t* new_block,block_t* old_block,
        iterator_or_null<block_bunch_slice_iter_t> new_noninert_block_bunch,
                                block_bunch_slice_const_iter_t last_splitter,
                                         enum new_block_mode_t new_block_mode);
                                                                                #ifndef NDEBUG
                                                                                /// \brief print all transitions
                                                                                /// \details For each state (in order), its outgoing transitions are listed,
                                                                                /// sorted by goal constellation.  The function also indicates where the
                                                                                /// current out-slice pointer of the state points at.
                                                                                template <class LTS_TYPE>
                                                                                void print_trans(const bisim_partitioner_dnj<LTS_TYPE>& partitioner) const;
                                                                                #endif
};


/// \brief find the beginning of the out-slice
inline succ_iter_t succ_entry::out_slice_begin() const
{
    succ_iter_t result = begin_or_before_end();
    if (this < &*result)
    {                                                                           assert(this == &*result->begin_or_before_end());
        result += this - &*result;
    }                                                                           assert(block_bunch->pred->source->succ.begin <= result);
                                                                                // assert(this <= result); //< holds always, based on the if() above
                                                                                assert(this <= &*result->begin_or_before_end());
                                                                                assert(block_bunch->slice() == result->block_bunch->slice());
                                                                                assert(block_bunch->pred->source->succ.begin == result ||
                                                                                                      block_bunch->slice() != result[-1].block_bunch->slice());
    return result;
}


/// \brief find the end of the out-slice
inline succ_iter_t succ_entry::out_slice_before_end() const
{
    succ_iter_t result = begin_or_before_end();
    if (&*result < this)
    {
        result = result->begin_or_before_end();                                 assert(this <= &*result);
    }                                                                           assert(result < block_bunch->pred->source->succ_inert.begin);
                                                                                assert(block_bunch->slice() == result->block_bunch->slice());
                                                                                assert(result + 1 == block_bunch->pred->source->succ_inert.begin ||
                                                                                                       block_bunch->slice() != result[1].block_bunch->slice());
    return result;
}


inline bunch_t* succ_entry::bunch() const
{
    return block_bunch->slice()->bunch();
}


inline bool block_bunch_slice_t::empty() const
{                                                                               assert(block_bunch_begin <= end);  assert(end <= *block_bunch_end);
    return block_bunch_begin == end || this != &*end[-1].slice();
}


inline block_t* block_bunch_slice_t::source_block() const
{                                                                               assert(!empty());
    return end[-1].pred->source->block;
}

///@} (end of group part_trans)
                                                                                #ifndef NDEBUG
                                                                                inline std::string block_bunch_slice_t::debug_id() const
                                                                                {   assert(block_bunch_begin <= end);  assert(end <= *block_bunch_end);
                                                                                    std::string index_string = std::to_string(end - block_bunch_begin);
                                                                                    assert(!empty());  // Probably the following empty case is superfluous.
/* ************************************************************************* */     // if empty()
/*                                                                           */     // {
/*                            A L G O R I T H M S                            */     //     return "empty block_bunch_slice [" + index_string + "," +
/*                                                                           */     //                                                      index_string + ")";
/* ************************************************************************* */     // }
                                                                                    return "block_bunch-slice [_," + index_string +
                                                                                        ") containing transitions from " + source_block()->debug_id() +
                                                                                                                            " in " + bunch()->debug_id_short();
                                                                                }
                                                                                #endif
/// \defgroup part_refine
/// \brief classes to calculate the stutter equivalence quotient of a LTS
///@{



/*=============================================================================
=                                 main class                                  =
=============================================================================*/





enum refine_mode_t { extend_from_state_markings_only,
                     extend_from_state_markings_for_postprocessing,
                     extend_from_FromRed_only,
                     extend_from_bottom_state_markings_and_FromRed };

} // end namespace bisim_dnj

/// \class bisim_partitioner_dnj
/// \brief implements the main algorithm for the stutter equivalence quotient
template <class LTS_TYPE>
class bisim_partitioner_dnj
{
  private:
    LTS_TYPE& aut;
    bisim_dnj::part_state_t part_st;
    bisim_dnj::part_trans_t part_tr;
    bisim_gjkw::fixed_vector<bisim_dnj::iterator_or_counter<
                                bisim_dnj::action_block_iter_t> > action_label;
    bool const branching;
    bool const preserve_divergence;
                                                                                #ifndef NDEBUG
                                                                                friend class bisim_dnj::pred_entry;
                                                                                #endif
  public:
    // The constructor constructs the data structures and immediately
    // calculates the bisimulation quotient.  However, it does not change the
    // LTS.
    bisim_partitioner_dnj(LTS_TYPE& new_aut, bool new_branching = false,
                                          bool new_preserve_divergence = false)
      : aut(new_aut),
        part_st(new_aut.num_states()),
        part_tr(new_aut.num_transitions(), new_aut.num_action_labels()),
        action_label(new_aut.num_action_labels()),
        branching(new_branching),
        preserve_divergence(new_preserve_divergence)
    {                                                                           assert(branching || !preserve_divergence);
      create_initial_partition();
                                                                                #ifndef NDEBUG
                                                                                bisim_dnj::action_block_entry::action_block_orig_inert_begin =
                                                                                                                              part_tr.action_block_inert_begin;
                                                                                #endif
      refine_partition_until_it_becomes_stable();
    }
    ~bisim_partitioner_dnj()  {  }


    /// \brief Adapt the LTS after minimisation
    /// \details After the efficient branching bisimulation minimisation, the
    /// information about the quotient LTS is only stored in the partition data
    /// structure of the partitioner object.  This function exports the
    /// information back to the LTS by adapting its states and transitions:  it
    /// updates the number of states and adds those transitions that are
    /// mandated by the partition data structure.
    ///
    /// The parameter and return value are implicit with this function: a
    /// reference to the LTS was stored in the object by the constructor.
    void replace_transition_system();

    static state_type num_eq_classes()
    {
        return bisim_dnj::block_t::nr_of_blocks;
    }


    /// \brief Get the equivalence class of a state
    /// \details After running the minimisation algorithm, this function
    /// produces the number of the equivalence class of a state.  This number
    /// is the same as the number of the state in the minimised LTS to which
    /// the original state is mapped.
    state_type get_eq_class(state_type s) const
    {
        return part_st.block(s)->seqnr;
    }


    /// \brief Check whether two states are in the same equivalence class
    bool in_same_class(state_type s, state_type t) const
    {
        return part_st.block(s) == part_st.block(t);
    }
  private:

    /*-------- dbStutteringEquivalence -- Algorithm 2 of [GJKW 2017] --------*/

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
    void create_initial_partition();

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
    void refine_partition_until_it_becomes_stable();

    /*----------------- Refine -- Algorithm 3 of [GJKW 2017] ----------------*/

    /// \brief Split a block according to a splitter
    /// \details The function splits `refine_block` into the red part (states
    /// with a transition in `splitter`) and the blue part (states without a
    /// transition in `splitter`).  Depending on `mode`, the states are primed
    /// as follows:
    ///
    /// - If `mode == extend_from_state_markings_only`, then all states with a
    ///   transition must have been marked already.
    /// - If `mode == extend_from_state_markings_for_postprocessing`, states
    ///   are marked as above.  The only difference is the handling of new
    ///   non-inert transitions.
    /// - If `mode == extend_from_FromRed_only`, then no states must be marked;
    ///   the initial states with a transition in `splitter` are searched by
    ///   `refine()` itself.
    /// - If `mode == extend_from_bottom_state_markings_and_FromRed`, then
    ///   bottom states with a transition must have been marked already, but
    ///   there may be non-bottom states that also have a transition, which are
    ///   searched by `refine()`.
    ///
    /// The  function  will  also  adapt  all  data  structures  and  determine
    /// which  transitions  have  changed  from  inert  to  non-inert.   States
    /// with  a  new  non-inert  transition  will  be  marked  upon  returning.
    /// Normally,  the  new  non-inert  transitions  are  moved  to  a  new
    /// bunch,  which  will  be  specially  created.   However,  if  `mode ==
    /// extend_from_state_markings_for_postprocessing`, then the new non-inert
    /// transitions will be added to `splitter` (which must hold transitions
    /// that have just become non-inert before this call to `refine()`).  If
    /// the resulting block contains marked states, the caller has to call
    /// `postprocess_new_noninert()` to stabilise the block because the new
    /// bunch may make the block unstable.
    /// \param refine_block  block that needs to be refined
    /// \param splitter      transition set that makes the block unstable
    /// \param mode          indicates how to find states with a transition in
    ///                      `splitter`, as described above
    /// \returns (a pointer to) the red subblock.  It is an error to call the
    /// function with settings that lead to an empty red subblock.  (An empty
    /// blue subblock is ok.)
    bisim_dnj::block_t* refine(
          bisim_dnj::block_t* const refine_block,
          bisim_dnj::block_bunch_slice_iter_t const splitter,
          enum bisim_dnj::refine_mode_t const mode);

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
    bisim_dnj::block_t* prepare_for_postprocessing(
                            bisim_dnj::block_t* refine_block,
                            bisim_dnj::block_bunch_slice_iter_t last_splitter,
                                                       bool first_preparation);

    /// \brief Split a block with new non-inert transitions as needed
    /// \details The function splits refine_block by stabilising for all
    /// bunches in which it contains transitions.
    ///
    /// When this function starts, it assumes that the states with a new
    /// non-inert transition in refine_block are marked.  It is an error if it
    /// does not contain any marked states.
    ///
    /// The function first calls prepare_for_postprocessing() for refine_block.
    /// Then it walks through all the bunches that can be reached from the
    /// subblock that contains new bottom states to separate it into smaller,
    /// stable subblocks.
    /// \param refine_block   block containing states with new non-inert
    ///                       transitions that need to be stabilised
    /// \param last_splitter  splitter of the last separation before, i. e. the
    ///                       splitter that made these transitions non-inert
    ///                       (refine_block should already be stable w. r. t.
    ///                       last_splitter).
    /// \returns the block containing the old bottom states (and every state in
    ///          refine_block that cannot reach any new non-inert transition)
    bisim_dnj::block_t* postprocess_new_noninert(
                            bisim_dnj::block_t* refine_block,
                            bisim_dnj::block_bunch_slice_iter_t last_splitter);
                                                                                #ifndef NDEBUG
                                                                                /// \brief assert that the data structure is consistent and stable
                                                                                void assert_stability() const;
                                                                                #endif
};

///@} (end of group part_refine)





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

/** \brief Reduce transition system l with respect to strong or (divergence
 *  preserving) branching bisimulation.
 * \param[in,out] l                   The transition system that is reduced.
 * \param         branching           If true branching bisimulation is
 *                                    applied, otherwise strong bisimulation.
 * \param         preserve_divergence Indicates whether loops of internal
 *                                    actions on states must be preserved. If
 *                                    false these are removed. If true these
 *                                    are preserved. */
template <class LTS_TYPE>
void bisimulation_reduce_dnj(LTS_TYPE& l, bool const branching = false,
                                        bool const preserve_divergence = false)
{
  // First, remove tau loops in case of branching bisimulation.
  if (branching)
  {
    scc_reduce(l, preserve_divergence);
  }

  // Second, apply the branching bisimulation reduction algorithm. If there are
  // no taus, this will automatically yield strong bisimulation.
  detail::bisim_partitioner_dnj<LTS_TYPE> bisim_part(l, branching,
                                                          preserve_divergence);

  // Assign the reduced LTS
  bisim_part.replace_transition_system();
}


/** \brief Checks whether the two initial states of two LTSs are strong or
 * branching bisimilar.
 * \details The LTSs l1 and l2 are not usable anymore after this call.
 * The space consumption is O(n) and time is O(m log n). It uses the branching
 * bisimulation algorithm by Groote/Jansen/Keiren/Wijs.
 * \param[in,out] l1                  A first transition system.
 * \param[in,out] l2                  A second transistion system.
 * \param         branching           If true branching bisimulation is used,
 *                                    otherwise strong bisimulation is applied.
 * \param         preserve_divergence If true and branching is true, preserve
 *                                    tau loops on states.
 * \retval True iff the initial states of the current transition system and l2
 * are (divergence preserving) (branching) bisimilar. */
template <class LTS_TYPE>
bool destructive_bisimulation_compare_dnj(LTS_TYPE& l1, LTS_TYPE& l2,
          bool const branching = false, bool const preserve_divergence = false,
                                  bool const generate_counter_examples = false)
{
  if (generate_counter_examples)
  {
    mCRL2log(log::warning) << "The DNJ branching bisimulation algorithm does "
                                             "not generate counterexamples.\n";
  }
  size_t init_l2 = l2.initial_state() + l1.num_states();
  mcrl2::lts::detail::merge(l1, l2);
  l2.clear(); // No use for l2 anymore.

  // First remove tau loops in case of branching bisimulation.
  if (branching)
  {
    detail::scc_partitioner<LTS_TYPE> scc_part(l1);
    scc_part.replace_transition_system(preserve_divergence);
    init_l2 = scc_part.get_eq_class(init_l2);
  }

  detail::bisim_partitioner_dnj<LTS_TYPE> bisim_part(l1, branching,
                                                          preserve_divergence);
  return bisim_part.in_same_class(l1.initial_state(), init_l2);
}


/** \brief Checks whether the two initial states of two LTSs are strong or
 * branching bisimilar.
 * \details The LTSs l1 and l2 are first duplicated and subsequently reduced
 * modulo bisimulation. If memory space is a concern, one could consider to use
 * destructive_bisimulation_compare. This routine uses the O(m log n) branching
 * bisimulation routine. It runs in O(m log n) time and uses O(n) memory, where
 * n is the number of states and m is the number of transitions.
 * \param[in,out] l1                  A first transition system.
 * \param[in,out] l2                  A second transistion system.
 * \param         branching           If true branching bisimulation is used,
 *                                    otherwise strong bisimulation is applied.
 * \param         preserve_divergence If true and branching is true, preserve
 *                                    tau loops on states.
 * \retval True iff the initial states of the current transition system and l2
 * are (divergence preserving) (branching) bisimilar. */
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





/* ************************************************************************* */
/*                                                                           */
/*                       I M P L E M E N T A T I O N S                       */
/*                                                                           */
/* ************************************************************************* */





// This section contains implementations of functions that refer to details of
// classes defined later, so they could not be defined at the point of
// declaration.

namespace bisim_dnj
{

/// \brief quick check to find out whether the state has a transition in `SpBu`
/// \details If the current_out_slice pointer happens to be set to the `SpBu`
/// the function can quickly find out whether the state has a transition in
/// `SpBu`.
/// \param SpBu bunch of interest
/// \returns true if the state is known to have a transition in `SpBu`
/// \memberof state_info_entry
inline bool state_info_entry::surely_has_transition_to(const bunch_t* const
                                                                    SpBu) const
{                                                                               assert(succ.begin <= current_out_slice);
                                                                                assert(current_out_slice <= succ_inert.begin);
                                                                                assert(succ.begin==current_out_slice || current_out_slice==succ_inert.begin ||
                                                                                                 *current_out_slice[-1].bunch() < *current_out_slice->bunch());
    if(current_out_slice<succ_inert.begin && SpBu==current_out_slice->bunch())
    {
        return true;
    }
    // if (succ.begin<current_out_slice && SpBu==current_out_slice[-1].bunch())
    // {
    //     return true;
    // }
    return false;
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
inline bool state_info_entry::surely_has_no_transition_to(const bunch_t* const
                                                                    SpBu) const
{                                                                               assert(succ.begin <= current_out_slice);
                                                                                assert(current_out_slice <= succ_inert.begin);
                                                                                assert(succ.begin==current_out_slice || current_out_slice==succ_inert.begin ||
                                                                                                 *current_out_slice[-1].bunch() < *current_out_slice->bunch());
    if (current_out_slice < succ_inert.begin &&
                                          *current_out_slice->bunch() <= *SpBu)
    {
        return false;
    }
    if (succ.begin < current_out_slice &&
                                       *SpBu <= *current_out_slice[-1].bunch())
    {
        return false;
    }
    return true;
}
                                                                                #ifndef NDEBUG
                                                                                static struct {
                                                                                    bool operator()(iterator_or_counter<action_block_iter_t> p1,
                                                                                                                              action_block_iter_t action_block)
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
                                                                                          partitioner.action_label.begin(), partitioner.action_label.end(),
                                                                                                                      action_block, action_label_greater) -
                                                                                                                              partitioner.action_label.begin();
                                                                                    // class lts_lts_t uses a function pp() to transform the action label to a
                                                                                    // string.
                                                                                    return pp(partitioner.aut.action_label(label)) + "-transition " +
                                                                                                                                              debug_id_short();
                                                                                }
                                                                                #endif
} // end namespace bisim_dnj
} // end namespace detail
} // end namespace lts
} // end namespace mcrl2

#endif // ifndef LIBLTS_BISIM_DNJ_H
