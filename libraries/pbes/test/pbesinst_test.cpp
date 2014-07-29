// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbes_test.cpp
/// \brief Add your file description here.

//#define MCRL2_PBES_EXPRESSION_BUILDER_DEBUG
//#define MCRL2_ENUMERATE_QUANTIFIERS_BUILDER_DEBUG
//#define MCRL2_ENUMERATE_QUANTIFIERS_BUILDER_DEBUG2

#include <string>
#include <iostream>
#include <boost/test/minimal.hpp>
#include "mcrl2/utilities/logger.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/substitutions/mutable_indexed_substitution.h"
#include "mcrl2/lps/linearise.h"
#include "mcrl2/lps/detail/test_input.h"
#include "mcrl2/modal_formula/parse.h"
#include "mcrl2/modal_formula/detail/test_input.h"
#include "mcrl2/pbes/is_bes.h"
#include "mcrl2/pbes/lps2pbes.h"
#include "mcrl2/pbes/pbesinst_symbolic.h"
#include "mcrl2/pbes/txt2pbes.h"
#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/pbes/pbesinst_algorithm.h"
#include "mcrl2/pbes/pbesinst_finite_algorithm.h"
#include "mcrl2/pbes/detail/pbes_parameter_map.h"
#include "mcrl2/pbes/detail/instantiate_global_variables.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;

inline
pbes pbesinst_lazy(const pbes& p)
{
  pbes q = p;
  pbesinst_algorithm algorithm(q.data());
  algorithm.run(q);
  return algorithm.get_result();
}

inline
pbes pbesinst_finite(const pbes& p)
{
  pbes q = p;
  pbesinst_finite_algorithm algorithm(data::jitty);
  algorithm.run(q);
  return q;
}

std::string test1 =
  "pbes                                                                              \n"
  "                                                                                  \n"
  "nu X(b:Bool, n:Nat) = (val(b) => X(!b, n)) && (val(!b) => X(!b, n+1));            \n"
  "                                                                                  \n"
  "init X(true,0);                                                                   \n"
  ;

std::string test2 =
  "pbes                                                                              \n"
  "                                                                                  \n"
  "nu X(b:Bool, n:Nat) = forall c:Bool. X(c,n);                                      \n"
  "                                                                                  \n"
  "init X(true,0);                                                                   \n"
  ;

std::string test3 =
  "pbes                                                                              \n"
  "                                                                                  \n"
  "nu X(b:Bool, n:Nat) = exists c:Bool. X(c,n+1);                                    \n"
  "                                                                                  \n"
  "init X(true,0);                                                                   \n"
  ;

std::string test4 =
  "pbes                                                                              \n"
  "                                                                                  \n"
  "nu X(b:Bool, n:Nat) = val(b && n < 10) => X(!b,n+1);                              \n"
  "                                                                                  \n"
  "init X(true,0);                                                                   \n"
  ;

std::string test5 =
  "sort D = struct d1 | d2;                                                          \n"
  "                                                                                  \n"
  "pbes                                                                              \n"
  "                                                                                  \n"
  "nu X(d:D, n:Nat) = val(d == d1 && n < 10) => X(d2,n+1);                           \n"
  "                                                                                  \n"
  "init X(d1,0);                                                                     \n"
  ;

std::string test6 =
  "pbes                                                                              \n"
  "nu X(b:Bool) = forall c:Bool. X(if (c,!c,c));                                     \n"
  "                                                                                  \n"
  "init X(true);                                                                     \n"
  ;

std::string test7 =
  "sort Enum2 = struct e1_5 | e0_5;                                                  \n"
  "                                                                                  \n"
  "map                                                                               \n"
  "                                                                                  \n"
  "     C5_fun2: Enum2 # Enum2 # Enum2  -> Enum2;                                    \n"
  "     C5_fun1: Enum2 # Nat # Nat  -> Nat;                                          \n"
  "                                                                                  \n"
  "var  y23,y22,y21,x5,y14,y13,y12,y11,y10,x2,e3,e2,e1: Enum2;                       \n"
  "     y20,y19,y18,x4,y9,y8,y7,y6,y5,x1: Nat;                                       \n"
  "     y17,y16,y15,x3,y4,y3,y2,y1,y,x: Bool;                                        \n"
  "eqn                                                                               \n"
  "     C5_fun2(e0_5, y14, y13)  =  y14;                                             \n"
  "     C5_fun2(e1_5, y14, y13)  =  y13;                                             \n"
  "     C5_fun2(e3, x2, x2)  =  x2;                                                  \n"
  "     C5_fun1(e0_5,  y6, y5)  =  y5;                                               \n"
  "     C5_fun1(e1_5,  y6, y5)  =  y6;                                               \n"
  "     C5_fun1(e2,  x1, x1)  =  x1;                                                 \n"
  "                                                                                  \n"
  "pbes nu X(s3_P: Enum2,  n_P: Nat) =                                               \n"
  "                                                                                  \n"
  "(forall e: Enum2.  X(C5_fun2(e, e, e1_5), C5_fun1(e, 0, n_P))                     \n"
  "                                                                                  \n"
  ")                                                                                 \n"
  "                                                                                  \n"
  "                                                                                  \n"
  ";                                                                                 \n"
  "                                                                                  \n"
  "init X(e1_5,  0);                                                                 \n"
  ;

