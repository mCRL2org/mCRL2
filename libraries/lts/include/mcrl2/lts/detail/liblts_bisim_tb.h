// Author(s): David N. Jansen, Radboud Universiteit, Nijmegen, The Netherlands
//
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/// \file lts/detail/liblts_bisim_tb.h
///
/// \brief O(m log n)-time branching bisimulation algorithm
///
/// \details This file implements an efficient partition refinement algorithm
/// for labelled transition systems inspired by Groote / Jansen / Keiren / Wijs
/// and Valmari (2009) to calculate the branching bisimulation classes of a
/// labelled transition system.
///
/// \author David N. Jansen, Institute of Software, Chinese Academy of
/// Sciences, Beijing, China

#ifndef _LIBLTS_BRANCHING_BISIM_H
#define _LIBLTS_BRANCHING_BISIM_H

#include <list>  // for the list of B_a_B_slice_ts
#include <forward_list>  // for the list of B_a_B_slice_ts

#include "mcrl2/lts/detail/liblts_scc.h"
#include "mcrl2/lts/detail/liblts_merge.h"
#include "mcrl2/lts/detail/coroutine.h"
#include "mcrl2/lts/detail/check_complexity.h"
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

/// \brief type used to store label numbers and counts
typedef std::size_t label_type;





/* ************************************************************************* */
/*                                                                           */
/*                   R E F I N A B L E   P A R T I T I O N                   */
/*                                                                           */
/* ************************************************************************* */





namespace bisim_tb
{

class state_info_entry;

typedef state_info_entry* state_info_ptr;
typedef const state_info_entry* state_info_const_ptr;

/// \class permutation_t
/// \brief stores a permutation of the states, ordered by block
/// \details This is the central concept of the _refinable partition_: the
/// permutation of the states, such that states belonging to the same block are
/// adjacent, and also that blocks belonging to the same constellation are
/// adjacent.
///
/// Iterating over the states of a block or the blocks of a constellation will
/// therefore be done using the permutation_t array.
typedef bisim_gjkw::fixed_vector<state_info_ptr> permutation_t;
typedef permutation_t::iterator permutation_iter_t;
typedef permutation_t::const_iterator permutation_const_iter_t;

class block_t;
class bunch_t;

class B_a_B_entry;
class pred_entry;
class succ_entry;
typedef bisim_gjkw::fixed_vector<B_a_B_entry>::iterator B_a_B_iter_t;
typedef bisim_gjkw::fixed_vector<pred_entry>::iterator pred_iter_t;
typedef bisim_gjkw::fixed_vector<succ_entry>::iterator succ_iter_t;

typedef bisim_gjkw::fixed_vector<B_a_B_entry>::const_iterator B_a_B_const_iter_t;
typedef bisim_gjkw::fixed_vector<pred_entry>::const_iterator pred_const_iter_t;
typedef bisim_gjkw::fixed_vector<succ_entry>::const_iterator succ_const_iter_t;

class B_a_B_slice_t;
typedef std::list<B_a_B_slice_t>::iterator B_a_B_desc_iter_t;
typedef std::list<B_a_B_slice_t>::const_iterator B_a_B_desc_const_iter_t;

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
  private:
    /// \brief iterator to first incoming transition
    /// \details also serves as iterator past the last incoming transition of
    /// the previous state.
    pred_iter_t state_in_begin;

    /// \brief iterator to first outgoing transition
    /// \details also serves as iterator past the last outgoing transition of
    /// the previous state.
    succ_iter_t state_out_begin;
  public:
    union UI {
        /// number of incoming transitions (used during initialisation)
        trans_type count;
        /// iterator to first _inert_ incoming transition
        pred_iter_t inert_begin;

        // must specify a default constructor for this union because class
        // pred_iter_t has a non-trivial default constructor.
        UI()  {  count = 0;  }
    } state_in;

    union UO {
        /// number of outgoing transitions (used during initialisation)
        trans_type count;
        /// iterator to first _inert_ outgoing transition
        succ_iter_t inert_begin;

        UO()  {  count = 0;  }
    } state_out;

    /// block where the state belongs
    block_t* block;

    /// position of the state in the permutation array
    permutation_iter_t pos;

    /// number of inert transitions to non-blue states
    state_type notblue;

    /// iterator to first outgoing transition to the bunch of interest
    succ_iter_t current_bunch;

    /// iterator to first incoming transition
    pred_const_iter_t pred_begin() const  {  return state_in_begin;  }
    pred_iter_t pred_begin()  {  return state_in_begin;  }
    void set_pred_begin(pred_iter_t new_in_begin)
    {
        state_in_begin = new_in_begin;
    }

    /// iterator past the last incoming transition
    pred_const_iter_t pred_end() const
    {
        assert(s_i_begin <= this);
        assert(this < s_i_end);
        return this[1].state_in_begin;
    }
    pred_iter_t pred_end()
    {
        assert(s_i_begin <= this);
        assert(this < s_i_end);
        return this[1].state_in_begin;
    }
    void set_pred_end(pred_iter_t new_in_end)
    {
        // assert(s_i_begin <= this); -- will be checked in pred_end()
        // assert(this < s_i_end);
        this[1].set_pred_begin(new_in_end);
        assert (inert_pred_begin() <= pred_end());
    }

    /// iterator to first non-inert incoming transition
    pred_const_iter_t noninert_pred_begin() const  {  return state_in_begin;  }
    pred_iter_t noninert_pred_begin()  {  return state_in_begin;  }

    /// iterator past the last non-inert incoming transition
    pred_const_iter_t noninert_pred_end() const  { return inert_pred_begin(); }
    pred_iter_t noninert_pred_end()  {  return inert_pred_begin();  }

    /// iterator to first inert incoming transition
    pred_const_iter_t inert_pred_begin() const { return state_in.inert_begin; }
    pred_iter_t inert_pred_begin()  {  return state_in.inert_begin;  }
    void set_inert_pred_begin(pred_iter_t new_inert_in_begin)
    {
        state_in.inert_begin = new_inert_in_begin;
        assert(pred_begin() <= inert_pred_begin());
        // assert(inert_pred_begin() <= pred_end());
    }

    /// iterator one past the last inert incoming transition
    pred_const_iter_t inert_pred_end() const { return pred_end(); }
    pred_iter_t inert_pred_end()  {  return pred_end();  }

    /// iterator to first outgoing transition
    succ_const_iter_t succ_begin() const  {  return state_out_begin;  }
    succ_iter_t succ_begin()  {  return state_out_begin;  }
    void set_succ_begin(succ_iter_t new_out_begin)
    {
        state_out_begin = new_out_begin;
    }

    /// iterator past the last outgoing transition
    succ_const_iter_t succ_end() const
    {
        assert(s_i_begin <= this);
        assert(this < s_i_end);
        return this[1].state_out_begin;
    }
    succ_iter_t succ_end()
    {
        assert(s_i_begin <= this);
        assert(this < s_i_end);
        return this[1].state_out_begin;
    }
    void set_succ_end(succ_iter_t new_out_end)
    {
        // assert(s_i_begin <= this); -- will be checked in succ_end()
        // assert(this < s_i_end);
        this[1].set_succ_begin(new_out_end);
        assert(inert_succ_begin() <= succ_end());
    }

