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
#include "sylvan_obj.hpp"

namespace mcrl2 {

namespace pbes_system {

namespace bdd {

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

class bdd_sylvan
{
  public:
    typedef sylvan::Bdd bdd_type;
    typedef sylvan::BddMap bdd_substitution;
    typedef sylvan::BddSet bdd_variable_set;

  private:
    std::map<std::string, std::size_t> m_variable_index; // maps variable names to variable indices
    std::vector<std::string> m_variable_names; // maps variable indices to variable names
    std::vector<bdd_type> m_variables; // maps variable indices to the corresponding bdd

  public:
    bdd_type true_() const
    {
      return sylvan::sylvan_true;
    }

    bdd_type false_() const
    {
      return sylvan::sylvan_false;
    }

    // returns !x
    bdd_type not_(const bdd_type& x) const
    {
      return !x;
    }

    // returns x || y
    bdd_type or_(const bdd_type& x, const bdd_type& y) const
    {
      return x.Or(y);
    }

    // returns x && y
    bdd_type and_(const bdd_type& x, const bdd_type& y) const
    {
      return x.And(y);
    }

    // returns x <=> y
    bdd_type equiv(const bdd_type& x, const bdd_type& y) const
    {
      return x.Xnor(y);
    }

    // returns x => y
    bdd_type implies(const bdd_type& x, const bdd_type& y) const
    {
      return (!x).Or(y);
    }

    // returns if x then y else z
    bdd_type ite(const bdd_type& x, const bdd_type& y, const bdd_type& z) const
    {
      return x.Ite(y, z);
    }

    bdd_type forall(const bdd_variable_set& variables, const bdd_type& body) const
    {
      return body.UnivAbstract(variables);
    }

    bdd_type exists(const bdd_variable_set& variables, const bdd_type& body) const
    {
      return body.ExistAbstract(variables);
    }

    bdd_type quantify(const bdd_type& body, const bdd_variable_set& variables, bool is_forall) const
    {
      return is_forall ? forall(variables, body) : exists(variables, body);
    }

    bdd_type let(const bdd_substitution& sigma, const bdd_type& x) const
    {
      return x.Compose(sigma);
    }

    // Applies relation R to x
    bdd_type relation_forward(const bdd_type& R, const bdd_type& x, const bdd_variable_set& variables, const bdd_substitution& prev_substitution, bool optimized = false) const
    {
      if (optimized)
      {
        // Note that the parameter prev_substitution is not used. This is because Sylvan makes assumptions about the variable order
        return x.RelNext(R, variables);
      }
      return let(prev_substitution, exists(variables, x & R));
    };

    // Applies the inverse of the relation R to x
    bdd_type relation_backward(const bdd_type& R, const bdd_type& x, const bdd_variable_set& next_variables, const bdd_substitution& next_substitution, bool optimized = false) const
    {
      if (optimized)
      {
        // Note that the parameter next_substitution is not used. This is because Sylvan makes assumptions about the variable order
        return x.RelPrev(R, next_variables);
      }
      return exists(next_variables, let(next_substitution, x) & R);
    };

    // Returns a string representing one solution
    std::string pick_one_solution(const bdd_type& x, const bdd_variable_set& variables) const
    {
      if (x == false_())
      {
        return "no solution";
      }

      std::vector<bool> solution = x.PickOneCube(variables);
      std::vector<std::uint32_t> V = variables.toVector();
      assert(solution.size() == variables.size());

      std::vector<std::string> literals;
      for (std::size_t i = 0; i < solution.size(); ++i)
      {
        std::string literal = solution[i] ? m_variable_names[i] : '~' + m_variable_names[i];
        literals.push_back(literal);
      }
      return utilities::string_join(literals, " | ");
    }

    std::size_t count(const bdd_type& x, const bdd_variable_set & variables) const
    {
      return x.SatCount(variables);
    }

    bdd_type all(const std::vector<bdd_type>& v) const
    {
      if (v.empty())
      {
        return true_();
      }
      bdd_type result = v[0];
      for (auto i = ++v.begin(); i != v.end(); ++i)
      {
        result = and_(result, *i);
      }
      return result;
    }

