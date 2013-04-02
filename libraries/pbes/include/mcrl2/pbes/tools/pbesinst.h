// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/tools/pbesinst.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_TOOLS_PBESINST_H
#define MCRL2_PBES_TOOLS_PBESINST_H

#include "mcrl2/pbes/algorithms.h"
#include "mcrl2/pbes/io.h"
#include "mcrl2/pbes/pbesinst_algorithm.h"
#include "mcrl2/pbes/pbesinst_strategy.h"
#include "mcrl2/pbes/remove_equations.h"
#include "mcrl2/pbes/tools.h"
#include "mcrl2/pbes/detail/pbes_parameter_map.h"
#include "mcrl2/utilities/logger.h"

namespace mcrl2 {

namespace pbes_system {

bool pbesinst(const std::string& input_filename,
              const std::string& output_filename,
              pbes_file_format pbes_input_format,
              pbes_file_format pbes_output_format,
              data::rewrite_strategy rewrite_strategy,
              pbesinst_strategy m_strategy,
              const std::string& finite_parameter_selection,
              bool remove_redundant_equations,
              bool aterm_ascii
             )
{
  // load the pbes
  pbes<> p;
  load_pbes(p, input_filename, pbes_input_format);

  if (!p.is_closed())
  {
    mCRL2log(log::error) << "The PBES is not closed. Pbes2bes cannot handle this kind of PBESs" << std::endl << "Computation aborted." << std::endl;
    return false;
  }

  if (m_strategy == pbesinst_lazy)
  {
    pbesinst_algorithm algorithm(p.data(), rewrite_strategy, false, false);
    algorithm.run(p);
    p = algorithm.get_result();
  }
  else if (m_strategy == pbesinst_finite)
  {
    pbes_system::algorithms::pbesinst_finite(p, rewrite_strategy, finite_parameter_selection);
  }

  if (log::mcrl2_logger::get_reporting_level() >= log::verbose)
  {
    if (is_bes(p))
    {
      mCRL2log(log::debug) << "The result is a BES.\n";
    }
    else
    {
       mCRL2log(log::debug) << "The result is a PBES.\n";
    }
  }

  if (remove_redundant_equations)
  {
    std::vector<propositional_variable> V = remove_unreachable_variables(p);
    mCRL2log(log::verbose) << pbes_system::detail::print_removed_equations(V);
  }

  // save the result
  save_pbes(p, output_filename, pbes_output_format, aterm_ascii);

  return true;
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_TOOLS_PBESINST_H
