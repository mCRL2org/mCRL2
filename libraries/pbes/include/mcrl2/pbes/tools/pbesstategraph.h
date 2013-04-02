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
#include "mcrl2/pbes/tools.h"
#include "mcrl2/utilities/logger.h"

namespace mcrl2 {

namespace pbes_system {

void pbesstategraph(const std::string& input_filename,
                    const std::string& output_filename,
                    data::rewrite_strategy rewrite_strategy,
                    bool simplify,
                    bool apply_to_original,
                    bool use_local_variant,
                    bool print_influence_graph,
                    bool use_marking_optimization
                   )
{
  pbes<> p;
  pbes_system::algorithms::load_pbes(p, input_filename);
  pbes_system::algorithms::normalize(p);
  pbes<> q;

  if (use_local_variant)
  {
    pbes_system::detail::local_reset_variables_algorithm algorithm(p, rewrite_strategy);
    q = algorithm.run(simplify, use_marking_optimization);
    if (print_influence_graph)
    {
      pbes_system::detail::stategraph_influence_graph_algorithm ialgo(algorithm.get_pbes());
      ialgo.run();
    }
  }
  else
  {
    pbes_system::detail::global_reset_variables_algorithm algorithm(p, rewrite_strategy);
    q = algorithm.run(simplify);
    if (print_influence_graph)
    {
      pbes_system::detail::stategraph_influence_graph_algorithm ialgo(algorithm.get_pbes());
      ialgo.run();
    }
  }

  q.save(output_filename, true, true);
  if (!q.is_well_typed())
  {
    mCRL2log(log::error) << "pbesstategraph error: not well typed!" << std::endl;
    mCRL2log(log::error) << pbes_system::pp(q) << std::endl;
  }
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_TOOLS_PBESSTATEGRAPH_H
