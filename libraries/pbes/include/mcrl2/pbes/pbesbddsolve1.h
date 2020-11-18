// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/pbesbddsolve.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_PBESBDDSOLVE_H
#define MCRL2_PBES_PBESBDDSOLVE_H

#include <algorithm>
#include <cassert>
#include <utility>
#include "mcrl2/pbes/pbes_equation_index.h"
#include "mcrl2/pbes/srf_pbes.h"
#include "mcrl2/utilities/execution_timer.h"
#include "mcrl2/utilities/text_utility.h"
#include "sylvan_bdd.hpp"

using namespace sylvan::bdds;

namespace mcrl2 {

namespace pbes_system {

enum class bdd_granularity
{
    per_pbes,
    per_equation,
    per_summand
};

inline
std::vector<data::variable> add_underscore(const std::vector<data::variable>& v)
{
  std::vector<data::variable> result;
  result.reserve(v.size());
  for (const auto& x: v)
  {
    result.emplace_back(std::string(x.name()) + "_", x.sort());
  }
  return result;
}

inline
void print(const std::string& msg, const bdd& x, const bdd& context)
{
  std::cout << msg << " " << satcount(x, context) << std::endl;
}

inline
void print(const std::string& msg, const std::vector<bdd>& x, const bdd& context)
{
  std::cout << msg;
  for (const auto& xi: x)
  {
    std::cout << " " << satcount(xi, context);
  }
  std::cout << std::endl;
}

inline
void print(const std::string& msg, const std::map<std::uint32_t, bdd>& m, const bdd& context)
{
  std::cout << msg;
  for (const auto& [i, x]: m)
  {
    std::cout << " (" << i << ", " << satcount(x, context) << ")";
  }
  std::cout << std::endl;
}

// returns the smallest value m such that n <= 2**m
inline
std::size_t log2_rounded_up(std::size_t n)
{
  std::size_t m = 0;
  std::size_t value = 1;
  for (;;)
  {
    if (n <= value)
    {
      break;
    }
    m++;
    value *= 2;
  }
  return m;
}

class bdd_parity_game
{
  public:
    static constexpr bool even = false;
    static constexpr bool odd = true;

  private:
    const bdd& m_variables;
    const bdd& m_next_variables;
    const bdd& m_all_variables;
    const bdd_substitution& m_next_substitution;
    const bdd_substitution& m_prev_substitution;
    bdd m_V;
    std::vector<bdd> m_E;
    bdd m_even;
    bdd m_odd;
    std::map<std::uint32_t, bdd> m_priorities;
    bdd m_initial_state;
    bool m_use_sylvan_optimization;

    void info(const bdd& x, const std::string& msg, const bdd& variables) const
    {
      std::cout << msg << " " << satcount(x, variables) << std::endl;
    }

  public:
    void print() const
    {
      info(m_V, "V", m_variables);
      info(any(m_E), "E", m_all_variables);
      info(m_even, "even", m_variables);
      info(m_odd, "odd", m_variables);
      info(m_initial_state, "initial state", m_variables);
      for (const auto& [prio, x]: m_priorities)
      {
        info(x, "priority " + std::to_string(prio), m_variables);
      }
    }

    bdd_parity_game(
      const bdd& variables,
      const bdd& next_variables,
      const bdd& all_variables,
      const bdd_substitution& next_substitution,
      const bdd_substitution& prev_substitution,
      const bdd& V,
      const std::vector<bdd>& E,
      const bdd& even_nodes,
      const bdd& odd_nodes,
      const std::map<std::uint32_t, bdd>& priorities,
      const bdd& initial_state,
      bool use_sylvan_optimization = false
     )
    : m_variables(variables),
      m_next_variables(next_variables),
      m_all_variables(all_variables), m_next_substitution(next_substitution),
          m_prev_substitution(prev_substitution),
      m_V(V),
      m_E(E),
      m_even(even_nodes),
      m_odd(odd_nodes),
      m_priorities(priorities),
      m_initial_state(initial_state),
      m_use_sylvan_optimization(use_sylvan_optimization)
    {}

