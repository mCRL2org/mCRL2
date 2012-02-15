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
std::string DINING3_SPECIFICATION()
{
  return
    "% This is a specification of the dining philosophers problem                               \n"
    "% for 3 philosophers.                                                                      \n"
    "                                                                                           \n"
    "sort Phil = struct p1 | p2 | p3;                                                           \n"
    "     Fork = struct f1 | f2 | f3;                                                           \n"
    "                                                                                           \n"
    "map lf, rf: Phil -> Fork;                                                                  \n"
    "eqn lf(p1) = f1;                                                                           \n"
    "    lf(p2) = f2;                                                                           \n"
    "    lf(p3) = f3;                                                                           \n"
    "    rf(p1) = f3;                                                                           \n"
    "    rf(p2) = f1;                                                                           \n"
    "    rf(p3) = f2;                                                                           \n"
    "                                                                                           \n"
    "act get, put, up, down, lock, free: Phil # Fork;                                           \n"
    "    eat: Phil;                                                                             \n"
    "                                                                                           \n"
    "proc P_Phil(p: Phil) =                                                                     \n"
    "       %The following line implements the expression (get(p,lf(p))||get(p,rf(p)))          \n"
    "       (get(p,lf(p)).get(p,rf(p)) + get(p,rf(p)).get(p,lf(p)) + get(p,lf(p))|get(p,rf(p))) \n"
    "       . eat(p) .                                                                          \n"
    "       %The following line implements the expression (put(p,lf(p))||put(p,rf(p)))          \n"
    "       (put(p,lf(p)).put(p,rf(p)) + put(p,rf(p)).put(p,lf(p)) + put(p,lf(p))|put(p,rf(p))) \n"
    "       . P_Phil(p);                                                                        \n"
    "     P_Fork(f: Fork) = sum p:Phil. up(p,f) . down(p,f) . P_Fork(f);                        \n"
    "                                                                                           \n"
    "init block( { get, put, up, down },                                                        \n"
    "       comm( { get|up->lock, put|down->free },                                             \n"
    "         P_Fork(f1) || P_Fork(f2) || P_Fork(f3) ||                                         \n"
    "         P_Phil(p1) || P_Phil(p2) || P_Phil(p3)                                            \n"
    "     ));                                                                                   \n"
    ;
}

inline
std::string ONE_BIT_SLIDING_WINDOW_SPECIFICATION()
{
  return
    "% This file describes the onebit sliding window protocol as documented     \n"
    "% M.A. Bezem and J.F. Groote. A correctness proof of a one bit sliding     \n"
    "% window protocol in muCRL. The Computer Journal, 37(4): 289-307, 1994.    \n"
    "% This sliding window protocol is a bidirectional sliding window protocol  \n"
    "% with piggy backing, where the window sizes at the receiving and          \n"
    "% sending side have size 1. The behaviour of this sliding window protocol  \n"
    "% is that of two bidirectional buffers sending data from channel ra to     \n"
    "% sb, and from rc to sd. Both buffers have capacity 2.                     \n"
    "%   The external behaviour of the onebit protocol is rather complex.       \n"
    "% However, making only the behaviour visible at gates ra and sb reduced    \n"
    "% modulo branching bisimulation clearly shows the behaviour of             \n"
    "% a buffer of capacity 2.                                                  \n"
    "%                                                                          \n"
    "% Jan Friso Groote, translated from muCRL 30/12/2006                       \n"
    "                                                                           \n"
    "sort Bit = struct e0 | e1;                                                 \n"
    "     D= struct d1;                                                         \n"
    "     Frame=struct frame(dat:D,bit1:Bit,bit2:Bit);                          \n"
    "     Status=struct read?eq_read | choice?eq_choice | del?eq_del;           \n"
    "                                                                           \n"
    "map inv:Bit-> Bit;                                                         \n"
    "eqn  inv(e0)=e1;                                                           \n"
    "     inv(e1)=e0;                                                           \n"
    "                                                                           \n"
    "act  r,w,rc,sd:D;                                                          \n"
    "     rcl,scl,i_del,i_lost,ccl;                                             \n"
    "     r1,s1,c1,s2,r2,c2,s4,r4,c4:Frame;                                     \n"
    "                                                                           \n"
    "proc S(ready:Bool,rec:Bool,sts:Bool,d:D,e:D,p:Bit,q:Bit)=                  \n"
    "       ready -> sum d:D.r(d).S(false,rec,false,d,e,inv(p),q) +             \n"
    "       !rec -> w(e).S(ready,true,sts,d,e,p,q) +                            \n"
    "       rcl.S(ready,rec,false,d,e,p,q)+                                     \n"
    "       sum f:D,b1:Bit,b2:Bit.                                              \n"
    "           r4(frame(f,b1,b2)).                                             \n"
    "              (rec && b1==inv(q)) -> S(b2==p,false,sts,d,f,p,inv(q))       \n"
    "                                  <> S(b2==p,rec,sts,d,e,p,q) +            \n"
    "       !sts -> s1(frame(d,p,q)).S(ready,rec,true,d,e,p,q) +                \n"
    "       delta;                                                              \n"
    "                                                                           \n"
    "proc Tim= scl.Tim;                                                         \n"
    "                                                                           \n"
    "proc C(f:Frame,st:Status)=                                                 \n"
    "       eq_read(st) -> sum f:Frame.r1(f).C(f,choice)<>delta+                \n"
    "       eq_choice(st) -> (i_del.C(f,del)+i_lost.C(f,read))<>delta+          \n"
    "       eq_del(st) -> s2(f).C(f,read)<>delta ;                              \n"
    "                                                                           \n"
    "init hide ({c4,c2,ccl,c1,i_del,i_lost},                                    \n"
    "       allow({c1,ccl,c2,c4,i_del,i_lost,r,w,rc,sd},                        \n"
    "         comm({r2|s2->c2,r4|s4->c4},                                       \n"
    "           rename({w->sd},                                                 \n"
    "             allow({c1,ccl,r,w,s2,r4,i_del,i_lost},                        \n"
    "               comm({rcl|scl->ccl,r1|s1->c1},                              \n"
    "                 S(true,true,true,d1,d1,e0,e0)||                           \n"
    "                 Tim||                                                     \n"
    "                 C(frame(d1,e0,e0),read))))||                              \n"
    "           rename({r->rc,s2->s4,r4->r2},                                   \n"
    "             allow({c1,ccl,r,w,s2,r4,i_del,i_lost},                        \n"
    "               comm({rcl|scl->ccl,r1|s1->c1},                              \n"
    "                 S(true,true,true,d1,d1,e0,e0)||                           \n"
    "                 Tim||                                                     \n"
    "                 C(frame(d1,e0,e0) ,read)))))));                           \n"
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
