// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/tools/pbespp.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_TOOLS_PBESPP_H
#define MCRL2_PBES_TOOLS_PBESPP_H

#include <fstream>
#include "mcrl2/pbes/algorithms.h"
#include "mcrl2/pbes/tools.h"
#include "mcrl2/utilities/logger.h"

namespace mcrl2 {

namespace pbes_system {

void pbespp(const std::string& input_filename,
            const std::string& output_filename,
            pbes_file_format pbes_input_format,
            core::print_format_type format
           )
{
  pbes_system::pbes p;
  pbes_system::algorithms::load_pbes(p, input_filename, pbes_input_format);

  mCRL2log(log::verbose) << "printing PBES from "
                         << (input_filename.empty()?"standard input":input_filename)
                         << " to " << (output_filename.empty()?"standard output":output_filename)
                         << " in the " << core::pp_format_to_string(format) << " format" << std::endl;

  if (output_filename.empty())
  {
    if (format == core::print_internal)
    {
      std::cout << pbes_system::pbes_to_aterm(p);
    }
    else
    {
      std::cout << pbes_system::pp(p);
    }
  }
  else
  {
    std::ofstream out(output_filename.c_str());
    if (out)
    {
      if (format == core::print_internal)
      {
        out << pbes_system::pbes_to_aterm(p);
      }
      else
      {
        out << pbes_system::pp(p);
      }
      out.close();
    }
    else
    {
      throw mcrl2::runtime_error("could not open output file " + output_filename + " for writing");
    }
  }
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_TOOLS_PBESPP_H
