// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/stategraph_local_algorithm.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_STATEGRAPH_LOCAL_ALGORITHM_H
#define MCRL2_PBES_DETAIL_STATEGRAPH_LOCAL_ALGORITHM_H

#include "mcrl2/pbes/algorithms.h"
#include "mcrl2/pbes/detail/stategraph_algorithm.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

/// \brief Algorithm class for the local variant of the stategraph algorithm
class stategraph_local_algorithm: public stategraph_algorithm
{
  public:
    typedef stategraph_algorithm super;

  protected:
    // m_control_flow_index[X][j] = k => d_X[j] corresponds to m_control_flow_graphs[k]
    std::map<core::identifier_string, std::map<std::size_t, std::size_t> > m_control_flow_index;

    // m_belongs[k] corresponds with m_control_flow_graphs[k]
    std::vector<std::map<core::identifier_string, std::set<data::variable> > > m_belongs;

    void compute_control_flow_index()
    {
      for (std::size_t k = 0; k < m_control_flow_graphs.size(); k++)
      {
        const std::set<std::size_t>& component = m_connected_components[k];
        for (auto i = component.begin(); i != component.end(); ++i)
        {
          const control_flow_graph_vertex& u = m_control_flow_graph_vertices[*i];
          m_control_flow_index[u.name()][u.index()] = k;
        }
      }
    }

    void print_control_flow_index() const
    {
      std::ostringstream out;
      for (std::map<core::identifier_string, std::map<std::size_t, std::size_t> >::const_iterator i = m_control_flow_index.begin(); i != m_control_flow_index.end(); ++i)
      {
        const core::identifier_string& X = i->first;
        const std::map<std::size_t, std::size_t>& m = i->second;
        for (std::map<std::size_t, std::size_t>::const_iterator j = m.begin(); j != m.end(); ++j)
        {
          out << "(" << core::pp(X) << ", " << j->first << ") is part of graph " << j->second << std::endl;
        }
      }
      mCRL2log(log::debug, "stategraph") << "=== control flow index ===\n" << out.str() << std::endl;
    }

    // returns an index k such that d_X[j] corresponds to m_control_flow_graphs[k],
    // or std::numeric_limits<std::size_t>::max() if no such index was found
    std::size_t control_flow_index(const core::identifier_string& X, std::size_t j) const
    {
      std::map<core::identifier_string, std::map<std::size_t, std::size_t> >::const_iterator k = m_control_flow_index.find(X);
      if (k == m_control_flow_index.end())
      {
        return (std::numeric_limits<std::size_t>::max)();
      }
      const std::map<std::size_t, std::size_t>& m = k->second;
      std::map<std::size_t, std::size_t>::const_iterator i = m.find(j);
      if (i == m.end())
      {
        return (std::numeric_limits<std::size_t>::max)();
      }
      return i->second;
    }

    // @pre: (X0, p0) is in must_graph
    std::map<core::identifier_string, std::set<data::variable> > compute_belongs(const core::identifier_string& X0, std::size_t p0, const control_flow_graph& /* Vk unused. */ )
    {
      mCRL2log(log::debug, "stategraph") << "--- compute belongs (" << core::pp(X0) << ", " << p0 << ")" << std::endl;

      std::map<core::identifier_string, std::set<data::variable> > result;

/* TODO: remove the dependency on the must graph.
      std::set<const control_flow_graph_vertex*> todo;
      std::set<const control_flow_graph_vertex*> visited;

      const control_flow_graph_vertex& y0 = *find_vertex(X0, p0);
      todo.insert(&y0);
      mCRL2log(log::debug, "stategraph") << " insert todo element (" << core::pp(X0) << ", " << p0 << ")" << std::endl;
      while (!todo.empty())
      {
        std::set<const control_flow_graph_vertex*>::iterator ti = todo.begin();

        // y = (X, p)
        const control_flow_graph_vertex& y = **ti;
        const core::identifier_string& X = y.name();
        std::size_t p = y.index();
        todo.erase(ti);
        visited.insert(*ti);

        mCRL2log(log::debug, "stategraph") << " choose todo element (" << core::pp(X) << ", " << p << ")" << std::endl;

        if (y.outgoing_edges.empty())
        {
          continue;
        }

        const stategraph_equation& eq_X = *find_equation(m_pbes, X);
        std::vector<std::size_t> dp = data_parameter_indices(X);
        std::set<std::size_t> belongs(dp.begin(), dp.end());

        const std::set<control_flow_graph_vertex*>& E = y.outgoing_edges;
        for (std::set<control_flow_graph_vertex*>::const_iterator ei = E.begin(); ei != E.end(); ++ei)
        {
          // z = (Y, q)
          const control_flow_graph_vertex& z = **ei;
          const core::identifier_string& Y = z.X;
          std::size_t q = z.p;

          for (std::size_t i = 0; i < eq_X.predicate_variables().size(); i++)
          {
            const predicate_variable& X_i = eq_X.predicate_variables()[i];
            if (X_i.X.name() != Y)
            {
              continue;
            }
            if (visited.find(&z) == visited.end())
            {
              todo.insert(&z);
              mCRL2log(log::debug, "stategraph") << " insert todo element (" << core::pp(Y) << ", " << q << ")" << std::endl;
            }
            for (std::set<std::size_t>::iterator j = belongs.begin(); j != belongs.end(); )
            {
              if ((X_i.used.find(*j) != X_i.used.end() || X_i.changed.find(*j) != X_i.changed.end()) && !Vk.has_label(X, i))
              {
                belongs.erase(j++);
              }
              else
              {
                ++j;
              }
            }
          }
        }
        for (std::set<std::size_t>::const_iterator j = belongs.begin(); j != belongs.end(); ++j)
        {
          result[X].insert(eq_X.parameters()[*j]);
        }
      }
*/
      return result;
    }

