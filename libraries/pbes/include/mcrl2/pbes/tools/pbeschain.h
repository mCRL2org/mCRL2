// Author(s): Jore Booy
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/tools/pbeschain.h
/// \brief This file provides a tool that can simplify PBESs by
///        substituting PBES equations for variables in the rhs,
///        simplifying the result, and keeping it when it can
///        eliminate PBES variables.

#ifndef MCRL2_PBES_TOOLS_PBESCHAIN_H
#define MCRL2_PBES_TOOLS_PBESCHAIN_H

#include "mcrl2/core/identifier_string.h"
#include "mcrl2/data/rewrite_strategy.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/pbes/algorithms.h"
#include "mcrl2/pbes/detail/instantiate_global_variables.h"
#include "mcrl2/pbes/detail/iteration_builders.h"
#include "mcrl2/pbes/detail/stategraph_pbes.h"
#include "mcrl2/pbes/io.h"
#include "mcrl2/pbes/pbes_equation.h"
#include "mcrl2/pbes/pbes_expression.h"
#include "mcrl2/pbes/resolve_name_clashes.h"
#include "mcrl2/pbes/rewrite.h"
#include "mcrl2/pbes/srf_pbes.h"
#include "mcrl2/pbes/unify_parameters.h"
#include "mcrl2/utilities/logger.h"
#include <chrono>
#include <cstddef>
#include <iostream>

namespace mcrl2::pbes_system
{

struct pbeschain_options
{
  data::rewrite_strategy rewrite_strategy = data::rewrite_strategy::jitty;
  bool back_substitution = true;
  int max_depth = 12;
  bool count_unique_pvi = false;
  bool fill_pvi = false;
  double timeout = 0.0; // timeout in seconds, 0 = no timeout
  double pvi_pp_factor = 0.0; // factor of the maximum size the chained predicate formula should be after chaining
                              // compared to the size of the original PVI.
  bool quantifier_free = false;
  bool avoid_alternating = false;
  bool rewrite_only_substitution = false;
  std::size_t max_number_pvi = 1;
  double srf_factor; // factor of the maximum size the chained equation in SRF should be after chaining compared
                     // to the size of the original equation. Default is 1.0
  bool timings = false;
};

// Substitutor to target specific path, replace our specific pvi with true/false
template<template<class> class Builder>
struct substitute_propositional_variables_for_true_false_builder
  : public Builder<substitute_propositional_variables_for_true_false_builder<Builder>>
{
  using super = Builder<substitute_propositional_variables_for_true_false_builder<Builder>>;
  using super::apply;

  simplify_data_rewriter<data::rewriter> m_pbes_rewriter;
  propositional_variable_instantiation m_pvi;
  pbes_expression m_replacement;

  explicit substitute_propositional_variables_for_true_false_builder(simplify_data_rewriter<data::rewriter>& r)
    : m_pbes_rewriter(r)
  {}

  void set_pvi(const propositional_variable_instantiation x) { m_pvi = x; }
  void set_replacement(const pbes_expression x) { m_replacement = x; }

  template<class T>
  void apply(T& result, const propositional_variable_instantiation& x)
  {
    if (x == m_pvi)
    {
      result = m_replacement;
    }
    else
    {
      result = x;
    }
  }
};

template<template<class> class Builder>
struct rewrite_if_builder : public Builder<rewrite_if_builder<Builder>>
{
  using super = Builder<rewrite_if_builder<Builder>>;
  using super::apply;

  simplify_data_rewriter<data::rewriter> m_pbes_rewriter;

  explicit rewrite_if_builder(simplify_data_rewriter<data::rewriter>& r)
    : m_pbes_rewriter(r)
  {}

