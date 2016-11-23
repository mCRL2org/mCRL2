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

#include "mcrl2/data/undefined.h"
#include "mcrl2/pbes/algorithms.h"
#include "mcrl2/pbes/detail/stategraph_algorithm.h"
#include "mcrl2/utilities/detail/container_utility.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

struct default_rules_predicate
{
  const local_control_flow_graph& V;

  default_rules_predicate(const local_control_flow_graph& V_)
    : V(V_)
  {}

  bool operator()(const core::identifier_string& X, std::size_t i) const
  {
    return V.has_label(X, i);
  }
};

class belongs_relation
{
  protected:
    typedef std::map<core::identifier_string, std::set<data::variable> > belongs_map;

    belongs_map m_belongs_map;

  public:
    const std::set<data::variable>& operator[](const core::identifier_string& X) const
    {
      auto i = m_belongs_map.find(X);
      if (i == m_belongs_map.end())
      {
        throw mcrl2::runtime_error("unknown key encountered in belongs_relation::operator[]");
      }
      return i->second;
    }

    belongs_map::iterator begin()
    {
      return m_belongs_map.begin();
    }

    belongs_map::const_iterator begin() const
    {
      return m_belongs_map.begin();
    }

    belongs_map::iterator end()
    {
      return m_belongs_map.end();
    }

    belongs_map::const_iterator end() const
    {
      return m_belongs_map.end();
    }

    belongs_map::iterator find(const core::identifier_string& X)
    {
      return m_belongs_map.find(X);
    }

    belongs_map::const_iterator find(const core::identifier_string& X) const
    {
      return m_belongs_map.find(X);
    }

    std::set<data::variable>& operator[](const core::identifier_string& X)
    {
      return m_belongs_map[X];
    }

    std::size_t size() const
    {
      return m_belongs_map.size();
    }

    std::string print() const
    {
      std::ostringstream out;
      out << "{";
      for (auto i = m_belongs_map.begin(); i != m_belongs_map.end(); ++i)
      {
        if (i != m_belongs_map.begin())
        {
          out << ", ";
        }
        out << i->first << " -> " << core::detail::print_set(i->second);
      }
      out << "}";
      return out.str();
    }
};

/// \brief Algorithm class for the local variant of the stategraph algorithm
class stategraph_local_algorithm: public stategraph_algorithm
{
  public:
    typedef stategraph_algorithm super;

  protected:
    // m_belongs[k] corresponds with m_control_flow_graphs[k]
    std::vector<belongs_relation> m_belongs;

    // if true, marking updates will be cached in the nodes of the local CFGs
    bool m_cache_marking_updates;

    std::size_t m_marking_rewrite_count;

    std::size_t m_marking_rewrite_cached_count;

    std::vector<core::identifier_string> binding_variable_names() const
    {
      std::vector<core::identifier_string> result;
      auto const& equations = m_pbes.equations();
      for (const stategraph_equation& equation: equations)
      {
        result.push_back(equation.variable().name());
      }
      return result;
    }

    struct vertex_pair
    {
      const local_control_flow_graph_vertex* u;
      const local_control_flow_graph_vertex* v;
      std::size_t k; // index of the graph

      vertex_pair(const local_control_flow_graph_vertex* u_, const local_control_flow_graph_vertex* v_, std::size_t k_)
        : u(u_), v(v_), k(k_)
      {}

      bool operator<(const vertex_pair& other) const
      {
        return u < other.u || (u == other.u && v < other.v);
      }
    };

    struct equation_label_pair
    {
      const core::identifier_string X;
      std::size_t i; // label

      equation_label_pair(const core::identifier_string& X_, std::size_t i_)
        : X(X_), i(i_)
      {}

      bool operator<(const equation_label_pair& other) const
      {
        return X < other.X || (X == other.X && i < other.i);
      }
    };

    // maps (X, i) to the set of all edges with label i and source vertex with name X
    std::map<core::identifier_string, std::map<std::size_t, std::set<vertex_pair> > > m_edge_index;

    void compute_edge_index()
    {
      for (std::size_t k = 0; k < m_local_control_flow_graphs.size(); k++)
      {
        auto const& Gk = m_local_control_flow_graphs[k];
        auto const& vertices = Gk.vertices;
        for (const auto& u: vertices)
        {
          auto const& X = u.name();
          auto const& outgoing_edges = u.outgoing_edges();
          for(const auto& e: outgoing_edges)
          {
            auto const& v = *(e.first);
            auto const& I = e.second;
            for (std::size_t i: I)
            {
              m_edge_index[X][i].insert(vertex_pair(&u, &v, k));
            }
          }
        }
      }
    }

