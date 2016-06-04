// Author(s): David N. Jansen, Radboud Universiteit, Nijmegen, The Netherlands
//
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/// \file lts/detail/liblts_bisim_gjkw.h
///
/// \brief O(m log n)-time stuttering equivalence algorithm
///
/// \details This file implements the efficient partition refinement algorithm
/// by Groote / Jansen / Keiren / Wijs to calculate the stuttering equivalence
/// quotient of a Kripke structure.  (Labelled transition systems are converted
/// to Kripke structures before the main algorithm).
/// The file accompanies the planned publication in the ACM Trans. Comput. Log.
/// Log. special issue for TACAS 2016, to appear in 2017.
///
/// \author David N. Jansen, Radboud Universiteit, Nijmegen, The Netherlands

#ifndef _LIBLTS_BISIM_GJKW_H
#define _LIBLTS_BISIM_GJKW_H

#ifndef NDEBUG
    #include <iostream>  // for cout
#endif
#include <cstdlib>       // for size_t
#include <vector>        // for the list of new bottom states
#include <unordered_map> // used during initialisation
#include <cassert>

#include "mcrl2/lts/lts.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/lts/detail/liblts_scc.h"
#include "mcrl2/lts/detail/liblts_merge.h"
#include "mcrl2/lts/detail/coroutine.h"
#include "mcrl2/lts/detail/check_complexity.h"

namespace mcrl2
{
namespace lts
{
namespace detail
{

/// type used to store state numbers and counts
typedef size_t state_type;

/// type used to store transition (numbers and) counts
typedef size_t trans_type;

/// type used to store label numbers and counts
typedef size_t label_type;





/*****************************************************************************/
/*                                                                           */
/*                   R E F I N A B L E   P A R T I T I O N                   */
/*                                                                           */
/*****************************************************************************/





/// \defgroup part_state
/// \brief data structures for a refinable partition
/// \details The following definitions provide a _refinable partition_ data
/// structure.  The basic idea is that we store a permutation of the states (in
/// a permutation_t array), so that states belonging to the same block are
/// adjacent, and also that blocks belonging to the same constellation are
/// adjacent.
///
/// The basic structure therefore consists of the classes:
///
/// permutation_entry - an entry in the permutation array; it contains a
///                     pointer to a state_info_entry.
/// permutation_t     - an array of permutation_entries.
/// constln_t         - contains information about a constellation, in
///                     particular which slice of permutation_t contains its
///                     states.
/// block_t           - contains information about a block, in particular which
///                     slice of permutation_t contains its states, and its
///                     constellation.
/// state_info_entry  - contains information about a state, in particular its
///                     position in the permutation_t array and its block.
/// state_info_t      - an array of state_info_entries.
/// part_state_t      - the complete data structure combining state_info_t and
///                     permutation_t.
///
/// This basic structure is extended as follows:
/// - Because we combine this data structure with a partition of the
///   transitions, a state_info_entry also contains information which slice of
///   the incoming and outgoing, inert and non-inert transitions belong to the
///   state.  In many cases the slice of this state ends exactly where the
///   slice of the next state begins, so we can find the end of this state's
///   slice by looking at the next state's state_info_entry.  Therefore,
///   permutation_entry actually contains a pointer to a state_info_entry with
///   the additional guarantee that this is not the last entry in state_info_t.
///   (To make this a small bit more type safe, we could change the type
///   permutation_entry to something like ``pointer to an array with two
///   state_info_entries'', typedef state_info_entry (*permutation_entry)[2];.
///   Still, that would allow unsafe pointer juggling.)
/// - A block_t also contains information about its outgoing inert transitions.
/// - A state_info_entry also contains information used during trysplit or
///   process_new_bottom.
///@{

namespace bisim_gjkw
{

class state_info_entry;

//typedef state_info_entry* state_info_t;
typedef state_info_entry* permutation_entry;

/// \class permutation_t
/// \brief stores a permutation of the states, ordered by block
/// \details This is the central concept of the _refinable partition_: the
/// permutation of the states, such that states belonging to the same block are
/// adjacent, and also that blocks belonging to the same constellation are
/// adjacent.
///
/// Iterating over the states of a block or the blocks of a constellation will
/// therefore be done using the permutation_t array.
class permutation_t
{
private:
    /// number of states in the Kripke structure
    state_type num_states;

    /// pointer to storage for permutation
    permutation_entry* permutation;
public:
    permutation_t(state_type n)
        :num_states(n),
        permutation(new permutation_entry[n])
    {  }

    ~permutation_t()  {  delete [/* num_states */] permutation;  }

    /// iterator to the first entry in the permutation array
    permutation_entry* begin()  {  return permutation;  }
    const permutation_entry* begin() const  {  return permutation;  }

    /// iterator past the last entry in the permutation array
    permutation_entry* end()  {  return &permutation[num_states];  }
    const permutation_entry* end() const {  return &permutation[num_states];  }

    /// provides the number of states in the Kripke structure
    state_type size() const  {  return num_states;  }
};

class block_t;
class constln_t;

class B_to_C_entry;
class pred_entry;
class succ_entry;

class B_to_C_descriptor;

/// \class state_info_entry
/// \brief stores information about a single state
/// \details This class storesall other information about a state that the
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
    pred_entry* state_in_begin;

    /// \brief iterator to first outgoing transition
    /// \details also serves as iterator past the last outgoing transition of
    /// the previous state.
    succ_entry* state_out_begin;

    /// iterator to first _inert_ incoming transition
    pred_entry* state_inert_in_begin;

    /// iterator to first _inert_ outgoing transition
    succ_entry* state_inert_out_begin;

    /// iterator past the last _inert_ outgoing transition
    succ_entry* state_inert_out_end;

    /// block where the state belongs
    block_t* block;

    /// position of the state in the permutation array
    permutation_entry* pos;

    /// number of inert transitions to non-blue states
    state_type notblue;

    /// iterator to first outgoing transition to the constellation of interest
    succ_entry* current_constln;

    /// get constellation where the state belongs
    constln_t* get_constln() const;

    /// iterator to first incoming transition
    pred_entry* in_begin()           {  return state_in_begin;  }

    /// iterator past the last incoming transition
    pred_entry* in_end()             {  return this[1].state_in_begin;  }

    /// iterator to first non-inert incoming transition
    pred_entry* noninert_in_begin()  {  return state_in_begin;  }

    /// iterator past the last non-inert incoming transition
    pred_entry* noninert_in_end()    {  return state_inert_in_begin;  }

    /// iterator to first inert incoming transition
    pred_entry* inert_in_begin()     {  return state_inert_in_begin;  }

    /// iterator one past the last inert incoming transition
    const pred_entry* inert_in_end() const {  return this[1].state_in_begin;  }

