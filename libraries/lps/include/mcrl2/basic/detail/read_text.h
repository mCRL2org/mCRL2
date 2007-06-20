// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/basic/detail/read_text.h
/// \brief Add your file description here.

#ifndef MCRL2_BASIC_DETAIL_READ_TEXT_H
#define MCRL2_BASIC_DETAIL_READ_TEXT_H

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <iterator>

namespace lps {

namespace detail {

/// read text from a file
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

} // namespace lps

#endif // MCRL2_BASIC_DETAIL_READ_TEXT_H
