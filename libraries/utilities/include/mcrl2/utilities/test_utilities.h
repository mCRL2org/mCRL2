// Author(s): Frank Stappers
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/test_utilities.h
/// \brief Utility functions for unit testing

#ifndef MCRL2_UTILITIES_TEST_UTILITIES_H
#define MCRL2_UTILITIES_TEST_UTILITIES_H

#include "mcrl2/core/garbage_collection.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/rewriter.h"

namespace mcrl2
{

namespace utilities
{

/// \brief Garbage collect after each case.
/// Use with BOOST_GLOBAL_FIXTURE(collect_after_test_case)
struct collect_after_test_case
{
  ~collect_after_test_case()
  {
    core::garbage_collect();
  }
};

/// \brief Static initialisation of rewrite strategies used for testing.
static inline
std::vector<data::basic_rewriter<data::data_expression>::strategy> initialise_test_rewrite_strategies(const bool with_prover)
{
  std::vector<data::basic_rewriter<data::data_expression>::strategy> result;
  result.push_back(data::basic_rewriter<data::data_expression>::jitty);
  if (with_prover)
  {
    result.push_back(data::basic_rewriter<data::data_expression>::jitty_prover);
  }
#ifdef MCRL2_TEST_COMPILERS
#ifdef MCRL2_JITTYC_AVAILABLE
  result.push_back(data::basic_rewriter<data::data_expression>::jitty_compiling);
  if (with_prover)
  {
    result.push_back(data::basic_rewriter<data::data_expression>::jitty_compiling_prover);
  }
#endif // MCRL2_JITTYC_AVAILABLE
#endif // MCRL2_TEST_COMPILERS

  return result;
}

/// \brief Rewrite strategies that should be tested.
static inline
const std::vector<data::basic_rewriter<data::data_expression>::strategy>& get_test_rewrite_strategies(const bool with_prover)
{
  static std::vector<data::basic_rewriter<data::data_expression>::strategy> rewrite_strategies = initialise_test_rewrite_strategies(with_prover);
  return rewrite_strategies;
}

}

}

#endif //MCRL2_UTILITIES_TEST_UTILITIES_H