    bdd_type any(const std::vector<bdd_type>& v) const
    {
      if (v.empty())
      {
        return false_();
      }
      bdd_type result = v[0];
      for (auto i = ++v.begin(); i != v.end(); ++i)
      {
        result = or_(result, *i);
      }
      return result;
    }

    // Add a variable with the given name
    // Returns the corresponding bdd and variable index
    std::pair<bdd_type, std::size_t> add_variable(const std::string& name)
    {
      std::size_t index = m_variable_index.size();
      bdd_type result = bdd_type::bddVar(index);
      m_variable_index[name] = index;
      m_variables.push_back(result);
      return { result, index };
    }

    const bdd_type& variable_bdd(const std::string& name) const
    {
      if (m_variable_index.find(name) == m_variable_index.end())
      {
        throw std::runtime_error("could not find variable " + name);
      }
      return m_variables[m_variable_index.at(name)];
    }

    std::size_t variable_index(const std::string& name) const
    {
      return m_variable_index.at(name);
    }

    void print(const std::string& msg, const bdd_type& x, const bdd_variable_set& context) const
    {
      std::cout << msg << " " << count(x, context) << std::endl;
    }

    void print(const std::string& msg, const std::vector<bdd_type>& x, const bdd_variable_set& context) const
    {
      std::cout << msg;
      for (const auto& xi: x)
      {
        std::cout << " " << count(xi, context);
      }
      std::cout << std::endl;
    }

//    void print(const std::string& msg, const bdd_variable_set& variables) const
//    {
//      std::cout << msg;
//      for (const std::size_t i: variables.as_vector())
//      {
//        std::cout << " " << i;
//      }
//      std::cout << std::endl;
//    }

    void print(const std::string& msg, const std::map<std::uint32_t, bdd_sylvan::bdd_type>& m, const bdd_variable_set& context) const
    {
      std::cout << msg;
      for (const auto& [i, x]: m)
      {
        std::cout << " (" << i << ", " << count(x, context) << ")";
      }
      std::cout << std::endl;
    }

    std::string hash(const bdd_type& x) const
    {
      char hash[65];
      x.GetShaHash(hash);
      return std::string(hash);
    }

    std::size_t node_count(const bdd_type& x) const
    {
      return x.NodeCount();
    }

    std::size_t serialize(const bdd_type& x) const
    {
      mCRL2log(log::verbose) << "serialize " << node_count(x) << " " << hash(x) << std::endl;
      return sylvan::sylvan_serialize_add(x.GetBDD());
    }

    bdd_type deserialize(std::size_t index) const
    {
      bdd_type result = sylvan::sylvan_serialize_get_reversed(index);
      mCRL2log(log::verbose) << "deserialize " << node_count(result) << " " << hash(result) << std::endl;
      return result;
    }
};

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
    using bdd_type = bdd_sylvan::bdd_type;
    using bdd_substitution = bdd_sylvan::bdd_substitution;
    using bdd_variable_set = bdd_sylvan::bdd_variable_set;

    static constexpr bool even = false;
    static constexpr bool odd = true;

  private:
    bdd_sylvan& m_bdd;
    const bdd_variable_set& m_variables;
    const bdd_variable_set& m_next_variables;
    const bdd_variable_set& m_all_variables;
    const bdd_substitution& m_next_substitution;
    const bdd_substitution& m_prev_substitution;
    bdd_type m_V;
    std::vector<bdd_type> m_E;
    bdd_type m_even;
    bdd_type m_odd;
    std::map<std::uint32_t, bdd_type> m_priorities;
    bdd_type m_initial_state;
    bool m_use_sylvan_optimization;

    void info(const bdd_type& x, const std::string& msg, const bdd_variable_set& variables) const
    {
      std::cout << msg << " " << m_bdd.count(x, variables) << std::endl;
    }

  public:
    void print() const
    {
      info(m_V, "V", m_variables);
      info(m_bdd.any(m_E), "E", m_all_variables);
      info(m_even, "even", m_variables);
      info(m_odd, "odd", m_variables);
      info(m_initial_state, "initial state", m_variables);
      for (const auto& [prio, x]: m_priorities)
      {
        info(x, "priority " + std::to_string(prio), m_variables);
      }
    }

