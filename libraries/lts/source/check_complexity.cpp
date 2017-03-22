// Author(s): David N. Jansen, Radboud Universiteit, Nijmegen, The Netherlands
//
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/// \file count_iterations.cpp
///
/// \brief static variables for time complexity checks during test runs
///
/// \details The macro `MAX_ITERATIONS`, defined in count_iterations.h, uses
/// some static variables, which are defined in this file.
///
/// \author David N. Jansen, Radboud Universiteit, Nijmegen, The Netherlands


#include "mcrl2/lts/detail/check_complexity.h"

namespace mcrl2
{
namespace lts
{
namespace detail
{
namespace bisim_gjkw
{

#ifndef NDEBUG

state_type check_complexity::n = 0;
trans_type check_complexity::m = 0;
unsigned char check_complexity::log_n = '\0';
state_type check_complexity::sensible_work = 0;
state_type check_complexity::superfluous_work = 0;
check_complexity::global_counter_t check_complexity::global_counter;
const char *check_complexity::work_names[TRANS_MAX - GLOBAL_MIN + 1] =
{
    // global counters
    "init_helper: find extra Kripke states",
    "init_helper: initialise blocks",
    "init_helper: initialise B_to_C",
    "init_helper: initialise states",
    "init_helper: initialise constln_slices",
    "init_helper: initialise transitions",

    // constellation counters
    "2.4: while C contains a nontrivial constellation",
    "delete constellations",

    // block counters
    "2.9: Mark all states of SpB as predecessors",
    "2.20: for all refinable blocks RfnB (is splitter)",
    "repl_trans: for all blocks",
    "delete blocks",

    // state counters
    "2.10: for all s in SpB",
    "3.31: for all s in NewB",

    "3.6l: refine bottom state",
    "3.6l: refine bottom state (s is in SpB)",
    "3.15: refine visited state",
    "3.29: Move Blue/Red to a new block NewB (swap states)",
    "3.29: Move Blue/Red to a new block NewB (set pointers to new block)",
    "3.29: Move Blue/Red to a new block NewB (for all states s in NewB)",

    // temporary state counters (blue):
    "3.6l: while Test is not empty (s is in SpB, and 3.9l: s is red)",
    "3.6l: while Test is not empty (3.11l: s is blue)",
    "3.15l: while Blue contains unvisited states",

    // temporary state counters (red):
    "3.15r: while Red contains unvisited states",

    // counters for finalising the data:
    "repl_trans: for all extra Kripke states",

    // new bottom state counters: every state is visited once
    "4.8: for all bottom states s in RfnB",
    "4.15: for all old bottom states s in RedB (self-loop)",
    
    // B_to_C_descriptor counters
    "2.20: for all refinable blocks RfnB",
    "4.4: for all constellations C not in R reachable from RfnB",

    // transition counters: every transition is visited O(log n) times
    "2.11: for all s_prime in in(s)",
    "2.17: Register that inert transitions from s go to NewC (B_to_C)",
    "2.17: Register that inert transitions from s go to NewC (out)",
    "2.17: Register that inert transitions from s go to NewC (swap "
                                                                "transitions)",
    "3.29: Move Blue/Red to a new block NewB (for all transitions in out(s))",
    "3.32l: for all s_prime in in(s)",
    "3.32r: for all s_prime in out(s)",

    "3.6/3.23l: refine outgoing transition",
    "3.6l: refine outgoing transition to marked state",
    "3.18: refine incoming transition",

    // temporary transition counters (blue):
    "3.6l: while Test is not empty (3.9l: s is red)",
    "3.6l: while Test is not empty (3.9l: s is red) during postprocessing",
    "3.18l: for all s_prime in in(s) \\ Red",
    "3.23l: if ... s_prime has no transition to SpC (true)",
    "3.23l: if ... s_prime has no transition to SpC (false)",

    // temporary transition counters (red):
    "3.6r: while FromRed is not empty",
    "3.18r: for all s_prime in in(s)",

    // counters for finalising the data:
    "repl_trans: for all transitions in out(s)",

    // new bottom transition counters: every transition is visited once
    "3.6l: refine outgoing transition (called from PostprocessNewBottom)",
    "3.23l: refine outgoing transition (state is new bottom state)",
    "4.4: for all transitions from new bottom states (a priori)",
    "4.4: for all transitions from new bottom states (a posteriori)",
    "4.12: for all blocks B with transitions to SpC that need postprocessing "
                                                                  "(a priori)",
    "4.12: for all blocks B with transitions to SpC that need postprocessing "
                                                              "(a posteriori)",
    "4.15: for all old bottom states s in RedB"
};

#define test_work_name(var,ctr)                                               \
        do                                                                    \
        {                                                                     \
            assert((var) == check_complexity::ctr);                           \
            ++(var);                                                          \
            mCRL2log(log::debug, "bisim_gjkw") << "work_names[" #ctr "] = \"" \
                       << check_complexity::work_names[check_complexity::ctr] \
                                                                   << "\".\n";\
        }                                                                     \
        while (0)


#if 0
// the function is not really used.  See the code of check_complexity::init()
// below.
static void test_work_names()
{
    int i = check_complexity::GLOBAL_MIN;
    // global counters
    test_work_name(i, find_extra_Kripke_states_init_helper);
    test_work_name(i, initialise_blocks_init_helper);
    test_work_name(i, initialise_B_to_C_init_helper);
    test_work_name(i, initialise_states_init_helper);
    test_work_name(i, init_constln_slices_init_helper);
    test_work_name(i, initialise_transitions_init_helper);
    assert(check_complexity::GLOBAL_MAX + 1 == i);

    // constellation counters
    assert(check_complexity::CONSTLN_MIN == i);
    test_work_name(i, while_C_contains_a_nontrivial_constellation_2_4);
    test_work_name(i, delete_constellations);
    assert(check_complexity::CONSTLN_MAX + 1 == i);

    // block counters
    assert(check_complexity::BLOCK_MIN == i);
    test_work_name(i, Mark_all_states_of_SpB_as_predecessors_2_9);
    test_work_name(i, for_all_refinable_blocks_RfnB_is_splitter_2_20);
    test_work_name(i, for_all_blocks_repl_trans);
    test_work_name(i, delete_blocks);
    assert(check_complexity::BLOCK_MAX + 1 == i);

    // state counters
    assert(check_complexity::STATE_MIN == i);
    test_work_name(i, for_all_s_in_SpB_2_10);
    test_work_name(i, for_all_s_in_NewB_3_31);
    test_work_name(i, refine_bottom_state_3_6l);
    test_work_name(i, refine_bottom_state_3_6l_s_is_in_SpB);
    test_work_name(i, refine_visited_state_3_15);
    test_work_name(i, Move_Blue_or_Red_to_a_new_block_NewB_swap_3_29);
    test_work_name(i, Move_Blue_or_Red_to_a_new_block_NewB_pointer_3_29);
    test_work_name(i, Move_Blue_or_Red_to_a_new_block_states_3_29);

    // temporary state counters (blue):
    test_work_name(i, while_Test_is_not_empty_3_6l_s_is_in_SpB_and_red_3_9l); 
    test_work_name(i, while_Test_is_not_empty_3_6l_s_is_blue_3_11l);
    test_work_name(i, while_Blue_contains_unvisited_states_3_15l);

    // temporary state counters (red):
    test_work_name(i, while_Red_contains_unvisited_states_3_15r);

    // counters for finalising the data:
    test_work_name(i, for_all_extra_Kripke_states_repl_trans);

    // new bottom state counters
    test_work_name(i, for_all_bottom_states_s_in_RfnB_4_8);
    test_work_name(i, for_all_old_bottom_states_s_in_RedB_selfloop_4_15);
    assert(check_complexity::STATE_MAX + 1 == i);

    // B_to_C_descriptor counters
    assert(check_complexity::B_TO_C_MIN == i);
    test_work_name(i, for_all_refinable_blocks_RfnB_2_20);
    // temporary B_to_C_descriptor counters
    test_work_name(i, for_all_constellations_C_not_in_R_from_RfnB_4_4);
    assert(check_complexity::B_TO_C_MAX + 1 == i);

    // transition counters
    assert(check_complexity::TRANS_MIN == i);
    test_work_name(i, for_all_s_prime_in_pred_s_2_11);
    test_work_name(i,
                Register_that_inert_transitions_from_s_go_to_NewC_B_to_C_2_17);
    test_work_name(i,
                  Register_that_inert_transitions_from_s_go_to_NewC_succ_2_17);
    test_work_name(i,
                  Register_that_inert_transitions_from_s_go_to_NewC_swap_2_17);
    test_work_name(i, Move_Blue_or_Red_to_a_new_block_succ_3_29);
    test_work_name(i, for_all_s_prime_in_pred_s_3_32l);
    test_work_name(i, for_all_s_prime_in_succ_s_3_32r);

    test_work_name(i, refine_outgoing_transition_3_6_or_23l);
    test_work_name(i, refine_outgoing_transition_to_marked_state_3_6l);
    test_work_name(i, refine_incoming_transition_3_18);

    // temporary transition counters (blue):
    assert(check_complexity::TRANS_MIN_TEMPORARY == i);
    test_work_name(i, while_Test_is_not_empty_3_6l_s_is_red_3_9l);
    test_work_name(i,
                    while_Test_is_not_empty_3_6l_s_is_red_3_9l_postprocessing);
    test_work_name(i, for_all_s_prime_in_pred_s_setminus_Red_3_18l);
    test_work_name(i, if___s_prime_has_no_transition_to_SpC_3_23l);
    test_work_name(i, if___s_prime_has_transition_to_SpC_3_23l);

    // temporary transition counters (red):
    test_work_name(i, while_FromRed_is_not_empty_3_6r);
    test_work_name(i, for_all_s_prime_in_pred_s_3_18r);
    assert(check_complexity::TRANS_MAX_TEMPORARY + 1 == i);

    // counters for finalising the data:
    test_work_name(i, for_all_transitions_in_succ_s_repl_trans);

    // new bottom transition counters
    test_work_name(i, refine_outgoing_transition_postprocess_new_bottom_3_6);
    test_work_name(i, refine_outgoing_transition_from_new_bottom_3_23l);
    test_work_name(i, for_all_transitions_from_bottom_states_a_priori_4_4);
    test_work_name(i, for_all_transitions_from_bottom_states_a_posteriori_4_4);
    test_work_name(i, for_all_transitions_that_need_postproc_a_priori_4_12);
    test_work_name(i,for_all_transitions_that_need_postproc_a_posteriori_4_12);
    test_work_name(i, for_all_old_bottom_states_s_in_RedB_4_15);
    assert(check_complexity::TRANS_MAX + 1 == i);
}
#endif


void check_complexity::init(state_type new_n, trans_type new_m)
{
    // as debugging measure:
    // test_work_names();

    n = new_n;
    m = new_m;
    log_n = log2((double) new_n);
    for ( int i = 0 ; i <= GLOBAL_MAX - GLOBAL_MIN ; ++i )
    {
        global_counter.counters[i] = 0;
    }
    sensible_work = 0;
    superfluous_work = 0;
}


#endif

} // end namespace bisim_gjkw
} // end namespace detail
} // end namespace lts
} // end namespace mcrl2
