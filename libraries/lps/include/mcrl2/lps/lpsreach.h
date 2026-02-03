// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/lpsreach.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_LPSREACH_H
#define MCRL2_LPS_LPSREACH_H

#include "mcrl2/symbolic/data_index.h"
#include "mcrl2/utilities/logger.h"
#ifdef MCRL2_ENABLE_SYLVAN

#include "mcrl2/lps/detail/configuration.h"
#include "mcrl2/lps/detail/instantiate_global_variables.h"
#include "mcrl2/lps/detail/replace_global_variables.h"
#include "mcrl2/lps/io.h"
#include "mcrl2/lps/one_point_rule_rewrite.h"
#include "mcrl2/lps/order_summand_variables.h"
#include "mcrl2/lps/replace_constants_by_variables.h"
#include "mcrl2/lps/resolve_name_clashes.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/symbolic_lts.h"
#include "mcrl2/symbolic/ordering.h"
#include "mcrl2/symbolic/print.h"
#include "mcrl2/symbolic/symbolic_reachability.h"
#include "mcrl2/utilities/parse_numbers.h"
#include "mcrl2/utilities/stack_array.h"
#include "mcrl2/utilities/stopwatch.h"

#include <sylvan_ldd.hpp>

#include <chrono>
#include <iomanip>
#include <boost/dynamic_bitset.hpp>

namespace mcrl2::lps {

namespace detail {
  class lpsreach_statistics_t
  {
    public:
      std::map<std::size_t, std::size_t> num_relprod_calls_per_group;
      std::map<std::size_t, std::map<std::size_t, long long>> num_new_states_per_group_per_step_nosat;
      std::map<std::size_t, std::map<std::size_t, std::vector<long long>>> num_new_states_per_group_per_step_sat;
      std::map<std::pair<std::size_t, std::size_t>, std::map<std::size_t, std::vector<long long>>>
        num_new_states_per_group_per_step_sat_chaining;

      // Statistics for number of LDD nodes of the sets visited and todo at the start of each step
      std::map<std::size_t, std::size_t> num_ldd_nodes_visited_per_step;
      std::map<std::size_t, std::size_t> num_ldd_nodes_todo_per_step;

      // Statistics for number of LDD nodes of the sets visited and todo at the start of each summand group in step
      std::map<std::size_t, std::map<std::size_t, std::size_t>> num_ldd_nodes_todo_per_group_per_step_nosat;
      std::map<std::size_t, std::map<std::size_t, std::vector<std::size_t>>> num_ldd_nodes_todo_per_group_per_step_sat;
      std::map<std::pair<std::size_t, std::size_t>, std::map<std::size_t, std::vector<std::size_t>>>
        num_ldd_nodes_todo_per_group_per_step_sat_chaining;

      std::size_t num_rewrite_action_calls = 0;
      std::size_t num_learn_successors_calls = 0;
      std::size_t num_step_calls = 0;

      template <typename Rewriter, typename Substitution>
      void log_rewrite_action(const lps::multi_action& /*a*/, const Rewriter& /*rewr*/, const Substitution& /*sigma*/)
      {
        if constexpr (mcrl2::lps::detail::EnableSymbolicExplorationStatistics)
        {
          ++num_rewrite_action_calls;
        }
      }

      void log_learn_successors(std::size_t /*i*/, const symbolic::summand_group& /*R*/, const sylvan::ldds::ldd& /*X*/)
      {
        if constexpr (mcrl2::lps::detail::EnableSymbolicExplorationStatistics)
        {
          ++num_learn_successors_calls;
        }
      }

      void log_relprod(
        const sylvan::ldds::ldd& /*U*/,
        const lps_summand_group& /*group*/,
        std::size_t i)
      {
        if constexpr (mcrl2::lps::detail::EnableSymbolicExplorationStatistics)
        {
          auto it = num_relprod_calls_per_group.find(i);
          if (it == num_relprod_calls_per_group.end())
          {
            num_relprod_calls_per_group[i] = 1;
          }
          else
          {
            ++(it->second);
          }
        }
      }

      void log_step(
        const sylvan::ldds::ldd& visited,
        const sylvan::ldds::ldd& todo,
        bool /*learn_transitions*/,
        bool /*detect_deadlocks*/)
      {
        if constexpr (mcrl2::lps::detail::EnableSymbolicExplorationStatistics)
        {
          ++num_step_calls;
          num_ldd_nodes_visited_per_step[num_step_calls] = sylvan::ldds::nodecount(visited);
          num_ldd_nodes_todo_per_step[num_step_calls] = sylvan::ldds::nodecount(todo);
        }
      }

