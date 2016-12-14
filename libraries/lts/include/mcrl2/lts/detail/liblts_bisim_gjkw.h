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

#include <unordered_map> // used during initialisation
#include <list>          // for the list of B_to_C_descriptors

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

// state_type and trans_type are defined in check_complexity.h.

/// type used to store label numbers and counts
typedef size_t label_type;





/* ************************************************************************* */
/*                                                                           */
/*                   R E F I N A B L E   P A R T I T I O N                   */
/*                                                                           */
/* ************************************************************************* */





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
/// state_info_ptr - an entry in the permutation array; it contains a
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
///   state_info_ptr actually contains a pointer to a state_info_entry with
///   the additional guarantee that this is not the last entry in state_info_t.
///   (To make this a small bit more type safe, we could change the type
///   state_info_ptr to something like ``pointer to an array with two
///   state_info_entries'', typedef state_info_entry (*state_info_ptr)[2];.
///   Still, that would allow unsafe pointer juggling.)
/// - A block_t also contains information about its outgoing inert transitions.
/// - A state_info_entry also contains information used during trysplit or
///   process_new_bottom.
///@{

namespace bisim_gjkw
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
typedef fixed_vector<state_info_ptr> permutation_t;
typedef permutation_t::iterator permutation_iter_t;
typedef permutation_t::const_iterator permutation_const_iter_t;

class block_t;
class constln_t;

class B_to_C_entry;
class pred_entry;
class succ_entry;
typedef fixed_vector<B_to_C_entry>::iterator B_to_C_iter_t;
typedef fixed_vector<pred_entry>::iterator pred_iter_t;
typedef fixed_vector<succ_entry>::iterator succ_iter_t;

typedef fixed_vector<B_to_C_entry>::const_iterator B_to_C_const_iter_t;
typedef fixed_vector<pred_entry>::const_iterator pred_const_iter_t;
typedef fixed_vector<succ_entry>::const_iterator succ_const_iter_t;
class B_to_C_descriptor;
typedef std::list<B_to_C_descriptor> B_to_C_desc_list;
typedef B_to_C_desc_list::iterator B_to_C_desc_iter_t;
typedef B_to_C_desc_list::const_iterator B_to_C_desc_const_iter_t;

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
  private:
    /// \brief iterator to first incoming transition
    /// \details also serves as iterator past the last incoming transition of
    /// the previous state.
    pred_iter_t state_in_begin;

    /// \brief iterator to first outgoing transition
    /// \details also serves as iterator past the last outgoing transition of
    /// the previous state.
    succ_iter_t state_out_begin;

    /// iterator to first _inert_ incoming transition
    pred_iter_t state_inert_in_begin;

    /// iterator to first _inert_ outgoing transition
    succ_iter_t state_inert_out_begin;

    /// iterator past the last _inert_ outgoing transition
    succ_iter_t state_inert_out_end;
  public:
    /// block where the state belongs
    block_t* block;

    /// position of the state in the permutation array
    permutation_iter_t pos;

    /// number of inert transitions to non-blue states
    state_type notblue;
  private:
    /// iterator to first outgoing transition to the constellation of interest
    succ_iter_t int_current_constln;
  public:
    /// get constellation where the state belongs
    const constln_t* constln() const;
    constln_t* constln();

    succ_const_iter_t current_constln() const {  return int_current_constln;  }
    succ_iter_t current_constln()  {  return int_current_constln;  }
    void set_current_constln(succ_iter_t new_current_constln);

    /// iterator to first incoming transition
    pred_const_iter_t pred_begin() const  {  return state_in_begin;  }
    pred_iter_t pred_begin()  {  return state_in_begin;  }
    void set_pred_begin(pred_iter_t new_in_begin)
    {
        state_in_begin = new_in_begin;
    }

    /// iterator past the last incoming transition
    pred_const_iter_t pred_end() const  {  return this[1].state_in_begin;  }
    pred_iter_t pred_end()  {  return this[1].state_in_begin;  }
    void set_pred_end(pred_iter_t new_in_end)
    {
        this[1].set_pred_begin(new_in_end);
    }

    /// iterator to first non-inert incoming transition
    pred_const_iter_t noninert_pred_begin() const  {  return state_in_begin;  }
    pred_iter_t noninert_pred_begin()  {  return state_in_begin;  }

    /// iterator past the last non-inert incoming transition
    pred_const_iter_t noninert_pred_end() const  { return inert_pred_begin(); }
    pred_iter_t noninert_pred_end()  {  return inert_pred_begin();  }

    /// iterator to first inert incoming transition
    pred_const_iter_t inert_pred_begin() const { return state_inert_in_begin; }
    pred_iter_t inert_pred_begin()  {  return state_inert_in_begin;  }
    void set_inert_pred_begin(pred_iter_t new_inert_in_begin)
    {
        state_inert_in_begin = new_inert_in_begin;
        assert(pred_begin() <= inert_pred_begin());
        assert(inert_pred_begin() <= pred_end());
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
    succ_const_iter_t succ_end() const  {  return this[1].state_out_begin;  }
    succ_iter_t succ_end()  {  return this[1].state_out_begin;  }
    void set_succ_end(succ_iter_t new_out_end)
    {
        this[1].set_succ_begin(new_out_end);
        assert(succ_begin() <= succ_end());
    }

    /// iterator to first inert outgoing transition
    succ_const_iter_t inert_succ_begin() const  {return state_inert_out_begin;}
    succ_iter_t inert_succ_begin()  {  return state_inert_out_begin;  }
    void set_inert_succ_begin(succ_iter_t new_inert_out_begin);

    /// iterator past the last inert outgoing transition
    succ_const_iter_t inert_succ_end() const  {  return state_inert_out_end;  }
    succ_iter_t inert_succ_end()  {  return state_inert_out_end;  }
    void set_inert_succ_end(succ_iter_t new_inert_out_end);
    void set_inert_succ_begin_and_end(succ_iter_t new_inert_out_begin,
                                                 succ_iter_t new_inert_out_end)
    {
        state_inert_out_begin = new_inert_out_begin;
        state_inert_out_end = new_inert_out_end;
        assert(succ_begin() <= inert_succ_begin());
        assert(inert_succ_begin() <= inert_succ_end());
        assert(inert_succ_end() <= succ_end());
        // The following assertions cannot be tested always, as the function
        // may be called earlier than the assertions are reestablished.
        // assert(succ_begin() == inert_succ_begin() ||
        //            *inert_succ_begin()[-1].target->constln() <= *constln());
        // assert(succ_begin() == inert_succ_begin() ||
        //                      inert_succ_begin()[-1].target->block != block);
        // assert(inert_succ_begin() == inert_succ_end() ||
        //                      (inert_succ_begin()->target->block == block &&
        //                       inert_succ_end()[-1].target->block == block));
        // assert(succ_end() == inert_succ_end() ||
        //                  *constln() < *inert_succ_end()->target->constln());
    }

    bool surely_has_transition_to(const constln_t* SpC) const;
    bool surely_has_no_transition_to(const constln_t* SpC) const;

#ifndef NDEBUG
    /// \brief print a short state identification for debugging
    /// \details This function is only available if compiled in Debug mode.
    std::string debug_id_short() const
    {
        assert(s_i_begin <= this);
        return std::to_string(this - s_i_begin);
    }

    /// \brief print a state identification for debugging
    /// \details This function is only available if compiled in Debug mode.
    std::string debug_id() const
    {
        return "state " + debug_id_short();
    }

  private:
    static state_info_const_ptr s_i_begin;

    friend class part_state_t;
#endif
};


