// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/tools/pbesstategraph.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_TOOLS_PBESSTATEGRAPH_H
#define MCRL2_PBES_TOOLS_PBESSTATEGRAPH_H

#include "mcrl2/pbes/algorithms.h"
#include "mcrl2/pbes/stategraph.h"
#include "mcrl2/pbes/io.h"
#include "mcrl2/pbes/tools/pbesstategraph_options.h"
#include "mcrl2/utilities/logger.h"

namespace mcrl2 {

namespace pbes_system {

void pbesstategraph(const std::string& input_filename,
                    const std::string& output_filename,
                    const utilities::file_format& input_format,
                    const utilities::file_format& output_format,
                    const pbesstategraph_options& options)
{
  pbes p;
  load_pbes(p, input_filename, input_format);

  stategraph(p, options);

  save_pbes(p, output_filename, output_format, false);
  if (!p.is_well_typed())
  {
    mCRL2log(log::error) << "pbesstategraph error: not well typed!" << std::endl;
    mCRL2log(log::error) << pp(p) << std::endl;
  }
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_TOOLS_PBESSTATEGRAPH_H
