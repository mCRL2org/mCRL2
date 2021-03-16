// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/pbesreach.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_PBESREACH_H
#define MCRL2_PBES_PBESREACH_H

#include <chrono>
#include <iomanip>
#include <boost/dynamic_bitset.hpp>
#include <sylvan_ldd.hpp>
#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/detail/container_utility.h"
#include "mcrl2/data/consistency.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/merge_data_specifications.h"
#include "mcrl2/data/substitution_utility.h"
#include "mcrl2/lps/symbolic_reachability.h"
#include "mcrl2/pbes/detail/instantiate_global_variables.h"
#include "mcrl2/pbes/detail/pbes_io.h"
#include "mcrl2/pbes/normalize.h"
#include "mcrl2/pbes/replace_constants_by_variables.h"
#include "mcrl2/pbes/resolve_name_clashes.h"
#include "mcrl2/pbes/rewriters/one_point_rule_rewriter.h"
#include "mcrl2/pbes/srf_pbes.h"
#include "mcrl2/pbes/unify_parameters.h"
#include "mcrl2/utilities/text_utility.h"

namespace mcrl2 {

namespace pbes_system {

// Returns a data specification containing a structured sort with the names of the propositional variables
// in the PBES as elements.
inline
data::data_specification construct_propositional_variable_data_specification(const pbes_system::srf_pbes& pbesspec, const std::string& sort_name)
{
  // TODO: it should be possible to add a structured sort to pbesspec.data() directly, but I don't know how
  std::vector<std::string> names;
  for (const auto& equation: pbesspec.equations())
  {
    names.push_back(equation.variable().name());
  }
  std::string text = "sort " + sort_name + " = struct " +   utilities::string_join(names, " | ") + ";";
  return data::parse_data_specification(text);
}

struct symbolic_reachability_options: public lps::symbolic_reachability_options
{
  bool make_total = false;
  std::string srf;
};

inline
std::ostream& operator<<(std::ostream& out, const symbolic_reachability_options& options)
{
  out << static_cast<lps::symbolic_reachability_options>(options);
  out << "total = " << std::boolalpha << options.make_total << std::endl;
  return out;
}

inline
std::pair<std::set<data::variable>, std::set<data::variable>> read_write_parameters(
  const pbes_system::srf_equation& equation,
  const pbes_system::srf_summand& summand,
  const data::variable_list& process_parameters)
{
  using utilities::detail::set_union;
  using utilities::detail::set_intersection;
  using utilities::detail::as_set;

  std::set<data::variable> read_parameters = data::find_free_variables(summand.condition());
  std::set<data::variable> write_parameters;

  // We need special handling for the first parameter, since the clause 'propvar == X' is not in the condition yet
  read_parameters.insert(process_parameters.front());
  if (equation.variable().name() != summand.variable().name())
  {
    write_parameters.insert(process_parameters.front());
  }

  const data::data_expression_list& expressions = summand.variable().parameters();
  auto pi = ++process_parameters.begin(); // skip the first parameter
  auto ei = expressions.begin();
  for (; ei != expressions.end(); ++pi, ++ei)
  {
    if (*pi != *ei)
    {
      write_parameters.insert(*pi);
      data::find_free_variables(*ei, std::inserter(read_parameters, read_parameters.end()));
    }
  }

  auto process_parameter_set = as_set(process_parameters);
  return { set_intersection(read_parameters, process_parameter_set), set_intersection(write_parameters, process_parameter_set) };
}

inline
std::map<data::variable, std::size_t> process_parameter_index(const data::variable_list& process_parameters)
{
  std::map<data::variable, std::size_t> result;
  std::size_t i = 0;
  for (const data::variable& v: process_parameters)
  {
    result[v] = i++;
  }
  return result;
}

inline
std::vector<boost::dynamic_bitset<>> compute_read_write_patterns(const pbes_system::srf_pbes& pbesspec, const data::variable_list& process_parameters)
{
  std::vector<boost::dynamic_bitset<>> result;

  std::size_t n = process_parameters.size();
  std::map<data::variable, std::size_t> index = process_parameter_index(process_parameters);

  for (const auto& equation: pbesspec.equations())
  {
    for (const auto& summand: equation.summands())
    {
      auto [read_parameters, write_parameters] = read_write_parameters(equation, summand, process_parameters);
      auto read = lps::parameter_indices(read_parameters, index);
      auto write = lps::parameter_indices(write_parameters, index);
      boost::dynamic_bitset<> rw(2*n);
      for (std::size_t j: read)
      {
        rw[2*j] = true;
      }
      for (std::size_t j: write)
      {
        rw[2*j + 1] = true;
      }
      result.push_back(rw);
    }
  }

  return result;
}

inline
data::data_expression_list make_state(const pbes_system::propositional_variable_instantiation& x, const std::unordered_map<core::identifier_string, data::data_expression>& propvar_map)
{
  data::data_expression_list result = x.parameters();
  result.push_front(propvar_map.at(x.name()));
  return result;
}

struct summand_group: public lps::summand_group
{
  summand_group(
    const pbes_system::srf_pbes& pbesspec,
    const data::variable_list& process_parameters, // the reordered process parameters
    const std::unordered_map<core::identifier_string, data::data_expression>& propvar_map,
    const std::set<std::size_t>& summand_group_indices,
    const boost::dynamic_bitset<>& read_write_pattern,
    const std::vector<boost::dynamic_bitset<>>& read_write_patterns,
    const std::vector<std::size_t> variable_order // a permutation of [0 .. |process_parameters| - 1]
  )
    : lps::summand_group(process_parameters, read_write_pattern)
  {
    using lps::project;
    using utilities::detail::as_vector;
    using utilities::detail::as_set;
    using utilities::detail::set_union;
    using utilities::detail::contains;

    std::set<std::size_t> used;
    for (std::size_t j: read)
    {
      used.insert(2*j);
    }
    for (std::size_t j: write)
    {
      used.insert(2*j + 1);
    }

    const auto& equations = pbesspec.equations();
    std::size_t k = 0;
    for (std::size_t i = 0; i < equations.size(); i++)
    {
      const core::identifier_string& X_i = equations[i].variable().name();
      const auto& equation_summands = equations[i].summands();
      for (std::size_t j = 0; j < equation_summands.size(); j++, k++)
      {
        if (contains(summand_group_indices, k))
        {
          std::vector<int> copy;
          for (std::size_t q: used)
          {
            bool b = read_write_patterns[k][q];
            copy.push_back(b ? 0 : 1);
          }
          const pbes_system::srf_summand& smd = equation_summands[j];
          auto next_state = make_state(smd.variable(), propvar_map);
          next_state = lps::permute_copy(next_state, variable_order);
          summands.emplace_back(data::and_(data::equal_to(process_parameters.front(), propvar_map.at(X_i)), smd.condition()), smd.parameters(), project(as_vector(next_state), write), copy);
        }
      }
    }
  }
};

class pbesreach_algorithm
{
    using ldd = sylvan::ldds::ldd;
    using enumerator_element = data::enumerator_list_element_with_substitution<>;

