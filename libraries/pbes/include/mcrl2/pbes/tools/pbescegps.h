// Author(s): Jore Booy
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/tools/pbescegps.h
/// \brief This file provides a tool that can simplify PBESs by
///        substituting PBES equations for variables in the rhs,
///        simplifying the result, and keeping it when it can
///        eliminate PBES variables.

#ifndef MCRL2_PBES_TOOLS_PBESCEGPS_H
#define MCRL2_PBES_TOOLS_PBESCEGPS_H

#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/data/bool.h"
#include "mcrl2/data/container_sort.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/identifier_generator.h"
#include "mcrl2/data/rewrite_strategy.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/sort_expression.h"
#include "mcrl2/data/structured_sort.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/pbes/algorithms.h"
#include "mcrl2/pbes/builder.h"
#include "mcrl2/pbes/detail/instantiate_global_variables.h"
#include "mcrl2/pbes/detail/pbessolve_algorithm.h"
#include "mcrl2/pbes/detail/stategraph_global_algorithm.h"
#include "mcrl2/pbes/detail/stategraph_pbes.h"
#include "mcrl2/pbes/find.h"
#include "mcrl2/pbes/io.h"
#include "mcrl2/pbes/parelm.h"
#include "mcrl2/pbes/pbes_equation.h"
#include "mcrl2/pbes/pbes_expression.h"
#include "mcrl2/pbes/pbesinst_structure_graph.h"
#include "mcrl2/pbes/propositional_variable.h"
#include "mcrl2/pbes/rewrite.h"
#include <algorithm>
#include <ostream>
#ifdef MCRL2_ENABLE_SYLVAN
#include "mcrl2/pbes/pbesreach.h"
#include "mcrl2/pbes/tools/pbesstategraph_options.h"
#endif
#include "mcrl2/pbes/pbessolve_options.h"
#include "mcrl2/pbes/rewriters/dataspec_prune_rewriter.h"
#include "mcrl2/pbes/solve_structure_graph.h"
#include "mcrl2/utilities/exception.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/execution_timer.h"
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
#include <ranges>
#include <set>
#include <vector>

namespace bp = boost::process;

namespace mcrl2::pbes_system
{
// Abstraction builder for pbescegps - applies data abstraction to PBES expressions
template <typename Dummy = void>
struct abstraction_rewriter : public pbes_expression_builder<abstraction_rewriter<Dummy>>
{
  using super = pbes_expression_builder<abstraction_rewriter<Dummy>>;
  using super::apply;

  std::map<core::identifier_string, std::set<int>> m_pbes_parameters_abstraction_indices;
  const std::set<data::variable>& m_abstraction_vars;
  bool m_is_overapproximation;

  abstraction_rewriter( const std::set<data::variable>& vars, const std::map<core::identifier_string, std::set<int>>& pbes_parameters_abstraction_indices, bool is_over)
    : m_pbes_parameters_abstraction_indices(pbes_parameters_abstraction_indices), m_abstraction_vars(vars), m_is_overapproximation(is_over)
  {
  }

