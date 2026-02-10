// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/lpsreach_stats.h
/// \brief Facilities to collect statistics during symbolic reachability.

#ifndef MCRL2_LPS_LPSREACH_STATS_H
#define MCRL2_LPS_LPSREACH_STATS_H

#include <map>
#include <vector>

#include "mcrl2/lps/detail/configuration.h"

#include "mcrl2/lps/multi_action.h"
#include "mcrl2/symbolic/print.h"
#include "mcrl2/lps/lps_summand_group.h"
#include "sylvan_ldd.hpp"

    namespace mcrl2::lps::detail
{
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

  template<typename Rewriter, typename Substitution>
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

  void log_relprod(const sylvan::ldds::ldd& /*U*/, const lps_summand_group& /*group*/, std::size_t i)
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

  void log_step(const sylvan::ldds::ldd& visited,
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
  void log_step_iter_nosat(const sylvan::ldds::ldd& todo, const sylvan::ldds::ldd& succ, std::size_t i)
  {
    if constexpr (mcrl2::lps::detail::EnableSymbolicExplorationStatistics)
    {
      std::optional<long long> num_new
        = symbolic::safe_llround(sylvan::ldds::satcount(sylvan::ldds::minus(succ, todo)));
      if (!num_new.has_value())
      {
        num_new = std::numeric_limits<long long>::max();
        mCRL2log(log::warning) << "Failed to compute number of new states in log_step_iter_nosat for group " << i
                               << " using " << std::numeric_limits<long long>::max() << std::endl;
      }

      auto states_step_it = num_new_states_per_group_per_step_nosat.find(num_step_calls);
      if (states_step_it == num_new_states_per_group_per_step_nosat.end())
      {
        num_new_states_per_group_per_step_nosat[num_step_calls]
          = std::map<std::size_t, long long>({{i, num_new.value()}});
        ;
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
        num_new_states_per_group_per_step_sat[num_step_calls]
          = std::map<std::size_t, std::vector<long long>>({{i, {num_new.value()}}});
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

  void
  log_step_iter_sat_chaining(const sylvan::ldds::ldd& todo, const sylvan::ldds::ldd& succ, std::size_t i, std::size_t j)
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
    for (const auto& [group, count]: num_relprod_calls_per_group)
    {
      out << "  Group " << group << ": " << count << std::endl;
    }
    if (saturation)
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
      for (const auto& [step, group_map]: num_new_states_per_group_per_step_nosat)
      {
        out << "  Step " << step << ":" << std::endl;
        for (const auto& [group, count]: group_map)
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

        if (chaining)
        {
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

  template<typename T1, typename T2>
  std::ostream& output_json(const std::pair<T1, T2>& p, std::ostream& o)
  {
    o << "[";
    output_json(p.first, o);
    o << ", ";
    output_json(p.second, o);
    o << "]";
    return o;
  }

  template<typename T>
  std::ostream& output_json(const std::vector<T>& v, std::ostream& o)
  {
    o << "[";
    bool first = true;
    for (const auto& item: v)
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
    for (const auto& [key, value]: m)
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

  template<typename T>
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
} // namespace detail

#endif // MCRL2_LPS_LPSREACH_STATS_H
