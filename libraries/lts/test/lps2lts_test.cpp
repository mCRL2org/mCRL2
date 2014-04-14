// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lts_test.cpp
/// \brief Add your file description here.

#ifndef MCRL2_WITH_GARBAGE_COLLECTION
#define MCRL2_WITH_GARBAGE_COLLECTION
#endif

#include <stdio.h>
#include <time.h>
#include <iostream>
#include <sstream>
#include <cstdio>
#include <boost/test/included/unit_test_framework.hpp>
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/lts/detail/exploration.h"
#include "mcrl2/lts/lts_aut.h"
#include "mcrl2/lts/lts_fsm.h"
#include "mcrl2/lts/lts_lts.h"
#include "mcrl2/lts/lts_dot.h"
#include "mcrl2/lts/lts_bcg.h"
#include "mcrl2/utilities/test_utilities.h"

using mcrl2::utilities::collect_after_test_case;
using namespace mcrl2;
using namespace mcrl2::lps;



BOOST_GLOBAL_FIXTURE(collect_after_test_case)

template <class LTS_TYPE>
LTS_TYPE translate_lps_to_lts(lps::specification const& specification,
                              lts::exploration_strategy const strategy = lts::es_breadth,
                              mcrl2::data::rewrite_strategy const rewrite_strategy = mcrl2::data::jitty,
                              std::string priority_action = "")
{
  std::clog << "Translating LPS to LTS with exploration strategy " << strategy << ", rewrite strategy " << rewrite_strategy << "." << std::endl;
  lts::lts_generation_options options;
  options.trace_prefix = "lps2lts_test";
  options.specification = specification;
  options.priority_action = priority_action;
  options.strat = rewrite_strategy;
  options.expl_strat = strategy;

  options.lts = utilities::temporary_filename("lps2lts_test_file");

  LTS_TYPE result;
  options.outformat = result.type();
  lts::lps2lts_algorithm lps2lts;
  lps2lts.initialise_lts_generation(&options);
  lps2lts.generate_lts();
  lps2lts.finalise_lts_generation();

  result.load(options.lts);
  remove(options.lts.c_str()); // Clean up after ourselves

  return result;
}

// Configure rewrite strategies to be used.
typedef mcrl2::data::rewrite_strategy rewrite_strategy;
typedef std::vector<rewrite_strategy > rewrite_strategy_vector;

// Configure exploration strategies to be tested;
typedef std::vector< lts::exploration_strategy > exploration_strategy_vector;

static inline
exploration_strategy_vector initialise_exploration_strategies()
{
  exploration_strategy_vector result;
  result.push_back(lts::es_breadth);
  result.push_back(lts::es_depth);
  //result.push_back(lts::es_random);
  return result;
}

static inline
exploration_strategy_vector exploration_strategies()
{
  static exploration_strategy_vector exploration_strategies = initialise_exploration_strategies();
  return exploration_strategies;
}