  template <class T>
  void apply(T& result, const data::data_expression& x)
  {
    mCRL2log(log::debug) << "Processing data expression: " << x << std::endl;
    
    // Check if it's a variable using built-in function
    if (data::is_variable(x))
    {
      const data::variable& var = atermpp::down_cast<data::variable>(x);
      mCRL2log(log::debug) << "  -> Handling as data::variable: " << var.name() << std::endl;
      
      if (m_abstraction_vars.count(var) > 0)
      {
        mCRL2log(log::debug) << "     Abstracting variable " << var.name() 
                               << " to " << (m_is_overapproximation ? "true" : "false") << std::endl;
        data::data_expression abstracted_val = m_is_overapproximation ? data::true_() : data::false_();
        result = pbes_expression(abstracted_val);
      }
      else
      {
        // Variable is not abstracted, return as-is
        mCRL2log(log::debug) << "     Keeping variable " << var.name() << " as-is" << std::endl;
        result = pbes_expression(var);
      }
      return;
    }
    
    // Check if it's an application using built-in function
    if (data::is_application(x))
    {
      const data::application& app = atermpp::down_cast<data::application>(x);
      mCRL2log(log::debug) << "  -> Handling as data::application" << std::endl;
      
      // First check if any free variables depend on abstracted variables
      std::set<data::variable> free_vars = pbes_system::find_free_variables(app);
      
      bool depends_on_abstracted = false;
      for (const auto& var : free_vars)
      {
        if (m_abstraction_vars.count(var) > 0)
        {
          depends_on_abstracted = true;
          break;
        }
      }
      
      if (!depends_on_abstracted)
      {
        mCRL2log(log::debug) << "     Application does not depend on abstracted variables, keeping as-is" << std::endl;
        result = pbes_expression(app);
        return;
      }
      
      mCRL2log(log::debug) << "     Application depends on abstracted variables" << std::endl;
      
      // Expression depends on abstracted variables
      // Check if the function is monotonic or anti-monotonic
      bool is_monotonic = false;
      bool is_anti_monotonic = false;
      
      // Check for known monotonic functions using predicate functions
      if (data::sort_bool::is_and_application(app) ||
          data::sort_bool::is_or_application(app))
      {
        is_monotonic = true;
        mCRL2log(log::debug) << "     Function is monotonic (and/or)" << std::endl;
      }
      // Check for known anti-monotonic functions (implication in antecedent)
      else if (data::sort_bool::is_not_application(app))
      {
        // Implication is monotonic in consequent, anti-monotonic in antecedent
        // For now, treat conservatively
        is_anti_monotonic = true;
        mCRL2log(log::debug) << "     Function is anti-monotonic (not)" << std::endl;
      }
      // For unknown functions that depend on abstracted variables, use conservative approach
      else
      {
        // Conservative: return true/false
        mCRL2log(log::debug) << "     Unknown function, using conservative approach, returning " 
                               << (m_is_overapproximation ? "true" : "false") << std::endl;
        data::data_expression conservative_val = m_is_overapproximation ? data::true_() : data::false_();
        result = pbes_expression(conservative_val);
        return;
      }
      
      // Apply appropriate abstraction to arguments
      data::data_expression_list abstracted_args;
      for (const auto& arg : app)
      {
        pbes_expression abstracted_arg_expr;
        
        if (is_monotonic)
        {
          // Apply same abstraction mode to all arguments
          mCRL2log(log::debug) << "     Abstracting argument with monotonic mode" << std::endl;
          apply(abstracted_arg_expr, arg);
        }
        else if (is_anti_monotonic)
        {
          // Flip abstraction mode for arguments
          mCRL2log(log::debug) << "     Flipping abstraction mode for anti-monotonic argument" << std::endl;
          m_is_overapproximation = !m_is_overapproximation;
          apply(abstracted_arg_expr, arg);
          m_is_overapproximation = !m_is_overapproximation;
        }
        
        // Extract the data expression from the pbes_expression
        const data::data_expression& data_expr = atermpp::down_cast<data::data_expression>(abstracted_arg_expr);
        abstracted_args.push_front(data_expr);
      }
      
      // Reverse the arguments back to original order
      std::vector<data::data_expression> args_vec;
      for (const auto& arg : abstracted_args)
      {
        args_vec.push_back(arg);
      }
      abstracted_args = data::data_expression_list();
      for (auto it = args_vec.rbegin(); it != args_vec.rend(); ++it)
      {
        abstracted_args.push_front(*it);
      }
      
      // Reconstruct the application with abstracted arguments
      data::data_expression reconstructed = data::application(app.head(), abstracted_args);
      result = pbes_expression(reconstructed);
      return;
    }
    
    // For any other data expression type (e.g., function symbols), use the default builder behavior
    mCRL2log(log::debug) << "  -> Using default builder behavior (not a variable or application)" << std::endl;
    super::apply(result, x);
  }

  template <class T>
  void apply(T& result, const and_& x)
  {
    mCRL2log(log::debug) << "Processing PBES conjunction" << std::endl;
    pbes_expression left;
    pbes_expression right;
    super::apply(left, x.left());
    super::apply(right, x.right());
    make_and_(result, left, right);
  }

  template <class T>
  void apply(T& result, const or_& x)
  {
    mCRL2log(log::debug) << "Processing PBES disjunction" << std::endl;
    pbes_expression left;
    pbes_expression right;
    super::apply(left, x.left());
    super::apply(right, x.right());
    make_or_(result, left, right);
  }

  template <class T>
  void apply(T& result, const forall& x)
  {
    mCRL2log(log::debug) << "Processing PBES forall" << std::endl;
    pbes_expression body;
    super::apply(body, x.body());
    result = make_forall_(x.variables(), body);
  }

