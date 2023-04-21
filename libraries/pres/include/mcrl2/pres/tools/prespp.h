// Author(s): Jan Friso Groote. Based on pbes/tools/pbespp.h by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pres/tools/prespp.h
/// \brief add your file description here.

#ifndef MCRL2_PRES_TOOLS_PRESPP_H
#define MCRL2_PRES_TOOLS_PRESPP_H

#include "mcrl2/pres/io.h"

namespace mcrl2 {

namespace pres_system {

void prespp(const std::string& input_filename,
            const std::string& output_filename,
            const utilities::file_format& input_format,
            core::print_format_type format,
            bool use_pfnf_printer
           )
{
  pres p;
  load_pres(p, input_filename, input_format);

  mCRL2log(log::verbose) << "printing PRES from "
                         << (input_filename.empty()?"standard input":input_filename)
                         << " to " << (output_filename.empty()?"standard output":output_filename)
                         << " in the " << core::pp_format_to_string(format) << " format" << std::endl;

  if (output_filename.empty())
  {
    if (format == core::print_internal)
    {
      std::cout << pres_to_aterm(p);
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
        out << pres_to_aterm(p);
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

} // namespace pres_system

} // namespace mcrl2

#endif // MCRL2_PRES_TOOLS_PRESPP_H
