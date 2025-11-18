// Author(s): mCRL2 team
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
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

#define BOOST_TEST_MODULE lps2lts_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/data/detail/rewrite_strategies.h"
#include "mcrl2/lps/is_stochastic.h"
#include "mcrl2/lts/state_space_generator.h"
#include "mcrl2/lts/stochastic_lts_builder.h"
#include "mcrl2/utilities/test_utilities.h"

using namespace mcrl2;
using namespace mcrl2::lps;

template <bool Stochastic, bool Timed, typename Specification, typename LTSBuilder>
void generate_state_space(const Specification& lpsspec, LTSBuilder& builder, const std::string& output_filename, const lps::explorer_options& options)
{
  data::rewriter rewr = lps::construct_rewriter(lpsspec, options.rewrite_strategy, options.remove_unused_rewrite_rules);
  lps::explorer<Stochastic, Timed, Specification> explorer(lpsspec, options, rewr);
  lts::state_space_generator<Stochastic, Timed, Specification> generator(lpsspec, options, explorer);
  generator.explore(builder);
  builder.save(output_filename);
}

std::string file_extension(lts::lts_type output_format)
{
  switch (output_format)
  {
    case lts::lts_lts: return ".lts";
    case lts::lts_aut: return ".aut";
    case lts::lts_fsm: return ".fsm";
    case lts::lts_dot: return ".dot";
    default: throw mcrl2::runtime_error("unsupported format");
  }
}

void run_generatelts(
  const lps::stochastic_specification& stochastic_lpsspec,
  data::rewrite_strategy rstrategy,
  lps::exploration_strategy estrategy,
  lts::lts_type output_format,
  const std::string& outputfile,
  const std::string& priority_action
)
{
  lps::explorer_options options;
  options.trace_prefix = "lps2lts_test";
  options.confluence_action = priority_action;
  options.rewrite_strategy = rstrategy;
  options.search_strategy = estrategy;
  options.save_at_end = true;

  bool is_timed = stochastic_lpsspec.process().has_time();

  if (lps::is_stochastic(stochastic_lpsspec))
  {
    auto builder = create_stochastic_lts_builder(stochastic_lpsspec, options, output_format);
    if (is_timed)
    {
      generate_state_space<true, true>(stochastic_lpsspec, *builder, outputfile, options);
    }
    else
    {
      generate_state_space<true, false>(stochastic_lpsspec, *builder, outputfile, options);
    }
  }
  else
  {
    lps::specification lpsspec = lps::remove_stochastic_operators(stochastic_lpsspec);
    auto builder = create_lts_builder(lpsspec, options, output_format);
    if (is_timed)
    {
      generate_state_space<false, true>(lpsspec, *builder, outputfile, options);
    }
    else
    {
      generate_state_space<false, false>(lpsspec, *builder, outputfile, options);
    }
  }
}

template <typename LTSType>
void check_lts(
  const std::string& format,
  const lps::stochastic_specification& stochastic_lpsspec,
  data::rewrite_strategy rstrategy,
  lps::exploration_strategy estrategy,
  std::size_t expected_states,
  std::size_t expected_transitions,
  std::size_t expected_labels,
  const std::string& priority_action = ""
)
{
  std::cerr << "Translating LPS to LTS with exploration strategy " << estrategy << ", rewrite strategy " << rstrategy << "." << std::endl;
  std::cerr << format << " FORMAT\n";
  LTSType result;
  lts::lts_type output_format = result.type();
  std::string outputfile = static_cast<std::string>(boost::unit_test::framework::current_test_case().p_name) + ".generatelts" + file_extension(output_format);
  run_generatelts(stochastic_lpsspec, rstrategy, estrategy, output_format, outputfile, priority_action);
  result.load(outputfile);

  BOOST_CHECK_EQUAL(result.num_states(), expected_states);
  BOOST_CHECK_EQUAL(result.num_transitions(), expected_transitions);
  BOOST_CHECK_EQUAL(result.num_action_labels(), expected_labels);

  std::remove(outputfile.c_str());
}

