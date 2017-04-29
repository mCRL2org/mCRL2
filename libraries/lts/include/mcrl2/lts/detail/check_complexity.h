// Author(s): David N. Jansen, Radboud Universiteit, Nijmegen, The Netherlands
//
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
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
/// `finalise_work()`.  To cancel counters, use `cancel_work()`.  These
/// functions are not normally called directly, but through `red_is_smaller()`
/// or `blue_is_smaller()`:  If the red subblock is smaller, call
/// `red_is_smaller(temporary counter, normal counter, new counter value)` for
/// each state and each transition of the block that was refined;  if the blue
/// subblock is smaller, call `blue_is_smaller(...)`.
/// After all temporary work has been handled, call `check_temporary_work()` to
/// compare the amount of sensible work with the amount of cancelled work.
///
/// If the work could be assigned to one of several counters, I recommend to
/// assign it to all of them;  otherwise, it may happen that a later excess of
/// the time budget goes unnoticed because too few counters were advanced.
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
#include <climits>       // for CHAR_BIT

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

/// \brief type used to store transition (numbers and) counts
/// \details defined here because this is the most basic #include header that
/// uses it.
typedef std::size_t trans_type;

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
    /// There are counters assigned to constellations, states, B_to_C slices,
    /// and transitions.  Counters assigned to a constellation are regarded as
    /// if the work would be registered with every state in the constellation.
    /// Counters asssigned to a B_to_C slice are regarded as if the work would
    /// be registered with every transition in the slice.  (These conventions
    /// are important when splitting a constellation or a B_to_C slice.)
    enum counter_type
    {
        // block counters: every state in the block is regarded as visited.  In
        // this way, every state is ``visited'' O(log n) times.
        while_C_contains_a_nontrivial_constellation_2_4 = 0,
        BLOCK_MIN = while_C_contains_a_nontrivial_constellation_2_4,
        Mark_all_states_of_SpB_as_predecessors_2_9,
        for_all_refinable_blocks_RfnB_is_splitter_2_20,

        // If a loop runs over every state of a block exactly once, we simplify
        // the task of updating the counters by making the corresponding loop
        // counter a block counter:
        for_all_s_in_SpB_2_10,
        Move_Blue_or_Red_to_a_new_block_NewB_pointer_3_29,
        Move_Blue_or_Red_to_a_new_block_states_3_29,
        for_all_s_in_NewB_3_31,
        BLOCK_MAX = for_all_s_in_NewB_3_31,

        // state counters: every state is visited O(log n) times
        Move_Blue_or_Red_to_a_new_block_NewB_swap_3_29,
        STATE_MIN = Move_Blue_or_Red_to_a_new_block_NewB_swap_3_29,

        // The following counters are used when one refines a block:  the first
        // group is used to store the amount of work that (a posteriori) turns
        // out to be useful.  After that, there are two groups of counters to
        // store temporary work.
        refine_bottom_state_3_6l,
        refine_bottom_state_3_6l_s_is_in_SpB,
        refine_visited_state_3_15,

        // temporary state counters (blue):
        while_Test_is_not_empty_3_6l_s_is_in_SpB_and_red_3_9l, 
        while_Test_is_not_empty_3_6l_s_is_blue_3_11l,
        while_Blue_contains_unvisited_states_3_15l,

        // temporary state counters (red):
        while_Red_contains_unvisited_states_3_15r,

        // new bottom state counters: every state is visited once
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
        B_TO_C_MAX = for_all_constellations_C_not_in_R_from_RfnB_4_4,

        // transition counters: every transition is visited O(log n) times
        for_all_s_prime_in_pred_s_2_11,
        TRANS_MIN = for_all_s_prime_in_pred_s_2_11,
        Register_that_inert_transitions_from_s_go_to_NewC_succ_2_17,
        Register_that_inert_transitions_from_s_go_to_NewC_swap_2_17,
        Move_Blue_or_Red_to_a_new_block_succ_3_29,
        for_all_s_prime_in_pred_s_3_32l,
        for_all_s_prime_in_succ_s_3_32r,

        // temporary transition counters for refine: similar to the temporary
        // counters for states, we have a first group to store the work done
        // for the smaller half, and temporary counters to store the work until
        // it becomes clear which half wins.
        refine_outgoing_transition_3_6_or_23l,
        refine_outgoing_transition_to_marked_state_3_6l,
        refine_incoming_transition_3_18,

        // temporary transition counters (blue):
        while_Test_is_not_empty_3_6l_s_is_red_3_9l,
        TRANS_MIN_TEMPORARY = while_Test_is_not_empty_3_6l_s_is_red_3_9l,
        while_Test_is_not_empty_3_6l_s_is_red_3_9l_postprocessing,
        for_all_s_prime_in_pred_s_setminus_Red_3_18l,
        if___s_prime_has_no_transition_to_SpC_3_23l,
            // The work in the following counter is assigned to red (new
            // bottom) states if the blue block is smaller!
        if___s_prime_has_transition_to_SpC_3_23l,

        // temporary transition counters (red):
        while_FromRed_is_not_empty_3_6r,
        for_all_s_prime_in_pred_s_3_18r,
        TRANS_MAX_TEMPORARY = for_all_s_prime_in_pred_s_3_18r,

        // new bottom transition counters: every transition is visited once
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
        TRANS_MAX = for_all_old_bottom_states_s_in_RedB_4_15
    };

    /// \brief special value for temporary work without changing the balance
    #define DONT_COUNT_TEMPORARY (std::numeric_limits<unsigned char>::max()-1)

    /// \brief value of log2(n) for easy access
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
        return std::log2(size);
    }

  private:
    /// \brief counter to register the sensible work done in coroutines
    static state_type sensible_work;

    /// \brief counter to register the superfluous work done in coroutines
    static state_type superfluous_work;

  public:
    /// \brief printable names of the counter types (for error messages)
    static const char *work_names[TRANS_MAX - BLOCK_MIN + 1];

    /// \brief check that not too much superfluous work has been done
    /// \details After having moved all temporary work counters to the normal
    /// counters, this function can be used to ensure that not too much
    /// temporary work is cancelled.
    static void check_temporary_work()
    {
        assert(superfluous_work <= sensible_work + 1);
        sensible_work = 0;
        superfluous_work = 0;
    }

    /// \brief subset of counters (to be associated with a state or transition)
    template <enum counter_type FirstCounter, enum counter_type LastCounter>
    class counter_t
    {
      protected:
        /// \brief actual space to store the counters
        unsigned char counters[LastCounter - FirstCounter + 1];

        /// \brief cancel temporary work
        /// \details The function registers that all counters from `first` to
        /// `last` (inclusive) are counting superfluous work.  It adds them to
        /// the pool of superfluous work.
        void cancel_work(enum counter_type first, enum counter_type last)
        {
            assert(FirstCounter<=first && first <= last && last<=LastCounter);
            for (enum counter_type i = first ; i <= last ;
                                               i = (enum counter_type) (i + 1))
            {
                //if (0 != counters[i - FirstCounter])
                //{
                //    mCRL2log(log::debug,"bisim_gjkw") << ". cancel counters["
                //                                     << work_names[i] << "]";
                //}
                if (LastCounter < TRANS_MIN_TEMPORARY ||
                        FirstCounter > TRANS_MIN_TEMPORARY ||
                        i < TRANS_MIN_TEMPORARY || i > TRANS_MAX_TEMPORARY ||
                            DONT_COUNT_TEMPORARY != counters[i - FirstCounter])
                {
                    superfluous_work += counters[i - FirstCounter];
                }
                //else
                //{
                //    mCRL2log(log::debug, "bisim_gjkw") << '*';
                //}
                counters[i - FirstCounter] = 0;
            }
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
        bool finalise_work(enum counter_type from, enum counter_type to,
                                                       unsigned char max_value)
        {
            // assert(...) -- see move_work().
            if (LastCounter < TRANS_MIN_TEMPORARY ||
                    FirstCounter > TRANS_MAX_TEMPORARY ||
                    from < TRANS_MIN_TEMPORARY || from > TRANS_MAX_TEMPORARY ||
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

      public:
        /// \brief constructor, initializes all counters to 0
        counter_t()  {  std::memset(counters, '\0', sizeof(counters));  }

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
        bool add_work(enum counter_type ctr, unsigned char max_value)
        {
            assert(FirstCounter <= ctr && ctr <= LastCounter);
            assert(max_value <= log_n);
            if (counters[ctr - FirstCounter] >= max_value)
            {
                mCRL2log(log::error) << "Error: counter \"" << work_names[ctr]
                                           << "\" exceeded maximum value ("
                                           << (unsigned) max_value << ") for ";
                return false;
            }
            counters[ctr - FirstCounter] = max_value;
            //mCRL2log(log::debug,"bisim_gjkw") <<"counters[" <<work_names[ctr]
            //    <<"] = " <<(trans_type) counters[ctr - FirstCounter] <<";\n";
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
        bool move_work(enum counter_type from, enum counter_type to,
                                                       unsigned char max_value)
        {
            assert(FirstCounter <= from && from <= LastCounter);
            assert(FirstCounter <= to && to <= LastCounter);
            if (0 == max_value && 0 == counters[from - FirstCounter])
            {
                return true;
            }
            assert(max_value <= log_n);
            if (counters[to - FirstCounter] > max_value ||
                                 (counters[to - FirstCounter] == max_value &&
                                           0 != counters[from - FirstCounter]))
            {
                mCRL2log(log::error) << "Error: counter \"" << work_names[to]
                                           << "\" exceeded maximum value ("
                                           << (unsigned) max_value << ") for ";
                return false;
            }
            if (FirstCounter <= TRANS_MIN_TEMPORARY &&
                    TRANS_MAX_TEMPORARY <= LastCounter &&
                    from>= TRANS_MIN_TEMPORARY && from<= TRANS_MAX_TEMPORARY &&
                    to  >= TRANS_MIN_TEMPORARY && to  <= TRANS_MAX_TEMPORARY &&
                         DONT_COUNT_TEMPORARY == counters[from - FirstCounter])
            {
                counters[to - FirstCounter] = DONT_COUNT_TEMPORARY;
            }
            else
            {
                counters[to - FirstCounter] = max_value;
                assert(1 >= counters[from - FirstCounter]);
            }
            //mCRL2log(log::debug, "bisim_gjkw")<<". counters["<<work_names[to]
            //            << "] = " << (trans_type) counters[to - FirstCounter]
            //                   << " <- counters[" << work_names[from] << "]";
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
    };

    /// \brief counters for a B_to_C slice
    /// \details The counters stored with a B_to_C slice are meant to be
    /// assigned to each transition in the slice.  This means that the counter
    /// values need to be copied when the slice is split.
    class B_to_C_counter_t : public counter_t<B_TO_C_MIN, B_TO_C_MAX>
    {
      public:
        /// \brief ensures there is no orphaned temporary work counter
        /// \details When a refinement has finished, all work registered with
        /// temporary counters should have been moved to normal counters.  This
        /// function verifies this property.
        /// \returns false  iff some temporary counter was nonzero.  In that
        ///                 case, also the beginning of an error message is
        ///                 printed.
        bool no_temporary_work() const
        {
            if (counters[for_all_constellations_C_not_in_R_from_RfnB_4_4 -
                                                               B_TO_C_MIN] > 0)
            {
                mCRL2log(log::error) << "Error: counter \"" << work_names[
                             for_all_constellations_C_not_in_R_from_RfnB_4_4]
                             << "\" exceeded maximum value (" << 0 << ") for ";
                return false;
            }
            return true;
        }

        /// \brief returns the _temporary_ counter associated with line 4.4
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
    class state_counter_t : public counter_t<STATE_MIN, STATE_MAX>
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
        /// \param bottom  `true` iff the state to which these counters belong
        ///                is a bottom state
        /// \returns false  iff some temporary counter or some bottom-state
        ///                 counter of a non-bottom state was nonzero.  In that
        ///                 case, also the beginning of an error message is
        ///                 printed.
        bool no_temporary_work(bool bottom)
        {
            enum counter_type ctr =
                         while_Test_is_not_empty_3_6l_s_is_in_SpB_and_red_3_9l;
            // temporary state counters must be zero:
            for (; ctr <= while_Red_contains_unvisited_states_3_15r ;
                                           ctr = (enum counter_type) (ctr + 1))
            {
                if (counters[ctr - STATE_MIN] > 0)
                {
                    mCRL2log(log::error) << "Error: counter \""
                            << work_names[ctr] << "\" exceeded maximum value ("
                            << 0 << ") for ";
                    return false;
                }
            }
            // bottom state counters must be 0 for non-bottom states and 1 for
            // bottom states:
            unsigned char max_bot = bottom ? 1 : 0;
            for (; ctr <= STATE_MAX ; ctr = (enum counter_type) (ctr + 1))
            {
                if (counters[ctr - STATE_MIN] > max_bot)
                {
                    mCRL2log(log::error) << "Error: counter \""
                            << work_names[ctr] << "\" exceeded maximum value ("
                            << (unsigned) max_bot << ") for ";
                    return false;
                }
                counters[ctr - STATE_MIN] = max_bot;
            }
            return true;
        }

        /// \brief moves temporary counters to normal ones if the blue block is
        /// smaller
        /// \details When a refinement has finished and the blue block turns
        /// out to be smaller, this function moves the corresponding temporary
        /// work to the normal counters and cancels the work on the red state
        /// counters.
        /// \param max_NewB  the maximum allowed value of the normal counters
        ///                  (corresponding to the size of the blue block).
        ///                  Note that the blue block may be empty; in that
        ///                  case, this parameter should be 0.
        /// \param max_SpB   the maximum allowed value of the normal counter
        ///                  for line 3.6l, if refine() was called from
        ///                  line 2.26.  In that case, some work is ascribed to
        ///                  the marking of states instead of the size of the
        ///                  new block.  Otherwise, 0.
        /// \returns false  iff one of the normal counters was too large.  In
        ///                 that case, also the beginning of an error message
        ///                 is printed.
        bool blue_is_smaller(unsigned char max_NewB, unsigned char max_SpB)
        {
            cancel_work(while_Red_contains_unvisited_states_3_15r,
                                    while_Red_contains_unvisited_states_3_15r);
            return finalise_work(
                     while_Test_is_not_empty_3_6l_s_is_in_SpB_and_red_3_9l,
                              refine_bottom_state_3_6l_s_is_in_SpB, max_SpB) &&
                finalise_work(while_Test_is_not_empty_3_6l_s_is_blue_3_11l,
                                         refine_bottom_state_3_6l, max_NewB) &&
                finalise_work(while_Blue_contains_unvisited_states_3_15l,
                                          refine_visited_state_3_15, max_NewB);
        }

        /// \brief moves temporary counters to normal ones if the red block is
        /// smaller
        /// \details When a refinement has finished and the red block turns
        /// out to be smaller, this function moves the corresponding temporary
        /// work to the normal counters and cancels the work on the blue state
        /// counters.
        /// \param max_value  the maximum allowed value of the normal counters
        ///                   (corresponding to the size of the red block)
        /// \returns false  iff one of the normal counters was too large.  In
        ///                 that case, also the beginning of an error message
        ///                 is printed.
        bool red_is_smaller(unsigned char max_value)
        {
            cancel_work(while_Test_is_not_empty_3_6l_s_is_in_SpB_and_red_3_9l,
                                   while_Blue_contains_unvisited_states_3_15l);
            return finalise_work(while_Red_contains_unvisited_states_3_15r,
                                         refine_visited_state_3_15, max_value);
        }
    };
    class trans_counter_t : public counter_t<TRANS_MIN, TRANS_MAX>
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
        /// \param source_bottom  `true` iff the transition to which these
        ///                       counters belong starts in a bottom state
        /// \returns false  iff some temporary counter or some bottom-state
        ///                 counter of a transition with non-bottom source was
        ///                 nonzero.  In that case, also the beginning of an
        ///                 error message is printed.
        bool no_temporary_work(bool source_bottom)
        {
            enum counter_type ctr = TRANS_MIN_TEMPORARY;
            // temporary transition counters must be zero
            for (; ctr <= TRANS_MAX_TEMPORARY ; ctr=(enum counter_type)(ctr+1))
            {
                if (counters[ctr - TRANS_MIN] > 0)
                {
                    mCRL2log(log::error) << "Error: counter \""
                            << work_names[ctr] << "\" exceeded maximum value ("
                            << 0 << ") for ";
                    return false;
                }
            }
            // bottom state counters must be 0 for transitions from non-bottom
            // states and 1 for other transitions
            unsigned char max_bot = source_bottom ? 1 : 0;
            for (; ctr <= TRANS_MAX ; ctr = (enum counter_type) (ctr + 1))
            {
                if (counters[ctr - TRANS_MIN] > max_bot)
                {
                    mCRL2log(log::error) << "Error: counter \""
                            << work_names[ctr] << "\" exceeded maximum value ("
                            << (unsigned) max_bot << ") for ";
                    return false;
                }
                counters[ctr - TRANS_MIN] = max_bot;
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
        /// The function is normally called through the macro
        /// `mCRL2complexity()`.
        /// \param ctr        counter with which work is registered
        /// \param max_value  maximal allowed value of the counter.  The old
        ///                   value of the counter should be strictly smaller.
        ///                   (Because it is a temporary counter, only `1` is
        ///                   sensible.)
        /// \returns false  iff the counter was too large.  In that case, also
        ///                 the beginning of an error message is printed.
        bool add_work_notemporary(enum counter_type ctr,
                                                       unsigned char max_value)
        {
            if (TRANS_MIN_TEMPORARY > ctr || ctr > TRANS_MAX_TEMPORARY)
            {
                return add_work(ctr, max_value);
            }

            assert(1 == max_value);
            if (0 == counters[ctr - TRANS_MIN])
            {
                counters[ctr - TRANS_MIN] = DONT_COUNT_TEMPORARY;
                return true;
            }

            mCRL2log(log::error) << "Error: counter \"" << work_names[ctr]
                                         << "\" exceeded maximum value ("
                                         << (trans_type) max_value << ") for ";
            return false;
        }

        /// \brief moves temporary counters to normal ones if the blue block is
        /// smaller
        /// \details When a refinement has finished and the blue block turns
        /// out to be smaller, this function moves the corresponding temporary
        /// work to the normal counters and cancels the work on the red state
        /// counters.
        /// \param max_NewB  the maximum allowed value of the normal counters
        ///                  (corresponding to the size of the blue block).
        ///                  Note that the blue block may be empty; in that
        ///                  case, this parameter should be 0.
        /// \param max_SpB   the maximum allowed value of the normal counter
        ///                  for line 3.6l, if refine() was called from
        ///                  line 2.26.  In that case, some work is ascribed to
        ///                  the marking of states instead of the size of the
        ///                  new block.  Otherwise, 0.
        /// \returns false  iff one of the normal counters was too large.  In
        ///                 that case, also the beginning of an error message
        ///                 is printed.
        bool blue_is_smaller(unsigned char max_NewB, unsigned char max_SpB)
        {
            cancel_work(while_FromRed_is_not_empty_3_6r,
                                              for_all_s_prime_in_pred_s_3_18r);
            return finalise_work(while_Test_is_not_empty_3_6l_s_is_red_3_9l,
                   refine_outgoing_transition_to_marked_state_3_6l, max_SpB) &&
                finalise_work(
                   while_Test_is_not_empty_3_6l_s_is_red_3_9l_postprocessing,
                   refine_outgoing_transition_postprocess_new_bottom_3_6l,1) &&
                finalise_work(for_all_s_prime_in_pred_s_setminus_Red_3_18l,
                                  refine_incoming_transition_3_18, max_NewB) &&
                finalise_work(if___s_prime_has_no_transition_to_SpC_3_23l,
                            refine_outgoing_transition_3_6_or_23l, max_NewB) &&
                finalise_work(if___s_prime_has_transition_to_SpC_3_23l,
                          refine_outgoing_transition_from_new_bottom_3_23l, 1);
        }

        /// \brief moves temporary counters to normal ones if the red block is
        /// smaller
        /// \details When a refinement has finished and the red block turns
        /// out to be smaller, this function moves the corresponding temporary
        /// work to the normal counters and cancels the work on the blue state
        /// counters.
        /// \param max_value  the maximum allowed value of the normal counters
        ///                   (corresponding to the size of the red block)
        /// \returns false  iff one of the normal counters was too large.  In
        ///                 that case, also the beginning of an error message
        ///                 is printed.
        bool red_is_smaller(unsigned char max_value)
        {
            cancel_work(while_Test_is_not_empty_3_6l_s_is_red_3_9l,
                                     if___s_prime_has_transition_to_SpC_3_23l);
            return finalise_work(while_FromRed_is_not_empty_3_6r,
                           refine_outgoing_transition_3_6_or_23l, max_value) &&
                finalise_work(for_all_s_prime_in_pred_s_3_18r,
                                   refine_incoming_transition_3_18, max_value);
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
        // as debugging measure:
        // test_work_names();

        log_n = ilog2(n);
        assert(0 == sensible_work);     sensible_work = 0;
        assert(0 == superfluous_work);  superfluous_work = 0;
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
    #define mCRL2complexity(unit,call)                                        \
            do                                                                \
            {                                                                 \
                if (!(unit)->work_counter. call )                             \
                {                                                             \
                    mCRL2log(log::error) << (unit)->debug_id() << '\n';       \
                    exit(EXIT_FAILURE);                                       \
                }                                                             \
            }                                                                 \
            while (0)

};

} // end namespace bisim_gjkw

#else // ifndef NDEBUG

    #define mCRL2complexity(C,call)

#endif // ifndef NDEBUG

} // end namespace detail
} // end namespace lts
} // end namespace mcrl2

#endif // ifndef _COUNT_ITERATIONS_H
