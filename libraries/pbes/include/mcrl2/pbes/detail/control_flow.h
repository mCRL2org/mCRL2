// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/control_flow.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_CONTROL_FLOW_H
#define MCRL2_PBES_DETAIL_CONTROL_FLOW_H

#include <algorithm>
#include <map>
#include <set>
#include <sstream>
#include <vector>
#include "mcrl2/pbes/find.h"
#include "mcrl2/pbes/detail/is_pfnf.h"
#include "mcrl2/utilities/logger.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

/// \brief Algorithm class for the control_flow algorithm
class pbes_control_flow_algorithm
{
  public:
    struct vertex
    {
      core::identifier_string X;
      data::variable v;

      std::string print() const
      {
        std::ostringstream out;
        out << core::pp(X) << ", " << data::pp(v);
        return out.str();
      }

      vertex(const core::identifier_string& X_, const data::variable& v_)
        : X(X_), v(v_)
      {}
    };

    struct edge
    {
      std::size_t i;
      std::size_t j;
      const vertex* source;
      const vertex* target;

      std::string print() const
      {
        std::ostringstream out;
        out << core::pp(source->X) << " -- " << i << ", " << j << " --> " << core::pp(target->X);
        return out.str();
      }

      edge(std::size_t i_, std::size_t j_, const vertex* source_, const vertex* target_)
        : i(i_), j(j_), source(source_), target(target_)
      {}
    };

    void print_graph() const
    {
      std::cout << "--- vertices ---\n";
      for (std::vector<vertex>::const_iterator i = m_vertices.begin(); i != m_vertices.end(); ++i)
      {
        std::cout << i->print() << std::endl;
      }

      std::cout << "--- edges ---\n";
      for (std::vector<edge>::const_iterator i = m_edges.begin(); i != m_edges.end(); ++i)
      {
        std::cout << i->print() << std::endl;
      }
    }

  protected:
    std::vector<vertex> m_vertices;
    std::vector<edge> m_edges;

    // very inefficient
    std::vector<vertex>::const_iterator find_vertex(const core::identifier_string& X, const data::variable& v) const
    {
      for (std::vector<vertex>::const_iterator i = m_vertices.begin(); i != m_vertices.end(); ++i)
      {
        if (i->X == X && i->v == v)
        {
          return i;
        }
      }
      std::cout << "<error>" << core::pp(X) << " " << data::pp(v) << std::endl;
      return m_vertices.end();
    }

    propositional_variable find_propvar(const pbes<>& p, const core::identifier_string& X) const
    {
      const std::vector<pbes_equation>& equations = p.equations();
      for (std::vector<pbes_equation>::const_iterator i = equations.begin(); i != equations.end(); ++i)
      {
        if (i->variable().name() == X)
        {
          return i->variable();
        }
      }
      throw mcrl2::runtime_error("find_propvar failed!");
      return propositional_variable();
    }

    // extract the propositional variable instantiations from an expression of the form g => \/_j in J . X_j(e_j)
    std::vector<propositional_variable_instantiation> get_propvars(const pbes_expression& x)
    {
      std::vector<pbes_expression> v;
      pbes_expression y = x;
      if (is_imp(y))
      {
        y = imp(y).right();
      }
      split_or(y, v);

      std::vector<propositional_variable_instantiation> result;
      for (std::vector<pbes_expression>::iterator i = v.begin(); i != v.end(); ++i)
      {
        if (is_propositional_variable_instantiation(*i))
        {
          result.push_back(*i);
        }
      }
      return result;
    }

  public:

    /// \brief Runs the control_flow algorithm. The pbes \p is modified by the algorithm
    /// \param p A pbes
    /// \pre p is in PFNF format
    void run(pbes<>& P)
    {
      const std::vector<pbes_equation>& equations = P.equations();

      // compute the vertices of the control graph
      for (std::vector<pbes_equation>::const_iterator i = equations.begin(); i != equations.end(); ++i)
      {
        core::identifier_string X = i->variable().name();
        data::variable_list Xparams = i->variable().parameters();
        for (data::variable_list::const_iterator j = Xparams.begin(); j != Xparams.end(); ++j)
        {
          m_vertices.push_back(vertex(X, *j));
        }
      }

      // compute the edges of the control graph
      for (std::vector<pbes_equation>::const_iterator k = equations.begin(); k != equations.end(); ++k)
      {
        // we are considering the equation X(d_X) = phi
        propositional_variable X = k->variable();
        std::vector<data::variable> d_X(X.parameters().begin(), X.parameters().end());
        pbes_expression phi = k->formula();

        std::vector<pbes_expression> implications = detail::pfnf_implications(phi);
        for (std::size_t i = 0; i < implications.size(); i++)
        {
          std::vector<propositional_variable_instantiation> propvars = get_propvars(implications[i]);
          for (std::size_t j = 0; j < propvars.size(); j++)
          {
            propositional_variable_instantiation Y = propvars[j];
            std::vector<data::data_expression> Yparameters(Y.parameters().begin(), Y.parameters().end());
            propositional_variable Yvar = find_propvar(P, Y.name());
            std::vector<data::variable> d_Y(Yvar.parameters().begin(), Yvar.parameters().end());
            for (std::size_t p = 0; p < Yparameters.size(); p++)
            {
              std::set<data::variable> freevars = pbes_system::find_free_variables(Yparameters[p]);
              for (std::size_t m = 0; m < d_X.size(); m++)
              {
                if (std::find(freevars.begin(), freevars.end(), d_X[m]) != freevars.end())
                {
                  std::vector<vertex>::const_iterator source = find_vertex(X.name(), d_X[m]);
                  std::vector<vertex>::const_iterator target = find_vertex(Y.name(), d_Y[p]);
                  edge e(i, j, &(*source), &(*target));
                  m_edges.push_back(e);
                }
              }
            }
          }
        }
      }
    }
};

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_CONTROL_FLOW_H