static void check_lps2lts_specification(std::string const& specification,
                                 const size_t expected_states,
                                 const size_t expected_transitions,
                                 const size_t expected_labels,
                                 std::string priority_action = "")
{
  std::cerr << "CHECK STATE SPACE GENERATION FOR:\n" << specification << "\n";
  lps::specification lps = lps::parse_linear_process_specification(specification);

  rewrite_strategy_vector rstrategies(utilities::get_test_rewrite_strategies(false));
  for (rewrite_strategy_vector::const_iterator rewr_strategy = rstrategies.begin(); rewr_strategy != rstrategies.end(); ++rewr_strategy)
  {
    exploration_strategy_vector estrategies(exploration_strategies());
    for (exploration_strategy_vector::const_iterator expl_strategy = estrategies.begin(); expl_strategy != estrategies.end(); ++expl_strategy)
    {
      std::cerr << "AUT FORMAT\n";
      lts::lts_aut_t result1 = translate_lps_to_lts<lts::lts_aut_t>(lps, *expl_strategy, *rewr_strategy, priority_action);


      BOOST_CHECK_EQUAL(result1.num_states(), expected_states);
      BOOST_CHECK_EQUAL(result1.num_transitions(), expected_transitions);
      BOOST_CHECK_EQUAL(result1.num_action_labels(), expected_labels);

      std::cerr << "LTS FORMAT\n";
      lts::lts_lts_t result2 = translate_lps_to_lts<lts::lts_lts_t>(lps, *expl_strategy, *rewr_strategy, priority_action);


      BOOST_CHECK_EQUAL(result2.num_states(), expected_states);
      BOOST_CHECK_EQUAL(result2.num_transitions(), expected_transitions);
      BOOST_CHECK_EQUAL(result2.num_action_labels(), expected_labels);

      std::cerr << "FSM FORMAT\n";
      lts::lts_fsm_t result3 = translate_lps_to_lts<lts::lts_fsm_t>(lps, *expl_strategy, *rewr_strategy, priority_action);


      BOOST_CHECK_EQUAL(result3.num_states(), expected_states);
      BOOST_CHECK_EQUAL(result3.num_transitions(), expected_transitions);
      BOOST_CHECK_EQUAL(result3.num_action_labels(), expected_labels);

#ifdef USE_BCG
      lts::lts_bcg_t result6 = translate_lps_to_lts<lts::lts_bcg_t>(lps, *expl_strategy, *rewr_strategy, priority_action);

//	bool has_i_label = false;
//	bool has_tau = false;
//        for( process::action_label_list::iterator i = lps.action_labels().begin(); i != lps.action_labels().end(); ++i )
//	{
//                has_i_label = has_i_label || ( i -> name() == atermpp::aterm_string( "i" ) );
//	}

 //       for( lps::action_summand_vector::iterator i = lps.process().action_summands().begin() ;
 //         i != lps.process().action_summands().end() ;
 //         ++i )
 //       {
 //          has_tau = has_tau || i->is_tau() ;
 //       }


      std::cerr << "BCG FORMAT\n";
      BOOST_CHECK_EQUAL(result6.num_states(), expected_states);
      BOOST_CHECK_EQUAL(result6.num_transitions(), expected_transitions);
//        if( has_i_label )
//        {
//          BOOST_CHECK_EQUAL(result6.num_action_labels(), expected_labels );
//        }
//        else
//        {
//            BOOST_CHECK_EQUAL(result6.num_action_labels() , expected_labels + 1 );
//        }
#endif
    }
  }
}

BOOST_AUTO_TEST_CASE(test_a_delta)
{
  std::string lps(
    "act a;\n"
    "proc P(b:Bool) = (b) -> a.P(!b)\n"
    "               + delta;\n"
    "init P(true);\n"
  );
  check_lps2lts_specification(lps, 2, 1, 1);
}

