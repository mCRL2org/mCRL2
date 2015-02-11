// Author(s): Frank Stappers
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/rewrite_strategies.h
/// \brief Utility functions for unit testing

#ifndef MCRL2_DATA_DETAIL_REWRITE_STRATEGIES
#define MCRL2_DATA_DETAIL_REWRITE_STRATEGIES

#include <algorithm>
#include <string>
#include <sstream>
#include <vector>
#include <cctype>
#include <fstream>
#include "mcrl2/data/rewrite_strategy.h"
#include "mcrl2/data/detail/rewrite_strategies.h"

namespace mcrl2
{

namespace data
{

namespace detail
{

/// \brief Static initialisation of rewrite strategies used for testing.
static inline
std::vector<data::rewrite_strategy> initialise_test_rewrite_strategies(const bool with_prover)
{
  std::vector<data::rewrite_strategy> result;
  result.push_back(data::jitty);
  if (with_prover)
  {
    result.push_back(data::jitty_prover);
  }
#ifdef MCRL2_TEST_COMPILERS
#ifdef MCRL2_JITTYC_AVAILABLE
  result.push_back(data::jitty_compiling);
  if (with_prover)
  {
    result.push_back(data::jitty_compiling_prover);
  }
#endif // MCRL2_JITTYC_AVAILABLE
#endif // MCRL2_TEST_COMPILERS

  return result;
}

/// \brief Rewrite strategies that should be tested.
inline
const std::vector<data::rewrite_strategy>& get_test_rewrite_strategies(const bool with_prover)
{
  static std::vector<data::rewrite_strategy> rewrite_strategies = initialise_test_rewrite_strategies(with_prover);
  return rewrite_strategies;
}

}

}

}

#endif //MCRL2_DATA_DETAIL_REWRITE_STRATEGIES
