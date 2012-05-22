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
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/core/garbage_collection.h"
#include "mcrl2/pbes/parse.h"
#include "mcrl2/pbes/rewrite.h"
#include "mcrl2/pbes/pbes_solver_test.h"
#include "mcrl2/pbes/txt2pbes.h"
#include "mcrl2/pbes/detail/pfnf_traverser.h"
#include "mcrl2/pbes/detail/is_pfnf.h"

#define MCRL2_USE_PBESPGSOLVE
// N.B. The test fails if this flag is not set, due to a problem in pbes2bool.

#ifdef MCRL2_USE_PBESPGSOLVE
#include "mcrl2/pbes/pbespgsolve.h"
#endif

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
  visitor(t1);
  pbes_system::pbes_expression t2 = visitor.evaluate();
std::cerr << "t2 = " << pbes_system::pp(t2) << " " << t2 << std::endl;
  data::rewriter datar;
  pbes_system::simplifying_rewriter<pbes_system::pbes_expression, data::rewriter> R(datar);
  if (R(t1) != R(t2))
  {
    BOOST_CHECK(R(t1) == R(t2));
    std::cout << "--- failed test --- " << std::endl;
    std::cout << "t1    " << pbes_system::pp(t1) << std::endl;
    std::cout << "t2    " << pbes_system::pp(t2) << std::endl;
    std::cout << "R(t1) " << pbes_system::pp(R(t1)) << std::endl;
    std::cout << "R(t2) " << pbes_system::pp(R(t2)) << std::endl;
  }
  core::garbage_collect();
}

void test_pfnf_visitor()
{
  test_pfnf_expression("forall m:Nat. false");
  test_pfnf_expression("X && (Y(3) || X)");
  //test_pfnf_expression("forall m:Nat. (Y(m) || exists n:Nat. Y(n))");
  //test_pfnf_expression("forall m:Nat. (Y(m) || exists m:Nat. Y(m))");
  core::garbage_collect();
}

void test_pfnf(const std::string& pbes_spec)
{
  std::cerr << "--- test_pfnf ---" << std::endl;
  pbes<> p = txt2pbes(pbes_spec);
  std::cerr << "- before:" << std::endl;
  std::cerr << pbes_system::pp(p) << std::endl;
  pfnf_rewriter R;
  pbes_rewrite(p, R);
  std::cerr << "- after:" << std::endl;
  std::cerr << pbes_system::pp(p) << std::endl;
  std::cerr << "-----------------" << std::endl;
  core::garbage_collect();
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
  core::garbage_collect();
}

void test_pfnf_rewriter2(const std::string& text)
{
  pbes<> p = txt2pbes(text);
  std::cout << "\ntest_pfnf_rewriter2\n" << std::endl;
  std::cout << "--- before ---\n";
  std::cout << pbes_system::pp(p) << std::endl;

#ifdef MCRL2_USE_PBESPGSOLVE
  bool result1 = pbespgsolve(p);
#else
  bool result1 = pbes2_bool_test(p);
#endif
  pfnf_rewriter R;
  pbes_rewrite(p, R);

  std::cout << "--- after ---\n";
  std::cout << pbes_system::pp(p) << std::endl;

  BOOST_CHECK(p.is_well_typed());
#ifdef MCRL2_USE_PBESPGSOLVE
  bool result2 = pbespgsolve(p);
#else
  bool result2 = pbes2_bool_test(p);
#endif
  BOOST_CHECK(result1 == result2);
  core::garbage_collect();
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
  test_pfnf_rewriter2(text);
  core::garbage_collect();

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
    "     nu Y(n: Nat) = Y(0) || X(1) && X(2);   \n"
    "     nu Z(n: Nat) = true => (X(0) || X(1)); \n"
    "     nu X1(n: Nat) = true;                  \n"
    "     nu X2(n: Nat) = val(true);             \n"
    "     nu X3(n: Nat) = (true => (X(0) || X(1))) && val(true) && X(0); \n"
    "     nu X4(n: Nat) = (forall b:Bool. true) && (true => (X(0) || X(1))) && val(true) && X(0); \n"
    "init X(0);                                  \n"
    ;
  pbes<> p = txt2pbes(text, false);
  pbes_expression x;

  x = p.equations()[0].formula();
  BOOST_CHECK(pbes_system::detail::is_pfnf_or(x));
  BOOST_CHECK(pbes_system::detail::is_pfnf_imp(x));

  x = p.equations()[1].formula();
  BOOST_CHECK(!pbes_system::detail::is_pfnf_or(x));
  BOOST_CHECK(!pbes_system::detail::is_pfnf_imp(x));
  BOOST_CHECK(!pbes_system::detail::is_pfnf(x));
  std::vector<pbes_expression> v = pbes_system::detail::pfnf_implications(x);
  BOOST_CHECK(v.size() == 2);
  BOOST_CHECK(pbes_system::pp(v[0]) == "Y(0) || X(1)");
  BOOST_CHECK(pbes_system::pp(v[1]) == "X(2)");
  std::cout << "v[0] = " << pbes_system::pp(v[0]) << std::endl;
  std::cout << "v[1] = " << pbes_system::pp(v[1]) << std::endl;

  x = p.equations()[2].formula();
  BOOST_CHECK(pbes_system::detail::is_pfnf_imp(x));

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

  BOOST_CHECK(!pbes_system::detail::is_pfnf(p));
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT_DEBUG(argc, argv)

  test_pfnf_visitor();
  test_pfnf_rewriter();
  test_pfnf_rewriter2();
  test_is_pfnf();

  return 0;
}
