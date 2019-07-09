// Author(s): David N. Jansen, Radboud Universiteit, Nijmegen, The Netherlands
//
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/// \file lts/detail/check_complexity.h
///
/// \brief helper class for time complexity checks during test runs
///
/// \details We use the class in this file to check whether the overall time
/// complexity fits in O(m log n).  Although it is difficult to test this in
/// general because of the constant factor in the definition of the O()
/// notation, it is often possible to give a (rather tight) upper bound on the
/// number of iterations of most loops.
///
/// The principle of time measurement with this file is:  the work done in
/// every loop body is assigned to a state or a transition.  Every state and
/// every transition gets a counter for every loop body which is assigned
/// to it.  When the loop body is executed, the corresponding counter is
/// increased.  When increasing a counter, a new value is assigned, based on
/// the logarithm of the size of the corresponding block (or constellation).
/// If the new value is not actually larger than the old one, an error is
/// raised.  The new value never is allowed to become larger than log2(n), so
/// we have:  For every counter, its value is increased at most log2(n) times,
/// and therefore no more than log2(n) steps (of a certain kind) can be
/// assigned to any single state or transition.
///
/// Note that an ``increase'' is by at least 1, but it may be more than 1.  If
/// one always increases to the maximal allowed value, it is ensured that a
/// very small block found early will only incur work that corresponds to its
/// own size (and not to the size of the block from which it was split off).
///
/// To assign work to some unit, write `mCRL2complexity(unit, add_work(counter
/// type, new counter value));`  The `unit` is a state or transition;  the
/// `counter type` is a value of `enum check_complexity::counter_type` defined
/// below; the `new counter value` typically is `check_complexity::log_n -
/// check_complexity::ilog2(block size)`.
///
/// For coroutines, there is an additional provision: work can temporarily be
/// recorded in some special counters.  As soon as it becomes clear which
/// coroutine (the one handling red states or the one handling blue states, in
/// our case) is faster, its counters are transferred to normal counters and
/// the counters of the other coroutine are cancelled.  It is checked that not
/// too many counters are cancelled.
///
/// To transfer work from a temporary to a normal counter, one uses
/// `finalise_work()`.  To cancel counters, use `cancel_work()`.  (The file
/// liblts_bisim_gjkw.cpp contains wrapper functions `blue_is_smaller()` and
/// `red_is_smaller()` that call `finalise_work()` and `cancel_work()`.)
/// After all temporary work has been handled, call `check_temporary_work()` to
/// compare the amount of sensible work with the amount of cancelled work.
///
/// If the work could be assigned to one of several counters (in particular, to
/// any one transition out of a set of transitions), I recommend to assign it
/// to all of them;  otherwise, it may happen that a later excess of the time
/// budget goes unnoticed because too few counters were advanced.
/// This, however, poses some difficulties when using temporary counters: a
/// single unit of work should be assigned to multiple counters, but added to
/// the balance between sensible and superfluous work only once.  A variant of
/// `add_work()`, namely `add_work_notemporary()`, can be called in that case:
/// it assigns a special value `DONT_COUNT_TEMPORARY` to a temporary counter
/// meaning that it should be disregarded in the calculation of the balance.
///
/// \author David N. Jansen, Radboud Universiteit, Nijmegen, The Netherlands

#ifndef _CHECK_COMPLEXITY_H
#define _CHECK_COMPLEXITY_H

#include <cstring>       // for std::size_t and std::memset()
#include <cassert>
#include <cmath>         // for std::log2()
#include <climits>       // for CHAR_BIT and SIZE_MAX

#include "mcrl2/utilities/logger.h"


namespace mcrl2
{
namespace lts
{
namespace detail
{

/// \brief type used to store state (numbers and) counts
/// \details defined here because this is the most basic #include header that
/// uses it.
typedef std::size_t state_type;
#define STATE_TYPE_MIN ((state_type) 0)
#define STATE_TYPE_MAX SIZE_MAX

/// \brief type used to store differences between state counters
typedef std::ptrdiff_t signed_state_type;

/// \brief type used to store transition (numbers and) counts
/// \details defined here because this is the most basic #include header that
/// uses it.
typedef std::size_t trans_type;
#define TRANS_TYPE_MIN ((trans_type) 0)
#define TRANS_TYPE_MAX SIZE_MAX

#ifndef NDEBUG

namespace bisim_gjkw
{

/// \brief class for time complexity checks
/// \details The class stores (as static members) the global counters needed
/// for checking time complexity budgets.
///
/// It could almost be defined as a namespace because all members are static.
/// Still, just a few members are private.
class check_complexity
{
  public:
    /// \brief Type for complexity budget counters
    /// \details The enumeration constants defined by this type are used to
    /// distinguish the many counters that the time budget check uses.
    /// There are counters assigned to blocks, states, B_to_C slices, and
    /// transitions.  Counters assigned to a block are regarded as if the work
    /// would be registered with every state in the block.  Counters assigned
    /// to a B_to_C slice are regarded as if the work would be registered with
    /// every transition in the slice.  (These conventions are important when
    /// splitting a block or a B_to_C slice.)
    enum counter_type
    {
        // block counters: every state in the block is regarded as visited.  In
        // this way, every state is ``visited'' O(log n) times.
        // Invariant: block->constln()->size() << (counter value) <= n
        while_C_contains_a_nontrivial_constellation_2_4 = 0,
                BLOCK_MIN = while_C_contains_a_nontrivial_constellation_2_4,

        // If a loop runs over every state of a block exactly once, we simplify
        // the task of updating the counters by making the corresponding loop
        // counter a block counter:
        for_all_s_in_SpB_2_10,
            // Invariant of the following block counters:
            // block->size() << (counter value) <= n
        Move_Blue_or_Red_to_a_new_block_NewB_pointer_3_29,
        Move_Blue_or_Red_to_a_new_block_states_3_29,
        for_all_s_in_NewB_3_31,
                BLOCK_MAX = for_all_s_in_NewB_3_31,

        // state counters: every state is visited O(log n) times
        // Invariant: s->block->size() << (counter value) <= n
        Move_Blue_or_Red_to_a_new_block_NewB_swap_3_29,
                STATE_MIN = Move_Blue_or_Red_to_a_new_block_NewB_swap_3_29,

        // The following counters are used when one refines a block:  the first
        // group is used to store the amount of work that (a posteriori) turns
        // out to be useful.  After that, there are two groups of counters to
        // store temporary work.
        refine_bottom_state_3_6l,
        refine_visited_state_3_15,

        // temporary state counters (blue):
        while_Test_is_not_empty_3_6l_s_is_blue_3_11l,
                STATE_MIN_TEMP = while_Test_is_not_empty_3_6l_s_is_blue_3_11l,
        while_Blue_contains_unvisited_states_3_15l,

        // temporary state counters (red):
        while_Red_contains_unvisited_states_3_15r,
                STATE_MAX_TEMP = while_Red_contains_unvisited_states_3_15r,