/// swap two permutations
static inline void swap_permutation(permutation_iter_t s1,
                                                        permutation_iter_t s2)
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
/// (During the execution of some functions, more slices are subdivided
/// further;  however, as these subdivisions are local to a single function,
/// they are not stored here.)
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

    /// \brief iterator to the first inert transition of the block
    /// \details If there are no inert transitions, then `inert_begin` and
    /// `inert_end` point to the end of the B_to_C-slice containing transitions
    /// from the block to its own constellation.  If there is no such slice,
    /// both are equal to `B_to_C`.
    B_to_C_iter_t int_inert_begin;

    /// iterator past the last inert transition of the block
    B_to_C_iter_t int_inert_end;
  public:
    /// \brief list of B_to_C with transitions from this block
    /// \details This list serves two purposes: it contains all
    /// B_to_C_descriptors, so that the constellations reachable from this
    /// block can be found; and if this block has transitions to the current
    /// splitter SpC\SpB, then the first element of the list points to these
    /// transitions.
    B_to_C_desc_list to_constln;
  private:
    /// constellation to which the block belongs
    constln_t* int_constln;

    /// \brief next block in the list of refinable blocks
    /// \details If this is the last block in the list, `refinable_next` points
    /// to this very block.  Consequently, it is possible to check whether some
    /// block is refinable without an additional variable.
    block_t* refinable_next;

    /// first block in the list of refinable blocks
    static block_t* refinable_first;

    /// \brief unique sequence number of this block
    /// \details After the stuttering equivalence algorithm has terminated,
    /// this number is used as a state number in the quotient Kripke structure.
    /// (For blocks that contain extra Kripke states, the number is set to
    /// BLOCK_NO_SEQNR).
    state_type int_seqnr;

#define BLOCK_NO_SEQNR ((state_type) -1)

  public:
    /// \brief total number of blocks with unique sequence number allocated
    /// \details Upon starting the stuttering equivalence algorithm, the number
    /// of blocks must be zero.
    static state_type nr_of_blocks;

    /// \brief constructor
    /// \details The constructor initialises the block to: all states are
    /// bottom states, no state is marked, the block is not refinable.
    /// (The constructor is implemented later because it requires the full
    /// definition of constln_t.)
    /// \param constln_ constellation to which the block belongs
    /// \param begin_   initial iterator to the first state of the block
    /// \param end_     initial iterator past the last state of the block
    block_t(constln_t* constln_, permutation_iter_t begin_,
                                                      permutation_iter_t end_);

    ~block_t()  {  }

    /// assigns a unique sequence number
    void assign_seqnr()
    {
        assert(BLOCK_NO_SEQNR == int_seqnr);
        int_seqnr = nr_of_blocks++;
    }

    state_type seqnr() const  {  return int_seqnr;  }

    /// provides an arbitrary refinable block
    static block_t* get_some_refinable()  {  return refinable_first;  }

    /// \brief checks whether the block is refinable
    /// \returns true if the block is refinable
    bool is_refinable() const  {  return nullptr != refinable_next;  }

    /// \brief makes a block refinable (i. e. inserts it into the respective
    /// list)
    /// \returns true if the block was not refinable before
    bool make_refinable()
    {
        if (is_refinable())
        {
            return false;
        }
        refinable_next = nullptr == refinable_first ? this : refinable_first;
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
        refinable_first = refinable_next == this ? nullptr : refinable_next;
        refinable_next = nullptr;
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
        return begin() < other.begin();
    }

    /// constellation where the block belongs to
    const constln_t* constln() const  {  return int_constln;  }
    constln_t* constln()  {  return int_constln;  }
    void set_constln(constln_t* new_constln);

    /// read FromRed
    B_to_C_descriptor* FromRed(const constln_t* SpC);

    /// set FromRed to an existing element in to_constln
    void SetFromRed(B_to_C_desc_iter_t new_fromred);

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

    /// iterator to the first inert transition of the block
    B_to_C_const_iter_t inert_begin() const  {  return int_inert_begin;  }
    B_to_C_iter_t inert_begin()  {  return int_inert_begin;  }
    void set_inert_begin(B_to_C_iter_t new_inert_begin)
    {
        int_inert_begin = new_inert_begin;
        assert(int_inert_begin <= int_inert_end);
    }

    /// iterator past the last inert transition of the block
    B_to_C_const_iter_t inert_end() const  {  return int_inert_end;  }
    B_to_C_iter_t inert_end()  {  return int_inert_end;  }
    void set_inert_end(B_to_C_iter_t new_inert_end)
    {
        int_inert_end = new_inert_end;
        assert(int_inert_begin <= int_inert_end);
    }
    void set_inert_begin_and_end(B_to_C_iter_t new_inert_begin,
                                                   B_to_C_iter_t new_inert_end)
    {
        int_inert_begin = new_inert_begin;
        int_inert_end = new_inert_end;
        assert(int_inert_begin <= int_inert_end);
    }

  private:
    static const char mark_all_states_in_SpB[];
  public:
    /// \brief mark all states in the block
    /// \details This function is used to mark all states of the splitter.
    void mark_all_states()
    {
        assert(marked_nonbottom_begin() == marked_nonbottom_end() &&
                                marked_bottom_begin() == marked_bottom_end());
        check_complexity::count(mark_all_states_in_SpB, size(),
                                                check_complexity::n_log_n);
        set_marked_nonbottom_begin(nonbottom_begin());
        // set_marked_nonbottom_end(nonbottom_end());
        set_marked_bottom_begin(bottom_begin());
        // set_marked_bottom_end(bottom_end());
    }

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
            std::to_string(end() - perm_begin) + ")" +
            (BLOCK_NO_SEQNR != seqnr() ?" (#"+std::to_string(seqnr())+")" :"");
    }

    /// \brief provide an iterator to the beginning of the permutation array
    /// \details This iterator is required to be able to print identifications
    /// for debugging.  It is only available if compiled in Debug mode.
    static permutation_const_iter_t permutation_begin()  { return perm_begin; }
  private:
    static permutation_const_iter_t perm_begin;

    friend class part_state_t;