    /// iterator to first outgoing transition
    succ_entry* out_begin()  {  return state_out_begin;  }
    const succ_entry* out_begin() const   {  return state_out_begin;  }

    /// iterator past the last outgoing transition
    const succ_entry* out_end() const     {  return this[1].state_out_begin;  }

    /// iterator to first inert outgoing transition
    succ_entry* inert_out_begin()   {  return state_inert_out_begin;  }

    /// iterator past the last inert outgoing transition
    succ_entry* inert_out_end()     {  return state_inert_out_end  ;  }

    bool surely_has_transition_to(const constln_t* SpC) const;
    bool surely_has_no_transition_to(const constln_t* SpC) const;
};


/// swap two permutations
static inline void swap_permutation(permutation_entry*s1, permutation_entry*s2)
{
    // swap contents of permutation array
    std::swap(*s1, *s2);
    // swap pointers to permutation array
    (*s1)->pos = s1;
    (*s2)->pos = s2;
}

/// \brief swap three permutations
/// \details the entry in the permutation array at *s1 moves to *s2, the entry
/// at *s2 moves to *s3, and the entry at *s3 moves to *s1.
static inline void swap3_permutation(permutation_entry* s1,
                                permutation_entry* s2, permutation_entry* s3)
{
    // swap contents of permutation array
    permutation_entry temp = *s1;
    *s1 = *s3;
    *s3 = *s2;
    *s2 = temp;
    // swap pointers to permutation array
    (*s1)->pos = s1;
    (*s2)->pos = s2;
    (*s3)->pos = s3;
}


/// \class block_t
/// \brief stores information about a block
/// \details A block corresponds to a slice of the permutation array.  As the
/// number of blocks is initially unknown, we will allocate instances of this
/// class dynamically.
///
/// The slice in the permutation array containing the states of the block is
/// subdivided into the following subslices (in this order):
/// 1. unmarked nonbottom states
/// 2. marked nonbottom states (initially empty)
/// 3. unmarked bottom states
/// 4. marked bottom states (initially empty)
/// 5. old bottom states (only nonempty during PostprocessNewBottom)
///
/// A state should be marked iff it is a predecessor of the current splitter
/// (through a strong transition).  The marking is later extended to the red
/// states;  that are the states with a weak transition to the splitter.
///
/// In PostprocessNewBottom, all bottom states that already existed before the
/// refinement have a strong transition to the splitter;  therefore, there is
/// no need to separate them further, and this fact is recorded by moving them
/// to the slice of _old_ bottom states.
///
/// (During the execution of some functions, more slices are subdivided
/// further;  however, as these subdivisions are local to a single function,
/// they are not stored here.)
class block_t
{
private:
    /// iterator past the last state of the block
    permutation_entry* int_end;

    /// iterator to the first state of the block
    permutation_entry* int_begin;
public:
    /// iterator to the first marked nonbottom state of the block
    permutation_entry* int_marked_nonbottom_begin;

    /// iterator to the first bottom state of the block
    permutation_entry* int_bottom_begin;

    /// iterator to the first marked bottom state of the block
    permutation_entry* int_marked_bottom_begin;

    /// iterator to the first old bottom state of the block
    permutation_entry* int_old_bottom_begin;

    /// \brief iterator to the first inert transition of the block
    /// \details If there are no inert transitions, then `inert_begin` and
    /// `inert_end` point to the end of the B_to_C-slice containing transitions
    /// from the block to its own constellation.  If there is no such slice,
    /// both are equal to `B_to_C`.
    B_to_C_entry* int_inert_begin;

    /// iterator past the last inert transition of the block
    B_to_C_entry* int_inert_end;

    /// iterator to the first transition from the block to the splitter
    B_to_C_descriptor* FromRed;

    /// constellation to which the block belongs
    constln_t* int_constln;
private:
    /// \brief next block in the list of refinable blocks
    /// \details If this is the last block in the list, `refinable_next` points
    /// to this very block.  Consequently, it is possible to check whether some
    /// block is refinable without an additional variable.
    block_t* refinable_next;

    /// first block in the list of refinable blocks
    static block_t* refinable_first;

    //friend class constln_t;
public:
    /// \brief constructor
    /// \details The constructor initialises the block to: all states are
    /// bottom states, no state is marked, the block is not refinable.
    /// \param constln_ constellation to which the block belongs
    /// \param begin_   initial iterator to the first state of the block
    /// \param end_     initial iterator past the last state of the block
    block_t(constln_t* constln_, permutation_entry* begin_,
                                                    permutation_entry* end_)
        :int_end(end_),
        int_begin(begin_),
        int_marked_nonbottom_begin(begin_), // no nonbottom state is marked
        int_bottom_begin(begin_), // all states are bottom states
        int_marked_bottom_begin(end_), // no bottom state is marked
        int_old_bottom_begin(end_), // there are no old bottom states
        int_inert_begin(), // is initialised by part_trans_t::create_new_block
        int_inert_end(), // is initialised by part_trans_t::create_new_block
        FromRed(NULL),
        int_constln(constln_),
        refinable_next(NULL)
    {  }

    ~block_t()  {  }

    /// provides an arbitrary refinable block
    static block_t* get_some_refinable()  {  return refinable_first;  }

    /// checks whether the block is refinable
    /// \returns true if the block is refinable
    bool is_refinable() const  {  return NULL != refinable_next;  }

    /// makes a block refinable (i. e. inserts it into the respective list)
    /// \returns true if the block was not refinable before
    bool make_refinable()
    {
        if (is_refinable())
        {
            return false;
        }
        refinable_next = NULL == refinable_first ? this : refinable_first;
        refinable_first = this;
        return true;
    }

    /// \brief makes a block non-refinable (i. e. removes it from the
    /// respective list)
    /// \details This member function only works if the block is the first one
    /// in the list (which will normally be the case).
    void make_nonrefinable()
    {
        assert(refinable_first == this);
        refinable_first = refinable_next == this ? NULL : refinable_next;
        refinable_next = NULL;
    }

    /// provides the number of states in the block
    state_type size() const  {  return int_end - int_begin;  }

    /// \brief provides the number of marked bottom states in the block
    /// \details This size includes the old bottom states.
    state_type marked_bottom_size() const
    {
        return marked_bottom_end() - marked_bottom_begin();
    }

    /// \brief provides the number of marked states in the block
    /// \details This size includes the old bottom states;  in other words, the
    /// old bottom states are always regarded as marked.
    state_type marked_size() const
    {
        return marked_nonbottom_end() - marked_nonbottom_begin() +
                                                        marked_bottom_size();
    }

