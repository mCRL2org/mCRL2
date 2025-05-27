// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/pbessolve.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_PBESSOLVE_H
#define MCRL2_PBES_DETAIL_PBESSOLVE_H

#include "mcrl2/lps/detail/instantiate_global_variables.h"
#include "mcrl2/pbes/pbesinst_structure_graph.h"
#include "mcrl2/pbes/solve_structure_graph.h"
#include <optional>

namespace mcrl2 {

namespace pbes_system {

namespace detail {

bool pbessolve(const pbes& p)
{
  pbessolve_options options;
  pbes pbesspec = p;
  pbes_system::algorithms::normalize(pbesspec);
  structure_graph G;
  pbesinst_structure_graph_algorithm algorithm(options, pbesspec, G);
  algorithm.run();
  return solve_structure_graph(G).first;
}

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_PBESSOLVE_H