    // If optimized is true, the Sylvan RelPrev operator is applied to improve efficiency
    bdd predecessor(bool player, const bdd& U, bool optimized = false)
    {
      bdd V_player = (player == even) ? m_even : m_odd;
      bdd V_opponent = (player == odd) ? m_even : m_odd;

      std::vector<bdd> W_player;
      for (const auto& Ei: m_E)
      {
        W_player.push_back(relation_backward(
            Ei, U, m_next_variables, m_next_substitution, optimized));
      }
      bdd U_player = V_player & any(W_player);

      std::vector<bdd> W_opponent;
      for (const auto& Ei: m_E)
      {
        W_opponent.push_back(V_opponent & ~relation_backward(
                                              Ei, m_V & ~U, m_next_variables,
                                              m_next_substitution, optimized));
      }
      bdd U_opponent = all(W_opponent);

      return U_player | U_opponent;
    }

    // U is a BDD representing a set of vertices
    // player is either string 'even' or string 'odd'
    // attractor computation is a least fixpoint computation
    bdd attractor(bool player, const bdd& A)
    {
      mCRL2log(log::debug) << "attractor" << std::endl;
      std::size_t count = 0;

      bdd tmp = sylvan::bdds::false_();
      bdd tmp_ = A;
      while (tmp != tmp_)
      {
        mCRL2log(log::debug) << count++ << std::endl;
        tmp = tmp_;
        tmp_ = tmp_ | predecessor(player, tmp_, m_use_sylvan_optimization);
      }
      return tmp;
    }

    // removing a set of vertices represented by BDD A
    void remove(const bdd& A)
    {
      m_V = m_V & ~A;
      m_even = m_even & ~A;
      m_odd = m_odd & ~A;
      bdd A_next = let(m_next_substitution, A);

      //----------------------------------------------//
      // bdd AA = ~A & ~A_; // N.B. this turns out to be very inefficient in some cases
      // for (auto& Ei: m_E)
      // {
      //   Ei = Ei & AA;
      // }
      //----------------------------------------------//

      for (auto& Ei: m_E)
      {
        Ei = Ei & ~A & ~A_next;
      }

      std::map<std::uint32_t, bdd> priorities;
      for (const auto& [i, p_i]: m_priorities)
      {
        auto priority = p_i & ~A;
        if (priority != sylvan::bdds::false_())
        {
          priorities[i] = priority;
        }
      }
      m_priorities = priorities;
    }

    std::size_t game_size() const
    {
      return satcount(m_V, m_variables);
    }

    std::uint32_t maximum() const
    {
      std::uint32_t result = 0;
      for (const auto& [i, p_i]: m_priorities)
      {
        result = std::max(result, i);
      }
      return result;
    }

    const bdd& initial_state() const
    {
      return m_initial_state;
    }

    const bdd& all_variables() const
    {
      return m_all_variables;
    }

    const bdd& next_variables() const
    {
      return m_next_variables;
    }

    const bdd& variables() const
    {
      return m_variables;
    }

    const bdd_substitution& substitution() const
    {
      return m_next_substitution;
    }

    const bdd_substitution& reverse_substitution() const
    {
      return m_prev_substitution;
    }

    const bdd& nodes() const
    {
      return m_V;
    }

    const std::vector<bdd>& edges() const
    {
      return m_E;
    }

    const bdd& even_nodes() const
    {
      return m_even;
    }

    const bdd& odd_nodes() const
    {
      return m_odd;
    }

    const std::map<std::uint32_t, bdd>& priorities() const
    {
      return m_priorities;
    }

    // Perform reachability, return reachable states
    bdd reachable_vertices_default(const bdd& U)
    {
      bdd V_reachable = sylvan::bdds::false_();
      bdd V_reachable_ = U;
      std::size_t count = 0;
      while (V_reachable != V_reachable_)
      {
        V_reachable = V_reachable_;
        mCRL2log(log::verbose) << "  reachable_states_default iteration " << count++ << std::endl;
        for (const auto& Ei: m_E)
        {
          V_reachable_ = V_reachable_ | relation_forward(Ei, V_reachable_, m_variables, m_prev_substitution);
        }
      }
      return V_reachable;
    }

    // Perform reachability, return reachable states
    // exploit the partitioning to potentially more quickly explore the state space
    // Not reliably quicker (or slower)
    bdd reachable_vertices(const bdd& U)
    {
      bdd V_reachable = sylvan::bdds::false_();
      bdd V_frontier = U;
      std::vector<bdd> V_next;
      std::size_t count = 0;
      while ( (V_reachable | V_frontier) != V_reachable) // check whether V_frontier is a subset of V_reachable
      {
        V_reachable = V_reachable | V_frontier;
        mCRL2log(log::debug) << "  reachable_states iteration " << count++ << std::endl;
        V_next.clear();
        for (const auto& Ei: m_E)
        {
          V_next.push_back(relation_forward(Ei, V_frontier, m_variables, m_prev_substitution));
        }
        V_frontier = any(V_next);
      }
      return V_reachable;
    }

