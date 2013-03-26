// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbes/source/tools2.cpp
/// \brief Tool implementations.

#include <cassert>
#include <fstream>
#include <sstream>

#include "mcrl2/data/enumerator.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/modal_formula/state_formula.h"
#include "mcrl2/modal_formula/parse.h"
#include "mcrl2/pbes/constelm.h"
#include "mcrl2/pbes/file_formats.h"
#include "mcrl2/pbes/io.h"
#include "mcrl2/pbes/lps2pbes.h"
#include "mcrl2/pbes/pbes_rewriter_type.h"
#include "mcrl2/pbes/tools.h"
#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/pbes/remove_equations.h"

namespace mcrl2
{

namespace pbes_system
{

void lps2pbes(const std::string& input_filename,
              const std::string& output_filename,
              const std::string& formula_filename,
              bool timed,
              bool structured,
              bool unoptimized
             )
{
  if (formula_filename.empty())
  {
    throw mcrl2::runtime_error("option -f is not specified");
  }

  //load LPS
  if (input_filename.empty())
  {
    mCRL2log(log::verbose) << "reading LPS from stdin..." << std::endl;
  }
  else
  {
    mCRL2log(log::verbose) << "reading LPS from file '" <<  input_filename << "'..." << std::endl;
  }
  lps::specification spec;
  spec.load(input_filename);
  //load formula file
  mCRL2log(log::verbose) << "reading input from file '" <<  formula_filename << "'..." << std::endl;
  std::ifstream instream(formula_filename.c_str(), std::ifstream::in | std::ifstream::binary);
  if (!instream)
  {
    throw mcrl2::runtime_error("cannot open state formula file: " + formula_filename);
  }
  state_formulas::state_formula formula = state_formulas::parse_state_formula(instream, spec);
  instream.close();
  //convert formula and LPS to a PBES
  mCRL2log(log::verbose) << "converting state formula and LPS to a PBES..." << std::endl;
  pbes_system::pbes<> result = pbes_system::lps2pbes(spec, formula, timed, structured, unoptimized);
  //save the result
  if (output_filename.empty())
  {
    mCRL2log(log::verbose) << "writing PBES to stdout..." << std::endl;
  }
  else
  {
    mCRL2log(log::verbose) << "writing PBES to file '" <<  output_filename << "'..." << std::endl;
  }
  result.save(output_filename);
}

void pbesconstelm(const std::string& input_filename,
                  const std::string& output_filename,
                  data::rewriter::strategy rewrite_strategy,
                  pbes_rewriter_type rewriter_type,
                  bool compute_conditions,
                  bool remove_redundant_equations
                 )
{
  // load the pbes
  pbes<> p;
  load_pbes(p, input_filename);

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
        std::vector<propositional_variable> V = remove_unreachable_variables(p);
        mCRL2log(log::verbose) << pbes_system::detail::print_removed_equations(V);
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
        std::vector<propositional_variable> V = remove_unreachable_variables(p);
        mCRL2log(log::verbose) << pbes_system::detail::print_removed_equations(V);
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