      /// \brief Logs statistics for each summand group in step without saturation.
      /// \param visited The set of visited before applying transition group i.
      /// \param relprod The result of applying transition group i.
      /// \param i The index of the summand group.
      void log_step_iter_nosat(
        const sylvan::ldds::ldd& todo,
        const sylvan::ldds::ldd& succ,
        std::size_t i)
      {
        if constexpr (mcrl2::lps::detail::EnableSymbolicExplorationStatistics)
        {
          std::optional<long long> num_new = symbolic::safe_llround(sylvan::ldds::satcount(sylvan::ldds::minus(succ, todo)));
          if (!num_new.has_value())
          {
            num_new = std::numeric_limits<long long>::max();
            mCRL2log(log::warning) << "Failed to compute number of new states in log_step_iter_nosat for group " << i << " using " << std::numeric_limits<long long>::max() << std::endl;
          }

          auto states_step_it = num_new_states_per_group_per_step_nosat.find(num_step_calls);
          if (states_step_it == num_new_states_per_group_per_step_nosat.end())
          {
            num_new_states_per_group_per_step_nosat[num_step_calls] = std::map<std::size_t, long long>({{i, num_new.value()}});;
          }
          else
          {
            states_step_it->second[i] = num_new.value();
          }

          auto todo_nodes_step_it = num_ldd_nodes_todo_per_group_per_step_nosat.find(num_step_calls);
          if (todo_nodes_step_it == num_ldd_nodes_todo_per_group_per_step_nosat.end())
          {
            num_ldd_nodes_todo_per_group_per_step_nosat[num_step_calls]
              = std::map<std::size_t, std::size_t>({{i, sylvan::ldds::nodecount(todo)}});
          }
          else
          {
            todo_nodes_step_it->second[i] = sylvan::ldds::nodecount(todo);
          }

        }
      }

      void log_step_iter_sat(const sylvan::ldds::ldd& todo, const sylvan::ldds::ldd& succ, std::size_t i)
      {
        if constexpr (mcrl2::lps::detail::EnableSymbolicExplorationStatistics)
        {
          std::optional<long long> num_new
            = symbolic::safe_llround(sylvan::ldds::satcount(sylvan::ldds::minus(succ, todo)));
          if (!num_new.has_value())
          {
            num_new = std::numeric_limits<long long>::max();
            mCRL2log(log::warning) << "Failed to compute number of new states in log_step_iter_sat for group " << i
                                   << " using " << std::numeric_limits<long long>::max() << std::endl;
          }

          auto states_step_it = num_new_states_per_group_per_step_sat.find(num_step_calls);
          if (states_step_it == num_new_states_per_group_per_step_sat.end())
          {
            num_new_states_per_group_per_step_sat[num_step_calls] = std::map<std::size_t, std::vector<long long>>({{i, {num_new.value()}}});
          }
          else
          {
            states_step_it->second[i].push_back(num_new.value());
          }

          auto nodes_todo_step_it = num_ldd_nodes_todo_per_group_per_step_sat.find(num_step_calls);
          if (nodes_todo_step_it == num_ldd_nodes_todo_per_group_per_step_sat.end())
          {
            num_ldd_nodes_todo_per_group_per_step_sat[num_step_calls]
              = std::map<std::size_t, std::vector<std::size_t>>({{i, {sylvan::ldds::nodecount(todo)}}});
          }
          else
          {
            states_step_it->second[i].push_back(sylvan::ldds::nodecount(todo));
          }
        }
      }