    std::pair<bdd, bdd> zielonka()
    {
      mCRL2log(log::debug) << "start zielonka" << std::endl;

      if ( (m_even | m_odd) == sylvan::bdds::false_())
      {
        mCRL2log(log::debug) << "finish zielonka" << std::endl;
        return { m_even, m_odd };
      }
      else
      {
        auto p = maximum();
        bool parity = (p % 2 == 0) ? even : odd;
        bdd U = m_priorities[p];
        bdd A = attractor(parity, U);
        bdd_parity_game G = *this;
        G.remove(A);
        auto [W0, W1] = G.zielonka();
        if (parity == even && W1 == sylvan::bdds::false_())
        {
          mCRL2log(log::debug) << "finish zielonka" << std::endl;
          return { W0 | A, W1 };
        }
        else if (parity == odd && W0 == sylvan::bdds::false_())
        {
          mCRL2log(log::debug) << "finish zielonka" << std::endl;
          return { W0, A | W1 };
        }
        else
        {
          auto [W, opponent] = (parity == even) ? std::make_pair(W1, odd) : std::make_pair(W0, even);
          bdd B = attractor(opponent, W);
          bdd_parity_game H = *this;
          H.remove(B);
          auto [X0, X1] = H.zielonka();
          if (parity == even)
          {
            mCRL2log(log::debug) << "finish zielonka" << std::endl;
            return { X0, X1 | B };
          }
          else
          {
            mCRL2log(log::debug) << "finish zielonka" << std::endl;
            return { X0 | B, X1 };
          }
        }
      }
    }
};

class pbesbddsolve
{
  private:
    class variable_manager
    {
      private:
        std::map<std::string, std::size_t> m_variable_index; // maps variable names to variable indices
        std::vector<bdd> m_variables; // maps variable indices to the corresponding bdd

      public:
        // Add a variable with the given name
        // Returns the corresponding bdd and variable index
        std::pair<bdd, std::size_t> add(const std::string& name)
        {
          std::size_t index = m_variable_index.size();
          bdd result = make_variable(index);
          m_variable_index[name] = index;
          m_variables.push_back(result);
          return { result, index };
        }

        const bdd& variable(const std::string& name) const
        {
          if (m_variable_index.find(name) == m_variable_index.end())
          {
            throw std::runtime_error("could not find variable " + name);
          }
          return m_variables[m_variable_index.at(name)];
        }
    };

    const srf_pbes& m_pbes;
    pbes_equation_index m_pbes_index;

    bool m_unary_encoding;
    bdd_granularity m_granularity = bdd_granularity::per_pbes;
    utilities::execution_timer* m_timer = nullptr;     // if it is non-zero, it will be used to display timing information
    variable_manager m_variable_manager;

    void start_timer(const std::string& msg) const
    {
      if (m_timer)
      {
        m_timer->start(msg);
      }
    }

    void finish_timer(const std::string& msg) const
    {
      if (m_timer)
      {
        m_timer->finish(msg);
      }
    }

    bdd to_bdd(const data::variable& x) const
    {
      return m_variable_manager.variable(x.name());
    }

