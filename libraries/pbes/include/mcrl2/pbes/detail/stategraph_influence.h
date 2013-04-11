// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/stategraph_influence.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_STATEGRAPH_INFLUENCE_H
#define MCRL2_PBES_DETAIL_STATEGRAPH_INFLUENCE_H

#include <algorithm>
#include <iomanip>
#include <map>
#include <set>
#include <sstream>
#include <vector>
#include "mcrl2/data/replace.h"
#include "mcrl2/data/standard.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/pbes/detail/pfnf_pbes.h"
#include "mcrl2/pbes/detail/is_pfnf.h"
#include "mcrl2/pbes/detail/stategraph_pbes.h"
#include "mcrl2/pbes/detail/stategraph_utility.h"
#include "mcrl2/utilities/logger.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

/// \brief Algorithm class for the stategraph algorithm
class stategraph_influence_graph_algorithm
{
  public:
    stategraph_influence_graph_algorithm(const stategraph_pbes& p)
      : m_pbes(p)
    {}

    // vertex of the influence graph
    struct influence_vertex
    {
      core::identifier_string X;
      data::variable v;

      std::string print() const
      {
        std::ostringstream out;
        out << core::pp(X) << ", " << data::pp(v);
        return out.str();
      }

      influence_vertex(const core::identifier_string& X_, const data::variable& v_)
        : X(X_), v(v_)
      {}
    };

    // influence_edge of the influence graph
    struct influence_edge
    {
      const influence_vertex* source;
      const influence_vertex* target;

      std::string print() const
      {
        std::ostringstream out;
        out << core::pp(source->X) << " ---> " << core::pp(target->X);
        return out.str();
      }

      influence_edge(const influence_vertex* source_, const influence_vertex* target_)
        : source(source_), target(target_)
      {}
    };

    void print_influence_graph() const
    {
      mCRL2log(log::verbose) << "--- influence graph ---\n";
      for (std::vector<influence_vertex>::const_iterator i = m_influence_vertices.begin(); i != m_influence_vertices.end(); ++i)
      {
        mCRL2log(log::verbose) << i->print() << std::endl;
      }
      for (std::vector<influence_edge>::const_iterator i = m_influence_edges.begin(); i != m_influence_edges.end(); ++i)
      {
        mCRL2log(log::verbose) << i->print() << std::endl;
      }
    }

  protected:
    // the pbes that is considered
    const stategraph_pbes& m_pbes;

    // vertices of the influence graph
    std::vector<influence_vertex> m_influence_vertices;

    // edges of the influence graph
    std::vector<influence_edge> m_influence_edges;

    // very inefficient
    std::vector<influence_vertex>::const_iterator find_vertex(const core::identifier_string& X, const data::variable& v) const
    {
      for (std::vector<influence_vertex>::const_iterator i = m_influence_vertices.begin(); i != m_influence_vertices.end(); ++i)
      {
        if (i->X == X && i->v == v)
        {
          return i;
        }
      }
      return m_influence_vertices.end();
    }

    void compute_influence_graph()
    {
      // compute the vertices
      const std::vector<stategraph_equation>& equations = m_pbes.equations();
      for (std::vector<stategraph_equation>::const_iterator i = equations.begin(); i != equations.end(); ++i)
      {
        core::identifier_string X = i->variable().name();
        const std::vector<data::variable>& Xparams = i->parameters();
        for (std::vector<data::variable>::const_iterator j = Xparams.begin(); j != Xparams.end(); ++j)
        {
          m_influence_vertices.push_back(influence_vertex(X, *j));
        }
      }

      // compute the edges
      for (std::vector<stategraph_equation>::const_iterator k = equations.begin(); k != equations.end(); ++k)
      {
        const std::vector<data::variable>& d_X = k->parameters();
        const core::identifier_string Xname = k->variable().name();
        const std::vector<predicate_variable>& predvars = k->predicate_variables();
        for (std::vector<predicate_variable>::const_iterator i = predvars.begin(); i != predvars.end(); ++i)
        {
          const propositional_variable_instantiation& Y = i->X;
          std::vector<data::data_expression> Yparameters(Y.parameters().begin(), Y.parameters().end());
          stategraph_equation eqn = *find_equation(m_pbes, Y.name());
          const std::vector<data::variable> d_Y = eqn.parameters();
          for (std::size_t p = 0; p < Yparameters.size(); p++)
          {
            std::set<data::variable> freevars = data::find_free_variables(Yparameters[p]);
            for (std::size_t m = 0; m < d_X.size(); m++)
            {
              if (std::find(freevars.begin(), freevars.end(), d_X[m]) != freevars.end())
              {
                std::vector<influence_vertex>::const_iterator source = find_vertex(Xname, d_X[m]);
                std::vector<influence_vertex>::const_iterator target = find_vertex(Y.name(), d_Y[p]);
                influence_edge e(&(*source), &(*target));
                m_influence_edges.push_back(e);
              }
            }
          }
        }
      }
    }

  public:

    void run()
    {
      compute_influence_graph();
      print_influence_graph();
    }
  };

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_STATEGRAPH_INFLUENCE_H
