// Author(s): Wieger Wesselink, Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file linearization_test.cpp
/// \brief Add your file description here.

#include <boost/test/included/unit_test_framework.hpp>

#ifndef MCRL2_SKIP_LONG_TESTS

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
    lps::specification s=remove_stochastic_operators(linearise(spec, options));
    BOOST_CHECK(s != lps::specification());
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

BOOST_AUTO_TEST_CASE(The_unreachability_of_tau_is_not_properly_recognized)
{
  const std::string spec =
     "init (true -> delta <> delta) . tau;";

  run_linearisation_test_case(spec,true);
}

BOOST_AUTO_TEST_CASE(Type_checking_of_function_can_be_problematic)
{
  const std::string spec =
     "sort  State = struct S;\n"
     "proc X = ((lambda x: Nat. S)(3) == S)->tau.X;\n"
     "init X;\n";

  run_linearisation_test_case(spec,true);
}

BOOST_AUTO_TEST_CASE(Check_whether_the_sum_variable_will_not_get_the_same_name_as_the_newly_introduced_process_parameter)
{
  const std::string spec =
     "act  base ;\n"
     "     exponent: Real;\n"
     "proc Test_exponentation =\n"
     "       sum r: Real. base . exponent(r).delta ;\n"
     "\n"
     "init Test_exponentation+delta;\n";

  run_linearisation_test_case(spec,true);
}

BOOST_AUTO_TEST_CASE(Check_whether_the_sum_variable_will_not_get_the_same_name_as_the_newly_introduced_process_parameter2)
{
  const std::string spec =
     "act\n"
     "  a,c,b,d;\n"
     "\n"
     "proc\n"
     "  P = b;\n"
     "  Q = (((tau) . (sum b1: Bool . (sum b2: Bool . (R)))) . (tau)) + (((delta) . (tau)) . (R));\n"
     "  R = ((true) -> (a)) + ((true) -> (sum b1: Bool . ((d) + ((d) + (a)))) <> ((d) + (a)));\n"
     "\n"
     "init\n"
     "  hide({b}, ((R) || (Q)) || (P));\n";

  run_linearisation_test_case(spec,true);
} 

BOOST_AUTO_TEST_CASE(linearisation_of_the_enclosed_spec_caused_a_name_conflict_with_the_option_lstack)
{
  const std::string spec =
     "act\n"
     "  c;\n"
     "\n"
     "proc\n"
     "  Q = sum b1: Bool . R;\n"
     "  R = sum b1: Bool . c.delta;\n"
     "\n"
     "init Q;\n";

  run_linearisation_test_case(spec,true);
} 

#else // ndef MCRL2_SKIP_LONG_TESTS

BOOST_AUTO_TEST_CASE(skip_linearization_test)
{
}

#endif // ndef MCRL2_SKIP_LONG_TESTS

boost::unit_test::test_suite* init_unit_test_suite(int, char*[])
{
  return 0;
}

