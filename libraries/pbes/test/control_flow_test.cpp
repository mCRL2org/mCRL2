// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file control_flow_test.cpp
/// \brief Tests for control flow algorithm.

#include <boost/test/minimal.hpp>
#include "mcrl2/pbes/rewrite.h"
#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/pbes/txt2pbes.h"
#include "mcrl2/pbes/detail/is_pfnf.h"
#include "mcrl2/pbes/detail/control_flow.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;

void test_control_flow()
{
  std::string text =
    "pbes nu X(n: Nat)  = X(0) || X(n) || X(2);   \n"
    "     nu Y(m: Nat)  = Y(m) || X(1) || X(m);   \n"
    "     nu Z(p: Nat) = (forall b:Bool. true) && (true => (X(p) || X(1))) && val(true) && Y(p); \n"
    "init X(0);                                  \n"
    ;
  pbes<> p = txt2pbes(text, false);
  BOOST_CHECK(pbes_system::detail::is_pfnf(p));

  detail::pbes_control_flow_algorithm algorithm;
  algorithm.run(p);
  algorithm.print_graph();
}

void test_source_dest1()
{
  std::string text =
    "sort D = struct d1 | d2 | d3;\n"
    "\n"
    "map  match: Pos # D -> Bool;\n"
    "\n"
    "pbes nu Y(s3_Filter: Pos, d_Filter: D, s2_Filter: Pos, d_Filter1: D) =\n"
    "       (forall d5_Filter1: D. val(!(d5_Filter1 == d1)) || val(!(s3_Filter == 1 && match(1, d5_Filter1))) || X(2, d5_Filter1, s2_Filter, d_Filter1)) && (forall d4_Filter1: D. val(!(d4_Filter1 == d1)) || val(!(s3_Filter == 1 && !match(1, d4_Filter1))) || X(1, d1, s2_Filter, d_Filter1)) && (val(!(s3_Filter == 2 && s2_Filter == 1 && match(2, d_Filter))) || Y(1, d1, 2, d_Filter)) && (val(!(s3_Filter == 2 && s2_Filter == 1 && !match(2, d_Filter))) || Y(1, d1, 1, d1)) && (val(!(s2_Filter == 2)) || Y(s3_Filter, d_Filter, 1, d1)) && (forall d5_Filter: D. val(!(s3_Filter == 1 && match(1, d5_Filter))) || Y(2, d5_Filter, s2_Filter, d_Filter1)) && (forall d4_Filter: D. val(!(s3_Filter == 1 && !match(1, d4_Filter))) || Y(1, d1, s2_Filter, d_Filter1));\n"
    "     mu X(s3_Filter: Pos, d_Filter: D, s2_Filter: Pos, d_Filter1: D) =\n"
    "       (val(s3_Filter == 2 && s2_Filter == 1 && match(2, d_Filter)) || val(s3_Filter == 2 && s2_Filter == 1 && !match(2, d_Filter)) || val(s2_Filter == 2) || (exists d5_Filter1: D. val(s3_Filter == 1 && match(1, d5_Filter1))) || (exists d4_Filter1: D. val(s3_Filter == 1 && !match(1, d4_Filter1)))) && (val(!(s3_Filter == 2 && s2_Filter == 1 && match(2, d_Filter))) || X(1, d1, 2, d_Filter)) && (val(!(s3_Filter == 2 && s2_Filter == 1 && !match(2, d_Filter))) || X(1, d1, 1, d1)) && (val(d_Filter1 == d1) || val(!(s2_Filter == 2)) || X(s3_Filter, d_Filter, 1, d1)) && (forall d5_Filter: D. val(!(s3_Filter == 1 && match(1, d5_Filter))) || X(2, d5_Filter, s2_Filter, d_Filter1)) && (forall d4_Filter: D. val(!(s3_Filter == 1 && !match(1, d4_Filter))) || X(1, d1, s2_Filter, d_Filter1));\n"
    "\n"
    "init Y(1, d1, 1, d1);\n"
    ;
  pbes<> p = txt2pbes(text, false);
  pfnf_rewriter R;
  pbes_rewrite(p, R);
  BOOST_CHECK(pbes_system::detail::is_pfnf(p));

  detail::pbes_control_flow_algorithm algorithm;
  algorithm.run(p);
}

