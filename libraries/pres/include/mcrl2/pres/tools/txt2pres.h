// Author(s): Jan Friso Groote. Based on pres/tools/txt2pbes by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pres/tools/txt2pres.h
/// \brief add your file description here.

#ifndef MCRL2_PRES_TOOLS_TXT2PRES_H
#define MCRL2_PRES_TOOLS_TXT2PRES_H

#include "mcrl2/pres/txt2pres.h"

namespace mcrl2 {

namespace pres_system {

void txt2pres(const std::string& input_filename,
              const std::string& output_filename,
              const utilities::file_format& output_format,
              bool normalize
             )
{
  pres p;
  if (input_filename.empty())
  {
    //parse specification from stdin
    mCRL2log(log::verbose) << "reading input from stdin..." << std::endl;
    p = txt2pres(std::cin, normalize);
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
    p = txt2pres(instream, normalize);
    instream.close();
  }
  save_pres(p, output_filename, output_format);
}

} // namespace pres_system

} // namespace mcrl2

#endif // MCRL2_PRES_TOOLS_TXT2PRES_H
