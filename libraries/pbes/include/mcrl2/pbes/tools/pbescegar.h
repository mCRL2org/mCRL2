// Author(s): Jore Booy
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/tools/pbescegar.h
/// \brief This file provides a tool that can simplify PBESs by
///        substituting PBES equations for variables in the rhs,
///        simplifying the result, and keeping it when it can
///        eliminate PBES variables.

// TODO: Function symbols. See abstraction_mapping_text in absinthe.h

#ifndef MCRL2_PBES_TOOLS_PBESCEGAR_H
#define MCRL2_PBES_TOOLS_PBESCEGAR_H

#include "mcrl2/core/identifier_string.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/identifier_generator.h"
#include "mcrl2/data/rewrite_strategy.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/structured_sort.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/pbes/absinthe.h"
#include "mcrl2/pbes/algorithms.h"
#include "mcrl2/pbes/builder.h"
#include "mcrl2/pbes/detail/instantiate_global_variables.h"
#include "mcrl2/pbes/detail/iteration_builders.h"
#include "mcrl2/pbes/detail/stategraph_global_algorithm.h"
#include "mcrl2/pbes/detail/stategraph_global_graph.h"
#include "mcrl2/pbes/detail/stategraph_pbes.h"
#include "mcrl2/pbes/io.h"
#include "mcrl2/pbes/pbes_equation.h"
#include "mcrl2/pbes/pbes_expression.h"
#include "mcrl2/pbes/pbesinst_lazy_counter_example.h"
#include "mcrl2/pbes/pbesinst_structure_graph.h"
#include "mcrl2/pbes/pbessolve_options.h"
#include "mcrl2/pbes/resolve_name_clashes.h"
#include "mcrl2/pbes/rewrite.h"
#include "mcrl2/pbes/solve_structure_graph.h"
#include "mcrl2/pbes/srf_pbes.h"
#include "mcrl2/pbes/tools/pbesstategraph_options.h"
#include "mcrl2/pbes/unify_parameters.h"
#include "mcrl2/utilities/logger.h"
#include <chrono>
#include <cstddef>
#include <iostream>
#include <map>
#include <set>

namespace mcrl2::pbes_system
{

struct pbescegar_options
{
  data::rewrite_strategy rewrite_strategy = data::rewrite_strategy::jitty;
};

struct pbescegar_pbes_cegar_iterator
{
  // Solves the underapproximated PBES using structure graph solving
  bool solve_underapproximation(pbes& p, pbescegar_options options)
  {
    mCRL2log(log::verbose) << "Solving underapproximated PBES using structure graph..." << std::endl;

    try
    {
      // Normalize the PBES
      algorithms::normalize(p);

      pbessolve_options options2;
      options2.rewrite_strategy = options.rewrite_strategy;

      // Build the structure graph from the PBES
      data::mutable_map_substitution<> sigma;
      sigma = pbes_system::detail::instantiate_global_variables(p);
      pbes_system::detail::replace_global_variables(p, sigma);

      mCRL2log(log::verbose) << "=== SIGMA === " << std::endl;
      mCRL2log(log::verbose) << (sigma.to_string()) << std::endl;
      structure_graph G;
      mCRL2log(log::verbose) << "=== PBES === " << std::endl;
      mCRL2log(log::verbose) << pp(p) << std::endl;
      pbesinst_structure_graph_algorithm algorithm(options2, p, G);
      algorithm.run();

      // Solve the structure graph
      bool result = solve_structure_graph(G);

      if (result)
      {
        mCRL2log(log::verbose) << "Structure graph solver returned TRUE" << std::endl;
      }
      else
      {
        mCRL2log(log::verbose) << "Structure graph solver returned FALSE" << std::endl;
      }

      return result;
    }
    catch (const std::exception& e)
    {
      mCRL2log(log::warning) << "Exception during PBES solving: " << e.what() << std::endl;
      mCRL2log(log::warning) << "Defaulting to FALSE" << std::endl;
      return false;
    }
  }

