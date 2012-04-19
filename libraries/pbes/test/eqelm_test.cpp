// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file constelm_test.cpp
/// \brief Add your file description here.

// #define MCRL2_PBES_CONSTELM_DEBUG

#include <string>
#include <boost/foreach.hpp>
#include <boost/test/minimal.hpp>
#include "mcrl2/utilities/logger.h"
#include "mcrl2/pbes/eqelm.h"
#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/pbes/txt2pbes.h"
#include "mcrl2/pbes/detail/pbes_property_map.h"
#include "mcrl2/pbes/pbes_solver_test.h"
#include "mcrl2/pbes/pbespgsolve.h"
#include "mcrl2/atermpp/aterm_init.h"

#include "mcrl2/core/garbage_collection.h"

using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::pbes_system;

// Example provided by Frank Stappers
std::string t1 =
  "pbes nu X(m, n: Nat) =                  \n"
  "        val(n == m) && X(m + 1, n + 1); \n"
  "                                        \n"
  " init X(0, 0);                          \n"
  ;
std::string x1 = "binding_variables = X(m: Nat)";

// Example provided by Tim Willemse.
// The parameters n and m are not equivalent, and thus should not
// be removed.
std::string t2 =
  "pbes nu X(n,m: Nat) =          \n"
  "       forall p: Nat. X(m, p); \n"
  "                               \n"
  "init X(0, 0);                  \n"
  ;
std::string x2 = "binding_variables = X(n,m: Nat)";

void test_pbes(const std::string& pbes_spec,
               std::string expected_result,
               bool /* compute_conditions */,
               bool /* remove_equations = true */,
               std::string msg = "")
{
  typedef simplifying_rewriter<pbes_expression, data::rewriter> my_pbes_rewriter;

  pbes<> p = txt2pbes(pbes_spec);
  pbes<> q = p;

  // data rewriter
  data::rewriter datar(q.data());

  // pbes rewriter
  my_pbes_rewriter pbesr(datar);

  // constelm algorithm
  pbes_eqelm_algorithm<pbes_expression, data::rewriter, my_pbes_rewriter> algorithm(datar, pbesr);

  // run the algorithm
  algorithm.run(q);
  BOOST_CHECK(q.is_well_typed());
  if (!q.is_well_typed())
  {
    std::cerr << pbes_system::pp(q) << std::endl;
  }

  pbes_system::detail::pbes_property_map info1(q);
  pbes_system::detail::pbes_property_map info2(expected_result);
  std::string diff = info1.compare(info2);
  if (!diff.empty())
  {
    std::cerr << "\n------ FAILED TEST ------ " << msg << std::endl;
    std::cerr << "--- expected result" << std::endl;
    std::cerr << expected_result << std::endl;
    std::cerr << "--- found result" << std::endl;
    std::cerr << info1.to_string() << std::endl;
    std::cerr << "--- differences" << std::endl;
    std::cerr << diff << std::endl;
  }
  BOOST_CHECK(diff.empty());

  core::garbage_collect();
}

void test_eqelm(const std::string& pbes_spec)
{
  typedef simplifying_rewriter<pbes_expression, data::rewriter> my_pbes_rewriter;

  pbes<> p = txt2pbes(pbes_spec);
  pbes<> q = p;

  data::rewriter datar(q.data());
  my_pbes_rewriter pbesr(datar);
  pbes_eqelm_algorithm<pbes_expression, data::rewriter, my_pbes_rewriter> algorithm(datar, pbesr);
  algorithm.run(q);

  bool result1 = pbes2_bool_test(p);
  bool result2 = pbes2_bool_test(q);

  BOOST_CHECK(result1 == result2);
}

std::string random1 =
  "pbes mu X0(b: Bool) =                            \n"
  "       (X1(0, 0) || false) && true;              \n"
  "     mu X1(n,m: Nat) =                           \n"
  "       (X1(m + 1, 0) || false) || val(!(n < 2)); \n"
  "                                                 \n"
  "init X0(true);                                   \n"
  ;

int test_main(int argc, char** argv)
{
  ATinit();

  bool compute_conditions = false;
  bool remove_equations = true;
  test_pbes(t1 , x1, compute_conditions, remove_equations, "test 1");
  test_pbes(t2 , x2, compute_conditions, remove_equations, "test 2");
  test_eqelm(random1);

  return 0;
}