#endif
};


/// \class constln_t
/// \brief stores information about a constellation
/// \details A constellation corresponds to a slice in the permutation array;
/// its boundaries are also block boundaries.  As the number of constellations
/// is initially unknown, we will allocate it dynamically.
class constln_t
{
  private:
    /// iterator past the last state in the constellation
    permutation_iter_t int_end;

    /// iterator to the first state in the constellation
    permutation_iter_t int_begin;

    /// \brief next constellation in the list of non-trivial constellations
    /// \details If this is the last constellation in the list,
    /// `nontrivial_next` points to this very constellation.  Consequently, it
    /// is possible to check whether some constellation is trivial without an
    /// additional variable.
    constln_t* nontrivial_next;

    /// first constellation in the list of non-trivial constellations
    static constln_t* nontrivial_first;
  public:
    /// \brief iterator to the first transition into this constellation that
    /// needs postprocessing
    /// \details In `postprocess_new_bottom()`, all transitions from a refined
    /// block to the present constellation have to be gone through.  Because
    /// during this process the refined block may be refined even further, we
    /// need `postprocess_begin` and `postprocess_end` to store which
    /// transitions have to be gone through overall.
    ///
    /// If no transitions to this constellation need to be postprocessed, the
    /// variable is set to the same value as postprocess_end, preferably to
    /// part_trans_t::B_to_C->end().
    B_to_C_iter_t postprocess_begin;

    /// \brief iterator past the last transition into this constellation that
    /// needs postprocessing
    B_to_C_iter_t postprocess_end;

    /// \brief constructor
    /// \param begin_ iterator to the first state in the constellation
    /// \param end_   iterator past the last state in the constellation
    constln_t(permutation_iter_t begin_, permutation_iter_t end_,
                                                B_to_C_iter_t postprocess_none)
      : int_end(end_),
        int_begin(begin_),
        nontrivial_next(nullptr),
        postprocess_begin(postprocess_none),
        postprocess_end(postprocess_none)
    {
        assert(int_begin < int_end);
    }

    /// \brief destructor
    ~constln_t()  {  }

    /// \brief provides an arbitrary non-trivial constellation
    /// \details The static function is implemented in a way to provide the
    /// first constellation in the list of non-trivial constellations.
    static constln_t* get_some_nontrivial()  {  return nontrivial_first;  }

    /// \brief provides the next non-trivial constellation
    /// \details This (non-static!) function just returns the next non-trivial
    /// constellation in the list.  Note: If this constellation is the last in
    /// the list of non-trivial constellations, the convention is that the next
    /// pointer points to this constellation self (to distinguish it from
    /// nullptr).
    const constln_t* get_nontrivial_next() const  {  return nontrivial_next;  }

    /// \brief makes a constellation trivial (i. e. removes it from the
    /// respective list)
    /// \details This member function only works if the constellation is the
    /// first one in the list (which will normally be the case).
    void make_trivial()
    {
        assert(nontrivial_first == this);
        nontrivial_first = nontrivial_next == this ? nullptr : nontrivial_next;
        nontrivial_next = nullptr;
    }

    /// \brief makes a constellation non-trivial (i. e. inserts it into the
    /// respective list)
    void make_nontrivial()
    {
        if (nullptr == nontrivial_next)
        {
            nontrivial_next = nullptr == nontrivial_first ? this
                                                          : nontrivial_first;
            nontrivial_first = this;
        }
    }

    /// \brief returns true iff the constellation is trivial
    /// \details If this constellation is the last in the list of non-trivial
    /// constellations, the convention is that the next pointer points to this
    /// constellation itself (to distinguish it from nullptr).
    bool is_trivial() const
    {
        return nullptr == nontrivial_next;
    }

    /// \brief constant iterator to the first state in the constellation
    permutation_const_iter_t begin() const  {  return int_begin;  }
    /// \brief iterator to the first state in the constellation
    permutation_iter_t begin()  {  return int_begin;  }
    /// \brief set the iterator to the first state in the constellation
    void set_begin(permutation_iter_t new_begin)
    {
        int_begin = new_begin;
        assert(int_begin < int_end);
    }