    template <typename Context>
    friend void lps::learn_successors_callback(WorkerP*, Task*, std::uint32_t* v, std::size_t n, void* context);

  protected:
    const symbolic_reachability_options& m_options;
    pbes_system::srf_pbes m_pbes;
    data::rewriter m_rewr;
    data::mutable_indexed_substitution<> m_sigma;
    data::enumerator_identifier_generator m_id_generator;
    data::enumerator_algorithm<> m_enumerator;
    data::variable_list m_process_parameters;
    std::size_t m_n;
    std::vector<lps::data_expression_index> m_data_index;
    std::vector<summand_group> m_summand_groups;
    data::data_expression_list m_initial_state;
    std::vector<boost::dynamic_bitset<>> m_summand_patterns;
    std::vector<boost::dynamic_bitset<>> m_group_patterns;
    std::vector<std::size_t> m_variable_order;

    ldd state2ldd(const data::data_expression_list& x)
    {
      MCRL2_DECLARE_STACK_ARRAY(v, std::uint32_t, x.size());

      auto vi = v.begin();
      auto di = m_data_index.begin();
      auto xi = x.begin();
      for (; di != m_data_index.end(); ++vi, ++di, ++xi)
      {
        *vi = di->index(*xi);
      }

      return sylvan::ldds::cube(v.data(), x.size());
    };

    // R.L := R.L U {(x,y) in R | x in X}
    void learn_successors(std::size_t i, summand_group& R, const ldd& X)
    {
      mCRL2log(log::debug) << "learn successors of summand group " << i << " for X = " << print_states(m_data_index, X, R.read) << std::endl;

      using namespace sylvan::ldds;
      std::pair<pbesreach_algorithm&, summand_group&> context{*this, R};
      sat_all_nopar(X, lps::learn_successors_callback<std::pair<pbesreach_algorithm&, summand_group&>>, &context);
    }

