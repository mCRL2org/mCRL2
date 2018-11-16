// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/pbesinst_structure_graph2.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_PBESINST_STRUCTURE_GRAPH2_H
#define MCRL2_PBES_PBESINST_STRUCTURE_GRAPH2_H

#include "mcrl2/data/undefined.h"
#include "mcrl2/pbes/pbesinst_fatal_attractors.h"
#include "mcrl2/pbes/pbesinst_find_loops.h"
#include "mcrl2/pbes/replace.h"
#include "mcrl2/pbes/simple_structure_graph.h"
#include "mcrl2/pbes/pbesinst_structure_graph.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

class computation_guard
{
  protected:
     std::size_t m_count = 64;

  public:
    explicit computation_guard(std::size_t initial_count = 64)
      : m_count(initial_count)
    {}

    bool operator()(std::size_t count)
    {
      bool result = count >= m_count;
      while (m_count <= count)
      {
        m_count *= 2;
      }
      return result;
    }
};

class periodic_guard
{
  protected:
    std::size_t count = 0;
    std::size_t regeneration_period = 100;

  public:
    bool operator()(std::size_t period)
    {
      if (++count == regeneration_period)
      {
        count = 0;
        regeneration_period = period;
        return true;
      }
      return false;
    }
};

inline
void log_vertex_set(const vertex_set& V, const std::string& name)
{
  std::vector<structure_graph::index_type> vertices = V.vertices();
  std::sort(vertices.begin(), vertices.end());
  mCRL2log(log::debug) << name << " = " << core::detail::print_set(vertices) << std::endl;
}

} // namespace detail

/// \brief Adds an optimization to pbesinst_structure_graph.
class pbesinst_structure_graph_algorithm2: public pbesinst_structure_graph_algorithm
{
  protected:
    vertex_set S0;
    vertex_set S1;
    pbes_expression b;
    detail::computation_guard S0_guard;
    detail::computation_guard S1_guard;
    detail::computation_guard find_loops_guard;
    detail::computation_guard fatal_attractors_guard;
    detail::periodic_guard reset_guard;

    template<typename T>
    pbes_expression expr(const T& x) const
    {
      return static_cast<const pbes_expression&>(x);
    }

    std::pair<pbes_expression, pbes_expression> Rplus(const pbes_expression& x)
    {
      if (is_true(x) || is_false(x))
      {
        return { x, x };
      }
      else if (is_propositional_variable_instantiation(x))
      {
        auto u = m_graph_builder.find_vertex(x);
        if (u != structure_graph::undefined_vertex)
        {
          if (S0.contains(u))
          {
            return { expr(true_()), x };
          }
          else if (S1.contains(u))
          {
            return { expr(false_()), x };
          }
        }
        return { expr(data::undefined_data_expression()), x };
      }
      else if (is_and(x))
      {
        const auto& x_ = atermpp::down_cast<and_>(x);
        // TODO: use structured bindings
        pbes_expression b1, b2, x1, x2;
        std::tie(b1, x1) = Rplus(x_.left());
        std::tie(b2, x2) = Rplus(x_.right());
        if (is_true(b1) && is_true(b2))
        {
          return { expr(true_()), x };
        }
        else if (is_false(b1) && !is_false(b2))
        {
          return { expr(false_()), x1 };
        }
        else if (!is_false(b1) && is_false(b2))
        {
          return { expr(false_()), x2 };
        }
        else if (is_false(b1) && is_false(b2))
        {
          // TODO: use a heuristic for the smallest term (x1 or x2)
          return { expr(false_()), x1 };
        }
        else // if (b1 == data::undefined_data_expression() && b2 == data::undefined_data_expression())
        {
          return { expr(data::undefined_data_expression()), x };
        }
      }
      else if (is_or(x))
      {
        const auto& x_ = atermpp::down_cast<or_>(x);
        // TODO: use structured bindings
        pbes_expression b1, b2, x1, x2;
        std::tie(b1, x1) = Rplus(x_.left());
        std::tie(b2, x2) = Rplus(x_.right());
        if (is_false(b1) && is_false(b2))
        {
          return { expr(false_()), x };
        }
        else if (is_true(b1) && !is_true(b2))
        {
          return { expr(true_()), x1 };
        }
        else if (!is_true(b1) && is_true(b2))
        {
          return { expr(true_()), x2 };
        }
        else if (is_true(b1) && is_true(b2))
        {
          // TODO: use a heuristic for the smallest term (x1 or x2)
          return { expr(true_()), x1 };
        }
        else // if (b1 == data::undefined_data_expression() && b2 == data::undefined_data_expression())
        {
          return { expr(data::undefined_data_expression()), x };
        }
      }
      else
      {
        throw mcrl2::runtime_error("Unexpected term " + pbes_system::pp(x) + " encountered in Rplus");
      }
    }