    std::string print_edge_index()
    {
      mCRL2log(log::debug, "stategraph") << "--- computed marking edge index ---" << std::endl;
      auto const& equations = m_pbes.equations();
      std::ostringstream out;

      for (const stategraph_equation& eq_X: equations)
      {
        auto const& X = eq_X.variable().name();
        auto& EX = m_edge_index[X];
        out << "index for equation " << X << std::endl;
        for (std::size_t i = 0; i < eq_X.predicate_variables().size(); i++)
        {
          auto& EXi = EX[i];
          for (const auto& ei: EXi)
          {
            out << " edge " << *ei.u << " --" << i << "--> " << *ei.v << std::endl;
          }
        }
      }
      return out.str();
    }

    // prints a belong set
    std::string print_belong_set(const stategraph_equation& eq, const std::set<std::size_t>& belongs) const
    {
      std::set<data::variable> v;
      for (std::size_t i: belongs)
      {
        v.insert(eq.parameters()[i]);
      }
      return core::detail::print_set(v);
    }

    // prints a subset of parameters of the equation corresponding to X
    // I is a set of indices of parameters of the equation corresponding to X
    std::string print_parameters(const core::identifier_string& X, const std::set<std::size_t>& I) const
    {
      auto const& eq_X = *find_equation(m_pbes, X);
      auto const& param = eq_X.parameters();
      std::ostringstream out;
      out << "{";
      for (auto i = I.begin(); i != I.end(); ++i)
      {
        if (i != I.begin())
        {
          out << ", ";
        }
        out << "(" << *i << ", " << param[*i] << ")";
      }
      out << "}";
      return out.str();
    }

    template <typename RulesPredicate>
    belongs_relation compute_belongs(const local_control_flow_graph& Vk, RulesPredicate rules)
    {
      using utilities::detail::contains;

      std::set<core::identifier_string> Nk; // Nk contains the names of the vertices in Vk
      for (const auto& v: Vk.vertices)
      {
        Nk.insert(v.name());
      }

      belongs_relation Bk;
      auto const& equations = m_pbes.equations();
      for (const stategraph_equation& eqn: equations)
      {
        auto const& X = eqn.variable().name();
        if (!contains(Nk, X))
        {
          continue;
        }
        Bk[X]; // force the creation of an empty set corresponding to X
        auto const& eq_X = *find_equation(m_pbes, X);
        auto const& dpX = eq_X.data_parameter_indices();
        std::set<std::size_t> belongs(dpX.begin(), dpX.end());
        mCRL2log(log::debug1, "stategraph") << "  initial belong set for equation " << X << " = " << print_belong_set(eq_X, belongs) << std::endl;

        auto const& predvars = eq_X.predicate_variables();
        for (std::size_t i = 0; i < predvars.size(); i++)
        {
          auto const& Ye = predvars[i];
          for (auto j = belongs.begin(); j != belongs.end(); )
          {
            std::size_t m = *j;
            if ((contains(Ye.used(), m) || contains(Ye.changed(), m)) && !rules(X, i))
            {
              mCRL2log(log::debug1, "stategraph") << " remove (X, i, m) = (" << X << ", " << i << ", " << m << ") variable=" << eq_X.parameters()[m] << " from belongs " << std::endl;
              mCRL2log(log::debug2, "stategraph") << "  used = " << print_parameters(Ye.name(), Ye.used()) << " changed = " << print_parameters(Ye.name(), Ye.changed()) << std::endl;
              belongs.erase(j++);
            }
            else
            {
              ++j;
            }
          }
        }
        mCRL2log(log::debug1, "stategraph") << "  final   belong set for equation " << X << " = " << print_belong_set(eq_X, belongs) << std::endl;
        for (std::size_t i: belongs)
        {
          Bk[X].insert(eq_X.parameters()[i]);
        }
      }

      return Bk;
    }

    void compute_belongs()
    {
      assert(m_local_control_flow_graphs.size() == m_connected_components_values.size());
      mCRL2log(log::debug, "stategraph") << "=== computing belongs relation ===" << std::endl;
      for (std::size_t k = 0; k < m_local_control_flow_graphs.size(); k++)
      {
        mCRL2log(log::debug, "stategraph") << "--- compute belongs for graph " << k << " ---" << std::endl;
        auto const& Vk = m_local_control_flow_graphs[k];
        belongs_relation Bk = compute_belongs(Vk, default_rules_predicate(Vk));
        m_belongs.push_back(Bk);
      }
    }

    std::string print_belongs(const belongs_relation& B) const
    {
      std::ostringstream out;
      for (const auto& b: B)
      {
        out << b.first << " -> " << core::detail::print_set(b.second) << std::endl;
      }
      return out.str();
    }

    void print_belongs() const
    {
      std::ostringstream out;
      out << "\n=== belongs relation ===\n";
      for (std::size_t k = 0; k < m_belongs.size(); k++)
      {
        out << "--- belongs relation for graph " << k << " ---" << std::endl;
        auto const& Bk = m_belongs[k];
        out << print_belongs(Bk);
      }
      mCRL2log(log::debug, "stategraph") << out.str() << std::endl;
    }

