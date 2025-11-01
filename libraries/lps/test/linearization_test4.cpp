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

#define BOOST_TEST_MODULE linearization_test4
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/data/detail/rewrite_strategies.h"
#include "mcrl2/lps/is_well_typed.h"
#include "mcrl2/lps/linearise.h"

using namespace mcrl2;
using namespace mcrl2::lps;

#include "utility.h"

// Here various testcases are checked, which have been used in
// debugging the translation of the linearizer to the new data
// library. 
BOOST_AUTO_TEST_CASE(various_case_1)
{
  run_linearisation_test_case(
    "init delta;"
  );
}

BOOST_AUTO_TEST_CASE(various_case_2)
{
  const std::string various_case_2=
    "act a;"
    "proc X=a.X;"
    "init X;";
  run_linearisation_test_case(various_case_2);
}

BOOST_AUTO_TEST_CASE(various_case_3)
{
  const std::string various_case_3=
    "sort D     = struct d1 | d2;"
    "             Error = struct e;"
    "act r2: D # Bool;"
    "    s3: D # Bool;"
    "    s3: Error;"
    "    i;"
    "proc K = sum d:D,b:Bool. r2(d,b).(i.s3(d,b)+i.s3(e)).K;"
    "init K;";
  run_linearisation_test_case(various_case_3);
}

BOOST_AUTO_TEST_CASE(various_case_4)
{
  const std::string various_case_4=
    "act a:Nat;"
    "proc X=sum n:Nat. (n==0)->a(n).X;"
    "init X;";
  run_linearisation_test_case(various_case_4);
}

BOOST_AUTO_TEST_CASE(various_case_5)
{
  const std::string various_case_5=
    "act a,b,c;"
    "proc X=a.X;"
    "     Y=b.Y;"
    "init X||Y;";
  run_linearisation_test_case(various_case_5);
}

BOOST_AUTO_TEST_CASE(various_case_6)
{
  const std::string various_case_6=
    "act a1,a2,b,c;"
    "proc X=a1.a2.X;"
    "     Y=b.Y;"
    "init comm({a1|b->c},X||Y);";
  run_linearisation_test_case(various_case_6);
}

BOOST_AUTO_TEST_CASE(various_case_7)
{
  const std::string various_case_7=
    "proc X=tau.X;"
    "init X;";
  run_linearisation_test_case(various_case_7);
}

BOOST_AUTO_TEST_CASE(various_case_8)
{
  const std::string various_case_8=
    "act a,b;"
    "proc X= (a|b).X;"
    "init X;";
  run_linearisation_test_case(various_case_8);
}

BOOST_AUTO_TEST_CASE(various_case_9)
{
  const std::string various_case_9=
    "act a;"
    "init allow({a},a.a.delta);";
  run_linearisation_test_case(various_case_9);
}

BOOST_AUTO_TEST_CASE(various_case_10)
{
  const std::string various_case_10=
    "act a,b,c;"
    "init comm({a|b->c},(a|b).delta);";
  run_linearisation_test_case(various_case_10);
}

BOOST_AUTO_TEST_CASE(various_case_11)
{
  const std::string various_case_11=
    "act a,b,c:Nat;"
    "map n:Nat;"
    "init comm({a|b->c},(a(3)|b(n)));";
  run_linearisation_test_case(various_case_11);
}

BOOST_AUTO_TEST_CASE(various_case_12)
{
  const std::string various_case_12=
    "act c2:Nat#Nat;"
    "init allow({c2},c2(3,5));";
  run_linearisation_test_case(various_case_12);
}

BOOST_AUTO_TEST_CASE(various_case_13)
{
  const std::string various_case_13=
    "sort D = struct d1 | d2;"
    "act r1,s4: D;"
    "proc S(b:Bool)     = sum d:D. r1(d).S(true);"
    "init S(false);";
  run_linearisation_test_case(various_case_13);
}

BOOST_AUTO_TEST_CASE(various_case_14)
{
  const std::string various_case_14=
    "act r1: Bool;"
    "proc S(d:Bool) = sum d:Bool. r1(d).S(true);"
    "init S(false);";
  run_linearisation_test_case(various_case_14);
}

BOOST_AUTO_TEST_CASE(various_case_15)
{
  const std::string various_case_15=
    "act a;"
    "init (a+a.a+a.a.a+a.a.a.a).delta;";
  run_linearisation_test_case(various_case_15);
}

BOOST_AUTO_TEST_CASE(various_case_16)
{
  const std::string various_case_16=
    "act s6,r6,c6, i;"
    "proc T = r6.T;"
    "     K = i.K;"
    "     L = s6.L;"
    "init comm({r6|s6->c6},T || K || L);";
  run_linearisation_test_case(various_case_16);
}

BOOST_AUTO_TEST_CASE(various_case_17)
{
  const std::string various_case_17=
    "act s3,r3,c3,s6;"
    "proc R = r3.R;"
    "     K = s3.K;"
    "     L = s6.L;"
    "init comm({r3|s3->c3}, K || L || R);";
  run_linearisation_test_case(various_case_17);
}

BOOST_AUTO_TEST_CASE(various_case_18)
{
  const std::string various_case_18=
    "act a,b,c,d,e;"
    "init comm({c|d->b},(a|b|c|d|e).delta);";
  run_linearisation_test_case(various_case_18);
}