std::string test8 =
  "pbes                                                                              \n"
  "                                                                                  \n"
  "nu X(b:Bool) = val(b) && Y(!b);                                                   \n"
  "                                                                                  \n"
  "mu Y(c:Bool) = forall d:Bool. X(d && c) || Y(d);                                  \n"
  "                                                                                  \n"
  "init X(true);                                                                     \n"
  ;

// This pbes triggered a garbage collection problem, that has been solved.
std::string random1 =
  "pbes                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        \n"
  "nu X0(c:Bool, n:Nat) = (forall n:Nat.((val(n < 3)) && (((val(n < 3)) || (exists m:Nat.((val(m < 3)) || (X3(m + 1, m > 0))))) && ((forall m:Nat.((val(m < 3)) && (!X2(m + 1, 1)))) => ((val(c)) || (val(n < 3))))))) || ((val(false)) || (X0(false, n + 1)));                                                                                                                                                                                                                                                \n"
  "nu X1(b:Bool) = (!(!(forall k:Nat.((val(k < 3)) && ((forall k:Nat.((val(k < 3)) && ((X2(1, k + 1)) && (val(false))))) || ((X4(k > 0, k + 1)) && (X1(k > 1)))))))) && (!(forall m:Nat.((val(m < 3)) && (((val(m < 2)) && (val(m > 0))) && (val(true))))));                                                                                                                                                                                                                                                   \n"
  "mu X2(m:Nat, n:Nat) = (((val(m < 2)) && (X4(m == n, n + 1))) || ((val(false)) || ((val(true)) => (X0(n == m, 0))))) || (forall k:Nat.((val(k < 3)) && (exists m:Nat.((val(m < 3)) || ((val(n < 2)) && (X2(m + 1, m + 1)))))));                                                                                                                                                                                                                                                                              \n"
  "nu X3(n:Nat, c:Bool) = ((forall k:Nat.((val(k < 3)) && (!((forall m:Nat.((val(m < 3)) && (val(n > 0)))) => (val(c)))))) && ((X3(0, n < 3)) && (exists m:Nat.((val(m < 3)) || ((!(exists n:Nat.((val(n < 3)) || (val(m < 2))))) && (X2(n + 1, m + 1))))))) || ((!(!X1(n > 0))) || (val(false)));                                                                                                                                                                                                             \n"
  "nu X4(c:Bool, n:Nat) = (((exists m:Nat.((val(m < 3)) || (val(m > 0)))) && ((!(!X0(n < 3, 0))) || (!((val(n > 0)) => (!X2(0, 0)))))) => (forall k:Nat.((val(k < 3)) && (!(forall n:Nat.((val(n < 3)) && (val(n < 2)))))))) => (!(forall m:Nat.((val(m < 3)) && ((val(c)) && (forall m:Nat.((val(m < 3)) && (!X3(n + 1, false))))))));                                                                                                                                                                        \n"
  "                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            \n"
  "init X0(true, 0);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           \n"
  ;

