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

#include "mcrl2/pbes/detail/stategraph_algorithm.h"
#include "mcrl2/pbes/detail/stategraph_dependency_graph.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

/// \brief Algorithm class for the local variant of the stategraph algorithm
class stategraph_local_algorithm: public stategraph_algorithm
{
  public:
    typedef stategraph_algorithm super;

  protected:
    // the control flow graph
    std::vector<control_flow_graph> m_control_flow_graphs;

    // m_control_flow_index[X][j] = k => d_X[j] corresponds to m_control_flow_graphs[k]
    std::map<core::identifier_string, std::map<std::size_t, std::size_t> > m_control_flow_index;

    // m_belongs[k] corresponds with m_control_flow_graphs[k]
    std::vector<std::map<core::identifier_string, std::set<data::variable> > > m_belongs;

    // the must relation
    dependency_graph must_graph;

    // the may relation
    dependency_graph may_graph;

    void compute_must_graph()
    {
      // create vertices
      const std::vector<stategraph_equation>& equations = m_pbes.equations();
      for (std::vector<stategraph_equation>::const_iterator k = equations.begin(); k != equations.end(); ++k)
      {
        core::identifier_string X = k->variable().name();
        const std::vector<data::variable>& d_X = k->parameters();
        const std::vector<bool>& cf = m_is_control_flow[X];
        for (std::size_t i = 0; i < d_X.size(); ++i)
        {
          if (cf[i])
          {
            must_graph.insert_vertex(X, i);
          }
        }
      }
      must_graph.set_index();

      // create edges
      for (std::vector<stategraph_equation>::const_iterator k = equations.begin(); k != equations.end(); ++k)
      {
        const stategraph_equation& eqn = *k;
        core::identifier_string X = eqn.variable().name();
        const std::vector<predicate_variable>& predvars = eqn.predicate_variables();
        for (std::size_t i = 0; i < predvars.size(); i++)
        {
          const std::map<std::size_t, std::size_t>& m = predvars[i].copy;
          for (std::map<std::size_t, std::size_t>::const_iterator j = m.begin(); j != m.end(); ++j)
          {
            std::size_t p = j->first;
            std::size_t l = j->second;
            const core::identifier_string& Y = predvars[i].X.name();
            must_graph.insert_edge(X, p, Y, l);
          }
        }
      }
    }

    void compute_may_graph()
    {
      // create vertices
      const std::vector<stategraph_equation>& equations = m_pbes.equations();
      for (std::vector<stategraph_equation>::const_iterator k = equations.begin(); k != equations.end(); ++k)
      {
        core::identifier_string X = k->variable().name();
        const std::vector<data::variable>& d_X = k->parameters();
        const std::vector<bool>& cf = m_is_control_flow[X];
        for (std::size_t i = 0; i < d_X.size(); ++i)
        {
          if (cf[i])
          {
            may_graph.insert_vertex(X, i);
          }
        }
      }
      may_graph.set_index();

      // create edges
      for (std::vector<stategraph_equation>::const_iterator q = equations.begin(); q != equations.end(); ++q)
      {
        const stategraph_equation& eqn = *q;
        core::identifier_string X = eqn.variable().name();
        const std::vector<predicate_variable>& predvars = q->predicate_variables();

        for (std::size_t i = 0; i < predvars.size(); i++)
        {
          const std::map<std::size_t, data::data_expression>& source = predvars[i].source;
          const std::map<std::size_t, data::data_expression>& dest = predvars[i].dest;
          const core::identifier_string& Y = predvars[i].X.name();

          std::vector<std::size_t> CFP = control_flow_parameter_indices(X);
          for (std::vector<std::size_t>::iterator j = CFP.begin(); j != CFP.end(); ++j)
          {
            dependency_vertex& u = must_graph.find_vertex(X, *j);
            if (u.outgoing_edges.empty())
            {
              for (std::map<std::size_t, data::data_expression>::const_iterator k = dest.begin(); k != dest.end(); ++k)
              {
                std::size_t l = k->first;
                may_graph.insert_edge(X, *j, Y, l);
              }
            }
          }
        }
      }
    }

