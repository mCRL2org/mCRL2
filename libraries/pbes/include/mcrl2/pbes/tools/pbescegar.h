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
#include "mcrl2/data/container_sort.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/identifier_generator.h"
#include "mcrl2/data/rewrite_strategy.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/sort_expression.h"
#include "mcrl2/data/structured_sort.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/pbes/absinthe.h"
#include "mcrl2/pbes/algorithms.h"
#include "mcrl2/pbes/detail/instantiate_global_variables.h"
#include "mcrl2/pbes/detail/pbessolve_algorithm.h"
#include "mcrl2/pbes/detail/stategraph_global_algorithm.h"
#include "mcrl2/pbes/detail/stategraph_pbes.h"
#include "mcrl2/pbes/io.h"
#include "mcrl2/pbes/pbes_equation.h"
#include "mcrl2/pbes/pbesinst_structure_graph.h"
#ifdef MCRL2_ENABLE_SYLVAN
#include "mcrl2/pbes/pbesreach.h"
#include "mcrl2/pbes/tools/pbesstategraph_options.h"
#endif
#include "mcrl2/pbes/pbessolve_options.h"
#include "mcrl2/pbes/rewriters/dataspec_prune_rewriter.h"
#include "mcrl2/pbes/rewriters/one_point_rule_rewriter.h"
#include "mcrl2/pbes/rewriters/quantifiers_inside_rewriter.h"
#include "mcrl2/pbes/solve_structure_graph.h"
#include "mcrl2/utilities/exception.h"
#include "mcrl2/utilities/logger.h"
#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/container/flat_map.hpp>
#include <boost/filesystem.hpp>
#include <boost/process.hpp>
#include <boost/process/search_path.hpp>
#include <cstddef>
#include <iostream>
#include <map>
#include <set>

namespace bp = boost::process;

namespace mcrl2::pbes_system
{

struct pbescegar_options
{
  data::rewrite_strategy rewrite_strategy = data::rewrite_strategy::jitty;
  bool init_control_flow = false;
  bool solve_symbolic = false;
  std::string solve_symbolic_args = "";
};

srf_pbes tosrf(pbes_system::pbes pbesspec)
{
  pbes_system::detail::instantiate_global_variables(pbesspec);
  auto result = pbes2pre_srf(pbesspec, true);
  // Unify the parameters of the original PBES (which has potential counter example information)
  unify_parameters(result, true, false);
  pbes_system::resolve_summand_variable_name_clashes(result,
    result.equations().front().variable().parameters()); // N.B. This is a required preprocessing step.
  return pre_srf2srfpbes(result);
}

struct pbescegar_pbes_cegar_iterator
{
private:
  utilities::indexed_set<data::data_expression> m_values;

