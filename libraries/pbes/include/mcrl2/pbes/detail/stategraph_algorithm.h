// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/stategraph_algorithm.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_STATEGRAPH_ALGORITHM_H
#define MCRL2_PBES_DETAIL_STATEGRAPH_ALGORITHM_H

#include "mcrl2/data/detail/print_utility.h"
#include "mcrl2/data/undefined.h"
#include "mcrl2/pbes/detail/stategraph_graph.h"
#include "mcrl2/pbes/detail/stategraph_pbes.h"
#include "mcrl2/pbes/detail/stategraph_utility.h"
#include "mcrl2/pbes/tools/pbesstategraph_options.h"
#include "mcrl2/utilities/sequence.h"
#include <algorithm>
#include <functional>
#include <sstream>

namespace mcrl2 {

namespace pbes_system {

namespace detail {

inline
std::string print_index(std::size_t index)
{
  std::ostringstream out;
  if (index == data::undefined_index())
  {
    out << "undefined";
  }
  else
  {
    out << index;
  }
  return out.str();
}

/// \brief Algorithm class for the computation of the local and global control flow graphs
class stategraph_algorithm
{
  public:
    // simplify and rewrite the guards of the pbes p
    void simplify(stategraph_pbes& p) const
    {
      for (stategraph_equation& equation: p.equations())
      {
        for (predicate_variable& predvar: equation.predicate_variables())
        {
          predvar.simplify_guard();
        }
      }
    }

  protected:
    // the pbes that is considered
    stategraph_pbes m_pbes;

    // a data rewriter
    data::rewriter m_datar;

    // determines how local control flow parameters are computed
    //
    // Keuze uit twee alternatieven voor de berekening van lokale CFPs.
    // <default>
    //  * Een parameter d^X[n] is een LCFP indien voor alle i waarvoor geldt pred(phi_X,i) = X, danwel:
    //          a. source(X,i,n) and target(X,i,n) zijn beide defined, of
    //          b. copy(X,i,n) is defined en gelijk aan n.
    //
    // <alternative>
    //  * Een parameter d^X[n] is een LCFP indien voor alle i waarvoor geldt pred(phi_X,i) = X, danwel:
    //          a. copy(X,i,n) is undefined en source(X,i,n) and target(X,i,n) zijn beide defined, of
    //          b. copy(X,i,n) is defined (en gelijk aan n) en source(X,i,n) en target(X,i,n) zijn beide undefined.
    //
    // De tweede definieert in feite een exclusive or, terwijl de eerste een standaard or is.
    bool m_use_alternative_lcfp_criterion;

    // determines how global control flow parameters are related
    //
    // Keuze uit twee alternatieven voor het relateren van CFPs.
    // <default>
    //  * Parameters d^X[n] and d^Y[m] zijn gerelateerd als danwel:
    //         a. er is een i z.d.d. copy(X, i, n) = m, of
    //         b. er is een i z.d.d. copy(Y, i, m) = n
    //
    // <alternative>
    //  * Parameters d^X[n] and d^Y[m] zijn gerelateerd als danwel:
    //         a. er is een i z.d.d. copy(X, i, n) = m, en target(X, i, m) is ongedefinieerd, of
    //         b. er is een i z.d.d. copy(Y, i, m) = n en target(Y, i, n) is ongedefinieerd.
    // Hier zit het verschil er dus in dat we, in het tweede geval, parameters alleen relateren als er een copy is
    // van de een naar de ander EN de target in dat geval ongedefinieerd is.
    bool m_use_alternative_gcfp_relation;