static void check_lps2lts_specification(const std::string& specification,
                                        const std::size_t expected_states,
                                        const std::size_t expected_transitions,
                                        const std::size_t expected_labels,
                                        const std::string& priority_action = "")
{
  std::cerr << "CHECK STATE SPACE GENERATION FOR:\n" << specification << "\n";
  lps::stochastic_specification lpsspec;
  parse_lps(specification, lpsspec);
  bool contains_probabilities = lps::is_stochastic(lpsspec);

  for (data::rewrite_strategy rstrategy: data::detail::get_test_rewrite_strategies(false))
  {
    for (lps::exploration_strategy estrategy: { lps::es_breadth, lps::es_depth })
    {
      if (contains_probabilities)
      {
        check_lts<lts::probabilistic_lts_aut_t>("PROBABILISTIC AUT", lpsspec, rstrategy, estrategy, expected_states, expected_transitions, expected_labels, priority_action);
        check_lts<lts::probabilistic_lts_lts_t>("PROBABILISTIC LTS", lpsspec, rstrategy, estrategy, expected_states, expected_transitions, expected_labels, priority_action);
        check_lts<lts::probabilistic_lts_fsm_t>("PROBABILISTIC FSM", lpsspec, rstrategy, estrategy, expected_states, expected_transitions, expected_labels, priority_action);
      }
      else
      {
        check_lts<lts::lts_aut_t>("AUT", lpsspec, rstrategy, estrategy, expected_states, expected_transitions, expected_labels, priority_action);
        check_lts<lts::lts_lts_t>("LTS", lpsspec, rstrategy, estrategy, expected_states, expected_transitions, expected_labels, priority_action);
        check_lts<lts::lts_fsm_t>("FSM", lpsspec, rstrategy, estrategy, expected_states, expected_transitions, expected_labels, priority_action);
      }
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
  check_lps2lts_specification(lps, 2, 1, 2);
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
  check_lps2lts_specification(abp, 74, 92, 20);
  check_lps2lts_specification(abp, 74, 92, 20, "tau");
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
  check_lps2lts_specification(spec, 3, 2, 3, "tau");
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
  check_lps2lts_specification(spec, 4, 12, 5);
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
  check_lps2lts_specification(spec, 3, 2, 3);
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
  check_lps2lts_specification(spec, 2, 1, 2);
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
  check_lps2lts_specification(spec, 2, 1, 2);
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
  check_lps2lts_specification(spec, 1, 2, 2);
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
  check_lps2lts_specification(spec, 2, 2, 2);
}

BOOST_AUTO_TEST_CASE(test_whether_bag_enumeration_with_similar_elements_is_allowed)
{
  std::string spec(
    "sort S = struct s1 | s2;\n"
    "act int;\n"
    "proc P(b: Bag(S)) = (count(s1, b) < 3) -> int.P();\n"
    "init P({s1: 1, s1: 1});\n"
  );
  check_lps2lts_specification(spec, 1, 1, 2);
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
  check_lps2lts_specification(spec, 1, 1, 2);
}

BOOST_AUTO_TEST_CASE(test_whether_functions_can_be_enumerated)
{
  std::string spec(
    "act int:Bool->Bool;\n"
    "proc P = sum f:Bool->Bool.int(f).P;\n"
    "init P;\n"
  );
  check_lps2lts_specification(spec, 1, 4, 5);
}

BOOST_AUTO_TEST_CASE(test_whether_functions_with_more_arguments_can_be_enumerated)
{
  std::string spec(
    "act int:Bool#Bool#Bool->Bool;\n"
    "proc P = sum f:Bool#Bool#Bool->Bool.f(true,true,true)->int(f).P;\n"
    "init P;\n"
  );
  check_lps2lts_specification(spec, 1, 128, 129);
}

BOOST_AUTO_TEST_CASE(test_whether_finite_sets_can_be_enumerated)
{
  std::string spec(
    "act int:FSet(Bool);\n"
    "proc P = sum f:FSet(Bool).int(f).P;\n"
    "init P;\n"
  );
  check_lps2lts_specification(spec, 1, 4, 5);
}

BOOST_AUTO_TEST_CASE(test_whether_sets_can_be_enumerated)
{
  std::string spec(
    "act int:FSet(Bool);\n"
    "proc P = sum f:FSet(Bool).int(f).P;\n"
    "init P;\n"
  );
  check_lps2lts_specification(spec, 1, 4, 5);
}

BOOST_AUTO_TEST_CASE(test_whether_finite_sets_with_conditions_can_be_enumerated)
{
  std::string spec(
    "act int:FSet(Bool);\n"
    "proc P = sum f:FSet(Bool).(true in f) -> int(f).P;\n"
    "init P;\n"
  );
  check_lps2lts_specification(spec, 1, 2, 3);
}

BOOST_AUTO_TEST_CASE(test_whether_sets_with_conditions_can_be_enumerated)
{
  std::string spec(
    "act int:Set(Bool);\n"
    "proc P = sum f:Set(Bool).(true in f) -> int(f).P;\n"
    "init P;\n"
  );
  check_lps2lts_specification(spec, 1, 2, 3);
}

BOOST_AUTO_TEST_CASE(test_whether_finite_sets_of_functions_can_be_enumerated)
{
  std::string spec(
    "act int:FSet(Bool->Bool);\n"
    "proc P = sum f:FSet(Bool->Bool).((lambda x:Bool.true) in f) -> int(f).P;\n"
    "init P;\n"
  );
  check_lps2lts_specification(spec, 1, 8, 9);
}

BOOST_AUTO_TEST_CASE(test_whether_sets_of_functions_can_be_enumerated)
{
  std::string spec(
    "act int:Set(Bool->Bool);\n"
    "proc P = sum f:Set(Bool->Bool).((lambda x:Bool.true) in f) -> int(f).P;\n"
    "init P;\n"
  );
  check_lps2lts_specification(spec, 1, 8, 9);
}

BOOST_AUTO_TEST_CASE(test_whether_probabilistic_state_spaces_are_generated_correctly)   // Related to #1542
{
  std::string spec(
    "act a;\n"
    "proc P(b:Bool) = b->a.P(false);\n"
    "init dist b: Bool[if(b, 1 / 4, 3 / 4)] . P(b);\n"
  );
  check_lps2lts_specification(spec, 2, 1, 2);
}

BOOST_AUTO_TEST_CASE(coins_simulate_dice)   // Example from the probabilistic examples directory. 
{
    std::string spec(
    "act  flip: Bool;\n"
    "     dice: Nat;\n"
    "\n"
    "glob dc,dc3,dc4,dc5,dc6,dc7,dc8,dc9,dc10,dc11,dc12,dc13,dc14,dc15,dc16,dc17,dc18,dc19: Bool;\n"
    "     dc1,dc2: Nat;\n"
    "\n"
    "proc P(s1: Pos, b3,b2: Bool, s,d: Nat) =\n"
    "       (s1 == 1 && b2 && s == 0) ->\n"
    "         flip(b2) .\n"
    "         dist b2: Bool[1 / 2] .\n"
    "         P(s1 = 1, b3 = dc3, b2 = b2, s = 1, d = 0)\n"
    "     + (s1 == 1 && !b2 && s == 0) ->\n"
    "         flip(b2) .\n"
    "         dist b2: Bool[1 / 2] .\n"
    "         P(s1 = 1, b3 = dc4, b2 = b2, s = 2, d = 0)\n"
    "     + (s1 == 1 && b2 && s == 1 && !(s == 0)) ->\n"
    "         flip(b2) .\n"
    "         dist b2: Bool[1 / 2] .\n"
    "         P(s1 = 1, b3 = dc5, b2 = b2, s = 3, d = 0)\n"
    "     + (s1 == 1 && !b2 && s == 1 && !(s == 0)) ->\n"
    "         flip(b2) .\n"
    "         dist b2: Bool[1 / 2] .\n"
    "         P(s1 = 1, b3 = dc6, b2 = b2, s = 4, d = 0)\n"
    "     + (s1 == 1 && b2 && s == 2 && !(s == 1) && !(s == 0)) ->\n"
    "         flip(b2) .\n"
    "         dist b2: Bool[1 / 2] .\n"
    "         P(s1 = 1, b3 = dc7, b2 = b2, s = 5, d = 0)\n"
    "     + (s1 == 1 && !b2 && s == 2 && !(s == 1) && !(s == 0)) ->\n"
    "         flip(b2) .\n"
    "         dist b2: Bool[1 / 2] .\n"
    "         P(s1 = 1, b3 = dc8, b2 = b2, s = 6, d = 0)\n"
    "     + (s1 == 1 && b2 && s == 3 && !(s == 2) && !(s == 1) && !(s == 0)) ->\n"
    "         flip(b2) .\n"
    "         dist b2: Bool[1 / 2] .\n"
    "         P(s1 = 1, b3 = dc9, b2 = b2, s = 1, d = 0)\n"
    "     + (s1 == 1 && !b2 && s == 3 && !(s == 2) && !(s == 1) && !(s == 0)) ->\n"
    "         flip(b2) .\n"
    "         dist b2: Bool[1 / 2] .\n"
    "         P(s1 = 1, b3 = dc10, b2 = b2, s = 7, d = 1)\n"
    "     + (s1 == 1 && b2 && s == 4 && !(s == 3) && !(s == 2) && !(s == 1) && !(s == 0)) ->\n"
    "         flip(b2) .\n"
    "         dist b2: Bool[1 / 2] .\n"
    "         P(s1 = 1, b3 = dc11, b2 = b2, s = 7, d = 2)\n"
    "     + (s1 == 1 && !b2 && s == 4 && !(s == 3) && !(s == 2) && !(s == 1) && !(s == 0)) ->\n"
    "         flip(b2) .\n"
    "         dist b2: Bool[1 / 2] .\n"
    "         P(s1 = 1, b3 = dc12, b2 = b2, s = 7, d = 3)\n"
    "     + (s1 == 1 && b2 && s == 5 && !(s == 4) && !(s == 3) && !(s == 2) && !(s == 1) && !(s == 0)) ->\n"
    "         flip(b2) .\n"
    "         dist b2: Bool[1 / 2] .\n"
    "         P(s1 = 1, b3 = dc13, b2 = b2, s = 7, d = 4)\n"
    "     + (s1 == 1 && !b2 && s == 5 && !(s == 4) && !(s == 3) && !(s == 2) && !(s == 1) && !(s == 0)) ->\n"
    "         flip(b2) .\n"
    "         dist b2: Bool[1 / 2] .\n"
    "         P(s1 = 1, b3 = dc14, b2 = b2, s = 7, d = 5)\n"
    "     + (s1 == 1 && b2 && s == 6 && !(s == 5) && !(s == 4) && !(s == 3) && !(s == 2) && !(s == 1) && !(s == 0)) ->\n"
    "         flip(b2) .\n"
    "         dist b2: Bool[1 / 2] .\n"
    "         P(s1 = 1, b3 = dc15, b2 = b2, s = 7, d = 6)\n"
    "     + (s1 == 1 && !b2 && s == 6 && !(s == 5) && !(s == 4) && !(s == 3) && !(s == 2) && !(s == 1) && !(s == 0)) ->\n"
    "         flip(b2) .\n"
    "         dist b2: Bool[1 / 2] .\n"
    "         P(s1 = 1, b3 = dc16, b2 = b2, s = 2, d = 0)\n"
    "     + (s1 == 1 && s == 7) ->\n"
    "         dice(d) .\n"
    "         dist b2: Bool[1 / 2] .\n"
    "         P(s1 = 1, b3 = dc17, b2 = b2)\n"
    "     + (s1 == 2 && b3) ->\n"
    "         flip(b3) .\n"
    "         dist b2: Bool[1 / 2] .\n"
    "         P(s1 = 1, b3 = dc18, b2 = b2, s = 1, d = 0)\n"
    "     + (s1 == 2 && !b3) ->\n"
    "         flip(b3) .\n"
    "         dist b2: Bool[1 / 2] .\n"
    "         P(s1 = 1, b3 = dc19, b2 = b2, s = 2, d = 0)\n"
    "     + delta;\n"
    "\n"
    "init dist b3: Bool[1 / 2] . P(2, b3, dc, dc1, dc2);\n"
  );
  check_lps2lts_specification(spec, 26, 26, 9);
} 

BOOST_AUTO_TEST_CASE(test_whether_action_a_b_and_b_a_are_the_same)   // Related to #1595
{
  std::string spec(
    "act a,b;\n"
    "proc P = a|b.P+\n"
    "         b|a.P;\n"
    "init P;\n"
  );
  check_lps2lts_specification(spec, 1, 2, 2);
}


