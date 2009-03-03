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
#include "mcrl2/lps/parse.h"
#include "mcrl2/lps/process.h"
#include "mcrl2/lps/process_expression_visitor.h"
#include "mcrl2/lps/process_expression_builder.h"
#include "mcrl2/lps/detail/linear_process_expression_visitor.h"
#include "mcrl2/lps/detail/linear_process_conversion_visitor.h"

#include "mcrl2/lps/mcrl22lps.h"

using namespace mcrl2;
using namespace mcrl2::lps;

void visit_process_expression(const process_expression& x)
{
  process_expression_visitor<> visitor;
  visitor.visit(x);
}

void build_process_expression(const process_expression& x)
{
  process_expression_builder<> visitor;
  visitor.visit(x);
}

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

void test_process(std::string text)
{
  std::cout << "----------------------------------" << std::endl;
  //specification spec1 = mcrl22lps(text);
  //std::cout << "LPS summands:\n" << core::pp(spec1.process().summands()) << std::endl;

  process_specification spec = parse_process_specification(text);
  specification sp = parse_linear_process_specification(text);
  std::cout << "<spec>" << core::pp(sp) << std::endl;

  //std::cout << core::pp(spec.data()) << std::endl
  //          << core::pp(spec.actions()) << std::endl
  //          << core::pp(spec.equations()) << std::endl
  //          << core::pp(spec.init()) << std::endl;
  for (process_equation_list::iterator i = spec.equations().begin(); i != spec.equations().end(); ++i)
  {
    visit_process_expression(i->expression());
    build_process_expression(i->expression());
    bool linear = detail::linear_process_expression_visitor().is_linear(*i);
    std::cerr << core::pp(*i) << " is " << (linear ? "" : "not") << "linear" << std::endl;
    if (linear)
    {
      detail::linear_process_conversion_visitor visitor;
      visitor.convert(*i);
      std::cerr << "summands:\n";
      summand_list s(visitor.result.begin(), visitor.result.end());
      std::cerr << core::pp(s) << std::endl;
    }
  }
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  test_process(SPEC1);
  test_process(SPEC2);
  test_process(ABS_SPEC_LINEARIZED);

  return 0;
}
