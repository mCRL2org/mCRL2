// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/detail/test_input.h
/// \brief This file contains some specifications used for testing.

#ifndef MCRL2_LPS_DETAIL_TEST_INPUT_H
#define MCRL2_LPS_DETAIL_TEST_INPUT_H

#include <string>

namespace mcrl2 {

namespace lps {

namespace detail {

inline
std::string ABP_SPECIFICATION()
{
  return

  "% This file contains the alternating bit protocol, as described in W.J.    \n"
  "% Fokkink, J.F. Groote and M.A. Reniers, Modelling Reactive Systems.       \n"
  "%                                                                          \n"
  "% The only exception is that the domain D consists of two data elements to \n"
  "% facilitate simulation.                                                   \n"
  "                                                                           \n"
  "sort                                                                       \n"
  "  D     = struct d1 | d2;                                                  \n"
  "  Error = struct e;                                                        \n"
  "                                                                           \n"
  "act                                                                        \n"
  "  r1,s4: D;                                                                \n"
  "  s2,r2,c2: D # Bool;                                                      \n"
  "  s3,r3,c3: D # Bool;                                                      \n"
  "  s3,r3,c3: Error;                                                         \n"
  "  s5,r5,c5: Bool;                                                          \n"
  "  s6,r6,c6: Bool;                                                          \n"
  "  s6,r6,c6: Error;                                                         \n"
  "  i;                                                                       \n"
  "                                                                           \n"
  "proc                                                                       \n"
  "  S(b:Bool)     = sum d:D. r1(d).T(d,b);                                   \n"
  "  T(d:D,b:Bool) = s2(d,b).(r6(b).S(!b)+(r6(!b)+r6(e)).T(d,b));             \n"
  "                                                                           \n"
  "  R(b:Bool)     = sum d:D. r3(d,b).s4(d).s5(b).R(!b)+                      \n"
  "                  (sum d:D.r3(d,!b)+r3(e)).s5(!b).R(b);                    \n"
  "                                                                           \n"
  "  K             = sum d:D,b:Bool. r2(d,b).(i.s3(d,b)+i.s3(e)).K;           \n"
  "                                                                           \n"
  "  L             = sum b:Bool. r5(b).(i.s6(b)+i.s6(e)).L;                   \n"
  "                                                                           \n"
  "init                                                                       \n"
  "  allow({r1,s4,c2,c3,c5,c6,i},                                             \n"
  "    comm({r2|s2->c2, r3|s3->c3, r5|s5->c5, r6|s6->c6},                     \n"
  "        S(true) || K || L || R(true)                                       \n"
  "    )                                                                      \n"
  "  );                                                                       \n"
  ;
}

inline
std::string LINEAR_ABP_SPECIFICATION()
{
  return

      "sort D = struct d1 | d2;\n"
      "     Error = struct e;\n"
      "act  r1,s4: D;\n"
      "     s2,r2,c2,s3,r3,c3: D # Bool;\n"
      "     s3,r3,c3: Error;\n"
      "     s5,r5,c5,s6,r6,c6: Bool;\n"
      "     s6,r6,c6: Error;\n"
      "     i;\n"
      "proc P(s30_S: Pos, d_S: D, b_S: Bool, s31_K: Pos, d_K: D, b_K: Bool, s32_L: Pos, b_L: Bool, s33_R: Pos, d_R: D, b_R: Bool) =\n"
      "       sum d0_S: D.\n"
      "         (s30_S == 1) ->\n"
      "         r1(d0_S) .\n"
      "         P(s30_S = 2, d_S = d0_S)\n"
      "     + ((s31_K == 3 && s33_R == 1) && !b_R == b_K) ->\n"
      "         c3(d_K, !b_R) .\n"
      "         P(s31_K = 1, d_K = d1, b_K = true, s33_R = 4, d_R = d1)\n"
      "     + ((s31_K == 3 && s33_R == 1) && b_R == b_K) ->\n"
      "         c3(d_K, b_R) .\n"
      "         P(s31_K = 1, d_K = d1, b_K = true, s33_R = 2, d_R = d_K)\n"
      "     + (s31_K == 4 && s33_R == 1) ->\n"
      "         c3(e) .\n"
      "         P(s31_K = 1, d_K = d1, b_K = true, s33_R = 4, d_R = d1)\n"
      "     + (s32_L == 2) ->\n"
      "         i .\n"
      "         P(s32_L = 4, b_L = true)\n"
      "     + (s32_L == 2) ->\n"
      "         i .\n"
      "         P(s32_L = 3)\n"
      "     + (s33_R == 2) ->\n"
      "         s4(d_R) .\n"
      "         P(s33_R = 3, d_R = d1)\n"
      "     + (s32_L == 1 && s33_R == 4) ->\n"
      "         c5(!b_R) .\n"
      "         P(s32_L = 2, b_L = !b_R, s33_R = 1, d_R = d1)\n"
      "     + (s32_L == 1 && s33_R == 3) ->\n"
      "         c5(b_R) .\n"
      "         P(s32_L = 2, b_L = b_R, s33_R = 1, d_R = d1, b_R = !b_R)\n"
      "     + (s31_K == 2) ->\n"
      "         i .\n"
      "         P(s31_K = 3)\n"
      "     + (s31_K == 2) ->\n"
      "         i .\n"
      "         P(s31_K = 4, d_K = d1, b_K = true)\n"
      "     + (s30_S == 3 && s32_L == 4) ->\n"
      "         c6(e) .\n"
      "         P(s30_S = 2, s32_L = 1, b_L = true)\n"
      "     + ((s30_S == 3 && s32_L == 3) && !b_S == b_L) ->\n"
      "         c6(!b_S) .\n"
      "         P(s30_S = 2, s32_L = 1, b_L = true)\n"
      "     + ((s30_S == 3 && s32_L == 3) && b_S == b_L) ->\n"
      "         c6(b_S) .\n"
      "         P(s30_S = 1, d_S = d1, b_S = !b_S, s32_L = 1, b_L = true)\n"
      "     + (s30_S == 2 && s31_K == 1) ->\n"
      "         c2(d_S, b_S) .\n"
      "         P(s30_S = 3, s31_K = 2, d_K = d_S, b_K = b_S)\n"
      "     + delta;\n"
      "init P(1, d1, true, 1, d1, true, 1, true, 1, d1, true);\n"
  ;
}

} // namespace detail

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_DETAIL_TEST_INPUT_H