        // new bottom state counters: every state is visited once
        // Invariant: if s is a non-bottom state, the counter is 0;
        // otherwise, the counter is 0 or 1.
        for_all_bottom_states_s_in_RfnB_4_8,
            // the next counter is used to count the work done on a virtual
            // self-loop in line 4.15 (the new bottom state is regarded as red
            // because it is in the splitter, but there is no transition to the
            // splitter).
        for_all_old_bottom_states_s_in_RedB_selfloop_4_15,
                STATE_MAX = for_all_old_bottom_states_s_in_RedB_selfloop_4_15,

        // B_to_C_descriptor counters: every transition in the B_to_C-slice is
        // regarded as visited.  In this way, every transition is ``visited''
        // O(log n) times.
        // Invariant: to_constln()->size() << (counter value) <= n
        for_all_refinable_blocks_RfnB_2_20,
                B_TO_C_MIN = for_all_refinable_blocks_RfnB_2_20,

        // If a loop runs over every transition in a B_to_C slice exactly once,
        // we simplify the task of updating the counters by making the
        // corresponding loop counter a B_to_C counter:
        Register_that_inert_transitions_from_s_go_to_NewC_B_to_C_2_17,
        // the following counter is also meant for temporary work.
        // Sometimes, after separating the new bottom states from the old ones,
        // a constellation is reachable from the block of the new bottom
        // states, but only from non-bottom states in this block.  In that
        // case, it cannot yet be determined which state will be a new bottom
        // state.  Then, the work is assigned temporarily to the B_to_C slice,
        // until some new bottom state is found to which to assign it.
        for_all_constellations_C_not_in_R_from_RfnB_4_4,
               B_TO_C_MIN_TEMP=for_all_constellations_C_not_in_R_from_RfnB_4_4,
               B_TO_C_MAX_TEMP=for_all_constellations_C_not_in_R_from_RfnB_4_4,
               B_TO_C_MAX = for_all_constellations_C_not_in_R_from_RfnB_4_4,

        // transition counters: every transition is visited O(log n) times
            // counters for transitions into the splitter NewC
            // Invariant: target->constln()->size() << (counter value) <= n
        for_all_s_prime_in_pred_s_2_11,
                TRANS_MIN = for_all_s_prime_in_pred_s_2_11,
        Register_that_inert_transitions_from_s_go_to_NewC_succ_2_17,
        Register_that_inert_transitions_from_s_go_to_NewC_swap_2_17,
        refine_outgoing_transition_to_marked_state_3_6l,

            // counters for outgoing transitions
            // Invariant: source->block->size() << (counter value) <= n
        refine_outgoing_transition_3_6_or_23l,
        Move_Blue_or_Red_to_a_new_block_succ_3_29,
        for_all_s_prime_in_succ_s_3_32r,

            // counters for incoming transitions
            // Invariant: target->block->size() << (counter value) <= n
        refine_incoming_transition_3_18,
        for_all_s_prime_in_pred_s_3_32l,

        // temporary transition counters for refine: similar to the temporary
        // counters for states, we have a first group to store the work done
        // for the smaller half, and temporary counters to store the work until
        // it becomes clear which half wins.
            // Because we have to sort the transitions into those to NewC, the
            // outgoing and the incoming transitions, these counters are
            // distributed above.  The counters used to store the work done for
            // the smaller half are: refine_outgoing_transition_3_6_or_23l,
            // refine_outgoing_transition_to_marked_state_3_6l and
            // refine_incoming_transition_3_18.

        // temporary transition counters (blue):
        while_Test_is_not_empty_3_6l_s_is_red_3_9l,
                TRANS_MIN_TEMP = while_Test_is_not_empty_3_6l_s_is_red_3_9l,
        while_Test_is_not_empty_3_6l_s_is_red_3_9l_postprocessing,
        for_all_s_prime_in_pred_s_setminus_Red_3_18l,
            // The work in the following counter is assigned to red (new
            // bottom) states if the blue block is smaller!
        if___s_prime_has_transition_to_SpC_3_23l,

        // temporary transition counters (red):
        while_FromRed_is_not_empty_3_6r,
        for_all_s_prime_in_pred_s_3_18r,
                TRANS_MAX_TEMP = for_all_s_prime_in_pred_s_3_18r,

        // new bottom transition counters: every transition is visited once
        // Invariant: If source is a non-bottom state, the counter is 0;
        // otherwise, the counter is 0 or 1.
        refine_outgoing_transition_postprocess_new_bottom_3_6l,
        refine_outgoing_transition_from_new_bottom_3_23l,
            // For the following counters, we have an ``a priori'' and an ``a
            // posteriori'' variant.  The reason is, as explained with the
            // B_to_C slice counters, that sometimes a constellation is
            // reachable from the block of new bottom states but it is not yet
            // clear which of the source states will become a new bottom state.
            // In that case, the ``a posteriori'' counters are used.  Later,
            // the same block and the same constellation may be refined another
            // time, but now with known new bottom states; then, the ``a
            // priori'' counters are used.
        for_all_transitions_from_bottom_states_a_priori_4_4,
        for_all_transitions_from_bottom_states_a_posteriori_4_4,
        for_all_transitions_that_need_postproc_a_priori_4_12,
        for_all_transitions_that_need_postproc_a_posteriori_4_12,
        for_all_old_bottom_states_s_in_RedB_4_15,
                TRANS_MAX = for_all_old_bottom_states_s_in_RedB_4_15,

        /*-------------- counters for the bisim_dnj algorithm --------------*/

        // block counters
            // Block counters are used to assign some work to each state in the
            // block (and possibly, by transitivity, each incoming or outgoing
            // transition of the block).
        split_off_block,
                    BLOCK_dnj_MIN = split_off_block,
        adapt_transitions_for_new_block,
        create_initial_partition,
                    BLOCK_dnj_MAX = create_initial_partition,

        // state counters
            // If every state of a block is handled by some loop, we
            // abbreviate the counter to a block counter.
        refine__find_predecessors_of_red_or_blue_state,
                  STATE_dnj_MIN=refine__find_predecessors_of_red_or_blue_state,

        // temporary state counters (blue):
        refine_blue__find_predecessors_of_blue_state,
               STATE_dnj_MIN_TEMP=refine_blue__find_predecessors_of_blue_state,

        // temporary state counters (red):
        refine_red__find_predecessors_of_red_state,
                 STATE_dnj_MAX_TEMP=refine_red__find_predecessors_of_red_state,
        prepare_for_postprocessing,
                 STATE_dnj_MAX = prepare_for_postprocessing,

        // bunch counters (only for small bunches, i. e. bunches that have been
        // split off from a large bunch)
        refine_partition_until_it_becomes_stable__find_pred,
           BUNCH_dnj_MIN = refine_partition_until_it_becomes_stable__find_pred,
           BUNCH_dnj_MAX = refine_partition_until_it_becomes_stable__find_pred,

