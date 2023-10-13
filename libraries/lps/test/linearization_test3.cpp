// Author(s): Wieger Wesselink, Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file linearization_test3.cpp
/// \brief Add your file description here.

#define BOOST_TEST_MODULE linearization_test3
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/data/detail/rewrite_strategies.h"
#include "mcrl2/lps/is_well_typed.h"
#include "mcrl2/lps/linearise.h"

using namespace mcrl2;
using namespace mcrl2::lps;

typedef data::rewriter::strategy rewrite_strategy;
typedef std::vector<rewrite_strategy> rewrite_strategy_vector;

void run_linearisation_instance(const std::string& spec, const t_lin_options& options, bool expect_success)
{
  if (expect_success)
  {
    BOOST_CHECK(mcrl2::lps::detail::is_well_typed(linearise(spec, options)));
  }
  else
  {
    BOOST_CHECK_THROW(linearise(spec, options), mcrl2::runtime_error);
  }
}

void run_linearisation_test_case(const std::string& spec, const bool expect_success = true)
{
  // Set various rewrite strategies
  rewrite_strategy_vector rewrite_strategies = data::detail::get_test_rewrite_strategies(false);

  for (rewrite_strategy_vector::const_iterator i = rewrite_strategies.begin(); i != rewrite_strategies.end(); ++i)
  {
    std::clog << std::endl << "Testing with rewrite strategy " << *i << std::endl;

    t_lin_options options;
    options.rewrite_strategy=*i;

    std::clog << "  Default options" << std::endl;
    run_linearisation_instance(spec, options, expect_success);

    std::clog << "  Linearisation method regular2" << std::endl;
    options.lin_method=lmRegular2;
    run_linearisation_instance(spec, options, expect_success);

    std::clog << "  Linearisation method stack" << std::endl;
    options.lin_method=lmStack;
    run_linearisation_instance(spec, options, expect_success);

    std::clog << "  Linearisation method stack; binary enabled" << std::endl;
    options.binary=true;
    run_linearisation_instance(spec, options, expect_success);

    std::clog << "  Linearisation method regular; binary enabled" << std::endl;
    options.lin_method=lmRegular;
    run_linearisation_instance(spec, options, expect_success);

    std::clog << "  Linearisation method regular; no intermediate clustering" << std::endl;
    options.binary=false; // reset binary
    options.no_intermediate_cluster=true;
    run_linearisation_instance(spec, options, expect_success);
  }
}

BOOST_AUTO_TEST_CASE(The_unreachability_of_tau_is_not_properly_recognized)
{
  const std::string spec =
     "init (true -> delta <> delta) . tau;";

  run_linearisation_test_case(spec,true);
}

BOOST_AUTO_TEST_CASE(Moving_a_distribution_out_of_a_process_is_tricky)
{
  const std::string spec =
     "map  N:Pos;\n"
     "eqn  N=2;\n"
     "\n"
     "act  last_passenger_has_his_own_seat:Bool;\n"
     "     enter_plane:Bool#Bool;\n"
     "     enter;\n"
     "\n"
     "\n"
     "proc Plane(everybody_has_his_own_seat:Bool, number_of_empty_seats:Int)=\n"
     "             (enter.\n"
     "                dist b0:Bool[if(everybody_has_his_own_seat,if(b0,1,0),if(b0,1-1/number_of_empty_seats,1/number_of_empty_seats))].\n"
     "                b0 -> enter_plane(true,false).delta.Plane(everybody_has_his_own_seat,number_of_empty_seats-1)\n"
     "                   <>dist b1:Bool[if(b1,1/number_of_empty_seats,1-1/number_of_empty_seats)].\n"
     "                      enter_plane(false,b1).delta\n"
     "             );\n"
     "\n"
     "\n"
     "init dist b:Bool[if(b,1/N,(N-1)/N)].Plane(b,N-1);\n";

  run_linearisation_test_case(spec,true);
}

/* The test below failed with type checking (only in debug mode). The reason
 * is that the set {0,1} would obtain terms with different types (int, nat),
 * which is not a well typed set. This was reported by Muhammad Atif, error #1526. */
