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
#include "mcrl2/pbes/detail/stategraph_global_reset_variables.h"
#include "mcrl2/pbes/detail/stategraph_local_reset_variables.h"
#include "mcrl2/pbes/io.h"
#include "mcrl2/pbes/tools/pbesstategraph_options.h"
#include "mcrl2/utilities/logger.h"

namespace mcrl2 {

namespace pbes_system {

void pbesstategraph(const std::string& input_filename,
                    const std::string& output_filename,
                    const utilities::file_format* input_format,
                    const utilities::file_format* output_format,
                    const pbesstategraph_options& options)
{
  pbes p;
  load_pbes(p, input_filename, input_format);
  algorithms::normalize(p);
  pbes q;

  if (options.use_global_variant)
  {
    detail::global_reset_variables_algorithm algorithm(p, options);
    q = algorithm.run();
    if (options.print_influence_graph)
    {
      detail::stategraph_influence_graph_algorithm ialgo(algorithm.get_pbes());
      ialgo.run();
    }
  }
  else
  {
    detail::local_reset_variables_algorithm algorithm(p, options);
    q = algorithm.run();
    if (options.print_influence_graph)
    {
      detail::stategraph_influence_graph_algorithm ialgo(algorithm.get_pbes());
      ialgo.run();
    }
  }

  save_pbes(q, output_filename, output_format, false);
  if (!q.is_well_typed())
  {
    mCRL2log(log::error) << "pbesstategraph error: not well typed!" << std::endl;
    mCRL2log(log::error) << pp(q) << std::endl;
  }
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_TOOLS_PBESSTATEGRAPH_H
