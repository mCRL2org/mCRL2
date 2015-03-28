// Author(s): Wieger Wesselink, Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file linearization_test1.cpp
/// \brief Add your file description here.

#include <boost/test/included/unit_test_framework.hpp>

#ifndef MCRL2_SKIP_LONG_TESTS

#include <iostream>
#include <string>

#include "mcrl2/data/detail/rewrite_strategies.h"
#include "mcrl2/lps/linearise.h"
#include "mcrl2/utilities/logger.h"

using namespace mcrl2;
using namespace mcrl2::lps;

typedef data::rewriter::strategy rewrite_strategy;
typedef std::vector<rewrite_strategy> rewrite_strategy_vector;

void run_linearisation_instance(const std::string& spec, const t_lin_options& options, bool expect_success)
{
  if (expect_success)
  {
    lps::specification s=remove_stochastic_operators(linearise(spec, options));
    BOOST_CHECK(s != lps::specification());
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

BOOST_AUTO_TEST_CASE(test_multiple_linearization_calls)
{
  // Parameter i should be removed
  const std::string case_1(
    "act a;\n\n"
    "proc X(i: Nat) = a.X(i);\n\n"
    "init X(2);\n");

  // Parameter j should be removed
  const std::string case_2(
    "act a: Nat;\n\n"
    "proc X(i,j: Nat) = a(i). X(i,j);\n\n"
    "init X(0,1);\n");

  // Parameter j should be removed
  const std::string case_3(
    "act a;\n\n"
    "proc X(i,j: Nat)   = (i == 5) -> a. X(i,j);\n\n"
    "init X(0,1);\n");

  // Parameter j should be removed
  const std::string case_4(
    "act a;\n\n"
    "proc X(i,j: Nat) = a@i.X(i,j);\n\n"
    "init X(0,4);\n");

  // Nothing should be removed
  const std::string case_5(
    "act a: Nat;\n"
    "act b;\n\n"
    "proc X(i,j,k: Nat) =  a(i).X(k,j,k) +\n"
    "                         b.X(j,j,k);\n\n"
    "init X(1,2,3);");

  // Nothing should be removed
  const std::string case_6(
    "act act1, act2, act3: Nat;\n\n"
    "proc X(i: Nat)   = (i <  5) -> act1(i).X(i+1) +\n"
    "                   (i == 5) -> act3(i).Y(i, i);\n"
    "     Y(i,j: Nat) = act2(j).Y(i,j+1);\n\n"
    "init X(0);\n");

  const std::string case_7(
    "act act1, act2, act3: Nat;\n\n"
    "proc X(i,z,j: Nat)   = (i <  5) -> act1(i)@z.X(i+1,z, j) +\n"
    "                       (i == 5) -> act3(i).X(i, j, 4);\n\n"
    "init X(0,5, 1);\n"
  );

  const std::string case_8(
    "act a;\n"
    "init sum t:Nat. a@t;\n"
  );

  // Check that rewriting of non explicitly declared lists
  // works properly.
  const std::string case_9
  (
    "act c;\n"
    "init sum t:List(struct a | b) . c;\n"
  );

  stochastic_specification spec;
  spec = linearise(case_1);
  spec = linearise(case_2);
  spec = linearise(case_3);
  spec = linearise(case_4);
  spec = linearise(case_5);
  spec = linearise(case_6);
  spec = linearise(case_7);
  spec = linearise(case_8);
  spec = linearise(case_9);
}

BOOST_AUTO_TEST_CASE(test_process_assignments)
{
  const std::string assignment_case_1
  ("act a,b,c;"
   "proc X(v:Nat)=a.X(v=3)+Y(1,2);"
   "Y(v1:Nat, v2:Nat)=a.Y(v1=3)+b.X(5)+c.Y(v2=7);"
   "init X(3);"
  );

  const std::string assignment_case_2
  ("act a;"
   "proc X(v:Nat)=a.Y(w=true);"
   "Y(w:Bool)=a.X(v=0);"
   "init X(v=3);"
  );

  const std::string assignment_case_3
  ("act a;"
   "    b:Nat;"
   "proc X(v:Nat,w:List(Bool))=a.X(w=[])+"
   "                         (v>0) ->b(v).X(v=max(v,0));"
   "init X(v=3,w=[]);"

  );

  const std::string assignment_case_4
  ("act a;"
   "proc X(v:Pos,w:Nat)=sum w:Pos.a.X(v=w)+"
   "                    sum u:Pos.a.X(v=u);"
   "init X(3,4);"

  );

  const std::string assignment_case_5
  ("act a;"
   "proc X(v:Pos)=sum v:Pos.a@4.X();"
   "init X(3);"
  );

  run_linearisation_test_case(assignment_case_1);
  run_linearisation_test_case(assignment_case_2);
  run_linearisation_test_case(assignment_case_3);
  run_linearisation_test_case(assignment_case_4);
  run_linearisation_test_case(assignment_case_5);
}

BOOST_AUTO_TEST_CASE(test_struct)
{
  std::string text =
    "sort D = struct d1(Nat)?is_d1 | d2(arg2:Nat)?is_d2;\n"
    "                                                   \n"
    "init true->delta;                                  \n"
    ;
  run_linearisation_test_case(text);
}

BOOST_AUTO_TEST_CASE(test_lambda)
{
  run_linearisation_test_case(
    "map select : (Nat -> Bool) # List(Nat) -> List(Nat);\n"
    "var f : Nat -> Bool;\n"
    "    x : Nat;\n"
    "    xs : List(Nat);\n"
    "eqn select(f,[]) = [];\n"
    "    select(f,x|>xs) = if(f(x), x|>sxs, sxs) whr sxs = select(f, xs) end;\n"
    "act a : Nat;\n"
    "init sum n : Nat.\n"
    "  (n in select(lambda x : Nat.x mod 2 == 0, [1, 2, 3])) -> a(n).delta;\n");
}

BOOST_AUTO_TEST_CASE(test_no_free_variables)
{
  const std::string no_free_variables_case_1(
    "act a,b:Int;\n"
    "proc P = sum y:Int . (y == 4) -> a(y)@y . b(y*2)@(y+1) . P;\n"
    "init P;\n"
  );

  t_lin_options options;
  options.noglobalvars = true;

  stochastic_specification spec;
  spec = linearise(no_free_variables_case_1, options);
  BOOST_CHECK(spec.global_variables().empty());
}

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

#else // ndef MCRL2_SKIP_LONG_TESTS

BOOST_AUTO_TEST_CASE(skip_linearization_test)
{
}

#endif // ndef MCRL2_SKIP_LONG_TESTS

boost::unit_test::test_suite* init_unit_test_suite(int, char*[])
{
  return 0;
}

