///////////////////////////////////////////////////////////////////////////////
/// \file lps/detail/utility.h

#ifndef LPS_DETAIL_READ_TEXT_H
#define LPS_DETAIL_READ_TEXT_H

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

#endif // LPS_DETAIL_READ_TEXT_H
