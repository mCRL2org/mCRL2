// Author(s): Jan Friso Groote. Based on pbesconstelm by Wieger Wesselink.
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pres/tools/presconstelm.h
/// \brief add your file description here.

#ifndef MCRL2_PRES_TOOLS_PRESCONSTELM_H
#define MCRL2_PRES_TOOLS_PRESCONSTELM_H

#include "mcrl2/pres/constelm.h"
#include "mcrl2/pres/io.h"

namespace mcrl2 {

namespace pres_system {

void presconstelm(const std::string& input_filename,
                  const std::string& output_filename,
                  const utilities::file_format& input_format,
                  const utilities::file_format& output_format,
                  data::rewrite_strategy rewrite_strategy,
                  pres_rewriter_type rewriter_type,
                  bool compute_conditions,
                  bool remove_redundant_equations,
                  bool check_quantifiers
                 )
{
  // load the pres
  pres p;
  load_pres(p, input_filename, input_format);

  constelm(p, rewrite_strategy, rewriter_type, compute_conditions, remove_redundant_equations, check_quantifiers);

  // save the result
  save_pres(p, output_filename, output_format);
}

} // namespace pres_system

} // namespace mcrl2

#endif // MCRL2_PRES_TOOLS_PRESCONSTELM_H