  template <class T>
  void apply(T& result, const exists& x)
  {
    mCRL2log(log::debug) << "Processing PBES exists" << std::endl;
    pbes_expression body;
    super::apply(body, x.body());
    result = make_exists_(x.variables(), body);
  }

  template <class T>
  void apply(T& result, const propositional_variable_instantiation& x)
  {
    mCRL2log(log::debug) << "Processing PBES propositional variable instantiation: " << x.name() << std::endl;
    std::vector<data::data_expression> filtered_args_vec;
    
    std::size_t i = 0;
    for (auto it = x.parameters().begin(); it != x.parameters().end(); ++it, ++i)   {
      if (!m_pbes_parameters_abstraction_indices[x.name()].contains(i))
      {
          filtered_args_vec.push_back(atermpp::down_cast<data::data_expression>(*it));
      }
    }
    // Convert vector back to data_expression_list
    data::data_expression_list args;
    for (auto & it : std::ranges::reverse_view(filtered_args_vec))
    {
      args.push_front(it);
    }
    result = propositional_variable_instantiation(x.name(), args);
  }
  
  template <class T>
  void apply(T& result, const not_& x)
  {
      mCRL2log(log::debug) << "Processing PBES not (negation)" << std::endl;
      throw mcrl2::runtime_error("Not is not supported in this tool");
    m_is_overapproximation = !m_is_overapproximation;
    pbes_expression operand;
    super::apply(operand, x.operand());
    m_is_overapproximation = !m_is_overapproximation;
    make_not_(result, operand);
  }

  // Override imp: not allowed after normalization
  template <class T>
  void apply(T& result, const imp& x)
  {
    (void)result;
    (void)x;
    throw mcrl2::runtime_error("apply_abstraction encountered implication; PBES should be normalized");
  }
};
} // namespace mcrl2::pbes_system


namespace mcrl2::pbes_system
{

struct pbescegps_options
{
  data::rewrite_strategy rewrite_strategy = data::rewrite_strategy::jitty;
  bool init_control_flow = false;
  bool solve_symbolic = false;
  std::string solve_symbolic_args = "";
};

struct pbescegps_iterator
{
private:
  utilities::indexed_set<data::data_expression> m_values;

