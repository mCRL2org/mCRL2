// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file bisimulation_test.cpp
/// \brief Test the bisimulation algorithm.

#define BOOST_TEST_MODULE bisimulation_test
#include <boost/test/included/unit_test.hpp>
#include "mcrl2/lps/detail/test_input.h"
#include "mcrl2/lps/linearise.h"
#include "mcrl2/pbes/bisimulation.h"
#include "mcrl2/pbes/detail/pbessolve_algorithm.h"

using namespace mcrl2;
using namespace mcrl2::lps;
using namespace mcrl2::pbes_system;
using namespace mcrl2::log;

void test_bisimulation(const std::string& s1, const std::string& s2,
                       bool strongly_bisimilar,
                       bool branching_bisimilar,
                       bool branching_similar,
                       bool weakly_bisimilar,
                       bool linearize = false)
{
  specification spec1;
  specification spec2;
  if (linearize)
  {
    spec1 = remove_stochastic_operators(linearise(s1));
    spec2 = remove_stochastic_operators(linearise(s2));
  }
  else
  {
    spec1 = parse_linear_process_specification(s1);
    spec2 = parse_linear_process_specification(s2);
  }

  std::clog << "Testing strong bisimulation" << std::endl;
  pbes sb  = strong_bisimulation(spec1, spec2);
  BOOST_CHECK(sb.is_well_typed());
  bool sb_solution = pbes_system::detail::pbessolve(sb);
  BOOST_CHECK(sb_solution == strongly_bisimilar);

  std::clog << "Testing branching bisimulation" << std::endl;
  pbes bb  = branching_bisimulation(spec1, spec2);
  bool bb_solution = pbes_system::detail::pbessolve(bb);
  BOOST_CHECK(bb.is_well_typed());
  BOOST_CHECK(bb_solution == branching_bisimilar);

  std::clog << "Testing branching simulation" << std::endl;
  pbes bs = branching_simulation_equivalence(spec1, spec2);
  bool bs_solution = pbes_system::detail::pbessolve(bs);
  BOOST_CHECK(bs.is_well_typed());
  BOOST_CHECK(bs_solution == branching_similar);

  std::clog << "Testing weak bisimulation" << std::endl;
  pbes wb  = weak_bisimulation(spec1, spec2);
  bool wb_solution = pbes_system::detail::pbessolve(wb);
  BOOST_CHECK(wb.is_well_typed());
  BOOST_CHECK(wb_solution == weakly_bisimilar);
}

BOOST_AUTO_TEST_CASE(ABP)
{
  test_bisimulation(lps::detail::LINEAR_ABP_SPECIFICATION(), lps::detail::LINEAR_ABP_SPECIFICATION(), true, true, true, true);
}

BOOST_AUTO_TEST_CASE(SMALLSPEC)
{
  const std::string SMALLSPEC =
    "act a,b;                 \n"
    "proc X(s: Pos) =         \n"
    "  (s == 1) -> a . X(2)   \n"
    "+ (s == 2) -> tau . X(3) \n"
    "+ (s == 3) -> tau . X(4) \n"
    "+ (s == 4) -> b . X(1);  \n"
    "init X(1);               \n"
    ;
  test_bisimulation(SMALLSPEC, SMALLSPEC, true, true, true, true);
}

BOOST_AUTO_TEST_CASE(small_different_specs)
{
  const std::string s1 =
    "act a,b;                 \n"
    "proc X(s: Pos) =         \n"
    "  (s == 1) ->  a . X(2)  \n"
    "+ (s == 2) -> b . X(1);  \n"
    "init X(1);               \n"
    ;
  const std::string s2 =
    "act a,b,c;               \n"
    "proc X(s: Pos) =     \n"
    "  (s == 1) ->  a . X(2)  \n"
    "+ (s == 1) ->  c . X(1)  \n"
    "+ (s == 2) -> b . X(1);  \n"
    "init X(1);               \n"
    ;
    ;
  test_bisimulation(s1, s2, false, false, false, false);
  test_bisimulation(s2, s1, false, false, false, false);
}