        // block_bunch-slice counters (only for block_bunch-slices that are
        // part of a small bunch)
        refine_partition_until_it_becomes_stable__stabilize,
                    BLOCK_BUNCH_dnj_MIN =
                           refine_partition_until_it_becomes_stable__stabilize,
        refine_partition_until_it_becomes_stable__stabilize_for_large_splitter,
        second_move_transition_to_new_bunch,
        prepare_for_postprocessing__make_unstable_temp,
                    BLOCK_BUNCH_dnj_MIN_TEMP =
                                prepare_for_postprocessing__make_unstable_temp,
                    BLOCK_BUNCH_dnj_MAX_TEMP =
                                prepare_for_postprocessing__make_unstable_temp,
                    BLOCK_BUNCH_dnj_MAX =
                                prepare_for_postprocessing__make_unstable_temp,

        // transition counters
            // If every transition of a state is handled by some loop, we
            // abbreviate the counter to a state counter (and possibly, by
            // transitivity, to a block counter).
        move_out_slice_to_new_block, // source block size
                    TRANS_dnj_MIN = move_out_slice_to_new_block,
        refine__handle_transition_from_red_or_blue_state, // source block size
        refine__handle_transition_to_red_or_blue_state, // target block size
        refine__handle_transition_in_FromRed, // bunch size

        // temporary transition counters (blue):
        refine_blue__handle_transition_in_FromRed, // bunch size
                  TRANS_dnj_MIN_TEMP=refine_blue__handle_transition_in_FromRed,
        refine_blue__handle_transition_to_blue_state, // target block size
        refine_blue__slow_test, // red: new bottom; blue (inert): block size

        // temporary transition counters (red):
        refine_red__handle_transition_from_red_state, // bunch size
        refine_red__handle_transition_to_red_state, // target block size
            TRANS_dnj_MAX_TEMP = refine_red__handle_transition_to_red_state,

        // transition counters for new bottom states:
        refine__slow_test_found_red_state,
        prepare_for_postprocessing__make_unstable_a_priori,
        prepare_for_postprocessing__make_unstable_a_posteriori,
        postprocess_new_noninert__sort,
        postprocess_new_noninert__stabilize_a_priori,
        postprocess_new_noninert__stabilize_a_posteriori,
            TRANS_dnj_MAX = postprocess_new_noninert__stabilize_a_posteriori
    };

    /// \brief special value for temporary work without changing the balance
    #define DONT_COUNT_TEMPORARY (std::numeric_limits<unsigned char>::max()-1)

    /// \brief value of floor(log2(n)) for easy access
    /// \details This variable has to be set by `init()` before counting work
    /// can begin.
    static unsigned char log_n;

    /// \brief calculate the base-2 logarithm, rounded down
    static int ilog2(state_type size)
    {
        #ifdef __GNUC__
            if (sizeof(unsigned) == sizeof(state_type))
            {
                return sizeof(unsigned) * CHAR_BIT - 1 - __builtin_clz(size);
            }
            if (sizeof(unsigned long) == sizeof(state_type))
            {
                return sizeof(unsigned long)*CHAR_BIT-1 - __builtin_clzl(size);
            }
            if (sizeof(unsigned long long) == sizeof(state_type))
            {
                return sizeof(unsigned long long) * CHAR_BIT - 1 -
                                                         __builtin_clzll(size);
            }
        #endif
        return (int) std::log2(size);
    }

  private:
    /// \brief counter to register the work balance for coroutines
    /// \details Sensible work will be counted positively, and cancelled work
    /// negatively.
    static signed_state_type sensible_work;

  public:
    /// \brief printable names of the counter types (for error messages)
    static const char *work_names[TRANS_dnj_MAX - BLOCK_MIN + 1];

    /// \brief check that not too much superfluous work has been done
    /// \details After having moved all temporary work counters to the normal
    /// counters, this function can be used to ensure that not too much
    /// temporary work is cancelled.
    static void check_temporary_work()
    {
        assert(-1 <= sensible_work);
        sensible_work = 0;
    }

    /// \brief subset of counters (to be associated with a state or transition)
    template <enum counter_type FirstCounter, enum counter_type LastCounter,
                  enum counter_type FirstTempCounter =
                                         (enum counter_type) (LastCounter + 1),
                  enum counter_type FirstPostprocessCounter = FirstTempCounter>
    class counter_t
    {
      public:
        /// \brief actual space to store the counters
        unsigned char counters[LastCounter - FirstCounter + 1];

        /// \brief cancel temporary work
        /// \details The function registers that all counters from `first` to
        /// `last` (inclusive) are counting superfluous work.  It adds them to
        /// the pool of superfluous work.
        /// \param ctr  temporary counter whose work is superfluous
        /// \returns false  iff some counter was too large.  In that case, also
        ///                 the beginning of an error message is printed.
        ///                 The function should be called through the macro
        ///                 `mCRL2complexity()`, because that macro will print
        ///                 the remainder of the error message as needed.
        bool cancel_work(enum counter_type const ctr)
        {
            assert(FirstTempCounter <= ctr);
            assert(ctr < FirstPostprocessCounter);
            if ((FirstTempCounter != TRANS_MIN_TEMP &&
                 FirstTempCounter != TRANS_dnj_MIN_TEMP) ||
                          DONT_COUNT_TEMPORARY != counters[ctr - FirstCounter])
            {
                assert(counters[ctr - FirstCounter] <= 1);
                sensible_work -= counters[ctr - FirstCounter];
            }
            counters[ctr - FirstCounter] = 0;
            return true;
        }


        /// \brief move temporary work to its final counter
        /// \details The function moves work from a temporary counter to a
        /// normal counter.  It also checks that the normal counter does not
        /// get too large.
        /// \param from       temporary counter from where work is moved
        /// \param to         normal counter to which work is moved
        /// \param max_value  maximal allowed value to the normal counter.  The
        ///                   old value of the counter should be strictly
        ///                   smaller.
        /// \returns false  iff the counter was too large.  In that case, also
        ///                 the beginning of an error message is printed.
        ///                 The function should be called through the macro
        ///                 `mCRL2complexity()`, because that macro will print
        ///                 the remainder of the error message as needed.
        bool finalise_work(enum counter_type const from,
                          enum counter_type const to, unsigned const max_value)
        {
            // assert(...) -- see move_work().
            if ((FirstTempCounter != TRANS_MIN_TEMP &&
                 FirstTempCounter != TRANS_dnj_MIN_TEMP) ||
                         DONT_COUNT_TEMPORARY != counters[from - FirstCounter])
            {
                sensible_work += counters[from - FirstCounter];
            }
            else
            {
                counters[from - FirstCounter] = 1;
            }
            return move_work(from, to, max_value);
        }


        /// \brief constructor, initializes all counters to 0
        counter_t()
        {
            assert(FirstCounter < FirstTempCounter);
            assert(FirstTempCounter <= FirstPostprocessCounter);
            assert(FirstPostprocessCounter <=
                                        (enum counter_type) (LastCounter + 1));
            std::memset(counters, '\0', sizeof(counters));
        }