  data::data_expression simpl_data(const data::data_expression& x)
  {
    data::data_expression res = x;
    if (data::is_application(x))
    {
      data::application da = atermpp::down_cast<data::application>(x);

      if (data::is_function_symbol(da.head()))
      {
        data::function_symbol fname = atermpp::down_cast<data::function_symbol>(da.head());
        if (fname.name() == atermpp::aterm_string("if") && (da.head().sort().target_sort() == data::bool_()))
        {
          res = data::or_(data::and_(da[0], simpl_data(da[1])), data::and_(data::not_(da[0]), simpl_data(da[2])));
          res = atermpp::down_cast<data::data_expression>(m_pbes_rewriter(atermpp::down_cast<pbes_expression>(res)));
        }
      }
    }
    return res;
  }

  template<class T>
  void apply(T& result, const data::data_expression& x)
  {
    result = atermpp::down_cast<T>(simpl_data(x));
  }
};

template<template<class> class Builder>
struct substitute_propositional_variables_builder : public Builder<substitute_propositional_variables_builder<Builder>>
{
  using super = Builder<substitute_propositional_variables_builder<Builder>>;
  using super::apply;

  pbes_equation m_eq;
  core::identifier_string name;
  simplify_data_rewriter<data::rewriter> m_pbes_rewriter;
  bool m_stable = false;

  explicit substitute_propositional_variables_builder(simplify_data_rewriter<data::rewriter>& r)
    : m_pbes_rewriter(r)
  {}

  void set_stable(bool b) { m_stable = b; }

  bool stable() const { return m_stable; }

  void set_equation(const pbes_equation& eq) { m_eq = eq; }

  void set_name(const core::identifier_string& s) { name = s; }

  template<class T>
  void apply(T& result, const propositional_variable_instantiation& x)
  {
    if (x.name() == m_eq.variable().name())
    {
      data::mutable_indexed_substitution sigma;
      data::data_expression_list pars = x.parameters();
      for (const data::variable& v: m_eq.variable().parameters())
      {
        data::data_expression par = pars.front();
        pars.pop_front();
        if (par.sort() != v.sort())
        {
          // Parameters do not match with variables. Ignore this substitution.
          result = x;
          return;
        }
        sigma[v] = par;
      }
      pbes_expression p = pbes_rewrite(m_eq.formula(), m_pbes_rewriter, sigma);
      std::set<propositional_variable_instantiation> set = find_propositional_variable_instantiations(p);
      if (std::all_of(set.begin(),
            set.end(),
            [this](const propositional_variable_instantiation& v) { return v.name() != m_eq.variable().name(); }))
      {
        // The result does not contain the variable m_eq.variable().name() and is therefore considered simpler.
        mCRL2log(log::debug) << "Replaced in PBES equation for " << name << ":\n" << x << " --> " << p << "\n";
        result = p;
        m_stable = false;
        return;
      }
      mCRL2log(log::debug) << "No Replacement in PBES equation for " << name << ":\n" << x << " --> " << p << "\n";
      result = x;
      return;
    }
    result = x;
  }
};

template<template<class> class Builder>
struct substitute_propositional_variables_i_builder
  : public Builder<substitute_propositional_variables_i_builder<Builder>>
{
  using super = Builder<substitute_propositional_variables_i_builder<Builder>>;
  using super::apply;

  simplify_data_rewriter<data::rewriter> m_pbes_rewriter;
  std::vector<detail::predicate_variable> predvars;
  int i = 0;

  explicit substitute_propositional_variables_i_builder(simplify_data_rewriter<data::rewriter>& r,
    std::vector<detail::predicate_variable> predvars)
    : m_pbes_rewriter(r),
      predvars(predvars)
  {}

  template<class T>
  void apply(T& result, const propositional_variable_instantiation& x)
  {
    result = pbes_rewrite(pbes_expression(x), m_pbes_rewriter, predvars[i].sigma());
    i++;
  }
};

/// \brief Returns all data variables that occur in a range of expressions
/// \param[in] container a container with expressions
/// \return All data variables that occur in the term t
template<typename Container>
std::vector<propositional_variable_instantiation> get_propositional_variable_instantiations(Container const& container)
{
  std::vector<propositional_variable_instantiation> result;
  pbes_system::find_propositional_variable_instantiations(container, std::inserter(result, result.end()));
  return result;
}



/// \brief Helper class to track multiple time measurements and call counts
struct timing_tracker
{
  struct measurement
  {
    std::chrono::duration<double, std::milli> total_time = std::chrono::duration<double, std::milli>::zero();
    int call_count = 0;

