// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/detail/process_io.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_DETAIL_PROCESS_IO_H
#define MCRL2_PROCESS_DETAIL_PROCESS_IO_H

namespace mcrl2 {

namespace process {

namespace detail {

/// \brief Loads a process specification from input_filename, or from stdin if filename equals "".
inline
process::process_specification parse_process_specification(const std::string& input_filename)
{
  process::process_specification result;
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

/// \brief Loads a PBES from filename, or from stdin if filename equals "".
inline
process_specification load_process_specification(const std::string& filename)
{
  process_specification result;
  if (filename.empty())
  {
    result.load(std::cin);
  }
  else
  {
    std::ifstream from(filename, std::ifstream::in | std::ifstream::binary);
    result.load(from);
  }
  return result;
}

/// \brief Saves an PBES to filename, or to stdout if filename equals "".
inline
void save_process_specification(const process_specification& pbesspec, const std::string& filename)
{
  if (filename.empty())
  {
    pbesspec.save(std::cout);
  }
  else
  {
    std::ofstream to(filename, std::ofstream::out | std::ofstream::binary);
    if (!to.good())
    {
      throw mcrl2::runtime_error("Could not write to filename " + filename);
    }
    pbesspec.save(to);
  }
}

} // namespace detail

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_DETAIL_PROCESS_IO_H