        /// \brief register work with some counter
        /// \details The function increases a work counter to a larger value.
        /// It is also checked that the counter does not get too large.
        /// The function is normally called through the macro
        /// `mCRL2complexity()`.
        /// \param ctr        counter with which work is registered
        /// \param max_value  maximal allowed value of the counter.  The old
        ///                   value of the counter should be strictly smaller.
        /// \returns false  iff the counter was too large.  In that case, also
        ///                 the beginning of an error message is printed.
        ///                 The function should be called through the macro
        ///                 `mCRL2complexity()`, because that macro will print
        ///                 the remainder of the error message as needed.
        bool add_work(enum counter_type const ctr, unsigned const max_value)
        {
            assert(FirstCounter <= ctr);
            assert(ctr <= LastCounter);
            assert(max_value <= (ctr < FirstTempCounter ? log_n : 1U));
            if (counters[ctr - FirstCounter] >= max_value)
            {
                mCRL2log(log::error) << "Error 1: counter \""
                        << work_names[ctr - BLOCK_MIN] << "\" exceeded "
                                    "maximum value (" << max_value << ") for ";
                return false;
            }
            counters[ctr - FirstCounter] = max_value;
            return true;
        }


        /// \brief move temporary work to another counter
        /// \details The function moves work from a temporary counter to
        /// another temporary or a normal counter.  It also checks that the new
        /// counter does not get too large.
        /// The function is normally called through the macro
        /// `mCRL2complexity()`.
        /// \param from       temporary counter from where work is moved
        /// \param to         (temporary or normal) counter to which work is
        ///                   moved
        /// \param max_value  maximal allowed value to the normal counter.  The
        ///                   old value of the counter should be strictly
        ///                   smaller.
        /// \returns false  iff the new counter was too large.  In that case,
        ///                 also the beginning of an error message is printed.
        ///                 The function should be called through the macro
        ///                 `mCRL2complexity()`, because that macro will print
        ///                 the remainder of the error message as needed.
        bool move_work(enum counter_type const from,
                          enum counter_type const to, unsigned const max_value)
        {
            assert(FirstTempCounter <= from);
            assert(from < FirstPostprocessCounter);
            assert(FirstCounter <= to);
            assert(to <= LastCounter);
            assert(max_value <= (to < FirstTempCounter ? log_n : 1U));
            if (0 == counters[from - FirstCounter])  return true;
            if (counters[to - FirstCounter] >= max_value)
            {
                mCRL2log(log::error) << "Error 2: counter \""
                        << work_names[to - BLOCK_MIN] << "\" exceeded "
                                    "maximum value (" << max_value << ") for ";
                return false;
            }
            /*
            if ((FirstTempCounter == TRANS_MIN_TEMP ||
                 FirstTempCounter == TRANS_dnj_MIN_TEMP) &&
                    FirstTempCounter <= to && to < FirstPostprocessCounter &&
                         DONT_COUNT_TEMPORARY == counters[from - FirstCounter])
            {
                assert(1 == max_value);
                // the next assertion is always satisfied if 1 == max_value.
                // assert(0 == counters[to - FirstCounter]);
                counters[to - FirstCounter] = DONT_COUNT_TEMPORARY;
            }
            else
            */
            {
                counters[to - FirstCounter] = max_value;
                assert(1 == counters[from - FirstCounter]);
            }
            counters[from - FirstCounter] = 0;
            return true;
        }
    };


    // usage of the functions below: as soon as the block is refined, one
    // has to call state_counter_t::blue_is_smaller() or state_counter_t::
    // red_is_smaller() for every state in the old and new block.  Also, one
    // needs to call trans_counter_t::blue_is_smaller() or trans_counter_t::
    // red_is_smaller() for every incoming and outgoing transition in the old
    // and new block.  (Inert transitions need to be handled only once.)  After
    // that, one calls check_temporary_work() to verify that not too much
    // work was done on the larger block.

    /// \brief counters for a block
    /// \details The counters stored with a block are meant to be assigned to
    /// each state in the block.  This means that the counter values need to be
    /// copied when the block is split.
    class block_counter_t : public counter_t<BLOCK_MIN, BLOCK_MAX>
    {
      public:
        /// \brief ensures there is no orphaned temporary work counter
        /// \details When a refinement has finished, all work registered with
        /// temporary counters should have been moved to normal counters.  This
        /// function verifies this property.
        /// The function additionally ensures that no work counter exceeds its
        /// maximal allowed value, based on the size of the block or its
        /// constellation.  (The size of the constellation is the unit used for
        /// counters related to [blocks in the] splitter constellation;  the
        /// size of the block is used for other counters.)
        /// \param max_C  ilog2(n) - ilog2(size of constellation)
        /// \param max_B  ilog2(n) - ilog2(size of block)
        /// \returns false  iff some temporary counter was nonzero.  In that
        ///                 case, also the beginning of an error message is
        ///                 printed.  The function should be called through the
        ///                 macro `mCRL2complexity()`, because that macro will
        ///                 print the remainder of the error message as needed.
        bool no_temporary_work(unsigned const max_C, unsigned const max_B)
        {
            assert(max_C <= max_B);
            for (enum counter_type ctr = BLOCK_MIN;
                       ctr < Move_Blue_or_Red_to_a_new_block_NewB_pointer_3_29;
                                           ctr = (enum counter_type) (ctr + 1))
            {
                assert(counters[ctr - BLOCK_MIN] <= max_C);
                counters[ctr - BLOCK_MIN] = max_C;
            }
            assert(max_B <= log_n);
            for (enum counter_type ctr =
                         Move_Blue_or_Red_to_a_new_block_NewB_pointer_3_29;
                         ctr <= BLOCK_MAX; ctr = (enum counter_type) (ctr + 1))
            {
                assert(counters[ctr - BLOCK_MIN] <= max_B);
                counters[ctr - BLOCK_MIN] = max_B;
            }
            return true;
        }
    };