    void add_measurement(std::chrono::duration<double, std::milli> duration)
    {
      total_time += duration;
      call_count++;
    }

    double get_total_seconds() const { return std::chrono::duration<double>(total_time).count(); }
  };

  std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
  std::map<std::string, measurement> measurements;

  timing_tracker()
    : start_time(std::chrono::high_resolution_clock::now())
  {}

  /// \brief Start timing an operation (returns the start time for use with end_measurement)
  std::chrono::time_point<std::chrono::steady_clock> start_measurement() { return std::chrono::steady_clock::now(); }

  /// \brief End timing an operation and record it
  void end_measurement(const std::string& label, std::chrono::time_point<std::chrono::steady_clock> measurement_start)
  {
    auto duration = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(
      std::chrono::steady_clock::now() - measurement_start);
    measurements[label].add_measurement(duration);
  }

  /// \brief Log all measurements
  void log_measurements() const
  {
    double total_elapsed
      = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - start_time).count();
    mCRL2log(log::verbose) << "Total time: " << total_elapsed << "s" << std::endl;

    if (!measurements.empty())
    {
      // Find the longest label name for alignment
      std::size_t max_label_length = 0;
      for (const auto& [label, measurement]: measurements)
      {
        max_label_length = std::max(max_label_length, label.length() + 4);
      }

      // Log measurements with aligned values
      for (const auto& [label, measurement]: measurements)
      {
        mCRL2log(log::verbose) << std::left << std::setw(max_label_length) << (label + " ") << ": "
                               << std::right << std::setw(2) << std::fixed 
                               << measurement.get_total_seconds() << "s ("
                               << measurement.call_count << " calls)" << std::endl;
      }
    }
  }
};

/// \brief Generic decorator to wrap any function call with timing measurements
template<typename Func>
inline auto measure_time(timing_tracker& timer, const std::string& name, Func&& func)
{
  auto start = timer.start_measurement();
  if constexpr (std::is_same_v<void, decltype(func())>)
  {
    func();
    timer.end_measurement(name, start);
  }
  else
  {
    auto result = func();
    timer.end_measurement(name, start);
    return result;
  }
}

inline pbes_expression simplify_expr(pbes_expression& phi,
  rewrite_if_builder<pbes_system::pbes_expression_builder>& if_substituter,
  simplify_data_rewriter<data::rewriter>& pbes_rewriter)
{
  pbes_expression res = pbes_rewrite(phi, pbes_rewriter);
  if_substituter.apply(res, res);
  return res;
}

inline bool is_quantifier_free(pbes_expression& phi, pbeschain_options options)
{
  return !(options.quantifier_free) || (find_all_variables(phi).size() - find_free_variables(phi).size()) == 0;
}

inline bool
is_not_too_big(pbeschain_options& options, propositional_variable_instantiation& cur_x, pbes_expression& phi)
{
  return !(options.pvi_pp_factor > 0.0) || ((double)pp(phi).size() <= options.pvi_pp_factor * (double)pp(cur_x).size());
}

inline bool
is_avoiding_alternation(pbeschain_options& options, propositional_variable_instantiation& new_x, pbes_equation& eq)
{
  return !(options.avoid_alternating) || new_x.name() == eq.variable().name();
}

inline void log_number_pvi(std::size_t& initial_size, std::size_t& current_size)
{
  mCRL2log(log::status) << "New number of pvi: " << initial_size << " --> " << current_size << "" << std::endl;
}