    std::set<data::variable> significant_variables(const local_control_flow_graph_vertex& u) const
    {
      const core::identifier_string& X = u.name();
      const pbes_equation& eq_X = *find_equation(m_pbes, X);
      pbes_expression phi = eq_X.formula();
      if (u.index() != data::undefined_index())
      {
        data::rewriter::substitution_type sigma;
        sigma[u.variable()] = u.value();
        pbes_system::simplify_data_rewriter<data::rewriter> pbesr(m_datar);
        phi = pbesr(phi, sigma);
      }
      return pbes_system::algorithms::significant_variables(phi);
    }

    // returns the index of variable d in the vector d_Y
    std::size_t find_parameter_index(const std::vector<data::variable>& d_Y, const data::variable& d) const
    {
      auto i = std::find(d_Y.begin(), d_Y.end(), d);
      if (i == d_Y.end())
      {
        throw mcrl2::runtime_error("find_parameter_index: variable not found!");
      }
      return i - d_Y.begin();
    }

    bool belongs_contains(const belongs_relation& B,
                          const core::identifier_string& X,
                          const data::variable& d
                         ) const
    {
      auto i = B.find(X);
      if (i == B.end())
      {
        return false;
      }
      return utilities::detail::contains(i->second, d);
    }

    // returns the intersection of V with { d | (X, d) in B }
    std::set<data::variable> belongs_intersection(const std::set<data::variable>& V,
                                                  const belongs_relation& B,
                                                  const core::identifier_string& X
                                                 )
    {
      auto i = B.find(X);
      if (i == B.end())
      {
        return std::set<data::variable>();
      }
      return utilities::detail::set_intersection(V, i->second);
    }

    template <typename Substitution>
    data::data_expression rewr(const data::data_expression& x, Substitution sigma)
    {
      return m_datar(x, sigma);
    }

    std::set<data::variable> marking_update(const local_control_flow_graph_vertex& u,
                                            std::size_t i,
                                            const data::variable& d,
                                            const data::data_expression_list& e,
                                            const stategraph_equation& eq_Y,
                                            const belongs_relation& B)
    {
      if (m_cache_marking_updates)
      {
        // check if the update is cached in u
        std::pair<std::size_t, data::variable> p(i, d);
        auto const& marking_update = u.marking_update();
        auto j = marking_update.find(p);
        if (j != marking_update.end())
        {
          m_marking_rewrite_count++;
          m_marking_rewrite_cached_count++;
          return j->second;
        }
      }

      // compute the value
      auto const& X = u.name();
      std::size_t l = find_parameter_index(eq_Y.parameters(), d);
      data::rewriter::substitution_type sigma;
      sigma[u.variable()] = u.value();
      auto W = FV(rewr(nth_element(e, l), sigma));
      std::set<data::variable> V = belongs_intersection(W, B, X);
      if (m_cache_marking_updates)
      {
        u.set_marking_update(i, d, V);
      }
      m_marking_rewrite_count++;
      return V;
    }

    void print_marking_statistics()
    {
      mCRL2log(log::verbose, "stategraph") << "--- marking statistics: " << m_marking_rewrite_count << " rewrite calls, from which " << m_marking_rewrite_cached_count << " were cached" << std::endl;
    }

    // updates u.marking
    // returns true if u.marking has changed
    bool update_marking_rule(const belongs_relation& B,
                             const local_control_flow_graph_vertex& u,
                             std::size_t i,
                             const local_control_flow_graph_vertex& v,
                             bool check_belongs // false corresponds with rule1, true corresponds with rule2
                            )
    {
      std::size_t size = u.marking().size();
      auto const& X = u.name();
      auto const& eq_X = *find_equation(m_pbes, X);
      auto const& Y = v.name();
      auto const& eq_Y = *find_equation(m_pbes, Y);
      auto const& Ye = eq_X.predicate_variables()[i];
      auto const& e = Ye.parameters();
      auto m = v.marking(); // N.B. a copy must be made, to handle the case u == v properly
      for (const data::variable& d: m)
      {
        if (check_belongs && belongs_contains(B, Y, d))
        {
          continue;
        }
        auto const update = marking_update(u, i, d, e, eq_Y, B);
        u.extend_marking(update);
      }
      return u.marking().size() != size;
    }

    // returns true if there is a vertex u in V and an edge (u, i, v) in V, such that u.name() == X
    bool has_incoming_edge(const local_control_flow_graph& /* V */, const local_control_flow_graph_vertex& v, const core::identifier_string& X, std::size_t i) const
    {
      using utilities::detail::contains;

      auto const& incoming_edges = v.incoming_edges();
      for(const auto& e: incoming_edges)
      {
        auto const& u = *e.first;
        auto const& I = e.second;
        if (u.name() == X && contains(I, i))
        {
          return true;
        }
      }
      return false;
    }

