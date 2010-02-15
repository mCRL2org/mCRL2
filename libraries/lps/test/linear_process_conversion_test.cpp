// Author(s): Wieger Wesselink, Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file linear_process_conversion_test.cpp
/// \brief Add your file description here.

#include <set>
#include <boost/test/minimal.hpp>
#include "mcrl2/core/garbage_collection.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/lps/detail/linear_process_conversion_visitor.h"
#include "mcrl2/process/process_specification.h"
#include "mcrl2/process/parse.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace mcrl2;
using namespace mcrl2::process;
using namespace mcrl2::lps;

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

const std::string SPEC3 =
  "act a : Bool;                      \n"
  "proc X = sum b,c:Bool . (b && c) -> a(b).X; \n"
  "init X;                            \n"
  ;

const std::string ABS_SPEC_LINEARIZED = 
//  "sort D = struct d1 | d2;                                                                                                     \n"
//  "     Error = struct e;                                                                                                       \n"
//  "                                                                                                                             \n"
//  "act  r1,s4: D;                                                                                                               \n"
//  "     s2,r2,c2,s3,r3,c3: D # Bool;                                                                                            \n"
//  "     s3,r3,c3: Error;                                                                                                        \n"
//  "     s5,r5,c5,s6,r6,c6: Bool;                                                                                                \n"
//  "     s6,r6,c6: Error;                                                                                                        \n"
//  "     i;                                                                                                                      \n"
//  "                                                                                                                             \n"
//  "proc P(s31_S: Pos, d_S: D, b_S: Bool, s32_K: Pos, d_K: D, b_K: Bool, s33_L: Pos, b_L: Bool, s34_R: Pos, d_R: D, b_R: Bool) = \n"
//  "       sum e1_S: Bool.                                                                                                       \n"
//  "         ((s31_S == 3 && s33_L == 3) && if(e1_S, !b_S, b_S) == b_L) ->                                                       \n"
//  "         c6(if(e1_S, !b_S, b_S)) .                                                                                           \n"
//  "         P(if(e1_S, 2, 1), if(e1_S, d_S, d2), if(e1_S, b_S, !b_S), s32_K, d_K, b_K, 1, false, s34_R, d_R, b_R)               \n"
//  "     + (s31_S == 3 && s33_L == 4) ->                                                                                         \n"
//  "         c6(e) .                                                                                                             \n"
//  "         P(2, d_S, b_S, s32_K, d_K, b_K, 1, false, s34_R, d_R, b_R)                                                          \n"
//  "     + (s31_S == 2 && s32_K == 1) ->                                                                                         \n"
//  "         c2(d_S, b_S) .                                                                                                      \n"
//  "         P(3, d_S, b_S, 2, d_S, b_S, s33_L, b_L, s34_R, d_R, b_R)                                                            \n"
//  "     + sum e2_K: Bool.                                                                                                       \n"
//  "         (s32_K == 2) ->                                                                                                     \n"
//  "         i .                                                                                                                 \n"
//  "         P(s31_S, d_S, b_S, if(e2_K, 4, 3), if(e2_K, d2, d_K), if(e2_K, false, b_K), s33_L, b_L, s34_R, d_R, b_R)            \n"
//  "     + sum e4_R: Bool.                                                                                                       \n"
//  "         (s33_L == 1 && if(e4_R, s34_R == 4, s34_R == 3)) ->                                                                 \n"
//  "         c5(if(e4_R, !b_R, b_R)) .                                                                                           \n"
//  "         P(s31_S, d_S, b_S, s32_K, d_K, b_K, 2, if(e4_R, !b_R, b_R), 1, d2, if(e4_R, b_R, !b_R))                             \n"
//  "     + (s34_R == 2) ->                                                                                                       \n"
//  "         s4(d_R) .                                                                                                           \n"
//  "         P(s31_S, d_S, b_S, s32_K, d_K, b_K, s33_L, b_L, 3, d2, b_R)                                                         \n"
//  "     + sum e3_L: Bool.                                                                                                       \n"
//  "         (s33_L == 2) ->                                                                                                     \n"
//  "         i .                                                                                                                 \n"
//  "         P(s31_S, d_S, b_S, s32_K, d_K, b_K, if(e3_L, 4, 3), if(e3_L, false, b_L), s34_R, d_R, b_R)                          \n"
//  "     + (s32_K == 4 && s34_R == 1) ->                                                                                         \n"
//  "         c3(e) .                                                                                                             \n"
//  "         P(s31_S, d_S, b_S, 1, d2, false, s33_L, b_L, 4, d2, b_R)                                                            \n"
//  "     + sum e5_R: Bool.                                                                                                       \n"
//  "         ((s32_K == 3 && s34_R == 1) && if(e5_R, b_R, !b_R) == b_K) ->                                                       \n"
//  "         c3(d_K, if(e5_R, b_R, !b_R)) .                                                                                      \n"
//  "         P(s31_S, d_S, b_S, 1, d2, false, s33_L, b_L, if(e5_R, 2, 4), if(e5_R, d_K, d2), b_R)                                \n"
//  "     + sum d3_S: D.                                                                                                          \n"
//  "         (s31_S == 1) ->                                                                                                     \n"
//  "         r1(d3_S) .                                                                                                          \n"
//  "         P(2, d3_S, b_S, s32_K, d_K, b_K, s33_L, b_L, s34_R, d_R, b_R)                                                       \n"
//  "     + true ->                                                                                                               \n"
//  "         delta;                                                                                                              \n"
//  "                                                                                                                             \n"
//  "init P(1, d2, true, 1, d2, false, 1, false, 1, d2, true);                                                                    \n"
//  ;
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
  "glob dc,dc0,dc1,dc3,dc5,dc7,dc13,dc14,dc15,dc16,dc17,dc18: D;                                                                \n"
  "     dc2,dc4,dc6,dc8,dc9,dc10,dc11,dc12: Bool;                                                                               \n"
  "                                                                                                                             \n"
  "proc P(s30_S: Pos, d_S: D, b_S: Bool, s31_K: Pos, d_K: D, b_K: Bool, s32_L: Pos, b_L: Bool, s33_R: Pos, d_R: D, b_R: Bool) = \n"
  "       sum e_S: Bool.                                                                                                        \n"
  "         ((s30_S == 3 && s32_L == 3) && if(e_S, b_S, !b_S) == b_L) ->                                                        \n"
  "         c6(if(e_S, b_S, !b_S)) .                                                                                            \n"
  "         P(s30_S = if(e_S, 1, 2), d_S = if(e_S, dc0, d_S), b_S = if(e_S, !b_S, b_S), s32_L = 1, b_L = dc11)                  \n"
  "     + (s30_S == 3 && s32_L == 4) ->                                                                                         \n"
  "         c6(e) .                                                                                                             \n"
  "         P(s30_S = 2, s32_L = 1, b_L = dc12)                                                                                 \n"
  "     + (s30_S == 2 && s31_K == 1) ->                                                                                         \n"
  "         c2(d_S, b_S) .                                                                                                      \n"
  "         P(s30_S = 3, s31_K = 2, d_K = d_S, b_K = b_S)                                                                       \n"
  "     + sum e3_R: Bool.                                                                                                       \n"
  "         ((s31_K == 3 && s33_R == 1) && if(e3_R, !b_R, b_R) == b_K) ->                                                       \n"
  "         c3(d_K, if(e3_R, !b_R, b_R)) .                                                                                      \n"
  "         P(s31_K = 1, d_K = dc5, b_K = dc6, s33_R = if(e3_R, 4, 2), d_R = if(e3_R, dc14, d_K))                               \n"
  "     + (s31_K == 4 && s33_R == 1) ->                                                                                         \n"
  "         c3(e) .                                                                                                             \n"
  "         P(s31_K = 1, d_K = dc7, b_K = dc8, s33_R = 4, d_R = dc15)                                                           \n"
  "     + sum e2_R: Bool.                                                                                                       \n"
  "         (s32_L == 1 && if(e2_R, s33_R == 4, s33_R == 3)) ->                                                                 \n"
  "         c5(if(e2_R, !b_R, b_R)) .                                                                                           \n"
  "         P(s32_L = 2, b_L = if(e2_R, !b_R, b_R), s33_R = 1, d_R = if(e2_R, dc18, dc17), b_R = if(e2_R, b_R, !b_R))           \n"
  "     + (s33_R == 2) ->                                                                                                       \n"
  "         s4(d_R) .                                                                                                           \n"
  "         P(s33_R = 3, d_R = dc16)                                                                                            \n"
  "     + sum e1_L: Bool.                                                                                                       \n"
  "         (s32_L == 2) ->                                                                                                     \n"
  "         i .                                                                                                                 \n"
  "         P(s32_L = if(e1_L, 4, 3), b_L = if(e1_L, dc10, b_L))                                                                \n"
  "     + sum e0_K: Bool.                                                                                                       \n"
  "         (s31_K == 2) ->                                                                                                     \n"
  "         i .                                                                                                                 \n"
  "         P(s31_K = if(e0_K, 4, 3), d_K = if(e0_K, dc3, d_K), b_K = if(e0_K, dc4, b_K))                                       \n"
  "     + sum d0_S: D.                                                                                                          \n"
  "         (s30_S == 1) ->                                                                                                     \n"
  "         r1(d0_S) .                                                                                                          \n"
  "         P(s30_S = 2, d_S = d0_S)                                                                                            \n"
  "     + delta;                                                                                                                \n"
  "                                                                                                                             \n"
  "init P(1, dc, true, 1, dc1, dc2, 1, dc9, 1, dc13, true);                                                                     \n"
  ;

void test_process(std::string text)
{
  process_specification pspec = parse_process_specification(text);
  specification lspec = parse_linear_process_specification(text);

  bool linear = is_linear(pspec, true);
  if (linear)
  {
    process::detail::linear_process_conversion_visitor visitor;
    specification spec = visitor.convert(pspec);
  }
  else
  {
    try
    {
      process::detail::linear_process_conversion_visitor visitor;
      specification spec = visitor.convert(pspec);
      BOOST_CHECK(false); // not supposed to arrive here
    }
    catch(...)
    {
      // skip
    }
  }
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  std::clog << "SPEC1" << std::endl;
  test_process(SPEC1);
  core::garbage_collect();
  std::clog << "SPEC2" << std::endl;
  test_process(SPEC2);
  core::garbage_collect();
  std::clog << "SPEC3" << std::endl;
  test_process(SPEC3);
  core::garbage_collect();
  std::clog << "ABS_SPEC_LINEARIZED" << std::endl;
  test_process(ABS_SPEC_LINEARIZED);
  core::garbage_collect();

  return 0;
}