inline void self_substitute(pbes_equation& equation,
  std::size_t initial_size,
  substitute_propositional_variables_for_true_false_builder<pbes_system::pbes_expression_builder>& pvi_substituter,
  rewrite_if_builder<pbes_system::pbes_expression_builder>& if_substituter,
  simplify_data_rewriter<data::rewriter>& pbes_rewriter,
  simplify_data_rewriter<data::rewriter>& pbes_default_rewriter,
  pbeschain_options options)
{
  bool stable = false;
  timing_tracker timer;

  if (options.timeout > 0.0)
  {
    mCRL2log(log::verbose) << "Starting substitution for equation " << equation.variable().name() << " with timeout "
                           << options.timeout << "s" << std::endl;
  }

  std::set<propositional_variable_instantiation> stable_set = {}; // To record pvi that have reach a max depth
  std::size_t previous_size = initial_size;
  std::size_t current_size = initial_size;
  while (!stable)
  {
    stable = true;
    std::vector<propositional_variable_instantiation> set
      = get_propositional_variable_instantiations(equation.formula());

    std::set<std::string> parameterNames = {};
    for (data::variable a: equation.variable().parameters())
    {
      parameterNames.insert(a.name());
    }
    for (const propositional_variable_instantiation& x: set)
    {
      // Check timeout
      if (options.timeout > 0.0)
      {
        std::chrono::time_point current_time = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double>(current_time - timer.start_time).count();
        if (elapsed >= options.timeout)
        {
          stable = true;
          mCRL2log(log::verbose) << "Timeout reached (" << options.timeout << "s) for equation "
                                 << equation.variable().name() << " after " << elapsed << "s, stopping substitution"
                                 << std::endl;
          break;
        }
      }
      if (equation.variable().name() != x.name())
      {
        continue;
      }
      if (auto it = stable_set.find(x); it != stable_set.end())
      {
        continue;
      }
      // Check if during the substitution of the other pvi this one got cancelled out
      std::set<propositional_variable_instantiation> path = {x};

      propositional_variable_instantiation cur_x = x;
      pbes_expression result = x;

      mCRL2log(log::debug) << " -  -  -  -  -  -  -  -  -  -  -  -  -  -  -\n"
                           << "\n\nStart " << (cur_x) << "\n";
      bool pvi_done = false;
      int depth = 0;
      while (!pvi_done)
      {
        depth = depth + 1;

        // (1) simplify
        data::mutable_indexed_substitution sigma;
        data::data_expression_list pars = cur_x.parameters();
        for (const data::variable& v: equation.variable().parameters())
        {
          data::data_expression par = pars.front();
          pars.pop_front();
          if (par.sort() != v.sort())
          {
            throw mcrl2::runtime_error("PBES is not well-typed.");
          }
          sigma[v] = par;
        }

        pbes_expression phi = measure_time(timer,
          "substitution",
          [&]() { return pbes_rewrite(equation.formula(), pbes_default_rewriter, sigma); });

        std::vector<propositional_variable_instantiation> phi_vector = get_propositional_variable_instantiations(phi);

        // (2) replace all reoccuring with true (nu) and false (mu)
        auto it = find(phi_vector.begin(), phi_vector.end(), cur_x);
        if (it != phi_vector.end())
        {
          mCRL2log(log::debug) << "Need to replace this with true/false " << pp(cur_x) << "\n";
          mCRL2log(log::debug) << phi << "\n";
          mCRL2log(log::debug) << equation.formula() << "\n";

          pvi_substituter.set_pvi(cur_x);
          pvi_substituter.set_replacement(equation.symbol().is_nu() ? true_() : false_());
          pvi_substituter.apply(phi, phi);

          mCRL2log(log::debug) << phi << "\n";

          mCRL2log(log::debug) << "- - - - - - - - - - - - - - - - - - - - \n";
        }

        // Simplify
        phi = measure_time(timer, "simplify_expr", [&]() { return simplify_expr(phi, if_substituter, pbes_rewriter); });

        phi_vector = get_propositional_variable_instantiations(phi);
        std::size_t size = phi_vector.size();
        if (options.count_unique_pvi)
        {
          size = std::set(phi_vector.begin(), phi_vector.end()).size();
        }

        // TODO: Also queue the other PVI in some way

        // (3) check if simpler
        bool condition_result = (size >= 1 && size <= options.max_number_pvi);

        if (condition_result)
        {
          condition_result = measure_time(timer,
            "is_avoiding_alternation",
            [&]() { return is_avoiding_alternation(options, *phi_vector.begin(), equation); });
        }

        if (condition_result)
        {
          condition_result
            = measure_time(timer, "is_not_too_big", [&]() { return is_not_too_big(options, cur_x, phi); });
        }

        if (condition_result)
        {
          condition_result
            = measure_time(timer, "is_quantifier_free", [&]() { return is_quantifier_free(phi, options); });
        }

        if (condition_result)
        {
          std::set<propositional_variable_instantiation> phi_set(phi_vector.begin(), phi_vector.end());
          bool all_in_path = true;
          measure_time(timer, "successful_substitutions", [&]() { return true; });

          for (const propositional_variable_instantiation& phi_x: phi_set)
          {
            mCRL2log(log::debug) << "Trying loop " << phi_x << " in path with \n";
            for (const propositional_variable_instantiation& itr: path)
            {
              mCRL2log(log::debug) << itr << "\n";
            }

            if (path.contains(phi_x))
            {
              // We have already seen this, so we are in a loop.
              mCRL2log(log::debug) << "Loop, seen " << phi_x << " in path after " << cur_x << "    " << phi << "\n";
              for (const propositional_variable_instantiation& itr: path)
              {
                mCRL2log(log::debug) << itr << "\n";
              }
              pvi_substituter.set_pvi(phi_x);
              pvi_substituter.set_replacement(equation.symbol().is_nu() ? true_() : false_());
              pvi_substituter.apply(result, phi);
            }
            else
            {
              all_in_path = false;
            }
          }

          if (all_in_path)
          {
            pvi_substituter.set_pvi(cur_x);
            pvi_substituter.set_replacement(result);
            pvi_substituter.apply(equation.formula(), equation.formula());

            mCRL2log(log::debug) << "new_phi " << result << "\n";
            stable = false;
            pvi_done = true;
          }
          else
          {
            // The result does not contain the variable m_eq.variable().name() and is therefore considered simpler.
            mCRL2log(log::debug) << "Replaced in PBES equation for " << cur_x << "\n-->\n"
                                 << phi << "\n"
                                 << core::detail::print_list(phi_set) << "\n";

            pvi_substituter.set_pvi(cur_x);
            pvi_substituter.set_replacement(phi);
            pvi_substituter.apply(equation.formula(), equation.formula());
            // Get the set of elements that have the same name as the equation variable
            std::set<propositional_variable_instantiation> phi_set_same_name;
            std::copy_if(phi_set.begin(),
              phi_set.end(),
              std::inserter(phi_set_same_name, phi_set_same_name.begin()),
              [&](const propositional_variable_instantiation& pvi)
              { return pvi.name() == equation.variable().name(); });
            stable = false;
            if (phi_set_same_name.size() == 0)
            {
              pvi_done = true;
            }
            else
            {
              cur_x = *phi_set_same_name.begin();
            }
          }
        }
        else if (size == 0 && is_quantifier_free(phi, options))
        {
          pvi_substituter.set_pvi(cur_x);
          pvi_substituter.set_replacement(phi);
          pvi_substituter.apply(equation.formula(), equation.formula());
          stable = false;
          mCRL2log(log::debug) << "Replaced in PBES equation for " << cur_x << ":\n" << x << " \n-->\n " << phi << "\n";
          pvi_done = true;
          measure_time(timer, "successful_substitutions", [&]() { return true; });
        }
        else
        {
          mCRL2log(log::debug) << "Not simpler: " << cur_x << " \n--> size: " << phi_vector.size() << "\n " << phi
                               << " and size " << phi_vector.size() << "\n";
          pvi_done = true;
          if (depth > 1)
          {
            stable = false;
          }
        }
        if (pvi_done)
        {
          mCRL2log(log::debug) << "Nothing further to do\n";
          break;
        }
        else if (depth >= options.max_depth)
        {
          stable_set.insert(x);
          stable_set.insert(cur_x);
          pvi_done = true;
          break;
        }
      }

      std::vector<propositional_variable_instantiation> set
        = get_propositional_variable_instantiations(equation.formula());

      current_size = set.size();
      log_number_pvi(initial_size, current_size);

      // Simplify
      if (current_size == 0 || (previous_size >= current_size + 10))
      {
        previous_size = current_size;
        equation.formula() = measure_time(timer,
          "simplify_expr",
          [&]() { return simplify_expr(equation.formula(), if_substituter, pbes_rewriter); });
      }
    }
  }

  log_number_pvi(initial_size, current_size);

  if (current_size == 0)
  {
    equation.formula() = measure_time(timer,
      "simplify_expr",
      [&]() { return simplify_expr(equation.formula(), if_substituter, pbes_rewriter); });
  }

  if (options.timings)
  {
    timer.log_measurements();
  }
}