    pbes_system::srf_pbes preprocess(pbes_system::pbes pbesspec, bool make_total)
    {
      pbes_system::detail::instantiate_global_variables(pbesspec);
      normalize(pbesspec);
      if (m_options.one_point_rule_rewrite)
      {
        pbes_system::one_point_rule_rewriter R;
        pbes_system::replace_pbes_expressions(pbesspec, R, false);
      }
      if (m_options.replace_constants_by_variables)
      {
        pbes_system::replace_constants_by_variables(pbesspec, m_rewr, m_sigma);
      }

      pbes_system::srf_pbes result = pbes2srf(pbesspec);
      if (make_total)
      {
        result.make_total();
      }
      unify_parameters(result);
      pbes_system::resolve_summand_variable_name_clashes(result, result.equations().front().variable().parameters()); // N.B. This is a required preprocessing step.

      // add a sort for the propositional variable names
      data::data_specification propvar_dataspec = construct_propositional_variable_data_specification(result, "PropositionalVariable");
      result.data() = data::merge_data_specifications(result.data(), propvar_dataspec);
      mCRL2log(log::debug) << "--- srf pbes ---\n" << result.to_pbes() << std::endl;

      return result;
    }

  public:
    pbesreach_algorithm(const pbes_system::pbes& pbesspec, const symbolic_reachability_options& options_)
      : m_options(options_),
        m_pbes(preprocess(pbesspec, options_.make_total)),
        m_rewr(lps::construct_rewriter(m_pbes.data(), m_options.rewrite_strategy, pbes_system::find_function_symbols(pbesspec), m_options.remove_unused_rewrite_rules)),
        m_enumerator(m_rewr, m_pbes.data(), m_rewr, m_id_generator, false)
    {
      if (!m_options.srf.empty())
      {
        detail::save_pbes(m_pbes.to_pbes(), m_options.srf);
      }

      data::basic_sort propvar_sort("PropositionalVariable"); // todo: choose a unique name
      std::unordered_map<core::identifier_string, data::data_expression> propvar_map;
      for (const auto& equation: m_pbes.equations())
      {
        propvar_map[equation.variable().name()] = data::function_symbol(equation.variable().name(), propvar_sort);
      }

      m_process_parameters = m_pbes.equations().front().variable().parameters();
      m_process_parameters.push_front(data::variable("propvar", propvar_sort)); // todo: choose a unique name
      m_n = m_process_parameters.size();
      m_initial_state = make_state(m_pbes.initial_state(), propvar_map);

      m_summand_patterns = compute_read_write_patterns(m_pbes, m_process_parameters);
      lps::adjust_read_write_patterns(m_summand_patterns, m_options);

      m_variable_order = lps::compute_variable_order(m_options.variable_order, m_summand_patterns, true);
      assert(m_variable_order[0] == 0); // It is required that the propositional variable name stays up front
      mCRL2log(log::debug) << "variable order = " << core::detail::print_list(m_variable_order) << std::endl;
      m_summand_patterns = lps::reorder_read_write_patterns(m_summand_patterns, m_variable_order);
      mCRL2log(log::debug) << lps::print_read_write_patterns(m_summand_patterns);

      m_process_parameters = lps::permute_copy(m_process_parameters, m_variable_order);
      m_initial_state = lps::permute_copy(m_initial_state, m_variable_order);
      mCRL2log(log::debug) << "process parameters = " << core::detail::print_list(m_process_parameters) << std::endl;
      mCRL2log(log::debug) << "initial state = " << core::detail::print_list(m_initial_state) << std::endl;

      std::vector<std::set<std::size_t>> groups = lps::compute_summand_groups(m_options.summand_groups, m_summand_patterns);
      for (const auto& group: groups)
      {
        mCRL2log(log::debug) << "group " << core::detail::print_set(group) << std::endl;
      }
      m_group_patterns = lps::compute_summand_group_patterns(m_summand_patterns, groups);
      for (std::size_t j = 0; j < m_group_patterns.size(); j++)
      {
        m_summand_groups.emplace_back(m_pbes, m_process_parameters, propvar_map, groups[j], m_group_patterns[j], m_summand_patterns, m_variable_order);
      }

      for (std::size_t i = 0; i < m_summand_groups.size(); i++)
      {
        mCRL2log(log::debug) << "=== summand group " << i << " ===\n" << m_summand_groups[i] << std::endl;
      }

      for (const data::variable& param: m_process_parameters)
      {
        m_data_index.push_back(lps::data_expression_index(param.sort()));
      }
    }

