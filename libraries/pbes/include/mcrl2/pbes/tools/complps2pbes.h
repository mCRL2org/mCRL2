// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/tools/complps2pbes.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_TOOLS_COMPLPS2PBES_H
#define MCRL2_PBES_TOOLS_COMPLPS2PBES_H

#include <fstream>
#include "mcrl2/lps/linearise.h"
#include "mcrl2/modal_formula/algorithms.h"
#include "mcrl2/modal_formula/state_formula.h"
#include "mcrl2/pbes/complps2pbes.h"
#include "mcrl2/pbes/io.h"
#include "mcrl2/process/parse.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/text_utility.h"

namespace mcrl2 {

namespace pbes_system {

void complps2pbes(const std::string& input_filename,
                  const std::string& output_filename,
                  const utilities::file_format* output_format,
                  const std::string& formula_filename
                 )
{
  if (formula_filename.empty())
  {
    throw mcrl2::runtime_error("option -f is not specified");
  }

  // load mCRL2 specification
  std::string text;
  if (input_filename.empty())
  {
    mCRL2log(log::verbose) << "reading mCRL2 specification from stdin..." << std::endl;
    text = utilities::read_text(std::cin);
  }
  else
  {
    mCRL2log(log::verbose) << "reading mCRL2 specification from file '" <<  input_filename << "'..." << std::endl;
    std::ifstream from(input_filename.c_str());
    text = utilities::read_text(from);
  }
  // TODO: check if alpha reduction should be applied
  process::process_specification procspec = process::parse_process_specification(text, false);
  lps::specification spec = lps::linearise(procspec);

  // load state formula
  mCRL2log(log::verbose) << "reading formula from file '" <<  formula_filename << "'..." << std::endl;
  std::ifstream instream(formula_filename.c_str(), std::ifstream::in|std::ifstream::binary);
  if (!instream)
  {
    throw mcrl2::runtime_error("cannot open state formula file: " + formula_filename);
  }
  state_formulas::state_formula formula = state_formulas::algorithms::parse_state_formula(instream, spec);
  instream.close();

  pbes result = complps2pbes(procspec, formula);

  // save the result
  if (output_filename.empty())
  {
    mCRL2log(log::verbose) << "writing PBES to stdout..." << std::endl;
  }
  else
  {
    mCRL2log(log::verbose) << "writing PBES to file '" <<  output_filename << "'..." << std::endl;
  }
  save_pbes(result, output_filename, output_format);
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_TOOLS_COMPLPS2PBES_H
