// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/tools/lps2pbes.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_TOOLS_LPS2PBES_H
#define MCRL2_PBES_TOOLS_LPS2PBES_H

#include <fstream>
#include "mcrl2/data/merge_data_specifications.h"
#include "mcrl2/lps/io.h"
#include "mcrl2/modal_formula/algorithms.h"
#include "mcrl2/modal_formula/parse.h"
#include "mcrl2/modal_formula/state_formula.h"
#include "mcrl2/pbes/lps2pbes.h"
#include "mcrl2/pbes/io.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/text_utility.h"

namespace mcrl2 {

namespace pbes_system {

void lps2pbes(const std::string& input_filename,
              const std::string& output_filename,
              const utilities::file_format* output_format,
              const std::string& formula_filename,
              bool timed,
              bool structured,
              bool unoptimized,
              bool preprocess_modal_operators,
              bool generate_counter_example
             )
{
  if (formula_filename.empty())
  {
    throw mcrl2::runtime_error("option -f is not specified");
  }
  if (input_filename.empty())
  {
    mCRL2log(log::verbose) << "reading LPS from stdin..." << std::endl;
  }
  else
  {
    mCRL2log(log::verbose) << "reading LPS from file '" <<  input_filename << "'..." << std::endl;
  }
  lps::specification lpsspec;
  load_lps(lpsspec, input_filename);
  mCRL2log(log::verbose) << "reading input from file '" <<  formula_filename << "'..." << std::endl;
  std::ifstream from(formula_filename.c_str(), std::ifstream::in | std::ifstream::binary);
  if (!from)
  {
    throw mcrl2::runtime_error("cannot open state formula file: " + formula_filename);
  }
  std::string text = utilities::read_text(from);
  state_formulas::state_formula_specification formspec = state_formulas::algorithms::parse_state_formula_specification(text, lpsspec);
  mCRL2log(log::verbose) << "converting state formula and LPS to a PBES..." << std::endl;
  pbes_system::pbes result = pbes_system::lps2pbes(lpsspec, formspec, timed, structured, unoptimized, preprocess_modal_operators, generate_counter_example);
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

#endif // MCRL2_PBES_TOOLS_LPS2PBES_H
