// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/stategraph_global_algorithm.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_STATEGRAPH_GLOBAL_ALGORITHM_H
#define MCRL2_PBES_DETAIL_STATEGRAPH_GLOBAL_ALGORITHM_H

#include "mcrl2/data/substitutions/sequence_sequence_substitution.h"
#include "mcrl2/pbes/algorithms.h"
#include "mcrl2/pbes/detail/stategraph_algorithm.h"
#include "mcrl2/pbes/detail/stategraph_global_graph.h"
#include "mcrl2/utilities/detail/container_utility.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

/// \brief Algorithm class for the global variant of the stategraph algorithm
class stategraph_global_algorithm: public stategraph_algorithm
{
  public:
    typedef stategraph_algorithm super;

  protected:
    // the control flow graph
    global_control_flow_graph m_control_flow_graph;

    // eq_X is the equation corresponding to u = (X, e)
    // eq_Y is the equation corresponding to Yf
    bool enabled_edge(const global_control_flow_graph_vertex& u, const stategraph_equation& eq_X, const predicate_variable& Yf)
    {
      auto const& e = u.values();
      auto const& cfp_X = eq_X.control_flow_parameter_indices();
      for (std::size_t k = 0; k < cfp_X.size(); k++)
      {
        auto q = Yf.source(cfp_X[k]);
        if (q != data::undefined_data_expression() && q != nth_element(e, k))
        {
          return false;
        }
      }
      return true;
    }

    // Returns k such that cfp[k] == l. Throws an exception if no such k exists.
    std::size_t unproject(const predicate_variable& Yf, const std::vector<std::size_t>& cfp, std::size_t l) const
    {
      mCRL2log(log::debug1, "stategraph") << "stategraph_global_algorithm::unproject: cfp = " << core::detail::print_list(cfp) << " l = " << l << std::endl;
      for (std::size_t k = 0; k < cfp.size(); k++)
      {
        if (Yf.copy(cfp[k]) == l)
        {
          return k;
        }
      }
      throw mcrl2::runtime_error("no index found in stategraph_global_algorithm::unproject");
    }

    const global_control_flow_graph_vertex& compute_vertex(global_control_flow_graph& G, const global_control_flow_graph_vertex& u, const stategraph_equation& eq_X, const predicate_variable& Yf, const stategraph_equation& eq_Y)
    {
      data::data_expression_vector f;
      auto const& X = u.name();
      auto const& e = u.values();
      auto const& cfp_X = eq_X.control_flow_parameter_indices();
      auto const& cfp_Y = eq_Y.control_flow_parameter_indices();

      mCRL2log(log::debug1, "stategraph") << "compute_vertex u = (X, e) = (" << X << ", " << core::detail::print_list(e) << "), Y(f) = " << Yf << std::endl;
      mCRL2log(log::debug1, "stategraph") << "cfp_X = " << core::detail::print_list(cfp_X) << std::endl;
      mCRL2log(log::debug1, "stategraph") << "cfp_Y = " << core::detail::print_list(cfp_Y) << std::endl;

      for (std::size_t l = 0; l < cfp_Y.size(); l++)
      {
        auto q = Yf.target(cfp_Y[l]);
        if (q != data::undefined_data_expression())
        {
          mCRL2log(log::debug1, "stategraph") << "q = " << q << std::endl;
          f.push_back(q);
        }
        else
        {
          mCRL2log(log::debug1, "stategraph") << "q = undefined" << std::endl;
          // Compute k such that (X, k) and (Y, l) are related. This implies copy(X, i, cfp_X[k]) == cfp_Y[l].
          //                                                                 Yf.copy[cfp_X[k]] = cfp_Y[l]
          auto p = cfp_Y[l];
          mCRL2log(log::debug2, "stategraph") << "Yf = " << Yf << "\n" << Yf.print() << " l = " << l << " Yf.copy(" << l << ") = " << p << std::endl;
          assert(p != data::undefined_index());
          std::size_t k = unproject(Yf, cfp_X, p);
          assert(k < e.size());
          f.push_back(nth_element(e, k));
        }
      }
      return G.insert_vertex(global_control_flow_graph_vertex(Yf.variable().name(), data::data_expression_list(f.begin(), f.end())));
    }

    void compute_significant_variables()
    {
      pbes_system::simplify_data_rewriter<data::rewriter> pbesr(m_datar);
      for (const auto& u : m_control_flow_graph)
      {
        auto const& X = u.name();
        auto const& eq_X = *find_equation(m_pbes, X);
        auto const& d = eq_X.control_flow_parameters();
        auto const& e = u.values();
        assert(d.size() == e.size());
        data::sequence_sequence_substitution<data::variable_vector, data::data_expression_list> sigma(d, e);
        u.set_significant_variables(pbes_system::algorithms::significant_variables(pbesr(eq_X.formula(), sigma)));
      }
    }

    void compute_global_control_flow_graph()
    {
      using utilities::detail::contains;
      using utilities::detail::pick_element;

      mCRL2log(log::debug, "stategraph") << "=== compute control flow graph ===" << std::endl;
      std::set<const global_control_flow_graph_vertex*> todo;
      std::set<const global_control_flow_graph_vertex*> done;

      // initialize todo
      auto const& Xinit = m_pbes.initial_state();
      auto const& eq_X = *find_equation(m_pbes, Xinit.name());
      auto einit = eq_X.project(Xinit.parameters());
      auto const& u = m_control_flow_graph.insert_vertex(global_control_flow_graph_vertex(Xinit.name(), einit));
      todo.insert(&u);

      while (!todo.empty())
      {
        // u = (X, e)
        auto const& u = *pick_element(todo);
        auto const& X = u.name();
        auto const& eq_X = *find_equation(m_pbes, X);
        auto const& predvars = eq_X.predicate_variables();

        mCRL2log(log::debug1, "stategraph") << "choose todo element " << u << std::endl;

        for (std::size_t i = 0; i < predvars.size(); i++)
        {
          auto const& Yf = predvars[i];
          auto const& Y = Yf.variable().name();
          auto const& eq_Y = *find_equation(m_pbes, Y);
          if (enabled_edge(u, eq_X, Yf))
          {
            const global_control_flow_graph_vertex& v = compute_vertex(m_control_flow_graph, u, eq_X, Yf, eq_Y);
            m_control_flow_graph.insert_edge(u, i, v);
            if (!contains(done, &v))
            {
              mCRL2log(log::debug1, "stategraph") << "insert todo element " << v << std::endl;
              todo.insert(&v);
              done.insert(&v);
            }
          }
        }
      }
      m_control_flow_graph.compute_index();
      compute_significant_variables();
      mCRL2log(log::debug) << "--- global control flow graph ---\n" << m_control_flow_graph << std::endl;
    }

  public:
    stategraph_global_algorithm(const pbes& p, const pbesstategraph_options& options)
      : stategraph_algorithm(p, options)
    { }

    /// \brief Computes the control flow graph
    void run()
    {
      super::run();
      start_timer("compute_global_control_flow_graph");
      compute_global_control_flow_graph();
      finish_timer("compute_global_control_flow_graph");
    }
};

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_STATEGRAPH_GLOBAL_ALGORITHM_H