      void log_step_iter_sat_chaining(const sylvan::ldds::ldd& todo, const sylvan::ldds::ldd& succ, std::size_t i, std::size_t j)
      {
        if constexpr (mcrl2::lps::detail::EnableSymbolicExplorationStatistics)
        {
          std::optional<long long> num_new
            = symbolic::safe_llround(sylvan::ldds::satcount(sylvan::ldds::minus(succ, todo)));
          if (!num_new.has_value())
          {
            num_new = std::numeric_limits<long long>::max();
            mCRL2log(log::warning) << "Failed to compute number of new states in log_step_iter_sat for group " << i
                                   << " using " << std::numeric_limits<long long>::max() << std::endl;
          }

          auto states_step_it = num_new_states_per_group_per_step_sat_chaining.find({num_step_calls, i});
          if (states_step_it == num_new_states_per_group_per_step_sat_chaining.end())
          {
            num_new_states_per_group_per_step_sat_chaining[{num_step_calls, i}]
              = std::map<std::size_t, std::vector<long long>>({{j, {num_new.value()}}});
          }
          else
          {
            states_step_it->second[j].push_back(num_new.value());
          }

          auto todo_nodes_step_it = num_ldd_nodes_todo_per_group_per_step_sat_chaining.find({num_step_calls, i});
          if (todo_nodes_step_it == num_ldd_nodes_todo_per_group_per_step_sat_chaining.end())
          {
            num_ldd_nodes_todo_per_group_per_step_sat_chaining[{num_step_calls, i}]
              = std::map<std::size_t, std::vector<std::size_t>>({{j, {sylvan::ldds::nodecount(todo)}}});
          }
          else
          {
            states_step_it->second[j].push_back(sylvan::ldds::nodecount(todo));
          }
        }
      }

      std::ostream& print(std::ostream& out, bool saturation, bool chaining)
      {
        out << "Number of rewrite_action calls: " << num_rewrite_action_calls << std::endl;
        out << "Number of learn_successors calls: " << num_learn_successors_calls << std::endl;
        out << "Number of step calls: " << num_step_calls << std::endl;
        out << "Number of relprod calls per group:" << std::endl;
        for (const auto& [group, count] : num_relprod_calls_per_group)
        {
          out << "  Group " << group << ": " << count << std::endl;
        }
        if(saturation)
        {
          out << "Number of new states per group per step (saturation):" << std::endl;
          for (const auto& [step, group_map]: num_new_states_per_group_per_step_sat)
          {
            out << "  Step " << step << ":" << std::endl;
            for (const auto& [group, counts]: group_map)
            {
              out << "    Group " << group << ": ";
              for (const auto& count: counts)
              {
                out << count << " ";
              }
              out << std::endl;
              if (chaining)
              {
                out << "      Chaining steps for group " << group << ": " << std::endl;
                for (const auto& [chain_group, counts]: num_new_states_per_group_per_step_sat_chaining[{step, group}])
                {
                  out << "        Group " << chain_group << ": ";
                  for (const auto& count: counts)
                  {
                    out << count << " ";
                  }
                  out << std::endl;
                }
              }
            }
          }
        }
        else
        {
          if (chaining)
          {
            out << "Number of new states per group per step (no saturation, chaining):" << std::endl;
          }
          else
          {
            out << "Number of new states per group per step (no saturation, no chaining):" << std::endl;
          }
          for (const auto& [step, group_map] : num_new_states_per_group_per_step_nosat)
          {
            out << "  Step " << step << ":" << std::endl;
            for (const auto& [group, count] : group_map)
            {
              out << "    Group " << group << ": " << count << std::endl;
            }
          }
        }

        return out;
      }

      void log(const bool saturation, const bool chaining)
      {
        if constexpr (mcrl2::lps::detail::EnableSymbolicExplorationStatistics)
        {
          mCRL2log(log::debug) << "=== Symbolic exploration statistics ===" << std::endl;
          print(log::logger(log::debug).get(), saturation, chaining);
        }
      }