    /// provides the number of unmarked bottom states in the block
    state_type unmarked_bottom_size() const
    {
        return unmarked_bottom_end() - unmarked_bottom_begin();
    }

    /// \brief compares two blocks for ordering them
    /// \details The blocks are ordered according to their positions in the
    /// permutation array.  This is a suitable order, as blocks may be refined,
    /// but never swap positions as a whole.  Refining will make the new
    /// subblocks compare in the same way to other blocks as the original,
    /// larger block.
    bool operator<(const block_t& other) const
    {
        return int_begin < other.int_begin;
    }

    /// constellation where the block belongs to
    constln_t* get_constln()  {  return int_constln;  }
    const constln_t* get_constln() const  {  return int_constln;  }

    /// iterator to the first state in the block
    permutation_entry* begin()  {  return int_begin;  }

    /// iterator past the last state in the block
    const permutation_entry* end() const  {  return int_end;  }
    permutation_entry* end()  {  return int_end;  }

    /// iterator to the first nonbottom state in the block
    const permutation_entry* nonbottom_begin() const  { return int_begin; }
    permutation_entry* nonbottom_begin()  { return int_begin; }

    /// iterator past the last nonbottom state in the block
    const permutation_entry* nonbottom_end() const { return int_bottom_begin; }
    permutation_entry* nonbottom_end()  {  return int_bottom_begin;  }

    /// iterator to the first bottom state in the block
    const permutation_entry* bottom_begin() const  { return int_bottom_begin; }
    permutation_entry* bottom_begin()  {  return int_bottom_begin;  }

    /// iterator past the last bottom state in the block
    const permutation_entry* bottom_end() const  {  return int_end;  }
    permutation_entry* bottom_end()  {  return int_end;  }

    /// iterator to the first unmarked nonbottom state in the block
    permutation_entry* unmarked_nonbottom_begin()
    {
        return int_begin;
    }

    /// iterator past the last unmarked nonbottom state in the block
    permutation_entry* unmarked_nonbottom_end()
    {
        return int_marked_nonbottom_begin;
    }

    /// iterator to the first marked nonbottom state in the block
    const permutation_entry* marked_nonbottom_begin() const
    {
        return int_marked_nonbottom_begin;
    }
    permutation_entry* marked_nonbottom_begin()
    {
        return int_marked_nonbottom_begin;
    }

    /// iterator one past the last marked nonbottom state in the block
    const permutation_entry* marked_nonbottom_end() const
    {
        return int_bottom_begin;
    }
    permutation_entry* marked_nonbottom_end()
    {
        return int_bottom_begin;
    }

    /// iterator to the first unmarked bottom state in the block
    const permutation_entry* unmarked_bottom_begin() const
    {
        return int_bottom_begin;
    }
    permutation_entry* unmarked_bottom_begin()  {  return int_bottom_begin;  }

    /// iterator past the last unmarked bottom state in the block
    const permutation_entry* unmarked_bottom_end() const
    {
        return int_marked_bottom_begin;
    }
    permutation_entry* unmarked_bottom_end()
    {
        return int_marked_bottom_begin;
    }

    /// iterator to the first marked bottom state in the block
    const permutation_entry* marked_bottom_begin() const
    {
        return int_marked_bottom_begin;
    }
    permutation_entry* marked_bottom_begin()
    {
        return int_marked_bottom_begin;
    }

    /// \brief iterator past the last marked bottom state in the block
    /// \details This includes the old bottom states.
    const permutation_entry* marked_bottom_end() const
    {
        return int_end;
    }
    permutation_entry* marked_bottom_end()
    {
        return int_end;
    }

    /// iterator to the first bottom state that is not old
    permutation_entry* non_old_bottom_begin()
    {
        return int_bottom_begin;
    }

    /// iterator past the last bottom state that is not old
    permutation_entry* non_old_bottom_end()
    {
        return int_old_bottom_begin;
    }

    /// iterator to the first marked bottom state that is not old
    permutation_entry* non_old_marked_bottom_begin()
    {
        return int_marked_bottom_begin;
    }

    /// iterator past the last marked bottom state that is not old
    permutation_entry* non_old_marked_bottom_end()
    {
        return int_old_bottom_begin;
    }

    /// iterator to the first old bottom state in the block
    permutation_entry* old_bottom_begin()  {  return int_old_bottom_begin;  }

    /// iterator past the last old bottom state in the block
    permutation_entry* old_bottom_end()  {  return int_end;  }

    /// iterator to the first inert transition of the block
    B_to_C_entry* inert_begin()  {  return int_inert_begin;  }

    /// iterator past the last inert transition of the block
    B_to_C_entry* inert_end()  {  return int_inert_end;  }

private:
    static const char* const mark_all_states_in_SpB;
public:
    /// \brief mark all states in the block
    /// \details This function is used to mark all states of the splitter.
    void mark_all_states()
    {
        assert(marked_nonbottom_begin() == marked_nonbottom_end() &&
                                marked_bottom_begin() == marked_bottom_end());
        check_complexity::count(mark_all_states_in_SpB, size(),
                                                check_complexity::n_log_n);
        int_marked_nonbottom_begin = nonbottom_begin();
        int_marked_bottom_begin = bottom_begin();
    }

    /// unmark all states in the block
    void unmark_all_states()
    {
        int_marked_nonbottom_begin = marked_nonbottom_end();
        int_marked_bottom_begin = marked_bottom_end();
        // int_old_bottom_begin = old_bottom_end();
    }

    /// \brief mark a nonbottom state
    /// \details Marking is done by moving the state to the slice of the marked
    /// nonbottom states of the block.
    /// \param s the nonbottom state that has to be marked
    /// \returns true if the state was not marked before
    bool mark_nonbottom(state_info_entry* s)
    {
        assert(s->pos < nonbottom_end() && nonbottom_begin() <= s->pos);

        if (marked_nonbottom_begin() <= s->pos)  return false;
        --int_marked_nonbottom_begin;
        swap_permutation(s->pos, marked_nonbottom_begin());
        return true;
    }

    /// \brief mark a state
    /// \details Marking is done by moving the state to the slice of the marked
    /// bottom or nonbottom states of the block.  If `s` is an old bottom
    /// state, it is treated as if it already were marked.
    /// \param s the state that has to be marked
    /// \returns true if the state was not marked before
    bool mark(state_info_entry* s)
    {
        assert(s->pos < end());

        if (bottom_begin() <= s->pos)
        {
            if (marked_bottom_begin() <= s->pos)  return false;
            --int_marked_bottom_begin;
            swap_permutation(s->pos, marked_bottom_begin());
            return true;
        }
        return mark_nonbottom(s);
    }