// This pbes triggered a garbage collection problem, that has been solved.
std::string random2 =
  "pbes                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        \n"
  "mu X0(m:Nat, b:Bool) = (X0(m + 1, m > 0)) && (((forall m:Nat.((val(m < 3)) && (forall k:Nat.((val(k < 3)) && (!((val(k > 1)) && (val(false)))))))) && (exists n:Nat.((val(n < 3)) || (!(!(val(n > 1))))))) || (exists n:Nat.((val(n < 3)) || (forall n:Nat.((val(n < 3)) && (exists n:Nat.((val(n < 3)) || (((forall m:Nat.((val(m < 3)) && (val(m == n)))) => (X3(0, n > 1))) && (!(!X2(n == m)))))))))));                                                                                                 \n"
  "mu X1(c:Bool, b:Bool) = (((!X1(c, true)) || ((!X2(b)) || (val(b)))) && (forall k:Nat.((val(k < 3)) && (((val(c)) => (X4(c, 1))) => (val(true)))))) => (val(true));                                                                                                                                                                                                                                                                                                                                          \n"
  "nu X2(b:Bool) = (!(!(((exists m:Nat.((val(m < 3)) || (val(m < 3)))) => ((X0(1, true)) => (!(val(false))))) && ((!((!X1(b, true)) && (!X3(0, false)))) => (val(false)))))) => (exists m:Nat.((val(m < 3)) || (forall k:Nat.((val(k < 3)) && (exists n:Nat.((val(n < 3)) || (forall n:Nat.((val(n < 3)) && (exists n:Nat.((val(n < 3)) || (forall n:Nat.((val(n < 3)) && (val(false))))))))))))));                                                                                                            \n"
  "mu X3(m:Nat, c:Bool) = (exists m:Nat.((val(m < 3)) || ((!(val(c))) && (forall k:Nat.((val(k < 3)) && (exists m:Nat.((val(m < 3)) || (forall k:Nat.((val(k < 3)) && (val(false))))))))))) || (exists m:Nat.((val(m < 3)) || (exists n:Nat.((val(n < 3)) || ((!((exists k:Nat.((val(k < 3)) || (!X4(false, k + 1)))) && ((!X1(m > 0, n < 2)) || ((val(n > 0)) || (val(c)))))) && (exists k:Nat.((val(k < 3)) || (exists n:Nat.((val(n < 3)) || (exists k:Nat.((val(k < 3)) || (X3(k + 1, k == n))))))))))))); \n"
  "nu X4(b:Bool, n:Nat) = (exists m:Nat.((val(m < 3)) || ((val(b)) || ((val(n > 1)) && ((X1(m > 1, n == m)) || (val(m > 0))))))) && ((exists k:Nat.((val(k < 3)) || (!(!((!X3(1, n > 0)) && (!(val(k == n)))))))) => (forall n:Nat.((val(n < 3)) && (exists k:Nat.((val(k < 3)) || (!(!(X4(n > 1, 0)))))))));                                                                                                                                                                                                  \n"
  "                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            \n"
  "init X0(0, true);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           \n"
  ;

// This pbes triggered an error with pbesinst finite
std::string random3 =
  "pbes                                                                                                                                                                                                                                                                                                                                                              \n"
  "mu X0(n:Nat, c:Bool) = ((!(((val(n < 2)) && (!X1)) && (exists k:Nat.((val(k < 3)) || (val(c)))))) && (((forall n:Nat.((val(n < 3)) && (!X4(n > 1)))) || (!(val(n > 1)))) => (val(true)))) && (exists n:Nat.((val(n < 3)) || (exists k:Nat.((val(k < 3)) || (forall m:Nat.((val(m < 3)) && (X0(k + 1, n < 3))))))));                                               \n"
  "mu X1 = (((!((val(true)) => (X2))) => (!(!(!(!X1))))) && (((val(false)) || (X3(1))) && (forall n:Nat.((val(n < 3)) && (val(true)))))) || (forall k:Nat.((val(k < 3)) && (!(forall k:Nat.((val(k < 3)) && (exists m:Nat.((val(m < 3)) || (val(k < 2)))))))));                                                                                                      \n"
  "mu X2 = ((exists k:Nat.((val(k < 3)) || (exists k:Nat.((val(k < 3)) || (exists m:Nat.((val(m < 3)) || ((val(m == k)) => (X1)))))))) => (((val(true)) => (!X2)) || (exists k:Nat.((val(k < 3)) || (val(false)))))) => (!((val(false)) => (!(X3(0)))));                                                                                                             \n"
  "mu X3(n:Nat) = ((exists n:Nat.((val(n < 3)) || (exists n:Nat.((val(n < 3)) || (forall n:Nat.((val(n < 3)) && (val(false)))))))) && (forall m:Nat.((val(m < 3)) && (((forall m:Nat.((val(m < 3)) && (exists m:Nat.((val(m < 3)) || (exists m:Nat.((val(m < 3)) || (!(val(n > 0))))))))) && ((!X4(m > 1)) || (!X2))) || (val(n < 2)))))) => ((val(n < 2)) => (X1)); \n"
  "nu X4(b:Bool) = ((val(true)) => ((forall m:Nat.((val(m < 3)) && (val(false)))) => ((X1) || ((!X3(0)) => (forall n:Nat.((val(n < 3)) && (val(n > 1)))))))) && (forall m:Nat.((val(m < 3)) && (forall n:Nat.((val(n < 3)) && ((X2) && (val(false)))))));                                                                                                            \n"
  "                                                                                                                                                                                                                                                                                                                                                                  \n"
  "init X0(0, true);                                                                                                                                                                                                                                                                                                                                                 \n"
  ;