inline void substitute(pbes_equation& into,
  const pbes_equation& by,
  substitute_propositional_variables_builder<pbes_system::pbes_expression_builder>& substituter)
{
  substituter.set_equation(by);
  substituter.set_name(into.variable().name());
  pbes_expression p;
  substituter.apply(p, into.formula());
  into.formula() = p;
}

// Fill in the parameters of the pvi based on the guard of each pvi
inline pbes fill_pvi(pbes& p, data::rewriter data_rewriter)
{
  detail::stategraph_pbes stategraph(p, data_rewriter);
  simplify_data_rewriter<data::rewriter> pbes_default_rewriter(data_rewriter);

  // Preparation
  for (detail::stategraph_equation& equation: stategraph.equations())
  {
    for (detail::predicate_variable& predvar: equation.predicate_variables())
    {
      predvar.simplify_guard();
    }
  }
  stategraph.compute_source_target_copy();

  // Reduction
  std::vector<pbes_equation> eqn = {};
  for (detail::stategraph_equation& eq: stategraph.equations())
  {
    substitute_propositional_variables_i_builder<pbes_system::pbes_expression_builder> substituter(
      pbes_default_rewriter,
      eq.predicate_variables());

    pbes_expression new_formula;
    substituter.apply(new_formula, eq.formula());
    pbes_equation new_eq(static_cast<const pbes_equation&>(eq));
    new_eq.formula() = new_formula;
    eqn.push_back(new_eq);
  }

  // Back to pbes
  pbes res(p.data(), p.global_variables(), eqn, p.initial_state());
  return res;
}

