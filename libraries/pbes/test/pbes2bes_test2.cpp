// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbes2bes_test2.cpp
/// \brief Add your file description here.

//#define MCRL2_ENUMERATE_QUANTIFIERS_BUILDER_DEBUG
//#define MCRL2_ENUMERATE_QUANTIFIERS_BUILDER_DEBUG2
//#define MCRL2_PBES_EXPRESSION_BUILDER_DEBUG

#include <string>
#include <iostream>
#include <boost/test/minimal.hpp>
#include "mcrl2/core/text_utility.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/pbes/pbes_parse.h"
#include "mcrl2/pbes/pbes2bes_algorithm.h"
#include "mcrl2/core/garbage_collection.h"

using namespace mcrl2;
using namespace pbes_system;
using modal::detail::mcf2statefrm;
using modal::state_formula;
using lps::mcrl22lps;
using lps::specification;

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

void test1()
{
  // create a pbes p
  specification spec    = mcrl22lps(CABP_SPECIFICATION);
  state_formula formula = mcf2statefrm(INFINITELY_OFTEN_SEND, spec);
  bool timed = false;
  pbes<> p = lps2pbes(spec, formula, timed);
  p.instantiate_free_variables();

  std::string expr = "(exists d_RM_00: DATA. (val(d_RM_00 == d1) && val(s31_RM == 1)) && X(2, bit0, frame(d_RM_00, b_RM), s32_K, f_K, s33_RF1, d_RF1, b_RF1, b_AS1, s35_L, b_L, s36_AR1, b_AR1)) || (((((((((((exists d_RM_00: DATA. (val(!(d_RM_00 == d1)) && val(s31_RM == 1)) && Y(2, bit0, frame(d_RM_00, b_RM), s32_K, f_K, s33_RF1, d_RF1, b_RF1, b_AS1, s35_L, b_L, s36_AR1, b_AR1)) || (exists e4_RF1_00: Bool, d4_RF1_00: DATA. val((s32_K == 3 && s33_RF1 == 1) && if(e4_RF1_00, frame(d4_RF1_00, invert(b_RF1)), frame(d4_RF1_00, b_RF1)) == f_K) && Y(s31_RM, b_RM, f_RM, 1, frame(d1, bit0), if(e4_RF1_00, 1, 2), if(e4_RF1_00, d1, d4_RF1_00), b_RF1, b_AS1, s35_L, b_L, s36_AR1, b_AR1))) || val(s32_K == 4 && s33_RF1 == 1) && Y(s31_RM, b_RM, f_RM, 1, frame(d1, bit0), 1, d1, b_RF1, b_AS1, s35_L, b_L, s36_AR1, b_AR1)) || val(s33_RF1 == 2) && Y(s31_RM, b_RM, f_RM, s32_K, f_K, 3, d1, b_RF1, b_AS1, s35_L, b_L, s36_AR1, b_AR1)) || val(s35_L == 1) && Y(s31_RM, b_RM, f_RM, s32_K, f_K, s33_RF1, d_RF1, b_RF1, b_AS1, 2, b_AS1, s36_AR1, b_AR1)) || (exists e5_L_00: Enum3. val(C3_fun2(e5_L_00, true, true, true) && s35_L == 2) && Y(s31_RM, b_RM, f_RM, s32_K, f_K, s33_RF1, d_RF1, b_RF1, b_AS1, C3_fun(e5_L_00, 1, 3, 4), C3_fun3(e5_L_00, bit0, b_L, bit0), s36_AR1, b_AR1))) || val(s35_L == 4 && s36_AR1 == 1) && Y(s31_RM, b_RM, f_RM, s32_K, f_K, s33_RF1, d_RF1, b_RF1, b_AS1, 1, bit0, 1, b_AR1)) || (exists e7_AR1_00: Bool. val((s35_L == 3 && s36_AR1 == 1) && if(e7_AR1_00, b_AR1, invert(b_AR1)) == b_L) && Y(s31_RM, b_RM, f_RM, s32_K, f_K, s33_RF1, d_RF1, b_RF1, b_AS1, 1, bit0, if(e7_AR1_00, 2, 1), b_AR1))) || val(s33_RF1 == 3) && Y(s31_RM, b_RM, f_RM, s32_K, f_K, 1, d1, invert(b_RF1), invert(b_AS1), s35_L, b_L, s36_AR1, b_AR1)) || (exists e_K_00: Enum3. val(C3_fun2(e_K_00, true, true, true) && s32_K == 2) && Y(s31_RM, b_RM, f_RM, C3_fun(e_K_00, 1, 3, 4), C3_fun1(e_K_00, frame(d1, bit0), f_K, frame(d1, bit0)), s33_RF1, d_RF1, b_RF1, b_AS1, s35_L, b_L, s36_AR1, b_AR1))) || val(s31_RM == 2 && s32_K == 1) && Y(2, bit0, f_RM, 2, f_RM, s33_RF1, d_RF1, b_RF1, b_AS1, s35_L, b_L, s36_AR1, b_AR1)) || val(s31_RM == 2 && s36_AR1 == 2) && Y(1, invert(getb(f_RM)), frame(d1, bit0), s32_K, f_K, s33_RF1, d_RF1, b_RF1, b_AS1, s35_L, b_L, 1, invert(b_AR1))";
  std::string subst = "s31_RM:Pos := 2; b_RM:bit := bit0; f_RM:Frame := frame(d2, bit0); s32_K:Pos := 3; f_K:Frame := frame(d2, bit0); s33_RF1:Pos := 1; d_RF1:DATA := d1; b_RF1:bit := bit0; b_AS1:bit := bit1; s35_L:Pos := 1; b_L:bit := bit0; s36_AR1:Pos := 1; b_AR1:bit := bit0";

  pbes2bes_substitution_function sigma;
  pbes_expression t = parse_pbes_expression(expr, subst, p, sigma);
  pbes2bes_algorithm algorithm(p.data());
  pbes2bes_rewriter& R = algorithm.rewriter();
  pbes_expression z = R(t, sigma);
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT_DEBUG(argc, argv)
  test1();
  core::garbage_collect();

  return 0;
}