    bdd to_bdd(const data::data_expression& x) const
    {
      if (data::is_variable(x) && data::is_bool(atermpp::down_cast<data::variable>(x).sort()))
      {
        const auto& x_ = atermpp::down_cast<data::variable>(x);
        return to_bdd(x_);
      }
      else if (data::is_true(x))
      {
        return sylvan::bdds::true_();
      }
      else if (data::is_false(x))
      {
        return sylvan::bdds::false_();
      }
      else if (data::is_not(x))
      {
        const auto& operand = data::sort_bool::arg(x);
        return ~to_bdd(operand);
      }
      else if (data::is_equal_to(x))
      {
        const auto& left  = data::binary_left1(x);
        const auto& right = data::binary_right1(x);
        return equiv(to_bdd(left), to_bdd(right));
      }
      else if (data::is_not_equal_to(x))
      {
        const auto& left  = data::binary_left1(x);
        const auto& right = data::binary_right1(x);
        return implies(to_bdd(left), !(to_bdd(right)));
      }
      else if (data::is_or(x))
      {
        const auto& left  = data::binary_left1(x);
        const auto& right = data::binary_right1(x);
        return to_bdd(left) | to_bdd(right);
      }
      else if (data::is_and(x))
      {
        const auto& left  = data::binary_left1(x);
        const auto& right = data::binary_right1(x);
        return to_bdd(left) & to_bdd(right);
      }
      else if (data::is_imp(x))
      {
        const auto& left  = data::binary_left1(x);
        const auto& right = data::binary_right1(x);
        return implies(to_bdd(left), to_bdd(right));
      }
      else if (is_if_application(x))
      {
        const auto& x_ = atermpp::down_cast<data::application>(x);
        const data::data_expression& condition = x_[0];
        const data::data_expression& then_ = x_[1];
        const data::data_expression& else_ = x_[2];
        return ite(to_bdd(condition), to_bdd(then_), to_bdd(else_));
      }
      throw mcrl2::runtime_error("Unsupported data expression " + data::pp(x) + " encountered in to_bdd.");
    }

    std::vector<bdd> to_bdd(const data::data_expression_list& v) const
    {
      std::vector<bdd> result;
      for (const data::data_expression& x: v)
      {
        result.push_back(to_bdd(x));
      }
      return result;
    }

    std::vector<bdd> to_bdd(const data::variable_list & v) const
    {
      std::vector<bdd> result;
      result.reserve(v.size());
      for (const data::variable& x: v)
      {
        result.push_back(to_bdd(x));
      }
      return result;
    }

    std::vector<bdd> to_bdd(const std::vector<data::variable>& v) const
    {
      std::vector<bdd> result;
      result.reserve(v.size());
      for (const data::variable& x: v)
      {
        result.push_back(to_bdd(x));
      }
      return result;
    }

    // Generates boolean variables that are used to identify a PBES variable
    std::vector<data::variable> compute_id_variables(std::size_t n, bool unary_encoding)
    {
      std::size_t m = unary_encoding ? n : log2_rounded_up(n);
      std::vector<data::variable> result;
      for (std::size_t i = 0; i < m; i++)
      {
        result.emplace_back("rep" + std::to_string(i), data::sort_bool::bool_());
      }
      return result;
    }

    std::vector<bdd> compute_nodes(std::size_t equation_count, const std::vector<bdd>& id_variables, bool unary_encoding)
    {
      std::vector<bdd> result;
      std::vector<std::vector<bdd>> sequences(equation_count, std::vector<bdd>());

      if (unary_encoding)
      {
        for (std::size_t i = 0; i < equation_count; i++)
        {
          for (std::size_t j = 0; j < equation_count; j++)
          {
            sequences[j].push_back(i == j ? ~id_variables[i] : id_variables[i]);
          }
        }
      }
      else
      {
        std::size_t m = id_variables.size();

        std::size_t repeat = 1;
        for (std::size_t i = 0; i < m; i++)
        {
          for (std::size_t j = 0; j < equation_count; j++)
          {
            bool negate = (j / repeat) % 2 == 0;
            sequences[j].push_back(negate ? ~id_variables[i] : id_variables[i]);
          }
          repeat *= 2;
        }
      }

      result.reserve(equation_count);
      for (std::size_t j = 0; j < equation_count; j++)
      {
        result.push_back(all(sequences[j]));
      }
      return result;
    }

    inline
    std::map<std::size_t, std::vector<bdd>> compute_priority_map(const std::vector<bdd>& equation_ids) const
    {
      std::map<std::size_t, std::vector<bdd>> result;
      const auto& equations = m_pbes.equations();
      for (std::size_t i = 0; i < equations.size(); i++)
      {
        const srf_equation& eqn = equations[i];
        std::size_t rank = m_pbes_index.rank(eqn.variable().name());
        result[rank].push_back(equation_ids[i]);
      }
      return result;
    }

    bdd parameter_updates(const std::vector<bdd>& parameters, const std::vector<bdd>& values) const
    {
      assert(parameters.size() == values.size());
      std::vector<bdd> v;
      for (std::size_t i = 0; i < parameters.size(); i++)
      {
        v.push_back(equiv(parameters[i], values[i]));
      }
      return all(v);
    }

