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

#include <boost/test/included/unit_test_framework.hpp>
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/pbes/normalize.h"
#include "mcrl2/pbes/rewrite.h"
#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/pbes/txt2pbes.h"
#include "mcrl2/pbes/pbespgsolve.h"
#include "mcrl2/pbes/pbes_solver_test.h"
#include "mcrl2/pbes/detail/is_pfnf.h"
#include "mcrl2/pbes/detail/control_flow.h"
#include "mcrl2/utilities/logger.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;

bool solve_pbes(const pbes<>& p)
{
  std::cout << "<solve_pbes>" << std::endl;
  std::cout << pbes_system::pp(p) << std::endl;
  bool result;
  if (is_normalized(p))
  {
    pbes<> q = p;
    result = pbes2_bool_test(q);
  }
  else
  {
    std::cout << "<normalizing>" << std::endl;
    pbes<> q = p;
    pbes_system::normalize(q);
    std::cout << pbes_system::pp(q) << std::endl;
    result = pbes2_bool_test(q);
  }
  std::cout << "<result>" << std::boolalpha << result << std::endl;
  return result;
}

BOOST_AUTO_TEST_CASE(test_control_flow1)
{
  std::string text =
    "pbes nu X(n: Nat)  = X(0) || X(n) || X(2);   \n"
    "     nu Y(m: Nat)  = Y(m) || X(1) || X(m);   \n"
    "     nu Z(p: Nat) = (forall b:Bool. true) && (true => (X(p) || X(1))) && val(true) && Y(p); \n"
    "init X(0);                                  \n"
    ;
  pbes<> p = txt2pbes(text, false);
  BOOST_CHECK(pbes_system::detail::is_pfnf(p));

  detail::control_flow_algorithm algorithm;
  algorithm.run(p, true, false, true);
}

BOOST_AUTO_TEST_CASE(test_control_flow)
{
  std::string text =
    "pbes nu X(n: Nat)  = X(2) || X(n) || Y(1); \n"
    "     nu Y(m: Nat)  = X(m) || Y(4);         \n"
    "init X(0);                                 \n"
    ;
  pbes<> p = txt2pbes(text, false);
  BOOST_CHECK(pbes_system::detail::is_pfnf(p));

  detail::control_flow_algorithm algorithm;
  algorithm.run(p, true, false, true);
}

BOOST_AUTO_TEST_CASE(test_control_flow2)
{
  std::string text =
    "pbes nu X(n: Nat, m:Nat) = (val(n == 1) => Y(n)) && (val(n == 2) => Y(m)); \n"
    "     nu Y(p: Nat)  = X(p,p);         \n"
    "init X(1,0); \n"
    ;
  pbes<> p = txt2pbes(text, false);
  BOOST_CHECK(pbes_system::detail::is_pfnf(p));

  detail::control_flow_algorithm algorithm;
  algorithm.run(p, true, false, true);
}

BOOST_AUTO_TEST_CASE(test_source_dest1)
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

  detail::control_flow_algorithm algorithm;
  algorithm.run(p, true, false, true);
}

BOOST_AUTO_TEST_CASE(test_running_example)
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
  BOOST_CHECK(pbes_system::detail::is_pfnf(p));

  detail::control_flow_algorithm algorithm;
  algorithm.run(p, true, false, true);
}

BOOST_AUTO_TEST_CASE(test_simplify)
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
  detail::control_flow_algorithm algorithm;

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

// found by random testing 7 Sep 2012
BOOST_AUTO_TEST_CASE(test_stategraph1)
{
  std::string text =
    "pbes nu X0 =                                                                          \n"
    "       true && X1 || true && (forall u: Nat. false && (X2(u > 0, 1) && X0 || false)); \n"
    "     mu X1 =                                                                          \n"
    "       true;                                                                          \n"
    "     nu X2(c: Bool, m: Nat) =                                                         \n"
    "       true && false;                                                                 \n"
    "                                                                                      \n"
    "init X0;                                                                              \n"
    ;
  pbes<> p = txt2pbes(text, true);
  bool answer1 = solve_pbes(p);
  detail::control_flow_algorithm algorithm;
  pbes<> q = algorithm.run(p, true, false, true);
  BOOST_CHECK(q.is_well_typed());
  bool answer2 = solve_pbes(q);
  BOOST_CHECK(answer1 == answer2);
}

