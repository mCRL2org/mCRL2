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

#include <iostream>
#include <string>

#include <boost/test/included/unit_test_framework.hpp>

#include "mcrl2/lps/linearise.h"
#include "mcrl2/lts/detail/exploration.h"
#include "mcrl2/lts/lts_aut.h"
#include "mcrl2/lts/lts_algorithm.h"

#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/test_utilities.h"

using namespace mcrl2;
using namespace mcrl2::lps;

typedef data::basic_rewriter<data::data_expression>::strategy rewrite_strategy;
typedef std::vector<rewrite_strategy> rewrite_strategy_vector;

template <class LTS_TYPE>
inline
LTS_TYPE translate_lps_to_lts(lps::specification const& specification)
{
  lts::lts_generation_options options;
  options.trace_prefix = "linearization_instantiation_compare_test";
  options.specification = specification;
  options.lts = utilities::temporary_filename("linearization_instantiation_compare_test_file");

  LTS_TYPE result;
  options.outformat = result.type();
  lts::lps2lts_algorithm lps2lts;
  lps2lts.initialise_lts_generation(&options);
  lps2lts.generate_lts();
  lps2lts.finalise_lts_generation();

  result.load(options.lts);
  remove(options.lts.c_str()); // Clean up after ourselves
  return result;
}

static
void run_linearisation_instance(const std::string& spec, const t_lin_options& options, const lts::lts_aut_t& expected_statespace)
{
  std::clog << "  Linearisation method " << options.lin_method << std::endl
            << "    rewrite strategy: " << options.rewrite_strategy << std::endl
            << "    binary: " << std::boolalpha << options.binary << std::endl
            << "    nocluster: " << std::boolalpha << options.no_intermediate_cluster << std::endl;

  lps::specification s = linearise(spec, options);
  BOOST_CHECK(s != lps::specification());

  lts::lts_aut_t result = translate_lps_to_lts<lts::lts_aut_t>(s);
  BOOST_CHECK(lts::compare(result, expected_statespace, lts::lts_eq_bisim));
}

static
void run_linearisation_test_case(const std::string& spec, const lts::lts_aut_t& expected_statespace)
{
  // Set various rewrite strategies
  rewrite_strategy_vector rewrite_strategies = utilities::get_test_rewrite_strategies(false);

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

BOOST_AUTO_TEST_CASE(bad_renaming_non_bisimilar)
{
  const std::string spec =
      "sort D = struct d1;\n"
      "act a, b;\n"
      "proc\n"
      "P(p:D) = a. Q(p);\n"
      "Q(q:D) = sum l:List(D). (#l<=1) ->\n"
      "             b.\n"
      "            (([] == l) -> P(q) <>  Q());\n"
      "init P(d1);\n";

  const std::string expected_statespace =
      "des (0,6,4)\n"
      "(0,\"a\",1)\n"
      "(1,\"b\",2)\n"
      "(1,\"b\",3)\n"
      "(2,\"a\",1)\n"
      "(3,\"b\",2)\n"
      "(3,\"b\",3)\n";
  std::stringstream is(expected_statespace);

  lts::lts_aut_t statespace;
  statespace.load(is);
  run_linearisation_test_case(spec,statespace);
}

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
  return 0;
}

