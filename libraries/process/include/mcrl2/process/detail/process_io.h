// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/detail/process_io.h

#ifndef MCRL2_PROCESS_DETAIL_PROCESS_IO_H
#define MCRL2_PROCESS_DETAIL_PROCESS_IO_H

#include <fstream>
#include "mcrl2/process/parse.h"

namespace mcrl2::process::detail
{

/// \brief Loads a process specification from input_filename, or from stdin if filename equals "".
inline
process_specification parse_process_specification(const std::string& input_filename)
{
  process_specification result;
  if (input_filename.empty())
  {
    return process::parse_process_specification(std::cin);
  }
  else
  {
    std::ifstream from(input_filename, std::ifstream::in | std::ifstream::binary);
    return process::parse_process_specification(from);
  }
  return result;
}

} // namespace mcrl2::process::detail

#endif // MCRL2_PROCESS_DETAIL_PROCESS_IO_H
