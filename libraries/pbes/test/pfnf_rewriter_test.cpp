// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pfnf_rewriter.cpp
/// \brief Tests for pfnf rewriter.

#include <boost/test/minimal.hpp>
#include "mcrl2/pbes/parse.h"
#include "mcrl2/pbes/rewrite.h"
#include "mcrl2/pbes/rewriters/pfnf_rewriter.h"
#include "mcrl2/pbes/rewriters/simplify_rewriter.h"
#include "mcrl2/pbes/pbes_solver_test.h"
#include "mcrl2/pbes/txt2pbes.h"
#include "mcrl2/pbes/detail/pfnf_traverser.h"
#include "mcrl2/pbes/detail/is_pfnf.h"
#include "mcrl2/pbes/detail/pfnf_print.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;

const std::string VARIABLE_SPECIFICATION =
  "datavar         \n"
  "  b:  Bool;     \n"
  "  b1: Bool;     \n"
  "  b2: Bool;     \n"
  "  b3: Bool;     \n"
  "                \n"
  "  n:  Nat;      \n"
  "  n1: Nat;      \n"
  "  n2: Nat;      \n"
  "  n3: Nat;      \n"
  "                \n"
  "  p:  Pos;      \n"
  "  p1: Pos;      \n"
  "  p2: Pos;      \n"
  "  p3: Pos;      \n"
  "                \n"
  "predvar         \n"
  "  X;            \n"
  "  Y: Nat;       \n"
  "  W: Bool;      \n"
  "  Z: Bool, Nat; \n"
  "  X0;           \n"
  "  X1: Bool;     \n"
  "  X2: Nat, Nat; \n"
  "  X3: Bool, Nat;\n"
  "  X4: Nat, Bool;\n"
  ;

inline
pbes_system::pbes_expression expr(const std::string& text)
{
  return pbes_system::parse_pbes_expression(text, VARIABLE_SPECIFICATION);
}

void test_pfnf_expression(std::string s)
{
  pbes_system::detail::pfnf_traverser visitor;
  pbes_system::pbes_expression t1 = expr(s);
std::cerr << "t1 = " << pbes_system::pp(t1) << " " << t1 << std::endl;
  visitor.apply(t1);
  pbes_system::pbes_expression t2 = visitor.evaluate();
std::cerr << "t2 = " << pbes_system::pp(t2) << " " << t2 << std::endl;
  data::rewriter datar;
  pbes_system::simplify_data_rewriter<data::rewriter> R(datar);
  if (R(t1) != R(t2))
  {
    BOOST_CHECK(R(t1) == R(t2));
    std::cout << "--- failed test --- " << std::endl;
    std::cout << "t1    " << pbes_system::pp(t1) << std::endl;
    std::cout << "t2    " << pbes_system::pp(t2) << std::endl;
    std::cout << "R(t1) " << pbes_system::pp(R(t1)) << std::endl;
    std::cout << "R(t2) " << pbes_system::pp(R(t2)) << std::endl;
  }
}

void test_pfnf_visitor()
{
  test_pfnf_expression("forall m:Nat. false");
  test_pfnf_expression("X && (Y(3) || X)");
  //test_pfnf_expression("forall m:Nat. (Y(m) || exists n:Nat. Y(n))");
  //test_pfnf_expression("forall m:Nat. (Y(m) || exists m:Nat. Y(m))");
}

void test_pfnf(const std::string& pbes_spec)
{
  std::cerr << "--- test_pfnf ---" << std::endl;
  pbes p = txt2pbes(pbes_spec);
  std::cerr << "- before:" << std::endl;
  std::cerr << pbes_system::pp(p) << std::endl;
  pfnf_rewriter R;
  pbes_rewrite(p, R);
  std::cerr << "- after:" << std::endl;
  std::cerr << pbes_system::pp(p) << std::endl;
  BOOST_CHECK(pbes_system::detail::is_pfnf(p));
  std::cerr << "-----------------" << std::endl;
}

void test_pfnf_rewriter()
{
  using namespace pbes_system;

  std::string PFNF1 =
    "pbes                                                    \n"
    "nu X(b:Bool) = (val(b) && (X(b) || (X(!b) && X(!!b)))); \n"
    "                                                        \n"
    "init X(true);                                           \n"
    ;

  std::string PFNF2 =
    "pbes                                                        \n"
    "nu X(m:Nat) = (forall n:Nat. X(n)) && (forall j:Nat. X(j)); \n"
    "                                                            \n"
    "init X(0);                                                  \n"
    ;

  test_pfnf(PFNF1);
  test_pfnf(PFNF2);

  pfnf_rewriter R;
  pbes_expression x = parse_pbes_expression("val(n1 > 3) && forall b: Bool. forall n: Nat. val(n > 3) || exists n:Nat. val(n > 5)", VARIABLE_SPECIFICATION);
  pbes_expression y = R(x);
}

