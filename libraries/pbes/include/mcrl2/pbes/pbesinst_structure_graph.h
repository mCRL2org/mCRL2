// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/pbesinst_structure_graph.h
/// \brief A variant of the lazy algorithm for instantiating a PBES, that produces a structure_graph.

#ifndef MCRL2_PBES_PBESINST_STRUCTURE_GRAPH_H
#define MCRL2_PBES_PBESINST_STRUCTURE_GRAPH_H

#include "mcrl2/pbes/algorithms.h"
#include "mcrl2/pbes/join.h"
#include "mcrl2/pbes/pbesinst_lazy.h"
#include "mcrl2/pbes/structure_graph.h"

namespace mcrl2 {

namespace pbes_system {

/// \brief Variant of pbesinst that will compute a structure graph for a PBES.
/// The result will be put in the structure graph that is passed in the constructor.
class pbesinst_structure_graph_algorithm: public pbesinst_lazy_algorithm
{
  protected:
    detail::structure_graph_builder m_graph_builder;
    bool m_initial_state_assigned;

    void SG0(const propositional_variable_instantiation& X, const pbes_expression& psi, std::size_t k)
    {
      int vertex_phi = m_graph_builder.insert_variable(X, psi, k);
      if (is_true(psi))
      {
        // skip
      }
      else if (is_false(psi))
      {
        // skip
      }
      else if (is_propositional_variable_instantiation(psi))
      {
        int vertex_psi = m_graph_builder.insert_variable(psi);
        m_graph_builder.insert_edge(vertex_phi, vertex_psi);
      }
      else if (is_and(psi))
      {
        for (const pbes_expression& psi_i: split_and(psi))
        {
          int vertex_psi_i = SG1(psi_i);
          m_graph_builder.insert_edge(vertex_phi, vertex_psi_i);
        }
      }
      else if (is_or(psi))
      {
        for (const pbes_expression& psi_i: split_or(psi))
        {
          int vertex_psi_i = SG1(psi_i);
          m_graph_builder.insert_edge(vertex_phi, vertex_psi_i);
        }
      }
    }

    int SG1(const pbes_expression& psi)
    {
      int vertex_psi = m_graph_builder.insert_vertex(psi);
      if (is_true(psi))
      {
        // skip
      }
      else if (is_false(psi))
      {
        // skip
      }
      else if (is_propositional_variable_instantiation(psi))
      {
        // skip
      }
      else if (is_and(psi))
      {
        for (const pbes_expression& psi_i: split_and(psi))
        {
          int vertex_psi_i = SG1(psi_i);
          m_graph_builder.insert_edge(vertex_psi, vertex_psi_i);
        }
      }
      else if (is_or(psi))
      {
        for (const pbes_expression& psi_i: split_or(psi))
        {
          int vertex_psi_i = SG1(psi_i);
          m_graph_builder.insert_edge(vertex_psi, vertex_psi_i);
        }
      }
      return vertex_psi;
    }

  public:
    pbesinst_structure_graph_algorithm(
         const pbes& p,
         structure_graph& G,
         data::rewriter::strategy rewrite_strategy = data::jitty,
         search_strategy search_strategy = breadth_first,
         transformation_strategy transformation_strategy = lazy
        )
      : pbesinst_lazy_algorithm(p, rewrite_strategy, search_strategy, transformation_strategy),
        m_graph_builder(G),
        m_initial_state_assigned(false)
    {}

    void report_equation(const propositional_variable_instantiation& X, const pbes_expression& psi, std::size_t k) override
    {
      if (!m_initial_state_assigned)
      {
        m_graph_builder.set_initial_state(X);
        m_initial_state_assigned = true;
      }
      SG0(X, psi, k);
    }

    void run() override
    {
      pbesinst_lazy_algorithm::run();
      m_graph_builder.finalize();
    }
};

inline
void pbesinst_structure_graph(const pbes& p,
                              structure_graph& G,
                              data::rewriter::strategy rewrite_strategy = data::jitty,
                              search_strategy search_strategy = breadth_first,
                              transformation_strategy transformation_strategy = lazy
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
  pbesinst_structure_graph_algorithm algorithm(q, G, rewrite_strategy, search_strategy, transformation_strategy);
  algorithm.run();
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PBESINST_STRUCTURE_GRAPH_H