  bp::child sym_process;

public:
  bool solve(const pbes& p, pbescegar_options options)
  {
    data::rewriter datar(p.data(), options.rewrite_strategy);
    // simplify_quantifiers_data_rewriter<data::rewriter> pbes_default_rewriter(datar);
    pbes p_copy(p);
    utilities::execution_timer timer;
    timer.start("preprocessing");
    mCRL2log(log::debug) << "MY BEFORE" << pp(p) << "MY END" << std::endl;
    simplify_data_rewriter<data::rewriter> pbesr(datar);
    dataspec_prune_rewriter rewr;
    mCRL2log(log::debug) << "Normal rewr" << std::endl;
    pbes_rewrite(p_copy, pbesr);
    mCRL2log(log::debug) << "Data spec rewr" << std::endl;
    p_copy = rewr(p_copy);
    quantifiers_inside_rewriter pbesr_inside;
    mCRL2log(log::debug) << "Inside" << std::endl;
    pbes_rewrite(p_copy, pbesr_inside);
    one_point_rule_rewriter pbesr_one_point;
    mCRL2log(log::debug) << "One point" << std::endl;
    replace_pbes_expressions(p_copy, pbesr_one_point, false);
    mCRL2log(log::debug) << "Regular" << std::endl;
    pbes_rewrite(p_copy, pbesr);
    mCRL2log(log::debug) << "Data prune" << std::endl;
    p_copy = rewr(p_copy);
    timer.finish("preprocessing");

    mCRL2log(log::debug) << "MY APPROX" << pp(p_copy) << "MY END" << std::endl;

    bool result = false;
    timer.start("solving approximation");
    if (options.solve_symbolic)
    {
      try
      {
        bp::ipstream output_sym_stream;
        bp::opstream input_sym_stream;
        mCRL2log(log::debug) << "Solving symbolic with args: " << options.solve_symbolic_args << std::endl;
        sym_process = bp::child(("pbessolvesymbolic - " + options.solve_symbolic_args),
          bp::std_in<input_sym_stream, bp::std_out> output_sym_stream);

        std::ostringstream buffer(std::ios::binary);
        atermpp::binary_aterm_ostream(buffer) << p_copy;

        const std::string& data = buffer.str();
        input_sym_stream.write(data.data(), data.size());

        input_sym_stream.flush();

        std::vector<std::string> outline;
        std::string line;
        while (sym_process.running() && std::getline(output_sym_stream, line))
        {
          mCRL2log(log::debug) << "[symbolic]: " << line << std::endl;
          outline.push_back(line);
        }
        mCRL2log(log::verbose) << "Result: " << outline.back() << std::endl;
        sym_process.wait();

        result = outline.back() == "true";
      }
      catch (const std::exception& e)
      {
        sym_process.wait();
        mcrl2::runtime_error("symbolic solver failed: " + std::string(e.what()));
      }
    }
    else
    {
      pbessolve_options options2;
      options2.rewrite_strategy = options.rewrite_strategy;

      structure_graph G;
      pbesinst_structure_graph_algorithm algorithm(options2, p_copy, G);
      algorithm.run();

      // Solve the structure graph
      result = solve_structure_graph(G);
      mCRL2log(log::verbose) << "Structure graph solver returned " << (result ? "TRUE" : "FALSE") << std::endl;
    }
    timer.finish("solving approximation");
    if (mcrl2::log::mCRL2logEnabled(log::verbose))
    {
      timer.report();
    }
    return result;
  }

  // Solves the underapproximated PBES using structure graph solving
  bool solve_approximation(const pbes& p,
    pbescegar_options options,
    const std::string& abstraction_text,
    const bool& is_overapproximation)
  {
    mCRL2log(log::verbose) << "Calculating " << (is_overapproximation ? "over" : "under") << "approximation"
                           << std::endl;

    data::mutable_map_substitution<> sigma;
    pbes p_copy(p);
    sigma = pbes_system::detail::instantiate_global_variables(p_copy);
    pbes_system::detail::replace_global_variables(p_copy, sigma);

    absinthe_algorithm algorithm;
    algorithm.run(p_copy, abstraction_text, is_overapproximation);

    try
    {
      mCRL2log(log::verbose) << "Solving " << (is_overapproximation ? "over" : "under") << "approximated PBES"
                             << std::endl;
      return solve(p_copy, options);
    }
    catch (const std::exception& e)
    {
      throw mcrl2::runtime_error("Exception during structure graph solving: " + std::string(e.what()));
    }
  }

  // Helper: Calculate Control Flow Parameters (CFP)
  std::map<core::identifier_string, std::vector<bool>>
  calculate_cfp(pbes& p, pbescegar_options& options, const bool use_init_control_flow)
  {
    std::map<core::identifier_string, std::vector<bool>> is_cfp;
    if (!use_init_control_flow)
    {
      for (pbes_equation& equation: p.equations())
      {
        is_cfp[equation.variable().name()] = std::vector<bool>(equation.variable().parameters().size(), false);
      }
      return is_cfp;
    }
    data::rewriter datar(p.data(), options.rewrite_strategy);
    detail::stategraph_pbes stategraph(p, datar);
    pbesstategraph_options opts;
    detail::stategraph_algorithm algo(p, opts);
    for (detail::stategraph_equation& equation: stategraph.equations())
    {
      is_cfp[equation.variable().name()] = std::vector<bool>(equation.parameters().size(), false);
      for (detail::predicate_variable& predvar: equation.predicate_variables())
      {
        predvar.simplify_guard();
      }
    }
    return is_cfp;
    stategraph.compute_source_target_copy();
    algo.run();
    is_cfp = algo.get_GCFP();

    return is_cfp;
  }

