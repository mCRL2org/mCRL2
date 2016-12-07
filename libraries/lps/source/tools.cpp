// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file tools.cpp

#include <fstream>

#include "mcrl2/utilities/exception.h"
#include "mcrl2/lps/tools.h"
#include "mcrl2/lps/binary.h"
#include "mcrl2/lps/constelm.h"
#include "mcrl2/lps/detail/specification_property_map.h"
#include "mcrl2/lps/invariant_checker.h"
#include "mcrl2/lps/invelm_algorithm.h"
#include "mcrl2/lps/parelm.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/lps/remove.h"
#include "mcrl2/lps/rewrite.h"
#include "mcrl2/lps/sumelm.h"
#include "mcrl2/lps/suminst.h"
#include "mcrl2/lps/stochastic_specification.h"
#include "mcrl2/lps/untime.h"
#include "mcrl2/lps/io.h"
#include "mcrl2/utilities/logger.h"

namespace mcrl2
{

namespace lps
{

void lpsbinary(const std::string& input_filename,
               const std::string& output_filename)
{
  lps::stochastic_specification spec;
  load_lps(spec, input_filename);
  data::rewriter r(spec.data());

  lps::binary_algorithm<data::rewriter, stochastic_specification>(spec, r).run();
  save_lps(spec, output_filename);
}

void lpsconstelm(const std::string& input_filename,
                 const std::string& output_filename,
                 data::rewriter::strategy rewrite_strategy,
                 bool instantiate_free_variables,
                 bool ignore_conditions,
                 bool remove_trivial_summands,
                 bool remove_singleton_sorts
                )
{
  lps::stochastic_specification spec;
  load_lps(spec, input_filename);
  mcrl2::data::rewriter R(spec.data(), rewrite_strategy);
  lps::constelm_algorithm<data::rewriter, stochastic_specification> algorithm(spec, R);

  // preprocess: remove single element sorts
  if (remove_singleton_sorts)
  {
    algorithm.remove_singleton_sorts();
  }

  // apply constelm
  algorithm.run(instantiate_free_variables, ignore_conditions);

  // postprocess: remove trivial summands
  if (remove_trivial_summands)
  {
    algorithm.remove_trivial_summands();
  }

  save_lps(spec, output_filename);
}

void lpsinfo(const std::string& input_filename,
             const std::string& input_file_message
            )
{
  stochastic_specification spec;
  load_lps(spec, input_filename);
  lps::detail::specification_property_map<stochastic_specification> info(spec);
  std::cout << input_file_message << "\n\n";
  std::cout << info.info();
}

bool lpsinvelm(const std::string& input_filename,
               const std::string& output_filename,
               const std::string& invariant_filename,
               const std::string& dot_file_name,
               data::rewriter::strategy rewrite_strategy,
               data::detail::smt_solver_type solver_type,
               const bool no_check,
               const bool no_elimination,
               const bool simplify_all,
               const bool all_violations,
               const bool counter_example,
               const bool path_eliminator,
               const bool apply_induction,
               const int time_limit)
{
  stochastic_specification spec;
  data::data_expression invariant;

  load_lps(spec, input_filename);

  if (!invariant_filename.empty())
  {
    std::ifstream instream(invariant_filename.c_str());

    if (!instream.is_open())
    {
      throw mcrl2::runtime_error("cannot open input file '" + invariant_filename + "'");
    }

    mCRL2log(log::verbose) << "parsing input file '" <<  invariant_filename << "'..." << std::endl;

    data::variable_list& parameters=spec.process().process_parameters();
    invariant = data::parse_data_expression(instream, parameters, spec.data());

    instream.close();
  }
  else
  {
    throw mcrl2::runtime_error("A file containing an invariant must be specified using the option --invariant=INVFILE.");
  }

  if (no_check)
  {
    mCRL2log(log::warning) << "The invariant is not checked; it may not hold for this LPS." << std::endl;
  }
  else
  {
    detail::Invariant_Checker<stochastic_specification> v_invariant_checker(spec,
                                          rewrite_strategy,
                                          time_limit,
                                          path_eliminator,
                                          solver_type,
                                          apply_induction,
                                          counter_example,
                                          all_violations,
                                          dot_file_name);

    if (!v_invariant_checker.check_invariant(invariant))
    { 
      return false; // The invariant was checked and found invalid. 
    }
  }

  invelm_algorithm<stochastic_specification> algorithm(spec,
                               rewrite_strategy,
                               time_limit,
                               path_eliminator,
                               solver_type,
                               apply_induction,
                               simplify_all);
  algorithm.run(invariant, !no_elimination);
  save_lps(spec, output_filename);
  return true;
}

void lpsparelm(const std::string& input_filename,
               const std::string& output_filename
              )
{
  lps::stochastic_specification spec;
  load_lps(spec, input_filename);
  lps::parelm(spec, true);
  save_lps(spec, output_filename);
}

void lpspp(const std::string& input_filename,
           const std::string& output_filename,
           bool print_summand_numbers,
           core::print_format_type format
          )
{
  lps::stochastic_specification spec;
  load_lps(spec, input_filename);

  mCRL2log(log::verbose) << "printing LPS from "
                    << (input_filename.empty()?"standard input":input_filename)
                    << " to " << (output_filename.empty()?"standard output":output_filename)
                    << " in the " << core::pp_format_to_string(format) << " format" << std::endl;

  std::string text;
  if (format == core::print_internal)
  {
    text = to_string(specification_to_aterm(spec));
  }
  else
  {
    text = print_summand_numbers ? lps::pp_with_summand_numbers(spec) : lps::pp(spec);
  }
  if (output_filename.empty())
  {
    std::cout << text;
  }
  else
  {
    std::ofstream output_stream(output_filename.c_str());
    if (output_stream)
    {
      output_stream << text;
      output_stream.close();
    }
    else
    {
      throw mcrl2::runtime_error("could not open output file " + output_filename + " for writing");
    }
  }
}

void lpsrewr(const std::string& input_filename,
             const std::string& output_filename,
             const data::rewriter::strategy rewrite_strategy
            )
{
  stochastic_specification spec;
  load_lps(spec, input_filename);
  mcrl2::data::rewriter R(spec.data(), rewrite_strategy);
  lps::rewrite(spec, R);
  lps::remove_trivial_summands(spec);
  lps::remove_redundant_assignments(spec);
  save_lps(spec, output_filename);
}

void lpssumelm(const std::string& input_filename,
               const std::string& output_filename,
               const bool decluster)
{
  stochastic_specification spec;
  load_lps(spec, input_filename);

  sumelm_algorithm<stochastic_specification>(spec, decluster).run();

  mCRL2log(log::debug) << "Sum elimination completed, saving to " <<  output_filename << std::endl;
  save_lps(spec, output_filename);
}

void lpssuminst(const std::string& input_filename,
                const std::string& output_filename,
                const data::rewriter::strategy rewrite_strategy,
                const std::string& sorts_string,
                const bool finite_sorts_only,
                const bool tau_summands_only)
{
  stochastic_specification spec;
  load_lps(spec, input_filename);
  std::set<data::sort_expression> sorts;

  // Determine set of sorts to be expanded
  if(!sorts_string.empty())
  {
    std::vector<std::string> parts = utilities::split(utilities::remove_whitespace(sorts_string), ",");
    for (auto & part : parts)
    {
      sorts.insert(data::parse_sort_expression(part, spec.data()));
    }
  }
  else if (finite_sorts_only)
  {
    sorts = lps::finite_sorts(spec.data());
  }
  else
  {
    const std::set<data::sort_expression>& sort_set=spec.data().sorts();
    sorts = std::set<data::sort_expression>(sort_set.begin(),sort_set.end());
  }

  mCRL2log(log::verbose, "lpssuminst") << "expanding summation variables of sorts: " << data::pp(sorts) << std::endl;

  mcrl2::data::rewriter r(spec.data(), rewrite_strategy);
  lps::suminst_algorithm<data::rewriter, stochastic_specification>(spec, r, sorts, tau_summands_only).run();
  save_lps(spec, output_filename);
}

void lpsuntime(const std::string& input_filename,
               const std::string& output_filename,
               const bool add_invariants)
{
  stochastic_specification spec;
  load_lps(spec, input_filename);
  untime_algorithm<stochastic_specification>(spec, add_invariants).run();
  save_lps(spec, output_filename);
}

void txt2lps(const std::string& input_filename,
             const std::string& output_filename
            )
{
  lps::stochastic_specification spec;
  load_lps(spec, input_filename, lps_format_text());
  save_lps(spec, output_filename);
}

} // namespace lps

} // namespace mcrl2