void test_pbes(const std::string& pbes_spec, bool test_finite, bool test_lazy)
{
  pbes p = txt2pbes(pbes_spec);
  std::cout << "------------------------------\n" << pbes_system::pp(p) << std::endl;
  if (!p.is_closed())
  {
    std::cout << "ERROR: the pbes is not closed!" << std::endl;
    return;
  }

  if (test_finite)
  {
    std::cout << "FINITE" << std::endl;
    try
    {
      using namespace pbes_system;
      pbes q1 = pbesinst_finite(p);
      std::cout << pbes_system::pp(q1) << std::endl;
    }
    catch (mcrl2::runtime_error e)
    {
      std::cout << "pbesinst failed: " << e.what() << std::endl;
    }
  }

  if (test_lazy)
  {
    std::cout << "LAZY" << std::endl;
    try
    {
      using namespace pbes_system;
      pbes q1 = pbesinst_lazy(p);
      std::cout << pbes_system::pp(q1) << std::endl;
    }
    catch (mcrl2::runtime_error e)
    {
      std::cout << "pbesinst failed: " << e.what() << std::endl;
    }
  }
}

void test_pbesinst()
{
  test_pbes(test1, true, false);
  test_pbes(test2, true, true);
  test_pbes(test3, true, false);
  test_pbes(test4, true, true);
  test_pbes(test5, true, true);
  test_pbes(test6, true, true);
  test_pbes(test7, true, true);
  test_pbes(test8, true, true);
#ifdef MCRL2_EXTENDED_TESTS
  test_pbes(random1, false, true);
  test_pbes(random2, false, true);
#endif
}