      std::ostream& output_json(std::ostream& os, bool saturation, bool chaining)
      {
        if constexpr (mcrl2::lps::detail::EnableSymbolicExplorationStatistics)
        {
          os << "{";
          output_json("num_rewrite_action_calls", os);
          os << ": ";
          output_json(num_rewrite_action_calls, os);

          os << ", ";
          output_json("num_learn_successors_calls", os);
          os << ": ";
          output_json(num_learn_successors_calls, os);

          os << ", ";
          output_json("num_step_calls", os);
          os << ": ";
          output_json(num_step_calls, os);

          os << ", ";
          output_json("num_relprod_calls_per_group", os);
          os << ": ";
          output_json(num_relprod_calls_per_group, os);

          if (saturation)
          {
            os << ", ";
            output_json("num_new_states_per_group_per_step_sat", os);
            os << ": ";
            output_json(num_new_states_per_group_per_step_sat, os);

            if (chaining)
            {
              os << ", ";
              output_json("num_new_states_per_group_per_step_sat_chaining", os);
              os << ": ";
              output_json(num_new_states_per_group_per_step_sat_chaining, os);
            }
          }
          else
          {
            os << ", ";
            output_json("num_new_states_per_group_per_step_nosat", os);
            os << ": ";
            output_json(num_new_states_per_group_per_step_nosat, os);
          }

          os << ", ";
          output_json("num_ldd_nodes_visited_per_step", os);
          os << ": ";
          output_json(num_ldd_nodes_visited_per_step, os);

          os << ", ";
          output_json("num_ldd_nodes_todo_per_step", os);
          os << ": ";
          output_json(num_ldd_nodes_todo_per_step, os);

          if (saturation)
          {
            os << ", ";
            output_json("num_ldd_nodes_todo_per_group_per_step_sat", os);
            os << ": ";
            output_json(num_ldd_nodes_todo_per_group_per_step_sat, os);

            if (chaining) {
              os << ", ";
              output_json("num_ldd_nodes_todo_per_group_per_step_sat_chaining", os);
              os << ": ";
              output_json(num_ldd_nodes_todo_per_group_per_step_sat_chaining, os);
            }
          }
          else
          {
            os << ", ";
            output_json("num_ldd_nodes_todo_per_group_per_step_nosat", os);
            os << ": ";
            output_json(num_ldd_nodes_todo_per_group_per_step_nosat, os);
          }

          os << "}";
        }

        return os;
      }

    protected:
      std::ostream& output_json(const std::string& s, std::ostream& o)
      {
        o << "\"" << s << "\"";
        return o;
      }

      template<std::integral T>
      std::ostream& output_json(const T& n, std::ostream& o)
      {
        o << n;
        return o;
      }

      template <typename T1, typename T2>
      std::ostream& output_json(const std::pair<T1, T2>& p, std::ostream& o)
      {
        o << "[";
        output_json(p.first, o);
        o << ", ";
        output_json(p.second, o);
        o << "]";
        return o;
      }

      template <typename T>
      std::ostream& output_json(const std::vector<T>& v, std::ostream& o)
      {
        o << "[";
        bool first = true;
        for (const auto& item : v)
        {
          if (!first)
          {
            o << ", ";
          }
          first = false;
          output_json(item, o);
        }
        o << "]";
        return o;
      }

      template<typename T1, typename T2>
      std::ostream& output_json(const std::map<T1, T2>& m, std::ostream& o)
      {
        o << "[";
        bool first = true;
        for (const auto& [key, value] : m)
        {
          if (!first)
          {
            o << ", ";
          }
          first = false;
          o << "[";
          output_json(key, o);
          o << ", ";
          output_json(value, o);
          o << "]";
        }
        o << "]";
        return o;
      }

      template <typename T>
      std::ostream& output_json(const std::map<std::string, T>& m, std::ostream& o)
      {
        o << "{";
        bool first = true;
        for (const auto& [key, value]: m)
        {
          if (!first)
          {
            o << ", ";
          }
          output_json(key, o);
          o << ": ";
          output_json(value, o);
        }
        o << "}";
        return o;
      }
  };
}

class lpsreach_algorithm
{
    using ldd = sylvan::ldds::ldd;
    using enumerator_element = data::enumerator_list_element_with_substitution<>;

    template <typename Context, bool ActionLabel>
    friend void symbolic::learn_successors_callback(WorkerP*, Task*, std::uint32_t* v, std::size_t n, void* context);

  protected:
    const symbolic::symbolic_reachability_options& m_options;
    data::rewriter m_rewr;
    data::mutable_indexed_substitution<> m_sigma;
    data::enumerator_identifier_generator m_id_generator;
    data::enumerator_algorithm<> m_enumerator;
    std::vector<boost::dynamic_bitset<>> m_summand_patterns;
    std::vector<boost::dynamic_bitset<>> m_group_patterns;
    std::vector<std::size_t> m_variable_order;
    symbolic_lts m_lts;
    detail::lpsreach_statistics_t m_stats;

      /// \brief Rewrites all arguments of the given action.
      template<typename Rewriter, typename Substitution>
      lps::multi_action rewrite_action(const lps::multi_action& a, const Rewriter& rewr, const Substitution& sigma)
    {
      m_stats.log_rewrite_action(a, rewr, sigma);

      const process::action_list& actions = a.actions();
      const data::data_expression& time = a.time();
      return
        lps::multi_action(
          process::action_list(
            actions.begin(),
            actions.end(),
            [&](const process::action& a)
            {
              const auto& args = a.arguments();
              return process::action(a.label(), data::data_expression_list(args.begin(), args.end(), [&](const data::data_expression& x) { return rewr(x, sigma); }));
            }
          ),
          a.has_time() ? rewr(time, sigma) : time
        );
    }

