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

#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace mcrl2
{

namespace utilities
{

/// \brief Generate a random alphanumeric character
inline
char rand_alnum()
{
  char c;
  do
  {
    c = static_cast<char>(std::rand());
  } while(!std::isalnum(static_cast<unsigned char>(c)));
  // The cast to unsigned char, above here, is to ensure that the value passed
  // to isalnum is between 0 and 255, which is required by std::isalnum.
  // MSVC checks for these bounds in debug mode.
  return c;

}

/// \brief Generate a random string of length n
inline
std::string rand_alnum_str(const std::string::size_type n)
{
  std::string s;
  s.reserve(n);
  std::generate_n(std::back_inserter(s), n, rand_alnum);
  return s;
}

inline
bool file_exists(const char *filename)
{
  std::ifstream ifile(filename);
  return ifile.good();
}

/// \brief Get filename with random suffix
/// \warning is prone to race conditions
inline
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

} // namespace utilities

} // namespace mcrl2

#endif // MCRL2_UTILITIES_TEST_UTILITIES_H
