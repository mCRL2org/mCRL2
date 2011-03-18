// Author(s): Wieger Wesselink, Alexander van Dam
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/io.h
/// \brief IO routines for boolean equation systems.

#ifndef MCRL2_PBES_IO_H
#define MCRL2_PBES_IO_H

#include <utility>
#include <iostream>
#include <string>
#include "mcrl2/bes/io.h"
#include "mcrl2/pbes/is_bes.h"
#include "mcrl2/pbes/pbesinstconversion.h"
#include "mcrl2/core/messaging.h"

namespace mcrl2
{

namespace pbes_system
{

/// \brief Supported PBES output formats.
enum pbes_output_format
{
  pbes_output_pbes,
  pbes_output_bes,
  pbes_output_cwi
};

/// \brief Save a PBES in the format specified.
/// \param output_format Determines the format in which the result is written.
/// If the PBES is not a BES, and the format pbes_output_bes_cwi is selected, then
/// the result will be written an ATerm binary format instead.
/// \param save_as_bes If the PBES is a BES, then the result will be written in BES format.
inline
void save_pbes(const pbes<>& pbes_spec, std::string outfilename, pbes_output_format output_format, bool aterm_ascii = false)
{
  switch (output_format)
  {
    case pbes_output_pbes:
    {
      if (aterm_ascii)
      {
        core::gsVerboseMsg("Saving result in ATerm ascii format...\n");
        pbes_spec.save(outfilename, false);
      }
      else
      {
        core::gsVerboseMsg("Saving result in ATerm binary format...\n");
        pbes_spec.save(outfilename, true);
      }
      break;
    }
    case pbes_output_bes:
    {
      if (!is_bes(pbes_spec))
      {
        throw mcrl2::runtime_error("the PBES cannot be saved as a BES");
      }
      bes::boolean_equation_system<> bes_spec = pbesinstconversion(pbes_spec);
      if (aterm_ascii)
      {
        core::gsVerboseMsg("Saving result in ATerm ascii format...\n");
        bes_spec.save(outfilename, false);
      }
      else
      {
        core::gsVerboseMsg("Saving result in ATerm binary format...\n");
        bes_spec.save(outfilename, true);
      }
      break;
    }
    case pbes_output_cwi:
    {
      if (!is_bes(pbes_spec))
      {
        throw mcrl2::runtime_error("the PBES cannot be saved as a BES");
      }
      core::gsVerboseMsg("Saving result in CWI format...\n");
      bes::bes2cwi(pbes_spec.equations().begin(), pbes_spec.equations().end(), outfilename);
      break;
    }
    default:
    {
      throw mcrl2::runtime_error("unknown output format encountered in save_pbes");
    }
  }
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_IO_H