    bdd_parity_game(
      bdd_sylvan& bdd,
      const bdd_variable_set& variables,
      const bdd_variable_set& next_variables,
      const bdd_variable_set& all_variables,
      const bdd_substitution& next_substitution,
      const bdd_substitution& prev_substitution,
      const bdd_type& V,
      const std::vector<bdd_type>& E,
      const bdd_type& even_nodes,
      const bdd_type& odd_nodes,
      const std::map<std::uint32_t, bdd_type>& priorities,
      const bdd_type& initial_state,
      bool use_sylvan_optimization = false
     )
    : m_bdd(bdd),
      m_variables(variables),
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
    bdd_type predecessor(bool player, const bdd_type& U, bool optimized = false)
    {
      bdd_type V_player = (player == even) ? m_even : m_odd;
      bdd_type V_opponent = (player == odd) ? m_even : m_odd;

      std::vector<bdd_type> W_player;
      for (const auto& Ei: m_E)
      {
        W_player.push_back(m_bdd.relation_backward(
            Ei, U, m_next_variables, m_next_substitution, optimized));
      }
      bdd_type U_player = V_player & m_bdd.any(W_player);

      std::vector<bdd_type> W_opponent;
      for (const auto& Ei: m_E)
      {
        W_opponent.push_back(V_opponent & ~m_bdd.relation_backward(
                                              Ei, m_V & ~U, m_next_variables,
                                              m_next_substitution, optimized));
      }
      bdd_type U_opponent = m_bdd.all(W_opponent);

      return U_player | U_opponent;
    }

    // U is a BDD representing a set of vertices
    // player is either string 'even' or string 'odd'
    // attractor computation is a least fixpoint computation
    bdd_type attractor(bool player, const bdd_type& A)
    {
      mCRL2log(log::debug) << "attractor" << std::endl;
      std::size_t count = 0;

      bdd_type tmp = m_bdd.false_();
      bdd_type tmp_ = A;
      while (tmp != tmp_)
      {
        mCRL2log(log::debug) << count++ << std::endl;
        tmp = tmp_;
        tmp_ = tmp_ | predecessor(player, tmp_, m_use_sylvan_optimization);
      }
      return tmp;
    }

    // removing a set of vertices represented by BDD A
    void remove(const bdd_type& A)
    {
      m_V = m_V & ~A;
      m_even = m_even & ~A;
      m_odd = m_odd & ~A;
      bdd_type A_next = m_bdd.let(m_next_substitution, A);

      //----------------------------------------------//
      // bdd_type AA = ~A & ~A_; // N.B. this turns out to be very inefficient in some cases
      // for (auto& Ei: m_E)
      // {
      //   Ei = Ei & AA;
      // }
      //----------------------------------------------//

      for (auto& Ei: m_E)
      {
        Ei = Ei & ~A & ~A_next;
      }

      std::map<std::uint32_t, bdd_type> priorities;
      for (const auto& [i, p_i]: m_priorities)
      {
        auto priority = p_i & ~A;
        if (priority != m_bdd.false_())
        {
          priorities[i] = priority;
        }
      }
      m_priorities = priorities;
    }

