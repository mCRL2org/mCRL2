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

    bool solution_found(const propositional_variable_instantiation& init) const override
    {
      auto u = m_graph_builder.find_vertex(init);
      return S0.contains(u) || S1.contains(u);
    }

  public:
    typedef pbesinst_structure_graph_algorithm super;

    pbesinst_structure_graph_algorithm2(
        const pbes& p,
        structure_graph& G,
        data::rewriter::strategy rewrite_strategy = data::jitty,
        search_strategy search_strategy = breadth_first,
        transformation_strategy transformation_strategy = lazy,
        int optimization = 0
    )
      : pbesinst_structure_graph_algorithm(p, G, rewrite_strategy, search_strategy, transformation_strategy, optimization)
    {}

    pbes_expression rewrite_psi(const pbes_expression& psi) override
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
      std::tie(b, x) = Rplus(psi);
      return x;
    }

    void report_equation(const propositional_variable_instantiation& X, const pbes_expression& psi, std::size_t k) override
    {
      super::report_equation(X, psi, k);
      if (is_true(b))
      {
        auto u = m_graph_builder.find_vertex(X);
        S0.insert(u);
        if (m_optimization > 2)
        {
          simple_structure_graph G(m_graph_builder.m_vertices);

          // Compute the attractor set U of u, and add it to S0
          auto N = G.size();
          vertex_set U(N);
          U.insert(u);
          U = compute_attractor_set(G, U, 0);
          for (auto v: U.vertices())
          {
            S0.insert(v);
          }

          // Compute the attractor set of S0
          if (S0_guard(S0.size()))
          {
            S0 = compute_attractor_set(G, S0, 0);
          }
        }
      }
      else if (is_false(b))
      {
        auto u = m_graph_builder.find_vertex(X);
        S1.insert(u);
        if (m_optimization > 2)
        {
          simple_structure_graph G(m_graph_builder.m_vertices);

          // Compute the attractor set U of u, and add it to S1
          auto N = G.size();
          vertex_set U(N);
          U.insert(u);
          U = compute_attractor_set(G, U, 1);
          for (auto v: U.vertices())
          {
            S0.insert(v);
          }

          // Compute the attractor set of S1
          if (S1_guard(S1.size()))
          {
            S1 = compute_attractor_set(G, S1, 1);
          }
        }
      }
    }
};

inline
void pbesinst_structure_graph2(const pbes& p,
                               structure_graph& G,
                               data::rewriter::strategy rewrite_strategy = data::jitty,
                               search_strategy search_strategy = breadth_first,
                               transformation_strategy transformation_strategy = lazy,
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
  pbesinst_structure_graph_algorithm2 algorithm(q, G, rewrite_strategy, search_strategy, transformation_strategy, optimization1);
  algorithm.run();
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PBESINST_STRUCTURE_GRAPH2_H