BOOST_AUTO_TEST_CASE(test_abp)
{
  std::string abp(
    "sort Error = struct e;\n"
    "     D = struct d1 | d2;\n"
    "\n"
    "act  i;\n"
    "     c6,r6,s6: Error;\n"
    "     c6,r6,s6,c5,r5,s5: Bool;\n"
    "     c3,r3,s3: Error;\n"
    "     c3,r3,s3,c2,r2,s2: D # Bool;\n"
    "     s4,r1: D;\n"
    "\n"
    "glob dc,dc0,dc1,dc3,dc5,dc7,dc13,dc14,dc15,dc16,dc17,dc18: D;\n"
    "     dc2,dc4,dc6,dc8,dc9,dc10,dc11,dc12: Bool;\n"
    "\n"
    "proc P(s30_S: Pos, d_S: D, b_S: Bool, s31_K: Pos, d_K: D, b_K: Bool, s32_L: Pos, b_L: Bool, s33_R: Pos, d_R: D, b_R: Bool) =\n"
    "       sum d0_S: D.\n"
    "         (s30_S == 1) ->\n"
    "         r1(d0_S) .\n"
    "         P(s30_S = 2, d_S = d0_S)\n"
    "     + sum e0_K: Bool.\n"
    "         (s31_K == 2) ->\n"
    "         i .\n"
    "         P(s31_K = if(e0_K, 4, 3), d_K = if(e0_K, dc3, d_K), b_K = if(e0_K, dc4, b_K))\n"
    "     + sum e1_L: Bool.\n"
    "         (s32_L == 2) ->\n"
    "         i .\n"
    "         P(s32_L = if(e1_L, 4, 3), b_L = if(e1_L, dc10, b_L))\n"
    "     + (s33_R == 2) ->\n"
    "         s4(d_R) .\n"
    "         P(s33_R = 3, d_R = dc16)\n"
    "     + sum e2_R: Bool.\n"
    "         (s32_L == 1 && if(e2_R, s33_R == 4, s33_R == 3)) ->\n"
    "         c5(if(e2_R, !b_R, b_R)) .\n"
    "         P(s32_L = 2, b_L = if(e2_R, !b_R, b_R), s33_R = 1, d_R = if(e2_R, dc18, dc17), b_R = if(e2_R, b_R, !b_R))\n"
    "     + (s31_K == 4 && s33_R == 1) ->\n"
    "         c3(e) .\n"
    "         P(s31_K = 1, d_K = dc7, b_K = dc8, s33_R = 4, d_R = dc15)\n"
    "     + sum e3_R: Bool.\n"
    "         ((s31_K == 3 && s33_R == 1) && if(e3_R, !b_R, b_R) == b_K) ->\n"
    "         c3(d_K, if(e3_R, !b_R, b_R)) .\n"
    "         P(s31_K = 1, d_K = dc5, b_K = dc6, s33_R = if(e3_R, 4, 2), d_R = if(e3_R, dc14, d_K))\n"
    "     + (s30_S == 2 && s31_K == 1) ->\n"
    "         c2(d_S, b_S) .\n"
    "         P(s30_S = 3, s31_K = 2, d_K = d_S, b_K = b_S)\n"
    "     + (s30_S == 3 && s32_L == 4) ->\n"
    "         c6(e) .\n"
    "         P(s30_S = 2, s32_L = 1, b_L = dc12)\n"
    "     + sum e_S: Bool.\n"
    "         ((s30_S == 3 && s32_L == 3) && if(e_S, b_S, !b_S) == b_L) ->\n"
    "         c6(if(e_S, b_S, !b_S)) .\n"
    "         P(s30_S = if(e_S, 1, 2), d_S = if(e_S, dc0, d_S), b_S = if(e_S, !b_S, b_S), s32_L = 1, b_L = dc11)\n"
    "     + delta;\n"
    "\n"
    "init P(1, dc, true, 1, dc1, dc2, 1, dc9, 1, dc13, true);\n"
  );
  check_lps2lts_specification(abp, 74, 92, 19);
  check_lps2lts_specification(abp, 74, 92, 19, "tau");
}

BOOST_AUTO_TEST_CASE(test_confluence)
{
  std::string spec(
    "sort State = struct S_FSM_UNINITIALIZED?isS_FSM_UNINITIALIZED | S_OFF?isS_OFF;\n"
    "     IdList = List(Nat);\n"
    "\n"
    "act  rs: State;\n"
    "\n"
    "proc P(s3_P: Pos, s1_P: State, ActPhaseArgs_pp2_P: IdList) =\n"
    "       (s3_P == 1 && ActPhaseArgs_pp2_P == []) ->\n"
    "         tau .\n"
    "         P(s3_P = 1, s1_P = S_FSM_UNINITIALIZED, ActPhaseArgs_pp2_P = [12, 9])\n"
    "     + sum s11_P: State.\n"
    "         (s3_P == 1 && !(ActPhaseArgs_pp2_P == [])) ->\n"
    "         rs(s11_P) .\n"
    "         P(s3_P = 2, s1_P = s11_P)\n"
    "     + delta;\n"
    "\n"
    "init P(1, S_FSM_UNINITIALIZED, []);\n"
  );
  check_lps2lts_specification(spec, 4, 3, 3);
  check_lps2lts_specification(spec, 3, 2, 2, "tau");
}

BOOST_AUTO_TEST_CASE(test_function_updates)
{
  std::string spec(
    "act  set,s: Pos;\n"
    "map  f:Pos->Bool;\n"
    "var  n:Pos;\n"
    "eqn  f(n)=false;\n"
    "\n"
    "proc P(b_Sensor: Pos -> Bool) =\n"
    "       sum n_Sensor: Pos.\n"
    "         (n_Sensor <= 2) ->\n"
    "         s(n_Sensor) .\n"
    "         P(b_Sensor = b_Sensor[n_Sensor -> true])\n"
    "     + sum n_Sensor0: Pos.\n"
    "         (b_Sensor(n_Sensor0) && n_Sensor0 <= 2) ->\n"
    "         set(n_Sensor0) .\n"
    "         P(b_Sensor = b_Sensor[n_Sensor0 -> false])\n"
    "     + delta;\n"
    "\n"
    "init P(f);\n"
  );
  check_lps2lts_specification(spec, 4, 12, 4);
}

