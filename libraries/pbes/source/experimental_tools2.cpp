// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbes/source/experimental_tools2.cpp
/// \brief Tool implementations.

#include <cassert>
#include <fstream>
#include <sstream>
#include "mcrl2/lps/linearise.h"
#include "mcrl2/modal_formula/parse.h"
#include "mcrl2/process/parse.h"
#include "mcrl2/pbes/complps2pbes.h"
#include "mcrl2/pbes/io.h"
#include "mcrl2/pbes/normalize.h"
#include "mcrl2/pbes/detail/stategraph_global_reset_variables.h"
#include "mcrl2/pbes/detail/stategraph_local_reset_variables.h"

namespace mcrl2
{

namespace pbes_system
{

void pbesstategraph(const std::string& input_filename,
                    const std::string& output_filename,
                    data::rewriter::strategy rewrite_strategy,
                    bool simplify,
                    bool apply_to_original,
                    bool use_local_variant,
                    bool print_influence_graph,
                    bool use_marking_optimization
                   )
{
  pbes<> p;
  load_pbes(p, input_filename);
  pbes_system::normalize(p);
  pbes<> q;

  if (use_local_variant)
  {
    pbes_system::detail::local_reset_variables_algorithm algorithm(p, rewrite_strategy);
    q = algorithm.run(simplify, use_marking_optimization);
    if (print_influence_graph)
    {
      pbes_system::detail::stategraph_influence_graph_algorithm ialgo(algorithm.get_pbes());
      ialgo.run();
    }
  }
  else
  {
    pbes_system::detail::global_reset_variables_algorithm algorithm(p, rewrite_strategy);
    q = algorithm.run(simplify);
    if (print_influence_graph)
    {
      pbes_system::detail::stategraph_influence_graph_algorithm ialgo(algorithm.get_pbes());
      ialgo.run();
    }
  }

  q.save(output_filename, true, true);
  if (!q.is_well_typed())
  {
    mCRL2log(log::error) << "pbesstategraph error: not well typed!" << std::endl;
    mCRL2log(log::error) << pbes_system::pp(q) << std::endl;
  }
}

void complps2pbes(const std::string& input_filename,
                  const std::string& output_filename,
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
  state_formulas::state_formula formula = state_formulas::parse_state_formula(instream, spec);
  instream.close();

  pbes_system::pbes<> result = pbes_system::complps2pbes(procspec, formula);

  // save the result
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

} // namespace pbes_system

} // namespace mcrl2