// Note: this test takes a lot of time!
void test_cabp()
{
  std::string CABP_SPECIFICATION =
    "% This file contains the cabp protocol as described in section 3.5 of         \n"
    "% S. Mauw and G.J. Veltink, editors, Algebraic Specification of Communication \n"
    "% Protocols, Cambridge tracts in theoretical computer science 36, Cambridge   \n"
    "% University Press, Cambridge 1993.                                           \n"
    "%                                                                             \n"
    "% With two data elements, the generated transition system has 464 states.     \n"
    "%                                                                             \n"
    "% It is interesting to see the clustering of this statespace in ltsgraph.     \n"
    "% The statespace after branching bisimulation contains 3 states and is        \n"
    "% exactly the same as the reduced statespace of the alternating bit protocol. \n"
    "%                                                                             \n"
    "% Note that it is interesting to compare the differences of the alternating   \n"
    "% bit protocol (abp), concurrent alternating bit protocol (cabp), one bit     \n"
    "% sliding window protocol (onebit) and the alternating bit protocol with      \n"
    "% independent acknowledgements (par), regarding the implementation, the       \n"
    "% the number of states and the external behaviour.                            \n"
    "                                                                              \n"
    "%-------------------------------  DATA  ----------------------------------    \n"
    "                                                                              \n"
    "sort DATA = struct d1 | d2;                                                   \n"
    "                                                                              \n"
    "%-------------------------------  error  ----------------------------------   \n"
    "                                                                              \n"
    "sort  error = struct ce | ae;                                                 \n"
    "                                                                              \n"
    "%-------------------------------  bit ------------------------------------    \n"
    "                                                                              \n"
    "sort  bit = struct bit0 | bit1;                                               \n"
    "                                                                              \n"
    "map   invert:bit -> bit;                                                      \n"
    "eqn   invert(bit1)=bit0;                                                      \n"
    "      invert(bit0)=bit1;                                                      \n"
    "                                                                              \n"
    "%-------------------------------  Frame ----------------------------------    \n"
    "                                                                              \n"
    "sort Frame = struct frame(getd : DATA, getb: bit);                            \n"
    "                                                                              \n"
    "%------------------------------  ACK   -----------------------------------    \n"
    "                                                                              \n"
    "sort ACK = struct ac;                                                         \n"
    "                                                                              \n"
    "%------------------------------  act   -----------------------------------    \n"
    "                                                                              \n"
    "act   r1,s2 : DATA;                                                           \n"
    "      c3,r3,s3,c4,r4,s4 : Frame;                                              \n"
    "      c4,r4,s4,c7,r7,s7 : error;                                              \n"
    "      c5,r5,s5,c8,r8,s8 : ACK;                                                \n"
    "      c6,r6,s6,c7,r7,s7 : bit;                                                \n"
    "      skip;                                                                   \n"
    "                                                                              \n"
    "%------------------------------  proc  -----------------------------------    \n"
    "                                                                              \n"
    "                                                                              \n"
    "proc  S = RM(bit0);                                                           \n"
    "      RM(b:bit) = sum d:DATA.r1(d).SF(frame(d,b));                            \n"
    "      SF(f:Frame) = s3(f).SF(f) + r8(ac).RM(invert(getb(f)));                 \n"
    "                                                                              \n"
    "      K  = sum f:Frame.r3(f).K(f);                                            \n"
    "      K(f:Frame) = (skip.s4(f)+skip.s4(ce)+skip).K;                           \n"
    "                                                                              \n"
    "      R = RF(bit0);                                                           \n"
    "      RF(b:bit) = sum d:DATA.r4(frame(d,b)).s2(d).s5(ac).RF(invert(b))        \n"
    "                     + sum d:DATA. r4(frame(d,invert(b))).RF(b)               \n"
    "                     + r4(ce).RF(b);                                          \n"
    "                                                                              \n"
    "      AS = AS(bit1);                                                          \n"
    "      AS(b:bit) = r5(ac).AS(invert(b)) + s6(b).AS(b);                         \n"
    "                                                                              \n"
    "      L = sum b:bit.r6(b) . L(b);                                             \n"
    "      L(b:bit) = ( skip.s7(b) + skip.s7(ae) + skip ).L;                       \n"
    "                                                                              \n"
    "      AR = AR(bit0);                                                          \n"
    "      AR(b:bit) = ( r7(ae) + r7(invert(b))) . AR(b)                           \n"
    "                   + r7(b).s8(ac).AR(invert(b));                              \n"
    "                                                                              \n"
    "init                                                                          \n"
    "   hide({c3,c4,c5,c6,c7,c8,skip},                                             \n"
    "     allow({c3,c4,c5,c6,c7,c8,skip,r1,s2},                                    \n"
    "       comm({r3|s3->c3, r4|s4->c4, r5|s5->c5, r6|s6->c6,                      \n"
    "                r7|s7->c7, r8|s8->c8},                                        \n"
    "               S || K || R || AS || L || AR )));                              \n"
    ;

  std::string INFINITELY_OFTEN_SEND = "nu X. mu Y. (<r1(d1)>X || <!r1(d1)>Y)";

  // create a pbes p
  lps::specification spec    = lps::linearise(CABP_SPECIFICATION);
  state_formulas::state_formula formula = state_formulas::parse_state_formula(INFINITELY_OFTEN_SEND, spec);
  bool timed = false;
  pbes p = lps2pbes(spec, formula, timed);
  pbes_system::detail::instantiate_global_variables(p);

  std::string expr = "(exists d_RM_00: DATA. (val(d_RM_00 == d1) && val(s31_RM == 1)) && X(2, bit0, frame(d_RM_00, b_RM), s32_K, f_K, s33_RF1, d_RF1, b_RF1, b_AS1, s35_L, b_L, s36_AR1, b_AR1)) || (((((((((((exists d_RM_00: DATA. (val(!(d_RM_00 == d1)) && val(s31_RM == 1)) && Y(2, bit0, frame(d_RM_00, b_RM), s32_K, f_K, s33_RF1, d_RF1, b_RF1, b_AS1, s35_L, b_L, s36_AR1, b_AR1)) || (exists e4_RF1_00: Bool, d4_RF1_00: DATA. val((s32_K == 3 && s33_RF1 == 1) && if(e4_RF1_00, frame(d4_RF1_00, invert(b_RF1)), frame(d4_RF1_00, b_RF1)) == f_K) && Y(s31_RM, b_RM, f_RM, 1, frame(d1, bit0), if(e4_RF1_00, 1, 2), if(e4_RF1_00, d1, d4_RF1_00), b_RF1, b_AS1, s35_L, b_L, s36_AR1, b_AR1))) || val(s32_K == 4 && s33_RF1 == 1) && Y(s31_RM, b_RM, f_RM, 1, frame(d1, bit0), 1, d1, b_RF1, b_AS1, s35_L, b_L, s36_AR1, b_AR1)) || val(s33_RF1 == 2) && Y(s31_RM, b_RM, f_RM, s32_K, f_K, 3, d1, b_RF1, b_AS1, s35_L, b_L, s36_AR1, b_AR1)) || val(s35_L == 1) && Y(s31_RM, b_RM, f_RM, s32_K, f_K, s33_RF1, d_RF1, b_RF1, b_AS1, 2, b_AS1, s36_AR1, b_AR1)) || (exists e5_L_00: Enum3. val(C3_fun2(e5_L_00, true, true, true) && s35_L == 2) && Y(s31_RM, b_RM, f_RM, s32_K, f_K, s33_RF1, d_RF1, b_RF1, b_AS1, C3_fun(e5_L_00, 1, 3, 4), C3_fun3(e5_L_00, bit0, b_L, bit0), s36_AR1, b_AR1))) || val(s35_L == 4 && s36_AR1 == 1) && Y(s31_RM, b_RM, f_RM, s32_K, f_K, s33_RF1, d_RF1, b_RF1, b_AS1, 1, bit0, 1, b_AR1)) || (exists e7_AR1_00: Bool. val((s35_L == 3 && s36_AR1 == 1) && if(e7_AR1_00, b_AR1, invert(b_AR1)) == b_L) && Y(s31_RM, b_RM, f_RM, s32_K, f_K, s33_RF1, d_RF1, b_RF1, b_AS1, 1, bit0, if(e7_AR1_00, 2, 1), b_AR1))) || val(s33_RF1 == 3) && Y(s31_RM, b_RM, f_RM, s32_K, f_K, 1, d1, invert(b_RF1), invert(b_AS1), s35_L, b_L, s36_AR1, b_AR1)) || (exists e_K_00: Enum3. val(C3_fun2(e_K_00, true, true, true) && s32_K == 2) && Y(s31_RM, b_RM, f_RM, C3_fun(e_K_00, 1, 3, 4), C3_fun1(e_K_00, frame(d1, bit0), f_K, frame(d1, bit0)), s33_RF1, d_RF1, b_RF1, b_AS1, s35_L, b_L, s36_AR1, b_AR1))) || val(s31_RM == 2 && s32_K == 1) && Y(2, bit0, f_RM, 2, f_RM, s33_RF1, d_RF1, b_RF1, b_AS1, s35_L, b_L, s36_AR1, b_AR1)) || val(s31_RM == 2 && s36_AR1 == 2) && Y(1, invert(getb(f_RM)), frame(d1, bit0), s32_K, f_K, s33_RF1, d_RF1, b_RF1, b_AS1, s35_L, b_L, 1, invert(b_AR1))";
  std::string subst = "s31_RM:Pos := 2; b_RM:bit := bit0; f_RM:Frame := frame(d2, bit0); s32_K:Pos := 3; f_K:Frame := frame(d2, bit0); s33_RF1:Pos := 1; d_RF1:DATA := d1; b_RF1:bit := bit0; b_AS1:bit := bit1; s35_L:Pos := 1; b_L:bit := bit0; s36_AR1:Pos := 1; b_AR1:bit := bit0";

  data::mutable_map_substitution<> sigma;
  pbes_expression t = parse_pbes_expression(expr, subst, p, sigma);
  pbesinst_algorithm algorithm(p.data());
  enumerate_quantifiers_rewriter& R = algorithm.rewriter();
  data::mutable_indexed_substitution<> sigma1;
  for (auto i = sigma.begin(); i != sigma.end(); ++i)
  {
    sigma1[i->first] = i->second;
  }
  pbes_expression z = R(t, sigma1);
}