    // determines how global control flow parameters are selected
    //
    // Keuze voor de selectie van globale CFPs (of globale consistentie eisen).
    // <default>
    //  * Een set van CFPs is consistent als voor elke d^X[n], en voor alle Y in bnd(E)\{X} (dus in alle andere vergelijkingen), voor alle i waarvoor geldt pred(phi_Y, i) = X, danwel:
    //         a. target(Y, i, n) is gedefinieerd, of
    //         b. copy(Y, i, m) = n voor een of andere globale CFP d^Y[m]
    // Deze eis is in principe voldoende om globale CFPs te identificeren. Als we echter een strikte scheiding tussen control flow parameters en data parameters willen bewerkstelligen, dan moet hier optioneel de volgende eis aan toegevoegd worden:
    //
    // <alternative>
    //  * Een set van CFPs is consistent als de voorgaande eisen gelden, en bovendien voor elke d^X[n] geldt dat voor alle i waarvoor pred(phi_X, i) = Y != X, als d^X[n] affects data(phi_X, i)[m], dan is d^Y[m] een globale control flow parameter.
    // Waar de eerste gemarkeerd is als "detect conflicts for parameters of Y in equations of the form X(d) = ... Y(e)"
    // en de tweede als "detect conflicts for parameters of X in equations of the form X(d) = ... Y(e)".
    bool m_use_alternative_gcfp_consistency;

    // TODO: remove the three booleans above, since they are also present in m_options
    pbesstategraph_options m_options;

    // the local control flow parameters
    std::map<core::identifier_string, std::vector<bool> > m_is_LCFP;

    // the global control flow parameters
    std::map<core::identifier_string, std::vector<bool> > m_is_GCFP;

    // the vertices of the control flow graph
    GCFP_graph m_GCFP_graph;

    // the connected components in the control flow graph; a component contains the indices of vertices in the graph m_GCFP_graph
    std::vector<std::set<std::size_t> > m_connected_components;

    // the possible values of the connected components in the control flow graph
    std::vector<std::set<data::data_expression> > m_connected_components_values;

    // the local control flow graph(s)
    std::vector<local_control_flow_graph> m_local_control_flow_graphs;

    // for readability
    std::set<data::variable> FV(const data::data_expression& x) const
    {
      return data::find_free_variables(x);
    }

    void start_timer(const std::string& msg) const
    {
      if (m_options.timing_enabled())
      {
        m_options.timer->start(msg);
      }
    }

    void finish_timer(const std::string& msg) const
    {
      if (m_options.timing_enabled())
      {
        m_options.timer->finish(msg);
      }
    }

  public:
    void compute_control_flow_parameters()
    {
      start_timer("compute_local_control_flow_parameters");
      compute_local_control_flow_parameters();
      finish_timer("compute_local_control_flow_parameters");

      start_timer("compute_global_control_flow_parameters");
      compute_global_control_flow_parameters();
      finish_timer("compute_global_control_flow_parameters");

      start_timer("compute_related_GCFP_parameters");
      compute_related_GCFP_parameters();
      finish_timer("compute_related_GCFP_parameters");

      start_timer("compute_connected_components");
      compute_connected_components();
      finish_timer("compute_connected_components");
    }

    template <typename T>
    std::string print_control_flow_parameters(const std::string& msg, T& is_CFP)
    {
      std::ostringstream out;
      out << msg << std::endl;
      for (const stategraph_equation& equation: m_pbes.equations())
      {
        auto const& X = equation.variable().name();
        auto const& d_X = equation.parameters();
        const std::vector<bool>& cf = is_CFP[X];
        for (std::size_t i = 0; i < cf.size(); ++i)
        {
          if (cf[i])
          {
            out << "(" << X << ", " << i << ", " << d_X[i] << ")" << std::endl;
          }
        }
      }
      return out.str();
    }

    std::string print_LCFP()
    {
      return print_control_flow_parameters("--- computed LCFP parameters (before removing components) ---", m_is_LCFP);
    }

    std::string print_GCFP()
    {
      return print_control_flow_parameters("--- computed GCFP parameters ---", m_is_GCFP);
    }

    // returns true if m is not a key in the map
    template <typename Map>
    bool is_undefined(const Map& m, const typename Map::key_type& key) const
    {
      return m.find(key) == m.end();
    }