    void compute_control_flow_marking()
    {
      mCRL2log(log::debug, "stategraph") << "=== computing control flow marking ===" << std::endl;
      using utilities::detail::pick_element;

      start_timer("marking initialization");
      std::size_t J = m_local_control_flow_graphs.size();
      for (std::size_t j = 0; j < J; j++)
      {
        auto const& Vj = m_local_control_flow_graphs[j];
        auto const& Bj = m_belongs[j];
        for (const auto& u : Vj.vertices)
        {
          auto const& X = u.name();
          u.set_marking(belongs_intersection(significant_variables(u), Bj, X));
        }
        mCRL2log(log::debug, "stategraph") << "--- initial control flow marking for graph " << j << "\n" << Vj.print_marking();
      }
      finish_timer("marking initialization");

      start_timer("marking computation");
      bool stable = false;
      while (!stable)
      {
        bool stableint = false;
        while (!stableint)
        {
          stableint = true;
          for (std::size_t j = 0; j < J; j++)
          {
            auto const& Vj = m_local_control_flow_graphs[j];
            auto const& Bj = m_belongs[j];
            std::set<const local_control_flow_graph_vertex*> todo;
            for (const auto& v: Vj.vertices)
            {
              todo.insert(&v);
            }
            while (!todo.empty())
            {
              auto const& v = *pick_element(todo);

              mCRL2log(log::debug1, "stategraph") << " extend marking rule1: v = " << v << " marking(v) = " << core::detail::print_set(v.marking()) << std::endl;

              auto const& incoming_edges = v.incoming_edges();
              for (const auto& e: incoming_edges)
              {
                bool changed = false;
                auto const& u = *e.first;
                auto const& labels = e.second;
                for (std::size_t i: labels)
                {
                    // consider edge (u, i, v)
                    bool updated = update_marking_rule(Bj, u, i, v, false);
                    changed = changed || updated;
                }
                if (changed)
                {
                  mCRL2log(log::debug1, "stategraph") << "   marking(u)' = " << core::detail::print_set(u.marking()) << std::endl;
                  todo.insert(&u);
                  stableint = false;
                }
              }
            }
          }
        }

        bool stableext = false;
        while (!stableext)
        {
          stableext = true;
          for (std::size_t j = 0; j < J; j++)
          {
            auto const& Vj = m_local_control_flow_graphs[j];
            auto const& Bj = m_belongs[j];
            for (const auto &u : Vj.vertices)
            {
              auto const& X = u.name();
              if (u.marking().size() == Bj[X].size())
              {
                continue;
              }
              mCRL2log(log::debug1, "stategraph") << " extend marking rule2: u = " << u << " marking(u) = " << core::detail::print_set(u.marking()) << std::endl;

              bool changed = false;
              auto const& eq_X = *find_equation(m_pbes, X); // slow
              auto const& predvars = eq_X.predicate_variables();
              auto const& outgoing_edges = u.outgoing_edges();
              for (const auto& e: outgoing_edges)
              {
                auto const& labels = e.second;
                for (std::size_t i: labels)
                {
                  auto const& Ye = predvars[i];
                  auto const& Y = Ye.name();
                  for (std::size_t k = 0; k < J; k++)
                  {
                    if (j == k)
                    {
                      continue;
                    }
                    auto const& Vk = m_local_control_flow_graphs[k];
                    for (auto vk = Vk.vertices.begin(); vk != Vk.vertices.end(); ++vk)
                    {
                      auto const& v = *vk;
                      mCRL2log(log::debug1, "stategraph") << "     v = " << v << " marking(v) = " << core::detail::print_set(v.marking()) << std::endl;
                      if (v.name() != Y)
                      {
                        continue;
                      }
                      if (has_incoming_edge(Vk, v, X, i))
                      {
                        bool updated = update_marking_rule(Bj, u, i, v, true);
                        changed = changed || updated;
                      }
                    }
                  }
                }
              }
              if (changed)
              {
                mCRL2log(log::debug1, "stategraph") << "   marking(u)' = " << core::detail::print_set(u.marking()) << std::endl;
                stableint = false;
                stableext = false;
              }
            }
          }
        }
        stable = stableint;
      }
      finish_timer("marking computation");
    }

    // add (X, i) to todo for each incoming edge u = (X, n, dX[n] = z) --i--> v
    void add_equation_labels(std::set<equation_label_pair>& todo, const local_control_flow_graph_vertex& v)
    {
      auto const& incoming_edges = v.incoming_edges();
      for (const auto& e: incoming_edges)
      {
        auto const& u = *e.first;
        auto const& X = u.name();
        auto const& labels = e.second;
        for (std::size_t i: labels)
        {
          todo.insert(equation_label_pair(X, i));
        }
      }
    }