inline pbes tosrf(pbes_system::pbes pbesspec)
{
  pbes_system::detail::instantiate_global_variables(pbesspec);
  auto result = pbes2pre_srf(pbesspec, true);
  // Unify the parameters of the original PBES (which has potential counter example information)
  unify_parameters(result, true, false);
  pbes_system::resolve_summand_variable_name_clashes(result,
    result.equations().front().variable().parameters()); // N.B. This is a required preprocessing step.
  return pre_srf2srfpbes(result).to_pbes();
}

struct pbeschain_pbes_backward_substituter
{
  void run(pbes& p, pbeschain_options options)
  {
    data::rewriter data_rewriter(p.data(), options.rewrite_strategy);
    data::rewriter data_default_rewriter(p.data());
#ifdef MCRL2_ENABLE_JITTYC
    if (options.rewrite_strategy == data::rewriter::strategy::jitty_compiling
        || options.rewrite_strategy == data::rewriter::strategy::jitty_compiling_prover)
    {
      data_default_rewriter = data_rewriter;
    }
#endif // MCRL2_ENABLE_JITTYC
    simplify_data_rewriter<data::rewriter> pbes_rewriter(data_rewriter);
    simplify_data_rewriter<data::rewriter> pbes_rewriter2(data_rewriter);
    simplify_data_rewriter<data::rewriter> pbes_default_rewriter(data_default_rewriter);
    substitute_propositional_variables_builder<pbes_system::pbes_expression_builder> substituter(pbes_default_rewriter);
    rewrite_if_builder<pbes_system::pbes_expression_builder> if_rewriter(pbes_default_rewriter);
    substitute_propositional_variables_for_true_false_builder<pbes_system::pbes_expression_builder> pvi_substituter(
      pbes_rewriter);

    if (options.fill_pvi)
    {
      p = fill_pvi(p, data_rewriter);
    }

    if (options.max_depth <= 0)
    {
      return;
    }

    std::vector<std::size_t> initial_sizes = {};
    for (pbes_equation& i: p.equations())
    {
      initial_sizes.push_back(get_propositional_variable_instantiations(i.formula()).size());
    }

    pbes original_pbes;
    if (options.srf_factor > 0)
    {
      original_pbes = p;
    }

    for (std::vector<pbes_equation>::reverse_iterator i = p.equations().rbegin(); i != p.equations().rend(); i++)
    {
      mCRL2log(log::verbose) << "Investigating the equation for " << i->variable().name() << "\n";
      std::size_t original_i = (p.equations().rend() - i) - 1;

      self_substitute(*i,
        initial_sizes[original_i],
        pvi_substituter,
        if_rewriter,
        options.rewrite_only_substitution ? pbes_default_rewriter : pbes_rewriter,
        pbes_default_rewriter,
        options);

      std::set<propositional_variable_instantiation> pvi_set
        = find_propositional_variable_instantiations((*i).formula());

      // If the SRF form of some equation is too big, replace the formula of that equation with the original formula.
      if (pvi_set.size() > 0 && options.srf_factor > 0)
      {
        // Use the same SRF form as pbessolvesymbolic
        pbes result_srf_pbes = tosrf(p);
        pbes original_srf_pbes = tosrf(original_pbes);

        // Find our equation in both PBESs
        pbes_equation original_eq = original_pbes.equations()[original_i];
        core::identifier_string original_variable_name = original_eq.variable().name();

        auto result_srf_eq = std::find_if(result_srf_pbes.equations().rbegin(),
          result_srf_pbes.equations().rend(),
          [&](const pbes_equation& eq) { return eq.variable().name() == original_variable_name; });
        if (result_srf_eq == result_srf_pbes.equations().rend())
          continue;

        auto original_srf_eq = std::find_if(original_srf_pbes.equations().rbegin(),
          original_srf_pbes.equations().rend(),
          [&](const pbes_equation& eq) { return eq.variable().name() == original_variable_name; });
        if (original_srf_eq == original_srf_pbes.equations().rend())
          continue;

        std::size_t original_size = pp(original_srf_eq->formula()).size();
        std::size_t new_size = pp(result_srf_eq->formula()).size();
        mCRL2log(log::debug) << "Original size: " << original_size << " New size: " << new_size << "\n";
        if (options.srf_factor * (double)original_size <= (double)new_size)
        {
          log_number_pvi(initial_sizes[original_i], initial_sizes[original_i]);
          (*i).formula() = original_eq.formula();
          pvi_set = find_propositional_variable_instantiations((*i).formula());
        }
      }

      mCRL2log(log::verbose) << "How many unique PVI are left? " << pvi_set.size() << "\n";

      // Substitute back
      if (pvi_set.size() == 0 && options.back_substitution)
      {
        if (options.rewrite_only_substitution)
        {
          (*i).formula() = simplify_expr((*i).formula(), if_rewriter, pbes_rewriter);
        }
        for (std::vector<pbes_equation>::reverse_iterator j = i + 1; j != p.equations().rend(); j++)
        {
          substitute(*j, *i, substituter);
        }
      }
    }
  }
};

inline void pbeschain(const std::string& input_filename,
  const std::string& output_filename,
  const utilities::file_format& input_format,
  const utilities::file_format& output_format,
  pbeschain_options options)
{
  pbes p;
  load_pbes(p, input_filename, input_format);
  algorithms::normalize(p);
  pbeschain_pbes_backward_substituter backward_substituter;
  backward_substituter.run(p, options);
  save_pbes(p, output_filename, output_format);
}

} // namespace mcrl2::pbes_system

#endif // MCRL2_PBES_TOOLS_PBESCHAIN_H