    // returns true if the mapping m maps key to value
    template <typename Map>
    bool is_mapped_to(const Map& m, const typename Map::key_type& key, const typename Map::mapped_type& value) const
    {
      auto i = m.find(key);
      return i != m.end() && i->second == value;
    }

    bool maps_to_and_is_GFCP(const std::map<std::size_t, std::size_t>& m, std::size_t value, const core::identifier_string& X)
    {
      for (const auto& i: m)
      {
        if (i.second == value && m_is_GCFP[X][i.first])
        {
          return true;
        }
      }
      return false;
    }

    void compute_local_control_flow_parameters()
    {
      mCRL2log(log::debug, "stategraph") << "=== computing local control flow parameters ===" << std::endl;

      auto const& equations = m_pbes.equations();

      // initialize all control flow parameters to true
      for (const stategraph_equation& equation: equations)
      {
        auto const& X = equation.variable().name();
        auto const& d_X = equation.parameters();
        m_is_LCFP[X] = std::vector<bool>(d_X.size(), true);
      }

      for (const stategraph_equation& equation: equations)
      {
        auto const& X = equation.variable().name();
        auto const& d_X = equation.parameters();
        auto const& predvars = equation.predicate_variables();
        for (const predicate_variable& Ye: predvars)
        {
          if (Ye.name() == X)
          {
            for (std::size_t n = 0; n < d_X.size(); n++)
            {
              if (m_use_alternative_lcfp_criterion)
              {
                // Een parameter d^X[n] is een LCFP indien voor alle i waarvoor geldt pred(phi_X,i) = X, danwel:
                // 1. copy(X,i,n) is undefined en source(X,i,n) and target(X,i,n) zijn beide defined, of
                // 2. copy(X,i,n) is defined en source(X,i,n) en target(X,i,n) zijn beide undefined.
                if (Ye.copy().find(n) == Ye.copy().end())
                {
                  // copy(X,i,n) is undefined
                  if (is_undefined(Ye.source(), n) || is_undefined(Ye.target(), n))
                  {
                    mCRL2log(log::debug, "stategraph") << "parameter " << print_cfp(X, n) << " is not an LCFP because of predicate variable " << Ye << " in equation " << X << " (copy and source/target undefined)" << std::endl;
                    m_is_LCFP[X][n] = false;
                  }
                }
                else
                {
                  // copy(X,i,n) is defined
                  if (!is_undefined(Ye.source(), n) || !is_undefined(Ye.target(), n))
                  {
                    mCRL2log(log::debug, "stategraph") << "parameter " << print_cfp(X, n) << " is not an LCFP because of predicate variable " << Ye << " in equation " << X << " (copy defined and source/target defined)" << std::endl;
                    m_is_LCFP[X][n] = false;
                  }
                }
              }
              else
              {
                if ((is_undefined(Ye.source(), n) || is_undefined(Ye.target(), n)) && !is_mapped_to(Ye.copy(), n, n))
                {
                  mCRL2log(log::debug, "stategraph") << "parameter " << print_cfp(X, n) << " is not an LCFP due to " << Ye << "(source and target undefined, and no copy to self)" << std::endl;
                  m_is_LCFP[X][n] = false;
                }
              }
            }
          }
        }
      }
      mCRL2log(log::debug, "stategraph") << print_LCFP();
    }