    // R.L := R.L U {(x,y) in R | x in X}
    void learn_successors(std::size_t i, symbolic::summand_group& R, const ldd& X)
    {
      m_stats.log_learn_successors(i, R, X);
      mCRL2log(log::trace) << "learn successors of summand group " << i
                           << " for X = " << print_states(m_lts.data_index, X, R.read) << std::endl;


      using namespace sylvan::ldds;
      std::pair<lpsreach_algorithm&, symbolic::summand_group&> context{*this, R};
      sat_all_nopar(X, symbolic::learn_successors_callback<std::pair<lpsreach_algorithm&, lps_summand_group&>, true>, &context);
    }

    template <typename Specification>
    Specification preprocess(const Specification& lpsspec)
    {
      Specification result = lpsspec;
      if (m_options.replace_dont_care)
      {
        lps::detail::replace_global_variables(result);
      }
      lps::detail::instantiate_global_variables(result);
      lps::order_summand_variables(result);
      resolve_summand_variable_name_clashes(result); // N.B. This is a required preprocessing step.
      if (m_options.one_point_rule_rewrite)
      {
        one_point_rule_rewrite(result);
      }
      if (m_options.replace_constants_by_variables)
      {
        replace_constants_by_variables(result, m_rewr, m_sigma);
      }

      return result;
    }

    std::string print_size(const sylvan::ldds::ldd& L)
    {
      return symbolic::print_size(L, m_options.print_exact, m_options.print_nodesize);
    }

  public:
    lpsreach_algorithm(const lps::specification& lpsspec, const symbolic::symbolic_reachability_options& options_)
      : m_options(options_),
        m_rewr(symbolic::construct_rewriter(lpsspec.data(), m_options.rewrite_strategy, lps::find_function_symbols(lpsspec), m_options.remove_unused_rewrite_rules)),
        m_enumerator(m_rewr, lpsspec.data(), m_rewr, m_id_generator, false)
    {
      using utilities::detail::as_vector;

      lps::specification lpsspec_ = preprocess(lpsspec);
      m_lts.process_parameters = lpsspec_.process().process_parameters();

      // Rewrite the initial expressions to normal form,
      std::vector<data::data_expression> initial_values;
      for (const data::data_expression& expression : lpsspec_.initial_process().expressions())
      {
        initial_values.push_back(m_rewr(expression));
      }

      data::data_expression_list initial_state(initial_values.begin(), initial_values.end());

      m_summand_patterns = compute_read_write_patterns(lpsspec_);
      mCRL2log(log::debug) << "Original read/write matrix:" << std::endl;
      mCRL2log(log::debug) << symbolic::print_read_write_patterns(m_summand_patterns);

      symbolic::adjust_read_write_patterns(m_summand_patterns, m_options);

      m_variable_order = symbolic::compute_variable_order(m_options.variable_order, m_lts.process_parameters.size(), m_summand_patterns);
      mCRL2log(log::debug) << "variable order = " << core::detail::print_list(m_variable_order) << std::endl;
      m_summand_patterns = symbolic::reorder_read_write_patterns(m_summand_patterns, m_variable_order);

      m_lts.process_parameters = symbolic::permute_copy(m_lts.process_parameters, m_variable_order);
      for (const data::variable& param : m_lts.process_parameters)
      {
        m_lts.data_index.emplace_back(param.sort());
      }

      m_lts.initial_state = symbolic::state2ldd(symbolic::permute_copy(initial_state, m_variable_order), m_lts.data_index);
      mCRL2log(log::debug) << "process parameters = " << core::detail::print_list(m_lts.process_parameters) << std::endl;

      std::vector<std::set<std::size_t>> groups = symbolic::compute_summand_groups(m_options.summand_groups, m_summand_patterns);
      for (const auto& group: groups)
      {
        mCRL2log(log::debug) << "group " << core::detail::print_set(group) << std::endl;
      }
      m_group_patterns = symbolic::compute_summand_group_patterns(m_summand_patterns, groups);
      for (std::size_t j = 0; j < m_group_patterns.size(); j++)
      {
        m_lts.summand_groups.emplace_back(lpsspec_, m_lts.process_parameters, groups[j], m_group_patterns[j], m_summand_patterns, m_variable_order);
      }

      for (std::size_t i = 0; i < m_lts.summand_groups.size(); i++)
      {
        mCRL2log(log::debug) << "=== summand group " << i << " ===\n" << m_lts.summand_groups[i] << std::endl;
      }

      mCRL2log(log::debug) << "Final read/write matrix:" << std::endl;
      mCRL2log(log::debug) << symbolic::print_read_write_patterns(m_summand_patterns);
    }

