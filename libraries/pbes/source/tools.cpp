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
#include <fstream>
#include <sstream>

#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/detail/one_point_rule_preprocessor.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/linearise.h"
#include "mcrl2/modal_formula/state_formula.h"
#include "mcrl2/modal_formula/parse.h"
#include "mcrl2/pbes/constelm.h"
#include "mcrl2/pbes/detail/pbes_property_map.h"
#include "mcrl2/pbes/detail/pbes_parameter_map.h"
#include "mcrl2/pbes/file_formats.h"
#include "mcrl2/pbes/io.h"
#include "mcrl2/pbes/complps2pbes.h"
#include "mcrl2/pbes/lps2pbes.h"
#include "mcrl2/pbes/one_point_rule_rewriter.h"
#include "mcrl2/pbes/parelm.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/pbes_rewriter_type.h"
#include "mcrl2/pbes/tools.h"
#include "mcrl2/pbes/rewrite.h"
#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/pbes/remove_equations.h"
#include "mcrl2/pbes/txt2pbes.h"
#include "mcrl2/pbes/detail/bqnf_traverser.h"
#include "mcrl2/pbes/detail/ppg_traverser.h"
#include "mcrl2/pbes/detail/ppg_rewriter.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/text_utility.h"
#include "mcrl2/utilities/number_postfix_generator.h"

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
        atermpp::vector<propositional_variable> V = remove_unreachable_variables(p);
        mCRL2log(log::verbose) << pbes_system::detail::print_removed_equations(V);
      }
      break;
    }
    case quantifier_all:
    case quantifier_finite:
    {
      typedef pbes_system::enumerate_quantifiers_rewriter<pbes_system::pbes_expression, data::rewriter_with_variables, data::data_enumerator<> > my_pbes_rewriter;
      bool enumerate_infinite_sorts = (rewriter_type == quantifier_all);
      utilities::number_postfix_generator name_generator("UNIQUE_PREFIX");
      data::data_enumerator<> datae(p.data(), datar, name_generator);
      data::rewriter_with_variables datarv(datar);
      my_pbes_rewriter pbesr(datarv, datae, enumerate_infinite_sorts);
      pbes_constelm_algorithm<pbes_system::pbes_expression, data::rewriter, my_pbes_rewriter> algorithm(datar, pbesr);
      algorithm.run(p, compute_conditions);
      if (remove_redundant_equations)
      {
        atermpp::vector<propositional_variable> V = remove_unreachable_variables(p);
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

void pbesinfo(const std::string& input_filename,
              const std::string& input_file_message,
              pbes_file_format file_format, bool opt_full
             )
{
  pbes<> p;
  load_pbes(p, input_filename, file_format);

  pbes_system::detail::pbes_property_map info(p);

  // Show file from which PBES was read
  std::cout << input_file_message << "\n\n";

  // Show if PBES is closed and well formed
  std::cout << "The PBES is " << (p.is_closed() ? "" : "not ") << "closed and " << (p.is_well_typed() ? "" : "not ") << "well formed" << std::endl;

  // Show number of equations
  std::cout << "Number of equations: " << p.equations().size() << std::endl;

  // Show number of mu's with the predicate variables from the mu's
  std::cout << "Number of mu's:      " << info["mu_equation_count"] << std::endl;

  // Show number of nu's with the predicate variables from the nu's
  std::cout << "Number of nu's:      " << info["nu_equation_count"] << std::endl;

  // Show number of nu's with the predicate variables from the nu's
  std::cout << "Block nesting depth: " << info["block_nesting_depth"] << std::endl;

  // Show binding variables with their signature
  if (opt_full)
  {
    std::cout << "Predicate variables:\n";
    for (atermpp::vector<pbes_equation>::const_iterator i = p.equations().begin(); i != p.equations().end(); ++i)
    {
      std::cout << core::pp(i->symbol()) << "." << pbes_system::pp(i->variable()) << std::endl;
    }
  }
}

void pbesparelm(const std::string& input_filename,
                const std::string& output_filename
               )
{
  // load the pbes
  pbes<> p;
  load_pbes(p, input_filename);

  // apply the algorithm
  pbes_system::pbes_parelm_algorithm algorithm;
  algorithm.run(p);

  // save the result
  p.save(output_filename);
}

void pbespp(const std::string& input_filename,
            const std::string& output_filename,
            pbes_file_format pbes_input_format,
            core::print_format_type format
           )
{
  pbes_system::pbes<> p;
  load_pbes(p, input_filename, pbes_input_format);

  mCRL2log(log::verbose) << "printing PBES from "
                         << (input_filename.empty()?"standard input":input_filename)
                         << " to " << (output_filename.empty()?"standard output":output_filename)
                         << " in the " << core::pp_format_to_string(format) << " format" << std::endl;

  if (output_filename.empty())
  {
    if (format == core::print_internal)
    {
      std::cout << pbes_system::pbes_to_aterm(p);
    }
    else
    {
      std::cout << pbes_system::pp(p);
    }
  }
  else
  {
    std::ofstream out(output_filename.c_str());
    if (out)
    {
      if (format == core::print_internal)
      {
        out << pbes_system::pbes_to_aterm(p);
      }
      else
      {
        out << pbes_system::pp(p);
      }
      out.close();
    }
    else
    {
      throw mcrl2::runtime_error("could not open output file " + output_filename + " for writing");
    }
  }
}

void pbesrewr(const std::string& input_filename,
              const std::string& output_filename,
              const data::rewriter::strategy rewrite_strategy,
              pbes_rewriter_type rewriter_type,
              bool skip_data
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
      simplifying_rewriter<pbes_expression, data::rewriter> pbesr(datar);
      pbes_rewrite(p, pbesr);
      break;
    }
    case quantifier_all:
    {
      utilities::number_postfix_generator generator("UNIQUE_PREFIX");
      data::data_enumerator<> datae(p.data(), datar, generator);
      data::rewriter_with_variables datarv(datar);
      bool enumerate_infinite_sorts = true;
      enumerate_quantifiers_rewriter<pbes_expression, data::rewriter_with_variables, data::data_enumerator<> > pbesr(datarv, datae, enumerate_infinite_sorts, skip_data);
      pbes_rewrite(p, pbesr);
      break;
    }
    case quantifier_finite:
    {
      utilities::number_postfix_generator generator("UNIQUE_PREFIX");
      data::data_enumerator<> datae(p.data(), datar, generator);
      data::rewriter_with_variables datarv(datar);
      bool enumerate_infinite_sorts = false;
      enumerate_quantifiers_rewriter<pbes_expression, data::rewriter_with_variables, data::data_enumerator<> > pbesr(datarv, datae, enumerate_infinite_sorts, skip_data);
      pbes_rewrite(p, pbesr);
      break;
    }
    case quantifier_one_point:
    {
      // first preprocess data expressions
      data::detail::one_point_rule_preprocessor one_point_processor;
      data_rewriter<pbes_expression, data::detail::one_point_rule_preprocessor> datar(one_point_processor);
      pbes_rewrite(p, datar);

      one_point_rule_rewriter pbesr;
      pbes_rewrite(p, pbesr);

      break;
    }
    case pfnf:
    {
      pfnf_rewriter pbesr;
      pbes_rewrite(p, pbesr);
      break;
    }
    case ppg:
    {
      //bool bqnf = detail::is_bqnf(p);
      //std::clog << "bqnf_traverser says: p is " << (bqnf ? "" : "NOT ") << "in BQNF." << std::endl;
      bool ppg = detail::is_ppg(p);
      if (ppg)
      {
        mCRL2log(log::verbose) << "PBES is already a PPG." << std::endl;
      }
      else
      {
        mCRL2log(log::verbose) << "Rewriting..." << std::endl;
        pbes<> q = detail::to_ppg(p);
        mCRL2log(log::verbose) << "Rewriting done." << std::endl;
        ppg = detail::is_ppg(q);
        if (!ppg)
        {
          throw(std::runtime_error("The result PBES if not a PPG!"));
        }
        p = q;
      }
      break;
    }
    case bqnf_quantifier:
    {
      bqnf_rewriter pbesr;
      pbes_rewrite(p, pbesr);
      break;
    }
    case prover:
    default:
    {
      // Just ignore.
      assert(0);  // The PBES rewriter cannot be activated through
      // the commandline. So, we cannot end up here.
      break;
    }
  }
  p.save(output_filename);
}

void txt2pbes(const std::string& input_filename,
              const std::string& output_filename
             )
{
  pbes_system::pbes<> p;
  if (input_filename.empty())
  {
    //parse specification from stdin
    mCRL2log(log::verbose) << "reading input from stdin..." << std::endl;
    p = pbes_system::txt2pbes(std::cin);
  }
  else
  {
    //parse specification from input filename
    mCRL2log(log::verbose) << "reading input from file '" <<  input_filename << "'..." << std::endl;
    std::ifstream instream(input_filename.c_str(), std::ifstream::in|std::ifstream::binary);
    if (!instream)
    {
      throw mcrl2::runtime_error("cannot open input file: " + input_filename);
    }
    p = pbes_system::txt2pbes(instream);
    instream.close();
  }
  if (output_filename.empty())
  {
    mCRL2log(log::verbose) << "writing PBES to stdout..." << std::endl;
  }
  else
  {
    mCRL2log(log::verbose) << "writing PBES to file '" <<  output_filename << "'..." << std::endl;
  }
  p.save(output_filename);
}

} // namespace pbes_system

} // namespace mcrl2