    // a more comprehensible version
    void compute_control_flow_marking_using_edge_index()
    {
      mCRL2log(log::debug, "stategraph") << "=== computing control flow marking ===" << std::endl;
      using utilities::detail::pick_element;

      start_timer("marking initialization");
      std::size_t J = m_local_control_flow_graphs.size();
      std::set<equation_label_pair> todo;
      for (std::size_t j = 0; j < J; j++)
      {
        auto const& Vj = m_local_control_flow_graphs[j];
        auto const& Bj = m_belongs[j];
        for (const auto& v: Vj.vertices)
        {
          auto const& Y = v.name();
          v.set_marking(belongs_intersection(significant_variables(v), Bj, Y));
          // Insert those pairs (X,i) for which some vertex u = (X,n,dX[n]=z) --i--> v and v.marking() != {}
          if (v.marking().empty())
          {
            continue;
          }
          auto const& incoming_edges = v.incoming_edges();
          for (const auto& e: incoming_edges)
          {
            auto const& u = *e.first;
            auto const& X = u.name();
            auto const& labels = e.second;
            for (std::size_t i: labels)
            {
              todo.insert(equation_label_pair(X, i));
            }
          }
       }
        mCRL2log(log::debug, "stategraph") << "--- initial control flow marking for graph " << j << "\n" << Vj.print_marking();
      }
      finish_timer("marking initialization");

      start_timer("marking computation");
      while (!todo.empty())
      {
        auto const& Xi = pick_element(todo);
        auto const& X = Xi.X;
        std::size_t i = Xi.i;

        mCRL2log(log::debug1, "stategraph") << "    rule: considering equation " << X << std::endl;
        mCRL2log(log::debug1, "stategraph") << "    rule2: considering PVI nr. " << i << std::endl;

        auto& EX = m_edge_index[X];
        auto& EXi = EX[i];
        for (auto ei = EXi.begin(); ei != EXi.end(); ++ei)
        {
          const local_control_flow_graph_vertex& u = *ei->u;
          mCRL2log(log::debug1, "stategraph") << " extend marking rule2: u = " << u << " marking(u) = " << core::detail::print_set(u.marking()) << std::endl;
          std::size_t j = ei->k;
          auto const& Bj = m_belongs[j];
          for (const auto& ej: EXi)
          {
            const local_control_flow_graph_vertex& u1 = *ej.u;
            const local_control_flow_graph_vertex& v1 = *ej.v;
            std::size_t k = ej.k;
            auto const& Bk = m_belongs[k];
            bool updated = update_marking_rule(Bk, u1, i, v1, false);
            if (updated)
            {
              add_equation_labels(todo, u1);
            }
            if (j != k  )
            {
              updated = update_marking_rule(Bj, u, i, v1, true);
              if (updated)
              {
                add_equation_labels(todo, u);
              }
            }
          }
        }
      }
      finish_timer("marking computation");
    }

    // an efficient version that uses an edge index
    void compute_control_flow_marking_efficient()
    {
      mCRL2log(log::debug, "stategraph") << "=== computing control flow marking ===" << std::endl;
      using utilities::detail::pick_element;

      start_timer("marking initialization");
      std::size_t J = m_local_control_flow_graphs.size();
      for (std::size_t j = 0; j < J; j++)
      {
        auto const& Vj = m_local_control_flow_graphs[j];
        auto const& Bj = m_belongs[j];
        for (const auto& u: Vj.vertices)
        {
          auto const& X = u.name();
          u.set_marking(belongs_intersection(significant_variables(u), Bj, X));
        }
        mCRL2log(log::debug, "stategraph") << "--- initial control flow marking for graph " << j << "\n" << Vj.print_marking();
      }
      finish_timer("marking initialization");

      start_timer("marking computation");
      bool stable = false;
      while (!stable)
      {
        bool stableint = false;
        while (!stableint)
        {
          stableint = true;
          for (std::size_t j = 0; j < J; j++)
          {
            auto const& Vj = m_local_control_flow_graphs[j];
            auto const& Bj = m_belongs[j];
            std::set<const local_control_flow_graph_vertex*> todo;
            for (const auto& v: Vj.vertices)
            {
              todo.insert(&v);
            }
            while (!todo.empty())
            {
              auto const& v = *pick_element(todo);

              mCRL2log(log::debug1, "stategraph") << " extend marking rule1: v = " << v << " marking(v) = " << core::detail::print_set(v.marking()) << std::endl;

              auto const& incoming_edges = v.incoming_edges();
              for (const auto& e: incoming_edges)
              {
                bool changed = false;
                auto const& u = *e.first;
                auto const& labels = e.second;
                for (std::size_t i: labels)
                {
                    // consider edge (u, i, v)
                    bool updated = update_marking_rule(Bj, u, i, v, false);
                    changed = changed || updated;
                }
                if (changed)
                {
                  mCRL2log(log::debug1, "stategraph") << "   marking(u)' = " << core::detail::print_set(u.marking()) << std::endl;
                  todo.insert(&u);
                  stableint = false;
                }
              }
            }
          }
        }

        // stableext := false
        // while not stableext do
        //   stableext := true;
        //   for X in bnd(E) do
        //     for i <= npred(phi_X) do
        //       let E := { ( u, i, v) in E1 union ... union EJ | exists n,z: u = (X,n,dX[n]=z) };
        //       for (u,i,v) in E do
        //         let j be such that u in Vj;
        //         if (marking(u) = {d | (X,d) in Bj}) then continue;
        //         for (u',i,v') in E do
        //           let k be such that u' in Vk;
        //           if (j == k) then continue;
        //           m := marking(u);
        //           marking(u) := UpdateMarkingRule2(Bj,u,i,v')
        //           if not(marking(u) == m) then
        //             stableint := false;
        //             stableext := false;
        // stable := stableint /\ stableext;
        bool stableext = false;
        auto const& equations = m_pbes.equations();
        while (!stableext)
        {
          stableext = true;
          for (const auto& eq_X: equations)
          {
            auto const& X = eq_X.variable().name();
            auto& EX = m_edge_index[X];
            for (std::size_t i = 0; i < eq_X.predicate_variables().size(); i++)
            {
              auto& EXi = EX[i];
              for (auto ei = EXi.begin(); ei != EXi.end(); ++ei)
              {
                const local_control_flow_graph_vertex& u = *ei->u;
                // const local_control_flow_graph_vertex& v = *ei->v;
                std::size_t j = ei->k;
                auto const& Bj = m_belongs[j];
                if (u.marking().size() == Bj[X].size())
                {
                  continue;
                }
                for (const auto& ej: EXi)
                {
                  // const local_control_flow_graph_vertex& u1 = *ej->u;
                  const local_control_flow_graph_vertex& v1 = *ej.v;
                  std::size_t k = ej.k;
                  if (j == k)
                  {
                    continue;
                  }
                  bool updated = update_marking_rule(Bj, u, i, v1, true);
                  if (updated)
                  {
                    stableint = false;
                    stableext = false;
                  }
                }
              }
            }
          }
        }
        stable = stableint;
      }
      finish_timer("marking computation");
    }