    /// \brief refine the block (the blue subblock is smaller)
    /// \details This function is called after a refinement function has found
    /// that the blue subblock is the smaller one.  It creates a new block for
    /// the blue states.
    /// \param blue_nonbottom_end iterator past the last blue nonbottom state
    /// \returns pointer to the new (blue) block
    block_t* split_off_blue(permutation_entry* blue_nonbottom_end);

    /// \brief refine the block (the red subblock is smaller)
    /// \details This function is called after a refinement function has found
    /// that the red subblock is the smaller one.  It creates a new block for
    /// the red states.
    /// \param red_nonbottom_begin iterator to the first red nonbottom state
    /// \returns pointer to the new (red) block
    block_t* split_off_red(permutation_entry* red_nonbottom_begin);
};

// The following member function can only be defined after the full class
// definition of block_t because it refer to members of that class.

/// get the constellation of the state
inline constln_t* state_info_entry::get_constln() const
{
    return block->get_constln();
}


/// \class constln_t
/// \brief stores information about a constellation
/// \details A constellation corresponds to a slice in the permutation array;
/// its boundaries are also block boundaries.  As the number of constellations
/// is initially unknown, we will allocate it dynamically.
///
/// The constellations keep track of the total number of constellations
/// allocated and number themselves sequentially using the static member
/// `nr_of_constlns`.  It is therefore impossible to have multiple refinements
/// running at the same time.
class constln_t
{
private:
    /// iterator past the last state in the constellation
    permutation_entry* int_end;

    /// iterator to the first state in the constellation
    permutation_entry* int_begin;

    /// \brief next constellation in the list of nontrivial constellations
    /// \details If this is the last constellation in the list,
    /// `nontrivial_next` points to this very constellation.  Consequently, it
    /// is possible to check whether some constellation is trivial without an
    /// additional variable.
    constln_t* nontrivial_next;

    /// first constellation in the list of nontrivial constellations
    static constln_t* nontrivial_first;
public:
    /// \brief unique sequence number of this constellation
    /// \details After the stuttering equivalence algorithm has terminated,
    /// this number is used as a state number in the quotient Kripke structure.
    state_type seqnr;

    /// \brief total number of constellations allocated
    /// \details Upon starting the stuttering equivalence algorithm, the number
    /// of constellations must be zero.
    static state_type nr_of_constlns;

    /// \brief iterator to the first transition into this constellation that
    /// needs postprocessing
    /// \details In `PostprocessNewBottom()`, all transitions from a refined
    /// block to the present constellation have to be gone through.  Because
    /// during this process the refined block may be refined even further, we
    /// need `postprocess_begin` and `postprocess_end` to store which
    /// transitions have to be gone through overall.
    B_to_C_entry* postprocess_begin;

    /// \brief iterator past the last transition into this constellation that
    /// needs postprocessing
    B_to_C_entry* postprocess_end;

    /// \brief constructor
    /// \param begin_ iterator to the first state in the constellation
    /// \param end_   iterator past the last state in the constellation
    constln_t(permutation_entry* begin_, permutation_entry* end_)
        :int_end(end_),
        int_begin(begin_),
        nontrivial_next(NULL),
        seqnr(nr_of_constlns++)
    {  }

    /// destructor
    ~constln_t()  {  }

    /// provides an arbitrary nontrivial constellation
    static constln_t* get_some_nontrivial()  {  return nontrivial_first;  }

    /// \brief makes a constellation trivial (i. e. removes it from the
    /// respective list)
    /// \details This member function only works if the constellation is the
    /// first one in the list (which will normally be the case).
    void make_trivial()
    {
        assert(nontrivial_first == this);
        nontrivial_first = nontrivial_next == this ? NULL : nontrivial_next;
        nontrivial_next = NULL;
    }

    /// \brief makes a constellation nontrivial (i. e. inserts it into the
    /// respective list)
    void make_nontrivial()
    {
        if (NULL == nontrivial_next)
        {
            nontrivial_next = NULL==nontrivial_first ? this : nontrivial_first;
            nontrivial_first = this;
        }
    }

    /// iterator to the first state in the constellation
    permutation_entry* begin()  {  return int_begin;  }
    const permutation_entry* begin() const  {  return int_begin;  }

    /// iterator past the last state in the constellation
    permutation_entry* end()  {  return int_end;  }
    const permutation_entry* end() const  {  return int_end;  }

    /// \brief compares two constellations for ordering them
    /// \details The constellations are ordered according to their positions in
    /// the permutation array.  This is a suitable order, as constellations may
    /// be refined, but never swap positions as a whole.  Refining will make
    /// the new subconstellations compare in the same way to other
    /// constellations as the original, larger constellation.
    bool operator<(const constln_t& other) const
    {
        return *begin() < *other.begin();
    }

    /// \brief split off a single block from the constellation
    /// \details The function splits the current constellation after its first
    /// block or before its last block, whichever is smaller.  It creates a new
    /// constellation for the split-off block and returns a pointer to the
    /// block.
    block_t* split_off_small_block()
    {
        block_t* FirstB = (*begin())->block, * LastB = int_end[-1]->block;
        assert(FirstB != LastB);
        if (FirstB->end() == LastB->begin())  make_trivial();

        assert(this == FirstB->get_constln());
        assert(this == LastB->get_constln());
        constln_t* NewC = new constln_t(begin(), end());

        /// It doesn't matter very much how ties are resolved here:
        /// `part_tr.change_to_C()` is faster if the first block is selected to
        /// be split off.  `part_tr.split_s_inert_out()` is faster if the last
        /// block is selected.
        if (FirstB->size() > LastB->size())
        {
            NewC->int_begin = int_end = LastB->begin();
            LastB->int_constln = NewC;
            return LastB;
        }
        else
        {
            NewC->int_end = int_begin = FirstB->end();
            FirstB->int_constln = NewC;
            return FirstB;
        }
    }
};



template <class LTS_TYPE>
class bisim_partitioner_gjkw_initialise_helper;

/// \class part_state_t
/// \brief refinable partition data structure
/// \details This class collects all information about a partition of the
/// states.
class part_state_t
{
private:
    /// permutation array
    permutation_t permutation;

    /// \brief pointer to array with all other information about states
    /// \details We allocate 1 additional ``state'' to allow for the iterators
    /// past the last transition, as described in the documentation of
    /// `state_info_entry`.
    state_info_entry* state_info;

    template <class LTS_TYPE>
    friend class bisim_partitioner_gjkw_initialise_helper;
public:
    /// \brief constructor
    /// \details The constructor allocates memory, but does not actually
    /// initialise the partition.
    /// \param n number of states in the Kripke structure
    part_state_t(state_type n)
        :permutation(n),
        state_info(new state_info_entry[n+1]) // 1 additional ``state''
    {
        assert(0 == constln_t::nr_of_constlns);
    }