// Note: this test takes a lot of time!
void test_balancing_plat()
{
  using namespace pbes_system;

  const std::string BALANCE_PLAT_SPECIFICATION =
    " % Specification of balancing coins to determine the single coin with                 \n"
    " % different weight.                                                                  \n"
    "                                                                                      \n"
    " % C: Total number of coins                                                           \n"
    " map  C: Nat;                                                                         \n"
    " eqn  C = 12;                                                                         \n"
    "                                                                                      \n"
    " % Every coin can be in one of four categories: NHL, NH, NL, and N,                   \n"
    " % where:                                                                             \n"
    " % N: possibly normal weight                                                          \n"
    " % H: possibly heavy weight                                                           \n"
    " % L: possibly light weight                                                           \n"
    " % We count the number of coins in every category, but we do not count                \n"
    " % the number of coins in N explicitly, because:                                      \n"
    " %   |N| = C - ( |NHL| + |NH| + |NL| )                                                \n"
    "                                                                                      \n"
    " map                                                                                  \n"
    "      lexleq: Nat # Nat # Nat # Nat # Nat # Nat -> Bool;                              \n"
    "      is_better: Nat # Nat # Nat # Nat # Nat # Nat -> Bool;                           \n"
    "      is_useful: Nat # Nat # Nat # Nat # Nat # Nat # Nat # Nat # Nat -> Bool;         \n"
    "      is_possible: Nat # Nat # Nat # Nat # Nat # Nat # Nat # Nat # Nat -> Bool;       \n"
    "                                                                                      \n"
    " var  d1,d2,d3,e1,e2,e3,f1,f2,f3: Nat;                                                \n"
    "                                                                                      \n"
    " eqn                                                                                  \n"
    "      % lexicographical ordening on distributions; this is needed to                  \n"
    "      % eliminate half of the possibilities for balancing: only consider              \n"
    "      % X vs. Y and not Y vs. X, if X <= Y.                                           \n"
    "      lexleq(d1,d2,d3,e1,e2,e3) =                                                     \n"
    "          d1<e1 || (d1==e1 && d2<e2) || (d1==e1 && d2==e2 && d3<=e3);                 \n"
    "                                                                                      \n"
    "      % determines whether a distribution d is 'better than' a                        \n"
    "      % distribution e, in the sense that in d we have more certainty (or             \n"
    "      % less uncertainty) about a larger number of coins                              \n"
    "      is_better(d1,d2,d3,e1,e2,e3) = d1+d2+d3 < e1+e2+e3 || d1 < e1;                  \n"
    "                                                                                      \n"
    "      % determines whether weighing e against f is useful in situation d:             \n"
    "      % all possible outcomes should be an improvement                                \n"
    "      is_useful(d1,d2,d3,e1,e2,e3,f1,f2,f3) =                                         \n"
    "        is_better(Int2Nat(d1-e1-f1),Int2Nat(d2-e2-f2),Int2Nat(d3-e3-f3),d1,d2,d3) &&  \n"
    "        is_better(0,e1+e2,f1+f3,d1,d2,d3) &&                                          \n"
    "        is_better(0,f1+f2,e1+e3,d1,d2,d3);                                            \n"
    "                                                                                      \n"
    "      % determines whether weighing e against f is possible in situation              \n"
    "      % d:                                                                            \n"
    "      % - for every category X: X(e) + X(f) <= X(d)                                   \n"
    "      % - if total(e) < total(f) then N 'normal' coins are added to e                 \n"
    "      %   such that N = total(f) - total(e), so N 'normal' coins must                 \n"
    "      %   be available in situation d, i.e. N <= C - total(d).                        \n"
    "      % - analogously if total(e) > total(f).                                         \n"
    "      is_possible(d1,d2,d3,e1,e2,e3,f1,f2,f3) =                                       \n"
    "        e1+f1 <= d1 && e2+f2 <= d2 && e3+f3 <= d3 &&                                  \n"
    "        ( e1+e2+e3 == f1+f2+f3 ||                                                     \n"
    "          (e1+e2+e3 < f1+f2+f3 && f1+f2+f3 - e1-e2-e3 <= C - d1-d2-d3) ||             \n"
    "          (f1+f2+f3 < e1+e2+e3 && e1+e2+e3 - f1-f2-f3 <= C - d1-d2-d3)                \n"
    "        );                                                                            \n"
    "                                                                                      \n"
    " act  weigh, equal, greater, smaller: Nat # Nat # Nat # Nat # Nat # Nat;              \n"
    "      done;                                                                           \n"
    "                                                                                      \n"
    " proc BalancingAct(NHL,NH,NL:Nat) =                                                   \n"
    "      % we're done if |NHL| + |NH| + |NL| == 1                                        \n"
    "        (NHL+NH+NL == 1) -> done . BalancingAct(NHL,NH,NL)                            \n"
    "                                                                                      \n"
    "      + (NHL+NH+NL >  1) ->                                                           \n"
    "          (                                                                           \n"
    "            sum nhl_l,nh_l,nl_l:Nat, nhl_r,nh_r,nl_r:Nat .                            \n"
    "                                                                                      \n"
    "            (lexleq(nhl_l,nh_l,nl_l,nhl_r,nh_r,nl_r) &&                               \n"
    "            is_possible(NHL,NH,NL,nhl_l,nh_l,nl_l,nhl_r,nh_r,nl_r) &&                 \n"
    "            is_useful(NHL,NH,NL,nhl_l,nh_l,nl_l,nhl_r,nh_r,nl_r)) ->                  \n"
    "                                                                                      \n"
    "              weigh(nhl_l,nh_l,nl_l,nhl_r,nh_r,nl_r) .                                \n"
    "              (                                                                       \n"
    "              % left and right have equal weight                                      \n"
    "              ((NHL-nhl_l-nhl_r + NH-nh_l-nh_r + NL-nl_l-nl_r > 0) ->                 \n"
    "                equal(nhl_l,nh_l,nl_l,nhl_r,nh_r,nl_r) .                              \n"
    "                  BalancingAct(Int2Nat(NHL-nhl_l-nhl_r),                              \n"
    "                               Int2Nat(NH-nh_l-nh_r),                                 \n"
    "                               Int2Nat(NL-nl_l-nl_r)))                                \n"
    "              +                                                                       \n"
    "              % left is heavier than right                                            \n"
    "              ((nhl_l+nh_l + nhl_r+nl_r > 0) ->                                       \n"
    "                greater(nhl_l,nh_l,nl_l,nhl_r,nh_r,nl_r) .                            \n"
    "                  BalancingAct(0,nhl_l+nh_l,nhl_r+nl_r))                              \n"
    "              +                                                                       \n"
    "              % left is lighter than right                                            \n"
    "              ((nhl_r+nh_r + nhl_l+nl_l > 0) ->                                       \n"
    "                smaller(nhl_l,nh_l,nl_l,nhl_r,nh_r,nl_r) .                            \n"
    "                  BalancingAct(0,nhl_r+nh_r,nhl_l+nl_l))                              \n"
    "              )                                                                       \n"
    "          );                                                                          \n"
    "                                                                                      \n"
    " init BalancingAct(C,0,0);                                                            \n"
    ;

  lps::specification spec = lps::linearise(BALANCE_PLAT_SPECIFICATION);
  state_formulas::state_formula formula = state_formulas::parse_state_formula(lps::detail::NO_DEADLOCK(), spec);
  bool timed = false;
  pbes p = lps2pbes(spec, formula, timed);
  pbes_system::pbesinst_algorithm algorithm(p.data());
  algorithm.run(p);
  pbes q = algorithm.get_result();
}