    /// iterator to first non-inert outgoing transition
    succ_const_iter_t noninert_succ_begin() const  {  return succ_begin();  }
    succ_iter_t noninert_succ_begin()  {  return succ_begin();  }

    /// iterator past the last non-inert outgoing transition
    succ_const_iter_t noninert_succ_end() const  { return inert_succ_begin(); }
    succ_iter_t noninert_succ_end()  {  return inert_succ_begin();  }

    /// iterator to first inert outgoing transition
    succ_const_iter_t inert_succ_begin() const  {return state_out.inert_begin;}
    succ_iter_t inert_succ_begin()  {  return state_out.inert_begin;  }
    void set_inert_succ_begin(succ_iter_t const new_inert_out_begin)
    {
        // The following assertions cannot be tested because the respective
        // types are not yet complete.
        // if (new_inert_out_begin > inert_succ_begin())
        // {
        //     assert(new_inert_out_begin[-1].target->block != block);
        // }
        // else if (new_inert_out_begin < inert_succ_begin())
        // {
        //     assert(new_inert_out_begin->target->block == block);
        // }
        state_out.inert_begin = new_inert_out_begin;
        assert(succ_begin() <= inert_succ_begin());
        // assert(inert_succ_begin() <= inert_succ_end());
    }

    /// iterator past the last inert outgoing transition
    succ_const_iter_t inert_succ_end() const  {  return succ_end();  }
    succ_iter_t inert_succ_end()  {  return succ_end();  }

    bool surely_has_transition_to(const bunch_t* SpBu) const;
    bool surely_has_no_transition_to(const bunch_t* SpBu, const bunch_t* NewBu)
                                                                         const;

#ifndef NDEBUG
    /// \brief print a short state identification for debugging
    /// \details This function is only available if compiled in Debug mode.
    std::string debug_id_short() const
    {
        assert(s_i_begin <= this);
        assert(this < s_i_end);
        return std::to_string(this - s_i_begin);
    }

    /// \brief print a state identification for debugging
    /// \details This function is only available if compiled in Debug mode.
    std::string debug_id() const
    {
        return "state " + debug_id_short();
    }

  private:
    /// \brief pointer at the first entry in the `state_info` array
    static state_info_const_ptr s_i_begin;
    /// \brief pointer past the last actual entry in the `state_info` array
    /// \details `state_info` actually contains an additional entry that is
    /// only used partially, namely to store pointers to the end of the
    /// transition slices of the last state.  In other words, `s_i_end` points
    /// at this additional, partially used entry.
    static state_info_const_ptr s_i_end;

    friend class part_state_t;
  public:
    mutable bisim_gjkw::check_complexity::state_counter_t work_counter;
#endif
};


