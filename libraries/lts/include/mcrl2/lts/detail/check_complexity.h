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
/// \details We use the class in this group to check whether the overall time
/// complexity fits in O(m log n).  Although it is difficult to test this in
/// general because of the constant factor in the definition of the O()
/// notation, it is often possible to give a (rather tight) upper bound on the
/// number of iterations of most loops.
///
/// The principle of time measurement with this file is:  every loop body is
/// assigned to a state or a transition.  Every state and every transition gets
/// a counter for every loop body which is assigned to it.  When the loop body
/// is executed, the corresponding counter is increased.  Assertions ensure
/// that not a single counter exceeds log_2(n), or, in the case of counters for
/// new bottom states, 1.
///
/// \author David N. Jansen, Radboud Universiteit, Nijmegen, The Netherlands

#ifndef _COUNT_ITERATIONS_H
#define _COUNT_ITERATIONS_H

#include <cstring>       // for size_t and memset()
#include <cassert>
#include <cmath>         // for log2()
#include <limits>        // for std::numeric_limits<unsigned char>::max()
#include "mcrl2/utilities/logger.h"


namespace mcrl2
{
namespace lts
{
namespace detail
{

/// type used to store state (numbers and) counts
typedef size_t state_type;
#define STATE_TYPE_MIN ((state_type) 0)
#define STATE_TYPE_MAX SIZE_MAX

/// type used to store transition (numbers and) counts
typedef size_t trans_type;

#ifndef NDEBUG

namespace bisim_gjkw
{

class check_complexity
{
  public:
    enum counter_type
    {
        // global counters: they count up to n or m, in situations where not a
        // single state or transition can be found to which to assign the work,
        // or when those structures are not yet set up during initialisation.
        find_extra_Kripke_states_init_helper = 0,
        GLOBAL_MIN = find_extra_Kripke_states_init_helper,
        initialise_blocks_init_helper,
        initialise_B_to_C_init_helper,
        initialise_states_init_helper,
        init_constln_slices_init_helper,
        initialise_transitions_init_helper,
        GLOBAL_MAX = initialise_transitions_init_helper,

        // constellation counters: every state in the constellation is regarded
        // as visited.  In this way, every state is ``visited'' O(log n) times.
        while_C_contains_a_nontrivial_constellation_2_4,
        CONSTLN_MIN = while_C_contains_a_nontrivial_constellation_2_4,
        delete_constellations,
        CONSTLN_MAX = delete_constellations,

        // block counters: every state in the block is regarded as visited.
        // In this way, every state is ``visited'' O(log n) times.
            // Another possibility would be to make these two counters global.
            // Then, their maximum value would be n (which will often be too
            // large for unsigned char).
        Mark_all_states_of_SpB_as_predecessors_2_9,
        BLOCK_MIN = Mark_all_states_of_SpB_as_predecessors_2_9,
        for_all_refinable_blocks_RfnB_is_splitter_2_20,
        for_all_blocks_repl_trans,
        delete_blocks,
        BLOCK_MAX = delete_blocks,

        // state counters: every state is visited O(log n) times
        for_all_s_in_SpB_2_10,
        STATE_MIN = for_all_s_in_SpB_2_10,
        for_all_s_in_NewB_3_31,

        // The following counters are used when one refines a block:  the first
        // group is used to store the amount of work that (a posteriori) turns
        // out to be useful.  After that, there are two groups of counters to
        // store temporary work.  As soon as refine() finds out which of the
        // two halves is the smaller, the work corresponding to that half is
        // moved to the first group of counters, and the work corresponding to
        // the other half is ``cancelled''.  A global comparison ensures that
        // at most 50% of the work is cancelled.
        refine_bottom_state_3_6l,
        refine_bottom_state_3_6l_s_is_in_SpB,
        refine_visited_state_3_15,
        Move_Blue_or_Red_to_a_new_block_NewB_swap_3_29,
        Move_Blue_or_Red_to_a_new_block_NewB_pointer_3_29,
        Move_Blue_or_Red_to_a_new_block_states_3_29,
        
        // temporary state counters (blue):
        while_Test_is_not_empty_3_6l_s_is_in_SpB_and_red_3_9l, 
        while_Test_is_not_empty_3_6l_s_is_blue_3_11l,
        while_Blue_contains_unvisited_states_3_15l,