void test_pbesinst_finite()
{
  pbes p = txt2pbes(random3);
  pbes q = pbesinst_finite(p);
  std::cerr << pbes_system::pp(q) << std::endl;

  std::string text =
    "sort D = struct d1 | d2;                                        \n"
    "                                                                \n"
    "pbes                                                            \n"
    "                                                                \n"
    "nu X(d:D) = (val(d == d1) && X(d2)) || (val(d == d2) && X(d1)); \n"
    "                                                                \n"
    "init X(d1);                                                     \n"
    ;
  pbes p1 = txt2pbes(text);
  pbesinst_finite_algorithm algorithm(data::jitty);
  pbesinst_variable_map variable_map = mcrl2::pbes_system::detail::parse_pbes_parameter_map(p1, "X(*:D)");
  algorithm.run(p1, variable_map);
}

void test_abp_no_deadlock()
{
  lps::specification spec = lps::linearise(lps::detail::ABP_SPECIFICATION());
  state_formulas::state_formula formula = state_formulas::parse_state_formula(lps::detail::NO_DEADLOCK(), spec);
  bool timed = false;
  pbes p = lps2pbes(spec, formula, timed);
  data::rewriter::strategy rewriter_strategy = data::jitty;
  bool print_equations = true;
  pbes_system::pbesinst_algorithm algorithm(p.data(), rewriter_strategy, print_equations);
  algorithm.run(p);
  pbes q = algorithm.get_result();
  std::cout << "--- ABP ---" << std::endl;
  std::cout << pbes_system::pp(q) << std::endl;
  BOOST_CHECK(is_bes(q));
}