    void compute_attractor_set_S0(const simple_structure_graph& G)
    {
      S0 = compute_attractor_set(G, S0, 0);
    }

    void compute_attractor_set_S1(const simple_structure_graph& G)
    {
      S1 = compute_attractor_set(G, S1, 1);
    }

    void find_loops(const simple_structure_graph& G)
    {
      std::unordered_map<structure_graph::index_type, bool> visited;
      for (const propositional_variable_instantiation& X: discovered)
      {
        structure_graph::index_type u = m_graph_builder.find_vertex(X);
        const auto& u_ = G.find_vertex(u);
        if (u_.rank == data::undefined_index())
        {
          continue;
        }
        auto i = visited.find(u);
        if (i != visited.end())
        {
          visited[u] = false;
        }
        bool b = detail::find_loop(G, u, u, u_.rank, visited);
        visited[u] = b;
        if (b)
        {
          if (u_.rank % 2 == 1)
          {
            S1.insert(u);
          }
          else
          {
            S0.insert(u);
          }
        }
      }
    }

    void insert(std::map<std::size_t, vertex_set>& U_rank_map, structure_graph::index_type u, std::size_t j, std::size_t n)
    {
      auto i = U_rank_map.find(j);
      if (i == U_rank_map.end())
      {
        i = U_rank_map.insert({ j, vertex_set(n) }).first;
      }
      i->second.insert(u);
    }

