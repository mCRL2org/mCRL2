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
    options.add_delta=false;  // Do not add delta summands, required for timed linearisation. 

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

BOOST_AUTO_TEST_CASE(Check_single_timed_process)
{
  const std::string spec =
    "act a;\n"
    "init a@2.delta;\n"
    ;

  run_linearisation_test_case(spec,true);
}

BOOST_AUTO_TEST_CASE(Check_parallel_timed_processes)
{
  const std::string spec =
    "act a;\n"
    "init a@2.delta || a@4.delta;\n"
    ;

  run_linearisation_test_case(spec,true);
}

BOOST_AUTO_TEST_CASE(Check_parallel_timed_processes_reversed)
{
  const std::string spec =
   "act a;\n"
   "init a@5.delta || a@3.delta;\n"
   ;

  run_linearisation_test_case(spec,true);
}

BOOST_AUTO_TEST_CASE(Check_parallel_deltas)
{
  const std::string spec =
    "init delta@2 || delta@4;\n"
    ;

  run_linearisation_test_case(spec,true);
}


boost::unit_test::test_suite* init_unit_test_suite(int, char*[])
{
  return nullptr;
}