    struct dependency_vertex_compare
    {
      const detail::dependency_vertex* source;
      const stategraph_pbes& p;

      dependency_vertex_compare(const stategraph_pbes& p_)
        : source(0),
          p(p_)
      {}

      bool operator()(const detail::dependency_vertex* u, const detail::dependency_vertex* v) const
      {
        if ((source->X == u->X) != (source->X == v->X))
        {
          return (source->X == u->X) > (source->X == v->X);
        }
        core::identifier_string s = find_equation(p, source->X)->parameters()[source->p].name();
        core::identifier_string u1 = find_equation(p, u->X)->parameters()[u->p].name();
        core::identifier_string v1 = find_equation(p, v->X)->parameters()[v->p].name();
        return (s == u1) > (s == v1);
      }
    };

    // projects X(e_1, ..., e_p ..., e_m) to X(e_p)
    propositional_variable_instantiation project(const propositional_variable_instantiation& x, std::size_t p) const
    {
      core::identifier_string X = x.name();
      const data::data_expression& e_p = nth_element(x.parameters(), p);
      data::data_expression_list e;
      e.push_front(e_p);
      return propositional_variable_instantiation(X, e);
    }

    // used for printing the control flow graphs
    std::map<core::identifier_string, data::variable_list> print_map(const core::identifier_string& X, std::size_t p)
    {
      std::map<core::identifier_string, data::variable_list> result;

      std::map<core::identifier_string, std::size_t> m = must_graph.dependency_map(X, p);
      for (std::map<core::identifier_string, std::size_t>::iterator i = m.begin(); i != m.end(); ++i)
      {
        core::identifier_string Y = i->first;
        std::size_t q = i->second;
        const stategraph_equation& eq_Y = *find_equation(m_pbes, Y);
        data::variable v = eq_Y.parameters()[q];
        result[Y] = atermpp::make_list(v);
      }
      return result;
    }

    void print_control_flow_graphs()
    {
      mCRL2log(log::debug, "stategraph") << "\n=== local control flow graphs ===" << std::endl;
      propositional_variable_instantiation X_init = m_pbes.initial_state();
      const core::identifier_string& X = X_init.name();
      const stategraph_equation& eq_X = *find_equation(m_pbes, X);
      const std::vector<data::variable>& d_X = eq_X.parameters();
      std::vector<std::size_t> CFP = control_flow_parameter_indices(X);
      for (std::size_t i = 0; i < CFP.size(); i++)
      {
        std::size_t p = CFP[i];
        mCRL2log(log::debug, "stategraph") << "--- graph for control flow parameter " << data::pp(d_X[p]) << " ---" << std::endl;
        mCRL2log(log::debug, "stategraph") << m_control_flow_graphs[i].print(print_map(X, p)) << std::endl;
      }
    }

