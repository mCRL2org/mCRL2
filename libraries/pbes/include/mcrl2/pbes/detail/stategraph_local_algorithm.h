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

    belongs_map m_belongs;

  public:
    const std::set<data::variable>& operator[](const core::identifier_string& X) const
    {
      auto i = m_belongs.find(X);
      if (i == m_belongs.end())
      {
        throw mcrl2::runtime_error("unknown key encountered in belongs_relation::operator[]");
      }
      return i->second;
    }

    belongs_map::iterator begin()
    {
      return m_belongs.begin();
    }

    belongs_map::const_iterator begin() const
    {
      return m_belongs.begin();
    }

    belongs_map::iterator end()
    {
      return m_belongs.end();
    }

    belongs_map::const_iterator end() const
    {
      return m_belongs.end();
    }

    belongs_map::iterator find(const core::identifier_string& X)
    {
      return m_belongs.find(X);
    }

    belongs_map::const_iterator find(const core::identifier_string& X) const
    {
      return m_belongs.find(X);
    }

    std::set<data::variable>& operator[](const core::identifier_string& X)
    {
      return m_belongs[X];
    }

    std::size_t size() const
    {
      return m_belongs.size();
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

    // prints a belong set
    std::string print_belong_set(const stategraph_equation& eq, const std::set<std::size_t>& belongs) const
    {
      std::set<data::variable> v;
      for (auto i = belongs.begin(); i != belongs.end(); ++i)
      {
        v.insert(eq.parameters()[*i]);
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
    belongs_relation compute_belongs(const local_control_flow_graph& Vk, const std::set<data::data_expression>& values_k, RulesPredicate rules)
    {
      belongs_relation Bk;
      for (auto p = Vk.vertices.begin(); p != Vk.vertices.end(); ++p)
      {
        auto const& X = p->name();
        Bk[X]; // force the creation of an empty set corresponding to X
        auto const& eq_X = *find_equation(m_pbes, X);
        std::set<std::size_t> belongs = data_parameter_indices(X);
        mCRL2log(log::debug1, "stategraph") << "  initial belong set for vertex " << *p << " = " << print_belong_set(eq_X, belongs) << std::endl;

        auto const& predvars = eq_X.predicate_variables();
        for (std::size_t i = 0; i < predvars.size(); i++)
        {
          const predicate_variable& X_i = predvars[i];
          for (std::set<std::size_t>::iterator j = belongs.begin(); j != belongs.end(); )
          {
            std::size_t m = *j;
            if ((X_i.used.find(m) != X_i.used.end() || X_i.changed.find(m) != X_i.changed.end()) && !rules(X, i))
            {
              mCRL2log(log::debug1, "stategraph") << " remove (X, i, m) = (" << X << ", " << i << ", " << m << ") variable=" << eq_X.parameters()[m] << " from belongs " << std::endl;
              mCRL2log(log::debug2, "stategraph") << "  used = " << print_parameters(X_i.X.name(), X_i.used) << " changed = " << print_parameters(X_i.X.name(), X_i.changed) << std::endl;
              belongs.erase(j++);
            }
            else
            {
              ++j;
            }
          }
        }
        mCRL2log(log::debug1, "stategraph") << "  final   belong set for vertex " << *p << " = " << print_belong_set(eq_X, belongs) << std::endl;
        for (std::set<std::size_t>::const_iterator j = belongs.begin(); j != belongs.end(); ++j)
        {
          Bk[X].insert(eq_X.parameters()[*j]);
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
        auto const& values_k = m_connected_components_values[k];
        belongs_relation Bk = compute_belongs(Vk, values_k, default_rules_predicate(Vk));
        m_belongs.push_back(Bk);
      }
    }

    std::string print_belongs(const belongs_relation& B) const
    {
      std::ostringstream out;
      for (auto i = B.begin(); i != B.end(); ++i)
      {
        out << i->first << " -> " << core::detail::print_set(i->second) << std::endl;
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
mCRL2log(log::debug2, "stategraph") << "  compute significant variables of vertex u = " << u << std::endl;
pbes_expression phi0 = phi;
      if (u.index() != data::undefined_index())
      {
        data::variable d = u.variable();
        data::data_expression e = u.value();
        data::mutable_map_substitution<> sigma;
        sigma[d] = e;
        pbes_system::simplifying_rewriter<pbes_expression, data::rewriter> pbesr(m_datar);
        phi = pbesr(phi, sigma);
      }
mCRL2log(log::debug2, "stategraph") << "  rhs           = " << phi0 << std::endl;
mCRL2log(log::debug2, "stategraph") << "  simplify(rhs) = " << phi << std::endl;
      // return pbes_system::algorithms::significant_variables(phi);
      std::set<data::variable> result = pbes_system::algorithms::significant_variables(phi);
mCRL2log(log::debug2, "stategraph") << "  significant variables: " << core::detail::print_set(result) << std::endl;
      return result;
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
      return data::detail::set_intersection(V, i->second);
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
#ifdef MCRL2_CACHE_MARKING_UPDATES
      // check if the update is cached in u
      auto p = std::make_pair(i, d);
      auto const& marking_update = u.marking_update();
      auto j = marking_update.find(p);
      if (j != marking_update.end())
      {
        return j->second;
      }
#endif
      // compute the value
      auto const& X = u.name();
      std::size_t l = find_parameter_index(eq_Y.parameters(), d);
      data::mutable_map_substitution<> sigma;
      sigma[u.variable()] = u.value();
      auto W = FV(rewr(nth_element(e, l), sigma));
      std::set<data::variable> V = belongs_intersection(W, B, X);
#ifdef MCRL2_CACHE_MARKING_UPDATES
      u.set_marking_update(i, d, V);
#endif
      return V;
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
      const predicate_variable& Ye = eq_X.predicate_variables()[i];
      auto const& e = Ye.X.parameters();
      auto m = v.marking(); // N.B. a copy must be made, to handle the case u == v properly
      for (auto di = m.begin(); di != m.end(); ++di)
      {
        auto const& d = *di;
        if (check_belongs && belongs_contains(B, Y, d))
        {
          continue;
        }
        auto update = marking_update(u, i, d, e, eq_Y, B);
        u.set_marking(data::detail::set_union(u.marking(), update));
      }
      return u.marking().size() != size;
    }

    // returns true if there is a vertex u in V and an edge (u, i, v) in V, such that u.name() == X
    bool has_incoming_edge(const local_control_flow_graph& V, const local_control_flow_graph_vertex& v, const core::identifier_string& X, std::size_t i) const
    {
      for (auto j = V.vertices.begin(); j != V.vertices.end(); ++j)
      {
        auto const& u = *j;
        if (u.name() != X)
        {
          continue;
        }
        auto const& outgoing_edges = u.outgoing_edges();
        auto k = outgoing_edges.find(&v);
        if (k != outgoing_edges.end())
        {
          auto const& I = k->second;
          if (I.find(i) != I.end())
          {
            return true;
          }
        }
      }
      return false;
    }

    void compute_control_flow_marking()
    {
      mCRL2log(log::debug, "stategraph") << "=== computing control flow marking ===" << std::endl;
      using data::detail::set_difference;
      using data::detail::set_intersection;
      using data::detail::set_union;

      std::size_t J = m_local_control_flow_graphs.size();
      for (std::size_t j = 0; j < J; j++)
      {
        auto const& Vj = m_local_control_flow_graphs[j];
        auto const& Bj = m_belongs[j];
        for (auto ui = Vj.vertices.begin(); ui != Vj.vertices.end(); ++ui)
        {
          auto& u = *ui;
          auto const& X = u.name();
          u.set_marking(belongs_intersection(significant_variables(u), Bj, X));
        }
        mCRL2log(log::debug, "stategraph") << "--- initial control flow marking for graph " << j << "\n" << Vj.print_marking();
      }

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
            for (auto q = Vj.vertices.begin(); q != Vj.vertices.end(); ++q)
            {
              todo.insert(&(*q));
            }
            while (!todo.empty())
            {
              auto const& u = *pick_element(todo);
              auto const& X = u.name();
              if (u.marking().size() == Bj[X].size())
              {
                continue;
              }
              mCRL2log(log::debug1, "stategraph") << " extend marking rule1: u = " << u << " marking(u) = " << core::detail::print_set(u.marking()) << std::endl;
              auto m = u.marking();

              auto const& outgoing_edges = u.outgoing_edges();
              for (auto ei = outgoing_edges.begin(); ei != outgoing_edges.end(); ++ei)
              {
                auto const& v = *ei->first;
                auto const& labels = ei->second;
                for (auto ii = labels.begin(); ii != labels.end(); ++ii)
                {
                  // consider edge (u, i, v)
                  std::size_t i = *ii;
                  bool changed = update_marking_rule(Bj, u, i, v, false);
                  if (changed)
                  {
                    mCRL2log(log::debug1, "stategraph") << "   marking(u)' = " << core::detail::print_set(u.marking()) << std::endl;
                    auto const& incoming_edges = u.incoming_edges();
                    for (auto fi = incoming_edges.begin(); fi != incoming_edges.end(); ++fi)
                    {
                      todo.insert(fi->first);
                    }
                    stableint = false;
                  }
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
            for (auto vj = Vj.vertices.begin(); vj != Vj.vertices.end(); ++vj)
            {
              auto const& u = *vj;
              auto const& X = u.name();
              if (u.marking().size() == Bj[X].size())
              {
                continue;
              }
              mCRL2log(log::debug1, "stategraph") << " extend marking rule2: u = " << u << " marking(u) = " << core::detail::print_set(u.marking()) << std::endl;
              auto const& eq_X = *find_equation(m_pbes, X);
              auto const& predvars = eq_X.predicate_variables();
              for (std::size_t i = 0; i < predvars.size(); i++)
              {
                auto const& Ye = predvars[i];
                auto const& Y = Ye.X.name();
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
                      auto m = u.marking();
                      bool changed = update_marking_rule(Bj, u, i, v, true);
                      if (changed)
                      {
                        mCRL2log(log::debug1, "stategraph") << "   marking(u)' = " << core::detail::print_set(u.marking()) << std::endl;
                        stableint = false;
                        stableext = false;
                      }
                    }
                  }
                }
              }
            }
          }
        }
        stable = stableint;
      }
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

    void remove_belongs(belongs_relation& B,
                        const belongs_relation& B1)
    {
      for (auto i = B1.begin(); i != B1.end(); ++i)
      {
        auto const& X = i->first;
        auto const& V = i->second;
        auto& B_X = B[X];
        for (auto v = V.begin(); v != V.end(); ++v)
        {
          B_X.erase(*v);
        }
      }
    }

    void compute_extra_local_control_flow_graph()
    {
      local_control_flow_graph V;
      belongs_relation B;

      auto const& equations = m_pbes.equations();
      for (auto k = equations.begin(); k != equations.end(); ++k)
      {
        auto const& eq_X = *k;
        auto const& X = eq_X.variable().name();
        auto const& u = V.insert_vertex(local_control_flow_graph_vertex(X, data::undefined_data_expression()));

        auto const& d_X = eq_X.parameters();
        B[X]; // force creation of empty set corresponding to X
        std::set<std::size_t> I = data_parameter_indices(X);
        for (auto i = I.begin(); i != I.end(); ++i)
        {
          B[X].insert(d_X[*i]);
        }

        auto const& predvars = eq_X.predicate_variables();
        for (std::size_t i = 0; i < predvars.size(); i++)
        {
          const predicate_variable& X_i = predvars[i];
          for (auto j = m_belongs.begin(); j != m_belongs.end(); ++j)
          {
            auto const& Bj = *j;
            remove_belongs(B, Bj);
          }
          auto const& v = V.insert_vertex(local_control_flow_graph_vertex(X_i.X.name(), data::undefined_data_expression()));
          V.insert_edge(u, i, v);
        }
      }
      m_local_control_flow_graphs.push_back(V);
      m_local_control_flow_graphs.back().compute_index();
      m_belongs.push_back(B);
      mCRL2log(log::debug, "stategraph") << "--- extra local control flow graph\n" << m_local_control_flow_graphs.back() << std::endl;
      mCRL2log(log::debug, "stategraph") << "--- belongs relation for extra graph\n" << print_belongs(B);
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
      compute_belongs();
      print_belongs();
      compute_extra_local_control_flow_graph();
      compute_control_flow_marking();
      print_control_flow_marking();
    }
};

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_STATEGRAPH_LOCAL_ALGORITHM_H