// found by random testing 7 Sep 2012
BOOST_AUTO_TEST_CASE(test_stategraph2)
{
  std::string text =
    "pbes nu X0(m: Nat, c: Bool) =                         \n"
    "       true && (false || X1(0, true));                \n"
    "     mu X1(m: Nat, b: Bool) =                         \n"
    "       true && (X1(0, true) && false || X2 || false); \n"
    "     nu X2 =                                          \n"
    "       true;                                          \n"
    "                                                      \n"
    "init X0(0, true);                                     \n"
    ;
  pbes<> p = txt2pbes(text, true);
  bool answer1 = solve_pbes(p);
  detail::control_flow_algorithm algorithm;
  pbes<> q = algorithm.run(p, true, false, true);
  BOOST_CHECK(q.is_well_typed());
  bool answer2 = solve_pbes(q);
  BOOST_CHECK(answer1 == answer2);
}

#ifdef MCRL2_CONTROL_FLOW_TEST_ALL
// found by random testing 7 Sep 2012
BOOST_AUTO_TEST_CASE(test_stategraph3)
{
  std::string text =
    "pbes mu X0 =                                                                                           \n"
    "       (forall t: Nat. false) || (forall t: Nat. false) || (exists t: Nat. false || val(t > 1)) || X1; \n"
    "     nu X1 =                                                                                           \n"
    "       false;                                                                                          \n"
    "                                                                                                       \n"
    "init X0;                                                                                               \n"
    ;
  pbes<> p = txt2pbes(text, true);
  bool answer1 = solve_pbes(p);
  detail::control_flow_algorithm algorithm;
  pbes<> q = algorithm.run(p, true, false, true);
  BOOST_CHECK(q.is_well_typed());
  bool answer2 = solve_pbes(q);
  BOOST_CHECK(answer1 == answer2);
}

// found by random testing 7 Sep 2012
BOOST_AUTO_TEST_CASE(test_stategraph4)
{
  std::string text =
    "pbes                                                                                                                                                                                                                                                                                                                           \n"
    "mu X0(n:Nat, m:Nat) = ((val(m < 2)) && ((val(m > 0)) => ((!X0(m + 1, m + 1)) || (!X2(false, m > 1))))) => ((exists u:Nat.((val(u < 3)) || ((val(m < 2)) && (val(u == n))))) && (X1(n < 3, n > 1)));                                                                                                                            \n"
    "mu X1(b:Bool, c:Bool) = ((((!(exists u:Nat.((val(u < 3)) || (val(u < 2))))) => (forall u:Nat.((val(u < 3)) && (X1(u > 0, u < 3))))) && ((val(true)) || (!(val(b))))) && (forall u:Nat.((val(u < 3)) && (!(val(c)))))) && (forall v:Nat.((val(v < 3)) && (forall t:Nat.((val(t < 3)) && ((X0(0, v + 1)) || (X2(c, t < 2))))))); \n"
    "mu X2(b:Bool, c:Bool) = ((!X1(false, true)) => (X2(false, true))) && (((forall t:Nat.((val(t < 3)) && ((val(t < 2)) && (val(t > 0))))) => (!(X0(0, 1)))) => ((val(false)) && (val(true))));                                                                                                                                    \n"
    "                                                                                                                                                                                                                                                                                                                               \n"
    "init X0(0, 0);                                                                                                                                                                                                                                                                                                                 \n"
    ;
  pbes<> p = txt2pbes(text, true);
  bool answer1 = solve_pbes(p);
  detail::control_flow_algorithm algorithm;
  pbes<> q = algorithm.run(p, true, false, true);
  BOOST_CHECK(q.is_well_typed());
  bool answer2 = solve_pbes(q);
  BOOST_CHECK(answer1 == answer2);
}
#endif // MCRL2_CONTROL_FLOW_TEST_ALL

// found by random testing 7 Sep 2012
BOOST_AUTO_TEST_CASE(test_stategraph4)
{
  std::string text =
    "pbes mu X0 =                              \n"
    "       forall b: Bool. X2(true) || X1(b); \n"
    "     mu X1(c: Bool) =                     \n"
    "       val(c);                            \n"
    "     mu X2(d: Bool) =                     \n"
    "       X1(false);                         \n"
    "                                          \n"
    "init X0;                                  \n"
    ;
  pbes<> p = txt2pbes(text, true);
  bool answer1 = solve_pbes(p);
  detail::control_flow_algorithm algorithm;
  pbes<> q = algorithm.run(p, true, false, true);
  BOOST_CHECK(q.is_well_typed());
  bool answer2 = solve_pbes(q);
  BOOST_CHECK(answer1 == answer2);
}

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)
  log::mcrl2_logger::set_reporting_level(log::debug, "control_flow");
  return 0;
}
