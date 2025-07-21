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

#include "utility.h"

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