    /// \brief counters for a B_to_C slice
    /// \details The counters stored with a B_to_C slice are meant to be
    /// assigned to each transition in the slice.  This means that the counter
    /// values need to be copied when the slice is split.
    class B_to_C_counter_t : public counter_t<B_TO_C_MIN, B_TO_C_MAX,
                    B_TO_C_MIN_TEMP, (enum counter_type) (B_TO_C_MAX_TEMP + 1)>
    {
      public:
        /// \brief ensures there is no orphaned temporary work counter
        /// \details When a refinement has finished, all work registered with
        /// temporary counters should have been moved to normal counters.  This
        /// function verifies this property.
        /// The function additionally ensures that no work counter exceeds its
        /// maximal allowed value, based on the size of the target
        /// constellation.
        /// \param max_targetC  ilog2(n) - ilog2(size of target constellation)
        /// \returns false  iff some temporary counter was nonzero.  In that
        ///                 case, also the beginning of an error message is
        ///                 printed.  The function should be called through the
        ///                 macro `mCRL2complexity()`, because that macro will
        ///                 print the remainder of the error message as needed.
        bool no_temporary_work(unsigned const max_targetC)
        {
            assert(max_targetC <= log_n);
            for (enum counter_type ctr = B_TO_C_MIN;
                    ctr < B_TO_C_MIN_TEMP; ctr = (enum counter_type) (ctr + 1))
            {
                assert(counters[ctr - B_TO_C_MIN] <= max_targetC);
                counters[ctr - B_TO_C_MIN] = max_targetC;
            }
            for (enum counter_type ctr = B_TO_C_MIN_TEMP;
                   ctr <= B_TO_C_MAX_TEMP; ctr = (enum counter_type) (ctr + 1))
            {
                if (counters[ctr - B_TO_C_MIN] > 0)
                {
                    mCRL2log(log::error) << "Error 3: counter \""
                        << work_names[ctr - BLOCK_MIN] << "\" exceeded "
                                            "maximum value (" << 0 << ") for ";
                    return false;
                }
            }
            assert(B_TO_C_MAX_TEMP == B_TO_C_MAX);
            return true;
        }


        /// \brief returns the _temporary_ counter associated with line 4.4
        /// \details The counter associated with line 4.4 is used when some
        /// constellation is reachable from a block containing new bottom
        /// states but it is not yet clear which states are the new bottom
        /// states that can reach the constellation.  Then, the work is
        /// temporarily assigned to the B_to_C slice until it has become clear
        /// to which new bottom states it can be assigned.
        ///
        /// We cannot use the normal mechanism of `move_work()` here because
        /// the normal counters are transition counters, not B_to_C slice
        /// counters.
        ///
        /// This function helps to decide whether the work still needs to be
        /// moved from the temporary counter to a normal counter.
        /// \returns the value of the _temporary_ counter associated with
        ///          line 4.4
        unsigned char get_work_counter_4_4() const
        {
            return counters[for_all_constellations_C_not_in_R_from_RfnB_4_4 -
                                                                   B_TO_C_MIN];
        }


        /// \brief sets the temporary counter associated with line 4.4 to zero
        /// \details The counter associated with line 4.4 is needed when some
        /// constellation is reachable from a block containing new bottom
        /// states but it is not yet clear which states are the new bottom
        /// states that can reach the constellation.  Then, the work is
        /// temporarily assigned to the B_to_C slice until it has become clear
        /// to which new bottom states it has to be assigned.
        ///
        /// We cannot use the normal mechanism of `move_work()` here because
        /// the normal counters are transition counters, not B_to_C slice
        /// counters.
        ///
        /// This function resets the temporary work counter and is meant to be
        /// called as soon as the work can be assigned to normal counters.
        void reset_work_counter_4_4()
        {
            counters[for_all_constellations_C_not_in_R_from_RfnB_4_4 -
                                                               B_TO_C_MIN] = 0;
        }
    };
    class state_counter_t : public counter_t<STATE_MIN, STATE_MAX,
                      STATE_MIN_TEMP, (enum counter_type) (STATE_MAX_TEMP + 1)>
    {
      public:
        /// \brief ensures there is no orphaned temporary work counter
        /// \details When a refinement has finished, all work registered with
        /// temporary counters should have been moved to normal counters.
        /// Further, there should not be any work ascribed to bottom-state
        /// counters in non-bottom states, but only to (new) bottom states.
        /// This function verifies these properties.  It also sets all counters
        /// for bottom states to 1 so that later no more work can be assigned
        /// to them.
        /// The function additionally ensures that no work counter exceeds its
        /// maximal allowed value, based on the size of the block of which the
        /// state is a member.
        /// \param max_B    log2(n) - log2(size of the block containing this
        ///                 state)
        /// \param bottom   `true` iff the state to which these counters belong
        ///                 is a bottom state
        /// \returns false  iff some temporary counter or some bottom-state
        ///                 counter of a non-bottom state was nonzero.  In that
        ///                 case, also the beginning of an error message is
        ///                 printed.  The function should be called through the
        ///                 macro `mCRL2complexity()`, because that macro will
        ///                 print the remainder of the error message as needed.
        bool no_temporary_work(unsigned const max_B, bool const bottom)
        {
            assert(max_B <= log_n);
            for (enum counter_type ctr = STATE_MIN;
                     ctr < STATE_MIN_TEMP; ctr = (enum counter_type) (ctr + 1))
            {
                assert(counters[ctr - STATE_MIN] <= max_B);
                counters[ctr - STATE_MIN] = max_B;
            }

            // temporary state counters must be zero:
            for (enum counter_type ctr = STATE_MIN_TEMP;
                    ctr <= STATE_MAX_TEMP; ctr = (enum counter_type) (ctr + 1))
            {
                if (counters[ctr - STATE_MIN] > 0)
                {
                    mCRL2log(log::error) << "Error 4: counter \""
                        << work_names[ctr - BLOCK_MIN] << "\" exceeded "
                                            "maximum value (" << 0 << ") for ";
                    return false;
                }
            }
            // bottom state counters must be 0 for non-bottom states and 1 for
            // bottom states:
            // assert((unsigned) bottom <= 1);
            for(enum counter_type ctr = (enum counter_type) (STATE_MAX_TEMP+1);
                        ctr <= STATE_MAX ; ctr = (enum counter_type) (ctr + 1))
            {
                if (counters[ctr - STATE_MIN] > (unsigned) bottom)
                {
                    mCRL2log(log::error) << "Error 5: counter \""
                        << work_names[ctr - BLOCK_MIN] << "\" exceeded "
                            "maximum value (" << (unsigned) bottom << ") for ";
                    return false;
                }
                counters[ctr - STATE_MIN] = (unsigned) bottom;
            }
            return true;
        }
    };
    class trans_counter_t : public counter_t<TRANS_MIN, TRANS_MAX,
                      TRANS_MIN_TEMP, (enum counter_type) (TRANS_MAX_TEMP + 1)>
    {
      public:
        /// \brief ensures there is no orphaned temporary work counter
        /// \details When a refinement has finished, all work registered with
        /// temporary counters should have been moved to normal counters.
        /// Further, there should not be any work ascribed to bottom-state
        /// counters in transitions from non-bottom states, but only to
        /// transitions from (new) bottom states.  This function verifies these
        /// properties.  It also sets all counters for bottom states to 1 so
        /// that later no more work can be assigned to them.
        /// The function additionally ensures that no work counter exceeds its
        /// maximal allowed value, based on the size of the source block,
        /// target block or target constellation.  (The constellation size is
        /// the relevant unit for counters that are related to transitions into
        /// the splitter, which is the constellation `NewC`.  The block size is
        /// the unit for counters that are related to refinements.  Because
        /// some parts of a refinement look at incoming transitions of the
        /// refined block and others at outgoing transitions, we need two block
        /// sizes.)
        /// \param max_sourceB    the maximum allowed value for work counters
        ///                       based on the source state of the transition
        /// \param max_targetC    the maximum allowed value for work counters
        ///                       based on the target constellation
        /// \param max_targetB    the maximum allowed value for work counters
        ///                       based on the target block
        /// \param source_bottom  `true` iff the transition to which these
        ///                       counters belong starts in a bottom state
        /// \returns false  iff some temporary counter or some bottom-state
        ///                 counter of a transition with non-bottom source was
        ///                 nonzero.  In that case, also the beginning of an
        ///                 error message is printed.  The function should be
        ///                 called through the macro `mCRL2complexity()`,
        ///                 because that macro will print the remainder of the
        ///                 error message as needed.
        bool no_temporary_work(unsigned const max_sourceB,
                          unsigned const max_targetC,
                          unsigned const max_targetB, bool const source_bottom)
        {
            assert(max_targetC <= max_targetB);
            for (enum counter_type ctr = TRANS_MIN;
                                   ctr < refine_outgoing_transition_3_6_or_23l;
                                           ctr = (enum counter_type) (ctr + 1))
            {
                assert(counters[ctr - TRANS_MIN] <= max_targetC);
                counters[ctr - TRANS_MIN] = max_targetC;
            }
            assert(max_sourceB <= log_n);
            for (enum counter_type ctr = refine_outgoing_transition_3_6_or_23l;
                                         ctr < refine_incoming_transition_3_18;
                                           ctr = (enum counter_type) (ctr + 1))
            {
                assert(counters[ctr - TRANS_MIN] <= max_sourceB);
                counters[ctr - TRANS_MIN] = max_sourceB;
            }
            assert(max_targetB <= log_n);
            for (enum counter_type ctr = refine_incoming_transition_3_18;
                     ctr < TRANS_MIN_TEMP; ctr = (enum counter_type) (ctr + 1))
            {
                assert(counters[ctr - TRANS_MIN] <= max_targetB);
                counters[ctr - TRANS_MIN] = max_targetB;
            }
            // temporary transition counters must be zero
            for (enum counter_type ctr = TRANS_MIN_TEMP;
                     ctr <= TRANS_MAX_TEMP; ctr = (enum counter_type)(ctr + 1))
            {
                if (counters[ctr - TRANS_MIN] > 0)
                {
                    mCRL2log(log::error) << "Error 6: counter \""
                        << work_names[ctr - BLOCK_MIN] << "\" exceeded "
                                            "maximum value (" << 0 << ") for ";
                    return false;
                }
            }
            // bottom state counters must be 0 for transitions from non-bottom
            // states and 1 for other transitions
            assert((unsigned) source_bottom <= 1);
            for(enum counter_type ctr = (enum counter_type) (TRANS_MAX_TEMP+1);
                        ctr <= TRANS_MAX ; ctr = (enum counter_type) (ctr + 1))
            {
                if (counters[ctr - TRANS_MIN] > (unsigned) source_bottom)
                {
                    mCRL2log(log::error) << "Error 7: counter \""
                        << work_names[ctr - BLOCK_MIN] << "\" exceeded "
                     "maximum value (" << (unsigned) source_bottom << ") for ";
                    return false;
                }
                counters[ctr - TRANS_MIN] = (unsigned) source_bottom;
            }
            return true;
        }


        /// \brief register work with some temporary counter without changing
        /// the balance between sensible and superfluous work
        /// \details The function increases a temporary work counter.  It is
        /// also checked that the counter does not get too large.  This variant
        /// of `add_work()` should be used if one wants to assign a single step
        /// of work to multiple temporary counters of transitions:  for one of
        /// them, the normal `add_work()` is called, and for the others
        /// `add_work_notemporary()`.
        /// \param ctr        counter with which work is registered
        /// \param max_value  maximal allowed value of the counter.  The old
        ///                   value of the counter should be strictly smaller.
        ///                   (Because it is a temporary counter, only `1` is
        ///                   sensible.)
        /// \returns false  iff the counter was too large.  In that case, also
        ///                 the beginning of an error message is printed.
        ///                 The function should be called through the macro
        ///                 `mCRL2complexity()`, because that macro will print
        ///                 the remainder of the error message as needed.
        bool add_work_notemporary(enum counter_type const ctr,
                                                      unsigned const max_value)
        {
            if (TRANS_MIN_TEMP > ctr || ctr > TRANS_MAX_TEMP)
            {
                return add_work(ctr, max_value);
            }

            assert(1 == max_value);
            if (0 == counters[ctr - TRANS_MIN])
            {
                counters[ctr - TRANS_MIN] = DONT_COUNT_TEMPORARY;
                return true;
            }

            mCRL2log(log::error) << "Error 8: counter \""
                        << work_names[ctr - BLOCK_MIN] << "\" exceeded "
                                    "maximum value (" << max_value << ") for ";
            return false;
        }
    };