    // Computes the global control flow parameters. The result is stored in m_is_GCFP.
    void compute_global_control_flow_parameters()
    {
      mCRL2log(log::debug, "stategraph") << "=== computing global control flow parameters ===" << std::endl;

      m_is_GCFP = m_is_LCFP;

      auto const& equations = m_pbes.equations();

      bool changed;
      do
      {
        changed = false;

        // Detect conflicts for parameters of Y in equations of the form X(d) = ... Y(e)
        for (const stategraph_equation& equation: equations)
        {
          auto const& X = equation.variable().name();
          auto const& predvars = equation.predicate_variables();
          for (const predicate_variable& Ye: predvars)
          {
            auto const& Y = Ye.name();
            if (Y != X)
            {
              auto const& eq_Y = *find_equation(m_pbes, Y);
              auto const& d_Y = eq_Y.parameters();
              for (std::size_t n = 0; n < d_Y.size(); n++)
              {
                if (is_undefined(Ye.target(), n) && (!maps_to_and_is_GFCP(Ye.copy(), n, X)))
                {
                  if (m_is_GCFP[Y][n])
                  {
                    m_is_GCFP[Y][n] = false;
                    changed = true;
                    mCRL2log(log::debug, "stategraph") << "parameter " << print_cfp(Y, n) << " is not a GCFP because of predicate variable " << Ye << " in equation " << X << std::endl;
                  }
                }
              }
            }
          }
        }

        // Detect conflicts for parameters of X in equations of the form X(d) = ... Y(e)
        if (m_use_alternative_gcfp_consistency)
        {
          for (const stategraph_equation& equation: equations)
          {
            auto const& X = equation.variable().name();
            auto const& predvars = equation.predicate_variables();
            auto const& d_X = equation.parameters();
            std::size_t M = d_X.size();

            for (const predicate_variable& Ye : predvars)
            {
              auto const& Y = Ye.name();
              if (Y == X)
              {
                continue;
              }
              auto const& e = Ye.parameters();
              std::size_t n = 0;
              for (auto ei = e.begin(); ei != e.end(); ++ei, ++n)
              {
                std::set<data::variable> V = FV(*ei);
                for (std::size_t m = 0; m < M; m++)
                {
                  if (m_is_GCFP[X][m] && !m_is_GCFP[Y][n] && (V.find(d_X[m]) != V.end()))
                  {
                    m_is_GCFP[X][m] = false;
                    changed = true;
                    mCRL2log(log::debug, "stategraph") << "parameter " << print_cfp(X, m) << " is not a GCFP because of predicate variable " << Ye << " in equation " << X << std::endl;
                  }
                }
              }
            }
          }
        }
      }
      while (changed);

      mCRL2log(log::debug, "stategraph") << print_GCFP();
    }

    bool is_LCFP_parameter(const core::identifier_string& X, std::size_t i) const
    {
      auto j = m_is_LCFP.find(X);
      assert(j != m_is_LCFP.end());
      const std::vector<bool>& cf = j->second;
      assert(i < cf.size());
      return cf[i];
    }

    bool is_GCFP_parameter(const core::identifier_string& X, std::size_t i) const
    {
      auto j = m_is_GCFP.find(X);
      assert(j != m_is_GCFP.end());
      const std::vector<bool>& cf = j->second;
      assert(i < cf.size());
      return cf[i];
    }

    bool is_global_control_flow_parameter(const core::identifier_string& X, std::size_t i) const
    {
      using utilities::detail::contains;
      auto const& eq_X = *find_equation(m_pbes, X);
      const std::vector<std::size_t>& I = eq_X.control_flow_parameter_indices();
      return contains(I, i);
    }

    // relate (X, n) and (Y, m) in the dependency graph
    // \pre: the equation of X has a lower rank than the equation of Y
    void relate_GCFP_vertices(const core::identifier_string& X, std::size_t n, const core::identifier_string& Y, std::size_t m)
    {
      GCFP_vertex& u = m_GCFP_graph.find_vertex(X, n);
      GCFP_vertex& v = m_GCFP_graph.find_vertex(Y, m);
      u.neighbors().insert(&v);
      v.neighbors().insert(&u);
    }

    std::string print_cfp(const core::identifier_string& X, std::size_t i) const
    {
      auto const& eq_X = *find_equation(m_pbes, X);
      auto const& d = eq_X.parameters()[i];
      std::ostringstream out;
      out << "(" << X << ", " << i << ", " << d << ")";
      return out.str();
    }