    /// \brief constant iterator past the last state in the constellation
    permutation_const_iter_t end() const  {  return int_end;  }
    /// \brief iterator past the last state in the constellation
    permutation_iter_t end()  {  return int_end;  }
    /// \brief set the iterator past the last state in the constellation
    void set_end(permutation_iter_t new_end)
    {
        int_end = new_end;
        assert(int_begin < int_end);
    }

    /// \brief compares two constellations for ordering them
    /// \details The constellations are ordered according to their positions in
    /// the permutation array.  This is a suitable order, as constellations may
    /// be refined, but never swap positions as a whole.  Refining will make
    /// the new subconstellations compare in the same way to other
    /// constellations as the original, larger constellation.
    bool operator<(const constln_t& other) const
    {
        return begin() < other.begin();
    }
    bool operator> (const constln_t& other) const  {  return other < *this;  }
    bool operator<=(const constln_t& other) const  { return !(other < *this); }
    bool operator>=(const constln_t& other) const  { return !(*this < other); }

    /// \brief split off a single block from the constellation
    /// \details The function splits the current constellation after its first
    /// block or before its last block, whichever is smaller.  It creates a new
    /// constellation for the split-off block and returns a pointer to the
    /// block.
    block_t* split_off_small_block()
    {
        assert(begin() < end());
        block_t* const FirstB = (*begin())->block;
        block_t* const LastB = end()[-1]->block;
        assert(FirstB != LastB);
        if (FirstB->end() == LastB->begin())  make_trivial();

        assert(FirstB->constln() == this);
        assert(LastB->constln() == this);
        assert(postprocess_begin == postprocess_end);
        // 2.6: Create a new constellation NewC and ...
        constln_t* const NewC = new constln_t(begin(), end(), postprocess_end);

        // 2.5: Choose a small splitter block SpB subset of SpC from P,
        //      i.e. |SpB| <= 1/2*|SpC|
        /// It doesn't matter very much how ties are resolved here:
        /// `part_tr.change_to_C()` is faster if the first block is selected to
        /// be split off.  `part_tr.split_s_inert_out()` is faster if the last
        /// block is selected.
        if (FirstB->size() > LastB->size())
        {
            // 2.6: ... and move SpB from SpC to NewC
            set_end(LastB->begin());
            NewC->set_begin(end());
            LastB->set_constln(NewC);
            return LastB;
        }
        else
        {
            // 2.6: ... and move SpB from SpC to NewC
            set_begin(FirstB->end());
            NewC->set_end(begin());
            FirstB->set_constln(NewC);
            return FirstB;
        }
    }

#ifndef NDEBUG
    /// \brief print a constellation identification for debugging
    std::string debug_id() const
    {
        return "constellation [" +
            std::to_string(begin() - block_t::permutation_begin()) + "," +
            std::to_string(end() - block_t::permutation_begin()) + ")";
    }
#endif
};



