// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/control_flow.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_CONTROL_FLOW_H
#define MCRL2_PBES_DETAIL_CONTROL_FLOW_H

#include "mcrl2/pbes/detail/control_flow_reset_variables.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

/// \brief Algorithm class for the control_flow algorithm
class control_flow_algorithm: public control_flow_reset_variables_algorithm
{
  public:
    /// \brief Runs the control_flow algorithm
    /// \param simplify If true, simplify the resulting PBES
    /// \param apply_to_original_pbes Apply resetting variables to the original PBES instead of the PFNF one
    pbes<> run(const pbes<>& p, bool simplify = true, bool apply_to_original_pbes = false, bool verbose = true)
    {
      control_flow_graph_algorithm::run(p, verbose);
      return control_flow_reset_variables_algorithm::run(p, simplify, apply_to_original_pbes, verbose);
    }
};

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_CONTROL_FLOW_H
