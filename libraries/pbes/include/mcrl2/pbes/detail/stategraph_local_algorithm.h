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

/// \brief Algorithm class for the local variant of the stategraph algorithm
class stategraph_local_algorithm: public stategraph_algorithm
{
  public:
    typedef stategraph_algorithm super;

  protected:
    // m_belongs[k] corresponds with m_control_flow_graphs[k]
    std::vector<std::map<core::identifier_string, std::set<data::variable> > > m_belongs;

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

    template <typename RulesPredicate>
    std::map<core::identifier_string, std::set<data::variable> > compute_belongs(const local_control_flow_graph& Vk, const std::set<data::data_expression>& values_k, RulesPredicate rules)
    {
      std::map<core::identifier_string, std::set<data::variable> > Bk;
      for (auto p = Vk.vertices.begin(); p != Vk.vertices.end(); ++p)
      {
        auto const& X = p->name();
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
        std::map<core::identifier_string, std::set<data::variable> > Bk = compute_belongs(Vk, values_k, default_rules_predicate(Vk));
        m_belongs.push_back(Bk);
      }
    }

    void print_belongs() const
    {
      std::ostringstream out;
      out << "\n=== belongs relation ===\n";
      for (std::size_t k = 0; k < m_belongs.size(); k++)
      {
        out << "--- belongs relation for graph " << k << " ---" << std::endl;
        const std::map<core::identifier_string, std::set<data::variable> >& Bk = m_belongs[k];
        for (std::map<core::identifier_string, std::set<data::variable> >::const_iterator i = Bk.begin(); i != Bk.end(); ++i)
        {
          out << core::pp(i->first) << " -> " << core::detail::print_set(i->second) << std::endl;
        }
      }
      mCRL2log(log::debug, "stategraph") << out.str() << std::endl;
    }

    std::set<data::variable> significant_variables(const local_control_flow_graph_vertex& u) const
    {
      const core::identifier_string& X = u.name();
      const pbes_equation& eq_X = *find_equation(m_pbes, X);
      pbes_expression phi = eq_X.formula();
mCRL2log(log::debug, "stategraph") << "  compute significant variables of vertex u = " << u << std::endl;
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
mCRL2log(log::debug, "stategraph") << "  rhs           = " << phi0 << std::endl;
mCRL2log(log::debug, "stategraph") << "  simplify(rhs) = " << phi << std::endl;
      // return pbes_system::algorithms::significant_variables(phi);
      std::set<data::variable> result = pbes_system::algorithms::significant_variables(phi);
mCRL2log(log::debug, "stategraph") << "  significant variables: " << core::detail::print_set(result) << std::endl;
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

    bool belongs_contains(const std::map<core::identifier_string, std::set<data::variable> >& B,
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
                                                  const std::map<core::identifier_string, std::set<data::variable> >& B,
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

    // updates u.marking
    // returns true if u.marking has changed
    bool update_marking_rule1(const local_control_flow_graph& V,
                              const std::map<core::identifier_string, std::set<data::variable> >& B,
                              const local_control_flow_graph_vertex& u
                             )
    {
      std::size_t size = u.marking().size();
      auto const& X = u.name();
      auto const& eq_X = *find_equation(m_pbes, X);
      auto const& outgoing_edges = u.outgoing_edges();
      for (auto ei = outgoing_edges.begin(); ei != outgoing_edges.end(); ++ei)
      {
        auto const& v = *ei->first;
        auto const& labels = ei->second;
        for (auto ii = labels.begin(); ii != labels.end(); ++ii)
        {
          // consider edge (u, i, v)
          std::size_t i = *ii;
          auto const& Y = v.name();
          auto const& eq_Y = *find_equation(m_pbes, Y);
          const predicate_variable& Ye = eq_X.predicate_variables()[i];
          auto const& e = Ye.X.parameters();
          for (auto di = v.marking().begin(); di != v.marking().end(); ++di)
          {
            auto const& d = *di;
            std::size_t l = find_parameter_index(eq_Y.parameters(), d);
            data::mutable_map_substitution<> sigma;
            sigma[u.variable()] = u.value();
            auto W = FV(rewr(nth_element(e, l), sigma));
            u.set_marking(data::detail::set_union(u.marking(), belongs_intersection(W, B, X)));
          }
        }
      }
      return u.marking().size() != size;
    }

    // updates u.marking
    // returns true if u.marking has changed
    bool update_marking_rule2(std::size_t i,
                              const local_control_flow_graph& V,
                              const std::map<core::identifier_string, std::set<data::variable> >& B,
                              const local_control_flow_graph& V1,
                              const std::map<core::identifier_string, std::set<data::variable> >& B1,
                              const local_control_flow_graph_vertex& u,
                              const local_control_flow_graph_vertex& v
                             )
    {
      std::size_t size = u.marking().size();
      auto const& X = u.name();
      auto const& eq_X = *find_equation(m_pbes, X);
      auto const& Y = v.name();
      auto const& eq_Y = *find_equation(m_pbes, Y);
      const predicate_variable& Ye = eq_X.predicate_variables()[i];
      auto const& e = Ye.X.parameters();
      for (auto di = v.marking().begin(); di != v.marking().end(); ++di)
      {
        auto const& d = *di;
        if (belongs_contains(B, Y, d))
        {
          continue;
        }
        std::size_t l = find_parameter_index(eq_Y.parameters(), d);
        data::mutable_map_substitution<> sigma;
        sigma[u.variable()] = u.value();
        auto W = FV(rewr(nth_element(e, l), sigma));
        u.set_marking(data::detail::set_union(u.marking(), belongs_intersection(W, B, X)));
      }
      return u.marking().size() != size;
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
              auto ti = todo.begin();
              todo.erase(ti);
              const local_control_flow_graph_vertex& u = **ti;
              auto m = u.marking();
              bool changed = update_marking_rule1(Vj, Bj, u);
              if (changed)
              {
                auto const& outgoing_edges = u.outgoing_edges();
                for (auto ei = outgoing_edges.begin(); ei != outgoing_edges.end(); ++ei)
                {
                  auto const& v = *ei->first;
                  todo.insert(&v);
                }
                stableint = false;
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
              auto const& eq_X = *find_equation(m_pbes, X);
              auto const& predvars = eq_X.predicate_variables();
              for (std::size_t i = 0; i < predvars.size(); i++)
              {
                auto const& Ye = predvars[i];
                auto const& Y = Ye.X.name();
                for (std::size_t k = j + 1; k < J; k++)
                {
                  auto const& Vk = m_local_control_flow_graphs[k];
                  auto const& Bk = m_belongs[k];
                  for (auto vk = Vk.vertices.begin(); vk != Vk.vertices.end(); ++vk)
                  {
                    auto const& v = *vk;
                    if (v.name() != Y)
                    {
                      continue;
                    }
                    for (auto di = v.marking().begin(); di != v.marking().end(); ++di)
                    {
                      auto const& d = *di;
                      if (!belongs_contains(Bk, Y, d))
                      {
                        auto m = u.marking();
                        bool changed = update_marking_rule2(i, Vj, Bj, Vk, Bk, u, v);
                        if (changed)
                        {
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
      compute_control_flow_marking();
      print_control_flow_marking();
    }
};

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_STATEGRAPH_LOCAL_ALGORITHM_H