BOOST_AUTO_TEST_CASE(test_timed) // For bug #756
{
  std::string spec(
    "act  a,Terminate;\n"
    "\n"
    "proc P(s3: Pos) =\n"
    "       (s3 == 1) ->\n"
    "         a @ 3 .\n"
    "         P(s3 = 2)\n"
    "     + (s3 == 2) ->\n"
    "         Terminate .\n"
    "         P(s3 = 3)\n"
    "     + (s3 == 3) ->\n"
    "         delta;\n"
    "\n"
    "init P(1);\n"
  );
  check_lps2lts_specification(spec, 3, 2, 2);
}

BOOST_AUTO_TEST_CASE(test_struct)
{
  std::string spec(
    "sort Bits = struct b0 | b1;\n"
    "     t_sys_regset_fsm_state = Bits;\n"
    "     t_timer_counter_fsm_state = Bits;\n"
    "map  timer_counter_fsm_state_idle: Bits;\n"
    "act  a: t_sys_regset_fsm_state;\n"
    "glob globd: t_sys_regset_fsm_state;\n"
    "proc P(s3_P: Pos) =\n"
    "       (s3_P == 1) ->\n"
    "         a(globd) .\n"
    "         P(s3_P = 2)\n"
    "     + delta;\n"
    "init P(1);\n"
  );
  check_lps2lts_specification(spec, 2, 1, 1);
}

BOOST_AUTO_TEST_CASE(test_alias_complex)
{
  std::string spec(
    "sort Bits = struct singleBit(bit: Bool)?isSingleBit | bitVector(bitVec: List(Bool))?isBitVector;\n"
    "     t_sys_regset_fsm_state = Bits;\n"
    "     t_timer_counter_fsm_state = Bits;\n"
    "\n"
    "map  repeat_rec: Bool # Nat -> List(Bool);\n"
    "     repeat: Bool # Nat -> Bits;\n"
    "\n"
    "var  b: Bool;\n"
    "     n: Nat;\n"
    "eqn  repeat(b, n)  =  if(n <= 1, singleBit(b), bitVector(repeat_rec(b, n)));\n"
    "\n"
    "act  a: t_sys_regset_fsm_state;\n"
    "\n"
    "proc P(s3: Pos) =\n"
    "       (s3 == 1) ->\n"
    "         a(repeat(true, 32)) .\n"
    "         P(s3 = 2)\n"
    "     + delta;\n"
    "\n"
    "init P(1);\n"
  );
  check_lps2lts_specification(spec, 2, 1, 1);
}

BOOST_AUTO_TEST_CASE(test_equality_with_empty_set)
{
  std::string spec(
    "map  emptyset:Set(Bool);\n"
    "eqn  emptyset={};\n"
    "proc P=({ b: Bool | false }  == emptyset) -> tau.P;\n"
    "init P;"
  );
  check_lps2lts_specification(spec, 1, 1, 1);
}

BOOST_AUTO_TEST_CASE(test_equality_of_finite_sets)
{
  std::string spec(
    "sort R = struct r2 | r1 ;\n"
    "map All: Set(R);\n"
    "eqn All = {  r: R | true };\n"
    "proc P=({r1, r2} == All) -> tau.P;\n"
    "init P;"
  );
  check_lps2lts_specification(spec, 1, 1, 1);
}

// Example from bug #832
BOOST_AUTO_TEST_CASE(test_plus)
{
  // This example provides two identical transitions between a state.
  // There is a discussion on whether this is desirable. Currently, for
  // efficiency reasons such extra transitions are not removed by lps2lts.
  std::string spec(
    "act a;\n"
    "proc P = a.P + a.P;\n"
    "init P;\n"
  );
  check_lps2lts_specification(spec, 1, 2, 1);
}

// The example below fails if #[0,1] does not have a decent
// type. The tricky thing is that the type of the list can be List(Nat),
// List(Int) or List(Real). Toolset version 10180 resolved this by
// delivering the type List({Nat, Int, Real}), i.e. a set of possible
// options. But this was not expected and understood by the other tools.
// This is related to bug report #949.
BOOST_AUTO_TEST_CASE(test_well_typedness_of_length_of_list_of_numbers)
{
  std::string spec(
    "proc B (i:Int) = (i >= 2) -> tau. B();\n"
    "init B(#[0,1]);\n"
  );
  check_lps2lts_specification(spec, 1, 1, 1);
}

