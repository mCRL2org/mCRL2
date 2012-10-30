// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file process_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <string>
#include <set>
#include <boost/test/minimal.hpp>
#include "mcrl2/process/parse.h"
#include "mcrl2/process/find.h"
#include "mcrl2/process/process_specification.h"

using namespace mcrl2;
using namespace mcrl2::process;

const std::string SPEC1 =
  "act a;                  \n"
  "proc X = a;             \n"
  "init X;                 \n"
  ;

const std::string SPEC2 =
  "act a;                  \n"
  "proc X(i: Nat) = a.X(i);\n"
  "init X(2);              \n"
  ;

const std::string ABS_SPEC_LINEARIZED =
  "sort D = struct d1 | d2;                                                                                                     \n"
  "     Error = struct e;                                                                                                       \n"
  "                                                                                                                             \n"
  "act  r1,s4: D;                                                                                                               \n"
  "     s2,r2,c2,s3,r3,c3: D # Bool;                                                                                            \n"
  "     s3,r3,c3: Error;                                                                                                        \n"
  "     s5,r5,c5,s6,r6,c6: Bool;                                                                                                \n"
  "     s6,r6,c6: Error;                                                                                                        \n"
  "     i;                                                                                                                      \n"
  "                                                                                                                             \n"
  "proc P(s31_S: Pos, d_S: D, b_S: Bool, s32_K: Pos, d_K: D, b_K: Bool, s33_L: Pos, b_L: Bool, s34_R: Pos, d_R: D, b_R: Bool) = \n"
  "       sum e1_S: Bool.                                                                                                       \n"
  "         ((s31_S == 3 && s33_L == 3) && if(e1_S, !b_S, b_S) == b_L) ->                                                       \n"
  "         c6(if(e1_S, !b_S, b_S)) .                                                                                           \n"
  "         P(if(e1_S, 2, 1), if(e1_S, d_S, d2), if(e1_S, b_S, !b_S), s32_K, d_K, b_K, 1, false, s34_R, d_R, b_R)               \n"
  "     + (s31_S == 3 && s33_L == 4) ->                                                                                         \n"
  "         c6(e) .                                                                                                             \n"
  "         P(2, d_S, b_S, s32_K, d_K, b_K, 1, false, s34_R, d_R, b_R)                                                          \n"
  "     + (s31_S == 2 && s32_K == 1) ->                                                                                         \n"
  "         c2(d_S, b_S) .                                                                                                      \n"
  "         P(3, d_S, b_S, 2, d_S, b_S, s33_L, b_L, s34_R, d_R, b_R)                                                            \n"
  "     + sum e2_K: Bool.                                                                                                       \n"
  "         (s32_K == 2) ->                                                                                                     \n"
  "         i .                                                                                                                 \n"
  "         P(s31_S, d_S, b_S, if(e2_K, 4, 3), if(e2_K, d2, d_K), if(e2_K, false, b_K), s33_L, b_L, s34_R, d_R, b_R)            \n"
  "     + sum e4_R: Bool.                                                                                                       \n"
  "         (s33_L == 1 && if(e4_R, s34_R == 4, s34_R == 3)) ->                                                                 \n"
  "         c5(if(e4_R, !b_R, b_R)) .                                                                                           \n"
  "         P(s31_S, d_S, b_S, s32_K, d_K, b_K, 2, if(e4_R, !b_R, b_R), 1, d2, if(e4_R, b_R, !b_R))                             \n"
  "     + (s34_R == 2) ->                                                                                                       \n"
  "         s4(d_R) .                                                                                                           \n"
  "         P(s31_S, d_S, b_S, s32_K, d_K, b_K, s33_L, b_L, 3, d2, b_R)                                                         \n"
  "     + sum e3_L: Bool.                                                                                                       \n"
  "         (s33_L == 2) ->                                                                                                     \n"
  "         i .                                                                                                                 \n"
  "         P(s31_S, d_S, b_S, s32_K, d_K, b_K, if(e3_L, 4, 3), if(e3_L, false, b_L), s34_R, d_R, b_R)                          \n"
  "     + (s32_K == 4 && s34_R == 1) ->                                                                                         \n"
  "         c3(e) .                                                                                                             \n"
  "         P(s31_S, d_S, b_S, 1, d2, false, s33_L, b_L, 4, d2, b_R)                                                            \n"
  "     + sum e5_R: Bool.                                                                                                       \n"
  "         ((s32_K == 3 && s34_R == 1) && if(e5_R, b_R, !b_R) == b_K) ->                                                       \n"
  "         c3(d_K, if(e5_R, b_R, !b_R)) .                                                                                      \n"
  "         P(s31_S, d_S, b_S, 1, d2, false, s33_L, b_L, if(e5_R, 2, 4), if(e5_R, d_K, d2), b_R)                                \n"
  "     + sum d3_S: D.                                                                                                          \n"
  "         (s31_S == 1) ->                                                                                                     \n"
  "         r1(d3_S) .                                                                                                          \n"
  "         P(2, d3_S, b_S, s32_K, d_K, b_K, s33_L, b_L, s34_R, d_R, b_R)                                                       \n"
  "     + true ->                                                                                                               \n"
  "         delta;                                                                                                              \n"
  "                                                                                                                             \n"
  "init P(1, d2, true, 1, d2, false, 1, false, 1, d2, true);                                                                    \n"
  ;