        // temporary state counters (red):
        while_Red_contains_unvisited_states_3_15r,

        // counters for finalising the data structure (after the algorithm has
        // factually terminated):
        for_all_extra_Kripke_states_repl_trans,

        // new bottom state counters: every state is visited once
        for_all_bottom_states_s_in_RfnB_4_8,
            // the next counter is used to count the work done on a virtual
            // self-loop in line 4.15 (the new bottom state is regarded as red
            // because it is in the splitter, but there is no transition to the
            // splitter).
        for_all_old_bottom_states_s_in_RedB_selfloop_4_15,
        STATE_MAX = for_all_old_bottom_states_s_in_RedB_selfloop_4_15,

        // B_to_C_descriptor counters: every transition in the B_to_C-slice is
        // visited O(log n) times.
        for_all_refinable_blocks_RfnB_2_20,
        B_TO_C_MIN = for_all_refinable_blocks_RfnB_2_20,
        // the following two counters are also meant for temporary work.
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
        Register_that_inert_transitions_from_s_go_to_NewC_B_to_C_2_17,
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

        // counters for finalising the data:
        for_all_transitions_in_succ_s_repl_trans,

        // new bottom transition counters: every transition is visited once
        refine_outgoing_transition_postprocess_new_bottom_3_6,
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

    // Sometimes we want to ascribe an action to a set of transitions, namely
    // all transitions from a state to a certain constellation.  This is
    // problematic if this ascription is temporary because then the variables
    // sensible_work and superfluous_work might get wrong values.  To avoid
    // that, one transition is ascribed one unit of work, and the other
    // transitions are ascribed DONT_COUNT_TEMPORARY units of work.  The
    // methods finalise_work() and cancel_work() take this special value into
    // account.
    #define DONT_COUNT_TEMPORARY (std::numeric_limits<unsigned char>::max()-1)

    static state_type n;
    static trans_type m;
    static unsigned char log_n;

  private:
    static state_type sensible_work, superfluous_work;
  public:
    static const char *work_names[TRANS_MAX - GLOBAL_MIN + 1];

    static void check_temporary_work()
    {
        assert(superfluous_work <= sensible_work + 1);
        sensible_work = 0;
        superfluous_work = 0;
    }