#if 0 // This test has been disabled; it was decided not to fix this issue.
// The following example illustrates that enumeration can sometimes need
// a large stack depth.
// The example was attached to bug #1019, and fails with limited stack,
// succeeds with unlimited stack.
BOOST_AUTO_TEST_CASE(test_deep_stack)
{
  std::string spec(
    "sort Packet = struct packet(d0: Bool, Bool, Bool, Bool, Bool, Bool, Bool, Bool, Bool, Bool, Bool, Bool, Bool, Bool, Bool, Bool, Bool, Bool, Bool);\n"
    "act  Terminate;\n"
    "proc P(s3: Pos) =\n"
    "   (s3 == 2) ->\n"
    "     Terminate .\n"
    "     P(s3 = 3)\n"
    " + sum p: Packet.\n"
    "     (s3 == 1) ->\n"
    "     tau .\n"
    "     P(s3 = 2)\n"
    " + delta;\n"
    "init P(1);\n"
  );
  check_lps2lts_specification(spec, 3, 524289, 2);
}
#endif // false


BOOST_AUTO_TEST_CASE(test_max_states)
{
  std::string spec(
  "act a;\n"
  "proc P(s: Pos) =\n"
  "  (s <= 10) -> a . P(s+1);\n"
  "init P(1);\n");

  lps::specification specification = lps::parse_linear_process_specification(spec);

  lts::lts_generation_options options;
  options.trace_prefix = "lps2lts_test";
  options.specification = specification;
  options.lts = utilities::temporary_filename("lps2lts_test_file");
  options.max_states = 5;

  lts::lts_aut_t result;
  options.outformat = result.type();
  lts::lps2lts_algorithm lps2lts;
  lps2lts.initialise_lts_generation(&options);
  lps2lts.generate_lts();
  lps2lts.finalise_lts_generation();
  result.load(options.lts);
  remove(options.lts.c_str()); // Clean up after ourselves

  BOOST_CHECK_LT(result.num_states(), 10);
}

BOOST_AUTO_TEST_CASE(test_interaction_sum_and_assignment_notation1)
{
  std::string spec(
    "proc B (i:Bool) = sum i:Bool.tau. B();\n"
    "init B(true);\n"
  );
  check_lps2lts_specification(spec, 1, 2, 1);
}

BOOST_AUTO_TEST_CASE(test_interaction_sum_and_assignment_notation2)
{
  std::string spec(
    "proc B (i:Bool) = sum i:Bool.tau. B(i=i);\n"
    "init B(true);\n"
  );
  check_lps2lts_specification(spec, 2, 4, 1);
}

BOOST_AUTO_TEST_CASE(test_whether_function_update_is_declared)
{
  std::string spec(
    "sort  T = struct a|b;\n"
    "act int;\n"
    "map g: Bool -> T;\n"
    "var y: Bool;\n"
    "eqn g(y) = a;\n"
    "act d: Bool -> T;\n"
    "proc P(f: Bool -> T) = int . P(f[true->b]);\n"
    "init P(g) ;\n"
  );
  check_lps2lts_specification(spec, 2, 2, 1);
}

BOOST_AUTO_TEST_CASE(test_whether_bag_enumeration_with_similar_elements_is_allowed)
{
  std::string spec(
    "sort S = struct s1 | s2;\n"
    "act int;\n"
    "proc P(b: Bag(S)) = (count(s1, b) < 3) -> int.P();\n"
    "init P({s1: 1, s1: 1});\n"
  );
  check_lps2lts_specification(spec, 1, 1, 1);
}
  
BOOST_AUTO_TEST_CASE(test_whether_functions_to_functions_are_causing_problems)
{
  std::string spec(
    "map f:Nat->Nat->Nat->Nat;\n"
    "    g:Nat->Nat;\n"
    "var x,y,z:Nat;"
    "eqn f(x)(y)(z)=x+y+z;\n"
    "    f(x)(y)=g;\n"
    "act int:Nat;\n"
    "proc P = int(f(1)(2)(3)).P();\n"
    "init P;\n"
  );
  check_lps2lts_specification(spec, 1, 1, 1);
}
  


boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
 // Initialise random seed to allow parallel running with lps2lts_test_old
  return 0;
}

