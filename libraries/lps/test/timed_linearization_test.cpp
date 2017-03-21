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

data::data_expression ultimate_delay(const stochastic_action_summand_vector& l)
{
  data::data_expression result=data::sort_real::real_("0");
  for(const stochastic_action_summand& s: l)
  {
    if (s.condition()!=data::sort_bool::false_() && s.has_time())
    {
      result=data::sort_real::maximum(result, s.multi_action().time());
    }
  }
  return result;
}

data::data_expression ultimate_delay(const deadlock_summand_vector& l)
{
  data::data_expression result=data::sort_real::real_("0");
  for(const deadlock_summand& s: l)
  {
    BOOST_CHECK(s.has_time());
    if (s.condition()!=data::sort_bool::false_())
    {
      result=data::sort_real::maximum(result, s.deadlock().time());
    }
  }
  return result;
}

void run_linearisation_instance(const std::string& spec, 
                                const t_lin_options& options, 
                                const bool expect_success,
                                const data::data_expression max_expected_action_ultimate_delay,
                                const bool check_max_expected_deadlock_ultimate_delay,
                                const data::data_expression max_expected_deadlock_ultimate_delay)
{
  if (expect_success)
  {
    lps::stochastic_specification s=linearise(spec, options);
    data::rewriter r(s.data());
    data::data_expression max_action_ultimate_delay=r(ultimate_delay(s.process().action_summands()));
    if (r(data::equal_to(ultimate_delay(s.process().action_summands()),max_expected_action_ultimate_delay))!=data::sort_bool::true_())
    {
      std::clog << "Expected action time does not match:\n";
      std::clog << "Action time " << max_action_ultimate_delay << "\n";
      std::clog << "Expected maximum delay " << max_expected_action_ultimate_delay << "\n";
      BOOST_CHECK(r(data::equal_to(ultimate_delay(s.process().action_summands()),max_expected_action_ultimate_delay))==data::sort_bool::true_());
    }
    if (check_max_expected_deadlock_ultimate_delay)
    {
      data::data_expression max_deadlock_ultimate_delay=r(ultimate_delay(s.process().deadlock_summands()));
      if (r(data::equal_to(ultimate_delay(s.process().deadlock_summands()),max_expected_deadlock_ultimate_delay))!=data::sort_bool::true_())
      {
        std::clog << "Expected deadlock time does not match:\n";
        std::clog << "Deadlock time " << ultimate_delay(s.process().deadlock_summands()) << "\n";
        std::clog << "Expected maximum delay " << max_expected_deadlock_ultimate_delay << "\n";
        BOOST_CHECK(r(data::equal_to(ultimate_delay(s.process().deadlock_summands()),max_expected_deadlock_ultimate_delay))==data::sort_bool::true_());
      }
    }
  }
  else
  {
    BOOST_CHECK_THROW(linearise(spec, options), mcrl2::runtime_error);
  }
}

// The ultimate delays are the maximum of the ultimate delays over all actions resp. deadlocks
// not looking at the conditions of the actions or deadlocks. 
void run_linearisation_test_case(const std::string& spec, 
                                 const bool expect_success, 
                                 const size_t max_expected_action_ultimate_delay_, 
                                 const bool check_max_expected_deadlock_ultimate_delay,
                                 const size_t max_expected_deadlock_ultimate_delay_)
{
  // Set various rewrite strategies
  rewrite_strategy_vector rewrite_strategies = data::detail::get_test_rewrite_strategies(false);
  const data::data_expression max_expected_action_ultimate_delay=data::sort_real::real_(max_expected_action_ultimate_delay_); 
  const data::data_expression max_expected_deadlock_ultimate_delay=data::sort_real::real_(max_expected_deadlock_ultimate_delay_); 

  for (rewrite_strategy_vector::const_iterator i = rewrite_strategies.begin(); i != rewrite_strategies.end(); ++i)
  {
    std::clog << std::endl << "Testing with rewrite strategy " << *i << std::endl;
    std::clog << spec << "\n";

    t_lin_options options;
    options.ignore_time=false;  // Do not ignore time.

    options.rewrite_strategy=*i;

    std::clog << "  Default options" << std::endl;
    run_linearisation_instance(spec, options, expect_success,max_expected_action_ultimate_delay,check_max_expected_deadlock_ultimate_delay,max_expected_deadlock_ultimate_delay);

    std::clog << "  Linearisation method regular2" << std::endl;
    options.lin_method=lmRegular2;
    run_linearisation_instance(spec, options, expect_success,max_expected_action_ultimate_delay,check_max_expected_deadlock_ultimate_delay,max_expected_deadlock_ultimate_delay);

    std::clog << "  Linearisation method stack" << std::endl;
    options.lin_method=lmStack;
    run_linearisation_instance(spec, options, expect_success,max_expected_action_ultimate_delay,check_max_expected_deadlock_ultimate_delay,max_expected_deadlock_ultimate_delay);

    std::clog << "  Linearisation method stack; binary enabled" << std::endl;
    options.binary=true;
    run_linearisation_instance(spec, options, expect_success,max_expected_action_ultimate_delay,check_max_expected_deadlock_ultimate_delay,max_expected_deadlock_ultimate_delay);

    std::clog << "  Linearisation method regular; binary enabled" << std::endl;
    options.lin_method=lmRegular;
    run_linearisation_instance(spec, options, expect_success,max_expected_action_ultimate_delay,check_max_expected_deadlock_ultimate_delay,max_expected_deadlock_ultimate_delay);

    std::clog << "  Linearisation method regular; no intermediate clustering" << std::endl;
    options.binary=false; // reset binary
    options.no_intermediate_cluster=true;
    run_linearisation_instance(spec, options, expect_success,max_expected_action_ultimate_delay,check_max_expected_deadlock_ultimate_delay,max_expected_deadlock_ultimate_delay);
  }
}