    void print_control_flow_marking() const
    {
      std::size_t K = m_local_control_flow_graphs.size();
      for (std::size_t k = 0; k < K; k++)
      {
        const local_control_flow_graph& Gk = m_local_control_flow_graphs[k];
        mCRL2log(log::debug, "stategraph") <<  "--- computed control flow marking for graph " << k << "\n" << Gk.print_marking() << std::endl;
      }
    }

    void remove_belongs(std::set<data::variable>& B_X,
                        const core::identifier_string& X,
                        const belongs_relation& Bk,
                        std::size_t k)
    {
      for (const auto& i: Bk)
      {
        if (i.first != X)
        {
          continue;
        }
        auto const& V = i.second;
        for (const data::variable& v: V)
        {
          auto j = B_X.find(v);
          if (j != B_X.end())
          {
            mCRL2log(log::debug, "stategraph") <<  "removing belongs variable " << *j << " in B[" << X << "] , since it already appears in belongs relation " << k << std::endl;
            B_X.erase(*j);
          }
        }
      }
    }

    void compute_extra_local_control_flow_graph()
    {
      mCRL2log(log::debug1, "stategraph") << "--- computing extra local control flow graph" << std::endl;

      local_control_flow_graph V;
      belongs_relation B;

      auto const& equations = m_pbes.equations();
      for (const auto& eq_X: equations)
      {
        auto const& X = eq_X.variable().name();
        auto const& u = V.insert_vertex(local_control_flow_graph_vertex(X, data::undefined_data_expression()));

        auto const& d_X = eq_X.parameters();
        auto& B_X = B[X];  // N.B. This forces the creation of an empty belong set B[X].
        auto const& I = eq_X.data_parameter_indices();
        for (std::size_t i : I)
        {
          B_X.insert(d_X[i]);
        }
        mCRL2log(log::debug1, "stategraph") << "initial belongs relation B[" << X << "] = " << core::detail::print_set(B_X) << std::endl;

        for (std::size_t k = 0; k < m_belongs.size(); k++)
        {
          auto const& Bk = m_belongs[k];
          remove_belongs(B_X, X, Bk, k);
        }

        auto const& predvars = eq_X.predicate_variables();
        for (std::size_t i = 0; i < predvars.size(); i++)
        {
          auto const& Ye = predvars[i];

          bool add_edge = false;

          //if X != Y || exists k: d_X[k] \in B && (used(X, j, k) || changed(X, j, k)) then
          // implemented by checking for non-empty intersection
          if(X != Ye.name())
          {
            add_edge = true;
          }
          else
          {
            std::set<data::variable> used_or_changed;
            for (std::size_t j: Ye.changed())
            {
              used_or_changed.insert(d_X[j]);
            }
            for (std::size_t j: Ye.used())
            {
              used_or_changed.insert(d_X[j]);
            }

            std::set<data::variable>::const_iterator bi = B_X.begin();
            std::set<data::variable>::const_iterator ui = used_or_changed.begin();
            while (bi != B_X.end() && ui != used_or_changed.end())
            {
              if (*ui < *bi)
              {
                ++ui;
              }
              else if (*bi < *ui)
              {
                ++bi;
              }
              else
              {
                add_edge = true;
                break;
              }
            }
          }

          if(add_edge)
          {
            auto const& v = V.insert_vertex(local_control_flow_graph_vertex(Ye.name(), data::undefined_data_expression()));
            V.insert_edge(u, i, v);
          }
        }
      }
      m_local_control_flow_graphs.push_back(V);
      m_local_control_flow_graphs.back().compute_index();
      m_belongs.push_back(B);
      mCRL2log(log::debug, "stategraph") << "--- extra local control flow graph\n" << m_local_control_flow_graphs.back() << std::endl;
      mCRL2log(log::debug, "stategraph") << "--- belongs relation for extra graph\n" << print_belongs(B);
    }

