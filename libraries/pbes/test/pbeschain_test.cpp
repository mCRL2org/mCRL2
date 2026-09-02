// Author(s): Jore Booy
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbeschain_test.cpp
/// \brief Test for the pbeschain tool.

#include "mcrl2/utilities/logger.h"
#define BOOST_TEST_MODULE pbeschain_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/pbes/algorithms.h"
#include "mcrl2/pbes/txt2pbes.h"
#include "mcrl2/pbes/tools/pbeschain.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;

static pbes run_pbeschain(const std::string& pbes_text, pbeschain_options options)
{
  pbes p = txt2pbes(pbes_text);
  algorithms::normalize(p);
  pbeschain_pbes_backward_substituter backward_substituter;
  backward_substituter.run(p, options);
  return p;
}

// The chain X(0) -> X(1) -> X(2) -> X(1) enters a loop that does not return to
// the initial pvi. Chaining with Gauss elimination must collapse the equation,
// after which back substitution removes it altogether.
BOOST_AUTO_TEST_CASE(test_pbeschain_indirect_loop_collapse)
{
  std::string pbes_text =
    "pbes nu X(n: Nat) = (val(n == 4) && X(0)) || (val(n == 0) && X(1)) || (val(n == 1) && X(2)) "
    "|| (val(n == 2) && X(1));                                                          \n"
    "init X(0);                                                                         \n";
  pbeschain_options options;
  options.srf_factor = 0.0; // N.B. pbeschain_options does not initialize srf_factor itself
  pbes p = run_pbeschain(pbes_text, options);
  BOOST_CHECK(p.equations().empty());
}

// A pvi that reoccurs directly in its own unfolding is replaced by true/false
// (direct Gauss elimination), after which the equation is removed.
BOOST_AUTO_TEST_CASE(test_pbeschain_direct_loop_collapse)
{
  std::string pbes_text =
    "pbes nu X(n: Nat) = val(n == 0) && X(0); \n"
    "init X(0);                               \n";
  pbeschain_options options;
  options.srf_factor = 0.0;
  pbes p = run_pbeschain(pbes_text, options);
  BOOST_CHECK(p.equations().empty());
}

// With Gauss elimination disabled no pvi may be replaced by true/false. The
// equation must be kept and the substitution must terminate. A hang of this
// test indicates that the chain ping-pongs between pvi instead of stopping.
BOOST_AUTO_TEST_CASE(test_pbeschain_no_gauss_elimination)
{
  std::string pbes_text =
    "pbes nu X(n: Nat) = (val(n == 4) && X(0)) || (val(n == 0) && X(1)) || (val(n == 1) && X(2)) "
    "|| (val(n == 2) && X(1));                                                          \n"
    "init X(0);                                                                         \n";
  pbeschain_options options;
  options.srf_factor = 0.0;
  options.disable_gauss_elimination = true;
  mcrl2::log::logger::set_reporting_level(log::debug);
  pbes p = run_pbeschain(pbes_text, options);
  BOOST_CHECK(!p.equations().empty());
  BOOST_CHECK(!find_propositional_variable_instantiations(p.equations().front().formula()).empty());
}
