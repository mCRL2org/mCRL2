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

#include "mcrl2/pbes/replace.h"
#include "mcrl2/pbes/simple_structure_graph.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

}

/// \brief Adds an optimization to pbesinst_structure_graph.
class pbesinst_structure_graph_algorithm2: public pbesinst_structure_graph_algorithm
{
  protected:
    vertex_set S0;
    vertex_set S1;

  public:
    typedef pbesinst_structure_graph_algorithm super;

    pbesinst_structure_graph_algorithm2(
        const pbes& p,
        structure_graph& G,
        data::rewriter::strategy rewrite_strategy = data::jitty,
        search_strategy search_strategy = breadth_first,
        transformation_strategy transformation_strategy = lazy,
        bool optimization1 = false
    )
      : pbesinst_structure_graph_algorithm(p, G, rewrite_strategy, search_strategy, transformation_strategy, optimization1)
    {}

    pbes_expression rewrite_psi(const pbes_expression& psi) override
    {
      // N.B. The sets S0 and S1 need to be resized, because new vertices may have been added.
      S0.resize(m_graph_builder.m_vertices.size());
      S1.resize(m_graph_builder.m_vertices.size());
      return replace_propositional_variables(psi, [&](const propositional_variable_instantiation& x) {
          auto u = m_graph_builder.find_vertex(x);
          if (u != structure_graph::undefined_vertex)
          {
            if (S0.contains(u))
            {
              return static_cast<const pbes_expression&>(true_());
            }
            else if (S1.contains(u))
            {
              return static_cast<const pbes_expression&>(false_());
            }
          }
          return static_cast<const pbes_expression&>(x);
        }
      );
    }

    void report_equation(const propositional_variable_instantiation& X, const pbes_expression& psi, std::size_t k) override
    {
      super::report_equation(X, psi, k);
      if (is_true(psi))
      {
        S0.insert(m_graph_builder.find_vertex(X));
        simple_structure_graph G(m_graph_builder.m_vertices);
        S0 = compute_attractor_set(G, S0, 0);
      }
      else if (is_false(psi))
      {
        S1.insert(m_graph_builder.find_vertex(X));
        simple_structure_graph G(m_graph_builder.m_vertices);
        S1 = compute_attractor_set(G, S1, 1);
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