    void compute_belongs()
    {
      mCRL2log(log::debug, "stategraph") << "=== computing belongs relation ===" << std::endl;
      const propositional_variable_instantiation& X_init = m_pbes.initial_state();
      const core::identifier_string& X = X_init.name();
      std::vector<std::size_t> CFP = control_flow_parameter_indices(X);
      for (std::size_t k = 0; k < m_control_flow_graphs.size(); k++)
      {
        std::map<core::identifier_string, std::set<data::variable> > Bk = compute_belongs(X, CFP[k], m_control_flow_graphs[k]);
        m_belongs.push_back(Bk);
      }
    }

    void print_belongs() const
    {
      std::ostringstream out;
      out << "\n=== belongs relation ===\n";
      for (std::size_t k = 0; k < m_belongs.size(); k++)
      {
        out << "--- belongs " << k << " ---" << std::endl;
        const std::map<core::identifier_string, std::set<data::variable> >& Bk = m_belongs[k];
        for (std::map<core::identifier_string, std::set<data::variable> >::const_iterator i = Bk.begin(); i != Bk.end(); ++i)
        {
          out << core::pp(i->first) << " -> " << core::detail::print_set(i->second) << std::endl;
        }
      }
      mCRL2log(log::debug, "stategraph") << out.str() << std::endl;
    }

    // Returns true if there is a local control flow graph Gk such that
    // 1) k != k0
    // 2) (X, d) \in Bk
    // 3) Gk has a vertex u = X(e) such that d \in marking(u)
    bool is_marked_in_other_graph(const core::identifier_string& X, const data::variable& d, std::size_t k0)
    {
      std::size_t K = m_control_flow_graphs.size();
      for (std::size_t k = 0; k < K; k++)
      {
        if (k == k0)
        {
          continue;
        }
        if (!utilities::detail::contains(m_belongs[k][X], d))
        {
          continue;
        }
        control_flow_graph& Gk = m_control_flow_graphs[k];
        const std::set<stategraph_vertex*>& inst = Gk.index(X);
        for (std::set<stategraph_vertex*>::const_iterator i = inst.begin(); i != inst.end(); ++i)
        {
          const stategraph_vertex& u = **i;
          if (utilities::detail::contains(u.marking, d))
          {
            mCRL2log(log::debug, "stategraph") << "is_marked_in_other_graph(" << core::pp(X) << ", " << data::pp(d) << ") = true" << std::endl;
            mCRL2log(log::debug, "stategraph") << "witness: u = " << pbes_system::pp(u.X) << " in graph " << k << std::endl;
            return true;
          }
        }
      }
      return false;
    }

    std::string print_vertex(const stategraph_vertex& u) const
    {
      std::ostringstream out;
      out << pbes_system::pp(u.X)
          << " marking = " << core::detail::print_set(u.marking)
          << " marking indices = " << print_set(u.marking_variable_indices(m_pbes));
      return out.str();
    }