    // @pre: p is a control flow parameter of X_init
    control_flow_graph compute_control_flow_graph(const propositional_variable_instantiation& X_init, std::size_t p)
    {
      typedef std::map<stategraph_vertex*, const dependency_vertex*> todo_map;
      control_flow_graph G;
      todo_map todo;

      // handle the initial state
      vertex_iterator i = G.insert_vertex(project(X_init, p));
      stategraph_vertex& u0 = i->second;
      const dependency_vertex& y0 = must_graph.find_vertex(X_init.name(), p);
      todo[&u0] = &y0;

      // compute sig(u) for marking algorithm
      pbes_system::simplifying_rewriter<pbes_expression, data::rewriter> pbesr(m_datar);
      data::data_expression e0 = u0.X.parameters().front();
      const stategraph_equation& eq_X = *find_equation(m_pbes, X_init.name());
      data::variable d0 = eq_X.parameters()[p];
      data::mutable_map_substitution<> sigma;
      sigma[d0] = e0;
      u0.sig = significant_variables(pbesr(eq_X.formula(), sigma));

      mCRL2log(log::debug, "stategraph") << "u0 = " << pbes_system::pp(u0.X) << std::endl;

      while (!todo.empty())
      {
        todo_map::iterator ti = todo.begin();
        stategraph_vertex& u = *(ti->first);
        const dependency_vertex& y = *(ti->second);
        todo.erase(ti);
        mCRL2log(log::debug, "stategraph") << "choose u = " << pbes_system::pp(u.X) << std::endl;

        // y = (X, p)
        const core::identifier_string& X = y.X;
        std::size_t p = y.p;
        assert(u.X.name() == X);
        data::data_expression e = u.X.parameters().front();
        const stategraph_equation& eq_X = *find_equation(m_pbes, X);

        if (y.outgoing_edges.empty())
        {
          continue;
        }

        const std::set<dependency_vertex*>& E = y.outgoing_edges;
        for (std::set<dependency_vertex*>::const_iterator ei = E.begin(); ei != E.end(); ++ei)
        {
          // z = (Y, q)
          const dependency_vertex& z = **ei;
          const core::identifier_string& Y = z.X;
          std::size_t q = z.p;

          for (std::size_t i = 0; i < eq_X.predicate_variables().size(); i++)
          {
            const predicate_variable& X_i = eq_X.predicate_variables()[i];
            if (X_i.X.name() != Y)
            {
              continue;
            }

            data::data_expression f;
            if (X_i.source.find(p) != X_i.source.end() && X_i.source.find(p)->second == e && X_i.dest.find(q) != X_i.dest.end())
            {
              f = X_i.dest.find(q)->second;
            }
            else if (X_i.source.find(p) == X_i.source.end() && X_i.dest.find(q) != X_i.dest.end() && X != Y)
            {
              f = X_i.dest.find(q)->second;
            }
            else if (X_i.source.find(p) == X_i.source.end() && X_i.copy.find(p) != X_i.copy.end() && X_i.copy.find(p)->second == q && X != Y)
            {
              f = e;
            }
            else
            {
              continue;
            }
            propositional_variable_instantiation Ye(Y, atermpp::make_list(f));
            mCRL2log(log::debug, "stategraph") << "v = " << pbes_system::pp(Ye) << std::endl;

            vertex_iterator vi = G.find(Ye);
            bool has_vertex = vi != G.end();
            if (!has_vertex)
            {
              mCRL2log(log::debug, "stategraph") << "insert vertex v" << std::endl;
              vi = G.insert_vertex(Ye);
              stategraph_vertex& v = vi->second;
              todo[&v] = &z;

              // compute sig(u) for marking algorithm
              data::data_expression e = v.X.parameters().front();
              const stategraph_equation& eq_vX = *find_equation(m_pbes, v.X.name());
              data::variable d = eq_vX.parameters()[q];
              data::mutable_map_substitution<> sigma;
              sigma[d] = e;
              v.sig = significant_variables(pbesr(eq_vX.formula(), sigma));
            }

            stategraph_vertex& v = vi->second;
            stategraph_edge edge(&u, &v, i);
            mCRL2log(log::debug, "stategraph") << "insert edge (u, v) with label " << i << std::endl;
            u.outgoing_edges.insert(edge);
            v.incoming_edges.insert(edge);
          }
        }
      }

      G.create_index();
      return G;
    }

    // Notation:
    // use u, v for vertices in the control flow graph
    // use y, z for vertices in the must graph
    void compute_control_flow_graphs()
    {
      mCRL2log(log::debug, "stategraph") << "=== computing local control flow graphs ===" << std::endl;
      propositional_variable_instantiation X_init = m_pbes.initial_state();
      std::vector<std::size_t> CFP = control_flow_parameter_indices(X_init.name());
      for (std::vector<std::size_t>::const_iterator k = CFP.begin(); k != CFP.end(); ++k)
      {
        std::size_t p = *k;
        mCRL2log(log::debug, "stategraph") << "compute local control flow graph for index " << p << std::endl;
        control_flow_graph G = compute_control_flow_graph(m_pbes.initial_state(), p);
        m_control_flow_graphs.push_back(G);
      }
      print_control_flow_graphs();
    }

