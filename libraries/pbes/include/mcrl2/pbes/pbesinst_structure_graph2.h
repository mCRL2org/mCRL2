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

struct resizable_bitset
{
  boost::dynamic_bitset<> v;

  resizable_bitset()
    : v(64)
  { }

  void insert(std::size_t i)
  {
    if (v.size() <= i)
    {
      auto size = v.size();
      while (size <= i)
      {
        size *= 2;
      }
      v.resize(size);
    }
    v[i] = true;
  }

  bool contains(std::size_t i) const
  {
    return v[i];
  }
};

inline
bool find_loop(const simple_structure_graph& G,
               const detail::resizable_bitset& U,
               structure_graph::index_type v,
               structure_graph::index_type w,
               std::size_t p,
               std::unordered_map<structure_graph::index_type, bool>& visited
              )
{
  const auto& w_ = G.find_vertex(w);
  if (w_.decoration == structure_graph::d_true || w_.decoration == structure_graph::d_false)
  {
    return false;
  }
  if (w_.rank != data::undefined_index() && w_.rank != p)
  {
    return false;
  }
  auto i = visited.find(w);
  if (i != visited.end())
  {
    return i->second;
  }
  if (is_propositional_variable_instantiation(w_.formula) && U.contains(w))
  {
    visited[w] = false;
    if (w_.decoration == data::undefined_index() ||
                   ((w_.rank % 2 == 0 && w_.decoration == structure_graph::d_disjunction) ||
                    (w_.rank % 2 != 0 && w_.decoration == structure_graph::d_conjunction))
       )
    {
      for (structure_graph::index_type u: w_.successors)
      {
        if (u == v || find_loop(G, U, v, u, p, visited))
        {
          visited[w] = true;
          return true;
        }
      }
    }
    else
    {
      for (structure_graph::index_type u: w_.successors)
      {
        if (u != v && !find_loop(G, U, v, u, p, visited))
        {
          visited[w] = false;
          return false;
        }
      }
    }
  }
  return false;
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
    detail::periodic_guard reset_guard;

    // Contains the vertices that have been reported.
    detail::resizable_bitset done;

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
        bool b = detail::find_loop(G, done, u, u, u_.rank, visited);
        visited[u] = b;
        if (b)
        {
          if (u_.decoration == structure_graph::d_conjunction)
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
                              const structure_graph::vertex& u_,
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

        if (u_.decoration == structure_graph::d_none && has_successor_not_in(G, u, u_, S0, S1, done1))
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
      : pbesinst_structure_graph_algorithm(p, G, rewrite_strategy, search_strategy, optimization)
    {}

    pbes_expression rewrite_psi(const fixpoint_symbol& symbol,
                                const propositional_variable_instantiation& X,
                                const pbes_expression& psi
                               ) override
    {
      // N.B. The sets S0 and S1 need to be resized, because new vertices may have been added.
      S0.resize(m_graph_builder.m_vertices.size());
      S1.resize(m_graph_builder.m_vertices.size());
//      return replace_propositional_variables(psi, [&](const propositional_variable_instantiation& x) {
//          auto u = m_graph_builder.find_vertex(x);
//          if (u != structure_graph::undefined_vertex)
//          {
//            if (S0.contains(u))
//            {
//              return expr(true_());
//            }
//            else if (S1.contains(u))
//            {
//              return expr(false_());
//            }
//          }
//          return expr(x);
//        }
//      );
      pbes_expression x;
      std::tie(b, x) = Rplus(super::rewrite_psi(symbol, X, psi));
      return x;
    }

    void report_equation(const propositional_variable_instantiation& X, const pbes_expression& psi, std::size_t k) override
    {
      super::report_equation(X, psi, k);
      auto u = m_graph_builder.find_vertex(X);
      done.insert(u);
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
            if (v != u && !S0.contains(v))
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
            if (v != u && !S1.contains(v))
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

inline
void pbesinst_structure_graph2(const pbes& p,
                               structure_graph& G,
                               data::rewriter::strategy rewrite_strategy = data::jitty,
                               search_strategy search_strategy = breadth_first,
                               bool optimization1 = false
                              )
{
  if (search_strategy == breadth_first_short)
  {
    throw mcrl2::runtime_error("The breadth_first_short option is not supported!");
  }
  if (search_strategy == depth_first_short)
  {
    throw mcrl2::runtime_error("The depth_first_short option is not supported!");
  }
  pbes q = p;
  if (!algorithms::is_normalized(q))
  {
    algorithms::normalize(q);
  }
  pbesinst_structure_graph_algorithm2 algorithm(q, G, rewrite_strategy, search_strategy, optimization1);
  algorithm.run();
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PBESINST_STRUCTURE_GRAPH2_H