    /// \brief Computes relprod(U, group).
    ldd relprod_impl(const ldd& U, const lps_summand_group& group, std::size_t i)
    {
      m_stats.log_relprod(U, group, i);
      if (m_options.no_relprod)
      {
        ldd z = symbolic::alternative_relprod(U, group);
        mCRL2log(log::trace) << "relprod(" << i << ", todo) = " << print_states(m_lts.data_index, z) << std::endl;
        return z;
      }
      else
      {
        ldd z = relprod(U, group.L, group.Ir);
        mCRL2log(log::trace) << "relprod(" << i << ", todo) = " << print_states(m_lts.data_index, z) << std::endl;
        return z;
      }
    }

    /// \brief Perform a single breadth first step.
    /// \returns The tuple <visited, todo, deadlocks>
    std::tuple<ldd, ldd, ldd> step(const ldd& visited, const ldd& todo, bool learn_transitions = true, bool detect_deadlocks = false)
    {
      m_stats.log_step(visited, todo, learn_transitions, detect_deadlocks);
      using namespace sylvan::ldds;
      auto& R = m_lts.summand_groups;

      ldd todo1 = empty_set();
      ldd potential_deadlocks = detect_deadlocks ? todo : empty_set();

      if (!m_options.saturation)
      {
        // regular and chaining.
        todo1 = m_options.chaining ? todo : empty_set();

        for (std::size_t i = 0; i < R.size(); i++)
        {
          if (learn_transitions)
          {
            ldd proj = project(m_options.chaining ? todo1 : todo, R[i].Ip);
            learn_successors(i, R[i], m_options.cached ? minus(proj, R[i].Ldomain) : proj);

            mCRL2log(log::trace) << "L =\n" << print_relation(m_lts.data_index, R[i].L, R[i].read, R[i].write) << std::endl;
          }

          ldd step_i = relprod_impl(m_options.chaining ? todo1 : todo, R[i], i);
          m_stats.log_step_iter_nosat(todo1, step_i, i);
          todo1 = union_(todo1, step_i);


          if (detect_deadlocks)
          {
            potential_deadlocks = minus(potential_deadlocks, relprev(todo1, R[i].L, R[i].Ir, potential_deadlocks));
          }
        }
      }
      else
      {
        // saturation and chaining
        todo1 = todo;
        ldd todo1_old; // the old todo set.

        for (std::size_t i = 0; i < R.size(); i++)
        {
          if (learn_transitions)
          {
            ldd proj = project(todo1, R[i].Ip);
            learn_successors(i, R[i], m_options.cached ? minus(proj, R[i].Ldomain) : proj);

            mCRL2log(log::trace) << "L =\n" << print_relation(m_lts.data_index, R[i].L, R[i].read, R[i].write) << std::endl;
          }

          // Apply one transition relation repeatedly.
          do
          {
            todo1_old = todo1;
            ldd succ_i = relprod_impl(todo1, R[i], i);
            m_stats.log_step_iter_sat(todo1, succ_i, i);
            todo1 = union_(todo1, succ_i);
          }
          while (todo1 != todo1_old);

          if (detect_deadlocks)
          {
            potential_deadlocks = minus(potential_deadlocks, relprev(todo1, R[i].L, R[i].Ir, potential_deadlocks));
          }

          // Apply all previously learned transition relations repeatedly.
          if (m_options.chaining)
          {
            do
            {
              todo1_old = todo1;
              for (std::size_t j = 0; j <= i; j++)
              {
                ldd succ_j = relprod_impl(todo1, R[j], j);
                m_stats.log_step_iter_sat_chaining(todo1, succ_j, i, j);
                todo1 = union_(todo1, succ_j);
              }
            }
            while (todo1 != todo1_old);
          }
        }
      }

      // after all transition groups are applied the remaining potential deadlocks are actual deadlocks.
      return std::make_tuple(union_(visited, todo), minus(todo1, visited), potential_deadlocks);
    }

