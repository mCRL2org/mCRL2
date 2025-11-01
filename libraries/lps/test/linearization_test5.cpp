// Author(s): Wieger Wesselink, Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file linearization_test1.cpp
/// \brief Add your file description here.

#define BOOST_TEST_MODULE linearization_test5
#include <boost/test/included/unit_test.hpp>

#ifndef MCRL2_SKIP_LONG_TESTS

#include "mcrl2/data/detail/rewrite_strategies.h"
#include "mcrl2/lps/is_well_typed.h"
#include "mcrl2/lps/linearise.h"

using namespace mcrl2;
using namespace mcrl2::lps;

#include "utility.h"


BOOST_AUTO_TEST_CASE(various_case_27)
{
  const std::string various_case_27=
    "act a:Pos;\n"
    "proc P(id,n:Pos)=(id<n) -> a(n).P(id,n);\n"
    "     Q(n:Pos)=P(1,n)||P(2,n)||P(3,n);\n"
    "init Q(15);\n";
  run_linearisation_test_case(various_case_27);
}

BOOST_AUTO_TEST_CASE(various_case_28)
{
  const std::string various_case_28=
    "sort A=List(Nat->Nat);"
    "T=struct f(Nat->Nat);"
    "act b:A;"
    "proc P(a:A)=b(a).P([]);"
    "init P([lambda n:Nat.n]);";
  run_linearisation_test_case(various_case_28);
}

BOOST_AUTO_TEST_CASE(various_case_29)
{
  const std::string various_case_29=
    "sort Data = struct x;"
    "Coloured = struct flow(data : Data) | noflowG | noflowR;"
    "act A, B : Coloured;"
    "proc Sync = ( (A(noflowG) | B(noflowR))  + "
    "     (A(noflowR) | B(noflowR)) + "
    "     (sum d : Data.(A(flow(d)) | B(flow(d)))) "
    "  ).Sync;"
    "init Sync;";
  run_linearisation_test_case(various_case_29);
}

// The test case below is to test whether the elements of a multi-action
// are dealt with properly when they occur in a subexpression. The linearised
// process below should have three and not two summand.
BOOST_AUTO_TEST_CASE(various_case_30)
{
  const std::string various_case_30=
    "act a;"
    "    b,b':Nat;"
    "init a.(b(0)|b'(0))+a.(b(0)|b(0));";
  run_linearisation_test_case(various_case_30);
}

BOOST_AUTO_TEST_CASE(various_case_31)
{
  const std::string various_case_31=
    "act a:List(List(Nat));"
    "proc X(x:List(List(Nat)))=a(x).delta;"
    "init X([[]]);";
  run_linearisation_test_case(various_case_31);
}

// Original name: LR2plus.mcrl2
// This example can only be parsed unambiguously by an LR(k) parser generator
// for the current grammar, where k > 1. Namely, process expression 'a + tau'
// cannot be parsed unambiguously. After parsing the identifier 'a', it has to
// be determined if 'a' is an action or process reference, or if 'a' is a data
// expression, viz. part of the left hand side of a conditional process
// expression. With a lookahead of 1, we may only use the '+' as extra
// information, which is not enough, because this symbol is also ambiguous.

BOOST_AUTO_TEST_CASE(various_case_LR2plus)
{
  const std::string various_case_LR2plus=
    "act\n"
    " a;\n\n"
    "init\n"
    " a + tau;";
  run_linearisation_test_case(various_case_LR2plus);
}

// Original name: LR2par.mcrl2
// This example can only be parsed unambiguously by an LR(k) parser generator
// for the current grammar, where k > 1. Namely, process expression '(a)'
// cannot be parsed unambiguously. After parsing the left parenthesis '(', it
// has to be determined if it is part of a process or data expression, viz.
// part of the left hand side of a conditional process expression. With a
// lookahead of 1, we may only use the identifier 'a' as extra information,
// which is not enough, because this symbol is also ambiguous.

BOOST_AUTO_TEST_CASE(various_case_LR2par)
{
  const std::string various_case_LR2par=
    "act\n"
    " a;\n\n"
    "init\n"
    " (a);";
  run_linearisation_test_case(various_case_LR2par);
}


// This test case is a simple test to test sort normalisation in the lineariser,
// added because assertion failures in the domineering example were observed 
BOOST_AUTO_TEST_CASE(various_case_32)
{
  const std::string various_case_32 =
    "sort Position = struct Full | Empty;\n"
    "     Row = List(Position);\n"
    "\n"
    "proc P(r:Row) = delta;\n"
    "init P([Empty]);\n"
    ;
  run_linearisation_test_case(various_case_32);
}

// This test case is a test to check whether constant elimination in the
// linearizer goes well. This testcase is inspired by an example by Chilo
// van Best. The problem is that the constant x:Nat below may not be
// recorded in the assignment list of process P, and therefore forgotten 
BOOST_AUTO_TEST_CASE(various_case_33)
{
  const std::string various_case_33 =
    "act  a:Nat; "
    "proc P(x:Nat,b:Bool,r:Real) = a(x).P(x,!b,r); "
    "     P(x:Nat) = P(x,true,1); "
    "init P(1); "
    ;
  run_linearisation_test_case(various_case_33);
}

// The test case below checks whether the alphabet conversion does not accidentally
// reverse the order of hide and sum operators. If this happens the linearizer will
// not be able to linearize this process 
BOOST_AUTO_TEST_CASE(various_case_34)
{
  const std::string various_case_34 =
    "act a; "

    "proc X = a.X;"
    "proc Y = sum n:Nat. X;"

    "init hide({a}, sum n:Nat. X);"
    ;
  run_linearisation_test_case(various_case_34);
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

