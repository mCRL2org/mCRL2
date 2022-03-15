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

#include <iomanip>
#include <shared_mutex>

#include "mcrl2/pbes/algorithms.h"
#include "mcrl2/pbes/join.h"
#include "mcrl2/pbes/pbesinst_lazy.h"
#include "mcrl2/pbes/structure_graph_builder.h"

namespace mcrl2 {

namespace pbes_system {

/// \brief Variant of pbesinst that will compute a structure graph for a PBES.
/// The result will be put in the structure graph that is passed in the constructor.
class pbesinst_structure_graph_algorithm: public pbesinst_lazy_algorithm
{
  protected:
    detail::structure_graph_builder m_graph_builder;

    void SG0(const propositional_variable_instantiation& X, const pbes_expression& psi, std::size_t k, std::shared_mutex& realloc_mutex)
    {
      auto vertex_phi = m_graph_builder.insert_variable(X, psi, k, realloc_mutex);
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
        auto vertex_psi = m_graph_builder.insert_variable(psi, realloc_mutex);
        m_graph_builder.insert_edge(vertex_phi, vertex_psi, realloc_mutex);
      }
      else if (is_and(psi))
      {
        for (const pbes_expression& psi_i: split_and(psi))
        {
          auto vertex_psi_i = SG1(psi_i, realloc_mutex);
          m_graph_builder.insert_edge(vertex_phi, vertex_psi_i, realloc_mutex);
        }
      }
      else if (is_or(psi))
      {
        for (const pbes_expression& psi_i: split_or(psi))
        {
          auto vertex_psi_i = SG1(psi_i, realloc_mutex);
          m_graph_builder.insert_edge(vertex_phi, vertex_psi_i, realloc_mutex);
        }
      }
    }

    structure_graph::index_type SG1(const pbes_expression& psi, std::shared_mutex& realloc_mutex)
    {
      auto vertex_psi = m_graph_builder.insert_vertex(psi, realloc_mutex);
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
          auto vertex_psi_i = SG1(psi_i, realloc_mutex);
          m_graph_builder.insert_edge(vertex_psi, vertex_psi_i, realloc_mutex);
        }
      }
      else if (is_or(psi))
      {
        for (const pbes_expression& psi_i: split_or(psi))
        {
          auto vertex_psi_i = SG1(psi_i, realloc_mutex);
          m_graph_builder.insert_edge(vertex_psi, vertex_psi_i, realloc_mutex);
        }
      }
      return vertex_psi;
    }

    std::string status_message(std::size_t equation_count) override
    {
      if (equation_count > 0 && equation_count % 1000 == 0)
      {
        std::ostringstream out;
        out << "Generated " << equation_count << " BES equations (" << std::fixed << std::setprecision(2) <<
          ((100.0 * equation_count) / m_graph_builder.extent()) << "% explored)" << std::endl;
        return out.str();
      }
      return "";
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

    void on_report_equation(const std::size_t /* thread_index */,
                            std::shared_mutex& realloc_mutex,
                            const propositional_variable_instantiation& X,
                            const pbes_expression& psi,
                            std::size_t k
                           ) override
    {
      // the body of this if statement will only be executed for the first equation
      if (m_graph_builder.m_initial_state == data::undefined_data_expression())
      {
        m_graph_builder.set_initial_state(X);
      }
      SG0(X, psi, k, realloc_mutex);
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