    /// destructor
    ~part_state_t()
    {
        // the destructor deallocates constellations and blocks.
        // We have to deallocate constellations first because deallocating
        // blocks makes the constellations inaccessible.
        #ifndef NDEBUG
            state_type deleted_constlns = 0;
        #endif
        permutation_entry* permutation_iter = permutation.end();
        while (permutation.begin() != permutation_iter)
        {
            constln_t* C = permutation_iter[-1]->get_constln();
            assert(C->end() == permutation_iter);
            permutation_iter = C->begin();
            delete C;
            #ifndef NDEBUG
                ++deleted_constlns;
            #endif
        }
        assert(deleted_constlns == constln_t::nr_of_constlns);
        constln_t::nr_of_constlns = 0;

        permutation_iter = permutation.end();
        while (permutation.begin() != permutation_iter)
        {
            block_t* B = permutation_iter[-1]->block;
            assert(B->end() == permutation_iter);
            permutation_iter = B->begin();
            delete B;
        }
        delete [/* size() */] state_info;
    }

    /// provide stored number of states
    state_type size() const  {  return permutation.size();  }

    /// find constellation of a state (identified by number)
    const constln_t* get_constln(state_type s) const
    {
        return state_info[s].get_constln();
    }

#ifndef NDEBUG

private:
    void print_block(const char* message, const block_t* B,
            const permutation_entry* begin, const permutation_entry* end) const
    {
        if (end - begin != 0)
        {
            std::cout << "\t\t" << message << (end-begin > 1 ? "s:\n" : ":\n");
            do
            {
                std::cout << "\t\t\tState "
                                << (state_type) (begin - permutation.begin());
                if (B != (*begin)->block)
                {
                    std::cout << ", inconsistent: points to block at "
                                << static_cast<const void*>((*begin)->block);
                }
                std::cout << "\n";
            }
            while (++begin != end);
        }
    }
public:
    /// print the partition as a tree (per constellation and block)
    void print() const
    {
        const constln_t* C = (*permutation.begin())->get_constln();
        for ( ;; )
        {
            std::cout << "Constellation " << C->seqnr << ":\n";
            const block_t* B = (*C->begin())->block;
            for ( ;; )
            {
                std::cout << "\tBlock at " << static_cast<const void*>(B);
                if (C != B->get_constln())
                {
                    std::cout << ", inconsistent: points to constellation "
                                                    << B->get_constln()->seqnr;
                }
                std::cout << ":\n";
                print_block("Nonbottom state", B, B->nonbottom_begin(),
                                                        B->nonbottom_end());
                print_block("Bottom state", B, B->bottom_begin(),
                                                        B->bottom_end());
                // go to next block
                if (C->end() == B->end())  break;
                B = (*B->end())->block;
            }
            // go to next constellation
            if (permutation.end() == C->end())  break;
            C = (*C->end())->get_constln();
        }
    }

#endif // ifndef NDEBUG

};

///@} (end of group part_state)





/*****************************************************************************/
/*                                                                           */
/*                           T R A N S I T I O N S                           */
/*                                                                           */
/*****************************************************************************/





/// \defgroup part_trans
/// \brief data structures for transitions used during partition refinement
/// \details These definitions provide a partition for transition data
/// structure that can be used for the partition refinement algorithm.
///
/// Basically, transitions are stored in three arrays:
/// - `pred`: transitions ordered by goal state, to allow finding all
///   predecessors of a goal state.
/// - `succ`: transitions ordered by source state and goal constellation, to
///   allow finding all successors of a source state.  Given a transition in
///   this array, it is easy to find all transitions from the same source state
///   to the same goal constellation.  It is possible to find out, in time
///   logarithmic in the out-degree, whether a state has a transition to a
///   given constellation.
/// - `B_to_C`: a permutation of the transitions such that transitions from the
///    same source block to the same goal constellation are adjacent.  Further,
///    this array does not need a specific sort order.
///
/// Within this sort order, inert transitions are always placed after non-inert
/// transitions.
///
/// state_info_entry and block_t (defined above) contain pointers to the slices
/// of these arrays.  For the incoming transitions, they contain enough
/// information; for the outgoing and the B_to_C-transitions, we additionally
/// use so-called _descriptors_ that show which slice belongs together.

///@{

class out_descriptor;
class B_to_C_descriptor;

/* pred_entry, succ_entry, and B_to_C_entry contain the data that is stored
about a transition.  Every transition has one of each data structure; the three
structures are linked through the iterators (used here as pointers). */
class pred_entry
{
public:
    succ_entry* succ;
    permutation_entry source;
};


class succ_entry
{
public:
    B_to_C_entry* B_to_C;
    permutation_entry target;
    out_descriptor* constln_slice;

    friend class part_trans_t;
};


class B_to_C_entry
{
public:
    pred_entry* pred;
    B_to_C_descriptor* slice;

    friend class part_trans_t;
};


/* out_descriptor and B_to_C_descriptor are data types that indicate which
slice of states belongs together. */
class out_descriptor
{
public:
    succ_entry* end, * begin;

    out_descriptor(succ_entry* iter)  :end(iter), begin(iter)  {  }
    state_type size() const  {  return end - begin;  }
};


class B_to_C_descriptor
{
public:
    B_to_C_entry* end, * begin;

    B_to_C_descriptor(B_to_C_entry* begin_, B_to_C_entry* end_)
        :end(end_),
        begin(begin_)
    {  }
};


/* part_trans_t collects and organises all data for the transitions. */
class part_trans_t
{
private:
    trans_type nr_of_transitions;
    pred_entry* pred;
    succ_entry* succ;
    B_to_C_entry* B_to_C;

    template <class LTS_TYPE>
    friend class bisim_partitioner_gjkw_initialise_helper;

    void swap_in(B_to_C_entry* pos1, B_to_C_entry* pos2)
    {
        // swap contents
        pred_entry temp_entry(*pos1->pred);
        *pos1->pred = *pos2->pred;
        *pos2->pred = temp_entry;
        // swap pointers to contents
        pred_entry* temp_iter(pos1->pred);
        pos1->pred = pos2->pred;
        pos2->pred = temp_iter;
    }

    void swap_out(pred_entry* pos1, pred_entry* pos2)
    {
        // swap contents
        succ_entry temp_entry(*pos1->succ);
        *pos1->succ = *pos2->succ;
        *pos2->succ = temp_entry;
        // swap pointers to contents
        succ_entry* temp_iter(pos1->succ);
        pos1->succ = pos2->succ;
        pos2->succ = temp_iter;
    }

