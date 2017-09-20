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

#include "mcrl2/pbes/io.h"
#include "mcrl2/pbes/detail/pfnf_print.h"
#include "mcrl2/pbes/detail/pfnf_print.h"
#include "mcrl2/utilities/logger.h"
#include <fstream>

namespace mcrl2 {

namespace pbes_system {

void pbespp(const std::string& input_filename,
            const std::string& output_filename,
            const utilities::file_format& input_format,
            core::print_format_type format,
            bool use_pfnf_printer
           )
{
  pbes p;
  load_pbes(p, input_filename, input_format);

  mCRL2log(log::verbose) << "printing PBES from "
                         << (input_filename.empty()?"standard input":input_filename)
                         << " to " << (output_filename.empty()?"standard output":output_filename)
                         << " in the " << core::pp_format_to_string(format) << " format" << std::endl;

  if (output_filename.empty())
  {
    if (format == core::print_internal)
    {
      std::cout << pbes_to_aterm(p);
    }
    else if(use_pfnf_printer && detail::is_pfnf(p))
    {
      std::cout << pfnf_pp(p);
    }
    else
    {
      std::cout << pp(p);
    }
  }
  else
  {
    std::ofstream out(output_filename.c_str());
    if (out)
    {
      if (format == core::print_internal)
      {
        out << pbes_to_aterm(p);
      }
      else if(use_pfnf_printer && detail::is_pfnf(p))
      {
        out << pfnf_pp(p);
      }
      else
      {
        out << pp(p);
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