    /*----------------- class specialisations for bisim_dnj -----------------*/

    class block_dnj_counter_t : public counter_t<BLOCK_dnj_MIN, BLOCK_dnj_MAX,
                                                      create_initial_partition>
    {
      public:
        /// \brief ensures there is no orphaned temporary work counter
        /// \details When a refinement has finished, all work registered with
        /// temporary counters should have been moved to normal counters.  This
        /// function verifies this property.
        /// The function additionally ensures that no work counter exceeds its
        /// maximal allowed value, based on the size of the block or its
        /// constellation.  (The size of the constellation is the unit used for
        /// counters related to [blocks in the] splitter constellation;  the
        /// size of the block is used for other counters.)
        /// \param max_block ilog2(n^2) - ilog2(size of block)
        /// \returns false  iff some temporary counter was nonzero.  In that
        ///                 case, also the beginning of an error message is
        ///                 printed.  The function should be called through the
        ///                 macro `mCRL2complexity()`, because that macro will
        ///                 print the remainder of the error message as needed.
        bool no_temporary_work(unsigned const max_block)
        {
            assert((log_n + 1U) / 2U <= max_block);
            if (max_block > log_n)
            {
                    mCRL2log(log::error) << "Error 14: max_block == "
                                         << max_block << " exceeded log_n == "
                                         << (unsigned) log_n << " for ";
                    return false;
            }
            assert(max_block <= log_n);
            for (enum counter_type ctr = BLOCK_dnj_MIN;
                                ctr < create_initial_partition;
                                           ctr = (enum counter_type) (ctr + 1))
            {
                assert(counters[ctr - BLOCK_dnj_MIN] <= max_block);
                counters[ctr - BLOCK_dnj_MIN] = max_block;
            }
            for (enum counter_type ctr = create_initial_partition;
                     ctr <= BLOCK_dnj_MAX; ctr = (enum counter_type) (ctr + 1))
            {
                assert(counters[ctr - BLOCK_dnj_MIN] <= 1);
                counters[ctr - BLOCK_dnj_MIN] = 1;
            }
            return true;
        }
    };

