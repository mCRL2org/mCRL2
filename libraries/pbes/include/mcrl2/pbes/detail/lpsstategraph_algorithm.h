// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/lpsstategraph_algorithm.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_LPSSTATEGRAPH_ALGORITHM_H
#define MCRL2_PBES_DETAIL_LPSSTATEGRAPH_ALGORITHM_H

#include "mcrl2/lps/io.h"
#include "mcrl2/lps/is_well_typed.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/pbes/detail/stategraph_global_reset_variables.h"
#include "mcrl2/pbes/detail/stategraph_local_reset_variables.h"
#include "mcrl2/pbes/tools/pbesstategraph_options.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

inline
pbes construct_stategraph_pbes(const lps::specification& lpsspec)
{
  return pbes();
}

inline
void extract_stategraph_lps(lps::specification& lpsspec /* some parameters containing the marking */)
{
}

} // namespace detail

/// \brief Apply the stategraph algorithm
/// \param p A PBES to which the algorithm is applied.
/// \param options The options for the algorithm.
inline
void lpsstategraph(lps::specification& lpsspec, const pbesstategraph_options& options)
{
  pbes p = detail::construct_stategraph_pbes(lpsspec);

  algorithms::normalize(p);
  if (options.use_global_variant)
  {
    detail::global_reset_variables_algorithm algorithm(p, options);
    p = algorithm.run();
    if (options.print_influence_graph)
    {
      detail::stategraph_influence_graph_algorithm ialgo(algorithm.get_pbes());
      ialgo.run();
    }
  }
  else
  {
    detail::local_reset_variables_algorithm algorithm(p, options);
    p = algorithm.run();
    if (options.print_influence_graph)
    {
      detail::stategraph_influence_graph_algorithm ialgo(algorithm.get_pbes());
      ialgo.run();
    }
  }

  detail::extract_stategraph_lps(lpsspec);
}

void lpsstategraph(const std::string& input_filename,
                   const std::string& output_filename,
                   const pbesstategraph_options& options)
{
  lps::specification lpsspec;
  lps::load_lps(lpsspec, input_filename);
  lpsstategraph(lpsspec, options);
  lps::save_lps(lpsspec, output_filename);
  if (!lps::detail::is_well_typed(lpsspec))
  {
    mCRL2log(log::error) << "lpsstategraph error: not well typed!" << std::endl;
    mCRL2log(log::error) << pp(lpsspec) << std::endl;
  }
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_LPSSTATEGRAPH_ALGORITHM_H