BOOST_AUTO_TEST_CASE(buffers_silent_lose)
{
  const std::string buffer =
    "sort D = struct d1 | d2;\n"
    "map  n: Pos;\n"
    "eqn  n  =  2;\n"
    "act  r,s: D;\n"
    "proc P(b_Buffer: List(D)) =\n"
    "       !(b_Buffer == []) ->\n"
    "         s(rhead(b_Buffer)) .\n"
    "         P(b_Buffer = rtail(b_Buffer))\n"
    "     + sum d_Buffer: D.\n"
    "         (#b_Buffer < 2) ->\n"
    "         r(d_Buffer) .\n"
    "         P(b_Buffer = d_Buffer |> b_Buffer)\n"
    "     + delta;\n"
    "init P([]);\n";

  const std::string lossy_buffer =
    "sort D = struct d1 | d2;\n"
    "map  n: Pos;\n"
    "eqn  n  =  2;\n"
    "act  r,s: D;\n"
    "proc P(s3_Buffer: Pos, d_Buffer: D, b_Buffer: List(D)) =\n"
    "       sum e_Buffer: Bool.\n"
    "         (s3_Buffer == 2) ->\n"
    "         tau .\n"
    "         P(s3_Buffer = 1, d_Buffer = d1, b_Buffer = if(e_Buffer, d_Buffer |> b_Buffer, b_Buffer))\n"
    "     + (s3_Buffer == 1 && !(b_Buffer == [])) ->\n"
    "         s(rhead(b_Buffer)) .\n"
    "         P(s3_Buffer = 1, d_Buffer = d1, b_Buffer = rtail(b_Buffer))\n"
    "     + sum d0_Buffer: D.\n"
    "         (s3_Buffer == 1 && #b_Buffer < 2) ->\n"
    "         r(d0_Buffer) .\n"
    "         P(s3_Buffer = 2, d_Buffer = d0_Buffer)\n"
    "     + delta;\n"
    "init P(1, d1, []);\n";

  test_bisimulation(buffer, lossy_buffer, false, false, false, false);
  test_bisimulation(lossy_buffer, buffer, false, false, false, false);
}

BOOST_AUTO_TEST_CASE(buffers_explicit_lose)
{
  const std::string buffer =
    "sort D = struct d1 | d2;\n"
    "map  n: Pos;\n"
    "eqn  n  =  2;\n"
    "act  r,s: D;\n"
    "proc P(b_Buffer: List(D)) =\n"
    "       !(b_Buffer == []) ->\n"
    "         s(rhead(b_Buffer)) .\n"
    "         P(b_Buffer = rtail(b_Buffer))\n"
    "     + sum d_Buffer: D.\n"
    "         (#b_Buffer < 2) ->\n"
    "         r(d_Buffer) .\n"
    "         P(b_Buffer = d_Buffer |> b_Buffer)\n"
    "     + delta;\n"
    "init P([]);\n";

  const std::string lossy_buffer =
      "sort D = struct d1 | d2;\n"
      "map  n: Pos;\n"
      "eqn  n  =  2;\n"
      "act  r,s: D;\n"
      "     lose;\n"
      "proc P(s3_Buffer: Pos, d_Buffer: D, b_Buffer: List(D)) =\n"
      "       sum d0_Buffer: D.\n"
      "         (s3_Buffer == 1 && #b_Buffer < 2) ->\n"
      "         r(d0_Buffer) .\n"
      "         P(s3_Buffer = 2, d_Buffer = d0_Buffer)\n"
      "     + (s3_Buffer == 1 && !(b_Buffer == [])) ->\n"
      "         s(rhead(b_Buffer)) .\n"
      "         P(s3_Buffer = 1, d_Buffer = d1, b_Buffer = rtail(b_Buffer))\n"
      "     + (s3_Buffer == 2) ->\n"
      "         tau .\n"
      "         P(s3_Buffer = 3, d_Buffer = d1)\n"
      "     + (s3_Buffer == 2) ->\n"
      "         tau .\n"
      "         P(s3_Buffer = 1, d_Buffer = d1, b_Buffer = d_Buffer |> b_Buffer)\n"
      "     + (s3_Buffer == 3) ->\n"
      "         lose .\n"
      "         P(s3_Buffer = 1, d_Buffer = d1)\n"
      "     + delta;\n"
      "init P(1, d1, []);\n";

  test_bisimulation(buffer, lossy_buffer, false, false, false, false);
  test_bisimulation(lossy_buffer, buffer, false, false, false, false);
}