    // The result contains a bdd with an edge relation for each summand
    std::vector<bdd> compute_edge_relation(
        const std::vector<srf_equation>& equations,
        const std::vector<bdd>& equation_ids,
        const std::vector<bdd>& equation_ids_next,
        const std::vector<bdd>& parameters_next
      ) const
    {
      std::vector<bdd> result;

      std::size_t N = equation_ids.size();
      for (std::size_t i = 0; i < N; i++)
      {
        std::vector<bdd> summand_bdds;
        const srf_equation& eqn = equations[i];
        for (const srf_summand& summand: eqn.summands())
        {
          const auto& condition = summand.condition();
          const auto& variable = summand.variable();

          if (!summand.parameters().empty())
          {
            throw mcrl2::runtime_error("quantifiers are not yet supported");
          }
          const bdd& id0 = equation_ids[i];
          std::size_t i1 = m_pbes_index.index(variable.name());
          bdd id1 = equation_ids_next[i1];
          bdd f = to_bdd(condition);
          std::vector<bdd> v = { id0, id1, f };
          if (!parameters_next.empty())
          {
            bdd updates = parameter_updates(parameters_next, to_bdd(summand.variable().parameters()));
            v.push_back(updates);
          }
          summand_bdds.push_back(all(v));
        }
        if (m_granularity == bdd_granularity::per_summand)
        {
          result.insert(result.end(), summand_bdds.begin(), summand_bdds.end());
        }
        else
        {
          result.push_back(any(summand_bdds));
        }
      }
      if (m_granularity == bdd_granularity::per_pbes)
      {
        return { any(result) };
      }
      else
      {
        return result;
      }
    }

    bdd compute_initial_state(const std::vector<bdd>& ids) const
    {
      const propositional_variable_instantiation& init = m_pbes.initial_state();
      const data::data_expression_list& e = init.parameters();
      std::size_t index = m_pbes_index.index(init.name());
      const propositional_variable& X_init = m_pbes.equations()[index].variable();
      const data::variable_list& d = X_init.parameters();

      bdd initvar = ids[index];
      std::vector<bdd> param0 = to_bdd(d);
      std::vector<bdd> param1 = to_bdd(e);

      std::vector<bdd> v;
      v.push_back(initvar);
      for (std::size_t i = 0; i < param0.size(); i++)
      {
        v.push_back(equiv(param0[i], param1[i]));
      }
      return all(v);
    }

    std::vector<bdd> make_bdd_variables(const std::vector<data::variable>& v)
    {
      std::vector<bdd> result;
      result.reserve(v.size());
      for (const data::variable& var: v)
      {
        result.push_back(to_bdd(var));
      }
      return result;
    }