    class state_dnj_counter_t : public counter_t<STATE_dnj_MIN, STATE_dnj_MAX,
              STATE_dnj_MIN_TEMP, (enum counter_type) (STATE_dnj_MAX_TEMP + 1)>
    {
      public:
        /// \brief ensures there is no orphaned temporary work counter
        /// \details When a refinement has finished, all work registered with
        /// temporary counters should have been moved to normal counters.  This
        /// function verifies this property.
        /// The function additionally ensures that no work counter exceeds its
        /// maximal allowed value, based on the size of the block or its
        /// constellation.  (The size of the constellation is the unit used for
        /// counters related to [blocks in the] splitter constellation;  the
        /// size of the block is used for other counters.)
        /// \param max_block ilog2(n^2) - ilog2(size of block)
        /// \returns false  iff some temporary counter was nonzero.  In that
        ///                 case, also the beginning of an error message is
        ///                 printed.  The function should be called through the
        ///                 macro `mCRL2complexity()`, because that macro will
        ///                 print the remainder of the error message as needed.
        bool no_temporary_work(unsigned const max_block, bool const bottom)
        {
            assert((log_n + 1U) / 2U <= max_block);
            assert(max_block <= log_n);
            for (enum counter_type ctr = STATE_dnj_MIN;
                 ctr < STATE_dnj_MIN_TEMP; ctr = (enum counter_type) (ctr + 1))
            {
                assert(counters[ctr - STATE_dnj_MIN] <= max_block);
                counters[ctr - STATE_dnj_MIN] = max_block;
            }
            for (enum counter_type ctr = STATE_dnj_MIN_TEMP;
                ctr <= STATE_dnj_MAX_TEMP; ctr = (enum counter_type) (ctr + 1))
            {
                assert(counters[ctr - STATE_dnj_MIN] <= 0);
            }
            // assert((unsigned) bottom <= 1);
            for (enum counter_type ctr =
                    (enum counter_type) (STATE_dnj_MAX_TEMP + 1);
                     ctr <= STATE_dnj_MAX; ctr = (enum counter_type) (ctr + 1))
            {
                assert(counters[ctr - STATE_dnj_MIN] <= (unsigned) bottom);
                counters[ctr - STATE_dnj_MIN] = (unsigned) bottom;
            }
            return true;
        }
    };

    class bunch_dnj_counter_t : public counter_t<BUNCH_dnj_MIN, BUNCH_dnj_MAX>
    {
      public:
        /// \brief ensures there is no orphaned temporary work counter
        /// \details When a refinement has finished, all work registered with
        /// temporary counters should have been moved to normal counters.  This
        /// function verifies this property.
        /// The function additionally ensures that no work counter exceeds its
        /// maximal allowed value, based on the size of the block or its
        /// constellation.  (The size of the constellation is the unit used for
        /// counters related to [blocks in the] splitter constellation;  the
        /// size of the block is used for other counters.)
        /// \param max_bunch ilog2(n^2) - ilog2(size of bunch)
        /// \returns false  iff some temporary counter was nonzero.  In that
        ///                 case, also the beginning of an error message is
        ///                 printed.  The function should be called through the
        ///                 macro `mCRL2complexity()`, because that macro will
        ///                 print the remainder of the error message as needed.
        bool no_temporary_work(unsigned const max_bunch)
        {
            assert(max_bunch <= log_n);
            for (enum counter_type ctr = BUNCH_dnj_MIN;
                     ctr <= BUNCH_dnj_MAX; ctr = (enum counter_type) (ctr + 1))
            {
                assert(counters[ctr - BUNCH_dnj_MIN] <= max_bunch);
                counters[ctr - BUNCH_dnj_MIN] = max_bunch;
            }
            return true;
        }
    };

    class block_bunch_dnj_counter_t : public counter_t<BLOCK_BUNCH_dnj_MIN,
                        BLOCK_BUNCH_dnj_MAX, BLOCK_BUNCH_dnj_MIN_TEMP,
                            (enum counter_type) (BLOCK_BUNCH_dnj_MAX_TEMP + 1)>
    {
      public:
        /// \brief ensures there is no orphaned temporary work counter
        /// \details When a refinement has finished, all work registered with
        /// temporary counters should have been moved to normal counters.  This
        /// function verifies this property.
        /// The function additionally ensures that no work counter exceeds its
        /// maximal allowed value, based on the size of the block or its
        /// constellation.  (The size of the constellation is the unit used for
        /// counters related to [blocks in the] splitter constellation;  the
        /// size of the block is used for other counters.)
        /// \param max_bunch ilog2(n^2) - ilog2(size of bunch)
        /// \returns false  iff some temporary counter was nonzero.  In that
        ///                 case, also the beginning of an error message is
        ///                 printed.  The function should be called through the
        ///                 macro `mCRL2complexity()`, because that macro will
        ///                 print the remainder of the error message as needed.
        bool no_temporary_work(unsigned const max_bunch)
        {
            assert(max_bunch <= log_n);
            for (enum counter_type ctr = BLOCK_BUNCH_dnj_MIN;
                            ctr < BLOCK_BUNCH_dnj_MIN_TEMP;
                                           ctr = (enum counter_type) (ctr + 1))
            {
                if (counters[ctr - BLOCK_BUNCH_dnj_MIN] > max_bunch)
                {
                    mCRL2log(log::error) << "Error 12: counter \""
                        << work_names[ctr - BLOCK_MIN] << "\" exceeded "
                        "maximum value (" << (unsigned) max_bunch << ") for ";
                    return false;
                }
                assert(counters[ctr - BLOCK_BUNCH_dnj_MIN] <= max_bunch);
                counters[ctr - BLOCK_BUNCH_dnj_MIN] = max_bunch;
            }
            for (enum counter_type ctr = BLOCK_BUNCH_dnj_MIN_TEMP;
               ctr <= BLOCK_BUNCH_dnj_MAX; ctr = (enum counter_type) (ctr + 1))
            {
                if (counters[ctr - BLOCK_BUNCH_dnj_MIN] > 0)
                {
                    mCRL2log(log::error) << "Error 13: counter \""
                        << work_names[ctr - BLOCK_MIN] << "\" exceeded "
                        "maximum value (" << (unsigned) 0 << ") for ";
                    return false;
                }
                assert(counters[ctr - BLOCK_BUNCH_dnj_MIN] <= 0);
            }
            assert(BLOCK_BUNCH_dnj_MAX_TEMP == BLOCK_BUNCH_dnj_MAX);
            return true;
        }

        bool has_temporary_work()
        {
            assert(BLOCK_BUNCH_dnj_MIN_TEMP == BLOCK_BUNCH_dnj_MAX_TEMP);
            return counters[BLOCK_BUNCH_dnj_MIN_TEMP-BLOCK_BUNCH_dnj_MIN] > 0;
        }
        void reset_temporary_work()
        {
            assert(BLOCK_BUNCH_dnj_MIN_TEMP == BLOCK_BUNCH_dnj_MAX_TEMP);
            counters[BLOCK_BUNCH_dnj_MIN_TEMP - BLOCK_BUNCH_dnj_MIN] = 0;
        }
    };

