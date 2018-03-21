// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/pbes_io.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_PBES_IO_H
#define MCRL2_PBES_DETAIL_PBES_IO_H

#include <fstream>
#include <iostream>
#include <string>
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/utilities/exception.h"
#include "mcrl2/utilities/text_utility.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

/// \brief Loads a PBES from filename, or from stdin if filename equals "".
inline
pbes load_pbes(const std::string& filename)
{
  pbes result;
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
void save_pbes(const pbes& pbesspec, const std::string& filename)
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

// inline
// std::string file_extension(const std::string& filename)
// {
//   auto pos = filename.find_last_of('.');
//   if (pos == std::string::npos)
//   {
// 	return "";
//   }
//   return filename.substr(pos + 1);
// }
//
// inline
// void my_save_pbes(const pbes& p, const std::string& filename)
// {
//   auto ext = file_extension(filename);
//   if (ext == "pbes")
//   {
//     save_pbes(p, filename, pbes_format_internal());
//   }
//   else if (ext == "bes")
//   {
//     bes::save_pbes(p, filename, bes::bes_format_internal());
//   }
//   else if (ext == "pg")
//   {
//     bes::save_pbes(p, filename, bes::bes_format_pgsolver());
//   }
//   else
//   {
//     pbes_system::save_pbes(p, filename);
//   }
// }

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_PBES_IO_H