    // we use the must_graph to determine this
    void compute_control_flow_index()
    {
      propositional_variable_instantiation X_init = m_pbes.initial_state();
      const core::identifier_string& X = X_init.name();
      std::vector<std::size_t> CFP = control_flow_parameter_indices(X_init.name());
      std::size_t K = m_control_flow_graphs.size();
      assert(K == CFP.size());
      for (std::size_t k = 0; k < K; k++)
      {
        std::map<core::identifier_string, std::size_t> m = must_graph.dependency_map(X, CFP[k]);
        for (std::map<core::identifier_string, std::size_t>::const_iterator i = m.begin(); i != m.end(); ++i)
        {
          m_control_flow_index[i->first][i->second] = k;
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
    std::map<core::identifier_string, std::set<data::variable> > compute_belongs(const core::identifier_string& X0, std::size_t p0, const control_flow_graph& Vk)
    {
      mCRL2log(log::debug, "stategraph") << "--- compute belongs (" << core::pp(X0) << ", " << p0 << ")" << std::endl;

      std::map<core::identifier_string, std::set<data::variable> > result;
      std::set<const dependency_vertex*> todo;
      std::set<const dependency_vertex*> visited;

      const dependency_vertex& y0 = must_graph.find_vertex(X0, p0);
      todo.insert(&y0);
      mCRL2log(log::debug, "stategraph") << " insert todo element (" << core::pp(X0) << ", " << p0 << ")" << std::endl;
      while (!todo.empty())
      {
        std::set<const dependency_vertex*>::iterator ti = todo.begin();

        // y = (X, p)
        const dependency_vertex& y = **ti;
        const core::identifier_string& X = y.X;
        std::size_t p = y.p;
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

        const std::set<dependency_vertex*>& E = y.outgoing_edges;
        for (std::set<dependency_vertex*>::const_iterator ei = E.begin(); ei != E.end(); ++ei)
        {
          // z = (Y, q)
          const dependency_vertex& z = **ei;
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
          out << core::pp(i->first) << " -> " << data::detail::print_set(i->second) << std::endl;
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
        if (!contains(m_belongs[k][X], d))
        {
          continue;
        }
        control_flow_graph& Gk = m_control_flow_graphs[k];
        const std::set<stategraph_vertex*>& inst = Gk.index(X);
        for (std::set<stategraph_vertex*>::const_iterator i = inst.begin(); i != inst.end(); ++i)
        {
          const stategraph_vertex& u = **i;
          if (contains(u.marking, d))
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
          << " marking = " << data::detail::print_set(u.marking)
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
                  mCRL2log(log::debug, "stategraph") << "update marking u with M = " << data::detail::print_set(M) << std::endl;
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
                if (!contains(Bk[Y], d_Y[l]))
                {
                  std::set<data::variable> M = set_difference(set_intersection(m_belongs[k][X], FV(nth_element(f, l))), u.marking);
                  if (M.empty())
                  {
                    continue;
                  }
                  if (is_marked_in_other_graph(Y, d_Y[l], k))
                  {
                    u.marking = set_union(u.marking, M);
                    mCRL2log(log::debug, "stategraph") << "update marking u with M = " << data::detail::print_set(M) << std::endl;
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
    stategraph_local_algorithm(const pbes<>& p, data::rewriter::strategy rewrite_strategy = data::jitty)
      : stategraph_algorithm(p, rewrite_strategy)
    { }

    /// \brief Computes the control flow graph
    void run()
    {
      super::run();
      compute_must_graph();
      mCRL2log(log::debug, "stategraph") << "=== must graph ===\n" << must_graph.print() << std::endl;
      compute_may_graph();
      mCRL2log(log::debug, "stategraph") << "=== may graph ===\n" << may_graph.print() << std::endl;
      bool result = remove_may_transitions(must_graph, may_graph, dependency_vertex_compare(m_pbes));
      if (!result)
      {
        throw mcrl2::runtime_error("error: could not find a must graph that satisfies all constraints!");
      }
      mCRL2log(log::debug, "stategraph") << "=== must graph after removing may transitions ===\n" << must_graph.print() << std::endl;
      compute_control_flow_graphs();
      compute_control_flow_index();
      print_control_flow_index();
      compute_belongs();
      print_belongs();
      compute_control_flow_marking();
      print_control_flow_marking();
    }
};

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_STATEGRAPH_LOCAL_ALGORITHM_H