void test_source_dest2()
{
  std::string text =
    "sort D = struct d1 | d2 | d3;                                                \n"
    "                                                                             \n"
    "map match: Pos # D -> Bool;                                                  \n"
    "                                                                             \n"
    "pbes                                                                         \n"
    "                                                                             \n"
    "nu Y(s1: Pos, e1: D, s2: Pos, e2: D) =                                       \n"
    "    forall d: D.                                                             \n"
    "    (                                                                        \n"
    "        (val(s1 == 1 && match(1, d1))                  => X(2, d1, s2, e2))  \n"
    "     && (val(s1 == 1 && !match(1, d1))                 => X(1, d1, s2, e2))  \n"
    "     && (val(s1 == 2 && s2 == 1 && match(2, e1))       => Y(1, d1, 2, e1))   \n"
    "     && (val(s1 == 2 && s2 == 1 && !match(2, e1))      => Y(1, d1, 1, d1))   \n"
    "     && (val(s2 == 2)                                  => Y(s1, e1, 1, d1))  \n"
    "     && (val(s1 == 1 && match(1, d))                   => Y(2, d, s2, e2))   \n"
    "     && (val(s1 == 1 && !match(1, d))                  => Y(1, d1, s2, e2))  \n"
    "    );                                                                       \n"
    "                                                                             \n"
    "mu X(s1: Pos, e1: D, s2: Pos, e2: D) =                                       \n"
    "    forall d: D.                                                             \n"
    "    (                                                                        \n"
    "        (val(s1 == 2 && s2 == 1 && match(2, e1))       => X(1, d1, 2, e1))   \n"
    "     && (val(s1 == 2 && s2 == 1 && !match(2, e1))      => X(1, d1, 1, d1))   \n"
    "     && (val(e2 != d1 && s2 == 2)                      => X(s1, e1, 1, d1))  \n"
    "     && (val(s1 == 1 && match(1, d))                   => X(2, d, s2, e2))   \n"
    "     && (val(s1 == 1 && !match(1, d))                  => X(1, d1, s2, e2))  \n"
    "    );                                                                       \n"
    "                                                                             \n"
    "init Y(1, d1, 1, d1);                                                        \n"
    ;
  pbes<> p = txt2pbes(text, false);
//  pfnf_rewriter R;
//  pbes_rewrite(p, R);
  BOOST_CHECK(pbes_system::detail::is_pfnf(p));

  detail::pbes_control_flow_algorithm algorithm;
  algorithm.run(p);
}

void test_simplify()
{
  std::string ptext =
    "pbes                                                  \n"
    "  nu X0(b: Bool, n: Nat)  = val(b) => val(n == 0);    \n"
    "  nu X1(b: Bool, c: Bool) = val(b != c);              \n"
    "  nu X2(b: Bool, c: Bool) = !(val(b) => val(b != c)); \n"
    "  nu X3(n:Nat)            = !(val(!(n == 1)));        \n"
    "init X0(true, 0);                                     \n"
    ;
  pbes<> p = txt2pbes(ptext, false);

  std::string qtext =
    "pbes                                                  \n"
    "  nu X0(b: Bool, n: Nat) = !val(b) || val(n == 0);    \n"
    "  nu X1(b: Bool, c: Bool) = !val(b == c);             \n"
    "  nu X2(b: Bool, c: Bool) = val(b) || val(b == c);    \n"
    "  nu X3(n:Nat)            = val(n == 1);              \n"
    "init X0(true, 0);                                     \n"
    ;
  pbes<> q = txt2pbes(qtext, false);
  detail::pbes_control_flow_algorithm algorithm;

  for (std::size_t i = 0; i < p.equations().size(); i++)
  {
    pbes_expression x = algorithm.simplify(p.equations()[i].formula());
    pbes_expression y = q.equations()[i].formula();
    std::cout << "--- simplify case " << i << std::endl;
    std::cout << " x = " << pbes_system::pp(x) << std::endl;
    std::cout << " y = " << pbes_system::pp(y) << std::endl;
    BOOST_CHECK(x == y);
  }
}

int test_main(int argc, char** argv)
{
  test_control_flow();
  test_simplify();
  test_source_dest1();
  test_source_dest2();

  return 0;
}