  // Iterative refinement: progressively un-abstract parameters
  void iterative_refinement(pbes& p,
    pbescegar_options options,
    const detail::stategraph_global_algorithm& algorithm,
    std::set<data::sort_expression>& sorts_to_abstract,
    std::size_t max_iterations = 10)
  {
    for (std::size_t iteration = 0; iteration < max_iterations; ++iteration)
    {
      mCRL2log(log::verbose) << "=== CEGAR Iteration " << iteration << " ===" << std::endl;
      mCRL2log(log::verbose) << "Currently abstracting " << sorts_to_abstract.size() << " sorts" << std::endl;

      // Solve current underapproximation
      bool result = solve_underapproximation(p, options);

      if (result)
      {
        mCRL2log(log::verbose) << "Underapproximation returned TRUE - problem solved!" << std::endl;
        return;
      }

      // If underapproximation is false, need to refine
      // Pick a new sort to un-abstract (add back to the PBES)
      if (sorts_to_abstract.empty())
      {
        mCRL2log(log::verbose) << "No more sorts to abstract - problem likely FALSE" << std::endl;
        return;
      }

      // Remove the first abstracted sort and re-solve
      auto sort_to_refine = *sorts_to_abstract.begin();
      sorts_to_abstract.erase(sorts_to_abstract.begin());

      mCRL2log(log::verbose) << "Refining: removing abstraction of sort " << core::pp(sort_to_refine) << std::endl;

      // Reconstruct PBES with one fewer abstracted sort and iterate
      // To do this, we need to reload the original PBES and apply abstraction
      // only with the reduced set of sorts

      mCRL2log(log::debug) << "Reconstructing PBES with reduced abstraction set..." << std::endl;
      // The current iteration removes one sort from sorts_to_abstract
      // We would need to reload the original PBES and reapply abstraction
      // This is a placeholder for the next iteration of refinement
      mCRL2log(log::verbose) << "Restarting with reduced abstraction set (not yet fully implemented)" << std::endl;
    }

    mCRL2log(log::verbose) << "Maximum iterations reached" << std::endl;
  }

  std::string tolower(const std::string& str)
  {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
  }