    // *pos1 -> *pos2 -> *pos3 -> *pos1
    void swap3_out(pred_entry* pos1, pred_entry* pos2, pred_entry* pos3)
    {
        assert(pos1 != pos2 || pos1 == pos3);
        // swap contents
        succ_entry temp_entry(*pos1->succ);
        *pos1->succ = *pos3->succ;
        *pos3->succ = *pos2->succ;
        *pos2->succ = temp_entry;
        // swap pointers to contents
        succ_entry* temp_iter(pos1->succ);
        pos1->succ = pos3->succ;
        pos3->succ = pos2->succ;
        pos2->succ = temp_iter;
    }

    void swap_B_to_C(succ_entry* pos1, succ_entry* pos2)
    {
        // swap contents
        B_to_C_entry temp_entry(*pos1->B_to_C);
        *pos1->B_to_C = *pos2->B_to_C;
        *pos2->B_to_C = temp_entry;
        // swap pointers to contents
        B_to_C_entry* temp_iter(pos1->B_to_C);
        pos1->B_to_C = pos2->B_to_C;
        pos2->B_to_C = temp_iter;
    }

    // *pos1 -> *pos2 -> *pos3 -> *pos1
    void swap3_B_to_C(succ_entry* pos1, succ_entry* pos2, succ_entry* pos3)
    {
        assert(pos1 != pos2 || pos1 == pos3);
        // swap contents
        B_to_C_entry temp_entry(*pos1->B_to_C);
        *pos1->B_to_C = *pos3->B_to_C;
        *pos3->B_to_C = *pos2->B_to_C;
        *pos2->B_to_C = temp_entry;
        // swap pointers to contents
        B_to_C_entry* temp_iter(pos1->B_to_C);
        pos1->B_to_C = pos3->B_to_C;
        pos3->B_to_C = pos2->B_to_C;
        pos2->B_to_C = temp_iter;
    }
public:
    part_trans_t(trans_type m)
        :nr_of_transitions(m),
        pred(new pred_entry[m]),
        succ(new succ_entry[m]),
        B_to_C(new B_to_C_entry[m])
    {  }
    ~part_trans_t()
    {
        // deallocate the descriptors
        for (B_to_C_entry* B_to_C_iter = &B_to_C[nr_of_transitions];
                                                    B_to_C != B_to_C_iter; )
        {
            B_to_C_descriptor* desc = B_to_C_iter[-1].slice;
            B_to_C_iter = desc->begin;
            delete desc;
        }
        for (succ_entry* succ_iter = &succ[nr_of_transitions];
                                                        succ != succ_iter; )
        {
            out_descriptor* desc = succ_iter[-1].constln_slice;
            succ_iter = desc->begin;
            delete desc;
        }
        delete [/* nr_of_transitions */] B_to_C;
        delete [/* nr_of_transitions */] succ;
        delete [/* nr_of_transitions */] pred;
    }

    trans_type size() const  {  return nr_of_transitions;  }

    /* split_inert_to_C splits the B_to_C slice of block b to its own
    constellation into two slices: one for the inert and one for the non-inert
    transitions.  It is called with SpB just after a constellation is split, as
    the transitions from SpB to itself (= the inert transitions) now go to a
    different constellation than the other transitions from SpB to its old
    constellation.  It does, however, not adapt the other transition arrays to
    reflect that noninert and inert transitions from block b would go to
    different constellations.
    Its time complexity is O(1+min {|out_noninert(b-->C)|, |out_inert(b)|}). */
    void split_inert_to_C(block_t* B);

    /* part_trans_t::change_to_C has to be called after a transition target has
    changed its constellation.  The member function will adapt the transition
    data structure.  It assumes that the transition is non-inert and that the
    new constellation does not (yet) have inert incoming transitions.  It
    returns the boundary between transitions to OldC and transitions to NewC in
    the state's outgoing transition array. */
    succ_entry* change_to_C(pred_entry*pred_iter,constln_t*OldC,constln_t*NewC,
            bool first_transition_of_state, bool first_transition_of_block);

    /* split_s_inert_out splits the outgoing transitions from s to its own
    constellation into two:  the inert transitions become transitions to the
    new constellation of which s is now part;  the non-inert transitions remain
    transitions to OldC.  It returns the boundary between transitions to
    OldC and transitions to NewC in the outgoing transition array of s.
    Its time complexity is O(1 + min { |out_\nottau(s)|, |out_\tau(s)| }). */
    succ_entry* split_s_inert_out(state_info_entry* s, constln_t* OldC);

    /* part_trans_t::make_noninert makes the transition identified by succ_iter
    noninert. */
    void make_noninert(succ_entry* succ_iter)
    {
        const succ_entry* other_succ = succ_iter->B_to_C->pred->source->
                                                    state_inert_out_begin++;
        const B_to_C_entry* other_B_to_C = succ_iter->B_to_C->pred->source->
                                                    block->int_inert_begin++;
        const pred_entry* other_pred=succ_iter->target->state_inert_in_begin++;
        // change B_to_C
        assert(succ_iter->B_to_C->slice->begin <= other_B_to_C);
        swap_B_to_C(succ_iter, other_B_to_C->pred->succ);
        // change pred
        swap_in(succ_iter->B_to_C, other_pred->succ->B_to_C);
        // change succ
        swap_out(succ_iter->B_to_C->pred, other_succ->B_to_C->pred);
    }

