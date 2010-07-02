// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lts_test.cpp
/// \brief Add your file description here.

#ifndef MCRL2_WITH_GARBAGE_COLLECTION
#define MCRL2_WITH_GARBAGE_COLLECTION
#endif

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <boost/test/included/unit_test_framework.hpp>
#include "mcrl2/core/garbage_collection.h"
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/lts/exploration.h"
#include "mcrl2/lts/lts.h"

using namespace mcrl2;

// Garbage collect after each case.
struct collect_after_test_case {
  ~collect_after_test_case()
  {
    core::garbage_collect();
  }
};

BOOST_GLOBAL_FIXTURE(collect_after_test_case)

lts::lts translate_lps_to_lts(lps::specification const& specification, lts::exploration_strategy const strategy = lts::es_breadth)
{
  lts::lts_generation_options options;
  options.trace_prefix = "lps2lts_test";
  options.specification = specification;

  char buffer [L_tmpnam];
  int fd = mkstemp(buffer); // Get temporary filename
  close(fd); // Just to make sure it is created, and avoid race condition in file creation.
  options.lts = std::string(buffer);
  options.outformat = lts::lts_aut;

  lts::lps2lts_algorithm lps2lts;
  lps2lts.initialise_lts_generation(&options);
  lps2lts.generate_lts();
  lps2lts.finalise_lts_generation();

  lts::lts result(options.lts, options.outformat);

  remove(options.lts.c_str());

  return result;
}

void check_lps2lts_specification(std::string const& specification,
                                 const unsigned int expected_states,
                                 const unsigned int expected_transitions,
                                 const unsigned int expected_labels)
{
  lps::specification lps = lps::parse_linear_process_specification(specification);

  lts::lts result = translate_lps_to_lts(lps);

  BOOST_CHECK_EQUAL(result.num_states(), expected_states);
  BOOST_CHECK_EQUAL(result.num_transitions(), expected_transitions);
  BOOST_CHECK_EQUAL(result.num_labels(), expected_labels);
}

BOOST_AUTO_TEST_CASE(test_a) {
  std::string lps(
    "act a;\n"
    "proc P(b:Bool) = (b) -> a.P(!b)\n"
    "               + delta;\n"
    "init P(true);\n"
  );
  check_lps2lts_specification(lps, 2, 1, 1);
}

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  return 0;
}

