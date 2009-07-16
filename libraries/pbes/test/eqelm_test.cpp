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
#include "mcrl2/core/messaging.h"
#include "mcrl2/pbes/eqelm.h"
#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/pbes/txt2pbes.h"
#include "mcrl2/pbes/detail/pbes_property_map.h"

#include "mcrl2/core/garbage_collection.h"

using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::pbes_system;

std::string t1 =
  "% Example provided by Frank Stappers          \n"
  "                                              \n"
  "pbes nu X(n0,n_P: Nat) =                      \n"
  "        val(n_P == n0) && X(n0 + 1, n_P + 1); \n"
  "                                              \n"
  " init X(0, 0);                                \n"
  ;
std::string x1 = "binding_variables = X(n_P: Nat)";

void test_pbes(const std::string& pbes_spec, std::string expected_result, bool compute_conditions, bool remove_equations = true)
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
    std::cerr << pp(pbes_to_aterm(q)) << std::endl;
  }

  pbes_system::detail::pbes_property_map info1(q);  
  pbes_system::detail::pbes_property_map info2(expected_result);
  std::string diff = info1.compare(info2);
  if (!diff.empty())
  {
    std::cerr << "\n------ FAILED TEST ------" << std::endl;
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

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT_DEBUG(argc, argv)

  test_pbes(t1 , x1 , false);

  return 0;
}
