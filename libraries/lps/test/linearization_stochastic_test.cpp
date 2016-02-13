// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file stochastic_linearization_test.cpp
/// \brief Add your file description here.

#include <boost/test/included/unit_test_framework.hpp>

#include <iostream>
#include <string>

#include "mcrl2/data/detail/rewrite_strategies.h"
#include "mcrl2/lps/linearise.h"
#include "mcrl2/utilities/logger.h"

using namespace mcrl2;
using namespace mcrl2::lps;

typedef data::rewriter::strategy rewrite_strategy;
typedef std::vector<rewrite_strategy> rewrite_strategy_vector;

void run_linearisation_instance(const std::string& spec, const t_lin_options& options, bool expect_success)
{
  if (expect_success)
  {
    lps::stochastic_specification s=linearise(spec, options);
    BOOST_CHECK(s != lps::stochastic_specification());
  }
  else
  {
    BOOST_CHECK_THROW(linearise(spec, options), mcrl2::runtime_error);
  }
}

void run_linearisation_test_case(const std::string& spec, const bool expect_success = true)
{
  // Set various rewrite strategies
  rewrite_strategy_vector rewrite_strategies = data::detail::get_test_rewrite_strategies(false);

  for (rewrite_strategy_vector::const_iterator i = rewrite_strategies.begin(); i != rewrite_strategies.end(); ++i)
  {
    std::clog << std::endl << "Testing with rewrite strategy " << *i << std::endl;

    t_lin_options options;
    options.rewrite_strategy=*i;

    std::clog << "  Default options" << std::endl;
    run_linearisation_instance(spec, options, expect_success);

    std::clog << "  Linearisation method regular2" << std::endl;
    options.lin_method=lmRegular2;
    run_linearisation_instance(spec, options, expect_success);

    std::clog << "  Linearisation method stack" << std::endl;
    options.lin_method=lmStack;
    run_linearisation_instance(spec, options, expect_success);

    std::clog << "  Linearisation method stack; binary enabled" << std::endl;
    options.binary=true;
    run_linearisation_instance(spec, options, expect_success);

    std::clog << "  Linearisation method regular; binary enabled" << std::endl;
    options.lin_method=lmRegular;
    run_linearisation_instance(spec, options, expect_success);

    std::clog << "  Linearisation method regular; no intermediate clustering" << std::endl;
    options.binary=false; // reset binary
    options.no_intermediate_cluster=true;
    run_linearisation_instance(spec, options, expect_success);
  }
}

BOOST_AUTO_TEST_CASE(Check_that_a_probability_distribution_works_well_in_combination_with_a_nonterminating_initial_process)
{
  const std::string spec =
    "act a:Bool;\n"
    "init dist x:Bool[1/2].a(x);\n";

  run_linearisation_test_case(spec,true);
}

BOOST_AUTO_TEST_CASE(Check_distribution_of_dist_over_plus)
{
  const std::string spec =
    "act a,b:Bool;\n"
    "init dist x:Bool[1/2].a(x).delta+dist y:Bool[1/2].a(y).delta;\n";

  run_linearisation_test_case(spec,true);
}

BOOST_AUTO_TEST_CASE(Check_distribution_of_dist_over_sum)
{
  const std::string spec =
    "act a:Bool#Bool;\n"
    "init dist x:Bool[1/2].sum y:Bool.a(x,y).delta;\n";

  run_linearisation_test_case(spec,true);
}


boost::unit_test::test_suite* init_unit_test_suite(int, char*[])
{
  return 0;
}