    void compute_local_control_flow_graph(const std::set<local_control_flow_graph_vertex>& U, const std::map<core::identifier_string, std::size_t>& C)
    {
      using utilities::detail::pick_element;

      mCRL2log(log::debug, "stategraph") << "--- compute_local_control_flow_graph for vertices " << core::detail::print_set(U) << std::endl;
      local_control_flow_graph V;
      std::set<const local_control_flow_graph_vertex*> todo;

      for (const local_control_flow_graph_vertex& u: U)
      {
        auto k = V.insert(u);
        todo.insert(&(*k.first));
      }

      while (!todo.empty())
      {
        // u = (X, k, d = e)
        auto const& u = *pick_element(todo);
        auto const& X = u.name();
        const data::variable& d = u.variable();
        const data::data_expression& e = u.value();
        mCRL2log(log::debug1, "stategraph") << "choose todo element (X, k, d=e) = " << u << std::endl;
        std::size_t k = u.index();
        auto const& eq_X = *find_equation(m_pbes, X);
        auto const& predvars = eq_X.predicate_variables();

        for (auto i = predvars.begin(); i != predvars.end(); ++i)
        {
          std::size_t edge_label = i - predvars.begin();
          auto const& Y = i->name();
          auto q = C.find(Y);

          if (d == data::undefined_variable())
          {
            // case 1: (X, ?, ?=?) -> (Y, k', d'=e')
            mCRL2log(log::debug1, "stategraph") << "case 1" << std::endl;
            if (q != C.end()) // (Y, k1) in C
            {
              std::size_t k1 = q->second;
              auto e1 = i->dest(k1);
              if (e1 != data::undefined_data_expression())
              {
                // dest(X, i, k') = e'
                mCRL2log(log::debug1, "stategraph") << "case 1: (X, e) -> (Y, d', e') ; dest(X, i, k') = e' ; k' = " << print_index(k1) << std::endl;
                V.insert_edge(todo, m_pbes, u, Y, k1, e1, edge_label);
              }
            }
            // case 2: (X, ?, ?=?) -> (Y, ?, ?=?)
            else
            {
              if (X != Y)
              {
                mCRL2log(log::debug1, "stategraph") << "case 2: (X, ?) -> (Y, ?)" << std::endl;
                V.insert_edge(todo, m_pbes, u, Y, data::undefined_index(), data::undefined_data_expression(), edge_label);
              }
            }
          }
          else
          {
            // case 3: (X, d, e) -> (Y, d', e')
            if (q != C.end()) // (Y, k') in C
            {
              std::size_t k1 = q->second;
              if (is_mapped_to(i->source(), k, e))
              {
                // source(X, i, k) = e && dest(X, i, k') = e'
                auto e1 = i->dest(k1);
                mCRL2log(log::debug1, "stategraph") << "case 3a: (X, d, e) -> (Y, d', e') ; source(X, i, k) = e && dest(X, i, k') = e' ; k' = " << print_index(k1) << std::endl;
                if (e1 != data::undefined_data_expression())
                {
                  V.insert_edge(todo, m_pbes, u, Y, k1, e1, edge_label);
                }
              }
              else if (Y != X && is_undefined(i->source(), k))
              {
                // Y != X && undefined(source(X, i, k)) && dest(X, i, k') = e'
                auto e1 = i->dest(k1);
                if (e1 != data::undefined_data_expression())
                {
                  mCRL2log(log::debug1, "stategraph") << "case 3b: (X, d, e) -> (Y, d', e') ; Y != X && undefined(source(X, i, k)) && dest(X, i, k') = e' ; k' = " << print_index(k1) << std::endl;
                  V.insert_edge(todo, m_pbes, u, Y, k1, e1, edge_label);
                }

                // Y != X && undefined(source(X, i, k)) && copy(X, i, k) = k'
                if (i->copy(k) == k1)
                {
                  mCRL2log(log::debug1, "stategraph") << "case 3c: (X, d, e) -> (Y, d', e') ; Y != X && undefined(source(X, i, k)) && copy(X, i, k) = k' ; k' = " << print_index(k1) << std::endl;
                  V.insert_edge(todo, m_pbes, u, Y, k1, e, edge_label);
                }
              }
            }
            // case 4: (X, d, e) -> (Y, ?)
            else
            {
              auto e1 = i->source(k);
              if (e1 == e || e1 == data::undefined_data_expression())
              {
                std::size_t k1 = data::undefined_index();
                mCRL2log(log::debug1, "stategraph") << "case 4: (X, d, e) -> (Y, ?)" << std::endl;
                V.insert_edge(todo, m_pbes, u, Y, k1, data::undefined_data_expression(), edge_label);
              }
            }
          }
        }
      }
      // V.self_check();
      m_local_control_flow_graphs.push_back(V);
      m_local_control_flow_graphs.back().compute_index();
    }