template <class LTS_TYPE>
class bisim_partitioner_gjkw_initialise_helper;

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

  private:
    /// \brief array with all other information about states
    /// \details We allocate 1 additional ``state'' to allow for the iterators
    /// past the last transition, as described in the documentation of
    /// `state_info_entry`.
    fixed_vector<state_info_entry> state_info;

    template <class LTS_TYPE>
    friend class bisim_partitioner_gjkw_initialise_helper;
  public:
    /// \brief constructor
    /// \details The constructor allocates memory, but does not actually
    /// initialise the partition.  Immediately afterwards, the initialisation
    /// helper `bisim_partitioner_gjkw_initialise_helper::init_transitions()`
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
        #endif
    }

    /// \brief destructor
    /// \details The destructor assumes that the caller has already executed
    /// `clear()` to deallocate the memory for the partition.
    ~part_state_t()
    {
        assert(state_info.empty());
        assert(permutation.empty());
    }
  private:
    static const char delete_constellations[];
    static const char delete_blocks[];
  public:
    /// \brief deallocates constellations and blocks
    /// \details This function can be called shortly before destructing the
    /// partition.  Afterwards, the data structure is in a unusable state,
    /// as all information on states, blocks and constellations is deleted and
    /// deallocated.
    void clear()
    {
        // We have to deallocate constellations first because deallocating
        // blocks makes the constellations inaccessible.
        permutation_iter_t permutation_iter = permutation.end();
        while (permutation.begin() != permutation_iter)
        {
            bisim_gjkw::check_complexity::count(delete_constellations, 1,
                                              bisim_gjkw::check_complexity::n);
            constln_t* const C = permutation_iter[-1]->constln();
            // permutation_iter[-1]->block->set_constln(nullptr);
            assert(C->end() == permutation_iter);
            // assert that constellation is trivial:
            assert(permutation_iter[-1]->block->begin() == C->begin());
            permutation_iter = C->begin();
            delete C;
        }

        #ifndef NDEBUG
            state_type deleted_blocks = 0;
        #endif
        permutation_iter = permutation.end();
        while (permutation.begin() != permutation_iter)
        {
            bisim_gjkw::check_complexity::count(delete_blocks, 1,
                                              bisim_gjkw::check_complexity::n);
            block_t* const B = permutation_iter[-1]->block;
            assert(B->end() == permutation_iter);
            permutation_iter = B->begin();
            #ifndef NDEBUG
                if (BLOCK_NO_SEQNR != B->seqnr())
                {
                    ++deleted_blocks;
                }
                else
                {
                    assert(0 == deleted_blocks);
                }
            #endif
            delete B;
        }
        #ifndef NDEBUG
            assert(deleted_blocks == block_t::nr_of_blocks);
        #endif
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

/* pred_entry, succ_entry, and B_to_C_entry contain the data that is stored
about a transition.  Every transition has one of each data structure; the three
structures are linked through the iterators (used here as pointers). */
class succ_entry
{
  public:
    B_to_C_iter_t B_to_C;
    state_info_ptr target;
    out_descriptor* constln_slice;
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
#endif
};


class B_to_C_entry
{
  public:
    pred_iter_t pred;
    B_to_C_desc_iter_t B_to_C_slice;
};


/* out_descriptor and B_to_C_descriptor are data types that indicate which
slice of states belongs together. */
class out_descriptor
{
  private:
    succ_iter_t int_end, int_begin;
  public:

    out_descriptor(succ_iter_t iter)
      : int_end(iter),
        int_begin(iter)
    {
        // assert(int_begin <= int_end);
    }

    state_type size() const
    {
        return int_end - int_begin;
    }

    succ_iter_t begin()
    {
        return int_begin;
    }
    succ_const_iter_t begin() const
    {
        return int_begin;
    }
    void set_begin(succ_iter_t new_begin)
    {
        int_begin = new_begin;
        assert(int_begin <= int_end);
    }
    succ_iter_t end()
    {
        return int_end;
    }
    succ_const_iter_t end() const
    {
        return int_end;
    }
    void set_end(succ_iter_t new_end)
    {
        int_end = new_end;
        assert(int_begin <= int_end);
    }
};


class B_to_C_descriptor
{
  public:
    B_to_C_iter_t end, begin;

    B_to_C_descriptor(B_to_C_iter_t begin_, B_to_C_iter_t end_)
      : end(end_),
        begin(begin_)
    {  }

    /// compute the source block of the transitions in this slice
    const block_t* from_block() const
    {
        assert(begin < end);
        assert(begin->pred->succ->B_to_C == begin);
        return begin->pred->source->block;
    }
    block_t* from_block()
    {
        assert(begin < end);
        assert(begin->pred->succ->B_to_C == begin);
        return begin->pred->source->block;
    }

    /// compute the goal constellation of the transitions in this slice
    const constln_t* to_constln() const
    {
        assert(begin < end);
        assert(begin->pred->succ->B_to_C == begin);
        return begin->pred->succ->target->constln();
    }
    constln_t* to_constln()
    {
        assert(begin < end);
        assert(begin->pred->succ->B_to_C == begin);
        return begin->pred->succ->target->constln();
    }

    /// \brief returns true iff the slice is marked for postprocessing
    /// \details The function uses the data registered with the goal
    /// constellation.
    bool needs_postprocessing() const
    {
        assert(to_constln()->postprocess_end <= begin ||
                                         end <= to_constln()->postprocess_end);
        assert(to_constln()->postprocess_begin <= begin ||
                                       end <= to_constln()->postprocess_begin);
        return to_constln()->postprocess_begin <= begin &&
                                          end <= to_constln()->postprocess_end;
    }

#ifndef NDEBUG
    /// \brief print a B_to_C slice identification for debugging
    /// \details This function is only available if compiled in Debug mode.
    std::string debug_id() const
    {
        assert(begin < end);
        std::string result("slice containing transition");
        if (end - begin > 1)
            result += "s ";
        else
            result += " ";
        B_to_C_const_iter_t iter = begin;
        assert(iter->pred->succ->B_to_C == iter);
        result += iter->pred->debug_id_short();
        if (end - iter > 4)
        {
            assert(iter[1].pred->succ->B_to_C == iter+1);
            result += ", ";
            result += iter[1].pred->debug_id_short();
            result += ", ...";
            iter = end - 3;
        }
        while (++iter != end)
        {
            assert(iter->pred->succ->B_to_C == iter);
            result += ", ";
            result += iter->pred->debug_id_short();
        }
        return result;
    }
#endif
};


/* part_trans_t collects and organises all data for the transitions. */
class part_trans_t
{
  private:
    fixed_vector<pred_entry> pred;
    fixed_vector<succ_entry> succ;
    fixed_vector<B_to_C_entry> B_to_C;

    template <class LTS_TYPE>
    friend class bisim_partitioner_gjkw_initialise_helper;

    void swap_in(B_to_C_iter_t const pos1, B_to_C_iter_t const pos2)
    {
        assert(B_to_C.end() > pos1 && pos1->pred->succ->B_to_C == pos1);
        assert(B_to_C.end() > pos2 && pos2->pred->succ->B_to_C == pos2);

        // swap contents
        pred_entry const temp_entry(*pos1->pred);
        *pos1->pred = *pos2->pred;
        *pos2->pred = temp_entry;
        // swap pointers to contents
        pred_iter_t const temp_iter(pos1->pred);
        pos1->pred = pos2->pred;
        pos2->pred = temp_iter;

        assert(B_to_C.end() > pos1 && pos1->pred->succ->B_to_C == pos1);
        assert(B_to_C.end() > pos2 && pos2->pred->succ->B_to_C == pos2);
    }

    void swap_out(pred_iter_t const pos1, pred_iter_t const pos2)
    {
        assert(pred.end() > pos1 && pos1->succ->B_to_C->pred == pos1);
        assert(pred.end() > pos2 && pos2->succ->B_to_C->pred == pos2);

        // swap contents
        succ_entry const temp_entry(*pos1->succ);
        *pos1->succ = *pos2->succ;
        *pos2->succ = temp_entry;
        // swap pointers to contents
        succ_iter_t const temp_iter(pos1->succ);
        pos1->succ = pos2->succ;
        pos2->succ = temp_iter;

        assert(pred.end() > pos1 && pos1->succ->B_to_C->pred == pos1);
        assert(pred.end() > pos2 && pos2->succ->B_to_C->pred == pos2);
    }

    // *pos1 -> *pos2 -> *pos3 -> *pos1
    void swap3_out(pred_iter_t const pos1, pred_iter_t const pos2,
                                                        pred_iter_t const pos3)
    {
        assert(pred.end() > pos1 && pos1->succ->B_to_C->pred == pos1);
        assert(pred.end() > pos2 && pos2->succ->B_to_C->pred == pos2);
        assert(pred.end() > pos3 && pos3->succ->B_to_C->pred == pos3);

        assert(pos1 != pos2 || pos1 == pos3);
        // swap contents
        succ_entry const temp_entry(*pos1->succ);
        *pos1->succ = *pos3->succ;
        *pos3->succ = *pos2->succ;
        *pos2->succ = temp_entry;
        // swap pointers to contents
        succ_iter_t const temp_iter(pos2->succ);
        pos2->succ = pos3->succ;
        pos3->succ = pos1->succ;
        pos1->succ = temp_iter;

        assert(pred.end() > pos1 && pos1->succ->B_to_C->pred == pos1);
        assert(pred.end() > pos2 && pos2->succ->B_to_C->pred == pos2);
        assert(pred.end() > pos3 && pos3->succ->B_to_C->pred == pos3);
    }

    void swap_B_to_C(succ_iter_t const pos1, succ_iter_t const pos2)
    {
        assert(succ.end() > pos1 && pos1->B_to_C->pred->succ == pos1);
        assert(succ.end() > pos2 && pos2->B_to_C->pred->succ == pos2);

        // swap contents
        B_to_C_entry const temp_entry(*pos1->B_to_C);
        *pos1->B_to_C = *pos2->B_to_C;
        *pos2->B_to_C = temp_entry;
        // swap pointers to contents
        B_to_C_iter_t const temp_iter(pos1->B_to_C);
        pos1->B_to_C = pos2->B_to_C;
        pos2->B_to_C = temp_iter;

        assert(succ.end() > pos1 && pos1->B_to_C->pred->succ == pos1);
        assert(succ.end() > pos2 && pos2->B_to_C->pred->succ == pos2);
    }

    // *pos1 -> *pos2 -> *pos3 -> *pos1
    void swap3_B_to_C(succ_iter_t const pos1, succ_iter_t const pos2,
                                                        succ_iter_t const pos3)
    {
        assert(succ.end() > pos1 && pos1->B_to_C->pred->succ == pos1);
        assert(succ.end() > pos2 && pos2->B_to_C->pred->succ == pos2);
        assert(succ.end() > pos3 && pos3->B_to_C->pred->succ == pos3);

        assert(pos1 != pos2 || pos1 == pos3);
        // swap contents
        B_to_C_entry const temp_entry(*pos1->B_to_C);
        *pos1->B_to_C = *pos3->B_to_C;
        *pos3->B_to_C = *pos2->B_to_C;
        *pos2->B_to_C = temp_entry;
        // swap pointers to contents
        B_to_C_iter_t const temp_iter(pos2->B_to_C);
        pos2->B_to_C = pos3->B_to_C;
        pos3->B_to_C = pos1->B_to_C;
        pos1->B_to_C = temp_iter;

        assert(succ.end() > pos1 && pos1->B_to_C->pred->succ == pos1);
        assert(succ.end() > pos2 && pos2->B_to_C->pred->succ == pos2);
        assert(succ.end() > pos3 && pos3->B_to_C->pred->succ == pos3);
    }
  public:
    part_trans_t(trans_type m)
      : pred(m),
        succ(m),
        B_to_C(m)
    {  }
    ~part_trans_t()
    {
        assert(B_to_C.empty());
        assert(succ.empty());
        assert(pred.empty());
    }

    /// clear allocated memory
    void clear()
    {
        // B_to_C_descriptors are deallocated when their respective lists are
        // deallocated by destructing the blocks.
        // out_descriptors do not need to be deallocated individually, but they
        // are cleared by deleting the constln_slice_pool.
//        constln_slice_pool.clear();
        B_to_C.clear();
        succ.clear();
        pred.clear();
    }

    trans_type trans_size() const  {  return pred.size();  }

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
    succ_iter_t change_to_C(pred_iter_t pred_iter, constln_t* OldC,
                            constln_t* NewC, bool first_transition_of_state,
                                            bool first_transition_of_block);

    /* split_s_inert_out splits the outgoing transitions from s to its own
    constellation into two:  the inert transitions become transitions to the
    new constellation of which s is now part;  the non-inert transitions remain
    transitions to OldC.  It returns the boundary between transitions to
    OldC and transitions to NewC in the outgoing transition array of s.
    Its time complexity is O(1 + min { |out_\nottau(s)|, |out_\tau(s)| }). */
    succ_iter_t split_s_inert_out(state_info_ptr s, constln_t* OldC);

    /* part_trans_t::make_noninert makes the transition identified by succ_iter
    noninert. */
    void make_noninert(succ_iter_t const succ_iter)
    {
        // change B_to_C
        B_to_C_iter_t const other_B_to_C =
                    succ_iter->B_to_C->pred->source->block->inert_begin();
        assert(succ_iter->B_to_C->B_to_C_slice->begin <= other_B_to_C);
        assert(other_B_to_C <= succ_iter->B_to_C);
        assert(succ_iter->B_to_C < succ_iter->B_to_C->B_to_C_slice->end);
        swap_B_to_C(succ_iter, other_B_to_C->pred->succ);
        succ_iter->B_to_C->pred->source->block->set_inert_begin(other_B_to_C +
                                                                            1);
        // change pred
        pred_iter_t const other_pred = succ_iter->target->inert_pred_begin();
        assert(succ_iter->target->pred_begin() <= other_pred);
        assert(other_pred <= succ_iter->B_to_C->pred);
        assert(succ_iter->B_to_C->pred < succ_iter->target->pred_end());
        swap_in(succ_iter->B_to_C, other_pred->succ->B_to_C);
        succ_iter->target->set_inert_pred_begin(other_pred + 1);
        // change succ
        succ_iter_t const other_succ =
                        succ_iter->B_to_C->pred->source->inert_succ_begin();
        assert(succ_iter->B_to_C->pred->source->succ_begin() <= other_succ);
        assert(other_succ <= succ_iter);
        assert(succ_iter < succ_iter->B_to_C->pred->source->succ_end());
        swap_out(succ_iter->B_to_C->pred, other_succ->B_to_C->pred);
        succ_iter->B_to_C->pred->source->set_inert_succ_begin(other_succ + 1);
    }

    /* part_trans_t::new_block_created splits the B_to_C-slices to reflect that
    some transitions now start in the new block NewB.  They can no longer be in
    the same slice as the transitions that start in the old block.
    
    We need separate functions for blue and red blocks because the B_to_C-slice
    of the red block should come after the B_to_C-slice of the blue block,
    at least while postprocessing.

    Its time complexity is O(1 + |out(NewB)|). */
    void new_blue_block_created(block_t* OldB, block_t* NewB, bool primary);
    void new_red_block_created(block_t* OldB, block_t* NewB, bool primary);

    B_to_C_const_iter_t B_to_C_begin() const  {  return B_to_C.begin();  }
    B_to_C_iter_t       B_to_C_end  ()        {  return B_to_C.end  ();  }
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
    state_type nr_of_states;
    const state_type orig_nr_of_states;
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



struct refine_shared_t;

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
  public:
    // The constructor constructs the data structures and immediately
    // calculates the bisimulation quotient.  However, it does not change the
    // LTS.
    bisim_partitioner_gjkw(LTS_TYPE& l, bool branching = false,
                                        bool preserve_divergence = false)
      : init_helper(l, branching, preserve_divergence),
        part_st(init_helper.get_nr_of_states()),
        part_tr(init_helper.get_nr_of_transitions())
    {
        assert(branching || !preserve_divergence);
        create_initial_partition_gjkw(branching, preserve_divergence);
        refine_partition_until_it_becomes_stable_gjkw();
    }
    ~bisim_partitioner_gjkw()
    {
        part_tr.clear();
        part_st.clear();
        bisim_gjkw::check_complexity::stats();
    }

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
        return bisim_gjkw::block_t::nr_of_blocks;
    }
    state_type get_eq_class(state_type s) const
    {
        return part_st.block(s)->seqnr();
    }
    bool in_same_class(state_type s, state_type t) const
    {
        return part_st.block(s) == part_st.block(t);
    }
  private:

    /*-------- dbStutteringEquivalence -- Algorithm 2 of [GJKW 2017] --------*/

    void create_initial_partition_gjkw(bool branching,
                                                     bool preserve_divergence);
    void refine_partition_until_it_becomes_stable_gjkw();

    /*---------------- Refine -- Algorithms 3 of [GJKW 2017] ----------------*/

    bisim_gjkw::block_t* refine(bisim_gjkw::block_t* RfnB,
                        const bisim_gjkw::constln_t* SpC,
                        const bisim_gjkw::B_to_C_descriptor* FromRed,
                        bool postprocessing);

    DECLARE_COROUTINE(refine_blue,
    /* formal parameters:*/ ((bisim_gjkw::block_t* const, RfnB))
                            ((const bisim_gjkw::constln_t* const, SpC))
                            ((bool const, postprocessing)),
    /* local variables:  */ ((bisim_gjkw::permutation_iter_t, visited_end))
                            ((bisim_gjkw::state_info_ptr, s))
                            ((bisim_gjkw::pred_iter_t, pred_iter))
                            ((bisim_gjkw::state_info_ptr, s_prime))
                            ((bisim_gjkw::permutation_iter_t,
                                                           blue_nonbottom_end))
                            ((bisim_gjkw::succ_const_iter_t, begin))
                            ((bisim_gjkw::succ_const_iter_t, end)),
    /* shared data:      */ struct bisim_gjkw::refine_shared_t, shared_data,
    /* interrupt locatns:*/ (REFINE_BLUE_PREDECESSOR_HANDLED)
                            (REFINE_BLUE_TESTING)
                            (REFINE_BLUE_STATE_HANDLED)
                            (REFINE_BLUE_COLLECT_BOTTOM));

    DECLARE_COROUTINE(refine_red,
    /* formal parameters:*/ ((bisim_gjkw::block_t* const, RfnB))
                            ((const bisim_gjkw::B_to_C_descriptor* const,
                                                                      FromRed))
                            ((bool const, postprocessing)),
    /* local variables:  */ ((bisim_gjkw::B_to_C_iter_t,fromred_visited_begin))
                            ((bisim_gjkw::permutation_iter_t, visited_begin))
                            ((bisim_gjkw::state_info_ptr, s))
                            ((bisim_gjkw::pred_iter_t, pred_iter)),
    /* shared data:      */ struct bisim_gjkw::refine_shared_t, shared_data,
    /* interrupt locatns:*/ (REFINE_RED_COLLECT_FROMRED)
                            (REFINE_RED_PREDECESSOR_HANDLED)
                            (REFINE_RED_STATE_HANDLED));

    /*--------- PostprocessNewBottom -- Algorithm 4 of [GJKW 2017] ----------*/

    bisim_gjkw::block_t* postprocess_new_bottom(bisim_gjkw::block_t* RedB);
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
void bisimulation_reduce_gjkw(LTS_TYPE& l, bool const branching /* = false */,
                                  bool const preserve_divergence /* = false */)
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
  l.set_num_states(bisim_part.num_eq_classes());
  //mCRL2log(log::debug, "bisim_gjkw") << "number of states in the lumped "
  //    "chain: " << bisim_part.num_eq_classes()
  //    << "; initial state: originally state " << l.initial_state()
  //    <<" = lumped state "<<bisim_part.get_eq_class(l.initial_state())<<"\n";
  l.set_initial_state(bisim_part.get_eq_class(l.initial_state()));
  bisim_part.replace_transitions(branching, preserve_divergence);
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





/* ************************************************************************* */
/*                                                                           */
/*                       I M P L E M E N T A T I O N S                       */
/*                                                                           */
/* ************************************************************************* */





// This section contains implementations of functions that refer to details of
// classes defined later, so they could not be defined inline.

namespace bisim_gjkw
{

inline void state_info_entry::set_current_constln(succ_iter_t const
                                                           new_current_constln)
{
    int_current_constln = new_current_constln;
    // current_constln points to a successor transition of this state:
    assert(succ_begin() <= int_current_constln);
    assert(int_current_constln <= succ_end());
    // it points to a place where a constellation slice starts or ends:
    assert(succ_begin() == int_current_constln ||
        succ_end() == int_current_constln ||
        int_current_constln[-1].constln_slice !=
                                           int_current_constln->constln_slice);
    // it points to the relevant constellation:
    // The following assertions cannot be executed immediately after each call.
    // assert(succ_begin() == current_constln() ||
    //                       *current_constln()[-1].target->constln() <= *SpC);
    // assert(succ_end() == current_constln() ||
    //                          *SpC <= *current_constln()->target->constln());
}


inline void state_info_entry::set_inert_succ_begin(succ_iter_t const
                                                           new_inert_out_begin)
{
    if (new_inert_out_begin > inert_succ_begin())
    {
        assert(*new_inert_out_begin[-1].target->constln() <= *constln());
        assert(new_inert_out_begin[-1].target->block != block);
    }
    else if (new_inert_out_begin < inert_succ_begin())
    {
        assert(new_inert_out_begin->target->block == block);
    }
    state_inert_out_begin = new_inert_out_begin;
    assert(succ_begin() <= inert_succ_begin());
    assert(inert_succ_begin() <= inert_succ_end());
}


inline void state_info_entry::set_inert_succ_end(succ_iter_t const
                                                             new_inert_out_end)
{
    state_inert_out_end = new_inert_out_end;
    assert(inert_succ_begin() <= inert_succ_end());
    assert(inert_succ_end() <= succ_end());
    assert(inert_succ_begin() == inert_succ_end() ||
                                  inert_succ_end()[-1].target->block == block);
    assert(succ_end() == inert_succ_end() ||
                            *constln() < *inert_succ_end()->target->constln());
}


/// get the constellation of the state
inline const constln_t* state_info_entry::constln() const
{
    return block->constln();
}

inline constln_t* state_info_entry::constln()
{
    return block->constln();
}

/// \brief constructor
/// \details The constructor initialises the block to: all states are
/// bottom states, no state is marked, the block is not refinable.
/// \param constln_ constellation to which the block belongs
/// \param begin_   initial iterator to the first state of the block
/// \param end_     initial iterator past the last state of the block
inline block_t::block_t(constln_t* const constln_,
                permutation_iter_t const begin_, permutation_iter_t const end_)
  : int_end(end_),
    int_begin(begin_),
    int_marked_nonbottom_begin(begin_), // no non-bottom state is marked
    int_bottom_begin(begin_), // all states are bottom states
    int_marked_bottom_begin(end_), // no bottom state is marked
    // int_inert_begin -- is initialised by part_trans_t::create_new_block
    // int_inert_end -- is initialised by part_trans_t::create_new_block
    to_constln(), // empty list
    int_constln(constln_),
    refinable_next(nullptr),
    int_seqnr(BLOCK_NO_SEQNR)
{
    assert(int_begin <= int_marked_nonbottom_begin);
    assert(int_marked_nonbottom_begin <= int_bottom_begin);
    assert(int_bottom_begin <= int_marked_bottom_begin);
    assert(int_marked_bottom_begin <= int_end);
    assert(int_bottom_begin < int_end);
    assert(int_constln->begin() <= int_begin && int_end <= int_constln->end());
}


inline void block_t::set_constln(constln_t* const new_constln)
{
    int_constln = new_constln;
    assert(nullptr == int_constln ||
         (int_constln->begin() <= int_begin && int_end <= int_constln->end()));
}


/// read FromRed
inline B_to_C_descriptor* block_t::FromRed(const constln_t* const SpC)
{
    if (to_constln.begin() != to_constln.end() &&
                                       to_constln.begin()->to_constln() == SpC)
    {
        return &*to_constln.begin();
    }
    else
    {
        return nullptr;
    }
}


/// set FromRed to an existing element in to_constln
inline void block_t::SetFromRed(B_to_C_desc_iter_t const new_fromred)
{
    assert(to_constln.begin() != to_constln.end());
    if (to_constln.begin() != new_fromred)
    {
        to_constln.splice(to_constln.begin(), to_constln, new_fromred);
    }
    assert(new_fromred->from_block() == this);
}


/// \brief quick check to find out whether the state has a transition to `SpC`
/// \details If the current constellation pointer happens to be set to `SpC` or
/// its successor, the function can quickly find out whether the state has a
/// transition to `SpC`.
/// The function should not be called for the constellation in which the state
/// resides.
/// \param SpC constellation of interest
/// \returns true if the state is known to have a transition to `SpC`
/// \memberof state_info_entry
inline bool state_info_entry::surely_has_transition_to(const constln_t* const
                                                                     SpC) const
{
    assert(succ_begin() <= current_constln() && current_constln()<=succ_end());
    assert(succ_begin()==current_constln() || succ_end()==current_constln() ||
                            *current_constln()[-1].target->constln() <
                                        *current_constln()->target->constln());
    assert(constln() != SpC);
    // either current_constln()->target or current_constln()[-1].target is in
    // SpC
    if (current_constln() != succ_end() &&
                                   current_constln()->target->constln() == SpC)
    {
        return true;
    }
    if (current_constln() != succ_begin() &&
                                current_constln()[-1].target->constln() == SpC)
    {
        return true;
    }
    return false;
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
inline bool state_info_entry::surely_has_no_transition_to(
                                              const constln_t* const SpC) const
{
    assert(succ_begin() <= current_constln() && current_constln()<=succ_end());
    assert(succ_begin()==current_constln() || succ_end()==current_constln() ||
                            *current_constln()[-1].target->constln() <
                                        *current_constln()->target->constln());
    assert(constln() != SpC);
    // condition:
    // current_constln()->target is in a constellation > SpC and
    // current_constln()[-1].target is in a constellation < SpC.
    if (current_constln() != succ_end() &&
                                 *current_constln()->target->constln() <= *SpC)
    {
        return false;
    }
    if (current_constln() != succ_begin() &&
                              *SpC <= *current_constln()[-1].target->constln())
    {
        return false;
    }
    return true;
}


} // end namespace bisim_gjkw
} // end namespace detail
} // end namespace lts
} // end namespace mcrl2

#endif // ifndef _LIBLTS_BISIM_GJKW_H
