// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/tools/txt2pbes.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_TOOLS_TXT2PBES_H
#define MCRL2_PBES_TOOLS_TXT2PBES_H

#include "mcrl2/pbes/algorithms.h"
#include "mcrl2/pbes/txt2pbes.h"
#include "mcrl2/utilities/logger.h"
#include <fstream>

namespace mcrl2 {

namespace pbes_system {

void txt2pbes(const std::string& input_filename,
              const std::string& output_filename,
              const utilities::file_format& output_format,
              bool normalize
             )
{
  pbes p;
  if (input_filename.empty())
  {
    //parse specification from stdin
    mCRL2log(log::verbose) << "reading input from stdin..." << std::endl;
    p = txt2pbes(std::cin, normalize);
  }
  else
  {
    //parse specification from input filename
    mCRL2log(log::verbose) << "reading input from file '" <<  input_filename << "'..." << std::endl;
    std::ifstream instream(input_filename.c_str(), std::ifstream::in|std::ifstream::binary);
    if (!instream)
    {
      throw mcrl2::runtime_error("cannot open input file: " + input_filename);
    }
    p = txt2pbes(instream, normalize);
    instream.close();
  }
  save_pbes(p, output_filename, output_format);
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_TOOLS_TXT2PBES_H
