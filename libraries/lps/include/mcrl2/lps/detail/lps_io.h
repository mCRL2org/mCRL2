// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/detail/lps_io.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_DETAIL_LPS_IO_H
#define MCRL2_LPS_DETAIL_LPS_IO_H

#include "mcrl2/atermpp/aterm_io_binary.h"
#include "mcrl2/lps/specification.h"

#include <fstream>

namespace mcrl2 {

namespace lps {

namespace detail {

/// \brief Loads an lps from filename, or from stdin if filename equals "".
inline
lps::specification load_lps(const std::string& filename)
{
  lps::specification result;
  if (filename.empty())
  {
    atermpp::binary_aterm_istream(std::cin) >> result;
  }
  else
  {
    std::ifstream from(filename, std::ifstream::in | std::ifstream::binary);
    if (!from.good())
    {
      throw mcrl2::runtime_error("Could not read from filename " + filename);
    }
    atermpp::binary_aterm_istream(from) >> result;
  }
  return result;
}

/// \brief Saves an LPS to filename, or to stdout if filename equals "".
inline
void save_lps(const lps::specification& lpsspec, const std::string& filename)
{
  if (filename.empty())
  {
    atermpp::binary_aterm_ostream(std::cout) << lpsspec;
  }
  else
  {
    std::ofstream to(filename, std::ofstream::out | std::ofstream::binary);
    if (!to.good())
    {
      throw mcrl2::runtime_error("Could not write to filename " + filename);
    }
    atermpp::binary_aterm_ostream(to) << lpsspec;
  }
}

} // namespace detail

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_DETAIL_LPS_IO_H
