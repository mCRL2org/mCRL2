#ifndef LPS_TEST_SPECIFICATIONS_H
#define LPS_TEST_SPECIFICATIONS_H

#include <string>

const std::string SPEC1 =
  "act a:Nat;                               \n"
  "                                         \n"
  "map smaller: Nat#Nat -> Bool;            \n"
  "                                         \n"
  "var x,y : Nat;                           \n"
  "                                         \n"
  "eqn smaller(x,y) = x < y;                \n"
  "                                         \n"
  "proc P(n:Nat) = sum m: Nat. a(m). P(m);  \n"
  "                                         \n"
  "init P(0);                               \n";

const std::string LINEAR_ABP =
  "sort Error = struct e;\n"
  "    D = struct d1 | d2;\n"

  "act  i;\n"
  "     c6,r6,s6: Error;\n"
  "     c6,r6,s6,c5,r5,s5: Bool;\n"
  "     c3,r3,s3: Error;\n"
  "     c3,r3,s3,c2,r2,s2: D # Bool;\n"
  "     s4,r1: D;\n"

  "proc P(s30_S: Pos, d_S: D, b_S: Bool, s31_K: Pos, d_K: D, b_K: Bool, s32_L: Pos, b_L: Bool, s33_R: Pos, d_R: D, b_R: Bool) =\n"
  "       sum d0_S: D.\n"
  "         (s30_S == 1) ->\n"
  "         r1(d0_S) .\n"
  "         P(s30_S = 2, d_S = d0_S)\n"
  "     + sum e0_K: Bool.\n"
  "         (s31_K == 2) ->\n"
  "         i .\n"
  "         P(s31_K = if(e0_K, 4, 3), d_K = if(e0_K, d1, d_K), b_K = if(e0_K, true, b_K))\n"
  "     + sum e1_L: Bool.\n"
  "         (s32_L == 2) ->\n"
  "         i .\n"
  "         P(s32_L = if(e1_L, 4, 3), b_L = if(e1_L, true, b_L))\n"
  "     + (s33_R == 2) ->\n"
  "         s4(d_R) .\n"
  "         P(s33_R = 3, d_R = d1)\n"
  "     + sum e2_R: Bool.\n"
  "         (s32_L == 1 && if(e2_R, s33_R == 4, s33_R == 3)) ->\n"
  "         c5(if(e2_R, !b_R, b_R)) .\n"
  "         P(s32_L = 2, b_L = if(e2_R, !b_R, b_R), s33_R = 1, d_R = d1, b_R = if(e2_R, b_R, !b_R))\n"
  "     + (s31_K == 4 && s33_R == 1) ->\n"
  "         c3(e) .\n"
  "         P(s31_K = 1, d_K = d1, b_K = true, s33_R = 4, d_R = d1)\n"
  "     + sum e3_R: Bool.\n"
  "         ((s31_K == 3 && s33_R == 1) && if(e3_R, !b_R, b_R) == b_K) ->\n"
  "         c3(d_K, if(e3_R, !b_R, b_R)) .\n"
  "         P(s31_K = 1, d_K = d1, b_K = true, s33_R = if(e3_R, 4, 2), d_R = if(e3_R, d1, d_K))\n"
  "     + (s30_S == 2 && s31_K == 1) ->\n"
  "         c2(d_S, b_S) .\n"
  "         P(s30_S = 3, s31_K = 2, d_K = d_S, b_K = b_S)\n"
  "     + (s30_S == 3 && s32_L == 4) ->\n"
  "         c6(e) .\n"
  "         P(s30_S = 2, s32_L = 1, b_L = true)\n"
  "     + sum e_S: Bool.\n"
  "         ((s30_S == 3 && s32_L == 3) && if(e_S, b_S, !b_S) == b_L) ->\n"
  "         c6(if(e_S, b_S, !b_S)) .\n"
  "         P(s30_S = if(e_S, 1, 2), d_S = if(e_S, d1, d_S), b_S = if(e_S, !b_S, b_S), s32_L = 1, b_L = true)\n"
  "     + delta;\n"

  "init P(1, d1, true, 1, d1, true, 1, true, 1, d1, true);\n";

#endif // LPS_TEST_SPECIFICATIONS_H