BOOST_AUTO_TEST_CASE(various_case_19)
{
  const std::string various_case_19=
    "act a,b,c,d,e;"
    "init comm({e|d->b},(a|b|c|d|e).delta);";
  run_linearisation_test_case(various_case_19);
}

BOOST_AUTO_TEST_CASE(various_case_20)
{
  const std::string various_case_20=
    "act a:Nat;"
    "proc X(n:Nat)="
    "  sum n:Nat.(n>25) -> a(n).X(n)+"
    "  sum n:Nat.(n>25) -> a(n).X(n)+"
    "  (n>25) -> a(n).X(n);"
    "init X(1);";
  run_linearisation_test_case(various_case_20);
} 

BOOST_AUTO_TEST_CASE(various_case_21)
{
  const std::string various_case_21=
    "act a,b:Pos;"
    "proc X(m:Pos)= sum n:Nat. (n<1) -> a(1)|b(1)@1.X(1)+"
    "               sum n:Nat. (n<2) -> a(2)|b(2)@2.X(2)+"
    "               sum n:Nat. (n<3) -> a(3)|b(3)@3.X(3)+"
    "               sum n:Nat. (n<4) -> a(4)|b(4)@4.X(4)+"
    "               sum n:Nat. (n<5) -> a(5)|b(5)@5.X(5);"
    "init X(1);";
  run_linearisation_test_case(various_case_21);
}

BOOST_AUTO_TEST_CASE(various_case_22)
{
  const std::string various_case_22=
    "% This test is expected to fail with a proper error message.\n"
    "act a;\n"
    "proc P = (a || a) . P;\n"
    "init P;\n";
  run_linearisation_test_case(various_case_22, false);
}

BOOST_AUTO_TEST_CASE(various_case_23)
{
  const std::string various_case_23=
    "act a,b;"
    "init a@1.b@2.delta||tau.tau;";
  run_linearisation_test_case(various_case_23);
}

BOOST_AUTO_TEST_CASE(various_case_24)
{
  const std::string various_case_24=
    "act  a: Pos;"
    "glob x: Pos;"
    "proc P = a(x).P;"
    "init P;";
  run_linearisation_test_case(various_case_24);
}

// The testcase below is designed to test the constant elimination in the lineariser.
// Typically, x1 and x2 can be eliminated as they are always constant. Care must be
// taken however that the variable y does not become unbound in the process.
BOOST_AUTO_TEST_CASE(various_case_25)
{
  const std::string various_case_25=
    "act a:Pos#Pos#Pos;"
    "    b;"
    "proc Q(y:Pos)=P(y,1,1)||delta;"
    "     P(x1,x2,x3:Pos)=a(x1,x2,x3).P(x1,x2,x3+1);"
    "init Q(2);";
  run_linearisation_test_case(various_case_25);
}

// The following testcase exhibits a problem that occurred in the lineariser in
// August 2009. The variable m would only be partly renamed, and show up as an
// undeclared variable in the resulting LPS. The LPS turned out to be not well
// typed.
BOOST_AUTO_TEST_CASE(various_case_26)
{
  const std::string various_case_26=
    "act  r,s1,s2:Nat;\n"
    "proc P=sum m:Nat.r(m).((m==1)->s1(m).P+(m==2)->P+P);\n"
    "init P;\n";
  run_linearisation_test_case(various_case_26);
}

// The following testcase tests whether two parameters with the same name
// but different type is handled properly. Added July 20, 2025. 
BOOST_AUTO_TEST_CASE(various_case_27)
{
  const std::string various_case_27=
    "act a:Bool;\n"
    "proc X(n:Nat)=a(n<10).Y(true);\n"
    "     Y(n:Bool)=a(n).X(7);\n"
    "init X(1);\n";
  run_linearisation_test_case(various_case_27);
}

// The following testcase tests whether variables with different types
// but different names are handled properly. Added October 24, 2025. 
BOOST_AUTO_TEST_CASE(variable_clash)
{
  const std::string variable_clash=
    "act a1,b1,c1,a2,b2,c2:Bool;\n"
    "    a1,b1,c1,a2,b2,c2:Nat;\n"
    "\n"
    "proc P1 = sum x:Bool.a1(x).b1(x).c1(x).P1+\n"
    "          sum x:Nat. a1(x).b1(x).c1(x).P1;\n"
    "     P2 = sum x:Bool.a2(x).b2(x).c2(x).P2+\n"
    "          sum x:Nat. a2(x).b2(x).c2(x).P2;\n"
    "\n"
    "init allow({a1,b1,c1,a2,b2,c2},P1||P2);\n";
  run_linearisation_test_case(variable_clash);
}

// The following testcase tests whether variables with different types
// but different names are handled properly. Added October 24, 2025. 
BOOST_AUTO_TEST_CASE(variable_clash_with_sharing_proces_body)
{
  const std::string variable_clash=
    "act a1,b1,c1,a2,b2,c2:Bool;\n"
    "    a1,b1,c1,a2,b2,c2:Nat;\n"
    "\n"
    "proc P1 = sum x:Bool.a1(x).b1(x).c1(x).P1+\n"
    "          sum x:Nat. a1(x).b1(x).c1(x).P2;\n"
    "     P2 = sum x:Bool.a2(x).b2(x).c2(x).P2+\n" 
    "          sum x:Nat. a2(x).b2(x).c2(x).P1;\n"
    "\n"
    "init allow({a1,b1,c1,a2,b2,c2},P1||P2);\n";
  run_linearisation_test_case(variable_clash);
}