    class trans_dnj_counter_t : public counter_t<TRANS_dnj_MIN, TRANS_dnj_MAX,
              TRANS_dnj_MIN_TEMP, (enum counter_type) (TRANS_dnj_MAX_TEMP + 1)>
    {
      public:
        /// \brief ensures there is no orphaned temporary work counter
        /// \details When a refinement has finished, all work registered with
        /// temporary counters should have been moved to normal counters.  This
        /// function verifies this property.
        /// The function additionally ensures that no work counter exceeds its
        /// maximal allowed value, based on the size of the block or its
        /// constellation.  (The size of the constellation is the unit used for
        /// counters related to [blocks in the] splitter constellation;  the
        /// size of the block is used for other counters.)
        /// \param max_source_block  ilog2(n) - ilog2(size of source block)
        /// \param max_target_block  ilog2(n) - ilog2(size of target block)
        /// \param max_bunch  ilog2(n^2) - ilog2(size of bunch)
        /// \param bottom true iff the transition source is a bottom state
        /// \returns false  iff some temporary counter was nonzero.  In that
        ///                 case, also the beginning of an error message is
        ///                 printed.  The function should be called through the
        ///                 macro `mCRL2complexity()`, because that macro will
        ///                 print the remainder of the error message as needed.
        bool no_temporary_work(unsigned const max_source_block,
                unsigned const max_target_block, unsigned const max_bunch,
                                                             bool const bottom)
        {
            assert((log_n + 1U) / 2U <= max_source_block);
            assert(max_source_block <= log_n);
            for (enum counter_type ctr = TRANS_dnj_MIN;
                        ctr < refine__handle_transition_to_red_or_blue_state;
                                           ctr = (enum counter_type) (ctr + 1))
            {
                assert(counters[ctr - TRANS_dnj_MIN] <= max_source_block);
                counters[ctr - TRANS_dnj_MIN] = max_source_block;
            }
            assert((log_n + 1U) / 2U <= max_target_block);
            assert(max_target_block <= log_n);
            for (enum counter_type ctr =
                                refine__handle_transition_to_red_or_blue_state;
                        ctr < refine__handle_transition_in_FromRed;
                                           ctr = (enum counter_type) (ctr + 1))
            {
                assert(counters[ctr - TRANS_dnj_MIN] <= max_target_block);
                counters[ctr - TRANS_dnj_MIN] = max_target_block;
            }
            assert(max_bunch <= log_n);
            for (enum counter_type ctr = refine__handle_transition_in_FromRed;
                 ctr < TRANS_dnj_MIN_TEMP; ctr = (enum counter_type) (ctr + 1))
            {

                if (counters[ctr - TRANS_dnj_MIN] > max_bunch)
                {
                    mCRL2log(log::error) << "Error 10: counter \""
                        << work_names[ctr - BLOCK_MIN] << "\" exceeded "
                                    "maximum value (" << max_bunch << ") for ";
                    return false;
                }
                counters[ctr - TRANS_dnj_MIN] = max_bunch;
            }
            for (enum counter_type ctr = TRANS_dnj_MIN_TEMP;
                ctr <= TRANS_dnj_MAX_TEMP; ctr = (enum counter_type) (ctr + 1))
            {
                assert(counters[ctr - TRANS_dnj_MIN] <= 0);
            }
            // assert((unsigned) bottom <= 1);
            for (enum counter_type ctr =
                    (enum counter_type) (TRANS_dnj_MAX_TEMP + 1);
                     ctr <= TRANS_dnj_MAX; ctr = (enum counter_type) (ctr + 1))
            {
                if (counters[ctr - TRANS_dnj_MIN] > (unsigned) bottom)
                {
                    mCRL2log(log::error) << "Error 11: counter \""
                        << work_names[ctr - BLOCK_MIN] << "\" exceeded "
                            "maximum value (" << (unsigned) bottom << ") for ";
                    return false;
                }
                counters[ctr - TRANS_dnj_MIN] = (unsigned) bottom;
            }
            return true;
        }

        /// \brief register work with some temporary counter without changing
        /// the balance between sensible and superfluous work
        /// \details The function increases a temporary work counter.  It is
        /// also checked that the counter does not get too large.  This variant
        /// of `add_work()` should be used if one wants to assign a single step
        /// of work to multiple temporary counters of transitions:  for one of
        /// them, the normal `add_work()` is called, and for the others
        /// `add_work_notemporary()`.
        /// \param ctr        counter with which work is registered
        /// \param max_value  maximal allowed value of the counter.  The old
        ///                   value of the counter should be strictly smaller.
        ///                   (Because it is a temporary counter, only `1` is
        ///                   sensible.)
        /// \returns false  iff the counter was too large.  In that case, also
        ///                 the beginning of an error message is printed.
        ///                 The function should be called through the macro
        ///                 `mCRL2complexity()`, because that macro will print
        ///                 the remainder of the error message as needed.
        bool add_work_notemporary(enum counter_type const ctr,
                                                      unsigned const max_value)
        {
            if (TRANS_dnj_MIN_TEMP > ctr || ctr > TRANS_dnj_MAX_TEMP)
            {
                return add_work(ctr, max_value);
            }

            assert(1 == max_value);
            if (0 == counters[ctr - TRANS_dnj_MIN])
            {
                counters[ctr - TRANS_dnj_MIN] = DONT_COUNT_TEMPORARY;
                return true;
            }

            mCRL2log(log::error) << "Error 9: counter \""
                        << work_names[ctr - BLOCK_MIN] << "\" exceeded "
                                    "maximum value (" << max_value << ") for ";
            return false;
        }
    };

    #if 0
        /// \brief prints a message for each counter, for debugging purposes
        /// \details The function can be called, e. g., from
        /// `check_complexity::init()`.  However, as it is not
        /// actually called, it is disabled by default.
        static void test_work_names();
    #endif


    /// \brief starts counting for a new refinement run
    /// \details Before any work can be counted, `init()` should be called to
    /// set the size of the state space to the appropriate value.
    /// \param n  size of the state space
    static void init(state_type n)
    {
        #if 0
            // as debugging measure:
            test_work_names();
        #endif

        log_n = ilog2(n);
        assert(0 == sensible_work);     sensible_work = 0;
    }


    /// \brief Assigns work to a counter and checks for errors
    /// \details Many functions that assign work to a counter actually return
    /// `true` if everything is ok and `false` if the counter was too large.
    /// In the latter case, they also print the start of an error message
    /// (``Counter ... too large''), but as they do not know to which larger
    /// unit (state, transition etc.) the counter belongs, this macro prints
    /// the end of the error message and aborts the program with a return code
    /// indicating failure.
    ///
    /// If debugging is off, the macro is translated to do nothing.
    /// \param unit   the unit to which work is assigned
    /// \param call   a function call that assigns work to a counter of `unit`
    #define mCRL2complexity(unit, call, ...)                                  \
            do                                                                \
            {                                                                 \
                if (!((unit)->work_counter. call ))                           \
                {                                                             \
                    mCRL2log(log::error)<<(unit)->debug_id(__VA_ARGS__)<<'\n';\
                    exit(EXIT_FAILURE);                                       \
                }                                                             \
                /* else                                                       \
                {                                                             \
                    mCRL2log(log::debug, "bisim_dnj") << "mCRL2complexity("   \
                         << (unit)->debug_id(__VA_ARGS__) << ", " #call ")\n";\
                } */                                                          \
            }                                                                 \
            while (0)

};

} // end namespace bisim_gjkw

#else // ifndef NDEBUG

    #define mCRL2complexity(unit, call, ...) /* ignore unit and call */

#endif // ifndef NDEBUG

} // end namespace detail
} // end namespace lts
} // end namespace mcrl2

#endif // ifndef _COUNT_ITERATIONS_H