BOOST_AUTO_TEST_CASE(type_checking_a_finite_set_with_numbers_can_go_astray)
{
  const std::string spec =
     "act sendInt, rcvInt, transferInt:Int;\n"
     "\n"
     "proc P (bg:Bag(Int))=(count(2,bg)<2)->sum i:Int.rcvInt(i).P(Set2Bag({i})+bg);\n"
     "     Px(c:Nat)= (c<2)->sum x:Int.(x<3&&x>-2)->sendInt(x).Px(c+1);\n"
     "     Py(c:Nat)= (c<2)->sum x:Int.(x<3&&x>-2)->sendInt(x).Py(c+1);\n"
     "\n"
     "init allow({transferInt},\n"
     "       comm({sendInt|rcvInt->transferInt},\n"
     "           P(Set2Bag({0,1}))||Px(0)||Py(0) ));\n";

  run_linearisation_test_case(spec,true);
}

#ifndef MCRL2_SKIP_LONG_TESTS 

BOOST_AUTO_TEST_CASE(Type_checking_of_function_can_be_problematic)
{
  const std::string spec =
     "sort  State = struct S;\n"
     "proc X = ((lambda x: Nat. S)(3) == S)->tau.X;\n"
     "init X;\n";

  run_linearisation_test_case(spec,true);
}

BOOST_AUTO_TEST_CASE(Check_whether_the_sum_variable_will_not_get_the_same_name_as_the_newly_introduced_process_parameter)
{
  const std::string spec =
     "act  base ;\n"
     "     exponent: Real;\n"
     "proc Test_exponentation =\n"
     "       sum r: Real. base . exponent(r).delta ;\n"
     "\n"
     "init Test_exponentation+delta;\n";

  run_linearisation_test_case(spec,true);
}

BOOST_AUTO_TEST_CASE(Check_whether_the_sum_variable_will_not_get_the_same_name_as_the_newly_introduced_process_parameter2)
{
  const std::string spec =
     "act\n"
     "  a,c,b,d;\n"
     "\n"
     "proc\n"
     "  P = b;\n"
     "  Q = (((tau) . (sum b1: Bool . (sum b2: Bool . (R)))) . (tau)) + (((delta) . (tau)) . (R));\n"
     "  R = ((true) -> (a)) + ((true) -> (sum b1: Bool . ((d) + ((d) + (a)))) <> ((d) + (a)));\n"
     "\n"
     "init\n"
     "  hide({b}, ((R) || (Q)) || (P));\n";

  run_linearisation_test_case(spec,true);
} 

BOOST_AUTO_TEST_CASE(linearisation_of_the_enclosed_spec_caused_a_name_conflict_with_the_option_lstack)
{
  const std::string spec =
     "act\n"
     "  c;\n"
     "\n"
     "proc\n"
     "  Q = sum b1: Bool . R;\n"
     "  R = sum b1: Bool . c.delta;\n"
     "\n"
     "init Q;\n";

  run_linearisation_test_case(spec,true);
} 

// In the test below, the value 1 for the parameter step should be properly substituted,
// also in the distribution.
BOOST_AUTO_TEST_CASE(linearisation_of_distribution_over_initial_process)
{
  const std::string spec =
     "act RequestReading_;\n"
     "    SWFault;\n"
     "\n"
     "map sw_fault_prob: Pos -> Real;\n"
     "var x: Pos;\n"
     "eqn sw_fault_prob(x) = if(x == 2, 1/20, if(x == 1, 1/10, Int2Real(0)));\n"
     "\n"
     "proc Software(step: Pos) = (\n"
     "    (dist f:Bool[if(f, sw_fault_prob(step), 1-sw_fault_prob(step))] . (f -> SWFault . delta <> (\n"
     "        (step == 1) -> (\n"
     "            % First step: get sensor reading\n"
     "            RequestReading_ . delta\n"
     "        )))));\n"
     "\n"
     "init Software(1);\n";

  run_linearisation_test_case(spec,true);
}   



#else // ndef MCRL2_SKIP_LONG_TESTS

BOOST_AUTO_TEST_CASE(skip_linearization_test)
{
}

#endif // ndef MCRL2_SKIP_LONG_TESTS

