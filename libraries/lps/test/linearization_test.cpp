// Author(s): Wieger Wesselink, Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file linearization_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <string>

#include <boost/test/included/unit_test_framework.hpp>

#include "mcrl2/lps/linearise.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/test_utilities.h"

using namespace mcrl2;
using namespace mcrl2::lps;

typedef data::basic_rewriter<data::data_expression>::strategy rewrite_strategy;
typedef std::vector<rewrite_strategy> rewrite_strategy_vector;

void run_linearisation_instance(const std::string& spec, const t_lin_options& options, bool expect_success)
{
  if (expect_success)
  {
    lps::specification s = linearise(spec, options);
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
  rewrite_strategy_vector rewrite_strategies = utilities::get_test_rewrite_strategies(false);

  for (rewrite_strategy_vector::const_iterator i = rewrite_strategies.begin(); i != rewrite_strategies.end(); ++i)
  {
    std::clog << std::endl << "Testing with rewrite strategy " << data::pp(*i) << std::endl;

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

  specification spec;
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

  specification spec;
  spec = linearise(no_free_variables_case_1, options);
  BOOST_CHECK(spec.global_variables().empty());
}

/* Here various testcases are checked, which have been used in
     debugging the translation of the linearizer to the new data
     library. */
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

/* Original name: LR2plus.mcrl2
 This example can only be parsed unambiguously by an LR(k) parser generator
 for the current grammar, where k > 1. Namely, process expression 'a + tau'
 cannot be parsed unambiguously. After parsing the identifier 'a', it has to
 be determined if 'a' is an action or process reference, or if 'a' is a data
 expression, viz. part of the left hand side of a conditional process
 expression. With a lookahead of 1, we may only use the '+' as extra
 information, which is not enough, because this symbol is also ambiguous.
*/
BOOST_AUTO_TEST_CASE(various_case_LR2plus)
{
  const std::string various_case_LR2plus=
    "act\n"
    " a;\n\n"
    "init\n"
    " a + tau;";
  run_linearisation_test_case(various_case_LR2plus);
}

/* Original name: LR2par.mcrl2
 This example can only be parsed unambiguously by an LR(k) parser generator
 for the current grammar, where k > 1. Namely, process expression '(a)'
 cannot be parsed unambiguously. After parsing the left parenthesis '(', it
 has to be determined if it is part of a process or data expression, viz.
 part of the left hand side of a conditional process expression. With a
 lookahead of 1, we may only use the identifier 'a' as extra information,
 which is not enough, because this symbol is also ambiguous.
*/
BOOST_AUTO_TEST_CASE(various_case_LR2par)
{
  const std::string various_case_LR2par=
    "act\n"
    " a;\n\n"
    "init\n"
    " (a);";
  run_linearisation_test_case(various_case_LR2par);
}


/* This test case is a simple test to test sort normalisation in the lineariser,
   added because assertion failures in the domineering example were observed */
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

/* This test case is a test to check whether constant elimination in the
   linearizer goes well. This testcase is inspired by an example by Chilo
   van Best. The problem is that the constant x:Nat below may not be
   recorded in the assignment list of process P, and therefore forgotten */
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

/* The test case below checks whether the alphabet conversion does not accidentally
 * reverse the order of hide and sum operators. If this happens the linearizer will
 * not be able to linearize this process */
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

BOOST_AUTO_TEST_CASE(various_case_par)
{
  const std::string various_par =
    "act a;\n"
    "init a || a;\n"
    ;
  run_linearisation_test_case(various_par);
}

BOOST_AUTO_TEST_CASE(gpa_10_3)
{
  const std::string various_gpa_10_3 =
    "act\n"
    "  c,r_dup, s_dup1, s_dup2, r_inc, s_inc, r_mul1, r_mul2, s_mul: Int;\n"
    "proc\n"
    "  Dup = sum x:Int. r_dup(x) | s_dup1(x) | s_dup2(x) . Dup;\n"
    "  Inc = sum x:Int. r_inc(x) | s_inc(x+1) . Inc;\n"
    "  Mul = sum x,y:Int. r_mul1(x) | r_mul2(y) | s_mul(x*y) . Mul;\n"
    "  Dim = allow({r_dup | s_mul},\n"
    "          hide({c},comm({s_dup1 | r_mul1 -> c , s_dup2 | r_inc -> c, s_inc | r_mul2 ->c},\n"
    "            Dup || Inc || Mul\n"
    "          ))\n"
    "        );\n"
    "init Dim;\n"
    ;
  run_linearisation_test_case(various_gpa_10_3);
}

/* The following test cases fail because the n-parallel support in the alphabet reductions is broken
 * (Checked JK 31/8/2010)
 */
/*
BOOST_AUTO_TEST_CASE(philosophers)
{
  const std::string various_philosophers =
    "map K: Pos;\n"
    "eqn K = 10;\n"
    "act get,_get,__get,put,_put,__put: Pos#Pos;\n"
    "    eat: Pos;\n"
    "proc\n"
    "  Phil(n:Pos) = _get(n,n)._get(n,if(n==K,1,n+1)).eat(n)._put(n,n)._put(n,if(n==K,1,n+1)).Phil(n);\n"
    "  Fork(n:Pos) = sum m:Pos.get(m,n).put(m,n).Fork(n);\n"
    "  ForkPhil(n:Pos) = Fork(n) || Phil(n);\n"
    "  KForkPhil(p:Pos) =\n"
    "    (p>1) -> (ForkPhil(p)||KForkPhil(max(p-1,1)))<>ForkPhil(1);\n"
    "init allow( { __get, __put, eat },\n"
    "       comm( { get|_get->__get, put|_put->__put },\n"
    "         KForkPhil(K)\n"
    "     ));\n"
    ;
  run_linearisation_test_case(various_philosophers);
}

BOOST_AUTO_TEST_CASE(philosophers_nat)
{
  const std::string various_philosophers_nat =
    "map K: Nat;\n"
    "eqn K = 10;\n"
    "act get,_get,__get,put,_put,__put: Nat#Nat;\n"
    "    eat: Nat;\n"
    "proc\n"
    "  Phil(n:Nat) = _get(n,n)._get(n,if(n==K,1,n+1)).eat(n)._put(n,n)._put(n,if(n==K,1,n+1)).Phil(n);\n"
    "  Fork(n:Nat) = sum m:Nat.get(m,n).put(m,n).Fork(n);\n"
    "  ForkPhil(n:Nat) = Fork(n) || Phil(n);\n"
    "  KForkPhil(p:Nat) =\n"
    "    (p>1) -> (ForkPhil(p)||KForkPhil(max(p-1,1)))<>ForkPhil(1);\n"
    "init allow( { __get, __put, eat },\n"
    "       comm( { get|_get->__get, put|_put->__put },\n"
    "         KForkPhil(K)\n"
    "     ));\n"
    ;
  run_linearisation_test_case(various_philosophers_nat);
}
*/

BOOST_AUTO_TEST_CASE(sort_aliases)
{
  const std::string various_sort_aliases =
    "sort Bits = struct b0|b1;\n"
    "     t_sys_regset_fsm_state = Bits;\n"
    "     t_timer_counter_fsm_state = Bits;\n"
    "map  timer_counter_fsm_state_idle: t_timer_counter_fsm_state;\n"
    "act  a:Bits;\n"
    "proc P(d:Bits)=a(d).delta;\n"
    "glob globd:Bits;\n"
    "init P(globd);\n"
    ;
  run_linearisation_test_case(various_sort_aliases);
}

BOOST_AUTO_TEST_CASE(test_aliases_complex)
{
  const std::string spec =
    "sort\n"
    "  Bits = struct singleBit (bit: Bool)?isSingleBit | bitVector (bitVec:List(Bool))?isBitVector;\n"
    "  t_sys_regset_fsm_state = Bits;\n"
    "  t_timer_counter_fsm_state = Bits;\n"
    "map\n"
    "  repeat : Bool # Nat -> Bits;\n"
    "  repeat_rec : Bool # Nat -> List(Bool);\n"
    "var\n"
    "  b:Bool;\n"
    "  n:Nat;\n"
    "eqn\n"
    "  repeat(b,n) = if(n <= 1, singleBit(b), bitVector(repeat_rec(b,n)));\n"
    "act a:Bits;\n"
    "init a(repeat(true,32)).delta;\n";
  run_linearisation_test_case(spec);
}

BOOST_AUTO_TEST_CASE(test_bug_775a)
{
  const std::string spec =
    "sort V = struct v( l: Bool  );\n"
    "act  a:  List( Bool );\n"
    "sort B = Bool -> List(Bool);\n"
    "proc X( M: B ) = \n"
    "  sum v:V.  a( [] ) \n"
    "+ sum v:V.  a( M(l(v)))\n"
    ";\n"
    "init X( lambda i: Bool. [] );\n";
  run_linearisation_test_case(spec);
}

BOOST_AUTO_TEST_CASE(test_bug_775b)
{
  const std::string spec =
    "sort V = struct v( l: Bool  );\n"
    "act  a:  List( Bool );\n"
    "sort B = Bool -> List(Bool);\n"
    "proc X( M: B ) = \n"
    "  sum v:V.  a( M(l(v)))\n"
    "+ sum v:V.  a( [] ) \n"
    ";\n"
    "init X( lambda i: Bool. [] );\n";
  run_linearisation_test_case(spec);
}

BOOST_AUTO_TEST_CASE(test_bug_alphabet_reduction)
{
  const std::string spec =
    "sort Variables = struct s;\n"
    "\n"
    "act send_c, recv_c, comm_c: Bool;\n"
    "    chng: Set(Variables);\n"
    "\n"
    "init allow( { comm_c | chng | chng } ,\n"
    "  comm( { send_c | recv_c -> comm_c,\n"
    "          chng | chng -> chng},\n"
    "          send_c(false) | chng( {} ) || recv_c(false) | chng( {s} )\n"
    "       ) );\n";
  run_linearisation_test_case(spec);
}

BOOST_AUTO_TEST_CASE(test_bug_alphabet_reduction1)
{
  const std::string spec =
    "act a: Bool;\n"
    "    b: Nat;\n"
    "\n"
    "proc X = allow( {b | a | a },\n"
    "          a(true)| b(5) | a(false) );\n"
    "\n"
    "init X;\n";
  run_linearisation_test_case(spec);
}

// The testcase below is added because the typechecker could not deal with this
// case. The three options for the action a caused it to become confused.
BOOST_AUTO_TEST_CASE(test_multiple_action_types)
{
  const std::string spec =
     "sort Id = Pos;\n"
     "\n"
     "map FALSE: Id -> Bool;\n"
     "var n: Id;\n"
     "eqn FALSE(n) = false;\n"
     "\n"
     "act a: (Id -> Bool);\n"
     "    a: Bool;\n"
     "    a: Pos;\n"
     "\n"
     "proc P = a(FALSE) . delta;\n"
     "init P;\n";

  run_linearisation_test_case(spec);
}

// Test case for bug #1085
BOOST_AUTO_TEST_CASE(bug_1085)
{
  const std::string spec =
    "proc P = tau. a | b | c;\n"
    "act a,b,c;\n"
    "init P;\n";

  run_linearisation_test_case(spec);
}

BOOST_AUTO_TEST_CASE(bug_978)
{
  const std::string spec =
     "act rd,wr:Bool;\n"
     "proc R(a,b : Bool) = \n"
     "        rd(a) . R() +\n"
     "        rd(b) . R() + \n"
     "        sum b:Bool. wr(b) . R(a=b);\n"
     "init R(true,true);\n"; 
  run_linearisation_test_case(spec);
}

BOOST_AUTO_TEST_CASE(not_properly_ordered_assignments)
{
  const std::string spec =
     "proc R(a,b : Bool) = \n"
     "        tau . R(a=true,b=false) +\n"
     "        tau . R(b=false,a=false); \n"
     "init R(true,true);\n"; 
  run_linearisation_test_case(spec);
}

BOOST_AUTO_TEST_CASE(the_bound_c_may_not_show_up_in_linear_process)
{
  const std::string spec =
     "act \n"
     "rcv_req; \n"
     "snd_return : Bool; \n"
     "internal; \n"
     " \n"
     "proc \n"
     " \n"
     "test1 = \n"
     "( \n"
     "    rcv_req. \n"
     "    sum c:Bool. \n"
     "    ( \n"
     "        internal.snd_return(c) \n"
     "    ). \n"
     "    test1 \n"
     "); \n"
     " \n"
     "init test1; \n";
  run_linearisation_test_case(spec);
}

BOOST_AUTO_TEST_CASE(process_parameters_with_different_types_can_cause_problems)
{
  const std::string spec =
     "proc P(x:Nat)=tau.Q(true); \n"
     "     Q(x:Bool)=tau.P(1); \n"
     "init P(1);\n";

  run_linearisation_test_case(spec);
}

BOOST_AUTO_TEST_CASE(unguarded_recursion)
{
  const std::string spec =
     "proc P=P; \n"
     "init P;\n";

  run_linearisation_test_case(spec,false);
}

BOOST_AUTO_TEST_CASE(unguarded_recursion_with_parallel_operator)
{
  const std::string spec =
     "proc P=P||P; \n"
     "init P;\n";

  run_linearisation_test_case(spec,false);
}

BOOST_AUTO_TEST_CASE(The_unreachability_of_tau_is_not_properly_recognized)
{
  const std::string spec =
     "init (true -> delta <> delta) . tau;";

  run_linearisation_test_case(spec,true);
}


boost::unit_test::test_suite* init_unit_test_suite(int , char* )
{
  return 0;
}