// CASE?? specifications were borrowed from sumelm_test.

std::string CASE1 =
  "sort S = struct s1 | s2;\n"
  "map f : S -> Bool;\n"
  "act a : S # Bool;\n"
  "proc P = sum c : S, b : Bool . (b == f(c) && c == s2) -> a(c, b) . P;\n"
  "init P;\n"
  ;

std::string CASE2 =
  "act a,b;\n"
  "proc P(s3_P: Pos) = sum y_P: Int. (s3_P == 1) -> a . P(2)\n"
  "                  + (s3_P == 2) -> b . P(1);\n"
  "init P(1);\n"
  ;

std::string CASE3 =
  "act a;\n"
  "proc P = sum y:Int . (4 == y) -> a . P;\n"
  "init P;\n"
  ;

std::string CASE4 =
  "act a;\n"
  "proc P = sum y:Int . (y == 4) -> a . P;\n"
  "init P;\n"
  ;

std::string CASE5 =
  "act a,b:Int;\n"
  "proc P = sum y:Int . (y == 4) -> a(y)@y . b(y*2)@(y+1) . P;\n"
  "init P;\n"
  ;

std::string CASE6 =
  "act a;\n"
  "proc P = sum y:Int . (y == y + 1) -> a . P;\n"
  "init P;\n"
  ;

std::string CASE7 =
  "sort D = struct d1 | d2 | d3;\n"
  "map g : D -> D;\n"
  "act a;\n"
  "proc P(c:D) = sum d:D . sum e:D . sum f:D . (d == e && e == g(e) && e == f) -> a . P(d);\n"
  "init P(d1);\n"
  ;

std::string CASE8 =
  "sort D = struct d1 | d2 | d3;\n"
  "act a;\n"
  "proc P(c:D) = sum d:D . sum e:D . sum f:D . (d == e && d == f) -> a . P(d);\n"
  "init P(d1);\n"
  ;

std::string CASE9 =
  "proc P = sum y:Bool . y -> delta;\n"
  "init P;\n"
  ;

std::string CASE10 =
  "act a:Nat;\n"
  "proc P(n0: Nat) = sum n: Nat. (n == n0 && n == 1) -> a(n0) . P(n);\n"
  "init P(0);\n"
  ;

void test_process(std::string text)
{
  process_specification spec = parse_process_specification(text);
  std::set<data::sort_expression> sorts;
  process::find_sort_expressions(spec, std::inserter(sorts, sorts.end()));
  std::cerr << "sorts: " << data::pp(data::sort_expression_list(sorts.begin(), sorts.end())) << std::endl;
}

int test_main(int argc, char* argv[])
{
  test_process(CASE1);
  test_process(CASE2);
  test_process(CASE3);
  test_process(CASE4);
  test_process(CASE5);
  test_process(CASE6);
  test_process(CASE7);
  test_process(CASE8);
  test_process(CASE9);
  test_process(CASE10);
  test_process(SPEC1);
  test_process(SPEC2);
  test_process(ABS_SPEC_LINEARIZED);

  return 0;
}