    std::string log_related_parameters(const core::identifier_string& X,
                                       std::size_t n,
                                       const core::identifier_string& Y,
                                       std::size_t m,
                                       const predicate_variable& Ye,
                                       const std::string& reason = ""
                                      ) const
    {
      std::ostringstream out;
      if (X != Y || n != m)
      {
        out << print_cfp(X, n) << " and " << print_cfp(Y, m) << " are related " << "because of recursion " << Ye << " in the equation for " << X << reason;
      }
      return out.str();
    }

    // Determines which control flow parameters are related. This is done by assigning neighbors to the
    // vertices in m_GCFP_graph.
    void compute_related_GCFP_parameters()
    {
      mCRL2log(log::debug, "stategraph") << "=== computing related global control flow parameters ===" << std::endl;
      const std::vector<stategraph_equation>& equations = m_pbes.equations();

      // step 1: create vertices in m_GCFP_graph
      for (const stategraph_equation& equation: equations)
      {
        auto const& X = equation.variable().name();
        auto const& d_X = equation.parameters();
        for (std::size_t n = 0; n < d_X.size(); n++)
        {
          if (is_GCFP_parameter(X, n))
          {
            m_GCFP_graph.add_vertex(GCFP_vertex(X, n, d_X[n]));
          }
        }
      }

      // step 2: create edges between related vertices in m_GCFP_graph
      for (const stategraph_equation& equation: equations)
      {
        auto const& X = equation.variable().name();
        auto const& predvars = equation.predicate_variables();
        for (const predicate_variable& Ye: predvars)
        {
          auto const& Y = Ye.name();
          auto const& copy = Ye.copy();
          for (const auto& j: copy)
          {
            std::size_t n = j.first;
            std::size_t m = j.second;
            if (is_GCFP_parameter(X, n) && is_GCFP_parameter(Y, m))
            {
              if (m_use_alternative_gcfp_relation)
              {
                // Twee parameters zijn alleen gerelateerd als er een copy is van de een naar de ander,
                // EN dat de target in dat geval ongedefinieerd is (dus we weten echt geen concrete waarde
                // voor de parameter op dat punt).
                if (is_undefined(Ye.target(), m))
                {
                  mCRL2log(log::debug, "stategraph") << log_related_parameters(X, n, Y, m, Ye) << std::endl;
                  relate_GCFP_vertices(X, n, Y, m);
                }
              }
              else
              {
                mCRL2log(log::debug, "stategraph") << log_related_parameters(X, n, Y, m, Ye, " (target is undefined)") << std::endl;
                relate_GCFP_vertices(X, n, Y, m);
              }
            }
          }
        }
      }
    }

    // a connected component is valid if it does not contain two nodes (X, n) and (Y, m) with X == Y
    bool is_valid_connected_component(const std::set<std::size_t>& component) const
    {
      std::set<core::identifier_string> V;
      for (std::size_t i: component)
      {
        auto const& X = m_GCFP_graph.vertex(i).name();
        if (V.find(X) != V.end())
        {
          return false;
        }
        V.insert(X);
      }
      return true;
    }

    std::string print_connected_component(const std::set<std::size_t>& component) const
    {
      std::ostringstream out;
      out << "{";
      for (auto i = component.begin(); i != component.end(); ++i)
      {
        if (i != component.begin())
        {
          out << ", ";
        }
        out << m_GCFP_graph.vertex(*i);
      }
      out << "}";
      if (!is_valid_connected_component(component))
      {
        out << " (invalid)";
      }
      return out.str();
    }

    void print_connected_components() const
    {
      for (const std::set<std::size_t>& component: m_connected_components)
      {
        mCRL2log(log::debug, "stategraph") << print_connected_component(component) << std::endl;
      }
    }

    // compute the connected component belonging to vertex i in m_GCFP_graph
    std::set<std::size_t> compute_connected_component(std::size_t i, std::vector<bool>& done) const
    {
      using utilities::detail::pick_element;

      std::set<std::size_t> todo;
      std::set<std::size_t> component;

      todo.insert(i);
      while (!todo.empty())
      {
        std::size_t u_index = pick_element(todo);
        const GCFP_vertex& u = m_GCFP_graph.vertex(u_index);
        done[u_index] = true;
        component.insert(u_index);

        for (GCFP_vertex* w: u.neighbors())
        {
          std::size_t w_index = m_GCFP_graph.index(*w);
          if (!done[w_index])
          {
            todo.insert(w_index);
          }
        }
      }
      return component;
    }