  // Helper: Collect sorts of non-CFP parameters
  std::set<data::sort_expression> collect_sorts_to_abstract(pbes& p,
    const std::map<core::identifier_string, std::vector<bool>>& is_cfp)
  {
    mCRL2log(log::debug) << "Collecting sorts to abstract..." << std::endl;
    std::set<data::sort_expression> sorts_to_abstract;

    for (const auto& eq: p.equations())
    {
      mCRL2log(log::debug) << "Processing equation: " << eq.variable().name() << std::endl;
      const data::variable_list& params = eq.variable().parameters();
      const std::vector<bool>& cfp_flags = is_cfp.at(eq.variable().name());
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

    mCRL2log(log::debug) << "Abstracting " << sorts_to_abstract.size() << " non-CFP sorts" << std::endl;
    return sorts_to_abstract;
  }

  std::string sanitize(const std::string& term)
  {
    std::string result = term;
    result.erase(std::remove(result.begin(), result.end(), '('), result.end());
    result.erase(std::remove(result.begin(), result.end(), ')'), result.end());
    return result;
  }

  // Helper: Create abstracted data specification with abstract constructors
  std::string create_abstraction_specification(const pbes& p, const std::set<data::sort_expression>& sorts_to_abstract)
  {
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
      std::string org_sort_name = pp(sort);
      // mCRL2log(log::verbose) << "Processing sort for abstraction: " << pp(sort[0]) << std::endl;
      mCRL2log(log::debug) << "Processing sort for abstraction: " << pp(sort) << std::endl;

      // Create a structured sort with a single abstract constructor
      // Remove any parentheses from the sort name
      std::string san_org_sort_name = sanitize(org_sort_name);
      std::string abs_sort_name = "Abs_" + san_org_sort_name;
      std::string cons_name = "abs_" + san_org_sort_name;

      std::vector<data::structured_sort_constructor> constructors;
      constructors.emplace_back(cons_name);

      data::structured_sort abs_sort(constructors);
      data::basic_sort abs_sort_basic(abs_sort_name);
      sorts_text += abs_sort_name + " = " + pp(abs_sort) + ";\n";

      // Var and eqn text
      std::string var_arb_original = generator(tolower(san_org_sort_name + "gen"));
      var_text += var_arb_original + " : " + org_sort_name + ";\n";
      eqn_text += "h(" + var_arb_original + ") = " + cons_name + ";\n";
      absmap_text += "h: " + org_sort_name + " -> " + abs_sort_name + ";\n";

      std::string var_new_1 = generator(tolower(san_org_sort_name + "gen"));
      std::string var_new_2 = generator(tolower(san_org_sort_name) + "gen");
      var_text += var_new_1 + " : " + abs_sort_name + ";\n";
      var_text += var_new_2 + " : " + abs_sort_name + ";\n";
      eqn_text += "abseq(" + var_new_1 + ", " + var_new_2 + ") = {true,false};\n";
      // eqn_text += "" + var_new_1 + " == " + var_new_2 + " = true;\n";

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
    mCRL2log(log::debug) << "  Abstraction text: " << abstraction_text << std::endl;
    return abstraction_text;
  }

  // Iterative refinement: progressively un-abstract parameters
  void update_parameters(const pbes& p, std::set<data::sort_expression>& sorts_to_abstract)
  {
    mCRL2log(log::verbose) << "Updating parameters for refinement..." << std::endl;

    // Find the first parameter in the first equation that is not yet enabled
    for (const pbes_equation& eq: p.equations())
    {
      mCRL2log(log::debug) << "for loop" << std::endl;
      const std::vector<data::variable>& original_vars = as_vector(eq.variable().parameters());

      mCRL2log(log::debug) << "Checking equation: " << eq.variable().name() << std::endl;

      // Iterate through parameters
      for (std::size_t i = 0; i < original_vars.size(); ++i)
      {
        const data::variable& param = atermpp::down_cast<data::variable>(original_vars[i]);

        // Check if this parameter is CFP (control flow) - if so, skip
        if (sorts_to_abstract.find(param.sort()) != sorts_to_abstract.end())
        {
          mCRL2log(log::verbose) << "Enabling parameter " << i << " (" << param << ") in equation "
                                 << eq.variable().name() << std::endl;
          mCRL2log(log::debug) << "  Sort: " << pp(param.sort()) << std::endl;

          // Remove this sort from sorts_to_abstract
          sorts_to_abstract.erase(param.sort());
          mCRL2log(log::debug) << "Removed sort " << pp(param.sort()) << " from abstraction" << std::endl;
          mCRL2log(log::verbose) << "Remaining sorts to abstract: " << pp(sorts_to_abstract) << std::endl;
          return;
        }
      }
    }
  }

  void report_abstracted_sorts(const std::set<data::sort_expression>& sorts_to_abstract)
  {
    mCRL2log(log::verbose) << "Abstracted sorts: " << pp(sorts_to_abstract) << std::endl;
  }

  std::string tolower(const std::string& str)
  {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
  }

  static bool is_even(data::sort_expression i)
  {
    mCRL2log(log::debug) << "is_even: " << pp(i[0]) << std::endl;
    return pp(i[0]) == "ControlPhase" || pp(i[0]) == "SingleLight" || pp(i[0]) == "DoubleLight" || pp(i[0]) == "Enum3";
    // || pp(i[0]) == "Colour" || pp(i[0]) == "Sluice";
  }

  bool run(pbes& p, pbescegar_options options)
  {
    // Calculate Control Flow Parameters
    std::map<core::identifier_string, std::vector<bool>> is_cfp = calculate_cfp(p, options, options.init_control_flow);

    // Collect sorts to abstract (non-CFP parameters)
    std::set<data::sort_expression> sorts_to_abstract = collect_sorts_to_abstract(p, is_cfp);
    sorts_to_abstract.erase(data::sort_nat::nat());
    sorts_to_abstract.erase(data::sort_pos::pos());
    sorts_to_abstract.erase(data::sort_bool::bool_());
    sorts_to_abstract.erase(data::sort_real::real_());
    sorts_to_abstract.erase(data::sort_list::list(data::sort_bool::bool_()));
    sorts_to_abstract.erase(data::sort_list::list(data::sort_nat::nat()));
    sorts_to_abstract.erase(data::sort_list::list(data::sort_real::real_()));
    auto it = std::find_if(sorts_to_abstract.begin(), sorts_to_abstract.end(), is_even);
    while (it != sorts_to_abstract.end())
    {
      sorts_to_abstract.erase(it);
      it = std::find_if(sorts_to_abstract.begin(), sorts_to_abstract.end(), is_even);
    }
    mCRL2log(log::verbose) << "Abstracted sorts: " << pp(sorts_to_abstract) << std::endl;
    bool tried_all = false;
    do
    {
      if (sorts_to_abstract.size() == 0)
      {
        mCRL2log(log::verbose) << "No sorts to abstract, solving normally." << std::endl;
        return solve(p, options);
      }

      // Create abstraction specification and solve
      std::string abstraction_text = create_abstraction_specification(p, sorts_to_abstract);

      bool under_result = solve_approximation(p, options, abstraction_text, false);
      if (under_result)
      {
        report_abstracted_sorts(sorts_to_abstract);
        return true;
      }
      bool over_result = solve_approximation(p, options, abstraction_text, true);
      if (!over_result)
      {
        report_abstracted_sorts(sorts_to_abstract);
        return false;
      }

      tried_all = sorts_to_abstract.size() == 0;

      update_parameters(p, sorts_to_abstract);
    }
    while (!tried_all);

    throw mcrl2::runtime_error("Could not find a solution");
  }
};

inline bool pbescegar(const std::string& input_filename,
  const utilities::file_format& input_format,
  const pbescegar_options options)
{
  pbes p;
  load_pbes(p, input_filename, input_format);
  algorithms::normalize(p);

  // TODO: Error when encountering implication
  pbescegar_pbes_cegar_iterator iterator;
  bool result = iterator.run(p, options);

  // mCRL2log(log::verbose) << "Saving PBES to " << output_filename << std::endl;
  // save_pbes(p, output_filename, output_format);
  // mCRL2log(log::verbose) << "PBES saved successfully" << std::endl;

  mCRL2log(log::info) << (result ? "true" : "false") << std::endl;
  return result;
}
}; // namespace mcrl2::pbes_system

#endif // MCRL2_PBES_TOOLS_PBECEGAR_H