    void fatal_attractors(const simple_structure_graph& G)
    {
      mCRL2log(log::debug) << "Apply fatal attractors to graph:\n" << G << std::endl;

      // count the number of insertions in the sets S0 and S1
      std::size_t insertion_count = 0;

      std::size_t n = m_graph_builder.m_vertices.size();

      // compute todo_
      boost::dynamic_bitset<> todo_(n);
      for (const propositional_variable_instantiation& X: todo)
      {
        structure_graph::index_type u = m_graph_builder.find_vertex(X);
        todo_[u] = true;
      }

      // compute done and discovered_
      vertex_set done(n);
      vertex_set discovered_(n);
      for (const propositional_variable_instantiation& X: discovered)
      {
        structure_graph::index_type u = m_graph_builder.find_vertex(X);
        discovered_.insert(u);
        if (!todo_[u])
        {
          done.insert(u);
        }
      }

      // compute U_j_map, such that U_j_map[j] = U_j
      std::map<std::size_t, vertex_set> U_j_map;
      for (structure_graph::index_type u: done.vertices())
      {
        std::size_t j = G.rank(u);
        auto alpha = j % 2;
        vertex_set& S_alpha = alpha == 0 ? S0 : S1;
        if ((alpha == 0 && G.decoration(u) == structure_graph::decoration_type::d_false) || (alpha == 1 && G.decoration(u) == structure_graph::decoration_type::d_true))
        {
          continue;
        }
        if (!S_alpha.contains(u))
        {
          insert(U_j_map, u, j, n);
        }
      }

      detail::log_vertex_set(done, "done");
      detail::log_vertex_set(discovered_, "discovered");
      detail::log_vertex_set(S0, "S0");
      detail::log_vertex_set(S1, "S1");

      for (auto& p: U_j_map)
      {
        std::size_t j = p.first;
        const vertex_set& U_j = p.second;
        detail::log_vertex_set(U_j, "U_" + std::to_string(j));
        auto alpha = j % 2;
        vertex_set& S_alpha = alpha == 0 ? S0 : S1;
        vertex_set X = detail::compute_attractor_set_min_rank(G, U_j, alpha, done, j);

        // compute discovered \ (X \cup S_alpha)
        vertex_set discovered_minus_X_S_alpha(n);
        for (structure_graph::index_type u: discovered_.vertices())
        {
          if (!X.contains(u) && !S_alpha.contains(u))
          {
            discovered_minus_X_S_alpha.insert(u);
          }
        }
        detail::log_vertex_set(discovered_minus_X_S_alpha, "discovered \\ X");

        // compute Y
        vertex_set Y(n);
        discovered_minus_X_S_alpha = compute_attractor_set(G, discovered_minus_X_S_alpha, 1 - alpha);
        for (structure_graph::index_type u: U_j.vertices())
        {
          if (!discovered_minus_X_S_alpha.contains(u))
          {
            Y.insert(u);
          }
        }

        detail::log_vertex_set(Y, "Y");

        if (!Y.is_empty())
        {
          Y = detail::compute_attractor_set_min_rank(G, Y, alpha, done, j);
          detail::log_vertex_set(Y, "AttrMinRank(Y)");
          for (structure_graph::index_type y: Y.vertices())
          {
            insertion_count++;
            S_alpha.insert(y);
            mCRL2log(log::debug) << "Fatal attractors: insert vertex " << y << " in S" << alpha << std::endl;
          }
        }
      }
      mCRL2log(log::debug) << "Fatal attractors: inserted " << insertion_count << " vertices." << std::endl;
    }

    bool solution_found(const propositional_variable_instantiation& init) const override
    {
      auto u = m_graph_builder.find_vertex(init);
      return S0.contains(u) || S1.contains(u);
    }

    bool successors_disjoint(const simple_structure_graph& G, const structure_graph::index_type u, const vertex_set& S) const
    {
      for (auto v: G.successors(u))
      {
        if (S.contains(v))
        {
          return false;
        }
      }
      return true;
    }

    // returns true if u has a successor that is not contained in the union of S0, S1 and done
    bool has_successor_not_in(const simple_structure_graph& G,
                              const structure_graph::index_type u,
                              const vertex_set& S0,
                              const vertex_set& S1,
                              const std::unordered_set<pbes_expression>& done
                             ) const
    {
      using utilities::detail::contains;

      for (auto v: G.successors(u))
      {
        if (S0.contains(v))
        {
          return false;
        }
        if (S1.contains(v))
        {
          return false;
        }
        const auto& v_ = m_graph_builder.m_vertices[v];
        if (contains(done, v_.formula))
        {
          return false;
        }
      }
      return true;
    }

