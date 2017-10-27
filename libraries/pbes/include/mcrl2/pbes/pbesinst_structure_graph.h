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

#include "mcrl2/pbes/join.h"
#include "mcrl2/pbes/pbesinst_lazy.h"
#include "mcrl2/pbes/structure_graph.h"

namespace mcrl2 {

namespace pbes_system {

class pbesinst_structure_graph: public pbesinst_lazy_algorithm
{
  protected:
    structure_graph m_graph;

    void SG0(const propositional_variable_instantiation& X, const pbes_expression& psi, std::size_t k)
    {
      const auto& vertex_phi = m_graph.insert_variable(X, k);
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
        const auto& vertex_psi = m_graph.insert_variable(atermpp::down_cast<propositional_variable_instantiation>(psi));
        m_graph.insert_edge(vertex_phi, vertex_psi);
      }
      else if (is_and(psi))
      {
        for (const pbes_expression& psi_i: split_and(psi))
        {
          const auto& vertex_psi_i = SG1(psi_i);
          m_graph.insert_edge(vertex_phi, vertex_psi_i);
        }
      }
      else if (is_or(psi))
      {
        for (const pbes_expression& psi_i: split_or(psi))
        {
          const auto& vertex_psi_i = SG1(psi_i);
          m_graph.insert_edge(vertex_phi, vertex_psi_i);
        }
      }
    }

    const structure_graph::vertex& SG1(const pbes_expression& psi)
    {
      const auto& vertex_psi = m_graph.insert_vertex(psi);
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
          const auto& vertex_psi_i = SG1(psi_i);
          m_graph.insert_edge(vertex_psi, vertex_psi_i);
        }
      }
      else if (is_or(psi))
      {
        for (const pbes_expression& psi_i: split_or(psi))
        {
          const auto& vertex_psi_i = SG1(psi_i);
          m_graph.insert_edge(vertex_psi, vertex_psi_i);
        }
      }
      return vertex_psi;
    }

  public:
    pbesinst_structure_graph(
         const pbes& p,
         data::rewriter::strategy rewrite_strategy = data::jitty,
         search_strategy search_strategy = breadth_first,
         transformation_strategy transformation_strategy = lazy
        )
      : pbesinst_lazy_algorithm(p, rewrite_strategy, search_strategy, transformation_strategy)
    {}

    void report_equation(const propositional_variable_instantiation& X, const pbes_expression& psi, std::size_t k)
    {
      SG0(X, psi, k);
    }

    const structure_graph& get_graph() const
    {
      return m_graph;
    }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PBESINST_STRUCTURE_GRAPH_H