    void compute_connected_components()
    {
      mCRL2log(log::debug, "stategraph") << "=== computing connected components ===" << std::endl;

      // done[i] means that vertex i in m_GCFP_graph has been processed
      std::vector<bool> done(m_GCFP_graph.vertices().size(), false);

      for (std::size_t i = 0; i < done.size(); i++)
      {
        if (done[i])
        {
          continue;
        }
        std::set<std::size_t> component = compute_connected_component(i, done);
        m_connected_components.push_back(component);
      }
      mCRL2log(log::debug, "stategraph") << "--- computed connected components ---" << std::endl;
      print_connected_components();
    }

    // removes the connected components V for which !is_valid_connected_component(V)
    void remove_invalid_connected_components()
    {
      auto i = std::remove_if(m_connected_components.begin(), m_connected_components.end(),
                              std::bind(std::logical_not<bool>(),
                                        std::bind(&stategraph_algorithm::is_valid_connected_component, this, std::placeholders::_1)));
      m_connected_components.erase(i, m_connected_components.end());
    }

    // Returns true if d_X[m] is not only copied.
    //
    // A CFP d_X[m] is not only copied if
    //    (1) for some i    : source(X, i, m) is defined
    // or (2) for some Y, i : pred(phi_Y , i) = X and target(Y, i, m) is defined
    bool is_not_only_copied(const core::identifier_string& X, std::size_t m) const
    {
      // check (1)
      auto const& eq_X = *find_equation(m_pbes, X);
      auto const& predvars = eq_X.predicate_variables();
      for (const predicate_variable& predvar: predvars)
      {
        if (!is_undefined(predvar.source(), m))
        {
          return true;
        }
      }

      // check (2)
      for (const stategraph_equation& equation: m_pbes.equations())
      {
        for (const auto& predvar: equation.predicate_variables())
        {
          if (predvar.name() == X && !is_undefined(predvar.target(), m))
          {
            return true;
          }
        }
      }
      return false;
    }

    // Returns true if all CFPs in component are 'only copied'
    bool has_only_copied_CFPs(const std::set<std::size_t>& component) const
    {
      for (std::size_t i: component)
      {
        const GCFP_vertex& u = m_GCFP_graph.vertex(i);
        const auto& X = u.name();
        auto m = u.index();
        if (is_not_only_copied(X, m))
        {
          return false;
        }
      }
      return true;
    }

    // Removes the connected components V that consist of CFPs that are only copied.
    void remove_only_copy_components()
    {
      auto i = std::remove_if(m_connected_components.begin(), m_connected_components.end(), std::bind(&stategraph_algorithm::has_only_copied_CFPs, this, std::placeholders::_1));
      m_connected_components.erase(i, m_connected_components.end());
      mCRL2log(log::debug, "stategraph") << "--- connected components after removing 'only copy' ones ---" << std::endl;
      print_connected_components();
    }

    const std::vector<bool>& is_GCFP(const core::identifier_string& X) const
    {
      auto i = m_is_GCFP.find(X);
      assert (i != m_is_GCFP.end());
      return i->second;
    }

    // returns the control flow parameters of the propositional variable with name X
    std::set<data::variable> control_flow_parameters(const core::identifier_string& X) const
    {
      std::set<data::variable> result;
      const std::vector<bool>& b = is_GCFP(X);
      auto const& eq_X = *find_equation(m_pbes, X);
      auto const& d_X = eq_X.parameters();
      std::size_t index = 0;
      for (auto i = d_X.begin(); i != d_X.end(); ++i, index++)
      {
        if (b[index])
        {
          result.insert(*i);
        }
      }
      return result;
    }