    template <enum counter_type FirstCounter, enum counter_type LastCounter,
                                                    typename T = unsigned char>
    class counter_t
    {
      protected:
        // make counters[] have a multiple of 4 as size.
        T counters[(LastCounter - FirstCounter + 4) & ~3];

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
                if (i < TRANS_MIN_TEMPORARY || i > TRANS_MAX_TEMPORARY ||
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

        void finalise_work(enum counter_type from, enum counter_type to,
                                                                   T max_value)
        {
            if (from < TRANS_MIN_TEMPORARY || from > TRANS_MAX_TEMPORARY ||
                         DONT_COUNT_TEMPORARY != counters[from - FirstCounter])
            {
                sensible_work += counters[from - FirstCounter];
            }
            else
            {
                counters[from - FirstCounter] = 1;
            }
            move_work(from, to, max_value);
        }

      public:
        counter_t()  {  }
        void init()  {  memset(counters, '\0', sizeof(counters));  }

        bool add_check_work(enum counter_type ctr, T max_value)
        {
            assert(FirstCounter <= ctr && ctr <= LastCounter);
            counters[ctr - FirstCounter]++;
            //mCRL2log(log::debug,"bisim_gjkw") <<"counters[" <<work_names[ctr]
            //    <<"] = " <<(trans_type) counters[ctr - FirstCounter] <<";\n";
            if (counters[ctr - FirstCounter] <= max_value)  return true;

            mCRL2log(log::verbose) << "Error: counter \"" << work_names[ctr]
                                         << "\" exceeded maximum value ("
                                         << (trans_type) max_value << ") for ";
            return false;
        }

        void move_work(enum counter_type from,enum counter_type to,T max_value)
        {
            assert(FirstCounter <= from && from <= LastCounter);
            assert(FirstCounter <= to && to <= LastCounter);
            if (0 == counters[from - FirstCounter])  return;
            counters[to - FirstCounter] += counters[from - FirstCounter];
            counters[from - FirstCounter] = 0;

            //mCRL2log(log::debug, "bisim_gjkw")<<". counters["<<work_names[to]
            //            << "] = " << (trans_type) counters[to - FirstCounter]
            //                   << " <- counters[" << work_names[from] << "]";

            if ((from < TRANS_MIN_TEMPORARY || from > TRANS_MAX_TEMPORARY ||
                        DONT_COUNT_TEMPORARY != counters[to - FirstCounter]) &&
                counters[to - FirstCounter] > max_value)
            {
                mCRL2log(log::verbose) << "Error: counter \"" << work_names[to]
                                         << "\" exceeded maximum value ("
                                         << (trans_type) max_value << ") for ";
                assert(0);
            }
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

    class global_counter_t : public counter_t<GLOBAL_MIN,GLOBAL_MAX,trans_type> 
    {  };
    static global_counter_t global_counter;

    class constln_counter_t : public counter_t<CONSTLN_MIN, CONSTLN_MAX>
    {
      public:
        constln_counter_t()  {  init();  }
        bool no_temporary_work() const
        {
            return 0 == counters[delete_constellations - CONSTLN_MIN];
        }
    };
    class block_counter_t : public counter_t<BLOCK_MIN, BLOCK_MAX>
    {
      public:
        block_counter_t()  {  init();  }
        bool no_temporary_work() const
        {
            return 0 == counters[for_all_blocks_repl_trans - BLOCK_MIN] &&
                   0 == counters[delete_blocks - BLOCK_MIN];
        }
    };
    class B_to_C_counter_t : public counter_t<B_TO_C_MIN, B_TO_C_MAX>
    {
      public:
        B_to_C_counter_t()  {  init();  }
        bool no_temporary_work() const
        {
            return 0==counters[for_all_constellations_C_not_in_R_from_RfnB_4_4-
                                                                   B_TO_C_MIN];
        }
        unsigned char get_work_counter_4_4() const
        {
            return counters[for_all_constellations_C_not_in_R_from_RfnB_4_4 -
                                                                   B_TO_C_MIN];
        }
        void reset_work_counter_4_4()
        {
            counters[for_all_constellations_C_not_in_R_from_RfnB_4_4 -
                                                               B_TO_C_MIN] = 0;
        }
    };
    class state_counter_t : public counter_t<STATE_MIN, STATE_MAX>
    {
      public:
        state_counter_t()  {  init();  }
        bool no_temporary_work() const
        {
            return 0 == counters[
                        while_Test_is_not_empty_3_6l_s_is_in_SpB_and_red_3_9l - 
                                                                  STATE_MIN] &&
                   0 == counters[while_Test_is_not_empty_3_6l_s_is_blue_3_11l -
                                                                  STATE_MIN] &&
                   0 == counters[while_Blue_contains_unvisited_states_3_15l -
                                                                  STATE_MIN] &&
                   0 == counters[while_Red_contains_unvisited_states_3_15r -
                                                                  STATE_MIN] &&
                   0 == counters[for_all_extra_Kripke_states_repl_trans -
                                                                  STATE_MIN];
        }
        bool no_bottom_work() const
        {
            return 0 == counters[for_all_bottom_states_s_in_RfnB_4_8 -
                                                                  STATE_MIN] &&
                   0 == counters[
                            for_all_old_bottom_states_s_in_RedB_selfloop_4_15 -
                                                                    STATE_MIN];
        }
        void is_bottom()
        {
            // set the new bottom counters already to 1, so that this state
            // cannot be handled as new bottom state.
            counters[for_all_bottom_states_s_in_RfnB_4_8 - STATE_MIN] = 1;
            counters[for_all_old_bottom_states_s_in_RedB_selfloop_4_15 -
                                                                STATE_MIN] = 1;
        }
        void blue_is_smaller(unsigned char max_value)
        {
            finalise_work(
                         while_Test_is_not_empty_3_6l_s_is_in_SpB_and_red_3_9l,
                              refine_bottom_state_3_6l_s_is_in_SpB, max_value);
            finalise_work(while_Test_is_not_empty_3_6l_s_is_blue_3_11l,
                                          refine_bottom_state_3_6l, max_value);
            finalise_work(while_Blue_contains_unvisited_states_3_15l,
                                         refine_visited_state_3_15, max_value);
            cancel_work(while_Red_contains_unvisited_states_3_15r,
                                    while_Red_contains_unvisited_states_3_15r);
        }
        void red_is_smaller(unsigned char max_value)
        {
            finalise_work(while_Red_contains_unvisited_states_3_15r,
                                         refine_visited_state_3_15, max_value);
            cancel_work(while_Test_is_not_empty_3_6l_s_is_in_SpB_and_red_3_9l,
                                   while_Blue_contains_unvisited_states_3_15l);
        }
    };
    class trans_counter_t : public counter_t<TRANS_MIN, TRANS_MAX>
    {
      public:
        trans_counter_t()  {  init();  }
        bool no_temporary_work() const
        {
            return 0 == counters[while_Test_is_not_empty_3_6l_s_is_red_3_9l -
                                                                  TRANS_MIN] &&
                   0 == counters[
                    while_Test_is_not_empty_3_6l_s_is_red_3_9l_postprocessing -
                                                                  TRANS_MIN] &&
                   0 == counters[for_all_s_prime_in_pred_s_setminus_Red_3_18l -
                                                                  TRANS_MIN] &&
                   0 == counters[if___s_prime_has_no_transition_to_SpC_3_23l -
                                                                  TRANS_MIN] &&
                   0 == counters[if___s_prime_has_transition_to_SpC_3_23l -
                                                                  TRANS_MIN] &&
                   0 == counters[while_FromRed_is_not_empty_3_6r -
                                                                  TRANS_MIN] &&
                   0 == counters[for_all_s_prime_in_pred_s_3_18r -
                                                                  TRANS_MIN] &&
                   0 == counters[for_all_transitions_in_succ_s_repl_trans -
                                                                    TRANS_MIN];
        }
        bool no_bottom_work() const
        {
            // the function checks the counters that are set to 1 when the
            // source of the transition becomes a new bottom state.  So when
            // the *source* is not a bottom state the function should return
            // true.
            return 0 == counters[
                        refine_outgoing_transition_postprocess_new_bottom_3_6 -
                                                                  TRANS_MIN] &&
                   0 == counters[
                             refine_outgoing_transition_from_new_bottom_3_23l -
                                                                  TRANS_MIN] &&
                   0 == counters[
                          for_all_transitions_from_bottom_states_a_priori_4_4 -
                                                                  TRANS_MIN] &&
                   0 == counters[
                      for_all_transitions_from_bottom_states_a_posteriori_4_4 -
                                                                  TRANS_MIN] &&
                   0 == counters[
                         for_all_transitions_that_need_postproc_a_priori_4_12 -
                                                                  TRANS_MIN] &&
                   0 == counters[
                     for_all_transitions_that_need_postproc_a_posteriori_4_12 -
                                                                  TRANS_MIN] &&
                   0 == counters[for_all_old_bottom_states_s_in_RedB_4_15 -
                                                                    TRANS_MIN];
        }
        void is_bottom()
        {
            // This function initialises transitions that start in a bottom
            // state. It already sets the counters to 1 so that no work can be
            // assigned to this state. 
            counters[refine_outgoing_transition_postprocess_new_bottom_3_6 -
                                                                TRANS_MIN] = 1;
            counters[refine_outgoing_transition_from_new_bottom_3_23l -
                                                                TRANS_MIN] = 1;
            counters[for_all_transitions_from_bottom_states_a_priori_4_4 -
                                                                TRANS_MIN] = 1;
            counters[for_all_transitions_from_bottom_states_a_posteriori_4_4 -
                                                                TRANS_MIN] = 1;
            counters[for_all_transitions_that_need_postproc_a_priori_4_12 -
                                                                TRANS_MIN] = 1;
            counters[for_all_transitions_that_need_postproc_a_posteriori_4_12 -
                                                                TRANS_MIN] = 1;
            counters[for_all_old_bottom_states_s_in_RedB_4_15 - TRANS_MIN] = 1;
        }
        bool add_check_work_notemporary(enum counter_type ctr,
                                                       unsigned char max_value)
        {
            if (TRANS_MIN_TEMPORARY > ctr || ctr > TRANS_MAX_TEMPORARY)
            {
                return add_check_work(ctr, max_value);
            }

            assert(1 == max_value);
            if (0 == counters[ctr - TRANS_MIN])
            {
                counters[ctr - TRANS_MIN] = DONT_COUNT_TEMPORARY;
                return true;
            }

            mCRL2log(log::verbose) << "Error: counter \"" << work_names[ctr]
                                         << "\" exceeded maximum value ("
                                         << (trans_type) max_value << ") for ";
            return false;
        }
        void blue_is_smaller(unsigned char max_value)
        {
            finalise_work(while_Test_is_not_empty_3_6l_s_is_red_3_9l,
                   refine_outgoing_transition_to_marked_state_3_6l, max_value);
            finalise_work(
                     while_Test_is_not_empty_3_6l_s_is_red_3_9l_postprocessing,
                     refine_outgoing_transition_postprocess_new_bottom_3_6, 1);
            finalise_work(for_all_s_prime_in_pred_s_setminus_Red_3_18l,
                                   refine_incoming_transition_3_18, max_value);
            finalise_work(if___s_prime_has_no_transition_to_SpC_3_23l,
                             refine_outgoing_transition_3_6_or_23l, max_value);
            finalise_work(if___s_prime_has_transition_to_SpC_3_23l,
                          refine_outgoing_transition_from_new_bottom_3_23l, 1);
            cancel_work(while_FromRed_is_not_empty_3_6r,
                                              for_all_s_prime_in_pred_s_3_18r);
        }
        void red_is_smaller(unsigned char max_value)
        {
            finalise_work(while_FromRed_is_not_empty_3_6r,
                             refine_outgoing_transition_3_6_or_23l, max_value);
            finalise_work(for_all_s_prime_in_pred_s_3_18r,
                                   refine_incoming_transition_3_18, max_value);
            cancel_work(while_Test_is_not_empty_3_6l_s_is_red_3_9l,
                                     if___s_prime_has_transition_to_SpC_3_23l);
        }
    };


    // static void test_work_names(); // is not actually called, see init().
    static void init(state_type new_n, trans_type new_m)
    {
        // as debugging measure:
        // test_work_names();

        n = new_n;
        m = new_m;
        log_n = log2((double) new_n);
        global_counter.init();
        assert(0 == sensible_work);     sensible_work = 0;
        assert(0 == superfluous_work);  superfluous_work = 0;
    }


    #define add_work(C,ctr,max_value)                                         \
            do                                                                \
            {                                                                 \
                /* mCRL2log(log::debug, "bisim_gjkw") << '('               */ \
                /*                             << (C)->debug_id() << ")."; */ \
                if (!(C)->work_counter.add_check_work((ctr), (max_value)))    \
                {                                                             \
                    mCRL2log(log::verbose) << (C)->debug_id() << '\n';        \
                    exit(EXIT_FAILURE);                                       \
                }                                                             \
            }                                                                 \
            while (0)

    #define add_work_notemporary(C,ctr,max_value)                             \
            do                                                                \
            {                                                                 \
                if (!(C)->work_counter.add_check_work_notemporary((ctr),      \
                                                                (max_value))) \
                {                                                             \
                    mCRL2log(log::verbose) << (C)->debug_id() << '\n';        \
                    exit(EXIT_FAILURE);                                       \
                }                                                             \
            }                                                                 \
            while (0)

    #define add_work_global(ctr,max_value)                                    \
            do                                                                \
            {                                                                 \
                if (!check_complexity::global_counter.add_check_work((ctr),   \
                                                                 (max_value)))\
                {                                                             \
                    mCRL2log(log::verbose) << "the Kripke structure\n";       \
                    exit(EXIT_FAILURE);                                       \
                }                                                             \
            }                                                                 \
            while (0)

};

} // end namespace bisim_gjkw

#else // ifndef NDEBUG

/*
  public:
    class counter_t
    {
      public:
        // counter_t()  {  }
        // void add_work(enum counter_type ctr, unsigned char max_value)  {  }
    };

    class block_counter_t : public counter_t  {  };
    class B_to_C_counter_t : public counter_t  {  };
    class state_counter_t : public counter_t
    {
        // void blue_is_smaller(unsigned char max_value)  {  }
        // void red_is_smaller(unsigned char max_value)  {  }
    };
    class trans_counter_t : public counter_t
    {
        // void blue_is_smaller(unsigned char max_value)  {  }
        // void red_is_smaller(unsigned char max_value)  {  }
    };

};
*/

    #define add_work(C,ctr,max_value)
    #define add_work_notemporary(C,ctr,max_value)
    #define add_work_global(ctr,max_value)

#endif // ifndef NDEBUG

} // end namespace detail
} // end namespace lts
} // end namespace mcrl2

#endif // ifndef _COUNT_ITERATIONS_H
