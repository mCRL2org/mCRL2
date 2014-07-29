// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/tools/pbesparelm.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_TOOLS_PBESPARELM_H
#define MCRL2_PBES_TOOLS_PBESPARELM_H

#include "mcrl2/pbes/io.h"
#include "mcrl2/pbes/parelm.h"

namespace mcrl2 {

namespace pbes_system {

void pbesparelm(const std::string& input_filename,
                const std::string& output_filename,
                const utilities::file_format* input_format,
                const utilities::file_format* output_format
               )
{
  // load the pbes
  pbes p;
  load_pbes(p, input_filename, input_format);

  // apply the algorithm
  pbes_parelm_algorithm algorithm;
  algorithm.run(p);

  // save the result
  save_pbes(p, output_filename, output_format);
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_TOOLS_PBESPARELM_H