    ldd run()
    {
      using namespace sylvan::ldds;
      auto& R = m_lts.summand_groups;
      std::size_t iteration_count = 0;

      mCRL2log(log::trace) << "initial state = " << core::detail::print_list(m_lts.initial_state) << std::endl;

      auto start = std::chrono::steady_clock::now();
      ldd x = m_lts.initial_state;
      std::chrono::duration<double> elapsed_seconds = std::chrono::steady_clock::now() - start;
      ldd visited = empty_set();
      ldd todo = x;
      ldd deadlocks = empty_set();
      ldd potential_deadlocks = empty_set();

      while (todo != empty_set() && (m_options.max_iterations == 0 || iteration_count < m_options.max_iterations))
      {
        stopwatch loop_start;
        iteration_count++;
        mCRL2log(log::trace) << "--- iteration " << iteration_count << " ---" << std::endl;
        mCRL2log(log::trace) << "todo = " << print_states(m_lts.data_index, todo) << std::endl;

        std::tie(visited, todo, potential_deadlocks) = step(visited, todo, true, m_options.detect_deadlocks);

        mCRL2log(log::verbose) << "explored " << std::setw(12) << print_size(union_(visited, todo)) << " states after "
                               << std::setw(3) << iteration_count << " iterations (time = " << std::setprecision(2)
                               << std::fixed << loop_start.seconds() << "s)" << std::endl;
        if (m_options.detect_deadlocks)
        {
          deadlocks = union_(deadlocks, potential_deadlocks);
          mCRL2log(log::verbose) << "found " << std::setw(12) << print_size(deadlocks) << " deadlocks" << std::endl;
        }

        sylvan::sylvan_stats_report(stderr);
      }

      elapsed_seconds = std::chrono::steady_clock::now() - start;
      std::cout << "number of states = " << print_size(visited) << " (time = " << std::setprecision(2) << std::fixed << elapsed_seconds.count() << "s)" << std::endl;
      mCRL2log(log::verbose) << "used variable order = " << core::detail::print_list(m_variable_order) << std::endl;

      double total_time = 0.0;
      for (std::size_t i = 0; i < R.size(); i++)
      {
        mCRL2log(log::verbose) << "group " << std::setw(4) << i << " contains " << std::setw(10) << print_size(R[i].L) << " transitions (learn time = "
                               << std::setw(5) << std::setprecision(2) << std::fixed << R[i].learn_time << "s with " << std::setw(9) << R[i].learn_calls
                               << " calls, cached " << print_size(R[i].Ldomain) << " values" << ")" << std::endl;

        total_time += R[i].learn_time;
      }
      mCRL2log(log::verbose) << "learning transitions took " << total_time << "s" << std::endl;

      std::size_t i = 0;
      for (const auto& param : m_lts.process_parameters)
      {
        auto& table = m_lts.data_index[i];

        mCRL2log(log::verbose) << "Parameter " << i << " (" << param << ")" << " has " << table.size() << " values."<< std::endl;
        for (const auto& data : table)
        {
          mCRL2log(log::debug) << table.index(data) << ": " << data << std::endl;
        }

        ++i;
      }

      mCRL2log(log::verbose) << "There are " << m_lts.action_index.size() << " action labels" << std::endl;
      for (const auto& action : m_lts.action_index)
      {
          mCRL2log(log::debug) << m_lts.action_index.index(action) << ": " << action << std::endl;
      }

      m_stats.output_json(std::cout, m_options.saturation, m_options.chaining);

      m_lts.states = visited;
      return visited;
    }

    const std::vector<boost::dynamic_bitset<>>& read_write_patterns() const
    {
      return m_summand_patterns;
    }

    const std::vector<boost::dynamic_bitset<>>& read_write_group_patterns() const
    {
      return m_group_patterns;
    }

    std::vector<symbolic::data_expression_index>& data_index()
    {
      return m_lts.data_index;
    }

    utilities::indexed_set<lps::multi_action>& action_index()
    {
      return m_lts.action_index;
    }

    const symbolic_lts& get_symbolic_lts()
    {
      return m_lts;
    }
};

} // namespace mcrl2::lps

#endif // MCRL2_ENABLE_SYLVAN

#endif // MCRL2_LPS_LPSREACH_H