    void compute_control_flow_marking()
    {
      mCRL2log(log::debug, "stategraph") << "=== computing control flow marking ===" << std::endl;
      using data::detail::set_difference;
      using data::detail::set_intersection;
      using data::detail::set_union;

      std::size_t K = m_control_flow_graphs.size();

      // initial marking
      for (std::size_t k = 0; k < K; k++)
      {
        control_flow_graph& Gk = m_control_flow_graphs[k];
        for (control_flow_graph::vertex_iterator i = Gk.begin(); i != Gk.end(); ++i)
        {
          stategraph_vertex& u = i->second;
          const core::identifier_string& X = u.X.name();
          u.marking = set_intersection(u.sig, m_belongs[k][X]);
        }
        mCRL2log(log::debug, "stategraph") << "--- initial control flow marking " << k << "\n" << Gk.print_marking();
      }

      bool stable = false;
      while (!stable)
      {
        stable = true;
        for (std::size_t k = 0; k < K; k++)
        {
          // initialize todo list
          control_flow_graph& Gk = m_control_flow_graphs[k];
          std::set<stategraph_vertex*> todo;
          for (vertex_iterator gi = Gk.begin(); gi != Gk.end(); ++gi)
          {
            stategraph_vertex& v = gi->second;
            todo.insert(&v);
          }

          while (!todo.empty())
          {
            std::set<stategraph_vertex*>::iterator ti = todo.begin();
            todo.erase(ti);

            // v = X(e)
            stategraph_vertex& v = **ti;
            std::set<std::size_t> v_index = v.marking_variable_indices(m_pbes);
            // const core::identifier_string& X = v.X.name();
            mCRL2log(log::debug, "stategraph") << "selected todo element v = " << print_vertex(v) << std::endl;
            for (std::set<stategraph_edge>::iterator ei = v.incoming_edges.begin(); ei != v.incoming_edges.end(); ++ei)
            {
              // u = Y(f)
              // edge = (u, v) with label i
              stategraph_vertex& u = *(ei->source);
              std::set<std::size_t> u_index = u.marking_variable_indices(m_pbes);
              std::size_t i = ei->label;
              const stategraph_equation& eq_Y = *find_equation(m_pbes, u.X.name());

              // Y_i = X(g)
              const predicate_variable& Y_i = eq_Y.predicate_variables()[i];
              const core::identifier_string& Y = u.X.name();
              // const data::data_expression_list& f = u.X.parameters();
              mCRL2log(log::debug, "stategraph") << "  vertex u = " << print_vertex(u) << std::endl;
              for (std::set<std::size_t>::const_iterator ji = v_index.begin(); ji != v_index.end(); ++ji)
              {
                std::size_t j = *ji;
                data::data_expression g_j = nth_element(Y_i.X.parameters(), j);
                std::set<data::variable> M = set_intersection(set_difference(FV(g_j), u.marking), m_belongs[k][Y]);
                if (!M.empty())
                {
                  mCRL2log(log::debug, "stategraph") << "update marking u with M = " << core::detail::print_set(M) << std::endl;
                  u.marking = set_union(u.marking, M);
                  todo.insert(&u);
                  stable = false;
                }
              }
            }
          }

          for (vertex_iterator gi = Gk.begin(); gi != Gk.end(); ++gi)
          {
            // u = X(e)
            stategraph_vertex& u = gi->second;
            const core::identifier_string& X = u.X.name();
            const stategraph_equation& eq_X = *find_equation(m_pbes, X);
            for (std::size_t i = 0; i < eq_X.predicate_variables().size(); i++)
            {
              const predicate_variable& X_i = eq_X.predicate_variables()[i];
              const core::identifier_string& Y = X_i.X.name();
              const data::data_expression_list& f = X_i.X.parameters();
              const stategraph_equation& eq_Y = *find_equation(m_pbes, Y);
              const std::vector<data::variable>& d_Y = eq_Y.parameters();
              for (std::size_t l = 0; l < d_Y.size(); ++l)
              {
                std::map<core::identifier_string, std::set<data::variable> >& Bk = m_belongs[k];
                if (!utilities::detail::contains(Bk[Y], d_Y[l]))
                {
                  std::set<data::variable> M = set_difference(set_intersection(m_belongs[k][X], FV(nth_element(f, l))), u.marking);
                  if (M.empty())
                  {
                    continue;
                  }
                  if (is_marked_in_other_graph(Y, d_Y[l], k))
                  {
                    u.marking = set_union(u.marking, M);
                    mCRL2log(log::debug, "stategraph") << "update marking u with M = " << core::detail::print_set(M) << std::endl;
                    stable = false;
                  }
                }
              }
            }
          }
        }
      }
    }

    void print_control_flow_marking() const
    {
      mCRL2log(log::debug, "stategraph") << "\n=== control flow marking ===" << std::endl;
      std::size_t K = m_control_flow_graphs.size();
      for (std::size_t k = 0; k < K; k++)
      {
        const control_flow_graph& Gk = m_control_flow_graphs[k];
        mCRL2log(log::debug, "stategraph") <<  "--- control flow marking for graph " << k << "\n" << Gk.print_marking() << std::endl;
      }
    }

  public:
    stategraph_local_algorithm(const pbes& p, data::rewriter::strategy rewrite_strategy = data::jitty,
                               bool use_alternative_lcfp_criterion = false,
                               bool use_alternative_gcfp_relation = false,
                               bool use_alternative_gcfp_consistency = false
                              )
      : stategraph_algorithm(p, rewrite_strategy, use_alternative_lcfp_criterion, use_alternative_gcfp_relation, use_alternative_gcfp_consistency)
    { }

    /// \brief Computes the control flow graph
    void run()
    {
      super::run();
      compute_control_flow_index();
      print_control_flow_index();
//      compute_belongs();
//      print_belongs();
//      compute_control_flow_marking();
//      print_control_flow_marking();
    }
};

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_STATEGRAPH_LOCAL_ALGORITHM_H
