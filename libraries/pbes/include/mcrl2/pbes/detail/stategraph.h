// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/stategraph.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_STATEGRAPH_H
#define MCRL2_PBES_DETAIL_STATEGRAPH_H

#include "mcrl2/pbes/detail/stategraph_reset_variables.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

/// \brief Algorithm class for the stategraph algorithm
class stategraph_algorithm: public stategraph_reset_variables_algorithm
{
  public:
    /// \brief Runs the stategraph algorithm
    /// \param simplify If true, simplify the resulting PBES
    /// \param apply_to_original_pbes Apply resetting variables to the original PBES instead of the STATEGRAPH one
    pbes<> run(const pbes<>& p, bool simplify = true, bool apply_to_original_pbes = false)
    {
      stategraph_graph_algorithm::run(p);
      return stategraph_reset_variables_algorithm::run(p, simplify, apply_to_original_pbes);
    }
};

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_STATEGRAPH_H