    std::vector<std::size_t> control_flow_parameter_indices(const core::identifier_string& X) const
    {
      std::vector<std::size_t> result;
      auto const& eq_X = *find_equation(m_pbes, X);
      auto const& d_X = eq_X.parameters();
      for (std::size_t k = 0; k < d_X.size(); k++)
      {
        if (is_global_control_flow_parameter(X, k))
        {
          result.push_back(k);
        }
      }
      return result;
    }

    std::string print_data_parameters(const core::identifier_string& X, std::set<std::size_t> I) const
    {
      std::ostringstream out;
      out << "  data parameters for vertex " << X << ":";
      for (std::size_t q: I)
      {
        out << " " << print_cfp(X, q);
      }
      return out.str();
    }

    // set the control flow and data parameter information in p
    void set_parameters(const stategraph_pbes& p)
    {
      // set control flow parameters
      std::map<core::identifier_string, std::set<std::size_t> > CFP; // CFP["X"] is the set of indices of control flow parameters of equation "X"
      for (const std::set<std::size_t>& component: m_connected_components)
      {
        for (std::size_t j: component)
        {
          const GCFP_vertex& u = m_GCFP_graph.vertex(j);
          if (u.has_variable())
          {
            CFP[u.name()].insert(u.index());
          }
        }
      }
      for (auto& i: CFP)
      {
        auto const& X = i.first;
        auto const& eqn = *find_equation(p, X);
        eqn.set_control_flow_parameters(i.second);
      }

      // set data parameters
      for (const stategraph_equation& eq_X: m_pbes.equations())
      {
        auto const& X = eq_X.variable().name();
        std::set<std::size_t> I; // data parameters of equation eq_X
        for (std::size_t i = 0; i < eq_X.parameters().size(); ++i)
        {
          I.insert(i);
        }
        for (const std::set<std::size_t>& component : m_connected_components)
        {
          for (std::size_t j: component)
          {
            const GCFP_vertex& u = m_GCFP_graph.vertex(j);
            if (u.name() == X && u.index() != data::undefined_index())
            {
              I.erase(u.index());
            }
          }
        }
        eq_X.set_data_parameters(I);
      }
    }

    // prints all vertices of the connected components
    void print_final_control_flow_parameters() const
    {
      std::ostringstream out;
      mCRL2log(log::verbose, "stategraph") << "--- computed control flow parameters ---" << std::endl;

      // collect the control flow points in the map CFP
      std::map<core::identifier_string, std::set<const GCFP_vertex*> > CFP;
      for (const std::set<std::size_t>& component: m_connected_components)
      {
        for (std::size_t j: component)
        {
          const GCFP_vertex& u = m_GCFP_graph.vertex(j);
          if (u.has_variable())
          {
            CFP[u.name()].insert(&u);
          }
        }
      }

      // print the map CFP
      for (auto i = CFP.begin(); i != CFP.end(); ++i)
      {
        if (i != CFP.begin())
        {
          out << "\n";
        }
        auto const& V = i->second;
        for (auto j = V.begin(); j != V.end(); ++j)
        {
          if (j != V.begin())
          {
            out << ", ";
          }
          out << **j;
        }
      }
      mCRL2log(log::verbose, "stategraph") << out.str() << std::endl;
    }

    stategraph_algorithm(const pbes& p, const pbesstategraph_options& options)
      : m_datar(p.data(), options.rewrite_strategy),
        m_use_alternative_lcfp_criterion(options.use_alternative_lcfp_criterion),
        m_use_alternative_gcfp_relation(options.use_alternative_gcfp_relation),
        m_use_alternative_gcfp_consistency(options.use_alternative_gcfp_consistency),
        m_options(options)
    {
      m_pbes = stategraph_pbes(p);
    }

    /// \brief Returns the connected components of the global control flow graph.
    const std::vector<std::set<std::size_t> >& connected_components() const
    {
      return m_connected_components;
    }