    void reset(const propositional_variable_instantiation& init,
               std::deque<propositional_variable_instantiation>& todo,
               std::size_t regeneration_period
              ) override
    {
      using utilities::detail::contains;

      if (!reset_guard(regeneration_period))
      {
        return;
      }

      simple_structure_graph G(m_graph_builder.m_vertices);
      std::unordered_set<pbes_expression> todo1{init};
      std::unordered_set<pbes_expression> done1;

      while (!todo1.empty())
      {
        auto X = *todo1.begin();
        todo1.erase(todo1.begin());
        done1.insert(X);
        auto u = m_graph_builder.find_vertex(X);
        const auto& u_ = m_graph_builder.m_vertices[u];

        if (u_.decoration == structure_graph::d_conjunction && successors_disjoint(G, u, S1))
        {
          for (auto v: G.successors(u))
          {
            if (S0.contains(v))
            {
              continue;
            }
            const auto& v_ = m_graph_builder.m_vertices[v];
            const auto& Y = v_.formula;
            if (contains(done1, Y))
            {
              continue;
            }
            todo1.insert(Y);
          }
        }

        if (u_.decoration == structure_graph::d_disjunction && successors_disjoint(G, u, S0))
        {
          for (auto v: G.successors(u))
          {
            if (S1.contains(v))
            {
              continue;
            }
            const auto& v_ = m_graph_builder.m_vertices[v];
            const auto& Y = v_.formula;
            if (contains(done1, Y))
            {
              continue;
            }
            todo1.insert(Y);
          }
        }

        if (u_.decoration == structure_graph::d_none && has_successor_not_in(G, u, S0, S1, done1))
        {
          for (auto v: G.successors(u))
          {
            const auto& v_ = m_graph_builder.m_vertices[v];
            const auto& Y = v_.formula;
            todo1.insert(Y);
          }
        }
      }

      std::deque<propositional_variable_instantiation> new_todo;
      for (const propositional_variable_instantiation& X: todo)
      {
        if (contains(done1, X))
        {
          new_todo.push_back(X);
        }
      }
      std::swap(todo, new_todo);
    };

  public:
    typedef pbesinst_structure_graph_algorithm super;

    pbesinst_structure_graph_algorithm2(
        const pbes& p,
        structure_graph& G,
        data::rewriter::strategy rewrite_strategy = data::jitty,
        search_strategy search_strategy = breadth_first,
        int optimization = 0
    )
      : pbesinst_structure_graph_algorithm(p, G, rewrite_strategy, search_strategy, optimization),
        find_loops_guard(2), fatal_attractors_guard(2)
    {}

    // Optimization 2 is implemented by overriding the function rewrite_psi.

    pbes_expression rewrite_psi(const fixpoint_symbol& symbol,
                                const propositional_variable_instantiation& X,
                                const pbes_expression& psi
                               ) override
    {
      // N.B. The sets S0 and S1 need to be resized, because new vertices may have been added.
      S0.resize(m_graph_builder.m_vertices.size());
      S1.resize(m_graph_builder.m_vertices.size());
      pbes_expression x;
      std::tie(b, x) = Rplus(super::rewrite_psi(symbol, X, psi));
      return x;
    }

    void report_equation(const propositional_variable_instantiation& X, const pbes_expression& psi, std::size_t k) override
    {
      mCRL2log(log::debug) << "generated equation " << X << " = " << psi << " with rank " << k << std::endl;

      super::report_equation(X, psi, k);
      auto u = m_graph_builder.find_vertex(X);
      simple_structure_graph G(m_graph_builder.m_vertices);

      if (is_true(b))
      {
        S0.insert(u);
        if (m_optimization > 2)
        {
          // Compute the attractor set U of u, and add it to S0
          auto N = G.size();
          vertex_set U(N);
          U.insert(u);
          U = compute_attractor_set(G, U, 0);
          for (auto v: U.vertices())
          {
            if (v != u)
            {
              S0.insert(v);
            }
          }
        }
      }
      else if (is_false(b))
      {
        S1.insert(u);
        if (m_optimization > 2)
        {
          // Compute the attractor set U of u, and add it to S1
          auto N = G.size();
          vertex_set U(N);
          U.insert(u);
          U = compute_attractor_set(G, U, 1);
          for (auto v: U.vertices())
          {
            if (v != u)
            {
              S1.insert(v);
            }
          }
        }
      }
      if (m_optimization == 4 && find_loops_guard(std::max(S0.size(), S1.size())))
      {
        find_loops(G);
      }
      else if (m_optimization == 5 && fatal_attractors_guard(m_iteration_count))
      {
        fatal_attractors(G);
      }
      if (S0_guard(S0.size()))
      {
        compute_attractor_set_S0(G);
      }
      if (S1_guard(S1.size()))
      {
        compute_attractor_set_S1(G);
      }
    }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PBESINST_STRUCTURE_GRAPH2_H
