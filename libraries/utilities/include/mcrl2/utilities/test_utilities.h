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

#include <string>
#include <sstream>
#include <vector>
#include <cctype>
#include <fstream>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include "mcrl2/core/garbage_collection.h"
#include "mcrl2/data/rewrite_strategy.h"

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

/// \brief Generate a random alphanumeric character
inline
char rand_alnum()
{
  char c;
  do
  {
    c = static_cast<char>(std::rand());
  } while(!std::isalnum(c));
  return c;

}

/// \brief Generate a random string of length n
inline
std::string rand_alnum_str(const std::string::size_type n)
{
  std::string s;
  s.reserve(n);
  generate_n(std::back_inserter(s), n, rand_alnum);
  return s;
}

bool file_exists(const char *filename)
{
  std::ifstream ifile(filename);
  return ifile;
}

/// \brief Get filename with random suffix
/// \warning is prone to race conditions
std::string temporary_filename(std::string const& prefix = "")
{
  std::string basename(prefix + "_" + rand_alnum_str(8));
  std::string result = basename ;
  int suffix = 0;
  while (file_exists(result.c_str()))
  {
    std::stringstream suffix_s;
    suffix_s << suffix;
    result = basename + suffix_s.str();
    ++suffix;
  }
  return result;
}

}

}

#endif //MCRL2_UTILITIES_TEST_UTILITIES_H
