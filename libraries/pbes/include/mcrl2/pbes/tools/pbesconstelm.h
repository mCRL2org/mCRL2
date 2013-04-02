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

#include "mcrl2/data/enumerator.h"
#include "mcrl2/pbes/algorithms.h"
#include "mcrl2/pbes/constelm.h"
#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/pbes/tools.h"
#include "mcrl2/utilities/logger.h"

namespace mcrl2 {

namespace pbes_system {

void pbesconstelm(const std::string& input_filename,
                  const std::string& output_filename,
                  data::rewrite_strategy rewrite_strategy,
                  pbes_rewriter_type rewriter_type,
                  bool compute_conditions,
                  bool remove_redundant_equations
                 )
{
  // load the pbes
  pbes<> p;
  pbes_system::algorithms::load_pbes(p, input_filename);

  // data rewriter
  data::rewriter datar(p.data(), rewrite_strategy);

  // pbes rewriter
  switch (rewriter_type)
  {
    case simplify:
    {
      typedef simplifying_rewriter<pbes_system::pbes_expression, data::rewriter> my_pbes_rewriter;
      my_pbes_rewriter pbesr(datar);
      pbes_constelm_algorithm<pbes_system::pbes_expression, data::rewriter, my_pbes_rewriter> algorithm(datar, pbesr);
      algorithm.run(p, compute_conditions);
      if (remove_redundant_equations)
      {
        std::vector<propositional_variable> V = pbes_system::algorithms::remove_unreachable_variables(p);
        mCRL2log(log::verbose) << pbes_system::algorithms::print_removed_equations(V);
      }
      break;
    }
    case quantifier_all:
    case quantifier_finite:
    {
      typedef pbes_system::enumerate_quantifiers_rewriter<pbes_system::pbes_expression, data::rewriter_with_variables, data::data_enumerator> my_pbes_rewriter;
      bool enumerate_infinite_sorts = (rewriter_type == quantifier_all);
      data::data_enumerator datae(p.data(), datar);
      data::rewriter_with_variables datarv(datar);
      my_pbes_rewriter pbesr(datarv, datae, enumerate_infinite_sorts);
      pbes_constelm_algorithm<pbes_system::pbes_expression, data::rewriter, my_pbes_rewriter> algorithm(datar, pbesr);
      algorithm.run(p, compute_conditions);
      if (remove_redundant_equations)
      {
        std::vector<propositional_variable> V = pbes_system::algorithms::remove_unreachable_variables(p);
        mCRL2log(log::verbose) << pbes_system::algorithms::print_removed_equations(V);
      }
      break;
    }
    default:
    { }
  }

  // save the result
  p.save(output_filename);
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_TOOLS_PBESCONSTELM_H