    void compute_local_control_flow_graph(const local_control_flow_graph_vertex& u, const std::map<core::identifier_string, std::size_t>& C)
    {
      std::set<local_control_flow_graph_vertex> U;
      U.insert(u);
      compute_local_control_flow_graph(U, C);
    }

    // Computes a local control flow graph that corresponds to the given component in m_GCFP_graph.
    void compute_local_control_flow_graph(const std::set<std::size_t>& component, const std::set<data::data_expression>& component_values)
    {
      mCRL2log(log::debug, "stategraph") << "Compute local control flow graphs for component " << print_connected_component(component) << std::endl;

      // preprocessing
      std::map<core::identifier_string, std::size_t> C;
      for (std::size_t p: component)
      {
        const GCFP_vertex& w = m_GCFP_graph.vertex(p);
        C[w.name()] = w.index();
      }

      std::set<local_control_flow_graph_vertex> U;
      for (std::size_t p: component)
      {
        const GCFP_vertex& u = m_GCFP_graph.vertex(p);
        for (const auto& value: component_values)
        {
          U.insert(local_control_flow_graph_vertex(u.name(), u.index(), u.variable(), value));
        }
      }

      // add a node (Xinit, ?, ?=?)
      const core::identifier_string& Xinit = m_pbes.initial_state().name();
      if (C.find(Xinit) == C.end())
      {
        U.insert(local_control_flow_graph_vertex(Xinit, data::undefined_index(), data::undefined_variable(), data::undefined_data_expression()));
      }

      compute_local_control_flow_graph(U, C);
    }

    void print_local_control_flow_graphs() const
    {
      for (auto i = m_local_control_flow_graphs.begin(); i != m_local_control_flow_graphs.end(); ++i)
      {
        mCRL2log(log::debug, "stategraph") << "--- computed local control flow graph " << (i - m_local_control_flow_graphs.begin()) << "\n" << *i << std::endl;
      }
    }

    void compute_local_control_flow_graphs()
    {
      for (std::size_t i = 0; i < m_connected_components.size(); i++)
      {
        compute_local_control_flow_graph(m_connected_components[i], m_connected_components_values[i]);
      }
    }

  public:
    stategraph_local_algorithm(const pbes& p, const pbesstategraph_options& options)
      : stategraph_algorithm(p, options),
        m_cache_marking_updates(options.cache_marking_updates),
        m_marking_rewrite_count(0),
        m_marking_rewrite_cached_count(0)

    { }

    /// \brief Computes the control flow graph
    void run()
    {
      super::run();

      start_timer("compute_local_control_flow_graphs");
      compute_local_control_flow_graphs();
      finish_timer("compute_local_control_flow_graphs");
      print_local_control_flow_graphs();

      start_timer("compute_belongs");
      compute_belongs();
      finish_timer("compute_belongs");
      print_belongs();

      start_timer("compute_extra_local_control_flow_graph");
      compute_extra_local_control_flow_graph();
      finish_timer("compute_extra_local_control_flow_graph");

      start_timer("compute_control_flow_marking");
      switch (m_options.marking_algorithm)
      {
        case 0: {
          compute_control_flow_marking();
          break;
        }
        case 1: {
          compute_edge_index();
          mCRL2log(log::debug2, "stategraph") << print_edge_index() << std::endl;
          compute_control_flow_marking_using_edge_index();
          break;
        }
        case 2: {
          compute_edge_index();
          mCRL2log(log::debug2, "stategraph") << print_edge_index() << std::endl;
          compute_control_flow_marking_efficient();
          break;
        }
        default: {
          throw mcrl2::runtime_error("unknown value of marking_algorithm");
        }
      }
      finish_timer("compute_control_flow_marking");
      print_control_flow_marking();

      print_marking_statistics();
    }
};

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_STATEGRAPH_LOCAL_ALGORITHM_H