    ldd initial_state()
    {
      return state2ldd(m_initial_state);
    }

    ldd run(bool report_states = false)
    {
      using namespace sylvan::ldds;
      auto& R = m_summand_groups;
      std::size_t iteration_count = 0;

      mCRL2log(log::debug) << "initial state = " << core::detail::print_list(m_initial_state) << std::endl;

      auto start = std::chrono::steady_clock::now();
      ldd x = state2ldd(m_initial_state);
      std::chrono::duration<double> elapsed_seconds = std::chrono::steady_clock::now() - start;
      ldd visited = empty_set();
      ldd todo = x;

      while (todo != empty_set())
      {
        auto loop_start = std::chrono::steady_clock::now();
        iteration_count++;
        mCRL2log(log::debug) << "--- iteration " << iteration_count << " ---" << std::endl;
        mCRL2log(log::debug) << "todo = " << print_states(m_data_index, todo) << std::endl;

        ldd todo1 = m_options.chaining ? todo : empty_set();
        for (std::size_t i = 0; i < R.size(); i++)
        {
          learn_successors(i, R[i], minus(project(m_options.chaining ? todo1 : todo, R[i].Ip), R[i].Ldomain));
          mCRL2log(log::debug) << "L =\n" << print_relation(m_data_index, R[i].L, R[i].read, R[i].write) << std::endl;

          if (m_options.no_relprod)
          {
            ldd z = lps::alternative_relprod(m_options.chaining ? todo1 : todo, R[i]);
            mCRL2log(log::debug) << "relprod(" << i << ", todo) = " << print_states(m_data_index, z) << std::endl;
            todo1 = union_(z, todo1);
          }
          else
          {
            mCRL2log(log::debug) << "relprod(" << i << ", todo) = " << print_states(m_data_index, relprod(todo, R[i].L, R[i].Ir)) << std::endl;
            todo1 = relprod_union(m_options.chaining ? todo1 : todo, R[i].L, R[i].Ir, todo1);
          }
        }

        visited = union_(visited, todo);
        todo = minus(todo1, visited);

        elapsed_seconds = std::chrono::steady_clock::now() - loop_start;
        mCRL2log(log::verbose) << "found " << std::setw(12) << satcount(visited) << " states after "
                               << std::setw(3) << iteration_count << " iterations (time = " << std::setprecision(2)
                               << std::fixed << elapsed_seconds.count() << "s)" << std::endl;
        mCRL2log(log::verbose) << "LDD size = " << nodecount(visited) << std::endl;
      }

      elapsed_seconds = std::chrono::steady_clock::now() - start;
      if (report_states)
      {
        std::cout << "number of states = " << satcount(visited) << " (time = " << std::setprecision(2) << std::fixed << elapsed_seconds.count() << "s)" << std::endl;
      }
      else
      {
        mCRL2log(log::verbose) << "number of states = " << satcount(visited) << " (time = " << std::setprecision(2) << std::fixed << elapsed_seconds.count() << "s)" << std::endl;
      }

      mCRL2log(log::verbose) << "LDD size = " << nodecount(visited) << std::endl;
      mCRL2log(log::verbose) << "used variable order = " << core::detail::print_list(m_variable_order) << std::endl;

      for (std::size_t i = 0; i < R.size(); i++)
      {
        mCRL2log(log::verbose) << "group " << std::setw(4) << i << " contains " << std::setw(7) << satcount(R[i].L) << " transitions" << std::endl;
      }

      return visited;
    }

    const std::vector<summand_group>& summand_groups() const
    {
      return m_summand_groups;
    }

    const srf_pbes& pbes() const
    {
      return m_pbes;
    }

    const data::variable_list& process_parameters() const
    {
      return m_process_parameters;
    }

    const std::vector<lps::data_expression_index>& data_index() const
    {
      return m_data_index;
    }

    const std::vector<boost::dynamic_bitset<>>& read_write_patterns() const
    {
      return m_summand_patterns;
    }

    const std::vector<boost::dynamic_bitset<>>& read_write_group_patterns() const
    {
      return m_group_patterns;
    }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PBESREACH_H