BOOST_AUTO_TEST_CASE(Check_single_timed_process)
{
  const std::string spec =
    "act a;\n"
    "init a@2.delta@10;\n"
    ;

  run_linearisation_test_case(spec,true,2,true,10);
}

BOOST_AUTO_TEST_CASE(Check_parallel_timed_processes)
{
  const std::string spec =
    "act a;\n"
    "init a@2.delta@10 || a@4.delta@10;\n"
    ;

  run_linearisation_test_case(spec,true,4,true,10);
}

BOOST_AUTO_TEST_CASE(Check_parallel_timed_processes_with_the_same_time)
{
  const std::string spec =
    "act a;\n"
    "init a@2.delta@10 || a@2.delta@10;\n"
    ;

  run_linearisation_test_case(spec,true,2,true,10);
}

BOOST_AUTO_TEST_CASE(Check_parallel_timed_processes_reversed)
{
  const std::string spec =
   "act a;\n"
   "init a@5.delta@10 || a@3.delta@10;\n"
   ;

  run_linearisation_test_case(spec,true,5,true,10);
}

BOOST_AUTO_TEST_CASE(Check_parallel_deltas)
{
  const std::string spec =
    "init delta@2 || delta@4;\n"
    ;

  run_linearisation_test_case(spec,true,0,true,2);
}

BOOST_AUTO_TEST_CASE(Check_parallel_deltas_with_the_same_time)
{
  const std::string spec =
    "init delta@2 || delta@2;\n"
    ;

  run_linearisation_test_case(spec,true,0,true,2);
}

BOOST_AUTO_TEST_CASE(Check_parallel_action_and_delta_with_the_same_time)
{
  const std::string spec =
    "act a;\n"
    "init a@3.delta@10 || delta@3;";
    ;

  run_linearisation_test_case(spec,true,0,true,3);
}

BOOST_AUTO_TEST_CASE(Check_parallel_action_and_delta_with_different_time1)
{
  const std::string spec =
    "act a;\n"
    "init a@3.delta@10 || delta@4;";
    ;

  run_linearisation_test_case(spec,true,3,true,4);
}

BOOST_AUTO_TEST_CASE(Check_parallel_action_and_delta_with_different_time2)
{
  const std::string spec =
    "act a;\n"
    "init a@4.delta@10 || delta@3;";
    ;

  run_linearisation_test_case(spec,true,0,true,3);
}

BOOST_AUTO_TEST_CASE(Check_terminate)
{
  const std::string spec =
    "act a;\n"
    "init a@2;";
    ;

  run_linearisation_test_case(spec,true,2,false,0);
}

BOOST_AUTO_TEST_CASE(Check_terminate_and_parallelism)
{
  const std::string spec =
    "act a;\n"
    "init a@2||a@3;";
    ;

  run_linearisation_test_case(spec,true,3,false,0);
}

BOOST_AUTO_TEST_CASE(Check_terminate_and_synchrony)
{
  const std::string spec =
    "act a;\n"
    "init a@2||a@2;";
    ;

  run_linearisation_test_case(spec,true,2,false,0);
}

BOOST_AUTO_TEST_CASE(Check_terminate_and_parallelism_deadlock1)
{
  const std::string spec =
    "act a;\n"
    "init a@2||delta@3;";
    ;

  run_linearisation_test_case(spec,true,2,false,0);  // As it stands, cannot check the the deadlock. Should be "true,3".
}

BOOST_AUTO_TEST_CASE(Check_terminate_and_parallelism_deadlock2)
{
  const std::string spec =
    "act a;\n"
    "init a@4||delta@3;";
    ;

  run_linearisation_test_case(spec,true,0,false,0);
}

BOOST_AUTO_TEST_CASE(Check_terminate_and_synchrony_and_deadlock)
{
  const std::string spec =
    "act a;\n"
    "init a@2||delta@2;";
    ;

  run_linearisation_test_case(spec,true,0,false,0);
}


boost::unit_test::test_suite* init_unit_test_suite(int, char*[])
{
  return nullptr;
}