void test_pfnf_rewriter2(const std::string& text)
{
  pbes p = txt2pbes(text);
  std::cout << "\ntest_pfnf_rewriter2\n" << std::endl;
  std::cout << "--- before ---\n";
  std::cout << pbes_system::pp(p) << std::endl;

  bool result1 = pbes2_bool_test(p);

  pfnf_rewriter R;
  pbes_rewrite(p, R);

  std::cout << "--- after ---\n";
  std::cout << pbes_system::pp(p) << std::endl;

  BOOST_CHECK(p.is_well_typed());
  bool result2 = pbes2_bool_test(p);
  BOOST_CHECK(result1 == result2);
}

void test_pfnf_rewriter2()
{
  // problematic case found by random tests 14-1-2011
  std::string text =
    "pbes nu X =                                                  \n"
    "       (exists v: Nat. true) && (forall v: Nat. val(v < 3)); \n"
    "                                                             \n"
    "init X;                                                      \n"
    ;
  test_pfnf_rewriter2(text);

  text =
    "pbes                                                                                   \n"
    "nu X = (exists v: Nat. (val(v > 4) || Y(v))) && (forall v: Nat. (val(v < 3) && Y(v))); \n"
    "mu Y(n: Nat) = true;                                                                   \n"
    "                                                                                       \n"
    "init X;                                                                                \n"
    ;
//  test_pfnf_rewriter2(text);

  // problematic case found by random tests 15-1-2011
  text =
    "pbes nu X0(m: Nat) =                                                                                                                                                                  \n"
    "       ((false && (forall u: Nat. false)) && false && (forall v: Nat. val(!(v < 3)) && (forall u: Nat. val(!(u < 3)) && val(!(v == u))))) || (exists u: Nat. val(!(u < 3)) || false); \n"
    "     mu X1 =                                                                                                                                                                          \n"
    "       true;                                                                                                                                                                          \n"
    "     mu X2 =                                                                                                                                                                          \n"
    "       true && false;                                                                                                                                                                 \n"
    "                                                                                                                                                                                      \n"
    "init X0(0);                                                                                                                                                                           \n"
    ;
  test_pfnf_rewriter2(text);
}

