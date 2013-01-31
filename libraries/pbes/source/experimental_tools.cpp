// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbes/source/tools.cpp
/// \brief Tool implementations.

#include <cassert>
#include <sstream>
#include "mcrl2/data/enumerator.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/pbes/bisimulation.h"
#include "mcrl2/pbes/absinthe.h"
#include "mcrl2/pbes/abstract.h"
#include "mcrl2/pbes/detail/pbes_property_map.h"
#include "mcrl2/pbes/detail/pbes_parameter_map.h"
#include "mcrl2/pbes/eqelm.h"
#include "mcrl2/pbes/file_formats.h"
#include "mcrl2/pbes/io.h"
#include "mcrl2/pbes/normalize.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/pbesinst.h"
#include "mcrl2/pbes/pbesinst_algorithm.h"
#include "mcrl2/pbes/pbesinst_strategy.h"
#include "mcrl2/pbes/pbesinst_finite_algorithm.h"
#include "mcrl2/pbes/pbes_rewriter_type.h"
#include "mcrl2/pbes/tools.h"
#include "mcrl2/pbes/rewrite.h"
#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/pbes/remove_equations.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/text_utility.h"
#include "mcrl2/utilities/number_postfix_generator.h"
#include "mcrl2/pbes/detail/control_flow.h"
#include "mcrl2/pbes/detail/stategraph.h"
#include "mcrl2/pbes/detail/is_pfnf.h"

namespace mcrl2
{

namespace pbes_system
{

void pbesabsinthe(const std::string& input_filename,
                  const std::string& output_filename,
                  const std::string& abstraction_file,
                  absinthe_strategy strategy,
                  bool print_used_function_symbols,
                  bool enable_logging
                 )
{
  // load the pbes
  pbes<> p;
  p.load(input_filename);

  if (print_used_function_symbols)
  {
    pbes_system::detail::print_used_function_symbols(p);
  }

  std::string abstraction_text;
  if (!abstraction_file.empty())
  {
    abstraction_text = utilities::read_text(abstraction_file);
  }

  bool over_approximation = (strategy == absinthe_over);

  absinthe_algorithm algorithm;
  if (enable_logging)
  {
    algorithm.enable_logging();
  }
  algorithm.run(p, abstraction_text, over_approximation);

  // save the result
  p.save(output_filename);
}

void pbesabstract(const std::string& input_filename,
                  const std::string& output_filename,
                  const std::string& parameter_selection,
                  bool value_true
                 )
{
  // load the pbes
  pbes<> p;
  load_pbes(p, input_filename);

  // run the algorithm
  pbes_abstract_algorithm algorithm;
  pbes_system::detail::pbes_parameter_map parameter_map = pbes_system::detail::parse_pbes_parameter_map(p, parameter_selection);
  algorithm.run(p, parameter_map, value_true);

  // save the result
  p.save(output_filename);
}

void lpsbisim2pbes(const std::string& input_filename1,
                   const std::string& input_filename2,
                   const std::string& output_filename,
                   bisimulation_type type,
                   bool normalize
                  )
{
  lps::specification M;
  lps::specification S;

  M.load(input_filename1);
  S.load(input_filename2);
  pbes<> result;
  switch (type)
  {
    case strong_bisim:
      result = strong_bisimulation(M, S);
      break;
    case weak_bisim:
      result = weak_bisimulation(M, S);
      break;
    case branching_bisim:
      result = branching_bisimulation(M, S);
      break;
    case branching_sim:
      result = branching_simulation_equivalence(M, S);
      break;
  }
  if (normalize)
  {
    pbes_system::normalize(result);
  }
  result.save(output_filename);
}

bool pbesinst(const std::string& input_filename,
              const std::string& output_filename,
              pbes_file_format pbes_input_format,
              pbes_file_format pbes_output_format,
              data::rewriter::strategy rewrite_strategy,
              pbesinst_strategy m_strategy,
              const std::string& finite_parameter_selection,
              bool remove_redundant_equations,
              bool aterm_ascii
             )
{
  // load the pbes
  pbes<> p;
  load_pbes(p, input_filename, pbes_input_format);

  // data rewriter
  data::rewriter datar(p.data(), rewrite_strategy);

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
    pbesinst_finite_algorithm algorithm(rewrite_strategy);
    pbes_system::detail::pbes_parameter_map parameter_map = pbes_system::detail::parse_pbes_parameter_map(p, finite_parameter_selection);
    algorithm.run(p, parameter_map);
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
    atermpp::vector<propositional_variable> V = remove_unreachable_variables(p);
    mCRL2log(log::verbose) << pbes_system::detail::print_removed_equations(V);
  }

  // save the result
  save_pbes(p, output_filename, pbes_output_format, aterm_ascii);

  return true;
}

void pbespareqelm(const std::string& input_filename,
                  const std::string& output_filename,
                  data::rewriter::strategy rewrite_strategy,
                  pbes_rewriter_type rewriter_type,
                  bool ignore_initial_state
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
      pbes_eqelm_algorithm<pbes_system::pbes_expression, data::rewriter, my_pbes_rewriter> algorithm(datar, pbesr);
      algorithm.run(p, ignore_initial_state);
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
      pbes_eqelm_algorithm<pbes_system::pbes_expression, data::rewriter, my_pbes_rewriter> algorithm(datar, pbesr);
      algorithm.run(p, ignore_initial_state);
      break;
    }
    default:
    { }
  }

  // save the result
  p.save(output_filename);
}

void pbesstategraph(const std::string& input_filename,
                    const std::string& output_filename,
                    bool simplify,
                    bool apply_to_original,
                    bool use_pfnf_variant
                   )
{
  pbes<> p;
  load_pbes(p, input_filename);
  pbes_system::normalize(p);
  pbes<> q;

  if (use_pfnf_variant)
  {
    pbes_system::detail::control_flow_algorithm algorithm;
    q = algorithm.run(p, simplify, apply_to_original);
  }
  else
  {
    pbes_system::detail::stategraph_algorithm algorithm;
    q = algorithm.run(p, simplify, apply_to_original);
  }

  q.save(output_filename, true, true);
  if (!q.is_well_typed())
  {
    mCRL2log(log::error) << "pbesstategraph error: not well typed!" << std::endl;
    mCRL2log(log::error) << pbes_system::pp(q) << std::endl;
  }
}

} // namespace pbes_system

} // namespace mcrl2
