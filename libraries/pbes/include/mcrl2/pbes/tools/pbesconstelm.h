// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/tools/pbesconstelm.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_TOOLS_PBESCONSTELM_H
#define MCRL2_PBES_TOOLS_PBESCONSTELM_H

#include "mcrl2/pbes/constelm.h"
#include "mcrl2/pbes/io.h"
#include "mcrl2/pbes/pbes_rewriter_type.h"
#include "mcrl2/utilities/logger.h"

namespace mcrl2 {

namespace pbes_system {

void pbesconstelm(const std::string& input_filename,
                  const std::string& output_filename,
                  const utilities::file_format* input_format,
                  const utilities::file_format* output_format,
                  data::rewrite_strategy rewrite_strategy,
                  pbes_rewriter_type rewriter_type,
                  bool compute_conditions,
                  bool remove_redundant_equations
                 )
{
  // load the pbes
  pbes p;
  load_pbes(p, input_filename, input_format);

  constelm(p, rewrite_strategy, rewriter_type, compute_conditions, remove_redundant_equations);

  // save the result
  save_pbes(p, output_filename, output_format);
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_TOOLS_PBESCONSTELM_H