// Example supplied by Tim Willemse, 23-05-2011
void test_functions()
{
  std::string text =
    "sort D = struct one | two;           \n"
    "                                     \n"
    "map  f: D -> D;                      \n"
    "                                     \n"
    "eqn  f  =  lambda x: D. one;         \n"
    "                                     \n"
    "pbes nu X(d: D, g: D -> D) =         \n"
    "       forall e: D. X(e, g[e -> e]); \n"
    "                                     \n"
    "init X(one, f);                      \n"
    ;
  pbes p = txt2pbes(text);
  data::rewriter::strategy rewrite_strategy = data::jitty;
  pbesinst_finite_algorithm algorithm(rewrite_strategy);
  mcrl2::pbes_system::detail::pbes_parameter_map parameter_map = mcrl2::pbes_system::detail::parse_pbes_parameter_map(p, "X(*:D)");
  algorithm.run(p, parameter_map);
}

void test_pbesinst_symbolic(const std::string& text)
{
  pbes p;
  p = txt2pbes(text);
  pbesinst_symbolic_algorithm algorithm(p);
  algorithm.run();
}

void test_pbesinst_symbolic()
{
  test_pbesinst_symbolic(test2);
  test_pbesinst_symbolic(test4);
  test_pbesinst_symbolic(test5);
  test_pbesinst_symbolic(test6);
}

int test_main(int argc, char** argv)
{
  mcrl2::log::mcrl2_logger::set_reporting_level(mcrl2::log::debug, "symbolic");
  mcrl2::log::mcrl2_logger::set_reporting_level(mcrl2::log::debug);

  test_pbesinst_symbolic();
  pbes_system::detail::set_bes_equation_limit(100000);
  test_pbesinst();
  test_pbesinst_finite();
  test_abp_no_deadlock();
  test_functions();

#ifdef MCRL2_EXTENDED_TESTS
  test_cabp();
  test_balancing_plat();
#endif

  return 0;
}
