// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file linearization_test.cpp
/// \brief A test of the linearizer, comparing the result with the expected state space modulo strong bisimulation. 

#define BOOST_TEST_MODULE linearization_instantiation_compare_probabilistic_test

#include <boost/test/included/unit_test.hpp>

#include "mcrl2/utilities/test_utilities.h"

#include "mcrl2/data/detail/rewrite_strategies.h"

#include "mcrl2/lps/linearise.h"
#include "mcrl2/lts/detail/liblts_pbisim_grv.h"
#include "mcrl2/lts/state_space_generator.h"
#include "mcrl2/lts/stochastic_lts_builder.h"

using namespace mcrl2;
using namespace mcrl2::lps;

using rewrite_strategy = data::rewriter::strategy;
using rewrite_strategy_vector = std::vector<rewrite_strategy>;

template <class LTS_TYPE>
inline
LTS_TYPE translate_lps_to_lts(const lps::stochastic_specification& specification)
{
  lps::explorer_options options;
  options.trace_prefix = "linearization_instantiation_compare_test";
  options.search_strategy = lps::es_breadth;
  options.save_at_end = true;
  const std::string& output_filename = utilities::temporary_filename("linearization_instantiation_probabilistic_compare_test_file");

  LTS_TYPE result;
  data::rewriter rewr = lps::construct_rewriter(specification, options.rewrite_strategy, options.remove_unused_rewrite_rules);
  lps::explorer<true, false, lps::stochastic_specification> explorer(specification, options, rewr);
  lts::state_space_generator<true, false, lps::stochastic_specification> generator(specification, options, explorer);
  auto builder = create_stochastic_lts_builder(specification, options, result.type());
  generator.explore(*builder);
  builder->save(output_filename);

  result.load(output_filename); 
  return result;
}

static
void run_linearisation_instance(const std::string& spec, const t_lin_options& options, const lts::probabilistic_lts_aut_t& expected_statespace)
{
  std::clog << "  Linearisation method " << options.lin_method << std::endl
            << "    rewrite strategy: " << options.rewrite_strategy << std::endl
            << "    binary: " << std::boolalpha << options.binary << std::endl
            << "    nocluster: " << std::boolalpha << options.no_intermediate_cluster << std::endl;

  lps::stochastic_specification s=linearise(spec, options);
  BOOST_CHECK(s != lps::stochastic_specification());

  lts::probabilistic_lts_aut_t result = translate_lps_to_lts<lts::probabilistic_lts_aut_t>(s);
  utilities::execution_timer local_timer("dummy");
  BOOST_CHECK(probabilistic_bisimulation_compare_grv(result, expected_statespace, local_timer));
}

static
void run_linearisation_test_case(const std::string& spec, const lts::probabilistic_lts_aut_t& expected_statespace)
{
  // Set various rewrite strategies
  rewrite_strategy_vector rewrite_strategies = data::detail::get_test_rewrite_strategies(false);

  for (rewrite_strategy_vector::const_iterator i = rewrite_strategies.begin(); i != rewrite_strategies.end(); ++i)
  {
    t_lin_options options;
    options.rewrite_strategy=*i;

    run_linearisation_instance(spec, options, expected_statespace);

    options.lin_method=lmRegular2;
    run_linearisation_instance(spec, options, expected_statespace);

    options.lin_method=lmStack;
    run_linearisation_instance(spec, options, expected_statespace);

    options.binary=true;
    run_linearisation_instance(spec, options, expected_statespace);

    options.lin_method=lmRegular;
    run_linearisation_instance(spec, options, expected_statespace);

    options.binary=false; // reset binary
    options.no_intermediate_cluster=true;
    run_linearisation_instance(spec, options, expected_statespace);
  }
}

BOOST_AUTO_TEST_CASE(test_where_renaming_of_variables_goes_astray)
{
  const std::string spec =
      "act step:Bool;\n"
      "\n"
      "map f0:Nat->Bool;\n"
      "var n:Nat;\n"
      "eqn f0(n)=false;\n"
      "\n"
      "proc P(f:Nat->Bool)= step(f(0)). dist b:Bool[1/2].P(f[0->b]);\n"
      "\n"
      "init sum b:Bool.P(f0[0->b]);\n";

  const std::string expected_statespace =
      "des (0,5,4)\n"
      "(0,\"step(false)\",1 1/2 2)\n"
      "(0,\"step(true)\",1 1/2 2)\n"
      "(1,\"step(false)\",3 1/2 2)\n"
      "(2,\"step(true)\",3 1/2 2)\n"
      "(3,\"step(false)\",3 1/2 2)\n";

  std::stringstream is(expected_statespace);

  lts::probabilistic_lts_aut_t statespace;
  statespace.load(is);
  run_linearisation_test_case(spec,statespace);
}