    /* part_trans_t::new_block_created splits the B_to_C-slices to reflect that
    some transitions now start in the new block NewB.  They can no longer be in
    the same slice as the transitions that start in the old block.
    Its time complexity is O(1 + |out(NewB)|). */
    void new_block_created(block_t* OldB, block_t* NewB);
};

// The following functions can only be implemented after the definition of
// succ_entry was given fully.

/// \brief quick check to find out whether the state has a transition to `SpC`
/// \details If the current constellation pointer happens to be set to `SpC` or
/// its successor, the function can quickly find out whether the state has a
/// transition to `SpC`.
/// \param SpC constellation of interest
/// \returns true if the state is known to have a transition to `SpC`
/// \memberof state_info_entry
inline bool state_info_entry::surely_has_transition_to(const constln_t* SpC)
                                                                        const
{
    // either current_constln->target or current_constln[-1].target is in SpC
    if (current_constln != out_end() &&
                            current_constln->target->get_constln() == SpC)
        return true;
    if (current_constln != out_begin() &&
                            current_constln[-1].target->get_constln() == SpC)
        return true;
    return false;
}

/// \brief quick check to find out whether the state has _no_ transition to
/// `SpC`
/// \details If the current constellation pointer happens to be set to `SpC` or
/// its successor, the function can quickly find out whether the state has a
/// transition to `SpC`.
/// \param SpC constellation of interest
/// \returns true if the state is known to have _no_ transition to `SpC`
/// \memberof state_info_entry
inline bool state_info_entry::surely_has_no_transition_to(const constln_t* SpC)
                                                                        const
{
    // condition:
    // current_constln->target is in a constellation > SpC and
    // current_constln[-1].target is in a constellation < SpC.
    if (current_constln != out_end() &&
                        current_constln->target->get_constln() <= SpC)
        return false;
    if (current_constln != out_begin() &&
                        current_constln[-1].target->get_constln() >= SpC)
        return false;
    return true;
}

///@} (end of group part_trans)






/*****************************************************************************/
/*                                                                           */
/*                            A L G O R I T H M S                            */
/*                                                                           */
/*****************************************************************************/





/// \defgroup part_refine
/// \brief classes to calculate the stutter equivalence quotient of a LTS
///@{



/*=============================================================================
=                create initial partition and data structures                 =
=============================================================================*/



/// \class bisim_partitioner_gjkw_initialise_helper
/// \brief helps with initialising the refinable partition data structure
/// \details Before allocating memory for the refinable partition data
/// structure, the number of states and transitions (including extra states
/// generated by the translation from labelled transition system to Kripke
/// structure) has to be known.  This class serves to calculate these numbers.
///
/// The helper class also initialises the variables used by check_complexity to
/// find the number of states and transitions in time complexity checks.
template<class LTS_TYPE>
class bisim_partitioner_gjkw_initialise_helper
{
private:
    LTS_TYPE& aut;
    state_type nr_of_states, orig_nr_of_states;
    trans_type nr_of_transitions;

    // key and hash function for (action, target state) pair. Required since
    // unordered_map does not directly allow to use pair keys
    class Key 
    {
    public:
        label_type first;
        state_type second;

        Key(const label_type& f, const state_type& s)
            : first(f),
            second(s)
        {} 

        bool operator==(const Key &other) const 
        {
            return (first == other.first && second == other.second);
        }
    };

    class KeyHasher 
    {
    public:
        std::size_t operator()(const Key& k) const
        {
            return std::hash<label_type>()(k.first) ^
                                    (std::hash<state_type>()(k.second) << 1);
        }
    };
    // Map used to convert LTS to Kripke structure
    // (also used when converting Kripke structure back to LTS)
    std::unordered_map<Key, state_type, KeyHasher> extra_kripke_states;

    // temporary map to keep track of blocks. maps transition labels (different
    // from tau) to blocks
    std::unordered_map<label_type, state_type> action_block_map;

    std::vector<state_type> noninert_out_per_state, inert_out_per_state;
    std::vector<state_type> noninert_in_per_state, inert_in_per_state;
    std::vector<state_type> noninert_out_per_block, inert_out_per_block;
    std::vector<state_type> states_per_block;
    state_type nr_of_nonbottom_states;
public:
    bisim_partitioner_gjkw_initialise_helper(LTS_TYPE& l, bool branching,
                                                    bool preserve_divergence);

    /// initialise the state in part_st and the transitions in part_tr
    void init_transitions(part_state_t& part_st, part_trans_t& part_tr,
                                    bool branching, bool preserve_divergence);

    // replace_transitions() replaces the transitions of the LTS stored here by
    // those of its bisimulation quotient.  However, it does not change
    // anything else; in particular, it does not change the number of states of
    // the LTS.
    void replace_transitions(const part_state_t& part_st, bool branching,
                                                    bool preserve_divergence);

    /// provides the number of states in the Kripke structure
    state_type get_nr_of_states() const  {  return nr_of_states;  }

    /// provides the number of transitions in the Kripke structure
    trans_type get_nr_of_transitions() const  {  return nr_of_transitions;  }
};



/*=============================================================================
=                                 main class                                  =
=============================================================================*/



struct secondary_refine_shared;

} // end namespace bisim_gjkw

/// \class bisim_partitioner_gjkw
/// \brief implements the main algorithm for the stutter equivalence quotient
template <class LTS_TYPE>
class bisim_partitioner_gjkw
{
private:
    bisim_gjkw::bisim_partitioner_gjkw_initialise_helper<LTS_TYPE> init_helper;
    bisim_gjkw::part_state_t part_st;
    bisim_gjkw::part_trans_t part_tr;
    std::vector<bisim_gjkw::state_info_entry*> new_bottom_states;
public:
    // The constructor constructs the data structures and immediately
    // calculates the bisimulation quotient.  However, it does not change the
    // LTS.
    bisim_partitioner_gjkw(LTS_TYPE& l, bool branching = false,
                                        bool preserve_divergence = false)
        :init_helper(l, branching, preserve_divergence),
        part_st(init_helper.get_nr_of_states()),
        part_tr(init_helper.get_nr_of_transitions())
    {
        assert(branching || !preserve_divergence);
        create_initial_partition_gjkw(branching, preserve_divergence);
        refine_partition_until_it_becomes_stable_gjkw();
    }
    ~bisim_partitioner_gjkw()  {  }

    // replace_transitions() replaces the transitions of the LTS stored here by
    // those of its bisimulation quotient.  However, it does not change
    // anything else; in particular, it does not change the number of states of
    // the LTS.
    void replace_transitions(bool branching, bool preserve_divergence)
    {
        init_helper.replace_transitions(part_st,branching,preserve_divergence);
    }

    static state_type num_eq_classes()
    {
        return bisim_gjkw::constln_t::nr_of_constlns;
    }
    state_type get_eq_class(state_type s) const
    {
        return part_st.get_constln(s)->seqnr;
    }
    bool in_same_class(state_type s, state_type t) const
    {
        return part_st.get_constln(s) == part_st.get_constln(t);
    }
private:

    /*-------- dbStutteringEquivalence -- Algorithm 2 of [GJKW 2017] --------*/

    void create_initial_partition_gjkw(bool branching,
                                                    bool preserve_divergence);
    void refine_partition_until_it_becomes_stable_gjkw();

    /*------------- PrimaryRefine -- Algorithm 3 of [GJKW 2017] -------------*/

    bisim_gjkw::block_t* primary_refine(bisim_gjkw::block_t* RefB);

    DECLARE_COROUTINE(primary_blue,
    /* formal parameters:   */ (bisim_gjkw::block_t*, RefB),
    /* local variables:     */ (bisim_gjkw::permutation_entry*, visited_end,
                                bisim_gjkw::state_info_entry*, s,
                                bisim_gjkw::pred_entry*, pred_iter,
                                bisim_gjkw::permutation_entry*,
                                                        blue_nonbottom_end),
    /* shared data:         */ bisim_gjkw::permutation_entry*,
                                                    notblue_initialised_end,
    /* interrupt locations: */ (PRIMARY_BLUE_PREDECESSOR_HANDLED,
                                PRIMARY_BLUE_STATE_HANDLED));