void test_is_pfnf()
{
  std::string text =
    "pbes nu X(n: Nat) = X(0) || X(1) || X(2);   \n"
    "     nu Y(n: Nat) = (Y(0) || X(1)) && X(2); \n"
    "     nu Z(n: Nat) = true => (X(0) || X(1)); \n"
    "     nu X3(n: Nat) = true;                  \n"
    "     nu X4(n: Nat) = val(true);             \n"
    "     nu X5(n: Nat) = (true => (X(0) || X(1))) && val(true) && X(0); \n"
    "     nu X6(n: Nat) = (forall b:Bool. true) && (true => (X(0) || X(1))) && val(true) && X(0); \n"
    "     nu X7(n: Nat) = false;                 \n"
    "     nu X8(n: Nat) = val(false);            \n"
    "     nu X9(n: Nat) = X(0);                  \n"
    "     nu X10(n:Nat) = (X(0) || X(1)) && X(0);\n"
    "     nu X11(b: Bool) = val(b) && (X11(b) || X11(!b)) && (X11(b) || X11(!!b)); \n"
    "init X(0);                                  \n"
    ;
  pbes p = txt2pbes(text, false);
  pbes_expression x;

  x = p.equations()[0].formula();
  BOOST_CHECK(pbes_system::detail::is_pfnf_or(x));
  BOOST_CHECK(pbes_system::detail::is_pfnf_imp(x));

  x = p.equations()[1].formula();
  BOOST_CHECK(!pbes_system::detail::is_pfnf_or(x));
  BOOST_CHECK(!pbes_system::detail::is_pfnf_imp(x));
  BOOST_CHECK(pbes_system::detail::is_pfnf(x));

  x = p.equations()[2].formula();
  BOOST_CHECK(pbes_system::detail::is_pfnf_imp(x));
  std::vector<pbes_expression> v = pbes_system::detail::pfnf_implications(x);
  BOOST_CHECK(v.size() == 1);
  BOOST_CHECK(pbes_system::pp(v[0]) == "val(true) => X(0) || X(1)");

  x = p.equations()[3].formula();
  BOOST_CHECK(pbes_system::detail::is_pfnf_or(x));
  BOOST_CHECK(pbes_system::detail::is_pfnf_imp(x));

  x = p.equations()[4].formula();
  BOOST_CHECK(pbes_system::detail::is_pfnf_or(x));
  BOOST_CHECK(pbes_system::detail::is_pfnf_imp(x));

  x = p.equations()[5].formula();
  BOOST_CHECK(pbes_system::detail::is_pfnf_inner_and(x));

  x = p.equations()[6].formula();
  BOOST_CHECK(pbes_system::detail::is_pfnf_outer_and(x));

  x = p.equations()[7].formula();
  BOOST_CHECK(pbes_system::detail::is_pfnf_outer_and(x));
  BOOST_CHECK(pbes_system::detail::is_pfnf_inner_and(x));

  x = p.equations()[8].formula();
  BOOST_CHECK(pbes_system::detail::is_pfnf_outer_and(x));
  BOOST_CHECK(pbes_system::detail::is_pfnf_inner_and(x));

  x = p.equations()[9].formula();
  BOOST_CHECK(pbes_system::detail::is_pfnf_or(x));
  BOOST_CHECK(pbes_system::detail::is_pfnf_imp(x));
  BOOST_CHECK(pbes_system::detail::is_pfnf_inner_and(x));
  BOOST_CHECK(pbes_system::detail::is_pfnf_outer_and(x));

  x = p.equations()[10].formula();
  BOOST_CHECK(!pbes_system::detail::is_pfnf_or(x));
  BOOST_CHECK(!pbes_system::detail::is_pfnf_imp(x));
  BOOST_CHECK(pbes_system::detail::is_pfnf_inner_and(x));
  BOOST_CHECK(pbes_system::detail::is_pfnf_outer_and(x));

  x = p.equations()[11].formula();
  BOOST_CHECK(pbes_system::detail::is_pfnf(x));
  BOOST_CHECK(pbes_system::detail::is_pfnf_outer_and(x));
  BOOST_CHECK(pbes_system::detail::is_pfnf_expression(x));

  BOOST_CHECK(pbes_system::detail::is_pfnf(p));

  text =
    "pbes nu X0(m: Nat) =                                                            \n"
    "  exists u2: Nat. forall v: Nat. forall u1: Nat. forall u: Nat. val(!(u2 < 3)); \n"
    "mu X1 =                                                                         \n"
    "  true;                                                                         \n"
    "mu X2 =                                                                         \n"
    "  false;                                                                        \n"
    "                                                                                \n"
    "init X0(0);                                                                     \n"
    ;
  p = txt2pbes(text, false);
  BOOST_CHECK(pbes_system::detail::is_pfnf(p));

  text =
    "pbes nu X0(m: Nat) =                                                            \n"
    "  val(false);                                                                   \n"
    "                                                                                \n"
    "init X0(0);                                                                     \n"
    ;
  p = txt2pbes(text, false);
  BOOST_CHECK(pbes_system::detail::is_pfnf(p));

  text =
    "pbes nu X(b: Bool) =                                  \n"
    "       val(b) && (X(b) || X(!b)) && (X(b) || X(!!b)); \n"
    "                                                      \n"
    "init X(true);                                         \n"
    ;
  p = txt2pbes(text, false);
  BOOST_CHECK(pbes_system::detail::is_pfnf(p));
}

void test_pfnf_print()
{
  std::string text =
    "pbes nu X(n: Nat) = X(0) || X(1) || X(2);   \n"
    "     nu Y(n: Nat) = forall c:Bool. forall b:Bool. (X(0) || X(1) && X(0)) && (val(n > 0) => (X(2) || X(3))); \n"
    "init X(0);                                  \n"
    ;
  pbes p = txt2pbes(text);
  pfnf_rewriter R;
  pbes_rewrite(p, R);
  std::cout << "--- pfnf print ---" << std::endl;
  std::cout << pp(p) << std::endl;
  std::cout << "---" << std::endl;
  std::cout << pfnf_pp(p) << std::endl;
  BOOST_CHECK(pbes_system::detail::is_pfnf(p));
}

void test_pfnf_rewriter3()
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
  pbes p = txt2pbes(text, true);
  BOOST_CHECK(!pbes_system::detail::is_pfnf(p));
  pfnf_rewriter R;
  pbes_system::pbes_rewrite(p, R);
  BOOST_CHECK(pbes_system::detail::is_pfnf(p));
}

int test_main(int argc, char** argv)
{
  test_pfnf_visitor();
  test_pfnf_rewriter();
  test_pfnf_rewriter2();
  test_pfnf_rewriter3();
  test_is_pfnf();
  test_pfnf_print();

  return 0;
}