    std::tuple<
        bdd,
        bdd,
        bdd,
        bdd_substitution,
        bdd_substitution,
        bdd,
        std::vector<bdd>,
        bdd,
        bdd,
        bdd,
        std::map<std::uint32_t, bdd>
        >
    compute_parity_game()
    {
      // Attributes of the parity game
      bdd variable_set;
      bdd next_variable_set;
      bdd all_variable_set;
      bdd_substitution substitution;
      bdd_substitution reverse_substitution;
      bdd V;
      std::vector<bdd> E;
      bdd even;
      bdd odd;
      bdd initial_state;
      std::map<std::uint32_t, bdd> priorities;
      const std::vector<srf_equation>& equations = m_pbes.equations();
      std::size_t N = equations.size();

      // boolean variables
      std::vector<data::variable> iparameters = compute_id_variables(m_pbes.equations().size(), m_unary_encoding);
      const data::variable_list& pbes_parameters = m_pbes.equations().front().variable().parameters();
      std::vector<data::variable> parameters(pbes_parameters.begin(), pbes_parameters.end());
      std::vector<data::variable> iparameters_next = add_underscore(iparameters);
      std::vector<data::variable> parameters_next = add_underscore(parameters);

      std::vector<bdd> variables0;
      std::vector<bdd> variables1;

      for (const data::variable& v: iparameters)
      {
        auto [bdd0, index0] = m_variable_manager.add(v.name());
        auto [bdd1, index1] = m_variable_manager.add(std::string(v.name()) + "_");
        variables0.push_back(bdd0);
        variables1.push_back(bdd1);
        substitution.put(index0, bdd1);
        reverse_substitution.put(index1, bdd0);
      }

      for (const data::variable& v: parameters)
      {
        auto [bdd0, index0] = m_variable_manager.add(v.name());
        auto [bdd1, index1] = m_variable_manager.add(std::string(v.name()) + "_");
        variables0.push_back(bdd0);
        variables1.push_back(bdd1);
        substitution.put(index0, bdd1);
        reverse_substitution.put(index1, bdd0);
      }

      variable_set = all(variables0);
      next_variable_set = all(variables1);
      all_variable_set = variable_set & next_variable_set;

      // bdd variables
      std::vector<bdd> iparameters_bdd = make_bdd_variables(iparameters);
      std::vector<bdd> parameters_bdd = make_bdd_variables(parameters);
      std::vector<bdd> iparameters_bdd_next = make_bdd_variables(iparameters_next);
      std::vector<bdd> parameters_bdd_next = make_bdd_variables(parameters_next);

      // each PBES variable Xi(e) is a node of the graph
      std::vector<bdd> nodes = compute_nodes(m_pbes.equations().size(), iparameters_bdd, m_unary_encoding);
      std::vector<bdd> nodes_next = compute_nodes(m_pbes.equations().size(), iparameters_bdd_next, m_unary_encoding);

      // compute the set V of graph nodes
      V = any(nodes);

      // compute the sets of even and odd graph nodes
      std::vector<bdd> even_nodes;
      std::vector<bdd> odd_nodes;
      for (std::size_t i = 0; i < N; i++)
      {
        const srf_equation& eqn = equations[i];
        if (eqn.is_conjunctive())
        {
          odd_nodes.push_back(nodes[i]);
        }
        else
        {
          even_nodes.push_back(nodes[i]);
        }
        even = any(even_nodes);
        odd = any(odd_nodes);
      }

      //  compute the priority map
      std::map<std::size_t, std::vector<bdd>> priority_ids = compute_priority_map(nodes);
      std::size_t min_rank = m_pbes_index.rank(equations.front().variable().name());
      std::size_t max_rank = m_pbes_index.rank(equations.back().variable().name());
      // We prefer a max priority game, so the ranks need to be reversed
      // Start at 0 if max_rank is even
      for (std::size_t rank = min_rank; rank <= max_rank; rank++)
      {
        std::size_t r = max_rank - rank + (max_rank % 2);
        priorities[r] = any(priority_ids[rank]);
      }

      E = compute_edge_relation(equations, nodes, nodes_next, parameters_bdd_next);

      initial_state = compute_initial_state(nodes);

      return { variable_set, next_variable_set, all_variable_set, substitution,
           reverse_substitution, V, E, even, odd, initial_state, priorities };
    }

  public:
    explicit pbesbddsolve(const srf_pbes& p, bool unary_encoding = false,
               bdd_granularity granularity = bdd_granularity::per_pbes,
               utilities::execution_timer* timer = nullptr)
        : m_pbes(p), m_pbes_index(m_pbes), m_unary_encoding(unary_encoding), m_granularity(granularity), m_timer(timer)
    { }

    bool run(bool use_sylvan_optimization = true, bool remove_unreachable_vertices = true)
    {
      mCRL2log(log::verbose) << "Computing parity game" << std::endl;
      start_timer("compute-parity-game");
      auto [variable_set, next_variable_set, all_variable_set, substitution,
            reverse_substitution, V, E, even, odd, initial_state, priorities] = compute_parity_game();
      bdd_parity_game G(variable_set, next_variable_set, all_variable_set, substitution,
                            reverse_substitution, V, E, even, odd, priorities, initial_state, use_sylvan_optimization);
      finish_timer("compute-parity-game");
      mCRL2log(log::debug) << "bdd node count |V| = " << G.nodes().node_count() << std::endl;
      mCRL2log(log::debug) << "bdd node count |E| = " << any(G.edges()).node_count() << std::endl;

      if (remove_unreachable_vertices)
      {
        mCRL2log(log::verbose) << "Computing reachable vertices" << std::endl;
        start_timer("remove-unreachable-vertices");
        G.remove(~G.reachable_vertices(G.initial_state()));
        finish_timer("remove-unreachable-vertices");
      }
      mCRL2log(log::verbose) << "Running Zielonka algorithm" << std::endl;
      start_timer("run-zielonka-algorithm");
      auto [W0, W1] = G.zielonka();
      finish_timer("run-zielonka-algorithm");
      return (W0 | G.initial_state()) == W0;
    }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PBESBDDSOLVE_H