    std::string print(const GCFP_vertex& u) const
    {
      std::ostringstream out;
      out << '(' << u.name() << ", " << find_equation(m_pbes, u.name())->parameters()[u.index()].name() << ')';
      return out.str();
    }

    /// \brief Computes the values that the CFPs in component can attain.
    std::set<data::data_expression> compute_connected_component_values(const std::set<std::size_t>& component)
    {
      std::set<data::data_expression> result;

      // search for a node that corresponds to a variable in the init of the PBES
      const propositional_variable_instantiation& init = m_pbes.initial_state();
      auto const& Xinit = init.name();

      for (std::size_t j: component)
      {
        const GCFP_vertex& u = m_GCFP_graph.vertex(j);
        if (u.name() == Xinit)
        {
          const data::data_expression& d = nth_element(init.parameters(), u.index());
          result.insert(d);
        }
      }

      // source(X, i, k) = v
      for (std::size_t p: component)
      {
        const GCFP_vertex& u = m_GCFP_graph.vertex(p);
        auto const& X = u.name();
        std::size_t k = u.index();
        auto const& eq_X = *find_equation(m_pbes, X);
        for (const predicate_variable& predvar: eq_X.predicate_variables())
        {
          auto q = predvar.source().find(k);
          if (q != predvar.source().end())
          {
            const data::data_expression& v = q->second;
            result.insert(v);
          }
        }
      }

      // target(X, i, k) = v
      for (std::size_t p: component)
      {
        const GCFP_vertex& u = m_GCFP_graph.vertex(p);
        auto const& Y = u.name();
        std::size_t k = u.index();
        auto const& eq_Y = *find_equation(m_pbes, Y);
        for (const predicate_variable& predvar: eq_Y.predicate_variables())
        {
          if (predvar.name() != Y)
          {
            continue;
          }
          auto q = predvar.target().find(k);
          if (q != predvar.target().end())
          {
            const data::data_expression& v = q->second;
            result.insert(v);
          }
        }
      }

      return result;
    }

    void compute_connected_component_values()
    {
      mCRL2log(log::debug, "stategraph") << "=== computing values for the components" << std::endl;
      for (const std::set<std::size_t>& component: m_connected_components)
      {
        std::set<data::data_expression> values = compute_connected_component_values(component);
        m_connected_components_values.push_back(values);
        mCRL2log(log::debug, "stategraph") << print_connected_component(component) << " values = " << core::detail::print_set(values) << std::endl;
      }
    }

    // Returns the possible values of the vertex (X, j). N.B. Very inefficient!
    std::vector<data::data_expression> compute_values(const core::identifier_string& X, std::size_t j) const
    {
      // search for a graph that contains (X, j)
      for (std::size_t k = 0; k < m_local_control_flow_graphs.size(); k++)
      {
        auto const& Gk = m_local_control_flow_graphs[k];
        if (Gk.has_vertex(X, j))
        {
          auto const& values = m_connected_components_values[k];
          return std::vector<data::data_expression>(values.begin(), values.end());
        }
      }
      throw mcrl2::runtime_error("error in compute_values: vertex not found");
    }

    /// \brief Computes the control flow graph
    void run()
    {
      simplify(m_pbes);
      m_pbes.compute_source_target_copy();
      mCRL2log(log::debug, "stategraph") << "--- source, target, copy ---\n" << m_pbes.print_source_target_copy() << std::endl;
      compute_control_flow_parameters();
      remove_invalid_connected_components();
      remove_only_copy_components();
      mCRL2log(log::debug1, "stategraph") << "--- GCFP graph = ---\n" << m_GCFP_graph << std::endl;
      set_parameters(m_pbes);
      print_final_control_flow_parameters();

      start_timer("compute_connected_component_values");
      compute_connected_component_values();
      finish_timer("compute_connected_component_values");
    }

    const stategraph_pbes& get_pbes() const
    {
      return m_pbes;
    }

    const GCFP_graph& GCFP() const
    {
      return m_GCFP_graph;
    }
};

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_STATEGRAPH_ALGORITHM_H