  void run(pbes& p, pbescegar_options options)
  {
    mCRL2log(log::verbose) << "=== CEGAR RUN STARTING ===" << std::endl;
    mCRL2log(log::verbose) << "Number of equations: " << p.equations().size() << std::endl;

    // Store original parameters for each propositional variable
    data::rewriter datar(p.data(), options.rewrite_strategy);
    std::map<core::identifier_string, data::variable_list> original_params;
    for (const auto& eq: p.equations())
    {
      original_params[eq.variable().name()] = eq.variable().parameters();
    }
    mCRL2log(log::verbose) << "Original parameters stored" << std::endl;

    // Collect control flow parameters (CFP) from the PBES structure
    // For now, we identify CFPs as parameters that appear in guards
    // and parameters that flow unchanged through all equations
    std::map<core::identifier_string, std::vector<bool>> is_cfp;

    // Initialize: assume all parameters might be CFP
    pbesstategraph_options opts;
    detail::stategraph_algorithm algo(p, opts);
    detail::stategraph_pbes stategraph(p, datar);
    simplify_data_rewriter<data::rewriter> pbes_default_rewriter(datar);

    // Preparation
    for (detail::stategraph_equation& equation: stategraph.equations())
    {
      for (detail::predicate_variable& predvar: equation.predicate_variables())
      {
        predvar.simplify_guard();
      }
    }
    mCRL2log(log::verbose) << "TARGET COPY" << std::endl;
    stategraph.compute_source_target_copy();
    mCRL2log(log::verbose) << "CFP calculation" << std::endl;
    algo.run();
    is_cfp = algo.get_GCFP();
    mCRL2log(log::verbose) << "CFP flags initialized" << std::endl;

    // Collect sorts to abstract: sorts used in non-CFP parameters
    mCRL2log(log::verbose) << "Collecting sorts to abstract..." << std::endl;
    std::set<data::sort_expression> sorts_to_abstract;
    for (const auto& eq: p.equations())
    {
      mCRL2log(log::debug) << "Processing equation: " << eq.variable().name() << std::endl;
      const data::variable_list& params = eq.variable().parameters();
      const std::vector<bool>& cfp_flags = is_cfp[eq.variable().name()];
      mCRL2log(log::debug) << "Equation has " << params.size() << " parameters" << std::endl;

      mCRL2log(log::debug) << "Equation has ";
      for (const auto& flag: cfp_flags)
      {
        mCRL2log(log::debug) << (flag ? "Y" : "N") << " ";
      }
      mCRL2log(log::debug) << "as CFP parameters" << std::endl;

      // Collect sorts of all non-CFP parameters
      std::size_t i = 0;
      for (const data::variable& param: params)
      {
        if (!cfp_flags[i])
        {
          sorts_to_abstract.insert(param.sort());
          mCRL2log(log::debug) << "  Parameter " << param << " (non-CFP) has sort " << pp(param.sort()) << std::endl;
        }
        i++;
      }
    }

    mCRL2log(log::verbose) << "Abstracting " << sorts_to_abstract.size() << " non-CFP sorts" << std::endl;

    // Create abstracted data specification with structured sorts
    mCRL2log(log::verbose) << "Creating abstracted data specification..." << std::endl;
    data::data_specification abstracted_data = p.data();
    std::map<data::sort_expression, data::sort_expression> sort_map;
    std::map<data::sort_expression, data::function_symbol> abs_constructors;
    std::string sorts_text = "sort ";
    std::string var_text = "var ";
    std::string eqn_text = "eqn ";
    std::string absmap_text = "absmap ";
    std::string absfunc_text = "absfunc ";

    data::set_identifier_generator generator;

    std::string var_bool = generator("bool");
    var_text += var_bool + ": Bool;\n";

    for (const data::sort_expression& sort: sorts_to_abstract)
    {
      std::string org_sort_name = pp(sort[0]);
      mCRL2log(log::debug) << "Processing sort for abstraction: " << pp(sort) << std::endl;

      // Create a structured sort with a single abstract constructor
      // This represents the abstraction of the entire sort into one value
      std::string abs_sort_name = "Abs_" + org_sort_name;
      std::string cons_name = "abs_" + org_sort_name;

      // Create constructor with no arguments
      std::vector<data::structured_sort_constructor> constructors;
      constructors.emplace_back(cons_name);

      data::structured_sort abs_sort(constructors);
      data::basic_sort abs_sort_basic(abs_sort_name);
      sorts_text += abs_sort_name + " = " + pp(abs_sort) + ";\n";

      // Var and eqn text
      std::string var_arb_original = generator(tolower(org_sort_name + "gen"));
      var_text += var_arb_original + " : " + org_sort_name + ";\n";
      eqn_text += "h(" + var_arb_original + ") = " + cons_name + ";\n";
      absmap_text += "h: " + org_sort_name + " -> " + abs_sort_name + ";\n";

      std::string var_new_1 = generator(tolower(abs_sort_name + "gen"));
      std::string var_new_2 = generator(tolower(abs_sort_name + "gen"));
      var_text += var_new_1 + " : " + abs_sort_name + ";\n";
      var_text += var_new_2 + " : " + abs_sort_name + ";\n";
      eqn_text += "abseq(" + var_new_1 + ", " + var_new_2 + ") = {true,false};\n";

      eqn_text += "absif(" + var_bool + ", " + var_new_1 + ", " + var_new_2 + ") = {" + cons_name + "};\n";

      absfunc_text += "if: Bool # " + org_sort_name + " # " + org_sort_name + " -> " + org_sort_name
                      + " := absif: Bool # " + abs_sort_name + " # " + abs_sort_name + " -> Set(" + abs_sort_name
                      + ")\n";
      absfunc_text += "==: " + org_sort_name + " # " + org_sort_name + " -> Bool := abseq: " + abs_sort_name + " # "
                      + abs_sort_name + " -> Set(Bool)\n";

      // Add the sort as an alias to the structured sort
      try
      {
        mCRL2log(log::debug) << "Adding alias for " << abs_sort_name << std::endl;
        // Use structured_sort.h directly to create and add the structured sort
        data::alias abs_alias(abs_sort_basic, abs_sort);
        abstracted_data.add_alias(abs_alias);

        // Get the constructor function and add it
        mCRL2log(log::debug) << "Adding constructor for " << cons_name << std::endl;
        data::function_symbol cons = constructors[0].constructor_function(abs_sort_basic);

        sort_map[sort] = abs_sort_basic;
        abs_constructors[sort] = cons;

        mCRL2log(log::debug) << "Created abstraction: " << sort << " -> " << abs_sort_name << std::endl;
      }
      catch (const std::exception& e)
      {
        mCRL2log(log::warning) << "Failed to add structured sort alias: " << e.what() << std::endl;
        // Fall back to basic sort
        mCRL2log(log::debug) << "Falling back to basic sort for " << abs_sort_name << std::endl;
        abstracted_data.add_sort(abs_sort_basic);

        // Create a function symbol for the basic sort
        data::function_symbol cons_basic(core::identifier_string(cons_name), abs_sort_basic);
        abstracted_data.add_constructor(cons_basic);

        sort_map[sort] = abs_sort_basic;
        abs_constructors[sort] = cons_basic;
      }
    }

    std::string abstraction_text
      = sorts_text + "\n" + var_text + "\n" + eqn_text + "\n" + absmap_text + "\n" + absfunc_text + "\n";
    absinthe_algorithm algorithm;
    pbes p_copy = p;
    mCRL2log(log::verbose) << "=== ABSTRACTED DATA SPECIFICATION ===" << std::endl << abstraction_text << std::endl;
    algorithm.run(p_copy, abstraction_text, false);
    bool result = solve_underapproximation(p_copy, options);

    mCRL2log(log::verbose) << "Solving completed with result: " << (result ? "TRUE" : "FALSE") << std::endl;

    // mCRL2log(log::verbose) << "Abstracted data specification created" << std::endl;
    // p.data() = abstracted_data;
    // mCRL2log(log::verbose) << "Updated PBES data specification" << std::endl;

    // Update equations: replace parameter sorts and apply A_cap
    // mCRL2log(log::verbose) << "Updating equations with abstraction..." << std::endl;
    // std::vector<pbes_equation> new_equations;
    // for (auto& eq: p.equations())
    // {
    //   mCRL2log(log::debug) << "Processing equation: " << eq.variable().name() << std::endl;
    //   data::variable_list new_params;
    //   const auto& cfp_flags = is_cfp[eq.variable().name()];

    //   auto vec = as_vector(eq.variable().parameters());
    //   std::size_t size = eq.variable().parameters().size();
    //   for (int i = size - 1; i >= 0; i--)
    //   {
    //     data::variable param = atermpp::down_cast<data::variable>(vec[i]);
    //     // Keep CFP parameters unchanged
    //     if (cfp_flags[i])
    //     {
    //       new_params.push_front(param);
    //     }
    //     else
    //     {
    //       auto it = sort_map.find(param.sort());
    //       if (it != sort_map.end())
    //       {
    //         new_params.push_front(data::variable(param.name(), it->second));
    //       }
    //       else
    //       {
    //         new_params.push_front(param);
    //       }
    //     }
    //   }

    //   propositional_variable new_var(eq.variable().name(), new_params);
    //   mCRL2log(log::debug) << "Applying A_cap to formula..." << std::endl;
    //   pbes_expression new_formula = apply_A_cap(eq.formula(), abs_constructors, sort_map, original_params);
    //   mCRL2log(log::debug) << "A_cap applied" << std::endl;
    //   pbes_equation new_eq(eq.symbol(), new_var, new_formula);
    //   new_equations.push_back(new_eq);
    // }
    //   p.equations() = new_equations;
    //   mCRL2log(log::verbose) << "Equations updated with abstraction" << std::endl;
    //   mCRL2log(log::verbose) << pp(p) << std::endl;

    //   // Analyze and prune edges based on guard evaluation
    //   mCRL2log(log::verbose) << "Analyzing and pruning edges..." << std::endl;
    //   analyze_and_prune_edges(p, datar, sort_map);
    //   mCRL2log(log::verbose) << "Edge analysis completed" << std::endl;

    //   // Solve the underapproximated PBES iteratively with refinement
    //   mCRL2log(log::verbose) << "Starting iterative CEGAR solving..." << std::endl;
    //   bool result = solve_underapproximation(p);
    //   mCRL2log(log::verbose) << "Solving completed with result: " << (result ? "TRUE" : "FALSE") << std::endl;

    //   if (result)
    //   {
    //     mCRL2log(log::verbose) << "Underapproximation returned true - problem solved!" << std::endl;
    //   }
    //   else
    //   {
    //     mCRL2log(log::verbose) << "Underapproximation returned false - need refinement" << std::endl;
    //     mCRL2log(log::verbose) << "Note: Full iterative refinement loop not yet enabled" << std::endl;
    //   }

    //   mCRL2log(log::verbose) << "=== CEGAR RUN COMPLETED ===" << std::endl;
  }
};

inline void pbescegar(const std::string& input_filename,
  const std::string& output_filename,
  const utilities::file_format& input_format,
  const utilities::file_format& output_format,
  pbescegar_options options)
{
  mCRL2log(log::verbose) << "=== PBESCEGAR STARTING ===" << std::endl;

  pbes p;
  mCRL2log(log::verbose) << "Loading PBES from " << input_filename << std::endl;
  load_pbes(p, input_filename, input_format);
  mCRL2log(log::verbose) << "PBES loaded successfully. Equations: " << p.equations().size() << std::endl;

  mCRL2log(log::verbose) << "Normalizing PBES..." << std::endl;
  algorithms::normalize(p);
  mCRL2log(log::verbose) << "PBES normalized successfully" << std::endl;

  mCRL2log(log::verbose) << "Running CEGAR iterator..." << std::endl;
  pbescegar_pbes_cegar_iterator iterator;
  iterator.run(p, options);
  mCRL2log(log::verbose) << "CEGAR iterator completed" << std::endl;

  mCRL2log(log::verbose) << "Saving PBES to " << output_filename << std::endl;
  save_pbes(p, output_filename, output_format);
  mCRL2log(log::verbose) << "PBES saved successfully" << std::endl;

  mCRL2log(log::verbose) << "=== PBESCEGAR COMPLETED ===" << std::endl;
}
}; // namespace mcrl2::pbes_system

#endif // MCRL2_PBES_TOOLS_PBECEGAR_H
