// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
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
#include "mcrl2/pbes/pbessolve_vertex_set.h"
#include "mcrl2/pbes/structure_graph.h"
#include "mcrl2/pbes/structure_graph_builder.h"

namespace mcrl2 {

namespace pbes_system {

/// \brief Variant of pbesinst that will compute a structure graph for a PBES.
/// The result will be put in the structure graph that is passed in the constructor.
class pbesinst_structure_graph_algorithm: public pbesinst_lazy_algorithm
{
  protected:
    detail::structure_graph_builder m_graph_builder;

    void SG0(const propositional_variable_instantiation& X, const pbes_expression& psi, std::size_t k)
    {
      auto vertex_phi = m_graph_builder.insert_variable(X, psi, k);
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
        auto vertex_psi = m_graph_builder.insert_variable(psi);
        m_graph_builder.insert_edge(vertex_phi, vertex_psi);
      }
      else if (is_and(psi))
      {
        for (const pbes_expression& psi_i: split_and(psi))
        {
          auto vertex_psi_i = SG1(psi_i);
          m_graph_builder.insert_edge(vertex_phi, vertex_psi_i);
        }
      }
      else if (is_or(psi))
      {
        for (const pbes_expression& psi_i: split_or(psi))
        {
          auto vertex_psi_i = SG1(psi_i);
          m_graph_builder.insert_edge(vertex_phi, vertex_psi_i);
        }
      }
    }

    structure_graph::index_type SG1(const pbes_expression& psi)
    {
      auto vertex_psi = m_graph_builder.insert_vertex(psi);
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
          auto vertex_psi_i = SG1(psi_i);
          m_graph_builder.insert_edge(vertex_psi, vertex_psi_i);
        }
      }
      else if (is_or(psi))
      {
        for (const pbes_expression& psi_i: split_or(psi))
        {
          auto vertex_psi_i = SG1(psi_i);
          m_graph_builder.insert_edge(vertex_psi, vertex_psi_i);
        }
      }
      return vertex_psi;
    }

  public:
    pbesinst_structure_graph_algorithm(
      const pbessolve_options& options,
      const pbes& p,
      structure_graph& G
    )
      : pbesinst_lazy_algorithm(options, p),
        m_graph_builder(G)
    {}

    void on_report_equation(const propositional_variable_instantiation& X, const pbes_expression& psi, std::size_t k) override
    {
      // the body of this if statement will only be executed for the first equation
      if (m_graph_builder.m_initial_state == data::undefined_data_expression())
      {
        m_graph_builder.set_initial_state(X);
      }
      SG0(X, psi, k);
    }

    void run() override
    {
      pbesinst_lazy_algorithm::run();
      m_graph_builder.finalize();
    }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PBESINST_STRUCTURE_GRAPH_H