    DECLARE_COROUTINE(primary_red,
    /* formal parameters:   */ (bisim_gjkw::block_t*, RefB),
    /* local variables:     */ (bisim_gjkw::permutation_entry*, visited_begin,
                                bisim_gjkw::state_info_entry*, s,
                                bisim_gjkw::pred_entry*, pred_iter),
    /* shared data:         */ bisim_gjkw::permutation_entry*,
                                                    notblue_initialised_end,
    /* interrupt locations: */ (PRIMARY_RED_PREDECESSOR_HANDLED,
                                PRIMARY_RED_STATE_HANDLED));

    /*------------ SecondaryRefine -- Algorithm 4 of [GJKW 2017] ------------*/

    bisim_gjkw::block_t* secondary_refine(bisim_gjkw::block_t* RefB,
                                const bisim_gjkw::constln_t* SpC,
                                const bisim_gjkw::B_to_C_descriptor* FromRed);

    DECLARE_COROUTINE(secondary_blue,
    /* formal parameters:   */ (bisim_gjkw::block_t*, RefB,
                                const bisim_gjkw::constln_t*, SpC),
    /* local variables:     */ (bisim_gjkw::permutation_entry*, visited_end,
                                bisim_gjkw::state_info_entry*, s,
                                bisim_gjkw::pred_entry*, pred_iter,
                                bisim_gjkw::state_info_entry*, s_prime,
                                bisim_gjkw::permutation_entry*,
                                                            blue_nonbottom_end,
                                const bisim_gjkw::succ_entry*, begin,
                                const bisim_gjkw::succ_entry*, end),
    /* shared data:         */ struct bisim_gjkw::secondary_refine_shared,
                                                                shared_data,
    /* interrupt locations: */ (SECONDARY_BLUE_PREDECESSOR_HANDLED,
                                SECONDARY_BLUE_TESTING,
                                SECONDARY_BLUE_STATE_HANDLED));

    DECLARE_COROUTINE(secondary_red,
    /* formal parameters:   */ (bisim_gjkw::block_t*, RefB,
                                const bisim_gjkw::B_to_C_descriptor*, FromRed),
    /* local variables:     */ (bisim_gjkw::B_to_C_entry*,
                                                        fromred_visited_begin,
                                bisim_gjkw::permutation_entry*, visited_begin,
                                bisim_gjkw::state_info_entry*, s,
                                bisim_gjkw::pred_entry*, pred_iter),
    /* shared data:         */ struct bisim_gjkw::secondary_refine_shared,
                                                                shared_data,
    /* interrupt locations: */ (SECONDARY_RED_COLLECT_FROMRED,
                                SECONDARY_RED_PREDECESSOR_HANDLED,
                                SECONDARY_RED_STATE_HANDLED));

    /*--------- PostprocessNewBottom -- Algorithm 5 of [GJKW 2017] ----------*/

    void postprocess_new_bottom();
};

///@} (end of group part_refine)





/*****************************************************************************/
/*                                                                           */
/*                             I N T E R F A C E                             */
/*                                                                           */
/*****************************************************************************/





/// \defgroup part_interface
/// \brief nonmember functions serving as interface with the rest of mCRL2
/// \details These functions are copied, almost without changes, from
/// liblts_bisim_gw.h.
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
void bisimulation_reduce_gjkw(LTS_TYPE& l, bool branching = false,
                                            bool preserve_divergence = false);

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
bool destructive_bisimulation_compare_gjkw(LTS_TYPE& l1, LTS_TYPE& l2,
                    bool branching = false, bool preserve_divergence = false,
                                    bool generate_counter_examples = false);

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
bool bisimulation_compare_gjkw(const LTS_TYPE& l1, const LTS_TYPE& l2,
                    bool branching = false, bool preserve_divergence = false);

/// calculates the bisimulation quotient of a LTS.
template <class LTS_TYPE>
void bisimulation_reduce_gjkw(LTS_TYPE& l, bool branching /* = false */,
                                        bool preserve_divergence /* = false */)
{
  // First, remove tau loops in case of branching bisimulation.
  if (branching)
  {
    scc_reduce(l, preserve_divergence);
  }

  // Second, apply the branching bisimulation reduction algorithm. If there are
  // no taus, this will automatically yield strong bisimulation.
  detail::bisim_partitioner_gjkw<LTS_TYPE> bisim_part(l, branching,
                                                        preserve_divergence);

  // Clear the state labels of the LTS l
  l.clear_state_labels();

  // Assign the reduced LTS
  l.set_initial_state(bisim_part.get_eq_class(l.initial_state()));
  bisim_part.replace_transitions(branching, preserve_divergence);
  l.set_num_states(bisim_part.num_eq_classes());
}

template <class LTS_TYPE>
inline bool bisimulation_compare_gjkw(const LTS_TYPE& l1, const LTS_TYPE& l2,
        bool branching /* = false */, bool preserve_divergence /* = false */)
{
  LTS_TYPE l1_copy(l1);
  LTS_TYPE l2_copy(l2);
  return destructive_bisimulation_compare_gjkw(l1_copy, l2_copy, branching,
                                                        preserve_divergence);
}

template <class LTS_TYPE>
bool destructive_bisimulation_compare_gjkw(LTS_TYPE& l1, LTS_TYPE& l2,
        bool branching /* = false */, bool preserve_divergence /* = false */,
                                bool generate_counter_examples /* = false */)
{
  if (generate_counter_examples)
  {
    mCRL2log(log::warning) << "The GJKW branching bisimulation algorithm does "
                                            "not generate counterexamples.\n";
  }
  size_t init_l2 = l2.initial_state() + l1.num_states();
  mcrl2::lts::detail::merge(l1, l2);
  l2.clear(); // No use for l2 anymore.

  // First remove tau loops in case of branching bisimulation.
  if (branching)
  {
    detail::scc_partitioner<LTS_TYPE> scc_part(l1);
    scc_part.replace_transitions(preserve_divergence);
    l1.set_num_states(scc_part.num_eq_classes());
    l1.set_initial_state(scc_part.get_eq_class(l1.initial_state()));
    init_l2 = scc_part.get_eq_class(init_l2);
  }

  detail::bisim_partitioner_gjkw<LTS_TYPE> bisim_part(l1, branching,
                                                        preserve_divergence);
  return bisim_part.in_same_class(l1.initial_state(), init_l2);
}

///@} (end of group part_interface)

} // end namespace detail
} // end namespace lts
} // end namespace mcrl2

#endif // ifndef _LIBLTS_BISIM_GJKW_H