    std::size_t game_size() const
    {
      return m_bdd.count(m_V, m_variables);
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

    const bdd_type& initial_state() const
    {
      return m_initial_state;
    }

    const bdd_variable_set& all_variables() const
    {
      return m_all_variables;
    }

    const bdd_variable_set& next_variables() const
    {
      return m_next_variables;
    }

    const bdd_variable_set& variables() const
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

    const bdd_type& nodes() const
    {
      return m_V;
    }

    const std::vector<bdd_type>& edges() const
    {
      return m_E;
    }

    const bdd_type& even_nodes() const
    {
      return m_even;
    }

    const bdd_type& odd_nodes() const
    {
      return m_odd;
    }

    const std::map<std::uint32_t, bdd_type>& priorities() const
    {
      return m_priorities;
    }

    // Perform reachability, return reachable states
    bdd_type reachable_vertices_default(const bdd_type& U)
    {
      bdd_type V_reachable = m_bdd.false_();
      bdd_type V_reachable_ = U;
      std::size_t count = 0;
      while (V_reachable != V_reachable_)
      {
        V_reachable = V_reachable_;
        mCRL2log(log::verbose) << "  reachable_states_default iteration " << count++ << std::endl;
        for (const auto& Ei: m_E)
        {
          V_reachable_ = V_reachable_ | m_bdd.relation_forward(Ei, V_reachable_, m_variables, m_prev_substitution);
        }
      }
      return V_reachable;
    }

    // Perform reachability, return reachable states
    // exploit the partitioning to potentially more quickly explore the state space
    // Not reliably quicker (or slower)
    bdd_type reachable_vertices(const bdd_type& U)
    {
      bdd_type V_reachable = m_bdd.false_();
      bdd_type V_frontier = U;
      std::vector<bdd_type> V_next;
      std::size_t count = 0;
      while ( (V_reachable | V_frontier) != V_reachable) // check whether V_frontier is a subset of V_reachable
      {
        V_reachable = V_reachable | V_frontier;
        mCRL2log(log::debug) << "  reachable_states iteration " << count++ << std::endl;
        V_next.clear();
        for (const auto& Ei: m_E)
        {
          V_next.push_back(m_bdd.relation_forward(Ei, V_frontier, m_variables, m_prev_substitution));
        }
        V_frontier = m_bdd.any(V_next);
      }
      return V_reachable;
    }

    std::pair<bdd_type, bdd_type> zielonka()
    {
      mCRL2log(log::debug) << "start zielonka" << std::endl;
//      mCRL2log(log::debug) << "node_count(V) = " << m_bdd.node_count(nodes()) << " sat_count(V) = " << m_bdd.count(nodes(), m_variables) << std::endl;
//      mCRL2log(log::debug) << "node_count(E) = " << m_bdd.node_count(m_bdd.any(edges())) << " sat_count(E) = " << m_bdd.count(m_bdd.any(edges()), m_all_variables) << std::endl;
//      mCRL2log(log::debug) << "node_count(even) = " << m_bdd.node_count(m_even) << " sat_count(even) = " << m_bdd.count(m_even, m_all_variables) << std::endl;
//      mCRL2log(log::debug) << "node_count(odd) = " << m_bdd.node_count(m_odd) << " sat_count(odd) = " << m_bdd.count(m_odd, m_all_variables) << std::endl;

      if ( (m_even | m_odd) == m_bdd.false_())
      {
        mCRL2log(log::debug) << "finish zielonka" << std::endl;
        return { m_even, m_odd };
      }
      else
      {
        auto p = maximum();
        bool parity = (p % 2 == 0) ? even : odd;
        bdd_type U = m_priorities[p];
        bdd_type A = attractor(parity, U);
        bdd_parity_game G = *this;
        G.remove(A);
        auto [W0, W1] = G.zielonka();
        if (parity == even && W1 == m_bdd.false_())
        {
          mCRL2log(log::debug) << "finish zielonka" << std::endl;
          return { W0 | A, W1 };
        }
        else if (parity == odd && W0 == m_bdd.false_())
        {
          mCRL2log(log::debug) << "finish zielonka" << std::endl;
          return { W0, A | W1 };
        }
        else
        {
          auto [W, opponent] = (parity == even) ? std::make_pair(W1, odd) : std::make_pair(W0, even);
          bdd_type B = attractor(opponent, W);
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
  public:
    using bdd_type = bdd_sylvan::bdd_type;
    using bdd_substitution = bdd_sylvan::bdd_substitution;
    using bdd_variable_set = bdd_sylvan::bdd_variable_set;

  private:
    const srf_pbes& m_pbes;
    pbes_equation_index m_pbes_index;

    bdd_sylvan m_bdd;
    bool m_unary_encoding;
    bdd_granularity m_granularity = bdd_granularity::per_pbes;
    utilities::execution_timer* m_timer = nullptr;     // if it is non-zero, it will be used to display timing information

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

    bdd_type to_bdd(const data::variable& x) const
    {
      return m_bdd.variable_bdd(x.name());
    }

    bdd_type to_bdd(const data::data_expression& x) const
    {
      if (data::is_variable(x) && data::is_bool(atermpp::down_cast<data::variable>(x).sort()))
      {
        const auto& x_ = atermpp::down_cast<data::variable>(x);
        return to_bdd(x_);
      }
      else if (data::is_true(x))
      {
        return sylvan::sylvan_true;
      }
      else if (data::is_false(x))
      {
        return sylvan::sylvan_false;
      }
      else if (data::is_not(x))
      {
        const auto& operand = data::sort_bool::arg(x);
        return m_bdd.not_(to_bdd(operand));
      }
      else if (data::is_equal_to(x))
      {
        const auto& left  = data::binary_left1(x);
        const auto& right = data::binary_right1(x);
        return m_bdd.equiv(to_bdd(left), to_bdd(right));
      }
      else if (data::is_not_equal_to(x))
      {
        const auto& left  = data::binary_left1(x);
        const auto& right = data::binary_right1(x);
        return m_bdd.implies(to_bdd(left), !(to_bdd(right)));
      }
      else if (data::is_or(x))
      {
        const auto& left  = data::binary_left1(x);
        const auto& right = data::binary_right1(x);
        return m_bdd.or_(to_bdd(left), to_bdd(right));
      }
      else if (data::is_and(x))
      {
        const auto& left  = data::binary_left1(x);
        const auto& right = data::binary_right1(x);
        return m_bdd.and_(to_bdd(left), to_bdd(right));
      }
      else if (data::is_imp(x))
      {
        const auto& left  = data::binary_left1(x);
        const auto& right = data::binary_right1(x);
        return m_bdd.implies(to_bdd(left), to_bdd(right));
      }
      else if (is_if_application(x))
      {
        const auto& x_ = atermpp::down_cast<data::application>(x);
        const data::data_expression& condition = x_[0];
        const data::data_expression& then_ = x_[1];
        const data::data_expression& else_ = x_[2];
        return m_bdd.ite(to_bdd(condition), to_bdd(then_), to_bdd(else_));
      }
      throw mcrl2::runtime_error("Unsupported data expression " + data::pp(x) + " encountered in to_bdd.");
    }

    std::vector<bdd_type> to_bdd(const data::data_expression_list& v) const
    {
      std::vector<bdd_type> result;
      for (const data::data_expression& x: v)
      {
        result.push_back(to_bdd(x));
      }
      return result;
    }

    std::vector<bdd_type> to_bdd(const data::variable_list & v) const
    {
      std::vector<bdd_type> result;
      result.reserve(v.size());
      for (const data::variable& x: v)
      {
        result.push_back(to_bdd(x));
      }
      return result;
    }

    std::vector<bdd_type> to_bdd(const std::vector<data::variable>& v) const
    {
      std::vector<bdd_type> result;
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

    std::vector<bdd_type> compute_nodes(std::size_t equation_count, const std::vector<bdd_type>& id_variables, bool unary_encoding)
    {
      std::vector<bdd_type> result;
      std::vector<std::vector<bdd_type>> sequences(equation_count, std::vector<bdd_type>());

      if (unary_encoding)
      {
        for (std::size_t i = 0; i < equation_count; i++)
        {
          for (std::size_t j = 0; j < equation_count; j++)
          {
            sequences[j].push_back(i == j ? m_bdd.not_(id_variables[i]) : id_variables[i]);
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
            sequences[j].push_back(negate ? m_bdd.not_(id_variables[i]) : id_variables[i]);
          }
          repeat *= 2;
        }
      }

      result.reserve(equation_count);
      for (std::size_t j = 0; j < equation_count; j++)
      {
        result.push_back(m_bdd.all(sequences[j]));
      }
      return result;
    }

    inline
    std::map<std::size_t, std::vector<bdd_type>> compute_priority_map(const std::vector<bdd_type>& equation_ids) const
    {
      std::map<std::size_t, std::vector<bdd_type>> result;
      const auto& equations = m_pbes.equations();
      for (std::size_t i = 0; i < equations.size(); i++)
      {
        const srf_equation& eqn = equations[i];
        std::size_t rank = m_pbes_index.rank(eqn.variable().name());
        result[rank].push_back(equation_ids[i]);
      }
      return result;
    }

    bdd_type parameter_updates(const std::vector<bdd_type>& parameters, const std::vector<bdd_type>& values) const
    {
      assert(parameters.size() == values.size());
      std::vector<bdd_type> v;
      for (std::size_t i = 0; i < parameters.size(); i++)
      {
        v.push_back(m_bdd.equiv(parameters[i], values[i]));
      }
      return m_bdd.all(v);
    }

    // The result contains a bdd with an edge relation for each summand
    std::vector<bdd_type> compute_edge_relation(
        const std::vector<srf_equation>& equations,
        const std::vector<bdd_type>& equation_ids,
        const std::vector<bdd_type>& equation_ids_next,
        const std::vector<bdd_type>& parameters_next
      ) const
    {
      std::vector<bdd_type> result;

      std::size_t N = equation_ids.size();
      for (std::size_t i = 0; i < N; i++)
      {
        std::vector<bdd_type> summand_bdds;
        const srf_equation& eqn = equations[i];
        for (const srf_summand& summand: eqn.summands())
        {
          const auto& condition = summand.condition();
          const auto& variable = summand.variable();

          if (!summand.parameters().empty())
          {
            throw mcrl2::runtime_error("quantifiers are not yet supported");
          }
          const bdd_type& id0 = equation_ids[i];
          std::size_t i1 = m_pbes_index.index(variable.name());
          bdd_type id1 = equation_ids_next[i1];
          bdd_type f = to_bdd(condition);
          std::vector<bdd_type> v = { id0, id1, f };
          if (!parameters_next.empty())
          {
            bdd_type updates = parameter_updates(parameters_next, to_bdd(summand.variable().parameters()));
            v.push_back(updates);
          }
          summand_bdds.push_back(m_bdd.all(v));
        }
        if (m_granularity == bdd_granularity::per_summand)
        {
          result.insert(result.end(), summand_bdds.begin(), summand_bdds.end());
        }
        else
        {
          result.push_back(m_bdd.any(summand_bdds));
        }
      }
      if (m_granularity == bdd_granularity::per_pbes)
      {
        return { m_bdd.any(result) };
      }
      else
      {
        return result;
      }
    }

    bdd_type compute_initial_state(const std::vector<bdd_type>& ids) const
    {
      const propositional_variable_instantiation& init = m_pbes.initial_state();
      const data::data_expression_list& e = init.parameters();
      std::size_t index = m_pbes_index.index(init.name());
      const propositional_variable& X_init = m_pbes.equations()[index].variable();
      const data::variable_list& d = X_init.parameters();

      bdd_type initvar = ids[index];
      std::vector<bdd_type> param0 = to_bdd(d);
      std::vector<bdd_type> param1 = to_bdd(e);

      std::vector<bdd_type> v;
      v.push_back(initvar);
      for (std::size_t i = 0; i < param0.size(); i++)
      {
        v.push_back(m_bdd.equiv(param0[i], param1[i]));
      }
      return m_bdd.all(v);
    }

    std::vector<bdd_type> make_bdd_variables(const std::vector<data::variable>& v)
    {
      std::vector<bdd_type> result;
      result.reserve(v.size());
      for (const data::variable& var: v)
      {
        result.push_back(to_bdd(var));
      }
      return result;
    }

    std::tuple<
        bdd_variable_set,
        bdd_variable_set,
        bdd_variable_set,
        bdd_substitution,
        bdd_substitution,
        bdd_type,
        std::vector<bdd_type>,
        bdd_type,
        bdd_type,
        bdd_type,
        std::map<std::uint32_t, bdd_type>
        >
    compute_parity_game()
    {
      // Attributes of the parity game
      bdd_variable_set variable_set;
      bdd_variable_set next_variable_set;
      bdd_variable_set all_variable_set;
      bdd_substitution substitution;
      bdd_substitution reverse_substitution;
      bdd_type V;
      std::vector<bdd_type> E;
      bdd_type even;
      bdd_type odd;
      bdd_type initial_state;
      std::map<std::uint32_t, bdd_type> priorities;
      const std::vector<srf_equation>& equations = m_pbes.equations();
      std::size_t N = equations.size();

      // boolean variables
      std::vector<data::variable> iparameters = compute_id_variables(m_pbes.equations().size(), m_unary_encoding);
      const data::variable_list& pbes_parameters = m_pbes.equations().front().variable().parameters();
      std::vector<data::variable> parameters(pbes_parameters.begin(), pbes_parameters.end());
      std::vector<data::variable> iparameters_next = add_underscore(iparameters);
      std::vector<data::variable> parameters_next = add_underscore(parameters);

      std::vector<bdd_type> id_variables;
      for (const data::variable& v: iparameters)
      {
        auto [bdd0, index0] = m_bdd.add_variable(v.name());
        auto [bdd1, index1] = m_bdd.add_variable(std::string(v.name()) + "_");
        variable_set.add(index0);
        next_variable_set.add(index1);
        id_variables.push_back(bdd0);
        all_variable_set.add(index0);
        all_variable_set.add(index1);
        substitution.put(index0, bdd1);
        reverse_substitution.put(index1, bdd0);
      }
      for (const data::variable& v: parameters)
      {
        auto [bdd0, index0] = m_bdd.add_variable(v.name());
        auto [bdd1, index1] = m_bdd.add_variable(std::string(v.name()) + "_");
        variable_set.add(index0);
        next_variable_set.add(index1);
        all_variable_set.add(index0);
        all_variable_set.add(index1);
        substitution.put(index0, bdd1);
        reverse_substitution.put(index1, bdd0);
      }

      // bdd variables
      std::vector<bdd_type> iparameters_bdd = make_bdd_variables(iparameters);
      std::vector<bdd_type> parameters_bdd = make_bdd_variables(parameters);
      std::vector<bdd_type> iparameters_bdd_next = make_bdd_variables(iparameters_next);
      std::vector<bdd_type> parameters_bdd_next = make_bdd_variables(parameters_next);

      // each PBES variable Xi(e) is a node of the graph
      std::vector<bdd_type> nodes = compute_nodes(m_pbes.equations().size(), iparameters_bdd, m_unary_encoding);
      std::vector<bdd_type> nodes_next = compute_nodes(m_pbes.equations().size(), iparameters_bdd_next, m_unary_encoding);

      // compute the set V of graph nodes
      V = m_bdd.any(nodes);

      // compute the sets of even and odd graph nodes
      std::vector<bdd_type> even_nodes;
      std::vector<bdd_type> odd_nodes;
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
        even = m_bdd.any(even_nodes);
        odd = m_bdd.any(odd_nodes);
      }

      //  compute the priority map
      std::map<std::size_t, std::vector<bdd_type>> priority_ids = compute_priority_map(nodes);
      std::size_t min_rank = m_pbes_index.rank(equations.front().variable().name());
      std::size_t max_rank = m_pbes_index.rank(equations.back().variable().name());
      // We prefer a max priority game, so the ranks need to be reversed
      // Start at 0 if max_rank is even
      for (std::size_t rank = min_rank; rank <= max_rank; rank++)
      {
        std::size_t r = max_rank - rank + (max_rank % 2);
        priorities[r] = m_bdd.any(priority_ids[rank]);
      }

      E = compute_edge_relation(equations, nodes, nodes_next, parameters_bdd_next);

      initial_state = compute_initial_state(nodes);

      return { variable_set, next_variable_set, all_variable_set, substitution,
           reverse_substitution, V, E, even, odd, initial_state, priorities };
    }

  public:
    pbesbddsolve(const srf_pbes& p, bdd_sylvan& bdd, bool unary_encoding = false,
               bdd::bdd_granularity granularity = bdd::bdd_granularity::per_pbes,
               utilities::execution_timer* timer = nullptr)
        : m_pbes(p), m_pbes_index(m_pbes), m_bdd(bdd), m_unary_encoding(unary_encoding), m_granularity(granularity), m_timer(timer)
    { }

    bool run(bool use_sylvan_optimization = true, bool remove_unreachable_vertices = true)
    {
      mCRL2log(log::verbose) << "Computing parity game" << std::endl;
      start_timer("compute-parity-game");
      auto [variable_set, next_variable_set, all_variable_set, substitution,
            reverse_substitution, V, E, even, odd, initial_state, priorities] = compute_parity_game();
      bdd_parity_game G(m_bdd, variable_set, next_variable_set, all_variable_set, substitution,
                            reverse_substitution, V, E, even, odd, priorities, initial_state, use_sylvan_optimization);
      finish_timer("compute-parity-game");
      mCRL2log(log::debug) << "bdd node count |V| = " << m_bdd.node_count(G.nodes()) << std::endl;
      mCRL2log(log::debug) << "bdd node count |E| = " << m_bdd.node_count(m_bdd.any(G.edges())) << std::endl;

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

} // namespace bdd

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PBESBDDSOLVE_H