  bp::child sym_process;

public:
  bool solve(const pbes& p, pbescegps_options options)
  {
    pbes p_copy(p);
    utilities::execution_timer timer;

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
    pbescegps_options options,
    const bool& is_overapproximation)
  {
    mCRL2log(log::verbose) << "Calculating " << (is_overapproximation ? "over" : "under") << "approximation"
                           << std::endl;

    data::mutable_map_substitution<> sigma;
    pbes p_copy(p);
    sigma = pbes_system::detail::instantiate_global_variables(p_copy);
    pbes_system::detail::replace_global_variables(p_copy, sigma);

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

  // Collects all parameters W = decl(E) from a PBES
  // This gathers all data variables that appear in PBES equations
  std::set<data::variable> collect_parameters(const pbes& p)
  {
    std::set<data::variable> parameters;
    for (const pbes_equation& eq : p.equations())
    {
      for (const auto& param : eq.variable().parameters())
      {
        parameters.insert(atermpp::down_cast<data::variable>(param));
      }
    }
    return parameters;
  }

  // Calculate the indices of the parameters to abstract
  // Maps equation names to sets of parameter indices that should be abstracted
  std::map<core::identifier_string, std::set<int>> calculate_parameter_abstraction_indices(
    const pbes& p,
    const std::map<core::identifier_string, std::set<data::variable>>& abstraction_vars_per_eq)
  {
    std::map<core::identifier_string, std::set<int>> pbes_parameters_abstraction_indices;
    for (const auto& eq : p.binding_variables())
    {
      pbes_parameters_abstraction_indices[eq.name()] = std::set<int>();
      int i = 0;
      for (auto it = eq.parameters().begin(); it != eq.parameters().end(); ++it, ++i)
      {
        if (abstraction_vars_per_eq.at(eq.name()).contains(*it))
        {
          pbes_parameters_abstraction_indices[eq.name()].insert(i);
        }
      }
    }
    return pbes_parameters_abstraction_indices;
  }

  // // Applies abstraction to a PBES expression
  // // Replaces data expressions depending on abstracted variables with true/false
  pbes_expression apply_abstraction(const pbes_expression& expr,
    const std::set<data::variable>& abstraction_vars,
    const std::map<core::identifier_string, std::set<int>>& pbes_parameters_abstraction_indices,
    bool is_overapproximation);

  // Applies abstraction to all equations in a PBES
  pbes apply_abstraction_to_pbes(const pbes& p,
    std::map<core::identifier_string, std::set<data::variable>>& abstraction_vars_per_eq,
    const std::map<core::identifier_string, std::set<int>>& pbes_parameters_abstraction_indices,
    bool is_overapproximation,
    pbescegps_options options)
  {
    pbes result = p;
    
    // TODO: Filter the parameters of the original PBES
    std::vector<pbes_equation> new_equations;
    for (const pbes_equation& eq : result.equations())
    {
      propositional_variable bnd_var = eq.variable();
      data::variable_list filtered_params;
      std::size_t i = 0;
      for (auto it = bnd_var.parameters().begin(); it != bnd_var.parameters().end(); ++it, ++i)   {
        if (!abstraction_vars_per_eq[bnd_var.name()].contains(*it))
        {
        mCRL2log(log::debug) << pp(*it) << std::endl;
          filtered_params.push_front(atermpp::down_cast<data::variable>(*it));
        }
      }
      propositional_variable new_bnd_var(bnd_var.name(), filtered_params);
      pbes_equation new_eq(eq.symbol(), new_bnd_var, eq.formula());
      new_equations.push_back(new_eq);
    }
    result.equations() = new_equations;
    
    for (pbes_equation& eq : result.equations())
    {
      // Find abstraction set for this equation
      auto it = abstraction_vars_per_eq.find(eq.variable().name());
      if (it != abstraction_vars_per_eq.end())
      {
        eq.formula() = apply_abstraction(eq.formula(), it->second, pbes_parameters_abstraction_indices, is_overapproximation);
      }
    }

    mCRL2log(log::debug) << pp(result) << std::endl;
    // // Remove unused parameters after abstraction
    // // Temporarily suppress logging during parelm to reduce noise
    // mcrl2::log::log_level_t saved_level = mcrl2::log::logger::get_reporting_level();
    // mcrl2::log::logger::set_reporting_level(mcrl2::log::error);
    // pbes_system::parelm(result, false);
    // mcrl2::log::logger::set_reporting_level(saved_level);

    // Rewrite expressions for simplification
    data::rewriter datar(p.data(), options.rewrite_strategy);
    simplify_data_rewriter<data::rewriter> pbesr(datar);
    pbes_rewrite(result, pbesr);

    return result;
  }

  // Helper: Calculate non-Control Flow Parameters (CFP) per equation
  // Returns a map from equation name to set of non-CFP variables
  std::map<core::identifier_string, std::set<data::variable>>
  calculate_non_cfp(pbes& p, pbescegps_options& options, const bool use_init_control_flow)
  {
    // Initialize result with ALL parameters for each equation
    std::map<core::identifier_string, std::set<data::variable>> result;
    for (const pbes_equation& eq : p.equations())
    {
      for (const auto& param : eq.variable().parameters())
      {
        result[eq.variable().name()].insert(atermpp::down_cast<data::variable>(param));
      }
    }
    
    if (!use_init_control_flow)
    {
      return result;
    }

    data::rewriter datar(p.data(), options.rewrite_strategy);
    detail::stategraph_pbes stategraph(p, datar);
    pbesstategraph_options opts;
    detail::stategraph_algorithm algo(p, opts);

    for (detail::stategraph_equation& equation: stategraph.equations())
    {
      for (detail::predicate_variable& predvar: equation.predicate_variables())
      {
        predvar.simplify_guard();
      }
    }

    stategraph.compute_source_target_copy();
    algo.run();

    // Get the GCFP vector for each equation
    const auto& gcfp_map = algo.get_GCFP();

    for (const auto& [eq_name, cfp_vector] : gcfp_map)
    {
      // Find the corresponding equation to get parameter list
      for (const pbes_equation& eq : p.equations())
      {
        if (eq.variable().name() == eq_name)
        {
          const data::variable_list& params = eq.variable().parameters();
          std::set<data::variable>& non_cfp_set = result[eq_name];

          // cfp_vector[i] == true means parameter i is a CFP
          // cfp_vector[i] == false means parameter i is NOT a CFP (non-CFP)
          for (std::size_t i = 0; i < cfp_vector.size() && i < params.size(); ++i)
          {
            if (cfp_vector[i])  // If IS a CFP (remove from non-CFP set)
            {
              non_cfp_set.erase(atermpp::down_cast<data::variable>(params[i]));
            }
          }
          break;
        }
      }
    }

    return result;
  }

  // This can probably be optimized if you create a dependency graph a priori
  void make_data_closed(const pbes& p, std::map<core::identifier_string, std::set<data::variable>>& W, std::map<core::identifier_string, std::set<int>>& pbes_parameters_abstraction_indices)
  {
      bool done = false;
      mCRL2log(log::debug) << "======== Closing the data ======"<< std::endl;
      auto global_variables = p.global_variables();
      do {
        done = true;
        for (const auto& eq: p.equations())
      {
        std::set<propositional_variable_instantiation> pvis = find_propositional_variable_instantiations(eq.formula());
        for (const propositional_variable_instantiation& pvi : pvis)
        {
                
                    int i = 0;
                    mCRL2log(log::debug) << "Data-closed: " << pvi << " has abstracted parameters at indices " << core::detail::print_list(pbes_parameters_abstraction_indices.at(pvi.name())) << " with " << core::detail::print_list(W.at(pvi.name())) << std::endl;
                    for (auto pvi_param_it = pvi.parameters().begin(); pvi_param_it != pvi.parameters().end(); ++pvi_param_it, ++i)   {
                        if (pbes_parameters_abstraction_indices.at(pvi.name()).count(i) > 0)
                        {
                            std::set<data::variable> free_vars = find_free_variables(*pvi_param_it);
                            mCRL2log(log::debug) << "Data-closed: free_vars " << core::detail::print_list(free_vars) << " in \"" << pp(*pvi_param_it) << "\" due to " << pp(pvi) << std::endl;
                            std::set<data::variable> included_vars;
                            for (const data::variable& v : free_vars)
                            {
                                if (W[eq.variable().name()].find(v) == W[eq.variable().name()].end() && std::find(global_variables.begin(), global_variables.end(), v) == global_variables.end())
                                {
                                    included_vars.insert(v);
                                    mCRL2log(log::debug) << "Data-closed: concrete param " << pp(v) << " in W=" << core::detail::print_list(W[eq.variable().name()]) << " of equation " << pvi.name() << " due to " << pp(pvi) << std::endl;
                                    // Find the parameter with the same index 
                                    int i2 = 0;
                                    for (auto it2 = eq.variable().parameters().begin(); it2 != eq.variable().parameters().end(); ++it2, ++i2)   {
                                        if (*it2 == v)
                                        {
                                            W[pvi.name()].erase(v);
                                            pbes_parameters_abstraction_indices.at(pvi.name()).erase(i2);
                                            done = false;
                                            break;
                                        }
                                }
                                }
                                break;
                            }
                        }
                    }
                
        }
      }
    } while (!done);
      mCRL2log(log::debug) << "======== Data closed ======"<< std::endl;
    mCRL2log(log::verbose) << "Data closed: W = " << std::endl;
    for (const auto& [eq_name, var_set] : W)
    {
        mCRL2log(log::verbose) << "" << eq_name << ": " << core::detail::print_list(var_set) << std::endl;
    }

  }

  void report_abstracted_parameters(const std::map<core::identifier_string, std::set<data::variable>>& W_map)
  {
    for (const auto& [eq_name, var_set] : W_map)
    {
      std::string param_names;
      for (const auto& var : var_set)
      {
        param_names += var.name();
        param_names += " ";
      }
      mCRL2log(log::verbose) << "Abstracted parameters for " << eq_name << ": " << param_names << std::endl;
    }
  }

  // Iterative refinement: progressively un-abstract parameters
  // Removes one parameter from one equation's abstraction set per call
  void add_relevant_parameter(const pbes& p, std::map<core::identifier_string, std::set<data::variable>>& W)
  {
    mCRL2log(log::verbose) << "Updating parameters for refinement..." << std::endl;

    // Find the first non-empty equation's abstraction set
    for (auto& [eq_name, var_set] : W)
    {
      if (!var_set.empty())
      {
        // Remove the first parameter from this equation's set
        auto it = var_set.begin();
        data::variable to_remove = *it;
        var_set.erase(it);

        mCRL2log(log::verbose) << "Un-abstracted parameter " << to_remove.name() 
                               << " from equation " << eq_name << std::endl;

        return;
      }
    }

    mCRL2log(log::verbose) << "No more parameters to un-abstract" << std::endl;
  }

  bool run(pbes& p, pbescegps_options options)
  {
    // Calculate non-Control Flow Parameters (parameters to abstract) per equation
    std::map<core::identifier_string, std::set<data::variable>> W = 
      calculate_non_cfp(p, options, options.init_control_flow);

    pbes original_p = p;

    // Calculate the indices of the parameters to abstract
    std::map<core::identifier_string, std::set<int>> pbes_parameters_abstraction_indices = 
      calculate_parameter_abstraction_indices(p, W);

    // Ensure W is data-closed
    make_data_closed(p, W, pbes_parameters_abstraction_indices);

    // Collect sorts to abstract (non-CFP parameters)
    report_abstracted_parameters(W);

    mCRL2log(log::verbose) << "Regular...\n" << pp(p) << std::endl;

    // Iterative refinement loop
    do
    {
      // Check if all equations have empty abstraction sets
      bool all_empty = true;
      for (const auto& [eq_name, var_set] : W)
      {
        if (!var_set.empty())
        {
          all_empty = false;
          break;
        }
      }

      if (all_empty)
      {
        mCRL2log(log::verbose) << "No parameters to abstract, solving normally." << std::endl;
        return solve(p, options);
      }

      // Try under-approximation
      pbes p_under = apply_abstraction_to_pbes(p, W, pbes_parameters_abstraction_indices, false, options);
      mCRL2log(log::verbose) << "Trying under-approximation...\n" << pp(p_under) << std::endl;
      bool under_result = solve_approximation(p_under, options, false);

      if (under_result)
      {
        mCRL2log(log::verbose) << "Under-approximation solved to TRUE" << std::endl;
        report_abstracted_parameters(W);
        return true;
      }

      // Try over-approximation
      pbes p_over = apply_abstraction_to_pbes(p, W, pbes_parameters_abstraction_indices, true, options);
      bool over_result = solve_approximation(p_over, options, true);
      mCRL2log(log::verbose) << "Trying over-approximation...\n" << pp(p_over) << std::endl;

      if (!over_result)
      {
        mCRL2log(log::verbose) << "Over-approximation solved to FALSE" << std::endl;
        report_abstracted_parameters(W);
        return false;
      }

      // TODO: Implement essential variables calculation
      
      // Both approximations are inconclusive, refine by un-abstracting one parameter
      mCRL2log(log::verbose) << "Both approximations inconclusive, refining..." << std::endl;
      p = original_p;
      add_relevant_parameter(p, W);
      pbes_parameters_abstraction_indices = calculate_parameter_abstraction_indices(p, W);
      make_data_closed(p, W, pbes_parameters_abstraction_indices);
    }
    while (true);

    throw mcrl2::runtime_error("Could not find a solution");
  }
};

// TODO: fix pbescegps -v scratch/tempredgreen_septypes.pbes

// Abstraction builder implementation
// Must be defined outside the struct due to template constraints
pbes_expression pbescegps_iterator::apply_abstraction(const pbes_expression& expr,
  const std::set<data::variable>& abstraction_vars,
  const std::map<core::identifier_string, std::set<int>>& pbes_parameters_abstraction_indices,
  bool is_overapproximation)
{
  mCRL2log(log::debug) << "=== Entering apply_abstraction ===" << std::endl;
  mCRL2log(log::debug) << "Abstraction mode: " << (is_overapproximation ? "OVER-approximation" : "UNDER-approximation") << std::endl;
  mCRL2log(log::debug) << "Number of variables to abstract: " << abstraction_vars.size() << std::endl;
  for (const auto& var : abstraction_vars)
  {
    mCRL2log(log::debug) << "  - " << var.name() << std::endl;
  }
  
  pbes_expression result;
  abstraction_rewriter<> rewriter(abstraction_vars, pbes_parameters_abstraction_indices, is_overapproximation);
  mCRL2log(log::debug) << "Created abstraction_rewriter, now applying to expression" << std::endl;
  rewriter.apply(result, expr);
  mCRL2log(log::debug) << "=== Exiting apply_abstraction ===" << std::endl;
  return result;
}

inline bool pbescegps(const std::string& input_filename,
  const utilities::file_format& input_format,
  const pbescegps_options options)
{
  pbes p;
  load_pbes(p, input_filename, input_format);
  algorithms::normalize(p);

  pbescegps_iterator iterator;
  bool result = iterator.run(p, options);

  mCRL2log(log::info) << (result ? "true" : "false") << std::endl;
  return result;
}
}; // namespace mcrl2::pbes_system

#endif // MCRL2_PBES_TOOLS_PBESCEGPS_H