/// swap two permutations
static inline
void swap_permutation(permutation_iter_t s1, permutation_iter_t s2)
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
static inline void swap3_permutation(permutation_iter_t s1,
                                  permutation_iter_t s2, permutation_iter_t s3)
{
    assert(s1 != s2 || s1 == s3);
    // swap contents of permutation array
    state_info_ptr temp = *s1;
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
/// 1. unmarked non-bottom states
/// 2. marked non-bottom states (initially empty)
/// 3. unmarked bottom states
/// 4. marked bottom states (initially empty)
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
  private:
    /// iterator past the last state of the block
    permutation_iter_t int_end;

    /// iterator to the first state of the block
    permutation_iter_t int_begin;

    /// iterator to the first marked non-bottom state of the block
    permutation_iter_t int_marked_nonbottom_begin;

    /// iterator to the first bottom state of the block
    permutation_iter_t int_bottom_begin;

    /// iterator to the first marked bottom state of the block
    permutation_iter_t int_marked_bottom_begin;

  public:
    /// \brief list of B_a_B slices containing transitions starting in this
    /// block
    /// \details Every std::list<B_a_B_slice_t> describes the transitions of
    /// one bunch.  Every B_a_B_slice_t describes the transitions of one
    /// B_a_B_slice.  Perhaps the inner list can be replaced by a circular list
    /// (a kind of forward list where the last element points back to the first
    /// one, so one can find the first element when one has passed by the last
    /// one).
    /// Note that this list of lists has to be defined before int_inert_slice.
    ///
    /// When a bunch is split into two, one of the B_a_B_slice_t entries is
    /// moved to a new list.  Which one it is depends on the physical position
    /// of the slices in the bunch.
    ///
    /// When a block is split into two, every B_a_B_slice with incoming
    /// transitions is also split into two;  the slices obviously get related
    /// positions.
    std::list<B_a_B_slice_t> from_block;

  private:
    /// \brief pointer to the slice containing the inert transition of the
    /// block
    /// \details If there are no inert transitions, the field is
    /// `from_block->end()`.  Therefore, `from_block` has to be defined before
    /// this field.
    B_a_B_desc_iter_t int_inert_slice;

    /// \brief next block in the list of refinable blocks
    /// \details If this is the last block in the list, `refinable_next` points
    /// to this very block.  Consequently, it is possible to check whether some
    /// block is refinable without an additional variable.
    block_t* postprocessing_this_bunch_next;

    /// first block in the list of refinable blocks
    static block_t* postprocessing_this_bunch_first;

  public:
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
    /// bottom states, no state is marked, the block is not refinable.
    /// \param constln_ constellation to which the block belongs
    /// \param begin_   initial iterator to the first state of the block
    /// \param end_     initial iterator past the last state of the block
    block_t(permutation_iter_t const begin_, permutation_iter_t const end_)
      : int_end(end_),
        int_begin(begin_),
        int_marked_nonbottom_begin(begin_), // no non-bottom state is marked
        int_bottom_begin(begin_), // all states are bottom states
        int_marked_bottom_begin(end_), // no bottom state is marked
        from_block(),
        // int_inert_slice -- is initialised by part_trans_t::create_new_block
        postprocessing_this_bunch_next(nullptr),
        seqnr(nr_of_blocks++)
    {
        // The following assertions hold trivially.
        // assert(int_begin <= int_marked_nonbottom_begin);
        // assert(int_marked_nonbottom_begin <= int_bottom_begin);
        // assert(int_bottom_begin <= int_marked_bottom_begin);
        // assert(int_marked_bottom_begin <= int_end);
        assert(int_bottom_begin < int_end);
        // The following assertions cannot be tested because constln_t is not
        // yet complete.
        // assert(int_constln->begin() <= int_begin &&
        //                                      int_end <= int_constln->end());
    }

    ~block_t()  {  }

    /// provides an arbitrary refinable block
    static block_t* get_some_postprocessing_this_bunch()
    {
        return postprocessing_this_bunch_first;
    }

    /// \brief checks whether the block is refinable
    /// \returns true if the block is refinable
    bool needs_postprocessing_this_bunch() const
    {
        return nullptr != postprocessing_this_bunch_next;
    }

    /// \brief makes a block refinable (i. e. inserts it into the respective
    /// list)
    /// \returns true if the block was not refinable before
    bool make_postprocessing_this_bunch()
    {
        if (needs_postprocessing_this_bunch())
        {
            return false;
        }
        postprocessing_this_bunch_next=nullptr==postprocessing_this_bunch_first
                                             ? this
                                             : postprocessing_this_bunch_first;
        postprocessing_this_bunch_first = this;
        return true;
    }

    /// \brief makes a block non-refinable (i. e. removes it from the
    /// respective list)
    /// \details This member function only works if the block is the first one
    /// in the list (which will normally be the case).
    void make_non_postprocessing_this_bunch()
    {
        assert(postprocessing_this_bunch_first == this);
        postprocessing_this_bunch_first = postprocessing_this_bunch_next==this
                                              ? nullptr
                                              : postprocessing_this_bunch_next;
        postprocessing_this_bunch_next = nullptr;
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
//    bool operator<(const block_t& other) const
//    {
//        return begin() < other.begin();
//    }

    /// iterator to the first state in the block
    permutation_const_iter_t begin()  const  {  return int_begin;  }
    permutation_iter_t begin()  {  return int_begin;  }
    void set_begin(permutation_iter_t new_begin)
    {
        int_begin = new_begin;
        assert(int_begin <= int_marked_nonbottom_begin);
    }

    /// iterator past the last state in the block
    permutation_const_iter_t end()  const  {  return int_end;  }
    permutation_iter_t end()  {  return int_end;  }
    void set_end(permutation_iter_t new_end)
    {
        int_end = new_end;
        assert(int_marked_bottom_begin <= int_end);
        assert(int_bottom_begin < int_end);
    }

    /// iterator to the first non-bottom state in the block
    permutation_const_iter_t nonbottom_begin()  const  {  return int_begin;  }
    permutation_iter_t nonbottom_begin()  {  return int_begin;  }

    /// iterator past the last non-bottom state in the block
    permutation_const_iter_t nonbottom_end() const { return int_bottom_begin; }
    permutation_iter_t nonbottom_end()  {  return int_bottom_begin;  }
    void set_nonbottom_end(permutation_iter_t new_nonbottom_end)
    {
        int_bottom_begin = new_nonbottom_end;
        assert(int_marked_nonbottom_begin <= int_bottom_begin);
        assert(int_bottom_begin <= int_marked_bottom_begin);
        assert(int_bottom_begin < int_end);
    }

    /// iterator to the first bottom state in the block
    permutation_const_iter_t bottom_begin() const  { return int_bottom_begin; }
    permutation_iter_t bottom_begin()  {  return int_bottom_begin;  }
    void set_bottom_begin(permutation_iter_t new_bottom_begin)
    {
        int_bottom_begin = new_bottom_begin;
        assert(int_marked_nonbottom_begin <= int_bottom_begin);
        assert(int_bottom_begin <= int_marked_bottom_begin);
        // assert(int_bottom_begin < int_end);
    }

    /// iterator past the last bottom state in the block
    permutation_const_iter_t bottom_end() const  {  return int_end;  }
    permutation_iter_t bottom_end()  {  return int_end;  }

    /// iterator to the first unmarked non-bottom state in the block
    permutation_const_iter_t unmarked_nonbottom_begin()const{return int_begin;}
    permutation_iter_t unmarked_nonbottom_begin()  {  return int_begin;  }

    /// iterator past the last unmarked non-bottom state in the block
    permutation_const_iter_t unmarked_nonbottom_end() const
    {
        return int_marked_nonbottom_begin;
    }
    permutation_iter_t unmarked_nonbottom_end()
    {
        return int_marked_nonbottom_begin;
    }
    void set_unmarked_nonbottom_end(permutation_iter_t
                                                    new_unmarked_nonbottom_end)
    {
        int_marked_nonbottom_begin = new_unmarked_nonbottom_end;
        assert(int_begin <= int_marked_nonbottom_begin);
        assert(int_marked_nonbottom_begin <= int_bottom_begin);
    }

    /// iterator to the first marked non-bottom state in the block
    permutation_const_iter_t marked_nonbottom_begin() const
    {
        return int_marked_nonbottom_begin;
    }
    permutation_iter_t marked_nonbottom_begin()
    {
        return int_marked_nonbottom_begin;
    }
    void set_marked_nonbottom_begin(permutation_iter_t
                                                    new_marked_nonbottom_begin)
    {
        int_marked_nonbottom_begin = new_marked_nonbottom_begin;
        assert(int_begin <= int_marked_nonbottom_begin);
        assert(int_marked_nonbottom_begin <= int_bottom_begin);
    }

    /// iterator one past the last marked non-bottom state in the block
    permutation_const_iter_t marked_nonbottom_end() const
    {
        return int_bottom_begin;
    }
    permutation_iter_t marked_nonbottom_end()  {  return int_bottom_begin;  }

    /// iterator to the first unmarked bottom state in the block
    permutation_const_iter_t unmarked_bottom_begin() const
    {
        return int_bottom_begin;
    }
    permutation_iter_t unmarked_bottom_begin()  {  return int_bottom_begin;  }

    /// iterator past the last unmarked bottom state in the block
    permutation_const_iter_t unmarked_bottom_end() const
    {
        return int_marked_bottom_begin;
    }
    permutation_iter_t unmarked_bottom_end()  {return int_marked_bottom_begin;}
    void set_unmarked_bottom_end(permutation_iter_t new_unmarked_bottom_end)
    {
        int_marked_bottom_begin = new_unmarked_bottom_end;
        assert(int_bottom_begin <= int_marked_bottom_begin);
        assert(int_marked_bottom_begin <= int_end);
    }

    /// iterator to the first marked bottom state in the block
    permutation_const_iter_t marked_bottom_begin() const
    {
        return int_marked_bottom_begin;
    }
    permutation_iter_t marked_bottom_begin()  {return int_marked_bottom_begin;}
    void set_marked_bottom_begin(permutation_iter_t new_marked_bottom_begin)
    {
        int_marked_bottom_begin = new_marked_bottom_begin;
        assert(int_bottom_begin <= int_marked_bottom_begin);
        assert(int_marked_bottom_begin <= int_end);
    }

    /// \brief iterator past the last marked bottom state in the block
    /// \details This includes the old bottom states.
    permutation_const_iter_t marked_bottom_end() const  {  return int_end;  }
    permutation_iter_t marked_bottom_end()  {  return int_end;  }

    const B_a_B_desc_iter_t inert_slice() const  {  return int_inert_slice;  }
    B_a_B_desc_iter_t inert_slice()  {  return int_inert_slice;  }
    void set_inert_slice(B_a_B_desc_iter_t new_inert_slice)
    {
        int_inert_slice = new_inert_slice;
    }

    /// \brief mark all states in the block
    /// \details This function is used to mark all states of the splitter.
    void mark_all_states();

    /// \brief mark a non-bottom state
    /// \details Marking is done by moving the state to the slice of the marked
    /// non-bottom states of the block.
    /// \param s the non-bottom state that has to be marked
    /// \returns true if the state was not marked before
    bool mark_nonbottom(state_info_ptr s)
    {
        assert(s->pos < nonbottom_end() && nonbottom_begin() <= s->pos);

        if (marked_nonbottom_begin() <= s->pos)  return false;
        set_marked_nonbottom_begin(marked_nonbottom_begin() - 1);
        swap_permutation(s->pos, marked_nonbottom_begin());
        return true;
    }

    /// \brief mark a state
    /// \details Marking is done by moving the state to the slice of the marked
    /// bottom or non-bottom states of the block.  If `s` is an old bottom
    /// state, it is treated as if it already were marked.
    /// \param s the state that has to be marked
    /// \returns true if the state was not marked before
    bool mark(state_info_ptr s)
    {
        assert(s->pos < end());

        if (bottom_begin() <= s->pos)
        {
            if (marked_bottom_begin() <= s->pos)  return false;
            set_marked_bottom_begin(marked_bottom_begin() - 1);
            swap_permutation(s->pos, marked_bottom_begin());
            return true;
        }
        return mark_nonbottom(s);
    }

    /// \brief refine the block (the blue subblock is smaller)
    /// \details This function is called after a refinement function has found
    /// that the blue subblock is the smaller one.  It creates a new block for
    /// the blue states.
    /// \param blue_nonbottom_end iterator past the last blue non-bottom state
    /// \returns pointer to the new (blue) block
    block_t* split_off_blue(permutation_iter_t blue_nonbottom_end);

    /// \brief refine the block (the red subblock is smaller)
    /// \details This function is called after a refinement function has found
    /// that the red subblock is the smaller one.  It creates a new block for
    /// the red states.
    /// \param red_nonbottom_begin iterator to the first red non-bottom state
    /// \returns pointer to the new (red) block
    block_t* split_off_red(permutation_iter_t red_nonbottom_begin);

#ifndef NDEBUG
    /// \brief print a block identification for debugging
    /// \details This function is only available if compiled in Debug mode.
    std::string debug_id() const
    {
        return "block [" + std::to_string(begin() - perm_begin) + "," +
                                          std::to_string(end() - perm_begin) +
                                          ") (#" + std::to_string(seqnr) + ")";
    }

    /// \brief provide an iterator to the beginning of the permutation array
    /// \details This iterator is required to be able to print identifications
    /// for debugging.  It is only available if compiled in Debug mode.
    static permutation_const_iter_t permutation_begin()  { return perm_begin; }

    mutable bisim_gjkw::check_complexity::block_counter_t work_counter;
  private:
    static permutation_const_iter_t perm_begin;

    friend class part_state_t;
#endif
};


/// \brief mark all states in the block
/// \details This function is used to mark all states of the splitter.
inline void block_t::mark_all_states()
{
    assert(marked_nonbottom_begin() == marked_nonbottom_end() &&
                                 marked_bottom_begin() == marked_bottom_end());
// In onderstaande regel heb ik uitgecommentarieerde want Mark_all_states_of_SpB_as_predecessors_2_9
// is nergens gedefinieerd, en deze code lijkt niet te compileren in maintainer mode. 
//    mCRL2complexity(this, add_work(
//                   bisim_gjkw::check_complexity::Mark_all_states_of_SpB_as_predecessors_2_9,
//                   bisim_gjkw::check_complexity::log_n - bisim_gjkw::check_complexity::ilog2(size())));
    set_marked_nonbottom_begin(nonbottom_begin());
    // set_marked_nonbottom_end(nonbottom_end());
    set_marked_bottom_begin(bottom_begin());
    // set_marked_bottom_end(bottom_end());
}



class part_trans_t;

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
    /// elements, and blocks that belong to the same constellation are stored
    /// in adjacent slices.
    permutation_t permutation;

    /// \brief array with all other information about states
    /// \details We allocate 1 additional ``state'' to allow for the iterators
    /// past the last transition, as described in the documentation of
    /// `state_info_entry`.
    bisim_gjkw::fixed_vector<state_info_entry> state_info;

    /// \brief constructor
    /// \details The constructor allocates memory, but does not actually
    /// initialise the partition.  Immediately afterwards, the initialisation
    /// helper `bisim_partitioner_tb_initialise_helper::init_transitions()`
    /// should be called.
    /// \param n number of states in the Kripke structure
    part_state_t(state_type n)
      : permutation(n),
        state_info(n+1) //< an additional ``state'' is needed to store pointers
            // to the end of the slices of transitions of the last state
    {
        assert(0 == block_t::nr_of_blocks);
        #ifndef NDEBUG
            block_t::perm_begin = permutation.begin();
            state_info_entry::s_i_begin = state_info.data();
            state_info_entry::s_i_end = state_info_entry::s_i_begin + n;
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
    /// \details The destructor assumes that the caller has already executed
    /// `clear()` to deallocate the memory for the partition.
    ~part_state_t()
    {
        assert(state_info.empty());
        assert(permutation.empty());
    }

    /// \brief deallocates constellations and blocks
    /// \details This function can be called shortly before destructing the
    /// partition.  Afterwards, the data structure is in a unusable state,
    /// as all information on states, blocks and constellations is deleted and
    /// deallocated.
    void clear()
    {
        permutation_iter_t permutation_iter = permutation.end();
        #ifndef NDEBUG
            state_type deleted_blocks = 0;
        #endif
        while (permutation.begin() != permutation_iter)
        {
            block_t* const B = permutation_iter[-1]->block;
            assert(B->end() == permutation_iter);
            permutation_iter = B->begin();
            #ifndef NDEBUG
                ++deleted_blocks;
            #endif
            delete B;
        }
        assert(deleted_blocks == block_t::nr_of_blocks);
        block_t::nr_of_blocks = 0;
        state_info.clear();
        permutation.clear();
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
    /// \details If the slice indicated by the parameters is not empty, the
    /// states in this slice will be printed.
    /// \param message text printed as a title if the slice is not empty
    /// \param B       block that is being printed (it is checked whether
    ///                states belong to this block)
    /// \param begin   iterator to the beginning of the slice
    /// \param end     iterator past the end of the slice
    void print_block(const char* message, const block_t* B,
        permutation_const_iter_t begin, permutation_const_iter_t end) const;
  public:
    /// \brief print the partition as a tree (per constellation and block)
    /// \details The function prints all constellations (in order); for each
    /// constellation it prints the blocks it consists of; and for each block,
    /// it lists its states, separated into nonbottom and bottom states.
    /// \param part_tr partition for the transitions
    void print_part(const part_trans_t& part_tr) const;

    /// \brief print all transitions
    /// \details For each state (in order), its outgoing transitions are
    /// listed, sorted by goal constellation.  The function also indicates
    /// where the current constellation pointer of the state points at.
    void print_trans() const;
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
/// Basically, transitions are stored in three arrays:
/// - `pred`: transitions ordered by goal state, to allow finding all
///   predecessors of a goal state.
/// - `succ`: transitions ordered by source state and goal constellation, to
///   allow finding all successors of a source state.  Given a transition in
///   this array, it is easy to find all transitions from the same source state
///   to the same goal constellation.  It is possible to find out, in time
///   logarithmic in the out-degree, whether a state has a transition to a
///   given constellation.
/// - `B_a_B`: a permutation of the transitions such that transitions from the
///    same source block, with the same action, to the same goal block are
///    adjacent.  Further, this array does not need a specific sort order.
///
/// Within this sort order, inert transitions are always placed after non-inert
/// transitions.
///
/// state_info_entry and block_t (defined above) contain pointers to the slices
/// of these arrays.  For the incoming transitions, they contain enough
/// information; for the outgoing and the B_a_B-transitions, we additionally
/// use so-called _descriptors_ that show which slice belongs together.

///@{

class out_descriptor;

/* pred_entry, succ_entry, and B_a_B_entry contain the data that is stored
about a transition.  Every transition has one of each data structure; the three
structures are linked through the iterators (used here as pointers). */
class succ_entry
{
  public:
    B_a_B_iter_t B_a_B;
    state_info_ptr target;
    out_descriptor* bunch_slice;
};


class pred_entry
{
  public:
    succ_iter_t succ;
    state_info_ptr source;

#ifndef NDEBUG
    /// \brief print a short transition identification for debugging
    /// \details This function is only available if compiled in Debug mode.
    std::string debug_id_short() const
    {
        return "from " + source->debug_id_short() + " to " +
                                                succ->target->debug_id_short();
    }

    /// \brief print a transition identification for debugging
    /// \details This function is only available if compiled in Debug mode.
    std::string debug_id() const
    {
        return "transition " + debug_id_short();
    }

    mutable bisim_gjkw::check_complexity::trans_counter_t work_counter;
#endif
};


class B_a_B_entry
{
  public:
    pred_iter_t pred;
    B_a_B_desc_iter_t B_a_B_slice;
};


/* out_descriptor and B_a_B_slice_t are data types that indicate which
slice of states belongs together. */
class out_descriptor
{
  public:
    succ_iter_t end, begin;

    out_descriptor(succ_iter_t iter)
      : end(iter),
        begin(iter)
    {
        // assert(int_begin <= int_end);
    }

    state_type size() const
    {
        return end - begin;
    }



#ifndef NDEBUG
    /// adds work (for time complexity measurement) to every transition in the
    /// slice.
    void add_work_to_transns(enum bisim_gjkw::check_complexity::counter_type ctr,
                                                       unsigned char max_value)
    {
        assert(begin < end);
        succ_iter_t iter = begin;
        mCRL2complexity(iter->B_a_B->pred, add_work(ctr, max_value));
        while (++iter != end)
        {
            // treat temporary counters specially
            mCRL2complexity(iter->B_a_B->pred,
                                         add_work_notemporary(ctr, max_value));
        }
    }
#endif
};


/// \brief Information about a set of transitions with the same source block,
/// transition label, and target block
/// \details This describes the finer partition of the transitions.  In the end
/// we will have one slice for each transition, so we should try to minimize
/// this data structure as much as possible.  During refinement, the labels do
/// not need to be copied; they can easily be kept in a separate partition to
/// collect the information later.  Perhaps even the bunch pointer can be
/// deleted?  Do we need a slice pointer with each individual transition or
/// would a bunch pointer be enough?
///
/// Also note that these slices are part of a doubly-linked list.  We cannot
/// change this to a singly-linked list because to find all transitions in
/// FromRed, we need to be able to go forward and backward in this list (or we
/// would have to store, for each relevant pair <block, bunch>, the first slice
/// containing transitions from this block in this bunch, leading to larger
/// memory usage than before).

class B_a_B_slice_t
{
  public:
    /// pointer past the end of the transitions in the B_a_B array
    B_a_B_iter_t end;

    /// pointer to the beginning of the transitions in the B_a_B array
    B_a_B_iter_t begin;

    /// \brief pointer to the first transition in the B_a_B array that starts
    /// in a non-bottom state
    /// \details This is only used during postprocessing; if it's not used, it
    /// should be set equal to `begin`.
    B_a_B_iter_t begin_from_non_bottom;

    /// bunch to which this slice belongs
    bunch_t* bunch;

    /// label of the transitions in this slice
    /// \details We do not actually need to store the label with each slice
    /// individually; it would be enough to store a separate partition of the
    /// transitions into equally-labelled transitions. As these transitions
    /// will never leave such blocks, the label can easily be restored
    /// afterwards.
    label_type label;

    B_a_B_slice_t(B_a_B_iter_t begin_, B_a_B_iter_t end_, label_type label_,
                                                               bunch_t* bunch_)
      : end(end_),
        begin(begin_),
        begin_from_non_bottom(begin_),
        bunch(bunch_),
        label(label_)
    {  }

    /// compute the source block of the transitions in this slice
    const block_t* from_block() const
    {
        assert(begin < end);
        assert(begin->pred->succ->B_a_B == begin);
        return begin->pred->source->block;
    }
    block_t* from_block()
    {
        assert(begin < end);
        assert(begin->pred->succ->B_a_B == begin);
        return begin->pred->source->block;
    }

    /// compute the goal block of the transitions in this slice
    const block_t* to_block() const
    {
        assert(begin < end);
        assert(begin->pred->succ->B_a_B == begin);
        return begin->pred->succ->target->block;
    }
    block_t* to_block()
    {
        assert(begin < end);
        assert(begin->pred->succ->B_a_B == begin);
        return begin->pred->succ->target->block;
    }

    trans_type size() const
    {
        return end - begin;
    }

    /// \brief returns true iff the slice is marked for postprocessing
    /// \details The function uses the data registered with the goal
    /// constellation.
    bool needs_postprocessing() const
    {
        // assert(to_block()->postprocess_end <= begin ||
        //                               end <= to_constln()->postprocess_end);
        // assert(to_block()->postprocess_begin <= begin ||
        //                             end <= to_constln()->postprocess_begin);
        // return to_block()->postprocess_begin <= begin &&
        //                                end <= to_constln()->postprocess_end;
        assert(0);
        exit(1);
    }

#ifndef NDEBUG
    /// \brief print a B_a_B slice identification for debugging
    /// \details This function is only available if compiled in Debug mode.
    std::string debug_id() const
    {
        assert(begin < end);
        std::string result("slice containing transition");
        if (end - begin > 1)
            result += "s ";
        else
            result += " ";
        B_a_B_const_iter_t iter = begin;
        assert(iter->pred->succ->B_a_B == iter);
        result += iter->pred->debug_id_short();
        if (end - iter > 4)
        {
            assert(iter[1].pred->succ->B_a_B == iter+1);
            result += ", ";
            result += iter[1].pred->debug_id_short();
            result += ", ...";
            iter = end - 3;
        }
        while (++iter != end)
        {
            assert(iter->pred->succ->B_a_B == iter);
            result += ", ";
            result += iter->pred->debug_id_short();
        }
        return result;
    }


    /// The function is meant to transfer work temporarily assigned to the
    /// B_a_B slice to the transitions in the slice.  It is used during
    /// handling of new bottom states, so the work is only assigned to
    /// transitions that start in a (new) bottom state.
    /// If at this moment no such (new) bottom state has been found, the work
    /// is kept with the slice and the function returns false.  The work should
    /// be transferred later (but if there is no later transfer, it should be
    /// tested that the function returns true).
    bool add_work_to_bottom_transns(enum bisim_gjkw::check_complexity::counter_type ctr,
                                                       unsigned char max_value)
    {
        bool added = false;

        for (B_a_B_const_iter_t iter = begin; iter != end; ++iter)
        {
            if (iter->pred->source->pos >=
                                     iter->pred->source->block->bottom_begin())
            {
                // source state of the transition is a bottom state
                mCRL2complexity(iter->pred, add_work(ctr, max_value));
                added = true;
            }
        }
        return added;
    }


    //mutable check_complexity::B_a_B_counter_t work_counter;
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
    B_a_B_iter_t begin, end;

    union U {
        void* null;
        std::forward_list<B_a_B_desc_iter_t>::iterator list_entry;

        // must specify a default constructor for the union because the
        // iterator has a non-trivial default constructor:
        U()  {  null = nullptr;  }
    } postprocess;

    /// \brief points to a slice in this bunch that contains transitions from
    /// `new_slice_block`.
    /// \details When splitting a block, transitions from its states in the
    /// same bunch should be placed together in the list of all outgoing
    /// transitions from that block.  If some transition from the new block in
    /// this bunch is found, `new_slice_pos` is set to the slice of this
    /// transition, and `new_slice_block` is set to the new block of that
    /// moment.
    B_a_B_desc_iter_t new_slice_pos;

    const block_t* new_slice_block;
  private:
    static bunch_t* first_nontrivial;

    bunch_t* next_nontrivial;
  public:
    bunch_t(B_a_B_iter_t new_begin, B_a_B_iter_t new_end)
      : begin(new_begin),
        end(new_end),
        next_nontrivial(nullptr)
    {  }


    static bunch_t* get_some_nontrivial()  {  return first_nontrivial;  }


    void make_nontrivial()
    {
        assert(nullptr == next_nontrivial);
        next_nontrivial = nullptr==first_nontrivial ? this : first_nontrivial;
        first_nontrivial = this;
    }


    void make_trivial()
    {
        assert(nullptr != next_nontrivial);
        assert(first_nontrivial == this);
        first_nontrivial = this==next_nontrivial ? nullptr : next_nontrivial;
        next_nontrivial = nullptr;
    }


    /// \brief split off a single B_a_B slice from the bunch
    /// \details The function splits the current bunch after its first B_a_B
    /// slice or before its last B_a_B slice, whichever is smaller.  It creates
    /// a new bunch for the split-off slice and returns a pointer to the
    /// slice.  The caller may have to move the slice to another place in its
    /// list of slices so as to ensure that it is
    B_a_B_desc_iter_t split_off_small_B_a_B()
    {
        assert(begin < end);
        B_a_B_desc_iter_t const FirstSl = begin->B_a_B_slice;
        B_a_B_desc_iter_t const LastSl = end[-1].B_a_B_slice;
        assert(FirstSl != LastSl);
        if (FirstSl->end == LastSl->begin)  make_trivial();

        assert(FirstSl->bunch == this);
        assert(LastSl->bunch == this);
        //assert(postprocess_begin == postprocess_end);
        // 2.6: Create a new bunch NewBu and ...
        bunch_t* const NewBu = new bunch_t(begin, end);

        // 2.5: Choose a small splitter B_a_B slice SpSl subset of SpBu,
        //      i.e. |SpSl| <= 1/2*|SpBu|
        /// It doesn't matter very much how ties are resolved here:
        /// `part_tr.move_to_new_bunch()` is faster if the first block is selected to
        /// be split off.  `part_tr.split_s_inert_out()` is faster if the last
        /// block is selected.
        if (FirstSl->size() > LastSl->size())
        {
            // 2.6: ... and move SpB from SpC to NewC
            end = LastSl->begin;
            NewBu->begin = end;
            LastSl->bunch = NewBu;
            return LastSl;
        }
        else
        {
            // 2.6: ... and move SpB from SpC to NewC
            begin = FirstSl->end;
            NewBu->end = begin;
            FirstSl->bunch = NewBu;
            return FirstSl;
        }
    }


};


/* part_trans_t collects and organises all data for the transitions. */
class part_trans_t
{
  public:
    bisim_gjkw::fixed_vector<pred_entry> pred;
    bisim_gjkw::fixed_vector<succ_entry> succ;
    bisim_gjkw::fixed_vector<B_a_B_entry> B_a_B;

    /// futureFromRed is a pointer to a slice that will be a splitter in the
    /// future.  This pointer must be updated if the block is refined:
    /// afterwards, the slice should contain transitions from the red subblock
    /// to the same bunch.  This pointer may be nullptr; in that case, nothing
    /// needs to be done with it.
    bisim_tb::B_a_B_desc_iter_t futureFromRed;

    /// list of slices that need postprocessing
    std::forward_list<bisim_tb::B_a_B_desc_iter_t> R;

    void swap_in(pred_iter_t const pos1, pred_iter_t const pos2)
    {
        assert(pred.end() > pos1 && pos1->succ->B_a_B->pred == pos1);
        assert(pred.end() > pos2 && pos2->succ->B_a_B->pred == pos2);

        // swap contents
        pred_entry const temp_entry(*pos1);
        *pos1 = *pos2;
        *pos2 = temp_entry;
        // swap pointers to contents
        pos1->succ->B_a_B->pred = pos1;
        pos2->succ->B_a_B->pred = pos2;
    }

    void swap_out(succ_iter_t const pos1, succ_iter_t const pos2)
    {
        assert(succ.end() > pos1 && pos1->B_a_B->pred->succ == pos1);
        assert(succ.end() > pos2 && pos2->B_a_B->pred->succ == pos2);

        // swap contents
        succ_entry const temp_entry(*pos1);
        *pos1 = *pos2;
        *pos2 = temp_entry;
        // swap pointers to contents
        pos1->B_a_B->pred->succ = pos1;
        pos2->B_a_B->pred->succ = pos2;
    }

    // *pos1 -> *pos2 -> *pos3 -> *pos1
    void swap3_out(succ_iter_t const pos1, succ_iter_t const pos2,
                                                        succ_iter_t const pos3)
    {
        assert(succ.end() > pos1 && pos1->B_a_B->pred->succ == pos1);
        assert(succ.end() > pos2 && pos2->B_a_B->pred->succ == pos2);
        assert(succ.end() > pos3 && pos3->B_a_B->pred->succ == pos3);

        assert(pos1 != pos2 || pos1 == pos3);
        // swap contents
        succ_entry const temp_entry(*pos1);
        *pos1 = *pos3;
        *pos3 = *pos2;
        *pos2 = temp_entry;
        // swap pointers to contents
        pos1->B_a_B->pred->succ = pos1;
        pos2->B_a_B->pred->succ = pos2;
        pos3->B_a_B->pred->succ = pos3;
    }

    void swap_B_a_B(B_a_B_iter_t const pos1, B_a_B_iter_t const pos2)
    {
        assert(B_a_B.end() > pos1 && pos1->pred->succ->B_a_B == pos1);
        assert(B_a_B.end() > pos2 && pos2->pred->succ->B_a_B == pos2);

        // swap contents
        B_a_B_entry const temp_entry(*pos1);
        *pos1 = *pos2;
        *pos2 = temp_entry;
        // swap pointers to contents
        pos1->pred->succ->B_a_B = pos1;
        pos2->pred->succ->B_a_B = pos2;
    }

    // *pos1 -> *pos2 -> *pos3 -> *pos1
    void swap3_B_a_B(B_a_B_iter_t const pos1, B_a_B_iter_t const pos2,
                                                       B_a_B_iter_t const pos3)
    {
        assert(B_a_B.end() > pos1 && pos1->pred->succ->B_a_B == pos1);
        assert(B_a_B.end() > pos2 && pos2->pred->succ->B_a_B == pos2);
        assert(B_a_B.end() > pos3 && pos3->pred->succ->B_a_B == pos3);

        assert(pos1 != pos2 || pos1 == pos3);
        // swap contents
        B_a_B_entry const temp_entry(*pos1);
        *pos1 = *pos3;
        *pos3 = *pos2;
        *pos2 = temp_entry;
        // swap pointers to contents
        pos1->pred->succ->B_a_B = pos1;
        pos2->pred->succ->B_a_B = pos2;
        pos3->pred->succ->B_a_B = pos3;
    }

    part_trans_t(trans_type m)
      : pred(m),
        succ(m),
        B_a_B(m)
    {  }
    ~part_trans_t()
    {
        assert(B_a_B.empty());
        assert(succ.empty());
        assert(pred.empty());
    }

    /// clear allocated memory
    void clear()
    {
        // B_a_B_slice_ts are deallocated when their respective lists are
        // deallocated by destructing the blocks.
        // out_descriptors do not need to be deallocated individually, but they
        // are cleared by deleting the constln_slice_pool.
//        constln_slice_pool.clear();
        B_a_B.clear();
        succ.clear();
        pred.clear();
    }

    trans_type trans_size() const  {  return pred.size();  }

    /* split_inert_to_C splits the B_a_B slice of block b to its own
    constellation into two slices: one for the inert and one for the non-inert
    transitions.  It is called with SpB just after a constellation is split, as
    the transitions from SpB to itself (= the inert transitions) now go to a
    different constellation than the other transitions from SpB to its old
    constellation.  It does, however, not adapt the other transition arrays to
    reflect that noninert and inert transitions from block b would go to
    different constellations.
    Its time complexity is O(1+min {|out_noninert(b-->C)|, |out_inert(b)|}). */
    void split_inert_to_C(block_t* B);

    /* part_trans_t::move_to_new_bunch has to be called after a transition has
    changed its bunch.  The member function will adapt the transition
    data structure.  It assumes that the transition is non-inert and that the
    new bunch does not have inert transitions.  It returns the boundary between
    transitions in OldBu and transitions in NewBu in the state's outgoing
    transition array. */
    succ_iter_t move_to_new_bunch(B_a_B_iter_t s_iter, bunch_t* OldBu,
                            bunch_t* NewBu, bool first_transition_of_state);

    /* part_trans_t::make_noninert marks the transition identified by
    B_a_B_iter as noninert in the pred and succ arrays.  The caller should mark
    it as noninert in the B_a_B array. */
    void make_noninert_pred_succ(B_a_B_iter_t const B_a_B_iter)
    {
        pred_iter_t pred_iter = B_a_B_iter->pred;
        succ_iter_t succ_iter = pred_iter->succ;
        // change pred
        pred_iter_t const other_pred = succ_iter->target->inert_pred_begin();
        assert(succ_iter->target->pred_begin() <= other_pred);
        assert(other_pred <= pred_iter);
        assert(pred_iter < succ_iter->target->pred_end());
        swap_in(pred_iter, other_pred);
        succ_iter->target->set_inert_pred_begin(other_pred + 1);
        // change succ
        succ_iter_t const other_succ = other_pred->source->inert_succ_begin();
        assert(other_pred->source->succ_begin() <= other_succ);
        assert(other_succ <= succ_iter);
        assert(succ_iter < other_pred->source->succ_end());
        swap_out(succ_iter, other_succ);
        other_pred->source->set_inert_succ_begin(other_succ + 1);
    }

  private:
    /// \brief create a new slice for transitions from NewB in the correct
    /// bunch
    /// \details When one creates a new slice for transitions from NewB, one
    /// has to ensure that the order of slices in the list `NewB->from_block`
    /// is correct:  slices that belong to the same bunch should be placed
    /// together.  This function creates a new slice and inserts it in the
    /// correct place in that list.
    B_a_B_desc_iter_t create_new_slice_for_NewB(
        B_a_B_desc_iter_t const old_B_a_B_slice, block_t* NewB,
             B_a_B_iter_t const new_pos, const bunch_t* const futureFromRedBu);

    /// \brief handle one transition after a block has been split
    /// \details The main task of this method is to move the transition to the
    /// correct place in the B_a_B slice.  In addition, it also checks whether
    /// the transition has become non-inert and if necessary adapts pred and
    /// succ, and it makes the source state a new bottom state.
    /// It also maintains futureFromRed correct.  If the new block is the red
    /// block and the transition starts in the red block, `futureFromRedBu`
    /// should be set to the bunch of FromRed;  otherwise, `futureFromRedBu`
    /// should be `nullptr`.
    void handle_transition(B_a_B_iter_t const old_pos, block_t* OldB,
                                block_t* NewB, const bunch_t* futureFromRedBu);
  public:
    /* part_trans_t::new_block_created splits the B_a_B-slices to reflect that
    some transitions now start in the new block NewB.  They can no longer be in
    the same slice as the transitions that start in the old block.
    
    Its time complexity is O(1 + |in(NewB)| + |out(NewB)|). */
    void new_block_created(block_t* OldB, block_t* NewB, bool NewB_is_blue);

    B_a_B_const_iter_t B_a_B_begin() const  {  return B_a_B.begin();  }
    B_a_B_iter_t       B_a_B_end  ()        {  return B_a_B.end  ();  }
    pred_const_iter_t pred_end() const  {  return pred.end();  }
    succ_const_iter_t succ_end() const  {  return succ.end();  }

#ifndef NDEBUG
    /// \brief assert that the data structure is consistent and stable
    void assert_stability(const part_state_t& part_st) const;
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



struct refine_shared_t;

} // end namespace bisim_tb

/// \class bisim_partitioner_tb
/// \brief implements the main algorithm for the stutter equivalence quotient
template <class LTS_TYPE>
class bisim_partitioner_tb
{
  private:
    LTS_TYPE& aut;
    bisim_tb::part_state_t part_st;
    bisim_tb::part_trans_t part_tr;
  public:
    // The constructor constructs the data structures and immediately
    // calculates the bisimulation quotient.  However, it does not change the
    // LTS.
    bisim_partitioner_tb(LTS_TYPE& l, bool branching = false,
                                        bool preserve_divergence = false)
      : aut(l),
        part_st(l.num_states()),
        part_tr(l.num_transitions())
        // It would be more efficient not to store the self-loops in part_tr
        // at all... To that end, we would have to go through the transitions
        // first to find out how many self-loops there are before we initialise
        // part_tr.
    {
      assert(branching || !preserve_divergence);
      create_initial_partition_tb(branching, preserve_divergence);
      refine_partition_until_it_becomes_stable_tb(branching);
    }
    ~bisim_partitioner_tb()
    {
      part_tr.clear();
      part_st.clear();
    }

    // replace_transition_system() replaces the transitions of the LTS stored
    // here by those of its bisimulation quotient.  However, it does not change
    // anything else; in particular, it does not change the number of states of
    // the LTS.
    void replace_transition_system(bool branching, bool preserve_divergence);

    static state_type num_eq_classes()
    {
        return bisim_tb::block_t::nr_of_blocks;
    }

    state_type get_eq_class(state_type s) const
    {
        return part_st.block(s)->seqnr;
    }

    bool in_same_class(state_type s, state_type t) const
    {
        return part_st.block(s) == part_st.block(t);
    }

  private:

    /*-------- dbStutteringEquivalence -- Algorithm 2 of [GJKW 2017] --------*/

    void create_initial_partition_tb(bool branching, bool preserve_divergence);
    void refine_partition_until_it_becomes_stable_tb(bool branching);

    /*----------------- Refine -- Algorithm 3 of [GJKW 2017] ----------------*/

    bisim_tb::block_t* refine(bisim_tb::block_t* const RfnB,
                           bisim_tb::B_a_B_desc_iter_t FromRed,
                           const bisim_tb::bunch_t* NewBu = nullptr,
                           bool unmarked_bottom_states_are_blue = true);

    DECLARE_COROUTINE(refine_blue,
    /* formal parameters:*/ ((bisim_tb::block_t* const, RfnB))
                            ((bisim_tb::B_a_B_desc_const_iter_t const,FromRed))
                            ((const bisim_tb::bunch_t* const, NewBu))
                            ((const bool, unmarked_bottom_states_are_blue)),
    /* local variables:  */ ((bisim_tb::permutation_iter_t, visited_end))
                            ((bisim_tb::state_info_ptr, s))
                            ((bisim_tb::pred_iter_t, pred_iter))
                            ((bisim_tb::state_info_ptr, s_prime))
                            ((bisim_tb::permutation_iter_t,blue_nonbottom_end))
                            ((bisim_tb::succ_const_iter_t, succ_iter)),
    /* shared data:      */ struct bisim_tb::refine_shared_t, shared_data,
    /* interrupt locatns:*/ (REFINE_BLUE_PREDECESSOR_HANDLED)
                            (REFINE_BLUE_TESTING)
                            (REFINE_BLUE_STATE_HANDLED)
                            (REFINE_BLUE_COLLECT_BOTTOM));

    DECLARE_COROUTINE(refine_red,
    /* formal parameters:*/ ((bisim_tb::block_t* const, RfnB))
                            ((bisim_tb::B_a_B_desc_iter_t const, FromRed))
                            ((const bool, unmarked_bottom_states_are_blue)),
    /* local variables:  */ ((bisim_tb::B_a_B_iter_t, fromred_visited_ptr))
                            ((bool, forward))
                            ((bisim_tb::B_a_B_desc_iter_t, slice))
                            ((bisim_tb::permutation_iter_t, visited_begin))
                            ((bisim_tb::state_info_ptr, s))
                            ((bisim_tb::pred_iter_t, pred_iter)),
    /* shared data:      */ struct bisim_tb::refine_shared_t, shared_data,
    /* interrupt locatns:*/ (REFINE_RED_COLLECT_FROMRED)
                            (REFINE_RED_PREDECESSOR_HANDLED)
                            (REFINE_RED_STATE_HANDLED)
                            (REFINE_RED_COLLECT_FROMRED_SLICE));

    /*--------- PostprocessNewBottom -- Algorithm 4 of [GJKW 2017] ----------*/

    bisim_tb::block_t* prepare_for_postprocessing(bisim_tb::block_t** RfnB);

    bisim_tb::block_t* postprocess_new_bottom(bisim_tb::block_t* RedB
                                /* , bisim_tb::block_t* BlueB */);
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
void bisimulation_reduce_tb(LTS_TYPE& l, bool const branching = false,
                                        bool const preserve_divergence = false)
{
  // First, remove tau loops in case of branching bisimulation.
  if (branching)
  {
    scc_reduce(l, preserve_divergence);
  }

  // Second, apply the branching bisimulation reduction algorithm. If there are
  // no taus, this will automatically yield strong bisimulation.
  detail::bisim_partitioner_tb<LTS_TYPE> bisim_part(l, branching,
                                                          preserve_divergence);

  // Assign the reduced LTS
  //mCRL2log(log::debug, "bisim_tb") <<"number of states in the lumped chain: "
  //    << bisim_part.num_eq_classes()
  //    << "; initial state: originally state " << l.initial_state()
  //    <<" = lumped state "<<bisim_part.get_eq_class(l.initial_state())<<"\n";
  bisim_part.replace_transition_system(branching, preserve_divergence);
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
bool destructive_bisimulation_compare_tb(LTS_TYPE& l1, LTS_TYPE& l2,
          bool const branching = false, bool const preserve_divergence = false,
                                  bool const generate_counter_examples = false)
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
    scc_part.replace_transition_system(preserve_divergence);
    init_l2 = scc_part.get_eq_class(init_l2);
  }

  detail::bisim_partitioner_tb<LTS_TYPE> bisim_part(l1, branching,
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
inline bool bisimulation_compare_tb(const LTS_TYPE& l1, const LTS_TYPE& l2,
          bool const branching = false, bool const preserve_divergence = false)
{
  LTS_TYPE l1_copy(l1);
  LTS_TYPE l2_copy(l2);
  return destructive_bisimulation_compare_tb(l1_copy, l2_copy, branching,
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

namespace bisim_tb
{

/// \brief quick check to find out whether the state has a transition to `SpC`
/// \details If the current constellation pointer happens to be set to `SpC` or
/// its successor, the function can quickly find out whether the state has a
/// transition to `SpC`.
/// The function should not be called for the constellation in which the state
/// resides.
/// \param SpC constellation of interest
/// \returns true if the state is known to have a transition to `SpC`
/// \memberof state_info_entry
inline bool state_info_entry::surely_has_transition_to(const bunch_t* const
                                                                    SpBu) const
{
    if (succ_begin() == succ_end())
    {
        assert(succ_end() == current_bunch);
    }
    else
    {
        assert(succ_begin() < current_bunch);
        assert(current_bunch <= succ_end());
        assert(succ_end() == current_bunch ||
                        current_bunch[-1].B_a_B->B_a_B_slice->bunch !=
                                     current_bunch->B_a_B->B_a_B_slice->bunch);
    }
    return current_bunch != succ_end() &&
                              current_bunch->B_a_B->B_a_B_slice->bunch == SpBu;
}


/// \brief quick check to find out whether the state has _no_ transition to
/// `SpC`
/// \details If the current constellation pointer happens to be set to `SpC` or
/// its successor, the function can quickly find out whether the state has a
/// transition to `SpC`.
/// The function should not be called for the constellation in which the state
/// resides.
/// \param SpC constellation of interest
/// \returns true if the state is known to have _no_ transition to `SpC`
/// \memberof state_info_entry
inline bool state_info_entry::surely_has_no_transition_to(const bunch_t* SpBu,
                                                    const bunch_t* NewBu) const
{
    if (succ_begin() == succ_end())
    {
        assert(succ_end() == current_bunch);
        return true;
    }
    else
    {
        assert(succ_begin() < current_bunch);
        assert(current_bunch <= succ_end());
        assert(succ_end() == current_bunch ||
                        current_bunch[-1].B_a_B->B_a_B_slice->bunch !=
                                     current_bunch->B_a_B->B_a_B_slice->bunch);
        return current_bunch[-1].B_a_B->B_a_B_slice->bunch == NewBu &&
                              current_bunch->B_a_B->B_a_B_slice->bunch != SpBu;
    }
}


} // end namespace bisim_tb
} // end namespace detail
} // end namespace lts
} // end namespace mcrl2

#endif // ifndef _LIBLTS_BISIM_GJKW_H
