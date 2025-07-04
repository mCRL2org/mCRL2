// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/stategraph.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_STATEGRAPH_H
#define MCRL2_PBES_STATEGRAPH_H

#include "mcrl2/pbes/detail/stategraph_global_reset_variables.h"
#include "mcrl2/pbes/detail/stategraph_local_reset_variables.h"



namespace mcrl2::pbes_system {

/// \brief Apply the stategraph algorithm
/// \param p A PBES to which the algorithm is applied.
/// \param options The options for the algorithm.
inline
void stategraph(pbes& p, const pbesstategraph_options& options)
{
  algorithms::normalize(p);
  if (options.use_global_variant)
  {
    detail::global_reset_variables_algorithm algorithm(p, options);
    algorithm.run();
    p = algorithm.result();
    if (options.print_influence_graph)
    {
      detail::stategraph_influence_graph_algorithm ialgo(algorithm.get_pbes());
      ialgo.run();
    }
  }
  else
  {
    detail::local_reset_variables_algorithm algorithm(p, options);
    algorithm.run();
    p = algorithm.result();
    if (options.print_influence_graph)
    {
      detail::stategraph_influence_graph_algorithm ialgo(algorithm.get_pbes());
      ialgo.run();
    }
  }
}

} // namespace mcrl2::pbes_system



#endif // MCRL2_PBES_STATEGRAPH_H
