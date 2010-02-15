// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/detail/read_text.h
/// \brief Add your file description here.

#ifndef MCRL2_BASIC_DETAIL_READ_TEXT_H
#define MCRL2_BASIC_DETAIL_READ_TEXT_H

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <iterator>

namespace mcrl2 {

namespace modal_formula {

namespace detail {

/// \brief Reads text from a file
/// \param filename A string
/// \param warn If true, print a warning
/// \return The text that has been read, or an empty string upon failure
inline
std::string read_text(const std::string& filename, bool warn=false)
{
  std::ifstream in(filename.c_str());
  if (!in)
  {
    if (warn)
      std::cerr << "Could not open input file: " << filename << std::endl;
    return "";
  }
  in.unsetf(std::ios::skipws); //  Turn of white space skipping on the stream

  std::string s;
  std::copy(
    std::istream_iterator<char>(in),
    std::istream_iterator<char>(),
    std::back_inserter(s)
  );

  return s;
}

} // namespace detail

} // namespace modal_formula

} // namespace mcrl2

#endif // MCRL2_BASIC_DETAIL_READ_TEXT_H
